/***************************************************************************
 *
 *  Copyright (C) 1997-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dscommon.h
 *  Content:    DirectSound common header file.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/10/01    dereks  Created.
 *
 ***************************************************************************/

#ifndef __DSCOMMON_H__
#define __DSCOMMON_H__

//
// Preprocessor definitions
//

#if DBG && !defined(DEBUG)
#define DEBUG
#endif

#if defined(DEBUG) && !defined(VALIDATE_PARAMETERS)
#define VALIDATE_PARAMETERS
#endif

//
// Public includes
//

#ifdef _XBOX

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>
#include <pci.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#define NODSOUND
#include <xtl.h>
#undef NODSOUND

#ifdef DSNDVER
#include <dsndver.h>
#endif // DSNDVER

#include <xboxp.h>
#include <dsoundp.h>

#else // _XBOX

#include <windows.h>
#include <commctrl.h>
#include <objbase.h>
#include <mmsystem.h>
#include <mmreg.h>

#endif // _XBOX

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <xboxverp.h>
#include <tchar.h>

//
// Private includes
//

#include "macros.h"
#include "debug.h"
#include "ntlist.h"
#include "memmgr.h"
#include "format.h"
#include "drvhlp.h"
#include "refcount.h"
#include "waveldr.h"
#include "imaadpcm.h"

//
// Namespaces
//

#ifdef __cplusplus

using namespace DirectSound;

#endif // __cplusplus

#endif // __DSCOMMON_H__
