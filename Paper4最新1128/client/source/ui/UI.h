#pragma once

class BasePlayer;
class Pie_graph;
class Skill_gauge;
class SceneMain;

class UI
{
public:
	//===============================================
	//	�萔
	//===============================================
	enum IMAGE
	{
		TEROP,
		NUMBER,
		ACTION,
		TAPE,
		TAPE_BAR,
		SKILL_GUN,
		SKILL_SYURIKEN,
		SKILL_KABUTO,
		SKILL_ZENRYOKU,
		MAX
	};

private:
	//===============================================
	//	�����̎���
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Skill_gauge *gauge;

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


	// ��
	bool isYooiDon;

public:
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
	//�@GetSet
	//===============================================
	bool isStart(){ return isYooiDon; }

	//===============================================
	//	���[�h�ύX
	//===============================================
	void Change_mode(SceneMain::MODE m)
	{
		SAFE_DELETE(mode);
		switch (m)
		{
		case SceneMain::MODE::START:
			mode = new Mode::Start(this);
			break;

		case SceneMain::MODE::MAIN:
			mode = new Mode::Main(this);
			break;

		case SceneMain::MODE::END:
			mode = new Mode::End(this);
			break;
		}
		mode->Initialize();
	}
};


extern UI *ui;