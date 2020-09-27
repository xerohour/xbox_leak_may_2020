/*++

Copyright (c) 1996-1999 Microsoft Corporation

Module Name:

    mqoai.h

Abstract:

    Master include file for MSMQ COM objects

--*/

#ifndef __MQOAI_H__
#define __MQOAI_H__

#if _MSC_VER > 1000
#pragma once
#endif


#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0286 */
/* at Tue Jul 11 17:47:22 2000
 */
/* Compiler settings for mqoa.odl:
    Os (OptLev=s), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef ___mqoai_h__
#define ___mqoai_h__

/* Forward Declarations */ 

#ifndef __IMSMQQuery_FWD_DEFINED__
#define __IMSMQQuery_FWD_DEFINED__
typedef interface IMSMQQuery IMSMQQuery;
#endif 	/* __IMSMQQuery_FWD_DEFINED__ */


#ifndef __IMSMQQueueInfo_FWD_DEFINED__
#define __IMSMQQueueInfo_FWD_DEFINED__
typedef interface IMSMQQueueInfo IMSMQQueueInfo;
#endif 	/* __IMSMQQueueInfo_FWD_DEFINED__ */


#ifndef __IMSMQQueueInfo2_FWD_DEFINED__
#define __IMSMQQueueInfo2_FWD_DEFINED__
typedef interface IMSMQQueueInfo2 IMSMQQueueInfo2;
#endif 	/* __IMSMQQueueInfo2_FWD_DEFINED__ */


#ifndef __IMSMQQueueInfo3_FWD_DEFINED__
#define __IMSMQQueueInfo3_FWD_DEFINED__
typedef interface IMSMQQueueInfo3 IMSMQQueueInfo3;
#endif 	/* __IMSMQQueueInfo3_FWD_DEFINED__ */


#ifndef __IMSMQQueue_FWD_DEFINED__
#define __IMSMQQueue_FWD_DEFINED__
typedef interface IMSMQQueue IMSMQQueue;
#endif 	/* __IMSMQQueue_FWD_DEFINED__ */


#ifndef __IMSMQQueue2_FWD_DEFINED__
#define __IMSMQQueue2_FWD_DEFINED__
typedef interface IMSMQQueue2 IMSMQQueue2;
#endif 	/* __IMSMQQueue2_FWD_DEFINED__ */


#ifndef __IMSMQMessage_FWD_DEFINED__
#define __IMSMQMessage_FWD_DEFINED__
typedef interface IMSMQMessage IMSMQMessage;
#endif 	/* __IMSMQMessage_FWD_DEFINED__ */


#ifndef __IMSMQQueueInfos_FWD_DEFINED__
#define __IMSMQQueueInfos_FWD_DEFINED__
typedef interface IMSMQQueueInfos IMSMQQueueInfos;
#endif 	/* __IMSMQQueueInfos_FWD_DEFINED__ */


#ifndef __IMSMQQueueInfos2_FWD_DEFINED__
#define __IMSMQQueueInfos2_FWD_DEFINED__
typedef interface IMSMQQueueInfos2 IMSMQQueueInfos2;
#endif 	/* __IMSMQQueueInfos2_FWD_DEFINED__ */


#ifndef __IMSMQQueueInfos3_FWD_DEFINED__
#define __IMSMQQueueInfos3_FWD_DEFINED__
typedef interface IMSMQQueueInfos3 IMSMQQueueInfos3;
#endif 	/* __IMSMQQueueInfos3_FWD_DEFINED__ */


#ifndef __IMSMQEvent_FWD_DEFINED__
#define __IMSMQEvent_FWD_DEFINED__
typedef interface IMSMQEvent IMSMQEvent;
#endif 	/* __IMSMQEvent_FWD_DEFINED__ */


#ifndef __IMSMQEvent2_FWD_DEFINED__
#define __IMSMQEvent2_FWD_DEFINED__
typedef interface IMSMQEvent2 IMSMQEvent2;
#endif 	/* __IMSMQEvent2_FWD_DEFINED__ */


#ifndef __IMSMQEvent3_FWD_DEFINED__
#define __IMSMQEvent3_FWD_DEFINED__
typedef interface IMSMQEvent3 IMSMQEvent3;
#endif 	/* __IMSMQEvent3_FWD_DEFINED__ */


#ifndef __IMSMQTransaction_FWD_DEFINED__
#define __IMSMQTransaction_FWD_DEFINED__
typedef interface IMSMQTransaction IMSMQTransaction;
#endif 	/* __IMSMQTransaction_FWD_DEFINED__ */


#ifndef __IMSMQCoordinatedTransactionDispenser_FWD_DEFINED__
#define __IMSMQCoordinatedTransactionDispenser_FWD_DEFINED__
typedef interface IMSMQCoordinatedTransactionDispenser IMSMQCoordinatedTransactionDispenser;
#endif 	/* __IMSMQCoordinatedTransactionDispenser_FWD_DEFINED__ */


#ifndef __IMSMQTransactionDispenser_FWD_DEFINED__
#define __IMSMQTransactionDispenser_FWD_DEFINED__
typedef interface IMSMQTransactionDispenser IMSMQTransactionDispenser;
#endif 	/* __IMSMQTransactionDispenser_FWD_DEFINED__ */


#ifndef __IMSMQQuery2_FWD_DEFINED__
#define __IMSMQQuery2_FWD_DEFINED__
typedef interface IMSMQQuery2 IMSMQQuery2;
#endif 	/* __IMSMQQuery2_FWD_DEFINED__ */


#ifndef __IMSMQQuery3_FWD_DEFINED__
#define __IMSMQQuery3_FWD_DEFINED__
typedef interface IMSMQQuery3 IMSMQQuery3;
#endif 	/* __IMSMQQuery3_FWD_DEFINED__ */


#ifndef __MSMQQuery_FWD_DEFINED__
#define __MSMQQuery_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQQuery MSMQQuery;
#else
typedef struct MSMQQuery MSMQQuery;
#endif /* __cplusplus */

#endif 	/* __MSMQQuery_FWD_DEFINED__ */


#ifndef __IMSMQMessage2_FWD_DEFINED__
#define __IMSMQMessage2_FWD_DEFINED__
typedef interface IMSMQMessage2 IMSMQMessage2;
#endif 	/* __IMSMQMessage2_FWD_DEFINED__ */


#ifndef __IMSMQMessage3_FWD_DEFINED__
#define __IMSMQMessage3_FWD_DEFINED__
typedef interface IMSMQMessage3 IMSMQMessage3;
#endif 	/* __IMSMQMessage3_FWD_DEFINED__ */


#ifndef __MSMQMessage_FWD_DEFINED__
#define __MSMQMessage_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQMessage MSMQMessage;
#else
typedef struct MSMQMessage MSMQMessage;
#endif /* __cplusplus */

#endif 	/* __MSMQMessage_FWD_DEFINED__ */


#ifndef __IMSMQQueue3_FWD_DEFINED__
#define __IMSMQQueue3_FWD_DEFINED__
typedef interface IMSMQQueue3 IMSMQQueue3;
#endif 	/* __IMSMQQueue3_FWD_DEFINED__ */


#ifndef __MSMQQueue_FWD_DEFINED__
#define __MSMQQueue_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQQueue MSMQQueue;
#else
typedef struct MSMQQueue MSMQQueue;
#endif /* __cplusplus */

#endif 	/* __MSMQQueue_FWD_DEFINED__ */


#ifndef __IMSMQPrivateEvent_FWD_DEFINED__
#define __IMSMQPrivateEvent_FWD_DEFINED__
typedef interface IMSMQPrivateEvent IMSMQPrivateEvent;
#endif 	/* __IMSMQPrivateEvent_FWD_DEFINED__ */


#ifndef ___DMSMQEventEvents_FWD_DEFINED__
#define ___DMSMQEventEvents_FWD_DEFINED__
typedef interface _DMSMQEventEvents _DMSMQEventEvents;
#endif 	/* ___DMSMQEventEvents_FWD_DEFINED__ */


#ifndef __MSMQEvent_FWD_DEFINED__
#define __MSMQEvent_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQEvent MSMQEvent;
#else
typedef struct MSMQEvent MSMQEvent;
#endif /* __cplusplus */

#endif 	/* __MSMQEvent_FWD_DEFINED__ */


#ifndef __MSMQQueueInfo_FWD_DEFINED__
#define __MSMQQueueInfo_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQQueueInfo MSMQQueueInfo;
#else
typedef struct MSMQQueueInfo MSMQQueueInfo;
#endif /* __cplusplus */

#endif 	/* __MSMQQueueInfo_FWD_DEFINED__ */


#ifndef __MSMQQueueInfos_FWD_DEFINED__
#define __MSMQQueueInfos_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQQueueInfos MSMQQueueInfos;
#else
typedef struct MSMQQueueInfos MSMQQueueInfos;
#endif /* __cplusplus */

#endif 	/* __MSMQQueueInfos_FWD_DEFINED__ */


#ifndef __IMSMQTransaction2_FWD_DEFINED__
#define __IMSMQTransaction2_FWD_DEFINED__
typedef interface IMSMQTransaction2 IMSMQTransaction2;
#endif 	/* __IMSMQTransaction2_FWD_DEFINED__ */


#ifndef __IMSMQTransaction3_FWD_DEFINED__
#define __IMSMQTransaction3_FWD_DEFINED__
typedef interface IMSMQTransaction3 IMSMQTransaction3;
#endif 	/* __IMSMQTransaction3_FWD_DEFINED__ */


#ifndef __MSMQTransaction_FWD_DEFINED__
#define __MSMQTransaction_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQTransaction MSMQTransaction;
#else
typedef struct MSMQTransaction MSMQTransaction;
#endif /* __cplusplus */

#endif 	/* __MSMQTransaction_FWD_DEFINED__ */


#ifndef __IMSMQCoordinatedTransactionDispenser2_FWD_DEFINED__
#define __IMSMQCoordinatedTransactionDispenser2_FWD_DEFINED__
typedef interface IMSMQCoordinatedTransactionDispenser2 IMSMQCoordinatedTransactionDispenser2;
#endif 	/* __IMSMQCoordinatedTransactionDispenser2_FWD_DEFINED__ */


#ifndef __IMSMQCoordinatedTransactionDispenser3_FWD_DEFINED__
#define __IMSMQCoordinatedTransactionDispenser3_FWD_DEFINED__
typedef interface IMSMQCoordinatedTransactionDispenser3 IMSMQCoordinatedTransactionDispenser3;
#endif 	/* __IMSMQCoordinatedTransactionDispenser3_FWD_DEFINED__ */


#ifndef __MSMQCoordinatedTransactionDispenser_FWD_DEFINED__
#define __MSMQCoordinatedTransactionDispenser_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQCoordinatedTransactionDispenser MSMQCoordinatedTransactionDispenser;
#else
typedef struct MSMQCoordinatedTransactionDispenser MSMQCoordinatedTransactionDispenser;
#endif /* __cplusplus */

#endif 	/* __MSMQCoordinatedTransactionDispenser_FWD_DEFINED__ */


#ifndef __IMSMQTransactionDispenser2_FWD_DEFINED__
#define __IMSMQTransactionDispenser2_FWD_DEFINED__
typedef interface IMSMQTransactionDispenser2 IMSMQTransactionDispenser2;
#endif 	/* __IMSMQTransactionDispenser2_FWD_DEFINED__ */


#ifndef __IMSMQTransactionDispenser3_FWD_DEFINED__
#define __IMSMQTransactionDispenser3_FWD_DEFINED__
typedef interface IMSMQTransactionDispenser3 IMSMQTransactionDispenser3;
#endif 	/* __IMSMQTransactionDispenser3_FWD_DEFINED__ */


#ifndef __MSMQTransactionDispenser_FWD_DEFINED__
#define __MSMQTransactionDispenser_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQTransactionDispenser MSMQTransactionDispenser;
#else
typedef struct MSMQTransactionDispenser MSMQTransactionDispenser;
#endif /* __cplusplus */

#endif 	/* __MSMQTransactionDispenser_FWD_DEFINED__ */


#ifndef __IMSMQApplication_FWD_DEFINED__
#define __IMSMQApplication_FWD_DEFINED__
typedef interface IMSMQApplication IMSMQApplication;
#endif 	/* __IMSMQApplication_FWD_DEFINED__ */


#ifndef __IMSMQApplication2_FWD_DEFINED__
#define __IMSMQApplication2_FWD_DEFINED__
typedef interface IMSMQApplication2 IMSMQApplication2;
#endif 	/* __IMSMQApplication2_FWD_DEFINED__ */


#ifndef __IMSMQApplication3_FWD_DEFINED__
#define __IMSMQApplication3_FWD_DEFINED__
typedef interface IMSMQApplication3 IMSMQApplication3;
#endif 	/* __IMSMQApplication3_FWD_DEFINED__ */


#ifndef __MSMQApplication_FWD_DEFINED__
#define __MSMQApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQApplication MSMQApplication;
#else
typedef struct MSMQApplication MSMQApplication;
#endif /* __cplusplus */

#endif 	/* __MSMQApplication_FWD_DEFINED__ */


#ifndef __IMSMQDestination_FWD_DEFINED__
#define __IMSMQDestination_FWD_DEFINED__
typedef interface IMSMQDestination IMSMQDestination;
#endif 	/* __IMSMQDestination_FWD_DEFINED__ */


#ifndef __IMSMQPrivateDestination_FWD_DEFINED__
#define __IMSMQPrivateDestination_FWD_DEFINED__
typedef interface IMSMQPrivateDestination IMSMQPrivateDestination;
#endif 	/* __IMSMQPrivateDestination_FWD_DEFINED__ */


#ifndef __MSMQDestination_FWD_DEFINED__
#define __MSMQDestination_FWD_DEFINED__

#ifdef __cplusplus
typedef class MSMQDestination MSMQDestination;
#else
typedef struct MSMQDestination MSMQDestination;
#endif /* __cplusplus */

#endif 	/* __MSMQDestination_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __MSMQ_LIBRARY_DEFINED__
#define __MSMQ_LIBRARY_DEFINED__

/* library MSMQ */
/* [version][lcid][helpstringdll][helpstring][uuid] */ 

#ifndef MIDL_INTERFACE
#if _MSC_VER >= 1100
#define MIDL_INTERFACE(x)   struct __declspec(uuid(x)) __declspec(novtable)
#else
#define MIDL_INTERFACE(x)   struct
#endif //_MSC_VER
#endif //MIDL_INTERFACE

typedef short Boolean;

typedef unsigned char BYTE;

typedef unsigned long ULONG;

typedef unsigned long DWORD;

typedef int BOOL;

















typedef /* [helpstringcontext] */ 
enum _MQCALG
    {	MQMSG_CALG_MD2	= 0x8000 + 0 + 1,
	MQMSG_CALG_MD4	= 0x8000 + 0 + 2,
	MQMSG_CALG_MD5	= 0x8000 + 0 + 3,
	MQMSG_CALG_SHA	= 0x8000 + 0 + 4,
	MQMSG_CALG_MAC	= 0x8000 + 0 + 5,
	MQMSG_CALG_RSA_SIGN	= 0x2000 + 0x400 + 0,
	MQMSG_CALG_DSS_SIGN	= 0x2000 + 0x200 + 0,
	MQMSG_CALG_RSA_KEYX	= 0xa000 + 0x400 + 0,
	MQMSG_CALG_DES	= 0x6000 + 0x600 + 1,
	MQMSG_CALG_RC2	= 0x6000 + 0x600 + 2,
	MQMSG_CALG_RC4	= 0x6000 + 0x800 + 1,
	MQMSG_CALG_SEAL	= 0x6000 + 0x800 + 2
    }	MQCALG;

typedef /* [helpstringcontext] */ 
enum _MQTRANSACTION
    {	MQ_NO_TRANSACTION	= 0,
	MQ_MTS_TRANSACTION	= 1,
	MQ_XA_TRANSACTION	= 2,
	MQ_SINGLE_MESSAGE	= 3
    }	MQTRANSACTION;

typedef /* [helpstringcontext] */ 
enum _RELOPS
    {	REL_NOP	= 0,
	REL_EQ	= REL_NOP + 1,
	REL_NEQ	= REL_EQ + 1,
	REL_LT	= REL_NEQ + 1,
	REL_GT	= REL_LT + 1,
	REL_LE	= REL_GT + 1,
	REL_GE	= REL_LE + 1
    }	RELOPS;

typedef /* [helpstringcontext] */ 
enum _MQCERT_REGISTER
    {	MQCERT_REGISTER_ALWAYS	= 1,
	MQCERT_REGISTER_IF_NOT_EXIST	= 2
    }	MQCERT_REGISTER;

typedef /* [helpstringcontext] */ 
enum _MQMSGCURSOR
    {	MQMSG_FIRST	= 0,
	MQMSG_CURRENT	= 1,
	MQMSG_NEXT	= 2
    }	MQMSGCURSOR;

typedef /* [helpstringcontext] */ 
enum _MQMSGCLASS
    {	MQMSG_CLASS_NORMAL	= 0 + 0 + 0,
	MQMSG_CLASS_REPORT	= 0 + 0 + 0x1,
	MQMSG_CLASS_ACK_REACH_QUEUE	= 0 + 0 + 0x2,
	MQMSG_CLASS_ACK_RECEIVE	= 0 + 0x4000 + 0,
	MQMSG_CLASS_NACK_BAD_DST_Q	= 0x8000 + 0 + 0,
	MQMSG_CLASS_NACK_PURGED	= 0x8000 + 0 + 0x1,
	MQMSG_CLASS_NACK_REACH_QUEUE_TIMEOUT	= 0x8000 + 0 + 0x2,
	MQMSG_CLASS_NACK_Q_EXCEED_QUOTA	= 0x8000 + 0 + 0x3,
	MQMSG_CLASS_NACK_ACCESS_DENIED	= 0x8000 + 0 + 0x4,
	MQMSG_CLASS_NACK_HOP_COUNT_EXCEEDED	= 0x8000 + 0 + 0x5,
	MQMSG_CLASS_NACK_BAD_SIGNATURE	= 0x8000 + 0 + 0x6,
	MQMSG_CLASS_NACK_BAD_ENCRYPTION	= 0x8000 + 0 + 0x7,
	MQMSG_CLASS_NACK_COULD_NOT_ENCRYPT	= 0x8000 + 0 + 0x8,
	MQMSG_CLASS_NACK_NOT_TRANSACTIONAL_Q	= 0x8000 + 0 + 0x9,
	MQMSG_CLASS_NACK_NOT_TRANSACTIONAL_MSG	= 0x8000 + 0 + 0xa,
	MQMSG_CLASS_NACK_UNSUPPORTED_CRYPTO_PROVIDER	= 0x8000 + 0 + 0xb,
	MQMSG_CLASS_NACK_Q_DELETED	= 0x8000 + 0x4000 + 0,
	MQMSG_CLASS_NACK_Q_PURGED	= 0x8000 + 0x4000 + 0x1,
	MQMSG_CLASS_NACK_RECEIVE_TIMEOUT	= 0x8000 + 0x4000 + 0x2,
	MQMSG_CLASS_NACK_RECEIVE_TIMEOUT_AT_SENDER	= 0x8000 + 0x4000 + 0x3
    }	MQMSGCLASS;

typedef /* [helpstringcontext] */ 
enum _MQMSGDELIVERY
    {	MQMSG_DELIVERY_EXPRESS	= 0,
	MQMSG_DELIVERY_RECOVERABLE	= 1
    }	MQMSGDELIVERY;

typedef /* [helpstringcontext] */ 
enum _MQMSGACKNOWLEDGEMENT
    {	MQMSG_ACKNOWLEDGMENT_NONE	= 0,
	MQMSG_ACKNOWLEDGMENT_POS_ARRIVAL	= 0x1,
	MQMSG_ACKNOWLEDGMENT_POS_RECEIVE	= 0x2,
	MQMSG_ACKNOWLEDGMENT_NEG_ARRIVAL	= 0x4,
	MQMSG_ACKNOWLEDGMENT_NEG_RECEIVE	= 0x8,
	MQMSG_ACKNOWLEDGMENT_NACK_REACH_QUEUE	= 0x4,
	MQMSG_ACKNOWLEDGMENT_FULL_REACH_QUEUE	= 0x4 + 0x1,
	MQMSG_ACKNOWLEDGMENT_NACK_RECEIVE	= 0x4 + 0x8,
	MQMSG_ACKNOWLEDGMENT_FULL_RECEIVE	= 0x4 + 0x8 + 0x2
    }	MQMSGACKNOWLEDGEMENT;

typedef /* [helpstringcontext] */ 
enum _MQMSGJOURNAL
    {	MQMSG_JOURNAL_NONE	= 0,
	MQMSG_DEADLETTER	= 1,
	MQMSG_JOURNAL	= 2
    }	MQMSGJOURNAL;

typedef /* [helpstringcontext] */ 
enum _MQMSGTRACE
    {	MQMSG_TRACE_NONE	= 0,
	MQMSG_SEND_ROUTE_TO_REPORT_QUEUE	= 1
    }	MQMSGTRACE;

typedef /* [helpstringcontext] */ 
enum _MQMSGSENDERIDTYPE
    {	MQMSG_SENDERID_TYPE_NONE	= 0,
	MQMSG_SENDERID_TYPE_SID	= 1
    }	MQMSGSENDERIDTYPE;

typedef /* [helpstringcontext] */ 
enum _MQMSGPRIVLEVEL
    {	MQMSG_PRIV_LEVEL_NONE	= 0,
	MQMSG_PRIV_LEVEL_BODY	= 1,
	MQMSG_PRIV_LEVEL_BODY_BASE	= 1,
	MQMSG_PRIV_LEVEL_BODY_ENHANCED	= 3
    }	MQMSGPRIVLEVEL;

typedef /* [helpstringcontext] */ 
enum _MQMSGAUTHLEVEL
    {	MQMSG_AUTH_LEVEL_NONE	= 0,
	MQMSG_AUTH_LEVEL_ALWAYS	= 1,
	MQMSG_AUTH_LEVEL_MSMQ10	= 2,
	MQMSG_AUTH_LEVEL_MSMQ20	= 4
    }	MQMSGAUTHLEVEL;

typedef /* [helpstringcontext] */ 
enum _MQMSGIDSIZE
    {	MQMSG_MSGID_SIZE	= 20,
	MQMSG_CORRELATIONID_SIZE	= 20,
	MQMSG_XACTID_SIZE	= 20
    }	MQMSGIDSIZE;

typedef /* [helpstringcontext] */ 
enum _MQMSGMAX
    {	MQ_MAX_MSG_LABEL_LEN	= 249
    }	MQMSGMAX;

typedef /* [helpstringcontext] */ 
enum _MQMSGAUTHENTICATION
    {	MQMSG_AUTHENTICATION_NOT_REQUESTED	= 0,
	MQMSG_AUTHENTICATION_REQUESTED	= 1,
	MQMSG_AUTHENTICATION_REQUESTED_EX	= 3
    }	MQMSGAUTHENTICATION;

typedef /* [helpstringcontext] */ 
enum _MQSHARE
    {	MQ_DENY_NONE	= 0,
	MQ_DENY_RECEIVE_SHARE	= 1
    }	MQSHARE;

typedef /* [helpstringcontext] */ 
enum _MQACCESS
    {	MQ_RECEIVE_ACCESS	= 1,
	MQ_SEND_ACCESS	= 2,
	MQ_PEEK_ACCESS	= 0x20
    }	MQACCESS;

typedef /* [helpstringcontext] */ 
enum _MQJOURNAL
    {	MQ_JOURNAL_NONE	= 0,
	MQ_JOURNAL	= 1
    }	MQJOURNAL;

typedef /* [helpstringcontext] */ 
enum _MQTRANSACTIONAL
    {	MQ_TRANSACTIONAL_NONE	= 0,
	MQ_TRANSACTIONAL	= 1
    }	MQTRANSACTIONAL;

typedef /* [helpstringcontext] */ 
enum _MQAUTHENTICATE
    {	MQ_AUTHENTICATE_NONE	= 0,
	MQ_AUTHENTICATE	= 1
    }	MQAUTHENTICATE;

typedef /* [helpstringcontext] */ 
enum _MQPRIVLEVEL
    {	MQ_PRIV_LEVEL_NONE	= 0,
	MQ_PRIV_LEVEL_OPTIONAL	= 1,
	MQ_PRIV_LEVEL_BODY	= 2
    }	MQPRIVLEVEL;

typedef /* [helpstringcontext] */ 
enum _MQPRIORITY
    {	MQ_MIN_PRIORITY	= 0,
	MQ_MAX_PRIORITY	= 7
    }	MQPRIORITY;

typedef /* [helpstringcontext] */ 
enum _MQMAX
    {	MQ_MAX_Q_NAME_LEN	= 124,
	MQ_MAX_Q_LABEL_LEN	= 124
    }	MQMAX;

typedef /* [helpstringcontext] */ 
enum _MQDEFAULT
    {	DEFAULT_M_PRIORITY	= 3,
	DEFAULT_M_DELIVERY	= 0,
	DEFAULT_M_ACKNOWLEDGE	= 0,
	DEFAULT_M_JOURNAL	= 0,
	DEFAULT_M_APPSPECIFIC	= 0,
	DEFAULT_M_PRIV_LEVEL	= 0,
	DEFAULT_M_AUTH_LEVEL	= 0,
	DEFAULT_M_SENDERID_TYPE	= 1,
	DEFAULT_Q_JOURNAL	= 0,
	DEFAULT_Q_BASEPRIORITY	= 0,
	DEFAULT_Q_QUOTA	= 0xffffffff,
	DEFAULT_Q_JOURNAL_QUOTA	= 0xffffffff,
	DEFAULT_Q_TRANSACTION	= 0,
	DEFAULT_Q_AUTHENTICATE	= 0,
	DEFAULT_Q_PRIV_LEVEL	= 1,
	DEFAULT_M_LOOKUPID	= 0
    }	MQDEFAULT;

