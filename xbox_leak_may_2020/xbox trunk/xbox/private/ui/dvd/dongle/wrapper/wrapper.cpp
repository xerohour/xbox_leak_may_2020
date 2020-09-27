/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    wrapper.cpp

Abstract:

    This module contains templates used to generate imports library for
    module in hardware key.

--*/

#define EXTERN_C extern "C"

typedef void *P;

#define ARGUMENT0
#define ARGUMENT1   P
#define ARGUMENT2   P, P
#define ARGUMENT3   P, P, P
#define ARGUMENT4   P, P, P, P
#define ARGUMENT5   P, P, P, P, P
#define ARGUMENT6   P, P, P, P, P, P
#define ARGUMENT7   P, P, P, P, P, P, P
#define ARGUMENT8   P, P, P, P, P, P, P, P
#define ARGUMENT9   P, P, P, P, P, P, P, P, P
#define ARGUMENT10  P, P, P, P, P, P, P, P, P, P

EXTERN_C void __stdcall dummy0(  ARGUMENT0  ) {}
EXTERN_C void __stdcall dummy1(  ARGUMENT1  ) {}
EXTERN_C void __stdcall dummy2(  ARGUMENT2  ) {}
EXTERN_C void __stdcall dummy3(  ARGUMENT3  ) {}
EXTERN_C void __stdcall dummy4(  ARGUMENT4  ) {}
EXTERN_C void __stdcall dummy5(  ARGUMENT5  ) {}
EXTERN_C void __stdcall dummy6(  ARGUMENT6  ) {}
EXTERN_C void __stdcall dummy7(  ARGUMENT7  ) {}
EXTERN_C void __stdcall dummy8(  ARGUMENT8  ) {}
EXTERN_C void __stdcall dummy9(  ARGUMENT9  ) {}
EXTERN_C void __stdcall dummy10( ARGUMENT10 ) {}

EXTERN_C void __cdecl   cdummy() {}