
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
//	�\�P�b�g�Ǘ�
//
//*****************************************************************************************
SocketManager* SocketManager::m_pInst(0);


//=======================================================================
//	������
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


	/*ID�擾*/
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
	// ���Ŗ��O��t����
	std::ifstream in("���O.txt");
	// �o�b�t�@��yomi����
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
//	�X�V
//=======================================================================

//---------------------------------------------------------------------
//   �I��
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


	/*	���̉��̏�Ԃ𖈉񑗂��	*/
	data.com = TEAM_DATA;
	data.team = m_team;
	data.isReady = isReady;
	m_pClient->Send(&data, sizeof(data));


	/*	����v���C���[�̏�Ԃ��X�V	*/
	UserData receive;

	for (int i = 0; i<PLAYER_MAX; ++i)
	{
		if (m_pClient->Receive((char*)&receive, sizeof(UserData)) <= 0)
			break;

		m_user[i] = receive;
	}
}


//===================================================================================
//  �����O
//===================================================================================
void SocketManager::InitGame()
{
	BYTE com = INIT_GAME_DATA;
	BYTE receive[2];


	// �F���V�[�����C���ɍs���Ă��̊֐��܂ł���܂Ŗ������[�v
	while (1)
	{
		m_pClient->Send(&com, sizeof(com));

		if (m_pClient->Receive((char*)&receive, sizeof(receive)) <= 0)
			continue;

		//	��������
		if (receive[0] == 0xFF)
			break;
	}

	//	//VVVV 

	// �T�[�o����̎�M�f�[�^����ɂ���
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
//  ����
//===================================================================================

//---------------------------------------------------------------------
//  �v���C���[�̍X�V
//---------------------------------------------------------------------
void SocketManager::UpdateUser()	/* �v���C���[�X�V */
{
	if (m_pClient->m_sock == INVALID_SOCKET)
		return;

	bench.Start();

	/*���M�pDESC*/
	struct
	{
		BYTE    com;		//Com��ǉ���������
		BYTE	moveFlag;			//	�㉺���E����t���O
		float mouseX;	//�}�E�XX
		float mouseY;	//�}�E�XY
		BYTE motion_no;	// ���[�V�����ԍ�
		BYTE	rendFlag;	// �j���Ƃ��ɑ���
		BYTE	controlFlag;
	}send;

	//�@���x�[�X�v���C���[�̃R���g���[���f�X�N�������Ă���
	BasePlayer::CONTROL_DESC desc;
	desc = player[m_myID]->GetDesc();

	send.com = USER_DATA;
	send.moveFlag = desc.moveFlag;
	send.mouseX = desc.mouseX;
	send.mouseY = desc.mouseY;
	send.motion_no = desc.motion_no;
	send.rendFlag = desc.rendFlag;
	send.controlFlag = desc.controlFlag;

	m_pClient->Send(&send, sizeof(send)); //�����̑��삵������n��


	/*��M*/
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
		// �X���b�g�p���c�@�[�ł͂����ő����Ă������ɐ�����������Ă���
		//if (receive[i].damage >= tank[i]->GetHP())
		//	tank[i]->Death();
		//else
		//	tank[i]->Born();

		m_player[i].pos = receive[i].pos;
		m_player[i].angleY = receive[i].angleY;
		m_player[i].motion_no = receive[i].motion_no;
		m_player[i].action_part = receive[i].action_part;
	}



	//�@�Q�[���̏��(UI)�̃f�[�^���Ⴄ
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
		//	//UI�̏�������

		//}
	}
};

void SocketManager::UpdatePoster()
{
	BYTE comcom = POSTER_DATA;
	m_pClient->Send(&comcom, sizeof(comcom)); //�����̑��삵������n��

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