typedef /* [helpstringcontext] */ 
enum _MQERROR
    {	MQ_ERROR	= 0xc00e0001,
	MQ_ERROR_PROPERTY	= 0xc00e0002,
	MQ_ERROR_QUEUE_NOT_FOUND	= 0xc00e0003,
	MQ_ERROR_QUEUE_EXISTS	= 0xc00e0005,
	MQ_ERROR_INVALID_PARAMETER	= 0xc00e0006,
	MQ_ERROR_INVALID_HANDLE	= 0xc00e0007,
	MQ_ERROR_OPERATION_CANCELLED	= 0xc00e0008,
	MQ_ERROR_SHARING_VIOLATION	= 0xc00e0009,
	MQ_ERROR_SERVICE_NOT_AVAILABLE	= 0xc00e000b,
	MQ_ERROR_MACHINE_NOT_FOUND	= 0xc00e000d,
	MQ_ERROR_ILLEGAL_SORT	= 0xc00e0010,
	MQ_ERROR_ILLEGAL_USER	= 0xc00e0011,
	MQ_ERROR_NO_DS	= 0xc00e0013,
	MQ_ERROR_ILLEGAL_QUEUE_PATHNAME	= 0xc00e0014,
	MQ_ERROR_ILLEGAL_PROPERTY_VALUE	= 0xc00e0018,
	MQ_ERROR_ILLEGAL_PROPERTY_VT	= 0xc00e0019,
	MQ_ERROR_BUFFER_OVERFLOW	= 0xc00e001a,
	MQ_ERROR_IO_TIMEOUT	= 0xc00e001b,
	MQ_ERROR_ILLEGAL_CURSOR_ACTION	= 0xc00e001c,
	MQ_ERROR_MESSAGE_ALREADY_RECEIVED	= 0xc00e001d,
	MQ_ERROR_ILLEGAL_FORMATNAME	= 0xc00e001e,
	MQ_ERROR_FORMATNAME_BUFFER_TOO_SMALL	= 0xc00e001f,
	MQ_ERROR_UNSUPPORTED_FORMATNAME_OPERATION	= 0xc00e0020,
	MQ_ERROR_ILLEGAL_SECURITY_DESCRIPTOR	= 0xc00e0021,
	MQ_ERROR_SENDERID_BUFFER_TOO_SMALL	= 0xc00e0022,
	MQ_ERROR_SECURITY_DESCRIPTOR_TOO_SMALL	= 0xc00e0023,
	MQ_ERROR_CANNOT_IMPERSONATE_CLIENT	= 0xc00e0024,
	MQ_ERROR_ACCESS_DENIED	= 0xc00e0025,
	MQ_ERROR_PRIVILEGE_NOT_HELD	= 0xc00e0026,
	MQ_ERROR_INSUFFICIENT_RESOURCES	= 0xc00e0027,
	MQ_ERROR_USER_BUFFER_TOO_SMALL	= 0xc00e0028,
	MQ_ERROR_MESSAGE_STORAGE_FAILED	= 0xc00e002a,
	MQ_ERROR_SENDER_CERT_BUFFER_TOO_SMALL	= 0xc00e002b,
	MQ_ERROR_INVALID_CERTIFICATE	= 0xc00e002c,
	MQ_ERROR_CORRUPTED_INTERNAL_CERTIFICATE	= 0xc00e002d,
	MQ_ERROR_INTERNAL_USER_CERT_EXIST	= 0xc00e002e,
	MQ_ERROR_NO_INTERNAL_USER_CERT	= 0xc00e002f,
	MQ_ERROR_CORRUPTED_SECURITY_DATA	= 0xc00e0030,
	MQ_ERROR_CORRUPTED_PERSONAL_CERT_STORE	= 0xc00e0031,
	MQ_ERROR_COMPUTER_DOES_NOT_SUPPORT_ENCRYPTION	= 0xc00e0033,
	MQ_ERROR_BAD_SECURITY_CONTEXT	= 0xc00e0035,
	MQ_ERROR_COULD_NOT_GET_USER_SID	= 0xc00e0036,
	MQ_ERROR_COULD_NOT_GET_ACCOUNT_INFO	= 0xc00e0037,
	MQ_ERROR_ILLEGAL_MQCOLUMNS	= 0xc00e0038,
	MQ_ERROR_ILLEGAL_PROPID	= 0xc00e0039,
	MQ_ERROR_ILLEGAL_RELATION	= 0xc00e003a,
	MQ_ERROR_ILLEGAL_PROPERTY_SIZE	= 0xc00e003b,
	MQ_ERROR_ILLEGAL_RESTRICTION_PROPID	= 0xc00e003c,
	MQ_ERROR_ILLEGAL_MQQUEUEPROPS	= 0xc00e003d,
	MQ_ERROR_PROPERTY_NOTALLOWED	= 0xc00e003e,
	MQ_ERROR_INSUFFICIENT_PROPERTIES	= 0xc00e003f,
	MQ_ERROR_MACHINE_EXISTS	= 0xc00e0040,
	MQ_ERROR_ILLEGAL_MQQMPROPS	= 0xc00e0041,
	MQ_ERROR_DS_IS_FULL	= 0xc00e0042,
	MQ_ERROR_DS_ERROR	= 0xc00e0043,
	MQ_ERROR_INVALID_OWNER	= 0xc00e0044,
	MQ_ERROR_UNSUPPORTED_ACCESS_MODE	= 0xc00e0045,
	MQ_ERROR_RESULT_BUFFER_TOO_SMALL	= 0xc00e0046,
	MQ_ERROR_DELETE_CN_IN_USE	= 0xc00e0048,
	MQ_ERROR_NO_RESPONSE_FROM_OBJECT_SERVER	= 0xc00e0049,
	MQ_ERROR_OBJECT_SERVER_NOT_AVAILABLE	= 0xc00e004a,
	MQ_ERROR_QUEUE_NOT_AVAILABLE	= 0xc00e004b,
	MQ_ERROR_DTC_CONNECT	= 0xc00e004c,
	MQ_ERROR_TRANSACTION_IMPORT	= 0xc00e004e,
	MQ_ERROR_TRANSACTION_USAGE	= 0xc00e0050,
	MQ_ERROR_TRANSACTION_SEQUENCE	= 0xc00e0051,
	MQ_ERROR_MISSING_CONNECTOR_TYPE	= 0xc00e0055,
	MQ_ERROR_STALE_HANDLE	= 0xc00e0056,
	MQ_ERROR_TRANSACTION_ENLIST	= 0xc00e0058,
	MQ_ERROR_QUEUE_DELETED	= 0xc00e005a,
	MQ_ERROR_ILLEGAL_CONTEXT	= 0xc00e005b,
	MQ_ERROR_ILLEGAL_SORT_PROPID	= 0xc00e005c,
	MQ_ERROR_LABEL_TOO_LONG	= 0xc00e005d,
	MQ_ERROR_LABEL_BUFFER_TOO_SMALL	= 0xc00e005e,
	MQ_ERROR_MQIS_SERVER_EMPTY	= 0xc00e005f,
	MQ_ERROR_MQIS_READONLY_MODE	= 0xc00e0060,
	MQ_ERROR_SYMM_KEY_BUFFER_TOO_SMALL	= 0xc00e0061,
	MQ_ERROR_SIGNATURE_BUFFER_TOO_SMALL	= 0xc00e0062,
	MQ_ERROR_PROV_NAME_BUFFER_TOO_SMALL	= 0xc00e0063,
	MQ_ERROR_ILLEGAL_OPERATION	= 0xc00e0064,
	MQ_ERROR_WRITE_NOT_ALLOWED	= 0xc00e0065,
	MQ_ERROR_WKS_CANT_SERVE_CLIENT	= 0xc00e0066L,
	MQ_ERROR_DEPEND_WKS_LICENSE_OVERFLOW	= 0xc00e0067L,
	MQ_CORRUPTED_QUEUE_WAS_DELETED	= 0xc00e0068L,
	MQ_ERROR_REMOTE_MACHINE_NOT_AVAILABLE	= 0xc00e0069L,
	MQ_ERROR_UNSUPPORTED_OPERATION	= 0xc00e006aL,
	MQ_ERROR_ENCRYPTION_PROVIDER_NOT_SUPPORTED	= 0xc00e006bL,
	MQ_ERROR_CANNOT_SET_CRYPTO_SEC_DESCR	= 0xc00e006cL,
	MQ_ERROR_CERTIFICATE_NOT_PROVIDED	= 0xc00e006dL,
	MQ_ERROR_Q_DNS_PROPERTY_NOT_SUPPORTED	= 0xc00e006eL,
	MQ_ERROR_CANT_CREATE_CERT_STORE	= 0xc00e006fL,
	MQ_ERROR_CANNOT_CREATE_CERT_STORE	= 0xc00e006fL,
	MQ_ERROR_CANT_OPEN_CERT_STORE	= 0xc00e0070L,
	MQ_ERROR_CANNOT_OPEN_CERT_STORE	= 0xc00e0070L,
	MQ_ERROR_ILLEGAL_ENTERPRISE_OPERATION	= 0xc00e0071L,
	MQ_ERROR_CANNOT_GRANT_ADD_GUID	= 0xc00e0072L,
	MQ_ERROR_CANNOT_LOAD_MSMQOCM	= 0xc00e0073L,
	MQ_ERROR_NO_ENTRY_POINT_MSMQOCM	= 0xc00e0074L,
	MQ_ERROR_NO_MSMQ_SERVERS_ON_DC	= 0xc00e0075L,
	MQ_ERROR_CANNOT_JOIN_DOMAIN	= 0xc00e0076L,
	MQ_ERROR_CANNOT_CREATE_ON_GC	= 0xc00e0077L,
	MQ_ERROR_GUID_NOT_MATCHING	= 0xc00e0078L,
	MQ_ERROR_PUBLIC_KEY_NOT_FOUND	= 0xc00e0079L,
	MQ_ERROR_PUBLIC_KEY_DOES_NOT_EXIST	= 0xc00e007aL,
	MQ_ERROR_ILLEGAL_MQPRIVATEPROPS	= 0xc00e007bL,
	MQ_ERROR_NO_GC_IN_DOMAIN	= 0xc00e007cL,
	MQ_ERROR_NO_MSMQ_SERVERS_ON_GC	= 0xc00e007dL,
	MQ_ERROR_CANNOT_GET_DN	= 0xc00e007eL,
	MQ_ERROR_CANNOT_HASH_DATA_EX	= 0xc00e007fL,
	MQ_ERROR_CANNOT_SIGN_DATA_EX	= 0xc00e0080L,
	MQ_ERROR_CANNOT_CREATE_HASH_EX	= 0xc00e0081L,
	MQ_ERROR_FAIL_VERIFY_SIGNATURE_EX	= 0xc00e0082L
    }	MQERROR;

typedef /* [helpstringcontext] */ 
enum _MQWARNING
    {	MQ_INFORMATION_PROPERTY	= 0x400e0001,
	MQ_INFORMATION_ILLEGAL_PROPERTY	= 0x400e0002,
	MQ_INFORMATION_PROPERTY_IGNORED	= 0x400e0003,
	MQ_INFORMATION_UNSUPPORTED_PROPERTY	= 0x400e0004,
	MQ_INFORMATION_DUPLICATE_PROPERTY	= 0x400e0005,
	MQ_INFORMATION_OPERATION_PENDING	= 0x400e0006,
	MQ_INFORMATION_FORMATNAME_BUFFER_TOO_SMALL	= 0x400e0009,
	MQ_INFORMATION_INTERNAL_USER_CERT_EXIST	= 0x400e000aL,
	MQ_INFORMATION_OWNER_IGNORED	= 0x400e000bL
    }	MQWARNING;

typedef struct _MQBOID
    {
    BYTE rgb[ 16 ];
    }	MQBOID;

typedef MQBOID MQXACTUOW;

typedef LONG MQISOLEVEL;

typedef struct _MQXACTTRANSINFO
    {
    MQXACTUOW uow;
    MQISOLEVEL isoLevel;
    ULONG isoFlags;
    DWORD grfTCSupported;
    DWORD grfRMSupported;
    DWORD grfTCSupportedRetaining;
    DWORD grfRMSupportedRetaining;
    }	MQXACTTRANSINFO;


DEFINE_GUID(LIBID_MSMQ,0xD7D6E071,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifndef __IMSMQQuery_INTERFACE_DEFINED__
#define __IMSMQQuery_INTERFACE_DEFINED__

/* interface IMSMQQuery */
/* [object][nonextensible][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQuery,0xD7D6E072,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E072-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQQuery : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE LookupQueue( 
            /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *Label,
            /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
            /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
            /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
            /* [retval][out] */ IMSMQQueueInfos __RPC_FAR *__RPC_FAR *ppqinfos) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQuery __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQuery __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQuery __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQuery __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQuery __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQuery __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQuery __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LookupQueue )( 
            IMSMQQuery __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *Label,
            /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
            /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
            /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
            /* [retval][out] */ IMSMQQueueInfos __RPC_FAR *__RPC_FAR *ppqinfos);
        
        END_INTERFACE
    } IMSMQQueryVtbl;

    interface IMSMQQuery
    {
        CONST_VTBL struct IMSMQQueryVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQuery_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQuery_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQuery_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQuery_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQuery_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQuery_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQuery_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQuery_LookupQueue(This,QueueGuid,ServiceTypeGuid,Label,CreateTime,ModifyTime,RelServiceType,RelLabel,RelCreateTime,RelModifyTime,ppqinfos)	\
    (This)->lpVtbl -> LookupQueue(This,QueueGuid,ServiceTypeGuid,Label,CreateTime,ModifyTime,RelServiceType,RelLabel,RelCreateTime,RelModifyTime,ppqinfos)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQuery_LookupQueue_Proxy( 
    IMSMQQuery __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
    /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
    /* [optional][in] */ VARIANT __RPC_FAR *Label,
    /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
    /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
    /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
    /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
    /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
    /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
    /* [retval][out] */ IMSMQQueueInfos __RPC_FAR *__RPC_FAR *ppqinfos);


void __RPC_STUB IMSMQQuery_LookupQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQuery_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueueInfo_INTERFACE_DEFINED__
#define __IMSMQQueueInfo_INTERFACE_DEFINED__

/* interface IMSMQQueueInfo */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueueInfo,0xD7D6E07B,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E07B-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQQueueInfo : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueueGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ServiceTypeGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_ServiceTypeGuid( 
            /* [in] */ BSTR bstrGuidServiceType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR bstrLabel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PathName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PathName( 
            /* [in] */ BSTR bstrPathName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_FormatName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_FormatName( 
            /* [in] */ BSTR bstrFormatName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsTransactional( 
            /* [retval][out] */ Boolean __RPC_FAR *pisTransactional) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PrivLevel( 
            /* [retval][out] */ long __RPC_FAR *plPrivLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PrivLevel( 
            /* [in] */ long lPrivLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Journal( 
            /* [retval][out] */ long __RPC_FAR *plJournal) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Journal( 
            /* [in] */ long lJournal) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Quota( 
            /* [retval][out] */ long __RPC_FAR *plQuota) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Quota( 
            /* [in] */ long lQuota) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_BasePriority( 
            /* [retval][out] */ long __RPC_FAR *plBasePriority) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_BasePriority( 
            /* [in] */ long lBasePriority) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_CreateTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ModifyTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Authenticate( 
            /* [retval][out] */ long __RPC_FAR *plAuthenticate) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Authenticate( 
            /* [in] */ long lAuthenticate) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_JournalQuota( 
            /* [retval][out] */ long __RPC_FAR *plJournalQuota) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_JournalQuota( 
            /* [in] */ long lJournalQuota) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsWorldReadable( 
            /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
            /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ long Access,
            /* [in] */ long ShareMode,
            /* [retval][out] */ IMSMQQueue __RPC_FAR *__RPC_FAR *ppq) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueueInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueueInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueueGuid )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ServiceTypeGuid )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ServiceTypeGuid )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ BSTR bstrGuidServiceType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Label )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ BSTR bstrLabel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PathName )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PathName )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ BSTR bstrPathName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FormatName )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FormatName )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ BSTR bstrFormatName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsTransactional )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisTransactional);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrivLevel )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPrivLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrivLevel )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long lPrivLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Journal )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournal);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Journal )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long lJournal);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Quota )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plQuota);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Quota )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long lQuota);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BasePriority )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBasePriority);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BasePriority )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long lBasePriority);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreateTime )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModifyTime )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Authenticate )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthenticate);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Authenticate )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long lAuthenticate);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_JournalQuota )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournalQuota);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_JournalQuota )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long lJournalQuota);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsWorldReadable )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
            /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IMSMQQueueInfo __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            IMSMQQueueInfo __RPC_FAR * This,
            /* [in] */ long Access,
            /* [in] */ long ShareMode,
            /* [retval][out] */ IMSMQQueue __RPC_FAR *__RPC_FAR *ppq);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IMSMQQueueInfo __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            IMSMQQueueInfo __RPC_FAR * This);
        
        END_INTERFACE
    } IMSMQQueueInfoVtbl;

    interface IMSMQQueueInfo
    {
        CONST_VTBL struct IMSMQQueueInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueueInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueueInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueueInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueueInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueueInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueueInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueueInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueueInfo_get_QueueGuid(This,pbstrGuidQueue)	\
    (This)->lpVtbl -> get_QueueGuid(This,pbstrGuidQueue)

#define IMSMQQueueInfo_get_ServiceTypeGuid(This,pbstrGuidServiceType)	\
    (This)->lpVtbl -> get_ServiceTypeGuid(This,pbstrGuidServiceType)

#define IMSMQQueueInfo_put_ServiceTypeGuid(This,bstrGuidServiceType)	\
    (This)->lpVtbl -> put_ServiceTypeGuid(This,bstrGuidServiceType)

#define IMSMQQueueInfo_get_Label(This,pbstrLabel)	\
    (This)->lpVtbl -> get_Label(This,pbstrLabel)

#define IMSMQQueueInfo_put_Label(This,bstrLabel)	\
    (This)->lpVtbl -> put_Label(This,bstrLabel)

#define IMSMQQueueInfo_get_PathName(This,pbstrPathName)	\
    (This)->lpVtbl -> get_PathName(This,pbstrPathName)

#define IMSMQQueueInfo_put_PathName(This,bstrPathName)	\
    (This)->lpVtbl -> put_PathName(This,bstrPathName)

#define IMSMQQueueInfo_get_FormatName(This,pbstrFormatName)	\
    (This)->lpVtbl -> get_FormatName(This,pbstrFormatName)

#define IMSMQQueueInfo_put_FormatName(This,bstrFormatName)	\
    (This)->lpVtbl -> put_FormatName(This,bstrFormatName)

#define IMSMQQueueInfo_get_IsTransactional(This,pisTransactional)	\
    (This)->lpVtbl -> get_IsTransactional(This,pisTransactional)

#define IMSMQQueueInfo_get_PrivLevel(This,plPrivLevel)	\
    (This)->lpVtbl -> get_PrivLevel(This,plPrivLevel)

#define IMSMQQueueInfo_put_PrivLevel(This,lPrivLevel)	\
    (This)->lpVtbl -> put_PrivLevel(This,lPrivLevel)

#define IMSMQQueueInfo_get_Journal(This,plJournal)	\
    (This)->lpVtbl -> get_Journal(This,plJournal)

#define IMSMQQueueInfo_put_Journal(This,lJournal)	\
    (This)->lpVtbl -> put_Journal(This,lJournal)

#define IMSMQQueueInfo_get_Quota(This,plQuota)	\
    (This)->lpVtbl -> get_Quota(This,plQuota)

#define IMSMQQueueInfo_put_Quota(This,lQuota)	\
    (This)->lpVtbl -> put_Quota(This,lQuota)

#define IMSMQQueueInfo_get_BasePriority(This,plBasePriority)	\
    (This)->lpVtbl -> get_BasePriority(This,plBasePriority)

#define IMSMQQueueInfo_put_BasePriority(This,lBasePriority)	\
    (This)->lpVtbl -> put_BasePriority(This,lBasePriority)

#define IMSMQQueueInfo_get_CreateTime(This,pvarCreateTime)	\
    (This)->lpVtbl -> get_CreateTime(This,pvarCreateTime)

#define IMSMQQueueInfo_get_ModifyTime(This,pvarModifyTime)	\
    (This)->lpVtbl -> get_ModifyTime(This,pvarModifyTime)

#define IMSMQQueueInfo_get_Authenticate(This,plAuthenticate)	\
    (This)->lpVtbl -> get_Authenticate(This,plAuthenticate)

#define IMSMQQueueInfo_put_Authenticate(This,lAuthenticate)	\
    (This)->lpVtbl -> put_Authenticate(This,lAuthenticate)

#define IMSMQQueueInfo_get_JournalQuota(This,plJournalQuota)	\
    (This)->lpVtbl -> get_JournalQuota(This,plJournalQuota)

#define IMSMQQueueInfo_put_JournalQuota(This,lJournalQuota)	\
    (This)->lpVtbl -> put_JournalQuota(This,lJournalQuota)

#define IMSMQQueueInfo_get_IsWorldReadable(This,pisWorldReadable)	\
    (This)->lpVtbl -> get_IsWorldReadable(This,pisWorldReadable)

#define IMSMQQueueInfo_Create(This,IsTransactional,IsWorldReadable)	\
    (This)->lpVtbl -> Create(This,IsTransactional,IsWorldReadable)

#define IMSMQQueueInfo_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IMSMQQueueInfo_Open(This,Access,ShareMode,ppq)	\
    (This)->lpVtbl -> Open(This,Access,ShareMode,ppq)

#define IMSMQQueueInfo_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IMSMQQueueInfo_Update(This)	\
    (This)->lpVtbl -> Update(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_QueueGuid_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue);


void __RPC_STUB IMSMQQueueInfo_get_QueueGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_ServiceTypeGuid_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType);


void __RPC_STUB IMSMQQueueInfo_get_ServiceTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_ServiceTypeGuid_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ BSTR bstrGuidServiceType);


void __RPC_STUB IMSMQQueueInfo_put_ServiceTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_Label_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);


void __RPC_STUB IMSMQQueueInfo_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_Label_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ BSTR bstrLabel);


void __RPC_STUB IMSMQQueueInfo_put_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_PathName_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName);


void __RPC_STUB IMSMQQueueInfo_get_PathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_PathName_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ BSTR bstrPathName);


void __RPC_STUB IMSMQQueueInfo_put_PathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_FormatName_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);


void __RPC_STUB IMSMQQueueInfo_get_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_FormatName_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ BSTR bstrFormatName);


void __RPC_STUB IMSMQQueueInfo_put_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_IsTransactional_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisTransactional);


void __RPC_STUB IMSMQQueueInfo_get_IsTransactional_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_PrivLevel_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPrivLevel);


void __RPC_STUB IMSMQQueueInfo_get_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_PrivLevel_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long lPrivLevel);


void __RPC_STUB IMSMQQueueInfo_put_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_Journal_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournal);


void __RPC_STUB IMSMQQueueInfo_get_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_Journal_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long lJournal);


void __RPC_STUB IMSMQQueueInfo_put_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_Quota_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plQuota);


void __RPC_STUB IMSMQQueueInfo_get_Quota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_Quota_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long lQuota);


void __RPC_STUB IMSMQQueueInfo_put_Quota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_BasePriority_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plBasePriority);


void __RPC_STUB IMSMQQueueInfo_get_BasePriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_BasePriority_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long lBasePriority);


void __RPC_STUB IMSMQQueueInfo_put_BasePriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_CreateTime_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime);


void __RPC_STUB IMSMQQueueInfo_get_CreateTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_ModifyTime_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime);


void __RPC_STUB IMSMQQueueInfo_get_ModifyTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_Authenticate_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthenticate);


void __RPC_STUB IMSMQQueueInfo_get_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_Authenticate_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long lAuthenticate);


void __RPC_STUB IMSMQQueueInfo_put_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_JournalQuota_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournalQuota);


void __RPC_STUB IMSMQQueueInfo_get_JournalQuota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_put_JournalQuota_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long lJournalQuota);


void __RPC_STUB IMSMQQueueInfo_put_JournalQuota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_get_IsWorldReadable_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable);


void __RPC_STUB IMSMQQueueInfo_get_IsWorldReadable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_Create_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
    /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable);


void __RPC_STUB IMSMQQueueInfo_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_Delete_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_Open_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This,
    /* [in] */ long Access,
    /* [in] */ long ShareMode,
    /* [retval][out] */ IMSMQQueue __RPC_FAR *__RPC_FAR *ppq);


void __RPC_STUB IMSMQQueueInfo_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_Refresh_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo_Update_Proxy( 
    IMSMQQueueInfo __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueueInfo_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueueInfo2_INTERFACE_DEFINED__
#define __IMSMQQueueInfo2_INTERFACE_DEFINED__

/* interface IMSMQQueueInfo2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueueInfo2,0xFD174A80,0x89CF,0x11D2,0xB0,0xF2,0x00,0xE0,0x2C,0x07,0x4F,0x6B);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FD174A80-89CF-11D2-B0F2-00E02C074F6B")
    IMSMQQueueInfo2 : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueueGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ServiceTypeGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_ServiceTypeGuid( 
            /* [in] */ BSTR bstrGuidServiceType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR bstrLabel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PathName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PathName( 
            /* [in] */ BSTR bstrPathName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_FormatName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_FormatName( 
            /* [in] */ BSTR bstrFormatName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsTransactional( 
            /* [retval][out] */ Boolean __RPC_FAR *pisTransactional) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PrivLevel( 
            /* [retval][out] */ long __RPC_FAR *plPrivLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PrivLevel( 
            /* [in] */ long lPrivLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Journal( 
            /* [retval][out] */ long __RPC_FAR *plJournal) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Journal( 
            /* [in] */ long lJournal) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Quota( 
            /* [retval][out] */ long __RPC_FAR *plQuota) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Quota( 
            /* [in] */ long lQuota) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_BasePriority( 
            /* [retval][out] */ long __RPC_FAR *plBasePriority) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_BasePriority( 
            /* [in] */ long lBasePriority) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_CreateTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ModifyTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Authenticate( 
            /* [retval][out] */ long __RPC_FAR *plAuthenticate) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Authenticate( 
            /* [in] */ long lAuthenticate) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_JournalQuota( 
            /* [retval][out] */ long __RPC_FAR *plJournalQuota) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_JournalQuota( 
            /* [in] */ long lJournalQuota) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsWorldReadable( 
            /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
            /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ long Access,
            /* [in] */ long ShareMode,
            /* [retval][out] */ IMSMQQueue2 __RPC_FAR *__RPC_FAR *ppq) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PathNameDNS( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathNameDNS) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Security( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSecurity) = 0;
        
        virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_Security( 
            /* [in] */ VARIANT varSecurity) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueueInfo2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueueInfo2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueueGuid )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ServiceTypeGuid )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ServiceTypeGuid )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ BSTR bstrGuidServiceType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Label )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ BSTR bstrLabel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PathName )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PathName )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ BSTR bstrPathName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FormatName )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FormatName )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ BSTR bstrFormatName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsTransactional )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisTransactional);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrivLevel )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPrivLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrivLevel )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long lPrivLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Journal )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournal);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Journal )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long lJournal);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Quota )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plQuota);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Quota )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long lQuota);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BasePriority )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBasePriority);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BasePriority )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long lBasePriority);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreateTime )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModifyTime )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Authenticate )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthenticate);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Authenticate )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long lAuthenticate);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_JournalQuota )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournalQuota);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_JournalQuota )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long lJournalQuota);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsWorldReadable )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
            /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IMSMQQueueInfo2 __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ long Access,
            /* [in] */ long ShareMode,
            /* [retval][out] */ IMSMQQueue2 __RPC_FAR *__RPC_FAR *ppq);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IMSMQQueueInfo2 __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            IMSMQQueueInfo2 __RPC_FAR * This);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PathNameDNS )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathNameDNS);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Security )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSecurity);
        
        /* [id][propput][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Security )( 
            IMSMQQueueInfo2 __RPC_FAR * This,
            /* [in] */ VARIANT varSecurity);
        
        END_INTERFACE
    } IMSMQQueueInfo2Vtbl;

    interface IMSMQQueueInfo2
    {
        CONST_VTBL struct IMSMQQueueInfo2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueueInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueueInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueueInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueueInfo2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueueInfo2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueueInfo2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueueInfo2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueueInfo2_get_QueueGuid(This,pbstrGuidQueue)	\
    (This)->lpVtbl -> get_QueueGuid(This,pbstrGuidQueue)

#define IMSMQQueueInfo2_get_ServiceTypeGuid(This,pbstrGuidServiceType)	\
    (This)->lpVtbl -> get_ServiceTypeGuid(This,pbstrGuidServiceType)

#define IMSMQQueueInfo2_put_ServiceTypeGuid(This,bstrGuidServiceType)	\
    (This)->lpVtbl -> put_ServiceTypeGuid(This,bstrGuidServiceType)

#define IMSMQQueueInfo2_get_Label(This,pbstrLabel)	\
    (This)->lpVtbl -> get_Label(This,pbstrLabel)

#define IMSMQQueueInfo2_put_Label(This,bstrLabel)	\
    (This)->lpVtbl -> put_Label(This,bstrLabel)

#define IMSMQQueueInfo2_get_PathName(This,pbstrPathName)	\
    (This)->lpVtbl -> get_PathName(This,pbstrPathName)

#define IMSMQQueueInfo2_put_PathName(This,bstrPathName)	\
    (This)->lpVtbl -> put_PathName(This,bstrPathName)

#define IMSMQQueueInfo2_get_FormatName(This,pbstrFormatName)	\
    (This)->lpVtbl -> get_FormatName(This,pbstrFormatName)

#define IMSMQQueueInfo2_put_FormatName(This,bstrFormatName)	\
    (This)->lpVtbl -> put_FormatName(This,bstrFormatName)

#define IMSMQQueueInfo2_get_IsTransactional(This,pisTransactional)	\
    (This)->lpVtbl -> get_IsTransactional(This,pisTransactional)

#define IMSMQQueueInfo2_get_PrivLevel(This,plPrivLevel)	\
    (This)->lpVtbl -> get_PrivLevel(This,plPrivLevel)

#define IMSMQQueueInfo2_put_PrivLevel(This,lPrivLevel)	\
    (This)->lpVtbl -> put_PrivLevel(This,lPrivLevel)

#define IMSMQQueueInfo2_get_Journal(This,plJournal)	\
    (This)->lpVtbl -> get_Journal(This,plJournal)

#define IMSMQQueueInfo2_put_Journal(This,lJournal)	\
    (This)->lpVtbl -> put_Journal(This,lJournal)

#define IMSMQQueueInfo2_get_Quota(This,plQuota)	\
    (This)->lpVtbl -> get_Quota(This,plQuota)

#define IMSMQQueueInfo2_put_Quota(This,lQuota)	\
    (This)->lpVtbl -> put_Quota(This,lQuota)

#define IMSMQQueueInfo2_get_BasePriority(This,plBasePriority)	\
    (This)->lpVtbl -> get_BasePriority(This,plBasePriority)

#define IMSMQQueueInfo2_put_BasePriority(This,lBasePriority)	\
    (This)->lpVtbl -> put_BasePriority(This,lBasePriority)

#define IMSMQQueueInfo2_get_CreateTime(This,pvarCreateTime)	\
    (This)->lpVtbl -> get_CreateTime(This,pvarCreateTime)

#define IMSMQQueueInfo2_get_ModifyTime(This,pvarModifyTime)	\
    (This)->lpVtbl -> get_ModifyTime(This,pvarModifyTime)

#define IMSMQQueueInfo2_get_Authenticate(This,plAuthenticate)	\
    (This)->lpVtbl -> get_Authenticate(This,plAuthenticate)

#define IMSMQQueueInfo2_put_Authenticate(This,lAuthenticate)	\
    (This)->lpVtbl -> put_Authenticate(This,lAuthenticate)

#define IMSMQQueueInfo2_get_JournalQuota(This,plJournalQuota)	\
    (This)->lpVtbl -> get_JournalQuota(This,plJournalQuota)

#define IMSMQQueueInfo2_put_JournalQuota(This,lJournalQuota)	\
    (This)->lpVtbl -> put_JournalQuota(This,lJournalQuota)

#define IMSMQQueueInfo2_get_IsWorldReadable(This,pisWorldReadable)	\
    (This)->lpVtbl -> get_IsWorldReadable(This,pisWorldReadable)

#define IMSMQQueueInfo2_Create(This,IsTransactional,IsWorldReadable)	\
    (This)->lpVtbl -> Create(This,IsTransactional,IsWorldReadable)

#define IMSMQQueueInfo2_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IMSMQQueueInfo2_Open(This,Access,ShareMode,ppq)	\
    (This)->lpVtbl -> Open(This,Access,ShareMode,ppq)

#define IMSMQQueueInfo2_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IMSMQQueueInfo2_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IMSMQQueueInfo2_get_PathNameDNS(This,pbstrPathNameDNS)	\
    (This)->lpVtbl -> get_PathNameDNS(This,pbstrPathNameDNS)

#define IMSMQQueueInfo2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#define IMSMQQueueInfo2_get_Security(This,pvarSecurity)	\
    (This)->lpVtbl -> get_Security(This,pvarSecurity)

#define IMSMQQueueInfo2_put_Security(This,varSecurity)	\
    (This)->lpVtbl -> put_Security(This,varSecurity)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_QueueGuid_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue);


