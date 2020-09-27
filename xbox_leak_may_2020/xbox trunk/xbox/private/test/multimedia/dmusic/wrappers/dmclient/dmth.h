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
//===========================================================================

#ifndef _DMTHCLS_H
#define _DMTHCLS_H

//===========================================================================

// included headers
#include <windows.h>


#include <dmusicc.h>
#include <dmusici.h>
#include <dmusicf.h>
#include <dmusics.h>


//BUGBUG
//DMusicc.h has lost
//DEFINE_GUID(CLSID_DirectMusicSynth,0x58C2B4D0,0x46E7,0x11D1,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
//and
//DMusics.h has lost
DEFINE_GUID(CLSID_DirectMusicSynthSink,0xaec17ce3, 0xa514, 0x11d1, 0xaf, 0xa6, 0x0, 0xaa, 0x0, 0x24, 0xd8, 0xb6);
//by-design
//we ned them to compile


// we require dsound.h we don't need to include it because dmusic includes it already
// BUT, we must use internal dsoundp.h renamed as dsound.h
// to prevent compiler errors (undefined internal only stuff)
// #include <dsound.h>

#include <fnshell2.h>

#include "dmthhelp.h"
#include "memptr.h"
#include "dmStress.h"  //So the wrapper definitions can know about their object stressor objects.
#include "dmStressControl.h" //So the app may control which interfaces are stressed.

//codecoverage and other special tests
#define TDM_USER_MODE_SYNTH         1
#define TDM_KERNEL_MODE_SYNTH       2
#define TDM_HARDWARE_MODE_SYNTH     4

extern BOOL  g_fSelectingDefaultPort;
extern DWORD g_dwUseThisSynth;


//BUGBUG - WHY NOT USE THE REAL THING?
// Added by a-llucar 8/4/98
#define SOFTWARESYNTH_PORT  DMUS_PC_SOFTWARESYNTH
#define EXTERNAL_PORT       DMUS_PC_EXTERNAL
#define GMINHARDWARE_PORT   DMUS_PC_GMINHARDWARE
#define GSINHARDWARE_PORT   DMUS_PC_GSINHARDWARE

#define WINMM               DMUS_PORT_WINMM_DRIVER
#define USER_MODE           DMUS_PORT_USER_MODE_SYNTH
#define KERNEL_MODE         DMUS_PORT_KERNEL_MODE

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

// BUGBUG - MUST WRAP
// interface IDirectMusicThru;

// "dmusicc"
class CtIDirectMusic;
class CtIDirectMusic8;              //dx8
class CtIDirectMusicBuffer;
class CtIDirectMusicPort;
//class CtIDirectMusicPort8;            //dx8
//class CtIDirectMusicVoice;            //dx8
class CtIDirectMusicSynthSink;
//class CtIDirectSoundDownloadedWave;   //dx8
class CtIDirectMusicCollection;
class CtIDirectMusicInstrument;
class CtIDirectMusicDownloadedInstrument;
class CtIDirectMusicPortDownload;
class CtIDirectMusicDownload;
class CtIReferenceClock;

// "dmusici"
class CtIDirectMusicObject;
//class CtIDirectMusicObject8;      //dx8
class CtIDirectMusicLoader;
class CtIDirectMusicLoader8;        //dx8
class CtIDirectMusicSegment;
class CtIDirectMusicSegment8;       //dx8
class CtIDirectMusicSegmentState;
class CtIDirectMusicSegmentState8;  //dx8
class CtIDirectMusicAudioPath;      //dx8
class CtIDirectMusicTrack;
class CtIDirectMusicTrack8;         //dx8
class CtIDirectMusicContainer;      //dx8
//PHOOPHOO
//class CtIDirectMusicParamHook;        //dx8
class CtIDirectMusicPerformance;
class CtIDirectMusicPerformance8;   //dx8
class CtIDirectMusicTool;
class CtIDirectMusicTool8;          //dx8
class CtIDirectMusicGraph;
class CtIDirectMusicStyle;
class CtIDirectMusicStyle8;         //dx8
class CtIDirectMusicChordMap;
class CtIDirectMusicComposer;
//class CtIDirectMusicComposer8;        //dx8
class CtIDirectMusicPatternTrack;   //dx8
class CtIDirectMusicSynth;
class CtIDirectMusicSynth8;         //dx8
class CtIDirectMusicBand;
//class CtIDirectMusicBand8;            //dx8
class CtIDirectMusicGetLoader;
// class CtIDirectMusicSong;           //dx8
class CtIDirectMusicScript;         //dx8
//PHOOPHOO
// class CtIDirectSoundWave;            //dx8
// class CtIDirectMusicScriptError; //dx8  REMOVED

// kcraven tdmusic8 unit test helper comments
/*
IDirectMusic;
IDirectMusicBand;
IDirectMusicBuffer;
IDirectMusicChordMap;
IDirectMusicCollection;
IDirectMusicComposer;
IDirectMusicDownload;
IDirectMusicDownloadedInstrument;
IDirectMusicGetLoader;
IDirectMusicGraph;
IDirectMusicInstrument;
IDirectMusicLoader;
IDirectMusicObject;
IDirectMusicPerformance;
IDirectMusicPort;
IDirectMusicPortDownload;
IDirectMusicSegment;
IDirectMusicSegmentState;
IDirectMusicStyle;
IDirectMusicSynth;
IDirectMusicSynthSink;
IDirectMusicTool;                   not in tdmusic8
IDirectMusicTrack;
IReferenceClock;

tdmusic8 dx8 progress

IDirectMusic8;              //dx8   not sure how to do yet - no new methods but new interface
IDirectMusicAudioPath;      //dx8   dummy unit source done
IDirectMusicBand8;          //dx8   dummy unit source done
IDirectMusicComposer8;      //dx8   dummy unit source done
IDirectMusicContainer;      //dx8   dummy unit source done
IDirectMusicLoader8;        //dx8   dummy unit source done
IDirectMusicObject8;        //dx8   not sure how to do yet
IDirectMusicPatternTrack;   //dx8   dummy unit source done
IDirectMusicPerformance8;   //dx8   dummy unit source done
IDirectMusicPort8;          //dx8   dummy unit source done
IDirectMusicScript;         //dx8   not doing in tdmusic8 - jimmo?
IDirectMusicScriptError;    //dx8   not doing in tdmusic8 - jimmo?
IDirectMusicSegment8;       //dx8   dummy unit source done
IDirectMusicSegmentState8;  //dx8   dummy unit source done
IDirectMusicSong;           //dx8   dummy unit source done
IDirectMusicStyle8;         //dx8   dummy unit source done
IDirectMusicSynth8;         //dx8   dummy unit source done
IDirectMusicTool8;          //dx8
IDirectMusicTrack8;         //dx8   dummy unit source done
IDirectMusicVoice;          //dx8   dummy unit source done
IDirectSoundDownloadedWave; //dx8   not doing unit tests - no methods
IDirectSoundWave;           //dx8   not doing unit tests - may be moved to dsound
*/

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
    ~CtIUnknown();
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
    DMOBJECTSTRESSOR    *m_pDMObjectStressor;

}; // ** end CtIUnknown

//===========================================================================
// CtIDirectMusic
//
// Wrapper class for IDirectMusic
//
// Inherits from: CtIUnknown
//===========================================================================
class CtIDirectMusic : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusic();
    ~CtIDirectMusic();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusic *pdm);
    virtual HRESULT GetRealObjPtr(IDirectMusic **ppdm);
    // replacementversions of the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusic methods
    virtual HRESULT EnumPort(DWORD dwIdx, LPDMUS_PORTCAPS lpPortCaps);

    virtual HRESULT CreateMusicBuffer(LPDMUS_BUFFERDESC pBufferDesc,
                                CtIDirectMusicBuffer **pptdmBuffer,
                                IUnknown *punk);
    virtual HRESULT CreatePort(REFGUID rguidPort,
                               LPDMUS_PORTPARAMS pPortParams,
                               CtIDirectMusicPort **pptdmPort,
                               IUnknown *punk);
    virtual HRESULT EnumMasterClock(DWORD dwIdx, LPDMUS_CLOCKINFO lpClockCaps);
    virtual HRESULT GetMasterClock(GUID *guidClock,
                                CtIReferenceClock **pptReferenceClock);
    virtual HRESULT SetMasterClock(REFGUID guidClock);
    virtual HRESULT Activate(BOOL fEnable);
    virtual HRESULT GetDefaultPort(GUID *pguidPort);
    virtual HRESULT SetDirectSound(LPDIRECTSOUND pDirectSound,
                                   HWND hWnd);

}; // ** end CtIDirectMusic

//===========================================================================
// CtIDirectMusic8
//
// Wrapper class for IDirectMusic8
//
// Inherits from: CtIDirectMusic
//===========================================================================
class CtIDirectMusic8 : public CtIDirectMusic
{
    public:
    // constructor / destructor
    CtIDirectMusic8();
    ~CtIDirectMusic8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusic8 *pdm);
    virtual HRESULT GetRealObjPtr(IDirectMusic8 **ppdm);
    // replacementversions of the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusic methods
//  virtual HRESULT EnumPort(DWORD dwIdx, LPDMUS_PORTCAPS lpPortCaps);

//  virtual HRESULT CreateMusicBuffer(LPDMUS_BUFFERDESC pBufferDesc,
//                                CtIDirectMusicBuffer **pptdmBuffer,
//                                IUnknown *punk);
//  virtual HRESULT CreatePort(REFGUID rguidPort,
//                               LPDMUS_PORTPARAMS pPortParams,
//                               CtIDirectMusicPort **pptdmPort,
//                               IUnknown *punk);
//  virtual HRESULT EnumMasterClock(DWORD dwIdx, LPDMUS_CLOCKINFO lpClockCaps);
//  virtual HRESULT GetMasterClock(GUID *guidClock,
//                                CtIReferenceClock **pptReferenceClock);
//  virtual HRESULT SetMasterClock(REFGUID guidClock);
//  virtual HRESULT Activate(BOOL fEnable);
//  virtual HRESULT GetDefaultPort(GUID *pguidPort);
//  virtual HRESULT SetDirectSound(LPDIRECTSOUND pDirectSound,
//                                   HWND hWnd);

    //dx8
    virtual HRESULT SetExternalMasterClock(IReferenceClock *pClock); // don't use wrapped clock

}; // ** end CtIDirectMusic8


