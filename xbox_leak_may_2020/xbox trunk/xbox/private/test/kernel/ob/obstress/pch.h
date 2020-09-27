/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    pch.h

Abstract:

    This program contains routines to stress object manager

Author:

    Silviu Calinoiu (silviuc) 18-Mar-1997

Environment:

    Xbox

Revision History:

    04-May-2000     schanbai

        Ported to Xbox

--*/

#ifndef __HTSTRESS_PCH_HXX__
#define __HTSTRESS_PCH_HXX__

extern "C" {
#include <nt.h>
#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
}

#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define DbgMessage _DbgMessage

#endif // __HTSTRESS_PCH_HXX__
