#include "Deferred.h"
/*
*	Deferred
*/

Deferred* Deferred::pInstance = nullptr;

/// �C���X�^���X
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

/// ������
void Deferred::Init()
{
	// G_Buffer���쐬
	diffuse	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET); // RGB �A���׃h
	normal	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);	 // RGB �@���@A �A���r�G���g���[�g
	specular= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET); // R �X�y�L���� G �u���[���@B �������C�g
	depth	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_FLOAT);		 // R �f�v�X

	// G-Buffer�𗘗p����T�[�t�F�C�X
	light	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);
	spec	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);
	dof		= new iex2DObj(Reduction_x, Reduction_y, IEX2D_HDR);			//A8R8G8B8
	shadow	= new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);
	
	// FinalPassSurface
	screen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);

	// ForwardSurface
	forward = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_USEALPHA);		//	�ォ��킹�邽�ߓ����x���̗p

	// Bloom
	bloomScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_HDR);			//�@�Q�[����ʂ�HDR�p�ɂ�����p
	bloom = new iex2DObj(MiniTex_x, MiniTex_y, IEX2D_HDR);		// HDR

	// Glow
	glowScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);	//�@�Q�[����ʂ�Glow�p�ɂ�����p
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

	// ���}�b�v
	EnvMap = new iex2DObj("DATA/Shader/Env.png");
	shaderD->SetValue("EnvMap", EnvMap);
}

Deferred::Deferred()
{
	Init();
}

/// ���
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

/// G_Buffer�쐬�J�n &�O��g�p�����̃T�[�t�F�C�X�̃N���A
void Deferred::Bigin()
{
	//�o�b�N�o�b�t�@�T�[�t�F�C�X�̕ۑ�
	iexSystem::Device->GetRenderTarget(0, &backbuffer);

	//���C�g�T�[�t�F�C�X��ʃN���A
	light->RenderTarget();
	spec->RenderTarget(1);
	//if (ssaoFlag == true)ssao->RenderTarget(2);


	//���C�g�T�[�t�F�C�X��ʃN���A
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	//MRT�̊��蓖��
	diffuse->RenderTarget(SURFACE_NAME::DIFFUSE);
	normal->RenderTarget(SURFACE_NAME::NORMAL);
	specular->RenderTarget(SURFACE_NAME::SPECULAR);
	depth->RenderTarget(SURFACE_NAME::DEPTH);

	//G_Buffer��ʃN���A
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);



}
/// G_Buffer�쐬�I��
void Deferred::End()
{
	//�o�b�N�o�b�t�@�T�[�t�F�C�X�̕����EMRT�̏I������
	iexSystem::Device->SetRenderTarget(NORMAL, nullptr);
	iexSystem::Device->SetRenderTarget(SPECULAR, nullptr);
	iexSystem::Device->SetRenderTarget(DEPTH, nullptr);


	//��������e�N�X�`����]��
	shaderD->SetValue("DepthBuf", depth);
	shaderD->SetValue("SpecularBuf", specular);
	shaderD->SetValue("NormalBuf", normal);


	iexSystem::Device->SetRenderTarget(0, backbuffer);


}

/// Deferred�̍X�V
void Deferred::Update(const Vector3 ViewPos)
{
	this->ViewPos = ViewPos;
	shaderD->SetValue("ViewPos", this->ViewPos);

	//�v���W�F�N�V�����t�s�񐶐�
	Matrix invproj = matProjection;
	D3DXMatrixInverse(&invproj, nullptr, &invproj);
	shaderD->SetValue("InvProj", invproj);

	//SSAO�𓱓�����Ȃ�A�v���W�F�N�V�����ϊ��s���]��
	if (ssaoFlag) shaderD->SetValue("matProjection", matProjection);
	//  ���������Ă���̂ł����Ȃ� 
	//	�V�F�[�_�[�ɕK�v�ȏ��𑗂�
	//shaderD->SetValue("matView", matView);

	if (shadowFlag){
		//�r���[�t�s�񐶐�
		Matrix invview = matView;
		D3DXMatrixInverse(&invview, nullptr, &invview);
		shaderD->SetValue("InvViewProj", invproj * invview);
	}

}

