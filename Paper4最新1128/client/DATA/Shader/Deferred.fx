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
float4x4 matProjection;	//  ビュー->プロジェクション変換行列

//------------------------------------------------------
///		テクスチャサンプラー	
//------------------------------------------------------
///WRAP...反対側から同じ絵を繰り返します
///CLAMP...端の色を繰り返します
///BORDER...透明色にします
texture Texture;	//	デカールテクスチャ
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

	//ディヒューズ
	//OUT.Diffuse = In.Color * tex2D( DecaleSamp,Tex );
	OUT.Diffuse = tex2D(DecaleSamp, Tex);

	//スペキュラ
	OUT.Specular = tex2D(SpecularSamp, Tex);
	OUT.Specular.a = 1.0f;
	//OUT.Position.a = 1.0f;

	//深度 r32bit
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
	OUT.Normal.a = tex2D(HeightSamp, Tex).r;//AOマップを法線のアルファに
	
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


PS_DEFERRED PS_Sea(VS_OUTPUT_SEA In) : COLOR
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
	OUT.Normal.a = 1.0f;// ハイトマップを使っているのでAOは仮に1.0f

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
///
///		ディファードでのライト処理
///
//************************************************************************************************

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
	pos.y *= -1;
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

float3 LightVec = { 1.0f, -1.0f, 1.0f };// cpu側でビュー座標系に変換する
float RimPower = 2.0f;

DEF_LIGHT PS_DirLight(float2 Tex:TEXCOORD0) :COLOR
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
	float rate = dot(normal, -LightVec);
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
	float3 R = normalize(-LightVec - E);
	float sp = pow(max(0, dot(R, normal)), 80) * sp_rate;// いつも通り絞って


	//　各色に光のパラメーターを設定
	//  R->Spec  G->Bloom  B->Rim
	float3 multiRate = tex2D(SpecularBufSamp, Tex).rgb;

		// (R) スペキュラの強さ
		OUT.spec.rgb = LightColor * multiRate.r * sp;// *(1.0f - multiRate.b) + env.rgb * multiRate.b;
		OUT.spec.a = 1.0f;

		// (B) リムライト
		//ライトがカメラ正面近くにあるほど強くなるようにする。
		float RimPower = max(.0f, dot(-E, LightVec));
		//輪郭付近の強度を強くする
		float OutlinePower = pow(1.0f - dot(normal, -E), 5.0f);

		//リムカラー
		float3 RimColor = (LightColor * (OutlinePower*RimPower))*multiRate.b;// 青の要素で強さを変える
		OUT.spec.rgb += RimColor;


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
float3 LightPos = { .0f, .0f, .0f };// cpu側でビュー座標にした光源位置
float LightRange = .0f;				// 光源の広がり具合
float LightPower = .0f;

DEF_LIGHT PS_PointLight(float2 Tex : TEXCOORD0) :COLOR
{
	DEF_LIGHT OUT;

	//スクリーン座標系位置の取得
	float4 pos = ProjectionToView(Tex);

	// (光源との距離)と(光の向き)を取得
	float3 lightDir = ((float3)pos - LightPos) / LightRange;

	if (length(lightDir) < 1.0f)
	{
		// 減衰量込みの距離
		float distance = max(0, 1.0f - (length(lightDir)
			/ LightPower));//←数値を変え絞る

		// 減衰量込みのポイントライトを乗せる
		OUT.color.rgb = LightColor * distance;

		// 正規化してベクトルへ
		float3 lightVec = normalize(lightDir);

		// 法線マップから法線の向きの取得
		float4 n = tex2D(DecaleSamp, Tex);
			float3 normal = n.rgb * 2.0f - 1.0f;
			normal = normalize(normal);
		// ライト率
		float r = max(0, dot(normal, -lightVec));

		// ピクセル色決定
		OUT.color.rgb *= r;
		OUT.color.a = 1;


		//スペキュラ
		float3 E = pos.xyz;
			E = normalize(E);
		float3 R = normalize(-lightVec - E);
			float sp = pow(max(0, dot(R, normal)), 10) * r;

		float3 multiRate = tex2D(SpecularBufSamp, Tex).rgb;
			OUT.spec.rgb = LightColor * multiRate.r * sp;
		OUT.spec.a = 1.0f;

		//減衰
		OUT.spec.rgb *= distance * distance;

	}

	return OUT;

}

