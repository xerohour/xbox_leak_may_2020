/*++

Copyright (c) Microsoft Corporation

Module Name:

    powerdwn.c

Abstract:

    Xbox Auto Power Down

    Provides a 6 hour auto power off timer.

    The following private API's are provided the dashboard
    and internal XSS components to manage the auto power down
    feature.

    XAutoPowerDownSet
    XAutoPowerDownGet
    XAutoPowerDownResetTimer

    For debug builds there is also:

    XAutoPowerDownDebugSetTimeout

Environment:

    XAPI

Notes:

Revision History:

    05-17-01 created by Mitchell Dernis (mitchd)

--*/
#include "basedll.h"
#include <xboxp.h>

#define AUTOPOWERDOWNTIMEOUT (-216000000000)  //This should be 6 hours worth of 100 ns intervals
                                              //6*60*60*1000*1000*10 : negative means from current time.
//---------------------------------------------------------
//  Declaration of Globals used in this module
//---------------------------------------------------------
typedef struct _XAPI_AUTOPOWERDOWN_GLOBAL
{
    KDPC    AutoPowerDownDpc;
    KTIMER  AutoPowerDownTimer;
    BOOL    fAutoPowerDown;
    BOOL    fReadWriteFailure;
#if DBG
    LARGE_INTEGER liTimeOut;  //The timeout is adjustable in debug builds only
#endif
} XAPI_AUTOPOWERDOWN_GLOBAL;
XAPI_AUTOPOWERDOWN_GLOBAL XapiAutoPowerDownGlobals;

//---------------------------------------------------------
//  Forward Declaration of Support Routines
//---------------------------------------------------------
VOID  XapiAutoPowerDownTimeout(PKDPC, PVOID, PVOID, PVOID); //Dpc Timeout

#ifdef XAPILIBP

/*
**  PRIVATE API ACCESSIBLE TO OTHER COMPONENTS
**  OF XSS and THE DASHBOARD
*/

DWORD
XAutoPowerDownSet(BOOL fAutoPowerDown)
/*++
  Routine Description:
   Turns the auto-power-down feature on and off.

  Arguments:
   fAutoPowerDown - The desired new setting.
  
  Return Value:
   Error code returned by XSetValue.

  Remarks:
   
   The only portion of this API that can fail is writing the new setting
   to EEPROM.  The error code returned is the one returned by this
   XSetValue. Regardless of whether the new value was successfully persisted,
   the new setting is adopted for the current session.

   THIS ROUTINE IS NOT THREAD SAFE!!! The only intended client
   is XDash, nobody else should be touching this.
   
--*/
{
    DWORD dwError;
    ULONG ulType, ulSize;
    DWORD dwMiscFlags;

    //
    //  Save the new value of the flag.
    //
    XapiAutoPowerDownGlobals.fAutoPowerDown = fAutoPowerDown;
    
    dwError = XQueryValue(XC_MISC_FLAGS, &ulType, &dwMiscFlags, sizeof(dwMiscFlags), &ulSize);

    //
    //  Sets or cancels the timer as appropriate.
    //
    if(fAutoPowerDown)
    {
        XAutoPowerDownResetTimer();
        dwMiscFlags |= XC_MISC_FLAG_AUTOPOWERDOWN;
    } else
    {
        dwMiscFlags &= ~XC_MISC_FLAG_AUTOPOWERDOWN;
    }

    //
    //  If the query succeeded, then write out the new value.
    //
    if(!dwError)
    {
        dwError = XSetValue(XC_MISC_FLAGS, REG_DWORD, &dwMiscFlags, sizeof(dwMiscFlags));
    }

    //
    //  Whether error or not we need to update the fReadWriteFailure flag.
    //
    XapiAutoPowerDownGlobals.fReadWriteFailure = dwError;

    return dwError;
}

DWORD
XAutoPowerDownGet(BOOL *pfAutoPowerDown)
/*++
  Routine Description:
   Retrieves the current setting of the
   auto-power-down feature.

  Arguments:
   pfAutoPowerDown - pointer to BOOL to receive setting.

  Return Value:
   Error code returned by XAutoPowerDownGet.
 
  Remarks:
   The API maintains the current operating mode of the feature
   separate from the persisted value.

   On each call to XAutoPowerDownGet, the API attempts to reestablish
   coherency between the persisted value and the current operating
   value.  It does this by checking to see if the last read\write
   operation falied.  If it was a failure, it calls
   XAutoPowerDownSet with the current operating value.
   
   In all cases, *pfAutoPowerDown is set to the current operating
   value.
--*/
{
    DWORD dwError = 0;

    //
    //  Save out the return value
    //
    *pfAutoPowerDown = XapiAutoPowerDownGlobals.fAutoPowerDown;

    //
    //  If the last read or write was a failure, try writing
    //  out the setting again.
    //
    if(XapiAutoPowerDownGlobals.fReadWriteFailure)
    {
        dwError = XAutoPowerDownSet(XapiAutoPowerDownGlobals.fAutoPowerDown);
    }
    return dwError;
}


#if DBG
void XAutoPowerDownDebugSetTimeout(LONGLONG llTimeout)
/*++
  Routine Description:
   In debug builds only we need a way to set the timer to something short to test
   the feature.

  Arguments:
   liTimeout - Time in 100 ns intervals to set the timeout.  This value should be negative.
               If the time is not negative, it is the absolute time after boot that the box should
               go off.
--*/
{
    XapiAutoPowerDownGlobals.liTimeOut.QuadPart = llTimeout;
}
#endif

#endif // XAPILIBP

