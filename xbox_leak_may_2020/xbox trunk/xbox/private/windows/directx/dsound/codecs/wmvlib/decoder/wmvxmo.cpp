/*++

Copyright (c) 2001  Microsoft Corporation. All rights reserved.

Module Name:

    wmvxmo.cpp

Abstract:

    XMO wrapper for the WMV decoder library

--*/


#include "bldsetup.h"
#pragma comment(linker, "/merge:WMVDECRW=WMVDEC")
#pragma comment(linker, "/merge:WMVDECRD=WMVDEC")
#pragma comment(linker, "/section:WMVDEC,ERW")

#ifdef WMV_MEMORY_DEBUGGING
#include <ntos.h>
#endif

#include <xtl.h>
#include <wmvxmo.h>
#include <xdbg.h>

#include "wmcdecode.h"

//
// Convert the last error code to HRESULT
//
inline HRESULT GetLastHRESULT() {
    DWORD err = GetLastError();
    return HRESULT_FROM_WIN32(err);
}


//
// Helper class for implementing file data buffering
//
struct IOBUF {
    BYTE* StartPtr;
    DWORD StartOffset;
    DWORD ByteCount;
    INT State;
    OVERLAPPED Overlapped;

    enum { EMPTY, PENDING, READY };

    BOOL IsReady() { return State == READY; }
    BOOL IsPending() { return State == PENDING; }
    BOOL IsEmpty() { return State == EMPTY; }

    BOOL Contains(DWORD offset) {
        return (offset >= StartOffset) && (offset < StartOffset+ByteCount);
    }

    BOOL Contains(DWORD offset, DWORD count) {
        return (offset >= StartOffset) && (offset + count <= StartOffset + ByteCount);
    }
};

struct CFileBuffer {
    //
    // Constructor
    //
    CFileBuffer() {
        m_File = INVALID_HANDLE_VALUE;
        m_AllocBuffer = NULL;
        m_ErrorFlag = FALSE;
        memset(m_Buffers, 0, sizeof(m_Buffers));
        memset(m_Events, 0, sizeof(m_Events));
    }

    //
    // Destructor
    //
    ~CFileBuffer() {
        if (m_File != INVALID_HANDLE_VALUE) {
            BOOL closeok = CloseHandle(m_File);
            ASSERT(closeok);
        }

        if (m_AllocBuffer) {
            BOOL freeok = VirtualFree(m_AllocBuffer, 0, MEM_RELEASE);
            ASSERT(freeok);

            XDBGTRC("WMVDEC", "Max backward jump: %d", m_MaxBackwardJump);
        }

        for (INT i=0; i < FILE_BUFFER_COUNT_TOTAL; i++) {
            if (m_Events[i]) {
                CloseHandle(m_Events[i]);
            }
        }
    }

    //
    // Wait for pending overlapped I/O to complete
    //
    VOID Flush() {
        if (IsOpen()) {
            for (INT i=0; i < FILE_BUFFER_COUNT_TOTAL; i++) {
                CheckIoBufCompletion(&m_Buffers[i], TRUE);
            }
        }
    }

    //
    // Open a file
    //
    BOOL IsOpen() { return (m_File != INVALID_HANDLE_VALUE); }

    BOOL OpenFile(LPCSTR filename) {
        DWORD fileflag = FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN;

        #ifndef DISABLE_FILE_BUFFERING
        fileflag |= FILE_FLAG_OVERLAPPED|FILE_FLAG_NO_BUFFERING;
        #endif

        m_File = CreateFile(filename,
                            GENERIC_READ,
                            FILE_SHARE_READ,
                            NULL,
                            OPEN_EXISTING,
                            fileflag,
                            NULL);
        
        if (m_File == INVALID_HANDLE_VALUE) {
            XDBGWRN("WMVDEC", "Failed to open file %s: %d", filename, GetLastError());
            return FALSE;
        }

        m_FileSize = GetFileSize(m_File, NULL);
        if (m_FileSize == 0xffffffff) {
            XDBGWRN("WMVDEC", "Failed to get file size: %d", GetLastError());
            return FALSE;
        }

        ASSERT(m_FileSize <= 0xffffffff - FILE_BUFFER_COUNT_FORWARD*FILE_BUFFER_SIZE);
        return TRUE;
    }

    //
    // Initialize the file buffer object
    //
    HRESULT Initialize() {
        if (!m_AllocBuffer) {
            DWORD bufsize = FILE_BUFFER_COUNT_TOTAL * FILE_BUFFER_SIZE;
            m_AllocBuffer = (LPBYTE) VirtualAlloc(NULL, bufsize, MEM_COMMIT, PAGE_READWRITE); 
            if (!m_AllocBuffer) return DSERR_OUTOFMEMORY;
            memset(m_AllocBuffer, 0, bufsize);
        }

        m_CurBuf = m_Buffers;
        memset(m_Buffers, 0, sizeof(m_Buffers));

        for (INT i=0; i < FILE_BUFFER_COUNT_TOTAL; i++) {
            m_Buffers[i].StartPtr = m_AllocBuffer + i*FILE_BUFFER_SIZE;

            if (!m_Events[i]) {
                m_Events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
                if (!m_Events[i])
                    return GetLastHRESULT();
            }
            m_Buffers[i].Overlapped.hEvent = m_Events[i];
        }

        m_MaxBackwardJump = 0;
        m_ErrorFlag = FALSE;
        return DS_OK;
    }

