/*
**
** xbdir
**
** Directory list for files on the Xbox
**
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "xbfile.hxx"
#include "xbfilr.hxx"
#include "optn.hxx"
#include "resource.h"
#include <xboxverp.h>

enum { mdOutLong, mdOutBare, mdOutWide };
enum { mdSortNone, mdSortName, mdSortSize, mdSortDate };
TIME_ZONE_INFORMATION g_tzi;
BOOL fHaveTimeZone;
int mdOut;
int mdSort;
BOOL fReverseSort;
BOOL fListedSomething;
BOOL fFirstHeader;
int g_cDirs;
int g_cFiles, g_cCurDirFiles;
ULONGLONG g_cbyTotalFileSize, g_cbyCurDirTotalFileSize;

static char g_szBuffer[32] = {'\0'};

char *FormatBigNumber(ULONGLONG n)
{
    // Convert the number into a comma-separated string; ie "1,234,567,890"
    char *sz = (char*)(g_szBuffer + 31);
    int i = 0;

    *sz = '\0';
    do {
        *--sz = ((char)(n%10)) + '0';
        if ((++i)%3 == 0 && n/10)
            *--sz = ',';
    } while (n/=10);

    return sz;
}

char *FormatNumber(int n)
{
    return FormatBigNumber((ULONGLONG)(ULONG)n);
}

/* Directory file */
struct DIRF : public FILR {
	BOOL m_fPrintedHeader;
	DIRF *m_pdirfDir;
	DIRF *m_pdirfNext;
	DIRF **m_ppdirfNext;
	DIRF *m_pdirfHead;
	BOOL m_fPrintingDir;

	DIRF(const FIL &fil) : FILR(fil), m_fPrintingDir(FALSE) {
		m_fBreadthFirst = TRUE;
		m_fPreOrder = true;
	}

	BOOL FStartDir(void);
	void DoWork(void);
	void EndDir(void);

	virtual void PrintHeader(void);
	virtual void PrintOne(void) {}
};

/* Directory list: long */
struct DIRL : public DIRF {
	DIRL(const FIL &fil) : DIRF(fil) { m_fPrintingDir = TRUE; }

	FILR *PfilrCopy(void) const { return new DIRL(*this); }

	void PrintOne(void);
};

/* Directory list: wide */
struct DIRW : public DIRF {

	DIRW(const FIL &fil) : DIRF(fil) {}

	void EndDir(void);
	FILR *PfilrCopy(void) const { return new DIRW(*this); }
};

/* Directory list: bare */
struct DIRB : public DIRF {
	DIRB(const FIL &fil) : DIRF(fil) { m_fPrintingDir = TRUE; }

	FILR *PfilrCopy(void) const { return new DIRB(*this); }

	void PrintHeader(void) {}
	void PrintOne(void);
};

BOOL FFtToLt(LPFILETIME lpft, LPSYSTEMTIME lpst)
{
	SYSTEMTIME stZ;

	/* Convert the file time to a system time */
	if(!FileTimeToSystemTime(lpft, &stZ))
		return FALSE;
	/* Convert the system time to a local time */
	if(!fHaveTimeZone)
		fHaveTimeZone = GetTimeZoneInformation(&g_tzi) != TIME_ZONE_ID_UNKNOWN;
	if(!fHaveTimeZone || !SystemTimeToTzSpecificLocalTime(&g_tzi, &stZ,
			lpst))
		*lpst = stZ;
	return TRUE;
}

