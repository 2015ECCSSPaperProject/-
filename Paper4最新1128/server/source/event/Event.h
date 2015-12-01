
#pragma once

// 指定したタイミングでイベントを発生させる
class Event
{
public:
	Event();
	Event(int time);
	virtual ~Event(){};

	virtual void Start() = 0;
	// イベントが発生する時間<ゲーム開始からの経過時間 (1/1000秒)>
	void Set_time(unsigned int time);
	void Set_time(unsigned int min, unsigned int sec, unsigned int msec);
	// 更新 : 時間がきたら<Startを実行, trueを返す>
	virtual bool Update();

protected:
	unsigned int start_time;
};
