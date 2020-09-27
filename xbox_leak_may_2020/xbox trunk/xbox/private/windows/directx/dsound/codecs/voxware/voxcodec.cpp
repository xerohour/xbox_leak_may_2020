/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       voxcodec.cpp
 *  Content:    Voxware CODEC XMO
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/08/00    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"
#include "vci.h"
#include "vcisc3.h"
#include "vcisc6.h"
#include "vcivr12.h"
#include "vcierror.h"

#include "xvocver.h"

DEFINELPCTYPE(BYTE);
DEFINELPTYPE(SHORT);

#define VOXWARE_SC03_MAXBITSPERSEC		3200
#define VOXWARE_SC03_MAXBYTESPERSEC		(VOXWARE_SC03_MAXBITSPERSEC / 8)
#define VOXWARE_SC03_TRAILFRAMES		1
#define VOXWARE_SC03_TIMEOUT			20
#define VOXWARE_SC03_FRAMELENGTH		((VOXWARE_SC03_TIMEOUT * VOXWARE_SC03_MAXBYTESPERSEC) / 1000)

#define VOXWARE_SC06_MAXBITSPERSEC		6400
#define VOXWARE_SC06_MAXBYTESPERSEC		(VOXWARE_SC06_MAXBITSPERSEC / 8)
#define VOXWARE_SC06_TRAILFRAMES		1
#define VOXWARE_SC06_TIMEOUT			20
#define VOXWARE_SC06_FRAMELENGTH		((VOXWARE_SC06_TIMEOUT * VOXWARE_SC06_MAXBYTESPERSEC) / 1000)

#define VOXWARE_VR12_MAXBITSPERSEC		1822
#define VOXWARE_VR12_TRAILFRAMES		1
#define VOXWARE_VR12_TIMEOUT			90
#define VOXWARE_VR12_FRAMELENGTH		21


//
// Voxware function pointers
//

typedef VCI_RETCODE (*LPFNVCIGETINFO)(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);
typedef VCI_RETCODE (*LPFNVCIPROCESS)(LPVOID pEncodeMemBlk, VCI_CODEC_IO_BLOCK *pvciCodecIOBlk);
typedef VCI_RETCODE (*LPFNVCIINIT)(LPVOID *hEncodeMemBlk); 
typedef VCI_RETCODE (*LPFNVCIFREE)(LPVOID *hEncodeMemBlk);

//
// Voxware CODEC XMO
//

class CVoxMediaObject
    : public XMediaObject
{
protected:
    BOOL                    m_fEncoder;                     // TRUE if we're an encoder
    ULONG                   m_ulRefCount;                   // Object reference count
    VCI_CODEC_INFO_BLOCK    m_vciCodecInfo;                 // CODEC info
    WORD                    m_wEncodeFrameSize;             // Encoded frame size

private:
    LPFNVCIGETINFO          m_pfnVciGetInfo;
    LPFNVCIPROCESS          m_pfnVciProcess;
    LPFNVCIINIT             m_pfnVciInit;
    LPFNVCIFREE             m_pfnVciFree;
    LPVOID                  m_pvMemBlock;

public:
    CVoxMediaObject(void);
    virtual ~CVoxMediaObject(void);

public:
    // Initialization
    void STDMETHODCALLTYPE Initialize(WORD wEncodeFormatTag, BOOL fCreateEncoder, LPVOICECODECWAVEFORMAT pwfxEncoded, LPWAVEFORMATEX pwfxDecoded);
    
    // IUnknown methods
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // XMediaObject methods
    virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
    virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
    virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pxmbInput, LPCXMEDIAPACKET pxmbOutput);
    virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
    virtual HRESULT STDMETHODCALLTYPE Flush(void);

protected:
    DWORD STDMETHODCALLTYPE GetEncodeAlignment(void);
    DWORD STDMETHODCALLTYPE GetDecodeAlignment(void);
    
    DWORD STDMETHODCALLTYPE GetInputAlignment(void);
    DWORD STDMETHODCALLTYPE GetOutputAlignment(void);
};

__inline HRESULT CVoxMediaObject::GetStatus(LPDWORD pdwStatus)
{
    *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;
    return DS_OK;
}

__inline HRESULT CVoxMediaObject::Discontinuity(void)
{
    return DS_OK;
}

__inline HRESULT CVoxMediaObject::Flush(void)
{
    return DS_OK;
}