DIRF *PdirfSort(DIRF *pdirf)
{
	DIRF *pdirf1;
	DIRF *pdirf2;
	DIRF **ppdirf1;
	DIRF **ppdirf2;
	int sgn;

	if(!pdirf || !pdirf->m_pdirfNext)
		return pdirf;

	ppdirf1 = &pdirf1;
	ppdirf2 = &pdirf2;

	while(pdirf) {
		*ppdirf1 = pdirf;
		ppdirf1 = &pdirf->m_pdirfNext;
		pdirf = *ppdirf1;
		if(pdirf) {
			*ppdirf2 = pdirf;
			ppdirf2 = &pdirf->m_pdirfNext;
			pdirf = *ppdirf2;
		}
	}
	*ppdirf1 = *ppdirf2 = NULL;

	pdirf1 = PdirfSort(pdirf1);
	pdirf2 = PdirfSort(pdirf2);

	ppdirf1 = &pdirf;
	while(pdirf1 || pdirf2) {
		if(!pdirf1)
			sgn = 1;
		else if(!pdirf2)
			sgn = -1;
		else {
			PDM_FILE_ATTRIBUTES pdmfa1 = pdirf1->PdmfaGet();
			PDM_FILE_ATTRIBUTES pdmfa2 = pdirf2->PdmfaGet();
			if(!pdmfa1)
				sgn = 1;
			else if(!pdmfa2)
				sgn = -1;
			else {
				switch(mdSort) {
				case mdSortName:
					sgn = _stricmp(pdirf1->m_szName, pdirf2->m_szName);
					break;
				case mdSortSize:
					if(pdmfa1->SizeHigh > pdmfa2->SizeHigh)
						sgn = 1;
					else if(pdmfa1->SizeHigh < pdmfa2->SizeHigh)
						sgn = -1;
					else if(pdmfa1->SizeLow > pdmfa2->SizeLow)
						sgn = 1;
					else
						sgn = -1;
					break;
				case mdSortDate:
					if(((PULARGE_INTEGER)&pdmfa1->ChangeTime)->QuadPart >
							((PULARGE_INTEGER)&pdmfa2->ChangeTime)->QuadPart)
						sgn = 1;
					else
						sgn = -1;
					break;
				}
				if(fReverseSort)
					sgn = -sgn;
			}
		}
		if(sgn < 0) {
			*ppdirf1 = pdirf1;
			ppdirf1 = &pdirf1->m_pdirfNext;
			pdirf1 = *ppdirf1;
		} else {
			*ppdirf1 = pdirf2;
			ppdirf1 = &pdirf2->m_pdirfNext;
			pdirf2 = *ppdirf1;
		}
	}

	*ppdirf1 = NULL;
	return pdirf;
}

void DIRF::PrintHeader(void)
{
	char szName[1024];

	if(!m_pdirfDir->m_fPrintedHeader) {
        m_pdirfDir->FillDisplayName(szName);
		printf("\n Directory of %s\n\n", szName);
		m_pdirfDir->m_fPrintedHeader = TRUE;
	}
}

BOOL DIRF::FStartDir(void)
{
	m_pdirfDir = this;
	m_fPrintedHeader = FALSE;

	if (!fSubdirs || m_fMatches)
		PrintHeader();

	/* Set up to keep a list of files in this directory */
	m_ppdirfNext = &m_pdirfHead;
	return TRUE;
}

void DIRF::DoWork(void)
{
	/* Add this file to the list */
	if(m_fPrintingDir && !mdSort)
		PrintOne();
	else {
		DIRF *pdirf = (DIRF *)PfilrCopy();
		*m_pdirfDir->m_ppdirfNext = pdirf;
		m_pdirfDir->m_ppdirfNext = &pdirf->m_pdirfNext;
	}
}

void DIRF::EndDir(void)
{
    if (m_fPrintedHeader)
    {
        printf("%16s File(s)",  FormatNumber(g_cCurDirFiles));
        printf(" %13s bytes\n", FormatBigNumber(g_cbyCurDirTotalFileSize));
        g_cCurDirFiles = 0;
        g_cbyCurDirTotalFileSize = 0;
    }

	*m_ppdirfNext = NULL;
	if(mdSort) {
		DIRF *pdirfHead = PdirfSort(m_pdirfHead);
		DIRF *pdirf;

		while(pdirfHead) {
			pdirf = pdirfHead;
			pdirfHead = pdirf->m_pdirfNext;
			pdirf->PrintOne();
			delete pdirf;
		}
	}
}

