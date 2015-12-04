#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"

#include	"PlayerManager.h"

#include	"../sound/SoundManager.h"

#include	"../blur/blur.h"

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

	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.motion_no = 0;
	m_controlDesc.mouseX = .0f;
	m_controlDesc.mouseY = .0f;
	m_controlDesc.rendFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;
	m_controlDesc.skillFlag = 0x0;

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
	toggle_c = true;

	// 3D実体
	models[(int)MODEL::NORMAL]	 = objs[(int)PlayerManager::CLONE_TYPE::NORMAL]->Clone();
	models[(int)MODEL::DIE]		 = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::PLANE]	 = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::GUN]		 = objs[(int)PlayerManager::CLONE_TYPE::GUN]->Clone();

	skill_data[(int)SKILL::GUN].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::SYURIKEN].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::KABUTO].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::ZENRYOKU].do_action = ACTION_PART::GUN;

	// 絶対低い順に並べる
	// 案ロックカウント
	skill_data[(int)SKILL::GUN].unlock_rend_count = 0;
	skill_data[(int)SKILL::SYURIKEN].unlock_rend_count = 5;
	skill_data[(int)SKILL::KABUTO].unlock_rend_count = 10;
	skill_data[(int)SKILL::ZENRYOKU].unlock_rend_count = 15;

	// クールタイム
	skill_data[(int)SKILL::GUN].cool_time = 600;
	skill_data[(int)SKILL::SYURIKEN].cool_time = 300;
	skill_data[(int)SKILL::KABUTO].cool_time = 150;
	skill_data[(int)SKILL::ZENRYOKU].cool_time = 600;

	// 全部ロックをかける
	for (int i = 0; i < (int)SKILL::MAX; i++)
	{
		skill_data[i].unlock = false;
		skill_data[i].wait_time = 0;
	}
	skill_data[(int)SKILL::GUN].unlock = true; // 最初のスキルは最初から使える

	select_skill = SKILL::GUN;


	// 行動状態初期化
	action[(int)ACTION_PART::MOVE]		 = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_TARGET]= new BasePlayer::Action::MoveTarget(this);
	action[(int)ACTION_PART::ATTACK]	 = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE]		 = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND]		 = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::FREEZE]	 = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::DIE]		 = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::RESPAWN]	 = new BasePlayer::Action::Respawn(this);
	action[(int)ACTION_PART::PLANE]		 = new BasePlayer::Action::Hikouki(this);
	action[(int)ACTION_PART::GUN]		 = new BasePlayer::Action::Gun(this);
	action[(int)ACTION_PART::MANHOLE] = new BasePlayer::Action::Manhole(this);
	action[(int)ACTION_PART::THROUGH] = new BasePlayer::Action::Through(this);

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

	if (KEY(KEY_B)==3)
	{
		ExplosionAction();
		EffectFireFlour(pos + Get_Flont(), FIRE_COLOR::BLUE, 3);
	}

	// スキルゲージ更新
	for (int i = 0; i < (int)SKILL::MAX; i++)
	{
		if (!skill_data[i].unlock)break;
		(skill_data[i].wait_time > 0) ? skill_data[i].wait_time-- : skill_data[i].wait_time &= 0x00000000;
	}
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
	if (models[(int)MODEL::NORMAL]->GetMotion() != no)
	{
		models[(int)MODEL::NORMAL]->SetMotion(no);
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

	// 待機モーションセット
	me->Set_motion(1);

	me->model_part = MODEL::NORMAL;

}

void BasePlayer::Action::Move::Update()
{
	// 扱いに注意
	me->Set_motion(me->motion_no);

	// 無理やり
	if (me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::DOWN ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::UP ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::LEFT ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::RIGHT)
	{
		if (me->se_receive == -1) me->se_receive = se->Play("歩行", me->pos, Vector3(me->models[(int)me->model_part]->TransMatrix._31, 0, me->models[(int)me->model_part]->TransMatrix._32), true);
		else se->Set_data(me->se_receive, me->pos, Vector3(me->models[(int)me->model_part]->TransMatrix._31, 0, me->models[(int)me->model_part]->TransMatrix._32), me->move);
	}
	else if (me->se_receive != -1)
	{
		se->Stop(me->se_receive);
		me->se_receive = -1;
	}

	if (me->models[(int)MODEL::NORMAL]->GetFrame() == 265) se->Play("ジャンプ", me->pos);


	Update_obj();
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

	// 待機モーションセット
	me->Set_motion(1);

	me->model_part = MODEL::NORMAL;
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

	me->Set_motion(4);

	me->model_part = MODEL::NORMAL;
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
//		「ポスター貼り付け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->Set_motion(3);

	me->model_part = MODEL::NORMAL;
}

void BasePlayer::Action::Paste::Update()
{
	Update_obj();
}

void BasePlayer::Action::Paste::Render(iexShader *shader, char *name)
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

	se->Play("破る構え");
}

void BasePlayer::Action::Rend::Update()
{
	me->m_controlDesc.rendFlag &= 0x00000000;

	// 扱いに注意
	me->Set_motion(me->motion_no);

	// マウススラッシュ
	const float mouse_move = sqrtf(me->m_controlDesc.mouseX*me->m_controlDesc.mouseX + me->m_controlDesc.mouseY + me->m_controlDesc.mouseY);
	if (mouse_move > 80000)
	{
		me->m_controlDesc.rendFlag |= (BYTE)REND_FLAG::RIGHT;
	}

	// 破くモーションのフレーム
	if (me->models[(int)me->model_part]->GetParam(0) == 1)
	{
		// 破く処理
		if (me->se_receive == -1)
		{
			me->se_receive = se->Play("破る", me->pos);
		}
	}

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
}

void BasePlayer::Action::Freeze::Render(iexShader *shader, char *name)
{
	//me->models[(int)me->model_part]->Animation();	// アニメーションしない
	me->models[(int)me->model_part]->SetScale(me->scale);
	me->models[(int)me->model_part]->SetAngle(me->angleY);
	me->models[(int)me->model_part]->SetPos(me->pos);
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
//		「紙ヒコーキ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Hikouki::Initialize()
{
	me->model_part = MODEL::PLANE;
}

void BasePlayer::Action::Hikouki::Update()
{
	Update_obj();
}

void BasePlayer::Action::Hikouki::Render(iexShader *shader, char *name)
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
//		「紙鉄砲」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Gun::Initialize()
{
	// 入力情報初期化
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

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
		//se->Play("紙鉄砲");
		me->ExplosionAction();
		EffectFireFlour(me->pos+me->Get_Flont(), FIRE_COLOR::BLUE, 3);

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
	me->models[(int)me->model_part]->SetMotion(0);
}

void BasePlayer::Action::Manhole::Update()
{
	//if (me->models[(int)me->model_part]->GetParam(0) == 1)
	//{
	//se->Play("紙鉄砲");
	//}
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
	me->models[(int)me->model_part]->SetMotion(0);
}

void BasePlayer::Action::Through::Update()
{
	//if (me->models[(int)me->model_part]->GetParam(0) == 1)
	//{
	//se->Play("紙鉄砲");
	//}
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


//　実態
//BasePlayer* player[PLAYER_MAX];