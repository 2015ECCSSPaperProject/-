#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"
#include	"../ui/UI.h"
#include	"../../IEX/OKB.h"
#include	"../../../share_data/Enum_public.h"
#include	"../Sound/SoundManager.h"
#include "../pie_graph/pie_graph.h"
#include "../Animation/Spread2D.h"
#include	"../Manhole/Manhole.h"
#include	"../Barrier/Barrier.h"
#include	"../paper object/paper object manager.h"
#include	"../paper object/Rend data.h"
#include	"../Rush/Rush.h"

//****************************************************************************************************************
//
//  ������
//
//****************************************************************************************************************
MyPlayer::MyPlayer() :BasePlayer()
{
	//skillGage = new Pie_graph_content("DATA/skillGage/SpiritCircle_gage.png");	//	�Q�[�W
	//skillGage->Add_content("DATA/skillGage/SpiritCircle_UNDER.png");

	// �֐��|�C���^������
	RendSE[KIND_PAPER_OBJECT::POSTER] = &MyPlayer::RendPosterSE;
	RendSE[KIND_PAPER_OBJECT::FLYER] = &MyPlayer::RendFlyerSE;
	RendSE[KIND_PAPER_OBJECT::CALENDAR] = &MyPlayer::RendCalendarSE;
	RendSE[KIND_PAPER_OBJECT::MAGAZIN] = &MyPlayer::RendMagazineSE;
	RendSE[KIND_PAPER_OBJECT::MONEY] = &MyPlayer::RendMoneySE;
	RendSE[KIND_PAPER_OBJECT::SEISHO] = &MyPlayer::RendSeisyoSE;
	RendSE[KIND_PAPER_OBJECT::SHINBUN] = &MyPlayer::RendShinbunSE;
	RendSE[KIND_PAPER_OBJECT::SIGN] = &MyPlayer::RendSignSE;
	RendSE[KIND_PAPER_OBJECT::TOILET_PAPER] = &MyPlayer::RendToileSE;
	RendSE[KIND_PAPER_OBJECT::ZASSHI] = &MyPlayer::RendZasshiSE;
	RendSE[KIND_PAPER_OBJECT::SHOJI] = &MyPlayer::RendShojiSE;
}

MyPlayer::~MyPlayer()
{
	for (int i = 0; i < KIND_PAPER_OBJECT::KIND_MAX_PAPER_OBJ; i++)
	{
		delete command_data[i];
	}
	delete[] command_data;
}

void MyPlayer::Initialize(iex3DObj **obj)
{
	BasePlayer::Initialize(obj);
	isMyNunber = true;
	se_step = 0;

	ShowCursor(FALSE);

	// �j��}�E�X�̓����̏�����
	const int num = 4;
	Rend_data::Movedata data[num]=
	{
		{ 80000, 0 },
		{ -80000, 0 },
		{ 80000, 0 },
		{ -80000, 0 }
	};

	// command�f�[�^
	command_data = new Rend_data*[KIND_PAPER_OBJECT::KIND_MAX_PAPER_OBJ];
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::CALENDAR],	"DATA/paper object/calendar/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::FLYER],		"DATA/paper object/flyer/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::MAGAZIN],	"DATA/paper object/magazin/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::MONEY],		"DATA/paper object/money/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::POSTER],	"DATA/paper object/Poster/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::SEISHO],	"DATA/paper object/seisho/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::SHINBUN],	"DATA/paper object/shinbun/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::SHOJI],		"DATA/paper object/shoji/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::SIGN],		"DATA/paper object/sign/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::TOILET_PAPER], "DATA/paper object/toilet_paper/rend command.txt");
	Rend_data::Load(command_data[KIND_PAPER_OBJECT::ZASSHI],	"DATA/paper object/zasshi/rend command.txt");
}

void MyPlayer::Release()
{
	BasePlayer::Release();
}




//*************************************************************************************************************************
//		�X�V
//*************************************************************************************************************************