    //
    // Check if there is no data available in the file buffer
    //
    BOOL NoDataAvailable() {
        return CheckIoBufCompletion(&m_Buffers[0], FALSE);
    }

    //
    // Get the next file buffer after the specified one
    //
    IOBUF* GetNextBuf(IOBUF* iobuf) {
        return iobuf == &m_Buffers[FILE_BUFFER_COUNT_TOTAL-1] ? m_Buffers : iobuf+1;
    }

    //
    // Read data from the file
    //  NOTE: We assume the access pattern is almost sequential
    //  and optimize for that case accordingly.
    //
    DWORD ReadData(DWORD offset, DWORD byteCount, LPBYTE* ppData) {
        #ifdef DISABLE_FILE_BUFFERING

        DWORD cb;
        ASSERT(byteCount <= MAX_WMV_READ_REQUEST);
        if (SetFilePointer(m_File, offset, NULL, FILE_BEGIN) != offset ||
            !ReadFile(m_File, m_AllocBuffer, byteCount, &cb, NULL)) {
            XDBGWRN("WMVDEC", "ReadFile failed: %d", GetLastError());
            return 0;
        }

        *ppData = m_AllocBuffer;
        return cb;

        #else // !DISABLE_FILE_BUFFERING

        if (m_CurBuf->IsReady() && m_CurBuf->Contains(offset, byteCount)) {
            //
            // Fast code path: requested data is entirely in the current buffer
            //
            *ppData = m_CurBuf->StartPtr + (offset - m_CurBuf->StartOffset);
            return byteCount;
        }

        // We're in error state, fail all reads.
        *ppData = m_AllocBuffer;
        if (m_ErrorFlag || byteCount == 0) return 0;
        
        ASSERT(byteCount <= MAX_WMV_READ_REQUEST);
        if (offset >= m_FileSize || byteCount > m_FileSize-offset) {
            XDBGWRN("WMVDEC", "Trying to read past end of file: %d+%d", offset, byteCount);
            return 0;
        }

        //
        // If the new window base in the buffering window but
        // is behind the current window base, then we'll just
        // change the window base.
        //
        // Otherwise, we need to slide the entire buffering window.
        //
        DWORD newWindowBase = offset & ~(FILE_BUFFER_SIZE-1);

        // Barf if we're seeking back past our back buffer.
        // Note that one case is allowed: rewind from the index
        // at the end of the file to the beginning of the file.
        #if DBG
        DWORD backdelta;
        if ((newWindowBase < m_CurBuf->StartOffset) &&
            (backdelta = m_CurBuf->StartOffset - newWindowBase) < m_FileSize/2) {
            if (backdelta > m_MaxBackwardJump)
                m_MaxBackwardJump = backdelta;
            if (backdelta > FILE_BUFFER_COUNT_BACKWARD*FILE_BUFFER_SIZE) {
                DbgPrint(
                    "Your WMV file is badly encoded. Please read the Xbox WMV\n"
                    "encoding guidelines and reencode your movie.\n");
                __asm int 3;
            }
        }
        #endif

        INT i = (newWindowBase>>FILE_BUFFER_SIZE_SHIFT) % FILE_BUFFER_COUNT_TOTAL;
        IOBUF* curbuf = m_CurBuf = &m_Buffers[i];

        if (curbuf->IsPending() && curbuf->StartOffset != newWindowBase) {
            CheckIoBufCompletion(curbuf, TRUE);
        }

        i = FILE_BUFFER_COUNT_FORWARD;
        do {
            if (curbuf->IsEmpty() ||
                curbuf->StartOffset != newWindowBase && !curbuf->IsPending()) {
                StartReadRequest(curbuf, newWindowBase);
            }
            curbuf = GetNextBuf(curbuf);
            newWindowBase += FILE_BUFFER_SIZE;
        } while (--i && newWindowBase < m_FileSize);

        if (m_CurBuf->IsPending()) {
            CheckIoBufCompletion(m_CurBuf, TRUE);
        }

        if (m_CurBuf->Contains(offset, byteCount)) {
            *ppData = m_CurBuf->StartPtr + (offset - m_CurBuf->StartOffset);
            return byteCount;
        }
    
        if (m_CurBuf->ByteCount != FILE_BUFFER_SIZE) {
            XDBGWRN("WMVDEC", "Not enough data: %d+%d", offset, byteCount);
            return 0;
        }

        //
        // The data is split into two different buffers
        //
        DWORD cnt1 = m_CurBuf->StartOffset + m_CurBuf->ByteCount - offset;
        memcpy(m_PartialBuf, m_CurBuf->StartPtr+m_CurBuf->ByteCount-cnt1, cnt1);
        offset += cnt1;

        DWORD cnt2 = byteCount - cnt1;
        BYTE* p;
        if (ReadData(offset, cnt2, &p) != cnt2)
            return 0;

        ASSERT(p != m_PartialBuf);
        memcpy(&m_PartialBuf[cnt1], p, cnt2);
        *ppData = m_PartialBuf;
        return byteCount;

        #endif // !DISABLE_FILE_BUFFERING
    }


