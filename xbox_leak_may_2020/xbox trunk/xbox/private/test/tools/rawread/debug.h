/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.h
 *  Content:    Debugger helpers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  05/15/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG
#endif  // DEBUG
#endif // _DEBUG

#ifdef DEBUG
extern void dprintf(const char *pszFormat, ...);
#define DPF dprintf
#else // DEBUG
#pragma warning(disable:4002)
#define DPF(a)
#endif // DEBUG

#endif // __DEBUG_H__
