// parse.cpp : parsing functions
//

#include "stdafx.h"


//
// NOTE: the order of the names below MUST match the
// XACT_TRACK_EVENT_TYPES enum in xactp.h
//

char *g_aszEventNames[NUM_EVENT_TYPES] = {

        "PLAY",
        "PLAYWITHPITCHANDAVOLUME",
        "STOP",
        "PITCHANDAVOLUMEVARIATION",
        "SETFREQUENCY",
        "SETVOLUME",
        "SETHEADROOM",
        "SETLFO",
        "SETEG",
        "SETFILTER",
        "MARKER",
        "LOOPSTART",
        "LOOPEND",
        "SETMIXBINVOLUMES",
        "SETEFFECTDATA"        

};

void
CXACTFileGenerator::Print(
                        IN PCHAR    pFormat,
                        IN ...
                        )
/*++

Routine Description:

    Virtual print method. Defaults to stderr

Arguments:

    IN pFormat -    Format string
    IN ... -        Additional arguments

Return Value:

    None

--*/
{
    CHAR    szString[0x400];
    va_list va;

    va_start(va, pFormat);
    vsprintf(szString, pFormat, va);
    va_end(va);

    fprintf(stderr, szString);
}

void CXACTFileGenerator::UsageFailure(int err)
{
    if (err != ERROR_SUCCESS) {
        Print("\n Error: 0x%x\n", err);
    }

    Print("\n Usage: %s [Soundbank description INI filename] [output soundbank filename]\n",
          m_szAppName);

}

void CXACTFileGenerator::DebugPrint(DWORD dwLevel, PCHAR pszFormat, ...)
{
    CHAR                    szString[0x400];
    va_list                 va;

    va_start(va, pszFormat);
    vsprintf(szString, pszFormat, va);
    va_end(va);
    
    if (m_dwDebugLevel >= dwLevel) {
        
        Print("*** %s ***\n", szString);
    }

}

int CXACTFileGenerator::ErrorCheckOnParser(PCHAR pResult, PCHAR pDefault,DWORD dwBytesRead, DWORD dwSize)
{

    if (dwBytesRead == dwSize-1) {
        
        
        DebugPrint(DBGLVL_INFO,"Parsing error, GetProfile function thought our buffer is too small!!");
        return -1;
        
    }
    
    if (dwBytesRead == dwSize-2) {        
        
        DebugPrint(DBGLVL_INFO,"Parsing error, GetProfile function thought our buffer is too small!!");
        return -1;
        
    }

    if (dwBytesRead == 0) {

        DebugPrint(DBGLVL_INFO,"Parsing error, GetProfile function returned 0 bytes");
        return -1;

    }

    if (!strcmp(pResult,pDefault)) {


        return -1;

    }


    return ERROR_SUCCESS;

}

void CXACTFileGenerator::PrintParsingError(PCHAR pSection, PCHAR pKey, PCHAR pszFormat, ...)
{
    CHAR                    szString[0x400];
    va_list                 va;

    va_start(va, pszFormat);
    vsprintf(szString, pszFormat, va);
    va_end(va);
    
    Print("*** ParsingInputFile : SectionName %s, KeyName %s.\n"\
                   "    %s\n",
                   pSection,pKey,szString);

}

