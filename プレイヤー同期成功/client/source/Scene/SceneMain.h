#ifndef _SCENEMAIN_H_
#define _SCENEMAIN_H_

#include	"../system/Scene.h"

class PaperClient;
class Thread;

class SceneMain : public Scene
{
public:
	//�������E���
	bool Initialize();
	~SceneMain();
	//����
	void Update();
	//�`��
	void Render();
	
private:
	iexView* view;
	iexMesh2* stage;
	iexMesh2* sky;

	PaperClient* net;

	Thread* m_pThread;// �X���b�h
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

};

#endif // !_SCENEMAIN_H_