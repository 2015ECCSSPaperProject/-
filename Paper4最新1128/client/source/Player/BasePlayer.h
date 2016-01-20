#pragma once
#include "../Animation/AnimationUV.h" 
#include "../Explosion/Explosion.h"
class AnimationUV;
class Explosion;
class Barrier;
class Rush;
class SkillBegin;
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
		BYTE skillFlag;			// �X�L�������t���O
	};
	CONTROL_DESC m_controlDesc;//�@�R���g���[��


	//==============================================================================
	// �ǉ�����
	//===============================================
	//	�萔
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_TARGET, ATTACK, REND, FREEZE, DIE, RESPAWN, GUN, MANHOLE, THROUGH, SYURIKEN, TRANS_FORM, REND_OBJ, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };
	enum class MODEL
	{
		NORMAL,		// ���f��
		DIE,		// �j��郂�f��
		GUN,		// �S�C
		SYURIKEN,	// �藠��
		REND_CALENDAR,
		REND_MONEY,
		REND_SIGN,
		REND_SHINBUN,
		REND_WC_PAPER,
		REND_ZASSHI,
		REND_MAGAZINE,
		REND_SEISHO,
		REND_SHOJI,
		MAX
	};
	enum class SKILL{ GUN, SYURIKEN, KABUTO, MAX };

protected:
	//===============================================
	//	��{�p�����[�^
	//===============================================
	Vector3			pos;
	float			angleY;
	float			scale;

	Vector3			move;
	Vector3			prev_pos;
	float			speed;
	float			fallspeed;
	int				se_receive;
	int				se_receive2;

	iex3DObj		*models[(int)MODEL::MAX];

	POINT			mousePos;		//	�}�E�X���W
	int				motion_no;
	int				god_gage;

	bool			isMyNunber;
	int kind_paper_obj;			// �j���Ă鏬���̎��(-1�������j���Ă��Ȃ����)�w�b�_�[�̃C���N���[�h�̖�肪����̂�int�^�ɂ���

	BYTE			 stateFlag;	// ���
	bool			isBarrier;

	//===============================================
	//	�X�L���Q�[�W
	//===============================================
	struct
	{
		bool unlock;			// ���ւ��Ă邩�ǂ���
		int unlock_rend_count;	// ���ւɕK�v�Ȏ���j�閇��
		int wait_time;			// 0�Ȃ�X�L�����Ă� 1�ȏ�Ȃ�f�N�������g
		int cool_time;			// �g�p��ɑ҂���(�Œ�l)
		ACTION_PART do_action;	// �����A�N�V����
	}skill_data[(int)SKILL::MAX];

	int select_skill;	// ++�Ƃ�����̂�int

	void Check_unlock(int rend_count)
	{
		for (int i = (int)SKILL::MAX - 1; i >= 0; i--)
		{
			if (skill_data[i].unlock) break;
			if (rend_count >= skill_data[i].unlock_rend_count)
				skill_data[i].unlock = true;
		}
	}

	//===============================================
	//	���̑�
	//===============================================
	int				poster_num;
	MODEL			model_part;

	//===============================================
	//	�G�t�F�N�g
	//===============================================
	Explosion* explosion;

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
			virtual void Render(iexShader *shader = nullptr, char *name = '\0'){}



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
			void Render(iexShader *shader = nullptr, char *name = '\0');
			
		};

		//===========================================
		//	�ړ����(�^�[�Q�b�e�B���O��)
		class MoveTarget : public Base
		{
		public:
			MoveTarget(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�U�����
		class Attack : public Base
		{
		public:
			Attack(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
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
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�ł܂��Ă���
		class Freeze : public Base
		{
		public:
			Freeze(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
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
			void Render(iexShader *shader = nullptr, char *name = '\0');
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
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	���S�C���
		class Gun : public Base
		{
		public:
			Gun(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�}���z�[������
		class Manhole : public Base
		{
		public:
			Manhole (BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�����ǒʂ蔲��
		class Through : public Base
		{
		public:
			Through(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�藠��
		class Syuriken : public Base
		{
		private:
			bool trg;
		public:
			Syuriken(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�ϐg
		class TransForm : public Base
		{
		public:
			TransForm(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	�Ώ����̔j��
		class RendObj : public Base
		{
		public:
			RendObj(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};
	};


	//===============================================
	//	�Ϗ��N���X�ւ̃|�C���^
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// ���݂̃v���C���[�̃��[�h
	Barrier *barrier;			// �o���A�G�t�F�N�g
	Rush *rush;
	SkillBegin *skill_begin;
	DO_FLAG do_flag;			// Z�L�[�������牽�����邩


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
	virtual void Render(iexShader *shader = nullptr, char *name = '\0');
	void Render_forword();
	void Render_rush();
	
	//===============================================
	//	�G�t�F�N�g�̍X�V�ƕ`��
	//===============================================
	void EffectInit();
	void EffectUpdate();
	void EffectRender();
	void EffectRelease();

	void ExplosionAction();

	//===============================================
	//	�Q�b�^�[,�Z�b�^�[
	//===============================================
	const CONTROL_DESC& GetDesc()const{ return	m_controlDesc; }

	// ���W
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	Vector3 Get_center_pos(){ return Vector3(pos.x, pos.y + 5.0f, pos.z); }
	void Set_pos(const Vector3 &p){ pos = p; }
	
	// �O�擾
	Vector3 Get_Flont(){
		Vector3 Out;
		Out.x = models[(int)model_part]->TransMatrix._31;
		Out.y = models[(int)model_part]->TransMatrix._32;
		Out.z = models[(int)model_part]->TransMatrix._33;
		Out.Normalize();
		return Out;
	}

	// �A���O��
	float Get_angleY(){ return angleY; }
	//void Get_angle(Vector3 &out){ out = angle; }
	void Set_angleY(float p){ angleY = p; }
	Vector3 Get_forward(){ return Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33); }

	// �v���C���[�̃��[�h
	ACTION_PART Get_action(){ return action_part; }
	virtual void Set_action(ACTION_PART part);
	virtual void Change_action(ACTION_PART part)
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

	int Get_poster_num(){ return poster_num; }
	void Set_poster_num(int no){ poster_num = no; }

	// 0�`1
	bool isUnlockSkill(int no){ return skill_data[no].unlock; }
	int Get_select_skill(){ return (int)select_skill; }
	float Get_skill_percentage(int no){ return 1.0f - ((float)skill_data[no].wait_time / (float)skill_data[no].cool_time); }

	// ���f��
	iex3DObj* Get_Model(){ return models[(int)model_part]; }

	// ��ԃt���O
	void Set_state_flag(BYTE flag)
	{
		stateFlag = flag;
	}
	// �}���z�[��
	bool			isManhole;
	bool			manhole_no_haninai;
	Vector3			next_manhole_pos;	// �~�肽��́A�オ������̍��W
};

//extern BasePlayer* player[PLAYER_MAX];