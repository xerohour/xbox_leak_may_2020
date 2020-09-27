/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    hawk.H

Abstract:

    Header file for hawk driver

Environment:

    kernel mode

Revision History:

    06-28-2000 : started : georgioc

--*/

//*****************************************************************************
// I N C L U D E S
//*****************************************************************************

#define _XAPI_
#define _KERNEL32_
#define NODSOUND
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <usb.h>
#include <xapidrv.h>
#include <dsoundp.h>
#include <xdbg.h>

//
// BUGBUG   If we are using a standard USB reader for dev, set this define
//

#define STANDARD_USB_DEVICE 1

//*****************************************************************************
// D E F I N E S
//*****************************************************************************

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define SET_FLAG(Flags, Bit)    ((Flags) |= (Bit))
#define CLEAR_FLAG(Flags, Bit)  ((Flags) &= ~(Bit))
#define TEST_FLAG(Flags, Bit)   ((Flags) & (Bit))

#if DBG || DEBUG_LOG

    extern ULONG               HawkDebugLevel;     // Level of debug output

#endif

#if !DBG

#define DBGPRINT(level, _x_)

#else

#define DBGPRINT(level, _x_) do { \
    if (level <= HawkDebugLevel) { \
        KdPrint(("HAWK: ")); \
        KdPrint( _x_ ); \
    } \
} while (0)

#endif

//*****************************************************************************
// A few remainder macros shamelessly stolen from dsound.
//*****************************************************************************
#define QUOTE0(a)               #a
#define QUOTE1(a)               QUOTE0(a)
                                
//
// #pragma Reminders
//

#define MESSAGE(a)              message(__FILE__ "(" QUOTE1(__LINE__) "): " a)
#define TODO(a)                 MESSAGE("TODO: " a)
#define BUGBUG(a)               MESSAGE("BUGBUG: " a)
#define HACKHACK(a)             MESSAGE("HACKHACK: " a)

//*****************************************************************************
// Helpful defintions
//*****************************************************************************
#define HAWK_MICROPHONE_CLASS_SPECIFIC_TYPE 0
#define HAWK_HEADPHONE_CLASS_SPECIFIC_TYPE 1

//*****************************************************************************
//  The sample rate and size of the device are hardcoded by specification.
//  The sources file can override this for building against test hardware
//  that doesn't meet spec.
//*****************************************************************************
#ifndef HAWK_CONSTANTS_PREDEFINED
#define HAWK_MICROPHONE_SAMPLE_RATE 16 //In kilohertz
#define HAWK_MICROPHONE_BYTES_PER_SAMPLE 2
#define HAWK_HEADPHONE_SAMPLE_RATE 16 //In kilohertz
#define HAWK_HEADPHONE_BYTES_PER_SAMPLE 2
#endif

#pragma TODO("Performance tune HAWK_ISOCH_MAX_ATTACH_BUFFERS")
#define HAWK_ISOCH_MAX_ATTACH_BUFFERS 3
#define HAWK_BYTES_PER_USB_FRAME (HAWK_HEADPHONE_SAMPLE_RATE*HAWK_HEADPHONE_BYTES_PER_SAMPLE)
#define HAWK_USB_MAX_FRAMES 8 //This is a fundemental USB isoch thing.

#if (HAWK_MICROPHONE_SAMPLE_RATE!=HAWK_HEADPHONE_SAMPLE_RATE)
#error Microphone Sample Rate Must Equal Headphone Sample Rate
#endif
#if (HAWK_MICROPHONE_BYTES_PER_SAMPLE!=HAWK_HEADPHONE_BYTES_PER_SAMPLE)
#error Microphone Bytes Per Sample Must Equal Headphone Bytes Per Sample
#endif

    
typedef struct _HAWK_TYPE_SETTINGS
{
    ULONG         ulHawkClassSpecificType;
    
    // Pointer to waveformat to report
    PWAVEFORMATEX pWaveFormat;

    // Fixed parameters returned by GetInputInfo,
    // or GetOutputInfo
    DWORD dwFlags;
    DWORD cbSize;
    DWORD cbMaxLookahead;
    DWORD cbAlignment;

} HAWK_TYPE_SETTINGS, *PHAWK_TYPE_SETTINGS;


typedef struct _HAWK_PACKET_CONTEXT *PHAWK_PACKET_CONTEXT;
typedef struct _HAWK_PACKET_CONTEXT
{
    XMEDIAPACKET         mediaPacket;
    DWORD                dwCompletedSize;
    PHAWK_PACKET_CONTEXT pNextPacket;
} HAWK_PACKET_CONTEXT;

class CHawkPacketQueue
{
    public:

     void InsertTail(PHAWK_PACKET_CONTEXT pPacketContext);
     PHAWK_PACKET_CONTEXT RemoveHead();
     PHAWK_PACKET_CONTEXT GetHead () {return m_pHead;}
        
    private:

     PHAWK_PACKET_CONTEXT m_pHead;
     PHAWK_PACKET_CONTEXT m_pTail;
};


#define HAWK_BCF_SHORT_PACKET  1
#define HAWK_BCF_DISCONTINUITY 2

