// ----------------------------------------------------------------------------
// nuke.c
//
// Copyright (C) Microsoft Corporation
// ----------------------------------------------------------------------------

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

// ----------------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------------

typedef struct {
    UINT    cEnt;
    char *  rgpsz[1];
} PATHINFO;

typedef struct {
    char *      psz;
    DWORD       dwFlags;
} PATENTRY;

#define PATENTRYF_DIRWILD   0x00000001
#define PATENTRYF_HASWILD   0x00000002
#define PATENTRYF_ALLWILD   0x00000004

typedef struct {
    char *      psz;
    DWORD       dwFlags;
    UINT        cSpec;
    UINT        cWild;
    UINT        cEnt;
    PATENTRY    rgpe[1];
} PAT;

#define PATF_HASDIRWILD     0x00000001

// ----------------------------------------------------------------------------
// Forward declarations
// ----------------------------------------------------------------------------

BOOL MatchItem(PATENTRY * ppe, char * psz);

// ----------------------------------------------------------------------------
// Globals
// ----------------------------------------------------------------------------

BOOL    g_fStartAtRoot      = TRUE;
BOOL    g_fDisplayOnly      = FALSE;
BOOL    g_fDisplayDeletes   = FALSE;
BOOL    g_fDisplayArchives  = FALSE;
BOOL    g_fDisplayRemain    = TRUE;
BOOL    g_fDisplayRestores  = FALSE;

BOOL    g_fUndo             = FALSE;
BOOL    g_fVerbose          = FALSE;

char *  g_pszCurDir         = NULL;
char *  g_pszIniDir         = NULL;
char *  g_pszArcDir         = NULL;

UINT    g_cpatDel           = 0;
PAT **  g_rgppatDel         = NULL;
UINT    g_cpatNoDel         = 0;
PAT **  g_rgppatNoDel       = NULL;
UINT    g_cpatArc           = 0;
PAT **  g_rgppatArc         = NULL;
UINT    g_cpatNoArc         = 0;
PAT **  g_rgppatNoArc       = NULL;
UINT    g_cpatUndo          = 0;
PAT **  g_rgppatUndo        = NULL;

UINT    g_cFile             = 0;
char ** g_rgpszFile         = NULL;
UINT    g_cDel              = 0;
char ** g_rgpszDel          = NULL;
UINT    g_cArc              = 0;
char ** g_rgpszArc          = NULL;
UINT    g_cRem              = 0;
char ** g_rgpszRem          = NULL;

static PAT *** g_rgpppat[]   = { &g_rgppatDel, &g_rgppatNoDel, &g_rgppatArc, &g_rgppatNoArc };
static UINT * g_rgcpat[]     = { &g_cpatDel, &g_cpatNoDel, &g_cpatArc, &g_cpatNoArc };
static char * g_rgpszSect[]  = { "delete", "dont_delete", "archive", "dont_archive" };

#define ARRAYSIZE(a)        (sizeof(a)/sizeof(*a))

// ----------------------------------------------------------------------------
// Program
// ----------------------------------------------------------------------------

void Usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    nuke [options] { ... }\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    Nuke searches from the current directory to the root for a nuke.ini file\n");
    fprintf(stderr, "    and follows the instructions there to remove generated files from a\n");
    fprintf(stderr, "    project.  Depending on the instructions, a generated file may be deleted\n");
    fprintf(stderr, "    outright, moved into a parallel archive tree, or left alone.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    Archived files may be restored by using the -u (undo) switch.  Selective\n");
    fprintf(stderr, "    undo is possible by specifying a file pattern to the undo switch.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    By default all operations occur from the directory where nuke.ini is found.\n");
    fprintf(stderr, "    To limit operations to the current directory and its children, add '...' to\n");
    fprintf(stderr, "    the command line.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "    -?,-h           print this message\n");
    fprintf(stderr, "    -n              print out what would have happened instead of doing it\n");
    fprintf(stderr, "    -u [pattern]    restore files from the archive which match the pattern,\n");
    fprintf(stderr, "                    or all files if no pattern is given\n");
    fprintf(stderr, "    -v              print out verbose descriptions\n");
    fprintf(stderr, "\n");

    exit(1);
}

void __cdecl Fatal(char * pszFormat, ...)
{
    char szBuf[1024];
    va_list va;

    va_start(va, pszFormat);
    vsprintf(szBuf, pszFormat, va);
    va_end(va);

    fprintf(stderr, "%s\n", szBuf);
    exit(1);
}

void Win32Error(char * pszFunc)
{
    char szBuf[512];

    szBuf[0] = 0;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szBuf, sizeof(szBuf), NULL);
    Fatal("%s failed.  %s", pszFunc, szBuf);
}

