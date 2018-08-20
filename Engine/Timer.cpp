#include "Timer.h"

Timer::Timer()
	:
	last { std::chrono::steady_clock::now() }
{}

float Timer::Mark()
{
	const auto temp = last;
	last = std::chrono::steady_clock::now();
	const std::chrono::duration<float> frame_time = last - temp;

	return frame_time.count();
}