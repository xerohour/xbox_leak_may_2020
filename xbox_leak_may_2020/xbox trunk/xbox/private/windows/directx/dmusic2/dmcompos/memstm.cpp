//
//  Based on source from Win2k ole32.dll
//

#include "pchcompos.h"

#undef Assert
#define Assert(x) assert(x)

# define _xmemset memset
# define _xmemcpy memcpy
# define _xmemcmp memcmp
# define _xmemmove memmove

CMemStm::CMemStm()
{
        // mutex automatically initialized
        m_hMem = NULL;
        m_pData = NULL;
        m_pos = 0;
        m_refs = 0;
}

CMemStm::~CMemStm()
{
        // empty destructor, mutex automatically cleaned up
}


STDMETHODIMP CMemStm::QueryInterface(REFIID iidInterface,
        void FAR* FAR* ppvObj)
{
        HRESULT         error;

        *ppvObj = NULL;

        // Two interfaces supported: IUnknown, IStream

        if (m_pData != NULL && (IsEqualIID(iidInterface, IID_IStream) ||
                IsEqualIID(iidInterface, IID_ISequentialStream) ||
                IsEqualIID(iidInterface, IID_IUnknown)))
        {

                AddRef();   // A pointer to this object is returned
                *ppvObj = this;
                error = NOERROR;
        }
        else
        {                 // Not accessible or unsupported interface
                *ppvObj = NULL;
                error = E_NOINTERFACE;
        }

        return error;
}

STDMETHODIMP_(ULONG) CMemStm::AddRef(void)
{
        return InterlockedIncrement((LONG *) &m_refs);
}

STDMETHODIMP_(ULONG) CMemStm::Release(void)
{
        // The reason for this here is that there is a race when releasing
        // this object. If two threads are trying to release this object
        // at the same time, there is a case where the first one dec's
        // the ref count & then loses the processor to the second thread.
        // This second thread decrements the reference count to 0 and frees
        // the memory. The first thread can no longer safely examine the
        // internal state of the object.
        ULONG ulResult = InterlockedDecrement((LONG *) &m_refs);

        if (ulResult == 0)
        {
                // this MEMSTM handle was GlobalLock'ed in ::Create
                // we unlock it here, as we no longer need it.
                GlobalUnlock(m_hMem);

                ReleaseMemStm(&m_hMem, FALSE);

                delete this;
        }

        return ulResult;
}

STDMETHODIMP CMemStm::Read(void HUGEP* pb, ULONG cb, ULONG FAR* pcbRead)
{
        HRESULT         error = NOERROR;
        ULONG           cbRead = cb;

        if(cb)
        {
            // VDATEPTROUT( pb, char);
        }

        // Single thread
        CLock lck(m_mxs);

        if (pcbRead)
        {
                // VDATEPTROUT( pcbRead, ULONG );
                *pcbRead = 0L;
        }

	// cbRead + m_pos could cause roll-over.
        if ( ( (cbRead + m_pos) > m_pData->cb) || ( (cbRead + m_pos) < m_pos) )
        {
                // Caller is asking for more bytes than we have left
                if(m_pData->cb > m_pos)
                    cbRead = m_pData->cb - m_pos;
                else
                    cbRead = 0;
        }

        if (cbRead > 0)
        {
                assert(m_pData->hGlobal);
                BYTE HUGEP* pGlobal = (BYTE HUGEP *)GlobalLock(
                        m_pData->hGlobal);
                if (NULL==pGlobal)
                {
                        // LEERROR(1, "GlobalLock Failed!");

                        return STG_E_READFAULT;
                }
                // overlap is currently considered a bug (see the discussion
                // on the Write method
                _xmemcpy(pb, pGlobal + m_pos, cbRead);
                GlobalUnlock (m_pData->hGlobal);
                m_pos += cbRead;
        }

        if (pcbRead != NULL)
        {
                *pcbRead = cbRead;
        }

        return error;
}

