#include	<assert.h>
#include	"../template.h"
#include	"../../IEX/iextreme.h"
#include	"../Manager/InputManager.h"
#include	"../Manager/GameManager.h"
#include	"ShadingManager.h"

#include	"iextreme.h"
#include	"system/system.h"
#include	"../SystemAdd/Drawing.h"
#include	"../Net/Socket.h"

#include	"Item.h"

#include	"../Object/Tank.h"
#include	"../Object/TankController.h"
#include	"../Object/Player.h"
#include	"../Object/TankFactory.h"

#include	"../Object/Parts.h"

#include	"../Slot/Slot.h"
#include	"../Slot/Coin.h"

#include	"CapsuleWeapon.h"

#include	"Interface.h"
#include	"../Scene/sceneMain.h"
#include	"../InformationBuffer.h"

#include	"../Object/Stage.h"

#include	"../Scene/sceneMulti.h"

//*****************************************************************************************************************************
// �N���C�A���g�p�Q�[�����
//*****************************************************************************************************************************
namespace 
{
	const char* fontName = "HGS�n�p�p�߯�ߑ�";

	LPCSTR		g_NPCNameList[]	=	{
		"�݂�����",	"�Ă�Ղ�",	"�₫����",	"�Ƃ񂩂�",	"�������",	"���炠��",	"�����イ",	"���ł�",
		"���傤��",	"�炟�߂�",	
	};
}
//
//
//HANDLE				GameMasterClient::hMutex					= nullptr;
//int					GameMasterClient::myID						= -1;
//
//time_t				GameMasterClient::countDownEndTime			= 0;
//time_t				GameMasterClient::startTime					= 0;
//time_t				GameMasterClient::endTime					= 0;
//time_t				GameMasterClient::curTime					= 0;
//
//GAME_STATE_TYPE		GameMasterClient::gameState					= GAME_STATE_TYPE::GAME_ERR;
//PLAYER_STATE		GameMasterClient::tankState					= PLAYER_STATE::END;
//BYTE				GameMasterClient::inputInfo					= 0x00;
//POINT				GameMasterClient::cursorPos					= { 0, 0 };
//BYTE				GameMasterClient::selectSlot				= 0xFF;
//
//const GameUserInfo*	GameMasterClient::userInfo		= nullptr;
//int					GameMasterClient::coinNum[PANZER_MAX]		= { 0, 0, 0, 0,  0, 0, 0, 0 };
//
//Global::TEAM_NAME	GameMasterClient::curOccupyTeam				= Global::TEAM_NO;
//float				GameMasterClient::occupyRate				= 0.0f;
//D3DXCOLOR			GameMasterClient::occupyCircleColor			= D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
//
//iex2DObj*			GameMasterClient::butttonImage[2][3];
//iex2DObj*			GameMasterClient::teamPlate;
//iex2DObj*			GameMasterClient::gaugeOutSideImage;
//iex2DObj*			GameMasterClient::teamGagebar[2];
//iex2DObj*			GameMasterClient::teamNamePlate[2];
//SlotClient*			GameMasterClient::slotArray[8];
//
//
//void	GameMasterClient::Initialize( time_t startTime, BYTE myID, BYTE* teamList, char* nameList[] )
//{
//	if( userInfo != nullptr )	return;
//
//	myID = myID;
//
//	userInfo	= new	GameUserInfo[PANZER_MAX]
//	{
//		//	 ���O							�`�[��									�`�[�����ԍ�
//		{ nameList[ 0 ],		teamList[ 0 ] & 0xF0,		teamList[ 0 ] & 0x0F },
//		{ nameList[ 1 ],		teamList[ 1 ] & 0xF0,		teamList[ 1 ] & 0x0F },
//		{ nameList[ 2 ],		teamList[ 2 ] & 0xF0,		teamList[ 2 ] & 0x0F },
//		{ nameList[ 3 ],		teamList[ 3 ] & 0xF0,		teamList[ 3 ] & 0x0F },
//		{ nameList[ 4 ],		teamList[ 4 ] & 0xF0,		teamList[ 4 ] & 0x0F },
//		{ nameList[ 5 ],		teamList[ 5 ] & 0xF0,		teamList[ 5 ] & 0x0F },
//		{ nameList[ 6 ],		teamList[ 6 ] & 0xF0,		teamList[ 6 ] & 0x0F },
//		{ nameList[ 7 ],		teamList[ 7 ] & 0xF0,		teamList[ 7 ] & 0x0F },
//	};
//	
//	//	�Q�[���J�n��������	
//	const time_t	counDownTime = 10;			//	( 10�b )
//	const time_t	gameWidth = 60 * 10;		//	( 10�� )
//
//	const time_t	gameStartTime = startTime;
//
//	countDownEndTime	= gameStartTime + counDownTime;
//	startTime			= countDownEndTime;
//	endTime				= startTime + gameWidth;
//	curTime				= time( NULL );
//
//	hMutex		= CreateMutex( NULL, FALSE, NULL );
//}
//void	GameMasterClient::LoadRenderObj()
//{
//	butttonImage[0][0]	= new iex2DObj("DATA/Interface/SlotstartA.png");
//	butttonImage[0][1]	= new iex2DObj("DATA/Interface/SlotstartB.png");
//	butttonImage[0][2]	= new iex2DObj("DATA/Interface/SlotstartC.png");
//	butttonImage[1][0]	= new iex2DObj("DATA/Interface/SlotbetA.png");
//	butttonImage[1][1]	= new iex2DObj("DATA/Interface/SlotbetB.png");
//	butttonImage[1][2]	= new iex2DObj("DATA/Interface/SlotbetC.png");
//
//	
//	teamPlate			= new iex2DObj("DATA/Interface/time&coin.png");
//	gaugeOutSideImage	= new iex2DObj("DATA/Interface/br_gauge000.png");
//	teamGagebar[0]		= new iex2DObj("DATA/Interface/br_gauge001.png");
//	teamGagebar[1]		= new iex2DObj("DATA/Interface/br_gauge003.png");
//	teamNamePlate[0]	= new iex2DObj("DATA/Interface/player1.png");
//	teamNamePlate[1]	= new iex2DObj("DATA/Interface/player0.png");
//
//	/* ���ɕ��ׂ�(��) */
//	float	stx = 400.0f;
//	float	endx = -400.0f;
//	float	sp = (endx - stx) / (Global::SLOT_NUM - 1);
//	
//	for(size_t i = 0; i < 4; i++)
//	{
//		slotArray[i] = new SlotClient("DATA/Slot/Slot/Slot3.IMO");
//		slotArray[i]->SetPos(		Vector3(stx + sp*(float)i, 0.0f, -600.0f) );
//		slotArray[i]->SetAngle( 0.0f );
//		slotArray[i]->SetScale( 8.0f );
//		slotArray[i]->Update();
//
//		slotArray[i + 4] = new SlotClient("DATA/Slot/Slot/Slot3.IMO");
//		slotArray[i + 4]->SetPos(	Vector3(stx + sp*(float)i, 0.0f, +600.0f) );
//		slotArray[i + 4]->SetAngle( PI );
//		slotArray[i + 4]->SetScale( 8.0f );
// 		slotArray[i + 4]->Update();
//	}
//	
//}
//void	GameMasterClient::Exit()
//{
//	CloseHandle(hMutex);
//
//	SAFE_ARRAY_DELETE( userInfo );
//
//	SAFE_DELETE( butttonImage[0][0] );
//	SAFE_DELETE( butttonImage[0][1] );
//	SAFE_DELETE( butttonImage[0][2] );
//	SAFE_DELETE( butttonImage[1][0] );
//	SAFE_DELETE( butttonImage[1][1] );
//	SAFE_DELETE( butttonImage[1][2] );
//	SAFE_DELETE( teamPlate			);
//	SAFE_DELETE( gaugeOutSideImage	);
//	SAFE_DELETE( teamGagebar[0]		);
//	SAFE_DELETE( teamGagebar[1]		);
//	SAFE_DELETE( teamNamePlate[0]	);	
//	SAFE_DELETE( teamNamePlate[1]	);
//
//	for(size_t i = 0; i < 8; i++)
//		SAFE_DELETE( slotArray[i] )
//	
//
//}
//
///* scene���� */
//void	GameMasterClient::Update()
//{
//	WaitForSingleObject( hMutex, INFINITE );
//
//	//	�X���b�g�X�V
//	for(size_t i = 0; i < 8; i++)
//		slotArray[i]->UpdateReel();
//	
//	// �}�E�X��ԍX�V
//	inputInfo =  0x0000;
//	inputInfo |=  ( InputManager::GetMouseButton(0) == KEY_STATE_PRESSED ) ? 0x01 : 0x00;
//	inputInfo |=  ( InputManager::GetMouseButton(1) == KEY_STATE_PRESSED ) ? 0x02 : 0x00;
//	inputInfo |=  ( InputManager::GetMouseButton(2) == KEY_STATE_PRESSED ) ? 0x04 : 0x00;
//
//	cursorPos = InputManager::GetMouseCursorCoord();
//
//	//	��L�~�X�V
//	ShadingManager::SetCircleColor(  occupyCircleColor.r, occupyCircleColor.g, occupyCircleColor.b, occupyCircleColor.a );
//	ShadingManager::SetCircleWidth (  10.0f );
//	ShadingManager::SetCircleRadius( 100.0f );
//	ShadingManager::SetCirclePos(	   0.0f,	 0.0f );
//	
//	curTime = time( NULL );
//
//	ReleaseMutex( hMutex );
//}
//void	GameMasterClient::RenderUserCard( const char* name, int col, Global::TEAM_NAME team )
//{
//	int		leftX		= 10;
//	int		rightX		= 1280 - 240;
//	int		cardHeight	= 60;
//	int		addSpaceY   = 10;
//
//	const float	CARD_SCALE	= 0.23f;
//
//	bool	isMyTeam = ( team == GameMasterClient::GetMyTeam() );
//	iex2DObj*	cardImage = teamNamePlate[team];
//
//	float	imageW	= (float)cardImage->GetWidth();
//	float	imageH	= (float)cardImage->GetHeight();
//	int	dstX = ( isMyTeam ) ? leftX : rightX;
//	int dstY =	col * cardHeight + addSpaceY;
//	int dstW = (int)(imageW * CARD_SCALE);
//	int dstH = (int)(imageH * CARD_SCALE);
//
//	int	cutW	= ( isMyTeam ) ? (int)-imageW : (int)imageW;
//	int	cutH	= (int)imageH;
//	
//	cardImage->Render( dstX, dstY, dstW, dstH, 0, 0, cutW, cutH, RS_COPY, 0xFFFFFFFF, 0.0f );
//
//}
//void	GameMasterClient::RenderUser()
//{
//	int		leftX		= 10;
//	int		rightX		= 1280 - 240;
//	int		cardHeight	= 60;
//	int		addSpaceY   = 10;
//
//	for(size_t i = 0; i < PANZER_MAX; i++)
//	{
//		const int	col					= GameMasterClient::GetTeamNo( i );
//		const Global::TEAM_NAME	team	= GameMasterClient::GetTeam( i );
//
//		const char*	clientName	= GameMasterClient::GetName( i );
//		const bool	isMyTeam	= ( team == GameMasterClient::GetMyTeam() );
//
//		RenderUserCard( clientName, col, team );
//			
//		char str[64]		= { 0 };
//		const DWORD	strColor	= ( SOCKET_MANAGER->GetID() == i ) ? 0xFFFFFF00 : 0xFFFFFFFF;
//
//		wsprintf( str, "%03d", GameMasterClient::GetPlayerCoin( i ) );
//		const float	coindstX	= (float)( ( team == GameMasterClient::GetMyTeam() ) ? leftX + 48 : rightX + 152 );
//		const float	coindstY	= 32.0f + (float)( col * cardHeight + addSpaceY );
//
//		Drawing::DrawString2Dc(str, fontName, 16, coindstX, coindstY, 1.0f, 1.0f, strColor, 0.0f, RS_COPY);
//
//
//		const float namedstX	= (float)( ( team == GameMasterClient::GetMyTeam() ) ? leftX + 142 : rightX + 28 );
//		const float namedstY	= 32.0f + (float)( col * cardHeight + addSpaceY );
//
//		Drawing::DrawString2Dc(GameMasterClient::GetName( i ), fontName, 16, namedstX, namedstY, 1.0f, 1.0f, strColor, 0.0f, RS_COPY);
//	}
//
//}
//void	GameMasterClient::RenderGameInfo()
//{
//	int myTeamCoin = 0;
//	int enemyTeamCoin = 0;
//	for(size_t i = 0; i < PANZER_MAX; i++)
//	{
//		if( GameMasterClient::GetMyTeam() == GameMasterClient::GetTeam( i ) )
//			myTeamCoin	  += GameMasterClient::GetPlayerCoin( i );
//		else
//			enemyTeamCoin += GameMasterClient::GetPlayerCoin( i );
//	}
//
//	float imageScale = 0.0f;
//	int dstX,dstY,dstW,dstH;
//
//	{
//		imageScale = 0.35f;
//		
//		dstX	= iexSystem::ScreenWidth/2;
//		dstY	= 10;
//		dstW	= (int)( (float)teamPlate->GetWidth()  * imageScale  );
//		dstH	= (int)( (float)teamPlate->GetHeight() * imageScale  );
//		teamPlate->Render( dstX - dstW/2, dstY, dstW, dstH );
//	
//		imageScale = 0.35f;
//		dstX	= iexSystem::ScreenWidth/2;
//		dstY	= dstY + dstH;
//		dstW	= (int)( (float)gaugeOutSideImage->GetWidth()  * imageScale  );
//		dstH	= (int)( (float)gaugeOutSideImage->GetHeight() * imageScale  );
//	
//		gaugeOutSideImage->Render( dstX - dstW/2, dstY, dstW, dstH );
//
//	}
//
//	Global::TEAM_NAME	myTeam = GameMasterClient::GetMyTeam();
//	Global::TEAM_NAME	enemyTeam = ( myTeam == Global::TEAM_A ) ? Global::TEAM_B : Global::TEAM_A;
//	const float priority = GameMasterClient::GetOccupyRate() * 0.5f + 0.5f;
//	const float	maxWidth = ((float)gaugeOutSideImage->GetWidth() - 48) * imageScale;
//	{
//		int srcX = teamGagebar[0]->GetWidth() - (int)maxWidth;
//		int srcY = 0;
//		int srcH = teamGagebar[0]->GetHeight();
//		int srcW = 64;
//
//		dstX	= iexSystem::ScreenWidth/2 - (int)(maxWidth*0.5f);
//		dstW	= (int)( maxWidth * priority );
//		if( dstW > maxWidth )	dstW = (int)maxWidth;
//
//		dstH	= (int)( (float)teamGagebar[0]->GetHeight() * imageScale  );
//
//		teamGagebar[myTeam]->Render( dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH );
//	
//		dstX	= dstX + dstW;
//		dstW	= (int)maxWidth - dstW;
//		
//		teamGagebar[enemyTeam]->Render( dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH );
//	
//	}
//
//	const int fontSize = 24;
//	dstY = 45;
//	char strCoinNum[10] = { 0 };
//	{
//		dstX	= iexSystem::ScreenWidth/2 - 128;
//
//		sprintf_s( strCoinNum, "%4d", myTeamCoin );
//		
//		Drawing::DrawString2Dc( strCoinNum, fontName, fontSize, (float)dstX, (float)dstY, 0.0f, RS_COPY );
//
//
//	}
//	{
//
//		dstX	= iexSystem::ScreenWidth/2 + 58;
//
//		sprintf_s( strCoinNum, "%4d", enemyTeamCoin );
//		
//		Drawing::DrawString2Dc( strCoinNum, fontName, fontSize, (float)dstX, (float)dstY, 0.0f, RS_COPY );
//	}
//
//	{
//		dstX	= iexSystem::ScreenWidth/2 - 48;
//		dstY	= 58;
//		const int fontSize = 28;
//
//		u32	sec = (u32)( GameMasterClient::endTime - curTime );
//		u32	min = sec / 60;
//
//		char	timeStr[64];
//		wsprintf(timeStr, "%02d : %02d", min, sec % 60);
//		Drawing::DrawString2Dc(timeStr, "HGS�n�p�p�߯�ߑ�", fontSize, (float)dstX, (float)dstY, 1.0f, 1.0f, 0xFFFFFFFF, 0.0f, RS_COPY);
//		
//	}
//
//
//	RenderUser();
//
//}
//void	GameMasterClient::RenderSlot()
//{	
//	for(size_t i = 0; i < 8; i++)
//		slotArray[i]->Render(uvAnime, "copy");
////	for(size_t i = 0; i < 8; i++)
////		ShadingManager::RenderReserveTank(slotArray[i]);
//}
//
///* ���ݒ�( �Ƃ肠��������̂� )�@*/
//void	GameMasterClient::SetGameState( BYTE state )
//{
//	gameState = static_cast<GAME_STATE_TYPE>(state);
//}
//void	GameMasterClient::SetFlagRate( float rate )
//{
//	occupyRate = rate;
//}
//void	GameMasterClient::SetTankState( BYTE state )
//{
//	tankState = static_cast<PLAYER_STATE>(state);
//}
//void	GameMasterClient::SetCoin( int no, BYTE coin )
//{
//	coinNum[ no ] = (int)coin;
//}
//void	GameMasterClient::SetSlot( int no, WORD action )
//{
//	slotArray[ no ]->SetState( action );
//}
//void	GameMasterClient::SetSelectSlot( BYTE no )
//{
//	selectSlot = no;
//}


