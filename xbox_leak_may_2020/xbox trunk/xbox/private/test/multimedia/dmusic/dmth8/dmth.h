//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       dmth.h
//
//--------------------------------------------------------------------------

//===========================================================================
// dmth.h
//
// DirectMusic test harness class descriptions
//
// History:
//  10/14/1997 - davidkl - created
//  01/09/1998 - a-llucar - added method to dmloader.h
//  01/13/1998 - a-llucar - added IDMDownload (from dmusic.h)
//  03/02/1998 - davidkl - ported to latest dmusic architecture changes
//  03/25/1998 - davidkl - removed all non-class goop, renamed to dmthcls
//  05/11/1998 - a-trevg - added explicit _stdcall to exported functions
//  07/31/1998 - a-llucar - added GetResolvedTime method
//  10/13/1998 - kcraven - added Loader::SetObject method
//  10/12/1999 - kcraven - started adding DX8 wmethods
//  01/24/2000 - kcraven - continued adding DX8 methods
//  01/25/2000 - kcraven - start moving dhthCreate Helpers to common file
//  02/18/2000 - kcraven - added loader8 LoadObjectFromFile commented out
//  02/26/2000 - danhaff - Added Port8::SetSink and GetSink methods.
//  03/17/2000 - kcraven - changed Seg8 and SegSt8 to match changes in dmusic.
//  03/29/2000 - kcraven - attempting to sync to dmusic again.
//  04/04/2000 - kcraven - changing to use the SD dmusic headers and
//                         removing dependencies on private checked-in headers
//  04/06/2000 - danhaff-  Removd certain wrapper objects as per dmusic
//                         header changes.
//  04/12/2000 - danhaff-  Added logging functions for DSFX
//  05/01/2000 - kcraven-  Added wrapper for idirectmusic8
//  01/01/2001 - danhaff-  Ported to XBox.
//===========================================================================

#pragma once

//===========================================================================


// included headers
#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <stdlib.h>
#include <float.h>

#include <dsound.h>
#include <dmusici.h>

#include "dmthhelp.h"
#include "memptr.h"
//#include "dmStress.h"  //So the wrapper definitions can know about their object stressor objects.
//#include "dmStressControl.h" //So the app may control which interfaces are stressed.

// symbolic constants
#ifndef MAX_LOGSTRING
#define MAX_LOGSTRING       127
#endif
// logging level for tracking method calls
#ifndef CALLLOGLEVEL
#define CALLLOGLEVEL        3
#endif
// logging level for parameter data
#ifndef PARAMLOGLEVEL
#define PARAMLOGLEVEL       5
#endif
// logging level for structure contents
#ifndef STRUCTLOGLEVEL
#define STRUCTLOGLEVEL      6
#endif
// MINimum logging level
#ifndef MINLOGLEVEL
#define MINLOGLEVEL         0
#endif
// MAXimum logging level
#ifndef MAXLOGLEVEL
#define MAXLOGLEVEL         10
#endif


//---------------------------------------------------------------------------

// forward definitions for test classes
class CtIUnknown;

//---------------------------------------------------------------------------
class CtIDirectMusicObject8;      
typedef CtIDirectMusicObject8 CtIDirectMusicObject;


class CtIDirectMusicLoader8;      
typedef CtIDirectMusicLoader8 CtIDirectMusicLoader;

class CtIDirectMusicSegment8;     
typedef CtIDirectMusicSegment8 CtIDirectMusicSegment;


class CtIDirectMusicAudioPath;    

class CtIDirectMusicSegmentState8;
typedef CtIDirectMusicSegmentState8 CtIDirectMusicSegmentState;

class CtIDirectMusicPerformance8;
typedef CtIDirectMusicPerformance8 CtIDirectMusicPerformance;

class CtIDirectMusicScript;       

class CtIDirectMusicTool;

//===========================================================================
// CtIUnknown
//
// Wrapper class for IUnknown.
//    This is the base class for all other dmusic test harness classes
//
//===========================================================================
class CtIUnknown
{
    public:
    // constructor / destructor
    CtIUnknown();
    virtual ~CtIUnknown();
    // test class helpers
    virtual HRESULT InitTestClass(IUnknown *pUnk);
    virtual HRESULT GetRealObjPtr(IUnknown **ppUnk);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    virtual DWORD AddRef(void);
    virtual DWORD Release(void);

    //danhaff - snuck this out so template functions could print what interface they're using.
    LPSTR       m_szInterfaceName;

    protected:
    IUnknown            *m_pUnk;
    DWORD               m_dwRefCount;
//    DMOBJECTSTRESSOR    *m_pDMObjectStressor;

}; // ** end CtIUnknown


//===========================================================================
// CtIDirectMusicObject8
//
// Wrapper class for IDirectMusicObject8
//
// Inherits from CtIDirectMusicObject
//===========================================================================
class CtIDirectMusicObject8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicObject8();
    ~CtIDirectMusicObject8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicObject8 *pdmObject8);
    virtual HRESULT GetRealObjPtr(IDirectMusicObject8 **ppdmObject8);

    virtual HRESULT GetDescriptor(LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT SetDescriptor(LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc);

    // IDirectMusicObject8
//    virtual void Zombie(void);

    protected:
//    BOOL    m_bZombie; //special BOOL for testing Zombie

}; // ** end CtIDirectMusicObject8


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
    ~CtIDirectMusicLoader8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicLoader8 *pdmLoader8);
    virtual HRESULT GetRealObjPtr(IDirectMusicLoader8 **ppdmLoader8);


    //Loader1
    virtual HRESULT GetObject(LPDMUS_OBJECTDESC pDesc,
                            REFIID riid,
                            void** ppv);
    virtual HRESULT SetObject(LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT SetSearchDirectory(GUID idClass,
                                        CHAR *pzPath,
                                        BOOL fClear);
    
    //Loader8
    virtual HRESULT ClearCache(GUID idClass);

/*
    virtual HRESULT ScanDirectory(GUID idClass,
                                WCHAR *pwzFileExtension,
                                WCHAR *pwzCacheFileName);
    virtual HRESULT ReleaseObject(CtIDirectMusicObject *pObject);
    virtual HRESULT EnableCache(GUID idClass,
                                BOOL fEnable);
    virtual HRESULT CacheObject(CtIDirectMusicObject *pObject);
    virtual HRESULT EnumObject(GUID idClass,
                                DWORD dwIndex,
                                LPDMUS_OBJECTDESC pDesc);




  virtual HRESULT EnableGarbageCollector(BOOL fEnable);
*/

    virtual void CollectGarbage(void);
    virtual HRESULT ReleaseObjectByUnknown(CtIUnknown *pObject);

//  virtual HRESULT GetDynamicallyReferencedObject(CtIDirectMusicObject *pSourceObject,
//                                                           LPDMUS_OBJECTDESC pDesc,
//                                                           REFIID riid,
//                                                           LPVOID FAR *ppv);
//  virtual HRESULT ReportDynamicallyReferencedObject(CtIDirectMusicObject *pSourceObject,
//                                                           CtIUnknown *pReferencedObject);
    virtual HRESULT LoadObjectFromFile(REFGUID rguidClassID,
                                        REFIID iidInterfaceID,
                                        const CHAR *pzFilePath,
                                        void ** ppObject);

}; // ** end CtIDirectMusicLoader8


