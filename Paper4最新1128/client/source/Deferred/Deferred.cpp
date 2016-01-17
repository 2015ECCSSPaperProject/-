#include "Deferred.h"
/*
*	Deferred
*/

Deferred* Deferred::pInstance = nullptr;

/// インスタンス
Deferred& Deferred::GetInstance()
{
	if (!pInstance)
	{
		pInstance = new Deferred;
	}
	return *pInstance;
}

void Deferred::Release()
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
	}
}

/* マップの読み込み情報 */
/// N RGB->法線 
/// S R->スペキュラ G->ブルームレート B->リムライト　A→エミッシブ
/// H R->AO G->エミッシブ追加予定　 B->空白

/// 初期化
void Deferred::Init()
{
	// G_Bufferを作成
	diffuse = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET); // RGB アルべド
	normal	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);	 // RGB 法線　A アンビエントレート
	specular= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);	 // R スペキュラ G ブルーム　B リムライト A　エミッシブ
	depth	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_FLOAT);		 // R デプス

	// G-Bufferを利用するサーフェイス
	light	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);
	spec	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);
	dof		= new iex2DObj(Reduction_x, Reduction_y, IEX2D_HDR);			//A8R8G8B8
	shadow	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);
	
	// FinalPassSurface
	screen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);

	// ForwardSurface
	forward = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);		//	上から被せるため透明度も採用

	// Bloom HDR済みのものを描画しているので　HDRからUSEALPHAへ
	bloomScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);			//　ゲーム画面をHDR用にいじる用
	bloom = new iex2DObj(MiniTex_x, MiniTex_y, IEX2D_USEALPHA);		// HDR

	// Glow
	glowScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);	//　ゲーム画面をGlow用にいじる用
	glow = new iex2DObj(MiniTex_x, MiniTex_y, IEX2D_HDR);			// Glow

	// DownSample
	downSample = new iex2DObj(downSampleSize, downSampleSize, IEX2D_FLOAT);
	calcDownSample = new iex2DObj(downSampleSize, downSampleSize, IEX2D_SYSTEMMEM);
	
	// 計算用PointLight
	infoPointLight = new iex2DObj(InfoPL_X, InfoPL_Y, IEX2D_FLOAT4_SYSTEMMEM);

	// 全部一括に保存　環境マップのために
	all = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);


	// 露光レベル
	exposure = 0.0f;
	// 輝度レベル
	luminance = 0.0f;

	// BackBuffer
	iexSystem::Device->GetRenderTarget(0, &backbuffer);
	iexSystem::Device->GetDepthStencilSurface(&backbufferZ);
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	iexSystem::Device->GetViewport(&saveViewport);// 現在のビューポートの一時保管


	//ShadowMaps
	shadowZ = nullptr;
	shadowMap = nullptr;
	shadowMapL = nullptr;
	softShadowMap = nullptr;
	shadowSize = 0;
	shadowRange = 0.0f;
	shadowFlag = false;
	cascadeFlag = false;

	ViewPos = Vector3(.0f, .0f, .0f);

	// 環境マップ
	EnvMap = new iex2DObj("DATA/Shader/Env.png");
	shaderD->SetValue("EnvMap", EnvMap);

}

Deferred::Deferred()
{
	Init();
}

/// 解放
Deferred::~Deferred()
{
	// G-Buffer
	ReleaseSurface(diffuse);
	ReleaseSurface(normal);
	ReleaseSurface(specular);
	ReleaseSurface(depth);

	// SecondPassSurface
	ReleaseSurface(light);
	ReleaseSurface(spec);
	ReleaseSurface(dof);
	ReleaseSurface(shadow);

	// FinalPassSurface
	ReleaseSurface(screen);

	// ForwardSurface
	ReleaseSurface(forward);

	// glowSurface
	ReleaseSurface(bloomScreen);
	ReleaseSurface(bloom);

	// glowSurface
	ReleaseSurface(glow);
	ReleaseSurface(glowScreen);
		
	// downSample
	ReleaseSurface(downSample);
	ReleaseSurface(calcDownSample);

	// infoPointLight
	ReleaseSurface(infoPointLight);

	// ollSurface
	ReleaseSurface(all);

	//BackBufferSurface
	backbuffer->Release();
	backbuffer = nullptr;

	backbufferZ->Release();
	backbufferZ = nullptr;

	savebackbuffer->Release();
	savebackbuffer = nullptr;


	SAFE_DELETE(EnvMap);

	if (shadowFlag)
	{
		ReleaseSurface(shadowMap);

	}
	if (cascadeFlag)
	{
		ReleaseSurface(shadowMapL);
		ReleaseSurface(softShadowMap);
	}
}

/// G_Buffer作成開始 &前回使用したのサーフェイスのクリア
void Deferred::G_Bigin()
{
	//バックバッファサーフェイスの保存
	iexSystem::Device->GetRenderTarget(0, &backbuffer);

	//ライトサーフェイス画面クリア
	light->RenderTarget();
	spec->RenderTarget(1);
	//if (ssaoFlag == true)ssao->RenderTarget(2);


	//ライトサーフェイス画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	//MRTの割り当て
	diffuse->RenderTarget(SURFACE_NAME::DIFFUSE);
	normal->RenderTarget(SURFACE_NAME::NORMAL);
	specular->RenderTarget(SURFACE_NAME::SPECULAR);
	depth->RenderTarget(SURFACE_NAME::DEPTH);

	//G_Buffer画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);



}
/// G_Buffer作成終了
void Deferred::G_End()
{
	//バックバッファサーフェイスの復元・MRTの終了処理
	iexSystem::Device->SetRenderTarget(NORMAL, nullptr);
	iexSystem::Device->SetRenderTarget(SPECULAR, nullptr);
	iexSystem::Device->SetRenderTarget(DEPTH, nullptr);


	//作った情報テクスチャを転送
	shaderD->SetValue("DepthBuf", depth);
	shaderD->SetValue("SpecularBuf", specular);
	shaderD->SetValue("NormalBuf", normal);

	iexSystem::Device->SetRenderTarget(0, backbuffer);


}

