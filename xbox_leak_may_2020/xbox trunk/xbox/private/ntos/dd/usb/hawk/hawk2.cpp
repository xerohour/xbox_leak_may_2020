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
#include "hawk2.h"

#include "xvocver.h"

#if DBG || DEBUG_LOG

    ULONG               HawkDebugLevel=2;     // Level of debug output

#endif

//------------------------------------------------------------------------------
//  Declaration of static tables
//------------------------------------------------------------------------------
XHawkMediaObject *XHawkMediaObject::sm_pMicrophones = NULL;
XHawkMediaObject *XHawkMediaObject::sm_pHeadphones = NULL;
USHORT            XHawkMediaObject::sm_AvailableHeadphoneResources = 0;
USHORT            XHawkMediaObject::sm_AvailableMicrophoneResources = 0;
PHAWK_STREAMING_RESOURCES XHawkMediaObject::sm_pFreeStreamingResources = NULL;
HAWK_SAMPLE_RATE_INFORMATION XHawkMediaObject::sm_HawkSampleRates[HAWK_SAMPLE_RATE_COUNT] = 
{
    { 8000, 16, 0},     // 8.000 kHz
    {11025, 22, 40},    //11.025 kHz
    {16000, 32, 0},     //16.000 kHz
    {22050, 44, 20},    //22.050 kHz
    {24000, 48, 0}      //24.000 kHz
};
PUCHAR XHawkMediaObject::sm_pRateIndices = NULL;
PUCHAR XHawkMediaObject::sm_pRateIndexRefCounts = NULL;

//------------------------------------------------------------------------------
//  Declare Hawk types and class.
//------------------------------------------------------------------------------
DECLARE_XPP_TYPE(XDEVICE_TYPE_VOICE_MICROPHONE)
DECLARE_XPP_TYPE(XDEVICE_TYPE_VOICE_HEADPHONE)
USB_DEVICE_TYPE_TABLE_BEGIN(Hawk)
USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_VOICE_MICROPHONE),
USB_DEVICE_TYPE_TABLE_ENTRY(XDEVICE_TYPE_VOICE_HEADPHONE)
USB_DEVICE_TYPE_TABLE_END()
USB_CLASS_DRIVER_DECLARATION(Hawk, USB_DEVICE_CLASS_AUDIO, 0xFF, 0xFF)
USB_CLASS_DRIVER_DECLARATION_DUPLICATE(Hawk, 1, XBOX_DEVICE_CLASS_AUDIO_DEVICE, 0xFF, 0xFF)

#pragma data_seg(".XPP$ClassHawk")
USB_CLASS_DECLARATION_POINTER(Hawk)
USB_CLASS_DECLARATION_POINTER_DUPLICATE(Hawk, 1)
#pragma data_seg(".XPP$Data")

USB_RESOURCE_REQUIREMENTS HAWK_gResourceRequirements = 
    {USB_CONNECTOR_TYPE_HIGH_POWER, 0, 2, 1, 0, 0, 0, 0, 2, HAWK_ISOCH_MAX_ATTACH_BUFFERS};

