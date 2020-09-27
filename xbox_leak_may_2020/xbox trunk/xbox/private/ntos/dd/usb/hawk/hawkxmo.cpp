/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    hawk.CPP

Abstract:


Environment:

    kernel mode

Revision History:


--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************
#include "hawkxmo.h"

#if DBG || DEBUG_LOG

    ULONG               HawkDebugLevel=3;     // Level of debug output

#endif


//------------------------------------------------------------------------------
//  Declare Hawk types and class.
//------------------------------------------------------------------------------
DECLARE_XPP_TYPE(XDEVICE_TYPE_CHAT_MICROPHONE)
DECLARE_XPP_TYPE(XDEVICE_TYPE_CHAT_HEADPHONE)
USB_DEVICE_TYPE_TABLE_BEGIN(Hawk)
USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_CHAT_MICROPHONE, USB_CONNECTOR_TYPE_HIGH_POWER,1,0,0,2,4,40,0),
USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_CHAT_HEADPHONE, USB_CONNECTOR_TYPE_HIGH_POWER,1,0,0,2,4,40,0)
USB_DEVICE_TYPE_TABLE_END()
USB_CLASS_DRIVER_DECLARATION(Hawk, USB_DEVICE_CLASS_AUDIO, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION_DUPLICATE(Hawk, 1, XBOX_DEVICE_CLASS_AUDIO_DEVICE, 0xFF, 0xFF)
#pragma data_seg(".XPP$ClassHawk")
USB_CLASS_DECLARATION_POINTER(Hawk)
USB_CLASS_DECLARATION_POINTER_DUPLICATE(Hawk, 1)
#pragma data_seg(".XPP$Data")

//------------------------------------------------------------------------------
// USB Class Driver Interface
//------------------------------------------------------------------------------

EXTERNUSB VOID
HawkInit ()
{
    DBGPRINT(2, ("enter: HawkInit\n"));
    
    XHawkMediaObject::InitializeClass();

    DBGPRINT(2, ("exit:  HawkInit\n"));    
    return;
}


EXTERNUSB VOID
HawkAddDevice(
     IN IUsbDevice *Device
     )
{
	const USB_ENDPOINT_DESCRIPTOR   *pEndpointDescriptor;
    XHawkMediaObject *pHawkMediaObject = NULL;
    DWORD dwPort;

    //
	//  Get the device port and slot
	//

    dwPort = Device->GetPort();
    if(dwPort >= 16)
    {
        DBGPRINT(2,("AddDevice: dwPort %d, bottom slot, not supported!\n",  dwPort));
        Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
        return;
    }

    //
    //  get an endpoint descriptor for the isoch endpoint
    //  (we don't know the direction (microphone or
    //  headphone).  Try input first.
    //

    pEndpointDescriptor = Device->GetEndpointDescriptor(
                                USB_ENDPOINT_TYPE_ISOCHRONOUS,
                                TRUE,
                                0);
    if(pEndpointDescriptor)
    {
        pHawkMediaObject = &XHawkMediaObject::sm_pMicrophones[dwPort];
        pHawkMediaObject->m_pTypeSettings = &pHawkMediaObject->sm_MicrophoneSettings;
    }
    else {
        pEndpointDescriptor = Device->GetEndpointDescriptor(
                                USB_ENDPOINT_TYPE_ISOCHRONOUS,
                                FALSE,
                                0);
        if(pEndpointDescriptor)
        {
            pHawkMediaObject = &XHawkMediaObject::sm_pHeadphones[dwPort];
            pHawkMediaObject->m_pTypeSettings = &pHawkMediaObject->sm_HeadphoneSettings;
        } else 
        {
            ASSERT(FALSE && "Isoch endpoint not found");
            Device->AddComplete(USBD_STATUS_UNSUPPORTED_DEVICE);
            return;
        }
    }
    
    //
    //  Check and initialize the state 
    //
    pHawkMediaObject->m_fConnected = TRUE;

    //
    // Use XHawkMediaObject as the device extension
    //
    
    Device->SetExtension(pHawkMediaObject);
    pHawkMediaObject->m_pDevice = Device;
    
    //
    //  Record the endpoint address, and wMaxPacket
    //

    pHawkMediaObject->m_bEndpointAddress = pEndpointDescriptor->bEndpointAddress;
    pHawkMediaObject->m_wMaxPacket = pEndpointDescriptor->wMaxPacketSize;

    DBGPRINT(2,("IsochMaxPacket = %x, bEndpointAddress = 0x%02x\n, Interface = %d\n", 
                     (ULONG)pEndpointDescriptor->wMaxPacketSize,
                     (ULONG)pEndpointDescriptor->bEndpointAddress,
                     Device->GetInterfaceNumber()
                     ));

    //
    // Set the class specific type so it can be enumerated
    //
    Device->SetClassSpecificType(
                (UCHAR)pHawkMediaObject->m_pTypeSettings->ulHawkClassSpecificType
                );

    Device->AddComplete(USBD_STATUS_SUCCESS);
}


EXTERNUSB VOID
HawkRemoveDevice (
    IN IUsbDevice *Device
    )
{
    XHawkMediaObject *pHawkMediaObject = (XHawkMediaObject *)Device->GetExtension();
 
    //
    //  Mark the device as not connected 
    //

    pHawkMediaObject->m_fConnected = FALSE;

    //
    //  If the device is opened, we cannot call
    //  remove complete, because there is
    //  an endpoint open (or in the process
    //  of being closed.
    //

    if(pHawkMediaObject->m_fOpened)
    {
        //
        //  Mark the media object remove pending,
        //  so that when the endpoint finishes
        //  closing, the CloseEndpointComplete
        //  routine will call RemoveComplete.
        //

        pHawkMediaObject->m_fRemovePending = TRUE;

        //
        //  If the endpoint is not in the process
        //  of being closed, then start that process
        //

        if(!pHawkMediaObject->m_fClosePending)
        {
            pHawkMediaObject->CloseEndpoint();
        }

    } else

    //
    //  The media object wasn't opened, so we can
    //  call remove complete
    //

    {
        pHawkMediaObject->m_pDevice = NULL;
        Device->SetExtension(NULL);
        Device->RemoveComplete();
    }
}    

//------------------------------------------------------------------------------
//  We do not include the CRT, but we need new.
//  we declare it static, so we don't conflict on linking
//  with other modules overriding new.
//------------------------------------------------------------------------------
#pragma warning(disable:4211)
static __inline void * __cdecl operator new(size_t memSize)
    {
        return ExAllocatePoolWithTag(
                            memSize,
                            'kwah'
                            );
    }
static __inline void __cdecl operator delete(void *pMemory)
    {
        ExFreePool(pMemory);
    }
#pragma warning(default:4211)

//------------------------------------------------------------------------------
//  Declaration of static tables
//------------------------------------------------------------------------------
XHawkMediaObject *XHawkMediaObject::sm_pMicrophones = NULL;
XHawkMediaObject *XHawkMediaObject::sm_pHeadphones = NULL;


WAVEFORMATEX 
XHawkMediaObject::sm_MicrophoneWaveformat = 
    {
        WAVE_FORMAT_PCM, //wFormatTag
        1,               //nChannels
        HAWK_MICROPHONE_SAMPLE_RATE*1000,   //nSamplesPerSec 
        HAWK_MICROPHONE_SAMPLE_RATE*1000*
        HAWK_MICROPHONE_BYTES_PER_SAMPLE,   //nAvgBytesPerSec
        HAWK_MICROPHONE_BYTES_PER_SAMPLE,   //nBlockAlign
        HAWK_MICROPHONE_BYTES_PER_SAMPLE*8, //wBitsPerSample
        0                                  //cbSize
    };

WAVEFORMATEX 
XHawkMediaObject::sm_HeadphoneWaveformat = 
    {
        WAVE_FORMAT_PCM, //wFormatTag
        1,               //nChannels
        HAWK_HEADPHONE_SAMPLE_RATE*1000,   //nSamplesPerSec 
        HAWK_HEADPHONE_SAMPLE_RATE*1000*
        HAWK_HEADPHONE_BYTES_PER_SAMPLE,   //nAvgBytesPerSec
        HAWK_HEADPHONE_BYTES_PER_SAMPLE,   //nBlockAlign
        HAWK_HEADPHONE_BYTES_PER_SAMPLE*8, //wBitsPerSample
        0                //cbSize
    };  

HAWK_TYPE_SETTINGS 
XHawkMediaObject::sm_MicrophoneSettings = 
    {
        HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE,        //ulHawkClassSpecificType
        &XHawkMediaObject::sm_MicrophoneWaveformat, //pWaveFormat
        XMO_STREAMF_WHOLE_SAMPLES	|
	    XMO_STREAMF_FIXED_SAMPLE_SIZE |
        XMO_STREAMF_OUTPUT_ASYNC,                   //dwFlags
        HAWK_BYTES_PER_USB_FRAME,                   //cbSize
        0,                                          //cbMaxLookahead
        1                                           //cbAlignment
    };

HAWK_TYPE_SETTINGS 
XHawkMediaObject::sm_HeadphoneSettings = 
    {
        HAWK_HEADPHONE_CLASS_SPECIFIC_TYPE,        //ulHawkClassSpecificType
        &XHawkMediaObject::sm_HeadphoneWaveformat, //pWaveFormat
        XMO_STREAMF_WHOLE_SAMPLES |
	    XMO_STREAMF_FIXED_SAMPLE_SIZE |
        XMO_STREAMF_INPUT_ASYNC,                    //dwFlags
        HAWK_BYTES_PER_USB_FRAME,                   //cbSize
        0,                                          //cbMaxLookahead
        1                                           //cbAlignment
    };

//------------------------------------------------------------------------------
// XHawkMediaObject
//------------------------------------------------------------------------------
void XHawkMediaObject::InitializeClass()
{
    //
    //  Allocate a microphone and a headphone
    //  instance for each of the ports up front.
    //
    ULONG ulPortCount = XGetPortCount();
    sm_pHeadphones = new XHawkMediaObject[ulPortCount];
    sm_pMicrophones = new XHawkMediaObject[ulPortCount];
    
    //
    //  We could loop over them and morph them into
    //  microphones and headphones, but we might
    //  as leave that until an AddDevice.
    //
}


STDMETHODIMP_(ULONG) XHawkMediaObject::AddRef (void)
{
	return (ULONG) InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) XHawkMediaObject::Release (void)
{
	LONG lRefCount;
	lRefCount = InterlockedDecrement(&m_lRefCount);
	//
	//	Check for decrement below zero
	//
	if(0>lRefCount)
	{
		DBGPRINT(1, ("XHawkMediaObject: Reference Count Error: Count went below 0!\n"));
		return 0;
	} else if (0==lRefCount)
	{
        FreeStreamingResources();
	}
	return (ULONG)lRefCount;
}

STDMETHODIMP
XHawkMediaObject::GetInfo (
    IN OUT PXMEDIAINFO pInfo
    )
{
    #pragma TODO("RIP args for XHawkMediaObject::GetInputInfo")
    
    pInfo->dwFlags = m_pTypeSettings->dwFlags;
    
    if (HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE == m_pTypeSettings->ulHawkClassSpecificType) {

        pInfo->dwInputSize = 0;
        pInfo->dwOutputSize = m_pTypeSettings->cbSize;

    } else {

        pInfo->dwInputSize = m_pTypeSettings->cbSize;
        pInfo->dwOutputSize = 0;
    }

    pInfo->dwMaxLookahead = m_pTypeSettings->cbMaxLookahead;
    
    return ERROR_SUCCESS;
}
        

STDMETHODIMP 
XHawkMediaObject::Flush (void)
{
    #pragma TODO("Implement Flush")
    return E_NOTIMPL;
}
        

STDMETHODIMP 
XHawkMediaObject::Discontinuity (void)
{
    return S_OK;
}
        

STDMETHODIMP
XHawkMediaObject::GetStatus (
            OUT DWORD *pdwFlags
            )
{
    
    #pragma TODO("RIP args for XHawkMediaObject::GetStatus")

    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    
    if( m_fConnected && m_pStreamingResources->Free.GetHead())
    {
        if(HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE == m_pTypeSettings->ulHawkClassSpecificType)
        {
            *pdwFlags = XMO_STATUSF_ACCEPT_OUTPUT_DATA;
    
        } else
        {
            *pdwFlags = XMO_STATUSF_ACCEPT_INPUT_DATA;            
        }

    } else
    {
        *pdwFlags = 0;
    }
    
    KeLowerIrql(oldIrql);
    
    return S_OK;
}
        
HRESULT
XHawkMediaObject::Process (
    IN const XMEDIAPACKET *pInputPacket,
    IN const XMEDIAPACKET *pOutputPacket
    )
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    HRESULT hr = S_OK;
    const XMEDIAPACKET *pMediaPacket;
    
    if(HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE == m_pTypeSettings->ulHawkClassSpecificType)
    {
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", NULL==pInputPacket);
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", NULL!=pOutputPacket);
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", pOutputPacket->dwMaxSize > 0);
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", 0 == (pOutputPacket->dwMaxSize % m_pTypeSettings->cbSize));
        
        pMediaPacket = pOutputPacket;
         
    } else 
    {
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", NULL!=pInputPacket);
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", NULL==pOutputPacket);
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", pInputPacket->dwMaxSize > 0);
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", 0 == (pInputPacket->dwMaxSize % m_pTypeSettings->cbSize));

        pMediaPacket = pInputPacket;
    }

    if(m_fConnected)
    {

        PHAWK_PACKET_CONTEXT pPacketContext = m_pStreamingResources->Free.RemoveHead();    
        if(pPacketContext)
        {

            //
            //  Copy the media packet into the hawk packet context.
            //  The structure is identical except hawk has one few extra
            //  field, for queuing
            //

            RtlCopyMemory(&pPacketContext->mediaPacket, pMediaPacket, sizeof(XMEDIAPACKET));

            //
            //  Record that we haven't done anything yet, and that 
            //  the request is pending.
            //
            XMOAcceptPacket(&pPacketContext->mediaPacket);
            pPacketContext->dwCompletedSize=0;

            //
            //  Insert the packet onto the pending programming queue.
            //

            m_pStreamingResources->PendingProgram.InsertTail(pPacketContext);
            
            //
            //  Jog the programming routine.
            //

            ProgramTransfer();
            
        } else
        {
            hr = HRESULT_FROM_WIN32(ERROR_BUSY);
        }

    } else 
    {
        hr = HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }

    if(FAILED(hr))
    {
        XMOCompletePacket(pMediaPacket, 0, m_pfnCallback, m_pvContext, XMEDIAPACKET_STATUS_FAILURE);
    }

    KeLowerIrql(oldIrql);
    return hr;

}


