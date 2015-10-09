
#pragma once

class Mouse
{
private:
	static POINT p;
	static RECT rc;
	static float axis_x, axis_y;


public:
	static void Update();
	static float Get_axis_x(){ return axis_x; }
	static float Get_axis_y(){ return axis_y; }
	static bool isPushLeft(){ return ((GetKeyState(0x01) & 0x80) != 0); }
	static bool isPushRight(){ return ((GetKeyState(0x02) & 0x80) != 0); }
	static bool isPushCenter(){ return ((GetKeyState(0x04) & 0x80) != 0); }
};