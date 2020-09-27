#pragma once

#ifdef _XBOX
    #include <xtl.h>
    #include <dmusici.h>
#else
    #include <windows.h>
    #include <dmusici.h>


//    #include <dsound.h>
#endif


#include "create.h"


class CtIUnknown
{
    public:
    CtIUnknown();
    virtual HRESULT InitTestClass(void);
    virtual HRESULT InitTestClass(LPVOID pObject);
    virtual HRESULT GetRealObjPtr(LPVOID *ppVoid);
    DWORD AddRef();
    DWORD Release();

    
    DWORD m_dwRefCount;
    LPVOID m_pvXBoxPointer;
    char m_szClassName[MAX_PATH];

};


//BUGBUG: This interface isn't wrapped yet.
typedef CtIUnknown CtIDirectMusicSegmentState;
//typedef IUnknown IDirectMusic;

//===========================================================================
// CtIDirectMusicLoader
//
// Wrapper class for IDirectMusicLoader
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicLoader : public CtIUnknown
{
/*
    public:
    // constructor / destructor
    CtIDirectMusicLoader();
    ~CtIDirectMusicLoader();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicLoader *pdmLoader);
    virtual HRESULT GetRealObjPtr(IDirectMusicLoader **ppdmLoader);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicLoader methods
    // NOTENOTE: we are overloading here due to a late change in the GetObject
    //  method on the actual IDirectMusicLoader interface.  Since we have
    //  quite a number of test cases implemented using the old version of
    //  GetObject, we simply re-implemented it in our class
    virtual HRESULT GetObject(CtIDirectMusicObject **ppWObject,
                                LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT GetObject(LPDMUS_OBJECTDESC pDesc,
                            REFIID riid,
                            void** ppv);
*/
/*
    virtual HRESULT SetSearchDirectory(GUID idClass,
                                        WCHAR *pwzPath,
                                        BOOL fClear);
    virtual HRESULT ScanDirectory(GUID idClass,
                                WCHAR *pwzFileExtension,
                                WCHAR *pwzCacheFileName);
    virtual HRESULT CacheObject(CtIDirectMusicObject *pObject);
    virtual HRESULT ReleaseObject(CtIDirectMusicObject *pObject);
    virtual HRESULT ClearCache(GUID idClass);
    virtual HRESULT EnableCache(GUID idClass,
                                BOOL fEnable);
    virtual HRESULT EnumObject(GUID idClass,
                                DWORD dwIndex,
                                LPDMUS_OBJECTDESC pDesc);
*/

}; // ** end CtIDirectMusicLoader

//===========================================================================
// CtIDirectMusicLoader8
//
// Wrapper class for IDirectMusicLoader8
//
// Inherits from CtIDirectMusicLoader
//===========================================================================
class CtIDirectMusicLoader8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicLoader8();
    //~CtIDirectMusicLoader8();
    // test class helpers
    //virtual HRESULT InitTestClass(IDirectMusicLoader8 *pdmLoader8);

    virtual HRESULT LoadObjectFromFile(REFGUID rguidClassID,
                                        REFIID iidInterfaceID,
                                        CHAR *pzFilePath,
                                        void ** ppObject);
    
    virtual HRESULT SetObject(LPDMUS_OBJECTDESC pDesc);


    // IDirectMusicLoader8
//  virtual HRESULT EnableGarbageCollector(BOOL fEnable);
/*
    virtual void CollectGarbage(void);
    virtual HRESULT ReleaseObjectByUnknown(CtIUnknown *pObject);
*/
//  virtual HRESULT GetDynamicallyReferencedObject(CtIDirectMusicObject *pSourceObject,
//                                                           LPDMUS_OBJECTDESC pDesc,
//                                                           REFIID riid,
//                                                           LPVOID FAR *ppv);
//  virtual HRESULT ReportDynamicallyReferencedObject(CtIDirectMusicObject *pSourceObject,
//                                                           CtIUnknown *pReferencedObject);


}; // ** end CtIDirectMusicLoader8




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

/*
    virtual HRESULT InitAudio(IDirectMusic** ppDirectMusic,               // Optional DMusic pointer.
                                            IDirectSound** ppDirectSound,           // Optional DSound pointer.
                                            HWND hWnd,                              // HWND for DSound.
                                            DWORD dwDefaultPathType,                // Requested default audio path type, also optional.
                                            DWORD dwPChannelCount,                  // Number of PChannels, if default audio path to be created.
                                            DWORD dwFlags,                          // DMUS_AUDIOF flags, if no pParams structure.
                                            DMUS_AUDIOPARAMS *pParams);             // Optional initialization structure, defining required voices, buffers, etc.

*/

    virtual HRESULT InitAudioX(
                            DWORD dwDefaultPathType,                // Requested default audio path type, also optional.
                            DWORD dwPChannelCount                  // Number of PChannels, if default audio path to be created.
                            );             // Optional initialization structure, defining required voices, buffers, etc.


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




