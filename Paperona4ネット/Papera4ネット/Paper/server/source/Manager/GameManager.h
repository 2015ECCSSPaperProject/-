#pragma once

#include	<windows.h>
#include	<d3dx9.h>
#include	"../Net/PaperServer.h" 
#include	<vector>


class	Scene;
struct	UserData;
class	Player;
class	Framework;
class Thread;

enum	GAME_STATE_TYPE
{
	WAIT,
	MUTCH,
	GAME_RESULT,
};

//*****************************************************************************************************************************
// �Q�[���}�X�^�[( �T�[�o�[ )
//*****************************************************************************************************************************
class GameMasterServerA
{
private:
	HANDLE	hMutex;

private:
	// �Q�[�����
	GAME_STATE_TYPE			gameState;
	time_t					startTime;
	time_t					endTime;
	time_t					curTime;

	//	�X�V�p
	bool					isGameStart;
	Framework*				MainFrame;




	//	�ʐM�p
	bool			isRun;
	Thread*		m_pThread;

public:
	// �R���X�g���N�^�E�f�X�g���N�^
	GameMasterServerA();
	~GameMasterServerA();
	void GameRun();

	// ---- �����O -------------------------------------------------------------------------------------
	// :_������:sceneMutch
	bool GameInitialize();
	// :_playerList�̊��蓖��
	void CreateRoom();

	// ---- ������ -------------------------------------------------------------------------------------
	// :_�X�V:sceneMutch
	void Update();

private:

private:
	// ---- �����I�� -----------------------------------------------------------------------------------
	// :_���:playerList
	void DestroyRoom();
	// :_�Q�[����ҋ@��Ԃ�:
	void MutchExit();
	// :_���:m_pThread��MainFrame
	void SystemExit();

	// ---- �ʐM ( �T�u�X���b�h ) -----------------------------------------------------------------------------------
	// :_�ʐM�̊J�n
	void ServerActivate();
	// :_�ʐM���̏���
	static void ThreadFunc(void* pData);
	// :_�ʐM�̏I��( �T�[�o�[�X�V���[�v�̏I�� )													
	void ServerDisActivate(){ isRun = false; }
public:


public:

};




GameMasterServerA*	GetGameInfo();


#define	GAME_MASTER	GetGameInfo()


