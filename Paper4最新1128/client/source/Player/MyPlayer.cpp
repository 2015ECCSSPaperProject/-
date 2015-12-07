
#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"

#include	"../../IEX/OKB.h"
#include	"../../../share_data/Enum_public.h"
#include	"../Sound/SoundManager.h"

//****************************************************************************************************************
//
//  ������
//
//****************************************************************************************************************
MyPlayer::MyPlayer() :BasePlayer()
{}

MyPlayer::~MyPlayer()
{}

//void MyPlayer::Initialize(iex3DObj *obj, iex3DObj *die)
//{
//	BasePlayer::Initialize(obj, die);
//}

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
	m_controlDesc.controlFlag &= ((BYTE)PLAYER_CONTROL::TRG_C ^ 0xff);
	if (toggle_c)
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;

	/*�}�E�X�̏���*/
	Mouse_Update();

	BasePlayer::Update();

	// ���X�i�[���
	Update_listener();
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
		}
		else
		{
			// ���܂��ĂȂ�������E�N���b�N���ĂȂ��悤�ɂ���
			m_controlDesc.skillFlag = 0;
			//�@��{�I�ɏ��0�𑗂�`
		}
		//SPI_GET_WHEELSCROLL
	}
	else if (KeyBoard(MOUSE_CENTAR))
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::ATTACK_BUTTON;
	}

	if (KeyBoard(KB_SPACE))
	{
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::SPACE;
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

	SoundManager::Set_listener(pos, front, up, move);
}

//*************************************************************************************************************************
//		�`��
//*************************************************************************************************************************
void MyPlayer::Render(iexShader *shader, char *name)
{
	BasePlayer::Render(shader, name);
	char str[64];
	if (select_skill == (int)SKILL::GUN) sprintf(str, "���S�C");
	else if (select_skill == (int)SKILL::SYURIKEN) sprintf(str, "�藠��");
	else if (select_skill == (int)SKILL::KABUTO) sprintf(str, "��");

	Text::Draw(32, 520, 0xff00ffff, "�I���X�L�� : %s", str);
	Text::Draw(32, 560, 0xff000000, "�I���X�L���̃Q�[�W : %d", skill_data[(int)select_skill].wait_time);
}