//------------------------------------------------------------------------------
// USB Class Driver Interface
//------------------------------------------------------------------------------
#pragma code_seg(".XPPCINIT")
EXTERNUSB VOID
HawkInit (IUsbInit *pUsbInit)
{
    DBGPRINT(3, ("enter: HawkInit\n"));
    ULONG MicrophoneCount;
    ULONG HeadphoneCount; 
    
    //sentry against double init, required because we have two driver
    //table entries (look in the tables above) - this is still in there to support some older firmware.
    #pragma BUGBUG("Supporting USB_DEVICE_CLASS_AUDIO for old Hawk FW, we don't support any USB audio, so this must go at some point.")
    static BOOL fInitialized = FALSE;
    if(fInitialized) return;
    fInitialized = TRUE;

    
    if(pUsbInit->UseDefaultCount())
    {
        MicrophoneCount = HAWK_DEFAULT_MAX_OPENED;
        HeadphoneCount = HAWK_DEFAULT_MAX_OPENED;
    } else
    {
        MicrophoneCount = pUsbInit->GetMaxDeviceTypeCount(XDEVICE_TYPE_VOICE_MICROPHONE);
        HeadphoneCount = pUsbInit->GetMaxDeviceTypeCount(XDEVICE_TYPE_VOICE_HEADPHONE);
        ULONG ulMaxPorts = XGetPortCount();
        RIP_ON_NOT_TRUE_WITH_MESSAGE(MicrophoneCount <= ulMaxPorts, "XInitDevices: requested more XDEVICE_TYPE_VOICE_MICROPHONE than available ports.");
        if(MicrophoneCount > ulMaxPorts) MicrophoneCount = ulMaxPorts;
        RIP_ON_NOT_TRUE_WITH_MESSAGE(HeadphoneCount <= ulMaxPorts, "XInitDevices: requested more XDEVICE_TYPE_VOICE_HEADPHONE than available ports.");
        if(HeadphoneCount > ulMaxPorts) HeadphoneCount = ulMaxPorts;
    }
    //
    //  Register for microphones and headphone
    //
    HAWK_gResourceRequirements.MaxDevices = (UCHAR)MicrophoneCount;
    pUsbInit->RegisterResources(&HAWK_gResourceRequirements);
    HAWK_gResourceRequirements.MaxDevices = (UCHAR)HeadphoneCount;
    pUsbInit->RegisterResources(&HAWK_gResourceRequirements);
    
    XHawkMediaObject::InitializeClass(MicrophoneCount, HeadphoneCount);

    DBGPRINT(3, ("exit:  HawkInit\n"));    
    return;
}
#pragma code_seg(".XPPCODE")

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
        pHawkMediaObject->m_fMicrophone = TRUE;
    }
    else {
        pEndpointDescriptor = Device->GetEndpointDescriptor(
                                USB_ENDPOINT_TYPE_ISOCHRONOUS,
                                FALSE,
                                0);
        if(pEndpointDescriptor)
        {
            pHawkMediaObject = &XHawkMediaObject::sm_pHeadphones[dwPort];
            pHawkMediaObject->m_fMicrophone = FALSE;
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

    DBGPRINT(3,("IsochMaxPacket = %x, bEndpointAddress = 0x%02x\n, Interface = %d\n", 
                     (ULONG)pEndpointDescriptor->wMaxPacketSize,
                     (ULONG)pEndpointDescriptor->bEndpointAddress,
                     Device->GetInterfaceNumber()
                     ));

    //
    // Set the class specific type so it can be enumerated
    //
    Device->SetClassSpecificType(
                pHawkMediaObject->m_fMicrophone ? HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE : HAWK_HEADPHONE_CLASS_SPECIFIC_TYPE
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
    //  of being closed.)
    //

    //
    //  Mark the media object remove pending,
    //
    pHawkMediaObject->m_fRemovePending = TRUE;

    //
    //  If the device is ready, we should close the endpoints
    //
    if(pHawkMediaObject->m_fReady)
    {
        //
        //  If the endpoint is not in the process
        //  of being closed, then start that process
        //
        pHawkMediaObject->CloseEndpoint();
    } else
    //
    //  The media object wasn't opened, so we can
    //  call remove complete
    //
    {
        //
        //  If a close is pending then we 
        //  cannot call RemoveComplete, but
        //  when the close completes it will
        //  see the m_fRemovePending flag.
        //
        if(!pHawkMediaObject->m_fClosePending)
        {
            pHawkMediaObject->m_pDevice = NULL;
            Device->SetExtension(NULL);
            Device->RemoveComplete();
            pHawkMediaObject->m_fRemovePending = FALSE;
        }
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
// XHawkMediaObject
//------------------------------------------------------------------------------
#pragma code_seg(".XPPCINIT")
void XHawkMediaObject::InitializeClass(ULONG ulMicrophoneCount, ULONG ulHeadphoneCount)
{
    //
    //  Allocate a microphone and a headphone
    //  instance for each of the ports up front.
    //
    ULONG ulPortCount = XGetPortCount();
    ULONG ulTotalStreamingResources = ulHeadphoneCount + ulMicrophoneCount;
    PHAWK_STREAMING_RESOURCES pStreamingResourceArray;
    
    //
    //  Handle insane case, where the user requested no microphone or headphones, but
    //  some how we are here anyway.
    //
    if(ulTotalStreamingResources)
        pStreamingResourceArray = new HAWK_STREAMING_RESOURCES[ulTotalStreamingResources];

    
    sm_pHeadphones = new XHawkMediaObject[ulPortCount];
    sm_pMicrophones = new XHawkMediaObject[ulPortCount];
    sm_pRateIndices = new UCHAR[ulPortCount];
    sm_pRateIndexRefCounts = new UCHAR[ulPortCount];
    
    RIP_ON_NOT_TRUE_WITH_MESSAGE(sm_pHeadphones, "XInitDevices failed due to insufficent memory.  There is no graceful recovery.");
    RIP_ON_NOT_TRUE_WITH_MESSAGE(sm_pMicrophones, "XInitDevices failed due to insufficent memory.  There is no graceful recovery.");
    RIP_ON_NOT_TRUE_WITH_MESSAGE(sm_pRateIndices, "XInitDevices failed due to insufficent memory.  There is no graceful recovery.");
    RIP_ON_NOT_TRUE_WITH_MESSAGE(sm_pRateIndexRefCounts, "XInitDevices failed due to insufficent memory.  There is no graceful recovery.");
    RIP_ON_NOT_TRUE_WITH_MESSAGE(pStreamingResourceArray, "XInitDevices failed due to insufficent memory.  There is no graceful recovery.");
    
    //
    //  Make a free list out of pStreamingResourceArray
    //
    sm_pFreeStreamingResources = NULL;
    for(ULONG i=0; i < ulTotalStreamingResources; i++)
    {
        pStreamingResourceArray[i].pNextFree = sm_pFreeStreamingResources;
        sm_pFreeStreamingResources = &pStreamingResourceArray[i];
    }
    //
    //  Initialize the number of available microphone and headphone resources
    //
    sm_AvailableMicrophoneResources = (USHORT)ulMicrophoneCount;
    sm_AvailableHeadphoneResources = (USHORT)ulHeadphoneCount;


    //
    //  Initially none of the rates are set.
    //
    RtlZeroMemory(sm_pRateIndexRefCounts, sizeof(UCHAR)*ulPortCount);
    
    //
    //  We could loop over them and morph them into
    //  microphones and headphones, but we might
    //  as well leave that until an AddDevice.
    //

}
#pragma code_seg(".XPPCODE")

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
    OUT PXMEDIAINFO pInfo
    )
{
    //
    //  If this assert fails the reference count is hosed
    //  or the caller called us after releasing the last
    //  reference.
    //
    ASSERT(m_pStreamingResources);
    
    //
    // align values to our block size. In gets CEILed, out gets FLOORed
    //
    if (m_fMicrophone) {
        pInfo->dwFlags =
	        XMO_STREAMF_FIXED_SAMPLE_SIZE |
            XMO_STREAMF_OUTPUT_ASYNC;
        pInfo->dwOutputSize = 
            m_pStreamingResources->BytesPerUSBFrame * HAWK_MIN_PACKET_FRAMES;
        pInfo->dwInputSize = 0;
    } else {
        pInfo->dwFlags =
	        XMO_STREAMF_FIXED_SAMPLE_SIZE |
            XMO_STREAMF_INPUT_ASYNC;
        pInfo->dwInputSize = 
            m_pStreamingResources->BytesPerUSBFrame * HAWK_MIN_PACKET_FRAMES;
        pInfo->dwOutputSize = 0;
    }
    
    //
    //  Look ahead is 0.
    //
    pInfo->dwMaxLookahead = 0;
    
    return S_OK;
}

STDMETHODIMP 
XHawkMediaObject::Flush (void)
{
    return S_OK;
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
    RIP_ON_NOT_TRUE("XHawkMediaObject::GetStatus", NULL!=pdwFlags);

    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    
    ASSERT(m_pStreamingResources);
    if( m_fReady && m_pStreamingResources->Free.GetHead())
    {
        if(m_fMicrophone)
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
    IN LPCXMEDIAPACKET pInputPacket,
    IN LPCXMEDIAPACKET pOutputPacket
    )
{
    HRESULT hr = S_OK;
    LPCXMEDIAPACKET pMediaPacket;
    
    if(m_fMicrophone)
    {
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", NULL==pInputPacket);
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", NULL!=pOutputPacket);
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", 
                         pOutputPacket->dwMaxSize > (DWORD)m_pStreamingResources->BytesPerUSBFrame * HAWK_MIN_PACKET_FRAMES);
        RIP_ON_NOT_TRUE("Hawk Microphone XMediaObject::Process", 
                         0 == pOutputPacket->dwMaxSize%HAWK_BYTES_PER_SAMPLE);
        pMediaPacket = pOutputPacket;
         
    } else 
    {
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", NULL!=pInputPacket);
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", NULL==pOutputPacket);
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", 
                         pInputPacket->dwMaxSize > (DWORD)m_pStreamingResources->BytesPerUSBFrame * HAWK_MIN_PACKET_FRAMES);
        RIP_ON_NOT_TRUE("Hawk Headphone XMediaObject::Process", 
                         0 == pInputPacket->dwMaxSize%HAWK_BYTES_PER_SAMPLE);
        pMediaPacket = pInputPacket;
    }

    // Raise IRQL after rips
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();
    if(m_fReady)
    {

        PHAWK_PACKET_CONTEXT pPacketContext = m_pStreamingResources->Free.RemoveHead();    
        if(pPacketContext)
        {

            //
            //  Initialize the packet
            //
            
            XMOAcceptPacket(pMediaPacket);

            //
            //  Copy the media packet into the hawk packet context.
            //  The structure is identical except hawk has one extra
            //  field, for queuing
            //
            
            RtlCopyMemory(&pPacketContext->MediaPacket, pMediaPacket, sizeof(XMEDIAPACKET));
                
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
XVoiceCreateMediaObjectInternal(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      IN LPWAVEFORMATEX pwfxFormat,
      IN PFNXMEDIAOBJECTCALLBACK    pfnCallback,
      IN PVOID                      pvContext,
      OUT XMediaObject **ppXmediaObject
      )
{
    HRESULT hr;
    XHawkMediaObject *pHawkMediaObject = NULL;

    hr = XVoiceCreateMediaObject(
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


XBOXAPI EXTERN_C HRESULT WINAPI
XVoiceCreateMediaObject(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      IN PWAVEFORMATEX  pwfxFormat  OPTIONAL,
      OUT XMediaObject **ppXmediaObject
      )
/*++
    Routine Description:
        This is the factory method for XHawkMediaObjects.
        It is an exported API for games.
--*/
{
    //
    //  We need the class Id to find the device.
    //
	
    KIRQL		     oldIrql;
    PNP_CLASS_ID     classId;
    HRESULT          hr = S_OK;
    XHawkMediaObject *pHawkMediaObject = NULL;
    UCHAR            ucRateIndex=2; //default rate is 16 kHz, at index 2 in the rate table.
    
    //
    //  Verify that XInitDevices has been called.
    //
    RIP_ON_NOT_TRUE_WITH_MESSAGE(XPP_XInitDevicesHasBeenCalled, "XVoiceCreateMediaObject: XInitDevices must be called first!");

    //
    //  Verify port
    //
    RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, dwPort<XGetPortCount());

    //
    //  Verify that a reasonable dwMaxAttachedPackets was passed.
    //
    RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, (dwMaxAttachedPackets > 1));

    //
    //  Use raised Irql to synchronize the allocation
    //  and opening of instances
    //

    oldIrql = KeRaiseIrqlToDpcLevel();

    //
    //  Claim an existing instance of XHawkMediaObject
    //
	
    if(XDEVICE_TYPE_VOICE_MICROPHONE == XppDeviceType)
	{
        if(0==XHawkMediaObject::sm_AvailableMicrophoneResources)
        {
            KeLowerIrql(oldIrql);
            RIP("XVoiceCreateMediaObject: attempt to exceed number of mircophones requested in XInitDevices.\n");
            return E_OUTOFMEMORY;
        }
        pHawkMediaObject = &XHawkMediaObject::sm_pMicrophones[dwPort];
    } else if(XDEVICE_TYPE_VOICE_HEADPHONE == XppDeviceType)
    {
        if(0==XHawkMediaObject::sm_AvailableHeadphoneResources)
        {
            KeLowerIrql(oldIrql);
            RIP("XVoiceCreateMediaObject: attempt to exceed number of headphones requested in XInitDevices.\n");
            return E_OUTOFMEMORY;
        }
        pHawkMediaObject = &XHawkMediaObject::sm_pHeadphones[dwPort];
    }
#if DBG    
    else {
        KeLowerIrql(oldIrql);
        RIP("XVoiceCreateMediaObject: XppDeviceType is not a valid type for XVoiceCreateMediaObject\n");
	}
#endif //DBG

    //
    //  Check the wave format
    //
    if(pwfxFormat)
    {
        //
        //  Verify that the setting are supported
        //
#if DBG
        KeLowerIrql(oldIrql);
        RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, pwfxFormat->wFormatTag==WAVE_FORMAT_PCM);
        RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, pwfxFormat->nChannels==1);
        RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, 2*pwfxFormat->nSamplesPerSec==pwfxFormat->nAvgBytesPerSec);
        RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, pwfxFormat->wBitsPerSample==16);
        RIP_ON_NOT_TRUE(XVoiceCreateMediaObject, pwfxFormat->nBlockAlign == 2);
        oldIrql = KeRaiseIrqlToDpcLevel();
#endif

        //
        //  First validate rate against allowed rates.
        //
        for(ucRateIndex=0; ucRateIndex < HAWK_SAMPLE_RATE_COUNT; ucRateIndex++)
        {
            if(pwfxFormat->nSamplesPerSec == (DWORD)XHawkMediaObject::sm_HawkSampleRates[ucRateIndex].nSamplePerSecond)
            {
                break;
            }
        }
#if DBG
        if(HAWK_SAMPLE_RATE_COUNT == ucRateIndex)
        {
            KeLowerIrql(oldIrql);
            RIP("XVoiceCreateMediaObject: pWaveFormatEx->nSamplesPerSec has illegal value");
        }
#endif //DBG
    }

    if( pHawkMediaObject->m_fOpened )
    {
        pHawkMediaObject = NULL;
        hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);
    } else if (pHawkMediaObject->m_fConnected)
    {
        //
        //  Set the sample rate
        //
        if(XHawkMediaObject::sm_pRateIndexRefCounts[dwPort])
        //
        //  If the reference count is non-zero, then the
        //  other half of this device has set the rate
        //  (microphone and headphone must have the same rate)
        //
        {
            XHawkMediaObject::sm_pRateIndexRefCounts[dwPort]++;
            //
            //  Verify that the rates match
            //
            if(XHawkMediaObject::sm_pRateIndices[dwPort]!=ucRateIndex)
            {
                // Be kinder here, return an error, I could vaguely see this slipping through testing.
                KeLowerIrql(oldIrql);
                RIP("XVoiceCreateMediaObject: Attempt to open microphone and headphone of same device with different sampling rates.");
                return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
            }
        } else
        //
        //  The rate hasn't been set yet, so set it
        //
        {
            XHawkMediaObject::sm_pRateIndexRefCounts[dwPort] = 1;
            XHawkMediaObject::sm_pRateIndices[dwPort]=ucRateIndex;
            hr = pHawkMediaObject->SetSampleRate(ucRateIndex, oldIrql);
			hr = pHawkMediaObject->SetAGC(HAWK_AGC_ON, oldIrql);
        }

        //
        //  Allocate the streaming resources.
        //
        if(SUCCEEDED(hr))
        {
            hr = pHawkMediaObject->AllocateStreamingResources(dwMaxAttachedPackets, ucRateIndex);
        }
        if(SUCCEEDED(hr))
        {
            pHawkMediaObject->m_fOpened = TRUE;
            pHawkMediaObject->m_fReady = TRUE;
            pHawkMediaObject->m_lRefCount = 1;
        } else
        {
           pHawkMediaObject = NULL;
           XHawkMediaObject::sm_pRateIndexRefCounts[dwPort]--;
        }
    } else
    {
        pHawkMediaObject = NULL;
        hr = HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }

    // Record that we opened a device
    if(pHawkMediaObject)
    {
        if(pHawkMediaObject->m_fMicrophone) XHawkMediaObject::sm_AvailableMicrophoneResources--;
        else XHawkMediaObject::sm_AvailableHeadphoneResources--;
    }

    //
    //  Restore Irql since the stuff that needs
    //  to be synchronous is done.
    //
    KeLowerIrql(oldIrql);
    
    *ppXmediaObject = pHawkMediaObject;	
    return hr;
}
            
