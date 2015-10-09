
#include "Score.h"
#include "iextreme.h"
#include <assert.h>

void Score::Add(int in, TEAM_COLOR color)
{
	assert(color != TEAM_COLOR::NONE);
	score[(int)color] += in;
}

void Score::Render()
{
	char	str[64];
	wsprintf(str, "SCORE ONE %d", score[(int)TEAM_COLOR::ONE]);
	IEX_DrawText(str, 10, 64, 200, 20, 0xFFFF0000);

	wsprintf(str, "SCORE TWO %d", score[(int)TEAM_COLOR::TWO]);
	IEX_DrawText(str, 10, 96, 200, 20, 0xFF0000FF);
}
