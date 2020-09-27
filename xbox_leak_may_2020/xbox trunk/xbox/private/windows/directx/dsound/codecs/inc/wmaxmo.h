/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wmaxmo.h
 *  Content:    WMA XMO API.
 *  History:
 *   Date       By        Reason
 *   ====       ==        ======
 *  07/26/00    jpalevic  Created.
 *
 ****************************************************************************/

#ifndef __WMAXMO_H__
#define __WMAXMO_H__

#include <dsound.h>
#include "wma_dec_win32_x86.h"

//
// Xbox WMA decompressor interface
//

DWORD WINAPI WmaXmoThreadProc(
   LPVOID lpParameter   // thread data
);


// You must provide at least these many bytes of valid header data when
// initializing the CWMAMediaObject. (Of course, if the file is smaller
// than this size, just provide as many bytes as are in the file.)

#define WMAMO_HEADER_SIZE 2048


typedef struct _WMAXMO_ASYNC_CONTEXT {

    LIST_ENTRY ListEntry;
    PVOID pThis;
    XMEDIAPACKET Xmp;

} WMAXMO_ASYNC_CONTEXT, *PWMAXMO_ASYNC_CONTEXT;

VOID
WMAAsyncFileReadCallback (
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
    );

class CWMAXMediaObject: public XWmaFileMediaObject {

public:
    CWMAXMediaObject();
    ~CWMAXMediaObject();

    HRESULT STDMETHODCALLTYPE Initialize(
        PCSTR pszFileName,
        HANDLE hFile,
        BOOL fAsyncMode,
        DWORD dwLookahead,
        DWORD dwMaxPackets,
        DWORD dwYieldRate,
        WAVEFORMATEX* pWave /* Pass NULL if you don't care */
		);

    HRESULT InitializeIntermediate(
        LPFNWMAXMODATACALLBACK     pCallback,
        PVOID                      pCallbackContext,
        DWORD                      dwYieldRate,
        LPWAVEFORMATEX             pwfxCompressed
        );

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
       long l = InterlockedDecrement((long*)&m_cRef);
       if (l == 0)
          delete this;
       return l;
    }

    HRESULT STDMETHODCALLTYPE Process( 
        LPCXMEDIAPACKET pSrcBuffer,
        LPCXMEDIAPACKET pDstBuffer
        );
    
    HRESULT STDMETHODCALLTYPE Decode( 
        LPCXMEDIAPACKET pDstBuffer
        );

	HRESULT STDMETHODCALLTYPE GetInfo(
			LPXMEDIAINFO pInfo
		);

    HRESULT STDMETHODCALLTYPE Flush();
    HRESULT STDMETHODCALLTYPE Discontinuity(void);

    HRESULT STDMETHODCALLTYPE GetStatus( 
            LPDWORD pdwStatus 
		);

	// Used in the implementation of the codec

	DWORD GetData(DWORD offset, DWORD num_bytes,
		void** ppData);

    HRESULT STDMETHODCALLTYPE Seek(LONG lOffset, DWORD dwOrigin, LPDWORD pdwAbsolute);
    HRESULT STDMETHODCALLTYPE GetLength(LPDWORD pdwLength);

    HRESULT STDMETHODCALLTYPE GetFileContentDescription(WMAXMOFileContDesc* pContentDesc);

    HRESULT STDMETHODCALLTYPE GetFileHeader(WMAXMOFileHeader *pFileHeader) {

        ASSERT(pFileHeader);
        memset(pFileHeader,0,sizeof(WMAXMOFileHeader));

        pFileHeader->dwVersion = m_hdr.version;
        pFileHeader->dwSampleRate = m_hdr.sample_rate;
        pFileHeader->dwNumChannels = m_hdr.num_channels;
        pFileHeader->dwDuration = m_hdr.duration;
        pFileHeader->dwBitrate = m_hdr.bitrate;

        return S_OK;
    }

private:

    VOID Reset();

    __inline DWORD WMASampleRateToHz(tWMAFileSampleRate sampleRate);
    HRESULT STDMETHODCALLTYPE GetFormat(
		WAVEFORMATEX* pWave);

    HRESULT ReadWmaDataFromFile(
        PVOID pvBuffer,
        DWORD dwSize,
        PWMAXMO_ASYNC_CONTEXT   pContext
        );

    friend VOID
    WMAAsyncFileReadCallback (
        IN PVOID ApcContext,
        IN PIO_STATUS_BLOCK IoStatusBlock,
        IN ULONG Reserved
        );

	tWMAFileHeader m_hdr;
	tHWMAFileState m_state;

    //
	// Used by the CBGetData callback
    //

    PUCHAR m_pSrcBuffer; 

    //
    // Used in async mode
    //

    PUCHAR m_pContextPool;

    DWORD  m_dwSrcBufferOffset;
    DWORD  m_dwCurrentWmaFileOffset;

    //
    // used for the in-memory decoder instance
    //

    LPFNWMAXMODATACALLBACK  m_pCallback;
    PVOID                   m_pCallbackContext;


protected:

    friend DWORD WINAPI WmaXmoThreadProc(
      LPVOID lpParameter   // thread data
    );

    VOID ThreadProc();

    HANDLE m_hDeleteThreadEvent;
    HANDLE m_hFile;
    OVERLAPPED m_overlapped;

    DWORD m_dwThreadId;
    CRITICAL_SECTION m_CS;
    HANDLE m_hThread;


    ULONG m_cRef;	// Reference count
    DWORD m_dwPendingPackets;
    DWORD m_dwLookaheadSize;
    DWORD m_dwMaxContexts;

    LIST_ENTRY m_AvailableList;
    LIST_ENTRY m_PendingList;

    BOOL m_bExternalHandle;
	BOOL m_bDecodeMore;
    BOOL m_bAsyncMode;

    DWORD m_dwYieldRate;

};


#endif // __WMAXMO_H__
