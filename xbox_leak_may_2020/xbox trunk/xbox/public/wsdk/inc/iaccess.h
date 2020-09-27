
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Sep 24 19:29:14 1999
 */
/* Compiler settings for iaccess.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __iaccess_h__
#define __iaccess_h__

/* Forward Declarations */ 

#ifndef __IAccessControl_FWD_DEFINED__
#define __IAccessControl_FWD_DEFINED__
typedef interface IAccessControl IAccessControl;
#endif 	/* __IAccessControl_FWD_DEFINED__ */


#ifndef __IAuditControl_FWD_DEFINED__
#define __IAuditControl_FWD_DEFINED__
typedef interface IAuditControl IAuditControl;
#endif 	/* __IAuditControl_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "accctrl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_iaccess_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992-1999.
//
//--------------------------------------------------------------------------
#if ( _MSC_VER >= 1020 )
#pragma once
#endif
typedef /* [allocate] */ PACTRL_ACCESSW PACTRL_ACCESSW_ALLOCATE_ALL_NODES;

typedef /* [allocate] */ PACTRL_AUDITW PACTRL_AUDITW_ALLOCATE_ALL_NODES;




extern RPC_IF_HANDLE __MIDL_itf_iaccess_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_iaccess_0000_v0_0_s_ifspec;

#ifndef __IAccessControl_INTERFACE_DEFINED__
#define __IAccessControl_INTERFACE_DEFINED__

