/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Module Name:

    Profiler.c

Abstract:

    Profiling code

-------------------------------------------------------------------*/

// ++++ Include Files +++++++++++++++++++++++++++++++++++++++++++++++
#include "stdafx.h"

#ifdef DO_PROFILE

long *pOutputBuffer;
long *pOutputCursor;
long tagReturnTag[4096];
short tagReturnThread[4096];
long resourceStart[4096];
int tagType[4096];
typeProfileInfo g_rgprofinfo[MAX_PROFILE_EVENTS];

DWORD g_dwTickStart;
int   g_iCurProfEvent = 0;
DWORD g_dwPerfFreq;

__inline DWORD GetTick()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.LowPart;
}

void AppendEntry(int nType, int nStart, int nEnd)
{
    *pOutputCursor++ = nType;
    *pOutputCursor++ = (long)((float)(nStart - g_dwTickStart) / 397.f * 500000.f * 10000.f / (float)g_dwPerfFreq);
    *pOutputCursor++ = (long)((float)(nEnd - g_dwTickStart) / 397.f * 500000.f  * 10000.f / (float)g_dwPerfFreq);
}

int g_cProfileMappings;
char g_rgszProfileMappings[100][200];
int g_cPredefinedMappings;
void InitProfile()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    g_dwPerfFreq = li.LowPart;

    g_dwTickStart = GetTick();
    g_iCurProfEvent = 0;

    // Create the pre-defined mappings
    g_cProfileMappings = 0;

    char *rgszPredefinedMappings[] = {
        "Profile End",
        "CDVD::Reset",
        "CDVD::PopulateToList",
        "CDVD::CompactLayer",
        "CDVD::RefreshRelationships",
        "CDVD::PopulateDirEntries",
        "CDVD::RefreshFolderDirectoryEntry",
        "CDVD::RefreshDirectoryEntries",
        "CDVD::Insert",
        "CDVD::ValidatePlaceholders",
        "CDVD::CheckValidPlaceholderLSN",
        "CDVD::RefreshPlaceholders",
        "CDVD::PersistTo",
        "CDVD::CreateFrom",
        "CDVD::PersistFST",
        "CDVD::PersistBIN",
        0
    };

    while (rgszPredefinedMappings[g_cProfileMappings])
    {
        strcpy(g_rgszProfileMappings[g_cProfileMappings], rgszPredefinedMappings[g_cProfileMappings]);
        g_cProfileMappings++;
    }
    g_cPredefinedMappings = g_cProfileMappings - 1;
}

DWORD MapProfile(char *szName)
{
    strcpy(g_rgszProfileMappings[g_cProfileMappings++], szName);
    return g_cProfileMappings - 1;
}

void StartProfile(DWORD nType)
{
    // Store the info in memory - we'll parse it at the end
    g_rgprofinfo[g_iCurProfEvent].dwTick = GetTick();
    g_rgprofinfo[g_iCurProfEvent].nType = nType;
    g_iCurProfEvent++;
}

void EndProfile(DWORD nType)
{
    // Store the info in memory - we'll parse it at the end
    g_rgprofinfo[g_iCurProfEvent].dwTick = GetTick();
    g_rgprofinfo[g_iCurProfEvent].nType = nType | END_EVENT;
    g_iCurProfEvent++;
}

void DumpProfile()
{
	char szTemp[512];
	HANDLE hfile;
	long tag;
	int funcTag;
	int funcStart;
    unsigned long tmpVal;
    int i;
	DWORD dwWritten;

    // **************************************
    // Create the tags.log file.  Hack one up
    // **************************************

    // Create the tags file
	hfile = CreateFile(TEXT("\\tags.log"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	sprintf( szTemp, "tagtype,tag,subtag,description,datadesc1,datadesc2\r\n");
	WriteFile(hfile, szTemp, strlen(szTemp), &dwWritten, NULL); 
    // Write TAG_RANGE_START(TAG_RANGE_DDHAL	, "DDHAL / D3DHAL" )
	sprintf( szTemp, "%d,%d,%d,%s,%s,%s\r\n", 0, 0, 0, "xbGameDisc", "", "");
	WriteFile(hfile, szTemp, strlen(szTemp), &dwWritten, NULL); 
	sprintf( szTemp, "%d,%d,%d,%s,%s,%s\r\n", 1, 1, 0, "xbGameDisc", "", "");
	WriteFile(hfile, szTemp, strlen(szTemp), &dwWritten, NULL); 

    // Write out each of the tags
    for (i = 0; i < g_cProfileMappings; i++)
    {
	    sprintf( szTemp, "%d,%d,%d,%s,%s,%s\r\n", 10, i+2, 0, g_rgszProfileMappings[i], "", "");
	    WriteFile(hfile, szTemp, strlen(szTemp), &dwWritten, NULL); 
    }

    // We're done with the tags file - close it.
	CloseHandle(hfile);

    // **************************************
    // Now write out the events to events.log
    // **************************************

	pOutputBuffer = (long *)malloc(2000000);
	assert (pOutputBuffer);
	pOutputCursor = pOutputBuffer;

	for( tag=0; tag<4096; tag++ )
	{
		tagType[tag] = -1;
		tagReturnTag[tag] = -1;
		tagReturnThread[tag] = -1;
		resourceStart[tag] = -1;
	}
	funcTag = -1;


    for (i = 0; i < g_iCurProfEvent; i++)
    {
		tag = g_rgprofinfo[i].nType;
		if (tag & END_EVENT)
		{
			// Exit function
			AppendEntry(((tag & 0x7FFFFFFF)+2)<<4, funcStart, g_rgprofinfo[i].dwTick );
            funcStart = g_rgprofinfo[i].dwTick;
			funcTag   = tagReturnTag[(tag & 0x7FFFFFFF)];
			tagReturnTag[(tag & 0x7FFFFFFF)] = -1;
		}
		else
		{
			// Enter function
			if(funcTag != -1 ) // already in a function - this will go one deeper
				AppendEntry((funcTag+2)<<4, funcStart, g_rgprofinfo[i].dwTick );
			tagReturnTag[tag] = funcTag;
			funcTag           = tag;
			funcStart         = g_rgprofinfo[i].dwTick;
		}
    }

	// For now, we copy all data... later we may only copy up to the timelog of the last entry
	hfile = CreateFile("\\Events.log", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	tmpVal = ((unsigned long) pOutputCursor - (unsigned long) pOutputBuffer)>>2;

	WriteFile(hfile, &tmpVal, 4, &dwWritten, NULL);
	WriteFile(hfile, pOutputBuffer, (unsigned long)pOutputCursor - (unsigned long)pOutputBuffer, &dwWritten, NULL);
	FlushFileBuffers(hfile);
    CloseHandle(hfile);

	free((void *)pOutputBuffer);
}
#else
DWORD MapProfile(char *szName)
{
    return 0;
}
#endif