void __RPC_STUB IMSMQQueueInfo2_get_QueueGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_ServiceTypeGuid_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType);


void __RPC_STUB IMSMQQueueInfo2_get_ServiceTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_ServiceTypeGuid_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ BSTR bstrGuidServiceType);


void __RPC_STUB IMSMQQueueInfo2_put_ServiceTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_Label_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);


void __RPC_STUB IMSMQQueueInfo2_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_Label_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ BSTR bstrLabel);


void __RPC_STUB IMSMQQueueInfo2_put_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_PathName_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName);


void __RPC_STUB IMSMQQueueInfo2_get_PathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_PathName_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ BSTR bstrPathName);


void __RPC_STUB IMSMQQueueInfo2_put_PathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_FormatName_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);


void __RPC_STUB IMSMQQueueInfo2_get_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_FormatName_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ BSTR bstrFormatName);


void __RPC_STUB IMSMQQueueInfo2_put_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_IsTransactional_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisTransactional);


void __RPC_STUB IMSMQQueueInfo2_get_IsTransactional_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_PrivLevel_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPrivLevel);


void __RPC_STUB IMSMQQueueInfo2_get_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_PrivLevel_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long lPrivLevel);


void __RPC_STUB IMSMQQueueInfo2_put_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_Journal_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournal);


void __RPC_STUB IMSMQQueueInfo2_get_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_Journal_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long lJournal);


void __RPC_STUB IMSMQQueueInfo2_put_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_Quota_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plQuota);


void __RPC_STUB IMSMQQueueInfo2_get_Quota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_Quota_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long lQuota);


void __RPC_STUB IMSMQQueueInfo2_put_Quota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_BasePriority_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plBasePriority);


void __RPC_STUB IMSMQQueueInfo2_get_BasePriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_BasePriority_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long lBasePriority);


void __RPC_STUB IMSMQQueueInfo2_put_BasePriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_CreateTime_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime);


void __RPC_STUB IMSMQQueueInfo2_get_CreateTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_ModifyTime_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime);


void __RPC_STUB IMSMQQueueInfo2_get_ModifyTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_Authenticate_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthenticate);


void __RPC_STUB IMSMQQueueInfo2_get_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_Authenticate_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long lAuthenticate);


void __RPC_STUB IMSMQQueueInfo2_put_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_JournalQuota_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournalQuota);


void __RPC_STUB IMSMQQueueInfo2_get_JournalQuota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_JournalQuota_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long lJournalQuota);


void __RPC_STUB IMSMQQueueInfo2_put_JournalQuota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_IsWorldReadable_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable);


void __RPC_STUB IMSMQQueueInfo2_get_IsWorldReadable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_Create_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
    /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable);


void __RPC_STUB IMSMQQueueInfo2_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_Delete_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo2_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_Open_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ long Access,
    /* [in] */ long ShareMode,
    /* [retval][out] */ IMSMQQueue2 __RPC_FAR *__RPC_FAR *ppq);


void __RPC_STUB IMSMQQueueInfo2_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_Refresh_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo2_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_Update_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo2_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_PathNameDNS_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPathNameDNS);


void __RPC_STUB IMSMQQueueInfo2_get_PathNameDNS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_Properties_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQueueInfo2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_get_Security_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSecurity);


void __RPC_STUB IMSMQQueueInfo2_get_Security_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo2_put_Security_Proxy( 
    IMSMQQueueInfo2 __RPC_FAR * This,
    /* [in] */ VARIANT varSecurity);


void __RPC_STUB IMSMQQueueInfo2_put_Security_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueueInfo2_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueueInfo3_INTERFACE_DEFINED__
#define __IMSMQQueueInfo3_INTERFACE_DEFINED__

/* interface IMSMQQueueInfo3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueueInfo3,0xeba96b1d,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b1d-2168-11d3-898c-00e02c074f6b")
    IMSMQQueueInfo3 : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueueGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ServiceTypeGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_ServiceTypeGuid( 
            /* [in] */ BSTR bstrGuidServiceType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR bstrLabel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PathName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PathName( 
            /* [in] */ BSTR bstrPathName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_FormatName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_FormatName( 
            /* [in] */ BSTR bstrFormatName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsTransactional( 
            /* [retval][out] */ Boolean __RPC_FAR *pisTransactional) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PrivLevel( 
            /* [retval][out] */ long __RPC_FAR *plPrivLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PrivLevel( 
            /* [in] */ long lPrivLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Journal( 
            /* [retval][out] */ long __RPC_FAR *plJournal) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Journal( 
            /* [in] */ long lJournal) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Quota( 
            /* [retval][out] */ long __RPC_FAR *plQuota) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Quota( 
            /* [in] */ long lQuota) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_BasePriority( 
            /* [retval][out] */ long __RPC_FAR *plBasePriority) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_BasePriority( 
            /* [in] */ long lBasePriority) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_CreateTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ModifyTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Authenticate( 
            /* [retval][out] */ long __RPC_FAR *plAuthenticate) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Authenticate( 
            /* [in] */ long lAuthenticate) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_JournalQuota( 
            /* [retval][out] */ long __RPC_FAR *plJournalQuota) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_JournalQuota( 
            /* [in] */ long lJournalQuota) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsWorldReadable( 
            /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
            /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ long Access,
            /* [in] */ long ShareMode,
            /* [retval][out] */ IMSMQQueue3 __RPC_FAR *__RPC_FAR *ppq) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PathNameDNS( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathNameDNS) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Security( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSecurity) = 0;
        
        virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_Security( 
            /* [in] */ VARIANT varSecurity) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ADsPath( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrADsPath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueInfo3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueueInfo3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueueInfo3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueueGuid )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ServiceTypeGuid )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ServiceTypeGuid )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ BSTR bstrGuidServiceType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Label )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ BSTR bstrLabel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PathName )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PathName )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ BSTR bstrPathName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FormatName )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FormatName )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ BSTR bstrFormatName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsTransactional )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisTransactional);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrivLevel )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPrivLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrivLevel )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long lPrivLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Journal )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournal);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Journal )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long lJournal);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Quota )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plQuota);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Quota )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long lQuota);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BasePriority )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plBasePriority);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BasePriority )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long lBasePriority);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CreateTime )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModifyTime )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Authenticate )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthenticate);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Authenticate )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long lAuthenticate);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_JournalQuota )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournalQuota);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_JournalQuota )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long lJournalQuota);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsWorldReadable )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
            /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            IMSMQQueueInfo3 __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ long Access,
            /* [in] */ long ShareMode,
            /* [retval][out] */ IMSMQQueue3 __RPC_FAR *__RPC_FAR *ppq);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IMSMQQueueInfo3 __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Update )( 
            IMSMQQueueInfo3 __RPC_FAR * This);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PathNameDNS )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPathNameDNS);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Security )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSecurity);
        
        /* [id][propput][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Security )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [in] */ VARIANT varSecurity);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ADsPath )( 
            IMSMQQueueInfo3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrADsPath);
        
        END_INTERFACE
    } IMSMQQueueInfo3Vtbl;

    interface IMSMQQueueInfo3
    {
        CONST_VTBL struct IMSMQQueueInfo3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueueInfo3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueueInfo3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueueInfo3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueueInfo3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueueInfo3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueueInfo3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueueInfo3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueueInfo3_get_QueueGuid(This,pbstrGuidQueue)	\
    (This)->lpVtbl -> get_QueueGuid(This,pbstrGuidQueue)

#define IMSMQQueueInfo3_get_ServiceTypeGuid(This,pbstrGuidServiceType)	\
    (This)->lpVtbl -> get_ServiceTypeGuid(This,pbstrGuidServiceType)

#define IMSMQQueueInfo3_put_ServiceTypeGuid(This,bstrGuidServiceType)	\
    (This)->lpVtbl -> put_ServiceTypeGuid(This,bstrGuidServiceType)

#define IMSMQQueueInfo3_get_Label(This,pbstrLabel)	\
    (This)->lpVtbl -> get_Label(This,pbstrLabel)

#define IMSMQQueueInfo3_put_Label(This,bstrLabel)	\
    (This)->lpVtbl -> put_Label(This,bstrLabel)

#define IMSMQQueueInfo3_get_PathName(This,pbstrPathName)	\
    (This)->lpVtbl -> get_PathName(This,pbstrPathName)

#define IMSMQQueueInfo3_put_PathName(This,bstrPathName)	\
    (This)->lpVtbl -> put_PathName(This,bstrPathName)

#define IMSMQQueueInfo3_get_FormatName(This,pbstrFormatName)	\
    (This)->lpVtbl -> get_FormatName(This,pbstrFormatName)

#define IMSMQQueueInfo3_put_FormatName(This,bstrFormatName)	\
    (This)->lpVtbl -> put_FormatName(This,bstrFormatName)

#define IMSMQQueueInfo3_get_IsTransactional(This,pisTransactional)	\
    (This)->lpVtbl -> get_IsTransactional(This,pisTransactional)

#define IMSMQQueueInfo3_get_PrivLevel(This,plPrivLevel)	\
    (This)->lpVtbl -> get_PrivLevel(This,plPrivLevel)

#define IMSMQQueueInfo3_put_PrivLevel(This,lPrivLevel)	\
    (This)->lpVtbl -> put_PrivLevel(This,lPrivLevel)

#define IMSMQQueueInfo3_get_Journal(This,plJournal)	\
    (This)->lpVtbl -> get_Journal(This,plJournal)

#define IMSMQQueueInfo3_put_Journal(This,lJournal)	\
    (This)->lpVtbl -> put_Journal(This,lJournal)

#define IMSMQQueueInfo3_get_Quota(This,plQuota)	\
    (This)->lpVtbl -> get_Quota(This,plQuota)

#define IMSMQQueueInfo3_put_Quota(This,lQuota)	\
    (This)->lpVtbl -> put_Quota(This,lQuota)

#define IMSMQQueueInfo3_get_BasePriority(This,plBasePriority)	\
    (This)->lpVtbl -> get_BasePriority(This,plBasePriority)

#define IMSMQQueueInfo3_put_BasePriority(This,lBasePriority)	\
    (This)->lpVtbl -> put_BasePriority(This,lBasePriority)

#define IMSMQQueueInfo3_get_CreateTime(This,pvarCreateTime)	\
    (This)->lpVtbl -> get_CreateTime(This,pvarCreateTime)

#define IMSMQQueueInfo3_get_ModifyTime(This,pvarModifyTime)	\
    (This)->lpVtbl -> get_ModifyTime(This,pvarModifyTime)

#define IMSMQQueueInfo3_get_Authenticate(This,plAuthenticate)	\
    (This)->lpVtbl -> get_Authenticate(This,plAuthenticate)

#define IMSMQQueueInfo3_put_Authenticate(This,lAuthenticate)	\
    (This)->lpVtbl -> put_Authenticate(This,lAuthenticate)

#define IMSMQQueueInfo3_get_JournalQuota(This,plJournalQuota)	\
    (This)->lpVtbl -> get_JournalQuota(This,plJournalQuota)

#define IMSMQQueueInfo3_put_JournalQuota(This,lJournalQuota)	\
    (This)->lpVtbl -> put_JournalQuota(This,lJournalQuota)

#define IMSMQQueueInfo3_get_IsWorldReadable(This,pisWorldReadable)	\
    (This)->lpVtbl -> get_IsWorldReadable(This,pisWorldReadable)

#define IMSMQQueueInfo3_Create(This,IsTransactional,IsWorldReadable)	\
    (This)->lpVtbl -> Create(This,IsTransactional,IsWorldReadable)

#define IMSMQQueueInfo3_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define IMSMQQueueInfo3_Open(This,Access,ShareMode,ppq)	\
    (This)->lpVtbl -> Open(This,Access,ShareMode,ppq)

#define IMSMQQueueInfo3_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IMSMQQueueInfo3_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IMSMQQueueInfo3_get_PathNameDNS(This,pbstrPathNameDNS)	\
    (This)->lpVtbl -> get_PathNameDNS(This,pbstrPathNameDNS)

#define IMSMQQueueInfo3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#define IMSMQQueueInfo3_get_Security(This,pvarSecurity)	\
    (This)->lpVtbl -> get_Security(This,pvarSecurity)

#define IMSMQQueueInfo3_put_Security(This,varSecurity)	\
    (This)->lpVtbl -> put_Security(This,varSecurity)

#define IMSMQQueueInfo3_get_ADsPath(This,pbstrADsPath)	\
    (This)->lpVtbl -> get_ADsPath(This,pbstrADsPath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_QueueGuid_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidQueue);


void __RPC_STUB IMSMQQueueInfo3_get_QueueGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_ServiceTypeGuid_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidServiceType);


void __RPC_STUB IMSMQQueueInfo3_get_ServiceTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_ServiceTypeGuid_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ BSTR bstrGuidServiceType);


void __RPC_STUB IMSMQQueueInfo3_put_ServiceTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_Label_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);


void __RPC_STUB IMSMQQueueInfo3_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_Label_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ BSTR bstrLabel);


void __RPC_STUB IMSMQQueueInfo3_put_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_PathName_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPathName);


void __RPC_STUB IMSMQQueueInfo3_get_PathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_PathName_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ BSTR bstrPathName);


void __RPC_STUB IMSMQQueueInfo3_put_PathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_FormatName_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);


void __RPC_STUB IMSMQQueueInfo3_get_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_FormatName_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ BSTR bstrFormatName);


void __RPC_STUB IMSMQQueueInfo3_put_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_IsTransactional_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisTransactional);


void __RPC_STUB IMSMQQueueInfo3_get_IsTransactional_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_PrivLevel_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPrivLevel);


void __RPC_STUB IMSMQQueueInfo3_get_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_PrivLevel_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long lPrivLevel);


void __RPC_STUB IMSMQQueueInfo3_put_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_Journal_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournal);


void __RPC_STUB IMSMQQueueInfo3_get_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_Journal_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long lJournal);


void __RPC_STUB IMSMQQueueInfo3_put_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_Quota_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plQuota);


void __RPC_STUB IMSMQQueueInfo3_get_Quota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_Quota_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long lQuota);


void __RPC_STUB IMSMQQueueInfo3_put_Quota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_BasePriority_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plBasePriority);


void __RPC_STUB IMSMQQueueInfo3_get_BasePriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_BasePriority_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long lBasePriority);


void __RPC_STUB IMSMQQueueInfo3_put_BasePriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_CreateTime_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarCreateTime);


void __RPC_STUB IMSMQQueueInfo3_get_CreateTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_ModifyTime_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarModifyTime);


void __RPC_STUB IMSMQQueueInfo3_get_ModifyTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_Authenticate_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthenticate);


void __RPC_STUB IMSMQQueueInfo3_get_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_Authenticate_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long lAuthenticate);


void __RPC_STUB IMSMQQueueInfo3_put_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_JournalQuota_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournalQuota);


void __RPC_STUB IMSMQQueueInfo3_get_JournalQuota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_JournalQuota_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long lJournalQuota);


void __RPC_STUB IMSMQQueueInfo3_put_JournalQuota_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_IsWorldReadable_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisWorldReadable);


void __RPC_STUB IMSMQQueueInfo3_get_IsWorldReadable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_Create_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *IsTransactional,
    /* [optional][in] */ VARIANT __RPC_FAR *IsWorldReadable);


void __RPC_STUB IMSMQQueueInfo3_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_Delete_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo3_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_Open_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ long Access,
    /* [in] */ long ShareMode,
    /* [retval][out] */ IMSMQQueue3 __RPC_FAR *__RPC_FAR *ppq);


void __RPC_STUB IMSMQQueueInfo3_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_Refresh_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo3_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_Update_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfo3_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_PathNameDNS_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPathNameDNS);


void __RPC_STUB IMSMQQueueInfo3_get_PathNameDNS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_Properties_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQueueInfo3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_Security_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSecurity);


void __RPC_STUB IMSMQQueueInfo3_get_Security_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_put_Security_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [in] */ VARIANT varSecurity);


void __RPC_STUB IMSMQQueueInfo3_put_Security_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfo3_get_ADsPath_Proxy( 
    IMSMQQueueInfo3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrADsPath);


void __RPC_STUB IMSMQQueueInfo3_get_ADsPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueueInfo3_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueue_INTERFACE_DEFINED__
#define __IMSMQQueue_INTERFACE_DEFINED__

/* interface IMSMQQueue */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueue,0xD7D6E076,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E076-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQQueue : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Access( 
            /* [retval][out] */ long __RPC_FAR *plAccess) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ShareMode( 
            /* [retval][out] */ long __RPC_FAR *plShareMode) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfo) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Handle( 
            /* [retval][out] */ long __RPC_FAR *plHandle) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsOpen( 
            /* [retval][out] */ Boolean __RPC_FAR *pisOpen) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Receive( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Peek( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE EnableNotification( 
            /* [in] */ IMSMQEvent __RPC_FAR *Event,
            /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveCurrent( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekNext( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekCurrent( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueue __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueue __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueue __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueue __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueue __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Access )( 
            IMSMQQueue __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAccess);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShareMode )( 
            IMSMQQueue __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plShareMode);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueueInfo )( 
            IMSMQQueue __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfo);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Handle )( 
            IMSMQQueue __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plHandle);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsOpen )( 
            IMSMQQueue __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisOpen);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IMSMQQueue __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Receive )( 
            IMSMQQueue __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Peek )( 
            IMSMQQueue __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableNotification )( 
            IMSMQQueue __RPC_FAR * This,
            /* [in] */ IMSMQEvent __RPC_FAR *Event,
            /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IMSMQQueue __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveCurrent )( 
            IMSMQQueue __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekNext )( 
            IMSMQQueue __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekCurrent )( 
            IMSMQQueue __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        END_INTERFACE
    } IMSMQQueueVtbl;

    interface IMSMQQueue
    {
        CONST_VTBL struct IMSMQQueueVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueue_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueue_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueue_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueue_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueue_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueue_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueue_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueue_get_Access(This,plAccess)	\
    (This)->lpVtbl -> get_Access(This,plAccess)

#define IMSMQQueue_get_ShareMode(This,plShareMode)	\
    (This)->lpVtbl -> get_ShareMode(This,plShareMode)

#define IMSMQQueue_get_QueueInfo(This,ppqinfo)	\
    (This)->lpVtbl -> get_QueueInfo(This,ppqinfo)

#define IMSMQQueue_get_Handle(This,plHandle)	\
    (This)->lpVtbl -> get_Handle(This,plHandle)

#define IMSMQQueue_get_IsOpen(This,pisOpen)	\
    (This)->lpVtbl -> get_IsOpen(This,pisOpen)

#define IMSMQQueue_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IMSMQQueue_Receive(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> Receive(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue_Peek(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> Peek(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue_EnableNotification(This,Event,Cursor,ReceiveTimeout)	\
    (This)->lpVtbl -> EnableNotification(This,Event,Cursor,ReceiveTimeout)

#define IMSMQQueue_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IMSMQQueue_ReceiveCurrent(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> ReceiveCurrent(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue_PeekNext(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> PeekNext(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue_PeekCurrent(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> PeekCurrent(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_get_Access_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAccess);


void __RPC_STUB IMSMQQueue_get_Access_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_get_ShareMode_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plShareMode);


void __RPC_STUB IMSMQQueue_get_ShareMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_get_QueueInfo_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfo);


void __RPC_STUB IMSMQQueue_get_QueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_get_Handle_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plHandle);


void __RPC_STUB IMSMQQueue_get_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_get_IsOpen_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisOpen);


void __RPC_STUB IMSMQQueue_get_IsOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_Close_Proxy( 
    IMSMQQueue __RPC_FAR * This);


void __RPC_STUB IMSMQQueue_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_Receive_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue_Receive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_Peek_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue_Peek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_EnableNotification_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [in] */ IMSMQEvent __RPC_FAR *Event,
    /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout);


void __RPC_STUB IMSMQQueue_EnableNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_Reset_Proxy( 
    IMSMQQueue __RPC_FAR * This);


void __RPC_STUB IMSMQQueue_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_ReceiveCurrent_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue_ReceiveCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_PeekNext_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue_PeekNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue_PeekCurrent_Proxy( 
    IMSMQQueue __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue_PeekCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueue_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueue2_INTERFACE_DEFINED__
#define __IMSMQQueue2_INTERFACE_DEFINED__

/* interface IMSMQQueue2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueue2,0xEF0574E0,0x06D8,0x11D3,0xB1,0x00,0x00,0xE0,0x2C,0x07,0x4F,0x6B);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF0574E0-06D8-11D3-B100-00E02C074F6B")
    IMSMQQueue2 : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Access( 
            /* [retval][out] */ long __RPC_FAR *plAccess) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ShareMode( 
            /* [retval][out] */ long __RPC_FAR *plShareMode) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfo) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Handle( 
            /* [retval][out] */ long __RPC_FAR *plHandle) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsOpen( 
            /* [retval][out] */ Boolean __RPC_FAR *pisOpen) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE Receive_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE Peek_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE EnableNotification( 
            /* [in] */ IMSMQEvent2 __RPC_FAR *Event,
            /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveCurrent_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekNext_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekCurrent_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Receive( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Peek( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveCurrent( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekNext( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekCurrent( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueue2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueue2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueue2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Access )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAccess);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShareMode )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plShareMode);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueueInfo )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfo);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Handle )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plHandle);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsOpen )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisOpen);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IMSMQQueue2 __RPC_FAR * This);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Receive_v1 )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Peek_v1 )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableNotification )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [in] */ IMSMQEvent2 __RPC_FAR *Event,
            /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IMSMQQueue2 __RPC_FAR * This);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveCurrent_v1 )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekNext_v1 )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekCurrent_v1 )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Receive )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Peek )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveCurrent )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekNext )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekCurrent )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQueue2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQQueue2Vtbl;

    interface IMSMQQueue2
    {
        CONST_VTBL struct IMSMQQueue2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueue2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueue2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueue2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueue2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueue2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueue2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueue2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueue2_get_Access(This,plAccess)	\
    (This)->lpVtbl -> get_Access(This,plAccess)

#define IMSMQQueue2_get_ShareMode(This,plShareMode)	\
    (This)->lpVtbl -> get_ShareMode(This,plShareMode)

#define IMSMQQueue2_get_QueueInfo(This,ppqinfo)	\
    (This)->lpVtbl -> get_QueueInfo(This,ppqinfo)

#define IMSMQQueue2_get_Handle(This,plHandle)	\
    (This)->lpVtbl -> get_Handle(This,plHandle)

#define IMSMQQueue2_get_IsOpen(This,pisOpen)	\
    (This)->lpVtbl -> get_IsOpen(This,pisOpen)

#define IMSMQQueue2_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IMSMQQueue2_Receive_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> Receive_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue2_Peek_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> Peek_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue2_EnableNotification(This,Event,Cursor,ReceiveTimeout)	\
    (This)->lpVtbl -> EnableNotification(This,Event,Cursor,ReceiveTimeout)

#define IMSMQQueue2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IMSMQQueue2_ReceiveCurrent_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> ReceiveCurrent_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue2_PeekNext_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> PeekNext_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue2_PeekCurrent_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> PeekCurrent_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue2_Receive(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> Receive(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue2_Peek(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> Peek(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue2_ReceiveCurrent(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceiveCurrent(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue2_PeekNext(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekNext(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue2_PeekCurrent(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekCurrent(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_get_Access_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAccess);


void __RPC_STUB IMSMQQueue2_get_Access_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_get_ShareMode_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plShareMode);


