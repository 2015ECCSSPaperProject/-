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


	// �Z�b�g����e�N�X�`��
	Texture2D *textures[6] =
	{
		iexTexture::Load("DATA/CHR/player/player_red.png"),
		iexTexture::Load("DATA/CHR/player/player_blue.png"),
		iexTexture::Load("DATA/CHR/player/player_yellow.png"),
		iexTexture::Load("DATA/CHR/player/player_green.png"),
		iexTexture::Load("DATA/CHR/player/player_purple.png"),
		iexTexture::Load("DATA/CHR/player/player_pink.png")
	};

	//�������@����Ǝ����ŕ�����
	for (int i = 0; i <PLAYER_MAX; ++i)
	{
		if (i == SOCKET_MANAGER->GetID()){
			players[i] = new MyPlayer();
			camera->Initialize(players[i]);	// �J�����ݒ�
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
		//�@�����̂Ȃ����ԏd�v
		//�@�����Ă��������e�v���C���[�ɃZ�b�g�I�I
		PlayerData sendPlayer = SOCKET_MANAGER->GetPlayer(i);
		players[i]->Set_pos(sendPlayer.pos);
		players[i]->Set_angleY(sendPlayer.angleY);

		players[i]->Set_motion_no(sendPlayer.motion_no);	// �����ŋA���Ă������[�V�����̔ԍ����Z�b�g
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

		Text::Draw(950, 20 + (i * 32), 0xff00ffff, "���O�F%s", SOCKET_MANAGER->GetUser(i).name);
	}
}

PlayerManager *player_mng;