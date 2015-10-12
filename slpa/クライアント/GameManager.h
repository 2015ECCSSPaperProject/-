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
		KILL_BONUS,			//	�|������Ԃɂ��{�[�i�X�i�L���X�g�j
		INBRED_BONUS,		//	�|������Ԃ̃x�b�g���� + �����̃x�b�g����
		OCCUPY_BONUS,		//	�苒�ł̃{�[�i�X
		ITEM_BONUS,			//	�A�C�e���̉e���ɂ��ǉ��{�[�i�X
		EVENT_BONUS,		//	�C�x���g�̏����ɂ���V
		ASSIST_BONUS,		//	�A�V�X�g�{�[�i�X
		BET_TOTAL,			//	�x�b�g������( num <= 0 )
		CONFISCATE,			//	���s�ɂ��R�C���̏��n( ����:(�|�R�C������/2), ����:(�{�R�C������/2),
		BONUS_VALUE
	};
	int  _m[BONUS_VALUE];
};

//*****************************************************************************************************************************
// �Q�[���}�X�^�[( �N���C�A���g )
//*****************************************************************************************************************************
class GameMasterClientA
{
private:
	//	�Q�[�����
	static GAME_STATE_TYPE		gameState;
	static GameUserInfo*		userInfo;
	static time_t				startTime;
	static time_t				endTime;
	static time_t				curTime;
	static time_t				gameWidth;

	//	�������g�̏��
	static int					myID;
	static PLAYER_STATE			tankState;
	static SLOT_STATE			slotState;
	static BYTE					userReqestSlotButton;
	static BYTE					userSelectSlot;

	//	�X���b�g
	static	SlotClient*			slotArray[SLOT_NUM];

	//	��L�����
	static Global::TEAM_NAME	occupyTeam;
	static float				occupyRate;
	static Vector4				circleColor;

	//	���ꎞ�ێ��p
	static	BYTE				netReciveGameState;
	static	BYTE				netReciveTankState;
	static	WORD				netReciveCoin[ PANZER_MAX ];
	static	WORD				netSlotActionInfo[SLOT_NUM];
	static	WORD				netAssistData[ PANZER_MAX ];
	static	WORD				netOccupyData[ PANZER_MAX ];

	//	�`��p���
	static iex2DObj*			teamPlate;
	static iex2DObj*			gaugeOutSideImage;
	static iex2DObj*			teamNamePlate[2];
	static iex2DObj*			teamCPUPlate[2];
	static iex2DObj*			teamGagebar[2];
	
public:
	// ---- �����O -------------------------------------------------------------------------------------
	// :_�ǂݍ���:�摜�ƃX���b�g
	static void LoadInfoImage();
	// :_������:userInfo�ƃQ�[�����
	static void GameInitialize();

	// ---- ������ -------------------------------------------------------------------------------------
	// :_�X�V:�T�[�o�[����̎󂯎�������𔽉f
	static void UpdateServerInfo();
	// :_�X�V:��L�Q�[�W�ƃX�e�[�W�~�X�V( ������ )
	static void UpdateOccupyRate();
	// :_�X�V:�X���b�g��]
	static void UpdateSlots();

	// ---- �����I�� -----------------------------------------------------------------------------------
	// :_���:�摜�ƃX���b�g
	static void ReleaseInfoImage();
	// :_���:userInfo
	static void GameExit();
	

	// �Q�[������`�� 
	static void RenderPlayerInfo();	
	static void RenderGameInfo();
	static void RenderPlayerCardImage();
	static void RenderCenterCardImage();
	static void RenderSlots();

	/* + ���ݒ� *************************************************************************************************************************/	 
	// ���X�|�[���p�X�e�b�v
	static void		SetSlotState( SLOT_STATE state )				{ slotState	= state;	}

	// ���M�p���ݒ�
	static	void	SetSendSelectSlot( BYTE slot )					{ userSelectSlot		= slot;		}
	static	void	SetSendReqest( BYTE reqest )					{ userReqestSlotButton  = reqest;	}
	static	void	SendReqestClear()								{ userReqestSlotButton	= 0x00;		}

	// ��M���ݒ�
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

	/* + ���擾 *************************************************************************************************************************/	 
	// �Q�[�����
	static GAME_STATE_TYPE			GetGameState()					{ return gameState;					}
	static	bool					IsGameEnd()						{ return ( curTime > endTime );		}
	static	bool					IsGamePlay()					{ return ( curTime < startTime && curTime > endTime );		}
	static time_t					GetCurTime()					{ return curTime;							}

	// �������g�̏��
	static int						GetMyID()						{ return myID;						}
	static PLAYER_STATE				GetMyTankState()				{ return tankState;					}
	static SLOT_STATE				GetMySlotState()				{ return slotState;					}
	static const char*				GetMyName()						{ return userInfo[ myID ].name;		}
	static Global::TEAM_NAME		GetMyTeam()						{ return userInfo[ myID ].team;		}
	static int						GetMyTeamID()					{ return userInfo[ myID ].teamNo;	}
	static BYTE						GetSelectSlot()					{ return userSelectSlot;			}
	static BYTE						GetReqest()						{ return userReqestSlotButton;		}

	// �G�`�[��
	static Global::TEAM_NAME		GetEnemyTeam(){
		return ( GetMyTeam() == Global::TEAM_A ) ? Global::TEAM_B : Global::TEAM_A;
	}

	// ���̃v���C���[���
	static const char* 				GetPlayerName( int id )			{ return userInfo[ id ].name;		}
	static	int			 			GetPlayerCoin( int id )			{ return userInfo[ id ].coinNum;	}
	static	int			 			GetAssistBonusCoin( int id )	{ return (int)netAssistData[id];	}
	static	int			 			GetStageBonusCoin( int id )		{ return (int)netOccupyData[id];	}
	static	Global::TEAM_NAME		GetPlayerTeam( int id )			{ return userInfo[ id ].team;		}
	static	int			 			GetPlayerTeamID( int id )		{ return userInfo[ id ].teamNo;	}
	static	int						GetPlayerResultKill( int id )	{ return userInfo[id].resultKill;	}
	static	int						GetPlayerResultDeath( int id )	{ return userInfo[id].resultDeath;	}
	static  bool					IsAI( int id )					{ return userInfo[id].isAI;			}

	// �X���b�g���
	static	SlotClient*				GetSlot( int id )				{	return	slotArray[ id ];			}
	static	int						GetNumSlot( void )				{	return	SLOT_NUM;					}

	// dbg�p
	static	void					dbgInfo();


	

};




