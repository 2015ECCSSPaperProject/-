#pragma once
#include	"../system/Scene.h"

class PaperClient;
class AnimationRippleEx;

class Mouse;

class SceneTitle : public Scene
{
public:
	//èâä˙âªÅEâï˙
	bool Initialize();
	~SceneTitle();
	//èàóù
	void Update();
	//ï`âÊ
	void Render();
	void RenderShadow();

private:
	int FarShadowTimer;
	iexView* view;
	iexMesh* sky;		
	Vector3 LightVec;// Light 
	Vector3 viewPos;
	Vector3 viewTarget;
	enum class STEP
	{
		WAIT,
		CLICK,
		DRAG,
		REND_PAPER,
	};
	STEP step;

	struct
	{
		Vector3 pos;
		iex3DObj *obj;
		bool pointing;
		bool rend;
	}start_button;

	const Vector2 max_v = Vector2(1245, 670), min_v = Vector2(950, 470);

	enum IMAGE{ BACK, CURSOR, CLICK1, CLICK2, CLICK3, GAME_START, MOUSE, TITLE, ARROW,ICON, MAX };
	iex2DObj* images[MAX];

	Vector2 move_mouse;

	//---------------------------------------------------------------------
	// ImageAnimation
	//---------------------------------------------------------------------
	int arrowPosY;
	int arrowMoveY;

	AnimationRippleEx* titleEx;


	Mouse *mouse;
};