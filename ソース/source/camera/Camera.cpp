
#include	"Camera.h"

//#include	"../scene/sceneMain.h"
#include	"../player/Player_manager.h"
#include	"../player/Player.h"

#include	"../Mouse/Mouse.h"

// �J�����˃v���C���[�̊�{����
const float Camera::Mode::Base::DIST = 20.0f;


Camera::Camera() : iexView()
{
	// ��{�p�����[�^������
	pos = Vector3(0, 10.0f, -20.0f);
	target = Vector3(0, 0, 0);
	ipos = Vector3(0, 10.0f, -20.0f);
	itarget = Vector3(0, 0, 0);
	angle = Vector3(0, 0, 0);

	// �s����ԏ�����
	mode[MODE_PART::M_TPS] = new Camera::Mode::TPS(this);
	mode[MODE_PART::M_FPS] = new Camera::Mode::FPS(this);
	mode[MODE_PART::M_STOP] = new Camera::Mode::Stop(this);
	mode[MODE_PART::M_PASTE] = new Camera::Mode::Paste(this);

	Change_mode(MODE_PART::M_TPS);	// �ŏ��͎O�l��
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
//		��ԏ����x�[�X�֌W
//
//*****************************************************************************
void Camera::Mode::Base::Collision()
{
	//extern iexMesh* hitStage;

	//Vector3 Out, ray_Pos, Vec;

	////���C���΂����_�͒����_
	//ray_Pos = target;

	////�x�N�g���͂��̒����_����J�������g�̍��W
	//Vec = pos - target;
	//Vec.Normalize();

	//float Dist = 500.0f;

	//ray_Pos -= Vec;
	//if (hitStage->RayPick(&Out, &ray_Pos, &Vec, &Dist) != -1){

	//	//���������_����ǂ̋�����radius��菬�����ꍇ
	//	if ((Out - target).Length() < (pos - target).Length()) pos = Out;
	//}
}

//*****************************************************************************
//
//		�u3�l�̎��_�v��ԏ���
//
//*****************************************************************************

void Camera::Mode::TPS::Initialize()
{
	me->angle.x = 0;
	dist = DIST;
}

void Camera::Mode::TPS::Update()
{
#ifndef MOUSE

	// �v���C���[�p�x�擾
	Vector3 p_angle;
	me->player_mng->Get_Player()->Get_angle(p_angle);
	me->angle.y = p_angle.y;
	me->angle.x += Mouse::Get_axis_y() * .05f;

#endif

	//�J�����̏㉺�ړ��ɐ���
	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.2f)me->angle.x = 0.2f;

	// �I�[�o�[�t���[�h�~
	if (me->angle.y > PI*2)me->angle.y -= PI*2;
	else if (me->angle.y < -PI*2)me->angle.y += PI*2;


	// �p�x�̒l�ɂ��x�N�g�����쐬
	float ay_x = sinf(me->angle.y);
	float ay_z = cosf(me->angle.y);
	float ax_y = sinf(me->angle.x);
	float ax_z = cosf(me->angle.x);

	Vector3 vec(
		ay_x * ax_z,
		ax_y,
		ay_z * ax_z);

	// �x�N�g���̒�������
	vec *= this->dist;


	// �v���C���[���W�擾
	Vector3 p_pos;
	me->player_mng->Get_Player()->Get_pos(p_pos);

	p_pos.y += 2.5f;	// �������



	// �p�x�̒l�̃x�N�g���ƃv���C���[����J�����ʒu�Z�o
	me->ipos.x = p_pos.x - vec.x;
	me->ipos.y = p_pos.y - vec.y;
	me->ipos.z = p_pos.z - vec.z;

	me->ipos.y += 4.0f;	// �������

	// �����_�̓v���C���[
	me->target = p_pos;


	//�ǔ���
	//Collision();

	// �ڕW���W�܂ł�����蓮����
	me->pos = me->pos * .85f + me->ipos * .15f;

	me->Set(me->pos, me->target);



	// �v���C���[���[�h�ŃJ�����؂�ւ�
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE_FPS) me->Change_mode(MODE_PART::M_FPS);
	//else if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_PASTE);
}


//*****************************************************************************
//
//		�u1�l�̎��_�v��ԏ���
//
//*****************************************************************************

