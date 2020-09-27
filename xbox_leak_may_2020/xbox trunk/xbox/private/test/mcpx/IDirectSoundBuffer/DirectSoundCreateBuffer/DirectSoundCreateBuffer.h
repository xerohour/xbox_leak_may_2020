/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       DirectSoundCreateBuffer.h
 *  Content:    DirectSoundCreateBuffer utility exports
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/11/01    danrose Created to test DirectSoundCreateBuffer
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>

HRESULT DirectSoundCreateBuffer( LPDIRECTSOUNDBUFFER* ppBuffer, LPWAVEFORMATEX pWfx = NULL, LPDSBUFFERDESC pDsbd = NULL );