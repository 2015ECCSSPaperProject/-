//********************************************************************
//																									
//		�R�c�p�V�F�[�_�[		
//
//********************************************************************
//------------------------------------------------------
//		���֘A
//------------------------------------------------------
float4x4 Projection;	//	���e�ϊ��s��
float4x4 TransMatrix;	//�@���[���h�ϊ�
float4x4 matView;		//�@�r���[�ϊ�
//------------------------------------------------------
//		�e�N�X�`���T���v���[	
//------------------------------------------------------
//WRAP...���Α����瓯���G���J��Ԃ��܂�
//CLAMP...�[�̐F���J��Ԃ��܂�
//BORDER...�����F�ɂ��܂�
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
texture NormalMap;	//	�@���}�b�v�e�N�X�`��
sampler NormalSamp = sampler_state
{
	Texture = <NormalMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};

texture SpecularMap;	//	�X�y�L�����}�b�v�e�N�X�`��
sampler SpecularSamp = sampler_state
{
	Texture = <SpecularMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};

texture HeightMap;		//	�����}�b�v�e�N�X�`��
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
//		���C�e�B���O �@
//
//**************************************************************************************************

//**************************************************************************************************
//	�������C�e�B���O
//**************************************************************************************************
float3 SkyColor = { 0.65f, 0.55f, 0.55f };
float3 GroundColor = { 0.65f, 0.55f, 0.55f };

inline float4 HemiLight(float3 normal)//�@�㉺�̊����𕪂���
{
	float4 color;
	float rate = (normal.y*0.5f) + 0.5f;
	color.rgb = SkyColor * rate;
	color.rgb += GroundColor * (1 - rate);
	color.a = 1.0f;

	return color;
}

//**************************************************************************************************
//	���s���@�@
//**************************************************************************************************
float3 LightDir = { 1.0f, -1.0f, 0.0f };		//�@���C�g�̕���
float3 DirLightColor = { 0.85f, 0.8f, 0.8f };	//�@���C�g�̐F

inline float3 DirLight(float3 dir, float3 normal)//�@���C�g�̌����Ō��̐F�̋�����Ԃ�
{
	float3 light;
	float rate = max(0.0f, dot(-dir, normal));//�@���̌v�Z�@���ςŌ��̔��˂����߂�
	light = DirLightColor * rate;//

	return light;
}

//**************************************************************************************************
//	�X�y�L�����@���_�ƕ��̂̊p�x�ɂ���Č������g���f�荞��łł���n�C���C�g�@�@
//**************************************************************************************************
float3	ViewPos = { 0.0f, -1.0f, 1.0f };	//�@�ڐ��̃|�W�V����
inline float Specular(float3 pos, float3 normal)
{
	float   sp;//�@�X�y�L�����̏�������ϐ�

	float3	H = normalize(ViewPos - pos);//
		H = normalize(H - LightDir);

	sp = dot(normal, H);
	sp = max(0, sp);
	sp = pow(sp, 10);

	return sp;
}
//------------------------------------------------------
//		���_�t�H�[�}�b�g
//------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos		: POSITION;
    float4 Color	: COLOR0;
    float2 Tex		: TEXCOORD0;

	float4 wPos			: TEXCOORD1;//�@�s�N�Z���ɑ�����Ƀ��[���h����̃|�W�V�����ǉ�
	float4 ProjectionPos: TEXCOORD2;//�@�s�N�Z���ɑ�����ɃQ�[����ʃ��X�g�̃|�W�V�����ǉ�
};

struct VS_INPUT
{
    float4 Pos    : POSITION;
    float3 Normal : NORMAL;
	float4 Color  : COLOR0;
    float2 Tex	  : TEXCOORD0;
};

//------------------------------------------------------
//		�V���h�E�֘A�̃T���v���[�ƃv���W�F�N�V����
//------------------------------------------------------
texture	Far_ShadowMap;//�������V���h�E�}�b�v 
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

texture	Near_ShadowMap;//�ߋ����V���h�E�}�b�v 
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
//ShadowBuf�ȊO�ő���v���W�F�N�V����
float4x4 Far_ShadowProjection;	//�@������
float4x4 Near_ShadowProjection;	//	�ߋ���

