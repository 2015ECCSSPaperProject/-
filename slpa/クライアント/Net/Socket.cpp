
#pragma comment( lib, "wsock32.lib" )

#include	<mbstring.h>

#include	"iextreme.h"
#include	"system/system.h"
#include	"sceneMain.h"

#include	"../Manager/GameManager.h"
#include	"../Slot/Coin.h"

#include	"Socket.h"
#include	"UDPclient.h"

#include	"Item.h"

#include	"Tank.h"
#include	"../Object/TankController.h"
#include	"../Slot/Slot.h"

#include	"TankScrapper.h"

#include	"CapsuleWeapon.h"

#include	"Event.h"

#include	"../Object/Player.h"   
 
#include	"Interface.h"

#include	"InputManager.h"
#include	"Effect.h"
#include	"SoundManager.h"

#include	"Shield.h"

#define BYTE_TRUE 0xAA
#define BYTE_FALSE 0xF0
//*****************************************************************************************
//
//	�\�P�b�g�Ǘ�
//
//*****************************************************************************************
SocketManager* SocketManager::m_pInst(0);


//=======================================================================
//	������
//=======================================================================
SocketManager::SocketManager():m_myID(-1)
{	
	for (int i = 0; i < PANZER_MAX; i++)
	{
		m_DeathWait[i] = 0;
	}
	WSADATA	wsaData;
	WSAStartup( MAKEWORD(2,2), &wsaData );
	m_pClient = new UDPClient;
}
bool SocketManager::Init()
{
	m_myName = "\0";
	m_team	 = 0;

	ZeroMemory( m_user,		sizeof(UserData)	* PANZER_MAX );
	ZeroMemory( m_panzer,	sizeof(PanzerData)	* PANZER_MAX );

	BYTE com(NEW_USER);
	m_pClient->Send( (char*)&com, sizeof(BYTE) );


	/*ID�擾*/
	if( m_pClient->Receive( &m_myID, sizeof(int) ) <= 0 )
	{
		closesocket( m_pClient->m_sock );
		m_pClient->m_sock = INVALID_SOCKET;
		return false;
	}

	//	�����t���O������
	for( int i = 0; i < PANZER_MAX; i++ ){
		m_Alive[i]	=	false;
	}

	return true;
}
SocketManager::~SocketManager()
{
	BYTE com( EXIT_USER );
	m_pClient->Send( &com, sizeof(BYTE) );

	delete m_pClient;
}
void SocketManager::SendName()
{
	struct
	{
		BYTE com;
		char name[9];
	} info;

	info.com = INIT_DATA;
	strcpy_s( info.name, m_myName.c_str() );
	m_pClient->Send( (char*)&info, sizeof(info) );
}




//=======================================================================
//	�X�V
//=======================================================================
//---------------------------------------------------------------------
//   �I��
//---------------------------------------------------------------------
void SocketManager::UpdateTeam( int isReady )
{
	if( m_pClient->m_sock == INVALID_SOCKET ) 
		return;
	
	struct DATA
	{
		BYTE com;			
		BYTE row;
		BYTE isReady;
	}data;


	/**/
	data.com	 = TEAM_DATA;
	data.row	 = m_team;
	data.isReady = isReady;
	m_pClient->Send( &data, sizeof(data) );
	
	
	/**/
	UserData receive;

	for( int i=0 ; i<PANZER_MAX ; ++i )
	{	
		if( m_pClient->Receive( (char*)&receive, sizeof( UserData )) <= 0 ) 
			break;

		m_user[i] = receive;
	
	}
}





