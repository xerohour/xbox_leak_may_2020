/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DirectSoundCreate.h
 *  Content:    DirectSoundCreate utility exports
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/11/01    danrose Created to test DirectSoundCreate
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>

HRESULT DirectSoundCreate( LPDIRECTSOUND* ppDirectSound );