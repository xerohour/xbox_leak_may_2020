#include "globals.h"

CClassTracker *g_pClassList = NULL;
BOOL g_bQuiet = FALSE;


HRESULT CommandLineParamFromCLSID(const GUID *pCLSID, LPSTR szParam);

CLSIDElement CLSIDTable[] =
{
{&CLSID_DirectMusic,                    "CLSID_DirectMusic",                "DirectMusicCreate(iid,ppvInterface)",              "DirectMusic object - not used on XBox"},
{&CLSID_DirectMusicCollection,          "CLSID_DirectMusicCollection",      "DirectMusicCollectionCreate(iid,ppvInterface)",    "DLS Collection: manages a DLS file - always required for MIDI + DLS"},
{&CLSID_DirectMusicSynth,               "CLSID_DirectMusicSynth",           "DirectMusicSynthCreate(iid,ppvInterface)",         "Synthesizer: controls the hardware synthesizer - always required"},
{&CLSID_DirectMusicFileStream,          "CLSID_DirectMusicFileStream",      "DirectMusicFileStreamCreate(iid,ppvInterface)",    "File Stream - used to read content from disk"},
{&CLSID_DirectMusicStreamStream,        "CLSID_DirectMusicStreamStream",    "DirectMusicStreamStreamCreate(iid,ppvInterface)",  "Stream wrapper - used to read content from app supplied IStream"},
{&CLSID_DirectMusicMemStream,           "CLSID_DirectMusicMemStream",       "DirectMusicMemStreamCreate(iid,ppvInterface)",     "Memory Stream - used to read content from memory"},
{&CLSID_DirectMusicSegment,             "CLSID_DirectMusicSegment",         "DirectMusicSegmentCreate(iid,ppvInterface)",       "Segment - primary playback mechanism - always required"},
{&CLSID_DirectMusicLoader,              "CLSID_DirectMusicLoader",          "DirectMusicLoaderCreate(iid,ppvInterface)",        "Loader: required to read files from disk and link objects. May be replaced by application"},
{&CLSID_DirectMusicTempoTrack,          "CLSID_DirectMusicTempoTrack",      "DirectMusicTempoTrackCreate(iid,ppvInterface)",    "Tempo Track - defines tempo changes, always required for music"},
{&CLSID_DirectMusicTimeSigTrack,        "CLSID_DirectMusicTimeSigTrack",    "DirectMusicTimeSigTrackCreate(iid,ppvInterface)",  "Time Signature Track - defines time signature in segments, always required for music synchronization"},
{&CLSID_DirectMusicBandTrack,           "CLSID_DirectMusicBandTrack",       "DirectMusicBandTrackCreate(iid,ppvInterface)",     "Band Track - manages one or more bands - always required for any MIDI + DLS playback"},
{&CLSID_DirectMusicPerformance,         "CLSID_DirectMusicPerformance",     "DirectMusicPerformanceCreate(iid,ppvInterface)",   "Performance - segment playback framework, always required"},
{&CLSID_DirectMusicBand,                "CLSID_DirectMusicBand",            "DirectMusicBandCreate(iid,ppvInterface)",          "Band - manages downloading of DLS instruments as well as volume and pan presets - always required for MIDI + DLS"},
{&CLSID_DirectMusicSeqTrack,            "CLSID_DirectMusicSeqTrack",        "DirectMusicSeqTrackCreate(iid,ppvInterface)",      "Sequence Track: plays straight MIDI sequences"},
{&CLSID_DirectSoundWave,                "CLSID_DirectSoundWave",            "DirectSoundWaveCreate(iid,ppvInterface)",          "Wave: represents the file version of a wave - required if wave files are used"},
{&CLSID_DirectMusicWaveTrack,           "CLSID_DirectMusicWaveTrack",       "DirectMusicWaveTrackCreate(iid,ppvInterface)",     "Wave Track - plays one or more waves"},
{&CLSID_DirectMusicChordTrack,          "CLSID_DirectMusicChordTrack",      "DirectMusicChordTrackCreate(iid,ppvInterface)",    "Chord Track - used for style playback"},
{&CLSID_DirectMusicCommandTrack,        "CLSID_DirectMusicCommandTrack",    "DirectMusicCommandTrackCreate(iid,ppvInterface)",  "Groove Track - used to store groove levels for style playback"},
{&CLSID_DirectMusicStyleTrack,          "CLSID_DirectMusicStyleTrack",      "DirectMusicStyleTrackCreate(iid,ppvInterface)",    "Style Track - selects style for playback"},
{&CLSID_DirectMusicPatternTrack,        "CLSID_DirectMusicPatternTrack",    "DirectMusicPatternTrackCreate(iid,ppvInterface)",  "Pattern Track - plays patterns with variations tranposed to chord progression"},
{&CLSID_DirectMusicMotifTrack,          "CLSID_DirectMusicMotifTrack",      "DirectMusicMotifTrackCreate(iid,ppvInterface)",    "Motif Track - mechanism for playing a style motif segment"},
{&CLSID_DirectMusicStyle,               "CLSID_DirectMusicStyle",           "DirectMusicStyleCreate(iid,ppvInterface)",         "Style - style playback object, represents a specific style type"},
{&CLSID_DirectMusicMelodyFormulationTrack, "CLSID_DirectMusicMelodyFormulationTrack", "DirectMusicMelodyFormulationTrackCreate(iid,ppvInterface)", "Melody Formulation Track - define the shape of a melody"},
{&CLSID_DirectMusicComposer,            "CLSID_DirectMusicComposer",        "DirectMusicComposerCreate(iid,ppvInterface)",      "Composer: creates playback segments and autotransitions from chordmaps and template segments"},
{&CLSID_DirectMusicChordMap,            "CLSID_DirectMusicChordMap",        "DirectMusicChordMapCreate(iid,ppvInterface)",      "ChordMap - defines chords to create chord progressions and transitions on the fly"},
{&CLSID_DirectMusicChordMapTrack,       "CLSID_DirectMusicChordMapTrack",   "DirectMusicChordMapTrackCreate(iid,ppvInterface)", "ChordMap Track - defines ChordMap to be used to compose a chord track"},
{&CLSID_DirectMusicSignPostTrack,       "CLSID_DirectMusicSignPostTrack",   "DirectMusicSignPostTrackCreate(iid,ppvInterface)", "SignPost Track - defines phrasing to be used to compose a chord track"},
{&CLSID_AutDirectMusicAudioPath,        "CLSID_AutDirectMusicAudioPath",    "DirectMusicAutAudioPathCreate(pUnkOuter,iid,ppvInterface)",     "Audio Path Automation - used by scripting to control a style directly"},
{&CLSID_AutDirectMusicAudioPathConfig,  "CLSID_AutDirectMusicAudioPathConfig", "DirectMusicAutAudioPathConfigCreate(pUnkOuter,iid,ppvInterface)", "Audio Path Configuration Automation - used by scripting to control an audiopath configuration"},
{&CLSID_AutDirectMusicSong,             "CLSID_AutDirectMusicSong",         "DirectMusicAutSongCreate(pUnkOuter,iid,ppvInterface)",          "Song Automation - used by scripting to control a song"},
{&CLSID_AutDirectMusicPerformance,      "CLSID_AutDirectMusicPerformance",  "DirectMusicAutPerformanceCreate(pUnkOuter,iid,ppvInterface)",   "Performance Automation - used by scripting to control a performance"},
{&CLSID_AutDirectMusicSegment,          "CLSID_AutDirectMusicSegment",      "DirectMusicAutSegmentCreate(pUnkOuter,iid,ppvInterface)",       "Segment Automation: used by scripting to control a segment"},
{&CLSID_DirectMusicScript,              "CLSID_DirectMusicScript",          "DirectMusicScriptCreate(iid,ppvInterface)",        "Script: Manages a script and all embedded and linked content"},
{&CLSID_DirectMusicScriptTrack,         "CLSID_DirectMusicScriptTrack",     "DirectMusicScriptTrackCreate(iid,ppvInterface)",   "Script Track: Sequences script routines to play in time with the music"},
{&CLSID_DirectMusicContainer,           "CLSID_DirectMusicContainer",       "DirectMusicContainerCreate(iid,ppvInterface)",     "Container: storage container for multiple DirectMusic file objects. Always used in scripting"},
{&CLSID_AutDirectMusicSegmentState,     "CLSID_AutDirectMusicSegmentState", "AutDirectMusicSegmentStateCreate(pUnkOuter,iid,ppvInterface)","A helper scripting object that implements IDispatch"},
{&CLSID_DirectMusicMuteTrack,           "CLSID_DirectMusicMuteTrack",       "DirectMusicMuteTrackCreate(iid,ppvInterface)",     "Mute Track - controls muting on sequence, style, and other event generating tracks"},
{&CLSID_DirectMusicMarkerTrack,         "CLSID_DirectMusicMarkerTrack",     "DirectMusicMarkerTrackCreate(iid,ppvInterface)",   "Marker Track - indicates segment start and alignment points"},
{&CLSID_DirectMusicSong,                "CLSID_DirectMusicSong",            "DirectMusicSongCreate(iid,ppvInterface)",          "Song: manages the composition and playback of a set of segments"},
{&CLSID_DirectMusicAudioPathConfig,     "CLSID_DirectMusicAudioPathConfig", "DirectMusicAudioPathConfigCreate(iid,ppvInterface)", "Audio Path Configuration: file representation of an audiopath"},
{&CLSID_DirectMusicGraph,               "CLSID_DirectMusicGraph",           "DirectMusicGraphCreate(iid,ppvInterface)",         "Tool Graph: manages a set of Tools (MIDI Processors)"},
{&CLSID_DirectMusicSysExTrack,          "CLSID_DirectMusicSysExTrack",      "DirectMusicSysexTrackCreate(iid,ppvInterface)",    "System Exclusive Track: sends system exclusive messages to the synthesizer"},
{&CLSID_DirectMusicSegmentTriggerTrack, "CLSID_DirectMusicSegmentTriggerTrack", "DirectMusicTriggerTrackCreate(iid,ppvInterface)", "Segment Trigger Track: triggers additional segments"},
{&CLSID_DirectMusicLyricsTrack,         "CLSID_DirectMusicLyricsTrack",     "DirectMusicLyricsTrackCreate(iid,ppvInterface)",   "Lyrics Track: sends lyrics"},
{&CLSID_DirectMusicParamControlTrack,   "CLSID_DirectMusicParamControlTrack","DirectMusicParamsTrackCreate(iid,ppvInterface)", "Parameter Control Track: provides realtime control of audio and MIDI effects"},

{&CLSID_DirectMusicAudioVBScriptIgnore, "CLSID_DirectMusicAudioVBScriptIgnore", "ERROR(1): Please report.", "ERROR(1): Please report."},
{&CLSID_DirectMusicIgnoreThisOne,       "CLSID_DirectMusicIgnoreThisOne",       "ERROR(2): Please report.", "ERROR(2): Please report."},
};
DWORD dwCLSIDTableSize = NUMELEMS(CLSIDTable);

