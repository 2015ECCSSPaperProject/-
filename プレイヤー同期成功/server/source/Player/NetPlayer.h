#pragma once
/*	�l�b�g�v���C���[�B	*/


//	����f�X�N
struct	CONTROL_DESC{
	BYTE	moveFlag; //�@�㉺���E�ɓ������t���O

};

/*�O���錾*/
class BasePlayer;

//********************************************************************************************************************
//		net
//****************************************************************************************************************
class	NetPlayer : public	BasePlayer
{
private:
	int		m_id;

public:
	NetPlayer(int id);
	void	Update(void);

	//	�X�V�i����j
	void	Control_Update(const CONTROL_DESC& _ControlDesc);

};
