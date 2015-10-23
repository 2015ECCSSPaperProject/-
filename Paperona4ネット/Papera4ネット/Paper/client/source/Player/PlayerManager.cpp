#include	"iextreme.h"

//netは一番初めにインクルードしないとエラーが起こる
#include	"../Net/Socket.h"

#include	"../../../share_data/Enum_public.h"

#include	"../camera/Camera.h"
#include	"BasePlayer.h"
#include	"MyPlayer.h"
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


	// セットするテクスチャ
	Texture2D *textures[6] =
	{
		iexTexture::Load("DATA/CHR/player/player_red.png"),
		iexTexture::Load("DATA/CHR/player/player_blue.png"),
		iexTexture::Load("DATA/CHR/player/player_yellow.png"),
		iexTexture::Load("DATA/CHR/player/player_green.png"),
		iexTexture::Load("DATA/CHR/player/player_purple.png"),
		iexTexture::Load("DATA/CHR/player/player_pink.png")
	};

	//■■■　相手と自分で分ける
	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		if (i == SOCKET_MANAGER->GetID()){
			players[i] = new MyPlayer();
			camera->Initialize(players[i]);	// カメラ設定
		}
		else
			players[i] = new NetPlayer();

		players[i]->Initialize(clone_model[(int)CLONE_TYPE::NORMAL], clone_model[(int)CLONE_TYPE::DIE]);

		if (i < 6)
		{
			players[i]->Set_texture(textures[i]);
		}
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
		//　ここのながれ一番重要
		//　送られてきた情報を各プレイヤーにセット！！
		PlayerData sendPlayer = SOCKET_MANAGER->GetPlayer(i);
		players[i]->Set_pos(sendPlayer.pos);
		players[i]->Set_angleY(sendPlayer.angleY);

		players[i]->Set_motion_no(sendPlayer.motion_no);	// ここで帰ってきたモーションの番号をセット
		players[i]->Set_aciton((BasePlayer::ACTION_PART)sendPlayer.action_part);
		players[i]->Update();
	}
}

void PlayerManager::Render()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i]->Render();
		Text::Draw(1100, 20 + (i * 32), 0xff00ffff, "pos.x->%.2f", players[i]->Get_pos().y);

		Text::Draw(950, 20 + (i * 32), 0xff00ffff, "名前：%s", SOCKET_MANAGER->GetUser(i).name);
	}
}

PlayerManager *player_mng;