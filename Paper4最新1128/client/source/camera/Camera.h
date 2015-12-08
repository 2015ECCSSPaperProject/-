
#pragma once

#include "iextreme.h"

class BasePlayer;
class EffectCamera;

class Camera : public iexView
{
public:
	//===============================================
	//	定数
	//===============================================
	enum MODE{ M_FIX, M_PAN, M_SLERP, M_DEBUG, M_TPS, M_FPS, M_RESPAWN, M_ZOOM, M_TARGET, M_THROUGH, M_SYURIKEN, M_MAX };
	enum class FOV_TYPE{ DEFAULT, HARD };

private:

	//===============================================
	//	基本パラメータ
	//===============================================
	Vector3 pos;
	Vector3 target;
	Vector3 ipos, itarget;	//理想の位置
	Vector3 angle;
	float slerp_percentage;	// 補完割合

	iexMesh *collision_stage;


	//------------------------------------------------------
	//		投影関係
	//------------------------------------------------------
	struct{
		float fovY, Near, Far;
	}parth;


	//===============================================
	//	カメラ委譲クラス(インナークラス)
	//===============================================
	class Mode
	{
	public:

		//===========================================
		//	モード基底クラス
		class Base
		{
		protected:
			Camera *me;
			float dist;	// カメラ→プレイヤの距離
			const static float DIST;
			void Collision();

		public:
			Base(Camera* me) :me(me){}
			virtual void Initialize(const Vector3 &pos, const Vector3 &target) {}
			virtual void Update(){}
		};

		//===========================================
		//	固定視点
		class Fix : public Base
		{
		public:
			Fix(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	パン(座標固定、プレイヤー追跡)
		class Pan : public Base
		{
		public:
			Pan(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	補完カメラ
		class Slerp : public Base
		{
		public:
			Slerp(Camera *me) :Base(me){}

			//void Initialize(const Vector3 &Pos, const Vector3 &Target);
			void Update();
		};

		//===========================================
		//	デバッグ視点モード
		class Debug : public Base
		{
		public:
			Debug(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	3人称視点モード
		class TPS : public Base
		{
		private:
			float iangle;
		public:
			TPS(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	1人称視点モード
		class FPS : public Base
		{
		public:
			FPS(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	止まっているモード
		class Respawn : public Base
		{
		public:
			Respawn(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	プチズームモード
		class Zoom : public Base
		{
		public:
			Zoom(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	ターゲットモード
		class Target : public Base
		{
		public:
			Target(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	狭いとこ通り抜けモード
		class Through : public Base
		{
		public:
			Through(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};

		//===========================================
		//	狭いとこ通り抜けモード
		class Syuriken : public Base
		{
		public:
			Syuriken(Camera *me) :Base(me){}

			void Initialize(const Vector3 &pos, const Vector3 &target);
			void Update();
		};
	};

	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	Mode::Base *mode[MODE::M_MAX];
	MODE mode_part;
	EffectCamera *effect_camera;

	//===============================================
	//	自分のプレイヤー
	//===============================================
	BasePlayer *my_player;

public:
	//------------------------------------------------------
	//	スクリプト実行状態
	//------------------------------------------------------
	bool scriptON;

	//===============================================
	//	初期化と解放
	//===============================================
	Camera();
	~Camera();

	void Initialize(BasePlayer *my);

	//===============================================
	//	更新・描画
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	ゲッター,セッター
	//===============================================

	// 座標
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	Vector3 Get_target(){ return target; }

	// モード
	MODE Get_mode(){ return mode_part; }
	void Change_mode(MODE part)
	{
		Change_mode(part, this->pos, this->target);
	}
	void Change_mode(MODE part, const Vector3 &pos, const Vector3 &target)
	{
		// モード切替
		mode_part = part;
		mode[(int)part]->Initialize(pos, target);
	}

	BasePlayer *Get_my_player(){ return my_player; }


	//------------------------------------------------------
	//	Slerpにモードチェンジする際に呼び出すこと
	//------------------------------------------------------
	void Set_slerp(const Vector3 &Pos, const Vector3 &Target, const Vector3 &Next_pos, const Vector3 Next_target, float percentage)
	{
		pos = Pos;
		target = Target;
		ipos = Next_pos;
		itarget = Next_target;
		slerp_percentage = percentage;
	}

	void Set_slerp(Vector3 &Next_pos, const Vector3 Next_target, float percentage)
	{
		Set_slerp(pos, target, Next_pos, Next_target, percentage);
	}

};

#include	"../textLoader\textLoader.h"

class EffectCamera : public textLoader
{
private:
	enum class APPOINT
	{
		NONE,			// 座標そのまま
		DIRECT,			// Pos直接指定型
		SOME_ONE,		// 誰かのPos
		SOME_ONE_COOD	// 誰かのPos＋Vector3
	};

	//------------------------------------------------------
	//	スクリプト関係
	//------------------------------------------------------
	int wait_timer;		// コマンド待機時間
	char message[256];	// コマンドの名前？
	unsigned long ptr;	// 

	bool In_event(char *command);
	void Out_event();
	bool Jump(char *label_name);
	void Change_camera_mode();
	void Setting_camera(Camera::MODE mode);
	void Getting_targeter(Vector3 *out);
	void Getting_targeter_coodinate(Vector3 *out);

	//------------------------------------------------------
	//	制御するカメラさん
	//------------------------------------------------------
	Camera *camera;


public:

	//------------------------------------------------------
	//	初期化・解放
	//------------------------------------------------------
	EffectCamera();
	~EffectCamera();
	void Initialize(Camera *me);

	//------------------------------------------------------
	//	更新
	//------------------------------------------------------
	void Update();

	//------------------------------------------------------
	//	行動セット
	//------------------------------------------------------
	bool Set_pattern(int pat);
};

// 
extern Camera *camera;