void Camera::Mode::FPS::Initialize()
{}

void Camera::Mode::FPS::Update()
{
#ifndef MOUSE
	me->angle.x += Mouse::Get_axis_y() *.05f;
#endif


	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.4f)me->angle.x = 0.4f;


	// �v���C���[���W�E�p�x�擾
	Vector3 p_pos, p_angle;
	me->player_mng->Get_Player()->Get_pos(p_pos);
	p_pos.y += 2.5f;	// �������

	me->player_mng->Get_Player()->Get_angle(p_angle);
	me->angle.y = p_angle.y;

	// �p�x�̒l�ɂ��x�N�g�����쐬
	float ay_x = sinf(me->angle.y);
	float ay_z = cosf(me->angle.y);
	float ax_y = sinf(me->angle.x);
	float ax_z = cosf(me->angle.x);

	Vector3 vec(
		ay_x * ax_z,
		ax_y,
		ay_z * ax_z);

	// ���W�ݒ�
	me->pos = p_pos;
	me->pos.x = p_pos.x - (vec.x*-0.5f);
	me->pos.y = p_pos.y - (vec.y*-0.5f);
	me->pos.z = p_pos.z - (vec.z*-0.5f);

	me->pos.y += 1.25f;	// �������



	me->target.x = me->pos.x + (sinf(me->angle.y));
	me->target.y = me->pos.y + (tanf(me->angle.x));
	me->target.z = me->pos.z + (cosf(me->angle.y));

	me->Set(me->pos, me->target);


	// �v���C���[���[�h�ŃJ�����؂�ւ�
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE) me->Change_mode(MODE_PART::M_TPS);
	//else if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_PASTE);
}




//*****************************************************************************
//
//		�u�~�܂��Ă�v��ԏ���
//
//*****************************************************************************

void Camera::Mode::Stop::Initialize()
{}

void Camera::Mode::Stop::Update()
{
	// �������Ȃ�
	me->Set(me->pos, me->target);
}


//*****************************************************************************
//
//		�u�v���C���[���|�X�^�[�𒣂�t���Ă�v��ԏ���
//
//*****************************************************************************

void Camera::Mode::Paste::Initialize()
{
	dist = DIST * .5f;
}

void Camera::Mode::Paste::Update()
{
#ifndef MOUSE

	// �v���C���[�p�x�擾
	Vector3 p_angle;
	me->player_mng->Get_Player()->Get_angle(p_angle);
	me->angle.y = p_angle.y;
	me->angle.x += Mouse::Get_axis_y() * .05f;

#endif

	//�J�����̏㉺�ړ��ɐ���
	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.2f)me->angle.x = 0.2f;

	// �I�[�o�[�t���[�h�~
	if (me->angle.y > PI * 2)me->angle.y -= PI * 2;
	else if (me->angle.y < -PI * 2)me->angle.y += PI * 2;


	// �p�x�̒l�ɂ��x�N�g�����쐬
	float ay_x = sinf(me->angle.y);
	float ay_z = cosf(me->angle.y);
	float ax_y = sinf(me->angle.x);
	float ax_z = cosf(me->angle.x);

	Vector3 vec(
		ay_x * ax_z,
		ax_y,
		ay_z * ax_z);

	// �x�N�g���̒�������
	vec *= this->dist;


	// �v���C���[���W�擾
	Vector3 p_pos;
	me->player_mng->Get_Player()->Get_pos(p_pos);

	p_pos.y += 2.5f;	// �������



	// �p�x�̒l�̃x�N�g���ƃv���C���[����J�����ʒu�Z�o
	me->ipos.x = p_pos.x - vec.x;
	me->ipos.y = p_pos.y - vec.y;
	me->ipos.z = p_pos.z - vec.z;

	me->ipos.y += 4.0f;	// �������

	// �����_�̓v���C���[
	me->target = p_pos;


	//�ǔ���
	//Collision();

	// �ڕW���W�܂ł�����蓮����
	me->pos = me->pos * .85f + me->ipos * .15f;

	// �������Ȃ�
	me->Set(me->pos, me->target);

	// �v���C���[���[�h��TPS�Ȃ�J�����؂�ւ�
	if (me->player_mng->Get_Player()->Get_action() != Player::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_TPS);
}