int CXACTFileGenerator::ParseCommandLine(int argc, char **argv)
{

    int err = ERROR_SUCCESS;

    if (argc < 2) {

        err = -1;
        UsageFailure(err);
        return err;
    }

    if (strstr("?",argv[1])) {
        UsageFailure(ERROR_SUCCESS);
        err = -1;
        return err;
    }

    if (strstr("/?",argv[1])) {
        UsageFailure(ERROR_SUCCESS);
        err = -1;
        return err;
    }

    if (strstr("-?",argv[1])) {
        UsageFailure(ERROR_SUCCESS);
        err = -1;
        return err;
    }

    //
    // create full path name for main INI
    //

    LPTSTR pBuffer;

    if (GetFullPathName(argv[1],
        sizeof(m_szIniFileName),
        m_szIniFileName,        
        &pBuffer)== 0){


        err = GetLastError();
        Print("\n Failed to find the description INI file.Error 0x%x\n", err);
        return err;

    }

    printf("\nInput File to parse: %s\n", m_szIniFileName);
    
    memcpy(m_szOutputFileName,argv[2],sizeof(m_szOutputFileName));
    printf("Output File: %s\n\n", m_szOutputFileName);
    
    m_hInputFile = CreateFile(
        (const char *)m_szIniFileName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL);

    if (m_hInputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        Print("\n Failed to open the Ini description file.Error 0x%x\n", err);
        return err;

    }

    //
    // we dont need the input file anymore
    //

    CloseHandle(m_hInputFile);
    m_hInputFile = INVALID_HANDLE_VALUE;

    return ERROR_SUCCESS;

}


// ****************************************************************************
// parsing
// ****************************************************************************