STDMETHODIMP CMemStm::Write(void const HUGEP* pb, ULONG cb,
        ULONG FAR* pcbWritten)
{
        HRESULT                 error = NOERROR;
        ULONG                   cbWritten = cb;
        ULARGE_INTEGER          ularge_integer;
        BYTE HUGEP*             pGlobal;

        if(cb)
        {
            // VDATEPTRIN( pb , char );
        }

        // Single thread
        CLock lck(m_mxs);

        if (pcbWritten != NULL)
        {
                *pcbWritten = 0;
        }

        if (cbWritten + m_pos > m_pData->cb)
        {
                ULISet32( ularge_integer, m_pos+cbWritten );
                error = SetSize(ularge_integer);
                if (error != NOERROR)
                {
                        goto Exit;
                }
        }

        // we don't write anything if 0 bytes are asked for for two
        // reasons: 1. optimization, 2. m_pData->hGlobal could be a
        // handle to a zero-byte memory block, in which case GlobalLock
        // will fail.

        if( cbWritten > 0 )
        {
                pGlobal = (BYTE HUGEP *)GlobalLock (m_pData->hGlobal);
                if (NULL==pGlobal)
                {
                        // LEERROR(1, "GlobalLock Failed!");

                        return ResultFromScode (STG_E_WRITEFAULT);
                }

                // we use memmove here instead of memcpy to handle the
                // overlap case.  Recall that the app originally gave
                // use the memory for the memstm.  He could (either through
                // a CopyTo or through really strange code), be giving us
                // this region to read from, so we have to handle the overlapp
                // case.  The same argument also applies for Read, but for
                // now, we'll consider overlap on Read a bug.
                _xmemmove(pGlobal + m_pos, pb, cbWritten);
                GlobalUnlock (m_pData->hGlobal);

                m_pos += cbWritten;
        }

        if (pcbWritten != NULL)
        {
                *pcbWritten = cbWritten;
        }

Exit:

        return error;
}

STDMETHODIMP CMemStm::Seek(LARGE_INTEGER dlibMoveIN, DWORD dwOrigin,
        ULARGE_INTEGER FAR* plibNewPosition)
{
        HRESULT                 error  = NOERROR;
        LONG                    dlibMove = dlibMoveIN.LowPart ;
        ULONG                   cbNewPos = dlibMove;

        // Single thread
        CLock lck(m_mxs);

        if (plibNewPosition != NULL)
        {
                // VDATEPTROUT( plibNewPosition, ULONG );
                ULISet32(*plibNewPosition, m_pos);
        }

        switch(dwOrigin)
        {

        case STREAM_SEEK_SET:
                if (dlibMove >= 0)
                {
                        m_pos = dlibMove;
                }
                else
                {
                        error = STG_E_SEEKERROR;
                }

                break;

        case STREAM_SEEK_CUR:
                if (!(dlibMove < 0 && ((ULONG) -dlibMove) > m_pos))
                {
                        m_pos += dlibMove;
                }
                else
                {
                        error = STG_E_SEEKERROR;
                }
                break;

        case STREAM_SEEK_END:
                if (!(dlibMove < 0 && ((ULONG) -dlibMove) > m_pData->cb))
                {
                        m_pos = m_pData->cb + dlibMove;
                }
                else
                {
                        error = STG_E_SEEKERROR;
                }
                break;

        default:
                error = STG_E_SEEKERROR;
        }

        if (plibNewPosition != NULL)
        {
                ULISet32(*plibNewPosition, m_pos);
        }

        return error;
}

STDMETHODIMP CMemStm::SetSize(ULARGE_INTEGER cb)
{
        HANDLE hMemNew;

        // Single thread
        CLock lck(m_mxs);

        // make sure we aren't in overflow conditions.

        assert(cb.HighPart == 0);

        if (m_pData->cb == cb.LowPart)
        {
                return NOERROR;
        }

        hMemNew = GlobalReAlloc(m_pData->hGlobal, max (cb.LowPart,1),
                        GMEM_SHARE | GMEM_MOVEABLE);

        if (hMemNew == NULL)
        {
                return E_OUTOFMEMORY;
        }

        m_pData->hGlobal = hMemNew;
        m_pData->cb = cb.LowPart;

        return NOERROR;
}