EXTERN_C HRESULT WINAPI
XHawkCreateMediaObjectInternal(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      OUT LPWAVEFORMATEX pwfxFormat OPTIONAL,
      PFNXMEDIAOBJECTCALLBACK    pfnCallback,
      PVOID                      pvContext,
      OUT XMediaObject **ppXmediaObject
      )
{
    HRESULT hr;
    XHawkMediaObject *pHawkMediaObject = NULL;

    hr = XHawkCreateMediaObject(
          XppDeviceType,
          dwPort,
          dwMaxAttachedPackets,
          pwfxFormat,
          ppXmediaObject
          );

    if (SUCCEEDED(hr)) {

        //
        // save callback and context
        //

        pHawkMediaObject = (XHawkMediaObject *) *ppXmediaObject;
        pHawkMediaObject->m_pvContext = pvContext;
        pHawkMediaObject->m_pfnCallback = pfnCallback;

    }


    return hr;

}

EXTERN_C HRESULT WINAPI
XHawkCreateMediaObject(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      OUT LPWAVEFORMATEX pwfxFormat OPTIONAL,
      OUT XMediaObject **ppXmediaObject
      )
/*++
    Routine Description:
        This is the factory method for XHawkMediaObjects.
        It is an exported API for games.
--*/
{
    #pragma TODO("RIP args for XcreateHawkMediaObject")

    //
    //  We need the class Id to find the device.
    //
	
    KIRQL		     oldIrql;
    PNP_CLASS_ID     classId;
    HRESULT          hr = S_OK;
    XHawkMediaObject *pHawkMediaObject = NULL;
    USHORT           portBit = (SHORT)(1 << dwPort);
    
    
    //
    //  Use raised Irql to synchronize the allocation
    //  and opening of instances
    //

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Claim an existing instance of XHawkMediaObject
    //
	
    if(XDEVICE_TYPE_CHAT_MICROPHONE == XppDeviceType)
	{
	    pHawkMediaObject = &XHawkMediaObject::sm_pMicrophones[dwPort];
    } else if(XDEVICE_TYPE_CHAT_HEADPHONE == XppDeviceType)
    {

        pHawkMediaObject = &XHawkMediaObject::sm_pHeadphones[dwPort];
    } else
    {
        RIP("XHawkCreateMediaObject: XppDeviceType is not a valid type for XHawkCreateMediaObject\n");
        return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
	};
	
    if( pHawkMediaObject->m_fOpened )
    {
        pHawkMediaObject = NULL;
        hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);

    } else if (pHawkMediaObject->m_fConnected)
    {

        //
        //  Allocate the streaming resources.
        //
        hr = pHawkMediaObject->AllocateStreamingResources(dwMaxAttachedPackets);
        if(SUCCEEDED(hr))
        {
            pHawkMediaObject->m_fOpened = TRUE;
            pHawkMediaObject->m_lRefCount = 1;
        } else
        {
           pHawkMediaObject = NULL;
        }

    } else
    {
        pHawkMediaObject = NULL;
        hr = HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }

    //
    //  Restore Irql since the stuff needs
    //  to be synchronously is done.
    //

    KeLowerIrql(oldIrql);

    //
    //  Fill out the pWaveFormatEx output
    //
    if(pwfxFormat && pHawkMediaObject)
    {
        RtlCopyMemory(
            pwfxFormat,
            pHawkMediaObject->m_pTypeSettings->pWaveFormat,
            sizeof(*pwfxFormat)
            );
    }

    *ppXmediaObject = pHawkMediaObject;	
    return hr;
}


