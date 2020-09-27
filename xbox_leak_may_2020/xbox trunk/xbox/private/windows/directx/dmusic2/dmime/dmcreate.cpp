#include "pchime.h"
#include <xdbg.h>

CRITICAL_SECTION		g_APICriticalSection;	
long					g_lCritSecCount;

/*#include <xtl.h>
#include <dmusicc.h>
#include <dmusici.h>

extern "C" ULONG __cdecl DbgPrint(PCH Format, ...);
#include "..\shared\xcreate.h"
#include "..\shared\dmscriptautguids.h"*/

void DbgPrintGUID ( REFIID riid)
{
    DbgPrint("(0x%08lX,0x%04X,0x%04X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X)",
        riid.Data1,
        riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2],
        riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);
}

class CClassTracker 
{
public:
    CClassTracker() { m_pNext = NULL; m_dwCount = 1; m_fSaved = FALSE; }
    CClassTracker *     m_pNext;
    DWORD               m_dwCount;
    char *              m_pszClassName;
    char *              m_pszFunctionName;
    char *              m_pszComment;
    BOOL                m_fSaved;
};

static CClassTracker *g_pClassList = NULL;

void InsertClass(char *pszClassName, char *pszFunctionName, char *pszComment)

{
    CClassTracker *pScan = g_pClassList;
    for (;pScan;pScan = pScan->m_pNext)
    {
        if (pScan->m_pszClassName == pszClassName)
        {
            pScan->m_dwCount++;
            return;
        }
    }
    pScan = new CClassTracker;
    if (pScan)
    {
        pScan->m_pszClassName = pszClassName;
        pScan->m_pszFunctionName = pszFunctionName;
        pScan->m_pszComment = pszComment;
        pScan->m_pNext = g_pClassList;
        g_pClassList = pScan;
    }
}

void CreateClassLinkFile(char *pszFileName)

{
    HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
		DWORD dwBytesWritten;
        char *szHeader[6] = {
            "HRESULT DirectMusicCreateInstance(REFCLSID clsid,\n",
            "                                  LPUNKNOWN pUnkOuter,\n", 
            "                                  REFIID iid,\n", 
            "                                  LPVOID *ppvInterface)\n",
            "{\n",
            "    HRESULT hr = E_FAIL;\n"
        };
        for (DWORD dwI = 0; dwI < 6; dwI++)
        {
            WriteFile(hFile,szHeader[dwI],strlen(szHeader[dwI]),&dwBytesWritten,NULL);
        }
        CClassTracker *pScan = g_pClassList;
        BOOL fFirst = TRUE;
        for (;;)
        {
            CClassTracker *pBestSoFar = NULL;
            DWORD dwMaxCount = 0;
            for (pScan = g_pClassList;pScan;pScan = pScan->m_pNext)
            {
                if (!pScan->m_fSaved)
                {
                    if (pScan->m_dwCount > dwMaxCount)
                    {
                        pBestSoFar = pScan;
                        dwMaxCount = pScan->m_dwCount;
                    }
                }
            }
            if (!pBestSoFar)
            {
                break;
            }
            pBestSoFar->m_fSaved = TRUE;
			if (fFirst)
			{
				WriteFile(hFile,"    ",4,&dwBytesWritten,NULL);
			}
            if (!fFirst)
            {
                WriteFile(hFile,"    else ",9,&dwBytesWritten,NULL);
            }
            fFirst = FALSE;
            char szData[400];
            wsprintfA(szData,"if (clsid == %s)\n    {\n        // %s.\n        hr = %s;\n    }\n",
                pBestSoFar->m_pszClassName,pBestSoFar->m_pszComment,pBestSoFar->m_pszFunctionName);
            WriteFile(hFile,szData,strlen(szData),&dwBytesWritten,NULL);
        }
        char *szTail[2] = {
            "    return hr;\n",
            "}\n", 
        };
        for (dwI = 0; dwI < 2; dwI++)
        {
            WriteFile(hFile,szTail[dwI],strlen(szTail[dwI]),&dwBytesWritten,NULL);
        }
        CloseHandle(hFile);
    }
}

#define BASEMIDI
#define ADDSEGMENT
#define ADDWAVE
#define ADDSTYLE
#define ADDCOMPOSER
#define ADDMELODY
#define ADDSCRIPT
#define ADDMISC

LPDIRECTMUSICFACTORYFN gDirectMusicFactoryFn;
void DirectMusicInitializedCheck();

