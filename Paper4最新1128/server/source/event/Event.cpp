
#include "Event.h"
#include "../timer/Timer.h"
#include "../timer/Timer.h"
#include "../score/Score.h"

Event::Event() : start_time(0), telop_id(-1)
{};

Event::Event(int time) : start_time(time), telop_id(-1)
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

void Event::Set_telop_id(int in)
{
	telop_id = in;
}


bool Event::Update()
{
	if ((int)start_time <= timer->Get_delta())
	{
		Start();
		if (telop_id >= 0)
			score->Set_telopID(telop_id);
		return true;
	}
	
	return false;
}
