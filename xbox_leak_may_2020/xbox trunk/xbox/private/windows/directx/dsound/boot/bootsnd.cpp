/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       bootsnd.cpp
 *  Content:    DirectSound bootsound source.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/19/01    dereks  Created.
 *
 ****************************************************************************/

#include "bootsnd.h"

#pragma comment(linker, "/merge:DSOUND_RW=DSOUND")
#pragma comment(linker, "/merge:DSOUND_RD=DSOUND")
#pragma comment(linker, "/section:INIT,ERW")

#include "..\dsound\dscommon.cpp"
#include "..\dsound\dsapi.cpp"
#include "..\dsound\mcpbuf.cpp"
#include "..\dsound\mcpapu.cpp"
#include "..\dsound\mcpvoice.cpp"
#include "..\dsound\mcpxcore.cpp"
#include "..\dsound\dspdma.cpp"
#include "..\dsound\gpdsp.cpp"
#include "..\dsound\heap.cpp"
#include "..\dsound\globals.c"
#include "..\ac97\ac97.cpp"
