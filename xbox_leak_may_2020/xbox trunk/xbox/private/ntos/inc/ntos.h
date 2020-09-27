/*++ BUILD Version: 0006    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    ntos.h

Abstract:

    Top level include file for the NTOS component.

Author:

    Steve Wood (stevewo) 28-Feb-1989


Revision History:

--*/

#ifndef _NTOS_
#define _NTOS_

#ifdef __cplusplus
extern "C" {
#endif

#include <nt.h>
#include <ntrtl.h>

/* disable undesirable warnings at warning level 4 */
#pragma warning(disable:4214 4201 4115 4200 4100 4514 4057 4127)

#include "ntosdef.h"
#include "exboosts.h"
#include "bugcodes.h"
#include "init.h"

#ifdef _X86_
#include "i386.h"
#endif

#include "ke.h"
#include "kd.h"
#include "ex.h"
#include "ps.h"
#include "io.h"
#include "ob.h"
#include "mm.h"
#include "fscache.h"
#include "xpcicfg.h"
#include "hal.h"

#define _NTDDK_

//
// Temp. Until we define a header file for types
// Outside of the kernel these are exported by reference
//

#if !defined(_NTSYSTEM_)
extern POBJECT_TYPE ExEventObjectType;
extern POBJECT_TYPE ExMutantObjectType;
extern POBJECT_TYPE ExSemaphoreObjectType;
extern POBJECT_TYPE ExTimerObjectType;
extern POBJECT_TYPE PsProcessObjectType;
extern POBJECT_TYPE PsThreadObjectType;
extern POBJECT_TYPE IoCompletionObjectType;
extern POBJECT_TYPE IoDeviceObjectType;
extern POBJECT_TYPE IoFileObjectType;
extern POBJECT_TYPE ObDirectoryObjectType;
extern POBJECT_TYPE ObSymbolicLinkObjectType;
#else
extern OBJECT_TYPE ExEventObjectType;
extern OBJECT_TYPE ExMutantObjectType;
extern OBJECT_TYPE ExSemaphoreObjectType;
extern OBJECT_TYPE ExTimerObjectType;
extern OBJECT_TYPE PsProcessObjectType;
extern OBJECT_TYPE PsThreadObjectType;
extern OBJECT_TYPE IoCompletionObjectType;
extern OBJECT_TYPE IoDeviceObjectType;
extern OBJECT_TYPE IoFileObjectType;
extern OBJECT_TYPE ObDirectoryObjectType;
extern OBJECT_TYPE ObSymbolicLinkObjectType;
#endif

#ifdef __cplusplus
}
#endif

#endif // _NTOS_
