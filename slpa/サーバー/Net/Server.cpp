#include<winsock2.h>
#pragma comment( lib, "wsock32.lib" )

#include<time.h>

#include"iextreme.h"
#include"../system/Framework.h"

#include"Server.h"
#include"UDPServer.h"

#include"scenemain.h"

#include	"Item.h"

#include"Tank.h"
#include"TankController.h"

#include"Player.h"

#include"../Manager/GameManager.h"
#include"../Slot/Slot.h"

#include "Event.h"

#include"Interface.h"

#include	"Shield.h"

#define BYTE_TRUE 0xAA
#define BYTE_FALSE 0xF0

//*****************************************************************************************
//
//	UDP�T�[�o�[
//
//*****************************************************************************************
ServerManager::RoomData			ServerManager::m_room;
UDPServer*						ServerManager::m_pServer(0);
std::list<ServerManager::Chat>	ServerManager::m_chat;
std::list<ServerManager::Log>	ServerManager::m_log;



//=======================================================================
//	������
//=======================================================================
void ServerManager::Init()
{
	WSADATA	wsaData;
	WSAStartup( MAKEWORD(2,2), &wsaData );
	m_pServer = new UDPServer();

	
	for ( int i = 0; i <PANZER_MAX ; ++i )
	{
		m_room.user[i].com		= 0;
		m_room.user[i].col		= 0xFF;
		m_room.user[i].row		= 0xFF;
		m_room.user[i].isReady	= 0;
		strcpy_s( m_room.user[i].name, "\0" );
	}	
	
	ZeroMemory( m_room.desc, sizeof(ContorlDesc)*PANZER_MAX );
}
void ServerManager::Release()
{
	delete m_pServer;
}
void ServerManager::Update()
{
	{
		/*�`���b�g���X�V*/
		std::list<Chat>::iterator it( m_chat.begin());
		while( it != m_chat.end() )
		{
			/*�S���ɑ��M�ς݂Ȃ�*/
			if ( it->sendFlg == 0xFF )
				it = m_chat.erase( it );
			else
				++it;
		}
	}
	{
		/*�Q�[�����O�X�V*/
		std::list<Log>::iterator it( m_log.begin());
		while( it != m_log.end() )
		{
			/*�S���ɑ��M�ς݂Ȃ�*/
			if ( it->sendFlg == 0xFF )
				it = m_log.erase( it );
			else
				++it;
		}
	}
	
	

	/*�Q�[���X�V*/
	char data[512];
	int	size = sizeof(data);
	int client = m_pServer->Receive( data, &size );

	if( client == -1 ) 
		return;

	switch( data[0] )
	{
	case NEW_USER:
		NewUser(data,client);
		break;

	case INIT_DATA:		
		InitData(data,client);
		break;

	case TEAM_DATA:		
		TeamData(data,client);
		break;

	case INIT_GAME_DATA:
		GameInitData(data,client);
		break;

	case USER_INIT:
		InitUser(data,client);
		break;

	case USER_DATA:
		UpdateUser(data,client);
		break;

	case CHAT_DATA_SEND:
		SendChat(data,client);
		break;

	case CHAT_DATA_RECEIVE:
		ReceiveChat(data,client);
		break;

	case LOG_DATA_SEND:
		SendGameLog(data,client);
		break;
	
	case ITEM_GET_INFO_SEND:
		SendItemInfo(data,client);
		break;

	case ITEM_DATA_SEND:
		SendItem(data,client);
		break;

	case EXIT_USER:
		ExitUser(data,client);
		break;

	// ���ǉ�
	case MUTCH_GAME_DATA:
		MutchGameData( data, client );
		break;
	case MUTCH_TEAM_DATA:
		MutchTeamData( data, client );
		break;
	case MUTCH_GAME_END:
		MutchGameEnd( data, client );
		break;

	case MUTCH_USE_SLOT:
		MutchUsingSlotPlayer( data, client );
		break;

	case MUTCH_COIN_DATA:
		MutchCoinData( data, client );
		break;

	//	����ǉ�
	case EVENT_REQUEST_RECEIVE:
		//SendEventData(data, client);
		break;

		//	����ǉ�
	case EVENT_FLAG_SEND:
		//ReturnEventFlag(data, client);
		break;

		//	����ǉ�
	case EVENT_BASE_DATA_SEND:
		//ReturnEventBaseData(data, client);
		break;

		//	����ǉ�
	case EVENT_OBJECT_DATA_SEND:
		//RetunEventObjectData(data, client);
		break;

	}
}
	


