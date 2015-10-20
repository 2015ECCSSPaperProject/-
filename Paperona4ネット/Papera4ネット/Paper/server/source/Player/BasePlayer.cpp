#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"

#include	"../poster/Poster_manager.h"

/*	ベースプレイヤー	*/

//****************************************************************************************************************
//
//  
//
//****************************************************************************************************************
BasePlayer::BasePlayer(int id) :m_id(id)
{
	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;
}

void BasePlayer::Initialize(iex3DObj *obj)
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

	// 3D実体
	model = obj->Clone();

	mynumber = m_id;



	// 行動状態初期化
	action[(int)ACTION_PART::MOVE] = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_FPS] = new BasePlayer::Action::MoveFPS(this);
	action[(int)ACTION_PART::ATTACK] = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE] = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND] = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::DIE] = new BasePlayer::Action::Die(this);

	Change_action(ACTION_PART::MOVE);	// 最初は移動状態
	camera_mode = CAMERA_MODE::TPS;

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
	SAFE_DELETE(model);
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


	// ここの代入をmotion_noも代入するようにしたら動いた(ゲッターで返すから)
	controlDesc.motion_no = motion_no = ServerManager::GetDesc(m_id).motion_no;//モーション番号

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
	model->Animation();
	model->SetScale(scale);
	model->SetAngle(angleY);
	model->SetPos(pos);
	model->Update();
	model->Render();
}


void BasePlayer::Set_motion(int no)
{
	if (model->GetMotion() != no)
	{
		model->SetMotion(no);
	}
}


//*****************************************************************************
//
//		「TPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{
	me->camera_mode = CAMERA_MODE::TPS;
}

void BasePlayer::Action::Move::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	// ADWS
	//if (KEY(KEY_LEFT) == 1) AxisX = -1;
	//else if (KEY(KEY_RIGHT) == 1) AxisX = 1;
	//if (KEY(KEY_UP) == 1) AxisY = 1;
	//else if (KEY(KEY_DOWN) == 1) AxisY = -1;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

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

	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// ポスターがあった
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_rend(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::REND);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// ポスターがあった
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_paste(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::PASTE);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::ATTACK_BUTTON)
	{
		me->Change_action(ACTION_PART::ATTACK);
	}

	else if (!(_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::TRG_C))
	{
		me->Change_action(ACTION_PART::MOVE_FPS);
	}
}


//*****************************************************************************
//
//		「FPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::MoveFPS::Initialize()
{
	me->camera_mode = CAMERA_MODE::FPS;
}

void BasePlayer::Action::MoveFPS::Update(const CONTROL_DESC &_ControlDesc)
{

	float AxisX = 0, AxisY = 0;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

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
		//Set_motion(1);
	}
	// 何かしら動いてる状態
	else
	{
		// 移動モーション
		//Set_motion(0);
	}

	me->move += Vector3(0, me->move.y - me->fallspeed, 0);

	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// ポスターがあった
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_rend(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::REND);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// ポスターがあった
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_paste(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::PASTE);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::ATTACK_BUTTON)
	{
		me->Change_action(ACTION_PART::ATTACK);
	}

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
	me->Set_motion(4);
}

void BasePlayer::Action::Attack::Update(const CONTROL_DESC &_ControlDesc)
{
	// モーション終了
	if (me->model->GetParam(0) == 2)
	{
		(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
		me->Set_motion(0);
	}

	// 破くモーションのフレーム
	else if (me->model->GetParam(0) == 1)
	{
		// 破く処理
		int no = PlayerManager::Check_attack(me->m_id);
		if (no != -1)
		{
			player[no]->Change_action(ACTION_PART::DIE);
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
	me->Set_motion(3);

	// ポスターに応じて座標と向きを変更
	const static float dist = 5.0f;
	me->pos = poster_mng->Get_pos(me->poster_num);
	me->angleY = poster_mng->Get_angle(me->poster_num) + PI;
	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);
}

void BasePlayer::Action::Paste::Update(const CONTROL_DESC &_ControlDesc)
{
	me->motion_no = 3;
	if (timer++ > 60)
	{
		(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
		me->Set_motion(0);
	}

	if (timer == 45)
	{
		// 貼り付ける処理
		poster_mng->Paste_poster(me->mynumber, me->poster_num);
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
	//me->Set_motion(2);

	// ポスターに応じて座標と向きを変更
	const static float dist = 5.0f;
	me->pos = poster_mng->Get_pos(me->poster_num);
	me->angleY = poster_mng->Get_angle(me->poster_num) + PI;
	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);
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
			(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
			me->Set_motion(0);
		}
	}
	else // 破き中
	{
		me->motion_no = 2;
		me->Set_motion(2);

		// モーション終了
		if (me->model->GetParam(0) == 2)
		{
			(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
			me->Set_motion(0);
		}

		// 破くモーションのフレーム
		else if (me->model->GetParam(0) == 1)
		{
			// 破く処理
			poster_mng->Rend_poster(me->mynumber, me->poster_num);
		}
	}
}


//*****************************************************************************
//
//		「死ぬ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{
	die_frame = 0;

	me->move = VECTOR_ZERO;
}

void BasePlayer::Action::Die::Update(const CONTROL_DESC &_ControlDesc)
{
	if (die_frame++ > 60)
	{
		(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
	}
}



//*****************************************************************************
//
//		「紙ヒコーキ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Hikouki::Initialize()
{}

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
{}

void BasePlayer::Action::Gun::Update(const CONTROL_DESC &_ControlDesc)
{}

//　実態
BasePlayer* player[PLAYER_MAX];







int PlayerManager::Check_attack(int me)
{
	int no = -1;
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (i == me)continue;

		Vector3 vec(player[i]->Get_pos() - player[me]->Get_pos());

		// 距離
		if (vec.Length() < 16)
		{
			vec.Normalize();
			Vector3 front(sinf(player[me]->Get_angleY()), 0, cosf(player[me]->Get_angleY()));

			// 角度
			if (Vector3Dot(vec, front) > .707f)
			{
				no = i;
				break;
			}
		}
	}
	return no;
}
