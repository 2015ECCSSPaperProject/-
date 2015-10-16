
#pragma once

#include "../Enum_public.h"

class Score
{
private:
	int score[(int)TEAM_COLOR::NONE];
public:
	Score(){ score[(int)TEAM_COLOR::ONE] = 0; score[(int)TEAM_COLOR::TWO] = 0; }

	void Add(int in, TEAM_COLOR color);

	void Render();
};

extern Score *score;
