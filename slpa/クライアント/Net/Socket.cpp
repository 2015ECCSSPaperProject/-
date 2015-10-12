
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
//	ソケット管理
//
//*****************************************************************************************
SocketManager* SocketManager::m_pInst(0);


//=======================================================================
//	初期化
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


	/*ID取得*/
	if( m_pClient->Receive( &m_myID, sizeof(int) ) <= 0 )
	{
		closesocket( m_pClient->m_sock );
		m_pClient->m_sock = INVALID_SOCKET;
		return false;
	}

	//	生存フラグ初期化
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
//	更新
//=======================================================================
//---------------------------------------------------------------------
//   選択中
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
//  試合
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

		//	同期完了
		if( receive[0] == 0xFF )
			break;
	}
	
//	//VVVV 沖追加 VVVVVVVVV 川口くんがやっていた同期を前回のに戻して下に新たに追加

	// サーバからの受信データを空にする
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

	/*送信*/
	BYTE com = USER_INIT;
	m_pClient->Send( &com, sizeof(com) );

	/*受信*/
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
//★★★
void SocketManager::UpdateUser()
{	
	if( m_pClient->m_sock == INVALID_SOCKET ) 
		return;

	/*送信用DESｋ*/
	struct
	{
		BYTE    com;		//Comを追加しただけ
		BYTE	fire;			//	発砲フラグ
		BYTE	boost;			//	ブーストフラグ
		BYTE	shield;			//	シールドフラグ
		BYTE	acceleMode;		//	アクセルフラグ
		BYTE	rapidAccele;	//	ラピッドアクセルフラグ
		BYTE	quickTurn;		//	クイックターンフラグ
		float	bodyRot;		//	車体の回転量
		float	headRot;		//	砲台の回転量
		float	cannonRot;		//	砲身の回転量

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

	/*受信*/
	struct
	{
		Vector3					pos;
		D3DXQUATERNION			orien;
		float					canonAngle;
		float					headAngle;
		float					energy;
		WORD					damage;
		WORD					coin;
		WORD					slotActionState;	//	追記
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


	// 死亡時の爆発は[SocketManager::ReceiveLog()]で行っています
	for( int i=0 ; i<PANZER_MAX ; ++i )
	{	
//			m_DeathWait[i] --;
//	
//			//	死亡判定（仮）
//			if( ( receive[i].damage >=	tank[i]->GetHP() && tank[i]->GetDamage() < tank[i]->GetHP() )
//			||	( receive[i].damage <  tank[i]->GetDamage() ) ){
//				if( m_Alive[i] ){
//					if (m_DeathWait[i] < 0)
//					{
//						m_DeathWait[i] = 60 * 60;
//						//破片セット
//						TankScrapper::Set(*tank[i]);
//	
//						//	エフェクトセット
//						{
//							float	hp			=	tank[i]->GetHP();
//							float	tankScale	=	hp / 600.0f;
//							tankScale	=	1.0f + ( tankScale - 1.0f ) * 0.5f;
//	
//							if( tankScale > 1.5f )	tankScale	=	1.25f;
//							if( tankScale < 0.5f )	tankScale	=	0.75f;
//	
//							//	爆発エフェクトをセット
//							ExplosionEffect::Set(tank[i]->GetPos(), 0.25f * tankScale, 5, 1.0f);
//						}
//	
//	#ifdef	USE_SOUND_EFFECT
//						//	爆発音を鳴らす
//						{
//							bool	loop = false;
//							float	pitch = 1.0f - (rand() % 10) * 0.001f;
//							float	gain = 5.0f * 20.0f;
//							Vector3	pos = tank[i]->GetPos();
//							Vector3	velocity = Vector3(0.0f, 0.0f, 0.0f);
//							SoundManager::Play("爆発音", loop, pitch, gain, pos, velocity);
//						}
//	#endif
//					}
//	
//					//	生存フラグを折る
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
		// サーバーにＭｙＩＤが操作できるスロット番号を取得
		// それが選択スロットだった場合スロットの操作を次のステップへ
		BYTE clientSelectSlot	= GameMasterClientA::GetSelectSlot();		// クライアントが操作許可を
		BYTE serverUsingSlot	= 0xFF;										// サーバーが使用しているスロット
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

	//イベントデータを要求

	/*送信*/
	struct
	{
		BYTE com;
		BYTE IsCountDownRequest;
	}send;
	

	send.com = EVENT_REQUEST_SEND;
	
	send.IsCountDownRequest = EventManager::IsCountDownRequest();

	
	m_pClient->Send(&send, sizeof(send));

	/*受信*/
	
	const int MaxByte = 255;
	BYTE receive[MaxByte];
	if (m_pClient->Receive((char*)&receive, sizeof(receive)) <= 0)
		return;
	if (receive[0] == EVENT_NODATA)
		return;
	int i = receive[0];

	Event* lpEvent = EventManager::GetEvent();

	//comの内容により変化
	switch (receive[0])
	{
	case EVENT_FLAG_RECEIVE:
		//イベント番号
		EventManager::ReceiveEventFlag((char*)&receive);
		break;

	case EVENT_BASE_DATA_RECEIVE:
		//土台データ
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
		//オブジェクトデータ
		break;

	case EVENT_COUNTDOWN_FLAG_RECEIVE:
		EventManager::ReceiveCountDownFlag();
		//カウントダウンフラグ
		break;

	case EVENT_CURRENT_DATA_RECEIVE:
		EventManager::ReceiveCountDownFlag();
		if (lpEvent)
			lpEvent->ReceiveCurrentData((char*)&receive);
		else
			return;
		//オブジェクトデータ
		break;

	}
	/*送り返す*/
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


	//送信
	struct
	{
		BYTE    com;
		char    message[MAX_CHAT];
	}send;

	send.com = CHAT_DATA_SEND;

	//	送信するメッセージを格納	
	{
		//	文字数が上限を超えている場合、制限文字数内の一番最後の１文字を消して、終端文字を入れる
		if( str.size() > MAX_CHAT - 1 ){
			//	制限文字数内の最後の文字のバイト数を調べる
			int	lastCharaByte	=	0;
			int	byteCursor		=	0;

			//	制限文字数を超えるまでループ
			while( true ){
				//	バイト数を調べる
				lastCharaByte	=	_mbclen( ( const unsigned char* )( &str.c_str()[ byteCursor ] ) );

				//	終了判定
				if( byteCursor + lastCharaByte >= MAX_CHAT )	break;
				//	カーソルをずらす
				else											byteCursor	+=	lastCharaByte;
			}

			//	文字をコピー
			int	copyBytes		=	byteCursor;
			for( int i = 0; i < copyBytes; i++ ){
				send.message[i]	=	str.c_str()[i];
			}

			//	終端文字を入れる
			send.message[ byteCursor ]	=	'\0';
		}
		//	文字数が制限内に収まる場合はそのままコピー
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

	//	受信
	struct
	{
		BYTE    flg;				//	0無視 1有効
		BYTE    clientID;
		char    message[MAX_CHAT];
	}receive;

	m_pClient->Receive( &receive, sizeof(receive) );

	//	受け取った
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
		BYTE	giveCoinNum;	//	倒した人が取得したコイン数
	}receive;

	m_pClient->Receive( &receive, sizeof(receive) );


	//	受け取った
	if( receive.flg == 1 )
	{
		//	キルログセット
		int			playerID		=	GameMasterClientA::GetMyID();
		LPCSTR		myName			=	GameMasterClientA::GetPlayerName( receive.slayer );
		LPCSTR		shooterName		=	GameMasterClientA::GetPlayerName( receive.killer );

		D3DXCOLOR	myColor
			=	( GameMasterClientA::GetPlayerTeam( receive.slayer ) != GameMasterClientA::GetMyTeam() )?
				0xAFff0000	:	0xAF0000ff;
		D3DXCOLOR	shooterColor
			=	( GameMasterClientA::GetPlayerTeam( receive.killer ) != GameMasterClientA::GetMyTeam() )?
				0xAFff0000	:	0xAF0000ff;

		//	自分は黄色に
		if( receive.slayer == playerID )	myColor			=	0xAFff8f00;
		if( receive.killer == playerID )	shooterColor	=	0xAFff8f00;
		
		//	自分と同じならギミックに殺された
		if( receive.slayer == receive.killer )	GameLog::SetKillLog( "ギミック", myName, 0xAF00ff00, myColor );
		else									GameLog::SetKillLog( shooterName, myName, shooterColor, myColor );


		PlayerEffectCoin::AddDeathEffect( receive.killer, receive.slayer, receive.giveCoinNum );

		//	自分が死亡した場合は倒した敵をロックオン（キルカメラ用）
		if( receive.slayer == playerID ){
			tank[ playerID ]->SetLockOnTank( receive.killer );
		}

		//破片セット
		TankScrapper::Set(receive.slayer);

		//	エフェクトセット
		{
			float	hp			=	tank[receive.slayer]->GetHP();
			float	tankScale	=	hp / 600.0f;
			tankScale	=	1.0f + ( tankScale - 1.0f ) * 0.5f;

			if( tankScale > 1.5f )	tankScale	=	1.25f;
			if( tankScale < 0.5f )	tankScale	=	0.75f;

			//	爆発エフェクトをセット
			ExplosionEffect::Set(tank[receive.slayer]->GetPos(), 0.25f * tankScale, 5, 1.0f);
		}

#ifdef	USE_SOUND_EFFECT
		//	爆発音を鳴らす
		{
			bool	loop = false;
			float	pitch = 1.0f - (rand() % 10) * 0.001f;
			float	gain = 5.0f * 20.0f;
			Vector3	pos = tank[receive.slayer]->GetPos();
			Vector3	velocity = Vector3(0.0f, 0.0f, 0.0f);
			SoundManager::Play("爆発音", loop, pitch, gain, pos, velocity);
		}
#endif
	}

}

// 沖追加
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
	//	送信
	BYTE com( ITEM_GET_INFO_RECEIVE );
	m_pClient->Send( &com, sizeof(com) );

	//	受信
	struct
	{	
		BYTE	is;
		u32		respawn;
		SHORT	slotUV[3];
	}receive;

	m_pClient->Receive( &receive, sizeof(receive) );

	//	アイテムゲット
	if( receive.is )
	{
		ItemManager::ItemStart( receive.slotUV );
	}

	ItemManager::m_itemExistPoint = receive.respawn;
}
void SocketManager::ReceiveItem()
{
	//	送信
	BYTE com( ITEM_DATA_RECEIVE );
	m_pClient->Send( &com, sizeof( com ));

	//	受信
	struct
	{	
		BYTE effect;
		BYTE level;
	}receive[PANZER_MAX];
		
	//	全員分受け取る
	m_pClient->Receive( &receive, sizeof(receive) );
	
	//	受け取ったデータを格納
	for( int i=0; i<PANZER_MAX; ++i )
		tank[ i ]->SetItemEffect( (ITEM_EFFECT)receive[i].effect, receive[i].level );
}