HRESULT XHawkMediaObject::AllocateStreamingResources(ULONG ulMaxAttachedPackets)
/*++
    Routine Description:
        This is a helper function called by Factory Method,
        XcreateHawkMediaObject, to allocate resources for
        providing streaming.
--*/
{
    ULONG i;
    
    //
    //  Allocate memory for streaming resources
    //
    ULONG AllocLength = sizeof(HAWK_STREAMING_RESOURCES) + 
                        (sizeof(HAWK_PACKET_CONTEXT)*ulMaxAttachedPackets);
    PUCHAR pResourceMemory = new UCHAR[AllocLength];
    if(!pResourceMemory) return E_OUTOFMEMORY;    

    //
    //  Zero the memory for good measure.
    //

    RtlZeroMemory(pResourceMemory, AllocLength);
    
    //
    //  Break that memory up into the pStreamingResouces and ulMaxAttachedPackets
    //  count of packet contexts.
    //
    
    PHAWK_STREAMING_RESOURCES pResources = (PHAWK_STREAMING_RESOURCES)pResourceMemory;
    PHAWK_PACKET_CONTEXT  pPacketContextArray = (PHAWK_PACKET_CONTEXT) (pResourceMemory +
                                                sizeof(HAWK_STREAMING_RESOURCES));

    //
    // Populate the free list of packet contexts
    //

    for(i=0; i < ulMaxAttachedPackets; i++)
    {
        pResources->Free.InsertTail(pPacketContextArray + i);
    }
         
    //
    //  Open the isoch endpoint
    //

    URB_ISOCH_OPEN_ENDPOINT OpenUrb;
    USB_BUILD_ISOCH_OPEN_ENDPOINT(
                &OpenUrb,
                m_bEndpointAddress,
                m_wMaxPacket,
                HAWK_ISOCH_MAX_ATTACH_BUFFERS,
                0);
    USBD_STATUS status = m_pDevice->SubmitRequest((PURB)&OpenUrb);
    
    if(USBD_ERROR(status))
    {
        delete pResources;
        return HRESULT_FROM_WIN32(IUsbDevice::Win32FromUsbdStatus(status));
    }
    pResources->EndpointHandle = OpenUrb.EndpointHandle;

    //
    //  Initialize the portion of the IsochBufferDescriptor
    //  that is the same for all transfers.
    //

    for(i =0; i < 8; i++)
    {
        pResources->IsochBufferDescriptor.Pattern[i] = HAWK_BYTES_PER_USB_FRAME;
    }
    pResources->IsochBufferDescriptor.TransferComplete = (PFNUSBD_ISOCH_TRANSFER_COMPLETE)XHawkMediaObject::TransferComplete;
    pResources->IsochBufferDescriptor.Context = (PVOID) this;

    //
    //  start the stream (since we don't have a start API)
    //

    URB_ISOCH_START_TRANSFER StartTransferUrb;
    USB_BUILD_ISOCH_START_TRANSFER(
        &StartTransferUrb,
        pResources->EndpointHandle,
        0,
        URB_FLAG_ISOCH_START_ASAP
        );
    status = m_pDevice->SubmitRequest((PURB)&StartTransferUrb);
    ASSERT(USBD_SUCCESS(status));
        
    //
    //  The stream should be ready to go, we just need to
    //  attach transfers.
    //

    m_pStreamingResources = (PHAWK_STREAMING_RESOURCES) pResources;

    return S_OK;
}

