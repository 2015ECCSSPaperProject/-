//********************************************************************
//																									
//		３Ｄ用シェーダー		
//
//********************************************************************
//------------------------------------------------------
//		環境関連
//------------------------------------------------------
float4x4 Projection;	//	投影変換行列
float4x4 TransMatrix;	//　ワールド変換
float4x4 matView;		//　ビュー変換
//------------------------------------------------------
//		テクスチャサンプラー	
//------------------------------------------------------
//WRAP...反対側から同じ絵を繰り返します
//CLAMP...端の色を繰り返します
//BORDER...透明色にします
texture Texture;
sampler DecaleSamp = sampler_state
{
    Texture = <Texture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};
texture NormalMap;	//	法線マップテクスチャ
sampler NormalSamp = sampler_state
{
	Texture = <NormalMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};

texture SpecularMap;	//	スペキュラマップテクスチャ
sampler SpecularSamp = sampler_state
{
	Texture = <SpecularMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};

texture HeightMap;		//	高さマップテクスチャ
sampler HeightSamp = sampler_state
{
	Texture = <HeightMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};
//**************************************************************************************************
//
//		ライティング 　
//
//**************************************************************************************************

//**************************************************************************************************
//	半球ライティング
//**************************************************************************************************
float3 SkyColor = { 0.65f, 0.55f, 0.55f };
float3 GroundColor = { 0.65f, 0.55f, 0.55f };

inline float4 HemiLight(float3 normal)//　上下の環境光を分ける
{
	float4 color;
	float rate = (normal.y*0.5f) + 0.5f;
	color.rgb = SkyColor * rate;
	color.rgb += GroundColor * (1 - rate);
	color.a = 1.0f;

	return color;
}

//**************************************************************************************************
//	平行光　　
//**************************************************************************************************
float3 LightDir = { 1.0f, -1.0f, 0.0f };		//　ライトの方向
float3 DirLightColor = { 0.85f, 0.8f, 0.8f };	//　ライトの色

inline float3 DirLight(float3 dir, float3 normal)//　ライトの向きで光の色の強さを返す
{
	float3 light;
	float rate = max(0.0f, dot(-dir, normal));//　光の計算　内積で光の反射を求める
	light = DirLightColor * rate;//

	return light;
}

//**************************************************************************************************
//	スペキュラ　視点と物体の角度によって光源自身が映り込んでできるハイライト　　
//**************************************************************************************************
float3	ViewPos = { 0.0f, -1.0f, 1.0f };	//　目線のポジション
inline float Specular(float3 pos, float3 normal)
{
	float   sp;//　スペキュラの情報を入れる変数

	float3	H = normalize(ViewPos - pos);//
		H = normalize(H - LightDir);

	sp = dot(normal, H);
	sp = max(0, sp);
	sp = pow(sp, 10);

	return sp;
}
//------------------------------------------------------
//		頂点フォーマット
//------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos		: POSITION;
    float4 Color	: COLOR0;
    float2 Tex		: TEXCOORD0;

	float4 wPos			: TEXCOORD1;//　ピクセルに送る情報にワールドからのポジション追加
	float4 ProjectionPos: TEXCOORD2;//　ピクセルに送る情報にゲーム画面ラストのポジション追加
};

struct VS_INPUT
{
    float4 Pos    : POSITION;
    float3 Normal : NORMAL;
	float4 Color  : COLOR0;
    float2 Tex	  : TEXCOORD0;
};

//------------------------------------------------------
//		シャドウ関連のサンプラーとプロジェクション
//------------------------------------------------------
texture	Far_ShadowMap;//遠距離シャドウマップ 
sampler Far_ShadowSamp = sampler_state
{
	Texture = <Far_ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	BorderColor = 0xFFFFFFFF;
	AddressU = BORDER;
	AddressV = BORDER;

};

texture	Near_ShadowMap;//近距離シャドウマップ 
sampler Near_ShadowSamp = sampler_state
{
	Texture = <Near_ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	BorderColor = 0xFFFFFFFF;
	AddressU = BORDER;
	AddressV = BORDER;

};
//ShadowBuf以外で送るプロジェクション
float4x4 Far_ShadowProjection;	//　遠距離
float4x4 Near_ShadowProjection;	//	近距離

