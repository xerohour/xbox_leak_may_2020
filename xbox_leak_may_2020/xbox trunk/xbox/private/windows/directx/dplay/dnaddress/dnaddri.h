/***************************************************************************
 *
 *  Copyright (C) 1997-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dnaddri.h
 *  Content:    DirectPlayAddress master internal header file.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/28/00    dereks  Created.
 *  09/11/00	mgere	Converted to DPlay
 *
 ***************************************************************************/

#ifndef __DNADDRI_H__
#define __DNADDRI_H__

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

#ifdef __cplusplus
}
#endif // __cplusplus

#include <xtl.h>
#include <winsockx.h>
#include <dplay8p.h>
#include <dpaddrp.h>
#include <xdbg.h>
#include <stdio.h>
#ifdef DPLAY_DOWORK_STATEMN
#include <statemn.h>
#endif

#ifdef __cplusplus

template <class type> type *__AddRef(type *p)
{
    if(p)
    {
        p->AddRef();
    }

    return p;
}

#define ADDREF(p) \
    __AddRef(p)

template <class type> void __Release(type **pp)
{
    type * p = *pp;

    if(p)
    {
        p->Release();
    }
}

#define RELEASE(p) \
    __Release(&(p))

#endif // __cplusplus

//
// Private includes
//

#include "dndbg.h"
#include "osind.h"
#include "comutil.h"
#include "strutils.h"
#include "classbilink.h"
#include "fpm.h"
#include "strcache.h"
#include "addcore.h"
#include "addparse.h"

#endif // __DSOUNDI_H__