//===========================================================================
// CtIDirectMusicBuffer
//
// Wrapper class for IDirectMusicBuffer
//
// Inherits from: CtIUnknown
//===========================================================================
class CtIDirectMusicBuffer : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicBuffer();
    ~CtIDirectMusicBuffer();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicBuffer *pdmBuf);
    virtual HRESULT GetRealObjPtr(IDirectMusicBuffer **ppdmBuf);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicBuffer methods
    virtual HRESULT Flush(void);
    virtual HRESULT TotalTime(LPREFERENCE_TIME prtTime);
    virtual HRESULT PackStructured(REFERENCE_TIME rt, DWORD dwChannelGroup,
                                    DWORD dwMsg);
    virtual HRESULT PackUnstructured(REFERENCE_TIME rt, DWORD dwChannelGroup,
                                DWORD cb, LPBYTE lpb);
    virtual HRESULT ResetReadPtr(void);
    virtual HRESULT GetNextEvent(LPREFERENCE_TIME prt, LPDWORD pdwChannelGroup,
                                LPDWORD pdwLength, LPBYTE *ppData);
    virtual HRESULT GetRawBufferPtr(LPBYTE *ppData);
    virtual HRESULT GetStartTime(LPREFERENCE_TIME prt);
    virtual HRESULT GetUsedBytes(LPDWORD pcb);
    virtual HRESULT GetMaxBytes(LPDWORD pcb);
    virtual HRESULT SetStartTime(REFERENCE_TIME rt);
    virtual HRESULT SetUsedBytes(DWORD cb);
    virtual HRESULT GetBufferFormat(LPGUID pGuidFormat);

}; // ** end CtIDirectMusicBuffer

//===========================================================================
// CtIDirectMusicPort
//
// Wrapper class for IDirectMusicPort
//
// Inherits from: CtIUnknown
//===========================================================================
class CtIDirectMusicPort : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicPort();
    ~CtIDirectMusicPort();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicPort *pdmPort);
    virtual HRESULT GetRealObjPtr(IDirectMusicPort **pdmPort);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicPort methods
    virtual HRESULT PlayBuffer(CtIDirectMusicBuffer *ptdmBuf);
    virtual HRESULT SetReadNotificationHandle(HANDLE hEvent);
    virtual HRESULT Read(CtIDirectMusicBuffer *ptdmBuf);
    virtual HRESULT DownloadInstrument(CtIDirectMusicInstrument *ptdmInst,
                             CtIDirectMusicDownloadedInstrument **pptdmDlInst,
                              DMUS_NOTERANGE* pdmNoteRange,
                            DWORD dwNumRanges);
    virtual HRESULT UnloadInstrument(CtIDirectMusicDownloadedInstrument *ptdmDlInst);
    virtual HRESULT GetLatencyClock(CtIReferenceClock **pptClock);
    virtual HRESULT GetRunningStats(LPDMUS_SYNTHSTATS pStats);
    virtual HRESULT Compact(void);
    virtual HRESULT GetCaps(LPDMUS_PORTCAPS pPortCaps);
    virtual HRESULT DeviceIoControl(DWORD dwIoControlCode, LPVOID lpInBuffer,
                                    DWORD nInBufferSize, LPVOID lpOutBuffer,
                                    DWORD nOutBufferSize, LPDWORD lpBytesReturned,
                                    LPOVERLAPPED lpOverlapped);
    virtual HRESULT GetNumChannelGroups(LPDWORD pdwGroups);
    virtual HRESULT SetNumChannelGroups(DWORD dwChannelGroups);
    virtual HRESULT Activate(BOOL fEnable);
    virtual HRESULT SetChannelPriority(DWORD dwChannelGroup,
                                      DWORD dwChannel,
                                      DWORD dwPriority);
    virtual HRESULT GetChannelPriority(DWORD dwChannelGroup,
                                      DWORD dwChannel,
                                      LPDWORD pdwPriority);
    virtual HRESULT SetDirectSound(LPDIRECTSOUND pDirectSound,
                                    LPDIRECTSOUNDBUFFER pDirectSoundBuffer);
    virtual HRESULT GetFormat(LPWAVEFORMATEX pWaveFormatEx,
                                LPDWORD pdwWaveFormatExSize,
                                LPDWORD pdwBufferSize);


}; // *** end CtIDirectMusicPort

//===========================================================================
// CtIDirectMusicPort8
//
// Wrapper class for IDirectMusicPort8
//
// Inherits from: CtIDirectMusicPort
//===========================================================================
/*
class CtIDirectMusicPort8 : public CtIDirectMusicPort
{
    public:
    // constructor / destructor
    CtIDirectMusicPort8();
    ~CtIDirectMusicPort8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicPort8 *pdmPort);
    virtual HRESULT GetRealObjPtr(IDirectMusicPort8 **pdmPort);
    // IDirectMusicPort8
    virtual HRESULT DownloadWave(IN  CtIDirectSoundWave *pWave, OUT CtIDirectSoundDownloadedWave **ppWave);
    virtual HRESULT UnloadWave(IN CtIDirectSoundDownloadedWave *pWave);
//PHOOPHOO
//    virtual HRESULT AllocVoice(IN CtIDirectSoundDownloadedWave *pWave,IN DWORD dwChannel,IN DWORD dwChannelGroup,
//                              IN REFERENCE_TIME rtStart,IN REFERENCE_TIME rtReadahead,OUT CtIDirectMusicVoice **ppVoice);
    virtual HRESULT AllocVoice
        (
         IN CtIDirectSoundDownloadedWave *pWave,    // Wave to play on this voice
         IN DWORD dwChannel,                        // Channel and channel group
         IN DWORD dwChannelGroup,                   //  this voice will play on
         IN REFERENCE_TIME rtStart,                 // Start position (stream only)
         IN SAMPLE_TIME stLoopStart,                // Loop start (one-shot only)
         IN SAMPLE_TIME stLoopEnd,                  // Loop end (one-shot only)
         OUT CtIDirectMusicVoice **ppVoice          // Returned voice
        );
//PHOOPHOO
//    virtual HRESULT AllocVoice
//        (IN CtIDirectSoundDownloadedWave *pWave,IN DWORD dwChannel,IN DWORD dwChannelGroup,IN REFERENCE_TIME rtStart,
//         IN REFERENCE_TIME rtReadahead,IN SAMPLE_TIME stLoopStart,IN SAMPLE_TIME stLoopEnd,OUT CtIDirectMusicVoice **ppVoice);
    virtual HRESULT AssignChannelToBuses(IN DWORD dwChannelGroup,IN DWORD dwChannel,IN LPDWORD pdwBusses,IN DWORD cBussCount);
    virtual HRESULT SetSink(IN  IDirectSoundSink *pSink);
    virtual HRESULT GetSink(OUT IDirectSoundSink **ppSink);
}; // *** end CtIDirectMusicPort8
*/

//===========================================================================
// CtIDirectSoundDownloadedWave
//
// Wrapper class for IDirectSoundDownloadedWave
//
// Inherits from: CtIUnknown
//===========================================================================
/*
class CtIDirectSoundDownloadedWave : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectSoundDownloadedWave();
    ~CtIDirectSoundDownloadedWave();
    // overloaded versions of test class helpers
    virtual HRESULT InitTestClass(IDirectSoundDownloadedWave *pdmDSDLWave);
    virtual HRESULT GetRealObjPtr(IDirectSoundDownloadedWave **ppdmDSDLWave);
    // replacementversions of the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // IDirectMusicDownloadedWave
    // none
};
*/


//===========================================================================
// CtIDirectMusicVoice
//
// Wrapper class for IDirectMusicVoice
//
// Inherits from: CtIUnknown
//===========================================================================
/*
class CtIDirectMusicVoice : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicVoice();
    ~CtIDirectMusicVoice();
    // overloaded versions of test class helpers
    virtual HRESULT InitTestClass(IDirectMusicVoice *pdmVoice);
    virtual HRESULT GetRealObjPtr(IDirectMusicVoice **ppdmVoice);
    // replacementversions of the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // IDirectMusicVoice
    virtual HRESULT Play(REFERENCE_TIME rtStart,LONG prPitch,LONG vrVolume);
    virtual HRESULT Stop(REFERENCE_TIME rtStop);

};
*/
//===========================================================================
// CtIDirectMusicSynthSink
//
// Wrapper class for IDirectMusicSynthSink
//
// Inherits from: CtIUnknown
//===========================================================================
class CtIDirectMusicSynthSink : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSynthSink();
    ~CtIDirectMusicSynthSink();
    // overloaded versions of test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSynthSink *pdmSink);
    virtual HRESULT GetRealObjPtr(IDirectMusicSynthSink **ppdmSink);
    // replacementversions of the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicSynthSink methods
    virtual HRESULT Init(CtIDirectMusicSynth *ptdmSynth);
    virtual HRESULT SetMasterClock(CtIReferenceClock *ptClock);
    virtual HRESULT GetLatencyClock(CtIReferenceClock **pptClock);
    virtual HRESULT Activate(BOOL fEnable);
    virtual HRESULT SampleToRefTime(DWORD dwSampleTime,REFERENCE_TIME *prfTime);
    virtual HRESULT RefTimeToSample(REFERENCE_TIME rfTime, REFERENCE_TIME *prtSampleTime);
    virtual HRESULT SetDirectSound(LPDIRECTSOUND pDirectSound,
                                   LPDIRECTSOUNDBUFFER pDirectSoundBuffer);
    virtual HRESULT GetDesiredBufferSize(LPDWORD pdwBufferSizeInSamples);

}; // ** end CtIDirectMusicSynthSink

