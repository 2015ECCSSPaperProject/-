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
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, DIE, MAX };
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
	class Action
	{
	public:

		//===========================================
		//	状態基底クラス
		class Base
		{
		protected:
			BasePlayer *me;

		public:
			Base(BasePlayer* me) :me(me){}
			virtual void Initialize(){}
			virtual void Update(const CONTROL_DESC &_ControlDesc){}
			virtual void Render(){}

			void Set_motion(int no);
		};

		//===========================================
		//	移動状態(TPS)
		class Move : public Base
		{
		public:
			Move(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	移動状態(FPS)
		class MoveFPS : public Base
		{
		public:
			MoveFPS(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	攻撃状態
		class Attack : public Base
		{
		public:
			Attack(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	ポスター貼り付け状態
		class Paste : public Base
		{
		private:
			int timer;
		public:
			Paste(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	ポスター破り状態
		class Rend : public Base
		{
		private:
			bool rended;
		public:
			Rend(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	死んでる状態
		class Die : public Base
		{
		private:
			int die_frame;

		public:
			Die(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	紙ひこーき状態
		class Hikouki : public Base
		{
		public:
			Hikouki(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	紙鉄砲状態
		class Gun : public Base
		{
		public:
			Gun(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};
	};


	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
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
	ACTION_PART Get_action(){ return action_part; }
	void Change_action(ACTION_PART part)
	{
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Zキーで何をするか
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

};

extern BasePlayer* player[PLAYER_MAX];