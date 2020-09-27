/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    OHCD.cpp

Abstract:

    WinDbg Extension Api

Author:

    Mitchell Dernis (mitchd) 10/11/2000

Environment:

    User Mode.

Revision History:

--*/

#define _NTSYSTEM_
#include "api.h"
#include "x86api.h"
#include <usbd.h>

#define OHCD_ISOCHRONOUS_SUPPORTED
extern "C"
{
    #include <ohcd.h>
    #include <isoch.h>
}

struct KD_OHCD_ENDPOINT
{
    ULONG  OriginalAddress;
    ULONG  Flags;
    ULONG  ScheduleIndex; // Periodic Only
    ULONG  ScheduleOrder; // All lists.
    union {
        OHCD_ENDPOINT       Endpoint;
        OHCD_ISOCH_ENDPOINT IsochEndpoint;
    };
};

#define KDEPF_CONTROL_LIST          0x00000001
#define KDEPF_BULK_LIST             0x00000002
#define KDEPF_PERIODIC_TABLE        0x00000004
#define KDEPF_PENDING_CLOSE_LIST    0x00000008
#define KDEPF_NOT_IN_PHYSICAL_LIST  0x00000010
#define KDEPF_NOT_IN_VIRTUAL_LIST   0x00000020
#define KDEPF_WRONG_PHYSICAL_LIST   0x00000040
#define KDEPF_WRONG_LIST_TYPE       0x00000080
#define KDEPF_WRONG_PERIODIC_TYPE   0x00000100
#define KDEPF_BAD_ENDPOINT          0x00000200

UCHAR g_DeviceTreeBuffer[sizeof(CDeviceTree)];
CDeviceTree *g_pDeviceTree;
IUsbDevice  *g_pDevices;
USBD_HOST_CONTROLLER     *g_pUsbdHostController;
OHCD_DEVICE_EXTENSION    *g_pOhcdExtension;
OHCI_OPERATIONAL_REGISTERS  g_OperationalRegisters;
OHCI_HCCA                 g_HCCA;
#define MAX_ENDPOINTS_EXPECTED 150
KD_OHCD_ENDPOINT          g_EndpointBuffer[MAX_ENDPOINTS_EXPECTED];
ULONG                     g_EndpointCount;

UCHAR g_HcBuffer[sizeof(USBD_HOST_CONTROLLER)+sizeof(OHCD_DEVICE_EXTENSION)];

//
//  Bandwidth expected at each node in the schedule
//

ULONG g_ExpectedBandwidth[64];
ULONG g_ExpectedChildrenBandwidth[64];
ULONG g_ExpectedParentBandwidth[64];

//
//  Forward Declarations
//
void DumpHostController(ULONG ulHC);
void FindEndpoints();
void DumpEndpoints();
ULONG OHCD_CalculateBandwidth(ULONG MaxPacketSize, BOOL LowSpeed, BOOL Isoch);
void PerformBandwidthCheck();

//
// USBD function definitions
//

DECLARE_API( ohcd )

/*++

Routine Description:

  Validates and dumps the endpoint schedule.

  1) The physical address schedule is validated against the virtual address
  schedule, descrepencies are noted.

  2) Function addresses are validated against the free address list,
  and against the device tree.

  3) Checks the bandwidth data in the schedule for internal consistency.  Reports
  problems.  Including dumping important register information.

  4) Dumps all the endpoints in the transfer schedule.  They are displayed sorted first by
  Function Address and then by endpoint address.  Fairly complete information is provided.

  5) Checks and dumps the number of endpoints in the preallocated list.
  
  6) Summary of types of corruption found.


Arguments:

    args - flags

Return Value:

    None

--*/

