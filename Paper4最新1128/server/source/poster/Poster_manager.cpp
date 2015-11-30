
#include "Poster_manager.h"
#include "Poster.h"
#include "iextreme.h"
#include "../../../share_data/Enum_public.h"
//#include "Poster_frame.h"

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
		p->Set_number(0);	// 最初から全部貼ってる
		posters.push_back(p);
		this->number_of_posters++;
	}
}



Poster_manager::Poster_manager() : poster_model(nullptr)
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
	//frame = new Poster_frame;
	//frame->Initialize(new iexMesh("DATA/Poster/waku.IMO"));

	// ここでポスターの配置を決める
	Load_poster_pos("DATA/MATI/poster_pos.txt");
}

void Poster_manager::Release()
{
	delete poster_model; poster_model = nullptr;
	//delete frame; frame = nullptr;
	for (auto it : posters) delete it; posters.resize(0);
}

void Poster_manager::Update()
{
	for (auto it = posters.begin(); it != posters.end(); it++)
	{
		if ((*it)->Get_number() == PLAYER_MAX) continue;
		(*it)->Update();
	}

	// テスト
	//static int user_change_timer(0);
	//if (300 < user_change_timer++)
	//{
	//	user_change_timer = 0;
	//	int number = rand() % (PLAYER_MAX - 1);
	//	posters[rand() % number_of_posters]->Change_user(number);
	//}
}

void Poster_manager::Render()
{
	for (auto it : posters)
	{
		it->Render();
		//frame->Render(it->Get_pos(), it->Get_angle());
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

int Poster_manager::Can_do(BasePlayer *player)
{
	int num = -1;

	for (unsigned int i = 0; i < posters.size(); i++)
	{
		if (posters[i]->Can_do(player))
		{
			num = i;
		}
	}

	return num;
}

int Poster_manager::Can_dist(const Vector3 &pos, float dist)
{
	float min_dist = 114514;
	int ret_num = -1;
	for (unsigned int i = 0; i < posters.size(); i++)
	{
		if (posters[i]->Can_dist(pos, dist))
		{
			const float dist = (posters[i]->Get_pos() - pos).Length();
			if (dist < min_dist)	// 最も近い位置
			{
				min_dist = dist;
				ret_num = i;
			}
		}
	}
	return ret_num;
}

void Poster_manager::Can_dist(const Vector3 &pos, float dist, int number, int out[])
{
	int count = 0;
	for (unsigned int i = 0; i < posters.size(); i++)
	{
		if (posters[i]->Can_dist(pos, dist, number))
		{
			out[count++] = i;
		}
	}
	out[count] = -1;	// 終端は-1
}

void Poster_manager::Can_dist(const Vector3 &pos, float dist, int out[])
{
	int count = 0;
	for (unsigned int i = 0; i < posters.size(); i++)
	{
		if (posters[i]->Can_dist(pos, dist))
		{
			out[count++] = i;
		}
	}
	out[count] = -1;	// 終端は-1
}

int Poster_manager::Can_targeting(BasePlayer *player, float range_dist, int range_degree)
{
	float min_dist = range_dist;
	int ret_num = -1;

	for(unsigned int i = 0; i < posters.size(); i++)
	{
		//④v1とv2のなす角（度）を計算し、変数angleに代入する
		Vector3 player_front(sinf(player->Get_angleY()), 0, cosf(player->Get_angleY()));
		Vector3 to_target_vec(posters[i]->Get_pos() - player->Get_pos());

		const float to_target_len = to_target_vec.Length();
		const int theta = (int)(acosf(Vector3Dot(player_front, to_target_vec) / (player_front.Length() * to_target_len)) / 0.01745f);

		if (theta < range_degree && to_target_len < range_dist)	// 距離と角度
		{
			if (to_target_len < min_dist)
			{
				ret_num = i;
				min_dist = to_target_len;
			}
		}
	}
	return ret_num;
}

bool Poster_manager::Can_rend(int number, int poster_num)
{
	return posters[poster_num]->Can_rend(number);
}

bool Poster_manager::Can_rend(int poster_num)
{
	return posters[poster_num]->Can_rend();
}

bool Poster_manager::Can_paste(int number, int poster_num)
{
	return posters[poster_num]->Can_paste(number);
}

void Poster_manager::Rend_poster(int number, int poster_num)
{
	posters[poster_num]->Rend();
}

void Poster_manager::Rend_poster(int poster_num)
{
	posters[poster_num]->Rend();
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
