
#include "skill_gauge.h"
#include "../../IEX/iextreme.h"

Skill_gauge::DATA::DATA()
{
	BACKPOS = { 32, 620 };
	BACKSIZE = { 512, 80 };
	PAPERSIZE = 64;
	PAPERDIST = 48;
	PAPERSTART = { (int)((BACKSIZE.y - PAPERSIZE) * 0.5f), (int)((BACKSIZE.y - PAPERSIZE) * 0.5f) };
}

Skill_gauge::DATA::~DATA()
{}



Skill_gauge::Textures::Textures() :back(nullptr), dot(nullptr)
{
	for (int i = 0; i < NUM_PAPER; i++) paper[i] = nullptr;
	for (int i = 0; i < NUM_SKILLMARK; i++) skill_mark[i] = nullptr;
}

Skill_gauge::Textures::~Textures()
{
	delete back;
	delete dot;
	for (int i = 0; i < NUM_PAPER; i++)delete paper[i];
	for (int i = 0; i < NUM_SKILLMARK; i++)delete skill_mark[i];
}



Skill_gauge::Skill_gauge(){}

Skill_gauge::~Skill_gauge(){}

void Skill_gauge::Initialize()
{
	textures.back = new iex2DObj("DATA/UI/skill/black.png");

	textures.dot = new iex2DObj("DATA/UI/skill/black.png");

	textures.paper[0] = new iex2DObj("DATA/UI/skill/1.png");
	textures.paper[1] = new iex2DObj("DATA/UI/skill/2.png");
	textures.paper[2] = new iex2DObj("DATA/UI/skill/3.png");
	textures.paper[3] = new iex2DObj("DATA/UI/skill/4.png");
	textures.paper[4] = new iex2DObj("DATA/UI/skill/5.png");
	textures.paper[5] = new iex2DObj("DATA/UI/skill/6.png");
	textures.paper[6] = new iex2DObj("DATA/UI/skill/1.png");
	textures.paper[7] = new iex2DObj("DATA/UI/skill/2.png");
	textures.paper[8] = new iex2DObj("DATA/UI/skill/3.png");
	textures.paper[9] = new iex2DObj("DATA/UI/skill/4.png");
}

void Skill_gauge::Render(int num, // —­‚Ü‚Á‚Ä‚é”
	int num_use)
{
	textures.back->Render(data.BACKPOS.x, data.BACKPOS.y, data.BACKSIZE.x, data.BACKSIZE.y, 0, 0, 2, 2);

	for (int i = 0; i < textures.NUM_PAPER; i++)
		textures.dot->Render(data.BACKPOS.x + data.PAPERSTART.x + data.PAPERDIST*i, data.BACKPOS.y + data.PAPERSTART.y, data.PAPERSIZE, data.PAPERSIZE, 0, 0, 8, 8);

	assert(num <= textures.NUM_PAPER);
	for (int i = 0; i < num; i++)
	{
		if (num_use > num || num_use < num - i)
		{
			textures.paper[i]->Render(
				data.BACKPOS.x + data.PAPERSTART.x + data.PAPERDIST*i,
				data.BACKPOS.y + data.PAPERSTART.y,
				data.PAPERSIZE,
				data.PAPERSIZE,
				0, 0, 2, 2);
		}
		else
		{
			textures.paper[i]->Render(
				data.BACKPOS.x + data.PAPERSTART.x + data.PAPERDIST*i - 8,
				data.BACKPOS.y + data.PAPERSTART.y - 8,
				data.PAPERSIZE + 16,
				data.PAPERSIZE + 16,
				0, 0, 2, 2);
		}
	}
}
