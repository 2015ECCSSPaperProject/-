#include	"iextreme.h"

#include	"../system/Framework.h"
#include	"SceneTitle.h"
#include	"SceneSelect.h"
#include	"SceneMakePoster.h"

#include "../stage/Stage.h"


#include	"../../IEX/OKB.h"

#include "../Animation/AnimationRippleEx.h"


using namespace std;

static const Vector2 MOUSE_POS(960, 454);

//******************************************************************
//		�������E���
//******************************************************************
bool SceneTitle::Initialize()
{
	// �f�B�t�@�[�h
	LightVec = Vector3(1, 1, 1);
	FarShadowTimer = 0;
	DeferredManager;
	DeferredManager.CreateShadowMap(2048);
	DeferredManager.CreateShadowMapL(1024);
	DeferredManager.SetEmissiveRate(1000);// �G�~�b�V�u���[�g
	DeferredManager.SetExposure(-10);
	// �ڂ̃V���~���[�g���ʂ𑗂�
	shaderD->SetValue("exposure", DeferredManager.GetExposure());

	// �e
	// �ǉ�
	float ShadowFar = 200.0f;	// �ߋ����̉e�����炩�ɓ���ւ����
	float ShadowNear = 150.0f;	// �ߋ����̉e�����炩�ɓ���ւ����
	shaderD->SetValue("ShadowFar", ShadowFar);
	shaderD->SetValue("ShadowNear", ShadowNear);
	float AdjustValue = -0.001f;
	shaderD->SetValue("AdjustValue", AdjustValue);

	stage = new Stage;
	stage->Initialize();

	sky = new iexMesh("DATA/Skydome/Skydome.IMO");
	sky->SetScale(5.0f);
	sky->SetPos(0, -100, 0);
	sky->Update();

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN_W, 22);

	view = new iexView();
	viewPos = Vector3(0, 10, -90);
	viewTarget = Vector3(0, 20, 0);
	view->Set(viewPos, viewTarget);

	images[IMAGE::BACK] = new iex2DObj("DATA/Image/title/title_back.png");
	images[IMAGE::CLICK1] = new iex2DObj("DATA/Image/title/click.png");
	images[IMAGE::CLICK2] = new iex2DObj("DATA/Image/title/click2.png");
	images[IMAGE::CLICK3] = new iex2DObj("DATA/Image/title/click3.png");
	images[IMAGE::GAME_START] = new iex2DObj("DATA/Image/title/gamestart.png");
	images[IMAGE::MOUSE] = new iex2DObj("DATA/Image/title/mouse.png");
	images[IMAGE::TITLE] = new iex2DObj("DATA/Image/title/title.png");
	images[IMAGE::CURSOR] = new iex2DObj("DATA/makePoster/cursor4.png");
	images[IMAGE::ARROW] = new iex2DObj("DATA/Image/title/Arrow.png");
	images[IMAGE::ICON] = new iex2DObj("DATA/Image/title/cursor_mini.png");
	//---------------------------------------------------------------------
	// ImageAnimation
	//---------------------------------------------------------------------
	arrowPosY = 0;
	arrowMoveY = 0;

	// �^�C�g���̊G
	titleEx = new AnimationRippleEx("DATA/Image/title/title.png",
		12, 10, 12, 3.0f, -(2.5f / 12.0f), true);
	titleEx->Action();

	start_button.pos = Vector3(42.7f, 13.4f, 0);
	Texture2D *texture = iexTexture::Load("DATA/Image/title/gamestart.png");
	start_button.obj = new iex3DObj("DATA/paper object/Poster/posuta-.IEM");
	start_button.obj->SetTexture(texture, 0);
	start_button.obj->SetAngle(0,PI,PI*-.5f);
	start_button.obj->SetScale(1.0f);
	start_button.obj->SetPos(start_button.pos);
	start_button.obj->SetMotion(0);

	start_button.rend = false;
	start_button.pointing = false;

	step = STEP::WAIT;

	move_mouse = MOUSE_POS;

	return true;
}

