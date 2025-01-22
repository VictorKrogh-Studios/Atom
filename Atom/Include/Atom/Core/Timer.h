#pragma once
#include <chrono>

namespace Atom
{

	class Timer
	{
	public:
		Timer();

		void Reset();

		float Elapsed() const;
		float ElapsedMilliseconds() const;
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	};

}