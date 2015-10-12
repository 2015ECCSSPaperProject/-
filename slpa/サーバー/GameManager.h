#pragma once

#include	<windows.h>
#include	<d3dx9.h>
#include	"../Net/Server.h" 
#include	<vector>

namespace Global
{
	enum TEAM_NAME
	{
		TEAM_A,
		TEAM_B,
		TEAM_NO,
	};
	enum GAME_MODE
	{
		SINGLE,		//	一人用
		MUTCH		//	ネットワーク
	};
}

class	Scene;
struct	UserData;
class	Player;
class	Framework;
namespace EL{ class Thread;};

enum	GAME_STATE_TYPE
{
	WAIT,
	MUTCH,
	GAME_RESULT,
};
enum	PLAYER_STATE
{
	COMBAT,
	DEATH,
	RESPAWN
};
struct CoinData
{
	enum 
	{
		DEFAULT_BONUS,		//	+2
		KILL_BONUS,			//	倒した戦車によるボーナス（キルスト）
		INBRED_BONUS,		//	倒した戦車のベット枚数 + 自分のベット枚数
		OCCUPY_BONUS,		//	占拠でのボーナス
		ITEM_BONUS,			//	アイテムの影響による追加ボーナス
		EVENT_BONUS,		//	イベントの勝利による報酬
		ASSIST_BONUS,		//	アシストボーナス
		BET_TOTAL,			//	ベット総枚数( num <= 0 )
		CONFISCATE,			//	勝敗によるコインの譲渡( 負け:(－コイン枚数/2), 勝ち:(＋コイン枚数/2),
		BONUS_VALUE
	};
	int  _m[BONUS_VALUE];
};
struct PlayerInfo
{
	bool				isAI;
	bool				isClientConnect;
	char				name[256];
	Global::TEAM_NAME	team;
	int					teamNo;				
	PLAYER_STATE		state;
	CoinData			coinData;

	BYTE				curPlaySlot;
	BYTE				reqest;
	int					oneLifeRecord;	
	int					betNum;
	int					totalKill;		
	int					totalDeath;

	//中野追加
	int					invincibleFrame;
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
	PlayerInfo*				playerList;				
	time_t					startTime;				
	time_t					endTime;				
	time_t					curTime;				

	//	更新用
	bool					isGameStart;			
	Framework*				MainFrame;				

	// スロット情報
	// :_ビット演算用
	enum SLOT_ACTIONINFO_BIT_OPERAND
	{
		IS_ACTIVE		= 0x1000,				//	起動中
		IS_STOP_TOP		= 0x2000,				//	回転中  ( '上段'停止ボタンを押されてない )
		IS_STOP_CENTER	= 0x4000,				//	回転中  ( '中段'停止ボタンを押されてない )
		IS_STOP_BOTTOM	= 0x8000,				//	回転中  ( '下段'停止ボタンを押されてない )
		NUM_TOP			= 0x0F00,				//	番号取得( 上段 ) ( 'F'　ならエラー )
		NUM_CENTER		= 0x00F0,				//	番号取得( 中段 ) ( 'F'　ならエラー )
		NUM_BOTTOM		= 0x000F,				//	番号取得( 下段 ) ( 'F'　ならエラー )
	};
	enum SLOT_REQEST_BIT_OPERAND
	{											
		BET_TYPE	= 0xF0,						//   0xF0:ベットされたボタンタイプ
		REEL_TYPE	= 0x0F,						// 入力情報
		REEL_TOP	= 0x01,						//   0x01:クリックされたリール( 上 )
		REEL_CENTER	= 0x02,						//   0x02:クリックされたリール( 中 )
		REEL_BOTTOM	= 0x04,						//   0x04:クリックされたリール( 下 ) 
	};

	// :_情報保持用構造体( スロット )
	struct 
	{
		// 状態( クライアントに送信する )
		WORD	actionInfo;
		// 回転量  [上,中,下]
		float	reelUV[3];
		// 図柄番号[上,中,下]
		int		reelNo[3];
		// リスポーン位置情報
		Matrix	transMatrix;	

	}		slotInfo[ SLOT_NUM ];			

	// :_情報保持用構造体( アシスト )
	struct AssistData
	{
		int timer[PANZER_MAX];
	} assistData[PANZER_MAX];

	// センターフラッグ情報
	float					occupyRate;

