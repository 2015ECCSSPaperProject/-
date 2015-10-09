
#include	"Camera.h"

//#include	"../scene/sceneMain.h"
#include	"../player/Player_manager.h"
#include	"../player/Player.h"

#include	"../Mouse/Mouse.h"

// カメラ⇒プレイヤーの基本距離
const float Camera::Mode::Base::DIST = 20.0f;


Camera::Camera() : iexView()
{
	// 基本パラメータ初期化
	pos = Vector3(0, 10.0f, -20.0f);
	target = Vector3(0, 0, 0);
	ipos = Vector3(0, 10.0f, -20.0f);
	itarget = Vector3(0, 0, 0);
	angle = Vector3(0, 0, 0);

	// 行動状態初期化
	mode[MODE_PART::M_TPS] = new Camera::Mode::TPS(this);
	mode[MODE_PART::M_FPS] = new Camera::Mode::FPS(this);
	mode[MODE_PART::M_STOP] = new Camera::Mode::Stop(this);
	mode[MODE_PART::M_PASTE] = new Camera::Mode::Paste(this);

	Change_mode(MODE_PART::M_TPS);	// 最初は三人称
}

Camera::~Camera()
{
	for (int i = 0; i < MODE_PART::M_MAX; i++)
	{
		delete mode[i];
	}
}

void Camera::Initialize(Player_manager *player_mng)
{
	this->player_mng = player_mng;
}

void Camera::Update()
{
	mode[mode_part]->Update();
}


//*****************************************************************************
//
//		状態処理ベース関係
//
//*****************************************************************************
void Camera::Mode::Base::Collision()
{
	//extern iexMesh* hitStage;

	//Vector3 Out, ray_Pos, Vec;

	////レイを飛ばす原点は注視点
	//ray_Pos = target;

	////ベクトルはその注視点からカメラ自身の座標
	//Vec = pos - target;
	//Vec.Normalize();

	//float Dist = 500.0f;

	//ray_Pos -= Vec;
	//if (hitStage->RayPick(&Out, &ray_Pos, &Vec, &Dist) != -1){

	//	//もし注視点から壁の距離がradiusより小さい場合
	//	if ((Out - target).Length() < (pos - target).Length()) pos = Out;
	//}
}

//*****************************************************************************
//
//		「3人称視点」状態処理
//
//*****************************************************************************

void Camera::Mode::TPS::Initialize()
{
	me->angle.x = 0;
	dist = DIST;
}

void Camera::Mode::TPS::Update()
{
#ifndef MOUSE

	// プレイヤー角度取得
	Vector3 p_angle;
	me->player_mng->Get_Player()->Get_angle(p_angle);
	me->angle.y = p_angle.y;
	me->angle.x += Mouse::Get_axis_y() * .05f;

#endif

	//カメラの上下移動に制限
	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.2f)me->angle.x = 0.2f;

	// オーバーフロー防止
	if (me->angle.y > PI*2)me->angle.y -= PI*2;
	else if (me->angle.y < -PI*2)me->angle.y += PI*2;


	// 角度の値によるベクトルを作成
	float ay_x = sinf(me->angle.y);
	float ay_z = cosf(me->angle.y);
	float ax_y = sinf(me->angle.x);
	float ax_z = cosf(me->angle.x);

	Vector3 vec(
		ay_x * ax_z,
		ax_y,
		ay_z * ax_z);

	// ベクトルの長さ決定
	vec *= this->dist;


	// プレイヤー座標取得
	Vector3 p_pos;
	me->player_mng->Get_Player()->Get_pos(p_pos);

	p_pos.y += 2.5f;	// 少し上に



	// 角度の値のベクトルとプレイヤーからカメラ位置算出
	me->ipos.x = p_pos.x - vec.x;
	me->ipos.y = p_pos.y - vec.y;
	me->ipos.z = p_pos.z - vec.z;

	me->ipos.y += 4.0f;	// 少し上に

	// 注視点はプレイヤー
	me->target = p_pos;


	//壁判定
	//Collision();

	// 目標座標までゆっくり動かす
	me->pos = me->pos * .85f + me->ipos * .15f;

	me->Set(me->pos, me->target);



	// プレイヤーモードでカメラ切り替え
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE_FPS) me->Change_mode(MODE_PART::M_FPS);
	//else if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_PASTE);
}


