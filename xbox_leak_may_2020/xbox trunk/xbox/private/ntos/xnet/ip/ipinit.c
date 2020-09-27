/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    ipinit.c

Abstract:

    IP module initialization and cleanup related functions

Revision History:

    05/24/2000 davidx
        Created it.

--*/

#include "precomp.h"


//
// Global interface table
//
IfInfo* Interfaces[IFINDEX_MAX];

VOID
IpTimerProc()

/*++

Routine Description:

    IP timer routine - called once a second

Arguments:

    dpc, context, param1, param2 - DPC function parameters

Return Value:

    NONE

--*/

{
    IfInfo* ifp;

    // Call interface timer routines
    LOOP_THRU_INTERFACE_LIST(ifp)
        // NOTE: This is a hack to ensure we don't call the Ethernet interface's
        // timer function twice when we are running under the debug monitor.
        if (XnetInsideDbgmon() &&
            ifp->refcount > 1 &&
            IfRunning(ifp))
            continue;

        if (ifp->Timer) ifp->Timer(ifp);
        if (ifp->mcastData) {
            IgmpTimerProc(ifp);
        }
        if (IfDhcpEnabled(ifp)) {
            DhcpTimerProc(ifp);
        }
    END_INTERFACE_LIST_LOOP()

    // Datagram reassembly timer routine
    IpReassemblyTimerProc();
}


//
// Interface initialization function
//
typedef NTSTATUS (*IfInitProc)(IfInfo**);
static const IfInitProc IfInitProcTable[IFINDEX_MAX] = {
    LoopbackInitialize,     // loopback interface
    EnetInitialize,         // Ethernet interface
    NULL                    // dial-up interface
};

NTSTATUS
IpInitialize()

/*++

Routine Description:

    Initialize the IP module

Arguments:

    NONE

Return Value:

    Status code

--*/

{
    NTSTATUS status;
    UINT ifindex;

    status = IpInitRouteTable();
    if (!NT_SUCCESS(status)) goto failed;

    for (ifindex=0; ifindex < IFINDEX_MAX; ifindex++) {
        IfInitProc initProc = IfInitProcTable[ifindex];
        IfInfo* ifp;

        if (!initProc) continue;
        status = initProc(&ifp);
        if (!NT_SUCCESS(status)) goto failed;

        ASSERT(ifp->hwaddrlen <= MAXHWADDRLEN);
        Interfaces[ifindex] = ifp;

        // Adjust the pseudo-random number generator seed
        // with the Ethernet hardware ID
        if (ifp->iftype == IFTYPE_ETHERNET) {
            ULONG seed = 0;
            WORD addrbyte;
            for (addrbyte=0; addrbyte < ifp->hwaddrlen; addrbyte++) {
                seed = (seed << 8) ^ ifp->hwaddr[addrbyte];
            }
            XnetRandSeed = (XnetRandSeed ^ seed) & 0x7fffffff;
        }

        if (IfBcastEnabled(ifp)) {
            status = IpSetBroadcastInterface(ifp);
            if (!NT_SUCCESS(status)) goto failed;
        }

        if (IfMcastEnabled(ifp)) {
            status = IfInitMcastGroup(ifp);
            if (!NT_SUCCESS(status)) goto failed;
        }

        if (IfDhcpEnabled(ifp)) {
            status = DhcpInitialize(ifp);
            if (!NT_SUCCESS(status)) goto failed;
        }
    }
    
    IpNextDgramId = (WORD) XnetRand();
    return NETERR_OK;

failed:
    WARNING_("IpInitialize failed: 0x%x", status);
    return status;
}


VOID
IpCleanup()

/*++

Routine Description:

    Cleanup the IP module

Arguments:

    NONE

Return Value:

    NONE

--*/

{
    IfInfo* ifp;

    RUNS_AT_DISPATCH_LEVEL

    // Cleanup DHCP info
    LOOP_THRU_INTERFACE_LIST(ifp)
        if (IfDhcpEnabled(ifp)) {
            DhcpCleanup(ifp);
        }
    END_INTERFACE_LIST_LOOP()

    IpCleanupReassemblyPkts(NULL);
    IpCleanupRouteTable();

    // Delete the interfaces
    LOOP_THRU_INTERFACE_LIST(ifp)
        Interfaces[_ifindex] = NULL;

        // NOTE: We shouldn't free per-interface multicast data
        // if the interface is shared by the debug monitor stack.
        if (ifp->refcount <= 1) {
            SysFree(ifp->mcastData);
            ifp->mcastData = NULL;
        }

        ifp->Delete(ifp);
    END_INTERFACE_LIST_LOOP()
    ZeroMem(Interfaces, sizeof(Interfaces));
}