/// Deferredの更新
void Deferred::Update(const Vector3 ViewPos)
{
	this->ViewPos = ViewPos;
	shaderD->SetValue("ViewPos", this->ViewPos);

	//プロジェクション逆行列生成
	Matrix invproj = matProjection;
	D3DXMatrixInverse(&invproj, nullptr, &invproj);
	shaderD->SetValue("InvProj", invproj);
	
	if (shadowFlag){
		//ビュー逆行列生成
		Matrix invview = matView;
		D3DXMatrixInverse(&invview, nullptr, &invview);
		shaderD->SetValue("InvViewProj", invproj * invview);
	}

}

//**************************************************
///				Lighting
//**************************************************
/// 平行光
void Deferred::DirLight(const Vector3 dir, const Vector3 color)
{
	Matrix matV = matView;

	Vector3 LightDir;
	// ワールドの平行光を渡す
	Vector3 wLightDir = dir;
	wLightDir.Normalize();
	shaderD->SetValue("wLightVec", wLightDir);
	
	//ビュー座標系に変換
	LightDir.x = dir.x * matV._11 + dir.y * matV._21 + dir.z * matV._31;
	LightDir.y = dir.x * matV._12 + dir.y * matV._22 + dir.z * matV._32;
	LightDir.z = dir.x * matV._13 + dir.y * matV._23 + dir.z * matV._33;


	LightDir.Normalize();

	//	シェーダー設定 shaderDに送る
	shaderD->SetValue("ViewLightVec", LightDir);
	shaderD->SetValue("LightColor", (Vector3)color);

	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//レンダーターゲットの切替え
	light->RenderTarget();
	spec->RenderTarget(1);


	//	レンダリング
	normal->Render(shaderD, "def_DirLight");

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);
}

// 全部一括で
void Deferred::AllLight(const Vector3 dir, const Vector3 color, const Vector3 SkyColor, const Vector3 GroundColor, float EmissivePower)
{

	Matrix matV = matView;

	Vector3 LightDir;

	//ビュー座標系に変換
	LightDir.x = dir.x * matV._11 + dir.y * matV._21 + dir.z * matV._31;
	LightDir.y = dir.x * matV._12 + dir.y * matV._22 + dir.z * matV._32;
	LightDir.z = dir.x * matV._13 + dir.y * matV._23 + dir.z * matV._33;


	LightDir.Normalize();

	//	シェーダー設定 
	shaderD->SetValue("LightVec", LightDir);
	shaderD->SetValue("LightColor", (Vector3)color);
	//	シェーダー設定
	shaderD->SetValue("SkyColor", (Vector3)SkyColor);
	shaderD->SetValue("GroundColor", (Vector3)GroundColor);
	//	シェーダー設定
	shaderD->SetValue("EmissivePowerRate", EmissivePower);

	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//レンダーターゲットの切替え
	light->RenderTarget();
	spec->RenderTarget(1);


	//	レンダリング
	normal->Render(shaderD, "def_AllLight");

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);
}


//****************************
///		POINTLIGHT
//****************************

void Deferred::ClearPointLight()
{
	PL_NUM = 0;
	PLdata.clear(); 

	// ポイントライトの情報を初期化
}
void Deferred::SetInfoPointLight(const Vector3 pos, const Vector3 color, const float range, const float power)
{
	PointLightData data;

	// 今のままだとカリングが意味をなさない
	Matrix mWV = matView;// ビュー行列
	// 視錐台カリング処理
	Vector3 flont(mWV._13, mWV._23, mWV._33);
	flont.Normalize();
	Vector3 dir = pos - (ViewPos - flont * range);			//レンジの値分下げて、カリングを緩める。
	dir.Normalize();
	float dot = Vector3Dot(dir, flont);
	if (dot < 0.5)
	{
		return;			// 見えていないポイントライトを消去
	}


	// カメラ空間変換
	Matrix mat = matView;
	data.pos.x = pos.x * mat._11 + pos.y * mat._21 + pos.z * mat._31 + mat._41;
	data.pos.y = pos.x * mat._12 + pos.y * mat._22 + pos.z * mat._32 + mat._42;
	data.pos.z = pos.x * mat._13 + pos.y * mat._23 + pos.z * mat._33 + mat._43;

	data.range = range;
	data.color = color;
	data.power = power;

	// 配列に加える
	PLdata.push_back(data);

}
//void Deferred::PointLightRender()
//{
//	//// ポイントライトの数
//	//shaderD->SetValue("PL_NUM", (int)PLdata.size());
//
//	//// ポイントライトの情報を送る
//	//for (int i = 0; i < (int)PLdata.size(); i++)
//	//{
//	//	if (i >= PL_MAX)
//	//	{
//	//		MessageBox(iexSystem::Window, "ポイントライトの最大数を超えました", "ERROR", MB_OK);
//	//	}
//	//	char str[128];
//	//	sprintf(str, "PLpos[%d]", i);
//	//	shaderD->SetValue(str, PLdata[i].pos);
//	//	sprintf(str, "PLrange[%d]", i);
//	//	shaderD->SetValue(str, PLdata[i].range);
//	//	sprintf(str, "PLcolor[%d]", i);
//	//	shaderD->SetValue(str, PLdata[i].color);
//	//	sprintf(str, "PLpower[%d]", i);
//	//	shaderD->SetValue(str, PLdata[i].power);
//	//}
//
//
//	//// 現在のレンダーターゲットを一時的に確保
//	//Surface* now = nullptr;
//	//iexSystem::Device->GetRenderTarget(0, &now);
//
//	//// レンダーターゲットの切替え
//	//light->RenderTarget();
//	//
//	//// レンダリング
//	//normal->Render(shaderD, "def_PointLight");
//
//	//// レンダーターゲットの復元
//	//iexSystem::Device->SetRenderTarget(0, now);
//}