    //
    // Initiate an overlapped read operation
    //
    VOID StartReadRequest(IOBUF* iobuf, DWORD offset) {
        iobuf->StartOffset = offset & ~(FILE_BUFFER_SIZE-1);
        iobuf->Overlapped.OffsetHigh = 0;
        iobuf->Overlapped.Offset = iobuf->StartOffset;
        ResetEvent(iobuf->Overlapped.hEvent);

        BOOL result = ReadFile(
                            m_File,
                            iobuf->StartPtr,
                            FILE_BUFFER_SIZE, 
                            &iobuf->ByteCount,
                            &iobuf->Overlapped);

        if (result) {
            // The read completes successfully right away
            iobuf->State = IOBUF::READY;
        } else {
            iobuf->ByteCount = 0;
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING) {
                iobuf->State = IOBUF::PENDING;
            } else {
                XDBGWRN("WMVDEC", "ReadFile failed: error = %d\n", err);
                iobuf->State = IOBUF::EMPTY;
                m_ErrorFlag = TRUE;
            }
        }
    }

    //
    // Check if both IO buffers have pending I/O operations
    //
    BOOL CheckIoBufCompletion(IOBUF* iobuf, BOOL wait) {
        if (!iobuf->IsPending()) return FALSE;

        // Do the faster check first
        if (!wait && !HasOverlappedIoCompleted(&iobuf->Overlapped))
            return TRUE;
        
        BOOL result = GetOverlappedResult(m_File, &iobuf->Overlapped, &iobuf->ByteCount, wait);

        if (result) {
            iobuf->State = IOBUF::READY;
        } else {
            DWORD err = GetLastError();
            if (err == ERROR_IO_INCOMPLETE)
                return TRUE;

            XDBGWRN("WMVDEC", "Overlapped read failed: error = %d\n", err);
            iobuf->State = IOBUF::EMPTY;
            iobuf->ByteCount = 0;
            m_ErrorFlag = TRUE;
        }

        return FALSE;
    }

    enum {
        MAX_DISK_SECTOR_SIZE = 2*1024,      // for both DVD and hard disk
        MAX_WMV_READ_REQUEST = 512,         // max WMV read request - currently at 512 bytes
        FILE_BUFFER_SIZE_SHIFT = 16,
        FILE_BUFFER_SIZE = 1 << FILE_BUFFER_SIZE_SHIFT,
        FILE_BUFFER_COUNT_FORWARD = 4,
        FILE_BUFFER_COUNT_BACKWARD = 14,
        FILE_BUFFER_COUNT_TOTAL = FILE_BUFFER_COUNT_FORWARD+FILE_BUFFER_COUNT_BACKWARD,
    };

    HANDLE m_File;
    DWORD m_FileSize;
    BYTE* m_AllocBuffer;
    BOOL m_ErrorFlag;
    DWORD m_MaxBackwardJump;
    IOBUF* m_CurBuf;
    IOBUF m_Buffers[FILE_BUFFER_COUNT_TOTAL];
    HANDLE m_Events[FILE_BUFFER_COUNT_TOTAL];
    BYTE m_PartialBuf[MAX_WMV_READ_REQUEST];
};

class CWMVDecoder : public IWMVDecoder {
public:
    CWMVDecoder() {
        m_RefCount = 1;
        m_Signature = MYSIGNATURE;
        m_LastHR = DS_OK;
        m_Callback = NULL;
        m_LastHR = DS_OK;
        m_WMC = NULL;
        m_NumAudioStreams = m_NumVideoStreams = 0;
        m_AudioSampleSize = 0;
    }

    ~CWMVDecoder() { Reset(); }

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement(&m_RefCount);
    }

    STDMETHODIMP_(ULONG) Release() {
       long refcnt = InterlockedDecrement(&m_RefCount);
       if (refcnt == 0) delete this;
       return refcnt;
    }

    STDMETHODIMP GetInfo(LPXMEDIAINFO pInfo) {
        //
        // These flags are copied from WMA XMO interface.
        // I'm not sure what effects they actually have, if any.
        //
        pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE |
                         XMO_STREAMF_FIXED_PACKET_ALIGNMENT;

        // dwInputSize field is not applicable to us.
        //  Just default it to something.
        pInfo->dwInputSize = m_AudioSampleSize;

        //
        // dwOutputSize is applicable only to audio output packet
        // and not to video output packet
        //
        pInfo->dwOutputSize = 2048 * m_AudioSampleSize;
        pInfo->dwMaxLookahead = 0;

        return DS_OK;
    }

    STDMETHODIMP GetStatus(DWORD* pdwStatus) {
        //
        // Since we're synchronous, we're always ready to accept input and output
        //
        *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;
        return DS_OK;
    }

    STDMETHODIMP Process(LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket) {
        //
        // Not supported, use Decode method instead
        //
        return DSERR_UNSUPPORTED;
    }

    STDMETHODIMP Discontinuity() {
        //
        // This is a NOP for us, always return success
        //
        return DS_OK;
    }

    STDMETHODIMP Flush() {
        //
        // Clean up the current state and reinitialize the decoder
        //
        HRESULT hr = Reset();
        if (SUCCEEDED(hr)) hr = Initialize();
        return (m_LastHR = hr);
    }

    STDMETHODIMP GetPlayDuration(PREFERENCE_TIME prtPlayDuration, PREFERENCE_TIME prtPreroll) {
        *prtPlayDuration = (REFERENCE_TIME) m_WMVHeaderInfo.u32PlayDuration * 10000;
        *prtPreroll = (REFERENCE_TIME) m_WMVHeaderInfo.u32Preroll * 10000;
        return DS_OK;
    }

    STDMETHODIMP GetVideoInfo(LPWMVVIDEOINFO pVideoInfo);
    STDMETHODIMP GetAudioInfo(LPWAVEFORMATEX pAudioInfo);
    STDMETHODIMP ProcessMultiple(LPCXMEDIAPACKET pVideoOutputPacket, LPCXMEDIAPACKET pAudioOutputPacket);
    STDMETHODIMP Seek(PREFERENCE_TIME prtSeekPosition);

    HRESULT Initialize();
    HRESULT Reset();
    DWORD ReadFileData(DWORD offset, DWORD byteCount, LPBYTE* ppData);

    LONG m_RefCount;
    DWORD m_Signature;
    LPFNWMVXMODATACALLBACK m_Callback;
    LPVOID m_ClientContext;
    HRESULT m_LastHR;
    HWMCDECODER m_WMC;
    tVideoFormat_WMC m_VideoOutputFormat;
    DWORD m_NumAudioStreams, m_NumVideoStreams;
    strAudioInfo_WMC m_AudioStreamInfo;
    strVideoInfo_WMC m_VideoStreamInfo;
    strHeaderInfo_WMC m_WMVHeaderInfo;
    DWORD m_AudioSampleSize;
    INT m_LastStreamId;
    INT m_NoOutputDataCount;
    CFileBuffer m_FileBuffer;

    enum { MYSIGNATURE = 'WMVX' };
    enum { STREAMID_NONE = 0xffffffff };
};


