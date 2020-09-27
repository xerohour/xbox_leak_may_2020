/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    sctrsz.h

Abstract: Filter driver to change the sector size. based on ervinp's boiler plate
code in the DDK.

Author:

    mitchd

Environment:

    Kernel mode

Revision History:


--*/

//
//	Extension to the boiler plate devision extension
//
typedef struct _SCTRSZ_EXT
{
	

} SCTRSZ_EXT, *PSCTRSZ_EXT;

#include "filter.h"

//
//	Declaration of filter routine
//


NSTATUS
SCTRSZ_Ioctl(
	devExt,
	Irp,
	&passIrpDown);