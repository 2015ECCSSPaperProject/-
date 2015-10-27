
#pragma once

#include "iextreme.h"

class Pie_graph;

class Score
{
private:
	int score[PLAYER_MAX];
	Pie_graph *graph;
public:
	Score();
	~Score();

	void Add(int point, int number);

	void Render();
};

extern Score *score;
