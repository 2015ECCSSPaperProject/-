
#include "Score.h"
#include "iextreme.h"
#include "../pie_graph/pie_graph.h"
#include <assert.h>

Score::Score()
{
	for (int i = 0; i < PLAYER_MAX; i++)score[i] = 0;

	graph = new Pie_graph;

	graph->Add_content("DATA/UI/score/black.png");
	graph->Add_content("DATA/UI/score/blue.png");
	graph->Add_content("DATA/UI/score/green.png");
	graph->Add_content("DATA/UI/score/purple.png");
	graph->Add_content("DATA/UI/score/red.png");
	graph->Add_content("DATA/UI/score/white.png");
}

Score::~Score()
{
	delete graph;
}

void Score::Add(int in, int number)
{
	assert(0 <= number && number < PLAYER_MAX);
	score[(int)number] += in;
}

void Score::Render()
{
	for (int i = 0; i < PLAYER_MAX; i++)graph->Set_percent(i, score[i]);
	graph->Render(0, 0, 256, 256, 0, 0, 2, 2);
}

Score *score = nullptr;
