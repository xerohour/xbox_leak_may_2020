#include "util.h"

LPSTR _stdcall tdmXlatGUID(REFGUID rguid);

void DbgPrintGUID ( REFIID riid)
{
	Output("(0x%08lX,0x%04X,0x%04X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X)",
		riid.Data1,
		riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2],
		riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6], riid.Data4[7]);
}


void _stdcall tdmGUIDtoString(REFGUID rguid, LPSTR szBuf)
{

    sprintf(szBuf, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            rguid.Data1, rguid.Data2, rguid.Data3,
            rguid.Data4[0], rguid.Data4[1], rguid.Data4[2], rguid.Data4[3],
            rguid.Data4[4], rguid.Data4[5], rguid.Data4[6], rguid.Data4[7]);

} //*** end dmthGUIDtoString()


void _stdcall LogDMUS_OBJECTDESCStruct(DMUS_OBJECTDESC Desc)
{
    char     szIID[1024];

    Output(		"DMUS_OBJECTDESC");
    Output(		"===============");

    

    // Size of this structure. 
    Output(		"dwSize                == %d",
            Desc.dwSize);
    /// Flags indicating which fields below are valid. 
    Output(		"dwValidData           == %08Xh",
            Desc.dwValidData);

    // Unique ID for this object. 
    if (Desc.dwValidData & DMUS_OBJ_OBJECT)
    {
         tdmGUIDtoString((REFGUID)Desc.guidObject, szIID);
        Output(	"guidObject            == %s (%s)",
                szIID, tdmXlatGUID((REFGUID)Desc.guidObject));
    }

    // GUID for the class of object. 
    if (Desc.dwValidData & DMUS_OBJ_CLASS)
    {
         tdmGUIDtoString((REFGUID)Desc.guidClass, szIID);
        Output(	"guidClass             == %s (%s)",
                szIID, tdmXlatGUID((REFGUID)Desc.guidClass));
    }

    // Last edited date of object. 
    Output(		"ftDate.LowDateTime    == %d",
            Desc.ftDate.dwLowDateTime);
    Output(		"ftDate.HighDateTime   == %d",
            Desc.ftDate.dwHighDateTime);

    //BUGBUG  MS DWORD needs to be shifted.  -jimmo
    // Version. 
    if (Desc.dwValidData & DMUS_OBJ_VERSION)
    {
        Output(	"Version               == %d.%d",
                Desc.vVersion.dwVersionMS, Desc.vVersion.dwVersionLS);
    }

    // Name of object. 
    if (Desc.dwValidData & DMUS_OBJ_NAME){
        Output(	"Name                  == %ls",
                Desc.wszName);
    }

    // Category for object (optional). 
    if (Desc.dwValidData & DMUS_OBJ_CATEGORY){
        Output(	"Category              == %ls",
                Desc.wszCategory);
    }

    // File path. 
    if (Desc.dwValidData & DMUS_OBJ_FULLPATH){
        Output(	"FileName (fullpath)   == %ls",
                Desc.wszFileName);
    }
    else if (Desc.dwValidData & DMUS_OBJ_FILENAME){
        Output(	"FileName              == %ls",
                Desc.wszFileName);
    }


} ///** end dmthLogDMUS_OBJECTDESCStruct()

//


// GUID / IID / CLSID list (used by dmthXlatGUID)
typedef struct
{
    GUID *pguid;
    LPSTR   sz;
} GUIDLIST;