BOOL FileExists(char * pszFile)
{
    DWORD dw = GetFileAttributes(pszFile);

    if (dw == 0xFFFFFFFF)
    {
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
            return(FALSE);

        Win32Error("FileExists --> GetFileAttributes");
    }

    return((dw & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

PATHINFO * ParsePath(char * pszPath)
{
    PATHINFO * ppi;
    char * pch;
    int iItem, cEnt = 1;

    for (pch = pszPath; *pch; ++pch)
        if (*pch == '\\')
            cEnt += 1;

    ppi = (PATHINFO *)malloc(offsetof(PATHINFO, rgpsz) + cEnt * sizeof(char *));
    if (ppi == NULL)
        Fatal("Out of memory");

    ppi->cEnt = cEnt;

    for (iItem = 0, pch = pszPath; iItem < cEnt; ++iItem)
    {
        ppi->rgpsz[iItem] = pch;
        while (*pch && *pch != '\\')
            pch++;
        *pch++ = 0;
    }

    return(ppi);
}

BOOL ParsePatternItem(char * psz, PATENTRY * ppe, UINT * pcSpec, UINT * pcWild)
{
    char * pch = psz;

    ppe->psz = psz;
    ppe->dwFlags = 0;

    if (pch[0] == '*' && pch[1] == '*' && pch[2] == 0)
    {
        ppe->dwFlags |= PATENTRYF_DIRWILD;
        (*pcWild) += 1;
        return(TRUE);
    }

    if (pch[0] == '*' && pch[1] == 0)
    {
        ppe->dwFlags |= PATENTRYF_ALLWILD;
        (*pcWild) += 1;
        return(TRUE);
    }

    for (; *pch; ++pch)
    {
        if (*pch == '*')
        {
            if (pch[1] == '*')
                return(FALSE);
            ppe->dwFlags |= PATENTRYF_HASWILD;
            (*pcWild) += 1;
        }
        else
        {
            (*pcSpec) += 1;
        }
    }

    return(TRUE);
}

int __cdecl
comparepatspec(const void * pv1, const void * pv2)
{
    PAT * ppat1 = *(PAT **)pv1;
    PAT * ppat2 = *(PAT **)pv2;

    if (ppat1->cSpec < ppat2->cSpec)
        return(+1);
    else if (ppat1->cSpec > ppat2->cSpec)
        return(-1);
    else if (ppat1->cWild < ppat2->cWild)
        return(-1);
    else if (ppat1->cWild > ppat2->cWild)
        return(+1);
    else
        return(0);
}

void DumpPattern(PAT * ppat)
{
    UINT i;

    printf("pattern=\"%s\" (cSpec=%d.%d) %s\n",
           ppat->psz, ppat->cSpec, ppat->cWild,
           (ppat->dwFlags & PATF_HASDIRWILD) ? "HASDIRWILD" : "");

    for (i = 0; i < ppat->cEnt; ++i)
    {
        printf("  \"%s\" %s%s%s\n", ppat->rgpe[i].psz,
               (ppat->rgpe[i].dwFlags & PATENTRYF_DIRWILD) ? "DIRWILD" : "",
               (ppat->rgpe[i].dwFlags & PATENTRYF_HASWILD) ? "HASWILD" : "",
               (ppat->rgpe[i].dwFlags & PATENTRYF_ALLWILD) ? "ALLWILD" : "");
    }
}

void DumpPatternList(UINT cpat, PAT ** pppat)
{
    for (; cpat > 0; --cpat, ++pppat)
    {
        DumpPattern(*pppat);
    }
}

void DumpAllPatterns()
{
    UINT i, j;

    for (i = 0; i < ARRAYSIZE(g_rgpszSect); ++i)
    {
        printf("[%s] (cPat=%d)\n", g_rgpszSect[i], *g_rgcpat[i]);
        DumpPatternList(*g_rgcpat[i], *g_rgpppat[i]);
    }
}

PAT * ParsePattern(char * pszPattern)
{
    PATHINFO *  ppi;
    PAT *       ppat;
    char        szBuf[MAX_PATH];
    char *      pszPat;
    char *      pszBuf;
    UINT        i;
    
    if (*pszPattern == '\\')
    {
        pszPattern += 1;
    }

    strcpy(szBuf, pszPattern);

    pszPat = _strdup(szBuf);
    pszBuf = _strdup(szBuf);
    ppi    = ParsePath(pszBuf);

    ppat = (PAT *)malloc(offsetof(PAT, rgpe) + ppi->cEnt * sizeof(PATENTRY));
    if (ppat == NULL)
        Fatal("Out of memory");

    ppat->psz     = pszPat;
    ppat->dwFlags = 0;
    ppat->cSpec   = 0;
    ppat->cWild   = 0;
    ppat->cEnt    = ppi->cEnt;

    for (i = 0; i < ppi->cEnt; ++i)
    {
        if (!ParsePatternItem(ppi->rgpsz[i], &ppat->rgpe[i], &ppat->cSpec, &ppat->cWild))
            return(NULL);

        if (ppat->rgpe[i].dwFlags & PATENTRYF_DIRWILD)
        {
            if (ppat->dwFlags & PATF_HASDIRWILD)
                return(NULL);
            ppat->dwFlags |= PATF_HASDIRWILD;
        }
    }

    if (ppat->rgpe[ppat->cEnt - 1].dwFlags & PATENTRYF_DIRWILD)
        return(NULL);

    return(ppat);
}

void AddPatternToList(PAT * ppat, UINT * pcpat, PAT *** ppppat)
{
    if ((*pcpat % 16) == 0)
    {
        PAT ** pppatNew = (PAT **)malloc((*pcpat + 16) * sizeof(PAT **));
        if (pppatNew == NULL)
            Fatal("Out of memory");
        if (*pcpat > 0)
        {
            memcpy(pppatNew, *ppppat, *pcpat * sizeof(PAT **));
            free(*ppppat);
        }
        *ppppat = pppatNew;
    }

    (*ppppat)[*pcpat] = ppat;
    (*pcpat) += 1;
}

void ParseAddPattern(char * pszIniFile, int iSection, char * pszPattern)
{
    PAT *       ppat;
    UINT *      pcpat;
    PAT **      pppat;

    ppat  = ParsePattern(pszPattern);

    if (ppat == NULL)
    {
        Fatal("File %s:\n  Syntax error in [%s] section:\n  Pattern \"%s\"\n",
              pszIniFile, g_rgpszSect[iSection], pszPattern);
    }

    AddPatternToList(ppat, g_rgcpat[iSection], g_rgpppat[iSection]);
}

void ParseError(char * pszFile, int iSection, char * pszLine)
{
    char * pch = pszLine;

    while (*pch && *pch != '\r' && *pch != '\n')
        ++pch;

    *pch = 0;

    Fatal("File %s:\n  Syntax error in [%s] section:\n  \"%s\"\n",
          pszFile, g_rgpszSect[iSection], pszLine);
}

void ParseIniFile(char * pszFile)
{
    char szBuf[4096];
    char * pch, * pch2, * pch3, *pch4;
    int i;
    DWORD dw;

    for (i = 0; i < 4; ++i)
    {
        szBuf[0] = 0;
        szBuf[1] = 0;
        dw = GetPrivateProfileSection(g_rgpszSect[i], szBuf, sizeof(szBuf), pszFile);

        pch = szBuf;

        while (*pch)
        {
            pch2 = pch;

            while (*pch2 && isspace(*pch2))
                ++pch2;

            if (*pch2 == ';')
            {
                while (*pch2 && *pch2 != '\r' && *pch2 != '\n')
                    ++pch2;
            }

            if (*pch2 == 0 || *pch2 == '\r' || *pch2 == '\n')
            {
                // Blank lines are ok.  Just go on to the next one.
                
                if (*pch2)
                {
                    pch2++;
                    if (*pch2 == '\r' || *pch2 == '\n')
                        pch2++;
                }
                pch = pch2;
            }
            else
            {
                if (CompareString(0, NORM_IGNORECASE, pch2, 7, "pattern", 7) != CSTR_EQUAL)
                    goto parse_error;

                pch2 += 7;

                while (*pch2 && isspace(*pch2))
                    ++pch2;

                if (*pch2++ != '=')
                    goto parse_error;

                while (*pch2 && isspace(*pch2))
                    ++pch2;

                pch3 = pch2;

                while (*pch3 && !isspace(*pch3))
                    ++pch3;

                pch4 = pch3;

                while (*pch4 && isspace(*pch4))
                    ++pch4;

                if (*pch4 == ';')
                {
                    while (*pch4 && *pch4 != '\r' && *pch4 != '\n')
                        ++pch4;
                }

                if (*pch4 && *pch4 != '\r' && *pch4 != '\n')
                    goto parse_error;

                if (pch3 - pch2 == 0)
                    goto parse_error;

                *pch3 = 0;

                ParseAddPattern(pszFile, i, pch2);

                if (*pch4 && (*pch4 == '\r' || *pch4 == '\n'))
                    ++pch4;

                pch = pch4 + 1;
            }
        }
    }

    return;

parse_error:

    ParseError(pszFile, i, pch);

}

void ParseIniFiles()
{
    char szDir[MAX_PATH];
    char szFile[MAX_PATH];
    char * pch;
    UINT i;

    if (!GetCurrentDirectory(sizeof(szDir), szDir))
    {
        Win32Error("ParseIniFiles --> GetCurrentDirectory");
    }

    g_pszCurDir = _strdup(szDir);
    strcpy(szDir, g_pszCurDir);

    while (1)
    {
        strcpy(szFile, szDir);
        strcat(szFile, "\\nuke.ini");

        if (FileExists(szFile))
        {
            g_pszIniDir = _strdup(szDir);
            ParseIniFile(szFile);

            strcpy(szFile, szDir);
            strcat(szFile, "\\nukemore.ini");

            if (FileExists(szFile))
            {
                ParseIniFile(szFile);
            }
            break;
        }

        pch = &szDir[lstrlen(szDir) - 1];

        while (pch >= szDir && *pch != '\\' && *pch != ':')
            *pch-- = 0;

        if (*pch == 0 || *pch == ':')
        {
            Fatal("Can't find nuke.ini in any directory up to the root of the current drive");
        }

        *pch = 0;
    }

    for (i = 0; i < ARRAYSIZE(g_rgpszSect); ++i)
    {
        qsort((void *)(*g_rgpppat[i]), (size_t)(*g_rgcpat[i]), sizeof(PAT **), comparepatspec);
    }

#ifdef DEBUG_PATTERN_MATCH
    DumpAllPatterns();
#endif
}

void SetupArchiveDir()
{
    char szDir[MAX_PATH];
    HANDLE hFind;
    WIN32_FIND_DATA fd;

    strcpy(szDir, g_pszIniDir);
    szDir[2] = 0; // Drive letter and colon only
    strcat(szDir, "\\$nuke");
    g_pszArcDir = _strdup(szDir);

    hFind = FindFirstFile(g_pszArcDir, &fd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        if (GetLastError() != ERROR_FILE_NOT_FOUND)
            Win32Error("SetupArchiveDir --> FindFirstFile");
        if (!CreateDirectory(g_pszArcDir, NULL))
            Win32Error("SetupArchiveDir --> CreateDirectory");
        if (!SetFileAttributes(g_pszArcDir, FILE_ATTRIBUTE_HIDDEN))
            Win32Error("SetupArchiveDir --> SetFileAttributes");
    }
    else
    {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            Fatal("Cannot create archive root because a file with the same\n"
                  "name already exists there.%s", g_pszArcDir);
        }
        FindClose(hFind);
    }
}

void PushScanList(char *** pppszScan, UINT * pcScanAlloc, UINT * pcScan, UINT iIns, char * psz)
{
    psz = _strdup(psz);
    if (psz == NULL)
        Fatal("Out of memory");

    if (*pcScan == *pcScanAlloc)
    {
        char ** ppszNew = (char **)malloc((*pcScanAlloc + 128) * sizeof(char *));
        if (ppszNew == NULL)
            Fatal("Out of memory");
        if (*pcScan)
        {
            memcpy(ppszNew, *pppszScan, *pcScan * sizeof(char *));
            free(*pppszScan);
        }
        *pppszScan = ppszNew;
        *pcScanAlloc += 128;
    }

    if (iIns < *pcScan)
    {
        memmove((*pppszScan) + iIns + 1, (*pppszScan) + iIns, (*pcScan - iIns) * sizeof(char *));
    }

    (*pppszScan)[iIns] = psz;
    (*pcScan) += 1;
}

char * PopScanList(char ** ppszScan, UINT * pcScan)
{
    char * psz = ppszScan[0];

    if (*pcScan > 1)
    {
        memmove(ppszScan, ppszScan + 1, (*pcScan - 1) * sizeof(char *));
    }

    (*pcScan) -= 1;
    
    return(psz);
}

void AddToFileList(char *** pppszFile, UINT * pcFile, char * psz)
{
    psz = _strdup(psz);
    if (psz == NULL)
        Fatal("Out of memory");

    if ((*pcFile % 256) == 0)
    {
        char ** ppszNew = (char **)malloc((*pcFile + 256) * sizeof(char *));
        if (ppszNew == NULL)
            Fatal("Out of memory");
        if (*pcFile)
        {
            memcpy(ppszNew, *pppszFile, *pcFile * sizeof(char *));
            free(*pppszFile);
        }
        *pppszFile = ppszNew;
    }
    (*pppszFile)[*pcFile] = psz;
    (*pcFile) += 1;
}

void GenerateFileList()
{
    HANDLE hFind;
    WIN32_FIND_DATA fd;
    char ** ppszScan = NULL;
    UINT cScanAlloc = 0;
    UINT cScan = 0;
    char * pszDir, * pszFile;
    UINT iDir;
    UINT cPrefix;
    UINT cpat;
    PAT ** ppat;
    char szBuf[MAX_PATH];

    fprintf(stderr, "nuke: %4d files found\r", 0);

    szBuf[0] = 0;

    if (g_fUndo)
    {
        strcpy(szBuf, g_pszArcDir);

        if (g_fStartAtRoot)
            strcat(szBuf, g_pszIniDir + 2);
        else
            strcat(szBuf, g_pszCurDir + 2);

        cPrefix = lstrlen(g_pszArcDir) + lstrlen(g_pszIniDir) - 1;
    }
    else
    {
        if (g_fStartAtRoot)
            strcpy(szBuf, g_pszIniDir);
        else
            strcpy(szBuf, g_pszCurDir);

        cPrefix = lstrlen(g_pszIniDir) + 1;
    }

    strcat(szBuf, "\\*");

    PushScanList(&ppszScan, &cScanAlloc, &cScan, 0, szBuf);

    while (cScan > 0)
    {
        pszDir = PopScanList(ppszScan, &cScan);
        iDir = 0;

        hFind = FindFirstFile(pszDir, &fd);

        if (hFind == INVALID_HANDLE_VALUE)
        {
            if (g_fUndo && GetLastError() == ERROR_PATH_NOT_FOUND)
                continue;
            Win32Error("GenerateFileList --> FindFirstFile");
        }

        do
        {
            if (fd.dwFileAttributes & (FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN))
                continue;

            strcpy(szBuf, pszDir);
            szBuf[lstrlen(szBuf) - 1] = 0;
            strcat(szBuf, fd.cFileName);

            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (lstrcmpi(szBuf, g_pszArcDir) == 0)
                    continue;
                if (lstrcmp(fd.cFileName, ".") == 0)
                    continue;
                if (lstrcmp(fd.cFileName, "..") == 0)
                    continue;

                strcat(szBuf, "\\*");

                PushScanList(&ppszScan, &cScanAlloc, &cScan, iDir, szBuf);
                iDir += 1;
            }
            else
            {
                if (g_fUndo)
                {
                    for (cpat = g_cpatUndo, ppat = g_rgppatUndo; cpat > 0; --cpat, ++ppat)
                    {
                        if (MatchItem(&(*ppat)->rgpe[0], fd.cFileName))
                            break;
                    }

                    if (cpat == 0)
                        continue;
                }

                AddToFileList(&g_rgpszFile, &g_cFile, szBuf + cPrefix);
                fprintf(stderr, "nuke: %4d file%s found\r", g_cFile, g_cFile == 1 ? "" : "s");
            }
        }
        while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }

    fprintf(stderr, "nuke: %4ld file%s found \n", g_cFile, g_cFile == 1 ? "" : "s");
}

BOOL MatchWild(char * pszPat, char * pszItem)
{
    while (*pszPat)
    {
        if (*pszPat == '*')
        {
            ++pszPat;

            if (*pszPat == 0)
                return(TRUE);
            else if (*pszItem == 0)
                return(FALSE);

            while (*pszItem)
            {
                if (MatchWild(pszPat, pszItem))
                    return(TRUE);
                ++pszItem;
            }
            return(FALSE);
        }
        else if (*pszItem == 0)
            return(FALSE);
        else if (_tolower(*pszPat) != _tolower(*pszItem))
            return(FALSE);

        ++pszPat;
        ++pszItem;
    }

    return(*pszPat == 0 && *pszItem == 0);
}

BOOL MatchItem(PATENTRY * ppe, char * psz)
{
    if (ppe->dwFlags & PATENTRYF_ALLWILD)
        return(TRUE);

    if (ppe->dwFlags & PATENTRYF_DIRWILD)
        Fatal("Internal error (DIRWILD)");

    if (ppe->dwFlags & PATENTRYF_HASWILD)
        return(MatchWild(ppe->psz, psz));

    return(lstrcmpi(ppe->psz, psz) == 0);
}

BOOL MatchPattern(PAT * ppat, PATHINFO * ppi)
{
    PATENTRY * ppe;
    char ** ppsz;
    int i;

    if ((ppat->dwFlags & PATF_HASDIRWILD) == 0)
    {
        if (ppi->cEnt != ppat->cEnt)
            return(FALSE);
    }
    else if (ppi->cEnt < ppat->cEnt - 1)
    {
        return(FALSE);
    }

    // Match filenames first

    if (!MatchItem(&ppat->rgpe[ppat->cEnt - 1], ppi->rgpsz[ppi->cEnt - 1]))
        return(FALSE);

    // Now match directory paths starting from the end.  If we encounter a DIRWILD
    // entry, we just break out and continue scaning from the beginning.

    i    = (int)ppat->cEnt - 2;
    ppe  = &ppat->rgpe[ppat->cEnt - 2];
    ppsz = &ppi->rgpsz[ppi->cEnt - 2];

    for (; i >= 0; --i, --ppe, --ppsz)
    {
        if (ppe->dwFlags & PATENTRYF_DIRWILD)
        {
            int iStop = i;

            i    = 0;
            ppe  = &ppat->rgpe[0];
            ppsz = &ppi->rgpsz[0];

            for (; i < iStop; ++i, ++ppe, ++ppsz)
            {
                if (!MatchItem(ppe, *ppsz))
                    return(FALSE);
            }
        }
        else if (!MatchItem(ppe, *ppsz))
            return(FALSE);
    }

    return(TRUE);
}

PAT * FindPattern(UINT cpat, PAT ** pppat, PATHINFO * ppi)
{
    PAT * ppat;

    for (; cpat > 0; --cpat, ++pppat)
    {
        ppat = *pppat;

        if (MatchPattern(ppat, ppi))
            return(ppat);
    }

    return(NULL);
}

BOOL IsMoreSpecific(PAT * ppat1, PAT * ppat2)
{
    // Is ppat1 more specific than ppat2?

    if (ppat1 == NULL)
        return(FALSE);
    else if (ppat2 == NULL)
        return(TRUE);
    else if (ppat1->cSpec < ppat2->cSpec)
        return(FALSE);
    else if (ppat1->cSpec > ppat2->cSpec)
        return(TRUE);
    else if (ppat1->cWild >= ppat2->cWild)
        return(FALSE);
    else
        return(TRUE);
}

void ClassifyFileList()
{
    PATHINFO *  ppi;
    PAT *       ppatDel;
    PAT *       ppatNoDel;
    PAT *       ppatArc;
    PAT *       ppatNoArc;
    UINT        iFile;
    char *      pszFile;
    char        szBuf[MAX_PATH];

    for (iFile = 0; iFile < g_cFile; ++iFile)
    {
        pszFile = g_rgpszFile[iFile];

        strcpy(szBuf, pszFile);

        ppi = ParsePath(szBuf);

        ppatDel   = FindPattern(g_cpatDel, g_rgppatDel, ppi);
        ppatNoDel = FindPattern(g_cpatNoDel, g_rgppatNoDel, ppi);
        ppatArc   = FindPattern(g_cpatArc, g_rgppatArc, ppi);
        ppatNoArc = FindPattern(g_cpatNoArc, g_rgppatNoArc, ppi);

        free(ppi);

#ifdef DEBUG_PATTERN_MATCH
        printf("%s\n", pszFile);
        if (ppatDel)
            printf("  Del   = %2d.%d %s\n", ppatDel->cSpec, ppatDel->cWild, ppatDel->psz);
        if (ppatNoDel)
            printf("  NoDel = %2d.%d %s\n", ppatNoDel->cSpec, ppatNoDel->cWild, ppatNoDel->psz);
        if (ppatArc)
            printf("  Arc   = %2d.%d %s\n", ppatArc->cSpec, ppatArc->cWild, ppatArc->psz);
        if (ppatNoArc)
            printf("  NoArc = %2d.%d %s\n", ppatNoArc->cSpec, ppatNoArc->cWild, ppatNoArc->psz);
#endif

        if (ppatDel && !IsMoreSpecific(ppatNoDel, ppatDel) && !IsMoreSpecific(ppatArc, ppatDel))
        {
            AddToFileList(&g_rgpszDel, &g_cDel, pszFile);
        }
        else if (ppatArc && !IsMoreSpecific(ppatNoArc, ppatArc))
        {
            AddToFileList(&g_rgpszArc, &g_cArc, pszFile);
        }
        else
        {
            AddToFileList(&g_rgpszRem, &g_cRem, pszFile);
        }
    }
}

void DisplayOperations()
{
    UINT i;

    printf("nuke: %4d file%s would be deleted\n", g_cDel, g_cDel == 1 ? "" : "s");
    printf("nuke: %4d file%s would be archived\n", g_cArc, g_cArc == 1 ? "" : "s");
    printf("nuke: %4d file%s would remain\n", g_cRem, g_cRem == 1 ? "" : "s");

    if (g_fDisplayDeletes && g_cDel > 0)
    {
        printf("\n");
        for (i = 0; i < g_cDel; ++i)
        {
            printf("[del] %s\\%s\n", g_pszIniDir, g_rgpszDel[i]);
        }
    }

    if (g_fDisplayArchives && g_cArc > 0)
    {
        printf("\n");
        for (i = 0; i < g_cArc; ++i)
        {
            printf("[arc] %s\\%s\n", g_pszIniDir, g_rgpszArc[i]);
        }
    }

    if (g_fDisplayRemain && g_cRem > 0)
    {
        printf("\n");
        for (i = 0; i < g_cRem; ++i)
        {
            printf("[rem] %s\\%s\n", g_pszIniDir, g_rgpszRem[i]);
        }
    }
}

BOOL ArchiveFile(char * psz)
{
    char szBufSrc[MAX_PATH];
    char szBufDst[MAX_PATH];

    strcpy(szBufSrc, g_pszIniDir);
    strcat(szBufSrc, "\\");
    strcat(szBufSrc, psz);

    strcpy(szBufDst, g_pszArcDir);
    strcat(szBufDst, szBufSrc + 2);

    if (!MoveFileEx(szBufSrc, szBufDst, MOVEFILE_REPLACE_EXISTING))
    {
        PATHINFO *  ppi;
        char        szBuf1[MAX_PATH];
        char        szBuf2[MAX_PATH];
        int         i;

        if (GetLastError() != ERROR_PATH_NOT_FOUND)
        {
            char szErr[512];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szErr, sizeof(szErr), NULL);
            fprintf(stderr, "                                           \n");
            fprintf(stderr, "nuke: warning: Could not archive %s\n", szBufSrc);
            fprintf(stderr, "               %s\n", szErr);
            return(FALSE);
        }

        strcpy(szBuf1, szBufDst);
        ppi = ParsePath(szBuf1);

        strcpy(szBuf2, ppi->rgpsz[0]);

        for (i = 1; i < (int)ppi->cEnt - 1; ++i)
        {
            strcat(szBuf2, "\\");
            strcat(szBuf2, ppi->rgpsz[i]);

            if (!CreateDirectory(szBuf2, NULL))
            {
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                    Win32Error("ArchiveFile --> CreateDirectory");
            }
        }

        if (!MoveFileEx(szBufSrc, szBufDst, MOVEFILE_REPLACE_EXISTING))
        {
            char szErr[512];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szErr, sizeof(szErr), NULL);
            fprintf(stderr, "                                           \n");
            fprintf(stderr, "nuke: warning: Could not archive %s\n", szBufSrc);
            fprintf(stderr, "               %s\n", szErr);
            return(FALSE);
        }
    }

    return(TRUE);
}

