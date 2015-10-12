
#include	"iextreme.h"
#include	"system/system.h"
#include	"../Manager/GameManager.h"
#include	"BasePlayer.h"
#include	"NetPlayer.h"

// OLL ENUM
enum {
	UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8,
	BUTTON1 = 16, BUTTON2 = 32, BUTTON3 = 64
};


//****************************************************************************************************************
//
//  
//
//****************************************************************************************************************
NetPlayer::NetPlayer(int id) :
m_id(id)
{
	pos.x =-10+(m_id * 4);// ��
}


//*************************************************************************************************************************
//		�X�V
//*************************************************************************************************************************
void NetPlayer::Update(void)
{
	CONTROL_DESC	controlDesc;

	// �����ɃT�[�o�[�}�l�[�W���[����f�X�N������Ă���
	controlDesc.moveFlag = ServerManager::GetDesc(m_id).moveFlag;

	//	�R���g���[���[�ɑ���p�����[�^���p�X
	Control_Update(controlDesc);//���f�X�N�Z�b�g

	//	�f�X�N������
	ServerManager::ResetDesc(m_id);

	//���Ńv���C���[�X�V
	BasePlayer::Update();
}

/**********************************************************/
//		�f�X�N�̏���
/**********************************************************/
void NetPlayer::Control_Update(const CONTROL_DESC& _ControlDesc)
{

	if (_ControlDesc.moveFlag&UP)pos.z += 0.8f;
	if (_ControlDesc.moveFlag&DOWN)pos.z -= 0.8f;
	if (_ControlDesc.moveFlag&LEFT)pos.x -= 0.8f;
	if (_ControlDesc.moveFlag&RIGHT)pos.x += 0.8f;




}