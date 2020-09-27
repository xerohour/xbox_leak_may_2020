/***************************************************************************
 *
 *  Copyright (C) 8/29/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsperf.cpp
 *  Content:    Performance tools.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  8/29/2001   dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

#ifdef IMPLEMENT_PERF_COUNTERS

#include <dm.h>
#include <xbdm.h>

const DSPERFCOUNTER CPerfMon::m_aCounters[] =
{
    { "DirectSound free 2D voices", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &g_dwDirectSoundFree2dVoices },
    { "DirectSound free 3D voices", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &g_dwDirectSoundFree3dVoices },
    { "DirectSound free buffer SGEs", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &g_dwDirectSoundFreeBufferSGEs },
    { "DirectSound pool memory used", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &g_dwDirectSoundPoolMemoryUsage },
    { "DirectSound physical memory used", DMCOUNT_VALUE | DMCOUNT_ASYNC32, &g_dwDirectSoundPhysicalMemoryUsage },
    { "DirectSound percent free GP cycles", DMCOUNT_VALUE | DMCOUNT_SYNC, GetFreeGPCycles },
    { "DirectSound percent free EP cycles", DMCOUNT_VALUE | DMCOUNT_SYNC, GetFreeEPCycles },
};


/****************************************************************************
 *
 *  RegisterCounters
 *
 *  Description:
 *      Registers DirectSound performance counters.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPerfMon::RegisterCounters"

void
CPerfMon::RegisterCounters
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();

    for(i = 0; i < NUMELMS(m_aCounters); i++)
    {
        DmTell_RegisterPerformanceCounter(m_aCounters[i].pszName, m_aCounters[i].dwFlags, m_aCounters[i].pvData);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  UnregisterCounters
 *
 *  Description:
 *      Unregisters DirectSound performance counters.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPerfMon::UnregisterCounters"

void
CPerfMon::UnregisterCounters
(
    void
)
{
    DWORD                   i;
    
    DPF_ENTER();

    for(i = 0; i < NUMELMS(m_aCounters); i++)
    {
        DmTell_UnregisterPerformanceCounter(m_aCounters[i].pszName);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  PerfGetFreeGPCycles
 *
 *  Description:
 *      Gets the count of available GP DSP cycles.
 *
 *  Arguments:
 *      PLARGE_INTEGER [out]: count.
 *      PLARGE_INTEGER [out]: reserved.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPerfMon::GetFreeGPCycles"

STDAPI
CPerfMon::GetFreeGPCycles
(
    PLARGE_INTEGER          pliData, 
    PLARGE_INTEGER          pliUnused
)
{
    DPF_ENTER();

    pliData->QuadPart = GetFreeDSPCycles(0xFE8301F0, 32);

    DPF_LEAVE_HRESULT(XBDM_NOERR);

    return XBDM_NOERR;
}


/****************************************************************************
 *
 *  PerfGetFreeEPCycles
 *
 *  Description:
 *      Gets the count of available EP DSP cycles.
 *
 *  Arguments:
 *      PLARGE_INTEGER [out]: count.
 *      PLARGE_INTEGER [out]: reserved.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPerfMon::GetFreeEPCycles"

STDAPI
CPerfMon::GetFreeEPCycles
(
    PLARGE_INTEGER          pliData, 
    PLARGE_INTEGER          pliUnused
)
{
    DPF_ENTER();

    pliData->QuadPart = GetFreeDSPCycles(0xFE85A010, 256);

    DPF_LEAVE_HRESULT(XBDM_NOERR);

    return XBDM_NOERR;
}


/****************************************************************************
 *
 *  PerfGetFreeDSPCycles
 *
 *  Description:
 *      Gets the count of available DSP cycles.
 *
 *  Arguments:
 *      DWORD [in]: performance register address.
 *      DWORD [in]: samples per clock.
 *
 *  Returns:  
 *      DWORD: count of idle cycles.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPerfMon::GetFreeDSPCycles"

DWORD
CPerfMon::GetFreeDSPCycles
(
    DWORD                   dwAddress,
    DWORD                   dwSamplesPerClock
)
{
    static DWORD            dwClockSpeed;
    PCI_SLOT_NUMBER         Slot;
    DWORD                   dwMaxIdle;
    DWORD                   dwUsage;
    
    DPF_ENTER();

    //
    // Get the DSP clock speed
    //
    
    if(!dwClockSpeed)
    {
        Slot.u.AsULONG = 0;
        Slot.u.bits.DeviceNumber = 1;
        Slot.u.bits.FunctionNumber = 0;

        HalReadPCISpace(0, Slot.u.AsULONG, 0x8C, &dwClockSpeed, sizeof(dwClockSpeed));

        switch((dwClockSpeed >> 26) & 3)
        {
            case 1:
                dwClockSpeed = 133;
                break;

            case 2:
                dwClockSpeed = 200;
                break;

            default:
                dwClockSpeed = 160;
                break;
        }
    }

    //
    // Calculate max idle cycles
    //

    dwMaxIdle = ((dwSamplesPerClock * 1000) / 48) * dwClockSpeed;

    //
    // Get the count of cycles currently being used
    //

    dwUsage = *(LPDWORD)dwAddress;

    //
    // Convert to idle cycles
    //

    if(dwUsage >= dwMaxIdle)
    {
        dwUsage = dwMaxIdle;
    }
    else
    {
        dwUsage = dwMaxIdle - dwUsage;
    }

    //
    // Convert to percentage
    //

    dwUsage *= 100;
    dwUsage /= dwMaxIdle;

    DPF_LEAVE(dwUsage);

    return dwUsage;
}

#endif // IMPLEMENT_PERF_COUNTERS


