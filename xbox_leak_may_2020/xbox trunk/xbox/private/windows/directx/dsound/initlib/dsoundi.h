/***************************************************************************
 *
 *  Copyright (C) 1997-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsoundi.h
 *  Content:    DirectSound master internal header file.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/28/00    dereks  Created.
 *
 ***************************************************************************/

#ifndef __DSOUNDI_H__
#define __DSOUNDI_H__

//
// Put all code and data into a DSOUND section
//

#pragma code_seg("DSOUND")
#pragma data_seg("DSOUND_RW")
#pragma const_seg("DSOUND_RD")
#pragma bss_seg("DSOUND_URW")

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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
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
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

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
#include "heap.h"
#include "ac97.h"
#include "dsp.h"
#include "cipher.h"
#include "dsapi.h"
#include "dsperf.h"
#include "mcpxcore.h"

#endif // __DSOUNDI_H__
