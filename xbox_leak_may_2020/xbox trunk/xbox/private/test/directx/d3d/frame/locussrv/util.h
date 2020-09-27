/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    util.h

Description:

    Utility functions.

*******************************************************************************/

#ifndef __UTIL_H__
#define __UTIL_H__

//******************************************************************************
// Debugging function prototype
//******************************************************************************

#ifdef _DEBUG
void                DebugString(LPCTSTR szFormat, ...);
#else
inline void         DebugString(LPCTSTR szFormat, ...) {}
#endif

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
// Miscellaneous prototypes
//******************************************************************************

DWORD               FormatToBitDepth(D3DFORMAT d3dfmt);
BOOL                IsCompressedFormat(D3DFORMAT d3dfmt);
float               DW2F(DWORD dw);

#endif // __UTIL_H__
