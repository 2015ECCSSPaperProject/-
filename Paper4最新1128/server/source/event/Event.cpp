
#include "Event.h"
#include "../timer/Timer.h"
#include "../timer/Timer.h"

Event::Event() : start_time(0)
{};

Event::Event(int time) : start_time(time)
{}

void Event::Set_time(unsigned int time)
{
	start_time = time;
}
void Event::Set_time(unsigned int min, unsigned int sec, unsigned int msec)
{
	sec += min * 60;
	msec += sec * 1000;
	start_time = msec;
}


bool Event::Update()
{
	if (start_time <= timer->Get_delta())
	{
		Start();
		return true;
	}
	
	return false;
}
