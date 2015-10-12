#pragma once
//*****************************************************************************************************************************
//
//		
//
//*****************************************************************************************************************************
#include	<string>
class UDPClient;


//===================================================================================
//   
//===================================================================================
//---------------------------------------------------------------------
//  �v���C���[�̃��O�C�����
//---------------------------------------------------------------------
struct UserData
{	
	BYTE	com;		//	�f�[�^�`��	
	char	name[9];	//	���O			
	BYTE	row;		//
	BYTE	col;		//
	BYTE	isReady;	//
};

//---------------------------------------------------------------------
//  ��Ԃ̃p�����[�^
//---------------------------------------------------------------------
struct PanzerData
{
	int						parts[3];
	bool					isGold[3];
	float					centerGravitiy;
	Vector3					pos;
	D3DXQUATERNION			orien;
	float					canonAngle;
	float					headAngle;

	float					energy;

	bool					isInvincible;

	int						prize;
	int						numBet;

	bool					IsBoost;
};

struct EventData
{
	int						EventType;
	BYTE					ReelStart;
	BYTE*				EventBaseData;	//memcpy���Ďg���B���g�̓C�x���g�ɂ���ăo���o��
	BYTE*				EventObjectData; //memcpy���Ďg���B���g�̓C�x���g�ɂ���ăo���o��
	BYTE					ReadyFlg;
	BYTE					CountDownFlg;
	BYTE*				EventCurrentData; //memcpy���Ďg���B���g�̓C�x���g�ɂ���ăo���o��
};



//=======================================================================
//	
//=======================================================================
class SocketManager
{
public:
	std::string				m_myName;
	s32						m_team;		

private:
	UDPClient*				m_pClient;	
	s32						m_myID;

	UserData				m_user	[ PANZER_MAX ];
	PanzerData				m_panzer[ PANZER_MAX ];

	//����ǉ�
	EventData				m_event;

	static SocketManager*	m_pInst;
	SocketManager();

	static const int MAX_CHAT = 255;
	enum
	{
		NEW_USER  ,
		INIT_DATA ,
		TEAM_DATA ,	

		INIT_GAME_DATA,
		USER_INIT,
		USER_DATA ,
		USER_RESPOWN,

		CHAT_DATA_SEND,
		CHAT_DATA_RECEIVE,
	
		LOG_DATA_RECEIVE,
	
		ITEM_GET_INFO_RECEIVE,
		ITEM_DATA_RECEIVE,

		EXIT_USER,

		MUTCH_TEAM_DATA,	//	���ǉ�
		MUTCH_GAME_DATA,	//	���ǉ�
		MUTCH_GAME_END,		//	���ǉ�
		MUTCH_USE_SLOT,		//	���ǉ�
		MUTCH_COIN_DATA,	//	���ǉ�

		EVENT_REQUEST_SEND = 30,				//����ǉ�

		EVENT_NODATA,									//����ǉ�

		EVENT_FLAG_RECEIVE,						//����ǉ�
		EVENT_BASE_DATA_RECEIVE,				//����ǉ�
		EVENT_OBJECT_DATA_RECEIVE,				//����ǉ�
		EVENT_COUNTDOWN_FLAG_RECEIVE,			//����ǉ�
		EVENT_CURRENT_DATA_RECEIVE,				//����ǉ�


	};

private:
	bool					m_Alive[ PANZER_MAX ];
	int						m_DeathWait[PANZER_MAX];

public:
	~SocketManager();
	bool Init();


	//---------------------------------------------------------------------
	//   network
	//---------------------------------------------------------------------
	void UpdateTeam( int isReady );

	void InitGame();
	void InitUser();
	void UpdateUser();
	void UpdateEvent();

	void SendChat( const std::string& str );
	void ReceiveChat();
	void ReceiveLog();	

	void ReceiveGetItemInfo();
	void ReceiveItem();

	// ���ǉ�
	void SendGameEndFlag();
		
	int					GetID		()				const{ return m_myID;				}
	const char*			GetName		()				const{ return m_myName.c_str();	}
	const UserData&		GetUser		( s32 client )	const{ return m_user[client];		}
	const PanzerData    GetPanzer	( int idx )		const{ return m_panzer[idx];		}
	void				SendName	();
	void				SetName		( int idx, const char* name ){ strcpy_s( m_user[idx].name, name );}



	//---------------------------------------------------------------------
	//   singleton
	//---------------------------------------------------------------------
	static SocketManager* getInstance()
	{
		if( !m_pInst )m_pInst = new SocketManager;
        return m_pInst;
    }
	static void Release()
	{
		if( m_pInst )
		{
			delete m_pInst;
			m_pInst=0;
		}
	}
};


#define SOCKET_MANAGER ( SocketManager::getInstance() )