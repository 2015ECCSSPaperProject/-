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

/// 初期化
void Deferred::Init()
{
	// G_Bufferを作成
	diffuse	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET); // RGB アルべド
	normal	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);	 // RGB 法線　A アンビエントレート
	specular= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET); // R スペキュラ G ブルーム　B リムライト
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

	// Bloom
	bloomScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);			//　ゲーム画面をHDR用にいじる用
	bloom = new iex2DObj(MiniTex_x, MiniTex_y, IEX2D_HDR);		// HDR

	// Glow
	glowScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);	//　ゲーム画面をGlow用にいじる用
	glow = new iex2DObj(MiniTex_x, MiniTex_y, IEX2D_HDR);		// Glow

	// SSAO
	ssao = nullptr;
	ssaoFlag = false;

	// BackBuffer
	iexSystem::Device->GetRenderTarget(0, &backbuffer);
	iexSystem::Device->GetDepthStencilSurface(&backbufferZ);
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);

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

	// ssaoSurface
	if (ssaoFlag)
	{
		ReleaseSurface(ssao);
		ReleaseSurface(ssaoScreen);
	}
	

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
void Deferred::Bigin()
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
void Deferred::End()
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

	//SSAOを導入するなら、プロジェクション変換行列を転送
	if (ssaoFlag) shaderD->SetValue("matProjection", matProjection);
	//  多分送られているのでかかない 
	//	シェーダーに必要な情報を送る
	//shaderD->SetValue("matView", matView);

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

	//ビュー座標系に変換
	LightDir.x = dir.x * matV._11 + dir.y * matV._21 + dir.z * matV._31;
	LightDir.y = dir.x * matV._12 + dir.y * matV._22 + dir.z * matV._32;
	LightDir.z = dir.x * matV._13 + dir.y * matV._23 + dir.z * matV._33;


	LightDir.Normalize();

	//	シェーダー設定 shaderDに送る
	shaderD->SetValue("LightVec", LightDir);
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

/// ポイントライト
void Deferred::PointLight(const Vector3 pos, const Vector3 color, const float range, const float power)
{

	Matrix mWV = matView;// ビュー行列

	// 視錐台カリング処理
	Vector3 flont(mWV._13, mWV._23, mWV._33);
	flont.Normalize();
	Vector3 dir = pos - (ViewPos - flont * range);			//レンジの値分下げて、カリングを緩める。
	dir.Normalize();

	float dot = Vector3Dot(dir, flont);
	if (dot < .5f) return;

	// ライト位置をビュー座標系に変換
	Vector3 PointLight;
	D3DXVECTOR3 plight = D3DXVECTOR3(pos.x, pos.y, pos.z);
	D3DXVec3TransformCoord(&plight, &plight, &mWV);
	PointLight = Vector3(plight.x, plight.y, plight.z);

	// 必要パラメータの転送
	shaderD->SetValue("LightPos", PointLight);
	shaderD->SetValue("LightColor", (Vector3)color);
	shaderD->SetValue("LightRange", range);
	shaderD->SetValue("LightPower", power);

	// 現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	// レンダーターゲットの切替え
	light->RenderTarget();
	spec->RenderTarget(1);


	// レンダリング
	normal->Render(shaderD, "def_PointLight");

	// レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);

}