STDMETHODIMP CMemStm::CopyTo(IStream FAR *pstm, ULARGE_INTEGER cb,
        ULARGE_INTEGER FAR * pcbRead, ULARGE_INTEGER FAR * pcbWritten)
{
        ULONG   cbRead          = cb.LowPart;
        ULONG   cbWritten       = 0;
        HRESULT hresult         = NOERROR;

        // pstm cannot be NULL

        // VDATEPTRIN(pstm, LPSTREAM);

        // Single thread
        CLock lck(m_mxs);

        // the spec says that if cb is it's maximum value (all bits set,
        // since it's unsigned), then we will simply read the copy of
        // this stream

        if ( ~(cb.LowPart) == 0 && ~(cb.HighPart) == 0 )
        {
                cbRead = m_pData->cb - m_pos;
        }
        else if ( cb.HighPart > 0 )
        {
                // we assume that our memory stream cannot
                // be large enough to accomodate very large (>32bit)
                // copy to requests.  Since this is probably an error
                // on the caller's part, we assert.

                assert(0);

                // set the Read value to what's left, so that "Ignore"ing
                // the assert works properly.

                cbRead = m_pData->cb - m_pos;
        }
        else if ( cbRead + m_pos > m_pData->cb )
        {
                // more bytes were requested to read than we had left.
                // cbRead is set to the amount remaining.

                cbRead = m_pData->cb - m_pos;
        }

        // now write the data to the stream

        if ( cbRead > 0 )
        {
                BYTE HUGEP* pGlobal = (BYTE HUGEP *)GlobalLock(
                                m_pData->hGlobal);

                if( pGlobal == NULL )
                {
                        // LEERROR(1, "GlobalLock failed");

                        return STG_E_INSUFFICIENTMEMORY;
                }

                hresult = pstm->Write(pGlobal + m_pos, cbRead, &cbWritten);

                // in the error case, the spec says that the return values
                // may be meaningless, so we do not need to do any special
                // error handling here

                GlobalUnlock(m_pData->hGlobal);
        }

        // increment our seek pointer and set the out parameters

        m_pos += cbRead;

        if( pcbRead )
        {
                ULISet32(*pcbRead, cbRead);
        }

        if( pcbWritten )
        {
                ULISet32(*pcbWritten, cbWritten);
        }

        return hresult;

}

STDMETHODIMP CMemStm::Commit(DWORD grfCommitFlags)
{
        return NOERROR;
}

STDMETHODIMP CMemStm::Revert(void)
{
        return NOERROR;
}

STDMETHODIMP CMemStm::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb,
        DWORD dwLockType)
{
        return STG_E_INVALIDFUNCTION;
}

STDMETHODIMP CMemStm::UnlockRegion(ULARGE_INTEGER libOffset,
        ULARGE_INTEGER cb, DWORD dwLockType)
{
        return STG_E_INVALIDFUNCTION;
}

STDMETHODIMP CMemStm::Stat(STATSTG FAR *pstatstg, DWORD statflag)
{
        // VDATEPTROUT( pstatstg, STATSTG );

        memset ( pstatstg, 0, sizeof(STATSTG) );

        pstatstg->type                  = STGTY_STREAM;
        pstatstg->cbSize.LowPart        = m_pData->cb;

        return NOERROR;
}

STDMETHODIMP CMemStm::Clone(IStream FAR * FAR *ppstm)
{
        CMemStm FAR*    pCMemStm;

        // VDATEPTROUT (ppstm, LPSTREAM);

        *ppstm = pCMemStm = CMemStm::Create(m_hMem);

        if (pCMemStm == NULL)
        {
                return E_OUTOFMEMORY;
        }

        pCMemStm->m_pos = m_pos;

        return NOERROR;
}

CMemStm FAR* CMemStm::Create(HANDLE hMem)
{
        CMemStm FAR* pCMemStm = NULL;
        struct MEMSTM FAR* pData;

        pData = (MEMSTM FAR*) GlobalLock(hMem);

        if (pData != NULL)
        {
                pCMemStm = new CMemStm;

                if (pCMemStm != NULL)
                {
                        // Initialize CMemStm
                        pCMemStm->m_hMem = hMem;
                        (pCMemStm->m_pData = pData)->cRef++; // AddRefMemStm
                        pCMemStm->m_refs = 1;
                        pCMemStm->m_dwSig = STREAM_SIG;
                }
                else
                {
                        // uh-oh, low on memory
                        GlobalUnlock(hMem);
                }
        }

        // we do *not* unlock the memory now, the memstm structure should
        // be locked for the lifetime of any CMemStm's that refer to it.
        // when the CMemStm is destroyed, we will release our lock on
        // hMem.

        return pCMemStm;
}


