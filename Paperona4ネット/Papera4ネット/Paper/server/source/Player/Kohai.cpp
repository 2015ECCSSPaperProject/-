
#include "../../IEX/iextreme.h"
#include "BasePlayer.h"
#include "Kohai.h"
#include "../poster/Poster_manager.h"

Kohai::Kohai(int id) : BasePlayer(id), action_chase(this), action_paste(this)
{
	for (unsigned int i = 0; i < (unsigned int)ACTION_PART::MAX; i++)action[i] = nullptr;
}

Kohai::~Kohai()
{
	Release();
}

void Kohai::Release()
{
	BasePlayer::Release();
}

void Kohai::Initialize(iex3DObj *obj)
{
	BasePlayer::Initialize(obj);

	// 行動状態初期化
	action[(int)ACTION_PART::CHASE] = &action_chase;
	action[(int)ACTION_PART::PASTE] = &action_paste;

	action_part = ACTION_PART::CHASE;
}

void Kohai::Update()
{
	action[(unsigned int)action_part]->Update();

	pos += move;
	move = Vector3(0, 0, 0);
}

int Kohai::Get_action()
{
	return (int)action_part;
}

void Kohai::Change_action(int part)
{
	action_part = (ACTION_PART)part;
	action[(int)action_part]->Initialize();
}


//**************************************************
/// AI
//**************************************************

// 追いかける

Kohai::ActionChase::ActionChase(Kohai *me) :ActionBase(me){}

void Kohai::ActionChase::Initialize()
{

}

void Kohai::ActionChase::Update()
{
	const int dist_player(50); // これより離れると近づく
	const float dist_poster(50); // これより近くにあると貼りに行く
	// 移動
	// playerの近くにいる時
	if (!me->GotoA_far(player[me->mynumber]->Get_pos(), dist_player))
	{
		// ポスター貼りに行く
		int poster = me->action_paste.Nearest_poster(dist_poster);
		if (poster >= 0)
		{
			me->Change_action((int)ACTION_PART::PASTE);
		}
	}
}

// 貼る

Kohai::ActionPaste::ActionPaste(Kohai *me) :ActionBase(me){}

void Kohai::ActionPaste::Initialize()
{
	process = PROCESS::WALK;
	timer = 0;
}

void Kohai::ActionPaste::Update()
{
	switch (process)
	{
	case Kohai::ActionPaste::PROCESS::WALK:
		if (!me->GotoA_far(poster_mng->Get_pos(target_poster), 10))
		{
			const float dist(5.0f); // ポスターから離れる距離
			me->pos = poster_mng->Get_pos(target_poster);
			me->angleY = poster_mng->Get_angle(target_poster) + PI;
			me->pos += Vector3(-sinf(me->angleY), 0, -cosf(me->angleY)) * dist;

			process = PROCESS::PASTE;
		}
		break;

	case Kohai::ActionPaste::PROCESS::PASTE:

		me->motion_no = 3;
		if (timer++ > 60)
		{
			me->motion_no = 0;
			me->Change_action((int)ACTION_PART::CHASE);
		}

		if (timer == 45)
		{
			// 貼り付ける処理
			poster_mng->Paste_poster(me->mynumber, target_poster);
		}
		break;

	default:
		break;
	}
}

int Kohai::ActionPaste::Nearest_poster(float dist)
{
	// distより近いポスターに貼りに行く
	float nearl(dist * dist);
	int index = -1;
	float l;
	for (int i = 0; i < poster_mng->Get_numof(); i++)
	{
		// 貼れない
		if (!poster_mng->Can_paste(me->mynumber, i))
			continue;
		// プレイヤーから一番近いの
		l = (poster_mng->Get_pos(i) - player[me->mynumber]->Get_pos()).LengthSq();
		if (l < nearl)
		{
			nearl = l;
			index = i;
			target_poster = i;
		}
	}

	return index;
}

// いろいろ

void Kohai::GotoA(const Vector3 &A)
{
	Vector3 toA(A - pos);
	toA.Normalize();
	move = toA * speed;
}

bool Kohai::GotoA_far(const Vector3 &A, float dist)
{
	Vector3 toA = A - pos;
	float l = toA.LengthSq();
	if (l < dist * dist)
		return false;

	l = sqrtf(l);
	toA /= l;
	move = toA * speed;
	return true;
}

bool Kohai::GotoA_near(const Vector3 &A, float dist)
{
	Vector3 toA = A - pos;
	float l = toA.LengthSq();
	if (l > dist * dist)
		return false;

	l = sqrtf(l);
	toA /= l;
	move = toA * speed;

	return true;
}
