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
		REND_CALENDAR,
		REND_MONEY,
		REND_SIGN,
		REND_SHINBUN,
		REND_WC_PAPER,
		REND_ZASSHI,
		REND_MAGAZINE,
		REND_SEISHO,
		REND_SHOJI,
		REND_FLYER,
		REND_PANEL,
		START,
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
	void Initialize();
	void Release();

	//===============================================
	//	�X�V�ƕ`��
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');
	void Render_forword();
	void Render_rush();
	//===============================================
	//	�G�t�F�N�g�̍X�V�ƕ`��
	//===============================================
	void EffectRender();

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