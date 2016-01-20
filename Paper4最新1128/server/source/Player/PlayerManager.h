#pragma once

class BasePlayer;

class PlayerManager
{
public:
	//===============================================
	//	�萔
	//===============================================
	enum class CLONE_TYPE
	{
		NORMAL,		// ���f��
		DIE,		// �j��郂�f��
		GUN,		// �S�C
		SYURIKEN,	// �藠��
		MAX
	};

private:

	BasePlayer *players[PLAYER_MAX];						// �v���C���[����
	iex3DObj *clone_model[(int)CLONE_TYPE::MAX];			// clone��

public:

	//===============================================
	//	�������Ɖ��
	//===============================================
	PlayerManager();
	~PlayerManager();
	void Init_pos();
	void Initialize();
	void Release();

	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	����
	//===============================================
	int Check_attack(int me, float dist);
	void Check_gun_attack(int me, float radius);


	//===============================================
	//	Get&Set
	//===============================================
	BasePlayer *Get_player(int no)
	{
		assert(no >= 0 && no < PLAYER_MAX);
		return players[no];
	}


};

extern PlayerManager *player_mng;