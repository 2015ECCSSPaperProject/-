#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"

#include	"../paper object/paper object manager.h"

#include	"../Net/PaperServer.h"
#include	"../Player/PlayerManager.h"

#include	"../score/Score.h"
#include	"../timer/Timer.h"
#include "../paperQueue/paperQueue.h"
#include	"../Manhole/Manhole.h"

/*	ベースプレイヤー	*/

const float CAN_TARGET_DIST = 40.0f;
const bool auto_target = false;
const bool thrash_rend = true;
static const float JUMP_POW = 1.8f;

//****************************************************************************************************************
//
//				 初	期	化	と	解	放	
//
//****************************************************************************************************************
BasePlayer::BasePlayer(int id) :m_id(id)
{
	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;
}

void BasePlayer::Init_pos()
{
	// 基本パラメータ初期化
	angleY = PI / 3 * m_id;
	pos.z = sinf( angleY ) * 100;
	pos.y = 1;
	pos.x = cosf( angleY ) * 100;// 仮
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 1.5f;
	fallspeed = .1f;
	se_receive = 0;
	isJump = isLand = attackFlag = false;
	jump_pow = 0;
	invincible = false;
	kabuto_timer = 0;
	god_gage = 0;
	mynumber = m_id;
	isManhole = false;
	stateFlag &= 0x00;
	// モーション番号
	motion_no = 0;

	//for (int i = 0; i < (int)SKILL::MAX; i++)
	//{
	//	skill_data[i].unlock = false;
	//	skill_data[i].wait_time = 0;
	//}

	select_skill = SKILL::GUN;

	if (action[0] != nullptr)
	{
		Change_action(ACTION_PART::MOVE);	// 最初は移動状態
	}
}

void BasePlayer::Initialize(iex3DObj **objs)
{
	paperqueue = new PaperQueue;

	// 基本パラメータ初期化
	angleY = PI / 3 * m_id;
	pos.z = cosf( angleY ) * 100;
	pos.x = sinf( angleY ) * 100;// 仮
	scale = .5f;
	move = huttobi=Vector3(0, 0, 0);
	speed = 1.5f;
	fallspeed = .075f;
	se_receive = 0;
	isJump = isLand = attackFlag = false;
	jump_pow = 0;
	invincible = false;
	god_gage = 0;
	isManhole = false;
	push_rend = false;
	stateFlag &= 0x00;
	kabuto_timer = 0;

	skill_data[(int)SKILL::GUN].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::SYURIKEN].do_action = ACTION_PART::SYURIKEN;
	skill_data[(int)SKILL::KABUTO].do_action = ACTION_PART::GUN;

	// 絶対低い順に並べる
	//skill_data[(int)SKILL::GUN].unlock_rend_count = 0;
	//skill_data[(int)SKILL::SYURIKEN].unlock_rend_count = 5;
	//skill_data[(int)SKILL::KABUTO].unlock_rend_count = 10;
	//skill_data[(int)SKILL::ZENRYOKU].unlock_rend_count = 15;

	//skill_data[(int)SKILL::GUN].cool_time = 600;
	//skill_data[(int)SKILL::SYURIKEN].cool_time = 300;
	//skill_data[(int)SKILL::KABUTO].cool_time = 150;
	//skill_data[(int)SKILL::ZENRYOKU].cool_time = 600;

	//for (int i = 0; i < (int)SKILL::MAX; i++)
	//{
	//	skill_data[i].unlock = false;
	//	skill_data[i].wait_time = 0;
	//}

	select_skill = SKILL::GUN;

	// 3D実体
	models[(int)MODEL::NORMAL] = objs[(int)PlayerManager::CLONE_TYPE::NORMAL]->Clone();
	models[(int)MODEL::DIE] = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::PLANE] = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::GUN] = objs[(int)PlayerManager::CLONE_TYPE::GUN]->Clone();
	models[(int)MODEL::SYURIKEN] = objs[(int)PlayerManager::CLONE_TYPE::SYURIKEN]->Clone();

	mynumber = m_id;

	// 行動状態初期化
	action[(int)ACTION_PART::MOVE] = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_TARGET] = new BasePlayer::Action::MoveTarget(this);
	action[(int)ACTION_PART::ATTACK] = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::REND] = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::FREEZE] = new BasePlayer::Action::Freeze(this);
	action[(int)ACTION_PART::DIE] = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::RESPAWN] = new BasePlayer::Action::Respawn(this);
	action[(int)ACTION_PART::GUN] = new BasePlayer::Action::Gun(this);
	action[(int)ACTION_PART::MANHOLE] = new BasePlayer::Action::Manhole(this);
	action[(int)ACTION_PART::THROUGH] = new BasePlayer::Action::Through(this);
	action[(int)ACTION_PART::SYURIKEN] = new BasePlayer::Action::Syuriken(this);
	action[(int)ACTION_PART::TRANS_FORM] = new BasePlayer::Action::TransForm(this);
	action[(int)ACTION_PART::REND_OBJ] = new BasePlayer::Action::RendObj(this);

	Change_action(ACTION_PART::MOVE);	// 最初は移動状態

	// モーション番号
	motion_no = 0;
}

