/*++

  Copyright (c) 2000 Microsoft Corporation

  Module Name:

	dllmain.cpp

  Abstract:

	TBD

  Author:

	Toby Dziubala (a-tobydz) 22-May-2000

  Enviornment:

	Windows 2000

  Revision History:
	
	  - First Version:	20-April-2000
	  - XBox Version:	22-May-2000

--*/

#include <xtl.h>
#include "dll.h"
#include "prototypes.h"
#include "xtestlib.h"


//
// Export function pointers of StartTest and EndTest
//

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( xmmapi )
#pragma data_seg()

BEGIN_EXPORT_TABLE( xmmapi )
    EXPORT_TABLE_ENTRY( "StartTest", XMmApiStartTest )
    EXPORT_TABLE_ENTRY( "EndTest", XMmApiEndTest )
END_EXPORT_TABLE( xmmapi )
