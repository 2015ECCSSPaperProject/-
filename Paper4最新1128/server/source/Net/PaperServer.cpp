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
//	UDP�T�[�o�[
//
//*****************************************************************************************
bool ServerManager::be_playing = false;
ServerManager::RoomData			ServerManager::m_room;
UDPServer*						ServerManager::m_pServer(0);


//=======================================================================
//	������
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

	//�@���얽�߂̃f�[�^��S��������
	ZeroMemory(m_room.desc, sizeof(ContorlDesc)*PLAYER_MAX);
	// �S�����̃��C���[�������@10*6�l
	ZeroMemory(m_room.layer, sizeof(LayerData)* PLAYER_MAX);
	enum { NO_LAYER = -1 };
	for (int p = 0; p < PLAYER_MAX; p++)
	{
		for (int la = 0; la < LAYER_MAX; la++)
		{
			m_room.layer[p].layerdata[la].kind = NO_LAYER;// NO_DATA ��ނ���-1��
		}
	}


}
void ServerManager::Release()
{
	delete m_pServer;
}

void ServerManager::Update()
{
	/*�Q�[���X�V*/
	char data[512];
	int	size = sizeof(data);
	int client = m_pServer->Receive(data, &size);

	if (client == -1)
		return;

	/*�S���������ŏ�������*/
	switch (data[0])//�ŏ���BYTE�@com�����Ĕ��f���Ă�
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
//		�f�[�^����M
//=======================================================================
void ServerManager::NewUser(char* data, int client)
{
	if (client != -1)
	{
		/*�lID���M*/
		m_pServer->Send(client, (char*)&client, sizeof(int));
		Sleep(100);
		printf("�ڑ�:%d\n", client);
	}
}

void ServerManager::InitData(char* data, int client)
{
	CopyMemory(m_room.user[client].name, &data[1], nameRen);
	printf("�Q��:%s\n", m_room.user[client].name, client);
}


//---------------------------------------------------------------------
//   team�@�Z���N�g���
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


	/*�����ɍX�V���Ă��Ă�N���C�A���g��active�ɂ���*/
	m_room.user[client].com = UserData::ACTIVE_USER;
	m_room.user[client].isReady = d->isReady;


	/*�S�����������H*/
	int count(0);
	int active(0);
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_room.user[i].com == UserData::ACTIVE_USER)
			++active;

		// isReady��READY_MUTCH(1)�ɂȂ��Ă���@�J�E���g�����
		if (m_room.user[i].isReady == UserData::READY_MUTCH ||
			m_room.user[i].isReady == UserData::READY_MUTCH_ALL||
			m_room.user[i].isReady == UserData::READY_MUTCH_SYNC)
			++count;
	}


	/*�����A�N�e�B�u�ȃ��[�U�[��isReady�������Ȃ�J�n�t���Oon*/
	if (active == count)
	{
		// ��l�ȏサ���n�߂��Ȃ��I
		//if (active >= 2)
		{
			//�@�Q�[���X�^�[�g�̏���
			m_room.user[client].isReady = UserData::READY_MUTCH_ALL;//�@�S���Q�����Ă�����isReady��READY_MUTCH_ALL(2)�ɁI
			
			//�@�Q�[���X�^�[�g�̏���
			//for (int i = 0; i < PLAYER_MAX; ++i)
			//{
			//	if (m_room.user[i].com == UserData::ACTIVE_USER)5
			//	m_room.user[i].isReady = UserData::READY_MUTCH_ALL;//�@�S���Q�����Ă�����isReady��READY_MUTCH_ALL(2)�ɁI
			//}
		}
	}

	/*�N���C�A���g�ɍ��̃`�[���̏����m�F�����邽�ߑ��M*/
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		UserData data;
		data.com = m_room.user[i].com;
		CopyMemory(data.name, m_room.user[i].name, nameRen); //���O
		data.team = m_room.user[i].team;
		data.isReady = m_room.user[i].isReady;		

		m_pServer->Send(client, (char*)&data, sizeof(UserData));
	}
}


