/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       main.c
 *  Content:    Application entry point.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/07/01    dereks  Created.
 *
 ****************************************************************************/

#include "testds.h"

BOOL fTestMe = FALSE;
BOOL fBreakTest = FALSE;


/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "main"

void __cdecl
main
(
    void
)
{
    HRESULT                 hr;
    
    while(TRUE)
    {
        hr = DS_OK;

#ifdef STALL_ON_START

        fTestMe = FALSE;

        DPF_TEST("");
        DPF_TEST("******************************************************************************");
        DPF_TEST("");
        DPF_TEST("Enter the following into the debugger in order to start testing:");
        DPF_TEST("ed testds!fTestMe 1;g");
        DPF_TEST("");
        DPF_TEST("******************************************************************************");
        DPF_TEST("");

        while(!fTestMe);

#elif defined(BREAK_ON_START)

        __asm int 3

#endif // STALL_ON_START

        fBreakTest = FALSE;

        if(SUCCEEDED(hr)) hr = TestDirectSound();
        // if(SUCCEEDED(hr)) hr = TestAc97Xmo(DSAC97_CHANNEL_ANALOG, DSAC97_MODE_PCM);
        // if(SUCCEEDED(hr)) hr = TestAc97Xmo(DSAC97_CHANNEL_DIGITAL, DSAC97_MODE_PCM);
        // if(SUCCEEDED(hr)) hr = TestAc97Xmo(DSAC97_CHANNEL_DIGITAL, DSAC97_MODE_ENCODED);
        // if(SUCCEEDED(hr)) hr = TestMemoryUsage();

        if(SUCCEEDED(hr))
        {
            DPF_TEST("Test successful");
        }
        else
        {
            DPF_TEST("Test failed");
        }

        DirectSoundDumpMemoryUsage(TRUE);
    }
}


