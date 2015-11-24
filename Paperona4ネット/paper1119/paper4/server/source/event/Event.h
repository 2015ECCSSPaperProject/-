
#pragma once

#include <vector>

class Event
{
public:
	Event();
	Event(int time);
	virtual ~Event(){};

	virtual void Start() = 0;
	// イベントが発生する時間<ゲーム開始からの経過時間 (1/1000秒)>
	void Set_time(unsigned int time);
	// 更新
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
