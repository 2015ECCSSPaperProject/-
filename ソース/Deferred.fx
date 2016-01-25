//**************************************************************************************************
///																									
///		基本シェーダー		
///
//**************************************************************************************************

//------------------------------------------------------
///		環境関連
//------------------------------------------------------
float4x4 TransMatrix;	//	ワールド変換行列
float4x4 matView;		//	ビュー変換
float4x4 Projection;	//	投影変換行列 WVP 

float3	ViewPos;		//  ワールド座標の目のポジション
float4x4 InvProj;		//  Projection->View　へ戻す逆行列
float4x4 InvViewProj;	//　プロジェクション->ワールド変換行列
float4x4 matProjection;	//  ビュー->プロジェクション変換行列

//------------------------------------------------------
///		テクスチャサンプラー	
//------------------------------------------------------

// MinFilter	縮小フィルタに使用されるメソッド
// MagFilter	拡大フィルタに使用されるメソッド
// MipFilter	MIPMAPに使用されるメソッド
// AddressU	0 - 1の範囲外にあるuテクスチャー座標の解決に使用されるメソッド
// AddressV	0 - 1の範囲外にあるvテクスチャー座標の解決に使用されるメソッド
// LINEAR		バイリニア補間フィルタリング。目的のピクセルに外接する2×2テクセルの重み付き平均領域を使用
// NONE		MIPMAPを無効にし、拡大フィルタを使用
// CLAMP		範囲[0.0, 1.0]の外にあるテクスチャー座標には、それぞれ0.0または1.0のテクスチャーカラーを設定
// MIPMAP		メインとなるテクスチャの画像を補完するよう事前計算され最適化された画像群。詳細さのレベルによって画像を切り替える

// WRAP...反対側から同じ絵を繰り返します
// CLAMP...端の色を繰り返します
// BORDER...透明色にします

texture Texture;	//	デカールテクスチャ
sampler DecaleSamp = sampler_state
{
    Texture = <Texture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;


	AddressU = CLAMP;
	AddressV = CLAMP;
};

// UV用
sampler DecaleSampUV = sampler_state
{
    Texture = <Texture>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

	//BorderColor = 0x00FFFFFF;
	AddressU = WRAP;
	AddressV = WRAP;
};


texture NormalMap;	//	法線マップテクスチャ
sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

	AddressU = WRAP;
	AddressV = WRAP;
};

texture SpecularMap;	//	スペキュラマップテクスチャ
sampler SpecularSamp = sampler_state
{
    Texture = <SpecularMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

	AddressU = WRAP;
	AddressV = WRAP;
};

texture HeightMap;		//	高さマップテクスチャ
sampler HeightSamp = sampler_state
{
    Texture = <HeightMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

	AddressU = WRAP;
	AddressV = WRAP;
};

//**************************************************************************************************
//		頂点フォーマット
//**************************************************************************************************

struct VS_INPUTL
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float4 Color  : COLOR0;
	float2 Tex	  : TEXCOORD0;
};

struct VS_DEFERRED
{
	float4 Pos				:POSITION;
	float4 Color			:COLOR0;
	float2 Tex				:TEXCOORD0;
	float3 Normal			:TEXCOORD1;			//法線
	float4 Depth			:TEXCOORD2;			//深度情報
	float3 T				:TEXCOORD3;			//タンジェント(接線)
	float3 B				:TEXCOORD4;			//Binormal(従法線)
	float3 vE				:TEXCOORD5;			//視線ベクトル
};

// 四つの情報をこの順番通りに描く
struct PS_DEFERRED
{
	float4 Diffuse			:COLOR0;	// テクスチャー
	float4 Normal			:COLOR1;	// 法線情報
	float4 Specular			:COLOR2;	// スペキュラ
	float4 Depth			:COLOR3;	// 奥行き
};

//*****************************************
//		G_Buffer
//*****************************************

VS_DEFERRED VS_G_Buffer(VS_INPUTL In)
{
	VS_DEFERRED Out = (VS_DEFERRED)0;

	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color = In.Color;
	Out.Color.a = 1.0f;

	//深度
	Out.Depth = Out.Pos;

	//法線情報の修正
	float3x3	mat = (float3x3)Projection;					//スクリーン座標なのでProjectionを掛け合わせる
	//float3x3 mat = (float3x3)mul(TransMatrix, matView); // こっちでやるとOUT
	float3 N = mul(In.Normal, mat);
	N = normalize(N);
	Out.Normal = N;

	//	頂点スクリーン座標系算出
	float3	vx;
	float3	vy = { .0f, 1.0f, -0.001f };			//仮のy方向ベクトル

	vx = cross(N, vy);
	vx = normalize(vx);
	Out.T = vx;

	vy = cross(N, vx);
	vy = normalize(vy);
	Out.B = vy;


	//視線ベクトル
	float3 P = mul((float3)In.Pos, (float3x3)TransMatrix);
	float3 E = (P - ViewPos);		//視線ベクトル
	//vx = float3(1.0f,.0f,0.001f);		//仮のx向きベクトル

	Out.vE.x = -dot(vx, E);
	Out.vE.y = dot(vy, E);
	Out.vE.z = dot(N, E);
	Out.vE = normalize(Out.vE);

	return Out;
}

PS_DEFERRED PS_G_Buffer(VS_DEFERRED In)
{
	PS_DEFERRED OUT = (PS_DEFERRED)0;

	float2 Tex = In.Tex;

	float3 E = normalize(In.vE);

	//　各色に光のパラメーターを設定
	//  R->AO  G->Emissive  B->未設定
	float3 multiRate = tex2D(HeightSamp, Tex).rgb;

	//ディヒューズ
	//OUT.Diffuse = In.Color * tex2D( DecaleSamp,Tex );
	OUT.Diffuse = tex2D(DecaleSamp, Tex);
	//OUT.Diffuse.a = multiRate.g;
	//OUT.Diffuse.a = 1.0f;

	//スペキュラ
	OUT.Specular = tex2D(SpecularSamp, Tex);
	//OUT.Specular.a = 1.0f;
	OUT.Specular.a = multiRate.g;
	//OUT.Position.a = 1.0f;

	//深度 r32bit.
	OUT.Depth = In.Depth.z / In.Depth.w;			//wで割って-1~1に変換する

	//法線 	頂点空間→カメラ変換行列
	float3x3 ts;								//法線から求めた軸
	ts[0] = normalize(In.T);					//接線
	ts[1] = normalize(In.B);					//従法線
	ts[2] = normalize(In.Normal);				//法線


	float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
	float3 normal = normalize(mul(N, ts));
	normal = normal * 0.5f + 0.5f;
	OUT.Normal.rgb = normal;
	OUT.Normal.a = multiRate.r;//AOマップを法線のアルファに
	
	return OUT;
}
/*ブレンディング オプション*/
/*
BLEND_ZERO			ソースの色は黒 (0, 0, 0, 0) です。ブレンディング前の処理はありません。
BLEND_ONE			ソースの色は白 (1, 1, 1, 1) です。ブレンディング前の処理はありません。
BLEND_SRC_COLOR		ピクセルシェーダーからのカラー データ (RGB) です。ブレンディング前の処理はありません。
BLEND_INV_SRC_COLOR ピクセルシェーダーからのカラー データ (RGB) です。ブレンディング前の処理によってデータが反転され、1 - RGB が生成されます。
BLEND_SRC_ALPHA		ピクセルシェーダーからのアルファ データ (A) です。ブレンディング前の処理はありません。
BLEND_INV_SRC_ALPHA ピクセルシェーダーからのアルファ データ (A) です。ブレンディング前の処理によってデータが反転され、1 - A が生成されます。
BLEND_DEST_ALPHA	レンダーターゲットからのアルファ データです。ブレンディング前の処理はありません。
*/

technique G_Buffer
{
	pass P0
	{
		// レンダリングステート
		AlphaBlendEnable = true;
		BlendOp = Add;
		//SrcBlend = SrcAlpha;		// 最初のRGBデータソースを指定します
		//DestBlend = InvSrcAlpha;	// 2番目のRGBデータソースを指定します
		SrcBlend = One;		//1,1,1,1
		DestBlend = Zero;	//0,0,0,0
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = true;
		//SeparateAlphaBlendEnable = true;// ダイレクト3Dの別々のアルファ・ブレンド・モードをイネーブルします
	
		AlphaRef = 0x00000080;
		AlphaFunc = GREATEREQUAL;	// αがAlphaRef以上ならOK

		VertexShader = compile vs_3_0 VS_G_Buffer();
		PixelShader = compile ps_3_0 PS_G_Buffer();
	}
}

/* deferredでαに情報を書き入れる方法 */
/*
	普通に描くと　半透明オブジェクトが重なり合い情報が正確にでない
	解決方法は
	SrcBlend = One;	
	DestBlend = Zero
	ZEnable = true;
	に設定する

	↑どういうことが起こるか
	ソースブレンドは　今書いているものをどれくらい乗っけるかの値
	これを One(1,1,1,1)にすることで全力で上にかぶせる用に設定

	デプスブレンドは　後に描いていた物体をどう薄めるか
	これを　Zero(0,0,0,0)にすることで一切描かない

	これだと　後ろが絶対見える処理になっていしまうけど
	Ｚ値を考慮すればそもそも後ろを書かなくできる

*/

//*****************************************
//		G_Water
//*****************************************

//-----------------------------------------
//		グローバル変数
//-----------------------------------------
float uvSea = 0.0f;
float3 SeaColor = { 0.2f, 0.8f, 1.5f };

//-----------------------------------------
//		最初に頂点から貰う情報
//-----------------------------------------
struct VS_INPUT_SEA
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
};

//-----------------------------------------
//		pixelに送る情報
//-----------------------------------------
struct VS_OUTPUT_SEA
{
	float4 Pos				:POSITION;
	float2 Tex1				:TEXCOORD0;
	float2 Tex2				:TEXCOORD1;

	float3 Normal			:TEXCOORD2;			//法線
	float4 Depth			:TEXCOORD3;			//深度情報

	float3 T				:TEXCOORD4;			//タンジェント(接線)
	float3 B				:TEXCOORD5;			//Binormal(従法線)
	float3 vE				:TEXCOORD6;			//視線ベクトル
};


