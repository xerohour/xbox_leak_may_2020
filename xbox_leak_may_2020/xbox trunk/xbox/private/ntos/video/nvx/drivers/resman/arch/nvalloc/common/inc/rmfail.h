 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1999 NVIDIA, Corp.  All rights reserved.        *|
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

/*
 * header file for resource manager failure injection (testing) support
 */

#ifdef DEBUG_RM_FAILURES

#define RM_FAILURES_MAX 1024  /* or whatever */
int rm_failure[RM_FAILURES_MAX];
int rm_failure_counts[RM_FAILURES_MAX];

// personality flags for the triggers
#define RM_FAILURES_FLAG_BREAK_ON_TRIGGER        0x80000000
#define RM_FAILURES_FLAG_DONT_DISABLE_ON_TRIGGER 0x40000000

// these occupy the bottom half of the word
#define RM_FAILURES_MASK_PERCENT 0x8000
#define RM_FAILURES_MAX_COUNT    0x7fff
#define RM_FAILURES_MASK_COUNT   0x7fff

#define RM_FAILURE_TRIGGER_ENABLED(n) \
          (rm_failure[(n)] != 0)

#define RM_FAILURE_TRIGGERED(n) \
          RmFailureTriggered((n))

#define RM_FAILURE_TRIGGER_DISABLE(n) \
          rm_failure[(n)] = 0

#define RM_FAILURE_TEST(n, trigger) \
    do { \
        if (RM_FAILURE_TRIGGER_ENABLED((n))) { \
             if (RM_FAILURE_TRIGGERED((n))) { \
                trigger;	/* trigger the error */ \
             } \
         } \
    } while (0)
        

#define RM_FAILURE_ENABLE()  RmFailuresEnable()
#define RM_FAILURE_DISABLE() RmFailuresDisable()

int RmFailureTriggered(int n);
void RmFailuresEnable(void);
void RmFailuresDisable(void);
 
// how do the RM_FAILURE routines "printf" ?

#define RM_FAILURE_PRINTF(args)  DbgPrint args

#else	/* ! DEBUG_RM_FAILURES */
#define RM_FAILURE_TEST(n, trigger)
#define RM_FAILURE_ENABLE()
#define RM_FAILURE_DISABLE()

#endif  /* DEBUG_RM_FAILURES */

//
// convenience macros to assign ranges to tests
//

#define RM_FAILURE_OS(n, trig)          RM_FAILURE_TEST( 00+(n), trig)
#define RM_FAILURE_DAC(n, trig)         RM_FAILURE_TEST( 20+(n), trig)
#define RM_FAILURE_BTREE(N, TRIG)       RM_FAILURE_TEST( 40+(N), TRIG)
#define RM_FAILURE_CLASS(N, TRIG)       RM_FAILURE_TEST( 60+(N), TRIG)
#define RM_FAILURE_DEVINIT(N, TRIG)     RM_FAILURE_TEST( 80+(N), TRIG)
#define RM_FAILURE_DMA(N, TRIG)         RM_FAILURE_TEST(100+(N), TRIG)
#define RM_FAILURE_FB(N, TRIG)          RM_FAILURE_TEST(120+(N), TRIG)
#define RM_FAILURE_FIFO(N, TRIG)        RM_FAILURE_TEST(140+(N), TRIG)
#define RM_FAILURE_GR(N, TRIG)          RM_FAILURE_TEST(160+(N), TRIG)
#define RM_FAILURE_HEAP(N, TRIG)        RM_FAILURE_TEST(180+(N), TRIG)
#define RM_FAILURE_MC(N, TRIG)          RM_FAILURE_TEST(200+(N), TRIG)
#define RM_FAILURE_MODULAR(N, TRIG)     RM_FAILURE_TEST(220+(N), TRIG)
#define RM_FAILURE_MP(N, TRIG)          RM_FAILURE_TEST(240+(N), TRIG)
#define RM_FAILURE_NVKERNEL(N, TRIG)    RM_FAILURE_TEST(260+(N), TRIG)
#define RM_FAILURE_STATE(N, TRIG)       RM_FAILURE_TEST(280+(N), TRIG)
#define RM_FAILURE_TMR(N, TRIG)         RM_FAILURE_TEST(300+(N), TRIG)
#define RM_FAILURE_VBLANK(N, TRIG)      RM_FAILURE_TEST(320+(N), TRIG)