//**************************************************
///				Lighting
//**************************************************
/// ���s��
void Deferred::DirLight(const Vector3 dir, const Vector3 color)
{
	Matrix matV = matView;

	Vector3 LightDir;

	//�r���[���W�n�ɕϊ�
	LightDir.x = dir.x * matV._11 + dir.y * matV._21 + dir.z * matV._31;
	LightDir.y = dir.x * matV._12 + dir.y * matV._22 + dir.z * matV._32;
	LightDir.z = dir.x * matV._13 + dir.y * matV._23 + dir.z * matV._33;


	LightDir.Normalize();

	//	�V�F�[�_�[�ݒ� shaderD�ɑ���
	shaderD->SetValue("LightVec", LightDir);
	shaderD->SetValue("LightColor", (Vector3)color);

	//���݂̃����_�[�^�[�Q�b�g���ꎞ�I�Ɋm��
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//�����_�[�^�[�Q�b�g�̐ؑւ�
	light->RenderTarget();
	spec->RenderTarget(1);


	//	�����_�����O
	normal->Render(shaderD, "def_DirLight");

	//�����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);
}

/// �|�C���g���C�g
void Deferred::PointLight(const Vector3 pos, const Vector3 color, const float range, const float power)
{

	Matrix mWV = matView;// �r���[�s��

	// ������J�����O����
	Vector3 flont(mWV._13, mWV._23, mWV._33);
	flont.Normalize();
	Vector3 dir = pos - (ViewPos - flont * range);			//�����W�̒l�������āA�J�����O���ɂ߂�B
	dir.Normalize();

	float dot = Vector3Dot(dir, flont);
	if (dot < .5f) return;

	// ���C�g�ʒu���r���[���W�n�ɕϊ�
	Vector3 PointLight;
	D3DXVECTOR3 plight = D3DXVECTOR3(pos.x, pos.y, pos.z);
	D3DXVec3TransformCoord(&plight, &plight, &mWV);
	PointLight = Vector3(plight.x, plight.y, plight.z);

	// �K�v�p�����[�^�̓]��
	shaderD->SetValue("LightPos", PointLight);
	shaderD->SetValue("LightColor", (Vector3)color);
	shaderD->SetValue("LightRange", range);
	shaderD->SetValue("LightPower", power);

	// ���݂̃����_�[�^�[�Q�b�g���ꎞ�I�Ɋm��
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	// �����_�[�^�[�Q�b�g�̐ؑւ�
	light->RenderTarget();
	spec->RenderTarget(1);


	// �����_�����O
	normal->Render(shaderD, "def_PointLight");

	// �����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);

}

/// �V���v���|�C���g���C�g
void Deferred::SimpliPointLight(const Vector3 pos, const Vector3 color, const float range)
{

	Matrix mWV = matView;// �r���[�s��

	// ������J�����O����
	Vector3 flont(mWV._13, mWV._23, mWV._33);
	flont.Normalize();
	Vector3 dir = pos - (ViewPos - flont * range);			//�����W�̒l�������āA�J�����O���ɂ߂�B
	dir.Normalize();

	float dot = Vector3Dot(dir, flont);
	if (dot < .5f) return;

	// ���C�g�ʒu���r���[���W�n�ɕϊ�
	Vector3 PointLight;
	D3DXVECTOR3 plight = D3DXVECTOR3(pos.x, pos.y, pos.z);
	D3DXVec3TransformCoord(&plight, &plight, &mWV);
	PointLight = Vector3(plight.x, plight.y, plight.z);

	// �K�v�p�����[�^�̓]��
	shaderD->SetValue("LightPos", PointLight);
	shaderD->SetValue("LightColor", (Vector3)color);
	shaderD->SetValue("LightRange", range);

	// ���݂̃����_�[�^�[�Q�b�g���ꎞ�I�Ɋm��
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	// �����_�[�^�[�Q�b�g�̐ؑւ�
	light->RenderTarget();
	spec->RenderTarget(1);

	// �����_�����O
	normal->Render(shaderD, "def_SimpliPointLight");

	// �����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, now);
	iexSystem::Device->SetRenderTarget(1, nullptr);
}
// �������C�e�B���O
void Deferred::HemiLight(const Vector3 SkyColor, const Vector3 GroundColor)
{
	//	�V�F�[�_�[�ݒ�
	shaderD->SetValue("SkyColor", (Vector3)SkyColor);
	shaderD->SetValue("GroundColor", (Vector3)GroundColor);

	//���݂̃����_�[�^�[�Q�b�g���ꎞ�I�Ɋm��
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//�����_�[�^�[�Q�b�g�̐ؑւ�
	light->RenderTarget();

	//	�����_�����O
	normal->Render(shaderD, "def_HemiLight");

	//�����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, now);
}


