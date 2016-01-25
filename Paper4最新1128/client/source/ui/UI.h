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
	bool erase;		// けしてね
	Telop(int id);
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
		RU,	// 右上
		RD,	// 右下
		LU,	// 左上
		LD,	// 左下
		ROLL	// 回転
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


	// コマンド矢印
	REND_COMMAND command;


	//===============================================
	//	何かの実体
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Pie_graph_content *gauge;


	//===============================================
	//	アニメーションUI
	//===============================================
	AnimationRippleEx* C_Five;
	AnimationRippleEx* C_Four;
	AnimationRippleEx* C_Three;
	AnimationRippleEx* C_Two;
	AnimationRippleEx* C_One;
	int CountTimeNo;

	int TimerX;			//Timerを動かす
	int SkillX;			//Skillを動かす
	int GraphX;			//Skillを動かす
	// スキル溜まったら
	static const int SKILL_MAX = 3;
	AnimationRippleEx*  SkileSphere[SKILL_MAX];
	AnimationRippleEx*  SkileRip[SKILL_MAX];
	float savePercent[SKILL_MAX];// 過去のパーセントを持っている 0.0f~1.0fまで
	//AnimationRippleEx*  SkileSphere_2;
	//AnimationRippleEx*  SkileSphere_3;
	AnimationRippleEx* action_hold;
	AnimationRippleEx* action_drag;

	AnimationRippleEx* DragRip; //ドラッグ
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
	//	それぞれ描画の関数
	//===============================================
	// 関数たち
	void Graph();		// 上ら辺の円グラフ
	void SkillGauge();	// 左下のスキルゲージ
	void Action();		// 右下のアクションUI
	void TimeLimit();	// タイムリミット
	void Telop_render();// テロップ
	void Manhole_fade();// マンホールのフェード
	void Command();

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
	void Render_mark();

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
	void Set_rend_command(REND_COMMAND com){ command = com; }

	//===============================================
	//	モード変更
	//===============================================
	void Change_mode(int m);
};


extern UI *ui;