void Deferred::PointLightRender()
{
	// ポイントライトの数
	shaderD->SetValue("PL_NUM", (int)PLdata.size());

	// サーフェスをロックしてコピー
	D3DLOCKED_RECT LockedRect;// テクスチャ リソース上の矩形をロックする
	// RGBA34のポイントライト
	infoPointLight->GetTexture()->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD);
	
	// float4の型を作成
	struct SEND_COLOR
	{
		float r, g, b, a;
	};
	SEND_COLOR col4;

	// 0初期化
	FillMemory(LockedRect.pBits, LockedRect.Pitch * InfoPL_Y, 0);
	// 絵を描きこむ
	float* pData;
	for (int y = 0; y < InfoPL_Y; y++)
	{
		/*書き込む行の先頭アドレスに移動！*/
		pData = (float*)((LPBYTE)LockedRect.pBits + LockedRect.Pitch * y);// yの値だけ移動

		for (int x = 0; x < InfoPL_X; x++)
		{
	
			// ポイントライトの最大数まで来たらcontinue
			if (x >= (int)PLdata.size()) continue;
	
			// memcpy
			// コピー先のポインタ
			// コピー元のポインタ
			// コピー元のポインタのサイズ

			col4.r = col4.g = col4.b = col4.a = 0.0f;	
			// テクスチャの0行目に ポジションとレンジを
			if (y == 0)
			{
				// ポイントライトのポジションをRGBに詰める！
				col4.r = PLdata[x].pos.x;
				col4.g = PLdata[x].pos.y;
				col4.b = PLdata[x].pos.z;

				// Rangeをαに詰める
				col4.a = PLdata[x].range;
				
				// テクスチャへ書き込む
				memcpy((float*)pData + (4 * x), &col4, sizeof(SEND_COLOR));
			}
			// テクスチャの1行目に カラーと強さを
			else if (y == 1)
			{
				// ポイントライトのカラーをRGBに詰める！
				col4.r = PLdata[x].color.x;
				col4.g = PLdata[x].color.y;
				col4.b = PLdata[x].color.z;

				// ポイントライトの強さをαに詰める
				col4.a = PLdata[x].power;

				// テクスチャへ書き込む
				memcpy((float*)pData + (4 * x), &col4, sizeof(SEND_COLOR));
			}
	
		}

	}

	// サーフェイスのロックを解除
	infoPointLight->GetTexture()->UnlockRect(0);


	// ポイントライトの情報を詰めたテクスチャをGPUへ送る！
	shaderD->SetValue("InfoPLMap", infoPointLight);


	/*********************************/
	//		ポイントライト描画
	/*********************************/

	// 現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	// レンダーターゲットの切替え
	light->RenderTarget();

	// レンダリング
	normal->Render(shaderD, "def_PointLight");

	// レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
}

//void Deferred::PointLight(const Vector3 pos, const Vector3 color, const float range, const float power)
//{
//
//	Matrix mWV = matView;// ビュー行列
//
//	// 視錐台カリング処理
//	Vector3 flont(mWV._13, mWV._23, mWV._33);
//	flont.Normalize();
//	Vector3 dir = pos - (ViewPos - flont * range);			//レンジの値分下げて、カリングを緩める。
//	dir.Normalize();
//
//	float dot = Vector3Dot(dir, flont);
//	if (dot < .5f) return;					//　わかったああああああ！！！ポイントライト消失事件
//
//	// ライト位置をビュー座標系に変換
//	Vector3 PointLight;
//	D3DXVECTOR3 plight = D3DXVECTOR3(pos.x, pos.y, pos.z);
//	D3DXVec3TransformCoord(&plight, &plight, &mWV);
//	PointLight = Vector3(plight.x, plight.y, plight.z);
//
//	// 必要パラメータの転送
//	shaderD->SetValue("LightPos", PointLight);
//	shaderD->SetValue("LightColor", (Vector3)color);
//	shaderD->SetValue("LightRange", range);
//	shaderD->SetValue("LightPower", power);
//
//	// 現在のレンダーターゲットを一時的に確保
//	Surface* now = nullptr;
//	iexSystem::Device->GetRenderTarget(0, &now);
//
//	// レンダーターゲットの切替え
//	light->RenderTarget();
//	//spec->RenderTarget(1);
//
//
//	// レンダリング
//	normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	//normal->Render(shaderD, "def_PointLight");
//	
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	//normal->Render(shaderD, "def_Emissive");
//	
//	// レンダーターゲットの復元
//	iexSystem::Device->SetRenderTarget(0, now);
//	//iexSystem::Device->SetRenderTarget(1, nullptr);
//
//}


