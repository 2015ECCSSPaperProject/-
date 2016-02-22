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

	BasePlayer *players[PLAYER_MAX];						// プレイヤー実体
	iex3DObj *clone_model[(int)CLONE_TYPE::MAX];			// clone元

public:

	//===============================================
	//	初期化と解放
	//===============================================
	PlayerManager();
	~PlayerManager();
	void Initialize();
	void Release();

	//===============================================
	//	更新と描画
	//===============================================
	void Update();
	void Render(iexShader *shader = nullptr, char *name = '\0');
	void Render_forword();
	void Render_rush();
	//===============================================
	//	エフェクトの更新と描画
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