
#pragma once

class iex2DObj;

class Skill_gauge
{
public:
	Skill_gauge();
	~Skill_gauge();

	void Initialize();
	void Render(int num, // ó≠Ç‹Ç¡ÇƒÇÈêî
		int num_use); // égÇ§êî

private:
	struct Textures
	{
		Textures();
		~Textures();

		enum
		{
			NUM_PAPER = 10,
			NUM_SKILLMARK = 3
		};
		iex2DObj *back;
		iex2DObj *dot;
		iex2DObj *paper[10];
		iex2DObj *skill_mark[3];
	}textures;
	struct DATA
	{
		struct f2{ int x, y; };

		f2 BACKPOS, BACKSIZE;
		int PAPERSIZE, PAPERDIST;
		f2 PAPERSTART;

		DATA();
		~DATA();
	};
	const DATA data;
};
