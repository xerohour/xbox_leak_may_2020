#ifndef _VBLANK_H_
#define _VBLANK_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/***************************** VBlank Module *******************************\
*                                                                           *
* Module: VBLANK.H                                                          *
*       VBlank notification structures.  VBlank is used as a scheduling     *
*   mechanism for events that are closely tied to the video refresh rate.   *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    02/25/95 - rewrote it.                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

typedef RM_STATUS (*VBLANKPROC)(PHWINFO, POBJECT, U032, V032, RM_STATUS);
typedef RM_STATUS (*VBLANKCALLBACKPROC)(PHWINFO, POBJECT, U032, U032, RM_STATUS);
//---------------------------------------------------------------------------
//
//  Time objects.
//
//---------------------------------------------------------------------------

typedef struct _def_vblank_notify
{
    VBLANKPROC Proc;
    POBJECT    Object;
    PDMAOBJECT Xlate;
    U032       Pending;
    U032       Action;
    V032       Param1;
    V032       Param2;
    RM_STATUS  Status;
    struct _def_vblank_notify *Next;
} VBLANKNOTIFY, *PVBLANKNOTIFY;

#define CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT        0x00000001
#define CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP    0x00000002

typedef struct _def_vblank_callback
{
    VBLANKCALLBACKPROC Proc;
    POBJECT            Object;
    U032               Param1;
    U032               Param2;
    U032               VBlankCount;
    U032               Flags;
    RM_STATUS          Status;
    struct _def_vblank_callback *Next;
} VBLANKCALLBACK, *PVBLANKCALLBACK;
//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

VOID VBlank(PHWINFO);
VOID VBlankAddCallback(PHWINFO, U032, PVBLANKCALLBACK);
VOID VBlankDeleteCallback(PHWINFO, U032, PVBLANKCALLBACK);
U032 VBlankPending(PHWINFO);

#define NV4_TV_DAC_SHIFT    0x00010004
								// due to a bug in NV4 HW, the cursor and video scalar are shifted,
                                // so we add an adjustment to the position
#ifdef  DAC_HACK
VOID FixupPcJoystick(VOID);
#endif  // DAC_HACK

#endif // _VBLANK_H_
