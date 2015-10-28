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
public:
	//==============================================================================
	// �ǉ�����
	//===============================================
	//	�萔
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, DIE, RESPAWN, PLANE, GUN, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };

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
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;

	int mynumber;


	//===============================================
	//	�v���C���[�s���Ϗ��N���X(�C���i�[�N���X)
	//===============================================
		//===========================================
		//	��Ԋ��N���X
	class ActionBase
		{
		protected:
			BasePlayer *me;

		public:
			ActionBase(BasePlayer* me) :me(me){}
			virtual void Initialize(){}
			virtual void Update(const CONTROL_DESC &_ControlDesc){}
			virtual void Render(){}

			void Set_motion(int no);
		};

	//===========================================
		//	�ړ����(TPS)
	class ActionMove : public ActionBase
		{
		public:
			ActionMove(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	�ړ����(FPS)
	class ActionMoveFPS : public ActionBase
		{
		public:
			ActionMoveFPS(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	�U�����
	class ActionAttack : public ActionBase
		{
		public:
			ActionAttack(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	�|�X�^�[�\��t�����
	class ActionPaste : public ActionBase
		{
		private:
			int timer;
		public:
			ActionPaste(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	�|�X�^�[�j����
	class ActionRend : public ActionBase
		{
		private:
			bool rended;
		public:
			ActionRend(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	����ł���
	class ActionDie : public ActionBase
		{
		private:
			BYTE die_frame;
			BYTE flashing;

		public:
			ActionDie(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	���X�|�[�����
	class ActionRespawn : public ActionBase
		{
		private:
			BYTE invincible_time;
			BYTE flashing;

		public:
			ActionRespawn(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	���Ђ��[�����
	class ActionHikouki : public ActionBase
		{
		public:
			ActionHikouki(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	���S�C���
	class ActionGun : public ActionBase
		{
		public:
			ActionGun(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};


	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	ActionBase *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// ���݂̃v���C���[�̃��[�h

	DO_FLAG do_flag;			// Z�L�[�������牽�����邩


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
	ACTION_PART Get_action()
	{
		return action_part; 
	}
	void Change_action(ACTION_PART part)
	{
		if (action_part > ACTION_PART::MAX)
		{
			MessageBox(0, "�T�[�o�[�̃A�N�V�����擾���̒l����������", null, MB_OK);
		}
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Z�L�[�ŉ������邩
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

	bool isInvincible(){ return invincible; }

};

namespace PlayerManager
{
	int Check_attack(int me);
}



extern BasePlayer* player[PLAYER_MAX];