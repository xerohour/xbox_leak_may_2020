/*** dumpstru.h - Dump Structure services definitions
 *
 *  Copyright (c) 1995,1996 Microsoft Corporation
 *  Author:     Michael Tsang (MikeTs)
 *  Created     11/06/95
 *
 *  MODIFICATION HISTORY
 */

#ifndef _DUMPSTRU_H
#define _DUMPSTRU_H

#ifndef ENTER
  #define ENTER(n,p)
#endif

#ifndef EXIT
  #define EXIT(n,p)
#endif

#ifndef TRACENAME
  #define TRACENAME(s)
#endif

#ifdef USE_CRUNTIME
    #define _PRINTF             dprintf
    #define _SPRINTF            sprintf
    #define _STRCPY             strcpy
    #define _STRCPYN            strncpy
    #define _STRCAT             strcat
    #define _STRLEN             strlen
#else
  #ifdef WINAPP
    #define _SPRINTF            wsprintf
    #define _STRCPY             lstrcpy
    #define _STRCPYN            lstrcpyn
    #define _STRCAT             lstrcat
    #define _STRLEN             lstrlen
  #else         //assume VxD
    #define _SPRINTF            _Sprintf
    #define _STRCPY(s1,s2)      _lstrcpyn(s1,s2,(DWORD)(-1))
    #define _STRCPYN(s1,s2,n)   _lstrcpyn(s1,s2,(n)+1)
    #define _STRCAT(s1,s2)      _lstrcpyn((s1)+_lstrlen(s1),s2,(DWORD)(-1))
    #define _STRLEN             _lstrlen
  #endif
#endif

//Error codes
#define FERR_NONE               0
#define FERR_INVALID_FORMAT     -1
#define FERR_INVALID_UNITSIZE   -2

//String constants
#define SZ_SEP_SPACE            " "
#define SZ_SEP_TAB              "\t"
#define SZ_SEP_COMMA            ","
#define SZ_SEP_SEMICOLON        ";"
#define SZ_SEP_COLON            ":"
#define SZ_FMT_DEC              "%d"
#define SZ_FMT_HEX              "%x"
#define SZ_FMT_HEX_BYTE         "%02x"
#define SZ_FMT_HEX_WORD         "%04x"
#define SZ_FMT_HEX_DWORD        "%08lx"
#define SZ_FMT_WORDOFFSET       SZ_FMT_HEX_WORD SZ_SEP_COLON
#define SZ_FMT_DWORDOFFSET      SZ_FMT_HEX_DWORD SZ_SEP_COLON

//bFmtType values
#define FMT_NUMBER              0
#define FMT_ENUM                1
#define FMT_BITS                2
#define FMT_STRING              3

//bUnitSize values
#define UNIT_BYTE               sizeof(BYTE)
#define UNIT_WORD               sizeof(WORD)
#define UNIT_DWORD              sizeof(DWORD)

//dwfFormat flags
#define FMTF_NO_EOL             0x80000000
#define FMTF_NO_INC_OFFSET      0x40000000
#define FMTF_NO_SEP             0x20000000
#define FMTF_NO_PRINT_DATA      0x10000000
#define FMTF_PRINT_OFFSET       0x08000000
#define FMTF_STR_ASCIIZ         0x00000001
#define FMTF_FIRST_FIELD        (FMTF_NO_EOL | FMTF_NO_INC_OFFSET | \
                                 FMTF_NO_PRINT_DATA)
#define FMTF_MIDDLE_FIELD       (FMTF_NO_EOL | FMTF_NO_INC_OFFSET | \
                                 FMTF_NO_PRINT_DATA)
#define FMTF_LAST_FIELD         FMTF_NO_PRINT_DATA

typedef VOID (*LPFN)(PBYTE, DWORD);

typedef struct fmthdr_s
{
    BYTE bFmtType;              //Format type: see FMT_*
    BYTE bUnitSize;             //Data unit size: see UNIT_*
    BYTE bUnitCnt;              //Data unit count for a format record
    DWORD dwfFormat;            //Format flags: see FMTF_*
    int iRepeatCnt;             //Repeat count for this format record
    PSZ pszOffsetFmt;           //Offset format
    PSZ pszFieldSep;            //Field separator between bUnitCnt of data
    PSZ pszLabel;               //LHS label
} FMTHDR;
typedef FMTHDR *PFMTHDR;

typedef struct fmt_s
{
    PSZ pszLabel;
    PFMTHDR pfmtType;
    LPFN lpfn;
} FMT;
typedef FMT *PFMT;

typedef struct fmtnum_s
{
    FMTHDR hdr;
    DWORD dwMask;
    BYTE bShiftCnt;
    PSZ pszNumFmt;
} FMTNUM;
typedef FMTNUM *PFMTNUM;

typedef struct fmtenum_s
{
    FMTHDR hdr;
    DWORD dwMask;
    BYTE bShiftCnt;
    DWORD dwStartEnum;
    DWORD dwEndEnum;
    PSZ *ppszEnumNames;
    PSZ pszOutOfRange;
} FMTENUM;
typedef FMTENUM *PFMTENUM;

typedef struct fmtbits_s
{
    FMTHDR hdr;
    DWORD dwMask;
    PSZ *ppszOnNames;
    PSZ *ppszOffNames;
} FMTBITS;
typedef FMTBITS *PFMTBITS;

typedef struct fmtstr_s
{
    FMTHDR hdr;
} FMTSTR;
typedef FMTSTR *PFMTSTR;

/*** Exported data
 */

extern FMTNUM fmtHexByteNoEOL, fmtHexByte, fmtHexWord, fmtHexDWord,
              fmt2HexDWord, fmt4HexDWord, fmt6HexDWord, fmt8HexDWord, fmtDecNum;
extern char cszDecFmt[], cszHexFmt[], cszByteFmt[], cszWordFmt[], cszDWordFmt[];
extern char cszLabelReserved[], cszReserved[], cszUndefined[], cszLabelFmt[],
            cszNull[];

/*** Exported API Prototypes
 */

#ifdef _PRINTF
int BinPrintf(PSZ pszBuffer, PFMT pfmt, PBYTE pb, PDWORD pdwOffset,
              PSZ pszOffsetFormat);
#endif
int BinSprintf(PSZ pszBuffer, PFMTHDR pfmt, PBYTE pb, PDWORD pdwOffset);

#endif	//ifndef _DUMPSTRU_H
