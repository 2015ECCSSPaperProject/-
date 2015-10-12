#pragma once
#include<list>
//*****************************************************************************************************************************
//
//		server
//
//*****************************************************************************************************************************
//=======================================================================
//	前方宣言
//=======================================================================
class UDPServer;


//---------------------------------------------------------------------
//  プレイヤーのログイン状態
//---------------------------------------------------------------------
struct UserData
{	
	BYTE com;
	char name[9];
	BYTE row;
	BYTE col;
	BYTE isReady;

	enum
	{
		PASSIVE_USER		= 0,
		ACTIVE_USER			= 1,
		READY_MUTCH			= 1,
		READY_MUTCH_ALL		= 2,
		READY_MUTCH_SYNC	= 3,
	};
};


//---------------------------------------------------------------------
//  プレーヤーの状態　毎フレーム更新
//---------------------------------------------------------------------
struct ContorlDesc
{
	BYTE	com;
	BYTE	fire;			//	発砲フラグ
	BYTE	boost;			//	ブーストフラグ
	BYTE	shield;			//	シールドフラグ
	BYTE	acceleMode;		//	アクセルフラグ
	BYTE	rapidAccele;	//	ラピッドアクセルフラグ
	BYTE	quickTurn;		//	クイックターンフラグ
	float	bodyRot;		//	車体の回転量
	float	headRot;		//	砲台の回転量
	float	cannonRot;		//	砲身の回転量

	BYTE	selectSlot;		//	
	BYTE	reqest;			//	クライアントの要求フラグ
	
};


//=======================================================================
//	サーバークラス
//=======================================================================
class ServerManager
{
public:
	enum
	{
		NEW_USER  ,
		INIT_DATA ,
		TEAM_DATA ,	

		INIT_GAME_DATA,
		USER_INIT,
		USER_DATA ,
		USER_RESPOWN,

		CHAT_DATA_RECEIVE,
		CHAT_DATA_SEND,

		LOG_DATA_SEND,
		
		ITEM_GET_INFO_SEND,
		ITEM_DATA_SEND,

		EXIT_USER,

		MUTCH_TEAM_DATA,	//	沖追加
		MUTCH_GAME_DATA,	//	沖追加
		MUTCH_GAME_END,		//	沖追加
		MUTCH_USE_SLOT,		//	沖追加
		MUTCH_COIN_DATA,	//	沖追加

		EVENT_REQUEST_RECEIVE = 30,		//中野追加

		EVENT_NODATA,

		EVENT_FLAG_SEND,							//中野追加
		EVENT_BASE_DATA_SEND,					//中野追加
		EVENT_OBJECT_DATA_SEND,				//中野追加
		EVENT_COUNTDOWN_FLAG_SEND,	//中野追加
		EVENT_CURRENT_DATA_SEND,			//中野追加



	};
	struct RoomData
	{

		UserData		user	[ PANZER_MAX ];
		ContorlDesc		desc	[ PANZER_MAX ];	

		BYTE			sendFlg;
		BYTE			receiveFlg;
		BYTE			fireFlg	[ PANZER_MAX ];
	};


private:	
	static RoomData		m_room;
	static UDPServer*	m_pServer;
	static const int CHAT_MAX = 255;

	struct Chat
	{
		BYTE sendFlg;		//	送った人のビットを立てる
		BYTE clientID;
		char message[CHAT_MAX];
	};
	static std::list<Chat> m_chat;

	struct Log
	{
		BYTE sendFlg;		//	送った人のビットを立てる
		u32  killer;
		u32  slayer;
		u32	 giveCoinNum;	//	倒した人が取得したコイン数
	};
	static std::list<Log> m_log;

	static void NewUser			( char* data, int client );
	static void InitData		( char* data, int client );
	static void TeamData		( char* data, int client ); 
	static void GameInitData	( char* data, int client );	
	static void InitUser		( char* data, int client );
	static void UpdateUser		( char* data, int client );	

	static void SendChat		( char* data, int client );
	static void ReceiveChat		( char* data, int client );

	static void SendGameLog		( char* data, int client );

	static void SendItemInfo	( char* data, int client );
	static void SendItem		( char* data, int client );


	static void ExitUser		( char* data, int client );

	static void MutchTeamData	( char* data, int client );
	static void MutchGameData	( char* data, int client );
	static void MutchGameEnd	( char* data, int client );
	static void MutchUsingSlotPlayer	( char* data, int client );
	static void MutchCoinData	( char* data, int client );

	static void SendEventData		(char*  data, int client );
	static void ReturnEventFlag	(char*  data, int client);
	static void ReturnEventBaseData	(char*  data, int client);
	static void RetunEventObjectData	(char*  data, int client);


public:
	static void Init();
	static void Release();
	static void Update();


	static const ContorlDesc&	GetDesc( int i ){ return m_room.desc[i]; }
	static const UserData&		GetUser( int i ){ return m_room.user[i]; }
	
	static void SetGameLog( u32 killer, u32 slayer );


	static void ResetDesc( int i )
	{ 	
		m_room.desc[i].com			=	0;
		m_room.desc[i].fire			=	0;
		m_room.desc[i].boost		=	0;
		//m_room.desc[i].shield		=	0;
		//m_room.desc[i].acceleMode	=	0;
		m_room.desc[i].rapidAccele	=	0;
		m_room.desc[i].quickTurn	=	0;
		m_room.desc[i].bodyRot		=	0;
		m_room.desc[i].headRot		=	0;
		m_room.desc[i].cannonRot	=	0;
	}
};