SceneTitle::~SceneTitle()
{
	// deferred�̉���
	DeferredManager.Release();

	delete stage;
	delete sky;
	delete titleEx;

	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)
	{
		delete images[i];
	}
	delete start_button.obj;
}


//******************************************************************
//		����
//******************************************************************

void SceneTitle::Update()
{
	static float ANGLE = 2.5f;
	if ((GetKeyState('U') & 0x80))ANGLE += 0.05f;
	if ((GetKeyState('I') & 0x80))ANGLE -= 0.05f;
	LightVec = Vector3(sinf(ANGLE), -0.98f, cosf(ANGLE));
	LightVec.Normalize();

	//�@��̏���
	static float skyAngle = 0;
	skyAngle += 0.0007f;
	sky->SetAngle(skyAngle);
	sky->Update();

	// �J����
	static float angle = 0.0f;
	angle += 0.001f;
	Vector3 vecAngle;
	vecAngle.x = sinf(angle);
	vecAngle.z = cosf(angle);
	vecAngle.y = .0;

	static Vector3 cameraPos = VECTOR_ZERO;

	if (KEY_Get(KEY_UP)){
		cameraPos.x += sinf(angle) * 2; cameraPos.z += cosf(angle) * 2;
	}
	if (KEY_Get(KEY_RIGHT))angle += 0.05f;
	if (KEY_Get(KEY_LEFT))angle -= 0.05f;
	if (KEY_Get(KEY_DOWN)){
		cameraPos.x -= sinf(angle); cameraPos.z -= cosf(angle);
	}
	viewPos = cameraPos+ Vector3(0, 10, 0) - vecAngle * 90;
	viewTarget = cameraPos + Vector3(0, 20, 0);

	view->Set(viewPos, viewTarget);


	mouse.Update();

	// �C���[�W�̃A�j���[�V����
	if (arrowPosY >= 0)
	{
		arrowMoveY = -1;
	}
	else if (arrowPosY <= -20)
	{
		arrowMoveY = 1;
	}
	// �₶�邵�X�V
	arrowPosY += arrowMoveY;


	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();


	// �^�C�g��EX
	if (KEY_Get(KEY_SPACE) == 3){
		titleEx->Action();
	}
	titleEx->Update();

	//if (KeyBoard(KB_A)) start_button.pos.x -= .1f;
	//if (KeyBoard(KB_W)) start_button.pos.y += .1f;
	//if (KeyBoard(KB_S)) start_button.pos.y -= .1f;
	//if (KeyBoard(KB_D)) start_button.pos.x += .1f;

	start_button.obj->SetPos(start_button.pos);

	start_button.pointing = (
		mouse.pos_x >= min_v.x && mouse.pos_x <= max_v.x&&
		mouse.pos_y >= min_v.y && mouse.pos_y <= max_v.y);
	switch (step)
	{
	case STEP::WAIT:
		if ((KeyBoard(MOUSE_LEFT) && start_button.pointing))
		{
			step = STEP::CLICK;
			start_button.obj->SetMotion(1);
			start_button.rend = true;
		}
		break;

	case STEP::CLICK:
		if (!KeyBoard(MOUSE_LEFT))	// �}�E�X����
		{
			move_mouse = MOUSE_POS;
			step = STEP::WAIT;
		}
		else if (mouse.Get_move_dist() > 20)
		{
			move_mouse = MOUSE_POS;
			step = STEP::DRAG;
		}

		{
			float move_x, move_y;
			Vector2 next_vec(1180 - move_mouse.x, 600 - move_mouse.y);
			if (next_vec.Length() < 4)
			{
				move_mouse = MOUSE_POS;
			}
			else
			{
				next_vec.Normalize(move_x, move_y);
				move_mouse.x += (int)(move_x * 4);
				move_mouse.y += (int)(move_y * 4);
			}
		}
		break;
		
	case STEP::DRAG:
		start_button.obj->SetMotion(1);
		start_button.rend = true;
		step = STEP::REND_PAPER;
		break;

	case STEP::REND_PAPER:
		start_button.obj->Animation();

		if (start_button.obj->GetFrame() >= 47) MainFrame->ChangeScene(new SceneSelect());
		return;
		break;
	}


	//�@debug
	if (KEY(KEY_ENTER) == 3)
	{
		MainFrame->ChangeScene(new SceneSelect());
		return;
	}
}



