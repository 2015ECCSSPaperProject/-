
#include "Poster_manager.h"
#include "Poster.h"
#include "iextreme.h"
#include "../Enum_public.h"
#include "Poster_frame.h"

Poster_manager::Poster_manager() : poster_model(nullptr), frame(nullptr)
{
	posters.resize(0);
	// ポスターの種類
	for (unsigned int i = 0; i < (int)TEAM_COLOR::NONE; i++)
	{
		poster_textures[i] = nullptr;
	}
}

Poster_manager::~Poster_manager()
{
	Release();
}

void Poster_manager::Initialize(Score *score)
{
	this->score = score;

	poster_model = new iex3DObj("DATA/Poster/posuta-.IEM");
	frame = new Poster_frame;
	frame->Initialize(new iexMesh("DATA/Poster/waku.IMO"));

	poster_textures[(int)TEAM_COLOR::ONE] = iexTexture::Load("DATA/Poster/red.png");
	poster_textures[(int)TEAM_COLOR::TWO] = iexTexture::Load("DATA/Poster/blue.png");

	// ここでポスターの配置を決める
	number_of_posters = 5;

	Poster *p;

	p = new Poster;
	p->Initialize(poster_model, score, 1);
	p->Set_pose(PI, Vector3(0, 0, 50));
	posters.push_back(p);

	p = new Poster;
	p->Initialize(poster_model, score, 1);
	p->Set_pose(PI, Vector3(50, 0, 50));
	posters.push_back(p);

	p = new Poster;
	p->Initialize(poster_model, score, 1);
	p->Set_pose(PI, Vector3(-50, 0, 50));
	posters.push_back(p);

	p = new Poster;
	p->Initialize(poster_model, score, 1);
	p->Set_pose(PI, Vector3(100, 0, 50));
	posters.push_back(p);

	p = new Poster;
	p->Initialize(poster_model, score, 1);
	p->Set_pose(PI, Vector3(-100, 0, 50));
	posters.push_back(p);

}

void Poster_manager::Release()
{
	delete poster_model; poster_model = nullptr;
	delete frame; frame = nullptr;
	for (auto it : posters) delete it; posters.resize(0);
}

void Poster_manager::Update()
{
	for (auto it : posters)
	{
		it->Update();
	}

	// テスト
	static int color_change_timer(0);
	if (300 < color_change_timer++)
	{
		color_change_timer = 0;
		posters[rand() % 4]->Change_color(TEAM_COLOR::TWO, poster_textures[(int)TEAM_COLOR::TWO]);
	}
}

void Poster_manager::Render()
{
	for (auto it : posters)
	{
		it->Render();
		frame->Render(it->Get_pos(), it->Get_angle());
	}
}



int Poster_manager::Can_do(BasePlayer *player, TEAM_COLOR color)
{
	int num = -1;

	for (unsigned int i = 0; i < posters.size(); i++)
	{
		if (posters[i]->Can_do(player, color))
		{
			num = i;
		}
	}

	return num;
}

bool Poster_manager::Can_rend(TEAM_COLOR color, int poster_num)
{
	return posters[poster_num]->Can_rend(color);
}

bool Poster_manager::Can_paste(TEAM_COLOR color, int poster_num)
{
	return posters[poster_num]->Can_paste(color);
}

void Poster_manager::Rend_poster(TEAM_COLOR color, int poster_num)
{
	posters[poster_num]->Rend(color);
}

void Poster_manager::Paste_poster(TEAM_COLOR color, int poster_num)
{
	posters[poster_num]->Paste(color, poster_textures[(int)color]);
}



void Poster_manager::Set_color(int index, TEAM_COLOR color)
{
	posters[index]->Change_color(color, poster_textures[(int)color]);
}
void Poster_manager::Set_animframe(int index, int frame)
{
	posters[index]->Set_anim_frame(frame);
}

Poster_manager *poster_mng;
