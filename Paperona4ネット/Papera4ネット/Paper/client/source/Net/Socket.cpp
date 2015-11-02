
#include	<mbstring.h>

#include	"iextreme.h"
#include	"../system/system.h"
#include	"../Scene/sceneMain.h"

#include	"Socket.h"
#include	"UDPclient.h"

#include	"../Player/BasePlayer.h"
#include	"../Player/PlayerManager.h"

#include	"../bench/Bench_mark.h"
#include "../poster/Poster_manager.h"
Bench_mark bench;

//*****************************************************************************************
//
//	ソケット管理
//
//*****************************************************************************************
SocketManager* SocketManager::m_pInst(0);


//=======================================================================
//	初期化
//=======================================================================
SocketManager::SocketManager() :m_myID(-1)
{
	WSADATA	wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_pClient = new UDPClient;
}
bool SocketManager::Init()
{
	m_myName = "\0";

	m_team = 0;

	ZeroMemory(m_user, sizeof(UserData)* PLAYER_MAX);
	ZeroMemory(m_player, sizeof(PlayerData)* PLAYER_MAX);
	ZeroMemory(m_layer, sizeof(LayerData)* PLAYER_MAX);// 全員分のレイヤー初期化　10*6人
	enum { NO_LAYER = -1 };
	for (int p = 0; p < PLAYER_MAX; p++)
	{
		for (int la = 0; la < LAYER_MAX; la++)
		{
			m_layer[p].layerdata[la].kind = NO_LAYER;// NO_DATA 種類だけ-1に
		}
	}


	BYTE com(NEW_USER);
	m_pClient->Send((char*)&com, sizeof(BYTE));


	/*ID取得*/
	if (m_pClient->Receive(&m_myID, sizeof(int)) <= 0)
	{
		closesocket(m_pClient->m_sock);
		m_pClient->m_sock = INVALID_SOCKET;
		return false;
	}

	// ※追加

	/* セーブしたレイヤーの読み込み　*/
	std::ifstream in("DATA/makePoster/text/Layer.txt");
	// バッファに読み込む
	if (in)//そのファイルがあるなら！
	{
		for (int i = 0; i < LAYER_MAX; i++)
		{
			in >> m_layer[m_myID].layerdata[i].kind;
			in >> m_layer[m_myID].layerdata[i].num;
			in >> m_layer[m_myID].layerdata[i].x;
			in >> m_layer[m_myID].layerdata[i].y;
			in >> m_layer[m_myID].layerdata[i].size;
			in >> m_layer[m_myID].layerdata[i].isHold;
		}

		in.close();
	}
	else
	{
		assert("ポスターのテキストがない!");
	}

	return true;
}
SocketManager::~SocketManager()
{
	BYTE com(EXIT_USER);
	m_pClient->Send(&com, sizeof(BYTE));

	delete m_pClient;
}

void SocketManager::SendName()
{
	// 仮で名前を付ける
	std::ifstream in("名前.txt");
	// バッファにyomi込む
	in >> m_myName;
	//std::fclose;

	struct
	{
		BYTE com;
		char name[9];
	} info;

	info.com = INIT_DATA;
	strcpy_s(info.name, m_myName.c_str());
	m_pClient->Send((char*)&info, sizeof(info));
}




//=======================================================================
//	更新
//=======================================================================

//---------------------------------------------------------------------
//   選択中
//---------------------------------------------------------------------
void SocketManager::UpdateTeam(int isReady)
{
	if (m_pClient->m_sock == INVALID_SOCKET)
		return;

	struct DATA
	{
		BYTE com;
		BYTE team;
		BYTE isReady;
	}data;


	/*	今の俺の状態を毎回送るよ	*/
	data.com = TEAM_DATA;
	data.team = m_team;
	data.isReady = isReady;
	m_pClient->Send(&data, sizeof(data));


	/*	毎回プレイヤーの状態を更新	*/
	UserData receive;

	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_pClient->Receive((char*)&receive, sizeof(UserData)) <= 0)
			break;

		m_user[i] = receive;
	}
}


//   選択中　レイヤー更新
void SocketManager::UpdateLayer()
{
	if (m_pClient->m_sock == INVALID_SOCKET)
		return;

	struct DATA
	{
		BYTE com;
		LayerData layers;

	}data;

	/*	自分のレイヤーを毎回送るよ	*/
	data.com = LAYER_DATA;
	data.layers = m_layer[m_myID];

	m_pClient->Send(&data, sizeof(data));

	/*	毎回皆のポスターを更新	*/
	LayerData receive;

	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_pClient->Receive((char*)&receive, sizeof(LayerData)) <= 0)
			break;

		m_layer[i] = receive;
	}


};

