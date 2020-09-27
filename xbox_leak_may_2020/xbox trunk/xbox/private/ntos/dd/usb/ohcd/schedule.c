/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    schedule.c

Abstract:
    
    Implementation of schedule manager.

    The code in this module maintains all of the endpoint lists associated
    with transfer schedule.  Basically scheduling is only involved when a
    new endpoint is opened or closed.

    The work to be done is very different for bulk and control versus
    isochronous and interrupt.

    For bulk and control the endpoint to be open is simply placed at the head
    of a linked list.

    For interrupt and isochronous a slot must be found in the schedule with
    sufficient bandwidth and the bandwidth tables must be updated.

    
Environment:

    Designed for XBOX.

Notes:

Revision History:

    01-20-00 created by Mitchell Dernis (mitchd)

--*/

//
//  Pull in OS headers
//
#include <ntos.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DCHO'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("OHCD");


//
//  Pull in usb headers
//
#include <hcdi.h>       //Interface between USBD and HCD
#include "ohcd.h"       //Private OHCD stuff


#define OHCD_GET_PARENT_INTERRUPT_INDEX(_index_)    ((_index_) >> 1)
#define OHCD_GET_CHILD1_INTERRUPT_INDEX(_index_)    ((_index_) << 1)
#define OHCD_GET_CHILD2_INTERRUPT_INDEX(_index_)    (((_index_) << 1) + 1)
#define OHCD_GET_SIBLING_INTERRUPT_INDEX(_index_)   ((_index_) ^ 1)

//----------------------------------------------------------------------------
// Forward declaration of functions defined and used only this in this module
//----------------------------------------------------------------------------
VOID
FASTCALL
OHCD_HookNewEndpointToChildren(
    POHCD_DEVICE_EXTENSION  DeviceExtension,
    ULONG                   EndpointPhysicalAddress,
    UCHAR                   ParentNode
    );

ULONG 
FASTCALL
OHCD_ReverseBits(
    ULONG NumBits,
    ULONG Value
    );

//----------------------------------------------------------------------------
// Implementation of OHCD_ScheduleXXX routines:
//      OHCD_ScheduleInitialize
//      OHCD_ScheduleAddEndpointPeriodic
//      OHCD_ScheduleRemoveEndpointPeriodic
//      OHCD_ScheduleAddEndpointControlOrBulk
//      OHCD_ScheduleRemoveEndpointControlOrBulk
//----------------------------------------------------------------------------
#pragma code_seg(".XPPCINIT")
VOID
FASTCALL
OHCD_ScheduleInitialize(
    POHCD_DEVICE_EXTENSION DeviceExtension
    )
