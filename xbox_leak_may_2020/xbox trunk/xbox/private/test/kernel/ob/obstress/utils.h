/*++

Copyright (c) 1997-2000  Microsoft Corporation

Module Name:

    utils.h

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

#ifndef __HTSTRESS_UTILS_HXX_INCLUDED__
#define __HTSTRESS_UTILS_HXX_INCLUDED__


#define ARRAY_LENGTH( array )   ( sizeof(array)/sizeof(array[0]) )


VOID
__cdecl 
_DbgMessage (
    LPCSTR Format,
    ...
    );


#endif // __HTSTRESS_UTILS_HXX_INCLUDED__
