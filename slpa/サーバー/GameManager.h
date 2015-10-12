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
		SINGLE,		//	��l�p
		MUTCH		//	�l�b�g���[�N
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

	//����ǉ�
	int					invincibleFrame;
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
	PlayerInfo*				playerList;				
	time_t					startTime;				
	time_t					endTime;				
	time_t					curTime;				

	//	�X�V�p
	bool					isGameStart;			
	Framework*				MainFrame;				

	// �X���b�g���
	// :_�r�b�g���Z�p
	enum SLOT_ACTIONINFO_BIT_OPERAND
	{
		IS_ACTIVE		= 0x1000,				//	�N����
		IS_STOP_TOP		= 0x2000,				//	��]��  ( '��i'��~�{�^����������ĂȂ� )
		IS_STOP_CENTER	= 0x4000,				//	��]��  ( '���i'��~�{�^����������ĂȂ� )
		IS_STOP_BOTTOM	= 0x8000,				//	��]��  ( '���i'��~�{�^����������ĂȂ� )
		NUM_TOP			= 0x0F00,				//	�ԍ��擾( ��i ) ( 'F'�@�Ȃ�G���[ )
		NUM_CENTER		= 0x00F0,				//	�ԍ��擾( ���i ) ( 'F'�@�Ȃ�G���[ )
		NUM_BOTTOM		= 0x000F,				//	�ԍ��擾( ���i ) ( 'F'�@�Ȃ�G���[ )
	};
	enum SLOT_REQEST_BIT_OPERAND
	{											
		BET_TYPE	= 0xF0,						//   0xF0:�x�b�g���ꂽ�{�^���^�C�v
		REEL_TYPE	= 0x0F,						// ���͏��
		REEL_TOP	= 0x01,						//   0x01:�N���b�N���ꂽ���[��( �� )
		REEL_CENTER	= 0x02,						//   0x02:�N���b�N���ꂽ���[��( �� )
		REEL_BOTTOM	= 0x04,						//   0x04:�N���b�N���ꂽ���[��( �� ) 
	};

	// :_���ێ��p�\����( �X���b�g )
	struct 
	{
		// ���( �N���C�A���g�ɑ��M���� )
		WORD	actionInfo;
		// ��]��  [��,��,��]
		float	reelUV[3];
		// �}���ԍ�[��,��,��]
		int		reelNo[3];
		// ���X�|�[���ʒu���
		Matrix	transMatrix;	

	}		slotInfo[ SLOT_NUM ];			

	// :_���ێ��p�\����( �A�V�X�g )
	struct AssistData
	{
		int timer[PANZER_MAX];
	} assistData[PANZER_MAX];

	// �Z���^�[�t���b�O���
	float					occupyRate;

	//	�ʐM�p
	bool			isRun;							
	EL::Thread*		m_pThread;						

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
	// :_�X�V:netPlayer
	void UpdateCliant( int playerID );
	//	:_�X�V:slot
	void UpdateReel( int id );	

public:
	// :_��ԑJ��:��Ԃ����S�i�v���C���[�ɎE���ꂽ�j
	void DeathTank( int ti, int shooter );								
	// :_��ԑJ��:��Ԃ����S�i���Łj
	void SelfDeathTank( int corpse );								
	// :_��ԑJ��:��Ԃ�����
	void RespawnTank( int playerID, Player* tank, int bet, int slot );	
	// :_��ԑJ��:��Ԃ��Q��
	void CombatTank( int playerID );								
public:
	// :_����:slot
	void PlaySlot( int id, int slot, BYTE botton );									

	// :_����:�`�[���{�[�i�X
	void TeamBonus( int team, int type, int coinNum );

	// :_����:�R�C������
	void AddCoin(int ID, size_t type, int coinNum );

	// ::_�ݒ�:�A�V�X�g���
	void SetAssistTank( int shooter, int hitTank );

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
	static void ThreadFunc( void* pData );		
	// :_�ʐM�̏I��( �T�[�o�[�X�V���[�v�̏I�� )													
	void ServerDisActivate(){ 	isRun	= false;	}						
public:	
	
	// :_����:�X���b�g���g�p���Ă���v���C���[
	int SearchSlot( int slotNum );
	
	/* + ���ݒ� *************************************************************************************************************************/	 
	void ConnectClient( int client ){
		playerList[client].isClientConnect = true;
	}
	void DisConnectClient( int client ){
		if( playerList != nullptr )
			playerList[client].isClientConnect = false;
	}

	/* + ���擾 *************************************************************************************************************************/	 
	//	�Q�[�����
	bool					IsGameStart()					const { return isGameStart;						}
	bool					IsGameTimerEnd()				const { return ( curTime > endTime );			}
	GAME_STATE_TYPE			GetGameState()					const { return gameState;						}		
	time_t					GetStartTime()					const { return startTime;						}
	time_t					GetCurTime()					const { return curTime;							}
	time_t					GetEndTime()					const { return endTime;							}
	int						GetGiveCoinNum( int shooter, int hitTank ) const;

	//	�v���C���[���
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

	//����ǉ�
	bool					IsInvincibleTank(int id)		const { return (playerList[id].invincibleFrame > 0); };

	//	�X���b�g���
	WORD					GetSlotActionFlag( int id )		const { return slotInfo[id].actionInfo;			}
	const Matrix			GetSlotTrankMatrix( int id )	const { return slotInfo[id].transMatrix;		}
	float					GetSlotUV( int id, int reel )	const { return slotInfo[id].reelUV[reel];		}
	
	// �Z���^�[�t���b�O���
	float					GetOccupyRate()					const { return occupyRate;						}	
	
	
	//	DBG�p
	void	dbgInfo();



public:
	//	�폜�\��
	Vector3					GetFlagPos();
	Global::TEAM_NAME		GetOccupyTeam();
};




GameMasterServerA*	GetGameInfo();


#define	GAME_MASTER	GetGameInfo()


