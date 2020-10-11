#include "Timer.h"

Timer::Timer(const char* t)
{
	begin = std::chrono::high_resolution_clock::now();
	text = t;
	ref = 0;
}

Timer::Timer(float* num)
{
	begin = std::chrono::high_resolution_clock::now();
	ref = num;
}

Timer::~Timer()
{
	end = std::chrono::high_resolution_clock::now();
	duration = end - begin;

	*ref = duration.count();

	//Optional message
	//std::cout << text << " took: " << duration.count() * 1000 << "ms\n";
}