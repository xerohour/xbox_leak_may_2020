/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    crttestp.h

Abstract:

    Pre-compiled header file for CRT test harness

--*/

#include <ntos.h>
#include <xtl.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cdecl PlumHallStartTest( void );
void __cdecl SlConfrmStartTest( void );

#ifdef __cplusplus
}
#endif
