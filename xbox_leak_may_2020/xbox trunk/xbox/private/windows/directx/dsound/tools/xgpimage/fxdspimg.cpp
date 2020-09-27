//
// fxdspimg.cpp 
// Parses and validates multiple DSP Audio FX chain specifications. It
// then creates a single DSP image, fixing jump points and pointers in DSP code,
// linking fx dsp routines for sequential processing.
// Does all resource allocation and fixup, providing offset maps of the final image
// to allow games to change FX parameters at runtime              
//
// Created      Author
// -------------------------------------------------------
// 3/15/2001    georgioc
//

#include "stdafx.h"
#include "..\xgpimage\fxdspimg.h"

//
// the order of the mixbins below is very important.
// it must stay in sync with the XBOX dsound mcpx driver and the mixbin
// definitions in dsound.h
//

char* vpMixBinNames[32] ={

    "VPMIXBIN_FRONTLEFT",
    "VPMIXBIN_FRONTRIGHT",
    "VPMIXBIN_CENTER",
    "VPMIXBIN_LFE",
    "VPMIXBIN_BACKLEFT",
    "VPMIXBIN_BACKRIGHT",
    "VPMIXBIN_XTLK_FL",
    "VPMIXBIN_XTLK_FR",
    "VPMIXBIN_XTLK_BL",
    "VPMIXBIN_XTLK_BR",
    "VPMIXBIN_I3DL2SEND",
    "VPMIXBIN_FXSEND0",
    "VPMIXBIN_FXSEND1",
    "VPMIXBIN_FXSEND2",
    "VPMIXBIN_FXSEND3",
    "VPMIXBIN_FXSEND4",
    "VPMIXBIN_FXSEND5",
    "VPMIXBIN_FXSEND6",
    "VPMIXBIN_FXSEND7",
    "VPMIXBIN_FXSEND8",
    "VPMIXBIN_FXSEND9",
    "VPMIXBIN_FXSEND10",
    "VPMIXBIN_FXSEND11",
    "VPMIXBIN_FXSEND12",
    "VPMIXBIN_FXSEND13",
    "VPMIXBIN_FXSEND14",
    "VPMIXBIN_FXSEND15",
    "VPMIXBIN_FXSEND16",
    "VPMIXBIN_FXSEND17",
    "VPMIXBIN_FXSEND18",
    "VPMIXBIN_FXSEND19",
    "VPMIXBIN_RESERVED"
};


char* gpMixBinNames[32] ={

    "GPMIXBIN_FRONTLEFT",
    "GPMIXBIN_FRONTRIGHT",
    "GPMIXBIN_CENTER",
    "GPMIXBIN_LFE",
    "GPMIXBIN_BACKLEFT",
    "GPMIXBIN_BACKRIGHT",
    "GPMIXBIN_XTLK_FL",
    "GPMIXBIN_XTLK_FR",
    "GPMIXBIN_XTLK_BL",
    "GPMIXBIN_XTLK_BR",
    "GPMIXBIN_I3DL2SEND",
    "GPMIXBIN_FXSEND0",
    "GPMIXBIN_FXSEND1",
    "GPMIXBIN_FXSEND2",
    "GPMIXBIN_FXSEND3",
    "GPMIXBIN_FXSEND4",
    "GPMIXBIN_FXSEND5",
    "GPMIXBIN_FXSEND6",
    "GPMIXBIN_FXSEND7",
    "GPMIXBIN_FXSEND8",
    "GPMIXBIN_FXSEND9",
    "GPMIXBIN_FXSEND10",
    "GPMIXBIN_FXSEND11",
    "GPMIXBIN_FXSEND12",
    "GPMIXBIN_FXSEND13",
    "GPMIXBIN_FXSEND14",
    "GPMIXBIN_FXSEND15",
    "GPMIXBIN_FXSEND16",
    "GPMIXBIN_FXSEND17",
    "GPMIXBIN_FXSEND18",
    "GPMIXBIN_FXSEND19",
    "GPMIXBIN_RESERVED"
};


CDspImageBuilder::CDspImageBuilder(char* AppName)
{
    memcpy(m_szAppName,AppName,sizeof(m_szAppName));

    memset(m_aTempBins,0,sizeof(m_aTempBins));
    m_hInputFile = NULL;
    m_hOutputFile = NULL;
    m_dwGraphCount = 0;
    m_dwDebugLevel = DBGLVL_WARN;

    m_dwTotalScratchSize = 0;
    m_dwTotalDspCodeSize = sizeof(DWORD); //add a word for the RTS we add;
    m_dwTotalDspStateSize = 0;
    m_dwTotalDspCyclesUsed = 0;
    m_dwTotalDspYMemSize = 0;

    m_dwTotalFX = 0;

    m_dwNumTempBins = 0;

    // Clear the path names
    SetIniPath(NULL);
    SetDspCodePath(NULL);
}

CDspImageBuilder::~CDspImageBuilder()
{
    if (m_hInputFile != NULL) {
        CloseHandle(m_hInputFile);
    }

    if (m_hOutputFile != NULL) {
        CloseHandle(m_hOutputFile);
    }

    for (ULONG i=0;i<m_dwGraphCount;i++){

        if (m_pGraphs[i]) {
            delete m_pGraphs[i];
        }

    }
}


int CDspImageBuilder::ParseCommandLine(int argc, char **argv)
{

    int err = ERROR_SUCCESS;
	TCHAR	buffer[1024];
	HKEY	hKey;
	DWORD	type;
	DWORD	size;

    if (argc < 3) {

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
        sizeof(m_szDspDataFileName),
        m_szDspDataFileName,        
        &pBuffer)== 0){


        err = GetLastError();
        Print("\n Failed to find the dsp image INI file.Error 0x%x\n", err);
        return err;

    }

    printf("\nInput File to parse: %s\n", m_szDspDataFileName);

    PVOID lpvEnvBlock = GetEnvironmentStrings();

    // If the paths have not been specified through the api
    // retrieve any enviroment variables
    // they are optional and if they are not present the author of
    // INI file must provide fully qualified paths for the state ini
    // and the code files
    if (strlen(m_szIniPath) == 0)
        GetEnvironmentVariable(ENV_VAR_INI_PATH, m_szIniPath, sizeof(m_szIniPath));

    if (strlen(m_szDspCodePath) == 0)
        GetEnvironmentVariable(ENV_VAR_DSP_CODE_PATH, m_szDspCodePath, sizeof(m_szDspCodePath));

	// Then try XDK install path
    if (strlen(m_szIniPath) == 0)
	{
       if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\XboxSDK\\", 0, KEY_QUERY_VALUE | KEY_SET_VALUE,
                        &hKey) == ERROR_SUCCESS)
		{
           size = sizeof(buffer);
           if(RegQueryValueEx(hKey,"InstallPath", NULL, 
               &type,(unsigned char*)buffer,
               &size) == ERROR_SUCCESS)
           {
               memcpy(m_szIniPath,buffer,sizeof(m_szIniPath));
               strcat(m_szIniPath,"\\source\\dsound\\dsp\\ini");
           }
           RegCloseKey(hKey);
		}
	}

    if (strlen(m_szDspCodePath) == 0)
	{
       if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\XboxSDK\\", 0, KEY_QUERY_VALUE | KEY_SET_VALUE,
                        &hKey) == ERROR_SUCCESS)
		{
           size = sizeof(buffer);
           if(RegQueryValueEx(hKey,"InstallPath", NULL, 
               &type,(unsigned char*)buffer,
               &size) == ERROR_SUCCESS)
           {
               memcpy(m_szDspCodePath,buffer,sizeof(m_szDspCodePath));
               strcat(m_szDspCodePath,"\\source\\dsound\\dsp\\bin");
           }
			RegCloseKey(hKey);
		}
	}

    memcpy(m_szCHeaderFileName,argv[3],sizeof(m_szCHeaderFileName));
    printf("Header File to generate: %s\n", m_szCHeaderFileName);

    memcpy(m_szScratchImageFileName,argv[2],sizeof(m_szScratchImageFileName));
    printf("Output File: %s\n\n", m_szScratchImageFileName);
    
    m_hInputFile = CreateFile(
        (const char *)m_szDspDataFileName,
        GENERIC_READ,
        0,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL);

    if (m_hInputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        Print("\n Failed to open the dsp image file.Error 0x%x\n", err);
        return err;

    }

    //
    // we dont need the input file anymore
    //

    CloseHandle(m_hInputFile);
    m_hInputFile = NULL;

    return ERROR_SUCCESS;

}


