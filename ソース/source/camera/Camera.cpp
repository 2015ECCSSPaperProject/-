
#include	"Camera.h"

//#include	"../scene/sceneMain.h"
#include	"../player/Player_manager.h"
#include	"../player/Player.h"

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
	dist = 20.0f;
}

void Camera::Mode::TPS::Update()
{
	// ���X���r��
	float axis_x = (float)KEY_GetAxisX2()*.001f;
	float axis_y = (float)KEY_GetAxisY2()*.001f;
	float dist = sqrtf(axis_x * axis_x + axis_y * axis_y);
	if (dist < .35f) {
		axis_x = 0;
		axis_y = 0;
	}
	if (dist > 1.f)
	{
		axis_x /= dist;
		axis_y /= dist;
	}
	me->angle.y += axis_x*.05f;
	me->angle.x -= axis_y*.05f;

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



	// �v���C���[���[�h��FPS�Ȃ�J�����؂�ւ�
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE_FPS) me->Change_mode(MODE_PART::M_FPS);
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
	// �E�X�e�B�b�N�擾
	float axis_x = (float)KEY_GetAxisX2()*.001f;
	float axis_y = (float)KEY_GetAxisY2()*.001f;
	float dist = sqrtf(axis_x * axis_x + axis_y * axis_y);
	if (dist < .35f) {
		axis_x = 0;
		axis_y = 0;
	}
	if (dist > 1.f)
	{
		axis_x /= dist;
		axis_y /= dist;
	}

	me->angle.x += axis_y*.05f;
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


	// �v���C���[���[�h��TPS�Ȃ�J�����؂�ւ�
	if (me->player_mng->Get_Player()->Get_action() == Player::ACTION_PART::MOVE) me->Change_mode(MODE_PART::M_TPS);
}