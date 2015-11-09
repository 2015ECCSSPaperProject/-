
#include	"Camera.h"
#include	"../system/system.h"
//#include	"../scene/sceneMain.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/MyPlayer.h"

#include	"../stage/Stage.h"

//#include	"../Mouse/Mouse.h"

// �J�����˃v���C���[�̊�{����
const float Camera::Mode::Base::DIST = 70.0f;

Camera::Camera() : iexView(), collision_stage(nullptr)
{
	
}

Camera::~Camera()
{
	for (int i = 0; i < MODE_PART::M_MAX; i++)
	{
		delete mode[i];
	}
	delete collision_stage;
}

void Camera::Initialize(BasePlayer *my)
{
	// ��{�p�����[�^������
	pos = Vector3(0, 10.0f, -20.0f);
	target = Vector3(0, 0, 0);
	ipos = Vector3(0, 10.0f, -20.0f);
	itarget = Vector3(0, 0, 0);
	angle = Vector3(0, 0, 0);

	// �s����ԏ�����
	mode[MODE_PART::M_DEBUG] = new Camera::Mode::Debug(this);
	mode[MODE_PART::M_TPS] = new Camera::Mode::TPS(this);
	mode[MODE_PART::M_FPS] = new Camera::Mode::FPS(this);
	mode[MODE_PART::M_STOP] = new Camera::Mode::Stop(this);
	mode[MODE_PART::M_ZOOM] = new Camera::Mode::Zoom(this);

	Change_mode(MODE_PART::M_TPS);	// �ŏ��͎O�l��
	//Change_mode(MODE_PART::M_DEBUG);	// �f�o�b�O�J����
	my_player = my;

	collision_stage = new iexMesh("DATA/MATI/stage_atari.IMO");
}

void Camera::Update()
{
	mode[mode_part]->Update();
	shader->SetValue("ViewPos", pos);
}


//*****************************************************************************
//
//		��ԏ����x�[�X�֌W
//
//*****************************************************************************
void Camera::Mode::Base::Collision()
{
	Vector3 Out, ray_Pos, Vec;

	//���C���΂����_�͒����_
	ray_Pos = me->target;

	//�x�N�g���͂��̒����_����J�������g�̍��W
	Vec = me->pos - me->target;
	Vec.Normalize();

	float Dist = 500.0f;

	ray_Pos -= Vec;
	if (me->collision_stage->RayPick(&Out, &ray_Pos, &Vec, &Dist) != -1){

		//���������_����ǂ̋�����radius��菬�����ꍇ
		if ((Out - me->target).Length() < (me->pos - me->target).Length()) me->pos = Out;
	}
}


//*****************************************************************************
//
//		�u�f�o�b�O���_�v��ԏ���
//
//*****************************************************************************
void Camera::Mode::Debug::Initialize()
{

}

