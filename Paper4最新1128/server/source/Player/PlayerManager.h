#pragma once

class BasePlayer;

class PlayerManager
{
public:
	//===============================================
	//	定数
	//===============================================
	enum class CLONE_TYPE
	{
		NORMAL,		// モデル
		DIE,		// 破れるモデル
		GUN,		// 鉄砲
		SYURIKEN,	// 手裏剣
		MAX
	};

private:

	BasePlayer *players[PLAYER_MAX];						// プレイヤー実体
	iex3DObj *clone_model[(int)CLONE_TYPE::MAX];			// clone元

public:

	//===============================================
	//	初期化と解放
	//===============================================
	PlayerManager();
	~PlayerManager();
	void Init_pos();
	void Initialize();
	void Release();

	//===============================================
	//	更新と描画
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	判定
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