#pragma once
/*	ネットプレイヤー達	*/

/*前方宣言*/
class BasePlayer;

//********************************************************************************************************************
//		net
//****************************************************************************************************************
class	NetPlayer : public	BasePlayer
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
	//	委譲クラスへのポインタ
	//===============================================
	//ActionBase *action[(unsigned int)ACTION_PART::MAX];
	ACTION_PART action_part;	// 現在のプレイヤーのモード

	DO_FLAG do_flag;			// Zキー押したら何をするか


	//===============================================
	//	その他
	//===============================================
	enum class CAMERA_MODE{ TPS, FPS };
	CAMERA_MODE		camera_mode;

public:
	NetPlayer(int id);
	~NetPlayer();
	void Initialize(iex3DObj *obj);
	void Release();

	void Update();
	void Render();



	//===========================================
	//	移動状態(TPS)
	class ActionMove : public ActionBase<NetPlayer>
	{
	public:
		ActionMove(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	移動状態(FPS)
	class ActionMoveFPS : public ActionBase<NetPlayer>
	{
	public:
		ActionMoveFPS(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	攻撃状態
	class ActionAttack : public ActionBase<NetPlayer>
	{
	public:
		ActionAttack(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	ポスター貼り付け状態
	class ActionPaste : public ActionBase<NetPlayer>
	{
	private:
		int timer;
	public:
		ActionPaste(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	ポスター破り状態
	class ActionRend : public ActionBase<NetPlayer>
	{
	private:
		bool rended;
	public:
		ActionRend(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	死んでる状態
	class ActionDie : public ActionBase<NetPlayer>
	{
	private:
		BYTE die_frame;
		BYTE flashing;

	public:
		ActionDie(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	リスポーン状態
	class ActionRespawn : public ActionBase<NetPlayer>
	{
	private:
		BYTE invincible_time;
		BYTE flashing;

	public:
		ActionRespawn(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	紙ひこーき状態
	class ActionHikouki : public ActionBase<NetPlayer>
	{
	public:
		ActionHikouki(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	//===========================================
	//	紙鉄砲状態
	class ActionGun : public ActionBase<NetPlayer>
	{
	public:
		ActionGun(NetPlayer*me) :ActionBase(me){}

		void Initialize();
		void Update(const CONTROL_DESC &_ControlDesc);
	};

	ActionBase<NetPlayer> **action;



	// プレイヤーのモード
	int Get_action()override
	{
		return (int)action_part;
	}
	void Change_action(int part) override
	{
		Change_action((ACTION_PART)part);
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

};