//****************************
///	FORWARD
//****************************

void Deferred::ClearForward()
{
	// �܂��͍��̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// �t�H���[�h���Z�b�g
	forward->RenderTarget(0);
	// �t�H���[�h��ʃN���A
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
}

// G_Buffe�Ώۂ�Z�o�b�t�@���c���Ă����ԂɃZ�b�g���邱��
void Deferred::ForwardBigin()
{
	// �܂��͍��̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// forward�p�T�[�t�F�C�X�ɕς���
	forward->RenderTarget(0);



};
// G_Buffe�Ώۂ�Z�o�b�t�@���c���Ă����ԂɃZ�b�g���邱��
void Deferred::ForwardEnd()
{
	//diffuse�ɖ߂�
	//diffuse->RenderTarget(0);
	//��������e�N�X�`����]��
	shaderD->SetValue("ForwardTex", forward);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
}

void Deferred::ForwardRender()
{
	// forward�ŕ`�悵���G���ォ�炩�Ԃ���
	forward->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);

}

//****************************
///		BLOOM
//****************************
void Deferred::ClearBloom()
{
	// ���̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// Bloom��`�悷�錳��
	bloomScreen->RenderTarget();
	iexSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::BeginDrawBloom()
{
	// �܂��͍��̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	bloomScreen->RenderTarget();//

};
void Deferred::EndDrawBloom()
{
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::BloomRender()
{
	D3DVIEWPORT9 save_viewport;//�r���[�|�[�g���ꎞ�ۑ�
	iexSystem::Device->GetViewport(&save_viewport);

	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	//�e�N�X�`���̃T�C�Y�̃r���[�|�[�g�ݒ�
	D3DVIEWPORT9 v = { 0, 0, MiniTex_x, MiniTex_y, 0, 1.0f };
	iexSystem::Device->SetViewport(&v);

	bloom->RenderTarget();
	iexSystem::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	shaderD->SetValue("TU", (float)1 / (float)MiniTex_x);
	shaderD->SetValue("TV", (float)1 / (float)MiniTex_y);

	// ��ʂ̖��邢�������u���[���̎�Ƃ��Ē��o
	bloomScreen->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "make_hdrBloomSeed");

	//�{�P�x�@�񐔂����Ȃ��قǃ{�P�Ȃ�
	for (int i = 0; i < 2; i++)
	{
		bloom->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, MiniTex_x, MiniTex_y, shaderD, "gaussZ");//���s���֎~
	}

	iexSystem::Device->SetViewport(&save_viewport);
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

	// HDR�u���[������Z�@
	bloom->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, MiniTex_x, MiniTex_y, shaderD, "add_hdrBloom");

};