HRESULT XHawkMediaObject::SetSampleRate(UCHAR ucRateIndex, KIRQL BaseIrql)
/*++
    Routine Description:
        Sends a command to hawk to change the sample rate.  This routine opens the
        default pipe. Sends a vendor specific SetFeature to change the rate.  The closes
        the default pipe.  The routine is synchronous, which means it must drop to below
        DISPATCH_LEVEL.
    Parameters:
        ucRateIndex - rate index to send to the device.
        BaseIrql - the lowest irql we are permitted to drop to.
    Comments:
        Must be called at DPC level, BaseIrql must be lower than DPC.
--*/
{
    USBD_STATUS usbdStatus;
    
    //
    //  Open the default endpoints
    //
    URB Urb;
    USB_BUILD_OPEN_DEFAULT_ENDPOINT(&Urb.OpenEndpoint);
    usbdStatus = m_pDevice->SubmitRequest(&Urb);
    if(USBD_ERROR(usbdStatus))
    {
        return HRESULT_FROM_WIN32(IUsbDevice::Win32FromUsbdStatus(usbdStatus));
    }

    //
    //  While still at DPC, set the m_fClosePending flag.  This will prevent HawkRemoveDevice
    //  from calling RemoveComplete on us.  We take on the burden of checking for m_fRemovePending
    //  when clearing this flag.
    //

    m_fClosePending = TRUE;   
    //
    //  Drop to BaseIrql so we can start doing the asynchronous parts synchronously.
    //

    ASSERT(BaseIrql < DISPATCH_LEVEL);
    KeLowerIrql(BaseIrql);

    //
    //  Send the SET_FEATURE(SampleRate) command.
    //

    USB_BUILD_CONTROL_TRANSFER(
        &Urb.ControlTransfer,
        NULL,   //Use the default endpoint
        NULL,   //No transfer buffer
        0,      
        0,
        NULL,   //Do this transfer synchronously
        0,      
        FALSE,  //Short Transfer NOT OK
        USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE | USB_HOST_TO_DEVICE,
        USB_REQUEST_SET_FEATURE,
        (USHORT) 0x0100 | ucRateIndex, // DD: ucRateIndex, //Rate Index to set, see Talon Specification.
        HAWK_VENDOR_FEATURE_SAMPLE_RATE,
        0); //Transfer length is zero

    usbdStatus = m_pDevice->SubmitRequest(&Urb);

    //
    //  Close the default endpoint
    //
    USB_BUILD_CLOSE_DEFAULT_ENDPOINT(&Urb.CloseEndpoint, NULL, NULL);
    m_pDevice->SubmitRequest(&Urb);

    //
    //  Now all the synchronous stuff is done we can
    //  go back up to DISPATCH_LEVEL
    //

    KeRaiseIrqlToDpcLevel();

    //
    //  We are not really open yet, nor are we pending a close.
    //

    m_fOpened = FALSE; 
    m_fClosePending = FALSE; 

    //
    //  Check the remove pending flag, and complete the remove
    //  it one was pending.
    //  Also fail this request with ERROR_DEVICE_NOT_CONNECTED.
    //
    if(m_fRemovePending)
    {
        m_pDevice->SetExtension(NULL);
        m_pDevice->RemoveComplete();
        m_pDevice = NULL;
        m_fRemovePending = FALSE;
        return HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }
    
    //
    //  If we are supporting fixed rate devices
    //  tolerate a failure, if the
    //  requested rate was 16 ksamples/second
    #ifdef HAWK_SUPPORT_FIXED_RATE
    if(ucRateIndex==2)
    {
        usbdStatus = USBD_STATUS_SUCCESS;
    }
    #endif //HAWK_SUPPORT_FIXED_RATE

    //
    //  Return the status of the request to set the data rate.  
    //

    return HRESULT_FROM_WIN32(IUsbDevice::Win32FromUsbdStatus(usbdStatus));
}