//===================================================================================
//  ����
//===================================================================================
void SocketManager::InitGame()
{
	BYTE com = INIT_GAME_DATA;
	BYTE receive[2];


	while(1)
	{
		m_pClient->Send( &com, sizeof(com) );

		if( m_pClient->Receive( (char*)&receive, sizeof( receive ) ) <= 0 ) 
			continue;

		//	��������
		if( receive[0] == 0xFF )
			break;
	}
	
//	//VVVV ���ǉ� VVVVVVVVV ������񂪂���Ă���������O��̂ɖ߂��ĉ��ɐV���ɒǉ�

	// �T�[�o����̎�M�f�[�^����ɂ���
	int cnt = 0;
	while( 1 )
	{
		BYTE tmp[2] = { 0x00, 0x00 };
		int read  = m_pClient->Receive( (char*)&tmp, sizeof( tmp ) );
		if( read > 0 )
			++cnt;
		else
			break;
	}
	GameMasterClientA::GameInitialize();
}
void SocketManager::InitUser()
{
	if( m_pClient->m_sock == INVALID_SOCKET ) 
		return;

	/*���M*/
	BYTE com = USER_INIT;
	m_pClient->Send( &com, sizeof(com) );

	/*��M*/
	struct
	{
		BYTE	bodyParts;
		BYTE	headParts;
		BYTE	canonParts;
		BYTE	goldAndInvincible;
		float	centerGrv;
	}receive[PANZER_MAX];
	
	m_pClient->Receive( (char*)&receive, sizeof( receive ));


	for( int i=0 ; i<PANZER_MAX ; ++i )
	{
		m_panzer[i].parts[2]		=	receive[i].bodyParts;
		m_panzer[i].parts[1]		=	receive[i].headParts;
		m_panzer[i].parts[0]		=	receive[i].canonParts;

		m_panzer[i].isGold[0]		=	( receive[i].goldAndInvincible & 1 )? true : false;
		m_panzer[i].isGold[1]		=	( receive[i].goldAndInvincible & 2 )? true : false;
		m_panzer[i].isGold[2]		=	( receive[i].goldAndInvincible & 4 )? true : false;

		m_panzer[i].isInvincible	=	( receive[i].goldAndInvincible & 8 )? true : false;

		m_panzer[i].centerGravitiy	=	receive[i].centerGrv;

		m_Alive[i]					=	true;
	}
}
//������
void SocketManager::UpdateUser()
{	
	if( m_pClient->m_sock == INVALID_SOCKET ) 
		return;

	/*���M�pDES��*/
	struct
	{
		BYTE    com;		//Com��ǉ���������
		BYTE	fire;			//	���C�t���O
		BYTE	boost;			//	�u�[�X�g�t���O
		BYTE	shield;			//	�V�[���h�t���O
		BYTE	acceleMode;		//	�A�N�Z���t���O
		BYTE	rapidAccele;	//	���s�b�h�A�N�Z���t���O
		BYTE	quickTurn;		//	�N�C�b�N�^�[���t���O
		float	bodyRot;		//	�ԑ̂̉�]��
		float	headRot;		//	�C��̉�]��
		float	cannonRot;		//	�C�g�̉�]��

		BYTE	selectSlot;		//	
		BYTE	reqest;
	}send;
	
	TankController::CONTROL_DESC desc;
	desc = tank[ m_myID ]->GetDesc();

	send.com			=	USER_DATA;
	send.fire			=	desc.fire;
	send.boost			=	desc.boost;
	send.shield			=	desc.shield;
	send.acceleMode		=	desc.acceleMode;
	send.rapidAccele	=	desc.rapidAccele;
	send.quickTurn		=	desc.quickTurn;
	send.bodyRot		=	desc.bodyRot;
	send.headRot		=	desc.headRot;
	send.cannonRot		=	desc.cannonRot;
	send.selectSlot		=	GameMasterClientA::GetSelectSlot();
	send.reqest			=	GameMasterClientA::GetReqest();

	m_pClient->Send( &send, sizeof(send) );

	/*��M*/
	struct
	{
		Vector3					pos;
		D3DXQUATERNION			orien;
		float					canonAngle;
		float					headAngle;
		float					energy;
		WORD					damage;
		WORD					coin;
		WORD					slotActionState;	//	�ǋL
		BYTE					fireFlg;
		BYTE					shieldFlag;
		BYTE					boostFlag;
		float					reelUV[3];
		BYTE					prize;
		BYTE					numBet;
		WORD					assistBous;
		WORD					occupyBous;
	}receive[PANZER_MAX];

	m_pClient->Receive( (char*)&receive, sizeof( receive ));


	// ���S���̔�����[SocketManager::ReceiveLog()]�ōs���Ă��܂�
	for( int i=0 ; i<PANZER_MAX ; ++i )
	{	
//			m_DeathWait[i] --;
//	
//			//	���S����i���j
//			if( ( receive[i].damage >=	tank[i]->GetHP() && tank[i]->GetDamage() < tank[i]->GetHP() )
//			||	( receive[i].damage <  tank[i]->GetDamage() ) ){
//				if( m_Alive[i] ){
//					if (m_DeathWait[i] < 0)
//					{
//						m_DeathWait[i] = 60 * 60;
//						//�j�ЃZ�b�g
//						TankScrapper::Set(*tank[i]);
//	
//						//	�G�t�F�N�g�Z�b�g
//						{
//							float	hp			=	tank[i]->GetHP();
//							float	tankScale	=	hp / 600.0f;
//							tankScale	=	1.0f + ( tankScale - 1.0f ) * 0.5f;
//	
//							if( tankScale > 1.5f )	tankScale	=	1.25f;
//							if( tankScale < 0.5f )	tankScale	=	0.75f;
//	
//							//	�����G�t�F�N�g���Z�b�g
//							ExplosionEffect::Set(tank[i]->GetPos(), 0.25f * tankScale, 5, 1.0f);
//						}
//	
//	#ifdef	USE_SOUND_EFFECT
//						//	��������炷
//						{
//							bool	loop = false;
//							float	pitch = 1.0f - (rand() % 10) * 0.001f;
//							float	gain = 5.0f * 20.0f;
//							Vector3	pos = tank[i]->GetPos();
//							Vector3	velocity = Vector3(0.0f, 0.0f, 0.0f);
//							SoundManager::Play("������", loop, pitch, gain, pos, velocity);
//						}
//	#endif
//					}
//	
//					//	�����t���O��܂�
//					m_Alive[i]	=	false;
//				}
//			}
		if( receive[i].damage >= tank[i]->GetHP() )
			tank[i]->Death();
		else
			tank[i]->Born();

		m_panzer[i].pos				= receive[i].pos;
		m_panzer[i].orien			= receive[i].orien;
		m_panzer[i].canonAngle		= receive[i].canonAngle;
		m_panzer[i].headAngle		= receive[i].headAngle;

		m_panzer[i].energy			= receive[i].energy;

		m_panzer[i].IsBoost = receive[i].boostFlag;

		tank[i]->SetFire	( receive[i].fireFlg );
		tank[i]->SetDamage	( receive[i].damage );

		if( receive[i].shieldFlag )	ShieldManager::Activate( i );
		else						ShieldManager::Disactivate( i );

		GameMasterClientA::SetRecivePlayerCoin( i, receive[i].coin );
		GameMasterClientA::SetReciveSlotActionInfo( i, receive[i].slotActionState );
		GameMasterClientA::SetReciveSlotUV( i, 0, receive[i].reelUV[0] );
		GameMasterClientA::SetReciveSlotUV( i, 1, receive[i].reelUV[1] );
		GameMasterClientA::SetReciveSlotUV( i, 2, receive[i].reelUV[2] );
		m_panzer[i].prize			=	receive[i].prize;
		m_panzer[i].numBet			=	receive[i].numBet;
		GameMasterClientA::SetRecivePlayerAssistBonus( i, receive[i].assistBous );
		GameMasterClientA::SetRecivePlayerOccupyBonus( i, receive[i].occupyBous );
	}

	{
		struct
		{
			BYTE					gameState;
			BYTE					tankState;
			float					flagRate;
		}gameData;
		BYTE com = MUTCH_GAME_DATA;
		m_pClient->Send( &com, sizeof(com) );
		if( m_pClient->Receive( (char*)&gameData, sizeof( gameData )) > 0 )
		{
			GameMasterClientA::SetReciveGameState(gameData.gameState);
			GameMasterClientA::SetReciveState( gameData.tankState);
			GameMasterClientA::SetReciveOccupy(gameData.flagRate);
		}
	}
	if( GameMasterClientA::GetMySlotState() == SLOT_STATE::SERVER_ENQUIRY )
	{
		// �T�[�o�[�ɂl���h�c������ł���X���b�g�ԍ����擾
		// ���ꂪ�I���X���b�g�������ꍇ�X���b�g�̑�������̃X�e�b�v��
		BYTE clientSelectSlot	= GameMasterClientA::GetSelectSlot();		// �N���C�A���g�����싖��
		BYTE serverUsingSlot	= 0xFF;										// �T�[�o�[���g�p���Ă���X���b�g
		BYTE com( MUTCH_USE_SLOT );
		m_pClient->Send( &com, sizeof(com) );
		if( m_pClient->Receive( (char*)&serverUsingSlot, sizeof( serverUsingSlot )) == sizeof( serverUsingSlot ) )
		{
			if( serverUsingSlot == 0xFF )
				GameMasterClientA::SetSlotState( SLOT_STATE::SELECT );
			else if( serverUsingSlot == clientSelectSlot )
				GameMasterClientA::SetSlotState( SLOT_STATE::PLAY );
		}
	}


}

