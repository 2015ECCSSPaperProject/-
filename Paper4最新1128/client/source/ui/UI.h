#pragma once

class BasePlayer;
class Pie_graph;
class Pie_graph_content;
class SceneMain;
class AnimationRippleEx;
#include<list>

class Telop
{
public:
	bool erase;		// �����Ă�
	Telop(int id);
	~Telop();
	void Update();
	void Render();

private:
	int step;
	int app_timer;	// �\������
	iex2DObj *moji;
};

class UI
{
public:
	//===============================================
	//	�萔
	//===============================================
	enum IMAGE
	{
		TELOP,
		NUMBER,
		NUMBER_BACK,
		ACTION,
		TAPE,
		TAPE_BAR,
		SKILL_GUN,
		SKILL_SYURIKEN,
		SKILL_KABUTO,
		SKILL_FRAME,
		SKILL_SELECT,
		SKILL_ROGO,
		MANHOLE_FADE,
		SCORE_FRAME,
		ARROW_LEFT,
		ARROW_RIGHT,
		ARROW_UP,
		ARROW_DOWN,
		ARROW_ROLL,
		MARK,
		ACTION_DRAG,
		ACTION_HOLD,
		MANHOLE_IN,
		MAX
	};
	enum class MANHOLE_FADE_TYPE
	{
		NONE, F_IN, F_OUT, MAX
	};
	enum class REND_COMMAND
	{
		NONE, LEFT, RIGHT, UP, DOWN,
		RU,	// �E��
		RD,	// �E��
		LU,	// ����
		LD,	// ����
		ROLL	// ��]
	};

private:
	int telopID;
	int seted_ID;


	float manhole_f_scale;
	MANHOLE_FADE_TYPE m_fade_type;
	void fadeM_none();
	void fadeM_in();
	void fadeM_out();
	void(UI::*fadeM_funk[(int)MANHOLE_FADE_TYPE::MAX])();


	// �R�}���h���
	REND_COMMAND command;


	//===============================================
	//	�����̎���
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Pie_graph_content *gauge;


	//===============================================
	//	�A�j���[�V����UI
	//===============================================
	AnimationRippleEx* C_Five;
	AnimationRippleEx* C_Four;
	AnimationRippleEx* C_Three;
	AnimationRippleEx* C_Two;
	AnimationRippleEx* C_One;
	int CountTimeNo;

	int TimerX;			//Timer�𓮂���
	int SkillX;			//Skill�𓮂���
	int GraphX;			//Skill�𓮂���
	// �X�L�����܂�����
	static const int SKILL_MAX = 3;
	AnimationRippleEx*  SkileSphere[SKILL_MAX];
	AnimationRippleEx*  SkileRip[SKILL_MAX];
	float savePercent[SKILL_MAX];// �ߋ��̃p�[�Z���g�������Ă��� 0.0f~1.0f�܂�
	//AnimationRippleEx*  SkileSphere_2;
	//AnimationRippleEx*  SkileSphere_3;
	AnimationRippleEx* action_hold;
	AnimationRippleEx* action_drag;

	AnimationRippleEx* DragRip; //�h���b�O
	//===============================================
	//	�Ϗ�
	//===============================================
	class Mode
	{
	public:
		class Base
		{
		protected:
			UI *me;
		public:
			Base(UI *me) :me(me){}
			virtual ~Base(){}
			virtual void Initialize(){}
			virtual void Update(){}
			virtual void Render(){}
		};

		class Start: public Base
		{
		private:
			int frame;
			int step;
			iex2DObj *yooi, *don;
			AnimationRippleEx* Ready;
			AnimationRippleEx* GO;
			AnimationRippleEx* GORip;
		public:
			Start(UI *me) :Base(me){}
			~Start();
			void Initialize();
			void Update();
			void Render();
			void YooiDon();
		};

		class Main : public Base
		{
		public:
			Main(UI *me) :Base(me){}
			//void Initialize();
			void Update();
			void Render();
		};

		class End :public Base
		{
		private:
			//iex2DObj *sokomade;
			AnimationRippleEx* TimeUp;
			AnimationRippleEx* TimeUpRip;
		public:
			End(UI *me) :Base(me){}
			~End();
			void Initialize();
			void Update();
			void Render();
		};
	};
	Mode::Base *mode;

	//===============================================
	//	���ꂼ��`��̊֐�
	//===============================================
	// �֐�����
	void Graph();		// ���ӂ̉~�O���t
	void SkillGauge();	// �����̃X�L���Q�[�W
	void Action();		// �E���̃A�N�V����UI
	void TimeLimit();	// �^�C�����~�b�g
	void Telop_render();// �e���b�v
	void Manhole_fade();// �}���z�[���̃t�F�[�h
	void Command();

	//===============================================
	//	�e���b�v���X�g
	//===============================================
	std::list<Telop*> List;


	// ��
	bool isYooiDon;

public:
	static float tape_len;	// �e���b�v�e�[�v�̒���(0�`1)

	//===============================================
	//	�������Ɖ��
	//===============================================
	UI();
	void Initialize(BasePlayer *my);
	~UI();

	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render();
	void Render_mark();

	//===============================================
	//	�e���b�v�ǉ�
	//===============================================
	void Set_telopID(int id){ telopID = id; }
	int Get_telopID(){ return telopID; }
	void Append_telop(int id);


	//===============================================
	//�@GetSet
	//===============================================
	bool isStart(){ return isYooiDon; }
	void SetManholeFade(MANHOLE_FADE_TYPE type);
	MANHOLE_FADE_TYPE GetManholeFade(){ return m_fade_type; }
	void Set_rend_command(REND_COMMAND com){ command = com; }

	//===============================================
	//	���[�h�ύX
	//===============================================
	void Change_mode(int m);
};


extern UI *ui;