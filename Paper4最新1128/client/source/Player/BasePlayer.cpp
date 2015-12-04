#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"

#include	"PlayerManager.h"

#include	"../sound/SoundManager.h"

#include	"../blur/blur.h"

#include	"../Effect/Effect.h"

/*	�x�[�X�v���C���[	*/

//****************************************************************************************************************
//
//  ������
//
//****************************************************************************************************************
BasePlayer::BasePlayer():prev_pos(pos)
{
	//ZeroMemory(models, sizeof(iex3DObj));
	for (int i = 0; i < (int)MODEL::MAX; i++) models[i] = nullptr;

	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.motion_no = 0;
	m_controlDesc.mouseX = .0f;
	m_controlDesc.mouseY = .0f;
	m_controlDesc.rendFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;
	m_controlDesc.skillFlag = 0x0;

	// �G�t�F�N�g������
	EffectInit();
}

BasePlayer::~BasePlayer()
{
	Release();
	EffectRelease();
}

void BasePlayer::Initialize(iex3DObj **objs)
{
	// ��{�p�����[�^������
	pos = Vector3(0, 0, 0);
	angleY = 0.0f;
	scale = .75f;
	move = Vector3(0, 0, 0);
	speed = 5.0f;
	fallspeed = .0f;
	se_receive = 0;

	// ���̑�������
	poster_num = 0;
	toggle_c = true;

	// 3D����
	models[(int)MODEL::NORMAL]	 = objs[(int)PlayerManager::CLONE_TYPE::NORMAL]->Clone();
	models[(int)MODEL::DIE]		 = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::PLANE]	 = objs[(int)PlayerManager::CLONE_TYPE::DIE]->Clone();
	models[(int)MODEL::GUN]		 = objs[(int)PlayerManager::CLONE_TYPE::GUN]->Clone();

	skill_data[(int)SKILL::GUN].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::SYURIKEN].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::KABUTO].do_action = ACTION_PART::GUN;
	skill_data[(int)SKILL::ZENRYOKU].do_action = ACTION_PART::GUN;

	// ��ΒႢ���ɕ��ׂ�
	// �ă��b�N�J�E���g
	skill_data[(int)SKILL::GUN].unlock_rend_count = 0;
	skill_data[(int)SKILL::SYURIKEN].unlock_rend_count = 5;
	skill_data[(int)SKILL::KABUTO].unlock_rend_count = 10;
	skill_data[(int)SKILL::ZENRYOKU].unlock_rend_count = 15;

	// �N�[���^�C��
	skill_data[(int)SKILL::GUN].cool_time = 600;
	skill_data[(int)SKILL::SYURIKEN].cool_time = 300;
	skill_data[(int)SKILL::KABUTO].cool_time = 150;
	skill_data[(int)SKILL::ZENRYOKU].cool_time = 600;

	// �S�����b�N��������
	for (int i = 0; i < (int)SKILL::MAX; i++)
	{
		skill_data[i].unlock = false;
		skill_data[i].wait_time = 0;
	}
	skill_data[(int)SKILL::GUN].unlock = true; // �ŏ��̃X�L���͍ŏ�����g����

	select_skill = SKILL::GUN;


	// �s����ԏ�����
	action[(int)ACTION_PART::MOVE]		 = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_TARGET]= new BasePlayer::Action::MoveTarget(this);
	action[(int)ACTION_PART::ATTACK]	 = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE]		 = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND]		 = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::FREEZE]	 = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::DIE]		 = new BasePlayer::Action::Die(this);
	action[(int)ACTION_PART::RESPAWN]	 = new BasePlayer::Action::Respawn(this);
	action[(int)ACTION_PART::PLANE]		 = new BasePlayer::Action::Hikouki(this);
	action[(int)ACTION_PART::GUN]		 = new BasePlayer::Action::Gun(this);
	action[(int)ACTION_PART::MANHOLE] = new BasePlayer::Action::Manhole(this);
	action[(int)ACTION_PART::THROUGH] = new BasePlayer::Action::Through(this);

	Change_action(ACTION_PART::MOVE);	// �ŏ��͈ړ����

	//	�L�[������
	GetCursorPos(&mousePos);

	// ���[�V�����ԍ�
	motion_no = 0;
}

