#pragma once
#include	"../system/Scene.h"

class PaperClient;

class SceneTitle : public Scene
{
public:
	//�������E���
	bool Initialize();
	~SceneTitle();
	//����
	void Update();
	//�`��
	void Render();

private:
	iexView* view;
	iex2DObj* title;


};
