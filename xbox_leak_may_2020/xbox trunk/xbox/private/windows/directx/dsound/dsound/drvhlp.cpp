/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       drvhlp.cpp
 *  Content:    Miscelaneous NT-style driver helper functions and objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/07/01    dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"

DWORD CFpState::m_dwRefCount = 0;
KFLOATING_SAVE CFpState::m_fps;