void CDspImageBuilder::UsageFailure(int err)
{
    if (err != ERROR_SUCCESS) {
        Print("\n Error: 0x%x\n", err);
    }

    Print("\n Usage: %s [FX description INI filename] [scratch image output filename] [generated C header]\n",
          m_szAppName);

}


int CDspImageBuilder::ErrorCheckOnParser(PCHAR pResult, PCHAR pDefault,DWORD dwBytesRead, DWORD dwSize)
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

    }

    if (!strcmp(pResult,pDefault)) {


        return -1;

    }


    return ERROR_SUCCESS;

}

void CDspImageBuilder::DebugPrint(DWORD dwLevel, PCHAR pszFormat, ...)
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


void CDspImageBuilder::PrintParsingError(PCHAR pSection, PCHAR pKey, PCHAR pszFormat, ...)
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

void CDspImageBuilder::PrintValidationError(DWORD dwDebugLevel, DWORD dwGraphIndex, DWORD dwFxIndex, PCHAR pszFormat, ...)
{
    CHAR                    szString[0x400];
    va_list                 va;

    va_start(va, pszFormat);
    vsprintf(szString, pszFormat, va);
    va_end(va);

    if (dwDebugLevel == DBGLVL_ERROR) {

        Print("*** Validating FX graphs : Error in GraphIndex %d, FxIndex %d.\n"\
            "    %s\n",
            dwGraphIndex,dwFxIndex,szString);

    } else {

        Print("*** Validating FX graphs : Warning in GraphIndex %d, FxIndex %d.\n"\
            "    %s\n",
            dwGraphIndex,dwFxIndex,szString);

    }


}

#define SECTION_MAX_SIZE 4096

