//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       dmthhelp.cpp
//
//--------------------------------------------------------------------------

//===========================================================================
// dmthhelp.cpp
//
// Helper functions provided by DMTH
//
// Functions:
//  dmthXlatHRESULT()
//  dmthXlatGUID()
//  dmthGUIDtoString()
//
// History:
//  04/03/1998 - davidkl - created
//  10/31/1998 - jimmo   - see //JIMMO section below.
//  05/13/1999 - jimmo   - Added CLSID_DirectMusicAuditionTrack to the glGuids[] array.
//  05/25/1999 - kcraven   - Removed CLSID_DirectMusicAuditionTrack to the glGuids[] array.
//  01/25/2000 - kcraven   - start moving dmthCreate helper functions to a common file
//  02/10/2000 - danhaff   - added dsound.h GUIDs and IIDs.
//  03/28/2000 - danhaff   - added dsound.h HRESULTS
//  02/07/2000 - danhaff   - Ported to XBox, removed unused stuff.
//
//===========================================================================

#include "globals.h"

//---------------------------------------------------------------------------

// HRESULT error list (used by dmthXlatHRESULT)
typedef struct
{
    HRESULT rval;
    LPSTR   sz;
} ERRLIST;

static ERRLIST elErrors[] =
{
    { S_OK,                                   "S_OK" },
    { S_FALSE,                                "S_FALSE" },
    { E_FAIL,                                 "E_FAIL" },
    { E_ACCESSDENIED,                         "E_ACCESSDENIED" },
    { E_HANDLE,                               "E_HANDLE" },
    { E_POINTER,                              "E_POINTER" },
    { E_NOTIMPL,                              "E_NOTIMPL" },
    { E_INVALIDARG,                           "E_INVALIDARG" },
    { E_NOINTERFACE,                          "E_NOINTERFACE" },
    { OLE_E_BLANK,                            "OLE_E_BLANK" },
    { CLASS_E_CLASSNOTAVAILABLE,              "CLASS_E_CLASSNOTAVAILABLE" },
    { REGDB_E_CLASSNOTREG,                    "REGDB_E_CLASSNOTREG" },

//    { DMUS_S_PARTIALLOAD,                     "DMUS_S_PARTIALLOAD" },
//    { DMUS_S_PARTIALDOWNLOAD,                 "DMUS_S_PARTIALDOWNLOAD" },

    { DMUS_S_REQUEUE,                         "DMUS_S_REQUEUE" },
    { DMUS_S_FREE,                            "DMUS_S_FREE" },
    { DMUS_S_END,                             "DMUS_S_END" },

//    { DMUS_S_STRING_TRUNCATED,                "DMUS_S_STRING_TRUNCATED" },
    { DMUS_S_LAST_TOOL,                       "DMUS_S_LAST_TOOL" },
    { DMUS_S_OVER_CHORD,                      "DMUS_S_OVER_CHORD" },
    { DMUS_S_UP_OCTAVE,                       "DMUS_S_UP_OCTAVE" },
    { DMUS_S_DOWN_OCTAVE,                     "DMUS_S_DOWN_OCTAVE" },
//    { DMUS_S_NOBUFFERCONTROL,                 "DMUS_S_NOBUFFERCONTROL" },
    { DMUS_S_GARBAGE_COLLECTED,               "DMUS_S_GARBAGE_COLLECTED" },

//    { DMUS_E_DRIVER_FAILED,                   "DMUS_E_DRIVER_FAILED" },
//    { DMUS_E_PORTS_OPEN,                      "DMUS_E_PORTS_OPEN" },
//    { DMUS_E_DEVICE_IN_USE,                   "DMUS_E_DEVICE_IN_USE" },
//    { DMUS_E_INSUFFICIENTBUFFER,              "DMUS_E_INSUFFICIENTBUFFER" },
//    { DMUS_E_BUFFERNOTSET,                    "DMUS_E_BUFFERNOTSET" },
//    { DMUS_E_BUFFERNOTAVAILABLE,              "DMUS_E_BUFFERNOTAVAILABLE" },

//    { DMUS_E_NOTADLSCOL,                      "DMUS_E_NOTADLSCOL" },
//    { DMUS_E_INVALIDOFFSET,                   "DMUS_E_INVALIDOFFSET" },

//    { DMUS_E_ALREADY_LOADED,                  "DMUS_E_ALREADY_LOADED" },

//    { DMUS_E_INVALIDPOS,                      "DMUS_E_INVALIDPOS" },
//    { DMUS_E_INVALIDPATCH,                    "DMUS_E_INVALIDPATCH" },
//    { DMUS_E_CANNOTSEEK,                      "DMUS_E_CANNOTSEEK" },
//    { DMUS_E_CANNOTWRITE,                     "DMUS_E_CANNOTWRITE" },
//    { DMUS_E_CHUNKNOTFOUND,                   "DMUS_E_CHUNKNOTFOUND" },
//    { DMUS_E_INVALID_DOWNLOADID,              "DMUS_E_INVALID_DOWNLOADID" },
//    { DMUS_E_NOT_DOWNLOADED_TO_PORT,          "DMUS_E_NOT_DOWNLOADED_TO_PORT" },
//    { DMUS_E_ALREADY_DOWNLOADED,              "DMUS_E_ALREADY_DOWNLOADED" },
//    { DMUS_E_UNKNOWN_PROPERTY,                "DMUS_E_UNKNOWN_PROPERTY" },
    { DMUS_E_SET_UNSUPPORTED,                 "DMUS_E_SET_UNSUPPORTED" },
    { DMUS_E_GET_UNSUPPORTED,                 "DMUS_E_GET_UNSUPPORTED" },
//    { DMUS_E_NOTMONO,                         "DMUS_E_NOTMONO" },
//    { DMUS_E_BADARTICULATION,                 "DMUS_E_BADARTICULATION" },
//    { DMUS_E_BADINSTRUMENT,                   "DMUS_E_BADINSTRUMENT" },
//    { DMUS_E_BADWAVELINK,                     "DMUS_E_BADWAVELINK" },
//    { DMUS_E_NOARTICULATION,                  "DMUS_E_NOARTICULATION" },
//    { DMUS_E_NOTPCM,                          "DMUS_E_NOTPCM" },
//    { DMUS_E_BADWAVE,                         "DMUS_E_BADWAVE" },
//    { DMUS_E_BADOFFSETTABLE,                  "DMUS_E_BADOFFSETTABLE" },
//    { DMUS_E_UNKNOWNDOWNLOAD,                 "DMUS_E_UNKNOWNDOWNLOAD" },
//    { DMUS_E_NOSYNTHSINK,                     "DMUS_E_NOSYNTHSINK" },
//    { DMUS_E_ALREADYOPEN,                     "DMUS_E_ALREADYOPEN" },
//    { DMUS_E_ALREADYCLOSED,                   "DMUS_E_ALREADYCLOSED" },
//    { DMUS_E_SYNTHNOTCONFIGURED,              "DMUS_E_SYNTHNOTCONFIGURED" },
//    { DMUS_E_SYNTHACTIVE,                     "DMUS_E_SYNTHACTIVE" },
//    { DMUS_E_CANNOTREAD,                      "DMUS_E_CANNOTREAD" },
//    { DMUS_E_DMUSIC_RELEASED,                 "DMUS_E_DMUSIC_RELEASED" },
//    { DMUS_E_BUFFER_EMPTY,                    "DMUS_E_BUFFER_EMPTY" },
//    { DMUS_E_BUFFER_FULL,                     "DMUS_E_BUFFER_FULL" },
//    { DMUS_E_PORT_NOT_CAPTURE,                "DMUS_E_PORT_NOT_CAPTURE" },
//    { DMUS_E_PORT_NOT_RENDER,                 "DMUS_E_PORT_NOT_RENDER" },
//    { DMUS_E_DSOUND_NOT_SET,                  "DMUS_E_DSOUND_NOT_SET" },
//    { DMUS_E_ALREADY_ACTIVATED,               "DMUS_E_ALREADY_ACTIVATED" },
//    { DMUS_E_INVALIDBUFFER,                   "DMUS_E_INVALIDBUFFER" },
//    { DMUS_E_WAVEFORMATNOTSUPPORTED,          "DMUS_E_WAVEFORMATNOTSUPPORTED" },
//    { DMUS_E_SYNTHINACTIVE,                   "DMUS_E_SYNTHINACTIVE" },
//    { DMUS_E_DSOUND_ALREADY_SET,              "DMUS_E_DSOUND_ALREADY_SET" },
//    { DMUS_E_INVALID_EVENT,                   "DMUS_E_INVALID_EVENT" },

//    { DMUS_E_UNSUPPORTED_STREAM,              "DMUS_E_UNSUPPORTED_STREAM" },
//    { DMUS_E_ALREADY_INITED,                  "DMUS_E_ALREADY_INITED" },
//    { DMUS_E_INVALID_BAND,                    "DMUS_E_INVALID_BAND" },

//    { DMUS_E_TRACK_HDR_NOT_FIRST_CK,          "DMUS_E_TRACK_HDR_NOT_FIRST_CK" },
//    { DMUS_E_TOOL_HDR_NOT_FIRST_CK,           "DMUS_E_TOOL_HDR_NOT_FIRST_CK" },
//    { DMUS_E_INVALID_TRACK_HDR,               "DMUS_E_INVALID_TRACK_HDR" },
//    { DMUS_E_INVALID_TOOL_HDR,                "DMUS_E_INVALID_TOOL_HDR" },
//    { DMUS_E_ALL_TOOLS_FAILED,                "DMUS_E_ALL_TOOLS_FAILED" },
//    { DMUS_E_ALL_TRACKS_FAILED,               "DMUS_E_ALL_TRACKS_FAILED" },
    { DMUS_E_NOT_FOUND,                       "DMUS_E_NOT_FOUND" },
    { DMUS_E_NOT_INIT,                        "DMUS_E_NOT_INIT" },
//    { DMUS_E_TYPE_DISABLED,                   "DMUS_E_TYPE_DISABLED" },
//    { DMUS_E_TYPE_UNSUPPORTED,                "DMUS_E_TYPE_UNSUPPORTED" },
    { DMUS_E_TIME_PAST,                       "DMUS_E_TIME_PAST" },
//    { DMUS_E_TRACK_NOT_FOUND,                 "DMUS_E_TRACK_NOT_FOUND" },
//    { DMUS_E_TRACK_NO_CLOCKTIME_SUPPORT,      "DMUS_E_TRACK_NO_CLOCKTIME_SUPPORT" },

//    { DMUS_E_NO_MASTER_CLOCK,                 "DMUS_E_NO_MASTER_CLOCK" },

//    { DMUS_E_LOADER_NOCLASSID,                "DMUS_E_LOADER_NOCLASSID" },
//    { DMUS_E_LOADER_BADPATH,                  "DMUS_E_LOADER_BADPATH" },
//    { DMUS_E_LOADER_FAILEDOPEN,               "DMUS_E_LOADER_FAILEDOPEN" },
//    { DMUS_E_LOADER_FORMATNOTSUPPORTED,       "DMUS_E_LOADER_FORMATNOTSUPPORTED" },
    { DMUS_E_LOADER_FAILEDCREATE,             "DMUS_E_LOADER_FAILEDCREATE" },
    { DMUS_E_LOADER_OBJECTNOTFOUND,           "DMUS_E_LOADER_OBJECTNOTFOUND" },
//    { DMUS_E_LOADER_NOFILENAME,               "DMUS_E_LOADER_NOFILENAME" },

    { DMUS_E_INVALIDFILE,                     "DMUS_E_INVALIDFILE" },
//    { DMUS_E_ALREADY_EXISTS,                  "DMUS_E_ALREADY_EXISTS" },
//    { DMUS_E_OUT_OF_RANGE,                    "DMUS_E_OUT_OF_RANGE" },
//    { DMUS_E_SEGMENT_INIT_FAILED,             "DMUS_E_SEGMENT_INIT_FAILED" },
//    { DMUS_E_ALREADY_SENT,                    "DMUS_E_ALREADY_SENT" },
//    { DMUS_E_CANNOT_FREE,                     "DMUS_E_CANNOT_FREE" },
//    { DMUS_E_CANNOT_OPEN_PORT,                "DMUS_E_CANNOT_OPEN_PORT" },
    { DMUS_E_CANNOT_CONVERT,                  "DMUS_E_CANNOT_CONVERT" },
//    { DMUS_E_DESCEND_CHUNK_FAIL,              "DMUS_E_DESCEND_CHUNK_FAIL" },
//    { DMUS_E_NOT_LOADED,                      "DMUS_E_NOT_LOADED" },
//    { DMUS_E_SCRIPT_LANGUAGE_INCOMPATIBLE,    "DMUS_E_SCRIPT_LANGUAGE_INCOMPATIBLE" },
//    { DMUS_E_SCRIPT_UNSUPPORTED_VARTYPE,      "DMUS_E_SCRIPT_UNSUPPORTED_VARTYPE" },
    { DMUS_E_SCRIPT_ERROR_IN_SCRIPT,          "DMUS_E_SCRIPT_ERROR_IN_SCRIPT" },
    { DMUS_E_SCRIPT_VARIABLE_NOT_FOUND,       "DMUS_E_SCRIPT_VARIABLE_NOT_FOUND"},
    { DMUS_E_SCRIPT_ROUTINE_NOT_FOUND,        "DMUS_E_SCRIPT_ROUTINE_NOT_FOUND"},
    { DMUS_E_SCRIPT_CONTENT_READONLY,         "DMUS_E_SCRIPT_CONTENT_READONLY"},
    { DMUS_E_SCRIPT_NOT_A_REFERENCE,          "DMUS_E_SCRIPT_NOT_A_REFERENCE"},
    { DMUS_E_SCRIPT_VALUE_NOT_SUPPORTED,      "DMUS_E_SCRIPT_VALUE_NOT_SUPPORTED"},
    { DMUS_E_AUDIOVBSCRIPT_SYNTAXERROR,       "DMUS_E_AUDIOVBSCRIPT_SYNTAXERROR"},
    { DMUS_E_AUDIOVBSCRIPT_RUNTIMEERROR,      "DMUS_E_AUDIOVBSCRIPT_RUNTIMEERROR"},
    { DMUS_E_AUDIOVBSCRIPT_OPERATIONFAILURE,  "DMUS_E_AUDIOVBSCRIPT_OPERATIONFAILURE"},
//    { DMUS_E_SCRIPT_CANTLOAD_OLEAUT32,        "DMUS_E_SCRIPT_CANTLOAD_OLEAUT32" },
//    { DMUS_E_SCRIPT_LOADSCRIPT_ERROR,         "DMUS_E_SCRIPT_LOADSCRIPT_ERROR" },
//    { DMUS_E_SCRIPT_INVALID_FILE,             "DMUS_E_SCRIPT_INVALID_FILE" },
//    { DMUS_E_INVALID_SCRIPTTRACK,             "DMUS_E_INVALID_SCRIPTTRACK" },
//    { DMUS_E_INVALID_SEGMENTTRIGGERTRACK,     "DMUS_E_INVALID_SEGMENTTRIGGERTRACK" },
//    { DMUS_E_INVALID_LYRICSTRACK,             "DMUS_E_INVALID_LYRICSTRACK" },
//    { DMUS_E_INVALID_PARAMCONTROLTRACK,       "DMUS_E_INVALID_PARAMCONTROLTRACK" },
    { DMUS_E_AUDIOVBSCRIPT_SYNTAXERROR,       "DMUS_E_AUDIOVBSCRIPT_SYNTAXERROR" },
    { DMUS_E_AUDIOVBSCRIPT_RUNTIMEERROR,      "DMUS_E_AUDIOVBSCRIPT_RUNTIMEERROR" },
    { DMUS_E_AUDIOVBSCRIPT_OPERATIONFAILURE,  "DMUS_E_AUDIOVBSCRIPT_OPERATIONFAILURE" },
//    { DMUS_E_AUDIOPATHS_NOT_VALID,            "DMUS_E_AUDIOPATHS_NOT_VALID" },
//    { DMUS_E_AUDIOPATHS_IN_USE,               "DMUS_E_AUDIOPATHS_IN_USE" },
    { DMUS_E_NO_AUDIOPATH_CONFIG,             "DMUS_E_NO_AUDIOPATH_CONFIG" },
//    { DMUS_E_AUDIOPATH_INACTIVE,              "DMUS_E_AUDIOPATH_INACTIVE"},
//    { DMUS_E_AUDIOPATH_NOPORT,                "DMUS_E_AUDIOPATH_NOPORT"},
//    { DMUS_E_NO_AUDIOPATH,                    "DMUS_E_NO_AUDIOPATH" },
//    { DMUS_E_AUDIOPATH_NOGLOBALFXBUFFER,      "DMUS_E_AUDIOPATH_NOGLOBALFXBUFFER"},
//    { DMUS_E_INVALIDCHUNK,                    "DMUS_E_INVALIDCHUNK"},
//    { DMUS_E_INVALID_CONTAINER_OBJECT,        "DMUS_E_INVALID_CONTAINER_OBJECT"},         
    { DS_OK,                                  "DS_OK"},
//    { DS_NO_VIRTUALIZATION ,                  "DS_NO_VIRTUALIZATION"},
//    { DS_INCOMPLETE ,                         "DS_INCOMPLETE"},
//    { DSERR_ACCESSDENIED ,                    "DSERR_ACCESSDENIED"},
//    { DSERR_ALLOCATED ,                       "DSERR_ALLOCATED"},
//    { DSERR_ALREADYINITIALIZED ,              "DSERR_ALREADYINITIALIZED"},
//    { DSERR_BADFORMAT ,                       "DSERR_BADFORMAT"},
//    { DSERR_BADSENDBUFFERGUID,                "DSERR_BADSENDBUFFERGUID"},
//    { DSERR_BUFFERLOST ,                      "DSERR_BUFFERLOST"},
    { DSERR_CONTROLUNAVAIL ,                  "DSERR_CONTROLUNAVAIL"},
//    { DSERR_DS8_REQUIRED,                     "DSERR_DS8_REQUIRED"},
    { DSERR_GENERIC ,                         "DSERR_GENERIC"},
//    { DSERR_HWUNAVAIL ,                       "DSERR_HWUNAVAIL"},
    { DSERR_INVALIDCALL ,                     "DSERR_INVALIDCALL"},
//    { DSERR_INVALIDPARAM ,                    "DSERR_INVALIDPARAM"},
    { DSERR_NOAGGREGATION ,                   "DSERR_NOAGGREGATION"},
    { DSERR_NODRIVER ,                        "DSERR_NODRIVER"},
//    { DSERR_NOINTERFACE ,                     "DSERR_NOINTERFACE"},
//    { DSERR_OTHERAPPHASPRIO ,                 "DSERR_OTHERAPPHASPRIO"},
    { DSERR_OUTOFMEMORY ,                     "DSERR_OUTOFMEMORY"},
//  { DSERR_PROPS_DISABLED ,                  "DSERR_PROPS_DISABLED"},
//    { DSERR_PRIOLEVELNEEDED ,                 "DSERR_PRIOLEVELNEEDED"},
//    { DSERR_UNINITIALIZED ,                   "DSERR_UNINITIALIZED"},
    { DSERR_UNSUPPORTED ,                     "DSERR_UNSUPPORTED"}

};


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


    // dmusici.h
    { (GUID*)&GUID_DirectMusicAllTypes,               "GUID_DirectMusicAllTypes" },
    { (GUID*)&GUID_NOTIFICATION_SEGMENT,              "GUID_NOTIFICATION_SEGMENT" },
    { (GUID*)&GUID_NOTIFICATION_MEASUREANDBEAT,       "GUID_NOTIFICATION_MEASUREANDBEAT" },
    { (GUID*)&GUID_NOTIFICATION_CHORD,                "GUID_NOTIFICATION_CHORD" },
    { (GUID*)&GUID_NOTIFICATION_COMMAND,              "GUID_NOTIFICATION_COMMAND" },
    { (GUID*)&GUID_NOTIFICATION_RECOMPOSE,            "GUID_NOTIFICATION_RECOMPOSE" },