/********************************************************************************
********************************************************************************/
const GUID *DefaultCLSIDsAdd[] = 
{
&CLSID_DirectMusicSynth,
&CLSID_DirectMusicFileStream,  
&CLSID_DirectMusicStreamStream,
&CLSID_DirectMusicMemStream,
&CLSID_DirectMusicComposer,
&CLSID_DirectMusicChordTrack,
&CLSID_DirectMusicChordMapTrack,
&CLSID_DirectMusicCommandTrack
};


/********************************************************************************
********************************************************************************/
const GUID *DefaultCLSIDsRemove[] = 
{
&CLSID_DirectMusic,
&CLSID_DirectMusicIgnoreThisOne,
&CLSID_DirectMusicAudioVBScriptIgnore
};

/********************************************************************************
********************************************************************************/
const GUID *DefaultCLSIDsIgnore[] = 
{
&CLSID_DirectMusic,
&CLSID_DirectMusicIgnoreThisOne,
&CLSID_DirectMusicAudioVBScriptIgnore
};


/********************************************************************************
********************************************************************************/
BOOL IgnoreThisCLSID(const GUID &guid)
{
    for (int i=0; i<NUMELEMS(DefaultCLSIDsIgnore); i++)
    {
        if (memcmp(&guid, DefaultCLSIDsIgnore[i], sizeof(GUID))==0)
        {
            return TRUE;
        }
    }

    return FALSE;
}


