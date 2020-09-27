#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dbghelp.h>

void Usage()
{
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage: mapdiff [-v] <mapfile1.map> { <mapfile2.map> }\n\n");
    fprintf(stderr, "  mapdiff reads a .map file and outputs a friendly report\n");
    fprintf(stderr, "  showing the sizes of sections and symbols.\n\n");
    fprintf(stderr, "  When two .map files are given, mapdiff computes a delta\n");
    fprintf(stderr, "  between the two and reports on the differences.  This is\n");
    fprintf(stderr, "  useful when comparing a new version of a program to a previous\n");
    fprintf(stderr, "  version to see the effect of the changes.  Only sections and\n");
    fprintf(stderr, "  symbols which have changed size are shown by default.  Use\n");
    fprintf(stderr, "  the -v switch to force all sections and symbols to be shown.\n");
    exit(1);
}

typedef struct SECINFO {
    UINT        iSeg;               // Segment number of section
    DWORD       dwOffset;           // Offset into segment where section begins
    DWORD       dwLength;           // Length of section
    char *      pszName;            // Name of section
    BOOL        fIsData;            // TRUE if this is a data section
} SECINFO;

typedef struct SECDIFF {
    SECINFO *   psi1;               // Pointer to section in first map (may be NULL)
    SECINFO *   psi2;               // Pointer to section in second map (may be NULL)
} SECDIFF;

typedef struct FUNINFO {
    UINT        iSeg;               // Segment number of function
    DWORD       dwOffset;           // Offset into segment where function begins
    DWORD       dwLength;           // Length of function
    DWORD       dwAddress;          // Absolute address of function
    char *      pszName;            // Name of function
    char *      pszModule;          // Name of lib:object containing function
    SECINFO *   psi;                // The section that this function belongs to
    BOOL        fIsStatic;          // TRUE if static function
} FUNINFO;

typedef struct FUNDIFF {
    FUNINFO *   pfi1;               // Pointer to function in first map (may be NULL)
    FUNINFO *   pfi2;               // Pointer to function in second map (may be NULL)
} FUNDIFF;

typedef struct MAPINFO {
    char *      pszFile;            // File name of map file
    char *      pszName;            // Map name from first line of map file
    char *      pszTimestamp;       // Timestamp for map file
    DWORD       dwBase;             // Preferred load address
    UINT        iSegEntry;          // Entry point segment
    DWORD       dwEntry;            // Entry point offset
    DWORD       cbSecCode;          // Size of all code sections
    DWORD       cbSecData;          // Size of all data sections
    DWORD       cbFunCode;          // Size of all functions in code sections
    DWORD       cbFunData;          // Size of all functions in data sections
    UINT        cSecInfo;           // Count of sections in section array
    UINT        cSecCode;           // Count of sections of code
    DWORD       cSecData;           // Count of sections of data
    SECINFO **  ppSecInfo;          // Array of sections
    UINT        cFunInfo;           // Count of functions in function array
    UINT        cFunCode;           // Count of functions in code sections
    UINT        cFunData;           // Count of functions in data sections
    FUNINFO **  ppFunInfo;          // Array of functions
} MAPINFO;

static DWORD dwStatic = 0;

char *
Undecorate(char * pszName)
{
    char szOrig[512];
    char szUndec[512];

    memset(szOrig, 0, sizeof(szOrig));
    memset(szUndec, 0, sizeof(szUndec));
    
    strcpy(szOrig, pszName);

    UnDecorateSymbolName(pszName, szUndec, sizeof(szUndec), 
                         UNDNAME_NAME_ONLY | UNDNAME_NO_ALLOCATION_MODEL | UNDNAME_NO_ALLOCATION_LANGUAGE);

    if (szUndec[0] && !lstrcmpi(szUndec, "`string'") == 0)
    {
        char * pch = szUndec;
        while (*pch && !isspace(*pch)) ++pch;
        *pch = 0;
        return(_strdup(szUndec));
    }

    return(pszName);
}

void *
MemAlloc(size_t size, BOOL fZero)
{
    void * pv = malloc(size);

    if (pv == NULL)
    {
        fprintf(stderr, "Out of memory allocating %d bytes\n", size);
        exit(1);
    }

    memset(pv, 0, size);

    return(pv);
}

