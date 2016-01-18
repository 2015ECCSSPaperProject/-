#pragma once

#include	"../system/Scene.h"
#include	"../../../share_data/Enum_public.h"

class PaperClient;
class Thread;
class AnimationRipple;
class Mouse;


class SceneSelect : public Scene
{
public:
	//�������E���
	bool Initialize();
	~SceneSelect();
	//����
	void Update();
	//�`��
	void Render();

	enum STEP
	{
		START_NO = 0, START_OK = 1, GAME = 2, GAME_FADE = 3
	};
	int step;//�X�e�b�v

private:
	iexView* view;

	enum IMAGE{ BACK, P1, P2, P3, P4, P5, P6, WAIT, OK, LIST, ACTION,NOPLAYER,WANTED,TEN,BACK2,INFO, MAX };
	iex2DObj* image[IMAGE::MAX];
	//---------------------------------------------------------------------
	//   ImageAnimation
	//---------------------------------------------------------------------
	bool isActivePlayer[PLAYER_MAX];
	int moveX[PLAYER_MAX];
	int alpha[PLAYER_MAX];
	AnimationRipple* IconRip[PLAYER_MAX];
	AnimationRipple* OKRip[PLAYER_MAX];
	bool OKFlag[PLAYER_MAX];

	//================================
	//		�{�^��
	struct Button
	{
		int dstX;
		int dstY;
		int dstW;
		int dstH;
		int srcX;
		int srcY;
		bool in;		// �摜�͈͓�
		iex2DObj *lpButton;
	};
	Button modoru;
	Button entry;
	void Initialize_buttons();
	void Update_buttons(const Vector2 &pos);

	struct
	{
		Vector3 pos;
		float angle;
		iex3DObj* obj;
	}chara;


	PaperClient* net;

	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

	//---------------------------------------------------------------------
	//   Shader&Poster
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];
	Surface* backbuffer;// �T�[�t�F�C�X�@
	void PosterInit();
	void PosterRender(int i);

	//�@�X�^���v�̉摜��2�����z��
	iex2DObj* StampPicture[KIND_MAX][NO_MAX];
	Mouse *mouse;
};