//****************************
///		GLOW
//****************************
void Deferred::ClearGlow()
{
	// ���̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// glow��`�悷�錳��
	glowScreen->RenderTarget();
	iexSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::BeginDrawGlow()
{
	// �܂��͍��̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	glowScreen->RenderTarget();//
	
};
void Deferred::EndDrawGlow()
{
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);
};
void Deferred::GlowRender()
{
	D3DVIEWPORT9 save_viewport;//�r���[�|�[�g���ꎞ�ۑ�
	iexSystem::Device->GetViewport(&save_viewport);

	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	//�e�N�X�`���̃T�C�Y�̃r���[�|�[�g�ݒ�
	D3DVIEWPORT9 v = { 0, 0, MiniTex_x, MiniTex_y, 0, 1.0f };
	iexSystem::Device->SetViewport(&v);

	glow->RenderTarget();
	iexSystem::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	shaderD->SetValue("TU", (float)1 / (float)MiniTex_x);
	shaderD->SetValue("TV", (float)1 / (float)MiniTex_y);

	glowScreen->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);

	//�{�P�x�@�񐔂����Ȃ��قǃ{�P�Ȃ�
	for (int i = 0; i < 2; i++)
	{
		glow->Render(0, 0, MiniTex_x, MiniTex_y, 0, 0, MiniTex_x, MiniTex_y, shaderD, "gaussZ", 0xFFFFFFFF);//���s���֎~
	}

	iexSystem::Device->SetViewport(&save_viewport);
	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

	//���邳�@�񐔂������قǖ��邭
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
	// SSAO��ǉ�����t���O�𗧂Ă�
	ssaoFlag = true;

	// SSAO�p�̃T�[�t�F�C�X�����
	ssaoScreen = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);	//�@SSAO�̌��ʂ�����
	ssao = new iex2DObj(iexSystem::ScreenWidth, iexSystem::ScreenHeight, IEX2D_RENDERTARGET);		// SSAO�̌��ʂ��ڂ����ŏI�o��

}

void Deferred::ClearSSAO()
{
	// ���̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &savebackbuffer);
	// glow��`�悷�錳��
	ssaoScreen->RenderTarget();
	iexSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	iexSystem::GetDevice()->SetRenderTarget(0, savebackbuffer);

}


void Deferred::SSAORender()
{
	// �ۑ��p�T�[�t�F�C�X
	Surface* save;
	// �܂��͍��̃T�[�t�F�C�X��ۑ�
	iexSystem::GetDevice()->GetRenderTarget(0, &save);

	// SSAO�𓱓�
	if (ssaoFlag == true)
	{
		ssao->RenderTarget();// SSAO�ɐ؂�ւ�
		iexSystem::Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		
		shaderD->SetValue("TU", (float)1 / (float)iexSystem::ScreenWidth);
		shaderD->SetValue("TV", (float)1 / (float)iexSystem::ScreenHeight);

		ssaoScreen->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "ssao");	

		//�{�P�x�@�񐔂����Ȃ��قǃ{�P�Ȃ�
		for (int i = 0; i < 2; i++)
		{
			ssao->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, shaderD, "gaussZ", 0xFFFFFFFF);//���s���֎~
		}

	}
	
	//�@���̃T�[�t�F�C�X�֖߂�
	iexSystem::GetDevice()->SetRenderTarget(0, save);

}

//*****************************************************
///						ShadowMaps
//*****************************************************

// �V���h�E�}�b�v�̏������@
void Deferred::CreateShadowMap(const int size)
{
	//2��ȏ�A�V���h�E�}�b�v�𐶐������Ȃ��B
	if (shadowFlag) return;

	//�V���h�E�}�b�v�̃T�C�Y���m��
	shadowSize = size;

	//�V���h�E�}�b�v�̐���
	shadowMap = new iex2DObj(size, size, IEX2D_FLOAT2);			//r������z�o�b�t�@g������z�o�b�t�@�̓��̒l������

	//�V���h�E�}�b�v�p�X�e���V���o�b�t�@�T�[�t�F�C�X�̍쐬
	if (FAILED(iexSystem::Device->CreateDepthStencilSurface(size, size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &shadowZ, NULL)))
	{
		MessageBox(iexSystem::Window, "�[�x�o�b�t�@���쐬�ł��Ȃ�����", "ERROR", MB_OK);
	};

	shadowFlag = true;	// �V���h�E�}�b�v�̗p
}

