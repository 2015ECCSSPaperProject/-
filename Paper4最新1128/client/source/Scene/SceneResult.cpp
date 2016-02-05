
#include	"iextreme.h"

//net�͈�ԏ��߂ɃC���N���[�h���Ȃ��ƃG���[���N����
#include	"../Net/Socket.h"
//#include	"../Net/net_config_loader.h"
#include "../Animation/AnimationRippleEx.h"


#include	"../system/Framework.h"
#include	"../system/Thread.h"

#include	"SceneResult.h"
#include	"SceneSelect.h"
#include	"../score/Score.h"
#include	"../sound/SoundManager.h"
#include "../../IEX/OKB.h"

//#include	"../score/Score.h"
//#include	"../data/LimitedData.h"

/**********************/
/*	�O���[�o���ϐ�	�@*/
/**********************/

// ���U���g���̓T�[�o�[�����Ă�̂łǂ����l�b�g���Ȃ����Ă鎞�ɒl�����Ă���
int result_my_number;

//�@�X���b�h���~�߂�ׂɉ��ō����
//static int ThreadEND = false;


using namespace std;

//******************************************************************
//		�������E���
//******************************************************************

int active(0);
//int PlayerMAX = 0;

bool SceneResult::Initialize()
{
	active = 0;
	int count(0);
	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
			++active;
	}
	//PlayerMAX = 6 - active;

	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN_W, 16);

	view = new iexView;
	view->Set(Vector3(0, 0, -8.5), Vector3(0, 0, 0));
	view->SetProjection(PI / 6, .1f, 500);

	image[IMAGE::R1] = new iex2DObj("DATA/Image/result/rank1.png");
	image[IMAGE::R2] = new iex2DObj("DATA/Image/result/rank2.png");
	image[IMAGE::R3] = new iex2DObj("DATA/Image/result/rank3.png");
	image[IMAGE::R4] = new iex2DObj("DATA/Image/result/rank4.png");
	image[IMAGE::R5] = new iex2DObj("DATA/Image/result/rank5.png");
	image[IMAGE::R6] = new iex2DObj("DATA/Image/result/rank6.png");
	image[IMAGE::P1] = new iex2DObj("DATA/Image/lobby/red.png");
	image[IMAGE::P2] = new iex2DObj("DATA/Image/lobby/blue.png");
	image[IMAGE::P3] = new iex2DObj("DATA/Image/lobby/yellow.png");
	image[IMAGE::P4] = new iex2DObj("DATA/Image/lobby/green.png");
	image[IMAGE::P5] = new iex2DObj("DATA/Image/lobby/purple.png");
	image[IMAGE::P6] = new iex2DObj("DATA/Image/lobby/pink.png");
	image[IMAGE::BACK] = new iex2DObj("DATA/Image/result/back2.png");
	image[IMAGE::WIN_BACK] = new iex2DObj("DATA/Image/result/orange.png");
	image[IMAGE::LOSE_BACK] = new iex2DObj("DATA/Image/result/blue.png");
	image[IMAGE::NORMAL_BACK] = new iex2DObj("DATA/Image/result/green.png");	
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/1.png");
	image[IMAGE::KEKKA] = new iex2DObj("DATA/Image/result/result.png");
	image[IMAGE::NUMBER] = new iex2DObj("DATA/UI/Num.png");
	image[IMAGE::OME] = new iex2DObj("DATA/Image/result/ome.png");	
	// Move�l��
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		MoveX[i] = 1280;
	}

	// ��
	//score_mng->Add(114, 1);
	//score_mng->Add(51, 3);
	//score_mng->Add(4, 5);

	Set_ranking();

	// �L�����N�^�[
	chara.pos = Vector3(19, 10, 0);
	chara.angle = PI;
	Texture2D *texture;
	switch (result_my_number)
	{
	case 1:
		texture = iexTexture::Load("DATA/CHR/player/player_blue.png");
		break;
	case 2:
		texture = iexTexture::Load("DATA/CHR/player/player_yellow.png");
		break;
	case 3:
		texture = iexTexture::Load("DATA/CHR/player/player_green.png");
		break;
	case 4:
		texture = iexTexture::Load("DATA/CHR/player/player_purple.png");
		break;
	case 5:
		texture = iexTexture::Load("DATA/CHR/player/player_pink.png");
		break;
	default:
		texture = iexTexture::Load("DATA/CHR/player/player_red.png");
		break;
	}

	chara.obj = new iex3DObj("DATA/CHR/player/result.IEM");
	chara.obj->SetTexture(texture, 0);
	chara.obj->SetAngle(chara.angle);
	chara.obj->SetScale(1.0f);
	chara.obj->SetPos(chara.pos);
	chara.obj->SetMotion(chara.motion_no);

	bgm->Fade_in("���v�g��", .1f);

	// ���ʂ�
	screen = new iex2DObj(1280, 720, IEX2D_RENDERTARGET);
	// ����X�N���[��	
	breakScreen = new iex3DObj("DATA/Screen/gamen.iem");//screen_grid
	breakScreen->SetAngle(3.14f);
	breakScreen->SetPos(0, 0, 0);
	breakScreen->SetScale(1.2f);
	//breakScreen->SetMotion(1);
	// �ӂ��̃X�N���[��	
	normalScreen = new iex3DObj("DATA/Screen/gamen.iem");
	normalScreen->SetAngle(3.14f);
	normalScreen->SetPos(0, 0, 11);
	normalScreen->SetScale(2.2f);
	normalScreen->Update();

	//backbuffer
	iexSystem::Device->GetRenderTarget(0, &backbuffer);//���`���


	step = STEP::START;
	startTimer = 0;

	return true;
}

