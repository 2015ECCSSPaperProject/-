#pragma once
#include	"../system/Scene.h"

class PaperClient;

class SceneTitle : public Scene
{
public:
	//‰Šú‰»E‰ğ•ú
	bool Initialize();
	~SceneTitle();
	//ˆ—
	void Update();
	//•`‰æ
	void Render();

private:
	iexView* view;
	iex2DObj* title;


};
