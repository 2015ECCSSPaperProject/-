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
	clone_model[(int)CLONE_TYPE::NORMAL]		 = new iex3DObj("DATA/CHR/player/run.IEM");
	clone_model[(int)CLONE_TYPE::DIE]			 = new iex3DObj("DATA/CHR/player/die.IEM");
	clone_model[(int)CLONE_TYPE::GUN]			 = new iex3DObj("DATA/CHR/player/gun/gun.IEM");
	clone_model[(int)CLONE_TYPE::SYURIKEN]		 = new iex3DObj("DATA/CHR/player/syuriken/syuriken.IEM");
	clone_model[(int)CLONE_TYPE::REND_CALENDAR]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_calendar.IEM");
	clone_model[(int)CLONE_TYPE::REND_MONEY]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_money.IEM");
	clone_model[(int)CLONE_TYPE::REND_SIGN]		 = new iex3DObj("DATA/CHR/player/rend_motion/animation_sign.IEM");
	clone_model[(int)CLONE_TYPE::REND_SHINBUN]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_sinbun.IEM");
	clone_model[(int)CLONE_TYPE::REND_WC_PAPER]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_toile.IEM");
	clone_model[(int)CLONE_TYPE::REND_ZASSHI]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_zassi.IEM");
	clone_model[(int)CLONE_TYPE::REND_MAGAZINE]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_magazin.IEM");
	clone_model[(int)CLONE_TYPE::REND_SEISHO]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_seisyo.IEM");
	clone_model[(int)CLONE_TYPE::REND_SHOJI]	 = new iex3DObj("DATA/CHR/player/rend_motion/animation_syoji.IEM");

	// セットするテクスチャ
	Texture2D *textures[3][6] =
	{
		// 普通のモデル用カラー
		{
			iexTexture::Load("DATA/CHR/player/player_red.png"),
			iexTexture::Load("DATA/CHR/player/player_blue.png"),
			iexTexture::Load("DATA/CHR/player/player_yellow.png"),
			iexTexture::Load("DATA/CHR/player/player_green.png"),
			iexTexture::Load("DATA/CHR/player/player_purple.png"),
			iexTexture::Load("DATA/CHR/player/player_pink.png")
		},

		// 鉄砲モデル用カラー
		{
			iexTexture::Load("DATA/CHR/player/gun/teppou_red.png"),
			iexTexture::Load("DATA/CHR/player/gun/teppou_blue.png"),
			iexTexture::Load("DATA/CHR/player/gun/teppou_yellow.png"),
			iexTexture::Load("DATA/CHR/player/gun/teppou_green.png"),
			iexTexture::Load("DATA/CHR/player/gun/teppou_purple.png"),
			iexTexture::Load("DATA/CHR/player/gun/teppou_pink.png")
		},

		// 手裏剣モデル用カラー
		{
			iexTexture::Load("DATA/CHR/player/syuriken/syuriken_red.png"),
			iexTexture::Load("DATA/CHR/player/syuriken/syuriken_blue.png"),
			iexTexture::Load("DATA/CHR/player/syuriken/syuriken_yellow.png"),
			iexTexture::Load("DATA/CHR/player/syuriken/syuriken_green.png"),
			iexTexture::Load("DATA/CHR/player/syuriken/syuriken_purple.png"),
			iexTexture::Load("DATA/CHR/player/syuriken/syuriken_pink.png")
		}
	};
	enum TEX_TYPE{ NORMAL, GUN, SYURIKEN };

	//■■■　相手と自分で分ける
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		if (i == SOCKET_MANAGER->GetID()){
			players[i] = new MyPlayer();
			camera->Initialize(players[i]);	// カメラ設定
		}
		else
			players[i] = new NetPlayer();

		players[i]->Initialize(clone_model);

		assert(i < 6);
		players[i]->Set_texture(BasePlayer::MODEL::NORMAL, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::DIE, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::PLANE, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::GUN, textures[TEX_TYPE::GUN][i]);
		players[i]->Set_texture(BasePlayer::MODEL::SYURIKEN, textures[TEX_TYPE::SYURIKEN][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_CALENDAR, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_MAGAZINE, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_MONEY, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_SHINBUN, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_SIGN, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_WC_PAPER, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_ZASSHI, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::REND_SEISHO, textures[TEX_TYPE::NORMAL][i]);
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
		players[i]->Set_action((BasePlayer::ACTION_PART)sendPlayer.action_part);
		players[i]->Set_god_gage(sendPlayer.god_gage);
		players[i]->Set_poster_num(sendPlayer.target_no);
		players[i]->Set_state_flag(sendPlayer.stateFlag);
		players[i]->Update();
	}
}

void PlayerManager::Render(iexShader *shader, char *name)
{

	if ((false)?false:shader)
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			//Vector3 v = players[i]->Get_pos() - Vector3(matView._41, matView._42, matView._43);
			//v.Normalize();
			//if (Vector3Dot(v, Vector3(matView._13, matView._23, matView._33)) < .707f) continue;
			players[i]->Render(shader, name);
		}
	}else
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			//Vector3 v = players[i]->Get_pos() - Vector3(matView._41, matView._42, matView._43);
			//v.Normalize();
			//if (Vector3Dot(v, Vector3(matView._13, matView._23, matView._33)) < .707f) continue;
			players[i]->Render();
			//Text::Draw(1100, 20 + (i * 32), 0xff00ffff, "pos.x->%.2f", players[i]->Get_pos().x);

			//Text::Draw(950, 20 + (i * 32), 0xff00ffff, "名前：%s", SOCKET_MANAGER->GetUser(i).name);
		}
	}
}

void PlayerManager::Render_forword()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i]->Render_forword();
	}
}

void PlayerManager::EffectRender()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i]->EffectRender();
	}
}



PlayerManager *player_mng;