static GUIDLIST glGuids[]=
{
    //** class IDs
    // dmusics.h
//    { (GUID*)&CLSID_DirectMusicSynthSink,             "CLSID_DirectMusicSynthSink" },

    // dmusicc.h
    { (GUID*)&CLSID_DirectMusic,                      "CLSID_DirectMusic" },
//    { (GUID*)&CLSID_DirectMusicCollection,            "CLSID_DirectMusicCollection" },
//    { (GUID*)&CLSID_DirectMusicSynth,                 "CLSID_DirectMusicSynth" },
    // dmusici.h
/*
    { (GUID*)&CLSID_DirectMusicPerformance,           "CLSID_DirectMusicPerformance" },
    { (GUID*)&CLSID_DirectMusicSegment,               "CLSID_DirectMusicSegment" },
    { (GUID*)&CLSID_DirectMusicSegmentState,          "CLSID_DirectMusicSegmentState" },
    { (GUID*)&CLSID_DirectMusicGraph,                 "CLSID_DirectMusicGraph" },
    { (GUID*)&CLSID_DirectMusicTempoTrack,            "CLSID_DirectMusicTempoTrack" },
    { (GUID*)&CLSID_DirectMusicSeqTrack,              "CLSID_DirectMusicSeqTrack" },
    { (GUID*)&CLSID_DirectMusicSysExTrack,            "CLSID_DirectMusicSysExTrack" },
    { (GUID*)&CLSID_DirectMusicTimeSigTrack,          "CLSID_DirectMusicTimeSigTrack" },
    { (GUID*)&CLSID_DirectMusicStyle,                 "CLSID_DirectMusicStyle" },
    { (GUID*)&CLSID_DirectMusicChordTrack,            "CLSID_DirectMusicChordTrack" },
   { (GUID*)&CLSID_DirectMusicCommandTrack,          "CLSID_DirectMusicCommandTrack" },
    { (GUID*)&CLSID_DirectMusicStyleTrack,            "CLSID_DirectMusicStyleTrack" },
    { (GUID*)&CLSID_DirectMusicMotifTrack,            "CLSID_DirectMusicMotifTrack" },
    { (GUID*)&CLSID_DirectMusicChordMap,              "CLSID_DirectMusicChordMap" },
    { (GUID*)&CLSID_DirectMusicComposer,              "CLSID_DirectMusicComposer" },
    { (GUID*)&CLSID_DirectMusicSignPostTrack,         "CLSID_DirectMusicSignPostTrack" },
    { (GUID*)&CLSID_DirectMusicLoader,                "CLSID_DirectMusicLoader" },
    { (GUID*)&CLSID_DirectMusicBandTrack,             "CLSID_DirectMusicBandTrack" },
    { (GUID*)&CLSID_DirectMusicBand,                  "CLSID_DirectMusicBand" },
    { (GUID*)&CLSID_DirectMusicChordMapTrack,         "CLSID_DirectMusicChordMapTrack" },
    { (GUID*)&CLSID_DirectMusicMuteTrack,             "CLSID_DirectMusicMuteTrack" },

    { (GUID*)&CLSID_DirectMusicPatternTrack,          "CLSID_DirectMusicPatternTrack" },
    { (GUID*)&CLSID_DirectMusicScript,                "CLSID_DirectMusicScript" },
    { (GUID*)&CLSID_DirectMusicScriptTrack,           "CLSID_DirectMusicScriptTrack" },
    { (GUID*)&CLSID_DirectMusicContainer,             "CLSID_DirectMusicContainer" },
    { (GUID*)&CLSID_DirectMusicMarkerTrack,           "CLSID_DirectMusicMarkerTrack" },
    { (GUID*)&CLSID_DirectSoundWave,                  "CLSID_DirectSoundWave" },
    { (GUID*)&CLSID_DirectMusicSegmentTriggerTrack,   "CLSID_DirectMusicSegmentTriggerTrack" },
    { (GUID*)&CLSID_DirectMusicLyricsTrack,           "CLSID_DirectMusicLyricsTrack" },
    { (GUID*)&CLSID_DirectMusicParamControlTrack,     "CLSID_DirectMusicParamControlTrack" },
    { (GUID*)&CLSID_DirectMusicSong,                  "CLSID_DirectMusicSong" },
    { (GUID*)&CLSID_DirectMusicMelodyFormulationTrack,"CLSID_DirectMusicMelodyFormulationTrack" },
    { (GUID*)&CLSID_DirectMusicWaveTrack,             "CLSID_DirectMusicWaveTrack" },
    { (GUID*)&CLSID_DirectMusicAudioPathConfig,       "CLSID_DirectMusicAudioPathConfig" },

    //** GUIDs
    // dmusics.h
    { (GUID*)&GUID_DMUS_PROP_SetSynthSink,            "GUID_DMUS_PROP_SetSynthSink" },
    { (GUID*)&GUID_DMUS_PROP_SinkUsesDSound,          "GUID_DMUS_PROP_SinkUsesDSound" },
*/

    // dmusicc.h
    { (GUID*)&GUID_DMUS_PROP_GM_Hardware,             "GUID_DMUS_PROP_GM_Hardware" },
    { (GUID*)&GUID_DMUS_PROP_GS_Hardware,             "GUID_DMUS_PROP_GS_Hardware" },
    { (GUID*)&GUID_DMUS_PROP_XG_Hardware,             "GUID_DMUS_PROP_XG_Hardware" },
    { (GUID*)&GUID_DMUS_PROP_XG_Capable,              "GUID_DMUS_PROP_XG_Capable" },
    { (GUID*)&GUID_DMUS_PROP_GS_Capable,              "GUID_DMUS_PROP_GS_Capable" },
    { (GUID*)&GUID_DMUS_PROP_DLS1,                    "GUID_DMUS_PROP_DLS1" },
    { (GUID*)&GUID_DMUS_PROP_DLS2,                    "GUID_DMUS_PROP_DLS2" },
    { (GUID*)&GUID_DMUS_PROP_INSTRUMENT2,             "GUID_DMUS_PROP_INSTRUMENT2" },
    { (GUID*)&GUID_DMUS_PROP_SynthSink_DSOUND,        "GUID_DMUS_PROP_SynthSink_DSOUND" },
    { (GUID*)&GUID_DMUS_PROP_SynthSink_WAVE,          "GUID_DMUS_PROP_SynthSink_WAVE" },
    { (GUID*)&GUID_DMUS_PROP_SampleMemorySize,        "GUID_DMUS_PROP_SampleMemorySize" },
    { (GUID*)&GUID_DMUS_PROP_SamplePlaybackRate,      "GUID_DMUS_PROP_SamplePlaybackRate" },

    { (GUID*)&GUID_DMUS_PROP_WriteLatency,            "GUID_DMUS_PROP_WriteLatency" },
    { (GUID*)&GUID_DMUS_PROP_WritePeriod,             "GUID_DMUS_PROP_WritePeriod" },
    { (GUID*)&GUID_DMUS_PROP_MemorySize,              "GUID_DMUS_PROP_MemorySize" },
    { (GUID*)&GUID_DMUS_PROP_WavesReverb,             "GUID_DMUS_PROP_WavesReverb" },
    { (GUID*)&GUID_DMUS_PROP_Effects,                 "GUID_DMUS_PROP_Effects" },
    { (GUID*)&GUID_DMUS_PROP_LegacyCaps,              "GUID_DMUS_PROP_LegacyCaps" },
    { (GUID*)&GUID_DMUS_PROP_Volume,                  "GUID_DMUS_PROP_Volume" },


    // dmusici.h
    { (GUID*)&GUID_DirectMusicAllTypes,               "GUID_DirectMusicAllTypes" },
    { (GUID*)&GUID_NOTIFICATION_SEGMENT,              "GUID_NOTIFICATION_SEGMENT" },
    { (GUID*)&GUID_NOTIFICATION_MEASUREANDBEAT,       "GUID_NOTIFICATION_MEASUREANDBEAT" },
    { (GUID*)&GUID_NOTIFICATION_CHORD,                "GUID_NOTIFICATION_CHORD" },
    { (GUID*)&GUID_NOTIFICATION_COMMAND,              "GUID_NOTIFICATION_COMMAND" },
    { (GUID*)&GUID_NOTIFICATION_RECOMPOSE,            "GUID_NOTIFICATION_RECOMPOSE" },
    { (GUID*)&GUID_CommandParam,                      "GUID_CommandParam" },
    { (GUID*)&GUID_CommandParam2,                     "GUID_CommandParam2" },
    { (GUID*)&GUID_CommandParamNext,                  "GUID_CommandParamNext" },
    { (GUID*)&GUID_ChordParam,                        "GUID_ChordParam" },
    { (GUID*)&GUID_RhythmParam,                       "GUID_RhythmParam" },
    { (GUID*)&GUID_IDirectMusicStyle,                 "GUID_IDirectMusicStyle" },
    { (GUID*)&GUID_TimeSignature,                     "GUID_TimeSignature" },
//    { (GUID*)&GUID_CueTimeSignature,                  "GUID_CueTimeSignature" }, // removed from dx8
    { (GUID*)&GUID_TempoParam,                        "GUID_TempoParam" },
    { (GUID*)&GUID_Valid_Start_Time,                  "GUID_Valid_Start_Time" },
    { (GUID*)&GUID_Play_Marker,                       "GUID_Play_Marker" },
    { (GUID*)&GUID_BandParam,                         "GUID_BandParam" },
    { (GUID*)&GUID_IDirectMusicBand,                  "GUID_IDirectMusicBand" },
//    { (GUID*)&GUID_IDirectMusicChordMap,              "GUID_IDirectMusicChordMap" },
    { (GUID*)&GUID_MuteParam,                         "GUID_MuteParam" },
    { (GUID*)&GUID_Download,                          "GUID_Download" },
    { (GUID*)&GUID_Unload,                            "GUID_Unload" },
//    { (GUID*)&GUID_ConnectToDLSCollection,            "GUID_ConnectToDLSCollection" },
//    { (GUID*)&GUID_Enable_Auto_Download,              "GUID_Enable_Auto_Download" },
//    { (GUID*)&GUID_Disable_Auto_Download,             "GUID_Disable_Auto_Download" },
//    { (GUID*)&GUID_Clear_All_Bands,                   "GUID_Clear_All_Bands" },
//    { (GUID*)&GUID_StandardMIDIFile,                  "GUID_StandardMIDIFile" },

//    { (GUID*)&GUID_DisableTimeSig,                    "GUID_DisableTimeSig" },
//    { (GUID*)&GUID_EnableTimeSig,                     "GUID_EnableTimeSig" },
//    { (GUID*)&GUID_DisableTempo,                      "GUID_DisableTempo" },
//    { (GUID*)&GUID_EnableTempo,                       "GUID_EnableTempo" },
//    { (GUID*)&GUID_SeedVariations,                    "GUID_SeedVariations" },
//    { (GUID*)&GUID_MelodyFragment,                    "GUID_MelodyFragment" },
//    { (GUID*)&GUID_MelodyPlaymode,                    "GUID_MelodyPlaymode" },
//    { (GUID*)&GUID_Clear_All_MelodyFragments,         "GUID_Clear_All_MelodyFragments" },
//    { (GUID*)&GUID_Variations,                        "GUID_Variations" },
    { (GUID*)&GUID_DownloadToAudioPath,               "GUID_DownloadToAudioPath" },
    { (GUID*)&GUID_UnloadFromAudioPath,               "GUID_UnloadFromAudioPath" },
    { (GUID*)&GUID_PerfMasterTempo,                   "GUID_PerfMasterTempo" },
    { (GUID*)&GUID_PerfMasterVolume,                  "GUID_PerfMasterVolume" },
    { (GUID*)&GUID_PerfMasterGrooveLevel,             "GUID_PerfMasterGrooveLevel" },
//    { (GUID*)&GUID_PerfAutoDownload,                  "GUID_PerfAutoDownload" },
    { (GUID*)&GUID_DefaultGMCollection,               "GUID_DefaultGMCollection" },
    // dsound.h
//    { (GUID*)&GUID_DSFX_SEND,                         "GUID_DSFX_SEND" },
//    { (GUID*)&GUID_DSFX_STANDARD_CHORUS,              "GUID_DSFX_STANDARD_CHORUS" },
//    { (GUID*)&GUID_DSFX_STANDARD_FLANGER,             "GUID_DSFX_STANDARD_FLANGER" },
//    { (GUID*)&GUID_DSFX_STANDARD_GARGLE,              "GUID_DSFX_STANDARD_GARGLE" },
//    { (GUID*)&GUID_DSFX_STANDARD_ECHO,                "GUID_DSFX_STANDARD_ECHO" },
//    { (GUID*)&GUID_DSFX_STANDARD_DISTORTION,          "GUID_DSFX_STANDARD_DISTORTION" },
//    { (GUID*)&GUID_DSFX_STANDARD_COMPRESSOR,          "GUID_DSFX_STANDARD_COMPRESSOR" },
//    { (GUID*)&GUID_DSFX_STANDARD_I3DL2REVERB,         "GUID_DSFX_STANDARD_I3DL2REVERB" },
//    { (GUID*)&GUID_DSFX_STANDARD_PARAMEQ,             "GUID_DSFX_STANDARD_PARAMEQ"},
//    { (GUID*)&GUID_DSFX_WAVES_REVERB,                 "GUID_DSFX_WAVES_REVERB"},

    

    //** interface IDs
    // dmusics.h
/*
    { (GUID*)&IID_IDirectMusicSynth,                  "IID_IDirectMusicSynth" },
    { (GUID*)&IID_IDirectMusicSynth8,                 "IID_IDirectMusicSynth8" },
    { (GUID*)&IID_IDirectMusicSynthSink,              "IID_IDirectMusicSynthSink" },

    // dmusicc.h
    { (GUID*)&IID_IReferenceClock,                    "IID_IReferenceClock" },
    { (GUID*)&IID_IDirectMusic,                       "IID_IDirectMusic" },
    { (GUID*)&IID_IDirectMusicBuffer,                 "IID_IDirectMusicBuffer" },
    { (GUID*)&IID_IDirectMusicPort,                   "IID_IDirectMusicPort" },
    { (GUID*)&IID_IDirectMusicPortDownload,           "IID_IDirectMusicPortDownload" },
    { (GUID*)&IID_IDirectMusicDownload,               "IID_IDirectMusicDownload" },
    { (GUID*)&IID_IDirectMusicCollection,             "IID_IDirectMusicCollection" },
    { (GUID*)&IID_IDirectMusicInstrument,             "IID_IDirectMusicInstrument" },
    { (GUID*)&IID_IDirectMusicDownloadedInstrument,   "IID_IDirectMusicDownloadedInstrument" },
    { (GUID*)&IID_IDirectMusic2,                      "IID_IDirectMusic2" },
//    { (GUID*)&IID_IDirectSoundDownloadedWave,         "IID_IDirectSoundDownloadedWave" },
//    { (GUID*)&IID_IDirectMusicVoice,                  "IID_IDirectMusicVoice" },
    { (GUID*)&IID_IDirectMusicPort8,                  "IID_IDirectMusicPort8" },
    { (GUID*)&IID_IDirectMusic8,                      "IID_IDirectMusic8" },
*/

    // dmusici.h
    { (GUID*)&IID_IDirectMusicLoader,                 "IID_IDirectMusicLoader" },
//    { (GUID*)&IID_IDirectMusicGetLoader,              "IID_IDirectMusicLoader" },
//    { (GUID*)&IID_IDirectMusicObject,                 "IID_IDirectMusicObject" },
    { (GUID*)&IID_IDirectMusicSegment,                "IID_IDirectMusicSegment" },
    { (GUID*)&IID_IDirectMusicSegmentState,           "IID_IDirectMusicSegmentState" },
//    { (GUID*)&IID_IDirectMusicTrack,                  "IID_IDirectMusicTrack" },
    { (GUID*)&IID_IDirectMusicPerformance,            "IID_IDirectMusicPerformance" },
//    { (GUID*)&IID_IDirectMusicTool,                   "IID_IDirectMusicTool" },
//    { (GUID*)&IID_IDirectMusicGraph,                  "IID_IDirectMusicGraph" },
//    { (GUID*)&IID_IDirectMusicStyle,                  "IID_IDirectMusicStyle" },
//    { (GUID*)&IID_IDirectMusicChordMap,               "IID_IDirectMusicChordMap" },
//    { (GUID*)&IID_IDirectMusicComposer,               "IID_IDirectMusicComposer" },
//    { (GUID*)&IID_IDirectMusicBand,                   "IID_IDirectMusicBand" },

//    { (GUID*)&IID_IDirectMusicPerformance2,           "IID_IDirectMusicPerformance2" },
//    { (GUID*)&IID_IDirectMusicSegment2,               "IID_IDirectMusicSegment2" },

    { (GUID*)&IID_IDirectMusicLoader8,                "IID_IDirectMusicLoader8" },
//    { (GUID*)&IID_IDirectMusicObject8,                "IID_IDirectMusicObject8" },
    { (GUID*)&IID_IDirectMusicPerformance8,           "IID_IDirectMusicPerformance8" },
    { (GUID*)&IID_IDirectMusicSegment8,               "IID_IDirectMusicSegment8" },
    { (GUID*)&IID_IDirectMusicSegmentState8,          "IID_IDirectMusicSegmentState8" },
//    { (GUID*)&IID_IDirectMusicTool8,                  "IID_IDirectMusicTool8" },
//    { (GUID*)&IID_IDirectMusicGraph8,                 "IID_IDirectMusicGraph8" },
//    { (GUID*)&IID_IDirectMusicTrack8,                 "IID_IDirectMusicTrack8" },
//    { (GUID*)&IID_IDirectMusicStyle8,                 "IID_IDirectMusicStyle8" },
//    { (GUID*)&IID_IDirectMusicComposer8,              "IID_IDirectMusicComposer8" },
//    { (GUID*)&IID_IDirectMusicBand8,                  "IID_IDirectMusicBand8" },
//    { (GUID*)&IID_IDirectMusicParamHook,              "IID_IDirectMusicParamHook" },
//    { (GUID*)&IID_IDirectMusicPatternTrack,           "IID_IDirectMusicPatternTrack" },
    { (GUID*)&IID_IDirectMusicScript,                 "IID_IDirectMusicScript" },
    //{ (GUID*)&IID_IDirectMusicContainer,              "IID_IDirectMusicContainer" },
    //{ (GUID*)&IID_IDirectMusicSong,                   "IID_IDirectMusicSong" },
    { (GUID*)&IID_IDirectMusicAudioPath,              "IID_IDirectMusicAudioPath" },

    // dsound.h
//    { (GUID*)&IID_IDirectSoundSink,                   "IID_IDirectSoundSink"   },
//    { (GUID*)&IID_IDirectSoundSink8,                  "IID_IDirectSoundSink8" },
//    { (GUID*)&IID_IDirectSoundBuffer,                 "IID_IDirectSoundBuffer"   },
//    { (GUID*)&IID_IDirectSoundBuffer8,                "IID_IDirectSoundBuffer8" },
//    { (GUID*)&IID_IDirectSound3DListener,             "IID_IDirectSound3DListener" },
//    { (GUID*)&IID_IDirectSoundNotify,                 "IID_IDirectSoundNotify" },
//    { (GUID*)&IID_IKsPropertySet,                     "IID_IKsPropertySet" },
//    { (GUID*)&IID_IDirectSound3DBuffer,               "IID_IDirectSound3DBuffer" },
//    { (GUID*)&IID_IDirectSoundFXSend,                 "IID_IDirectSoundFXSend"},
//    { (GUID*)&IID_IDirectSoundFXChorus,               "IID_IDirectSoundFXChorus"},
//    { (GUID*)&IID_IDirectSoundFXCompressor,           "IID_IDirectSoundFXCompressor"},
//    { (GUID*)&IID_IDirectSoundFXDistortion,           "IID_IDirectSoundFXDistortion"},
//    { (GUID*)&IID_IDirectSoundFXEcho,                 "IID_IDirectSoundFXEcho"},
//    { (GUID*)&IID_IDirectSoundFXFlanger,              "IID_IDirectSoundFXFlanger"},
//    { (GUID*)&IID_IDirectSoundFXGargle,               "IID_IDirectSoundFXGargle"},
//    { (GUID*)&IID_IDirectSoundFXI3DL2Reverb,          "IID_IDirectSoundFXI3DL2Reverb"},
//    { (GUID*)&IID_IDirectSoundFXParamEq,              "IID_IDirectSoundFXParamEq"},
//    { (GUID*)&IID_IDirectSoundFXWavesReverb,          "IID_IDirectSoundFXWavesReverb"},
//    { (GUID*)&GUID_All_Objects,                       "GUID_All_Objects"},

// misc GUIDs (>not< part of DirectMusic)
    { (GUID*)&IID_IUnknown,                           "IID_IUnknown"},
    { (GUID*)&GUID_NULL,                              "GUID_NULL" },


};

LPSTR _stdcall tdmXlatGUID(REFGUID rguid)
{
    int i;

    for( i=0;i<sizeof( glGuids )/sizeof( glGuids[0] );i++ )
    {
        if( IsEqualGUID(rguid, *(glGuids[i].pguid)))
        {
            return glGuids[i].sz;
        }
    }

    return "Unknown GUID";

} //*** end dmthXlatGUID()


#define TEXTW(quote) L##quote      

LPWSTR PathFindFileNameW(LPWSTR pPath)
{
    LPWSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == TEXTW('\\') || pPath[0] == TEXTW(':') || pPath[0] == TEXTW('/'))
            && pPath[1] &&  pPath[1] != TEXTW('\\')  &&   pPath[1] != TEXTW('/'))
            pT = pPath + 1;
    }

    return pT;
}

LPSTR PathFindFileNameA(LPSTR pPath)
{
    LPSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == '\\' || pPath[0] == ':' || pPath[0] == '/')
            && pPath[1] &&  pPath[1] != '\\'  &&   pPath[1] != '/')
            pT = pPath + 1;
    }

    return pT;
}
