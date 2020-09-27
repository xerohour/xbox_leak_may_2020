
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun Apr 07 15:08:27 2002
 */
/* Compiler settings for rpcsig.idl:
    Oi, W3, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, app_config, c_ext
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

#ifndef __rpcsig_h__
#define __rpcsig_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __RpcSignatureServerInterface_INTERFACE_DEFINED__
#define __RpcSignatureServerInterface_INTERFACE_DEFINED__

/* interface RpcSignatureServerInterface */
/* [auto_handle][version][uuid] */ 

boolean RpcSignatureServerAuthenticate( 
    /* [in] */ handle_t BindingHandle,
    /* [string][in] */ unsigned char *LoggingText);

boolean RpcSignatureServerGenSignature( 
    /* [in] */ handle_t BindingHandle,
    /* [string][in] */ unsigned char *LoggingText,
    /* [size_is][in] */ unsigned char *DataHashValue,
    /* [out] */ unsigned long *SignatureId,
    /* [size_is][out] */ unsigned char *Signature);



extern RPC_IF_HANDLE RpcSignatureServerInterface_v2_0_c_ifspec;
extern RPC_IF_HANDLE RpcSignatureServerInterface_v2_0_s_ifspec;
#endif /* __RpcSignatureServerInterface_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