/// シンプルポイントライト
//void Deferred::SimpliPointLight(const Vector3 pos, const Vector3 color, const float range)
//{
//
//	//Matrix mWV = matView;// ビュー行列
//
//	//// 視錐台カリング処理
//	//Vector3 flont(mWV._13, mWV._23, mWV._33);
//	//flont.Normalize();
//	//Vector3 dir = pos - (ViewPos - flont * range);			//レンジの値分下げて、カリングを緩める。
//	//dir.Normalize();
//
//	//float dot = Vector3Dot(dir, flont);
//	//if (dot < .5f) return;
//
//	//// ライト位置をビュー座標系に変換
//	//Vector3 PointLight;
//	//D3DXVECTOR3 plight = D3DXVECTOR3(pos.x, pos.y, pos.z);
//	//D3DXVec3TransformCoord(&plight, &plight, &mWV);
//	//PointLight = Vector3(plight.x, plight.y, plight.z);
//
//	//// 必要パラメータの転送
//	//shaderD->SetValue("LightPos", PointLight);
//	//shaderD->SetValue("LightColor", (Vector3)color);
//	//shaderD->SetValue("LightRange", range);
//
//	//// 現在のレンダーターゲットを一時的に確保
//	//Surface* now = nullptr;
//	//iexSystem::Device->GetRenderTarget(0, &now);
//
//	//// レンダーターゲットの切替え
//	//light->RenderTarget();
//	//spec->RenderTarget(1);
//
//	//// レンダリング
//	//normal->Render(shaderD, "def_SimpliPointLight");
//
//	//// レンダーターゲットの復元
//	//iexSystem::Device->SetRenderTarget(0, now);
//	//iexSystem::Device->SetRenderTarget(1, nullptr);
//}
// 半球ライティング
void Deferred::HemiLight(const Vector3 SkyColor, const Vector3 GroundColor)
{
	//	シェーダー設定
	shaderD->SetValue("SkyColor", (Vector3)SkyColor);
	shaderD->SetValue("GroundColor", (Vector3)GroundColor);

	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//レンダーターゲットの切替え
	light->RenderTarget();

	//	レンダリング
	normal->Render(shaderD, "def_HemiLight");

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
}

// エミッシブ
void Deferred::Emissive()
{
	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//レンダーターゲットの切替え
	light->RenderTarget();

	//  specular.aを使いたいので
	//	specularでレンダリング
	specular->Render(shaderD, "def_Emissive");

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
}

void Deferred::Emissive(float EmissivePower)
{
	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//レンダーターゲットの切替え
	light->RenderTarget();

	//	シェーダー設定
	shaderD->SetValue("EmissivePowerRate", EmissivePower);
	
	//  specular.aを使いたいので
	//	specularでレンダリング
	specular->Render(shaderD, "def_Emissive");

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
}

void Deferred::SetEmissiveRate(float rate)
{
	//	シェーダー設定
	shaderD->SetValue("EmissivePowerRate", rate);
}


void Deferred::Fog(const float FogNear, const float FogFar, const Vector3 FogColor)
{
	//	GPUへ送る情報
	shaderD->SetValue("FogNear", FogNear);
	shaderD->SetValue("FogFar",  FogFar);
	shaderD->SetValue("FogColor", (Vector3)FogColor);

	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//レンダーターゲットの切替え
	light->RenderTarget();

	//  深度を使いたいので
	//	depthでレンダリング
	depth->Render(shaderD, "def_Fog");

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
}

//****************************
///	FORWARD
//****************************

void Deferred::ClearForward()
{
	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// フォワードをセット
	forward->RenderTarget(0);
	// フォワード画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
}

// G_Buffe対象のZバッファが残っている状態にセットすること
void Deferred::ForwardBigin()
{
	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// forward用サーフェイスに変える
	forward->RenderTarget(0);



};
// G_Buffe対象のZバッファが残っている状態にセットすること
void Deferred::ForwardEnd()
{
	//diffuseに戻す
	//diffuse->RenderTarget(0);
	//作った情報テクスチャを転送
	shaderD->SetValue("ForwardTex", forward);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
}

void Deferred::ForwardRender()
{
	// forwardで描画した絵を上からかぶせる
	forward->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);

}

//****************************
///		BLOOM
//****************************
void Deferred::ClearBloom()
{
	// 今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// Bloomを描画する元の
	bloomScreen->RenderTarget();
	iexSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::BeginDrawBloom()
{
	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	bloomScreen->RenderTarget();//
	
	//iexSystem::Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);//Zバッファに書き込まない
	// ↓から映すであろう一枚絵は次から描画するものの為に
	//　Zバッファに書き込まないようにする

	// ※IEXのRenderがDefaltで書き込むように設定してるので　描画の対象に　RS_COPY_NOZとするしかない
};
void Deferred::EndDrawBloom()
{
	// ↑でZバッファをOFFにしていたので戻す
	//iexSystem::Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);//Zバッファに書き込む

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::BloomRender()
{
	D3DVIEWPORT9 save_viewport;//ビューポートを一時保存
	iexSystem::Device->GetViewport(&save_viewport);

	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	//テクスチャのサイズのビューポート設定
	D3DVIEWPORT9 v = { 0, 0, MiniTex_x, MiniTex_y, 0, 1.0f };
	iexSystem::Device->SetViewport(&v);

	bloom->RenderTarget();
	iexSystem::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	shaderD->SetValue("TU", (float)1.25f / (float)MiniTex_x);
	shaderD->SetValue("TV", (float)1.25f / (float)MiniTex_y);

	// 画面の明るい部分をブルームの種として抽出
	bloomScreen->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "make_hdrBloomSeed");

	//ボケ度　回数が少ないほどボケない
	for (int i = 0; i < 4; i++)
	{
		bloom->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, MiniTex_x, MiniTex_y, shaderD, "gaussZ");//奥行を禁止
	}

	iexSystem::Device->SetViewport(&save_viewport);
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

	// HDRブルームを乗算　
	bloom->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, MiniTex_x, MiniTex_y, shaderD, "add_hdrBloom");

};

