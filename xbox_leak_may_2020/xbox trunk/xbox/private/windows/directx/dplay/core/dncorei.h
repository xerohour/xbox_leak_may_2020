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

#ifndef __DNCOREI_H__
#define __DNCOREI_H__

//
// Public includes
//

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
//#include <stddef.h>
#include <ntos.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#include <xtl.h>
#include <xobjbase.h>
#ifdef ENABLE_DPLAY_VOICE
#include <Dvoicep.h>
#endif
#include <xdbg.h>
#include <dplay8p.h>
#include <DPAddrp.h>
#include <DPSP8p.h>
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

template <class type> void __Release(type **pp)
{
    type * p = *pp;

    if(p)
    {
        p->Release();
    }
}

#define RELEASE(p) \
    __Release(&(p))

#endif // __cplusplus

//
// Private includes
//

#include "dndbg.h"
#include "DNetErrors.h"
#include "OSInd.h"
#include "ClassBilink.h"
#include "fpm.h"
#include "PackBuff.h"
#include "LockedCCFPM.h"
#include "RCBuffer.h"
#include "dnpextern.h"
#include "dnprot.h"
#include "comstuff.h"
#include "comutil.h"

typedef	struct DPN_APPLICATION_DESC_INFO DPN_APPLICATION_DESC_INFO;
typedef struct _HANDLETABLE_ARRAY_ENTRY HANDLETABLE_ARRAY_ENTRY;
typedef struct IDirectPlay8Address	IDirectPlay8Address;

class DIRECTNETOBJECT;
class CPackedBuffer;
class CRefCountBuffer;
class CSyncEvent;
class CConnection;
class CGroupConnection;
class CGroupMember;
class CNameTableEntry;
class CNameTableOp;
class CAsyncOp;
class CPendingDeletion;
class CQueuedMsg;
class CWorkerJob;
class CMemoryBlockTiny;
class CMemoryBlockSmall;
class CMemoryBlockMedium;
class CMemoryBlockLarge;
class CMemoryBlockHuge;

#include "classfac.h"
#include "message.h"
#include "receive.h"
#include "NameTable.h"
#include "servprov.h"
#include "user.h"
#include "pools.h"
#include "worker.h"
#include "connect.h"
#include "ntoplist.h"
#include "request.h"
#include "common.h"
#include "dpprot.h"
#include "protocol.h"

#ifdef ENABLE_DPLAY_VOICE
#include "voice.h"
#endif

#include "spmessages.h"
#include "AppDesc.h"
#include "enum_sp.h"
#include "Async.h"
#include "AsyncOp.h"
#include "Caps.h"
#include "Connection.h"
#include "GroupCon.h"
#include "MemoryFPM.h"
#include "NTEntry.h"
#include "paramval.h"
#include "GroupMem.h"
#include "Cancel.h"
#include "EnumHosts.h"
#include "HandleTable.h"
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

#endif // __DNCOREI_H__
