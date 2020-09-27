/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvsndt.cpp
 *  Content:	Implementation of CSoundTarget class
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 09/02/99		rodtoll	Created
 * 09/01/2000   georgioc  started complete rewrite to use xmos
 ***************************************************************************/

#include "dvsndt.h"
#include <stdio.h>

#define SOUNDTARGET_WRITEAHEAD			2

// Max # of restarts to attempt on a buffer
#define SOUNDTARGET_MAX_RESTARTS		10

// Max # of frames of silence which are written ahead of the latest frame
// of audio
#define SOUNDTARGET_MAX_WRITEAHEAD		3


#undef DPF_MODNAME
#define DPF_MODNAME "CSoundTarget::Initialize"
//
// Initialize
//
HRESULT CSoundTarget::Initialize(
    DWORD dwFrameSize
    )
{

	m_dwFrameSize = dwFrameSize;
    return Flush();

}

HRESULT STDMETHODCALLTYPE CSoundTarget::Flush()    
{
    PDVMEDIAPACKET pEntry;
    HRESULT hr;
    
    DV_AUTO_LOCK(&m_CS);

    //
    // if there are any pending buffers, release them
    //

   
    if (!IsListEmpty(&m_PendingList)) {

        DV_FreeList(&m_PendingList, DVLAT_RAW);
        DV_FreeList(&m_AvailableList, DVLAT_RAW);

    }

    if (m_pMediaPacketBuffer) {

        DV_POOL_FREE(m_pMediaPacketBuffer);
        m_pMediaPacketBuffer = NULL;

    }

    InitializeListHead(&m_PendingList);

    if (IsListEmpty(&m_AvailableList)) {

        hr = DV_InitializeList(&m_AvailableList,
                               ST_MAX_FREE_BUFFER_CONTEXTS,
                               sizeof(XMEDIAPACKET),
                               DVLAT_RAW);
        if (FAILED(hr)) {            
            return hr;
        }

        m_pMediaPacketBuffer = m_AvailableList.Flink;

    }
    
    return NOERROR;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CSoundTarget::MixInSingle"

HRESULT CSoundTarget::MixInSingle( LPBYTE lpbBuffer )
{
	HRESULT hr;
    PXMEDIAPACKET pXmp;

    DV_AUTO_LOCK(&m_CS);

    DPVF_ENTER();

    //
    // check if we have any pending XMO output buffers
    //

    if (IsListEmpty(&m_PendingList)) {

        //
        // no buffers available, we have to discard the data..
        //

        DPVF(DPVF_WARNINGLEVEL,"No pending buffers, throwing away data!");
        return S_OK; 
    }

    //
    // take a context out, and complete it
    //

    pXmp = (PXMEDIAPACKET) DV_REMOVE_TAIL(&m_PendingList);
    memcpy((PUCHAR)pXmp->pvBuffer,
           lpbBuffer,
           m_dwFrameSize);


    DPVF(DPVF_SPAMLEVEL,"Mixing single frame, context %x",pXmp);

    if (pXmp->pdwCompletedSize) {

        *pXmp->pdwCompletedSize = m_dwFrameSize;

    }
    
    if (pXmp->pdwStatus) {

        *pXmp->pdwStatus = XMEDIAPACKET_STATUS_SUCCESS;

    }

    if (pXmp->hCompletionEvent) {

        SetEvent(pXmp->hCompletionEvent);

    }

    DV_INSERT_TAIL(&m_AvailableList,pXmp);
    return DV_OK;

}

HRESULT STDMETHODCALLTYPE CSoundTarget::GetStatus( 
    /* [out] */ DWORD __RPC_FAR *dwFlags)
{
    DV_AUTO_LOCK(&m_CS);
    *dwFlags = 0;

    if (!IsListEmpty(&m_AvailableList)) {

        *dwFlags = XMO_STATUSF_ACCEPT_OUTPUT_DATA;

    }

    return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CSoundTarget::Process"

HRESULT CSoundTarget::Process( 
    LPCXMEDIAPACKET pSrc,
    LPCXMEDIAPACKET pDst
    )
{
    PXMEDIAPACKET pXmp;
    DV_AUTO_LOCK(&m_CS);
    DPVF_ENTER();

    //
    // they are only supposed to call us async..
    //

    ASSERT (pDst->dwMaxSize >= m_dwFrameSize);
    ASSERT (pSrc==NULL);

    //
    // add this buffer to our media buffer list
    // next time the DVClientEngine has data 
    // a queued buffer will get the data and get released...
    //

    ASSERT(!IsListEmpty(&m_AvailableList));

    pXmp = (PXMEDIAPACKET) DV_REMOVE_TAIL(&m_AvailableList);

    DPVF(DPVF_SPAMLEVEL,"adding context %x to pending list!",pXmp);

    //
    // pending pool is a FIFO. Queue at head, dequeue at tail
    //

    memcpy(pXmp,pDst,sizeof(XMEDIAPACKET));
    *pXmp->pdwStatus = XMEDIAPACKET_STATUS_PENDING;  

    DV_INSERT_HEAD(&m_PendingList,pXmp);

    return S_OK;
}