//****************************
///		GLOW
//****************************
void Deferred::ClearGlow()
{
	// 今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// glowを描画する元の
	glowScreen->RenderTarget();
	iexSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::BeginDrawGlow()
{
	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	glowScreen->RenderTarget();//
	
};
void Deferred::EndDrawGlow()
{
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::GlowRender()
{
	D3DVIEWPORT9 save_viewport;//ビューポートを一時保存
	iexSystem::Device->GetViewport(&save_viewport);

	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	//テクスチャのサイズのビューポート設定
	D3DVIEWPORT9 v = { 0, 0, MiniTex_x, MiniTex_y, 0, 1.0f };
	iexSystem::Device->SetViewport(&v);

	glow->RenderTarget();
	iexSystem::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	shaderD->SetValue("TU", (float)1 / (float)MiniTex_x);
	shaderD->SetValue("TV", (float)1 / (float)MiniTex_y);

	glowScreen->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);

	//ボケ度　回数が少ないほどボケない
	for (int i = 0; i < 2; i++)
	{
		glow->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, MiniTex_x, MiniTex_y, shaderD, "gaussZ");//奥行を禁止
	}

	iexSystem::Device->SetViewport(&save_viewport);
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

	//明るさ　回数が多いほど明るく
	for (int i = 0; i < 2; i++)
	{
		glow->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, MiniTex_x, MiniTex_y, RS_ADD);
	}

};

//****************************
///		DOWNSAMPLE
//****************************
//void Deferred::BeginDownSample()
//{
//	// レンダーターゲット切り替え
//	// 今のサーフェイスを保存
//	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
//	downSample->RenderTarget();
//
//	// ビューポート作成
//	iexSystem::Device->GetViewport(&saveViewport);// 現在のビューポートの一時保管
//	// ビューポートの作成
//	D3DVIEWPORT9 vp = { 0, 0, downSampleSize, downSampleSize, 0, 1.0f };
//	iexSystem::Device->SetViewport(&vp);
//
//}
//void Deferred::EndDownSample()
//{
//	// 
//
//	// サーフェスをロックしてコピー
//	D3DLOCKED_RECT LockedRect;// テクスチャ リソース上の矩形をロックする
//	calcDownSample->GetTexture()->LockRect(0, &LockedRect, NULL, D3DLOCK_READONLY);
//
//	// 矩形分のバッファを作成 
//	float buffer[downSampleSize*downSampleSize];
//
//	// 全画像データをコピー
//	for (int y = 0; y < downSampleSize; y++)
//	{
//		CopyMemory(&buffer[downSampleSize * y], (char*)LockedRect.pBits + LockedRect.Pitch * y, downSampleSize * sizeof(float));
//	}
//
//	// サーフェイスのロックを解除
//	calcDownSample->GetTexture()->UnlockRect(0);
//
//	// pixelの数
//	float maxPixel = downSampleSize*downSampleSize;
//
//	// 全pixelの輝度を加算
//	float sum = 0;
//	for (int i = 0; i < maxPixel; i++)
//	{
//		sum += buffer[i];
//	}
//	// 最後に加算した分割って平均化
//	sum /= maxPixel;
//
//
//
//	// 元に
//	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
//	iexSystem::Device->SetViewport(&saveViewport);
//}


void Deferred::UpdateDownSample(float maxLumi, float minLumi)
{

	// レンダーターゲット切り替え
	// 今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	downSample->RenderTarget();

	// ビューポート作成
	iexSystem::Device->GetViewport(&saveViewport);// 現在のビューポートの一時保管
	// ビューポートの作成
	D3DVIEWPORT9 vp = { 0, 0, downSampleSize, downSampleSize, 0, 1.0f };
	iexSystem::Device->SetViewport(&vp);

	// 現在のスクリーンをダウンサンプル用に描画
	//screen->Render(0, 0, downSampleSize, downSampleSize, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "ToneMap_DownSample");
	screen->Render(0, 0, downSampleSize, downSampleSize, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "DownSample");

	// 絵を描き終わったので元に戻す
	iexSystem::Device->SetViewport(&saveViewport); // 先にビューポートから

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);


	// レンダリング先テクスチャの内容をサーフェスに転送
	iexSystem::GetDevice()->GetRenderTargetData(
		downSample->GetSurface(),
		calcDownSample->GetSurface());

	// サーフェスをロックしてコピー
	D3DLOCKED_RECT LockedRect;// テクスチャ リソース上の矩形をロックする
	calcDownSample->GetTexture()->LockRect(0, &LockedRect, NULL, D3DLOCK_READONLY);

	// 矩形分のバッファを作成 
	float buffer[downSampleSize*downSampleSize];

	// 全画像データをコピー
	for (int y = 0; y < downSampleSize; y++)
	{
		//Destination［入力］コピー先の開始アドレスへのポインタを指定します。
		//Source［入力］コピー元のメモリブロックの開始アドレスへのポインタを指定します。
		//Length［入力］コピーしたいメモリブロックのバイト数を指定します。
		CopyMemory(&buffer[downSampleSize * y], (char*)LockedRect.pBits + LockedRect.Pitch * y, downSampleSize * sizeof(float));
	}	

	// サーフェイスのロックを解除
	calcDownSample->GetTexture()->UnlockRect(0);

	// pixelの数
	float maxPixel = downSampleSize*downSampleSize;

	// 全pixelの輝度を加算
	luminance = 0;
	//for (int i = 0; i < maxPixel; i++)
	//{
	//	luminance += buffer[i];
	//}
	//// 最後に加算した分割って平均化
	//luminance /= maxPixel;

	//luminance += buffer[5];
	//luminance += buffer[6];
	//luminance += buffer[9];
	//luminance += buffer[10];
	//luminance += buffer[13];
	//luminance += buffer[14];
	//luminance /= 6;

	for (int i = 0; i < maxPixel; i++)
	{
		if (luminance < buffer[i])
		{
			luminance = buffer[i];
		}
	}

	// 最後に輝度により露光レベル調整
	// ・ワンポイント　滑らかに順応するようにするとよりリアルに
	
	// 何処で明るくするか
	if (luminance <= minLumi)
	{
		if (exposure <= -9.0f)
		{
			exposure += 0.004f;
		}

	}
	// 何処で暗く
	if (luminance >= maxLumi)
	{

	if (exposure >= -11.0f)
	{
		exposure -= 0.007f;
	}


	}

	// 結果を送る
	shaderD->SetValue("exposure", exposure);



