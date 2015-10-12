#pragma once

/*	ベースプレイヤー	*/

class BasePlayer
{
public:

	//	操作デスク
	struct	CONTROL_DESC{

		BYTE moveFlag;			//	発砲フラグ

	};
	CONTROL_DESC m_controlDesc;//　コントロール

protected:

	iexMesh2* obj;
	Vector3 pos;
	Vector3 move;



public:
	BasePlayer();
	~BasePlayer();

	virtual void Update();
	virtual void Render();

	//ゲッター・セッター
	Vector3 GetPos(){ return pos; };
	void SetPos(Vector3 p){ pos = p; }
	const CONTROL_DESC& GetDesc()const{ return	m_controlDesc; }
};

extern BasePlayer* player[PLAYER_MAX];