//    { (GUID*)&GUID_CommandParam,                      "GUID_CommandParam" },
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
//    { (GUID*)&GUID_BandParam,                         "GUID_BandParam" },
//    { (GUID*)&GUID_IDirectMusicBand,                  "GUID_IDirectMusicBand" },
//    { (GUID*)&GUID_IDirectMusicChordMap,              "GUID_IDirectMusicChordMap" },
    { (GUID*)&GUID_MuteParam,                         "GUID_MuteParam" },
//    { (GUID*)&GUID_Download,                          "GUID_Download" },
//    { (GUID*)&GUID_Unload,                            "GUID_Unload" },
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
//    { (GUID*)&GUID_DownloadToAudioPath,               "GUID_DownloadToAudioPath" },
////    { (GUID*)&GUID_UnloadFromAudioPath,               "GUID_UnloadFromAudioPath" },
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
    { (GUID*)&GUID_All_Objects,                       "GUID_All_Objects"},

// misc GUIDs (>not< part of DirectMusic)
    { (GUID*)&IID_IUnknown,                           "IID_IUnknown"},
    { (GUID*)&GUID_NULL,                              "GUID_NULL" },


// test wrapper guids
    { (GUID*)&CTIID_IUnknown,                         "CTIID_IUnknown" },
    /*
    { (GUID*)&CTIID_IDirectMusic,                       "CTIID_IDirectMusic" },
    { (GUID*)&CTIID_IDirectMusicBand,                 "CTIID_IDirectMusicBand" },
    { (GUID*)&CTIID_IDirectMusicBuffer,               "CTIID_IDirectMusicBuffer" },
    { (GUID*)&CTIID_IDirectMusicCollection,             "CTIID_IDirectMusicCollection" },
    { (GUID*)&CTIID_IDirectMusicComposer,             "CTIID_IDirectMusicComposer" },
    { (GUID*)&CTIID_IDirectMusicDownload,             "CTIID_IDirectMusicDownload" },
    { (GUID*)&CTIID_IDirectMusicDownloadedInstrument, "CTIID_IDirectMusicDownloadedInstrument" },
    { (GUID*)&CTIID_IDirectMusicGraph,                  "CTIID_IDirectMusicGraph" },
    { (GUID*)&CTIID_IDirectMusicInstrument,             "CTIID_IDirectMusicInstrument" },
*/
    { (GUID*)&CTIID_IDirectMusicLoader,               "CTIID_IDirectMusicLoader" },