VS_OUTPUT_SEA VS_Sea(VS_INPUT_SEA In)
{
	VS_OUTPUT_SEA Out = (VS_OUTPUT_SEA)0;

	Out.Pos = mul(In.Pos, Projection);

	//2つのUVを作る
	Out.Tex1 = In.Tex + float2(-uvSea*1.5, -uvSea * 1.5);
	Out.Tex2 = In.Tex + float2(uvSea, uvSea*1.5);

	//深度
	Out.Depth = Out.Pos;


	//法線情報の修正
	float3x3	mat = (float3x3)Projection;					//スクリーン座標なのでProjectionを掛け合わせる
	float3 N = mul(In.Normal, mat);
	N = normalize(N);
	Out.Normal = N;

	//	頂点スクリーン座標系算出
	float3	vx;
	float3	vy = { .0f, 1.0f, -0.001f };			//仮のy方向ベクトル
	vx = cross(N, vy);
	vx = normalize(vx);
	Out.T = vx;// X取得
	vy = cross(N, vx);
	vy = normalize(vy);
	Out.B = vy;// Y取得

	//視線ベクトル
	float3 P = mul((float3)In.Pos, (float3x3)TransMatrix);
	float3 E = (P - ViewPos);		//視線ベクトル
	Out.vE.x = -dot(vx, E);
	Out.vE.y = dot(vy, E);
	Out.vE.z = dot(N, E);
	Out.vE = normalize(Out.vE);

	return Out;
}


PS_DEFERRED PS_Sea(VS_OUTPUT_SEA In) 
{
	PS_DEFERRED OUT = (PS_DEFERRED)0;

	//UV２つ作成
	float2 UV1 = In.Tex1;
	float2 UV2 = In.Tex2;

	/* 視差マッピング */
	// 高さマップを参照し、高さを取得する
	float h = tex2D(HeightSamp, UV1).r - 0.5f;
	h += tex2D(HeightSamp, UV2).r - 0.5f;
	h *= 0.5;//　二つサンプリングしてるから平均を取ってくる

	// テクセルを頂点座標系での視線ベクトル方向に重みをつけてずらす。
	float3 E = normalize(In.vE);//目線
	UV2 -= 0.02f * h * E.xy + uvSea;
	UV1 -= 0.02f * h * E.xy + uvSea;//あとで数値上げる

	/* 視差適用後に 法線取得 */
	//２つのUVから法線マップを参照し、法線を取得する
	float3 NMap = (tex2D(NormalSamp, UV1).rgb - 0.5f)*2.0f;
	NMap += (tex2D(NormalSamp, UV2).rgb - 0.5f)*2.0f;
	NMap *= 0.5f;

	float3 Normal = NMap.r*In.T + NMap.g*In.B + NMap.b*In.Normal;
	OUT.Normal.rgb = Normal*0.5f + 0.5f;
	OUT.Normal.a = 1.0f;// ハイトマップとしてを使っているのでAOは仮に1.0f

	/*■デカールにまずは色を入れる*/
	OUT.Diffuse = tex2D(DecaleSamp, UV1) + tex2D(DecaleSamp, UV2);
	OUT.Diffuse *= 0.5;//　二つサンプリングしてるから平均を取ってくる
	OUT.Diffuse.rgb *= SeaColor;// 海の色指定
	OUT.Diffuse.a = 1.0f;


	//■スペキュラ
	float4 sp_tex = tex2D(SpecularSamp, UV1);
	sp_tex += tex2D(SpecularSamp, UV2);
	sp_tex *= 0.5;//　二つサンプリングしてるから平均を取ってきてる
	OUT.Specular = sp_tex;


	//■深度
	OUT.Depth = In.Depth.z / In.Depth.w;			//wで割って-1~1に変換する
	//0以下はPSで弾かれるため0~1の処理は行わない
	OUT.Depth.a = 1.0f;

	return OUT;
}

technique G_Sea
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;
		SeparateAlphaBlendEnable = true;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_Sea();
		PixelShader = compile ps_3_0 PS_Sea();
	}
}

//************************************************************************************************
//
///		ディファードでのライト処理
//
//************************************************************************************************

//------------------------------------------------------
///		環境用スクリーン	フォアード空間用
//------------------------------------------------------
texture EnvFullBuf;	//	EnvFullBuf
sampler EnvFullBufSamp = sampler_state
{
	Texture = <EnvFullBuf>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	// 横だけミラーにしてごまかす
	AddressU = MIRROR;//MIRROR
	AddressV = WRAP;//MIRROR
};

//------------------------------------------------------
///		スクリーンスペース深度
//------------------------------------------------------
texture DepthBuf;	//	カメラ空間深度
sampler DepthBufSamp = sampler_state
{
	Texture = <DepthBuf>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler DepthBufSampWater = sampler_state
{
	Texture = <DepthBuf>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};


//------------------------------------------------------
///		スペキュラバッファ
//------------------------------------------------------
texture SpecularBuf;	//	スペキュラ
sampler SpecularBufSamp = sampler_state
{
	Texture = <SpecularBuf>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

//------------------------------------------------------
///		スクリーンスペース法線
//------------------------------------------------------
texture NormalBuf;	//	法線
sampler NormalBufSamp = sampler_state
{
	Texture = <NormalBuf>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};



// ライトの処理では色の情報とスペキュラの情報を吐き出す
struct DEF_LIGHT
{
	float4	color	:	COLOR0;
	float4	spec	:	COLOR1;
};

float3 LightColor = { 1.0f, 1.0f, 1.0f };
float sp_rate = 2.0f;

///------------------------------------------------------
//		プロジェクションからビュー行列へ 
///------------------------------------------------------
// 逆行列と奥行の情報は外部からとってきてます
inline float4 ProjectionToView(float2 NormalTex)//←法線マップを使います
{
	float4 pos;
	pos.xy = NormalTex*2.0f - 1.0f;
	pos.y *= -1.0f;
	pos.z = tex2D(DepthBufSamp, NormalTex).r;//深度マップをサンプリングして奥行を手に入れる
	pos.w = 1.0f;

	// 逆行列でビュー座標系に変換
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	return pos;
}


//**************************************************************************************************
///	環境マップ用
//**************************************************************************************************
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
inline float4 Environment(float3 reflect)
{
	float4	color;

	float2	uv;
	uv.x = reflect.x*0.5f + 0.5f;
	uv.y = -reflect.y*0.5f + 0.5f;

	color = tex2D(EnvSamp, uv);
	return color;
}


//***************************************
///		ディファード平行光		
//***************************************

float3 ViewLightVec = { 1.0f, -1.0f, 1.0f };// cpu側でビュー座標系に変換する
float RimPowerRate = 5.0f;		 // リムライトの光の幅 
float EmissivePowerRate = 512.0f;// エミッシブの幅 エミッシブがいらない時はテクス茶を変えずこの数値を下げる

DEF_LIGHT PS_DirLight(float2 Tex:TEXCOORD0)
{
	DEF_LIGHT OUT;



	// 法線マップから法線の向きの取得
	float4 n = tex2D(DecaleSamp, Tex); 
	//if (n.a <= 0.1f)
	//{
	//	discard;
	//}
	// 法線取得
	float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
	normal = normalize(normal);			// 法線正規化
	
	// AOマップ取得
	float AO = n.a;

	// ライト率(ランバート)
	float rate = dot(normal, -ViewLightVec);
	rate = max(0, rate);// レートを-にさせない
	//if (rate <= 0.0f)discard;

	// ピクセルの色
	OUT.color.rgb = (rate * LightColor);
	OUT.color.a = 1;
	OUT.color.rgb *= AO;

	//　スペキュラ
	float4 pos = ProjectionToView(Tex);//　ビュー行列へ
								 //↑法線マップをぶち込んでください
	// ビュー行列での目線
	float3 E = pos.xyz;
		E = normalize(E);
	float3 R = normalize(-ViewLightVec - E);
	float sp = pow(max(0, dot(R, normal)), 80) * sp_rate;// いつも通り絞って


	//　各色に光のパラメーターを設定
	//  R->Spec  G->Bloom  B->Rim  A->Emissive
	float4 multiRate = tex2D(SpecularBufSamp, Tex).rgba;

		// (R) スペキュラの強さ
		OUT.spec.rgb = LightColor * multiRate.r * sp;// *(1.0f - multiRate.b) + env.rgb * multiRate.b;
		OUT.spec.a = 1.0f;

		// (B) リムライト
		//ライトがカメラ正面近くにあるほど強くなるようにする。
		float RimPower = max(.0f, dot(-E, ViewLightVec))*RimPowerRate;
		
		//輪郭付近の強度を強くする　乗数で絞る
		float OutlinePower = pow(1.0f - dot(normal, -E), 3.0f);

		// リムカラー
		float3 RimColor = (LightColor * (OutlinePower*RimPower))*multiRate.b;// 青の要素で強さを変える
		OUT.spec.rgb += RimColor;

		// エミッシブ
		OUT.color.rgb += multiRate.a * EmissivePowerRate;
		

	return OUT;

}

technique def_DirLight
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_DirLight();
	}
}



//**********************************
//
///		Deferred_PointLight
//
//**********************************

