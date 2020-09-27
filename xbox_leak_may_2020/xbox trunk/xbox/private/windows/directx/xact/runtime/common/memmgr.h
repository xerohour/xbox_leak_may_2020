/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       memmgr.h
 *  Content:    DirectSound memory manager.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/21/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __MEMMGR_H__
#define __MEMMGR_H__

#include "macros.h"

EXTERN_C LPVOID XactTrackMemAlloc(LPCSTR pszFile, ULONG nLine, LPCSTR pszClass, ULONG cbBuffer, BOOL fZeroInit);
EXTERN_C void XactTrackMemFree(LPVOID pvBuffer);

EXTERN_C LPVOID XactMemAlloc(ULONG cbBuffer, BOOL fZeroInit);
EXTERN_C void XactMemFree(LPVOID pvBuffer);


#endif // __MEMMGR_H__