int CDspImageBuilder::ParseInputFile()
{
    int err = ERROR_SUCCESS;
    DWORD dwBytesRead;
    DWORD dwValue = 0;
    DWORD i=0;

    LPCTSTR lpDefault = "DefaultString";
    LPSTR lpSectionName;
    LPSTR lpKeyName;
    PCHAR lpResult;

    CHAR szGraphName[256];

    CFxData *pFxData;
    CFxGraph *pGraph;

    lpResult = new CHAR[SECTION_MAX_SIZE];
    if (lpResult == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpSectionName = new CHAR[MAX_FX_NAME];
    if (lpSectionName == NULL) {
        
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    lpKeyName = new CHAR[MAX_FX_NAME];
    if (lpKeyName == NULL) {
        err = ERROR_OUTOFMEMORY;
        goto errorExit;
    }

    sprintf(lpSectionName,"%s",MAIN_SECTION_NAME);       

    //
    // see if the main section exists
    //
    
    dwBytesRead = GetPrivateProfileSection(lpSectionName,
        lpResult,
        SECTION_MAX_SIZE,
        (const char*)m_szDspDataFileName);
    
    if (!dwBytesRead) {
        
        PrintParsingError(lpSectionName,
            NULL,
            "FATAL: main section not found!");              
        
        return -1;
        
    }

    // **************************************************
    // read a friendlyname for this image
    // **************************************************

    sprintf(lpSectionName,"%s",MAIN_SECTION_NAME);       
    sprintf(lpKeyName,"%s",IMAGE_FRIENDLY_NAME);
    dwBytesRead = GetPrivateProfileString(lpSectionName,
        lpKeyName,
        lpDefault,
        (char *)m_szImageFriendlyName,
        MAX_FX_NAME,
        (const char*)m_szDspDataFileName);
    
    if (ErrorCheckOnParser((char *)m_szImageFriendlyName,(char*)lpDefault,dwBytesRead,MAX_FX_NAME)) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "WARNING: Key not found.");             

        sprintf(m_szImageFriendlyName,"DEFAULT");
        
    }

    // **************************************************
    // read max number of temporary bins used for routing
    // **************************************************

    sprintf(lpSectionName,"%s",MAIN_SECTION_NAME);       
    sprintf(lpKeyName,"%s",NUM_TEMPBINS_NAME);

    dwValue = GetPrivateProfileInt(lpSectionName,
        lpKeyName,
        -1,
        (const char*)m_szDspDataFileName);  
    
    if (dwValue == -1) {
        
        PrintParsingError(lpSectionName,
            lpKeyName,
            "WARNING: Key not found.");             

    } else {

        m_dwNumTempBins = dwValue;

    }
    
    //
    // graph enumeration loop
    //

    while (m_dwGraphCount < MAX_GRAPHS) {

        sprintf(lpSectionName,"%s",MAIN_SECTION_NAME);       
        sprintf(lpKeyName,"%s%d",GRAPH_KEY_NAME,m_dwGraphCount);
        dwBytesRead = GetPrivateProfileString(lpSectionName,
            lpKeyName,
            lpDefault,
            szGraphName,
            MAX_FX_NAME,
            (const char*)m_szDspDataFileName);
        
        if (ErrorCheckOnParser(szGraphName,(char*)lpDefault,dwBytesRead,MAX_FX_NAME)) {
            
            if (m_dwGraphCount == 0) {

                PrintParsingError(lpSectionName,
                lpKeyName,
                "ERROR: No Graph keys found!");             
                err = -1;
            
            }

            break;
            
        }

        //
        // Create a new Graph class that will contain a chain of FX
        // Then enter the FX enumeration loop that will find all the FX associated with
        // this graph section
        //

        pGraph= new CFxGraph();
        if (pGraph == NULL) {
            
            return ERROR_OUTOFMEMORY;
            
        }

        //
        // save the graph name
        //

        pGraph->SetGraphName(szGraphName);

        m_pGraphs[m_dwGraphCount++] = pGraph;

        //
        // check that no duplicate graph names have been specified
        //

        for (i=0;i<m_dwGraphCount-1;i++) {
            if (!strcmp(szGraphName,m_pGraphs[i]->GetGraphName())){

                //
                // duplicate name found
                //
                
                PrintParsingError(lpSectionName,
                    lpKeyName,
                    "FATAL: Duplicate Graph Names not allowed.");               
                return -1;

            }
        }

        while (pGraph->GetNumEffects() < MAX_FX_PER_GRAPH) {

            sprintf(lpSectionName,"%s",szGraphName);       
            sprintf(lpKeyName,"%s%d",FX_KEY_NAME,pGraph->GetNumEffects());
            dwBytesRead = GetPrivateProfileString(lpSectionName,
                lpKeyName,
                lpDefault,
                lpResult,
                MAX_FX_NAME,
                (const char*)m_szDspDataFileName);

            if (ErrorCheckOnParser(lpResult,(char*)lpDefault,dwBytesRead,MAX_FX_NAME)) {

                break;

            }

            //
            // we found a section for an effect in a graph.
            // now read the effect parameters from the FX section
            //

            pFxData = new CFxData();
            if (pFxData == NULL) {

                err = ERROR_OUTOFMEMORY;
                goto errorExit;

            }

            //
            // Add fx to graph. This will increment the total number of FX in
            // this graph instance
            //

            pGraph->SetFx(pFxData);

            // ****************************************************************
            // create FX subsection name
            // ****************************************************************

            sprintf(lpSectionName,"%s_FX%d_%s",
                szGraphName,
                pGraph->GetNumEffects()-1,
                lpResult);

            //
            // save the friendly FX name.
            //

            memcpy(pFxData->m_szFriendlyEffectName,lpResult,sizeof(pFxData->m_szFriendlyEffectName));

            dwBytesRead = GetPrivateProfileSection(lpSectionName,
                lpResult,
                SECTION_MAX_SIZE,
                (const char*)m_szDspDataFileName);
            
            if (!dwBytesRead) {
                
                PrintParsingError(lpSectionName,
                    lpKeyName,
                    "FATAL: FX section not found!");                
                
                return -1;
                
            }

            //
            // save the FX name.
            //

            memcpy(pFxData->m_szEffectName,lpSectionName,sizeof(pFxData->m_szEffectName));

            //
            // check real quick that no other FX in this graph has used the same name
            //

            for (i=0;i<pGraph->GetNumEffects()-1;i++) {

                CFxData *pFx = pGraph->GetFxData(i);
                if (!strcmp(pFx->m_szFriendlyEffectName,pFxData->m_szFriendlyEffectName)) {

                    //
                    // duplicate name found
                    //

                    PrintParsingError(lpSectionName,
                    lpKeyName,
                    "FATAL: Duplicate FX Names not allowed within a graph.");               
                    return -1;
                }
            }

            //
            // read the FX Dsp code filename
            //

            sprintf(lpKeyName,"%s", FXPARAM_DSPCODE_NAME);
            dwBytesRead = GetPrivateProfileString(lpSectionName,
                lpKeyName,
                lpDefault,
                lpResult,
                MAX_FX_NAME,
                (const char*)m_szDspDataFileName);
            
            if (ErrorCheckOnParser(lpResult,(char*)lpDefault,dwBytesRead,MAX_FX_NAME)) {
                
                PrintParsingError(lpSectionName,
                    lpKeyName,
                    "FATAL: FX dsp code filename not found");
                
                err = -1;
                goto errorExit;
                
            }
            
            //
            // if there is an environment variable for dsp code path
            // use it and assume a non qualified name was passed in the INI
            //

            if (m_szDspCodePath[0] !=0 ) {

                sprintf(pFxData->m_szDspCodeFileName,
                    "%s\\%s",
                    m_szDspCodePath,
                    lpResult);
            } else {

                memcpy(pFxData->m_szDspCodeFileName,
                    lpResult,
                    sizeof(pFxData->m_szDspCodeFileName));

            }

            //
            // read the FX Dsp STATE filename
            //

            sprintf(lpKeyName,"%s", FXPARAM_DSPSTATE_NAME);
            dwBytesRead = GetPrivateProfileString(lpSectionName,
                lpKeyName,
                lpDefault,
                lpResult,
                MAX_FX_NAME,
                (const char*)m_szDspDataFileName);
            
            if (ErrorCheckOnParser(lpResult,(char*)lpDefault,dwBytesRead,MAX_FX_NAME)) {
                
                PrintParsingError(lpSectionName,
                    lpKeyName,
                    "FATAL: FX dsp code filename not found");
                
                err = -1;
                goto errorExit;
                
            }

            //
            // if there was an environment variable specified, assume that the
            // the core INI file had an unqualified name. Make it into a full pathname here
            //
            
            if (m_szIniPath[0] != 0) {
                
                sprintf(pFxData->m_szDspStateFileName,
                    "%s\\%s",
                    m_szIniPath,
                    lpResult);
                
            } else {
                
                memcpy(pFxData->m_szDspStateFileName,lpResult,sizeof(pFxData->m_szDspStateFileName));
                
            }            

            // ****************************************************************
            // read number of inputs from the appropriate FX section under this graph
            // ****************************************************************

            err = ParseInputs(lpSectionName,pFxData);

            if (err != ERROR_SUCCESS) {
                return err;
            }

            // ****************************************************************
            // read number of outputs
            // ****************************************************************

            err = ParseOutputs(lpSectionName, pFxData);

            if (err != ERROR_SUCCESS) {
                return err;
            }

            // ****************************************************************
            // read number of state parameters
            // ****************************************************************
            // NOTE!!! parsing order is important since we must know the number of inputs
            // and number of outputs before trying to allocate a state parameter buffer         

            err = ParseStateParameters(lpSectionName, pFxData);

            if (err != ERROR_SUCCESS) {
                return err;
            }

            // ****************************************************************
            // create parameter descriptions
            // ****************************************************************

            err = ParseParameterDescriptions(pFxData->m_szDspStateFileName, &pFxData->m_pEffectStateParameters);

            if (err != ERROR_SUCCESS) {
                return err;
            }

            // ****************************************************************
            // determine if this FX overwrites its output. This must be done 
            // after parsing state
            // ****************************************************************
            
            sprintf(lpKeyName,"%s",FXFLAG_MIXOUTPUT);
            dwValue = GetPrivateProfileInt(lpSectionName,
                lpKeyName,
                -1,
                (const char*)m_szDspDataFileName);  
            
            if (dwValue == -1) {
                
                PrintParsingError(lpSectionName,
                    lpKeyName,
                    "WARNING: Key not found");              
                
            } else {
            
                //
                // Check : Make sure they pass a valid value for FXPARAM_MIXOUTPUT
                //
                
                if ((dwValue != 0) && (dwValue !=1) ) {
                    
                    PrintParsingError(lpSectionName,
                        lpKeyName,
                        "Invalid value specified for FXPARAM_MIXOUTPUT: Only 0 or 1 is valid");
                    
                    return -1;
                }
                
                ((PDSP_STATE_FIXED)pFxData->m_pDspState)->dwFlags |= (dwValue << BIT_FX_STATE_FLAG_GLOBAL_FX);
            
            }


            //
            // increment count of FX
            //

            m_dwTotalFX++;

        };//FX subloop

    }; //FX graph subloop

    DebugPrint(DBGLVL_ERROR,"Enumerated %d graph(s) and %d effect(s) total",
        m_dwGraphCount,
        m_dwTotalFX);


errorExit:

    if (lpResult) {
        delete [] lpResult;
    }

    if (lpKeyName) {
        delete [] lpKeyName;
    }

    if (lpSectionName) {
        delete [] lpSectionName;
    }

    return err;
}