HRESULT XHawkMediaObject::SetAGC(UCHAR ucAGC, KIRQL BaseIrql)
/*++
    Routine Description:
        Sends a command to hawk to change the sample rate.  This routine opens the
        default pipe. Sends a vendor specific SetFeature to change the rate.  The closes
        the default pipe.  The routine is synchronous, which means it must drop to below
        DISPATCH_LEVEL.
    Parameters:
        ucAGC - rate index to send to the device.
        BaseIrql - the lowest irql we are permitted to drop to.
    Comments:
        Must be called at DPC level, BaseIrql must be lower than DPC.
--*/
{
    USBD_STATUS usbdStatus;
    
	//
    //  Open the default endpoints
    //
    URB Urb;
    USB_BUILD_OPEN_DEFAULT_ENDPOINT(&Urb.OpenEndpoint);
    usbdStatus = m_pDevice->SubmitRequest(&Urb);
    if(USBD_ERROR(usbdStatus))
    {
        return HRESULT_FROM_WIN32(IUsbDevice::Win32FromUsbdStatus(usbdStatus));
    }

    //
    //  While still at DPC, set the m_fClosePending flag.  This will prevent HawkRemoveDevice
    //  from calling RemoveComplete on us.  We take on the burden of checking for m_fRemovePending
    //  when clearing this flag.
    //

    m_fClosePending = TRUE;   
    //
    //  Drop to BaseIrql so we can start doing the asynchronous parts synchronously.
    //

    ASSERT(BaseIrql < DISPATCH_LEVEL);
    KeLowerIrql(BaseIrql);

    //
    //  Send the SET_FEATURE(SampleRate) command.
    //

    USB_BUILD_CONTROL_TRANSFER(
        &Urb.ControlTransfer,
        NULL,   //Use the default endpoint
        NULL,   //No transfer buffer
        0,      
        0,
        NULL,   //Do this transfer synchronously
        0,      
        FALSE,  //Short Transfer NOT OK
        USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE | USB_HOST_TO_DEVICE,
        USB_REQUEST_SET_FEATURE,
        (USHORT)ucAGC, //ON/OFF, see Talon Specification.
        (USHORT)HAWK_VENDOR_FEATURE_AGC, 
        0); //Transfer length is zero

    usbdStatus = m_pDevice->SubmitRequest(&Urb);

    //
    //  Close the default endpoint
    //
    USB_BUILD_CLOSE_DEFAULT_ENDPOINT(&Urb.CloseEndpoint, NULL, NULL);
    m_pDevice->SubmitRequest(&Urb);

    //
    //  Now all the synchronous stuff is done we can
    //  go back up to DISPATCH_LEVEL
    //

    KeRaiseIrqlToDpcLevel();

    //
    //  We are not really open yet, nor are we pending a close.
    //

    m_fOpened = FALSE; 
    m_fClosePending = FALSE; 

    //
    //  Check the remove pending flag, and complete the remove
    //  it one was pending.
    //  Also fail this request with ERROR_DEVICE_NOT_CONNECTED.
    //
    if(m_fRemovePending)
    {
        m_pDevice->SetExtension(NULL);
        m_pDevice->RemoveComplete();
        m_pDevice = NULL;
        m_fRemovePending = FALSE;
        return HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }
    
    //
    //  Return the status of the request to set the data rate.  
    //
    return HRESULT_FROM_WIN32(IUsbDevice::Win32FromUsbdStatus(usbdStatus));
}


