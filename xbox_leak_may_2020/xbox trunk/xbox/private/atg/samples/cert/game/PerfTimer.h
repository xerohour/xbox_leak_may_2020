//-----------------------------------------------------------------------------
// File: PerfTimer.cpp
//
// Desc: Performance timers
//
// Hist: 04.10.01 - New for May XDK release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

typedef __int64 TimeStamp;

void GetTimeStamp(TimeStamp* pStamp);

double SecondsElapsed(const TimeStamp& ts1, const TimeStamp& ts2);



inline void GetTimeStamp(TimeStamp* pStamp)
{
	__asm
	{
		mov		esi,[pStamp]
		rdtsc
		mov		[esi],eax
		mov		[esi+4],edx
	}
}



inline double SecondsElapsed(const TimeStamp& ts1, const TimeStamp& ts2)
{
	return double(ts2 - ts1) * (1.0 / 800e6);
}
