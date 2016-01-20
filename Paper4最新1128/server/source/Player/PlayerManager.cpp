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

void PlayerManager::Init_pos()
{
	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		players[i]->Init_pos();
	}
}

void PlayerManager::Initialize()
{
	// �N���[��
	clone_model[(int)CLONE_TYPE::NORMAL] = new iex3DObj("DATA/CHR/player/run.IEM");
	clone_model[(int)CLONE_TYPE::DIE] = new iex3DObj("DATA/CHR/player/die.IEM");
	clone_model[(int)CLONE_TYPE::GUN] = new iex3DObj("DATA/CHR/player/gun.IEM");
	clone_model[(int)CLONE_TYPE::SYURIKEN] = new iex3DObj("DATA/CHR/player/syuriken/syuriken.IEM");

	iex3DObj *objs[(int)CLONE_TYPE::MAX] =
	{
		clone_model[(int)CLONE_TYPE::NORMAL],	// �ʏ탂�f��
		clone_model[(int)CLONE_TYPE::DIE],		// ���ʗp���f��
		clone_model[(int)CLONE_TYPE::GUN],		// �S�C���f��
		clone_model[(int)CLONE_TYPE::SYURIKEN]	// �藠�����f��
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

int PlayerManager::Check_attack(int me, float dist)
{
	int no = -1;
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (i == me)continue;
		else if (players[i]->isInvincible())continue;

		Vector3 vec(players[i]->Get_pos() - players[me]->Get_pos());

		// ����
		if (vec.LengthSq() < dist*dist)
		{
			vec.Normalize();
			Vector3 front(sinf(players[me]->Get_angleY()), 0, cosf(players[me]->Get_angleY()));

			// �p�x
			if (Vector3Dot(vec, front) > .114f)
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

		// ����
		if (vec.Length() < radius)
		{
			players[i]->Change_action(BasePlayer::ACTION_PART::DIE);
		}
	}
}


PlayerManager *player_mng;