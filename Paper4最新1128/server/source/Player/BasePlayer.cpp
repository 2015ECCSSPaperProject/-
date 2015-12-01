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

/*	ベースプレイヤー	*/

const float CAN_TARGET_DIST = 90.0f;

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

void BasePlayer::Initialize(iex3DObj **objs)
{
	// 基本パラメータ初期化
	pos.x = -(float)(10 + (m_id * 4));// 仮
	angleY = 0.0f;
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 1.5f;
	fallspeed = .1f;
	se_receive = 0;
	isJump = isLand = false;
	jump_pow = 0;
	invincible = false;
	god_gage = 0;

	// 3D実体
	models[(int)MODEL::NORMAL] = objs[(int)PlayerManager::CLONE_TYPE::NORMAL]->Clone();
	models[(int)MODEL::DIE] = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::PLANE] = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::GUN] = objs[(int)PlayerManager::CLONE_TYPE::GUN]->Clone();

	mynumber = m_id;


	// 行動状態初期化
	action[(int)ACTION_PART::MOVE] = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_TARGET] = new BasePlayer::Action::MoveTarget(this);
	action[(int)ACTION_PART::ATTACK] = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE] = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND] = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::FREEZE] = new BasePlayer::Action::Freeze(this);
	action[(int)ACTION_PART::DIE] = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::RESPAWN] = new BasePlayer::Action::Respawn(this);
	action[(int)ACTION_PART::PLANE] = new BasePlayer::Action::Hikouki(this);
	action[(int)ACTION_PART::GUN] = new BasePlayer::Action::Gun(this);
	action[(int)ACTION_PART::MANHOLE] = new BasePlayer::Action::Manhole(this);
	action[(int)ACTION_PART::THROUGH] = new BasePlayer::Action::Through(this);


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


	// クライアント側で再生しているモーションの番号(いらない)
	//controlDesc.motion_no = motion_no = ServerManager::GetDesc(m_id).motion_no;//モーション番号

	// 破くフラグ
	controlDesc.rendFlag = ServerManager::GetDesc(m_id).rendFlag;

	// ぼたん
	controlDesc.controlFlag = ServerManager::GetDesc(m_id).controlFlag;

	//	コントローラーに操作パラメータをパス
	//Control_Update(controlDesc);//←デスクセット
	action[(int)action_part]->Update(controlDesc);

	

	stage->Collision(pos, &move, 5, 2);
	if (stage->Collision_rand(pos, &move, 0))
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
	}
	else
	{
		isLand = false;
	}
	// 座標更新
	pos += move;


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
	trg_target = true;
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
	if (!me->isJump && me->isLand)
	{
		if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
		{
			me->jump_pow = 2.0f;
			me->isJump = true;
		}
	}
	if (me->isJump)
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
	//	左クリック処理
	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		me->poster_num = paper_obj_mng->Can_do(me);
		me->poster_num = paper_obj_mng->Can_targeting(me, CAN_TARGET_DIST, 45);

		// ポスターがあった
		if (me->poster_num != -1)
		{
			if(!trg_target)me->Change_action(ACTION_PART::MOVE_TARGET);
			return;
		}
		trg_target = true;
	}
	else trg_target = false;

	//===========================================================================
	//	右クリック処理
	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		//me->poster_num = paper_obj_mng->poster->Can_do(me, me->mynumber);

		//// ポスターがあった
		//if (me->poster_num != -1)
		//{
		//	if (paper_obj_mng->poster->Can_paste(me->mynumber, me->poster_num))
		//	{
		//		me->Change_action(ACTION_PART::PASTE);
		//		return;
		//	}
		//}
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
	else if (!(_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::TRG_C))
	{
		//me->Change_action(ACTION_PART::MOVE_FPS);
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
	// 距離範囲外または左クリック解除
	if ((paper_obj_mng->Get_pos(me->poster_num)-me->pos).Length() >CAN_TARGET_DIST * 1.5f
	|| !(_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK))
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

	float pow = sqrtf(AxisX*AxisX + AxisY*AxisY);
	if (pow)
	{
		AxisX *= 1 / pow;
		AxisY *= 1 / pow;
	}
	if (me->isJump)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	//アングル処理	角度補正
	if (!(_ControlDesc.controlFlag & ((BYTE)PLAYER_CONTROL::LEFT_CLICK | (BYTE)PLAYER_CONTROL::RIGHT_CLICK)))
	{
		float	work;
		work = _ControlDesc.mouseX *0.000001f;
		if (work > 0.1f) work = 0.1f;
		me->angleY += work;// Angleに加算
	}

	//	移動ベクトル設定
	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));
	front.Normalize();
	right.Normalize();

	//	移動量決定
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
	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		int no = paper_obj_mng->Can_do(me);

		// ポスターがあった
		if (no != -1 && me->poster_num == no)
		{
			me->Change_action(ACTION_PART::REND);
		}
	}
	//===========================================================================
	//	右クリック処理
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		//me->poster_num = paper_obj_mng->poster->Can_do(me, me->mynumber);

		//// ポスターがあった
		//if (me->poster_num != -1)
		//{
		//	if (paper_obj_mng->poster->Can_paste(me->mynumber, me->poster_num))
		//	{
		//		me->Change_action(ACTION_PART::PASTE);
		//		return;
		//	}
		//}
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
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::TRG_C)
	{
		me->Change_action(ACTION_PART::MOVE);
	}
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
	target_no = player_mng->Check_attack(me->m_id);
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
//		「ポスター貼り付け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	timer = 0;
	me->move = VECTOR_ZERO;

	me->model_part = MODEL::NORMAL;
	me->Set_motion(3);

	// ポスターに応じて座標と向きを変更
	const static float dist = 5.0f;
	me->pos = paper_obj_mng->Get_pos(me->poster_num);
	me->angleY = paper_obj_mng->Get_angle(me->poster_num) + PI;
	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);
}

