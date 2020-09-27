#pragma once
#include "globals.h"

namespace DMBUFFER
{
/********************************************************************************
MACROS
********************************************************************************/

enum TestApplies {TESTAPPLY_DEFERRED_NOUPDATE=0x5678, TESTAPPLY_DEFERRED_UPDATE, TESTAPPLY_IMMEDIATE};


/********************************************************************************
CLASS:
    BUFFER3D

This class supports all the methods of the buffer and listener (dsound), but
proxies the calls so it can move the objects relative to one another.

3D Stuff.  The real positions are calculated after these are set, but never stored.
There are two relativisms applied "underneath" the user.

1)  The relative position (m_vRelPos) is subtracted from both the listener
    and the buffer position.
2)  If we're in HEADRELATIVE mode, the listener position is subtracted from the
    buffer position before the buffer position is set.  

These two steps must be done in order.  I have a function to do this: 
SetActualPositions.
********************************************************************************/
class BUFFER3D
{
private:
    LPDIRECTSOUND           m_pDS;
    LPDIRECTSOUNDBUFFER     m_pDSB;

    //Data describing how to apply changes.
    DWORD                    m_dwActualApplyValue;
    TestApplies              m_eApply;
    BOOL                     m_bCommit;

    //3D data.
    D3DVECTOR               m_vLisPos;      //Where the user thinks the listener really is.
    D3DVECTOR               m_vBufPos;      //Where the user thinks the buffer really is.
    
    //Relative 3D data
    D3DVECTOR               m_vRelPos;      //Random user offset
    BOOL                    m_bUsingRelPos; //Are we using this?

public:

    //Contructor
    BUFFER3D(void);
    HRESULT Init(D3DVECTOR vRelative, IDirectSoundBuffer *pBuffer);

    //Destructor
    ~BUFFER3D(void);

    //Special functions to control proxying.
//    HRESULT DSB_Test_SetRelativePosition(FLOAT x, FLOAT y, FLOAT z);
//    HRESULT DSB_Test_SetUsingRelativePosition(BOOL bUsingRelPos);    

    HRESULT DSB_Test_SetTestingApply(TestApplies dwApply);
//    HRESULT DSB_Test_SetTestingMode(DWORD dwTestMode);


    HRESULT DS_SetActualPositions(void);
    
    __inline HRESULT GetDirectSound(IDirectSound **ppDS)
    {
        *ppDS = m_pDS;
        m_pDS->AddRef();
        return S_OK;
    }

    __inline HRESULT GetDirectSoundBuffer(IDirectSoundBuffer **ppDSB)
    {
        *ppDSB = m_pDSB;
        m_pDSB->AddRef();
        return S_OK;
    }

    __inline ULONG DS_AddRef(void)
    {
        return m_pDS->AddRef();
    }

    __inline ULONG DS_Release(void)
    {
        return m_pDS->Release();
    }

    __inline HRESULT DS_GetCaps(LPDSCAPS pdsc)
    {
        return m_pDS->GetCaps( pdsc);
    }

    __inline HRESULT DS_CreateSoundBuffer(LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter)
    {
        return m_pDS->CreateSoundBuffer( pdsbd, ppBuffer, pUnkOuter);
    }

    __inline HRESULT DS_CreateSoundStream(LPCDSSTREAMDESC pdssd, LPDIRECTSOUNDSTREAM *ppStream, LPUNKNOWN pUnkOuter)
    {
        return m_pDS->CreateSoundStream( pdssd, ppStream, pUnkOuter);
    }

    __inline HRESULT DS_GetSpeakerConfig(LPDWORD pdwSpeakerConfig)
    {
        return m_pDS->GetSpeakerConfig( pdwSpeakerConfig);
    }

    __inline HRESULT DS_CommitDeferredSettings(void)
    {
        return m_pDS->CommitDeferredSettings();
    }

    __inline HRESULT DS_GetTime(REFERENCE_TIME *prtCurrent)
    {
        return m_pDS->GetTime( prtCurrent);
    }

    __inline HRESULT DS_QueryInterface(REFIID iid, LPVOID *ppvInterface)
    {
        return m_pDS->QueryInterface( iid, ppvInterface);
    }