/********************************************************************************
Given a CLSID, tells you what the command-line parameter for it should be.
********************************************************************************/
HRESULT CommandLineParamFromCLSID(const GUID *pCLSID, LPSTR szParam)
{
    HRESULT hr = S_OK;
    
    struct MAPPING
    {
        LPCSTR pContainedString;
        LPCSTR pShortened;
    };

    MAPPING Mappings[]= { {"CLSID_DirectMusic",    "DM"},
                          {"CLSID_AutDirectMusic", "ADM"},
                          {"CLSID_DirectSound",    "DS"} };    
    
    BOOL bFoundCLSID     = FALSE;
    BOOL bFoundSubString = FALSE;
    
    bFoundCLSID     = FALSE;
    for (int i = 0; i<NUMELEMS(CLSIDTable) && SUCCEEDED(hr); i++)
    {
        if (IsEqualGUID(*(CLSIDTable[i].pclsid), *pCLSID ))
        {

            //Note that we found this.
            bFoundCLSID = TRUE;

            //Now find which prefix we're using.
            bFoundSubString = FALSE;
            for (int j=0; j<NUMELEMS(Mappings) && SUCCEEDED(hr); j++)
            {
                if (0 == strncmp(CLSIDTable[i].pszClassName, Mappings[j].pContainedString, strlen(Mappings[j].pContainedString)))
                {
                    strcpy(szParam, Mappings[j].pShortened);
                    LPCSTR szRemainder = CLSIDTable[i].pszClassName + strlen(Mappings[j].pContainedString);
                    strcat(szParam, szRemainder);
                    bFoundSubString = TRUE;
                    break;
                }
            }           

            if (!bFoundSubString)
            {
                printf("Error: Could not create an command-line parameter name for %s\n", CLSIDTable[i].pszClassName);
                hr = E_FAIL;
            }
        }
    }

    if (!bFoundCLSID)
    {
        _tprintf( TEXT("Unknown CLSID: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n"), pCLSID->Data1, pCLSID->Data2, pCLSID->Data3, pCLSID->Data4[0], pCLSID->Data4[1], pCLSID->Data4[2], pCLSID->Data4[3], pCLSID->Data4[4], pCLSID->Data4[5], pCLSID->Data4[6], pCLSID->Data4[7] );
        hr = E_FAIL;
    }

return hr;
};



//TODO:
//Add examples to PrintUsage function.

/********************************************************************************

AUDANALYZE                                                                  
    Description
    -----------
    It analyzes your application's DMusic content and writes a
    "factory" function containing a list of DMusic objects needed to run your
    content. When objects are omittied from this list, their code is also
    omitted from your application by the linker, decreasing your app's size.
    If you use DMusic's default factory function, it won't know anything
    about your content, and must include code for all possible DMusic objects,
    much of which may be unnecessary.  

    AudAnalyze is a tool that helps decrease the size of your DMusic
    applications.  Based on a list of content you provide, AudAnalyze
    creates a file containing a function named MyFactory() to replace       
    DirectMusicDefaultFactory().  This function contains a list of DMusic
    objects needed to play your content.  If your code passes MyFactory()
    instead of DirectMusicDefaultFactory() to DirectMusicInitializeEx(),
    the linker will exclude unneeded DMusic objects from your XBox app,
    decreasing its size.  (DirectMusicDefaultFactory() includes ALL the
    objects, whether needed or not.)

    Explicity Adding/Omitting DMusic Objects
    ----------------------------------------
    Note that AudAnalyze can not be entirely accurate for all content.  
    Some required objects may be ommitted from MyFactory().  If this is the case,
    when DMusic tries to create an object of that type, MyFactory() will
    print an error message stating the CLSID of the unknown object, and call
    DebugBreak().  

Usage:                                                                      
    %s [SEGMENT | LIST]... [[+|-]ObjectType][...] [/All] [/Q] OutputFile
                                                                            
    SEGMENT:     The name of a DirectMusic segment file, or a wildcard specifying
                 a set of files.
    LIST:        A file ending in .TXT containing a list of segments, one on each
                 line.  May not contain wildcards.  Relative pathnames in list
                 files are relative to the current directory, which is not
                 necessarily the list file's directory.
    ObjectType:  A DirectMusic object (see list below).                     
    OutputFile:  Output file containing MyFactory().
    +            Forces inclusion of an object type.                                  
    -            Forces exclusion of an object type.                                  
    /all         Forces inclusion of all object types.
    /q           Quiet mode.  Doesn't ask before overwriting destination file.


    Examples
    --------    
    Simply analyze one segment:
        AudAnalyze MySegment.sgt FACTORY.CPP

    Analyze two segments:
        AudAnalyze MySegment.sgt AnotherSegment.sgt FACTORY.CPP

    Analyze all segments in these subdirectories:
        AudAnalyze Disco\*.sgt HipHop\*.sgt FACTORY.CPP

    Analyze all segments in this list:
        AudAnalyze MyPlayList.txt FACTORY.CPP

    Analyze all segments, and add the 
        AudAnalyze MyPlayList.txt FACTORY.CPP

    Include everything.
        AudAnalyze /all FACTORY.CPP

    How To Convert Your Existing DMusic App To Use AudAnalyze
    ---------------------------------------------------------
    1) Put all your content into a directory.
    2) Run "AudAnalyze AllMyContent\*.* FACTORY.CPP
    3) #include "factory.cpp" in another one of your CPP files
       that has dmusici.h already included.
    4) Instead of using DirectMusicInitialze(), use
       DirectMusicInitialzeEx() or
       DirectMusicInitializeFixedSizeHeaps().  Admittedly there's
       currently no convenient way to keep your heap behavior
       while using this feature.  Anyway, pass MyFactory to
       the Init function you're using.
    5) Compile and run your app.  If it runs fine, go to step 11.
    6) Your app asserted.  Note the CLSID it was looking for.
    7) Find the corresponding CLSID in either dmusici.h or just
       above MyFactory().
    8) Type "AudAnalyze" with no parameters to find the parameter
       matching the CLSID you need to add
    9) Run AudAnalyze again on your content, adding the your parameter
       preceeded by a '+' (i.e. +MyParameter).
    10)Go back to step 5.   
    11)Great!  Your app is running!  However you may have EXTRA objects
       in your MyFactory() function that aren't needed.  If you see
       an object like CLSID_DirectMusicChordMapTrack used in MyFactory()
       but your content contains no chordmap tracks, it's a good candidate
       for removal.  Comment it out, or rerun AudAnalyze with the
       -DMChordMapTrack parameter.  If you get no asserts, congrats, you've
       just decreased your code size!!



********************************************************************************/
HRESULT PrintUsage(void)
{
printf("AUDANALYZE                                                                  \n");
printf("    Description\n");
printf("    -----------\n");
printf("    It analyzes your application's DMusic content and writes a\n");
printf("    \"factory\" function containing a list of DMusic objects needed to run your\n");
printf("    content. When objects are omittied from this list, their code is also\n");
printf("    omitted from your application by the linker, decreasing your app's size.\n");
printf("    If you use DMusic's default factory function, it won't know anything\n");
printf("    about your content, and must include code for all possible DMusic objects,\n");
printf("    much of which may be unnecessary.  \n");
printf("\n");
printf("    AudAnalyze is a tool that helps decrease the size of your DMusic\n");
printf("    applications.  Based on a list of content you provide, AudAnalyze\n");
printf("    creates a file containing a function named MyFactory() to replace       \n");
printf("    DirectMusicDefaultFactory().  This function contains a list of DMusic\n");
printf("    objects needed to play your content.  If your code passes MyFactory()\n");
printf("    instead of DirectMusicDefaultFactory() to DirectMusicInitializeEx(),\n");
printf("    the linker will exclude unneeded DMusic objects from your XBox app,\n");
printf("    decreasing its size.  (DirectMusicDefaultFactory() includes ALL the\n");
printf("    objects, whether needed or not.)\n");
printf("\n");
printf("    Explicity Adding/Omitting DMusic Objects\n");
printf("    ----------------------------------------\n");
printf("    Note that AudAnalyze can not be entirely accurate for all content.  \n");
printf("    Some required objects may be ommitted from MyFactory().  If this is the case,\n");
printf("    when DMusic tries to create an object of that type, MyFactory() will\n");
printf("    print an error message stating the CLSID of the unknown object, and call\n");
printf("    DebugBreak().  \n");
printf("\n");
printf("Usage:                                                                      \n");
printf("    AUDANALYZE [SEGMENT | LIST]... [[+|-]ObjectType][...] [/All] [/Q] OutputFile\n");
printf("                                                                            \n");
printf("    SEGMENT:     The name of a DirectMusic segment file, or a wildcard specifying\n");
printf("                 a set of files.\n");
printf("    LIST:        A file ending in .TXT containing a list of segments, one on each\n");
printf("                 line.  May not contain wildcards.  Relative pathnames in list\n");
printf("                 files are relative to the current directory, which is not\n");
printf("                 necessarily the list file's directory.\n");
printf("    ObjectType:  A DirectMusic object (see list below).                     \n");
printf("    OutputFile:  Output file containing MyFactory().\n");
printf("    +            Forces inclusion of an object type.                              \n");
printf("    -            Forces exclusion of an object type.                              \n");
printf("    /all         Forces inclusion of all object types.\n");
printf("    /q           Quiet mode.  Doesn't ask before overwriting destination file.\n");
printf("\n");
printf("\n");
printf("    Examples\n");
printf("    --------    \n");
printf("    Simply analyze one segment:\n");
printf("        AudAnalyze MySegment.sgt FACTORY.CPP\n");
printf("\n");
printf("    Analyze two segments:\n");
printf("        AudAnalyze MySegment.sgt AnotherSegment.sgt FACTORY.CPP\n");
printf("\n");
printf("    Analyze all segments in these subdirectories:\n");
printf("        AudAnalyze Disco\\*.sgt HipHop\\*.sgt FACTORY.CPP\n");
printf("\n");
printf("    Analyze all segments in this list:\n");
printf("        AudAnalyze MyPlayList.txt FACTORY.CPP\n");
printf("\n");
printf("    Analyze all segments, and add the \n");
printf("        AudAnalyze MyPlayList.txt FACTORY.CPP\n");
printf("\n");
printf("    Include everything.\n");
printf("        AudAnalyze /all FACTORY.CPP\n");
printf("\n");
printf("\n");
printf("    List of object types:                                                   \n");
printf("    CLSID                                    Corresponding parameter name   \n");
printf("    ---------------------------------------- -------------------------------\n");

for (DWORD i=0; i<dwCLSIDTableSize; i++)
{
    CHAR szParam[MAX_PATH];
    CommandLineParamFromCLSID(CLSIDTable[i].pclsid, szParam);
    if (IgnoreThisCLSID(*(CLSIDTable[i].pclsid)))
        continue;
    printf("    %-40s: %s\n", CLSIDTable[i].pszClassName, szParam);
}
printf("\n");
printf("\n");
printf("\n");

return S_OK;
};




