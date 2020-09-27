// Copyright (c) 1998 Microsoft Corporation
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <assert.h>
#include <initguid.h>
#include <objbase.h>

#include <dmusicc.h>
#include <dmusici.h>

#include "lsclsid.h"

static const int CLSID_STRING_SIZE = 39;

typedef struct _CLSIDENT
{
	struct _CLSIDENT *next;
	CLSID guid;
	char name[256];
} CLSIDENT;

typedef struct _COMDLL 
{
	struct _COMDLL *next;
	char dll[256];
	CLSIDENT *clsidList;
} COMDLL;

static COMDLL *comdllList = NULL;
static int verbose = 0;
static BOOL fShownNotFoundHeader = FALSE;

void AddClsid(GUID guid, LPSTR pstrName);

HRESULT CLSIDToStr(const CLSID &clsid, LPSTR szStr, int cbStr);
BOOL OpenRegistry();
void CloseRegistry();

void CheckFile(LPSTR pstrName);
int  CheckFilePath(LPSTR pstrPath, LPSTR pstrFile);
void PrintFileInfo(LPSTR pstrName, int verbose);
char *DirFormatTime(const time_t *pt);

static HKEY hkClsidRoot;
static BOOL fWin9x = FALSE;

#define Add(clsid) AddClsid(CLSID_##clsid, #clsid);

void main(int argc, char **argv)
{
    OSVERSIONINFO osvi;
	char sz[CLSID_STRING_SIZE + 1];
	COMDLL *comdll;
	CLSIDENT *clsident;
	int parserr = 0;
	int printclsids = 0;

    osvi.dwOSVersionInfoSize = sizeof(osvi);
    GetVersionEx(&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		fWin9x = TRUE;
	}

	for (argv++,argc--; argc; argv++,argc--) {
		char *p = *argv;

		if (p[0] == '-') {
			switch(p[1]) {
			case 'v' :
				verbose++;
				break;

			case 'c':
				printclsids++;
				break;

			default:
				fprintf(stderr, "lsclsid: invalid option '%c'\n", p[1]);
				parserr++;

			case '?':
				fprintf(stderr, "lsclsid [-v] [-c] [-?]\n");
				fprintf(stderr, "\t-v\tVerbose file information\n");
				fprintf(stderr, "\t-c\tPrint CLSID's per file\n");
				break;
			}
		}

		if (parserr) exit(1);
	}


	if (!OpenRegistry()) {
		fprintf(stderr, "Could not open registry\n");
		exit(1);
	}

	Add(DirectMusic);
	Add(DirectMusicCollection);
	Add(DirectMusicBand);
	Add(DirectMusicBandTrk);
	Add(DirectMusicPerformance);
	Add(DirectMusicSegment);
	Add(DirectMusicSegmentState);
	Add(DirectMusicGraph);
	Add(DirectMusicTempoTrack);
	Add(DirectMusicSeqTrack);
	Add(DirectMusicSysExTrack);
	Add(DirectMusicTimeSigTrack);
	Add(DirectMusicStyle);
	Add(DirectMusicChordTrack);
	Add(DirectMusicCommandTrack);
	Add(DirectMusicStyleTrack);
	Add(DirectMusicMotifTrack);
	Add(DirectMusicPersonality);
	Add(DirectMusicComposer);
	Add(DirectMusicSignPostTrack);
	Add(DirectMusicLoader);
	Add(DirectMusicSynth);

	if (fShownNotFoundHeader) {
		putchar('\n');
	}

	for (comdll = comdllList; comdll; comdll = comdll->next) {

		PrintFileInfo(comdll->dll, verbose);

		if (printclsids) {
			for (clsident = comdll->clsidList; clsident; clsident = clsident->next) {
				if (FAILED(CLSIDToStr(clsident->guid, sz, sizeof(sz))))
					*sz = '\0';
				printf("\t%s\t%s\n", sz, clsident->name);
			}

			putchar('\n');
		}
	}

	CloseRegistry();

	if (!verbose)
		putchar('\n');

//	if (fWin9x) {
		// These files not needed on Windows NT
		//
		CheckFile("dmusic32.dll");
		CheckFile("dmusic16.dll");
//	}
	CheckFile("ksuser.dll");
}

void AddClsid(GUID guid, LPSTR pstrName)
{
	LONG lr;
	char szClsid[CLSID_STRING_SIZE + 30];
	char szServer[256];
	char szKey[256];
	DWORD dwType;
	DWORD dwSize;
	HKEY hk;
	COMDLL *comdll;
	CLSIDENT *clsident;
	


	if (FAILED(CLSIDToStr(guid, szClsid, sizeof(szClsid)))) {
		fprintf(stderr, "%s: could not convert CLSID to string\n", pstrName);
		return;
	}

	strcat(szClsid, "\\%s");

	wsprintf(szKey, szClsid, "InProcServer32");
	lr = RegOpenKeyEx(hkClsidRoot,
					  szKey,
					  0,
					  KEY_READ,
					  &hk);
	if (lr) {
		if (!fShownNotFoundHeader) {
			fprintf(stderr, "The following CLSID's are not registered:\n");
			fShownNotFoundHeader = TRUE;
		}

		fprintf(stderr, "    CLSID_%s\n", pstrName);
		return;
	}

	dwSize = sizeof(szServer);
	lr = RegQueryValueEx(hk,
						 NULL,
						 NULL,
						 &dwType,
						 (LPBYTE)szServer,
						 &dwSize);

	if (lr)
	{
		fprintf(stderr, "%s: InProcServer32 not found in registry [%ld].\n", pstrName, lr);
	}
	else
	{
		for (comdll = comdllList; comdll; comdll = comdll->next) {
			if (!strcmp(comdll->dll, szServer)) {
				break;
			}
		}

		if (!comdll) {
			comdll = new COMDLL;
			ZeroMemory(comdll, sizeof(COMDLL));
			strcpy(comdll->dll, szServer);

			comdll->next = comdllList;
			comdllList = comdll;
		}

		clsident = new CLSIDENT;
		ZeroMemory(clsident, sizeof(CLSIDENT));

		strcpy(clsident->name, pstrName);
		clsident->guid = guid;

		clsident->next = comdll->clsidList;
		comdll->clsidList = clsident;
	}
	RegCloseKey(hk);
}