/// シンプルポイントライト
void Deferred::SimpliPointLight(const Vector3 pos, const Vector3 color, const float range)
{

	Matrix mWV = matView;// ビュー行列

	// 視錐台カリング処理
	Vector3 flont(mWV._13, mWV._23, mWV._33);
	flont.Normalize();
	Vector3 dir = pos - (ViewPos - flont * range);			//レンジの値分下げて、カリングを緩める。
	dir.Normalize();

	float dot = Vector3Dot(dir, flont);
	if (dot < .5f) return;

	// ライト位置をビュー座標系に変換
	Vector3 PointLight;
	D3DXVECTOR3 plight = D3DXVECTOR3(pos.x, pos.y, pos.z);
	D3DXVec3TransformCoord(&plight, &plight, &mWV);
	PointLight = Vector3(plight.x, plight.y, plight.z);

	// 必要パラメータの転送
	shaderD->SetValue("LightPos", PointLight);
	shaderD->SetValue("LightColor", (Vector3)color);
	shaderD->SetValue("LightRange", range);

	// 現在のレンダーターゲットを一時的に確保
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	// レンダーターゲットの切替え
	light->RenderTarget();
	spec->RenderTarget(1);

	// レンダリング
	normal->Render(shaderD, "def_SimpliPointLight");

	// レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);
}
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

};
void Deferred::EndDrawBloom()
{
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

	shaderD->SetValue("TU", (float)1 / (float)MiniTex_x);
	shaderD->SetValue("TV", (float)1 / (float)MiniTex_y);

	// 画面の明るい部分をブルームの種として抽出
	bloomScreen->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "make_hdrBloomSeed");

	//ボケ度　回数が少ないほどボケない
	for (int i = 0; i < 2; i++)
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
		glow->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, MiniTex_x, MiniTex_y, shaderD, "gaussZ", 0xFFFFFFFF);//奥行を禁止
	}

	iexSystem::Device->SetViewport(&save_viewport);
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

	//明るさ　回数が多いほど明るく
	for (int i = 0; i < 2; i++)
	{
		glow->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, MiniTex_x, MiniTex_y, RS_ADD, 0xffffffff);
	}

};

//****************************
///		SSAO
//****************************
void Deferred::CreateSSAO()
{
	// SSAOを追加するフラグを立てる
	ssaoFlag = true;

	// SSAO用のサーフェイスを作る
	ssaoScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);	//　SSAOの結果を入れる
	ssao = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);		// SSAOの結果をぼかし最終出力

}

void Deferred::ClearSSAO()
{
	// 今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// glowを描画する元の
	ssaoScreen->RenderTarget();
	iexSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

}


void Deferred::SSAORender()
{
	// 保存用サーフェイス
	Surface* save;
	// まずは今のサーフェイスを保存
	iexSystem::GetDevice()->GetRenderTarget(0, &save);

	// SSAOを導入
	if (ssaoFlag == true)
	{
		ssao->RenderTarget();// SSAOに切り替え
		iexSystem::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		
		shaderD->SetValue("TU", (float)1 / (float)iexSystem::ScreenWidth);
		shaderD->SetValue("TV", (float)1 / (float)iexSystem::ScreenHeight);

		ssaoScreen->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "ssao");	

		//ボケ度　回数が少ないほどボケない
		for (int i = 0; i < 2; i++)
		{
			ssao->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "gaussZ", 0xFFFFFFFF);//奥行を禁止
		}

	}
	
	//　元のサーフェイスへ戻す
	iexSystem::GetDevice()->SetRenderTarget(0, save);

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
		//OutputDebugString("Don't create shadowmap!!\n"); _PLACE_; return;
		MessageBox(iexSystem::Window, "Don't create shadowmap!!", __FUNCTION__, MB_OK);
		exit(-1);
	}

#endif

	
	
	//	シャドウ作成
	dir.Normalize();
	Vector3 pos = target - dir * dist;
	Vector3 up(.0f, 1.0f, .0f);

	//幅の指定
	float width = dist;

	//	視点とライト位置へ
	Matrix	ShadowMat, work;
	// ビュー行列の設定
	LookAtLH(ShadowMat, pos + (playerVec * 90), target + (playerVec * 90), up);
	// 平行投影行列作成 ビューボリューム
	OlthoLH(work, width, width, 0.02f, width * 2.5f);	//平行投影行列
	ShadowMat *= work;
	// 合成する
	shaderD->SetValue("ShadowProjection", ShadowMat);

	// アルファ調整
	shadowRange = width * .25f;

	//cascadeするならそっちの行列も作成する
	if (!cascadeFlag) return;
	pos = target  - dir * dist * 3.0f;			//近距離シャドウに比べ3倍ほど離す
	up = Vector3(.0f, 1.0f, .0f);

	D3DXMatrixIdentity(&ShadowMat);
	D3DXMatrixIdentity(&work);
	LookAtLH(ShadowMat, pos + (playerVec * 210), target + (playerVec * 210), up);
	OlthoLH(work, width * 3.0f, width * 3.0f, 0.02f, width * 7.5f);
	ShadowMat *= work;

	shaderD->SetValue("ShadowProjectionL", ShadowMat);
}

