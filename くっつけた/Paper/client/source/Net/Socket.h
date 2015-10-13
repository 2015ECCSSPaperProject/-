#pragma once
//*****************************************************************************************************************************
//
//		
//
//*****************************************************************************************************************************
#include	<string>
class UDPClient;


//===================================================================================
//   
//===================================================================================
//---------------------------------------------------------------------
//  プレイヤーのログイン状態
//---------------------------------------------------------------------
struct UserData
{
	BYTE	com;		//	データ形式	
	char	name[9];	//	名前
	BYTE	team;
	BYTE	isReady;	//

	enum
	{
		PASSIVE_USER = 0,
		ACTIVE_USER = 1,
		READY_MUTCH = 1,
		READY_MUTCH_ALL = 2,
		READY_MUTCH_SYNC = 3,
	};
};
//---------------------------------------------------------------------
//  プレイヤーの返ってくるパラメータ
//---------------------------------------------------------------------
struct PlayerData
{

	Vector3 pos;
	float angleY;
};

//=======================================================================
//	
//=======================================================================
class SocketManager
{
public:
	std::string				m_myName;
	s32						m_team;

private:
	UDPClient*				m_pClient;
	s32						m_myID;

	UserData				m_user[PLAYER_MAX];
	PlayerData				m_player[PLAYER_MAX];

	static SocketManager*	m_pInst;
	SocketManager();

	/* コマンドの種類 */
	enum EVENT_COM
	{
		NEW_USER = 0,
		INIT_DATA = 1,
		EXIT_USER = 2,
		TEAM_DATA = 3,
		INIT_GAME_DATA = 4,
		/*INIT_GAME,*/
		USER_DATA = 5 ,
	};

private:

public:
	~SocketManager();
	bool Init();


	//---------------------------------------------------------------------
	//   network
	//---------------------------------------------------------------------

	/*	更新	*/
	void UpdateTeam(int isReady);	/* 選択中 */
	void InitGame();				/* 試合前 */

	void UpdateUser();				/* プレイヤー更新 */

	/*	ゲッター・セッター	*/
	int					GetID()				const{ return m_myID; }
	const char*			GetName()				const{ return m_myName.c_str(); }
	const UserData&		GetUser(s32 client)	const{ return m_user[client]; }
	const PlayerData    GetPlayer(int idx)		const{ return m_player[idx]; }
	void				SendName();
	void				SetName(int idx, const char* name){ strcpy_s(m_user[idx].name, name); }

	


	//---------------------------------------------------------------------
	//   singleton
	//---------------------------------------------------------------------
	static SocketManager* getInstance()
	{
		if (!m_pInst)m_pInst = new SocketManager;
		return m_pInst;
	}
	static void Release()
	{
		if (m_pInst)
		{
			delete m_pInst;
			m_pInst = 0;
		}
	}
};


#define SOCKET_MANAGER ( SocketManager::getInstance() )