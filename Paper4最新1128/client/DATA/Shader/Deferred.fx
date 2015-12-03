//**************************************************************************************************
///																									
///		��{�V�F�[�_�[		
///
//**************************************************************************************************

//------------------------------------------------------
///		���֘A
//------------------------------------------------------
float4x4 TransMatrix;	//	���[���h�ϊ��s��
float4x4 matView;		//	�r���[�ϊ�
float4x4 Projection;	//	���e�ϊ��s�� WVP 

float3	ViewPos;		//  ���[���h���W�̖ڂ̃|�W�V����
float4x4 InvProj;		//  Projection->View�@�֖߂��t�s��
float4x4 matProjection;	//  �r���[->�v���W�F�N�V�����ϊ��s��

//------------------------------------------------------
///		�e�N�X�`���T���v���[	
//------------------------------------------------------
///WRAP...���Α����瓯���G���J��Ԃ��܂�
///CLAMP...�[�̐F���J��Ԃ��܂�
///BORDER...�����F�ɂ��܂�
texture Texture;	//	�f�J�[���e�N�X�`��
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
//		���_�t�H�[�}�b�g
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
	float3 Normal			:TEXCOORD1;			//�@��
	float4 Depth			:TEXCOORD2;			//�[�x���
	float3 T				:TEXCOORD3;			//�^���W�F���g(�ڐ�)
	float3 B				:TEXCOORD4;			//Binormal(�]�@��)
	float3 vE				:TEXCOORD5;			//�����x�N�g��
};

// �l�̏������̏��Ԓʂ�ɕ`��
struct PS_DEFERRED
{
	float4 Diffuse			:COLOR0;	// �e�N�X�`���[
	float4 Normal			:COLOR1;	// �@�����
	float4 Specular			:COLOR2;	// �X�y�L����
	float4 Depth			:COLOR3;	// ���s��
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

	//�[�x
	Out.Depth = Out.Pos;

	//�@�����̏C��
	float3x3	mat = (float3x3)Projection;					//�X�N���[�����W�Ȃ̂�Projection���|�����킹��
	//float3x3 mat = (float3x3)mul(TransMatrix, matView); // �������ł���OUT
	float3 N = mul(In.Normal, mat);
	N = normalize(N);
	Out.Normal = N;

	//	���_�X�N���[�����W�n�Z�o
	float3	vx;
	float3	vy = { .0f, 1.0f, -0.001f };			//����y�����x�N�g��

	vx = cross(N, vy);
	vx = normalize(vx);
	Out.T = vx;

	vy = cross(N, vx);
	vy = normalize(vy);
	Out.B = vy;


	//�����x�N�g��
	float3 P = mul((float3)In.Pos, (float3x3)TransMatrix);
	float3 E = (P - ViewPos);		//�����x�N�g��
	//vx = float3(1.0f,.0f,0.001f);		//����x�����x�N�g��

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

	//�f�B�q���[�Y
	//OUT.Diffuse = In.Color * tex2D( DecaleSamp,Tex );
	OUT.Diffuse = tex2D(DecaleSamp, Tex);

	//�X�y�L����
	OUT.Specular = tex2D(SpecularSamp, Tex);
	OUT.Specular.a = 1.0f;
	//OUT.Position.a = 1.0f;

	//�[�x r32bit
	OUT.Depth = In.Depth.z / In.Depth.w;			//w�Ŋ�����-1~1�ɕϊ�����

	//�@�� 	���_��ԁ��J�����ϊ��s��
	float3x3 ts;								//�@�����狁�߂���
	ts[0] = normalize(In.T);					//�ڐ�
	ts[1] = normalize(In.B);					//�]�@��
	ts[2] = normalize(In.Normal);				//�@��


	float3 N = tex2D(NormalSamp, Tex).xyz * 2.0f - 1.0f;
	float3 normal = normalize(mul(N, ts));
	normal = normal * 0.5f + 0.5f;
	OUT.Normal.rgb = normal;
	OUT.Normal.a = tex2D(HeightSamp, Tex).r;//AO�}�b�v��@���̃A���t�@��
	
	return OUT;
}
/*�u�����f�B���O �I�v�V����*/
/*
BLEND_ZERO			�\�[�X�̐F�͍� (0, 0, 0, 0) �ł��B�u�����f�B���O�O�̏����͂���܂���B
BLEND_ONE			�\�[�X�̐F�͔� (1, 1, 1, 1) �ł��B�u�����f�B���O�O�̏����͂���܂���B
BLEND_SRC_COLOR		�s�N�Z���V�F�[�_�[����̃J���[ �f�[�^ (RGB) �ł��B�u�����f�B���O�O�̏����͂���܂���B
BLEND_INV_SRC_COLOR �s�N�Z���V�F�[�_�[����̃J���[ �f�[�^ (RGB) �ł��B�u�����f�B���O�O�̏����ɂ���ăf�[�^�����]����A1 - RGB ����������܂��B
BLEND_SRC_ALPHA		�s�N�Z���V�F�[�_�[����̃A���t�@ �f�[�^ (A) �ł��B�u�����f�B���O�O�̏����͂���܂���B
BLEND_INV_SRC_ALPHA �s�N�Z���V�F�[�_�[����̃A���t�@ �f�[�^ (A) �ł��B�u�����f�B���O�O�̏����ɂ���ăf�[�^�����]����A1 - A ����������܂��B
BLEND_DEST_ALPHA	�����_�[�^�[�Q�b�g����̃A���t�@ �f�[�^�ł��B�u�����f�B���O�O�̏����͂���܂���B
*/

technique G_Buffer
{
	pass P0
	{
		// �����_�����O�X�e�[�g
		AlphaBlendEnable = true;
		BlendOp = Add;
		//SrcBlend = SrcAlpha;		// �ŏ���RGB�f�[�^�\�[�X���w�肵�܂�
		//DestBlend = InvSrcAlpha;	// 2�Ԗڂ�RGB�f�[�^�\�[�X���w�肵�܂�
		SrcBlend = One;		//1,1,1,1
		DestBlend = Zero;	//0,0,0,0
		CullMode = CCW;
		ZEnable = true;
		ZWriteEnable = true;
		//SeparateAlphaBlendEnable = true;// �_�C���N�g3D�̕ʁX�̃A���t�@�E�u�����h�E���[�h���C�l�[�u�����܂�

		VertexShader = compile vs_3_0 VS_G_Buffer();
		PixelShader = compile ps_3_0 PS_G_Buffer();
	}
}

