
#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"
#include	"NetPlayer.h"

#include	"../../../share_data/Enum_public.h"

//****************************************************************************************************************
//
//  
//
//****************************************************************************************************************
NetPlayer::NetPlayer(int id) :BasePlayer(id)
{}

NetPlayer::~NetPlayer()
{}



//*************************************************************************************************************************
//		�X�V
//*************************************************************************************************************************
void NetPlayer::Update()
{
	BasePlayer::Update();
}

void NetPlayer::Render()
{
	BasePlayer::Render();
}