
#pragma once

#include "iextreme.h"

//******************************************************************************
//		スコア管理クラス(Singleton)
//******************************************************************************

class Pie_graph;

class Score
{
public:
	//=========================================
	//	インスタンス取得
	static Score *getInstance()
	{
		static Score i;
		return &i;
	}

	void Initialize();
	~Score(){}

	void Add(int point, int number);
	void Set(int point, int number);
	int Get(int number);

	void Update();
	//void Render();

private:
	int score[PLAYER_MAX];
	Score(){ Initialize(); }
	Score(const Score&){}
	Score &operator=(const Score&){}
};

#define score_mng ( (Score::getInstance()) )

//extern Score *score;