//------------------------------------------------------
//		�t�H�O�֘A
//------------------------------------------------------
float	FogNear = 200.0f;	//	��������t�H�O���o�����@Far�̍����L����Ɗ��炩��
float	FogFar	= 600.0f;	//	�������犮�S�Ƀt�H�O�̐F�ɂ��邩
float3	FogColor = { 0.55f, 0.65f, 0.8f };//	���̐F

//------------------------------------------------------
//		�V���h�E�֘A�̃t�@���N�V����
//------------------------------------------------------

float	AdjustValue =  -0.002f;//�@��邭���߂���ƑS���e�ɂȂ�@�����낵���̂Œ���
float	ShadowLevel = 0.5f;
//�X�N���[���ƍ���
inline float4 mulScreen(float4 Pos, float4x4 mat)
{
	float4 Tex;
	//�e�N�X�`�����W�v�Z
	float4 p;
	p = Pos;
	p = mul(p, mat);
	p.xyz /= p.w;
	//�X�N���[���ϊ�
	p.y = -p.y;
	p.xy = 0.5f * p.xy + 0.5f;
	Tex = p;
	return Tex;
}
//�e�̐F��h��
inline float GetSM(float3 Tex, sampler samp)
{
	//�V���h�E�}�b�v����[�x�����o��
	float d = tex2D(samp, Tex.xy).r;
	//�����ɗ�O����������
	if (d == 1) return 1;
	float l = (d < Tex.z + AdjustValue) ? ShadowLevel : 1;	//������
	return l;
}
//�V�O���C�h
inline float sigmoid(float gain, float x, float adjust)
{
	//
	return 1.0 / (1.0 + exp(-gain * (x - adjust)));
}


//------------------------------------------------------
//	���}�b�v�p�@����̕��i���f�肱�ރ}�b�v
//------------------------------------------------------
texture EnvMap;		//	���e�N�X�`��
sampler EnvSamp = sampler_state
{
	Texture = <EnvMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = Wrap;
	AddressV = Wrap;
};
//�@EnvSamp�@�͂�����ʂ��Ďg�p���@�@
inline float4 Environment(float3 normal)
{
	float4	color;

	float2	uv;
	uv.x = normal.x*0.5f + 0.5f;
	uv.y = normal.y*0.5f + 0.5f;

	color = tex2D(EnvSamp, uv);
	return color;
}

float	EnvParam = 0.3f;	//	�f�肱�݂̊���

#include	"DATA\\Shader\\ShadowMap.inc"

//********************************************************************
//
//		��{�R�c�V�F�[�_�[		
//
//********************************************************************
//------------------------------------------------------
//		���_�V�F�[�_�[	
//------------------------------------------------------
VS_OUTPUT VS_Basic( VS_INPUT In )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
	//TransMatrix��Pos����������wPos�̏�񐶐�
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
//		�s�N�Z���V�F�[�_�[	
//------------------------------------------------------
float4 PS_Basic( VS_OUTPUT In) : COLOR
{   
	float4	OUT;
	//	�s�N�Z���F����
	OUT = In.Color * tex2D( DecaleSamp, In.Tex );

	return OUT;
}

//------------------------------------------------------
//		�e�`��e�N�j�b�N
//------------------------------------------------------
float4 PS_Shadow(VS_OUTPUT In) : COLOR
{
	float4	OUT;
	//	�s�N�Z���F����
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);

	//�V���h�E
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 11.0));//���̐��l�Œ��x�����ꏊ��T��
	OUT.rgb *= s;

	return OUT;
}
//------------------------------------------------------
//		�ʏ�`��e�N�j�b�N
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
// ��Z
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

// �V���h�E�ǉ�
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
//		�t�u�A�j���[�V�����p 
//
//**************************************************************************************************

float uvMove;//�@�A�j���[�V����

