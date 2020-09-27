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
#define NODSOUND  //so we can use the private version
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xboxp.h>
#include <usb.h>
#include <xapidrv.h>
#include <dsoundp.h>
#include <xdbg.h>

//*****************************************************************************
// Useful Macros
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
#define HAWK_HEADPHONE_CLASS_SPECIFIC_TYPE  1
#define HAWK_VENDOR_FEATURE_AGC             1
#define HAWK_VENDOR_FEATURE_SAMPLE_RATE     0
#define HAWK_BYTES_PER_SAMPLE               2
#define HAWK_USB_MAX_FRAMES                 8
#define HAWK_ISOCH_MAX_ATTACH_BUFFERS       3
#define HAWK_MIN_PACKET_FRAMES              10
#define HAWK_DEFAULT_MAX_OPENED             4
#define HAWK_MAX_SAMPLES_PER_USB_FRAME      24  //The highest support rate is 24 kHz
#define HAWK_MAX_BYTES_PER_USB_FRAME        (HAWK_MAX_SAMPLES_PER_USB_FRAME * HAWK_BYTES_PER_SAMPLE)
#define HAWK_WRAP_BUFFER_SIZE               (HAWK_USB_MAX_FRAMES * HAWK_MAX_BYTES_PER_USB_FRAME + 2)

#define HAWK_AGC_ON                         1
#define HAWK_AGC_OFF                        0
	
typedef struct _HAWK_STREAMING_RESOURCES *PHAWK_STREAMING_RESOURCES;
typedef struct _HAWK_PACKET_CONTEXT *PHAWK_PACKET_CONTEXT;
typedef struct _HAWK_TRANSFER_CONTEXT *PHAWK_TRANSFER_CONTEXT;
typedef struct _HAWK_WRAP_BUFFER *PHAWK_WRAP_BUFFER;
class XHawkMediaObject;

//*****************************************************************************
//  HAWK_PACKET_CONTEXT tracks a packet submitted via XMediaObject::Process  
//*****************************************************************************
typedef struct _HAWK_PACKET_CONTEXT
{
    XMEDIAPACKET         MediaPacket;
    PHAWK_PACKET_CONTEXT pNextPacket;
} HAWK_PACKET_CONTEXT;

//*****************************************************************************
//  HAWK_WRAP_BUFFER used to wrap 
//*****************************************************************************
typedef struct _HAWK_WRAP_BUFFER
{
    PHAWK_PACKET_CONTEXT pSecondPacket;
    PUCHAR               pucTransferBuffer;   //The beginning of the second buffer
    ULONG                ulSecondPacketBytes;
    UCHAR                Buffer[HAWK_WRAP_BUFFER_SIZE]; 
} HAWK_WRAP_BUFFER;

//*****************************************************************************
//  HAWK_TRANSFER_CONTEXT represents an oustanding Isoch Transfer
//*****************************************************************************
typedef struct _HAWK_TRANSFER_CONTEXT
{
    XHawkMediaObject           *pHawkMediaObject;    //The media object we are acting for.
    PHAWK_PACKET_CONTEXT        pPacketContext;      //The context we are tracking
    PUCHAR                      pucTransferBuffer;   //The beginning of this transfer
    ULONG                       ulTransferBytes;     //Bytes programmed in this transfer
    BOOLEAN                     fWrapBuffer;         //If TRUE, the wrap buffer was used.
    CHAR                        cFirstFrame;         //The first frame programmed
    UCHAR                       ucContextNumber;     //Number of this transfer frame.
    BOOLEAN                     fLastTransfer;       //Last Transfer of a Packet
} HAWK_TRANSFER_CONTEXT;

//*****************************************************************************
//  HAWK_SAMPLE_RATE_INFORMATION 
//*****************************************************************************
typedef struct _HAWK_SAMPLE_RATE_INFORMATION
{
    USHORT nSamplePerSecond;
    UCHAR  BytesPerUSBFrame;
    UCHAR  ExtraSampleInterval;
} HAWK_SAMPLE_RATE_INFORMATION, *PHAWK_SAMPLE_RATE_INFORMATION;
#define HAWK_SAMPLE_RATE_COUNT 5

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

#define HAWK_TRANSFER_CONTEXT_0 1
#define HAWK_TRANSFER_CONTEXT_1 2
#define HAWK_TRANSFER_CONTEXT_BOTH 3

typedef struct _HAWK_STREAMING_RESOURCES *PHAWK_STREAMING_RESOURCES;
typedef struct _HAWK_STREAMING_RESOURCES
{
    
    union
    {
        PHAWK_PACKET_CONTEXT m_pContextArray; //used to track the original allocation.
                                              //of packet contexts, so we can free them.

        PHAWK_STREAMING_RESOURCES pNextFree;  //only used while the streaming resources is on the 
                                              //free list.
    };      
    //
    //  Packet Queues
    //

    CHawkPacketQueue PendingProgram;
    CHawkPacketQueue Free;
    

    //
    //  USB Frame Information
    //
    
    CHAR  BytesPerUSBFrame;           //Bytes per USB frame (at some rates there is an extra 2 bytes, every
                                       //ExtraSampleInterval frames.
    CHAR  ExtraSampleInterval;        //USB frame intervals for which we must send\receive an extra sample.
    CHAR  ProgrammedFrame;            //Counts up to the ExtraSampleInterval
    CHAR  AvailableTransferContexts;  //Bit 0, is the 0

    //
    //  Contexts for keeping track of outstanding DMA
    //
    HAWK_TRANSFER_CONTEXT TransferContext[2];
    HAWK_WRAP_BUFFER      WrapBuffer;

    //
    //  Members for communicating with USB stack
    //
    KEVENT CloseEvent;
    URB_ISOCH_CLOSE_ENDPOINT     CloseUrb;
    PVOID EndpointHandle;
    
} HAWK_STREAMING_RESOURCES, *PHAWK_STREAMING_RESOURCES;


