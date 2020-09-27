//
//  Based on source from Win2k ole32.dll
//

#pragma once

#include <sem.hxx>

/*
 * MemStm APIs
 */

STDAPI_(void) 		ReleaseMemStm(LPHANDLE phMem, BOOL fInternalOnly);

//+-------------------------------------------------------------------------
//
//  Class:  	MEMSTM
//
//  Purpose:    A structure to describe the global memroy
//
//  Interface:
//
//  History:    dd-mmm-yy Author    Comment
//		09-Nov-93 alexgo    32bit port
//
//  Notes:
//
// cRef counts all CMemStm pointers to this MEMSTM plus the number of times
// a hMem handle to MEMSTM had been returned
//
//--------------------------------------------------------------------------

struct MEMSTM
{     	// Data in shared memory
    	DWORD  cb;              // Size of hGlobal
    	DWORD  cRef;            // See below
#ifdef NOTSHARED
    	HANDLE hGlobal;         // The data
#else
	BYTE * m_pBuf;
	HANDLE hGlobal;
#endif
	
	BOOL   fDeleteOnRelease;
};

#define STREAM_SIG (0x4d525453L)

//+-------------------------------------------------------------------------
//
//  Class: 	CMemStm
//
//  Purpose:    IStream on memory (shared mem for win16)
//
//  Interface:  IStream
//
//  History:    dd-mmm-yy Author    Comment
//		02-Dec-93 alexgo    32bit port
//
//  Notes:
//
// CMemStm is a stream implementation on top of global shared memory MEMSTM
//
// CMemStm
// +---------+
// + pvtf    +    Shared  memory
// +---------+   +--------------+
// + m_pMem  +-->|cb            |
// +---------+   |cRef          |
//               |hGlobal       |--->+--------------+
//               +--------------+	 | Actual Data	|
// CMemStm             MEMSTM		 +--------------+
//--------------------------------------------------------------------------
class FAR CMemStm : public IStream
{
public:
	STDMETHOD(QueryInterface) (REFIID iidInterface, void **ppvObj);
    	STDMETHOD_(ULONG,AddRef) (void);
    	STDMETHOD_(ULONG,Release) (void);
    	STDMETHOD(Read) (VOID HUGEP* pv, ULONG cb, ULONG FAR* pcbRead);
	STDMETHOD(Write) (VOID const HUGEP* pv, ULONG cb, ULONG *pcbWritten);
    	STDMETHOD(Seek) (LARGE_INTEGER dlibMove, DWORD dwOrigin,
			 ULARGE_INTEGER *plibNewPosition);
    	STDMETHOD(SetSize) (ULARGE_INTEGER cb);
	STDMETHOD(CopyTo) (IStream *pstm, ULARGE_INTEGER cb,
			ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
    	STDMETHOD(Commit) (DWORD grfCommitFlags);
    	STDMETHOD(Revert) (void);
    	STDMETHOD(LockRegion) (ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
    			DWORD dwLockType);
    	STDMETHOD(UnlockRegion) (ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
    			DWORD dwLockType);
	STDMETHOD(Stat) (STATSTG *pstatstg, DWORD statflag);
	STDMETHOD(Clone)(IStream **ppstm);

    	static CMemStm FAR* Create(HANDLE hMem);

	CMemStm();
	~CMemStm();

private:
 	DWORD 		m_dwSig;	// Signature indicating this is our
					// implementation of
					// IStream: STREAM_SIG
    	ULONG 		m_refs;  	// Number of references to this CmemStm
    	ULONG 		m_pos;   	// Seek pointer for Read/Write
    	HANDLE 		m_hMem; 	// Memory Handle passed on creation
    	MEMSTM 		FAR* m_pData;   // Pointer to that memroy
	CMutexSem	m_mxs;		// mutex for MultiThread protection
};