VS_OUTPUT VS_UvAni( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	Out.Pos   = mul(In.Pos, Projection);
	Out.Color = In.Color;
	Out.Tex   = In.Tex + float2(uvMove, .0f);// ���W

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
//		���e�e�N�X�`�� �@�i���ʁj
//
//**************************************************************************************************

texture TEXMap;		//	�����e�}�b�v
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
// Name : ����
// Desc : ���_�V�F�[�_
//-----------------------------------------------------------------------------------------------------
TEX_Out VS_TEX(TEX_In input)
{
	TEX_Out output = (TEX_Out)0;

	//�@���W�ϊ�
	output.Position = mul(input.Position, Projection);//�@�t�Ł~�ƂƂ�ł��Ȃ����ƂɂȂ�̂Œ���!

	//�@�ˉe�e�N�X�`���p
	float3 wvpNormal = normalize(mul(input.Normal, (float3x3)Projection));
		output.TexCoord = (output.Position.xy + wvpNormal.xy * NormalScale) / output.Position.z;
	output.TexCoord = output.TexCoord * float2(0.5f, -0.5f) + 0.5f;

	//�@���t���N�g���t���N�g�}�b�v
	//output.TexCoord = (input.Position.xy / input.Position.w)*0.5f + 0.5f;
	//output.TexCoord = -output.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------------------
// Name : ����
// Desc : �s�N�Z���V�F�[�_
//-----------------------------------------------------------------------------------------------------
float4 PS_TEX(TEX_Out input) : COLOR
{
	float4 output;

	//�@�e�N�X�`���J���[
	output = tex2D(TEXSamp, input.TexCoord);
	output.a = 1.0f;

	return output;
}

//------------------------------------------------------
//		����TEX�e�N�j�b�N
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
//		���˃}�b�v�쐬
//
//********************************************************************

//------------------------------------------------------
//	���˃}�b�v
//------------------------------------------------------
texture RefMap;		//	���˃}�b�v�e�N�X�`��
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
//		���_�t�H�[�}�b�g
//------------------------------------------------------
struct VS_OUTPUT_REFLECT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float4 wPos		: TEXCOORD1;	//	���s�N�Z���ɑ�����Ƀ��[���h����̃|�W�V�����ǉ�
	float4 ProjectionPos: TEXCOORD2;//�@���s�N�Z���ɑ�����ɃQ�[����ʃ��X�g�̃|�W�V�����ǉ�
};

struct VS_INPUT_REFLECT
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
};


float m_Height = 0.0f;

//------------------------------------------------------
//	���_�V�F�[�_
//------------------------------------------------------
VS_OUTPUT_REFLECT VS_Reflect(VS_INPUT_REFLECT In)
{
	VS_OUTPUT_REFLECT Out = (VS_OUTPUT_REFLECT)0;

	In.Pos.y = -In.Pos.y;

	//TransMatrix��Pos����������wPos�̏�񐶐�
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
//		�s�N�Z���V�F�[�_�[	
//------------------------------------------------------

float4 PS_Reflect(VS_OUTPUT_REFLECT In) : COLOR
{
	//���˃C���[�W�̃��[���h���W�`��ł̒��_�� Y ���W��m_Height�ϐ���荂���Ƃ��`�悵�Ȃ��悤�ɂ���
	//����ɂ�萅�ʉ��̃C���[�W�̂݃����_�����O����
	clip(m_Height - In.wPos.y);//�w�肳�ꂽ�l�� 0 ��菬�����ꍇ�ɁA���݂̃s�N�Z����j�����܂��B
	float4	OUT;
	//	�s�N�Z���F����
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);

	return OUT;
}

//------------------------------------------------------
//		�e�N�j�b�N	
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
//		FULL�G�t�F�N�g
//
//**************************************************************************************************

struct VS_FULL_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;
	// ���C�e�B���O
	float4 Light	: COLOR1;
	float4 Ambient	: COLOR2;
	float3 Normal	: COLOR3;
	float3 vLight	: TEXCOORD1;
	float3 vE		: TEXCOORD2;
	// �s��
	float4 wPos		: TEXCOORD3;//		���s�N�Z���ɑ�����Ƀ��[���h����̃|�W�V�����ǉ�
	float4 ProjectionPos: TEXCOORD4;//	���s�N�Z���ɑ�����ɃQ�[����ʃ��X�g�̃|�W�V�����ǉ�
	float3 worldNormal : TEXCOORD5;		//	���[���h�̖@��
};