STDAPI
WmvCreateDecoder(
    IN LPCSTR pszFileName,
    IN LPFNWMVXMODATACALLBACK pfnCallback,
    IN DWORD dwVideoOutputFormat,
    IN LPVOID pvContext,
    OUT LPWMVDECODER* ppWmvDecoder
    )

/*++

Routine Description:

    Create a WMV decoder instance

Arguments:

    pszFilename - Specifies the name of the WMV file to be decoded
    pfnCallback - Supply the callback function for reading input data
        either pszFilename or pfnCallback should be specified, but not both
    dwVideoOutputFormat - Specifies the video output format
        must be WMVVIDEOFORMAT_YUY2
    pvContext - This pointer will be passed to the callback function
    ppWmvDecoder - Returns a pointer to the new WMV decoder instance

Return Value:

    Error code

--*/

{
    ASSERT(pszFileName || pfnCallback);
    ASSERT(dwVideoOutputFormat == WMVVIDEOFORMAT_YUY2);

    #ifdef WMV_MEMORY_DEBUGGING
    WmvDebugMemInit();
    #endif

    *ppWmvDecoder = NULL;

    CWMVDecoder* decoder = new CWMVDecoder;
    if (!decoder) return DSERR_OUTOFMEMORY;
    decoder->m_ClientContext = pvContext;
    decoder->m_VideoOutputFormat = YUY2_WMV;

    HRESULT hr = DS_OK;
    if (pszFileName) {
        // If the filename is specified, we ignore the callback function.

        // Open the file
        if (!decoder->m_FileBuffer.OpenFile(pszFileName))
            hr = GetLastHRESULT();
    } else {
        decoder->m_Callback = pfnCallback;
    }

    if (SUCCEEDED(hr))
        hr = decoder->Initialize();

    if (SUCCEEDED(hr))
        *ppWmvDecoder = decoder;
    else
        delete decoder;

    return hr;
}


HRESULT
CWMVDecoder::Initialize()

/*++

Routine Description:

    Initialize the WMV decoder instance

Arguments:

    NONE

Return Value:

    Error code

--*/