//------------------------------------------------------
//		フォグ関連
//------------------------------------------------------
float	FogNear = 200.0f;	//	何処からフォグを出すか　Farの差を広げると滑らかに
float	FogFar	= 600.0f;	//	何処から完全にフォグの色にするか
float3	FogColor = { 0.55f, 0.65f, 0.8f };//	霧の色

//------------------------------------------------------
//		シャドウ関連のファンクション
//------------------------------------------------------

float	AdjustValue =  -0.002f;//　ゆるくし過ぎると全部影になる　超恐ろしいので注意
float	ShadowLevel = 0.5f;
//スクリーンと合成
inline float4 mulScreen(float4 Pos, float4x4 mat)
{
	float4 Tex;
	//テクスチャ座標計算
	float4 p;
	p = Pos;
	p = mul(p, mat);
	p.xyz /= p.w;
	//スクリーン変換
	p.y = -p.y;
	p.xy = 0.5f * p.xy + 0.5f;
	Tex = p;
	return Tex;
}
//影の色を塗る
inline float GetSM(float3 Tex, sampler samp)
{
	//シャドウマップから深度を取り出す
	float d = tex2D(samp, Tex.xy).r;
	//ここに例外処理を入れる
	if (d == 1) return 1;
	float l = (d < Tex.z + AdjustValue) ? ShadowLevel : 1;	//←判定
	return l;
}
//シグモイド
inline float sigmoid(float gain, float x, float adjust)
{
	//
	return 1.0 / (1.0 + exp(-gain * (x - adjust)));
}


//------------------------------------------------------
//	環境マップ用　周りの風景が映りこむマップ
//------------------------------------------------------
texture EnvMap;		//	環境テクスチャ
sampler EnvSamp = sampler_state
{
	Texture = <EnvMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};
//　EnvSamp　はここを通して使用↓　　
inline float4 Environment(float3 normal)
{
	float4	color;

	float2	uv;
	uv.x = normal.x*0.5f + 0.5f;
	uv.y = normal.y*0.5f + 0.5f;

	color = tex2D(EnvSamp, uv);
	return color;
}

float	EnvParam = 0.3f;	//	映りこみの割合

#include	"DATA\\Shader\\ShadowMap.inc"

//********************************************************************
//
//		基本３Ｄシェーダー		
//
//********************************************************************
//------------------------------------------------------
//		頂点シェーダー	
//------------------------------------------------------
VS_OUTPUT VS_Basic( VS_INPUT In )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
	//TransMatrixとPosを合成してwPosの情報生成
	//float4 wpos;
	//wpos =  mul(In.Pos, TransMatrix);
	Out.wPos = mul(In.Pos, TransMatrix);

    Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color = 1.0f;

	Out.ProjectionPos = Out.Pos;

    return Out;
}
//------------------------------------------------------
//		ピクセルシェーダー	
//------------------------------------------------------
float4 PS_Basic( VS_OUTPUT In) : COLOR
{   
	float4	OUT;
	//	ピクセル色決定
	OUT = In.Color * tex2D( DecaleSamp, In.Tex );

	return OUT;
}

//------------------------------------------------------
//		影描画テクニック
//------------------------------------------------------
float4 PS_Shadow(VS_OUTPUT In) : COLOR
{
	float4	OUT;
	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);

	//シャドウ
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 11.0));//左の数値で丁度いい場所を探す
	OUT.rgb *= s;

	return OUT;
}
//------------------------------------------------------
//		通常描画テクニック
//------------------------------------------------------
technique copy
{
    pass P0
    {
		AlphaBlendEnable = true;
		BlendOp          = Add;
		SrcBlend         = SrcAlpha;
		DestBlend        = InvSrcAlpha;
		CullMode         = CCW;
		ZEnable          = true;

		VertexShader = compile vs_2_0 VS_Basic();
		PixelShader  = compile ps_2_0 PS_Basic();
    }
}
// 乗算
technique add
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZWriteEnable = false;

		VertexShader = compile vs_3_0 VS_Basic();
		PixelShader = compile ps_3_0 PS_Basic();
	}
}

// シャドウ追加
technique shadow
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;

		VertexShader = compile vs_3_0 VS_Basic();
		PixelShader = compile ps_3_0 PS_Shadow();
	}
}

//**************************************************************************************************
//
//		ＵＶアニメーション用 
//
//**************************************************************************************************

float uvMove;//　アニメーション

VS_OUTPUT VS_UvAni( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	Out.Pos   = mul(In.Pos, Projection);
	Out.Color = In.Color;
	Out.Tex   = In.Tex + float2(uvMove, .0f);// 座標

	return Out;
}

