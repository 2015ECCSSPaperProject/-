#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"../SkillBegin/SkillBegin.h"
#include	"../../../share_data/Enum_public.h"
#include	"../Barrier/Barrier.h"
#include	"../Rush/Rush.h"
#include	"PlayerManager.h"
#include	"../sound/SoundManager.h"
#include	"../blur/blur.h"
#include	"../Effect/Effect.h"
#include	"../paper object/paper object manager.h"
#include	"../Manhole/Manhole.h"
#include	"../Effect/Effect.h"
/*	ベースプレイヤー	*/

//****************************************************************************************************************
//
//  初期化
//
//****************************************************************************************************************
BasePlayer::BasePlayer():prev_pos(pos)
{
	//ZeroMemory(models, sizeof(iex3DObj));
	for (int i = 0; i < (int)MODEL::MAX; i++) models[i] = nullptr;

	m_controlDesc.moveFlag &= 0x00;
	m_controlDesc.motion_no = 0;
	m_controlDesc.mouseX = .0f;
	m_controlDesc.mouseY = .0f;
	m_controlDesc.rendFlag &= 0x00;
	m_controlDesc.controlFlag &= 0x00;
	m_controlDesc.skillFlag &= 0x00;
	stateFlag &= 0x00;

	// エフェクト初期化
	EffectInit();
}

BasePlayer::~BasePlayer()
{
	Release();
	EffectRelease();
}

void BasePlayer::Initialize(iex3DObj **objs)
{
	// 基本パラメータ初期化
	pos = Vector3(0, 0, 0);
	angleY = 0.0f;
	scale = .75f;
	move = Vector3(0, 0, 0);
	speed = 5.0f;
	fallspeed = .0f;
	se_receive = 0;

	// その他初期化
	poster_num = 0;
	isManhole = false;
	manhole_no_haninai = false;
	isMyNunber = false;
	kind_paper_obj = -1;
	barrier = new Barrier;
	skill_begin = new SkillBegin;
	rush = new Rush;
	isBarrier = false;
	// 3D実体
	models[(int)MODEL::NORMAL]	 = objs[(int)PlayerManager::CLONE_TYPE::NORMAL]->Clone();
	models[(int)MODEL::DIE]		 = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::GUN]		 = objs[(int)PlayerManager::CLONE_TYPE::GUN]->Clone();
	models[(int)MODEL::SYURIKEN] = objs[(int)PlayerManager::CLONE_TYPE::SYURIKEN]->Clone();
	models[(int)MODEL::REND_CALENDAR] = objs[(int)PlayerManager::CLONE_TYPE::REND_CALENDAR]->Clone(2);
	models[(int)MODEL::REND_MAGAZINE] = objs[(int)PlayerManager::CLONE_TYPE::REND_MAGAZINE]->Clone(2);
	models[(int)MODEL::REND_MONEY] = objs[(int)PlayerManager::CLONE_TYPE::REND_MONEY]->Clone(2);
	models[(int)MODEL::REND_SHINBUN] = objs[(int)PlayerManager::CLONE_TYPE::REND_SHINBUN]->Clone(2);
	models[(int)MODEL::REND_SIGN] = objs[(int)PlayerManager::CLONE_TYPE::REND_SIGN]->Clone(2);
	models[(int)MODEL::REND_WC_PAPER] = objs[(int)PlayerManager::CLONE_TYPE::REND_WC_PAPER]->Clone(3);
	models[(int)MODEL::REND_ZASSHI] = objs[(int)PlayerManager::CLONE_TYPE::REND_ZASSHI]->Clone(2);
	models[(int)MODEL::REND_SEISHO] = objs[(int)PlayerManager::CLONE_TYPE::REND_SEISHO]->Clone(2);
	models[(int)MODEL::REND_SHOJI] = objs[(int)PlayerManager::CLONE_TYPE::REND_SHOJI]->Clone(2);

	skill_data[(int)SKILL::GUN].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::SYURIKEN].do_action = ACTION_PART::SYURIKEN;
	skill_data[(int)SKILL::KABUTO].do_action = ACTION_PART::GUN;

	// 絶対低い順に並べる
	// 案ロックカウント
	skill_data[(int)SKILL::GUN].unlock_rend_count = 0;
	skill_data[(int)SKILL::SYURIKEN].unlock_rend_count = 1;
	skill_data[(int)SKILL::KABUTO].unlock_rend_count = 30;

	// クールタイム
	skill_data[(int)SKILL::GUN].cool_time = 600;
	skill_data[(int)SKILL::SYURIKEN].cool_time = 300;
	skill_data[(int)SKILL::KABUTO].cool_time = 900;

	// 全部ロックをかける
	for (int i = 0; i < (int)SKILL::MAX; i++)
	{
		skill_data[i].unlock = false;
		skill_data[i].wait_time = 0;
	}
	skill_data[(int)SKILL::GUN].unlock = true; // 最初のスキルは最初から使える

	select_skill = (int)SKILL::GUN;


	// 行動状態初期化
	action[(int)ACTION_PART::MOVE]		 = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_TARGET]= new BasePlayer::Action::MoveTarget(this);
	action[(int)ACTION_PART::ATTACK]	 = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::REND]		 = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::FREEZE]	 = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::DIE]		 = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::RESPAWN]	 = new BasePlayer::Action::Respawn(this);
	action[(int)ACTION_PART::GUN]		 = new BasePlayer::Action::Gun(this);
	action[(int)ACTION_PART::MANHOLE] = new BasePlayer::Action::Manhole(this);
	action[(int)ACTION_PART::THROUGH] = new BasePlayer::Action::Through(this);
	action[(int)ACTION_PART::SYURIKEN] = new BasePlayer::Action::Syuriken(this);
	action[(int)ACTION_PART::TRANS_FORM] = new BasePlayer::Action::TransForm(this);
	action[(int)ACTION_PART::REND_OBJ] = new BasePlayer::Action::RendObj(this);

	Change_action(ACTION_PART::MOVE);	// 最初は移動状態

	//	キー初期化
	GetCursorPos(&mousePos);

	// モーション番号
	motion_no = 0;
}