int CDspImageBuilder::ParseParameterDescriptions(const char *pFileName, PFX_STATE_PARAMETERS *ppStateParams)
{
    int err = ERROR_SUCCESS;
    CHAR szKeyName[256];
    CHAR szSectionName[256];
    CHAR szDefault[256] = "DefaultString";
    CHAR szResult[256];

    DWORD dwValue, dwBytesRead;
    PFX_STATE_PARAMETERS pStateParams; 

    *ppStateParams = NULL;

    //
    // retrieve number of parameters
    //

    sprintf(szKeyName,"%s",FXPARAM_NUMPARAMS_NAME);
    sprintf(szSectionName,"%s",FXSTATE_SECTION_NAME);

    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        pFileName); 
    
    if (dwValue == -1) {
        
        PrintParsingError(szSectionName,
            szKeyName,
            "FATAL: Key not found or file %s not present",
            pFileName
            );              
        
        return -1;
    }
    
    DWORD dwNumParams = dwValue;

    sprintf(szKeyName,"%s",FXPARAM_NUMPUBLICPARAMS_NAME);
    sprintf(szSectionName,"%s",FXSTATE_SECTION_NAME);

    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        pFileName); 
    
    if (dwValue == -1) {
        
        PrintParsingError(szSectionName,
            szKeyName,
            "FATAL: Key not found or file %s not present",
            pFileName
            );              
        
        return -1;
    }
    
    //
    // allocate array that will hold parameter descriptions
    //

    PBYTE pBuffer;
    pBuffer = new BYTE[sizeof(FX_PARAM_DESCRIPTION)*(dwValue-1) + sizeof(FX_STATE_PARAMETERS)];
    if (pBuffer == NULL) {
        return ERROR_OUTOFMEMORY;
    }

    pStateParams = (PFX_STATE_PARAMETERS)pBuffer;
    pStateParams->dwNumParams = dwValue;

    //
    // retrieve Effect name
    //

    sprintf(szKeyName,"%s",FXSTATE_EFFECT_NAME);
    sprintf(szSectionName,"%s",FXSTATE_SECTION_NAME);

    dwBytesRead = GetPrivateProfileString(szSectionName,
        szKeyName,
        szDefault,
        szResult,
        MAX_FX_NAME,
        pFileName);
    
    if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
        
        PrintParsingError(szSectionName,
            szKeyName,
            "FATAL: Parsing %s file. Key not specified correctly or not present", pFileName);
        
        return -1;
        
    }

    memcpy(pStateParams->EffectName,szResult,sizeof(pStateParams->EffectName));

    // ****************************************************************
    // read all parameter values
    // ****************************************************************
    
    DWORD count = 0, dwIndex = 0;
    while (count < dwNumParams) {
        
        //
        // now retrieve information for each parameter..
        //

        //
        // Parameter name
        // if this key does not exist this is not an exposed parameter, thus
        // we ignore it and move on
        //

        sprintf(szKeyName,FXPARAM_DESC_NAME,count);        
        dwBytesRead = GetPrivateProfileString(szSectionName,
            szKeyName,
            szDefault,
            szResult,
            MAX_FX_NAME,
            pFileName);
        
        if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
            
            count++;
            continue;
                        
        }

        memcpy(pStateParams->aParamDescriptions[dwIndex].ParameterName,
            szResult,
            sizeof(pStateParams->aParamDescriptions[dwIndex].ParameterName));

        // Min
        sprintf(szKeyName,FXPARAM_DESC_MIN,count);
        dwValue = GetPrivateProfileInt(szSectionName,
            szKeyName,
            -1,
            pFileName);         
        if (dwValue == -1) {
                        
            break;
        }
        pStateParams->aParamDescriptions[dwIndex].dwParameterMin = dwValue;

        // Max
        sprintf(szKeyName,FXPARAM_DESC_MAX,count);
        dwValue = GetPrivateProfileInt(szSectionName,
            szKeyName,
            -1,
            pFileName); 
        
        if (dwValue == -1) {
                        
            break;
        }
        pStateParams->aParamDescriptions[dwIndex].dwParameterMax = dwValue;

        // default value
        sprintf(szKeyName,"%s%d",FXPARAM_STATEPARAM_NAME,count);
        dwValue = GetPrivateProfileInt(szSectionName,
            szKeyName,
            -1,
            pFileName);    
        
        if (dwValue == -1) {
                        
            break;
        }
        pStateParams->aParamDescriptions[dwIndex].dwDefaultValue = dwValue;

        // parameter type
        sprintf(szKeyName,FXPARAM_DESC_TYPE,count);        
        dwBytesRead = GetPrivateProfileString(szSectionName,
            szKeyName,
            szDefault,
            szResult,
            MAX_FX_NAME,
            pFileName);
        
        if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
            
            PrintParsingError(szSectionName,
                szKeyName,
                "FATAL: Parsing %s file. Key not specified correctly or not present", pFileName);
            
            return -1;
            
        }
        
        if (strstr(szResult,"FLOAT")) {
            pStateParams->aParamDescriptions[dwIndex].dwParameterType = DSFX_PARAM_TYPE_FLOAT;
        } else if(strstr(szResult,"INTEGER")) {
            pStateParams->aParamDescriptions[dwIndex].dwParameterType = DSFX_PARAM_TYPE_INTEGER;
        } else {
            PrintParsingError(szSectionName,
                szKeyName,
                "FATAL: Parsing %s file. Key value not valid. FLOAT or INTEGER are valid only", pFileName);
            return -1;
        }

        // Units name
        sprintf(szKeyName,FXPARAM_DESC_UNITS,count);        
        dwBytesRead = GetPrivateProfileString(szSectionName,
            szKeyName,
            szDefault,
            szResult,
            MAX_FX_NAME,
            pFileName);
        
        if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
            
            PrintParsingError(szSectionName,
                szKeyName,
                "FATAL: Parsing %s file. Key not specified correctly or not present", pFileName);
            
            return -1;
            
        }

        memcpy(pStateParams->aParamDescriptions[dwIndex].UnitsName,
            szResult,
            sizeof(pStateParams->aParamDescriptions[dwIndex].UnitsName));

        // Description
        sprintf(szKeyName,FXPARAM_DESC_DESCRIPTION,count);        
        dwBytesRead = GetPrivateProfileString(szSectionName,
            szKeyName,
            szDefault,
            szResult,
            MAX_FX_NAME,
            pFileName);
        
        if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
            
            //
            // this parameter is optional
            //

            memset(pStateParams->aParamDescriptions[dwIndex].ParameterDesc,
                0,
                sizeof(pStateParams->aParamDescriptions[dwIndex].ParameterDesc));        
            
        } else {

            memcpy(pStateParams->aParamDescriptions[dwIndex].ParameterDesc,
                szResult,
                sizeof(pStateParams->aParamDescriptions[dwIndex].ParameterDesc));        

        }

        // atomic
        sprintf(szKeyName,FXPARAM_DESC_ATOMIC,count);
        dwValue = GetPrivateProfileInt(szSectionName,
            szKeyName,
            -1,
            pFileName);    
        
        if (dwValue == -1) {
                        
            break;
        }
        pStateParams->aParamDescriptions[dwIndex].bIsAtomic = dwValue;

        //
        // keep track of the offset into the state block of this parameter
        //

        pStateParams->aParamDescriptions[dwIndex].dwOffset = count;

        dwIndex++;
        count++;
        
    } // FX state parameter enumeration subloop
    
    //
    // update numner of public parameters we found
    //

    pStateParams->dwNumParams = dwIndex;

    *ppStateParams = pStateParams;
    return err;
}

