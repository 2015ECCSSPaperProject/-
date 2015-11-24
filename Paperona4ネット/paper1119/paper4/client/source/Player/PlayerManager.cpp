#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
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
	// �N���[��
	clone_model[(int)CLONE_TYPE::NORMAL] = new iex3DObj("DATA/CHR/player/run.IEM");
	clone_model[(int)CLONE_TYPE::DIE] = new iex3DObj("DATA/CHR/player/die.IEM");
	clone_model[(int)CLONE_TYPE::GUN] = new iex3DObj("DATA/CHR/player/gun.IEM");

	// �Z�b�g����e�N�X�`��
	Texture2D *textures[2][6] =
	{
		// ���ʂ̃��f���p�J���[
		{
			iexTexture::Load("DATA/CHR/player/player_red.png"),
			iexTexture::Load("DATA/CHR/player/player_blue.png"),
			iexTexture::Load("DATA/CHR/player/player_yellow.png"),
			iexTexture::Load("DATA/CHR/player/player_green.png"),
			iexTexture::Load("DATA/CHR/player/player_purple.png"),
			iexTexture::Load("DATA/CHR/player/player_pink.png")
		},

		// �S�C���f���p�J���[
		{
			iexTexture::Load("DATA/CHR/player/teppou_red.png"),
			iexTexture::Load("DATA/CHR/player/teppou_blue.png"),
			iexTexture::Load("DATA/CHR/player/teppou_yellow.png"),
			iexTexture::Load("DATA/CHR/player/teppou_green.png"),
			iexTexture::Load("DATA/CHR/player/teppou_purple.png"),
			iexTexture::Load("DATA/CHR/player/teppou_pink.png")
		}
	};
	enum TEX_TYPE{ NORMAL, GUN };

	iex3DObj *objs[(int)CLONE_TYPE::MAX] =
	{
		clone_model[(int)CLONE_TYPE::NORMAL],	// �ʏ탂�f��
		clone_model[(int)CLONE_TYPE::DIE],		// ���ʗp���f��
		clone_model[(int)CLONE_TYPE::GUN]		// �S�C���f��
	};

	//�������@����Ǝ����ŕ�����
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		if (i == SOCKET_MANAGER->GetID()){
			players[i] = new MyPlayer();
			camera->Initialize(players[i]);	// �J�����ݒ�
		}
		else
			players[i] = new NetPlayer();

		players[i]->Initialize(objs);

		assert(i < 6);
		players[i]->Set_texture(BasePlayer::MODEL::NORMAL, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::DIE, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::PLANE, textures[TEX_TYPE::NORMAL][i]);
		players[i]->Set_texture(BasePlayer::MODEL::GUN, textures[TEX_TYPE::GUN][i]);

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
		//�@�����̂Ȃ����ԏd�v
		//�@�����Ă��������e�v���C���[�ɃZ�b�g�I�I
		PlayerData sendPlayer = SOCKET_MANAGER->GetPlayer(i);
		players[i]->Set_pos(sendPlayer.pos);
		players[i]->Set_angleY(sendPlayer.angleY);

		players[i]->Set_motion_no(sendPlayer.motion_no);	// �����ŋA���Ă������[�V�����̔ԍ����Z�b�g
		players[i]->Set_aciton((BasePlayer::ACTION_PART)sendPlayer.action_part);
		players[i]->Set_god_gage(sendPlayer.god_gage);
		players[i]->Update();
	}
}

void PlayerManager::Render()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		players[i]->Render();
		//Text::Draw(1100, 20 + (i * 32), 0xff00ffff, "pos.x->%.2f", players[i]->Get_pos().x);

		//Text::Draw(950, 20 + (i * 32), 0xff00ffff, "���O�F%s", SOCKET_MANAGER->GetUser(i).name);
	}
}

PlayerManager *player_mng;