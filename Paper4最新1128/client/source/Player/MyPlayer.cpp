
#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"

#include	"../../IEX/OKB.h"
#include	"../../../share_data/Enum_public.h"
#include	"../Sound/SoundManager.h"
#include "../pie_graph/pie_graph.h"
#include "../Animation/Spread2D.h"
#include	"../Manhole/Manhole.h"
#include	"../Barrier/Barrier.h"

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
}

MyPlayer::~MyPlayer()
{
	//SAFE_DELETE(skillGage);

}

void MyPlayer::Initialize(iex3DObj **obj)
{
	BasePlayer::Initialize(obj);
	isMyNunber = true;
	se_step = 0;
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
	/*���͎�t����*/
	Control_all();

	// C�L�[�̃g�O��
	//m_controlDesc.controlFlag &= ((BYTE)PLAYER_CONTROL::TRG_C ^ 0xff);
	//if (toggle_c)
	//	m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;

	/*�}�E�X�̏���*/
	Mouse_Update();

	BasePlayer::Update();
	Update_action();

	// ���X�i�[���
	Update_listener();
}

void MyPlayer::Update_action()
{
	// �p����̃A�N�V�����N���X���g���Ȃ��̂ł����Ŗ������X�V

	switch (action_part)
	{
	case ACTION_PART::REND:
		// �j�����[�V�����̃t���[��
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			// �����ŃL���[�Ƀv�b�V��

		}
		break;

	case ACTION_PART::MANHOLE:
		if (models[(int)model_part]->GetParam(0) == 1)
		{
			if (se_receive == -1)se_receive = se->Play((isManhole) ? "�̂ڂ�": "������");
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
				(int)PLAYER_SKILL::GUN,
				(int)PLAYER_SKILL::SYURIKEN,
				(int)PLAYER_SKILL::KABUTO,
				(int)PLAYER_SKILL::ZENRYOKU
			};

			m_controlDesc.skillFlag |= FLAG[(int)select_skill];
			// �X�L���������̂ŃN�[���^�C���ݒ�
			skill_data[(int)select_skill].wait_time = skill_data[(int)select_skill].cool_time;
			skill_wait = 3;	// 1�t���[��������������Ȃ������炽�܂ɔ������Ȃ��̂�3�t���[�����炢����

			if (FLAG[(int)select_skill] == (int)PLAYER_SKILL::KABUTO)
			{
				kabuto_timer = 20 * 60;
				barrier->Action();
			}
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

}
void MyPlayer::RendFlyerSE()
{

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
			se->Stop("�}�K�W���j��", se_receive);
			se->Play("�}�K�W���j��2");
			se_step = 99;	// �X�e�b�v�I���
		}
		break;
	}
}
void MyPlayer::RendMoneySE()
{

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

}
void MyPlayer::RendZasshiSE()
{

}

void MyPlayer::Set_action(ACTION_PART part)
{
	if (action_part != part)
	{
		if (action_part == ACTION_PART::MANHOLE && part == ACTION_PART::MOVE)
		{
			isManhole ^= 1;
			event_bgm->Set_manhole(isManhole);
		}
		if (part == ACTION_PART::REND_OBJ) se_step = 0;
		Change_action(part);
	}
}

//*************************************************************************************************************************
//		�`��
//*************************************************************************************************************************
void MyPlayer::Render(iexShader *shader, char *name)
{
	BasePlayer::Render(shader, name); 
	Text::Draw(32, 480, 0xff00ffff, "%.1f", angleY);
	Text::Draw(32, 520, 0xff00ffff, "%.1f",pos.x);
	Text::Draw(32, 560, 0xff00ffff, "%.1f",pos.y);
	Text::Draw(32, 600, 0xff00ffff, "%.1f",pos.z);
	//DeferredManager.ForwardBigin();
	if(manhole_no_haninai)
		Text::Draw(320, 600, 0xffffffff, "�}���z�[���͈͓�");

	//�~�Q�[�W
	//float persent =1.0f - ((float)skill_data[(int)SKILL::GUN].wait_time / (float)skill_data[(int)SKILL::GUN].cool_time);
	//Text::Draw(32, 420, 0xff00ffff, "aaa%f", persent);
	//skillGage->Render(persent, 0, 300, 128, 128, 0, 0, 128, 128);
	//DeferredManager.ForwardEnd();
}