#pragma once
#include<list>
#include	"../../../share_data/Enum_public.h"
//*****************************************************************************************************************************
//
//		server
//
//*****************************************************************************************************************************
//=======================================================================
//	�O���錾
//=======================================================================

class UDPServer;

//---------------------------------------------------------------------
//  �v���C���[�̃��O�C�����
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
//  �v���C���[�̑��얽�߂̃f�[�^
//---------------------------------------------------------------------
struct ContorlDesc
{
	BYTE	com; //�@�������ɍ��킷���߂ɈӖ��Ȃ����ǃR�}���h���
	BYTE	moveFlag; //�@�㉺���E�ɓ������t���O

	float	mouseX;			//�@�}�E�XX
	float	mouseY;			//�@�}�E�XY

	BYTE	motion_no;		// ���[�V�����̔ԍ�

	BYTE	rendFlag;		// �j���Ƃ��ɑ���t���O
	BYTE	controlFlag;	// �ڂ���
	BYTE	skillFlag;		// �K�E�Z
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
//	�T�[�o�[�N���X
//=======================================================================
class ServerManager
{
public:
	/* �R�}���h�̎�� */
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

	/* ���[�U�[�̃f�[�^ */
	struct RoomData
	{
		UserData		user[PLAYER_MAX];
		ContorlDesc		desc[PLAYER_MAX];
		LayerData		layer[PLAYER_MAX];// ���C���[
	};


private:
	static RoomData		m_room;
	static UDPServer*	m_pServer;// �����Ƒ�����q���T�[�o


	static void NewUser(char* data, int client);			/*	�����Ă����v���C���[�ɂ��̃v���C���[��ID��n��			*/
	static void InitData(char* data, int client);			/*	�Q��													*/
	static void ExitUser(char* data, int client);			/*	�N���C�A���g�Ƃ̐ڑ���؂�A�g���Ă����������Z�b�g	*/
	static void TeamData(char* data, int client);			/*	�`�[���I��												*/
	static void GameInitData(char* data, int client);		/*	�S����isReady==true���ƃQ�[���J�n�I						*/
	//static void InitUser(char* data, int client);
	static void UpdateUser(char* data, int client);			/*	�v���C���[�̍X�V										*/
	static void UpdateStage(int client);		/*	�|�X�^�[�̍X�V											*/
	static void UpdateLayer(char* data, int client);		/*	���C���[�̍X�V									�@�@	*/
	static void UpdateScore(int client);		/*	�X�R�A�̍X�V	*/

	static void GameEndGame(char* data, int client);			/* �G���h�Q�[�� */


public:
	static void Init();			/*	�l�b�g�v���O�����J�n	*/
	static void Release();		/*	�l�b�g�v���O�����I��	*/
	static void Update();		/*	���̍X�V				*/

	static void Render();		/*	�\�P�b�g�ȂǏ���`��	*/


	//���[���̃Q�b�^�[
	static const ContorlDesc&	GetDesc(int i){ return m_room.desc[i]; }
	static const UserData&		GetUser(int i){ return m_room.user[i]; }
	static const LayerData&		GetLayer(int i){ return m_room.layer[i]; }


	//�@�v���C���[�̑��얽�߂����Z�b�g
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