/* deferred�Ń��ɏ��������������@ */
/*
	���ʂɕ`���Ɓ@�������I�u�W�F�N�g���d�Ȃ荇����񂪐��m�ɂłȂ�
	�������@��
	SrcBlend = One;	
	DestBlend = Zero
	ZEnable = true;
	�ɐݒ肷��

	���ǂ��������Ƃ��N���邩
	�\�[�X�u�����h�́@�������Ă�����̂��ǂꂭ�炢������邩�̒l
	����� One(1,1,1,1)�ɂ��邱�ƂőS�͂ŏ�ɂ��Ԃ���p�ɐݒ�

	�f�v�X�u�����h�́@��ɕ`���Ă������̂��ǂ����߂邩
	������@Zero(0,0,0,0)�ɂ��邱�Ƃň�ؕ`���Ȃ�

	���ꂾ�Ɓ@��낪��Ό����鏈���ɂȂ��Ă����܂�����
	�y�l���l������΂����������������Ȃ��ł���

*/

//*****************************************
//		G_Water
//*****************************************

//-----------------------------------------
//		�O���[�o���ϐ�
//-----------------------------------------
float uvSea = 0.0f;
float3 SeaColor = { 0.2f, 0.8f, 1.5f };

//-----------------------------------------
//		�ŏ��ɒ��_����Ⴄ���
//-----------------------------------------
struct VS_INPUT_SEA
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float2 Tex	  : TEXCOORD0;
};

//-----------------------------------------
//		pixel�ɑ�����
//-----------------------------------------
struct VS_OUTPUT_SEA
{
	float4 Pos				:POSITION;
	float2 Tex1				:TEXCOORD0;
	float2 Tex2				:TEXCOORD1;

	float3 Normal			:TEXCOORD2;			//�@��
	float4 Depth			:TEXCOORD3;			//�[�x���

	float3 T				:TEXCOORD4;			//�^���W�F���g(�ڐ�)
	float3 B				:TEXCOORD5;			//Binormal(�]�@��)
	float3 vE				:TEXCOORD6;			//�����x�N�g��
};


VS_OUTPUT_SEA VS_Sea(VS_INPUT_SEA In)
{
	VS_OUTPUT_SEA Out = (VS_OUTPUT_SEA)0;

	Out.Pos = mul(In.Pos, Projection);

	//2��UV�����
	Out.Tex1 = In.Tex + float2(-uvSea*1.5, -uvSea * 1.5);
	Out.Tex2 = In.Tex + float2(uvSea, uvSea*1.5);

	//�[�x
	Out.Depth = Out.Pos;


	//�@�����̏C��
	float3x3	mat = (float3x3)Projection;					//�X�N���[�����W�Ȃ̂�Projection���|�����킹��
	float3 N = mul(In.Normal, mat);
	N = normalize(N);
	Out.Normal = N;

	//	���_�X�N���[�����W�n�Z�o
	float3	vx;
	float3	vy = { .0f, 1.0f, -0.001f };			//����y�����x�N�g��
	vx = cross(N, vy);
	vx = normalize(vx);
	Out.T = vx;// X�擾
	vy = cross(N, vx);
	vy = normalize(vy);
	Out.B = vy;// Y�擾

	//�����x�N�g��
	float3 P = mul((float3)In.Pos, (float3x3)TransMatrix);
	float3 E = (P - ViewPos);		//�����x�N�g��
	Out.vE.x = -dot(vx, E);
	Out.vE.y = dot(vy, E);
	Out.vE.z = dot(N, E);
	Out.vE = normalize(Out.vE);

	return Out;
}


