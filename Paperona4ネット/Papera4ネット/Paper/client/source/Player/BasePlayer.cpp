#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"

#include	"../../IEX/OKB.h"

/*	ベースプレイヤー	*/

//****************************************************************************************************************
//
//  初期化
//
//****************************************************************************************************************
BasePlayer::BasePlayer()
{
	model = nullptr;

	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.motion_no = 0;
	m_controlDesc.mouseX = .0f;
	m_controlDesc.mouseY = .0f;

}

BasePlayer::~BasePlayer()
{
	Release();
}

void BasePlayer::Initialize(iex3DObj *obj)
{
	// 基本パラメータ初期化
	pos = Vector3(0, 0, 0);
	angleY = 0.0f;
	scale = 1.0f;
	move = Vector3(0, 0, 0);
	speed = 5.0f;
	fallspeed = .0f;
	se_receive = 0;

	// その他初期化
	poster_num = 0;
	camera_mode = CAMERA_MODE::TPS;

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

	//	キー初期化
	GetCursorPos(&mousePos);

	// モーション番号
	motion_no = 0;
}

void BasePlayer::Release()
{
	SAFE_DELETE(model);

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
}

/*	操作　*/
void BasePlayer::Control_all()
{
	m_controlDesc.moveFlag = 0;

	if (KEY_Get(KEY_UP))
	{
		m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::UP;
	}
	else if (KEY_Get(KEY_DOWN))
	{
		m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::DOWN;
	}
	if (KEY_Get(KEY_RIGHT))
	{
		m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::RIGHT;
	}
	else if (KEY_Get(KEY_LEFT))
	{
		m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::LEFT;
	}

	if (KeyBoard(MOUSE_LEFT))
	{
		m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::LEFT_CLICK;
	}

	else if (KeyBoard(MOUSE_RIGHT))
	{
		m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::RIGHT_CLICK;
	}

	//if (Mouse::isPushCenter())
	//{
	//	m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::CENTER_CLICK;
	//}

}



//*************************************************************************************************************************
//		描画
//*************************************************************************************************************************
void BasePlayer::Render()
{
	action[(unsigned int)action_part]->Render();
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

	// 扱いに注意
	me->Set_motion(me->motion_no);

	me->model->Animation();
	me->model->SetScale(me->scale);
	me->model->SetAngle(me->angleY);
	me->model->SetPos(me->pos);
	me->model->Update();
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
}

void BasePlayer::Action::Move::Update()
{
	me->Control_all();	// 全キー受付

	if (me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::RIGHT ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::LEFT ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::UP ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::DOWN
		)
	{
		me->m_controlDesc.motion_no = 0;	// ここで送るモーションの番号を変更
	}
	else
	{
		me->m_controlDesc.motion_no = 1;	// ここで送るモーションの番号を変更
	}
}

void BasePlayer::Action::Move::Render()
{
	Update_obj();
	//me->model->Render(shader,"toon");
	me->model->Render();
}



//*****************************************************************************
//
//		「FPS移動」状態処理
//
//*****************************************************************************

void BasePlayer::Action::MoveFPS::Initialize()
{
	//me->do_flag = DO_FLAG::NONE;	// Zキー押しても何もしない

	// 待機モーションセット
	//Set_motion(0);
}

void BasePlayer::Action::MoveFPS::Update()
{
	me->Control_all();	// 全キー受付
}

void BasePlayer::Action::MoveFPS::Render()
{
	Update_obj();
	me->model->Render();
}



//*****************************************************************************
//
//		「攻撃」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{}

void BasePlayer::Action::Attack::Update()
{}

void BasePlayer::Action::Attack::Render()
{
	me->model->Render();
}




//*****************************************************************************
//
//		「ポスター貼り付け」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	timer = 0;
	me->m_controlDesc.moveFlag = 0;	// 超大事

	me->Set_motion(3);
}

void BasePlayer::Action::Paste::Update()
{
	if (timer++ > 120)
	{
		me->m_controlDesc.motion_no = 1;
		me->Change_action(ACTION_PART::MOVE);
	}
}

void BasePlayer::Action::Paste::Render()
{
	me->model->Animation();
	me->model->SetScale(me->scale);
	me->model->SetAngle(me->angleY);
	me->model->SetPos(me->pos);
	me->model->Update();
	me->model->Render();
}


//*****************************************************************************
//
//		「ポスター破る」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{
	me->m_controlDesc.moveFlag = 0;	// 超大事

	me->Set_motion(2);
}

void BasePlayer::Action::Rend::Update()
{

}

void BasePlayer::Action::Rend::Render()
{
	me->model->Animation();
	me->model->SetScale(me->scale);
	me->model->SetAngle(me->angleY);
	me->model->SetPos(me->pos);
	me->model->Update();
	me->model->Render();
}


//*****************************************************************************
//
//		「死ぬ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{}

void BasePlayer::Action::Die::Update()
{}

void BasePlayer::Action::Die::Render()
{
	Update_obj();
	me->model->Render();
}






//*****************************************************************************
//
//		「紙ヒコーキ」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Hikouki::Initialize()
{}

void BasePlayer::Action::Hikouki::Update()
{}

void BasePlayer::Action::Hikouki::Render()
{
	Update_obj();
	me->model->Render();
}




//*****************************************************************************
//
//		「紙鉄砲」状態処理
//
//*****************************************************************************

void BasePlayer::Action::Gun::Initialize()
{}

void BasePlayer::Action::Gun::Update()
{}

void BasePlayer::Action::Gun::Render()
{
	Update_obj();
	me->model->Render();
}



//　実態
BasePlayer* player[PLAYER_MAX];