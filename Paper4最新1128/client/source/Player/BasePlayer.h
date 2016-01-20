#pragma once
#include "../Animation/AnimationUV.h" 
#include "../Explosion/Explosion.h"
class AnimationUV;
class Explosion;
class Barrier;
class Rush;
class SkillBegin;
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
		BYTE skillFlag;			// スキル発動フラグ
	};
	CONTROL_DESC m_controlDesc;//　コントロール


	//==============================================================================
	// 追加たち
	//===============================================
	//	定数
	//===============================================
	enum class ACTION_PART{ MOVE, MOVE_TARGET, ATTACK, REND, FREEZE, DIE, RESPAWN, GUN, MANHOLE, THROUGH, SYURIKEN, TRANS_FORM, REND_OBJ, MAX };
	enum class DO_FLAG{ NONE, ATTACK, PASTE, REND, MAX };
	enum class MODEL
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
		MAX
	};
	enum class SKILL{ GUN, SYURIKEN, KABUTO, MAX };

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
	int				se_receive2;

	iex3DObj		*models[(int)MODEL::MAX];

	POINT			mousePos;		//	マウス座標
	int				motion_no;
	int				god_gage;

	bool			isMyNunber;
	int kind_paper_obj;			// 破ってる小物の種類(-1が何も破っていない状態)ヘッダーのインクルードの問題があるのでint型にする

	BYTE			 stateFlag;	// 状態
	bool			isBarrier;

	//===============================================
	//	スキルゲージ
	//===============================================
	struct
	{
		bool unlock;			// 解禁してるかどうか
		int unlock_rend_count;	// 解禁に必要な紙を破る枚数
		int wait_time;			// 0ならスキル撃てる 1以上ならデクリメント
		int cool_time;			// 使用後に待つ時間(固定値)
		ACTION_PART do_action;	// 発動アクション
	}skill_data[(int)SKILL::MAX];

	int select_skill;	// ++とかするのでint

	void Check_unlock(int rend_count)
	{
		for (int i = (int)SKILL::MAX - 1; i >= 0; i--)
		{
			if (skill_data[i].unlock) break;
			if (rend_count >= skill_data[i].unlock_rend_count)
				skill_data[i].unlock = true;
		}
	}

	//===============================================
	//	その他
	//===============================================
	int				poster_num;
	MODEL			model_part;

	//===============================================
	//	エフェクト
	//===============================================
	Explosion* explosion;

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

		//===========================================
		//	手裏剣
		class Syuriken : public Base
		{
		private:
			bool trg;
		public:
			Syuriken(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	変身
		class TransForm : public Base
		{
		public:
			TransForm(BasePlayer*me) : Base(me){}

			void Initialize();
			void Update();
			void Render(iexShader *shader = nullptr, char *name = '\0');
		};

		//===========================================
		//	対小物の破る
		class RendObj : public Base
		{
		public:
			RendObj(BasePlayer*me) : Base(me){}

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
	Barrier *barrier;			// バリアエフェクト
	Rush *rush;
	SkillBegin *skill_begin;
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
	void Render_forword();
	void Render_rush();
	
	//===============================================
	//	エフェクトの更新と描画
	//===============================================
	void EffectInit();
	void EffectUpdate();
	void EffectRender();
	void EffectRelease();

	void ExplosionAction();

	//===============================================
	//	ゲッター,セッター
	//===============================================
	const CONTROL_DESC& GetDesc()const{ return	m_controlDesc; }

	// 座標
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	Vector3 Get_center_pos(){ return Vector3(pos.x, pos.y + 5.0f, pos.z); }
	void Set_pos(const Vector3 &p){ pos = p; }
	
	// 前取得
	Vector3 Get_Flont(){
		Vector3 Out;
		Out.x = models[(int)model_part]->TransMatrix._31;
		Out.y = models[(int)model_part]->TransMatrix._32;
		Out.z = models[(int)model_part]->TransMatrix._33;
		Out.Normalize();
		return Out;
	}

	// アングル
	float Get_angleY(){ return angleY; }
	//void Get_angle(Vector3 &out){ out = angle; }
	void Set_angleY(float p){ angleY = p; }
	Vector3 Get_forward(){ return Vector3(models[(int)model_part]->TransMatrix._31, models[(int)model_part]->TransMatrix._32, models[(int)model_part]->TransMatrix._33); }

	// プレイヤーのモード
	ACTION_PART Get_action(){ return action_part; }
	virtual void Set_action(ACTION_PART part);
	virtual void Change_action(ACTION_PART part)
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

	// 0～1
	bool isUnlockSkill(int no){ return skill_data[no].unlock; }
	int Get_select_skill(){ return (int)select_skill; }
	float Get_skill_percentage(int no){ return 1.0f - ((float)skill_data[no].wait_time / (float)skill_data[no].cool_time); }

	// モデル
	iex3DObj* Get_Model(){ return models[(int)model_part]; }

	// 状態フラグ
	void Set_state_flag(BYTE flag)
	{
		stateFlag = flag;
	}
	// マンホール
	bool			isManhole;
	bool			manhole_no_haninai;
	Vector3			next_manhole_pos;	// 降りた先の、上がった先の座標
};

//extern BasePlayer* player[PLAYER_MAX];