void XHawkMediaObject::CloseEndpoint( void )
{
    ASSERT(m_pStreamingResources);
    //
    //  Build and submit the close URB for the Isoch endpoint
    //

    USB_BUILD_ISOCH_CLOSE_ENDPOINT(
            &m_pStreamingResources->CloseUrb,
            m_pStreamingResources->EndpointHandle,
            (PURB_COMPLETE_PROC)&XHawkMediaObject::CloseEndpointComplete,
            (PVOID)this
            );
    m_pStreamingResources->EndpointHandle = NULL;
    m_pDevice->SubmitRequest((PURB)&m_pStreamingResources->CloseUrb);
}

void XHawkMediaObject::CloseEndpointComplete (PURB pUrb, XHawkMediaObject *pThis)
/*++
    Routine Description:
        This is the call back when an isoch endpoint is closed.
        This could happen if a close is pending, or if 
        a remove is pending, or both.

--*/
{
    ASSERT(USBD_SUCCESS(pUrb->Header.Status));

    //
    //  Deal with the remove pending case.
    //

    if(pThis->m_fRemovePending)
    {
        pThis->m_pDevice->SetExtension(NULL);
        pThis->m_pDevice->RemoveComplete();
        pThis->m_pDevice = NULL;
        pThis->m_fRemovePending = FALSE;
    }

    //
    //  If this is the close pending case,
    //  then there is an event that we need to set
    //
    if(pThis->m_fClosePending)
    {
        //
        //  We have to clear the m_fClosePending
        //  and m_fOpened flags while still at
        //  DPC for synchronization purposes.
        //

        pThis->m_fClosePending = FALSE;
        pThis->m_fOpened = FALSE;

        //
        //  Signal the close event so that
        //  FreeStreamingResources can continue
        //  to clean up.
        //

        KeSetEvent(
            &pThis->m_pStreamingResources->CloseEvent,
            IO_NO_INCREMENT,
            FALSE
            );
    }
}

