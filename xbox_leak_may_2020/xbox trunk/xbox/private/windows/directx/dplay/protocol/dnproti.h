/***************************************************************************
 *
 *  Copyright (C) 1997-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dnaddri.h
 *  Content:    DirectPlayAddress master internal header file.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/28/00    dereks  Created.
 *  09/11/00	mgere	Converted to DPlay
 *
 ***************************************************************************/

#ifndef __DNPROTI_H__
#define __DNPROTI_H__

//
// Public includes
//

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include <xdbg.h>
#include <xtl.h>
#ifdef ENABLE_DPLAY_VOICE
#include <dvoicep.h>
#endif
#include <dplay8p.h>
#include <DPSP8p.h>
#ifdef DPLAY_DOWORK_STATEMN
#include <statemn.h>
#endif

//
// Private includes
//

class DIRECTNETOBJECT;
class CHandleTable;
class CNameTable;
class CAsyncOp;
class CConnection;
class CNameTableEntry;
class CSyncEvent;
class CNameTableOp;
class CGroupConnection;
class CGroupMember;
class CPendingDeletion;
class CQueuedMsg;
class CWorkerJob;
class CMemoryBlockTiny;
class CMemoryBlockSmall;
class CMemoryBlockMedium;
class CMemoryBlockLarge;
class CMemoryBlockHuge;

typedef	struct DPN_APPLICATION_DESC_INFO DPN_APPLICATION_DESC_INFO;
typedef struct _HANDLETABLE_ARRAY_ENTRY HANDLETABLE_ARRAY_ENTRY;

#include "dndbg.h"
#include "DNetErrors.h"
#include "OSInd.h"
#include "handletable.h"
#include "guidutil.h"
#include "bilink.h"
#include "Classbilink.h"
#include "fpm.h"
#include "PackBuff.h"
#include "LockedCCfpm.h"
#include "RCBuffer.h"
#include "StrUtils.h"
#include "Queue.h"
#include "LockedCCfpm.h"
#include "appdesc.h"
#include "nametable.h"
#include "MessageStructures.h"
#include "jobqueue.h"
#include "locals.h"
#include "iodata.h"
#include "contextcfpm.h"
#include "threadpool.h"
#include "createin.h"
#include "comutil.h"
#include "DNProt.h"
#include "frames.h"
#include "internal.h"
#include "DNPExtern.h"
#include "mytimer.h"
#include "DNCore.h"


#endif // __DNPROTI_H__