{
    UNREFERENCED_PARAMETER (dwProcessor);
    UNREFERENCED_PARAMETER (dwCurrentPc);
    UNREFERENCED_PARAMETER (hCurrentThread);
    UNREFERENCED_PARAMETER (hCurrentProcess);
    UNREFERENCED_PARAMETER (args);
    
    //
    //  Dump Title Header
    //
    dprintf("OpenHCD Endpoint Schedule and Information Dump-----\n");
    
    //
    // First find and load the device tree, it is the only global
    // we can start walking from.
    //

    ULONG ulDeviceTreeAddress = GetExpression( "g_DeviceTree" );
    if(!ulDeviceTreeAddress)
    {
        dprintf("Could not find xapi!g_DeviceTree\n");
        return;
    }

    //
    //  Read in the device tree
    //
    if(FALSE == ReadMemory(ulDeviceTreeAddress, (PVOID)g_DeviceTreeBuffer, sizeof(CDeviceTree), NULL))
    {
       dprintf("Error reading device tree.\n");
       return;
    }
    g_pDeviceTree = (CDeviceTree *)g_DeviceTreeBuffer;

    //
    //  Read in the devices (we need these to find the host controllers,
    //  and later to verify that endpoint address are used by actual
    //  devices.)
    //

    g_pDevices = (IUsbDevice *) new UCHAR[g_pDeviceTree->m_NodeCount*sizeof(IUsbDevice)];
    if(NULL == g_pDevices)
    {
        dprintf("Couldn't allocate memory for XPP devices.");
    }
    
    if(FALSE == ReadMemory((ULONG)g_pDeviceTree->m_Devices, (PVOID)g_pDevices, g_pDeviceTree->m_NodeCount*sizeof(IUsbDevice), NULL))
    {
       dprintf("Error reading devices.\n");
       return;
    }

    //
    //  Count Host Controllers (The first nodes of the
    //  tree would be root hubs
    ULONG ulHostControllerCount = 0;
    while(UDN_TYPE_ROOT_HUB == g_pDevices[ulHostControllerCount].m_Type)
    {
        ulHostControllerCount++;
    }
    dprintf("Host Controllers Found: %d\n", ulHostControllerCount);
    for(ULONG i=0; i<ulHostControllerCount; i++)
    {
        DumpHostController(i);
    }

    //
    //  Free any buffers allocated during this call.
    //
    delete g_pDevices;
}

void DumpHostController(ULONG ulHC)
{

    ULONG ulHCAddress = (ULONG) g_pDevices[ulHC].m_HostController;
    dprintf("--Host Controller @0x%0.8x----------------------\n", ulHCAddress);

    //
    //  Read in the host controller info (The USBD and OHCD portions are one
    //  big block.
    //
    if(FALSE == ReadMemory(ulHCAddress, (PVOID)g_HcBuffer, sizeof(USBD_HOST_CONTROLLER)+sizeof(OHCD_DEVICE_EXTENSION), NULL))
    {
       dprintf("Error reading host controller data.\n");
       return;
    }

    g_pUsbdHostController = (PUSBD_HOST_CONTROLLER)&g_HcBuffer;
    g_pOhcdExtension = (POHCD_DEVICE_EXTENSION)g_pUsbdHostController->HcdExtension;

    //
    //  Read in the OHCD operational registers
    //
    if(FALSE == ReadMemory((ULONG)g_pOhcdExtension->OperationalRegisters, (PVOID)&g_OperationalRegisters, sizeof(OHCI_OPERATIONAL_REGISTERS), NULL))
    {
       dprintf("Error reading operational registers.\n");
       return;
    }

    //
    //  Read in the HCCA area
    //
    if(FALSE == ReadMemory((ULONG)g_pOhcdExtension->HCCA, (PVOID)&g_HCCA, sizeof(OHCI_HCCA), NULL))
    {
       dprintf("Error reading HCCA data.\n");
       return;
    }
    
    //**
    //**  Now we can go on the endpoint hunt.
    //**  (As we find them, check that they appear in the physical
    //**  address table as well.)
    //**
    
    FindEndpoints();
    DumpEndpoints();
    PerformBandwidthCheck();
}