PS_DEFERRED PS_Sea(VS_OUTPUT_SEA In) : COLOR
{
	PS_DEFERRED OUT = (PS_DEFERRED)0;

	//UV�Q�쐬
	float2 UV1 = In.Tex1;
	float2 UV2 = In.Tex2;

	/* �����}�b�s���O */
	// �����}�b�v���Q�Ƃ��A�������擾����
	float h = tex2D(HeightSamp, UV1).r - 0.5f;
	h += tex2D(HeightSamp, UV2).r - 0.5f;
	h *= 0.5;//�@��T���v�����O���Ă邩�畽�ς�����Ă���

	// �e�N�Z���𒸓_���W�n�ł̎����x�N�g�������ɏd�݂����Ă��炷�B
	float3 E = normalize(In.vE);//�ڐ�
	UV2 -= 0.02f * h * E.xy + uvSea;
	UV1 -= 0.02f * h * E.xy + uvSea;//���ƂŐ��l�グ��

	/* �����K�p��� �@���擾 */
	//�Q��UV����@���}�b�v���Q�Ƃ��A�@�����擾����
	float3 NMap = (tex2D(NormalSamp, UV1).rgb - 0.5f)*2.0f;
	NMap += (tex2D(NormalSamp, UV2).rgb - 0.5f)*2.0f;
	NMap *= 0.5f;

	float3 Normal = NMap.r*In.T + NMap.g*In.B + NMap.b*In.Normal;
	OUT.Normal.rgb = Normal*0.5f + 0.5f;
	OUT.Normal.a = 1.0f;// �n�C�g�}�b�v���g���Ă���̂�AO�͉���1.0f

	/*���f�J�[���ɂ܂��͐F������*/
	OUT.Diffuse = tex2D(DecaleSamp, UV1) + tex2D(DecaleSamp, UV2);
	OUT.Diffuse *= 0.5;//�@��T���v�����O���Ă邩�畽�ς�����Ă���
	OUT.Diffuse.rgb *= SeaColor;// �C�̐F�w��
	OUT.Diffuse.a = 1.0f;


	//���X�y�L����
	float4 sp_tex = tex2D(SpecularSamp, UV1);
	sp_tex += tex2D(SpecularSamp, UV2);
	sp_tex *= 0.5;//�@��T���v�����O���Ă邩�畽�ς�����Ă��Ă�
	OUT.Specular = sp_tex;


	//���[�x
	OUT.Depth = In.Depth.z / In.Depth.w;			//w�Ŋ�����-1~1�ɕϊ�����
	//0�ȉ���PS�Œe����邽��0~1�̏����͍s��Ȃ�
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
///		�f�B�t�@�[�h�ł̃��C�g����
///
//************************************************************************************************

//------------------------------------------------------
///		�X�N���[���X�y�[�X�[�x
//------------------------------------------------------
texture DepthBuf;	//	�J������Ԑ[�x
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
///		�X�y�L�����o�b�t�@
//------------------------------------------------------
texture SpecularBuf;	//	�X�y�L����
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
///		�X�N���[���X�y�[�X�@��
//------------------------------------------------------
texture NormalBuf;	//	�@��
sampler NormalBufSamp = sampler_state
{
	Texture = <NormalBuf>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = NONE;

	AddressU = CLAMP;
	AddressV = CLAMP;
};



// ���C�g�̏����ł͐F�̏��ƃX�y�L�����̏���f���o��
struct DEF_LIGHT
{
	float4	color	:	COLOR0;
	float4	spec	:	COLOR1;
};

float3 LightColor = { 1.0f, 1.0f, 1.0f };
float sp_rate = 2.0f;

///------------------------------------------------------
//		�v���W�F�N�V��������r���[�s��� 
///------------------------------------------------------
// �t�s��Ɖ��s�̏��͊O������Ƃ��Ă��Ă܂�
inline float4 ProjectionToView(float2 NormalTex)//���@���}�b�v���g���܂�
{
	float4 pos;
	pos.xy = NormalTex*2.0f - 1.0f;
	pos.y *= -1;
	pos.z = tex2D(DepthBufSamp, NormalTex).r;//�[�x�}�b�v���T���v�����O���ĉ��s����ɓ����
	pos.w = 1.0f;

	// �t�s��Ńr���[���W�n�ɕϊ�
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	return pos;
}


//**************************************************************************************************
///	���}�b�v�p
//**************************************************************************************************
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
///		�f�B�t�@�[�h���s��		
//***************************************

float3 LightVec = { 1.0f, -1.0f, 1.0f };// cpu���Ńr���[���W�n�ɕϊ�����
float RimPower = 2.0f;

DEF_LIGHT PS_DirLight(float2 Tex:TEXCOORD0) :COLOR
{
	DEF_LIGHT OUT;



	// �@���}�b�v����@���̌����̎擾
	float4 n = tex2D(DecaleSamp, Tex); 
	//if (n.a <= 0.1f)
	//{
	//	discard;
	//}
	// �@���擾
	float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
	normal = normalize(normal);			// �@�����K��
	
	// AO�}�b�v�擾
	float AO = n.a;

	// ���C�g��(�����o�[�g)
	float rate = dot(normal, -LightVec);
	rate = max(0, rate);// ���[�g��-�ɂ����Ȃ�
	//if (rate <= 0.0f)discard;

	// �s�N�Z���̐F
	OUT.color.rgb = (rate * LightColor);
	OUT.color.a = 1;
	OUT.color.rgb *= AO;

	//�@�X�y�L����
	float4 pos = ProjectionToView(Tex);//�@�r���[�s���
								 //���@���}�b�v���Ԃ�����ł�������
	// �r���[�s��ł̖ڐ�
	float3 E = pos.xyz;
		E = normalize(E);
	float3 R = normalize(-LightVec - E);
	float sp = pow(max(0, dot(R, normal)), 80) * sp_rate;// �����ʂ�i����


	//�@�e�F�Ɍ��̃p�����[�^�[��ݒ�
	//  R->Spec  G->Bloom  B->Rim
	float3 multiRate = tex2D(SpecularBufSamp, Tex).rgb;

		// (R) �X�y�L�����̋���
		OUT.spec.rgb = LightColor * multiRate.r * sp;// *(1.0f - multiRate.b) + env.rgb * multiRate.b;
		OUT.spec.a = 1.0f;

		// (B) �������C�g
		//���C�g���J�������ʋ߂��ɂ���قǋ����Ȃ�悤�ɂ���B
		float RimPower = max(.0f, dot(-E, LightVec));
		//�֊s�t�߂̋��x����������
		float OutlinePower = pow(1.0f - dot(normal, -E), 5.0f);

		//�����J���[
		float3 RimColor = (LightColor * (OutlinePower*RimPower))*multiRate.b;// �̗v�f�ŋ�����ς���
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
float3 LightPos = { .0f, .0f, .0f };// cpu���Ńr���[���W�ɂ��������ʒu
float LightRange = .0f;				// �����̍L����
float LightPower = .0f;

DEF_LIGHT PS_PointLight(float2 Tex : TEXCOORD0) :COLOR
{
	DEF_LIGHT OUT;

	//�X�N���[�����W�n�ʒu�̎擾
	float4 pos = ProjectionToView(Tex);

	// (�����Ƃ̋���)��(���̌���)���擾
	float3 lightDir = ((float3)pos - LightPos) / LightRange;

	if (length(lightDir) < 1.0f)
	{
		// �����ʍ��݂̋���
		float distance = max(0, 1.0f - (length(lightDir)
			/ LightPower));//�����l��ς��i��

		// �����ʍ��݂̃|�C���g���C�g���悹��
		OUT.color.rgb = LightColor * distance;

		// ���K�����ăx�N�g����
		float3 lightVec = normalize(lightDir);

		// �@���}�b�v����@���̌����̎擾
		float4 n = tex2D(DecaleSamp, Tex);
			float3 normal = n.rgb * 2.0f - 1.0f;
			normal = normalize(normal);
		// ���C�g��
		float r = max(0, dot(normal, -lightVec));

		// �s�N�Z���F����
		OUT.color.rgb *= r;
		OUT.color.a = 1;


		//�X�y�L����
		float3 E = pos.xyz;
			E = normalize(E);
		float3 R = normalize(-lightVec - E);
			float sp = pow(max(0, dot(R, normal)), 10) * r;

		float3 multiRate = tex2D(SpecularBufSamp, Tex).rgb;
			OUT.spec.rgb = LightColor * multiRate.r * sp;
		OUT.spec.a = 1.0f;

		//����
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
///		�V���v���_����
//**********************************

DEF_LIGHT PS_SimpliPointLight(float2 Tex : TEXCOORD0) :COLOR
{
	DEF_LIGHT OUT = (DEF_LIGHT)0;

	//�X�N���[�����W�n�ʒu�̎擾
	float4 pos = ProjectionToView(Tex);

		// (�����Ƃ̋���)��(���̌���)���擾
		float3 lightDir = ((float3)pos - LightPos) / LightRange;

		// �����v�Z
		float distance = max(0, 1.0f - (length(lightDir)));//�����l��ς��i��

	//�����̋������Q�̏ꍇ�͔j������.
	if (length(lightDir) > 1.0f) discard;

		// �����ʍ��݂̃|�C���g���C�g���悹��
		OUT.color.rgb = LightColor * min(1.0f, (distance*4.0f));
		//OUT.color.r = 100;
		OUT.color.a = 1.0f;
		
		//�X�y�L����
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
///		�������C�e�B���O
//
//*************************************
float3 SkyColor = { .4f, .6f, .6f };
float3 GroundColor = { .6f, .4f, .4f };

float4 PS_HemiLight(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	//�@���}�b�v����@���̌����̎擾
	float4 n = tex2D(DecaleSamp, Tex);
		float3 normal = n.rgb * 2.0f - 1.0f;
		normal = normalize(normal);

	float AO = n.a;// Normal�̃�����AO�̒l���擾

	//��������
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
//		��ʊE�[�x
//
//****************************************************
float3	DOF_target;		//�J�����̃^�[�Q�b�g(�r���[���W�n)
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

	//�X�N���[�����W�n�ʒu�̎擾
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1����1�ɖ߂�
	pos.y *= -1;
	pos.z = tex2D(DepthBufSamp, Tex).r;
	pos.w = 1.0f;

	//�t�s��Ńr���[���W�n�ɕϊ�����
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	float3 Length = pos - DOF_target;
		float l = length(Length);

	//range��藣�ꂽ�ʒu���烿�𑝂₵�Ă���
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
float4x4 InvViewProj;			//�@�v���W�F�N�V����->���[���h�ϊ��s��

float AdjustValue = -0.002f;	//�@�����܂ł��e�Ɣ��f����̂��@��������Y��ɉe���t�����W���M�[���ڗ��@�グ��Ƃ��̋t
float Shadow = .6f;				//�@�e�̔Z��
float DistAlpha = .2f;			//�@�����ɉ����ĉe�̃���������

float ShadowRange = 0.0f;


//*************************************************
//
//			�T���v���[
//
//*************************************************

// �e�p�[�x�}�b�v
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

// �ߌi�V���h�E�}�b�v
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


//���i�V���h�E�}�b�v
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
//			�V���h�E�}�b�v�쐬
//***************************************

struct VS_SHADOW
{
	float4 Pos			:POSITION;
	float4 Color		:TEXCOORD0;		//�[�x�l
	float2 Tex			:TEXCOORD1;
};

// �ߋ����p�V���h�E�}�b�v�쐬
VS_SHADOW VS_ShadowBuf(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_SHADOW Out;

	// ���W�ϊ�
	float4x4	mat = mul(TransMatrix, ShadowProjection);
		Out.Pos = mul(Pos, mat);

	Out.Color.r = Out.Pos.z;
	Out.Color.g = Out.Pos.z * Out.Pos.z;
	Out.Color.b = .0f;
	Out.Color.a = 1.0f;

	//���s���e���Ă���̂�W�����Ŋ���K�v�Ȃ��@(���ڂ񂱂؂����񂱃��[�h)
	//Out.Color = Out.Pos.z / Out.Pos.w;

	Out.Tex = Tex;

	return Out;
}

// �������p�V���h�E�}�b�v�쐬
VS_SHADOW VS_ShadowBufL(float4 Pos : POSITION, float2 Tex : TEXCOORD0)
{
	VS_SHADOW Out;

	// ���W�ϊ�
	float4x4	mat = mul(TransMatrix, ShadowProjectionL);
		Out.Pos = mul(Pos, mat);

	Out.Color.r = Out.Pos.z;
	Out.Color.g = Out.Pos.z * Out.Pos.z;
	Out.Color.b = .0f;
	Out.Color.a = 1.0f;

	Out.Tex = Tex;

	return Out;
}

// PixelShader�͋��ʂ�
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
	float4 Spec			:COLOR1;// �X�y�L�������G��
};

//*************************
///		�e�N�X�`�����W�����֐�
//*************************
inline float3 GetShadowTex(float4 Pos)
{
	float4	Tex;
	// �e�N�X�`�����W�v�Z
	Tex = mul(Pos, ShadowProjection);

	Tex.y = -Tex.y;
	Tex.xy = 0.5f * Tex.xy + 0.5f;
	Tex.xyz /= Tex.w;

	return Tex.xyz;
}

inline float3 GetShadowTexL(float4 Pos)
{
	float4	Tex;
	// �e�N�X�`�����W�v�Z
	Tex = mul(Pos, ShadowProjectionL);

	Tex.y = -Tex.y;
	Tex.xy = 0.5f * Tex.xy + 0.5f;
	Tex.xyz /= Tex.w;

	return Tex.xyz;
}

//�ʏ��
inline float GetShadow(float3 Tex)
{
	float d = tex2D(ShadowSamp, Tex.xy).r;		//�V���h�E�}�b�v����[�x�����o��
	if (Tex.z > 1.0f) Tex.z = 1.0f;
	float l = (d < Tex.z + AdjustValue) ? Shadow : 1;
	return l;
}
inline float GetShadow(float3 Tex, sampler samp)
{
	float d = tex2D(samp, Tex.xy).r;		//�V���h�E�}�b�v����[�x�����o��
	if (Tex.z > 1.0f) Tex.z = 1.0f;
	float l = (d < Tex.z + AdjustValue) ? Shadow : 1;
	return l;
}
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
	float l = (d < Tex.z + AdjustValue) ? Shadow : 1;	//������
	return l;
}
//�V�O���C�h
inline float sigmoid(float gain, float x, float adjust)
{
	//
	return 1.0 / (1.0 + exp(-gain * (x - adjust)));
}

//�o���A���X
inline float GetVarianceShadow(float3 Tex)
{
	half d = Tex.z;

	//	�ߌi�l
	float2	t = tex2D(ShadowSamp, Tex.xy).rg;		// �V���h�E�}�b�v����[�x�����o��
		float d1 = t.r;							// �[�x�l
	float variance = max(t.g - d1 * d1, .0f);	// ���U

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// �`�F�r�V�F�t�̕s����

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;

	//	�Z�x�␳	
	l = 1.0f - (l * (1.0f - Shadow));

	return l;
}

inline float GetVarianceShadowL(float3 Tex)
{
	half d = Tex.z;

	//	���i�l
	float2	t = tex2D(ShadowSampL, Tex.xy).rg;		// �V���h�E�}�b�v����[�x�����o��
		float d1 = t.r;							// �[�x�l
	float variance = max(t.g - d1 * d1, .0f);	// ���U

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// �`�F�r�V�F�t�̕s����

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;

	//	�Z�x�␳	
	l = 1.0f - (l * (1.0f - Shadow));

	return l;
}

inline float GetVarianceShadow(float3 Tex, sampler shadowSamp)
{
	half d = Tex.z;

	//	���i�l
	float2	t = tex2D(shadowSamp, Tex.xy).rg;		// �V���h�E�}�b�v����[�x�����o��
		float d1 = t.r;							// �[�x�l
	float variance = max(t.g - d1 * d1, .0f);	// ���U

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// �`�F�r�V�F�t�̕s����

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * .9f;

	//	�Z�x�␳	
	l = 1.0f - (l * (1.3f - Shadow));

	return l;
}


inline float GetAlphaVarianceShadow(float3 Tex, float a)
{
	half d = Tex.z;

	//	�ߌi�l
	float2	t = tex2D(ShadowSamp, Tex.xy).rg;		// �V���h�E�}�b�v����[�x�����o��
		float d1 = t.r;							// �[�x�l
	float variance = max(t.g - d1 * d1, .0f);	// ���U

	float te = d - d1;					// t - E
	float l1 = variance / (variance + (te * te));		// �`�F�r�V�F�t�̕s����

	float l2 = (d1 < d + AdjustValue) ? .0f : 1.0f;
	float l = (1.0f - max(l1, l2)) * 0.7f;


	//	�Z�x�␳	
	l = 1.0f - (l * (1.0f - Shadow)) * a;

	return l;
}

//�J�X�P�[�h + �o���A���X��
inline float	GetShadowCascade(float3 Tex, float3 Tex2, float a)
{

	float d = Tex.z;

	//	�ߌi�l
	float2	t = tex2D(ShadowSamp, Tex.xy).rg;		// �V���h�E�}�b�v����[�x�����o��
		float	d1 = t.r;							// �[�x�l
	float	variance = max(t.g - d1*d1, 0.0);	// ���U

	float	 te = d - d1;					// t - E
	float	 l1 = variance / (variance + te * te);

	float l = (1.0f - l1) * a;

	//	���i�l
	float t2 = tex2D(ShadowSampL, Tex2.xy).r;	// �V���h�E�}�b�v����[�x�����o��
	float l2 = (t2 < Tex2.z + AdjustValue) ? 1.0f - a : 0;
	return saturate(1.0f - (l + l2));
}

///------------------------------------------------------
//		�v���W�F�N�V�������烏�[���h�s��� 
///------------------------------------------------------
// �t�s��Ɖ��s�̏��͊O������Ƃ��Ă��Ă܂�
//inline float4 ProjectionToWorld(float2 Tex, float depth)
//{
//	//�X�N���[�����W�̐���
//	float4 screen;
//	screen.xy = Tex * 2.0f - 1.0f;			//0~1��-1~1��
//	screen.y *= -1.0f;
//	screen.z = depth;
//	screen.w = 1.0f;
//
//	float4 world = mul(screen, InvViewProj);// ���[���h���W�ɕϊ�
//		world.xyz /= world.w;				// �p�[�X����
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
	///���[���h���W�ϊ�
	//*****************
	float depth = tex2D(DepthSamp, Tex).r;

	//�X�N���[�����W�̐���
	float4 screen;
	screen.xy = Tex * 2.0f - 1.0f;			//0~1��-1~1��
	screen.y *= -1.0f;
	screen.z = depth;
	screen.w = 1.0f;

	float4 world = mul(screen, InvViewProj);// ���[���h���W�ɕϊ�
		world.xyz /= world.w;				// �p�[�X����
	world.w = 1.0f;

	// �����̎��_��
	/// �@���s��depth	�X�N���[�����W��screen�@���[���h���Wworld�@ ���擾


	/*_____���s�����������Ă��邩���v�Z__________*/
	// �@���}�b�v����@���̌����̎擾
	float4 n = tex2D(NormalBufSamp, Tex);
		float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
		normal = normalize(normal);			// �@�����K��
	// ���C�g��(�����o�[�g) 0~1
	float r =max(0.0f, dot(normal, -LightVec));


	///*****************
	//	�e���Y
	///*****************

	//�@���[���h���W�́@���E�̂��̏ꏊ��Pos�Ɩڂ�Pos�ƈ����ā@�ڂ���|�W�V������Vector���Q�b�g
	float3 V = world - ViewPos;				//���_���烂�f���̃x�N�g��
		V.y = .0f;

	float dist = length(V);//�@���̌����̋������擾
	float vShadowW = 1.0f - (dist / ShadowRange) * DistAlpha;			//�����̒���
	vShadowW = saturate(vShadowW);

	//�@�����z�n�_����̂��̃|�W�V��������ɓ����
	float3 vShadow = GetShadowTex(world);
	//float shadow = GetAlphaVarianceShadow(vShadow, vShadowW);
	//float shadow = GetVarianceShadow(vShadow);
	float shadow = GetShadow(vShadow);


	OUT.Shadow.rgb = shadow;
	OUT.Shadow.a = 1.0f;

	//����
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
	// ���[���h���W�ϊ�
	//*****************
	float depth = tex2D(DepthSamp, Tex).r;

	//�X�N���[�����W�̐���
	float4 screen;
	screen.xy = Tex * 2.0f - 1.0f;			//0~1��-1~1��
	screen.y *= -1.0f;
	screen.z = depth;
	screen.w = 1.0f;

	float4 world = mul(screen, InvViewProj);
		world.xyz /= world.w;						//�p�[�X����
	world.w = 1.0f;


	//**************************
	// ���_����̋����v�Z(�J�X�P�[�h�p)
	//**************************
	float3 V = world - ViewPos;				//���_���烂�f���̃x�N�g��
		V.y = .0f;

	float dist = length(V);

	float3 vShadow = mulScreen(world, ShadowProjection);
	float3 vShadowL = mulScreen(world, ShadowProjectionL);
	float vShadowW = (30.0f - (dist - 110.0f)) * 1.25f;			//�����̒���
	vShadowW = saturate(vShadowW);

	float shadow = GetShadowCascade(vShadow, vShadowL, vShadowW);
	shadow = 1.0f - ((1.0f - shadow) * .5f);


	/*_____���s�����������Ă��邩���v�Z__________*/
	// �@���}�b�v����@���̌����̎擾
	float4 n = tex2D(NormalBufSamp, Tex);
	float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
	normal = normalize(normal);			// �@�����K��
	// ���C�g��(�����o�[�g) 0~1
	float r =  dot(normal, -LightVec);
	float flag = step(0.0f, r);//0�ȉ��Ȃ�1�Ɉȏ�Ȃ�0��

	//float near_ShadowLevel = max(Shadow, GetSM(vShadow.xyz, ShadowSamp));
	//float far_ShadowLevel = max(Shadow, GetSM(vShadowL.xyz, ShadowSampL));
	//float s = lerp(near_ShadowLevel, far_ShadowLevel, sigmoid(130.0, (depth / 23.0), 41.0));//���̐��l�Œ��x�����ꏊ��T��
	//OUT.rgb = s;

	// �����艟���ŉ����ɍs���Ɣ����Ȃ�
	float n_shadow = GetShadow(vShadow);
	n_shadow = 1 + n_shadow * -1;
	n_shadow *= 1.0 - (dist / ShadowRange) * DistAlpha;

	float f_shadow = 1 + shadow*-1;
	n_shadow += f_shadow;

	n_shadow=1 + n_shadow*-1;



	OUT.rgb = n_shadow + 1.0f - (1.0f*flag);//0~1
	OUT.a = 1.0f;

	//����


	return OUT;
}


PS_SHADOW PS_DualShadow(float2 Tex	:	TEXCOORD0) : COLOR
{
	PS_SHADOW OUT = (PS_SHADOW)0;

	//*********************************
	///���[���h���W�ϊ�	3�̍��W�擾
	//*********************************
	float depth = tex2D(DepthSamp, Tex).r;

	//�X�N���[�����W�̐���
	float4 screen;
	screen.xy = Tex * 2.0f - 1.0f;			//0~1��-1~1��
	screen.y *= -1.0f;
	screen.z = depth;
	screen.w = 1.0f;

	float4 world = mul(screen, InvViewProj);// ���[���h���W�ɕϊ�
	world.xyz /= world.w;				// �p�[�X����
	world.w = 1.0f;

	///*****************
	//	�e���Y
	///*****************

	//�@���[���h���W�́@���E�̂��̏ꏊ��Pos�Ɩڂ�Pos�ƈ����ā@�ڂ���|�W�V������Vector���Q�b�g
	float3 V = world - ViewPos;				//���_���烂�f���̃x�N�g��
		V.y = .0f;

	float dist = length(V);//�@���̌����̋������擾
	float vShadowW = 1.0f - (dist / 20) * DistAlpha;			//�����̒���
	vShadowW = saturate(vShadowW);

	//�@�����z�n�_����̂��̃|�W�V��������ɓ����
	float3 vShadow = mulScreen(world, ShadowProjection);
	float3 vShadowL = mulScreen(world, ShadowProjectionL);


	//float far_shadow = GetVarianceShadow(vShadowL, ShadowSampL);


	// �����艟���ŉ����ɍs���Ɣ����Ȃ�
	float n_shadow = GetShadow(vShadow);
	float f_shadow = GetShadow(vShadowL, ShadowSampL);
	//f_shadow *= 100;
	n_shadow = 1 + n_shadow*-1;
	n_shadow *= 1.0 - (dist / ShadowRange) * DistAlpha;
	f_shadow = 1 + f_shadow*-1;
	n_shadow += f_shadow;
	n_shadow = 1 + n_shadow*-1;


	/* ���s�����������Ă��邩���v�Z */
	// �@���}�b�v����@���̌����̎擾
	float4 n = tex2D(NormalBufSamp, Tex);
		float3 normal = n.rgb * 2.0f - 1.0f;// 1~-1
		normal = normalize(normal);			// �@�����K��
	// ���C�g��(�����o�[�g) -1~1
	float r = dot(normal, -LightVec);
	//r = max(0.0f, r);// ���[�g��-�ɂ����Ȃ�

	//float flag = step(0.4f, r);//0�ȉ��Ȃ�1�Ɉȏ�Ȃ�0�� 0.3�����@����𑝂₷�Ƃǂ��܂ŉA�e�ɉe�𗎂Ƃ��Ȃ����𒲐��ł���


	//�@HDR��1�ȏ�ɂȂ��Ă���̂�1�Ŏa��悤�ɂ���
	//OUT.Shadow.rgb = max(0.5f, n_shadow + 1.2f - (1.0f*r));//0~1; //�e�̔Z�������� 1.0�Œ���
	OUT.Shadow.rgb = n_shadow;//0~1;
	OUT.Shadow.b += 0.05f;// �V�тŐ��ۂ�
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
//		Forward�p
//
//************************************************************************************************

//------------------------------------------------------
//		���_�t�H�[�}�b�g
//------------------------------------------------------
struct VS_OUTPUT_HDR
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 Tex		: TEXCOORD0;

	float4 wPos			: TEXCOORD1;//�@�s�N�Z���ɑ�����Ƀ��[���h����̃|�W�V�����ǉ�
	float4 ProjectionPos: TEXCOORD2;//�@�s�N�Z���ɑ�����ɃQ�[����ʃ��X�g�̃|�W�V�����ǉ�
};

struct VS_INPUT_HDR
{
	float4 Pos    : POSITION;
	float3 Normal : NORMAL;
	float4 Color  : COLOR0;
	float2 Tex	  : TEXCOORD0;
};

//------------------------------------------------------
//		���_�V�F�[�_�[	
//------------------------------------------------------
VS_OUTPUT_HDR VS_HDR_Lighting(VS_INPUT_HDR In)
{
	VS_OUTPUT_HDR Out = (VS_OUTPUT_HDR)0;
	//TransMatrix��Pos����������wPos�̏�񐶐�
	Out.wPos = mul(In.Pos, TransMatrix);

	Out.Pos = mul(In.Pos, Projection);
	Out.Tex = In.Tex;
	Out.Color = 1.0f;

	Out.ProjectionPos = Out.Pos;

	//Out.Color.rgb = LightColor;

	return Out;
}
//------------------------------------------------------
//		�s�N�Z���V�F�[�_�[
//------------------------------------------------------
float4 PS_HDR_Lighting(VS_INPUT_HDR In) : COLOR
{
	float4	OUT;
	//	�s�N�Z���F����
	OUT = In.Color * tex2D(DecaleSamp, In.Tex);

	OUT.rgb *= 3;
	//OUT.a = 6000.0f;
	return OUT;
}
//------------------------------------------------------
//		HDR���ݕ`��e�N�j�b�N
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
//		�Ō�̏���
//
//************************************************************************************************

//--------------------------------------------------------
//		�O���[�o���ϐ�
//--------------------------------------------------------
float ambient = .0f;


//---------------------------------------------------------
//		���Z���ʂ̃T���v���[
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

texture SSAOTex;	//	SSAO�e�N�X�`��
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
//		�X�N���[���t�B���^
//------------------------------------------------------
float contrast = 1.0f;		//�R���g���X�g
float chroma = 1.0f;			//�ʓx
float3 ScreenColor = { 1.0f, 1.0f, 1.0f };

//*************************************
//		���C�g + �|�X�g�G�t�F�N�g
//*************************************
float4 PS_Def1(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float3 color;

	float3 dif;
	//�f�B�t���[�Y�J���[�̎擾
	dif = tex2D(DecaleSamp, Tex);
	color = dif;

	//���C�g�̓K��
	color *= tex2D(LightSamp, Tex);

	//�X�y�L�����K��
	color += tex2D(SpecSamp, Tex);

	//�V���h�E�̓K��
	color *= tex2D(ShadowTexSamp, Tex);

	//**************************
	//		�|�X�g�G�t�F�N�g
	//**************************

	//�R���g���X�g����
	color = ((color - 0.5f) * contrast) + 0.5f;						//�R���g���X�g

	//�ʓx����
	float rate = (color.r + color.g + color.b) / 3.0f;
	color = (color - rate) * chroma + rate;							//�F��

	//�J���[�o�����X����
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
//		���C�g + �|�X�g�G�t�F�N�g
//*************************************
float4 PS_Def2(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float3 color;

	float3 dif;
	//�f�B�t���[�Y�J���[�̎擾
	dif = tex2D(DecaleSamp, Tex);
	color = dif;

	//���C�g�̓K��
	color *= tex2D(LightSamp, Tex);

	//�X�y�L�����K��
	color += tex2D(SpecSamp, Tex);


	//**************************
	//		�|�X�g�G�t�F�N�g
	//**************************

	//�R���g���X�g����
	color = ((color - 0.5f) * contrast) + 0.5f;						//�R���g���X�g

	//�ʓx����
	float rate = (color.r + color.g + color.b) / 3.0f;
	color = (color - rate) * chroma + rate;							//�F��

	//�J���[�o�����X����
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
//		�S��
//*************************************
float4 PS_Def3(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	float3 color;

	float3 dif;
	//�f�B�t���[�Y�J���[�̎擾
	dif = tex2D(DecaleSamp, Tex);
	color = dif;

	//���C�g�̓K��
	color *= tex2D(LightSamp, Tex);

	//SSAO�̓K��
	color *= tex2D(SSAOSamp, Tex).r;

	//�X�y�L�����K��
	color += tex2D(SpecSamp, Tex);

	//�V���h�E�̓K��
	color *= tex2D(ShadowTexSamp, Tex);

	//**************************
	//		�|�X�g�G�t�F�N�g
	//**************************

	//�R���g���X�g����
	color = ((color - 0.5f) * contrast) + 0.5f;						//�R���g���X�g

	//�ʓx����
	float rate = (color.r + color.g + color.b) / 3.0f;
	color = (color - rate) * chroma + rate;							//�F��

	//�J���[�o�����X����
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
//		�t�u�A�j���[�V����
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
	Out.Color = In.Color;// ���_�J���[�擾
	Out.Tex = In.Tex + float2(tuAnime, tvAnime);//���W

	Out.Color.rgb = 1.0f;
	Out.Color.a *= alphaUV; //�@�����x

	return Out;
}


//------------------------------------------------------
//		�s�N�Z���V�F�[�_�[	
//------------------------------------------------------
float4 PS_UvAnime(VS_OUTPUT_UV In) : COLOR
{
	float4	OUT;
	//	�s�N�Z���F����
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
		ZEnable = true;			// ���̃I�u�W�F�N�g��Z�o�b�t�@���l������
		ZWriteEnable = false;	// ���̃I�u�W�F�N�g��Z�o�b�t�@�ɏ������܂Ȃ�

		VertexShader = compile vs_3_0 VS_UvAnime();
		PixelShader = compile ps_3_0 PS_UvAnime();
	}
}





/*************************************************************/
///
///				2D�̃|�X�g�G�t�F�N�g��
///
/*************************************************************/

/*�ڂ����p�T���v���[*/
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

	//�e�N�Z�����擾   
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

	//�擾�����e�N�Z���ʒu�̃J���[�����擾����B
	//���ꂼ��̃J���[�l�ɏd�݂������Ă���B���̏d�ݒl�͂��ׂĂ̍��v�� 1.0f �ɂȂ�悤�ɒ�������B
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

	//�J���[����������
	return p0 + p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9 + p10;

}

float4 PS_gaussY(float2 Tex : TEXCOORD0) : COLOR
{

	//�e�N�Z�����擾   
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

	//�擾�����e�N�Z���ʒu�̃J���[�����擾����B
	//���ꂼ��̃J���[�l�ɏd�݂������Ă���B���̏d�ݒl�͂��ׂĂ̍��v�� 1.0f �ɂȂ�悤�ɒ�������B
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

	//�J���[����������
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
		// �V�F�[�_
		PixelShader = compile ps_3_0 PS_gaussX();
	}

	pass P1
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		// �V�F�[�_
		PixelShader = compile ps_3_0 PS_gaussY();
	}
}

//************************************
//		�PPixelBlur
//************************************
float4 PS_Gaussian(float2 Tex : TEXCOORD0) :COLOR
{
	float4 OUT;

	//����
	OUT = tex2D(GaussianSamp, Tex) * .2f;
	//�E
	OUT += tex2D(GaussianSamp, float2(Tex.x + TU, Tex.y)) * .1f;
	//��
	OUT += tex2D(GaussianSamp, float2(Tex.x - TU, Tex.y)) * .1f;
	//��
	OUT += tex2D(GaussianSamp, float2(Tex.x, Tex.y - TV)) * .1f;
	//��
	OUT += tex2D(GaussianSamp, float2(Tex.x, Tex.y + TV)) * .1f;
	//����
	OUT += tex2D(GaussianSamp, float2(Tex.x - TU, Tex.y - TV)) * .1f;
	//����
	OUT += tex2D(GaussianSamp, float2(Tex.x - TU, Tex.y + TV)) * .1f;
	//�E��
	OUT += tex2D(GaussianSamp, float2(Tex.x + TU, Tex.y - TV)) * .1f;
	//�E��
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
//  HDR�u���[������
/*-------------------*/

// ------------------------------
// �u���[���̎���쐬
// ------------------------------

// �ڂ����O�ɂڂ������邳�𒲐�
float4 PS_hdrBloomSeed(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT;

	OUT = tex2D(GaussianSamp, Tex);

	OUT.rgb -= 0.75f;//�����Ő��l���i��

	//�X�y�L������g����������擾
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
		// �V�F�[�_
		PixelShader = compile ps_3_0 PS_hdrBloomSeed();
	}
}

// ------------------------------
// �쐬�����u���[�����̂�����
// ------------------------------
float4 PS_hdrBloom(float2 Tex:TEXCOORD0) : COLOR
{
	float4 OUT;
	OUT = tex2D(GaussianSamp, Tex);
	OUT.rgb += OUT.rgb* 2.5f;//���u���[���̐F�̋����𒲐�
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
		// �V�F�[�_
		PixelShader = compile ps_3_0 PS_hdrBloom();
	}
}




