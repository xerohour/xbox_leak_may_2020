/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       onlinebvt.h
 *  Content:    onlinebvt utility interface
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *  12/10/01    tristanj Created to test Xbox onlinebvt
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xlog.h>
#include <macros.h>
#include <xtestlib.h>
#include <xnetref.h>
#include "LogonTask.h"
#include "FriendEnumTask.h"
#include "MatchSessionCreateTask.h"
#include "OfferingEnumerateTask.h"
#include "ContentInstallTask.h"
#include "ContentVerifyTask.h"
#include "ContentRemoveTask.h"
#include "MatchSearchTask.h"
#include "OnlineAccounts.h"
#include "OnlineTimer.h"

#define MAX_LOGON_TIME           30000
#define MAX_FRIEND_ENUM_TIME      10000
#define MAX_SESSION_CREATE_TIME  10000
