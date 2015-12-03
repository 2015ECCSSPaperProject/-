#pragma once

class BasePlayer;
class Pie_graph;
class Skill_gauge;
class SceneMain;

#include<list>

class Telop
{
public:
	bool erase;		// けしてね
	Telop();
	~Telop();
	void Update();
	void Render();

private:
	int app_timer;	// 表示時間
	iex2DObj *moji;
};

class UI
{
public:
	//===============================================
	//	定数
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
		SKILL_ZENRYOKU,
		MAX
	};

private:
	int telopID;
	int seted_ID;

	//===============================================
	//	何かの実体
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Skill_gauge *gauge;

	//===============================================
	//	委譲
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
	//	それぞれ描画の関数
	//===============================================
	// 関数たち
	void Graph();		// 上ら辺の円グラフ
	void SkillGauge();	// 左下のスキルゲージ
	void Action();		// 右下のアクションUI
	void TimeLimit();	// タイムリミット


	//===============================================
	//	テロップリスト
	//===============================================
	std::list<Telop*> List;


	// く
	bool isYooiDon;

public:
	//===============================================
	//	初期化と解放
	//===============================================
	UI();
	void Initialize(BasePlayer *my);
	~UI();

	//===============================================
	//	更新と描画
	//===============================================
	void Update();
	void Render();


	//===============================================
	//	テロップ追加
	//===============================================
	void Set_telopID(int id){ telopID = id; }
	int Get_telopID(){ return telopID; }
	void Append_telop(int id);


	//===============================================
	//　GetSet
	//===============================================
	bool isStart(){ return isYooiDon; }


	//===============================================
	//	モード変更
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