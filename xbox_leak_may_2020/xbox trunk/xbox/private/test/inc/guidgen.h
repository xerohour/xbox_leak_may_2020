/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  guidgen.h

Abstract:

  This module contains the definitions for guidgen.dll

Author:

  Steven Kehrli (steveke) 30-Nov-2000

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#ifndef _GUIDGEN_H
#define _GUIDGEN_H

#include <xtl.h>



#ifdef __cplusplus
extern "C" {
#endif



// Function Prototypes

HRESULT
xCreateGUID(
    OUT GUID  *Guid
);

#ifdef __cplusplus
}
#endif

#endif
