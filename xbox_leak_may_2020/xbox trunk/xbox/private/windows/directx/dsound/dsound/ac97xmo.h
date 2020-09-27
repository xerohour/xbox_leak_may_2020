/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ac97xmo.h
 *  Content:    AC97 direct-access media object.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  02/05/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __AC97XMO_H__
#define __AC97XMO_H__

BEGIN_DEFINE_STRUCT()
    LIST_ENTRY      leListEntry;
    XMEDIAPACKET    xmp;
END_DEFINE_STRUCT(AC97PACKET);

#ifdef __cplusplus

//
// AC'97 XMO
//

namespace DirectSound
{

#ifdef VALIDATE_PARAMETERS

    class CAc97MediaObject
        : public XAc97MediaObject, CValidObject<'AC97'>, public CRefCount

#else // VALIDATE_PARAMETERS

    class CAc97MediaObject
        : public XAc97MediaObject, public CRefCount

#endif // VALIDATE_PARAMETERS

    {
    protected:
        static DWORD                m_dwGlobalRefCount;     // Shared reference count
        static CAc97Device *        m_pDevice;              // Shared AC97 device object
                                                        
    protected:                                              
        CAc97Channel *              m_pChannel;             // AC97 channel object
        LIST_ENTRY                  m_lstPending;           // Pending packet list
        LIST_ENTRY                  m_lstFree;              // Free packet list
        LPFNXMEDIAOBJECTCALLBACK    m_pfnCallback;          // Callback function
        LPVOID                      m_pvCallbackContext;    // Callback context
        LPAC97PACKET                m_paPackets;            // Packet pool
        DWORD                       m_dwStatus;             // XMO status

    public:
        CAc97MediaObject(void);
        virtual ~CAc97MediaObject(void);

    public:
        // Initialization
        virtual HRESULT STDMETHODCALLTYPE Initialize(DWORD dwChannel, LPFNXMEDIAOBJECTCALLBACK pfnCallback, LPVOID pvCallbackContext);
    
        // IUnknown methods
        virtual ULONG STDMETHODCALLTYPE AddRef(void);
        virtual ULONG STDMETHODCALLTYPE Release(void);

        // XMediaObject methods
        virtual HRESULT STDMETHODCALLTYPE GetInfo(LPXMEDIAINFO pInfo);
        virtual HRESULT STDMETHODCALLTYPE GetStatus(LPDWORD pdwStatus);
        virtual HRESULT STDMETHODCALLTYPE Process(LPCXMEDIAPACKET pInputPacket, LPCXMEDIAPACKET pOutputPacket);
        virtual HRESULT STDMETHODCALLTYPE Discontinuity(void);
        virtual HRESULT STDMETHODCALLTYPE Flush(void);

        // XAc97MediaObject methods
        virtual HRESULT STDMETHODCALLTYPE SetMode(DWORD dwMode);
        virtual HRESULT STDMETHODCALLTYPE GetCurrentPosition(LPDWORD pdwPosition);

    protected:
        // Packet completion
        void CompletePendingPackets(void);
        void CompletePacket(LPAC97PACKET pPacket, DWORD dwStatus);
    
    private:
        // Event handlers
        static void CALLBACK InterruptCallback(LPVOID pvContext);
    };

    __inline ULONG CAc97MediaObject::AddRef(void)
    {
        _ENTER_EXTERNAL_METHOD("CAc97MediaObject::AddRef");
        return CRefCount::AddRef();
    }

    __inline ULONG CAc97MediaObject::Release(void)
    {
        _ENTER_EXTERNAL_METHOD("CAc97MediaObject::Release");
        return CRefCount::Release();
    }

    __inline HRESULT CAc97MediaObject::Discontinuity(void)
    {
        return DS_OK;
    }

    __inline void CAc97MediaObject::InterruptCallback(LPVOID pvContext)
    {
        ((CAc97MediaObject *)pvContext)->CompletePendingPackets();
    }
}

#endif // __cplusplus

#endif // __AC97XMO_H__
