
#include "Score.h"
#include "iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"

#include <assert.h>

void Score::Initialize()
{
	for (int i = 0; i < PLAYER_MAX; i++)score[i] = 0;
}

void Score::Add(int in, int number)
{
	assert(0 <= number && number < PLAYER_MAX);
	score[(int)number] += in;
}

void Score::Set(int in, int number)
{
	assert(0 <= number && number < PLAYER_MAX);
	score[(int)number] = in;
}

int Score::Get(int number)
{
	assert(0 <= number && number < PLAYER_MAX);
	return score[number];
}

void Score::Update()
{
}


Score *score = nullptr;
