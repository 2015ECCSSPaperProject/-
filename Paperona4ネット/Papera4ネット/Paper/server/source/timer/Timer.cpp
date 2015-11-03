
#include "Timer.h"
#include <time.h>
#include <assert.h>

Timer *timer = nullptr;

Timer::Timer() :start(0), delta(0), limit(0)
{}

Timer::~Timer(){}

void Timer::Start()
{
	this->start = clock();
	this->delta = 0;
	this->limit = 0;
}
void Timer::Start(int limit)
{
	assert(limit >= 0);
	this->start = clock();
	this->delta = 0;
	this->limit = limit;
}
void Timer::Start(int m, int s, int ms)
{
	assert(m >= 0 && s >= 0 && ms >= 0);
	this->start = clock();
	this->delta = 0;
	limit = (60 * m + s) * 1000 + ms;
}

void Timer::Reset()
{
	this->start = 0;
	this->delta = 0;
}

int Timer::Check()
{
	this->delta = clock() - this->start;
	return this->delta;
}

int Timer::Get_delta()
{
	return this->delta;
}
float Timer::Get_second()
{
	return (float)this->delta * 0.001f;
}
void Timer::Get_second(int *s, int *ms)
{
	*s = (int)(this->delta * 0.001f);
	*ms = this->delta - *s * 1000;
}
void Timer::Get_second(int *m, int *s, int *ms)
{
	*s = (int)(this->delta * 0.001f);
	*ms = this->delta - *s * 1000;
	*m = *s / 60.0f;
	*s -= *m * 60;
}

int Timer::Get_delta_limit()
{
	return this->limit - this->delta;
}
float Timer::Get_second_limit()
{
	return  (float)(this->limit - this->delta) * 0.001f;
}
void Timer::Get_second_limit(int *s, int *ms)
{
	int d(this->limit - this->delta);
	*s = (int)(d * 0.001f);
	*ms = d - *s * 1000;
}
void Timer::Get_second_limit(int *m, int *s, int *ms)
{
	int d(this->limit - this->delta);
	*s = (int)(d * 0.001f);
	*ms = d - *s * 1000;
	*m = *s / 60.0f;
	*s -= *m * 60;
}
