//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

// from https://github.com/utilForever/DirectX/blob/master/DirectX%2011/Frank%20D.%20Luna/Common/GameTimer.cpp

#ifndef GAMETIMER_H
#define GAMETIMER_H

class GameTimer
{
public:
    GameTimer();

    float TotalTime()const; // in seconds
    float DeltaTime()const; // in seconds

    void Reset(); // Call before message loop.
    void Start(); // Call when unpaused.
    void Stop();  // Call when paused.
    void Tick();  // Call every frame.

private:
    double mSecondsPerCount;
    double mDeltaTime;

    __int64 mBaseTime;
    __int64 mPausedTime;
    __int64 mStopTime;
    __int64 mPrevTime;
    __int64 mCurrTime;

    bool mStopped;
};

#endif // GAMETIMER_H