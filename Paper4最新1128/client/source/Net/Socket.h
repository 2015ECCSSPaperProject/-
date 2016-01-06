#pragma once
//*****************************************************************************************************************************
//
//		
//
//*****************************************************************************************************************************
#include	<string>
#include	"../../../share_data/Enum_public.h"

class UDPClient;

static const int nameRen= 18;

//===================================================================================
//   
//===================================================================================
//---------------------------------------------------------------------
//  �v���C���[�̃��O�C�����
//---------------------------------------------------------------------
struct UserData
{
	BYTE	com;		//	�f�[�^�`��	
	char	name[nameRen];	//	���O
	BYTE	team;
	BYTE	isReady;	//

	enum READY_STATE
	{
		PASSIVE_USER = 0,
		ACTIVE_USER = 1,
		READY_MUTCH = 1,
		READY_MUTCH_ALL = 2,
		READY_MUTCH_SYNC = 3,
		END_SCENEMAIN = 4
	};
};
//---------------------------------------------------------------------
//  �v���C���[�̕Ԃ��Ă���p�����[�^
//---------------------------------------------------------------------
struct PlayerData
{
	Vector3 pos;
	float angleY;
	BYTE motion_no;
	BYTE action_part;
	BYTE god_gage;
	WORD timer;
	int target_no;
};
//---------------------------------------------------------------------
//  �|�X�^�[�̕Ԃ��Ă���p�����[�^
//---------------------------------------------------------------------
struct PosterData
{
	BYTE user_number;
	int anim_no;
};
struct AreaDate
{
	BYTE b;
};
//---------------------------------------------------------------------
//  �X�R�A�̕Ԃ��Ă���p�����[�^
//---------------------------------------------------------------------
struct ScoreData
{
	UINT score;
};

//---------------------------------------------------------------------
//  ���[�U�[�̃��C���[�̃p�����[�^
//---------------------------------------------------------------------
struct LayerData
{

	struct LAYER_DATA
	{
		/*��ނƃi���o�[*/
		int kind;
		int num;
		float x, y;//	�ꏊ
		int size;//		�T�C�Y
		bool isHold;//	�͂�ł邩
	};
	LAYER_DATA layerdata[LAYER_MAX];

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
	LayerData				m_layer[PLAYER_MAX];

	static SocketManager*	m_pInst;
	SocketManager();

	/* �R�}���h�̎�� */
	enum EVENT_COM
	{
		NEW_USER = 0,
		INIT_DATA = 1,
		EXIT_USER = 2,
		TEAM_DATA = 3,
		INIT_GAME_DATA = 4,
		/*INIT_GAME,*/
		USER_DATA = 5,
		STAGE_DATA = 6,
		LAYER_DATA = 7,
		SCORE_DATA = 8,
		END_GAME = 9
	};

private:

public:
	~SocketManager();
	bool Init();


	//---------------------------------------------------------------------
	//   network
	//---------------------------------------------------------------------

	/*	�X�V	*/
	void UpdateTeam(int isReady);	/* �I�� */
	void UpdateLayer();				/* �I�� ���C���[�X�V�@*/
	void InitGame();				/* �����O */
	void EndGame();					/* �G���h�Q�[�� */

	void UpdateUser();				/* �v���C���[�X�V	*/
	void UpdateStage();				/* �X�e�[�W�X�V		*/
	void UpdateScore();				/* �X�R�A�X�V		*/

	/*	�Q�b�^�[�E�Z�b�^�[	*/
	int					GetID()				const{ return m_myID; }
	const char*			GetName()				const{ return m_myName.c_str(); }
	const UserData&		GetUser(s32 client)	const{ return m_user[client]; }
	const PlayerData    GetPlayer(int idx)		const{ return m_player[idx]; }
	const LayerData    GetLayer(int idx)		const{ return m_layer[idx]; }
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