BasePlayer::~BasePlayer()
{
	Release();
}

void BasePlayer::Release()
{
	SAFE_DELETE( paperqueue );
	for (int i = 0; i < (int)ACTION_PART::MAX; i++)
	{
		SAFE_DELETE(action[i]);
	}
	for (int i = 0; i < (int)MODEL::MAX; i++) SAFE_DELETE(models[i]);
}


//*************************************************************************************************************************
//		更新
//*************************************************************************************************************************
void BasePlayer::Update()
{
	CONTROL_DESC controlDesc;

	// ここにサーバーマネージャーからデスクを取ってくる
	controlDesc.moveFlag = ServerManager::GetDesc(m_id).moveFlag;

	controlDesc.mouseX = ServerManager::GetDesc(m_id).mouseX;//マウスX
	controlDesc.mouseY = ServerManager::GetDesc(m_id).mouseY;//マウスY


	// クライアント側で再生しているモーションの番号(無理やり何かに使う)
	controlDesc.motion_no = ServerManager::GetDesc(m_id).motion_no;//モーション番号

	// 破くフラグ
	controlDesc.rendFlag = ServerManager::GetDesc(m_id).rendFlag;

	// ぼたん
	controlDesc.controlFlag = ServerManager::GetDesc(m_id).controlFlag;

	// 必殺技No
	controlDesc.skillFlag = ServerManager::GetDesc(m_id).skillFlag;

	// 救急車衝突
	if (controlDesc.controlFlag&(BYTE)PLAYER_CONTROL::AMBULANCE && action_part != ACTION_PART::DIE)
	{
		jump_pow = JUMP_POW + move.LengthSq() * .4f;
		huttobi = move * -2.1f;
		isJump = true;
		isLand = false;
		//assert(huttobi.Length() != 0);
		Change_action(ACTION_PART::DIE);
	}

	// 兜の無敵時間
	if (kabuto_timer & 0xffff)
	{
		invincible = true;
		if (--kabuto_timer <= 0)
		{
			kabuto_timer = 0;
			invincible = false;
			stateFlag &= (0xff ^ (int)PLAYER_FLAG::BARRIER);
		}
	}

	// アップデート！！
	action[(int)action_part]->Update(controlDesc);


	// 壁判定
	if (action_part == ACTION_PART::SYURIKEN)
	{
		Collision_syuriken();	// 手裏剣専用壁判定
	}
	else
	{
		Vector3 n;	// 受け取るけど使わない
		stage->Collision(pos, &move, 5.0f, 2, &n);
	}

	if (stage->Collision_rand(pos, &move))
	{
		// 飛ばなくする
		pos.y += move.y, move.y = 0;
	
		// 着地モーション
		if (isJump)
		{
			Set_motion(6);
			motion_no = 6;
		}
		isLand = true;
		isJump = false;
		attackFlag = false;
	}
	else
	{
		stage->Collision_up( pos, &move, 15 );
		isLand = false;
	}
	// 座標更新
	pos.x += move.x;
	pos.y += move.y;
	pos.z += move.z;

	//	デスク初期化
	ServerManager::ResetDesc(m_id);

	//仮でプレイヤー更新
	//BasePlayer::Update();
}