/****************************/
/*			ToneMap			*/
/****************************/

/*�I���x*/
float exposure = 0.0f;

/*�s�N�Z���V�F�[�_�[*/
float4 PS_Tone(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT = (float4)0;

	//No.2	1.0����2.0�@���邢
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
		// �G�t�F�N�g���o�Ȃ��Ȃ������� OFF��
		ZEnable = False;// ���̃I�u�W�F�N�g��Z�o�b�t�@���l�����Ȃ�

		// �V�F�[�_
		PixelShader = compile ps_3_0 PS_Tone();
	}

}

/*�s�N�Z���V�F�[�_�[*/
float4 PS_Tone_DOF(float2 Tex	:	TEXCOORD0) : COLOR
{
	float4 OUT = (float4)0;

	//No.2	1.0����2.0�@���邢
	OUT.rgb = tex2D(DecaleSamp, Tex)* exp2(exposure);
	OUT.a = 1.0f;


	//�X�N���[�����W�n�ʒu�̎擾
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1����1�ɖ߂�
	pos.y *= -1;
	pos.z = tex2D(DepthBufSamp, Tex).r;
	pos.w = 1.0f;

	//�t�s��Ńr���[���W�n�ɕϊ�����
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	float3 Length = pos - DOF_target;
		float l = length(Length);

	//range��藣�ꂽ�ʒu���烿�𑝂₵�Ă���
	//float alpha = saturate(l / range - 1.0f);
	float alpha = l / DOF_Range;
	OUT.a = saturate(alpha);// 0~1��

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
		// �V�F�[�_
		PixelShader = compile ps_3_0 PS_Tone_DOF();
	}

}


