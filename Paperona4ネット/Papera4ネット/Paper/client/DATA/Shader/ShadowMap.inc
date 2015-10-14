float4x4 ShadowProjection;

struct VS_SHADOW
{
	float4 Pos : POSITION;
	float4 Color : TEXCOORD0;	//í∏ì_êF
};


VS_SHADOW VS_ShadowBuf(float4 Pos : POSITION)
{
	VS_SHADOW Out;
	//ç¿ïWïœä∑
	float4x4 mat = mul(TransMatrix, ShadowProjection);
	Out.Pos = mul(Pos, mat);
	Out.Color = Out.Pos;
	return Out;
}

float4 PS_ShadowBuf(VS_SHADOW In) : COLOR0
{
	float4 Out = 0;
	Out.r = In.Color.b / In.Color.w;
	Out.g = Out.r  * Out.r ;
	Out.b = .0f;
	Out.a = .0f;
	 
	return Out;
}

technique ShadowBuf
{
	pass Pass0
	{
		AlphaBlendEnable = false;
		ZWriteEnable = true;
		CullMode = CCW;

		VertexShader = compile vs_3_0 VS_ShadowBuf();
		PixelShader = compile ps_3_0 PS_ShadowBuf();
	}
}

