#pragma once

/*	ベースプレイヤー	*/

#include "../../../share_data/Enum_public.h"

//	操作デスク
struct	CONTROL_DESC
{
	BYTE	moveFlag; //　上下左右に動かすフラグ

	float	mouseX; //マウス
	float	mouseY;

	BYTE	motion_no;// モーションの番号

	BYTE	rendFlag;	// 破くときに使う
	BYTE	controlFlag;// ボタン
};

#define TYPE1
//#define TYPE2

class BasePlayer
{
protected:
	//===============================================
	//	基本パラメータ
	//===============================================
	Vector3			pos;
	float			angleY;
	float			scale;

	Vector3			move;
	float			speed;
	float			fallspeed;
	int				se_receive;

	iex3DObj		*model;

	int				motion_no;

	bool			isLand;
	bool			isJump;
	float			jump_pow;
	bool			invincible;


	//===============================================
	//	その他
	//===============================================
	int				poster_num;

	int mynumber;


	//===============================================
	//	プレイヤー行動委譲クラス(インナークラス)
	//===============================================
	template<typename ME> // 自分のクラス
	class ActionBase
	{
	protected:
		ME *me;

	public:
		ActionBase(ME* me) :me(me){}
		virtual void Initialize(){}
		virtual void Update(const CONTROL_DESC &_ControlDesc){}
		virtual void Render(){}

		void Set_motion(int no);
	};

	int		m_id;

public:
	//===============================================
	//	初期化と解放
	//===============================================
	BasePlayer(){}
	BasePlayer(int id);
	~BasePlayer();
	virtual void Initialize(iex3DObj *obj);
	virtual void Release();


	//===============================================
	//	更新と描画
	//===============================================
	virtual void Update();
	virtual void Render();


	//===============================================
	//	ゲッター,セッター
	//===============================================
	// 座標
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// アングル
	float Get_angleY(){ return angleY; }

	Vector3 Get_forward(){ return Vector3(model->TransMatrix._31, model->TransMatrix._32, model->TransMatrix._33); }

	// プレイヤーのモード
	virtual int Get_action() = 0;
	virtual void Change_action(int part) = 0;

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

	bool isInvincible(){ return invincible; }

};

namespace PlayerManager
{
	int Check_attack(int me);
}



extern BasePlayer* player[PLAYER_MAX];