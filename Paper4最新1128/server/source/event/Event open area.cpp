
#include "Event.h"
#include "Event open area.h"

#include "../stage/Area.h"

Event_open_area::Event_open_area(int time, Area_mng *area, int index) : Event(time), area(area), index(index)
{}

void Event_open_area::Start()
{
	area->Open(index);
}
