#pragma once

#include	"../system/Scene.h"
#include	"../../../share_data/Enum_public.h"

class PaperClient;
class Thread;
class AnimationRippleEx;

class SceneResult : public Scene
{
public:
	//�������E���
	bool Initialize();
	~SceneResult();
	//����
	bool Update();
	//�`��
	void Render();

	enum STEP
	{
		START = 0, MAIN = 1, END = 2
	};
	int step;//�X�e�b�v
	int timer;

private:
	// �摜
	iexView* view;
	enum IMAGE{ BACK,WIN_BACK,LOSE_BACK,NORMAL_BACK, R1, R2, R3, R4, R5, R6, P1, P2, P3, P4, P5, P6, KEKKA, ACTION, NUMBER,OME, MAX };
	iex2DObj* image[IMAGE::MAX];

	//===============================================
	//	�A�j���[�V����UI
	//===============================================
	int MoveX[PLAYER_MAX];	// ���ꂼ��̃����L���O
	int active_num;			// �Q�����Ă�l��


	// ��낱�񂾂肷��L����
	struct
	{
		Vector3 pos;
		float angle;
		iex3DObj* obj;
		int motion_no;
	}chara;

	// �����L���O�̏���
	struct data
	{
		int score;
		int p_num;
	}datas[PLAYER_MAX];
	void Set_ranking();
	int Get_rank(int num)
	{
		for (int i = 0; i < PLAYER_MAX; i++)
		{
			if (datas[i].p_num == num) return i;
		}
		return -1;
	}
	
	PaperClient* net;

	//---------------------------------------------------------------------
	//   Shader&Poster
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];

	Surface* backbuffer;// �T�[�t�F�C�X�@
	iex2DObj* screen;

	//---------------------------------------------------------------------
	//   Screen
	//---------------------------------------------------------------------
	iex3DObj* breakScreen;
	iex3DObj* normalScreen;



	

};