void MyPlayer::Update()
{
	// C�L�[�̃g�O��
	//m_controlDesc.controlFlag &= ((BYTE)PLAYER_CONTROL::TRG_C ^ 0xff);
	//if (toggle_c)
	//	m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;

	if (ui->isStart())
	{
		/*���͎�t����*/
		Control_all();
		/*�}�E�X�̏���*/
		Mouse_Update();
	}

	BasePlayer::Update();
	Update_action();

	// ���X�i�[���
	Update_listener();

	//if (rend_data->Move_check(m_controlDesc.mouseX, m_controlDesc.mouseY))
	//{
	//	se->Play("�藠��");
	//	rend_data->Reset();
	//}
}


//*************************************************************************************************************************
//		�`��
//*************************************************************************************************************************
void MyPlayer::Render(iexShader *shader, char *name)
{
	BasePlayer::Render(shader, name);
	//Text::Draw(320, 480, 0xff00ffff, "%.3f", m_controlDesc.mouseX);
	//Text::Draw(320, 520, 0xff00ffff, "%.3f", m_controlDesc.mouseY);
	//Text::Draw(32, 560, 0xff00ffff, "%.1f",pos.y);
	//Text::Draw(32, 600, 0xff00ffff, "%.1f",pos.z);
	//DeferredManager.ForwardBigin();

	//�~�Q�[�W
	//float persent =1.0f - ((float)skill_data[(int)SKILL::GUN].wait_time / (float)skill_data[(int)SKILL::GUN].cool_time);
	//Text::Draw(32, 420, 0xff00ffff, "aaa%f", persent);
	//skillGage->Render(persent, 0, 300, 128, 128, 0, 0, 128, 128);
	//DeferredManager.ForwardEnd();
}

void MyPlayer::Update_action()
{
	// �p����̃A�N�V�����N���X���g���Ȃ��̂ł����Ŗ������X�V
	ui->Set_rend_command(UI::REND_COMMAND::NONE);

	switch (action_part)
	{
	case ACTION_PART::REND:
		//// �}�E�X�������烊�Z�b�g
		//if (!(m_controlDesc.controlFlag&(BYTE)PLAYER_CONTROL::LEFT_CLICK))
		//{
		//	command_data[paper_obj_mng->Get_kind(poster_num)]->Reset();
		//	return;
		//}
		//
		///* �R�}���h�}�[�N��UI�N���X�ɑ��� */
		//float move_x, move_y;
		//command_data[paper_obj_mng->Get_kind(poster_num)]->Get_commnand(&move_x, &move_y);
		//Vector2 com_move((int)move_x, (int)move_y);
		//com_move.Normalize(move_x, move_y);
		//UI::REND_COMMAND com;
		//if (move_x == 0)
		//{
		//	com = (move_y < 0) ? UI::REND_COMMAND::DOWN : UI::REND_COMMAND::UP;
		//}
		//else if (move_y == 0)
		//{
		//	com = (move_x < 0) ? UI::REND_COMMAND::LEFT : UI::REND_COMMAND::RIGHT;
		//}
		//else
		//{
		//	int i = 0;
		//}
		//ui->Set_rend_command(com);
		//
		//// �}�E�X�X���C�h�R�}���h���͂���������
		//if (command_data[paper_obj_mng->Get_kind(poster_num)]->Move_check(m_controlDesc.mouseX, m_controlDesc.mouseY))
		//{
		//	m_controlDesc.rendFlag |= (BYTE)PLAYER_FLAG::REND;
		//	command_data[paper_obj_mng->Get_kind(poster_num)]->Reset();
		//}

		if (sqrtf(m_controlDesc.mouseX*m_controlDesc.mouseX + m_controlDesc.mouseY*m_controlDesc.mouseY) > 80000) m_controlDesc.rendFlag |= (BYTE)PLAYER_FLAG::REND;

		break;


	case ACTION_PART::MANHOLE:
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			if (se_receive == -1)
			{
				se_receive = se->Play((isManhole) ? "�̂ڂ�" : "������");
			}
		}
		if (models[(int)model_part]->GetParam(0) == 2)
		{
			if (ui->GetManholeFade() != UI::MANHOLE_FADE_TYPE::F_OUT)
				ui->SetManholeFade(UI::MANHOLE_FADE_TYPE::F_OUT);
		}
		break;

	case ACTION_PART::REND_OBJ:
		if (kind_paper_obj != -1)(this->*RendSE[kind_paper_obj])();	// ���ꂼ��̃��[�V�����ɍ��킹�ĉ����o��
		break;
	}
}


