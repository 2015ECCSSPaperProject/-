#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"

#include	"../poster/Poster_manager.h"

/*	�x�[�X�v���C���[	*/

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
	// ��{�p�����[�^������
	pos.x = -(float)(10 + (m_id * 4));// ��
	angleY = 0.0f;
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 1.5f;
	fallspeed = .1f;
	se_receive = 0;
	isJump = isLand = false;
	jump_pow = 0;

	// 3D����
	model = obj->Clone();

	mynumber = m_id;



	// �s����ԏ�����
	action[(int)ACTION_PART::MOVE] = new BasePlayer::Action::Move(this);
	action[(int)ACTION_PART::MOVE_FPS] = new BasePlayer::Action::MoveFPS(this);
	action[(int)ACTION_PART::ATTACK] = new BasePlayer::Action::Attack(this);
	action[(int)ACTION_PART::PASTE] = new BasePlayer::Action::Paste(this);
	action[(int)ACTION_PART::REND] = new BasePlayer::Action::Rend(this);
	action[(int)ACTION_PART::DIE] = new BasePlayer::Action::Die(this);

	Change_action(ACTION_PART::MOVE);	// �ŏ��͈ړ����
	camera_mode = CAMERA_MODE::TPS;

	// ���[�V�����ԍ�
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
//		�X�V
//*************************************************************************************************************************
void BasePlayer::Update()
{
	CONTROL_DESC controlDesc;

	// �����ɃT�[�o�[�}�l�[�W���[����f�X�N������Ă���
	controlDesc.moveFlag = ServerManager::GetDesc(m_id).moveFlag;

	controlDesc.mouseX = ServerManager::GetDesc(m_id).mouseX;//�}�E�XX
	controlDesc.mouseY = ServerManager::GetDesc(m_id).mouseY;//�}�E�XY


	// �����̑����motion_no���������悤�ɂ����瓮����(�Q�b�^�[�ŕԂ�����)
	controlDesc.motion_no = motion_no = ServerManager::GetDesc(m_id).motion_no;//���[�V�����ԍ�

	// �j���t���O
	controlDesc.rendFlag = ServerManager::GetDesc(m_id).rendFlag;

	// �ڂ���
	controlDesc.controlFlag = ServerManager::GetDesc(m_id).controlFlag;

	//	�R���g���[���[�ɑ���p�����[�^���p�X
	//Control_Update(controlDesc);//���f�X�N�Z�b�g
	action[(int)action_part]->Update(controlDesc);

	

	stage->Collision(pos, &move, 5, 2);
	if (stage->Collision_rand(pos, &move, 0))
	{
		// ��΂Ȃ�����
		pos.y += move.y, move.y = 0;
		isLand = true;
		isJump = false;
	}
	else
	{
		isLand = false;
	}
	// ���W�X�V
	pos += move;


	//	�f�X�N������
	ServerManager::ResetDesc(m_id);

	//���Ńv���C���[�X�V
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
	me->camera_mode = CAMERA_MODE::TPS;
}

void BasePlayer::Action::Move::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	// ADWS
	//if (KEY(KEY_LEFT) == 1) AxisX = -1;
	//else if (KEY(KEY_RIGHT) == 1) AxisX = 1;
	//if (KEY(KEY_UP) == 1) AxisY = 1;
	//else if (KEY(KEY_DOWN) == 1) AxisY = -1;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

	// �W�����v
	if (!me->isJump && me->isLand)
	{
		if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
		{
			me->jump_pow = 2.0f;
			me->isJump = true;
		}
	}
	if (me->isJump)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	//�A���O������	�p�x�␳
	float	work;
	work = _ControlDesc.mouseX *0.000001f;
	if (work > 0.1f) work = 0.1f;
	me->angleY += work;// Angle�ɉ��Z

	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));


	front.Normalize();
	right.Normalize();
	// �ړ��ʌ���
	me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
	me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		//Set_motion(1);
	}
	// �������瓮���Ă���
	else
	{
		// �ړ����[�V����
		//Set_motion(0);


		// �Ȃ����[�V����
		//Set_motion()
	}

	
	me->move += Vector3(0, me->move.y - me->fallspeed, 0);

	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_rend(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::REND);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_paste(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::PASTE);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::ATTACK_BUTTON)
	{
		me->Change_action(ACTION_PART::ATTACK);
	}

	else if (!(_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::TRG_C))
	{
		me->Change_action(ACTION_PART::MOVE_FPS);
	}
}