struct EventFlag
{
	BYTE com;
	int EventType;
	BYTE IsRun;
};

void SocketManager::UpdateEvent()
{
	if (m_pClient->m_sock == INVALID_SOCKET)
		return;

	//�C�x���g�f�[�^��v��

	/*���M*/
	struct
	{
		BYTE com;
		BYTE IsCountDownRequest;
	}send;
	

	send.com = EVENT_REQUEST_SEND;
	
	send.IsCountDownRequest = EventManager::IsCountDownRequest();

	
	m_pClient->Send(&send, sizeof(send));

	/*��M*/
	
	const int MaxByte = 255;
	BYTE receive[MaxByte];
	if (m_pClient->Receive((char*)&receive, sizeof(receive)) <= 0)
		return;
	if (receive[0] == EVENT_NODATA)
		return;
	int i = receive[0];

	Event* lpEvent = EventManager::GetEvent();

	//com�̓��e�ɂ��ω�
	switch (receive[0])
	{
	case EVENT_FLAG_RECEIVE:
		//�C�x���g�ԍ�
		EventManager::ReceiveEventFlag((char*)&receive);
		break;

	case EVENT_BASE_DATA_RECEIVE:
		//�y��f�[�^
		if (lpEvent)
			lpEvent->ReceiveBaseData((char*)&receive);
		else
			return;
		break;

	case EVENT_OBJECT_DATA_RECEIVE:
		if (lpEvent)
			lpEvent->ReceiveObjectData((char*)&receive);
		else
			return;
		//�I�u�W�F�N�g�f�[�^
		break;

	case EVENT_COUNTDOWN_FLAG_RECEIVE:
		EventManager::ReceiveCountDownFlag();
		//�J�E���g�_�E���t���O
		break;

	case EVENT_CURRENT_DATA_RECEIVE:
		EventManager::ReceiveCountDownFlag();
		if (lpEvent)
			lpEvent->ReceiveCurrentData((char*)&receive);
		else
			return;
		//�I�u�W�F�N�g�f�[�^
		break;

	}
	/*����Ԃ�*/
	switch (receive[0])
	{
	case EVENT_FLAG_RECEIVE:
	case EVENT_BASE_DATA_RECEIVE:
	case EVENT_OBJECT_DATA_RECEIVE:
		m_pClient->Send(&receive, MaxByte);
		break;
	default:
		break;
	}
}