namespace		
{	
	const int	 initCoinNum		= 10;				//	�����l( �R�C�� ) 
	PLAYER_STATE initState			= DEATH;			//	�����l( ��ԁ@ ) 
}

//*****************************************************************************************************************************
// �N���C�A���g�p�Q�[�����
//*****************************************************************************************************************************

// �Q�[�����
GAME_STATE_TYPE		GameMasterClientA::gameState						= MUTCH;
GameUserInfo*		GameMasterClientA::userInfo							= nullptr;
time_t				GameMasterClientA::startTime						= 0;
time_t				GameMasterClientA::endTime							= 0;
time_t				GameMasterClientA::curTime							= 0;
time_t				GameMasterClientA::gameWidth						= 0;

//	�������g�̏��
int					GameMasterClientA::myID								= -1;
PLAYER_STATE		GameMasterClientA::tankState						= DEATH;
SLOT_STATE			GameMasterClientA::slotState						= SELECT;
BYTE				GameMasterClientA::userReqestSlotButton				= 0x00;
BYTE				GameMasterClientA::userSelectSlot					= 0xFF;

// �X���b�g�p�ϐ�
SlotClient*			GameMasterClientA::slotArray[SLOT_NUM]				= { nullptr };

//	��L�����
Global::TEAM_NAME	GameMasterClientA::occupyTeam						= Global::TEAM_NO;
float				GameMasterClientA::occupyRate						= 0.0f;
Vector4				GameMasterClientA::circleColor						= Vector4( 1.0f, 1.0f, 1.0f, 1.0f );