technique def_PointLight
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_PointLight();
	}
}

//**********************************
///		シンプル点光源
//**********************************

DEF_LIGHT PS_SimpliPointLight(float2 Tex : TEXCOORD0) :COLOR
{
	DEF_LIGHT OUT = (DEF_LIGHT)0;

	//スクリーン座標系位置の取得
	float4 pos = ProjectionToView(Tex);

		// (光源との距離)と(光の向き)を取得
		float3 lightDir = ((float3)pos - LightPos) / LightRange;

		// 距離計算
		float distance = max(0, 1.0f - (length(lightDir)));//←数値を変え絞る

	//光源の距離が＿の場合は破棄する.
	if (length(lightDir) > 1.0f) discard;

		// 減衰量込みのポイントライトを乗せる
		OUT.color.rgb = LightColor * min(1.0f, (distance*4.0f));
		//OUT.color.r = 100;
		OUT.color.a = 1.0f;
		
		//スペキュラ
		OUT.spec.rgb = 0.0f;
		OUT.spec.a = 1.0f;

	
	
	return OUT;

}

technique def_SimpliPointLight
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_SimpliPointLight();
	}
}

//*************************************
//
///		半球ライティング
//
//*************************************
float3 SkyColor = { .4f, .6f, .6f };
float3 GroundColor = { .6f, .4f, .4f };

float4 PS_HemiLight(float2 Tex : TEXCOORD0) :COLOR
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

float4 PS_DOF(float2 Tex : TEXCOORD0) :COLOR
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

	float3 Length = pos - DOF_target;
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
float4x4 InvViewProj;			//　プロジェクション->ワールド変換行列

float AdjustValue = -0.002f;	//　何処までを影と判断するのか　下げると綺麗に影が付くがジャギーが目立つ　上げるとその逆
float Shadow = .6f;				//　影の濃さ
float DistAlpha = .2f;			//　距離に応じて影のαを下げる

float ShadowRange = 0.0f;


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
		AlphaFunc = GREATEREQUAL;

		ZWriteEnable = true;
		CullMode = none;

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

//*************************
///		テクスチャ座標生成関数
//*************************
inline float3 GetShadowTex(float4 Pos)
{
	float4	Tex;
	// テクスチャ座標計算
	Tex = mul(Pos, ShadowProjection);

	Tex.y = -Tex.y;
	Tex.xy = 0.5f * Tex.xy + 0.5f;
	Tex.xyz /= Tex.w;

	return Tex.xyz;
}

inline float3 GetShadowTexL(float4 Pos)
{
	float4	Tex;
	// テクスチャ座標計算
	Tex = mul(Pos, ShadowProjectionL);

	Tex.y = -Tex.y;
	Tex.xy = 0.5f * Tex.xy + 0.5f;
	Tex.xyz /= Tex.w;

	return Tex.xyz;
}

//通常版
inline float GetShadow(float3 Tex)
{
	float d = tex2D(ShadowSamp, Tex.xy).r;		//シャドウマップから深度を取り出す
	if (Tex.z > 1.0f) Tex.z = 1.0f;
	float l = (d < Tex.z + AdjustValue) ? Shadow : 1;
	return l;
}
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

//バリアンス
inline float GetVarianceShadow(float3 Tex)
{
	half d = Tex.z;

	//	近景値
	float2	t = tex2D(ShadowSamp, Tex.xy).rg;		// シャドウマップから深度を取り出す
		float d1 = t.r;							// 深度値
	float variance = max(t.g - d1 * d1, .0f);	// 分散

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// チェビシェフの不等式

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;

	//	濃度補正	
	l = 1.0f - (l * (1.0f - Shadow));

	return l;
}