/*
    { (GUID*)&CTIID_IDirectMusicObject,               "CTIID_IDirectMusicObject" },
    { (GUID*)&CTIID_IDirectMusicPerformance,            "CTIID_IDirectMusicPerformance" },
    { (GUID*)&CTIID_IDirectMusicChordMap,             "CTIID_IDirectMusicChordMap" },
    { (GUID*)&CTIID_IDirectMusicPort,                 "CTIID_IDirectMusicPort" },
    { (GUID*)&CTIID_IDirectMusicPortDownload,         "CTIID_IDirectMusicPortDownload" },
*/
    { (GUID*)&CTIID_IDirectMusicSegment,              "CTIID_IDirectMusicSegment" },
    { (GUID*)&CTIID_IDirectMusicSegmentState,         "CTIID_IDirectMusicSegmentState" },
/*
    { (GUID*)&CTIID_IDirectMusicStyle,                  "CTIID_IDirectMusicStyle" },
    { (GUID*)&CTIID_IDirectMusicSynth,                  "CTIID_IDirectMusicSynth" },
    { (GUID*)&CTIID_IDirectMusicSynthSink,            "CTIID_IDirectMusicSynthSink" },
    { (GUID*)&CTIID_IDirectMusicTool,                 "CTIID_IDirectMusicTool" },
    { (GUID*)&CTIID_IDirectMusicTrack,                  "CTIID_IDirectMusicTrack" },
    { (GUID*)&CTIID_IReferenceClock,                  "CTIID_IReferenceClock" },
    { (GUID*)&CTIID_IDirectMusicGetLoader,            "CTIID_IDirectMusicGetLoader" },

//dx7
    { (GUID*)&CTIID_IDirectMusicPerformance2,         "CTIID_IDirectMusicPerformance2" },
    { (GUID*)&CTIID_IDirectMusicSegment2,             "CTIID_IDirectMusicSegment2" },

//dx8
    { (GUID*)&CTIID_IDirectMusic8,                    "CTIID_IDirectMusic8" },
    { (GUID*)&CTIID_IDirectMusicPatternTrack,         "CTIID_IDirectMusicPatternTrack" },
*/
    { (GUID*)&CTIID_IDirectMusicSegment8,            "CTIID_IDirectMusicSegment8" },
    { (GUID*)&CTIID_IDirectMusicLoader8,              "CTIID_IDirectMusicLoader8" },
    { (GUID*)&CTIID_IDirectMusicPerformance8,         "CTIID_IDirectMusicPerformance8" },
