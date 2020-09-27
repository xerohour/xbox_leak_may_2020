//depot/xbox/private/ntos/ani2/bootsound.cpp#7 - edit change 15970 (text)
/*
 *  Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 */



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>
#include <pci.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include "xtl.h"
#include "xdbg.h"
#include "bootsound.h"

#ifdef STARTUPANIMATION

#pragma data_seg("INIT_RW")
#pragma code_seg("INIT")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")

// Tell linker to put bootsound code and data into INIT section
#pragma comment(linker, "/merge:DSOUND=INIT")

#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")

#endif //STARTUPANIMATION

#if DBG
EXTERN_C g_dwDirectSoundPoolMemoryUsage;
EXTERN_C g_dwDirectSoundPhysicalMemoryUsage;
#endif

EXTERN_C SHORT system_clock_music;

KTIMER    g_BootSoundTimer;
KDPC      g_BootSoundDpc;

VOID
BootSoundDpc(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
	system_clock_music++;
	sos_main();
    return;
}


void BootSound_Start(){
    LARGE_INTEGER dueTime;

    dueTime.QuadPart = 0;

  	do_sos_init_return();

    KeInitializeTimerEx(&g_BootSoundTimer,NotificationTimer);
    KeInitializeDpc(&g_BootSoundDpc,BootSoundDpc,NULL);

    KeSetTimerEx(&g_BootSoundTimer,
                 dueTime,
                 5,
                 &g_BootSoundDpc);
}


// 
// here's our main sos worker thread.  This needs to get called regularly
// it does all the sequencing and writing to the sound chip to make noise
//
// not true...we create a worker thread instead in the init routine



void BootSound_Stop(void){

    KeCancelTimer(&g_BootSoundTimer);

    //
    // free dsound objects
    //

    dev_cleanup();

#if DBG
    //
    // check if we leaked any mem
    //

    ASSERT(g_dwDirectSoundPoolMemoryUsage == 0);
    ASSERT(g_dwDirectSoundPhysicalMemoryUsage == 0);

#endif

}


extern "C" int _cdecl _purecall(
	void
	)
{
	_asm int 3; return 0;
}



