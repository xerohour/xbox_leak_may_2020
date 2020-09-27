/*++

Copyright (c) Microsoft Corporation.  All rights reserved. 

Module Name:

    usbinit.cpp

Abstract:

    Implementation of IUsbInit.  This class is passed to class drivers to collect
    resource information during initialization.

Environment:

    XBOX kernel mode only

Notes:

Revision History:

    01-10-01 created by Mitchell Dernis (mitchd)

--*/


//
//  Pull in OS headers
//
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xapidrv.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DBSU'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("USBD");

//
//  Pull in usbd headers
//
#include <usbd.h>

#pragma code_seg(".XPPCINIT")
#pragma data_seg(".XPPDINIT")
#pragma const_seg(".XPPRINIT")


ULONG IUsbInit::GetMaxDeviceTypeCount(PXPP_DEVICE_TYPE XppDeviceType)
{
    ULONG ulIndex;
    
    //
    //  Return XGetPortCount(), if m_pDeviceTypes is NULL.
    //
    if(NULL == m_pDeviceTypes)
    {
        return 0;
    }
    //
    //  Otherwise, find the device type.
    //
    for(ulIndex = 0; ulIndex < m_NumDeviceTypes; ulIndex++)
    {
        if( m_pDeviceTypes[ulIndex].DeviceType == XppDeviceType)
        {
            ULONG ulResult = m_pDeviceTypes[ulIndex].dwPreallocCount;
            #if DBG
            if(0==ulResult)
            {
                XDebugPrint(XDBG_WARNING, "XInitDevices", "A device type was explictly listed requesting 0 open instances.\n\
This forces the linkage of the supporting driver library.  Ommitting the device\ntype from the list is always better practice.");
            }
            #endif
            return ulResult;
        }
    }
    //
    //  If the device type was not registered, then we support 0.
    //  (Don't spew here, this could happen because one device
    //  supported by a given driver is in the list, but others are not.)
    return 0;
}

BOOL IUsbInit::UseDefaultCount()
{
    return m_pDeviceTypes ? FALSE : TRUE;
}

//
//  It is inline, because we just want something more robust
//  than a MAX macro which has a number of problems.
//  It is static because we don't want code generated for it
//
inline static void SetToMax(UCHAR& dest, UCHAR src) {if(src > dest) dest = src;}