/********************************************************************************
********************************************************************************/
void HandleCLSID( REFCLSID clsid )
{
    BOOL bFound = FALSE;
    for (int i = 0; i<NUMELEMS(CLSIDTable); i++)
    {
        if (IsEqualGUID(*(CLSIDTable[i].pclsid), clsid ))
        {
            InsertClass(CLSIDTable[i].pszClassName, CLSIDTable[i].pszFunctionName, CLSIDTable[i].pszComment);           
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        _tprintf( TEXT("Unknown CLSID: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n"), clsid.Data1, clsid.Data2, clsid.Data3, clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6], clsid.Data4[7] );
    }
}


/********************************************************************************
********************************************************************************/
void RemoveCLSID( REFCLSID clsid )
{
    BOOL bFound = FALSE;
    for (int i = 0; i<NUMELEMS(CLSIDTable); i++)
    {
        if (IsEqualGUID(*(CLSIDTable[i].pclsid), clsid ))
        {
            RemoveClass(CLSIDTable[i].pszClassName);           
            bFound = TRUE;
            break;
        }
    }

    if (!bFound)
    {
        _tprintf( TEXT("Unknown CLSID: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\n"), clsid.Data1, clsid.Data2, clsid.Data3, clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6], clsid.Data4[7] );
    }
}

/********************************************************************************
********************************************************************************/
void CreateClassLinkFile(TCHAR *pszFileName)

