 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/******************************** Time Manager *****************************\
*                                                                           *
* Module: TIME.C                                                            *
*   Time objects are managed in this module.                                *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <state.h>
#include <dma.h>
#include <tmr.h>
#include <os.h>
#include <nv32.h>
#include <smp.h>
#include "nvhw.h"

//
// Object creation/deletion.
//
RM_STATUS tmrCreateTimer
(
    VOID*      pDevHandle,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *TimeObject,
    VOID*        pCreateParms
)
{
    PHWINFO pDev = (PHWINFO) pDevHandle;
    RM_STATUS status;
    
    RM_ASSERT(ClassObject->Base.Name == NV1_TIMER);

    status = osAllocMem((VOID **)TimeObject, sizeof(TIMEROBJECT));
    if (status)
        return (status);

//    ((PTIMEROBJECT)*TimeObject)->NotifyXlate  = NULL;
//    ((PTIMEROBJECT)*TimeObject)->NotifyAction = 0;
    ((PTIMEROBJECT)*TimeObject)->State        = BUFFER_IDLE;
    ((PTIMEROBJECT)*TimeObject)->AlarmLo      = 0;
    ((PTIMEROBJECT)*TimeObject)->AlarmHi      = 0;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*TimeObject, ClassObject, UserName);

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &((PTIMEROBJECT)*TimeObject)->CBase, 
                                      ((PTIMEROBJECT)*TimeObject)->CBase.ChID, 
                                      ((PTIMEROBJECT)*TimeObject)->CInstance);
    if (status != RM_OK)
        osFreeMem(*TimeObject);

    return (RM_OK);
}

RM_STATUS tmrDeleteContextTime
(
    POBJECT TimeObject
)
{
    return (osFreeMem(TimeObject));
}

RM_STATUS tmrDeleteTimer
(
	VOID*   pDevHandle,
    POBJECT TimeObject
)
{
    PHWINFO pDev = (PHWINFO) pDevHandle;

    //
    // Just in case...
    //
    if (TimeObject == NULL)
        return (RM_OK);

    //
    // Clear the subChannel ptrs for this object 
    //
	osEnterCriticalCode(pDev);
	fifoDeleteSubchannelContextEntry(pDev, TimeObject);
	osExitCriticalCode(pDev);

    //
    // Cancel any outstanding callbacks before deleting object.
    //
    tmrCancelCallback(pDev, TimeObject);
    return (osFreeMem(TimeObject));
}

//
// Methods.
//
RM_STATUS mthdSetTime
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS status;
    U032      TimeCheck;
    U032      TimeLo;
    U032      TimeHi;
    
    status = RM_OK;
    if (Offset == Method->Low)
    {
        ((PTIMEOBJECT)Object)->TimeLo = Data;
    }
    else
    {
        //
        // Treat time values less than 4.29 sec as relative.
        //
        if (!Data)
        {
            do
            {
                TimeCheck = REG_RD32(NV_PTIMER_TIME_1);
                TimeLo    = REG_RD32(NV_PTIMER_TIME_0);
                TimeHi    = REG_RD32(NV_PTIMER_TIME_1);
            } while (TimeCheck != TimeHi);
            ((PTIMEOBJECT)Object)->TimeLo += TimeLo;
            //
            // Handle carry.
            //
            if (((PTIMEOBJECT)Object)->TimeLo < TimeLo)
                Data = 1;
            Data += TimeHi;    
        }
        ((PTIMEOBJECT)Object)->TimeHi = Data;
        //
        // Reflect new time to all attached objects.
        //
        if (((PTIMEOBJECT)Object)->TimeProc)
            status = ((PTIMEOBJECT)Object)->TimeProc(pDev, ((PTIMEOBJECT)Object)->TimeChange);
    }
    return (status);
}

RM_STATUS mthdSetAlarmNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS status;

    //
    // Check for busy buffer.
    //
    if (((PTIMEROBJECT)Object)->State == BUFFER_BUSY)
    {
        osError(Object, Offset, Data, RM_ERROR);
        return (RM_ERR_DMA_IN_USE);
    }

    //
    // Connect to dma buffer.
    //    
    status = dmaValidateObjectName(pDev, Data,
                                   Object->ChID,
                                   &((Object)->NotifyXlate));
    if (status)
    {
        (Object)->NotifyXlate = NULL;
        return (status);
    }

    //
    // Make sure the context dma is large enough (2 notifiers)
    //    
    status = dmaValidateXlate(Object->NotifyXlate, 0, (2 * 0x0010));
    if (status)
    {
        (Object)->NotifyXlate = NULL;
        return (status);
    }

    return (status);
}

RM_STATUS mthdSetAlarmNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    RM_STATUS     status;

    //
    // Check for busy buffer.
    //
    if (((PTIMEROBJECT)Object)->State == BUFFER_BUSY)
    {
        osError(Object, Offset, Data, RM_ERROR);
        return (RM_ERR_DMA_IN_USE);
    }
    (Object)->NotifyAction = Data;
    //
    // Validate notification parameters.
    //    
    status = dmaValidateXlate((Object)->NotifyXlate, 0, 0x000F);
    if (status)
    {
        osError(Object, Offset, Data, status);
        return (status);
    }
	/* KJK
    status = dmaBeginXfer(((PTIMEROBJECT)Object)->NotifyXlate, NULL);
    if (status)
    {
        osError(Object, Offset, Data, status);
        return (status);
    }
	*/
    //
    // Schedule the alarm.
    //
    ((PTIMEROBJECT)Object)->State = BUFFER_BUSY;
    return (tmrScheduleCallbackAbs(pDev,
                                   tmrAlarmNotify,
                                   Object,
                                   ((PTIMEROBJECT)Object)->AlarmHi,
                                   ((PTIMEROBJECT)Object)->AlarmLo));
}

