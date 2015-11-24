
#pragma once

#include <time.h>

class Bench_mark
{
private:
	clock_t start_time, end_time;
	double time;
public:
	Bench_mark();
	~Bench_mark();

	void Start();
	void End();
	double Get_time();
};
