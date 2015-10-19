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
	BYTE team;
	BYTE isReady;

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
//  プレイヤーの操作命令のデータ
//---------------------------------------------------------------------
struct ContorlDesc
{
	BYTE	com; //　あっちに合わすために意味ないけどコマンド作る
	BYTE	moveFlag; //　上下左右に動かすフラグ

	float	mouseX;			//　マウスX
	float	mouseY;			//　マウスY

	BYTE	motion_no;		// モーションの番号

	BYTE	rendFlag;		// 破くときに送るフラグ
	BYTE	controlFlag;	// ぼたん
};


//=======================================================================
//	サーバークラス
//=======================================================================
class ServerManager
{
public:
	/* コマンドの種類 */
	enum EVENT_COM
	{
		NEW_USER = 0,
		INIT_DATA = 1,
		EXIT_USER = 2,
		TEAM_DATA = 3,
		INIT_GAME_DATA = 4,
		/*	USER_INIT,*/
		USER_DATA = 5,
		POSTER_DATA = 6,
	
	};

	/* ユーザーのデータ */
	struct RoomData
	{
		UserData		user[PLAYER_MAX];
		ContorlDesc		desc[PLAYER_MAX];
	};


private:
	static RoomData		m_room;
	static UDPServer*	m_pServer;// 自分と相手を繋ぐサーバ


	static void NewUser(char* data, int client);			/*	送ってきたプレイヤーにそのプレイヤーのIDを渡す			*/
	static void InitData(char* data, int client);			/*	参加													*/
	static void ExitUser(char* data, int client);			/*	クライアントとの接続を切り、使っていた情報をリセット	*/
	static void TeamData(char* data, int client);			/*	チーム選択												*/
	static void GameInitData(char* data, int client);		/*	全員がisReady==trueだとゲーム開始！						*/
	//static void InitUser(char* data, int client);
	static void UpdateUser(char* data, int client);			/*	プレイヤーの更新										*/
	static void UpdatePoster(int client);		/*	ポスターの更新											*/
public:
	static void Init();			/*	ネットプログラム開始	*/
	static void Release();		/*	ネットプログラム終り	*/
	static void Update();		/*	情報の更新				*/

	static void Render();		/*	ソケットなど情報を描画	*/


	//ルームのゲッター
	static const ContorlDesc&	GetDesc(int i){ return m_room.desc[i]; }
	static const UserData&		GetUser(int i){ return m_room.user[i]; }
	
	//　プレイヤーの操作命令をリセット
	static void ResetDesc(int i)
	{
		m_room.desc[i].com = 0;
		//m_room.desc[i].moveFlag = 0;

		//m_room.desc[i].mouseX = .0f;
		//m_room.desc[i].mouseY = .0f;
	}

};