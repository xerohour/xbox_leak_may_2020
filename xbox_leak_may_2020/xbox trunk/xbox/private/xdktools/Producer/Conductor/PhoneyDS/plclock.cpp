//      Copyright (c) 1996-1999 Microsoft Corporation
/*	CPhaseLockClock

  */

#include "stdafx.h"
#include "dmusici.h"
#include "dmusicc.h"
#include "plclock.h"

//#define _FILE_DEBUG 1
#ifdef _FILE_DEBUG
void FTRACE( LPSTR pszFormat, ... );
#endif

#define MILS_TO_REF	10000

CPhaseLockClock::CPhaseLockClock()
{
	m_rfOffset = 0;
    m_rfBaseOffset = 0;
}

// When the clock starts, it needs to mark down the 
// difference between the time it is given and its concept of time. 

void CPhaseLockClock::Start(REFERENCE_TIME rfMasterTime, REFERENCE_TIME rfSlaveTime)
{
	m_rfOffset = 0;
    m_rfBaseOffset = rfMasterTime - rfSlaveTime;
}	


// Convert the passed time to use the same base as the master clock.

void CPhaseLockClock::GetSlaveTime(REFERENCE_TIME rfSlaveTime, REFERENCE_TIME *prfTime)
{
	rfSlaveTime += (m_rfBaseOffset + m_rfOffset);
	*prfTime = rfSlaveTime;
}

void CPhaseLockClock::SetSlaveTime(REFERENCE_TIME rfSlaveTime, REFERENCE_TIME *prfTime)

{
	rfSlaveTime -= m_rfBaseOffset;
	*prfTime = rfSlaveTime;
}

/*	SyncToMaster provides the needed magic to keep the clock
	in sync. Since the clock uses its own clock (rfSlaveTime)
	to increment, it can drift. This call provides a reference
	time which the clock compares with its internal 
	concept of time. The difference between the two is
	considered the drift. Since the sync time may increment in
	a lurching way, the correction has to be subtle. 
	So, the difference between the two is divided by
	100 and added to the offset.
*/
void CPhaseLockClock::SyncToMaster(REFERENCE_TIME rfSlaveTime, REFERENCE_TIME rfMasterTime, BOOL fLockToMaster)
{
	rfSlaveTime += (m_rfOffset + m_rfBaseOffset);
	rfSlaveTime -= rfMasterTime;	// Find difference between calculated and expected time.
	rfSlaveTime /= 10;				// Reduce in magnitude.
    // If fLockToMaster is true, we want to adjust our offset that we use for conversions, 
    // so our clock will slave to the master clock.
    if (fLockToMaster)
    {
        m_rfBaseOffset -= rfSlaveTime;
    }
    // Otherwise, we want to put a value into m_rfOffset that will be used to 
    // tweak the master clock so it will slave to our time.
    else
    {
	    m_rfOffset -= rfSlaveTime;		// Subtract that from the original offset.
    }
}

CSampleClock::CSampleClock()
{
	m_dwStart = 0;
	m_dwSampleRate = 22050;
}

void CSampleClock::Start(IReferenceClock *pIClock, DWORD dwSampleRate, DWORD dwSamples)
{
	REFERENCE_TIME rfStart;
	m_dwStart = dwSamples;
	m_dwSampleRate = dwSampleRate;
	if (pIClock)
	{
		pIClock->GetTime(&rfStart);
		m_PLClock.Start(rfStart,0);
	}
}

CSampleClock::~CSampleClock()
{
}

void CSampleClock::SampleToRefTime(LONGLONG llSampleTime, REFERENCE_TIME *prfTime)
{
	llSampleTime -= m_dwStart;
	llSampleTime *= MILS_TO_REF;
	llSampleTime /= m_dwSampleRate;
	llSampleTime *= 1000;
	m_PLClock.GetSlaveTime(llSampleTime, prfTime);
}

LONGLONG CSampleClock::RefToSampleTime(REFERENCE_TIME rfTime)
{
	m_PLClock.SetSlaveTime(rfTime, &rfTime);
	rfTime /= 1000;
	rfTime *= m_dwSampleRate;
	rfTime /= MILS_TO_REF;
	rfTime += m_dwStart;
	return rfTime;
}

void CSampleClock::SyncToMaster(LONGLONG llSampleTime, IReferenceClock *pIClock, BOOL fLockToMaster)
{
	llSampleTime -= m_dwStart;
	llSampleTime *= MILS_TO_REF;
	llSampleTime /= m_dwSampleRate;
	llSampleTime *= 1000;
	if (pIClock)
	{
		REFERENCE_TIME rfMasterTime;
		pIClock->GetTime(&rfMasterTime);

#ifdef _FILE_DEBUG
		REFERENCE_TIME rtSlaveTime;
		m_PLClock.GetSlaveTime(llSampleTime, &rtSlaveTime);
		FTRACE("SyncToMaster: %d SampleTime %I64d MasterTime %I64d\n", timeGetTime(), rtSlaveTime, rfMasterTime );
#endif

		m_PLClock.SyncToMaster(llSampleTime, rfMasterTime,fLockToMaster);
	}
}


