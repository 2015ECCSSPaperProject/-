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
// ゲームマスター( サーバー )
//*****************************************************************************************************************************
class GameMasterServerA
{
private:
	HANDLE	hMutex;

private:
	// ゲーム情報
	GAME_STATE_TYPE			gameState;
	time_t					startTime;
	time_t					endTime;
	time_t					curTime;

	//	更新用
	bool					isGameStart;
	Framework*				MainFrame;




	//	通信用
	bool			isRun;
	Thread*		m_pThread;

public:
	// コンストラクタ・デストラクタ
	GameMasterServerA();
	~GameMasterServerA();
	void GameRun();

	// ---- 試合前 -------------------------------------------------------------------------------------
	// :_初期化:sceneMutch
	bool GameInitialize();
	// :_playerListの割り当て
	void CreateRoom();

	// ---- 試合中 -------------------------------------------------------------------------------------
	// :_更新:sceneMutch
	void Update();

private:

private:
	// ---- 試合終了 -----------------------------------------------------------------------------------
	// :_解放:playerList
	void DestroyRoom();
	// :_ゲームを待機状態へ:
	void MutchExit();
	// :_解放:m_pThreadとMainFrame
	void SystemExit();

	// ---- 通信 ( サブスレッド ) -----------------------------------------------------------------------------------
	// :_通信の開始
	void ServerActivate();
	// :_通信中の処理
	static void ThreadFunc(void* pData);
	// :_通信の終了( サーバー更新ループの終了 )													
	void ServerDisActivate(){ isRun = false; }
public:


public:

};




GameMasterServerA*	GetGameInfo();


#define	GAME_MASTER	GetGameInfo()