technique uvAnim
{
    pass P0
    {
		AlphaBlendEnable = true;
		BlendOp          = Add;
		SrcBlend         = SrcAlpha;
		DestBlend        = InvSrcAlpha;
		CullMode         = CCW;
		ZEnable          = true;

		VertexShader = compile vs_3_0 VS_UvAni();
		PixelShader  = compile ps_3_0 PS_Basic();
    }
}

//**************************************************************************************************
//
//		投影テクスチャ 　（迷彩）
//
//**************************************************************************************************

texture TEXMap;		//	仮投影マップ
sampler TEXSamp = sampler_state
{
	Texture = <TEXMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Clamp;
	AddressV = Clamp;
};

// struct VertexInput
struct TEX_In
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
};

// struct VertexOutput
struct TEX_Out
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
};
float4x4 TextureMatrix;

float	NormalScale = 1.0f;
//-----------------------------------------------------------------------------------------------------
// Name : 迷彩
// Desc : 頂点シェーダ
//-----------------------------------------------------------------------------------------------------
TEX_Out VS_TEX(TEX_In input)
{
	TEX_Out output = (TEX_Out)0;

	//　座標変換
	output.Position = mul(input.Position, Projection);//　逆で×ととんでもないことになるので注意!

	//　射影テクスチャ用
	float3 wvpNormal = normalize(mul(input.Normal, (float3x3)Projection));
		output.TexCoord = (output.Position.xy + wvpNormal.xy * NormalScale) / output.Position.z;
	output.TexCoord = output.TexCoord * float2(0.5f, -0.5f) + 0.5f;

	//　リフレクトリフレクトマップ
	//output.TexCoord = (input.Position.xy / input.Position.w)*0.5f + 0.5f;
	//output.TexCoord = -output.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------------------
// Name : 迷彩
// Desc : ピクセルシェーダ
//-----------------------------------------------------------------------------------------------------
float4 PS_TEX(TEX_Out input) : COLOR
{
	float4 output;

	//　テクスチャカラー
	output = tex2D(TEXSamp, input.TexCoord);
	output.a = 1.0f;

	return output;
}

//------------------------------------------------------
//		迷彩TEXテクニック
//------------------------------------------------------
technique TEX
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;

		VertexShader = compile vs_3_0 VS_TEX();
		PixelShader = compile ps_3_0 PS_TEX();
	}
}

//********************************************************************
//
//		反射マップ作成
//
//********************************************************************

//------------------------------------------------------
//	反射マップ
//------------------------------------------------------
texture RefMap;		//	反射マップテクスチャ
sampler RefSamp = sampler_state
{
	Texture = <RefMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};

//------------------------------------------------------
//		頂点フォーマット
//------------------------------------------------------
struct VS_OUTPUT_REFLECT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float4 wPos		: TEXCOORD1;	//	★ピクセルに送る情報にワールドからのポジション追加
	float4 ProjectionPos: TEXCOORD2;//　★ピクセルに送る情報にゲーム画面ラストのポジション追加
};

struct VS_INPUT_REFLECT
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
};


float m_Height = 0.0f;

//------------------------------------------------------
//	頂点シェーダ
//------------------------------------------------------
VS_OUTPUT_REFLECT VS_Reflect(VS_INPUT_REFLECT In)
{
	VS_OUTPUT_REFLECT Out = (VS_OUTPUT_REFLECT)0;

	In.Pos.y = -In.Pos.y;

	//TransMatrixとPosを合成してwPosの情報生成
	//float4 wpos;
	//wpos =  mul(In.Pos, TransMatrix);
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	
	Out.Tex = In.Tex;
	Out.Color = 1.0f;

	Out.ProjectionPos = Out.Pos;

	return Out;
}

//------------------------------------------------------
//		ピクセルシェーダー	
//------------------------------------------------------

float4 PS_Reflect(VS_OUTPUT_REFLECT In) : COLOR
{
	//反射イメージのワールド座標形状での頂点の Y 座標がm_Height変数より高いとき描画しないようにする
	//これにより水面下のイメージのみレンダリングする
	clip(m_Height - In.wPos.y);//指定された値が 0 より小さい場合に、現在のピクセルを破棄します。
	float4	OUT;
	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);

	return OUT;
}

