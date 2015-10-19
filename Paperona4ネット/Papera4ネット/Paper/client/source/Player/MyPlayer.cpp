
#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"



//****************************************************************************************************************
//
//  ������
//
//****************************************************************************************************************
MyPlayer::MyPlayer() :BasePlayer()
{}

MyPlayer::~MyPlayer()
{}

void MyPlayer::Initialize(iex3DObj *obj, iex3DObj *die)
{
	BasePlayer::Initialize(obj, die);
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
	/*�}�E�X�̏���*/
	Mouse_Update();

	BasePlayer::Update();
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
	mousePos.x = (float)mousePos.x * MOUSE_SENS;
	mousePos.y = (float)mousePos.y * MOUSE_SENS;
	if (GetActiveWindow() == IEX_GetWindow())
	{
		ShowCursor(FALSE);
		SetCursorPos(rc.left + iexSystem::ScreenWidth / 2, rc.top + iexSystem::ScreenHeight / 2);
	}				 

	//�@�}�E�X�̓������f�X�N�ɓn��
	m_controlDesc.mouseX = mousePos.x * 1000;
	m_controlDesc.mouseY = mousePos.y * 1000;

}

//*************************************************************************************************************************
//		�`��
//*************************************************************************************************************************
void MyPlayer::Render()
{
	BasePlayer::Render();
}