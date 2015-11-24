
#include "Event.h"
#include "../timer/Timer.h"

Event::Event(Timer *timer) : timer(timer), start_time(0)
{};

void Event::Set_time(unsigned int time)
{
	start_time = time;
}

void Event::Update()
{
	if (start_time <= timer->Get_delta())
		Start();
}