void XHawkMediaObject::FreeStreamingResources (void)
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();

    if(m_fConnected)
    {

        //
        //  Mark that this object is pending close
        //  and initialize the close event.
        //

        m_fClosePending = TRUE;
        KeInitializeEvent(
            &m_pStreamingResources->CloseEvent,
            SynchronizationEvent,
            FALSE);

        //
        //  If it is already pending remove
        //  then we can just wait for
        //  the endpoint to close, other
        //  wise we must initiate closing
        //   it.
        //

        if(!m_fRemovePending)
        {
            CloseEndpoint();
        }

        //
        //  Lower Irql and let the endpoint close.
        //

        KeLowerIrql(oldIrql);
        
        //
        // The endpoint close is under way,
        // wait fpr it to complete.
        //

        KeWaitForSingleObject (
            &m_pStreamingResources->CloseEvent,
            Executive,
            KernelMode,
            FALSE,
            NULL
            );

    } else
    {
        m_fOpened = FALSE;
        KeLowerIrql(oldIrql);
    }

    //
    //  Now we can cleanup what is left of the streaming resources
    //
    AbortMediaPackets(&m_pStreamingResources->PendingComplete);
    AbortMediaPackets(&m_pStreamingResources->PendingProgram);
    delete m_pStreamingResources;
    m_pStreamingResources = NULL;
}