void BasePlayer::Render()
{
	models[(int)model_part]->Animation();
	models[(int)model_part]->SetScale(scale);
	models[(int)model_part]->SetAngle(angleY);
	models[(int)model_part]->SetPos(pos);
	models[(int)model_part]->Update();
	models[(int)model_part]->Render();
}


void BasePlayer::Set_motion(int no)
{
	if (models[(int)model_part]->GetMotion() != no)
	{
		models[(int)model_part]->SetMotion(no);
	}
}


//*****************************************************************************
//
//		「TPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{
	me->model_part = MODEL::NORMAL;

	me->motion_no = 1;
	me->Set_motion(1);

	me->poster_num = -1;
}

void BasePlayer::Action::Move::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	// ADWS
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

	float pow = sqrtf(AxisX*AxisX + AxisY*AxisY);
	if (pow)
	{
		AxisX *= 1 / pow;
		AxisY *= 1 / pow;
	}

	// ジャンプ
	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
	{
		if (!me->isJump && me->isLand)
		{
			me->jump_pow = JUMP_POW;
			me->isJump = true;
			me->isLand = false;
		}
	}
	if (!me->isLand)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	//アングル処理	角度補正
	if (!(_ControlDesc.controlFlag & ((BYTE)PLAYER_CONTROL::LEFT_CLICK | (BYTE)PLAYER_CONTROL::RIGHT_CLICK))){
		float	work;
		work = _ControlDesc.mouseX *0.000001f;
		if (work > 0.1f) work = 0.1f;
		me->angleY += work;// Angleに加算
	}

	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));


	front.Normalize();
	right.Normalize();
	// 移動量決定
	me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
	me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

	// 止まってる状態
	if (me->move.Length() == 0)
	{
		// 待機モーション
		if (me->motion_no != 5 && me->motion_no != 6) me->motion_no = 1;
	}
	// 何かしら動いてる状態
	else
	{
		// 移動モーション
		if (me->motion_no != 5 && me->motion_no != 6)
		{
			if (AxisY > 0) me->motion_no = 0;
			else if (AxisX < 0) me->motion_no = 11;
			else if (AxisX > 0) me->motion_no = 10;
			else if (AxisY < 0) me->motion_no = 7;
		}
	}

	if (me->isJump)
	{
		// ジャンプモーション
		me->motion_no = 5;
	}

	else if (me->models[(int)me->model_part]->GetParam(0) == 9)
	{
		me->motion_no = 1;
		me->models[(int)me->model_part]->SetParam(0, 0);
	}

	me->move += Vector3(0, me->move.y - me->fallspeed, 0);

	//===========================================================================
	//	右クリック処理
	if (_ControlDesc.skillFlag != 0)
	{
		// ここを通る=必殺技発動

		// スキルアクション発動!!!
		if (_ControlDesc.skillFlag & (int)PLAYER_SKILL::GUN) me->select_skill = SKILL::GUN;
		else if (_ControlDesc.skillFlag & (int)PLAYER_SKILL::KABUTO)
		{
			if (!(me->kabuto_timer & 0xffff))//無敵中じゃない
			{
				me->stateFlag ^= (int)PLAYER_FLAG::BARRIER;
				me->kabuto_timer = 600;
			}
			return;
		}
		else if (_ControlDesc.skillFlag & (int)PLAYER_SKILL::SYURIKEN) me->select_skill = SKILL::SYURIKEN;
		me->reserve_action = me->skill_data[(int)me->select_skill].do_action;
		me->Change_action(ACTION_PART::TRANS_FORM);
	}

	//===========================================================================
	//	左クリック処理
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		if (manhole_mng->CheckManhole((me->isManhole) ? ManholeMng::LAND_TYPE::TIKA : ManholeMng::LAND_TYPE::TIJOU, 20, &me->pos, &me->angleY, &me->next_manhole_pos))
		{
			me->Change_action(ACTION_PART::MANHOLE);
		}
		if (!auto_target)
		{
			me->poster_num = paper_obj_mng->Can_targeting(me, (me->isJump) ? 15.0f : 10.0f, 180);

			if (me->poster_num != -1)
			{
				me->Change_action(ACTION_PART::MOVE_TARGET);
			}
		}
	}
	else
	{
		me->push_rend = false;
	}

	//===========================================================================
	//	真ん中クリック処理
	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::ATTACK_BUTTON)
	{
		if (!me->attackFlag)
		{
			me->Change_action(ACTION_PART::ATTACK);
			me->attackFlag = true;
		}
	}

	//	自動ターゲッティング
	me->poster_num = paper_obj_mng->Can_targeting(me, 10, 180);
	//if (me->poster_num != -1 && auto_target)
	//{
	//	me->Change_action(ACTION_PART::MOVE_TARGET);
	//	return;
	//}

	if (me->pos.y < -500)
	{
		me->Change_action(ACTION_PART::DIE);
	}
}