//===========================================================================
// CtIDirectMusicSegment8
//
// Wrapper class for IDirectMusicSegment8
//
// Inherits from CtIDirectMusicSegment
//===========================================================================
class CtIDirectMusicSegment8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSegment8();
    ~CtIDirectMusicSegment8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSegment8 *pdmSegment8);
    virtual HRESULT GetRealObjPtr(IDirectMusicSegment8 **ppdmSegment8);

    virtual HRESULT SetRepeats(DWORD dwRepeats);
    virtual HRESULT GetRepeats(DWORD *pdwRepeats);

    virtual HRESULT AddNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT RemoveNotificationType(REFGUID rguidNotificationType);


/*
    // IDirectMusicSegment8
    virtual HRESULT SetTrackConfig(REFGUID rguidTrackClassID,DWORD dwGroup, DWORD dwIndex,DWORD dwFlagsOn,DWORD dwFlagsOff);
//PHOOPHOO
//  virtual HRESULT GetTrackConfig(REFGUID rguidTrackClassID,DWORD dwGroup, DWORD dwIndex, DWORD *pdwFlags);
//  virtual HRESULT SetClockTimeDuration(REFERENCE_TIME rtDuration);
//  virtual HRESULT GetClockTimeDuration(REFERENCE_TIME *prtDuration);
//  virtual HRESULT SetFlags(DWORD dwFlags);
//  virtual HRESULT GetFlags(DWORD *pdwFlags);
//  virtual HRESULT GetObjectInPath(DWORD dwPChannel,DWORD dwStage, DWORD dwBuffer,
//                          REFGUID guidObject,DWORD dwIndex,REFGUID iidInterface,void ** ppObject);
    virtual HRESULT GetAudioPathConfig(CtIUnknown ** ppIAudioPathConfig);
*/


    virtual HRESULT Compose(MUSIC_TIME mtTime,CtIDirectMusicSegment* pFromSegment,
                            CtIDirectMusicSegment* pToSegment,CtIDirectMusicSegment** ppComposedSegment);

//    virtual HRESULT Download(CtIUnknown *pAudioPath);
//    virtual HRESULT Unload(CtIUnknown *pAudioPath);


    virtual HRESULT GetLength(MUSIC_TIME* pmtLength);
    virtual HRESULT SetLength(MUSIC_TIME mtLength);

    virtual HRESULT SetClockTimeLength(REFERENCE_TIME rtLength, BOOL fClockTime);
    virtual HRESULT GetClockTimeLength(REFERENCE_TIME * prtLength, BOOL *pfClockTime);
//    virtual HRESULT SetClockTimeLoopPoints(REFERENCE_TIME rtStart, REFERENCE_TIME rtEnd);
//    virtual HRESULT GetClockTimeLoopPoints(REFERENCE_TIME * prtStart, REFERENCE_TIME * prtEnd);
    virtual HRESULT SetLoopPoints(MUSIC_TIME mtStart, MUSIC_TIME mtEnd);
    virtual HRESULT GetLoopPoints(MUSIC_TIME * pmtStart, MUSIC_TIME * pmtEnd);
    virtual HRESULT SetStartPoint(MUSIC_TIME mtStart);
    virtual HRESULT GetStartPoint(MUSIC_TIME* pmtStart);
    virtual HRESULT SetWavePlaybackParams(DWORD dwFlags, DWORD dwReadAhead);

};

//===========================================================================
// CtIDirectMusicAudioPath
//
// Wrapper class for IDirectMusicAudioPath
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicAudioPath : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicAudioPath();
    ~CtIDirectMusicAudioPath();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicAudioPath *pdmAudioPath);
    virtual HRESULT GetRealObjPtr(IDirectMusicAudioPath **ppdmAudioPath);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicAudioPath methods
    virtual HRESULT GetObjectInPath(DWORD dwPChannel,
                                           DWORD dwStage,
                                             DWORD dwBuffer,
                                           REFGUID guidObject,
                                           DWORD dwIndex,
                                           REFGUID iidInterface,
                                           void ** ppObject);
    virtual HRESULT Activate(BOOL fActivate);
    virtual HRESULT SetVolume(long lVolume, DWORD dwDuration);
    virtual HRESULT SetPitch (long lPitch,  DWORD dwDuration);

//    virtual HRESULT ConvertPChannel(DWORD dwPChannelIn,DWORD *pdwPChannelOut);

};



//===========================================================================
// CtIDirectMusicSegmentState8
//
// Wrapper class for IDirectMusicSegmentState8
//
// Inherits from CtIDirectMusicSegmentState
//===========================================================================
class CtIDirectMusicSegmentState8 : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSegmentState8();
    ~CtIDirectMusicSegmentState8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSegmentState8 *pdmSegmentState);
    virtual HRESULT GetRealObjPtr(IDirectMusicSegmentState8 **ppdmSegmentState);
    // the IDirectMusicSegmentState8 methods
