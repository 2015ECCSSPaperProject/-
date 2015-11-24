
#pragma once

#include <vector>

class Event
{
public:
	Event();
	Event(int time);
	virtual ~Event(){};

	virtual void Start() = 0;
	// �C�x���g���������鎞��<�Q�[���J�n����̌o�ߎ��� (1/1000�b)>
	void Set_time(unsigned int time);
	// �X�V
	virtual void Update();

protected:
	;
	unsigned int start_time;
};



class Area;

class Event_open_area : public Event
{
public:
	Event_open_area(Area *area);
	Event_open_area(int time, Area *area);

	void Start();

private:
	Area *area;
};



extern Event *event_list;