int CDspImageBuilder::ParseStateParameters(PCHAR pFxName, CFxData *pFxData)
{
    int err = ERROR_SUCCESS;
    CHAR szKeyName[256];
    CHAR szSectionName[256];
    CHAR szDefault[256] = "DefaultString";

    DWORD dwValue;

    //
    // the state parameters are stored in a different file
    // each FX must specify a stateXXX.ini file that contains the
    // the state variable definitions
    //
    
    //
    // retrieve NUMINPUTS key
    //

    sprintf(szKeyName,"%s",FXPARAM_NUMINPUTS_NAME);
    sprintf(szSectionName,"%s",FXSTATE_SECTION_NAME);

    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        (const char*)pFxData->m_szDspStateFileName);    
    
    if (dwValue == -1) {
        
        PrintParsingError(pFxName,
            szKeyName,
            "FATAL: Parsing %s file.Key not found",             
            pFxData->m_szDspStateFileName);

        
        return -1;
    }

    //
    // make sure the numinputs we parsed earlier vs the num inputs
    // the DSP developer said this FX has, is the same
    //

    if (dwValue != pFxData->m_dwNumInputs) {

        PrintParsingError(pFxName,
            szKeyName,
            "FATAL: FX Description has a different number of inputs than FX %s requires",
            pFxData->m_szEffectName
            );              

        return -1;
    }

    //
    // retrieve NUMOUTPUTS key
    //

    sprintf(szKeyName,"%s",FXPARAM_NUMOUTPUTS_NAME);
    sprintf(szSectionName,"%s",FXSTATE_SECTION_NAME);

    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        (const char*)pFxData->m_szDspStateFileName);    
    
    if (dwValue == -1) {
        
        PrintParsingError(pFxName,
            szKeyName,
            "FATAL: Parsing %s file.Key not found",             
            pFxData->m_szDspStateFileName);
        
        return -1;
    }

    //
    // make sure the numinputs we parsed earlier vs the num inputs
    // the DSP developer said this FX has, is the same
    //

    if (dwValue != pFxData->m_dwNumOutputs) {

        PrintParsingError(pFxName,
            szKeyName,
            "FATAL: FX Description has a different number of outputs than FX %s requires",
            pFxData->m_szEffectName
            );              

        return -1;
    }

    //
    // the FX state memory is the size of the parameter words they specifed plus
    // one dsp word for each input/output ID plus extra dwords to describe scratch space usage.
    //

    //
    // retrieve NUMPARAMS key
    //

    sprintf(szKeyName,"%s",FXPARAM_NUMPARAMS_NAME);
    sprintf(szSectionName,"%s",FXSTATE_SECTION_NAME);

    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        (const char*)pFxData->m_szDspStateFileName);    
    
    if (dwValue == -1) {
        
        PrintParsingError(pFxName,
            szKeyName,
            "FATAL: Key not found or file %s not present",
            pFxData->m_szDspStateFileName
            );              
        
        return -1;
    }

    pFxData->m_dwNumParams = dwValue;
    pFxData->m_dwDspStateSize = pFxData->CalcStateSize();
    
    //
    // allocate Dsp state buffer
    //
    
    pFxData->m_pDspState = (DWORD*) new BYTE[pFxData->m_dwDspStateSize];
    if (pFxData->m_pDspState == NULL) {
        return ERROR_OUTOFMEMORY;
    }
    
    memset(pFxData->m_pDspState,0,pFxData->m_dwDspStateSize);

    // ****************************************************************
    // read DSP scratch offset length used by this FX
    // ****************************************************************
    
    sprintf(szKeyName,"%s",FXPARAM_SCRATCH_LENGTH);
    
    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        (const char*)pFxData->m_szDspStateFileName);    
    
    if (dwValue == -1) {
        
        PrintParsingError(szSectionName,
            szKeyName,
            "Key not found");               
        
        return -1;
    }
    
    ((PDSP_STATE_FIXED)pFxData->m_pDspState)->dwScratchLength = dwValue;            

    // ****************************************************************
    // read DSP Y memory size used by this FX
    // ****************************************************************

    sprintf(szKeyName,"%s",FXPARAM_YMEMORY_SIZE);
    
    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        (const char*)pFxData->m_szDspStateFileName);    
    
    if (dwValue == -1) {
        
        PrintParsingError(szSectionName,
            szKeyName,
            "Key not found");               
        
        return -1;
    }

    ((PDSP_STATE_FIXED)pFxData->m_pDspState)->dwYMemLength = dwValue;

    // ****************************************************************
    // read DSP cycles used by this FX
    // ****************************************************************
    
    sprintf(szKeyName,"%s",FXPARAM_DSPCYCLES_NAME);
    
    dwValue = GetPrivateProfileInt(szSectionName,
        szKeyName,
        -1,
        (const char*)pFxData->m_szDspStateFileName);    
    
    if (dwValue == -1) {
        
        PrintParsingError(szSectionName,
            szKeyName,
            "FATAL: Key not found");                
        
        return -1;
    }
    
    pFxData->m_dwDspCyclesUsed = dwValue;   

    // ****************************************************************
    // read all parameter values
    // ****************************************************************
    
    DWORD count = 0;
    while (count < pFxData->m_dwNumParams) {
        
        //
        // we expect keys for each input..
        //
        
        sprintf(szKeyName,"%s%d",FXPARAM_STATEPARAM_NAME,count);

        dwValue = GetPrivateProfileInt(szSectionName,
            szKeyName,
            -1,
            (const char*)pFxData->m_szDspStateFileName);    
        
        if (dwValue == -1) {
                        
            break;
        }
        
        DWORD offset = pFxData->GetParamOffset();
        pFxData->m_pDspState[count+offset] = dwValue;
        
        count++;
        
    } // FX state parameter enumeration subloop
    

    return err;
}

//*****************************************************************************