int CXACTFileGenerator::ParseIniFile()
{

    int err = ERROR_SUCCESS;
    DWORD dwBytesRead;
    DWORD dwValue = 0;
    DWORD i=0;

    LPCTSTR lpDefault = "DefaultString";
    LPSTR lpSectionName = NULL;
    LPSTR lpKeyName = NULL;
    PCHAR lpResult  = NULL;    

    lpResult = new CHAR[SECTION_MAX_SIZE];
    if (lpResult == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpSectionName = new CHAR[SECTION_MAX_SIZE];
    if (lpSectionName == NULL) {
        
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpKeyName = new CHAR[SECTION_MAX_SIZE];
    if (lpKeyName == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    sprintf(lpSectionName,"%s",SECTIONNAME_MAIN);       

    //
    // see if the main section exists
    //
    
    dwBytesRead = GetPrivateProfileSection(lpSectionName,
        lpResult,
        SECTION_MAX_SIZE,
        (const char*)m_szIniFileName);
    
    if (!dwBytesRead) {
        
        PrintParsingError(lpSectionName,
            NULL,
            "FATAL: main section not found!");              
        
        goto errorExit;
        
    }

    //
    // allocate file header
    //

    m_pFileHeader = (PXACT_SOUNDBANK_FILE_HEADER) new XACT_SOUNDBANK_FILE_HEADER[1];
    if (m_pFileHeader == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    memset(m_pFileHeader,0,sizeof(XACT_SOUNDBANK_FILE_HEADER));

    // **************************************************
    // read number of CUEs
    // **************************************************

    sprintf(lpSectionName,"%s",SECTIONNAME_MAIN);       
    sprintf(lpKeyName,"%s",KEYNAME_CUE_ENTRY_COUNT);

    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;

    }
    
    m_pFileHeader->dwCueEntryCount = dwValue;

    //
    // now that we know how many CUEs we have, allocate an array
    // of cue context data
    //

    m_pCueTable = new XACT_SOUNDBANK_CUE_ENTRY[m_pFileHeader->dwCueEntryCount];
    if (m_pCueTable == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    memset(m_pCueTable,0,sizeof(XACT_SOUNDBANK_CUE_ENTRY)*m_pFileHeader->dwCueEntryCount);

    // **************************************************
    // read number of sounds
    // **************************************************

    sprintf(lpSectionName,"%s",SECTIONNAME_MAIN);       
    sprintf(lpKeyName,"%s",KEYNAME_SOUND_ENTRY_COUNT);

    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;

    }
    
    m_pFileHeader->dwSoundEntryCount = dwValue;

    m_pSoundTable = new CXACTSoundData[m_pFileHeader->dwSoundEntryCount];
    if (m_pSoundTable == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }   

	//
	// allocate contigious buffer for XACT sound entries. we do this
	// to optimize the binary file generation
	//

    m_pXACTSoundTable = new XACT_SOUNDBANK_SOUND_ENTRY[m_pFileHeader->dwSoundEntryCount];
    if (m_pXACTSoundTable == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }   

	memset(m_pXACTSoundTable,0,sizeof(XACT_SOUNDBANK_SOUND_ENTRY)*m_pFileHeader->dwSoundEntryCount);

    //
    // parse data associated with each cue
    //

    for (i=0;i<m_pFileHeader->dwCueEntryCount;i++) {

        // **************************************************
        // get sound index associated with cue
        // **************************************************

        sprintf(lpSectionName,"%s%d",SECTIONNAME_CUE,i);       
        sprintf(lpKeyName,"%s",KEYNAME_SOUND_INDEX);
        
        dwValue = GetPrivateProfileInt(lpSectionName,
            lpKeyName,
            -1,
            (const char*)m_szIniFileName);  
        
        if (dwValue == -1) {
            
            PrintParsingError(lpSectionName,
                lpKeyName,
                "ERROR: Key not found.");             
            err = -1;
            goto errorExit;
            
        }

        m_pCueTable[i].dwSoundIndex = dwValue;

        // **************************************************
        // read a friendlyname for the cue
        // **************************************************
        
        sprintf(lpSectionName,"%s%d",SECTIONNAME_CUE,i);       
        sprintf(lpKeyName,"%s",KEYNAME_CUE_NAME);
        dwBytesRead = GetPrivateProfileString(lpSectionName,
            lpKeyName,
            lpDefault,
            m_pCueTable[i].szFriendlyName,
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH,
            (const char*)m_szIniFileName);
        
        if (ErrorCheckOnParser(m_pCueTable[i].szFriendlyName,
            (char*)lpDefault,
            dwBytesRead,
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH)) {
            
            PrintParsingError(lpSectionName,
                lpKeyName,
                "WARNING: Key not found.");             
            
            sprintf(m_pCueTable[i].szFriendlyName,"DEFAULT");
            
        }

        //
        // parse information associated with sound entry used by this cue
        // only parse sounds not already referenced and parsed by other cues
        //

        if (!m_pSoundTable[m_pCueTable[i].dwSoundIndex].IsValid()) {

            err = ParseSoundData(m_pCueTable[i].dwSoundIndex);
            if (err != ERROR_SUCCESS) {

                goto errorExit;
            }

        }


    }

errorExit:
    if (lpSectionName)
        delete [] lpSectionName;

    if (lpKeyName)
        delete [] lpKeyName;

    if (lpResult)
        delete [] lpResult;


    return err;

}


int CXACTFileGenerator::ParseSoundData(DWORD dwSoundIndex)
{

    int err = ERROR_SUCCESS;
    DWORD dwBytesRead;
    DWORD dwValue = 0;
    DWORD i=0;

    CHAR szName[64];
    LPCTSTR lpDefault = "DefaultString";
    LPSTR lpSectionName = NULL;
    LPSTR lpKeyName = NULL;
    PCHAR lpResult = NULL;    

    lpResult = new CHAR[SECTION_MAX_SIZE];
    if (lpResult == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpSectionName = new CHAR[SECTION_MAX_SIZE];
    if (lpSectionName == NULL) {
        
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpKeyName = new CHAR[SECTION_MAX_SIZE];
    if (lpKeyName == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    sprintf(lpSectionName,"%s%d",SECTIONNAME_SOUND, dwSoundIndex);       

    //
    // see if sound section exists
    //
    
    dwBytesRead = GetPrivateProfileSection(lpSectionName,
        lpResult,
        SECTION_MAX_SIZE,
        (const char*)m_szIniFileName);
    
    if (!dwBytesRead) {
        
        PrintParsingError(lpSectionName,
            NULL,
            "FATAL: sound section not found!");              
        
        return -1;
        
    }

    if (m_pSoundTable[dwSoundIndex].Initialize(&m_pXACTSoundTable[dwSoundIndex]) != ERROR_SUCCESS){
        goto errorExit;
    }

    // **************************************************
    // get sound slot 
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_SOUND_LAYER);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;
        
    }
    
    m_pSoundTable[dwSoundIndex].m_pSoundEntry->wLayer = (WORD)dwValue;

    // **************************************************
    // get sound group
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_SOUND_GROUP);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;
        
    }
    
    m_pSoundTable[dwSoundIndex].m_pSoundEntry->wGroupNumber = (WORD)dwValue;

    // **************************************************
    // get wavebank count
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_SOUND_WAVEBANK_COUNT);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;
        
    }
    
    m_pSoundTable[dwSoundIndex].m_pSoundEntry->wWaveBankCount = (WORD)dwValue;

    // **************************************************
    // get track count
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_SOUND_TRACK_COUNT);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;
        
    }
        
    m_pSoundTable[dwSoundIndex].m_pSoundEntry->wTrackCount = (WORD)dwValue;
            
    // **************************************************
    // parse sound flags (keys are optional)
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_SOUND_FLAG_3D);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue != -1) {
        
        m_pSoundTable[dwSoundIndex].m_pSoundEntry->dwFlags |= XACT_FLAG_SOUND_3D;

        //
        // this sounds uses 3d properties.
        // a zero initialized 3d property object will be associated for now
        //


        
    }

    //
    // allocate wavebank table
    //

    m_pSoundTable[dwSoundIndex].m_pWaveBankTable = (PXACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY) new XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY[m_pSoundTable[dwSoundIndex].m_pSoundEntry->wWaveBankCount];
    if (!m_pSoundTable[dwSoundIndex].m_pWaveBankTable) {

        err = ERROR_OUTOFMEMORY;
        goto errorExit;

    }

    memset(m_pSoundTable[dwSoundIndex].m_pWaveBankTable,0,sizeof(XACT_SOUNDBANK_WAVEBANK_TABLE_ENTRY)*m_pSoundTable[dwSoundIndex].m_pSoundEntry->wWaveBankCount);

    //
    // retrieve wavebank table information for the sound
    //

    for (i=0;i<m_pSoundTable[dwSoundIndex].m_pSoundEntry->wWaveBankCount;i++) {
        
        sprintf(lpSectionName,"%s%d_%s",SECTIONNAME_SOUND,dwSoundIndex,SECTIONNAME_WAVEBANK_TABLE);       
        sprintf(lpKeyName,"%s%d",KEYNAME_WAVEBANK_TABLE_WAVEBANK_NAME,i);

        dwBytesRead = GetPrivateProfileString(lpSectionName,
            lpKeyName,
            lpDefault,
            szName,
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH,
            (const char*)m_szIniFileName);
        
        if (ErrorCheckOnParser(szName,
            (char*)lpDefault,
            dwBytesRead,
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH)) {
            
            PrintParsingError(lpSectionName,
                lpKeyName,
                "ERROR: Key not found.");             
            
            err = -1;
            goto errorExit;
            
        }

        strcpy(m_pSoundTable[dwSoundIndex].m_pWaveBankTable[i].szFriendlyName,szName);        
        
    }

    //
    // retrieve track information
    //

    //
    // allocate track table
    //

    m_pSoundTable[dwSoundIndex].m_pTrackTable = new CXACTTrackData[m_pSoundTable[dwSoundIndex].m_pSoundEntry->wTrackCount];
    if (!m_pSoundTable[dwSoundIndex].m_pTrackTable) {

        err = ERROR_OUTOFMEMORY;
        goto errorExit;

    }

    m_pSoundTable[dwSoundIndex].m_pXACTTrackTable = new XACT_SOUNDBANK_TRACK_ENTRY[m_pSoundTable[dwSoundIndex].m_pSoundEntry->wTrackCount];
    if (!m_pSoundTable[dwSoundIndex].m_pXACTTrackTable) {

        err = ERROR_OUTOFMEMORY;
        goto errorExit;

    }

    memset(m_pSoundTable[dwSoundIndex].m_pXACTTrackTable,0,sizeof(XACT_SOUNDBANK_TRACK_ENTRY)*m_pSoundTable[dwSoundIndex].m_pSoundEntry->wTrackCount);


    //
    // retrieve track entry information for the sound
    //

    for (i=0;i<m_pSoundTable[dwSoundIndex].m_pSoundEntry->wTrackCount;i++) {
        
        sprintf(lpSectionName,"%s%d_%s%d",SECTIONNAME_SOUND,dwSoundIndex,SECTIONNAME_TRACK,i);       
        sprintf(lpKeyName,"%s",KEYNAME_TRACK_EVENT_COUNT);

        dwValue = GetPrivateProfileInt(lpSectionName,
            lpKeyName,
            -1,
            (const char*)m_szIniFileName);  
        
        if (dwValue == -1) {

            PrintParsingError(lpSectionName,
                lpKeyName,
                "ERROR: Key not found.");             
            err = -1;
            goto errorExit;

        }

		m_pSoundTable[dwSoundIndex].m_pTrackTable[i].SetTrackEntry(&m_pSoundTable[dwSoundIndex].m_pXACTTrackTable[i]);

        m_pSoundTable[dwSoundIndex].m_pTrackTable[i].m_pTrackEntry->wEventEntryCount = (WORD)dwValue;        

        err = ParseTrackEvents(dwSoundIndex, i);
        if (err != ERROR_SUCCESS) {

            goto errorExit;

        }
        
    }