void
ReadTextFile(char * pszFile, UINT * pcLines, char *** pppszLines)
{
    HANDLE  hFile;
    DWORD   dwSize;
    DWORD   dwRead;
    char *  pchBuf;
    char *  pch;
    char *  pchEnd;
    char ** ppszLines;
    UINT    cLines;

    hFile = CreateFile(pszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error opening '%s' (%d)\n", pszFile, GetLastError());
        exit(1);
    }
    
    dwSize = GetFileSize(hFile, NULL);

    if (dwSize == 0xFFFFFFFF)
    {
        fprintf(stderr, "Error getting size of '%s' (%d)\n", pszFile, GetLastError());
        exit(1);
    }

    pchBuf = MemAlloc(dwSize + 1, FALSE);

    if (ReadFile(hFile, pchBuf, dwSize, &dwRead, NULL) == 0)
    {
        fprintf(stderr, "Error reading contents of '%s' (%d)\n", pszFile, GetLastError());
        exit(1);
    }

    CloseHandle(hFile);

    // Scan the counting the number of lines.  The last line might not end with a
    // '\r\n', so the minimum number of lines will always be one.

    cLines = 1;
    pch = pchBuf;
    pchEnd = pchBuf + dwSize;

    for (; pch < pchEnd; ++pch)
    {
        if (*pch == '\n' || *pch == '\r')
        {
            cLines += 1;

            // Allow '\n', '\r\n', or '\n\r'

            if (    (pch + 1) < pchEnd
                &&  (   (*pch == '\n' && *(pch + 1) == '\r')
                     || (*pch == '\r' && *(pch + 1) == '\n')))
            {
                ++pch;
            }
        }
    }

    *pcLines = cLines;
    ppszLines = *pppszLines = MemAlloc(cLines * sizeof(char *), FALSE);

    pch = pchBuf;
    *ppszLines++ = pch;

    for (; pch < pchEnd; ++pch)
    {
        if (*pch == '\n' || *pch == '\r')
        {
            // Allow '\n', '\r\n', or '\n\r'

            if (    (pch + 1) < pchEnd
                &&  (   (*pch == '\n' && *(pch + 1) == '\r')
                     || (*pch == '\r' && *(pch + 1) == '\n')))
            {
                *pch = 0;
                *++pch = 0;
            }
            else
            {
                *pch = 0;
            }

            if ((pch + 1) <= pchEnd)
            {
                *ppszLines++ = pch + 1;
            }
        }
    }

    *pchEnd = 0;

    if (ppszLines - *pppszLines != (int)cLines)
    {
        fprintf(stderr, "Assert: ReadTextFile screwed up the line array\n");
        exit(1);
    }
}

static void
ParseError(char * pszFile, UINT iLine, char * pszError)
{
    fprintf(stderr, "Error parsing map file '%s':\n", pszFile);
    fprintf(stderr, "  Line %d: %s\n", iLine + 1, pszError);
    exit(1);
}

char *
skipspace(char * psz)
{
    while (*psz && isspace(*psz))
        ++psz;
    return(psz);
}

char *
skipnonspace(char * psz)
{
    while (*psz && !isspace(*psz))
        ++psz;
    return(psz);
}

char *
skiptochar(char * psz, char c)
{
    while (*psz && *psz != c)
        ++psz;
    return(psz);
}

char *
prefixmatch(char * psz, char * pszPrefix)
{
    int len1 = lstrlen(pszPrefix);
    int len2 = lstrlen(psz);

    if (len1 <= len2 && memcmp(pszPrefix, psz, len1) == 0)
    {
        return(psz + len1);
    }

    return(NULL);
}

DWORD
parsehex(char * psz)
{
    DWORD dw = 0;

    for (; *psz && !isspace(*psz); ++psz)
    {
        if (*psz >= '0' && *psz <= '9')
        {
            dw = (dw << 4) | (*psz - '0');
        }
        else if (*psz >= 'A' && *psz <= 'F')
        {
            dw = (dw << 4) | ((*psz - 'A') + 10);
        }
        else if (*psz >= 'a' && *psz <= 'f')
        {
            dw = (dw << 4) | ((*psz - 'a') + 10);
        }
        else
        {
            break;
        }
    }

    return(dw);
}

char *
nextline(char * pszFile, UINT * piLine, UINT * pcLines, char *** pppsz, BOOL fEofOk)
{
    if (*piLine >= *pcLines || (*piLine == *pcLines && !fEofOk))
    {
        ParseError(pszFile, *piLine, "unexpected enf of file");
    }

    if (*piLine == *pcLines)
        return(NULL);

    *piLine += 1;
    *pppsz += 1;
    return(**pppsz);
}

char *
skipblanklines(char * pszFile, UINT * piLine, UINT * pcLines, char *** pppsz, BOOL fEofOk)
{
    do
    {
        *piLine += 1;
        *pppsz += 1;

        if (*piLine >= *pcLines)
        {
            if (fEofOk)
            {
                return(NULL);
            }

            ParseError(pszFile, *piLine, "unexpected end of file");
        }
    }
    while (*skipspace(**pppsz) == 0);
        
    return(**pppsz);
}

int __cdecl
comparefuninfosegoffset(const void * pv1, const void * pv2)
{
    FUNINFO * pfi1 = *(FUNINFO **)pv1;
    FUNINFO * pfi2 = *(FUNINFO **)pv2;

    if (pfi1->iSeg < pfi2->iSeg)
        return(-1);
    else if (pfi1->iSeg > pfi2->iSeg)
        return(+1);
    else if (pfi1->dwOffset < pfi2->dwOffset)
        return(-1);
    else if (pfi1->dwOffset > pfi2->dwOffset)
        return(+1);
    else
        return(0);
}

int __cdecl
comparefuninfoname(const void * pv1, const void * pv2)
{
    FUNINFO * pfi1 = *(FUNINFO **)pv1;
    FUNINFO * pfi2 = *(FUNINFO **)pv2;
    int i;

    i = lstrcmpi(pfi1->pszName, pfi2->pszName);

    if (i == 0)
    {
        i = lstrcmpi(pfi1->pszModule, pfi2->pszModule);

        if (i == 0)
        {
            i = lstrcmpi(pfi1->psi->pszName, pfi2->psi->pszName);

            if (i == 0)
            {
                i = comparefuninfosegoffset(pv1, pv2);
            }
        }
    }

    return(i);
}