//------------------------------------------------------
//	���_�V�F�[�_
//------------------------------------------------------
VS_FULL_OUTPUT VS_Full(VS_INPUT In)
{
	VS_FULL_OUTPUT Out = (VS_FULL_OUTPUT)0;

	Out.Pos = mul(In.Pos, Projection);//	�v���W�F�N�V�����s��ƍ���
	Out.ProjectionPos = Out.Pos;
	Out.Color.a = 1;
	Out.Tex = In.Tex;//	�e�N�X�`���𑗂�

	float4 P = mul(In.Pos, TransMatrix);//	���[���h���W�Ɏ����Ă���
	Out.wPos = mul(In.Pos, TransMatrix);//
	float3x3	mat = TransMatrix;//		float4x4��TransMatrix�̍s����@float3x3��mat��
	Out.Normal = mul(In.Normal, mat);
	Out.Normal = normalize(Out.Normal);

	// �������C�e�B���O
	Out.Ambient.rgb = HemiLight(Out.Normal);

	// ���_���[�J�����W�n�Z�o
	float3	vx;
	float3	vy = { 0, 1, 0.001f };
	vx = cross(vy, Out.Normal);
	vx = normalize(vx);
	vy = cross(vx, Out.Normal);
	vy = normalize(vy);

	//	���C�g�x�N�g���␳ ���C�g�������������̊p�x
	Out.vLight.x = dot(vx, LightDir);
	Out.vLight.y = dot(vy, LightDir);
	Out.vLight.z = dot(Out.Normal, LightDir);

	// �����x�N�g���␳
	float3 E = P - ViewPos;   // �����x�N�g��
	Out.vE.x = dot(vx, E);
	Out.vE.y = dot(vy, E);
	Out.vE.z = dot(Out.Normal, E);

	//	�t�H�O�v�Z
	Out.Ambient.a = (FogFar - Out.Pos.z) / (FogFar - FogNear);
	Out.Ambient.a = saturate(Out.Ambient.a);//�w�肳�ꂽ�l�� 0 �` 1 �͈̔͂ɃN�����v���܂�

	//	�O���[�o���|�W�V����&�@��
	float3x3   m = (float3x3)TransMatrix;
	Out.worldNormal = mul(In.Normal, m);

	return Out;
}


//------------------------------------------------------
//	�s�N�Z���V�F�[�_
//------------------------------------------------------
float4 PS_Full(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

	//	����
	//float h = tex2D(HeightSamp, Tex).r - 0.5f;
	//�@�ڐ�
	float3 E = normalize(In.vE);
	//Tex.x += 0.04f * h * E.x;
	//Tex.y -= 0.04f * h * E.y;


	//	�@���擾
	float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
	//	���C�g�v�Z
	In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, N);

	//
	In.Color.rgb = light + In.Ambient;//Ambient=�����{

	//	�s�N�Z���F����
	OUT = In.Color * tex2D(DecaleSamp, Tex);

	//	�X�y�L�����}�b�v�擾
	float4 sp_tex = tex2D(SpecularSamp, Tex);
	//	�������˃x�N�g��
	float3 R = reflect(-E, N);
	//	�X�y�L����
	OUT.rgb += pow(max(0, dot(R, In.vLight)), 30) * (sp_tex);

	//�@�������C�e�B���O �ۂ��J�[�r�B�Ȃǂɂ悭����
	//float3 Rim = RimColor * Kr * pow(1 - dot(N, -E), Power);
	//OUT.rgb += Rim;

	//�V���h�E
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 12.0));//���̐��l�Œ��x�����ꏊ��T��
	//s = lerp(s, 1, sigmoid(2.0, (depth / 3.0), 33.0));

	OUT.rgb *= s;

	//	�t�H�O�̗p
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


//����
float Kr = 0.5f;//�グ��ƐF������
float Power = 2.5f;//�i�鐔�l
float4 RimColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

//------------------------------------------------------
//	�s�N�Z���V�F�[�_
//------------------------------------------------------
float4 PS_Full_Teki(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

		//	����
		//float h = tex2D(HeightSamp, Tex).r - 0.5f;
		//�@�ڐ�
		float3 E = normalize(In.vE);

		//	�@���擾
		float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
		//	���C�g�v�Z
		In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, N);

		//
		In.Color.rgb = light + In.Ambient;//Ambient=�����{

	//	�s�N�Z���F����
	OUT = In.Color * tex2D(DecaleSamp, Tex);

	//	�X�y�L�����}�b�v�擾
	float4 sp_tex = tex2D(SpecularSamp, Tex);
		//	�������˃x�N�g��
		float3 R = reflect(-E, N);
		//	�X�y�L����
		OUT.rgb += pow(max(0, dot(R, In.vLight)), 30) * (sp_tex);

	//�@�������C�e�B���O �ۂ��J�[�r�B�Ȃǂɂ悭����
	float3 Rim = RimColor * Kr * pow(1 - dot(N, -E), Power);
		OUT.rgb += Rim;

	//�V���h�E
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 12.0));//���̐��l�Œ��x�����ꏊ��T��
	OUT.rgb *= s;

	//	�t�H�O�̗p
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

