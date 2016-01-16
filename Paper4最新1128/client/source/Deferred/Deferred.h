#pragma once
#include "iextreme.h"
#include	"../system/system.h"
#include <vector>
/*
*	Deferred
*/

// サーフェイスを綺麗にリリース
#define ReleaseSurface(p) { if(p != nullptr) delete p; p = nullptr;}

//　シャドウマップのサイズ
#define SHADOWSIZE 2048

// 極小バッファのサイズ
static const int Reduction_lvl = 4;
static const int Reduction_x = 1280 / Reduction_lvl, Reduction_y = 720 / Reduction_lvl;

static const int MiniTex_lvl = 6;
static const int MiniTex_x = 1280 / MiniTex_lvl, MiniTex_y = 720 / MiniTex_lvl;

// ダウンサンプル用
static const int downSampleSize = 4;

// ポイントライト用
static const int InfoPL_X = 32;
static const int InfoPL_Y = 2;

enum SURFACE_NAME{
	DIFFUSE, NORMAL, SPECULAR, DEPTH,		//G-Buffer
	LIGHT, SPEC, DOF, SHADOW, SHADOWMAP, SHADOWMAPL,
	BLOOMSCREEN, BLOOM, GLOWSCREEN, GLOW, FORWARD, DOWNSAMPLE,INFOPL,
	SCREEN, ALLSCREEN
};

class Deferred
{
private:
	//初期化関数
	void Init();

	Deferred();
	~Deferred();
	static Deferred* pInstance;
public:
	// Instance
	static Deferred& GetInstance();
	static void Release();

	//***************************
	//		Deferred_Set
	//***************************

	void G_Bigin();
	void G_End();
	void Update(const Vector3 ViewPos);

	//***************************
	//		Lighting
	//***************************
	// 平行光
	void DirLight(const Vector3 dir, const Vector3 color);
		
	//void PointLight(const Vector3 pos, const Vector3 color, const float range = 100.0f, const float power = 3.0f);	
	// 軽量ポイントライト
	//void SimpliPointLight(const Vector3 pos, const Vector3 color, const float range = 100.0f);

	// 半球ライティング
	void HemiLight(const Vector3 SkyColor, const Vector3 GroundColor);

	// 自己発光
	void Emissive();
	void Emissive(float EmissivePower);
	void SetEmissiveRate(float rate);

	// フォグ
	void Fog(const float FogNear, const float FogFar, const Vector3 FogColor);
	

	// 全部一括でライト処理
	void AllLight(const Vector3 dir, const Vector3 color, const Vector3 SkyColor, const Vector3 GroundColor, float EmissivePower);

	//****************************
	///		PointLight
	//****************************
	void ClearPointLight();
	void SetInfoPointLight(const Vector3 pos, const Vector3 color, const float range = 100.0f, const float power = 2.0f);
	void PointLightRender();
	//void InfoPointLight();

	//****************************
	//		ShadowMaps
	//****************************
	void CreateShadowMap(const int size = SHADOWSIZE);
	void CreateShadowMapL(const int size = SHADOWSIZE);
	void CreateShadowMatrix(Vector3 dir, Vector3 target,Vector3 playerVec, const float dist = 100.0f);
	void CreateShadowMatrixL(Vector3 dir, Vector3 target, Vector3 playerVec, const float dist = 100.0f);
	void ShadowBegin();
	void ShadowEnd();
	void ShadowBeginL();
	void ShadowEndL();
	void ShadowSetting(const float ShadowStrength, const float AdjustValue, const float DistAlpha = .75f);

	//****************************
	//		FORWARD
	//****************************
	// G_Buffe対象のZバッファが残っている状態で描画すること
	void ClearForward();
	void ForwardBigin();
	void ForwardEnd();
	void ForwardRender();

	//****************************
	//		BLOOM
	//****************************
	void ClearBloom();
	void BeginDrawBloom();
	void EndDrawBloom();
	void BloomRender();

	//****************************
	//		GLOW
	//****************************
	void ClearGlow();
	void BeginDrawGlow();
	void EndDrawGlow();
	void GlowRender();

