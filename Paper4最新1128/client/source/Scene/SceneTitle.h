#pragma once
#include	"../system/Scene.h"

class PaperClient;
class AnimationRippleEx;

class Mouse
{
private:
	POINT current_point;
	POINT prev_point;
	RECT rc;

public:
	float axis_x, axis_y;
	int pos_x, pos_y;

	void Update();
	bool isPushLeft(){ return ((GetKeyState(0x01) & 0x80) != 0); }
	bool isPushRight(){ return ((GetKeyState(0x02) & 0x80) != 0); }
	bool isPushCenter(){ return ((GetKeyState(0x04) & 0x80) != 0); }
	float Get_move_dist()
	{
		float vx = (float)(current_point.x - prev_point.x), vy = (float)(current_point.y - prev_point.y);
		return sqrtf(vx*vx + vy*vy);
	}
};

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

	const Vector2 max_v = Vector2(1110, 645), min_v = Vector2(980, 460);

	enum IMAGE{ BACK, CURSOR, CLICK1, CLICK2, CLICK3, GAME_START, MOUSE, TITLE, ARROW,ICON, MAX };
	iex2DObj* images[MAX];

	Vector2 move_mouse;

	//---------------------------------------------------------------------
	// ImageAnimation
	//---------------------------------------------------------------------
	int arrowPosY;
	int arrowMoveY;

	AnimationRippleEx* titleEx;


	Mouse mouse;
};