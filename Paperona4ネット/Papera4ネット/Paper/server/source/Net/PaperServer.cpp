#include	<winsock2.h>
#include	<time.h>

#include	"iextreme.h"
#include	"../system/Framework.h"
#include	"../Player/BasePlayer.h"

#include	"PaperServer.h"
#include	"UDPServer.h"
#include	"../poster/Poster_manager.h"


//#define BYTE_TRUE 0xAA
//#define BYTE_FALSE 0xF0

//*****************************************************************************************
//
//	UDPサーバー
//
//*****************************************************************************************
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
	case POSTER_DATA:
		UpdatePoster(client);
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
	CopyMemory(m_room.user[client].name, &data[1], 9);
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
		CopyMemory(data.name, m_room.user[i].name, 9);
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
	if (count == active)
	{
		int hoge = 0;
		hoge++;
		//　オブジェをnew
		//GAME_MASTER->GameInitialize();
	}

	//	同期完了
	send.com = active == count ? 0xFF : 0;// みんなデータを読み込んだら255を送る
	m_pServer->Send(client, (char*)&send, sizeof(send));
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
		int motion_no;
		int action_part;
	}send[PLAYER_MAX];


	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		player[i]->Get_pos(send[i].pos);
		send[i].angleY = player[i]->Get_angleY();
		send[i].motion_no = player[i]->Get_motion_no();
		send[i].action_part = (int)player[i]->Get_action();
	}

	m_pServer->Send(client, (char*)&send, sizeof(send));
}

//---------------------------------------------------------------------
//   ★ポスターの更新
//---------------------------------------------------------------------
void ServerManager::UpdatePoster(int client)
{
	struct PosterData
	{
		BYTE color;
		int anim_no;
	}*send_poster(nullptr);

	int num_poster = poster_mng->Get_numof();
	send_poster = new PosterData[num_poster];

	for (int i = 0; i < num_poster; i++)
	{
		send_poster[i].color = (BYTE)poster_mng->Get_color(i);
		send_poster[i].anim_no = poster_mng->Get_animation_frame(i);
	}

	m_pServer->Send(client, (char*)send_poster, sizeof(*send_poster)*num_poster);

	delete[] send_poster;
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
