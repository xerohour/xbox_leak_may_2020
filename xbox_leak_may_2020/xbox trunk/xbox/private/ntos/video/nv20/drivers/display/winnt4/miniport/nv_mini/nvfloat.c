//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     nvFloat.C
//
// Abstract:
//
//     Contains routines for protecting the Floating Point operations for the modeset code.
//     Note: 
//     - I had to create a seperate file for these routines since I could not include
//     ntddk.h in nv.c as that would lead to redifinition compiler errors in miniport.h
//     and video.h etc..
//     - We can not include nv.h in nvFloat due to conflict with ntddk.h. 
//     Hence no reference to HwDeviceExtension structure is allowed in this file.
//
// Environment:
//
//     kernel mode only
//
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

#include "ntddk.h"
#include "nvMultiMon.h"
#include "nvsvc_public.h"

//
// These definitions are not found in ntddk.h for NT4.0, so manually define them.
//
#if _WIN32_WINNT < 0x0500
      //
      // The non-volatile 387 state
      //
        
      typedef struct _KFLOATING_SAVE {
          ULONG   DoNotUse1;
          ULONG   DoNotUse2;
          ULONG   DoNotUse3;
          ULONG   DoNotUse4;
          ULONG   DoNotUse5;
          ULONG   DoNotUse6;
          ULONG   DoNotUse7;
          ULONG   DoNotUse8;
      } KFLOATING_SAVE, *PKFLOATING_SAVE;
  

      NTKERNELAPI
      KeSaveFloatingPointState (
         OUT PKFLOATING_SAVE     FloatSave
         );
      
      NTKERNELAPI
      KeRestoreFloatingPointState (
         IN PKFLOATING_SAVE      FloatSave
         );
#endif // #if _WIN32_WINNT < 0x0500

VOID InitializeFPCode(VOID **ppSave, VOID **pMutex);
VOID EnterFPCode(VOID *pSave, VOID *pMutex);
VOID ExitFPCode(VOID *pSave, VOID *pMutex);

//VOID CheckForcedResolution(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG ulWidth, ULONG ulHeight, ULONG ulDepth, ULONG ulRefreshRate, BOOLEAN SetBestResolution );
//
// moved to nv.c because I can't include nv.h here (since ntddk.h is included)  
//
// The build dependency tree is a mess!


//
// Exported event functions
//
VOID SignalModeSwitchEvent();
VOID SignalNVCplHotKeyEvent();
VOID InitNVSVCEvents();

//
// internal helper event function.
//
VOID HandleEvent(PCSZ pEventName, ULONG ulAction);


//
// Does a one time initialization to create the mutex and the floating point save buffer
// This function does the following
// - Initilize the mutex. Supported only for Win2K and later.
// - Initialize the buffer to save the FP state. 
//
VOID InitializeFPCode(VOID **ppSave, VOID **ppMutex)
{
    *ppSave = NULL;
    *ppMutex = NULL;

    // Allocate the FP state save area.
    // 0 is for nonpaged pool.
    *ppSave = ExAllocatePool(NonPagedPool, sizeof(KFLOATING_SAVE));

#ifdef ENABLE_FP_MUTEX
#if (_WIN32_WINNT >= 0x0500)
    // Initialize the fast mutex. These are defined only in win2K.
    *ppMutex = ExAllocatePool(NonPagedPool, sizeof(FAST_MUTEX));
    if (*ppMutex == NULL)
    {
        return;
    }
    ExInitializeFastMutex((FAST_MUTEX *)*ppMutex);
#endif
#endif ENABLE_FP_MUTEX
}

//
// Call this function before doing Floatng point operations. Every call to EnterFPMutex() _should_ be matched with
// a corresponding call to ExitFPMutex().
// - Acuires the mutex inorder to disable all interrupts so that this thread does not get time sliced by the OS scheduler.
// - Saves the FP state.
//
VOID EnterFPCode(VOID *pSave, VOID *pMutex)
{
    NTSTATUS Status;
    // First Disable the interrupts
    // For NT4.0, we use the brute force way of cli/sti.
    // For Win2K, we use fast mutexes.
#ifdef ENABLE_FP_MUTEX
#if (_WIN32_WINNT >= 0x0500)
    if (pMutex != NULL)
    {
        // Acquire the mutex now.
        ExAcquireFastMutex((FAST_MUTEX *)pMutex);
    }
#else
    //
    // NT 4.0 case
    // Note: Do not do any registry reads while interrupts are disabled as that causes NT4.0
    // to BSOD.
    //
     _asm
     {
        cli
     }
#endif (_WIN32_WINNT >= 0x0500)
#endif ENABLE_FP_MUTEX

    // If the FP state save area was not created, we do nothing and exit.
    if (pSave == NULL)
    {
        return;
    }

    // Now save the FP state.
    Status = KeSaveFloatingPointState((KFLOATING_SAVE *)pSave);

}