__inline DWORD CVoxMediaObject::GetEncodeAlignment(void)
{
    return m_wEncodeFrameSize;
}

__inline DWORD CVoxMediaObject::GetDecodeAlignment(void)
{
    return m_vciCodecInfo.wNumSamplesPerFrame << 1;
}

__inline DWORD CVoxMediaObject::GetInputAlignment(void)
{
    return m_fEncoder ? GetDecodeAlignment() : GetEncodeAlignment();
}

__inline DWORD CVoxMediaObject::GetOutputAlignment(void)
{
    return m_fEncoder ? GetEncodeAlignment() : GetDecodeAlignment();
}


/****************************************************************************
 *
 *  VoxwareCreateConverter
 *
 *  Description:
 *      Creates a Voxware CODEC object.
 *
 *  Arguments:
 *      WORD [in]: encoded format tag.
 *      BOOL [in]: TRUE to initialize the object as an encoder.
 *      LPVOICECODECWAVEFORMAT [out]: encoded format.
 *      LPWAVEFORMATEX [out]: decoded format.
 *      XMediaObject ** [out]: CODEC object.  The caller is responsible for
 *                             freeing this object with Release.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "VoxwareCreateConverter"

HRESULT
VoxwareCreateConverter
(
    WORD                    wEncodeFormatTag, 
    BOOL                    fCreateEncoder, 
    LPVOICECODECWAVEFORMAT  pwfxEncoded, 
    LPWAVEFORMATEX          pwfxDecoded, 
    LPXMEDIAOBJECT *        ppMediaObject
)
{
    CVoxMediaObject *       pMediaObject;
    HRESULT                 hr;

    DPF_ENTER();

    hr = HRFROMP(pMediaObject = NEW(CVoxMediaObject));

    if(SUCCEEDED(hr))
    {
        pMediaObject->Initialize(wEncodeFormatTag, fCreateEncoder, pwfxEncoded, pwfxDecoded);
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
 *  CVoxMediaObject
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
#define DPF_FNAME "CVoxMediaObject::CVoxMediaObject"

CVoxMediaObject::CVoxMediaObject
(
    void
)
{
    DPF_ENTER();

    //
    // Initialize defaults
    //

    m_ulRefCount = 1;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CVoxMediaObject
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
#define DPF_FNAME "CVoxMediaObject::~CVoxMediaObject"

CVoxMediaObject::~CVoxMediaObject
(
    void
)
{
    DPF_ENTER();

    //
    // Free working memory
    //

    if(m_pvMemBlock)
    {
        ASSERT(m_pfnVciFree);
        m_pfnVciFree(&m_pvMemBlock);
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
 *      WORD [in]: encoded format tag.
 *      WORD [in]: channel count.
 *      BOOL [in]: TRUE to initialize the object as an encoder.
 *      LPVOICECODECWAVEFORMAT [out]: encoded format.
 *      LPWAVEFORMATEX [out]: decoded format.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CVoxMediaObject::Initialize"

void
CVoxMediaObject::Initialize
(
    WORD                    wEncodeFormatTag, 
    BOOL                    fCreateEncoder, 
    LPVOICECODECWAVEFORMAT  pwfxEncoded, 
    LPWAVEFORMATEX          pwfxDecoded
)
{
    DPF_ENTER();

    //
    // Save data members
    //

    m_fEncoder = fCreateEncoder;

    //
    // Set up the function table and save CODEC-specific values
    //

    switch(wEncodeFormatTag) 
    {
        case WAVE_FORMAT_VOXWARE_SC03:
            m_pfnVciGetInfo = vciGetInfoSC3;

            if(m_fEncoder)
            {
                m_pfnVciProcess = vciEncodeSC3;
                m_pfnVciInit = vciInitEncodeSC3;
                m_pfnVciFree = vciFreeEncodeSC3;
            }
            else
            {
                m_pfnVciProcess = vciDecodeSC3;
                m_pfnVciInit = vciInitDecodeSC3;
                m_pfnVciFree = vciFreeDecodeSC3;
            }

            m_wEncodeFrameSize = VOXWARE_SC03_FRAMELENGTH;

            break;

        case WAVE_FORMAT_VOXWARE_SC06:
            m_pfnVciGetInfo = vciGetInfoSC6;

            if(m_fEncoder)
            {
                m_pfnVciProcess = vciEncodeSC6;
                m_pfnVciInit = vciInitEncodeSC6;
                m_pfnVciFree = vciFreeEncodeSC6;
            }
            else
            {
                m_pfnVciProcess = vciDecodeSC6;
                m_pfnVciInit = vciInitDecodeSC6;
                m_pfnVciFree = vciFreeDecodeSC6;
            }

            m_wEncodeFrameSize = VOXWARE_SC06_FRAMELENGTH;

            break;

        case WAVE_FORMAT_VOXWARE_VR12:
            m_pfnVciGetInfo = vciGetInfoVR12;

            if(m_fEncoder)
            {
                m_pfnVciProcess = vciEncodeVR12;
                m_pfnVciInit = vciInitEncodeVR12;
                m_pfnVciFree = vciFreeEncodeVR12;
            }
            else
            {
                m_pfnVciProcess = vciDecodeVR12;
                m_pfnVciInit = vciInitDecodeVR12;
                m_pfnVciFree = vciFreeDecodeVR12;
            }

            m_wEncodeFrameSize = VOXWARE_VR12_FRAMELENGTH;

            break;

        default:
            ASSERTMSG("Invalid encode format tag");
            break;
    }

    //
    // Initialize the Voxware library
    //

    m_pfnVciInit(&m_pvMemBlock);
    m_pfnVciGetInfo(&m_vciCodecInfo);

    //
    // Build an encoded format for the caller
    //

    if(pwfxEncoded)
    {
        pwfxEncoded->wfx.wFormatTag = wEncodeFormatTag;
        pwfxEncoded->wfx.nChannels = m_vciCodecInfo.wNumChannels;
        pwfxEncoded->wfx.nSamplesPerSec = m_vciCodecInfo.wSamplingRate;
        pwfxEncoded->wfx.wBitsPerSample = m_vciCodecInfo.wNumBitsPerSample;
        pwfxEncoded->wfx.nBlockAlign = (WORD)GetEncodeAlignment();
        pwfxEncoded->wfx.cbSize = sizeof(*pwfxEncoded) - sizeof(pwfxEncoded->wfx);
        
        switch(wEncodeFormatTag) 
        {
            case WAVE_FORMAT_VOXWARE_SC03:
                pwfxEncoded->wfx.nAvgBytesPerSec = VOXWARE_SC03_MAXBITSPERSEC;
                
                pwfxEncoded->dwMilliSecPerFrame = VOXWARE_SC03_TIMEOUT;
                pwfxEncoded->dwTrailFrames = VOXWARE_SC03_TRAILFRAMES;

                break;

            case WAVE_FORMAT_VOXWARE_SC06:
                pwfxEncoded->wfx.nAvgBytesPerSec = VOXWARE_SC06_MAXBITSPERSEC;
                
                pwfxEncoded->dwMilliSecPerFrame = VOXWARE_SC06_TIMEOUT;
                pwfxEncoded->dwTrailFrames = VOXWARE_SC06_TRAILFRAMES;

                break;

            case WAVE_FORMAT_VOXWARE_VR12:
                pwfxEncoded->wfx.nAvgBytesPerSec = VOXWARE_VR12_MAXBITSPERSEC;
                
                pwfxEncoded->dwMilliSecPerFrame = VOXWARE_VR12_TIMEOUT;
                pwfxEncoded->dwTrailFrames = VOXWARE_VR12_TRAILFRAMES;

                break;

            default:
                ASSERTMSG("Invalid encode format tag");
                break;
        }
    }

    //
    // Build a decoded format for the caller
    //

    if(pwfxDecoded)
    {
        pwfxDecoded->wFormatTag = WAVE_FORMAT_PCM;
        pwfxDecoded->nChannels = m_vciCodecInfo.wNumChannels;
        pwfxDecoded->nSamplesPerSec = m_vciCodecInfo.wSamplingRate;
        pwfxDecoded->wBitsPerSample = m_vciCodecInfo.wNumBitsPerSample;
        pwfxDecoded->nBlockAlign = pwfxDecoded->nChannels * pwfxDecoded->wBitsPerSample >> 3;
        pwfxDecoded->nAvgBytesPerSec = pwfxDecoded->nBlockAlign * pwfxDecoded->nSamplesPerSec;
        pwfxDecoded->cbSize = 0;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  AddRef
 *
 *  Description:
 *      Increments the object reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      ULONG: reference count.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSecondaryBuffer::AddRef"

ULONG
CVoxMediaObject::AddRef
(
    void
)
{
    ULONG                   ulRefCount;

    DPF_ENTER();

    ASSERT(m_ulRefCount);
    
    ulRefCount = ++m_ulRefCount;

    DPF_LEAVE(ulRefCount);

    return ulRefCount;
}


/****************************************************************************
 *
 *  Release
 *
 *  Description:
 *      Decrements the object reference count.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      ULONG: reference count.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CDirectSoundSecondaryBuffer::Release"

ULONG
CVoxMediaObject::Release
(
    void
)
{
    ULONG                   ulRefCount;

    DPF_ENTER();

    ASSERT(m_ulRefCount);
    
    if(!(ulRefCount = --m_ulRefCount))
    {
        delete this;
    }

    DPF_LEAVE(ulRefCount);

    return ulRefCount;
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the object supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [in/out]: info structure.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CVoxMediaObject::GetInfo"

HRESULT
CVoxMediaObject::GetInfo
(
    LPXMEDIAINFO            pInfo
)
{
    DPF_ENTER();

    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;
    pInfo->dwInputSize = GetInputAlignment();
    pInfo->dwOutputSize = GetOutputAlignment();
    pInfo->dwMaxLookahead = 0;

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
 *      LPCXMEDIAPACKET  [in]: input buffer.
 *      LPCXMEDIAPACKET  [in]: output buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CVoxMediaObject::Process"

HRESULT
CVoxMediaObject::Process
(
    LPCXMEDIAPACKET         pxmbSource, 
    LPCXMEDIAPACKET         pxmbDest 
)
{
    HRESULT                 hr                  = DS_OK;
    LPCXMEDIAPACKET         pxmbDecode;
    LPCXMEDIAPACKET         pxmbEncode;
    DWORD                   dwEncodeAlignment;
    DWORD                   dwDecodeAlignment;
    DWORD                   dwFrames;
    VCI_CODEC_IO_BLOCK      vciBlock;
    VCI_RETCODE             vciResult;
    DWORD                   i;
    
    DPF_ENTER();

    ASSERT(pxmbSource && pxmbDest);

    if(m_fEncoder)
    {
        pxmbDecode = pxmbSource;
        pxmbEncode = pxmbDest;
    }
    else
    {
        pxmbDecode = pxmbDest;
        pxmbEncode = pxmbSource;
    }

    // 
    // Initialize output members of the packet descriptors
    //
    
    XMOAcceptPacket(pxmbSource);
    XMOAcceptPacket(pxmbDest);

    //
    // Set up the processing block
    //

    dwEncodeAlignment = GetEncodeAlignment();
    dwDecodeAlignment = GetDecodeAlignment();

    ZeroMemory(&vciBlock, sizeof(vciBlock));
    
    vciBlock.pPCMBuffer = (short *)pxmbDecode->pvBuffer;
    vciBlock.pVoxBuffer = (LPBYTE)pxmbEncode->pvBuffer;
    vciBlock.wVoxBufferSize = (WORD)pxmbEncode->dwMaxSize;

    dwFrames = pxmbDecode->dwMaxSize / dwDecodeAlignment;

    if(!m_fEncoder)
    {
        vciBlock.wVoxBufferWriteByteOffset = vciBlock.wVoxBufferSize;
        vciBlock.wVoxBufferSize += 1;
    }

    //
    // Enter the main loop
    //

    for(i = 0; i < dwFrames; i++)
    {
        if(VCI_NO_ERROR != (vciResult = m_pfnVciProcess(m_pvMemBlock, &vciBlock)))
        {
            DPF_ERROR("Encode failed with error %lu", vciResult);
            hr = DSERR_GENERIC;

            break;
        }

        vciBlock.pPCMBuffer += m_vciCodecInfo.wNumSamplesPerFrame;
        
        if(m_fEncoder)
        {
            vciBlock.wSamplesInPCMBuffer = 0;
        }
    }

    //
    // Complete the packets
    //

    XMOCompletePacketSync(pxmbEncode, i * dwEncodeAlignment, SUCCEEDED(hr) ? XMEDIAPACKET_STATUS_SUCCESS : XMEDIAPACKET_STATUS_FAILURE);
    XMOCompletePacketSync(pxmbDecode, i * dwDecodeAlignment, SUCCEEDED(hr) ? XMEDIAPACKET_STATUS_SUCCESS : XMEDIAPACKET_STATUS_FAILURE);

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