RM_STATUS mthdSetAlarmTime
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    U032    TimeCheck;
    U032    CurrentTimeHi;

    do
    {
        TimeCheck     = REG_RD32(NV_PTIMER_TIME_1);
        CurrentTimeHi = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != CurrentTimeHi);

    if (Offset == Method->Low) {
        // set AlarmHigh, in case AlarmLo is the only method sent
        ((PTIMEROBJECT)Object)->AlarmLo = Data;
        ((PTIMEROBJECT)Object)->AlarmHi = CurrentTimeHi;
    } else
        ((PTIMEROBJECT)Object)->AlarmHi = Data;

    return RM_OK;
}

RM_STATUS mthdSetAlarmTimeRelative
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    U032    TimeCheck;
    U032    CurrentTimeLo;
    U032    CurrentTimeHi;

    do
    {
        TimeCheck     = REG_RD32(NV_PTIMER_TIME_1);
        CurrentTimeLo = REG_RD32(NV_PTIMER_TIME_0);
        CurrentTimeHi = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != CurrentTimeHi);

    if (Offset == Method->Low) {
        // set AlarmHigh, in case AlarmLo is the only method sent
        ((PTIMEROBJECT)Object)->AlarmLo = CurrentTimeLo + Data;
        ((PTIMEROBJECT)Object)->AlarmHi = CurrentTimeHi;

         // if AlarmLo rolled over, increment AlarmHi
         if (((PTIMEROBJECT)Object)->AlarmLo < CurrentTimeLo)
            ((PTIMEROBJECT)Object)->AlarmHi++;
    } else
        ((PTIMEROBJECT)Object)->AlarmHi = CurrentTimeHi + Data;

    return RM_OK;
}

//
// Alarm callback.
//
RM_STATUS tmrAlarmNotify
(
	PHWINFO pDev,
    POBJECT Object
)
{
    notifyFillNotifierArray(pDev, (Object)->NotifyXlate, 0, 0, RM_OK,
                            NV004_NOTIFIERS_SET_ALARM_NOTIFY);

    if ((Object)->NotifyAction)
    {
        //
        // Do any OS specified action related to this notification.
        //
        osNotifyEvent(pDev, Object, 
                      1,	  // NV004_NOTIFIERS_SET_ALARM_NOTIFY
                      0, 
                      1, 
                      RM_OK, 
                      (Object)->NotifyAction);
    }

    ((PTIMEROBJECT)Object)->State = BUFFER_IDLE;

    return (RM_OK);
}

RM_STATUS tmrStopTransfer
(
	PHWINFO pDev,
	POBJECT Object, 
	PMETHOD Method, 
	U032    Offset,
	V032	Data
)
{

	return (RM_OK);

}

//
// NV_PTIMER_TIME_0 rolls over approx every 4 secs. For delays
// less than 1/4 of that time just compare against TIME_0
//
#define MAX_SMALLNS_TMRDELAY (0xFFFFFFFF >> 2)