/*	����@*/
void MyPlayer::Control_all()
{
	// ������
	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;

	static int skill_wait = 0;
	if (m_controlDesc.skillFlag != 0)
	{
		if (--skill_wait <= 0){ skill_wait = 0; m_controlDesc.skillFlag &= 0x00000000; }
	}

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

	// �E�N���b�N�ŕK�E�Z�𔭓������邩
	if (m_controlDesc.controlFlag & (int)PLAYER_CONTROL::RIGHT_CLICK)
	{
		if (action_part != ACTION_PART::MOVE) return;
		// �Q�[�W�����܂��Ă���
		if (skill_data[(int)select_skill].wait_time <= 0)
		{
			const int FLAG[] =
			{
				(int)PLAYER_SKILL::SYURIKEN,
				(int)PLAYER_SKILL::GUN,
				(int)PLAYER_SKILL::KABUTO,
			};

			m_controlDesc.skillFlag |= FLAG[(int)select_skill];
			// �X�L���������̂ŃN�[���^�C���ݒ�
			skill_data[(int)select_skill].wait_time = skill_data[(int)select_skill].cool_time;
			skill_wait = 3;	// 1�t���[��������������Ȃ������炽�܂ɔ������Ȃ��̂�3�t���[�����炢����
		}
		//SPI_GET_WHEELSCROLL
	}
}

float MOUSE_SENS = 0.8f;	// �}�E�X�̊��x

/*	�}�E�X�X�V	*/
void MyPlayer::Mouse_Update()
{
	//	�L�[�ݒ�	�J�[�\��
	GetCursorPos(&mousePos);

	//�@��ʂ̐^�񒆂ֈړ�
	RECT rc;
	GetWindowRect(iexSystem::Window, &rc);

	mousePos.x -= rc.left + iexSystem::ScreenWidth / 2;
	mousePos.y -= rc.top + iexSystem::ScreenHeight / 2;
	//if (mousePos.x < -128) mousePos.x = -128;
	//if (mousePos.x >  127) mousePos.x = 127;
	//if (mousePos.y < -128) mousePos.y = -128;
	//if (mousePos.y >  127) mousePos.y = 127;
	mousePos.x = (LONG)((float)mousePos.x * MOUSE_SENS);
	mousePos.y = (LONG)((float)mousePos.y * MOUSE_SENS);
	if (GetActiveWindow() == IEX_GetWindow())
	{
		ShowCursor(FALSE);
		SetCursorPos(rc.left + iexSystem::ScreenWidth / 2, rc.top + iexSystem::ScreenHeight / 2);
	}

	//�@�}�E�X�̓������f�X�N�ɓn��
	m_controlDesc.mouseX = (float)mousePos.x * 1000;
	m_controlDesc.mouseY = (float)mousePos.y * 1000;

	// �z�C�[��
	if (Get_wheel_flag() == WHEEL_FLAG::DOWN)
	{
		if (++select_skill < (int)SKILL::MAX)
		{
			if (skill_data[select_skill].unlock) return;
		}
		select_skill = 0;
	}
	else if (Get_wheel_flag() == WHEEL_FLAG::UP)
	{
		if (--select_skill < 0)
		{
			for (int i = (int)SKILL::MAX - 1;; i--)
			{
				if (skill_data[i].unlock) { select_skill = i; break; }
			}
		}
	}
}

void MyPlayer::Update_listener()
{
	const Vector3 front(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33);
	const Vector3 up(models[(int)model_part]->TransMatrix._21, models[(int)model_part]->TransMatrix._22, models[(int)model_part]->TransMatrix._23);

	se->Set_listener(pos, front, up, move);
}