void IUsbInit::RegisterResources(PUSB_RESOURCE_REQUIREMENTS pResourceRequirements)
/*++
  Routine Description:  
    Called by each driver for each type of connector it supports.  It is up to each
    driver to figure the maximum number of devices it can support in each slot type.
    If a driver supports composite devices (such as audio) it should add the resources
    of the composites together and register once.

    If a driver supports multiple device types (not composites) it should register them separately.
    If the devices have the same connector type, the driver may choose to register only the worst
    case device.
--*/
{
    PUSB_RESOURCE_REQUIREMENTS pConnectorResourceList;
    ULONG ulMaxDevices = pResourceRequirements->MaxDevices;
    ULONG ulMaxPorts = XGetPortCount();
    //
    //  Choose which list to reserve resource from
    //
    switch(pResourceRequirements->ConnectorType)
    {
        case  USB_CONNECTOR_TYPE_DIRECT:
            // Direct 
            pConnectorResourceList = m_Direct;
            break;
        case  USB_CONNECTOR_TYPE_HIGH_POWER:
            pConnectorResourceList = m_TopSlots;
            break;
        case  USB_CONNECTOR_TYPE_LOW_POWER:
            //
            //  Low power devices have their resource allocated first against
            //  the bottom slots
            //
            pConnectorResourceList = m_BottomSlots;
            // Low Power can go in top slots two, see
            // below for way we use the 0th index.
            SetToMax(
                m_TopSlots[0].MaxCompositeInterfaces,
                pResourceRequirements->MaxCompositeInterfaces
                );
            //
            //  If the device count exceeds the bottom slots, apply the rest
            //  towards the top-slots.  The easiest way is to change the
            //  connector type to high-power and call ourselves.
            //
            if(pResourceRequirements->MaxDevices > ulMaxPorts)
            {
                pResourceRequirements->MaxDevices -= (UCHAR)ulMaxPorts;
                pResourceRequirements->ConnectorType = USB_CONNECTOR_TYPE_HIGH_POWER;
                RegisterResources(pResourceRequirements);
                pResourceRequirements->MaxDevices += (UCHAR)ulMaxPorts;
                pResourceRequirements->ConnectorType = USB_CONNECTOR_TYPE_LOW_POWER;
                ulMaxDevices -= ulMaxPorts;
            }
            break;
    }
    //
    //  Composite Interface require nodes whether or not the device is opened.
    //  So we just use the 0th slot of the given type to track it, we are
    //  going to multiply by the number of ports anyway during Process.
    //
    SetToMax(
        pConnectorResourceList[0].MaxCompositeInterfaces,
        pResourceRequirements->MaxCompositeInterfaces
        );

    ASSERT(ulMaxDevices <= ulMaxPorts);

    // Some variables for walk the resource table.
    ULONG ulTempSlotIndex;
    ULONG ulDeviceCount;
    ULONG ulSlotIndex;
    //
    //  Record Control Endpoint Resources
    //
    ulSlotIndex = 0;
    ulDeviceCount = ulMaxDevices;
    while(ulDeviceCount && (ulSlotIndex < ulMaxPorts))
    {
        if(pConnectorResourceList[ulSlotIndex].MaxControlEndpoints > pResourceRequirements->MaxControlEndpoints)
        {
            //Increment the slot index and continue trying to place the resources somewhere.
            ulSlotIndex++;
            continue;
        } else
        {
            //Shift the resources to the right
            for(ulTempSlotIndex = ulMaxPorts; ulTempSlotIndex > ulSlotIndex; ulTempSlotIndex--)
            {
                pConnectorResourceList[ulTempSlotIndex].MaxControlEndpoints = pConnectorResourceList[ulTempSlotIndex-1].MaxControlEndpoints;
            }
            // Record the new resources
            pConnectorResourceList[ulSlotIndex].MaxControlEndpoints = pResourceRequirements->MaxControlEndpoints;
            ulSlotIndex++;
            ulDeviceCount--;
        }
    }
    //
    //  Record Bulk Endpoint Resources
    //
    ulSlotIndex = 0;
    ulDeviceCount = ulMaxDevices;
    while(ulDeviceCount && (ulSlotIndex < ulMaxPorts))
    {
        if(pConnectorResourceList[ulSlotIndex].MaxBulkEndpoints > pResourceRequirements->MaxBulkEndpoints)
        {
            //Increment the slot index and continue trying to place the resources somewhere.
            ulSlotIndex++;
            continue;
        } else
        {
            //Shift the resources to the right
            for(ulTempSlotIndex = ulMaxPorts; ulTempSlotIndex > ulSlotIndex; ulTempSlotIndex--)
            {
                pConnectorResourceList[ulTempSlotIndex].MaxBulkEndpoints = pConnectorResourceList[ulTempSlotIndex-1].MaxBulkEndpoints;
            }
            // Record the new resources
            pConnectorResourceList[ulSlotIndex].MaxBulkEndpoints = pResourceRequirements->MaxBulkEndpoints;
            ulSlotIndex++;
            ulDeviceCount--;
        }
    }
    //
    //  Record Interrupt Endpoint Resources
    //
    ulSlotIndex = 0;
    ulDeviceCount = ulMaxDevices;
    while(ulDeviceCount && (ulSlotIndex < ulMaxPorts))
    {
        if(pConnectorResourceList[ulSlotIndex].MaxInterruptEndpoints > pResourceRequirements->MaxInterruptEndpoints)
        {
            //Increment the slot index and continue trying to place the resources somewhere.
            ulSlotIndex++;
            continue;
        } else
        {
            //Shift the resources to the right
            for(ulTempSlotIndex = ulMaxPorts; ulTempSlotIndex > ulSlotIndex; ulTempSlotIndex--)
            {
                pConnectorResourceList[ulTempSlotIndex].MaxInterruptEndpoints = pConnectorResourceList[ulTempSlotIndex-1].MaxInterruptEndpoints;
            }
            // Record the new resources
            pConnectorResourceList[ulSlotIndex].MaxInterruptEndpoints = pResourceRequirements->MaxInterruptEndpoints;
            ulSlotIndex++;
            ulDeviceCount--;
        }
    }
    //
    //  Record Isochronous Endpoint Resources
    //
    ulSlotIndex = 0;
    ulDeviceCount = ulMaxDevices;
    while(ulDeviceCount && (ulSlotIndex < ulMaxPorts))
    {
        if(pConnectorResourceList[ulSlotIndex].MaxIsochEndpoints > pResourceRequirements->MaxIsochEndpoints)
        {
            //Increment the slot index and continue trying to place the resources somewhere.
            ulSlotIndex++;
            continue;
        } else
        {
            //Shift the resources to the right
            for(ulTempSlotIndex = ulMaxPorts; ulTempSlotIndex > ulSlotIndex; ulTempSlotIndex--)
            {
                pConnectorResourceList[ulTempSlotIndex].MaxIsochEndpoints = pConnectorResourceList[ulTempSlotIndex-1].MaxIsochEndpoints;
            }
            // Record the new resources
            pConnectorResourceList[ulSlotIndex].MaxIsochEndpoints = pResourceRequirements->MaxIsochEndpoints;
            ulSlotIndex++;
            ulDeviceCount--;
        }
    }

    //
    //  These are max'd across all connector types and all slots.  Unlike the other fields
    //  that are later summed across the connector types and slots.
    SetToMax(m_HcdResources.ControlTDQuota, pResourceRequirements->MaxControlEndpoints);
    SetToMax(m_HcdResources.BulkTDQuota, pResourceRequirements->MaxBulkTDperTransfer);
    SetToMax(m_HcdResources.IsochMaxBuffers, pResourceRequirements->MaxIsochMaxBuffers);
    //We keep track MaxComposite Interfaces, both per slot, and the global
    
    return;
}