HRESULT XHawkMediaObject::AllocateStreamingResources(ULONG ulMaxAttachedPackets, ULONG ulRateIndex)
/*++
    Routine Description:
        This is a helper function called by Factory Method,
        XVoiceCreateMediaObject, to allocate resources for
        providing streaming.
--*/
{
    ULONG i;
    
    //
    //  Allocate memory for Attached Packets
    //
    PHAWK_PACKET_CONTEXT pPacketContextArray  = new HAWK_PACKET_CONTEXT[ulMaxAttachedPackets];
    if(!pPacketContextArray) return E_OUTOFMEMORY;
    RtlZeroMemory(pPacketContextArray, sizeof(HAWK_PACKET_CONTEXT)*ulMaxAttachedPackets);

    //
    //  Grab a free streaming resources  (there has to be one
    //  at this point or there is a bug somewhere in this driver.)
    //
    PHAWK_STREAMING_RESOURCES pResources = sm_pFreeStreamingResources;
    ASSERT(pResources);
    sm_pFreeStreamingResources = pResources->pNextFree;
    RtlZeroMemory(pResources, sizeof(HAWK_STREAMING_RESOURCES));
    
    //
    // Populate the free list of packet contexts
    //
    pResources->m_pContextArray = pPacketContextArray;
    for(i=0; i < ulMaxAttachedPackets; i++)
    {
        pResources->Free.InsertTail(pPacketContextArray + i);
    }

    //
    //  Copy the sample rate parameters from the rate table into the
    //  pResources structure for quick access.
    //

    pResources->BytesPerUSBFrame = sm_HawkSampleRates[ulRateIndex].BytesPerUSBFrame;
    pResources->ExtraSampleInterval = sm_HawkSampleRates[ulRateIndex].ExtraSampleInterval;
    pResources->AvailableTransferContexts = HAWK_TRANSFER_CONTEXT_BOTH;
    //pResources->ProgrammedFrame = 0; //Not necessary since the memory was zeroed.
    pResources->TransferContext[0].pHawkMediaObject =
    pResources->TransferContext[1].pHawkMediaObject = this;
    pResources->TransferContext[0].ucContextNumber = HAWK_TRANSFER_CONTEXT_0;
    pResources->TransferContext[1].ucContextNumber = HAWK_TRANSFER_CONTEXT_1;

    //
    //  Open the isoch endpoint
    //

    URB_ISOCH_OPEN_ENDPOINT OpenUrb;
    USB_BUILD_ISOCH_OPEN_ENDPOINT(
                &OpenUrb,
                m_bEndpointAddress,
                m_wMaxPacket,
                0);
    USBD_STATUS status = m_pDevice->SubmitRequest((PURB)&OpenUrb);
    
    if(USBD_ERROR(status))
    {
        delete [] pPacketContextArray;
        pResources->pNextFree = sm_pFreeStreamingResources;
        sm_pFreeStreamingResources = pResources;
        return HRESULT_FROM_WIN32(IUsbDevice::Win32FromUsbdStatus(status));
    }
    pResources->EndpointHandle = OpenUrb.EndpointHandle;

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

    m_pStreamingResources = pResources;

    return S_OK;
}