//------------------------------------------------------
///		InfoPointLight
//------------------------------------------------------
texture InfoPLMap;	//	ポイントライトの情報
sampler InfoPLSamp = sampler_state
{
	Texture = <InfoPLMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

int InfoPL_X = 0;
int InfoPL_Y = 0;


//#define PL_NUM 50
// PointLightの数
int PL_NUM = 0;
 
// 最初は絶対定数で作っとかないといけないので最大数用意
//#define PL_MAX 20
//float3 PLpos[PL_MAX];
//float  PLrange[PL_MAX];
//float3 PLcolor[PL_MAX];
//float  PLpower[PL_MAX];

struct DEF_POINTLIGHT
{
	float4	color	:	COLOR0;
};
//float3 LightPos = { .0f, .0f, .0f };// cpu側でビュー座標にした光源位置
//float LightRange = .0f;				// 光源の広がり具合
//float LightPower = 1.0f;

DEF_LIGHT PS_PointLight(float2 Tex : TEXCOORD0)
{
	DEF_LIGHT OUT = (DEF_LIGHT)0;

	//スクリーン座標系位置の取得
	float4 pos = ProjectionToView(Tex);

	// for文でライトを回そう！(TODO)INDEXの限界がきたので何とかする
	for (int i = 0; i < PL_NUM; i++)
	{
		//(仮)
		// PLマップからPos&Range取得
		float4 PLInfo = tex2D(InfoPLSamp, float2((float)i / (float)32.0f, (float)0.0f / (float)2.0f));

		// ポジション
		float3 PLpos = PLInfo.rgb;
		// レンジ
		float  PLrange = PLInfo.a;

		float3 ViewLightVec = PLpos.xyz - (pos.xyz);// ライト←ポジション＆距離
			float dist = pow(max(0.0f, 1.0f - (length(ViewLightVec) / PLrange)), 2);//←数値をいじり絞る
	
		// 距離を入手(レンジが大きければ大きいほど広がりがます)
		// 1までを描画するように設定するから　そして1-をやることで反転
		//if (dist < 0.0f)discard;//　距離がマイナスになっても描画するのをふせぐため 
		//clip(dist);// 0以下ならピクセル破棄・・・しても処理速度あんまり変わらんかった

		ViewLightVec = normalize(ViewLightVec);// 光<-場所ベクトルへ

		/************************************************/
		// 法線マップから法線の向きの取得
		//float4 n = (tex2D(DecaleSamp, Tex) - 0.5f)*2.0f;
		//	n = normalize(n);
		//float rate = max(0, dot(n.xyz, LightVec.xyz));

		// PLマップからCol&Power取得
		 PLInfo = tex2D(InfoPLSamp, float2((float)i / (float)32.0f, (float)1.0f / (float)2.0f));

		// カラー
		float3 PLcolor = PLInfo.rgb;
		// パワー
		float  PLpower = PLInfo.a;

		//PLcolor.g = 100;
		//PLpower += 5;

		OUT.color.rgb += ((dist/**rate*/)*PLcolor.xyz)*PLpower;

	}

	OUT.color.a = 1;


	return OUT;

}

technique def_PointLight
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;//　追加
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_PointLight();
	}
}

//**********************************
///		シンプル点光源
//**********************************

//DEF_LIGHT PS_SimpliPointLight(float2 Tex: TEXCOORD0) 
//{
//	DEF_LIGHT OUT = (DEF_LIGHT)0;
//
//	//スクリーン座標系位置の取得
//	float4 pos = ProjectionToView(Tex);
//
//		// (光源との距離)と(光の向き)を取得
//		float3 lightDir = ((float3)pos - LightPos) / LightRange;
//
//		// 距離計算
//		float distance = max(0, 1.0f - (length(lightDir)));//←数値を変え絞る
//
//	//光源の距離が＿の場合は破棄する.
//	if (length(lightDir) > 1.0f) discard;
//
//		// 減衰量込みのポイントライトを乗せる
//		OUT.color.rgb = LightColor * min(1.0f, (distance*4.0f));
//		//OUT.color.r = 100;
//		OUT.color.a = 1.0f;
//		
//		//スペキュラ
//		OUT.spec.rgb = 0.0f;
//		OUT.spec.a = 1.0f;
//
//	
//	
//	return OUT;
//
//}
//
//technique def_SimpliPointLight
//{
//	pass P0
//	{
//		AlphaBlendEnable = true;
//		BlendOp = Add;
//		SrcBlend = SrcAlpha;
//		DestBlend = One;
//		ZEnable = False;
//
//		PixelShader = compile ps_3_0 PS_SimpliPointLight();
//	}
//}

//*************************************
//
///		半球ライティング
//
//*************************************
float3 SkyColor = { .4f, .6f, .6f };
float3 GroundColor = { .6f, .4f, .4f };

float4 PS_HemiLight(float2 Tex: TEXCOORD0) :COLOR
{
	float4 OUT;

	//法線マップから法線の向きの取得
	float4 n = tex2D(DecaleSamp, Tex);
		float3 normal = n.rgb * 2.0f - 1.0f;
		normal = normalize(normal);

	float AO = n.a;// NormalのαからAOの値を取得

	//半球処理
	float rate = normal.y * 0.5f + 0.5f;
	OUT.rgb = SkyColor * rate;
	OUT.rgb += GroundColor * (1.0f - rate);
	OUT.rgb *= AO;
	OUT.a = 1;

	return OUT;

}
technique def_HemiLight
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_HemiLight();
	}
}


//*************************************
//
///		エミッシブ
//
//*************************************
//float EmissivePowerRate = 512.0f;
float4 PS_Emissive(float2 Tex: TEXCOORD0) :COLOR
{
	float4 OUT;

	// デカールマップからエミッシブ情報を取得
	float emissive  = tex2D(DecaleSamp, Tex).a;

	// 直球
	OUT.rgb = emissive * EmissivePowerRate;
	OUT.a = 1;

	return OUT;

}
technique def_Emissive
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_Emissive();
	}
}

//*************************************
//
///		フォグ
//
//*************************************
float FogNear = 514.0f;
float FogFar = 1024.0f;
float3 FogColor = { 100.0f, 100.0f, 100.0f };

// 深度テクスチャを使用して使う
float4 PS_Fog(float2 Tex: TEXCOORD0) :COLOR
{
	float4 OUT;

	//スクリーン座標系位置の取得
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1から1に戻す
	pos.y *= -1;
	pos.z = tex2D(DecaleSamp, Tex).r;	// 深度バッファから奥行取得
	pos.w = 1.0f;

	//逆行列でビュー座標系に変換する
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;
	
	//float z = tex2D(DecaleSamp, Tex).r;

	//float fogRate = (FogFar - pos.z) / (FogFar - FogNear);
	//fogRate = saturate(fogRate);//指定された値を 0 ～ 1 の範囲にクランプします
	float fogRate = smoothstep(FogFar,FogNear , pos.z);

	// フォグの値を入れる
	OUT.rgb = FogColor * (1.0f - fogRate);
	OUT.a = 1;

	return OUT;

}
technique def_Fog
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_Fog();
	}
}


//*************************************
//
///	全部描画
//
//*************************************


DEF_LIGHT PS_AllLight(float2 Tex:TEXCOORD0)
{
	DEF_LIGHT OUT;

	// 法線マップから法線の向きの取得
	float4 n = tex2D(DecaleSamp, Tex);
		// 法線取得
		float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
		normal = normalize(normal);			// 法線正規化

	// AOマップ取得
	float AO = n.a;

	// ライト率(ランバート)
	float rate = dot(normal, -ViewLightVec);
	rate = max(0, rate);// レートを-にさせない
	//if (rate <= 0.0f)discard;

	// ピクセルの色
	OUT.color.rgb = (rate * LightColor);
	OUT.color.a = 1;
	OUT.color.rgb *= AO;

	//　スペキュラ
	float4 pos = ProjectionToView(Tex);//　ビュー行列へ
		//↑法線マップをぶち込んでください
		// ビュー行列での目線
		float3 E = pos.xyz;
		E = normalize(E);
	float3 R = normalize(-ViewLightVec - E);
		float sp = pow(max(0, dot(R, normal)), 80) * sp_rate;// いつも通り絞って


	//　各色に光のパラメーターを設定
	//  R->Spec  G->Bloom  B->Rim A->Emi
	float4 multiRate = tex2D(SpecularBufSamp, Tex).rgba;

		// (R) スペキュラの強さ
		OUT.spec.rgb = LightColor * multiRate.r * sp;// *(1.0f - multiRate.b) + env.rgb * multiRate.b;
	OUT.spec.a = 1.0f;

	// (B) リムライト
	//ライトがカメラ正面近くにあるほど強くなるようにする。
	float RimPower = max(.0f, dot(-E, ViewLightVec));
	//輪郭付近の強度を強くする
	float OutlinePower = pow(1.0f - dot(normal, -E), 5.0f);

	//リムカラー
	float3 RimColor = (LightColor * (OutlinePower*RimPower))*multiRate.b;// 青の要素で強さを変える
		OUT.spec.rgb += RimColor;


	/*************************/
	//		半球ライト
	/*************************/
	//半球処理
	rate = normal.y * 0.5f + 0.5f;
	OUT.color.rgb += SkyColor * rate;
	OUT.color.rgb += GroundColor * (1.0f - rate);
	OUT.color.rgb *= AO;


	/*************************/
	//		エミッシブ
	/*************************/
	// 直球
	OUT.color.rgb += multiRate.a * EmissivePowerRate;

	return OUT;

}

technique def_AllLight
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_AllLight();
	}
}


//****************************************************
//	
//		被写界深度
//
//****************************************************
float3	DOF_target;		//カメラのターゲット(ビュー座標系)
float	DOF_Range;		// 

sampler DecaleSamp2 = sampler_state
{
	Texture = <Texture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

float4 PS_DOF(float2 Tex: TEXCOORD0) :COLOR
{
	float4 OUT;
	OUT.rgb = tex2D(DecaleSamp2, Tex).rgb;

	//スクリーン座標系位置の取得
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1から1に戻す
	pos.y *= -1;
	pos.z = tex2D(DepthBufSamp, Tex).r;
	pos.w = 1.0f;

	//逆行列でビュー座標系に変換する
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	float3 Length = (float3)pos - DOF_target;
		float l = length(Length);

	//rangeより離れた位置からαを増やしていく
	//float alpha = saturate(l / range - 1.0f);
	float alpha = l / DOF_Range;
	OUT.a = alpha;

	return OUT;

}

technique def_DOF
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_DOF();
	}
}

//*************************************************
//
//				ShadowMaps
//
//*************************************************
float4x4 ShadowProjection;
float4x4 ShadowProjectionL;


float AdjustValue = -0.005f;	//　何処までを影と判断するのか　下げると綺麗に影が付くがジャギーが目立つ　上げるとその逆
float Shadow = .1f;				//　影の濃さ
float DistAlpha = .2f;			//　距離に応じて影のαを下げる
float ShadowRange = 0.0f;

// 追加
float ShadowFar = 300.0f;	// 近距離の影を滑らかに入れ替える為
float ShadowNear = 200.0f;	// 近距離の影を滑らかに入れ替える為



//*************************************************
//
//			サンプラー
//
//*************************************************

