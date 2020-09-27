
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sat Jan 05 01:42:47 2002
 */
/* Compiler settings for proffserv.idl:
    Oicf, W0, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data , no_format_optimization
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

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __proffserv_h__
#define __proffserv_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IProfferService_FWD_DEFINED__
#define __IProfferService_FWD_DEFINED__
typedef interface IProfferService IProfferService;
#endif 	/* __IProfferService_FWD_DEFINED__ */


/* header files for imported files */
#include "servprov.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_proffserv_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright 1995 - 1997 Microsoft Corporation. All Rights Reserved.
//
//  File: proffserv.h
//
//--------------------------------------------------------------------------
#ifndef _PROFFSERV_H_
#define _PROFFSERV_H_


extern RPC_IF_HANDLE __MIDL_itf_proffserv_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_proffserv_0000_v0_0_s_ifspec;

#ifndef __IProfferService_INTERFACE_DEFINED__
#define __IProfferService_INTERFACE_DEFINED__

/* interface IProfferService */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_IProfferService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB728B20-F786-11ce-92AD-00AA00A74CD0")
    IProfferService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ProfferService( 
            /* [in] */ REFGUID rguidService,
            /* [in] */ IServiceProvider *psp,
            /* [out] */ DWORD *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeService( 
            /* [in] */ DWORD dwCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProfferServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfferService * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfferService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfferService * This);
        
        HRESULT ( STDMETHODCALLTYPE *ProfferService )( 
            IProfferService * This,
            /* [in] */ REFGUID rguidService,
            /* [in] */ IServiceProvider *psp,
            /* [out] */ DWORD *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RevokeService )( 
            IProfferService * This,
            /* [in] */ DWORD dwCookie);
        
        END_INTERFACE
    } IProfferServiceVtbl;

    interface IProfferService
    {
        CONST_VTBL struct IProfferServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfferService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProfferService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProfferService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProfferService_ProfferService(This,rguidService,psp,pdwCookie)	\
    (This)->lpVtbl -> ProfferService(This,rguidService,psp,pdwCookie)

#define IProfferService_RevokeService(This,dwCookie)	\
    (This)->lpVtbl -> RevokeService(This,dwCookie)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IProfferService_ProfferService_Proxy( 
    IProfferService * This,
    /* [in] */ REFGUID rguidService,
    /* [in] */ IServiceProvider *psp,
    /* [out] */ DWORD *pdwCookie);


void __RPC_STUB IProfferService_ProfferService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IProfferService_RevokeService_Proxy( 
    IProfferService * This,
    /* [in] */ DWORD dwCookie);


void __RPC_STUB IProfferService_RevokeService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProfferService_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_proffserv_0090 */
/* [local] */ 

#define SID_SProfferService IID_IProfferService
#endif


extern RPC_IF_HANDLE __MIDL_itf_proffserv_0090_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_proffserv_0090_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


