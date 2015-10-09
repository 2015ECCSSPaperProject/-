
#pragma once

class Poster_manager;
class Stage;
#include	"../Enum_public.h"

class Player
{
public:

	//===============================================
	//	�萔
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, DIE, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };

private:

	//===============================================
	//	��{�p�����[�^
	//===============================================
	Vector3			pos;
	Vector3			angle;
	float			scale;

	Vector3			move;
	float			speed;
	float			fallspeed;
	int				se_receive;

	iex3DObj		*model;

	Poster_manager	*poster_mng;
	TEAM_COLOR		team_col;

	//===============================================
	//	���̑�
	//===============================================
	int				poster_num;
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;


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
			Player *me;

		public:
			Base(Player* me) :me(me){}
			virtual void Initialize(){}
			virtual void Update(){}
			virtual void Render(){}

			// 3DObj�X�V
			void Update_obj();

			void Set_motion(int no);
		};

		//===========================================
		//	�ړ����(TPS)
		class Move : public Base
		{
		public:
			Move(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	�ړ����(FPS)
		class MoveFPS : public Base
		{
		public:
			MoveFPS(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	�U�����
		class Attack : public Base
		{
		public:
			Attack(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	�|�X�^�[�\��t�����
		class Paste : public Base
		{
		public:
			Paste(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	�|�X�^�[�j����
		class Rend : public Base
		{
		public:
			Rend(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	����ł���
		class Die : public Base
		{
		private:
			int die_frame;

		public:
			Die(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};
	};


	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// ���݂̃v���C���[�̃��[�h
	
	DO_FLAG do_flag;			// Z�L�[�������牽�����邩
	

	//===============================================
	//	�e�N�X�`���ύX
	//===============================================
	void Set_texture(char *filename);
	void Change_color()
	{
		team_col = (team_col == TEAM_COLOR::ONE) ? TEAM_COLOR::TWO : TEAM_COLOR::ONE;
	}


public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	Player();
	~Player();
	void Initialize(iex3DObj *obj, Poster_manager *poster_mng, TEAM_COLOR col);
	void Release();


	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update(Stage *stage);
	void Render();


	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================

	// ���W
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// �A���O��
	Vector3 Get_angle(){ return angle; }
	void Get_angle(Vector3 &out){ out = angle; }
	void Set_angle(const Vector3 &a){ angle = a; }
	Vector3 Get_forward(){ return Vector3(model->TransMatrix._31, model->TransMatrix._32, model->TransMatrix._33); }

	// �v���C���[�̃��[�h
	ACTION_PART Get_action(){ return action_part; }
	void Change_action(ACTION_PART part)
	{
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Z�L�[�ŉ������邩
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

};
