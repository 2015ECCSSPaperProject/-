#include	<assert.h>
#include	<time.h>

#include	"../template.h"
#include	"../../IEX/iextreme.h"

#include	"../Manager/GameManager.h"
#include	"InputManager.h"
#include	"ShadingManager.h"


#include	"system/system.h"
#include	"../SystemAdd/Drawing.h"
#include	"../system/Framework.h"

#include	"Item.h"

#include	"../Object/Tank.h"
#include	"../Object/TankController.h"
#include	"../Object/Player.h"
#include	"../Object/AI.h"
#include	"../Net/NetPlayer.h"
#include	"../Object/TankFactory.h"

#include	"../Object/Parts.h"

#include	"../Slot/Coin.h"
#include	"../Slot/Slot.h"
#include	"../Scene/sceneMain.h"

#include	"Interface.h"
#include	"../Scene/sceneMain.h"
#include	"../InformationBuffer.h"

#include	"../Object/Stage.h"

#include	"Server.h"
#include	"../Scene/sceneMutch.h"

#include	"CapsuleWeapon.h"

#include	"../appMath.h"

#include	"../system/Thread.h"

/* **************************************************************************************** */

#define	CPU_NAME			"�b�o�t"
#define	NAME_BUFFER_MAX		9

float Wrap(float val)
{
	if(val < 0.0f)		return val + 1.0f;
	if(val > 1.0f)		return val - 1.0f;
	return val;
}
float Clamp(float val,float minval,float maxVal)
{
		 if( val < minval )	return minval;
	else if( val > maxVal )	return maxVal;
	else					return val;
}

//**********************************************************************************************
//
//	�Q�[���}�X�^�[
//
//**********************************************************************************************

namespace		
{	
	const int	 initCoinNum		= 10;		//	�����l( �R�C�� ) 
	PLAYER_STATE initState			= DEATH;	//	�����l( ��ԁ@ ) 
}

