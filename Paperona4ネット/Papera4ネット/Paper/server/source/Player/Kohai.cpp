
#include "../../IEX/iextreme.h"
#include "BasePlayer.h"
#include "Kohai.h"
#include "../poster/Poster_manager.h"

Kohai::Kohai(int id) : BasePlayer(id)
{
	for (unsigned int i = 0; i < (unsigned int)ACTION_PART::MAX; i++)action[i] = nullptr;
}

Kohai::~Kohai()
{
	Release();
}

void Kohai::Release()
{
	for (int i = 0; i < (int)ACTION_PART::MAX; i++)
	{
		SAFE_DELETE(action[i]);
	}
	BasePlayer::Release();
}

void Kohai::Initialize(iex3DObj *obj)
{
	BasePlayer::Initialize(obj);

	// 行動状態初期化
	action[(int)ACTION_PART::CHASE] = new Kohai::ActionChase(this);

	action_part = ACTION_PART::CHASE;
}

void Kohai::Update()
{
	action[(unsigned int)action_part]->Update();
}

int Kohai::Get_action()
{
	return (int)action_part;
}

void Kohai::Change_action(int part)
{
	action_part = (ACTION_PART)part;
}


//**************************************************
/// AI
//**************************************************

// 追いかける

void Kohai::ActionChase::Initialize()
{

}

void Kohai::ActionChase::Update()
{
	const int dist_player(50); // これより離れると近づく

	Vector3 ppos;
	player[me->mynumber]->Get_pos(ppos);

	Vector3 to_player(ppos - me->pos);
	float l(to_player.Length());
	to_player /= l;
	if (l > dist_player)
	{
		me->pos += to_player * me->speed;
	}
}

// 貼る

int Kohai::ActionPaste::Nearest_poster(float dist)
{
	// distより近いポスターに貼りに行く
	float l(dist * dist);
	for (int i = 0; i < poster_mng->Get_numof(); i++)
	{
		l = min(l, (poster_mng->Get_pos(i) - me->pos).LengthSq());
	}
	// 無かった
	if (l > dist * dist) return -1;


}
