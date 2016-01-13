#include	<winsock2.h>
#include	<time.h>

#include	"iextreme.h"
#include	"../system/Framework.h"
#include	"../Player/BasePlayer.h"

#include	"PaperServer.h"
#include	"UDPServer.h"
#include	"../paper object/paper object manager.h"
#include	"../Player/PlayerManager.h"

#include	"../score/Score.h"
#include	"../timer/Timer.h"
#include	"../stage/Stage.h"
#include "../event/Event list.h"
#include "../paperQueue/paperQueue.h"
//#define BYTE_TRUE 0xAA
//#define BYTE_FALSE 0xF0

//*****************************************************************************************
//
//	UDPサーバー
//
//*****************************************************************************************
bool ServerManager::be_playing = false;
ServerManager::RoomData			ServerManager::m_room;
UDPServer*						ServerManager::m_pServer(0);


//=======================================================================
//	初期化
//=======================================================================
void ServerManager::Init()
{
	WSADATA	wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_pServer = new UDPServer();


	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		m_room.user[i].com = 0;
		m_room.user[i].isReady = 0;
		strcpy_s(m_room.user[i].name, "\0");
	}

	//　操作命令のデータを全部初期化
	ZeroMemory(m_room.desc, sizeof(ContorlDesc)*PLAYER_MAX);
	// 全員分のレイヤー初期化　10*6人
	ZeroMemory(m_room.layer, sizeof(LayerData)* PLAYER_MAX);
	enum { NO_LAYER = -1 };
	for (int p = 0; p < PLAYER_MAX; p++)
	{
		for (int la = 0; la < LAYER_MAX; la++)
		{
			m_room.layer[p].layerdata[la].kind = NO_LAYER;// NO_DATA 種類だけ-1に
		}
	}


}
void ServerManager::Release()
{
	delete m_pServer;
}

void ServerManager::Update()
{
	/*ゲーム更新*/
	char data[512];
	int	size = sizeof(data);
	int client = m_pServer->Receive(data, &size);

	if (client == -1)
		return;

	/*全情報をここで処理する*/
	switch (data[0])//最初にBYTE　comを見て判断してる
	{
	case NEW_USER:
		NewUser(data, client);
		break;
	case INIT_DATA:
		InitData(data, client);
		break;
	case EXIT_USER:
		ExitUser(data, client);
		break;
	case TEAM_DATA:
		TeamData(data, client);
		break;
	case INIT_GAME_DATA:
		GameInitData(data, client);
		break;
	//case USER_INIT:
	//	InitUser(data, client);
	//	break;
	case USER_DATA:
		UpdateUser(data, client);
		break;
	case STAGE_DATA:
		UpdateStage(client);
		break;
	case LAYER_DATA:
		UpdateLayer(data, client);
		break;
	case SCORE_DATA:
		UpdateScore(client);
		break;
	case END_GAME:
		GameEndGame(data, client);
		break;
	}
}

void ServerManager::Render()
{

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
//		Text::Draw(0, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld",i, m_pServer->Get_ClientAddr(i));	
		Text::Draw(0, 300 + (i * 32), 0xffffffff, "name[%d]->%s", i, m_room.user[i].name);	
	}

#ifdef _DEBUG

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
	//	Text::Draw(0, 40 + (i * 32), 0xffffffff, "Clientaddr[%d]->%ld",i, m_pServer->Get_ClientAddr(i));	
		Text::Draw(0, 300 + (i * 32), 0xffffffff, "name[%d]->%s", i, m_room.user[i].name);
		
	}

#endif


}


//=======================================================================
//		データ送受信
//=======================================================================
void ServerManager::NewUser(char* data, int client)
{
	if (client != -1)
	{
		/*個人ID送信*/
		m_pServer->Send(client, (char*)&client, sizeof(int));
		Sleep(100);
		printf("接続:%d\n", client);
	}
}

void ServerManager::InitData(char* data, int client)
{
	CopyMemory(m_room.user[client].name, &data[1], nameRen);
	printf("参加:%s\n", m_room.user[client].name, client);
}