//===================================================================================
//  試合前
//===================================================================================
void SocketManager::InitGame()
{
	BYTE com = INIT_GAME_DATA;
	BYTE receive[2];


	// 皆がシーンメインに行ってこの関数までくるまで無限ループ
	while (1)
	{
		m_pClient->Send(&com, sizeof(com));

		if (m_pClient->Receive((char*)&receive, sizeof(receive)) <= 0)
			continue;

		//	同期完了
		if (receive[0] == 0xFF)
			break;
	}

	//	//VVVV 

	// サーバからの受信データを空にする
	//int cnt = 0;
	//while (1)
	//{
	//	BYTE tmp[2] = { 0x00, 0x00 };
	//	int read = m_pClient->Receive((char*)&tmp, sizeof(tmp));
	//	if (read > 0)
	//		++cnt;
	//	else
	//		break;
	//}

}
//===================================================================================
//  試合
//===================================================================================

//---------------------------------------------------------------------
//  プレイヤーの更新
//---------------------------------------------------------------------
void SocketManager::UpdateUser()	/* プレイヤー更新 */
{
	if (m_pClient->m_sock == INVALID_SOCKET)
		return;

	bench.Start();

	/*送信用DESC*/
	struct
	{
		BYTE    com;		//Comを追加しただけ
		BYTE	moveFlag;			//	上下左右操作フラグ
		float mouseX;	//マウスX
		float mouseY;	//マウスY
		BYTE motion_no;	// モーション番号
		BYTE	rendFlag;	// 破くときに送る
		BYTE	controlFlag;
	}send;

	//　※ベースプレイヤーのコントロールデスクを持ってくる
	BasePlayer::CONTROL_DESC desc;
	//desc = player[m_myID]->GetDesc();
	desc = player_mng->Get_player(m_myID)->GetDesc();

	send.com = USER_DATA;
	send.moveFlag = desc.moveFlag;
	send.mouseX = desc.mouseX;
	send.mouseY = desc.mouseY;
	send.motion_no = desc.motion_no;
	send.rendFlag = desc.rendFlag;
	send.controlFlag = desc.controlFlag;

	m_pClient->Send(&send, sizeof(send)); //自分の操作した情報を渡す


	/*受信*/
	struct
	{
		Vector3	pos;
		float angleY;
		BYTE motion_no;
		BYTE action_part;
	}receive[PLAYER_MAX];

	m_pClient->Receive((char*)&receive, sizeof(receive));
	


	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		// スロットパンツァーではここで送られてきた時に生死判定をしている
		//if (receive[i].damage >= tank[i]->GetHP())
		//	tank[i]->Death();
		//else
		//	tank[i]->Born();

		m_player[i].pos = receive[i].pos;
		m_player[i].angleY = receive[i].angleY;
		m_player[i].motion_no = receive[i].motion_no;
		m_player[i].action_part = receive[i].action_part;
	}



	//　ゲームの情報(UI)のデータも貰う
	{
		//struct
		//{
		//	BYTE					gameState;
		//	BYTE					tankState;
		//	float					flagRate;
		//}gameData;
		//BYTE com = MUTCH_GAME_DATA;
		//m_pClient->Send(&com, sizeof(com));
		//if (m_pClient->Receive((char*)&gameData, sizeof(gameData)) > 0)
		//{
		//	//UIの情報を入れる

		//}
	}
};

void SocketManager::UpdateKohai()	/* プレイヤー更新 */
{
	if (m_pClient->m_sock == INVALID_SOCKET)
		return;

	//　※ベースプレイヤーのコントロールデスクを持ってくる
	BasePlayer::CONTROL_DESC desc;
	//desc = player[m_myID]->GetDesc();
	desc = player_mng->Get_player(m_myID)->GetDesc();

	BYTE com = KOHAI_DATA;

	m_pClient->Send(&com, sizeof(com)); //後輩のデータくださいフラグ


	/*受信*/
	struct
	{
		Vector3	pos;
		float angleY;
		BYTE motion_no;
		BYTE action_part;
	}receive[PLAYER_MAX];

	m_pClient->Receive((char*)&receive, sizeof(receive));



	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		m_kohai[i].pos = receive[i].pos;
		m_kohai[i].angleY = receive[i].angleY;
		m_kohai[i].motion_no = receive[i].motion_no;
		m_kohai[i].action_part = receive[i].action_part;
	}
};

void SocketManager::UpdatePoster()
{
	BYTE comcom = POSTER_DATA;
	m_pClient->Send(&comcom, sizeof(comcom)); //自分の操作した情報を渡す

	int num_poster = poster_mng->Get_numof();
	PosterData *poster_data = new PosterData[num_poster];

	m_pClient->Receive((char*)poster_data, sizeof(*poster_data)*num_poster);

	for (int i = 0; i < num_poster; i++)
	{
		poster_mng->Set_user(i, (int)poster_data[i].user_number);
		poster_mng->Set_animframe(i, poster_data[i].anim_no);
	}

	delete[] poster_data;

	bench.End();
}
