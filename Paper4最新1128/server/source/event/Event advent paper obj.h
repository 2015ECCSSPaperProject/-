
// #include "Event.h"

#pragma once

class Paper_obj;

class Event_advent_paper_obj : public Event
{
public:
	Event_advent_paper_obj(Paper_obj *obj);

	void Start() override;

protected:
	Paper_obj *obj;
};