//=======================================================================
//		�f�[�^����M
//=======================================================================
void ServerManager::NewUser( char* data, int client)
{	
	if( client != -1 )
	{
		/*�lID���M*/
		m_pServer->Send( client, (char*)&client, sizeof(int) );
		Sleep( 100 );
		printf( "�ڑ�:%d\n", client );
	}
}
void ServerManager::InitData( char* data, int client)
{		
	CopyMemory( m_room.user[client].name, &data[1], 9 );
	printf( "�Q��:%s\n", m_room.user[client].name, client );
}



//---------------------------------------------------------------------
//   team
//---------------------------------------------------------------------
void ServerManager::TeamData( char* data, int client)
{		
	struct DATA
	{
		BYTE com;			
		BYTE row;
		BYTE isReady;
	};

	DATA* d(( DATA*)data );


	/*���O�`��ʒu����*/
	if( m_room.user[client].row != d->row )
	{	
		/*�󂫂𖄂߂�*/
		for( int i=0 ; i<PANZER_MAX ; ++i )
		{
			if( i == client )continue;
			if( m_room.user[i].com == UserData::PASSIVE_USER )continue;

			if( m_room.user[client].row == m_room.user[i].row && m_room.user[client].col < m_room.user[i].col )
				m_room.user[i].col--;
		}

		m_room.user[client].row = d->row;

		/*���ɂ���*/
		int col(0);
		for( int i=0 ; i<PANZER_MAX ; ++i )
		{
			if( i == client )continue;
			if( m_room.user[i].com == UserData::PASSIVE_USER )continue;
			if( m_room.user[client].row == m_room.user[i].row )
			++col;
		}
		
		m_room.user[client].col = col;
	}

	/*active�ɂ���*/
	m_room.user[client].com		= UserData::ACTIVE_USER;
	m_room.user[client].isReady	= d->isReady;


	/*�S�����������H*/
	int count(0);
	int active(0);
	for( int i=0 ; i<PANZER_MAX ; ++i )
	{
		if( m_room.user[i].com == UserData::ACTIVE_USER )
			++active;

		if( m_room.user[i].isReady == UserData::READY_MUTCH || 
			m_room.user[i].isReady == UserData::READY_MUTCH_ALL )
			++count;
	}


	/*�J�n�t���Oon*/
	if( active == count )
	{
		m_room.user[client].isReady = UserData::READY_MUTCH_ALL;

	}

	/*���M*/
	for( int i=0 ; i<PANZER_MAX ; ++i )
	{
		UserData data;
		data.com		= m_room.user[i].com;	
		data.row		= m_room.user[i].row;
		data.col		= m_room.user[i].col;
		data.isReady	= m_room.user[i].isReady;
		CopyMemory( data.name, m_room.user[i].name, 9 );

		m_pServer->Send( client, (char*)&data, sizeof( UserData ));
	}
}



