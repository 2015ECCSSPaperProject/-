
#pragma once

class Timer
{
private:
	int limit_time;
	int start;
public:
	void Render();
	void Set_limit_time( int time )
	{
		start = limit_time = time;
	}
	int Get_limit_time()
	{
		return limit_time;
	}
};

#define LIMIT_TIME 120
extern Timer *timer;