/*���}�b�v�l��*/

//	�s�N�Z���V�F�[�_
//------------------------------------------------------
float4 PS_Env(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

		//�@�ڐ�
		float3 E = normalize(In.vE);

		//	�@���擾
		float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
		//	���C�g�v�Z
		In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, N);

		//
		In.Color.rgb = light + In.Ambient;//Ambient=�����{

	//	�s�N�Z���F����
	OUT = In.Color * tex2D(DecaleSamp, Tex);

	float4 Env = Environment(In.worldNormal);
		//	���}�b�v�u�����h
		OUT = (OUT * (1 - EnvParam)) + (Env * EnvParam);

	//	�X�y�L�����}�b�v�擾
	float4 sp_tex = tex2D(SpecularSamp, Tex);
		//	�������˃x�N�g��
		float3 R = reflect(-E, N);
		//	�X�y�L����
		OUT.rgb += pow(max(0, dot(R, In.vLight)), 25) * (sp_tex);

	//�@�������C�e�B���O �ۂ��J�[�r�B�Ȃǂɂ悭����
	float3 Rim = RimColor * (Kr+0.3f) * pow(1 - dot(N, -E), Power);
	OUT.rgb += Rim;

	//�V���h�E
	float4 shadow;
	float depth = In.ProjectionPos.z;
	float near_ShadowLevel;
	float far_ShadowLevel;
	shadow = mulScreen(In.wPos, Near_ShadowProjection);
	near_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Near_ShadowSamp));
	shadow = mulScreen(In.wPos, Far_ShadowProjection);
	far_ShadowLevel = max(ShadowLevel, GetSM(shadow.xyz, Far_ShadowSamp));
	float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(30.0, (depth / 3.0), 12.0));//���̐��l�Œ��x�����ꏊ��T��
	//s = lerp(s, 1, sigmoid(2.0, (depth / 3.0), 33.0));
	OUT.rgb *= s;

	//	�t�H�O�̗p
	//OUT.rgb = (OUT.rgb * In.Ambient.a) + (FogColor * (1 - In.Ambient.a));



	return OUT;
}


//------------------------------------------------------
//		���}�b�s���O
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
//		�g�D�[�� 
//
//**************************************************************************************************

//-----------------------------------------------
//		�g�D�[���p���_
//-----------------------------------------------
struct VS_OUTPUT_TOON
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float  Level : TEXCOORD1;
};

float OutlineSize = 0.1f;	//�֊s���̑���

//�e�p���
float ToonLevel = 0.5f;		//�e�̃��x��
float ToonShadow = 0.4f;	//�e�̔Z��


/*		�A�E�g���C���p���_�V�F�[�_�[		*/
VS_OUTPUT VS_Outline(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;

	//�@�������Ɋg��
	In.Normal = normalize(In.Normal);
	In.Pos.xyz = In.Pos.xyz + (OutlineSize * In.Normal);

	//���e�ϊ�
	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color.rgb = 0;
	Out.Color.a = 1;

	return Out;
}
/*		�֊s��(�g�D�[��)�p�s�N�Z���V�F�[�_�[		*/
float4 PS_Outline(VS_OUTPUT In) : COLOR
{
	float4 OUT;

	//�s�N�Z���F����
	OUT = In.Color;

	return OUT;
}


/*		�g�D�[���e�V�F�[�_�[		*/
VS_OUTPUT_TOON VS_Toon(VS_INPUT In)
{
	VS_OUTPUT_TOON Out = (VS_OUTPUT_TOON)0;

	//���e�ϊ�
	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color = 1;

	//�@���ϊ�
	float3x3 mat = TransMatrix;
		float3 N = mul(In.Normal, mat);
		N = normalize(N);

	//���C�g�v�Z
	float3 vec = normalize(LightDir);
		Out.Level = dot(-vec, N);

	return Out;
}

/*		�g�D�[���e�V�F�[�_�[		*/
float4 PS_Toon(VS_OUTPUT_TOON In) : COLOR
{
	float4 OUT;

	//�s�N�Z���F����
	OUT = tex2D(DecaleSamp, In.Tex);

	//���x���������e�ɂ���
	if (In.Level < ToonLevel) OUT.rgb *= ToonShadow;

	return OUT;
}