{
    if (m_FileBuffer.IsOpen()) {
        //
        // Initialize the file buffer
        //
        HRESULT hr = m_FileBuffer.Initialize();
        if (FAILED(hr)) return hr;
    }

    //
    // Create the WMV decoder object
    //
    tWMCDecStatus wmcerr;
    m_WMC = WMCDecCreate((U32_WMC) this);
    if (!m_WMC) {
        wmcerr = WMCDec_Fail;
        goto exit;
    }

    //
    // Initialize the decoder
    //
    wmcerr = WMCDecInit(&m_WMC, m_VideoOutputFormat, TRUE, 0);
    if (wmcerr != WMCDec_Succeeded) goto exit;
    
    wmcerr = WMCDecGetHeaderInfo(m_WMC, &m_WMVHeaderInfo);
    if (wmcerr != WMCDec_Succeeded) goto exit;

    XDBGTRC("WMVDEC",
        "WMV header information:\n"
        "  play duration: %d\n"
        "  preroll: %d\n"
        "  send duration: %d\n"
        "  max bit rate: %d\n"
        "  has DRM: %d\n",
        m_WMVHeaderInfo.u32PlayDuration,
        m_WMVHeaderInfo.u32Preroll,
        m_WMVHeaderInfo.u32SendDuration,
        m_WMVHeaderInfo.u32MaxBitrate,
        m_WMVHeaderInfo.u8HasDRM);

    //
    // Get information about each stream in the WMV file
    //
    U32_WMC i, numStreams;
    tStreamIdnMediaType_WMC* streamTypes;
    wmcerr = WMCDecGetStreamInfo(m_WMC, &numStreams, &streamTypes);
    if (wmcerr != WMCDec_Succeeded) goto exit;

    XDBGTRC("WMVDEC", "Number of streams: %d", numStreams);

    tStreamIdPattern_WMC* decPatterns;
    decPatterns = (tStreamIdPattern_WMC*) wmvalloc(numStreams * sizeof(*decPatterns));
    if (!decPatterns) {
        wmcerr = WMCDec_BadMemory;
        goto exit;
    }

    U32_WMC selectedVideoStreamIndex = 0xffffffff;
    U32_WMC selectedAudioStreamIndex = 0xffffffff;
    m_NumAudioStreams = m_NumVideoStreams = 0;
    for (i=0; i < numStreams && wmcerr == WMCDec_Succeeded; i++) {
        U16_WMC streamId = streamTypes[i].wStreamId;
        XDBGTRC("WMVDEC", "Stream: id = %d", streamId);
        ASSERT(streamId != STREAMID_NONE);

        decPatterns[i].wStreamId = streamId;
        decPatterns[i].tPattern = Discard_WMC;

        switch (streamTypes[i].MediaType) {
        case Audio_WMC: {
            strAudioInfo_WMC audioStreamInfo;
            wmcerr = WMCDecGetAudioInfo(m_WMC, streamId, &audioStreamInfo);
            if (wmcerr != WMCDec_Succeeded) goto exit;

            m_NumAudioStreams++;
            if (selectedAudioStreamIndex != 0xffffffff) {
                if (m_AudioStreamInfo.u32BitsPerSecond < audioStreamInfo.u32BitsPerSecond) {
                    XDBGWRN("WMVDEC",
                        "Discarding audio stream: id = %d",
                        m_AudioStreamInfo.u16StreamId);
                    decPatterns[selectedAudioStreamIndex].tPattern = Discard_WMC;
                } else {
                    XDBGWRN("WMVDEC",
                        "Discarding audio stream: id = %d",
                        audioStreamInfo.u16StreamId);
                    break;
                }
            }

            selectedAudioStreamIndex = i;
            decPatterns[i].tPattern = Decompressed_WMC;
            m_AudioStreamInfo = audioStreamInfo;
            ASSERT(streamId == audioStreamInfo.u16StreamId);
            break;
        }

        case Video_WMC: {
            strVideoInfo_WMC videoStreamInfo;
            wmcerr = WMCDecGetVideoInfo(m_WMC, streamId, &videoStreamInfo);
            if (wmcerr != WMCDec_Succeeded) goto exit;

            m_NumVideoStreams++;
            if (selectedVideoStreamIndex != 0xffffffff) {
                if (m_VideoStreamInfo.u32BitsPerSecond < videoStreamInfo.u32BitsPerSecond) {
                    XDBGWRN("WMVDEC",
                        "Discarding video stream: id = %d",
                        m_VideoStreamInfo.u16StreamId);
                    decPatterns[selectedVideoStreamIndex].tPattern = Discard_WMC;
                } else {
                    XDBGWRN("WMVDEC",
                        "Discarding video stream: id = %d",
                        videoStreamInfo.u16StreamId);
                    break;
                }
            }

            selectedVideoStreamIndex = i;
            decPatterns[i].tPattern = Decompressed_WMC;
            m_VideoStreamInfo = videoStreamInfo;
            ASSERT(streamId == videoStreamInfo.u16StreamId);
            break;
        }

        case Binary_WMC: {
            XDBGWRN("WMVDEC", "Binary stream ignored");

            strBinaryInfo_WMC binaryStreamInfo;
            wmcerr = WMCDecGetBinaryInfo(m_WMC, streamId, &binaryStreamInfo);
            break;
        }

        default:
            XDBGWRN("WMVDEC", "Unknown stream type: %d", streamTypes[i].MediaType);
            break;
        }
    }

    if (wmcerr == WMCDec_Succeeded)
        wmcerr = WMCDecSetDecodePatternForStreams(&m_WMC, decPatterns);
    wmvfree(decPatterns);
    if (wmcerr != WMCDec_Succeeded) goto exit;

    //
    // Only 16-bit PCM audio is supported at the moment
    //
    if (m_NumAudioStreams) {
        XDBGTRC("WMVDEC",
            "Active audio stream:\n"
            "  format tag: %d\n"
            "  bits-per-second: %d\n"
            "  bits-per-sample: %d\n"
            "  samples-per-second: %d\n"
            "  number of channels: %d\n"
            "  block size: %d\n"
            "  valid bits-per-sample: %d\n"
            "  channel mask: %d\n"
            "  stream id: %d\n",
            m_AudioStreamInfo.u16FormatTag,
            m_AudioStreamInfo.u32BitsPerSecond,
            m_AudioStreamInfo.u16BitsPerSample,
            m_AudioStreamInfo.u16SamplesPerSecond,
            m_AudioStreamInfo.u16NumChannels,
            m_AudioStreamInfo.u32BlockSize,
            m_AudioStreamInfo.u16ValidBitsPerSample,
            m_AudioStreamInfo.u32ChannelMask,
            m_AudioStreamInfo.u16StreamId);

        ASSERT(m_AudioStreamInfo.u16BitsPerSample == 16 &&
               m_AudioStreamInfo.u16ValidBitsPerSample == 16);

        m_AudioSampleSize = m_AudioStreamInfo.u16NumChannels *
                            m_AudioStreamInfo.u16ValidBitsPerSample / 8;
    } else {
        XDBGWRN("WMVDEC", "No audio stream");
    }

    //
    // Frame width must be even
    //
    if (m_NumVideoStreams) {
        XDBGTRC("WMVDEC",
            "Active video stream:\n"
            "  compression: 0x%x\n"
            "  bits-per-second: %d\n"
            "  frames-per-second: %d\n"
            "  width: %d\n"
            "  height: %d\n"
            "  stream id: %d\n"
            "  output bit count: %d\n"
            "  output pixel type: 0x%x\n",
            m_VideoStreamInfo.u32Compression,
            m_VideoStreamInfo.u32BitsPerSecond,
            (ULONG) m_VideoStreamInfo.fltFramesPerSecond,
            m_VideoStreamInfo.i32Width,
            m_VideoStreamInfo.i32Height,
            m_VideoStreamInfo.u16StreamId,
            m_VideoStreamInfo.u16VideoOutputBitCount,
            m_VideoStreamInfo.u32VideoOutputPixelType);

        ASSERT(m_VideoStreamInfo.i32Width > 0 &&
               m_VideoStreamInfo.i32Height > 0 &&
               m_VideoStreamInfo.i32Width % 2 == 0);
    } else {
        XDBGWRN("WMVDEC", "No video stream");
    }

    m_LastStreamId = STREAMID_NONE;

    // Start this with a reasonably large negative number
    // because for some files the decoder will return
    // no data for quite a few calls to WMCDecDecodeData
    // at the beginning of the file.
    m_NoOutputDataCount = -100;

exit:
    if (wmcerr == WMCDec_Succeeded)
        return DS_OK;
    
    XDBGWRN("WMVDEC", "Initialization failed: %d", wmcerr);
    return MAKE_WMVHRESULT(wmcerr);
}


