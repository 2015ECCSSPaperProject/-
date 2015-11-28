
#pragma once

class Timer
{
private:
	int limit_time;
public:
	void Render();
	void Set_limit_time(int time){ limit_time = time; }
	int Get_limit_time(){ return limit_time; }
};

extern Timer *timer;