errorExit:
    if (lpSectionName)
        delete [] lpSectionName;

    if (lpKeyName)
        delete [] lpKeyName;

    if (lpResult)
        delete [] lpResult;


    return err;

}

int CXACTFileGenerator::ParseTrackEvents(DWORD dwSoundIndex, DWORD dwTrackIndex)
{

    int err = ERROR_SUCCESS;
    DWORD dwValue = 0;
    DWORD i=0;

    LPCTSTR lpDefault = "DefaultString";
    LPSTR lpSectionName;


    CXACTTrackData *pTrackEntry = &m_pSoundTable[dwSoundIndex].m_pTrackTable[dwTrackIndex]; 
    PXACT_TRACK_EVENT pEventTable, pEvent;

    lpSectionName = new CHAR[SECTION_MAX_SIZE];
    if (lpSectionName == NULL) {
        
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    //
    // allocate a worst case array large enough to hold all track events
    //

    dwValue = sizeof(XACT_TRACK_EVENT)*pTrackEntry->m_pTrackEntry->wEventEntryCount;
    pEventTable = (PXACT_TRACK_EVENT) new BYTE[dwValue];

    if (pEventTable == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    memset(pEventTable,0,dwValue);

	//
	// store event table pointer in track entry
	//

	pTrackEntry->m_pEventBuffer = pEventTable;

    //
    // read all events, one at a time
    //

	pEvent = &pEventTable[0];
    for (i=0;i<pTrackEntry->m_pTrackEntry->wEventEntryCount;i++)
    {

        sprintf(lpSectionName,"%s%d_%s%d_%s%d",SECTIONNAME_SOUND,dwSoundIndex,
            SECTIONNAME_TRACK, dwTrackIndex,
            SECTIONNAME_TRACK_EVENT,i);

        //
        // parse indivdual event entries        
        //

        err = ParseEvent(lpSectionName, dwSoundIndex, pEvent);
        if (err != ERROR_SUCCESS) {
            goto errorExit;
        }

        //
        // add event size to total data size
        //

		pTrackEntry->m_dwEventDataSize += (pEvent->Header.wSize+sizeof(XACT_TRACK_EVENT_HEADER));

		pEvent = (PXACT_TRACK_EVENT) ((PUCHAR)&pEventTable[0] + pTrackEntry->m_dwEventDataSize);

    }
   
errorExit:
    if (lpSectionName)
        delete [] lpSectionName;

    return err;
}
    

int CXACTFileGenerator::ParseEvent(LPSTR lpSectionName,
                                   DWORD dwSoundIndex,
                                   PXACT_TRACK_EVENT pEvent)
{

    DWORD dwValue = 0;
    DWORD dwBytesRead;

    BOOL bGetWaveData = FALSE;
    int err = ERROR_SUCCESS;

    PCHAR lpResult = NULL;    
    LPCTSTR lpDefault = "DefaultString";
    LPSTR lpKeyName = NULL;
    CHAR szName[64];

    lpResult = new CHAR[SECTION_MAX_SIZE];
    if (lpResult == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpKeyName = new CHAR[SECTION_MAX_SIZE];
    if (lpKeyName == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    //
    // some effects just have a single generic value associated
    // with them. read this here
    //

    // **************************************************
    // event type 
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_EVENT_TYPE);

    dwValue = -1;
    dwBytesRead = GetPrivateProfileString(lpSectionName,
        lpKeyName,
        lpDefault,
        szName,
        XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH,
        (const char*)m_szIniFileName);
    
    if (ERROR_SUCCESS == ErrorCheckOnParser(szName,
        (char*)lpDefault,
        dwBytesRead,
        XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH)) {

        for (DWORD i=0;i<NUM_EVENT_TYPES;i++)
        {

            if (!strcmp(g_aszEventNames[i],szName)){

                //
                // found a supported event type
                //

                dwValue = i;
                break;
            }

        }


    } 
    
    if (dwValue == -1){

        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found or event type not supported.");             
        err = -1;
        goto errorExit;
        
    }
    
    pEvent->Header.wType = (WORD)dwValue;

    // **************************************************
    // event timestamp
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_EVENT_TIMESTAMP);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Key not found.");             
        err = -1;
        goto errorExit;
        
    }
    
    pEvent->Header.lSampleTime = dwValue;

    // **************************************************
    // Data Value
    // **************************************************
    
    sprintf(lpKeyName,"%s",KEYNAME_EVENT_VALUE);
    
    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szIniFileName);  

    if(NUM_EVENT_TYPES != eXACTEvent_Max) {
        Print("Executable and xact.h header do not match");
        err = -1;
        goto errorExit;
    }
    
    if (dwValue == -1)
        dwValue = 0;

    sprintf(lpKeyName,"%s",KEYNAME_EVENT_TYPE);

    switch (pEvent->Header.wType) {
    case eXACTEvent_PlayWithPitchAndVolumeVariation:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_PLAY_WITH_PITCH_VOLUME_VAR);
        bGetWaveData = TRUE;
        break;
    case eXACTEvent_Play:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_PLAY);
        bGetWaveData = TRUE;
        break;
    case eXACTEvent_Stop:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_STOP);
        break;
    case eXACTEvent_SetFrequency:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETFREQUENCY);
        pEvent->EventData.SetFrequency.wFrequency = (WORD)dwValue;
        break;
    case eXACTEvent_SetVolume:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETVOLUME);
        pEvent->EventData.SetVolume.sVolume = (SHORT)dwValue;
        break;
    case eXACTEvent_SetHeadroom:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETHEADROOM);
        pEvent->EventData.SetHeadroom.wHeadroom = (WORD)dwValue;
        break;
    case eXACTEvent_SetLFO:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETLFO);
        pEvent->EventData.SetLFO.Desc.dwLFO = DSLFO_PITCH;
        pEvent->EventData.SetLFO.Desc.dwDelay = 10;
        pEvent->EventData.SetLFO.Desc.dwDelta = dwValue;
        pEvent->EventData.SetLFO.Desc.lPitchModulation = 0x1f;
        break;
    case eXACTEvent_SetEG:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETEG);
        pEvent->EventData.SetEG.Desc.dwEG = DSEG_AMPLITUDE;
        pEvent->EventData.SetEG.Desc.dwMode = DSEG_MODE_DELAY;
        pEvent->EventData.SetEG.Desc.dwDelay = 1;
        pEvent->EventData.SetEG.Desc.dwAttack = 100;
        pEvent->EventData.SetEG.Desc.dwHold = 0xa00;
        pEvent->EventData.SetEG.Desc.dwDecay = 100;
        pEvent->EventData.SetEG.Desc.dwSustain = 200;
        pEvent->EventData.SetEG.Desc.dwRelease = 1;
        break;
    case eXACTEvent_SetFilter:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETFILTER);
        pEvent->EventData.SetFilter.Desc.dwMode = DSFILTER_MODE_PARAMEQ;
        pEvent->EventData.SetFilter.Desc.dwQCoefficient = DSFILTER_MODE_PARAMEQ;

        //
        // set the filter for Fc=701Hz, Gain = 20dB, Q = 1
        //

        pEvent->EventData.SetFilter.Desc.adwCoefficients[0] = 0xbe1d;
        pEvent->EventData.SetFilter.Desc.adwCoefficients[1] = 0x8000;
        pEvent->EventData.SetFilter.Desc.adwCoefficients[2] = 0xc8dc;
        pEvent->EventData.SetFilter.Desc.adwCoefficients[3] = 0x1fff;
        break;
    case eXACTEvent_LoopStart:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_LOOPSTART);
        pEvent->EventData.LoopStart.wLoopCount = (WORD)dwValue;
        break;

    case eXACTEvent_LoopEnd:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_LOOPEND);
        break;

    case eXACTEvent_SetMixBinVolumes:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETMIXBINVOLUMES);
        break;

    case eXACTEvent_SetEffectData:
        pEvent->Header.wSize = sizeof(XACT_TRACK_EVENT_SETEFFECTDATA);
        
        //
        // assume one dword of data
        //
        
        pEvent->Header.wSize += sizeof(DWORD);

        //
        // safest thing to do is assume a mono effect then setting its
        // delay line to a small value
        //

        pEvent->EventData.SetEffectData.wEffectIndex = 3;
        pEvent->EventData.SetEffectData.wOffset = sizeof(DWORD); //dwScratchSize
        pEvent->EventData.SetEffectData.wDataSize = sizeof(DWORD);
        pEvent->EventData.SetEffectData.dwData[1] = dwValue;

        break;        
        
    default:
        PrintParsingError(lpSectionName,
            lpKeyName,
            "ERROR: Invalid event type specified");             
        err = -1;
        goto errorExit;
        break;
    }

    if (bGetWaveData) {
        
        // **************************************************
        // wavebank index
        // **************************************************
        
        sprintf(lpKeyName,"%s",KEYNAME_WAVEBANK_TABLE_WAVEBANK_NAME);
        
        dwBytesRead = GetPrivateProfileString(lpSectionName,
            lpKeyName,
            lpDefault,
            szName,
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH,
            (const char*)m_szIniFileName);
        
        if (!ErrorCheckOnParser(szName,
            (char*)lpDefault,
            dwBytesRead,
            XACT_SOUNDBANK_CUE_FRIENDLYNAME_LENGTH)) {
            
            //
            // get wavebank index
            //
            
            dwValue = m_pSoundTable[dwSoundIndex].GetWaveBankTableIndex(szName);
            if (dwValue != -1){
                pEvent->EventData.Play.PlayDesc.WaveSource.wWaveIndex = (USHORT)dwValue;
            } else {
                PrintParsingError(lpSectionName,
                    lpKeyName,
                    "ERROR: Wavebank (%s) referenced not found in sound wavebank table",
                    szName);
                
                err = -1;
                goto errorExit;
            }
            
        }
        
        // **************************************************
        // wave index
        // **************************************************
        
        sprintf(lpKeyName,"%s",KEYNAME_EVENT_WAVE_INDEX);
        
        dwValue = GetPrivateProfileInt(lpSectionName,
            lpKeyName,
            -1,
            (const char*)m_szIniFileName);  
        
        if (dwValue != -1) {
            
            pEvent->EventData.Play.PlayDesc.WaveSource.wWaveIndex = (USHORT) dwValue;
            
        }
        
    }

errorExit:

    if (lpResult)
        delete [] lpResult;

    if (lpKeyName)
        delete [] lpKeyName;

    return err;
}