/*
    { (GUID*)&CTIID_IDirectMusicTool8,                  "CTIID_IDirectMusicTool8" },
    { (GUID*)&CTIID_IDirectMusicStyle8,               "CTIID_IDirectMusicStyle8" },
    { (GUID*)&CTIID_IDirectMusicComposer8,              "CTIID_IDirectMusicComposer8" },
    */
    { (GUID*)&CTIID_IDirectMusicSegmentState8,        "CTIID_IDirectMusicSegmentState8" },
/*
    { (GUID*)&CTIID_IDirectMusicSynth8,               "CTIID_IDirectMusicSynth8" },
    { (GUID*)&CTIID_IDirectMusicTrack8,               "CTIID_IDirectMusicTrack8" },
    { (GUID*)&CTIID_IDirectMusicContainer,            "CTIID_IDirectMusicContainer" },
    { (GUID*)&CTIID_IDirectMusicSong,                 "CTIID_IDirectMusicSong" },
*/
    { (GUID*)&CTIID_IDirectMusicScript,               "CTIID_IDirectMusicScript" },
/*
    { (GUID*)&CTIID_IDirectMusicScriptError,            "CTIID_IDirectMusicScriptError" },
*/
    { (GUID*)&CTIID_IDirectMusicAudioPath,            "CTIID_IDirectMusicAudioPath" }

};


