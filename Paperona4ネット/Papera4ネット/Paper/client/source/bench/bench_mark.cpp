
#include "Bench_mark.h"

Bench_mark::Bench_mark() : start_time(0), end_time(0), time(0)
{}

Bench_mark::~Bench_mark(){}

void Bench_mark::Start()
{
	start_time = clock();
}

void Bench_mark::End()
{
	end_time = clock();
	time = end_time - start_time;
	time /= CLOCKS_PER_SEC;
}

double Bench_mark::Get_time()
{
	return time;
}
