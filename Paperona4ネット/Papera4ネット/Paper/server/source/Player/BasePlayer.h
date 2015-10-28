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
public:
	//==============================================================================
	// 追加たち
	//===============================================
	//	定数
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, DIE, RESPAWN, PLANE, GUN, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };

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
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;

	int mynumber;


	//===============================================
	//	プレイヤー行動委譲クラス(インナークラス)
	//===============================================
		//===========================================
		//	状態基底クラス
	class ActionBase
		{
		protected:
			BasePlayer *me;

		public:
			ActionBase(BasePlayer* me) :me(me){}
			virtual void Initialize(){}
			virtual void Update(const CONTROL_DESC &_ControlDesc){}
			virtual void Render(){}

			void Set_motion(int no);
		};

	//===========================================
		//	移動状態(TPS)
	class ActionMove : public ActionBase
		{
		public:
			ActionMove(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	移動状態(FPS)
	class ActionMoveFPS : public ActionBase
		{
		public:
			ActionMoveFPS(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	攻撃状態
	class ActionAttack : public ActionBase
		{
		public:
			ActionAttack(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	ポスター貼り付け状態
	class ActionPaste : public ActionBase
		{
		private:
			int timer;
		public:
			ActionPaste(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	ポスター破り状態
	class ActionRend : public ActionBase
		{
		private:
			bool rended;
		public:
			ActionRend(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	死んでる状態
	class ActionDie : public ActionBase
		{
		private:
			BYTE die_frame;
			BYTE flashing;

		public:
			ActionDie(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	リスポーン状態
	class ActionRespawn : public ActionBase
		{
		private:
			BYTE invincible_time;
			BYTE flashing;

		public:
			ActionRespawn(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	紙ひこーき状態
	class ActionHikouki : public ActionBase
		{
		public:
			ActionHikouki(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

	//===========================================
		//	紙鉄砲状態
	class ActionGun : public ActionBase
		{
		public:
			ActionGun(BasePlayer*me) :ActionBase(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};


	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	ActionBase *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// 現在のプレイヤーのモード

	DO_FLAG do_flag;			// Zキー押したら何をするか


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
	ACTION_PART Get_action()
	{
		return action_part; 
	}
	void Change_action(ACTION_PART part)
	{
		if (action_part > ACTION_PART::MAX)
		{
			MessageBox(0, "サーバーのアクション取得中の値がおかしい", null, MB_OK);
		}
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Zキーで何をするか
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

	bool isInvincible(){ return invincible; }

};

namespace PlayerManager
{
	int Check_attack(int me);
}



extern BasePlayer* player[PLAYER_MAX];