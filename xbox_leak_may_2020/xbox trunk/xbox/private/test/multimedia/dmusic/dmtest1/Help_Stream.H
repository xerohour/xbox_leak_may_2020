/****************************************************************************

    Contains the definition for IStream interface

    Copyright (C) Microsoft Corporation  1994 - 1999
    
    Stolen and Modified 3/5/96 by Mark Burton from 2/21/95 Tom Laird-McConnell

    07/28/1998 - t-jwex - added unicode support to the methods/functions contained
						here.  Also updated all old 16-bit File I/O calls to 
						respective Win32 calls.
    08/13/1998 - jimmo - Changed CFileStream::Read prototype
    11/11/1999 - danhaff - copied into dsapitst8.

*******************************************************************************/

#pragma once

namespace LocalFileStream
{

enum STREAM_MODES 
{
	STREAM_WRITE			= 	1,		
	STREAM_READ				= 	2,		
};

HRESULT AllocStreamFromFile( LPSTREAM *ppStream, 
                            LPCSTR szAbsFilePath, 
							DWORD dwOffset );
// **************************************
//
// CFileStream
//
// Implementation of IStream interface, and IUnknown
//
// **************************************
class CFileStream : public IStream
{
private:
    int     m_RefCnt;
    char    m_szFileName[MAX_PATH];

	HANDLE	m_hFile;
    DWORD   m_dwStartOffset;
    DWORD   m_dwLength;
    DWORD	m_fdwFlags;
	BOOL	m_fInited;

//    IStream *m_pCachedClone;
//    IStream *m_pOriginal;

public:

	CFileStream(HANDLE hfile) :m_hFile(hfile){ AddRef();}
	CFileStream(void);
    ~CFileStream();

    STDMETHODIMP	Init( const char *szAbsFilePath, DWORD dwOffset, DWORD fdwFlags );
	
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG,AddRef) (THIS) ;
    STDMETHOD_(ULONG,Release) (THIS);

    // *** IStream methods ***
    STDMETHOD(Read) (void* pv, ULONG cb, ULONG* pcbRead );

    STDMETHOD(Write) (THIS_ VOID const HUGEP *pv,
            ULONG cb,
            ULONG FAR *pcbWritten);
    STDMETHOD(Seek) (THIS_ LARGE_INTEGER dlibMove,
               DWORD dwOrigin,
               ULARGE_INTEGER FAR *plibNewPosition);
    STDMETHOD(SetSize) (THIS_ ULARGE_INTEGER libNewSize);
    STDMETHOD(CopyTo) (THIS_ IStream FAR *pstm,
             ULARGE_INTEGER cb,
             ULARGE_INTEGER FAR *pcbRead,
             ULARGE_INTEGER FAR *pcbWritten);
    STDMETHOD(Commit) (THIS_ DWORD grfCommitFlags);
    STDMETHOD(Revert) (THIS);
    STDMETHOD(LockRegion) (THIS_ ULARGE_INTEGER libOffset,
                 ULARGE_INTEGER cb,
                 DWORD dwLockType);
    STDMETHOD(UnlockRegion) (THIS_ ULARGE_INTEGER libOffset,
                 ULARGE_INTEGER cb,
                 DWORD dwLockType);
    STDMETHOD(Stat) (THIS_ STATSTG FAR *pstatstg, DWORD grfStatFlag);
    STDMETHOD(Clone)(THIS_ IStream FAR * FAR *ppstm);

//    STDMETHOD(GetMyClone)(THIS_ IStream FAR * FAR *ppstm){*ppstm = m_pClone; return S_OK;};
};

}//end namespace
