/*  
    Base implementation of MIDI Transform Filter object for DMUS_KERNEL_EVENT struct allocation.

    05/08/98    Martin Puryear      Created this file

*/

#include "stdafx.h"
#include "AllocatorMXF.h"

//  create the pool
CAllocatorMXF::CAllocatorMXF(void)
: CMXF(NULL)
{
    PDMUS_KERNEL_EVENT  pDMKEvt;
    PDMUS_KERNEL_EVENT  pEvtList;

    m_numFreeMessages = 0;

//    pDMKEvt = (PDMUS_KERNEL_EVENT)ExAllocatePoolWithTag(
//                NonPagedPool,
//                sizeof(DMUS_KERNEL_EVENT),
//                ' FXM');    //  'MXF '
    pDMKEvt = new DMUS_KERNEL_EVENT;

    if (pDMKEvt)
    {
        pDMKEvt->cbEvent = 0;
        pDMKEvt->usFlags = 0;
        pDMKEvt->ulChannelGroup = 0;
        pDMKEvt->ullPresTime100Ns = 0;
        pDMKEvt->pNextEvt = NULL;
        pDMKEvt->uData.pbData = NULL;

        m_numFreeMessages++;
        
        m_pFreeList = pDMKEvt;
        pEvtList = pDMKEvt;
    }
    else
    {
        return;
    }
    
    while (pDMKEvt && m_numFreeMessages < kNumMsgsInFreePool)
    {
//        pDMKEvt = (PDMUS_KERNEL_EVENT)ExAllocatePoolWithTag(
//                            NonPagedPool,
//                            sizeof(DMUS_KERNEL_EVENT),
//                            ' FXM');    //  'MXF '
        pDMKEvt = new DMUS_KERNEL_EVENT;
        if (pDMKEvt)
        {
//            InitializeListHead(&(pDMKEvt->listEntry));
            pDMKEvt->cbEvent = 0;
            pDMKEvt->usFlags = 0;
            pDMKEvt->ulChannelGroup = 0;
            pDMKEvt->ullPresTime100Ns = 0;
            pDMKEvt->pNextEvt = NULL;
            pDMKEvt->uData.pbData = NULL;

            m_numFreeMessages++;

            pEvtList->pNextEvt = pDMKEvt;
            pEvtList = pDMKEvt;
        }
    }
}

CAllocatorMXF::~CAllocatorMXF(void)
{
    PDMUS_KERNEL_EVENT  pEvtList;
    //  destroy the pool
    while (m_pFreeList)
    {
        pEvtList = m_pFreeList->pNextEvt;
        m_pFreeList->pNextEvt = NULL;
        delete m_pFreeList;
        m_pFreeList = pEvtList;
    }
}

HRESULT CAllocatorMXF::GetMessage(PDMUS_KERNEL_EVENT *pDMKEvt)
{
    if (m_numFreeMessages > 0)
    {
        //  manufacture a message
        //  take it off the free list
        *pDMKEvt = m_pFreeList;
        m_pFreeList = m_pFreeList->pNextEvt;
        (*pDMKEvt)->pNextEvt = NULL;
        m_numFreeMessages--;
    }
    else
    {
        *pDMKEvt = NULL;
        return E_OUTOFMEMORY;
    }
    return S_OK;
}

//  dispose of a message list
HRESULT CAllocatorMXF::PutMessage(PDMUS_KERNEL_EVENT pDMKEvt)
{
    PDMUS_KERNEL_EVENT  pEvtList;
    
    if (pDMKEvt->pNextEvt == NULL)
    {
        if (!(PACKAGE_EVT(pDMKEvt)))
        {
            pDMKEvt->ulChannelGroup = 0;
            pDMKEvt->ullPresTime100Ns = 0;
//        if (pDMKEvt->cbEvent > sizeof(PBYTE))
//        {
//            free(pDMKEvt->uData.pbData);
//        }
            pDMKEvt->uData.pbData = NULL;
            pDMKEvt->cbEvent = 0;
            pDMKEvt->usFlags = 0;
            pDMKEvt->pNextEvt = m_pFreeList;

            m_pFreeList = pDMKEvt;

            m_numFreeMessages++;
        }
        else
        {
            pEvtList = pDMKEvt->uData.pPackageEvt;
            pDMKEvt->uData.pPackageEvt = NULL;
            PutMessage(pDMKEvt);
            PutMessage(pEvtList);
        }
    }
    else
    {
        pEvtList = pDMKEvt->pNextEvt;
        pDMKEvt->pNextEvt = NULL;
        (void) PutMessage(pDMKEvt);
        (void) PutMessage(pEvtList);
    }
    return S_OK;
}