void Camera::Mode::Debug::Update()
{
	float speed = 2.0f;
	if (KEY(KEY_SPACE) == 1) speed *= 2.0f;

	//=================================
	//	�J����Z�ړ�
	if (GetKeyState(VK_UP) & 0x80){
		me->pos.x -= sinf(me->angle.y) * speed;
		me->pos.z -= cosf(me->angle.y) * speed;
	}
	else if (GetKeyState(VK_DOWN) & 0x80){
		me->pos.x += sinf(me->angle.y) * speed;
		me->pos.z += cosf(me->angle.y) * speed;
	}

	//=================================
	//	�J����X�ړ�
	if (GetKeyState(VK_RIGHT) & 0x80){
		me->pos.x += sinf(me->angle.y - (PI / 2)) * speed;
		me->pos.z += cosf(me->angle.y - (PI / 2)) * speed;
	}
	else if (GetKeyState(VK_LEFT) & 0x80){
		me->pos.x += sinf(me->angle.y + (PI / 2)) * speed;
		me->pos.z += cosf(me->angle.y + (PI / 2)) * speed;
	}

	//=================================
	//	�J����Y�ړ�
	if (GetKeyState(0x22) & 0x80) me->pos.y -= speed;
	else if (GetKeyState(0x21) & 0x80) me->pos.y += speed;


	//=================================
	//	�J������]
	if (GetKeyState(0x66) & 0x80) me->angle.y += 0.05f;
	else if (GetKeyState(0x64) & 0x80) me->angle.y -= 0.05f;
	if (GetKeyState(0x62) & 0x80) me->angle.x += 0.05f;
	else if (GetKeyState(0x68) & 0x80) me->angle.x -= 0.05f;


	me->target.x = me->pos.x - sinf(me->angle.y);
	me->target.y = me->pos.y - tanf(me->angle.x);
	me->target.z = me->pos.z - cosf(me->angle.y);

	me->Set(me->pos, me->target);
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
	// �p�x
	float p_angle=me->my_player->Get_angleY();
	me->angle.y = p_angle;
	//me->angle.x;

	//�J�����̏㉺�ړ��ɐ���
	if (me->angle.x < -0.2f)me->angle.x = -0.2f;
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
	me->my_player->Get_pos(p_pos);

	p_pos.y += 5.0f;	// �������



	// �p�x�̒l�̃x�N�g���ƃv���C���[����J�����ʒu�Z�o
	me->ipos.x = p_pos.x - vec.x;
	me->ipos.y = p_pos.y - vec.y;
	me->ipos.z = p_pos.z - vec.z;


	// �����_�̓v���C���[
	me->target = p_pos;

	me->ipos += Vector3(0, 15, 0);
	me->target += Vector3(0, 14, 0);

	// �ڕW���W�܂ł�����蓮����
	me->pos = me->pos * .0f + me->ipos * 1.0f;

	Collision();

	me->Set(me->pos, me->target);

	// �v���C���[���[�h�ŃJ�����؂�ւ�
	if (me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE_FPS) me->Change_mode(MODE_PART::M_FPS);
	else if (me->my_player->Get_action() == BasePlayer::ACTION_PART::REND || me->my_player->Get_action() == BasePlayer::ACTION_PART::PASTE)
	{
		me->Change_mode(MODE_PART::M_ZOOM);
	}
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
	//me->angle.x += Mouse::Get_axis_y() *.05f;

	if (me->angle.x < -0.4f)me->angle.x = -0.4f;
	if (me->angle.x > 0.4f)me->angle.x = 0.4f;


	// �v���C���[���W�E�p�x�擾
	Vector3 p_pos;
	me->my_player->Get_pos(p_pos);
	p_pos.y += 2.5f;	// �������

	float p_angle = me->my_player->Get_angleY();
	me->angle.y = p_angle;

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
	me->pos.x = p_pos.x - (vec.x*-3.5f);
	me->pos.y = p_pos.y - (vec.y*-3.5f);
	me->pos.z = p_pos.z - (vec.z*-3.5f);

	me->pos.y += 15.0f;	// �������


	me->target.x = me->pos.x + (sinf(me->angle.y));
	me->target.y = me->pos.y + (tanf(me->angle.x));
	me->target.z = me->pos.z + (cosf(me->angle.y));

	Collision();

	me->Set(me->pos, me->target);


	// �v���C���[���[�h�ŃJ�����؂�ւ�
	if (me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE) me->Change_mode(MODE_PART::M_TPS);
	else if (me->my_player->Get_action() == BasePlayer::ACTION_PART::REND || me->my_player->Get_action() == BasePlayer::ACTION_PART::PASTE) me->Change_mode(MODE_PART::M_ZOOM);

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
//		�u�v�`�Y�[���v��ԏ���
//
//*****************************************************************************

void Camera::Mode::Zoom::Initialize()
{
	dist = DIST * .75f;
}

void Camera::Mode::Zoom::Update()
{
	// �p�x
#ifndef TYPE1
	me->angle.y += Mouse::Get_axis_x() * .05f;
#endif
#ifndef TYPE2
	float p_angle = me->my_player->Get_angleY();
	me->angle.y = p_angle;
#endif


	//me->angle.x += Mouse::Get_axis_y() * .025f;



	//�J�����̏㉺�ړ��ɐ���
	if (me->angle.x < -0.2f)me->angle.x = -0.2f;
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
	me->my_player->Get_pos(p_pos);

	p_pos.y += 10.0f;	// �������



	// �p�x�̒l�̃x�N�g���ƃv���C���[����J�����ʒu�Z�o
	me->ipos.x = p_pos.x - vec.x;
	me->ipos.y = p_pos.y - vec.y;
	me->ipos.z = p_pos.z - vec.z;

	// �����_�̓v���C���[
	me->target = p_pos;

	me->ipos += Vector3(0, 13, 0);
	me->target += Vector3(0, 6, 0);

	// �ڕW���W�܂ł�����蓮����
	me->pos = me->pos * .85f + me->ipos * .15f;

	//�ǔ���
	Collision();

	me->Set(me->pos, me->target);

	if (me->my_player->Get_action() != BasePlayer::ACTION_PART::REND && me->my_player->Get_action() != BasePlayer::ACTION_PART::PASTE)
	{
		// �v���C���[���[�h��TPS�Ȃ�J�����؂�ւ�
		if (me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE)
		{
			me->Change_mode(MODE_PART::M_TPS);
		}
		else if (me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE_FPS)
		{
			me->Change_mode(MODE_PART::M_FPS);
		}
	}
}


Camera *camera;