//------------------------------------------------------
//		テクニック	
//------------------------------------------------------
technique Reflect
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZWriteEnable = true;
		CullMode = cw;

		VertexShader = compile vs_3_0 VS_Reflect();
		PixelShader = compile ps_3_0 PS_Reflect();
	}
}

//**************************************************************************************************
//
//		FULLエフェクト
//
//**************************************************************************************************

struct VS_FULL_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;
	// ライティング
	float4 Light	: COLOR1;
	float4 Ambient	: COLOR2;
	float3 Normal	: COLOR3;
	float3 vLight	: TEXCOORD1;
	float3 vE		: TEXCOORD2;
	// 行列
	float4 wPos		: TEXCOORD3;//		★ピクセルに送る情報にワールドからのポジション追加
	float4 ProjectionPos: TEXCOORD4;//	★ピクセルに送る情報にゲーム画面ラストのポジション追加
	float3 worldNormal : TEXCOORD5;		//	ワールドの法線
};


//------------------------------------------------------
//	頂点シェーダ
//------------------------------------------------------
VS_FULL_OUTPUT VS_Full(VS_INPUT In)
{
	VS_FULL_OUTPUT Out = (VS_FULL_OUTPUT)0;

	Out.Pos = mul(In.Pos, Projection);//	プロジェクション行列と合成
	Out.ProjectionPos = Out.Pos;
	Out.Color.a = 1;
	Out.Tex = In.Tex;//	テクスチャを送る

	float4 P = mul(In.Pos, TransMatrix);//	ワールド座標に持っていく
	Out.wPos = mul(In.Pos, TransMatrix);//
	float3x3	mat = TransMatrix;//		float4x4のTransMatrixの行列を　float3x3のmatに
	Out.Normal = mul(In.Normal, mat);
	Out.Normal = normalize(Out.Normal);

	// 半球ライティング
	Out.Ambient.rgb = HemiLight(Out.Normal);

	// 頂点ローカル座標系算出
	float3	vx;
	float3	vy = { 0, 1, 0.001f };
	vx = cross(vy, Out.Normal);
	vx = normalize(vx);
	vy = cross(vx, Out.Normal);
	vy = normalize(vy);

	//	ライトベクトル補正 ライトが当たった時の角度
	Out.vLight.x = dot(vx, LightDir);
	Out.vLight.y = dot(vy, LightDir);
	Out.vLight.z = dot(Out.Normal, LightDir);

	// 視線ベクトル補正
	float3 E = P - ViewPos;   // 視線ベクトル
	Out.vE.x = dot(vx, E);
	Out.vE.y = dot(vy, E);
	Out.vE.z = dot(Out.Normal, E);

	//	フォグ計算
	Out.Ambient.a = (FogFar - Out.Pos.z) / (FogFar - FogNear);
	Out.Ambient.a = saturate(Out.Ambient.a);//指定された値を 0 ～ 1 の範囲にクランプします

	//	グローバルポジション&法線
	float3x3   m = (float3x3)TransMatrix;
	Out.worldNormal = mul(In.Normal, m);

	return Out;
}


//------------------------------------------------------
//	ピクセルシェーダ
//------------------------------------------------------
float4 PS_Full(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

	//	視差
	//float h = tex2D(HeightSamp, Tex).r - 0.5f;
	//　目線
	float3 E = normalize(In.vE);
	//Tex.x += 0.04f * h * E.x;
	//Tex.y -= 0.04f * h * E.y;


	//	法線取得
	float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
	//	ライト計算
	In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, N);

	//
	In.Color.rgb = light + In.Ambient;//Ambient=環境光＋

	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, Tex);

	//	スペキュラマップ取得
	float4 sp_tex = tex2D(SpecularSamp, Tex);
	//	視線反射ベクトル
	float3 R = reflect(-E, N);
	//	スペキュラ
	OUT.rgb += pow(max(0, dot(R, In.vLight)), 30) * (sp_tex);

	//　リムライティング 丸いカービィなどによく合う
	//float3 Rim = RimColor * Kr * pow(1 - dot(N, -E), Power);
	//OUT.rgb += Rim;

	//シャドウ
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 12.0));//左の数値で丁度いい場所を探す
	//s = lerp(s, 1, sigmoid(2.0, (depth / 3.0), 33.0));

	OUT.rgb *= s;

	//	フォグ採用
	OUT.rgb = (OUT.rgb * In.Ambient.a) + (FogColor * (1 - In.Ambient.a));

	return OUT;
}