//---------------------------------------------------------------------
//   team　セレクト画面
//---------------------------------------------------------------------
void ServerManager::TeamData(char* data, int client)
{
	struct DATA
	{
		BYTE com;
		BYTE team;
		BYTE isReady;
	};
	DATA* d((DATA*)data);


	/*ここに更新してきてるクライアントをactiveにする*/
	m_room.user[client].com = UserData::ACTIVE_USER;
	m_room.user[client].isReady = d->isReady;


	/*全員準備完了？*/
	int count(0);
	int active(0);
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_room.user[i].com == UserData::ACTIVE_USER)
			++active;

		// isReadyがREADY_MUTCH(1)になってたら　カウントするよ
		if (m_room.user[i].isReady == UserData::READY_MUTCH ||
			m_room.user[i].isReady == UserData::READY_MUTCH_ALL||
			m_room.user[i].isReady == UserData::READY_MUTCH_SYNC)
			++count;
	}


	/*もしアクティブなユーザーとisReadyが同じなら開始フラグon*/
	if (active == count)
	{
		// 二人以上しか始められない！
		//if (active >= 2)
		{
			//　ゲームスタートの処理
			m_room.user[client].isReady = UserData::READY_MUTCH_ALL;//　全員参加していたらisReadyをREADY_MUTCH_ALL(2)に！
			
			//　ゲームスタートの処理
			//for (int i = 0; i < PLAYER_MAX; ++i)
			//{
			//	if (m_room.user[i].com == UserData::ACTIVE_USER)5
			//	m_room.user[i].isReady = UserData::READY_MUTCH_ALL;//　全員参加していたらisReadyをREADY_MUTCH_ALL(2)に！
			//}
		}
	}

	/*クライアントに今のチームの情報を確認させるため送信*/
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		UserData data;
		data.com = m_room.user[i].com;
		CopyMemory(data.name, m_room.user[i].name, nameRen); //名前
		data.team = m_room.user[i].team;
		data.isReady = m_room.user[i].isReady;		

		m_pServer->Send(client, (char*)&data, sizeof(UserData));
	}
}


//---------------------------------------------------------------------
//   ゲーム前の処理
//---------------------------------------------------------------------
void ServerManager::GameInitData(char* data, int client)
{
	/*同期*/
	m_room.user[client].isReady = UserData::READY_MUTCH_SYNC;// SYNCはメインのここに行ったときに変える

	// いつものソートで
	/*全員が同期完了か*/
	int count(0);
	int active(0);
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		/*人間?*/
		if (m_room.user[i].com == UserData::ACTIVE_USER)
			++active;

		/*読み込み終了？*/
		if (m_room.user[i].isReady == UserData::READY_MUTCH_SYNC)
			++count;
	}
	//全員がステージやプレイヤーなどのオブジェクトを読み込んだらゲーム開始へ！

	/*開始フラグ*/
	struct
	{
		BYTE com;
	}send;

	//	ゲーム初期化
	if (count == active && be_playing == false)
	{
		SAFE_DELETE(timer);
		SAFE_DELETE(score);
		SAFE_DELETE(paper_obj_mng);
		SAFE_DELETE(player_mng);
		SAFE_DELETE(stage);
		SAFE_DELETE(event_list);

		// イベント
		event_list = new Event_list;

		stage = new Stage;
		stage->Initialize();

		//　プレイヤー初期化
		player_mng = new PlayerManager;
		player_mng->Initialize();

		score = new Score;
		paper_obj_mng = new Paper_obj_mng;
		paper_obj_mng->Initialize();

		//	※タイマーをここでONにします！
		if( !timer )timer = new Timer;
		timer->Start( 0, LIMIT_TIME, 0 );

		// すべて作成してから
		be_playing = true;
	}

	//	同期完了
	send.com = active == count ? 0xFF : 0;// みんなデータを読み込んだら255を送る
	m_pServer->Send(client, (char*)&send, sizeof(send));
}


//---------------------------------------------------------------------
//   ゲーム終了時の処理
//---------------------------------------------------------------------
void ServerManager::GameEndGame(char* data, int client)
{
	/*同期*/

	/*通ったよフラグ*/
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		m_room.user[i].isReady = 0;
	}
	//	ゲーム終りの処理
	be_playing = false;

	/*★何も送り変えさない　受け取るだけ　*/

	//	同期完了
	//send.com = 0xFF;// みんなデータを読み込んだら255を送る
	//m_pServer->Send(client, (char*)&send, sizeof(send));
}

//---------------------------------------------------------------------
//   ゲーム開始時の処理？？？
//---------------------------------------------------------------------

///void ServerManager::InitUser(char* data, int client)
//{
//	/*送信*/
//	//struct
//	//{
//	//	BYTE	bodyParts;
//	//	BYTE	headParts;
//	//	BYTE	canonParts;
//	//	BYTE	goldAndInvincible;
//	//	float	centerGrv;
//	//}send[PANZER_MAX];
//
//	//for (int i = 0; i<PANZER_MAX; ++i)
//	//{
//	//	int tmp[3];
//	//	tank[i]->GetPartsNum(tmp);
//	//	send[i].canonParts = tmp[0];
//	//	send[i].headParts = tmp[1];
//	//	send[i].bodyParts = tmp[2];
//
//	//	BYTE	cannonIsGold = (tank[i]->CannonIsGold()) ? 1 : 0;
//	//	BYTE	bodyIsGold = (tank[i]->BodyIsGold()) ? 2 : 0;
//	//	BYTE	legIsGold = (tank[i]->LegIsGold()) ? 4 : 0;
//	//	BYTE	isInvincible = (GAME_MASTER->IsInvincibleTank(i)) ? 8 : 0;
//
//	//	send[i].goldAndInvincible = cannonIsGold | bodyIsGold | legIsGold | isInvincible;
//	//	send[i].centerGrv = tank[i]->GetCenterGravity();
//	//}
//
//	//m_pServer->Send(client, (char*)&send, sizeof(send));
//}

