#pragma once
#include<list>
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
		POSTER_DATA = 6,
	
	};

	/* ���[�U�[�̃f�[�^ */
	struct RoomData
	{
		UserData		user[PLAYER_MAX];
		ContorlDesc		desc[PLAYER_MAX];
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
	static void UpdatePoster(int client);		/*	�|�X�^�[�̍X�V											*/
public:
	static void Init();			/*	�l�b�g�v���O�����J�n	*/
	static void Release();		/*	�l�b�g�v���O�����I��	*/
	static void Update();		/*	���̍X�V				*/

	static void Render();		/*	�\�P�b�g�ȂǏ���`��	*/


	//���[���̃Q�b�^�[
	static const ContorlDesc&	GetDesc(int i){ return m_room.desc[i]; }
	static const UserData&		GetUser(int i){ return m_room.user[i]; }
	
	//�@�v���C���[�̑��얽�߂����Z�b�g
	static void ResetDesc(int i)
	{
		m_room.desc[i].com = 0;
		//m_room.desc[i].moveFlag = 0;

		//m_room.desc[i].mouseX = .0f;
		//m_room.desc[i].mouseY = .0f;
	}

};