technique FULL
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Full();
		PixelShader = compile ps_3_0 PS_Full();
	}


}


//リム
float Kr = 0.5f;//上げると色が強く
float Power = 2.5f;//絞る数値
float4 RimColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

//------------------------------------------------------
//	ピクセルシェーダ
//------------------------------------------------------
float4 PS_Full_Teki(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

		//	視差
		//float h = tex2D(HeightSamp, Tex).r - 0.5f;
		//　目線
		float3 E = normalize(In.vE);

		//	法線取得
		float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
		//	ライト計算
		In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, N);

		//
		In.Color.rgb = light + In.Ambient;//Ambient=環境光＋

	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, Tex);

	//	スペキュラマップ取得
	float4 sp_tex = tex2D(SpecularSamp, Tex);
		//	視線反射ベクトル
		float3 R = reflect(-E, N);
		//	スペキュラ
		OUT.rgb += pow(max(0, dot(R, In.vLight)), 30) * (sp_tex);

	//　リムライティング 丸いカービィなどによく合う
	float3 Rim = RimColor * Kr * pow(1 - dot(N, -E), Power);
		OUT.rgb += Rim;

	//シャドウ
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 12.0));//左の数値で丁度いい場所を探す
	OUT.rgb *= s;

	//	フォグ採用
	OUT.rgb = (OUT.rgb * In.Ambient.a) + (FogColor * (1 - In.Ambient.a));

	return OUT;
}

technique FULL_TEKI
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Full();
		PixelShader = compile ps_3_0 PS_Full_Teki();
	}
}

/*環境マップ考慮*/

//	ピクセルシェーダ
//------------------------------------------------------
float4 PS_Env(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

		//　目線
		float3 E = normalize(In.vE);

		//	法線取得
		float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
		//	ライト計算
		In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, N);

		//
		In.Color.rgb = light + In.Ambient;//Ambient=環境光＋

	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, Tex);

	float4 Env = Environment(In.worldNormal);
		//	環境マップブレンド
		OUT = (OUT * (1 - EnvParam)) + (Env * EnvParam);

	//	スペキュラマップ取得
	float4 sp_tex = tex2D(SpecularSamp, Tex);
		//	視線反射ベクトル
		float3 R = reflect(-E, N);
		//	スペキュラ
		OUT.rgb += pow(max(0, dot(R, In.vLight)), 25) * (sp_tex);

	//　リムライティング 丸いカービィなどによく合う
	float3 Rim = RimColor * (Kr+0.3f) * pow(1 - dot(N, -E), Power);
	OUT.rgb += Rim;

	//シャドウ
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 12.0));//左の数値で丁度いい場所を探す
	//s = lerp(s, 1, sigmoid(2.0, (depth / 3.0), 33.0));
	OUT.rgb *= s;

	//	フォグ採用
	//OUT.rgb = (OUT.rgb * In.Ambient.a) + (FogColor * (1 - In.Ambient.a));



	return OUT;
}


//------------------------------------------------------
//		環境マッピング
//------------------------------------------------------
technique FULL_RINGO
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Full();
		PixelShader = compile ps_3_0 PS_Env();
	}
}

technique FULL_SUIKA
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Full();
		PixelShader = compile ps_3_0 PS_Full_Teki();
	}
}


//**************************************************************************************************
//
//		トゥーン 
//
//**************************************************************************************************

//-----------------------------------------------
//		トゥーン用頂点
//-----------------------------------------------
struct VS_OUTPUT_TOON
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float  Level : TEXCOORD1;
};

float OutlineSize = 0.1f;	//輪郭線の太さ

//影用情報
float ToonLevel = 0.5f;		//影のレベル
float ToonShadow = 0.4f;	//影の濃さ


/*		アウトライン用頂点シェーダー		*/
VS_OUTPUT VS_Outline(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	//法線方向に拡大
	In.Normal = normalize(In.Normal);
	In.Pos.xyz = In.Pos.xyz + (OutlineSize * In.Normal);

	//投影変換
	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color.rgb = 0;
	Out.Color.a = 1;

	return Out;
}
/*		輪郭線(トゥーン)用ピクセルシェーダー		*/
float4 PS_Outline(VS_OUTPUT In) : COLOR
{
	float4 OUT;

	//ピクセル色決定
	OUT = In.Color;

	return OUT;
}