// 影用深度マップ
texture DepthTex;
sampler DepthSamp = sampler_state
{
	Texture = <DepthTex>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

// 近景シャドウマップ
texture ShadowMap;
sampler ShadowSamp = sampler_state
{
	Texture = <ShadowMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	BorderColor = 0xFFFFFFFF;
	AddressU = BORDER;
	AddressV = BORDER;
};

//遠景シャドウマップ
texture ShadowMapL;
sampler ShadowSampL = sampler_state
{
	Texture = <ShadowMapL>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	BorderColor = 0xFFFFFFFF;
	AddressU = BORDER;
	AddressV = BORDER;
};

//***************************************
//			シャドウマップ作成
//***************************************

struct VS_SHADOW
{
	float4 Pos			:POSITION;
	float4 Color		:TEXCOORD0;		//深度値
	float2 Tex			:TEXCOORD1;
};

// 近距離用シャドウマップ作成
VS_SHADOW VS_ShadowBuf(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_SHADOW Out;

	// 座標変換
	float4x4	mat = mul(TransMatrix, ShadowProjection);
		Out.Pos = mul(Pos, mat);

	Out.Color.r = Out.Pos.z;
	Out.Color.g = Out.Pos.z * Out.Pos.z;
	Out.Color.b = .0f;
	Out.Color.a = 1.0f;

	//平行投影しているのでW成分で割る必要ない　(いぼんこぺったんこモード)
	//Out.Color = Out.Pos.z / Out.Pos.w;

	Out.Tex = Tex;

	return Out;
}

// 遠距離用シャドウマップ作成
VS_SHADOW VS_ShadowBufL(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_SHADOW Out;

	// 座標変換
	float4x4	mat = mul(TransMatrix, ShadowProjectionL);
		Out.Pos = mul(Pos, mat);

	Out.Color.r = Out.Pos.z;
	Out.Color.g = Out.Pos.z * Out.Pos.z;
	Out.Color.b = .0f;
	Out.Color.a = 1.0f;

	Out.Tex = Tex;

	return Out;
}

// PixelShaderは共通で
float4 PS_ShadowBuf(VS_SHADOW In) : COLOR
{
	float4 OUT;
	OUT.rgb = In.Color.rgb;
	OUT.a = tex2D(DecaleSamp, In.Tex).a;
	return OUT;
}

technique ShadowBuf
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		AlphaRef = 0x00000080;
		AlphaFunc = GREATEREQUAL;// αがAlphaRef以上ならOK

		ZWriteEnable = true;
		CullMode = none;		 // カリングをしないことで綺麗に影を描画　

		VertexShader = compile vs_2_0 VS_ShadowBuf();
		PixelShader = compile ps_2_0 PS_ShadowBuf();
	}
}

technique ShadowBufL
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = one;
		DestBlend = zero;
		ZWriteEnable = true;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_ShadowBufL();
		PixelShader = compile ps_3_0 PS_ShadowBuf();
	}
}

//*********************************************
///				ShadowMethod
//*********************************************

// MRT
struct PS_SHADOW
{
	float4 Shadow		:COLOR0;
	float4 Spec			:COLOR1;// スペキュラも触る
};

//******************************
///		テクスチャ座標生成関数
//******************************

//通常版
inline float GetShadow(float3 Tex, sampler samp)
{
	float d = tex2D(samp, Tex.xy).r;		//シャドウマップから深度を取り出す
	if (Tex.z > 1.0f) Tex.z = 1.0f;
	float l = (d < Tex.z + AdjustValue) ? Shadow : 1;
	return l;
}

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
	float l = (d < Tex.z + AdjustValue) ? Shadow : 1;	//←判定
	return l;
}

//シグモイド
inline float sigmoid(float gain, float x, float adjust)
{
	//
	return 1.0 / (1.0 + exp(-gain * (x - adjust)));
}

//バリアンス (ソフトシャドウ)
inline float GetVarianceShadow(float3 Tex, sampler shadowSamp)
{
	half d = Tex.z;

	//	遠景値
	float2	t = tex2D(shadowSamp, Tex.xy).rg;		// シャドウマップから深度を取り出す
		float d1 = t.r;							// 深度値
	float variance = max(t.g - d1 * d1, .0f);	// 分散

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// チェビシェフの不等式

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * .9f;// ちょい薄く

	//	濃度補正	
	l = 1.0f - (l * (1.3f - Shadow));

	return l;
}

// 透明度
inline float GetAlphaVarianceShadow(float3 Tex, sampler shadowSamp, float alpha)
{
	half d = Tex.z;

	//	近景値
	float2	t = tex2D(shadowSamp, Tex.xy).rg;		// シャドウマップから深度を取り出す
		float d1 = t.r;							// 深度値
	float variance = max(t.g - d1 * d1, .0f);	// 分散

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// チェビシェフの不等式

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;


	//	濃度補正	
	l = 1.0f - (l * (1.0f - Shadow)) * alpha;

	return l;
}

//カスケード + バリアンス版
inline float	GetShadowCascade(float3 Tex, float3 Tex2, float a)
{

	float d = Tex.z;

	//	近景値
	float2	t = tex2D(ShadowSamp, Tex.xy).rg;		// シャドウマップから深度を取り出す
		float	d1 = t.r;							// 深度値
	float	variance = max(t.g - d1*d1, 0.0);	// 分散

	float	 te = d - d1;					// t - E
	float	 l1 = variance / (variance + te * te);

	float l = (1.0f - l1) * a;

	//	遠景値
	float t2 = tex2D(ShadowSampL, Tex2.xy).r;	// シャドウマップから深度を取り出す
	float l2 = (t2 < Tex2.z + AdjustValue) ? 1.0f - a : 0;
	return saturate(1.0f - (l + l2));
}

///------------------------------------------------------
//		プロジェクションからワールド行列へ 
///------------------------------------------------------
// 逆行列と奥行の情報は外部からとってきてます
//inline float4 ProjectionToWorld(float2 Tex, float depth)
//{
//	//スクリーン座標の生成
//	float4 screen;
//	screen.xy = Tex * 2.0f - 1.0f;			//0~1を-1~1へ
//	screen.y *= -1.0f;
//	screen.z = depth;
//	screen.w = 1.0f;
//
//	float4 world = mul(screen, InvViewProj);// ワールド座標に変換
//		world.xyz /= world.w;				// パース解除
//	world.w = 1.0f;
//}

//***********************************************
//
//									PixelShader
//
//***********************************************

PS_SHADOW PS_ShadowMap(float2 Tex	:	TEXCOORD0)
{
	PS_SHADOW OUT = (PS_SHADOW)0;

	//*****************
	///ワールド座標変換
	//*****************
	float depth = tex2D(DepthSamp, Tex).r;

	//スクリーン座標の生成
	float4 screen;
	screen.xy = Tex * 2.0f - 1.0f;			//0~1を-1~1へ
	screen.y *= -1.0f;
	screen.z = depth;
	screen.w = 1.0f;

	float4 world = mul(screen, InvViewProj);// ワールド座標に変換
		world.xyz /= world.w;				// パース解除
	world.w = 1.0f;

	// ★この時点で
	/// 　奥行のdepth	スクリーン座標のscreen　ワールド座標world　 を取得


	/*_____平行光が当たっているかを計算__________*/
	// 法線マップから法線の向きの取得
	float4 n = tex2D(NormalBufSamp, Tex);
		float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
		normal = normalize(normal);			// 法線正規化
	// ライト率(ランバート) 0~1
	float r = max(0.0f, dot(normal, -ViewLightVec));


	///*****************
	//	影生産
	///*****************

	//　ワールド座標の　世界のその場所のPosと目のPosと引いて　目からポジションのVectorをゲット
	float3 V = (float3)world - ViewPos;				//視点からモデルのベクトル
		V.y = .0f;

	float dist = length(V);//　その向きの距離を取得
	float vShadowW = 1.0f - (dist / ShadowRange) * DistAlpha;			//距離の調整
	vShadowW = saturate(vShadowW);

	//　↓太陽始点からのそのポジションを手に入れる
	float3 vShadow = mulScreen(world, ShadowProjection);
	//float shadow = GetAlphaVarianceShadow(vShadow, vShadowW);
	//float shadow = GetVarianceShadow(vShadow);
	float shadow = GetShadow(vShadow, ShadowSamp);


	OUT.Shadow.rgb = shadow;
	OUT.Shadow.a = 1.0f;

	//仮で
	//if (r <= 0.0f)
	//{
	//	OUT.Shadow.rgb = 1.0f;
	//}

	OUT.Spec.rgb = shadow;
	OUT.Spec.a = 1.0f;

	return OUT;
}



