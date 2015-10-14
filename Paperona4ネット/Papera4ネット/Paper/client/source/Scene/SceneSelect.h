#pragma once

#include	"../system/Scene.h"

class PaperClient;
class Thread;

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
		START_NO = 0, START_OK = 1, GAME = 2
	};
	int step;//�X�e�b�v

private:
	iexView* view;
	iexMesh* stage;
	iex2DObj* Select;

	PaperClient* net;

	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);
};
