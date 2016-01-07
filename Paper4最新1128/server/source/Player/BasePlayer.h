#pragma once

/*	�x�[�X�v���C���[	*/

#include "../../../share_data/Enum_public.h"

//	����f�X�N
struct	CONTROL_DESC
{
	BYTE	moveFlag;	// �㉺���E�ɓ������t���O

	float	mouseX;		// �}�E�X
	float	mouseY;

	BYTE	motion_no;	// ���[�V�����̔ԍ�

	BYTE	rendFlag;	// �j���Ƃ��Ɏg��
	BYTE	controlFlag;// �{�^��
	BYTE	skillFlag;	// �X�L�������t���O
};

#define TYPE1

class PaperQueue;

class BasePlayer
{
public:
	PaperQueue *paperqueue;

	//==============================================================================
	// �ǉ�����
	//===============================================
	//	�萔
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_TARGET, ATTACK, PASTE, REND, FREEZE, DIE, RESPAWN, PLANE, GUN, MANHOLE, THROUGH, SYURIKEN, TRANS_FORM, REND_OBJ, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };
	enum class MODEL{ NORMAL, DIE, PLANE, GUN, SYURIKEN, MAX };
	enum class SKILL{ GUN, SYURIKEN, KABUTO, ZENRYOKU, MAX };

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

	iex3DObj		*models[(int)MODEL::MAX];

	int				motion_no;

	bool			isLand;
	bool			isJump;
	bool			attackFlag;	// �󒆂Ō��Ă�̂�1�񂾂�
	float			jump_pow;
	bool			invincible;

	int				god_gage;	// �_�Q�[�W(���������j���������ɂ���)


	//===============================================
	//	�X�L���Q�[�W
	//===============================================
	struct
	{
		//bool unlock;			// ���ւ��Ă邩�ǂ���
		//int unlock_rend_count;	// ���ւɕK�v�Ȏ���j�閇��
		//int wait_time;			// 0�Ȃ�X�L�����Ă� 1�ȏ�Ȃ�f�N�������g
		//int cool_time;			// �g�p��ɑ҂���(�Œ�l)
		ACTION_PART do_action;	// �����A�N�V����
	}skill_data[(int)SKILL::MAX];

	SKILL select_skill;

	//void Check_unlock(int rend_count)
	//{
	//	for (int i = (int)ACTION_PART::MAX - 1; i >= 0; i--)
	//	{
	//		if (skill_data[i].unlock) break;
	//		if (rend_count >= skill_data[i].unlock_rend_count) skill_data[i].unlock = true;
	//	}
	//}

	//===============================================
	//	���̑�
	//===============================================
	int				poster_num;
	MODEL			model_part;

	int mynumber;


	//===============================================
	//	�v���C���[�s���Ϗ��N���X(�C���i�[�N���X)
	//===============================================
	class Action
	{
	public:

		//===========================================
		//	��Ԋ��N���X
		class Base
		{
		protected:
			BasePlayer *me;

		public:
			Base(BasePlayer* me) :me(me){}
			virtual void Initialize(){}
			virtual void Update(const CONTROL_DESC &_ControlDesc){}
			virtual void Render(){}

			void Set_motion(int no);
		};

		//===========================================
		//	�ړ����(TPS)
		class Move : public Base
		{
		private:
			bool trg_target;
		public:
			Move(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�ړ����(�^�[�Q�b�g)
		class MoveTarget : public Base
		{
		public:
			MoveTarget(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�U�����
		class Attack : public Base
		{
		private:
			int target_no;

		public:
			Attack(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�|�X�^�[�\��t�����
		class Paste : public Base
		{
		private:
			int timer;
		public:
			Paste(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�|�X�^�[�j����
		class Rend : public Base
		{
		private:
			bool rended;
		public:
			Rend(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�ł܂��Ă���
		class Freeze : public Base
		{
		public:
			Freeze(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	����ł���
		class Die : public Base
		{
		private:
			BYTE die_frame;
			BYTE flashing;

		public:
			Die(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	���X�|�[�����
		class Respawn : public Base
		{
		private:
			BYTE invincible_time;
			BYTE flashing;

		public:
			Respawn(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	���Ђ��[�����
		class Hikouki : public Base
		{
		public:
			Hikouki(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	���S�C���
		class Gun : public Base
		{
		public:
			Gun(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�}���z�[������
		class Manhole : public Base
		{
		public:
			Manhole(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�����ǒʂ蔲��
		class Through : public Base
		{
		public:
			Through(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�藠��
		class Syuriken : public Base
		{
		private:
			int syurikentaimaa;
			float max_speed;
			float accel;
			float kasoku;
			bool r;
			Vector3 move_vec;
		public:
			Syuriken(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�ԐM
		class TransForm : public Base
		{
		private:
			int hensintaimaa;
		public:
			TransForm(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	�Ώ����̔j��
		class RendObj : public Base
		{
		private:
			int rend_timer;	// Initiailze�ł��̎��Ԃ�ݒ肵�āA���̎��Ԃ��߂��������

		public:
			RendObj(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};
	};


	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;		// ���݂̃v���C���[�̃��[�h
	ACTION_PART reserve_action;		// �ϐg�����Ƃ��̗\��

	DO_FLAG do_flag;				// Z�L�[�������牽�����邩


	int		m_id;

public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	BasePlayer(){}
	BasePlayer(int id);
	virtual ~BasePlayer();
	virtual void Init_pos();
	virtual void Initialize(iex3DObj **objs);
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
	void Set_angleY(float a){ angleY = a; }

	Vector3 Get_forward(){ return Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33); }

	// �v���C���[�̃��[�h
	ACTION_PART Get_action()
	{
		return action_part; 
	}
	void Change_action(ACTION_PART part)
	{
		if (action_part > ACTION_PART::MAX)
		{
			//MessageBox(0, "�T�[�o�[�̃A�N�V�����擾���̒l����������", null, MB_OK);
		}
		action_part = part;
		action[(int)part]->Initialize();
	}

	// Z�L�[�ŉ������邩
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

	bool isInvincible(){ return invincible; }

	int Get_god_gage(){ return god_gage; }

	int Get_target_no(){return poster_num; }
};