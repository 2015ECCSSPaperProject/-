#pragma once

/*	�x�[�X�v���C���[	*/

#include "../../../share_data/Enum_public.h"

//	����f�X�N
struct	CONTROL_DESC
{
	BYTE	moveFlag; //�@�㉺���E�ɓ������t���O

	float	mouseX; //�}�E�X
	float	mouseY;

	BYTE	motion_no;// ���[�V�����̔ԍ�

	BYTE	rendFlag;	// �j���Ƃ��Ɏg��
	BYTE	controlFlag;// �{�^��
};

#define TYPE1
//#define TYPE2

class BasePlayer
{
protected:
	//===============================================
	//	��{�p�����[�^
	//===============================================
	Vector3			pos;
	float			angleY;
	float			scale;

	Vector3			move;
	float			speed;
	float			fallspeed;
	int				se_receive;

	iex3DObj		*model;

	int				motion_no;

	bool			isLand;
	bool			isJump;
	float			jump_pow;
	bool			invincible;


	//===============================================
	//	���̑�
	//===============================================
	int				poster_num;

	int mynumber;


	//===============================================
	//	�v���C���[�s���Ϗ��N���X(�C���i�[�N���X)
	//===============================================
	template<typename ME> // �����̃N���X
	class ActionBase
	{
	protected:
		ME *me;

	public:
		ActionBase(ME* me) :me(me){}
		virtual void Initialize(){}
		virtual void Update(const CONTROL_DESC &_ControlDesc){}
		virtual void Render(){}

		void Set_motion(int no);
	};

	int		m_id;

public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	BasePlayer(){}
	BasePlayer(int id);
	~BasePlayer();
	virtual void Initialize(iex3DObj *obj);
	virtual void Release();


	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	virtual void Update();
	virtual void Render();


	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================
	// ���W
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// �A���O��
	float Get_angleY(){ return angleY; }

	Vector3 Get_forward(){ return Vector3(model->TransMatrix._31, model->TransMatrix._32, model->TransMatrix._33); }

	// �v���C���[�̃��[�h
	virtual int Get_action() = 0;
	virtual void Change_action(int part) = 0;

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

	bool isInvincible(){ return invincible; }

};

namespace PlayerManager
{
	int Check_attack(int me);
}



extern BasePlayer* player[PLAYER_MAX];