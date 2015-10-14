#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"

#include	"../../../share_data/Enum_public.h"
#include	"../stage/Stage.h"

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
	speed = 1.0f;
	fallspeed = .1f;
	se_receive = 0;

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


	//	�R���g���[���[�ɑ���p�����[�^���p�X
	//Control_Update(controlDesc);//���f�X�N�Z�b�g
	action[(int)action_part]->Update(controlDesc);

	

	stage->Collision(pos, &move, 5, 2);
	if (stage->Collision_rand(pos, &move, 0))
	{
		// ��΂Ȃ�����
		//pos.y += move.y, move.y = 0;
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


//*****************************************************************************
//
//		�uTPS�ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Move::Initialize()
{}

void BasePlayer::Action::Move::Update(const CONTROL_DESC &_ControlDesc)
{
	float AxisX = 0, AxisY = 0;

	// ADWS
	//if (KEY(KEY_LEFT) == 1) AxisX = -1;
	//else if (KEY(KEY_RIGHT) == 1) AxisX = 1;
	//if (KEY(KEY_UP) == 1) AxisY = 1;
	//else if (KEY(KEY_DOWN) == 1) AxisY = -1;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY = -1;


	//�A���O������	�p�x�␳
	float	work;
	work = _ControlDesc.mouseX *0.000001f;
	if (work > 0.1f) work = 0.1f;
	me->angleY += work;// Angle�ɉ��Z

#ifndef TYPE1

	Vector3 front(matView._13, 0, matView._33);
	Vector3 right(matView._11, 0, matView._31);

	me->angle.y += AxisX * .05f;

#endif

#ifndef TYPE2

	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));

	//me->angle.y += Mouse::Get_axis_x() * .065f;

#endif


	front.Normalize();
	right.Normalize();
	// �ړ��ʌ���
	me->move = (front*AxisY + right*AxisX) * (me->speed) +
		Vector3(0, me->move.y - me->fallspeed, 0);


	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		//Set_motion(1);
	}
	// �������瓮���Ă���
	else
	{

#ifndef TYPE1
		//	���E����
		float x1 = me->move.x, z1 = me->move.z;
		float x2 = sinf(me->angle.y), z2 = cosf(me->angle.y);
		//	�O��
		float g = x1*z2 - x2*z1;
		//	�␮�ʒ���	
		float d = sqrtf(x1*x1 + z1*z1);
		float n = (x1*x2 + z1*z2) / d;
		float adjust = (1 - n) * 2.0f;
		if (adjust > 0.3f) adjust = 0.3f;
		//	�����]��
		if (g < 0) me->angle.y -= adjust;
		else me->angle.y += adjust;
#endif


		// �ړ����[�V����
		//Set_motion(0);


		// �Ȃ����[�V����
		//Set_motion()
	}
}


//*****************************************************************************
//
//		�uFPS�ړ��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::MoveFPS::Initialize()
{
	// �ҋ@���[�V�����Z�b�g
	//Set_motion(0);
}

void BasePlayer::Action::MoveFPS::Update(const CONTROL_DESC &_ControlDesc)
{

	float AxisX = 0, AxisY = 0;

	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::LEFT) AxisX = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::RIGHT) AxisX = 1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::UP) AxisY = -1;
	if (_ControlDesc.moveFlag & (BYTE)PLAYER_IMPUT::DOWN) AxisY = -1;

	//me->angle.y += Mouse::Get_axis_x() * .05f;

	//	�ړ��x�N�g���ݒ�
	Vector3 front(sinf(me->angleY), 0, cosf(me->angleY));
	Vector3 right(sinf(me->angleY + PI * .5f), 0, cosf(me->angleY + PI * .5f));
	front.Normalize();
	right.Normalize();

	//	�ړ��ʌ���
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);

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
}


//*****************************************************************************
//
//		�u�U���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Attack::Initialize()
{}

void BasePlayer::Action::Attack::Update(const CONTROL_DESC &_ControlDesc)
{}


//*****************************************************************************
//
//		�u�|�X�^�[�\��t���v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Paste::Initialize()
{}

void BasePlayer::Action::Paste::Update(const CONTROL_DESC &_ControlDesc)
{}


//*****************************************************************************
//
//		�u�|�X�^�[�j��v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Rend::Initialize()
{}

void BasePlayer::Action::Rend::Update(const CONTROL_DESC &_ControlDesc)
{

}


//*****************************************************************************
//
//		�u���ʁv��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Die::Initialize()
{}

void BasePlayer::Action::Die::Update(const CONTROL_DESC &_ControlDesc)
{}



//*****************************************************************************
//
//		�u���q�R�[�L�v��ԏ���
//
//*****************************************************************************

void BasePlayer::Action::Hikouki::Initialize()
{}

void BasePlayer::Action::Hikouki::Update(const CONTROL_DESC &_ControlDesc)
{}



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