/*
    virtual HRESULT SetTrackConfig(
                            REFGUID rguidTrackClassID,
                            DWORD dwGroup,
                            DWORD dwIndex,
                            DWORD dwFlagsOn,
                            DWORD dwFlagsOff);
//PHOOPHOO
//  virtual HRESULT GetTrackConfig(
//                          REFGUID rguidTrackClassID,
//                          DWORD dwGroup,
//                          DWORD dwIndex,
//                          DWORD *pdwFlags);
*/
    virtual HRESULT GetObjectInPath(
                            DWORD dwPChannel,
                            DWORD dwStage,
                            DWORD dwBuffer,
                            REFGUID guidObject,
                            DWORD dwIndex,
                            REFGUID iidInterface,
                            void ** ppObject);

    virtual HRESULT GetSegment(CtIDirectMusicSegment** pptdmSegment);
    virtual HRESULT GetStartTime(MUSIC_TIME* pmtOffset);
    virtual HRESULT SetVolume(long lVolume, DWORD dwDuration);
    virtual HRESULT SetPitch (long lPitch,  DWORD dwDuration);



};



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
    ~CtIDirectMusicPerformance8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicPerformance8 *pdmPerformance);
    virtual HRESULT GetRealObjPtr(IDirectMusicPerformance8 **ppdmPerformance);

    virtual DWORD AddRef(void);
    virtual DWORD Release(void);

    // the IDirectMusicPerformance methods
/*
    virtual HRESULT Init(CtIDirectMusic** pptdm,
                        LPDIRECTSOUND pDirectSound,
                        HWND hWnd);
    virtual HRESULT PlaySegment(CtIDirectMusicSegment* ptdmSegment,
                            DWORD dwFlags, REFERENCE_TIME rtStartTime,
                            CtIDirectMusicSegmentState** pptdmSegmentState);
    virtual HRESULT Stop(CtIDirectMusicSegment* ptdmSegment,
                        CtIDirectMusicSegmentState* ptdmSegmentState,
                        MUSIC_TIME mtTime, DWORD dwFlags);
*/
    virtual HRESULT GetSegmentState(CtIDirectMusicSegmentState** pptdmSegmentState,
                                    MUSIC_TIME mtTime);
    virtual HRESULT SendPMsg(DMUS_PMSG* pPMSG);
/*
    virtual HRESULT SetPrepareTime(DWORD dwMilliSeconds);
    virtual HRESULT GetPrepareTime(DWORD* pdwMilliSeconds);
    virtual HRESULT SetBumperLength(DWORD dwMilliSeconds);
    virtual HRESULT GetBumperLength(DWORD* pdwMilliSeconds);
*/
    virtual HRESULT MusicToReferenceTime(MUSIC_TIME mtTime,
                                            REFERENCE_TIME* prtTime);
    virtual HRESULT ReferenceToMusicTime(REFERENCE_TIME rtTime,
                                            MUSIC_TIME* pmtTime);
    virtual HRESULT IsPlaying(CtIDirectMusicSegment8* ptdmSegment,
                            CtIDirectMusicSegmentState* ptdmSegState);
    virtual HRESULT GetTime(REFERENCE_TIME* prtNow, MUSIC_TIME* pmtNow);
    virtual HRESULT AllocPMsg(ULONG cb, DMUS_PMSG** ppPMSG);
    virtual HRESULT FreePMsg(DMUS_PMSG* pPMSG);
/*
    virtual HRESULT GetGraph(CtIDirectMusicGraph** pptdmGraph);
    virtual HRESULT SetGraph(CtIDirectMusicGraph* ptdmGraph);
    virtual HRESULT SetNotificationHandle(HANDLE hNotification,
                                        REFERENCE_TIME rtMinimum);
*/
    virtual HRESULT GetNotificationPMsg(DMUS_NOTIFICATION_PMSG** ppNotificationPMsg);
    virtual HRESULT AddNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT RemoveNotificationType(REFGUID rguidNotificationType);
/*
    virtual HRESULT AddPort(CtIDirectMusicPort* ptdmPort);
    virtual HRESULT RemovePort(CtIDirectMusicPort* ptdmPort);
    virtual HRESULT AssignPChannelBlock(DWORD dwBlockNum,
                                        CtIDirectMusicPort* ptdmPort,
                                        DWORD dwGroup);
    virtual HRESULT AssignPChannel(DWORD dwPChannel,
                                    CtIDirectMusicPort* ptdmPort,
                                    DWORD dwGroup, DWORD dwMChannel);
    virtual HRESULT PChannelInfo(DWORD dwPChannel,
                                    CtIDirectMusicPort** pptdmPort,
                                    DWORD* pdwGroup, DWORD* pdwMChannel);
    virtual HRESULT DownloadInstrument( CtIDirectMusicInstrument* ptdmInst,
                                        DWORD dwPChannel,
                                        CtIDirectMusicDownloadedInstrument** pptdmDownInst,
                                        DMUS_NOTERANGE* pNoteRanges,
                                        DWORD dwNumNoteRanges,
                                        CtIDirectMusicPort** pptdmPort,
                                        DWORD* pdwGroup,
                                        DWORD* pdwMChannel);
*/
    virtual HRESULT Invalidate(MUSIC_TIME mtTime, DWORD dwFlags);
/*
    virtual HRESULT GetParam(REFGUID rguidType, DWORD dwGroupBits,
                            DWORD dwIndex, MUSIC_TIME mtTime,
                            MUSIC_TIME* pmtNext, void* pData);
    virtual HRESULT SetParam(REFGUID rguidType, DWORD dwGroupBits,
                            DWORD dwIndex, MUSIC_TIME mtTime,
                            void* pData);
*/
    virtual HRESULT GetGlobalParam(REFGUID rguidType, void* pData,
                                    DWORD dwSize);
    virtual HRESULT SetGlobalParam(REFGUID rguidType, void* pData,
                                    DWORD dwSize);
    virtual HRESULT CloseDown(void);