HRESULT
CWMVDecoder::Reset()

/*++

Routine Description:

    Reset the WMV decoder instance

Arguments:

    NONE

Return Value:

    Error code

--*/

{
    m_FileBuffer.Flush();

    if (m_WMC) {
        tWMCDecStatus wmcerr = WMCDecClose(&m_WMC);
        if (wmcerr == WMCDec_Succeeded)
            m_WMC = NULL;
        else
            return MAKE_WMVHRESULT(wmcerr);
    }

    return DS_OK;
}


U32_WMC
WMCDecCBGetData(
    HWMCDECODER hWMCDec,
    U64_WMC nOffset,
    U32_WMC nNumBytesRequest,
    U8_WMC **ppData,
    U32_WMC u32UserData
    )

/*++

Routine Description:

    This is the callback function for the WMV decoder library

Arguments:

    hWMCDec - Handle to the decoder object
    nOffset - Specifies the byte offset from the beginning of the WMV file
    nNumBytesRequest - Specifies the amount of data requested
    ppData - Returns a pointer to the input data
    u32UserData - User context

Return Value:

    Number of bytes returned, 0 if there is an error

--*/

{
    // NOTE: WMV XMO callback only support 32-bit offset
    ASSERT((nOffset >> 32) == 0);
    DWORD offset = (DWORD) nOffset;

    CWMVDecoder* decoder = (CWMVDecoder*) u32UserData;

    if (decoder->m_Callback) {
        return (*decoder->m_Callback)(
                    decoder->m_ClientContext,
                    offset,
                    nNumBytesRequest,
                    (LPVOID*) ppData);
    } else {
        return decoder->m_FileBuffer.ReadData(offset, nNumBytesRequest, ppData);
    }
}


STDMETHODIMP
CWMVDecoder::GetAudioInfo(
    LPWAVEFORMATEX pAudioInfo
    )

/*++

Routine Description:

    Get information about the active audio stream

Arguments:

    pAudioInfo - Output buffer for returning the audio stream information

Return Value:

    Error code

--*/

{
    if (m_NumAudioStreams == 0)
        return WMVERR_NO_AUDIO_STREAM;

    pAudioInfo->wFormatTag = WAVE_FORMAT_PCM;
    pAudioInfo->nChannels = m_AudioStreamInfo.u16NumChannels;
    pAudioInfo->nSamplesPerSec = m_AudioStreamInfo.u16SamplesPerSecond;
    pAudioInfo->wBitsPerSample = m_AudioStreamInfo.u16ValidBitsPerSample;
    pAudioInfo->nBlockAlign = (WORD) m_AudioSampleSize;
    pAudioInfo->nAvgBytesPerSec = m_AudioStreamInfo.u16SamplesPerSecond * m_AudioSampleSize;
    pAudioInfo->cbSize = 0;

    return DS_OK;
}


STDMETHODIMP
CWMVDecoder::GetVideoInfo(
    LPWMVVIDEOINFO pVideoInfo
    )

/*++

Routine Description:

    Get information about the active video stream

Arguments:

    pVideoInfo - Output buffer for returning the video stream information

Return Value:

    Error code

--*/

{
    if (m_NumVideoStreams == 0)
        return WMVERR_NO_VIDEO_STREAM;

    pVideoInfo->dwWidth = m_VideoStreamInfo.i32Width;
    pVideoInfo->dwHeight = m_VideoStreamInfo.i32Height;
    pVideoInfo->dwOutputBitsPerPixel = m_VideoStreamInfo.u16VideoOutputBitCount;
    pVideoInfo->fFramesPerSecond = m_VideoStreamInfo.fltFramesPerSecond;
    pVideoInfo->dwBitsPerSecond = m_VideoStreamInfo.u32BitsPerSecond;

    return DS_OK;
}


STDMETHODIMP
CWMVDecoder::ProcessMultiple(
    LPCXMEDIAPACKET pVideoOutputPacket,
    LPCXMEDIAPACKET pAudioOutputPacket
    )

/*++

Routine Description:

    Description of the function

Arguments:

    pVideoOutputPacket - Video output packet
    pAudioOutputPacket - Audio output packet

Return Value:

    Error code

--*/