//---------------------------------------------------------------------------

// DMUS_APATH list (used by dmthXlatDMUS_APATH)
typedef struct
{
    DWORD dwAPathType;
    LPSTR   sz;
} DMUS_APATH_LIST;



static DMUS_APATH_LIST glAPathList[]=
{
    // dmusici.h
    { DMUS_APATH_SHARED_STEREOPLUSREVERB,   "DMUS_APATH_SHARED_STEREOPLUSREVERB" },
    { DMUS_APATH_DYNAMIC_3D,                "DMUS_APATH_DYNAMIC_3D" },
    { DMUS_APATH_DYNAMIC_MONO,              "DMUS_APATH_DYNAMIC_MONO" },
    { DMUS_APATH_SHARED_STEREO,             "DMUS_APATH_SHARED_STEREO" },
    
    { DMUS_APATH_MIXBIN_QUAD          , "DMUS_APATH_MIXBIN_QUAD"},
    { DMUS_APATH_MIXBIN_QUAD_ENV      , "DMUS_APATH_MIXBIN_QUAD_ENV"},
    { DMUS_APATH_MIXBIN_QUAD_MUSIC    , "DMUS_APATH_MIXBIN_QUAD_MUSIC"},
    { DMUS_APATH_MIXBIN_5DOT1         , "DMUS_APATH_MIXBIN_5DOT1"},
    { DMUS_APATH_MIXBIN_5DOT1_ENV     , "DMUS_APATH_MIXBIN_5DOT1_ENV"},
    { DMUS_APATH_MIXBIN_5DOT1_MUSIC   , "DMUS_APATH_MIXBIN_5DOT1_MUSIC"},
    { DMUS_APATH_MIXBIN_STEREO_EFFECTS, "DMUS_APATH_MIXBIN_STEREO_EFFECTS"},
    { 0,                                    "NULL" }



};

//---------------------------------------------------------------------------

// DMUS_STAGE list (used by dmthXlatDMUS_STAGE)
typedef struct
{
    DWORD dwStageType;
    LPSTR   sz;
} DMUS_STAGE_LIST;


static DMUS_STAGE_LIST glStageList[]=
{
    // dmusics.h
     {DMUS_PATH_SEGMENT            ,"DMUS_PATH_SEGMENT"},
//     {DMUS_PATH_SEGMENT_TRACK      ,"DMUS_PATH_SEGMENT_TRACK"},
//     {DMUS_PATH_SEGMENT_GRAPH      ,"DMUS_PATH_SEGMENT_GRAPH"},
//     {DMUS_PATH_SEGMENT_TOOL       ,"DMUS_PATH_SEGMENT_TOOL"},
     {DMUS_PATH_AUDIOPATH          ,"DMUS_PATH_AUDIOPATH"},
//     {DMUS_PATH_AUDIOPATH_GRAPH    ,"DMUS_PATH_AUDIOPATH_GRAPH"},
//     {DMUS_PATH_AUDIOPATH_TOOL     ,"DMUS_PATH_AUDIOPATH_TOOL"},
     {DMUS_PATH_PERFORMANCE        ,"DMUS_PATH_PERFORMANCE"},
//     {DMUS_PATH_PERFORMANCE_GRAPH  ,"DMUS_PATH_PERFORMANCE_GRAPH"},
//     {DMUS_PATH_PERFORMANCE_TOOL   ,"DMUS_PATH_PERFORMANCE_TOOL"},
//     {DMUS_PATH_PORT               ,"DMUS_PATH_PORT"},
     {DMUS_PATH_BUFFER             ,"DMUS_PATH_BUFFER"},
//     {DMUS_PATH_BUFFER_DMO         ,"DMUS_PATH_BUFFER_DMO"},
//     {DMUS_PATH_MIXIN_BUFFER       ,"DMUS_PATH_MIXIN_BUFFER"},
//     {DMUS_PATH_MIXIN_BUFFER_DMO   ,"DMUS_PATH_MIXIN_BUFFER_DMO"},
//     {DMUS_PATH_PRIMARY_BUFFER     ,"DMUS_PATH_PRIMARY_BUFFER"},
     {0                            ,"NULL" }

};