//
// Timer related functions.
//
RM_STATUS tmrDelay
(
    PHWINFO pDev,
    U032 nsec
)
{
    U032 TimeHi;
    U032 TimeLo;
    U032 TimeCheck;
    U032 IsPllSlowed = 0;  // some compilers think its uninitialized otherwise
    static U032 TimeStart;
    //
    // Check to see if NVCLK is currently being dynamically slowed
    // If so, temporarily disable so we can get a true time delay
    //
    // KJK: I wonder if this should be halified?
    //
    if (IsNV11(pDev))
    {
        IsPllSlowed = REG_RD_DRF(_PBUS, _DEBUG_1, _CORE_SLOWDWN);
        FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, 0);
    }

    //
    // For small delays which can only wrap PTIMER_TIME_0 once, doing 2's
    // comp math on TIME_0 is enough and means we don't have to sync TIME_1
    //
    if (nsec < MAX_SMALLNS_TMRDELAY)
    {
	TimeStart = REG_RD32(NV_PTIMER_TIME_0);
	while (nsec > ((volatile U032)REG_RD32(NV_PTIMER_TIME_0) - TimeStart))
	    ;

    //
    // Restore PLL slowdown setting
    //
    if (IsNV11(pDev))
        FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);

	return (RM_OK);
    }

    //
    // Get current time.
    //
    do
    {
        TimeCheck = REG_RD32(NV_PTIMER_TIME_1);
        TimeLo    = REG_RD32(NV_PTIMER_TIME_0);
        TimeHi    = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != TimeHi);
    //
    // Add nanosecond delay.
    //
    TimeLo += nsec;
    if (TimeLo < nsec)
        TimeHi++;
    //
    // Wait until time catches up.
    //
    while (TimeHi > (U032)((volatile U032)REG_RD32(NV_PTIMER_TIME_1)))
	    ;
#if 0
    //
    // There's a potential hang event (or extremely long delay) that can
    // occur here under special circumstances.
    //
    while (TimeLo > (volatile U032)REG_RD32(NV_PTIMER_TIME_0));
#else
    //
    // Try to avoid infinite delay.
    //
    while ((TimeLo > (U032)((volatile U032)REG_RD32(NV_PTIMER_TIME_0))) &&
           (TimeHi == (U032)((volatile U032)REG_RD32(NV_PTIMER_TIME_1))))
		       ;
#endif

    //
    // Restore PLL slowdown setting
    //
    if (IsNV11(pDev))
        FLD_WR_DRF_NUM(_PBUS, _DEBUG_1, _CORE_SLOWDWN, IsPllSlowed);

    return (RM_OK);
}
RM_STATUS tmrGetCurrentTime
(
	PHWINFO pDev,
    U032 *TimeHi,
    U032 *TimeLo
)
{
    U032 TimeCheck;
    
    do
    {
        TimeCheck = REG_RD32(NV_PTIMER_TIME_1);
        *TimeLo   = REG_RD32(NV_PTIMER_TIME_0);
        *TimeHi   = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != *TimeHi);
    return (RM_OK);
}

RM_STATUS tmrScheduleCallbackRel
(
	PHWINFO pDev,
    TIMEPROC Proc,
    POBJECT  Object,
    U032     RelTimeHi,
    U032     RelTimeLo
)
{
    U032 TimeCheck;
    U032 AbsTimeHi;
    U032 AbsTimeLo;
    
    do
    {
        TimeCheck = REG_RD32(NV_PTIMER_TIME_1);
        AbsTimeLo = REG_RD32(NV_PTIMER_TIME_0);
        AbsTimeHi = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != AbsTimeHi);

    AbsTimeLo += RelTimeLo;
    if (AbsTimeLo < RelTimeLo)
        AbsTimeHi++;
    AbsTimeHi += RelTimeHi;

    return (tmrScheduleCallbackAbs(pDev, Proc, Object, AbsTimeHi, AbsTimeLo));
}

