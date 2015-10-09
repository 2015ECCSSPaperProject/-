#include	"iextreme.h"

#include	"../stage/Stage.h"
#include	"../poster/Poster_manager.h"
#include	"Player.h"
#include	"../sound/SoundManager.h"

#include	"../Mouse/Mouse.h"

#define GAME_PAD
//#define MOUSE

Player::Player() : model(nullptr)
{
	ZeroMemory(action, sizeof(Action::Base));
}

Player::~Player()
{
	Release();
}

void Player::Initialize(iex3DObj *obj, Poster_manager *poster_mng, TEAM_COLOR col)
{
	// ��{�p�����[�^������
	pos = Vector3(0, 0, 0);
	angle = Vector3(0, 0, 0);
	scale = .25f;
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
	action[(int)ACTION_PART::MOVE] = new Player::Action::Move(this);
	action[(int)ACTION_PART::MOVE_FPS] = new Player::Action::MoveFPS(this);
	action[(int)ACTION_PART::ATTACK] = new Player::Action::Attack(this);
	action[(int)ACTION_PART::PASTE] = new Player::Action::Paste(this);
	action[(int)ACTION_PART::REND] = new Player::Action::Rend(this);
	action[(int)ACTION_PART::DIE] = new Player::Action::Die(this);

	Change_action(ACTION_PART::MOVE);	// �ŏ��͈ړ����
	do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�


	// �F�ύX
	//switch (col)
	//{
	//	case TEAM_COLOR::ONE:
	//		Set_texture("DATA/CHR/teki_blue.png");
	//		break;

	//	case TEAM_COLOR::TWO:
	//		Set_texture("DATA/CHR/teki_red.png");
	//		break;
	//}
	team_col = col;

	// �Ϗ�����
	this->poster_mng = poster_mng;
}

void Player::Release()
{
	delete model;

	for (int i = 0; i < (int)ACTION_PART::MAX; i++)
	{
		delete action[i], action[i] = nullptr;
	}
}

void Player::Update(Stage *stage)
{
	action[(int)action_part]->Update();

	fallspeed -= 0.1f;
	move.y = fallspeed;
	stage->Collision(pos, &move, 1.0f, 2);
	if (stage->Collision_rand(pos, &move, .0f))
		fallspeed = .0f;

	action[(int)action_part]->Update_obj();
}

void Player::Render()
{
	char str[64];

	wsprintf(str, "[X]�L�[�ŐF�؂�ւ�");
	IEX_DrawText(str, 10, 176, 200, 20, 0xFFffff00);

	if (team_col == TEAM_COLOR::ONE)
	{
		wsprintf(str, "���ԐF");
		IEX_DrawText(str, 10, 196, 200, 20, 0xFFff0000);
	}
	else
	{
		wsprintf(str, "���F");
		IEX_DrawText(str, 10, 196, 200, 20, 0xFF0000ff);
	}

	action[(unsigned int)action_part]->Render();
}

void Player::Set_texture(char *filename)
{
	//iexMesh2_textures texture;
	//texture.Create_load(1, filename);
	//obj->Texture_change(texture);
	//texture.Release();
}



//*****************************************************************************
//
//		��ԏ����x�[�X�֌W
//
//*****************************************************************************
void Player::Action::Base::Update_obj()
{
	me->pos += me->move;
	//me->move = Vector3(0, 0, 0);

	me->model->Animation();
	me->model->SetScale(me->scale);
	me->model->SetAngle(me->angle);
	me->model->SetPos(me->pos);
	me->model->Update();
}

void Player::Action::Base::Set_motion(int no)
{
	if (me->model->GetMotion() != no) me->model->SetMotion(no);
}




//*****************************************************************************
//
//		�uTPS�ړ��v��ԏ���
//
//*****************************************************************************

void Player::Action::Move::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// �ҋ@���[�V�����Z�b�g
	Set_motion(1);
}