void __RPC_STUB IMSMQQueue2_get_ShareMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_get_QueueInfo_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfo);


void __RPC_STUB IMSMQQueue2_get_QueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_get_Handle_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plHandle);


void __RPC_STUB IMSMQQueue2_get_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_get_IsOpen_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisOpen);


void __RPC_STUB IMSMQQueue2_get_IsOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_Close_Proxy( 
    IMSMQQueue2 __RPC_FAR * This);


void __RPC_STUB IMSMQQueue2_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_Receive_v1_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_Receive_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_Peek_v1_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_Peek_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_EnableNotification_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [in] */ IMSMQEvent2 __RPC_FAR *Event,
    /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout);


void __RPC_STUB IMSMQQueue2_EnableNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_Reset_Proxy( 
    IMSMQQueue2 __RPC_FAR * This);


void __RPC_STUB IMSMQQueue2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_ReceiveCurrent_v1_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_ReceiveCurrent_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_PeekNext_v1_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_PeekNext_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_PeekCurrent_v1_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_PeekCurrent_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_Receive_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_Receive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_Peek_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_Peek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_ReceiveCurrent_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_ReceiveCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_PeekNext_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_PeekNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_PeekCurrent_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage2 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue2_PeekCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueue2_get_Properties_Proxy( 
    IMSMQQueue2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQueue2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueue2_INTERFACE_DEFINED__ */


#ifndef __IMSMQMessage_INTERFACE_DEFINED__
#define __IMSMQMessage_INTERFACE_DEFINED__

/* interface IMSMQMessage */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQMessage,0xD7D6E074,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E074-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQMessage : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ long __RPC_FAR *plClass) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PrivLevel( 
            /* [retval][out] */ long __RPC_FAR *plPrivLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PrivLevel( 
            /* [in] */ long lPrivLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthLevel( 
            /* [retval][out] */ long __RPC_FAR *plAuthLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthLevel( 
            /* [in] */ long lAuthLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsAuthenticated( 
            /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Delivery( 
            /* [retval][out] */ long __RPC_FAR *plDelivery) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Delivery( 
            /* [in] */ long lDelivery) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Trace( 
            /* [retval][out] */ long __RPC_FAR *plTrace) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Trace( 
            /* [in] */ long lTrace) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Priority( 
            /* [retval][out] */ long __RPC_FAR *plPriority) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Priority( 
            /* [in] */ long lPriority) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Journal( 
            /* [retval][out] */ long __RPC_FAR *plJournal) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Journal( 
            /* [in] */ long lJournal) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseQueueInfo( 
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AppSpecific( 
            /* [retval][out] */ long __RPC_FAR *plAppSpecific) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AppSpecific( 
            /* [in] */ long lAppSpecific) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SourceMachineGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_BodyLength( 
            /* [retval][out] */ long __RPC_FAR *pcbBody) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Body( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBody) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Body( 
            /* [in] */ VARIANT varBody) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminQueueInfo( 
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_CorrelationId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_CorrelationId( 
            /* [in] */ VARIANT varMsgId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Ack( 
            /* [retval][out] */ long __RPC_FAR *plAck) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Ack( 
            /* [in] */ long lAck) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR bstrLabel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MaxTimeToReachQueue( 
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MaxTimeToReachQueue( 
            /* [in] */ long lMaxTimeToReachQueue) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MaxTimeToReceive( 
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MaxTimeToReceive( 
            /* [in] */ long lMaxTimeToReceive) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_HashAlgorithm( 
            /* [retval][out] */ long __RPC_FAR *plHashAlg) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_HashAlgorithm( 
            /* [in] */ long lHashAlg) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_EncryptAlgorithm( 
            /* [retval][out] */ long __RPC_FAR *plEncryptAlg) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_EncryptAlgorithm( 
            /* [in] */ long lEncryptAlg) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SentTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ArrivedTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_DestinationQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoDest) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderCertificate( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderCertificate( 
            /* [in] */ VARIANT varSenderCert) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderIdType( 
            /* [retval][out] */ long __RPC_FAR *plSenderIdType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderIdType( 
            /* [in] */ long lSenderIdType) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Send( 
            /* [in] */ IMSMQQueue __RPC_FAR *DestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE AttachCurrentSecurityContext( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQMessageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQMessage __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQMessage __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQMessage __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Class )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plClass);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrivLevel )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPrivLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrivLevel )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lPrivLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthLevel )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthLevel )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lAuthLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsAuthenticated )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Delivery )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plDelivery);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Delivery )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lDelivery);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Trace )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plTrace);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Trace )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lTrace);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Priority )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPriority);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Priority )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lPriority);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Journal )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournal);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Journal )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lJournal);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseQueueInfo )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseQueueInfo )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AppSpecific )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAppSpecific);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AppSpecific )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lAppSpecific);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceMachineGuid )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BodyLength )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pcbBody);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Body )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBody);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Body )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ VARIANT varBody);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminQueueInfo )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminQueueInfo )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Id )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CorrelationId )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CorrelationId )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ VARIANT varMsgId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Ack )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAck);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Ack )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lAck);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Label )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ BSTR bstrLabel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxTimeToReachQueue )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxTimeToReachQueue )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lMaxTimeToReachQueue);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxTimeToReceive )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxTimeToReceive )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lMaxTimeToReceive);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithm )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plHashAlg);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithm )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lHashAlg);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EncryptAlgorithm )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plEncryptAlg);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EncryptAlgorithm )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lEncryptAlg);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SentTime )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ArrivedTime )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationQueueInfo )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoDest);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderCertificate )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderCertificate )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ VARIANT varSenderCert);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderId )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderIdType )( 
            IMSMQMessage __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSenderIdType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderIdType )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ long lSenderIdType);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send )( 
            IMSMQMessage __RPC_FAR * This,
            /* [in] */ IMSMQQueue __RPC_FAR *DestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AttachCurrentSecurityContext )( 
            IMSMQMessage __RPC_FAR * This);
        
        END_INTERFACE
    } IMSMQMessageVtbl;

    interface IMSMQMessage
    {
        CONST_VTBL struct IMSMQMessageVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQMessage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQMessage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQMessage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQMessage_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQMessage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQMessage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQMessage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQMessage_get_Class(This,plClass)	\
    (This)->lpVtbl -> get_Class(This,plClass)

#define IMSMQMessage_get_PrivLevel(This,plPrivLevel)	\
    (This)->lpVtbl -> get_PrivLevel(This,plPrivLevel)

#define IMSMQMessage_put_PrivLevel(This,lPrivLevel)	\
    (This)->lpVtbl -> put_PrivLevel(This,lPrivLevel)

#define IMSMQMessage_get_AuthLevel(This,plAuthLevel)	\
    (This)->lpVtbl -> get_AuthLevel(This,plAuthLevel)

#define IMSMQMessage_put_AuthLevel(This,lAuthLevel)	\
    (This)->lpVtbl -> put_AuthLevel(This,lAuthLevel)

#define IMSMQMessage_get_IsAuthenticated(This,pisAuthenticated)	\
    (This)->lpVtbl -> get_IsAuthenticated(This,pisAuthenticated)

#define IMSMQMessage_get_Delivery(This,plDelivery)	\
    (This)->lpVtbl -> get_Delivery(This,plDelivery)

#define IMSMQMessage_put_Delivery(This,lDelivery)	\
    (This)->lpVtbl -> put_Delivery(This,lDelivery)

#define IMSMQMessage_get_Trace(This,plTrace)	\
    (This)->lpVtbl -> get_Trace(This,plTrace)

#define IMSMQMessage_put_Trace(This,lTrace)	\
    (This)->lpVtbl -> put_Trace(This,lTrace)

#define IMSMQMessage_get_Priority(This,plPriority)	\
    (This)->lpVtbl -> get_Priority(This,plPriority)

#define IMSMQMessage_put_Priority(This,lPriority)	\
    (This)->lpVtbl -> put_Priority(This,lPriority)

#define IMSMQMessage_get_Journal(This,plJournal)	\
    (This)->lpVtbl -> get_Journal(This,plJournal)

#define IMSMQMessage_put_Journal(This,lJournal)	\
    (This)->lpVtbl -> put_Journal(This,lJournal)

#define IMSMQMessage_get_ResponseQueueInfo(This,ppqinfoResponse)	\
    (This)->lpVtbl -> get_ResponseQueueInfo(This,ppqinfoResponse)

#define IMSMQMessage_putref_ResponseQueueInfo(This,pqinfoResponse)	\
    (This)->lpVtbl -> putref_ResponseQueueInfo(This,pqinfoResponse)

#define IMSMQMessage_get_AppSpecific(This,plAppSpecific)	\
    (This)->lpVtbl -> get_AppSpecific(This,plAppSpecific)

#define IMSMQMessage_put_AppSpecific(This,lAppSpecific)	\
    (This)->lpVtbl -> put_AppSpecific(This,lAppSpecific)

#define IMSMQMessage_get_SourceMachineGuid(This,pbstrGuidSrcMachine)	\
    (This)->lpVtbl -> get_SourceMachineGuid(This,pbstrGuidSrcMachine)

#define IMSMQMessage_get_BodyLength(This,pcbBody)	\
    (This)->lpVtbl -> get_BodyLength(This,pcbBody)

#define IMSMQMessage_get_Body(This,pvarBody)	\
    (This)->lpVtbl -> get_Body(This,pvarBody)

#define IMSMQMessage_put_Body(This,varBody)	\
    (This)->lpVtbl -> put_Body(This,varBody)

#define IMSMQMessage_get_AdminQueueInfo(This,ppqinfoAdmin)	\
    (This)->lpVtbl -> get_AdminQueueInfo(This,ppqinfoAdmin)

#define IMSMQMessage_putref_AdminQueueInfo(This,pqinfoAdmin)	\
    (This)->lpVtbl -> putref_AdminQueueInfo(This,pqinfoAdmin)

#define IMSMQMessage_get_Id(This,pvarMsgId)	\
    (This)->lpVtbl -> get_Id(This,pvarMsgId)

#define IMSMQMessage_get_CorrelationId(This,pvarMsgId)	\
    (This)->lpVtbl -> get_CorrelationId(This,pvarMsgId)

#define IMSMQMessage_put_CorrelationId(This,varMsgId)	\
    (This)->lpVtbl -> put_CorrelationId(This,varMsgId)

#define IMSMQMessage_get_Ack(This,plAck)	\
    (This)->lpVtbl -> get_Ack(This,plAck)

#define IMSMQMessage_put_Ack(This,lAck)	\
    (This)->lpVtbl -> put_Ack(This,lAck)

#define IMSMQMessage_get_Label(This,pbstrLabel)	\
    (This)->lpVtbl -> get_Label(This,pbstrLabel)

#define IMSMQMessage_put_Label(This,bstrLabel)	\
    (This)->lpVtbl -> put_Label(This,bstrLabel)

#define IMSMQMessage_get_MaxTimeToReachQueue(This,plMaxTimeToReachQueue)	\
    (This)->lpVtbl -> get_MaxTimeToReachQueue(This,plMaxTimeToReachQueue)

#define IMSMQMessage_put_MaxTimeToReachQueue(This,lMaxTimeToReachQueue)	\
    (This)->lpVtbl -> put_MaxTimeToReachQueue(This,lMaxTimeToReachQueue)

#define IMSMQMessage_get_MaxTimeToReceive(This,plMaxTimeToReceive)	\
    (This)->lpVtbl -> get_MaxTimeToReceive(This,plMaxTimeToReceive)

#define IMSMQMessage_put_MaxTimeToReceive(This,lMaxTimeToReceive)	\
    (This)->lpVtbl -> put_MaxTimeToReceive(This,lMaxTimeToReceive)

#define IMSMQMessage_get_HashAlgorithm(This,plHashAlg)	\
    (This)->lpVtbl -> get_HashAlgorithm(This,plHashAlg)

#define IMSMQMessage_put_HashAlgorithm(This,lHashAlg)	\
    (This)->lpVtbl -> put_HashAlgorithm(This,lHashAlg)

#define IMSMQMessage_get_EncryptAlgorithm(This,plEncryptAlg)	\
    (This)->lpVtbl -> get_EncryptAlgorithm(This,plEncryptAlg)

#define IMSMQMessage_put_EncryptAlgorithm(This,lEncryptAlg)	\
    (This)->lpVtbl -> put_EncryptAlgorithm(This,lEncryptAlg)

#define IMSMQMessage_get_SentTime(This,pvarSentTime)	\
    (This)->lpVtbl -> get_SentTime(This,pvarSentTime)

#define IMSMQMessage_get_ArrivedTime(This,plArrivedTime)	\
    (This)->lpVtbl -> get_ArrivedTime(This,plArrivedTime)

#define IMSMQMessage_get_DestinationQueueInfo(This,ppqinfoDest)	\
    (This)->lpVtbl -> get_DestinationQueueInfo(This,ppqinfoDest)

#define IMSMQMessage_get_SenderCertificate(This,pvarSenderCert)	\
    (This)->lpVtbl -> get_SenderCertificate(This,pvarSenderCert)

#define IMSMQMessage_put_SenderCertificate(This,varSenderCert)	\
    (This)->lpVtbl -> put_SenderCertificate(This,varSenderCert)

#define IMSMQMessage_get_SenderId(This,pvarSenderId)	\
    (This)->lpVtbl -> get_SenderId(This,pvarSenderId)

#define IMSMQMessage_get_SenderIdType(This,plSenderIdType)	\
    (This)->lpVtbl -> get_SenderIdType(This,plSenderIdType)

#define IMSMQMessage_put_SenderIdType(This,lSenderIdType)	\
    (This)->lpVtbl -> put_SenderIdType(This,lSenderIdType)

#define IMSMQMessage_Send(This,DestinationQueue,Transaction)	\
    (This)->lpVtbl -> Send(This,DestinationQueue,Transaction)

#define IMSMQMessage_AttachCurrentSecurityContext(This)	\
    (This)->lpVtbl -> AttachCurrentSecurityContext(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Class_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plClass);


void __RPC_STUB IMSMQMessage_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_PrivLevel_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPrivLevel);


void __RPC_STUB IMSMQMessage_get_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_PrivLevel_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lPrivLevel);


void __RPC_STUB IMSMQMessage_put_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_AuthLevel_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthLevel);


void __RPC_STUB IMSMQMessage_get_AuthLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_AuthLevel_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lAuthLevel);


void __RPC_STUB IMSMQMessage_put_AuthLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_IsAuthenticated_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated);


void __RPC_STUB IMSMQMessage_get_IsAuthenticated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Delivery_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plDelivery);


void __RPC_STUB IMSMQMessage_get_Delivery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Delivery_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lDelivery);


void __RPC_STUB IMSMQMessage_put_Delivery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Trace_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plTrace);


void __RPC_STUB IMSMQMessage_get_Trace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Trace_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lTrace);


void __RPC_STUB IMSMQMessage_put_Trace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Priority_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPriority);


void __RPC_STUB IMSMQMessage_get_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Priority_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lPriority);


void __RPC_STUB IMSMQMessage_put_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Journal_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournal);


void __RPC_STUB IMSMQMessage_get_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Journal_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lJournal);


void __RPC_STUB IMSMQMessage_put_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_ResponseQueueInfo_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse);


void __RPC_STUB IMSMQMessage_get_ResponseQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_putref_ResponseQueueInfo_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse);


void __RPC_STUB IMSMQMessage_putref_ResponseQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_AppSpecific_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAppSpecific);


void __RPC_STUB IMSMQMessage_get_AppSpecific_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_AppSpecific_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lAppSpecific);


void __RPC_STUB IMSMQMessage_put_AppSpecific_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_SourceMachineGuid_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine);


void __RPC_STUB IMSMQMessage_get_SourceMachineGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_BodyLength_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pcbBody);


void __RPC_STUB IMSMQMessage_get_BodyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Body_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBody);


void __RPC_STUB IMSMQMessage_get_Body_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Body_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ VARIANT varBody);


void __RPC_STUB IMSMQMessage_put_Body_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_AdminQueueInfo_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin);


void __RPC_STUB IMSMQMessage_get_AdminQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_putref_AdminQueueInfo_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin);


void __RPC_STUB IMSMQMessage_putref_AdminQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Id_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);


void __RPC_STUB IMSMQMessage_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_CorrelationId_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);


void __RPC_STUB IMSMQMessage_get_CorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_CorrelationId_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ VARIANT varMsgId);


void __RPC_STUB IMSMQMessage_put_CorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Ack_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAck);


void __RPC_STUB IMSMQMessage_get_Ack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Ack_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lAck);


void __RPC_STUB IMSMQMessage_put_Ack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_Label_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);


void __RPC_STUB IMSMQMessage_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_Label_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ BSTR bstrLabel);


void __RPC_STUB IMSMQMessage_put_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_MaxTimeToReachQueue_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue);


void __RPC_STUB IMSMQMessage_get_MaxTimeToReachQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_MaxTimeToReachQueue_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lMaxTimeToReachQueue);


void __RPC_STUB IMSMQMessage_put_MaxTimeToReachQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_MaxTimeToReceive_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive);


void __RPC_STUB IMSMQMessage_get_MaxTimeToReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_MaxTimeToReceive_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lMaxTimeToReceive);


void __RPC_STUB IMSMQMessage_put_MaxTimeToReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_HashAlgorithm_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plHashAlg);


void __RPC_STUB IMSMQMessage_get_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_HashAlgorithm_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lHashAlg);


void __RPC_STUB IMSMQMessage_put_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_EncryptAlgorithm_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plEncryptAlg);


void __RPC_STUB IMSMQMessage_get_EncryptAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_EncryptAlgorithm_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lEncryptAlg);


void __RPC_STUB IMSMQMessage_put_EncryptAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_SentTime_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime);


void __RPC_STUB IMSMQMessage_get_SentTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_ArrivedTime_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime);


void __RPC_STUB IMSMQMessage_get_ArrivedTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_DestinationQueueInfo_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoDest);


void __RPC_STUB IMSMQMessage_get_DestinationQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_SenderCertificate_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert);


void __RPC_STUB IMSMQMessage_get_SenderCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_SenderCertificate_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ VARIANT varSenderCert);


void __RPC_STUB IMSMQMessage_put_SenderCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_SenderId_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId);


void __RPC_STUB IMSMQMessage_get_SenderId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_get_SenderIdType_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSenderIdType);


void __RPC_STUB IMSMQMessage_get_SenderIdType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_put_SenderIdType_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ long lSenderIdType);


void __RPC_STUB IMSMQMessage_put_SenderIdType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_Send_Proxy( 
    IMSMQMessage __RPC_FAR * This,
    /* [in] */ IMSMQQueue __RPC_FAR *DestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction);


void __RPC_STUB IMSMQMessage_Send_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage_AttachCurrentSecurityContext_Proxy( 
    IMSMQMessage __RPC_FAR * This);


void __RPC_STUB IMSMQMessage_AttachCurrentSecurityContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQMessage_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueueInfos_INTERFACE_DEFINED__
#define __IMSMQQueueInfos_INTERFACE_DEFINED__

/* interface IMSMQQueueInfos */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueueInfos,0xD7D6E07D,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E07D-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQQueueInfos : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoNext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueInfosVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueueInfos __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueueInfos __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueueInfos __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueueInfos __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueueInfos __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueueInfos __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueueInfos __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IMSMQQueueInfos __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IMSMQQueueInfos __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoNext);
        
        END_INTERFACE
    } IMSMQQueueInfosVtbl;

    interface IMSMQQueueInfos
    {
        CONST_VTBL struct IMSMQQueueInfosVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueueInfos_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueueInfos_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueueInfos_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueueInfos_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueueInfos_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueueInfos_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueueInfos_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueueInfos_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IMSMQQueueInfos_Next(This,ppqinfoNext)	\
    (This)->lpVtbl -> Next(This,ppqinfoNext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos_Reset_Proxy( 
    IMSMQQueueInfos __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfos_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos_Next_Proxy( 
    IMSMQQueueInfos __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoNext);


void __RPC_STUB IMSMQQueueInfos_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueueInfos_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueueInfos2_INTERFACE_DEFINED__
#define __IMSMQQueueInfos2_INTERFACE_DEFINED__

/* interface IMSMQQueueInfos2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueueInfos2,0xeba96b0f,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b0f-2168-11d3-898c-00e02c074f6b")
    IMSMQQueueInfos2 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoNext) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueInfos2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueueInfos2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueueInfos2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IMSMQQueueInfos2 __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoNext);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQueueInfos2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQQueueInfos2Vtbl;

    interface IMSMQQueueInfos2
    {
        CONST_VTBL struct IMSMQQueueInfos2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueueInfos2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueueInfos2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueueInfos2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueueInfos2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueueInfos2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueueInfos2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueueInfos2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueueInfos2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IMSMQQueueInfos2_Next(This,ppqinfoNext)	\
    (This)->lpVtbl -> Next(This,ppqinfoNext)

#define IMSMQQueueInfos2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos2_Reset_Proxy( 
    IMSMQQueueInfos2 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfos2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos2_Next_Proxy( 
    IMSMQQueueInfos2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoNext);


void __RPC_STUB IMSMQQueueInfos2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos2_get_Properties_Proxy( 
    IMSMQQueueInfos2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQueueInfos2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueueInfos2_INTERFACE_DEFINED__ */


#ifndef __IMSMQQueueInfos3_INTERFACE_DEFINED__
#define __IMSMQQueueInfos3_INTERFACE_DEFINED__

/* interface IMSMQQueueInfos3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueueInfos3,0xeba96b1e,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b1e-2168-11d3-898c-00e02c074f6b")
    IMSMQQueueInfos3 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoNext) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueueInfos3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueueInfos3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueueInfos3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IMSMQQueueInfos3 __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoNext);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQueueInfos3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQQueueInfos3Vtbl;

    interface IMSMQQueueInfos3
    {
        CONST_VTBL struct IMSMQQueueInfos3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueueInfos3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueueInfos3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueueInfos3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueueInfos3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueueInfos3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueueInfos3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueueInfos3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueueInfos3_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IMSMQQueueInfos3_Next(This,ppqinfoNext)	\
    (This)->lpVtbl -> Next(This,ppqinfoNext)

#define IMSMQQueueInfos3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos3_Reset_Proxy( 
    IMSMQQueueInfos3 __RPC_FAR * This);


void __RPC_STUB IMSMQQueueInfos3_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos3_Next_Proxy( 
    IMSMQQueueInfos3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoNext);


void __RPC_STUB IMSMQQueueInfos3_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueueInfos3_get_Properties_Proxy( 
    IMSMQQueueInfos3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQueueInfos3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueueInfos3_INTERFACE_DEFINED__ */


#ifndef __IMSMQEvent_INTERFACE_DEFINED__
#define __IMSMQEvent_INTERFACE_DEFINED__

/* interface IMSMQEvent */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQEvent,0xD7D6E077,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E077-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQEvent : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMSMQEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQEvent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQEvent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQEvent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IMSMQEventVtbl;

    interface IMSMQEvent
    {
        CONST_VTBL struct IMSMQEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMSMQEvent_INTERFACE_DEFINED__ */


#ifndef __IMSMQEvent2_INTERFACE_DEFINED__
#define __IMSMQEvent2_INTERFACE_DEFINED__

/* interface IMSMQEvent2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQEvent2,0xeba96b12,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b12-2168-11d3-898c-00e02c074f6b")
    IMSMQEvent2 : public IMSMQEvent
    {
    public:
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQEvent2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQEvent2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQEvent2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQEvent2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQEvent2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQEvent2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQEvent2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQEvent2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQEvent2Vtbl;

    interface IMSMQEvent2
    {
        CONST_VTBL struct IMSMQEvent2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQEvent2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQEvent2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQEvent2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQEvent2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)



#define IMSMQEvent2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQEvent2_get_Properties_Proxy( 
    IMSMQEvent2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQEvent2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQEvent2_INTERFACE_DEFINED__ */


#ifndef __IMSMQEvent3_INTERFACE_DEFINED__
#define __IMSMQEvent3_INTERFACE_DEFINED__

/* interface IMSMQEvent3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQEvent3,0xeba96b1c,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b1c-2168-11d3-898c-00e02c074f6b")
    IMSMQEvent3 : public IMSMQEvent2
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMSMQEvent3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQEvent3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQEvent3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQEvent3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQEvent3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQEvent3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQEvent3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQEvent3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQEvent3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQEvent3Vtbl;

    interface IMSMQEvent3
    {
        CONST_VTBL struct IMSMQEvent3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQEvent3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQEvent3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQEvent3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQEvent3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQEvent3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQEvent3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQEvent3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)



#define IMSMQEvent3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMSMQEvent3_INTERFACE_DEFINED__ */


#ifndef __IMSMQTransaction_INTERFACE_DEFINED__
#define __IMSMQTransaction_INTERFACE_DEFINED__

/* interface IMSMQTransaction */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQTransaction,0xD7D6E07F,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E07F-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQTransaction : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Transaction( 
            /* [retval][out] */ long __RPC_FAR *plTransaction) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Commit( 
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *grfTC,
            /* [optional][in] */ VARIANT __RPC_FAR *grfRM) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Abort( 
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *fAsync) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQTransactionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQTransaction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQTransaction __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Transaction )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plTransaction);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *grfTC,
            /* [optional][in] */ VARIANT __RPC_FAR *grfRM);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abort )( 
            IMSMQTransaction __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *fAsync);
        
        END_INTERFACE
    } IMSMQTransactionVtbl;

    interface IMSMQTransaction
    {
        CONST_VTBL struct IMSMQTransactionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQTransaction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQTransaction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQTransaction_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQTransaction_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQTransaction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQTransaction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQTransaction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQTransaction_get_Transaction(This,plTransaction)	\
    (This)->lpVtbl -> get_Transaction(This,plTransaction)

#define IMSMQTransaction_Commit(This,fRetaining,grfTC,grfRM)	\
    (This)->lpVtbl -> Commit(This,fRetaining,grfTC,grfRM)

#define IMSMQTransaction_Abort(This,fRetaining,fAsync)	\
    (This)->lpVtbl -> Abort(This,fRetaining,fAsync)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransaction_get_Transaction_Proxy( 
    IMSMQTransaction __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plTransaction);


void __RPC_STUB IMSMQTransaction_get_Transaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransaction_Commit_Proxy( 
    IMSMQTransaction __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
    /* [optional][in] */ VARIANT __RPC_FAR *grfTC,
    /* [optional][in] */ VARIANT __RPC_FAR *grfRM);


void __RPC_STUB IMSMQTransaction_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransaction_Abort_Proxy( 
    IMSMQTransaction __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
    /* [optional][in] */ VARIANT __RPC_FAR *fAsync);


void __RPC_STUB IMSMQTransaction_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQTransaction_INTERFACE_DEFINED__ */


#ifndef __IMSMQCoordinatedTransactionDispenser_INTERFACE_DEFINED__
#define __IMSMQCoordinatedTransactionDispenser_INTERFACE_DEFINED__

/* interface IMSMQCoordinatedTransactionDispenser */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQCoordinatedTransactionDispenser,0xD7D6E081,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E081-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQCoordinatedTransactionDispenser : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [retval][out] */ IMSMQTransaction __RPC_FAR *__RPC_FAR *ptransaction) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQCoordinatedTransactionDispenserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransaction )( 
            IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
            /* [retval][out] */ IMSMQTransaction __RPC_FAR *__RPC_FAR *ptransaction);
        
        END_INTERFACE
    } IMSMQCoordinatedTransactionDispenserVtbl;

    interface IMSMQCoordinatedTransactionDispenser
    {
        CONST_VTBL struct IMSMQCoordinatedTransactionDispenserVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQCoordinatedTransactionDispenser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQCoordinatedTransactionDispenser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQCoordinatedTransactionDispenser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQCoordinatedTransactionDispenser_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQCoordinatedTransactionDispenser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQCoordinatedTransactionDispenser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQCoordinatedTransactionDispenser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQCoordinatedTransactionDispenser_BeginTransaction(This,ptransaction)	\
    (This)->lpVtbl -> BeginTransaction(This,ptransaction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQCoordinatedTransactionDispenser_BeginTransaction_Proxy( 
    IMSMQCoordinatedTransactionDispenser __RPC_FAR * This,
    /* [retval][out] */ IMSMQTransaction __RPC_FAR *__RPC_FAR *ptransaction);


void __RPC_STUB IMSMQCoordinatedTransactionDispenser_BeginTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQCoordinatedTransactionDispenser_INTERFACE_DEFINED__ */


#ifndef __IMSMQTransactionDispenser_INTERFACE_DEFINED__
#define __IMSMQTransactionDispenser_INTERFACE_DEFINED__

/* interface IMSMQTransactionDispenser */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQTransactionDispenser,0xD7D6E083,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E083-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQTransactionDispenser : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [retval][out] */ IMSMQTransaction __RPC_FAR *__RPC_FAR *ptransaction) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQTransactionDispenserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQTransactionDispenser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQTransactionDispenser __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQTransactionDispenser __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQTransactionDispenser __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQTransactionDispenser __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQTransactionDispenser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQTransactionDispenser __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransaction )( 
            IMSMQTransactionDispenser __RPC_FAR * This,
            /* [retval][out] */ IMSMQTransaction __RPC_FAR *__RPC_FAR *ptransaction);
        
        END_INTERFACE
    } IMSMQTransactionDispenserVtbl;

    interface IMSMQTransactionDispenser
    {
        CONST_VTBL struct IMSMQTransactionDispenserVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQTransactionDispenser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQTransactionDispenser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQTransactionDispenser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQTransactionDispenser_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQTransactionDispenser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQTransactionDispenser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQTransactionDispenser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQTransactionDispenser_BeginTransaction(This,ptransaction)	\
    (This)->lpVtbl -> BeginTransaction(This,ptransaction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransactionDispenser_BeginTransaction_Proxy( 
    IMSMQTransactionDispenser __RPC_FAR * This,
    /* [retval][out] */ IMSMQTransaction __RPC_FAR *__RPC_FAR *ptransaction);


void __RPC_STUB IMSMQTransactionDispenser_BeginTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQTransactionDispenser_INTERFACE_DEFINED__ */


#ifndef __IMSMQQuery2_INTERFACE_DEFINED__
#define __IMSMQQuery2_INTERFACE_DEFINED__

/* interface IMSMQQuery2 */
/* [object][nonextensible][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQuery2,0xeba96b0e,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b0e-2168-11d3-898c-00e02c074f6b")
    IMSMQQuery2 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE LookupQueue( 
            /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *Label,
            /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
            /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
            /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
            /* [retval][out] */ IMSMQQueueInfos2 __RPC_FAR *__RPC_FAR *ppqinfos) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQuery2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQuery2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQuery2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LookupQueue )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *Label,
            /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
            /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
            /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
            /* [retval][out] */ IMSMQQueueInfos2 __RPC_FAR *__RPC_FAR *ppqinfos);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQuery2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQQuery2Vtbl;

    interface IMSMQQuery2
    {
        CONST_VTBL struct IMSMQQuery2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQuery2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQuery2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQuery2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQuery2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQuery2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQuery2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQuery2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQuery2_LookupQueue(This,QueueGuid,ServiceTypeGuid,Label,CreateTime,ModifyTime,RelServiceType,RelLabel,RelCreateTime,RelModifyTime,ppqinfos)	\
    (This)->lpVtbl -> LookupQueue(This,QueueGuid,ServiceTypeGuid,Label,CreateTime,ModifyTime,RelServiceType,RelLabel,RelCreateTime,RelModifyTime,ppqinfos)

#define IMSMQQuery2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQuery2_LookupQueue_Proxy( 
    IMSMQQuery2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
    /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
    /* [optional][in] */ VARIANT __RPC_FAR *Label,
    /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
    /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
    /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
    /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
    /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
    /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
    /* [retval][out] */ IMSMQQueueInfos2 __RPC_FAR *__RPC_FAR *ppqinfos);


