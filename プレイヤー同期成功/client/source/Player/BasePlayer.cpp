#include	"iextreme.h"
#include	"../system/system.h"
#include	"BasePlayer.h"


/*	�x�[�X�v���C���[	*/

BasePlayer::BasePlayer()
{

	obj = new iexMesh("DATA/a.imo");

	pos = VECTOR_ZERO;
	move = VECTOR_ZERO;

	// ������
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




//�@����
BasePlayer* player[PLAYER_MAX];