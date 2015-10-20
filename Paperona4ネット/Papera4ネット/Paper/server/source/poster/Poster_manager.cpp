
#include "Poster_manager.h"
#include "Poster.h"
#include "iextreme.h"
#include "../../../share_data/Enum_public.h"
#include "Poster_frame.h"

#include "../Player/BasePlayer.h"
#include "../fstream/fstream_paper.h"
#include "../../IEX/IEX_Math2.h"

void Poster_manager::Load_poster_pos(char *filename)
{
	std::ifstream infs(filename);

	Poster *p(nullptr);
	float angle(0);
	Vector3 pos(0, 0, 0);
	int point(0);
	this->number_of_posters = 0;
	while (!infs.eof())
	{
		infs >> angle;
		angle = Degree_to_radian(angle);
		infs >> pos;
		infs >> point;
		p = new Poster;
		p->Initialize(poster_model, score, point);
		p->Set_pose(angle, pos);
		posters.push_back(p);
		this->number_of_posters++;
	}
}



Poster_manager::Poster_manager() : poster_model(nullptr), frame(nullptr)
{
	posters.resize(0);
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

	// ここでポスターの配置を決める
	Load_poster_pos("DATA/MATI/poster_pos.txt");
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
	static int user_change_timer(0);
	if (300 < user_change_timer++)
	{
		user_change_timer = 0;
		int number = rand() % (PLAYER_MAX - 1);
		posters[rand() % number_of_posters]->Change_user(number);
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



int Poster_manager::Can_do(BasePlayer *player, int number)
{
	int num = -1;

	for (unsigned int i = 0; i < posters.size(); i++)
	{
		if (posters[i]->Can_do(player, number))
		{
			num = i;
		}
	}

	return num;
}

bool Poster_manager::Can_rend(int number, int poster_num)
{
	return posters[poster_num]->Can_rend(number);
}

bool Poster_manager::Can_paste(int number, int poster_num)
{
	return posters[poster_num]->Can_paste(number);
}

void Poster_manager::Rend_poster(int number, int poster_num)
{
	posters[poster_num]->Rend(number);
}

void Poster_manager::Paste_poster(int number, int poster_num)
{
	posters[poster_num]->Paste(number);
}



int Poster_manager::Get_number(int index)
{
	return posters[index]->Get_number();
}

int Poster_manager::Get_animation_frame(int index)
{
	return posters[index]->Get_animation_frame();
}



const Vector3& Poster_manager::Get_pos(int index)
{
	return posters[index]->Get_pos();
}

float Poster_manager::Get_angle(int index)
{
	return posters[index]->Get_angle();
}



Poster_manager *poster_mng = nullptr;