typedef struct _HAWK_STREAMING_RESOURCES
{
    PVOID EndpointHandle;

    //
    //  Packet Queues
    //

    CHawkPacketQueue PendingProgram;
    CHawkPacketQueue PendingComplete;
    CHawkPacketQueue Free;

    //
    //  Programming State
    //

    ULONG ulBytesRemaining; 
    ULONG ulOutstandingTransfers;

    //
    //  The IsochBufferDescriptor could be allocated
    //  on the stack in ProgramTransfer, but many
    //  of the arguments are set at open time (a.k.a.
    //  XcreateHawkMediaObject) so we just keep
    //  the structure around.
    //
    //  CloseEvent is used to synchronize
    //  closing the endpoint handle when the
    //  object reference count goes to zero.
    //  Since ProgramTransfer won't initiate
    //  any transfers after a close is initiated
    //  the memory can be shared with IsochBufferDescriptor.
    //

    union 
    {
        USBD_ISOCH_BUFFER_DESCRIPTOR IsochBufferDescriptor;
        KEVENT CloseEvent;
    };
    
    //
    //  The close urb is needed when it is time to close  
    //  the stream.
    //
    
    URB_ISOCH_CLOSE_ENDPOINT     CloseUrb;
    
} HAWK_STREAMING_RESOURCES, *PHAWK_STREAMING_RESOURCES;


EXTERNUSB VOID
HawkInit ();

EXTERNUSB VOID
HawkAddDevice (
    IN IUsbDevice *Device
    );

EXTERNUSB VOID
HawkRemoveDevice (
    IN IUsbDevice *Device
    );


//*****************************************************************************
// XHawkMediaObject
//*****************************************************************************

class XHawkMediaObject : public XMediaObject
{

    public:
        //
        //  Declaration of IUnknown members
        //  (minus QI which was nuked)
        
        STDMETHOD_(ULONG, AddRef) (void);

        STDMETHOD_(ULONG, Release) (void);

        //
        //  Declaration of XMediaObject members
        //
        STDMETHOD(GetInfo) (
            IN OUT LPXMEDIAINFO pInfo
            );
                
        STDMETHOD(Flush) (void);
        
        STDMETHOD(Discontinuity) (void);
        
        STDMETHOD(GetStatus) (
            OUT DWORD *pdwFlags
            );
        
        STDMETHOD(Process) (
            IN const XMEDIAPACKET *pInputPacket,
            IN const XMEDIAPACKET *pOutputPacket
            );
        
        XHawkMediaObject() :
            m_pTypeSettings(NULL),
            m_pDevice(NULL),
		    m_wMaxPacket(0),
            m_bEndpointAddress(0),
            m_fConnected(FALSE),
		    m_fOpened(FALSE),
		    m_fClosePending(FALSE),
            m_fRemovePending(FALSE),
            m_Reserved(0),
            m_lRefCount(0),
            m_pStreamingResources(NULL),
            m_pfnCallback(NULL),
            m_pvContext(NULL)
            {}

        static void InitializeClass(); //Initialize static members
	
		//
        //	Keeps track of the static list
		//	of objects
		//
		static XHawkMediaObject *sm_pMicrophones;	//Array of microphone instances
		static XHawkMediaObject *sm_pHeadphones;	//Array of headphone instances

    private:
		//
		//	Static tables that apply to all instances
		//	(or for all instances of one type.)s
		//

		static WAVEFORMATEX sm_MicrophoneWaveformat;
		static WAVEFORMATEX sm_HeadphoneWaveformat;

		static HAWK_TYPE_SETTINGS sm_MicrophoneSettings;
		static HAWK_TYPE_SETTINGS sm_HeadphoneSettings;
		
		//
		//	Information Detected During enumeration.
		//

		PHAWK_TYPE_SETTINGS m_pTypeSettings;
        IUsbDevice          *m_pDevice;
		WORD			    m_wMaxPacket;
        UCHAR			    m_bEndpointAddress;
		
		//
		//	Running Per Instance State Information
		//
        //  Notes on state flag.  Connecting
        //  and opening a device is synchronous.
        //  Closing a device is asynchronous, because
        //  the 
        
		UCHAR			   m_fConnected:1;
		UCHAR			   m_fOpened:1;
		UCHAR			   m_fClosePending:1;
        UCHAR			   m_fRemovePending:1;
		UCHAR			   m_Reserved:4;
        
        
        LONG			   m_lRefCount;

        //
        // callback values
        //

        PFNXMEDIAOBJECTCALLBACK    m_pfnCallback;
        PVOID                      m_pvContext;

	    //
		//	Streaming Resources (only needed for operation)
		//
		PHAWK_STREAMING_RESOURCES m_pStreamingResources;

        //
        //  Private help methods
        //
        HRESULT AllocateStreamingResources (ULONG ulMaxAttachedPackets);
        void FreeStreamingResources (void);
        void CloseEndpoint (void);
        static void CloseEndpointComplete (IN PURB, IN XHawkMediaObject *pThis);
        void ProgramTransfer(void);
        static void TransferComplete(IN PUSBD_ISOCH_TRANSFER_STATUS Status, IN XHawkMediaObject *pThis);
        void AbortMediaPackets(IN CHawkPacketQueue *pPacketQueue);

    //
    //  The XcreateHawkMediaObject factory method, and the
    //  USB interface methods, need to be friends.  They
    //  all would be members, but they must be C calleable.
    //
    friend HRESULT WINAPI
    XHawkCreateMediaObject(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      OUT LPWAVEFORMATEX pwfxFormat OPTIONAL,
      OUT XMediaObject **ppXmediaObject
      );
    

    friend HRESULT WINAPI
    XHawkCreateMediaObjectInternal(
          IN PXPP_DEVICE_TYPE XppDeviceType,
          IN DWORD dwPort,
          IN DWORD dwMaxAttachedPackets,
          OUT LPWAVEFORMATEX pwfxFormat OPTIONAL,
          PFNXMEDIAOBJECTCALLBACK    pfnCallback,
          PVOID                      pvContext,
          OUT XMediaObject **ppXmediaObject
          );

    friend VOID HawkInit ();

    friend VOID HawkAddDevice (
                    IN IUsbDevice *Device
                    );

    friend VOID HawkRemoveDevice (
                    IN IUsbDevice *Device
                    );
};



