#pragma once

/*	�x�[�X�v���C���[(�N���C�A���g)	*/

#define TYPE1
//#define TYPE2

class BasePlayer
{
public:

	//	����f�X�N
	struct	CONTROL_DESC{

		BYTE moveFlag;

		float	mouseX;			//�@�}�E�XX
		float	mouseY;			//�@�}�E�XY

		BYTE motion_no;			// ���[�V�����ԍ�

		BYTE rendFlag;			// �j�����ɑ���
		BYTE controlFlag;		// �{�^��

	};
	CONTROL_DESC m_controlDesc;//�@�R���g���[��


	//==============================================================================
	// �ǉ�����
	//===============================================
	//	�萔
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, FREEZE, DIE, RESPAWN, PLANE, GUN, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };
	enum class MODEL{ NORMAL, DIE, PLANE, GUN, MAX };

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

	POINT			mousePos;		//	�}�E�X���W

	int				motion_no;
	int				god_gage;

	//===============================================
	//	���̑�
	//===============================================
	int				poster_num;
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;
	MODEL			model_part;
	bool			toggle_c;


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
			virtual void Update(){}
			virtual void Render(){}

			// 3DObj�X�V
			void Update_obj();
		};

		//===========================================
		//	�ړ����(TPS)
		class Move : public Base
		{
		public:
			Move(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	�ړ����(FPS)
		class MoveFPS : public Base
		{
		public:
			MoveFPS(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	�U�����
		class Attack : public Base
		{
		public:
			Attack(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
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
			void Update();
			void Render();
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
			void Update();
			void Render();
		};

		//===========================================
		//	�ł܂��Ă���
		class Freeze : public Base
		{
		public:
			Freeze(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	����ł���
		class Die : public Base
		{
		private:
			int flashing;

		public:
			Die(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	���X�|�[�����
		class Respawn : public Base
		{
		private:
			BYTE flashing;

		public:
			Respawn(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	���Ђ��[�����
		class Hikouki : public Base
		{
		public:
			Hikouki(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	���S�C���
		class Gun : public Base
		{
		public:
			Gun(BasePlayer*me) :Base(me){}

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

	void	Control_all();//�@������������ǉ�


public:
	//===============================================
	//	�������Ɖ��
	//===============================================
	BasePlayer();
	virtual ~BasePlayer();
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
	const CONTROL_DESC& GetDesc()const{ return	m_controlDesc; }

	// ���W
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// �A���O��
	float Get_angleY(){ return angleY; }
	//void Get_angle(Vector3 &out){ out = angle; }
	void Set_angleY(float p){ angleY = p; }
	Vector3 Get_forward(){ return Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33); }

	// �v���C���[�̃��[�h
	ACTION_PART Get_action(){ return action_part; }
	void Set_aciton(ACTION_PART part)
	{
		if (action_part != part)
		{
			Change_action(part);
		}
	}
	void Change_action(ACTION_PART part)
	{
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Z�L�[�ŉ������邩
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	void Set_motion_no(int no)
	{
		motion_no = no; 
	}
	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);


	void Set_texture(MODEL type, Texture2D *tex)
	{
		models[(int)type]->SetTexture(tex, 0);
	}

	int Get_god_gage(){ return god_gage; }
	void Set_god_gage(int val){ god_gage = val; }

	CONTROL_DESC Get_controlDesc(){ return m_controlDesc; }

};

//extern BasePlayer* player[PLAYER_MAX];