void XHawkMediaObject::ProgramTransfer()
/*++
    Routine Description:
        This routine translates a portion of a packet into USB Isochronous
        Transfer Requests.

    Note: Sometimes this routine in called at PASSIVE_LEVEL and sometimes
    at DISPATCH_LEVEL.  This routine is not inherently reentrant so we raise
    the IRQL to DISPATCH_LEVEL all the time.
--*/
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();   
 
    while(m_pStreamingResources->ulOutstandingTransfers < HAWK_ISOCH_MAX_ATTACH_BUFFERS)
    {
        PHAWK_PACKET_CONTEXT pPacketContext = m_pStreamingResources->PendingProgram.GetHead();
        
        //
        //  If there are no more packet contexts pending programming, break the
        //  loop, we are done.
        //

        if(!pPacketContext) break;
        
        //
        //  If there are no bytes remaining, then this item was just added to the
        //  pending program list, so reset a programing context for this packet
        //

        if(0 == m_pStreamingResources->ulBytesRemaining)
        {
            //
            //  ulBytesComplete may start as non-zero if there was data
            //  in the packet prior to the calling ProcessOutput.
            //

            m_pStreamingResources->ulBytesRemaining = pPacketContext->mediaPacket.dwMaxSize;
        }

        //
        //  Calculate the maximum number of frames we can transfer
        //
        
        ULONG ulFrameCount = m_pStreamingResources->ulBytesRemaining/HAWK_BYTES_PER_USB_FRAME;
        if(ulFrameCount > HAWK_USB_MAX_FRAMES) ulFrameCount = HAWK_USB_MAX_FRAMES;
        m_pStreamingResources->IsochBufferDescriptor.FrameCount = ulFrameCount;
        
        //
        //  Update the transfer packet information
        //
        
        m_pStreamingResources->IsochBufferDescriptor.TransferBuffer = pPacketContext->mediaPacket.pvBuffer;
                
        //
        //  Update the Current Buffer Pos and the bytes remaining.
        //

        ULONG ulTransferBytes = HAWK_BYTES_PER_USB_FRAME * ulFrameCount;
        pPacketContext->mediaPacket.pvBuffer = (PVOID)(((PUCHAR)pPacketContext->mediaPacket.pvBuffer) + ulTransferBytes);
        m_pStreamingResources->ulBytesRemaining -= ulTransferBytes;

        //
        //  If there are no bytes remaining, move this packet context
        //  onto the pending complete queue

        if(0 == m_pStreamingResources->ulBytesRemaining)
        {
            m_pStreamingResources->PendingProgram.RemoveHead();
            m_pStreamingResources->PendingComplete.InsertTail(pPacketContext);
        }

        //
        //  Build and submit a USB request
        //
        
        URB_ISOCH_ATTACH_BUFFER Urb;
        USB_BUILD_ISOCH_ATTACH_BUFFER(
            &Urb,
            m_pStreamingResources->EndpointHandle,
            USBD_DELAY_INTERRUPT_0_MS,
            &m_pStreamingResources->IsochBufferDescriptor
            );
        
        m_pStreamingResources->ulOutstandingTransfers++;

        USBD_STATUS usbdStatus = m_pDevice->SubmitRequest((PURB)&Urb);
        ASSERT(USBD_SUCCESS(usbdStatus));
        
    }

    KeLowerIrql(oldIrql);
}