//---------------------------------------------------------------------
//   �Q�[���O�̏���
//---------------------------------------------------------------------
void ServerManager::GameInitData(char* data, int client)
{
	/*����*/
	m_room.user[client].isReady = UserData::READY_MUTCH_SYNC;// SYNC�̓��C���̂����ɍs�����Ƃ��ɕς���

	// �����̃\�[�g��
	/*�S��������������*/
	int count(0);
	int active(0);
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		/*�l��?*/
		if (m_room.user[i].com == UserData::ACTIVE_USER)
			++active;

		/*�ǂݍ��ݏI���H*/
		if (m_room.user[i].isReady == UserData::READY_MUTCH_SYNC)
			++count;
	}
	//�S�����X�e�[�W��v���C���[�Ȃǂ̃I�u�W�F�N�g��ǂݍ��񂾂�Q�[���J�n�ցI

	/*�J�n�t���O*/
	struct
	{
		BYTE com;
	}send;

	//	�Q�[��������
	if (count == active && be_playing == false)
	{
		SAFE_DELETE(timer);
		SAFE_DELETE(score);
		SAFE_DELETE(paper_obj_mng);
		SAFE_DELETE(player_mng);
		SAFE_DELETE(stage);
		SAFE_DELETE(event_list);

		// �C�x���g
		event_list = new Event_list;

		stage = new Stage;
		stage->Initialize();

		//�@�v���C���[������
		player_mng = new PlayerManager;
		player_mng->Initialize();

		score = new Score;
		paper_obj_mng = new Paper_obj_mng;
		paper_obj_mng->Initialize();

		//	���^�C�}�[��������ON�ɂ��܂��I
		if( !timer )timer = new Timer;
		timer->Start( 0, LIMIT_TIME, 0 );

		// ���ׂč쐬���Ă���
		be_playing = true;
	}

	//	��������
	send.com = active == count ? 0xFF : 0;// �݂�ȃf�[�^��ǂݍ��񂾂�255�𑗂�
	m_pServer->Send(client, (char*)&send, sizeof(send));
}


//---------------------------------------------------------------------
//   �Q�[���I�����̏���
//---------------------------------------------------------------------
void ServerManager::GameEndGame(char* data, int client)
{
	/*����*/

	/*�ʂ�����t���O*/
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		m_room.user[i].isReady = 0;
	}
	//	�Q�[���I��̏���
	be_playing = false;

	/*����������ς����Ȃ��@�󂯎�邾���@*/

	//	��������
	//send.com = 0xFF;// �݂�ȃf�[�^��ǂݍ��񂾂�255�𑗂�
	//m_pServer->Send(client, (char*)&send, sizeof(send));
}

//---------------------------------------------------------------------
//   �Q�[���J�n���̏����H�H�H
//---------------------------------------------------------------------

///void ServerManager::InitUser(char* data, int client)
//{
//	/*���M*/
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
//   ���v���C���[�̍X�V
//---------------------------------------------------------------------
void ServerManager::UpdateUser(char* data, int client)
{
	ContorlDesc* receive = (ContorlDesc*)data;

	//	��M
	m_room.desc[client] = *receive;

	//�@����ɂ������Ӗ������肻�������Ӗ��͕�����Ȃ��@���������Ɏg��
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



	//	�S�����̏��𑗐M
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
//   ���M�~�b�N���̍X�V
//---------------------------------------------------------------------

void ServerManager::UpdateStage(int client)
{
	// �e���b�v
	int telop_id = score->Get_telopID();
	unsigned int telop_size = sizeof(int);

	// ��
	/*
	unsigned int paper_obj_size = paper_obj_mng->Get_send_data_size();
	char *paper_obj_data = new char[paper_obj_size]
	{};
	paper_obj_mng->Get_send_data( paper_obj_data );
	*/
	unsigned int paper_obj_size = sizeof( PaperData );
	PaperData paper_obj_data = player_mng->Get_player( client )->paperqueue->Update();

	// �܂Ƃ߂đ��M
	unsigned int size = paper_obj_size + telop_size;
	char *send_data(nullptr);
	send_data = new  char[size];
	memcpy_s(send_data, telop_size, &telop_id, telop_size);
	memcpy_s( send_data + telop_size, paper_obj_size, &paper_obj_data, paper_obj_size );

	m_pServer->Send(client, send_data, size);

	delete[] send_data;
}


//---------------------------------------------------------------------
//   �����C���[�̍X�V
//---------------------------------------------------------------------
void ServerManager::UpdateLayer(char* data, int client)
{
	struct RECEIVE_LAYER
	{
		BYTE com;
		LayerData layers;
	};
	RECEIVE_LAYER* receive = (RECEIVE_LAYER*)data;

	//	�����Ă������C���[����M
	m_room.layer[client] = receive->layers;


	/*�N���C�A���g�ɂ݂�Ȃ̃��C���[���𑗐M*/
	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		LayerData data;
		data = m_room.layer[i];

		m_pServer->Send(client, (char*)&data, sizeof(LayerData));// �T�C�Y�I�t�̌`�ɂ͋C��t���悤(��������)
	}
}

//---------------------------------------------------------------------
//   ���X�R�A�̍X�V
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
	printf("�E��:%d\n", client);

	m_room.user[client].com = 0;
	m_room.user[client].isReady = 0;
	strcpy_s(m_room.user[client].name, "\0");

	//	�\�P�b�g�����
	m_pServer->CloseClient(client);

}
