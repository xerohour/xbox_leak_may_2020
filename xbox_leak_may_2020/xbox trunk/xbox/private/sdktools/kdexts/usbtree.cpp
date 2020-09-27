/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    USBD.c

Abstract:

    WinDbg Extension Api

Author:

    Mitchell Dernis (mitchd) 8/11/2000

Environment:

    User Mode.

Revision History:

--*/

#define _NTSYSTEM_
#include "api.h"
#include "x86api.h"

#include <usbd.h>

void DumpCurrentlyEnumerating(CDeviceTree *pTree,IUsbDevice *pDevices);
void DumpPendingEnumerationList(CDeviceTree *pTree, IUsbDevice *pDevices);
void DumpActiveNode(CDeviceTree *pTree, IUsbDevice *pDevices, UCHAR ParentNode, UCHAR Node, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpFunctionNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpInterfaceFunctionNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpHubNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpCompositeFunctionNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpInterfaceNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpPendingEnumerationNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpEnumeratingNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth);
void DumpPendingRemoved(CDeviceTree *pTree, IUsbDevice *pDevices);
void DumpFreeNodes(CDeviceTree *pTree, IUsbDevice *pDevices);

CHAR *EnumStageString(UCHAR Stage);
PNP_CLASS_ID GetPnpClassId(IUsbDevice *pDevice);
BOOL CheckMarkAccessed(UCHAR Node, PULONG Bitfield, BOOL fCheckOnly);

struct DepthStrings
{
    PCHAR DepthString;
};

DepthStrings g_DepthString[4] = 
{
    "1__",
    "2____",
    "3______",
    "4________"
};

//
//  The a 64 bits used to keep track
//  of Nodes that are already in use.
//  Used to check for corruption.
//
ULONG g_ActiveNodes[4];
ULONG g_PendingEnum[4];
ULONG g_FreeNodes[4];

//
// USBD function definitions
//

DECLARE_API( xpptree )

