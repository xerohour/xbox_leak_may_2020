#ifndef _MFGTEST_H
#define _MFGTEST_H




#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))



BOOL WINAPI MfgTestCopyImageToDrive (
    LPCSTR Name,
    PBYTE ImageBuffer,
    SIZE_T Size
    );

BOOL WINAPI MfgTestMapDvdDrive (VOID);

BOOL WINAPI MfgTestReboot (LPCSTR Name);

#endif
