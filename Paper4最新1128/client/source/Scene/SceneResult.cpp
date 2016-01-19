
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

//#include	"../score/Score.h"
//#include	"../data/LimitedData.h"

/**********************/
/*	�O���[�o���ϐ�	�@*/
/**********************/

int result_my_number;

//�@�X���b�h���~�߂�ׂɉ��ō����
//static int ThreadEND = false;


using namespace std;

//******************************************************************
//		�������E���
//******************************************************************

bool SceneResult::Initialize()
{
	//	���ݒ�
	iexLight::SetAmbient(0x808080);
	iexLight::SetFog(800, 3000, 0);

	// Fade����
	FadeControl::Setting(FadeControl::FADE_IN_W, 16);

	view = new iexView;
	view->Set(Vector3(0, 30, -60), Vector3(0, 30, 0));
	view->SetProjection(PI / 4, .1f, 500);

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
	image[IMAGE::BACK] = new iex2DObj("DATA/Image/result/back.png");
	image[IMAGE::ACTION] = new iex2DObj("DATA/UI/action/1.png");
	image[IMAGE::KEKKA] = new iex2DObj("DATA/Image/result/result.png");
	image[IMAGE::NUMBER] = new iex2DObj("DATA/UI/Num.png");
	
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

	chara.obj = new iex3DObj("DATA/CHR/player/run.IEM");
	chara.obj->SetTexture(texture, 0);
	chara.obj->SetAngle(chara.angle);
	chara.obj->SetScale(1.0f);
	chara.obj->SetPos(chara.pos);
	chara.obj->SetMotion(chara.motion_no);

	return true;
}

SceneResult::~SceneResult()
{
	delete view;
	for (int i = 0; i < IMAGE::MAX; i++)delete image[i];
}

//******************************************************************
//		����
//******************************************************************
void SceneResult::Update()
{
	//�i���o�[�G�t�F�N�g
	Number_Effect::Update();

	//�t�F�[�h����
	FadeControl::Update();

	chara.obj->Animation();

	// �����L���O�̃C���X�g�̃X���C�h
	for (int i = (PLAYER_MAX-1); i >= 0; --i)
	{
		if (MoveX[i] >= 0){
			MoveX[i] -= 48;
			break;				// ��l�Âړ���
		}
	}

	// �߂�
	if (KEY_Get(KEY_ENTER) == 3)
	{
		   MainFrame->ChangeScene(new SceneSelect());
		   return;
	}

}


//******************************************************************
//		�`��
//******************************************************************

void SceneResult::Render()
{
	view->Activate();
	view->Clear();

	// �w�i ���c��������
	image[IMAGE::BACK]->RenderBack(0, 0, 1280, 720, 0, 0, 1280, 720, RS_COPY);

	// �L�����N�^�[
	chara.obj->Update();
	chara.obj->Render();

	// ���ʔ��\
	image[IMAGE::KEKKA]->Render(12, 28, 256, 64, 0, 0, 256, 64);

	// �A�N�V����UI
	//image[IMAGE::ACTION]->Render(1060, 500, 256, 256, 0, 0, 256, 256);

	for (int i = 0; i < PLAYER_MAX; ++i)
	{
		image[IMAGE::P1 + datas[i].p_num]->Render(MoveX[i]+64, 136 + i * 96, 64, 64, 0, 0, 64, 64);

		int iti, juu, hyaku, sen, man;
		int s = datas[i].score * 10;
		man = s / 10000;
		s %= 10000;
		sen = s / 1000;
		s %= 1000;
		hyaku = s / 100;
		s %= 100;
		juu = s / 10;
		s %= 10;
		iti = s;

		image[IMAGE::NUMBER]->Render(MoveX[i]+392, 138 + i * 96, 64, 64, 64 * man, 0, 64, 64);
		image[IMAGE::NUMBER]->Render(MoveX[i]+424, 138 + i * 96, 64, 64, 64 * sen, 0, 64, 64);
		image[IMAGE::NUMBER]->Render(MoveX[i]+456, 138 + i * 96, 64, 64, 64 * hyaku, 0, 64, 64);
		image[IMAGE::NUMBER]->Render(MoveX[i]+488, 138 + i * 96, 64, 64, 64 * juu, 0, 64, 64);
		image[IMAGE::NUMBER]->Render(MoveX[i]+520, 138 + i * 96, 64, 64, 64 * iti, 0, 64, 64);
		image[IMAGE::NUMBER]->Render(MoveX[i]+568, 138 + i * 96, 64, 64, 64 * 11, 0, 64, 64);
	
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

	// �����N�\��
	image[IMAGE::R1 + Get_rank(result_my_number)]->Render(848, 52, 256, 128, 0, 0, 256, 128);

	// �X�R�A�`��
	//Text::Draw(320, 320, 0xffffffff, "�X�R�A : %d",limited_data->Get_score(SOCKET_MANAGER->GetID()));

	//�X�e�b�v
	switch (step)
	{
	case STEP::GAME:

		Text::Draw(10, 620, 0xff00ffff, "ENTER�Ői��");

		break;
	}

	//�i���o�[�G�t�F�N�g
	Number_Effect::Render();

	//�t�F�[�h����
	FadeControl::Render();

}

// �����L���O����
void SceneResult::Set_ranking()
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		datas[i].p_num = i;
		datas[i].score = score_mng->Get(i);
	}
	for (int i = 0; i < PLAYER_MAX; i++) for (int j = i + 1; j < PLAYER_MAX; j++)
	{
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
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (datas[i].p_num == result_my_number)
		{
			if (i == 0)chara.motion_no = 23;				// 1��
			else if (i == PLAYER_MAX-1)chara.motion_no = 24;	// �ŉ���
			else chara.motion_no = 22;						// ����
			break;
		}
	}
}