//===========================================================================
// CtIDirectMusicCollection
//
// Wrapper class for IDirectMusicCollection
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicCollection : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicCollection();
    ~CtIDirectMusicCollection();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicCollection *pdmCollect);
    virtual HRESULT GetRealObjPtr(IDirectMusicCollection **ppdmCollect);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicCollection methods
    virtual HRESULT GetInstrument(DWORD dwPatch,
                                CtIDirectMusicInstrument** pptdmInst);
    virtual HRESULT EnumInstrument(DWORD dwIndex, DWORD* pdwPatch,
                                 LPWSTR pName, DWORD cwchName);

}; // ** end CtIDirectMusicCollection

//===========================================================================
// CtIDirectMusicInstrument
//
// Wrapper class for IDirectMusicInstrument
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicInstrument : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicInstrument();
    ~CtIDirectMusicInstrument();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicInstrument *pdmInstr);
    virtual HRESULT GetRealObjPtr(IDirectMusicInstrument **ppdmInstr);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicInstrument methods
    virtual HRESULT GetPatch(DWORD *pdwPatch);
    virtual HRESULT SetPatch(DWORD dwPatch);

}; // ** end CtIDirectMusicInstrument

//===========================================================================
// CtIDirectMusicDownloadedInstrument
//
// Wrapper class for IDirectMusicDownloadedInstrument
//
// Code file(s): tdmobj7.cpp
//===========================================================================
class CtIDirectMusicDownloadedInstrument : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicDownloadedInstrument();
    ~CtIDirectMusicDownloadedInstrument();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicDownloadedInstrument *pdmDlInst);
    virtual HRESULT GetRealObjPtr(IDirectMusicDownloadedInstrument **ppdmDlInst);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // IDirectMusicDownloadedInstrument
    // none

}; // ** end CtIDirectMusicDownloadedInstrument

//===========================================================================
// CtIReferenceClock
//
// Wrapper class for IReferenceClock
//
// Inherits from: CtIUnknown
//===========================================================================
class CtIReferenceClock : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIReferenceClock();
    ~CtIReferenceClock();
    // test class helpers
    virtual HRESULT InitTestClass(IReferenceClock *pRefClock);
    virtual HRESULT GetRealObjPtr(IReferenceClock **ppRefClock);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IReferenceClock methods
    virtual HRESULT GetTime(REFERENCE_TIME *pTime);
    virtual HRESULT AdviseTime(REFERENCE_TIME baseTime,
                                REFERENCE_TIME streamTime,
                                HANDLE hEvent, DWORD * pdwAdviseCookie);
    virtual HRESULT AdvisePeriodic(REFERENCE_TIME startTime,
                                    REFERENCE_TIME periodTime,
                                    HANDLE hSemaphore,
                                    DWORD *pdwAdviseCookie);
    virtual HRESULT Unadvise(DWORD dwAdviseCookie);

}; // ** end CtIReferenceClock

//===========================================================================
// CtIDirectMusicPortDownload
//
// Wrapper class for IDirectMusicPortDownload
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicPortDownload : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicPortDownload();
    ~CtIDirectMusicPortDownload();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicPortDownload* pdmPortDl);
    virtual HRESULT GetRealObjPtr(IDirectMusicPortDownload** ppdmPortDl);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID* ppvObj);
    // the IDirectMusicPortDownload methods
    virtual DWORD GetBuffer(DWORD dwId, CtIDirectMusicDownload** pptdmDownload);
    virtual DWORD AllocateBuffer(DWORD dwSize, CtIDirectMusicDownload** pptdmDownload);
    virtual DWORD GetDLId(DWORD* pdwStartDLId, DWORD dwCount);
    virtual DWORD GetAppend(DWORD* pdwAppend);
    virtual DWORD Download(CtIDirectMusicDownload* ptdmDownload);
    virtual DWORD Unload(CtIDirectMusicDownload* ptdmDownload);

}; // ** end CtIDirectMusicPortDownload

//===========================================================================
// CtIDirectMusicDownload
//
// Wrapper class for IDirectMusicDownload
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicDownload : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicDownload();
    ~CtIDirectMusicDownload();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicDownload *pdmPortDl);
    virtual HRESULT GetRealObjPtr(IDirectMusicDownload **ppdmPortDl);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicDownload methods
    virtual HRESULT GetBuffer(void** ppvBuffer, DWORD* dwSize);

}; // ** end CtIDirectMusicDownload

//===========================================================================
// CtIDirectMusicObject
//
// Wrapper class for IDirectMusicObject
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicObject : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicObject();
    ~CtIDirectMusicObject();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicObject *pdmObject);
    virtual HRESULT GetRealObjPtr(IDirectMusicObject **ppdmObject);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicObject methods
    virtual HRESULT GetDescriptor(LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT SetDescriptor(LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT ParseDescriptor(LPSTREAM pStream, LPDMUS_OBJECTDESC pDesc);

}; // ** end CtIDirectMusicObject

//===========================================================================
// CtIDirectMusicObject8
//
// Wrapper class for IDirectMusicObject8
//
// Inherits from CtIDirectMusicObject
//===========================================================================
//class CtIDirectMusicObject8 : public CtIDirectMusicObject
//{
//    public:
//    // constructor / destructor
//    CtIDirectMusicObject8();
//    ~CtIDirectMusicObject8();
//    // test class helpers
//    virtual HRESULT InitTestClass(IDirectMusicObject8 *pdmObject8);
//    virtual HRESULT GetRealObjPtr(IDirectMusicObject8 **ppdmObject8);
//    // IDirectMusicObject8
//    virtual void Zombie(void);
//
//    protected:
//    BOOL    m_bZombie; //special BOOL for testing Zombie
//
//}; // ** end CtIDirectMusicObject8



//===========================================================================
// CtIDirectMusicLoader
//
// Wrapper class for IDirectMusicLoader
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicLoader : public CtIUnknown
{
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
    virtual HRESULT SetObject(LPDMUS_OBJECTDESC pDesc);
    virtual HRESULT SetSearchDirectory(GUID idClass,
                                        CHAR *pzPath,
                                        BOOL fClear);
    virtual HRESULT ScanDirectory(GUID idClass,
                                CHAR *pzFileExtension,
                                CHAR *pzCacheFileName);
    virtual HRESULT CacheObject(CtIDirectMusicObject *pObject);
    virtual HRESULT ReleaseObject(CtIDirectMusicObject *pObject);
    virtual HRESULT ClearCache(GUID idClass);
    virtual HRESULT EnableCache(GUID idClass,
                                BOOL fEnable);
    virtual HRESULT EnumObject(GUID idClass,
                                DWORD dwIndex,
                                LPDMUS_OBJECTDESC pDesc);

}; // ** end CtIDirectMusicLoader

//===========================================================================
// CtIDirectMusicLoader8
//
// Wrapper class for IDirectMusicLoader8
//
// Inherits from CtIDirectMusicLoader
//===========================================================================
class CtIDirectMusicLoader8 : public CtIDirectMusicLoader
{
    public:
    // constructor / destructor
    CtIDirectMusicLoader8();
    ~CtIDirectMusicLoader8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicLoader8 *pdmLoader8);
    virtual HRESULT GetRealObjPtr(IDirectMusicLoader8 **ppdmLoader8);
    // IDirectMusicLoader8
//  virtual HRESULT EnableGarbageCollector(BOOL fEnable);
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
                                        CHAR *pzFilePath,
                                        void ** ppObject);

}; // ** end CtIDirectMusicLoader8

//===========================================================================
// CtIDirectMusicSegment
//
// Wrapper class for IDirectMusicSegment
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicSegment : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSegment();
    ~CtIDirectMusicSegment();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSegment *pdmSegment);
    virtual HRESULT GetRealObjPtr(IDirectMusicSegment **ppdmSegment);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicSegment methods
    virtual HRESULT GetLength(MUSIC_TIME* pmtLength);
    virtual HRESULT SetLength(MUSIC_TIME mtLength);
    virtual HRESULT GetRepeats(DWORD* pdwRepeats);
    virtual HRESULT SetRepeats(DWORD dwRepeats);
    virtual HRESULT GetDefaultResolution(DWORD* pdwResolution);
    virtual HRESULT SetDefaultResolution(DWORD dwResolution);
    virtual HRESULT GetTrack(REFGUID rguidDataType, DWORD dwGroupBits,
                                DWORD dwIndex, CtIDirectMusicTrack** pptdmTrack);
    virtual HRESULT GetTrackGroup(CtIDirectMusicTrack* ptdmTrack,
                                DWORD* pdwGroupBits);
    virtual HRESULT InsertTrack(CtIDirectMusicTrack* ptdmTrack,
                                DWORD dwGroupBits);
    virtual HRESULT RemoveTrack(CtIDirectMusicTrack* ptdmTrack);
    virtual HRESULT InitPlay(CtIDirectMusicSegmentState** pptdmSegState,
                                CtIDirectMusicPerformance* ptdmPerformance,
                                DWORD dwFlags);
    virtual HRESULT GetGraph(CtIDirectMusicGraph** pptdmGraph);
    virtual HRESULT SetGraph(CtIDirectMusicGraph* ptdmGraph);
    virtual HRESULT AddNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT RemoveNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT GetParam(REFGUID rguidType, DWORD dwGroupBits,
                                DWORD dwIndex, MUSIC_TIME mtTime,
                                MUSIC_TIME* pmtNext, void* pData);
    virtual HRESULT SetParam(REFGUID rguidType, DWORD dwGroupBits,
                                DWORD dwIndex, MUSIC_TIME mtTime, void* pData);
    virtual HRESULT Clone(MUSIC_TIME mtStart, MUSIC_TIME mtEnd,
                                CtIDirectMusicSegment** pptdmSegment);
    virtual HRESULT SetStartPoint(MUSIC_TIME mtStart);
    virtual HRESULT GetStartPoint(MUSIC_TIME* pmtStart);
    virtual HRESULT SetLoopPoints(MUSIC_TIME mtStart, MUSIC_TIME mtEnd);
    virtual HRESULT GetLoopPoints(MUSIC_TIME* pmtStart, MUSIC_TIME* pmtEnd);
    virtual HRESULT SetPChannelsUsed(DWORD dwNumPChannels, DWORD* paPChannels);

}; // ** end CtIDirectMusicSegment