	//****************************
	//		DOWNSAMPLE
	//****************************
	//void BeginDownSample();
	//void EndDownSample();
	void UpdateDownSample(float maxLuminance, float minLuminance);
	float GetExposure(){ return exposure; }
	void SetExposure(float exposure){ this->exposure = exposure; }
	void AddExposure(float exposure){ this->exposure += exposure; }
	float GetLuminance(){ return luminance; }

	//****************************
	//		DeferredRender
	//****************************
	void FinalResultDeferred();	// G_Bufferを合わせる

	void RenderDeferred(const int outX = 0, const int outY = 0, const int W = iexSystem::ScreenWidth, const int H = iexSystem::ScreenHeight, const int inX = iexSystem::ScreenWidth, const int inY = iexSystem::ScreenHeight);
	void RenderDOF(const Vector3 target, const float range);
	void RenderShadow();// 
	
	void DefaultRender();// デフォルト


	/********************/
	// OllRender
	/********************/
	void ClearAllRender();
	void AllBegin();
	void AllEnd();
	void AllRender();

	//ゲッター・セッター
	iex2DObj* GetTex(const int type); //テクスチャを取ってくる

	bool GetShadowFlag(){ return shadowFlag; }	//シャドウマップを採用しているのか
	void SetShadowFlag(bool flag){ shadowFlag = flag; }	//
	bool GetCascadeFlag(){ return cascadeFlag; }

private:

	// G-Buffer　MRT (マルチレンダーターゲット)
	iex2DObj* diffuse;
	iex2DObj* normal;
	iex2DObj* specular;
	iex2DObj* depth;

	// G-Bufferを利用するSecondSurface
	iex2DObj* light;			// ライト用サーフェイス	
	iex2DObj* spec;				// スペキュラ用サーフェイス
	iex2DObj* dof;				// 被写界深度用サーフェイス
	iex2DObj* shadow;			// シャドウ用サーフェイス

	// FinalPassSurface
	iex2DObj* screen;			// 演算結果をまとめたサーフェイス	
	iex2DObj* bloomScreen;		// HDRをかけるスクリーン
	iex2DObj* bloom;			// HDR
	iex2DObj* glowScreen;		// GLOWをかけるスクリーン
	iex2DObj* glow;				// GLOW

	// DownSample
	iex2DObj* downSample;		// DownSample用
	iex2DObj* calcDownSample;	// 計算用DownSample用

	// PointLight
	iex2DObj* infoPointLight;	// 情報用PointLight用

	// AllEnv
	iex2DObj* all;				// Deferred+Forward+Bloom


	// 露光レベル
	float exposure;
	// シーンの輝度
	float luminance;

	// ForwardSurface
	iex2DObj* forward;			// deferredで通さず直接描画するサーフェイス

	// BackBufferSurface 
	// 最終的に画面に映すサーフェイス
	Surface* backbuffer;
	// バックステンシルバッファサーフェイス
	Surface* backbufferZ;
	// 保存用サーフェイス
	Surface* savebackbuffer;
	D3DVIEWPORT9	saveViewport;// ビューポート保存用

	// カメラの位置
	Vector3 ViewPos;

	// 環境マップ
	iex2DObj* EnvMap;

	/*************************/
	//	PointLight用
	/*************************/
	typedef struct
	{
		Vector3 pos;
		float range;
		Vector3 color;
		float power;
	} PointLightData;
	std::vector<PointLightData> PLdata;	// ポイントライトのデータ
	static const int PL_MAX = 20;		// ポイントライトの最大数
	int PL_NUM;							// ポイントライトの数

	//****************************
	//		ShadowMap
	//****************************
	//シャドウマップ書き込み時ステンシルバッファサーフェイス
	Surface*		shadowZ;
	Surface*		orgZ;		// Zバッファ保存用
	D3DVIEWPORT9	orgViewport;// ビューポート保存用

	iex2DObj*	shadowMap;		// ｚ値テクスチャ
	iex2DObj*	shadowMapL;		// 遠距離用マップ
	iex2DObj*	softShadowMap;	// ソフトシャドウマップ
	
	int		shadowSize;			// シャドウマップのサイズ
	int		shadowSizeL;		// シャドウマップのサイズL
	float	shadowRange;		// Shadowのカメラからの距離

	//シャドウマップの使用フラグ
	bool shadowFlag;
	bool cascadeFlag;


};

#define DeferredManager (Deferred::GetInstance())