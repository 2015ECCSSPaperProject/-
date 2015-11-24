
#pragma once

class Timer;

class Event
{
public:
	Event(Timer *timer);
	virtual ~Event(){};

	virtual void Start() = 0;
	// イベントが発生する時間<ゲーム開始からの経過時間 (1/1000秒)>
	void Set_time(unsigned int time);
	// 更新
	virtual void Update();

protected:
	Timer *timer;
	unsigned int start_time;
};
