/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvsndt.h
 *  Content:	definition of CSoundTarget class
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 09/02/99		rodtoll	Created
 * 09/01/2000  georgioc started rewrite/port to xbox
 ***************************************************************************/
 
#ifndef __DVSNDT_H
#define __DVSNDT_H

#define NODSOUND

#include "dvntos.h"
#include "in_core.h"
#include <dsoundp.h>
#include <dvoicep.h>

#include "dvshared.h"

// CSoundTarget
//
// This class represents a single mixer target within the DirectPlayClient system.  Normally there
// is only a single mixer target ("main") for all incoming audio.  However, using the CreateUserBuffer and
// DeleteUserBuffer APIs the developer can specify that they wish to seperate a group or a player
// from the main mixer target.  In this manner they can control the 3d spatialization of the group/player's
// incoming audio stream.  
//
// This class handles all the details related to a mixer target.  It encapsulates the mixing of single 
// or multiple source audio frames and then commiting them to the corresponding directsound buffer.  It also
// handles timing errors in the directsoundbuffer.
//
// For example, if the directsoundbuffer stops running, it will attempt to reset the buffer.
//
// If the directsoundbuffer slows down (because of high CPU), it moves the read pointer forward.  In short
// it ensures that there is always 1 or 2 frames of mixed audio present in the buffer in advance of the 
// read pointer.
//
// In addition the class provides reference counting to prevent premature deletion of the class.  If you 
// wish to take a reference to the class, call AddRef and you MUST then call Release when you are done.
//
// Do not destroy the object directly.  When the last reference to the object is released the object will
// destroy itself.
//
// This class is not multithread safe (except for AddRef and Release).  Only one thread should be 
// accessing it.
//

#define ST_MAX_FREE_BUFFER_CONTEXTS 0x10

class CVoicePlayer;

class CSoundTarget : public XMediaObject
{
public:    

    __inline void *__cdecl operator new(size_t size)
    {
        return ExAllocatePoolWithTag(size, 'lcvd');
    }

    __inline void __cdecl operator delete(void *pv)
    {
        ExFreePool(pv);
    }

    CSoundTarget()
    {
        m_pMediaPacketBuffer = NULL;
        m_cRef = 0;
        InitializeCriticalSection(&m_CS);

        InitializeListHead(&m_PendingList);
        InitializeListHead(&m_AvailableList);

    }
    ~CSoundTarget()
    {
        DV_AUTO_LOCK(&m_CS);

        if (!IsListEmpty(&m_PendingList)) {
    
            DV_FreeList(&m_PendingList, DVLAT_RAW);

        }

        if (m_pMediaPacketBuffer) {
    
            DV_POOL_FREE(m_pMediaPacketBuffer);
            m_pMediaPacketBuffer = NULL;
    
        }

    }

    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
       long l = InterlockedDecrement((long*)&m_cRef);
       if (l == 0)
          delete this;
       return l;
    }

    HRESULT STDMETHODCALLTYPE GetInfo( 
        PXMEDIAINFO pInfo
        )
    {
        pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE | XMO_STREAMF_WHOLE_SAMPLES |
                    XMO_STREAMF_DISCARDABLE;

        //
        // align values to our block size. In gets CEILed, out gets FLOORed
        //
    
        pInfo->dwOutputSize = m_dwFrameSize;
        
        pInfo->dwMaxLookahead = 0;

        return NOERROR;
    }    
    
    HRESULT STDMETHODCALLTYPE Discontinuity()
    {
        return NOERROR;
    }
        
    HRESULT STDMETHODCALLTYPE Flush();

    HRESULT STDMETHODCALLTYPE GetStatus( 
        /* [out] */ DWORD __RPC_FAR *dwFlags);
    
    HRESULT STDMETHODCALLTYPE Process( 
        LPCXMEDIAPACKET pSrcBuffer,
        LPCXMEDIAPACKET pDstBuffer);    


protected:
   
    friend class CVoicePlayer;
    HRESULT MixInSingle( LPBYTE lpbBuffer );    
    HRESULT Initialize(DWORD dwFrameSize);

    CRITICAL_SECTION        m_CS;
    DWORD                   m_dwFrameSize;
    ULONG m_cRef;

    PVOID                    m_pMediaPacketBuffer;
    LIST_ENTRY               m_PendingList;
    LIST_ENTRY               m_AvailableList;

};


#endif