//	���ꎞ�ێ��p
BYTE				GameMasterClientA::netReciveTankState				=  0 ;
WORD				GameMasterClientA::netReciveCoin[PANZER_MAX]		={ 0 };
WORD				GameMasterClientA::netAssistData[PANZER_MAX]		={ 0 };
WORD				GameMasterClientA::netOccupyData[PANZER_MAX]		={ 0 };
BYTE				GameMasterClientA::netReciveGameState				=  0;
WORD				GameMasterClientA::netSlotActionInfo[SLOT_NUM]		= { 0x0000 };

// �`��p�ϐ�
iex2DObj*			GameMasterClientA::teamPlate						=  nullptr;
iex2DObj*			GameMasterClientA::gaugeOutSideImage				=  nullptr;
iex2DObj*			GameMasterClientA::teamGagebar[2]					= {nullptr};
iex2DObj*			GameMasterClientA::teamNamePlate[2]					= {nullptr};
iex2DObj*			GameMasterClientA::teamCPUPlate[2]					= {nullptr};

//****************************************************************************************************
//	�����O
//****************************************************************************************************
void	GameMasterClientA::LoadInfoImage()
{
	// �ǂݍ���:�摜
	teamPlate			= new iex2DObj( "DATA/Interface/time&coin.png"		);
	gaugeOutSideImage	= new iex2DObj( "DATA/Interface/br_gauge000.png"	);
	teamGagebar[0]		= new iex2DObj( "DATA/Interface/br_gauge003.png"	);
	teamGagebar[1]		= new iex2DObj( "DATA/Interface/br_gauge001.png"	);
	teamNamePlate[0]	= new iex2DObj( "DATA/Interface/player0.png"		);
	teamNamePlate[1]	= new iex2DObj( "DATA/Interface/player1.png"		);
	teamCPUPlate[0]		= new iex2DObj( "DATA/Interface/player_cpu1.png"	);
	teamCPUPlate[1]		= new iex2DObj( "DATA/Interface/player_cpu2.png"	);
	
	//�@�ǂݍ��݂Ə�����:�X���b�g
	char*	filename	= "DATA/Slot/Slot/Slot4.IMO";
	WORD	state = 0x0000;

	const float SLOT_SCALE		= 8.0f;
	struct 
	{
		Vector3 pos;
		float	angle;
		float	scale;
	} slotConfig[PANZER_MAX]
	{
		{ Vector3(  500.0f,  0.0f, -850.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3(  170.0f,  0.0f, -850.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3( -155.0f,  0.0f, -850.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3( -480.0f,  0.0f, -850.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3(  480.0f,  0.0f,  850.0f ), PI	, SLOT_SCALE },
		{ Vector3(  155.0f,  0.0f,  850.0f ), PI	, SLOT_SCALE },
		{ Vector3( -170.0f,  0.0f,  850.0f ), PI	, SLOT_SCALE },
		{ Vector3( -500.0f,  0.0f,  850.0f ), PI	, SLOT_SCALE },
	};
		
	for(size_t i = 0; i < SLOT_NUM; i++)
	{
		slotArray[i] = new SlotClient( filename		);
		slotArray[i]->SetPos( slotConfig[i].pos		);
		slotArray[i]->SetAngle( slotConfig[i].angle );
		slotArray[i]->SetScale( slotConfig[i].scale );
		slotArray[i]->SetState( state );
		slotArray[i]->Update();
	}


}
void	GameMasterClientA::GameInitialize()
{
	// _������:�Q�[�����
	FILE*	fp = nullptr;
	fopen_s( &fp, "./DATA/GAMETIME.txt", "r" );
	assert( fp );
	fscanf_s( fp, "%d", &gameWidth );
	fclose( fp );

	GameMasterClientA::startTime		= time( NULL );	
	GameMasterClientA::endTime			= startTime + gameWidth;
	GameMasterClientA::curTime			= time( NULL );	
	GameMasterClientA::occupyTeam		= Global::TEAM_NO;
	GameMasterClientA::circleColor		= Vector4( 1.0f, 1.0f, 1.0f, 1.0f );

	// _������:�������g�̏��
	GameMasterClientA::myID = SOCKET_MANAGER->GetID();

	// _������:�Q�[����V��ł���v���C���[
	SAFE_ARRAY_DELETE( userInfo );
	struct 
	{
		char	name[64];
		BYTE	col;
		BYTE	row;
		bool	isAI;
	} playerList[PANZER_MAX];
	int colWork[2] = { 0, 0 };
	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		playerList[i].isAI = ( SOCKET_MANAGER->GetUser(i).com == 0 );
		if( playerList[i].isAI == true ) continue;
		int team = ( SOCKET_MANAGER->GetUser(i).row == 0 ) ? Global::TEAM_A : Global::TEAM_B;
		sprintf_s( playerList[i].name, SOCKET_MANAGER->GetUser(i).name );
		playerList[i].col	= colWork[team]++;
		playerList[i].row	= team;
	}
	int aiCnt = 1;

	int		numCPUNames	=	sizeof( g_NPCNameList ) / sizeof( LPCSTR );
	bool*	useName		=	new	bool[ numCPUNames ];
	for( int i = 0; i < PANZER_MAX; i++ ){
		useName[i] = false;
	}

	for (size_t i = 0; i < PANZER_MAX; i++)
	{
		playerList[i].isAI = (SOCKET_MANAGER->GetUser(i).com == 0);
		if (playerList[i].isAI == false) continue;
		int team = (colWork[0] < 4) ? Global::TEAM_A : Global::TEAM_B;
		ZeroMemory(playerList[i].name, sizeof(playerList[i].name));
		int write = sprintf_s(playerList[i].name, "CPU%d", aiCnt);

		//	�g�p�ł��閼�O��T���iNPC�p�̖��O������Ȃ�������~�܂�j
		int	useNum = 0;
		while (true){
			useNum = rand() % numCPUNames;

			if (useName[useNum] == false){
				useName[useNum] = true;
				break;
			}
		}
		sprintf_s(playerList[i].name, g_NPCNameList[useNum]);

		playerList[i].col = colWork[team]++;
		playerList[i].row = team;

		++aiCnt;
	}
	delete[]	useName;

	SAFE_ARRAY_DELETE(userInfo);
	userInfo = new	GameUserInfo[PANZER_MAX]{
		//	 ���O					�`�[��					�`�[�����ԍ�
		{ playerList[0].name, playerList[0].row, playerList[0].col, initCoinNum, playerList[0].isAI },
		{ playerList[1].name, playerList[1].row, playerList[1].col, initCoinNum, playerList[1].isAI },
		{ playerList[2].name, playerList[2].row, playerList[2].col, initCoinNum, playerList[2].isAI },
		{ playerList[3].name, playerList[3].row, playerList[3].col, initCoinNum, playerList[3].isAI },
		{ playerList[4].name, playerList[4].row, playerList[4].col, initCoinNum, playerList[4].isAI },
		{ playerList[5].name, playerList[5].row, playerList[5].col, initCoinNum, playerList[5].isAI },
		{ playerList[6].name, playerList[6].row, playerList[6].col, initCoinNum, playerList[6].isAI },
		{ playerList[7].name, playerList[7].row, playerList[7].col, initCoinNum, playerList[7].isAI },
	};

	// _�N���A:�T�[�o�[���瑗���Ă�����
	slotState			= SLOT_STATE::INIT;
	tankState			= initState;
	netReciveGameState	= GAME_STATE_TYPE::WAIT;
	netReciveTankState	= initState;
	userSelectSlot			= 0x00;
	userReqestSlotButton	= 0x00;
	for (size_t i = 0; i < 8; i++)
	{
		netReciveCoin[i]		= initCoinNum;
		netSlotActionInfo[i]	= 0x0000;
		slotArray[i]->Clear();
	}

	PlayerEffectCoin::Initialize();
	PlayerGameInfoCard::Initialize();

	// �ύX�\��
	ShadingManager::SetCircleColor(0xFFffffff);
	ShadingManager::SetCirclePos(0.0f, 0.0f);
	ShadingManager::SetCircleWidth(10.0f);
	ShadingManager::SetCircleRadius(200.0f);

}

//****************************************************************************************************
//	������
//****************************************************************************************************
void GameMasterClientA::UpdateServerInfo()
{
	// �Q�[���̏��
	gameState = static_cast<GAME_STATE_TYPE>(netReciveGameState);
	curTime = time(NULL);
	if (gameState == GAME_STATE_TYPE::WAIT)
	{
		GameMasterClientA::startTime = time(NULL);
		GameMasterClientA::endTime = startTime + gameWidth;
	}

	// �������g
	tankState = static_cast<PLAYER_STATE>(netReciveTankState);

	// �v���C���[
	for (int i = 0; i < PANZER_MAX; ++i)
	{
		PanzerData panzer = SOCKET_MANAGER->GetPanzer(i);
		tank[i]->LegUVPosUpdate(panzer.pos, panzer.orien);
		tank[i]->SetCenterGravity(panzer.centerGravitiy);
		tank[i]->SetPos(panzer.pos);
		tank[i]->SetOrien(panzer.orien);
		tank[i]->SetCannonAngle(panzer.canonAngle);
		tank[i]->SetHeadAngle(panzer.headAngle);
		tank[i]->SetPartsNum(panzer.parts);
		tank[i]->SetGold(panzer.isGold[0], panzer.isGold[1], panzer.isGold[2]);
		tank[i]->SetEnergy(panzer.energy);
		if (panzer.IsBoost)
		{
			if (i != GameMasterClientA::GetMyID())
			{
				if (tank[i]->IsAlive())
				{
					Burning* p = new Burning(i);
					if (!BurningManager::Set(p))
					{
						delete p;
					}
				}
			}
		}
		userInfo[ i ].coinNum		= static_cast<int>( netReciveCoin[ i ] );
	}

	// �X���b�g
	for(size_t i = 0; i < 8; i++)
	{
		slotArray[i]->SetState(netSlotActionInfo[i]);
		slotArray[i]->UpdateReel();
	}

	//	�X�e�[�W���
	UpdateOccupyRate();

	
	
}

void GameMasterClientA::UpdateSlots()
{
	for(size_t i = 0; i < SLOT_NUM; i++)
		slotArray[ i ]->UpdateReel();
}
void GameMasterClientA::UpdateOccupyRate()
{
	// ���ݐ�L���`�[������
		 if( occupyRate >= +1.0f )	occupyTeam = Global::TEAM_A;
	else if( occupyRate <= -1.0f )	occupyTeam = Global::TEAM_B;
	else if( occupyRate > -0.001f && occupyRate < +0.001f ) 
		occupyTeam = Global::TEAM_NO;

	// �T�[�N���F�ݒ�		
	Vector4	teamColor[3];
	teamColor[0]	= Vector4(1.0f, 1.0f, 1.0f, 0.8f);	// ��L�`�[���i ����	�j
	teamColor[1]	= Vector4(0.0f, 0.0f, 1.0f, 0.8f);	// ��L�`�[���i ����	�j
	teamColor[2]	= Vector4(1.0f, 0.0f, 0.0f, 0.8f);	// ��L�`�[���i �G		�j
	
	Vector4	color = teamColor[0];
	
		 if( GetMyTeam() == occupyTeam )
		color = teamColor[1];
	else if( GetEnemyTeam() == occupyTeam )
		color = teamColor[2];
	
	D3DXVec4Lerp( &circleColor, &circleColor, &color, 0.1f );
	ShadingManager::SetCircleColor( circleColor );
		
}

//****************************************************************************************************
// ������
//****************************************************************************************************
void GameMasterClientA::GameExit()
{
	SAFE_ARRAY_DELETE( userInfo );
}
void GameMasterClientA::ReleaseInfoImage()
{
	SAFE_DELETE( teamPlate				);
	SAFE_DELETE( gaugeOutSideImage	);
	SAFE_DELETE( teamGagebar[0]		);
	SAFE_DELETE( teamGagebar[1]		);
	SAFE_DELETE( teamNamePlate[0]	);
	SAFE_DELETE( teamNamePlate[1]	);
	SAFE_DELETE( teamCPUPlate[0]	);
	SAFE_DELETE( teamCPUPlate[1]	);
	for(size_t i = 0; i < 8; i++)
		SAFE_DELETE( slotArray[i] );

	PlayerEffectCoin::Release();
	PlayerGameInfoCard::Release();
}

//****************************************************************************************************
// �`��
//****************************************************************************************************
void GameMasterClientA::RenderPlayerInfo()
{
//		const float	cardHeight		= 60;
//		const int	strSize_Name	= 13;
//		const float	addspaceY_Name	= 44;
//		const int	strSize_Coin	= 17;
//		const float	addspaceY_Coin	= 41.0f;
//		const float	c[][ 2 ] = 
//		{
//			{ 51.0f,		1280.0f	-	 90.0f	},			//	���O
//			{ 176.0f,		1280.0f	-	194.0f	},			//	�R�C��
//		};
//	
//		//	�����`�[���͍����ŕ\��
//		Global::TEAM_NAME	myTeam = GetMyTeam();
//		const int	tbl[] = {
//			( myTeam == GetPlayerTeam( 0 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 1 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 2 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 3 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 4 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 5 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 6 )) ? 0 : 1,
//			( myTeam == GetPlayerTeam( 7 )) ? 0 : 1
//		};
//	
//		
//		int work[2] = { 0 ,0 };
//		for(size_t i = 0; i < PANZER_MAX; ++i )
//		{
//			DWORD	color	= ( GetMyID() == i ) ? 0xFFFFFF00 : 0xFFFFFFFF;
//	
//			Global::TEAM_NAME team = GetPlayerTeam(i);
//			float		dstX	= c[ 0 ][ tbl[ i ] ];
//			float		dstY	= (float)work[ team ] * cardHeight + addspaceY_Name;
//			++work[ team ];
//			Drawing::DrawString2Dc( GetPlayerName( i ), fontName, strSize_Name, dstX, dstY, 1.0f, 1.0f, color, 0.0f, RS_COPY );
//		}
//		work[0] = 0;
//		work[1] = 0;
//		for(size_t i = 0; i < PANZER_MAX; ++i )
//		{
//			char	srtNum[10] = { 0 };
//			DWORD	color	= ( GetMyID() == i ) ? 0xFFFFFF00 : 0xFFFFFFFF;
//			Global::TEAM_NAME team = GetPlayerTeam(i);
//			float		dstX	= c[ 1 ][ tbl[ i ] ];
//			float		dstY	= (float)work[ team ] * cardHeight + addspaceY_Coin;
//	//		sprintf_s( srtNum, "%03d", GetPlayerCoin( i ) );
//			sprintf_s( srtNum, "%03d", PlayerEffectCoin::GetFacadeCoinNum( i ) );
//			Drawing::DrawString2Dc( srtNum, fontName, strSize_Coin, dstX, dstY, 1.0f, 1.0f, color, 0.0f, RS_COPY );
//	
//			++work[ team ];
//		}
}
void GameMasterClientA::RenderGameInfo()
{
	Global::TEAM_NAME myTeam	= GetMyTeam();
	Global::TEAM_NAME enemyTeam = ( myTeam == Global::TEAM_A ) ? Global::TEAM_B : Global::TEAM_A;

	{
		int teamCoin[ 2 ] = { 0, 0 };
		for(size_t i = 0; i < PANZER_MAX; i++)
		{
			Global::TEAM_NAME	team = userInfo[ i ].team;
			if( myTeam == team )
				teamCoin[myTeam]	+= userInfo[ i ].coinNum;
			else
				teamCoin[enemyTeam] += userInfo[ i ].coinNum;
		}
		
		float  c[ 2 ] = 
			{ (float)iexSystem::ScreenWidth / 2 -128,  (float)iexSystem::ScreenWidth / 2 + 58 };
		
		int		fontSize	= 24;
		float	height		= 46;
		DWORD	color		= 0xFFFFFFFF;
		char	str[ 64 ]	= { 0 };
		sprintf_s( str, "%4d", teamCoin[ 0 ] );
		Drawing::DrawString2Dc( str, fontName, fontSize, c[ myTeam ],    height, 1.0f, 1.0f, color, 0.0f, RS_COPY );
		sprintf_s( str, "%4d", teamCoin[ 1 ] );
		Drawing::DrawString2Dc( str, fontName, fontSize, c[ enemyTeam ], height, 1.0f, 1.0f, color, 0.0f, RS_COPY );
	}

	{
		int		fontSize = 25;
		float	x		 = (float)iexSystem::ScreenWidth / 2 - 44;
		float	y		 = 59;
		DWORD	color;

		int sec		= (int)(endTime - (int)curTime);
		int minute	= (int)sec / 60;
		if( sec < 0 )	
		{	
			sec		= 0;
			minute	= 0;
		}

		//	�T�b�O���ƐF����
		if( minute == 0 && sec%60 < 5 )	color = 0xFFFF0000;
		else							color = 0xFFFFFFFF;

		//	
		char	str[ 64 ] = { 0 };
		sprintf_s( str, "%02d�F%02d", minute, sec%60 );
		Drawing::DrawString2Dc( str, fontName, fontSize, x, y, 1.0f, 1.0f, color, 0.0f, RS_COPY );
	}

	float priority = ( GetMyTeam() == Global::TEAM_A ) ? GameMasterClientA::occupyRate : -GameMasterClientA::occupyRate;
	if( priority > 1.0f )	priority = 1.0f;
	if( priority < -1.0f )	priority = -1.0f;
	float imageScale = 0.35f;
	int dstX,dstY,dstW,dstH;
	const float	maxWidth = ((float)gaugeOutSideImage->GetWidth() - 50) * imageScale;
	{
		dstX	= iexSystem::ScreenWidth/2;
		dstY	= 10;
		dstW	= (int)( (float)teamPlate->GetWidth()  * imageScale  );
		dstH	= (int)( (float)teamPlate->GetHeight() * imageScale  );
	
		imageScale = 0.35f;
		dstX	= iexSystem::ScreenWidth/2;
		dstY	= dstY + dstH;
		dstW	= (int)( (float)gaugeOutSideImage->GetWidth()  * imageScale  );
		dstH	= (int)( (float)gaugeOutSideImage->GetHeight() * imageScale  );

		int srcX = teamGagebar[0]->GetWidth() - (int)maxWidth;
		int srcY = 0;
		int srcH = teamGagebar[0]->GetHeight();
		int srcW = 64;

		dstX	 = iexSystem::ScreenWidth/2 - (int)(maxWidth*0.5f);
		dstW	 = (int)( maxWidth * ( priority *0.5 + 0.5f ) );
		if( dstW > maxWidth )	dstW = (int)maxWidth;

		dstH	= (int)( (float)teamGagebar[0]->GetHeight() * imageScale  );
	
		const int left	= 0;
		const int right = 1;

		//	�[�����`��
		if( priority <= -1.0f )
			teamGagebar[right]->Render( dstX - 6, dstY, 8, dstH, 14, 0, 16, srcH );
		else
			teamGagebar[left]->Render(    dstX - 6, dstY, 8, dstH, 14, 0, 16, srcH );

		teamGagebar[left]->Render( dstX, dstY, dstW, dstH, srcX, srcY, srcW, srcH );

		dstX	= dstX + dstW;
		dstW	= (int)maxWidth - dstW; 

		//	�[�����`��
		if( priority >= +1.0f )
			teamGagebar[left]->Render(	dstX + (int)dstW - 4, dstY, 8, dstH, -30, 0, 16, srcH );
		else
			teamGagebar[right]->Render( dstX + (int)dstW - 4, dstY, 8, dstH, -30, 0, 16, srcH );
		
		teamGagebar[right]->Render( dstX - 2, dstY, dstW, dstH, srcX, srcY, srcW, srcH );
	}
}
void GameMasterClientA::RenderPlayerCardImage()
{
//	int		leftX		= 10;
//	int		rightX		= 1280 - 240;
//	int		cardHeight	= 60;
//	int		addSpaceY	= 10;
//
//	const float	CARD_SCALE	= 0.23f;
//
//	float imageW	= (float)teamNamePlate[ 0 ]->GetWidth();
//	float imageH	= (float)teamNamePlate[ 0 ]->GetHeight();
//	int   dstW		= (int)(imageW * CARD_SCALE);
//	int   dstH		= (int)(imageH * CARD_SCALE);
//	int	  cutH		= (int)imageH;
//	
//	struct 
//	{
//		int posX;
//		int srcW;
//	} c [2] = {
//		{  leftX,   (int)+imageW },
//		{  rightX , (int)-imageW }
//	};
//	const int left	= 0;
//	const int right = 1;
//
//	Global::TEAM_NAME myTeam = GetMyTeam();
//
//	int colCnt[2] = {0,0};
//	for(size_t i = 0; i < PANZER_MAX; i++)
//	{
//		Global::TEAM_NAME	team = GetPlayerTeam( i );
//		
//		int row = ( team == myTeam ) ? left : right;
//		int col = colCnt[row] * cardHeight + addSpaceY;
//		teamNamePlate[ row ]->Render( c[ row ].posX , col, dstW, dstH, 0, 0, c[ row ].srcW, cutH, RS_COPY, 0xFFFFFFFF, 0.0f );
//		
//		bool isAI = userInfo[i].isAI;
//		if( isAI == true )
//			teamCPUPlate[ row ]->Render( c[ row ].posX , col, dstW, dstH, 0, 0, c[ row ].srcW, cutH, RS_COPY, 0xFFFFFFFF, 0.0f );
//
//		++colCnt[ row ];
//	}
}
void GameMasterClientA::RenderCenterCardImage()
{
	float imageScale = 0.0f;
	int dstX,dstY,dstW,dstH;
	int srcX,srcY,srcW,srcH;

	{
		imageScale = 0.35f;
		
		dstX	= iexSystem::ScreenWidth/2;
		dstY	= 10;
		dstW	= (int)( (float)teamPlate->GetWidth()  * imageScale  );
		dstH	= (int)( (float)teamPlate->GetHeight() * imageScale  );
		srcX	= 0;
		srcY	= 0;
		srcW	= -(int)teamPlate->GetWidth();
		srcH	= teamPlate->GetHeight();

		teamPlate->Render( dstX - dstW/2, dstY, dstW, dstH, srcX, srcY, srcW, srcH );
		
		imageScale = 0.35f;
		dstX	= iexSystem::ScreenWidth/2;
		dstY	= dstY + dstH;
		dstW	= (int)( (float)gaugeOutSideImage->GetWidth()  * imageScale  );
		dstH	= (int)( (float)gaugeOutSideImage->GetHeight() * imageScale  );
	
		gaugeOutSideImage->Render( dstX - dstW/2, dstY, dstW, dstH );

	}





}
void GameMasterClientA::RenderSlots()
{	
	//for(size_t i = 0; i < SLOT_NUM; i++)
	//	slotArray[i]->Render(uvAnime, "copy");
	
	static	const	D3DXCOLOR	c_BackLightColor[2]	=	{
		D3DXCOLOR(	1.0f,	0.25f,	0.25f,	1.0f	),
		D3DXCOLOR(	1.0f,	0.25f,	0.25f,	1.0f	),
	};

	for( size_t i = 0; i < 8; i++ ){
		D3DXCOLOR	backLightColor	=	c_BackLightColor[ i / 4 ];
		float		backLightPower	=	2.0f;
		float		backLightRate	=	1.0f;
		float		alpha			=	1.0f;

		ShadingManager::RenderReserveSlot( slotArray[i], alpha, backLightColor, backLightPower, backLightRate );
	}
}

//****************************************************************************************************
// ���ݒ�
//****************************************************************************************************
void GameMasterClientA::SetReciveSlotUV( int id, int reel, float uv ){
	 slotArray[id]->SetReelUV( reel, uv );		}

//****************************************************************************************************
// DBG�p
//****************************************************************************************************
void GameMasterClientA::dbgInfo()
{

	DWORD	backColor	= 0x00000000;
	DWORD	flag		= D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;

	iexSystem::GetDevice()->Clear( 0, NULL, flag, backColor, 1.0f, 0 );

	char* gameStateName[]	= {	"WAIT",	"MUTCH","GAME_RESULT",		};
	char* teamName[]		= { "TEAM_A", "TEAM_B" };
	char* tankStateName[]	= { "COMBAT", "DETH", "RESPOWN" };
	char* slotStateName[]	= { "SELECT", "PLAY", "UNION", "NONE" };

	const int	stLine		= 96;
	const int	chSize		= 24;
	const char* fontName	= "HGS����B";
	const int	strWidth	= iexSystem::ScreenWidth;

	int col	=  0;
	
	IEX_DrawText(" -- [ sceneMutch ] -- ", 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
	++col;
	{	
		char tmp[256] = {0};
		int sec = (int)(endTime - curTime);
		int min = sec / 60;
		int	totalCoin[2] = { 0, 0 };
		for(size_t i = 0; i < PANZER_MAX; i++)
		{
			Global::TEAM_NAME	team		= userInfo[i].team;
			int					playerCoin	= 
					( userInfo[i].coinNum >= 0 ) ? userInfo[i].coinNum : 0;
			totalCoin[team] += playerCoin;
		}
		sprintf_s( tmp, "[ %s ], [ %02d : %02d ], TEAM_A( %03d ) vs TEAM_B( %03d ) ",
						gameStateName[gameState], min,   sec%60,   totalCoin[0],   totalCoin[1] );

		IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
		++col;
	}

	IEX_DrawText( " -- [ player ] -- ", 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
	++col;
	Global::TEAM_NAME	team		= GetMyTeam();
	PLAYER_STATE		state		= GetMyTankState();
	BYTE				reqestInfo	= GetReqest();
	char	tmp[256] = { 0 };
	sprintf_s( tmp, "%02d : [ %s ( %s ) ], team : %s_%02d, tankState : %7s  coin : %03d cliant : %d, hp[ %.5lf / %.5lf ], request[ %02X ]",
			myID,
			userInfo[myID].name,
			"false",
			teamName[GetMyTeam()],
			userInfo[myID].teamNo,
			tankStateName[tankState],
			userInfo[myID].coinNum,
			myID,
			tank[myID]->GetDamage(),	tank[myID]->GetHP(),
			reqestInfo );

	IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
	++col;

	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		char	tmp[256] = { 0 };
		int p[3];
		tank[i]->GetPartsNum(p);
		sprintf_s( tmp, "parts[ %d, %d, %d ]", p[0], p[1], p[2] );
		IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
		++col;
	}

	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		WORD	flg = GameMasterClientA::GetSlot(i)->GetActionInfo();
		float	uv[3] ={
			slotArray[i]->dbgGetTU(0),
			slotArray[i]->dbgGetTU(1),
			slotArray[i]->dbgGetTU(2),
		};
		int* step = slotArray[i]->dbgGetstep();

		sprintf_s( tmp, "slot[ %04X ] UV[ %0.5f,  %0.5f,  %0.5f ], step[ %d, %d, %d ]", 
			flg, uv[0], uv[1], uv[2], step[0], step[1], step[2] );

		IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
		++col;
	}


}