void SocketManager::SendChat( const std::string& str )
{
	if( m_pClient->m_sock == INVALID_SOCKET ) 
		return;


	//���M
	struct
	{
		BYTE    com;
		char    message[MAX_CHAT];
	}send;

	send.com = CHAT_DATA_SEND;

	//	���M���郁�b�Z�[�W���i�[	
	{
		//	������������𒴂��Ă���ꍇ�A�������������̈�ԍŌ�̂P�����������āA�I�[����������
		if( str.size() > MAX_CHAT - 1 ){
			//	�������������̍Ō�̕����̃o�C�g���𒲂ׂ�
			int	lastCharaByte	=	0;
			int	byteCursor		=	0;

			//	�����������𒴂���܂Ń��[�v
			while( true ){
				//	�o�C�g���𒲂ׂ�
				lastCharaByte	=	_mbclen( ( const unsigned char* )( &str.c_str()[ byteCursor ] ) );

				//	�I������
				if( byteCursor + lastCharaByte >= MAX_CHAT )	break;
				//	�J�[�\�������炷
				else											byteCursor	+=	lastCharaByte;
			}

			//	�������R�s�[
			int	copyBytes		=	byteCursor;
			for( int i = 0; i < copyBytes; i++ ){
				send.message[i]	=	str.c_str()[i];
			}

			//	�I�[����������
			send.message[ byteCursor ]	=	'\0';
		}
		//	���������������Ɏ��܂�ꍇ�͂��̂܂܃R�s�[
		else{
			strcpy_s( send.message, str.c_str() );
		}
	}

	m_pClient->Send( &send, sizeof(send) );
}
void SocketManager::ReceiveChat()
{
	BYTE com(CHAT_DATA_RECEIVE);
	m_pClient->Send( &com, sizeof(com) );

	//	��M
	struct
	{
		BYTE    flg;				//	0���� 1�L��
		BYTE    clientID;
		char    message[MAX_CHAT];
	}receive;

	m_pClient->Receive( &receive, sizeof(receive) );

	//	�󂯎����
	if( receive.flg == 1 )
	{
		const	int				id				=	receive.clientID;
		const	bool			isRed			=	GameMasterClientA::GetPlayerTeam( id ) != GameMasterClientA::GetMyTeam();

		const	LPSTR			playerName		=	m_user[ id ].name;
		const	DWORD			playerColor		=	( isRed )?	0xFFff4040	:	0xFF4040ff;
		const	std::string&	content			=	receive.message;
		const	DWORD			contentColor	=	playerColor;

		ChatLog::SetLog( playerName, playerColor, content, contentColor );
	}
}
void SocketManager::ReceiveLog()
{
	BYTE com( LOG_DATA_RECEIVE );
	m_pClient->Send( &com, sizeof(com) );

	struct
	{
		BYTE    flg;
		BYTE    killer;
		BYTE    slayer;
		BYTE	giveCoinNum;	//	�|�����l���擾�����R�C����
	}receive;

	m_pClient->Receive( &receive, sizeof(receive) );


	//	�󂯎����
	if( receive.flg == 1 )
	{
		//	�L�����O�Z�b�g
		int			playerID		=	GameMasterClientA::GetMyID();
		LPCSTR		myName			=	GameMasterClientA::GetPlayerName( receive.slayer );
		LPCSTR		shooterName		=	GameMasterClientA::GetPlayerName( receive.killer );

		D3DXCOLOR	myColor
			=	( GameMasterClientA::GetPlayerTeam( receive.slayer ) != GameMasterClientA::GetMyTeam() )?
				0xAFff0000	:	0xAF0000ff;
		D3DXCOLOR	shooterColor
			=	( GameMasterClientA::GetPlayerTeam( receive.killer ) != GameMasterClientA::GetMyTeam() )?
				0xAFff0000	:	0xAF0000ff;

		//	�����͉��F��
		if( receive.slayer == playerID )	myColor			=	0xAFff8f00;
		if( receive.killer == playerID )	shooterColor	=	0xAFff8f00;
		
		//	�����Ɠ����Ȃ�M�~�b�N�ɎE���ꂽ
		if( receive.slayer == receive.killer )	GameLog::SetKillLog( "�M�~�b�N", myName, 0xAF00ff00, myColor );
		else									GameLog::SetKillLog( shooterName, myName, shooterColor, myColor );


		PlayerEffectCoin::AddDeathEffect( receive.killer, receive.slayer, receive.giveCoinNum );

		//	���������S�����ꍇ�͓|�����G�����b�N�I���i�L���J�����p�j
		if( receive.slayer == playerID ){
			tank[ playerID ]->SetLockOnTank( receive.killer );
		}

		//�j�ЃZ�b�g
		TankScrapper::Set(receive.slayer);

		//	�G�t�F�N�g�Z�b�g
		{
			float	hp			=	tank[receive.slayer]->GetHP();
			float	tankScale	=	hp / 600.0f;
			tankScale	=	1.0f + ( tankScale - 1.0f ) * 0.5f;

			if( tankScale > 1.5f )	tankScale	=	1.25f;
			if( tankScale < 0.5f )	tankScale	=	0.75f;

			//	�����G�t�F�N�g���Z�b�g
			ExplosionEffect::Set(tank[receive.slayer]->GetPos(), 0.25f * tankScale, 5, 1.0f);
		}

#ifdef	USE_SOUND_EFFECT
		//	��������炷
		{
			bool	loop = false;
			float	pitch = 1.0f - (rand() % 10) * 0.001f;
			float	gain = 5.0f * 20.0f;
			Vector3	pos = tank[receive.slayer]->GetPos();
			Vector3	velocity = Vector3(0.0f, 0.0f, 0.0f);
			SoundManager::Play("������", loop, pitch, gain, pos, velocity);
		}
#endif
	}

}