void ExecuteOperations()
{
    char * psz;
    char szBuf[MAX_PATH];
    UINT i;
    UINT iDel = 0;
    UINT iArc = 0;

    if (!g_fVerbose)
        fprintf(stderr, "nuke: %4d files deleted \r", 0);

    for (i = 0; i < g_cDel; ++i)
    {
        strcpy(szBuf, g_pszIniDir);
        strcat(szBuf, "\\");
        strcat(szBuf, g_rgpszDel[i]);
        
        if (!DeleteFile(szBuf))
        {
            char szErr[512];
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, szErr, sizeof(szErr), NULL);
            fprintf(stderr, "                                   \n");
            fprintf(stderr, "nuke: warning: Could not delete %s\n", szBuf);
            fprintf(stderr, "               %s\n", szErr);
            AddToFileList(&g_rgpszRem, &g_cRem, g_rgpszDel[i]);
        }
        else if (g_fVerbose)
        {
            iDel += 1;
            if (i == 0)
                printf("\n");
            printf("[del] %s\n", szBuf);
        }
        else
        {
            iDel += 1;
            fprintf(stderr, "nuke: %4d file%s deleted \r", iDel, iDel == 1 ? "" : "s");
        }
    }

    if (!g_fVerbose)
    {
        fprintf(stderr, "nuke: %4d file%s deleted \n", iDel, iDel == 1 ? "" : "s");
        fprintf(stderr, "nuke: %4d files archived \r", 0);
    }

    for (i = 0; i < g_cArc; ++i)
    {
        if (ArchiveFile(g_rgpszArc[i]))
        {
            iArc += 1;

            if (g_fVerbose)
            {
                if (i == 0)
                    printf("\n");
                printf("[arc] %s\\%s\n", g_pszIniDir, g_rgpszArc[i]);
            }
            else
            {
                fprintf(stderr, "nuke: %4d file%s archived \r", iArc, iArc == 1 ? "" : "s");
            }
        }
        else
        {
            AddToFileList(&g_rgpszRem, &g_cRem, g_rgpszArc[i]);
        }
    }

    if (g_fVerbose)
    {
        if (iDel || iArc)
            printf("\n");
        fprintf(stderr, "nuke: %4d file%s deleted    \n", iDel, iDel == 1 ? "" : "s");
    }
    fprintf(stderr, "nuke: %4d file%s archived \n", iArc, iArc == 1 ? "" : "s");
    fprintf(stderr, "nuke: %4d file%s remain%s \n", g_cRem, g_cRem == 1 ? "" : "s", g_cRem == 1 ? "s" : "");

    if (g_fDisplayRemain && g_cRem > 0)
    {
        printf("\n");
        for (i = 0; i < g_cRem; ++i)
        {
            printf("[rem] %s\\%s\n", g_pszIniDir, g_rgpszRem[i]);
        }
    }
}

