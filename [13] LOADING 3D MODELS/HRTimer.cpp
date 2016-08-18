//#include "HRTimer.h"
//
//void HRTimer::StartTimer()
//{
//	LARGE_INTEGER frequencyCount;
//	QueryPerformanceFrequency(&frequencyCount);
//
//	countsPerSecond = double(frequencyCount.QuadPart);
//
//	QueryPerformanceCounter(&frequencyCount);
//	CounterStart = frequencyCount.QuadPart;
//}
//
//double HRTimer::GetTime()
//{
//	LARGE_INTEGER currentTime;
//
//	QueryPerformanceCounter(&currentTime);
//	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;
//}
//
//double HRTimer::GetFrameTime()
//{
//	LARGE_INTEGER currentTime;
//	__int64 tickCount;
//	QueryPerformanceCounter(&currentTime);
//
//	tickCount = currentTime.QuadPart - frameTimeOld;
//	frameTimeOld = currentTime.QuadPart;
//
//	if (tickCount < 0.0f)
//		tickCount = 0.0f;
//
//	return float(tickCount) / countsPerSecond;
//}

#include "HRTimer.h"

HRTimer::HRTimer() :mStartTime(), mCurrentTime(), mLastTime(), mFrequency()
{
	mFrequency = GetFrequency();
	Reset();
}

double HRTimer::getTotalTime()
{
	return totalTime;
}

double HRTimer::getElapsedTime()
{
	return elapsedTime;
}

const LARGE_INTEGER& HRTimer::StartTime() const
{
	return mStartTime;
}

const LARGE_INTEGER& HRTimer::CurrentTime() const
{
	return mCurrentTime;
}

const LARGE_INTEGER& HRTimer::LastTime() const
{
	return mLastTime;
}

void HRTimer::Reset()
{
	GetTime(mStartTime);
	mCurrentTime = mStartTime;
	mLastTime = mCurrentTime;
	//printTimes();
}

double HRTimer::GetFrequency() const
{
	LARGE_INTEGER frequency;
	
	if (QueryPerformanceFrequency(&frequency) == false )
	{
		throw std::exception("QueryPerformanceFrequency failed");
	}
	return (double)frequency.QuadPart;
}

void HRTimer::GetTime(LARGE_INTEGER& time) const
{
	QueryPerformanceCounter(&time);
}
void HRTimer::UpdateTime()
{
	GetTime(mCurrentTime);
	totalTime = ( mCurrentTime.QuadPart - mStartTime.QuadPart ) / mFrequency;
	elapsedTime = (mCurrentTime.QuadPart - mLastTime.QuadPart) / mFrequency;
	//printTimes();
	mLastTime = mCurrentTime;
}

void HRTimer::printTimes()
{
	char  str[80];
	sprintf_s(str, "totalTime: %f elapsedTime: %f \n", totalTime, elapsedTime);
	OutputDebugString(str);

}