/*
    virtual HRESULT GetLatencyTime(REFERENCE_TIME*);
    virtual HRESULT GetQueueTime(REFERENCE_TIME* prtTime);
    virtual HRESULT AdjustTime(REFERENCE_TIME rtAmount);
    virtual HRESULT GetResolvedTime(REFERENCE_TIME rtTime, REFERENCE_TIME* prtResolved, DWORD dwFlags);
    virtual HRESULT MIDIToMusic(BYTE bMIDIValue,
                                    DMUS_CHORD_KEY* pChord,
                                    BYTE bPlayMode,
                                    BYTE bChordLevel,
                                    WORD *pwMusicValue);
    virtual HRESULT MusicToMIDI(WORD wMusicValue,
                                    DMUS_CHORD_KEY* pChord,
                                    BYTE bPlayMode,
                                    BYTE bChordLevel,
                                    BYTE *pbMIDIValue);
    virtual HRESULT TimeToRhythm(MUSIC_TIME mtTime,
                                    DMUS_TIMESIGNATURE *pTimeSig,
                                    WORD *pwMeasure,
                                    BYTE *pbBeat,
                                    BYTE *pbGrid,
                                    short *pnOffset);
    virtual HRESULT RhythmToTime(WORD wMeasure,
                                    BYTE bBeat,
                                    BYTE bGrid,
                                    short nOffset,
                                    DMUS_TIMESIGNATURE *pTimeSig,
                                    MUSIC_TIME *pmtTime);
*/






    // IDirectMusicPerformance8

    virtual HRESULT InitAudioX(DWORD dwDefaultPathType,  // Requested default audio path type, also optional.
	                           DWORD dwPChannelCount,    // Number of PChannels, if default audio path to be created.
                               DWORD dwVoiceCount,       // Number of Voices (DSoundBuffers) allocated by synth.
                               DWORD dwFlags);           // 0=DMusic will run DoWork() thread, 1=App will do it.

    virtual HRESULT PlaySegmentEx(CtIUnknown* pSource,                             // Segment to play. Alternately, could be an IDirectMusicSong.
                                            CHAR *pzSegmentName,                  // If song, which segment in the song.
                                            CtIUnknown* pTransition,                  // Optional template segment to compose transition with.
                                            DWORD dwFlags,                          // DMUS_SEGF_ flags.
                                            __int64 i64StartTime,                   // Time to start playback.
                                            CtIDirectMusicSegmentState8** ppSegmentState8, // Returned Segment State.
                                            CtIUnknown *pFrom,                        // Optional segmentstate or audiopath to replace.
                                            CtIUnknown *pAudioPath);             // Optional audioPath to play on.
    virtual HRESULT StopEx(CtIUnknown *pObjectToStop,
                                            __int64 i64StopTime,
                                            DWORD dwFlags);
    virtual HRESULT ClonePMsg(DMUS_PMSG* pSourcePMSG,
                                            DMUS_PMSG** ppCopyPMSG);
    virtual HRESULT CreateAudioPath(CtIUnknown *pSourceConfig,
                                            BOOL fActivate,
                                            CtIDirectMusicAudioPath **ppNewPath);
    virtual HRESULT CreateStandardAudioPath(DWORD dwType, DWORD dwPChannelCount,
                                            BOOL fActivate,
                                            CtIDirectMusicAudioPath **ppNewPath);
    virtual HRESULT SetDefaultAudioPath(CtIDirectMusicAudioPath *pAudioPath);
    virtual HRESULT GetDefaultAudioPath(CtIDirectMusicAudioPath **ppAudioPath);
//PHOOPHOO
//  virtual HRESULT SetParamHook(CtIDirectMusicParamHook *pIHook);
    virtual HRESULT GetParamEx(REFGUID rguidType,   // GetParam command ID.
                                DWORD dwTrackID,        // Virtual track ID of caller.
                                DWORD dwGroupBits,      // Group bits of caller.
                                DWORD dwIndex,          // Index to Nth parameter.
                                MUSIC_TIME mtTime,      // Time of requested parameter.
                                MUSIC_TIME* pmtNext,    // Returned delta to next parameter.
                                void* pParam);          // Data structure to fill with parameter.

};

//===========================================================================
// CtIDirectMusicScript
//
// Wrapper class for IDirectMusicScript
//
// Inherits from CtIUnknown
//
// NOTE:  We're using IUnknown instead of CtIUnknown for the Get/SetVariableObject
//        methods, since it will simplify testing code and it's really another
//        variable like BOOL or LONG, for which we do not wrap. - Jimmo
//===========================================================================
class CtIDirectMusicScript : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicScript();
    ~CtIDirectMusicScript();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicScript  *pdmScript);
    virtual HRESULT GetRealObjPtr(IDirectMusicScript **ppdmScript);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicScript methods
    virtual HRESULT Init(CtIDirectMusicPerformance *pPerformance, DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT CallRoutine(CHAR *pszRoutineName, DMUS_SCRIPT_ERRORINFO *pdmScriptErrorInfo);
/*
    virtual HRESULT SetVariableVariant(WCHAR *pwszVariableName,
                                               VARIANT varValue,
                                               BOOL fSetRef,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT GetVariableVariant(WCHAR *pwszVariableName,
                                               VARIANT *pvarValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
*/
    virtual HRESULT SetVariableNumber(CHAR *pszVariableName,
                                               LONG lValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT GetVariableNumber(CHAR *pszVariableName,
                                               LONG *plValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT SetVariableObject(CHAR *pszVariableName,
                                               IUnknown *punkValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT GetVariableObject(CHAR *pszVariableName,
                                               REFIID riid,
                                               LPVOID FAR *ppv,
                                               DMUS_SCRIPT_ERRORINFO *pErrorInfo);
//  virtual HRESULT GetVariableObject(WCHAR *pwszVariableName,
//                                               IUnknown **ppunkValue,
//                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
/*
    virtual HRESULT EnumRoutine(DWORD dwIndex,WCHAR *pwszName);
    virtual HRESULT EnumVariable(DWORD dwIndex,WCHAR *pwszName);
*/

    virtual HRESULT GetVariableString
    (
        CHAR                   *pszVariableName,
        CHAR                   *pszValue,
        LONG                    lLength,
        LONG                   *plConverted,
        DMUS_SCRIPT_ERRORINFO  *pdmScriptErrorInfo
    );


    virtual HRESULT SetVariableString
    (
        CHAR                   *pszVariableName,
        CHAR                   *pszValue,
        DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
    );

};


//===========================================================================
// CtIDirectMusicGraph
//
// Wrapper class for IDirectMusicGraph
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicGraph : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicGraph();
    ~CtIDirectMusicGraph();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicGraph *pdmGraph);
    virtual HRESULT GetRealObjPtr(IDirectMusicGraph **ppdmGraph);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicGraph methods
    virtual HRESULT StampPMsg(DMUS_PMSG* pPMSG);
    virtual HRESULT InsertTool(CtIDirectMusicTool* ptdmTool,
                                        DWORD* pdwPChannels,
                                        DWORD cPChannel,
                                        LONG lIndex);
    virtual HRESULT GetTool(DWORD dwIndex,
                            CtIDirectMusicTool **pptdmTool);
    virtual HRESULT RemoveTool(CtIDirectMusicTool *ptdmTool);

}; // ** end CtIDirectMusicGraph



//===========================================================================
// CtIDirectMusicTool
//
// Wrapper class for IDirectMusicTool
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicTool : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicTool();
    ~CtIDirectMusicTool();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicTool *pdmTool);
    virtual HRESULT GetRealObjPtr(IDirectMusicTool **ppdmTool);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicTool methods
    virtual HRESULT Init(CtIDirectMusicGraph* ptdmGraph);
    virtual HRESULT GetMsgDeliveryType(DWORD* pdwDeliveryType);
    virtual HRESULT GetMediaTypeArraySize(DWORD* pdwNumElements);
    virtual HRESULT GetMediaTypes(DWORD** padwMediaTypes,
                                DWORD dwNumElements);
    virtual HRESULT ProcessPMsg(CtIDirectMusicPerformance* ptdmPerf,
                                DMUS_PMSG* pPMSG);
    virtual HRESULT Flush(CtIDirectMusicPerformance* ptdmPerf,
                            DMUS_PMSG* pPMSG,
                            REFERENCE_TIME rtTime);


}; // ** end CtIDirectMusicTool