//**********************************************************************************************
//	�R���X�g���N�^�E�f�X�g���N�^
//**********************************************************************************************
GameMasterServerA::GameMasterServerA()
 : playerList( nullptr )
{
	hMutex		= CreateMutex( NULL, FALSE, NULL );
	MainFrame	= new Framework();
	
	//	�X���b�g(���X�|�[��)�ʒu�ݒ�
	//�@�ǂݍ��݂Ə�����:�X���b�g
	char*	filename	= "DATA/Slot/Slot/Slot4.IMO";
	WORD	state = 0x0000;

	const float SLOT_SCALE		= 8.0f;
	struct 
	{
		Vector3 pos;
		float	angle;
		float	scale;
	} slotConfig[PANZER_MAX]
	{
		{ Vector3(  500.0f,  0.0f, -800.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3(  170.0f,  0.0f, -800.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3( -155.0f,  0.0f, -800.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3( -480.0f,  0.0f, -800.0f ), 0.0f	, SLOT_SCALE },
		{ Vector3(  480.0f,  0.0f,  800.0f ), PI	, SLOT_SCALE },
		{ Vector3(  155.0f,  0.0f,  800.0f ), PI	, SLOT_SCALE },
		{ Vector3( -170.0f,  0.0f,  800.0f ), PI	, SLOT_SCALE },
		{ Vector3( -500.0f,  0.0f,  800.0f ), PI	, SLOT_SCALE },
	};
		
	for(size_t i = 0; i < SLOT_NUM; i++)
	{
		Matrix scale,rot,trans;
		D3DXMatrixScaling( &scale, slotConfig[i].scale, slotConfig[i].scale, slotConfig[i].scale );
		D3DXMatrixRotationY( &rot, slotConfig[i].angle );
		D3DXMatrixTranslation( &trans, slotConfig[i].pos.x, slotConfig[i].pos.y, slotConfig[i].pos.z );
		slotInfo[i].transMatrix = scale * rot * trans;
	}
}
GameMasterServerA::~GameMasterServerA()
{
	CloseHandle(hMutex);
	SAFE_ARRAY_DELETE( playerList );
}

//**********************************************************************************************
// ���C�����[�v����
//**********************************************************************************************
void	GameMasterServerA::GameRun()
{
	ServerActivate();		//	�ʐM�J�n
	MSG	msg;
	
	//	���C�����[�v
	for(;;){
		if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ){
			if( !GetMessage(&msg, NULL, 0, 0) ) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if( IsGameStart() == true )
			if( MainFrame->Update() ) MainFrame->Render();
	}

	Sleep( 100 );

	ServerDisActivate();	//	�ʐM�I��

	DestroyRoom();			//	�����폜
	SystemExit();
}

//**********************************************************************************************
// �ʐM����
//**********************************************************************************************
void GameMasterServerA::ServerActivate()
{
	isRun = true;
	//	�X���b�h����
	m_pThread		= new EL::Thread( ThreadFunc, this );
	m_pThread->Run();
}
void GameMasterServerA::ThreadFunc( void* pData )
{
	GameMasterServerA* pMain((GameMasterServerA*)pData);
	ServerManager::Init();

	while(pMain->isRun)
		ServerManager::Update();
	
	ServerManager::Release();
}

//*****************************************************************************
//	�����O
//*****************************************************************************
bool GameMasterServerA::GameInitialize()
{	
	//	�Q�[���v���C���͏��������Ȃ�
	if( isGameStart == true )	return true;
	WaitForSingleObject( hMutex, INFINITE );
	
	//	�v���C���[���ݒ�	
	CreateRoom();	

	//	�X���b�g��񏉊���
	for( size_t i = 0; i < SLOT_NUM; i++)
	{
		slotInfo[i].actionInfo		=	0x0000;
		slotInfo[i].reelUV[0]		=	  0.0f;
		slotInfo[i].reelUV[1]		=	  0.0f;
		slotInfo[i].reelUV[2]		=	  0.0f;
		slotInfo[i].reelNo[0]		=	     0;
		slotInfo[i].reelNo[1]		=	     0;
		slotInfo[i].reelNo[2]		=	     0;
	}
	for( size_t i = 0; i < PANZER_MAX; i++)
	{
		for(size_t j = 0; j < PANZER_MAX/2; j++)
		{
			assistData[i].timer[j] = 0;
		}
	}

	ReleaseMutex( hMutex );

	//	�Q�[�����ݒ�
	FILE*	fp = nullptr;
	fopen_s( &fp, "./DATA/GAMETIME.txt", "r" );
	assert( fp );
	time_t gameWidth = 0;
	fscanf_s( fp, "%d", &gameWidth );
	fclose( fp );

	gameState	= WAIT;
	startTime	= time( NULL );							
	endTime		= startTime  + gameWidth;	
	curTime		= time( NULL );

	//	scene����
	MainFrame->ChangeScene( new sceneMutch() );

	// �Q�[���O�����͂��̑O�ɏI��点�Ă���( �X���b�h������ )
	isGameStart		= true;

	return true;
}
void GameMasterServerA::CreateRoom()
{
	//	�����l��ݒ�
	SAFE_ARRAY_DELETE( playerList );

	playerList = new PlayerInfo[PANZER_MAX];
	for (s32 i = 0; i < PANZER_MAX; ++i)
	{
		playerList[ i ].isAI			= true;
		playerList[ i ].isClientConnect = true;
		playerList[ i ].name[0]			= '\0';
		playerList[ i ].team			= Global::TEAM_NO;
		playerList[ i ].teamNo			= -1;
		playerList[ i ].state			= initState;
		for(size_t c = 0; c < CoinData::BONUS_VALUE; c++)
			playerList[ i ].coinData._m[c] = 0;		
		AddCoin( i, CoinData::DEFAULT_BONUS, initCoinNum );
		playerList[ i ].curPlaySlot		= 0xFF;
		playerList[ i ].oneLifeRecord	= 0;
		playerList[ i ].betNum			= 0;
		playerList[ i ].reqest			= 0;
		playerList[ i ].totalDeath		= 0;
		playerList[ i ].totalKill		= 0;
		playerList[ i ].invincibleFrame = 0;
	}

	//	'NetPlayer'�ݒ�
	int work[2] = {0, 0};
	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		UserData		userData = ServerManager::GetUser( i );
		if(userData.com == 0 )			continue;						// 'AI' �͏Ȃ�
		Global::TEAM_NAME team = ( userData.row == 0 ) ? Global::TEAM_A : Global::TEAM_B;

		sprintf_s( playerList[i].name, userData.name );					
		playerList[i].team				= team;								
		playerList[i].teamNo			= work[team]++;
		playerList[i].isAI				= false;							
		playerList[i].isClientConnect	= false;
	}

	//	�󂫗̈��'AI'�Ŗ��߂�
	int cnt = 0;
	for (int i=0; i < PANZER_MAX; ++i)
	{
		if( playerList[i].team == Global::TEAM_NO ) 
		{
			Global::TEAM_NAME team = ( work[0] < 4 ) ? Global::TEAM_A : Global::TEAM_B;
			sprintf_s( playerList[i].name, "%s%d", CPU_NAME, cnt++ );			
			playerList[i].team		= team;
			playerList[i].teamNo	= work[team]++;
		}
	}

}

//*****************************************************************************
//	������
//*****************************************************************************
void GameMasterServerA::Update()
{
	// ���[��
	for(size_t i = 0; i < SLOT_NUM; i++)
		UpdateReel( i );
	
	// �X�e�[�W
	occupyRate = StageManager::GetOccupyRate();

	// �N���C�A���g
	for(size_t i = 0; i < PANZER_MAX; i++)
		UpdateCliant( i );
	
	// �Q�[���̏��
	GAME_STATE_TYPE curType = GetGameState();
	switch(curType)
	{
	case WAIT:
	{
		int connectNum = 0;
		for(size_t i = 0; i < PANZER_MAX; i++)
		{
			if(	playerList[i].isClientConnect == true )
				++connectNum;
		}
		if( connectNum == PANZER_MAX )
			gameState = GAME_STATE_TYPE::MUTCH;

	}
	break;
	case MUTCH:
	{
		 curTime = time( NULL );
		int connectNum = 0;
		if( IsGameTimerEnd() == true )
		{
		
			for(size_t i = 0; i < PANZER_MAX; i++)
			{
				if(	playerList[i].isClientConnect	== false ||
					playerList[i].isAI				== true )
				{
					++connectNum;
				}
			}
			if( connectNum == PANZER_MAX )
				MutchExit();
		}
	}
	break;
	default:break;
	}
}
void GameMasterServerA::UpdateCliant( int playerID )
{
	PlayerInfo*	player = &playerList[playerID];

	// �v���C���[���X�V
	switch(player->state)
	{
	case COMBAT:
	{
		if(player->curPlaySlot != 0xFF )
		{
			slotInfo[player->curPlaySlot].actionInfo &= 0x0FFF;
			player->curPlaySlot						  = -1;
		}
	}
		break;
	case DEATH:
	{
		int playSlot = player->curPlaySlot;
		if( playSlot == 0xFF )		return;
		WORD curSlotFlg = slotInfo[player->curPlaySlot].actionInfo;
		if((curSlotFlg & 0xF000 ) == 0xF000 )
		{
			RespawnTank( playerID, tank[playerID], 1, playSlot);
		}
	}
		break;
	case RESPAWN:
	{
		//����ǉ��@���G��Ԃ��X�V
		player->invincibleFrame--;
		if( player->invincibleFrame <= 0 )
			CombatTank( playerID );
	}
		break;
	default:
		break;
	}

	// �A�V�X�g���X�V
	int* timerData = assistData[playerID].timer;
	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		if( timerData[i] > 0 )
			--timerData[i];
	}

}
void GameMasterServerA::UpdateReel( int id )
{
	/* ���擾 */
	WORD	reelFlag	= slotInfo[id].actionInfo;
	WORD	stepFlag	= (WORD)( ( reelFlag & 0xF000 ) );
	WORD	frontNoFlag	= (WORD)( ( reelFlag & 0x0FFF ) );

	bool isActive		= ( ( stepFlag & 0x1000 ) == 0x1000 );
	bool isRotateTop	= ( ( stepFlag & 0x2000 ) == 0x0000 );
	bool isRotateCenter	= ( ( stepFlag & 0x4000 ) == 0x0000 );
	bool isRotateBottom	= ( ( stepFlag & 0x8000 ) == 0x0000 );
	
	//	�����ϐ�
	const float blockNum		= 10.0f;
	const float blockSize		= 1.0f / blockNum;
	const float accelReelSpeed	= 0.02f / 3.0f;

	const int playerID = SearchSlot( id );
	if( playerID == 0xFF )	return;
	const int betNum = playerList[playerID].betNum;

	//	���[���̉�]���x����
	float reelSpeed	= 0.0f;
	if( isActive ){
		int   moveReelCnt = 0;
		const float identSpeed = 0.0105f;
		if( !isRotateTop	)	moveReelCnt++;
		if( !isRotateCenter	)	moveReelCnt++;
		if( !isRotateBottom	)	moveReelCnt++;
		reelSpeed += identSpeed + (float)moveReelCnt*(float)identSpeed;

		float rate[3]
		{
			SLOT_SPEED_BET1,
			SLOT_SPEED_BET2,
			SLOT_SPEED_BET3
		};
		if( betNum > 0 )
			reelSpeed *= rate[betNum-1];
		
   	}
	

	//	����ΏۃX���b�g
	float*	const reelUV	= slotInfo[id].reelUV;
	int*	const reelNo	= slotInfo[id].reelNo;

	//	���[���̏�ԍX�V
	const float roundTU = blockSize / 2;
	        
	/* ��i */
	if( !isRotateTop || !isActive )
	{
		float tu	= reelUV[0] + roundTU;
		reelNo[0]	= (int)( tu / blockSize );
		reelUV[0]	= (float)reelNo[0] * blockSize;
	}
	else
		slotInfo[id].reelUV[0] = Wrap( slotInfo[id].reelUV[0] + reelSpeed );

	/* ���i */
	if( !isRotateCenter || !isActive )
	{
		float tu	= reelUV[1] + roundTU;
		reelNo[1]	= (int)( tu / blockSize );
		reelUV[1]	= (float)reelNo[1] * blockSize;
	}
	else
		slotInfo[id].reelUV[1] = Wrap( slotInfo[id].reelUV[1] + reelSpeed );

	/* ���i */
	if( !isRotateBottom || !isActive )
	{
		float tu	= reelUV[2] + roundTU;
		reelNo[2]	= (int)( tu / blockSize );
		reelUV[2]	= (float)reelNo[2] * blockSize;
	}
	else
		slotInfo[id].reelUV[2] = Wrap( slotInfo[id].reelUV[2] + reelSpeed );

	//	��Ԃ��X�V
	slotInfo[id].actionInfo &= 0xF000;
	slotInfo[id].actionInfo |= (WORD)( ( reelNo[0] & 0x0F ) << 8 );
	slotInfo[id].actionInfo |= (WORD)( ( reelNo[1] & 0x0F ) << 4 );
	slotInfo[id].actionInfo |= (WORD)( ( reelNo[2] & 0x0F ) );

}

const int confiscatediv = 4;
void GameMasterServerA::DeathTank( int corpse, int shooter )		// ( ����, ���� )
{
	if( corpse == shooter )
	{
		SelfDeathTank( corpse );
		return;
	}

	float confiscatef = (float)GetPlayerCoinNum( corpse ) / (float)confiscatediv;	//	
	int confiscate = (int)(confiscatef+0.5f);										//	���s�ɂ��R�C���̏��n( ����:(�|�R�C������/2), ����:(�{�R�C������/2),�l�̌ܓ�

	// �|���ꂽ��Ԃւ̏���
	tank[corpse]->Death();
	tank[corpse]->SetDamage( (float)SHRT_MAX );		// �ʐM����WORD�^�Ɋۂ߂���̂�[SHRT_MAX]
	
	//�o�[�j���O�������
	CapsuleWeapon* Weapon = tank[corpse]->GetCapsuleWeapon();
	if (Weapon)
	{
		Weapon->EnableDeleteRequest();
	}

	TankController::CONTROL_DESC desc;
	desc.acceleMode = TankController::ACCELE_MODE_STAY;
	desc.bodyRot	= 0.0f;
	desc.cannonRot	= 0.0f;
	desc.headRot	= 0.0f;
	desc.fire		= false;
	tank[corpse]->TankController::Update( desc );
	
	playerList[corpse].state		 = DEATH;
	int tiOneRecord					 = playerList[corpse].oneLifeRecord;
	playerList[corpse].oneLifeRecord = 0;
	AddCoin(  corpse,  CoinData::DEFAULT_BONUS,	 1	);

	// ��эX�V
	++playerList[corpse].totalDeath;

	// �|�����v���C���[�������`�[���ł���Γ|�����v���C���[�ɂ͏������s��Ȃ�
	if( playerList[corpse].team != playerList[shooter].team )	
	{
		// ����֓n����������
		AddCoin( corpse, CoinData::CONFISCATE, -confiscate );
		// �R�C����V����	
		int defBonus	= 2;														// �f�t�H���g��V
		int inbredBonus	= playerList[corpse].betNum + playerList[shooter].betNum;		// �X���b�g�x�b�g���ɂ���V[ ������BetNum + �����betNum ]
		int killStreak	= tiOneRecord;												// �L���X�g���[�N( ����̓|���Ă�����Ԃ̑䐔 )
		int itemBonus	= 0;														// �A�C�e���ɂ���V
		{																			// �A�C�e���ɂ���V( �v�Z )
			TankController::ItemEffective item = tank[shooter]->GetItemEffect();
			if( item.effect == ITEM_EFFECT::ITEM_EFFECT_GET_COIN_UP )
				itemBonus = item.level;
		}

		AddCoin( shooter, CoinData::DEFAULT_BONUS,	defBonus	);
		AddCoin( shooter, CoinData::INBRED_BONUS,	inbredBonus );
		AddCoin( shooter, CoinData::KILL_BONUS,		killStreak	);
		AddCoin( shooter, CoinData::ITEM_BONUS,		itemBonus	);
		AddCoin( shooter, CoinData::CONFISCATE,		confiscate	);
	
		// ��эX�V
		++playerList[shooter].oneLifeRecord;
		++playerList[shooter].totalKill;
	}
	
	// �A�V�X�g��Ԃւ̃{�[�i�X
	const int assistBonus = 1;
	for(size_t id = 0; id < PANZER_MAX; id++)
	{
		if( id != corpse && id != shooter && assistData[id].timer[corpse] > 0 )
		{
			AddCoin( id, CoinData::ASSIST_BONUS, assistBonus );	
		}		
		assistData[id].timer[corpse] = 0;
	}

}
void GameMasterServerA::SelfDeathTank( int corpse )
{
	// �|���ꂽ��Ԃւ̏���
	tank[corpse]->Death();
	tank[corpse]->SetDamage( (float)SHRT_MAX );		// �ʐM����WORD�^�Ɋۂ߂���̂�[SHRT_MAX]
	
	TankController::CONTROL_DESC desc;
	desc.acceleMode = TankController::ACCELE_MODE_STAY;
	desc.bodyRot	= 0.0f;
	desc.cannonRot	= 0.0f;
	desc.headRot	= 0.0f;
	desc.fire		= false;
	tank[corpse]->TankController::Update( desc );
	
	playerList[corpse].state		 = DEATH;
	playerList[corpse].oneLifeRecord = 0;
	AddCoin(  corpse,  CoinData::DEFAULT_BONUS,	 1	);

	// �A�V�X�g��Ԃւ̃{�[�i�X
	const int assistBonus = 1;
	for(size_t id = 0; id < PANZER_MAX; id++)
	{
		if( id != corpse && assistData[id].timer[corpse] > 0 )
		{
			AddCoin( id, CoinData::ASSIST_BONUS, assistBonus );	
		}		
		assistData[id].timer[corpse] = 0;
	}

}
void GameMasterServerA::RespawnTank( int playerID, Player* tank, int bet, int slot )
{
	PlayerInfo*	player = &playerList[playerID];
	
	//	�p�����[�^�[���Z�b�g
	tank->SetDamage( 0.0f );
	tank->Clear();
	tank->Born();

	//	��Ԉړ�
	float angle = ( slot < 4 ) ? 0.0f : PI;
	D3DXQUATERNION	orien;
	D3DXQuaternionRotationYawPitchRoll(&orien, angle, 0.0f, 0.0f);
	const Matrix& slotMat = GetSlotTrankMatrix( slot );
	Vector3	slotPos(slotMat._41, slotMat._42 + 10.0f, slotMat._43);
	Vector3 Front(slotMat._31, slotMat._32, slotMat._33);
	Front.Normalize();
	tank->SetOrienAndPos( orien, slotPos + Front * 20.0f);
	

	//	�p�[�c�Z�b�g
	{
		int top		=	(int)( slotInfo[slot].actionInfo >> 8 )	& 0x0F;
		int center	=	(int)( slotInfo[slot].actionInfo >> 4 )	& 0x0F;
		int bottom	=	(int)( slotInfo[slot].actionInfo  )		& 0x0F;
		
		// dbg�͈͉��
		top		%= PARTS_VARIATION_NUM;
		center	%= PARTS_VARIATION_NUM;
		bottom	%= PARTS_VARIATION_NUM;


		int parts[3] = { top, center, bottom };

		//parts[0]	=	8;
		//parts[1]	=	0;
		//parts[2]	=	0;

		tank->SetPartsNum( parts );
		tank->SetCollisionPartsNum();
	}

	//	�����p�[�c���I
	{
		int		killCount	=	GAME_MASTER->GetPlayerTotalKill( playerID );
		int		deathCount	=	GAME_MASTER->GetPlayerTotalDeath( playerID );

		Global::TEAM_NAME	myTeam	=	GetPlayerTeam( playerID );
		int					bonus	=	deathCount - killCount;

		bonus	=	( int )( bonus * 2.0f + 0.5f );

		int		bonusParts		=	0;//rand() % 3;
		
		bool	cannonIsGold	=	( bonusParts == 0 )? rand()%100 < bonus : false;
		bool	bodyIsGold		=	( bonusParts == 1 )? rand()%100 < bonus : false;
		bool	legIsGold		=	( bonusParts == 2 )? rand()%100 < bonus : false;

		tank->SetGold( cannonIsGold, bodyIsGold, legIsGold );
	}
	
	//	�g�p���Ă����X���b�g������		
	if(player->curPlaySlot != 0xFF )
	{
		slotInfo[player->curPlaySlot].actionInfo   &= 0x0FFF;
		player->curPlaySlot						    = 0xFF;
		player->reqest								= 0x00;
	}

	player->state = RESPAWN;
	
	//����ǉ�
	player->invincibleFrame = 60 * 10;

}
void GameMasterServerA::CombatTank( int playerID )
{
	PlayerInfo*	player = &playerList[playerID];
	player->state = COMBAT;	
}							

void GameMasterServerA::PlaySlot( int id, int slot, BYTE userReqest )
{
	if( gameState != GAME_STATE_TYPE::MUTCH )	return;

	// �X���b�g��I������Ă��Ȃ�
	if( slot > SLOT_NUM )				return;	

	//	�g�p���̃v���C���[�����Ȃ�������( �����ȊO�Ȃ珈�������Ȃ� )
	int player = SearchSlot( slot );
	if( player != -1 && player != id )	
		return;
	else	
		playerList[id].curPlaySlot = slot;

	playerList[id].reqest = userReqest;


	WORD	addActionInfo = 0x0000;

	if(( userReqest & 0xF0 ) > 0 )
		addActionInfo |= IS_ACTIVE;

	if(( userReqest & 0x01 ) > 0 )
		addActionInfo |= IS_STOP_TOP;

	if(( userReqest & 0x02 ) > 0 )
		addActionInfo |= IS_STOP_CENTER;

	if(( userReqest & 0x04 ) > 0 )
		addActionInfo |= IS_STOP_BOTTOM;
	
	// �R�C�������炷����
	WORD curActionAnfo = slotInfo[slot].actionInfo;
	if(( curActionAnfo & IS_ACTIVE) == 0x0000 &&
	   ( addActionInfo & IS_ACTIVE) != 0x0000 )
	{
		const int bet = (int)( userReqest >> 4 );
		playerList[id].betNum = bet;
		AddCoin( id, CoinData::BET_TOTAL, -bet );
	}	
	
	slotInfo[slot].actionInfo |= addActionInfo;
	
}

void GameMasterServerA::TeamBonus( int team, int type, int coinNum )
{
	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		if(playerList[i].team == team)
		{
			AddCoin( i, type, coinNum );
		}
	}
}