void RestoreFromArchive()
{
    UINT i;
    char * psz;
    char szBufSrc[MAX_PATH];
    char szBufDst[MAX_PATH];
    BOOL fDisp = FALSE;

    for (i = 0; i < g_cFile; ++i)
    {
        psz = g_rgpszFile[i];

        strcpy(szBufSrc, g_pszArcDir);
        strcat(szBufSrc, g_pszIniDir + 2);
        strcat(szBufSrc, "\\");
        strcat(szBufSrc, psz);

        strcpy(szBufDst, g_pszIniDir);
        strcat(szBufDst, "\\");
        strcat(szBufDst, psz);

        if (FileExists(szBufDst))
            AddToFileList(&g_rgpszRem, &g_cRem, szBufDst);
        else
        {
            if (!g_fDisplayOnly)
            {
                if (!MoveFileEx(szBufSrc, szBufDst, 0))
                {
                    Win32Error("RestoreFromArchive --> MoveFileEx");
                }
            }

            if (g_fDisplayRestores)
            {
                if (i == 0)
                    printf("\n");
                printf("[restored] %s\n", szBufDst);
                fDisp = TRUE;
            }
        }
    }
    
    if (g_cRem > 0 && g_fDisplayRestores)
    {
        printf("\n");

        for (i = 0; i < g_cRem; ++i)
            printf("[NOCLOBBER] %s\n", g_rgpszRem[i]);

        fDisp = TRUE;
    }

    if (fDisp)
        printf("\n");

    printf("nuke: %4d file%s %srestored \n",
           g_cFile - g_cRem, g_cFile - g_cRem == 1 ? "" : "s", g_fDisplayOnly ? "would be " : "");

    if (g_cRem > 0)
    {
        printf("nuke: %4d file%s %s clobbered\n",
               g_cRem, g_cRem == 1 ? "" : "s", g_fDisplayOnly ? "would not be" : "not");
    }
}

