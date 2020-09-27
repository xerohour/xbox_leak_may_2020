/******************************************************************************
*
*	Module: smp.h
*
*	Description:
*		This file contains macros that replace calls to functions containing 
*	critical code with calls to SMP-safe wrappers for the functions.  It is
*	only meant to be included in those modules that contain calls to these
*	functions, and not the modules that define these functions.
*
*	THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
*	NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
*	IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
*
******************************************************************************/

#if defined(NTRM) && !defined(UNIX) && !defined(MACOS)

#if (_WIN32_WINNT < 0x0500)

// replacement macros for SMP-safe wrapper functions
#define fifoUpdateObjectInstance(a, b, c, d) smpFifoUpdateObjectInstance(a, b, c, d)
#define fifoDeleteObjectInstance(a, b, c) smpFifoDeleteObjectInstance(a, b, c)
#define fifoResetChannelState(a, b) smpFifoResetChannelState(a, b)
#define fifoDeleteSubchannelContextEntry(a, b) smpFifoDeleteSubchannelContextEntry(a, b)
#define nvHalFbSetAllocParameters(a, b) smpNvHalFbSetAllocParameters(a, b)
#define nvHalFbFree(a, b) smpNvHalFbFree(a, b)
#define VBlankDeleteCallback(a, b, c) smpVBlankDeleteCallback(a, b, c)
#define classDirectSoftwareMethod(a, b, c, d) smpClassDirectSoftwareMethod(a, b, c, d)

// SMP-safe wrapper function prototypes
RM_STATUS smpFifoUpdateObjectInstance
(
	PHWINFO pDev,
	POBJECT Object,
	U032    ChID,
	U032    Instance
);
RM_STATUS smpFifoDeleteObjectInstance
(
	PHWINFO pDev,
	POBJECT Object,
	U032    ChID
);
RM_STATUS smpFifoResetChannelState
(
	PHWINFO pDev,
	U032    ChID
);
RM_STATUS smpFifoDeleteSubchannelContextEntry
(
    PHWINFO pDev,
    POBJECT Object
);
RM_STATUS smpNvHalFbSetAllocParameters
(
	PHWINFO pDev,
    PFBALLOCINFO pFbAllocInfo
);
RM_STATUS smpNvHalFbFree
(
	PHWINFO pDev,
    U032 hwResId
);
VOID smpVBlankDeleteCallback
(
	PHWINFO pDev,
    U032 Head,
    PVBLANKCALLBACK Callback
);
RM_STATUS smpClassDirectSoftwareMethod
(
    PHWINFO pDev,
    POBJECT Object,
    U032    Offset,
    V032    Data
);

#endif // (_WIN32_WINNT < 0x0500)
#endif