/*		トゥーン影シェーダー		*/
VS_OUTPUT_TOON VS_Toon(VS_INPUT In)
{
	VS_OUTPUT_TOON Out = (VS_OUTPUT_TOON)0;

	//投影変換
	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color = 1;

	//法線変換
	float3x3 mat = TransMatrix;
		float3 N = mul(In.Normal, mat);
		N = normalize(N);

	//ライト計算
	float3 vec = normalize(LightDir);
		Out.Level = dot(-vec, N);

	return Out;
}

/*		トゥーン影シェーダー		*/
float4 PS_Toon(VS_OUTPUT_TOON In) : COLOR
{
	float4 OUT;

	//ピクセル色決定
	OUT = tex2D(DecaleSamp, In.Tex);

	//レベル未満を影にする
	if (In.Level < ToonLevel) OUT.rgb *= ToonShadow;

	return OUT;
}


technique toon
{
	//輪郭線
	pass Outline
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CW;
		ZEnable = true;

		VertexShader = compile vs_2_0 VS_Outline();
		PixelShader = compile ps_2_0 PS_Outline();

	}

	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Basic();
		PixelShader = compile ps_3_0 PS_Basic();
	}
}

//**************************************************************************************************
//
//		CookTorrance
//
//**************************************************************************************************

// Beckman分布
float BechmannDistribution(float d, float m) {
	float d2 = d * d;
	float m2 = m * m;
	return exp((d2 - 1.0) / (d2 * m2)) / (m2 * d2 * d2);
}

// フレネル項
float Fresnel(float c, float f0) {
	float sf = sqrt(f0);
	float n = (1.0 + sf) / (1.0 - sf);
	float g = sqrt(n * n + c * c - 1.0);
	float ga = (c * (g + c) - 1.0) * (c * (g + c) - 1.0);
	float gb = (c * (g - c) + 1.0) * (c * (g - c) + 1.0);
	return (g - c) * (g - c) / (2.0 * (g + c) + (g + c)) * (1.0 + ga / gb);
}

//------------------------------------------------------
//	ピクセルシェーダ
//------------------------------------------------------
float4 PS_CookTorrance(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

	//　目線
	float3 E = -normalize(In.vE);//　目線は反対にさせる
	//	法線取得
	float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
	//	ライト計算
	In.vLight = normalize(In.vLight);

	float3 L = -In.vLight;

	float3 H = normalize(L + E);

	float NV = dot(N, E);
	float NH = dot(N, H);
	float VH = dot(E, H);
	float NL = dot(N, L);
	float LH = dot(L, H);

	// Beckmann 分布関数
	const float m = 0.35f;//絞る数値
	float NH2 = NH*NH;
	float m2 = m * m;
	float D = exp(-(1.0f - NH2) / (NH2*m2)) / (4*m2*NH2*NH2);//４*は絞る数値

	// 幾何減衰率
	float G = min(1, min(2 * NH*NV / VH, 2 * NH*NL / VH));

	// フレネル
	float n = 20.0f;
	float g = sqrt(n*n + LH*LH - 1);
	float gpc = g + LH;
	float gnc = g - LH;
	float cgpc = LH*gpc - 1;
	float cgnc = LH*gnc + 1;
	float F = 0.5f*gnc*gnc*(1 + cgpc*cgpc / (cgnc*cgnc)) / (gpc*gpc);

	// 金属の色
	float4 ks = { 2.0f*0.486f, 2.0f*0.433f, 2.0f*0.185f, 1.0f };

	//ライティング
	float3 light = DirLight(In.vLight, N);
	In.Color.rgb = light * float3(0.7f, 0.5f, 0.1f);
	In.Color.rgb += float3(0.5f, 0.3f, 0.0f);

	//	ピクセル色決定
	//OUT = In.Color * tex2D(DecaleSamp, Tex);
	
	return In.Color + ks * max(0, F*D*G / NV);//ここで終り

}

technique CookTorrance
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Full();
		PixelShader = compile ps_3_0 PS_CookTorrance();
	}

}

//**************************************************************************************************
//
//		海
//
//**************************************************************************************************