//===========================================================================
// CtIDirectMusicPerformance8
//
// Wrapper class for IDirectMusicPerformance8
//
// Inherits from CtIDirectMusicPerformance
//===========================================================================
class CtIDirectMusicSegment8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSegment8();
    //~CtIDirectMusicSegment8();
    // test class helpers
    //HRESULT InitTestClass(void);
    //HRESULT InitTestClass(LPVOID pvXBoxSegment);


    HRESULT Download(CtIUnknown *pAudioPath);
    HRESULT Unload(CtIUnknown *pAudioPath);





};
// {A260086A-C91B-4bfc-B536-5A3835C701B9}
static const GUID xxx =                {0xa260086a, 0xc91b, 0x4bfc, {0xb5, 0x36, 0x5a, 0x38, 0x35, 0xc7, 0x1, 0xb9 } };
static const GUID xIID_IDirectMusicLoader = {0x4fdad3f6, 0xe642, 0x4a1b, {0x90, 0x28, 0x1c, 0x2f, 0xfd, 0x91, 0x5c, 0x2a}};
static const GUID xIID_IDirectMusicSegment = { 0x3fc8898, 0xac24, 0x4bb8, { 0xaf, 0x2f, 0x6f, 0xba, 0xb6, 0x40, 0x8a, 0x8e}};
static const GUID xIID_IDirectMusicSegmentState = { 0xfbdf2f1d, 0x6378, 0x43ba, { 0x97, 0x29, 0x4b, 0x74, 0xb1, 0xdb, 0x3b, 0xd5}};
static const GUID xIID_IDirectMusicPerformance = {0x37a8aa56, 0x79fd, 0x4fcc, {0x8b, 0x58, 0xd3, 0x9d, 0x75, 0x86, 0x1f, 0x3}};
static const GUID xIID_IDirectMusicBand = {0x70a433c2, 0x4637, 0x40e0, {0xb5, 0xa4, 0x1, 0x91, 0x1c, 0x95, 0xaa, 0x2c}};
static const GUID xIID_IDirectMusicGraph = {0x5ae1e2a9, 0x38d7, 0x42a2, {0x9d, 0x31, 0xa1, 0x9c, 0x9a, 0x93, 0x6a, 0x4a}};
static const GUID xIID_IDirectMusicGetLoader = {0xb0e1656f, 0x3e45, 0x418e, {0x9b, 0x2d, 0x34, 0xd5, 0x33, 0xd1, 0x77, 0xe1}};
static const GUID xIID_IDirectMusicObject = {0x632aee51, 0xb9d, 0x4ea4, {0x9b, 0x60, 0x23, 0xcc, 0x58, 0xf5, 0x56, 0x1e}};
static const GUID xIID_IDirectMusicTool = {0xe59eeefe, 0x7a62, 0x4ca7, {0x8b, 0x47, 0x1f, 0xdd, 0x72, 0x8f, 0xba, 0x57}};
static const GUID xIID_IDirectMusicScript = { 0x801413c2, 0x392, 0x4265, { 0xb3, 0x1a, 0x13, 0xd, 0x7, 0xdf, 0x31, 0xd0}};
#define xIID_IDirectMusicScript8 xIID_IDirectMusicScript
static const GUID xIID_IDirectMusicAudioPath = {0x242ed927, 0xf094, 0x42e4, {0x9b, 0xb9, 0x52, 0xd2, 0x14, 0x19, 0x94, 0x4a}};
#define xIID_IDirectMusicAudioPath8 xIID_IDirectMusicAudioPath
/* unchanged interfaces (alias only) */
#define xIID_IDirectMusicGraph8 xIID_IDirectMusicGraph
#define xIID_IDirectMusicLoader8 xIID_IDirectMusicLoader
#define xIID_IDirectMusicPerformance8 xIID_IDirectMusicPerformance
#define xIID_IDirectMusicSegment8 xIID_IDirectMusicSegment
#define xIID_IDirectMusicSegmentState8 xIID_IDirectMusicSegmentState
#define xIID_IDirectMusicBand8 xIID_IDirectMusicBand