#ifdef _DEBUG

	if ((GetKeyState('O') & 0x80))exposure += 0.01f;
	if ((GetKeyState('P') & 0x80))exposure -= 0.01f;
	if ((GetKeyState('K') & 0x80))exposure += 1.0f;
	if ((GetKeyState('L') & 0x80))exposure -= 1.0f;

	// 1以上の輝度はない
	if (maxLumi >= 1.0f || minLumi >= 1.0f)
	{
		assert(false);
	}


#endif

}



//*****************************************************
///						ShadowMaps
//*****************************************************

// シャドウマップの初期化　
void Deferred::CreateShadowMap(const int size)
{
	//2回以上、シャドウマップを生成させない。
	if (shadowFlag) return;

	//シャドウマップのサイズを確保
	shadowSize = size;

	//シャドウマップの生成
	shadowMap = new iex2DObj(size, size, IEX2D_FLOAT2);			//r成分にzバッファg成分にzバッファの二乗の値が入る

	//シャドウマップ用ステンシルバッファサーフェイスの作成
	if (FAILED(iexSystem::Device->CreateDepthStencilSurface(size, size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &shadowZ, NULL)))
	{
		MessageBox(iexSystem::Window, "深度バッファが作成できなかった", "ERROR", MB_OK);
	};

	shadowFlag = true;	// シャドウマップ採用
}

void Deferred::CreateShadowMapL(const int size)
{
	//シャドウマップLのサイズを確保
	shadowSizeL = size;
	//ブラー処理シャドウマップの生成
	softShadowMap = new iex2DObj(size, size, IEX2D_FLOAT2);
	shadowMapL = new iex2DObj(size, size, IEX2D_FLOAT2);

	cascadeFlag = true;	// 二重シャドウマップ採用
}

//　シャドウの更新
void Deferred::CreateShadowMatrix(Vector3 dir, Vector3 target, Vector3 playerVec, const float dist)
{

#ifdef _DEBUG
	//シャドウマップが生成されていなければ転送しない
	if (!shadowFlag){
		MessageBox(iexSystem::Window, "シャドウマップを作っていない", __FUNCTION__, MB_OK);
		exit(-1);
	}

#endif

	
	
	//	シャドウ作成
	dir.Normalize();
	Vector3 pos = target - dir * (dist*2);
	Vector3 up(.0f, 1.0f, .0f);

	//幅の指定
	float width = dist;

	//	視点とライト位置へ
	Matrix	ShadowMat, work;
	// ビュー行列の設定
	LookAtLH(ShadowMat, pos + (playerVec), target + (playerVec), up);
	// 平行投影行列作成 ビューボリューム
	OlthoLH(work, width, width, 0.02f, width * 2.5f);	//平行投影行列
	ShadowMat *= work;
	// 合成する
	shaderD->SetValue("ShadowProjection", ShadowMat);

	// アルファ調整
	shadowRange = width * .25f;

	////cascadeするならそっちの行列も作成する
	//if (!cascadeFlag) return;
	//pos = target  - dir * dist * 3.0f;			//近距離シャドウに比べ3倍ほど離す
	//up = Vector3(.0f, 1.0f, .0f);

	//D3DXMatrixIdentity(&ShadowMat);
	//D3DXMatrixIdentity(&work);
	//LookAtLH(ShadowMat, pos + (playerVec * 210), target + (playerVec * 210), up);
	//OlthoLH(work, width * 3.0f, width * 3.0f, 0.02f, width * 7.5f);
	//ShadowMat *= work;

	//shaderD->SetValue("ShadowProjectionL", ShadowMat);
}

//　シャドウの更新
void Deferred::CreateShadowMatrixL(Vector3 dir, Vector3 target, Vector3 playerVec, const float dist)
{

	if (!cascadeFlag) return;

	//	シャドウ作成
	dir.Normalize();
	Vector3 pos = target - dir * dist;
	Vector3 up(.0f, 1.0f, .0f);

	//幅の指定
	float width = dist;

	//	視点とライト位置へ
	Matrix	ShadowMat, work;

	//cascadeするならそっちの行列も作成する	
	//pos = target - dir * dist;			//近距離シャドウに比べ3倍ほど離す
	//up = Vector3(.0f, 1.0f, .0f);
	D3DXMatrixIdentity(&ShadowMat);
	D3DXMatrixIdentity(&work);
	float MulPower = 2.0f; // ★遠距離はくそ広くてＯＫ？
	LookAtLH(ShadowMat, pos + (playerVec)*MulPower, target + (playerVec)*MulPower, up);
	OlthoLH(work, width*MulPower, width*MulPower, 0.02f, width * 2.5f);
	ShadowMat *= work;

	shaderD->SetValue("ShadowProjectionL", ShadowMat);
}