{
        char *szHeader[] = {
            "                                                                                                  \r\n",
            "//--------------------------------------------------------------------------------------------    \r\n",
            "DEFINE_GUID(CLSID_DirectMusicBand,0x79ba9e00, 0xb6ee, 0x11d1, 0x86, 0xbe, 0x0, 0xc0, 0x4f, 0xbf, 0x8f, 0xef); \r\n",
            "DEFINE_GUID(CLSID_DirectMusicTempoTrack,0xd2ac2885, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicSeqTrack,0xd2ac2886, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicSysExTrack,0xd2ac2887, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicTimeSigTrack,0xd2ac2888, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicChordTrack,0xd2ac288b, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicCommandTrack,0xd2ac288c, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicStyleTrack,0xd2ac288d, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicMotifTrack,0xd2ac288e, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicSignPostTrack,0xf17e8672, 0xc3b4, 0x11d1, 0x87, 0xb, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicBandTrack,0xd2ac2894, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicChordMapTrack,0xd2ac2896, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicMuteTrack,0xd2ac2898, 0xb39b, 0x11d1, 0x87, 0x4, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xbd);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicScriptTrack,0x4108fa85, 0x3586, 0x11d3, 0x8b, 0xd7, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); /* {4108FA85-3586-11d3-8BD7-00600893B1B6} */\r\n",
            "DEFINE_GUID(CLSID_DirectMusicMarkerTrack,0x55a8fd00, 0x4288, 0x11d3, 0x9b, 0xd1, 0x8a, 0xd, 0x61, 0xc8, 0x88, 0x35);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicSegmentTriggerTrack, 0xbae4d665, 0x4ea1, 0x11d3, 0x8b, 0xda, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); /* {BAE4D665-4EA1-11d3-8BDA-00600893B1B6} */\r\n",
            "DEFINE_GUID(CLSID_DirectMusicLyricsTrack, 0x995c1cf5, 0x54ff, 0x11d3, 0x8b, 0xda, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); /* {995C1CF5-54FF-11d3-8BDA-00600893B1B6} */\r\n",
            "DEFINE_GUID(CLSID_DirectMusicParamControlTrack, 0x4be0537b, 0x5c19, 0x11d3, 0x8b, 0xdc, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); /* {4BE0537B-5C19-11d3-8BDC-00600893B1B6} */\r\n",
            "DEFINE_GUID(CLSID_DirectMusicMelodyFormulationTrack, 0xb0684266, 0xb57f, 0x11d2, 0x97, 0xf9, 0x0, 0xc0, 0x4f, 0xa3, 0x6e, 0x58);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicWaveTrack,0xeed36461, 0x9ea5, 0x11d3, 0x9b, 0xd1, 0x0, 0x80, 0xc7, 0x15, 0xa, 0x74);\r\n",
            "DEFINE_GUID(CLSID_DirectMusicSynth,0x58C2B4D0,0x46E7,0x11D1,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);\r\n",
            "DEFINE_GUID(CLSID_AutDirectMusicPerformance,		0xa861c6e2, 0xfcfc, 0x11d2, 0x8b, 0xc9, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); // {A861C6E2-FCFC-11d2-8BC9-00600893B1B6}\r\n",
            "DEFINE_GUID(CLSID_AutDirectMusicSegment,			0x4062c116, 0x0270, 0x11d3, 0x8b, 0xcb, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); // {4062C116-0270-11d3-8BCB-00600893B1B6}\r\n",
            "DEFINE_GUID(CLSID_AutDirectMusicSong,				0xa16f1761, 0xb6d8, 0x42eb, 0x8d, 0x57, 0x4a, 0x44, 0xfe, 0xdd, 0x3b, 0xd2);// {A16F1761-B6D8-42eb-8D57-4A44FEDD3BD2}\r\n",
            "DEFINE_GUID(CLSID_AutDirectMusicSegmentState,		0xebf2320a, 0x2502, 0x11d3, 0x8b, 0xd1, 0x0, 0x60, 0x8, 0x93, 0xb1, 0xb6); // {EBF2320A-2502-11d3-8BD1-00600893B1B6}\r\n",
            "DEFINE_GUID(CLSID_AutDirectMusicAudioPathConfig,	0x1cebde3e, 0x6b91, 0x484a, 0xaf, 0x48, 0x5e, 0x4f, 0x4e, 0xd6, 0xb1, 0xe1);// {1CEBDE3E-6B91-484a-AF48-5E4F4ED6B1E1}\r\n",
            "DEFINE_GUID(CLSID_AutDirectMusicAudioPath,			0x2c5f9b72, 0x7148, 0x4d97, 0xbf, 0xc9, 0x68, 0xa0, 0xe0, 0x76, 0xbe, 0xbd);// {2C5F9B72-7148-4d97-BFC9-68A0E076BEBD}\r\n",
            "\r\n",
            "STDAPI DirectMusicChordTrackCreate( REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicCommandTrackCreate( REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicStyleTrackCreate( REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicMelodyFormulationTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicMuteTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicChordMapTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicChordMapCreate(REFIID iid, void ** ppChordMap );\r\n",
            "STDAPI DirectMusicSignPostTrackCreate(REFIID iid, void ** ppChordMap );\r\n",
            "STDAPI DirectMusicTempoTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicTimeSigTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicMarkerTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicBandTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicPatternTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicMotifTrackCreate(REFIID iid, void ** ppDMT );\r\n",
            "STDAPI DirectMusicWaveTrackCreate(REFIID iid, void ** ppDMT);\r\n",
            "STDAPI DirectMusicSegmentCreate( REFIID iid, void ** ppDMSeg );\r\n",
            "STDAPI DirectMusicTemplCreate(REFIID iid, void ** ppDMTempl );\r\n",
            "STDAPI DirectMusicPerformanceCreate(REFIID iid, void ** ppDMP);\r\n",
            "STDAPI DirectMusicBandCreate(REFIID iid, void ** ppBand);\r\n",
            "STDAPI DirectMusicStyleCreate(REFIID iid, void ** ppStyle);\r\n",
            "STDAPI DirectMusicSectionCreate(REFIID iid, void ** ppSection);\r\n",
            "STDAPI DirectMusicAutAudioPathCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);\r\n",
            "STDAPI DirectMusicAutAudioPathConfigCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);\r\n",
            "STDAPI DirectMusicAutSongCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);\r\n",
            "STDAPI DirectMusicAutPerformanceCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);\r\n",
            "STDAPI DirectMusicAutSegmentCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);\r\n",
            "STDAPI DirectMusicScriptCreate(REFIID iid, void ** ppScript);\r\n",
            "STDAPI DirectMusicScriptTrackCreate(REFIID iid, void ** ppTrack);\r\n",
            "STDAPI DirectMusicSongCreate(REFIID iid, void ** ppSong);\r\n",
            "STDAPI DirectMusicAudioPathConfigCreate(REFIID iid, void ** ppPath);\r\n",
            "STDAPI DirectMusicSeqTrackCreate(REFIID iid, void ** ppTrack);\r\n",
            "STDAPI DirectMusicGraphCreate(REFIID iid, void ** ppGraph);\r\n",
            "STDAPI DirectMusicSysexTrackCreate(REFIID iid, void ** ppTrack);\r\n",
            "STDAPI DirectMusicTriggerTrackCreate(REFIID iid, void ** ppTrack);\r\n",
            "STDAPI DirectMusicLyricsTrackCreate(REFIID iid, void ** ppTrack);\r\n",
            "STDAPI DirectMusicParamsTrackCreate(REFIID iid, void ** ppTrack);\r\n",
            "STDAPI DirectMusicSegStateCreate(REFIID iid, void ** ppSegState);\r\n",
            "STDAPI DirectMusicComposerCreate(REFIID iid, void ** ppDMComp8 );\r\n",
            "STDAPI DirectMusicCreate(REFIID iid, void ** ppDMC );\r\n",
            "STDAPI DirectMusicCollectionCreate(REFIID iid, void ** ppDMC );\r\n",
            "STDAPI DirectMusicSynthCreate(REFIID iid, void ** ppDMSynth );\r\n",
            "STDAPI DirectSoundWaveCreate( REFIID iid, void ** ppDSW );\r\n",
            "STDAPI DirectMusicContainerCreate( REFIID iid, void ** ppContainer );\r\n",
            "STDAPI DirectMusicLoaderCreate( REFIID iid, void ** ppLoader );\r\n",
            "STDAPI AutDirectMusicSegmentStateCreate(LPUNKNOWN pUnkOuter, REFIID iid, void ** ppLoader );\r\n",
            "STDAPI DirectMusicMemStreamCreate( REFIID iid, void ** ppStream );\r\n",
            "STDAPI DirectMusicStreamStreamCreate( REFIID iid, void ** ppStream );\r\n",
            "STDAPI DirectMusicFileStreamCreate( REFIID iid, void ** ppStream );\r\n",
            "                                                                                                  \r\n",
            "                                                                                                  \r\n",
            "HRESULT CALLBACK MyFactory(REFCLSID clsid,\r\n",
            "                           LPUNKNOWN pUnkOuter,\r\n", 
            "                           REFIID iid,\r\n", 
            "                           LPVOID *ppvInterface)\r\n",
            "{\r\n",
            "   HRESULT hr = E_FAIL;\r\n"
            
        };



    //Check to see if the file exists.
    if (!g_bQuiet)
    {
        if (_access(pszFileName, 00) == 0)
        {

            //Check to see if the file is writeable.
            if (_access(pszFileName, 02) != 0)
            {
                printf("Error: Cannot write to %s.\n", pszFileName);
                return;
            }
        
            printf("%s exists, overwrite? (y/n)\n", pszFileName);
            if (!YesOrNo())
                return;
        }
    }

    HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
		DWORD dwBytesWritten;
        for (DWORD dwI = 0; dwI < NUMELEMS(szHeader); dwI++)
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
				WriteFile(hFile,"    if ",7,&dwBytesWritten,NULL);
			}
            if (!fFirst)
            {
                WriteFile(hFile,"    else if ",12,&dwBytesWritten,NULL);
            }
            fFirst = FALSE;
            char szData[400];
            wsprintfA(szData,"(clsid == %s)\r\n    {\r\n        // %s.\r\n        hr = %s;\r\n    }\r\n",
                pBestSoFar->m_pszClassName,pBestSoFar->m_pszComment,pBestSoFar->m_pszFunctionName);
            WriteFile(hFile,szData,strlen(szData),&dwBytesWritten,NULL);
        }
        char *szTail[] = {
            "    #ifndef AUDANALYZE_NODEBUG     \r\n",
            "    else                           \r\n",
            "    {                              \r\n",
            "        OutputDebugStringA(\"DirectMusic needs to allocate an object that your MyFactory() function doesn't support.  \\n\");\r\n",
            "        OutputDebugStringA(\"Please locate the CLSID in the list above MyFactory(), or in dmusici.h, and make sure    \\n\");\r\n",
            "        OutputDebugStringA(\"AudAnalyzer includes support for this CLSID when generating MyFactory(), by using        \\n\");\r\n",
            "        OutputDebugStringA(\"a command-line parameter if necessary (see AudAnalyzer docs for more details.)           \\n\");\r\n",
            "        CHAR szTemp[MAX_PATH] ={0};\r\n",
            "        sprintf(szTemp,\"Unknown CLSID: %08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x\\n\", clsid.Data1, clsid.Data2, clsid.Data3, clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6], clsid.Data4[7] );\r\n"
            "        OutputDebugStringA(szTemp);\r\n",
            "        DebugBreak();              \r\n",
            "    }                              \r\n",
            "    #endif AUDANALYZE_NODEBUG     \r\n",
            "    return hr;                     \r\n",
            "}\r\n", 
        };
        for (dwI = 0; dwI < NUMELEMS(szTail); dwI++)
        {
            WriteFile(hFile,szTail[dwI],strlen(szTail[dwI]),&dwBytesWritten,NULL);
        }
        CloseHandle(hFile);
    }
}