//===========================================================================
// dmthXlatHRESULT
//
// Translates HRESULT codes into human readable form.
//
// Parameters:
//
// Returns:
//
// History:
//  10/17/1997 - davidkl - created (adapted from tdinput sources)
//  04/03/1998 - davidkl - renamed (old name still works)
//===========================================================================
LPSTR dmthXlatHRESULT(HRESULT hRes)
{
    int i;

    for( i=0;i<sizeof( elErrors )/sizeof( elErrors[0] );i++ )
    {
        if( hRes == elErrors[i].rval )
        {
            return elErrors[i].sz;
        }
    }

    return "Unknown HRESULT";

} //*** end dmthXlatHRESULT()


//===========================================================================
// dmthXlatGUID
//
// Translates GUIDs into what you would type into your source
//
// Parameters:
//
// Returns:
//
// History:
//  03/23/1998 - davidkl - created (adapted from tdmXlatHRESULT)
//  04/03/1998 - davidkl - renamed
//===========================================================================
LPSTR dmthXlatGUID(REFGUID rguid)
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


//===========================================================================
// tdmGUIDtoString
//
// converts refguid to a string
//
// Parameters:
//
// Returns: nothing
//
// History:
//  02/27/1998 - davidkl - stole this fn from tdinput
//  04/03/1998 - davidkl - renamed
//===========================================================================
void dmthGUIDtoString(REFGUID rguid, LPSTR szBuf)
{

    sprintf(szBuf, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            rguid.Data1, rguid.Data2, rguid.Data3,
            rguid.Data4[0], rguid.Data4[1], rguid.Data4[2], rguid.Data4[3],
            rguid.Data4[4], rguid.Data4[5], rguid.Data4[6], rguid.Data4[7]);

} //*** end dmthGUIDtoString()

//===========================================================================
// Logs a GUID as data.
//
// Parameters:
//
// Returns: nothing
//
// History:
// 02/23/00 - danhaff - created.
//===========================================================================
void dmthLogRawGUID(UINT uLogLevel, REFGUID rguid)
{
    char szString[512] = {0};
    dmthGUIDtoString(rguid, szString);
    fnsLog(uLogLevel, szString);
}


//===========================================================================
// dmthXlatDMUS_APATH
//
// Translates DMUS_APATH codes into human readable form.
//
// Parameters:
//
// Returns:
//
// History:
//   02/09/2000 - danhaff - created.
//===========================================================================
LPSTR dmthXlatDMUS_APATH(DWORD dwAPath)
{
    int i;

    for( i=0;i<sizeof( glAPathList)/sizeof( glAPathList[0] );i++ )
    {
        if( dwAPath == glAPathList[i].dwAPathType)
        {
            return glAPathList[i].sz;
        }
    }

    return "Unknown DMUS_APATH";

} //*** end dmthXlatSTANDARD_PATH()

//===========================================================================
// dmthXlatDMUS_STAGE
//
// Translates DMUS_STAGE codes into human readable form.
//
// Parameters:
//
// Returns:
//
// History:
//   02/09/2000 - danhaff - created.
//===========================================================================
LPSTR dmthXlatDMUS_STAGE(DWORD dwStage)
{
    int i;

    for( i=0;i<sizeof( glStageList)/sizeof( glStageList[0] );i++ )
    {
        if( dwStage == glStageList[i].dwStageType)
        {
            return glStageList[i].sz;
        }
    }

    return "Unknown stage";

} //*** end dmthXlatDMUS_STAGE()







//===========================================================================
// dmth_lstrcpyW
//
// Copies a unicode string.
//
// Parameters:
//  LPWSTR  pszDest - destination buffer
//  LPCWSTR pcszSrc - source buffer
//
// Returns:
//  LPWSTR - ptr to dest buffer
//
// History:
//  04/16/1998 - davidkl - stolen from tdinput code
//===========================================================================
LPWSTR dmth_lstrcpyW(LPWSTR pwszDest, LPCWSTR pcwszSrc)
{
    int i = 0;

    // validate pointers
    if(!helpIsValidPtr((void *)pwszDest, sizeof(WCHAR), FALSE) ||
        !helpIsValidPtr((void *)pcwszSrc, sizeof(WCHAR), FALSE))
    {
        *pwszDest = NULL;
        return (LPWSTR)NULL;
    }

    // BUGBUG how to tell if we are about to overrun pwszDest??

    while(pcwszSrc[i] != L'\0')
    {
        pwszDest[i] = (WCHAR)pcwszSrc[i];
        i++;
    }
    pwszDest[i] = L'\0';

    return pwszDest;

} //*** end dmth_lstrcpyW()




/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//
//
//    LOG STRUCTURE HELPERS
//
//
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////




