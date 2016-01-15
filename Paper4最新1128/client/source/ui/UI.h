#pragma once

class BasePlayer;
class Pie_graph;
class Pie_graph_content;
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
	int step;
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
	//	何かの実体
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Pie_graph_content *gauge;

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
	void Telop_render();// テロップ
	void Manhole_fade();// マンホールのフェード

	//===============================================
	//	テロップリスト
	//===============================================
	std::list<Telop*> List;


	// く
	bool isYooiDon;

public:
	static float tape_len;	// テロップテープの長さ(0～1)

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
	void SetManholeFade(MANHOLE_FADE_TYPE type);
	MANHOLE_FADE_TYPE GetManholeFade(){ return m_fade_type; }

	//===============================================
	//	モード変更
	//===============================================
	void Change_mode(int m);
};


extern UI *ui;