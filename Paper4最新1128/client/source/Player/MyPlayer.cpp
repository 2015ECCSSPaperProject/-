
#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"

#include	"../../IEX/OKB.h"
#include	"../../../share_data/Enum_public.h"
#include	"../Sound/SoundManager.h"



//****************************************************************************************************************
//
//  初期化
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
//		更新
//*************************************************************************************************************************

void MyPlayer::Update()
{
	/*入力受付処理*/
	Control_all();

	// Cキーのトグル
	m_controlDesc.controlFlag &= ((BYTE)PLAYER_CONTROL::TRG_C ^ 0xff);
	if (toggle_c)
		m_controlDesc.controlFlag |= (BYTE)PLAYER_CONTROL::TRG_C;

	/*マウスの処理*/
	Mouse_Update();

	BasePlayer::Update();

	// リスナー情報
	Update_listener();
}


/*	操作　*/
void MyPlayer::Control_all()
{
	// 初期化
	m_controlDesc.moveFlag &= 0x00000000;
	m_controlDesc.controlFlag &= 0x00000000;

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
	mousePos.x = (LONG)((float)mousePos.x * MOUSE_SENS);
	mousePos.y = (LONG)((float)mousePos.y * MOUSE_SENS);
	if (GetActiveWindow() == IEX_GetWindow())
	{
		ShowCursor(FALSE);
		SetCursorPos(rc.left + iexSystem::ScreenWidth / 2, rc.top + iexSystem::ScreenHeight / 2);
	}				 

	//　マウスの動きをデスクに渡す
	m_controlDesc.mouseX = (float)mousePos.x * 1000;
	m_controlDesc.mouseY = (float)mousePos.y * 1000;

}

void MyPlayer::Update_listener()
{
	const Vector3 front(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33);
	const Vector3 up(models[(int)model_part]->TransMatrix._21, models[(int)model_part]->TransMatrix._22, models[(int)model_part]->TransMatrix._23);

	SoundManager::Set_listener(pos, front, up, move);
}

//*************************************************************************************************************************
//		描画
//*************************************************************************************************************************
void MyPlayer::Render()
{
	BasePlayer::Render();
	//Text::Draw(32, 600, 0xff000000, "ゲージ : %d", god_gage);
	//Text::Draw(32, 620, 0xff000000, "50以上かつ真ん中クリックで紙鉄砲", god_gage);
}