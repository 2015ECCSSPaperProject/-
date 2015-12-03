#pragma once

#include	"../system/Scene.h"
#include	"../../../share_data/Enum_public.h"

class PaperClient;
class Thread;

class SceneResult : public Scene
{
public:
	//�������E���
	bool Initialize();
	~SceneResult();
	//����
	void Update();
	//�`��
	void Render();

	enum STEP
	{
		START_NO = 0, START_OK = 1, GAME = 2
	};
	int step;//�X�e�b�v

private:
	iexView* view;
	enum IMAGE{ BACK, R1, R2, R3, R4, R5, R6, P1, P2, P3, P4, P5, P6, KEKKA, ACTION, NUMBER, MAX };
	iex2DObj* image[IMAGE::MAX];

	struct data
	{
		int score;
		int p_num;
	}datas[PLAYER_MAX];
	void Set_ranking();
	
	PaperClient* net;

	//---------------------------------------------------------------------
	//   Shader&Poster
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];
	Surface* backbuffer;// �T�[�t�F�C�X�@
};