/* Long list: guts */
void DIRL::PrintOne(void)
{
	PDM_FILE_ATTRIBUTES pdmfa = PdmfaGet();
	char szTime[32];
	char szSize[32];

	PrintHeader();
    fListedSomething = TRUE;
    fFirstHeader = FALSE;

	/* Get the appropriate attributes and print this sucker out */
	SYSTEMTIME st;

	if(FFtToLt(&pdmfa->ChangeTime, &st))
		sprintf(szTime, "%02d/%02d/%04d  %02d:%02d%c",
			st.wMonth, st.wDay, st.wYear,
			st.wHour > 12 ? st.wHour - 12 : st.wHour,
			st.wMinute,
			st.wHour < 12 ? 'a' : 'p');
	else
		sprintf(szTime, "%-18.18s", "  <bad date>");

	if(pdmfa->Attributes & FILE_ATTRIBUTE_DIRECTORY) {
		sprintf(szSize, "%-18.18s", "    <DIR>");
        g_cDirs++;
    }
	else {
		int ichReal = 18;
		int ich = 0;
		DWORD dwSize = pdmfa->SizeLow;

		szSize[ichReal] = 0;
		if(dwSize == 0) {
			szSize[--ichReal] = '0';
			--ichReal;
		} else while(ichReal-- && dwSize) {
			if(ich && (ich % 3) == 0)
				szSize[ichReal--] = ',';
			szSize[ichReal] = (char)(dwSize % 10) + '0';
			dwSize /= 10;
			++ich;
		}
		while(ichReal >= 0)
			szSize[ichReal--] = ' ';

        g_cbyTotalFileSize       += pdmfa->SizeLow;
        g_cbyCurDirTotalFileSize += pdmfa->SizeLow;
        g_cFiles++;
        g_cCurDirFiles++;
	}

	printf("%s  %s %s\n", szTime, szSize, pdmfa->Name[0] ? pdmfa->Name :
		m_szName);
}

/* Bare list: guts */
void DIRB::PrintOne(void)
{
	char szName[1024];

    fListedSomething = TRUE;
	if(fSubdirs || fRecurse) {
		FillDisplayName(szName);
		puts(szName);
	} else
		puts(m_szName);
}

/* Wide list: guts */
void DIRW::EndDir(void)
{
	const int ccol = 5;
	const int cchCol = (79 / ccol) - 1;
	DIRF *rgpdirf[ccol+1];
	int icol;
    int crw;

	*m_ppdirfNext = NULL;
	if(mdSort)
		m_pdirfHead = PdirfSort(m_pdirfHead);
	rgpdirf[0] = m_pdirfHead;
	/* Set up the column heads */
	for(icol = 1; icol <= ccol; ++icol) {
		rgpdirf[icol] = rgpdirf[icol - 1] ? rgpdirf[icol - 1]->m_pdirfNext :
			NULL;
	}
    crw = rgpdirf[0] ? 1 : 0;
	if(rgpdirf[ccol]) {
		/* We have at least one file per column; populate the columns */
		icol = 0;
		while(rgpdirf[ccol]) {
			for(int icolT = icol + 1; icolT <= ccol; ++icolT)
				rgpdirf[icolT] = rgpdirf[icolT]->m_pdirfNext;
            if(icol == 0)
                ++crw;
			icol = (icol + 1) % ccol;
		}
	}

	/* With all of the columns filled, we can start printing */
	while(crw--) {
		PrintHeader();
        fListedSomething = TRUE;
		/* If this is the last row, we need to null out all columns that
		 * have been finished */
		if(!rgpdirf[ccol - 1]) {
			while(icol < ccol)
				rgpdirf[icol++] = NULL;
			icol = 0;
		}
		/* Print out the files we have */
		if(rgpdirf[0]) {
			for(int icolT = 0; icolT < ccol; ++icolT) {
				if(rgpdirf[icolT]) {
					char sz[64];
					char *pszFormat;
					PDM_FILE_ATTRIBUTES pdmfa = rgpdirf[icolT]->PdmfaGet();
					DWORD dwAttr = pdmfa ? pdmfa->Attributes : 0;
					if(dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
                        pszFormat = "[%s]";
                        g_cDirs++;
                    } else {
                        if(dwAttr & FILE_ATTRIBUTE_READONLY)
						    pszFormat = "%s*";
					    else
						    pszFormat = "%s";
                        g_cbyTotalFileSize += pdmfa->SizeLow;
                        g_cbyCurDirTotalFileSize += pdmfa->SizeLow;
                        g_cFiles++;
                        g_cCurDirFiles++;
                    }
					sprintf(sz, pszFormat, rgpdirf[icolT]->m_szName);
					printf("%-*.*s ", cchCol, cchCol, sz);
					rgpdirf[icolT] = rgpdirf[icolT]->m_pdirfNext;
				}
			}
			putchar('\n');
		}
	}

	/* Now we can free the file list on this directory */
	while(m_pdirfHead) {
		DIRF *pdirf = m_pdirfHead;
		m_pdirfHead = pdirf->m_pdirfNext;
		delete pdirf;
	}

    if (m_fPrintedHeader)
    {
        printf("%16s File(s)",  FormatNumber(g_cCurDirFiles));
        printf(" %13s bytes\n", FormatBigNumber(g_cbyCurDirTotalFileSize));
        g_cCurDirFiles = 0;
        g_cbyCurDirTotalFileSize = 0;
    }
}

