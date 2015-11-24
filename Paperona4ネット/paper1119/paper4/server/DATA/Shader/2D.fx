texture Texture;
sampler DecaleSamp = sampler_state
{
    Texture = <Texture>;
    MinFilter = POINT;//LINEAR;
    MagFilter = POINT;//LINEAR;
    MipFilter = NONE;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

sampler DecaleSamp2 = sampler_state
{
    Texture = <Texture>;
    MinFilter = GAUSSIANQUAD;
    MagFilter = GAUSSIANQUAD;
    MipFilter = NONE;

    AddressU = CLAMP;
    AddressV = CLAMP;
};


// -------------------------------------------------------------
// 頂点シェーダからピクセルシェーダに渡すデータ
// -------------------------------------------------------------
struct VS_OUTPUT_G
{
    float4 Pos			: POSITION;
	float2 Tex			: TEXCOORD0;
    float4 Color		: COLOR0;
};

// -------------------------------------------------------------
// 頂点シェーダプログラム
// -------------------------------------------------------------
VS_OUTPUT_G VS_pass1 (
      float4 Pos    : POSITION,          // モデルの頂点
      float4 Tex    : TEXCOORD0,	         // テクスチャ座標
      float4 Color  : COLOR0
){
    VS_OUTPUT_G Out;        // 出力データ
    
    // 位置座標
    Out.Pos = Pos;
    Out.Tex = Tex;
    Out.Color = Color;
    return Out;
}

// -------------------------------------------------------------
// ピクセルシェーダプログラム
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT_G In) : COLOR
{
	return In.Color * tex2D( DecaleSamp2, In.Tex );
}

float4 PS_pass1(VS_OUTPUT_G In) : COLOR
{   
    float4	Color;
    float4 temp;

	temp = tex2D( DecaleSamp, In.Tex );
	Color = temp*8;

	//	横	14t
	temp = tex2D( DecaleSamp, In.Tex + float2( + 0.002f, 0 )) + tex2D( DecaleSamp, In.Tex + float2( - 0.002f, 0 ) );
	Color += temp*4;
	
	temp = tex2D( DecaleSamp, In.Tex + float2( + 0.006f, 0 )) + tex2D( DecaleSamp, In.Tex + float2( - 0.005f, 0 ) );
	Color += temp*2;

	temp = tex2D( DecaleSamp, In.Tex + float2( + 0.01f, 0 )) + tex2D( DecaleSamp, In.Tex + float2( - 0.008f, 0 ) );
	Color += temp;

	//	縦	14t
	temp = tex2D( DecaleSamp, In.Tex + float2( 0, + 0.002f )) + tex2D( DecaleSamp, In.Tex + float2( 0, - 0.002f ) );
	Color += temp*4;
	
	temp = tex2D( DecaleSamp, In.Tex + float2( 0, + 0.006f )) + tex2D( DecaleSamp, In.Tex + float2( 0, - 0.005f ) );
	Color += temp*2;

	temp = tex2D( DecaleSamp, In.Tex + float2( 0, + 0.01f )) + tex2D( DecaleSamp, In.Tex + float2( 0, - 0.008f ) );
	Color += temp;


	//	上横	8t	
	temp = tex2D( DecaleSamp, In.Tex + float2( + 0.002f, -0.002f )) + tex2D( DecaleSamp, In.Tex + float2( - 0.002f, -0.002f ) );
	Color += temp*4;
	
	//	下横	8t
	temp = tex2D( DecaleSamp, In.Tex + float2( + 0.002f, +0.002f )) + tex2D( DecaleSamp, In.Tex + float2( - 0.002f, +0.002f ) );
	Color += temp*4;
	
	Color /= 52;
	
    return Color;
}
// -------------------------------------------------------------
// テクニック
// -------------------------------------------------------------
technique gauss
{
    pass P0
    {
 		AlphaBlendEnable = false;
		BlendOp          = Add;
		SrcBlend         = SrcAlpha;
		DestBlend        = Zero;
		// シェーダ
        VertexShader = compile vs_3_0 VS_pass1();
        PixelShader  = compile ps_3_0 PS_pass1();
    }
}

technique copy
{
    pass P0
    {
		AlphaBlendEnable = true;
		BlendOp          = Add;
		SrcBlend         = SrcAlpha;
		DestBlend        = InvSrcAlpha;
		// シェーダ
        VertexShader = compile vs_3_0 VS_pass1();
        PixelShader  = compile ps_3_0 PS_pass2();
    }
}

technique add
{
    pass P0
    {
		AlphaBlendEnable = true;
		BlendOp          = Add;
		SrcBlend         = SrcAlpha;
		DestBlend        = one;
		// シェーダ
        VertexShader = compile vs_3_0 VS_pass1();
        PixelShader  = compile ps_3_0 PS_pass2();
    }
}


//------------------------------------------------------
//		スクリーンフィルタ
//------------------------------------------------------
float	contrast = 1.8;
float	chroma	= .9f;
float3	ScreenColor = { 1.0f, 1.9f, 1.1f };

struct VS_M
{
    float4 Pos		: POSITION;
    float4 Color	: COLOR0;
    float2 Tex		: TEXCOORD0;
};

//------------------------------------------------------
//		基本頂点シェーダー	
//------------------------------------------------------
VS_M VS_Master( VS_M In ){
	return In;
}

//------------------------------------------------------
//		ピクセルシェーダー	
//------------------------------------------------------
float4 PS_Master( VS_M In) : COLOR
{   
	float4	OUT;

	float3	col = tex2D( DecaleSamp, In.Tex );

	col = ((col-0.5f)*contrast) + 0.5f;

	float	avr = (col.r + col.g + col.b) / 3;
	col = (col-avr) * chroma + avr;

	//	ピクセル色決定
	OUT.rgb = col * ScreenColor;
	OUT.w = 1;
	
	return OUT;
}


//------------------------------------------------------
//
//------------------------------------------------------
technique Mastering
{
    pass P0
    {
		AlphaBlendEnable = false;
		BlendOp          = Add;
		SrcBlend         = one;
		DestBlend        = zero;

		VertexShader = compile vs_2_0 VS_Master();
		PixelShader  = compile ps_2_0 PS_Master();
    }
}


//------------------------------------------------------
//		パーティクル
//------------------------------------------------------

float3 LightDir = { 0.707f,0.707f,-0.707f };

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

//------------------------------------------------------
//		基本頂点シェーダー	
//------------------------------------------------------
VS_M VS_Particle( VS_M In ){
	return In;
}

//------------------------------------------------------
//		ピクセルシェーダー	
//------------------------------------------------------
float4 PS_Particle( VS_M In) : COLOR
{   
	float4	OUT;

	//	ライト計算
	float3 N = tex2D( NormalSamp, In.Tex ).xyz * 2.0f - 1.0f;
	float rate = max( 0, dot( -LightDir, N ) );
	float4	col = tex2D( DecaleSamp2, In.Tex );
	//	ピクセル色決定
	OUT = In.Color * col;
	OUT.rgb *= rate;
	
	return OUT;
}


//------------------------------------------------------
//
//------------------------------------------------------
technique Particle
{
    pass P0
    {
		AlphaBlendEnable = true;
		BlendOp          = Add;
		SrcBlend         = SrcAlpha;
		DestBlend        = InvSrcAlpha;
		ZWriteEnable     = false;

		VertexShader = compile vs_3_0 VS_Particle();
		PixelShader  = compile ps_3_0 PS_Particle();
    }
}

