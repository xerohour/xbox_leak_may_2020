/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ac97xmo.cpp
 *  Content:    AC97 direct-access media object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  02/05/01    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY      leListEntry;
    XMEDIAPACKET    xmp;
END_DEFINE_STRUCT(AC97PACKET);

//
// AC'97 XMO
//

#ifdef VALIDATE_PARAMETERS

class CAc97MediaObject
    : public XAc97MediaObject, CValidObject<'AC97'>, public CRefCountTest

#else // VALIDATE_PARAMETERS

class CAc97MediaObject
    : public XAc97MediaObject, public CRefCountTest

#endif // VALIDATE_PARAMETERS

{
protected:
    static DWORD                m_dwGlobalRefCount;     // Shared reference count
    static CAc97Device *        m_pDevice;              // Shared AC97 device object
                                                        
protected:                                              
    CAc97Channel *              m_pChannel;             // AC97 channel object
    LIST_ENTRY                  m_lstPending;           // Pending packet list
    LIST_ENTRY                  m_lstFree;              // Free packet list
    LPFNXMEDIAOBJECTCALLBACK    m_pfnCallback;          // Callback function
    LPVOID                      m_pvCallbackContext;    // Callback context
    LPAC97PACKET                m_paPackets;            // Packet pool
    DWORD                       m_dwStatus;             // XMO status

public:
    CAc97MediaObject(void);
    virtual ~CAc97MediaObject(void);

public:
    // Initialization
    virtual HRESULT STDMETHODCALLTYPE Initialize(DWORD dwChannel, LPFNXMEDIAOBJECTCALLBACK pfnCallback, LPVOID pvCallbackContext);
    
    // IUnknown methods
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // XMediaObject methods
    virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
    virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
    virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket);
    virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
    virtual HRESULT STDMETHODCALLTYPE Flush(void);

    // XAc97MediaObject methods
    virtual HRESULT STDMETHODCALLTYPE SetMode(DWORD dwMode);
    virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition(LPDWORD pdwPosition);

protected:
    // Packet completion
    void CompletePendingPackets(void);
    void CompletePacket(LPAC97PACKET pPacket, DWORD dwStatus);
    
private:
    // Event handlers
    static void CALLBACK InterruptCallback(LPVOID pvContext);
};

__inline ULONG CAc97MediaObject::AddRef(void)
{
    _ENTER_EXTERNAL_METHOD("CAc97MediaObject::AddRef");
    return CRefCountTest::AddRef();
}

__inline ULONG CAc97MediaObject::Release(void)
{
    _ENTER_EXTERNAL_METHOD("CAc97MediaObject::Release");
    return CRefCountTest::Release();
}

__inline HRESULT CAc97MediaObject::Discontinuity(void)
{
    return DS_OK;
}

__inline void CAc97MediaObject::InterruptCallback(LPVOID pvContext)
{
    ((CAc97MediaObject *)pvContext)->CompletePendingPackets();
}


