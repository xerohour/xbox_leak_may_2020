#ifndef _TMR_H_
#define _TMR_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
//
/****************************** Timer Module *******************************\
*                                                                           *
* Module: TMR.H                                                             *
*       Timer functions.                                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)    01/24/95 - rewrote it.                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

#define NUM_TIMERCALLBACKS              32
typedef RM_STATUS (*TIMEPROC)(PHWINFO, POBJECT);

//---------------------------------------------------------------------------
//
//  Time objects.
//
//---------------------------------------------------------------------------

struct _def_context_time_object
{
    OBJECT   Base;
    U032     TimeLo;
    U032     TimeHi;
    TIMEPROC TimeProc;
    POBJECT  TimeChange;
};
typedef struct _def_timer_object
{
    COMMONOBJECT    Common;
//    PDMAOBJECT NotifyXlate;
//    U032       NotifyAction;
    U032            State;
    U032            AlarmLo;
    U032            AlarmHi;
} TIMEROBJECT, *PTIMEROBJECT;

//---------------------------------------------------------------------------
//
//  Timer callbacks.
//
//---------------------------------------------------------------------------

typedef struct _def_timer_callback
{
    TIMEPROC Callback;
    POBJECT  Object;
    U032     TimeLo;
    U032     TimeHi;
    struct   _def_timer_callback *Next;
} TIMERCALLBACK, *PTIMERCALLBACK;

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

RM_STATUS initTmr(PHWINFO);
RM_STATUS tmrCreateTimer(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS tmrDeleteTimer(VOID*, POBJECT);
RM_STATUS tmrAlarmNotify(PHWINFO, POBJECT);
RM_STATUS tmrStatTimer(PHWINFO, POBJECT);
RM_STATUS tmrGetCurrentTime(PHWINFO, U032 *, U032 *);
RM_STATUS tmrDelay(PHWINFO, U032);
RM_STATUS tmrScheduleCallbackAbs(PHWINFO, TIMEPROC, POBJECT, U032, U032);
RM_STATUS tmrScheduleCallbackRel(PHWINFO, TIMEPROC, POBJECT, U032, U032);
RM_STATUS tmrCancelCallback(PHWINFO, POBJECT);
RM_STATUS tmrStopTransfer(PHWINFO, POBJECT, PMETHOD, U032, V032);
V032      tmrService(PHWINFO);
#endif // _TMR_H_
