// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//	ksirptgt.cpp
//
// Abstract:
//  Class implementation of CKsIrpTarget.
//
// -------------------------------------------------------------------------------

#include "stdafx.h"
#include "kslibp.h"

// ----------------------------------------------------------------------------------
// CKsIrpTarget::GetPropertySimple
//  Get the value of a simple (non-multi) property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::GetPropertySimple
(
    IN  REFGUID guidPropertySet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ULONG       ulReturned = 0;
    KSPROPERTY  ksProperty;

    ksProperty.Set    = guidPropertySet;    //KSPROPSETID_Topology;
    ksProperty.Id     = nProperty;          //KSPROPERTY_TOPOLOGY_NODES;
    ksProperty.Flags  = KSPROPERTY_TYPE_GET;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &ksProperty,
            sizeof(KSPROPERTY),
            pvValue,
            cbValue,
            &ulReturned
        );
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::GetPropertyMulti
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::GetPropertyMulti
(
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL        fRes;
    ULONG       cbMultipleItem = 0;
    ULONG       ulReturned = 0;
    KSPROPERTY  ksProperty;

    ksProperty.Set    = guidPropertySet;    //KSPROPSETID_Topology;
    ksProperty.Id     = nProperty;          //KSPROPERTY_TOPOLOGY_NODES;
    ksProperty.Flags  = KSPROPERTY_TYPE_GET;

    fRes =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &ksProperty,
            sizeof(KSPROPERTY),
            NULL,
            0,
            &cbMultipleItem
        );

    if (fRes && cbMultipleItem)
    {
        *ppKsMultipleItem = (PKSMULTIPLE_ITEM)LocalAlloc(LPTR, cbMultipleItem);
        
        fRes =
            SyncIoctl
            (
                m_handle,
                IOCTL_KS_PROPERTY,
                &ksProperty,
                sizeof(KSPROPERTY),
                (PVOID)*ppKsMultipleItem,
                cbMultipleItem,
                &ulReturned
            );
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::GetPropertyMulti
// Use this when input is not KSPROPERTY.
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::GetPropertyMulti
(
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    IN  PVOID               pvData,
    IN  ULONG               cbData,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL        fRes;
    ULONG       cbMultipleItem = 0;
    ULONG       ulReturned = 0;
    PKSPROPERTY pKsProperty;

    if (!pvData)
    {
        return FALSE;
    }

    pKsProperty         = (PKSPROPERTY) pvData;
    pKsProperty->Set    = guidPropertySet;
    pKsProperty->Id     = nProperty;
    pKsProperty->Flags  = KSPROPERTY_TYPE_GET;

    fRes =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            (PKSPROPERTY) pvData,
            cbData,
            NULL,
            0,
            &cbMultipleItem
        );

    if (fRes && cbMultipleItem)
    {
        *ppKsMultipleItem = (PKSMULTIPLE_ITEM)LocalAlloc(LPTR, cbMultipleItem);
        
        fRes =
            SyncIoctl
            (
                m_handle,
                IOCTL_KS_PROPERTY,
                (PKSPROPERTY) pvData,
                cbData,
                (PVOID)*ppKsMultipleItem,
                cbMultipleItem,
                &ulReturned
            );
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::GetNodePropertyChannel
//  Get the value of a simple (non-multi) KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::GetNodePropertyChannel
(
    IN  ULONG   nNodeID,
    IN  ULONG   nChannel,
    IN  REFGUID guidPropertySet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ULONG                           ulReturned = 0;
    KSNODEPROPERTY_AUDIO_CHANNEL    KsNProp;

    KsNProp.NodeProperty.Property.Set = guidPropertySet;
    KsNProp.NodeProperty.Property.Id = nProperty;
    KsNProp.NodeProperty.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    KsNProp.NodeProperty.NodeId = nNodeID;
    KsNProp.NodeProperty.Reserved = 0;
    KsNProp.Channel = nChannel;
    KsNProp.Reserved = 0;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsNProp,
            sizeof(KsNProp),
            pvValue,
            cbValue,
            &ulReturned
        );
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::GetNodePropertySimple
//  Get the value of a single type KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::GetNodePropertySimple
(
    IN  ULONG               nNodeID,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nPropertySet,
    OUT PVOID               pvDest,
    OUT ULONG               cbDest
)
{
    KSNODEPROPERTY          ksNodeProperty;
    ULONG                   ulReturned;

    ksNodeProperty.Property.Set = guidPropertySet;
    ksNodeProperty.Property.Id = nPropertySet;
    ksNodeProperty.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;

    ksNodeProperty.NodeId = nNodeID;
    ksNodeProperty.Reserved = 0;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &ksNodeProperty,
            sizeof(ksNodeProperty),
            pvDest,
            cbDest,
            &ulReturned
        );
} // GetNodePropertySimple

// ----------------------------------------------------------------------------------
// CKsIrpTarget::GetNodePropertyMulti
//  Get the value of a multi-type KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::GetNodePropertyMulti
(
    IN  ULONG               nNodeID,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL        fRes;
    ULONG       ulReturned = 0;
    ULONG       cbMultipleItem = 0;
    KSP_NODE    KsNProp;

    KsNProp.Property.Set = guidPropertySet;
    KsNProp.Property.Id = nProperty;
    KsNProp.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    KsNProp.NodeId = nNodeID;
    KsNProp.Reserved = 0;

    fRes =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsNProp.Property,
            sizeof(KSP_NODE),
            NULL,
            0,
            &cbMultipleItem
        );

    if (fRes)
    {
        *ppKsMultipleItem = (PKSMULTIPLE_ITEM)LocalAlloc(LPTR, cbMultipleItem);
        
        fRes =
            SyncIoctl
            (
                m_handle,
                IOCTL_KS_PROPERTY,
                &KsNProp,
                sizeof(KSP_NODE),
                (PVOID)*ppKsMultipleItem,
                cbMultipleItem,
                &ulReturned
            );
    }

    return fRes;
}


// ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------
// CKsIrpTarget::SetPropertySimple
//  Get the value of a simple (non-multi) property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::SetPropertySimple
(
    IN  REFGUID guidPropertySet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ULONG       ulReturned = 0;
    KSPROPERTY  ksProperty;

    ksProperty.Set    = guidPropertySet; 
    ksProperty.Id     = nProperty;       
    ksProperty.Flags  = KSPROPERTY_TYPE_SET;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &ksProperty,
            sizeof(KSPROPERTY),
            pvValue,
            cbValue,
            &ulReturned
        );
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::SetPropertyMulti
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::SetPropertyMulti
(
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL        fRes;
    ULONG       cbMultipleItem = 0;
    ULONG       ulReturned = 0;
    KSPROPERTY  ksProperty;

    ksProperty.Set    = guidPropertySet; 
    ksProperty.Id     = nProperty;       
    ksProperty.Flags  = KSPROPERTY_TYPE_SET;

    fRes =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &ksProperty,
            sizeof(KSPROPERTY),
            NULL,
            0,
            &cbMultipleItem
        );

    if (fRes)
    {
        *ppKsMultipleItem = (PKSMULTIPLE_ITEM)LocalAlloc(LPTR, cbMultipleItem);
        
        fRes =
            SyncIoctl
            (
                m_handle,
                IOCTL_KS_PROPERTY,
                &ksProperty,
                sizeof(KSPROPERTY),
                (PVOID)*ppKsMultipleItem,
                cbMultipleItem,
                &ulReturned
            );
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::SetNodePropertyChannel
//  Set the value of a simple (non-multi) KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::SetNodePropertyChannel
(
    IN  ULONG   nNodeID,
    IN  ULONG   nChannel,
    IN  REFGUID guidPropertySet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ULONG                           ulReturned = 0;
    KSNODEPROPERTY_AUDIO_CHANNEL    KsNProp;

    KsNProp.NodeProperty.Property.Set = guidPropertySet;
    KsNProp.NodeProperty.Property.Id = nProperty;
    KsNProp.NodeProperty.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    KsNProp.NodeProperty.NodeId = nNodeID;
    KsNProp.NodeProperty.Reserved = 0;
    KsNProp.Channel = nChannel;
    KsNProp.Reserved = 0;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsNProp,
            sizeof(KsNProp),
            pvValue,
            cbValue,
            &ulReturned
        );
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::SetNodePropertyMulti
//  Set the value of a multi-type KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::SetNodePropertyMulti
(
    IN  ULONG               nNodeID,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL        fRes;
    ULONG       ulReturned = 0;
    ULONG       cbMultipleItem = 0;
    KSP_NODE    KsNProp;

    KsNProp.Property.Set = guidPropertySet;
    KsNProp.Property.Id = nProperty;
    KsNProp.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    KsNProp.NodeId = nNodeID;
    KsNProp.Reserved = 0;

    fRes =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsNProp.Property,
            sizeof(KSP_NODE),
            NULL,
            0,
            &cbMultipleItem
        );

    if (fRes)
    {
        *ppKsMultipleItem = (PKSMULTIPLE_ITEM)LocalAlloc(LPTR, cbMultipleItem);
        
        fRes =
            SyncIoctl
            (
                m_handle,
                IOCTL_KS_PROPERTY,
                &KsNProp,
                sizeof(KSP_NODE),
                (PVOID)*ppKsMultipleItem,
                cbMultipleItem,
                &ulReturned
            );
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsIrpTarget::SetNodePropertySimple
//  Set the value of a single KSPROPERTY_Node
// ----------------------------------------------------------------------------------
BOOL
CKsIrpTarget::SetNodePropertySimple
(
    IN  ULONG               nNodeID,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PVOID               pvDest,
    OUT ULONG               cbDest
)
{
    ULONG                           ulReturned = 0;
    KSNODEPROPERTY                  KsNProp;

    KsNProp.Property.Set = guidPropertySet;
    KsNProp.Property.Id = nProperty;
    KsNProp.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
    KsNProp.NodeId = nNodeID;
    KsNProp.Reserved = 0;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsNProp,
            sizeof(KsNProp),
            pvDest,
            cbDest,
            &ulReturned
        );
} // SetNodePropertySimple