	//	通信用
	bool			isRun;							
	EL::Thread*		m_pThread;						

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
	// :_更新:netPlayer
	void UpdateCliant( int playerID );
	//	:_更新:slot
	void UpdateReel( int id );	

public:
	// :_状態遷移:戦車が死亡（プレイヤーに殺された）
	void DeathTank( int ti, int shooter );								
	// :_状態遷移:戦車が死亡（自滅）
	void SelfDeathTank( int corpse );								
	// :_状態遷移:戦車が復活
	void RespawnTank( int playerID, Player* tank, int bet, int slot );	
	// :_状態遷移:戦車が参戦
	void CombatTank( int playerID );								
public:
	// :_操作:slot
	void PlaySlot( int id, int slot, BYTE botton );									

	// :_操作:チームボーナス
	void TeamBonus( int team, int type, int coinNum );

	// :_操作:コイン増加
	void AddCoin(int ID, size_t type, int coinNum );

	// ::_設定:アシスト情報
	void SetAssistTank( int shooter, int hitTank );

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
	static void ThreadFunc( void* pData );		
	// :_通信の終了( サーバー更新ループの終了 )													
	void ServerDisActivate(){ 	isRun	= false;	}						
public:	
	
	// :_検索:スロットを使用しているプレイヤー
	int SearchSlot( int slotNum );
	
	/* + 情報設定 *************************************************************************************************************************/	 
	void ConnectClient( int client ){
		playerList[client].isClientConnect = true;
	}
	void DisConnectClient( int client ){
		if( playerList != nullptr )
			playerList[client].isClientConnect = false;
	}

	/* + 情報取得 *************************************************************************************************************************/	 
	//	ゲーム情報
	bool					IsGameStart()					const { return isGameStart;						}
	bool					IsGameTimerEnd()				const { return ( curTime > endTime );			}
	GAME_STATE_TYPE			GetGameState()					const { return gameState;						}		
	time_t					GetStartTime()					const { return startTime;						}
	time_t					GetCurTime()					const { return curTime;							}
	time_t					GetEndTime()					const { return endTime;							}
	int						GetGiveCoinNum( int shooter, int hitTank ) const;

	//	プレイヤー情報
	const	char*			GetPlayerName( int id )			const { return playerList[id].name;				}
	PLAYER_STATE			GetPlayerState( int id )		const { return playerList[id].state;			}
	Global::TEAM_NAME		GetPlayerTeam( int id )			const { return playerList[id].team;				}
	int						GetPlayerTeamNo( int id )		const { return playerList[id].teamNo;			}
	int						GetPlayerCoinNum(  int id )		const {
		int out = 0;
		size_t length = ARRAYSIZE( playerList[id].coinData._m );
		for(size_t i = 0; i < length; i++)
			out += playerList[id].coinData._m[i];
		return out;
	}	
	const CoinData&			GetPlayerCoinData( int id )		const { return playerList[id].coinData;			}
	int						GetCurPlaySlot( int id )		const { return playerList[id].curPlaySlot;		}
	bool					IsAITank( int id )				const { return playerList[id].isAI;				}
	int						GetPlayerTotalDeath(  int id )	const { return playerList[id].totalDeath;		}	
	int						GetPlayerTotalKill(  int id )	const { return playerList[id].totalKill;		}	
	int						GetOneLifeRecord( int id )		{ return playerList[id].oneLifeRecord;		}
	int						GetNumBet( int id )				{ return playerList[id].betNum;				}

	//中野追加
	bool					IsInvincibleTank(int id)		const { return (playerList[id].invincibleFrame > 0); };

	//	スロット情報
	WORD					GetSlotActionFlag( int id )		const { return slotInfo[id].actionInfo;			}
	const Matrix			GetSlotTrankMatrix( int id )	const { return slotInfo[id].transMatrix;		}
	float					GetSlotUV( int id, int reel )	const { return slotInfo[id].reelUV[reel];		}
	
	// センターフラッグ情報
	float					GetOccupyRate()					const { return occupyRate;						}	
	
	
	//	DBG用
	void	dbgInfo();



public:
	//	削除予定
	Vector3					GetFlagPos();
	Global::TEAM_NAME		GetOccupyTeam();
};




GameMasterServerA*	GetGameInfo();


#define	GAME_MASTER	GetGameInfo()