void GameMasterServerA::AddCoin(int ID, size_t type, int coinNum )
{
	CoinData& data = playerList[ID].coinData;
	data._m[type]  += coinNum;
}

void GameMasterServerA::SetAssistTank( int shooter, int hitTank )
{
	assistData[shooter].timer[hitTank] = 60*5;	
}

//*****************************************************************************
// ������
//*****************************************************************************
void GameMasterServerA::DestroyRoom()
{
	SAFE_ARRAY_DELETE( playerList );
}
void GameMasterServerA::MutchExit()
{
	gameState	= GAME_STATE_TYPE::GAME_RESULT;
	isGameStart = false;
}
void GameMasterServerA::SystemExit()
{
	SAFE_DELETE(m_pThread);
	SAFE_DELETE( MainFrame );
}

// ���ݎg�p���̃X���b�g����
int GameMasterServerA::SearchSlot( int slotNum )
{
	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		if( playerList[i].curPlaySlot == slotNum )
			return i;
	}
	return -1;
}

int	GameMasterServerA::GetGiveCoinNum( int shooter, int hitTank ) const
{
	if( playerList[shooter].team == playerList[hitTank].team ) return 0;
	// �R�C����V����	
	int defBonus	= 2;														// �f�t�H���g��V
	int inbredBonus	= playerList[hitTank].betNum + playerList[shooter].betNum;	// �X���b�g�x�b�g���ɂ���V[ ������BetNum + �����betNum ]
	int killStreak	= playerList[hitTank].oneLifeRecord;						// �L���X�g���[�N( ����̓|���Ă�����Ԃ̑䐔 )
	int itemBonus	= 0;														//	�A�C�e���ɂ���V
	{																			//	�A�C�e���ɂ���V( �v�Z )
		TankController::ItemEffective item = tank[hitTank]->GetItemEffect();
		if( item.effect == ITEM_EFFECT::ITEM_EFFECT_GET_COIN_UP )
			itemBonus = item.level;
	}
	float confiscatef = (float)GetPlayerCoinNum( hitTank ) / (float)confiscatediv;	//	
	int confiscate = (int)(confiscatef+0.5f);										//	���s�ɂ��R�C���̏��n( ����:(�|�R�C������/2), ����:(�{�R�C������/2),�l�̌ܓ�
		
	return defBonus + inbredBonus + killStreak + itemBonus + confiscate;
}

