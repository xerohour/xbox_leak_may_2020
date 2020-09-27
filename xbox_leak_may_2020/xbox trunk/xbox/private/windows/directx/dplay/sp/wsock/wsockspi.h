/***************************************************************************
 *
 *  Copyright (C) 1997-1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dnaddri.h
 *  Content:    SPWSOCK master internal header file.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  07/28/00    dereks  Created.
 *  09/11/00	mgere	Converted to DPlay
 *
 ***************************************************************************/

#ifndef __SPWSOCKI_H__
#define __SPWSOCKI_H__

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

#include <xtl.h>
#include <DPlay8p.h>
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

// WINSOCK DEFINES

#define p_ntohs ntohs
#define p_htonl htonl
#define p_gethostname gethostname
#define p_WSAGetLastError WSAGetLastError
#define p_gethostbyname gethostbyname
#define p_htons htons
#define p_setsockopt setsockopt
#define p_inet_addr inet_addr
#define p_socket socket
#define p_bind bind
#define p_closesocket closesocket
#define p_getsockname getsockname
#define p_select select
#define p_recvfrom recvfrom
#define p_sendto sendto
#define p___WSAFDIsSet __WSAFDIsSet
#define p_inet_ntoa inet_ntoa
#define p_WSASendTo WSASendTo
#define p_WSARecvFrom WSARecvFrom
#define p_ioctlsocket ioctlsocket
#define p_shutdown shutdown
#define p_WSAGetOverlappedResult WSAGetOverlappedResult
#define p_getsockopt getsockopt

//
// Private includes
//

#include "DNDbg.h"
#include "DNetErrors.h"
#include "debugutils.h"
#include "OSInd.h"
#include "comutil.h"
#include "ClassBilink.h"
#include "ClassFPM.h"
#include "FPM.h"
#include "ClassHash.h"
#include "ContextCFPM.h"
#include "LockedContextFixedPool.h"
#include "LockedPool.h"
#include "PackBuff.h"
#include "StrUtils.h"
#include "LockedCFPM.h"

#include "createin.h"
#include "wsocksp.h"
#include "locals.h"
#include "adapterentry.h"
#include "cmddata.h"
#include "messagestructures.h"
#include "handletables.h"
#include "iodata.h"
#include "poolss.h"
#include "spaddress.h"
#include "ipaddress.h"
#include "sendqueue.h"
//#include "rsip.h"
#include "socketport.h"
#include "jobqueue.h"
#include "threadpool.h"
#include "utils.h"
#include "spdata.h"
#include "endpoint.h"
#include "ipendpt.h"


#endif // __SPWSOCKI_H__
