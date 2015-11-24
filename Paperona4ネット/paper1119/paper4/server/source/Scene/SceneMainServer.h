#ifndef _SCENEMAIN_H_
#define _SCENEMAIN_H_

#include	"../../../share_data/Enum_public.h"
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


	//	�ʐM�p
	bool isRun;
	Thread* m_pThread;
	//---------------------------------------------------------------------
	//   thread
	//---------------------------------------------------------------------
	static void ThreadFunc(void* pData, bool*);

	//---------------------------------------------------------------------
	//   �|�X�^�[
	//---------------------------------------------------------------------
	iex2DObj* posterScreen[PLAYER_MAX];
	iex2DObj* posterFrame[PLAYER_MAX];

	Surface* backbuffer;// �T�[�t�F�C�X�@
	void PosterInit();
	void PosterRender(int i);

	//�@�X�^���v�̉摜��2�����z��
	iex2DObj* StampPicture[KIND_MAX][NO_MAX];
};

#endif // !_SCENEMAIN_H_