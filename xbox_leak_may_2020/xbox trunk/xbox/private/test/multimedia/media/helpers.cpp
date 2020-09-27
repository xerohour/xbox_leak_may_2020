/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Helpers.cpp

Abstract:

	Helper functions for logging, etc

Author:

	Daniel Haffner (danhaff) 04/29/01

Revision History:



--*/

//------------------------------------------------------------------------------
//	Includes:
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <xtestlib.h>
#include <xlog.h>
#include <stdio.h>
#include <io.h>
#include <xgraphics.h>
#include <dmusici.h>
#include "media.h"
#include "helpers.h"
#include "macros.h"
#include "DSSTDFX.h"

#ifdef _DEBUG
    extern "C" DWORD g_dwDirectSoundDebugLevel;
    extern "C" DWORD g_dwDirectSoundDebugBreakLevel;
#endif

using namespace Media;

namespace Media {

AUDIOPATHPAIR AudioPathPairs[4] = 
{
{"DMUS_APATH_SHARED_STEREOPLUSREVERB", DMUS_APATH_SHARED_STEREOPLUSREVERB},
{"DMUS_APATH_SHARED_STEREO", DMUS_APATH_SHARED_STEREO},
{"DMUS_APATH_DYNAMIC_MONO", DMUS_APATH_DYNAMIC_MONO},
{"DMUS_APATH_DYNAMIC_3D", DMUS_APATH_DYNAMIC_3D}
};


  
//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT Log
(
    LPSTR                   szFormat,
    ...
)
{
    va_list va;
    char szBuffer[1000];

    va_start(va, szFormat);
    vsprintf(szBuffer, szFormat, va);
    va_end(va);

    OutputDebugStringA(szBuffer);
    return S_OK;
};


//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT RecurseDirectory(LPSTR szDir, LOADPROC LoadFile, LPVOID pVoid)
{
	HRESULT hr = S_OK;
	WIN32_FIND_DATA findData;
	BOOL bMore = TRUE;
	HANDLE hFiles;
	CHAR szFullPath[MAX_PATH];
	CHAR szFiles[MAX_PATH];

	sprintf( szFiles, "%s\\*", szDir ); 

	hFiles = FindFirstFile( szFiles, &findData );

	while ( INVALID_HANDLE_VALUE != hFiles && TRUE == bMore && SUCCEEDED( hr ) )
	{
		sprintf( szFullPath, "%s\\%s", szDir, findData.cFileName );

		if ( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			CHECKRUN( RecurseDirectory( szFullPath, LoadFile, pVoid) );
		}

		else
		{
			CHECKRUN( LoadFile(szFullPath, pVoid) );
		}

		bMore = FindNextFile( hFiles, &findData );
	}

	FindClose( hFiles );

	return hr;
}



//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LPCSTR PathFindFileName(LPCSTR pPath)
{
    LPCSTR pT;

    for (pT = pPath; *pPath; pPath++) {
        if ((pPath[0] == '\\' || pPath[0] == ':' || pPath[0] == '/')
            && pPath[1] &&  pPath[1] != '\\'  &&   pPath[1] != '/')
            pT = pPath + 1;
    }

    return pT;
}


//------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HRESULT ChopPath(const char *szFullString, LPSTR szPathOut, LPSTR szFileOut)
{
LPSTR szFile = NULL;
DWORD dwLength;

if (!szFullString)
{
    szPathOut[0] = NULL;
    szFileOut[0] = NULL;
    return E_POINTER;
}

//Parse out the file name.
szFile = (LPSTR)PathFindFileName(szFullString);
if (!szFile)
    return E_FAIL;

strcpy(szFileOut, szFile);

//Get the length of the directory name.
dwLength = strlen(szFullString) - strlen(szFile);

//Copy only that length, and set the final character of the out-string to NULL.
strncpy(szPathOut, szFullString, dwLength);
szPathOut[dwLength] = NULL;

return S_OK;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT TranslatePath(const char *szFullString, LPSTR szRemotePath, LPSTR szLocalDir )
{
LPSTR szFile = NULL;
DWORD dwLength;

if (!szFullString)
{
    szRemotePath[0] = NULL;
    szLocalDir[0] = NULL;
    return E_POINTER;
}

//Parse out the file name.
szFile = (LPSTR)PathFindFileName(szFullString);
if (!szFile)
    return E_FAIL;

//Get the length of the directory name.
dwLength = strlen(szFullString) - strlen(szFile);

//Copy only that length, and set the final character of the out-string to NULL.
strncpy(szLocalDir, szFullString, dwLength);
szLocalDir[dwLength] = NULL;

//translate local dir into remote dir
strcpy( szRemotePath, "mediastress/" );
LPSTR lpszTemp = strstr( szLocalDir, "t:\\media\\" );
if ( szLocalDir != lpszTemp )
{
	return E_FAIL;
}
int i = 12;
int j = 9;
while ( '\0' != szLocalDir[j] )
{
	if ( MAX_PATH <= i )
	{
		return E_FAIL;
	}
	if ( '\\' == szLocalDir[j] )
	{
		szRemotePath[i] = '/';
	}
	else
	{
		szRemotePath[i] = szLocalDir[j];
	}
	i++; j++;
}
strcpy( &szRemotePath[i], szFile );

if ( NULL == strchr( szFile, '.' ) )
{
	strcat( szLocalDir, szFile );
	strcat( szRemotePath, "/" );
}

return S_OK;
}



TESTPARAMS g_TestParams = {0};

//------------------------------------------------------------------------------
//Print Test Params
//------------------------------------------------------------------------------
HRESULT PrintTestParams(void)
{
    Log("\n");
    Log("---------- STRESS PARAMETERS --------\n");
    Log("dwSeed:                %lu\n", g_TestParams.dwSeed);
    Log("dwBallDetail:          %lu\n", g_TestParams.dwBallDetail);
    Log("dwWallDetail:          %lu\n", g_TestParams.dwWallDetail);
    Log("dwLightDetail:         %lu\n", g_TestParams.dwLightDetail);
    Log("dwNumLayers:           %lu\n", g_TestParams.dwNumLayers);
    Log("dwNumBalls:            %lu\n", g_TestParams.dwNumBalls);
    Log("dwMaxBalls:            %lu\n", g_TestParams.dwMaxBalls);
    Log("dwNumLights:           %lu\n", g_TestParams.dwNumLights);
    Log("dwMaxLights:           %lu\n", g_TestParams.dwMaxLights);
    Log("fRadius:               %g\n", g_TestParams.fRadius);
    Log("fMaxSpeed:             %g\n", g_TestParams.fMaxSpeed);
    Log("fMinSpeed:             %g\n", g_TestParams.fMinSpeed);
    Log("bTextureBalls          %s\n", g_TestParams.bTextureBalls ? "TRUE" : "FALSE");
    Log("szBallBaseTex:         %s\n", g_TestParams.szBallBaseTex);
    Log("szBallBumpTex:         %s\n", g_TestParams.szBallBumpTex);
    Log("szSphereBaseTex:       %s\n", g_TestParams.szSphereBaseTex);
    Log("szLightVSH:            %s\n", g_TestParams.szLightVSH);
    Log("szSparkVSH:            %s\n", g_TestParams.szSparkVSH);
    Log("bDMusic:               %s\n", g_TestParams.bDMusic ? "TRUE" : "FALSE");
    Log("bDMDoWorkLocally:      %s\n", g_TestParams.bDMDoWorkLocally ? "TRUE" : "FALSE");
    Log("bDMUseGM:              %s\n", g_TestParams.bDMUseGM ? "TRUE" : "FALSE");
    Log("bDMLoaderThread:       %s\n", g_TestParams.bDMLoaderThread ? "TRUE" : "FALSE");
    Log("dwDMVoiceCount:        %d\n", g_TestParams.dwDMVoiceCount);
    Log("szDMStressDir:         %s\n", g_TestParams.szDMStressDir);
    Log("szDMSupportDir:        %s\n", g_TestParams.szDMSupportDir);
    Log("szDMAudioPath:         %s\n", g_TestParams.szDMAudioPath);
    Log("szDMHeap:              %s\n", g_TestParams.szDMHeap);
    Log("dwDMFixedHeapSizeNorm: %u\n", g_TestParams.dwDMFixedHeapSizeNorm);
    Log("dwDMFixedHeapSizePhys: %u\n", g_TestParams.dwDMFixedHeapSizePhys);
    Log("dwMaxSparks:           %lu\n", g_TestParams.dwMaxSparks);
    Log("bSparksAreLights:      %s\n", g_TestParams.bSparksAreLights ? "TRUE" : "FALSE");
    Log("dwSparksPerCollision:  %lu\n", g_TestParams.dwSparksPerCollision);
    Log("dwSparkMaxAge:         %lu\n", g_TestParams.dwSparkMaxAge);
    Log("bDrawWireframe:        %s\n", g_TestParams.bDrawWireframe ? "TRUE" : "FALSE");
    Log("bRelativeSpeed:        %s\n", g_TestParams.bRelativeSpeed ? "TRUE" : "FALSE");
    Log("dwFileIOSize:          %lu\n", g_TestParams.dwFileIOSize);
    Log("lDMMasterVolume:       %ld\n", g_TestParams.lDMMasterVolume);
    Log("lDMMasterTranspose:    %hd\n", g_TestParams.lDMMasterTranspose);
    Log("fDMMasterTempo:        %g\n", g_TestParams.fDMMasterTempo);
    Log("bD3DPerf:              %s\n", g_TestParams.bD3DPerf ? "TRUE" : "FALSE");
    Log("szDSScratchImage:      %s\n", g_TestParams.szDSScratchImage);
    Log("szDSEnvironment:       %s\n", g_TestParams.szDSEnvironment);
    Log("b3DSound:              %s\n", g_TestParams.b3DSound ? "TRUE" : "FALSE");
    Log("bSubMixBallSound:      %s\n", g_TestParams.bSubMixBallSound ? "TRUE" : "FALSE");
    Log("szLightSound:          %s\n", g_TestParams.szLightSound);
    Log("lDSVolume:             %ld\n", g_TestParams.lDSVolume);
    Log("dwDSFreq:              %lu\n", g_TestParams.dwDSFreq);
    Log("lAmbientVolume:        %ld\n", g_TestParams.lAmbientVolume);
    Log("bCopyMediaFromNetwork  %s\n", g_TestParams.bCopyMediaFromNetwork ? "TRUE" : "FALSE");
    Log("dwMultiSampleType:     %lu\n", g_TestParams.dwMultiSampleType);
    Log("dwBackBufferCount:     %lu\n", g_TestParams.dwBackBufferCount);
    Log("bCycleDisplayModes:    %s\n", g_TestParams.bCycleDisplayModes ? "TRUE" : "FALSE");
    Log("dwCycleTime:           %ld\n", g_TestParams.dwCycleTime);
    Log("dwDMDebugLevel:        %ld\n", g_TestParams.dwDMDebugLevel);
    Log("dwDMRIPLevel:          %ld\n", g_TestParams.dwDMRIPLevel);

    if (g_TestParams.dwDMSegments)
    {
        Log("ppDMSegments:  \n");
        for (DWORD i=0; i<g_TestParams.dwDMSegments; i++)
            Log("                                  %s\n", g_TestParams.ppDMSegments[i]);

    }
    else
    {
        Log("ppDMSegments:         NONE\n");
    }

#ifdef _DEBUG
    Log("g_dwDirectSoundDebugLevel:         %ld\n", g_dwDirectSoundDebugLevel);
    Log("g_dwDirectSoundDebugBreakLevel:    %ld\n", g_dwDirectSoundDebugBreakLevel);
#endif

    Log("-----------------------------------\n");
    return S_OK;
};

//------------------------------------------------------------------------------
//Read Test Params
//-----------------------------------------------------------------------------
static LPCSTR szAppName = "MEDIA";
static LPCSTR szDefaultININame =    "D:\\testini.ini";
static LPCSTR szSecondaryININame =  "T:\\testini.ini";
static LPCSTR szSectionName = "DMSEGMENTS";
#define MAXSIZE 0xFFFF

HRESULT LoadTestParams(void)
    {
    CHAR *szTemp;
    FLOAT fTemp = 0;
    DWORD dwTemp = 0;
    DWORD dwBufferSize=0;
    DWORD dwReturned = 0;
    LARGE_INTEGER liSeed = {0};
    DWORD i=0;
    CHAR szININame[MAX_PATH] = {0};

    //First look for testini's in the current directory, then on t:\    
    if (0 == _access(szDefaultININame, 0))
    {
        Log("Note: Found default ini file %s\n", szDefaultININame);
        strcpy(szININame, szDefaultININame);
    }
    else
    {
        Log("Note: Couldn't find %s; trying secondary INI file %s\n", szDefaultININame, szSecondaryININame);
        if (0 == _access(szSecondaryININame, 0))
        {
            Log("Note: Found secondary ini file %s\n", szSecondaryININame);            
            strcpy(szININame, szSecondaryININame);
        }
        else
        {            
            Log("Note: Couldn't find secondary file %s either; giving up!!\n", szSecondaryININame);
            return E_FAIL;
        }
    }

    //Allocate 0xFFFF- more than enough to hold all the file names
    szTemp = new CHAR[MAXSIZE];
    memset(szTemp, 0, MAXSIZE);
    memset(&g_TestParams, 0, sizeof(g_TestParams));

    //Randomize calls to rand() based on current time.
    g_TestParams.dwSeed = GetPrivateProfileIntA(szAppName, "dwseed", 0, szININame);
    if (g_TestParams.dwSeed == 0)
    {
        Log("Seed not specified; generating one.\n");
        QueryPerformanceCounter(&liSeed);
        g_TestParams.dwSeed = liSeed.LowPart;
    }
    srand(g_TestParams.dwSeed);    

    g_TestParams.dwBallDetail = GetPrivateProfileIntA(szAppName, "dwballdetail", 12, szININame);
    g_TestParams.dwWallDetail = GetPrivateProfileIntA(szAppName, "dwwalldetail", 20, szININame);
    g_TestParams.dwLightDetail = GetPrivateProfileIntA(szAppName, "dwlightdetail", 12, szININame);
    g_TestParams.dwNumLayers = GetPrivateProfileIntA(szAppName, "dwnumlayers", 0, szININame);
    g_TestParams.dwNumBalls = GetPrivateProfileIntA(szAppName, "dwnumballs", 20, szININame);
    g_TestParams.dwMaxBalls = GetPrivateProfileIntA(szAppName, "dwmaxballs", 50, szININame);
    g_TestParams.dwNumLights = GetPrivateProfileIntA(szAppName, "dwnumlights", 5, szININame);
    g_TestParams.dwMaxLights = GetPrivateProfileIntA(szAppName, "dwmaxlights", 10, szININame);

    g_TestParams.dwMaxSparks = GetPrivateProfileIntA(szAppName, "dwmaxsparks", 500, szININame);
    g_TestParams.bSparksAreLights =GetPrivateProfileIntA(szAppName, "bsparksarelights", 1, szININame);
    g_TestParams.dwSparksPerCollision =GetPrivateProfileIntA(szAppName, "dwsparkspercollision", 4, szININame);
    g_TestParams.dwSparkMaxAge =GetPrivateProfileIntA(szAppName, "dwsparkmaxage", 100, szININame);
    g_TestParams.bDrawWireframe =GetPrivateProfileIntA(szAppName, "bdrawwireframe", 1, szININame);
    g_TestParams.bRelativeSpeed =GetPrivateProfileIntA(szAppName, "brelativespeed", 1, szININame);

    g_TestParams.dwFileIOSize = GetPrivateProfileIntA(szAppName, "dwFileIOSize", 128, szININame);
    g_TestParams.lDMMasterVolume=    GetPrivateProfileIntA(szAppName, "lDMMasterVolume", 0, szININame);
    g_TestParams.lDMMasterTranspose = (short)GetPrivateProfileIntA(szAppName, "lDMMasterTranspose", 0, szININame);
    GetPrivateProfileStringA(szAppName, "fDMMasterTempo", "1",  szTemp, 49, szININame);
    g_TestParams.fDMMasterTempo = (FLOAT)atof(szTemp);

    GetPrivateProfileStringA(szAppName,   "fradius",   "5.5", szTemp, 49, szININame);
    fTemp = (float)atof(szTemp);
    if (fTemp != 0.0f)
        g_TestParams.fRadius = fTemp;    

    GetPrivateProfileStringA(szAppName, "fmaxspeed", "10",  szTemp, 49, szININame);
    g_TestParams.fMaxSpeed = (FLOAT)atof(szTemp);

    GetPrivateProfileStringA(szAppName, "fminspeed", "0.1", szTemp, 49, szININame);
    g_TestParams.fMinSpeed = (FLOAT)atof(szTemp);

    g_TestParams.bTextureBalls = GetPrivateProfileIntA(szAppName, "bTextureBalls", TRUE, szININame);
    g_TestParams.bD3DPerf = GetPrivateProfileIntA(szAppName, "bD3DPerf", TRUE, szININame);

    GetPrivateProfileStringA(szAppName, "szBallBaseTexture", "t:\\media\\graphics\\ballbase.bmp", g_TestParams.szBallBaseTex, MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szBallBumpTexture", "t:\\media\\graphics\\ballbump.bmp", g_TestParams.szBallBumpTex, MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szSphereBaseTexture", "t:\\media\\graphics\\sphrbase.bmp", g_TestParams.szSphereBaseTex, MAX_PATH, szININame);

    GetPrivateProfileStringA(szAppName, "szLightVSH", "t:\\media\\graphics\\light.vsh", g_TestParams.szLightVSH, MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szSparkVSH", "t:\\media\\graphics\\spark.vsh", g_TestParams.szSparkVSH, MAX_PATH, szININame);

    GetPrivateProfileStringA(szAppName, "szBounceSound", "t:\\media\\audio\\pcm\\ball.wav", g_TestParams.szBounceSound, MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szLightSound", "", g_TestParams.szLightSound, MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szDSHRTF", "FULL", g_TestParams.szDSHRTF, MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szDSEnvironment", "SewerPipe", g_TestParams.szDSEnvironment, MAX_PATH, szININame);

    g_TestParams.b3DSound = GetPrivateProfileIntA(szAppName, "b3DSound", TRUE, szININame);
    g_TestParams.bSubMixBallSound = GetPrivateProfileIntA(szAppName, "bSubMixBallSound", TRUE, szININame);
    g_TestParams.bLightSound = GetPrivateProfileIntA(szAppName, "bLightSound", TRUE, szININame);
    g_TestParams.lDSVolume = GetPrivateProfileIntA(szAppName, "lDSVolume", 0, szININame);
    g_TestParams.dwDSFreq = GetPrivateProfileIntA(szAppName, "dwDSFreq", 48000, szININame);
    g_TestParams.lAmbientVolume = GetPrivateProfileIntA(szAppName, "lAmbientVolume", -2200, szININame);

    g_TestParams.bDMusic            = GetPrivateProfileIntA(szAppName, "bDMusic",          TRUE, szININame);
    g_TestParams.bDMDoWorkLocally   = GetPrivateProfileIntA(szAppName, "bDMDoWorkLocally", FALSE, szININame);
    g_TestParams.bDMUseGM           = GetPrivateProfileIntA(szAppName, "bDMUseGM"       , FALSE, szININame);
    g_TestParams.bDMLoaderThread    = GetPrivateProfileIntA(szAppName, "bDMLoaderThread", TRUE, szININame);
    g_TestParams.dwDMVoiceCount     = GetPrivateProfileIntA(szAppName, "dwDMVoiceCount",   128, szININame);

    g_TestParams.dwDMDebugLevel     = GetPrivateProfileIntA(szAppName, "dwDMDebugLevel",   1, szININame);
    g_TestParams.dwDMRIPLevel       = GetPrivateProfileIntA(szAppName, "dwDMRIPLevel",     0, szININame);

    GetPrivateProfileStringA(szAppName, "szDMStressDir",  "t:\\media\\music\\default", g_TestParams.szDMStressDir,  MAX_PATH, szININame);
    GetPrivateProfileStringA(szAppName, "szDMSupportDir", "t:\\media\\music\\support", g_TestParams.szDMSupportDir, MAX_PATH, szININame);

    //If user specified random audiopath, choose a different one now.
    GetPrivateProfileStringA(szAppName, "szDMAudioPath", "Random", g_TestParams.szDMAudioPath, MAX_PATH, szININame);
    if (_strcmpi("random", g_TestParams.szDMAudioPath)==0)
    {
        strcpy(g_TestParams.szDMAudioPath, AudioPathPairs[rand() % NUMELEMS(AudioPathPairs)].szString);
        Log("Choosing random audiopath: %s", g_TestParams.szDMAudioPath);
    }



    //READ IN THE SEGMENT NAMES!!
    //===========================
    //Get our string; parse it out.
    dwReturned = GetPrivateProfileSectionA(szSectionName, szTemp, MAXSIZE, szININame);
    if (dwReturned == MAXSIZE - 2)
    {
        Log("ERROR: Filled up entire %d characters, need to increase MAXSIZE", MAXSIZE);
    }
    //Count the words in the string, if it exists.
    if (szTemp[0])
    {
        for (i=0;;i++)
        {
            if (szTemp[i]==0 && i)
                g_TestParams.dwDMSegments++;

            if (szTemp[i]==0 && szTemp[i+1] == 0)
                break;
        }
    }
    //If there's any strings, then allocate space and copy them.
    if (g_TestParams.dwDMSegments)
    {
        //Allocate that many LPSTR's
        g_TestParams.ppDMSegments = new LPSTR[g_TestParams.dwDMSegments];

        //Go through again and allocate each ptr, and copy the word into it.
        DWORD dwBaseIndex = 0;
        for (i=0; i<g_TestParams.dwDMSegments; i++)
        {
            //Get length + 1
            dwTemp = strlen(&szTemp[dwBaseIndex]) + 1;
            g_TestParams.ppDMSegments[i] = new CHAR[dwTemp];
            strcpy(g_TestParams.ppDMSegments[i], &szTemp[dwBaseIndex]);
            while (szTemp[dwBaseIndex++]);
        }
    }
    delete[]szTemp;


    //Scratch Image
    GetPrivateProfileStringA(szAppName, "szDSScratchImage", "Default", g_TestParams.szDSScratchImage, MAX_PATH, szININame);

    //Heap
    //Default size is 2 MB apiece for a fixed heap, and of course 0 for non-fixed heaps (they can grow as big as they want)
    GetPrivateProfileStringA(szAppName, "szDMHeap", "Tracking", g_TestParams.szDMHeap, MAX_PATH, szININame);
    BOOL bFixedSizeHeap = (_strcmpi(g_TestParams.szDMHeap, "FixedUser") == 0) || (_strcmpi(g_TestParams.szDMHeap, "FixedDefault") == 0) || (_strcmpi(g_TestParams.szDMHeap, "FixedCache") == 0);
    g_TestParams.dwDMFixedHeapSizeNorm    = GetPrivateProfileIntA(szAppName, "dwDMFixedHeapSizeNorm",     bFixedSizeHeap ? 4*1024*1024 : 0, szININame);
    g_TestParams.dwDMFixedHeapSizePhys  = GetPrivateProfileIntA(szAppName, "dwDMFixedHeapSizePhys",   bFixedSizeHeap ? 1024*1024 / 2: 0, szININame);

    g_TestParams.bCopyMediaFromNetwork = GetPrivateProfileIntA(szAppName, "bCopyMediaFromNetwork", TRUE, szININame);

    //Dsound debug
#ifdef _DEBUG
    g_dwDirectSoundDebugLevel = GetPrivateProfileIntA(szAppName,      "dwDSDebugLevel", 3, szININame);
    g_dwDirectSoundDebugBreakLevel = GetPrivateProfileIntA(szAppName, "dwDSDebugBreakLevel", 2, szININame);
#endif

    g_TestParams.dwMultiSampleType = GetPrivateProfileIntA(szAppName, "dwMultiSampleType", 0x0011, szININame);
    g_TestParams.dwBackBufferCount = GetPrivateProfileIntA(szAppName, "dwBackBufferCount", 0, szININame);
    g_TestParams.bCycleDisplayModes = GetPrivateProfileIntA(szAppName, "bCycleDisplayModes", FALSE, szININame);
    g_TestParams.dwCycleTime = GetPrivateProfileIntA(szAppName, "dwCycleTime", 1000, szININame);

    PrintTestParams();
    return S_OK;
};


//------------------------------------------------------------------------------
//Verify files exist.  This must be called AFTER the files are copied down.
//-----------------------------------------------------------------------------
HRESULT SanityCheckFiles(void)
{
HRESULT hr = S_OK;

    BOOL bFound = FALSE;
    for (DWORD i=0; i<NUMELEMS(AudioPathPairs); i++)
    {
        if (_strcmpi(g_TestParams.szDMAudioPath, (const char *)AudioPathPairs[i].szString)==0)
        {
            bFound = TRUE;
            break;
        }
    }

    //If the name didn't match one of the audiopath types or "random" then it's a path, and
    //  we should make sure it exists.
    if (!bFound)
    {
        if (_access(g_TestParams.szDMAudioPath, 0))
        {
            Log("Error: Audiopath file %s is not found\n", g_TestParams.szDMAudioPath);
            hr = E_FAIL;
        }
    }

    if (_access(g_TestParams.szBounceSound, 0))
    {
        Log("Error: Wave file for bounce sound (%s) is not found\n", g_TestParams.szBounceSound);
        hr = E_FAIL;
    }

    if (_strcmpi("Default", g_TestParams.szDSScratchImage))
    {
        if (_access(g_TestParams.szDSScratchImage, 0))
        {
            Log("Error: DSP image file (%s) is not found\n", g_TestParams.szDSScratchImage);
            hr = E_FAIL;
        }
    }


    if (_access(g_TestParams.szDMStressDir, 0))
    {
        Log("Error: Directory for DMusic segment files (%s) is not found\n", g_TestParams.szDMStressDir);
        hr = E_FAIL;
    }

    return hr;

}


//------------------------------------------------------------------------------
//Print warning messages for things that aren't right.
//-----------------------------------------------------------------------------
HRESULT SanityCheckTestParams(void)
{
HRESULT hr = S_OK;

    if (g_TestParams.ppDMSegments && g_TestParams.dwDMSegments == 0)
    {
        Log("Error: ppDMSegments is not NULL but dwDMSegments == 0\n");
        hr = E_FAIL;
    }

    if (g_TestParams.ppDMSegments == 0 && g_TestParams.dwDMSegments > 0)
    {
        Log("Error: ppDMSegments is NULL but dwDMSegments > 0\n");
        hr = E_FAIL;
    }

    if (g_TestParams.ppDMSegments && g_TestParams.szDMStressDir)
    {
        Log("Warning: szDMStressDir = %s but [DMSegments] section was specified.  Using [DMSegments] section\n", g_TestParams.szDMStressDir);
    }

    if (!g_TestParams.bDMusic && g_TestParams.ppDMSegments)
    {
        Log("Warning: bDMusic=FALSE but [DMSegments] section was specified.  Ignoring [DMSegments] section\n");
    }

    if (g_TestParams.dwNumBalls > g_TestParams.dwMaxBalls)
    {
        Log("Error: dwNumBalls = %d; dwMaxBalls = %d\n", g_TestParams.dwNumBalls , g_TestParams.dwMaxBalls);
        hr = E_FAIL;
    }

    if (g_TestParams.dwNumLights > g_TestParams.dwMaxLights)
    {
        Log("Error: dwNumLights = %d; dwMaxLights = %d\n", g_TestParams.dwNumLights , g_TestParams.dwMaxLights);
        hr = E_FAIL;
    }
    if (g_TestParams.dwSparksPerCollision > g_TestParams.dwMaxSparks)
    {
        Log("Error: dwSparksPerCollision = %d; dwMaxSparks = %d\n", g_TestParams.dwSparksPerCollision , g_TestParams.dwMaxSparks);
        hr = E_FAIL;
    }

 

    if (_strcmpi(g_TestParams.szDSHRTF, "full") &&
        _strcmpi(g_TestParams.szDSHRTF, "light")
        )
    {
        Log("Error: Specified %s for g_TestParams.szDSHRTF; need to specify ""FULL"" or ""LIGHT""\n", g_TestParams.szDSHRTF);
        hr = E_FAIL;
    }

    if (_strcmpi(g_TestParams.szDMHeap, "Default") &&
        _strcmpi(g_TestParams.szDMHeap, "Tracking") &&
        _strcmpi(g_TestParams.szDMHeap, "FixedUser") &&
        _strcmpi(g_TestParams.szDMHeap, "FixedDefault") &&
        _strcmpi(g_TestParams.szDMHeap, "FixedCache")

        )
    {
        Log("Error: Specified %s for g_TestParams.szDMHeap; need to specify ""Default"", ""Tracking"", ""FixedUser"", ""FixedDefault"", or ""FixedCache""\n", g_TestParams.szDMHeap);
        hr = E_FAIL;
    }

    if (!(_strcmpi(g_TestParams.szDMHeap, "FixedUser") || _strcmpi(g_TestParams.szDMHeap, "FixedDefault") || _strcmpi(g_TestParams.szDMHeap, "FixedCache")))
    {
        if(g_TestParams.dwDMFixedHeapSizeNorm != 0)
        {
            Log("Warning: Specified %s (not ""Fixed*"") for g_TestParams.szDMHeap, but specified a non-zero size (%u) for the szDMFixedHeapSizeNorm.\n", g_TestParams.szDMHeap, g_TestParams.dwDMFixedHeapSizeNorm);
        }
        if(g_TestParams.dwDMFixedHeapSizePhys != 0)
        {
            Log("Warning: Specified %s (not ""Fixed*"") for g_TestParams.szDMHeap, but specified a non-zero size (%u) for the szDMFixedHeapSizePhys.\n", g_TestParams.szDMHeap, g_TestParams.dwDMFixedHeapSizePhys);
        }
    }

    switch (g_TestParams.dwMultiSampleType & 0xFFFF) {
        case D3DMULTISAMPLE_NONE:
        case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_LINEAR:
        case D3DMULTISAMPLE_2_SAMPLES_MULTISAMPLE_QUINCUNX:
        case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_HORIZONTAL_LINEAR:
        case D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR:
        case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_LINEAR:
        case D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN:
        case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_LINEAR:
        case D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN:
        case D3DMULTISAMPLE_9_SAMPLES_MULTISAMPLE_GAUSSIAN:
        case D3DMULTISAMPLE_9_SAMPLES_SUPERSAMPLE_GAUSSIAN:
            break;
        default:
            Log("Warning: Invalid multisample type 0x%X specified. Using D3DMULTISAMPLE_NONE by default.\n", g_TestParams.dwMultiSampleType & 0xFFFF);
            g_TestParams.dwMultiSampleType = D3DMULTISAMPLE_NONE;
            break;
    }

    switch (g_TestParams.dwMultiSampleType & 0xF0000) {
        case D3DMULTISAMPLE_PREFILTER_FORMAT_DEFAULT:
        case D3DMULTISAMPLE_PREFILTER_FORMAT_X1R5G5B5:
        case D3DMULTISAMPLE_PREFILTER_FORMAT_R5G6B5:
        case D3DMULTISAMPLE_PREFILTER_FORMAT_X8R8G8B8:
        case D3DMULTISAMPLE_PREFILTER_FORMAT_A8R8G8B8:
            break;
        default:
            Log("Warning: Invalid prefilter buffer format 0x%X specified, switching to the default.\n", g_TestParams.dwMultiSampleType & 0xF0000);
            g_TestParams.dwMultiSampleType &= 0xFFF0FFFF;
            break;
    }

    if (g_TestParams.dwBackBufferCount > 2) {
        Log("Error: Specified %d for g_TestParams.dwBackBufferCount\n", g_TestParams.dwBackBufferCount);
        hr = E_FAIL;
    }

    return hr;
};



//------------------------------------------------------------------------------
//Deallocate everything allocated by testparams.
//-----------------------------------------------------------------------------
HRESULT FreeTestParams(void)
{
DWORD i = 0;

for (DWORD i=0; i<g_TestParams.dwDMSegments; i++)
{
    if (g_TestParams.ppDMSegments)
    {
        delete []g_TestParams.ppDMSegments[i];
        g_TestParams.ppDMSegments[i] = NULL;
    }
    else
        Log("ERROR!!  Deleting string of ppDMSegments[%d] but it was already NULL!", i);

}

delete []g_TestParams.ppDMSegments;
g_TestParams.ppDMSegments = NULL;
return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DownloadScratch
// Desc: Downloads a DSP scratch image to the DSP
//-----------------------------------------------------------------------------
HRESULT DownloadScratch(IDirectSound *pDSound, PCHAR pszScratchFile)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwSize = 0;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc = {0};
    EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
    EffectLoc.dwCrosstalkIndex   = I3DL2_CHAIN_XTALK;
    
    if (_strcmpi("Default", g_TestParams.szDSScratchImage) == 0)
    {
        CHECKRUN(XAudioDownloadEffectsImage("dsstdfx", &EffectLoc, XAUDIO_DOWNLOADFX_XBESECTION, &pDesc));
    }
    else
    {
        CHECKRUN(XAudioDownloadEffectsImage(pszScratchFile, &EffectLoc, XAUDIO_DOWNLOADFX_EXTERNFILE, &pDesc));
    }

    return hr;
}

//------------------------------------------------------------------------------
// Multiply the given color values and convert the result to a 32 bit color
//-----------------------------------------------------------------------------
D3DCOLOR ModulateColors(D3DCOLORVALUE* pdcv1, D3DCOLORVALUE* pdcv2) {

    D3DCOLORVALUE dcv;
    dcv.r = pdcv1->r * pdcv2->r;
    dcv.g = pdcv1->g * pdcv2->g;
    dcv.b = pdcv1->b * pdcv2->b;
    dcv.a = pdcv1->a * pdcv2->a;
    return (BYTE)(dcv.a * 255.0f) << 24 | (BYTE)(dcv.r * 255.0f) << 16 |
           (BYTE)(dcv.g * 255.0f) << 8 | (BYTE)(dcv.b * 255.0f);
}




//------------------------------------------------------------------------------
//  WaitForSegmentStop
//------------------------------------------------------------------------------
HRESULT 
WaitForSegmentStop(
                   IN IDirectMusicPerformance8*   ptPerf8, 
                   IN IDirectMusicSegment8*       ptSegment8, 
                   IN IDirectMusicSegmentState8*  ptSegmentState8, 
                   IN DWORD                         dwTimeout
                   )
/*++

Routine Description:

    Returns S_OK if the segment stopped within the specified time; E_FAIL otherwise.

Arguments:

    IN ptPerf8 -            Performance
    IN ptSegment8 -         Segment to query 
    IN ptSegmentState8 -    Segment state 
    IN dwTimeout -          Milleseconds to timeout
    IN enableBreak -        TRUE to enable the user/tester to break out

Return Value:

    S_OK if the segment stopped within the specified time; E_FAIL otherwise.

--*/
{
    DWORD   dwStartTime     = 0;

    dwStartTime = timeGetTime();
    do
    {
        // Has the segment stopped playing?
        OptionalDoWork();
        if(ptPerf8->IsPlaying(ptSegment8, ptSegmentState8) != S_OK)
            return S_OK;
          
    }
    while (timeGetTime() - dwStartTime < dwTimeout);

    return E_FAIL;
}

//------------------------------------------------------------------------------
//  WaitForSegmentStart
//------------------------------------------------------------------------------
HRESULT 
WaitForSegmentStart(
                    IN IDirectMusicPerformance8*  ptPerf8, 
                    IN IDirectMusicSegment8*      ptSegment8, 
                    IN IDirectMusicSegmentState8* ptSegmentState8,
                    IN DWORD                        dwTimeout
                    )
/*++

Routine Description:

    Returns S_OK if the segment started within the specified time; E_FAIL otherwise.

Arguments:

    IN ptPerf8 -            Performance
    IN ptSegment8 -         Segment to query 
    IN ptSegmentState8 -    Segment state 
    IN dwTimeout -          Milleseconds to timeout
    IN enableBreak -        TRUE to enable the user/tester to break out

Return Value:

    S_OK if the segment started within the specified time; E_FAIL otherwise.

--*/
{
    DWORD dwStartTime = 0;
    dwStartTime = timeGetTime();
    do
    {
        // Has the segment started playing?
        OptionalDoWork();
        if(ptPerf8->IsPlaying(ptSegment8, ptSegmentState8) == S_OK)
            return S_OK;
    }
    while (timeGetTime() - dwStartTime < dwTimeout);

    return E_FAIL;
};

/********************************************************************************
Returns S_OK if the segment started within the specified time; E_FAIL otherwise.
Prints a message on error.
********************************************************************************/
HRESULT ExpectSegmentStart(IDirectMusicPerformance8 *ptPerf8, IDirectMusicSegment8 *ptSegment8, IDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPSTR szSegname)
{
HRESULT hr = S_OK;
hr = WaitForSegmentStart(ptPerf8, ptSegment8, ptSegmentState8, dwTimeout);
if (FAILED(hr))
    Log("ERROR: ""%s"" didn't start playing after %d ms", szSegname, dwTimeout);

return hr;
};


/********************************************************************************
Returns S_OK if the segment stopped within the specified time; E_FAIL otherwise.
Prints a message on error.
********************************************************************************/
HRESULT ExpectSegmentStop(IDirectMusicPerformance8 *ptPerf8, IDirectMusicSegment8 *ptSegment8, IDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPSTR szSegname)
{
HRESULT hr = S_OK;
hr = WaitForSegmentStop(ptPerf8, ptSegment8, ptSegmentState8, dwTimeout);
if (FAILED(hr))
    Log("ERROR: ""%s"" didn't stop playing after %d ms", szSegname, dwTimeout);

return hr;
};


/********************************************************************************
********************************************************************************/
void OptionalDoWork(void)
{
    if (g_TestParams.bDMDoWorkLocally)
        DirectMusicDoWork(10);

}

/********************************************************************************
********************************************************************************/
void WaitForThreadEnd(HANDLE *phThread)
{
    DWORD dwWaitResult = WAIT_TIMEOUT;

    if(*phThread)
    {
        dwWaitResult = WAIT_TIMEOUT;
        while(dwWaitResult != WAIT_OBJECT_0)
        {
            dwWaitResult = WaitForSingleObject(*phThread, 0);
            Sleep(100);
        }

        CloseHandle(*phThread);
        *phThread = NULL;
    }
}

}



