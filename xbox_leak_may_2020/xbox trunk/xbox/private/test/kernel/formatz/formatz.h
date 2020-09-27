/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    formatz.h

Abstract:

    test out the XFormatUtilityDrive() API
    
Author:

    John Daly (johndaly) 23-jan-2002

Revision History:

--*/

#ifndef _FORMATZ_
#define _FORMATZ_

//
// includes
//

#include <stdio.h>
#include <xapip.h>
#include <xboxp.h>
#include <xlog.h>
#include <xtestlib.h>

//
// function declarations
//

VOID
WINAPI
test_XFormatUtilityDriveNoFile(
    void
    );

VOID
WINAPI
test_XFormatUtilityDriveOneFile(
    void
    );

void 
__cdecl 
main(
    void
    );

#endif // _FORMATZ_