// ************************************************************
//		�f�o�b�N�p
// ************************************************************
void GameMasterServerA::dbgInfo()
{
	char* gameStateName[]	= {	"WAIT",	"MUTCH","GAME_RESULT",		};
	char* teamName[]		= { "TEAM_A", "TEAM_B", "TEAM_NO" };
	char* tankStateName[]	= { "COMBAT", "DETH", "RESPOWN" };
	char* slotStateName[]	= { "SELECT", "PLAY", "UNION", "NONE" };
	char* booleanName[]		= { "FALSE", "TRUE" };

	const int	stLine	= 96;
	const int	chSize	= 24;
	const char* fontName = "HGS����B";
	const int	strWidth = iexSystem::ScreenWidth;


	int col		=  0;
	IEX_DrawText(" -- [ sceneMutch ] -- ", 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
	++col;
	{	
		char tmp[256] = {0};
		int sec = (int)(endTime - curTime);
		int min = sec / 60;
		int	totalCoin[2] = { 0, 0 };
		for(size_t i = 0; i < PANZER_MAX; i++)
		{
			Global::TEAM_NAME	team		= playerList[i].team;
			int					playerCoin	= GetPlayerCoinNum( i );
			totalCoin[team] += playerCoin;
		}
		const float priority = StageManager::GetOccupyRate();
		Global::TEAM_NAME team = static_cast<Global::TEAM_NAME>(StageManager::GetOccupyTeam());

		sprintf_s( tmp, "[ %s ], [ %02d : %02d ], TEAM_A( %03d ) vs TEAM_B( %03d ),  priority[ %lf_%s ] ",
						gameStateName[gameState], min, sec%60,  totalCoin[0], totalCoin[1], priority, teamName[team] );

		IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
		++col;
	}
	
	IEX_DrawText( " -- [ player ] -- ", 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
	++col;
	for(size_t i = 0; i < 8; i++)
	{
		char tmp[256] = {0};
		Global::TEAM_NAME	team = playerList[i].team;
		PLAYER_STATE state	= GetPlayerState(i);
		const char*	isAI	= ( playerList[i].isAI == true ) ? "true" : "false";
		int p[3];
		tank[i]->GetPartsNum(p);
		char*	isConect = booleanName[playerList[i].isClientConnect];
		sprintf_s( tmp, "%02d : [ %s ( %s ) ], team : %s_%02d, tankState : %7s  coin : %03d, p[ %d, %d, %d ], hp[ %.5lf / %.5lf ], slot[ %d ], reqest[ %X ]",
				i,
				playerList[i].name,				isAI,
				teamName[playerList[i].team],	playerList[i].teamNo,
				tankStateName[ state ],
				GetPlayerCoinNum( i ),
				p[0],p[1],p[2],
				tank[i]->GetHP()-tank[i]->GetDamage(), tank[i]->GetHP(),
				playerList[i].curPlaySlot,
				playerList[i].reqest
				  );

		IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
		++col;
	}

	IEX_DrawText( " -- [ Slot ] -- ", 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
	++col;
	for(size_t i = 0; i < 8; i++)
	{
		char tmp[256] = {0};
		int playerID = SearchSlot( i );
		const char*	playerName = ( playerID != -1 ) ? playerList[playerID].name : "NONE";
		sprintf_s( tmp,"slot %02d : player[ %s ], reelNumber[ %d, %d, %d ], UV[ %.5f,  %.5f,  %.5f ] ,action[ %04X ]",
				i,
				playerName,
				slotInfo[i].reelNo[0],	slotInfo[i].reelNo[1],	slotInfo[i].reelNo[2], 
				slotInfo[i].reelUV[0],	slotInfo[i].reelUV[1],	slotInfo[i].reelUV[2],
				slotInfo[i].actionInfo
		 );

		IEX_DrawText( tmp, 10, stLine + col*chSize, strWidth, chSize, 0xFFFFFF00 );
		++col;
	}
	

}

// �V���O���g��
GameMasterServerA*	GetGameInfo()
{
	static	GameMasterServerA	 instanse;
	return &instanse;
}

// �폜�\��
Vector3				GameMasterServerA::GetFlagPos()
{
	return Vector3( StageManager::GetPennonPos().x, 0.0f, StageManager::GetPennonPos().y );
}
Global::TEAM_NAME	GameMasterServerA::GetOccupyTeam()
{
	int team = StageManager::GetOccupyTeam();
	return static_cast<Global::TEAM_NAME>( team );
}