void __RPC_STUB IMSMQQuery2_LookupQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQuery2_get_Properties_Proxy( 
    IMSMQQuery2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQuery2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQuery2_INTERFACE_DEFINED__ */


#ifndef __IMSMQQuery3_INTERFACE_DEFINED__
#define __IMSMQQuery3_INTERFACE_DEFINED__

/* interface IMSMQQuery3 */
/* [object][nonextensible][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQuery3,0xeba96b19,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b19-2168-11d3-898c-00e02c074f6b")
    IMSMQQuery3 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE LookupQueue( 
            /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *Label,
            /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
            /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
            /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
            /* [retval][out] */ IMSMQQueueInfos3 __RPC_FAR *__RPC_FAR *ppqinfos) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQuery3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQuery3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQuery3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LookupQueue )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
            /* [optional][in] */ VARIANT __RPC_FAR *Label,
            /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
            /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
            /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
            /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
            /* [retval][out] */ IMSMQQueueInfos3 __RPC_FAR *__RPC_FAR *ppqinfos);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQuery3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQQuery3Vtbl;

    interface IMSMQQuery3
    {
        CONST_VTBL struct IMSMQQuery3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQuery3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQuery3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQuery3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQuery3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQuery3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQuery3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQuery3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQuery3_LookupQueue(This,QueueGuid,ServiceTypeGuid,Label,CreateTime,ModifyTime,RelServiceType,RelLabel,RelCreateTime,RelModifyTime,ppqinfos)	\
    (This)->lpVtbl -> LookupQueue(This,QueueGuid,ServiceTypeGuid,Label,CreateTime,ModifyTime,RelServiceType,RelLabel,RelCreateTime,RelModifyTime,ppqinfos)

#define IMSMQQuery3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQuery3_LookupQueue_Proxy( 
    IMSMQQuery3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *QueueGuid,
    /* [optional][in] */ VARIANT __RPC_FAR *ServiceTypeGuid,
    /* [optional][in] */ VARIANT __RPC_FAR *Label,
    /* [optional][in] */ VARIANT __RPC_FAR *CreateTime,
    /* [optional][in] */ VARIANT __RPC_FAR *ModifyTime,
    /* [optional][in] */ VARIANT __RPC_FAR *RelServiceType,
    /* [optional][in] */ VARIANT __RPC_FAR *RelLabel,
    /* [optional][in] */ VARIANT __RPC_FAR *RelCreateTime,
    /* [optional][in] */ VARIANT __RPC_FAR *RelModifyTime,
    /* [retval][out] */ IMSMQQueueInfos3 __RPC_FAR *__RPC_FAR *ppqinfos);


void __RPC_STUB IMSMQQuery3_LookupQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQuery3_get_Properties_Proxy( 
    IMSMQQuery3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQuery3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQuery3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQQuery,0xD7D6E073,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E073-DCCD-11d0-AA4B-0060970DEBAE")
MSMQQuery;
#endif

#ifndef __IMSMQMessage2_INTERFACE_DEFINED__
#define __IMSMQMessage2_INTERFACE_DEFINED__

/* interface IMSMQMessage2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQMessage2,0xD9933BE0,0xA567,0x11D2,0xB0,0xF3,0x00,0xE0,0x2C,0x07,0x4F,0x6B);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D9933BE0-A567-11D2-B0F3-00E02C074F6B")
    IMSMQMessage2 : public IDispatch
    {
    public:
        virtual /* [id][propget][hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ long __RPC_FAR *plClass) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PrivLevel( 
            /* [retval][out] */ long __RPC_FAR *plPrivLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PrivLevel( 
            /* [in] */ long lPrivLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthLevel( 
            /* [retval][out] */ long __RPC_FAR *plAuthLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthLevel( 
            /* [in] */ long lAuthLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsAuthenticated( 
            /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Delivery( 
            /* [retval][out] */ long __RPC_FAR *plDelivery) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Delivery( 
            /* [in] */ long lDelivery) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Trace( 
            /* [retval][out] */ long __RPC_FAR *plTrace) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Trace( 
            /* [in] */ long lTrace) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Priority( 
            /* [retval][out] */ long __RPC_FAR *plPriority) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Priority( 
            /* [in] */ long lPriority) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Journal( 
            /* [retval][out] */ long __RPC_FAR *plJournal) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Journal( 
            /* [in] */ long lJournal) = 0;
        
        virtual /* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseQueueInfo_v1( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse) = 0;
        
        virtual /* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseQueueInfo_v1( 
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AppSpecific( 
            /* [retval][out] */ long __RPC_FAR *plAppSpecific) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AppSpecific( 
            /* [in] */ long lAppSpecific) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SourceMachineGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_BodyLength( 
            /* [retval][out] */ long __RPC_FAR *pcbBody) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Body( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBody) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Body( 
            /* [in] */ VARIANT varBody) = 0;
        
        virtual /* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminQueueInfo_v1( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin) = 0;
        
        virtual /* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminQueueInfo_v1( 
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_CorrelationId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_CorrelationId( 
            /* [in] */ VARIANT varMsgId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Ack( 
            /* [retval][out] */ long __RPC_FAR *plAck) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Ack( 
            /* [in] */ long lAck) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR bstrLabel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MaxTimeToReachQueue( 
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MaxTimeToReachQueue( 
            /* [in] */ long lMaxTimeToReachQueue) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MaxTimeToReceive( 
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MaxTimeToReceive( 
            /* [in] */ long lMaxTimeToReceive) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_HashAlgorithm( 
            /* [retval][out] */ long __RPC_FAR *plHashAlg) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_HashAlgorithm( 
            /* [in] */ long lHashAlg) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_EncryptAlgorithm( 
            /* [retval][out] */ long __RPC_FAR *plEncryptAlg) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_EncryptAlgorithm( 
            /* [in] */ long lEncryptAlg) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SentTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ArrivedTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_DestinationQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoDest) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderCertificate( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderCertificate( 
            /* [in] */ VARIANT varSenderCert) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderIdType( 
            /* [retval][out] */ long __RPC_FAR *plSenderIdType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderIdType( 
            /* [in] */ long lSenderIdType) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Send( 
            /* [in] */ IMSMQQueue2 __RPC_FAR *DestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE AttachCurrentSecurityContext( void) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderVersion( 
            /* [retval][out] */ long __RPC_FAR *plSenderVersion) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Extension( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarExtension) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Extension( 
            /* [in] */ VARIANT varExtension) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ConnectorTypeGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidConnectorType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_ConnectorTypeGuid( 
            /* [in] */ BSTR bstrGuidConnectorType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_TransactionStatusQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoXactStatus) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_DestinationSymmetricKey( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarDestSymmKey) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_DestinationSymmetricKey( 
            /* [in] */ VARIANT varDestSymmKey) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Signature( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSignature) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Signature( 
            /* [in] */ VARIANT varSignature) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthenticationProviderType( 
            /* [retval][out] */ long __RPC_FAR *plAuthProvType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthenticationProviderType( 
            /* [in] */ long lAuthProvType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthenticationProviderName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthProvName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthenticationProviderName( 
            /* [in] */ BSTR bstrAuthProvName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderId( 
            /* [in] */ VARIANT varSenderId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MsgClass( 
            /* [retval][out] */ long __RPC_FAR *plMsgClass) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MsgClass( 
            /* [in] */ long lMsgClass) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_TransactionId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarXactId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsFirstInTransaction( 
            /* [retval][out] */ Boolean __RPC_FAR *pisFirstInXact) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsLastInTransaction( 
            /* [retval][out] */ Boolean __RPC_FAR *pisLastInXact) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoResponse) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseQueueInfo( 
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoResponse) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoAdmin) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminQueueInfo( 
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ReceivedAuthenticationLevel( 
            /* [retval][out] */ short __RPC_FAR *psReceivedAuthenticationLevel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQMessage2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQMessage2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQMessage2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Class )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plClass);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrivLevel )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPrivLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrivLevel )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lPrivLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthLevel )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthLevel )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lAuthLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsAuthenticated )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Delivery )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plDelivery);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Delivery )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lDelivery);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Trace )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plTrace);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Trace )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lTrace);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Priority )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPriority);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Priority )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lPriority);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Journal )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournal);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Journal )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lJournal);
        
        /* [hidden][id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseQueueInfo_v1 )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse);
        
        /* [hidden][id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseQueueInfo_v1 )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AppSpecific )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAppSpecific);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AppSpecific )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lAppSpecific);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceMachineGuid )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BodyLength )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pcbBody);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Body )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBody);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Body )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varBody);
        
        /* [hidden][id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminQueueInfo_v1 )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin);
        
        /* [hidden][id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminQueueInfo_v1 )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Id )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CorrelationId )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CorrelationId )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varMsgId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Ack )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAck);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Ack )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lAck);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Label )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ BSTR bstrLabel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxTimeToReachQueue )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxTimeToReachQueue )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lMaxTimeToReachQueue);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxTimeToReceive )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxTimeToReceive )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lMaxTimeToReceive);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithm )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plHashAlg);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithm )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lHashAlg);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EncryptAlgorithm )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plEncryptAlg);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EncryptAlgorithm )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lEncryptAlg);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SentTime )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ArrivedTime )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationQueueInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoDest);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderCertificate )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderCertificate )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varSenderCert);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderId )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderIdType )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSenderIdType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderIdType )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lSenderIdType);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ IMSMQQueue2 __RPC_FAR *DestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AttachCurrentSecurityContext )( 
            IMSMQMessage2 __RPC_FAR * This);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderVersion )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSenderVersion);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Extension )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarExtension);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Extension )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varExtension);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectorTypeGuid )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidConnectorType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectorTypeGuid )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ BSTR bstrGuidConnectorType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransactionStatusQueueInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoXactStatus);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationSymmetricKey )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarDestSymmKey);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationSymmetricKey )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varDestSymmKey);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Signature )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSignature);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Signature )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varSignature);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthenticationProviderType )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthProvType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthenticationProviderType )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lAuthProvType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthenticationProviderName )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthProvName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthenticationProviderName )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ BSTR bstrAuthProvName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderId )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ VARIANT varSenderId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MsgClass )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMsgClass);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MsgClass )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ long lMsgClass);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransactionId )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarXactId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsFirstInTransaction )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisFirstInXact);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsLastInTransaction )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisLastInXact);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseQueueInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoResponse);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseQueueInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoResponse);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminQueueInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoAdmin);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminQueueInfo )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceivedAuthenticationLevel )( 
            IMSMQMessage2 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psReceivedAuthenticationLevel);
        
        END_INTERFACE
    } IMSMQMessage2Vtbl;

    interface IMSMQMessage2
    {
        CONST_VTBL struct IMSMQMessage2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQMessage2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQMessage2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQMessage2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQMessage2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQMessage2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQMessage2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQMessage2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQMessage2_get_Class(This,plClass)	\
    (This)->lpVtbl -> get_Class(This,plClass)

#define IMSMQMessage2_get_PrivLevel(This,plPrivLevel)	\
    (This)->lpVtbl -> get_PrivLevel(This,plPrivLevel)

#define IMSMQMessage2_put_PrivLevel(This,lPrivLevel)	\
    (This)->lpVtbl -> put_PrivLevel(This,lPrivLevel)

#define IMSMQMessage2_get_AuthLevel(This,plAuthLevel)	\
    (This)->lpVtbl -> get_AuthLevel(This,plAuthLevel)

#define IMSMQMessage2_put_AuthLevel(This,lAuthLevel)	\
    (This)->lpVtbl -> put_AuthLevel(This,lAuthLevel)

#define IMSMQMessage2_get_IsAuthenticated(This,pisAuthenticated)	\
    (This)->lpVtbl -> get_IsAuthenticated(This,pisAuthenticated)

#define IMSMQMessage2_get_Delivery(This,plDelivery)	\
    (This)->lpVtbl -> get_Delivery(This,plDelivery)

#define IMSMQMessage2_put_Delivery(This,lDelivery)	\
    (This)->lpVtbl -> put_Delivery(This,lDelivery)

#define IMSMQMessage2_get_Trace(This,plTrace)	\
    (This)->lpVtbl -> get_Trace(This,plTrace)

#define IMSMQMessage2_put_Trace(This,lTrace)	\
    (This)->lpVtbl -> put_Trace(This,lTrace)

#define IMSMQMessage2_get_Priority(This,plPriority)	\
    (This)->lpVtbl -> get_Priority(This,plPriority)

#define IMSMQMessage2_put_Priority(This,lPriority)	\
    (This)->lpVtbl -> put_Priority(This,lPriority)

#define IMSMQMessage2_get_Journal(This,plJournal)	\
    (This)->lpVtbl -> get_Journal(This,plJournal)

#define IMSMQMessage2_put_Journal(This,lJournal)	\
    (This)->lpVtbl -> put_Journal(This,lJournal)

#define IMSMQMessage2_get_ResponseQueueInfo_v1(This,ppqinfoResponse)	\
    (This)->lpVtbl -> get_ResponseQueueInfo_v1(This,ppqinfoResponse)

#define IMSMQMessage2_putref_ResponseQueueInfo_v1(This,pqinfoResponse)	\
    (This)->lpVtbl -> putref_ResponseQueueInfo_v1(This,pqinfoResponse)

#define IMSMQMessage2_get_AppSpecific(This,plAppSpecific)	\
    (This)->lpVtbl -> get_AppSpecific(This,plAppSpecific)

#define IMSMQMessage2_put_AppSpecific(This,lAppSpecific)	\
    (This)->lpVtbl -> put_AppSpecific(This,lAppSpecific)

#define IMSMQMessage2_get_SourceMachineGuid(This,pbstrGuidSrcMachine)	\
    (This)->lpVtbl -> get_SourceMachineGuid(This,pbstrGuidSrcMachine)

#define IMSMQMessage2_get_BodyLength(This,pcbBody)	\
    (This)->lpVtbl -> get_BodyLength(This,pcbBody)

#define IMSMQMessage2_get_Body(This,pvarBody)	\
    (This)->lpVtbl -> get_Body(This,pvarBody)

#define IMSMQMessage2_put_Body(This,varBody)	\
    (This)->lpVtbl -> put_Body(This,varBody)

#define IMSMQMessage2_get_AdminQueueInfo_v1(This,ppqinfoAdmin)	\
    (This)->lpVtbl -> get_AdminQueueInfo_v1(This,ppqinfoAdmin)

#define IMSMQMessage2_putref_AdminQueueInfo_v1(This,pqinfoAdmin)	\
    (This)->lpVtbl -> putref_AdminQueueInfo_v1(This,pqinfoAdmin)

#define IMSMQMessage2_get_Id(This,pvarMsgId)	\
    (This)->lpVtbl -> get_Id(This,pvarMsgId)

#define IMSMQMessage2_get_CorrelationId(This,pvarMsgId)	\
    (This)->lpVtbl -> get_CorrelationId(This,pvarMsgId)

#define IMSMQMessage2_put_CorrelationId(This,varMsgId)	\
    (This)->lpVtbl -> put_CorrelationId(This,varMsgId)

#define IMSMQMessage2_get_Ack(This,plAck)	\
    (This)->lpVtbl -> get_Ack(This,plAck)

#define IMSMQMessage2_put_Ack(This,lAck)	\
    (This)->lpVtbl -> put_Ack(This,lAck)

#define IMSMQMessage2_get_Label(This,pbstrLabel)	\
    (This)->lpVtbl -> get_Label(This,pbstrLabel)

#define IMSMQMessage2_put_Label(This,bstrLabel)	\
    (This)->lpVtbl -> put_Label(This,bstrLabel)

#define IMSMQMessage2_get_MaxTimeToReachQueue(This,plMaxTimeToReachQueue)	\
    (This)->lpVtbl -> get_MaxTimeToReachQueue(This,plMaxTimeToReachQueue)

#define IMSMQMessage2_put_MaxTimeToReachQueue(This,lMaxTimeToReachQueue)	\
    (This)->lpVtbl -> put_MaxTimeToReachQueue(This,lMaxTimeToReachQueue)

#define IMSMQMessage2_get_MaxTimeToReceive(This,plMaxTimeToReceive)	\
    (This)->lpVtbl -> get_MaxTimeToReceive(This,plMaxTimeToReceive)

#define IMSMQMessage2_put_MaxTimeToReceive(This,lMaxTimeToReceive)	\
    (This)->lpVtbl -> put_MaxTimeToReceive(This,lMaxTimeToReceive)

#define IMSMQMessage2_get_HashAlgorithm(This,plHashAlg)	\
    (This)->lpVtbl -> get_HashAlgorithm(This,plHashAlg)

#define IMSMQMessage2_put_HashAlgorithm(This,lHashAlg)	\
    (This)->lpVtbl -> put_HashAlgorithm(This,lHashAlg)

#define IMSMQMessage2_get_EncryptAlgorithm(This,plEncryptAlg)	\
    (This)->lpVtbl -> get_EncryptAlgorithm(This,plEncryptAlg)

#define IMSMQMessage2_put_EncryptAlgorithm(This,lEncryptAlg)	\
    (This)->lpVtbl -> put_EncryptAlgorithm(This,lEncryptAlg)

#define IMSMQMessage2_get_SentTime(This,pvarSentTime)	\
    (This)->lpVtbl -> get_SentTime(This,pvarSentTime)

#define IMSMQMessage2_get_ArrivedTime(This,plArrivedTime)	\
    (This)->lpVtbl -> get_ArrivedTime(This,plArrivedTime)

#define IMSMQMessage2_get_DestinationQueueInfo(This,ppqinfoDest)	\
    (This)->lpVtbl -> get_DestinationQueueInfo(This,ppqinfoDest)

#define IMSMQMessage2_get_SenderCertificate(This,pvarSenderCert)	\
    (This)->lpVtbl -> get_SenderCertificate(This,pvarSenderCert)

#define IMSMQMessage2_put_SenderCertificate(This,varSenderCert)	\
    (This)->lpVtbl -> put_SenderCertificate(This,varSenderCert)

#define IMSMQMessage2_get_SenderId(This,pvarSenderId)	\
    (This)->lpVtbl -> get_SenderId(This,pvarSenderId)

#define IMSMQMessage2_get_SenderIdType(This,plSenderIdType)	\
    (This)->lpVtbl -> get_SenderIdType(This,plSenderIdType)

#define IMSMQMessage2_put_SenderIdType(This,lSenderIdType)	\
    (This)->lpVtbl -> put_SenderIdType(This,lSenderIdType)

#define IMSMQMessage2_Send(This,DestinationQueue,Transaction)	\
    (This)->lpVtbl -> Send(This,DestinationQueue,Transaction)

#define IMSMQMessage2_AttachCurrentSecurityContext(This)	\
    (This)->lpVtbl -> AttachCurrentSecurityContext(This)

#define IMSMQMessage2_get_SenderVersion(This,plSenderVersion)	\
    (This)->lpVtbl -> get_SenderVersion(This,plSenderVersion)

#define IMSMQMessage2_get_Extension(This,pvarExtension)	\
    (This)->lpVtbl -> get_Extension(This,pvarExtension)

#define IMSMQMessage2_put_Extension(This,varExtension)	\
    (This)->lpVtbl -> put_Extension(This,varExtension)

#define IMSMQMessage2_get_ConnectorTypeGuid(This,pbstrGuidConnectorType)	\
    (This)->lpVtbl -> get_ConnectorTypeGuid(This,pbstrGuidConnectorType)

#define IMSMQMessage2_put_ConnectorTypeGuid(This,bstrGuidConnectorType)	\
    (This)->lpVtbl -> put_ConnectorTypeGuid(This,bstrGuidConnectorType)

#define IMSMQMessage2_get_TransactionStatusQueueInfo(This,ppqinfoXactStatus)	\
    (This)->lpVtbl -> get_TransactionStatusQueueInfo(This,ppqinfoXactStatus)

#define IMSMQMessage2_get_DestinationSymmetricKey(This,pvarDestSymmKey)	\
    (This)->lpVtbl -> get_DestinationSymmetricKey(This,pvarDestSymmKey)

#define IMSMQMessage2_put_DestinationSymmetricKey(This,varDestSymmKey)	\
    (This)->lpVtbl -> put_DestinationSymmetricKey(This,varDestSymmKey)

#define IMSMQMessage2_get_Signature(This,pvarSignature)	\
    (This)->lpVtbl -> get_Signature(This,pvarSignature)

#define IMSMQMessage2_put_Signature(This,varSignature)	\
    (This)->lpVtbl -> put_Signature(This,varSignature)

#define IMSMQMessage2_get_AuthenticationProviderType(This,plAuthProvType)	\
    (This)->lpVtbl -> get_AuthenticationProviderType(This,plAuthProvType)

#define IMSMQMessage2_put_AuthenticationProviderType(This,lAuthProvType)	\
    (This)->lpVtbl -> put_AuthenticationProviderType(This,lAuthProvType)

#define IMSMQMessage2_get_AuthenticationProviderName(This,pbstrAuthProvName)	\
    (This)->lpVtbl -> get_AuthenticationProviderName(This,pbstrAuthProvName)

#define IMSMQMessage2_put_AuthenticationProviderName(This,bstrAuthProvName)	\
    (This)->lpVtbl -> put_AuthenticationProviderName(This,bstrAuthProvName)

#define IMSMQMessage2_put_SenderId(This,varSenderId)	\
    (This)->lpVtbl -> put_SenderId(This,varSenderId)

#define IMSMQMessage2_get_MsgClass(This,plMsgClass)	\
    (This)->lpVtbl -> get_MsgClass(This,plMsgClass)

#define IMSMQMessage2_put_MsgClass(This,lMsgClass)	\
    (This)->lpVtbl -> put_MsgClass(This,lMsgClass)

#define IMSMQMessage2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#define IMSMQMessage2_get_TransactionId(This,pvarXactId)	\
    (This)->lpVtbl -> get_TransactionId(This,pvarXactId)

#define IMSMQMessage2_get_IsFirstInTransaction(This,pisFirstInXact)	\
    (This)->lpVtbl -> get_IsFirstInTransaction(This,pisFirstInXact)

#define IMSMQMessage2_get_IsLastInTransaction(This,pisLastInXact)	\
    (This)->lpVtbl -> get_IsLastInTransaction(This,pisLastInXact)

#define IMSMQMessage2_get_ResponseQueueInfo(This,ppqinfoResponse)	\
    (This)->lpVtbl -> get_ResponseQueueInfo(This,ppqinfoResponse)

#define IMSMQMessage2_putref_ResponseQueueInfo(This,pqinfoResponse)	\
    (This)->lpVtbl -> putref_ResponseQueueInfo(This,pqinfoResponse)

#define IMSMQMessage2_get_AdminQueueInfo(This,ppqinfoAdmin)	\
    (This)->lpVtbl -> get_AdminQueueInfo(This,ppqinfoAdmin)

#define IMSMQMessage2_putref_AdminQueueInfo(This,pqinfoAdmin)	\
    (This)->lpVtbl -> putref_AdminQueueInfo(This,pqinfoAdmin)

#define IMSMQMessage2_get_ReceivedAuthenticationLevel(This,psReceivedAuthenticationLevel)	\
    (This)->lpVtbl -> get_ReceivedAuthenticationLevel(This,psReceivedAuthenticationLevel)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Class_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plClass);


void __RPC_STUB IMSMQMessage2_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_PrivLevel_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPrivLevel);


void __RPC_STUB IMSMQMessage2_get_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_PrivLevel_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lPrivLevel);


void __RPC_STUB IMSMQMessage2_put_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_AuthLevel_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthLevel);


void __RPC_STUB IMSMQMessage2_get_AuthLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_AuthLevel_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lAuthLevel);


void __RPC_STUB IMSMQMessage2_put_AuthLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_IsAuthenticated_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated);


void __RPC_STUB IMSMQMessage2_get_IsAuthenticated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Delivery_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plDelivery);


void __RPC_STUB IMSMQMessage2_get_Delivery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Delivery_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lDelivery);


void __RPC_STUB IMSMQMessage2_put_Delivery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Trace_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plTrace);


void __RPC_STUB IMSMQMessage2_get_Trace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Trace_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lTrace);


void __RPC_STUB IMSMQMessage2_put_Trace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Priority_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPriority);


void __RPC_STUB IMSMQMessage2_get_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Priority_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lPriority);


void __RPC_STUB IMSMQMessage2_put_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Journal_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournal);


void __RPC_STUB IMSMQMessage2_get_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Journal_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lJournal);


void __RPC_STUB IMSMQMessage2_put_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_ResponseQueueInfo_v1_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse);


void __RPC_STUB IMSMQMessage2_get_ResponseQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_putref_ResponseQueueInfo_v1_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse);


void __RPC_STUB IMSMQMessage2_putref_ResponseQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_AppSpecific_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAppSpecific);


void __RPC_STUB IMSMQMessage2_get_AppSpecific_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_AppSpecific_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lAppSpecific);


void __RPC_STUB IMSMQMessage2_put_AppSpecific_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_SourceMachineGuid_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine);


void __RPC_STUB IMSMQMessage2_get_SourceMachineGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_BodyLength_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pcbBody);


void __RPC_STUB IMSMQMessage2_get_BodyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Body_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBody);


void __RPC_STUB IMSMQMessage2_get_Body_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Body_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varBody);


void __RPC_STUB IMSMQMessage2_put_Body_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_AdminQueueInfo_v1_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin);


void __RPC_STUB IMSMQMessage2_get_AdminQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_putref_AdminQueueInfo_v1_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin);


void __RPC_STUB IMSMQMessage2_putref_AdminQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Id_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);


void __RPC_STUB IMSMQMessage2_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_CorrelationId_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);


void __RPC_STUB IMSMQMessage2_get_CorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_CorrelationId_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varMsgId);


void __RPC_STUB IMSMQMessage2_put_CorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Ack_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAck);


void __RPC_STUB IMSMQMessage2_get_Ack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Ack_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lAck);


void __RPC_STUB IMSMQMessage2_put_Ack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Label_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);


void __RPC_STUB IMSMQMessage2_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Label_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ BSTR bstrLabel);


void __RPC_STUB IMSMQMessage2_put_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_MaxTimeToReachQueue_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue);


void __RPC_STUB IMSMQMessage2_get_MaxTimeToReachQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_MaxTimeToReachQueue_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lMaxTimeToReachQueue);


void __RPC_STUB IMSMQMessage2_put_MaxTimeToReachQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_MaxTimeToReceive_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive);


void __RPC_STUB IMSMQMessage2_get_MaxTimeToReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_MaxTimeToReceive_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lMaxTimeToReceive);


void __RPC_STUB IMSMQMessage2_put_MaxTimeToReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_HashAlgorithm_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plHashAlg);


void __RPC_STUB IMSMQMessage2_get_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_HashAlgorithm_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lHashAlg);


void __RPC_STUB IMSMQMessage2_put_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_EncryptAlgorithm_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plEncryptAlg);


void __RPC_STUB IMSMQMessage2_get_EncryptAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_EncryptAlgorithm_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lEncryptAlg);


void __RPC_STUB IMSMQMessage2_put_EncryptAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_SentTime_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime);


void __RPC_STUB IMSMQMessage2_get_SentTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_ArrivedTime_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime);


void __RPC_STUB IMSMQMessage2_get_ArrivedTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_DestinationQueueInfo_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoDest);


void __RPC_STUB IMSMQMessage2_get_DestinationQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_SenderCertificate_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert);


void __RPC_STUB IMSMQMessage2_get_SenderCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_SenderCertificate_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varSenderCert);


void __RPC_STUB IMSMQMessage2_put_SenderCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_SenderId_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId);


void __RPC_STUB IMSMQMessage2_get_SenderId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_SenderIdType_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSenderIdType);


void __RPC_STUB IMSMQMessage2_get_SenderIdType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_SenderIdType_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lSenderIdType);


void __RPC_STUB IMSMQMessage2_put_SenderIdType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_Send_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ IMSMQQueue2 __RPC_FAR *DestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction);


void __RPC_STUB IMSMQMessage2_Send_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_AttachCurrentSecurityContext_Proxy( 
    IMSMQMessage2 __RPC_FAR * This);


void __RPC_STUB IMSMQMessage2_AttachCurrentSecurityContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_SenderVersion_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSenderVersion);


void __RPC_STUB IMSMQMessage2_get_SenderVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Extension_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarExtension);


void __RPC_STUB IMSMQMessage2_get_Extension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Extension_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varExtension);


void __RPC_STUB IMSMQMessage2_put_Extension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_ConnectorTypeGuid_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidConnectorType);


void __RPC_STUB IMSMQMessage2_get_ConnectorTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_ConnectorTypeGuid_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ BSTR bstrGuidConnectorType);


void __RPC_STUB IMSMQMessage2_put_ConnectorTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_TransactionStatusQueueInfo_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoXactStatus);


void __RPC_STUB IMSMQMessage2_get_TransactionStatusQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_DestinationSymmetricKey_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarDestSymmKey);


void __RPC_STUB IMSMQMessage2_get_DestinationSymmetricKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_DestinationSymmetricKey_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varDestSymmKey);


void __RPC_STUB IMSMQMessage2_put_DestinationSymmetricKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Signature_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSignature);


void __RPC_STUB IMSMQMessage2_get_Signature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_Signature_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varSignature);


void __RPC_STUB IMSMQMessage2_put_Signature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_AuthenticationProviderType_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthProvType);


void __RPC_STUB IMSMQMessage2_get_AuthenticationProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_AuthenticationProviderType_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lAuthProvType);


void __RPC_STUB IMSMQMessage2_put_AuthenticationProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_AuthenticationProviderName_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthProvName);


void __RPC_STUB IMSMQMessage2_get_AuthenticationProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_AuthenticationProviderName_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ BSTR bstrAuthProvName);


void __RPC_STUB IMSMQMessage2_put_AuthenticationProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_SenderId_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ VARIANT varSenderId);


void __RPC_STUB IMSMQMessage2_put_SenderId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_MsgClass_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMsgClass);


void __RPC_STUB IMSMQMessage2_get_MsgClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_put_MsgClass_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ long lMsgClass);


void __RPC_STUB IMSMQMessage2_put_MsgClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_Properties_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQMessage2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_TransactionId_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarXactId);


void __RPC_STUB IMSMQMessage2_get_TransactionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_IsFirstInTransaction_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisFirstInXact);


void __RPC_STUB IMSMQMessage2_get_IsFirstInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_IsLastInTransaction_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisLastInXact);


void __RPC_STUB IMSMQMessage2_get_IsLastInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_ResponseQueueInfo_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoResponse);


void __RPC_STUB IMSMQMessage2_get_ResponseQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_putref_ResponseQueueInfo_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoResponse);


void __RPC_STUB IMSMQMessage2_putref_ResponseQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_AdminQueueInfo_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoAdmin);


void __RPC_STUB IMSMQMessage2_get_AdminQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_putref_AdminQueueInfo_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoAdmin);


void __RPC_STUB IMSMQMessage2_putref_AdminQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage2_get_ReceivedAuthenticationLevel_Proxy( 
    IMSMQMessage2 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *psReceivedAuthenticationLevel);


void __RPC_STUB IMSMQMessage2_get_ReceivedAuthenticationLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQMessage2_INTERFACE_DEFINED__ */