void BasePlayer::Release()
{
	for (int i = 0; i < (int)MODEL::MAX; i++) SAFE_DELETE(models[i]);

	for (int i = 0; i < (int)ACTION_PART::MAX; i++)
	{
		SAFE_DELETE(action[i]);
	}
	delete barrier;
	delete rush;
	delete skill_begin;
}




//*************************************************************************************************************************
//		更新
//*************************************************************************************************************************

void BasePlayer::Update()
{
	action[(int)action_part]->Update();

	// move値算出
	move = Vector3(pos - prev_pos);
	prev_pos = pos;

	// エフェクト更新
	EffectUpdate();

	//if (KEY(KEY_B)==3)
	//{
	//	ExplosionAction();
	//	EffectFireFlour(pos + Get_Flont(), FIRE_COLOR::BLUE, 3);
	//}

	// スキルゲージ更新
	for (int i = 0; i < (int)SKILL::MAX; i++)
	{
		if (!skill_data[i].unlock)break;
		if (skill_data[i].do_action == action_part || action_part == ACTION_PART::TRANS_FORM) continue;
		(skill_data[i].wait_time > 0) ? skill_data[i].wait_time-- : skill_data[i].wait_time &= 0x00000000;
	}

	if (stateFlag & (int)PLAYER_FLAG::BARRIER)
	{
		if (isBarrier) barrier->Update(pos+Vector3(0,8,0), Vector3(matView._13, matView._23, matView._33), .1f);
		else
		{
			isBarrier = true;
			barrier->Action();
		}
	}
	else
	{
		isBarrier = false;
		if (isBarrier) barrier->Stop();
	}
	rush->Update(pos, Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33), 25.0f, Vector3(0, angleY, 0));
	skill_begin->Update(pos);
}


//*************************************************************************************************************************
//		描画
//*************************************************************************************************************************
void BasePlayer::Render(iexShader *shader, char *name)
{
	if (shader)
	{
		action[(unsigned int)action_part]->Render(shader, name);
	}
	else
	{
		action[(unsigned int)action_part]->Render();
	}
}

void BasePlayer::Render_forword()
{
	if (stateFlag & (int)PLAYER_FLAG::BARRIER)
	{
		barrier->Render();
	}
	skill_begin->Render();
}

void BasePlayer::Render_rush()
{
	rush->Render();
}

//*************************************************************************************************************************
//		エフェクト色々
//*************************************************************************************************************************
void   BasePlayer::EffectInit()
{
	explosion = new Explosion();

}

void   BasePlayer::EffectRelease()
{
	SAFE_DELETE(explosion);

}

void   BasePlayer::EffectUpdate()
{
	explosion->Update(pos, Get_Flont(), 30);

}

void  BasePlayer::EffectRender()
{
	explosion->Render();

}

