#ifndef _INC_TEXT
#define _INC_TEXT

#include "defs.h"
#include "image.h"

#define VERSION         "3.22"

#define RAWREAD         "RawRead"
#define RAWWRITE        "RawWrite"

#ifdef MSINTERNAL
#define PROPAGANDA1     RAWREAD "/" RAWWRITE " version " VERSION " by Derek Smith (dereks)."
#else // MSINTERNAL
#define PROPAGANDA1     RAWREAD "/" RAWWRITE " version " VERSION "."
#endif // MSINTERNAL

#define PROPAGANDA2     "Copyright (c) 1995-2000 Microsoft Corporation.  All rights reserved."
#define PROPAGANDA3     "Reads or writes an exact copy of any MS-DOS or Win9X formatted media."

#ifdef MSINTERNAL
#define PROPAGANDA4     "This is a Microsoft-internal tool and may not be distributed."
#endif // MSINTERNAL

#define CRLF            "\r\n"

#define SZCLUSTER       "%10lu"
#define SZBACK          "\b\b\b\b\b\b\b\b\b\b"
#define SZDONE          "[ok]      " CRLF

#define LINE            "==============================================================================" CRLF

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes
extern BOOL AskUser(LPIMAGESTRUCT, LPCSTR, ...);
extern void InsertDisk(LPIMAGESTRUCT, LPCSTR, char);
extern int ErrorMsg(LPIMAGESTRUCT, int);
extern int GetFullPath(LPSTR, LPCSTR, UINT);
extern int GetFullPathInPlace(LPSTR, UINT);
extern int GetFileLocation(LPCSTR, LPSTR, LPSTR);
extern int SetFilePath(LPCSTR, LPCSTR, LPSTR);
extern int ValidateFileName(LPCSTR);
extern LPSTR StrCpy(LPSTR, LPCSTR);
extern int StrLen(LPCSTR);
extern int StrCmp(LPCSTR, LPCSTR);
extern int StrCmpI(LPCSTR, LPCSTR);
extern LPSTR StrCat(LPSTR, LPCSTR);
extern void DumpBootSector(LPSTR, LPRRBOOTSECTOR);
extern void DumpDPB(LPSTR, LPDPB);
extern void DumpLeadSector(LPSTR, LPLEADSECTOR);
extern BOOL IsDrivePath(LPSTR, LPBYTE);

#ifdef WIN32

extern int ImageMessageBox(HWND, UINT, LPCSTR, ...);
extern int GetShortFileName(LPCSTR, LPSTR);

#else // WIN32

extern void GoPropaganda(void);
extern int GoRawReadHelp(void);
extern int GoRawWriteHelp(void);
extern void FlushKeyboardBuffer(void);

#endif // WIN32

#ifdef __cplusplus
}
#endif

#endif // _INC_TEXT
