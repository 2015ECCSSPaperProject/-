#pragma once

/*	ベースプレイヤー(クライアント)	*/

#define TYPE1
//#define TYPE2

class BasePlayer
{
public:

	//	操作デスク
	struct	CONTROL_DESC{

		BYTE moveFlag;

		float	mouseX;			//　マウスX
		float	mouseY;			//　マウスY

		BYTE motion_no;			// モーション番号

		BYTE rendFlag;			// 破く時に送る
		BYTE controlFlag;		// ボタン

	};
	CONTROL_DESC m_controlDesc;//　コントロール


	//==============================================================================
	// 追加たち
	//===============================================
	//	定数
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_TARGET, ATTACK, PASTE, REND, FREEZE, DIE, RESPAWN, PLANE, GUN, MANHOLE, THROUGH, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };
	enum class MODEL{ NORMAL, DIE, PLANE, GUN, MAX };

protected:
	//===============================================
	//	基本パラメータ
	//===============================================
	Vector3			pos;
	float			angleY;
	float			scale;

	Vector3			move;
	Vector3			prev_pos;
	float			speed;
	float			fallspeed;
	int				se_receive;

	iex3DObj		*models[(int)MODEL::MAX];

	POINT			mousePos;		//	マウス座標

	int				motion_no;
	int				god_gage;

	//===============================================
	//	その他
	//===============================================
	int				poster_num;
	MODEL			model_part;
	bool			toggle_c;


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
			virtual void Update(){}
			virtual void Render(iexShader *shader = nullptr, char *name = '\0'){}

			// 3DObj更新
			void Update_obj();
		};

		//===========================================
		//	移動状態(TPS)
		class Move : public Base
		{
		public:
			Move(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	移動状態(ターゲッティング時)
		class MoveTarget : public Base
		{
		public:
			MoveTarget(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	攻撃状態
		class Attack : public Base
		{
		public:
			Attack(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
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
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
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
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	固まってる状態
		class Freeze : public Base
		{
		public:
			Freeze(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	死んでる状態
		class Die : public Base
		{
		private:
			int flashing;

		public:
			Die(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	リスポーン状態
		class Respawn : public Base
		{
		private:
			BYTE flashing;

		public:
			Respawn(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	紙ひこーき状態
		class Hikouki : public Base
		{
		public:
			Hikouki(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	紙鉄砲状態
		class Gun : public Base
		{
		public:
			Gun(BasePlayer*me) :Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	マンホール潜る
		class Manhole : public Base
		{
		public:
			Manhole (BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	狭い壁通り抜け
		class Through : public Base
		{
		public:
			Through(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};


	};


	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	Action::Base *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// 現在のプレイヤーのモード

	DO_FLAG do_flag;			// Zキー押したら何をするか


public:
	//===============================================
	//	初期化と解放
	//===============================================
	BasePlayer();
	virtual ~BasePlayer();
	virtual void Initialize(iex3DObj **objs);
	virtual void Release();


	//===============================================
	//	更新と描画
	//===============================================
	virtual void Update();
	virtual void Render(iexShader *shader = nullptr, char *name = '\0');

	//===============================================
	//	ゲッター,セッター
	//===============================================
	const CONTROL_DESC& GetDesc()const{ return	m_controlDesc; }

	// 座標
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	Vector3 Get_center_pos(){ return Vector3(pos.x, pos.y + 5.0f, pos.z); }
	void Set_pos(const Vector3 &p){ pos = p; }

	// アングル
	float Get_angleY(){ return angleY; }
	//void Get_angle(Vector3 &out){ out = angle; }
	void Set_angleY(float p){ angleY = p; }
	Vector3 Get_forward(){ return Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33); }

	// プレイヤーのモード
	ACTION_PART Get_action(){ return action_part; }
	void Set_aciton(ACTION_PART part)
	{
		if (action_part != part)
		{
			Change_action(part);
		}
	}
	void Change_action(ACTION_PART part)
	{
		action_part = part;
		action[(unsigned int)part]->Initialize();
	}

	// Zキーで何をするか
	DO_FLAG Get_do_flag(){ return do_flag; }
	void Set_do_flag(DO_FLAG flag){ do_flag = flag; }

	void Set_motion_no(int no)
	{
		motion_no = no; 
	}
	int Get_motion_no(){ return motion_no; }
	void Set_motion(int no);


	void Set_texture(MODEL type, Texture2D *tex)
	{
		models[(int)type]->SetTexture(tex, 0);
	}

	int Get_god_gage(){ return god_gage; }
	void Set_god_gage(int val){ god_gage = val; }

	CONTROL_DESC Get_controlDesc(){ return m_controlDesc; }

	int Get_poster_num(){ return poster_num; }
	void Set_poster_num(int no){ poster_num = no; }

};

//extern BasePlayer* player[PLAYER_MAX];