HRESULT WINAPI DirectMusicCreateInstance(REFCLSID clsid, LPUNKNOWN pUnkOuter, REFIID iid, LPVOID *ppvInterface)
{
    DirectMusicInitializedCheck();
    return (*gDirectMusicFactoryFn)(clsid, pUnkOuter, iid, ppvInterface);
}

STDAPI_(HRESULT) DirectMusicRegisterFactory(LPDIRECTMUSICFACTORYFN pFactory){
    ASSERTMSG("Another factory is already registered.", !gDirectMusicFactoryFn);
    gDirectMusicFactoryFn = pFactory;
    return S_OK;
}

HRESULT CALLBACK DirectMusicDefaultFactory(REFCLSID clsid, LPUNKNOWN pUnkOuter, REFIID iid, LPVOID *ppvInterface)
{
    HRESULT hr = E_FAIL;
    static fFirstTime = TRUE;
    if (fFirstTime)
    {
        InitializeCriticalSection(&g_APICriticalSection);
		g_lCritSecCount = 0;
        fFirstTime = FALSE;
    }
    char * pszClassName = "";
    char * pszFunctionName = "";
    char * pszComment = "";
#ifdef BASEMIDI
/*    if (clsid == CLSID_DirectMusic)
    {
        pszClassName = "CLSID_DirectMusic";
        pszFunctionName = "DirectMusicCreate(iid,ppvInterface)";
        pszComment = "DirectMusic: core timing and port code is always required";
        hr = DirectMusicCreate(iid,ppvInterface);
    }
    else */
    if (clsid == CLSID_DirectMusicCollection)
    {
        pszClassName = "CLSID_DirectMusicCollection";
        pszFunctionName = "DirectMusicCollectionCreate(iid,ppvInterface)";
        pszComment = "DLS Collection: manages a DLS file - always required for MIDI + DLS";
        hr = DirectMusicCollectionCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSynth)
    {
        pszClassName = "CLSID_DirectMusicSynth";
        pszFunctionName = "DirectMusicSynthCreate(iid,ppvInterface)";
        pszComment = "Synthesizer: controls the hardware synthesizer - always required";
        hr = DirectMusicSynthCreate(iid,ppvInterface);
    }
#endif
#ifdef ADDSEGMENT
    else if (clsid == CLSID_DirectMusicFileStream)
    {
        // File Stream - used to read content from disk
        pszClassName = "CLSID_DirectMusicFileStream";
        pszFunctionName = "DirectMusicFileStreamCreate(iid,ppvInterface)";
        pszComment = "File Stream - used to read content from disk";
        hr = DirectMusicFileStreamCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicStreamStream)
    {
        // Stream wrapper - used to read content from app supplied IStream
        pszClassName = "CLSID_DirectMusicStreamStream";
        pszFunctionName = "DirectMusicStreamStreamCreate(iid,ppvInterface)";
        pszComment = "Stream wrapper - used to read content from app supplied IStream";
        hr = DirectMusicStreamStreamCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicMemStream)
    {
        // Memory Stream - used to read content from memory
        pszClassName = "CLSID_DirectMusicMemStream";
        pszFunctionName = "DirectMusicMemStreamCreate(iid,ppvInterface)";
        pszComment = "Memory Stream - used to read content from memory";
        hr = DirectMusicMemStreamCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSegment)
    {
        // Segment - primary playback mechanism - always required
        pszClassName = "CLSID_DirectMusicSegment";
        pszFunctionName = "DirectMusicSegmentCreate(iid,ppvInterface)";
        pszComment = "Segment - primary playback mechanism - always required";
        hr = DirectMusicSegmentCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicLoader)
    {
        pszClassName = "CLSID_DirectMusicLoader";
        pszFunctionName = "DirectMusicLoaderCreate(iid,ppvInterface)";
        pszComment = "Loader: required to read files from disk and link objects. May be replaced by application";
        hr = DirectMusicLoaderCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicTempoTrack)
    {
        pszClassName = "CLSID_DirectMusicTempoTrack";
        pszFunctionName = "DirectMusicTempoTrackCreate(iid,ppvInterface)";
        pszComment = "Tempo Track - defines tempo changes, always required for music";
        hr = DirectMusicTempoTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicTimeSigTrack)
    {
        pszClassName = "CLSID_DirectMusicTimeSigTrack";
        pszFunctionName = "DirectMusicTimeSigTrackCreate(iid,ppvInterface)";
        pszComment = "Time Signature Track - defines time signature in segments, always required for music synchronization";
        hr = DirectMusicTimeSigTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicBandTrack)
    {
        pszClassName = "CLSID_DirectMusicBandTrack";
        pszFunctionName = "DirectMusicBandTrackCreate(iid,ppvInterface)";
        pszComment = "Band Track - manages one or more bands - always required for any MIDI + DLS playback";
        hr = DirectMusicBandTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicPerformance)
    {
        pszClassName = "CLSID_DirectMusicPerformance";
        pszFunctionName = "DirectMusicPerformanceCreate(iid,ppvInterface)";
        pszComment = "Performance - segment playback framework, always required";
        hr = DirectMusicPerformanceCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicBand)
    {
        pszClassName = "CLSID_DirectMusicBand";
        pszFunctionName = "DirectMusicBandCreate(iid,ppvInterface)";
        pszComment = "Band - manages downloading of DLS instruments as well as volume and pan presets - always required for MIDI + DLS";
        hr = DirectMusicBandCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSeqTrack)
    {
        pszClassName = "CLSID_DirectMusicSeqTrack";
        pszFunctionName = "DirectMusicSeqTrackCreate(iid,ppvInterface)";
        pszComment = "Sequence Track: plays straight MIDI sequences";
        hr = DirectMusicSeqTrackCreate(iid,ppvInterface);
    }
#endif
#ifdef ADDWAVE
    else if (clsid == CLSID_DirectSoundWave)
    {
        pszClassName = "CLSID_DirectSoundWave";
        pszFunctionName = "DirectSoundWaveCreate(iid,ppvInterface)";
        pszComment = "Wave: represents the file version of a wave - required if wave files are used";
        hr = DirectSoundWaveCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicWaveTrack)
    {
        pszClassName = "CLSID_DirectMusicWaveTrack";
        pszFunctionName = "DirectMusicWaveTrackCreate(iid,ppvInterface)";
        pszComment = "Wave Track - plays one or more waves";
        hr = DirectMusicWaveTrackCreate(iid,ppvInterface);
    }
#endif
#ifdef ADDSTYLE
    else if (clsid == CLSID_DirectMusicChordTrack)
    {
        pszClassName = "CLSID_DirectMusicChordTrack";
        pszFunctionName = "DirectMusicChordTrackCreate(iid,ppvInterface)";
        pszComment = "Chord Track - used for style playback";
        hr = DirectMusicChordTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicCommandTrack)
    {
        pszClassName = "CLSID_DirectMusicCommandTrack";
        pszFunctionName = "DirectMusicCommandTrackCreate(iid,ppvInterface)";
        pszComment = "Groove Track - used to store groove levels for style playback";
        hr = DirectMusicCommandTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicStyleTrack)
    {
        pszClassName = "CLSID_DirectMusicStyleTrack";
        pszFunctionName = "DirectMusicStyleTrackCreate(iid,ppvInterface)";
        pszComment = "Style Track - selects style for playback";
        hr = DirectMusicStyleTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicPatternTrack)
    {
        pszClassName = "CLSID_DirectMusicPatternTrack";
        pszFunctionName = "DirectMusicPatternTrackCreate(iid,ppvInterface)";
        pszComment = "Pattern Track - plays patterns with variations tranposed to chord progression";
        hr = DirectMusicPatternTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicMotifTrack)
    {
        pszClassName = "CLSID_DirectMusicMotifTrack";
        pszFunctionName = "DirectMusicMotifTrackCreate(iid,ppvInterface)";
        pszComment = "Motif Track - mechanism for playing a style motif segment";
        hr = DirectMusicMotifTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicStyle)
    {
        pszClassName = "CLSID_DirectMusicStyle";
        pszFunctionName = "DirectMusicStyleCreate(iid,ppvInterface)";
        pszComment = "Style - style playback object, represents a specific style type";
        hr = DirectMusicStyleCreate(iid,ppvInterface);
    }
#endif
#ifdef ADDMELODY
    else if (clsid == CLSID_DirectMusicMelodyFormulationTrack)
    {
        pszClassName = "CLSID_DirectMusicMelodyFormulationTrack";
        pszFunctionName = "DirectMusicMelodyFormulationTrackCreate(iid,ppvInterface)";
        pszComment = "Melody Formulation Track - define the shape of a melody";
        hr = DirectMusicMelodyFormulationTrackCreate(iid,ppvInterface);
    }
#endif
#ifdef ADDCOMPOSER
    else if (clsid == CLSID_DirectMusicComposer)
    {
        pszClassName = "CLSID_DirectMusicComposer";
        pszFunctionName = "DirectMusicComposerCreate(iid,ppvInterface)";
        pszComment = "Composer: creates playback segments and autotransitions from chordmaps and template segments";
        hr = DirectMusicComposerCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicChordMap)
    {
        pszClassName = "CLSID_DirectMusicChordMap";
        pszFunctionName = "DirectMusicChordMapCreate(iid,ppvInterface)";
        pszComment = "ChordMap - defines chords to create chord progressions and transitions on the fly";
        hr = DirectMusicChordMapCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicChordMapTrack)
    {
        pszClassName = "CLSID_DirectMusicChordMapTrack";
        pszFunctionName = "DirectMusicChordMapTrackCreate(iid,ppvInterface)";
        pszComment = "ChordMap Track - defines ChordMap to be used to compose a chord track";
        hr = DirectMusicChordMapTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSignPostTrack)
    {
        pszClassName = "CLSID_DirectMusicSignPostTrack";
        pszFunctionName = "DirectMusicSignPostTrackCreate(iid,ppvInterface)";
        pszComment = "SignPost Track - defines phrasing to be used to compose a chord track";
        hr = DirectMusicSignPostTrackCreate(iid,ppvInterface);
    }
#endif
#ifdef ADDSCRIPT
    else if (clsid == CLSID_AutDirectMusicAudioPath)
    {
        pszClassName = "CLSID_AutDirectMusicAudioPath";
        pszFunctionName = "DirectMusicAutAudioPathCreate(pUnkOuter,iid,ppvInterface)";
        pszComment = "Audio Path Automation - used by scripting to control a style directly";
        hr = DirectMusicAutAudioPathCreate(pUnkOuter,iid,ppvInterface);
    }
    else if (clsid == CLSID_AutDirectMusicAudioPathConfig)
    {
        pszClassName = "CLSID_AutDirectMusicAudioPathConfig";
        pszFunctionName = "DirectMusicAutAudioPathConfigCreate(pUnkOuter,iid,ppvInterface)";
        pszComment = "Audio Path Configuration Automation - used by scripting to control an audiopath configuration";
        hr = DirectMusicAutAudioPathConfigCreate(pUnkOuter,iid,ppvInterface);
    }
    else if (clsid == CLSID_AutDirectMusicSong)
    {
        pszClassName = "CLSID_AutDirectMusicSong";
        pszFunctionName = "DirectMusicAutSongCreate(pUnkOuter,iid,ppvInterface)";
        pszComment = "Song Automation - used by scripting to control a song";
        hr = DirectMusicAutSongCreate(pUnkOuter,iid,ppvInterface);
    }
    else if (clsid == CLSID_AutDirectMusicPerformance)
    {
        pszClassName = "CLSID_AutDirectMusicPerformance";
        pszFunctionName = "DirectMusicAutPerformanceCreate(pUnkOuter,iid,ppvInterface)";
        pszComment = "Performance Automation - used by scripting to control a performance";
        hr = DirectMusicAutPerformanceCreate(pUnkOuter,iid,ppvInterface);
    }
    else if (clsid == CLSID_AutDirectMusicSegment)
    {
        pszClassName = "CLSID_AutDirectMusicSegment";
        pszFunctionName = "DirectMusicAutSegmentCreate(pUnkOuter,iid,ppvInterface)";
        pszComment = "Segment Automation: used by scripting to control a segment";
        hr = DirectMusicAutSegmentCreate(pUnkOuter,iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicScript)
    {
        pszClassName = "CLSID_DirectMusicScript";
        pszFunctionName = "DirectMusicScriptCreate(iid,ppvInterface)";
        pszComment = "Script: Manages a script and all embedded and linked content";
        hr = DirectMusicScriptCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicScriptTrack)
    {
        pszClassName = "CLSID_DirectMusicScriptTrack";
        pszFunctionName = "DirectMusicScriptTrackCreate(iid,ppvInterface)";
        pszComment = "Script Track: Sequences script routines to play in time with the music";
        hr = DirectMusicScriptTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicContainer)
    {
        pszClassName = "CLSID_DirectMusicContainer";
        pszFunctionName = "DirectMusicContainerCreate(iid,ppvInterface)";
        pszComment = "Container: storage container for multiple DirectMusic file objects. Always used in scripting";
        hr = DirectMusicContainerCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_AutDirectMusicSegmentState)
    {
        pszClassName = "CLSID_AutDirectMusicSegmentState";
        pszFunctionName = "CAutDirectMusicSegmentState(iid,ppvInterface)";
        pszComment = "A helper scripting object that implements IDispatch";
        hr = AutDirectMusicSegmentStateCreate(pUnkOuter,iid,ppvInterface);
    }
#endif
#ifdef ADDMISC
    else if (clsid == CLSID_DirectMusicMuteTrack)
    {
        pszClassName = "CLSID_DirectMusicMuteTrack";
        pszFunctionName = "DirectMusicMuteTrackCreate(iid,ppvInterface)";
        pszComment = "Mute Track - controls muting on sequence, style, and other event generating tracks";
        hr = DirectMusicMuteTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicMarkerTrack)
    {
        pszClassName = "CLSID_DirectMusicMarkerTrack";
        pszFunctionName = "DirectMusicMarkerTrackCreate(iid,ppvInterface)";
        pszComment = "Marker Track - indicates segment start and alignment points";
        hr = DirectMusicMarkerTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSong)
    {
        pszClassName = "CLSID_DirectMusicSong";
        pszFunctionName = "DirectMusicSongCreate(iid,ppvInterface)";
        pszComment = "Song: manages the composition and playback of a set of segments";
        hr = DirectMusicSongCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicAudioPathConfig)
    {
        pszClassName = "CLSID_DirectMusicAudioPathConfig";
        pszFunctionName = "DirectMusicAudioPathConfigCreate(iid,ppvInterface)";
        pszComment = "Audio Path Configuration: file representation of an audiopath";
        hr = DirectMusicAudioPathConfigCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicGraph)
    {
        pszClassName = "CLSID_DirectMusicGraph";
        pszFunctionName = "DirectMusicGraphCreate(iid,ppvInterface)";
        pszComment = "Tool Graph: manages a set of Tools (MIDI Processors)";
        hr = DirectMusicGraphCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSysExTrack)
    {
        pszClassName = "CLSID_DirectMusicSysExTrack";
        pszFunctionName = "DirectMusicSysexTrackCreate(iid,ppvInterface)";
        pszComment = "System Exclusive Track: sends system exclusive messages to the synthesizer";
        hr = DirectMusicSysexTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicSegmentTriggerTrack)
    {
        pszClassName = "CLSID_DirectMusicSegmentTriggerTrack";
        pszFunctionName = "DirectMusicTriggerTrackCreate(iid,ppvInterface)";
        pszComment = "Segment Trigger Track: triggers additional segments";
        hr = DirectMusicTriggerTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicLyricsTrack)
    {
        pszClassName = "CLSID_DirectMusicLyricsTrack";
        pszFunctionName = "DirectMusicLyricsTrackCreate(iid,ppvInterface)";
        pszComment = "Lyrics Track: sends lyrics";
        hr = DirectMusicLyricsTrackCreate(iid,ppvInterface);
    }
    else if (clsid == CLSID_DirectMusicParamControlTrack)
    {
        pszClassName = "CLSID_DirectMusicParamControlTrack";
        pszFunctionName = "DirectMusicParamsTrackCreate(iid,ppvInterface)";
        pszComment = "Parameter Control Track: provides realtime control of audio and MIDI effects";
        hr = DirectMusicParamsTrackCreate(iid,ppvInterface);
    }
#endif
//    else
//    {
//        DbgPrint("Creating Unknown object ");
//        DbgPrintGUID(clsid);
//        DbgPrint(" Failed\n");
//        return hr;
//    }
    if (SUCCEEDED(hr))
    {
        InsertClass(pszClassName, pszFunctionName, pszComment);
#ifdef SPEW
        DbgPrint("Creating %s object with interface ",pszClassName);
        DbgPrintGUID(iid);
        DbgPrint(" Succeeded\n");
#endif
    }
    else
    {
        DbgPrint("Creating \"%s\" object with interface ",pszClassName);
        DbgPrintGUID(iid);
        DbgPrint("And clsid: ");
        DbgPrintGUID(clsid);
        DbgPrint(" Failed, returned error: %lx\n",hr);
    }
    return hr;
}

char *GetClassName(REFGUID clsid)

{
    if (clsid == CLSID_DirectMusic)
    {
        return "DirectMusic";
    }
    else if (clsid == CLSID_DirectMusicCollection)
    {
        return "DLS Collection";
    }
    else if (clsid == CLSID_DirectMusicSynth)
    {
        return "Microsoft Synth";
    }
    else if (clsid == CLSID_DirectMusicFileStream)
    {
        return "File Stream";
    }
    else if (clsid == CLSID_DirectMusicStreamStream)
    {
        return "Stream Wrapper";
    }
    else if (clsid == CLSID_DirectMusicMemStream)
    {
        return "Memory Stream";
    }
    else if (clsid == CLSID_DirectMusicSegment)
    {
        return "Segment";
    }
    else if (clsid == CLSID_DirectMusicLoader)
    {
        return "Loader";
    }
    else if (clsid == CLSID_DirectMusicTempoTrack)
    {
        return "Tempo Track";
    }
    else if (clsid == CLSID_DirectMusicTimeSigTrack)
    {
        return "Time Signature Track";
    }
    else if (clsid == CLSID_DirectMusicBandTrack)
    {
        return "Band Track";
    }
    else if (clsid == CLSID_DirectMusicPerformance)
    {
        return "Performance";
    }
    else if (clsid == CLSID_DirectMusicBand)
    {
        return "Band";
    }
    else if (clsid == CLSID_DirectMusicSeqTrack)
    {
        return "Seqquence Track";
    }
    else if (clsid == CLSID_DirectSoundWave)
    {
        return "Wave";
    }
    else if (clsid == CLSID_DirectMusicWaveTrack)
    {
        return "Wave Track";
    }
    else if (clsid == CLSID_DirectMusicChordTrack)
    {
        return "Chord Track";
    }
    else if (clsid == CLSID_DirectMusicCommandTrack)
    {
        return "Command Track";
    }
    else if (clsid == CLSID_DirectMusicStyleTrack)
    {
        return "Style Track";
    }
    else if (clsid == CLSID_DirectMusicPatternTrack)
    {
        return "Pattern Track";
    }
    else if (clsid == CLSID_DirectMusicMotifTrack)
    {
        return "Motif Track";
    }
    else if (clsid == CLSID_DirectMusicStyle)
    {
        return "Style";
    }
    else if (clsid == CLSID_DirectMusicMelodyFormulationTrack)
    {
        return "Melody Formulation Track";
    }
    else if (clsid == CLSID_DirectMusicComposer)
    {
        return "Composer";
    }
    else if (clsid == CLSID_DirectMusicChordMap)
    {
        return "Chord Map";
    }
    else if (clsid == CLSID_DirectMusicChordMapTrack)
    {
        return "Chord Map Track";
    }
    else if (clsid == CLSID_DirectMusicSignPostTrack)
    {
        return "Sign Post Track";
    }
    else if (clsid == CLSID_AutDirectMusicAudioPath)
    {
        return "AudioPath IDispatch";
    }
    else if (clsid == CLSID_AutDirectMusicAudioPathConfig)
    {
        return "AudioPath Config IDispatch";
    }
    else if (clsid == CLSID_AutDirectMusicSong)
    {
        return "Song IDispatch";
    }
    else if (clsid == CLSID_AutDirectMusicPerformance)
    {
        return "Performance IDispatch";
    }
    else if (clsid == CLSID_AutDirectMusicSegment)
    {
        return "Segment IDispatch";
    }
    else if (clsid == CLSID_DirectMusicScript)
    {
        return "Script";
    }
    else if (clsid == CLSID_DirectMusicScriptTrack)
    {
        return "Script Track";
    }
    else if (clsid == CLSID_DirectMusicContainer)
    {
        return "Container";
    }
    else if (clsid == CLSID_AutDirectMusicSegmentState)
    {
        return "Segment State IDispatch";
    }
    else if (clsid == CLSID_DirectMusicMuteTrack)
    {
        return "Mute Track";
    }
    else if (clsid == CLSID_DirectMusicMarkerTrack)
    {
        return "Marker Track";
    }
    else if (clsid == CLSID_DirectMusicSong)
    {
        return "Song";
    }
    else if (clsid == CLSID_DirectMusicAudioPathConfig)
    {
        return "AudioPath Config";
    }
    else if (clsid == CLSID_DirectMusicGraph)
    {
        return "Tool Graph";
    }
    else if (clsid == CLSID_DirectMusicSysExTrack)
    {
        return "SysEx Track";
    }
    else if (clsid == CLSID_DirectMusicSegmentTriggerTrack)
    {
        return "Segment Trigger Track";
    }
    else if (clsid == CLSID_DirectMusicLyricsTrack)
    {
        return "Lyrics Track";
    }
    else if (clsid == CLSID_DirectMusicParamControlTrack)
    {
        return "Parameter Control Track";
    }
    return "Unknown object";
}


