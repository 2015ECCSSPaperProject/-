#pragma once

class BasePlayer;
class Pie_graph;
class Skill_gauge;

class UI
{
public:
	//===============================================
	//	定数
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
	//	何かの実体
	//===============================================
	BasePlayer *my_player;
	iex2DObj *image[IMAGE::MAX];
	Pie_graph *graph;
	Skill_gauge *gauge;

	//===============================================
	//	それぞれ描画の関数
	//===============================================
	// 関数たち
	void Graph();		// 上ら辺の円グラフ
	void SkillGauge();	// 左下のスキルゲージ
	void Action();		// 右下のアクションUI
	void TimeLimit();	// タイムリミット

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
};

extern UI *ui;