//---------------------------------------------------------------------
//   ★プレイヤーの更新
//---------------------------------------------------------------------
void ServerManager::UpdateUser(char* data, int client)
{
	ContorlDesc* receive = (ContorlDesc*)data;

	//	受信
	m_room.desc[client] = *receive;

	//　これにすごく意味がありそうだが意味は分からない　多分調整に使う
	//m_room.receiveFlg = 1 << client;		//	

	//GAME_MASTER->ConnectClient(client);
	//switch (GAME_MASTER->GetPlayerState(client))
	//{
	//case DEATH:
	//	GAME_MASTER->PlaySlot(client, receive->selectSlot, receive->reqest);
	//	break;
	//default:
	//	break;
	//}



	//	全員分の情報を送信
	struct
	{
		Vector3 pos;
		float angleY;
		BYTE motion_no;
		BYTE action_part;
		BYTE god_gage;
		WORD timer;
		int target_no;
		BYTE stateFlag;
	}send[PLAYER_MAX];


	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		player_mng->Get_player(i)->Get_pos(send[i].pos);
		send[i].angleY = player_mng->Get_player(i)->Get_angleY();
		send[i].motion_no = player_mng->Get_player(i)->Get_motion_no();
		send[i].action_part = (BYTE)player_mng->Get_player(i)->Get_action();
		send[i].god_gage = player_mng->Get_player(i)->Get_god_gage();
		send[i].target_no = player_mng->Get_player(i)->Get_target_no();
		send[i].stateFlag = player_mng->Get_player(i)->Get_state_flag();
		int rcv, dammy;
		(timer) ? timer->Get_second_limit(&rcv, &dammy) : rcv = -999;
		send[i].timer = rcv;
	}

	m_pServer->Send(client, (char*)&send, sizeof(send));
}

//---------------------------------------------------------------------
//   ★ギミック等の更新
//---------------------------------------------------------------------

void ServerManager::UpdateStage(int client)
{
	// テロップ
	int telop_id = score->Get_telopID();
	unsigned int telop_size = sizeof(int);

	// 紙
	/*
	unsigned int paper_obj_size = paper_obj_mng->Get_send_data_size();
	char *paper_obj_data = new char[paper_obj_size]
	{};
	paper_obj_mng->Get_send_data( paper_obj_data );
	*/
	unsigned int paper_obj_size = sizeof( PaperData );
	PaperData paper_obj_data = player_mng->Get_player( client )->paperqueue->Update();

	// まとめて送信
	unsigned int size = paper_obj_size + telop_size;
	char *send_data(nullptr);
	send_data = new  char[size];
	memcpy_s(send_data, telop_size, &telop_id, telop_size);
	memcpy_s( send_data + telop_size, paper_obj_size, &paper_obj_data, paper_obj_size );

	m_pServer->Send(client, send_data, size);

	delete[] send_data;
}


//---------------------------------------------------------------------
//   ★レイヤーの更新
//---------------------------------------------------------------------
void ServerManager::UpdateLayer(char* data, int client)
{
	struct RECEIVE_LAYER
	{
		BYTE com;
		LayerData layers;
	};
	RECEIVE_LAYER* receive = (RECEIVE_LAYER*)data;

	//	送られてきたレイヤーを受信
	m_room.layer[client] = receive->layers;


	/*クライアントにみんなのレイヤー情報を送信*/
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		LayerData data;
		data = m_room.layer[i];

		m_pServer->Send(client, (char*)&data, sizeof(LayerData));// サイズオフの形には気を付けよう(悔い改め)
	}
}

//---------------------------------------------------------------------
//   ★スコアの更新
//---------------------------------------------------------------------
void ServerManager::UpdateScore(int client)
{
	struct ScoreData
	{
		UINT score;
	}*send_score(nullptr);

	send_score = new ScoreData[PLAYER_MAX];

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		send_score[i].score = score->Get_score(i);
	}

	m_pServer->Send(client, (char*)send_score, sizeof(*send_score)*PLAYER_MAX);

	delete[] send_score;
}

//---------------------------------------------------------------------
//   exit
//---------------------------------------------------------------------
void ServerManager::ExitUser(char* data, int client)
{
	printf("脱退:%d\n", client);

	m_room.user[client].com = 0;
	m_room.user[client].isReady = 0;
	strcpy_s(m_room.user[client].name, "\0");

	//	ソケットを閉じる
	m_pServer->CloseClient(client);

}