void Deferred::CreateShadowMapL(const int size)
{
	//�V���h�E�}�b�vL�̃T�C�Y���m��
	shadowSizeL = size;
	//�u���[�����V���h�E�}�b�v�̐���
	softShadowMap = new iex2DObj(size, size, IEX2D_FLOAT2);

	shadowMapL = new iex2DObj(size, size, IEX2D_FLOAT2);

	cascadeFlag = true;	// ��d�V���h�E�}�b�v�̗p
}

//�@�V���h�E�̍X�V
void Deferred::CreateShadowMatrix(Vector3 dir, Vector3 target, Vector3 playerVec, const float dist)
{

#ifdef _DEBUG
	//�V���h�E�}�b�v����������Ă��Ȃ���Γ]�����Ȃ�
	if (!shadowFlag){
		//OutputDebugString("Don't create shadowmap!!\n"); _PLACE_; return;
		MessageBox(iexSystem::Window, "Don't create shadowmap!!", __FUNCTION__, MB_OK);
		exit(-1);
	}

#endif

	
	
	//	�V���h�E�쐬
	dir.Normalize();
	Vector3 pos = target - dir * dist;
	Vector3 up(.0f, 1.0f, .0f);

	//���̎w��
	float width = dist;

	//	���_�ƃ��C�g�ʒu��
	Matrix	ShadowMat, work;
	// �r���[�s��̐ݒ�
	LookAtLH(ShadowMat, pos + (playerVec * 90), target + (playerVec * 90), up);
	// ���s���e�s��쐬 �r���[�{�����[��
	OlthoLH(work, width, width, 0.02f, width * 2.5f);	//���s���e�s��
	ShadowMat *= work;
	// ��������
	shaderD->SetValue("ShadowProjection", ShadowMat);

	// �A���t�@����
	shadowRange = width * .25f;

	//cascade����Ȃ炻�����̍s����쐬����
	if (!cascadeFlag) return;
	pos = target  - dir * dist * 3.0f;			//�ߋ����V���h�E�ɔ��3�{�قǗ���
	up = Vector3(.0f, 1.0f, .0f);

	D3DXMatrixIdentity(&ShadowMat);
	D3DXMatrixIdentity(&work);
	LookAtLH(ShadowMat, pos + (playerVec * 210), target + (playerVec * 210), up);
	OlthoLH(work, width * 3.0f, width * 3.0f, 0.02f, width * 7.5f);
	ShadowMat *= work;

	shaderD->SetValue("ShadowProjectionL", ShadowMat);
}

//�@�V���h�E�}�b�v�p�̕`��J�n
void Deferred::ShadowBegin()
{
#ifdef _DEBUG
	// �V���h�E�}�b�v����������Ă��Ȃ���Ώ����Ȃ�
	if (!shadowFlag){
		//OutputDebugString("Don't create shadowmap!!\n"); _PLACE_; return;
		MessageBox(iexSystem::Window, "Don't create shadowmap!!", __FUNCTION__, MB_OK);
		exit(-1);
	}
#endif


	// ���݂̃T�[�t�F�C�X���ꎞ�ۊ�
	iexSystem::Device->GetRenderTarget(0, &backbuffer);
	
	// ���݂̃X�e���V���o�b�t�@���ꎞ�ۊ�
	iexSystem::Device->GetDepthStencilSurface(&backbufferZ);
	// �V���h�E�}�b�v�p�X�e���V���o�b�t�@�ɐ؂�ւ�
	iexSystem::Device->SetDepthStencilSurface(shadowZ);

	// ���݂̃r���[�|�[�g�̈ꎞ�ۊ�
	iexSystem::Device->GetViewport(&orgViewport);
	// �r���[�|�[�g�̍쐬
	D3DVIEWPORT9 vp = { 0, 0, shadowSize, shadowSize, 0, 1.0f };
	iexSystem::Device->SetViewport(&vp);

	// �V���h�E�}�b�v�̃����_�[�X�^�[�g
	shadowMap->RenderTarget();

	//�@��ʃN���A
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// ���̊֐����I������e��`�悵�����Ώۂ�`��
}
//�@�V���h�E�}�b�v�p�̕`��J�n_L
void Deferred::ShadowBeginL()
{
#ifdef _DEBUG
	// �V���h�E�}�b�v����������Ă��Ȃ���Ώ����Ȃ�
	if (!cascadeFlag){
		//OutputDebugString("Don't create shadowmap!!\n"); _PLACE_; return;
		MessageBox(iexSystem::Window, "Don't create cascadeFlag!!", __FUNCTION__, MB_OK);
		exit(-1);
	}

#endif

	// ���݂̃T�[�t�F�C�X���ꎞ�ۊ�
	iexSystem::Device->GetRenderTarget(0, &backbuffer);

	// ���݂̃X�e���V���o�b�t�@���ꎞ�ۊ�
	iexSystem::Device->GetDepthStencilSurface(&backbufferZ);
	// �V���h�E�}�b�v�p�X�e���V���o�b�t�@�ɐ؂�ւ�
	iexSystem::Device->SetDepthStencilSurface(shadowZ);

	// ���݂̃r���[�|�[�g�̈ꎞ�ۊ�
	iexSystem::Device->GetViewport(&orgViewport);
	// �r���[�|�[�g�̍쐬
	D3DVIEWPORT9 vp = { 0, 0, shadowSizeL, shadowSizeL, 0, 1.0f };
	iexSystem::Device->SetViewport(&vp);

	// �V���h�E�}�b�vL�̃����_�[�X�^�[�g
	shadowMapL->RenderTarget();

	//�@��ʃN���A
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// ���̊֐����I������e��`�悵�����Ώۂ�`��i�������j
}