void Player::Action::Move::Update()
{
	float AxisX = 0, AxisY = 0;

#ifndef GAME_PAD

	//	���X�e�B�b�N�擾
	float dist;
	AxisX = (float)KEY_GetAxisX() * .001f;
	AxisY = (float)-KEY_GetAxisY()* .001f;

	// ���X����X�e�B�b�N�̌X����r��
	dist = sqrtf(AxisX * AxisX + AxisY * AxisY);
	if (dist < .35f) {
		AxisX = 0;
		AxisY = 0;
	}
	if (dist > 1.0f)
	{
		AxisX /= dist;
		AxisY /= dist;
	}

	Vector3 front(matView._13, 0, matView._33);
	Vector3 right(matView._11, 0, matView._31);
#endif

#ifndef MOUSE
	if (KEY(KEY_LEFT) == 1) AxisX = -1;
	else if (KEY(KEY_RIGHT) == 1) AxisX = 1;
	if (KEY(KEY_UP) == 1) AxisY = 1;
	else if (KEY(KEY_DOWN) == 1) AxisY = -1;

	Vector3 front(sinf(me->angle.y), 0, cosf(me->angle.y));
	Vector3 right(sinf(me->angle.y + PI * .5f), 0, cosf(me->angle.y + PI * .5f));
#endif

	front.Normalize();
	right.Normalize();
	// �ړ��ʌ���
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);

#ifndef GAME_PAD
	me->angle.y += AxisX * .05f;
#endif

#ifndef MOUSE
	me->angle.y += Mouse::Get_axis_x() * .1f;
#endif



	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		Set_motion(1);

		se->Stop(me->se_receive);
	}
	// �������瓮���Ă���
	else
	{

#ifndef GAME_PAD
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
		Set_motion(0);

		int receive = se->Play("���s", true);
		if (receive != SoundBase::NOT_FOUND)
			me->se_receive = receive;
	}

	// �E�N���b�N�\��
	if (Mouse::isPushRight())
	{
		me->poster_num = me->poster_mng->Can_do(me, me->team_col);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (me->poster_mng->Can_paste(me->team_col, me->poster_num))
			{
				me->Change_action(ACTION_PART::PASTE);
				return;
			}
		}
	}

	// ���N���b�N�Ŕj��
	else if (Mouse::isPushLeft())
	{
		me->poster_num = me->poster_mng->Can_do(me, me->team_col);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (me->poster_mng->Can_rend(me->team_col, me->poster_num))
			{
				me->Change_action(ACTION_PART::REND);
				return;
			}
		}
	}

	// B�L�[�ŃJ���[�`�F���W
	//if (KEY(KEY_B) == 3) me->Change_color();

	// C�L�[�ŃJ�������[�h�ؑ�
	if (KEY(KEY_C) == 3)
	{
		me->Change_action(ACTION_PART::MOVE_FPS);
		me->camera_mode = CAMERA_MODE::FPS;
	}
}

void Player::Action::Move::Render()
{
	me->model->Render();
}



//*****************************************************************************
//
//		�uFPS�ړ��v��ԏ���
//
//*****************************************************************************

void Player::Action::MoveFPS::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// �ҋ@���[�V�����Z�b�g
	Set_motion(0);
}

void Player::Action::MoveFPS::Update()
{
	float AxisX = 0, AxisY = 0;

#ifndef MOUSE
	if (KEY(KEY_LEFT) == 1) AxisX = -1;
	else if (KEY(KEY_RIGHT) == 1) AxisX = 1;
	if (KEY(KEY_UP) == 1) AxisY = 1;
	else if (KEY(KEY_DOWN) == 1) AxisY = -1;

	me->angle.y += Mouse::Get_axis_x() * .05f;
#endif




	//	�ړ��x�N�g���ݒ�
	Vector3 front(sinf(me->angle.y), 0, cosf(me->angle.y));
	Vector3 right(sinf(me->angle.y + PI * .5f), 0, cosf(me->angle.y + PI * .5f));
	front.Normalize();
	right.Normalize();

	//	�ړ��ʌ���
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);

	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		Set_motion(1);

		se->Stop(me->se_receive);
	}
	// �������瓮���Ă���
	else
	{
		// �ړ����[�V����
		Set_motion(0);

		int receive = se->Play("���s", true);
		if (receive != SoundBase::NOT_FOUND)
			me->se_receive = receive;
	}

	// �E�N���b�N�\��
	if (Mouse::isPushRight())
	{
		me->poster_num = me->poster_mng->Can_do(me, me->team_col);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (me->poster_mng->Can_paste(me->team_col, me->poster_num))
			{
				me->Change_action(ACTION_PART::PASTE);
				return;
			}
		}
	}

	// ���N���b�N�Ŕj��
	else if (Mouse::isPushLeft())
	{
		me->poster_num = me->poster_mng->Can_do(me, me->team_col);

		// �|�X�^�[��������
		if (me->poster_num != -1)
		{
			if (me->poster_mng->Can_rend(me->team_col, me->poster_num))
			{
				me->Change_action(ACTION_PART::REND);
				return;
			}
		}
	}

	// C�L�[�ŃJ�������[�h�ؑ�
	if (KEY(KEY_C) == 3)
	{
		me->Change_action(ACTION_PART::MOVE);
		me->camera_mode = CAMERA_MODE::TPS;
	}

}

