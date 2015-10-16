#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"

#include	"../../IEX/OKB.h"

/*	�x�[�X�v���C���[	*/

//****************************************************************************************************************
//
//  ������
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
	// ��{�p�����[�^������
	pos = Vector3(0, 0, 0);
	angleY = 0.0f;
	scale = 1.0f;
	move = Vector3(0, 0, 0);
	speed = 5.0f;
	fallspeed = .0f;
	se_receive = 0;

	// ���̑�������
	poster_num = 0;
	camera_mode = CAMERA_MODE::TPS;

	// 3D����
	model = obj->Clone();

	// �s����ԏ�����
	action[(int)ACTION_PART::MOVE] = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_FPS] = new BasePlayer::Action::MoveFPS(this);
	action[(int)ACTION_PART::ATTACK] = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE] = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND] = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::DIE] = new BasePlayer::Action::Die(this);

	Change_action(ACTION_PART::MOVE);	// �ŏ��͈ړ����

	//	�L�[������
	GetCursorPos(&mousePos);

	// ���[�V�����ԍ�
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
//		�X�V
//*************************************************************************************************************************

void BasePlayer::Update()
{
	action[(int)action_part]->Update();
}

/*	����@*/
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
//		�`��
//*************************************************************************************************************************
void BasePlayer::Render()
{
	action[(unsigned int)action_part]->Render();
}


//*****************************************************************************
//
//		��ԏ����x�[�X�֌W
//
//*****************************************************************************
void BasePlayer::Action::Base::Update_obj()
{
	//me->pos += me->move;
	//me->move = Vector3(0, 0, 0);

	// �����ɒ���
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
//		�uTPS�ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{
}

void BasePlayer::Action::Move::Update()
{
	me->Control_all();	// �S�L�[��t

	if (me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::RIGHT ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::LEFT ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::UP ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::DOWN
		)
	{
		me->m_controlDesc.motion_no = 0;	// �����ő��郂�[�V�����̔ԍ���ύX
	}
	else
	{
		me->m_controlDesc.motion_no = 1;	// �����ő��郂�[�V�����̔ԍ���ύX
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
//		�uFPS�ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::MoveFPS::Initialize()
{
	//me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// �ҋ@���[�V�����Z�b�g
	//Set_motion(0);
}

void BasePlayer::Action::MoveFPS::Update()
{
	me->Control_all();	// �S�L�[��t
}

void BasePlayer::Action::MoveFPS::Render()
{
	Update_obj();
	me->model->Render();
}



//*****************************************************************************
//
//		�u�U���v��ԏ���
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
//		�u�|�X�^�[�\��t���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	timer = 0;
	me->m_controlDesc.moveFlag = 0;	// ���厖

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
//		�u�|�X�^�[�j��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{
	me->m_controlDesc.moveFlag = 0;	// ���厖

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
//		�u���ʁv��ԏ���
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
//		�u���q�R�[�L�v��ԏ���
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
//		�u���S�C�v��ԏ���
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



//�@����
BasePlayer* player[PLAYER_MAX];