//　シャドウマップ用の描画開始
void Deferred::ShadowBegin()
{
#ifdef _DEBUG
	// シャドウマップが生成されていなければ書かない
	if (!shadowFlag){
		MessageBox(iexSystem::Window, "シャドウマップが作られていない", __FUNCTION__, MB_OK);
		exit(-1);
	}
#endif


	// 現在のサーフェイスを一時保管
	iexSystem::Device->GetRenderTarget(0, &backbuffer);
	
	// 現在のステンシルバッファを一時保管
	iexSystem::Device->GetDepthStencilSurface(&backbufferZ);
	// シャドウマップ用ステンシルバッファに切り替え
	iexSystem::Device->SetDepthStencilSurface(shadowZ);

	// 現在のビューポートの一時保管
	iexSystem::Device->GetViewport(&orgViewport);
	// ビューポートの作成
	D3DVIEWPORT9 vp = { 0, 0, shadowSize, shadowSize, 0, 1.0f };
	iexSystem::Device->SetViewport(&vp);

	// シャドウマップのレンダースタート
	shadowMap->RenderTarget();

	//　画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// この関数を終えたら影を描画したい対象を描画
}
//　シャドウマップ用の描画開始_L
void Deferred::ShadowBeginL()
{
#ifdef _DEBUG
	// シャドウマップが生成されていなければ書かない
	if (!cascadeFlag){
		MessageBox(iexSystem::Window, "遠距離シャドウマップが作られていない！", __FUNCTION__, MB_OK);
		exit(-1);
	}

#endif

	// 現在のサーフェイスを一時保管
	iexSystem::Device->GetRenderTarget(0, &backbuffer);

	// 現在のステンシルバッファを一時保管
	iexSystem::Device->GetDepthStencilSurface(&backbufferZ);
	// シャドウマップ用ステンシルバッファに切り替え
	iexSystem::Device->SetDepthStencilSurface(shadowZ);

	// 現在のビューポートの一時保管
	iexSystem::Device->GetViewport(&orgViewport);
	// ビューポートの作成
	D3DVIEWPORT9 vp = { 0, 0, shadowSizeL, shadowSizeL, 0, 1.0f };
	iexSystem::Device->SetViewport(&vp);

	// シャドウマップLのレンダースタート
	shadowMapL->RenderTarget();

	//　画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// この関数を終えたら影を描画したい対象を描画（遠距離）
}

//　シャドウマップ用の描画終了
void Deferred::ShadowEnd()
{

	//shaderD->SetValue("TU", 1.0f / shadowSize); // 2でもありかも
	//shaderD->SetValue("TV", 1.0f / shadowSize);
	//// シャドウマップの解像度でブラー処理を変化	
	//for (int i = 0; i < 2; i++)
	//{
	//	shadowMap->Render(0, 0, shadowSize, shadowSize, 0, 0, shadowSize, shadowSize, shaderD, "gaussian");//弱いぼかし
	//}
	
	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	//ステンシルバッファの復元
	iexSystem::Device->SetDepthStencilSurface(backbufferZ);

	//ビューポートの復元
	iexSystem::Device->SetViewport(&orgViewport);
}
//　シャドウマップ用の描画終了_L
void Deferred::ShadowEndL()
{

	// ソフトシャドウマップにシャドウマップの結果を書き込み
	/*_____________________________________________________________________________*/
	softShadowMap->RenderTarget();
	shaderD->SetValue("TU", 1.0f / shadowSizeL); // 2でもありかも
	shaderD->SetValue("TV", 1.0f / shadowSizeL);

	// シャドウマップの解像度でブラー処理を変化	
	//for (int i = 0; i < 1; i++)
	{
		shadowMapL->Render(0, 0, shadowSizeL, shadowSizeL, 0, 0, shadowSizeL, shadowSizeL, shaderD, "gaussian");//奥行を禁止

	}
	
	/*_____________________________________________________________________________*/


	///ビューポートの復元
	iexSystem::Device->SetViewport(&orgViewport);


	// レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	// ステンシルバッファの復元
	iexSystem::Device->SetDepthStencilSurface(backbufferZ);

}

// シャドウの描画
void Deferred::RenderShadow()
{
	//シェーダーへ転送
	shaderD->SetValue("DepthTex", depth);			// 奥行用のサーフェイスを送る
	shaderD->SetValue("ShadowMap",shadowMap);
	//shaderD->SetValue("ShadowMap", softShadowMap);	// ソフトシャドウのサーフェイスをシャドウマップとして送る
	shaderD->SetValue("ShadowRange", shadowRange);	// 近距離の幅

	shadow->RenderTarget();

	//画面クリア
	//iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET, 0xFFFFFFFF, 1.0f, 0);// (?)　ZBufferを消さないようにしました?

	// 影によりスペキュラも弱めるため2番目に設定
	spec->RenderTarget(1);

	// シャドウマップを二つ利用しているかの判定
	if (!cascadeFlag)
	{
		shadow->Render(shaderD, "shadow");
	}
	else 
	{
		shaderD->SetValue("ShadowMapL", softShadowMap);// 遠距離マップを送る(ソフト)
	//	shaderD->SetValue("ShadowMapL", shadowMapL);// 遠距離マップを送る
		shadow->Render(shaderD, "DualShadow");
	}

	// 計算し作った影を後でからめる為シェーダーへ
	shaderD->SetValue("ShadowTex", shadow);

	iexSystem::Device->SetRenderTarget(1, nullptr);
}

