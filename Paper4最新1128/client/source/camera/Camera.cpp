
#include	"Camera.h"
#include	"../system/system.h"
#include	"../Player/BasePlayer.h"
#include	"../Player/MyPlayer.h"
#include	"../poster/Poster_manager.h"

#include	"../stage/Stage.h"

#include	"../../../share_data/Enum_public.h"

// �J�����˃v���C���[�̊�{����
const float Camera::Mode::Base::DIST = 50.0f;

const float FOVY[2] =
{
	D3DX_PI / 4,	// 0.79f
	.9f
};


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
	mode[MODE_PART::M_RESPAWN] = new Camera::Mode::Respawn(this);
	mode[MODE_PART::M_ZOOM] = new Camera::Mode::Zoom(this);
	mode[MODE_PART::M_TARGET] = new Camera::Mode::Target(this);

	Change_mode(MODE_PART::M_TPS);	// �ŏ��͎O�l��
	//Change_mode(MODE_PART::M_DEBUG);	// �f�o�b�O�J����
	my_player = my;

	collision_stage = new iexMesh("DATA/MATI/stage_atari.IMO");


	// �p�[�X
	parth.fovY = FOVY[(int)FOV_TYPE::DEFAULT];
	parth.Near = .1f;
	parth.Far = 2000.0f;
}

void Camera::Update()
{
	mode[mode_part]->Update();
	shader->SetValue("ViewPos", pos);

	// ���e�ݒ�
	SetProjection(parth.fovY, parth.Near, parth.Far);
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
	iangle = me->angle.y;
	dist = DIST;
}

void Camera::Mode::TPS::Update()
{
	if (me->my_player->Get_action() == BasePlayer::ACTION_PART::REND||me->my_player->Get_action() == BasePlayer::ACTION_PART::MOVE_TARGET)
	{
		me->Change_mode(MODE_PART::M_TARGET);
		return;
	}

	// �p�x
	float p_angle = me->my_player->Get_angleY();
	iangle = p_angle;

	// �I�[�o�[�t���[�h�~
	//if (iangle >= PI * 2)iangle -= PI * 2;
	//else if (iangle <= -PI * 2)iangle += PI * 2;

	me->angle.y = me->angle.y * .7f + iangle * .3f;
	//me->angle.y = iangle;

	//�A���O������	�p�x�␳
	if (!(me->my_player->Get_controlDesc().controlFlag & ((BYTE)PLAYER_CONTROL::LEFT_CLICK | (BYTE)PLAYER_CONTROL::RIGHT_CLICK))){
		float	work;
		work = me->my_player->Get_controlDesc().mouseY *0.0000005f;
		if (work > 0.1f) work = 0.1f;
		me->angle.x -= work;// Angle�ɉ��Z
	}

	//�J�����̏㉺�ړ��ɐ���
	if (me->angle.x < -0.2f)me->angle.x = -0.2f;
	if (me->angle.x > 0.2f)me->angle.x = 0.2f;

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
	me->parth.fovY = me->parth.fovY * .8f + FOVY[(int)FOV_TYPE::DEFAULT] * .2f;

	Collision();

	me->Set(me->pos, me->target);
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

}




//*****************************************************************************
//
//		�u���X�|�[���v��ԏ���
//
//*****************************************************************************

void Camera::Mode::Respawn::Initialize()
{

}

void Camera::Mode::Respawn::Update()
{

}


//*****************************************************************************
//
//		�u�v�`�Y�[���v��ԏ���
//
//*****************************************************************************

void Camera::Mode::Zoom::Initialize()
{
	dist = DIST;
}

void Camera::Mode::Zoom::Update()
{
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

	me->ipos += Vector3(0, 8, 0);
	me->target += Vector3(0, 1, 0);

	// �ڕW���W�܂ł�����蓮����
	me->pos = me->pos * .9f + me->ipos * .1f;
	me->parth.fovY = me->parth.fovY * .9f + FOVY[(int)FOV_TYPE::HARD] * .1f;

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
	}
}


//*****************************************************************************
//
//		�u�^�[�Q�b�g�v��ԏ���
//
//*****************************************************************************

void Camera::Mode::Target::Initialize()
{

}

void Camera::Mode::Target::Update()
{
	// �v���C���[���[�h�ŃJ�����؂�ւ�
	if (me->my_player->Get_action() != BasePlayer::ACTION_PART::MOVE_TARGET
		&& me->my_player->Get_action() != BasePlayer::ACTION_PART::REND)
	{
		me->Change_mode(MODE_PART::M_TPS);
		return;
	}

	static bool hosei = false;

	Vector3 p_pos;
	me->my_player->Get_pos(p_pos);
	Vector3 target_pos;
	target_pos = poster_mng->Get_pos(me->my_player->Get_poster_num());
	p_pos.y = .0f;

	Vector3 back = p_pos - target_pos;
	//Back = p_pos - e_pos;
	float ang = atan2(back.x, back.z);

	back.y = .0f;

	Vector3 to_target_vec(poster_mng->Get_pos(me->my_player->Get_poster_num()) - me->pos);	// ���Əꍇ��ipos�ɐ؂�ւ�

	const static float add_angle = 4.0f;	// ���̒l�Ńv���C���[�ƓG�̐���ɃJ������u���Ȃ��悤�ɕ␳����
	if (back.Length() > 80){
		back.Normalize();
		me->ipos.x = (p_pos.x + back.x * 50.0f) + sinf(ang + add_angle)*15.0f;
		me->ipos.z = (p_pos.z + back.z * 50.0f) + cosf(ang + add_angle)*15.0f;
		me->ipos.y = p_pos.y + 20.0f;
	}
	else {
		back.Normalize();
		me->ipos.x = (p_pos.x + back.x * 30.0f) + sinf(ang + add_angle)*10.0f;
		me->ipos.z = (p_pos.z + back.z * 30.0f) + cosf(ang + add_angle)*10.0f;
		me->ipos.y = p_pos.y + 15.0f;
	}

	//�����_
	if (target_pos.y - me->pos.y > 30.0f) target_pos.y -= 20.0f;
	else  target_pos.y += 15.0f;
	me->itarget = target_pos;

	me->angle.y = me->my_player->Get_angleY();

	Vector3 camera_front(me->matView._13, me->matView._23, me->matView._33);
	to_target_vec.Normalize();
	float siita;// degree.
	float r = Vector3Dot(camera_front, to_target_vec);

	// �Ƃ��Ă��厖
	siita = acosf(r / (camera_front.Length() * to_target_vec.Length())) / 0.01745f;

	if (hosei)
	{
		const float percentage = siita / 360;
		me->target.x = me->target.x * (1 - percentage) + me->itarget.x * percentage;
		me->target.z = me->target.z * (1 - percentage) + me->itarget.z * percentage;
		if (siita < 5)
		{
			hosei = false;	// 5�x�ȓ��Ȃ�␳�I��
		}
	}


	// �Ɠx�ɓ����Ă��Ȃ��Ȃ�ǐ�
	else
	{
		if (siita > 10)
		{
			hosei = true;	// �Ɠx�z������␳������(60�͓G���Ƃɐݒ萄��)
		}
		else
		{
			me->target.x = me->target.x * .99f + me->itarget.x * .01f;	// ���������
			me->target.z = me->target.z * .99f + me->itarget.z * .01f;
		}
	}

	me->target.y = me->target.y * .995f + me->itarget.y * .005f;
	me->pos = me->pos * 0.9f + me->ipos * 0.1f;

	//	���_�ݒ�
	me->Set(me->pos, me->target);
}

Camera *camera;