
#pragma once

#include "iextreme.h"

class Score
{
private:
	int telopID;
	int score[PLAYER_MAX];
public:
	Score():telopID(-1){ for (int i = 0; i < PLAYER_MAX; i++)score[i] = 0; }

	void Add(int in, int number);
	int Get_score(int number)
	{
		assert(0 <= number && number < PLAYER_MAX);
		return score[number];
	}
	void Set_telopID(int id){ telopID = id; }
	int Get_telopID(){ return telopID; }

	void Render();
};

extern Score *score;
