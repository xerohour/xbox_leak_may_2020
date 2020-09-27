/****************************************************************************

    Contains the definition for IStream interface

    Copyright (c) Microsoft Corporation  1994,  1995
    
    Stolen and Modified 3/5/96 by Mark Burton from 2/21/95 Tom Laird-McConnell

*******************************************************************************/

#ifndef _CStreams_
#define _CStreams_

#include "riffstrm.h"

STDAPI AllocStreamToFile(LPSTREAM FAR *ppStream, const char *pAbsFilePath, DWORD dwOffset);
STDAPI AllocStreamFromFile(LPSTREAM FAR *ppStream, const char *pAbsFilePath, DWORD dwOffset);
enum STREAM_MODES 
{
	STREAM_WRITE			= 	1,		
	STREAM_READ				= 	2,		
};

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
    char    m_szFileName[255];
    
    HFILE   m_hFile;
    DWORD   m_dwStartOffset;
    DWORD   m_dwLength;
    DWORD	m_fdwFlags;
	BOOL	m_fInited;

public:
    CFileStream(void);
    ~CFileStream();

    STDMETHODIMP	Init(const char *pAbsFilePath,DWORD dwOffset,DWORD fdwFlags);

    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR* ppvObj);
    STDMETHOD_(ULONG,AddRef) (THIS) ;
    STDMETHOD_(ULONG,Release) (THIS);

    // *** IStream methods ***
    STDMETHOD(Read) (THIS_ VOID HUGEP *pv,
                     ULONG cb, ULONG FAR *pcbRead);
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
};

#endif
