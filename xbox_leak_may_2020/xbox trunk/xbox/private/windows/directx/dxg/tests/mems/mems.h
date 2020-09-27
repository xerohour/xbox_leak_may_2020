//-----------------------------------------------------------------------------
// FILE: MEMS.H
//
// Desc: mems header file
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define PAGE_SIZE 0x1000

#define ARRAYSIZE(_a)     (sizeof(_a) / sizeof((_a)[0]))

//=========================================================================
// Class to help time things
//=========================================================================
class CTimer
{
public:
    // constructor
    CTimer()
    {
        QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
        ticksPerSecond /= 1000;
    }

    ~CTimer() {};

    void Start()
        { QueryPerformanceCounter((LARGE_INTEGER *)&ticksstart); }

    void Stop()
        { QueryPerformanceCounter((LARGE_INTEGER *)&ticksend); }

    float getTime()
        {
            QueryPerformanceCounter((LARGE_INTEGER *)&ticksend);
            return ((ticksend - ticksstart) * 1.0f / ticksPerSecond);
        }

    _int64 ticksstart;
    _int64 ticksend;
    _int64 ticksPerSecond;
};