int __cdecl
comparefuninfodiff(FUNINFO * pfi1, FUNINFO * pfi2)
{
    if (pfi2 == NULL)
        return(-1);
    else if (pfi1 == NULL)
        return(+1);
    else
        return(lstrcmpi(pfi1->pszName, pfi2->pszName));
}

int __cdecl
comparefundiff(const void * pv1, const void * pv2)
{
    FUNDIFF * pfd1 = (FUNDIFF *)pv1;
    FUNDIFF * pfd2 = (FUNDIFF *)pv2;
    char * psz1;
    char * psz2;
    int iCmp = 0;

    // Sort first by the segment name of the first map

    psz1 = pfd1->pfi1 ? pfd1->pfi1->psi->pszName : "";
    psz2 = pfd2->pfi1 ? pfd2->pfi1->psi->pszName : "";
    iCmp = lstrcmpi(psz1, psz2);

    if (iCmp == 0)
    {
        // Sort next by the segment name of the second map
        
        psz1 = pfd1->pfi2 ? pfd1->pfi2->psi->pszName : "";
        psz2 = pfd2->pfi2 ? pfd2->pfi2->psi->pszName : "";
        iCmp = lstrcmpi(psz1, psz2);

        if (iCmp == 0)
        {
            // Sort by the name of the function

            FUNINFO * pfi1 = pfd1->pfi1 ? pfd1->pfi1 : pfd1->pfi2;
            FUNINFO * pfi2 = pfd2->pfi1 ? pfd2->pfi1 : pfd2->pfi2;

            iCmp = comparefuninfoname(&pfi1, &pfi2);
        }
    }

    return(iCmp);
}

int __cdecl
comparesecinfosegoffset(const void * pv1, const void * pv2)
{
    SECINFO * psi1 = *(SECINFO **)pv1;
    SECINFO * psi2 = *(SECINFO **)pv2;

    if (psi1->iSeg < psi2->iSeg)
        return(-1);
    else if (psi1->iSeg > psi2->iSeg)
        return(+1);
    else if (psi1->dwOffset < psi2->dwOffset)
        return(-1);
    else if (psi1->dwOffset > psi2->dwOffset)
        return(+1);
    else
        return(0);
}

int __cdecl
comparesecinfoname(const void * pv1, const void * pv2)
{
    SECINFO * psi1 = *(SECINFO **)pv1;
    SECINFO * psi2 = *(SECINFO **)pv2;
    int i;

    i = lstrcmpi(psi1->pszName, psi2->pszName);

    if (i == 0)
    {
        i = comparesecinfosegoffset(pv1, pv2);
    }

    return(i);
}

int __cdecl
comparesecinfodiff(SECINFO * psi1, SECINFO * psi2)
{
    if (psi2 == NULL)
        return(-1);
    else if (psi1 == NULL)
        return(+1);
    else
        return(lstrcmpi(psi1->pszName, psi2->pszName));
}

int __cdecl
comparesecdiff(const void * pv1, const void * pv2)
{
    SECDIFF * psd1 = (SECDIFF *)pv1;
    SECDIFF * psd2 = (SECDIFF *)pv2;
    SECINFO * psi1 = psd1->psi1 ? psd1->psi1 : psd1->psi2;
    SECINFO * psi2 = psd2->psi1 ? psd2->psi1 : psd2->psi2;
    
    return(comparesecinfoname(&psi1, &psi2));
}

char *
commaize(char * pszBuf, LONG l)
{
    int cch = sprintf(pszBuf, "%ld", l);
    int n = (cch - 1 - (l < 0)) / 3;
    int i;
    char * psz;

    for (i = 0; i < n; ++i)
    {
        psz = &pszBuf[cch - (4 * i) - 3];
        memmove(psz + 1, psz, (4 * i) + 3 + 1);
        *psz = ',';
        cch += 1;
    }

    return(pszBuf);
}    

void
MapInfoAddSecFun(MAPINFO * pmi, UINT * pcCount, void *** pppv, void * pvItem, int cbItem)
{
    void * pvCopy = MemAlloc(cbItem, FALSE);
    memcpy(pvCopy, pvItem, cbItem);

    if ((*pcCount % 100) == 0)
    {
        void ** ppvNew = MemAlloc((*pcCount + 100) * sizeof(void *), FALSE);
        memcpy(ppvNew, *pppv, (*pcCount) * sizeof(void *));
        *pppv = ppvNew;
    }

    (*pppv)[(*pcCount)++] = pvCopy;
}

