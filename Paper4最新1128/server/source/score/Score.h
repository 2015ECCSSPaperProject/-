
#pragma once

#include "iextreme.h"

class Score
{
private:
	int score[PLAYER_MAX];
public:
	Score(){ for (int i = 0; i < PLAYER_MAX; i++)score[i] = 0; }

	void Add(int in, int number);
	int Get_score(int number)
	{
		assert(0 <= number && number < PLAYER_MAX);
		return score[number];
	}

	void Render();
};

extern Score *score;