/* トリガー */
void BasePlayer::ExplosionAction()
{
	explosion->Action();
	BlurFilter::Set(18, 0, 0);
}

//*****************************************************************************
//
//		状態処理ベース関係
//
//*****************************************************************************
void BasePlayer::Action::Base::Update_obj()
{
	//me->pos += me->move;
	//me->move = Vector3(0, 0, 0);

	me->models[(int)me->model_part]->Animation();
	me->models[(int)me->model_part]->SetScale(me->scale);
	me->models[(int)me->model_part]->SetAngle(me->angleY);
	me->models[(int)me->model_part]->SetPos(me->pos);
}

void BasePlayer::Set_motion(int no)
{
	if (models[(int)model_part]->GetMotion() != no)
	{
		models[(int)model_part]->SetMotion(no);
	}
}

void BasePlayer::Set_action(ACTION_PART part)
{
	if (action_part != part)
	{
		if (part == ACTION_PART::TRANS_FORM)
		{
			skill_begin->Action();
		}
		if (action_part == ACTION_PART::MANHOLE && part == ACTION_PART::MOVE)
		{
			isManhole ^= 1;
		}
		if (action_part == ACTION_PART::SYURIKEN && part == ACTION_PART::MOVE)
		{
			// 手裏剣エフェクトストップ
			rush->Stop();
			Sand_effect(pos.x, pos.y, pos.z);
		}
		Change_action(part);
	}
}


//*****************************************************************************
//
//		「TPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{
	me->m_controlDesc.mouseX = .0f;
	me->m_controlDesc.mouseY = .0f;

	me->model_part = MODEL::NORMAL;

	// 待機モーションセット
	me->Set_motion(1);

	me->Check_unlock(me->god_gage);

	me->se_receive = me->se_receive2 = -1;
	me->kind_paper_obj = -1;
}

void BasePlayer::Action::Move::Update()
{
	// 扱いに注意
	me->Set_motion(me->motion_no);
	
	// 無理やり
	//if (me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::DOWN ||
	//	me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::UP ||
	//	me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::LEFT ||
	//	me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::RIGHT)
	//{
	//	if (me->se_receive == -1) me->se_receive = se->Play("歩行", me->pos, Vector3(me->models[(int)me->model_part]->TransMatrix._31, 0, me->models[(int)me->model_part]->TransMatrix._32), true);
	//	else se->Set_data(me->se_receive, me->pos, Vector3(me->models[(int)me->model_part]->TransMatrix._31, 0, me->models[(int)me->model_part]->TransMatrix._32), me->move);
	//}
	//else if (me->se_receive != -1)
	//{
	//	se->Stop(me->se_receive);
	//	me->se_receive = -1;
	//}
	if (me->models[(int)MODEL::NORMAL]->GetParam(1) == 1)
	{
		if(me->se_receive==-1)me->se_receive=se->Play("歩行1", me->pos);
	}
	else if (me->models[(int)MODEL::NORMAL]->GetParam(1) == 2)
	{
		if(me->se_receive2==-1)me->se_receive2=se->Play("歩行2", me->pos);
	}
	else if (me->models[(int)MODEL::NORMAL]->GetParam(1) == 0)
	{
		me->se_receive = me->se_receive2 = -1;
	}
	if (me->se_receive != -1) se->Set_pos("歩行1", me->se_receive, me->pos);
	if (me->se_receive2 != -1) se->Set_pos("歩行2", me->se_receive2, me->pos);

	if (me->models[(int)MODEL::NORMAL]->GetFrame() == 265) se->Play("ジャンプ", me->pos);


	//if (KEY(KEY_ENTER) == 3)
	//{
	//	me->model_part = MODEL::REND_MONEY;
	//	me->Set_motion(0);
	//}
	Update_obj();

	// マンホール範囲内
	me->manhole_no_haninai = manhole_mng->CheckManhole((me->isManhole) ? ManholeMng::LAND_TYPE::TIKA : ManholeMng::LAND_TYPE::TIJOU, me->pos, 8, &me->next_manhole_pos);
}

void BasePlayer::Action::Move::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		「ターゲット移動移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::MoveTarget::Initialize()
{
	me->m_controlDesc.mouseX = .0f;
	me->m_controlDesc.mouseY = .0f;

	me->model_part = MODEL::NORMAL;

	// 待機モーションセット
	me->Set_motion(1);
}

void BasePlayer::Action::MoveTarget::Update()
{
	// 扱いに注意
	me->Set_motion(me->motion_no);

	Update_obj();
}

