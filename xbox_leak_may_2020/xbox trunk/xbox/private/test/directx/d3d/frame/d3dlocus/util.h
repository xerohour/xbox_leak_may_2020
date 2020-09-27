/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    util.h

Author:

    Matt Bronder

Description:

    Utility functions.

*******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

//******************************************************************************
// Preprocessor definitions
//******************************************************************************

#define TSTART_HARNESS          0x00000001
#define TSTART_SEQUENTIAL       0x00000002
#define TSTART_PARALLEL         0x00000004
#define TSTART_STRESS           0x00000008

//******************************************************************************
// Data types
//******************************************************************************

typedef DWORD                   TESTSTARTUPCONTEXT;

//******************************************************************************
// Debugging function prototypes
//******************************************************************************

//#if defined(DEBUG) || defined(_DEBUG) || defined(DBG)
void                DebugString(LPCTSTR szFormat, ...);
BOOL                ResultFailed(HRESULT hr, LPCTSTR sz);
//#else
//inline void         DebugString(LPCTSTR szFormat, ...) {}
//inline BOOL         ResultFailed(HRESULT hr, LPCTSTR sz) {return FALSE;};
//#endif

//******************************************************************************
// Memory allocation prototypes
//******************************************************************************

BOOL                CreateHeap();
void                ReleaseHeap();
LPVOID              MemAlloc(UINT uNumBytes);
void                MemFree(LPVOID pvMem);
LPVOID              MemAlloc32(UINT uNumBytes);
void                MemFree32(LPVOID pvMem);

//******************************************************************************
// Test startup information
//******************************************************************************

void                RegisterStartupContext(TESTSTARTUPCONTEXT tscontext);
TESTSTARTUPCONTEXT  GetStartupContext();

//******************************************************************************
// Display information
//******************************************************************************

void                RegisterDisplay(class CDisplay* pDisplay);
CDisplay*           GetDisplay();
void                EnableDisplayStability(BOOL bEnable);
BOOL                IsDisplayStable();

//******************************************************************************
// Test module instance
//******************************************************************************

void                RegisterTestInstance(HINSTANCE hInstance);
HINSTANCE           GetTestInstance();
DWORD               GetModuleName(HMODULE hModule, LPTSTR szModuleName, DWORD dwSize);

//******************************************************************************
// CMap32
//******************************************************************************

//******************************************************************************
typedef struct _PAIR32 {
    DWORD           dwKey;
    DWORD           dwData;
    struct _PAIR32* ppairNext;
} PAIR32, *PPAIR32;

//******************************************************************************
class CMap32 {

protected:

                    PPAIR32 m_ppairList;

public:

                    CMap32();
                    ~CMap32();
    BOOL            Add(DWORD dwKey, DWORD dwData);
    BOOL            Remove(DWORD dwKey);
    BOOL            Find(DWORD dwKey, LPDWORD pdwData);
    BOOL            Find(LPDWORD pdwKey, DWORD dwData);
};

//******************************************************************************
// Miscellaneous functions
//******************************************************************************

inline void         F2L(int* pint, float f) {
                        __asm fld f;
                        __asm mov edx, pint
                        __asm fistp [edx]
                    }

#endif // __UTIL_H__