//*****************************************************************************
//
//		「ターゲット移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::MoveTarget::Initialize()
{
	me->model_part = MODEL::NORMAL;

	me->motion_no = 1;
	me->Set_motion(1);
}

void BasePlayer::Action::MoveTarget::Update(const CONTROL_DESC &_ControlDesc)
{
	// 距離範囲外
	if ((paper_obj_mng->Get_pos(me->poster_num) - me->pos).Length() > CAN_TARGET_DIST * 1.5f
		|| (!auto_target && !(_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)))
	{
		me->Change_action(ACTION_PART::MOVE);
		return;
	}
	
	float AxisX = 0, AxisY = 0;

	// ADWS
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

	//アングル処理	角度補正
	if (!(_ControlDesc.controlFlag & ((BYTE)PLAYER_CONTROL::LEFT_CLICK | (BYTE)PLAYER_CONTROL::RIGHT_CLICK))){
		float	work;
		work = _ControlDesc.mouseX *0.000001f;
		if (work > 0.1f) work = 0.1f;
		me->angleY += work;// Angleに加算
	}

	float pow = sqrtf(AxisX*AxisX + AxisY*AxisY);
	if (pow)
	{
		AxisX *= 1 / pow;
		AxisY *= 1 / pow;

		if (auto_target)
		{
			//	角度補正
			Vector3 v1, vn;
			v1.x = paper_obj_mng->Get_pos(me->poster_num).x - me->pos.x;
			v1.y = 0;
			v1.z = paper_obj_mng->Get_pos(me->poster_num).z - me->pos.z;
			vn = v1;
			vn.Normalize();
			float	x2 = sinf(me->angleY);
			float	z2 = cosf(me->angleY);
			//	内積による補正量調整
			float	d = sqrtf(vn.x*x2 + vn.z*z2);
			if (d == 0){ d = 0.1f; v1.z = 0.1f; }

			if (d > 0)
			{
				//	内積
				float n = (vn.x*x2 + vn.z*z2);
				//	角度補正量
				float adjust = (1 - n) * 2.0f;
				if (adjust > 0.1f) adjust = 0.1f;
				//	外積による左右回転
				float	g = v1.x*z2 - x2*v1.z;
				me->angleY += (g < 0) ? -adjust : adjust;
			}
		}
	}

	// ジャンプ
	if (!me->isJump && me->isLand)
	{
		if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
		{
			me->jump_pow = JUMP_POW;
			me->isJump = true;
		}
	}
	if (me->isJump)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));

	front.Normalize();
	right.Normalize();
	// 移動量決定
	me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
	me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

	// 止まってる状態
	if (me->move.Length() == 0)
	{
		// 待機モーション
		if (me->motion_no != 5 && me->motion_no != 6) me->motion_no = 1;
	}
	// 何かしら動いてる状態
	else
	{
		// 移動モーション
		if (me->motion_no != 5 && me->motion_no != 6)
		{
			if (AxisY > 0) me->motion_no = 0;
			else if (AxisX < 0) me->motion_no = 11;
			else if (AxisX > 0) me->motion_no = 10;
			else if (AxisY < 0) me->motion_no = 7;
		}
	}

	if (me->isJump)
	{
		// ジャンプモーション
		me->motion_no = 5;
	}

	else if (me->models[(int)me->model_part]->GetParam(0) == 9)
	{
		me->motion_no = 1;
		me->models[(int)me->model_part]->SetParam(0, 0);
	}

	me->move += Vector3(0, me->move.y - me->fallspeed, 0);


	//===========================================================================
	//	左クリック処理
	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK && !me->push_rend)
	{
		if (auto_target)
		{
			const int no = paper_obj_mng->Can_do(me);

			// ポスターがあった
			if (no != -1 && me->poster_num == no)
			{
				me->Change_action(ACTION_PART::REND);
			}
		}
		else
		{
			const int no = paper_obj_mng->Can_targeting(me, (me->isJump) ? 15.0f : 10.0f, 180);

			// ポスターがあった
			if (no != -1)
			{
				me->poster_num = no;
				me->Change_action(ACTION_PART::REND);
			}
		}
	}
	//===========================================================================
	//	右クリック処理
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{

	}

	//===========================================================================
	//	真ん中クリック処理
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::ATTACK_BUTTON)
	{
		// 神ゲージ50消費
		if (me->god_gage >= 50)
		{
			me->god_gage -= 50;
			me->Change_action(ACTION_PART::GUN);
		}
		else
		{
			me->Change_action(ACTION_PART::ATTACK);
		}
	}

	//===========================================================================
	//	Cトグル処理
	//else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::TRG_C)
	//{
	//	me->Change_action(ACTION_PART::MOVE);
	//}
}