//
// Call this function before exiting Floatng point operations. Every call to EnterFPMutex() _should_ be matched with
// a corresponding call to ExitFPMutex().
// - Releases the mutex
// - Restores the FP state.
//
VOID ExitFPCode(VOID *pSave, VOID *pMutex)
{
    NTSTATUS Status;

    // If the FP state save area was created, we restore the Floating Point state.
    if (pSave != NULL)
    {
        Status = KeRestoreFloatingPointState((KFLOATING_SAVE *)pSave);
    }

    // Now enable the interrupts.
    // For NT4.0, we use the brute force way of cli/sti.
    // For Win2K, we use fast mutexes.
#ifdef ENABLE_FP_MUTEX
#if (_WIN32_WINNT >= 0x0500)
    if (pMutex != NULL)
    {
        // Release the mutex now.
        ExReleaseFastMutex((FAST_MUTEX *)pMutex);
    }
#else
    // NT 4.0 case
    // Note: Do not do any registry reads while interrupts are disabled as that causes NT4.0
    // to BSOD.
     _asm
     {
        sti
     }
#endif (_WIN32_WINNT >= 0x0500)
#endif ENABLE_FP_MUTEX

}


//
// Signals the "NVModeSwitchEvent" object to wake up the NVSVC to induce a modeset. The actual deviceMask
// to be used by the nvPanel will be returned in an escape callNV_ESC_INDUCE_MODESET
//
VOID SignalModeSwitchEvent()
{
    HandleEvent(NVSVC_EVENT_MODESWITCH_DRIVER, EVENT_ACTION_SIGNAL);
}

//
// Signals the "NVCplHotKeyEvent" object to let the control panel thread know that a 
// hotkey device switch has taken place.
//
VOID SignalNVCplHotKeyEvent()
{
    HandleEvent(NVCPL_EVENT_HOTKEY_DRIVER, EVENT_ACTION_SIGNAL);
}

//
// Create the events that NVSVC sleeps on and set them to cleared state. We need to
// explicitly clear the created event since they would be created with signalled state.
//
VOID InitNVSVCEvents()
{
    UNICODE_STRING EventUnicodeString;
    PKEVENT EventPtr = NULL;
    HANDLE  EventHandle;

    HandleEvent(NVSVC_EVENT_ICONBEGIN_DRIVER, EVENT_ACTION_CREATE);
    HandleEvent(NVSVC_EVENT_ICONEND_DRIVER, EVENT_ACTION_CREATE);
    HandleEvent(NVSVC_EVENT_MODESWITCH_DRIVER, EVENT_ACTION_CREATE);

    //
    // This event is _not_ used by NVSVC. It is used for a special case control panel thread. If the control
    // panel is already open, and the user preses a hotkey on laptops, control panel needs to know that
    // a hotkey device switch has happened. This is because if the same desktop mode is maintained after the 
    // device switch, the OS does not send a WM_DISPLAYCHANGE message to the control panel thread.
    //
    HandleEvent(NVCPL_EVENT_HOTKEY_DRIVER, EVENT_ACTION_CREATE);
}



//
// This helper function performs the specified operation on the requested named event.
// Arguements:
// pEventName:      In parameter: An event name such as  "\\BaseNamedObjects\\SomeEventName" passed as constant string. 
//                  These names are defined uniquely in either nvsvc_public.h or nvMultiMon.h
// ulAction:        In Parameter: 
//                  EVENT_ACTION_CREATE: creates the named event and sets it to cleared state.
//                  EVENT_ACTION_SIGNAL: signals the named event.
//                  EVENT_ACTION_CLEAR:  clears the named event.                        
// Return Value:    None
//
VOID HandleEvent(PCSZ pEventName, ULONG ulAction)
{
    ANSI_STRING sAnsiString;
    ULONG NameLength;
    UNICODE_STRING sEventUniCodeString;
    PKEVENT EventPtr = NULL;
    HANDLE  EventHandle;
    WCHAR   NameBuffer[NVSVC_EVENT_NAME_MAX_LENGTH];

    //
    // Sanity checks.
    //
    if (pEventName == NULL)
    {
        return;
    }
    NameLength = strlen(pEventName);
    if (NameLength > NVSVC_EVENT_NAME_MAX_LENGTH)
    {
        return;
    }
       
    //
    // convert the requested reguar string to an ansi string.
    //
    RtlInitAnsiString(&sAnsiString, pEventName);
    //
    // Allocate the buffer for the string for the unicode
    //
    sEventUniCodeString.Length = sEventUniCodeString.MaximumLength = sizeof(NameBuffer);
    sEventUniCodeString.Buffer = (PWSTR)NameBuffer;
    //
    // Convert the ansi string to a unicode string.
    //
    RtlAnsiStringToUnicodeString(&sEventUniCodeString, &sAnsiString, FALSE);
    //
    // Finally, get the event pointer and signal the event.
    //
    EventPtr = IoCreateSynchronizationEvent(&sEventUniCodeString, &EventHandle);
    if (EventPtr)
    {
        //
        // Perform the requested operation
        //
        switch (ulAction)
        {
        case EVENT_ACTION_CREATE:
            //
            // We need to clear because a newly created event gets born in a signalled state.
            //
            KeClearEvent(EventPtr);
            break;
        case EVENT_ACTION_CLEAR:
            KeClearEvent(EventPtr);
            break;
        case EVENT_ACTION_SIGNAL:
            KeSetEvent(EventPtr, 0, FALSE);
            break;
        default:
            break;
        }
    }
}