EXTERNUSB VOID
HawkInit (IUsbInit *pUsbInit);

EXTERNUSB VOID
HawkAddDevice (
    IN IUsbDevice *Device
    );

EXTERNUSB VOID
HawkRemoveDevice (
    IN IUsbDevice *Device
    );

XBOXAPI
EXTERN_C
HRESULT
WINAPI
XVoiceCreateMediaObject(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      IN PWAVEFORMATEX  pwfxFormat OPTIONAL,
      OUT XMediaObject **ppXmediaObject
      );

EXTERN_C
HRESULT
WINAPI
XVoiceCreateMediaObjectInternal(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      IN LPWAVEFORMATEX pwfxFormat,
      IN PFNXMEDIAOBJECTCALLBACK    pfnCallback,
      IN PVOID                      pvContext,
      OUT XMediaObject **ppXmediaObject
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
            OUT PXMEDIAINFO pInfo
            );
        
        STDMETHOD(Flush) (void);
        
        STDMETHOD(Discontinuity) (void);
        
        STDMETHOD(GetStatus) (
            OUT DWORD *pdwFlags
            );
        
        STDMETHOD(Process) (
            IN LPCXMEDIAPACKET   pInputPacket,
            IN LPCXMEDIAPACKET   pOutputPacket
            );
        
        XHawkMediaObject() :
            m_pDevice(NULL),
		    m_wMaxPacket(0),
            m_bEndpointAddress(0),
            m_fConnected(FALSE),
		    m_fOpened(FALSE),
		    m_fClosePending(FALSE),
            m_fRemovePending(FALSE),
            m_fMicrophone(FALSE),
            m_fReady(FALSE),
            m_fClosingEndpoint(FALSE),
            m_Reserved(0),
            m_lRefCount(0),
            m_pStreamingResources(NULL),
            m_pfnCallback(NULL),
            m_pvContext(NULL)
            {}

        static void InitializeClass(ULONG ulMicrophoneCount, ULONG ulHeadphoneCount); //Initialize static members
	
		//
        //	Keeps track of the static list
		//	of objects
		//
		static XHawkMediaObject *sm_pMicrophones;	//Array of microphone instances
		static XHawkMediaObject *sm_pHeadphones;	//Array of headphone instances
        static UCHAR            *sm_pRateIndices;   //Array of sample rate indices for device instances
        static UCHAR            *sm_pRateIndexRefCounts; //Array of reference counts on rate indices
        static USHORT           sm_AvailableHeadphoneResources; //Count of streaming resources available for headphones
        static USHORT           sm_AvailableMicrophoneResources; //Count of streaming resources available for microphones
        static PHAWK_STREAMING_RESOURCES sm_pFreeStreamingResources; //Free list of streaming resource structures.
        static HAWK_SAMPLE_RATE_INFORMATION sm_HawkSampleRates[HAWK_SAMPLE_RATE_COUNT];
        

    private:
        
        //
		//	Information Detected During enumeration.
		//
        IUsbDevice          *m_pDevice;
		WORD			    m_wMaxPacket;
        UCHAR			    m_bEndpointAddress;
		
		//
		//	Running Per Instance State Information
		//
        //  Notes on state flag.  Connecting
        //  and opening a device is synchronous.
        //  Closing a device is asynchronous.
        //
        
		UCHAR			   m_fConnected:1;
		UCHAR			   m_fOpened:1;
		UCHAR			   m_fClosePending:1;
        UCHAR			   m_fRemovePending:1;
        UCHAR			   m_fMicrophone:1;
        UCHAR              m_fReady:1;
        UCHAR              m_fClosingEndpoint:1;
        UCHAR              m_Reserved:1;

        LONG			   m_lRefCount;

        //
        //  For internal callbacks
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
        HRESULT AllocateStreamingResources (ULONG ulMaxAttachedPackets, ULONG ulRateIndex);
        void FreeStreamingResources (void);
        void CloseEndpoint (void);
        static void CloseEndpointComplete (IN PURB, IN XHawkMediaObject *pThis);
        void ProgramTransfer(void);
        static void TransferComplete(IN PUSBD_ISOCH_TRANSFER_STATUS Status, IN PVOID pvContext);
        void AbortMediaPackets();
        HRESULT SetSampleRate(UCHAR ucRateIndex, KIRQL BaseIrql);
		HRESULT SetAGC(UCHAR ucAGC, KIRQL BaseIrql);


    //
    //  The XcreateHawkMediaObject factory method, and the
    //  USB interface methods, need to be friends.  They
    //  all would be members, but they must be C calleable.
    //
    friend HRESULT 
    XVoiceCreateMediaObject(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      IN PWAVEFORMATEX  pwfxFormat  OPTIONAL,
      OUT XMediaObject **ppXmediaObject
      );

    friend HRESULT
    XVoiceCreateMediaObjectInternal(
      IN PXPP_DEVICE_TYPE XppDeviceType,
      IN DWORD dwPort,
      IN DWORD dwMaxAttachedPackets,
      IN LPWAVEFORMATEX pwfxFormat,
      IN PFNXMEDIAOBJECTCALLBACK    pfnCallback,
      IN PVOID                      pvContext,
      OUT XMediaObject **ppXmediaObject
      );

    
    friend VOID HawkInit (IUsbInit *pUsbInit);

    friend VOID HawkAddDevice (
                    IN IUsbDevice *Device
                    );

    friend VOID HawkRemoveDevice (
                    IN IUsbDevice *Device
                    );
};



