
#include	"Camera.h"

//#include	"../scene/sceneMain.h"
#include	"../player/Player_manager.h"
#include	"../player/Player.h"

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
	dist = 20.0f;
}

void Camera::Mode::TPS::Update()
{
	// 微傾き排除
	float axis_x = (float)KEY_GetAxisX2()*.001f;
	float axis_y = (float)KEY_GetAxisY2()*.001f;
	float dist = sqrtf(axis_x * axis_x + axis_y * axis_y);
	if (dist < .35f) {
		axis_x = 0;
		axis_y = 0;
	}
	if (dist > 1.f)
	{
		axis_x /= dist;
		axis_y /= dist;
	}
	me->angle.y += axis_x*.05f;
	me->angle.x -= axis_y*.05f;

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



	// プレイヤーモードがFPSならカメラ切り替え
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE_FPS) me->Change_mode(MODE_PART::M_FPS);
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
	// 右スティック取得
	float axis_x = (float)KEY_GetAxisX2()*.001f;
	float axis_y = (float)KEY_GetAxisY2()*.001f;
	float dist = sqrtf(axis_x * axis_x + axis_y * axis_y);
	if (dist < .35f) {
		axis_x = 0;
		axis_y = 0;
	}
	if (dist > 1.f)
	{
		axis_x /= dist;
		axis_y /= dist;
	}

	me->angle.x += axis_y*.05f;
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


	// プレイヤーモードがTPSならカメラ切り替え
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE) me->Change_mode(MODE_PART::M_TPS);
}