////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/*++

Copyright 1991-1998 Microsoft Corporation

Module Name:

    rpc.h

Abstract:

    Master include file for RPC applications.

--*/

#if !defined
//#include <windows.h>
#endif // RPC_NO_WINDOWS_H

#ifndef __RPC_H__
#define __RPC_H__

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------------
// platform specific defines
//-------------------------------------------------------------

//-------------------  MAC ---------------------------
#if defined

#define __RPC_MAC__
// Set the packing level for RPC structures.

#include <pshpack2.h>

//-------------------  WIN32 ---------------------------
#else

#define __RPC_WIN32__

#endif


#ifndef __MIDL_USER_DEFINED
#define midl_user_allocate MIDL_user_allocate
#define midl_user_free     MIDL_user_free
#define __MIDL_USER_DEFINED
#endif

typedef void * I_RPC_HANDLE;
typedef long RPC_STATUS;

#ifndef _MAC
#define RPC_UNICODE_SUPPORTED
#endif //_MAC


#ifdef __RPC_MAC__
#	define __RPC_FAR
#	define __RPC_API
#	define __RPC_USER
#	define __RPC_STUB
#	define RPC_ENTRY
#elif _MSC_VER>=800 || defined
#	define __RPC_FAR
#	define __RPC_API  __stdcall
#	define __RPC_USER __stdcall
#	define __RPC_STUB __stdcall
#	define  RPC_ENTRY __stdcall
#else
#	define __RPC_FAR
#	define __RPC_API
#	define __RPC_USER
#	define __RPC_STUB
#endif

// Some RPC platforms don't define DECLSPEC_IMPORT
#if !defined
#if defined
#define DECLSPEC_IMPORT __declspec(dllimport)
#else
#define DECLSPEC_IMPORT
#endif
#endif

#if !defined
#define RPCRTAPI DECLSPEC_IMPORT
#else
#define RPCRTAPI
#endif

#if !defined
#define RPCNSAPI DECLSPEC_IMPORT
#else
#define RPCNSAPI
#endif

#ifdef IN
#undef IN
#undef OUT
#undef OPTIONAL
#endif /* IN */

#ifdef __RPC_MAC__

#include <setjmp.h>

#define RPCXCWORD (sizeof(jmp_buf)/sizeof(int))

#pragma warning( disable: 4005 )
#include <rpcdce.h>
#include <rpcnsi.h>
#include <rpcerr.h>
#include <rpcmac.h>
#pragma warning( default :  4005 )

typedef void  (RPC_ENTRY *MACYIELDCALLBACK)(/*OSErr*/ short *) ;
RPC_STATUS RPC_ENTRY
RpcMacSetYieldInfo(
	MACYIELDCALLBACK pfnCallback) ;

#if !defined
#define UNALIGNED
#endif

#include <poppack.h>

#else

#include <rpcdce.h>
#include <rpcnsi.h>
#include <rpcnterr.h>
#include <excpt.h>
#include <winerror.h>

#define RpcTryExcept \
    __try \
        {

// trystmts

#define RpcExcept(expr) \
        } \
    __except (expr) \
        {

// exceptstmts

#define RpcEndExcept \
        }

#define RpcTryFinally \
    __try \
        {

// trystmts

#define RpcFinally \
        } \
    __finally \
        {

// finallystmts

#define RpcEndFinally \
        }

#define RpcExceptionCode() GetExceptionCode()
#define RpcAbnormalTermination() AbnormalTermination()

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcImpersonateClient (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcRevertToSelfEx (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcRevertToSelf (
    );

RPCRTAPI
long
RPC_ENTRY
I_RpcMapWin32Status (
    IN RPC_STATUS Status
    );

#endif // __RPC_MAC__

// Definitions which depend on windows.h
#if !defined

#include <rpcasync.h>

#endif // RPC_NO_WINDOWS_H

#ifdef __cplusplus
}
#endif

#endif // __RPC_H__