/********************************************************************************
********************************************************************************/
// Detours stuff
extern "C" {
#define FUNCPTR_CoCreate HRESULT (STDAPICALLTYPE *)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter, IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv)


HRESULT (STDAPICALLTYPE *DynamicTrampoline)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv) = NULL;
}

STDAPI DynamicDetour(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	HandleCLSID( rclsid );

	return DynamicTrampoline( rclsid, pUnkOuter, dwClsContext, riid, ppv );
}



/********************************************************************************
********************************************************************************/
HRESULT ProcessDMusicFile(IDirectMusicPerformance8 *pPerf, IDirectMusicLoader8 *pLoader, LPCSTR szFileFull)
{

    HRESULT hr = S_OK;
    IDirectMusicSegment8* pSegment = NULL;
    IDirectMusicScript8*  pScript  = NULL;
    IDirectMusicContainer *  pContainer = NULL;
//    IDirectMusicAudioPath *pPath = NULL;
    IUnknown *  pStyle = NULL;
    BOOL bWarning = FALSE;


    ASSERT(pLoader);
    ASSERT(szFileFull);
    
    // DMusic only takes wide strings, for some odd, inefficient reason.
    CHAR  szDirectory[MAX_PATH] = {0};
    WCHAR wstrFileName[MAX_PATH] = {0};
    WCHAR wstrDirectory[MAX_PATH] = {0};
    
    CHECKRUN(ChopPath(szFileFull, szDirectory, NULL));
    DXUtil_ConvertGenericStringToWide( wstrFileName,  szFileFull, MAX_PATH );
    DXUtil_ConvertGenericStringToWide( wstrDirectory, szDirectory, MAX_PATH );

    ASSERT(szDirectory[0]);
    ASSERT(szFileFull [0]);
    
    //Can't use macro cuz this dumb function returns S_FALSE.
    if (SUCCEEDED(hr))
    {
        hr = pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, wstrDirectory, FALSE);
    }
    if (hr == S_FALSE)
        hr = S_OK;

    if (strstr(szFileFull, ".sgt") || strstr(szFileFull, ".wav"))
    {

	    CHECKRUN(pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                              IID_IDirectMusicSegment8,
                                              wstrFileName,
                                              (LPVOID*) &pSegment ));
        
        
