
#pragma once

#include "iextreme.h"

class BasePlayer;

class Camera : public iexView
{
public:

	//===============================================
	//	定数
	//===============================================
	enum MODE_PART{ M_DEBUG, M_TPS, M_FPS, M_RESPAWN, M_ZOOM, M_MAX };
	enum class FOV_TYPE{ DEFAULT, HARD };

private:

	//===============================================
	//	基本パラメータ
	//===============================================
	Vector3 pos;
	Vector3 target;
	Vector3 ipos, itarget;	//理想の位置
	Vector3 angle;

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
			virtual void Initialize(){}
			virtual void Update(){}
		};

		//===========================================
		//	デバッグ視点モード
		class Debug : public Base
		{
		public:
			Debug(Camera *me) :Base(me){}

			void Initialize();
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

			void Initialize();
			void Update();
		};

		//===========================================
		//	1人称視点モード
		class FPS : public Base
		{
		public:
			FPS(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};

		//===========================================
		//	止まっているモード
		class Respawn : public Base
		{
		public:
			Respawn(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};

		//===========================================
		//	プチズームモード
		class Zoom : public Base
		{
		public:
			Zoom(Camera *me) :Base(me){}

			void Initialize();
			void Update();
		};
	};

	//===============================================
	//	委譲クラスへのポインタ
	//===============================================
	Mode::Base *mode[MODE_PART::M_MAX];
	MODE_PART mode_part;


	//===============================================
	//	自分のプレイヤー
	//===============================================
	BasePlayer *my_player;

public:
	//===============================================
	//	初期化と解放
	//===============================================
	Camera();
	~Camera();

	void Initialize(BasePlayer *my);

	//===============================================
	//	更新
	//===============================================
	void Update();


	//===============================================
	//	ゲッター,セッター
	//===============================================

	// 座標
	Vector3 Get_pos(){ return pos; }
	void Get_pos(Vector3 &out){ out = pos; }
	void Set_pos(const Vector3 &p){ pos = p; }

	// モード
	MODE_PART Get_mode(){ return mode_part; }
	void Change_mode(MODE_PART part)
	{
		// モード切替
		mode_part = part;
		mode[part]->Initialize();
	}



};

// 
extern Camera *camera;