int __cdecl main(int argc, char * argv[])
{
    int i;

    for (i = 1; i < argc; ++i)
    {
        char * pch = argv[i];

        if (lstrcmp(pch, "...") == 0)
            g_fStartAtRoot = FALSE;
        else if (*pch == '-')
        {
            ++pch;

            if (*pch == 0 || pch[1] != 0)
                Usage();

            if (*pch == 'n')
                g_fDisplayOnly = TRUE;
            else if (*pch == 'v')
            {
                g_fVerbose = TRUE;
                g_fDisplayDeletes = TRUE;
                g_fDisplayArchives = TRUE;
                g_fDisplayRemain = TRUE;
                g_fDisplayRestores = TRUE;
            }
            else if (*pch == 'u')
            {
                g_fUndo = TRUE;
            }
            else
                Usage();
        }
        else if (g_fUndo)
        {
            PAT * ppat = ParsePattern(pch);

            if (ppat == NULL || ppat->cEnt > 1)
                Fatal("Syntax error in undo pattern - \"%s\"", pch);

            AddPatternToList(ppat, &g_cpatUndo, &g_rgppatUndo);
        }
        else
            Usage();
    }

    if (g_fUndo && g_cpatUndo == 0)
    {
        AddPatternToList(ParsePattern("*"), &g_cpatUndo, &g_rgppatUndo);
    }

    ParseIniFiles();
    SetupArchiveDir();
    GenerateFileList();
    
    if (g_fUndo)
    {
        RestoreFromArchive();
    }
    else
    {
        ClassifyFileList();

        if (g_fDisplayOnly)
            DisplayOperations();
        else
            ExecuteOperations();
    }

    return(0);
}
