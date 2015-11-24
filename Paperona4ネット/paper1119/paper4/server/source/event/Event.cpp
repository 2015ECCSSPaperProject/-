
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

void Event::Update()
{
	if (start_time <= timer->Get_delta())
		Start();
}



#include "../stage/Area.h"

Event_open_area::Event_open_area(int time, Area *area) : Event(time), area(area)
{}

void Event_open_area::Start()
{
	area->Open();
}



Event *event_list = nullptr;
