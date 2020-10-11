#pragma once
#include <chrono>
#include <iostream>

/*Benchmarking class - add Timer object to a scope to measure its execution time*/
class Timer
{
public:
	Timer(const char*);
	Timer(float*);
	~Timer();

private:
	std::chrono::high_resolution_clock::time_point begin, end;
	std::chrono::duration<float> duration;
	const char* text;
	float* ref;
};

