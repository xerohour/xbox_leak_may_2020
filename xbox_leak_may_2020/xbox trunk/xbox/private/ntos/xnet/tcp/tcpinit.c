/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tcpinit.c

Abstract:

    TCP/UDP initialization and cleanup related functions.

Revision History:

    05/24/2000 davidx
        Created it.

    06/30/2000 davidx
        Added TCP timer functions here.

--*/

#include "precomp.h"

//
// TCP timer objects and function
//
PRIVATE KTIMER TcpFastTimer;
PRIVATE KDPC TcpFastTimerDpc;
PRIVATE UINT TcpTimerFlag;

// TCP slow timer tick count (incremented every 500ms)
UINT TcpTickCount;

// Fast TCP timer fires every 200ms
#define FAST_TCP_TIMER_FREQ 5


PRIVATE VOID
TcpFastTimerProc(
    PKDPC dpc,
    VOID* context,
    VOID* param1,
    VOID* param2
    )

/*++

Routine Description:

    TCP fast (200ms cycle) timer routine

Arguments:

    dpc, context, param1, param2 - DPC function parameters

Return Value:

    NONE

--*/

{
    PCB* pcb;
    TCB* tcb;
    UINT isSlowTick;

    TcpTimerFlag = TcpTimerFlag << 1;

    // The slow timer has 500ms cycle.
    // NOTE: 500 is not an exact multiple of 200. So we end up firing
    // once after 400ms and again after 600ms. The approximation should
    // be good enough for our purpose. It saves us from having two timers.
    isSlowTick = (TcpTimerFlag & 0x24);
    if (isSlowTick) TcpTickCount++;

    LOOP_THRU_PCB_LIST(pcb)

        if (!IsTcb(pcb)) continue;
        tcb = (TCB*) pcb;

        // The fast timer is used to implement the TCP delayed-ACK feature
        if (tcb->delayedAcks) {
            TcbEmitACK(tcb);
        }

        if (isSlowTick) {
            // Connection establishment timer expired
            if (tcb->synTimer && --tcb->synTimer == 0) {
                if (TcbReset(tcb, NETERR_TIMEOUT) == NULL) continue;
            }

            // Transmission timer expired
            if (tcb->xmitTimer && --tcb->xmitTimer == 0) {
                if (TcbXmitTimeout(tcb) == NULL) continue;
            }

            // Time-wait period expired or linger period expired
            if (tcb->timeWaitTimer && --tcb->timeWaitTimer == 0) {
                ASSERT(TcbIsLingering(tcb));
                TcbDelete(tcb);
            }
        }

    END_PCB_LIST_LOOP

    // IP timer fires once every second
    if (TcpTimerFlag == (1 << FAST_TCP_TIMER_FREQ)) {
        TcpTimerFlag = 1;
        IpTimerProc();
    }
}


NTSTATUS
TcpInitialize()

/*++

Routine Description:

    Initialize TCP/UDP protocol module

Arguments:

    NONE

Return Value:

    Status code

--*/

{
    LARGE_INTEGER dueTime;
    UINT period;
    NTSTATUS status;

    InitializeListHead(&PcbList);
    InitializeListHead(&DeadTcbList);
    PcbCount = 0;

    // Initialize the TCP timers:
    //  fast timer has a 200ms cycle
    //  initial due time is 1sec

    TcpTickCount = 1;
    dueTime.QuadPart = -10000000;

    period = 1000 / FAST_TCP_TIMER_FREQ;
    KeInitializeDpc(&TcpFastTimerDpc, TcpFastTimerProc, NULL);
    KeInitializeTimer(&TcpFastTimer);
    KeSetTimerEx(&TcpFastTimer, dueTime, period, &TcpFastTimerDpc);

    // Initialize the IP module
    status = IpInitialize();
    TcpTimerFlag = 1;
    return status;
}


VOID
TcpCleanup()

/*++

Routine Description:

    Clean up TCP/UDP protocol module

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    RUNS_AT_DISPATCH_LEVEL

    if (!TcpTimerFlag) return;
    TcpTimerFlag = 0;

    // Cancel TCP timers
    KeCancelTimer(&TcpFastTimer);

    // Clean up the IP module
    IpCleanup();
}