//===========================================================================
// test class GUIDs.
//
// These allow us to mascquerade as the real interfaces
//===========================================================================

DEFINE_GUID(CTIID_IUnknown,                             0x88e62400, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicSegment8,                 0xf65da357, 0x7cfb, 0x4cc7, 0xbe, 0x9d, 0x6, 0x25, 0x14, 0xd5, 0xcf, 0x18);
DEFINE_GUID(CTIID_IDirectMusicLoader8,                  0xb48f6473, 0x759f, 0x472d, 0x8c, 0xa0, 0x47, 0x90, 0x7a, 0xe3, 0x6f, 0x87);
DEFINE_GUID(CTIID_IDirectMusicObject8,                    0xe92c35c, 0x4980, 0x40af, 0x99, 0x9f, 0x64, 0x65, 0x99, 0xcd, 0x8d, 0x1c);
DEFINE_GUID(CTIID_IDirectMusicPerformance8,             0x5cffd8de, 0x3d40, 0x4020, 0xac, 0x3e, 0x6d, 0xd2, 0x7, 0x92, 0x98, 0x1d);
DEFINE_GUID(CTIID_IDirectMusicSegmentState8,            0x7af67385, 0x94db, 0x4f5e, 0x99, 0xe4, 0xec, 0xdd, 0xb2, 0x67, 0x76, 0x5b);
DEFINE_GUID(CTIID_IDirectMusicScript,                   0x8aa10b67, 0x1894, 0x46dc, 0xb5, 0xa0, 0x50, 0x6d, 0x3, 0xf5, 0x50, 0x1f);
DEFINE_GUID(CTIID_IDirectMusicAudioPath,                0x50a0078d, 0x687e, 0x4a26, 0x8b, 0x83, 0x1d, 0x16, 0x3b, 0x5c, 0xd2, 0xf6);
//DEFINE_GUID(CTIID_IDirectMusicLoader,                   0x88e6240B, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicObject,                   0x88e6240C, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicPerformance,              0x88e6240D, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicSegment,                  0x88e62411, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicSegmentState,             0x88e62412, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusic,                         0x88e62401, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicBand,                     0x88e62402, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicBuffer,                   0x88e62403, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicCollection,               0x88e62404, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicComposer,                 0x88e62405, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicDownload,                 0x88e62406, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicDownloadedInstrument,     0x88e62407, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicGraph,                    0x88e62408, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicInstrument,               0x88e6240A, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicChordMap,                 0x88e6240E, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicPort,                     0x88e6240F, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicPortDownload,             0x88e62410, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicStyle,                    0x88e62413, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicSynth,                    0x88e62414, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicSynthSink,                0x88e62415, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicTool,                     0x88e62416, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicTrack,                    0x88e62417, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IReferenceClock,                      0x88e62418, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
//DEFINE_GUID(CTIID_IDirectMusicGetLoader,                0x88e62420, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);

//dx7
//DEFINE_GUID(CTIID_IDirectMusicPerformance2,             0x7153d4c9, 0x5d74, 0x45dc, 0x8d, 0xef, 0x64, 0xc1, 0x3c, 0xb7, 0xed, 0xa1);
//DEFINE_GUID(CTIID_IDirectMusicSegment2,                 0x224c56e7, 0xdee, 0x49c7, 0xa5, 0x4e, 0x95, 0xf9, 0xfe, 0x7b, 0xdb, 0xfc);

//dx8
//DEFINE_GUID(CTIID_IDirectMusic8,                        0xb0ee2a0c, 0x152a, 0x4729, 0x8b, 0xfb, 0x3d, 0xd4, 0x8a, 0xd2, 0x5d, 0x3a);
//DEFINE_GUID(CTIID_IDirectMusicBand8,                  0x8da34241, 0x365d, 0x4865, 0x83, 0x1a, 0x57, 0x2d, 0x33, 0x7, 0x6a, 0x56);
//DEFINE_GUID(CTIID_IDirectMusicVoice,                  0xc06836cc, 0xeefe, 0x42da, 0xa2, 0xaa, 0xc5, 0xfc, 0xcd, 0x90, 0x4e, 0xfd);
//PHOOPHOO
//DEFINE_GUID(CTIID_IDirectMusicParamHook,              0xd639af13, 0xede6, 0x4258, 0xb3, 0x13, 0xfb, 0x85, 0xf9, 0x98, 0x6f, 0xa6);
//DEFINE_GUID(CTIID_IDirectMusicPatternTrack,             0xb30e0e2b, 0x5347, 0x4d4a, 0x9c, 0xcb, 0xcd, 0x7e, 0xba, 0x28, 0x9c, 0x33);
//DEFINE_GUID(CTIID_IDirectSoundDownloadedWave,         0xb491d035, 0x559b, 0x42c9, 0xbf, 0xa2, 0x23, 0xca, 0xf6, 0xf1, 0xec, 0x7d);
//DEFINE_GUID(CTIID_IDirectSoundWave,                     0x0c3145e8, 0x9b0c, 0x447a, 0x8f, 0xda, 0x58, 0xff, 0x74, 0x6c, 0xca, 0xb7);

