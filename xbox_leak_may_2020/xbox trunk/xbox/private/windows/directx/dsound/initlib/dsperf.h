/***************************************************************************
 *
 *  Copyright (C) 8/29/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsperf.h
 *  Content:    Performance tools.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  8/29/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __DSPERF_H__
#define __DSPERF_H__

#if defined(DEBUG) && !defined(IMPLEMENT_PERF_COUNTERS)
#define IMPLEMENT_PERF_COUNTERS
#endif // IMPLEMENT_PERF_COUNTERS

BEGIN_DEFINE_STRUCT()
    LPCSTR      pszName;
    DWORD       dwFlags;
    LPVOID      pvData;
END_DEFINE_STRUCT(DSPERFCOUNTER);

#ifdef __cplusplus

//
// Performance monitor object
//

class CDirectSoundPerformanceMonitor
{

#ifdef IMPLEMENT_PERF_COUNTERS

private:
    static const DSPERFCOUNTER  m_aCounters[];

#endif // IMPLEMENT_PERF_COUNTERS

public:
    static void RegisterCounters(void);
    static void UnregisterCounters(void);

#ifdef IMPLEMENT_PERF_COUNTERS

protected:
    static HRESULT STDAPICALLTYPE GetFree2dVoices(PLARGE_INTEGER pliData, PLARGE_INTEGER pliUnused);
    static HRESULT STDAPICALLTYPE GetFree3dVoices(PLARGE_INTEGER pliData, PLARGE_INTEGER pliUnused);
    static HRESULT STDAPICALLTYPE GetFreeSGECount(PLARGE_INTEGER pliData, PLARGE_INTEGER pliUnused);
    static HRESULT STDAPICALLTYPE GetFreeGPCycles(PLARGE_INTEGER pliData, PLARGE_INTEGER pliUnused);
    static HRESULT STDAPICALLTYPE GetFreeEPCycles(PLARGE_INTEGER pliData, PLARGE_INTEGER pliUnused);

private:
    static DWORD GetFreeDSPCycles(DWORD dwAddress, DWORD dwSamplesPerClock);

#endif // IMPLEMENT_PERF_COUNTERS

};

#ifndef IMPLEMENT_PERF_COUNTERS

__inline void CDirectSoundPerformanceMonitor::RegisterCounters(void)
{
}

__inline void CDirectSoundPerformanceMonitor::UnregisterCounters(void)
{
}

#endif // IMPLEMENT_PERF_COUNTERS

#endif // __cplusplus

#endif // __DSPERF_H__