void MyPlayer::RendPosterSE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 20, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se_receive = se->Play("�j��");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendFlyerSE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 20, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se_receive = se->Play("�j��");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendCalendarSE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se_receive = se->Play("�J�����_�[�j��", true);
			se_step++;
		}
		break;
	case 1:
		// �j���I���
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Stop("�J�����_�[�j��", se_receive);
			se_step++;
		}
		break;
	case 2:
		// �Y����
		if (models[(int)model_part]->GetParam(5) == 3)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se_step = 99;	// �X�e�b�v�I���
		}
	}
}
void MyPlayer::RendMagazineSE()
{
	switch (se_step)
	{
	case 0:
		// �ŏ�����炷
		se_receive=se->Play("�}�K�W���j��");
		se_step++;

		break;
	case 1:
		// �j��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Stop("�}�K�W���j��", se_receive);
			se->Play("�}�K�W���j��2");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendMoneySE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se_receive = se->Play("�����j��", true);
			se_step++;
		}
		break;
	case 1:
		// �j���I���
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Stop("�����j��", se_receive);
			se_step++;
		}
		break;
	case 2:
		// �����̂�
		if (models[(int)model_part]->GetParam(5) == 3)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Play("�T�C���j��2");
			se_step = 99;	// �X�e�b�v�I���
		}
	}
}
void MyPlayer::RendSeisyoSE()
{
	switch (se_step)
	{
	case 0:
		// �ŏ�����炷
		se_receive = se->Play("�����j��");
		se_step++;
		break;

	case 1:
		// ��ɓ���
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se->Play("�����j��2");
			se_step++;	// �X�e�b�v�I���
		}
		break;

	case 2:
		// �j��
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Stop("�����j��", se_receive);
			se->Play("�����j��3");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendSignSE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Play("�T�C���j��");
			se_step++;
		}
		break;
	case 1:
		// �j���I���
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			//se->Play("�T�C���j��2");
			se->Play("�Z���j��");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendShinbunSE()
{
	switch (se_step)
	{
	case 0:
		// ���̉��I��
		se_step++;
		break;

	case 1:
		// �j���n��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Play("�V���j��");
			se_step++;
		}
		break;
	case 2:
		// �j���I���
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Play("�V���j��2");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendToileSE()
{
	switch (se_step)
	{
	case 0:
		// �ŏ�����炷
		se_receive = se->Play("�g�C���j��");
		se_step++;

		break;
	case 1:
		// �j��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Stop("�g�C���j��", se_receive);
			se->Play("�g�C���j��2");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendZasshiSE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se_receive = se->Play("�Z���j��");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}

void MyPlayer::RendShojiSE()
{
	switch (se_step)
	{
	case 0:
		// �j���n��
		if (models[(int)model_part]->GetParam(5) == 1)
		{
			se_receive = se->Play("�J�����_�[�j��", true);
			se_step++;
		}
		break;
	case 1:
		// �j���I���
		if (models[(int)model_part]->GetParam(5) == 2)
		{
			se->Stop("�J�����_�[�j��", se_receive);
			se_step++;
		}
		break;
	case 2:
		// �R��
		if (models[(int)model_part]->GetParam(5) == 3)
		{
			//if (poster_num != -1)
			//{
			//	Number_Effect::SetNum(paper_obj_mng->Get_pos(poster_num) + Vector3(0, 10, 0), paper_obj_mng->Get_point(poster_num), 4);
			//}
			se->Play("�T�C���j��");
			se_step = 99;	// �X�e�b�v�I���
		}
	}
}

void MyPlayer::Set_action(ACTION_PART part)
{
	if (action_part != part)
	{
		if (action_part == ACTION_PART::SYURIKEN && part == ACTION_PART::MOVE)
		{
			// �藠���G�t�F�N�g�X�g�b�v
			rush->Stop();
		}
		if (action_part == ACTION_PART::MANHOLE && part == ACTION_PART::MOVE)
		{
			isManhole ^= 1;
			event_bgm->Set_manhole(isManhole);
			ui->SetManholeFade(UI::MANHOLE_FADE_TYPE::F_IN);
		}
		if (part == ACTION_PART::REND_OBJ) se_step = 0;
		Change_action(part);
	}
}