//---------------------------------------------------------------------
//   game
//---------------------------------------------------------------------
void ServerManager::GameInitData( char* data, int client )
{
	/*����*/
	m_room.user[client].isReady = UserData::READY_MUTCH_SYNC;


	/*�S������������*/
	int count(0);
	int active(0);
	for( int i=0 ; i<PANZER_MAX ; ++i )
	{

		/*�l��?*/
		if( m_room.user[i].com == UserData::ACTIVE_USER )
			++active;


		/*�ǂݍ��ݏI���H*/
		if( m_room.user[i].isReady == UserData::READY_MUTCH_SYNC )
			++count;
	}

	/*�J�n�t���Oon*/
	struct
	{
		BYTE com;
		BYTE dm;
	}send;

	//	�Q�[��������
	if( count == active )
	{
		GAME_MASTER->GameInitialize();
	}

	//	��������
	send.com = active == count? 0xFF : 0;
	m_pServer->Send( client, (char*)&send, sizeof(send) );
}
void ServerManager::InitUser( char* data, int client )
{
	/*���M*/
	struct
	{
		BYTE	bodyParts;
		BYTE	headParts;
		BYTE	canonParts;
		BYTE	goldAndInvincible;
		float	centerGrv;
	}send[PANZER_MAX];

	for( int i=0; i<PANZER_MAX; ++i )
	{ 
		int tmp[ 3 ];
		tank[ i ]->GetPartsNum( tmp );
		send[i].canonParts		=	tmp[ 0 ];
		send[i].headParts		=	tmp[ 1 ];
		send[i].bodyParts		=	tmp[ 2 ];

		BYTE	cannonIsGold	=	( tank[i]->CannonIsGold() )?				1	:	0;
		BYTE	bodyIsGold		=	( tank[i]->BodyIsGold() )?					2	:	0;
		BYTE	legIsGold		=	( tank[i]->LegIsGold() )?					4	:	0;
		BYTE	isInvincible	=	( GAME_MASTER->IsInvincibleTank( i ) )?		8	:	0;
		
		send[i].goldAndInvincible	=	cannonIsGold | bodyIsGold | legIsGold | isInvincible;
		send[i].centerGrv			=	tank[ i ]->GetCenterGravity();
	}		
	
	m_pServer->Send( client, (char*)&send, sizeof(send) );
}
void ServerManager::UpdateUser( char* data, int client )
{
	ContorlDesc* receive =( ContorlDesc* )data;

	//	��M
	m_room.desc[client]		= *receive;
	m_room.fireFlg[client]	= receive->fire;	//	�ۑ�
	m_room.receiveFlg		= 1 << client;		//	


	GAME_MASTER->ConnectClient( client );
	switch(GAME_MASTER->GetPlayerState(client))
	{
	case DEATH:
		GAME_MASTER->PlaySlot( client, receive->selectSlot, receive->reqest );
		break;
	default:
		break;
	}



	//	���M
	struct
	{
		Vector3					pos;
		D3DXQUATERNION			orien;
		float					canonAngle;
		float					headAngle;
		float					energy;
		WORD					damage;
		WORD					coin;
		WORD					slotActionState;	
		BYTE					fireFlg;
		BYTE					shieldFlag;
		BYTE					boostFlag;
		float					reelUV[3];
		BYTE					prize;
		BYTE					numBet;
		WORD					assistBous;
		WORD					occupyBous;
	}send[PANZER_MAX];


	for( int i=0; i<PANZER_MAX; ++i )
	{
		send[i].orien				= tank[i]->GetOrien();
		send[i].pos					= tank[i]->GetPos();
		send[i].canonAngle			= tank[i]->GetCannonAngle();
		send[i].headAngle			= tank[i]->GetHeadAngle();

		send[i].energy				= tank[i]->GetEnergy();
			
		send[i].damage				= (WORD)tank[i]->GetDamage();
		send[i].fireFlg				= tank[i]->GetFire()? 1 : 0;
		send[i].shieldFlag			= ShieldManager::IsActive( i );
		send[i].boostFlag			= tank[i]->GetBoost() ? 1 : 0;
			
		//	[i]���ǉ�
		send[i].coin				= (WORD)GAME_MASTER->GetPlayerCoinNum( i );
		send[i].slotActionState		= (WORD)GAME_MASTER->GetSlotActionFlag( i );
		send[i].reelUV[0]			= GAME_MASTER->GetSlotUV( i, 0 );
		send[i].reelUV[1]			= GAME_MASTER->GetSlotUV( i, 1 );
		send[i].reelUV[2]			= GAME_MASTER->GetSlotUV( i, 2 );
			
		send[i].prize				= GAME_MASTER->GetOneLifeRecord( i );
		send[i].numBet				= GAME_MASTER->GetNumBet( i );
		const CoinData& coinData = GAME_MASTER->GetPlayerCoinData(i);
		send[i].assistBous			= coinData._m[CoinData::ASSIST_BONUS];
		send[i].occupyBous			= coinData._m[CoinData::OCCUPY_BONUS];
	}		

	m_pServer->Send( client, (char*)&send, sizeof(send) );
}
void ServerManager::SendChat( char* data, int client )
{
	//	��Ȃ��ʒm
	if( m_chat.empty())
	{
		BYTE flg(0);
		m_pServer->Send( client, (char*)&flg, sizeof(flg) );
		return;
	}


	/*���M*/
	Chat* pChat = &m_chat.front();

	/*���M�ς݂��ǂ���*/
	if( ( pChat->sendFlg & ( 1 << client )) == 0 )
	{
		/*���M�ς݂ɂ���*/
		pChat->sendFlg |= 1 << client;

		struct
		{
			BYTE    flg;				//	0���� 1�L��
			BYTE    clientID;			//
			char    message[CHAT_MAX];	//
		}send;


		send.flg = 1;
		send.clientID = pChat->clientID;

		for( int i=0; i<CHAT_MAX; i++ )
			send.message[i] = pChat->message[i];

		m_pServer->Send( client, (char*)&send, sizeof(send) );
	}
	else
	{
		BYTE flg(0);
		m_pServer->Send( client, (char*)&flg, sizeof(flg) );
	}
}
void ServerManager::ReceiveChat( char* data, int client )
{
	struct Data
	{
		BYTE    com;
		char    message[CHAT_MAX];
	};
		
	Data* receive =( Data*)data;
		
	//	�󂯎�����f�[�^����ʂɕ\��
	{
		const	int				playerID		=	client;
		const	bool			isRed			=	GAME_MASTER->GetPlayerTeam( playerID ) == Global::TEAM_NAME::TEAM_A;
		
		const	LPSTR			playerName		=	m_room.user[ playerID ].name;
		const	DWORD			playerColor		=	( isRed )?	0xFFff4040	:	0xFF4040ff;
		const	std::string&	content			=	receive->message;
		const	DWORD			contentColor	=	playerColor;
		
		ChatLog::SetLog( playerName, playerColor, content, contentColor );
	}

	//	���ۑ�
	{
		Chat chat;
		chat.sendFlg	= 1 << client;
		chat.clientID	= client;


		//	AI�Ȃ瑗�M���Ȃ��̂ōς݃t���O�𗧂Ă�
		for( int i=0; i<PANZER_MAX; ++i )
		{
			if( m_room.user[i].com != UserData::ACTIVE_USER )	
				chat.sendFlg |= 1 << i;
		}


		//	
		for( int i=0; i<CHAT_MAX; i++)
			chat.message[i] = receive->message[i];

		m_chat.push_back(chat);
	}
}