//===========================================================================
// CtIDirectMusicSegment8
//
// Wrapper class for IDirectMusicSegment8
//
// Inherits from CtIDirectMusicSegment
//===========================================================================
class CtIDirectMusicSegment8 : public CtIDirectMusicSegment
{
    public:
    // constructor / destructor
    CtIDirectMusicSegment8();
    ~CtIDirectMusicSegment8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSegment8 *pdmSegment8);
    virtual HRESULT GetRealObjPtr(IDirectMusicSegment8 **ppdmSegment8);
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
    virtual HRESULT Compose(MUSIC_TIME mtTime,CtIDirectMusicSegment* pFromSegment,
                            CtIDirectMusicSegment* pToSegment,CtIDirectMusicSegment** ppComposedSegment);
    virtual HRESULT Download(CtIUnknown *pAudioPath);
    virtual HRESULT Unload(CtIUnknown *pAudioPath);

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

    virtual HRESULT ConvertPChannel(DWORD dwPChannelIn,DWORD *pdwPChannelOut);

};

//===========================================================================
// CtIDirectMusicPatternTrack
//
// Wrapper class for IDirectMusicPatternTrack
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicPatternTrack : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicPatternTrack();
    ~CtIDirectMusicPatternTrack();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicPatternTrack *pdmPatternTrack);
    virtual HRESULT GetRealObjPtr(IDirectMusicPatternTrack **ppdmPatternTrack);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicPatternTrack methods
    virtual HRESULT CreateSegment(CtIDirectMusicStyle* pStyle,
                                  CtIDirectMusicSegment** ppSegment);
    virtual HRESULT SetVariation(CtIDirectMusicSegmentState* pSegState,
                                 DWORD dwVariationFlags,
                                 DWORD dwPart);
    virtual HRESULT SetPatternByName(CtIDirectMusicSegmentState* pSegState,
                                     CHAR* szName,
                                     CtIDirectMusicStyle* pStyle,
                                     DWORD dwPatternType,
                                     DWORD* pdwLength);

}; // ** end CtIDirectMusicPatternTrack

//===========================================================================
// CtIDirectMusicSegmentState
//
// Wrapper class for DMIME's IDirectMusicSegmentState
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicSegmentState : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSegmentState();
    ~CtIDirectMusicSegmentState();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSegmentState *pdmSegmentState);
    virtual HRESULT GetRealObjPtr(IDirectMusicSegmentState **ppdmSegmentState);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicSegmentState methods
    virtual HRESULT GetRepeats(DWORD* pdwRepeats);
    virtual HRESULT GetSegment(CtIDirectMusicSegment** pptdmSegment);
    virtual HRESULT GetStartTime(MUSIC_TIME* pmtOffset);
    virtual HRESULT GetSeek(MUSIC_TIME* pmtSeek);
    virtual HRESULT GetStartPoint(MUSIC_TIME *pmtStart);

}; // ** end CtIDirectMusicSegmentState

//===========================================================================
// CtIDirectMusicSegmentState8
//
// Wrapper class for IDirectMusicSegmentState8
//
// Inherits from CtIDirectMusicSegmentState
//===========================================================================
class CtIDirectMusicSegmentState8 : public CtIDirectMusicSegmentState
{
    public:
    // constructor / destructor
    CtIDirectMusicSegmentState8();
    ~CtIDirectMusicSegmentState8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSegmentState8 *pdmSegmentState);
    virtual HRESULT GetRealObjPtr(IDirectMusicSegmentState8 **ppdmSegmentState);
    // the IDirectMusicSegmentState8 methods
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
    virtual HRESULT GetObjectInPath(
                            DWORD dwPChannel,
                            DWORD dwStage,
                            DWORD dwBuffer,
                            REFGUID guidObject,
                            DWORD dwIndex,
                            REFGUID iidInterface,
                            void ** ppObject);

};

//===========================================================================
// CtIDirectMusicTrack
//
// Wrapper class for IDirectMusicTrack
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicTrack : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicTrack();
    ~CtIDirectMusicTrack();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicTrack *pdmTrack);
    virtual HRESULT GetRealObjPtr(IDirectMusicTrack **ppdmTrack);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicTrack methods
    virtual HRESULT Init(CtIDirectMusicSegment* ptdmSegment);
    virtual HRESULT InitPlay(CtIDirectMusicSegmentState* ptdmSegmentState,
                                CtIDirectMusicPerformance* ptdmPerformance,
                                void** ppStateData, DWORD dwVirtualTrackID,
                                DWORD dwFlags);
    virtual HRESULT EndPlay(void* pStateData);
    virtual HRESULT Play(void* pStateData, MUSIC_TIME mtStart,
                        MUSIC_TIME mtEnd, MUSIC_TIME mtOffset,
                        DWORD dwFlags, CtIDirectMusicPerformance* ptdmPerf,
                        CtIDirectMusicSegmentState* ptdmSegSt,
                        DWORD dwVirtualID);
    virtual HRESULT GetParam(REFGUID rguidType, MUSIC_TIME mtTime,
                                     MUSIC_TIME* pmtNext, void* pData);
    virtual HRESULT SetParam(REFGUID rguidType, MUSIC_TIME mtTime,
                                void* pData);
    virtual HRESULT IsParamSupported(REFGUID rguidType);
    virtual HRESULT AddNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT RemoveNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT Clone(MUSIC_TIME mtStart, MUSIC_TIME mtEnd,
                            CtIDirectMusicTrack** pptdmTrack);

}; // ** end CtIDirectMusicTrack

//===========================================================================
// CtIDirectMusicTrack8
//
// Wrapper class for IDirectMusicTrack8
//
// Inherits from CtIDirectMusicTrack
//===========================================================================
class CtIDirectMusicTrack8 : public CtIDirectMusicTrack
{
    public:
    // constructor / destructor
    CtIDirectMusicTrack8();
    ~CtIDirectMusicTrack8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicTrack8 *pdmTrack8);
    virtual HRESULT GetRealObjPtr(IDirectMusicTrack8 **ppdmTrack8);
    // IDirectMusicTrack8
    virtual HRESULT PlayEx(void* pStateData,
                            REFERENCE_TIME rtStart,
                            REFERENCE_TIME rtEnd,
                            REFERENCE_TIME rtOffset,
                            DWORD dwFlags,
                            CtIDirectMusicPerformance* pPerf,
                            CtIDirectMusicSegmentState* pSegSt,
                            DWORD dwVirtualID);
    virtual HRESULT GetParamEx(REFGUID rguidType,
                            REFERENCE_TIME rtTime,
                            REFERENCE_TIME* prtNext,
                            void* pParam,
                            void * pStateData,
                            DWORD dwFlags);
    virtual HRESULT SetParamEx(REFGUID rguidType,
                            REFERENCE_TIME rtTime,
                            void* pParam,
                            void * pStateData,
                            DWORD dwFlags);
    virtual HRESULT Compose(CtIUnknown* pContext,
                            DWORD dwTrackGroup,
                            CtIDirectMusicTrack** ppResultTrack);
    virtual HRESULT Join(CtIDirectMusicTrack* pNewTrack,
                            MUSIC_TIME mtJoin,
                            CtIUnknown* pContext,
                            DWORD dwTrackGroup,
                            CtIDirectMusicTrack** ppResultTrack);

}; // ** end CtIDirectMusicTrack8

//===========================================================================
// CtIDirectMusicContainer
//
// Wrapper class for CtIDirectMusicContainer
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicContainer : public CtIUnknown
{
     public:
    // constructor / destructor
    CtIDirectMusicContainer();
    ~CtIDirectMusicContainer();

    // test class helpers

    virtual HRESULT InitTestClass(IDirectMusicContainer *pdmContainer);
    virtual HRESULT GetRealObjPtr(IDirectMusicContainer **ppdmContainer);
   /*  IUnknown */
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);

    // IDirectMusicContainer
    /*
    virtual HRESULT EnumObject(REFGUID rguidClass,
                               DWORD dwIndex,
                               LPDMUS_OBJECTDESC pDesc,
                               WCHAR *pwszAlias);
*/

};