SceneResult::~SceneResult()
{
	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)delete image[i];

	delete breakScreen;
	delete normalScreen;
	delete screen;
}

//******************************************************************
//		����
//******************************************************************
bool SceneResult::Update()
{
	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	if (KEY_Get(KEY_C) == 3)
	{
		breakScreen->SetMotion(1);
		// Fade����
		//FadeControl::Setting(FadeControl::FADE_OUT_W, 16);
	}
	if (KEY_Get(KEY_UP) == 3)
	{
		breakScreen->SetScale(breakScreen->GetScale().x + 0.1f);

	}
	if (KEY_Get(KEY_DOWN) == 3)
	{
		breakScreen->SetScale(breakScreen->GetScale().x - 0.1f);
	}

	// �X�e�b�v
	switch (step)
	{
	case SceneResult::START:

		startTimer++;
		if (startTimer == 60)
		{
			breakScreen->SetMotion(1);
		}

		if (breakScreen->GetParam(0)==1)
		{
			step = SceneResult::MAIN;
		}

		chara.obj->Animation();

		break;
	case SceneResult::MAIN:

		chara.obj->Animation();

		// �����L���O�̃C���X�g�̃X���C�h
		for (int i = (PLAYER_MAX - 1); i >= 0; --i)
		{
			if (i <= active)
			{
				if (MoveX[i] >= 0){
					MoveX[i] -= 48;
					break;				// ��l�Âړ���
				}
			}
		}

		// �߂�
		if (KEY_Get(KEY_ENTER) == 3 || KeyBoardTRG(MOUSE_LEFT))
		{
			bgm->Stop("���v�g��");
			MainFrame->ChangeScene(new SceneSelect());
			return true;
		}

		break;
	case SceneResult::END:


		break;
	default:


		break;
	}


	breakScreen->Animation();
	breakScreen->Update();


	return true;

}


//******************************************************************
//		�`��
//******************************************************************