void BasePlayer::Action::MoveTarget::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		「攻撃」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->model_part = MODEL::NORMAL;

	(sqrtf(me->move.y*me->move.y) >= .1f) ? me->Set_motion(21) : me->Set_motion(4);
}

void BasePlayer::Action::Attack::Update()
{
	Update_obj();
}

void BasePlayer::Action::Attack::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}

//*****************************************************************************
//
//		「ポスター破る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000; 
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->se_receive = -1;
	me->model_part = MODEL::NORMAL;

	//se->Play("破る構え");
}

void BasePlayer::Action::Rend::Update()
{
	me->m_controlDesc.rendFlag &= 0x00000000;

	// 扱いに注意
	me->Set_motion(me->motion_no);

	// 破くモーションのフレーム

	Update_obj();
}

void BasePlayer::Action::Rend::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}

//*****************************************************************************
//
//		「固まる」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Freeze::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;
}

void BasePlayer::Action::Freeze::Update()
{
	//me->models[(int)me->model_part]->Animation();	// アニメーションしない
	me->models[(int)me->model_part]->SetScale(me->scale);
	me->models[(int)me->model_part]->SetAngle(me->angleY);
	me->models[(int)me->model_part]->SetPos(me->pos);
}

void BasePlayer::Action::Freeze::Render(iexShader *shader, char *name)
{

	me->models[(int)me->model_part]->Update();

	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}

//*****************************************************************************
//
//		「死ぬ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	flashing = 0;

	me->model_part = MODEL::DIE;

	// ばぐるかも
	me->models[(int)me->model_part]->SetFrame(0);
}

void BasePlayer::Action::Die::Update()
{
	Update_obj();
}

void BasePlayer::Action::Die::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	flashing++;
	if (flashing < 4) {
		if (shader)
		{
			me->models[(int)me->model_part]->Render(shader, name);
		}
		else
		{
			me->models[(int)me->model_part]->Render();
		}
	}
	else if (flashing > 8){
		flashing = 0;
	}

}


//*****************************************************************************
//
//		「リスポーン」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Respawn::Initialize()
{
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	flashing = 0;

	me->model_part = MODEL::NORMAL;

	me->Set_motion(8);
}

void BasePlayer::Action::Respawn::Update()
{
	// 扱いに注意
	me->Set_motion(me->motion_no);

	Update_obj();
}

void BasePlayer::Action::Respawn::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	flashing++;
	if (flashing < 4) {
		if (shader)
		{
			me->models[(int)me->model_part]->Render(shader, name);
		}
		else
		{
			me->models[(int)me->model_part]->Render();
		}
	}
	else if (flashing > 8){
		flashing = 0;
	}
}


//*****************************************************************************
//
//		「紙鉄砲」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Gun::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->se_receive = -1;

	// モデルを鉄砲に変更
	me->model_part = MODEL::GUN;

	// 0フレームにリセット
	me->models[(int)me->model_part]->SetFrame(0);
	me->models[(int)me->model_part]->SetParam(0, 0);
}

void BasePlayer::Action::Gun::Update()
{
	if (me->models[(int)me->model_part]->GetParam(0) == 1)
	{
		if (me->se_receive == -1){
			me->se_receive = se->Play("紙鉄砲", me->pos);
			me->ExplosionAction();
			EffectFireFlour(me->pos + me->Get_Flont(), FIRE_COLOR::BLUE, 3);
		}
	}
	Update_obj();
}

void BasePlayer::Action::Gun::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		「マンホール潜る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Manhole::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->model_part = MODEL::NORMAL;
	me->models[(int)me->model_part]->SetMotion((me->isManhole)?22:18);

	me->se_receive = -1;
}

void BasePlayer::Action::Manhole::Update()
{
	Update_obj();
}

void BasePlayer::Action::Manhole::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		「狭い壁通り抜け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Through::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->model_part = MODEL::NORMAL;
	me->models[(int)me->model_part]->SetMotion(14);
}

void BasePlayer::Action::Through::Update()
{
	Update_obj();
}

void BasePlayer::Action::Through::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	{
		me->models[(int)me->model_part]->Render();
	}
}


//*****************************************************************************
//
//		「手裏剣」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Syuriken::Initialize()
{
	me->model_part = MODEL::SYURIKEN;
	me->Set_motion(1);
	trg = false;

	BlurFilter::Set(18, 0, 0);
	me->se_receive = se->Play("手裏剣", me->pos);
	me->rush->Action();
}