/* interface IAccessControl */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAccessControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EEDD23E0-8410-11CE-A1C3-08002B2B8D8F")
    IAccessControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GrantAccessRights( 
            /* [in] */ PACTRL_ACCESSW pAccessList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAccessRights( 
            /* [in] */ PACTRL_ACCESSW pAccessList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOwner( 
            /* [in] */ PTRUSTEEW pOwner,
            /* [in] */ PTRUSTEEW pGroup) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeAccessRights( 
            /* [in] */ LPWSTR lpProperty,
            /* [in] */ ULONG cTrustees,
            /* [size_is][in] */ TRUSTEEW __RPC_FAR prgTrustees[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAllAccessRights( 
            /* [in] */ LPWSTR lpProperty,
            /* [out] */ PACTRL_ACCESSW_ALLOCATE_ALL_NODES __RPC_FAR *ppAccessList,
            /* [out] */ PTRUSTEEW __RPC_FAR *ppOwner,
            /* [out] */ PTRUSTEEW __RPC_FAR *ppGroup) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsAccessAllowed( 
            /* [in] */ PTRUSTEEW pTrustee,
            /* [in] */ LPWSTR lpProperty,
            /* [in] */ ACCESS_RIGHTS AccessRights,
            /* [out] */ BOOL __RPC_FAR *pfAccessAllowed) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAccessControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAccessControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAccessControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GrantAccessRights )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ PACTRL_ACCESSW pAccessList);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAccessRights )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ PACTRL_ACCESSW pAccessList);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetOwner )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ PTRUSTEEW pOwner,
            /* [in] */ PTRUSTEEW pGroup);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RevokeAccessRights )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ LPWSTR lpProperty,
            /* [in] */ ULONG cTrustees,
            /* [size_is][in] */ TRUSTEEW __RPC_FAR prgTrustees[  ]);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllAccessRights )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ LPWSTR lpProperty,
            /* [out] */ PACTRL_ACCESSW_ALLOCATE_ALL_NODES __RPC_FAR *ppAccessList,
            /* [out] */ PTRUSTEEW __RPC_FAR *ppOwner,
            /* [out] */ PTRUSTEEW __RPC_FAR *ppGroup);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsAccessAllowed )( 
            IAccessControl __RPC_FAR * This,
            /* [in] */ PTRUSTEEW pTrustee,
            /* [in] */ LPWSTR lpProperty,
            /* [in] */ ACCESS_RIGHTS AccessRights,
            /* [out] */ BOOL __RPC_FAR *pfAccessAllowed);
        
        END_INTERFACE
    } IAccessControlVtbl;

    interface IAccessControl
    {
        CONST_VTBL struct IAccessControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccessControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAccessControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAccessControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAccessControl_GrantAccessRights(This,pAccessList)	\
    (This)->lpVtbl -> GrantAccessRights(This,pAccessList)

#define IAccessControl_SetAccessRights(This,pAccessList)	\
    (This)->lpVtbl -> SetAccessRights(This,pAccessList)

#define IAccessControl_SetOwner(This,pOwner,pGroup)	\
    (This)->lpVtbl -> SetOwner(This,pOwner,pGroup)

#define IAccessControl_RevokeAccessRights(This,lpProperty,cTrustees,prgTrustees)	\
    (This)->lpVtbl -> RevokeAccessRights(This,lpProperty,cTrustees,prgTrustees)

#define IAccessControl_GetAllAccessRights(This,lpProperty,ppAccessList,ppOwner,ppGroup)	\
    (This)->lpVtbl -> GetAllAccessRights(This,lpProperty,ppAccessList,ppOwner,ppGroup)

#define IAccessControl_IsAccessAllowed(This,pTrustee,lpProperty,AccessRights,pfAccessAllowed)	\
    (This)->lpVtbl -> IsAccessAllowed(This,pTrustee,lpProperty,AccessRights,pfAccessAllowed)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAccessControl_GrantAccessRights_Proxy( 
    IAccessControl __RPC_FAR * This,
    /* [in] */ PACTRL_ACCESSW pAccessList);


void __RPC_STUB IAccessControl_GrantAccessRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccessControl_SetAccessRights_Proxy( 
    IAccessControl __RPC_FAR * This,
    /* [in] */ PACTRL_ACCESSW pAccessList);


void __RPC_STUB IAccessControl_SetAccessRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccessControl_SetOwner_Proxy( 
    IAccessControl __RPC_FAR * This,
    /* [in] */ PTRUSTEEW pOwner,
    /* [in] */ PTRUSTEEW pGroup);


void __RPC_STUB IAccessControl_SetOwner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccessControl_RevokeAccessRights_Proxy( 
    IAccessControl __RPC_FAR * This,
    /* [in] */ LPWSTR lpProperty,
    /* [in] */ ULONG cTrustees,
    /* [size_is][in] */ TRUSTEEW __RPC_FAR prgTrustees[  ]);


void __RPC_STUB IAccessControl_RevokeAccessRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccessControl_GetAllAccessRights_Proxy( 
    IAccessControl __RPC_FAR * This,
    /* [in] */ LPWSTR lpProperty,
    /* [out] */ PACTRL_ACCESSW_ALLOCATE_ALL_NODES __RPC_FAR *ppAccessList,
    /* [out] */ PTRUSTEEW __RPC_FAR *ppOwner,
    /* [out] */ PTRUSTEEW __RPC_FAR *ppGroup);


void __RPC_STUB IAccessControl_GetAllAccessRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccessControl_IsAccessAllowed_Proxy( 
    IAccessControl __RPC_FAR * This,
    /* [in] */ PTRUSTEEW pTrustee,
    /* [in] */ LPWSTR lpProperty,
    /* [in] */ ACCESS_RIGHTS AccessRights,
    /* [out] */ BOOL __RPC_FAR *pfAccessAllowed);


void __RPC_STUB IAccessControl_IsAccessAllowed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAccessControl_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_iaccess_0010 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_iaccess_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_iaccess_0010_v0_0_s_ifspec;

#ifndef __IAuditControl_INTERFACE_DEFINED__
#define __IAuditControl_INTERFACE_DEFINED__

/* interface IAuditControl */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAuditControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1da6292f-bc66-11ce-aae3-00aa004c2737")
    IAuditControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GrantAuditRights( 
            /* [in] */ PACTRL_AUDITW pAuditList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAuditRights( 
            /* [in] */ PACTRL_AUDITW pAuditList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeAuditRights( 
            /* [in] */ LPWSTR lpProperty,
            /* [in] */ ULONG cTrustees,
            /* [size_is][in] */ TRUSTEEW __RPC_FAR prgTrustees[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAllAuditRights( 
            /* [in] */ LPWSTR lpProperty,
            /* [out] */ PACTRL_AUDITW __RPC_FAR *ppAuditList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsAccessAudited( 
            /* [in] */ PTRUSTEEW pTrustee,
            /* [in] */ ACCESS_RIGHTS AuditRights,
            /* [out] */ BOOL __RPC_FAR *pfAccessAudited) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAuditControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAuditControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAuditControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAuditControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GrantAuditRights )( 
            IAuditControl __RPC_FAR * This,
            /* [in] */ PACTRL_AUDITW pAuditList);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAuditRights )( 
            IAuditControl __RPC_FAR * This,
            /* [in] */ PACTRL_AUDITW pAuditList);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RevokeAuditRights )( 
            IAuditControl __RPC_FAR * This,
            /* [in] */ LPWSTR lpProperty,
            /* [in] */ ULONG cTrustees,
            /* [size_is][in] */ TRUSTEEW __RPC_FAR prgTrustees[  ]);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllAuditRights )( 
            IAuditControl __RPC_FAR * This,
            /* [in] */ LPWSTR lpProperty,
            /* [out] */ PACTRL_AUDITW __RPC_FAR *ppAuditList);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsAccessAudited )( 
            IAuditControl __RPC_FAR * This,
            /* [in] */ PTRUSTEEW pTrustee,
            /* [in] */ ACCESS_RIGHTS AuditRights,
            /* [out] */ BOOL __RPC_FAR *pfAccessAudited);
        
        END_INTERFACE
    } IAuditControlVtbl;

    interface IAuditControl
    {
        CONST_VTBL struct IAuditControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAuditControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAuditControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAuditControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAuditControl_GrantAuditRights(This,pAuditList)	\
    (This)->lpVtbl -> GrantAuditRights(This,pAuditList)

#define IAuditControl_SetAuditRights(This,pAuditList)	\
    (This)->lpVtbl -> SetAuditRights(This,pAuditList)

#define IAuditControl_RevokeAuditRights(This,lpProperty,cTrustees,prgTrustees)	\
    (This)->lpVtbl -> RevokeAuditRights(This,lpProperty,cTrustees,prgTrustees)

#define IAuditControl_GetAllAuditRights(This,lpProperty,ppAuditList)	\
    (This)->lpVtbl -> GetAllAuditRights(This,lpProperty,ppAuditList)

#define IAuditControl_IsAccessAudited(This,pTrustee,AuditRights,pfAccessAudited)	\
    (This)->lpVtbl -> IsAccessAudited(This,pTrustee,AuditRights,pfAccessAudited)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAuditControl_GrantAuditRights_Proxy( 
    IAuditControl __RPC_FAR * This,
    /* [in] */ PACTRL_AUDITW pAuditList);


void __RPC_STUB IAuditControl_GrantAuditRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuditControl_SetAuditRights_Proxy( 
    IAuditControl __RPC_FAR * This,
    /* [in] */ PACTRL_AUDITW pAuditList);


void __RPC_STUB IAuditControl_SetAuditRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuditControl_RevokeAuditRights_Proxy( 
    IAuditControl __RPC_FAR * This,
    /* [in] */ LPWSTR lpProperty,
    /* [in] */ ULONG cTrustees,
    /* [size_is][in] */ TRUSTEEW __RPC_FAR prgTrustees[  ]);


void __RPC_STUB IAuditControl_RevokeAuditRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuditControl_GetAllAuditRights_Proxy( 
    IAuditControl __RPC_FAR * This,
    /* [in] */ LPWSTR lpProperty,
    /* [out] */ PACTRL_AUDITW __RPC_FAR *ppAuditList);


void __RPC_STUB IAuditControl_GetAllAuditRights_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuditControl_IsAccessAudited_Proxy( 
    IAuditControl __RPC_FAR * This,
    /* [in] */ PTRUSTEEW pTrustee,
    /* [in] */ ACCESS_RIGHTS AuditRights,
    /* [out] */ BOOL __RPC_FAR *pfAccessAudited);


void __RPC_STUB IAuditControl_IsAccessAudited_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAuditControl_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