MAPINFO *
MapRead(char * pszFile)
{
    UINT        cLines;
    char **     ppszLines;
    UINT        iLine;
    char **     ppsz;
    char *      psz;
    char *      psz2;
    MAPINFO *   pmi;
    SECINFO     si;
    FUNINFO     fi;
    SECINFO *   psi;
    SECINFO *   psiNext;
    FUNINFO *   pfi;
    FUNINFO *   pfiNext;
    UINT        ifi;
    UINT        isi;
    BOOL        fIsStatic;

    ReadTextFile(pszFile, &cLines, &ppszLines);

    pmi = MemAlloc(sizeof(MAPINFO), TRUE);

    pmi->pszFile = _strdup(pszFile);

    iLine = 0;
    ppsz = ppszLines;

    if (cLines == 0)
    {
        ParseError(pszFile, 0, "file is empty");
    }

    psz = skipspace(*ppsz);
    pmi->pszName = psz;
    psz = skipnonspace(psz);
    *psz = 0;
    if (psz == pmi->pszName || *skipspace(psz) != 0)
    {
        ParseError(pszFile, iLine, "expected single token on first line");
    }

    skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

    psz = skipspace(*ppsz);

    if (!prefixmatch(psz, "Timestamp is "))
    {
        ParseError(pszFile, iLine, "can't find timestamp");
    }

    psz  = skiptochar(psz, '(');
    psz2 = skiptochar(psz, ')');
    if (*psz != '(' || *psz2 != ')')
    {
        ParseError(pszFile, iLine, "can't find timestamp date string");
    }
    
    pmi->pszTimestamp = psz + 1;
    *psz2 = 0;

    skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

    psz = prefixmatch(skipspace(*ppsz), "Preferred load address is ");
    if (psz == NULL)
    {
        ParseError(pszFile, iLine, "can't find preferred load address");
    }
    
    pmi->dwBase = parsehex(psz);

    skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

    if (!prefixmatch(skipspace(*ppsz), "Start         Length     Name                   Class"))
    {
        ParseError(pszFile, iLine, "can't find start of section map");
    }

    psz = skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

    while (1)
    {
        psz = skipspace(psz);
        if (*psz == 0)
            break;

        // printf("'%s'\n", psz);
        // 0001:00000000 0000040cH .rdata                  CODE

        memset(&si, 0, sizeof(si));

        psz2 = skiptochar(psz, ':');

        if (psz2 - psz != 4)
        {
            ParseError(pszFile, iLine, "malformed section map line (segnum)");
        }

        *psz2 = 0;

        si.iSeg = atoi(psz);

        psz += 5;
        psz2 = skipnonspace(psz);

        if (psz2 - psz != 8)
        {
            ParseError(pszFile, iLine, "malformed section map line (offset)");
        }

        si.dwOffset = parsehex(psz);

        psz = skipspace(psz2);
        psz2 = skiptochar(psz, 'H');
        if (*psz2 != 'H' || psz2 - psz != 8)
        {
            ParseError(pszFile, iLine, "malformed section map line (length)");
        }

        si.dwLength = parsehex(psz);

        psz = skipspace(psz2 + 1);
        psz2 = skipnonspace(psz);

        if (psz == psz2)
        {
            ParseError(pszFile, iLine, "malformed section map line (name)");
        }

        si.pszName = psz;

        psz = skipspace(psz2);
        *psz2 = 0;
        psz2 = skipnonspace(psz);

        if (psz == psz2)
        {
            ParseError(pszFile, iLine, "malformed section map line (class)");
        }

        *psz2 = 0;

        if (lstrcmpi(psz, "DATA") == 0)
        {
            si.fIsData = TRUE;
            pmi->cSecData += 1;
        }
        else
        {
            pmi->cSecCode += 1;
        }

        MapInfoAddSecFun(pmi, &pmi->cSecInfo, &pmi->ppSecInfo, &si, sizeof(SECINFO));

        psz = nextline(pszFile, &iLine, &cLines, &ppsz, FALSE);
    }

    psz = skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

    if (!prefixmatch(skipspace(*ppsz), "Address         Publics by Value              Rva+Base     Lib:Object"))
    {
        ParseError(pszFile, iLine, "can't find start of publics map");
    }

    psz = skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

    fIsStatic = FALSE;

    while (1)
    {
        while (1)
        {
            psz = skipspace(psz);
            if (*psz == 0)
                break;

            // printf("'%s'\n", psz);
            // 0001:00001518       _memset                    00402518 f   msvcrtd:MSVCRTD.dll
            // 0002:00000000       ___xc_a                    00403000     msvcrtd:cinitexe.obj

            memset(&fi, 0, sizeof(fi));

            fi.fIsStatic = fIsStatic;

            psz2 = skiptochar(psz, ':');

            if (psz2 - psz != 4)
            {
                ParseError(pszFile, iLine, "malformed publics map line (segnum)");
            }

            *psz2 = 0;

            fi.iSeg = atoi(psz);

            psz += 5;
            psz2 = skipnonspace(psz);

            if (psz2 - psz != 8)
            {
                ParseError(pszFile, iLine, "malformed publics map line (offset)");
            }

            fi.dwOffset = parsehex(psz);

            psz = skipspace(psz2);
            psz2 = skipnonspace(psz);

            if (psz == psz2)
            {
                ParseError(pszFile, iLine, "malformed publics map line (name)");
            }

            fi.pszName = Undecorate(psz);

            psz = skipspace(psz2);
            *psz2 = 0;
            psz2 = skipnonspace(psz);

            if (psz2 - psz != 8)
            {
                ParseError(pszFile, iLine, "malformed publics map line (address)");
            }

            fi.dwAddress = parsehex(psz);

            while (1)
            {
                psz = skipspace(psz2);
                psz2 = skipnonspace(psz);

                if (*skipspace(psz2) == 0)
                    break;
            }

            if (psz == psz2)
            {
                ParseError(pszFile, iLine, "malformed publics map line (lib:obj)");
            }

            fi.pszModule = psz;
            *psz2 = 0;

            MapInfoAddSecFun(pmi, &pmi->cFunInfo, &pmi->ppFunInfo, &fi, sizeof(FUNINFO));

            psz = nextline(pszFile, &iLine, &cLines, &ppsz, FALSE);
        }

        if (fIsStatic)
            break;

        fIsStatic = TRUE;

        psz = skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);
        psz = prefixmatch(skipspace(psz), "entry point at");

        if (psz == NULL)
        {
            ParseError(pszFile, iLine, "can't find entry point line");
        }

        psz = skipspace(psz);
        psz2 = skiptochar(psz, ':');

        if (psz2 - psz != 4 || *psz2 != ':')
        {
            ParseError(pszFile, iLine, "malformed entry point line (segnum)");
        }

        *psz2 = 0;

        pmi->iSegEntry = atoi(psz);

        psz += 5;
        psz2 = skipnonspace(psz);

        if (psz2 - psz != 8 || *skipspace(psz2) != 0)
        {
            ParseError(pszFile, iLine, "malformed entry point line (offset)");
        }

        pmi->dwEntry = parsehex(psz);

        psz = skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);

        if (!prefixmatch(skipspace(psz), "Static symbols"))
        {
            ParseError(pszFile, iLine, "can't find static symbols section");
        }

        psz = skipblanklines(pszFile, &iLine, &cLines, &ppsz, FALSE);
    }

    // Verify that the sections in a segment don't overlap.  There may be holes due to alignment.