//        CHECKRUN(pPerf->CreateStandardAudioPath(DMUS_APATH_SHARED_STEREOPLUSREVERB, 16, TRUE, &pPath));
//        CHECKRUN(pSegment->Download(pPath))
//        CHECKRUN(pPerf->PlaySegmentEx(pSegment, 0, 0, 0, 0, 0, 0, pPath));
    }
    else if (strstr(szFileFull, ".spt"))
    {
	    CHECKRUN(pLoader->LoadObjectFromFile( CLSID_DirectMusicScript,
                                              IID_IDirectMusicScript8,
                                              wstrFileName,
                                              (LPVOID*) &pScript ));        

    }
   else if (strstr(szFileFull, ".con"))
    {

        CHECKRUN(pLoader->LoadObjectFromFile(CLSID_DirectMusicContainer,
                                             IID_IDirectMusicContainer,
                                             wstrFileName,
                                             (void**)&pContainer)); 
    }
   else if (strstr(szFileFull, ".sty"))
    {

        CHECKRUN(pLoader->LoadObjectFromFile(CLSID_DirectMusicStyle,
                                             IID_IDirectMusicStyle,
                                             wstrFileName,
                                             (void**)&pStyle)); 
    }
    else
    {
		_ftprintf(stderr,TEXT("Warning: DMusic file [%s] is of unknown type - exluding from analysis.\n"), szFileFull);        
        bWarning = TRUE;
    }
    


	if( FAILED(hr))
    {
		_ftprintf(stderr,TEXT("Error loading DMusic file [%s]\n"), szFileFull);
    }
    else if (!bWarning)
    {
		_ftprintf(stderr,TEXT("Successfully processed [%s]\n"), szFileFull);
    }

    //pPerf->StopEx(0, 0, 0);    
//    if (pSegment && pPath)
//    {
//        pSegment->Unload(pPath);
//    }
//	RELEASE(pPath);
	RELEASE(pStyle);
	RELEASE(pContainer);
    RELEASE(pSegment);
	RELEASE(pScript);

    return hr;
}


/********************************************************************************
********************************************************************************/
HRESULT ProcessTextFile(IDirectMusicPerformance8 *pPerf, IDirectMusicLoader8 *pLoader, LPCSTR szFileFull)
{
    HRESULT hr = S_OK;
    CHAR szSegment[MAX_PATH] = {0};
    CHAR szLineOText[MAX_PATH] = {0};
    FILE *fyle = NULL;

    ASSERT(pLoader);
    ASSERT(szFileFull);

    fyle = fopen(szFileFull, "rt");
    CHECKALLOC(fyle);    
    if (SUCCEEDED(hr))
    {
        printf("Processing text file %s\n", szFileFull);
        while (fgets(szLineOText, MAX_PATH - 1, fyle) && SUCCEEDED(hr))
        {
            //Bail if it's a comment.
            if (szLineOText[0] == ';')
                continue;

            //Bail if we didn't get one item.
            DWORD dwScanned = sscanf(szLineOText, " %[^\"\n]",szSegment);
            if (1 != dwScanned)
            {
                if (0!=dwScanned)
                {
                    printf("Error: Too many tokens on line: The following line of %s was not understood and not processed:\n%s\n", szFileFull, szLineOText);
                }
                continue;
            }

            CHAR szFullPath[MAX_PATH] = {0};
            GetFullPathName(szSegment, MAX_PATH-1, szFullPath, NULL);
            CHECKRUN(ProcessDMusicFile(pPerf, pLoader, szFullPath));
        }
    }
    else
    {
        printf("Error: Could not open file %s\n", szFileFull);
    }

    //Close the file (reset it).
    if (fyle)
    {
        fclose(fyle);
        fyle = NULL;
    }
return hr;
};


/********************************************************************************
So we must test if something's a command-line param first - anything that's not
one must be a segment name.  However, we must do the actual processing of
segments first and command-line params second (since the latter process adds/removes 
things from a list), hence the "dwLookingFor" variable.


Priority of adding/removing
---------------------------
Command-line parameters
Default CLSIDs.
Content

i.e.
1) Add to list based on content
2) Add/Remove from list based on default CLSIDs
3) Add/Remove from list based on command-line parameters.
********************************************************************************/
#define FILES 0
#define DEFAULTCLSIDS 1
#define CHECKSCRIPTING 2
#define PARAMS 3


