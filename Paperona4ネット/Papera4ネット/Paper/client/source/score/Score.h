
#pragma once

#include "iextreme.h"

class Score
{
private:
	int score[PLAYER_MAX];
public:
	Score(){ for (int i = 0; i < PLAYER_MAX; i++)score[i] = 0; }

	void Add(int point, int number);

	void Render();
};

extern Score *score;