secverify:

    qsort((void *)pmi->ppSecInfo, (size_t)pmi->cSecInfo, sizeof(SECINFO *), comparesecinfosegoffset);

    for (isi = 0; isi < pmi->cSecInfo; ++isi)
    {
        psi = pmi->ppSecInfo[isi];

        // If this section is the first in a new segment, make sure it has an offset of zero

        if (    isi == 0
            ||  psi->iSeg != pmi->ppSecInfo[isi - 1]->iSeg)
        {
            if (psi->dwOffset != 0)
            {
                char szName[32];

                fprintf(stderr, "warning: section %s is first in segment but doesn't have zero offset\n", psi->pszName);

                // Add a fake section to make up for it, then restart the verification process
                // because the array could have been reallocated.

                memset(&si, 0, sizeof(si));

                sprintf(szName, ".segfill%d", psi->iSeg);

                si.iSeg = psi->iSeg;
                si.dwOffset = 0;
                si.dwLength = psi->dwOffset;
                si.pszName = _strdup(szName);
                si.fIsData = FALSE;

                pmi->cSecCode += 1;

                MapInfoAddSecFun(pmi, &pmi->cSecInfo, &pmi->ppSecInfo, &si, sizeof(SECINFO));

                goto secverify;
            }
        }

        // If this section is not the last in its segment, make sure its offset plus length
        // is less than or equal to the offset of the next section.

        if ((isi + 1) < pmi->cSecInfo)
        {
            psiNext = pmi->ppSecInfo[isi + 1];
                
            if (    psi->iSeg == psiNext->iSeg
                &&  psi->dwOffset + psi->dwLength > psiNext->dwOffset)
            {
                fprintf(stderr, "warning: section %s overlaps section %s; truncating.", psi->pszName, psiNext->pszName);
                psi->dwLength = psiNext->dwOffset - psi->dwOffset;
            }
        }
    }

    // Sort the function array by seg:offset to get the static symbols mixed
    // into the public symbols.

    qsort((void *)pmi->ppFunInfo, (size_t)pmi->cFunInfo, sizeof(FUNINFO *), comparefuninfosegoffset);

    // For each section, make sure there is a function which starts at its segment offset.  If not,
    // make one up to backfill.

    for (isi = 0; isi < pmi->cSecInfo; ++isi)
    {
        psi = pmi->ppSecInfo[isi];

        if (psi->dwLength == 0)
            continue;

        for (ifi = 0; ifi < pmi->cFunInfo; ++ifi)
        {
            pfi = pmi->ppFunInfo[ifi];

            if (pfi->iSeg < psi->iSeg)
                continue;

            if (pfi->iSeg > psi->iSeg)
                break;

            if (pfi->dwOffset < psi->dwOffset)
                continue;

            if (pfi->dwOffset >= psi->dwOffset)
                break;
        }

        if (    ifi == pmi->cFunInfo
            ||  pfi->iSeg > psi->iSeg
            ||  pfi->dwOffset > psi->dwOffset)
        {
            memset(&fi, 0, sizeof(fi));
            fi.iSeg = psi->iSeg;
            fi.dwOffset = psi->dwOffset;
            fi.pszName = Undecorate(psi->pszName);
            fi.pszModule = psi->pszName;
            fi.fIsStatic = TRUE;

            MapInfoAddSecFun(pmi, &pmi->cFunInfo, &pmi->ppFunInfo, &fi, sizeof(FUNINFO));

            // Sort the function array by seg:offset

            qsort((void *)pmi->ppFunInfo, (size_t)pmi->cFunInfo, sizeof(FUNINFO *), comparefuninfosegoffset);
        }
    }

    // Assign each function to a section.  If a function starts in a hole between sections,
    // emit a warning, delete the function, and restart the verification.

