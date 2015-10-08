#include	"iextreme.h"

#include	"../stage/Stage.h"
#include	"../poster/Poster_manager.h"
#include	"Player.h"


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
	scale = .5f;
	move = Vector3(0, 0, 0);
	speed = 3.0f;
	fallspeed = .0f;

	// 3D����
	model = obj->Clone();

	// �s����ԏ�����
	action[(unsigned int)ACTION_PART::MOVE] = new Player::Action::Move(this);
	action[(unsigned int)ACTION_PART::MOVE_FPS] = new Player::Action::MoveFPS(this);
	action[(unsigned int)ACTION_PART::ATTACK] = new Player::Action::Attack(this);
	action[(unsigned int)ACTION_PART::PASTE] = new Player::Action::Paste(this);
	action[(unsigned int)ACTION_PART::REND] = new Player::Action::Rend(this);
	action[(unsigned int)ACTION_PART::DIE] = new Player::Action::Die(this);

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

	for (unsigned int i = 0; i < (unsigned int)ACTION_PART::MAX; i++)
	{
		delete action[i], action[i] = nullptr;
	}
}

void Player::Update(Stage *stage)
{
	action[(unsigned int)action_part]->Update();

	fallspeed -= 0.1f;
	move.y = fallspeed;
	stage->Collision(pos, &move, 1.0f, 2);
	if (stage->Collision_rand(pos, &move, .0f))
		fallspeed = .0f;

	action[(unsigned int)action_part]->Update_obj();
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
	Set_motion(0);
}

void Player::Action::Move::Update()
{
	//	���X�e�B�b�N�擾
	float AxisX = (float)KEY_GetAxisX() * .001f;
	float AxisY = (float)-KEY_GetAxisY()* .001f;

	// ���X����X�e�B�b�N�̌X����r��
	float dist = sqrtf(AxisX * AxisX + AxisY * AxisY);
	if (dist < .35f) {
		AxisX = 0;
		AxisY = 0;
	}
	if (dist > 1.0f)
	{
		AxisX /= dist;
		AxisY /= dist;
	}

	//	�ړ��x�N�g���ݒ�
	Vector3 front(matView._13, 0, matView._33);
	Vector3 right(matView._11, 0, matView._31);
	front.Normalize();
	right.Normalize();
	//	�ړ��ʌ���
	me->move = (front*AxisY + right*AxisX) * (me->speed * .1f);


	// �~�܂��Ă���
	if (me->move.Length() == 0)
	{
		// �ҋ@���[�V����
		Set_motion(0);
	}
	// �������瓮���Ă���
	else
	{
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

		// �ړ����[�V����
		Set_motion(1);
	}


	// Z�L�[�ōU��
	//if (KEY(KEY_A) == 3) me->Change_action(ACTION_PART::ATTACK);

	// X�L�[�Ń|�X�^�[�\��t��
	//if (KEY(KEY_B) == 3) me->Change_action(ACTION_PART::PASTE);

	// Z�L�[�ŃA�N�V����
	if (KEY(KEY_A) == 3)
	{
		me->poster_mng->Do_poster(me, me->team_col);

		switch (me->do_flag)
		{
		case DO_FLAG::NONE:	 // �������Ȃ�
			break;

		case DO_FLAG::ATTACK:// �U��
			me->Change_action(ACTION_PART::ATTACK);
			break;

		case DO_FLAG::PASTE:// �\��
			me->Change_action(ACTION_PART::PASTE);
			break;

		case DO_FLAG::REND:	// �j��
			me->Change_action(ACTION_PART::REND);
			break;
		}
	}

	// B�L�[�ŃJ���[�`�F���W
	if (KEY(KEY_B) == 3) me->Change_color();

	// C�L�[�ŃJ�������[�h�ؑ�
	if (KEY(KEY_C) == 3) me->Change_action(ACTION_PART::MOVE_FPS);


	// 3DObj�X�V
	Update_obj();
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
	//===============================================
	//	�����]��(�E�X�e�B�b�N)
	//===============================================

	//	�E�X�e�B�b�N�擾
	float AxisX = (float)KEY_GetAxisX2() * .001f;
	float AxisY = (float)-KEY_GetAxisY2()* .001f;

	// ���X����X�e�B�b�N�̌X����r��
	float dist = sqrtf(AxisX * AxisX + AxisY * AxisY);
	if (dist < .35f) {
		AxisX = 0;
		AxisY = 0;
	}
	if (dist > 1.0f)
	{
		AxisX /= dist;
		AxisY /= dist;
	}

	me->angle.y += AxisX * .05f;



	//===============================================
	//	�ړ�(���X�e�B�b�N)
	//===============================================

	//	���X�e�B�b�N�擾
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
		Set_motion(0);
	}
	// �������瓮���Ă���
	else
	{
		// �ړ����[�V����
		Set_motion(1);
	}


	//// Z�L�[�ōU��
	//if (KEY(KEY_A) == 3) me->Change_action(ACTION_PART::ATTACK);

	//// X�L�[�Ń|�X�^�[�\��t��
	//if (KEY(KEY_B) == 3) me->Change_action(ACTION_PART::PASTE);


	// Z�L�[�ŃA�N�V����
	if (KEY(KEY_A) == 3)
	{

	}

	// C�L�[�ŃJ�������[�h�ؑ�
	if (KEY(KEY_C) == 3) me->Change_action(ACTION_PART::MOVE);


	// 3DObj�X�V
	Update_obj();
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
	Set_motion(8);
}

void Player::Action::Attack::Update()
{
	// �U�����[�V�����I��
	if (me->model->GetParam(0) == 1)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// 3DObj�X�V
	Update_obj();
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
	Set_motion(9);
}

void Player::Action::Paste::Update()
{
	// �\��t�����[�V�����I��
	if (me->model->GetParam(0) == 3)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// �\��t���t���[��
	if (me->model->GetParam(0) == 1)
	{
		//me->poster_mng->Do_poster(0, me->team_col);
		//me->poster_mng->Do_poster(0, TEAM_COLOR::ONE);
	}

	// 3DObj�X�V
	Update_obj();
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
	Set_motion(9);
}

void Player::Action::Rend::Update()
{
	// �j�胂�[�V�����I��
	if (me->model->GetParam(0) == 3)
	{
		me->Change_action(ACTION_PART::MOVE);
	}

	// �j��t���[��
	if (me->model->GetParam(0) == 1)
	{
		//me->poster_mng->Do_poster(0, me->team_col);
		//me->poster_mng->Do_poster(0, TEAM_COLOR::TWO);
	}

	// 3DObj�X�V
	Update_obj();
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
	Set_motion(4);
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