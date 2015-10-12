#pragma once

#include	<time.h>
#include	<windows.h>
#include	<process.h>
#include	<d3dx9.h>


struct UserData;
class  SlotClient;

struct	GameUserInfo
{
	char					name[9];
	Global::TEAM_NAME		team;
	const int				teamNo;
	int						coinNum;
	int						resultKill;
	int						resultDeath;
	bool					isAI;
	GameUserInfo( const char* name, int team, int teamNo, int initCoin, bool isAI )
	 : team( ( team == 0x00 ) ? Global::TEAM_A : Global::TEAM_B ),
	  teamNo( teamNo ),
	  coinNum( initCoin ),
	  resultKill( 0 ),
	  resultDeath( 0 ),
	  isAI( isAI )
	 {
	 	strcpy_s( this->name, name );
	 }
};
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
	RESPAWN,
};
enum	SLOT_STATE
{
	INIT,
	SELECT,
	SERVER_ENQUIRY,
	PLAY,
	UNION
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

//*****************************************************************************************************************************
// ゲームマスター( クライアント )
//*****************************************************************************************************************************
class GameMasterClientA
{
private:
	//	ゲーム情報
	static GAME_STATE_TYPE		gameState;
	static GameUserInfo*		userInfo;
	static time_t				startTime;
	static time_t				endTime;
	static time_t				curTime;
	static time_t				gameWidth;

	//	自分自身の情報
	static int					myID;
	static PLAYER_STATE			tankState;
	static SLOT_STATE			slotState;
	static BYTE					userReqestSlotButton;
	static BYTE					userSelectSlot;

	//	スロット
	static	SlotClient*			slotArray[SLOT_NUM];

	//	占有率情報
	static Global::TEAM_NAME	occupyTeam;
	static float				occupyRate;
	static Vector4				circleColor;

	//	情報一時保持用
	static	BYTE				netReciveGameState;
	static	BYTE				netReciveTankState;
	static	WORD				netReciveCoin[ PANZER_MAX ];
	static	WORD				netSlotActionInfo[SLOT_NUM];
	static	WORD				netAssistData[ PANZER_MAX ];
	static	WORD				netOccupyData[ PANZER_MAX ];

	//	描画用情報
	static iex2DObj*			teamPlate;
	static iex2DObj*			gaugeOutSideImage;
	static iex2DObj*			teamNamePlate[2];
	static iex2DObj*			teamCPUPlate[2];
	static iex2DObj*			teamGagebar[2];
	
public:
	// ---- 試合前 -------------------------------------------------------------------------------------
	// :_読み込み:画像とスロット
	static void LoadInfoImage();
	// :_初期化:userInfoとゲーム情報
	static void GameInitialize();

	// ---- 試合中 -------------------------------------------------------------------------------------
	// :_更新:サーバーからの受け取った情報を反映
	static void UpdateServerInfo();
	// :_更新:占有ゲージとステージ円更新( 未実装 )
	static void UpdateOccupyRate();
	// :_更新:スロット回転
	static void UpdateSlots();

	// ---- 試合終了 -----------------------------------------------------------------------------------
	// :_解放:画像とスロット
	static void ReleaseInfoImage();
	// :_解放:userInfo
	static void GameExit();
	

	// ゲーム情報を描画 
	static void RenderPlayerInfo();	
	static void RenderGameInfo();
	static void RenderPlayerCardImage();
	static void RenderCenterCardImage();
	static void RenderSlots();

	/* + 情報設定 *************************************************************************************************************************/	 
	// リスポーン用ステップ
	static void		SetSlotState( SLOT_STATE state )				{ slotState	= state;	}

	// 送信用情報設定
	static	void	SetSendSelectSlot( BYTE slot )					{ userSelectSlot		= slot;		}
	static	void	SetSendReqest( BYTE reqest )					{ userReqestSlotButton  = reqest;	}
	static	void	SendReqestClear()								{ userReqestSlotButton	= 0x00;		}

	// 受信情報設定
	static	void	SetReciveGameState( BYTE state )				{ netReciveGameState		= state;		}
	static	void	SetReciveState( BYTE state )					{ netReciveTankState		= state;		}
	static	void	SetRecivePlayerCoin( int id, WORD coin )		{ netReciveCoin		[ id ]	= coin;			}
	static	void	SetReciveSlotActionInfo(int id, WORD actionInfo){ netSlotActionInfo	[ id ]	= actionInfo;	}
	static	void	SetReciveSlotUV( int id, int reel, float uv );
	static	void	SetReciveOccupy( float rate )					{ occupyRate = rate;						}
	static	void	SetReciveResult( u16 result[PANZER_MAX][2] ){
		for(size_t i = 0; i < PANZER_MAX; i++)
		{
			userInfo[i].resultKill	= result[i][0];
			userInfo[i].resultDeath	= result[i][1];
		}
	}
	static	void	SetRecivePlayerOccupyBonus( int id, WORD coin )		{ netOccupyData		[ id ]	= coin;			}
	static	void	SetRecivePlayerAssistBonus( int id, WORD coin )		{ netAssistData		[ id ]	= coin;			}

	/* + 情報取得 *************************************************************************************************************************/	 
	// ゲーム情報
	static GAME_STATE_TYPE			GetGameState()					{ return gameState;					}
	static	bool					IsGameEnd()						{ return ( curTime > endTime );		}
	static	bool					IsGamePlay()					{ return ( curTime < startTime && curTime > endTime );		}
	static time_t					GetCurTime()					{ return curTime;							}

	// 自分自身の情報
	static int						GetMyID()						{ return myID;						}
	static PLAYER_STATE				GetMyTankState()				{ return tankState;					}
	static SLOT_STATE				GetMySlotState()				{ return slotState;					}
	static const char*				GetMyName()						{ return userInfo[ myID ].name;		}
	static Global::TEAM_NAME		GetMyTeam()						{ return userInfo[ myID ].team;		}
	static int						GetMyTeamID()					{ return userInfo[ myID ].teamNo;	}
	static BYTE						GetSelectSlot()					{ return userSelectSlot;			}
	static BYTE						GetReqest()						{ return userReqestSlotButton;		}

	// 敵チーム
	static Global::TEAM_NAME		GetEnemyTeam(){
		return ( GetMyTeam() == Global::TEAM_A ) ? Global::TEAM_B : Global::TEAM_A;
	}

	// 他のプレイヤー情報
	static const char* 				GetPlayerName( int id )			{ return userInfo[ id ].name;		}
	static	int			 			GetPlayerCoin( int id )			{ return userInfo[ id ].coinNum;	}
	static	int			 			GetAssistBonusCoin( int id )	{ return (int)netAssistData[id];	}
	static	int			 			GetStageBonusCoin( int id )		{ return (int)netOccupyData[id];	}
	static	Global::TEAM_NAME		GetPlayerTeam( int id )			{ return userInfo[ id ].team;		}
	static	int			 			GetPlayerTeamID( int id )		{ return userInfo[ id ].teamNo;	}
	static	int						GetPlayerResultKill( int id )	{ return userInfo[id].resultKill;	}
	static	int						GetPlayerResultDeath( int id )	{ return userInfo[id].resultDeath;	}
	static  bool					IsAI( int id )					{ return userInfo[id].isAI;			}

	// スロット情報
	static	SlotClient*				GetSlot( int id )				{	return	slotArray[ id ];			}
	static	int						GetNumSlot( void )				{	return	SLOT_NUM;					}

	// dbg用
	static	void					dbgInfo();


	

};




