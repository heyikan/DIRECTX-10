# DIRECTX 10 HIGH RESOLUTION TIMER

![FPS](.//img//fps.png)

* Here we will learn how to use the QueryPerformanceCounter() and QueryPerformanceTimer() functions to impliment a high resolution performance game timer which we will use to find the Frames per Second, and display them on the screen using the ID3DX10Font interface we learned about in one of the earlier lessons. We will also use this timer to find the time it takes for each frame, and update our camera depending on the time it takes per frame. This will keep the camera moving smoothly and at the same speed, whatever the frames per second are. To make things slightly more fun we'll also include a score with the FPS, where you get one point for every sphere you shoot.

#### HRTimer.h
```c
#include <Windows.h>
#include <sstream>

class HRTimer
{
public:


	HRTimer();

	const LARGE_INTEGER& StartTime() const;
	const LARGE_INTEGER& CurrentTime() const;
	const LARGE_INTEGER& LastTime() const;

	void Reset();
	double GetFrequency() const;
	void GetTime(LARGE_INTEGER& time)const;
	void UpdateTime();

	void printTimes();

	double getTotalTime();
	double getElapsedTime();
private:
	HRTimer(const HRTimer& rhs);
	HRTimer& operator=(const HRTimer& rhs);

	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mCurrentTime;
	LARGE_INTEGER mLastTime;

	double totalTime;
	double elapsedTime;

	double mFrequency;

};
```

#### HRTimer.cpp

```c

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
	mLastTime = mCurrentTime;
}

```
