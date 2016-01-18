#pragma once

class Mouse
{
private:
	POINT current_point;
	POINT prev_point;
	RECT rc;

public:
	float axis_x, axis_y;
	Vector2 pos;

	void Initialize(BOOL show);
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