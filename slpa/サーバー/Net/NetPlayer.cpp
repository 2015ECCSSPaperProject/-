
#include	"iextreme.h"
#include	"system/system.h"

#include	"SystemAdd/Drawing.h"
#include	"InputManager.h"

#include	"Item.h"

#include	"Tank.h"
#include	"TankController.h"

#include	"Stage.h"

#include	"Player.h"
#include	"Camera.h"

#include	"InformationBuffer.h"
#include	"Interface.h"
#include	"NetPlayer.h"

#include	"Server.h"
//****************************************************************************************************************
//
//  
//
//****************************************************************************************************************
NetPlayer::NetPlayer( int id ):
m_id(id)
{
}


//*************************************************************************************************************************
//		更新
//*************************************************************************************************************************
void NetPlayer::Update( void )
{
	CONTROL_DESC	controlDesc;

	controlDesc.fire		=	ServerManager::GetDesc( m_id ).fire		==	1;
	controlDesc.boost		=	ServerManager::GetDesc( m_id ).boost	==	1;
	controlDesc.shield		=	ServerManager::GetDesc( m_id ).shield	==	1;
	controlDesc.acceleMode	=	( ACCELE_MODE )ServerManager::GetDesc( m_id ).acceleMode;
	controlDesc.bodyRot		=	ServerManager::GetDesc( m_id ).bodyRot;
	controlDesc.headRot		=	ServerManager::GetDesc( m_id ).headRot;
	controlDesc.cannonRot   =	ServerManager::GetDesc( m_id ).cannonRot;
	controlDesc.rapidAccele	=	( ACCELE_MODE )ServerManager::GetDesc( m_id ).rapidAccele;
	controlDesc.quickTurn	=	( ACCELE_MODE )ServerManager::GetDesc( m_id ).quickTurn;
		
	//	コントローラーに操作パラメータをパス
	TankController::Update( controlDesc );

	//	デスク初期化
	ServerManager::ResetDesc( m_id );
}