void XHawkMediaObject::CloseEndpoint( void )
{
    ASSERT(m_pStreamingResources);
    ASSERT(m_pStreamingResources->EndpointHandle);
    
    //
    //  The device is not ready, if the endpoint is not open.
    //

    m_fReady = FALSE;
    
    if(m_fClosingEndpoint) return;
    m_fClosingEndpoint = TRUE;

    //
    //  Build and submit the close URB for the Isoch endpoint
    //

    USB_BUILD_ISOCH_CLOSE_ENDPOINT(
            &m_pStreamingResources->CloseUrb,
            m_pStreamingResources->EndpointHandle,
            (PURB_COMPLETE_PROC)&XHawkMediaObject::CloseEndpointComplete,
            (PVOID)this
            );
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
    DWORD dwPort;
    ASSERT(USBD_SUCCESS(pUrb->Header.Status));

    //
    //  Now mark the endpoint closed.
    //
    pThis->m_pStreamingResources->EndpointHandle = NULL;

    //
    //  Decrement the reference count on the rate, for
    //  which we need the port index.
    //

    if(pThis->m_fMicrophone)
    {
        dwPort = (DWORD)(pThis - XHawkMediaObject::sm_pMicrophones);
    } else
    {
        dwPort = (DWORD)(pThis - XHawkMediaObject::sm_pHeadphones);
    }
    XHawkMediaObject::sm_pRateIndexRefCounts[dwPort]--;
    
    
    //
    //  Abort outstanding packets.
    //
    pThis->AbortMediaPackets();

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
    pThis->m_fClosingEndpoint = FALSE;
}

void XHawkMediaObject::FreeStreamingResources (void)
{
    KIRQL oldIrql = KeRaiseIrqlToDpcLevel();

    if(m_fConnected && m_pStreamingResources->EndpointHandle)
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
        // wait for it to complete.
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
    
    if(m_fMicrophone) sm_AvailableMicrophoneResources++;
    else sm_AvailableHeadphoneResources++;

    //
    //  Now we can cleanup what is left of the streaming resources
    //
    delete [] m_pStreamingResources->m_pContextArray;
    m_pStreamingResources->pNextFree = sm_pFreeStreamingResources;
    sm_pFreeStreamingResources = m_pStreamingResources;
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
    USBD_ISOCH_BUFFER_DESCRIPTOR isochBufferDescriptor;
    PHAWK_TRANSFER_CONTEXT pTransferContext = NULL;
    int i;

    //
    //  Claim a context
    //
    if(TEST_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_0))
    {
        CLEAR_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_0);
        pTransferContext = &m_pStreamingResources->TransferContext[0];
    } else if(TEST_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_1))
    {
        CLEAR_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_1);
        pTransferContext = &m_pStreamingResources->TransferContext[1];
    }

    isochBufferDescriptor.TransferComplete = (PFNUSBD_ISOCH_TRANSFER_COMPLETE)XHawkMediaObject::TransferComplete;

    while(pTransferContext)
    {
        PHAWK_PACKET_CONTEXT pPacketContext = m_pStreamingResources->PendingProgram.GetHead();
        
        //
        //  If there are no more packet contexts pending programming, break the
        //  loop, we are done.
        //

        if(!pPacketContext)
        {
            SET_FLAG(m_pStreamingResources->AvailableTransferContexts, pTransferContext->ucContextNumber);
            break;
        }

        //
        //  Fill out the transfer context
        //

        pTransferContext->pPacketContext = pPacketContext;
        pTransferContext->pucTransferBuffer = (PUCHAR) pPacketContext->MediaPacket.pvBuffer;
        pTransferContext->ulTransferBytes = 0;
        pTransferContext->cFirstFrame = m_pStreamingResources->ProgrammedFrame;
        pTransferContext->fWrapBuffer = FALSE;
        pTransferContext->fLastTransfer = FALSE;

        //
        //  Walk through each frame, up to HAWK_USB_MAX_FRAMES,
        //

        for(i=0; i < HAWK_USB_MAX_FRAMES; i++)
        {   
            isochBufferDescriptor.Pattern[i] = m_pStreamingResources->BytesPerUSBFrame;
            if(
                m_pStreamingResources->ExtraSampleInterval && 
                (++m_pStreamingResources->ProgrammedFrame == m_pStreamingResources->ExtraSampleInterval)
            )
            {
                isochBufferDescriptor.Pattern[i] += 2;
                m_pStreamingResources->ProgrammedFrame=0;
            }
            pTransferContext->ulTransferBytes += isochBufferDescriptor.Pattern[i];
            
            if(pTransferContext->ulTransferBytes > pPacketContext->MediaPacket.dwMaxSize)
            {
                m_pStreamingResources->WrapBuffer.ulSecondPacketBytes = 
                        pTransferContext->ulTransferBytes - pPacketContext->MediaPacket.dwMaxSize;
                pTransferContext->ulTransferBytes = pPacketContext->MediaPacket.dwMaxSize;
                pTransferContext->fWrapBuffer = TRUE;
                //
                //  If this is for a headphones copy the output bytes
                //  for this packet.
                //
                if(!m_fMicrophone)
                {
                    RtlCopyMemory(
                        m_pStreamingResources->WrapBuffer.Buffer,
                        pPacketContext->MediaPacket.pvBuffer,
                        pTransferContext->ulTransferBytes);
                }
            }
            if(pTransferContext->ulTransferBytes == pPacketContext->MediaPacket.dwMaxSize)
            {
                pTransferContext->fLastTransfer = TRUE;
                i++;
                break;
            }
        }
        ASSERT(i==8 || pTransferContext->fLastTransfer);
        pPacketContext->MediaPacket.dwMaxSize -= pTransferContext->ulTransferBytes;
        isochBufferDescriptor.Context = pTransferContext;
        isochBufferDescriptor.FrameCount = i;
        isochBufferDescriptor.TransferBuffer = pPacketContext->MediaPacket.pvBuffer;
        pPacketContext->MediaPacket.pvBuffer = ((PUCHAR)pPacketContext->MediaPacket.pvBuffer) + pTransferContext->ulTransferBytes;

        //
        //  If we are done with the current packet, then pop it off the pending program queue.
        //
        if(pTransferContext->fLastTransfer)
        {
            m_pStreamingResources->PendingProgram.RemoveHead();
        }

        //
        //  If the wrap buffer is in use than we need to get the next
        //  packet and start programming it in the beginning of the wrap buffer.
        //
        if(pTransferContext->fWrapBuffer)
        {
            DBGPRINT(1,("W"));
            PHAWK_WRAP_BUFFER pWrapBuffer = &m_pStreamingResources->WrapBuffer;
            isochBufferDescriptor.TransferBuffer = pWrapBuffer->Buffer;
            pPacketContext = m_pStreamingResources->PendingProgram.GetHead();
            if(pPacketContext)
            {
                pWrapBuffer->pSecondPacket = pPacketContext;
                pWrapBuffer->pucTransferBuffer = (PUCHAR)pPacketContext->MediaPacket.pvBuffer;
                ASSERT(pWrapBuffer->ulSecondPacketBytes <= pPacketContext->MediaPacket.dwMaxSize);
                if(!m_fMicrophone)
                {
                    RtlCopyMemory(
                        pWrapBuffer->Buffer+pTransferContext->ulTransferBytes,
                        pWrapBuffer->pucTransferBuffer,
                        pWrapBuffer->ulSecondPacketBytes);
                }
                pPacketContext->MediaPacket.pvBuffer = pWrapBuffer->pucTransferBuffer + pWrapBuffer->ulSecondPacketBytes;
                pPacketContext->MediaPacket.dwMaxSize -= pWrapBuffer->ulSecondPacketBytes; 
            } else
            //
            //  No more packets pending
            //
            {
                pWrapBuffer->pSecondPacket = NULL;
                pWrapBuffer->pucTransferBuffer = NULL;
                if(!m_fMicrophone)
                {
                    RtlZeroMemory(
                        pWrapBuffer->Buffer+pTransferContext->ulTransferBytes,
                        pWrapBuffer->ulSecondPacketBytes);
                }
            }
        }

        //
        //  Build and submit a USB request
        //
        
        URB_ISOCH_ATTACH_BUFFER Urb;
        USB_BUILD_ISOCH_ATTACH_BUFFER(
            &Urb,
            m_pStreamingResources->EndpointHandle,
            USBD_DELAY_INTERRUPT_0_MS,
            &isochBufferDescriptor
            );
        
        USBD_STATUS usbdStatus = m_pDevice->SubmitRequest((PURB)&Urb);
        ASSERT(USBD_SUCCESS(usbdStatus));
        
        //
        //  Try to claim another context
        //  
        if(TEST_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_0))
        {
            CLEAR_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_0);
            pTransferContext = &m_pStreamingResources->TransferContext[0];
        } else if(TEST_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_1))
        {
            CLEAR_FLAG(m_pStreamingResources->AvailableTransferContexts, HAWK_TRANSFER_CONTEXT_1);
            pTransferContext = &m_pStreamingResources->TransferContext[1];
        } else
        {   //
            //  no more contexts
            //
            pTransferContext = NULL;
        }
    }

    KeLowerIrql(oldIrql);
}