//*****************************************
//		ScreenSpaceAmbientOcclusion
//*****************************************

//�T���v�����O��(16,12,10,8)
#define SAMPLES 16							//���̓s�N�Z���̃T���v�����O��
float HemRadius = 1.0f;						//�����Օ�����p�̔����̔��a
float Zfar = 0.01f;							//�����Օ�����p�̐[�x���̍ő�l
float AOPower = 3.0f;						//�A�̋��x

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

	//�`��s�N�Z���̖@���x�N�g�����擾
	float3 normal = tex2D(NormalBufSamp, Tex).rgb * 2.0f - 1.0f;
		normal = normalize(normal);

	//�`��s�N�Z���̐[�x�̎擾
	float depth = tex2D(DepthBufSamp, Tex).r;

	//�v���W�F�N�V�������W�n�̎擾
	float4 pos;
	pos.xy = Tex * 2.0f - 1.0f;					//-1����1�ɖ߂�
	pos.y *= -1;
	pos.z = depth;
	pos.w = 1.0f;

	//�r���[���W�n�ł̈ʒu�̎擾
	pos = mul(pos, InvProj);
	pos.xyz /= pos.w;

	float normAO = .0f;
	float depthAO = .0f;

	//���C���΂��ĎՕ�����Ă��邩
	for (uint i = 0; i < SAMPLES; i++){

		//���C�̕����x�N�g��
		float3 ray = SphereArray[i] * HemRadius;

		//���C�̕����x�N�g����`��s�N�Z���̖@�������̔������Ɏ��܂�悤�ɕ�����ϊ�����
		ray = sign(dot(normal, ray)) * ray;

		//���̓s�N�Z���̍��W
		float4 envPos;
		//���C�����Ɉړ�
		envPos.xyz = pos.xyz + ray;
		//�v���W�F�N�V�������W�n�ɕϊ�
		envPos = mul(float4(envPos.xyz, 1.0f), matProjection);
		//�X�N���[�����W�n�ɕϊ�
		envPos.xy = envPos.xy / envPos.w * float2(.5f, -.5f) + .5f;

		//���̓s�N�Z���̖@���x�N�g���̎擾
		float3 envNormal = tex2D(NormalBufSamp, envPos.xy).xyz * 2.0f - 1.0f;
			envNormal = normalize(envNormal);

		//���Ӄs�N�Z���̐[�x�̎擾
		float envDepth = tex2D(DepthBufSamp, envPos.xy).r;

		//���ς̊p�x���傫���Ȃ�قǊ����Օ��W�����傫���Ȃ�悤�Ɍv�Z����
		float n = dot(normal, envNormal) * .5f + .5f;

		//�G�b�W���ʂɂȂ��Ă��镔���͎Օ�����Ȃ��悤�ɂ���
		n += step(depth, envDepth);
		normAO += min(n, 1.0f);

		//�[�x�l�̋������傫���قǊ����Օ��̉e���͂��������Ȃ�悤�ɂ���
		depthAO += abs(depth - envDepth) / Zfar;
	}

	float Color = normAO / (float)SAMPLES + depthAO;
	//�A����������
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