inline float GetVarianceShadowL(float3 Tex)
{
	half d = Tex.z;

	//	遠景値
	float2	t = tex2D(ShadowSampL, Tex.xy).rg;		// シャドウマップから深度を取り出す
		float d1 = t.r;							// 深度値
	float variance = max(t.g - d1 * d1, .0f);	// 分散

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// チェビシェフの不等式

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;

	//	濃度補正	
	l = 1.0f - (l * (1.0f - Shadow));

	return l;
}

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
	float l = (1.0f - max(l1, l2)) * .9f;

	//	濃度補正	
	l = 1.0f - (l * (1.3f - Shadow));

	return l;
}


inline float GetAlphaVarianceShadow(float3 Tex, float a)
{
	half d = Tex.z;

	//	近景値
	float2	t = tex2D(ShadowSamp, Tex.xy).rg;		// シャドウマップから深度を取り出す
		float d1 = t.r;							// 深度値
	float variance = max(t.g - d1 * d1, .0f);	// 分散

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// チェビシェフの不等式

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;


	//	濃度補正	
	l = 1.0f - (l * (1.0f - Shadow)) * a;

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

PS_SHADOW PS_ShadowMap(float2 Tex	:	TEXCOORD0) : COLOR
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
	float r =max(0.0f, dot(normal, -LightVec));


	///*****************
	//	影生産
	///*****************

	//　ワールド座標の　世界のその場所のPosと目のPosと引いて　目からポジションのVectorをゲット
	float3 V = world - ViewPos;				//視点からモデルのベクトル
		V.y = .0f;

	float dist = length(V);//　その向きの距離を取得
	float vShadowW = 1.0f - (dist / ShadowRange) * DistAlpha;			//距離の調整
	vShadowW = saturate(vShadowW);

	//　↓太陽始点からのそのポジションを手に入れる
	float3 vShadow = GetShadowTex(world);
	//float shadow = GetAlphaVarianceShadow(vShadow, vShadowW);
	//float shadow = GetVarianceShadow(vShadow);
	float shadow = GetShadow(vShadow);


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

float4 PS_ShadowMapCascade(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT;

	//*****************
	// ワールド座標変換
	//*****************
	float depth = tex2D(DepthSamp, Tex).r;

	//スクリーン座標の生成
	float4 screen;
	screen.xy = Tex * 2.0f - 1.0f;			//0~1を-1~1へ
	screen.y *= -1.0f;
	screen.z = depth;
	screen.w = 1.0f;

	float4 world = mul(screen, InvViewProj);
		world.xyz /= world.w;						//パース解除
	world.w = 1.0f;


	//**************************
	// 視点からの距離計算(カスケード用)
	//**************************
	float3 V = world - ViewPos;				//視点からモデルのベクトル
		V.y = .0f;

	float dist = length(V);

	float3 vShadow = mulScreen(world, ShadowProjection);
	float3 vShadowL = mulScreen(world, ShadowProjectionL);
	float vShadowW = (30.0f - (dist - 110.0f)) * 1.25f;			//距離の調整
	vShadowW = saturate(vShadowW);

	float shadow = GetShadowCascade(vShadow, vShadowL, vShadowW);
	shadow = 1.0f - ((1.0f - shadow) * .5f);


	/*_____平行光が当たっているかを計算__________*/
	// 法線マップから法線の向きの取得
	float4 n = tex2D(NormalBufSamp, Tex);
	float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
	normal = normalize(normal);			// 法線正規化
	// ライト率(ランバート) 0~1
	float r =  dot(normal, -LightVec);
	float flag = step(0.0f, r);//0以下なら1に以上なら0に

	//float near_ShadowLevel = max(Shadow, GetSM(vShadow.xyz, ShadowSamp));
	//float far_ShadowLevel = max(Shadow, GetSM(vShadowL.xyz, ShadowSampL));
	//float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(130.0, (depth / 23.0), 41.0));//左の数値で丁度いい場所を探す
	//OUT.rgb = s;

	// ★ごり押しで遠くに行くと薄くなる
	float n_shadow = GetShadow(vShadow);
	n_shadow = 1 + n_shadow * -1;
	n_shadow *= 1.0 - (dist / ShadowRange) * DistAlpha;

	float f_shadow = 1 + shadow*-1;
	n_shadow += f_shadow;

	n_shadow=1 + n_shadow*-1;



	OUT.rgb = n_shadow + 1.0f - (1.0f*flag);//0~1
	OUT.a = 1.0f;

	//仮で


	return OUT;
}


PS_SHADOW PS_DualShadow(float2 Tex	:	TEXCOORD0) : COLOR
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

	///*****************
	//	影生産
	///*****************

	//　ワールド座標の　世界のその場所のPosと目のPosと引いて　目からポジションのVectorをゲット
	float3 V = world - ViewPos;				//視点からモデルのベクトル
		V.y = .0f;

	float dist = length(V);//　その向きの距離を取得
	float vShadowW = 1.0f - (dist / 20) * DistAlpha;			//距離の調整
	vShadowW = saturate(vShadowW);

	//　↓太陽始点からのそのポジションを手に入れる
	float3 vShadow = mulScreen(world, ShadowProjection);
	float3 vShadowL = mulScreen(world, ShadowProjectionL);


	//float far_shadow = GetVarianceShadow(vShadowL, ShadowSampL);


	// ★ごり押しで遠くに行くと薄くなる
	float n_shadow = GetShadow(vShadow);
	float f_shadow = GetShadow(vShadowL, ShadowSampL);
	//f_shadow *= 100;
	n_shadow = 1 + n_shadow*-1;
	n_shadow *= 1.0 - (dist / ShadowRange) * DistAlpha;
	f_shadow = 1 + f_shadow*-1;
	n_shadow += f_shadow;
	n_shadow = 1 + n_shadow*-1;


	/* 平行光が当たっているかを計算 */
	// 法線マップから法線の向きの取得
	float4 n = tex2D(NormalBufSamp, Tex);
		float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
		normal = normalize(normal);			// 法線正規化
	// ライト率(ランバート) -1~1
	float r = dot(normal, -LightVec);
	//r = max(0.0f, r);// レートを-にさせない

	//float flag = step(0.4f, r);//0以下なら1に以上なら0に 0.3調整　これを増やすとどこまで陰影に影を落とさないかを調整できる


	//　HDRで1以上になっているので1で斬るようにする
	//OUT.Shadow.rgb = max(0.5f, n_shadow + 1.2f - (1.0f*r));//0~1; //影の濃さを一定に 1.0で調整
	OUT.Shadow.rgb = n_shadow;//0~1;
	OUT.Shadow.b += 0.05f;// 遊びで青っぽく
	OUT.Shadow.a = 1.0f;


	OUT.Spec.rgb = max(0.3f, n_shadow);//0~1;
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

technique CascadeShadow
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

		PixelShader = compile ps_3_0 PS_ShadowMapCascade();
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

	OUT.rgb *= 3;
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
float chroma = 1.0f;			//彩度
float3 ScreenColor = { 1.0f, 1.0f, 1.0f };

//*************************************
//		ライト + ポストエフェクト
//*************************************
float4 PS_Def1(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float3 color;

	float3 dif;
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
	OUT.rgb = color * ScreenColor;
	OUT.a = 1.0f;

	//return float4(color,1.0f);
	return OUT;
}

technique def_mix1
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = Zero;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_Def1();
	}
}