VOID
IUsbInit::Process()
/*
    This routine takes all the table information, and boils it down to the information
    that USBD needs.
*/
{
    ULONG ulMaxPorts = XGetPortCount();
    ULONG ulControlEndpoints = 0;
    ULONG ulBulkEndpoints = 0;
    ULONG ulInterruptEndpoints = 0;
    
    // The members should all be 0 from the c'tor:
    
    
    //
    //  Figure the base number of nodes based on how
    //  many devices may be pending enumeration at once.
    //  This number to USBD_BASE_NODES_PER_PORT per port.
    //  This is four, one for the hub, one for "direct connect"
    //  and one for each slot.  It assumes there are no composites
    //  composites are figured in below.
    //
    m_NodeCount = USBD_BASE_NODES_PER_PORT*ulMaxPorts;

    //  Add two nodes: one for a host controller and one for
    //  an internal hub.  Alternatively, on SILVER we support
    //  two host controllers but no internal hub.
    //
    //  Also add a slop node.  This is for when a device stops
    //  responding and needs to be reenumerated on a full bus.
    //  Due to timing constraints only one slop node should ever
    //  be needed.
    //
    m_NodeCount += 3;
    
    //
    //  Now add nodes for the number of composites that can be found
    //  while enumerating a devices that is potentially not support.
    //
    m_NodeCount += 
        (m_Direct[0].MaxCompositeInterfaces +
         m_BottomSlots[0].MaxCompositeInterfaces +
         m_TopSlots[0].MaxCompositeInterfaces) * ulMaxPorts;

    //
    //  We also need nodes for composite interfaces of unsupported
    //  devices while they are enumerated, but before they are rejected.
    //
    SetToMax(m_MaxCompositeInterfaces,m_Direct[0].MaxCompositeInterfaces);
    SetToMax(m_MaxCompositeInterfaces,m_BottomSlots[0].MaxCompositeInterfaces);
    SetToMax(m_MaxCompositeInterfaces,m_TopSlots[0].MaxCompositeInterfaces);
    m_NodeCount += m_MaxCompositeInterfaces;

    //
    //  Add up the info that gets add across connector types and slots
    //
    for(ULONG ulSlotIndex = 0;  ulSlotIndex < ulMaxPorts; ulSlotIndex++)
    {
        //
        //  Sum the Interrupt endpoints by type  
        //
        ulInterruptEndpoints += m_Direct[ulSlotIndex].MaxInterruptEndpoints;
        ulInterruptEndpoints += m_BottomSlots[ulSlotIndex].MaxInterruptEndpoints;
        ulInterruptEndpoints += m_TopSlots[ulSlotIndex].MaxInterruptEndpoints;

        //
        //  Sum the Control endpoints by type  
        //
        ulControlEndpoints += m_Direct[ulSlotIndex].MaxControlEndpoints;
        ulControlEndpoints += m_BottomSlots[ulSlotIndex].MaxControlEndpoints;
        ulControlEndpoints += m_TopSlots[ulSlotIndex].MaxControlEndpoints;

        //
        //  Sum the Bulk endpoints by type  
        //
        ulBulkEndpoints += m_Direct[ulSlotIndex].MaxBulkEndpoints;
        ulBulkEndpoints += m_BottomSlots[ulSlotIndex].MaxBulkEndpoints;
        ulBulkEndpoints += m_TopSlots[ulSlotIndex].MaxBulkEndpoints;

        //
        //  Sum the Isoch endpoints by type
        //
        m_HcdResources.IsochEndpointCount += m_Direct[ulSlotIndex].MaxIsochEndpoints;
        m_HcdResources.IsochEndpointCount += m_BottomSlots[ulSlotIndex].MaxIsochEndpoints;
        m_HcdResources.IsochEndpointCount += m_TopSlots[ulSlotIndex].MaxIsochEndpoints;
    }

    //
    //  The hub driver is a special case.  It would be a different slot type,
    //  but instead it does not register, we just hard code it's resources.
    //  (The control qouta is less than enumerations, so don't worry about it.)
    //
    ulInterruptEndpoints += ulMaxPorts+1; //1 Interrupt EP per hub, one hub per port, plus an internal hub
    ulControlEndpoints += ulMaxPorts+1; //1 Control EP per hub, one hub per port, plus an internal hub

    //
    //  An extra control endpoint is needed for enumeration
    //  USBD also has a control TD quota
    ulControlEndpoints += 1;
    SetToMax(m_HcdResources.ControlTDQuota, USBD_CONTROL_TD_QUOTA);

    //
    //  add the control and bulk endpoints,
    //  we will add in the interrupt below.
    //
    m_HcdResources.EndpointCount = ulControlEndpoints + ulBulkEndpoints;

    //
    //  TD Count is the control quota plus the bulk quota plus
    //  one for each control and bulk endpoint, and USBD_INTERRUPT_TD_QUOTA
    //  times the number of interrupt endpoints.
    //
    m_HcdResources.TDCount = m_HcdResources.ControlTDQuota + m_HcdResources.BulkTDQuota +
                             m_HcdResources.EndpointCount +
                             (HCD_INTERRUPT_TD_QUOTA * ulInterruptEndpoints);

    //
    //  Now add in the interrupt endpoint.  Note that the isoch endpoint
    //  count is not included, ever.  Isoch endpoints are allocated differently.
    //
    m_HcdResources.EndpointCount += ulInterruptEndpoints;
}