    __inline HRESULT DS_SetCooperativeLevel(HWND hWnd, DWORD dwLevel)
    {
        return m_pDS->SetCooperativeLevel( hWnd, dwLevel);
    }

    __inline HRESULT DS_Compact(void)
    {
        return m_pDS->Compact();
    }



    __inline ULONG DSB_AddRef(void)
    {
        return m_pDSB->AddRef();
    }

    __inline ULONG DSB_Release(void)
    {
        return m_pDSB->Release();
    }

    __inline HRESULT DSB_Play(DWORD dwReserved1, DWORD dwReserved2, DWORD dwFlags)
    {
        return S_OK;
//        return m_pDSB->Play( dwReserved1, dwReserved2, dwFlags);
    }

    __inline HRESULT DSB_PlayEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags)
    {
        return S_OK;
//        return m_pDSB->PlayEx( rtTimeStamp, dwFlags);
    }

    __inline HRESULT DSB_Stop(void)
    {
        return S_OK;
//        return m_pDSB->Stop();
    }

    __inline HRESULT DSB_StopEx(REFERENCE_TIME rtTimeStamp, DWORD dwFlags)
    {
        return S_OK;
//        return m_pDSB->StopEx( rtTimeStamp, dwFlags);
    }


//    __inline HRESULT DSB_SetLoopRegion(DWORD dwLoopStart, DWORD dwLoopLength)
//    {
//        return m_pDSB->SetLoopRegion( dwLoopStart, dwLoopLength);
//    }


    __inline HRESULT DSB_GetStatus(LPDWORD pdwStatus)
    {
        return m_pDSB->GetStatus( pdwStatus);
    }

    __inline HRESULT DSB_GetCurrentPosition(LPDWORD pdwPlayCursor, LPDWORD pdwWriteCursor)
    {
        return m_pDSB->GetCurrentPosition( pdwPlayCursor, pdwWriteCursor);
    }

    __inline HRESULT DSB_SetCurrentPosition(DWORD dwPlayCursor)
    {
        return S_OK;
     //   return m_pDSB->SetCurrentPosition( dwPlayCursor);
    }

    __inline HRESULT DSB_SetBufferData(LPVOID pvBufferData, DWORD dwBufferBytes)
    {
        return m_pDSB->SetBufferData( pvBufferData, dwBufferBytes);
    }

    __inline HRESULT DSB_Lock(DWORD dwOffset, DWORD dwBytes, LPVOID *ppvAudioPtr1, LPDWORD pdwAudioBytes1, LPVOID *ppvAudioPtr2, LPDWORD pdwAudioBytes2, DWORD dwFlags)
    {
        return m_pDSB->Lock( dwOffset, dwBytes, ppvAudioPtr1, pdwAudioBytes1, ppvAudioPtr2, pdwAudioBytes2, dwFlags);
    }

    __inline HRESULT DSB_SetFrequency(DWORD dwFrequency)
    {
        return m_pDSB->SetFrequency( dwFrequency);
    }

    __inline HRESULT DSB_SetVolume(LONG lVolume)
    {
        return m_pDSB->SetVolume( lVolume);
    }

/*
    __inline HRESULT DSB_SetChannelVolume(LPDSCHANNELVOLUME pdscv)
    {
        return m_pDSB->SetChannelVolume(pdscv);
    }
*/


    //3D Stuff that must be wrapped due to proxying of 3D positions.
    HRESULT DSB_SetAllParameters(LPCDS3DBUFFER pds3db);
    HRESULT DSB_SetConeAngles(DWORD dwInsideConeAngle, DWORD dwOutsideConeAngle);
    HRESULT DSB_SetConeOrientation(FLOAT x, FLOAT y, FLOAT z);
    HRESULT DSB_SetConeOutsideVolume(LONG lConeOutsideVolume);
    HRESULT DSB_SetMaxDistance(FLOAT flMaxDistance);
    HRESULT DSB_SetMinDistance(FLOAT flMinDistance);
    HRESULT DSB_SetMode(DWORD dwMode);
    HRESULT DSB_SetPosition(FLOAT x, FLOAT y, FLOAT z);
    HRESULT DSB_SetVelocity(FLOAT x, FLOAT y, FLOAT z);