// 戻り値に背マンティクスを書き込んでいたら　　　↓にセマンティクスを書かなくていい
PS_SHADOW PS_DualShadow(float2 Tex : TEXCOORD0)
{
	PS_SHADOW OUT = (PS_SHADOW)0;

	//*********************************
	///ワールド座標変換	3つの座標取得
	//*********************************
	float depth = tex2D(DepthSamp, Tex).r;

	//スクリーン座標の生成
	float4 screen;
	screen.xy = Tex * 2.0f - 1.0f;			//0~1を-1~1へ
	screen.y *= -1.0f;
	screen.z = depth;
	screen.w = 1.0f;

	float4 world = mul(screen, InvViewProj);// ワールド座標に変換
	world.xyz /= world.w;				// パース解除
	world.w = 1.0f;

	///*********************************
	//	影生産
	///*********************************

	//　ワールド座標の　世界のその場所のPosと目のPosと引いて　目からポジションのVectorをゲット
	float3 V = (float3)world - ViewPos;				//視点からモデルのベクトル
		V.y = .0f;

	float dist = length(V);//　その向きの距離を取得
	//float vShadowW = 1.0f - (dist / 220) * DistAlpha;			//距離の調整
	//vShadowW = saturate(vShadowW);//指定された値を 0 ～ 1 の範囲にクランプします
	// 距離に応じてα調整
	//float Alpha = (ShadowFar - dist) / (ShadowFar - ShadowNear);//30-35 / 30-20
	//Alpha = saturate(Alpha);//指定された値を 0 ～ 1 の範囲にクランプします
	float Alpha = smoothstep(ShadowFar, ShadowNear, dist);

	//　↓太陽始点からのそのポジションを手に入れる
	float4 vShadow = mulScreen(world, ShadowProjection);
	float4 vShadowL = mulScreen(world, ShadowProjectionL);

	/* 平行光が当たっているかを計算 */
	// 法線マップから法線の向きの取得
	float4 n = tex2D(NormalBufSamp, Tex);
	float AO = n.a;// AOレート取得
	float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
		normal = normalize(normal);			// 法線正規化
	// ライト率(ランバート) -1~1
	float rate = dot(normal, -ViewLightVec);
	rate = max(0.0f, rate);// レートを-にさせない


	// 近距離シャドウ
	float n_shadow = GetShadow((float3)vShadow, ShadowSamp);
	n_shadow = 1.0f + n_shadow*-1.0f;// 逆に影が付いている場所を＋に！
	n_shadow *= Alpha; // 離れるにつれ透明度を上げる処理
	
	// 遠距離シャドウ
	float f_shadow = GetVarianceShadow((float3)vShadowL, ShadowSampL);
	f_shadow = 1.0f + f_shadow*-1.0f;// 逆に影が付いている場所を＋に！
	f_shadow *= (1.0f - Alpha) + 0.2f; //(追加) 離れるにつれ透明度を下げる処理
	//　↑上の式をコメントアウトで二重に

	// 遠近合体シャドウ
	float dualShadow = n_shadow + f_shadow; // ファーとニア―を足した数値へ
	float adjust = 0.2f; //★光の法線の微調整
	dualShadow *= rate; // 法線のレートでセルフをごまかす
	dualShadow = 1.0f + dualShadow * -1.0f;	// 結果の数値を逆に戻す
	dualShadow = max(0.0f, dualShadow);// レートを-にさせない


	OUT.Shadow.rgb = (max(0.5f, dualShadow));
	OUT.Shadow.a = 1.0f;


	OUT.Spec.rgb = max(0.3f, dualShadow);//0~1;
	OUT.Spec.a = 1.0f;

	return OUT;
}

//*************************************
//
//							Technique
//
//*************************************

technique shadow
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = zero;
		DestBlend = SrcColor;

		BlendOpAlpha = Add;
		SrcBlendAlpha = zero;
		DestBlendAlpha = one;

		ZEnable = false;

		PixelShader = compile ps_3_0 PS_ShadowMap();
	}
}


technique DualShadow
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = zero;
		DestBlend = SrcColor;

		BlendOpAlpha = Add;
		SrcBlendAlpha = zero;
		DestBlendAlpha = one;

		ZEnable = false;

		PixelShader = compile ps_3_0 PS_DualShadow();
	}
}


//************************************************************************************************
//
//		Forward用
//
//************************************************************************************************

//------------------------------------------------------
//		頂点フォーマット
//------------------------------------------------------
struct VS_OUTPUT_HDR
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float4 wPos			: TEXCOORD1;//　ピクセルに送る情報にワールドからのポジション追加
	float4 ProjectionPos: TEXCOORD2;//　ピクセルに送る情報にゲーム画面ラストのポジション追加
};

struct VS_INPUT_HDR
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float4 Color  : COLOR0;
	float2 Tex	  : TEXCOORD0;
};

//------------------------------------------------------
//		頂点シェーダー	
//------------------------------------------------------
VS_OUTPUT_HDR VS_HDR_Lighting(VS_INPUT_HDR In)
{
	VS_OUTPUT_HDR Out = (VS_OUTPUT_HDR)0;
	//TransMatrixとPosを合成してwPosの情報生成
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color = 1.0f;

	Out.ProjectionPos = Out.Pos;

	//Out.Color.rgb = LightColor;

	return Out;
}
//------------------------------------------------------
//		ピクセルシェーダー
//------------------------------------------------------
float4 PS_HDR_Lighting(VS_INPUT_HDR In) : COLOR
{
	float4	OUT;
	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);

	OUT.rgb *= 800;
	//OUT.a = 6000.0f;
	return OUT;
}
//------------------------------------------------------
//		HDR込み描画テクニック
//------------------------------------------------------
technique HDR_Lighting
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = false;

		VertexShader = compile vs_2_0 VS_HDR_Lighting();
		PixelShader = compile ps_2_0 PS_HDR_Lighting();
	}
}


//------------------------------------------------------
//		水頂点フォーマット
//------------------------------------------------------
struct VS_INPUT_WATER
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float4 Color  : COLOR0;
	float2 Tex	  : TEXCOORD0;
};


struct VS_OUTPUT_WATER
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex1		: TEXCOORD0;
	float2 Tex2		: TEXCOORD3;

	float4 wPos			: TEXCOORD1;//　ピクセルに送る情報にワールドからのポジション追加
	float4 wvpPos		: TEXCOORD2;//　ピクセルに送る情報にゲーム画面ラストのポジション追加

	float3 vLight	: TEXCOORD4;
	float3 vE		: TEXCOORD5;

	float3 Normal	: COLOR1;
};


// グローバルパラメーター
float uvWater = 0.0f;

float3 wLightVec = { 1.0f, -1.0f, 1.0f };// 