funverify:

    isi = 0;
    psi = (pmi->cSecInfo > 0) ? pmi->ppSecInfo[isi] : NULL;

    for (ifi = 0; ifi < pmi->cFunInfo; ++ifi)
    {
        pfi = pmi->ppFunInfo[ifi];

        if (pfi->psi)
            continue;

        // Advance to the section that owns or is just past this function's seg:offset

        while (psi)
        {
            if (psi->iSeg > pfi->iSeg)
                break;

            if (    psi->iSeg == pfi->iSeg
                &&  psi->dwOffset + psi->dwLength > pfi->dwOffset)
                break;

            isi += 1;
            psi = (pmi->cSecInfo > isi) ? pmi->ppSecInfo[isi] : NULL;
        }

        // If the section we are on is past this function, then delete the function because
        // it lives "between" sections and we have no way of knowing how big it is.

        if (    psi == NULL
            ||  psi->iSeg != pfi->iSeg
            ||  psi->dwOffset > pfi->dwOffset)
        {
            if (pfi->iSeg != 0)
            {
                fprintf(stderr, "warning: function %04X:%08lX '%s' is not contained by any section\n",
                        pfi->iSeg, pfi->dwOffset, pfi->pszName);
            }
            memmove(&pmi->ppFunInfo[ifi], &pmi->ppFunInfo[ifi + 1],
                    (pmi->cFunInfo - ifi - 1) * sizeof(FUNINFO *));
            pmi->cFunInfo -= 1;
            goto funverify;
        }

        // Otherwise, match this function with its section

        pfi->psi = psi;
    }

    // Compute the length of each function by subtracting next function's offset from the
    // current function's offset.  If this is the last function in a section, however,
    // then subtract the length of the section from the current function's offset.

    for (ifi = 0; ifi < pmi->cFunInfo; ++ifi)
    {
        pfi     = pmi->ppFunInfo[ifi];
        pfiNext = (ifi + 1) == pmi->cFunInfo ? NULL : pmi->ppFunInfo[ifi + 1];

        if (!pfiNext || pfiNext->psi != pfi->psi)
        {
            pfi->dwLength = (pfi->psi->dwOffset + pfi->psi->dwLength) - pfi->dwOffset;
        }
        else
        {
            pfi->dwLength = pfiNext->dwOffset - pfi->dwOffset;
        }
    }

    // Compute the total size of all sections and functions

    for (isi = 0; isi < pmi->cSecInfo; ++isi)
    {
        DWORD dwSum = 0;

        psi = pmi->ppSecInfo[isi];

        if (psi->fIsData)
            pmi->cbSecData += psi->dwLength;
        else
            pmi->cbSecCode += psi->dwLength;

        for (ifi = 0; ifi < pmi->cFunInfo; ++ifi)
        {
            pfi = pmi->ppFunInfo[ifi];

            if (pfi->psi == psi)
            {
                dwSum += pfi->dwLength;
            }
        }

        if (dwSum != psi->dwLength)
        {
            fprintf(stderr, "warning: mismatch verifying length of section %s (sections map says %d, functions say %d)\n",
                    psi->pszName, psi->dwLength, dwSum);
        }
    }

    for (ifi = 0; ifi < pmi->cFunInfo; ++ifi)
    {
        pfi = pmi->ppFunInfo[ifi];

        if (pfi->psi->fIsData)
        {
            pmi->cbFunData += pfi->dwLength;
            pmi->cFunData += 1;
        }
        else
        {
            pmi->cbFunCode += pfi->dwLength;
            pmi->cFunCode += 1;
        }
    }

    return(pmi);
}

void
MapReportStats(MAPINFO * pmi, BOOL fVerbose)
{
    UINT        isi;
    SECINFO *   psi;
    UINT        ifi;
    FUNINFO *   pfi;
    char        szBuf[16];
    
    printf("\nMap File: %s (%s)\n\n", pmi->pszFile, pmi->pszTimestamp);
    printf("  %8s bytes of code in %d sections and %d symbols\n", commaize(szBuf, pmi->cbSecCode), pmi->cSecCode, pmi->cFunCode);
    printf("  %8s bytes of data in %d sections and %d symbols\n", commaize(szBuf, pmi->cbSecData), pmi->cSecData, pmi->cFunData);
    printf("  --------\n");
    printf("  %8s bytes of both in %d sections and %d symbols\n", commaize(szBuf, pmi->cbSecCode + pmi->cbSecData), pmi->cSecInfo, pmi->cFunInfo);
    printf("\n\n\n");

    printf("      Size d Section\n");
    printf("  -------- - ------------\n");

    for (isi = 0; isi < pmi->cSecInfo; ++isi)
    {
        psi = pmi->ppSecInfo[isi];

        printf("  %8s %c %.12s\n", commaize(szBuf, (LONG)psi->dwLength),
               psi->fIsData ? 'd' : ' ', psi->pszName);
    }
    printf("  -------- - ------------------------\n");
    printf("  %8s\n\n\n", commaize(szBuf, (LONG)pmi->cbSecCode + (LONG)pmi->cbSecData));

    printf("      Size sd Section      Module                   Symbol\n");
    printf("  -------- -- ------------ ------------------------ ----------------------------------------------------------------\n");
    
    for (ifi = 0; ifi < pmi->cFunInfo; ++ifi)
    {
        pfi = pmi->ppFunInfo[ifi];

        printf("  %8s %c%c %-12s %-24s %.64s\n", commaize(szBuf, (LONG)pfi->dwLength),
               pfi->fIsStatic ? 's' : ' ', pfi->psi->fIsData ? 'd' : ' ',
               pfi->psi->pszName, pfi->pszModule, pfi->pszName);
    }

    printf("  -------- -- ------------ ------------------------ ----------------------------------------------------------------\n");
    printf("  %8s\n", commaize(szBuf, (LONG)pmi->cbFunCode + (LONG)pmi->cbFunData));
}