//�@�V���h�E�}�b�v�p�̕`��I��
void Deferred::ShadowEnd()
{
	//�����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	//�X�e���V���o�b�t�@�̕���
	iexSystem::Device->SetDepthStencilSurface(backbufferZ);

	//�r���[�|�[�g�̕���
	iexSystem::Device->SetViewport(&orgViewport);
}
//�@�V���h�E�}�b�v�p�̕`��I��_L
void Deferred::ShadowEndL()
{

	// �\�t�g�V���h�E�}�b�v�ɃV���h�E�}�b�v�̌��ʂ���������
	/*_____________________________________________________________________________*/
	softShadowMap->RenderTarget();
	shaderD->SetValue("TU", 1.0f / shadowSizeL);
	shaderD->SetValue("TV", 1.0f / shadowSizeL);

	// �V���h�E�}�b�v�̉𑜓x�Ńu���[������ω�	
	shadowMapL->Render(0, 0, shadowSizeL, shadowSizeL, 0, 0, shadowSizeL, shadowSizeL, shaderD, "gaussZ");//���s���֎~

	/*_____________________________________________________________________________*/

	// �����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	// �X�e���V���o�b�t�@�̕���
	iexSystem::Device->SetDepthStencilSurface(backbufferZ);

	///�r���[�|�[�g�̕���
	iexSystem::Device->SetViewport(&orgViewport);
}

// �V���h�E�̕`��
void Deferred::RenderShadow()
{
	//�V�F�[�_�[�֓]��
	shaderD->SetValue("DepthTex", depth);			// ���s�p�̃T�[�t�F�C�X�𑗂�
	shaderD->SetValue("ShadowMap",shadowMap);
	//shaderD->SetValue("ShadowMap", softShadowMap);	// �\�t�g�V���h�E�̃T�[�t�F�C�X���V���h�E�}�b�v�Ƃ��đ���
	shaderD->SetValue("ShadowRange", shadowRange);

	shadow->RenderTarget();

	//��ʃN���A
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0);

	// �e�ɂ��X�y�L��������߂邽��2�Ԗڂɐݒ�
	spec->RenderTarget(1);

	// �V���h�E�}�b�v�����p���Ă��邩�̔���
	if (!cascadeFlag)
	{
		shadow->Render(shaderD, "shadow");
	}
	else 
	{
		shaderD->SetValue("ShadowMapL", softShadowMap);// �������}�b�v�𑗂�
		shadow->Render(shaderD, "DualShadow");
	}

	// �v�Z��������e����ł���߂�׃V�F�[�_�[��
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
/// �`��
void Deferred::Render(const int outX, const int outY, const int W, const int H, const int inX, const int inY)
{
	//���݂̃����_�[�^�[�Q�b�g���ꎞ�I�Ɋm��
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//�V�F�[�_�[�֓]��
	shaderD->SetValue("LightTex", light);
	shaderD->SetValue("SpecTex", spec);

	// �ʏ�`��
	DefaultRender();

	//�����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, now);

	// FinalPassSurface���ڂ�
	screen->Render(outX, outY, W, H, 0, 0, inX, inY,shaderD,"ToneMap");
	
}