//*****************************************************************************
//
//		「攻撃」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{
	me->move = VECTOR_ZERO;

	me->model_part = MODEL::NORMAL;
	me->Set_motion(4);


	// 固まらせる処理
	target_no = player_mng->Check_attack(me->m_id, 24);
	if (target_no != -1)
	{
		// 座標と向きを変更
		const static float dist = 5.0f;
		player_mng->Get_player(target_no)->Set_angleY(me->angleY);
		player_mng->Get_player(target_no)->Set_pos(me->pos - (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist));

		// 固まらせる
		player_mng->Get_player(target_no)->Change_action(ACTION_PART::FREEZE);
	}
}

void BasePlayer::Action::Attack::Update(const CONTROL_DESC &_ControlDesc)
{
	// モーション終了
	if (me->models[(int)me->model_part]->GetParam(0) == 2)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 破くモーションのフレーム
	else if (me->models[(int)me->model_part]->GetParam(0) == 1)
	{
		// 破く処理
		if (target_no != -1)
		{
			player_mng->Get_player(target_no)->Change_action(ACTION_PART::DIE);
		}
	}

	
}

//*****************************************************************************
//
//		「ポスター破る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{
	rended = false;
	me->move = VECTOR_ZERO;	

	me->model_part = MODEL::NORMAL;

	// ポスターに応じて座標と向きを変更
	//if (paper_obj_mng->Get_kind() == KIND_PAPER_OBJECT::SHOJI)
	//{
	//	const static float dist = 5.0f;
	//	me->pos = paper_obj_mng->Get_pos(me->poster_num);
	//	me->angleY = paper_obj_mng->Get_angle(me->poster_num) + PI;
	//	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);
	//}
	//Vector3 v = paper_obj_mng->Get_pos(me->poster_num) - me->pos;
	//v.Normalize();
	//me->angleY = atan2(v.x,v.z);

	me->motion_no = 1;
	me->Set_motion(1);

	//if (!thrash_rend)
	//{
	//	me->motion_no = 2;
	//	me->Set_motion(2);
	//
	//	// 送信するデータプッシュ
	//	for (int i = 0; i < PLAYER_MAX; i++)
	//	{
	//		PaperData data;
	//		data.from = me->mynumber;
	//		data.ID = me->poster_num;
	//		player_mng->Get_player(i)->paperqueue->Push(data);
	//	}
	//	me->Change_action(ACTION_PART::REND_OBJ);
	//	me->push_rend = true;
	//}
}