//	���ǉ�
void ServerManager::MutchTeamData( char* data, int client )
{
	/*���M*/
	struct
	{
		BYTE teamList[PANZER_MAX];			//	0x00  TEAM_A, 0x10 TEAM_B,  0x0F teamNo
		BYTE teamListA[PANZER_MAX];			//	0x00  TEAM_A, 0x10 TEAM_B,  0x0F teamNo
	}send;
	for( int i=0; i<PANZER_MAX; ++i )
	{
		send.teamList[ i ] = GAME_MASTER->GetPlayerTeam( i );
		send.teamListA[ i ] = GAME_MASTER->GetPlayerTeamNo( i );
	}
	m_pServer->Send( client, (char*)&send, sizeof(send) );
}
void ServerManager::MutchGameData( char* data, int client )
{
	struct
	{
		BYTE					gameState;
		BYTE					tankState;
		float					flagRate;
	}send;
	
	send.gameState	= GAME_MASTER->GetGameState();
	send.tankState	= GAME_MASTER->GetPlayerState(client);
	send.flagRate	= GAME_MASTER->GetOccupyRate();

	m_pServer->Send( client, (char*)&send, sizeof(send) );
}
void ServerManager::MutchGameEnd( char* data, int client )
{
	GAME_MASTER->DisConnectClient( client );

	enum { TOTAL_KILL, TOTAL_DEATH };
	struct 
	{
		u16 result[PANZER_MAX][2];
	} send;
	for(size_t i = 0; i < PANZER_MAX; i++)
	{
		send.result[i][TOTAL_KILL]	= GAME_MASTER->GetPlayerTotalKill( i );
		send.result[i][TOTAL_DEATH] = GAME_MASTER->GetPlayerTotalDeath( i );
	}

	m_pServer->Send( client, (char*)&send, sizeof(send) );
	
}
void ServerManager::MutchUsingSlotPlayer( char* data, int client )
{

	BYTE playerUseSlot = GAME_MASTER->GetCurPlaySlot( client );

	m_pServer->Send( client, (char*)&playerUseSlot, sizeof(playerUseSlot) );
}
void ServerManager::MutchCoinData( char* data, int client )
{
	SHORT  send[PANZER_MAX][CoinData::BONUS_VALUE] = { 0 };
	for(size_t id = 0; id < PANZER_MAX; id++)
	{
		const CoinData& coinData = GAME_MASTER->GetPlayerCoinData(id);
		for(size_t c = 0; c < CoinData::BONUS_VALUE; c++)
			send[id][c] = coinData._m[c];
	}
	m_pServer->Send( client, (char*)&send, sizeof(send) );
}