#ifndef __IMSMQMessage3_INTERFACE_DEFINED__
#define __IMSMQMessage3_INTERFACE_DEFINED__

/* interface IMSMQMessage3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQMessage3,0xeba96b1a,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b1a-2168-11d3-898c-00e02c074f6b")
    IMSMQMessage3 : public IDispatch
    {
    public:
        virtual /* [id][propget][hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Class( 
            /* [retval][out] */ long __RPC_FAR *plClass) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_PrivLevel( 
            /* [retval][out] */ long __RPC_FAR *plPrivLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_PrivLevel( 
            /* [in] */ long lPrivLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthLevel( 
            /* [retval][out] */ long __RPC_FAR *plAuthLevel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthLevel( 
            /* [in] */ long lAuthLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsAuthenticated( 
            /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Delivery( 
            /* [retval][out] */ long __RPC_FAR *plDelivery) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Delivery( 
            /* [in] */ long lDelivery) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Trace( 
            /* [retval][out] */ long __RPC_FAR *plTrace) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Trace( 
            /* [in] */ long lTrace) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Priority( 
            /* [retval][out] */ long __RPC_FAR *plPriority) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Priority( 
            /* [in] */ long lPriority) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Journal( 
            /* [retval][out] */ long __RPC_FAR *plJournal) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Journal( 
            /* [in] */ long lJournal) = 0;
        
        virtual /* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseQueueInfo_v1( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse) = 0;
        
        virtual /* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseQueueInfo_v1( 
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AppSpecific( 
            /* [retval][out] */ long __RPC_FAR *plAppSpecific) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AppSpecific( 
            /* [in] */ long lAppSpecific) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SourceMachineGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_BodyLength( 
            /* [retval][out] */ long __RPC_FAR *pcbBody) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Body( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBody) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Body( 
            /* [in] */ VARIANT varBody) = 0;
        
        virtual /* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminQueueInfo_v1( 
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin) = 0;
        
        virtual /* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminQueueInfo_v1( 
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_CorrelationId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_CorrelationId( 
            /* [in] */ VARIANT varMsgId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Ack( 
            /* [retval][out] */ long __RPC_FAR *plAck) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Ack( 
            /* [in] */ long lAck) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Label( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Label( 
            /* [in] */ BSTR bstrLabel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MaxTimeToReachQueue( 
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MaxTimeToReachQueue( 
            /* [in] */ long lMaxTimeToReachQueue) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MaxTimeToReceive( 
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MaxTimeToReceive( 
            /* [in] */ long lMaxTimeToReceive) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_HashAlgorithm( 
            /* [retval][out] */ long __RPC_FAR *plHashAlg) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_HashAlgorithm( 
            /* [in] */ long lHashAlg) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_EncryptAlgorithm( 
            /* [retval][out] */ long __RPC_FAR *plEncryptAlg) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_EncryptAlgorithm( 
            /* [in] */ long lEncryptAlg) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SentTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ArrivedTime( 
            /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_DestinationQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoDest) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderCertificate( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderCertificate( 
            /* [in] */ VARIANT varSenderCert) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderIdType( 
            /* [retval][out] */ long __RPC_FAR *plSenderIdType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderIdType( 
            /* [in] */ long lSenderIdType) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Send( 
            /* [in] */ IDispatch __RPC_FAR *DestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE AttachCurrentSecurityContext( void) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_SenderVersion( 
            /* [retval][out] */ long __RPC_FAR *plSenderVersion) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Extension( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarExtension) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Extension( 
            /* [in] */ VARIANT varExtension) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ConnectorTypeGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidConnectorType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_ConnectorTypeGuid( 
            /* [in] */ BSTR bstrGuidConnectorType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_TransactionStatusQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoXactStatus) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_DestinationSymmetricKey( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarDestSymmKey) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_DestinationSymmetricKey( 
            /* [in] */ VARIANT varDestSymmKey) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Signature( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSignature) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_Signature( 
            /* [in] */ VARIANT varSignature) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthenticationProviderType( 
            /* [retval][out] */ long __RPC_FAR *plAuthProvType) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthenticationProviderType( 
            /* [in] */ long lAuthProvType) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AuthenticationProviderName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthProvName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_AuthenticationProviderName( 
            /* [in] */ BSTR bstrAuthProvName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_SenderId( 
            /* [in] */ VARIANT varSenderId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MsgClass( 
            /* [retval][out] */ long __RPC_FAR *plMsgClass) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_MsgClass( 
            /* [in] */ long lMsgClass) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_TransactionId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarXactId) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsFirstInTransaction( 
            /* [retval][out] */ Boolean __RPC_FAR *pisFirstInXact) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsLastInTransaction( 
            /* [retval][out] */ Boolean __RPC_FAR *pisLastInXact) = 0;
        
        virtual /* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseQueueInfo_v2( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoResponse) = 0;
        
        virtual /* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseQueueInfo_v2( 
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoResponse) = 0;
        
        virtual /* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminQueueInfo_v2( 
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoAdmin) = 0;
        
        virtual /* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminQueueInfo_v2( 
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ReceivedAuthenticationLevel( 
            /* [retval][out] */ short __RPC_FAR *psReceivedAuthenticationLevel) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoResponse) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseQueueInfo( 
            /* [in] */ IMSMQQueueInfo3 __RPC_FAR *pqinfoResponse) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminQueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoAdmin) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminQueueInfo( 
            /* [in] */ IMSMQQueueInfo3 __RPC_FAR *pqinfoAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ResponseDestination( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestResponse) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_ResponseDestination( 
            /* [in] */ IDispatch __RPC_FAR *pdestResponse) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_AdminDestination( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestAdmin) = 0;
        
        virtual /* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE putref_AdminDestination( 
            /* [in] */ IDispatch __RPC_FAR *pdestAdmin) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Destination( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestDestination) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_LookupId( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarLookupId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQMessage3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQMessage3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQMessage3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Class )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plClass);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PrivLevel )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPrivLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PrivLevel )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lPrivLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthLevel )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthLevel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthLevel )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lAuthLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsAuthenticated )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Delivery )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plDelivery);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Delivery )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lDelivery);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Trace )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plTrace);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Trace )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lTrace);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Priority )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plPriority);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Priority )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lPriority);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Journal )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plJournal);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Journal )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lJournal);
        
        /* [hidden][id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseQueueInfo_v1 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse);
        
        /* [hidden][id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseQueueInfo_v1 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AppSpecific )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAppSpecific);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AppSpecific )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lAppSpecific);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceMachineGuid )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BodyLength )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pcbBody);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Body )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarBody);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Body )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varBody);
        
        /* [hidden][id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminQueueInfo_v1 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin);
        
        /* [hidden][id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminQueueInfo_v1 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Id )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CorrelationId )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CorrelationId )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varMsgId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Ack )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAck);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Ack )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lAck);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Label )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Label )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ BSTR bstrLabel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxTimeToReachQueue )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxTimeToReachQueue )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lMaxTimeToReachQueue);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxTimeToReceive )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxTimeToReceive )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lMaxTimeToReceive);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HashAlgorithm )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plHashAlg);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HashAlgorithm )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lHashAlg);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EncryptAlgorithm )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plEncryptAlg);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EncryptAlgorithm )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lEncryptAlg);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SentTime )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ArrivedTime )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationQueueInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoDest);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderCertificate )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderCertificate )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varSenderCert);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderId )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderIdType )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSenderIdType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderIdType )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lSenderIdType);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Send )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *DestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AttachCurrentSecurityContext )( 
            IMSMQMessage3 __RPC_FAR * This);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SenderVersion )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plSenderVersion);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Extension )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarExtension);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Extension )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varExtension);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectorTypeGuid )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidConnectorType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectorTypeGuid )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ BSTR bstrGuidConnectorType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransactionStatusQueueInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoXactStatus);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestinationSymmetricKey )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarDestSymmKey);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestinationSymmetricKey )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varDestSymmKey);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Signature )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarSignature);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Signature )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varSignature);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthenticationProviderType )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAuthProvType);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthenticationProviderType )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lAuthProvType);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AuthenticationProviderName )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthProvName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AuthenticationProviderName )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ BSTR bstrAuthProvName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SenderId )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ VARIANT varSenderId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MsgClass )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMsgClass);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MsgClass )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ long lMsgClass);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransactionId )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarXactId);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsFirstInTransaction )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisFirstInXact);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsLastInTransaction )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisLastInXact);
        
        /* [hidden][id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseQueueInfo_v2 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoResponse);
        
        /* [hidden][id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseQueueInfo_v2 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoResponse);
        
        /* [hidden][id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminQueueInfo_v2 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoAdmin);
        
        /* [hidden][id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminQueueInfo_v2 )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReceivedAuthenticationLevel )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psReceivedAuthenticationLevel);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseQueueInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoResponse);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseQueueInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo3 __RPC_FAR *pqinfoResponse);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminQueueInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoAdmin);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminQueueInfo )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IMSMQQueueInfo3 __RPC_FAR *pqinfoAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ResponseDestination )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestResponse);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_ResponseDestination )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pdestResponse);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AdminDestination )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestAdmin);
        
        /* [id][propputref][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_AdminDestination )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pdestAdmin);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Destination )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestDestination);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LookupId )( 
            IMSMQMessage3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarLookupId);
        
        END_INTERFACE
    } IMSMQMessage3Vtbl;

    interface IMSMQMessage3
    {
        CONST_VTBL struct IMSMQMessage3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQMessage3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQMessage3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQMessage3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQMessage3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQMessage3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQMessage3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQMessage3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQMessage3_get_Class(This,plClass)	\
    (This)->lpVtbl -> get_Class(This,plClass)

#define IMSMQMessage3_get_PrivLevel(This,plPrivLevel)	\
    (This)->lpVtbl -> get_PrivLevel(This,plPrivLevel)

#define IMSMQMessage3_put_PrivLevel(This,lPrivLevel)	\
    (This)->lpVtbl -> put_PrivLevel(This,lPrivLevel)

#define IMSMQMessage3_get_AuthLevel(This,plAuthLevel)	\
    (This)->lpVtbl -> get_AuthLevel(This,plAuthLevel)

#define IMSMQMessage3_put_AuthLevel(This,lAuthLevel)	\
    (This)->lpVtbl -> put_AuthLevel(This,lAuthLevel)

#define IMSMQMessage3_get_IsAuthenticated(This,pisAuthenticated)	\
    (This)->lpVtbl -> get_IsAuthenticated(This,pisAuthenticated)

#define IMSMQMessage3_get_Delivery(This,plDelivery)	\
    (This)->lpVtbl -> get_Delivery(This,plDelivery)

#define IMSMQMessage3_put_Delivery(This,lDelivery)	\
    (This)->lpVtbl -> put_Delivery(This,lDelivery)

#define IMSMQMessage3_get_Trace(This,plTrace)	\
    (This)->lpVtbl -> get_Trace(This,plTrace)

#define IMSMQMessage3_put_Trace(This,lTrace)	\
    (This)->lpVtbl -> put_Trace(This,lTrace)

#define IMSMQMessage3_get_Priority(This,plPriority)	\
    (This)->lpVtbl -> get_Priority(This,plPriority)

#define IMSMQMessage3_put_Priority(This,lPriority)	\
    (This)->lpVtbl -> put_Priority(This,lPriority)

#define IMSMQMessage3_get_Journal(This,plJournal)	\
    (This)->lpVtbl -> get_Journal(This,plJournal)

#define IMSMQMessage3_put_Journal(This,lJournal)	\
    (This)->lpVtbl -> put_Journal(This,lJournal)

#define IMSMQMessage3_get_ResponseQueueInfo_v1(This,ppqinfoResponse)	\
    (This)->lpVtbl -> get_ResponseQueueInfo_v1(This,ppqinfoResponse)

#define IMSMQMessage3_putref_ResponseQueueInfo_v1(This,pqinfoResponse)	\
    (This)->lpVtbl -> putref_ResponseQueueInfo_v1(This,pqinfoResponse)

#define IMSMQMessage3_get_AppSpecific(This,plAppSpecific)	\
    (This)->lpVtbl -> get_AppSpecific(This,plAppSpecific)

#define IMSMQMessage3_put_AppSpecific(This,lAppSpecific)	\
    (This)->lpVtbl -> put_AppSpecific(This,lAppSpecific)

#define IMSMQMessage3_get_SourceMachineGuid(This,pbstrGuidSrcMachine)	\
    (This)->lpVtbl -> get_SourceMachineGuid(This,pbstrGuidSrcMachine)

#define IMSMQMessage3_get_BodyLength(This,pcbBody)	\
    (This)->lpVtbl -> get_BodyLength(This,pcbBody)

#define IMSMQMessage3_get_Body(This,pvarBody)	\
    (This)->lpVtbl -> get_Body(This,pvarBody)

#define IMSMQMessage3_put_Body(This,varBody)	\
    (This)->lpVtbl -> put_Body(This,varBody)

#define IMSMQMessage3_get_AdminQueueInfo_v1(This,ppqinfoAdmin)	\
    (This)->lpVtbl -> get_AdminQueueInfo_v1(This,ppqinfoAdmin)

#define IMSMQMessage3_putref_AdminQueueInfo_v1(This,pqinfoAdmin)	\
    (This)->lpVtbl -> putref_AdminQueueInfo_v1(This,pqinfoAdmin)

#define IMSMQMessage3_get_Id(This,pvarMsgId)	\
    (This)->lpVtbl -> get_Id(This,pvarMsgId)

#define IMSMQMessage3_get_CorrelationId(This,pvarMsgId)	\
    (This)->lpVtbl -> get_CorrelationId(This,pvarMsgId)

#define IMSMQMessage3_put_CorrelationId(This,varMsgId)	\
    (This)->lpVtbl -> put_CorrelationId(This,varMsgId)

#define IMSMQMessage3_get_Ack(This,plAck)	\
    (This)->lpVtbl -> get_Ack(This,plAck)

#define IMSMQMessage3_put_Ack(This,lAck)	\
    (This)->lpVtbl -> put_Ack(This,lAck)

#define IMSMQMessage3_get_Label(This,pbstrLabel)	\
    (This)->lpVtbl -> get_Label(This,pbstrLabel)

#define IMSMQMessage3_put_Label(This,bstrLabel)	\
    (This)->lpVtbl -> put_Label(This,bstrLabel)

#define IMSMQMessage3_get_MaxTimeToReachQueue(This,plMaxTimeToReachQueue)	\
    (This)->lpVtbl -> get_MaxTimeToReachQueue(This,plMaxTimeToReachQueue)

#define IMSMQMessage3_put_MaxTimeToReachQueue(This,lMaxTimeToReachQueue)	\
    (This)->lpVtbl -> put_MaxTimeToReachQueue(This,lMaxTimeToReachQueue)

#define IMSMQMessage3_get_MaxTimeToReceive(This,plMaxTimeToReceive)	\
    (This)->lpVtbl -> get_MaxTimeToReceive(This,plMaxTimeToReceive)

#define IMSMQMessage3_put_MaxTimeToReceive(This,lMaxTimeToReceive)	\
    (This)->lpVtbl -> put_MaxTimeToReceive(This,lMaxTimeToReceive)

#define IMSMQMessage3_get_HashAlgorithm(This,plHashAlg)	\
    (This)->lpVtbl -> get_HashAlgorithm(This,plHashAlg)

#define IMSMQMessage3_put_HashAlgorithm(This,lHashAlg)	\
    (This)->lpVtbl -> put_HashAlgorithm(This,lHashAlg)

#define IMSMQMessage3_get_EncryptAlgorithm(This,plEncryptAlg)	\
    (This)->lpVtbl -> get_EncryptAlgorithm(This,plEncryptAlg)

#define IMSMQMessage3_put_EncryptAlgorithm(This,lEncryptAlg)	\
    (This)->lpVtbl -> put_EncryptAlgorithm(This,lEncryptAlg)

#define IMSMQMessage3_get_SentTime(This,pvarSentTime)	\
    (This)->lpVtbl -> get_SentTime(This,pvarSentTime)

#define IMSMQMessage3_get_ArrivedTime(This,plArrivedTime)	\
    (This)->lpVtbl -> get_ArrivedTime(This,plArrivedTime)

#define IMSMQMessage3_get_DestinationQueueInfo(This,ppqinfoDest)	\
    (This)->lpVtbl -> get_DestinationQueueInfo(This,ppqinfoDest)

#define IMSMQMessage3_get_SenderCertificate(This,pvarSenderCert)	\
    (This)->lpVtbl -> get_SenderCertificate(This,pvarSenderCert)

#define IMSMQMessage3_put_SenderCertificate(This,varSenderCert)	\
    (This)->lpVtbl -> put_SenderCertificate(This,varSenderCert)

#define IMSMQMessage3_get_SenderId(This,pvarSenderId)	\
    (This)->lpVtbl -> get_SenderId(This,pvarSenderId)

#define IMSMQMessage3_get_SenderIdType(This,plSenderIdType)	\
    (This)->lpVtbl -> get_SenderIdType(This,plSenderIdType)

#define IMSMQMessage3_put_SenderIdType(This,lSenderIdType)	\
    (This)->lpVtbl -> put_SenderIdType(This,lSenderIdType)

#define IMSMQMessage3_Send(This,DestinationQueue,Transaction)	\
    (This)->lpVtbl -> Send(This,DestinationQueue,Transaction)

#define IMSMQMessage3_AttachCurrentSecurityContext(This)	\
    (This)->lpVtbl -> AttachCurrentSecurityContext(This)

#define IMSMQMessage3_get_SenderVersion(This,plSenderVersion)	\
    (This)->lpVtbl -> get_SenderVersion(This,plSenderVersion)

#define IMSMQMessage3_get_Extension(This,pvarExtension)	\
    (This)->lpVtbl -> get_Extension(This,pvarExtension)

#define IMSMQMessage3_put_Extension(This,varExtension)	\
    (This)->lpVtbl -> put_Extension(This,varExtension)

#define IMSMQMessage3_get_ConnectorTypeGuid(This,pbstrGuidConnectorType)	\
    (This)->lpVtbl -> get_ConnectorTypeGuid(This,pbstrGuidConnectorType)

#define IMSMQMessage3_put_ConnectorTypeGuid(This,bstrGuidConnectorType)	\
    (This)->lpVtbl -> put_ConnectorTypeGuid(This,bstrGuidConnectorType)

#define IMSMQMessage3_get_TransactionStatusQueueInfo(This,ppqinfoXactStatus)	\
    (This)->lpVtbl -> get_TransactionStatusQueueInfo(This,ppqinfoXactStatus)

#define IMSMQMessage3_get_DestinationSymmetricKey(This,pvarDestSymmKey)	\
    (This)->lpVtbl -> get_DestinationSymmetricKey(This,pvarDestSymmKey)

#define IMSMQMessage3_put_DestinationSymmetricKey(This,varDestSymmKey)	\
    (This)->lpVtbl -> put_DestinationSymmetricKey(This,varDestSymmKey)

#define IMSMQMessage3_get_Signature(This,pvarSignature)	\
    (This)->lpVtbl -> get_Signature(This,pvarSignature)

#define IMSMQMessage3_put_Signature(This,varSignature)	\
    (This)->lpVtbl -> put_Signature(This,varSignature)

#define IMSMQMessage3_get_AuthenticationProviderType(This,plAuthProvType)	\
    (This)->lpVtbl -> get_AuthenticationProviderType(This,plAuthProvType)

#define IMSMQMessage3_put_AuthenticationProviderType(This,lAuthProvType)	\
    (This)->lpVtbl -> put_AuthenticationProviderType(This,lAuthProvType)

#define IMSMQMessage3_get_AuthenticationProviderName(This,pbstrAuthProvName)	\
    (This)->lpVtbl -> get_AuthenticationProviderName(This,pbstrAuthProvName)

#define IMSMQMessage3_put_AuthenticationProviderName(This,bstrAuthProvName)	\
    (This)->lpVtbl -> put_AuthenticationProviderName(This,bstrAuthProvName)

#define IMSMQMessage3_put_SenderId(This,varSenderId)	\
    (This)->lpVtbl -> put_SenderId(This,varSenderId)

#define IMSMQMessage3_get_MsgClass(This,plMsgClass)	\
    (This)->lpVtbl -> get_MsgClass(This,plMsgClass)

#define IMSMQMessage3_put_MsgClass(This,lMsgClass)	\
    (This)->lpVtbl -> put_MsgClass(This,lMsgClass)

#define IMSMQMessage3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#define IMSMQMessage3_get_TransactionId(This,pvarXactId)	\
    (This)->lpVtbl -> get_TransactionId(This,pvarXactId)

#define IMSMQMessage3_get_IsFirstInTransaction(This,pisFirstInXact)	\
    (This)->lpVtbl -> get_IsFirstInTransaction(This,pisFirstInXact)

#define IMSMQMessage3_get_IsLastInTransaction(This,pisLastInXact)	\
    (This)->lpVtbl -> get_IsLastInTransaction(This,pisLastInXact)

#define IMSMQMessage3_get_ResponseQueueInfo_v2(This,ppqinfoResponse)	\
    (This)->lpVtbl -> get_ResponseQueueInfo_v2(This,ppqinfoResponse)

#define IMSMQMessage3_putref_ResponseQueueInfo_v2(This,pqinfoResponse)	\
    (This)->lpVtbl -> putref_ResponseQueueInfo_v2(This,pqinfoResponse)

#define IMSMQMessage3_get_AdminQueueInfo_v2(This,ppqinfoAdmin)	\
    (This)->lpVtbl -> get_AdminQueueInfo_v2(This,ppqinfoAdmin)

#define IMSMQMessage3_putref_AdminQueueInfo_v2(This,pqinfoAdmin)	\
    (This)->lpVtbl -> putref_AdminQueueInfo_v2(This,pqinfoAdmin)

#define IMSMQMessage3_get_ReceivedAuthenticationLevel(This,psReceivedAuthenticationLevel)	\
    (This)->lpVtbl -> get_ReceivedAuthenticationLevel(This,psReceivedAuthenticationLevel)

#define IMSMQMessage3_get_ResponseQueueInfo(This,ppqinfoResponse)	\
    (This)->lpVtbl -> get_ResponseQueueInfo(This,ppqinfoResponse)

#define IMSMQMessage3_putref_ResponseQueueInfo(This,pqinfoResponse)	\
    (This)->lpVtbl -> putref_ResponseQueueInfo(This,pqinfoResponse)

#define IMSMQMessage3_get_AdminQueueInfo(This,ppqinfoAdmin)	\
    (This)->lpVtbl -> get_AdminQueueInfo(This,ppqinfoAdmin)

#define IMSMQMessage3_putref_AdminQueueInfo(This,pqinfoAdmin)	\
    (This)->lpVtbl -> putref_AdminQueueInfo(This,pqinfoAdmin)

#define IMSMQMessage3_get_ResponseDestination(This,ppdestResponse)	\
    (This)->lpVtbl -> get_ResponseDestination(This,ppdestResponse)

#define IMSMQMessage3_putref_ResponseDestination(This,pdestResponse)	\
    (This)->lpVtbl -> putref_ResponseDestination(This,pdestResponse)

#define IMSMQMessage3_get_AdminDestination(This,ppdestAdmin)	\
    (This)->lpVtbl -> get_AdminDestination(This,ppdestAdmin)

#define IMSMQMessage3_putref_AdminDestination(This,pdestAdmin)	\
    (This)->lpVtbl -> putref_AdminDestination(This,pdestAdmin)

#define IMSMQMessage3_get_Destination(This,ppdestDestination)	\
    (This)->lpVtbl -> get_Destination(This,ppdestDestination)

#define IMSMQMessage3_get_LookupId(This,pvarLookupId)	\
    (This)->lpVtbl -> get_LookupId(This,pvarLookupId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Class_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plClass);


void __RPC_STUB IMSMQMessage3_get_Class_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_PrivLevel_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPrivLevel);


void __RPC_STUB IMSMQMessage3_get_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_PrivLevel_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lPrivLevel);


void __RPC_STUB IMSMQMessage3_put_PrivLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AuthLevel_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthLevel);


void __RPC_STUB IMSMQMessage3_get_AuthLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_AuthLevel_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lAuthLevel);


void __RPC_STUB IMSMQMessage3_put_AuthLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_IsAuthenticated_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisAuthenticated);


void __RPC_STUB IMSMQMessage3_get_IsAuthenticated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Delivery_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plDelivery);


void __RPC_STUB IMSMQMessage3_get_Delivery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Delivery_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lDelivery);


void __RPC_STUB IMSMQMessage3_put_Delivery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Trace_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plTrace);


void __RPC_STUB IMSMQMessage3_get_Trace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Trace_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lTrace);


void __RPC_STUB IMSMQMessage3_put_Trace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Priority_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plPriority);


void __RPC_STUB IMSMQMessage3_get_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Priority_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lPriority);


void __RPC_STUB IMSMQMessage3_put_Priority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Journal_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plJournal);


void __RPC_STUB IMSMQMessage3_get_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Journal_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lJournal);


void __RPC_STUB IMSMQMessage3_put_Journal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ResponseQueueInfo_v1_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoResponse);


void __RPC_STUB IMSMQMessage3_get_ResponseQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_ResponseQueueInfo_v1_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoResponse);


void __RPC_STUB IMSMQMessage3_putref_ResponseQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AppSpecific_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAppSpecific);


void __RPC_STUB IMSMQMessage3_get_AppSpecific_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_AppSpecific_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lAppSpecific);


void __RPC_STUB IMSMQMessage3_put_AppSpecific_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_SourceMachineGuid_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidSrcMachine);


void __RPC_STUB IMSMQMessage3_get_SourceMachineGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_BodyLength_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pcbBody);


void __RPC_STUB IMSMQMessage3_get_BodyLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Body_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarBody);


void __RPC_STUB IMSMQMessage3_get_Body_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Body_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varBody);


void __RPC_STUB IMSMQMessage3_put_Body_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AdminQueueInfo_v1_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo __RPC_FAR *__RPC_FAR *ppqinfoAdmin);


void __RPC_STUB IMSMQMessage3_get_AdminQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_AdminQueueInfo_v1_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo __RPC_FAR *pqinfoAdmin);


void __RPC_STUB IMSMQMessage3_putref_AdminQueueInfo_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Id_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);


void __RPC_STUB IMSMQMessage3_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_CorrelationId_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarMsgId);


void __RPC_STUB IMSMQMessage3_get_CorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_CorrelationId_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varMsgId);


void __RPC_STUB IMSMQMessage3_put_CorrelationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Ack_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAck);


void __RPC_STUB IMSMQMessage3_get_Ack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Ack_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lAck);


void __RPC_STUB IMSMQMessage3_put_Ack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Label_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLabel);


void __RPC_STUB IMSMQMessage3_get_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Label_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ BSTR bstrLabel);


void __RPC_STUB IMSMQMessage3_put_Label_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_MaxTimeToReachQueue_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxTimeToReachQueue);


void __RPC_STUB IMSMQMessage3_get_MaxTimeToReachQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_MaxTimeToReachQueue_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lMaxTimeToReachQueue);


void __RPC_STUB IMSMQMessage3_put_MaxTimeToReachQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_MaxTimeToReceive_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMaxTimeToReceive);


void __RPC_STUB IMSMQMessage3_get_MaxTimeToReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_MaxTimeToReceive_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lMaxTimeToReceive);


void __RPC_STUB IMSMQMessage3_put_MaxTimeToReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_HashAlgorithm_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plHashAlg);


void __RPC_STUB IMSMQMessage3_get_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_HashAlgorithm_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lHashAlg);


void __RPC_STUB IMSMQMessage3_put_HashAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_EncryptAlgorithm_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plEncryptAlg);


void __RPC_STUB IMSMQMessage3_get_EncryptAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_EncryptAlgorithm_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lEncryptAlg);


void __RPC_STUB IMSMQMessage3_put_EncryptAlgorithm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_SentTime_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSentTime);


void __RPC_STUB IMSMQMessage3_get_SentTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ArrivedTime_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *plArrivedTime);


void __RPC_STUB IMSMQMessage3_get_ArrivedTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_DestinationQueueInfo_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoDest);


void __RPC_STUB IMSMQMessage3_get_DestinationQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_SenderCertificate_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderCert);


void __RPC_STUB IMSMQMessage3_get_SenderCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_SenderCertificate_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varSenderCert);


void __RPC_STUB IMSMQMessage3_put_SenderCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_SenderId_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSenderId);


void __RPC_STUB IMSMQMessage3_get_SenderId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_SenderIdType_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSenderIdType);


void __RPC_STUB IMSMQMessage3_get_SenderIdType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_SenderIdType_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lSenderIdType);


void __RPC_STUB IMSMQMessage3_put_SenderIdType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_Send_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *DestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction);


void __RPC_STUB IMSMQMessage3_Send_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_AttachCurrentSecurityContext_Proxy( 
    IMSMQMessage3 __RPC_FAR * This);


void __RPC_STUB IMSMQMessage3_AttachCurrentSecurityContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_SenderVersion_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plSenderVersion);


void __RPC_STUB IMSMQMessage3_get_SenderVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Extension_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarExtension);


void __RPC_STUB IMSMQMessage3_get_Extension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Extension_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varExtension);


void __RPC_STUB IMSMQMessage3_put_Extension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ConnectorTypeGuid_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidConnectorType);


void __RPC_STUB IMSMQMessage3_get_ConnectorTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_ConnectorTypeGuid_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ BSTR bstrGuidConnectorType);


void __RPC_STUB IMSMQMessage3_put_ConnectorTypeGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_TransactionStatusQueueInfo_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoXactStatus);


void __RPC_STUB IMSMQMessage3_get_TransactionStatusQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_DestinationSymmetricKey_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarDestSymmKey);


void __RPC_STUB IMSMQMessage3_get_DestinationSymmetricKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_DestinationSymmetricKey_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varDestSymmKey);


void __RPC_STUB IMSMQMessage3_put_DestinationSymmetricKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Signature_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarSignature);


void __RPC_STUB IMSMQMessage3_get_Signature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_Signature_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varSignature);


void __RPC_STUB IMSMQMessage3_put_Signature_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AuthenticationProviderType_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAuthProvType);


void __RPC_STUB IMSMQMessage3_get_AuthenticationProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_AuthenticationProviderType_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lAuthProvType);


void __RPC_STUB IMSMQMessage3_put_AuthenticationProviderType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AuthenticationProviderName_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAuthProvName);


void __RPC_STUB IMSMQMessage3_get_AuthenticationProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_AuthenticationProviderName_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ BSTR bstrAuthProvName);


void __RPC_STUB IMSMQMessage3_put_AuthenticationProviderName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_SenderId_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ VARIANT varSenderId);


void __RPC_STUB IMSMQMessage3_put_SenderId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_MsgClass_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMsgClass);


void __RPC_STUB IMSMQMessage3_get_MsgClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_put_MsgClass_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ long lMsgClass);


void __RPC_STUB IMSMQMessage3_put_MsgClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Properties_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQMessage3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_TransactionId_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarXactId);


void __RPC_STUB IMSMQMessage3_get_TransactionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_IsFirstInTransaction_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisFirstInXact);


void __RPC_STUB IMSMQMessage3_get_IsFirstInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_IsLastInTransaction_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisLastInXact);


void __RPC_STUB IMSMQMessage3_get_IsLastInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ResponseQueueInfo_v2_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoResponse);


void __RPC_STUB IMSMQMessage3_get_ResponseQueueInfo_v2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_ResponseQueueInfo_v2_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoResponse);


void __RPC_STUB IMSMQMessage3_putref_ResponseQueueInfo_v2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AdminQueueInfo_v2_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo2 __RPC_FAR *__RPC_FAR *ppqinfoAdmin);


void __RPC_STUB IMSMQMessage3_get_AdminQueueInfo_v2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_AdminQueueInfo_v2_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo2 __RPC_FAR *pqinfoAdmin);


void __RPC_STUB IMSMQMessage3_putref_AdminQueueInfo_v2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ReceivedAuthenticationLevel_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *psReceivedAuthenticationLevel);


void __RPC_STUB IMSMQMessage3_get_ReceivedAuthenticationLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ResponseQueueInfo_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoResponse);


void __RPC_STUB IMSMQMessage3_get_ResponseQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_ResponseQueueInfo_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo3 __RPC_FAR *pqinfoResponse);


void __RPC_STUB IMSMQMessage3_putref_ResponseQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AdminQueueInfo_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfoAdmin);


void __RPC_STUB IMSMQMessage3_get_AdminQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_AdminQueueInfo_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IMSMQQueueInfo3 __RPC_FAR *pqinfoAdmin);


void __RPC_STUB IMSMQMessage3_putref_AdminQueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_ResponseDestination_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestResponse);


void __RPC_STUB IMSMQMessage3_get_ResponseDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_ResponseDestination_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pdestResponse);


void __RPC_STUB IMSMQMessage3_putref_ResponseDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_AdminDestination_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestAdmin);


void __RPC_STUB IMSMQMessage3_get_AdminDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_putref_AdminDestination_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pdestAdmin);


void __RPC_STUB IMSMQMessage3_putref_AdminDestination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_Destination_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdestDestination);


void __RPC_STUB IMSMQMessage3_get_Destination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQMessage3_get_LookupId_Proxy( 
    IMSMQMessage3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarLookupId);


void __RPC_STUB IMSMQMessage3_get_LookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQMessage3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQMessage,0xD7D6E075,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E075-DCCD-11d0-AA4B-0060970DEBAE")
MSMQMessage;
#endif

#ifndef __IMSMQQueue3_INTERFACE_DEFINED__
#define __IMSMQQueue3_INTERFACE_DEFINED__

/* interface IMSMQQueue3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQQueue3,0xeba96b1b,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b1b-2168-11d3-898c-00e02c074f6b")
    IMSMQQueue3 : public IDispatch
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Access( 
            /* [retval][out] */ long __RPC_FAR *plAccess) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ShareMode( 
            /* [retval][out] */ long __RPC_FAR *plShareMode) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueueInfo( 
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfo) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_Handle( 
            /* [retval][out] */ long __RPC_FAR *plHandle) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsOpen( 
            /* [retval][out] */ Boolean __RPC_FAR *pisOpen) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE Receive_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE Peek_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE EnableNotification( 
            /* [in] */ IMSMQEvent3 __RPC_FAR *Event,
            /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveCurrent_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekNext_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekCurrent_v1( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Receive( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Peek( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveCurrent( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekNext( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekCurrent( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
        virtual /* [id][propget][helpstringcontext][hidden] */ HRESULT STDMETHODCALLTYPE get_Handle2( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarHandle) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveByLookupId( 
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveNextByLookupId( 
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceivePreviousByLookupId( 
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveFirstByLookupId( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE ReceiveLastByLookupId( 
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekByLookupId( 
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekNextByLookupId( 
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekPreviousByLookupId( 
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekFirstByLookupId( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE PeekLastByLookupId( 
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQQueue3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQQueue3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQQueue3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Access )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plAccess);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShareMode )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plShareMode);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueueInfo )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfo);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Handle )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plHandle);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsOpen )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ Boolean __RPC_FAR *pisOpen);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IMSMQQueue3 __RPC_FAR * This);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Receive_v1 )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Peek_v1 )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableNotification )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ IMSMQEvent3 __RPC_FAR *Event,
            /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IMSMQQueue3 __RPC_FAR * This);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveCurrent_v1 )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekNext_v1 )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [hidden][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekCurrent_v1 )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Receive )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Peek )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveCurrent )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekNext )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekCurrent )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        /* [id][propget][helpstringcontext][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Handle2 )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarHandle);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveNextByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceivePreviousByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveFirstByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceiveLastByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekNextByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekPreviousByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [in] */ VARIANT LookupId,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekFirstByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PeekLastByLookupId )( 
            IMSMQQueue3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
            /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
            /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
            /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);
        
        END_INTERFACE
    } IMSMQQueue3Vtbl;

    interface IMSMQQueue3
    {
        CONST_VTBL struct IMSMQQueue3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQQueue3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQQueue3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQQueue3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQQueue3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQQueue3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQQueue3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQQueue3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQQueue3_get_Access(This,plAccess)	\
    (This)->lpVtbl -> get_Access(This,plAccess)

#define IMSMQQueue3_get_ShareMode(This,plShareMode)	\
    (This)->lpVtbl -> get_ShareMode(This,plShareMode)

#define IMSMQQueue3_get_QueueInfo(This,ppqinfo)	\
    (This)->lpVtbl -> get_QueueInfo(This,ppqinfo)

#define IMSMQQueue3_get_Handle(This,plHandle)	\
    (This)->lpVtbl -> get_Handle(This,plHandle)

#define IMSMQQueue3_get_IsOpen(This,pisOpen)	\
    (This)->lpVtbl -> get_IsOpen(This,pisOpen)

#define IMSMQQueue3_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IMSMQQueue3_Receive_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> Receive_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue3_Peek_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> Peek_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue3_EnableNotification(This,Event,Cursor,ReceiveTimeout)	\
    (This)->lpVtbl -> EnableNotification(This,Event,Cursor,ReceiveTimeout)

#define IMSMQQueue3_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IMSMQQueue3_ReceiveCurrent_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> ReceiveCurrent_v1(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue3_PeekNext_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> PeekNext_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue3_PeekCurrent_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)	\
    (This)->lpVtbl -> PeekCurrent_v1(This,WantDestinationQueue,WantBody,ReceiveTimeout,ppmsg)

#define IMSMQQueue3_Receive(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> Receive(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue3_Peek(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> Peek(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue3_ReceiveCurrent(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceiveCurrent(This,Transaction,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekNext(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekNext(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekCurrent(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekCurrent(This,WantDestinationQueue,WantBody,ReceiveTimeout,WantConnectorType,ppmsg)

#define IMSMQQueue3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#define IMSMQQueue3_get_Handle2(This,pvarHandle)	\
    (This)->lpVtbl -> get_Handle2(This,pvarHandle)

#define IMSMQQueue3_ReceiveByLookupId(This,LookupId,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceiveByLookupId(This,LookupId,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_ReceiveNextByLookupId(This,LookupId,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceiveNextByLookupId(This,LookupId,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_ReceivePreviousByLookupId(This,LookupId,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceivePreviousByLookupId(This,LookupId,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_ReceiveFirstByLookupId(This,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceiveFirstByLookupId(This,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_ReceiveLastByLookupId(This,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> ReceiveLastByLookupId(This,Transaction,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekByLookupId(This,LookupId,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekByLookupId(This,LookupId,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekNextByLookupId(This,LookupId,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekNextByLookupId(This,LookupId,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekPreviousByLookupId(This,LookupId,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekPreviousByLookupId(This,LookupId,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekFirstByLookupId(This,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekFirstByLookupId(This,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#define IMSMQQueue3_PeekLastByLookupId(This,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)	\
    (This)->lpVtbl -> PeekLastByLookupId(This,WantDestinationQueue,WantBody,WantConnectorType,ppmsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_Access_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plAccess);


void __RPC_STUB IMSMQQueue3_get_Access_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_ShareMode_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plShareMode);