/*++

Routine Description:
    
    Initializes the internal data structures, operational registers,
    and shared common buffer areas which set the OpenHCI USB packet
    schedule.
Arguments:
    
    DeviceExtension - Pointer to OHCD's device extension.

Return Value:

    None.

--*/
{
    ULONG                   periodicStart;
    ULONG                   hcLSThreshold;
    POHCD_ENDPOINT_SCHEDULE schedule = &DeviceExtension->Schedule;
    
    //
    //  The structure is more or less initialized by
    //  zeroing everything out.  The whole device extension
    //  should have been zeroed before this routine
    //  was called, so on MAX_DBG builds only just
    //  assert that the structure really is clean
#ifdef MAX_DBG
    for(i = 0; i < 64; i++)
    {
        //
        //  Verify that the NULL and 0 fields were wiped
        //  with the rest of the DeviceExtension
        //
        ASSERT(0 == schedule->InterruptSchedule[i].Bandwidth);
        ASSERT(0 == schedule->InterruptSchedule[i].BandwidthChildren);
        ASSERT(0 == schedule->InterruptSchedule[i].BandwidthParents);
        ASSERT(NULL == schedule->InterruptSchedule[i].EndpointHead);
        ASSERT(NULL == schedule->InterruptSchedule[i].EndpointTail);
        ASSERT(FALSE == schedule->InterruptSchedule[i].RegisterHead);
    }
    ASSERT(0 == Schedule->BulkHead);
    ASSERT(0 == Schedule->BulkTail);
    ASSERT(0 == Schedule->ControlHead);
    ASSERT(0 == Schedule->ControlTail);
#endif

    //
    //  No need to initialize the HCCA, because the initial values
    //  are all zero and that was done when the space was allocated
    //  as part of initializing DMA.
    //

    //
    //  Check the alignment requirments of HCCA
    //
    #ifndef OHCD_XBOX_HARDWARE_ONLY
    #if DBG
    {
        ULONG AlignmentCheck;
        WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcHCCA, 0xFFFFFFFF);
        AlignmentCheck = READ_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcHCCA);
        if( (~AlignmentCheck)&((ULONG)(DeviceExtension->HCCA)) )
        {
            USB_DBG_ERROR_PRINT(("The 2nd USB host controller is not compatible with XSS as a second host controller.\
You can try swapping the USB cards.\n XSS will most likely crash if you see this message.\n"));
        }
    }
    #endif  //DBG
    #endif //OHCD_XBOX_HARDWARE_ONLY
    
    

    //
    //  Setup the schedule part of the
    //  Operational registers
    //
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcHCCA, OHCD_PoolGetPhysicalAddress(DeviceExtension->HCCA));
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcPeriodCurrentED, 0);
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcControlHeadED, 0);
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcControlCurrentED, 0);
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcBulkHeadED, 0);
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcBulkCurrentED, 0);
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcDoneHead, 0);


    //
    //  save off the total bandwidth - i.e. total number of bits/frame
    //
    schedule->BandwidthTotal = (USHORT)OHCI_CLOCKS_TO_BITS(OHCI_DEFAULT_FRAME_INTERVAL);

    //
    //  Set the HcPeriodicStart to be roughly 90% of FrameInterval.
    //  A special note, HcPeriodicStart is the clocks remaining when we switch
    //  to isoch.  The silly OpenHCI specification says to set to 10%, but they mean
    //  10% gone.
    //
    periodicStart = (9*OHCI_DEFAULT_FRAME_INTERVAL + 5)/10;
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcPeriodicStart, periodicStart);
    
    //
    //  Calculate the bandwidth (bits\frame) for periodic transfers
    //
    ASSERT(0xFFFF >= periodicStart);
    schedule->BandwidthPeriodic = (USHORT)OHCI_CLOCKS_TO_BITS(periodicStart);

    //
    //  Low speed threshold is the time (in full speed clocks) of 8 byte transfer
    //  for the slowest type (interrupt) of endpoint on a low speed device.
    //
    hcLSThreshold = USBD_CalculateUsbBandwidth(8, USB_ENDPOINT_TYPE_INTERRUPT, TRUE);
    WRITE_REGISTER_ULONG(&DeviceExtension->OperationalRegisters->HcLSThreshold, hcLSThreshold);
}
#pragma code_seg(".XPPCODE")

USBD_STATUS
FASTCALL
OHCD_ScheduleAddEndpointPeriodic(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Adds a periodic (interrupt or isochronous) endpoint into the interrupt schedule.

    1) Determines the nodes in the schedule with the proper rate.
    2) Determines which of the nodes with proper will most balance
    the schedule of periodic transfers.
    3) Assure that the best node, has enough bandwidth.
    4) Recomputes the bandwidth entries in the InterruptSchedule.
    5) Insert the endpoint into the InterruptSchedule.
    6) Revises the pointers in the HCCA and the physical address pointers
    of the endpoint descriptors to reflect the schedule changes.

Arguments:
    
    DeviceExtension - Pointer to OHCD's device extension.
    Endpoint        - Endpoint to add to schedule.

Return Value:

    None.