int CDspImageBuilder::ParseOutputs(PCHAR pFxName, CFxData *pFxData)
{

    CHAR szKeyName[64];
    CHAR szDefault [256] = "DefaultString";
    CHAR szResult[256];

    DWORD dwValue, dwBytesRead;

    //
    // retrieve number of outputs
    //

    sprintf(szKeyName,"%s",FXPARAM_NUMOUTPUTS_NAME);
    
    dwValue = GetPrivateProfileInt(pFxName,
        szKeyName,
        -1,
        (const char*)m_szDspDataFileName);
    
    
    if (dwValue == -1) {
        
        PrintParsingError(pFxName,
            szKeyName,
            "FATAL: Key (%s) not found",
            FXPARAM_NUMINPUTS_NAME);                
        
        return -1;
        
    }
    
    pFxData->m_dwNumOutputs = dwValue;
    
    // ****************************************************************
    // read input names...
    // ****************************************************************
    
    DWORD count = 0;
    while (count < pFxData->m_dwNumOutputs+1) {
        
        //
        // we expect keys for each input..
        //
        
        sprintf(szKeyName,"%s%d",FXPARAM_OUTPUT_NAME,count);
        dwBytesRead = GetPrivateProfileString(pFxName,
            szKeyName,
            szDefault,
            szResult,
            MAX_FX_NAME,
            (const char*)m_szDspDataFileName);
        
        if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
            
            if (count == pFxData->m_dwNumOutputs) {

                //
                // this is not an hour. We read one more input/output than we are
                // supposed to to catch any types by the author
                //

                break;
            } else {

                PrintParsingError(pFxName,
                    szKeyName,
                    "FATAL: Invalid output name found for output %d",
                    count);
                
                return -1;
            }
            
        } else {

            if (count == pFxData->m_dwNumOutputs) {

                PrintParsingError(pFxName,
                    szKeyName,
                    "WARNING: More outputs specified than FX_NUMOUTPUTS",
                    count);

                break;
            }
        } 
        
        pFxData->m_dwOutputIDs[count] = pFxData->OutputNameToOutputID(szResult);
        if (pFxData->m_dwOutputIDs[count] == -1) {
            
            PrintParsingError(pFxName,
                szKeyName,
                "FATAL: Invalid output name specified for output %d",
                count);
            
            return -1;
        }
        
        count++;
        if (count > MAX_FX_OUTPUTS) {

            PrintParsingError(pFxName,
                szKeyName,
                "FATAL: Too many outputs specified. Maximum is %d",
                MAX_FX_OUTPUTS);
            
            return -1;
        }


        
    } // FX output enumeration subloop
    
    return ERROR_SUCCESS;
}

//*****************************************************************************

int CDspImageBuilder::ParseInputs(PCHAR pFxName, CFxData *pFxData)
{

    CHAR szKeyName[64];
    CHAR szDefault[256] = "DefaultString";
    CHAR szResult[256];

    DWORD dwValue, dwBytesRead;

    //
    // retrieve number of inputs
    //

    sprintf(szKeyName,"%s",FXPARAM_NUMINPUTS_NAME);
    
    dwValue = GetPrivateProfileInt(pFxName,
        szKeyName,
        -1,
        (const char*)m_szDspDataFileName);
    
    
    if (dwValue == -1) {
        
        PrintParsingError(pFxName,
            pFxName,
            "FATAL: Key (%s) not found",
            FXPARAM_NUMINPUTS_NAME);                
        
        return -1;
        
    }
    
    pFxData->m_dwNumInputs = dwValue;
    
    // ****************************************************************
    // read input names...
    // ****************************************************************
    
    DWORD count = 0;
    while (count < pFxData->m_dwNumInputs) {
        
        //
        // we expect keys for each input..
        //
        
        sprintf(szKeyName,"%s%d",FXPARAM_INPUT_NAME,count);
        dwBytesRead = GetPrivateProfileString(pFxName,
            szKeyName,
            szDefault,
            szResult,
            MAX_FX_NAME,
            (const char*)m_szDspDataFileName);
        
        if (ErrorCheckOnParser(szResult,(char*)szDefault,dwBytesRead,MAX_FX_NAME)) {
            
            if (count == pFxData->m_dwNumInputs) {
                
                //
                // this is not an hour. We read one more input/output than we are
                // supposed to to catch any types by the author
                //
                
                break;
            } else {
                
                PrintParsingError(pFxName,
                    szKeyName,
                    "FATAL: Invalid input name found for input %d",
                    count);
                
                return -1;
            }
            
        } else {
            
            if (count == pFxData->m_dwNumInputs) {
                
                PrintParsingError(pFxName,
                    szKeyName,
                    "WARNING: More inputs specified than FX_NUMOUTPUTS",
                    count);
                
                break;
            }
        } 
        
        pFxData->m_dwInputIDs[count] = pFxData->InputNameToInputID(szResult);
        if (pFxData->m_dwInputIDs[count] == -1) {
            
            PrintParsingError(pFxName,
                szKeyName,
                "FATAL: Invalid input name specified for input %d",
                count);
            
            return -1;
        }
        
        count++;

        if (count > MAX_FX_INPUTS) {

            PrintParsingError(pFxName,
                szKeyName,
                "FATAL: Too many inputs specified. Maximum is %d",
                MAX_FX_INPUTS);
            
            return -1;
        }

        
    } // FX input enumeration subloop

    return ERROR_SUCCESS;
}

DWORD CFxData::OutputNameToOutputID(PCHAR pName)
{

    DWORD dwId = -1,i;
    CHAR szName[64];

    //
    // first check if the output specified is a mix bin
    //

    for (i=0;i<NUM_GPMIXBINS;i++) {

        if (!strcmp(gpMixBinNames[i],pName) && 
            !strstr(gpMixBinNames[i],"RESERVED")) {

            dwId = FXOUTPUT_GPMIXBIN_BASE;

            //
            // place the actuall GP mix bin memory offset here
            //

            dwId |= (i * DSP_BUFFER_FRAME_LENGTH) + DSP_GPMIXBIN_MEMOFFSET_BASE;
            return dwId;
        }

    }

    //
    // see if they specified internal temp bins
    //

    for (i=0;i<MAX_TEMPBINS;i++) {

        sprintf(szName,"%s%d",FXOUTPUT_GPTEMPBIN_NAME,i);
        if (!strcmp(szName,pName)) {

            dwId = FXOUTPUT_GPTEMPBIN_BASE;

            //
            // OR the bin number
            //

            dwId |= i;
            return dwId;
        }

    }

    return dwId;
}

