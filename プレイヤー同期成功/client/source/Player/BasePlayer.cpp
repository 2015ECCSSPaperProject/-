#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"

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
}

BasePlayer::~BasePlayer()
{
	Release();
}

void BasePlayer::Initialize(iexMesh *obj)
{
	// ��{�p�����[�^������
	pos = Vector3(0, 0, 0);
	angle = Vector3(0, 0, 0);
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

	//if (Mouse::isPushLeft())
	//{
	//	m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::LEFT_CLICK;
	//}

	//else if (Mouse::isPushRight())
	//{
	//	m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::RIGHT_CLICK;
	//}

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
	action[(int)action_part]->Update_obj();
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

	//me->model->Animation();
	me->model->SetScale(me->scale);
	me->model->SetAngle(me->angle);
	me->model->SetPos(me->pos);
	me->model->Update();
}

void BasePlayer::Action::Base::Set_motion(int no)
{
	//if (me->model->GetMotion(0) != no)
	//{
	//	me->model->SetMotion(0, no);
	//	me->model->SetMotion(1, no);
	//}
}




//*****************************************************************************
//
//		�uTPS�ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{}

void BasePlayer::Action::Move::Update()
{
	me->Control_all();	// �S�L�[��t
}

void BasePlayer::Action::Move::Render()
{
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
{}

void BasePlayer::Action::Paste::Update()
{}

void BasePlayer::Action::Paste::Render()
{
	me->model->Render();
}


//*****************************************************************************
//
//		�u�|�X�^�[�j��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{}

void BasePlayer::Action::Rend::Update()
{
	me->m_controlDesc.moveFlag = 0;

	// ���N���b�N��t
	//if (Mouse::isPushLeft())
	//{
	//	me->m_controlDesc.moveFlag |= (int)PLAYER_IMPUT::LEFT_CLICK;
	//}
}

void BasePlayer::Action::Rend::Render()
{
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
	me->model->Render();
}



//�@����
BasePlayer* player[PLAYER_MAX];