void XHawkMediaObject::TransferComplete(PUSBD_ISOCH_TRANSFER_STATUS Status, XHawkMediaObject *pThis)
/*++
    Routine Description:
        This is the completion routine whenever an isoch transfer completes.
        It continues to program the current media packet, releases the media
        packet when it is complete, and moves on to the next packet.
--*/
{

    PHAWK_STREAMING_RESOURCES pStreamingResources = pThis->m_pStreamingResources;

    //
    //  Diagnostics to determine data overrun and data underrun
    //
    #ifdef HAWK_OVERRUN_UNDERRUN_SPEW
    static int TotalCalls=0;
    static int Overrun=0;
    static int Underrun=0;
    TotalCalls++;
    if(HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE == pThis->m_pTypeSettings->ulHawkClassSpecificType)
    {
        for(ULONG i=0; i<Status->FrameCount;i++)
        {
            if(USBD_ISOCH_STATUS_DATA_OVERRUN==Status->PacketStatus[i].ConditionCode)
            {
                DbgPrint("Overrun:%dof%d\n", ++Overrun,TotalCalls);
            } else if(32!=Status->PacketStatus[i].BytesRead)
            {
                DbgPrint("Underrun:%dof%d\n", ++Underrun,TotalCalls);
            }
        }
    }
    #endif
                
    //
    //  Find the correct packet context.  It is the oldest context waiting
    //  for completion. Either the head of the PendingComplete Queue, or
    //  the head of PendingProgram queue.
    //
    PHAWK_PACKET_CONTEXT pPacketContext = pStreamingResources->PendingComplete.GetHead();
    if(NULL == pPacketContext)
    {
        pPacketContext = pStreamingResources->PendingProgram.GetHead();
        ASSERT(pPacketContext);
    }

    //
    //  increment the count of completed bytes.
    //
    pPacketContext->dwCompletedSize += Status->FrameCount*HAWK_BYTES_PER_USB_FRAME;

    //
    //  Check if this completion, completes the whole packet
    //

    ASSERT(pPacketContext->dwCompletedSize <= pPacketContext->mediaPacket.dwMaxSize);
    if(pPacketContext->dwCompletedSize == pPacketContext->mediaPacket.dwMaxSize)
    {
        //
        //  Remove the packet from the pending complete list
        //

        PHAWK_PACKET_CONTEXT pCompletedPacketContext = pStreamingResources->PendingComplete.RemoveHead();
        ASSERT(pCompletedPacketContext == pPacketContext);

        //
        //  Move the packet context to the free list
        //

        pStreamingResources->Free.InsertTail(pCompletedPacketContext);

        //
        //  Update the users status and signal the event if there is one.
        //
        XMOCompletePacket(&pCompletedPacketContext->mediaPacket,
                          pPacketContext->dwCompletedSize,
                          pThis->m_pfnCallback,
                          pThis->m_pvContext,
                          XMEDIAPACKET_STATUS_SUCCESS);
    }

    pStreamingResources->ulOutstandingTransfers--;

    //
    // Jog Program Transfer, as we just freed up a transfer
    //
    if(pThis->m_fConnected && pThis->m_fOpened && !pThis->m_fClosePending)
    {
        pThis->ProgramTransfer();
    }
}