void BasePlayer::Action::Rend::Update(const CONTROL_DESC &_ControlDesc)
{
	// まだ破いていない
	if (!rended)
	{
		// 破けコマンドがONなら
		if (me->isJump && (_ControlDesc.controlFlag &(BYTE)PLAYER_CONTROL::LEFT_CLICK))
		{
			me->motion_no = 21;
			me->Set_motion(21);
			rended = true;

			// 送信するデータプッシュ
			for (int i = 0; i < PLAYER_MAX; i++)
			{
				PaperData data;
				data.from = me->mynumber;
				data.ID = me->poster_num;
				player_mng->Get_player(i)->paperqueue->Push(data);
			}
			me->Change_action(ACTION_PART::REND_OBJ);
			me->push_rend = true;
		}
		else if (_ControlDesc.rendFlag & (BYTE)PLAYER_FLAG::REND)
		{
			me->motion_no = 2;
			me->Set_motion(2);
			rended = true;

			// 送信するデータプッシュ
			for( int i = 0; i < PLAYER_MAX; i++ )
			{
				PaperData data;
				data.from = me->mynumber;
				data.ID = me->poster_num;
				player_mng->Get_player( i )->paperqueue->Push( data );
			}
			me->Change_action(ACTION_PART::REND_OBJ);
			me->push_rend = true;
		}
		// マウス離したらモード戻す
		else if ((_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK) == 0)
		{
			me->Change_action(ACTION_PART::MOVE);
		}
	}
	else // 破き中
	{
		me->motion_no = 2;
		me->Set_motion(2);

		// モーション終了
		if (me->models[(int)me->model_part]->GetParam(0) == 2)
		{
			me->Change_action(ACTION_PART::MOVE);
			me->poster_num = -1;
		}

		// 破くモーションのフレーム
		else if (me->models[(int)me->model_part]->GetParam(0) == 1)
		{
			// 破く処理
			score->Add(paper_obj_mng->Get_point(me->poster_num), me->mynumber);
			paper_obj_mng->Rend(me->poster_num);
			me->god_gage++;
		}
	}
}

//*****************************************************************************
//
//		「固まる」状態処理
//
//*****************************************************************************
void BasePlayer::Action::Freeze::Initialize()
{
	me->move = VECTOR_ZERO;
}

void BasePlayer::Action::Freeze::Update(const CONTROL_DESC &_ControlDesc)
{}




//*****************************************************************************
//
//		「死ぬ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{
	die_frame = 0;
	me->move = VECTOR_ZERO;
	me->model_part = MODEL::DIE;
	// 0フレームにリセット
	me->models[(int)me->model_part]->SetFrame(0);

	me->invincible = true;
}

void BasePlayer::Action::Die::Update(const CONTROL_DESC &_ControlDesc)
{
	if (!me->isLand)
	{
		me->move.x = me->huttobi.x;
		me->move.z = me->huttobi.z;
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}
	else
	{
		me->move.x = 0;
		me->move.z = 0;
	}

	me->move.y -= me->fallspeed;

	if (die_frame++ > 180)
	{
		me->Change_action(ACTION_PART::RESPAWN);
		me->huttobi = VECTOR_ZERO;
	}
}


//*****************************************************************************
//
//		「リスポーン」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Respawn::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;
	invincible_time = 0;
	me->pos.y = 150.0f;

	me->model_part = MODEL::NORMAL;
	me->Set_motion(8);
	me->motion_no = 8;
}

void BasePlayer::Action::Respawn::Update(const CONTROL_DESC &_ControlDesc)
{
	// 着地モーション中
	if (me->motion_no == 9)
	{
		if (me->models[(int)me->model_part]->GetFrame() >= 495)
		{
			me->invincible = false;
			me->Change_action(ACTION_PART::MOVE);
		}
		me->move.x *= .1f;
		me->move.z *= .1f;
		me->move.y -= me->fallspeed;
		if(me->isLand)me->models[(int)me->model_part]->Animation();
	}

	// 飛来中
	else
	{
		//アングル処理	角度補正
		float	work;
		work = _ControlDesc.mouseX *0.000001f;
		if (work > 0.1f) work = 0.1f;
		me->angleY += work;// Angleに加算

		float AxisX = .0f, AxisY = .0f;

		if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
		if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
		if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
		if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

		//	移動ベクトル設定
		Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
		Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));
		front.Normalize();
		right.Normalize();

		//	移動量決定
		me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
		me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

		// 重力による落下
		me->move.y = -(me->fallspeed);

		// 着地
		if (me->isLand || _ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK || _ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
		{
			me->Set_motion(9);
			me->motion_no = 9;
		}
	}
}

