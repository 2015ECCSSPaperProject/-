#pragma once
#include<list>
#include	"../../../share_data/Enum_public.h"
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
	BYTE	skillFlag;		// 必殺技
};

//---------------------------------------------------------------------
//  ユーザーのレイヤーのパラメータ
//---------------------------------------------------------------------
struct LayerData
{

	struct LAYER_DATA
	{
		/*種類とナンバー*/
		int kind;
		int num;
		float x, y;//	場所
		int size;//		サイズ
		bool isHold;//	掴んでるか
	};
	LAYER_DATA layerdata[LAYER_MAX];

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
		STAGE_DATA = 6,
		LAYER_DATA = 7,
		SCORE_DATA = 8,
		END_GAME = 9
	};

	/* ユーザーのデータ */
	struct RoomData
	{
		UserData		user[PLAYER_MAX];
		ContorlDesc		desc[PLAYER_MAX];
		LayerData		layer[PLAYER_MAX];// レイヤー
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
	static void UpdateStage(int client);		/*	ポスターの更新											*/
	static void UpdateLayer(char* data, int client);		/*	レイヤーの更新									　　	*/
	static void UpdateScore(int client);		/*	スコアの更新	*/

	static void GameEndGame(char* data, int client);			/* エンドゲーム */


public:
	static void Init();			/*	ネットプログラム開始	*/
	static void Release();		/*	ネットプログラム終り	*/
	static void Update();		/*	情報の更新				*/

	static void Render();		/*	ソケットなど情報を描画	*/


	//ルームのゲッター
	static const ContorlDesc&	GetDesc(int i){ return m_room.desc[i]; }
	static const UserData&		GetUser(int i){ return m_room.user[i]; }
	static const LayerData&		GetLayer(int i){ return m_room.layer[i]; }


	//　プレイヤーの操作命令をリセット
	static void ResetDesc(int i)
	{
		m_room.desc[i].com = 0;
		//m_room.desc[i].moveFlag = 0;

		//m_room.desc[i].mouseX = .0f;
		//m_room.desc[i].mouseY = .0f;
	}
	static void InitDesc()
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			m_room.desc[i].skillFlag = 0xff;
		}
	}


};