//*****************************************************************************
//
//		�uFPS�ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::MoveFPS::Initialize()
{
	me->camera_mode = CAMERA_MODE::FPS;
}

void BasePlayer::Action::MoveFPS::Update(const CONTROL_DESC &_ControlDesc)
{

	float AxisX = 0, AxisY = 0;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX += -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY += 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY += -1;

	if (!me->isJump && me->isLand)
	{
		if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
		{
			me->jump_pow = 2.0f;
			me->isJump = true;
		}
	}
	if (me->isJump)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	//�A���O������	�p�x�␳
	float	work;
	work = _ControlDesc.mouseX *0.000001f;
	if (work > 0.1f) work = 0.1f;
	me->angleY += work;// Angle�ɉ��Z

	//	�ړ��x�N�g���ݒ�
	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));
	front.Normalize();
	right.Normalize();

	//	�ړ��ʌ���
	me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
	me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		//Set_motion(1);
	}
	// �������瓮���Ă���
	else
	{
		// �ړ����[�V����
		//Set_motion(0);
	}

	me->move += Vector3(0, me->move.y - me->fallspeed, 0);

	if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_rend(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::REND);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::RIGHT_CLICK)
	{
		me->poster_num = poster_mng->Can_do(me, me->mynumber);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (poster_mng->Can_paste(me->mynumber, me->poster_num))
			{
				me->Change_action(ACTION_PART::PASTE);
				return;
			}
		}
	}
	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::ATTACK_BUTTON)
	{
		me->Change_action(ACTION_PART::ATTACK);
	}

	else if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::TRG_C)
	{
		me->Change_action(ACTION_PART::MOVE);
	}
}


//*****************************************************************************
//
//		�u�U���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{
	me->move = VECTOR_ZERO;
	me->Set_motion(4);
}

void BasePlayer::Action::Attack::Update(const CONTROL_DESC &_ControlDesc)
{
	// ���[�V�����I��
	if (me->model->GetParam(0) == 2)
	{
		(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
		me->Set_motion(0);
	}

	// �j�����[�V�����̃t���[��
	else if (me->model->GetParam(0) == 1)
	{
		// �j������
		int no = PlayerManager::Check_attack(me->m_id);
		if (no != -1)
		{
			player[no]->Change_action(ACTION_PART::DIE);
		}
	}
}


//*****************************************************************************
//
//		�u�|�X�^�[�\��t���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{
	timer = 0;
	me->move = VECTOR_ZERO;
	me->Set_motion(3);

	// �|�X�^�[�ɉ����č��W�ƌ�����ύX
	const static float dist = 5.0f;
	me->pos = poster_mng->Get_pos(me->poster_num);
	me->angleY = poster_mng->Get_angle(me->poster_num) + PI;
	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);
}

void BasePlayer::Action::Paste::Update(const CONTROL_DESC &_ControlDesc)
{
	me->motion_no = 3;
	if (timer++ > 60)
	{
		(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
		me->Set_motion(0);
	}

	if (timer == 45)
	{
		// �\��t���鏈��
		poster_mng->Paste_poster(me->mynumber, me->poster_num);
	}
}


//*****************************************************************************
//
//		�u�|�X�^�[�j��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{
	rended = false;
	me->move = VECTOR_ZERO;
	//me->Set_motion(2);

	// �|�X�^�[�ɉ����č��W�ƌ�����ύX
	const static float dist = 5.0f;
	me->pos = poster_mng->Get_pos(me->poster_num);
	me->angleY = poster_mng->Get_angle(me->poster_num) + PI;
	me->pos += (Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist);
}