//*************************************
//		ライト + ポストエフェクト
//*************************************
float4 PS_Def2(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float3 color;

	float3 dif;
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
	OUT.rgb = color * ScreenColor;
	OUT.a = 1.0f;

	//return float4(color,1.0f);
	return OUT;
}

technique def_mix2
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = Zero;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_Def2();
	}
}

//*************************************
//		全部
//*************************************
float4 PS_Def3(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float3 color;

	float3 dif;
	//ディフューズカラーの取得
	dif = tex2D(DecaleSamp, Tex);
	color = dif;

	//ライトの適応
	color *= tex2D(LightSamp, Tex);

	//SSAOの適応
	color *= tex2D(SSAOSamp, Tex).r;

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
	OUT.rgb = color * ScreenColor;
	OUT.a = 1.0f;

	//return float4(color,1.0f);
	return OUT;
}

technique def_mix3
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = Zero;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_Def3();
	}
}




//**************************************
//
//		ＵＶアニメーション
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
//		ピクセルシェーダー	
//------------------------------------------------------
float4 PS_UvAnime(VS_OUTPUT_UV In) : COLOR
{
	float4	OUT;
	//	ピクセル色決定
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);
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





/*************************************************************/
///
///				2Dのポストエフェクト類
///
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
//		１PixelBlur
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
// ブルームの種を作成
// ------------------------------