RM_STATUS tmrScheduleCallbackAbs
(
	PHWINFO  pDev,
    TIMEPROC Proc,
    POBJECT  Object,
    U032     TimeHi,
    U032     TimeLo
)
{
    U032           TimeCheck;
    U032           CurrentTimeLo;
    U032           CurrentTimeHi;
    PTIMERCALLBACK tmrInsert;
    PTIMERCALLBACK tmrScan;
    
    do
    {
        TimeCheck     = REG_RD32(NV_PTIMER_TIME_1);
        CurrentTimeLo = REG_RD32(NV_PTIMER_TIME_0);
        CurrentTimeHi = REG_RD32(NV_PTIMER_TIME_1);
    } while (TimeCheck != CurrentTimeHi);

    if ((TimeHi <  CurrentTimeHi)
    || ((TimeHi == CurrentTimeHi) && (TimeLo <= CurrentTimeLo)))
    {
        //
        // Call immediately if time passed.
        //
        Proc(pDev, Object);
    }
    else
    {
        //
        // Schedule callback at appropriate time.
        //
        tmrInsert = pDev->DBtmrCallbackFreeList;
        if (tmrInsert == NULL)
        {
            //
            // Uh-ho.  Out of callback headers.  Call the proc anyway.
            //
            Proc(pDev, Object);
            return (RM_ERR_OUT_OF_TIMER_CALLBACKS);
        }
        pDev->DBtmrCallbackFreeList = pDev->DBtmrCallbackFreeList->Next;
        tmrInsert->Callback = Proc;
        tmrInsert->Object   = Object;
        tmrInsert->TimeLo   = TimeLo;
        tmrInsert->TimeHi   = TimeHi;
        tmrInsert->Next     = NULL;
        if (pDev->DBtmrCallbackList == NULL)
        {
            //
            // Insert this immediatelly.
            //
            pDev->DBtmrCallbackList = tmrInsert;
            REG_WR32(NV_PTIMER_ALARM_0, TimeLo);
        }
        else
        {
            //
            // Scan looking for insert place.
            //
            if ((TimeHi <  pDev->DBtmrCallbackList->TimeHi)
            || ((TimeHi == pDev->DBtmrCallbackList->TimeHi) && (TimeLo <= pDev->DBtmrCallbackList->TimeLo)))
            {
                tmrInsert->Next = pDev->DBtmrCallbackList;
                pDev->DBtmrCallbackList = tmrInsert;
                REG_WR32(NV_PTIMER_ALARM_0, TimeLo);
            }
            else
            {
                tmrScan = pDev->DBtmrCallbackList;
                while (tmrScan->Next)
                {
                    if ((TimeHi <  tmrScan->Next->TimeHi)
                    || ((TimeHi == tmrScan->Next->TimeHi) && (TimeLo <= tmrScan->Next->TimeLo)))
                    {
                        tmrInsert->Next = tmrScan->Next;
                        tmrScan->Next   = tmrInsert;
                        return (RM_OK);
                    }
                    tmrScan = tmrScan->Next;
                }
                tmrScan->Next = tmrInsert;
            }
        }
    }

    //
    // Call service routine to catch any timing window.
    //
    tmrService(pDev);
    return (RM_OK);
}

RM_STATUS tmrCancelCallback
(
	PHWINFO  pDev,
    POBJECT  Object
)
{
    PTIMERCALLBACK tmrDelete;
    PTIMERCALLBACK tmrScan;
    
    if (pDev->DBtmrCallbackList)
    {
        while (pDev->DBtmrCallbackList->Object == Object)
        {
            //
            // Pull from head of list.
            //
            tmrDelete           = pDev->DBtmrCallbackList;
            pDev->DBtmrCallbackList     = pDev->DBtmrCallbackList->Next;
            tmrDelete->Next     = pDev->DBtmrCallbackFreeList;
            pDev->DBtmrCallbackFreeList = tmrDelete;
            if (pDev->DBtmrCallbackList)
                REG_WR32(NV_PTIMER_ALARM_0, pDev->DBtmrCallbackList->TimeLo);
            else
                return (RM_OK);
        }
        tmrScan = pDev->DBtmrCallbackList;
        while (tmrScan->Next)
        {
            //
            // Scan list looking for matchs.
            //
            if (tmrScan->Next->Object == Object)
            {
                tmrDelete           = tmrScan->Next;
                tmrScan->Next       = tmrScan->Next->Next;
                tmrDelete->Next     = pDev->DBtmrCallbackFreeList;
                pDev->DBtmrCallbackFreeList = tmrDelete;
            }
            else
            {
                tmrScan = tmrScan->Next;
            }
        }
    }
    //
    // Call service routine to catch any timing window.
    //
    tmrService(pDev);
    return (RM_OK);
}