void XHawkMediaObject::TransferComplete(PUSBD_ISOCH_TRANSFER_STATUS Status,  PVOID pvContext)
/*++
    Routine Description:
        This is the completion routine whenever an isoch transfer completes.
        It continues to program the current media packet, releases the media
        packet when it is complete, and moves on to the next packet.
--*/
{
    PHAWK_TRANSFER_CONTEXT pTransferContext = (PHAWK_TRANSFER_CONTEXT)pvContext;
    XHawkMediaObject *pThis = pTransferContext->pHawkMediaObject;
    PHAWK_STREAMING_RESOURCES pStreamingResources = pThis->m_pStreamingResources;
    PHAWK_PACKET_CONTEXT pPacketContext = pTransferContext->pPacketContext;
    CHAR extraSampleClock = pThis->m_pStreamingResources->ExtraSampleInterval;
    if(extraSampleClock) extraSampleClock--;
   
    //
    //  Post processing of data is only useful\needed for microphones.
    //
    if(pThis->m_fMicrophone)
    {
        ULONG bytesThisFrame;
        PUSHORT pBufferPosition;
        if(pTransferContext->fWrapBuffer)
        {
            pBufferPosition = (PUSHORT)pStreamingResources->WrapBuffer.Buffer;
        } else
        {
            pBufferPosition = (PUSHORT)pTransferContext->pucTransferBuffer;
        }
        //
        //  First handle overflow and underflow conditions
        //
        
        for(ULONG ulFrameIndex = 0; ulFrameIndex < Status->FrameCount; ulFrameIndex++)
        {
            bytesThisFrame = pStreamingResources->BytesPerUSBFrame;
            if(
                extraSampleClock &&
                (ulFrameIndex + pTransferContext->cFirstFrame == (ULONG)extraSampleClock)
            )
            {
                bytesThisFrame += 2;
            }
            //
            //  Fill in short frames with data copied from the previous samples(s)
            //
            if(USBD_ISOCH_STATUS_DATA_UNDERRUN == Status->PacketStatus[ulFrameIndex].ConditionCode)
            {
                //Copy bytes from the previous sample
                if(Status->PacketStatus[ulFrameIndex].BytesRead)
                {
                    #if DBG
                    if(0!=Status->PacketStatus[ulFrameIndex].BytesRead%2)
                    {
                        DBGPRINT(1,("Hardware violated sample boundary"));
                    }
                    #endif
                    ULONG samplesToDuplicate = (bytesThisFrame - Status->PacketStatus[ulFrameIndex].BytesRead) >> 1;
                    pBufferPosition = (PUSHORT)(((ULONG_PTR)pBufferPosition) + Status->PacketStatus[ulFrameIndex].BytesRead-2);
                    USHORT sample = *pBufferPosition++;
                    while(samplesToDuplicate--)
                    {
                        *pBufferPosition++ = sample;
                    }
                } else
                //
                //  This handles devices that mute by sending zero length data packets.
                //
                {
                    RtlZeroMemory(pBufferPosition,bytesThisFrame);
                    pBufferPosition  = (PUSHORT)((ULONG_PTR)pBufferPosition + bytesThisFrame); 
                }
            } else
            {
                pBufferPosition  = (PUSHORT)((ULONG_PTR)pBufferPosition + bytesThisFrame); 
            }
            //
            //  Check for data overrun to try to adapt to 
            //  small clock differences
            //
            if(
                extraSampleClock &&
                (USBD_ISOCH_STATUS_DATA_OVERRUN == Status->PacketStatus[ulFrameIndex].ConditionCode)
            )
            {
                CHAR clockDifference = extraSampleClock - (pTransferContext->cFirstFrame+(CHAR)ulFrameIndex);
                pStreamingResources->ProgrammedFrame += clockDifference;
            }
        }
        //
        //  If the wrap buffer was in use, then we need to copy the bytes back to their
        //  original buffer
        //
        if(pTransferContext->fWrapBuffer)
        {
            //
            //  First copy the part for the first packet
            //
            RtlCopyMemory(
                pTransferContext->pucTransferBuffer,
                pStreamingResources->WrapBuffer.Buffer, 
                pTransferContext->ulTransferBytes
                );
            //
            //  Then copy the part for the second packet
            //
            if(pStreamingResources->WrapBuffer.pSecondPacket)
            {
                RtlCopyMemory(
                    pStreamingResources->WrapBuffer.pucTransferBuffer,
                    pStreamingResources->WrapBuffer.Buffer + pTransferContext->ulTransferBytes,
                    pStreamingResources->WrapBuffer.ulSecondPacketBytes
                    );
            }
        }
    } else
    {
        #if DBG
        for(ULONG ulFrameIndex = 0; ulFrameIndex < Status->FrameCount; ulFrameIndex++)
        {
            if(Status->PacketStatus[ulFrameIndex].ConditionCode!=USBD_STATUS_SUCCESS)
            {
                DBGPRINT(1,("Failed Write: Frame=%d, CC=0x%0.1x, BytesWritten=0x%0.3x", 
                        ulFrameIndex,
                        (DWORD)Status->PacketStatus[ulFrameIndex].ConditionCode,
                        (DWORD)Status->PacketStatus[ulFrameIndex].BytesRead
                        ));
            }
        }
        #endif
    }
    
    //
    //  Post processing of data is complete, update completed size
    // 
    
    if(pPacketContext->MediaPacket.pdwCompletedSize)
        *pPacketContext->MediaPacket.pdwCompletedSize += pTransferContext->ulTransferBytes;
    if( pTransferContext->fWrapBuffer&&
        pStreamingResources->WrapBuffer.pSecondPacket&&
        pStreamingResources->WrapBuffer.pSecondPacket->MediaPacket.pdwCompletedSize
    )
    {
        *pStreamingResources->WrapBuffer.pSecondPacket->MediaPacket.pdwCompletedSize += 
            pStreamingResources->WrapBuffer.ulSecondPacketBytes;
    }
    //
    //  Mark the pTransferContext as unused
    //
    BOOL fLastTransfer = pTransferContext->fLastTransfer;
    SET_FLAG(
        pStreamingResources->AvailableTransferContexts,
        pTransferContext->ucContextNumber
        );

    //
    //  Complete packet if necessary
    //
    if(fLastTransfer)
    {
        //
        //  Cache the media packet info on the stack
        //  so we can free the packet context
        //  before calling XMOCompletePacket.
        //
        XMEDIAPACKET mediaPacket;
        DWORD       dwCompletedSize = 0;
        RtlCopyMemory(&mediaPacket, &pPacketContext->MediaPacket, sizeof(XMEDIAPACKET));
        if(mediaPacket.pdwCompletedSize)
            dwCompletedSize = *mediaPacket.pdwCompletedSize;
        

        //
        //  Place the packet back on the free list  (do this before
        //  calling XMOCompletePacket) so that a packet context
        //  is available during the callback (if a callaback is
        //  used) to program the next packet.
        //
        pStreamingResources->Free.InsertTail(pPacketContext);

        XMOCompletePacket(&mediaPacket,
                          dwCompletedSize, 
                          pThis->m_pfnCallback,
                          pThis->m_pvContext,
                          XMEDIAPACKET_STATUS_SUCCESS);

    }

    //
    //  Jog the program loop (if the device is still opened and ready).
    //
    if(pThis->m_fReady)
    {
        pThis->ProgramTransfer();
    }
}

