 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/***************************** Timer Module ********************************\
*                                                                           *
* Module: TMRSTATE.C                                                        *
*   The Timer cntrol state is maintained in this module.                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <tmr.h>
#include <os.h>
#include "nvhw.h"


//
// Month to days conversion.
//
U032 tmrMonthDays[12] =
{
    31, 28, 31, 30,
    31, 30, 31, 31,
    30, 31, 30, 31
};

//
// Change Master Control hardware state.
//
RM_STATUS stateTmr
(
    PHWINFO pDev,
    U032    msg
)
{
    RM_STATUS status;
#if defined(UNIX) || defined(MACOS)
    U032      seconds;
    U032      useconds;
    long long mseconds;			// U064 is a struct, not long long
#else    
    U032      year;
    U032      month;
    U032      day;
    U032      hour;
    U032      min;
    U032      sec;
    U032      msec;
    U032      days;
#endif
    U032      sec5;
    
    switch (msg)
    {
        case STATE_UPDATE:
            break;
        case STATE_LOAD:
            //
            // Set timer values.
            //
            REG_WR32(NV_PTIMER_NUMERATOR,   pDev->Timer.Numerator);
            REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.Denominator);
            REG_WR32(NV_PTIMER_ALARM_0,     0xFFFFFFFF);
            //
            // Get current time from operating system.
            //
            
#if defined(UNIX) || defined(MACOS)
            // On UNIX, we get the time in seconds since 1970.
            // From within the unix kernel it is actually hard to get
            //   real time of day.  We don't really need it anyway.
            osGetCurrentTime(&seconds, &useconds);
			DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, "NVRM: Time = ");
			DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, seconds);
			DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, " seconds, ");
			DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, useconds);
			DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, " useconds.\n");
            mseconds = seconds * 1000 + (useconds / 1000);
            sec5 = mseconds / 4295;
#else
            osGetCurrentTime(&year, &month, &day, &hour, &min, &sec, &msec);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, "NVRM: Date = ");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, month);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, "/");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, day);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, "/");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, year);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, " ");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, hour);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, ":");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, min);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, ":");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, sec);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, ":");
            DBG_PRINT_VALUE(DEBUGLEVEL_SETUPINFO, msec);
            DBG_PRINT_STRING(DEBUGLEVEL_SETUPINFO, "\n");

            //
            // Perform a quick error check of the time
            //
            if (month > 12)
                    month = 12;
            if (day > 31)
                    day = 20;
            if (hour > 24)
                    hour = 12;
            if (min > 59)
                    min = 59;
            if (sec > 59)
                    sec = 59;

            //
            // Get days so far.
            //
            if (year > 1990)
                year -= 1990;
            days = year * 365 + (year + 1) / 4;
            //
            // If this is a current leap year, check the month.
            //
            if (!((year + 2) & 3) && (month > 2))
                days++;
            while (--month)
                days += tmrMonthDays[month];
            days += day;
            sec5 = days * 20117 + hour * 838 + (min * 257698 + sec * 4295) / 1000;

            //
            // Add number of 4.295 sec increments between 1970 and 1990 to the 
            // accumulated time.
            //
            sec5 += 146951526;
#endif

            //
            // Since the internal timer is only accurate to about 5 sec/day,
            // that is about the accuracy of the upper 32 bits of the nsec timer.
            // To initialize, just calculate the current time to the nearest 4.295 sec.
            //
            REG_WR32(NV_PTIMER_TIME_0, 0);
            REG_WR32(NV_PTIMER_TIME_1, sec5);
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: TIMER = ");
            DBG_PRINT_VALUE(DEBUGLEVEL_TRACEINFO, sec5);
            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, ":0x00000000\n");

            if (!pDev->Vga.Enabled) // interrupts wake up from standby on some motherboards
            {
                //
                // Enable Timer interrupt.
                //
                REG_WR32(NV_PTIMER_INTR_0,    0xFFFFFFFF);
                REG_WR32(NV_PTIMER_INTR_EN_0, 0xFFFFFFFF);
            }            
#ifdef RM_STATS
            /*
            For now, no perf timer needed!!
            
            //
            // Schedule performance timer.
            //
            tmrScheduleCallbackRel(tmrStatTimer,
                                   (POBJECT)0x69,
                                   pDev->Statistics.UpdateHi,
                                   pDev->Statistics.UpdateLo);
            */
#endif
            break;
        case STATE_UNLOAD:
            //
            // Disable Timer interrupt.
            //
            REG_WR32(NV_PTIMER_INTR_EN_0, 0);
            //
            // Cancel performance timer.
            //
            tmrCancelCallback(pDev, (POBJECT)0x69);
            break;
        case STATE_INIT:
            status = initTmr(pDev);
            break;
        case STATE_DESTROY:
            //
            // Restore previous values.
            //
            REG_WR32(NV_PTIMER_NUMERATOR,   pDev->Timer.tmrSave[0]);
            REG_WR32(NV_PTIMER_DENOMINATOR, pDev->Timer.tmrSave[1]);
            osFreeMem((VOID *)pDev->DBtmrCallbackTable);
            break;
    }    
    return (RM_OK);
}
