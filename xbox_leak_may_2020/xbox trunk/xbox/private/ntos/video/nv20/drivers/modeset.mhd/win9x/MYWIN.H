/*
    FILE:   mywin.h
    DATE:   4/8/99

    This file has some things in it that are in windows.h, but I
    don't want to include windows.h because it just makes a mess.
*/

// A few constants
#define TRUE    1
#define FALSE   0
#define NULL    0

// These are constants for the registry functions
#define MAX_KEY_LEN             0x200
#define HKEY_CLASSES_ROOT       0x80000000
#define HKEY_CURRENT_USER       0x80000001
#define HKEY_LOCAL_MACHINE      0x80000002
#define HKEY_USERS              0x80000003
#define HKEY_PERFORMANCE_DATA   0x80000004
#define HKEY_CURRENT_CONFIG     0x80000005
#define HKEY_DYN_DATA           0x80000006
#define ERROR_SUCCESS           0x00

// This is for memory allocation
#define GMEM_SHARED 0x2000

// A few more constants
#define DLL_PROCESS_DETACH 0
#define DLL_PROCESS_ATTACH 1
#define DLL_THREAD_ATTACH  2
#define DLL_THREAD_DETACH  3

// Handle the WIN16, WIN32 difference as seemlessly as possible.
#ifdef  _WIN32
#define FAR
#define CFUNC   __cdecl
#define WINAPI  _stdcall
#else
#define FAR     __far
#define CFUNC   __cdecl  _loadds
#define WINAPI  _far _pascal  _loadds
#endif

// A few basic types
typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;
typedef UCHAR  FAR *    LPCHAR;
typedef USHORT FAR *    LPUSHORT;
typedef ULONG  FAR *    LPULONG;

// A few Windows functions that we need.
#ifdef _WIN32
#define RegOpenKey      RegOpenKeyA
#define RegEnumKey      RegEnumKeyA
#define RegEnumValue    RegEnumValueA
#define RegDeleteValue  RegDeleteValueA
#define RegQueryValueEx RegQueryValueExA
#define RegSetValueEx   RegSetValueExA
#define RegCreateKey    RegCreateKeyA
#define wsprintf  wsprintfA
#define OutputDebugString OutputDebugStringA
extern int    CFUNC  wsprintf(LPSTR, LPCSTR, ...);
#else
extern int    CFUNC  wsprintf();
#endif

#define REG_NONE        0x000000000
#define REG_SZ          0x000000001
#define REG_EXPAND_SZ   0x000000002
#define REG_BINARY      0x000000003
#define REG_DWORD       0x000000004

extern int    WINAPI GlobalAlloc (int nflags, ULONG dwSize);
extern void   WINAPI GlobalUnlock (int hMem);
extern void   WINAPI GlobalFree (int hMem);
extern int    WINAPI GlobalHandle (USHORT wSelector);
extern LPCHAR WINAPI GlobalLock (int hMem);
extern void   WINAPI OutputDebugString(LPCHAR lpStr);
extern long   WINAPI RegOpenKey   (ULONG hKey, LPCHAR lpName, LPULONG lphKey);
extern long   WINAPI RegCreateKey (ULONG hKey, LPCHAR lpName, LPULONG lphKey);
extern long   WINAPI RegCloseKey (ULONG hEnumCMEnumKey);
extern long   WINAPI RegEnumKey (ULONG hEnumCMEnumKey, ULONG dwSubKey,
                                LPCHAR szItem, ULONG dwLen);
extern long   WINAPI RegEnumValue (ULONG hEnumCMEnumKey, ULONG dwValueIndex,
                                LPCHAR lpValueName, LPULONG dwValueNameSize,
                                LPULONG lpReserved, LPULONG lpType,
                                LPCHAR lpData, LPULONG lpDataSize);
extern long   WINAPI RegQueryValueEx (ULONG hKey, LPCHAR lpValue,
                                LPULONG lpRes, LPULONG lpType,
                                LPCHAR lpBuf,  LPULONG lpSize);
extern long   WINAPI RegSetValueEx (ULONG hKey, LPCHAR lpValue,
                                LPULONG lpRes, ULONG dwType,
                                LPCHAR lpBuf,  ULONG dwSize);
extern long   WINAPI RegDeleteValue (ULONG hKey, LPCHAR lpValue);
extern LPCHAR WINAPI lstrcat (LPCHAR lpDst, LPCHAR lpSrc);
extern LPCHAR WINAPI lstrcpy (LPCHAR lpDst, LPCHAR lpSrc);
extern ULONG  WINAPI lstrlen (LPCHAR lpDst);
extern ULONG  WINAPI lstrcmpi (LPCHAR lpDst, LPCHAR lpSrc);

// A little bit of debugging support.
#ifdef DEBUG
extern  char szTemp[512];
#define DBG_PRINTF(x)       OutputDebugString((LPCHAR) (x))
#define DBG_PRINT_1DEC(x,y) {   wsprintf (szTemp, (x), (y));   \
                                OutputDebugString (szTemp);    \
                            }
#else
#define DBG_PRINTF(x)
#define DBG_PRINT_1DEC(x,y)
#endif

