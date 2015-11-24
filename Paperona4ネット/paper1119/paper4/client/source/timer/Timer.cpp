#include "iextreme.h"
#include "Timer.h"

void Timer::Render()
{
	Text::Draw(600, 32, 0xff00ffff, "%d", limit_time);
}

Timer *timer;