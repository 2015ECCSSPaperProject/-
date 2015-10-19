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
	m_controlDesc.rendFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;
}

BasePlayer::~BasePlayer()
{
	Release();
}

void BasePlayer::Initialize(iex3DObj *obj ,iex3DObj *die)
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
	die_model = die->Clone();

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
	SAFE_DELETE(die_model);

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
	// ������
	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;

	if (KEY_Get(KEY_UP) == 1)
	{
		m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::UP;
	}
	else if (KEY_Get(KEY_DOWN) == 1)
	{
		m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::DOWN;
	}
	if (KEY_Get(KEY_RIGHT) == 1)
	{
		m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::RIGHT;
	}
	else if (KEY_Get(KEY_LEFT) == 1)
	{
		m_controlDesc.moveFlag |= (BYTE)PLAYER_IMPUT::LEFT;
	}

	//if (GetAsyncKeyState(0x01) & 0x8000)
	if (KeyBoard(MOUSE_LEFT))
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::LEFT_CLICK;
	}

	else if (KeyBoard(MOUSE_RIGHT))
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::RIGHT_CLICK;
	}
	else if (KeyBoard(MOUSE_CENTAR))
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::ATTACK_BUTTON;
	}

	if (KeyBoard(KB_SPACE))
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::SPACE;
	}

	//if (KeyBoardTRG(KB_C))
	if (GetKeyState('C') & 0x1)	// �g�O��
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;
	}

}



//*************************************************************************************************************************
//		�`��
//*************************************************************************************************************************
void BasePlayer::Render()
{
	action[(unsigned int)action_part]->Render();

	if (m_controlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		Text::Draw(0, 600, 0xff000000, "�N���C�A���g:���N���b�N������");
	}

	if (m_controlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		Text::Draw(0, 560, 0xff000000, "�N���C�A���g:�E�N���b�N������");
	}
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
	me->m_controlDesc.mouseX = .0f;
	me->m_controlDesc.mouseY = .0f;

	// �ҋ@���[�V�����Z�b�g
	me->Set_motion(1);
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
	me->m_controlDesc.mouseX = .0f;
	me->m_controlDesc.mouseY = .0f;

	// �ҋ@���[�V�����Z�b�g
	me->Set_motion(1);
}

void BasePlayer::Action::MoveFPS::Update()
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
{
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->Set_motion(4);
}

void BasePlayer::Action::Attack::Update()
{
	if (GetKeyState('C') & 0x1)	// �g�O��
	{
		me->m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;
	}
}

void BasePlayer::Action::Attack::Render()
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
//		�u�|�X�^�[�\��t���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->Set_motion(3);
}

void BasePlayer::Action::Paste::Update()
{
	if (GetKeyState('C') & 0x1)	// �g�O��
	{
		me->m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;
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
	me->m_controlDesc.controlFlag &= 0x00000000; 
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	//me->Set_motion(2);
	me->Set_motion(1);
}

void BasePlayer::Action::Rend::Update()
{
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	if (me->motion_no == 2)
	{
		me->Set_motion(2);
	}

	if (GetKeyState('C') & 0x1)	// �g�O��
	{
		me->m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;
	}

	// �������ϒ�
	if (GetAsyncKeyState(0x01) & 0x8000)
	{
		me->m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::LEFT_CLICK;
	}


	// �}�E�X�X���b�V��
	const float mouse_move = sqrtf(me->m_controlDesc.mouseX*me->m_controlDesc.mouseX + me->m_controlDesc.mouseY + me->m_controlDesc.mouseY);
	if (mouse_move > 80000)
	{
		me->m_controlDesc.rendFlag |= (BYTE)REND_FLAG::RIGHT;
	}
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
{
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	die_frame = 0;
	flashing = 0;

	// �΂��邩��
	me->die_model->SetFrame(0);
}

void BasePlayer::Action::Die::Update()
{
	if (GetKeyState('C') & 0x1)	// �g�O��
	{
		me->m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;
	}
}

void BasePlayer::Action::Die::Render()
{
	me->die_model->Animation();
	me->die_model->SetScale(me->scale);
	me->die_model->SetAngle(me->angleY);
	me->die_model->SetPos(me->pos);
	me->die_model->Update();

	flashing++;
	if (flashing < 4) {
		me->die_model->Render();
	}
	else if (flashing > 8){
		flashing = 0;
	}

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