/****************************************************************************
 *
 *  Ac97CreateMediaObject
 *
 *  Description:
 *      Creates and initializes an AC97 Media Object.
 *
 *  Arguments:
 *      DWORD [in]: channel identifier.
 *      XMediaObject ** [out]: receives channel object.  The caller is
 *                             responsible for freeing this object with
 *                             Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "Ac97CreateMediaObject"

HRESULT
Ac97CreateMediaObject
(
    DWORD                       dwChannel,
    LPFNXMEDIAOBJECTCALLBACK    pfnCallback, 
    LPVOID                      pvCallbackContext,
    XAc97MediaObject **         ppMediaObject
)
{
    CAc97MediaObject *          pMediaObject    = NULL;
    HRESULT                     hr;
    
    DPF_ENTER();
    ENTER_EXTERNAL_FUNCTION();

    //
    // Create the AC97 media object
    //

    hr = HRFROMP(pMediaObject = NEW(CAc97MediaObject));
   
    if(SUCCEEDED(hr))
    {
        hr = pMediaObject->Initialize(dwChannel, pfnCallback, pvCallbackContext);
    }

    if(SUCCEEDED(hr))
    {
        *ppMediaObject = ADDREF(pMediaObject);
    }

    RELEASE(pMediaObject);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CAc97MediaObject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::CAc97MediaObject"

DWORD CAc97MediaObject::m_dwGlobalRefCount = 0;
CAc97Device *CAc97MediaObject::m_pDevice = NULL;

CAc97MediaObject::CAc97MediaObject
(
    void
)
{
    DPF_ENTER();

    InitializeListHead(&m_lstPending);
    InitializeListHead(&m_lstFree);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CAc97MediaObject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::~CAc97MediaObject"

CAc97MediaObject::~CAc97MediaObject
(
    void
)
{
    DPF_ENTER();

    //
    // Flush any remaining packets
    //

    Flush();

    //
    // Free the channel
    //

    RELEASE(m_pChannel);

    //
    // Free resources
    //

    MEMFREE(m_paPackets);

    //
    // Release the shared stuff
    //

    ASSERT(m_dwGlobalRefCount);
    
    if(!--m_dwGlobalRefCount)
    {
        DELETE(m_pDevice);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      DWORD [in]: channel type.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::Initialize"

HRESULT
CAc97MediaObject::Initialize
(
    DWORD                       dwChannel,
    LPFNXMEDIAOBJECTCALLBACK    pfnCallback, 
    LPVOID                      pvCallbackContext
)
{
    HRESULT                     hr              = DS_OK;
    AC97CHANNELTYPE             nChannelType;
    DWORD                       i;
    
    DPF_ENTER();

    m_pfnCallback = pfnCallback;
    m_pvCallbackContext = pvCallbackContext;

    //
    // Set up data shared between all instances of the AC97 XMO
    //

    if(!m_dwGlobalRefCount++)
    {
        ASSERT(!m_pDevice);

        hr = HRFROMP(m_pDevice = NEW(CAc97Device));

        if(SUCCEEDED(hr))
        {
            hr = m_pDevice->Initialize();
        }
    }
    else
    {
        ASSERT(m_pDevice);
    }

    //
    // Create the audio channel
    //

    if(SUCCEEDED(hr))
    {
        switch(dwChannel)
        {
            case DSAC97_CHANNEL_ANALOG:
                nChannelType = AC97_CHANNELTYPE_ANALOG;
                break;

            case DSAC97_CHANNEL_DIGITAL:
                nChannelType = AC97_CHANNELTYPE_DIGITAL;
                break;

            default:
                ASSERTMSG("Invalid channel type");
                break;
        }
    }

    if(SUCCEEDED(hr))
    {
        if(m_pDevice->m_apChannels[nChannelType])
        {
            DPF_ERROR("Channel already open");
            hr = DSERR_INVALIDCALL;
        }
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pDevice->CreateChannel(nChannelType, &m_pChannel);
    }

    if(SUCCEEDED(hr))
    {
        hr = m_pChannel->Initialize(AC97CHANNEL_OBJECTF_STREAM, InterruptCallback, this);
    }

    //
    // Allocate packet data
    //

    if(SUCCEEDED(hr))
    {
        hr = HRFROMP(m_paPackets = MEMALLOC(AC97PACKET, DSAC97_MAX_ATTACHED_PACKETS));
    }

    if(SUCCEEDED(hr))
    {
        for(i = 0; i < DSAC97_MAX_ATTACHED_PACKETS; i++)
        {
            InsertTailListUninit(&m_lstFree, &m_paPackets[i].leListEntry);
        }
    }

    //
    // Set the initial status
    //

    if(SUCCEEDED(hr))
    {
        m_dwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the object supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [in/out]: info data.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::GetInfo"

HRESULT
CAc97MediaObject::GetInfo
(
    LPXMEDIAINFO            pInfo
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pInfo)
    {
        DPF_ERROR("XMO info buffer not supplied");
    }

#endif // VALIDATE_PARAMETERS

    ZeroMemory(pInfo, sizeof(*pInfo));

    //
    // Flags are easy
    //
    
    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_INPUT_ASYNC;

    //
    // Input size is just sample-aligment, since that's the smallest amount of
    // data we require to process.
    //

    pInfo->dwInputSize = m_pDevice->m_wfxFormat.nBlockAlign;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Flush
 *
 *  Description:
 *      Resets the stream to it's default state.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::Flush"

HRESULT
CAc97MediaObject::Flush
(
    void
)                                       
{                                           
    PLIST_ENTRY             plePacket;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

    //
    // Reset the channel
    //
    
    if(m_pChannel)
    {
        m_pChannel->Flush();
    }

    //
    // Reset our internal packet list
    //

    while((plePacket = m_lstPending.Flink) != &m_lstPending)
    {
        MoveEntryTailList(&m_lstFree, plePacket);
        
        CompletePacket(CONTAINING_RECORD(plePacket, AC97PACKET, leListEntry), XMEDIAPACKET_STATUS_FLUSHED);
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  GetStatus
 *
 *  Description:
 *      Gets stream status.
 *
 *  Arguments:
 *      LPDWORD [out]: stream status.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::GetStatus"

HRESULT
CAc97MediaObject::GetStatus
(
    LPDWORD                 pdwStatus
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdwStatus)
    {
        DPF_ERROR("Status buffer not supplied");
    }

#endif // VALIDATE_PARAMETERS

    *pdwStatus = m_dwStatus;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Submits buffers to the stream.
 *
 *  Arguments:
 *      LPXMEDIAPACKET  [in]: input buffer.
 *      LPXMEDIAPACKET  [in]: output buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::Process"

HRESULT
CAc97MediaObject::Process
(
    LPCXMEDIAPACKET         pxmbSource, 
    LPCXMEDIAPACKET         pxmbDest 
)
{
    HRESULT                 hr                      = DS_OK;
    PLIST_ENTRY             plePacket;
    LPAC97PACKET            pPacket;
    CIrql                   irql;

#ifdef VALIDATE_PARAMETERS

    DWORD                   dwBasePhysicalAddress;
    DWORD                   dwContiguousLength;
    DWORD                   dwPhysicalAddress;

#endif // VALIDATE_PARAMETERS

    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pxmbSource)
    {
        DPF_ERROR("No input buffer supplied");
    }

    if(!pxmbSource->pvBuffer)
    {
        DPF_ERROR("No input data buffer supplied");
    }

    if(!pxmbSource->dwMaxSize)
    {
        DPF_ERROR("No input buffer size");
    }

    if(pxmbSource->dwMaxSize % m_pDevice->m_wfxFormat.nBlockAlign)
    {
        DPF_ERROR("Input buffer size not block-aligned");
    }

    if(pxmbSource->dwMaxSize > 131068)
    {
        DPF_ERROR("Input buffers are restricted to 131068 bytes or less");
    }

    if(pxmbDest)
    {
        DPF_ERROR("AC97 streams are input only");
    }

#endif // VALIDATE_PARAMETERS

    //
    // Get a packet from the free list and add it to the pending list
    //

    irql.Raise();
    
    if((plePacket = MoveHeadTailList(&m_lstPending, &m_lstFree)) == &m_lstFree)
    {
        DPF_ERROR("No free packets");
        hr = DSERR_INVALIDCALL;
    }

    //
    // Initialize the packet data and add it to the pending list
    //

    if(SUCCEEDED(hr))
    {
        pPacket = CONTAINING_RECORD(plePacket, AC97PACKET, leListEntry);
        
        pPacket->xmp = *pxmbSource;

        XMOAcceptPacket(&pPacket->xmp);

        DPF_BLAB("Accepted packet %x", pPacket);
    }

    irql.Lower();

    //
    // Lock packet data
    //

    if(SUCCEEDED(hr))
    {
        MmLockUnlockBufferPages(pPacket->xmp.pvBuffer, pPacket->xmp.dwMaxSize, FALSE);

#ifdef VALIDATE_PARAMETERS

        dwBasePhysicalAddress = MmGetPhysicalAddress(pxmbSource->pvBuffer);
        dwContiguousLength = PAGE_SIZE - BYTE_OFFSET(pxmbSource->pvBuffer); 

        while(dwContiguousLength < pxmbSource->dwMaxSize)
        {
            dwPhysicalAddress = MmGetPhysicalAddress((LPBYTE)pxmbSource->pvBuffer + dwContiguousLength);
            ASSERT(!(dwPhysicalAddress & (PAGE_SIZE - 1)));

            if(dwBasePhysicalAddress + dwContiguousLength != dwPhysicalAddress)
            {
                DPF_ERROR("Packet data submitted to the AC97 XMO must be physically contiguous.  Use XPhysicalAlloc");
                break;
            }

            dwContiguousLength += PAGE_SIZE;
        }

#endif // VALIDATE_PARAMETERS

    }

    //
    // Send the packet to the channel
    //

    if(SUCCEEDED(hr))
    {
        m_pChannel->AttachPacket(pPacket->xmp.pvBuffer, pPacket->xmp.dwMaxSize);
    }

    //
    // Signal a discontinuity to the channel.  This will prevent the channel
    // from playing crap if the stream starves.  It's really low overhead to
    // do this, so better safe than sorry.  As a side effect of this, the
    // AC97 channel will never complain about being starved.  Oh well.
    //

    if(SUCCEEDED(hr))
    {
        m_pChannel->Discontinuity();
    }

    //
    // Start the channel running
    //

    if(SUCCEEDED(hr))
    {
        m_pChannel->Run();
    }

    //
    // Update the status
    //

    if(SUCCEEDED(hr) && IsListEmpty(&m_lstFree))
    {
        ASSERT(XMO_STATUSF_ACCEPT_INPUT_DATA == m_dwStatus);

        m_dwStatus = 0;
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  CompletePendingPackets
 *
 *  Description:
 *      Completes all packets in the pending list that have been played.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::CompletePendingPackets"

void
CAc97MediaObject::CompletePendingPackets
(
    void
)
{
    PLIST_ENTRY             plePacket;
    LPAC97PACKET            pPacket;
    CIrql                   irql;
    
    DPF_ENTER();

    //
    // Complete the first packet in the list
    //

    irql.Raise();

    if((plePacket = MoveHeadTailList(&m_lstFree, &m_lstPending)) != &m_lstPending)
    {
        pPacket = CONTAINING_RECORD(plePacket, AC97PACKET, leListEntry);
    }
    else
    {
        pPacket = NULL;
    }

    irql.Lower();

    if(pPacket)
    {
        CompletePacket(pPacket, XMEDIAPACKET_STATUS_SUCCESS);
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CompletePacket
 *
 *  Description:
 *      Marks a packet as complete.
 *
 *  Arguments:
 *      LPAC97PACKET [in]: packet.
 *      DWORD [in]: status.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::CompletePacket"

void
CAc97MediaObject::CompletePacket
(
    LPAC97PACKET            pPacket,
    DWORD                   dwStatus
)
{
    DPF_ENTER();

    DPF_BLAB("Completing packet %x", pPacket);

    //
    // Unlock packet data
    //

    MmLockUnlockBufferPages(pPacket->xmp.pvBuffer, pPacket->xmp.dwMaxSize, TRUE);

    //
    // Update the status to reflect the fact that we have at least one
    // free packet.
    //

    ASSERT(!IsListEmpty(&m_lstFree));
    
    m_dwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA;

    //
    // Finalize completion
    //

    XMOCompletePacket(&pPacket->xmp, pPacket->xmp.dwMaxSize, m_pfnCallback, m_pvCallbackContext, dwStatus);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetMode
 *
 *  Description:
 *      Sets the channel mode.
 *
 *  Arguments:
 *      DWORD [in]: mode.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::SetMode"

HRESULT
CAc97MediaObject::SetMode
(
    DWORD                   dwMode
)
{
    HRESULT                 hr              = DS_OK;
    DWORD                   dwSpeakerConfig;
    
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if((DSAC97_MODE_PCM != dwMode) && (DSAC97_MODE_ENCODED != dwMode))
    {
        DPF_ERROR("Invalid channel mode");
    }

    if((AC97_CHANNELTYPE_ANALOG == m_pChannel->m_nChannelType) && (DSAC97_MODE_ENCODED == dwMode))
    {
        DPF_ERROR("The analog channel can only output PCM data");
    }

#endif // VALIDATE_PARAMETERS

    if(DSAC97_MODE_PCM != dwMode)
    {
        dwSpeakerConfig = XAudioGetSpeakerConfig();

        if(!(dwSpeakerConfig & DSSPEAKER_ENCODED_VALID))
        {
            DPF_ERROR("Encoded output not enabled");
            hr = DSERR_INVALIDCALL;
        }
    }
    
    if(SUCCEEDED(hr))
    {
        m_pChannel->SetMode(dwMode);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/****************************************************************************
 *
 *  GetCurrentPosition
 *
 *  Description:
 *      Gets the current stream position.
 *
 *  Arguments:
 *      LPDWORD [out]: stream position.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CAc97MediaObject::GetCurrentPosition"

HRESULT
CAc97MediaObject::GetCurrentPosition
(
    LPDWORD                 pdwPosition
)
{
    DPF_ENTER();
    ENTER_EXTERNAL_METHOD();

#ifdef VALIDATE_PARAMETERS

    if(!pdwPosition)
    {
        DPF_ERROR("Failed to supply a position buffer");
    }

#endif // VALIDATE_PARAMETERS

    *pdwPosition = m_pChannel->GetPosition();

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


