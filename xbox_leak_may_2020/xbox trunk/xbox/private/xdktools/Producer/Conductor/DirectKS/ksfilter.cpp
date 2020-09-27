// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//
//	ksfilter.cpp
//
// Abstract:
//
//  Class implementation of CKsFilter, CTopologyFilter, 
//  and CSysAudioDeviceFilter.
//
// -------------------------------------------------------------------------------

#include "stdafx.h"
#include "kslibp.h"

// ----------------------------------------------------------------------------------
// CKsFilter::CKsFilter
//      constructor
// ----------------------------------------------------------------------------------
CKsFilter::CKsFilter()
  : CKsIrpTarget(INVALID_HANDLE_VALUE),
    m_pRenderPin(NULL),
    m_pCapturePin(NULL),
    m_eType(eUnknown)
{
    m_szFilterName[0] = 0;
    m_szFriendlyName[0] = 0;
    m_szCLSID[0] = 0;
    m_szService[0] = 0;
    m_szBinary[0] = 0;
    m_UserDef = NULL;
}

CKsFilter::CKsFilter
(
    CKsFilter* pksfCopy
) : CKsIrpTarget(INVALID_HANDLE_VALUE),
    m_pRenderPin(NULL),
    m_pCapturePin(NULL),
    m_eType(eUnknown)
{
    if (pksfCopy)
    {
        strcpy(m_szFilterName, pksfCopy->m_szFilterName);
        strcpy(m_szFriendlyName, pksfCopy->m_szFriendlyName);
        strcpy(m_szCLSID, pksfCopy->m_szCLSID);
        strcpy(m_szService, pksfCopy->m_szService);
        strcpy(m_szBinary, pksfCopy->m_szBinary);

        m_eType = pksfCopy->m_eType;
        m_UserDef = pksfCopy->m_UserDef;

        // Copy nodes.
        CNode<CKsNode>*  pNodeNode;
        CKsNode*         pNode;

        pNodeNode = pksfCopy->m_listNodes.GetHead();
        while (pNodeNode)
        {
            pNode = new CKsNode(this, pNodeNode->pData);
            m_listNodes.AddTail(pNode);
            pNodeNode = pksfCopy->m_listNodes.GetNext(pNodeNode);
        }
        
        // Copy Pins.
        CNode<CKsPin>*   pNodePin;
        CKsPin*          pPin;
        
        pNodePin = pksfCopy->m_listPins.GetHead();
        while (pNodePin)
        {
            pPin = new CKsPin(this, pNodePin->pData);
            m_listPins.AddTail(pPin);
            pNodePin = pksfCopy->m_listPins.GetNext(pNodePin);
        }

        // Copy Connections
        CNode<CKsConnection> *pNodeConnection;
        PCKsConnection        pConnection=NULL;

        pNodeConnection = pksfCopy->m_listConnections.GetHead();
        while (pNodeConnection)
        {
            pConnection = new CKsConnection(pNodeConnection->pData);
            m_listConnections.AddTail(pConnection);
            pNodeConnection = pksfCopy->m_listConnections.GetNext(pNodeConnection);
        }

        ClassifyPins(&m_listPins);
    }
}

// ------------------------------------------------------------------------------
CKsFilter::CKsFilter
(
    LPCTSTR     pszName,
    LPCTSTR     pszFriendlyName,
    LPCTSTR     pszCLSID,
    LPCTSTR     pszService,
    LPCTSTR     pszBinary
) : CKsIrpTarget(INVALID_HANDLE_VALUE),
    m_pRenderPin(NULL),
    m_pCapturePin(NULL),
    m_eType(eUnknown)
{
    m_UserDef = NULL;
    ASSERT(pszName && pszFriendlyName && pszCLSID);

    lstrcpy(m_szFilterName, pszName);
    lstrcpy(m_szFriendlyName, pszFriendlyName);
    lstrcpy(m_szCLSID, pszCLSID);
    lstrcpy(m_szService, pszService);
    lstrcpy(m_szBinary, pszBinary);
}

// ------------------------------------------------------------------------------
CKsFilter::~CKsFilter
( 
    void 
)
{
    DestroyLists();
    
    SafeCloseHandle(m_handle);
}

