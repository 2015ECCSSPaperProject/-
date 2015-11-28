
#include "Score.h"
#include "iextreme.h"
#include <assert.h>

void Score::Add(int in, int number)
{
	assert(0 <= number && number < PLAYER_MAX);
	score[number] += in;
}

void Score::Render()
{
}

Score *score = nullptr;
