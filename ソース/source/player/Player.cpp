#include	"iextreme.h"

#include	"../stage/Stage.h"
#include	"../poster/Poster_manager.h"
#include	"Player.h"


Player::Player() : model(nullptr)
{
	ZeroMemory(action, sizeof(Action::Base));
}

Player::~Player()
{
	Release();
}

void Player::Initialize(iex3DObj *obj, Poster_manager *poster_mng, TEAM_COLOR col)
{
	// 基本パラメータ初期化
	pos = Vector3(0, 0, 0);
	angle = Vector3(0, 0, 0);
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 3.0f;
	fallspeed = .0f;

	// 3D実体
	model = obj->Clone();

	// 行動状態初期化
	action[(unsigned int)ACTION_PART::MOVE] = new Player::Action::Move(this);
	action[(unsigned int)ACTION_PART::MOVE_FPS] = new Player::Action::MoveFPS(this);
	action[(unsigned int)ACTION_PART::ATTACK] = new Player::Action::Attack(this);
	action[(unsigned int)ACTION_PART::PASTE] = new Player::Action::Paste(this);
	action[(unsigned int)ACTION_PART::REND] = new Player::Action::Rend(this);
	action[(unsigned int)ACTION_PART::DIE] = new Player::Action::Die(this);

	Change_action(ACTION_PART::MOVE);	// 最初は移動状態
	do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない


	// 色変更
	//switch (col)
	//{
	//	case TEAM_COLOR::ONE:
	//		Set_texture("DATA/CHR/teki_blue.png");
	//		break;

	//	case TEAM_COLOR::TWO:
	//		Set_texture("DATA/CHR/teki_red.png");
	//		break;
	//}
	team_col = col;

	// 委譲くん
	this->poster_mng = poster_mng;
}

void Player::Release()
{
	delete model;

	for (unsigned int i = 0; i < (unsigned int)ACTION_PART::MAX; i++)
	{
		delete action[i], action[i] = nullptr;
	}
}

void Player::Update(Stage *stage)
{
	action[(unsigned int)action_part]->Update();

	fallspeed -= 0.1f;
	move.y = fallspeed;
	stage->Collision(pos, &move, 1.0f, 2);
	if (stage->Collision_rand(pos, &move, .0f))
		fallspeed = .0f;

	action[(unsigned int)action_part]->Update_obj();
}

void Player::Render()
{
	char str[64];

	wsprintf(str, "[X]キーで色切り替え");
	IEX_DrawText(str, 10, 176, 200, 20, 0xFFffff00);

	if (team_col == TEAM_COLOR::ONE)
	{
		wsprintf(str, "今赤色");
		IEX_DrawText(str, 10, 196, 200, 20, 0xFFff0000);
	}
	else
	{
		wsprintf(str, "今青色");
		IEX_DrawText(str, 10, 196, 200, 20, 0xFF0000ff);
	}

	action[(unsigned int)action_part]->Render();
}

void Player::Set_texture(char *filename)
{
	//iexMesh2_textures texture;
	//texture.Create_load(1, filename);
	//obj->Texture_change(texture);
	//texture.Release();
}



//*****************************************************************************
//
//		状態処理ベース関係
//
//*****************************************************************************
void Player::Action::Base::Update_obj()
{
	me->pos += me->move;
	//me->move = Vector3(0, 0, 0);

	me->model->Animation();
	me->model->SetScale(me->scale);
	me->model->SetAngle(me->angle);
	me->model->SetPos(me->pos);
	me->model->Update();
}

void Player::Action::Base::Set_motion(int no)
{
	if (me->model->GetMotion() != no) me->model->SetMotion(no);
}




//*****************************************************************************
//
//		「TPS移動」状態処理
//
//*****************************************************************************

void Player::Action::Move::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 待機モーションセット
	Set_motion(0);
}

void Player::Action::Move::Update()
{
	//	左スティック取得
	float AxisX = (float)KEY_GetAxisX() * .001f;
	float AxisY = (float)-KEY_GetAxisY()* .001f;

	// 微々たるスティックの傾きを排除
	float dist = sqrtf(AxisX * AxisX + AxisY * AxisY);
	if (dist < .35f) {
		AxisX = 0;
		AxisY = 0;
	}
	if (dist > 1.0f)
	{
		AxisX /= dist;
		AxisY /= dist;
	}

	//	移動ベクトル設定
	Vector3 front(matView._13, 0, matView._33);
	Vector3 right(matView._11, 0, matView._31);
	front.Normalize();
	right.Normalize();
	//	移動量決定
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);


	// 止まってる状態
	if (me->move.Length() == 0)
	{
		// 待機モーション
		Set_motion(0);
	}
	// 何かしら動いてる状態
	else
	{
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

		// 移動モーション
		Set_motion(1);
	}


	// Zキーで攻撃
	//if (KEY(KEY_A) == 3) me->Change_action(ACTION_PART::ATTACK);

	// Xキーでポスター貼り付け
	//if (KEY(KEY_B) == 3) me->Change_action(ACTION_PART::PASTE);

	// Zキーでアクション
	if (KEY(KEY_A) == 3)
	{
		me->poster_mng->Do_poster(me, me->team_col);

		switch (me->do_flag)
		{
		case DO_FLAG::NONE:	 // 何もしない
			break;

		case DO_FLAG::ATTACK:// 攻撃
			me->Change_action(ACTION_PART::ATTACK);
			break;

		case DO_FLAG::PASTE:// 貼る
			me->Change_action(ACTION_PART::PASTE);
			break;

		case DO_FLAG::REND:	// 破く
			me->Change_action(ACTION_PART::REND);
			break;
		}
	}

	// Bキーでカラーチェンジ
	if (KEY(KEY_B) == 3) me->Change_color();

	// Cキーでカメラモード切替
	if (KEY(KEY_C) == 3) me->Change_action(ACTION_PART::MOVE_FPS);


	// 3DObj更新
	Update_obj();
}