void BasePlayer::Action::Syuriken::Update()
{
	if (!trg)
	{
		if (!(me->m_controlDesc.controlFlag & (int)PLAYER_CONTROL::RIGHT_CLICK))
			trg = true;
		me->m_controlDesc.controlFlag &= (0xff ^ (int)PLAYER_CONTROL::RIGHT_CLICK);
	}
	if (se->isPlay("手裏剣", me->se_receive))se->Set_pos("手裏剣", me->se_receive, me->pos);
	//Update_obj();

	if (me->move.LengthSq() < 1)
	{
		BlurFilter::Set(8, 0, 0);
	}
	else
	{
		me->models[(int)me->model_part]->Animation();
	}
	me->models[(int)me->model_part]->SetScale(me->scale);
	me->models[(int)me->model_part]->SetAngle(me->angleY);
	me->models[(int)me->model_part]->SetPos(me->pos);
}

void BasePlayer::Action::Syuriken::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	{
		me->models[(int)me->model_part]->Render();
	}
}

//*****************************************************************************
//
//		「変身」状態処理
//
//*****************************************************************************

void BasePlayer::Action::TransForm::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->model_part = MODEL::NORMAL;
	me->Set_motion(12);
}

void BasePlayer::Action::TransForm::Update()
{
	Update_obj();
}

void BasePlayer::Action::TransForm::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	{
		me->models[(int)me->model_part]->Render();
	}
}


//*****************************************************************************
//
//		「対小物破り」状態処理
//
//*****************************************************************************

void BasePlayer::Action::RendObj::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	// SE受け取り情報初期化
	me->se_receive = me->se_receive2 = -1;

	// 破りまくったらバグってしまう原因を直す
	if (me->poster_num == -1)
	{
		me->Change_action(ACTION_PART::MOVE);
		return;
	}
	me->kind_paper_obj = (int)paper_obj_mng->Get_kind(me->poster_num);	// poster_numが-1になる時がたまにあり、配列参照外で落ちる

	switch ((KIND_PAPER_OBJECT)me->kind_paper_obj)
	{
	case KIND_PAPER_OBJECT::CALENDAR:
		me->model_part = MODEL::REND_CALENDAR;
		break;

	case KIND_PAPER_OBJECT::MAGAZIN:
		me->model_part = MODEL::REND_MAGAZINE;
		break;

	case KIND_PAPER_OBJECT::MONEY:
		me->model_part = MODEL::REND_MONEY;
		break;

	case KIND_PAPER_OBJECT::SEISHO:
		me->model_part = MODEL::REND_SEISHO;
		break;

	case KIND_PAPER_OBJECT::SHINBUN:
		me->model_part = MODEL::REND_SHINBUN;
		break;

	case KIND_PAPER_OBJECT::SIGN:
		me->model_part = MODEL::REND_SIGN;
		break;

	case KIND_PAPER_OBJECT::TOILET_PAPER:
		me->model_part = MODEL::REND_WC_PAPER;
		break;

	case KIND_PAPER_OBJECT::ZASSHI:
		me->model_part = MODEL::REND_ZASSHI;
		break;

	case KIND_PAPER_OBJECT::SHOJI:
		me->model_part = MODEL::REND_SHOJI;
		break;

	case KIND_PAPER_OBJECT::POSTER:
	case KIND_PAPER_OBJECT::FLYER:
		me->model_part = MODEL::NORMAL;
		me->Set_motion((me->motion_no == 5 || me->motion_no == 21) ? 21 : 2);
		break;
	}

	// 差し替えモデルの場合
	if (me->model_part != MODEL::NORMAL)
	{
		me->models[(int)me->model_part]->SetFrame(0);
		me->models[(int)me->model_part]->SetParam(0, 0);
		me->models[(int)me->model_part]->SetParam(5, 0);
		me->Set_motion(0);
	}

	me->m_controlDesc.motion_no = (int)PLAYER_CONTROL::RENDING;
}

void BasePlayer::Action::RendObj::Update()
{
	Update_obj();
	if (me->models[(int)me->model_part]->GetParam(0) == 2)
	{
		me->m_controlDesc.motion_no = 0;
		//me->Change_action(ACTION_PART::MOVE);
	}
	if (me->models[(int)me->model_part]->GetMotion() == 21)
	{
		me->models[(int)me->model_part]->Animation();
	}
}

void BasePlayer::Action::RendObj::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	{
		me->models[(int)me->model_part]->Render();
	}
}

//　実態
//BasePlayer* player[PLAYER_MAX];