void XHawkMediaObject::AbortMediaPackets(CHawkPacketQueue *pPacketQueue)
{
    PHAWK_PACKET_CONTEXT pPacketContext = pPacketQueue->RemoveHead();
    while(pPacketContext)
    {
        //
        //  Move the packet context to the free list
        //
        m_pStreamingResources->Free.InsertTail(pPacketContext);

        //
        //  Update the users status and signal the event if there is one.
        //

        XMOCompletePacket(&pPacketContext->mediaPacket,
                          0,
                          m_pfnCallback,
                          m_pvContext,
                          XMEDIAPACKET_STATUS_FLUSHED);

        //
        //  Get the next packet.
        //
        pPacketContext = pPacketQueue->RemoveHead();
    }
}


//-------------------------------------------------------------------------
// CHawkPacketQueue implementation
//-------------------------------------------------------------------------

void CHawkPacketQueue::InsertTail(PHAWK_PACKET_CONTEXT pPacketContext)
{
    pPacketContext->pNextPacket = NULL;
    if(m_pTail)
    {
        m_pTail->pNextPacket = pPacketContext;
    } else
    {
        m_pHead = pPacketContext;
    }
    m_pTail = pPacketContext;
}

PHAWK_PACKET_CONTEXT CHawkPacketQueue::RemoveHead()
{
    PHAWK_PACKET_CONTEXT pRetVal;
    if(m_pHead)
    {
         pRetVal = m_pHead;
         m_pHead = m_pHead->pNextPacket;
         if(!m_pHead) m_pTail = NULL;
    } else
    {
        pRetVal = NULL;
    }
    return pRetVal;
}
