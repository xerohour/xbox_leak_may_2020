#ifndef _INC_DEFS
#define _INC_DEFS

#if defined(WIN32) || defined(WIN16)
#include <windows.h>
#endif // defined(WIN32) || defined(WIN16)

// Internal sector size.  This is not the true size of a sector on the drive,
// just what we call a sector when talking about the Pad Sector, Lead Sector, etc.
#define CBSEC                   512

// Command-line switches
#define SWITCH_HELP             '?'
#define SWITCH_ALLOCATED        'A'
#define SWITCH_FINDBAD          'B'
#define SWITCH_COMPRESS         'C'
#define SWITCH_DUMPINFO         'D'
#define SWITCH_FORCE            'F'
#define SWITCH_IGNORETYPE       'I'
#define SWITCH_NOLOCK           'L'
#define SWITCH_NOPROMPT         'N'
#define SWITCH_OVERWRITE        'O'
#define SWITCH_NOSTATUS         'Q'
#define SWITCH_NORECOVER        'R'
#define SWITCH_SPAN             'S'
#define SWITCH_TEXTCOMMENT      'T'
#define SWITCH_VERIFY           'V'
#define SWITCH_WIPE             'W'

// Undocumented switch for reading/writing 1 cluster at a time
#define SWITCH_1CLUSTER         '1'

// Options
#define OPTIONS_NONE            0x0000
#define OPTIONS_COMPRESS        0x0001
#define OPTIONS_NOPROMPT        0x0002
#define OPTIONS_SPAN            0x0004
#define OPTIONS_IGNORETYPE      0x0008
#define OPTIONS_NOLOCK          0x0010
#define OPTIONS_OVERWRITE       0x0020
#define OPTIONS_NORECOVER       0x0040
#define OPTIONS_FORCE           0x0080
#define OPTIONS_ALLOCATED       0x0100
#define OPTIONS_WIPE            0x0200
#define OPTIONS_FINDBAD         0x0400
#define OPTIONS_NOSTATUS        0x0800
#define OPTIONS_VERIFY          0x1000
#define OPTIONS_DUMPINFO        0x2000
#define OPTIONS_1CLUSTER        0x8000

// Error returns
#define ERROR_NONE              0
#define ERROR_FILEOPEN          1
#define ERROR_FILEWRITE         2
#define ERROR_FILEREAD          3
#define ERROR_DRIVEREAD         4
#define ERROR_DRIVEWRITE        5
#define ERROR_OOM               6
#define ERROR_TOOMANYFRAGMENTS  7
#define ERROR_INVALIDIMAGE      8
#define ERROR_DRIVETOOSMALL     9
#define ERROR_LOCKFAIL          10
#define ERROR_OUTOFSEQUENCE     11
#define ERROR_COMPRESSEDDRIVE   12
#define ERROR_DRIVECLUSTERSTOOBIG   13
#define ERROR_IMAGECLUSTERSTOOBIG   14
#define ERROR_COMPRESS          15
#define ERROR_UNCOMPRESS        16
#define ERROR_DPB               17
#define ERROR_OLDKERNEL         18
#define ERROR_DRIVETOOBIG       19
#define ERROR_STUPIDRECOVERY    20
#define ERROR_WRONGFSTYPE       21
#define ERROR_BADCLUSTEROFFSET  22
#define ERROR_RECOVERYFAILED    23
#define ERROR_VERIFYFILE        24
#define ERROR_VERIFYDISK        25
#define ERROR_USERCANCEL        26
#define ERROR_LFN               27
#define ERROR_UNSUPPORTEDFS     28

#ifdef WIN32
#define READWRITE_BUFFER_SIZE   0x40000
#else // WIN32
#define READWRITE_BUFFER_SIZE   0x8000
#endif // WIN32

#ifndef TRUE
#define TRUE                    1
#endif // TRUE

#ifndef FALSE
#define FALSE                   0
#endif // FALSE

#ifndef NULL
#define NULL                    0
#endif // NULL

#ifndef MAX_FILE
#ifdef WIN32
#define MAX_FILE                256
#else // WIN32
#define MAX_FILE                64
#endif // WIN32
#endif // MAX_FILE

#ifndef MAX_DIR
#define MAX_DIR                 MAX_FILE
#endif // MAX_DIR

#ifndef MAX_PATH
#define MAX_PATH                (MAX_FILE + 2)
#endif // MAX_PATH

#ifndef MAXWORD
#define MAXWORD                 ((WORD)0xFFFF)
#endif // MAXWORD

#ifndef MAXDWORD
#define MAXDWORD                ((DWORD)0xFFFFFFFF)
#endif // MAXDWORD

#ifndef min
#define min(a, b)               ((a < b) ? a : b)
#endif // min

#ifndef max
#define max(a, b)               ((a > b) ? a : b)
#endif // max

#if defined(WIN32) || defined(WIN16)

typedef HANDLE                  FILEHANDLE;

#else // defined(WIN32) || defined(WIN16)

#define CALLBACK                __cdecl __far
#define NEAR                    __near
#define FAR                     __far
#define PASCAL                  __pascal

typedef char                    FAR *LPSTR, const FAR *LPCSTR;

typedef unsigned char           BYTE,   *PBYTE,     FAR *LPBYTE;
typedef unsigned short          WORD,   *PWORD,     FAR *LPWORD;
typedef unsigned int            UINT,   *PUINT,     FAR *LPUINT;
typedef unsigned long           DWORD,  *PDWORD,    FAR *LPDWORD;
typedef int                     BOOL,   *PBOOL,     FAR *LPBOOL;
typedef int                     FILEHANDLE;
typedef void                            *PVOID,     FAR *LPVOID;
typedef char                                        FAR *LPSTR;

#endif // defined(WIN32) || defined(WIN16)

#ifdef WIN16
#define LOADDS                  __loadds
#else // WIN16
#define LOADDS
#endif // WIN16

#ifndef LONIBBLE
#define LONIBBLE(b)             ((BYTE)(((BYTE)(b)) & 0xF))
#endif // LONIBBLE

#ifndef HINIBBLE
#define HINIBBLE(b)             ((BYTE)((((BYTE)(b)) >> 4) & 0xF))
#endif // HINIBBLE

#ifndef LOBYTE
#define LOBYTE(w)               ((BYTE)((WORD)(w)))
#endif // LOBYTE

#ifndef HIBYTE
#define HIBYTE(w)               ((BYTE)((((WORD)(w)) >> 8) & 0xFF))
#endif // HIBYTE

#ifndef LOWORD
#define LOWORD(dw)              ((WORD)((DWORD)(dw)))
#endif // LOWORD

#ifndef HIWORD
#define HIWORD(dw)              ((WORD)((((DWORD)(dw)) >> 16) & 0xFFFF))
#endif // HIWORD

#ifndef MAKEWORD
#define MAKEWORD(l, h)          ((((WORD)h) << 8) | (WORD)l)
#endif // MAKEWORD

#ifndef MAKEDWORD
#define MAKEDWORD(l, h)         ((((DWORD)h) << 16) | (DWORD)l)
#endif // MAKEDWORD

#define DIVUP(a, b)             (((a) / (b)) + (((a) % (b)) ? 1 : 0))

#endif // _INC_DEFS