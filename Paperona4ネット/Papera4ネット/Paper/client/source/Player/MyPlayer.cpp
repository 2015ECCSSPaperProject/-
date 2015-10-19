
#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"

#include	"../../../share_data/Enum_public.h"
//#include	"../Mouse/Mouse.h"



//****************************************************************************************************************
//
//  初期化
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
//		更新
//*************************************************************************************************************************

void MyPlayer::Update()
{
	/*マウスの処理*/
	Mouse_Update();

	BasePlayer::Update();
}

float MOUSE_SENS = 0.8f;	// マウスの感度

/*	マウス更新	*/
void MyPlayer::Mouse_Update()
{
	//	キー設定	カーソル
	GetCursorPos(&mousePos);

	//　画面の真ん中へ移動
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

	//　マウスの動きをデスクに渡す
	m_controlDesc.mouseX = mousePos.x * 1000;
	m_controlDesc.mouseY = mousePos.y * 1000;

}

//*************************************************************************************************************************
//		描画
//*************************************************************************************************************************
void MyPlayer::Render()
{
	BasePlayer::Render();
}