//===========================================================================
// CtIDirectMusicParamHook
//
// Wrapper class for IDirectMusicParamHook
//
// Inherits from CtIUnknown
//===========================================================================
/*
//PHOOPHOO
class CtIDirectMusicParamHook : public CtIUnknown
{
     public:
    // constructor / destructor
    CtIDirectMusicParamHook();
    ~CtIDirectMusicParamHook();

    // test class helpers

    virtual HRESULT InitTestClass(IDirectMusicParamHook *pdmParamHook);
    virtual HRESULT GetRealObjPtr(IDirectMusicParamHook **ppdmParamHook);
   //  IUnknown
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    //  IDirectMusicParamHook
    virtual HRESULT GetParam(
                        REFGUID rguidType,
                        DWORD dwGroupBits,
                        DWORD dwIndex,
                        MUSIC_TIME mtTime,
                        MUSIC_TIME* pmtNext,
                        void* pData,
                        CtIDirectMusicSegmentState *pSegState,
                        DWORD dwTrackFlags,
                        HRESULT hr);

};
*/
//===========================================================================
// CtIDirectMusicPerformance
//
// Wrapper class for IDirectMusicPerformance
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicPerformance : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicPerformance();
    ~CtIDirectMusicPerformance();

    // test class helpers

    virtual HRESULT InitTestClass(IDirectMusicPerformance *pdmPerformance);
    virtual HRESULT GetRealObjPtr(IDirectMusicPerformance **ppdmPerformance);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);

    //BUGBUG - DO WE STIL NEED THESE WRAPPED?
    //we don't normally wrap these, but they were needed in a few tests to help track leaks
    virtual DWORD AddRef(void);
    virtual DWORD Release(void);

    // the IDirectMusicPerformance methods
    virtual HRESULT Init(CtIDirectMusic** pptdm,
                        LPDIRECTSOUND pDirectSound,
                        HWND hWnd);
    virtual HRESULT PlaySegment(CtIDirectMusicSegment* ptdmSegment,
                            DWORD dwFlags, REFERENCE_TIME rtStartTime,
                            CtIDirectMusicSegmentState** pptdmSegmentState);
    virtual HRESULT Stop(CtIDirectMusicSegment* ptdmSegment,
                        CtIDirectMusicSegmentState* ptdmSegmentState,
                        MUSIC_TIME mtTime, DWORD dwFlags);
    virtual HRESULT GetSegmentState(CtIDirectMusicSegmentState** pptdmSegmentState,
                                    MUSIC_TIME mtTime);
    virtual HRESULT SetPrepareTime(DWORD dwMilliSeconds);
    virtual HRESULT GetPrepareTime(DWORD* pdwMilliSeconds);
    virtual HRESULT SetBumperLength(DWORD dwMilliSeconds);
    virtual HRESULT GetBumperLength(DWORD* pdwMilliSeconds);
    virtual HRESULT SendPMsg(DMUS_PMSG* pPMSG);
    virtual HRESULT MusicToReferenceTime(MUSIC_TIME mtTime,
                                            REFERENCE_TIME* prtTime);
    virtual HRESULT ReferenceToMusicTime(REFERENCE_TIME rtTime,
                                            MUSIC_TIME* pmtTime);
    virtual HRESULT IsPlaying(CtIDirectMusicSegment* ptdmSegment,
                            CtIDirectMusicSegmentState* ptdmSegState);
    virtual HRESULT GetTime(REFERENCE_TIME* prtNow, MUSIC_TIME* pmtNow);
    virtual HRESULT AllocPMsg(ULONG cb, DMUS_PMSG** ppPMSG);
    virtual HRESULT FreePMsg(DMUS_PMSG* pPMSG);
    virtual HRESULT GetGraph(CtIDirectMusicGraph** pptdmGraph);
    virtual HRESULT SetGraph(CtIDirectMusicGraph* ptdmGraph);
    virtual HRESULT SetNotificationHandle(HANDLE hNotification,
                                        REFERENCE_TIME rtMinimum);
    virtual HRESULT GetNotificationPMsg(DMUS_NOTIFICATION_PMSG** ppNotificationPMsg);
    virtual HRESULT AddNotificationType(REFGUID rguidNotificationType);
    virtual HRESULT RemoveNotificationType(REFGUID rguidNotificationType);
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
    virtual HRESULT Invalidate(MUSIC_TIME mtTime, DWORD dwFlags);
    virtual HRESULT GetParam(REFGUID rguidType, DWORD dwGroupBits,
                            DWORD dwIndex, MUSIC_TIME mtTime,
                            MUSIC_TIME* pmtNext, void* pData);
    virtual HRESULT SetParam(REFGUID rguidType, DWORD dwGroupBits,
                            DWORD dwIndex, MUSIC_TIME mtTime,
                            void* pData);
    virtual HRESULT GetGlobalParam(REFGUID rguidType, void* pData,
                                    DWORD dwSize);
    virtual HRESULT SetGlobalParam(REFGUID rguidType, void* pData,
                                    DWORD dwSize);
    virtual HRESULT GetLatencyTime(REFERENCE_TIME*);
    virtual HRESULT GetQueueTime(REFERENCE_TIME* prtTime);
    virtual HRESULT AdjustTime(REFERENCE_TIME rtAmount);
    virtual HRESULT CloseDown(void);
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

}; // ** end CtIDirectMusicPerformance

//===========================================================================
// CtIDirectMusicPerformance8
//
// Wrapper class for IDirectMusicPerformance8
//
// Inherits from CtIDirectMusicPerformance
//===========================================================================
class CtIDirectMusicPerformance8 : public CtIDirectMusicPerformance
{
    public:
    // constructor / destructor
    CtIDirectMusicPerformance8();
    ~CtIDirectMusicPerformance8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicPerformance8 *pdmPerformance);
    virtual HRESULT GetRealObjPtr(IDirectMusicPerformance8 **ppdmPerformance);
    // IDirectMusicPerformance8
    virtual HRESULT InitAudio(CtIDirectMusic** ppDirectMusic,               // Optional DMusic pointer.
                                            IDirectSound** ppDirectSound,           // Optional DSound pointer.
                                            HWND hWnd,                              // HWND for DSound.
                                            DWORD dwDefaultPathType,                // Requested default audio path type, also optional.
                                            DWORD dwPChannelCount,                  // Number of PChannels, if default audio path to be created.
                                            DWORD dwFlags,                          // DMUS_AUDIOF flags, if no pParams structure.
                                            DMUS_AUDIOPARAMS *pParams);             // Optional initialization structure, defining required voices, buffers, etc.
//PHOOPHOO
//  virtual HRESULT InitAudio(CtIDirectMusic** ppDirectMusic,                           // Optional
//                                           IDirectSound** ppDirectSound,              // Optional
//                                           HWND hWnd,                                 // Optional maybe
//                                           DWORD dwDefaultPathType,                   // Optional
//                                           DWORD dwPChannelCount,                     // Optional sometimes
//                                           DWORD dwSampleRate);                       // Optional maybe
    virtual HRESULT PlaySegmentEx(CtIUnknown* pSource,                             // Segment to play. Alternately, could be an IDirectMusicSong.
                                            CHAR *pzSegmentName,                  // If song, which segment in the song.
                                            CtIUnknown* pTransition,                  // Optional template segment to compose transition with.
                                            DWORD dwFlags,                          // DMUS_SEGF_ flags.
                                            __int64 i64StartTime,                   // Time to start playback.
                                            CtIDirectMusicSegmentState** ppSegmentState, // Returned Segment State.
                                            CtIUnknown *pFrom,                        // Optional segmentstate or audiopath to replace.
                                            CtIUnknown *pAudioPath);             // Optional audioPath to play on.