/*++

Routine Description:

   Dumps the Xbox Peripheral Port Tree

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
    
    UCHAR DeviceTreeBuffer[sizeof(CDeviceTree)];
    ULONG ulDeviceTreeAddress;
    CDeviceTree *pDeviceTree;
    IUsbDevice *pDevices;
    
    ulDeviceTreeAddress = GetExpression( "g_DeviceTree" );
    
    if(!ulDeviceTreeAddress)
    {
        dprintf("Could not find Xbox Peripheral Port Tree\n");
        return;
    }
    //
    //  Dump Tree Header
    //
    dprintf("Xbox Peripheral Port Tree @0x%0.8x--------------\n", ulDeviceTreeAddress);
    //
    //  Read in the device tree
    //
    if(FALSE == ReadMemory(ulDeviceTreeAddress, (PVOID)DeviceTreeBuffer, sizeof(CDeviceTree), NULL))
    {
       dprintf("Error reading device tree.\n");
       return;
    }
    pDeviceTree = (CDeviceTree *)DeviceTreeBuffer;
    //
    //  Display node count.
    //
    ULONG NodeCount = pDeviceTree->m_NodeCount;
    dprintf("Static Node Count = %d\n", NodeCount);
    dprintf("Device nodes @0x%0.8x\n", pDeviceTree->m_Devices);
    //
    //  Read in the devices.
    //
    pDevices = (IUsbDevice *) new UCHAR[NodeCount*sizeof(IUsbDevice)];

    if(FALSE == ReadMemory((ULONG)pDeviceTree->m_Devices, (PVOID)pDevices, NodeCount*sizeof(IUsbDevice), NULL))
    {
       dprintf("Error reading devices.\n");
       return;
    }

    //
    //  Clear used nodes
    //
    memset(g_ActiveNodes, 0, sizeof(g_ActiveNodes));
    memset(g_PendingEnum, 0, sizeof(g_PendingEnum));
    memset(g_FreeNodes, 0, sizeof(g_FreeNodes));

    //
    //  Dump information on enumerating nodes.
    //
    if(pDeviceTree->m_InProgress)
    {
        DumpCurrentlyEnumerating(pDeviceTree, pDevices);
        DumpPendingEnumerationList(pDeviceTree, pDevices);
    } else
    {
        dprintf("--Not Currently Enumerating------------------------\n");
    }
    //
    //  Dump active node tree under host controller 1
    //
    if(UDN_TYPE_ROOT_HUB == pDevices[0].m_Type)
    {
        dprintf("--Host Controller 1--------------------------------\n");
        if(CheckMarkAccessed(0, g_PendingEnum, TRUE))
        {
            dprintf("CORRUPT TREE: Root Hub for HostController 1 in pending enum list!\n");    
        }
        if(CheckMarkAccessed(0, g_FreeNodes, TRUE))
        {
            dprintf("CORRUPT TREE: Root Hub for HostController 1 in free node list!\n");    
        }
        CheckMarkAccessed(0, g_ActiveNodes, FALSE);
        dprintf("Host Controller: 0x%0.8x\n", pDevices[0].m_HostController);
        DumpActiveNode(pDeviceTree, pDevices, 0, pDevices[0].m_FirstChild, pDevices[0].m_HostController, 0);
    }
    //
    //  Dump active node tree under host controller 2
    //
    if(UDN_TYPE_ROOT_HUB == pDevices[1].m_Type)
    {
        dprintf("---------------------------------------------------\n");
        dprintf("--Host Controller 2--------------------------------\n");
        if(CheckMarkAccessed(1, g_PendingEnum, TRUE))
        {
            dprintf("CORRUPT TREE: Root Hub for HostController 2 in pending enum list!\n");    
        }
        if(CheckMarkAccessed(1, g_FreeNodes, TRUE))
        {
            dprintf("CORRUPT TREE: Root Hub for HostController 2 in free node list!\n");    
        }
        CheckMarkAccessed(1, g_ActiveNodes, FALSE);
        dprintf("Host Controller: 0x%0.8x", pDevices[1].m_HostController);
        DumpActiveNode(pDeviceTree, pDevices, 1, pDevices[1].m_FirstChild, pDevices[1].m_HostController, 0);
    }
    //
    //  Dump removed nodes.
    //
    DumpPendingRemoved(pDeviceTree, pDevices);

    //
    //  Dump free nodes.
    //
    DumpFreeNodes(pDeviceTree, pDevices);

    //
    //  Clean Up
    //
    delete pDevices;
}

void DumpCurrentlyEnumerating(CDeviceTree *pTree,IUsbDevice *pDevices)
{
    dprintf("--Currently Enumerating ---------------------------\n");    
    dprintf("Node %d.\n", pTree->m_CurrentEnum - pTree->m_Devices);
    if(pTree->m_DeviceRemoved)
    {
        dprintf("Device has been removed.\n");
    }
    dprintf("Stage %s.\n", EnumStageString(pTree->m_EnumStage));
    dprintf("Remaining retires: %d.\n", pTree->m_RetryCount);
}

void DumpPendingEnumerationList(CDeviceTree *pTree, IUsbDevice *pDevices)
{
   if(!pTree->m_FirstPendingEnum) return;
   dprintf("--Pending Enumeration -----------------------------\n");
   UCHAR PendingNode =  pTree->m_FirstPendingEnum - pTree->m_Devices;
   IUsbDevice *pPendingDevice = pDevices + PendingNode;
   while(pPendingDevice)
   {
       dprintf("Node: %d(0x%0.8x)\n", PendingNode, pTree->m_Devices + PendingNode);
       if(CheckMarkAccessed(PendingNode, g_PendingEnum, FALSE))
       {
         dprintf("CORRUPT TREE: Circular Pending Enumeration List\n");
         return;
       }
       if(pDevices->m_NextPending)
       {
            PendingNode = pDevices->m_NextPending - pTree->m_Devices;
            pPendingDevice = pDevices + PendingNode;
       } else
       {
         pPendingDevice = NULL;
       }
   }
}


void DumpActiveNode(CDeviceTree *pTree, IUsbDevice *pDevices, UCHAR ParentNode, UCHAR Node, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    //
    //  Once we hit the end return.
    //  
    if(UDN_INVALID_NODE_INDEX == Node) return;
    //
    //  Print out the node we are dumping
    //
    dprintf("---------------------------------------------------\n");
    dprintf("%sNode %d(0x%0.8x)\n", pDepth, Node, pTree->m_Devices + Node);
    //
    //  Check for illegal depth
    //
    if(3 == Depth)
    {
        dprintf("%sCORRUPT TREE: maximum node depth exceeded.\n", pDepth);
    }
    //
    //  Check for corruption
    //
    if(pTree->m_NodeCount <= Node)
    {
        dprintf("%sCORRUPT TREE: Illegal Node\n", pDepth);
        return;
    }
    //  
    //  Check for circular tree error
    //
    if(CheckMarkAccessed(Node, g_ActiveNodes, FALSE))
    {
        dprintf("%sCORRUPT TREE: Circular Node Reference.\n", pDepth);
        return;
    }
    //
    //  Node is presumably reasonable
    //
    IUsbDevice *pDevice = pDevices+Node;
    //
    //  Check for problems with respect to pending enum list.
    //
    if(CheckMarkAccessed(Node, g_PendingEnum, TRUE))
    {
        if(UDN_TYPE_PENDING_ENUM != pDevice->m_Type)
        {
            dprintf("%sCORRUPT TREE: Active or Enumerating Node is also in  pending enumeration list.\n", pDepth);
        }
    } else
    {
        if(UDN_TYPE_PENDING_ENUM == pDevice->m_Type)
        {
            dprintf("%sCORRUPT TREE: Node is pending enumeration, but not in the pending enumeration list.\n", pDepth);
        }
    }
    //
    //  Check that the parent is correct
    //
    if(pDevice->m_Parent != ParentNode)
    {
        dprintf("%sCORRUPT TREE: Parent (0x%0.8x) incorrect.\n", pDepth, (ULONG)pDevice->m_Parent);
    }
    //
    //  switch on the device type
    //
    switch(pDevice->m_Type)
    {
        case UDN_TYPE_ROOT_HUB:
            dprintf("%sCORRUPT TREE: ROOT HUB as child.\n", pDepth);
            break;
        case UDN_TYPE_HUB:
            DumpHubNode(pTree, pDevice, pHostController, Depth);
            // Recurse children
            DumpActiveNode(pTree, pDevices, Node, pDevice->m_FirstChild, pHostController, Depth + 1);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;            
        case UDN_TYPE_FUNCTION:
            DumpFunctionNode(pTree, pDevice, pHostController, Depth);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;
        case UDN_TYPE_INTERFACE_FUNCTION:
            DumpInterfaceFunctionNode(pTree, pDevice, pHostController, Depth);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;
        case UDN_TYPE_COMPOSITE_FUNCTION:
            DumpCompositeFunctionNode(pTree, pDevice, pHostController, Depth);
            // Recurse children
            DumpActiveNode(pTree, pDevices, Node, pDevice->m_FirstChild, pHostController, Depth + 1);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;
        case UDN_TYPE_INTERFACE:
            DumpInterfaceNode(pTree, pDevice, pHostController, Depth);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;
        case UDN_TYPE_UNUSED:
            dprintf("%sCORRUPT TREE: Unused node, found in active node tree.\n", pDepth);
            break;
        case UDN_TYPE_PENDING_ENUM:
            DumpPendingEnumerationNode(pTree, pDevice, pHostController, Depth);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;
        case UDN_TYPE_ENUMERATING:
            DumpEnumeratingNode(pTree, pDevice, pHostController, Depth);
            // Recurse siblings
            DumpActiveNode(pTree, pDevices, ParentNode, pDevice->m_Sibling, pHostController, Depth);
            break;
        default:
            dprintf("%sCORRUPT TREE: Illegal Node Type = 0x%0.8x.\n", pDepth, pDevice->m_Type);
            break;
    };
    return;
}

void DumpHubNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Hub\n", pDepth);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sFunction Address: %d\n", pDepth, pDevice->m_Address);
    dprintf("%sDefault Endpoint: 0x%0.8x\n", pDepth, pDevice->m_DefaultEndpoint);
    dprintf("%sMaxPacket0: 0x%0.8x\n", pDepth, pDevice->m_MaxPacket0);
    dprintf("%sPnpClassId: 0x%0.8x\n", pDepth, GetPnpClassId(pDevice).AsLong);
    dprintf("%sHub Extension: 0x%0.8x\n", pDepth, pDevice->m_ClassDriverExtension);
}


void DumpFunctionNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Function\n", pDepth);
    dprintf("%sPort Bit Position(ala XGetDevices): %d\n", pDepth, pDevice->m_ExternalPort);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sFunction Address: %d\n", pDepth, pDevice->m_Address);
    dprintf("%sDefault Endpoint: 0x%0.8x\n", pDepth, pDevice->m_DefaultEndpoint);
    dprintf("%sMaxPacket0: 0x%0.8x\n", pDepth, pDevice->m_MaxPacket0);
    dprintf("%sPnpClassId: 0x%0.8x\n", pDepth, GetPnpClassId(pDevice).AsLong);
    dprintf("%sClass Driver Extension: 0x%0.8x\n", pDepth, pDevice->m_ClassDriverExtension);
    dprintf("%sData Toggle Bits (for closed endpoints): 0x%0.8x\n", pDepth, pDevice->m_DataToggleBits);
}

void DumpInterfaceFunctionNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Interface Function\n", pDepth);
    dprintf("%sPort Bit Position(ala XGetDevices): %d\n", pDepth, pDevice->m_ExternalPort);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sFunction Address: %d\n", pDepth, pDevice->m_Address);
    dprintf("%sInterfaceNumber: %d\n", pDepth, pDevice->m_bInterfaceNumber);
    dprintf("%sDefault Endpoint: 0x%0.8x\n", pDepth, pDevice->m_DefaultEndpoint);
    dprintf("%sMaxPacket0: 0x%0.8x\n", pDepth, pDevice->m_MaxPacket0);
    dprintf("%sPnpClassId: 0x%0.8x\n", pDepth, GetPnpClassId(pDevice).AsLong);
    dprintf("%sClass Driver Extension: 0x%0.8x\n", pDepth, pDevice->m_ClassDriverExtension);
    dprintf("%sData Toggle Bits (for closed endpoints): 0x%0.8x\n", pDepth, pDevice->m_DataToggleBits);
}

void DumpCompositeFunctionNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Composite Function\n", pDepth);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sFunction Address: %d\n", pDepth, pDevice->m_Address);
    dprintf("%sDefault Endpoint: 0x%0.8x\n", pDepth, pDevice->m_DefaultEndpoint);
    dprintf("%sMaxPacket0: 0x%0.8x\n", pDepth, pDevice->m_MaxPacket0);
}

void DumpInterfaceNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Interface\n", pDepth);
    dprintf("%sPort Bit Position(ala XGetDevices): %d\n", pDepth, pDevice->m_ExternalPort);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sFunction Address: %d\n", pDepth, pDevice->m_Address);
    dprintf("%sInterfaceNumber: %d\n", pDepth, pDevice->m_bInterfaceNumber);
    dprintf("%sDefault Endpoint: 0x%0.8x\n", pDepth, pDevice->m_DefaultEndpoint);
    dprintf("%sMaxPacket0: 0x%0.8x\n", pDepth, pDevice->m_MaxPacket0);
    dprintf("%sPnpClassId: 0x%0.8x\n", pDepth, GetPnpClassId(pDevice).AsLong);
    dprintf("%sClass Driver Extension: 0x%0.8x\n", pDepth, pDevice->m_ClassDriverExtension);
    dprintf("%sData Toggle Bits (for closed endpoints): 0x%0.8x\n", pDepth, pDevice->m_DataToggleBits);
}

void DumpEnumeratingNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Enumerating - not known yet\n", pDepth);
    dprintf("%sMore information on node was presented at top.\n", pDepth);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sFunction Address: %d\n", pDepth, pDevice->m_Address);
    dprintf("%sDefault Endpoint: 0x%0.8x\n", pDepth, pDevice->m_DefaultEndpoint);
    dprintf("%sMaxPacket0: 0x%0.8x\n", pDepth, pDevice->m_MaxPacket0);
}

void DumpPendingEnumerationNode(CDeviceTree *pTree, IUsbDevice *pDevice, PUSBD_HOST_CONTROLLER pHostController, int Depth)
{
    CHAR *pDepth = g_DepthString[Depth].DepthString;
    dprintf("%sNode Type: Pending Enumeration\n", pDepth);
    //
    //  Only print the host controller if it is wrong, since
    //  you can look to the top of the dump.
    if(pHostController != pDevice->m_HostController)
    {
        dprintf("%sCORRUPT TREE: Host Controller invalid (0x%0.8x)\n", pDepth, pDevice->m_HostController);
    }
    dprintf("%sSpeed: %s\n", pDepth,  (pDevice->m_PortNumber&UDN_LOWSPEED_PORT) ? "Lowspeed" : "Fullspeed");
    dprintf("%sPort in parent hub: %d\n", pDepth,  pDevice->m_PortNumber&~UDN_LOWSPEED_PORT);
    dprintf("%sRemaining Retries: %d\n", pDepth, pDevice->m_RetryCount);
    dprintf("%sNextPendingEnum: 0x%0.8x\n", pDepth, pDevice->m_NextPending);
}
//
//  Nodes pending remove are not of type UDN_TYPE_UNUSED or UDN_TYPE_ROOT_HUB, and
//  never have a parent.  They should not appear in any other
//  list.  It may have children iff it is a composite device, or
//  a hub.
//
void DumpPendingRemoved(CDeviceTree *pTree, IUsbDevice *pDevices)
{
    dprintf("--Pending Removal----------------------------------\n");
    UCHAR Node;
    for(Node = 0; Node < pTree->m_NodeCount; Node++)
    {
        IUsbDevice *pDevice = pDevices + Node;
        if(
            (pDevice->m_Type != UDN_TYPE_ROOT_HUB) && 
            (pDevice->m_Type != UDN_TYPE_UNUSED) &&
            (pDevice->m_Parent == UDN_INVALID_NODE_INDEX)
        )
        {
            //reuse dump active node
            DumpActiveNode(pTree, pDevices, UDN_INVALID_NODE_INDEX, Node, pDevice->m_HostController, 0);
        }
    }
}
void DumpFreeNodes(CDeviceTree *pTree, IUsbDevice *pDevices)
{
    dprintf("--Free Nodes---------------------------------------\n");
    if(UDN_INVALID_NODE_INDEX == pTree->m_FirstFree) return;
    ULONG FreeNodeCount = 0;
    UCHAR FreeNode =  pTree->m_FirstFree;
    IUsbDevice *pFreeDevice = pDevices + FreeNode;
    
    while(pFreeDevice)
    {
       FreeNodeCount++;
       dprintf("Node: %d(0x%0.8x)\n", FreeNode, pTree->m_Devices + FreeNode);
       if(CheckMarkAccessed(FreeNode, g_FreeNodes, FALSE))
       {
         dprintf("CORRUPT TREE: Circular Free Node List\n");
         dprintf("CORRUPT TREE: Free Nodes in circular list: %d\n", FreeNodeCount-1);
         return;
       }
       //
       //   Make sure nodes is not in use.
       //
       if(CheckMarkAccessed(FreeNode, g_ActiveNodes, TRUE))
       {
          dprintf("CORRUPT TREE: Active Node is in free list\n");
       }
       //
       //   Make sure it is marked unused.
       //
       if(pFreeDevice->m_Type != UDN_TYPE_UNUSED)
       {
           dprintf("CORRUPT TREE: Node n free list is not marked unused.\n");
       }
       //iterate to next free node.
       if(UDN_INVALID_NODE_INDEX != pFreeDevice->m_NextFree)
       {
            FreeNode = pFreeDevice->m_NextFree;
            pFreeDevice = pDevices + FreeNode;
       } else
       {
            pFreeDevice = NULL;
       }
    }
    dprintf("Total Free Nodes: %d\n", FreeNodeCount);
}

PNP_CLASS_ID GetPnpClassId(IUsbDevice *pDevice)
{
    USB_CLASS_DRIVER_DESCRIPTION ClassDriverDescription;
    ReadMemory((ULONG)pDevice->m_ClassDriver, (PVOID)&ClassDriverDescription, sizeof(ClassDriverDescription), NULL);
    PNP_CLASS_ID PnPClassID;
    PnPClassID.AsLong = ClassDriverDescription.ClassId.AsLong;
    PnPClassID.USB.bClassSpecificType = pDevice->m_ClassSpecificType;
    return PnPClassID;
}


CHAR *EnumStageString(UCHAR Stage)
{
    switch(Stage)
    {
    case USBD_ENUM_DEVICE_CONNECTED: return "Connected - Waiting for power up";
    case USBD_ENUM_STAGE_0: return "0 - Reseting Port";
    case USBD_ENUM_STAGE_PRE1: return "Pre1 - Waiting 10 ms after reset";
    case USBD_ENUM_STAGE_1: return "1 - Getting Device Descriptor";
    case USBD_ENUM_STAGE_2: return "2 - Setting Address";
    case USBD_ENUM_STAGE_3: return "3 - Closing Default Endpoint";
    case USBD_ENUM_STAGE_PRE4: return "Pre4 - 2 ms after Set Address";
    case USBD_ENUM_STAGE_4: return "4 - Get Config Descriptor, or load function driver";
    case USBD_ENUM_STAGE_5: return "5 - Set Config";
    case USBD_ENUM_STAGE_6: return "6 - Load interface driver";
    case USBD_ENUM_STAGE_ADD_COMPLETE: return "Class Driver Enum Complete";
    case USBD_ENUM_STAGE_ABORT1: return "Abort Enumeration 1";
    case USBD_ENUM_STAGE_DISABLE_COMPLETE: return "Complete Disabling Aborted Device";
    case USBD_ENUM_STAGE_ABORT2: return "Abort Enumeration 2";
    }
    return "Invalid Stage";
}


BOOL CheckMarkAccessed(UCHAR Node, PULONG Bitfield, BOOL fCheckOnly)
{
    BOOL fPreviousAccess = FALSE;
    ULONG ulIndex = 0;
    while(Node >= 32)
    {
        ulIndex++;
        Node -= 32;
    }
    ULONG ulMask = (1 << Node);
    if(Bitfield[ulIndex]&ulMask) fPreviousAccess = TRUE;
    if(!fCheckOnly) Bitfield[ulIndex] |= ulMask;
    return fPreviousAccess;
}