//DEFINE_GUID(CTIID_IDirectMusicTool8,                    0xaa534b4d, 0xf3b4, 0x4bf9, 0x9c, 0x50, 0x3e, 0x4c, 0x76, 0x79, 0x62, 0x36);
//DEFINE_GUID(CTIID_IDirectMusicStyle8,                   0x29af2285, 0x4eeb, 0x4227, 0x93, 0x6e, 0x26, 0xfc, 0x2f, 0x91, 0xc, 0x4d);
//DEFINE_GUID(CTIID_IDirectMusicComposer8,              0x31149e2a, 0xe60c, 0x4557, 0x9c, 0xe2, 0x1f, 0xb5, 0x83, 0xfa, 0x82, 0x4e);
//DEFINE_GUID(CTIID_IDirectMusicPort8,                  0x89ea06d2, 0x10d1, 0x4d7e, 0x8d, 0x80, 0x2, 0x4b, 0xf6, 0xc9, 0x45, 0x9a);
//DEFINE_GUID(CTIID_IDirectMusicSynth8,                   0xbfe8d92d, 0xc522, 0x4279, 0xb3, 0x48, 0xe8, 0x73, 0x2d, 0xb6, 0xab, 0xc3);
//DEFINE_GUID(CTIID_IDirectMusicTrack8,                   0x689dc439, 0xfdfd, 0x4691, 0xb1, 0xe9, 0xcb, 0xda, 0x73, 0xcd, 0x68, 0x6f);
//DEFINE_GUID(CTIID_IDirectMusicContainer,                0x727c098a, 0xf55e, 0x4450, 0xbd, 0xf5, 0xb5, 0x29, 0x14, 0xdf, 0x4a, 0xc2);
//DEFINE_GUID(CTIID_IDirectMusicSong,                     0xfcba94e2, 0xfe1f, 0x4dfa, 0x8a, 0xbc, 0x5b, 0xf, 0x98, 0xdb, 0x46, 0xf5);
//DEFINE_GUID(CTIID_IDirectMusicScriptError,              0xfb8c07db, 0x80f5, 0x4f32, 0xbd, 0xce, 0x80, 0xcc, 0x75, 0x35, 0xb9, 0x60);



// ======================================================================================
//
//   ALIASES FOR INTERFACES THAT HAVE NO DX8 EQUIVALENT
//
// ======================================================================================
#define CTIID_IDirectMusicLoader CTIID_IDirectMusicLoader8
#define CTIID_IDirectMusicSegment CTIID_IDirectMusicSegment8
#define CTIID_IDirectMusicSegmentState CTIID_IDirectMusicSegmentState8
#define CTIID_IDirectMusicPerformance CTIID_IDirectMusicPerformance8
#define CTIID_IDirectMusicObject CTIID_IDirectMusicObject8


// ======================================================================================
//    LOG STRUCTURE HELPERS
// ======================================================================================
void _stdcall dmthLogDMUS_OBJECTDESCStruct(DMUS_OBJECTDESC  Desc);
void _stdcall dmthLogDMUS_SCRIPT_ERRORINFO(UINT uLogLevel, DMUS_SCRIPT_ERRORINFO *pdmScriptErrorInfo);
void _stdcall dmthLogWAVEFORMATEX(UINT uLogLevel, LPWAVEFORMATEX lpwfx);
void _stdcall dmthLogFormatTag(UINT uLogLevel, WORD wFmt);
void _stdcall dmthLogRawGUID(UINT uLogLevel, REFGUID rguid);
//PHOOPHOO
//void _stdcall dmthLogDMUS_WAVEART(UINT uLogLevel, LPDMUS_WAVEART pArticulation);

// ======================================================================================//
//    LEGACY DEFINES
// ======================================================================================
#define dmthLogDMUS_OBJECTDESCS dmthLogDMUS_OBJECTDESCStruct


// ======================================================================================
//
//    DSOUND LOGGING FUNCTIONS!!!
//
// ======================================================================================
void _stdcall Log_DS3DAlgorithm (int, LPGUID);
void _stdcall Log_DSErr (TCHAR *, HRESULT, int);
void _stdcall Log_DSCooperativeLevel (int, DWORD);
void _stdcall Log_GUID (int, LPGUID);
void _stdcall Log_GUID (int, REFGUID);
void _stdcall Log_DSCaps (int, LPDSCAPS);
void _stdcall Log_SpeakerConfig (int, DWORD);
void _stdcall Log_WaveFormatEx(int, LPCWAVEFORMATEX);
void _stdcall Log_DSBufferDesc(int, LPCDSBUFFERDESC);
void _stdcall Log_DSBLockFlags(int, DWORD);
void _stdcall Log_DSCBLockFlags(int, DWORD);
void _stdcall Log_DSBPlayFlags(int, DWORD);
void _stdcall Log_DS3DLAllParameters(int, LPCDS3DLISTENER);
void _stdcall Log_DS3DApplyMode (int, DWORD);
void _stdcall Log_DS3DBMode (int, DWORD);
void _stdcall Log_D3DVector(int, const struct _D3DVECTOR *);
void _stdcall Log_DS3DBAllParameters(int, LPCDS3DBUFFER);
void _stdcall Log_DSBStatus (int, DWORD);
void _stdcall Log_DSCBStatus (int, DWORD);
void _stdcall Log_DSNPositionNotifies (int, DWORD, LPCDSBPOSITIONNOTIFY);
void _stdcall Log_DSBCapsFlags(int, DWORD);
void _stdcall Log_DSCBCapsFlags(int, DWORD);
void _stdcall Log_wFormatTag (int, WORD);
DWORD _stdcall GetRandomDWORD(DWORD dwModulus);
FLOAT _stdcall GetRandomD3DVALUE(FLOAT flLower, FLOAT flUpper, FLOAT flResolution);
//void _stdcall Log_DSCBStartFlags (int, DWORD);
//void _stdcall Log_DSCCaps (int, LPDSCCAPS);
//void _stdcall Log_DSCBufferDesc (int, LPCDSCBUFFERDESC);
//void _stdcall Log_DSBCaps (int, LPDSBCAPS);
//void _stdcall Log_DSCBCaps (int, LPDSCBCAPS);



