
#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"


// OLL ENUM
enum {
	UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8,
	BUTTON1 = 16, BUTTON2 = 32, BUTTON3 = 64
};


//****************************************************************************************************************
//
//  
//
//****************************************************************************************************************
MyPlayer::MyPlayer() :BasePlayer()
{

}


//*************************************************************************************************************************
//		更新
//*************************************************************************************************************************
void MyPlayer::Update(void)
{
	/*操作*/
	Control();

	//プレイヤー更新
	BasePlayer::Update();
}

/*	操作　*/
void MyPlayer::Control()
{
	m_controlDesc.moveFlag = 0;

	if (KEY_Get(KEY_UP,0))
	{
		m_controlDesc.moveFlag |= UP;
	}
	if (KEY_Get(KEY_DOWN, 0))
	{
		m_controlDesc.moveFlag |= DOWN;
	}
	if (KEY_Get(KEY_RIGHT, 0))
	{
		m_controlDesc.moveFlag |= RIGHT;
	}
	if (KEY_Get(KEY_LEFT, 0))
	{
		m_controlDesc.moveFlag |= LEFT;
	}

}