void Player::Action::Move::Render()
{
	me->model->Render();
}



//*****************************************************************************
//
//		「FPS移動」状態処理
//
//*****************************************************************************

void Player::Action::MoveFPS::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 待機モーションセット
	Set_motion(0);
}

void Player::Action::MoveFPS::Update()
{
	//===============================================
	//	方向転換(右スティック)
	//===============================================

	//	右スティック取得
	float AxisX = (float)KEY_GetAxisX2() * .001f;
	float AxisY = (float)-KEY_GetAxisY2()* .001f;

	// 微々たるスティックの傾きを排除
	float dist = sqrtf(AxisX * AxisX + AxisY * AxisY);
	if (dist < .35f) {
		AxisX = 0;
		AxisY = 0;
	}
	if (dist > 1.0f)
	{
		AxisX /= dist;
		AxisY /= dist;
	}

	me->angle.y += AxisX * .05f;



	//===============================================
	//	移動(左スティック)
	//===============================================

	//	左スティック取得
	AxisX = (float)KEY_GetAxisX() * .001f;
	AxisY = (float)-KEY_GetAxisY()* .001f;

	// 微々たるスティックの傾きを排除
	dist = sqrtf(AxisX * AxisX + AxisY * AxisY);
	if (dist < .35f) {
		AxisX = 0;
		AxisY = 0;
	}
	if (dist > 1.0f)
	{
		AxisX /= dist;
		AxisY /= dist;
	}

	//	移動ベクトル設定
	Vector3 front(sinf(me->angle.y), 0, cosf(me->angle.y));
	Vector3 right(sinf(me->angle.y + PI * .5f), 0, cosf(me->angle.y + PI * .5f));
	front.Normalize();
	right.Normalize();

	//	移動量決定
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);

	// 止まってる状態
	if (me->move.Length() == 0)
	{
		// 待機モーション
		Set_motion(0);
	}
	// 何かしら動いてる状態
	else
	{
		// 移動モーション
		Set_motion(1);
	}


	//// Zキーで攻撃
	//if (KEY(KEY_A) == 3) me->Change_action(ACTION_PART::ATTACK);

	//// Xキーでポスター貼り付け
	//if (KEY(KEY_B) == 3) me->Change_action(ACTION_PART::PASTE);


	// Zキーでアクション
	if (KEY(KEY_A) == 3)
	{

	}

	// Cキーでカメラモード切替
	if (KEY(KEY_C) == 3) me->Change_action(ACTION_PART::MOVE);


	// 3DObj更新
	Update_obj();
}

void Player::Action::MoveFPS::Render()
{
	me->model->Render();
}



//*****************************************************************************
//
//		「攻撃」状態処理
//
//*****************************************************************************

void Player::Action::Attack::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 移動ストップ
	me->move = Vector3(0, 0, 0);

	// 攻撃モーションセット
	Set_motion(8);
}

void Player::Action::Attack::Update()
{
	// 攻撃モーション終了
	if (me->model->GetParam(0) == 1)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 3DObj更新
	Update_obj();
}

void Player::Action::Attack::Render()
{
	me->model->Render();
}




//*****************************************************************************
//
//		「ポスター貼り付け」状態処理
//
//*****************************************************************************

void Player::Action::Paste::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 移動ストップ
	me->move = Vector3(0, 0, 0);

	// 貼り付けモーションセット
	Set_motion(9);
}

void Player::Action::Paste::Update()
{
	// 貼り付けモーション終了
	if (me->model->GetParam(0) == 3)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 貼り付けフレーム
	if (me->model->GetParam(0) == 1)
	{
		//me->poster_mng->Do_poster(0, me->team_col);
		//me->poster_mng->Do_poster(0, TEAM_COLOR::ONE);
	}

	// 3DObj更新
	Update_obj();
}

void Player::Action::Paste::Render()
{
	me->model->Render();
}


//*****************************************************************************
//
//		「ポスター破る」状態処理
//
//*****************************************************************************

void Player::Action::Rend::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 移動ストップ
	me->move = Vector3(0, 0, 0);

	// 破りモーションセット
	Set_motion(9);
}

void Player::Action::Rend::Update()
{
	// 破りモーション終了
	if (me->model->GetParam(0) == 3)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 破るフレーム
	if (me->model->GetParam(0) == 1)
	{
		//me->poster_mng->Do_poster(0, me->team_col);
		//me->poster_mng->Do_poster(0, TEAM_COLOR::TWO);
	}

	// 3DObj更新
	Update_obj();
}

void Player::Action::Rend::Render()
{
	me->model->Render();
}


//*****************************************************************************
//
//		「死ぬ」状態処理
//
//*****************************************************************************

void Player::Action::Die::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 死んでる時間
	die_frame = 120;

	// 移動ストップ
	me->move = Vector3(0, 0, 0);

	// 死ぬモーションセット
	Set_motion(4);
}

void Player::Action::Die::Update()
{
	if (--die_frame < 0)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 3DObj更新
	Update_obj();
}

void Player::Action::Die::Render()
{
	me->model->Render();
}