void Player::Action::MoveFPS::Render()
{
	me->model->Render();
}



//*****************************************************************************
//
//		�u�U���v��ԏ���
//
//*****************************************************************************

void Player::Action::Attack::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// �ړ��X�g�b�v
	me->move = Vector3(0, 0, 0);

	// �U�����[�V�����Z�b�g
	//Set_motion(8);
}

void Player::Action::Attack::Update()
{
	// �U�����[�V�����I��
	//if (me->model->GetParam(0) == 1)
	//{
		me->Change_action(ACTION_PART::MOVE);
	//}

}

void Player::Action::Attack::Render()
{
	me->model->Render();
}




//*****************************************************************************
//
//		�u�|�X�^�[�\��t���v��ԏ���
//
//*****************************************************************************

void Player::Action::Paste::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// �ړ��X�g�b�v
	me->move = Vector3(0, 0, 0);

	// �\��t�����[�V�����Z�b�g
	//Set_motion(9);
}

void Player::Action::Paste::Update()
{
	// �\��t�����[�V�����I��
	//if (me->model->GetParam(0) == 3)
	//{
		me->Change_action((me->camera_mode == CAMERA_MODE::TPS) ? ACTION_PART::MOVE : ACTION_PART::MOVE_FPS);
	//}

	// �\��t���t���[��
	//if (me->model->GetParam(0) == 1)
	//{
		me->poster_mng->Paste_poster(me->team_col, me->poster_num);
		se->Play("�|�X�^�[�\��");
		//me->poster_mng->Do_poster(0, me->team_col);
		//me->poster_mng->Do_poster(0, TEAM_COLOR::ONE);
	//}

}

void Player::Action::Paste::Render()
{
	me->model->Render();
}


//*****************************************************************************
//
//		�u�|�X�^�[�j��v��ԏ���
//
//*****************************************************************************

void Player::Action::Rend::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// �ړ��X�g�b�v
	me->move = Vector3(0, 0, 0);

	// �j�胂�[�V�����Z�b�g
	Set_motion(2);
}

void Player::Action::Rend::Update()
{
	// �j�胂�[�V�����I��
	if (me->model->GetParam(0) == 2)
	{
		me->Change_action((me->camera_mode == CAMERA_MODE::TPS) ? ACTION_PART::MOVE : ACTION_PART::MOVE_FPS);
	}

	// �j��t���[��
	if (me->model->GetParam(0) == 1)
	{
		me->poster_mng->Rend_poster(me->team_col, me->poster_num);
		se->Play("�|�X�^�[�j��");
	}
}

void Player::Action::Rend::Render()
{
	me->model->Render();
}


//*****************************************************************************
//
//		�u���ʁv��ԏ���
//
//*****************************************************************************

void Player::Action::Die::Initialize()
{
	me->do_flag = DO_FLAG::NONE;	// Z�L�[�����Ă��������Ȃ�

	// ����ł鎞��
	die_frame = 120;

	// �ړ��X�g�b�v
	me->move = Vector3(0, 0, 0);

	// ���ʃ��[�V�����Z�b�g
	//Set_motion(4);
}

void Player::Action::Die::Update()
{
	if (--die_frame < 0)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 3DObj�X�V
	Update_obj();
}

void Player::Action::Die::Render()
{
	me->model->Render();
}