{
    if( pVideoOutputPacket ) {
        *(pVideoOutputPacket->pdwCompletedSize) = 0;
        *(pVideoOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_PENDING;
    }

    if( pAudioOutputPacket ) {
        *(pAudioOutputPacket->pdwCompletedSize) = 0;
        *(pAudioOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_PENDING;
    }

    // Check if the decoder object is in an error state
    if (FAILED(m_LastHR)) {
        if( pVideoOutputPacket )
            *(pVideoOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_FAILURE;
        if( pAudioOutputPacket )
            *(pAudioOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_FAILURE;
        return m_LastHR;
    }

    U32_WMC readyId, samplesReady;
    tWMCDecStatus wmcerr;
    HRESULT hr = DS_OK;

    if (m_LastStreamId != STREAMID_NONE) {
        readyId = m_LastStreamId;
        samplesReady = 1;
    } else {
    retry:
        //
        // If there is no data available in the file buffer, we'll end up
        // blocking in WMCDecCBGetData. So we don't call WMCDecDecodeData
        // at all and just return empty-handed. This is not foolproof but
        // should help in majority of the cases.
        //
        if (m_FileBuffer.NoDataAvailable())
            goto success;

        wmcerr = WMCDecDecodeData(m_WMC, &readyId, &samplesReady, 0);
        if (wmcerr == WMCDec_DecodeComplete) {
            XDBGTRC("WMVDEC", "Decoding completed: %d", samplesReady);
            hr = S_FALSE;
            goto success;
        }
    }

    if (samplesReady > 0) {
        U32_WMC outputSize = 0;
        I64_WMC timestamp;
        LPCXMEDIAPACKET packet;

        if (readyId == m_AudioStreamInfo.u16StreamId) {
            if( !pAudioOutputPacket ) {
                m_LastStreamId = readyId;
                hr = E_PENDING;
                goto success;
            }

            //
            // Audio stream is ready for output
            //
            I16_WMC* buf = (I16_WMC*) pAudioOutputPacket->pvBuffer;
            U32_WMC remainingSpace = pAudioOutputPacket->dwMaxSize / m_AudioSampleSize;
            U32_WMC samplesDecoded;
            I64_WMC ts;

            while (TRUE) {
                // We only care about the timestamp for the first output sample
                wmcerr = WMCDecGetAudioOutput(
                            m_WMC,
                            buf,
                            NULL,
                            remainingSpace,
                            &samplesDecoded,
                            (outputSize == 0) ? &timestamp : &ts);
                if (wmcerr != WMCDec_Succeeded) goto error;

                if (samplesDecoded == 0) break;
                outputSize += samplesDecoded;
                if ((remainingSpace -= samplesDecoded) == 0) break;
                buf += samplesDecoded;
            }

            *(pAudioOutputPacket->pdwCompletedSize) = outputSize * m_AudioSampleSize;
            packet = pAudioOutputPacket;
        } else if (readyId == m_VideoStreamInfo.u16StreamId) {
            ASSERT( samplesReady == 1 );
            if( !pVideoOutputPacket ) {
                m_LastStreamId = readyId;
                hr = E_PENDING;
                goto success;
            }

            //
            // Video stream is ready for output
            //
            Bool_WMC isKeyFrame;
            wmcerr = WMCDecGetVideoOutput(
                        m_WMC,
                        (U8_WMC*) pVideoOutputPacket->pvBuffer,
                        pVideoOutputPacket->dwMaxSize,
                        &outputSize,
                        &timestamp,
                        &isKeyFrame,
                        WMC_DispRotate0);

            if (wmcerr != WMCDec_Succeeded) goto error;

            ASSERT(outputSize == m_VideoStreamInfo.i32Width *
                                 m_VideoStreamInfo.i32Height *
                                 m_VideoStreamInfo.u16VideoOutputBitCount / 8);

            readyId = STREAMID_NONE;
            *(pVideoOutputPacket->pdwCompletedSize) = outputSize;
            packet = pVideoOutputPacket;
        } else {
            XDBGWRN("WMVDEC", "Unexpected stream is ready for output: %d", readyId);
        }

        if (outputSize) {
            // Convert from milliseconds to 100ns
            if (packet->prtTimestamp)
                *(packet->prtTimestamp) = timestamp * 10000;

            m_LastStreamId = readyId;
            m_NoOutputDataCount = 0;
        } else if (m_LastStreamId != STREAMID_NONE) {
            m_LastStreamId = STREAMID_NONE;
            goto retry;
        }
    } else if (m_NoOutputDataCount++ >= 4) {
        //
        // HACK: This to workaround a bug in the decoder library
        // where WMCDecDecodeData never returns WMCDec_DecodeComplete.
        // So we have a kluge here to assume that the decoding is finished
        // if 4 consecutive calls to WMCDecDecodeData all return no data.
        //
        XDBGWRN("WMVDEC", "No output data, assume end of movie");
        hr = S_FALSE;
    }

success:
    if( pVideoOutputPacket )
        *(pVideoOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_SUCCESS;
    if( pAudioOutputPacket )
        *(pAudioOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_SUCCESS;
    return hr;
    
error:
    XDBGWRN("WMVDEC", "Decode failed: %d", wmcerr);
    if( pVideoOutputPacket )
        *(pVideoOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_FAILURE;
    if( pAudioOutputPacket )
        *(pAudioOutputPacket->pdwStatus) = XMEDIAPACKET_STATUS_FAILURE;
    return MAKE_WMVHRESULT(wmcerr);
}

STDMETHODIMP
CWMVDecoder::Seek(
    PREFERENCE_TIME prtSeekPosition
    )

/*++

Routine Description:

    Seek to the specified time position in the movie

Arguments:

    prtSeekPosition - On entry, this specifies the position to seek to
        on exit, this is updated to reflect the actually position that was seeked to

Return Value:

    Error code

--*/

{
    if (FAILED(m_LastHR))
        return m_LastHR;

    tWMCDecStatus wmcerr;
    U64_WMC seekpos = *prtSeekPosition / 10000;

    if (m_LastStreamId != STREAMID_NONE) {
        XDBGWRN("WMVDEC", "Seeking during playback is not supported");
        return DSERR_UNSUPPORTED;
    }

    wmcerr = WMCDecSeek(m_WMC, seekpos, &seekpos);
    if (wmcerr != WMCDec_Succeeded)
        return MAKE_WMVHRESULT(wmcerr);

    m_NoOutputDataCount = 0;
    *prtSeekPosition = seekpos * 10000;
    return DS_OK;
}

//
// Memory debugging facilities
//

#ifdef WMV_MEMORY_DEBUGGING

#define MAXFILENAMELEN 12
#define TAILPADALIGN sizeof(ULONG_PTR)

struct WmvDebugMemHeader {
    LIST_ENTRY links;
    CHAR filename[MAXFILENAMELEN];
    INT linenum;
    INT size;
    VOID* ptr;
};

INITIALIZED_CRITICAL_SECTION(WmvDebugMemLock);
LIST_ENTRY WmvDebugMemList;
HANDLE WmvDebugMemHeap;

void WmvDebugMemInit()
{
    EnterCriticalSection(&WmvDebugMemLock);
    if (!WmvDebugMemHeap) {
        WmvDebugMemHeap = GetProcessHeap();
        InitializeListHead(&WmvDebugMemList);
    }
    LeaveCriticalSection(&WmvDebugMemLock);
}

void* WmvDebugAlloc(char* filename, int linenum, size_t size)
{
    size_t allocsize = sizeof(WmvDebugMemHeader) +
                       ((size + (TAILPADALIGN-1)) & ~(TAILPADALIGN-1)) +
                       TAILPADALIGN;

    WmvDebugMemHeader* entry = (WmvDebugMemHeader*) HeapAlloc(WmvDebugMemHeap, 0, allocsize);
    if (!entry) return NULL;

    char* basename = strrchr(filename, '\\');
    if (basename)
        basename += 1;
    else
        basename = filename;

    strncpy(entry->filename, basename, MAXFILENAMELEN);
    entry->linenum = linenum;
    entry->size = size;
    entry->ptr = entry + 1;

    ULONG_PTR tailmagic = (ULONG_PTR) entry->ptr + entry->size;
    ULONG_PTR* tail = (ULONG_PTR*) entry->ptr + (size / TAILPADALIGN);

    if (size % TAILPADALIGN) {
        *tail++ = tailmagic;
        tailmagic += TAILPADALIGN;
    }
    *tail = tailmagic;

    EnterCriticalSection(&WmvDebugMemLock);
    InsertHeadList(&WmvDebugMemList, &entry->links);
    LeaveCriticalSection(&WmvDebugMemLock);

    return entry->ptr;
}

#define WMVDEBUGFREEASSERT(cond) do { \
            if (!(cond)) { \
                DbgPrint("!!!!!!!! Memory corruption:\n"); \
                DbgPrint("  %s\n", #cond); \
                DbgPrint("  ptr = %x, entry = %x\n", ptr, entry); \
                __asm { int 3 }; \
            } \
        } while (0)

void WmvDebugFree(void* ptr)
{
    if (!ptr) return;

    EnterCriticalSection(&WmvDebugMemLock);

    WmvDebugMemHeader* entry = (WmvDebugMemHeader*) ptr - 1;
    WMVDEBUGFREEASSERT(entry->ptr == ptr);

    LIST_ENTRY* preventry = (LIST_ENTRY*) entry->links.Blink;
    WMVDEBUGFREEASSERT(preventry->Flink == (void*) entry);

    LIST_ENTRY* nextentry = (LIST_ENTRY*) entry->links.Flink;
    WMVDEBUGFREEASSERT(nextentry->Blink == (void*) entry);

    ULONG_PTR tailmagic = (ULONG_PTR) entry->ptr + entry->size;
    ULONG_PTR* tail = (ULONG_PTR*) entry->ptr + (entry->size / TAILPADALIGN);

    if (entry->size % TAILPADALIGN) {
        ULONG_PTR mask = -1 << (TAILPADALIGN - entry->size%TAILPADALIGN)*8;
        WMVDEBUGFREEASSERT((*tail & mask) == (tailmagic & mask));
        tail++;
        tailmagic += TAILPADALIGN;
    }

    WMVDEBUGFREEASSERT(*tail == tailmagic);

    RemoveEntryList(&entry->links);

    tail[-1] = *tail = 0;
    entry->ptr = NULL;
    entry->links.Flink = entry->links.Blink = NULL;
    LeaveCriticalSection(&WmvDebugMemLock);

    HeapFree(WmvDebugMemHeap, 0, entry);
}

void* WmvDebugRealloc(char* filename, int linenum, void* ptr, size_t size)
{
    // We assume this is called very infrequently
    //  just free the old memory and realloc new memory
    WmvDebugFree(ptr);
    return WmvDebugAlloc(filename, linenum, size);
}

#endif // WMV_MEMORY_DEBUGGING