HRESULT
CLSIDToStr(const CLSID &clsid,
           char *szStr,
           int cbStr)
{
    // XXX What to return here?
    //
    assert(cbStr >= CLSID_STRING_SIZE);
    
	LPOLESTR wszCLSID = NULL;
	HRESULT hr = StringFromCLSID(clsid, &wszCLSID);
    if (!SUCCEEDED(hr)) {
        return hr;
    }

	// Covert from wide characters to non-wide.
	wcstombs(szStr, wszCLSID, cbStr);

	// Free memory.
    CoTaskMemFree(wszCLSID);

    return S_OK;
}

BOOL
OpenRegistry()
{
	LONG lr;

	lr = RegCreateKeyEx(HKEY_CLASSES_ROOT,
                        "CLSID",
                        0,
                        NULL,
                        REG_OPTION_NON_VOLATILE,
                        KEY_READ,
                        NULL,
                        &hkClsidRoot,
                        NULL);

	return (lr == 0);
}

void
CloseRegistry()
{
	RegCloseKey(hkClsidRoot);
}

// Checks for the existence of a DLL in the LoadLibrary search path
//
void CheckFile(LPSTR pstrName)
{
	char *pathlist = getenv("PATH");
	char *savepathlist;
	char *semicolon;
	char sys[256];
	int  count = 0;

	printf("Search path for %s:\n", pstrName);

	if (pathlist) 
		pathlist = strdup(pathlist);
	savepathlist = pathlist;

	count += CheckFilePath(".", pstrName);

	GetSystemDirectory(sys, sizeof(sys));
	count += CheckFilePath(sys, pstrName);

	/* Sleazy hack to see if SYSTEM32; if so, then we need to check SYSTEM next (NT) 
	 * NOTE Docs guarantee the names of these under NT
	 */
	if (sys[strlen(sys) - 1] == '2') {
		sys[strlen(sys) - 2] = '\0';
		count += CheckFilePath(sys, pstrName);
	}

	GetWindowsDirectory(sys, sizeof(sys));
	count += CheckFilePath(sys, pstrName);
	
	if (pathlist) {
		while ((semicolon = strchr(pathlist, ';')) != NULL) {
			*semicolon++ = '\0';
			count += CheckFilePath(pathlist, pstrName);
			pathlist = semicolon;
		}

		if (*pathlist) 
			count += CheckFilePath(pathlist, pstrName);
	}

	if (savepathlist)
		free(savepathlist);

	if (count) {
		putchar('\n');
	} else {
		printf("This .DLL was not found in the search path.\n");
	}

}

int CheckFilePath(LPSTR pstrPath, LPSTR pstrFile)
{
	char sz[256];

	strcpy(sz, pstrPath);
	if (sz[strlen(sz) - 1] != '\\' &&
		sz[strlen(sz) - 1] != '/')
	{
		strcat(sz, "\\");
	}
	strcat(sz, pstrFile);

	if (_access(sz, 0) == -1)
		return 0;

	PrintFileInfo(sz, verbose);

	return 1;
}


void PrintFileInfo(LPSTR pstrName, int verbose)
{
	struct stat dllstat;
	
	if (stat(pstrName, &dllstat) == -1) {
		dllstat.st_mtime = 0;
	}	

	printf("%-16.16s  %-15.15s  %s\n", 
		   DirFormatTime(&dllstat.st_mtime),
		   FileVersionInfo(pstrName),
		   pstrName);

	if (verbose) {
		char *pTags[MAX_LONGVERINFO_LINES];
		char *pLines[MAX_LONGVERINFO_LINES];
		unsigned maxLine;
		int nLines = LongFileVersionInfo(pstrName, pTags, pLines, &maxLine);
		int idx;

		if (nLines == 0) {
			printf("\tNo version information available for this file.\n");
		} else {
			for (idx = 0; idx < nLines; ++idx) {
				printf("\t%-*.*s     %s\n",
					maxLine, maxLine, pTags[idx], pLines[idx]);
			}
		}

		putchar('\n');
 	}
}

char *DirFormatTime(const time_t *pt)
{
	static char szTimeFormat[] = "%02u/%02u/%02u  %02u:%02u%c";
	struct tm* ptm = localtime(pt);
	static char szTimeBuf[40];

	if (ptm == NULL)
		return "";

	int  hour = ptm->tm_hour;
	char ampm = (hour < 12) ? 'a' : 'p';

	hour %= 12; 
	if (hour == 0) 
		hour = 12;

	sprintf(szTimeBuf, 
			szTimeFormat, 
			ptm->tm_mon + 1,
			ptm->tm_mday,
			ptm->tm_year % 100,
			hour,
			ptm->tm_min,
			ampm);

	return szTimeBuf;
}