// ぼかす前にぼかす明るさを調整
float4 PS_hdrBloomSeed(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT;

	OUT = tex2D(GaussianSamp, Tex);

	OUT.rgb -= 0.75f;//ここで数値を絞る

	//スペキュラのg成分から情報取得
	float Reflect_rate = tex2D(SpecularBufSamp, Tex).g;

	OUT.r = max(0.0f, OUT.r);
	OUT.g = max(0.0f, OUT.g);
	OUT.b = max(0.0f, OUT.b);

	OUT.rgb *= Reflect_rate;

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
// 作成したブルームをのっける
// ------------------------------
float4 PS_hdrBloom(float2 Tex:TEXCOORD0) : COLOR
{
	float4 OUT;
	OUT = tex2D(GaussianSamp, Tex);
	OUT.rgb += OUT.rgb* 2.5f;//←ブルームの色の強さを調整
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
	OUT.rgb = tex2D(DecaleSamp, Tex)* exp2(exposure);
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
	OUT.rgb = tex2D(DecaleSamp, Tex)* exp2(exposure);
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

	float3 Length = pos - DOF_target;
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


//*****************************************
//		ScreenSpaceAmbientOcclusion
//*****************************************

//サンプリング数(16,12,10,8)
#define SAMPLES 16							//周囲ピクセルのサンプリング数
float HemRadius = 1.0f;						//環境光遮蔽判定用の半球の半径
float Zfar = 0.01f;							//環境光遮蔽判定用の深度差の最大値
float AOPower = 3.0f;						//陰の強度

#if SAMPLES == 16
const float3 SphereArray[16] = {
	float3(0.53812504f, 0.18565957f, -0.43192f),
	float3(0.13790712f, 0.24864247f, 0.44301823f),
	float3(0.33715037f, 0.56794053f, -0.005789503f),
	float3(-0.6999805f, -0.04511441f, -0.0019965635f),
	float3(0.06896307f, -0.15983082f, -0.85477847f),
	float3(0.056099437f, 0.006954967f, -0.1843352f),
	float3(-0.014653638f, 0.14027752f, 0.0762037f),
	float3(0.010019933f, -0.1924225f, -0.034443386f),
	float3(-0.35775623f, -0.5301969f, -0.43581226f),
	float3(-0.3169221f, 0.106360726f, 0.015860917f),
	float3(0.010350345f, -0.58698344f, 0.0046293875f),
	float3(-0.08972908f, -0.49408212f, 0.3287904f),
	float3(0.7119986f, -0.0154690035f, -0.09183723f),
	float3(-0.053382345f, 0.059675813f, -0.5411899f),
	float3(0.035267662f, -0.063188605f, 0.54602677f),
	float3(-0.47761092f, 0.2847911f, -0.0271716f)
};

#elif SAMPLES == 12
const float3 SphereArray[12] = {
	float3(-0.13657719, 0.30651027, 0.16118456),
	float3(-0.14714938, 0.33245975, -0.113095455),
	float3(0.030659059, 0.27887347, -0.7332209),
	float3(0.009913514, -0.89884496, 0.07381549),
	float3(0.040318526, 0.40091, 0.6847858),
	float3(0.22311053, -0.3039437, -0.19340435),
	float3(0.36235332, 0.21894878, -0.05407306),
	float3(-0.15198798, -0.38409665, -0.46785462),
	float3(-0.013492276, -0.5345803, 0.11307949),
	float3(-0.4972847, 0.037064247, -0.4381323),
	float3(-0.024175806, -0.008928787, 0.17719103),
	float3(0.694014, -0.122672155, 0.33098832)
};

#elif SAMPLES == 10
const float3 SphereArray[10] = {
	float3(-0.010735935, 0.01647018, 0.0062425877),
	float3(-0.06533369, 0.3647007, -0.13746321),
	float3(-0.6539235, -0.016726388, -0.53000957),
	float3(0.40958285, 0.0052428036, -0.5591124),
	float3(-0.1465366, 0.09899267, 0.15571679),
	float3(-0.44122112, -0.5458797, 0.04912532),
	float3(0.03755566, -0.10961345, -0.33040273),
	float3(0.019100213, 0.29652783, 0.066237666),
	float3(0.8765323, 0.011236004, 0.28265962),
	float3(0.29264435, -0.40794238, 0.15964167)
};

#else
const float3 SphereArray[8] = {
	float3(0.24710192, 0.6445882, 0.033550154),
	float3(0.00991752, -0.21947019, 0.7196721),
	float3(0.25109035, -0.1787317, -0.011580509),
	float3(-0.08781511, 0.44514698, 0.56647956),
	float3(-0.011737816, -0.0643377, 0.16030222),
	float3(0.035941467, 0.04990871, -0.46533614),
	float3(-0.058801126, 0.7347013, -0.25399926),
	float3(-0.24799341, -0.022052078, -0.13399573)
};

#endif


float4 PS_SSAO(float2 Tex : TEXCOORD0) :COLOR0
{
	float4 OUT;

	//描画ピクセルの法線ベクトルを取得
	float3 normal = tex2D(NormalBufSamp, Tex).rgb * 2.0f - 1.0f;
		normal = normalize(normal);

	//描画ピクセルの深度の取得
	float depth = tex2D(DepthBufSamp, Tex).r;

	//プロジェクション座標系の取得
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1から1に戻す
	pos.y *= -1;
	pos.z = depth;
	pos.w = 1.0f;

	//ビュー座標系での位置の取得
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	float normAO = .0f;
	float depthAO = .0f;

	//レイを飛ばして遮蔽されているか
	for (uint i = 0; i < SAMPLES; i++){

		//レイの方向ベクトル
		float3 ray = SphereArray[i] * HemRadius;

		//レイの方向ベクトルを描画ピクセルの法線方向の半球内に収まるように方向を変換する
		ray = sign(dot(normal, ray)) * ray;

		//周囲ピクセルの座標
		float4 envPos;
		//レイ方向に移動
		envPos.xyz = pos.xyz + ray;
		//プロジェクション座標系に変換
		envPos = mul(float4(envPos.xyz, 1.0f), matProjection);
		//スクリーン座標系に変換
		envPos.xy = envPos.xy / envPos.w * float2(.5f, -.5f) + .5f;

		//周囲ピクセルの法線ベクトルの取得
		float3 envNormal = tex2D(NormalBufSamp, envPos.xy).xyz * 2.0f - 1.0f;
			envNormal = normalize(envNormal);

		//周辺ピクセルの深度の取得
		float envDepth = tex2D(DepthBufSamp, envPos.xy).r;

		//内積の角度が大きくなるほど環境光遮蔽係数が大きくなるように計算する
		float n = dot(normal, envNormal) * .5f + .5f;

		//エッジが凸になっている部分は遮蔽されないようにする
		n += step(depth, envDepth);
		normAO += min(n, 1.0f);

		//深度値の距離が大きいほど環境光遮蔽の影響力が小さくなるようにする
		depthAO += abs(depth - envDepth) / Zfar;
	}

	float Color = normAO / (float)SAMPLES + depthAO;
	//陰を強調する
	Color = pow(Color, AOPower);

	OUT.rgb = Color;
	OUT.a = 1.0f;

	//OUT.rgba = 0.0f;
	return OUT;
}


technique ssao
{
	pass P0
	{
		AlphaBlendEnable = false;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		ZEnable = False;

		PixelShader = compile ps_3_0 PS_SSAO();
	}


}