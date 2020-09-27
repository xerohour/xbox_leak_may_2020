//      Copyright (c) 1996-1999 Microsoft Corporation
/*	PLClock.h

  */

#ifndef __PLCLOCK_H__
#define __PLCLOCK_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPhaseLockClock
{
public:
						CPhaseLockClock();
	void				Start(REFERENCE_TIME rfMasterTime, REFERENCE_TIME rfSlaveTime);
	void				GetSlaveTime(REFERENCE_TIME rfSlaveTime,REFERENCE_TIME *prfTime);
	void				SetSlaveTime(REFERENCE_TIME rfSlaveTime,REFERENCE_TIME *prfTime);
	void				SyncToMaster(REFERENCE_TIME rfSlaveTime, REFERENCE_TIME rfMasterTime,BOOL fLockToMaster);
	void				GetClockOffset(REFERENCE_TIME *prfTime) { *prfTime = m_rfOffset; };
private:
	REFERENCE_TIME		m_rfOffset;
    REFERENCE_TIME      m_rfBaseOffset;
};

class CSampleClock
{
public:
						CSampleClock();
						~CSampleClock();
	void				Start(IReferenceClock *pIClock, DWORD dwSampleRate, DWORD dwSamples);
	void				SampleToRefTime(LONGLONG llSampleTime,REFERENCE_TIME *prfTime);
	void				SyncToMaster(LONGLONG llSampleTime, IReferenceClock *pIClock,BOOL fLockToMaster);
	LONGLONG			RefToSampleTime(REFERENCE_TIME rfTime);
	void				GetClockOffset(REFERENCE_TIME *prfTime) { m_PLClock.GetClockOffset(prfTime); };

private:
	CPhaseLockClock		m_PLClock;
	DWORD				m_dwStart;			// Initial sample offset.
	DWORD				m_dwSampleRate;
};
#endif	// __PLCLOCK_H__