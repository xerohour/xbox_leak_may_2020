//==============================================================================
// Copyright (c) 2000 Microsoft Corporation.  All rights reserved.
//
// Module Name:
//	ksnode.cpp
//
// Abstract:
//  Class implementation of CKsNode.
//
//==============================================================================

#include "stdafx.h"
#include "kslibp.h"

// ------------------------------------------------------------------------
// CKsNode::CKsNode
//      copy constructor
// ------------------------------------------------------------------------
CKsNode::CKsNode
(
    CKsIrpTarget*   pIrpTarget,
    CKsNode*        pksnCopy
) : m_pIrpTarget(pIrpTarget), 
    m_ullSuptdPropsMaskGet(0x0000000000000000),
    m_ullSuptdPropsMaskSet(0x0000000000000000)
{
    if (pksnCopy)
    {
        m_nId = pksnCopy->m_nId;
    }
}

// ------------------------------------------------------------------------
// CKsNode::CKsNode
//      constructor
// ------------------------------------------------------------------------
CKsNode::CKsNode
(
    CKsIrpTarget*   pIrpTarget,
    ULONG           nID
) : m_pIrpTarget(pIrpTarget), 
    m_nId(nID), 
    m_ullSuptdPropsMaskGet(0x0000000000000000),
    m_ullSuptdPropsMaskSet(0x0000000000000000)
{
    return;    
}

// ----------------------------------------------------------------------------------
// CKsNode::GetNodePropertyChannel
//  Get simple node property
// ----------------------------------------------------------------------------------
BOOL
CKsNode::GetNodePropertyChannel
(
    IN  ULONG   nChannel,
    IN  REFGUID guidPropertyGet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ASSERT(m_pIrpTarget);

    return 
        m_pIrpTarget->GetNodePropertyChannel
        (
            m_nId,
            nChannel,
            guidPropertyGet,
            nProperty,
            pvValue,
            cbValue
        );
}

// ----------------------------------------------------------------------------------
// CKsNode::GetNodePropertyMulti
//  Get the value of a multi-type KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsNode::GetNodePropertyMulti
(
    IN  REFGUID             guidPropertyGet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    ASSERT(m_pIrpTarget);

    return 
        m_pIrpTarget->GetNodePropertyMulti
        (
            m_nId,
            guidPropertyGet,
            nProperty,
            ppKsMultipleItem
        );
}

// ----------------------------------------------------------------------------------
// CKsNode::SetNodePropertyChannel
//  set simple node property
// ----------------------------------------------------------------------------------
BOOL
CKsNode::SetNodePropertyChannel
(
    IN  ULONG   nChannel,
    IN  REFGUID guidPropertySet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ASSERT(m_pIrpTarget);

    return 
        m_pIrpTarget->SetNodePropertyChannel
        (
            m_nId,
            nChannel,
            guidPropertySet,
            nProperty,
            pvValue,
            cbValue
        );
}

// ----------------------------------------------------------------------------------
// CKsNode::SetNodePropertyMulti
//  Get the value of a multi-type KSPROPERTY_Node property
// ----------------------------------------------------------------------------------
BOOL
CKsNode::SetNodePropertyMulti
(
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    ASSERT(m_pIrpTarget);

    return 
        m_pIrpTarget->SetNodePropertyMulti
        (
            m_nId,
            guidPropertySet,
            nProperty,
            ppKsMultipleItem
        );
}