// ======================================================================================
//
//    DSOUND8 LOGGING FUNCTIONS!!!
//
// ======================================================================================
/*TODO: Put these back in!!!
void _stdcall Log_Chorus_Params( int nLogLvl, LPCDSFXChorus lpDSFXC );
void _stdcall Log_Flanger_Params( int nLogLvl, LPCDSFXFlanger lpDSFXF );
//void _stdcall Log_Send_Params( int nLogLvl, LPCDSFXSend lpDSFXS );
void _stdcall Log_Echo_Params( int nLogLvl, LPCDSFXEcho lpDSFXE );
void _stdcall Log_Distortion_Params( int nLogLvl, LPCDSFXDistortion lpDSFXD );
void _stdcall Log_Compressor_Params( int nLogLvl, LPCDSFXCompressor lpDSFXCP );
void _stdcall Log_I3DL2Reverb_Params( int nLogLvl, LPCDSFXI3DL2Reverb lpDSFX3R );
void _stdcall Log_WavesReverb_Params( int nLogLvl, LPCDSFXWavesReverb lpDSFXWR );
void _stdcall Log_Gargle_Params( int nLogLvl, LPCDSFXGargle lpDSFXG );
void _stdcall Log_ParamEq_Params( int nLogLvl, LPCDSFXParamEq lpDSFXP );
void _stdcall String_Chorus_Params( char *szMessage, char* szPrompt, LPCDSFXChorus lpDSFXC );
void _stdcall String_Flanger_Params( char *szMessage, char* szPrompt, LPCDSFXFlanger lpDSFXF );
//void _stdcall String_Send_Params( char *szMessage, char* szPrompt, LPCDSFXSend lpDSFXS );
void _stdcall String_Echo_Params( char *szMessage, char* szPrompt, LPCDSFXEcho lpDSFXE );
void _stdcall String_Distortion_Params( char *szMessage, char* szPrompt, LPCDSFXDistortion lpDSFXD );
void _stdcall String_Compressor_Params( char *szMessage, char* szPrompt, LPCDSFXCompressor lpDSFXCP );
void _stdcall String_I3DL2Reverberation_Params( char *szMessage, char* szPrompt, LPCDSFXI3DL2Reverb lpDSFX3R );
void _stdcall String_Gargle_Params( char *szMessage, char* szPrompt, LPCDSFXGargle lpDSFXG );
void _stdcall String_ParamEq_Params( char *szMessage, char* szPrompt, LPCDSFXParamEq lpDSFXP );
char*_stdcall String_Source_Preset( DWORD dwPreset );
char*_stdcall String_Room_Preset( DWORD dwPreset );

//BOOL _stdcall Equal_Send_Params( LPCDSFXSend lpDSFXS1, LPCDSFXSend lpDSFXS2 );

BOOL _stdcall Equal_Chorus_Params( LPCDSFXChorus lpDSFXC1, LPCDSFXChorus lpDSFXC2 );
BOOL _stdcall Equal_Flanger_Params( LPCDSFXFlanger lpDSFXF1, LPCDSFXFlanger lpDSFXF2 );
BOOL _stdcall Equal_Echo_Params( LPCDSFXEcho lpDSFXE1, LPCDSFXEcho lpDSFXE2 );
BOOL _stdcall Equal_Distortion_Params( LPCDSFXDistortion lpDSFXD1, LPCDSFXDistortion lpDSFXD2 );
BOOL _stdcall Equal_Compressor_Params( LPCDSFXCompressor lpDSFXCP1, LPCDSFXCompressor lpDSFXCP2 );
BOOL _stdcall Equal_Gargle_Params( LPCDSFXGargle lpDSFXG1, LPCDSFXGargle lpDSFXG2 );
BOOL _stdcall Equal_I3DL2Reverb_Params( LPCDSFXI3DL2Reverb lpDSFX3R1, LPCDSFXI3DL2Reverb lpDSFX3R2 );
BOOL _stdcall Equal_ParamEq_Params( LPCDSFXParamEq lpDSFXP1, LPCDSFXParamEq lpDSFXP2 );
BOOL _stdcall Equal_WavesReverb_Params( LPCDSFXWavesReverb lpDSFX3W1, LPCDSFXWavesReverb lpDSFX3W2 );
*/


//Eventually move all the "equal" param-checkers to this method.
#define EXPECT_EQUALITY   0x2345
#define EXPECT_INEQUALITY 0x3456
HRESULT _stdcall Compare_DS3DBUFFER(int iLogLevel, DS3DBUFFER *p1, DS3DBUFFER *p2, DWORD dwExpected);
HRESULT _stdcall Compare_DS3DLISTENER(int iLogLevel, DS3DLISTENER *p1, DS3DLISTENER *p2, DWORD dwExpected);

/*

void _stdcall Copy_Chorus_Params( LPDSFXChorus lpDSFXC1, LPCDSFXChorus lpDSFXC2 );
void _stdcall Copy_Flanger_Params( LPDSFXFlanger lpDSFXF1, LPCDSFXFlanger lpDSFXF2 );
void _stdcall Copy_Echo_Params( LPDSFXEcho lpDSFXE1, LPCDSFXEcho lpDSFXE2 );
void _stdcall Copy_Distortion_Params( LPDSFXDistortion lpDSFXD1, LPCDSFXDistortion lpDSFXD2 );
void _stdcall Copy_Compressor_Params( LPDSFXCompressor lpDSFXCP1, LPCDSFXCompressor lpDSFXCP2 );
void _stdcall Copy_Gargle_Params( LPDSFXGargle lpDSFXG1, LPCDSFXGargle lpDSFXG2 );
void _stdcall Copy_I3DL2Reverb_Params( LPDSFXI3DL2Reverb lpDSFX3R1, LPCDSFXI3DL2Reverb lpDSFX3R2 );
void _stdcall Copy_ParamEq_Params( LPDSFXParamEq lpDSFXP1, LPCDSFXParamEq lpDSFXP2 );
void _stdcall GetRandomEchoParams( LPDSFXEcho lpDSFXE );
void _stdcall GetRandomChorusParams( LPDSFXChorus lpDSFXC );
void _stdcall GetRandomFlangerParams( LPDSFXFlanger lpDSFXF );
void _stdcall GetRandomDistortionParams( LPDSFXDistortion lpDSFXD );
void _stdcall GetRandomCompressorParams( LPDSFXCompressor lpDSFXCP );
void _stdcall GetRandomGargleParams( LPDSFXGargle lpDSFXG );
void _stdcall GetRandomI3DL2ReverbParams( LPDSFXI3DL2Reverb lpDSFX3R );
void _stdcall GetRandomParamEqParams( LPDSFXParamEq lpDSFXP );
*/