void __RPC_STUB IMSMQQueue3_get_ShareMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_QueueInfo_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQQueueInfo3 __RPC_FAR *__RPC_FAR *ppqinfo);


void __RPC_STUB IMSMQQueue3_get_QueueInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_Handle_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plHandle);


void __RPC_STUB IMSMQQueue3_get_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_IsOpen_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ Boolean __RPC_FAR *pisOpen);


void __RPC_STUB IMSMQQueue3_get_IsOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_Close_Proxy( 
    IMSMQQueue3 __RPC_FAR * This);


void __RPC_STUB IMSMQQueue3_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_Receive_v1_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_Receive_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_Peek_v1_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_Peek_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_EnableNotification_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ IMSMQEvent3 __RPC_FAR *Event,
    /* [optional][in] */ VARIANT __RPC_FAR *Cursor,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout);


void __RPC_STUB IMSMQQueue3_EnableNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_Reset_Proxy( 
    IMSMQQueue3 __RPC_FAR * This);


void __RPC_STUB IMSMQQueue3_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceiveCurrent_v1_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceiveCurrent_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekNext_v1_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekNext_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekCurrent_v1_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [retval][out] */ IMSMQMessage __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekCurrent_v1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_Receive_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_Receive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_Peek_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_Peek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceiveCurrent_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceiveCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekNext_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekCurrent_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *ReceiveTimeout,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekCurrent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_Properties_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQQueue3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_get_Handle2_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarHandle);


void __RPC_STUB IMSMQQueue3_get_Handle2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceiveByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ VARIANT LookupId,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceiveByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceiveNextByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ VARIANT LookupId,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceiveNextByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceivePreviousByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ VARIANT LookupId,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceivePreviousByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceiveFirstByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceiveFirstByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_ReceiveLastByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Transaction,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_ReceiveLastByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ VARIANT LookupId,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekNextByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ VARIANT LookupId,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekNextByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekPreviousByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [in] */ VARIANT LookupId,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekPreviousByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekFirstByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekFirstByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQQueue3_PeekLastByLookupId_Proxy( 
    IMSMQQueue3 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *WantDestinationQueue,
    /* [optional][in] */ VARIANT __RPC_FAR *WantBody,
    /* [optional][in] */ VARIANT __RPC_FAR *WantConnectorType,
    /* [retval][out] */ IMSMQMessage3 __RPC_FAR *__RPC_FAR *ppmsg);


void __RPC_STUB IMSMQQueue3_PeekLastByLookupId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQQueue3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQQueue,0xD7D6E079,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E079-DCCD-11d0-AA4B-0060970DEBAE")
MSMQQueue;
#endif

#ifndef __IMSMQPrivateEvent_INTERFACE_DEFINED__
#define __IMSMQPrivateEvent_INTERFACE_DEFINED__

/* interface IMSMQPrivateEvent */
/* [object][dual][hidden][uuid] */ 


DEFINE_GUID(IID_IMSMQPrivateEvent,0xD7AB3341,0xC9D3,0x11d1,0xBB,0x47,0x00,0x80,0xC7,0xC5,0xA2,0xC0);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7AB3341-C9D3-11d1-BB47-0080C7C5A2C0")
    IMSMQPrivateEvent : public IDispatch
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Hwnd( 
            /* [retval][out] */ long __RPC_FAR *phwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FireArrivedEvent( 
            /* [in] */ IMSMQQueue __RPC_FAR *pq,
            /* [in] */ long msgcursor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FireArrivedErrorEvent( 
            /* [in] */ IMSMQQueue __RPC_FAR *pq,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ long msgcursor) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQPrivateEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQPrivateEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQPrivateEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Hwnd )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FireArrivedEvent )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [in] */ IMSMQQueue __RPC_FAR *pq,
            /* [in] */ long msgcursor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FireArrivedErrorEvent )( 
            IMSMQPrivateEvent __RPC_FAR * This,
            /* [in] */ IMSMQQueue __RPC_FAR *pq,
            /* [in] */ HRESULT hrStatus,
            /* [in] */ long msgcursor);
        
        END_INTERFACE
    } IMSMQPrivateEventVtbl;

    interface IMSMQPrivateEvent
    {
        CONST_VTBL struct IMSMQPrivateEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQPrivateEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQPrivateEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQPrivateEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQPrivateEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQPrivateEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQPrivateEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQPrivateEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQPrivateEvent_get_Hwnd(This,phwnd)	\
    (This)->lpVtbl -> get_Hwnd(This,phwnd)

#define IMSMQPrivateEvent_FireArrivedEvent(This,pq,msgcursor)	\
    (This)->lpVtbl -> FireArrivedEvent(This,pq,msgcursor)

#define IMSMQPrivateEvent_FireArrivedErrorEvent(This,pq,hrStatus,msgcursor)	\
    (This)->lpVtbl -> FireArrivedErrorEvent(This,pq,hrStatus,msgcursor)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IMSMQPrivateEvent_get_Hwnd_Proxy( 
    IMSMQPrivateEvent __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *phwnd);


void __RPC_STUB IMSMQPrivateEvent_get_Hwnd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMSMQPrivateEvent_FireArrivedEvent_Proxy( 
    IMSMQPrivateEvent __RPC_FAR * This,
    /* [in] */ IMSMQQueue __RPC_FAR *pq,
    /* [in] */ long msgcursor);


void __RPC_STUB IMSMQPrivateEvent_FireArrivedEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMSMQPrivateEvent_FireArrivedErrorEvent_Proxy( 
    IMSMQPrivateEvent __RPC_FAR * This,
    /* [in] */ IMSMQQueue __RPC_FAR *pq,
    /* [in] */ HRESULT hrStatus,
    /* [in] */ long msgcursor);


void __RPC_STUB IMSMQPrivateEvent_FireArrivedErrorEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQPrivateEvent_INTERFACE_DEFINED__ */


#ifndef ___DMSMQEventEvents_DISPINTERFACE_DEFINED__
#define ___DMSMQEventEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DMSMQEventEvents */
/* [hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(DIID__DMSMQEventEvents,0xD7D6E078,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("D7D6E078-DCCD-11d0-AA4B-0060970DEBAE")
    _DMSMQEventEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMSMQEventEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _DMSMQEventEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _DMSMQEventEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _DMSMQEventEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _DMSMQEventEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _DMSMQEventEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _DMSMQEventEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _DMSMQEventEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _DMSMQEventEventsVtbl;

    interface _DMSMQEventEvents
    {
        CONST_VTBL struct _DMSMQEventEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMSMQEventEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _DMSMQEventEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _DMSMQEventEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _DMSMQEventEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _DMSMQEventEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _DMSMQEventEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _DMSMQEventEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMSMQEventEvents_DISPINTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQEvent,0xD7D6E07A,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E07A-DCCD-11d0-AA4B-0060970DEBAE")
MSMQEvent;
#endif

DEFINE_GUID(CLSID_MSMQQueueInfo,0xD7D6E07C,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E07C-DCCD-11d0-AA4B-0060970DEBAE")
MSMQQueueInfo;
#endif

DEFINE_GUID(CLSID_MSMQQueueInfos,0xD7D6E07E,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E07E-DCCD-11d0-AA4B-0060970DEBAE")
MSMQQueueInfos;
#endif

#ifndef __IMSMQTransaction2_INTERFACE_DEFINED__
#define __IMSMQTransaction2_INTERFACE_DEFINED__

/* interface IMSMQTransaction2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQTransaction2,0x2CE0C5B0,0x6E67,0x11D2,0xB0,0xE6,0x00,0xE0,0x2C,0x07,0x4F,0x6B);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2CE0C5B0-6E67-11D2-B0E6-00E02C074F6B")
    IMSMQTransaction2 : public IMSMQTransaction
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE InitNew( 
            /* [in] */ VARIANT varTransaction) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQTransaction2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQTransaction2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQTransaction2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Transaction )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plTransaction);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *grfTC,
            /* [optional][in] */ VARIANT __RPC_FAR *grfRM);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abort )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *fAsync);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitNew )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [in] */ VARIANT varTransaction);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQTransaction2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQTransaction2Vtbl;

    interface IMSMQTransaction2
    {
        CONST_VTBL struct IMSMQTransaction2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQTransaction2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQTransaction2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQTransaction2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQTransaction2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQTransaction2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQTransaction2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQTransaction2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQTransaction2_get_Transaction(This,plTransaction)	\
    (This)->lpVtbl -> get_Transaction(This,plTransaction)

#define IMSMQTransaction2_Commit(This,fRetaining,grfTC,grfRM)	\
    (This)->lpVtbl -> Commit(This,fRetaining,grfTC,grfRM)

#define IMSMQTransaction2_Abort(This,fRetaining,fAsync)	\
    (This)->lpVtbl -> Abort(This,fRetaining,fAsync)


#define IMSMQTransaction2_InitNew(This,varTransaction)	\
    (This)->lpVtbl -> InitNew(This,varTransaction)

#define IMSMQTransaction2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransaction2_InitNew_Proxy( 
    IMSMQTransaction2 __RPC_FAR * This,
    /* [in] */ VARIANT varTransaction);


