/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       makeini.h
 *  Content:    makeini utility interface
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  04/ 3/01    danrose Created to test Xbox makeini
 *  01/30/02    danhaff Added makeini_WFVOTest
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>
#include <stdio.h>

HRESULT makeini_BasicTest( void );
HRESULT makeini_WFVOTest( void );