struct EventFlag
{
	BYTE com;
	long EventType;
	BYTE IsRun;
};
//	����ǉ�
void ServerManager::SendEventData(char* data, int client)
{
	if (EventManager::IsDataSendComplete(client) && EventManager::GetEventState() != EVENT_STATE::EVENT_RUNNING)
	{
		BYTE send = EVENT_NODATA;
		m_pServer->Send(client, (char*)&send, sizeof(send));
		return;
	}
	/*���M*/
	switch (EventManager::GetEventState())
	{
	case EVENT_STATE::NONE_STATE:
	case EVENT_STATE::EVENT_FLAG_SEND:
	{
		EventFlag send;
		send.com = EVENT_FLAG_SEND;
		send.EventType = EventManager::GetEventType();
		if (EventManager::GetIsRun())
			send.IsRun = BYTE_TRUE;
		else
			send.IsRun = BYTE_FALSE;

		m_pServer->Send(client, (char*)&send, sizeof(send));
		EventManager::SetSendData((char*)&send, sizeof(send), client);
	}
	break;
	
	case EVENT_STATE::BASE_DATA_SEND:
	{
		char* data = NULL;
		int size;
		//�T�[�o�[���̃C�x���g��Ԃɂ��ω�
		EventManager::GetEvent()->GetBaseData(EVENT_BASE_DATA_SEND ,data, &size);
		m_pServer->Send(client, data, size);
		EventManager::SetSendData(data, size, client);
		delete[] data;

	}
	break;

	case EVENT_STATE::OBJECT_DATA_SEND:
	{
		if (EventManager::IsDataSendComplete(client))
		{
			char send = EVENT_NODATA;
			m_pServer->Send(client, (char*)&send, sizeof(send));
			break;
		}

		char* data = NULL;
		int size;
		EventManager::GetEvent()->GetObjectData(EVENT_OBJECT_DATA_SEND,data, &size);
		m_pServer->Send(client, data, size);
		EventManager::SetSendData(data, size , client);
		delete[] data;
	}
	break;

	case EVENT_STATE::WAIT_COUNTDOWN_REQUEST:
	{
		struct DATA
		{
			BYTE com;
			BYTE IsCountDownRequest;
		}*convert;
		convert = (DATA*)data;
		if (convert->IsCountDownRequest == BYTE_TRUE)
			EventManager::SetCountDownRequest(client);
		char send = EVENT_NODATA;
		m_pServer->Send(client, (char*)&send, sizeof(send));
	}	
	break;

	case EVENT_STATE::COUNTDOWN_FLAG_SEND:
	{
		char send = EVENT_COUNTDOWN_FLAG_SEND;
		m_pServer->Send(client, (char*)&send, sizeof(send));
	}
	break;


	case EVENT_STATE::EVENT_RUNNING:
	{
		char* data = NULL;
		int size;
		//�T�[�o�[���̃C�x���g��Ԃɂ��ω�
		EventManager::GetEvent()->GetCurrentData(EVENT_CURRENT_DATA_SEND,data, &size);
		m_pServer->Send(client, data, size);
		delete[] data;

	}
	break;

	}

}


//	����ǉ�
void ServerManager::ReturnEventFlag(char* data, int client)
{
	EventManager::ReturnEventFlag(data, client);
}

//	����ǉ�
void ServerManager::ReturnEventBaseData(char* data, int client)
{
	EventManager::ReturnBaseData(data, client);
}