//------------------------------------------------------
//		水頂点シェーダー	
//------------------------------------------------------
VS_OUTPUT_WATER VS_WATER(VS_INPUT_WATER In)
{
	VS_OUTPUT_WATER Out = (VS_OUTPUT_WATER)0;
	//TransMatrixとPosを合成してwPosの情報生成
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	Out.wvpPos = Out.Pos;

	Out.Tex1 = In.Tex + float2(-uvWater*1.5f, -uvWater * 1.5f);
	Out.Tex2 = In.Tex + float2(uvWater, uvWater*1.5f);

	Out.Color = 1.0f;

	//	法線変換	
	float3x3 mat = TransMatrix;
		float3 N = mul(In.Normal, mat);
		N = normalize(N);
	Out.Normal = N;

	//	頂点ローカル座標系算出
	float3	T;
	float3	B={.0f,1.0f,0.0000001f};// ダミーでY軸のベクトルを作る
	T = cross(B, N);				// まずN(z軸)と仮のY軸で外積しXを入手
	T = normalize(T);				// Xの値をベクトルに変換
	B = cross(T, N);				// N(z軸)と入手したX軸で外積してY軸を入手
	B = normalize(B);				// Y軸の値をベクトルに

	//　ワールドから接空間へもどす必要がある
	//　ライトベクトルを接空間へ
	//　回転軸だけの逆行列は転置するだけでいいので　内積に置く
	Out.vLight.x = dot(T, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＸ軸を作る
	Out.vLight.y = dot(B, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＹ軸を作る
	Out.vLight.z = dot(N, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＺ軸を作る
	Out.vLight = normalize(Out.vLight); // 最後に正規化してベクトルへ


	// 視線ベクトルを接空間へ
	float3 E = Out.wPos - ViewPos;		// 視線ベクトル
	Out.vE.x = dot(T, E);				// 同じく接空間のＸ軸を作る
	Out.vE.y = dot(B, E);				// 同じく接空間のＹ軸を作る
	Out.vE.z = dot(N, E);				// 同じく接空間のＺ軸を作る
	Out.vE = normalize(Out.vE);			// 最後に正規化してベクトルへ

	return Out;
}
//------------------------------------------------------
//		水ピクセルシェーダー
//------------------------------------------------------
float4 PS_WATER(VS_OUTPUT_WATER In) : COLOR
{
	float4 OUT = (float4)0;

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
		UV1 -= 0.02f * h * E.xy + uvSea;//あとで数値上げる
		UV2 -= 0.02f * h * E.xy + uvSea;

	/*************************/
	//	視差適用後に 法線取得
	/*************************/
		
	//法線マップを参照し、法線を視差分ずらし取得する
	float3 NMap = (tex2D(NormalSamp, UV1).rgb - 0.5f)*2.0f;
		NMap += (tex2D(NormalSamp, UV2).rgb - 0.5f)*2.0f;
		NMap *= 0.5f;

		//	視線反射ベクトル
		float3 R = reflect(-E, NMap);

	//	ライト計算
	In.vLight = normalize(In.vLight);
	float3 light;
	float rate = max(0.0f, dot(-In.vLight, NMap));//　光の計算　内積で光の反射を求める
	light = rate;//

	//	スペキュラ
	float SpecPower = 1.0f;
	float 	S = pow(max(0, dot(R, In.vLight)), 80) * (SpecPower);

	// スペキュラ加算
	OUT.rgb += S;

	//******************************************************
	/// G_Bufferを合わせ取得する
	//******************************************************
	float2 G_Fetch = (In.wvpPos.xy / In.wvpPos.w)*0.5f + 0.5f;
		G_Fetch.y = -G_Fetch.y;


	//******************************************************
	/// Z値取得　水の
	//******************************************************
	float fZ = tex2D(DepthBufSampWater, G_Fetch).r;	// 深度バッファから奥行取得
	float Z = 1 / fZ;
	
	//Z /= 6.0f;
	
	/**************************************/
	//	取得した奥行でFOG
	/**************************************/
	// 深度が深ければ深いほど透明感をなくす
	
	float ZNear = 20.0f;
	float ZFar = 600.0f;
	//float ZNear = 0.99f;
	//float ZFar = 1.1f;
	float3 ZCol = { 0.0f, 1.0f, 0.0f };
		float ZRate = smoothstep(ZFar, ZNear, fZ);

	// フォグの色をフォグの値を入れる 
	ZCol = ZCol * (1.0f - ZRate);
	OUT.rgb += ZCol;

	//******************************************************
	/// 屈折の効果
	//******************************************************
	
	// 歪み度
	float distortion = 0.02f;
	
	// 環境度
	float3 Env = tex2D(EnvFullBufSamp, G_Fetch + (float2(NMap.x, NMap.y)*distortion));	
	Env *= 0.8f;// 色を暗くする度あい
	OUT.rgb += Env;// lerp(Env, OUT.rgb, 0.3f);


	//******************************************************
	/// フレネル反射
	//******************************************************

	// 水の法線ベクトルと始点ベクトルで内積　
	float fresnel = dot(normalize(ViewPos-In.wPos), In.Normal);

	// 線形補間
	float3 RefCol = { -0.1f, -0.08f, -0.0f };
	float3 SeaCol = { 0.0f, 0.0f, 0.0f };
	float AbujustF = +0.5f;
	
	// 描画
	float3 fresnelCol;
	fresnelCol = lerp(RefCol, SeaCol, fresnel + AbujustF);
	OUT.rgb += fresnelCol;

	/**************************************/
	//
	/**************************************/

	OUT.rgb += ZCol;

	OUT.a = 1.0f;
	return OUT;
}

/* 水 */
technique F_WATER
{
	pass p0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = true;
	
		VertexShader = compile vs_3_0 VS_WATER();
		PixelShader = compile ps_3_0 PS_WATER();
	
	}

}



//------------------------------------------------------
//		デバッグ＿頂点シェーダー	
//------------------------------------------------------
VS_OUTPUT_WATER VS_L(VS_INPUT_WATER In)
{
	VS_OUTPUT_WATER Out = (VS_OUTPUT_WATER)0;
	//TransMatrixとPosを合成してwPosの情報生成
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	Out.wvpPos = Out.Pos;

	Out.Tex1 = In.Tex;
	
	Out.Color = 1.0f;

	//	法線変換	
	float3x3 mat = TransMatrix;
		float3 N = mul(In.Normal, mat);
		N = normalize(N);
	Out.Normal = N;

	//	頂点ローカル座標系算出
	float3	T;
	float3	B = { 0.0f, 1.0f, 0.00001f };	// ダミーでY軸のベクトルを作る
		T = cross(B, N);				// まずN(z軸)と仮のY軸で外積しXを入手
	T = normalize(T);				// Xの値をベクトルに変換
	B = cross(T, N);				// N(z軸)と入手したX軸で外積してY軸を入手
	B = normalize(B);				// Y軸の値をベクトルに

	//　ワールドから接空間へもどす必要がある
	//　ライトベクトルを接空間へ
	//　回転軸だけの逆行列は転置するだけでいいので　内積に置く
	Out.vLight.x = dot(T, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＸ軸を作る
	Out.vLight.y = dot(B, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＹ軸を作る
	Out.vLight.z = dot(N, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＺ軸を作る
	Out.vLight = normalize(Out.vLight); // 最後に正規化してベクトルへ


	// 視線ベクトルを接空間へ
	//float3 VP = { 0.0f, 100.0f, 100.0f };
		float3 E = Out.wPos - ViewPos;		// 視線ベクトル
		Out.vE.x = dot(T, E);				// 同じく接空間のＸ軸を作る
	Out.vE.y = dot(B, E);				// 同じく接空間のＹ軸を作る
	Out.vE.z = dot(N, E);				// 同じく接空間のＺ軸を作る
	Out.vE = normalize(Out.vE);			// 最後に正規化してベクトルへ

	return Out;
}
//------------------------------------------------------
//		デバッグ＿ピクセルシェーダー
//------------------------------------------------------
float4 PS_L(VS_OUTPUT_WATER In) : COLOR
{
	float4 OUT = (float4)0;

	float late=	dot(In.Normal, wLightVec);

	float3 E = normalize(In.vE);//目線

	/*************************/
	//	視差適用後に 法線取得
	/*************************/
	//法線マップを参照し、法線を視差分ずらし取得する
	float3 NMap = (tex2D(NormalSamp, In.Tex1).rgb - 0.5f)*2.0f;
	//NMap=normalize(NMap);

	//	視線反射ベクトル
	float3 R = reflect(-E, NMap);

	//	ライト計算
	In.vLight = normalize(In.vLight);
	float3 light;
	float rate = max(0.0f, dot(-In.vLight, NMap));//　光の計算　内積で光の反射を求める
	light = rate;//

	//	スペキュラ
	float SpecPower = 1.0f;
	float 	S = pow(max(0, dot(R, In.vLight)), 80) * (SpecPower);


	
	//OUT.r = 1.0f*light + S;
	OUT.gb += S;
	OUT.a = 1.0f;

	//(仮)
	// PLマップから色取得
	float4 pl = (float4)0;
		pl = tex2D(InfoPLSamp, float2(1.0f / 64.0f, 1.0 / 2.0f));
		OUT.rgb = pl;

	return OUT;
}
technique F_Light
{
	pass p0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_L();
		PixelShader = compile ps_3_0 PS_L();

	}

}



//************************************************************************************************
//
//		最後の処理
//
//************************************************************************************************

//--------------------------------------------------------
//		グローバル変数
//--------------------------------------------------------
float ambient = .0f;


//---------------------------------------------------------
//		演算結果のサンプラー
//---------------------------------------------------------
texture LightTex;
sampler LightSamp = sampler_state
{
	Texture = <LightTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

texture SpecTex;
sampler SpecSamp = sampler_state
{
	Texture = <SpecTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

texture ForwardTex;
sampler ForwardSamp = sampler_state
{
	Texture = <ForwardTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

texture ShadowTex;
sampler ShadowTexSamp = sampler_state
{
	Texture = <ShadowTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

texture SSAOTex;	//	SSAOテクスチャ
sampler SSAOSamp = sampler_state
{
	Texture = <SSAOTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

//------------------------------------------------------
//		スクリーンフィルタ
//------------------------------------------------------
float contrast = 1.0f;		//コントラスト
float chroma = 1.1f;			//彩度
float3 ScreenColor = { 1.0f, 1.0f, 1.0f };

//*************************************
//		ライト + ポストエフェクト
//*************************************
float4 PS_FinalPut1(float2 Tex : TEXCOORD0) :COLOR
{
	// ゴリ押しを超えた何か
	//Tex.y += .0005f;
	//Tex.x += .0005f;

	// 保管されたピクセルの中心をずらす
	//Tex.y += 1.0f / 720.0f;
	//Tex.x += 1.0f / 1280.0f;

	float4 OUT;

	float4 color;

	float4 dif;
	//ディフューズカラーの取得
	dif = tex2D(DecaleSamp, Tex);
	color = dif;

	//ライトの適応
	color *= tex2D(LightSamp, Tex);

	//スペキュラ適応
	color += tex2D(SpecSamp, Tex);

	//シャドウの適応
	color *= tex2D(ShadowTexSamp, Tex);

	//**************************
	//		ポストエフェクト
	//**************************

	//コントラスト調整
	color = ((color - 0.5f) * contrast) + 0.5f;						//コントラスト

	//彩度調整
	float rate = (color.r + color.g + color.b) / 3.0f;
	color = (color - rate) * chroma + rate;							//色相

	//カラーバランス調整
	OUT.rgb = (float3)color * ScreenColor;
	OUT.a = 1.0f;

	//return float4(color,1.0f);
	return OUT;
}

technique finalPut
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = Zero;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_FinalPut1();
	}
}


//*************************************
//		ライト + ポストエフェクト
//*************************************
float4 PS_FinalPut2(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float4 color;

	float4 dif;
	//ディフューズカラーの取得
	dif = tex2D(DecaleSamp, Tex);
	color = dif;

	//ライトの適応
	color *= tex2D(LightSamp, Tex);

	//スペキュラ適応
	color += tex2D(SpecSamp, Tex);


	//**************************
	//		ポストエフェクト
	//**************************

	//コントラスト調整
	color = ((color - 0.5f) * contrast) + 0.5f;						//コントラスト

	//彩度調整
	float rate = (color.r + color.g + color.b) / 3.0f;
	color = (color - rate) * chroma + rate;							//色相

	//カラーバランス調整
	OUT.rgb = (float3)color * ScreenColor;
	OUT.a = 1.0f;

	//return float4(color,1.0f);
	return OUT;
}

technique finalPut_notShadow
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = Zero;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_FinalPut2();
	}
}





//**************************************
//
///		ＵＶアニメーション
//
//**************************************
struct VS_INPUT_UV
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
	float4 Color	: COLOR0;
};
struct VS_OUTPUT_UV
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;
};

// UV
float tuAnime = 0.0f;
float tvAnime = 0.0f;
float alphaUV = 1.0f;

VS_OUTPUT_UV VS_UvAnime(VS_INPUT_UV In)
{
	VS_OUTPUT_UV Out = (VS_OUTPUT_UV)0;

	Out.Pos = mul(In.Pos, Projection);
	Out.Color = In.Color;// 頂点カラー取得
	Out.Tex = In.Tex + float2(tuAnime, tvAnime);//座標

	Out.Color.rgb = 1.0f;
	Out.Color.a *= alphaUV; //　透明度

	return Out;
}


//------------------------------------------------------
///		ピクセルシェーダー	
//------------------------------------------------------
float4 PS_UvAnime(VS_OUTPUT_UV In) : COLOR
{
	float4	OUT;
	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSampUV, In.Tex);
	return OUT;
}

technique uvAnime
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;			// このオブジェクトはZバッファを考慮する
		ZWriteEnable = false;	// このオブジェクトをZバッファに書き込まない

		VertexShader = compile vs_3_0 VS_UvAnime();
		PixelShader = compile ps_3_0 PS_UvAnime();
	}
}



//------------------------------------------------------
//		バリアー用頂点フォーマット
//------------------------------------------------------

struct VS_OUTPUT_UVBARRIER
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float4 wPos		: TEXCOORD1;//　ピクセルに送る情報にワールドからのポジション追加
	float4 wvpPos	: TEXCOORD2;//　ピクセルに送る情報にゲーム画面ラストのポジション追加

	float3 vLight	: TEXCOORD4;
	float3 vE		: TEXCOORD5;
	float3 Normal	: COLOR1;
};

