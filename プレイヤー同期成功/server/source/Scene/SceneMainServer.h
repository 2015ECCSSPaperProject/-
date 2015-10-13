#ifndef _SCENEMAIN_H_
#define _SCENEMAIN_H_

#include	"../system/Scene.h"

class ShadowMap;
class Stage;
class Thread;

class SceneMainServer : public Scene
{
public:
	//�������E���
	SceneMainServer();
	bool Initialize();

	~SceneMainServer();
	//����
	void Update();
	//�`��
	void Render();
	
private:
	iexView* view;
	iexMesh* stage;


	//	�ʐM�p
	bool isRun;
	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

};

#endif // !_SCENEMAIN_H_