#define DSFX_WETDRYMIX_MIN              -100
#define DSFX_WETDRYMIX_MAX              100

#define DSFX_ECHO_LEFTDELAY_MIN         1
#define DSFX_ECHO_LEFTDELAY_MAX         2000
#define DSFX_ECHO_RIGHTDELAY_MIN        1
#define DSFX_ECHO_RIGHTDELAY_MAX        2000
#define DSFX_ECHO_FEEDBACK_MIN          0
#define DSFX_ECHO_FEEDBACK_MAX          10000
#define DSFX_ECHO_PANSWAP_MIN           0
#define DSFX_ECHO_PANSWAP_MAX           1

#define DSFX_CHORUS_DEPTH_MIN           0
#define DSFX_CHORUS_DEPTH_MAX           10000
#define DSFX_CHORUS_FEEDBACK_MIN        -100
#define DSFX_CHORUS_FEEDBACK_MAX        100
#define DSFX_CHORUS_FREQUENCY_MIN       0
#define DSFX_CHORUS_FREQUENCY_MAX       20
#define DSFX_CHORUS_WAVEFORM_MIN        0
#define DSFX_CHORUS_WAVEFORM_MAX        1
#define DSFX_CHORUS_DELAY_MIN           0
#define DSFX_CHORUS_DELAY_MAX           20
#define DSFX_CHORUS_PHASE_MIN           -180
#define DSFX_CHORUS_PHASE_MAX           180

#define DSFX_FLANGER_DEPTH_MIN           0
#define DSFX_FLANGER_DEPTH_MAX           10000
#define DSFX_FLANGER_FEEDBACK_MIN        -100
#define DSFX_FLANGER_FEEDBACK_MAX        100
#define DSFX_FLANGER_FREQUENCY_MIN       0
#define DSFX_FLANGER_FREQUENCY_MAX       20
#define DSFX_FLANGER_WAVEFORM_MIN        0
#define DSFX_FLANGER_WAVEFORM_MAX        1
#define DSFX_FLANGER_DELAY_MIN           0
#define DSFX_FLANGER_DELAY_MAX           20
#define DSFX_FLANGER_PHASE_MIN           -180
#define DSFX_FLANGER_PHASE_MAX           180

#define DSFX_COMPRESSOR_RATIO_MIN           1
#define DSFX_COMPRESSOR_RATIO_MAX           100
#define DSFX_COMPRESSOR_GAIN_MIN            -20
#define DSFX_COMPRESSOR_GAIN_MAX            20
#define DSFX_COMPRESSOR_ATTACK_MIN          0
#define DSFX_COMPRESSOR_ATTACK_MAX          500
#define DSFX_COMPRESSOR_RELEASE_MIN         50
#define DSFX_COMPRESSOR_RELEASE_MAX         3000
#define DSFX_COMPRESSOR_THRESHOLD_MIN       -60
#define DSFX_COMPRESSOR_THRESHOLD_MAX       0
#define DSFX_COMPRESSOR_PREDELAY_MIN        0
#define DSFX_COMPRESSOR_PREDELAY_MAX        500
#define DSFX_COMPRESSOR_COMPGAINMETER_MAX   1000
#define DSFX_COMPRESSOR_COMPGAINMETER_MIN   100
#define DSFX_COMPRESSOR_COMPINPUTMETER_MAX  1000
#define DSFX_COMPRESSOR_COMPINPUTMETER_MIN  100
#define DSFX_COMPRESSOR_COMPMETERRESET_MAX  1000
#define DSFX_COMPRESSOR_COMPMETERRESET_MIN  100

#define DSFX_DISTORTION_GAIN_MIN                        -60
#define DSFX_DISTORTION_GAIN_MAX                        0
#define DSFX_DISTORTION_EDGE_MIN                        0
#define DSFX_DISTORTION_EDGE_MAX                        100
#define DSFX_DISTORTION_POSTEQCENTERFREQUENCY_MIN       100
#define DSFX_DISTORTION_POSTEQCENTERFREQUENCY_MAX       8000
#define DSFX_DISTORTION_POSTEQBANDWIDTH_MIN             100
#define DSFX_DISTORTION_POSTEQBANDWIDTH_MAX             8000
#define DSFX_DISTORTION_PRELOWPASSCUTOFF_MIN            100
#define DSFX_DISTORTION_PRELOWPASSCUTOFF_MAX            8000

#define DSFX_GARGLE_RATEHZ_MIN                          1
#define DSFX_GARGLE_RATEHZ_MAX                          1000
#define DSFX_GARGLE_WAVESHAPE_MIN                       0
#define DSFX_GARGLE_WAVESHAPE_MAX                       1

#define DSFX_PARAMEQ_CENTER_MIN                         10.0f
#define DSFX_PARAMEQ_CENTER_MAX                         10000.0f
#define DSFX_PARAMEQ_BANDWIDTH_MIN                      10.0f
#define DSFX_PARAMEQ_BANDWIDTH_MAX                      10000.0f
#define DSFX_PARAMEQ_GAIN_MIN                           0.1f
#define DSFX_PARAMEQ_GAIN_MAX                           100.0f



void tstLof(int, TCHAR *, double);
LRESULT _cdecl DummyLog (DWORD, LPSTR, ...);
bool IsEqual(float f1, float f2);


#define SAFE_RELEASE( px )  if( px )                            \
                            {                                   \
                            px->Release();                      \
                            px = NULL;                          \
                            }

#define D3DVAL(val) ((float)(val))




DEFINE_GUID(GUID_NULL,                     0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x0, 0x00, 0x0, 0x00, 0x00, 0x00);


#define fnsLog Log
/*
//So the test logging will still work.  Grrr.
HRESULT fnsLog
(
    DWORD                   dwLevel,
    DWORD                   dwDestinationFlags, 
    LPSTR                   szFormat,
    ...
);

HRESULT Log
(
    DWORD                   dwLevel,
    LPSTR                   szFormat,
    ...
);

*/  