// ----------------------------------------------------------------------------------
// CKsFilter::Instantiate
// ----------------------------------------------------------------------------------
BOOL
CKsFilter::Instantiate
( 
    void 
)
{
    BOOL fRes;

    // get handle
    m_handle = 
        CreateFile
        (   
            m_szFilterName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            NULL
        );

    fRes = IsValidHandle(m_handle);
    if (!fRes)
    {
        DWORD dwError = GetLastError();
        
        LOG(eWarn2, "CKsFilter::Instantiate:  CreateFile failed for device %s. ErrorCode = dwError", m_szFilterName, dwError);
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsFilter::EnumerateConnections
// ----------------------------------------------------------------------------------
BOOL    
CKsFilter::EnumerateConnections
(
    void
)
{
    DWORD                   fRes = TRUE;
    ULONG                   cConnections;
    PKSMULTIPLE_ITEM        pkmiTopoConnections = NULL;
    ULONG                   nConnection;
    PKSTOPOLOGY_CONNECTION  pKsConnection;

    fRes =
        GetPropertyMulti
        (
            KSPROPSETID_Topology,
            KSPROPERTY_TOPOLOGY_CONNECTIONS,
            &pkmiTopoConnections 
        );

    if (!fRes)
        LOG(eWarn2, "Failed to get property KSPROPSETID_Topology.KSPROPERTY_TOPOLOGY_CONNECTIONS");

    if (fRes)
    {
        if (pkmiTopoConnections)
        {
            cConnections = pkmiTopoConnections->Count;

            pKsConnection = (PKSTOPOLOGY_CONNECTION) (pkmiTopoConnections + 1);

            for(nConnection = 0; nConnection < cConnections; nConnection++, pKsConnection++)
            {
                PCKsConnection pnewConnection = new CKsConnection(pKsConnection);
                if (pnewConnection)
                {
                    m_listConnections.AddTail(pnewConnection);
                }
            }
        }

        ASSERT(m_listConnections.GetCount() == cConnections);
    }

    SafeLocalFree(pkmiTopoConnections);

    return fRes;
} // EnumerateConnections

// ----------------------------------------------------------------------------------
// CKsFilter::EnumeratePins
// ----------------------------------------------------------------------------------
BOOL    
CKsFilter::EnumeratePins
(
    void
)
{
    DWORD   fRes            = TRUE;
    DWORD   fViableFilter   = FALSE;
    HANDLE  hPin            = NULL;
    ULONG   cPins, nPinId;
    DWORD   dwKsRet         = ERROR_NOT_SUPPORTED;

    // get the number of pins supported by SAD
    fRes = 
        GetPinPropertySimple
        (  
            0,
            KSPROPSETID_Pin,
            KSPROPERTY_PIN_CTYPES,
            &cPins,
            sizeof(cPins)
        );

    if (fRes)
    {
        fRes = FALSE;

        //
        // loop through the pins, looking for audio pins
        //
        for(nPinId = 0; nPinId < cPins; nPinId++)
        {
            //
            // create a new CKsPin
            //
            CKsPin* pNewPin = new CKsPin(this, nPinId);

            //
            // Get the data flow property
            //
            fRes = 
                GetPinPropertySimple
                ( 
                    nPinId,
                    KSPROPSETID_Pin,
                    KSPROPERTY_PIN_DATAFLOW,
                    &pNewPin->m_Descriptor.DataFlow,
                    sizeof(KSPIN_DATAFLOW)
                );
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_DATAFLOW");
                delete pNewPin;
                continue;
            }

            //
            // Get the communication property.
            //
            fRes = 
                GetPinPropertySimple
                ( 
                    nPinId,
                    KSPROPSETID_Pin,
                    KSPROPERTY_PIN_COMMUNICATION,
                    &pNewPin->m_Descriptor.Communication,
                    sizeof(KSPIN_COMMUNICATION)
                );
            if (!fRes)
            {
                LOG(eWarn2, "Failed to retrieve pin property KSPROPERTY_PIN_COMMUNICATION");
                delete pNewPin;
                continue;
            }


            if (!pNewPin)
            {
                LOG(eWarn2, "Failed to create pin");
                continue;
            }

            m_listPins.AddTail(pNewPin);
        }
    }
    
    ClassifyPins(&m_listPins);

    fViableFilter =
        !   (
                m_listRenderSinkPins.IsEmpty() && 
                m_listRenderSourcePins.IsEmpty() && 
                m_listCaptureSinkPins.IsEmpty() &&
                m_listCaptureSourcePins.IsEmpty()
            );

    // the end
    return fViableFilter;
}

// ----------------------------------------------------------------------------------
// CKsFilter::ClassifyPins
//  Classifies pins in the given list according to their communication and dataflow
//  properties
// ----------------------------------------------------------------------------------
BOOL
CKsFilter::ClassifyPins(CList <CKsPin> *plistPins)
{
    if (plistPins)
    {
        CNode <CKsPin> *pNodePin = plistPins->GetHead();

        while (pNodePin)
        {
            CKsPin *pPin = pNodePin->pData;
            pNodePin = plistPins->GetNext(pNodePin);
            
            if (pPin->m_Descriptor.DataFlow == KSPIN_DATAFLOW_IN)
            {
                switch (pPin->m_Descriptor.Communication)
                {
                    case KSPIN_COMMUNICATION_SINK:
                        m_listRenderSinkPins.AddTail(pPin);      // IRP sink, Data Sink == render sink
                        break;

                    case KSPIN_COMMUNICATION_SOURCE:
                        m_listCaptureSourcePins.AddTail(pPin);   // IRP source, Data Sink == capture source
                        break;

                    case KSPIN_COMMUNICATION_BOTH:
                        m_listRenderSinkPins.AddTail(pPin);      // IRP sink, Data Sink == render sink
                        m_listCaptureSourcePins.AddTail(pPin);   // IRP source, Data Sink == capture source
                        break;
                }
            }
            else
            {
                switch (pPin->m_Descriptor.Communication)
                {
                    case KSPIN_COMMUNICATION_SINK:
                        m_listCaptureSinkPins.AddTail(pPin);     // IRP sink, Data Source == capture sink
                        break;

                    case KSPIN_COMMUNICATION_SOURCE:
                        m_listRenderSourcePins.AddTail(pPin);    // IRP source, Data Source == render source
                        break;

                    case KSPIN_COMMUNICATION_BOTH:
                        m_listCaptureSinkPins.AddTail(pPin);     // IRP sink, Data Source == capture sink
                        m_listRenderSourcePins.AddTail(pPin);    // IRP source, Data Source == render source
                        break;
                }
            }
        }

        return TRUE;
    }

    return FALSE;
} // ClassifyPins

// ----------------------------------------------------------------------------------
// CKsFilter::DestoyLists
//  Destroys pin and node lists.
// ----------------------------------------------------------------------------------
void    
CKsFilter::DestroyLists
(
    void
)
{
    // Clean nodes.
    CNode<CKsNode> *pNodeNode = m_listNodes.GetHead();
    while (pNodeNode)
    {
        if (pNodeNode->pData)
        {
            delete pNodeNode->pData;
        }
        pNodeNode = m_listNodes.GetNext(pNodeNode);
    }

    // Clean pins.
    CNode<CKsPin>* pNodePin = m_listPins.GetHead();
    while(pNodePin)
    {
        if (pNodePin->pData)
        {
            delete pNodePin->pData;
        }
        pNodePin = m_listPins.GetNext(pNodePin);
    }

    // Clean connections.
    CNode<CKsConnection>* pNodeConnection = m_listConnections.GetHead();
    while (pNodeConnection)
    {
        if (pNodeConnection->pData)
        {
            delete pNodeConnection->pData;
        }
        pNodeConnection = m_listConnections.GetNext(pNodeConnection);
    }
    
    // Empty lists.
    m_listConnections.Empty();
    m_listPins.Empty();
    m_listNodes.Empty();
    m_listRenderSinkPins.Empty();
    m_listRenderSourcePins.Empty();
    m_listCaptureSinkPins.Empty();
    m_listCaptureSourcePins.Empty();
} // Close

// ----------------------------------------------------------------------------------
// CKsFilter::EnumerateNodes
//  populates m_listNodes
// ----------------------------------------------------------------------------------
BOOL    
CKsFilter::EnumerateNodes
(
    void
)
{
    DWORD               fRes = TRUE;
    ULONG               cNodes;
    PKSMULTIPLE_ITEM    pkmiTopoNodes = NULL;
    GUID*               argguidNodes;
    ULONG               nNode;

    fRes =
        GetPropertyMulti
        (
            KSPROPSETID_Topology,
            KSPROPERTY_TOPOLOGY_NODES,
            &pkmiTopoNodes
        );

    if (!fRes)
        LOG(eWarn2, "Failed to get property KSPROPSETID_Topology.KSPROPERTY_TOPOLOGY_NODES");

    if (fRes)
    {
        if (pkmiTopoNodes)
        {
            argguidNodes = (GUID*)(pkmiTopoNodes + 1);
            cNodes = pkmiTopoNodes->Count;

            ULONG       ulBytesReturned;
            KSP_NODE    kspNode;
            DWORD       dwSupport;
            BOOL        fResIoctl;
            ULONG       nProperty;

            kspNode.Property.Set = KSPROPSETID_Audio;
            kspNode.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT | KSPROPERTY_TYPE_TOPOLOGY;
    
            for(nNode = 0; nNode < cNodes; nNode++)
            {
                CKsNode* pnewNode = new CKsNode(this, nNode);
                m_listNodes.AddTail(pnewNode);

                CopyMemory(&pnewNode->m_guidType, &argguidNodes[nNode], sizeof(GUID));

                for(nProperty = KSPROPERTY_AUDIO_LATENCY; nProperty <= KSPROPERTY_AUDIO_3D_INTERFACE; nProperty++)
                {
                    dwSupport = 0;
                    kspNode.Property.Id = nProperty;
                    kspNode.NodeId = nNode;
                    kspNode.Reserved = 0;

                    fResIoctl =
                        SyncIoctl
                        (
                            m_handle,
                            IOCTL_KS_PROPERTY,
                            &kspNode,
                            sizeof(KSP_NODE),
                            &dwSupport,
                            sizeof(DWORD),
                            &ulBytesReturned
                        );

                    if (fResIoctl)
                    {
                        if (dwSupport & KSPROPERTY_TYPE_GET)
                            pnewNode->m_ullSuptdPropsMaskGet |= (1 << (nProperty - 1));

                        if (dwSupport & KSPROPERTY_TYPE_SET)
                            pnewNode->m_ullSuptdPropsMaskSet |= (1 << (nProperty - 1));
                    }
                }
            }
        }

        ASSERT(m_listNodes.GetCount() == cNodes);
    }

    SafeLocalFree(pkmiTopoNodes);

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsFilter::GetPinPropertySimple
//  Get the value of a simple (non-multi) pin property
// ----------------------------------------------------------------------------------
BOOL
CKsFilter::GetPinPropertySimple
(
    IN  ULONG   nPinID,
    IN  REFGUID guidPropertySet,
    IN  ULONG   nProperty,
    OUT PVOID   pvValue,
    OUT ULONG   cbValue
)
{
    ULONG   ulReturned = 0;
    KSP_PIN KsPProp;

    KsPProp.Property.Set = guidPropertySet;
    KsPProp.Property.Id = nProperty;
    KsPProp.Property.Flags = KSPROPERTY_TYPE_GET;
    KsPProp.PinId = nPinID;
    KsPProp.Reserved = 0;

    return 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsPProp,
            sizeof(KSP_PIN),
            pvValue,
            cbValue,
            &ulReturned
        );
}

// ----------------------------------------------------------------------------------
// CKsFilter::GetPinPropertyMulti
//  Get the value of a multi-type pin property
// ----------------------------------------------------------------------------------
BOOL
CKsFilter::GetPinPropertyMulti
(
    IN  ULONG               nPinID,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL    fRes;
    ULONG   ulReturned = 0;
    ULONG   cbMultipleItem = 0;
    KSP_PIN KsPProp;

    KsPProp.Property.Set = guidPropertySet;
    KsPProp.Property.Id = nProperty;
    KsPProp.Property.Flags = KSPROPERTY_TYPE_GET;
    KsPProp.PinId = nPinID;
    KsPProp.Reserved = 0;

    fRes =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &KsPProp.Property,
            sizeof(KSP_PIN),
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
                &KsPProp,
                sizeof(KSP_PIN),
                (PVOID)*ppKsMultipleItem,
                cbMultipleItem,
                &ulReturned
            );
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CTopologyFilter      CTopologyFilter      CTopologyFilter      CTopologyFilter      
// ----------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
// CTopologyFilter::CTopologyFilter
// ----------------------------------------------------------------------------------
CTopologyFilter::CTopologyFilter
(
    LPCTSTR  pszName,
    LPCTSTR  pszFriendlyName,
    LPCTSTR  pszCLSID,
    LPCTSTR  pszService,
    LPCTSTR  pszBinary
) : CKsFilter(pszName, pszFriendlyName, pszCLSID, pszService, pszBinary)
{
    m_eType = eTopology;
}

// ----------------------------------------------------------------------------------
// CSysAudioDeviceFilter   CSysAudioDeviceFilter   CSysAudioDeviceFilter   CSysAudioDeviceFilter
// ----------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------
// CSysAudioDeviceFilter::CSysAudioDeviceFilter
// ----------------------------------------------------------------------------------
CSysAudioDeviceFilter::CSysAudioDeviceFilter
(
    LPCTSTR  pszName,
    LPCTSTR  pszFriendlyName,
    LPCTSTR  pszCLSID,
    LPCTSTR  pszService, 
    LPCTSTR  pszBinary
) : CKsFilter(pszName, pszFriendlyName, pszCLSID, pszService, pszBinary)
{
    m_eType = eSysAudioDevice;
}

CSysAudioDeviceFilter::CSysAudioDeviceFilter
(
    LPCTSTR  pszName,
    LPCTSTR  pszFriendlyName
)
{
    m_eType = eSysAudioDevice;

    lstrcpy(m_szFilterName, pszName);
    lstrcpy(m_szFriendlyName, pszFriendlyName);
}

