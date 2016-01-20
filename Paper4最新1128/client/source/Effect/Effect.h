#pragma once
//*****************************************************************************************************************************
//
//	エフェクト
//
//*****************************************************************************************************************************

/*輝くエフェクト*/
void ShiningEffect(Vector3 _pos, float _scale = 1.0f);
/*バースト打てますEF*/
void AuraMAXEffect(Vector3 _pos, float _scale = 1.0f , Vector3 _col = Vector3(0.3f, 0.7f, 1.0f));

void Sand_effect(float x, float y, float z);

	//火花エフェクト
	enum FIRE_COLOR
	{
		RED,
		GREEN,
		BLUE,
		YELLOW,
		PURPLE,
		RANDOM
	};
	void EffectFireFlour(Vector3 p, FIRE_COLOR color = RED, float scale = 1.0f);

	//ボムエフェクト
	void	EffectBomb(Vector3 pos, float scale);

	extern iexParticlePlus	*	particle;

	//↓からクラス

	//爆発エフェクト
	class SimpleExplosion
	{
	private:

		iexMesh* chip_obj;

		int timer;
		Vector3 pos;
		float scale;
		static const int CHIP_MAX = 10;

		struct CHIP_STATUS
		{
			Vector3 pos;
			Vector3 move;
			Vector3 angle;
			Vector3 rotate;

			float scale;
		};

		CHIP_STATUS chip[CHIP_MAX];

		bool isLive;
	public:

		SimpleExplosion();
		~SimpleExplosion();

		void Init(Vector3 pos, float Scale);
		int Update();
		void Render();

		void EffectBomb();
		void EffectChup(Vector3 p);

		bool GetLive(){ return isLive; }
	};

	class BombScar
	{
	private:
	protected:

		iexMesh* obj;

		struct SCAR_STATUS{
			bool isLive;
			Vector3 pos;
			float scale;
			float angle;

			int timer;

		};
		static const int SCAR_MAX = 8;
		SCAR_STATUS scar[SCAR_MAX];

	public:


		BombScar();
		~BombScar();

		virtual void Init();
		virtual void Update();
		virtual void Render();

		virtual void Set(Vector3 pos, float scale = 0.5f, float angle = -1.0f);

		void chipEffect();
	};


