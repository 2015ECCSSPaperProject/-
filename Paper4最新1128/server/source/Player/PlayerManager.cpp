#include	"iextreme.h"


#include	"../../../share_data/Enum_public.h"

#include	"BasePlayer.h"
#include	"NetPlayer.h"
#include	"PlayerManager.h"

PlayerManager::PlayerManager()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i] = nullptr;
	}
	
	for (int i = 0; i < (int)CLONE_TYPE::MAX; i++)
	{
		clone_model[i] = nullptr;
	}
}

PlayerManager::~PlayerManager()
{
	Release();
}

void PlayerManager::Initialize()
{
	// クローン
	clone_model[(int)CLONE_TYPE::NORMAL] = new iex3DObj("DATA/CHR/player/run.IEM");
	clone_model[(int)CLONE_TYPE::DIE] = new iex3DObj("DATA/CHR/player/die.IEM");
	clone_model[(int)CLONE_TYPE::GUN] = new iex3DObj("DATA/CHR/player/gun.IEM");

	iex3DObj *objs[(int)CLONE_TYPE::MAX] =
	{
		clone_model[(int)CLONE_TYPE::NORMAL],	// 通常モデル
		clone_model[(int)CLONE_TYPE::DIE],		// 死ぬ用モデル
		clone_model[(int)CLONE_TYPE::GUN]		// 鉄砲モデル
	};

	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		players[i] = new NetPlayer(i);
		players[i]->Initialize(objs);
	}


}

void PlayerManager::Release()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		SAFE_DELETE(players[i]);
	}

	for (int i = 0; i < (int)CLONE_TYPE::MAX; i++)
	{
		delete clone_model[i], clone_model[i] = nullptr;
	}
}

void PlayerManager::Update()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i]->Update();
	}
}

void PlayerManager::Render()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i]->Render();
		Text::Draw(1000, 50 + (i * 40), 0xffff00ff, "pos.x->%.2f", players[i]->Get_pos().x);
	}
}

int PlayerManager::Check_attack(int me)
{
	int no = -1;
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (i == me)continue;
		else if (players[i]->isInvincible())continue;

		Vector3 vec(players[i]->Get_pos() - players[me]->Get_pos());

		// 距離
		if (vec.Length() < 16)
		{
			vec.Normalize();
			Vector3 front(sinf(players[me]->Get_angleY()), 0, cosf(players[me]->Get_angleY()));

			// 角度
			if (Vector3Dot(vec, front) > .707f)
			{
				no = i;
				break;
			}
		}
	}
	return no;
}

void PlayerManager::Check_gun_attack(int me, float radius)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (i == me || players[i]->isInvincible())continue;

		Vector3 vec(players[i]->Get_pos() - players[me]->Get_pos());

		// 距離
		if (vec.Length() < radius)
		{
			players[i]->Change_action(BasePlayer::ACTION_PART::DIE);
		}
	}
}


PlayerManager *player_mng;