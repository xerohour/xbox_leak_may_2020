#pragma once

#include "dmusici.h"
#include "DMClient.h"



//===========================================================================
// CtIDirectMusicPerformance
// Wrapper class for IDirectMusicPerformance
// Inherits from CtIUnknown
//===========================================================================
/*
class CtIDirectMusicPerformance// : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicPerformance();
    ~CtIDirectMusicPerformance();

    // test class helpers


    virtual HRESULT InitTestClass(IDirectMusicPerformance *pdmPerformance);
/*
    virtual HRESULT GetRealObjPtr(IDirectMusicPerformance **ppdmPerformance);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);

    //BUGBUG - DO WE STIL NEED THESE WRAPPED?
    //we don't normally wrap these, but they were needed in a few tests to help track leaks
    virtual DWORD AddRef(void);
    virtual DWORD Release(void);

    virtual HRESULT GetSegmentState(CtIDirectMusicSegmentState** pptdmSegmentState,
                                    MUSIC_TIME mtTime);
    virtual HRESULT SendPMsg(DMUS_PMSG* pPMSG);
    virtual HRESULT IsPlaying(CtIDirectMusicSegment* ptdmSegment,
                            CtIDirectMusicSegmentState* ptdmSegState);
    virtual HRESULT GetTime(REFERENCE_TIME* prtNow, MUSIC_TIME* pmtNow);
    virtual HRESULT AllocPMsg(ULONG cb, DMUS_PMSG** ppPMSG);
    virtual HRESULT FreePMsg(DMUS_PMSG* pPMSG);
    virtual HRESULT GetNotificationPMsg(DMUS_NOTIFICATION_PMSG** ppNotificationPMsg);
    virtual HRESULT AddNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT RemoveNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT GetGlobalParam(REFGUID rguidType, void* pData,
                                    DWORD dwSize);
    virtual HRESULT SetGlobalParam(REFGUID rguidType, void* pData,
                                    DWORD dwSize);
*/

//}; // ** end CtIDirectMusicPerformance

//===========================================================================
// CtIDirectMusicPerformance8
//
// Wrapper class for IDirectMusicPerformance8
//
// Inherits from CtIDirectMusicPerformance
//===========================================================================
class CtIDirectMusicPerformance8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicPerformance8();
    //~CtIDirectMusicPerformance8();
    // test class helpers
    //HRESULT InitTestClass(void);
    virtual HRESULT CloseDown(void);

    virtual HRESULT InitAudio(IDirectMusic** ppDirectMusic,               // Optional DMusic pointer.
                                            IDirectSound** ppDirectSound,           // Optional DSound pointer.
                                            HWND hWnd,                              // HWND for DSound.
                                            DWORD dwDefaultPathType,                // Requested default audio path type, also optional.
                                            DWORD dwPChannelCount,                  // Number of PChannels, if default audio path to be created.
                                            DWORD dwFlags,                          // DMUS_AUDIOF flags, if no pParams structure.
                                            DMUS_AUDIOPARAMS *pParams);             // Optional initialization structure, defining required voices, buffers, etc.
/*
    virtual HRESULT GetRealObjPtr(IDirectMusicPerformance8 **ppdmPerformance);
*/
  
    virtual HRESULT PlaySegmentEx(CtIUnknown* pSource,                             // Segment to play. Alternately, could be an IDirectMusicSong.
                                            CHAR *pzSegmentName,                  // If song, which segment in the song.
                                            CtIUnknown* pTransition,                  // Optional template segment to compose transition with.
                                            DWORD dwFlags,                          // DMUS_SEGF_ flags.
                                            __int64 i64StartTime,                   // Time to start playback.
                                            CtIDirectMusicSegmentState** ppSegmentState, // Returned Segment State.
                                            CtIUnknown *pFrom,                        // Optional segmentstate or audiopath to replace.
                                            CtIUnknown *pAudioPath);             // Optional audioPath to play on.
  /*  
  virtual HRESULT StopEx(CtIUnknown *pObjectToStop,
                                            __int64 i64StopTime,
                                            DWORD dwFlags);
    virtual HRESULT CreateAudioPath(CtIUnknown *pSourceConfig,
                                            BOOL fActivate,
                                            CtIDirectMusicAudioPath **ppNewPath);
    virtual HRESULT CreateStandardAudioPath(DWORD dwType, DWORD dwPChannelCount,
                                            BOOL fActivate,
                                            CtIDirectMusicAudioPath **ppNewPath);
    virtual HRESULT SetDefaultAudioPath(CtIDirectMusicAudioPath *pAudioPath);
*/


};