//------------------------------------------------------
//		頂点フォーマット
//------------------------------------------------------
struct VS_OUTPUT_SEA
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex1		: TEXCOORD0;
	float2 Tex2		: TEXCOORD1;

	//行列
	float4 wPos		: TEXCOORD2;//	ピクセルに送る情報にワールドからのポジション追加
	float4 RocalPos	: TEXCOORD3;//	ピクセルに送る情報にゲーム画面ラストのポジション追加

	//ライティング
	//float4 Light	: COLOR1;
	float4 Ambient	: COLOR2;
	float3 Normal : COLOR3;
	float3 vLight	: TEXCOORD4;
	float3 vE		: TEXCOORD5;

	//追加　座標系
	float4 ProjectionPos : TEXCOORD6;
};


struct VS_INPUT_SEA
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
};

////------------------------------------------------------
////		フォグ関連
////------------------------------------------------------
//float	AlphaNear = 10.0f;
//float	AlphaFar = 200.0f;
//float	SeaAlpha = 0.0f;//

float uvSea=0.0f;
float3 SeaColor = { 0.2f, 0.8f, 1.7f };


VS_OUTPUT_SEA VS_Sea(VS_INPUT_SEA In)
{
	VS_OUTPUT_SEA Out = (VS_OUTPUT_SEA)0;

	//ローカル座標系上での頂点の座標
	Out.RocalPos = In.Pos;

	Out.Pos = mul(In.Pos, Projection);//プロジェクション行列と合成
	Out.ProjectionPos = Out.Pos;

	Out.Color = 1.0f;//In.Color;
	Out.Tex1 = In.Tex + float2(-uvSea*1.5, -uvSea * 1.5);
	Out.Tex2 = In.Tex + float2(uvSea, uvSea*1.5);


	float4 P = mul(In.Pos, TransMatrix);//ワールド座標に持っていく
	Out.wPos = mul(In.Pos, TransMatrix);//
	float3x3	mat = TransMatrix;//float4x4のTransMatrixの行列を　float3x3のmatに
	//法線ベクトル
	Out.Normal = mul(In.Normal, mat);//
	Out.Normal = normalize(Out.Normal);//

	// 半球ライティング
	Out.Ambient.rgb = HemiLight(Out.Normal);

	// 頂点ローカル座標系算出
	float3	vx;
	float3	vy = { 0, 1, 0.001f };
	vx = cross(vy, Out.Normal);
	vx = normalize(vx);
	vy = cross(vx, Out.Normal);
	vy = normalize(vy);

	//	ライトベクトル補正 ライトが当たった時の角度
	Out.vLight.x = dot(vx, LightDir);			// ライトベクトルを頂点座標系に変換する
	Out.vLight.y = dot(vy, LightDir);			// ライトベクトルを頂点座標系に変換する
	Out.vLight.z = dot(Out.Normal, LightDir);	// ライトベクトルを頂点座標系に変換する
	Out.vLight = normalize(Out.vLight);

	// 視線ベクトル補正
	float3 E = P - ViewPos;			 // 視線ベクトル
	Out.vE.x = dot(vx, E);			 // 視線ベクトルを頂点座標系に変換する
	Out.vE.y = dot(vy, E);			 // 視線ベクトルを頂点座標系に変換する
	Out.vE.z = dot(Out.Normal, E);   // 視線ベクトルを頂点座標系に変換する
	Out.vE = normalize(Out.vE);//目線

	////	フォグ計算
	//Out.Ambient.a = (AlphaFar - Out.Pos.z) / (AlphaFar - AlphaNear);
	//Out.Ambient.a = saturate(Out.Ambient.a);//指定された値を 0 ～ 1 の範囲にクランプします

	return Out;
}

