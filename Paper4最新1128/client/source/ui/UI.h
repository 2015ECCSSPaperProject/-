#pragma once

class BasePlayer;
class Pie_graph;
class Pie_graph_content;
class SceneMain;

#include<list>

class Telop
{
public:
	bool erase;		// �����Ă�
	Telop();
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
		ACTION,
		TAPE,
		TAPE_BAR,
		SKILL_GUN,
		SKILL_SYURIKEN,
		SKILL_KABUTO,
		MANHOLE_FADE,
		MAX
	};
	enum class MANHOLE_FADE_TYPE
	{
		NONE, F_IN, F_OUT, MAX
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

	//===============================================
	//	�����̎���
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Pie_graph_content *gauge;

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
			iex2DObj *sokomade;
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

	//===============================================
	//	���[�h�ύX
	//===============================================
	void Change_mode(int m);
};


extern UI *ui;