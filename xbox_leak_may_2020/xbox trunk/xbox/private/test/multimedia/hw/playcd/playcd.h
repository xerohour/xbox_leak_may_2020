/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       playcd.h
 *  Content:    playcd utility interface
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/12/01    danrose Created to test Xbox playcd
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>

#include "ntiosvc.h"
#include "cdda.h"

#ifdef CAPTURE
#include <waveout.h>
#endif // CAPTURE