void BasePlayer::Action::Rend::Update(const CONTROL_DESC &_ControlDesc)
{
	// �܂��j���Ă��Ȃ�
	if (!rended)
	{
		// �j���R�}���h��ON�Ȃ�
		if (_ControlDesc.rendFlag & (BYTE)REND_FLAG::RIGHT)
		{
			me->motion_no = 2;
			me->Set_motion(2);
			rended = true;
		}
		// �}�E�X�������烂�[�h�߂�
		else if ((_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::LEFT_CLICK) == 0)
		{
			(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
			me->Set_motion(0);
		}
	}
	else // �j����
	{
		me->motion_no = 2;
		me->Set_motion(2);

		// ���[�V�����I��
		if (me->model->GetParam(0) == 2)
		{
			(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
			me->Set_motion(0);
		}

		// �j�����[�V�����̃t���[��
		else if (me->model->GetParam(0) == 1)
		{
			// �j������
			poster_mng->Rend_poster(me->mynumber, me->poster_num);
		}
	}
}


//*****************************************************************************
//
//		�u���ʁv��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{
	die_frame = 0;

	me->move = VECTOR_ZERO;
}

void BasePlayer::Action::Die::Update(const CONTROL_DESC &_ControlDesc)
{
	if (die_frame++ > 60)
	{
		(me->camera_mode == CAMERA_MODE::TPS) ? me->Change_action(ACTION_PART::MOVE) : me->Change_action(ACTION_PART::MOVE_FPS);
	}
}



//*****************************************************************************
//
//		�u���q�R�[�L�v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Hikouki::Initialize()
{}

void BasePlayer::Action::Hikouki::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY = -1;

	// �W�����v
	if (!me->isJump && me->isLand)
	{
		if (_ControlDesc.controlFlag & (BYTE)PLAYER_CONTROL::SPACE)
		{
			me->jump_pow = 2.0f;
			me->isJump = true;
		}
	}
	if (me->isJump)
	{
		me->move.y = me->jump_pow;
		me->jump_pow -= me->fallspeed;
	}

	//�A���O������	�p�x�␳
	float	work;
	work = _ControlDesc.mouseX *0.000001f;
	if (work > 0.1f) work = 0.1f;
	me->angleY += work;// Angle�ɉ��Z


	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));

	//me->angle.y += Mouse::Get_axis_x() * .065f;


	front.Normalize();
	right.Normalize();
	// �ړ��ʌ���
	me->move.x = (front.x*AxisY + right.x*AxisX) * (me->speed);
	me->move.z = (front.z*AxisY + right.z*AxisX) * (me->speed);

	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		//Set_motion(1);
	}
	// �������瓮���Ă���
	else
	{
		// �ړ����[�V����
		//Set_motion(0);


		// �Ȃ����[�V����
		//Set_motion()
	}


	me->move += Vector3(0, me->move.y - me->fallspeed, 0);
}



//*****************************************************************************
//
//		�u���S�C�v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Gun::Initialize()
{}

void BasePlayer::Action::Gun::Update(const CONTROL_DESC &_ControlDesc)
{}

//�@����
BasePlayer* player[PLAYER_MAX];







int PlayerManager::Check_attack(int me)
{
	int no = -1;
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (i == me)continue;

		Vector3 vec(player[i]->Get_pos() - player[me]->Get_pos());

		// ����
		if (vec.Length() < 16)
		{
			vec.Normalize();
			Vector3 front(sinf(player[me]->Get_angleY()), 0, cosf(player[me]->Get_angleY()));

			// �p�x
			if (Vector3Dot(vec, front) > .707f)
			{
				no = i;
				break;
			}
		}
	}
	return no;
}