void Deferred::ShadowSetting(const float ShadowStrength, const float AdjustValue, const float DistAlpha)
{
	shaderD->SetValue("Shadow", 1.0f - ShadowStrength);
	shaderD->SetValue("AdjustValue", AdjustValue);
	shaderD->SetValue("DistAlpha", 1.0f - DistAlpha);
}


//***********************************
///				Render
//***********************************

// 最後のスクリーンを作る
void Deferred::FinalResultDeferred()
{

	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//シェーダーへ転送
	shaderD->SetValue("LightTex", light);
	shaderD->SetValue("SpecTex", spec);

	// 通常描画
	DefaultRender();

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);

}

/// 描画
void Deferred::RenderDeferred(const int outX, const int outY, const int W, const int H, const int inX, const int inY)
{
	// FinalPassSurfaceを移す
	screen->Render(outX, outY, W, H, 0, 0, inX, inY);

}

// 被写界深度
void Deferred::RenderDOF(const Vector3 target, const float range)
{
	//現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//********************************************
	///		最終結果screenに被写界深度をかける
	//********************************************
	dof->RenderTarget();

	//targetをビュー座標系に変換する
	Matrix mWV = matView;
	D3DXVECTOR3 Vt(target.x, target.y, target.z);
	D3DXVec3TransformCoord(&Vt, &Vt, &mWV);
	Vector3 Target(Vt.x, Vt.y, Vt.z);

	//画面クリア		αは抜いておく
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	//シェーダーへ必要パラメータを転送
	shaderD->SetValue("DOF_target", Target);
	shaderD->SetValue("DOF_Range", range);

	// ToneMap+近い所のアルファを切り取る処理 
	screen->Render(0, 0, Reduction_x, Reduction_y, 0, 0, 1280, 720, shaderD, "def_DOF");	

	//********************************************
	///		↑で作成したイメージをぼかす
	//********************************************

	//シェーダーに1テクセルの値を転送
	shaderD->SetValue("TU", 1.0f / (float)Reduction_x);
	shaderD->SetValue("TV", 1.0f / (float)Reduction_y);

	//レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
	
	//画面クリア
	//iexSystem::GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

	//縮小バッファを使用したので引き延ばす
	dof->Render(0, 0, 1280, 720, 0, 0, Reduction_x, Reduction_y, shaderD, "gaussian");


}

/// デフォルト　平行光＋スペキュラ
void Deferred::DefaultRender()
{
	// シャドウサーフェイスの更新
	if (shadowFlag==true)
	{
		RenderShadow();
	}

	// FinalPassSurfaceに今までのSurfaceの情報を全て合わせる
	screen->RenderTarget();

	// 画面クリア
	// しかし毎回全て描画するため問題なさそうなのでコメントアウト
	//iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	if (shadowFlag == true)
	{
		diffuse->Render(shaderD, "finalPut");		//通常描画
	}
	else
	{
		diffuse->Render(shaderD, "finalPut_notShadow");		//影なし描画
	}
	
	// 中でトーンマッピング
	screen->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "ToneMap");

	//forward->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight,shaderD,"HDR_Lighting");


}

/*****************************/
//	OLLRender
/*****************************/
void Deferred::ClearAllRender()
{

	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// フォワードをセット
	all->RenderTarget(0);
	// フォワード画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);


}

void Deferred::AllBegin()
{
	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// forward用サーフェイスに変える
	all->RenderTarget(0);



}

void Deferred::AllEnd()
{
	// ★今は　環境用に
	//作った情報テクスチャを転送
	//shaderD->SetValue("EnvFullBuf", all);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);


}

void Deferred::AllRender()
{



}


//******************************************
///				texture
//******************************************

iex2DObj* Deferred::GetTex(const int type)
{
	iex2DObj* ret = nullptr;

	switch (type){
	case DIFFUSE:
		ret = diffuse;
		break;
	case NORMAL:
		ret = normal;
		break;
	case SPECULAR:
		ret = specular;
		break;
	case DEPTH:
		ret = depth;
		break;
	case LIGHT:
		ret = light;
		break;
	case SPEC:
		ret = spec;
		break;
	case DOF:
		ret = dof;
		break;
	case SHADOW:
		ret = shadow;
		break;
	case SHADOWMAP:
		if (!shadowFlag)
		MessageBox(iexSystem::Window, "影を作っていない！", __FUNCTION__, MB_OK);

		ret = shadowMap;
		break;
	case SHADOWMAPL:
		if (!cascadeFlag)
		MessageBox(iexSystem::Window, "二重影にしていない！", __FUNCTION__, MB_OK);
		
		ret = shadowMapL;
		break;
	case BLOOMSCREEN:
		ret = bloomScreen;
		break;
	case BLOOM:
		ret = bloom;
		break;
	case GLOWSCREEN:
		ret = glowScreen;
		break;
	case GLOW:
		ret = glow;
		break;
	case FORWARD:
		ret = forward;
		break;
	case DOWNSAMPLE:
		ret = downSample;
		break;
	case INFOPL:
		ret = infoPointLight;
		break;
	case SCREEN:
		ret = screen;
		break;
	case ALLSCREEN:
		ret = all;
		break;
	}

	return ret;
}