    HRESULT DS_SetAllParameters(LPCDS3DLISTENER pds3db);
    HRESULT DS_SetDistanceFactor(FLOAT flDistanceFactor);
    HRESULT DS_SetDopplerFactor(FLOAT flDopplerFactor);
    HRESULT DS_SetOrientation(FLOAT xFront, FLOAT yFront, FLOAT zFront, FLOAT xTop, FLOAT yTop, FLOAT zTop);
    HRESULT DS_SetPosition(FLOAT x, FLOAT y, FLOAT z);
    HRESULT DS_SetRolloffFactor(FLOAT flRolloffFactor);
    HRESULT DS_SetVelocity(FLOAT x, FLOAT y, FLOAT z);

    __inline HRESULT DSB_SetNotificationPositions(DWORD dwNotifyCount, LPCDSBPOSITIONNOTIFY paNotifies)
    {
        return m_pDSB->SetNotificationPositions( dwNotifyCount, paNotifies);
    }

    __inline HRESULT DSB_QueryInterface(REFIID iid, LPVOID *ppvInterface)
    {
        return m_pDSB->QueryInterface( iid, ppvInterface);
    }

    __inline HRESULT DSB_Unlock(LPVOID pvLock1, DWORD dwLockSize1, LPVOID pvLock2, DWORD dwLockSize2)
    {
        return m_pDSB->Unlock( pvLock1, dwLockSize1, pvLock2, dwLockSize2);
    }

    __inline HRESULT DSB_Restore(void)
    {
        return m_pDSB->Restore();
    }


};


static D3DVECTOR vDefault = {0,0,0};

// Disable warning messages about floating point overflow.
#pragma warning( disable : 4056 4756)
//#define INFINITY (FLT_MAX * FLT_MAX)       
//#define ALMOST_FLT_MAX (FLT_MAX * 0.99f)        //so we don't run up against MAXDISTANCE.
#define FLT_BIG (FLT_MAX / 100.f)
#define FLT_SMALL (FLT_MIN * 100.f)


/********************************************************************************
Note: These are crap functions cuz the D3DOVERLOADS don't work.
BUG 2371 Overloaded functions unusable on _D3DVECTOR due to inclusion of less functional _D3DVECTOR class in D3D8TYPES.H
//TODO: blow away when D3DOVERLOADS work.
********************************************************************************/
D3DVECTOR operator - (const D3DVECTOR& v, const D3DVECTOR& w);
D3DVECTOR operator -= (D3DVECTOR& v, const D3DVECTOR& w);
BOOL operator == (D3DVECTOR& v, const D3DVECTOR& w);
D3DVECTOR make_D3DVECTOR(FLOAT _x, FLOAT _y, FLOAT _z);

//Sets the x, y, or z component of a vector where x, y, z are indexed by values 0-2.
void DMSetComponent(D3DVECTOR *pVector, DWORD dwComponent, FLOAT fValue);

HRESULT DMHelp_DirectSoundCreate(DWORD dwDeviceId, LPDIRECTSOUND *ppDirectSound, LPUNKNOWN pUnkOuter);
HRESULT DMHelp_DirectSoundCreateBuffer(DWORD dwDeviceId, LPCDSBUFFERDESC pdsbd, LPDIRECTSOUNDBUFFER *ppBuffer, LPUNKNOWN pUnkOuter);



// Constants
#define PI 3.141592653589793238
#define PI_TIMES_TWO 6.28318530718
#define PI_OVER_TWO 1.570796326795
#define THREE_PI_OVER_TWO 4.712388980385
#define NEG_PI_OVER_TWO -1.570796326795
#define C180_OVER_PI 57.29577951308
#define PI_OVER_360 0.008726646259972
#define TWO_OVER_PI 0.6366197723676
#define SPEEDOFSOUND 359660.0
#define LOGE_2_INV 1.44269504088896
#define LOG10_2 0.30102999566398


char *String(TestApplies eTestApply);
}

using namespace DMBUFFER;
