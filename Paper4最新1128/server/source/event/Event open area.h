
#pragma once

class Area_mng;
// ƒGƒŠƒA‚ğ‰ğ•ú
class Event_open_area : public Event
{
public:
	Event_open_area(int time, Area_mng *area, int index);

	void Start();

private:
	int index;
	Area_mng *area;
};