DWORD CFxData::InputNameToInputID(PCHAR pName)
{

    DWORD dwId = -1,i;
    CHAR szName[64];

    //
    // first check if the input specified is a VP mix bin
    //

    for (i=0;i<NUM_VPMIXBINS;i++) {

        if (!strcmp(vpMixBinNames[i],pName) &&
            !strstr(vpMixBinNames[i],"RESERVED")) {

            dwId = FXINPUT_VPMIXBIN_BASE;
            dwId |= (i * DSP_BUFFER_FRAME_LENGTH) + DSP_VPMIXBIN_MEMOFFSET_BASE;

            return dwId;
        }

    }

    //
    // check if the input specified is a GP mix bin
    //

    for (i=0;i<NUM_GPMIXBINS;i++) {

        if (!strcmp(gpMixBinNames[i],pName)) {

            dwId = FXINPUT_GPMIXBIN_BASE;

            //
            // place the actuall GP mix bin memory offset here
            //

            dwId |= (i * DSP_BUFFER_FRAME_LENGTH) + DSP_GPMIXBIN_MEMOFFSET_BASE;

            return dwId;
        }

    }

    //
    // see if they specified internal temp bins
    //

    for (i=0;i<MAX_TEMPBINS;i++) {

        sprintf(szName,"%s%d",FXINPUT_GPTEMPBIN_NAME,i);
        if (!strcmp(szName,pName)) {

            dwId = FXINPUT_GPTEMPBIN_BASE;

            //
            // OR the bin number
            //

            dwId |= i;
            return dwId;
        }

    }


    return dwId;
}

DWORD CFxData::ParamNameToParamID(PCHAR pName)
{
#if 0
    DWORD dwId = -1,i;

    CHAR szName[64];
    PCHAR pszOffset;

    //
    // check if just a raw value is specified
    //
        
    sprintf(szName,"%s",FXPARAM_VALUE_NAME);
    pszOffset = strstr(pName,szName);
    
    if (pszOffset){
                
        //
        // extract the offset number from the definition
        //
        
        while (*pszOffset != '_') {
            pszOffset++;
            if (*pszOffset == 0) {
                
                return -1;
                
            }
        }
                
        i = atoi(pszOffset + 1);
        
        dwId = i | FXPARAM_VALUE_BASE;
        return dwId;
        
    }
#endif  
    return -1;
}



int CDspImageBuilder::ValidateFxGraphs()
{

    int err = ERROR_SUCCESS;
    DWORD dwFxIndex,dwGraphIndex, count;
    DWORD dwValue;
            
    //
    // for each graph, check each FX->FX connection for validity.
    //

    for (dwGraphIndex=0;dwGraphIndex<m_dwGraphCount;dwGraphIndex++){


        for (dwFxIndex=0;dwFxIndex<m_pGraphs[dwGraphIndex]->GetNumEffects();dwFxIndex++){

            CFxGraph *pGraph = m_pGraphs[dwGraphIndex];
            CFxData *pPreviousFx = NULL, *pCurrentFx = NULL, *pNextFx = NULL;

            pCurrentFx = pGraph->GetFxData(dwFxIndex);

            //
            // check : Presense of dsp code file
            //

            if (pCurrentFx->m_szDspCodeFileName[0] == 0) {

                PrintValidationError(DBGLVL_ERROR,dwGraphIndex,dwFxIndex,"Dsp code filename not specified");
                return -1;

            } else {

                //
                // read dsp code, convert to an array of dwords.
                //

                err = ReadDspCodeFile(pCurrentFx);
                if (err != ERROR_SUCCESS) {
                    return err;
                }
                
                PDWORD opcode = (PDWORD)pCurrentFx->m_pDspCode;
                
                //
                // verify its a 
                // move #>(CONSTANT),r5 
                // opcode
                //
                
                if (*opcode != DSP_563000_OPCODE_MOVE_X_TO_R5_IMM) {
                
                    PrintValidationError(DBGLVL_ERROR,dwGraphIndex,dwFxIndex,"First opcode of DSP FX must be *** move #>$40,r5 ***");
                    return -1;
                    
                }
                
                
            }

            //
            // check : Number of inputs
            //

            if (pCurrentFx->m_dwNumInputs > MAX_FX_INPUTS){

                PrintValidationError(DBGLVL_ERROR,dwGraphIndex,dwFxIndex,"Invalid number of Inputs");
                return -1;

            }

            //
            // check : Number of outputs
            //

            if (pCurrentFx->m_dwNumOutputs > MAX_FX_OUTPUTS){

                PrintValidationError(DBGLVL_ERROR,dwGraphIndex,dwFxIndex,"Invalid number of Outputs");
                return -1;

            }

            //
            // check : Validate Input IDs and connections
            //

            for (count=0;count<pCurrentFx->m_dwNumInputs;count++) {

                if (pCurrentFx->m_dwInputIDs[count] == 0) {

                    PrintValidationError(DBGLVL_ERROR,
                        dwGraphIndex,
                        dwFxIndex,
                        "No input source specified for input %d",
                        count);
                    
                    return -1;

                }

                //
                // if they specified a temp bin explicitly make sure its not allocated
                // or exceeds the number of bins they specified up front
                //

                if (pCurrentFx->m_dwInputIDs[count] & FXINPUT_GPTEMPBIN_BASE){

                    dwValue = pCurrentFx->m_dwInputIDs[count] & ~FXINPUT_GPTEMPBIN_BASE;
                    if (dwValue > m_dwNumTempBins-1) {

                        PrintValidationError(DBGLVL_ERROR,
                            dwGraphIndex,
                            dwFxIndex,
                            "Temp bin index is higher than specified in NUMTEMPBINS");

                        return -1;
                    }

                    if (!m_aTempBins[dwValue]) {

                        //
                        // allocate the bin in our mask
                        //
                        
                        m_aTempBins[dwValue] = TRUE;

                    }


                } //GPTEMPBIN  check
                                    
                            
            } // FX Input FOR loop

            //
            // Check : Output validation
            //

            for (count = 0;count < pCurrentFx->m_dwNumOutputs;count++){

                if (pCurrentFx->m_dwOutputIDs[count] == 0) {
                    
                    PrintValidationError(DBGLVL_ERROR,
                        dwGraphIndex,
                        dwFxIndex,
                        "No output destination specified for output %d",
                        count);
                    
                    return -1;
                    
                }

                //
                // if they specified a temp bin explicitly make sure its not allocated
                // or exceeds the number of bins they specified up front
                //

                if (pCurrentFx->m_dwOutputIDs[count] & FXOUTPUT_GPTEMPBIN_BASE){

                    dwValue = pCurrentFx->m_dwOutputIDs[count] & ~FXOUTPUT_GPTEMPBIN_BASE;
                    if (dwValue > m_dwNumTempBins-1) {

                        PrintValidationError(DBGLVL_ERROR,
                            dwGraphIndex,
                            dwFxIndex,
                            "Temp bin index is higher than specified in NUMTEMPBINS");

                        return -1;
                    }

                    if (!m_aTempBins[dwValue]) {

                        //
                        // allocate the bin in our mask
                        //
                        
                        m_aTempBins[dwValue] = TRUE;

                    }

                }
                                    
            }

            //****************************************************
            // DSP Resource checks
            //****************************************************

            //
            // keep track of the total size of all DSP code we have read so far
            //

            m_dwTotalDspCodeSize += pCurrentFx->m_dwDspCodeSize;
            if (m_dwTotalDspCodeSize > (DSP_PMEMORY_SIZE - DSP_EXECUTION_ENGINE_SIZE)) {

                PrintValidationError(DBGLVL_ERROR,
                    dwGraphIndex,
                    dwFxIndex,
                    " Dsp code space exceed with this FX");

                return -1;
            }

            //
            // keep track of the total size of X+Y memory used so far
            //

            m_dwTotalDspStateSize += pCurrentFx->m_dwDspStateSize;
            if (m_dwTotalDspStateSize > DSP_FX_STATE_MAX_SIZE) {

                PrintValidationError(DBGLVL_ERROR,
                    dwGraphIndex,
                    dwFxIndex,
                    "FATAL ERROR: DSP Local FX state memory space: %d. Total used so far: %d.",
                    DSP_FX_STATE_MAX_SIZE,
                    m_dwTotalDspStateSize);



                return -1;
            }

            //
            // keep track of scratch space usage
            //

            m_dwTotalScratchSize += pCurrentFx->GetScratchSize();

            //
            // keep track of Y memory usage
            //

            m_dwTotalDspYMemSize += pCurrentFx->GetYMemSize();
            if (m_dwTotalDspYMemSize > DSP_YMEMORY_SIZE) {

                PrintValidationError(DBGLVL_ERROR,
                    dwGraphIndex,
                    dwFxIndex,
                    "DSP Y memory space exceed with this FX");

                return -1;
            }

            //
            // track total cycles used
            //

            m_dwTotalDspCyclesUsed += pCurrentFx->m_dwDspCyclesUsed;

            if (m_dwTotalDspCyclesUsed > DSP_MAXCYCLES_AVAILABLE) {

                PrintValidationError(DBGLVL_ERROR,
                    dwGraphIndex,
                    dwFxIndex,
                    "FATAL ERROR: DSP Available processing cycles  = %d. Total including this FX %d",
                    DSP_MAXCYCLES_AVAILABLE,
                    m_dwTotalDspCyclesUsed);


                return -1;
            }

        }

    }


    //
    // count the total temporary bins we utilized. If its less than what the user
    // specified upfront, spew out a warning
    //

    dwValue = 0;
    for (ULONG i=0;i<MAX_TEMPBINS;i++) {

        if (m_aTempBins[i]){
            dwValue++;
        }

    }

    if (dwValue != m_dwNumTempBins) {

        PrintValidationError(DBGLVL_WARN,
            dwGraphIndex,
            dwFxIndex,
            "Number of temporary bins used = %d, Number of Bins specified in MAIN section %d.",
            dwValue,
            m_dwNumTempBins
            );

        //
        // will still use the value they gave us just in case they wanted to reserve space in xram
        //

    }

    //
    // add the right amount of words to XRAM utilization fo the TEMP bins
    //
    
    m_dwTotalDspStateSize += DSP_BUFFER_FRAME_LENGTH*sizeof(DWORD)*m_dwNumTempBins;
    
    //
    // align scratch space used to nearest page
    //

    m_dwTotalScratchSize = ((m_dwTotalScratchSize+PAGE_SIZE-1)/PAGE_SIZE)*PAGE_SIZE;

    return err;

}