void BasePlayer::Action::Paste::Update(const CONTROL_DESC &_ControlDesc)
{
	me->motion_no = 3;
	if (timer++ > 60)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	if (timer == 45)
	{
		// 貼り付ける処理
		//poster_mng->Paste_poster(me->mynumber, me->poster_num);
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
	const static float dist = 5.0f;
	me->pos = paper_obj_mng->Get_pos(me->poster_num);
	me->angleY = paper_obj_mng->Get_angle(me->poster_num) + PI;
	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);

	me->motion_no = 1;
	me->Set_motion(1);
}

void BasePlayer::Action::Rend::Update(const CONTROL_DESC &_ControlDesc)
{
	// まだ破いていない
	if (!rended)
	{
		// 破けコマンドがONなら
		if (_ControlDesc.rendFlag & (BYTE)REND_FLAG::RIGHT)
		{
			me->motion_no = 2;
			me->Set_motion(2);
			rended = true;
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
			paper_obj_mng->Rend(me->poster_num);
			score->Add(1, me->mynumber);	// 仮で1点
			if ((me->god_gage += 5) > 100)me->god_gage = 100;	// 神ゲージUP
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
	if (die_frame++ > 60)
	{
		me->Change_action(ACTION_PART::RESPAWN);
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
	me->pos.y += 80.0f;

	me->model_part = MODEL::NORMAL;
	me->Set_motion(8);
	me->motion_no = 8;
}

void BasePlayer::Action::Respawn::Update(const CONTROL_DESC &_ControlDesc)
{
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

	//if (invincible_time++ > 30)
	//{
	//	me->invincible = false;
	//}

	// 着地
	if (me->isLand || _ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK || _ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		me->invincible = false;
		me->Change_action(ACTION_PART::MOVE);
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
	// 0フレームにリセット
	me->models[(int)me->model_part]->SetFrame(0);
}

void BasePlayer::Action::Hikouki::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY = -1;

	// ジャンプ
	if (!me->isJump && me->isLand)
	{
		if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
		{
			me->jump_pow = 2.0f;
			me->isJump = true;
		}
	}
	if (me->isJump)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	//アングル処理	角度補正
	float	work;
	work = _ControlDesc.mouseX *0.000001f;
	if (work > 0.1f) work = 0.1f;
	me->angleY += work;// Angleに加算


	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));

	//me->angle.y += Mouse::Get_axis_x() * .065f;


	front.Normalize();
	right.Normalize();
	// 移動量決定
	me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
	me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

	// 止まってる状態
	if (me->move.Length() == 0)
	{
		// 待機モーション
		//Set_motion(1);
	}
	// 何かしら動いてる状態
	else
	{
		// 移動モーション
		//Set_motion(0);


		// なうモーション
		//Set_motion()
	}


	me->move += Vector3(0, me->move.y - me->fallspeed, 0);
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
		paper_obj_mng->Can_dist(me->pos, 60.0f, poster_numbers);	// 範囲内のポスター番号取得

		for (int i = 0; poster_numbers[i] != -1; i++)	// -1(終端)
		{
			paper_obj_mng->Rend(poster_numbers[i]);
			score->Add(1, me->mynumber);	// 仮で1点
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
}

void BasePlayer::Action::Manhole::Update()
{
}

void BasePlayer::Action::Manhole::Render()
{
}



//*****************************************************************************
//
//		「狭い壁通り抜け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Through::Initialize()
{
}

void BasePlayer::Action::Through::Update()
{
}

void BasePlayer::Action::Through::Render()
{
}