//===========================================================================
// dmthLogDMUS_OBJECTDESCStruct()
//
// Uses fnslog to output contents of a DMUS_OBJECTDESC structure
//
// Parameters:
// DMUS_OBJECTDESC  Desc
//
//  DWORD           dwSize;                         /* Size of this structure. */
//  DWORD           dwValidData;                    /* Flags indicating which fields below are valid. */
//  GUID            guidObject;                     /* Unique ID for this object. */
//  GUID            guidClass;                      /* GUID for the class of object. */
//  FILETIME        ftDate;                         /* Last edited date of object. */
//  DMUS_VERSION    vVersion;                       /* Version. */
//  WCHAR           wszName[DMUS_MAX_NAME];         /* Name of object.  Should we support Unicode? How? */
//  WCHAR           wszCategory[DMUS_MAX_CATEGORY]; /* Category for object (optional). */
//  WCHAR           wszFileName[DMUS_MAX_FILENAME]; /* File path. */
//
// History:
//  03/17/1998 - jimmo - created
//  03/29/1998 - davidkl - no need to return a value
//  04/01/1998 - davidkl - kelly verified that we do not need to convert
//                         strings for logging
//===========================================================================
void dmthLogDMUS_OBJECTDESCStruct(DMUS_OBJECTDESC Desc)
{
    char     szIID[MAX_LOGSTRING];

    fnsIncrementIndent();

    fnsLog(STRUCTLOGLEVEL,		"DMUS_OBJECTDESC");
    fnsLog(STRUCTLOGLEVEL,		"===============");

    /* Size of this structure. */
    fnsLog(STRUCTLOGLEVEL,		"dwSize                == %d",
            Desc.dwSize);
    /* Flags indicating which fields below are valid. */
    fnsLog(STRUCTLOGLEVEL,		"dwValidData           == %08Xh",
            Desc.dwValidData);

    /* Unique ID for this object. */
    if (Desc.dwValidData & DMUS_OBJ_OBJECT)
    {
         tdmGUIDtoString((REFGUID)Desc.guidObject, szIID);
        fnsLog(STRUCTLOGLEVEL,	"guidObject            == %s (%s)",
                szIID, tdmXlatGUID((REFGUID)Desc.guidObject));
    }

    /* GUID for the class of object. */
    if (Desc.dwValidData & DMUS_OBJ_CLASS)
    {
         tdmGUIDtoString((REFGUID)Desc.guidClass, szIID);
        fnsLog(STRUCTLOGLEVEL,	"guidClass             == %s (%s)",
                szIID, tdmXlatGUID((REFGUID)Desc.guidClass));
    }

    /* Last edited date of object. */
    fnsLog(STRUCTLOGLEVEL,		"ftDate.LowDateTime    == %d",
            Desc.ftDate.dwLowDateTime);
    fnsLog(STRUCTLOGLEVEL,		"ftDate.HighDateTime   == %d",
            Desc.ftDate.dwHighDateTime);

    //BUGBUG  MS DWORD needs to be shifted.  -jimmo
    /* Version. */
    if (Desc.dwValidData & DMUS_OBJ_VERSION)
    {
        fnsLog(STRUCTLOGLEVEL,	"Version               == %d.%d",
                Desc.vVersion.dwVersionMS, Desc.vVersion.dwVersionLS);
    }

    /* Name of object. */
    if (Desc.dwValidData & DMUS_OBJ_NAME){
        fnsLog(STRUCTLOGLEVEL,	"Name                  == %ls",
                Desc.wszName);
    }

    /* Category for object (optional). */
    if (Desc.dwValidData & DMUS_OBJ_CATEGORY){
        fnsLog(STRUCTLOGLEVEL,	"Category              == %ls",
                Desc.wszCategory);
    }

    /* File path. */
    if (Desc.dwValidData & DMUS_OBJ_FULLPATH){
        fnsLog(STRUCTLOGLEVEL,	"FileName (fullpath)   == %ls",
                Desc.wszFileName);
    }
    else if (Desc.dwValidData & DMUS_OBJ_FILENAME){
        fnsLog(STRUCTLOGLEVEL,	"FileName              == %ls",
                Desc.wszFileName);
    }

    fnsDecrementIndent();

} //*** end dmthLogDMUS_OBJECTDESCStruct()


//===========================================================================
// dmthLogDMUS_SCRIPT_ERRORINFO
//
// Logs the contents of DMUS_SCRIPT_ERRORINFO structure
//
// Parameters:
//  UINT                    uLogLevel   - fnshell logging level
//  DMUS_SCRIPT_ERRORINFO*  pdmpc      - pointer to DMUS_SCRIPT_ERRORINFO
//
// Returns: nothing
//
// History:
//  02/17/2000 - jimmo - created
//===========================================================================
void
dmthLogDMUS_SCRIPT_ERRORINFO
(
    UINT                    uLogLevel,
    DMUS_SCRIPT_ERRORINFO*  pdmScriptErrorInfo
)
{
    //////////////////////////////////////////////////////////////////
    // validate pdmScriptErrorInfo
    //////////////////////////////////////////////////////////////////
    if(!helpIsValidPtr((void*)pdmScriptErrorInfo, sizeof(DMUS_SCRIPT_ERRORINFO), FALSE))
    {
        fnsLog(uLogLevel, "Invalid DMUS_SCRIPT_ERRORINFO pointer (%p)..."
                "Unable to log structure contents", pdmScriptErrorInfo);
        return;
    }

    //////////////////////////////////////////////////////////////////
    // log the contents of pdmScriptErrorInfo
    //////////////////////////////////////////////////////////////////
    fnsLog(uLogLevel, "DMUS_SCRIPT_ERRORINFO structure");
    fnsLog(uLogLevel, "===============================");
    fnsLog(uLogLevel, "dwSize             == %08Xh(%08d)",  pdmScriptErrorInfo->dwSize,pdmScriptErrorInfo->dwSize);
    fnsLog(uLogLevel, "hr                 == %08Xh",        pdmScriptErrorInfo->hr);
    fnsLog(uLogLevel, "ulLineNumber       == %d",           pdmScriptErrorInfo->ulLineNumber);
    fnsLog(uLogLevel, "ichCharPosition    == %d",           pdmScriptErrorInfo->ichCharPosition);
    fnsLog(uLogLevel, "wszSourceFile      == %s",          pdmScriptErrorInfo->wszSourceFile);
    fnsLog(uLogLevel, "wszSourceComponent == %s",          pdmScriptErrorInfo->wszSourceComponent);
    fnsLog(uLogLevel, "wszDescription     == %s",          pdmScriptErrorInfo->wszDescription);
    fnsLog(uLogLevel, "wszSourceLineText  == %s",          pdmScriptErrorInfo->wszSourceLineText);

} //*** end tdmLogDMUS_SCRIPT_ERRORINFO()





struct PAIR
{
DWORD dwFlag;
CHAR *szString;
};

