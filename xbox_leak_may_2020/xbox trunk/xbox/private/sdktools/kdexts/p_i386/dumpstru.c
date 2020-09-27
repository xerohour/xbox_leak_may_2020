/*** dumpstru.c - Dump Structure services
 *
 *  This module contains format services for converting binary data into format
 *  text strings according to the format record.
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *  Author:     Michael Tsang (MikeTs)
 *  Created     11/06/95
 *
 *  MODIFICATION HISTORY
 */

#include "precomp.h"

char cszNull[] = "";
char cszDecFmt[] = SZ_FMT_DEC;
char cszHexFmt[] = SZ_FMT_HEX;
char cszByteFmt[] = SZ_FMT_HEX_BYTE;
char cszWordFmt[] = SZ_FMT_HEX_WORD;
char cszDWordFmt[] = SZ_FMT_HEX_DWORD;
char cszLabelReserved[] = "Reserved=";
char cszReserved[] = "Reserved";
char cszUndefined[] = "Undefined";
char cszLabelFmt[] = "%04x:%s";

FMTNUM fmtHexByteNoEOL =
{
    {FMT_NUMBER, UNIT_BYTE, 1, FMTF_NO_PRINT_DATA | FMTF_NO_EOL, 1, NULL, NULL,
     NULL},
    0xff, 0, cszByteFmt
};

