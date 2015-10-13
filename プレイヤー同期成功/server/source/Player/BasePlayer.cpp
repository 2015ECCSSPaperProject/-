#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"


/*	ベースプレイヤー	*/

BasePlayer::BasePlayer()
{

	obj = new iexMesh("DATA/a.imo");

	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;
}

BasePlayer::~BasePlayer()
{
	SAFE_DELETE(obj);
}

void BasePlayer::Update()
{


	obj->SetPos(pos);
	obj->Update();
};

void BasePlayer::Render()
{

	obj->Render();
};

//　実態
BasePlayer* player[PLAYER_MAX];