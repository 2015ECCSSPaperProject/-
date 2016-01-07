#pragma once

/*	ベースプレイヤー	*/

#include "../../../share_data/Enum_public.h"

//	操作デスク
struct	CONTROL_DESC
{
	BYTE	moveFlag;	// 上下左右に動かすフラグ

	float	mouseX;		// マウス
	float	mouseY;

	BYTE	motion_no;	// モーションの番号

	BYTE	rendFlag;	// 破くときに使う
	BYTE	controlFlag;// ボタン
	BYTE	skillFlag;	// スキル発動フラグ
};

#define TYPE1

class PaperQueue;

class BasePlayer
{
public:
	PaperQueue *paperqueue;

	//==============================================================================
	// 追加たち
	//===============================================
	//	定数
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_TARGET, ATTACK, PASTE, REND, FREEZE, DIE, RESPAWN, PLANE, GUN, MANHOLE, THROUGH, SYURIKEN, TRANS_FORM, REND_OBJ, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };
	enum class MODEL{ NORMAL, DIE, PLANE, GUN, SYURIKEN, MAX };
	enum class SKILL{ GUN, SYURIKEN, KABUTO, ZENRYOKU, MAX };

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

	iex3DObj		*models[(int)MODEL::MAX];

	int				motion_no;

	bool			isLand;
	bool			isJump;
	bool			attackFlag;	// 空中で撃てるのは1回だけ
	float			jump_pow;
	bool			invincible;

	int				god_gage;	// 神ゲージ(これを紙を破った枚数にする)


	//===============================================
	//	スキルゲージ
	//===============================================
	struct
	{
		//bool unlock;			// 解禁してるかどうか
		//int unlock_rend_count;	// 解禁に必要な紙を破る枚数
		//int wait_time;			// 0ならスキル撃てる 1以上ならデクリメント
		//int cool_time;			// 使用後に待つ時間(固定値)
		ACTION_PART do_action;	// 発動アクション
	}skill_data[(int)SKILL::MAX];

	SKILL select_skill;

	//void Check_unlock(int rend_count)
	//{
	//	for (int i = (int)ACTION_PART::MAX - 1; i >= 0; i--)
	//	{
	//		if (skill_data[i].unlock) break;
	//		if (rend_count >= skill_data[i].unlock_rend_count) skill_data[i].unlock = true;
	//	}
	//}

	//===============================================
	//	その他
	//===============================================
	int				poster_num;
	MODEL			model_part;

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
		private:
			bool trg_target;
		public:
			Move(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	移動状態(ターゲット)
		class MoveTarget : public Base
		{
		public:
			MoveTarget(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	攻撃状態
		class Attack : public Base
		{
		private:
			int target_no;

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
		//	固まってる状態
		class Freeze : public Base
		{
		public:
			Freeze(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	死んでる状態
		class Die : public Base
		{
		private:
			BYTE die_frame;
			BYTE flashing;

		public:
			Die(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	リスポーン状態
		class Respawn : public Base
		{
		private:
			BYTE invincible_time;
			BYTE flashing;

		public:
			Respawn(BasePlayer*me) :Base(me){}

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

		//===========================================
		//	マンホール潜る
		class Manhole : public Base
		{
		public:
			Manhole(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	狭い壁通り抜け
		class Through : public Base
		{
		public:
			Through(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	手裏剣
		class Syuriken : public Base
		{
		private:
			int syurikentaimaa;
			float max_speed;
			float accel;
			float kasoku;
			bool r;
			Vector3 move_vec;
		public:
			Syuriken(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	返信
		class TransForm : public Base
		{
		private:
			int hensintaimaa;
		public:
			TransForm(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};

		//===========================================
		//	対小物の破る
		class RendObj : public Base
		{
		private:
			int rend_timer;	// Initiailzeでこの時間を設定して、この時間が過ぎたら解除

		public:
			RendObj(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update(const CONTROL_DESC &_ControlDesc);
		};
	};


	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;		// 現在のプレイヤーのモード
	ACTION_PART reserve_action;		// 変身したときの予約

	DO_FLAG do_flag;				// Zキー押したら何をするか


	int		m_id;

public:
	//===============================================
	//	初期化と解放
	//===============================================
	BasePlayer(){}
	BasePlayer(int id);
	virtual ~BasePlayer();
	virtual void Init_pos();
	virtual void Initialize(iex3DObj **objs);
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
	void Set_angleY(float a){ angleY = a; }

	Vector3 Get_forward(){ return Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33); }

	// プレイヤーのモード
	ACTION_PART Get_action()
	{
		return action_part; 
	}
	void Change_action(ACTION_PART part)
	{
		if (action_part > ACTION_PART::MAX)
		{
			//MessageBox(0, "サーバーのアクション取得中の値がおかしい", null, MB_OK);
		}
		action_part = part;
		action[(int)part]->Initialize();
	}

	// Zキーで何をするか
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);

	bool isInvincible(){ return invincible; }

	int Get_god_gage(){ return god_gage; }

	int Get_target_no(){return poster_num; }
};