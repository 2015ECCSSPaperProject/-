#pragma once

class BasePlayer;

class PlayerManager
{
public:
	//===============================================
	//	’è”
	//===============================================
	enum class CLONE_TYPE
	{
		NORMAL,		// ƒ‚ƒfƒ‹
		DIE,		// ”j‚ê‚éƒ‚ƒfƒ‹
		GUN,		// “S–C
		SYURIKEN,	// Žè— Œ•
		MAX
	};

private:

	BasePlayer *players[PLAYER_MAX];						// ƒvƒŒƒCƒ„[ŽÀ‘Ì
	iex3DObj *clone_model[(int)CLONE_TYPE::MAX];			// cloneŒ³

public:

	//===============================================
	//	‰Šú‰»‚Æ‰ð•ú
	//===============================================
	PlayerManager();
	~PlayerManager();
	void Init_pos();
	void Initialize();
	void Release();

	//===============================================
	//	XV‚Æ•`‰æ
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	”»’è
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