void
MapReportDelta(MAPINFO * pmi1, MAPINFO * pmi2, BOOL fVerbose)
{
    UINT        cSecDiff = 0;
    SECDIFF *   pSecDiff;
    SECDIFF *   psd;
    UINT        isi1, csi1, isi2, csi2, isd;
    SECINFO *   psi1;
    SECINFO *   psi2;
    UINT        cFunDiff = 0;
    FUNDIFF *   pFunDiff;
    FUNDIFF *   pfd;
    UINT        ifi1, cfi1, ifi2, cfi2, ifd;
    FUNINFO *   pfi1;
    FUNINFO *   pfi2;
    int         iCmp;
    char        szBuf1[16];
    char        szBuf2[16];
    char        szBuf3[16];
    LONG        lSum1, lSum2;

    printf("\nMap File 1: %s (%s)\n\n", pmi1->pszFile, pmi1->pszTimestamp);
    printf("  %8s bytes of code in %d sections and %d symbols\n", commaize(szBuf1, pmi1->cbSecCode), pmi1->cSecCode, pmi1->cFunCode);
    printf("  %8s bytes of data in %d sections and %d symbols\n", commaize(szBuf1, pmi1->cbSecData), pmi1->cSecData, pmi1->cFunData);
    printf("  --------\n");
    printf("  %8s bytes of both in %d sections and %d symbols\n", commaize(szBuf1, pmi1->cbSecCode + pmi1->cbSecData), pmi1->cSecInfo, pmi1->cFunInfo);
    printf("\n");
    printf("Map File 2: %s (%s)\n\n", pmi2->pszFile, pmi2->pszTimestamp);
    printf("  %8s bytes of code in %d sections and %d symbols\n", commaize(szBuf1, pmi2->cbSecCode), pmi2->cSecCode, pmi2->cFunCode);
    printf("  %8s bytes of data in %d sections and %d symbols\n", commaize(szBuf1, pmi2->cbSecData), pmi2->cSecData, pmi2->cFunData);
    printf("  --------\n");
    printf("  %8s bytes of both in %d sections and %d symbols\n", commaize(szBuf1, pmi2->cbSecCode + pmi2->cbSecData), pmi2->cSecInfo, pmi1->cFunInfo);
    printf("\n\n");

    pSecDiff = psd = MemAlloc((pmi1->cSecInfo + pmi2->cSecInfo) * sizeof(SECDIFF), TRUE);
    pFunDiff = pfd = MemAlloc((pmi1->cFunInfo + pmi2->cFunInfo) * sizeof(FUNDIFF), TRUE);

    // Sort each of the map's section info by name

    qsort((void *)pmi1->ppSecInfo, (size_t)pmi1->cSecInfo, sizeof(SECINFO *), comparesecinfoname);
    qsort((void *)pmi2->ppSecInfo, (size_t)pmi2->cSecInfo, sizeof(SECINFO *), comparesecinfoname);

    isi1 = 0;
    csi1 = pmi1->cSecInfo;
    isi2 = 0;
    csi2 = pmi2->cSecInfo;

    while (isi1 < csi1 || isi2 < csi2)
    {
        psi1 = (isi1 < csi1) ? pmi1->ppSecInfo[isi1] : NULL;
        psi2 = (isi2 < csi2) ? pmi2->ppSecInfo[isi2] : NULL;
        iCmp = comparesecinfodiff(psi1, psi2);

        if (iCmp == 0)
        {
            psd->psi1 = psi1; ++isi1;
            psd->psi2 = psi2; ++isi2;
        }
        else if (iCmp < 0)
        {
            psd->psi1 = psi1; ++isi1;
        }
        else
        {
            psd->psi2 = psi2; ++isi2;
        }

        psd += 1;
        cSecDiff += 1;
    }

    printf("Section Differences%s\n\n", fVerbose ? "" : " (Unchanged Sections Not Shown)");
    printf("     Map 1    Map 2    Delta Section\n");
    printf("  -------- -------- -------- ------------\n");

    qsort((void *)pSecDiff, (size_t)cSecDiff, sizeof(SECDIFF), comparesecdiff);

    lSum1 = lSum2 = 0;

    for (isd = 0, psd = pSecDiff; isd < cSecDiff; ++isd, ++psd)
    {
        LONG l1, l2;

        psi1 = psd->psi1;
        psi2 = psd->psi2;

        l1 = psi1 ? (LONG)psi1->dwLength : 0;
        l2 = psi2 ? (LONG)psi2->dwLength : 0;

        if (l1 == l2 && !fVerbose)
            continue;

        printf("  %8s %8s %8s %s\n",
               commaize(szBuf1, l1), commaize(szBuf2, l2),
               l1 == l2 ? "" : commaize(szBuf3, l1 - l2),
               (psi1 ? psi1 : psi2)->pszName);

        lSum1 += l1;
        lSum2 += l2;
    }

    printf("  -------- -------- -------- ------------\n");
    printf("  %8s %8s %8s\n\n",
           fVerbose ? commaize(szBuf1, (LONG)lSum1) : "",
           fVerbose ? commaize(szBuf2, (LONG)lSum2) : "",
           commaize(szBuf3, (LONG)lSum1 - (LONG)lSum2));

    // Sort each of the map's function info by name

    qsort((void *)pmi1->ppFunInfo, (size_t)pmi1->cFunInfo, sizeof(FUNINFO *), comparefuninfoname);
    qsort((void *)pmi2->ppFunInfo, (size_t)pmi2->cFunInfo, sizeof(FUNINFO *), comparefuninfoname);

    ifi1 = 0;
    cfi1 = pmi1->cFunInfo;
    ifi2 = 0;
    cfi2 = pmi2->cFunInfo;

    while (ifi1 < cfi1 || ifi2 < cfi2)
    {
        pfi1 = (ifi1 < cfi1) ? pmi1->ppFunInfo[ifi1] : NULL;
        pfi2 = (ifi2 < cfi2) ? pmi2->ppFunInfo[ifi2] : NULL;
        iCmp = comparefuninfodiff(pfi1, pfi2);

        if (iCmp == 0)
        {
            pfd->pfi1 = pfi1; ++ifi1;
            pfd->pfi2 = pfi2; ++ifi2;
        }
        else if (iCmp < 0)
        {
            pfd->pfi1 = pfi1; ++ifi1;
        }
        else
        {
            pfd->pfi2 = pfi2; ++ifi2;
        }

        pfd += 1;
        cFunDiff += 1;
    }

    printf("\nSymbol Differences%s\n\n", fVerbose ? "": " (Unchanged Symbols Not Shown)");
    printf("     Map 1    Map 2    Delta Section 1    Section 2    Symbol\n");
    printf("  -------- -------- -------- ------------ ------------ ----------------------------------------------------------------\n");

    qsort((void *)pFunDiff, (size_t)cFunDiff, sizeof(FUNDIFF), comparefundiff);

    lSum1 = lSum2 = 0;

    for (ifd = 0, pfd = pFunDiff; ifd < cFunDiff; ++ifd, ++pfd)
    {
        LONG l1, l2;

        pfi1 = pfd->pfi1;
        pfi2 = pfd->pfi2;

        l1 = pfi1 ? (LONG)pfi1->dwLength : 0;
        l2 = pfi2 ? (LONG)pfi2->dwLength : 0;

        if (l1 == l2 && !fVerbose)
            continue;

        printf("  %8s %8s %8s %-12s %-12s %.64s\n",
               pfi1 ? commaize(szBuf1, l1) : "", pfi2 ? commaize(szBuf2, l2) : "",
               l1 == l2 ? "" : commaize(szBuf3, l1 - l2),
               pfi1 ? pfi1->psi->pszName : "",
               pfi2 ? pfi2->psi->pszName : "",
               (pfi1 ? pfi1 : pfi2)->pszName);

        lSum1 += l1;
        lSum2 += l2;
    }

    printf("  -------- -------- -------- ------------ ------------ ----------------------------------------------------------------\n");
    printf("  %8s %8s %8s\n\n",
           fVerbose ? commaize(szBuf1, (LONG)lSum1) : "",
           fVerbose ? commaize(szBuf2, (LONG)lSum2) : "",
           commaize(szBuf3, (LONG)lSum1 - (LONG)lSum2));
}

int __cdecl main(int argc, char * argv[])
{
    BOOL        fVerbose    = FALSE;
    int         argi        = 1;
    int         cMap        = 0;
    MAPINFO *   rgpmi[2]    = { 0, 0 };

    if (argc < 2)
    {
        Usage();
    }

    for (; argi < argc; ++argi)
    {
        if (argv[argi][0] == '-')
        {
            if (lstrcmpi(argv[argi], "-v") == 0)
            {
                fVerbose = TRUE;
            }
            else
            {
                Usage();
            }
        }
        else if (cMap == 2)
        {
            Usage();
        }
        else
        {
            rgpmi[cMap++] = MapRead(argv[argi]);
        }
    }

    if (cMap == 1)
    {
        MapReportStats(rgpmi[0], fVerbose);
    }
    else
    {
        MapReportDelta(rgpmi[0], rgpmi[1], fVerbose);
    }

    return(0);
}