void FindEndpoints()
//
//  Walk all the lists, and read the endpoint data into our global array of endpoints.
//
{
    
    g_EndpointCount = 0;
    
    //
    //  Walk the control endpoint list
    //

    ULONG ulOrder = 0;
    ULONG ulEndpointAddress = (ULONG)g_pOhcdExtension->Schedule.ControlHead;
    while(ulEndpointAddress)
    {
        g_EndpointBuffer[g_EndpointCount].OriginalAddress = ulEndpointAddress;
        g_EndpointBuffer[g_EndpointCount].Flags = KDEPF_CONTROL_LIST;
        g_EndpointBuffer[g_EndpointCount].ScheduleOrder = ulOrder++;
        if(FALSE == ReadMemory(ulEndpointAddress, &g_EndpointBuffer[g_EndpointCount].Endpoint, sizeof(OHCD_ENDPOINT), NULL))
        {
            dprintf("Error Reading Endpoint in Control List: Bad Address (0x%0.8x)\n", ulEndpointAddress);
            g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_BAD_ENDPOINT;
            g_EndpointCount++;
            break;
        }
        ulEndpointAddress = (ULONG) g_EndpointBuffer[g_EndpointCount].Endpoint.Next;
        g_EndpointCount++;
        if(MAX_ENDPOINTS_EXPECTED == g_EndpointCount)
        {
            dprintf("Debug extension found more than %d endpoints.  Increase MAX_ENDPOINTS_EXPECTED and recompile kd extension dll.\n", MAX_ENDPOINTS_EXPECTED);
            return;
        }
    }

    //
    //  walk the bulk endpoint list
    //

    ulOrder = 0;
    ulEndpointAddress = (ULONG)g_pOhcdExtension->Schedule.BulkHead;
    while(ulEndpointAddress)
    {
        g_EndpointBuffer[g_EndpointCount].OriginalAddress = ulEndpointAddress;
        g_EndpointBuffer[g_EndpointCount].Flags = KDEPF_BULK_LIST;
        g_EndpointBuffer[g_EndpointCount].ScheduleOrder = ulOrder++;
        if(FALSE == ReadMemory(ulEndpointAddress, &g_EndpointBuffer[g_EndpointCount].Endpoint, sizeof(OHCD_ENDPOINT), NULL))
        {
            dprintf("Error Reading Endpoint in Bulk List: Bad Address (0x%0.8x)\n", ulEndpointAddress);
            g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_BAD_ENDPOINT;
            g_EndpointCount++;
            break;
        }
        ulEndpointAddress = (ULONG) g_EndpointBuffer[g_EndpointCount].Endpoint.Next;
        g_EndpointCount++;
        if(MAX_ENDPOINTS_EXPECTED == g_EndpointCount)
        {
            dprintf("Debug extension found more than %d endpoints.  Increase MAX_ENDPOINTS_EXPECTED and recompile kd extension dll.\n", MAX_ENDPOINTS_EXPECTED);
            return;
        }
    }
    
    //
    //  walk the periodic schedule table.
    //

    //  Loop over all the nodes.
    for(int nodeIndex = 0; nodeIndex < 64; nodeIndex++)
    {
        ulOrder = 0;
        ulEndpointAddress = (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[nodeIndex].EndpointHead;
        g_ExpectedBandwidth[nodeIndex] = 0;
        while(ulEndpointAddress)
        {
            g_EndpointBuffer[g_EndpointCount].OriginalAddress = ulEndpointAddress;
            g_EndpointBuffer[g_EndpointCount].Flags = KDEPF_PERIODIC_TABLE;
            g_EndpointBuffer[g_EndpointCount].ScheduleOrder = ulOrder++;
            g_EndpointBuffer[g_EndpointCount].ScheduleIndex = nodeIndex;
            if(0==nodeIndex)
            {
                ULONG ulLength;
                if(FALSE == ReadMemory(ulEndpointAddress, &g_EndpointBuffer[g_EndpointCount].IsochEndpoint, sizeof(OHCD_ISOCH_ENDPOINT), &ulLength))
                {
                    if(ulLength >= sizeof(OHCD_ENDPOINT) && (0==g_EndpointBuffer[g_EndpointCount].Endpoint.HcEndpointDescriptor.Control.Format))
                    {
                            dprintf("Error: Non-isoch endpoint (0x%0.8x) found in isoch node of schedule.\n", ulEndpointAddress);
                            g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_WRONG_PERIODIC_TYPE;
                    } else
                    {
                        dprintf("Error Reading Isoch Endpoint in Periodic Table: (0x%0.8x)\n", ulEndpointAddress);
                        g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_BAD_ENDPOINT;
                        g_EndpointCount++;
                        break;
                    }
                }
                
                g_ExpectedBandwidth[0] += OHCD_CalculateBandwidth(
                                                    (ULONG)g_EndpointBuffer[g_EndpointCount].Endpoint.HcEndpointDescriptor.Control.MaximumPacketSize,
                                                    g_EndpointBuffer[g_EndpointCount].Endpoint.HcEndpointDescriptor.Control.Speed,
                                                    TRUE
                                                    );


            } else
            {
                if(FALSE == ReadMemory(ulEndpointAddress, &g_EndpointBuffer[g_EndpointCount].Endpoint, sizeof(OHCD_ENDPOINT), NULL))
                {
                    dprintf("Error Reading Endpoint in Periodic Schedule(node = %d): Bad Address (0x%0.8x)\n", nodeIndex, ulEndpointAddress);
                    g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_BAD_ENDPOINT;
                    g_EndpointCount++;
                    break;
                }
                g_ExpectedBandwidth[nodeIndex] += OHCD_CalculateBandwidth(
                                                    (ULONG)g_EndpointBuffer[g_EndpointCount].Endpoint.HcEndpointDescriptor.Control.MaximumPacketSize,
                                                    g_EndpointBuffer[g_EndpointCount].Endpoint.HcEndpointDescriptor.Control.Speed,
                                                    FALSE
                                                    );
            }
            ULONG ulLastEndpointAddress = ulEndpointAddress;
            ulEndpointAddress = (ULONG) g_EndpointBuffer[g_EndpointCount].Endpoint.Next;

            //
            //  Check for errant tail pointer
            //

            if(ulLastEndpointAddress == (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[nodeIndex].EndpointTail)
            {
                if(0 != ulEndpointAddress)
                {
                    dprintf("Error: Node %d, the tail pointer points to an endpoint before the end of the list.\n",  nodeIndex);
                    dprintf("  Tail Pointer = 0x%0.8x, next endpoint is 0x%0.8x\n", g_pOhcdExtension->Schedule.InterruptSchedule[nodeIndex].EndpointTail, ulEndpointAddress);
                }
            } else
            {
                    if(0 == ulEndpointAddress)
                    {
                        dprintf("Error: Node %d, the list ends and the tail pointer does not point to it.\n",  nodeIndex);
                        dprintf("  Tail Pointer = 0x%0.8x\n", g_pOhcdExtension->Schedule.InterruptSchedule[nodeIndex].EndpointTail);
                    }
            }

            g_EndpointCount++;
        }
    }

    //
    //  walk the pending close urb list.
    //

    URB Urb;
    ULONG ulUrbAddress;
    ulUrbAddress = (ULONG)g_pOhcdExtension->PendingCloses;
    ulOrder = 0;
    while(ulUrbAddress)
    {
        if(FALSE == ReadMemory(ulUrbAddress, &Urb, sizeof(URB_CLOSE_ENDPOINT), NULL))
        {
            dprintf("Error Reading Urb (0x%0.8x), in PendingClose list\n", ulUrbAddress);
            break;
        }

        //
        //  Now try to read the endpoint that is being closed.
        //
        ulEndpointAddress = (ULONG)Urb.CloseEndpoint.EndpointHandle;
        
        g_EndpointBuffer[g_EndpointCount].OriginalAddress = ulEndpointAddress;
        g_EndpointBuffer[g_EndpointCount].Flags = KDEPF_PENDING_CLOSE_LIST;
        g_EndpointBuffer[g_EndpointCount].ScheduleOrder = ulOrder++;
        if(FALSE == ReadMemory(ulEndpointAddress, &g_EndpointBuffer[g_EndpointCount].Endpoint, sizeof(OHCD_ENDPOINT), NULL))
        {
            dprintf("Error Reading Endpoint (0x%0.8x), in Pending Close list\n", ulEndpointAddress);
            g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_BAD_ENDPOINT;
            // NO NEED TO BREAK, BECAUSE THE urb'S ARE LINKED NOT THE ENDPOINTS
        } else if(g_EndpointBuffer[g_EndpointCount].Endpoint.HcEndpointDescriptor.Control.Format)
        //
        //  if this was really an Isoch Endpoint, read the extra bytes
        //
        {
            if(FALSE == ReadMemory(ulEndpointAddress, &g_EndpointBuffer[g_EndpointCount].IsochEndpoint, sizeof(OHCD_ISOCH_ENDPOINT), NULL))
            {
                dprintf("Error Reading Isoch Endpoint (0x%0.8x), in Pending Close list\n", ulEndpointAddress);
                g_EndpointBuffer[g_EndpointCount].Flags |= KDEPF_BAD_ENDPOINT;
            }
        }
        g_EndpointCount++;
        ulUrbAddress = (ULONG) Urb.CloseEndpoint.HcdNextClose;
    }
}


char *EndpointTypeStrings[4]=
{
    "Control",
    "Isochronous",
    "Bulk",
    "Interrupt"
};

char *EndpointDirectionStrings[4]=
{
    " - Bidirectional",
    " - IN",
    " - OUT",
    " - Bidirectional"
};

void DumpEndpoints()
{
    for(ULONG i=0; i<g_EndpointCount; i++)
    {
        //
        //  Dump Information about the endpoint
        //

        dprintf("----------------------------------------------------\n");
        dprintf("Endpoint (0x%0.8x)\n", g_EndpointBuffer[i].OriginalAddress);
        dprintf("FunctionAddress = 0x%0.2x, EndpointAddress = 0x%0.2x\n", 
            (ULONG)g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.FunctionAddress,
            (ULONG)g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.EndpointAddress);
        dprintf("%s%s\n", 
            EndpointTypeStrings[g_EndpointBuffer[i].Endpoint.EndpointType],
            EndpointDirectionStrings[g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.Direction]
            );
        if(0==g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.Format)
        {
            if(g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.Skip)
            {
                dprintf("Paused(%d,%d), ", g_EndpointBuffer[i].Endpoint.PendingPauseCount,g_EndpointBuffer[i].Endpoint.PauseFrame);
            }
            if(GET_HALTED(&g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor))
            {
                dprintf("Halted, ");
            }
            if(GET_TOGGLECARRY(&g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor))
            {
                dprintf("DATA1\n");
            } else
            {
                dprintf("DATA0\n");
            }
        }
        dprintf("%s, MaxPacket = %d Bytes\n", 
            (g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.Speed) ? "LowSpeed" : "FullSpeed",
            (ULONG)g_EndpointBuffer[i].Endpoint.HcEndpointDescriptor.Control.MaximumPacketSize);
        dprintf("TDs in use = %d\n", (ULONG)g_EndpointBuffer[i].Endpoint.TDInUseCount);
        dprintf("URBs Queued = %d\n", (ULONG)g_EndpointBuffer[i].Endpoint.QueuedUrbCount);
        dprintf("URBs Programmed = %d\n", (ULONG)g_EndpointBuffer[i].Endpoint.ProgrammedUrbCount);
        
        //
        //  Dump Information about where the endpoint was found
        //  and any problems we found with it.
        //
        if(g_EndpointBuffer[i].Flags & KDEPF_PERIODIC_TABLE)
        {
            dprintf("Endpoint %d in periodic node %d\n", g_EndpointBuffer[i].ScheduleOrder, g_EndpointBuffer[i].ScheduleIndex);
            dprintf("Bandwidth = %d bits/ms\n", (ULONG)g_EndpointBuffer[i].Endpoint.Bandwidth);
        } else
        {
            dprintf("Endpoint %d in schedule list\n", g_EndpointBuffer[i].ScheduleOrder);
        }
        

        if(g_EndpointBuffer[i].Flags & KDEPF_WRONG_LIST_TYPE)
        {
            dprintf("ERROR: Endpoint found in the wrong list type: ");
            BOOL fNeedComma = FALSE;
            if(g_EndpointBuffer[i].Flags & KDEPF_CONTROL_LIST)
            {
                dprintf("%sControl", fNeedComma ? ", " : "");
                fNeedComma = TRUE;
            }
            if(g_EndpointBuffer[i].Flags & KDEPF_BULK_LIST)
            {
                dprintf("%sBulk", fNeedComma ? ", " : "");
                fNeedComma = TRUE;
            }
            if(g_EndpointBuffer[i].Flags & KDEPF_PERIODIC_TABLE)
            {
                if(0 == g_EndpointBuffer[i].ScheduleIndex)
                {
                    dprintf("%sIsochronous", fNeedComma ? ", " : "");
                } else
                {
                    dprintf("%sInterrupt", fNeedComma ? ", " : "");
                }
                fNeedComma = TRUE;
            }
            if(g_EndpointBuffer[i].Flags & KDEPF_PENDING_CLOSE_LIST)
            {
                dprintf("%sClose Pending", fNeedComma ? ", " : "");
            }
            dprintf("\n");
        }
        if(g_EndpointBuffer[i].Flags & KDEPF_NOT_IN_PHYSICAL_LIST)
        {
            dprintf("ERROR: Not found in the physical list.\n");
        }
        if(g_EndpointBuffer[i].Flags & KDEPF_NOT_IN_VIRTUAL_LIST)
        {
            dprintf("ERROR: Not found in the virtual list.\n");
        }
        if(g_EndpointBuffer[i].Flags & KDEPF_WRONG_PHYSICAL_LIST)
        {
            dprintf("ERROR: Endpoint was in the wrong physical list.\n");
        }
        if(g_EndpointBuffer[i].Flags & KDEPF_WRONG_PHYSICAL_LIST)
        {
            dprintf("ERROR: The address for this endpoint was invalid.\n");
        }
    }
}


ULONG
OHCD_CalculateBandwidth(
    IN ULONG   MaxPacketSize,
    IN BOOL    LowSpeed,
    IN BOOL    Isoch
    )
{
    ULONG bw;
    if(Isoch)
    {
        bw = (USB_ISO_OVERHEAD_BYTES+MaxPacketSize);
    } else
    {
        bw = (USB_INTERRUPT_OVERHEAD_BYTES+MaxPacketSize);
    }
    bw = (bw*8*7)/6;
    if(LowSpeed)
    {
        bw *= 8;
    }
    return bw;
}


void PerformBandwidthCheck()
{
    int i;
    int ErrorCount;
    //
    //  Calculate Expected Parent Bandwidths
    //

    g_ExpectedParentBandwidth[0] = 0;
    for(i=1; i<64;i++)
    {
        ULONG parent = i >> 1;
        g_ExpectedParentBandwidth[i] = g_ExpectedParentBandwidth[parent] + g_ExpectedBandwidth[parent];
    }

    //
    //  Calculate Expected Children Bandwidth
    //
    for(i = 63; i >= 32; i--)
    {
        g_ExpectedChildrenBandwidth[i] = 0;
    }
    for(i = 31; i >= 1; i--)
    {
        ULONG child1 = i << 1;
        ULONG child2 = child1 + 1;
        ULONG expectedChild1Bandiwidth = g_ExpectedChildrenBandwidth[child1] + g_ExpectedBandwidth[child1];
        ULONG expectedChild2Bandiwidth = g_ExpectedChildrenBandwidth[child2] + g_ExpectedBandwidth[child2];
        if(expectedChild2Bandiwidth > expectedChild1Bandiwidth)
        {
            g_ExpectedChildrenBandwidth[i] = expectedChild2Bandiwidth;
        } else
        {
            g_ExpectedChildrenBandwidth[i] = expectedChild1Bandiwidth;
        }
    }
    g_ExpectedChildrenBandwidth[0] = g_ExpectedChildrenBandwidth[1] + g_ExpectedBandwidth[1];

    dprintf("----------- Bandwidth Check -----------------\n");
    ErrorCount = 0;
    for(i=0; i<64;i++)
    {
        BOOL fNodePrinted = FALSE;
        //
        //  Check nodes bandwidth
        //
        if(g_ExpectedBandwidth[i] != (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[i].Bandwidth)
        {
            ErrorCount++;
            dprintf("Node %d Errors:\n", i);
            fNodePrinted = TRUE;
            dprintf("    Expected Bandwidth = %d bits/ms, Recorded Bandwidth = %d bits/ms\n",
                g_ExpectedBandwidth[i],
                (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[i].Bandwidth
                );
        }
        if(g_ExpectedParentBandwidth[i] != (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[i].BandwidthParents)
        {
            ErrorCount++;
            if(!fNodePrinted)
            {
                dprintf("Node %d Errors:\n", i);
                fNodePrinted = TRUE;
            }
            dprintf("    Expected Parent Bandwidth = %d bits/ms, Recorded Parent Bandwidth = %d bits/ms\n",
                g_ExpectedParentBandwidth[i],
                (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[i].BandwidthParents
                );
        }
        if(g_ExpectedChildrenBandwidth[i] != (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[i].BandwidthChildren)
        {
            ErrorCount++;
            if(!fNodePrinted)
            {
                dprintf("Node %d Errors:\n", i);
                fNodePrinted = TRUE;
            }
            dprintf("    Expected Children Bandwidth = %d bits/ms, Recorded Children Bandwidth = %d bits/ms\n",
                g_ExpectedChildrenBandwidth[i],
                (ULONG)g_pOhcdExtension->Schedule.InterruptSchedule[i].BandwidthChildren
                );
        }
    }
    
    if(0==ErrorCount)
    {
        dprintf("Bandwidth table is consistent.\n");
    } else
    {
        dprintf("%d inconsistencies found in bandwidth table.\n", ErrorCount);
    }
    //
    //  Now Dump Available Bandwidth Summary
    //
    
    dprintf("Total Bandwidth for Host Controller = %d bits/ms\n", g_pOhcdExtension->Schedule.BandwidthTotal);
    dprintf("Bandwidth reserved for periodic transfers = %d bits/ms\n", g_pOhcdExtension->Schedule.BandwidthPeriodic);

    for(i = 1; i <= 32; i *= 2)
    {
        ULONG firstNode = i;
        ULONG lastNode = (i*2)-1;
        ULONG bestBandwidth = 12000;
        ULONG node;
        for(node = firstNode; node <= lastNode; node++)
        {
            ULONG bandwidth = g_pOhcdExtension->Schedule.InterruptSchedule[node].Bandwidth +
                              g_pOhcdExtension->Schedule.InterruptSchedule[node].BandwidthChildren +
                              g_pOhcdExtension->Schedule.InterruptSchedule[node].BandwidthParents;
            if(bandwidth < bestBandwidth) bestBandwidth = bandwidth;
        }
        dprintf("Poll Interval of %d ms: available bandwidth = %d bits/ms\n", i, g_pOhcdExtension->Schedule.BandwidthPeriodic - bestBandwidth);
    }
}