//------------------------------------------------------
//		バリアー頂点シェーダー	
//------------------------------------------------------
VS_OUTPUT_UVBARRIER VS_UvAnime_Barrier(VS_INPUT_UV In)
{
	VS_OUTPUT_UVBARRIER Out = (VS_OUTPUT_UVBARRIER)0;
	//TransMatrixとPosを合成してwPosの情報生成
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	Out.wvpPos = Out.Pos;

	Out.Tex = In.Tex + float2(tuAnime, tvAnime);//座標

	Out.Color = In.Color;

	//	法線変換	
	float3x3 mat = TransMatrix;
		float3 N = mul(In.Normal, mat);
		N = normalize(N);
	Out.Normal = N;

	//	頂点ローカル座標系算出
	float3	T;
	float3	B = {.0f,1.0f,0.00001f};// ダミーでY軸のベクトルを作る
	T = cross(B, N);				// まずN(z軸)と仮のY軸で外積しXを入手
	T = normalize(T);				// Xの値をベクトルに変換
	B = cross(T, N);				// N(z軸)と入手したX軸で外積してY軸を入手
	B = normalize(B);				// Y軸の値をベクトルに

	//　ワールドから接空間へもどす必要がある
	//　ライトベクトルを接空間へ
	//　回転軸だけの逆行列は転置するだけでいいので　内積に置く
	Out.vLight.x = dot(T, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＸ軸を作る
	Out.vLight.y = dot(B, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＹ軸を作る
	Out.vLight.z = dot(N, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＺ軸を作る
	Out.vLight = normalize(Out.vLight); // 最後に正規化してベクトルへ


	// 視線ベクトルを接空間へ
	float3 E = Out.wPos - ViewPos;		// 視線ベクトル
	Out.vE.x = dot(T, E);				// 同じく接空間のＸ軸を作る
	Out.vE.y = dot(B, E);				// 同じく接空間のＹ軸を作る
	Out.vE.z = dot(N, E);				// 同じく接空間のＺ軸を作る
	Out.vE = normalize(Out.vE);			// 最後に正規化してベクトルへ

	return Out;
}
//------------------------------------------------------
//		バリアーピクセルシェーダー
//------------------------------------------------------
float4 PS_UvAnime_Barrier(VS_OUTPUT_UVBARRIER In) : COLOR
{
	float4 OUT = (float4)0;
	
	//**********************************************************
	// 接空間の目線
	//**********************************************************  

	// テクセルを頂点座標系での視線ベクトル方向に重みをつけてずらす。
	float3 E = normalize(In.vE);//目線

	/*************************/
	//	視差適用後に 法線取得
	/*************************/

	//法線マップを参照し、法線を視差分ずらし取得する
	float3 NMap = (tex2D(NormalSamp, In.Tex).rgb - 0.5f)*2.0f;

	//	視線反射ベクトル
	float3 R = reflect(-E, NMap);

	//	ライト計算
	//In.vLight = normalize(In.vLight);
	//float3 light;
	//float rate = max(0.0f, dot(-In.vLight, NMap));//　光の計算　内積で光の反射を求める
	//light = rate;//

	////	スペキュラ
	//float SpecPower = 1.0f;
	//float 	S = pow(max(0, dot(R, In.vLight)), 80) * (SpecPower);
	//// スペキュラ加算
	//OUT.rgb += S;

	//******************************************************
	/// G_Bufferを合わせ取得する
	//******************************************************
	float2 G_Fetch = (In.wvpPos.xy / In.wvpPos.w)*0.5f + 0.5f;
		G_Fetch.y = -G_Fetch.y;

	//******************************************************
	/// 屈折の効果
	//******************************************************
	// 歪み度
	float distortion = 0.05f;
	// 環境度
	float3 Env = tex2D(EnvFullBufSamp, G_Fetch + (float2(NMap.x, NMap.y)*distortion));
	OUT.rgb += Env;// lerp(Env, OUT.rgb, 0.3f);


	//******************************************************
	/// リムの効果
	//******************************************************
	//輪郭付近の強度を強くする　乗数で絞る
	float OutlinePower = pow(1.0f - dot(NMap, -E), 3.0f);

	//// リムカラー
	//float3 RimCol = { 1, 1, 1 };
	//float3 RimColor = (RimCol * (OutlinePower));// 
	//OUT.rgb += RimColor;

	float AlphaRate = pow(1.0f - dot(NMap, -E), 1.0f);
	
	//OUT.a = 1.0f;
	OUT.rgb += .5f;
	//OUT.a = 2.0f*AlphaRate;
	//OUT.a = .5f;
	OUT.a = 1.5f*(alphaUV*AlphaRate);
	return OUT;
}

/* バリアー */
technique uvAnime_barrier
{
	pass p0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_UvAnime_Barrier();
		PixelShader = compile ps_3_0 PS_UvAnime_Barrier();

	}

}


//------------------------------------------------------
//		ATフィールド用頂点フォーマット
//------------------------------------------------------

struct VS_OUTPUT_AT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;
	float2 NotUVTex	: TEXCOORD3;

	float4 wPos		: TEXCOORD1;//　ピクセルに送る情報にワールドからのポジション追加
	float4 wvpPos	: TEXCOORD2;//　ピクセルに送る情報にゲーム画面ラストのポジション追加

	float3 vLight	: TEXCOORD4;
	float3 vE		: TEXCOORD5;
	float3 Normal	: COLOR1;
};

//------------------------------------------------------
//		AT頂点シェーダー	
//------------------------------------------------------
VS_OUTPUT_AT VS_UvAnime_AT(VS_INPUT_UV In)
{
	VS_OUTPUT_AT Out = (VS_OUTPUT_AT)0;
	//TransMatrixとPosを合成してwPosの情報生成
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	Out.wvpPos = Out.Pos;

	Out.Tex = In.Tex + float2(tuAnime, tvAnime);//座標
	Out.NotUVTex = In.Tex;

	Out.Color = In.Color;
	Out.Color.a *= alphaUV; //　透明度

	//	法線変換	
	float3x3 mat = TransMatrix;
		float3 N = mul(In.Normal, mat);
		N = normalize(N);
	Out.Normal = N;

	//	頂点ローカル座標系算出
	float3	T;
	float3	B = { .0f, 1.0f, 0.00001f };// ダミーでY軸のベクトルを作る
		T = cross(B, N);				// まずN(z軸)と仮のY軸で外積しXを入手
	T = normalize(T);				// Xの値をベクトルに変換
	B = cross(T, N);				// N(z軸)と入手したX軸で外積してY軸を入手
	B = normalize(B);				// Y軸の値をベクトルに

	//　ワールドから接空間へもどす必要がある
	//　ライトベクトルを接空間へ
	//　回転軸だけの逆行列は転置するだけでいいので　内積に置く
	Out.vLight.x = dot(T, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＸ軸を作る
	Out.vLight.y = dot(B, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＹ軸を作る
	Out.vLight.z = dot(N, wLightVec);	// ワールド空間のベクトルをBと内積して　	接空間のＺ軸を作る
	Out.vLight = normalize(Out.vLight); // 最後に正規化してベクトルへ


	// 視線ベクトルを接空間へ
	float3 E = Out.wPos - ViewPos;		// 視線ベクトル
		Out.vE.x = dot(T, E);				// 同じく接空間のＸ軸を作る
	Out.vE.y = dot(B, E);				// 同じく接空間のＹ軸を作る
	Out.vE.z = dot(N, E);				// 同じく接空間のＺ軸を作る
	Out.vE = normalize(Out.vE);			// 最後に正規化してベクトルへ

	return Out;
}
//------------------------------------------------------
//		ATピクセルシェーダー
//------------------------------------------------------
float4 PS_UvAnime_AT(VS_OUTPUT_AT In) : COLOR
{
	float4 OUT = (float4)0;

	//**********************************************************
	// 接空間の目線
	//**********************************************************  

	// テクセルを頂点座標系での視線ベクトル方向に重みをつけてずらす。
	float3 E = normalize(In.vE);//目線

	/*************************/
	//	色々なMAPから色を取り出す
	/*************************/

	//法線マップを参照し、法線を視差分ずらし取得する
	float3 NMap = (tex2D(NormalSamp, In.Tex).rgb - 0.5f)*2.0f;

	//	ピクセル色決定
	float4 col = In.Color * tex2D(DecaleSampUV, In.Tex);

	// HMapから蜂の巣取得
	float Hrate = (tex2D(HeightSamp, In.NotUVTex).r);


	//	視線反射ベクトル
	float3 R = reflect(-E, NMap);


	/*****************************/
	//	距離取得
	/*****************************/

	// 向きや距離
	float3 V = (float3)In.wPos - ViewPos;				//視点からモデルのベクトル
	V.y = .0f;
	float dist = length(V);//　その向きの距離を取得

	float ZRate = smoothstep(300.0f, 50.0f, dist);		// 奥と近距離を決めてレートを作る！

	float3 nearCol = { 0.1f, 0.3f, 0.2f };				// ニアカラー
	OUT.rgb += nearCol*ZRate;							// 近いものは紫に

	//******************************************************
	/// G_Bufferを合わせ取得する
	//******************************************************
	float2 G_Fetch = (In.wvpPos.xy / In.wvpPos.w)*0.5f + 0.5f;
	G_Fetch.y = -G_Fetch.y;

	//******************************************************
	/// 屈折の効果
	//******************************************************
	// 歪み度
	float distortion = 0.015f*ZRate;// 近かったらめっちゃ揺らすぜ！！
	// 環境度
	float3 Env = tex2D(EnvFullBufSamp, G_Fetch + (float2(NMap.x, NMap.y)*distortion));
		OUT.rgb += Env;// lerp(Env, OUT.rgb, 0.3f);

	//******************************************************
	/// 色の効果
	//******************************************************
	
	// 移動
	OUT.rgb += (col*Hrate)*col.a;	// 0は黒　1で色が変わるのでさらに自分のアルファも考慮
	
	//OUT.a = col.a*Hrate;
	OUT.a = 1.0f;
	//OUT.a = 1.5f*(alphaUV*AlphaRate);
	return OUT;
}

/* バリアー */
technique uvAnime_at
{
	pass p0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = true;

		VertexShader = compile vs_3_0 VS_UvAnime_AT();
		PixelShader = compile ps_3_0 PS_UvAnime_AT();

	}

}



/*************************************************************/
//
///			2Dのポストエフェクト類
//
/*************************************************************/

/*ぼかし用サンプラー*/
sampler GaussianSamp = sampler_state
{
	Texture = <Texture>;
	MinFilter = GAUSSIANQUAD;
	MagFilter = GAUSSIANQUAD;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};


float TU = 1.0f / 1280.0f, TV = 1.0f / 720.0f;
float BlurValue = 1.0f;
float4 PS_gaussX(float2 Tex : TEXCOORD0) : COLOR
{

	//テクセルを取得   
	float2 Texel0 = Tex + float2(-TU * 1 * BlurValue, 0.0f);
	float2 Texel1 = Tex + float2(-TU * 2 * BlurValue, 0.0f);
	float2 Texel2 = Tex + float2(-TU * 3 * BlurValue, 0.0f);
	float2 Texel3 = Tex + float2(-TU * 4 * BlurValue, 0.0f);
	float2 Texel4 = Tex + float2(-TU * 5 * BlurValue, 0.0f);

	float2 Texel5 = Tex + float2(TU * 1 * BlurValue, 0.0f);
	float2 Texel6 = Tex + float2(TU * 2 * BlurValue, 0.0f);
	float2 Texel7 = Tex + float2(TU * 3 * BlurValue, 0.0f);
	float2 Texel8 = Tex + float2(TU * 4 * BlurValue, 0.0f);
	float2 Texel9 = Tex + float2(TU * 5 * BlurValue, 0.0f);

	//取得したテクセル位置のカラー情報を取得する。
	//それぞれのカラー値に重みをかけている。この重み値はすべての合計が 1.0f になるように調整する。
	float4 p0 = tex2D(GaussianSamp, Tex) * 0.20f;

	float4 p1 = tex2D(GaussianSamp, Texel0) * 0.12f;
	float4 p2 = tex2D(GaussianSamp, Texel1) * 0.10f;
	float4 p3 = tex2D(GaussianSamp, Texel2) * 0.08f;
	float4 p4 = tex2D(GaussianSamp, Texel3) * 0.06f;
	float4 p5 = tex2D(GaussianSamp, Texel4) * 0.04f;

	float4 p6 = tex2D(GaussianSamp, Texel5) * 0.12f;
	float4 p7 = tex2D(GaussianSamp, Texel6) * 0.10f;
	float4 p8 = tex2D(GaussianSamp, Texel7) * 0.08f;
	float4 p9 = tex2D(GaussianSamp, Texel8) * 0.06f;
	float4 p10 = tex2D(GaussianSamp, Texel9) * 0.04f;

	//カラーを合成する
	return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9 + p10;

}

float4 PS_gaussY(float2 Tex : TEXCOORD0) : COLOR
{

	//テクセルを取得   
	float2 Texel0 = Tex + float2(0.0, -TV * 1 * BlurValue);
	float2 Texel1 = Tex + float2(0.0, -TV * 2 * BlurValue);
	float2 Texel2 = Tex + float2(0.0, -TV * 3 * BlurValue);
	float2 Texel3 = Tex + float2(0.0, -TV * 4 * BlurValue);
	float2 Texel4 = Tex + float2(0.0, -TV * 5 * BlurValue);
				    
	float2 Texel5 = Tex + float2(0.0, TV * 1 * BlurValue);
	float2 Texel6 = Tex + float2(0.0, TV * 2 * BlurValue);
	float2 Texel7 = Tex + float2(0.0, TV * 3 * BlurValue);
	float2 Texel8 = Tex + float2(0.0, TV * 4 * BlurValue);
	float2 Texel9 = Tex + float2(0.0, TV * 5 * BlurValue);

	///取得したテクセル位置のカラー情報を取得する。
	///それぞれのカラー値に重みをかけている。この重み値はすべての合計が 1.0f になるように調整する。
	float4 p0 = tex2D(GaussianSamp, Tex) * 0.20f;

	float4 p1 = tex2D(GaussianSamp, Texel0) * 0.12f;
	float4 p2 = tex2D(GaussianSamp, Texel1) * 0.10f;
	float4 p3 = tex2D(GaussianSamp, Texel2) * 0.08f;
	float4 p4 = tex2D(GaussianSamp, Texel3) * 0.06f;
	float4 p5 = tex2D(GaussianSamp, Texel4) * 0.04f;

	float4 p6 = tex2D(GaussianSamp, Texel5) * 0.12f;
	float4 p7 = tex2D(GaussianSamp, Texel6) * 0.10f;
	float4 p8 = tex2D(GaussianSamp, Texel7) * 0.08f;
	float4 p9 = tex2D(GaussianSamp, Texel8) * 0.06f;
	float4 p10 = tex2D(GaussianSamp, Texel9) * 0.04f;

	//カラーを合成する
	return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9 + p10;

}


technique gaussZ
{

	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// シェーダ
		PixelShader = compile ps_3_0 PS_gaussX();
	}

	pass P1
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// シェーダ
		PixelShader = compile ps_3_0 PS_gaussY();
	}
}