//*****************************************************************************
//
//		「紙鉄砲」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Gun::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;	// 鉄砲中は無敵

	me->model_part = MODEL::GUN;
	// 0フレームにリセット
	me->models[(int)me->model_part]->SetFrame(0);
	me->models[(int)me->model_part]->SetParam(0, 0);	// 普通モデル以外にパラメータを持たせる場合これが必要
}

void BasePlayer::Action::Gun::Update(const CONTROL_DESC &_ControlDesc)
{
	if (me->models[(int)me->model_part]->GetParam(0) == 2)	// モーション終了
	{
		me->invincible = false;
		me->Change_action(ACTION_PART::MOVE);
	}

	else if (me->models[(int)me->model_part]->GetParam(0) == 1)	// 判定フレーム
	{
		// Vs Poster
		int poster_numbers[128];
		paper_obj_mng->Can_dist(me->pos, 80.0f, poster_numbers);	// 範囲内のポスター番号取得

		for (int i = 0; poster_numbers[i] != -1; i++)	// -1(終端)
		{
			paper_obj_mng->Rend(poster_numbers[i]);
			score->Add(paper_obj_mng->Get_point(poster_numbers[i]), me->mynumber);
			me->god_gage++;

			// 送信するデータプッシュ
			for( int n = 0; n < PLAYER_MAX; n++ )
			{
				PaperData data;
				data.from = me->mynumber;
				data.ID = poster_numbers[i];
				player_mng->Get_player( n )->paperqueue->Push( data );
			}
		}

		// Vs Player
		player_mng->Check_gun_attack(me->mynumber, 40.0f);
	}
}


//*****************************************************************************
//
//		「マンホール潜る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Manhole::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;	// 無敵

	me->model_part = MODEL::NORMAL;

	me->motion_no = 19;
	me->Set_motion(19);
}

void BasePlayer::Action::Manhole::Update(const CONTROL_DESC &_ControlDesc)
{
	if (me->models[(int)me->model_part]->GetParam(0) == 2)
	{
		//pos.y = -120;
		me->isManhole ^= 0x1;
		me->pos = me->next_manhole_pos;
		me->Set_motion(1);
		me->Change_action(ACTION_PART::MOVE);
		me->invincible = false;
	}
}

//*****************************************************************************
//
//		「狭い壁通り抜け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Through::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;	// 無敵

	me->model_part = MODEL::NORMAL;

	me->motion_no = 14;
	me->Set_motion(14);
}

void BasePlayer::Action::Through::Update(const CONTROL_DESC &_ControlDesc)
{
	me->move.x = sinf(me->angleY)*2.0f;
	me->move.y = .0f;
	me->move.z = cosf(me->angleY)*2.0f;

	if (1)
	{
		me->Change_action(ACTION_PART::MOVE);
	}
}


//*****************************************************************************
//
//		「手裏剣」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Syuriken::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;	// 鉄砲中は無敵

	me->model_part = MODEL::SYURIKEN;
	// 0フレームにリセット
	me->motion_no = 1;
	me->Set_motion(1);

	max_speed = 6.0f;
	accel = max_speed;
	kasoku = .25f;

	hit_stop = 0;

	ServerManager::ResetControl(me->mynumber);
}

