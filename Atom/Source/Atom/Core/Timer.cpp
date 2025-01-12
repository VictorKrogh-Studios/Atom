#include "ATPCH.h"
#include "Timer.h"

namespace Atom
{

	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	float Timer::Elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(std::chrono::high_resolution_clock::now() - m_StartTimepoint).count() / 1000.0f;
	}

	float Timer::ElapsedMilliseconds() const
	{
		return Elapsed() * 1000.0f;
	}

}