float4 PS_Sea(VS_OUTPUT_SEA In) : COLOR
{
	float4	OUT;
	//UV２つ作成
	float2 UV1 = In.Tex1;
	float2 UV2 = In.Tex2;

	//**********************************************************
	// 視差マッピング
	//**********************************************************  
	// 高さマップを参照し、高さを取得する
	float h = tex2D(HeightSamp, UV1).r - 0.5f;
	h += tex2D(HeightSamp, UV2).r - 0.5f;
	h *= 0.5;//　二つサンプリングしてるから平均を取ってくる

	// テクセルを頂点座標系での視線ベクトル方向に重みをつけてずらす。
	float3 E = normalize(In.vE);//目線
		UV2 -= 0.02f * h * E.xy + uvSea;
	UV1 -= 0.02f * h * E.xy + uvSea;//あとで数値上げる

	/*************************/
	//	視差適用後に 法線取得
	/*************************/
	//法線マップを参照し、法線を取得する
	float3 NMap = (tex2D(NormalSamp, UV1).rgb - 0.5f)*2.0f;
	NMap += (tex2D(NormalSamp, UV2).rgb - 0.5f)*2.0f;
	NMap *= 0.5f;

	////	ライト計算はしない！　今はじかうち
	In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, NMap);
	//light *= 0.5f;

	//In.Color.rgb = light;//Ambient=環境光＋
	////	ピクセル色決定
	//OUT *= In.Color;

	OUT = tex2D(DecaleSamp, UV1) + tex2D(DecaleSamp, UV2);
	OUT *= 0.5;//　二つサンプリングしてるから平均を取ってくる
	OUT.rgb *= SeaColor + light;// 海の色指定
	OUT.a = .8f;

	/*************************/
	//	スペキュラマップ取得
	/*************************/
	float4 sp_tex = tex2D(SpecularSamp, UV1);
	sp_tex += tex2D(SpecularSamp, UV2);
	sp_tex *= 0.5;//　二つサンプリングしてるから平均を取ってきてる

	//	視線反射ベクトル
	float3 R = reflect(-E, NMap);
	//	スペキュラ
	float3 S;
	S = pow(max(0, dot(R, In.vLight)), 80) * (sp_tex);
	
	//************************************************************
	//　屈折
	//************************************************************
	//ゆがみ量の計算
	//水の厚みが薄くなるほど屈折しないようにする
	//float4 Offset = float4((Nmap - E).xy * 20, 0.0f, 0.0f);
	//　射影テクスチャ用
	//float3 wvpNormal = normalize(mul(Nmap, (float3x3)Projection));
	//float2 TexCoord;
	//TexCoord = (In.Pos.xy + wvpNormal.xy * 1) / In.Pos.z;
	//TexCoord = TexCoord * float2(0.5f, -0.5f) + 0.5f;

	//海底の色
	//float3 SeabedCol;
	//SeabedCol = tex2Dproj(TEXSamp, TexCoord );
	//OUT.rgb *= SeabedCol;

	//	環境マップ　Rを入れ反射マップに
	float3 env = Environment(R) * (0.8f - sp_tex.a);
		OUT.rgb = OUT.rgb * env + OUT.rgb;
	//*********************************************************************************************************
	//フレネル効果
	//*********************************************************************************************************
	//頂点 → 視点ベクトル と 法線ベクトルの内積を計算
	//float3 pos = In.Pos;
	//float fresnel = dot(normalize(-E - In.RocalPos), normalize(In.Normal));

	//////内積の結果が 0.0f に近いほど反射マップの色が強くなり、 1.0f に近いほど海底マップの色が強くなるように線形合成する
	//float4 Col;
	//float3 ReflectCol = float3(0.8, 0.5, 0.8);
	//float3 SeabedCol = float3(0.0, 0.9, 0.2);
	//
	//Col.rgb = lerp(ReflectCol, SeabedCol, fresnel);
	////Col.rgb = Col.rgb * Diffuse * WaterCol + S;
	//OUT.rgb = Col.rgb;

	//*********************************************************************************************************
	//透明度
	//*********************************************************************************************************  
	//水の厚みを計算する
	//float Z = SeabedZ - In.PosWVP.z / m_ZF;
	//Ｚ値が小さくなるほど透明度を高くする（注意１）
	//Z = min(Z * 20.0f, 1.0f);
	//Col.rgb = lerp(SeabedCol, Col.rgb, Z);

	OUT.rgb += S;
	
	//　射影テクスチャ用
	//float3 wvpNormal = normalize(mul(In.Normal, (float3x3)Projection));
	//	ref = (In.wPos.xy + wvpNormal.xy * 1) / In.wPos.z;
	//output.TexCoord = output.TexCoord * float2(0.5f, -0.5f) + 0.5f;

	//リフレクトマップ
	float2 ref = (In.ProjectionPos.xy / In.ProjectionPos.w)*0.5f + 0.5f;
		ref.y = -ref.y;
	float3 Env = tex2D(RefSamp, ref-0.2f * h * E.xy/*←+目線*/) * (OUT.rgb);
		Env *= 0.35f;
		OUT.rgb += Env;// lerp(Env, OUT.rgb, 0.3f);
	
	//	フォグ採用
	//OUT.a = ((1 - In.Ambient.a));


	return OUT;
}

technique Sea
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;

		VertexShader = compile vs_3_0 VS_Sea();
		PixelShader = compile ps_3_0 PS_Sea();
	}
}