#ifndef XAPILIBP

//---------------------------------------------------------
//  Initialization method called by Xapi Init
//---------------------------------------------------------
void XapiInitAutoPowerDown()
{
    ULONG ulSize, ulType;
    DWORD dwError, dwMiscFlags;

    //
    //  Initialize Dpc and Timer Structures
    //
    KeInitializeDpc(&XapiAutoPowerDownGlobals.AutoPowerDownDpc, XapiAutoPowerDownTimeout, NULL);
    KeInitializeTimer(&XapiAutoPowerDownGlobals.AutoPowerDownTimer);

#if DBG
    //
    //  In release builds, the timeout is hardcoded to six hours
    //  in debug builds it is variable.
    XapiAutoPowerDownGlobals.liTimeOut.QuadPart = AUTOPOWERDOWNTIMEOUT;
#endif

    //
    //  Initialize the feature state from the config settings
    //
    XapiAutoPowerDownGlobals.fAutoPowerDown = FALSE;
    XapiAutoPowerDownGlobals.fReadWriteFailure = FALSE;

    dwError = XQueryValue(XC_MISC_FLAGS, &ulType, &dwMiscFlags, sizeof(dwMiscFlags), &ulSize);
    if(!dwError)
    {
        XapiAutoPowerDownGlobals.fAutoPowerDown = (dwMiscFlags&XC_MISC_FLAG_AUTOPOWERDOWN) ? TRUE : FALSE;
    } else
    {
        XapiAutoPowerDownGlobals.fReadWriteFailure = TRUE;
    }

    //
    //  Start the timer.
    //
    XAutoPowerDownResetTimer();
}

void
XAutoPowerDownResetTimer()
/*++
  Routine Description:
   Resets the AutoPowerDownTimer so that the unit does not shut off.

  Remarks:
   Note that we always set the timer and never check XapiAutoPowerDownGlobals.fAutoPowerDown.
   This is by design.  The DPC timeout routine checks XapiAutoPowerDownGlobals.fAutoPowerDown.

   After some thought, it is not enough to check XapiAutoPowerDownGlobals.fAutoPowerDown here,
   unless you synchronize this routine with XAutoPowerDownSet.  That is a hassle, and it
   it is more code.

   Games must be able to run well with the timer running so you cannot argue that the having the
   timer running negatively affects performance.  In fact, if does negatively affect performance,
   it is even more important that it run all the time.

   So just let the timer run, we will check if the auto-power-down feature is on, only at the last
   moment when we need to turn off the box.

--*/
{
#if DBG
    KeSetTimer(
        &XapiAutoPowerDownGlobals.AutoPowerDownTimer,
        XapiAutoPowerDownGlobals.liTimeOut,
        &XapiAutoPowerDownGlobals.AutoPowerDownDpc
        );
#else
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = AUTOPOWERDOWNTIMEOUT;
    KeSetTimer(&XapiAutoPowerDownGlobals.AutoPowerDownTimer, liDueTime, &XapiAutoPowerDownGlobals.AutoPowerDownDpc);
#endif
}

ULONG XAutoPowerDownTimeRemaining()
/*++
  Routine Description:
    This routine is available to games so that they may poll to see if an auto power down is imminent.
    Developers want this so that they may auto-save before the box shuts down.  Notifying them would be
    too hard, as we would need to call them at DPC.

  Return Value:
    If the auto power down feature is off, the return value is 0xFFFFFFFF.  If the auto power
    down feature is on, the return value is the time in milliseconds until auto power-down.
--*/
{
    KIRQL  oldIrql;
    ULARGE_INTEGER uliTimeRemaining;
    ULONGLONG ullCurrentTime;
    if(!XapiAutoPowerDownGlobals.fAutoPowerDown)
    {
        return 0xFFFFFFFF;
    }
    
    //synchronize access to timer structure
    oldIrql = KeRaiseIrqlToDpcLevel();
    
    //Get the current interrupt time
    ullCurrentTime = KeQueryInterruptTime();

    // Comute the difference, in 100 ns intervals.  The interrupt timer starts out at zero at boot and just runs.
    // With a 64-bit - 100 ns counter - it will wrap once every 58,494 and a quater years.  Don't worry about
    // counter wrap!
    if(XapiAutoPowerDownGlobals.AutoPowerDownTimer.DueTime.QuadPart >= ullCurrentTime)
    {
        uliTimeRemaining.QuadPart = XapiAutoPowerDownGlobals.AutoPowerDownTimer.DueTime.QuadPart - ullCurrentTime;
    } else
    {
        uliTimeRemaining.QuadPart = 0;
    }

    //Done synchronizing.
    KeLowerIrql(oldIrql);

    //Convert from 100 ns intevals to milliseconds
    uliTimeRemaining.QuadPart /= 10000;

    //Since we know that the time was never set for more than
    //six hours, we can trust that the HighPart is now zero.
    return uliTimeRemaining.LowPart;
}

/*
**  INTERNAL SUPPORT METHODS
**  
*/

VOID
XapiAutoPowerDownTimeout(
    PKDPC bogus1,
    PVOID bogus2,
    PVOID bogus3,
    PVOID bogus4
    )
/*++
  Routine Description:
    This is the timeout DPC for the Auto-power-down feature.
    When this routine is called it checks XapiAutoPowerDownGlobals.fAutoPowerDown.
    If TRUE it calls the appropriate Hal method to power down Xbox.
--*/
{
    if(XapiAutoPowerDownGlobals.fAutoPowerDown)
    {
        HalInitiateShutdown();
    }
}

#endif // ! XAPILIBP