#ifdef RM_STATS
RM_STATUS tmrStatTimer
(
    PHWINFO pDev,
    POBJECT dummy
)
{
    //
    // Move current service counts to exception counts .
    //
    pDev->Statistics.FifoExceptionCount        = pDev->Fifo.ServiceCount;
    pDev->Statistics.FramebufferExceptionCount = pDev->Framebuffer.ServiceCount;
    pDev->Statistics.GraphicsExceptionCount    = pDev->Graphics.ServiceCount;
    pDev->Statistics.TimerExceptionCount       = pDev->Timer.ServiceCount;
    pDev->Statistics.TotalExceptionCount       = pDev->Chip.ServiceCount;
    pDev->Statistics.ExecTimeLo                = pDev->Chip.ServiceTimeLo;
    pDev->Statistics.ExecTimeHi                = pDev->Chip.ServiceTimeHi;
    pDev->Statistics.IntTimeLo                 = pDev->Chip.ExceptionTimeLo;
    pDev->Statistics.IntTimeHi                 = pDev->Chip.ExceptionTimeHi;
    //
    // Zero service counts.
    //
    pDev->Fifo.ServiceCount        = 0;
    pDev->Framebuffer.ServiceCount = 0;
    pDev->Graphics.ServiceCount    = 0;
    pDev->Timer.ServiceCount       = 0;
    pDev->Video.ServiceCount       = 0;
    pDev->MediaPort.ServiceCount   = 0;
    pDev->Chip.ServiceCount        = 0;
    pDev->Chip.ServiceTimeLo       = 0;
    pDev->Chip.ServiceTimeHi       = 0;
    pDev->Chip.ExceptionTimeLo     = 0;
    pDev->Chip.ExceptionTimeHi     = 0;
    //
    // Schedule next interval.
    //
    tmrScheduleCallbackRel(pDev,
                           tmrStatTimer,
                           (POBJECT)0x69,
                           pDev->Statistics.UpdateHi,
                           pDev->Statistics.UpdateLo);
    return (RM_OK);
}
#endif

//
// Timer alarm service.
//
V032 tmrService
(
    PHWINFO pDev
)
{
    U032           TimeCheck;
    U032           CurrentTimeLo;
    U032           CurrentTimeHi;
    PTIMERCALLBACK tmrDelete;
    
    REG_WR_DRF_DEF(_PTIMER, _INTR_0, _ALARM, _RESET);
    //
    // Call all callbacks that have expired.
    //
    if (pDev->DBtmrCallbackList)
    {
        do
        {
            TimeCheck     = REG_RD32(NV_PTIMER_TIME_1);
            CurrentTimeLo = REG_RD32(NV_PTIMER_TIME_0);
            CurrentTimeHi = REG_RD32(NV_PTIMER_TIME_1);
        } while (TimeCheck != CurrentTimeHi);

        //
        // Check for expired time.
        //
        while ((pDev->DBtmrCallbackList->TimeHi <  CurrentTimeHi)
           || ((pDev->DBtmrCallbackList->TimeHi == CurrentTimeHi) && (pDev->DBtmrCallbackList->TimeLo <= CurrentTimeLo)))
        {
            //
            // Pull from head of list.
            //
            tmrDelete           = pDev->DBtmrCallbackList;
            pDev->DBtmrCallbackList     = pDev->DBtmrCallbackList->Next;
            tmrDelete->Next     = pDev->DBtmrCallbackFreeList;
            pDev->DBtmrCallbackFreeList = tmrDelete;
            //
            // Call callback.  This could insert a new callback into the list.
            //
            tmrDelete->Callback(pDev, tmrDelete->Object);
            if (!pDev->DBtmrCallbackList)
                break;
            REG_WR32(NV_PTIMER_ALARM_0, pDev->DBtmrCallbackList->TimeLo);
            do
            {
                TimeCheck     = REG_RD32(NV_PTIMER_TIME_1);
                CurrentTimeLo = REG_RD32(NV_PTIMER_TIME_0);
                CurrentTimeHi = REG_RD32(NV_PTIMER_TIME_1);
            } while (TimeCheck != CurrentTimeHi);
        }
    }    
    return (REG_RD32(NV_PTIMER_INTR_0));
}