technique toon
{
	//�֊s��
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

// Beckman���z
float BechmannDistribution(float d, float m) {
	float d2 = d * d;
	float m2 = m * m;
	return exp((d2 - 1.0) / (d2 * m2)) / (m2 * d2 * d2);
}

// �t���l����
float Fresnel(float c, float f0) {
	float sf = sqrt(f0);
	float n = (1.0 + sf) / (1.0 - sf);
	float g = sqrt(n * n + c * c - 1.0);
	float ga = (c * (g + c) - 1.0) * (c * (g + c) - 1.0);
	float gb = (c * (g - c) + 1.0) * (c * (g - c) + 1.0);
	return (g - c) * (g - c) / (2.0 * (g + c) + (g + c)) * (1.0 + ga / gb);
}

//------------------------------------------------------
//	�s�N�Z���V�F�[�_
//------------------------------------------------------
float4 PS_CookTorrance(VS_FULL_OUTPUT In) : COLOR
{
	float4	OUT;
	float2 Tex = In.Tex;

	//�@�ڐ�
	float3 E = -normalize(In.vE);//�@�ڐ��͔��΂ɂ�����
	//	�@���擾
	float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
	//	���C�g�v�Z
	In.vLight = normalize(In.vLight);

	float3 L = -In.vLight;

	float3 H = normalize(L + E);

	float NV = dot(N, E);
	float NH = dot(N, H);
	float VH = dot(E, H);
	float NL = dot(N, L);
	float LH = dot(L, H);

	// Beckmann ���z�֐�
	const float m = 0.35f;//�i�鐔�l
	float NH2 = NH*NH;
	float m2 = m * m;
	float D = exp(-(1.0f - NH2) / (NH2*m2)) / (4*m2*NH2*NH2);//�S*�͍i�鐔�l

	// �􉽌�����
	float G = min(1, min(2 * NH*NV / VH, 2 * NH*NL / VH));

	// �t���l��
	float n = 20.0f;
	float g = sqrt(n*n + LH*LH - 1);
	float gpc = g + LH;
	float gnc = g - LH;
	float cgpc = LH*gpc - 1;
	float cgnc = LH*gnc + 1;
	float F = 0.5f*gnc*gnc*(1 + cgpc*cgpc / (cgnc*cgnc)) / (gpc*gpc);

	// �����̐F
	float4 ks = { 2.0f*0.486f, 2.0f*0.433f, 2.0f*0.185f, 1.0f };

	//���C�e�B���O
	float3 light = DirLight(In.vLight, N);
	In.Color.rgb = light * float3(0.7f, 0.5f, 0.1f);
	In.Color.rgb += float3(0.5f, 0.3f, 0.0f);

	//	�s�N�Z���F����
	//OUT = In.Color * tex2D(DecaleSamp, Tex);
	
	return In.Color + ks * max(0, F*D*G / NV);//�����ŏI��

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
//		�C
//
//**************************************************************************************************

//------------------------------------------------------
//		���_�t�H�[�}�b�g
//------------------------------------------------------
struct VS_OUTPUT_SEA
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex1		: TEXCOORD0;
	float2 Tex2		: TEXCOORD1;

	//�s��
	float4 wPos		: TEXCOORD2;//	�s�N�Z���ɑ�����Ƀ��[���h����̃|�W�V�����ǉ�
	float4 RocalPos	: TEXCOORD3;//	�s�N�Z���ɑ�����ɃQ�[����ʃ��X�g�̃|�W�V�����ǉ�

	//���C�e�B���O
	//float4 Light	: COLOR1;
	float4 Ambient	: COLOR2;
	float3 Normal : COLOR3;
	float3 vLight	: TEXCOORD4;
	float3 vE		: TEXCOORD5;

	//�ǉ��@���W�n
	float4 ProjectionPos : TEXCOORD6;
};


struct VS_INPUT_SEA
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
};

////------------------------------------------------------
////		�t�H�O�֘A
////------------------------------------------------------
//float	AlphaNear = 10.0f;
//float	AlphaFar = 200.0f;
//float	SeaAlpha = 0.0f;//

float uvSea=0.0f;
float3 SeaColor = { 0.2f, 0.8f, 1.7f };