//PHOOPHOO
//  virtual HRESULT PlaySegmentEx(CtIUnknown* pSource,
//                                          DWORD dwSegmentID,
//                                          CtIUnknown* pTransition,                        // Optional
//                                          DWORD dwFlags,
//                                          __int64 i64StartTime,
//                                          CtIDirectMusicSegmentState** ppSegmentState,    // Optional
//                                          CtIUnknown *pFrom,                            // Optional
//                                          CtIUnknown *pAudioPath);                        // Optional
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
// CtIDirectMusicTool8
//
// Wrapper class for IDirectMusicTool8
//
// Inherits from CtIDirectMusicTool
//===========================================================================
class CtIDirectMusicTool8 : public CtIDirectMusicTool
{
    public:
    // constructor / destructor
    CtIDirectMusicTool8();
    ~CtIDirectMusicTool8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicTool8 *pdmTool8);
    virtual HRESULT GetRealObjPtr(IDirectMusicTool8 **ppdmTool8);
    // IDirectMusicTool8
    virtual HRESULT Clone(CtIDirectMusicTool ** ppTool);

}; // ** end CtIDirectMusicTool8

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
// CtIDirectMusicStyle
//
// Wrapper class for IDirectMusicStyle
//
// Inherits from CtIUnknown
//===========================================================================
/*
class CtIDirectMusicStyle : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicStyle();
    ~CtIDirectMusicStyle();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicStyle *pdmStyle);
    virtual HRESULT GetRealObjPtr(IDirectMusicStyle **ppdmStyle);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicStyle methods
    virtual HRESULT GetBand(WCHAR *pwszName, CtIDirectMusicBand** pptdmBand);
    virtual HRESULT EnumBand(DWORD dwIndex, WCHAR *pwszName);
    virtual HRESULT GetDefaultBand(CtIDirectMusicBand** pptdmBand);
    virtual HRESULT EnumMotif(DWORD dwIndex, WCHAR *pwszName);
    virtual HRESULT GetMotif(WCHAR *pwszName,
                            CtIDirectMusicSegment** pptdmSegment);
    virtual HRESULT GetDefaultChordMap(CtIDirectMusicChordMap** pptdmChordMap);
    virtual HRESULT EnumChordMap(DWORD dwIndex, WCHAR *pwszName);
    virtual HRESULT GetChordMap(WCHAR *pwszName,
                                CtIDirectMusicChordMap** pptdmChordMap);
    virtual HRESULT GetTimeSignature(DMUS_TIMESIGNATURE* pTimeSig);
    virtual HRESULT GetEmbellishmentLength(BYTE bType, BYTE bLevel, DWORD* pdwMin,
                                            DWORD* pdwMax);
    virtual HRESULT GetTempo(double* pTempo);

}; // ** end CtIDirectMusicStyle

//===========================================================================
// CtIDirectMusicStyle8
//
// Wrapper class for IDirectMusicStyle8
//
// Inherits from CtIDirectMusicStyle
//===========================================================================
class CtIDirectMusicStyle8 : public CtIDirectMusicStyle
{
    public:
    // constructor / destructor
    CtIDirectMusicStyle8();
    ~CtIDirectMusicStyle8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicStyle8 *pdmStyle8);
    virtual HRESULT GetRealObjPtr(IDirectMusicStyle8 **ppdmStyle8);
    // IDirectMusicStyle8
    virtual HRESULT EnumPattern(DWORD dwIndex,DWORD dwPatternType,WCHAR* pwszName);
//PHOOPHOO
//  virtual HRESULT ComposeMelodyFromTemplate(CtIDirectMusicStyle* pStyle,
//                                              CtIDirectMusicSegment* pTemplate,
//                                              CtIDirectMusicSegment** ppSegment);

}; // ** end CtIDirectMusicStyle8

//===========================================================================
// CtIDirectMusicChordMap
//
// Wrapper class for DirectMusicChordMap
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicChordMap : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicChordMap();
    ~CtIDirectMusicChordMap();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicChordMap *pdmChordMap);
    virtual HRESULT GetRealObjPtr(IDirectMusicChordMap **ppdmChordMap);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicChordMap methods
    virtual HRESULT GetScale(DWORD* pdwScale);

}; // ** end CtIDirectMusicChordMap

//===========================================================================
// CtIDirectMusicComposer
//
// Wrapper class for IDirectMusicComposer
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicComposer : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicComposer();
    ~CtIDirectMusicComposer();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicComposer *pdmComposer);
    virtual HRESULT GetRealObjPtr(IDirectMusicComposer **ppdmComposer);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicComposer methods
    virtual HRESULT ComposeSegmentFromTemplate(CtIDirectMusicStyle* ptdmStyle,
                                    CtIDirectMusicSegment* ptdmTempSeg,
                                    WORD wActivity,
                                    CtIDirectMusicChordMap* ptdmChordMap,
                                    CtIDirectMusicSegment** pptdmSectionSeg);
    virtual HRESULT ComposeSegmentFromShape(CtIDirectMusicStyle* ptdmStyle,
                                    WORD wNumMeasures,
                                    WORD wShape,
                                    WORD wActivity,
                                    BOOL fIntro, BOOL fEnd,
                                    CtIDirectMusicChordMap* ptdmChordMap,
                                    CtIDirectMusicSegment** pptdmSectionSeg);
    virtual HRESULT ComposeTransition(CtIDirectMusicSegment* ptdmFromSeg,
                                    CtIDirectMusicSegment* ptdmToSeg,
                                    WORD wMeasureNum, WORD wCommand,
                                    DWORD dwFlags,
                                    CtIDirectMusicChordMap* ptdmChordMap,
                                    CtIDirectMusicSegment** pptdmSectionSeg);
    virtual HRESULT AutoTransition(CtIDirectMusicPerformance* ptdmPerformance,
                                    CtIDirectMusicSegment* pToSeg,
                                    WORD wCommand,
                                    DWORD dwFlags,
                                    CtIDirectMusicChordMap* ptdmChordMap,
                                    CtIDirectMusicSegment** pptdmTransSeg,
                                    CtIDirectMusicSegmentState** pptdmToSegState,
                                    CtIDirectMusicSegmentState** pptdmfsTransSegState);
    virtual HRESULT ComposeTemplateFromShape(WORD wNumMeasures,
                                    WORD wShape, BOOL fIntro,
                                    BOOL fEnd, WORD wEndLength,
                                    CtIDirectMusicSegment** pptdmTempSeg);
    virtual HRESULT ChangeChordMap(CtIDirectMusicSegment* ptdmSectionSeg,
                                    BOOL fTrackScale,
                                    CtIDirectMusicChordMap* ptdmChordMap);

}; // ** end CtIDirectMusicComposer

//===========================================================================
// CtIDirectMusicComposer8
//
// Wrapper class for IDirectMusicComposer8
//
// Inherits from CtIDirectMusicComposer
//===========================================================================
/*
class CtIDirectMusicComposer8 : public CtIDirectMusicComposer
{
    public:
    // constructor / destructor
    CtIDirectMusicComposer8();
    ~CtIDirectMusicComposer8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicComposer8 *pdmComposer8);
    virtual HRESULT GetRealObjPtr(IDirectMusicComposer8 **ppdmComposer8);
    // the IDirectMusicComposer8 methods
    virtual HRESULT ComposeSegmentFromTemplateEx(CtIDirectMusicStyle* pStyle,
                                    CtIDirectMusicSegment* pTemplate,
                                    DWORD dwFlags,
                                    DWORD dwActivity,
                                    CtIDirectMusicChordMap* pChordMap,
                                    CtIDirectMusicSegment** ppSegment);

//  virtual HRESULT ComposeTemplateFromShapeEx(WORD wNumMeasures,
//                                    WORD wShape,
//                                    BOOL fIntro,
//                                    BOOL fEnd,
//                                    CtIDirectMusicStyle* pStyle,
//                                    CtIDirectMusicSegment** ppTemplate);


}; // ** end CtIDirectMusicComposer8
*/
//===========================================================================
// CtIDirectMusicSynth
//
// Wrapper class for IDirectMusicSynth
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicSynth : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSynth();
    ~CtIDirectMusicSynth();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSynth *pdmSynth);
    virtual HRESULT GetRealObjPtr(IDirectMusicSynth **ppdmSynth);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicSynth methods
    virtual HRESULT Open(LPDMUS_PORTPARAMS pPortParams);
    virtual HRESULT Close(void);
    virtual HRESULT SetNumChannelGroups(DWORD dwGroups);
    virtual HRESULT Download(LPHANDLE phDownload,
                            LPVOID pvData, LPBOOL pbFree);
    virtual HRESULT Unload(HANDLE hDownload,
                            HRESULT ( CALLBACK *lpFreeHandle)(HANDLE,HANDLE),
                            HANDLE hUserData);
    virtual HRESULT PlayBuffer(REFERENCE_TIME rt,
                                LPBYTE pbBuffer, DWORD cbBuffer);
    virtual HRESULT GetRunningStats(LPDMUS_SYNTHSTATS pStats);
    virtual HRESULT GetPortCaps(LPDMUS_PORTCAPS pCaps);
    virtual HRESULT SetMasterClock(CtIReferenceClock *ptdmClock);
    virtual HRESULT GetLatencyClock(CtIReferenceClock **pptdmClock);
    virtual HRESULT Activate(BOOL fEnable);
    virtual HRESULT SetSynthSink(CtIDirectMusicSynthSink *ptdmSynthSink);
    virtual HRESULT Render(short *pBuffer, DWORD dwLength, DWORD dwPosition);
    virtual HRESULT SetChannelPriority(DWORD dwChannelGroup,DWORD dwChannel,DWORD dwPriority);
    virtual HRESULT GetChannelPriority(DWORD dwChannelGroup,DWORD dwChannel,LPDWORD pdwPriority);
    virtual HRESULT GetFormat(LPWAVEFORMATEX pWaveFormatEx, LPDWORD pdwWaveFormatExSize);
    virtual HRESULT GetAppend(DWORD* pdwAppend);

}; // ** end CtIDirectMusicSynth

//===========================================================================
// CtIDirectMusicSynth8
//
// Wrapper class for IDirectMusicSynth8
//
// Inherits from CtIDirectMusicSynth
//===========================================================================
class CtIDirectMusicSynth8 : public CtIDirectMusicSynth
{
    public:
    // constructor / destructor
    CtIDirectMusicSynth8();
    ~CtIDirectMusicSynth8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSynth8 *pdmSynth8);
    virtual HRESULT GetRealObjPtr(IDirectMusicSynth8 **ppdmSynth8);
    // IDirectMusicSynth8
//PHOOPHOO
//    virtual HRESULT PlayVoice(REFERENCE_TIME rt,DWORD dwVoiceId,DWORD dwChannelGroup,DWORD dwChannel,DWORD dwDLId,long  prPitch,long  vrVolume);
    virtual HRESULT PlayVoice(REFERENCE_TIME rt,
                               DWORD dwVoiceId,
                               DWORD dwChannelGroup,
                               DWORD dwChannel,
                               DWORD dwDLId,
                               long  prPitch,           //PREL not defined here
                               long vrVolume,          //VREL not defined here
                               SAMPLE_TIME stVoiceStart,
                               SAMPLE_TIME stLoopStart,
                               SAMPLE_TIME stLoopEnd);
    virtual HRESULT StopVoice(REFERENCE_TIME rt,DWORD dwVoiceId);
//PHOOPHOO
//    virtual HRESULT GetVoicePosition(DWORD dwVoice[],DWORD cbVoice,SAMPLE_POSITION dwVoicePos[]);
    virtual HRESULT Refresh(DWORD dwDownloadID,DWORD dwFlags);
    virtual HRESULT AssignChannelToBuses(DWORD dwChannelGroup,DWORD dwChannel,LPDWORD pdwBusses,DWORD cBusses);

}; // ** end CtIDirectMusicSynth8

//===========================================================================
// CtIDirectMusicBand
//
// Wrapper class for IDirectMusicBand
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicBand : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicBand();
    ~CtIDirectMusicBand();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicBand *pdmBand);
    virtual HRESULT GetRealObjPtr(IDirectMusicBand **ppdmBand);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicBand methods
    virtual HRESULT CreateSegment(CtIDirectMusicSegment** pptdmSegment);
    virtual HRESULT Download(CtIDirectMusicPerformance* ptdmPerformance);
    virtual HRESULT Unload(CtIDirectMusicPerformance* ptdmPerformance);

}; // ** end CtIDirectMusicBand