//　シャドウマップ用の描画開始
void Deferred::ShadowBegin()
{
#ifdef _DEBUG
	// シャドウマップが生成されていなければ書かない
	if (!shadowFlag){
		//OutputDebugString("Don't create shadowmap!!\n"); _PLACE_; return;
		MessageBox(iexSystem::Window, "Don't create shadowmap!!", __FUNCTION__, MB_OK);
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
		//OutputDebugString("Don't create shadowmap!!\n"); _PLACE_; return;
		MessageBox(iexSystem::Window, "Don't create cascadeFlag!!", __FUNCTION__, MB_OK);
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
	shaderD->SetValue("TU", 1.0f / shadowSizeL);
	shaderD->SetValue("TV", 1.0f / shadowSizeL);

	// シャドウマップの解像度でブラー処理を変化	
	shadowMapL->Render(0, 0, shadowSizeL, shadowSizeL, 0, 0, shadowSizeL, shadowSizeL, shaderD, "gaussZ");//奥行を禁止

	/*_____________________________________________________________________________*/

	// レンダーターゲットの復元
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	// ステンシルバッファの復元
	iexSystem::Device->SetDepthStencilSurface(backbufferZ);

	///ビューポートの復元
	iexSystem::Device->SetViewport(&orgViewport);
}

// シャドウの描画
void Deferred::RenderShadow()
{
	//シェーダーへ転送
	shaderD->SetValue("DepthTex", depth);			// 奥行用のサーフェイスを送る
	shaderD->SetValue("ShadowMap",shadowMap);
	//shaderD->SetValue("ShadowMap", softShadowMap);	// ソフトシャドウのサーフェイスをシャドウマップとして送る
	shaderD->SetValue("ShadowRange", shadowRange);

	shadow->RenderTarget();

	//画面クリア
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// 影によりスペキュラも弱めるため2番目に設定
	spec->RenderTarget(1);

	// シャドウマップを二つ利用しているかの判定
	if (!cascadeFlag)
	{
		shadow->Render(shaderD, "shadow");
	}
	else 
	{
		shaderD->SetValue("ShadowMapL", softShadowMap);// 遠距離マップを送る
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
///				Rendering
//***********************************
/// 描画
void Deferred::Render(const int outX, const int outY, const int W, const int H, const int inX, const int inY)
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

	// FinalPassSurfaceを移す
	screen->Render(outX, outY, W, H, 0, 0, inX, inY,shaderD,"ToneMap");
	
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
	screen->Render(0, 0, Reduction_x, Reduction_y, 0, 0, 1280, 720, shaderD, "ToneMap_DOF");	

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
		if (ssaoFlag==true){
			shaderD->SetValue("SSAOTex", ssao);
			diffuse->Render(shaderD, "def_mix3");
		}
		else
		{
			diffuse->Render(shaderD, "def_mix1");		//通常描画
			//diffuse->Render();
		}
		
	}
	else
	{
		diffuse->Render(shaderD, "def_mix2");		//影なし描画
	}
	

}

//******************************************
///				Debug
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
		MessageBox(iexSystem::Window, "Don't on shadowFlag!!", __FUNCTION__, MB_OK);

		ret = shadowMap;
		break;
	case SHADOWMAPL:
		if (!cascadeFlag)
		MessageBox(iexSystem::Window, "Don't on cascadeFlag!!", __FUNCTION__, MB_OK);
		
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
	case SSAOSCREEN:
		if (!ssaoFlag)
			MessageBox(iexSystem::Window, "SSAOマップが作られていない", __FUNCTION__, MB_OK);
		ret = ssaoScreen;
		break;
	case SSAO:
		if (!ssaoFlag)
		MessageBox(iexSystem::Window, "SSAOマップが作られていない", __FUNCTION__, MB_OK);
		ret = ssao;
		break;
	case SCREEN:
		ret = screen;
		break;
	}

	return ret;
}