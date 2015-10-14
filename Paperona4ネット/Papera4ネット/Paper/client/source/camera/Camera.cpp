
#include	"Camera.h"
#include	"../system/system.h"
//#include	"../scene/sceneMain.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/MyPlayer.h"

//#include	"../Mouse/Mouse.h"

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
	mode[MODE_PART::M_DEBUG] = new Camera::Mode::Debug(this);
	mode[MODE_PART::M_TPS] = new Camera::Mode::TPS(this);
	mode[MODE_PART::M_FPS] = new Camera::Mode::FPS(this);
	mode[MODE_PART::M_STOP] = new Camera::Mode::Stop(this);
	mode[MODE_PART::M_ZOOM] = new Camera::Mode::Zoom(this);

	Change_mode(MODE_PART::M_TPS);	// 最初は三人称
	//Change_mode(MODE_PART::M_DEBUG);	// デバッグカメラ
}

Camera::~Camera()
{
	for (int i = 0; i < MODE_PART::M_MAX; i++)
	{
		delete mode[i];
	}
}

void Camera::Initialize(BasePlayer *my)
{
	my_player = my;
}

void Camera::Update()
{
	mode[mode_part]->Update();
	shader->SetValue("ViewPos", pos);
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
//		「デバッグ視点」状態処理
//
//*****************************************************************************
void Camera::Mode::Debug::Initialize()
{

}

void Camera::Mode::Debug::Update()
{
	float speed = 2.0f;
	if (KEY(KEY_SPACE) == 1) speed *= 2.0f;

	//=================================
	//	カメラZ移動
	if (GetKeyState(VK_UP) & 0x80){
		me->pos.x -= sinf(me->angle.y) * speed;
		me->pos.z -= cosf(me->angle.y) * speed;
	}
	else if (GetKeyState(VK_DOWN) & 0x80){
		me->pos.x += sinf(me->angle.y) * speed;
		me->pos.z += cosf(me->angle.y) * speed;
	}

	//=================================
	//	カメラX移動
	if (GetKeyState(VK_RIGHT) & 0x80){
		me->pos.x += sinf(me->angle.y - (PI / 2)) * speed;
		me->pos.z += cosf(me->angle.y - (PI / 2)) * speed;
	}
	else if (GetKeyState(VK_LEFT) & 0x80){
		me->pos.x += sinf(me->angle.y + (PI / 2)) * speed;
		me->pos.z += cosf(me->angle.y + (PI / 2)) * speed;
	}

	//=================================
	//	カメラY移動
	if (GetKeyState(0x22) & 0x80) me->pos.y -= speed;
	else if (GetKeyState(0x21) & 0x80) me->pos.y += speed;


	//=================================
	//	カメラ回転
	if (GetKeyState(0x66) & 0x80) me->angle.y += 0.05f;
	else if (GetKeyState(0x64) & 0x80) me->angle.y -= 0.05f;
	if (GetKeyState(0x62) & 0x80) me->angle.x += 0.05f;
	else if (GetKeyState(0x68) & 0x80) me->angle.x -= 0.05f;


	me->target.x = me->pos.x - sinf(me->angle.y);
	me->target.y = me->pos.y - tanf(me->angle.x);
	me->target.z = me->pos.z - cosf(me->angle.y);

	me->Set(me->pos, me->target);
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
	// 角度
#ifndef TYPE1
	me->angle.y += Mouse::Get_axis_x() * .05f;
#endif
#ifndef TYPE2
	float p_angle=me->my_player->Get_angleY();
	me->angle.y = p_angle;
#endif


	//me->angle.x += Mouse::Get_axis_y() * .025f;



	//カメラの上下移動に制限
	if (me->angle.x < -0.2f)me->angle.x = -0.2f;
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
	me->my_player->Get_pos(p_pos);

	p_pos.y += 5.0f;	// 少し上に



	// 角度の値のベクトルとプレイヤーからカメラ位置算出
	me->pos.x = p_pos.x - vec.x;
	me->pos.y = p_pos.y - vec.y;
	me->pos.z = p_pos.z - vec.z;

	me->pos.y += 4.0f;	// 少し上に

	// 注視点はプレイヤー
	me->target = p_pos;


	//壁判定
	//Collision();

	// 目標座標までゆっくり動かす
	me->pos = me->pos * .85f + me->pos * .15f;

	me->Set(me->pos + Vector3(0, 2, 0), me->target + Vector3(0, 5, 0));



	// プレイヤーモードでカメラ切り替え
	if (me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE_FPS) me->Change_mode(MODE_PART::M_FPS);
	else if (me->my_player->Get_action() == BasePlayer::ACTION_PART::REND) me->Change_mode(MODE_PART::M_ZOOM);
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
	//me->angle.x += Mouse::Get_axis_y() *.05f;

	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.4f)me->angle.x = 0.4f;


	// プレイヤー座標・角度取得
	Vector3 p_pos;
	me->my_player->Get_pos(p_pos);
	p_pos.y += 2.5f;	// 少し上に

	float p_angle = me->my_player->Get_angleY();
	me->angle.y = p_angle;

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
	if (me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE) me->Change_mode(MODE_PART::M_TPS);
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
//		「プチズーム」状態処理
//
//*****************************************************************************

void Camera::Mode::Zoom::Initialize()
{
	dist = DIST * .5f;
}

void Camera::Mode::Zoom::Update()
{
#ifndef MOUSE

	// プレイヤー角度取得
	//Vector3 p_angle;
	//me->player_mng->Get_Player()->Get_angle(p_angle);
	//me->angle.y = p_angle.y;
	//me->angle.x += Mouse::Get_axis_y() * .05f;

#endif

	//カメラの上下移動に制限
	//if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	//if (me->angle.x > 0.2f)me->angle.x = 0.2f;

	// オーバーフロー防止
	//if (me->angle.y > PI * 2)me->angle.y -= PI * 2;
	//else if (me->angle.y < -PI * 2)me->angle.y += PI * 2;


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
	me->my_player->Get_pos(p_pos);

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
	if (me->my_player->Get_action() != BasePlayer::ACTION_PART::REND &&
		me->my_player->Get_action() != BasePlayer::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_TPS);
}


Camera *camera;