VS_OUTPUT_SEA VS_Sea(VS_INPUT_SEA In)
{
	VS_OUTPUT_SEA Out = (VS_OUTPUT_SEA)0;

	//���[�J�����W�n��ł̒��_�̍��W
	Out.RocalPos = In.Pos;

	Out.Pos = mul(In.Pos, Projection);//�v���W�F�N�V�����s��ƍ���
	Out.ProjectionPos = Out.Pos;

	Out.Color = 1.0f;//In.Color;
	Out.Tex1 = In.Tex + float2(-uvSea*1.5, -uvSea * 1.5);
	Out.Tex2 = In.Tex + float2(uvSea, uvSea*1.5);


	float4 P = mul(In.Pos, TransMatrix);//���[���h���W�Ɏ����Ă���
	Out.wPos = mul(In.Pos, TransMatrix);//
	float3x3	mat = TransMatrix;//float4x4��TransMatrix�̍s����@float3x3��mat��
	//�@���x�N�g��
	Out.Normal = mul(In.Normal, mat);//
	Out.Normal = normalize(Out.Normal);//

	// �������C�e�B���O
	Out.Ambient.rgb = HemiLight(Out.Normal);

	// ���_���[�J�����W�n�Z�o
	float3	vx;
	float3	vy = { 0, 1, 0.001f };
	vx = cross(vy, Out.Normal);
	vx = normalize(vx);
	vy = cross(vx, Out.Normal);
	vy = normalize(vy);

	//	���C�g�x�N�g���␳ ���C�g�������������̊p�x
	Out.vLight.x = dot(vx, LightDir);			// ���C�g�x�N�g���𒸓_���W�n�ɕϊ�����
	Out.vLight.y = dot(vy, LightDir);			// ���C�g�x�N�g���𒸓_���W�n�ɕϊ�����
	Out.vLight.z = dot(Out.Normal, LightDir);	// ���C�g�x�N�g���𒸓_���W�n�ɕϊ�����
	Out.vLight = normalize(Out.vLight);

	// �����x�N�g���␳
	float3 E = P - ViewPos;			 // �����x�N�g��
	Out.vE.x = dot(vx, E);			 // �����x�N�g���𒸓_���W�n�ɕϊ�����
	Out.vE.y = dot(vy, E);			 // �����x�N�g���𒸓_���W�n�ɕϊ�����
	Out.vE.z = dot(Out.Normal, E);   // �����x�N�g���𒸓_���W�n�ɕϊ�����
	Out.vE = normalize(Out.vE);//�ڐ�

	////	�t�H�O�v�Z
	//Out.Ambient.a = (AlphaFar - Out.Pos.z) / (AlphaFar - AlphaNear);
	//Out.Ambient.a = saturate(Out.Ambient.a);//�w�肳�ꂽ�l�� 0 �` 1 �͈̔͂ɃN�����v���܂�

	return Out;
}

