#ifndef _HRTIMER
#define _HRTIMER

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



//	void StartTimer();
//
//private:
//	//wstring printScoreFPS;
//	int totalScore = 0;
//
//	double countsPerSecond = 0.0f;
//	__int64 CounterStart = 0;
//
//	int frameCount = 0;
//	int fps = 0;
//
//
//	double GetTime();
//	double GetFrameTime();
//
//	__int64 frameTimeOld = 0;
//	double frameTime;
//
};


#endif