void XHawkMediaObject::AbortMediaPackets()
{

    PHAWK_PACKET_CONTEXT pPacketContext = NULL;
    if(!TEST_FLAG(m_pStreamingResources->AvailableTransferContexts,HAWK_TRANSFER_CONTEXT_0))
    {
        //
        //  Complete the packet (it cannot be on the pending program list)
        //
        if(m_pStreamingResources->TransferContext[0].fLastTransfer)
        {
            pPacketContext = m_pStreamingResources->TransferContext[0].pPacketContext;
        }
    }
    if((NULL==pPacketContext )&& !TEST_FLAG(m_pStreamingResources->AvailableTransferContexts,HAWK_TRANSFER_CONTEXT_1))
    {
        //
        //  Complete the packet (it cannot be on the pending program list)
        //
        if(m_pStreamingResources->TransferContext[1].fLastTransfer)
        {
            pPacketContext = m_pStreamingResources->TransferContext[1].pPacketContext;
        }
    }
    if(NULL==pPacketContext)
    {
        pPacketContext = m_pStreamingResources->PendingProgram.RemoveHead();
    }

    while(pPacketContext)
    {
        //
        //  Cache the media packet info on the stack
        //  so we can free the packet context
        //  before calling XMOCompletePacket.
        //
        XMEDIAPACKET mediaPacket;
        RtlCopyMemory(&mediaPacket, &pPacketContext->MediaPacket, sizeof(XMEDIAPACKET));

        //
        //  Place the packet back on the free list  (do this before
        //  calling XMOCompletePacket) so that a packet context
        //  is available during the callback (if a callaback is
        //  used) to program the next packet.
        //
        m_pStreamingResources->Free.InsertTail(pPacketContext);

        XMOCompletePacket(&mediaPacket,
                          0, 
                          m_pfnCallback,
                          m_pvContext,
                          XMEDIAPACKET_STATUS_FLUSHED);

        //
        //  Get the next packet.
        //

        pPacketContext = m_pStreamingResources->PendingProgram.RemoveHead();
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
