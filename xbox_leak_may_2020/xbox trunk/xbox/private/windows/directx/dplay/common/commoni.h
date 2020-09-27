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

#ifndef __COMMONI_H__
#define __COMMONI_H__

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
#include <stdio.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include <xtl.h>
#ifdef ENABLE_DPLAY_VOICE
#include <Dvoicep.h>
#endif
#include <xdbg.h>
#include <dplay8p.h>
#include <DPSP8p.h>
#include <DPAddrp.h>
#ifdef DPLAY_DOWORK_STATEMN
#include <statemn.h>
#endif

#ifdef __cplusplus

template <class type> type *__AddRef(type *p)
{
    if(p)
    {
        p->AddRef();
    }

    return p;
}

#define ADDREF(p) \
    __AddRef(p)

template <class type> void __Release(type *p)
{
    if(p)
    {
        p->Release();
    }
}

#define RELEASE(p) \
    __Release(p), p = NULL

#endif // __cplusplus

//
// Private includes
//

typedef	struct DPN_APPLICATION_DESC_INFO DPN_APPLICATION_DESC_INFO;
typedef struct _HANDLETABLE_ARRAY_ENTRY HANDLETABLE_ARRAY_ENTRY;
typedef struct IDirectPlay8Address	IDirectPlay8Address;

class DIRECTNETOBJECT;
class CWorkerJob;
class CSyncEvent;
class CNameTableOp;
class CPendingDeletion;
class CQueuedMsg;

#include "dndbg.h"
#include "DNetErrors.h"
#include "OSInd.h"
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
#include "createin.h"
#include "comutil.h"
#include "Connection.h"
#include "NTEntry.h"
#include "Async.h"
#include "AsyncOp.h"
#include "GroupCon.h"
#include "GroupMem.h"
#include "dnpextern.h"
#include "dnprot.h"

#include "message.h"
#include "AppDesc.h"
#include "NameTable.h"
#include "HandleTable.h"
#include "Memoryfpm.h"
#include "paramval.h"
#include "Cancel.h"
#include "EnumHosts.h"
#include "handles.h"
#include "DNCore.h"
#include "MessageStructures.h"
#include "jobqueue.h"
#include "locals.h"
#include "iodata.h"
#include "contextcfpm.h"
#include "threadpool.h"
#include "SyncEvent.h"
#include "WorkerJob.h"
#include "NTOp.h"
#include "PendingDel.h"
#include "QueuedMsg.h"

#endif // __COMMONI_H__