void BasePlayer::Action::Syuriken::Update(const CONTROL_DESC &_ControlDesc)
{
	if (hit_stop > 0)
	{
		me->move = VECTOR_ZERO;
		hit_stop--;
		return;
	}

	const Vector3 move_vec = Vector3(sinf(me->angleY), 0, cosf(me->angleY));
	me->move = move_vec * accel;
	if ((accel -= kasoku) < max_speed * .5f)
		accel = max_speed * .5f;
	if (_ControlDesc.controlFlag & (int)PLAYER_CONTROL::LEFT_CLICK ||
		_ControlDesc.controlFlag & (int)PLAYER_CONTROL::RIGHT_CLICK ||
		_ControlDesc.controlFlag & (int)PLAYER_CONTROL::ATTACK_BUTTON
		//syurikentaimaa - (int)timer->Get_second_limit() > 3 // 3秒後
		)
	{
		me->invincible = false;
		me->Change_action(ACTION_PART::MOVE);
	}

	// Vs Poster
	int rend_no = paper_obj_mng->Can_targeting(me, 20, 360);

	if (rend_no != -1)
	{
		paper_obj_mng->Rend(rend_no);
		score->Add(paper_obj_mng->Get_point(rend_no), me->mynumber);
		me->god_gage++;

		// 送信するデータプッシュ
		for (int n = 0; n < PLAYER_MAX; n++)
		{
			PaperData data;
			data.from = me->mynumber;
			data.ID = rend_no;
			player_mng->Get_player(n)->paperqueue->Push(data);
		}
		hit_stop = 2;
	}

	if (_ControlDesc.controlFlag & (int)PLAYER_CONTROL::SPACE)
	{
		me->jump_pow = 2.0f;
		me->isJump = true;
		me->invincible = false;
		me->Change_action(ACTION_PART::MOVE);
	}

	// vs Player
	int target_no = player_mng->Check_attack(me->m_id, 10);
	if (target_no != -1)
	{
		player_mng->Get_player(target_no)->Change_action(ACTION_PART::DIE);
		hit_stop = 12;
	}
}

void BasePlayer::Collision_syuriken()
{
	// レイピックでmove値が変わるので保存しておく
	Vector3 prev_move = move;
	Vector3 n;

	if (stage->Collision(pos, &move, 5, 2, &n))		// 壁にぶつかっていたら(true)
	{
		prev_move.Normalize();
		if (n.x*prev_move.x + n.z*prev_move.z < -0.8f)	// 壁に対してほぼ垂直にヒット
		{
			jump_pow = 2.0f;
			isJump = true;
			invincible = false;
			Change_action(ACTION_PART::MOVE);	// 手裏剣解除
		}
		else
		{
			angleY = atan2(move.x, move.z);	// 方向転換
		}
	}
}

//*****************************************************************************
//
//		「変身」状態処理
//
//*****************************************************************************

void BasePlayer::Action::TransForm::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;	// 鉄砲中は無敵

	me->model_part = MODEL::NORMAL;
	me->motion_no = 12;
	me->Set_motion(12);

	hensintaimaa = (int)timer->Get_second_limit();	// 使わん

	ServerManager::ResetControl(me->mynumber);
}

void BasePlayer::Action::TransForm::Update(const CONTROL_DESC &_ControlDesc)
{
	if (me->models[(int)me->model_part]->GetParam(0) == 1)
	{
		me->invincible = false;
		me->Change_action(me->reserve_action);
	}
}


//*****************************************************************************
//
//		「対小物破る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::RendObj::Initialize()
{
	me->move = VECTOR_ZERO;
	me->invincible = true;	// 無敵

	// サーバー側でモデル差し替えはしない、するのはクライアント側
	me->model_part = MODEL::NORMAL;
	me->Set_motion(1);

	ServerManager::ResetControl(me->mynumber);

	rend_timer = 10;

	score->Add(paper_obj_mng->Get_point(me->poster_num), me->mynumber);
	paper_obj_mng->Rend(me->poster_num);

	me->god_gage++;
}

void BasePlayer::Action::RendObj::Update(const CONTROL_DESC &_ControlDesc)
{
	// 時間経過
	if (--rend_timer < 0)
	{
		if (_ControlDesc.motion_no != (int)PLAYER_CONTROL::RENDING){ // クライアント->小物破り終わったよ
			me->invincible = false;
			me->Change_action(ACTION_PART::MOVE);
		}
	}
}