//===========================================================================
// CtIDirectMusicBand8
//
// Wrapper class for IDirectMusicBand8
//
// Inherits from CtIDirectMusicBand
//===========================================================================
/*
class CtIDirectMusicBand8 : public CtIDirectMusicBand
{
    public:
    // constructor / destructor
    CtIDirectMusicBand8();
    ~CtIDirectMusicBand8();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicBand8 *pdmBand8);
    virtual HRESULT GetRealObjPtr(IDirectMusicBand8 **ppdmBand8);
    // IDirectMusicBand8
    virtual HRESULT DownloadEx(CtIUnknown *pAudioPath);
    virtual HRESULT UnloadEx(CtIUnknown *pAudioPath);

}; // ** end CtIDirectMusicBand
*/
//===========================================================================
// CtIDirectMusicGetLoader
//
// Wrapper class for IDirectMusicGetLoader
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicGetLoader : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicGetLoader();
    ~CtIDirectMusicGetLoader();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicGetLoader *pdmGetLdr);
    virtual HRESULT GetRealObjPtr(IDirectMusicGetLoader **ppdmGetLdr);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicGetLoader methods
    virtual HRESULT GetLoader(CtIDirectMusicLoader **pptdmLoader);

}; // ** end CtIDirectMusicGetLoader

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
/*
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
    virtual HRESULT CallRoutine(WCHAR *pwszRoutineName, DMUS_SCRIPT_ERRORINFO *pdmScriptErrorInfo);
    virtual HRESULT SetVariableVariant(WCHAR *pwszVariableName,
                                               VARIANT varValue,
                                               BOOL fSetRef,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT GetVariableVariant(WCHAR *pwszVariableName,
                                               VARIANT *pvarValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT SetVariableNumber(WCHAR *pwszVariableName,
                                               LONG lValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT GetVariableNumber(WCHAR *pwszVariableName,
                                               LONG *plValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT SetVariableObject(WCHAR *pwszVariableName,
                                               IUnknown *punkValue,
                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT GetVariableObject(WCHAR *pwszVariableName,
                                               REFIID riid,
                                               LPVOID FAR *ppv,
                                               DMUS_SCRIPT_ERRORINFO *pErrorInfo);
//  virtual HRESULT GetVariableObject(WCHAR *pwszVariableName,
//                                               IUnknown **ppunkValue,
//                                               DMUS_SCRIPT_ERRORINFO* pdmScriptErrorInfo);
    virtual HRESULT EnumRoutine(DWORD dwIndex,WCHAR *pwszName);
    virtual HRESULT EnumVariable(DWORD dwIndex,WCHAR *pwszName);

};

//===========================================================================
// CtIDirectMusicScriptError
//
// Wrapper class for IDirectMusicScriptError
//
// Inherits from CtIUnknown
//===========================================================================
/*
//PHOOPHOO
class CtIDirectMusicScriptError : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicScriptError();
    ~CtIDirectMusicScriptError();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicScriptError *pdmScriptError);
    virtual HRESULT GetRealObjPtr(IDirectMusicScriptError **ppdmScriptError);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicScriptError methods
    virtual HRESULT GetError(DMUS_SCRIPT_ERRORINFO *pErrorInfo);

};
*/
/*
//===========================================================================
// CtIDirectMusicSong
//
// Wrapper class for IDirectMusicSong
//
// Inherits from CtIUnknown
//===========================================================================
class CtIDirectMusicSong : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectMusicSong();
    ~CtIDirectMusicSong();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectMusicSong *pdmSong);
    virtual HRESULT GetRealObjPtr(IDirectMusicSong **ppdmSong);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectMusicSong methods
    virtual HRESULT Compose(void);
    virtual HRESULT GetParam(REFGUID rguidType,
                             DWORD dwGroupBits,
                             DWORD dwIndex,
                             MUSIC_TIME mtTime,
                             MUSIC_TIME* pmtNext,
                             void* pParam);
//  virtual HRESULT EnumSegment(DWORD dwIndex,CtIDirectMusicSegment **ppSegment);
//PHOOPHOO
//  virtual HRESULT Clone(CtIDirectMusicSong **ppSong);
    virtual HRESULT GetSegment (WCHAR* pwzName, CtIDirectMusicSegment **ppSegment);
    virtual HRESULT GetAudioPathConfig(CtIUnknown ** ppAudioPathConfig);
    virtual HRESULT Download(CtIUnknown *pAudioPath);
    virtual HRESULT Unload(CtIUnknown *pAudioPath);

};
*/
//===========================================================================
// CtIDirectSoundWave
//
// Wrapper class for DMIME's IDirectSoundWave
//
// Inherits from CtIUnknown
//===========================================================================
/*
//PHOOPHOO
class CtIDirectSoundWave : public CtIUnknown
{
    public:
    // constructor / destructor
    CtIDirectSoundWave();
    ~CtIDirectSoundWave();
    // test class helpers
    virtual HRESULT InitTestClass(IDirectSoundWave *pdmSegmentState);
    virtual HRESULT GetRealObjPtr(IDirectSoundWave **ppdmSegmentState);
    // the IUnknown's
    virtual HRESULT QueryInterface(REFIID riid, LPVOID *ppvObj);
    // the IDirectSoundWave methods
//PHOOPHOO
//    virtual HRESULT GetFormat(LPWAVEFORMATEX pwfx, LPDWORD pcbSize);
    virtual HRESULT GetFormat(LPWAVEFORMATEX pwfxFormat, DWORD dwSizeAllocated, LPDWORD pdwSizeWritten);
//PHOOPHOO
//    virtual HRESULT GetWaveArticulation(LPDMUS_WAVEART pArticulation);
    virtual HRESULT CreateSource(IDirectSoundSource **ppSource, LPWAVEFORMATEX pwfx, DWORD dwFlags);
}; // ** end CtIDirectSoundWave
*/


#ifdef __cplusplus

//===========================================================================
// test class GUIDs.
//
// These allow us to mascquerade as the real interfaces
//===========================================================================

DEFINE_GUID(CTIID_IUnknown,                             0x88e62400, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusic,                         0x88e62401, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicBand,                     0x88e62402, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicBuffer,                   0x88e62403, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicCollection,               0x88e62404, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicComposer,                 0x88e62405, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicDownload,                 0x88e62406, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicDownloadedInstrument,     0x88e62407, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicGraph,                    0x88e62408, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicInstrument,               0x88e6240A, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicLoader,                   0x88e6240B, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicObject,                   0x88e6240C, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicPerformance,              0x88e6240D, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicChordMap,                 0x88e6240E, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicPort,                     0x88e6240F, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicPortDownload,             0x88e62410, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicSegment,                  0x88e62411, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicSegmentState,             0x88e62412, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicStyle,                    0x88e62413, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicSynth,                    0x88e62414, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicSynthSink,                0x88e62415, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicTool,                     0x88e62416, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicTrack,                    0x88e62417, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IReferenceClock,                      0x88e62418, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);
DEFINE_GUID(CTIID_IDirectMusicGetLoader,                0x88e62420, 0xc41b, 0x11d1, 0x8a, 0xfb, 0x0, 0xaa, 0x0, 0x44, 0x36, 0xf9);

//dx7
DEFINE_GUID(CTIID_IDirectMusicPerformance2,             0x7153d4c9, 0x5d74, 0x45dc, 0x8d, 0xef, 0x64, 0xc1, 0x3c, 0xb7, 0xed, 0xa1);
DEFINE_GUID(CTIID_IDirectMusicSegment2,                 0x224c56e7, 0xdee, 0x49c7, 0xa5, 0x4e, 0x95, 0xf9, 0xfe, 0x7b, 0xdb, 0xfc);

//dx8
DEFINE_GUID(CTIID_IDirectMusic8,                        0xb0ee2a0c, 0x152a, 0x4729, 0x8b, 0xfb, 0x3d, 0xd4, 0x8a, 0xd2, 0x5d, 0x3a);
//DEFINE_GUID(CTIID_IDirectMusicBand8,                  0x8da34241, 0x365d, 0x4865, 0x83, 0x1a, 0x57, 0x2d, 0x33, 0x7, 0x6a, 0x56);
//DEFINE_GUID(CTIID_IDirectMusicVoice,                  0xc06836cc, 0xeefe, 0x42da, 0xa2, 0xaa, 0xc5, 0xfc, 0xcd, 0x90, 0x4e, 0xfd);
//PHOOPHOO
//DEFINE_GUID(CTIID_IDirectMusicParamHook,              0xd639af13, 0xede6, 0x4258, 0xb3, 0x13, 0xfb, 0x85, 0xf9, 0x98, 0x6f, 0xa6);
DEFINE_GUID(CTIID_IDirectMusicPatternTrack,             0xb30e0e2b, 0x5347, 0x4d4a, 0x9c, 0xcb, 0xcd, 0x7e, 0xba, 0x28, 0x9c, 0x33);
//DEFINE_GUID(CTIID_IDirectSoundDownloadedWave,         0xb491d035, 0x559b, 0x42c9, 0xbf, 0xa2, 0x23, 0xca, 0xf6, 0xf1, 0xec, 0x7d);
//DEFINE_GUID(CTIID_IDirectSoundWave,                     0x0c3145e8, 0x9b0c, 0x447a, 0x8f, 0xda, 0x58, 0xff, 0x74, 0x6c, 0xca, 0xb7);