void SceneResult::Render()
{
	view->Activate();
	view->Clear(0xff000000, true);;
	screen->RenderTarget();
	view->Set(Vector3(0, 30, -60), Vector3(0, 30, 0));
	view->SetProjection(PI / 4, .1f, 500);
	view->Activate();
	view->Clear(0xff00ffff, true);

	// �w�i ���c��������
	if (chara.motion_no == 1)// 1��
	{
		// �w�i ���c��������
		image[IMAGE::WIN_BACK]->RenderBack(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);

	}		
	else if (chara.motion_no == 2)	// �ŉ���
	{
		// �w�i ���c��������
		image[IMAGE::LOSE_BACK]->RenderBack(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);
	}
	else
	{
		// �w�i ���c��������
		image[IMAGE::NORMAL_BACK]->RenderBack(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);

	}

	// �w�i ���c��������
	image[IMAGE::BACK]->RenderBack(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);




	// �L�����N�^�[
	chara.obj->Update();
	chara.obj->Render();

	// ���ʔ��\
	image[IMAGE::KEKKA]->RenderBack(12, 28, 256, 64, 0, 0, 256, 64);

	// �A�N�V����UI
	//image[IMAGE::ACTION]->Render(1060, 500, 256, 256, 0, 0, 256, 256);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		if (datas[i].p_num == -114514) continue;
		image[IMAGE::P1 + datas[i].p_num]->RenderBack(MoveX[i] + 64, 136 + i * 96, 64, 64, 0, 0, 64, 64);

		// �J���X�g
		if (datas[i].score >= 999999)
		{
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 392, 138 + i * 96, 64, 64, 64 * 9, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 424, 138 + i * 96, 64, 64, 64 * 9, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 456, 138 + i * 96, 64, 64, 64 * 9, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 488, 138 + i * 96, 64, 64, 64 * 9, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 520, 138 + i * 96, 64, 64, 64 * 9, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 568, 138 + i * 96, 64, 64, 64 * 11, 0, 64, 64);
		}
		else
		{
			int iti, juu, hyaku, sen, man, juman;
			int s = datas[i].score;
			juman = s / 100000;
			man = s / 10000;
			s %= 10000;
			sen = s / 1000;
			s %= 1000;
			hyaku = s / 100;
			s %= 100;
			juu = s / 10;
			s %= 10;
			iti = s;

			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 360, 138 + i * 96, 64, 64, 64 * juman, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 392, 138 + i * 96, 64, 64, 64 * man, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 424, 138 + i * 96, 64, 64, 64 * sen, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 456, 138 + i * 96, 64, 64, 64 * hyaku, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 488, 138 + i * 96, 64, 64, 64 * juu, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 520, 138 + i * 96, 64, 64, 64 * iti, 0, 64, 64);
			image[IMAGE::NUMBER]->RenderBack(MoveX[i] + 568, 138 + i * 96, 64, 64, 64 * 11, 0, 64, 64);
		}

	
	}

	// �����N�\��
	if (active == 1)
	{
		image[IMAGE::OME]->RenderBack(708, 32);
	}
	else
	{
		image[IMAGE::R1 + Get_rank(result_my_number)]->RenderBack(848, 52, 256, 128, 0, 0, 256, 128);

	}

	// �X�R�A�`��
	//Text::Draw(320, 320, 0xffffffff, "�X�R�A : %d",limited_data->Get_score(SOCKET_MANAGER->GetID()));



	//�i���o�[�G�t�F�N�g�@��sceneMain�ŏo�����G�t�F�N�g���c���ĕ`�悳���̂ŏ����Ă���
	//Number_Effect::Render();


	//�o�b�N�o�b�t�@�Ƀ`�F���W
	iexSystem::Device->SetRenderTarget(0, backbuffer);

	view->Set(Vector3(0, 0, -12.5), Vector3(0, 0, 0));
	view->SetProjection(PI / 4, .1f, 500);
	view->Activate();

	//screen->RenderBack(0, 0, 1280, 720, 0, 0, 1280, 720);

	// �����
	static float Angle = 0;
	Angle += 0.2f;
	
	breakScreen->Update();
	shader2D->SetValue("PosterMap_0", screen);
	//breakScreen->Render(shader2D,"poster_0");

	normalScreen->Render(shader2D,"poster_0");
	breakScreen->Render();


	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		if (datas[i].p_num == -114514) continue;

		// �����̖��Oor����	
		if (SOCKET_MANAGER->GetID() == i)
		{
			DWORD col = ARGB((BYTE)255, 0, 0, 0);
			// ���O
			Text::Draw(MoveX[i] + 124, 156 + i * 96, 0xff000000, "%s", SOCKET_MANAGER->GetUser(datas[i].p_num).name);
		}
		else
		{
			DWORD col = ARGB((BYTE)255, 120, 120, 120);
			// ���O
			Text::Draw(MoveX[i] + 124, 156 + i * 96, 0xff000000, "%s", SOCKET_MANAGER->GetUser(datas[i].p_num).name);
		}
	}

#ifdef _DEBUG

	Text::Draw(100, 100, 0xff00ffff, "Scale%.2f", breakScreen->GetScale().x);
#endif

	//�t�F�[�h����
	//FadeControl::Render();

}

// �����L���O����
void SceneResult::Set_ranking()
{
	active_num = 0;

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �A�N�e�B�u��������
		if (SOCKET_MANAGER->GetUser(i).com == UserData::ACTIVE_USER)
		{
			datas[i].p_num = i;
			datas[i].score = score_mng->Get(i);
			active_num++;
		}
		else
		{
			datas[i].p_num = datas[i].score = -114514;
		}
	}
	for (int i = 0; i < active_num; i++) for (int j = i + 1; j < active_num; j++)
	{
		// �X�R�A��r���ē���ւ�
		if (datas[i].score < datas[j].score)
		{
			int temp = datas[i].score;
			datas[i].score = datas[j].score;
			datas[j].score = temp;
			temp = datas[i].p_num;
			datas[i].p_num = datas[j].p_num;
			datas[j].p_num = temp;
		}
	}

	chara.motion_no = 0;
	for (int i = 0; i < active_num; i++)
	{
		if (datas[i].p_num == result_my_number)
		{
			if (i == 0)chara.motion_no = 1;				// 1��
			else if (i == active_num-1)chara.motion_no = 2;	// �ŉ���
			else chara.motion_no = 0;						// ����
			break;
		}
	}
}