STDAPI_(void) ReleaseMemStm (LPHANDLE phMem, BOOL fInternalOnly)
{
        struct MEMSTM FAR*      pData;

        pData = (MEMSTM FAR*) GlobalLock(*phMem);

        // check for NULL pointer in case handle got freed already
        // decrement ref count and free if no refs left
        if (pData != NULL && --pData->cRef == 0)
        {
                if (pData->fDeleteOnRelease)
                {
                        Verify (0==GlobalFree (pData->hGlobal));
                }

                if (!fInternalOnly)
                {
                        GlobalUnlock(*phMem);
                        Verify (0==GlobalFree(*phMem));
                        goto End;
                }
        }

        GlobalUnlock(*phMem);
End:
        *phMem = NULL;
}

#ifdef XBOX
STDAPI DMusic_CreateStreamOnHGlobal(HANDLE hGlobal, BOOL fDeleteOnRelease,
        LPSTREAM FAR* ppstm)
#else
STDAPI CreateStreamOnHGlobal(HANDLE hGlobal, BOOL fDeleteOnRelease,
        LPSTREAM FAR* ppstm)
#endif
{
        HANDLE                  hMem      = NULL;
        struct MEMSTM FAR*      pData     = NULL;
        LPSTREAM                pstm      = NULL;
        DWORD                   cbSize;
        HRESULT hresult;

        // VDATEPTROUT_LABEL (ppstm, LPSTREAM, SafeExit, hresult);

        *ppstm = NULL;

        if (NULL==hGlobal)
        {
#ifdef XBOX
            // XBox won't allow GMEM_MOVEABLE or GMEM_DISCARDABLE. Sharing is irrelevant here.
                hGlobal = GlobalAlloc(0, 0);
#else
                hGlobal = GlobalAlloc(GMEM_SHARE | GMEM_MOVEABLE, 0);
#endif
                if (hGlobal == NULL)
                {
                        goto ErrorExit;
                }
                cbSize = 0;
        }
        else
        {
                cbSize = (ULONG) GlobalSize (hGlobal);
                // Is there a way to verify a zero-sized handle?
                // we currently do no verification for them
                if (cbSize!=0)
                {
                        // verify validity of passed-in handle
                        if (NULL==GlobalLock(hGlobal))
                        {
                                // bad handle
                                hresult = E_INVALIDARG;
                                goto SafeExit;
                        }
                        GlobalUnlock (hGlobal);
                }
        }

#ifdef XBOX
            // XBox won't allow GMEM_MOVEABLE or GMEM_DISCARDABLE. Sharing is irrelevant here.
        hMem = GlobalAlloc (0, sizeof (MEMSTM));
#else
        hMem = GlobalAlloc (GMEM_SHARE | GMEM_MOVEABLE, sizeof (MEMSTM));
#endif
        if (hMem == NULL)
        {
                goto ErrorExit;
        }

        pData = (MEMSTM FAR*) GlobalLock(hMem);

        if (pData == NULL)
        {
                GlobalUnlock(hMem);
                goto FreeMem;
        }

        pData->cRef = 0;
        pData->cb = cbSize;
        pData->fDeleteOnRelease = fDeleteOnRelease;
        pData->hGlobal = hGlobal;
        GlobalUnlock(hMem);

        pstm = CMemStm::Create(hMem);

        if (pstm == NULL)
        {
                goto FreeMem;
        }

        *ppstm = pstm;

        // CALLHOOKOBJECTCREATE(S_OK,CLSID_NULL,IID_IStream,(IUnknown **)ppstm);
        hresult = NOERROR;
        goto SafeExit;

FreeMem:
        if (hMem)
        {
            Verify(0==GlobalFree(hMem));
        }
ErrorExit:

        // LEERROR(1, "Out of memory!");

        hresult = E_OUTOFMEMORY;

SafeExit:

        // OLETRACEOUT((API_CreateStreamOnHGlobal, hresult));

        return hresult;
}