void ListOneDir(const FIL &fil)
{
	DIRF *pdirf;
	LPCSTR szPattern;
	DM_FILE_ATTRIBUTES dmfa;

	switch(mdOut) {
	case mdOutBare:
		pdirf = new DIRB(fil);
		break;
	case mdOutWide:
		pdirf = new DIRW(fil);
		break;
	default:
		pdirf = new DIRL(fil);
		break;
	}

	if(!pdirf->FIsWildName() && SUCCEEDED(pdirf->HrGetFileAttributes(&dmfa)) &&
		(dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		/* As a directory file, we need to treat this as a wildcard on the
		 * directory contents */
		pdirf->Descend();
		pdirf->m_szName[0] = '*';
		pdirf->m_szName[1] = 0;
        /* And since we've at least found the directory we're going to list,
         * we can act as if we've already listed something so we don't generate
         * a file not found */
        fListedSomething = TRUE;
	}
	pdirf->DoTree(TRUE);
	delete pdirf;
}

BOOL OPTH::FParseOption(char *&sz, char)
{
	switch(*sz) {
	case 'b':
		if(mdOut) {
bw:
			fprintf(stderr, "error: only one of /b and /w can be specified\n");
			exit(1);
		}
		mdOut = mdOutBare;
		break;
	case 'w':
		if(mdOut)
			goto bw;
		mdOut = mdOutWide;
		break;
	case 'o':
		if(mdSort) {
			fprintf(stderr, "error: only one sort mode can be specified\n");
			exit(1);
		}
		if(*++sz == '-') {
			++sz;
			fReverseSort = TRUE;
		}
		switch(*sz) {
		case 'd':
			mdSort = mdSortDate;
			break;
		case 'n':
			mdSort = mdSortName;
			break;
		case 's':
			mdSort = mdSortSize;
			break;
		default:
			UsageFailure();
		}
		break;
	default:
		return FParseFilrOption(*sz);
	}
	return TRUE;
}

void OPTH::UsageFailure(void)
{
    fprintf(stderr, "xbdir version " VER_PRODUCTVERSION_STR "\n\n"
        "Displays a list of files and subdirectories in a directory on either\n"
        "the Xbox development or target system.\n\n");
	fprintf(stderr, "\nusage: %s [/x xboxname] [options] files\n", m_szExe);
	fprintf(stderr, "\t/r\trecursive listing\n"
		"\t/s\tsearch in subdirectories\n"
        "\t/h\tinclude hidden files\n"
		"\t/b\tlist filenames only\n"
		"\t/w\tlist files in columns\n"
		"\t/o[-]{n,d,s}\n\torder files by one of\n"
		"\t\tn: name\n"
		"\t\td: date\n"
		"\t\ts: size\n"
		"\t\t-: reverse order\n"
		"Specify Xbox files as xE:\\..., xD:\\..., etc.\n");
	exit(1);
}

int __cdecl main(int cArg, char **rgszArg)
{
    FIL filDir;
	HRESULT hr;
    DWORD cch;
    BOOL fRet;
    char szXboxName[256];
    char szTemp[1024];

    g_cFiles = 0;
    g_cDirs = 0;
    g_cbyTotalFileSize = 0;
    g_cCurDirFiles = 0;
    g_cbyCurDirTotalFileSize = 0;
    fFirstHeader = TRUE;

	/* Process arguments */
	OPTH opth(cArg, rgszArg);

	if(cArg < 0)
		opth.UsageFailure();

    /* Open up a shared connection so we can get an address out of it */
    DmUseSharedConnection(TRUE);
    hr = DmSendCommand(NULL, NULL, NULL, 0);

    /* Remember our name */
    cch = sizeof szXboxName;
    if(mdOut != mdOutBare && SUCCEEDED(DmGetXboxName(szXboxName, &cch))) {
        ULONG ulAddr;
        BYTE *pb = (BYTE *)&ulAddr;
        if(SUCCEEDED(hr) && SUCCEEDED(DmResolveXboxName(&ulAddr)))
            printf(" Xbox target system %s (%d.%d.%d.%d)\n", szXboxName,
            pb[3], pb[2], pb[1], pb[0]);
        else
            printf(" Xbox target system %s\n", szXboxName);
    }

    if(FAILED(hr))
        /* Unable to connect */
        Fatal(hr, 0);

	if(cArg == 0) {
		filDir = "*";
		ListOneDir(filDir);
	    if(fSubdirs && !fRecurse) {
		    fSubdirs = FALSE;
		    fRecurse = TRUE;
	    }
	} else {
		for(; cArg--; ++rgszArg) {
			filDir = *rgszArg;
			ListOneDir(filDir);
		}
	}

    // Output total sizes
    if (fListedSomething && mdOut != mdOutBare) {
        ULARGE_INTEGER nFreeBytes, nFreeOnDisk, nBytesOnDisk;

        if (fRecurse || fSubdirs) {
            printf("\n     Total Files Listed:\n");
            printf("%16s File(s)",  FormatNumber(g_cFiles));
            printf("%14s bytes\n", FormatBigNumber(g_cbyTotalFileSize));
        }
        printf("%16s dir(s)", FormatNumber(g_cDirs));

        if (filDir.m_fXbox)
        {
            sprintf(szTemp, "%c:\\", filDir.m_fXbox);
            DmGetDiskFreeSpace(szTemp, &nFreeBytes, &nBytesOnDisk, &nFreeOnDisk);
        } else {
            // Check if a drive was specified
            if (filDir.m_chPCDrive)
                sprintf(szTemp, "%c:\\", filDir.m_chPCDrive);
            else {
                // No drive specified - use current drive
                char sz[MAX_PATH];
                GetCurrentDirectory(MAX_PATH, sz);
                sprintf(szTemp, "%c:\\", sz[0]);
            }
            GetDiskFreeSpaceEx(szTemp, &nFreeBytes, &nBytesOnDisk, &nFreeOnDisk);
        }
        if (nFreeBytes.HighPart)
        {
            sprintf(szTemp, "  %s", FormatBigNumber(nFreeBytes.QuadPart));
            printf("%13s bytes free\n", szTemp);
        }
        else
            printf("  %13s bytes free\n", FormatNumber(nFreeBytes.LowPart));
    }

	DmUseSharedConnection(FALSE);

    if(!fListedSomething)
        printf("File Not Found\n");
    return !fListedSomething;
}
