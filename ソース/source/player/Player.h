
#pragma once

class Poster_manager;
class Stage;
#include	"../Enum_public.h"

class Player
{
public:

	//===============================================
	//	定数
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_FPS, ATTACK, PASTE, REND, DIE, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };

private:

	//===============================================
	//	基本パラメータ
	//===============================================
	Vector3			pos;
	Vector3			angle;
	float			scale;

	Vector3			move;
	float			speed;
	float			fallspeed;
	int				se_receive;

	iex3DObj		*model;

	Poster_manager	*poster_mng;
	TEAM_COLOR		team_col;

	//===============================================
	//	その他
	//===============================================
	int				poster_num;
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;


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
			Player *me;

		public:
			Base(Player* me) :me(me){}
			virtual void Initialize(){}
			virtual void Update(){}
			virtual void Render(){}

			// 3DObj更新
			void Update_obj();

			void Set_motion(int no);
		};

		//===========================================
		//	移動状態(TPS)
		class Move : public Base
		{
		public:
			Move(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	移動状態(FPS)
		class MoveFPS : public Base
		{
		public:
			MoveFPS(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	攻撃状態
		class Attack : public Base
		{
		public:
			Attack(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	ポスター貼り付け状態
		class Paste : public Base
		{
		public:
			Paste(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	ポスター破り状態
		class Rend : public Base
		{
		public:
			Rend(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};

		//===========================================
		//	死んでる状態
		class Die : public Base
		{
		private:
			int die_frame;

		public:
			Die(Player*me) :Base(me){}

			void Initialize();
			void Update();
			void Render();
		};
	};


	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// 現在のプレイヤーのモード
	
	DO_FLAG do_flag;			// Zキー押したら何をするか
	

	//===============================================
	//	テクスチャ変更
	//===============================================
	void Set_texture(char *filename);
	void Change_color()
	{
		team_col = (team_col == TEAM_COLOR::ONE) ? TEAM_COLOR::TWO : TEAM_COLOR::ONE;
	}


public:
	//===============================================
	//	初期化と解放
	//===============================================
	Player();
	~Player();
	void Initialize(iex3DObj *obj, Poster_manager *poster_mng, TEAM_COLOR col);
	void Release();


	//===============================================
	//	更新と描画
	//===============================================
	void Update(Stage *stage);
	void Render();


	//===============================================
	//	ゲッター,セッター
	//===============================================

	// 座標
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// アングル
	Vector3 Get_angle(){ return angle; }
	void Get_angle(Vector3 &out){ out = angle; }
	void Set_angle(const Vector3 &a){ angle = a; }
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

};