int CDspImageBuilder::ReadDspCodeFile(CFxData *pCurrentFx)
{
    int err;
    PTCHAR pszFileName;
    PUCHAR pBuffer,pDst;
    PDWORD pdwSize;

    pszFileName = pCurrentFx->m_szDspCodeFileName;
    pBuffer = (PUCHAR)pCurrentFx->m_pDspCode;
    pdwSize = &pCurrentFx->m_dwDspCodeSize;
    
    //
    // attempt to open and read the dsp code
    //
    
    m_hInputFile = CreateFile(
        (const char *)pszFileName,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    
    if (m_hInputFile == INVALID_HANDLE_VALUE) {
        
        err = GetLastError();
        DebugPrint(DBGLVL_ERROR,"Failed to open the dsp image file %s.Error 0x%x",
            pszFileName,
            err);
        return err;
        
    }
    
    //
    // read DSP code into FX data, determine size of file..
    //
    
    *pdwSize = SetFilePointer(m_hInputFile, 0, NULL, FILE_END);              
    SetFilePointer(m_hInputFile, 0, NULL, FILE_BEGIN);
    
    //
    // the cldtobin tool will append a few zero DWORDs at the end of the
    // of the dsp code. Make sure the dsp code length is dword aligned.
    // if not aligned it to the next lowest dword boundary
    //

    if (pCurrentFx->m_dwDspCodeSize & 0x00000003) {
        
        //
        // not dword aligned.. align it
        //

        pCurrentFx->m_dwDspCodeSize &= 0xFFFFFFFC;

    }

    //
    // allocate memory for the DSP code image
    //
    
    pBuffer = new BYTE[*pdwSize];
    if (pBuffer == NULL) {
        return ERROR_OUTOFMEMORY;
    }

    pDst = new BYTE[*pdwSize];
    if (pDst == NULL) {
        delete [] pBuffer;
        return ERROR_OUTOFMEMORY;
    }

    DWORD dwBytesRead;
    BOOL bResult = ReadFile(m_hInputFile,
        pBuffer,
        *pdwSize,
        &dwBytesRead,
        0);
    
    if (!bResult) {
        
        err = GetLastError();
        DebugPrint(DBGLVL_ERROR,"Failed to read dsp code from file %s.Error 0x%x",
            pszFileName,
            err);

        delete [] pBuffer;
        delete [] pDst;
        return err;
        
    }

    //
    // the file is encrypted. decode
    //

    XAudiopUtility_Decode(pCurrentFx->m_szDevKey,pBuffer,*pdwSize,pDst,TRUE);

    pCurrentFx->m_pDspCode = (PDWORD)pDst;

    //
    // note that encoded file is KEY_SIZE bytes larger
    // then raw dsp opcode binary
    //

    pCurrentFx->m_dwDspCodeSize -= KEY_SIZE;

    CloseHandle(m_hInputFile);
    m_hInputFile = NULL;

    delete [] pBuffer;

    return ERROR_SUCCESS;
}

//------------------------------------------------------------------------------
//  CDspImageBuilder::Print
//------------------------------------------------------------------------------
void
CDspImageBuilder::Print(
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

//------------------------------------------------------------------------------
//  CDspImageBuilder::SetIniPath
//------------------------------------------------------------------------------
void 
CDspImageBuilder::SetIniPath(
                             IN LPCTSTR pPath
                             )
/*++

Routine Description:

    Sets the path for .ini files

Arguments:

    IN pPath -  Path

Return Value:

    None

--*/
{
    if(pPath != NULL)
        strcpy(m_szIniPath, pPath);
    else
        memset(m_szIniPath, 0, sizeof(m_szIniPath));
}

//------------------------------------------------------------------------------
//  CDspImageBuilder::SetDspCodePath
//------------------------------------------------------------------------------
void 
CDspImageBuilder::SetDspCodePath(
                                 IN LPCTSTR pPath
                                 )
/*++

Routine Description:

    Sets the path for the dsp files

Arguments:

    IN pPath -  Path

Return Value:

    None

--*/
{
    if(pPath != NULL)
        strcpy(m_szDspCodePath, pPath);
    else
        memset(m_szDspCodePath, 0, sizeof(m_szDspCodePath));
}