void BasePlayer::Release()
{
	for (int i = 0; i < (int)MODEL::MAX; i++) SAFE_DELETE(models[i]);

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

	// move�l�Z�o
	move = Vector3(pos - prev_pos);
	prev_pos = pos;

	// �G�t�F�N�g�X�V
	EffectUpdate();

	if (KEY(KEY_B)==3)
	{
		ExplosionAction();
		EffectFireFlour(pos + Get_Flont(), FIRE_COLOR::BLUE, 3);
	}

	// �X�L���Q�[�W�X�V
	for (int i = 0; i < (int)SKILL::MAX; i++)
	{
		if (!skill_data[i].unlock)break;
		(skill_data[i].wait_time > 0) ? skill_data[i].wait_time-- : skill_data[i].wait_time &= 0x00000000;
	}
}


//*************************************************************************************************************************
//		�`��
//*************************************************************************************************************************
void BasePlayer::Render(iexShader *shader, char *name)
{
	if (shader)
	{
		action[(unsigned int)action_part]->Render(shader, name);
	}
	else
	{
		action[(unsigned int)action_part]->Render();
	}
}

//*************************************************************************************************************************
//		�G�t�F�N�g�F�X
//*************************************************************************************************************************
void   BasePlayer::EffectInit()
{
	explosion = new Explosion();

}

void   BasePlayer::EffectRelease()
{
	SAFE_DELETE(explosion);

}

void   BasePlayer::EffectUpdate()
{
	explosion->Update(pos, Get_Flont(), 30);

}

void  BasePlayer::EffectRender()
{
	explosion->Render();

}

/* �g���K�[ */
void BasePlayer::ExplosionAction()
{
	explosion->Action();
	BlurFilter::Set(18, 0, 0);
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

	me->models[(int)me->model_part]->Animation();
	me->models[(int)me->model_part]->SetScale(me->scale);
	me->models[(int)me->model_part]->SetAngle(me->angleY);
	me->models[(int)me->model_part]->SetPos(me->pos);
}

void BasePlayer::Set_motion(int no)
{
	if (models[(int)MODEL::NORMAL]->GetMotion() != no)
	{
		models[(int)MODEL::NORMAL]->SetMotion(no);
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

	me->model_part = MODEL::NORMAL;

}

void BasePlayer::Action::Move::Update()
{
	// �����ɒ���
	me->Set_motion(me->motion_no);

	// �������
	if (me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::DOWN ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::UP ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::LEFT ||
		me->m_controlDesc.moveFlag & (int)PLAYER_IMPUT::RIGHT)
	{
		if (me->se_receive == -1) me->se_receive = se->Play("���s", me->pos, Vector3(me->models[(int)me->model_part]->TransMatrix._31, 0, me->models[(int)me->model_part]->TransMatrix._32), true);
		else se->Set_data(me->se_receive, me->pos, Vector3(me->models[(int)me->model_part]->TransMatrix._31, 0, me->models[(int)me->model_part]->TransMatrix._32), me->move);
	}
	else if (me->se_receive != -1)
	{
		se->Stop(me->se_receive);
		me->se_receive = -1;
	}

	if (me->models[(int)MODEL::NORMAL]->GetFrame() == 265) se->Play("�W�����v", me->pos);


	Update_obj();
}

void BasePlayer::Action::Move::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		�u�^�[�Q�b�g�ړ��ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::MoveTarget::Initialize()
{
	me->m_controlDesc.mouseX = .0f;
	me->m_controlDesc.mouseY = .0f;

	// �ҋ@���[�V�����Z�b�g
	me->Set_motion(1);

	me->model_part = MODEL::NORMAL;
}

void BasePlayer::Action::MoveTarget::Update()
{
	// �����ɒ���
	me->Set_motion(me->motion_no);

	Update_obj();
}

void BasePlayer::Action::MoveTarget::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		�u�U���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->Set_motion(4);

	me->model_part = MODEL::NORMAL;
}

void BasePlayer::Action::Attack::Update()
{
	Update_obj();
}

void BasePlayer::Action::Attack::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}




//*****************************************************************************
//
//		�u�|�X�^�[�\��t���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->Set_motion(3);

	me->model_part = MODEL::NORMAL;
}

void BasePlayer::Action::Paste::Update()
{
	Update_obj();
}

void BasePlayer::Action::Paste::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}


//*****************************************************************************
//
//		�u�|�X�^�[�j��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000; 
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->se_receive = -1;
	me->model_part = MODEL::NORMAL;

	se->Play("�j��\��");
}

void BasePlayer::Action::Rend::Update()
{
	me->m_controlDesc.rendFlag &= 0x00000000;

	// �����ɒ���
	me->Set_motion(me->motion_no);

	// �}�E�X�X���b�V��
	const float mouse_move = sqrtf(me->m_controlDesc.mouseX*me->m_controlDesc.mouseX + me->m_controlDesc.mouseY + me->m_controlDesc.mouseY);
	if (mouse_move > 80000)
	{
		me->m_controlDesc.rendFlag |= (BYTE)REND_FLAG::RIGHT;
	}

	// �j�����[�V�����̃t���[��
	if (me->models[(int)me->model_part]->GetParam(0) == 1)
	{
		// �j������
		if (me->se_receive == -1)
		{
			me->se_receive = se->Play("�j��", me->pos);
		}
	}

	Update_obj();
}