float4 PS_Sea(VS_OUTPUT_SEA In) : COLOR
{
	float4	OUT;
	//UV�Q�쐬
	float2 UV1 = In.Tex1;
	float2 UV2 = In.Tex2;

	//**********************************************************
	// �����}�b�s���O
	//**********************************************************  
	// �����}�b�v���Q�Ƃ��A�������擾����
	float h = tex2D(HeightSamp, UV1).r - 0.5f;
	h += tex2D(HeightSamp, UV2).r - 0.5f;
	h *= 0.5;//�@��T���v�����O���Ă邩�畽�ς�����Ă���

	// �e�N�Z���𒸓_���W�n�ł̎����x�N�g�������ɏd�݂����Ă��炷�B
	float3 E = normalize(In.vE);//�ڐ�
		UV2 -= 0.02f * h * E.xy + uvSea;
	UV1 -= 0.02f * h * E.xy + uvSea;//���ƂŐ��l�グ��

	/*************************/
	//	�����K�p��� �@���擾
	/*************************/
	//�@���}�b�v���Q�Ƃ��A�@�����擾����
	float3 NMap = (tex2D(NormalSamp, UV1).rgb - 0.5f)*2.0f;
	NMap += (tex2D(NormalSamp, UV2).rgb - 0.5f)*2.0f;
	NMap *= 0.5f;

	////	���C�g�v�Z�͂��Ȃ��I�@���͂�������
	In.vLight = normalize(In.vLight);
	float3 light = DirLight(In.vLight, NMap);
	//light *= 0.5f;

	//In.Color.rgb = light;//Ambient=�����{
	////	�s�N�Z���F����
	//OUT *= In.Color;

	OUT = tex2D(DecaleSamp, UV1) + tex2D(DecaleSamp, UV2);
	OUT *= 0.5;//�@��T���v�����O���Ă邩�畽�ς�����Ă���
	OUT.rgb *= SeaColor + light;// �C�̐F�w��
	OUT.a = .8f;

	/*************************/
	//	�X�y�L�����}�b�v�擾
	/*************************/
	float4 sp_tex = tex2D(SpecularSamp, UV1);
	sp_tex += tex2D(SpecularSamp, UV2);
	sp_tex *= 0.5;//�@��T���v�����O���Ă邩�畽�ς�����Ă��Ă�

	//	�������˃x�N�g��
	float3 R = reflect(-E, NMap);
	//	�X�y�L����
	float3 S;
	S = pow(max(0, dot(R, In.vLight)), 80) * (sp_tex);
	
	//************************************************************
	//�@����
	//************************************************************
	//�䂪�ݗʂ̌v�Z
	//���̌��݂������Ȃ�قǋ��܂��Ȃ��悤�ɂ���
	//float4 Offset = float4((Nmap - E).xy * 20, 0.0f, 0.0f);
	//�@�ˉe�e�N�X�`���p
	//float3 wvpNormal = normalize(mul(Nmap, (float3x3)Projection));
	//float2 TexCoord;
	//TexCoord = (In.Pos.xy + wvpNormal.xy * 1) / In.Pos.z;
	//TexCoord = TexCoord * float2(0.5f, -0.5f) + 0.5f;

	//�C��̐F
	//float3 SeabedCol;
	//SeabedCol = tex2Dproj(TEXSamp, TexCoord );
	//OUT.rgb *= SeabedCol;

	//	���}�b�v�@R����ꔽ�˃}�b�v��
	float3 env = Environment(R) * (0.8f - sp_tex.a);
		OUT.rgb = OUT.rgb * env + OUT.rgb;
	//*********************************************************************************************************
	//�t���l������
	//*********************************************************************************************************
	//���_ �� ���_�x�N�g�� �� �@���x�N�g���̓��ς��v�Z
	//float3 pos = In.Pos;
	//float fresnel = dot(normalize(-E - In.RocalPos), normalize(In.Normal));

	//////���ς̌��ʂ� 0.0f �ɋ߂��قǔ��˃}�b�v�̐F�������Ȃ�A 1.0f �ɋ߂��قǊC��}�b�v�̐F�������Ȃ�悤�ɐ��`��������
	//float4 Col;
	//float3 ReflectCol = float3(0.8, 0.5, 0.8);
	//float3 SeabedCol = float3(0.0, 0.9, 0.2);
	//
	//Col.rgb = lerp(ReflectCol, SeabedCol, fresnel);
	////Col.rgb = Col.rgb * Diffuse * WaterCol + S;
	//OUT.rgb = Col.rgb;

	//*********************************************************************************************************
	//�����x
	//*********************************************************************************************************  
	//���̌��݂��v�Z����
	//float Z = SeabedZ - In.PosWVP.z / m_ZF;
	//�y�l���������Ȃ�قǓ����x����������i���ӂP�j
	//Z = min(Z * 20.0f, 1.0f);
	//Col.rgb = lerp(SeabedCol, Col.rgb, Z);

	OUT.rgb += S;
	
	//�@�ˉe�e�N�X�`���p
	//float3 wvpNormal = normalize(mul(In.Normal, (float3x3)Projection));
	//	ref = (In.wPos.xy + wvpNormal.xy * 1) / In.wPos.z;
	//output.TexCoord = output.TexCoord * float2(0.5f, -0.5f) + 0.5f;

	//���t���N�g�}�b�v
	float2 ref = (In.ProjectionPos.xy / In.ProjectionPos.w)*0.5f + 0.5f;
		ref.y = -ref.y;
	float3 Env = tex2D(RefSamp, ref-0.2f * h * E.xy/*��+�ڐ�*/) * (OUT.rgb);
		Env *= 0.35f;
		OUT.rgb += Env;// lerp(Env, OUT.rgb, 0.3f);
	
	//	�t�H�O�̗p
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

