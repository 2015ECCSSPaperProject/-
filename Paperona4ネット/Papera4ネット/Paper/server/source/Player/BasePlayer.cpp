#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"

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
	speed = 1.0f;
	fallspeed = .1f;
	se_receive = 0;

	// 3D実体
	model = obj->Clone();

	// 行動状態初期化
	action[(int)ACTION_PART::MOVE] = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_FPS] = new BasePlayer::Action::MoveFPS(this);
	action[(int)ACTION_PART::ATTACK] = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE] = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND] = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::DIE] = new BasePlayer::Action::Die(this);

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


	//	コントローラーに操作パラメータをパス
	//Control_Update(controlDesc);//←デスクセット
	action[(int)action_part]->Update(controlDesc);

	

	stage->Collision(pos, &move, 5, 2);
	if (stage->Collision_rand(pos, &move, 0))
	{
		// 飛ばなくする
		//pos.y += move.y, move.y = 0;
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


//*****************************************************************************
//
//		「TPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{}

void BasePlayer::Action::Move::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	// ADWS
	//if (KEY(KEY_LEFT) == 1) AxisX = -1;
	//else if (KEY(KEY_RIGHT) == 1) AxisX = 1;
	//if (KEY(KEY_UP) == 1) AxisY = 1;
	//else if (KEY(KEY_DOWN) == 1) AxisY = -1;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY = -1;


	//アングル処理	角度補正
	float	work;
	work = _ControlDesc.mouseX *0.000001f;
	if (work > 0.1f) work = 0.1f;
	me->angleY += work;// Angleに加算

#ifndef TYPE1

	Vector3 front(matView._13, 0, matView._33);
	Vector3 right(matView._11, 0, matView._31);

	me->angle.y += AxisX * .05f;

#endif

#ifndef TYPE2

	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));

	//me->angle.y += Mouse::Get_axis_x() * .065f;

#endif


	front.Normalize();
	right.Normalize();
	// 移動量決定
	me->move = (front*AxisY + right*AxisX) * (me->speed) +
		Vector3(0, me->move.y - me->fallspeed, 0);


	// 止まってる状態
	if (me->move.Length() == 0)
	{
		// 待機モーション
		//Set_motion(1);
	}
	// 何かしら動いてる状態
	else
	{

#ifndef TYPE1
		//	左右判定
		float x1 = me->move.x, z1 = me->move.z;
		float x2 = sinf(me->angle.y), z2 = cosf(me->angle.y);
		//	外積
		float g = x1*z2 - x2*z1;
		//	補整量調整	
		float d = sqrtf(x1*x1 + z1*z1);
		float n = (x1*x2 + z1*z2) / d;
		float adjust = (1 - n) * 2.0f;
		if (adjust > 0.3f) adjust = 0.3f;
		//	方向転換
		if (g < 0) me->angle.y -= adjust;
		else me->angle.y += adjust;
#endif


		// 移動モーション
		//Set_motion(0);


		// なうモーション
		//Set_motion()
	}
}


//*****************************************************************************
//
//		「FPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::MoveFPS::Initialize()
{
	// 待機モーションセット
	//Set_motion(0);
}

void BasePlayer::Action::MoveFPS::Update(const CONTROL_DESC &_ControlDesc)
{

	float AxisX = 0, AxisY = 0;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY = -1;

	//me->angle.y += Mouse::Get_axis_x() * .05f;

	//	移動ベクトル設定
	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));
	front.Normalize();
	right.Normalize();

	//	移動量決定
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);

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
}


//*****************************************************************************
//
//		「攻撃」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{}

void BasePlayer::Action::Attack::Update(const CONTROL_DESC &_ControlDesc)
{}


//*****************************************************************************
//
//		「ポスター貼り付け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{}

void BasePlayer::Action::Paste::Update(const CONTROL_DESC &_ControlDesc)
{}


//*****************************************************************************
//
//		「ポスター破る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{}

void BasePlayer::Action::Rend::Update(const CONTROL_DESC &_ControlDesc)
{

}


//*****************************************************************************
//
//		「死ぬ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{}

void BasePlayer::Action::Die::Update(const CONTROL_DESC &_ControlDesc)
{}



//*****************************************************************************
//
//		「紙ヒコーキ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Hikouki::Initialize()
{}

void BasePlayer::Action::Hikouki::Update(const CONTROL_DESC &_ControlDesc)
{}



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