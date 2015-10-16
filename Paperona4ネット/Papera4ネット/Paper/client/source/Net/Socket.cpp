
#include	<mbstring.h>

#include	"iextreme.h"
#include	"../system/system.h"
#include	"../Scene/sceneMain.h"

#include	"Socket.h"
#include	"UDPclient.h"

#include	"../Player/BasePlayer.h"

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


	BYTE com(NEW_USER);
	m_pClient->Send((char*)&com, sizeof(BYTE));


	/*ID取得*/
	if (m_pClient->Receive(&m_myID, sizeof(int)) <= 0)
	{
		closesocket(m_pClient->m_sock);
		m_pClient->m_sock = INVALID_SOCKET;
		return false;
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
	std::fclose;

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
	desc = player[m_myID]->GetDesc();

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

void SocketManager::UpdatePoster()
{
	BYTE comcom = POSTER_DATA;
	m_pClient->Send(&comcom, sizeof(comcom)); //自分の操作した情報を渡す

	int num_poster = poster_mng->Get_numof();
	PosterData *poster_data = new PosterData[num_poster];

	m_pClient->Receive((char*)poster_data, sizeof(*poster_data)*num_poster);

	for (int i = 0; i < num_poster; i++)
	{
		poster_mng->Set_color(i, (TEAM_COLOR)poster_data[i].color);
		poster_mng->Set_animframe(i, poster_data[i].anim_no);
	}

	delete[] poster_data;

	bench.End();
}