HRESULT HandleCommandLineParams(int argc, char *argv[], IDirectMusicLoader8 *pLoader, IDirectMusicPerformance8 *pPerf)
{
HRESULT hr = S_OK;
int i;
DWORD j, dwLookingFor;


for (dwLookingFor=0; dwLookingFor < 4 && SUCCEEDED(hr); dwLookingFor++)
{

    //#2 - add to list based on default CLSIDs.
    if (dwLookingFor == DEFAULTCLSIDS)
    {
        for (i=0; i<NUMELEMS(DefaultCLSIDsAdd) && SUCCEEDED(hr); i++)
        {
            CHECK(HandleCLSID(*DefaultCLSIDsAdd[i]));
        }
        for (i=0; i<NUMELEMS(DefaultCLSIDsRemove) && SUCCEEDED(hr); i++)
        {
            CHECK(RemoveCLSID(*DefaultCLSIDsRemove[i]));
        }
        continue;
    }

    //#2 - If scripts are enabled as a result of file parsing, we must also 
    //      include some other CLSIDs.
    if (dwLookingFor == CHECKSCRIPTING)
    {
        
        if (CLSIDPresent("CLSID_DirectMusicScript"))
        {
            HandleCLSID(CLSID_AutDirectMusicAudioPath);
            HandleCLSID(CLSID_AutDirectMusicPerformance);
            HandleCLSID(CLSID_AutDirectMusicSegmentState);
            HandleCLSID(CLSID_AutDirectMusicSong);
            HandleCLSID(CLSID_AutDirectMusicSegment);
            HandleCLSID(CLSID_AutDirectMusicAudioPathConfig);
        }
        continue;
    }


    //Go through all but the last parameter.
    for (i=1; i<argc-1 && SUCCEEDED(hr); i++)
    {
        CHAR szLwrParam[MAX_PATH] ={0};
        CHAR szLwrFromCLSID[MAX_PATH] ={0};

        strcpy(szLwrParam, argv[i]);
        _strlwr(szLwrParam);
    
        //First see if it's a command-line parameter.
        BOOL bFoundParam = FALSE;
        if (argv[i][0] == '-' || argv[i][0] == '+')
        {
            for (j=0; j<NUMELEMS(CLSIDTable) && SUCCEEDED(hr); j++)
            {            
            
                CHECKRUN(CommandLineParamFromCLSID(CLSIDTable[j].pclsid, szLwrFromCLSID));
                _strlwr(szLwrFromCLSID);
                if (strcmp(szLwrParam+1, szLwrFromCLSID)==0)
                {
                    bFoundParam = TRUE;

                    //If we're not looking for params yet, then just say "I found it!" and bail;
                    if (dwLookingFor == PARAMS)
                    {
                        switch(argv[i][0])
                        {
                            case '+':
                                CHECK(HandleCLSID(*(CLSIDTable[j].pclsid)));
                                printf("User added object %s\n", CLSIDTable[j].pszClassName);
                                break;
                            case '-':
                                CHECK(RemoveCLSID(*(CLSIDTable[j].pclsid)));
                                printf("User excluded object %s\n", CLSIDTable[j].pszClassName);
                                break;
                            default:
                                printf("AudAnalyze Error: Expected a '+' or '-' prefix on %s\n.  Please report this error.", argv[i]);
                                hr = E_FAIL;
                                break;
                        }
                    }
                    break;
                }
            }
        }
        else if (_strcmpi(argv[i], "/all")==0)
        {

            for (i=0; i<NUMELEMS(CLSIDTable); i++)
            {
                if (!IgnoreThisCLSID(*(CLSIDTable[i].pclsid)))
                {
                    HandleCLSID(*(CLSIDTable[i].pclsid));
                }
            }
            bFoundParam = TRUE;
        }
        else if (_strcmpi(argv[i], "/q") == 0)
        {
            g_bQuiet = TRUE;
            bFoundParam = TRUE;
        }

        if (SUCCEEDED(hr))
        {
            //If this is false, we must assume it's not a param, but a file.  
            if (!bFoundParam)
            {
                //If we're looking for files then jump on it.
                if (dwLookingFor == FILES)
                {            
                    WIN32_FIND_DATA FindFileData = {0};
                    HANDLE fyle = NULL;
                    CHAR szFullPath [MAX_PATH] = {0};
                    CHAR szDirectory[MAX_PATH] = {0};


                    //Get the full path+file name and path name of our parameter, which is a file our wildcard.
                    GetFullPathName(argv[i], MAX_PATH -1, szFullPath, NULL);                    
                    CHECKRUN(ChopPath(szFullPath, szDirectory, NULL));
                    ASSERT(szDirectory[MAX_PATH - 1] == 0);
                    ASSERT(szFullPath [MAX_PATH - 1] == 0);
                    ASSERT(szDirectory[0]);
                    ASSERT(szFullPath [0]);

                    //Are there any files that match this wildcard?
                    fyle = FindFirstFile(szFullPath, &FindFileData);


                    if (!fyle || INVALID_HANDLE_VALUE==fyle)
                    {
                        printf("warning: No files found that match %s\n", argv[i]);
                    }
                    else
                    {
                        do
                        {
                            CHAR szFullFileName[MAX_PATH] = {0};
                            sprintf(szFullFileName, "%s%s", szDirectory, FindFileData.cFileName);
                            
                            //Get the directory name from 
                            _strlwr(szFullFileName);
                            //If it's a text file, open it and read the names of all the segment files from it.                    
                            if (strstr(szFullFileName, ".txt"))
                            {
                                CHECKRUN(ProcessTextFile(pPerf, pLoader, szFullFileName));
                            }

                            //If it's a segment file, then just load and release it.
                            else
                            {
                                CHECKRUN(ProcessDMusicFile(pPerf, pLoader, szFullFileName));
                            }
                        }
                        while (FindNextFile(fyle, &FindFileData));

                        if (ERROR_NO_MORE_FILES != GetLastError())
                        {
                            CHAR szMessage[MAX_PATH] = {0};
                            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                          NULL,
                                          GetLastError(),
                                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                          szMessage,
                                          MAX_PATH - 1,
                                          NULL 
                                          );

                            printf("Error: %s\n", szMessage); 
                        }

                        if (fyle && INVALID_HANDLE_VALUE != fyle)
                        {
                            FindClose(fyle);
                        }
                    }
                }
            }

            if (FAILED(hr))
            {
                printf("Failed to process %s\n", argv[i]);
            }
        }
    }
}

if (SUCCEEDED(hr))
{
    if (argc == 1)
    {
        printf("Error: Not enough parameters.  You need at least one parameter specifying the destination file\n");
        hr = E_FAIL;
    }
    else
    {
        CreateClassLinkFile(argv[argc-1]);
    }
}

if (FAILED(hr))
    PrintUsage();



return hr;
}
#undef FILES
#undef CHECKSCRIPTING
#undef DEFAULTCLSIDS
#undef PARAMS






/********************************************************************************
********************************************************************************/
int __cdecl main(int argc, char* argv[])
{
    HRESULT hr = S_OK;
	IDirectMusicLoader8 *pIDirectMusicLoader8 = NULL;
	IDirectMusicPerformance8 *pIDirectMusicPerformance8 = NULL;

	CoInitialize( NULL );

	if( DynamicTrampoline == NULL )
	{
		HRESULT (STDAPICALLTYPE *DynamicTarget)(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
						IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv) = &CoCreateInstance;
		DynamicTrampoline = (FUNCPTR_CoCreate)DetourFunction( (PBYTE)DynamicTarget, (PBYTE)DynamicDetour );
	}

    // Create performance object
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                           IID_IDirectMusicPerformance8, (void**)&pIDirectMusicPerformance8 );
	    if( FAILED(hr) )
	    {
		    _ftprintf(stderr,TEXT("Error CoCreating CLSID_DirectMusicPerformance (0x%08X).  Make sure your have DirectX 8 installed on your machine.\n"), hr);
	    }
    }

    // Create loader object
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                               IID_IDirectMusicLoader8, (void**)&pIDirectMusicLoader8 );
	    if( FAILED(hr) )
	    {
		    _ftprintf(stderr,TEXT("Error CoCreating CLSID_DirectMusicLoader (0x%08X).\n"), hr);
	    }
    }


    // Initialize the performance with the standard audio path.
    // This initializes both DirectMusic and DirectSound and 
    // sets up the synthesizer. Typcially its easist to use an 
    // audio path for playing music and sound effects.
    
    //if (SUCCEEDED(hr))
    //{
    //    hr = pIDirectMusicPerformance8->InitAudio( NULL, NULL, NULL, DMUS_APATH_DYNAMIC_STEREO, 
    //                                    128, DMUS_AUDIOF_ALL, NULL );
	//    if( FAILED(hr) )
	//    {
	//	    _ftprintf(stderr,TEXT("Error calling Performance8->InitAudio: %08X.\n"), hr);
	//    }
    //}

    CHECKRUN(HandleCommandLineParams(argc, argv, pIDirectMusicLoader8, pIDirectMusicPerformance8));

    if (pIDirectMusicPerformance8)
    {
	    pIDirectMusicPerformance8->CloseDown();
	    RELEASE(pIDirectMusicPerformance8);
    }
	
    RELEASE(pIDirectMusicLoader8);


	if( DynamicTrampoline )
	{
		DetourRemove((PBYTE)DynamicTrampoline, (PBYTE)DynamicDetour);
		DynamicTrampoline = NULL;
	}

	CoUninitialize();

	return 0;
}