//******************************************************************
//		�`��
//******************************************************************

void SceneTitle::Render()
{
	view->Activate();
	view->Clear(0xff666666);
	
	// �f�B�t�@�[�h�Z�b�g�A�b�v
	DeferredManager.Update(viewPos);

	// �e�`��
	RenderShadow();

	//�@�`��N���A
	DeferredManager.ClearBloom();
	DeferredManager.ClearForward();
	DeferredManager.ClearAllRender();

	DeferredManager.G_Bigin();
	// �X�e�[�W�z�u
	stage->Render(shaderD, "G_Buffer");
	sky->Render(shaderD, "G_Buffer");
	DeferredManager.G_End();// �����܂�

	/*	G_buffer�𗘗p�����`��	*/
	DeferredManager.DirLight(LightVec, Vector3(642.5f, 640.0f, 640.0f));
	DeferredManager.HemiLight(Vector3(310.5f, 300.0f, 300.0f), Vector3(320.5f, 300.0f, 300.0f));
	DeferredManager.Fog(800, 2000, Vector3(200, 170, 200));
	DeferredManager.FinalResultDeferred(); // �Ō�ɂ܂Ƃ߂�

	/*�t�@�C�i�����U���g*/
	DeferredManager.RenderDeferred();

	// �t�H�A�[�h
	DeferredManager.ForwardBigin();
	start_button.obj->Update();
	start_button.obj->Render();
	DeferredManager.ForwardEnd();
	DeferredManager.ForwardRender();
	
	// �u���[��
	DeferredManager.BeginDrawBloom();
	DeferredManager.GetTex(SURFACE_NAME::SCREEN)->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight,
		0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight);
	DeferredManager.EndDrawBloom();
	DeferredManager.BloomRender();



	// ���c��������
	//images[IMAGE::BACK]->Render(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720, RS_COPY, 0xffffffff, 1.0f);
	//images[IMAGE::BACK]->RenderBack(0, 0, iexSystem::ScreenWidth, iexSystem::ScreenHeight, 0, 0, 1280, 720, RS_COPY);

	
	images[IMAGE::ARROW]->Render(1025, 364 + arrowPosY);				//	�₶�邵�҂��
	images[IMAGE::CLICK1]->Render(962, 520, 256, 64, 0, 0, 256, 64);

	if ((int)step >= (int)STEP::CLICK)
	{
		images[IMAGE::CLICK2]->Render(962, 520, 256, 64, 0, 0, 256, 64);
	}
	if ((int)step >= (int)STEP::DRAG)
	{
		images[IMAGE::CLICK3]->Render(962, 520, 256, 64, 0, 0, 256, 64);
	}
	if ((int)step == (int)STEP::WAIT)
	{
		images[IMAGE::MOUSE]->Render(MOUSE_POS.x, MOUSE_POS.y + (int)(arrowPosY*.5f), 64, 64, 0, 0, 64, 64);
	}
	else if ((int)step < (int)STEP::DRAG)
	{
		images[IMAGE::MOUSE]->Render(move_mouse.x, move_mouse.y, 64, 64, 0, 0, 64, 64);
	}

	// �^�C�g���G
	//images[IMAGE::TITLE]->Render(128, 8, 1024, 512, 0, 0, 1024, 512);
	// �A�j���[�V�����^�C�g��Ex
	titleEx->Render(128, 8, RS_COPY);

	// ��̃A�C�R��
	bool iconFlag = false;
	if (KeyBoard(MOUSE_LEFT))iconFlag = true;	// �}�E�X����
	images[IMAGE::ICON]->Render(mouse.pos_x - 32, mouse.pos_y - 32, 64, 64, iconFlag * 64, 0, 64, 64);


	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	//FadeControl::Render();

	//if(start_button.pointing)iexPolygon::Rect(min_v.x, min_v.y, max_v.x-min_v.x, max_v.y-min_v.y, RS_COPY, 0x99fffff);

