
#pragma once

class Timer;

class Event
{
public:
	Event(Timer *timer);
	virtual ~Event(){};

	virtual void Start() = 0;
	// �C�x���g���������鎞��<�Q�[���J�n����̌o�ߎ��� (1/1000�b)>
	void Set_time(unsigned int time);
	// �X�V
	virtual void Update();

protected:
	Timer *timer;
	unsigned int start_time;
};