PAIR Flags[] =
            {
            {DMUS_SEGF_REFTIME             ,"DMUS_SEGF_REFTIME "},
            {DMUS_SEGF_SECONDARY           ,"DMUS_SEGF_SECONDARY "},
            {DMUS_SEGF_QUEUE               ,"DMUS_SEGF_QUEUE "},
            {DMUS_SEGF_CONTROL             ,"DMUS_SEGF_CONTROL "},
            {DMUS_SEGF_AFTERPREPARETIME    ,"DMUS_SEGF_AFTERPREPARETIME "},
            {DMUS_SEGF_GRID                ,"DMUS_SEGF_GRID "},
            {DMUS_SEGF_BEAT                ,"DMUS_SEGF_BEAT "},
            {DMUS_SEGF_MEASURE             ,"DMUS_SEGF_MEASURE "},
            {DMUS_SEGF_DEFAULT             ,"DMUS_SEGF_DEFAULT "},
            {DMUS_SEGF_NOINVALIDATE        ,"DMUS_SEGF_NOINVALIDATE "},
            {DMUS_SEGF_ALIGN               ,"DMUS_SEGF_ALIGN "},
            {DMUS_SEGF_VALID_START_BEAT    ,"DMUS_SEGF_VALID_START_BEAT "},
            {DMUS_SEGF_VALID_START_GRID    ,"DMUS_SEGF_VALID_START_GRID "},
            {DMUS_SEGF_VALID_START_TICK    ,"DMUS_SEGF_VALID_START_TICK "},
            {DMUS_SEGF_AUTOTRANSITION      ,"DMUS_SEGF_AUTOTRANSITION "},
            {DMUS_SEGF_AFTERQUEUETIME      ,"DMUS_SEGF_AFTERQUEUETIME "},
            {DMUS_SEGF_AFTERLATENCYTIME    ,"DMUS_SEGF_AFTERLATENCYTIME  "},
            {DMUS_SEGF_SEGMENTEND          ,"DMUS_SEGF_SEGMENTEND "},
            {DMUS_SEGF_MARKER              ,"DMUS_SEGF_MARKER "},
            {DMUS_SEGF_TIMESIG_ALWAYS      ,"DMUS_SEGF_TIMESIG_ALWAYS "},
            {DMUS_SEGF_USE_AUDIOPATH       ,"DMUS_SEGF_USE_AUDIOPATH "},
            {DMUS_SEGF_VALID_START_MEASURE ,"DMUS_SEGF_VALID_START_MEASURE "}
            };


LPSTR Log_DMUS_SEGF(DWORD dwFlags)
{
static CHAR szString[300];
static CHAR szString2 [300];
ZeroMemory(szString, 300);
int i;
BOOL bFirst = TRUE;

for  (i=0; i<AMOUNT(Flags); i++)
{
    if (dwFlags & Flags[i].dwFlag)
    {
        strcat(szString, Flags[i].szString);

        if (bFirst)
            bFirst = FALSE;
        else
        {
            //strcat(szString, "| ");
            sprintf(szString2, "| %s", szString);
            strcpy(szString, szString2);
        }

        
    }
}
 
return szString;

};





//===========================================================================
// dmthCoCreateInstance
//
// Wraps CoCreateInstance, if passed a test IID will return a test object
//
// History:
//  03/29/1998 - davidkl - created
//===========================================================================
HRESULT dmthDirectMusicCreateInstance(REFCLSID rclsid, 
                            LPUNKNOWN pUnkOuter,
                            REFIID riid,
                            LPVOID *ppv)
{
    HRESULT hRes                = E_NOTIMPL;
    BOOL    fValid_pUnkOuter    = TRUE;
    BOOL    fValid_ppv          = TRUE;
    void    **ppvTemp           = NULL;
    char    szCLSID[MAX_LOGSTRING];
    char    szIID[MAX_LOGSTRING];
    BOOL    fMatchFound         = FALSE;

    // validate pUnkOuter
    if(!helpIsValidPtr((void*)pUnkOuter, sizeof(IUnknown), TRUE))
    {
        fValid_pUnkOuter = FALSE;
    }

    // validate ppv
    if(!helpIsValidPtr((void*)ppv, sizeof(IUnknown), FALSE))
    {
        fValid_ppv = FALSE;
    }

    fnsIncrementIndent();

    tdmGUIDtoString(rclsid, szCLSID);
    tdmGUIDtoString(riid, szIID);

    // BUGBUG - this will need to do some wrapper class creation
    // based on test harness specific IIDs

    // log inputs
    fnsLog(CALLLOGLEVEL, "---- Calling CoCreateInstance()");
    fnsLog(PARAMLOGLEVEL, "rclsid       == %s (%s)",
            szCLSID, tdmXlatGUID(rclsid));

    fnsLog(PARAMLOGLEVEL, "riid         == %s (%s)",
            szIID, tdmXlatGUID(riid));
    fnsLog(PARAMLOGLEVEL, "ppv          == %p   %s",
            ppv,
            fValid_ppv ? "" : "BAD");

    // call the real function
    hRes = DirectMusicCreateInstance(rclsid, 
                            NULL,
                            riid,
                            ppv);

    // log results
    fnsLog(CALLLOGLEVEL, "---- Returned from CoCreateInstance()");
    fnsLog(PARAMLOGLEVEL, "hRes == %s (%08Xh)",
            tdmXlatHRESULT(hRes), hRes);
    if(fValid_ppv)
    {
        fnsLog(PARAMLOGLEVEL, "*ppv == %p",
                *ppv);
    }

    // BUGBUG - iff harness IID, wrap the interface returned
    // BUGBUG - otherwise, copy ppvTemp to ppvObj

    fnsDecrementIndent();

    // done
    return hRes;

} //*** end dmthCoCreateInstance()



LPSTR tdmXlatHRESULT(HRESULT hRes)
{
    return dmthXlatHRESULT(hRes);
};

LPSTR tdmXlatGUID(REFGUID rguid)
{
    return dmthXlatGUID(rguid);
};

LPSTR tdmXlatDMUS_APATH(DWORD dwStandardPath)
{
    return dmthXlatDMUS_APATH(dwStandardPath);
};


void tdmGUIDtoString(REFGUID rguid, LPSTR szBuf)
{
    dmthGUIDtoString(rguid, szBuf);
}