#ifdef _DEBUG

	if ((GetKeyState('R') & 0x80))
		DeferredManager.GetTex(SURFACE_NAME::SHADOWMAP)->Render(0, 0, 1024/2, 1024/2, 0, 0, 1024, 1024);
	if ((GetKeyState('T') & 0x80))
		DeferredManager.GetTex(SURFACE_NAME::SHADOWMAPL)->Render(0, 0, 2048/2, 2048/2, 0, 0, 2048, 2048);
	//if ((GetKeyState('Y') & 0x80))DeferredManager.AddExposure(1.0f);
	//if ((GetKeyState('U') & 0x80))DeferredManager.AddExposure(-1.0f);

#endif

	Text::Draw(32, 32, 0xff000000, "%d", mouse.pos_x);
	Text::Draw(32, 64, 0xff000000, "%d", mouse.pos_y);
	//Text::Draw(32, 96, 0xff000000, "%.1f", start_button.pos.z);
}

void SceneTitle::RenderShadow()
{
	// �ߋ���
	// �e�p�v���W�F�N�V�����̍X�V
	Vector3 flontVec;
	flontVec = viewTarget - viewPos;
	flontVec.Normalize();

	DeferredManager.CreateShadowMatrix(LightVec, viewPos, flontVec * 30 , 450);
	// near
	DeferredManager.ShadowBegin();

	stage->Render(shaderD, "ShadowBuf");

	DeferredManager.ShadowEnd();// end

	// ������
	if (DeferredManager.GetCascadeFlag())
	{
		if (FarShadowTimer <= 600)
		{
			// �������v���W�F�N�V�����̍X�V
			DeferredManager.CreateShadowMatrixL(LightVec, viewPos, flontVec * 10, 650);
			// far
			DeferredManager.ShadowBeginL();

			stage->Render(shaderD, "ShadowBufL");

			DeferredManager.ShadowEndL();// end

			FarShadowTimer++;
		}
	}

}



//=============================================================================================
//		�}	�E	�X	�X		�V
void Mouse::Update()
{
	static const float Max = 0.6f;
	static const float Min = 0.25f;

	// �O����W�ۑ�
	prev_point = current_point;

	// �擾
	GetCursorPos(&current_point);
	GetWindowRect(iexSystem::Window, &rc);
	// ���ʒu�Ɖ��ɂ�钲��
	pos_x = current_point.x - rc.left - 8;
	pos_y = current_point.y - rc.top - 29;
	////�����I�t�Z�b�g�����K��
	axis_x = ((float)pos_x - (iexSystem::ScreenWidth / 2)) / (iexSystem::ScreenWidth / 2);
	axis_y = -((float)pos_y - (iexSystem::ScreenHeight / 2)) / (iexSystem::ScreenHeight / 2);

	// �ő�l����
	if (axis_x > Max)axis_x = Max;
	else if (axis_x < -Max)axis_x = -Max;
	if (axis_y > Max)axis_y = Max;
	else if (axis_y < -Max)axis_y = -Max;

	// �ŏ��l�Ȃ�0
	float val_x = sqrtf(axis_x * axis_x), val_y = sqrtf(axis_y * axis_y);
	if (val_x < Min) axis_x = 0;
	if (val_y < Min * 1.5f) axis_y = 0;
}
