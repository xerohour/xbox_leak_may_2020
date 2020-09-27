// FILE:       library\hardware\mpeg2enc\specific\iVacDev.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		21.10.1999
//
// PURPOSE: 	Interface between iVac API and the CineMaster Capture board class --- Header file
//
// HISTORY:
//

#ifndef IVACDEV_H
#define IVACDEV_H



#include "cinmenc.h"

#ifdef __cplusplus
extern "C"
{
#endif

_EXTERN void SetBoard(CineMasterCapture * capBoard);

#ifdef __cplusplus
}
#endif


#endif // IVACDEV_H