--*/
{
    POHCD_ENDPOINT_SCHEDULE schedule = &DeviceExtension->Schedule;
    UCHAR   node, nodeTemp, nodeMin, nodeMax, nodeBest;
    USHORT  bestBandwidth, nodeBandwidth;

    /*
    **  See the OpenHCI specification for a nice diagram of the schedule
    **  tree.  Note that the implementation here differs significantly
    **  then example code in the specification and from the win2k and
    **  win98 implementation.  In this those examples a static tree of
    **  endpoint descriptors is constructed that always have their skip
    **  bits set.
    **
    **  Here we have a static array of OHCD_INTERRUPT_SCHEDULE_NODE structures.
    **  Their purpose is two serve as a pointerless static binary tree
    **  for storing endpoint lists which are inserted at the same node in
    **  the schedule.  The array is static and it turns out that the tree
    **  can be traversed very easily and efficiently with bit manipulations.
    **  For example, given a pollin interval that is of a power of 2 (as
    **  more or less required by OpenHCI) it is easy to find the eligible
    **  nodes:  Node 0 is dedicated to isochronous endpoints.  Otherwise,
    **  the first eligible node index equals the polling interval.  There
    **  all nodes are eligible up to twice the polling interval minus.  i.e.
    **  if nodeMin = Polling Interval, than nodeMax = 2*nodeMin-1 or as in
    **  code you can shift left to perform the multiplication.
    **  The parent of a node (i.e. the level with shorter intervals) can be
    **  found by shifting the node index one bit to the right. The first child
    **  (i.e. longer polling intervals) requires shifting the node index one
    **  bit to the left.  The second child is one bit to the left plus one.
    **  Calculating these indices is easy and relieves a good deal of pointer
    **  storage overhead.
    **  
    **  Because we use an independent data structure rather than endpoint
    **  descriptors, we do not burden the endpoint descriptor with as
    **  much extra overhead.  Consequently, our transfer descriptors(TDs) and ED
    **  are 32 bytes long not 64.  The act of inserting an ED in the
    **  schedule is in two stages.  First we inset the Endpoint in a node
    **  in the InterruptSchedule.  Then we have to fix up the HCCA and ED 
    **  pointers to reflect the new change.
    **
    **  We treat the requested PollingInterval (from the USB Endpoint Desriptor -
    **  not to be confused with OpenHCI Endpoint Descriptor) as a maximum.
    **  Thus a requested interval of 10 ms, will end up in an 8 ms
    **  polling node.
    **/
    USB_DBG_ENTRY_PRINT(("Entering OHCD_ScheduleAddEndpointPeriodic"));

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    //  1) Finding the minimum node in the polling interval which is
    //  a power of 2 and less than the requested interval
    //

    if(USB_ENDPOINT_TYPE_INTERRUPT == Endpoint->EndpointType)
    //
    //  If it is interrupt we need to find the node with the most
    //  open bandwidth
    //
    {
        for(nodeMin = 32; nodeMin > Endpoint->PollingInterval; nodeMin >>= 1);
        USB_DBG_TRACE_PRINT(("Requested Polling Interval = %d, Actual Interval = %d.", Endpoint->PollingInterval, (ULONG)nodeMin));
        nodeMax = 2*nodeMin - 1;

    
        //
        //  2) Now check the bandwidth allocation for the worst case frame
        //  which intersects each eligible node.  Note the node with the
        //  least allocated (i.e. least used up) bandwidth.  The formula for this
        //  is to add up BandwidthChildren (already calculated to be
        //  the worst case frame), Bandwidth(i.e. of this node), and the
        //  BandwidthParents.  Each of these variables are maintained across
        //  the tree when an endpoint is inserted (later in this routine)
        //  or when an endpoint is removed from the schedule.
        //
        bestBandwidth = 12000;  //Certainly larger than the available bandwidth
        for(node = nodeMin; node <= nodeMax; node++)
        {
            nodeBandwidth = schedule->InterruptSchedule[node].BandwidthChildren +
                            schedule->InterruptSchedule[node].Bandwidth +
                            schedule->InterruptSchedule[node].BandwidthParents;
            if(nodeBandwidth < bestBandwidth)
            {
                bestBandwidth = nodeBandwidth;
                nodeBest = node;
                USB_DBG_TRACE_PRINT(("Found better node: bestBandwidth = %d, nodeBest = %d.", bestBandwidth , (ULONG)nodeBest));
            }
        }
    }
    else
    //
    //  If it isochronous we know that it is node zero and we either have
    //  the bandwidth or not.
    //
    {
        bestBandwidth = schedule->InterruptSchedule[0].BandwidthChildren +
                        schedule->InterruptSchedule[0].Bandwidth;
        nodeBest = 0;
    }

    //
    //  3) Is there room for this endpoint in the best node?
    //
    if( (bestBandwidth + Endpoint->Bandwidth) > schedule->BandwidthPeriodic )
    {
        USB_DBG_ERROR_PRINT((
            "INSUFFICIENT BANDIWDTH:\n Requested Bandwidth = %d bits/ms\nBest Node = %d\nBandwidth Available = %d bits/ms\n",
            (ULONG)Endpoint->Bandwidth,
            (ULONG)nodeBest,
            (ULONG)(schedule->BandwidthPeriodic - bestBandwidth)
            ));
        return USBD_STATUS_NO_BANDWIDTH;
    }
    Endpoint->ScheduleIndex = nodeBest;

    //
    //  4) Recalculate the bandwidth tables:
    //      a) Update the bandwidth of our node
    //      b) Propogate the change to the BandwidthParents
    //      variable to all of our children.
    //      c) Propogate the change to the Bandwidth Children
    //      to our parents as long as our branch is the worst
    //      case.

    //  a) ours
    schedule->InterruptSchedule[nodeBest].Bandwidth += Endpoint->Bandwidth;

    //  b) BandwidthParents of Childern
    //  We don't need a real recursion.  Since we are a regular binary tree.
    //  We just walk the side branches and iterate in between.
    //
    if(nodeBest == 0)
    {
        //
        //  Things are different for isochronous.
        //
        nodeMin = nodeMax = 1;
    }
    else
    {
        nodeMin = OHCD_GET_CHILD1_INTERRUPT_INDEX(nodeBest);
        nodeMax = OHCD_GET_CHILD2_INTERRUPT_INDEX(nodeBest);
    }
    while(nodeMax <=64)
    {
        for(node = nodeMin; node <= nodeMax; node++)
        {
            schedule->InterruptSchedule[node].BandwidthParents += Endpoint->Bandwidth;
        }
        nodeMin = OHCD_GET_CHILD1_INTERRUPT_INDEX(nodeMin);
        nodeMax = OHCD_GET_CHILD2_INTERRUPT_INDEX(nodeMax);
    }

    //  c) BandwidthChildren to parents
    node = nodeBest;
    while(node > 1)
    {

        //
        //  make nodeTemp point our sibiling
        //
        nodeTemp = OHCD_GET_SIBLING_INTERRUPT_INDEX(node);

        //
        //  If the sibling node and its children use
        //  more bandwidth than the path we are going
        //  down than we can stop, because nothing
        //  beneath needs to change.
        //
        if( 
            (schedule->InterruptSchedule[node].BandwidthChildren
            + schedule->InterruptSchedule[node].Bandwidth)
            <=
            (schedule->InterruptSchedule[nodeTemp].BandwidthChildren
            + schedule->InterruptSchedule[nodeTemp].Bandwidth)
        ) break;

        //
        //  make nodeTemp point to the parent
        //
        nodeTemp = OHCD_GET_PARENT_INTERRUPT_INDEX(node);

        //
        //  Fix parents bandwidth
        //
        schedule->InterruptSchedule[nodeTemp].BandwidthChildren =
            schedule->InterruptSchedule[node].BandwidthChildren +
            schedule->InterruptSchedule[node].Bandwidth;

        //
        //  Ready another iteration (parent becomes new node)
        //
        node = nodeTemp;
    }
    if(node == 1)
    {
        //
        //  if node = 1, it is possible that one was changed, so 
        //  we should update o (for isochronous) which we can
        //  do safely whether it needs updating or not.
        //
        schedule->InterruptSchedule[0].BandwidthChildren =
                schedule->InterruptSchedule[1].BandwidthChildren +
                schedule->InterruptSchedule[1].Bandwidth;
    }

    //
    //  5) Insert the endpoint into the InterruptSchedule.
    //
    
    if(NULL == schedule->InterruptSchedule[nodeBest].EndpointHead)
    {
        USB_DBG_TRACE_PRINT(("Node %d is empty", (ULONG)nodeBest));
        ASSERT(NULL == schedule->InterruptSchedule[nodeBest].EndpointTail);
    
        //
        //  6-a) Hook up new endpoint physical address
        //  to next endpoint, if there is one.
        node = OHCD_GET_PARENT_INTERRUPT_INDEX(nodeBest);
        while( !schedule->InterruptSchedule[node].EndpointHead && node)
        {
            node = OHCD_GET_PARENT_INTERRUPT_INDEX(node);
        }
        if(schedule->InterruptSchedule[node].EndpointHead)
        {
            ASSERT(NULL != schedule->InterruptSchedule[node].EndpointHead);
            USB_DBG_TRACE_PRINT(("Updating new endpoint to link to head of node %d", (ULONG)node));
            Endpoint->HcEndpointDescriptor.NextED = schedule->InterruptSchedule[node].EndpointHead->PhysicalAddress;
        } else
        {
            //
            //  The endpoint memory should have been zeroed when it was allocated.
            //
            ASSERT(0==Endpoint->HcEndpointDescriptor.NextED);
        }

        //
        //  This must be after the above block so isoch works,
        //  because Node 0 is its own parent.
        //
        schedule->InterruptSchedule[nodeBest].EndpointHead = Endpoint;
        schedule->InterruptSchedule[nodeBest].EndpointTail = Endpoint;
        Endpoint->Next = NULL;
    }
    else
    {
        
        ASSERT(NULL != schedule->InterruptSchedule[nodeBest].EndpointTail);
        Endpoint->Next = schedule->InterruptSchedule[nodeBest].EndpointHead;
        schedule->InterruptSchedule[nodeBest].EndpointHead = Endpoint;
        //
        //  6-a) Hook up new endpoint physical address
        //  to next endpoint.
        Endpoint->HcEndpointDescriptor.NextED = Endpoint->Next->PhysicalAddress;
    }
    //
    //  Walk down children and ensure that new endpoint physical address
    //  is in all chains.
    //
    OHCD_HookNewEndpointToChildren(DeviceExtension, Endpoint->PhysicalAddress, nodeBest);

    USB_DBG_EXIT_PRINT(("Exiting OHCD_ScheduleAddEndpointPeriodic"));
    return USBD_STATUS_SUCCESS;
}

