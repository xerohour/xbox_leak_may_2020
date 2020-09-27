//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1996-1999. All rights reserved.
//
//  MODULE: NMFinder.h
//
//  External Header for the NPP finder
//=============================================================================

#ifndef NMFINDER_H
#define NMFINDER_H

#include <NPPTypes.h>

#ifdef __cplusplus
extern "C"
{
#endif

// FINDER errors -- see also nmerr.h, blob.h
#define NMERR_NO_NPPS                       3016

#define NMERR_NO_MATCHING_NPPS              3017

#define NMERR_NO_NPP_SELECTED               3018

#define NMERR_NO_INPUT_BLOBS                3019

#define NMERR_NO_NPP_DLLS                   3020

#define NMERR_NO_VALID_NPP_DLLS             3021
//****************************************************************************
// Structures use by NPPs, the Finder, and monitors
//****************************************************************************

#pragma warning(disable:4200)
typedef struct
{
    DWORD         dwNumBlobs;
    HBLOB         hBlobs[0];
} BLOB_TABLE;
typedef BLOB_TABLE* PBLOB_TABLE;

typedef struct 
{
	DWORD size;
	BYTE* pBytes;
} MBLOB;

typedef struct
{
    DWORD         dwNumBlobs;
    MBLOB         mBlobs[0];
} MBLOB_TABLE;
typedef MBLOB_TABLE* PMBLOB_TABLE;

#pragma warning(default:4200)

//****************************************************************************
// Functions called by monitors, tools, netmon
//****************************************************************************
DWORD _cdecl GetNPPBlobTable(HBLOB          hFilterBlob,  
                      PBLOB_TABLE*   ppBlobTable);

DWORD _cdecl GetNPPBlobFromUI(HWND          hwnd,
                       HBLOB         hFilterBlob,
                       HBLOB*        phBlob);          

DWORD _cdecl GetNPPBlobFromUIExU(HWND          hwnd,
                          HBLOB         hFilterBlob,
                          HBLOB*        phBlob,
                          char*         szHelpFileName);          

DWORD _cdecl SelectNPPBlobFromTable( HWND   hwnd,
                              PBLOB_TABLE    pBlobTable,
                              HBLOB*         hBlob);

DWORD _cdecl SelectNPPBlobFromTableExU( HWND   hwnd,
                                 PBLOB_TABLE    pBlobTable,
                                 HBLOB*         hBlob,
                                 char*          szHelpFileName);


//****************************************************************************
// Helper functions provided by the Finder
//****************************************************************************
__inline DWORD BLOB_TABLE_SIZE(DWORD dwNumBlobs)
{
    return (DWORD) (sizeof(BLOB_TABLE)+dwNumBlobs*sizeof(HBLOB));
}

__inline PBLOB_TABLE  AllocBlobTable(DWORD dwNumBlobs)
{
    DWORD size = BLOB_TABLE_SIZE(dwNumBlobs);

    return (PBLOB_TABLE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

__inline DWORD MBLOB_TABLE_SIZE(DWORD dwNumBlobs)
{
    return (DWORD) (sizeof(MBLOB_TABLE)+dwNumBlobs*sizeof(MBLOB));
}

__inline PMBLOB_TABLE  AllocMBlobTable(DWORD dwNumBlobs)
{
    DWORD size = MBLOB_TABLE_SIZE(dwNumBlobs);

    return (PMBLOB_TABLE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

//****************************************************************************
// Functions provided by NPPs, called by the Finder
//****************************************************************************

// For NPP's that can return a Blob table without additional configuration.
DWORD _cdecl GetNPPBlobs(PBLOB_TABLE*       ppBlobTable);

typedef DWORD (_cdecl FAR* BLOBSPROC) (PBLOB_TABLE*       ppBlobTable);

// For NPP's that need additional information to return a Blob table.
DWORD _cdecl GetConfigBlob(HBLOB*      phBlob);

typedef DWORD (_cdecl FAR* GETCFGBLOB) (HBLOB, HBLOB*);
typedef DWORD (_cdecl FAR* CFGPROC) (HWND               hwnd,
                              HBLOB              SpecialBlob,
                              PBLOB_TABLE*       ppBlobTable);

//****************************************************************************
// Handy functions
BOOL  _cdecl FilterNPPBlob(HBLOB hBlob, HBLOB FilterBlob);

BOOL  _cdecl RaiseNMEvent(HINSTANCE    hInstance,
                   WORD         EventType, 
                   DWORD        EventID,
                   WORD         nStrings, 
                   const char** aInsertStrs,
                   LPVOID       lpvData,
                   DWORD        dwDataSize);

#ifdef __cplusplus
}
#endif

#endif