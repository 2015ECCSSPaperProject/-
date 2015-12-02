#pragma once
#include "iextreme.h"
#include	"../system/system.h"

/*
*	Deferred
*/

// �T�[�t�F�C�X���Y��Ƀ����[�X
#define ReleaseSurface(p) { if(p != nullptr) delete p; p = nullptr;}

//�@�V���h�E�}�b�v�̃T�C�Y
#define SHADOWSIZE 2048

// �ɏ��o�b�t�@�̃T�C�Y
static const int Reduction_lvl = 4;
static const int Reduction_x = 1280 / Reduction_lvl, Reduction_y = 720 / Reduction_lvl;

static const int MiniTex_lvl = 8;
static const int MiniTex_x = 1280 / MiniTex_lvl, MiniTex_y = 720 / MiniTex_lvl;

enum SURFACE_NAME{
	DIFFUSE, NORMAL, SPECULAR, DEPTH,		//G-Buffer
	LIGHT, SPEC, DOF, SHADOW, SHADOWMAP, SHADOWMAPL,
	BLOOMSCREEN, BLOOM, GLOWSCREEN, GLOW, FORWARD, SSAOSCREEN, SSAO,
	SCREEN
};

class Deferred
{
private:
	//�������֐�
	void Init();

	Deferred();
	~Deferred();
	static Deferred* pInstance;
public:
	// Instance
	static Deferred& GetInstance();
	static void Release();

	//***************************
	//		Deferred_Set
	//***************************

	void Bigin();
	void End();
	void Update(const Vector3 ViewPos);

	//***************************
	//		Lighting
	//***************************
	// ���s��
	void DirLight(const Vector3 dir, const Vector3 color);
	// �|�C���g���C�g
	void PointLight(const Vector3 pos, const Vector3 color, const float range = 100.0f, const float power = 3.0f);
	
	// �y�ʃ|�C���g���C�g
	void SimpliPointLight(const Vector3 pos, const Vector3 color, const float range = 100.0f);

	// �������C�e�B���O
	void HemiLight(const Vector3 SkyColor, const Vector3 GroundColor);

	//****************************
	//		ShadowMaps
	//****************************
	void CreateShadowMap(const int size = SHADOWSIZE);
	void CreateShadowMapL(const int size = SHADOWSIZE);
	void CreateShadowMatrix(Vector3 dir, Vector3 target,Vector3 playerVec, const float dist = 100.0f);
	void CreateShadowMatrixL(Vector3 dir, Vector3 target, Vector3 playerVec, const float dist = 100.0f);
	void ShadowBegin();
	void ShadowEnd();
	void ShadowBeginL();
	void ShadowEndL();
	void ShadowSetting(const float ShadowStrength, const float AdjustValue, const float DistAlpha = .75f);

	//****************************
	//		FORWARD
	//****************************
	// G_Buffe�Ώۂ�Z�o�b�t�@���c���Ă����Ԃŕ`�悷�邱��
	void ClearForward();
	void ForwardBigin();
	void ForwardEnd();
	void ForwardRender();

	//****************************
	//		BLOOM
	//****************************
	void ClearBloom();
	void BeginDrawBloom();
	void EndDrawBloom();
	void BloomRender();

	//****************************
	//		GLOW
	//****************************
	void ClearGlow();
	void BeginDrawGlow();
	void EndDrawGlow();
	void GlowRender();

	//****************************
	//		SSAO
	//****************************
	void CreateSSAO();
	void ClearSSAO();
	void SSAORender();
	void BeginDrawSSAO();
	void EndDrawSSAO();

	//****************************
	//		Deferredshading
	//****************************
	void Render(const int outX = 0, const int outY = 0, const int W = iexSystem::ScreenWidth, const int H = iexSystem::ScreenHeight, const int inX = iexSystem::ScreenWidth, const int inY = iexSystem::ScreenHeight);
	void RenderDOF(const Vector3 target, const float range);
	void RenderShadow();// 
	
	void DefaultRender();// �f�t�H���g

	//�Q�b�^�[�E�Z�b�^�[
	iex2DObj* GetTex(const int type); //�e�N�X�`��������Ă���

	bool GetShadowFlag(){ return shadowFlag; }	//�V���h�E�}�b�v���̗p���Ă���̂�
	bool GetCascadeFlag(){ return cascadeFlag; }

private:

	// G-Buffer�@MRT (�}���`�����_�[�^�[�Q�b�g)
	iex2DObj* diffuse;
	iex2DObj* normal;
	iex2DObj* specular;
	iex2DObj* depth;

	// G-Buffer�𗘗p����SecondSurface
	iex2DObj* light;			// ���C�g�p�T�[�t�F�C�X	
	iex2DObj* spec;				// �X�y�L�����p�T�[�t�F�C�X
	iex2DObj* dof;				// ��ʊE�[�x�p�T�[�t�F�C�X
	iex2DObj* shadow;			// �V���h�E�p�T�[�t�F�C�X

	// FinalPassSurface
	iex2DObj* screen;			// ���Z���ʂ��܂Ƃ߂��T�[�t�F�C�X	
	iex2DObj* bloomScreen;		// HDR��������X�N���[��
	iex2DObj* bloom;			// HDR
	iex2DObj* glowScreen;		// GLOW��������X�N���[��
	iex2DObj* glow;				// GLOW
	iex2DObj* ssaoScreen;		// SSAO��������X�N���[��
	iex2DObj* ssao;				// SSAO�p�T�[�t�F�C�X

	// ForwardSurface
	iex2DObj* forward;			// deferred�Œʂ������ڕ`�悷��T�[�t�F�C�X

	// BackBufferSurface 
	// �ŏI�I�ɉ�ʂɉf���T�[�t�F�C�X
	Surface* backbuffer;
	// �o�b�N�X�e���V���o�b�t�@�T�[�t�F�C�X
	Surface* backbufferZ;
	// �ۑ��p�T�[�t�F�C�X
	Surface* savebackbuffer;


	// �J�����̈ʒu
	Vector3 ViewPos;

	// ���}�b�v
	iex2DObj* EnvMap;

	// SSAO���g�p���邩�̃t���O
	bool ssaoFlag;

	//****************************
	//		ShadowMap
	//****************************
	//�V���h�E�}�b�v�������ݎ��X�e���V���o�b�t�@�T�[�t�F�C�X
	Surface*		shadowZ;
	Surface*		orgZ;		// Z�o�b�t�@�ۑ��p
	D3DVIEWPORT9	orgViewport;// �r���[�|�[�g�ۑ��p

	iex2DObj*	shadowMap;		// ���l�e�N�X�`��
	iex2DObj*	shadowMapL;		// �������p�}�b�v
	iex2DObj*	softShadowMap;	// �\�t�g�V���h�E�}�b�v
	
	int		shadowSize;			// �V���h�E�}�b�v�̃T�C�Y
	int		shadowSizeL;		// �V���h�E�}�b�v�̃T�C�YL
	float	shadowRange;		// Shadow�̃J��������̋���

	//�V���h�E�}�b�v�̎g�p�t���O
	bool shadowFlag;
	bool cascadeFlag;


};

#define DeferredManager (Deferred::GetInstance())