DEFINE_GUID(CTIID_IDirectMusicSegment8,                 0xf65da357, 0x7cfb, 0x4cc7, 0xbe, 0x9d, 0x6, 0x25, 0x14, 0xd5, 0xcf, 0x18);
DEFINE_GUID(CTIID_IDirectMusicLoader8,                  0xb48f6473, 0x759f, 0x472d, 0x8c, 0xa0, 0x47, 0x90, 0x7a, 0xe3, 0x6f, 0x87);
//DEFINE_GUID(CTIID_IDirectMusicObject8,                    0xe92c35c, 0x4980, 0x40af, 0x99, 0x9f, 0x64, 0x65, 0x99, 0xcd, 0x8d, 0x1c);
DEFINE_GUID(CTIID_IDirectMusicPerformance8,             0x5cffd8de, 0x3d40, 0x4020, 0xac, 0x3e, 0x6d, 0xd2, 0x7, 0x92, 0x98, 0x1d);
DEFINE_GUID(CTIID_IDirectMusicTool8,                    0xaa534b4d, 0xf3b4, 0x4bf9, 0x9c, 0x50, 0x3e, 0x4c, 0x76, 0x79, 0x62, 0x36);
DEFINE_GUID(CTIID_IDirectMusicStyle8,                   0x29af2285, 0x4eeb, 0x4227, 0x93, 0x6e, 0x26, 0xfc, 0x2f, 0x91, 0xc, 0x4d);
//DEFINE_GUID(CTIID_IDirectMusicComposer8,              0x31149e2a, 0xe60c, 0x4557, 0x9c, 0xe2, 0x1f, 0xb5, 0x83, 0xfa, 0x82, 0x4e);
DEFINE_GUID(CTIID_IDirectMusicSegmentState8,            0x7af67385, 0x94db, 0x4f5e, 0x99, 0xe4, 0xec, 0xdd, 0xb2, 0x67, 0x76, 0x5b);
//DEFINE_GUID(CTIID_IDirectMusicPort8,                  0x89ea06d2, 0x10d1, 0x4d7e, 0x8d, 0x80, 0x2, 0x4b, 0xf6, 0xc9, 0x45, 0x9a);
DEFINE_GUID(CTIID_IDirectMusicSynth8,                   0xbfe8d92d, 0xc522, 0x4279, 0xb3, 0x48, 0xe8, 0x73, 0x2d, 0xb6, 0xab, 0xc3);
DEFINE_GUID(CTIID_IDirectMusicTrack8,                   0x689dc439, 0xfdfd, 0x4691, 0xb1, 0xe9, 0xcb, 0xda, 0x73, 0xcd, 0x68, 0x6f);
DEFINE_GUID(CTIID_IDirectMusicContainer,                0x727c098a, 0xf55e, 0x4450, 0xbd, 0xf5, 0xb5, 0x29, 0x14, 0xdf, 0x4a, 0xc2);
DEFINE_GUID(CTIID_IDirectMusicSong,                     0xfcba94e2, 0xfe1f, 0x4dfa, 0x8a, 0xbc, 0x5b, 0xf, 0x98, 0xdb, 0x46, 0xf5);
DEFINE_GUID(CTIID_IDirectMusicScript,                   0x8aa10b67, 0x1894, 0x46dc, 0xb5, 0xa0, 0x50, 0x6d, 0x3, 0xf5, 0x50, 0x1f);
DEFINE_GUID(CTIID_IDirectMusicScriptError,              0xfb8c07db, 0x80f5, 0x4f32, 0xbd, 0xce, 0x80, 0xcc, 0x75, 0x35, 0xb9, 0x60);
DEFINE_GUID(CTIID_IDirectMusicAudioPath,                0x50a0078d, 0x687e, 0x4a26, 0x8b, 0x83, 0x1d, 0x16, 0x3b, 0x5c, 0xd2, 0xf6);

#endif // __cplusplus


// ======================================================================================
//
//   ALIASES FOR INTERFACES THAT HAVE NO DX8 EQUIVALENT
//
// ======================================================================================

#define CTIID_IDirectMusicGetLoader8 CTIID_IDirectMusicGetLoader
#define CTIID_IDirectMusicAudioPath8 CTIID_IDirectMusicAudioPath
//PHOOPHOO
//#define CTIID_IDirectMusicParamHook8 CTIID_IDirectMusicParamHook
#define CTIID_IDirectMusicGraph8 CTIID_IDirectMusicGraph
#define CTIID_IDirectMusicChordMap8 CTIID_IDirectMusicChordMap
#define CTIID_IDirectMusicPatternTrack8 CTIID_IDirectMusicPatternTrack
#define CTIID_IDirectMusicScript8 CTIID_IDirectMusicScript
#define CTIID_IDirectMusicScriptError8 CTIID_IDirectMusicScriptError
#define CTIID_IDirectMusicContainer8 CTIID_IDirectMusicContainer
#define CTIID_IDirectMusicSong8 CTIID_IDirectMusicSong

// ======================================================================================
//
//   TYPEDEF FOR INTERFACES THAT HAVE NO DX8 EQUIVALENT
//
// ======================================================================================

typedef CtIDirectMusicGetLoader CtIDirectMusicGetLoader8;
typedef CtIDirectMusicAudioPath CtIDirectMusicAudioPath8;
//PHOOPHOO
//typedef CtIDirectMusicParamHook CtIDirectMusicParamHook8;
typedef CtIDirectMusicGraph CtIDirectMusicGraph8;
typedef CtIDirectMusicChordMap CtIDirectMusicChordMap8;
typedef CtIDirectMusicPatternTrack CtIDirectMusicPatternTrack8;
typedef CtIDirectMusicScript CtIDirectMusicScript8;
//PHOOPHOO
//typedef CtIDirectMusicScriptError CtIDirectMusicScriptError8;  REMOVED**************
typedef CtIDirectMusicContainer CtIDirectMusicContainer8;
// typedef CtIDirectMusicSong CtIDirectMusicSong8;

// ======================================================================================
//
//    CREATE OBJECT HELPERS
//
// ======================================================================================

HRESULT _stdcall dmthCreateDMBaseObj(REFIID riid,CtIDirectMusic **pptdm);
HRESULT _stdcall dmthCreateDMBaseObj(REFIID riid,CtIDirectMusic8 **pptdm);
HRESULT _stdcall dmthCreateLoader(REFIID riid,CtIDirectMusicLoader **ppwLoader);
HRESULT _stdcall dmthCreateLoader(REFIID riid,CtIDirectMusicLoader8 **ppwLoader);
HRESULT _stdcall dmthCreatePort(REFIID riid,
                                REFGUID rguidPort,
                                CtIDirectMusic *ptdm,
                                CtIDirectMusicPort **pptPort);
HRESULT _stdcall dmthCreateCollection(REFIID riid,
                                CtIDirectMusicCollection** pwCollection,
                                LPWSTR pwszPath,
                                LPWSTR pwszFileName);

// ======================================================================================
//
//    LOG STRUCTURE HELPERS
//
// ======================================================================================

void _stdcall dmthLogDMUS_OBJECTDESCStruct(DMUS_OBJECTDESC  Desc);
void _stdcall tdmLogDMUS_PORTPARAMS(UINT uLogLevel, DMUS_PORTPARAMS *pdmpp);
void _stdcall tdmLogDMUS_PORTCAPS(UINT uLogLevel, DMUS_PORTCAPS *pdmpc);
void _stdcall dmthLogDMUS_SCRIPT_ERRORINFO(UINT uLogLevel, DMUS_SCRIPT_ERRORINFO *pdmScriptErrorInfo);
void _stdcall dmthLogWAVEFORMATEX(UINT uLogLevel, LPWAVEFORMATEX lpwfx);
void _stdcall dmthLogFormatTag(UINT uLogLevel, WORD wFmt);
void _stdcall dmthLogRawGUID(UINT uLogLevel, REFGUID rguid);
//PHOOPHOO
//void _stdcall dmthLogDMUS_WAVEART(UINT uLogLevel, LPDMUS_WAVEART pArticulation);

// ======================================================================================
//
//    LEGACY DEFINES
//
// ======================================================================================

//BUGBUG - FIX THESE IN ALL TEST CASES
#define tdmCreateDMBaseObj dmthCreateDMBaseObj
#define dmthLogDMUS_OBJECTDESCS dmthLogDMUS_OBJECTDESCStruct
#define dmthLogDMUS_PORTPARAMS tdmLogDMUS_PORTPARAMS
#define dmthLogDMUS_PORTCAPS tdmLogDMUS_PORTCAPS









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
void _stdcall Log_DSBCaps (int, LPDSBCAPS);
void _stdcall Log_DSCBCaps (int, LPDSCBCAPS);
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
void _stdcall Log_DSCCaps (int, LPDSCCAPS);
void _stdcall Log_DSCBufferDesc (int, LPCDSCBUFFERDESC);
void _stdcall Log_DSBStatus (int, DWORD);
void _stdcall Log_DSCBStatus (int, DWORD);
void _stdcall Log_DSCBStartFlags (int, DWORD);
void _stdcall Log_DSNPositionNotifies (int, DWORD, LPCDSBPOSITIONNOTIFY);
void _stdcall Log_DSBCapsFlags(int, DWORD);
void _stdcall Log_DSCBCapsFlags(int, DWORD);
void _stdcall Log_wFormatTag (int, WORD);
DWORD _stdcall GetRandomDWORD(DWORD dwModulus);
D3DVALUE _stdcall GetRandomD3DVALUE(D3DVALUE flLower, D3DVALUE flUpper, D3DVALUE flResolution);


// ======================================================================================
//
//    DSOUND8 LOGGING FUNCTIONS!!!
//
// ======================================================================================
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


//Eventually move all the "equal" param-checkers to this method.
#define EXPECT_EQUALITY   0x2345
#define EXPECT_INEQUALITY 0x3456
HRESULT _stdcall Compare_DS3DBUFFER(int iLogLevel, DS3DBUFFER *p1, DS3DBUFFER *p2, DWORD dwExpected);
HRESULT _stdcall Compare_DS3DLISTENER(int iLogLevel, DS3DLISTENER *p1, DS3DLISTENER *p2, DWORD dwExpected);



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


// ======================================================================================
//
//
//
// ======================================================================================
#if 0
#define  WAVE_FORMAT_DRM                        0x0009 /* Microsoft Corporation */
typedef struct drmwaveformat_tag {
    WAVEFORMATEX    wfx;
    WORD        wReserved;
    ULONG       ulContentId;
    WAVEFORMATEX    wfxSecure;
} DRMWAVEFORMAT;
#endif

#define SAFE_RELEASE( px )  if( px )                            \
                            {                                   \
                            px->Release();                      \
                            px = NULL;                          \
                            }

//danhaff:
//BUGBUG: We will get a "redefinition" error when manbug 31602 is fixed.  At that point, this line
//should be removed.
#define D3DVAL(val) ((float)(val))


#endif // _DMTHCLS_H