void BasePlayer::Action::Rend::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}

//*****************************************************************************
//
//		�u�ł܂�v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Freeze::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;
}

void BasePlayer::Action::Freeze::Update()
{
}

void BasePlayer::Action::Freeze::Render(iexShader *shader, char *name)
{
	//me->models[(int)me->model_part]->Animation();	// �A�j���[�V�������Ȃ�
	me->models[(int)me->model_part]->SetScale(me->scale);
	me->models[(int)me->model_part]->SetAngle(me->angleY);
	me->models[(int)me->model_part]->SetPos(me->pos);
	me->models[(int)me->model_part]->Update();

	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}

//*****************************************************************************
//
//		�u���ʁv��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	flashing = 0;

	me->model_part = MODEL::DIE;

	// �΂��邩��
	me->models[(int)me->model_part]->SetFrame(0);
}

void BasePlayer::Action::Die::Update()
{
	Update_obj();
}

void BasePlayer::Action::Die::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	flashing++;
	if (flashing < 4) {
		if (shader)
		{
			me->models[(int)me->model_part]->Render(shader, name);
		}
		else
		{
			me->models[(int)me->model_part]->Render();
		}
	}
	else if (flashing > 8){
		flashing = 0;
	}

}


//*****************************************************************************
//
//		�u���X�|�[���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Respawn::Initialize()
{
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	flashing = 0;

	me->model_part = MODEL::NORMAL;

	me->Set_motion(8);
}

void BasePlayer::Action::Respawn::Update()
{
	Update_obj();
}

void BasePlayer::Action::Respawn::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	flashing++;
	if (flashing < 4) {
		if (shader)
		{
			me->models[(int)me->model_part]->Render(shader, name);
		}
		else
		{
			me->models[(int)me->model_part]->Render();
		}
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
{
	me->model_part = MODEL::PLANE;
}

void BasePlayer::Action::Hikouki::Update()
{
	Update_obj();
}

void BasePlayer::Action::Hikouki::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}




//*****************************************************************************
//
//		�u���S�C�v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Gun::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	// ���f����S�C�ɕύX
	me->model_part = MODEL::GUN;

	// 0�t���[���Ƀ��Z�b�g
	me->models[(int)me->model_part]->SetFrame(0);
	me->models[(int)me->model_part]->SetParam(0, 0);
}

void BasePlayer::Action::Gun::Update()
{
	if (me->models[(int)me->model_part]->GetParam(0) == 1)
	{
		//se->Play("���S�C");
		me->ExplosionAction();
		EffectFireFlour(me->pos+me->Get_Flont(), FIRE_COLOR::BLUE, 3);

	}
	Update_obj();
}

void BasePlayer::Action::Gun::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		�u�}���z�[������v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Manhole::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->model_part = MODEL::NORMAL;
	me->models[(int)me->model_part]->SetMotion(0);
}

void BasePlayer::Action::Manhole::Update()
{
	//if (me->models[(int)me->model_part]->GetParam(0) == 1)
	//{
	//se->Play("���S�C");
	//}
	Update_obj();
}

void BasePlayer::Action::Manhole::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	else
	{
		me->models[(int)me->model_part]->Render();
	}
}



//*****************************************************************************
//
//		�u�����ǒʂ蔲���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Through::Initialize()
{
	// ���͏�񏉊���
	me->m_controlDesc.controlFlag &= 0x00000000;
	me->m_controlDesc.moveFlag &= 0x00000000;
	me->m_controlDesc.rendFlag &= 0x00000000;

	me->model_part = MODEL::NORMAL;
	me->models[(int)me->model_part]->SetMotion(0);
}

void BasePlayer::Action::Through::Update()
{
	//if (me->models[(int)me->model_part]->GetParam(0) == 1)
	//{
	//se->Play("���S�C");
	//}
	Update_obj();
}

void BasePlayer::Action::Through::Render(iexShader *shader, char *name)
{
	me->models[(int)me->model_part]->Update();
	if (shader)
	{
		me->models[(int)me->model_part]->Render(shader, name);
	}
	{
		me->models[(int)me->model_part]->Render();
	}
}


//�@����
//BasePlayer* player[PLAYER_MAX];