//************************************
//		１Pixelずつだけずらすブラ―(処理が軽い)
//************************************
float4 PS_Gaussian(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	//中央
	OUT = tex2D(GaussianSamp, Tex) * .2f;
	//右
	OUT += tex2D(GaussianSamp, float2(Tex.x + TU, Tex.y)) * .1f;
	//左
	OUT += tex2D(GaussianSamp, float2(Tex.x - TU, Tex.y)) * .1f;
	//上
	OUT += tex2D(GaussianSamp, float2(Tex.x, Tex.y - TV)) * .1f;
	//下
	OUT += tex2D(GaussianSamp, float2(Tex.x, Tex.y + TV)) * .1f;
	//左上
	OUT += tex2D(GaussianSamp, float2(Tex.x - TU, Tex.y - TV)) * .1f;
	//左下
	OUT += tex2D(GaussianSamp, float2(Tex.x - TU, Tex.y + TV)) * .1f;
	//右上
	OUT += tex2D(GaussianSamp, float2(Tex.x + TU, Tex.y - TV)) * .1f;
	//右下
	OUT += tex2D(GaussianSamp, float2(Tex.x + TU, Tex.y + TV)) * .1f;

	//OUT.rgb = 1.0f;

	//OUT.a = tex2D(GaussianSamp,Tex).a;
	return OUT;

}

technique gaussian
{
	pass P0
	{

		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_Gaussian();
	}
}

/*-------------------*/
//  HDRブルーム効果
/*-------------------*/

// ------------------------------
/// ブルームの種を作成
// ------------------------------

// ぼかす前にぼかす明るさを調整
float4 PS_hdrBloomSeed(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT;

	OUT = tex2D(GaussianSamp, Tex);

	OUT.rgb -= 0.7f;//ここで数値を絞る

	//スペキュラのg成分から情報取得
	float Reflect_rate = tex2D(SpecularBufSamp, Tex).g;

	OUT.r = max(0.0f, OUT.r);
	OUT.g = max(0.0f, OUT.g);
	OUT.b = max(0.0f, OUT.b);

	OUT.rgb *= Reflect_rate;//　レート分掛ける

	return OUT;
}
technique make_hdrBloomSeed
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// シェーダ
		PixelShader = compile ps_3_0 PS_hdrBloomSeed();
	}
}

// ------------------------------
/// 作成したブルームをのっける
// ------------------------------
float4 PS_hdrBloom(float2 Tex:TEXCOORD0) : COLOR
{
	float4 OUT;
	OUT = tex2D(GaussianSamp, Tex);
	OUT.rgb += OUT.rgb* 3.5f;//←ブルームの色の強さを調整
	return OUT;
}

technique add_hdrBloom
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = one;
		// シェーダ
		PixelShader = compile ps_3_0 PS_hdrBloom();
	}
}




/****************************/
/*			ToneMap			*/
/****************************/

/*露光度*/
float exposure = 0.0f;

/*ピクセルシェーダー*/
float4 PS_Tone(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT = (float4)0;

	//No.2	1.0から2.0　明るい
	OUT.rgba = tex2D(DecaleSamp, Tex)* exp2(exposure);
	OUT.a = 1.0f;

	return OUT;
}

technique ToneMap
{

	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// エフェクトが出なくなったため OFFに
		ZEnable = False;// このオブジェクトはZバッファを考慮しない

		// シェーダ
		PixelShader = compile ps_3_0 PS_Tone();
	}

}

/*ピクセルシェーダー*/
float4 PS_Tone_DOF(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT = (float4)0;

	//No.2	1.0から2.0　明るい
	OUT.rgba = tex2D(DecaleSamp, Tex)* exp2(exposure);
	OUT.a = 1.0f;


	//スクリーン座標系位置の取得
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1から1に戻す
	pos.y *= -1;
	pos.z = tex2D(DepthBufSamp, Tex).r;
	pos.w = 1.0f;

	//逆行列でビュー座標系に変換する
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	float3 Length = (float3)pos - DOF_target;
		float l = length(Length);

	//rangeより離れた位置からαを増やしていく
	//float alpha = saturate(l / range - 1.0f);
	float alpha = l / DOF_Range;
	OUT.a = saturate(alpha);// 0~1へ

	return OUT;
}

technique ToneMap_DOF
{

	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZEnable = False;
		// シェーダ
		PixelShader = compile ps_3_0 PS_Tone_DOF();
	}

}



/****************************/
/*	ToneMap	+ Downsample用	*/
/****************************/


/*ピクセルシェーダー*/
float4 PS_Tone_DownSample(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT = (float4)0;

	//No.2	1.0から2.0　明るい
	OUT.rgba = tex2D(DecaleSamp, Tex)* exp2(exposure);
	OUT.a = 1.0f;

	//彩度調整
	float4 color = OUT;
	float rate = (color.r + color.g + color.b) / 3.0f;
	color.r = (rate);//色相をR成分に詰める
	color.gb = 0.0f;
	color.a = 1.0f;
	//float4 OUT2 = (float4)0;
	//OUT2.r = rate;

	return color;
}

technique ToneMap_DownSample
{

	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// エフェクトが出なくなったため OFFに
		ZEnable = False;// このオブジェクトはZバッファを考慮しない

		// シェーダ
		PixelShader = compile ps_3_0 PS_Tone_DownSample();
	}

}

/****************************/
/*	 Downsample用	*/
/****************************/


/*ピクセルシェーダー*/
float4 PS_DownSample(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT = (float4)0;

	OUT.rgba = tex2D(DecaleSamp, Tex);
	OUT.a = 1.0f;

	//彩度調整
	float4 color = OUT;
		float rate = (color.r + color.g + color.b) / 3.0f;
	color.r = (rate);//色相をR成分に詰める
	color.gb = 0.0f;
	color.a = 1.0f;
	//float4 OUT2 = (float4)0;
	//OUT2.r = rate;

	return color;
}

technique DownSample
{

	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// エフェクトが出なくなったため OFFに
		ZEnable = False;// このオブジェクトはZバッファを考慮しない

		// シェーダ
		PixelShader = compile ps_3_0 PS_DownSample();
	}

}
