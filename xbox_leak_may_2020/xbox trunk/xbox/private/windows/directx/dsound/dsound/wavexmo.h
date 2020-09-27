/***************************************************************************
 *
 *  Copyright (C) 12/10/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       wavexmo.cpp
 *  Content:    Wave file XMO wrappers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/10/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WAVEXMO_H__
#define __WAVEXMO_H__

#ifdef __cplusplus

//
// Wave file XMO
//

namespace DirectSound
{
    class CWaveFileMediaObject
        : public XWaveFileMediaObject, public CRefCount, protected CWaveFile
    {
    protected:
        DWORD                   m_dwReadOffset;     // Current read offset

    public:
        CWaveFileMediaObject(void);
        virtual ~CWaveFileMediaObject(void);

    public:
        // Initialization
        HRESULT STDMETHODCALLTYPE Initialize(LPCSTR pszFileName);
        HRESULT STDMETHODCALLTYPE Initialize(HANDLE hFile);

        // IUnknown methods
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

        // XMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
        virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
        virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pxmbInput, LPCXMEDIAPACKET pxmbOutput);
        virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
        virtual HRESULT STDMETHODCALLTYPE Flush(void);

        // XFileMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE Seek(LONG lOffset, DWORD dwOrigin, LPDWORD pdwAbsolute);
        virtual HRESULT STDMETHODCALLTYPE GetLength(LPDWORD pdwLength);

        // XWaveFileMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE GetFormat(LPCWAVEFORMATEX *ppwfxFormat);
        virtual HRESULT STDMETHODCALLTYPE GetLoopRegion(LPDWORD pdwLoopStart, LPDWORD pdwLoopLength);
    };

    __inline ULONG CWaveFileMediaObject::AddRef(void)
    {
        return CRefCount::AddRef();
    }

    __inline ULONG CWaveFileMediaObject::Release(void)
    {
        return CRefCount::Release();
    }

    __inline HRESULT CWaveFileMediaObject::Initialize(LPCSTR pszFileName)
    {
        return CWaveFile::Open(pszFileName, NULL);
    }

    __inline HRESULT CWaveFileMediaObject::Initialize(HANDLE hFile)
    {
        return CWaveFile::Open(NULL, hFile);
    }

    __inline HRESULT CWaveFileMediaObject::GetStatus(LPDWORD pdwStatus)
    {
        *pdwStatus = XMO_STATUSF_ACCEPT_OUTPUT_DATA;
        return DS_OK;
    }

    __inline HRESULT CWaveFileMediaObject::Discontinuity(void)
    {
        return S_OK;
    }

    __inline HRESULT CWaveFileMediaObject::Flush(void)
    {
        return Seek(0, FILE_BEGIN, NULL);
    }

    __inline HRESULT CWaveFileMediaObject::GetLength(LPDWORD pdwLength)
    {
        return CWaveFile::GetDuration(pdwLength);
    }

    __inline HRESULT CWaveFileMediaObject::GetFormat(LPCWAVEFORMATEX *ppwfxFormat)
    {
        return CWaveFile::GetFormat(ppwfxFormat);
    }

    __inline HRESULT CWaveFileMediaObject::GetLoopRegion(LPDWORD pdwLoopStart, LPDWORD pdwLoopLength)
    {
        return CWaveFile::GetLoopRegion(pdwLoopStart, pdwLoopLength);
    }
}

//
// File XMO
//

namespace DirectSound
{
    class CFileMediaObject
        : public XFileMediaObject, public CRefCount, protected CStdFileStream
    {
    public:
        CFileMediaObject(void);
        virtual ~CFileMediaObject(void);

    public:
        // Initialization
        HRESULT STDMETHODCALLTYPE Initialize(LPCSTR pszFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDistribution, DWORD dwFlagsAndAttributes);
        HRESULT STDMETHODCALLTYPE Initialize(HANDLE hFile);

        // IUnknown methods
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

        // XMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
        virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
        virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pxmbInput, LPCXMEDIAPACKET pxmbOutput);
        virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
        virtual HRESULT STDMETHODCALLTYPE Flush(void);

        // XFileMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE Seek(LONG lOffset, DWORD dwOrigin, LPDWORD pdwAbsolute);
        virtual HRESULT STDMETHODCALLTYPE GetLength(LPDWORD pdwLength);
    };

    __inline ULONG CFileMediaObject::AddRef(void)
    {
        return CRefCount::AddRef();
    }

    __inline ULONG CFileMediaObject::Release(void)
    {
        return CRefCount::Release();
    }

    __inline HRESULT CFileMediaObject::Initialize(LPCSTR pszFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
    {
        return CStdFileStream::Open(pszFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes);
    }

    __inline HRESULT CFileMediaObject::Initialize(HANDLE hFile)
    {
        CStdFileStream::Attach(hFile);
        return DS_OK;
    }

    __inline HRESULT CFileMediaObject::GetStatus(LPDWORD pdwStatus)
    {
        *pdwStatus = XMO_STATUSF_ACCEPT_INPUT_DATA | XMO_STATUSF_ACCEPT_OUTPUT_DATA;
        return DS_OK;
    }

    __inline HRESULT CFileMediaObject::Discontinuity(void)
    {
        return S_OK;
    }

    __inline HRESULT CFileMediaObject::Flush(void)
    {
        return Seek(0, FILE_BEGIN, NULL);
    }

    __inline HRESULT CFileMediaObject::Seek(LONG lOffset, DWORD dwOrigin, LPDWORD pdwAbsolute)
    {
        return CStdFileStream::Seek(lOffset, dwOrigin, pdwAbsolute);
    }

    __inline HRESULT CFileMediaObject::GetLength(LPDWORD pdwLength)
    {
        return CStdFileStream::GetLength(pdwLength);
    }
}

#endif // __cplusplus

#endif // __WAVEXMO_H__
