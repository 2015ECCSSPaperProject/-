#include "Effector.h"

/**************************************************
エフェクトマネージャクラス
**************************************************/
Effector::Effector() :renderer(0)
{
	for (int i = 0; i < EFFECT_KIND; i++)
		eff[i] = NULL;
}

Effector::~Effector()
{
	manager->Destroy();
	renderer->Destory();
	for (int i = 0; i < EFFECT_KIND; i++){
		//if (eff[i] == NULL)// メモリリークが起こるので今はコメントアウト
		{
			ES_SAFE_RELEASE(eff[i]);
		}
	}
}

bool Effector::Init()
{
	// 描画管理インスタンスの生成
	renderer = ::EffekseerRendererDX9::Renderer::Create(iexSystem::Device, 50000);
	// エフェクト管理用インスタンスの生成
	manager = ::Effekseer::Manager::Create(50000);
	// 描画方法の指定
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	// テクスチャ画像の読込方法の指定
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	// 座標系の指定
	manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	return true;
}

bool Effector::LoadEffect(int ID, const EFK_CHAR* filename)
{
	eff[ID] = Effekseer::Effect::Create(manager, filename);

	return !(eff[ID] == NULL);
}

void Effector::SetProjectionMatrix(Matrix mat)
{
	::Effekseer::Matrix44 p;
	p.Values[0][0] = mat._11;
	p.Values[0][1] = mat._12;
	p.Values[0][2] = mat._13;
	p.Values[0][3] = mat._14;

	p.Values[1][0] = mat._21;
	p.Values[1][1] = mat._22;
	p.Values[1][2] = mat._23;
	p.Values[1][3] = mat._24;

	p.Values[2][0] = mat._31;
	p.Values[2][1] = mat._32;
	p.Values[2][2] = mat._33;
	p.Values[2][3] = mat._34;

	p.Values[3][0] = mat._41;
	p.Values[3][1] = mat._42;
	p.Values[3][2] = mat._43;
	p.Values[3][3] = mat._44;
	// 投影行列を更新
	renderer->SetProjectionMatrix(p);
}

void Effector::SetViewMatrix(Matrix mat)
{
	::Effekseer::Matrix44 v;
	v.Values[0][0] = mat._11;
	v.Values[0][1] = mat._12;
	v.Values[0][2] = mat._13;
	v.Values[0][3] = mat._14;

	v.Values[1][0] = mat._21;
	v.Values[1][1] = mat._22;
	v.Values[1][2] = mat._23;
	v.Values[1][3] = mat._24;

	v.Values[2][0] = mat._31;
	v.Values[2][1] = mat._32;
	v.Values[2][2] = mat._33;
	v.Values[2][3] = mat._34;

	v.Values[3][0] = mat._41;
	v.Values[3][1] = mat._42;
	v.Values[3][2] = mat._43;
	v.Values[3][3] = mat._44;
	// ビュー行列を更新
	renderer->SetCameraMatrix(v);
}

void Effector::Update()
{
	manager->Update();
}

void Effector::Render()
{
	renderer->BeginRendering();
	manager->Draw();
	renderer->EndRendering();
	//	レンダリング設定リセット

	//	ポリゴン描画設定
	iexSystem::Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	iexSystem::Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	iexSystem::Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	//	アルファブレンド設定
	iexSystem::Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	iexSystem::Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	iexSystem::Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	iexSystem::Device->SetRenderState(D3DRS_ALPHAREF, 0);
	//	ブレンド設定 
	iexSystem::Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	iexSystem::Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
	iexSystem::Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	iexSystem::Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	iexSystem::Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	iexSystem::Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	//	テクスチャ設定
	iexSystem::Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	iexSystem::Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	iexSystem::Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	iexSystem::Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	iexSystem::Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//	ライティング設定
	iexLight::SetAmbient(0xffffffff);
	iexSystem::Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	//	スクリーン設定
	iexSystem::Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	iexSystem::Device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);

	iexSystem::Device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
}