//	����ǉ�
void ServerManager::RetunEventObjectData(char* data, int client)
{
	EventManager::ReturnObjectData(data, client);
}



void ServerManager::SetGameLog( u32 killer, u32 slayer )
{
	Log log;
	log.sendFlg		= 0;
	log.killer		= killer;
	log.slayer		= slayer;
	log.giveCoinNum = GAME_MASTER->GetGiveCoinNum( killer, slayer );

	//	AI�Ȃ瑗�M���Ȃ��̂ōς݃t���O�𗧂Ă�
	for( int i=0; i<PANZER_MAX; ++i )
	{
		if( m_room.user[i].com != UserData::ACTIVE_USER )	
			log.sendFlg |= 1 << i;
	}

	//	�ǉ�
	m_log.push_back( log );
}
void ServerManager::SendGameLog( char* data, int client )
{
	//	��Ȃ��ʒm
	if( m_log.empty())
	{
		BYTE flg(0);
		m_pServer->Send( client, (char*)&flg, sizeof(flg) );
		return;
	}


	//	���M
	Log* pLog = &m_log.front();
	if( ( pLog->sendFlg & ( 1 << client )) == 0 )
	{
		//	���M�ς݂ɂ���
		pLog->sendFlg |= 1 << client;

		struct
		{
			BYTE    flg;
			BYTE    killer;
			BYTE    slayer;
			BYTE	giveCoinNum;
		}send;

		send.flg			= 1;
		send.killer			= (BYTE)pLog->killer;
		send.slayer			= (BYTE)pLog->slayer;
		send.giveCoinNum	= (BYTE)pLog->giveCoinNum;
		m_pServer->Send( client, (char*)&send, sizeof(send) );
	}
	else
	{
		BYTE flg(0);
		m_pServer->Send( client, (char*)&flg, sizeof(flg) );
	}
}


//---------------------------------------------------------------------
//  item
//---------------------------------------------------------------------
void ServerManager::SendItemInfo	( char* data, int client )
{	
	//	���[����]���H
	if( ItemManager::GetIsReelRot( client ) )
	{
		struct
		{	
			BYTE	is;
			u32		respawn;
			SHORT	slotUV[3];
		}itemEffect;

		//	�����t���O
		itemEffect.is = 1;

		//	�o���ʒu
		itemEffect.respawn = ItemManager::GetItemExistPoint();

		//	UV���~�߂��
		u32* reel = ItemManager::GetReelUV( client );
		for( int i=0; i<3; ++i )
			itemEffect.slotUV[i] = (SHORT)reel[i];

		//	�󋵂̑��M
		m_pServer->Send( client, (char*)&itemEffect, sizeof(itemEffect));
	}
	else
	{
		struct
		{	
			BYTE	is;
			u32		respawn;
		}itemEffect;

		itemEffect.is		= 0;
		itemEffect.respawn	= ItemManager::GetItemExistPoint();

		//	�󋵂̑��M
		m_pServer->Send( client, (char*)&itemEffect, sizeof(itemEffect));
	}
}
void ServerManager::SendItem( char* data, int client )
{
	//	���݂̃A�C�e���󋵎擾
	struct
	{	
		BYTE effect;
		BYTE level;
	}itemEffect[PANZER_MAX];
	

	//	�S�����̌��ʂ𑗂�
	for( int i=0; i<PANZER_MAX; ++i )
	{
		TankController::ItemEffective e = tank[i]->GetItemEffect();

		itemEffect[i].effect	= e.effect;
		itemEffect[i].level		= e.level;
	}

	//	�󋵂̑��M
	m_pServer->Send( client, (char*)&itemEffect, sizeof(itemEffect));
}


//---------------------------------------------------------------------
//   exit
//---------------------------------------------------------------------
void ServerManager::ExitUser( char* data, int client )
{	
	printf( "�E��:%d\n", client );	

	m_room.user[client].com		= 0;
	m_room.user[client].col		= 0xFF;
	m_room.user[client].row		= 0xFF;
	m_room.user[client].isReady	= 0;
	strcpy_s( m_room.user[client].name, "\0" );

	//	�\�P�b�g�����
	m_pServer->CloseClient( client );

	//�Q�[������E��
	GAME_MASTER->DisConnectClient( client );
}

