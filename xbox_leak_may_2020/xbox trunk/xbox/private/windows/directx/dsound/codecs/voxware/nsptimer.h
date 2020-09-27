/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1995 Intel Corporation. All Rights Reserved.
//
//  RCS:
//      $Source: d:/rcs/nsp20/test_sys/src/demo/tstapi.h $
//      $Revision:   1.0  $
//      $Date:   10 Feb 1998 14:21:32  $
//
//  Author: Smirnov I.
//
//  Purpose: Lite Test System Program Interface
//
//  Modification Log:
//	95/8/22: Smirnov I. Creation
//
*M*/

#if !defined _TSTAPI_H
# define _TSTAPI_H
#include <malloc.h> /* for size_t */


/*---------------------------------------------------------------*/
/* --- Global defintions ----------------------------------------*/
/*---------------------------------------------------------------*/

#define TSTAPI(ftype)                       extern ftype


/*---------------------------------------------------------------*/
/* --- Benchmark Timing functions -------------------------------*/
/*---------------------------------------------------------------*/

#define USECS     		1000000.0

TSTAPI(void) tstTimerStart(int TimerNo);
TSTAPI(void) tstTimerStop(int TimerNo);
/* TSTAPI(void) tstTimerContinue(int TimerNo); */

TSTAPI(double) tstTimerClock(int TimerNo);
TSTAPI(double) tstTimerSec(int TimerNo);
TSTAPI(double) tstTimerUSec(int TimerNo);

  
#endif /* _TSTAPI_H */