FMTNUM fmtHexByte =
{
    {FMT_NUMBER, UNIT_BYTE, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xff, 0, cszByteFmt
};

FMTNUM fmtHexWord =
{
    {FMT_NUMBER, UNIT_WORD, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xffff, 0, cszWordFmt
};

FMTNUM fmtHexDWord =
{
    {FMT_NUMBER, UNIT_DWORD, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xffffffff, 0, cszDWordFmt
};

FMTNUM fmt2HexDWord =
{
    {FMT_NUMBER, UNIT_DWORD, 2, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xffffffff, 0, cszDWordFmt
};

FMTNUM fmt4HexDWord =
{
    {FMT_NUMBER, UNIT_DWORD, 4, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xffffffff, 0, cszDWordFmt
};

FMTNUM fmt6HexDWord =
{
    {FMT_NUMBER, UNIT_DWORD, 6, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xffffffff, 0, cszDWordFmt
};

FMTNUM fmt8HexDWord =
{
    {FMT_NUMBER, UNIT_DWORD, 8, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xffffffff, 0, cszDWordFmt
};

FMTNUM fmtDecNum =
{
    {FMT_NUMBER, UNIT_BYTE, 1, FMTF_NO_PRINT_DATA, 1, NULL, NULL, NULL},
    0xff, 0, cszDecFmt
};

typedef int (*PFNFMT)(char *, PFMTHDR, BYTE *, DWORD *);

//Local function prototypes
int FormatNum(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset);
int FormatEnum(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset);
int FormatBits(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset);
int FormatString(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset);
int GetData(BYTE bUnitSize, PBYTE pb, DWORD dwOffset, PDWORD pdwData);
int PrintData(PSZ pszBuffer, BYTE bUnitSize, DWORD dwData, BOOL fPadSpace);

char szDefSep[] = SZ_SEP_SPACE;
char szDefOffsetFmt[] = SZ_FMT_WORDOFFSET;
PFNFMT FmtFuncTable[] =
{
    FormatNum,          //0: FMT_NUMBER
    FormatEnum,         //1: FMT_ENUM
    FormatBits,         //2: FMT_BITS
    FormatString,       //3: FMT_STRING
};
#define NUM_FMT_FUNCS   (sizeof(FmtFuncTable)/sizeof(PFNFMT))

#ifdef _PRINTF
/***EP  BinPrintf - Binary printf
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold the formatted string
 *                   (if NULL, use internal buffer)
 *      pfmt -> format record array
 *      pb -> binary data buffer
 *      pdwOffset -> offset to binary data buffer (if NULL, use internal)
 *      pszOffsetFormat -> offset format string (can be NULL)
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int BinPrintf(PSZ pszBuffer, PFMT pfmt, PBYTE pb, PDWORD pdwOffset,
              PSZ pszOffsetFormat)
{
    TRACENAME("BINPRINTF")
    int rc = FERR_NONE;
    DWORD dwOffset = 0, dwOldOffset;
    DWORD dwData;
    char szBuff[256];
    char *psz = pszBuffer? pszBuffer: szBuff;
    DWORD *pdw = pdwOffset? pdwOffset: &dwOffset;

    ENTER(4, ("BinPrintf(pszBuff=%lx,pfmt=%lx,pdwOffset=%lx,Offset=%lx)\n",
              pszBuffer, pfmt, pdwOffset, pdwOffset? *pdwOffset: 0));

    if (pfmt != NULL)
    {
        BYTE i, j;

        for (i = 0; pfmt[i].pfmtType != NULL; ++i)
        {
            if (pfmt[i].pszLabel != NULL)
            {
                if (pszOffsetFormat != NULL)
                {
                    _PRINTF(pszOffsetFormat, *pdw);
                    if (GetData(pfmt[i].pfmtType->bUnitSize, pb, *pdw, &dwData)
                        == FERR_NONE)
                    {
                        PrintData(psz, pfmt[i].pfmtType->bUnitSize, dwData,
                                  TRUE);
                        _PRINTF(psz);
                    }

                    for (j = 1; j < pfmt[i].pfmtType->bUnitCnt; ++j)
                    {
                        if (GetData(pfmt[i].pfmtType->bUnitSize, pb,
                                    *pdw + j*pfmt[i].pfmtType->bUnitSize,
                                    &dwData) == FERR_NONE)
                        {
                            _PRINTF(",");
                            PrintData(psz, pfmt[i].pfmtType->bUnitSize, dwData,
                                      FALSE);
                            _PRINTF(psz);
                        }
                    }
                }

                if (pfmt[i].pszLabel[0] != '\0')
                {
                    _PRINTF(";%s", pfmt[i].pszLabel);
                }
                else
                {
                    _PRINTF("\n");
                }
            }

            dwOldOffset = *pdw;
            if ((pfmt[i].pszLabel != NULL) && (pfmt[i].pszLabel[0] == '\0'))
            {
                *pdw += pfmt[i].pfmtType->bUnitCnt*pfmt[i].pfmtType->bUnitSize;
            }
            else
            {
                if (pfmt[i].pszLabel == NULL)
                {
                    _PRINTF(",");
                }

                if ((rc = BinSprintf(psz, pfmt[i].pfmtType, pb, pdw)) ==
                    FERR_NONE)
                {
                    PSZ psz1, psz2;
                    BOOL fSpace = FALSE;
                    //
                    // Compress multiple spaces into a single comma.
                    //
                    for (psz1 = psz2 = psz; *psz2 != '\0'; ++psz2)
                    {
                        if (*psz2 == ' ')
                        {
                            if (!fSpace && (psz1 != psz))
                            {
                                *psz1 = ',';
                                psz1++;
                            }
                            fSpace = TRUE;
                        }
                        else
                        {
                            fSpace = FALSE;
                            *psz1 = *psz2;
                            psz1++;
                        }
                    }

                    if ((psz1 > psz) && (*(psz1 - 1) == ','))
                    {
                        *(psz1 - 1) = '\0';
                    }
                    else if ((psz1 > psz) && (*(psz1 - 1) == '\n') &&
                             (*(psz1 - 2) == ','))
                    {
                        *(psz1 - 2) = '\n';
                        *(psz1 - 1) = '\0';
                    }
                    else
                    {
                        *psz1 = '\0';
                    }

                    _PRINTF(psz);
                }

            }

            if (pfmt[i].lpfn != NULL)
            {
                pfmt[i].lpfn(pb, dwOldOffset);
            }
        }
    }

    EXIT(4, ("BinPrintf=%d (Offset=%lx,Buff=%s)\n", rc, *pdwOffset, pszBuffer));
    return rc;
}       //BinPrintf
#endif  //ifdef _PRINTF

/***EP  BinSprintf - Binary sprintf
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold the formatted string
 *      pfmt -> format record
 *      pb -> binary data buffer
 *      pdwOffset -> offset to binary data buffer
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int BinSprintf(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset)
{
    TRACENAME("BINSPRINTF")
    int rc = FERR_NONE;

    ENTER(4, ("BinSprintf(fmt=%d,Offset=%lx)\n", pfmt->bFmtType, *pdwOffset));

    if (pfmt->bFmtType >= NUM_FMT_FUNCS)
        rc = FERR_INVALID_FORMAT;
    else
    {
        int i;
        BYTE j;
        DWORD dwData;

        *pszBuffer = '\0';
        for (i = 0; (rc == FERR_NONE) && (i < pfmt->iRepeatCnt); ++i)
        {
            if (pfmt->dwfFormat & FMTF_PRINT_OFFSET)
            {
                _SPRINTF(pszBuffer,
                         pfmt->pszOffsetFmt? pfmt->pszOffsetFmt: szDefOffsetFmt,
                         *pdwOffset);
            }

            if (!(pfmt->dwfFormat & FMTF_NO_PRINT_DATA) &&
                (GetData(pfmt->bUnitSize, pb, *pdwOffset, &dwData) ==
                 FERR_NONE))
            {
                PrintData(pszBuffer, pfmt->bUnitSize, dwData, FALSE);
                _STRCAT(pszBuffer, ";");
            }

            if (pfmt->pszLabel)
                _STRCAT(pszBuffer, pfmt->pszLabel);

            for (j = 0; (rc == FERR_NONE) && (j < pfmt->bUnitCnt); ++j)
            {
                rc = FmtFuncTable[pfmt->bFmtType]
                        (&pszBuffer[_STRLEN(pszBuffer)], pfmt, pb, pdwOffset);

                if (rc == FERR_NONE)
                {
                    if (!(pfmt->dwfFormat & FMTF_NO_SEP))
                    {
                        _STRCAT(pszBuffer,
                                pfmt->pszFieldSep? pfmt->pszFieldSep: szDefSep);
                    }

                    if (!(pfmt->dwfFormat & FMTF_NO_INC_OFFSET))
                        *pdwOffset += pfmt->bUnitSize;
                }
            }

            if ((rc == FERR_NONE) && !(pfmt->dwfFormat & FMTF_NO_EOL))
            {
                _STRCAT(pszBuffer, "\n");
            }
        }
    }

    EXIT(4, ("BinSprintf=%d (Offset=%lx,Buff=%s)\n",
             rc, *pdwOffset, pszBuffer));
    return rc;
}       //BinSprintf

/***LP  FormatNum - Format numbers
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold formatted string
 *      pfmt -> format record
 *      pb -> binary data buffer
 *      pdwOffset -> offset to binary data buffer
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int FormatNum(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset)
{
    TRACENAME("FORMATNUM")
    int rc;
    PFMTNUM pfmtNum = (PFMTNUM)pfmt;
    DWORD dwData;

    ENTER(5, ("FormatNum(Offset=%lx)\n", *pdwOffset));

    if ((rc = GetData(pfmt->bUnitSize, pb, *pdwOffset, &dwData)) == FERR_NONE)
    {
        dwData &= pfmtNum->dwMask;
        dwData >>= pfmtNum->bShiftCnt;

        _SPRINTF(&pszBuffer[_STRLEN(pszBuffer)], pfmtNum->pszNumFmt, dwData);
    }

    EXIT(5, ("FormatNum=%d (Offset=%lx,Buff=%s)\n",
         rc, *pdwOffset, pszBuffer));
    return rc;
}       //FormatNum

/***LP  FormatEnum - Format enumerated values
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold formatted string
 *      pfmt -> format record
 *      pb -> binary data buffer
 *      pdwOffset -> offset to binary data buffer
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int FormatEnum(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset)
{
    TRACENAME("FORMATENUM")
    int rc = FERR_NONE;
    PFMTENUM pfmtEnum = (PFMTENUM)pfmt;
    DWORD dwData;

    ENTER(5, ("FormatEnum(Offset=%lx)\n", *pdwOffset));

    if ((rc = GetData(pfmt->bUnitSize, pb, *pdwOffset, &dwData)) == FERR_NONE)
    {
        dwData &= pfmtEnum->dwMask;
        dwData >>= pfmtEnum->bShiftCnt;

        if ((dwData < pfmtEnum->dwStartEnum) || (dwData > pfmtEnum->dwEndEnum))
            _STRCAT(pszBuffer, pfmtEnum->pszOutOfRange);
        else
        {
            dwData -= pfmtEnum->dwStartEnum;
            _STRCAT(pszBuffer, pfmtEnum->ppszEnumNames[dwData]);
        }
    }

    EXIT(5, ("FormatEnum=%d (Offset=%lx,Buff=%s)\n",
             rc, *pdwOffset, pszBuffer));
    return rc;
}       //FormatEnum

/***LP  FormatBits - Format bit values
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold formatted string
 *      pfmt -> format record
 *      pb -> binary data buffer
 *      pdwOffset -> offset to binary data buffer
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int FormatBits(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset)
{
    TRACENAME("FORMATBITS")
    int rc = FERR_NONE;
    PFMTBITS pfmtBits = (PFMTBITS)pfmt;
    DWORD dwData;

    ENTER(5, ("FormatBits(Offset=%lx)\n", *pdwOffset));

    if ((rc = GetData(pfmt->bUnitSize, pb, *pdwOffset, &dwData)) == FERR_NONE)
    {
        int i, j;
        DWORD dw;

        for (i = 31, j = 0; i >= 0; --i)
        {
            dw = 1L << i;
            if (pfmtBits->dwMask & dw)
            {
                if (dwData & dw)
                {
                    if ((pfmtBits->ppszOnNames != NULL) &&
                        (pfmtBits->ppszOnNames[j] != NULL))
                    {
                        _STRCAT(pszBuffer, pfmtBits->ppszOnNames[j]);
                    }
                }
                else
                {
                    if ((pfmtBits->ppszOffNames != NULL) &&
                        (pfmtBits->ppszOffNames[j] != NULL))
                    {
                        _STRCAT(pszBuffer, pfmtBits->ppszOffNames[j]);
                    }
                }

                if (!(pfmt->dwfFormat & FMTF_NO_SEP))
                {
                    _STRCAT(pszBuffer,
                            pfmt->pszFieldSep? pfmt->pszFieldSep: szDefSep);
                }

                j++;
            }
        }
    }

    EXIT(5, ("FormatBits=%d (Offset=%lx,Buff=%s)\n",
             rc, *pdwOffset, pszBuffer));
    return rc;
}       //FormatBits

/***LP  FormatString - Format string data
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold formatted string
 *      pfmt -> format record
 *      pb -> binary data buffer
 *      pdwOffset -> offset to binary data buffer
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int FormatString(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset)
{
    TRACENAME("FORMATSTRING")
    int rc = FERR_NONE;

    ENTER(5, ("FormatString(Offset=%lx)\n", *pdwOffset));

    pb += *pdwOffset;
    if (pfmt->dwfFormat & FMTF_STR_ASCIIZ)
    {
        _STRCPY(pszBuffer, (char *)pb);
        if ((pfmt->bUnitSize == 0) && !(pfmt->dwfFormat & FMTF_NO_INC_OFFSET))
            *pdwOffset += _STRLEN((char *)pb) + 1;
    }
    else if (pfmt->bUnitSize != 0)
    {
        _STRCPYN(pszBuffer, (char *)pb, pfmt->bUnitSize);
        pszBuffer[pfmt->bUnitSize] = '\0';
    }

    EXIT(5, ("FormatString=%d (Offset=%lx,Buff=%s)\n",
             rc, *pdwOffset, pszBuffer));
    return rc;
}       //FormatString

/***LP  GetData - Get data of appropriate size from the binary buffer
 *
 *  ENTRY
 *      bUnitSize - size of data unit
 *      pb -> data buffer
 *      dwOffset - offset into data buffer
 *      pdwData -> to hold data
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int GetData(BYTE bUnitSize, PBYTE pb, DWORD dwOffset, PDWORD pdwData)
{
    TRACENAME("GETDATA")
    int rc = FERR_NONE;

    ENTER(6, ("GetData(UnitSize=%d,Data=%lx,Offset=%lx)\n",
              bUnitSize, *(DWORD *)pb, dwOffset));

    pb += dwOffset;
    switch (bUnitSize)
    {
        case UNIT_BYTE:
            *pdwData = (DWORD)(*pb);
            break;
        case UNIT_WORD:
            *pdwData = (DWORD)(*((WORD *)pb));
            break;
        case UNIT_DWORD:
            *pdwData = *(DWORD *)pb;
            break;
        default:
            rc = FERR_INVALID_UNITSIZE;

    }

    EXIT(6, ("GetData=%d (Data=%lx)\n", rc, *pdwData));
    return rc;
}       //GetData

/***LP  PrintData - Print data value according to its size
 *
 *  ENTRY
 *      pszBuffer -> buffer to hold formatted string
 *      bUnitSize - size of data unit
 *      dwData - number
 *      fPadSpace - if TRUE, pad space to 8 chars
 *
 *  EXIT-SUCCESS
 *      returns FERR_NONE
 *  EXIT-FAILURE
 *      returns negative error code
 */

int PrintData(PSZ pszBuffer, BYTE bUnitSize, DWORD dwData, BOOL fPadSpace)
{
    TRACENAME("PRINTDATA")
    int rc = FERR_NONE;

    ENTER(6, ("PrintData(UnitSize=%d,Data=%lx,fPadSpace=%x)\n",
              bUnitSize, dwData, fPadSpace));

    switch (bUnitSize)
    {
        case UNIT_BYTE:
            _SPRINTF(pszBuffer, "%02x", (BYTE)dwData);
            if (fPadSpace)
            {
                _STRCAT(pszBuffer, "      ");
            }
            break;
        case UNIT_WORD:
            _SPRINTF(pszBuffer, "%04x", (WORD)dwData);
            if (fPadSpace)
            {
                _STRCAT(pszBuffer, "    ");
            }
            break;
        case UNIT_DWORD:
            _SPRINTF(pszBuffer, "%08x", dwData);
            break;
        default:
            rc = FERR_INVALID_UNITSIZE;

    }

    EXIT(6, ("PrintData=%d (Buff=%s)\n", rc, pszBuffer));
    return rc;
}       //PrintData