VOID
FASTCALL
OHCD_ScheduleRemoveEndpointPeriodic(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Removes a periodic (interrupt or isochronous) endpoint from the interrupt schedule.

Arguments:
    
    DeviceExtension - Pointer to OHCD's device extension.
    Endpoint        - Endpoint to remove

Return Value:

    None.

--*/
{
    POHCD_ENDPOINT_SCHEDULE schedule = &DeviceExtension->Schedule;
    UCHAR node = Endpoint->ScheduleIndex;
    UCHAR nodeMin, nodeMax, nodeTemp, nodeParent;
    POHCD_ENDPOINT previousEndpoint, currentEndpoint;
    ULONG nextPhysicalAddress;
    USHORT endpointBandwidth = Endpoint->Bandwidth;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_ScheduleRemoveEndpointPeriodic"));

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Find endpoint in node list and remove it
    //
    previousEndpoint = NULL;
    currentEndpoint = schedule->InterruptSchedule[node].EndpointHead;
    do
    {
        if(currentEndpoint == Endpoint) break;
        previousEndpoint = currentEndpoint;
        currentEndpoint = currentEndpoint->Next;
    } while(currentEndpoint);
    //
    //  If this assertion fails, than the endpoint 
    //  is not in the schedule.
    ASSERT(Endpoint == currentEndpoint);
    //
    //  If we were the tail, update the tail pointer.
    //
    if(NULL == currentEndpoint->Next)
    {
        schedule->InterruptSchedule[node].EndpointTail = previousEndpoint;
        //
        //  Find the physical address of the next endpoint, if there is one.
        //
        nextPhysicalAddress = 0;
        if(node)  // The tail of the node 0, never has an endpoint after it.
        {
            nodeTemp = OHCD_GET_PARENT_INTERRUPT_INDEX(node);
            while( !schedule->InterruptSchedule[nodeTemp].EndpointHead && nodeTemp)
            {
                nodeTemp = OHCD_GET_PARENT_INTERRUPT_INDEX(nodeTemp);
            }
            if(schedule->InterruptSchedule[nodeTemp].EndpointHead)
            {
                nextPhysicalAddress = schedule->InterruptSchedule[nodeTemp].EndpointHead->PhysicalAddress;
            }
        }
    } else
    {
        nextPhysicalAddress = currentEndpoint->Next->PhysicalAddress;
    }
    //
    //  If we were the head, update the pointer.
    //  
    if(NULL == previousEndpoint)
    {
        schedule->InterruptSchedule[node].EndpointHead = currentEndpoint->Next;
        //
        // recurse to update physical pointers
        //
        OHCD_HookNewEndpointToChildren(
                    DeviceExtension, 
                    nextPhysicalAddress,
                    node
                    );
    }
    else
    //
    //  We were not the head, so update the endpoint
    //  in front of us
    //
    {
        previousEndpoint->Next = currentEndpoint->Next;
        //
        //  Just update physical address of previous endpoint
        //
        previousEndpoint->HcEndpointDescriptor.NextED = nextPhysicalAddress;
    }
    
    //
    //  Recalculate the bandwidth tables:
    //      a) Update the bandwidth of our node
    //      b) Propogate the change to the BandwidthParents
    //      variable to all of our children.
    //      c) Propogate the change to the Bandwidth Children
    //      to our parents as long as our branch is the worst
    //      case.

    //  a) ours
    schedule->InterruptSchedule[node].Bandwidth -= endpointBandwidth;

    //  b) BandwidthParents of Childern
    //  We don't need a real recursion.  Since we are a regular binary tree.
    //  We just walk the side branches and iterate in between.
    //
    if(node == 0)
    {
        //
        //  Things are different for isochronous.
        //
        nodeMin = nodeMax = 1;
    }
    else
    {
        nodeMin = OHCD_GET_CHILD1_INTERRUPT_INDEX(node);
        nodeMax = OHCD_GET_CHILD2_INTERRUPT_INDEX(node);
    }
    while(nodeMax <=64)
    {
        for(nodeTemp = nodeMin; nodeTemp <= nodeMax; nodeTemp++)
        {
            schedule->InterruptSchedule[nodeTemp].BandwidthParents -= Endpoint->Bandwidth;
        }
        nodeMin = OHCD_GET_CHILD1_INTERRUPT_INDEX(nodeMin);
        nodeMax = OHCD_GET_CHILD2_INTERRUPT_INDEX(nodeMax);
    }

    //  c) BandwidthChildren to parents
    while(node > 1)
    {
        //
        //  make nodeTemp point to our sibiling
        //  make nodeParent point to our parent
        //
        nodeTemp = OHCD_GET_SIBLING_INTERRUPT_INDEX(node);
        nodeParent = OHCD_GET_PARENT_INTERRUPT_INDEX(node);

        //
        //  If the sibling node and its children use
        //  more bandwidth than the we can switch 
        //  to the sibling node.
        //
        if( 
            (schedule->InterruptSchedule[node].BandwidthChildren
            + schedule->InterruptSchedule[node].Bandwidth)
            <=
            (schedule->InterruptSchedule[nodeTemp].BandwidthChildren
            + schedule->InterruptSchedule[nodeTemp].Bandwidth)
        )
        {
            //
            //  If the sibling and its children's bandwidth is the same as recorded
            //  for the parent's child, then we can stop.
            //
            if(
                (schedule->InterruptSchedule[nodeTemp].BandwidthChildren + schedule->InterruptSchedule[nodeTemp].Bandwidth) 
                == schedule->InterruptSchedule[nodeParent].BandwidthChildren
            ) {
             break;
            } else
            //
            //  Otherwise, keep walking, but switch to using the sibilings bandwidth
            //
            {
                node = nodeTemp;
            }
        }

        //
        //  Fix parents bandwidth
        //
        schedule->InterruptSchedule[nodeParent].BandwidthChildren =
            schedule->InterruptSchedule[node].BandwidthChildren +
            schedule->InterruptSchedule[node].Bandwidth;
        //
        //  Ready another iteration (parent becomes new node)
        //
        node = nodeParent;
    }
    if(node == 1)
    {
        //
        //  if node = 1, it is possible that one was changed, so 
        //  we should update o (for isochronous) which we can
        //  do safely whether it needs updating or not.
        //
        schedule->InterruptSchedule[0].BandwidthChildren =
                schedule->InterruptSchedule[1].BandwidthChildren +
                schedule->InterruptSchedule[1].Bandwidth;
    }

    USB_DBG_EXIT_PRINT(("Exiting OHCD_ScheduleRemoveEndpointPeriodic"));
}

VOID
FASTCALL
OHCD_ScheduleAddEndpointControlOrBulk(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Adds a control or bulk endpoint to the schedule

Arguments:

    DeviceExtension - Pointer to OHCD's device extension.
    Endpoint        - Endpoint to Add

Return Value:

    None.

--*/

{
    POHCD_ENDPOINT *listHead;
    PULONG          listHeadPhysical;

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Control Pipe
    //
    if(USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType)
    {
        listHead            = &DeviceExtension->Schedule.ControlHead;
        listHeadPhysical    = &DeviceExtension->OperationalRegisters->HcControlHeadED;
    }
    else
    //
    //  Bulk pipe
    //
    {
        listHead            = &DeviceExtension->Schedule.BulkHead;
        listHeadPhysical    = &DeviceExtension->OperationalRegisters->HcBulkHeadED;
    }
    //
    //  insert at head
    //
    Endpoint->Next = *listHead;
    *listHead = Endpoint;
    //
    //  Fixup physical pointers
    //
    if(NULL == Endpoint->Next)
    {
        Endpoint->HcEndpointDescriptor.NextED = 0;
    }
    else
    {
        Endpoint->HcEndpointDescriptor.NextED =
                    Endpoint->Next->PhysicalAddress;
    }
    *listHeadPhysical = Endpoint->PhysicalAddress;
}

VOID
FASTCALL
OHCD_ScheduleRemoveEndpointControlOrBulk(
    POHCD_DEVICE_EXTENSION DeviceExtension,
    POHCD_ENDPOINT Endpoint
    )
/*++

Routine Description:

    Removes a control or bulk endpoint from the schedule.

Arguments:
    
    DeviceExtension - Pointer to OHCD's device extension.
    Endpoint        - Endpoint to remove

Return Value:

    None.

--*/
{
    POHCD_ENDPOINT *listHead;
    POHCD_ENDPOINT  previousEndpoint, currentEndpoint;
    PULONG          listHeadPhysical;

    USB_DBG_ENTRY_PRINT(("Entering OHCD_ScheduleRemoveEndpointControlOrBulk"));

    ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL);

    //
    // Control Pipe
    //
    if(USB_ENDPOINT_TYPE_CONTROL == Endpoint->EndpointType)
    {
        listHead            = &DeviceExtension->Schedule.ControlHead;
        listHeadPhysical    = &DeviceExtension->OperationalRegisters->HcControlHeadED;
    }
    else
    //
    //  Bulk pipe
    //
    {
        listHead            = &DeviceExtension->Schedule.BulkHead;
        listHeadPhysical    = &DeviceExtension->OperationalRegisters->HcBulkHeadED;
    }

    //
    // Find the Endpoint in the list
    //
    previousEndpoint = NULL;
    currentEndpoint = *listHead;
    do
    {
        //stop when we find it
        if(Endpoint == currentEndpoint) break;
        previousEndpoint = currentEndpoint;
        currentEndpoint = currentEndpoint->Next;
    } while( currentEndpoint);
    ASSERT( NULL != currentEndpoint);

    //
    //  Remove from list (virtual and physical).
    //  Notice carefully that we do not change
    //  currentEndpoint->EndpointDescriptor->HcEndpointDescriptor.NextED.
    //  This way if this endpoint is being processed, nothing breaks.
    //  If we wait a frame we can safely remove it.  We do not even
    //  need to pause the endpoint!
    //
    if(previousEndpoint)
    {
        previousEndpoint->Next = currentEndpoint->Next;
        previousEndpoint->HcEndpointDescriptor.NextED =
            currentEndpoint->HcEndpointDescriptor.NextED;
    }
    else
    {
        *listHead = currentEndpoint->Next;
        *listHeadPhysical = 
            currentEndpoint->HcEndpointDescriptor.NextED;
    }
    USB_DBG_EXIT_PRINT(("Exiting OHCD_ScheduleRemoveEndpointControlOrBulk"));
}


//----------------------------------------------------------------------------
// Implementation of local helper routines:
//      OHCD_HookNewEndpointToChildren
//      OHCD_ReverseBits
//----------------------------------------------------------------------------

VOID
FASTCALL
OHCD_HookNewEndpointToChildren(
    POHCD_DEVICE_EXTENSION  DeviceExtension,
    ULONG                   EndpointPhysicalAddress,
    UCHAR                   ParentNode
    )
/*++

Routine Description:

    Helper function to recurse children in the schedule
    table and insert a pointer to the new endpoint.  Used
    for both inserting an endpoint and removing one.

Arguments:
    
    DeviceExtension - Pointer to OHCD's device extension.

Return Value:

    None.

--*/
{
    //USB_DBG_ENTRY_PRINT(("Entering OHCD_HookEndpointToChildren: ParentNode = %d", ParentNode));
    if(ParentNode >= 32)
    //
    //  This node is at the top of the
    //  tree, just fix up HCCA and we are done.
    //  
    {
        POHCI_HCCA  hcca;
        hcca = DeviceExtension->HCCA;
        hcca->InterruptTable[OHCD_ReverseBits(5, ParentNode - 32)] = 
            EndpointPhysicalAddress;
    }
    else
    //
    // Not at the top of the tree, we have two children,
    // deal with one at a time
    //
    {
        UCHAR nodeChild;
        UCHAR index;
        POHCD_INTERRUPT_SCHEDULE_NODE interruptSchedule;
        interruptSchedule = &DeviceExtension->Schedule.InterruptSchedule[0];
        nodeChild = OHCD_GET_CHILD2_INTERRUPT_INDEX(ParentNode);
        for(index = 0; index < 2; index ++) 
        {
            if(NULL == interruptSchedule[nodeChild].EndpointHead)
            //
            //  If this node is empty, skip it and go on to its children
            //
            {
                ASSERT(NULL == interruptSchedule[nodeChild].EndpointTail);
                //USB_DBG_TRACE_PRINT(("Node %d is empty, recursing", nodeChild));
                OHCD_HookNewEndpointToChildren(
                                    DeviceExtension,
                                    EndpointPhysicalAddress,
                                    nodeChild
                                    );
            }
            else
            //
            //  We can fix up this child.
            //
            {
                ASSERT(NULL != interruptSchedule[nodeChild].EndpointTail);
                /*USB_DBG_WARN_PRINT(("Node %d has members: Head = 0x%0.8x; Tail = 0x%0.8x",
                                            nodeChild,
                                            interruptSchedule[nodeChild].EndpointHead,
                                            interruptSchedule[nodeChild].EndpointTail));*/
                interruptSchedule[nodeChild].EndpointTail->HcEndpointDescriptor.NextED = EndpointPhysicalAddress;
            }
            nodeChild = OHCD_GET_CHILD1_INTERRUPT_INDEX(ParentNode);
            //
            //  It is possible that this is the isochronous node,
            //  if so skip the second iteration.
            if(!nodeChild) break;
        }
    }
    //USB_DBG_EXIT_PRINT(("Exiting OHCD_HookEndpointToChildren: ParentNode = %d", ParentNode));
    return;
}

ULONG 
FASTCALL
OHCD_ReverseBits(
    ULONG NumBits,
    ULONG Value)
/*++

Routine Description:

    Helper function to reverse the NumBits
    least significant bits of value (Assume
    remaining bits are 0).

Arguments:
    
    NumBits - number of bits to reverse.
    Value   - value to reverse.

Return Value:

    None.

--*/
{
    ULONG result = 0;
    ULONG index;
    for(index = 0; index < NumBits; index++)
    {
        result <<= 1;
        result += Value&1;
        Value >>= 1;
    }
    return result;
}