// ���ǉ�
void SocketManager::SendGameEndFlag()
{
	BYTE com( MUTCH_GAME_END );
	m_pClient->Send( &com, sizeof(com) );
	
	enum { TOTAL_KILL, TOTAL_DEATH };
	struct 
	{
		u16 result[PANZER_MAX][2];
	} receive;
	while( m_pClient->Receive( (char*)&receive, sizeof( receive ) ) != sizeof( receive ) );
	
	GameMasterClientA::SetReciveResult( receive.result );

}

//---------------------------------------------------------------------
//  item
//---------------------------------------------------------------------
void SocketManager::ReceiveGetItemInfo()
{
	//	���M
	BYTE com( ITEM_GET_INFO_RECEIVE );
	m_pClient->Send( &com, sizeof(com) );

	//	��M
	struct
	{	
		BYTE	is;
		u32		respawn;
		SHORT	slotUV[3];
	}receive;

	m_pClient->Receive( &receive, sizeof(receive) );

	//	�A�C�e���Q�b�g
	if( receive.is )
	{
		ItemManager::ItemStart( receive.slotUV );
	}

	ItemManager::m_itemExistPoint = receive.respawn;
}
void SocketManager::ReceiveItem()
{
	//	���M
	BYTE com( ITEM_DATA_RECEIVE );
	m_pClient->Send( &com, sizeof( com ));

	//	��M
	struct
	{	
		BYTE effect;
		BYTE level;
	}receive[PANZER_MAX];
		
	//	�S�����󂯎��
	m_pClient->Receive( &receive, sizeof(receive) );
	
	//	�󂯎�����f�[�^���i�[
	for( int i=0; i<PANZER_MAX; ++i )
		tank[ i ]->SetItemEffect( (ITEM_EFFECT)receive[i].effect, receive[i].level );
}