void __RPC_STUB IMSMQTransaction2_InitNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQTransaction2_get_Properties_Proxy( 
    IMSMQTransaction2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQTransaction2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQTransaction2_INTERFACE_DEFINED__ */


#ifndef __IMSMQTransaction3_INTERFACE_DEFINED__
#define __IMSMQTransaction3_INTERFACE_DEFINED__

/* interface IMSMQTransaction3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQTransaction3,0xeba96b13,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b13-2168-11d3-898c-00e02c074f6b")
    IMSMQTransaction3 : public IMSMQTransaction2
    {
    public:
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ITransaction( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarITransaction) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQTransaction3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQTransaction3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQTransaction3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Transaction )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plTransaction);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *grfTC,
            /* [optional][in] */ VARIANT __RPC_FAR *grfRM);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abort )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *fRetaining,
            /* [optional][in] */ VARIANT __RPC_FAR *fAsync);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitNew )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [in] */ VARIANT varTransaction);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ITransaction )( 
            IMSMQTransaction3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarITransaction);
        
        END_INTERFACE
    } IMSMQTransaction3Vtbl;

    interface IMSMQTransaction3
    {
        CONST_VTBL struct IMSMQTransaction3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQTransaction3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQTransaction3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQTransaction3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQTransaction3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQTransaction3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQTransaction3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQTransaction3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQTransaction3_get_Transaction(This,plTransaction)	\
    (This)->lpVtbl -> get_Transaction(This,plTransaction)

#define IMSMQTransaction3_Commit(This,fRetaining,grfTC,grfRM)	\
    (This)->lpVtbl -> Commit(This,fRetaining,grfTC,grfRM)

#define IMSMQTransaction3_Abort(This,fRetaining,fAsync)	\
    (This)->lpVtbl -> Abort(This,fRetaining,fAsync)


#define IMSMQTransaction3_InitNew(This,varTransaction)	\
    (This)->lpVtbl -> InitNew(This,varTransaction)

#define IMSMQTransaction3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)


#define IMSMQTransaction3_get_ITransaction(This,pvarITransaction)	\
    (This)->lpVtbl -> get_ITransaction(This,pvarITransaction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransaction3_get_ITransaction_Proxy( 
    IMSMQTransaction3 __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarITransaction);


void __RPC_STUB IMSMQTransaction3_get_ITransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQTransaction3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQTransaction,0xD7D6E080,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E080-DCCD-11d0-AA4B-0060970DEBAE")
MSMQTransaction;
#endif

#ifndef __IMSMQCoordinatedTransactionDispenser2_INTERFACE_DEFINED__
#define __IMSMQCoordinatedTransactionDispenser2_INTERFACE_DEFINED__

/* interface IMSMQCoordinatedTransactionDispenser2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQCoordinatedTransactionDispenser2,0xeba96b10,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b10-2168-11d3-898c-00e02c074f6b")
    IMSMQCoordinatedTransactionDispenser2 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [retval][out] */ IMSMQTransaction2 __RPC_FAR *__RPC_FAR *ptransaction) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQCoordinatedTransactionDispenser2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransaction )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQTransaction2 __RPC_FAR *__RPC_FAR *ptransaction);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQCoordinatedTransactionDispenser2Vtbl;

    interface IMSMQCoordinatedTransactionDispenser2
    {
        CONST_VTBL struct IMSMQCoordinatedTransactionDispenser2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQCoordinatedTransactionDispenser2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQCoordinatedTransactionDispenser2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQCoordinatedTransactionDispenser2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQCoordinatedTransactionDispenser2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQCoordinatedTransactionDispenser2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQCoordinatedTransactionDispenser2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQCoordinatedTransactionDispenser2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQCoordinatedTransactionDispenser2_BeginTransaction(This,ptransaction)	\
    (This)->lpVtbl -> BeginTransaction(This,ptransaction)

#define IMSMQCoordinatedTransactionDispenser2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQCoordinatedTransactionDispenser2_BeginTransaction_Proxy( 
    IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQTransaction2 __RPC_FAR *__RPC_FAR *ptransaction);


void __RPC_STUB IMSMQCoordinatedTransactionDispenser2_BeginTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQCoordinatedTransactionDispenser2_get_Properties_Proxy( 
    IMSMQCoordinatedTransactionDispenser2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQCoordinatedTransactionDispenser2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQCoordinatedTransactionDispenser2_INTERFACE_DEFINED__ */


#ifndef __IMSMQCoordinatedTransactionDispenser3_INTERFACE_DEFINED__
#define __IMSMQCoordinatedTransactionDispenser3_INTERFACE_DEFINED__

/* interface IMSMQCoordinatedTransactionDispenser3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQCoordinatedTransactionDispenser3,0xeba96b14,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b14-2168-11d3-898c-00e02c074f6b")
    IMSMQCoordinatedTransactionDispenser3 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [retval][out] */ IMSMQTransaction3 __RPC_FAR *__RPC_FAR *ptransaction) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQCoordinatedTransactionDispenser3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransaction )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQTransaction3 __RPC_FAR *__RPC_FAR *ptransaction);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQCoordinatedTransactionDispenser3Vtbl;

    interface IMSMQCoordinatedTransactionDispenser3
    {
        CONST_VTBL struct IMSMQCoordinatedTransactionDispenser3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQCoordinatedTransactionDispenser3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQCoordinatedTransactionDispenser3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQCoordinatedTransactionDispenser3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQCoordinatedTransactionDispenser3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQCoordinatedTransactionDispenser3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQCoordinatedTransactionDispenser3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQCoordinatedTransactionDispenser3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQCoordinatedTransactionDispenser3_BeginTransaction(This,ptransaction)	\
    (This)->lpVtbl -> BeginTransaction(This,ptransaction)

#define IMSMQCoordinatedTransactionDispenser3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQCoordinatedTransactionDispenser3_BeginTransaction_Proxy( 
    IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQTransaction3 __RPC_FAR *__RPC_FAR *ptransaction);


void __RPC_STUB IMSMQCoordinatedTransactionDispenser3_BeginTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQCoordinatedTransactionDispenser3_get_Properties_Proxy( 
    IMSMQCoordinatedTransactionDispenser3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQCoordinatedTransactionDispenser3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQCoordinatedTransactionDispenser3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQCoordinatedTransactionDispenser,0xD7D6E082,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E082-DCCD-11d0-AA4B-0060970DEBAE")
MSMQCoordinatedTransactionDispenser;
#endif

#ifndef __IMSMQTransactionDispenser2_INTERFACE_DEFINED__
#define __IMSMQTransactionDispenser2_INTERFACE_DEFINED__

/* interface IMSMQTransactionDispenser2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQTransactionDispenser2,0xeba96b11,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b11-2168-11d3-898c-00e02c074f6b")
    IMSMQTransactionDispenser2 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [retval][out] */ IMSMQTransaction2 __RPC_FAR *__RPC_FAR *ptransaction) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQTransactionDispenser2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransaction )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [retval][out] */ IMSMQTransaction2 __RPC_FAR *__RPC_FAR *ptransaction);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQTransactionDispenser2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQTransactionDispenser2Vtbl;

    interface IMSMQTransactionDispenser2
    {
        CONST_VTBL struct IMSMQTransactionDispenser2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQTransactionDispenser2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQTransactionDispenser2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQTransactionDispenser2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQTransactionDispenser2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQTransactionDispenser2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQTransactionDispenser2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQTransactionDispenser2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQTransactionDispenser2_BeginTransaction(This,ptransaction)	\
    (This)->lpVtbl -> BeginTransaction(This,ptransaction)

#define IMSMQTransactionDispenser2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransactionDispenser2_BeginTransaction_Proxy( 
    IMSMQTransactionDispenser2 __RPC_FAR * This,
    /* [retval][out] */ IMSMQTransaction2 __RPC_FAR *__RPC_FAR *ptransaction);


void __RPC_STUB IMSMQTransactionDispenser2_BeginTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQTransactionDispenser2_get_Properties_Proxy( 
    IMSMQTransactionDispenser2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQTransactionDispenser2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQTransactionDispenser2_INTERFACE_DEFINED__ */


#ifndef __IMSMQTransactionDispenser3_INTERFACE_DEFINED__
#define __IMSMQTransactionDispenser3_INTERFACE_DEFINED__

/* interface IMSMQTransactionDispenser3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQTransactionDispenser3,0xeba96b15,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b15-2168-11d3-898c-00e02c074f6b")
    IMSMQTransactionDispenser3 : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE BeginTransaction( 
            /* [retval][out] */ IMSMQTransaction3 __RPC_FAR *__RPC_FAR *ptransaction) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQTransactionDispenser3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginTransaction )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [retval][out] */ IMSMQTransaction3 __RPC_FAR *__RPC_FAR *ptransaction);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQTransactionDispenser3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQTransactionDispenser3Vtbl;

    interface IMSMQTransactionDispenser3
    {
        CONST_VTBL struct IMSMQTransactionDispenser3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQTransactionDispenser3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQTransactionDispenser3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQTransactionDispenser3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQTransactionDispenser3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQTransactionDispenser3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQTransactionDispenser3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQTransactionDispenser3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQTransactionDispenser3_BeginTransaction(This,ptransaction)	\
    (This)->lpVtbl -> BeginTransaction(This,ptransaction)

#define IMSMQTransactionDispenser3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQTransactionDispenser3_BeginTransaction_Proxy( 
    IMSMQTransactionDispenser3 __RPC_FAR * This,
    /* [retval][out] */ IMSMQTransaction3 __RPC_FAR *__RPC_FAR *ptransaction);


void __RPC_STUB IMSMQTransactionDispenser3_BeginTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQTransactionDispenser3_get_Properties_Proxy( 
    IMSMQTransactionDispenser3 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQTransactionDispenser3_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQTransactionDispenser3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQTransactionDispenser,0xD7D6E084,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E084-DCCD-11d0-AA4B-0060970DEBAE")
MSMQTransactionDispenser;
#endif

#ifndef __IMSMQApplication_INTERFACE_DEFINED__
#define __IMSMQApplication_INTERFACE_DEFINED__

/* interface IMSMQApplication */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQApplication,0xD7D6E085,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D7D6E085-DCCD-11d0-AA4B-0060970DEBAE")
    IMSMQApplication : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE MachineIdOfMachineName( 
            /* [in] */ BSTR MachineName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQApplication __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQApplication __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQApplication __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQApplication __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQApplication __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MachineIdOfMachineName )( 
            IMSMQApplication __RPC_FAR * This,
            /* [in] */ BSTR MachineName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuid);
        
        END_INTERFACE
    } IMSMQApplicationVtbl;

    interface IMSMQApplication
    {
        CONST_VTBL struct IMSMQApplicationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQApplication_MachineIdOfMachineName(This,MachineName,pbstrGuid)	\
    (This)->lpVtbl -> MachineIdOfMachineName(This,MachineName,pbstrGuid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication_MachineIdOfMachineName_Proxy( 
    IMSMQApplication __RPC_FAR * This,
    /* [in] */ BSTR MachineName,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuid);


void __RPC_STUB IMSMQApplication_MachineIdOfMachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQApplication_INTERFACE_DEFINED__ */


#ifndef __IMSMQApplication2_INTERFACE_DEFINED__
#define __IMSMQApplication2_INTERFACE_DEFINED__

/* interface IMSMQApplication2 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQApplication2,0x12A30900,0x7300,0x11D2,0xB0,0xE6,0x00,0xE0,0x2C,0x07,0x4F,0x6B);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("12A30900-7300-11D2-B0E6-00E02C074F6B")
    IMSMQApplication2 : public IMSMQApplication
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE RegisterCertificate( 
            /* [optional][in] */ VARIANT __RPC_FAR *Flags,
            /* [optional][in] */ VARIANT __RPC_FAR *ExternalCertificate) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE MachineNameOfMachineId( 
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MSMQVersionMajor( 
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionMajor) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MSMQVersionMinor( 
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionMinor) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_MSMQVersionBuild( 
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionBuild) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsDsEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsDsEnabled) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQApplication2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQApplication2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQApplication2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MachineIdOfMachineName )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [in] */ BSTR MachineName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuid);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterCertificate )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Flags,
            /* [optional][in] */ VARIANT __RPC_FAR *ExternalCertificate);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MachineNameOfMachineId )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSMQVersionMajor )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionMajor);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSMQVersionMinor )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionMinor);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSMQVersionBuild )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionBuild);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsDsEnabled )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsDsEnabled);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQApplication2 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQApplication2Vtbl;

    interface IMSMQApplication2
    {
        CONST_VTBL struct IMSMQApplication2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQApplication2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQApplication2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQApplication2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQApplication2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQApplication2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQApplication2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQApplication2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQApplication2_MachineIdOfMachineName(This,MachineName,pbstrGuid)	\
    (This)->lpVtbl -> MachineIdOfMachineName(This,MachineName,pbstrGuid)


#define IMSMQApplication2_RegisterCertificate(This,Flags,ExternalCertificate)	\
    (This)->lpVtbl -> RegisterCertificate(This,Flags,ExternalCertificate)

#define IMSMQApplication2_MachineNameOfMachineId(This,bstrGuid,pbstrMachineName)	\
    (This)->lpVtbl -> MachineNameOfMachineId(This,bstrGuid,pbstrMachineName)

#define IMSMQApplication2_get_MSMQVersionMajor(This,psMSMQVersionMajor)	\
    (This)->lpVtbl -> get_MSMQVersionMajor(This,psMSMQVersionMajor)

#define IMSMQApplication2_get_MSMQVersionMinor(This,psMSMQVersionMinor)	\
    (This)->lpVtbl -> get_MSMQVersionMinor(This,psMSMQVersionMinor)

#define IMSMQApplication2_get_MSMQVersionBuild(This,psMSMQVersionBuild)	\
    (This)->lpVtbl -> get_MSMQVersionBuild(This,psMSMQVersionBuild)

#define IMSMQApplication2_get_IsDsEnabled(This,pfIsDsEnabled)	\
    (This)->lpVtbl -> get_IsDsEnabled(This,pfIsDsEnabled)

#define IMSMQApplication2_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_RegisterCertificate_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [optional][in] */ VARIANT __RPC_FAR *Flags,
    /* [optional][in] */ VARIANT __RPC_FAR *ExternalCertificate);


void __RPC_STUB IMSMQApplication2_RegisterCertificate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_MachineNameOfMachineId_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [in] */ BSTR bstrGuid,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName);


void __RPC_STUB IMSMQApplication2_MachineNameOfMachineId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_get_MSMQVersionMajor_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *psMSMQVersionMajor);


void __RPC_STUB IMSMQApplication2_get_MSMQVersionMajor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_get_MSMQVersionMinor_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *psMSMQVersionMinor);


void __RPC_STUB IMSMQApplication2_get_MSMQVersionMinor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_get_MSMQVersionBuild_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [retval][out] */ short __RPC_FAR *psMSMQVersionBuild);


void __RPC_STUB IMSMQApplication2_get_MSMQVersionBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_get_IsDsEnabled_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsDsEnabled);


void __RPC_STUB IMSMQApplication2_get_IsDsEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQApplication2_get_Properties_Proxy( 
    IMSMQApplication2 __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQApplication2_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQApplication2_INTERFACE_DEFINED__ */


#ifndef __IMSMQApplication3_INTERFACE_DEFINED__
#define __IMSMQApplication3_INTERFACE_DEFINED__

/* interface IMSMQApplication3 */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQApplication3,0xeba96b1f,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b1f-2168-11d3-898c-00e02c074f6b")
    IMSMQApplication3 : public IMSMQApplication2
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IMSMQApplication3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQApplication3 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQApplication3 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MachineIdOfMachineName )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [in] */ BSTR MachineName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuid);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterCertificate )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [optional][in] */ VARIANT __RPC_FAR *Flags,
            /* [optional][in] */ VARIANT __RPC_FAR *ExternalCertificate);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MachineNameOfMachineId )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSMQVersionMajor )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionMajor);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSMQVersionMinor )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionMinor);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MSMQVersionBuild )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [retval][out] */ short __RPC_FAR *psMSMQVersionBuild);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsDsEnabled )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsDsEnabled);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQApplication3 __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQApplication3Vtbl;

    interface IMSMQApplication3
    {
        CONST_VTBL struct IMSMQApplication3Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQApplication3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQApplication3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQApplication3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQApplication3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQApplication3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQApplication3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQApplication3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQApplication3_MachineIdOfMachineName(This,MachineName,pbstrGuid)	\
    (This)->lpVtbl -> MachineIdOfMachineName(This,MachineName,pbstrGuid)


#define IMSMQApplication3_RegisterCertificate(This,Flags,ExternalCertificate)	\
    (This)->lpVtbl -> RegisterCertificate(This,Flags,ExternalCertificate)

#define IMSMQApplication3_MachineNameOfMachineId(This,bstrGuid,pbstrMachineName)	\
    (This)->lpVtbl -> MachineNameOfMachineId(This,bstrGuid,pbstrMachineName)

#define IMSMQApplication3_get_MSMQVersionMajor(This,psMSMQVersionMajor)	\
    (This)->lpVtbl -> get_MSMQVersionMajor(This,psMSMQVersionMajor)

#define IMSMQApplication3_get_MSMQVersionMinor(This,psMSMQVersionMinor)	\
    (This)->lpVtbl -> get_MSMQVersionMinor(This,psMSMQVersionMinor)

#define IMSMQApplication3_get_MSMQVersionBuild(This,psMSMQVersionBuild)	\
    (This)->lpVtbl -> get_MSMQVersionBuild(This,psMSMQVersionBuild)

#define IMSMQApplication3_get_IsDsEnabled(This,pfIsDsEnabled)	\
    (This)->lpVtbl -> get_IsDsEnabled(This,pfIsDsEnabled)

#define IMSMQApplication3_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMSMQApplication3_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQApplication,0xD7D6E086,0xDCCD,0x11d0,0xAA,0x4B,0x00,0x60,0x97,0x0D,0xEB,0xAE);

#ifdef __cplusplus

class DECLSPEC_UUID("D7D6E086-DCCD-11d0-AA4B-0060970DEBAE")
MSMQApplication;
#endif

#ifndef __IMSMQDestination_INTERFACE_DEFINED__
#define __IMSMQDestination_INTERFACE_DEFINED__

/* interface IMSMQDestination */
/* [object][dual][hidden][helpstringcontext][uuid] */ 


DEFINE_GUID(IID_IMSMQDestination,0xeba96b16,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b16-2168-11d3-898c-00e02c074f6b")
    IMSMQDestination : public IDispatch
    {
    public:
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Open( void) = 0;
        
        virtual /* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_IsOpen( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsOpen) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_IADs( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppIADs) = 0;
        
        virtual /* [id][propputref][hidden] */ HRESULT STDMETHODCALLTYPE putref_IADs( 
            /* [in] */ IDispatch __RPC_FAR *pIADs) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_ADsPath( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrADsPath) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_ADsPath( 
            /* [in] */ BSTR bstrADsPath) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_QueuePathName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrQueuePathName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_QueuePathName( 
            /* [in] */ BSTR bstrQueuePathName) = 0;
        
        virtual /* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE get_FormatName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName) = 0;
        
        virtual /* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE put_FormatName( 
            /* [in] */ BSTR bstrFormatName) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Destinations( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDestinations) = 0;
        
        virtual /* [id][propputref][hidden] */ HRESULT STDMETHODCALLTYPE putref_Destinations( 
            /* [in] */ IDispatch __RPC_FAR *pDestinations) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Properties( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQDestinationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQDestination __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQDestination __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQDestination __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            IMSMQDestination __RPC_FAR * This);
        
        /* [helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IMSMQDestination __RPC_FAR * This);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsOpen )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsOpen);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IADs )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppIADs);
        
        /* [id][propputref][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_IADs )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pIADs);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ADsPath )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrADsPath);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ADsPath )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ BSTR bstrADsPath);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_QueuePathName )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrQueuePathName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_QueuePathName )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ BSTR bstrQueuePathName);
        
        /* [id][propget][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FormatName )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);
        
        /* [id][propput][helpstringcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FormatName )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ BSTR bstrFormatName);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Destinations )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDestinations);
        
        /* [id][propputref][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Destinations )( 
            IMSMQDestination __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pDestinations);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Properties )( 
            IMSMQDestination __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);
        
        END_INTERFACE
    } IMSMQDestinationVtbl;

    interface IMSMQDestination
    {
        CONST_VTBL struct IMSMQDestinationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQDestination_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQDestination_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQDestination_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQDestination_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQDestination_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQDestination_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQDestination_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQDestination_Open(This)	\
    (This)->lpVtbl -> Open(This)

#define IMSMQDestination_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IMSMQDestination_get_IsOpen(This,pfIsOpen)	\
    (This)->lpVtbl -> get_IsOpen(This,pfIsOpen)

#define IMSMQDestination_get_IADs(This,ppIADs)	\
    (This)->lpVtbl -> get_IADs(This,ppIADs)

#define IMSMQDestination_putref_IADs(This,pIADs)	\
    (This)->lpVtbl -> putref_IADs(This,pIADs)

#define IMSMQDestination_get_ADsPath(This,pbstrADsPath)	\
    (This)->lpVtbl -> get_ADsPath(This,pbstrADsPath)

#define IMSMQDestination_put_ADsPath(This,bstrADsPath)	\
    (This)->lpVtbl -> put_ADsPath(This,bstrADsPath)

#define IMSMQDestination_get_QueuePathName(This,pbstrQueuePathName)	\
    (This)->lpVtbl -> get_QueuePathName(This,pbstrQueuePathName)

#define IMSMQDestination_put_QueuePathName(This,bstrQueuePathName)	\
    (This)->lpVtbl -> put_QueuePathName(This,bstrQueuePathName)

#define IMSMQDestination_get_FormatName(This,pbstrFormatName)	\
    (This)->lpVtbl -> get_FormatName(This,pbstrFormatName)

#define IMSMQDestination_put_FormatName(This,bstrFormatName)	\
    (This)->lpVtbl -> put_FormatName(This,bstrFormatName)

#define IMSMQDestination_get_Destinations(This,ppDestinations)	\
    (This)->lpVtbl -> get_Destinations(This,ppDestinations)

#define IMSMQDestination_putref_Destinations(This,pDestinations)	\
    (This)->lpVtbl -> putref_Destinations(This,pDestinations)

#define IMSMQDestination_get_Properties(This,ppcolProperties)	\
    (This)->lpVtbl -> get_Properties(This,ppcolProperties)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_Open_Proxy( 
    IMSMQDestination __RPC_FAR * This);


void __RPC_STUB IMSMQDestination_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_Close_Proxy( 
    IMSMQDestination __RPC_FAR * This);


void __RPC_STUB IMSMQDestination_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_IsOpen_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsOpen);


void __RPC_STUB IMSMQDestination_get_IsOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_IADs_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppIADs);


void __RPC_STUB IMSMQDestination_get_IADs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_putref_IADs_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pIADs);


void __RPC_STUB IMSMQDestination_putref_IADs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_ADsPath_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrADsPath);


void __RPC_STUB IMSMQDestination_get_ADsPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_put_ADsPath_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [in] */ BSTR bstrADsPath);


void __RPC_STUB IMSMQDestination_put_ADsPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_QueuePathName_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrQueuePathName);


void __RPC_STUB IMSMQDestination_get_QueuePathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_put_QueuePathName_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [in] */ BSTR bstrQueuePathName);


void __RPC_STUB IMSMQDestination_put_QueuePathName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_FormatName_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFormatName);


void __RPC_STUB IMSMQDestination_get_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][helpstringcontext] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_put_FormatName_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [in] */ BSTR bstrFormatName);


void __RPC_STUB IMSMQDestination_put_FormatName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_Destinations_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDestinations);


void __RPC_STUB IMSMQDestination_get_Destinations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propputref][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_putref_Destinations_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pDestinations);


void __RPC_STUB IMSMQDestination_putref_Destinations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQDestination_get_Properties_Proxy( 
    IMSMQDestination __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppcolProperties);


void __RPC_STUB IMSMQDestination_get_Properties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQDestination_INTERFACE_DEFINED__ */


#ifndef __IMSMQPrivateDestination_INTERFACE_DEFINED__
#define __IMSMQPrivateDestination_INTERFACE_DEFINED__

/* interface IMSMQPrivateDestination */
/* [object][dual][hidden][uuid] */ 


DEFINE_GUID(IID_IMSMQPrivateDestination,0xeba96b17,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eba96b17-2168-11d3-898c-00e02c074f6b")
    IMSMQPrivateDestination : public IDispatch
    {
    public:
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_Handle( 
            /* [retval][out] */ VARIANT __RPC_FAR *pvarHandle) = 0;
        
        virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_Handle( 
            /* [in] */ VARIANT varHandle) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMSMQPrivateDestinationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMSMQPrivateDestination __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMSMQPrivateDestination __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Handle )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pvarHandle);
        
        /* [id][propput][hidden] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Handle )( 
            IMSMQPrivateDestination __RPC_FAR * This,
            /* [in] */ VARIANT varHandle);
        
        END_INTERFACE
    } IMSMQPrivateDestinationVtbl;

    interface IMSMQPrivateDestination
    {
        CONST_VTBL struct IMSMQPrivateDestinationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMSMQPrivateDestination_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMSMQPrivateDestination_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMSMQPrivateDestination_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMSMQPrivateDestination_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMSMQPrivateDestination_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMSMQPrivateDestination_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMSMQPrivateDestination_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMSMQPrivateDestination_get_Handle(This,pvarHandle)	\
    (This)->lpVtbl -> get_Handle(This,pvarHandle)

#define IMSMQPrivateDestination_put_Handle(This,varHandle)	\
    (This)->lpVtbl -> put_Handle(This,varHandle)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQPrivateDestination_get_Handle_Proxy( 
    IMSMQPrivateDestination __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pvarHandle);


void __RPC_STUB IMSMQPrivateDestination_get_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE IMSMQPrivateDestination_put_Handle_Proxy( 
    IMSMQPrivateDestination __RPC_FAR * This,
    /* [in] */ VARIANT varHandle);


void __RPC_STUB IMSMQPrivateDestination_put_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMSMQPrivateDestination_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_MSMQDestination,0xeba96b18,0x2168,0x11d3,0x89,0x8c,0x00,0xe0,0x2c,0x07,0x4f,0x6b);

#ifdef __cplusplus

class DECLSPEC_UUID("eba96b18-2168-11d3-898c-00e02c074f6b")
MSMQDestination;
#endif
#endif /* __MSMQ_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



#endif //__MQOAI_H__