//*****************************************************************************
//
//		「1人称視点」状態処理
//
//*****************************************************************************

void Camera::Mode::FPS::Initialize()
{}

void Camera::Mode::FPS::Update()
{
#ifndef MOUSE
	me->angle.x += Mouse::Get_axis_y() *.05f;
#endif


	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.4f)me->angle.x = 0.4f;


	// プレイヤー座標・角度取得
	Vector3 p_pos, p_angle;
	me->player_mng->Get_Player()->Get_pos(p_pos);
	p_pos.y += 2.5f;	// 少し上に

	me->player_mng->Get_Player()->Get_angle(p_angle);
	me->angle.y = p_angle.y;

	// 角度の値によるベクトルを作成
	float ay_x = sinf(me->angle.y);
	float ay_z = cosf(me->angle.y);
	float ax_y = sinf(me->angle.x);
	float ax_z = cosf(me->angle.x);

	Vector3 vec(
		ay_x * ax_z,
		ax_y,
		ay_z * ax_z);

	// 座標設定
	me->pos = p_pos;
	me->pos.x = p_pos.x - (vec.x*-0.5f);
	me->pos.y = p_pos.y - (vec.y*-0.5f);
	me->pos.z = p_pos.z - (vec.z*-0.5f);

	me->pos.y += 1.25f;	// 少し上に



	me->target.x = me->pos.x + (sinf(me->angle.y));
	me->target.y = me->pos.y + (tanf(me->angle.x));
	me->target.z = me->pos.z + (cosf(me->angle.y));

	me->Set(me->pos, me->target);


	// プレイヤーモードでカメラ切り替え
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE) me->Change_mode(MODE_PART::M_TPS);
	//else if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_PASTE);
}




//*****************************************************************************
//
//		「止まってる」状態処理
//
//*****************************************************************************

void Camera::Mode::Stop::Initialize()
{}

void Camera::Mode::Stop::Update()
{
	// 何もしない
	me->Set(me->pos, me->target);
}


//*****************************************************************************
//
//		「プレイヤーがポスターを張り付けてる」状態処理
//
//*****************************************************************************

void Camera::Mode::Paste::Initialize()
{
	dist = DIST * .5f;
}

void Camera::Mode::Paste::Update()
{
#ifndef MOUSE

	// プレイヤー角度取得
	Vector3 p_angle;
	me->player_mng->Get_Player()->Get_angle(p_angle);
	me->angle.y = p_angle.y;
	me->angle.x += Mouse::Get_axis_y() * .05f;

#endif

	//カメラの上下移動に制限
	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.2f)me->angle.x = 0.2f;

	// オーバーフロー防止
	if (me->angle.y > PI * 2)me->angle.y -= PI * 2;
	else if (me->angle.y < -PI * 2)me->angle.y += PI * 2;


	// 角度の値によるベクトルを作成
	float ay_x = sinf(me->angle.y);
	float ay_z = cosf(me->angle.y);
	float ax_y = sinf(me->angle.x);
	float ax_z = cosf(me->angle.x);

	Vector3 vec(
		ay_x * ax_z,
		ax_y,
		ay_z * ax_z);

	// ベクトルの長さ決定
	vec *= this->dist;


	// プレイヤー座標取得
	Vector3 p_pos;
	me->player_mng->Get_Player()->Get_pos(p_pos);

	p_pos.y += 2.5f;	// 少し上に



	// 角度の値のベクトルとプレイヤーからカメラ位置算出
	me->ipos.x = p_pos.x - vec.x;
	me->ipos.y = p_pos.y - vec.y;
	me->ipos.z = p_pos.z - vec.z;

	me->ipos.y += 4.0f;	// 少し上に

	// 注視点はプレイヤー
	me->target = p_pos;


	//壁判定
	//Collision();

	// 目標座標までゆっくり動かす
	me->pos = me->pos * .85f + me->ipos * .15f;

	// 何もしない
	me->Set(me->pos, me->target);

	// プレイヤーモードがTPSならカメラ切り替え
	if (me->player_mng->Get_Player()->Get_action() != Player::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_TPS);
}