// ��ʊE�[�x
void Deferred::RenderDOF(const Vector3 target, const float range)
{
	//���݂̃����_�[�^�[�Q�b�g���ꎞ�I�Ɋm��
	Surface* now = nullptr;
	iexSystem::Device->GetRenderTarget(0, &now);

	//********************************************
	///		�ŏI����screen�ɔ�ʊE�[�x��������
	//********************************************
	dof->RenderTarget();

	//target���r���[���W�n�ɕϊ�����
	Matrix mWV = matView;
	D3DXVECTOR3 Vt(target.x, target.y, target.z);
	D3DXVec3TransformCoord(&Vt, &Vt, &mWV);
	Vector3 Target(Vt.x, Vt.y, Vt.z);

	//��ʃN���A		���͔����Ă���
	iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	//�V�F�[�_�[�֕K�v�p�����[�^��]��
	shaderD->SetValue("DOF_target", Target);
	shaderD->SetValue("DOF_Range", range);

	// ToneMap+�߂����̃A���t�@��؂��鏈�� 
	screen->Render(0, 0, Reduction_x, Reduction_y, 0, 0, 1280, 720, shaderD, "ToneMap_DOF");	

	//********************************************
	///		���ō쐬�����C���[�W���ڂ���
	//********************************************

	//�V�F�[�_�[��1�e�N�Z���̒l��]��
	shaderD->SetValue("TU", 1.0f / (float)Reduction_x);
	shaderD->SetValue("TV", 1.0f / (float)Reduction_y);

	//�����_�[�^�[�Q�b�g�̕���
	iexSystem::Device->SetRenderTarget(0, now);
	
	//��ʃN���A
	//iexSystem::GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

	//�k���o�b�t�@���g�p�����̂ň������΂�
	dof->Render(0, 0, 1280, 720, 0, 0, Reduction_x, Reduction_y, shaderD, "gaussian");


}

/// �f�t�H���g�@���s���{�X�y�L����
void Deferred::DefaultRender()
{
	// �V���h�E�T�[�t�F�C�X�̍X�V
	if (shadowFlag==true)
	{
		RenderShadow();
	}

	// FinalPassSurface�ɍ��܂ł�Surface�̏���S�č��킹��
	screen->RenderTarget();

	// ��ʃN���A
	// ����������S�ĕ`�悷�邽�ߖ��Ȃ������Ȃ̂ŃR�����g�A�E�g
	//iexSystem::GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	if (shadowFlag == true)
	{
		if (ssaoFlag==true){
			shaderD->SetValue("SSAOTex", ssao);
			diffuse->Render(shaderD, "def_mix3");
		}
		else
		{
			diffuse->Render(shaderD, "def_mix1");		//�ʏ�`��
			//diffuse->Render();
		}
		
	}
	else
	{
		diffuse->Render(shaderD, "def_mix2");		//�e�Ȃ��`��
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
			MessageBox(iexSystem::Window, "SSAO�}�b�v������Ă��Ȃ�", __FUNCTION__, MB_OK);
		ret = ssaoScreen;
		break;
	case SSAO:
		if (!ssaoFlag)
		MessageBox(iexSystem::Window, "SSAO�}�b�v������Ă��Ȃ�", __FUNCTION__, MB_OK);
		ret = ssao;
		break;
	case SCREEN:
		ret = screen;
		break;
	}

	return ret;
}