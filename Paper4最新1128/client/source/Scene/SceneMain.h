#ifndef _SCENEMAIN_H_
#define _SCENEMAIN_H_

#include	"../system/Scene.h"

class PaperClient;
class Thread;

class Stage;

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
	iexMesh* sky;

	PaperClient* net;

	// ���[�h�֌W
	enum MODE{ START, MAIN, END };
	MODE mode;
	void Start();						// �Q�[���n�܂���3�b�ҋ@
	void Main();						// ��{�I�ȃQ�[������
	void End();							// �Q�[���I�����Ă�����Ƒҋ@���Ă��烊�U���g�s������
	void(SceneMain::*Mode_funk[3])();	// �֐��|�C���^


	Thread* m_pThread;// �X���b�h
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

};

#endif // !_SCENEMAIN_H_