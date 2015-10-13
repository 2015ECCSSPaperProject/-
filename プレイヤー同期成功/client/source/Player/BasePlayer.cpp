#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"


/*	ベースプレイヤー	*/

BasePlayer::BasePlayer()
{

	obj = new iexMesh("DATA/a.imo");

	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;

	// 初期化
	m_controlDesc.moveFlag = 0;
 	 

}

BasePlayer::~BasePlayer()
{
	SAFE_DELETE(obj);
}

void BasePlayer::Update()
{


};

void BasePlayer::Render()
{

	obj->SetPos(pos);
	obj->Update();
	obj->Render();
};




//　実態
BasePlayer* player[PLAYER_MAX];