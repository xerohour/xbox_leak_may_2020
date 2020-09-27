
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0268 */
/* at Mon Jul 12 11:07:24 1999
 */
/* Compiler settings for autosvcs.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
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

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __autosvcs_h__
#define __autosvcs_h__

/* Forward Declarations */ 

#ifndef __ISecurityIdentityColl_FWD_DEFINED__
#define __ISecurityIdentityColl_FWD_DEFINED__
typedef interface ISecurityIdentityColl ISecurityIdentityColl;
#endif 	/* __ISecurityIdentityColl_FWD_DEFINED__ */


#ifndef __ISecurityCallersColl_FWD_DEFINED__
#define __ISecurityCallersColl_FWD_DEFINED__
typedef interface ISecurityCallersColl ISecurityCallersColl;
#endif 	/* __ISecurityCallersColl_FWD_DEFINED__ */


#ifndef __ISecurityCallContext_FWD_DEFINED__
#define __ISecurityCallContext_FWD_DEFINED__
typedef interface ISecurityCallContext ISecurityCallContext;
#endif 	/* __ISecurityCallContext_FWD_DEFINED__ */


#ifndef __IGetSecurityCallContext_FWD_DEFINED__
#define __IGetSecurityCallContext_FWD_DEFINED__
typedef interface IGetSecurityCallContext IGetSecurityCallContext;
#endif 	/* __IGetSecurityCallContext_FWD_DEFINED__ */


#ifndef __SecurityProperty_FWD_DEFINED__
#define __SecurityProperty_FWD_DEFINED__
typedef interface SecurityProperty SecurityProperty;
#endif 	/* __SecurityProperty_FWD_DEFINED__ */


#ifndef __ContextInfo_FWD_DEFINED__
#define __ContextInfo_FWD_DEFINED__
typedef interface ContextInfo ContextInfo;
#endif 	/* __ContextInfo_FWD_DEFINED__ */


#ifndef __ObjectContext_FWD_DEFINED__
#define __ObjectContext_FWD_DEFINED__
typedef interface ObjectContext ObjectContext;
#endif 	/* __ObjectContext_FWD_DEFINED__ */


#ifndef __ITransactionContextEx_FWD_DEFINED__
#define __ITransactionContextEx_FWD_DEFINED__
typedef interface ITransactionContextEx ITransactionContextEx;
#endif 	/* __ITransactionContextEx_FWD_DEFINED__ */


#ifndef __ITransactionContext_FWD_DEFINED__
#define __ITransactionContext_FWD_DEFINED__
typedef interface ITransactionContext ITransactionContext;
#endif 	/* __ITransactionContext_FWD_DEFINED__ */


#ifndef __ICreateWithTransactionEx_FWD_DEFINED__
#define __ICreateWithTransactionEx_FWD_DEFINED__
typedef interface ICreateWithTransactionEx ICreateWithTransactionEx;
#endif 	/* __ICreateWithTransactionEx_FWD_DEFINED__ */


#ifndef __ICreateWithTipTransactionEx_FWD_DEFINED__
#define __ICreateWithTipTransactionEx_FWD_DEFINED__
typedef interface ICreateWithTipTransactionEx ICreateWithTipTransactionEx;
#endif 	/* __ICreateWithTipTransactionEx_FWD_DEFINED__ */


#ifndef __IComUserEvent_FWD_DEFINED__
#define __IComUserEvent_FWD_DEFINED__
typedef interface IComUserEvent IComUserEvent;
#endif 	/* __IComUserEvent_FWD_DEFINED__ */


#ifndef __IComThreadEvents_FWD_DEFINED__
#define __IComThreadEvents_FWD_DEFINED__
typedef interface IComThreadEvents IComThreadEvents;
#endif 	/* __IComThreadEvents_FWD_DEFINED__ */


#ifndef __IComAppEvents_FWD_DEFINED__
#define __IComAppEvents_FWD_DEFINED__
typedef interface IComAppEvents IComAppEvents;
#endif 	/* __IComAppEvents_FWD_DEFINED__ */


#ifndef __IComInstanceEvents_FWD_DEFINED__
#define __IComInstanceEvents_FWD_DEFINED__
typedef interface IComInstanceEvents IComInstanceEvents;
#endif 	/* __IComInstanceEvents_FWD_DEFINED__ */


#ifndef __IComTransactionEvents_FWD_DEFINED__
#define __IComTransactionEvents_FWD_DEFINED__
typedef interface IComTransactionEvents IComTransactionEvents;
#endif 	/* __IComTransactionEvents_FWD_DEFINED__ */


#ifndef __IComMethodEvents_FWD_DEFINED__
#define __IComMethodEvents_FWD_DEFINED__
typedef interface IComMethodEvents IComMethodEvents;
#endif 	/* __IComMethodEvents_FWD_DEFINED__ */


#ifndef __IComObjectEvents_FWD_DEFINED__
#define __IComObjectEvents_FWD_DEFINED__
typedef interface IComObjectEvents IComObjectEvents;
#endif 	/* __IComObjectEvents_FWD_DEFINED__ */


#ifndef __IComResourceEvents_FWD_DEFINED__
#define __IComResourceEvents_FWD_DEFINED__
typedef interface IComResourceEvents IComResourceEvents;
#endif 	/* __IComResourceEvents_FWD_DEFINED__ */


#ifndef __IComSecurityEvents_FWD_DEFINED__
#define __IComSecurityEvents_FWD_DEFINED__
typedef interface IComSecurityEvents IComSecurityEvents;
#endif 	/* __IComSecurityEvents_FWD_DEFINED__ */


#ifndef __IComObjectPoolEvents_FWD_DEFINED__
#define __IComObjectPoolEvents_FWD_DEFINED__
typedef interface IComObjectPoolEvents IComObjectPoolEvents;
#endif 	/* __IComObjectPoolEvents_FWD_DEFINED__ */


#ifndef __IComObjectPoolEvents2_FWD_DEFINED__
#define __IComObjectPoolEvents2_FWD_DEFINED__
typedef interface IComObjectPoolEvents2 IComObjectPoolEvents2;
#endif 	/* __IComObjectPoolEvents2_FWD_DEFINED__ */


#ifndef __IComObjectConstructionEvents_FWD_DEFINED__
#define __IComObjectConstructionEvents_FWD_DEFINED__
typedef interface IComObjectConstructionEvents IComObjectConstructionEvents;
#endif 	/* __IComObjectConstructionEvents_FWD_DEFINED__ */


#ifndef __IComActivityEvents_FWD_DEFINED__
#define __IComActivityEvents_FWD_DEFINED__
typedef interface IComActivityEvents IComActivityEvents;
#endif 	/* __IComActivityEvents_FWD_DEFINED__ */


#ifndef __IComIdentityEvents_FWD_DEFINED__
#define __IComIdentityEvents_FWD_DEFINED__
typedef interface IComIdentityEvents IComIdentityEvents;
#endif 	/* __IComIdentityEvents_FWD_DEFINED__ */


#ifndef __IComQCEvents_FWD_DEFINED__
#define __IComQCEvents_FWD_DEFINED__
typedef interface IComQCEvents IComQCEvents;
#endif 	/* __IComQCEvents_FWD_DEFINED__ */


#ifndef __IComExceptionEvents_FWD_DEFINED__
#define __IComExceptionEvents_FWD_DEFINED__
typedef interface IComExceptionEvents IComExceptionEvents;
#endif 	/* __IComExceptionEvents_FWD_DEFINED__ */


#ifndef __ILBEvents_FWD_DEFINED__
#define __ILBEvents_FWD_DEFINED__
typedef interface ILBEvents ILBEvents;
#endif 	/* __ILBEvents_FWD_DEFINED__ */


#ifndef __IComCRMEvents_FWD_DEFINED__
#define __IComCRMEvents_FWD_DEFINED__
typedef interface IComCRMEvents IComCRMEvents;
#endif 	/* __IComCRMEvents_FWD_DEFINED__ */


#ifndef __IMtsEvents_FWD_DEFINED__
#define __IMtsEvents_FWD_DEFINED__
typedef interface IMtsEvents IMtsEvents;
#endif 	/* __IMtsEvents_FWD_DEFINED__ */


#ifndef __IMtsEventInfo_FWD_DEFINED__
#define __IMtsEventInfo_FWD_DEFINED__
typedef interface IMtsEventInfo IMtsEventInfo;
#endif 	/* __IMtsEventInfo_FWD_DEFINED__ */


#ifndef __IMTSLocator_FWD_DEFINED__
#define __IMTSLocator_FWD_DEFINED__
typedef interface IMTSLocator IMTSLocator;
#endif 	/* __IMTSLocator_FWD_DEFINED__ */


#ifndef __IMtsGrp_FWD_DEFINED__
#define __IMtsGrp_FWD_DEFINED__
typedef interface IMtsGrp IMtsGrp;
#endif 	/* __IMtsGrp_FWD_DEFINED__ */


#ifndef __IMessageMover_FWD_DEFINED__
#define __IMessageMover_FWD_DEFINED__
typedef interface IMessageMover IMessageMover;
#endif 	/* __IMessageMover_FWD_DEFINED__ */


#ifndef __IDispenserManager_FWD_DEFINED__
#define __IDispenserManager_FWD_DEFINED__
typedef interface IDispenserManager IDispenserManager;
#endif 	/* __IDispenserManager_FWD_DEFINED__ */


#ifndef __IHolder_FWD_DEFINED__
#define __IHolder_FWD_DEFINED__
typedef interface IHolder IHolder;
#endif 	/* __IHolder_FWD_DEFINED__ */


#ifndef __IDispenserDriver_FWD_DEFINED__
#define __IDispenserDriver_FWD_DEFINED__
typedef interface IDispenserDriver IDispenserDriver;
#endif 	/* __IDispenserDriver_FWD_DEFINED__ */


#ifndef __IObjectContext_FWD_DEFINED__
#define __IObjectContext_FWD_DEFINED__
typedef interface IObjectContext IObjectContext;
#endif 	/* __IObjectContext_FWD_DEFINED__ */


#ifndef __IObjectControl_FWD_DEFINED__
#define __IObjectControl_FWD_DEFINED__
typedef interface IObjectControl IObjectControl;
#endif 	/* __IObjectControl_FWD_DEFINED__ */


#ifndef __IEnumNames_FWD_DEFINED__
#define __IEnumNames_FWD_DEFINED__
typedef interface IEnumNames IEnumNames;
#endif 	/* __IEnumNames_FWD_DEFINED__ */


#ifndef __ISecurityProperty_FWD_DEFINED__
#define __ISecurityProperty_FWD_DEFINED__
typedef interface ISecurityProperty ISecurityProperty;
#endif 	/* __ISecurityProperty_FWD_DEFINED__ */


#ifndef __ObjectControl_FWD_DEFINED__
#define __ObjectControl_FWD_DEFINED__
typedef interface ObjectControl ObjectControl;
#endif 	/* __ObjectControl_FWD_DEFINED__ */


#ifndef __ISharedProperty_FWD_DEFINED__
#define __ISharedProperty_FWD_DEFINED__
typedef interface ISharedProperty ISharedProperty;
#endif 	/* __ISharedProperty_FWD_DEFINED__ */


#ifndef __ISharedPropertyGroup_FWD_DEFINED__
#define __ISharedPropertyGroup_FWD_DEFINED__
typedef interface ISharedPropertyGroup ISharedPropertyGroup;
#endif 	/* __ISharedPropertyGroup_FWD_DEFINED__ */


#ifndef __ISharedPropertyGroupManager_FWD_DEFINED__
#define __ISharedPropertyGroupManager_FWD_DEFINED__
typedef interface ISharedPropertyGroupManager ISharedPropertyGroupManager;
#endif 	/* __ISharedPropertyGroupManager_FWD_DEFINED__ */


#ifndef __IObjectConstruct_FWD_DEFINED__
#define __IObjectConstruct_FWD_DEFINED__
typedef interface IObjectConstruct IObjectConstruct;
#endif 	/* __IObjectConstruct_FWD_DEFINED__ */


#ifndef __IObjectConstructString_FWD_DEFINED__
#define __IObjectConstructString_FWD_DEFINED__
typedef interface IObjectConstructString IObjectConstructString;
#endif 	/* __IObjectConstructString_FWD_DEFINED__ */


#ifndef __IObjectContextActivity_FWD_DEFINED__
#define __IObjectContextActivity_FWD_DEFINED__
typedef interface IObjectContextActivity IObjectContextActivity;
#endif 	/* __IObjectContextActivity_FWD_DEFINED__ */


#ifndef __IObjectContextInfo_FWD_DEFINED__
#define __IObjectContextInfo_FWD_DEFINED__
typedef interface IObjectContextInfo IObjectContextInfo;
#endif 	/* __IObjectContextInfo_FWD_DEFINED__ */


#ifndef __IObjectContextTip_FWD_DEFINED__
#define __IObjectContextTip_FWD_DEFINED__
typedef interface IObjectContextTip IObjectContextTip;
#endif 	/* __IObjectContextTip_FWD_DEFINED__ */


#ifndef __IPlaybackControl_FWD_DEFINED__
#define __IPlaybackControl_FWD_DEFINED__
typedef interface IPlaybackControl IPlaybackControl;
#endif 	/* __IPlaybackControl_FWD_DEFINED__ */


#ifndef __IGetContextProperties_FWD_DEFINED__
#define __IGetContextProperties_FWD_DEFINED__
typedef interface IGetContextProperties IGetContextProperties;
#endif 	/* __IGetContextProperties_FWD_DEFINED__ */


#ifndef __IContextState_FWD_DEFINED__
#define __IContextState_FWD_DEFINED__
typedef interface IContextState IContextState;
#endif 	/* __IContextState_FWD_DEFINED__ */


#ifndef __ICrmLogControl_FWD_DEFINED__
#define __ICrmLogControl_FWD_DEFINED__
typedef interface ICrmLogControl ICrmLogControl;
#endif 	/* __ICrmLogControl_FWD_DEFINED__ */


#ifndef __ICrmCompensatorVariants_FWD_DEFINED__
#define __ICrmCompensatorVariants_FWD_DEFINED__
typedef interface ICrmCompensatorVariants ICrmCompensatorVariants;
#endif 	/* __ICrmCompensatorVariants_FWD_DEFINED__ */


#ifndef __ICrmCompensator_FWD_DEFINED__
#define __ICrmCompensator_FWD_DEFINED__
typedef interface ICrmCompensator ICrmCompensator;
#endif 	/* __ICrmCompensator_FWD_DEFINED__ */


#ifndef __ICrmMonitorLogRecords_FWD_DEFINED__
#define __ICrmMonitorLogRecords_FWD_DEFINED__
typedef interface ICrmMonitorLogRecords ICrmMonitorLogRecords;
#endif 	/* __ICrmMonitorLogRecords_FWD_DEFINED__ */


#ifndef __ICrmMonitorClerks_FWD_DEFINED__
#define __ICrmMonitorClerks_FWD_DEFINED__
typedef interface ICrmMonitorClerks ICrmMonitorClerks;
#endif 	/* __ICrmMonitorClerks_FWD_DEFINED__ */


#ifndef __ICrmMonitor_FWD_DEFINED__
#define __ICrmMonitor_FWD_DEFINED__
typedef interface ICrmMonitor ICrmMonitor;
#endif 	/* __ICrmMonitor_FWD_DEFINED__ */


#ifndef __ICrmFormatLogRecords_FWD_DEFINED__
#define __ICrmFormatLogRecords_FWD_DEFINED__
typedef interface ICrmFormatLogRecords ICrmFormatLogRecords;
#endif 	/* __ICrmFormatLogRecords_FWD_DEFINED__ */


#ifndef __SecurityIdentity_FWD_DEFINED__
#define __SecurityIdentity_FWD_DEFINED__

#ifdef __cplusplus
typedef class SecurityIdentity SecurityIdentity;
#else
typedef struct SecurityIdentity SecurityIdentity;
#endif /* __cplusplus */

#endif 	/* __SecurityIdentity_FWD_DEFINED__ */


#ifndef __SecurityCallers_FWD_DEFINED__
#define __SecurityCallers_FWD_DEFINED__

#ifdef __cplusplus
typedef class SecurityCallers SecurityCallers;
#else
typedef struct SecurityCallers SecurityCallers;
#endif /* __cplusplus */

#endif 	/* __SecurityCallers_FWD_DEFINED__ */


#ifndef __SecurityCallContext_FWD_DEFINED__
#define __SecurityCallContext_FWD_DEFINED__

#ifdef __cplusplus
typedef class SecurityCallContext SecurityCallContext;
#else
typedef struct SecurityCallContext SecurityCallContext;
#endif /* __cplusplus */

#endif 	/* __SecurityCallContext_FWD_DEFINED__ */


#ifndef __GetSecurityCallContextAppObject_FWD_DEFINED__
#define __GetSecurityCallContextAppObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class GetSecurityCallContextAppObject GetSecurityCallContextAppObject;
#else
typedef struct GetSecurityCallContextAppObject GetSecurityCallContextAppObject;
#endif /* __cplusplus */

#endif 	/* __GetSecurityCallContextAppObject_FWD_DEFINED__ */


#ifndef __IContextState_FWD_DEFINED__
#define __IContextState_FWD_DEFINED__
typedef interface IContextState IContextState;
#endif 	/* __IContextState_FWD_DEFINED__ */


#ifndef __Dummy30040732_FWD_DEFINED__
#define __Dummy30040732_FWD_DEFINED__

#ifdef __cplusplus
typedef class Dummy30040732 Dummy30040732;
#else
typedef struct Dummy30040732 Dummy30040732;
#endif /* __cplusplus */

#endif 	/* __Dummy30040732_FWD_DEFINED__ */


#ifndef __ObjectControl_FWD_DEFINED__
#define __ObjectControl_FWD_DEFINED__
typedef interface ObjectControl ObjectControl;
#endif 	/* __ObjectControl_FWD_DEFINED__ */


#ifndef __TransactionContext_FWD_DEFINED__
#define __TransactionContext_FWD_DEFINED__

#ifdef __cplusplus
typedef class TransactionContext TransactionContext;
#else
typedef struct TransactionContext TransactionContext;
#endif /* __cplusplus */

#endif 	/* __TransactionContext_FWD_DEFINED__ */


#ifndef __TransactionContextEx_FWD_DEFINED__
#define __TransactionContextEx_FWD_DEFINED__

#ifdef __cplusplus
typedef class TransactionContextEx TransactionContextEx;
#else
typedef struct TransactionContextEx TransactionContextEx;
#endif /* __cplusplus */

#endif 	/* __TransactionContextEx_FWD_DEFINED__ */


#ifndef __SharedProperty_FWD_DEFINED__
#define __SharedProperty_FWD_DEFINED__

#ifdef __cplusplus
typedef class SharedProperty SharedProperty;
#else
typedef struct SharedProperty SharedProperty;
#endif /* __cplusplus */

#endif 	/* __SharedProperty_FWD_DEFINED__ */


#ifndef __SharedPropertyGroup_FWD_DEFINED__
#define __SharedPropertyGroup_FWD_DEFINED__

#ifdef __cplusplus
typedef class SharedPropertyGroup SharedPropertyGroup;
#else
typedef struct SharedPropertyGroup SharedPropertyGroup;
#endif /* __cplusplus */

#endif 	/* __SharedPropertyGroup_FWD_DEFINED__ */


#ifndef __SharedPropertyGroupManager_FWD_DEFINED__
#define __SharedPropertyGroupManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class SharedPropertyGroupManager SharedPropertyGroupManager;
#else
typedef struct SharedPropertyGroupManager SharedPropertyGroupManager;
#endif /* __cplusplus */

#endif 	/* __SharedPropertyGroupManager_FWD_DEFINED__ */


#ifndef __COMEvents_FWD_DEFINED__
#define __COMEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class COMEvents COMEvents;
#else
typedef struct COMEvents COMEvents;
#endif /* __cplusplus */

#endif 	/* __COMEvents_FWD_DEFINED__ */


#ifndef __CoMTSLocator_FWD_DEFINED__
#define __CoMTSLocator_FWD_DEFINED__

#ifdef __cplusplus
typedef class CoMTSLocator CoMTSLocator;
#else
typedef struct CoMTSLocator CoMTSLocator;
#endif /* __cplusplus */

#endif 	/* __CoMTSLocator_FWD_DEFINED__ */


#ifndef __MtsGrp_FWD_DEFINED__
#define __MtsGrp_FWD_DEFINED__

#ifdef __cplusplus
typedef class MtsGrp MtsGrp;
#else
typedef struct MtsGrp MtsGrp;
#endif /* __cplusplus */

#endif 	/* __MtsGrp_FWD_DEFINED__ */


#ifndef __ComServiceEvents_FWD_DEFINED__
#define __ComServiceEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class ComServiceEvents ComServiceEvents;
#else
typedef struct ComServiceEvents ComServiceEvents;
#endif /* __cplusplus */

#endif 	/* __ComServiceEvents_FWD_DEFINED__ */


#ifndef __CRMClerk_FWD_DEFINED__
#define __CRMClerk_FWD_DEFINED__

#ifdef __cplusplus
typedef class CRMClerk CRMClerk;
#else
typedef struct CRMClerk CRMClerk;
#endif /* __cplusplus */

#endif 	/* __CRMClerk_FWD_DEFINED__ */


#ifndef __CRMRecoveryClerk_FWD_DEFINED__
#define __CRMRecoveryClerk_FWD_DEFINED__

#ifdef __cplusplus
typedef class CRMRecoveryClerk CRMRecoveryClerk;
#else
typedef struct CRMRecoveryClerk CRMRecoveryClerk;
#endif /* __cplusplus */

#endif 	/* __CRMRecoveryClerk_FWD_DEFINED__ */


#ifndef __LBEvents_FWD_DEFINED__
#define __LBEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class LBEvents LBEvents;
#else
typedef struct LBEvents LBEvents;
#endif /* __cplusplus */

#endif 	/* __LBEvents_FWD_DEFINED__ */


#ifndef __MessageMover_FWD_DEFINED__
#define __MessageMover_FWD_DEFINED__

#ifdef __cplusplus
typedef class MessageMover MessageMover;
#else
typedef struct MessageMover MessageMover;
#endif /* __cplusplus */

#endif 	/* __MessageMover_FWD_DEFINED__ */


#ifndef __DispenserManager_FWD_DEFINED__
#define __DispenserManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class DispenserManager DispenserManager;
#else
typedef struct DispenserManager DispenserManager;
#endif /* __cplusplus */

#endif 	/* __DispenserManager_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "oaidl.h"
#include "ocidl.h"
#include "transact.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_autosvcs_0000 */
/* [local] */ 

// -----------------------------------------------------------------------
// svcintfs.h -- Microsoft COM+ Services 1.0 Programming Interfaces       
//                                                                        
// This file provides the prototypes for the APIs and COM interfaces      
// for applications using COM+ Services.                                  
//                                                                        
// COM+ Services 1.0                                                      
//  Copyright (C) 1995-1999 Microsoft Corporation.  All rights reserved.
// -----------------------------------------------------------------------
#include <objbase.h>
#ifndef DECLSPEC_UUID
#if _MSC_VER >= 1100
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif



extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0000_v0_0_s_ifspec;

#ifndef __ISecurityIdentityColl_INTERFACE_DEFINED__
#define __ISecurityIdentityColl_INTERFACE_DEFINED__

/* interface ISecurityIdentityColl */
/* [unique][helpcontext][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_ISecurityIdentityColl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CAFC823C-B441-11d1-B82B-0000F8757E2A")
    ISecurityIdentityColl : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][helpcontext][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual /* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISecurityIdentityCollVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISecurityIdentityColl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISecurityIdentityColl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        /* [helpstring][helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem);
        
        /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISecurityIdentityColl __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } ISecurityIdentityCollVtbl;

    interface ISecurityIdentityColl
    {
        CONST_VTBL struct ISecurityIdentityCollVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISecurityIdentityColl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISecurityIdentityColl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISecurityIdentityColl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISecurityIdentityColl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISecurityIdentityColl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISecurityIdentityColl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISecurityIdentityColl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISecurityIdentityColl_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define ISecurityIdentityColl_get_Item(This,name,pItem)	\
    (This)->lpVtbl -> get_Item(This,name,pItem)

#define ISecurityIdentityColl_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISecurityIdentityColl_get_Count_Proxy( 
    ISecurityIdentityColl __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB ISecurityIdentityColl_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE ISecurityIdentityColl_get_Item_Proxy( 
    ISecurityIdentityColl __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT __RPC_FAR *pItem);


void __RPC_STUB ISecurityIdentityColl_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE ISecurityIdentityColl_get__NewEnum_Proxy( 
    ISecurityIdentityColl __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB ISecurityIdentityColl_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISecurityIdentityColl_INTERFACE_DEFINED__ */


#ifndef __ISecurityCallersColl_INTERFACE_DEFINED__
#define __ISecurityCallersColl_INTERFACE_DEFINED__

/* interface ISecurityCallersColl */
/* [unique][helpcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISecurityCallersColl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CAFC823D-B441-11d1-B82B-0000F8757E2A")
    ISecurityCallersColl : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][helpcontext][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual /* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long lIndex,
            /* [retval][out] */ ISecurityIdentityColl __RPC_FAR *__RPC_FAR *pObj) = 0;
        
        virtual /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISecurityCallersCollVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISecurityCallersColl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISecurityCallersColl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        /* [helpstring][helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [in] */ long lIndex,
            /* [retval][out] */ ISecurityIdentityColl __RPC_FAR *__RPC_FAR *pObj);
        
        /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISecurityCallersColl __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } ISecurityCallersCollVtbl;

    interface ISecurityCallersColl
    {
        CONST_VTBL struct ISecurityCallersCollVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISecurityCallersColl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISecurityCallersColl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISecurityCallersColl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISecurityCallersColl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISecurityCallersColl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISecurityCallersColl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISecurityCallersColl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISecurityCallersColl_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define ISecurityCallersColl_get_Item(This,lIndex,pObj)	\
    (This)->lpVtbl -> get_Item(This,lIndex,pObj)

#define ISecurityCallersColl_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallersColl_get_Count_Proxy( 
    ISecurityCallersColl __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB ISecurityCallersColl_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallersColl_get_Item_Proxy( 
    ISecurityCallersColl __RPC_FAR * This,
    /* [in] */ long lIndex,
    /* [retval][out] */ ISecurityIdentityColl __RPC_FAR *__RPC_FAR *pObj);


void __RPC_STUB ISecurityCallersColl_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallersColl_get__NewEnum_Proxy( 
    ISecurityCallersColl __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB ISecurityCallersColl_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISecurityCallersColl_INTERFACE_DEFINED__ */


#ifndef __ISecurityCallContext_INTERFACE_DEFINED__
#define __ISecurityCallContext_INTERFACE_DEFINED__

/* interface ISecurityCallContext */
/* [unique][helpcontext][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_ISecurityCallContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CAFC823E-B441-11d1-B82B-0000F8757E2A")
    ISecurityCallContext : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][helpcontext][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual /* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsCallerInRole( 
            BSTR bstrRole,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfInRole) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsSecurityEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsEnabled) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsUserInRole( 
            /* [in] */ VARIANT __RPC_FAR *pUser,
            /* [in] */ BSTR bstrRole,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfInRole) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISecurityCallContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISecurityCallContext __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISecurityCallContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        /* [helpstring][helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem);
        
        /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsCallerInRole )( 
            ISecurityCallContext __RPC_FAR * This,
            BSTR bstrRole,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfInRole);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsSecurityEnabled )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsEnabled);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsUserInRole )( 
            ISecurityCallContext __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pUser,
            /* [in] */ BSTR bstrRole,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfInRole);
        
        END_INTERFACE
    } ISecurityCallContextVtbl;

    interface ISecurityCallContext
    {
        CONST_VTBL struct ISecurityCallContextVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISecurityCallContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISecurityCallContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISecurityCallContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISecurityCallContext_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISecurityCallContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISecurityCallContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISecurityCallContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISecurityCallContext_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define ISecurityCallContext_get_Item(This,name,pItem)	\
    (This)->lpVtbl -> get_Item(This,name,pItem)

#define ISecurityCallContext_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#define ISecurityCallContext_IsCallerInRole(This,bstrRole,pfInRole)	\
    (This)->lpVtbl -> IsCallerInRole(This,bstrRole,pfInRole)

#define ISecurityCallContext_IsSecurityEnabled(This,pfIsEnabled)	\
    (This)->lpVtbl -> IsSecurityEnabled(This,pfIsEnabled)

#define ISecurityCallContext_IsUserInRole(This,pUser,bstrRole,pfInRole)	\
    (This)->lpVtbl -> IsUserInRole(This,pUser,bstrRole,pfInRole)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallContext_get_Count_Proxy( 
    ISecurityCallContext __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB ISecurityCallContext_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallContext_get_Item_Proxy( 
    ISecurityCallContext __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT __RPC_FAR *pItem);


void __RPC_STUB ISecurityCallContext_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallContext_get__NewEnum_Proxy( 
    ISecurityCallContext __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB ISecurityCallContext_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallContext_IsCallerInRole_Proxy( 
    ISecurityCallContext __RPC_FAR * This,
    BSTR bstrRole,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfInRole);


void __RPC_STUB ISecurityCallContext_IsCallerInRole_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallContext_IsSecurityEnabled_Proxy( 
    ISecurityCallContext __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfIsEnabled);


void __RPC_STUB ISecurityCallContext_IsSecurityEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISecurityCallContext_IsUserInRole_Proxy( 
    ISecurityCallContext __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pUser,
    /* [in] */ BSTR bstrRole,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfInRole);


void __RPC_STUB ISecurityCallContext_IsUserInRole_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISecurityCallContext_INTERFACE_DEFINED__ */


#ifndef __IGetSecurityCallContext_INTERFACE_DEFINED__
#define __IGetSecurityCallContext_INTERFACE_DEFINED__

/* interface IGetSecurityCallContext */
/* [unique][helpcontext][helpstring][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IGetSecurityCallContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CAFC823F-B441-11d1-B82B-0000F8757E2A")
    IGetSecurityCallContext : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetSecurityCallContext( 
            /* [retval][out] */ ISecurityCallContext __RPC_FAR *__RPC_FAR *ppObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGetSecurityCallContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGetSecurityCallContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGetSecurityCallContext __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGetSecurityCallContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGetSecurityCallContext __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGetSecurityCallContext __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGetSecurityCallContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGetSecurityCallContext __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSecurityCallContext )( 
            IGetSecurityCallContext __RPC_FAR * This,
            /* [retval][out] */ ISecurityCallContext __RPC_FAR *__RPC_FAR *ppObject);
        
        END_INTERFACE
    } IGetSecurityCallContextVtbl;

    interface IGetSecurityCallContext
    {
        CONST_VTBL struct IGetSecurityCallContextVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGetSecurityCallContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGetSecurityCallContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGetSecurityCallContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGetSecurityCallContext_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGetSecurityCallContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGetSecurityCallContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGetSecurityCallContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGetSecurityCallContext_GetSecurityCallContext(This,ppObject)	\
    (This)->lpVtbl -> GetSecurityCallContext(This,ppObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IGetSecurityCallContext_GetSecurityCallContext_Proxy( 
    IGetSecurityCallContext __RPC_FAR * This,
    /* [retval][out] */ ISecurityCallContext __RPC_FAR *__RPC_FAR *ppObject);


void __RPC_STUB IGetSecurityCallContext_GetSecurityCallContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGetSecurityCallContext_INTERFACE_DEFINED__ */


#ifndef __SecurityProperty_INTERFACE_DEFINED__
#define __SecurityProperty_INTERFACE_DEFINED__

/* interface SecurityProperty */
/* [unique][helpcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_SecurityProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E74A7215-014D-11d1-A63C-00A0C911B4E0")
    SecurityProperty : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetDirectCallerName( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetDirectCreatorName( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetOriginalCallerName( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE GetOriginalCreatorName( 
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct SecurityPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            SecurityProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            SecurityProperty __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            SecurityProperty __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            SecurityProperty __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            SecurityProperty __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            SecurityProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            SecurityProperty __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectCallerName )( 
            SecurityProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectCreatorName )( 
            SecurityProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOriginalCallerName )( 
            SecurityProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOriginalCreatorName )( 
            SecurityProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);
        
        END_INTERFACE
    } SecurityPropertyVtbl;

    interface SecurityProperty
    {
        CONST_VTBL struct SecurityPropertyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define SecurityProperty_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define SecurityProperty_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define SecurityProperty_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define SecurityProperty_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define SecurityProperty_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define SecurityProperty_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define SecurityProperty_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define SecurityProperty_GetDirectCallerName(This,bstrUserName)	\
    (This)->lpVtbl -> GetDirectCallerName(This,bstrUserName)

#define SecurityProperty_GetDirectCreatorName(This,bstrUserName)	\
    (This)->lpVtbl -> GetDirectCreatorName(This,bstrUserName)

#define SecurityProperty_GetOriginalCallerName(This,bstrUserName)	\
    (This)->lpVtbl -> GetOriginalCallerName(This,bstrUserName)

#define SecurityProperty_GetOriginalCreatorName(This,bstrUserName)	\
    (This)->lpVtbl -> GetOriginalCreatorName(This,bstrUserName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SecurityProperty_GetDirectCallerName_Proxy( 
    SecurityProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);


void __RPC_STUB SecurityProperty_GetDirectCallerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SecurityProperty_GetDirectCreatorName_Proxy( 
    SecurityProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);


void __RPC_STUB SecurityProperty_GetDirectCreatorName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SecurityProperty_GetOriginalCallerName_Proxy( 
    SecurityProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);


void __RPC_STUB SecurityProperty_GetOriginalCallerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SecurityProperty_GetOriginalCreatorName_Proxy( 
    SecurityProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *bstrUserName);


void __RPC_STUB SecurityProperty_GetOriginalCreatorName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __SecurityProperty_INTERFACE_DEFINED__ */


#ifndef __ContextInfo_INTERFACE_DEFINED__
#define __ContextInfo_INTERFACE_DEFINED__

/* interface ContextInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ContextInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("19A5A02C-0AC8-11d2-B286-00C04F8EF934")
    ContextInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsInTransaction( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsInTx) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTransaction( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppTx) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetTransactionId( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTxId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetActivityId( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrActivityId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetContextId( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCtxId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ContextInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ContextInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ContextInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ContextInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ContextInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ContextInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ContextInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ContextInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsInTransaction )( 
            ContextInfo __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsInTx);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTransaction )( 
            ContextInfo __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppTx);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTransactionId )( 
            ContextInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTxId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetActivityId )( 
            ContextInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrActivityId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContextId )( 
            ContextInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCtxId);
        
        END_INTERFACE
    } ContextInfoVtbl;

    interface ContextInfo
    {
        CONST_VTBL struct ContextInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ContextInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ContextInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ContextInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ContextInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ContextInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ContextInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ContextInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ContextInfo_IsInTransaction(This,pbIsInTx)	\
    (This)->lpVtbl -> IsInTransaction(This,pbIsInTx)

#define ContextInfo_GetTransaction(This,ppTx)	\
    (This)->lpVtbl -> GetTransaction(This,ppTx)

#define ContextInfo_GetTransactionId(This,pbstrTxId)	\
    (This)->lpVtbl -> GetTransactionId(This,pbstrTxId)

#define ContextInfo_GetActivityId(This,pbstrActivityId)	\
    (This)->lpVtbl -> GetActivityId(This,pbstrActivityId)

#define ContextInfo_GetContextId(This,pbstrCtxId)	\
    (This)->lpVtbl -> GetContextId(This,pbstrCtxId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ContextInfo_IsInTransaction_Proxy( 
    ContextInfo __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsInTx);


void __RPC_STUB ContextInfo_IsInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ContextInfo_GetTransaction_Proxy( 
    ContextInfo __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppTx);


void __RPC_STUB ContextInfo_GetTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ContextInfo_GetTransactionId_Proxy( 
    ContextInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTxId);


void __RPC_STUB ContextInfo_GetTransactionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ContextInfo_GetActivityId_Proxy( 
    ContextInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrActivityId);


void __RPC_STUB ContextInfo_GetActivityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ContextInfo_GetContextId_Proxy( 
    ContextInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCtxId);


void __RPC_STUB ContextInfo_GetContextId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ContextInfo_INTERFACE_DEFINED__ */


#ifndef __ObjectContext_INTERFACE_DEFINED__
#define __ObjectContext_INTERFACE_DEFINED__

/* interface ObjectContext */
/* [unique][helpcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ObjectContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74C08646-CEDB-11CF-8B49-00AA00B8A790")
    ObjectContext : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ BSTR bstrProgID,
            /* [retval][out] */ VARIANT __RPC_FAR *pObject) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetComplete( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE SetAbort( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE EnableCommit( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE DisableCommit( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsInTransaction( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsInTx) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsSecurityEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsEnabled) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE IsCallerInRole( 
            BSTR bstrRole,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbInRole) = 0;
        
        virtual /* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual /* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
        virtual /* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE get_Security( 
            /* [retval][out] */ SecurityProperty __RPC_FAR *__RPC_FAR *ppSecurityProperty) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ContextInfo( 
            /* [retval][out] */ ContextInfo __RPC_FAR *__RPC_FAR *ppContextInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ObjectContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ObjectContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ObjectContext __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ObjectContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ObjectContext __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ObjectContext __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ObjectContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ObjectContext __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateInstance )( 
            ObjectContext __RPC_FAR * This,
            /* [in] */ BSTR bstrProgID,
            /* [retval][out] */ VARIANT __RPC_FAR *pObject);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetComplete )( 
            ObjectContext __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAbort )( 
            ObjectContext __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableCommit )( 
            ObjectContext __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DisableCommit )( 
            ObjectContext __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsInTransaction )( 
            ObjectContext __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsInTx);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsSecurityEnabled )( 
            ObjectContext __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsEnabled);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsCallerInRole )( 
            ObjectContext __RPC_FAR * This,
            BSTR bstrRole,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbInRole);
        
        /* [helpstring][helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ObjectContext __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        /* [helpstring][helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ObjectContext __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem);
        
        /* [helpstring][helpcontext][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ObjectContext __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        /* [helpstring][helpcontext][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Security )( 
            ObjectContext __RPC_FAR * This,
            /* [retval][out] */ SecurityProperty __RPC_FAR *__RPC_FAR *ppSecurityProperty);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ContextInfo )( 
            ObjectContext __RPC_FAR * This,
            /* [retval][out] */ ContextInfo __RPC_FAR *__RPC_FAR *ppContextInfo);
        
        END_INTERFACE
    } ObjectContextVtbl;

    interface ObjectContext
    {
        CONST_VTBL struct ObjectContextVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ObjectContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ObjectContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ObjectContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ObjectContext_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ObjectContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ObjectContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ObjectContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ObjectContext_CreateInstance(This,bstrProgID,pObject)	\
    (This)->lpVtbl -> CreateInstance(This,bstrProgID,pObject)

#define ObjectContext_SetComplete(This)	\
    (This)->lpVtbl -> SetComplete(This)

#define ObjectContext_SetAbort(This)	\
    (This)->lpVtbl -> SetAbort(This)

#define ObjectContext_EnableCommit(This)	\
    (This)->lpVtbl -> EnableCommit(This)

#define ObjectContext_DisableCommit(This)	\
    (This)->lpVtbl -> DisableCommit(This)

#define ObjectContext_IsInTransaction(This,pbIsInTx)	\
    (This)->lpVtbl -> IsInTransaction(This,pbIsInTx)

#define ObjectContext_IsSecurityEnabled(This,pbIsEnabled)	\
    (This)->lpVtbl -> IsSecurityEnabled(This,pbIsEnabled)

#define ObjectContext_IsCallerInRole(This,bstrRole,pbInRole)	\
    (This)->lpVtbl -> IsCallerInRole(This,bstrRole,pbInRole)

#define ObjectContext_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define ObjectContext_get_Item(This,name,pItem)	\
    (This)->lpVtbl -> get_Item(This,name,pItem)

#define ObjectContext_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#define ObjectContext_get_Security(This,ppSecurityProperty)	\
    (This)->lpVtbl -> get_Security(This,ppSecurityProperty)

#define ObjectContext_get_ContextInfo(This,ppContextInfo)	\
    (This)->lpVtbl -> get_ContextInfo(This,ppContextInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_CreateInstance_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [in] */ BSTR bstrProgID,
    /* [retval][out] */ VARIANT __RPC_FAR *pObject);


void __RPC_STUB ObjectContext_CreateInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_SetComplete_Proxy( 
    ObjectContext __RPC_FAR * This);


void __RPC_STUB ObjectContext_SetComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_SetAbort_Proxy( 
    ObjectContext __RPC_FAR * This);


void __RPC_STUB ObjectContext_SetAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_EnableCommit_Proxy( 
    ObjectContext __RPC_FAR * This);


void __RPC_STUB ObjectContext_EnableCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_DisableCommit_Proxy( 
    ObjectContext __RPC_FAR * This);


void __RPC_STUB ObjectContext_DisableCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_IsInTransaction_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsInTx);


void __RPC_STUB ObjectContext_IsInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_IsSecurityEnabled_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbIsEnabled);


void __RPC_STUB ObjectContext_IsSecurityEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_IsCallerInRole_Proxy( 
    ObjectContext __RPC_FAR * This,
    BSTR bstrRole,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbInRole);


void __RPC_STUB ObjectContext_IsCallerInRole_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_get_Count_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB ObjectContext_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_get_Item_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT __RPC_FAR *pItem);


void __RPC_STUB ObjectContext_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_get__NewEnum_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB ObjectContext_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][propget][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_get_Security_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [retval][out] */ SecurityProperty __RPC_FAR *__RPC_FAR *ppSecurityProperty);


void __RPC_STUB ObjectContext_get_Security_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ObjectContext_get_ContextInfo_Proxy( 
    ObjectContext __RPC_FAR * This,
    /* [retval][out] */ ContextInfo __RPC_FAR *__RPC_FAR *ppContextInfo);


void __RPC_STUB ObjectContext_get_ContextInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ObjectContext_INTERFACE_DEFINED__ */


#ifndef __ITransactionContextEx_INTERFACE_DEFINED__
#define __ITransactionContextEx_INTERFACE_DEFINED__

/* interface ITransactionContextEx */
/* [unique][helpcontext][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ITransactionContextEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7999FC22-D3C6-11CF-ACAB-00A024A55AEF")
    ITransactionContextEx : public IUnknown
    {
    public:
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject) = 0;
        
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionContextExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionContextEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionContextEx __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionContextEx __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateInstance )( 
            ITransactionContextEx __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            ITransactionContextEx __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abort )( 
            ITransactionContextEx __RPC_FAR * This);
        
        END_INTERFACE
    } ITransactionContextExVtbl;

    interface ITransactionContextEx
    {
        CONST_VTBL struct ITransactionContextExVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionContextEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionContextEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionContextEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionContextEx_CreateInstance(This,rclsid,riid,pObject)	\
    (This)->lpVtbl -> CreateInstance(This,rclsid,riid,pObject)

#define ITransactionContextEx_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define ITransactionContextEx_Abort(This)	\
    (This)->lpVtbl -> Abort(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ITransactionContextEx_CreateInstance_Proxy( 
    ITransactionContextEx __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ REFIID riid,
    /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject);


void __RPC_STUB ITransactionContextEx_CreateInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ITransactionContextEx_Commit_Proxy( 
    ITransactionContextEx __RPC_FAR * This);


void __RPC_STUB ITransactionContextEx_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ITransactionContextEx_Abort_Proxy( 
    ITransactionContextEx __RPC_FAR * This);


void __RPC_STUB ITransactionContextEx_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionContextEx_INTERFACE_DEFINED__ */


#ifndef __ITransactionContext_INTERFACE_DEFINED__
#define __ITransactionContext_INTERFACE_DEFINED__

/* interface ITransactionContext */
/* [unique][helpcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ITransactionContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7999FC21-D3C6-11CF-ACAB-00A024A55AEF")
    ITransactionContext : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ BSTR pszProgId,
            /* [retval][out] */ VARIANT __RPC_FAR *pObject) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionContext __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITransactionContext __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITransactionContext __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITransactionContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITransactionContext __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateInstance )( 
            ITransactionContext __RPC_FAR * This,
            /* [in] */ BSTR pszProgId,
            /* [retval][out] */ VARIANT __RPC_FAR *pObject);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            ITransactionContext __RPC_FAR * This);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abort )( 
            ITransactionContext __RPC_FAR * This);
        
        END_INTERFACE
    } ITransactionContextVtbl;

    interface ITransactionContext
    {
        CONST_VTBL struct ITransactionContextVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionContext_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITransactionContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITransactionContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITransactionContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITransactionContext_CreateInstance(This,pszProgId,pObject)	\
    (This)->lpVtbl -> CreateInstance(This,pszProgId,pObject)

#define ITransactionContext_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#define ITransactionContext_Abort(This)	\
    (This)->lpVtbl -> Abort(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ITransactionContext_CreateInstance_Proxy( 
    ITransactionContext __RPC_FAR * This,
    /* [in] */ BSTR pszProgId,
    /* [retval][out] */ VARIANT __RPC_FAR *pObject);


void __RPC_STUB ITransactionContext_CreateInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ITransactionContext_Commit_Proxy( 
    ITransactionContext __RPC_FAR * This);


void __RPC_STUB ITransactionContext_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ITransactionContext_Abort_Proxy( 
    ITransactionContext __RPC_FAR * This);


void __RPC_STUB ITransactionContext_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionContext_INTERFACE_DEFINED__ */


#ifndef __ICreateWithTransactionEx_INTERFACE_DEFINED__
#define __ICreateWithTransactionEx_INTERFACE_DEFINED__

/* interface ICreateWithTransactionEx */
/* [unique][helpcontext][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICreateWithTransactionEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("455ACF57-5345-11d2-99CF-00C04F797BC9")
    ICreateWithTransactionEx : public IUnknown
    {
    public:
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ ITransaction __RPC_FAR *pTransaction,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICreateWithTransactionExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICreateWithTransactionEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICreateWithTransactionEx __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICreateWithTransactionEx __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateInstance )( 
            ICreateWithTransactionEx __RPC_FAR * This,
            /* [in] */ ITransaction __RPC_FAR *pTransaction,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject);
        
        END_INTERFACE
    } ICreateWithTransactionExVtbl;

    interface ICreateWithTransactionEx
    {
        CONST_VTBL struct ICreateWithTransactionExVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICreateWithTransactionEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICreateWithTransactionEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICreateWithTransactionEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICreateWithTransactionEx_CreateInstance(This,pTransaction,rclsid,riid,pObject)	\
    (This)->lpVtbl -> CreateInstance(This,pTransaction,rclsid,riid,pObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ICreateWithTransactionEx_CreateInstance_Proxy( 
    ICreateWithTransactionEx __RPC_FAR * This,
    /* [in] */ ITransaction __RPC_FAR *pTransaction,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ REFIID riid,
    /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject);


void __RPC_STUB ICreateWithTransactionEx_CreateInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICreateWithTransactionEx_INTERFACE_DEFINED__ */


#ifndef __ICreateWithTipTransactionEx_INTERFACE_DEFINED__
#define __ICreateWithTipTransactionEx_INTERFACE_DEFINED__

/* interface ICreateWithTipTransactionEx */
/* [unique][helpcontext][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICreateWithTipTransactionEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("455ACF59-5345-11d2-99CF-00C04F797BC9")
    ICreateWithTipTransactionEx : public IUnknown
    {
    public:
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ BSTR bstrTipUrl,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICreateWithTipTransactionExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICreateWithTipTransactionEx __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICreateWithTipTransactionEx __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICreateWithTipTransactionEx __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateInstance )( 
            ICreateWithTipTransactionEx __RPC_FAR * This,
            /* [in] */ BSTR bstrTipUrl,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject);
        
        END_INTERFACE
    } ICreateWithTipTransactionExVtbl;

    interface ICreateWithTipTransactionEx
    {
        CONST_VTBL struct ICreateWithTipTransactionExVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICreateWithTipTransactionEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICreateWithTipTransactionEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICreateWithTipTransactionEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICreateWithTipTransactionEx_CreateInstance(This,bstrTipUrl,rclsid,riid,pObject)	\
    (This)->lpVtbl -> CreateInstance(This,bstrTipUrl,rclsid,riid,pObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ICreateWithTipTransactionEx_CreateInstance_Proxy( 
    ICreateWithTipTransactionEx __RPC_FAR * This,
    /* [in] */ BSTR bstrTipUrl,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ REFIID riid,
    /* [iid_is][retval][out] */ void __RPC_FAR *__RPC_FAR *pObject);


void __RPC_STUB ICreateWithTipTransactionEx_CreateInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICreateWithTipTransactionEx_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0276 */
/* [local] */ 

typedef unsigned __int64 MTS_OBJID;

typedef unsigned __int64 MTS_RESID;

typedef unsigned __int64 ULONG64;

#ifndef _COMSVCSEVENTINFO_
#define _COMSVCSEVENTINFO_
typedef /* [public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][public][hidden] */ struct __MIDL___MIDL_itf_autosvcs_0276_0001
    {
    DWORD cbSize;
    DWORD dwPid;
    LONGLONG lTime;
    LONG lMicroTime;
    LONGLONG perfCount;
    GUID guidApp;
    LPOLESTR sMachineName;
    }	COMSVCSEVENTINFO;

#endif _COMSVCSEVENTINFO_


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0276_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0276_v0_0_s_ifspec;

#ifndef __IComUserEvent_INTERFACE_DEFINED__
#define __IComUserEvent_INTERFACE_DEFINED__

/* interface IComUserEvent */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComUserEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130A4-2E50-11d2-98A5-00C04F8EE1C4")
    IComUserEvent : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnUserEvent( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            VARIANT __RPC_FAR *pvarEvent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComUserEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComUserEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComUserEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComUserEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnUserEvent )( 
            IComUserEvent __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            VARIANT __RPC_FAR *pvarEvent);
        
        END_INTERFACE
    } IComUserEventVtbl;

    interface IComUserEvent
    {
        CONST_VTBL struct IComUserEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComUserEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComUserEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComUserEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComUserEvent_OnUserEvent(This,pInfo,pvarEvent)	\
    (This)->lpVtbl -> OnUserEvent(This,pInfo,pvarEvent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComUserEvent_OnUserEvent_Proxy( 
    IComUserEvent __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    VARIANT __RPC_FAR *pvarEvent);


void __RPC_STUB IComUserEvent_OnUserEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComUserEvent_INTERFACE_DEFINED__ */


#ifndef __IComThreadEvents_INTERFACE_DEFINED__
#define __IComThreadEvents_INTERFACE_DEFINED__

/* interface IComThreadEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComThreadEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130A5-2E50-11d2-98A5-00C04F8EE1C4")
    IComThreadEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnThreadStart( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwTheadCnt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadTerminate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwTheadCnt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadBindToApartment( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 AptID,
            /* [in] */ DWORD dwActCnt,
            /* [in] */ DWORD dwLowCnt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadUnBind( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 AptID,
            /* [in] */ DWORD dwActCnt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadWorkEnque( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadWorkPrivate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadWorkPublic( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadWorkRedirect( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen,
            /* [in] */ ULONG64 ThreadNum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadWorkReject( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadAssignApartment( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity,
            /* [in] */ ULONG64 AptID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnThreadUnassignApartment( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 AptID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComThreadEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComThreadEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComThreadEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadStart )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwTheadCnt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadTerminate )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwTheadCnt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadBindToApartment )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 AptID,
            /* [in] */ DWORD dwActCnt,
            /* [in] */ DWORD dwLowCnt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadUnBind )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 AptID,
            /* [in] */ DWORD dwActCnt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadWorkEnque )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadWorkPrivate )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadWorkPublic )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadWorkRedirect )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen,
            /* [in] */ ULONG64 ThreadNum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadWorkReject )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ThreadID,
            /* [in] */ ULONG64 MsgWorkID,
            /* [in] */ DWORD QueueLen);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadAssignApartment )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity,
            /* [in] */ ULONG64 AptID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnThreadUnassignApartment )( 
            IComThreadEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 AptID);
        
        END_INTERFACE
    } IComThreadEventsVtbl;

    interface IComThreadEvents
    {
        CONST_VTBL struct IComThreadEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComThreadEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComThreadEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComThreadEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComThreadEvents_OnThreadStart(This,pInfo,ThreadID,dwThread,dwTheadCnt)	\
    (This)->lpVtbl -> OnThreadStart(This,pInfo,ThreadID,dwThread,dwTheadCnt)

#define IComThreadEvents_OnThreadTerminate(This,pInfo,ThreadID,dwThread,dwTheadCnt)	\
    (This)->lpVtbl -> OnThreadTerminate(This,pInfo,ThreadID,dwThread,dwTheadCnt)

#define IComThreadEvents_OnThreadBindToApartment(This,pInfo,ThreadID,AptID,dwActCnt,dwLowCnt)	\
    (This)->lpVtbl -> OnThreadBindToApartment(This,pInfo,ThreadID,AptID,dwActCnt,dwLowCnt)

#define IComThreadEvents_OnThreadUnBind(This,pInfo,ThreadID,AptID,dwActCnt)	\
    (This)->lpVtbl -> OnThreadUnBind(This,pInfo,ThreadID,AptID,dwActCnt)

#define IComThreadEvents_OnThreadWorkEnque(This,pInfo,ThreadID,MsgWorkID,QueueLen)	\
    (This)->lpVtbl -> OnThreadWorkEnque(This,pInfo,ThreadID,MsgWorkID,QueueLen)

#define IComThreadEvents_OnThreadWorkPrivate(This,pInfo,ThreadID,MsgWorkID)	\
    (This)->lpVtbl -> OnThreadWorkPrivate(This,pInfo,ThreadID,MsgWorkID)

#define IComThreadEvents_OnThreadWorkPublic(This,pInfo,ThreadID,MsgWorkID,QueueLen)	\
    (This)->lpVtbl -> OnThreadWorkPublic(This,pInfo,ThreadID,MsgWorkID,QueueLen)

#define IComThreadEvents_OnThreadWorkRedirect(This,pInfo,ThreadID,MsgWorkID,QueueLen,ThreadNum)	\
    (This)->lpVtbl -> OnThreadWorkRedirect(This,pInfo,ThreadID,MsgWorkID,QueueLen,ThreadNum)

#define IComThreadEvents_OnThreadWorkReject(This,pInfo,ThreadID,MsgWorkID,QueueLen)	\
    (This)->lpVtbl -> OnThreadWorkReject(This,pInfo,ThreadID,MsgWorkID,QueueLen)

#define IComThreadEvents_OnThreadAssignApartment(This,pInfo,guidActivity,AptID)	\
    (This)->lpVtbl -> OnThreadAssignApartment(This,pInfo,guidActivity,AptID)

#define IComThreadEvents_OnThreadUnassignApartment(This,pInfo,AptID)	\
    (This)->lpVtbl -> OnThreadUnassignApartment(This,pInfo,AptID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadStart_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ DWORD dwThread,
    /* [in] */ DWORD dwTheadCnt);


void __RPC_STUB IComThreadEvents_OnThreadStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadTerminate_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ DWORD dwThread,
    /* [in] */ DWORD dwTheadCnt);


void __RPC_STUB IComThreadEvents_OnThreadTerminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadBindToApartment_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 AptID,
    /* [in] */ DWORD dwActCnt,
    /* [in] */ DWORD dwLowCnt);


void __RPC_STUB IComThreadEvents_OnThreadBindToApartment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadUnBind_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 AptID,
    /* [in] */ DWORD dwActCnt);


void __RPC_STUB IComThreadEvents_OnThreadUnBind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadWorkEnque_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 MsgWorkID,
    /* [in] */ DWORD QueueLen);


void __RPC_STUB IComThreadEvents_OnThreadWorkEnque_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadWorkPrivate_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 MsgWorkID);


void __RPC_STUB IComThreadEvents_OnThreadWorkPrivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadWorkPublic_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 MsgWorkID,
    /* [in] */ DWORD QueueLen);


void __RPC_STUB IComThreadEvents_OnThreadWorkPublic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadWorkRedirect_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 MsgWorkID,
    /* [in] */ DWORD QueueLen,
    /* [in] */ ULONG64 ThreadNum);


void __RPC_STUB IComThreadEvents_OnThreadWorkRedirect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadWorkReject_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ThreadID,
    /* [in] */ ULONG64 MsgWorkID,
    /* [in] */ DWORD QueueLen);


void __RPC_STUB IComThreadEvents_OnThreadWorkReject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadAssignApartment_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidActivity,
    /* [in] */ ULONG64 AptID);


void __RPC_STUB IComThreadEvents_OnThreadAssignApartment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComThreadEvents_OnThreadUnassignApartment_Proxy( 
    IComThreadEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 AptID);


void __RPC_STUB IComThreadEvents_OnThreadUnassignApartment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComThreadEvents_INTERFACE_DEFINED__ */


#ifndef __IComAppEvents_INTERFACE_DEFINED__
#define __IComAppEvents_INTERFACE_DEFINED__

/* interface IComAppEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComAppEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130A6-2E50-11d2-98A5-00C04F8EE1C4")
    IComAppEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnAppActivation( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAppShutdown( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAppForceShutdown( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComAppEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComAppEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComAppEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComAppEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnAppActivation )( 
            IComAppEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnAppShutdown )( 
            IComAppEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnAppForceShutdown )( 
            IComAppEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp);
        
        END_INTERFACE
    } IComAppEventsVtbl;

    interface IComAppEvents
    {
        CONST_VTBL struct IComAppEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComAppEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComAppEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComAppEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComAppEvents_OnAppActivation(This,pInfo,guidApp)	\
    (This)->lpVtbl -> OnAppActivation(This,pInfo,guidApp)

#define IComAppEvents_OnAppShutdown(This,pInfo,guidApp)	\
    (This)->lpVtbl -> OnAppShutdown(This,pInfo,guidApp)

#define IComAppEvents_OnAppForceShutdown(This,pInfo,guidApp)	\
    (This)->lpVtbl -> OnAppForceShutdown(This,pInfo,guidApp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComAppEvents_OnAppActivation_Proxy( 
    IComAppEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidApp);


void __RPC_STUB IComAppEvents_OnAppActivation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComAppEvents_OnAppShutdown_Proxy( 
    IComAppEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidApp);


void __RPC_STUB IComAppEvents_OnAppShutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComAppEvents_OnAppForceShutdown_Proxy( 
    IComAppEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidApp);


void __RPC_STUB IComAppEvents_OnAppForceShutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComAppEvents_INTERFACE_DEFINED__ */


#ifndef __IComInstanceEvents_INTERFACE_DEFINED__
#define __IComInstanceEvents_INTERFACE_DEFINED__

/* interface IComInstanceEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComInstanceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130A7-2E50-11d2-98A5-00C04F8EE1C4")
    IComInstanceEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnObjectCreate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity,
            /* [in] */ REFCLSID clsid,
            /* [in] */ REFGUID tsid,
            /* [in] */ ULONG64 CtxtID,
            /* [in] */ ULONG64 ObjectID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjectDestroy( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComInstanceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComInstanceEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComInstanceEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComInstanceEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjectCreate )( 
            IComInstanceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity,
            /* [in] */ REFCLSID clsid,
            /* [in] */ REFGUID tsid,
            /* [in] */ ULONG64 CtxtID,
            /* [in] */ ULONG64 ObjectID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjectDestroy )( 
            IComInstanceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID);
        
        END_INTERFACE
    } IComInstanceEventsVtbl;

    interface IComInstanceEvents
    {
        CONST_VTBL struct IComInstanceEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComInstanceEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComInstanceEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComInstanceEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComInstanceEvents_OnObjectCreate(This,pInfo,guidActivity,clsid,tsid,CtxtID,ObjectID)	\
    (This)->lpVtbl -> OnObjectCreate(This,pInfo,guidActivity,clsid,tsid,CtxtID,ObjectID)

#define IComInstanceEvents_OnObjectDestroy(This,pInfo,CtxtID)	\
    (This)->lpVtbl -> OnObjectDestroy(This,pInfo,CtxtID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComInstanceEvents_OnObjectCreate_Proxy( 
    IComInstanceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidActivity,
    /* [in] */ REFCLSID clsid,
    /* [in] */ REFGUID tsid,
    /* [in] */ ULONG64 CtxtID,
    /* [in] */ ULONG64 ObjectID);


void __RPC_STUB IComInstanceEvents_OnObjectCreate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComInstanceEvents_OnObjectDestroy_Proxy( 
    IComInstanceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID);


void __RPC_STUB IComInstanceEvents_OnObjectDestroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComInstanceEvents_INTERFACE_DEFINED__ */


#ifndef __IComTransactionEvents_INTERFACE_DEFINED__
#define __IComTransactionEvents_INTERFACE_DEFINED__

/* interface IComTransactionEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComTransactionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130A8-2E50-11d2-98A5-00C04F8EE1C4")
    IComTransactionEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnTransactionStart( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx,
            /* [in] */ REFGUID tsid,
            /* [in] */ BOOL fRoot) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnTransactionPrepare( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx,
            /* [in] */ BOOL fVoteYes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnTransactionAbort( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnTransactionCommit( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComTransactionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComTransactionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComTransactionEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComTransactionEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnTransactionStart )( 
            IComTransactionEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx,
            /* [in] */ REFGUID tsid,
            /* [in] */ BOOL fRoot);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnTransactionPrepare )( 
            IComTransactionEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx,
            /* [in] */ BOOL fVoteYes);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnTransactionAbort )( 
            IComTransactionEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnTransactionCommit )( 
            IComTransactionEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidTx);
        
        END_INTERFACE
    } IComTransactionEventsVtbl;

    interface IComTransactionEvents
    {
        CONST_VTBL struct IComTransactionEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComTransactionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComTransactionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComTransactionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComTransactionEvents_OnTransactionStart(This,pInfo,guidTx,tsid,fRoot)	\
    (This)->lpVtbl -> OnTransactionStart(This,pInfo,guidTx,tsid,fRoot)

#define IComTransactionEvents_OnTransactionPrepare(This,pInfo,guidTx,fVoteYes)	\
    (This)->lpVtbl -> OnTransactionPrepare(This,pInfo,guidTx,fVoteYes)

#define IComTransactionEvents_OnTransactionAbort(This,pInfo,guidTx)	\
    (This)->lpVtbl -> OnTransactionAbort(This,pInfo,guidTx)

#define IComTransactionEvents_OnTransactionCommit(This,pInfo,guidTx)	\
    (This)->lpVtbl -> OnTransactionCommit(This,pInfo,guidTx)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComTransactionEvents_OnTransactionStart_Proxy( 
    IComTransactionEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidTx,
    /* [in] */ REFGUID tsid,
    /* [in] */ BOOL fRoot);


void __RPC_STUB IComTransactionEvents_OnTransactionStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComTransactionEvents_OnTransactionPrepare_Proxy( 
    IComTransactionEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidTx,
    /* [in] */ BOOL fVoteYes);


void __RPC_STUB IComTransactionEvents_OnTransactionPrepare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComTransactionEvents_OnTransactionAbort_Proxy( 
    IComTransactionEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidTx);


void __RPC_STUB IComTransactionEvents_OnTransactionAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComTransactionEvents_OnTransactionCommit_Proxy( 
    IComTransactionEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidTx);


void __RPC_STUB IComTransactionEvents_OnTransactionCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComTransactionEvents_INTERFACE_DEFINED__ */


#ifndef __IComMethodEvents_INTERFACE_DEFINED__
#define __IComMethodEvents_INTERFACE_DEFINED__

/* interface IComMethodEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComMethodEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130A9-2E50-11d2-98A5-00C04F8EE1C4")
    IComMethodEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnMethodCall( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 oid,
            /* [in] */ REFCLSID guidCid,
            /* [in] */ REFIID guidRid,
            /* [in] */ ULONG iMeth) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnMethodReturn( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 oid,
            /* [in] */ REFCLSID guidCid,
            /* [in] */ REFIID guidRid,
            /* [in] */ ULONG iMeth,
            /* [in] */ HRESULT hresult) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnMethodException( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 oid,
            /* [in] */ REFCLSID guidCid,
            /* [in] */ REFIID guidRid,
            /* [in] */ ULONG iMeth) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComMethodEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComMethodEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComMethodEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComMethodEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnMethodCall )( 
            IComMethodEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 oid,
            /* [in] */ REFCLSID guidCid,
            /* [in] */ REFIID guidRid,
            /* [in] */ ULONG iMeth);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnMethodReturn )( 
            IComMethodEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 oid,
            /* [in] */ REFCLSID guidCid,
            /* [in] */ REFIID guidRid,
            /* [in] */ ULONG iMeth,
            /* [in] */ HRESULT hresult);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnMethodException )( 
            IComMethodEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 oid,
            /* [in] */ REFCLSID guidCid,
            /* [in] */ REFIID guidRid,
            /* [in] */ ULONG iMeth);
        
        END_INTERFACE
    } IComMethodEventsVtbl;

    interface IComMethodEvents
    {
        CONST_VTBL struct IComMethodEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComMethodEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComMethodEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComMethodEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComMethodEvents_OnMethodCall(This,pInfo,oid,guidCid,guidRid,iMeth)	\
    (This)->lpVtbl -> OnMethodCall(This,pInfo,oid,guidCid,guidRid,iMeth)

#define IComMethodEvents_OnMethodReturn(This,pInfo,oid,guidCid,guidRid,iMeth,hresult)	\
    (This)->lpVtbl -> OnMethodReturn(This,pInfo,oid,guidCid,guidRid,iMeth,hresult)

#define IComMethodEvents_OnMethodException(This,pInfo,oid,guidCid,guidRid,iMeth)	\
    (This)->lpVtbl -> OnMethodException(This,pInfo,oid,guidCid,guidRid,iMeth)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComMethodEvents_OnMethodCall_Proxy( 
    IComMethodEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 oid,
    /* [in] */ REFCLSID guidCid,
    /* [in] */ REFIID guidRid,
    /* [in] */ ULONG iMeth);


void __RPC_STUB IComMethodEvents_OnMethodCall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComMethodEvents_OnMethodReturn_Proxy( 
    IComMethodEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 oid,
    /* [in] */ REFCLSID guidCid,
    /* [in] */ REFIID guidRid,
    /* [in] */ ULONG iMeth,
    /* [in] */ HRESULT hresult);


void __RPC_STUB IComMethodEvents_OnMethodReturn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComMethodEvents_OnMethodException_Proxy( 
    IComMethodEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 oid,
    /* [in] */ REFCLSID guidCid,
    /* [in] */ REFIID guidRid,
    /* [in] */ ULONG iMeth);


void __RPC_STUB IComMethodEvents_OnMethodException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComMethodEvents_INTERFACE_DEFINED__ */


#ifndef __IComObjectEvents_INTERFACE_DEFINED__
#define __IComObjectEvents_INTERFACE_DEFINED__

/* interface IComObjectEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComObjectEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130AA-2E50-11d2-98A5-00C04F8EE1C4")
    IComObjectEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnObjectActivate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID,
            /* [in] */ ULONG64 ObjectID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjectDeactivate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID,
            /* [in] */ ULONG64 ObjectID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnDisableCommit( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEnableCommit( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSetComplete( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSetAbort( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComObjectEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComObjectEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComObjectEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjectActivate )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID,
            /* [in] */ ULONG64 ObjectID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjectDeactivate )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID,
            /* [in] */ ULONG64 ObjectID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnDisableCommit )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnEnableCommit )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnSetComplete )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnSetAbort )( 
            IComObjectEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 CtxtID);
        
        END_INTERFACE
    } IComObjectEventsVtbl;

    interface IComObjectEvents
    {
        CONST_VTBL struct IComObjectEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComObjectEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComObjectEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComObjectEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComObjectEvents_OnObjectActivate(This,pInfo,CtxtID,ObjectID)	\
    (This)->lpVtbl -> OnObjectActivate(This,pInfo,CtxtID,ObjectID)

#define IComObjectEvents_OnObjectDeactivate(This,pInfo,CtxtID,ObjectID)	\
    (This)->lpVtbl -> OnObjectDeactivate(This,pInfo,CtxtID,ObjectID)

#define IComObjectEvents_OnDisableCommit(This,pInfo,CtxtID)	\
    (This)->lpVtbl -> OnDisableCommit(This,pInfo,CtxtID)

#define IComObjectEvents_OnEnableCommit(This,pInfo,CtxtID)	\
    (This)->lpVtbl -> OnEnableCommit(This,pInfo,CtxtID)

#define IComObjectEvents_OnSetComplete(This,pInfo,CtxtID)	\
    (This)->lpVtbl -> OnSetComplete(This,pInfo,CtxtID)

#define IComObjectEvents_OnSetAbort(This,pInfo,CtxtID)	\
    (This)->lpVtbl -> OnSetAbort(This,pInfo,CtxtID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComObjectEvents_OnObjectActivate_Proxy( 
    IComObjectEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID,
    /* [in] */ ULONG64 ObjectID);


void __RPC_STUB IComObjectEvents_OnObjectActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectEvents_OnObjectDeactivate_Proxy( 
    IComObjectEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID,
    /* [in] */ ULONG64 ObjectID);


void __RPC_STUB IComObjectEvents_OnObjectDeactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectEvents_OnDisableCommit_Proxy( 
    IComObjectEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID);


void __RPC_STUB IComObjectEvents_OnDisableCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectEvents_OnEnableCommit_Proxy( 
    IComObjectEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID);


void __RPC_STUB IComObjectEvents_OnEnableCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectEvents_OnSetComplete_Proxy( 
    IComObjectEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID);


void __RPC_STUB IComObjectEvents_OnSetComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectEvents_OnSetAbort_Proxy( 
    IComObjectEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 CtxtID);


void __RPC_STUB IComObjectEvents_OnSetAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComObjectEvents_INTERFACE_DEFINED__ */


#ifndef __IComResourceEvents_INTERFACE_DEFINED__
#define __IComResourceEvents_INTERFACE_DEFINED__

/* interface IComResourceEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComResourceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130AB-2E50-11d2-98A5-00C04F8EE1C4")
    IComResourceEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnResourceCreate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId,
            /* [in] */ BOOL enlisted) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResourceAllocate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId,
            /* [in] */ BOOL enlisted,
            /* [in] */ DWORD NumRated,
            /* [in] */ DWORD Rating) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResourceRecycle( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResourceDestroy( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ HRESULT hr,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResourceTrack( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId,
            /* [in] */ BOOL enlisted) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComResourceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComResourceEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComResourceEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComResourceEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnResourceCreate )( 
            IComResourceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId,
            /* [in] */ BOOL enlisted);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnResourceAllocate )( 
            IComResourceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId,
            /* [in] */ BOOL enlisted,
            /* [in] */ DWORD NumRated,
            /* [in] */ DWORD Rating);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnResourceRecycle )( 
            IComResourceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnResourceDestroy )( 
            IComResourceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ HRESULT hr,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnResourceTrack )( 
            IComResourceEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjectID,
            /* [in] */ LPCOLESTR pszType,
            /* [in] */ ULONG64 resId,
            /* [in] */ BOOL enlisted);
        
        END_INTERFACE
    } IComResourceEventsVtbl;

    interface IComResourceEvents
    {
        CONST_VTBL struct IComResourceEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComResourceEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComResourceEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComResourceEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComResourceEvents_OnResourceCreate(This,pInfo,ObjectID,pszType,resId,enlisted)	\
    (This)->lpVtbl -> OnResourceCreate(This,pInfo,ObjectID,pszType,resId,enlisted)

#define IComResourceEvents_OnResourceAllocate(This,pInfo,ObjectID,pszType,resId,enlisted,NumRated,Rating)	\
    (This)->lpVtbl -> OnResourceAllocate(This,pInfo,ObjectID,pszType,resId,enlisted,NumRated,Rating)

#define IComResourceEvents_OnResourceRecycle(This,pInfo,ObjectID,pszType,resId)	\
    (This)->lpVtbl -> OnResourceRecycle(This,pInfo,ObjectID,pszType,resId)

#define IComResourceEvents_OnResourceDestroy(This,pInfo,ObjectID,hr,pszType,resId)	\
    (This)->lpVtbl -> OnResourceDestroy(This,pInfo,ObjectID,hr,pszType,resId)

#define IComResourceEvents_OnResourceTrack(This,pInfo,ObjectID,pszType,resId,enlisted)	\
    (This)->lpVtbl -> OnResourceTrack(This,pInfo,ObjectID,pszType,resId,enlisted)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComResourceEvents_OnResourceCreate_Proxy( 
    IComResourceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ObjectID,
    /* [in] */ LPCOLESTR pszType,
    /* [in] */ ULONG64 resId,
    /* [in] */ BOOL enlisted);


void __RPC_STUB IComResourceEvents_OnResourceCreate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComResourceEvents_OnResourceAllocate_Proxy( 
    IComResourceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ObjectID,
    /* [in] */ LPCOLESTR pszType,
    /* [in] */ ULONG64 resId,
    /* [in] */ BOOL enlisted,
    /* [in] */ DWORD NumRated,
    /* [in] */ DWORD Rating);


void __RPC_STUB IComResourceEvents_OnResourceAllocate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComResourceEvents_OnResourceRecycle_Proxy( 
    IComResourceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ObjectID,
    /* [in] */ LPCOLESTR pszType,
    /* [in] */ ULONG64 resId);


void __RPC_STUB IComResourceEvents_OnResourceRecycle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComResourceEvents_OnResourceDestroy_Proxy( 
    IComResourceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ObjectID,
    /* [in] */ HRESULT hr,
    /* [in] */ LPCOLESTR pszType,
    /* [in] */ ULONG64 resId);


void __RPC_STUB IComResourceEvents_OnResourceDestroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComResourceEvents_OnResourceTrack_Proxy( 
    IComResourceEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ObjectID,
    /* [in] */ LPCOLESTR pszType,
    /* [in] */ ULONG64 resId,
    /* [in] */ BOOL enlisted);


void __RPC_STUB IComResourceEvents_OnResourceTrack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComResourceEvents_INTERFACE_DEFINED__ */


#ifndef __IComSecurityEvents_INTERFACE_DEFINED__
#define __IComSecurityEvents_INTERFACE_DEFINED__

/* interface IComSecurityEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComSecurityEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130AC-2E50-11d2-98A5-00C04F8EE1C4")
    IComSecurityEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnAuthenticate( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            ULONG64 ObjectID,
            REFGUID guidIID,
            ULONG iMeth,
            ULONG cbByteOrig,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidOriginalUser,
            ULONG cbByteCur,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidCurrentUser,
            BOOL bCurrentUserInpersonatingInProc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnAuthenticateFail( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            ULONG64 ObjectID,
            REFGUID guidIID,
            ULONG iMeth,
            ULONG cbByteOrig,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidOriginalUser,
            ULONG cbByteCur,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidCurrentUser,
            BOOL bCurrentUserInpersonatingInProc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComSecurityEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComSecurityEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComSecurityEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComSecurityEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnAuthenticate )( 
            IComSecurityEvents __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            ULONG64 ObjectID,
            REFGUID guidIID,
            ULONG iMeth,
            ULONG cbByteOrig,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidOriginalUser,
            ULONG cbByteCur,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidCurrentUser,
            BOOL bCurrentUserInpersonatingInProc);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnAuthenticateFail )( 
            IComSecurityEvents __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            ULONG64 ObjectID,
            REFGUID guidIID,
            ULONG iMeth,
            ULONG cbByteOrig,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidOriginalUser,
            ULONG cbByteCur,
            /* [size_is][in] */ BYTE __RPC_FAR *pSidCurrentUser,
            BOOL bCurrentUserInpersonatingInProc);
        
        END_INTERFACE
    } IComSecurityEventsVtbl;

    interface IComSecurityEvents
    {
        CONST_VTBL struct IComSecurityEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComSecurityEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComSecurityEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComSecurityEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComSecurityEvents_OnAuthenticate(This,pInfo,guidActivity,ObjectID,guidIID,iMeth,cbByteOrig,pSidOriginalUser,cbByteCur,pSidCurrentUser,bCurrentUserInpersonatingInProc)	\
    (This)->lpVtbl -> OnAuthenticate(This,pInfo,guidActivity,ObjectID,guidIID,iMeth,cbByteOrig,pSidOriginalUser,cbByteCur,pSidCurrentUser,bCurrentUserInpersonatingInProc)

#define IComSecurityEvents_OnAuthenticateFail(This,pInfo,guidActivity,ObjectID,guidIID,iMeth,cbByteOrig,pSidOriginalUser,cbByteCur,pSidCurrentUser,bCurrentUserInpersonatingInProc)	\
    (This)->lpVtbl -> OnAuthenticateFail(This,pInfo,guidActivity,ObjectID,guidIID,iMeth,cbByteOrig,pSidOriginalUser,cbByteCur,pSidCurrentUser,bCurrentUserInpersonatingInProc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComSecurityEvents_OnAuthenticate_Proxy( 
    IComSecurityEvents __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidActivity,
    ULONG64 ObjectID,
    REFGUID guidIID,
    ULONG iMeth,
    ULONG cbByteOrig,
    /* [size_is][in] */ BYTE __RPC_FAR *pSidOriginalUser,
    ULONG cbByteCur,
    /* [size_is][in] */ BYTE __RPC_FAR *pSidCurrentUser,
    BOOL bCurrentUserInpersonatingInProc);


void __RPC_STUB IComSecurityEvents_OnAuthenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComSecurityEvents_OnAuthenticateFail_Proxy( 
    IComSecurityEvents __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidActivity,
    ULONG64 ObjectID,
    REFGUID guidIID,
    ULONG iMeth,
    ULONG cbByteOrig,
    /* [size_is][in] */ BYTE __RPC_FAR *pSidOriginalUser,
    ULONG cbByteCur,
    /* [size_is][in] */ BYTE __RPC_FAR *pSidCurrentUser,
    BOOL bCurrentUserInpersonatingInProc);


void __RPC_STUB IComSecurityEvents_OnAuthenticateFail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComSecurityEvents_INTERFACE_DEFINED__ */


#ifndef __IComObjectPoolEvents_INTERFACE_DEFINED__
#define __IComObjectPoolEvents_INTERFACE_DEFINED__

/* interface IComObjectPoolEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComObjectPoolEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130AD-2E50-11d2-98A5-00C04F8EE1C4")
    IComObjectPoolEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolPutObject( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            int nReason,
            DWORD dwAvailable,
            ULONG64 oid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolGetObject( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            REFGUID guidObject,
            DWORD dwAvailable,
            ULONG64 oid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolRecycleToTx( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            REFGUID guidObject,
            REFGUID guidTx,
            ULONG64 objid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolGetFromTx( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            REFGUID guidObject,
            REFGUID guidTx,
            ULONG64 objid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComObjectPoolEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComObjectPoolEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComObjectPoolEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComObjectPoolEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolPutObject )( 
            IComObjectPoolEvents __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            int nReason,
            DWORD dwAvailable,
            ULONG64 oid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolGetObject )( 
            IComObjectPoolEvents __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            REFGUID guidObject,
            DWORD dwAvailable,
            ULONG64 oid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolRecycleToTx )( 
            IComObjectPoolEvents __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            REFGUID guidObject,
            REFGUID guidTx,
            ULONG64 objid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolGetFromTx )( 
            IComObjectPoolEvents __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidActivity,
            REFGUID guidObject,
            REFGUID guidTx,
            ULONG64 objid);
        
        END_INTERFACE
    } IComObjectPoolEventsVtbl;

    interface IComObjectPoolEvents
    {
        CONST_VTBL struct IComObjectPoolEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComObjectPoolEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComObjectPoolEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComObjectPoolEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComObjectPoolEvents_OnObjPoolPutObject(This,pInfo,guidObject,nReason,dwAvailable,oid)	\
    (This)->lpVtbl -> OnObjPoolPutObject(This,pInfo,guidObject,nReason,dwAvailable,oid)

#define IComObjectPoolEvents_OnObjPoolGetObject(This,pInfo,guidActivity,guidObject,dwAvailable,oid)	\
    (This)->lpVtbl -> OnObjPoolGetObject(This,pInfo,guidActivity,guidObject,dwAvailable,oid)

#define IComObjectPoolEvents_OnObjPoolRecycleToTx(This,pInfo,guidActivity,guidObject,guidTx,objid)	\
    (This)->lpVtbl -> OnObjPoolRecycleToTx(This,pInfo,guidActivity,guidObject,guidTx,objid)

#define IComObjectPoolEvents_OnObjPoolGetFromTx(This,pInfo,guidActivity,guidObject,guidTx,objid)	\
    (This)->lpVtbl -> OnObjPoolGetFromTx(This,pInfo,guidActivity,guidObject,guidTx,objid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComObjectPoolEvents_OnObjPoolPutObject_Proxy( 
    IComObjectPoolEvents __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidObject,
    int nReason,
    DWORD dwAvailable,
    ULONG64 oid);


void __RPC_STUB IComObjectPoolEvents_OnObjPoolPutObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents_OnObjPoolGetObject_Proxy( 
    IComObjectPoolEvents __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidActivity,
    REFGUID guidObject,
    DWORD dwAvailable,
    ULONG64 oid);


void __RPC_STUB IComObjectPoolEvents_OnObjPoolGetObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents_OnObjPoolRecycleToTx_Proxy( 
    IComObjectPoolEvents __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidActivity,
    REFGUID guidObject,
    REFGUID guidTx,
    ULONG64 objid);


void __RPC_STUB IComObjectPoolEvents_OnObjPoolRecycleToTx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents_OnObjPoolGetFromTx_Proxy( 
    IComObjectPoolEvents __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidActivity,
    REFGUID guidObject,
    REFGUID guidTx,
    ULONG64 objid);


void __RPC_STUB IComObjectPoolEvents_OnObjPoolGetFromTx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComObjectPoolEvents_INTERFACE_DEFINED__ */


#ifndef __IComObjectPoolEvents2_INTERFACE_DEFINED__
#define __IComObjectPoolEvents2_INTERFACE_DEFINED__

/* interface IComObjectPoolEvents2 */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComObjectPoolEvents2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130AE-2E50-11d2-98A5-00C04F8EE1C4")
    IComObjectPoolEvents2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolCreateObject( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            DWORD dwObjsCreated,
            ULONG64 oid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolDestroyObject( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            DWORD dwObjsCreated,
            ULONG64 oid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolCreateDecision( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            DWORD dwThreadsWaiting,
            DWORD dwAvail,
            DWORD dwCreated,
            DWORD dwMin,
            DWORD dwMax) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolTimeout( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            REFGUID guidActivity,
            DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjPoolCreatePool( 
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            DWORD dwMin,
            DWORD dwMax,
            DWORD dwTimeout) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComObjectPoolEvents2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComObjectPoolEvents2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComObjectPoolEvents2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComObjectPoolEvents2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolCreateObject )( 
            IComObjectPoolEvents2 __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            DWORD dwObjsCreated,
            ULONG64 oid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolDestroyObject )( 
            IComObjectPoolEvents2 __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            DWORD dwObjsCreated,
            ULONG64 oid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolCreateDecision )( 
            IComObjectPoolEvents2 __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            DWORD dwThreadsWaiting,
            DWORD dwAvail,
            DWORD dwCreated,
            DWORD dwMin,
            DWORD dwMax);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolTimeout )( 
            IComObjectPoolEvents2 __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            REFGUID guidActivity,
            DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjPoolCreatePool )( 
            IComObjectPoolEvents2 __RPC_FAR * This,
            COMSVCSEVENTINFO __RPC_FAR *pInfo,
            REFGUID guidObject,
            DWORD dwMin,
            DWORD dwMax,
            DWORD dwTimeout);
        
        END_INTERFACE
    } IComObjectPoolEvents2Vtbl;

    interface IComObjectPoolEvents2
    {
        CONST_VTBL struct IComObjectPoolEvents2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComObjectPoolEvents2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComObjectPoolEvents2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComObjectPoolEvents2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComObjectPoolEvents2_OnObjPoolCreateObject(This,pInfo,guidObject,dwObjsCreated,oid)	\
    (This)->lpVtbl -> OnObjPoolCreateObject(This,pInfo,guidObject,dwObjsCreated,oid)

#define IComObjectPoolEvents2_OnObjPoolDestroyObject(This,pInfo,guidObject,dwObjsCreated,oid)	\
    (This)->lpVtbl -> OnObjPoolDestroyObject(This,pInfo,guidObject,dwObjsCreated,oid)

#define IComObjectPoolEvents2_OnObjPoolCreateDecision(This,pInfo,dwThreadsWaiting,dwAvail,dwCreated,dwMin,dwMax)	\
    (This)->lpVtbl -> OnObjPoolCreateDecision(This,pInfo,dwThreadsWaiting,dwAvail,dwCreated,dwMin,dwMax)

#define IComObjectPoolEvents2_OnObjPoolTimeout(This,pInfo,guidObject,guidActivity,dwTimeout)	\
    (This)->lpVtbl -> OnObjPoolTimeout(This,pInfo,guidObject,guidActivity,dwTimeout)

#define IComObjectPoolEvents2_OnObjPoolCreatePool(This,pInfo,guidObject,dwMin,dwMax,dwTimeout)	\
    (This)->lpVtbl -> OnObjPoolCreatePool(This,pInfo,guidObject,dwMin,dwMax,dwTimeout)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComObjectPoolEvents2_OnObjPoolCreateObject_Proxy( 
    IComObjectPoolEvents2 __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidObject,
    DWORD dwObjsCreated,
    ULONG64 oid);


void __RPC_STUB IComObjectPoolEvents2_OnObjPoolCreateObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents2_OnObjPoolDestroyObject_Proxy( 
    IComObjectPoolEvents2 __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidObject,
    DWORD dwObjsCreated,
    ULONG64 oid);


void __RPC_STUB IComObjectPoolEvents2_OnObjPoolDestroyObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents2_OnObjPoolCreateDecision_Proxy( 
    IComObjectPoolEvents2 __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    DWORD dwThreadsWaiting,
    DWORD dwAvail,
    DWORD dwCreated,
    DWORD dwMin,
    DWORD dwMax);


void __RPC_STUB IComObjectPoolEvents2_OnObjPoolCreateDecision_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents2_OnObjPoolTimeout_Proxy( 
    IComObjectPoolEvents2 __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidObject,
    REFGUID guidActivity,
    DWORD dwTimeout);


void __RPC_STUB IComObjectPoolEvents2_OnObjPoolTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComObjectPoolEvents2_OnObjPoolCreatePool_Proxy( 
    IComObjectPoolEvents2 __RPC_FAR * This,
    COMSVCSEVENTINFO __RPC_FAR *pInfo,
    REFGUID guidObject,
    DWORD dwMin,
    DWORD dwMax,
    DWORD dwTimeout);


void __RPC_STUB IComObjectPoolEvents2_OnObjPoolCreatePool_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComObjectPoolEvents2_INTERFACE_DEFINED__ */


#ifndef __IComObjectConstructionEvents_INTERFACE_DEFINED__
#define __IComObjectConstructionEvents_INTERFACE_DEFINED__

/* interface IComObjectConstructionEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComObjectConstructionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130AF-2E50-11d2-98A5-00C04F8EE1C4")
    IComObjectConstructionEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnObjectConstruct( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidObject,
            /* [in] */ LPCOLESTR sConstructString,
            /* [in] */ ULONG64 oid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComObjectConstructionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComObjectConstructionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComObjectConstructionEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComObjectConstructionEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnObjectConstruct )( 
            IComObjectConstructionEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidObject,
            /* [in] */ LPCOLESTR sConstructString,
            /* [in] */ ULONG64 oid);
        
        END_INTERFACE
    } IComObjectConstructionEventsVtbl;

    interface IComObjectConstructionEvents
    {
        CONST_VTBL struct IComObjectConstructionEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComObjectConstructionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComObjectConstructionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComObjectConstructionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComObjectConstructionEvents_OnObjectConstruct(This,pInfo,guidObject,sConstructString,oid)	\
    (This)->lpVtbl -> OnObjectConstruct(This,pInfo,guidObject,sConstructString,oid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComObjectConstructionEvents_OnObjectConstruct_Proxy( 
    IComObjectConstructionEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidObject,
    /* [in] */ LPCOLESTR sConstructString,
    /* [in] */ ULONG64 oid);


void __RPC_STUB IComObjectConstructionEvents_OnObjectConstruct_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComObjectConstructionEvents_INTERFACE_DEFINED__ */


#ifndef __IComActivityEvents_INTERFACE_DEFINED__
#define __IComActivityEvents_INTERFACE_DEFINED__

/* interface IComActivityEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComActivityEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130B0-2E50-11d2-98A5-00C04F8EE1C4")
    IComActivityEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnActivityCreate( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnActivityDestroy( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnActivityEnter( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ REFGUID guidEntered,
            /* [in] */ DWORD dwThread) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnActivityTimeout( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ REFGUID guidEntered,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnActivityReenter( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwCallDepth) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnActivityLeave( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ REFGUID guidLeft) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnActivityLeaveSame( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ DWORD dwCallDepth) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComActivityEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComActivityEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComActivityEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityCreate )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityDestroy )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidActivity);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityEnter )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ REFGUID guidEntered,
            /* [in] */ DWORD dwThread);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityTimeout )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ REFGUID guidEntered,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityReenter )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ DWORD dwThread,
            /* [in] */ DWORD dwCallDepth);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityLeave )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ REFGUID guidLeft);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnActivityLeaveSame )( 
            IComActivityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidCurrent,
            /* [in] */ DWORD dwCallDepth);
        
        END_INTERFACE
    } IComActivityEventsVtbl;

    interface IComActivityEvents
    {
        CONST_VTBL struct IComActivityEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComActivityEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComActivityEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComActivityEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComActivityEvents_OnActivityCreate(This,pInfo,guidActivity)	\
    (This)->lpVtbl -> OnActivityCreate(This,pInfo,guidActivity)

#define IComActivityEvents_OnActivityDestroy(This,pInfo,guidActivity)	\
    (This)->lpVtbl -> OnActivityDestroy(This,pInfo,guidActivity)

#define IComActivityEvents_OnActivityEnter(This,pInfo,guidCurrent,guidEntered,dwThread)	\
    (This)->lpVtbl -> OnActivityEnter(This,pInfo,guidCurrent,guidEntered,dwThread)

#define IComActivityEvents_OnActivityTimeout(This,pInfo,guidCurrent,guidEntered,dwThread,dwTimeout)	\
    (This)->lpVtbl -> OnActivityTimeout(This,pInfo,guidCurrent,guidEntered,dwThread,dwTimeout)

#define IComActivityEvents_OnActivityReenter(This,pInfo,guidCurrent,dwThread,dwCallDepth)	\
    (This)->lpVtbl -> OnActivityReenter(This,pInfo,guidCurrent,dwThread,dwCallDepth)

#define IComActivityEvents_OnActivityLeave(This,pInfo,guidCurrent,guidLeft)	\
    (This)->lpVtbl -> OnActivityLeave(This,pInfo,guidCurrent,guidLeft)

#define IComActivityEvents_OnActivityLeaveSame(This,pInfo,guidCurrent,dwCallDepth)	\
    (This)->lpVtbl -> OnActivityLeaveSame(This,pInfo,guidCurrent,dwCallDepth)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityCreate_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidActivity);


void __RPC_STUB IComActivityEvents_OnActivityCreate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityDestroy_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidActivity);


void __RPC_STUB IComActivityEvents_OnActivityDestroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityEnter_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidCurrent,
    /* [in] */ REFGUID guidEntered,
    /* [in] */ DWORD dwThread);


void __RPC_STUB IComActivityEvents_OnActivityEnter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityTimeout_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidCurrent,
    /* [in] */ REFGUID guidEntered,
    /* [in] */ DWORD dwThread,
    /* [in] */ DWORD dwTimeout);


void __RPC_STUB IComActivityEvents_OnActivityTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityReenter_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidCurrent,
    /* [in] */ DWORD dwThread,
    /* [in] */ DWORD dwCallDepth);


void __RPC_STUB IComActivityEvents_OnActivityReenter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityLeave_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidCurrent,
    /* [in] */ REFGUID guidLeft);


void __RPC_STUB IComActivityEvents_OnActivityLeave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComActivityEvents_OnActivityLeaveSame_Proxy( 
    IComActivityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidCurrent,
    /* [in] */ DWORD dwCallDepth);


void __RPC_STUB IComActivityEvents_OnActivityLeaveSame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComActivityEvents_INTERFACE_DEFINED__ */


#ifndef __IComIdentityEvents_INTERFACE_DEFINED__
#define __IComIdentityEvents_INTERFACE_DEFINED__

/* interface IComIdentityEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComIdentityEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130B1-2E50-11d2-98A5-00C04F8EE1C4")
    IComIdentityEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnIISRequestInfo( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjId,
            /* [in] */ LPCOLESTR pszClientIP,
            /* [in] */ LPCOLESTR pszServerIP,
            /* [in] */ LPCOLESTR pszURL) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComIdentityEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComIdentityEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComIdentityEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComIdentityEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnIISRequestInfo )( 
            IComIdentityEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 ObjId,
            /* [in] */ LPCOLESTR pszClientIP,
            /* [in] */ LPCOLESTR pszServerIP,
            /* [in] */ LPCOLESTR pszURL);
        
        END_INTERFACE
    } IComIdentityEventsVtbl;

    interface IComIdentityEvents
    {
        CONST_VTBL struct IComIdentityEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComIdentityEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComIdentityEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComIdentityEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComIdentityEvents_OnIISRequestInfo(This,pInfo,ObjId,pszClientIP,pszServerIP,pszURL)	\
    (This)->lpVtbl -> OnIISRequestInfo(This,pInfo,ObjId,pszClientIP,pszServerIP,pszURL)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComIdentityEvents_OnIISRequestInfo_Proxy( 
    IComIdentityEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 ObjId,
    /* [in] */ LPCOLESTR pszClientIP,
    /* [in] */ LPCOLESTR pszServerIP,
    /* [in] */ LPCOLESTR pszURL);


void __RPC_STUB IComIdentityEvents_OnIISRequestInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComIdentityEvents_INTERFACE_DEFINED__ */


#ifndef __IComQCEvents_INTERFACE_DEFINED__
#define __IComQCEvents_INTERFACE_DEFINED__

/* interface IComQCEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComQCEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130B2-2E50-11d2-98A5-00C04F8EE1C4")
    IComQCEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnQCRecord( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 objid,
            /* [in] */ WCHAR __RPC_FAR szQueue[ 60 ],
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ HRESULT msmqhr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnQCQueueOpen( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ WCHAR __RPC_FAR szQueue[ 60 ],
            /* [in] */ ULONG64 QueueID,
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnQCReceive( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 QueueID,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnQCReceiveFail( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 QueueID,
            /* [in] */ HRESULT msmqhr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnQCMoveToReTryQueue( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ ULONG RetryIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnQCMoveToDeadQueue( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnQCPlayback( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 objid,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ HRESULT hr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComQCEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComQCEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComQCEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCRecord )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 objid,
            /* [in] */ WCHAR __RPC_FAR szQueue[ 60 ],
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ HRESULT msmqhr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCQueueOpen )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ WCHAR __RPC_FAR szQueue[ 60 ],
            /* [in] */ ULONG64 QueueID,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCReceive )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 QueueID,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCReceiveFail )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 QueueID,
            /* [in] */ HRESULT msmqhr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCMoveToReTryQueue )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ ULONG RetryIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCMoveToDeadQueue )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnQCPlayback )( 
            IComQCEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG64 objid,
            /* [in] */ REFGUID guidMsgId,
            /* [in] */ REFGUID guidWorkFlowId,
            /* [in] */ HRESULT hr);
        
        END_INTERFACE
    } IComQCEventsVtbl;

    interface IComQCEvents
    {
        CONST_VTBL struct IComQCEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComQCEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComQCEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComQCEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComQCEvents_OnQCRecord(This,pInfo,objid,szQueue,guidMsgId,guidWorkFlowId,msmqhr)	\
    (This)->lpVtbl -> OnQCRecord(This,pInfo,objid,szQueue,guidMsgId,guidWorkFlowId,msmqhr)

#define IComQCEvents_OnQCQueueOpen(This,pInfo,szQueue,QueueID,hr)	\
    (This)->lpVtbl -> OnQCQueueOpen(This,pInfo,szQueue,QueueID,hr)

#define IComQCEvents_OnQCReceive(This,pInfo,QueueID,guidMsgId,guidWorkFlowId,hr)	\
    (This)->lpVtbl -> OnQCReceive(This,pInfo,QueueID,guidMsgId,guidWorkFlowId,hr)

#define IComQCEvents_OnQCReceiveFail(This,pInfo,QueueID,msmqhr)	\
    (This)->lpVtbl -> OnQCReceiveFail(This,pInfo,QueueID,msmqhr)

#define IComQCEvents_OnQCMoveToReTryQueue(This,pInfo,guidMsgId,guidWorkFlowId,RetryIndex)	\
    (This)->lpVtbl -> OnQCMoveToReTryQueue(This,pInfo,guidMsgId,guidWorkFlowId,RetryIndex)

#define IComQCEvents_OnQCMoveToDeadQueue(This,pInfo,guidMsgId,guidWorkFlowId)	\
    (This)->lpVtbl -> OnQCMoveToDeadQueue(This,pInfo,guidMsgId,guidWorkFlowId)

#define IComQCEvents_OnQCPlayback(This,pInfo,objid,guidMsgId,guidWorkFlowId,hr)	\
    (This)->lpVtbl -> OnQCPlayback(This,pInfo,objid,guidMsgId,guidWorkFlowId,hr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCRecord_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 objid,
    /* [in] */ WCHAR __RPC_FAR szQueue[ 60 ],
    /* [in] */ REFGUID guidMsgId,
    /* [in] */ REFGUID guidWorkFlowId,
    /* [in] */ HRESULT msmqhr);


void __RPC_STUB IComQCEvents_OnQCRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCQueueOpen_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ WCHAR __RPC_FAR szQueue[ 60 ],
    /* [in] */ ULONG64 QueueID,
    /* [in] */ HRESULT hr);


void __RPC_STUB IComQCEvents_OnQCQueueOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCReceive_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 QueueID,
    /* [in] */ REFGUID guidMsgId,
    /* [in] */ REFGUID guidWorkFlowId,
    /* [in] */ HRESULT hr);


void __RPC_STUB IComQCEvents_OnQCReceive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCReceiveFail_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 QueueID,
    /* [in] */ HRESULT msmqhr);


void __RPC_STUB IComQCEvents_OnQCReceiveFail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCMoveToReTryQueue_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidMsgId,
    /* [in] */ REFGUID guidWorkFlowId,
    /* [in] */ ULONG RetryIndex);


void __RPC_STUB IComQCEvents_OnQCMoveToReTryQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCMoveToDeadQueue_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ REFGUID guidMsgId,
    /* [in] */ REFGUID guidWorkFlowId);


void __RPC_STUB IComQCEvents_OnQCMoveToDeadQueue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComQCEvents_OnQCPlayback_Proxy( 
    IComQCEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG64 objid,
    /* [in] */ REFGUID guidMsgId,
    /* [in] */ REFGUID guidWorkFlowId,
    /* [in] */ HRESULT hr);


void __RPC_STUB IComQCEvents_OnQCPlayback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComQCEvents_INTERFACE_DEFINED__ */


#ifndef __IComExceptionEvents_INTERFACE_DEFINED__
#define __IComExceptionEvents_INTERFACE_DEFINED__

/* interface IComExceptionEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComExceptionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130B3-2E50-11d2-98A5-00C04F8EE1C4")
    IComExceptionEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnExceptionUser( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG code,
            /* [in] */ ULONG64 address,
            /* [in] */ LPCOLESTR pszStackTrace) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComExceptionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComExceptionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComExceptionEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComExceptionEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnExceptionUser )( 
            IComExceptionEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ ULONG code,
            /* [in] */ ULONG64 address,
            /* [in] */ LPCOLESTR pszStackTrace);
        
        END_INTERFACE
    } IComExceptionEventsVtbl;

    interface IComExceptionEvents
    {
        CONST_VTBL struct IComExceptionEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComExceptionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComExceptionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComExceptionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComExceptionEvents_OnExceptionUser(This,pInfo,code,address,pszStackTrace)	\
    (This)->lpVtbl -> OnExceptionUser(This,pInfo,code,address,pszStackTrace)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComExceptionEvents_OnExceptionUser_Proxy( 
    IComExceptionEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ ULONG code,
    /* [in] */ ULONG64 address,
    /* [in] */ LPCOLESTR pszStackTrace);


void __RPC_STUB IComExceptionEvents_OnExceptionUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComExceptionEvents_INTERFACE_DEFINED__ */


#ifndef __ILBEvents_INTERFACE_DEFINED__
#define __ILBEvents_INTERFACE_DEFINED__

/* interface ILBEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_ILBEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130B4-2E50-11d2-98A5-00C04F8EE1C4")
    ILBEvents : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TargetUp( 
            BSTR bstrServerName,
            BSTR bstrClsidEng) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TargetDown( 
            BSTR bstrServerName,
            BSTR bstrClsidEng) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EngineDefined( 
            BSTR bstrPropName,
            VARIANT __RPC_FAR *varPropValue,
            BSTR bstrClsidEng) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILBEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ILBEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ILBEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ILBEvents __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TargetUp )( 
            ILBEvents __RPC_FAR * This,
            BSTR bstrServerName,
            BSTR bstrClsidEng);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TargetDown )( 
            ILBEvents __RPC_FAR * This,
            BSTR bstrServerName,
            BSTR bstrClsidEng);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EngineDefined )( 
            ILBEvents __RPC_FAR * This,
            BSTR bstrPropName,
            VARIANT __RPC_FAR *varPropValue,
            BSTR bstrClsidEng);
        
        END_INTERFACE
    } ILBEventsVtbl;

    interface ILBEvents
    {
        CONST_VTBL struct ILBEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILBEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILBEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILBEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILBEvents_TargetUp(This,bstrServerName,bstrClsidEng)	\
    (This)->lpVtbl -> TargetUp(This,bstrServerName,bstrClsidEng)

#define ILBEvents_TargetDown(This,bstrServerName,bstrClsidEng)	\
    (This)->lpVtbl -> TargetDown(This,bstrServerName,bstrClsidEng)

#define ILBEvents_EngineDefined(This,bstrPropName,varPropValue,bstrClsidEng)	\
    (This)->lpVtbl -> EngineDefined(This,bstrPropName,varPropValue,bstrClsidEng)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILBEvents_TargetUp_Proxy( 
    ILBEvents __RPC_FAR * This,
    BSTR bstrServerName,
    BSTR bstrClsidEng);


void __RPC_STUB ILBEvents_TargetUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILBEvents_TargetDown_Proxy( 
    ILBEvents __RPC_FAR * This,
    BSTR bstrServerName,
    BSTR bstrClsidEng);


void __RPC_STUB ILBEvents_TargetDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILBEvents_EngineDefined_Proxy( 
    ILBEvents __RPC_FAR * This,
    BSTR bstrPropName,
    VARIANT __RPC_FAR *varPropValue,
    BSTR bstrClsidEng);


void __RPC_STUB ILBEvents_EngineDefined_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILBEvents_INTERFACE_DEFINED__ */


#ifndef __IComCRMEvents_INTERFACE_DEFINED__
#define __IComCRMEvents_INTERFACE_DEFINED__

/* interface IComCRMEvents */
/* [uuid][hidden][object] */ 


EXTERN_C const IID IID_IComCRMEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("683130B5-2E50-11d2-98A5-00C04F8EE1C4")
    IComCRMEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnCRMRecoveryStart( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMRecoveryDone( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMCheckpoint( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMBegin( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ GUID guidActivity,
            /* [in] */ GUID guidTx,
            /* [in] */ WCHAR __RPC_FAR szProgIdCompensator[ 64 ],
            /* [in] */ WCHAR __RPC_FAR szDescription[ 64 ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMPrepare( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMCommit( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMAbort( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMIndoubt( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMDone( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMRelease( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMAnalyze( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ DWORD dwCrmRecordType,
            /* [in] */ DWORD dwRecordSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMWrite( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ BOOL fVariants,
            /* [in] */ DWORD dwRecordSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMForget( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMForce( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnCRMDeliver( 
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ BOOL fVariants,
            /* [in] */ DWORD dwRecordSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IComCRMEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IComCRMEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IComCRMEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMRecoveryStart )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMRecoveryDone )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMCheckpoint )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidApp);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMBegin )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ GUID guidActivity,
            /* [in] */ GUID guidTx,
            /* [in] */ WCHAR __RPC_FAR szProgIdCompensator[ 64 ],
            /* [in] */ WCHAR __RPC_FAR szDescription[ 64 ]);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMPrepare )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMCommit )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMAbort )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMIndoubt )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMDone )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMRelease )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMAnalyze )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ DWORD dwCrmRecordType,
            /* [in] */ DWORD dwRecordSize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMWrite )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ BOOL fVariants,
            /* [in] */ DWORD dwRecordSize);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMForget )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMForce )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnCRMDeliver )( 
            IComCRMEvents __RPC_FAR * This,
            /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
            /* [in] */ GUID guidClerkCLSID,
            /* [in] */ BOOL fVariants,
            /* [in] */ DWORD dwRecordSize);
        
        END_INTERFACE
    } IComCRMEventsVtbl;

    interface IComCRMEvents
    {
        CONST_VTBL struct IComCRMEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComCRMEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IComCRMEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IComCRMEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IComCRMEvents_OnCRMRecoveryStart(This,pInfo,guidApp)	\
    (This)->lpVtbl -> OnCRMRecoveryStart(This,pInfo,guidApp)

#define IComCRMEvents_OnCRMRecoveryDone(This,pInfo,guidApp)	\
    (This)->lpVtbl -> OnCRMRecoveryDone(This,pInfo,guidApp)

#define IComCRMEvents_OnCRMCheckpoint(This,pInfo,guidApp)	\
    (This)->lpVtbl -> OnCRMCheckpoint(This,pInfo,guidApp)

#define IComCRMEvents_OnCRMBegin(This,pInfo,guidClerkCLSID,guidActivity,guidTx,szProgIdCompensator,szDescription)	\
    (This)->lpVtbl -> OnCRMBegin(This,pInfo,guidClerkCLSID,guidActivity,guidTx,szProgIdCompensator,szDescription)

#define IComCRMEvents_OnCRMPrepare(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMPrepare(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMCommit(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMCommit(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMAbort(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMAbort(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMIndoubt(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMIndoubt(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMDone(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMDone(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMRelease(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMRelease(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMAnalyze(This,pInfo,guidClerkCLSID,dwCrmRecordType,dwRecordSize)	\
    (This)->lpVtbl -> OnCRMAnalyze(This,pInfo,guidClerkCLSID,dwCrmRecordType,dwRecordSize)

#define IComCRMEvents_OnCRMWrite(This,pInfo,guidClerkCLSID,fVariants,dwRecordSize)	\
    (This)->lpVtbl -> OnCRMWrite(This,pInfo,guidClerkCLSID,fVariants,dwRecordSize)

#define IComCRMEvents_OnCRMForget(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMForget(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMForce(This,pInfo,guidClerkCLSID)	\
    (This)->lpVtbl -> OnCRMForce(This,pInfo,guidClerkCLSID)

#define IComCRMEvents_OnCRMDeliver(This,pInfo,guidClerkCLSID,fVariants,dwRecordSize)	\
    (This)->lpVtbl -> OnCRMDeliver(This,pInfo,guidClerkCLSID,fVariants,dwRecordSize)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMRecoveryStart_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidApp);


void __RPC_STUB IComCRMEvents_OnCRMRecoveryStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMRecoveryDone_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidApp);


void __RPC_STUB IComCRMEvents_OnCRMRecoveryDone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMCheckpoint_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidApp);


void __RPC_STUB IComCRMEvents_OnCRMCheckpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMBegin_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID,
    /* [in] */ GUID guidActivity,
    /* [in] */ GUID guidTx,
    /* [in] */ WCHAR __RPC_FAR szProgIdCompensator[ 64 ],
    /* [in] */ WCHAR __RPC_FAR szDescription[ 64 ]);


void __RPC_STUB IComCRMEvents_OnCRMBegin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMPrepare_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMPrepare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMCommit_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMAbort_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMIndoubt_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMIndoubt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMDone_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMDone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMRelease_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMRelease_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMAnalyze_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID,
    /* [in] */ DWORD dwCrmRecordType,
    /* [in] */ DWORD dwRecordSize);


void __RPC_STUB IComCRMEvents_OnCRMAnalyze_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMWrite_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID,
    /* [in] */ BOOL fVariants,
    /* [in] */ DWORD dwRecordSize);


void __RPC_STUB IComCRMEvents_OnCRMWrite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMForget_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMForget_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMForce_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID);


void __RPC_STUB IComCRMEvents_OnCRMForce_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IComCRMEvents_OnCRMDeliver_Proxy( 
    IComCRMEvents __RPC_FAR * This,
    /* [in] */ COMSVCSEVENTINFO __RPC_FAR *pInfo,
    /* [in] */ GUID guidClerkCLSID,
    /* [in] */ BOOL fVariants,
    /* [in] */ DWORD dwRecordSize);


void __RPC_STUB IComCRMEvents_OnCRMDeliver_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IComCRMEvents_INTERFACE_DEFINED__ */


#ifndef __IMtsEvents_INTERFACE_DEFINED__
#define __IMtsEvents_INTERFACE_DEFINED__

/* interface IMtsEvents */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMtsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BACEDF4D-74AB-11D0-B162-00AA00BA3258")
    IMtsEvents : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PackageName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PackageGuid( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PostEvent( 
            /* [in] */ VARIANT __RPC_FAR *vEvent) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FireEvents( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProcessID( 
            /* [retval][out] */ long __RPC_FAR *id) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMtsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMtsEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMtsEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMtsEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMtsEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMtsEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMtsEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMtsEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PackageName )( 
            IMtsEvents __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PackageGuid )( 
            IMtsEvents __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PostEvent )( 
            IMtsEvents __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *vEvent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FireEvents )( 
            IMtsEvents __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProcessID )( 
            IMtsEvents __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *id);
        
        END_INTERFACE
    } IMtsEventsVtbl;

    interface IMtsEvents
    {
        CONST_VTBL struct IMtsEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMtsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMtsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMtsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMtsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMtsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMtsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMtsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMtsEvents_get_PackageName(This,pVal)	\
    (This)->lpVtbl -> get_PackageName(This,pVal)

#define IMtsEvents_get_PackageGuid(This,pVal)	\
    (This)->lpVtbl -> get_PackageGuid(This,pVal)

#define IMtsEvents_PostEvent(This,vEvent)	\
    (This)->lpVtbl -> PostEvent(This,vEvent)

#define IMtsEvents_get_FireEvents(This,pVal)	\
    (This)->lpVtbl -> get_FireEvents(This,pVal)

#define IMtsEvents_GetProcessID(This,id)	\
    (This)->lpVtbl -> GetProcessID(This,id)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMtsEvents_get_PackageName_Proxy( 
    IMtsEvents __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IMtsEvents_get_PackageName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMtsEvents_get_PackageGuid_Proxy( 
    IMtsEvents __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IMtsEvents_get_PackageGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMtsEvents_PostEvent_Proxy( 
    IMtsEvents __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *vEvent);


void __RPC_STUB IMtsEvents_PostEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMtsEvents_get_FireEvents_Proxy( 
    IMtsEvents __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IMtsEvents_get_FireEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMtsEvents_GetProcessID_Proxy( 
    IMtsEvents __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *id);


void __RPC_STUB IMtsEvents_GetProcessID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMtsEvents_INTERFACE_DEFINED__ */


#ifndef __IMtsEventInfo_INTERFACE_DEFINED__
#define __IMtsEventInfo_INTERFACE_DEFINED__

/* interface IMtsEventInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMtsEventInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D56C3DC1-8482-11d0-B170-00AA00BA3258")
    IMtsEventInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Names( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pUnk) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DisplayName( 
            /* [retval][out] */ BSTR __RPC_FAR *sDisplayName) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_EventID( 
            /* [retval][out] */ BSTR __RPC_FAR *sGuidEventID) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *lCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR sKey,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMtsEventInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMtsEventInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMtsEventInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Names )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pUnk);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DisplayName )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *sDisplayName);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventID )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *sGuidEventID);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *lCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IMtsEventInfo __RPC_FAR * This,
            /* [in] */ BSTR sKey,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } IMtsEventInfoVtbl;

    interface IMtsEventInfo
    {
        CONST_VTBL struct IMtsEventInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMtsEventInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMtsEventInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMtsEventInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMtsEventInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMtsEventInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMtsEventInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMtsEventInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMtsEventInfo_get_Names(This,pUnk)	\
    (This)->lpVtbl -> get_Names(This,pUnk)

#define IMtsEventInfo_get_DisplayName(This,sDisplayName)	\
    (This)->lpVtbl -> get_DisplayName(This,sDisplayName)

#define IMtsEventInfo_get_EventID(This,sGuidEventID)	\
    (This)->lpVtbl -> get_EventID(This,sGuidEventID)

#define IMtsEventInfo_get_Count(This,lCount)	\
    (This)->lpVtbl -> get_Count(This,lCount)

#define IMtsEventInfo_get_Value(This,sKey,pVal)	\
    (This)->lpVtbl -> get_Value(This,sKey,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IMtsEventInfo_get_Names_Proxy( 
    IMtsEventInfo __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pUnk);


void __RPC_STUB IMtsEventInfo_get_Names_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMtsEventInfo_get_DisplayName_Proxy( 
    IMtsEventInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *sDisplayName);


void __RPC_STUB IMtsEventInfo_get_DisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IMtsEventInfo_get_EventID_Proxy( 
    IMtsEventInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *sGuidEventID);


void __RPC_STUB IMtsEventInfo_get_EventID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IMtsEventInfo_get_Count_Proxy( 
    IMtsEventInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *lCount);


void __RPC_STUB IMtsEventInfo_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IMtsEventInfo_get_Value_Proxy( 
    IMtsEventInfo __RPC_FAR * This,
    /* [in] */ BSTR sKey,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IMtsEventInfo_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMtsEventInfo_INTERFACE_DEFINED__ */


#ifndef __IMTSLocator_INTERFACE_DEFINED__
#define __IMTSLocator_INTERFACE_DEFINED__

/* interface IMTSLocator */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMTSLocator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D19B8BFD-7F88-11D0-B16E-00AA00BA3258")
    IMTSLocator : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEventDispatcher( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMTSLocatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMTSLocator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMTSLocator __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMTSLocator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMTSLocator __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMTSLocator __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMTSLocator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMTSLocator __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventDispatcher )( 
            IMTSLocator __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pUnk);
        
        END_INTERFACE
    } IMTSLocatorVtbl;

    interface IMTSLocator
    {
        CONST_VTBL struct IMTSLocatorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMTSLocator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMTSLocator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMTSLocator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMTSLocator_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMTSLocator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMTSLocator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMTSLocator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMTSLocator_GetEventDispatcher(This,pUnk)	\
    (This)->lpVtbl -> GetEventDispatcher(This,pUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMTSLocator_GetEventDispatcher_Proxy( 
    IMTSLocator __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pUnk);


void __RPC_STUB IMTSLocator_GetEventDispatcher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMTSLocator_INTERFACE_DEFINED__ */


#ifndef __IMtsGrp_INTERFACE_DEFINED__
#define __IMtsGrp_INTERFACE_DEFINED__

/* interface IMtsGrp */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IMtsGrp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4B2E958C-0393-11D1-B1AB-00AA00BA3258")
    IMtsGrp : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ long lIndex,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkDispatcher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMtsGrpVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMtsGrp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMtsGrp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMtsGrp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMtsGrp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMtsGrp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMtsGrp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMtsGrp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IMtsGrp __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IMtsGrp __RPC_FAR * This,
            /* [in] */ long lIndex,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkDispatcher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IMtsGrp __RPC_FAR * This);
        
        END_INTERFACE
    } IMtsGrpVtbl;

    interface IMtsGrp
    {
        CONST_VTBL struct IMtsGrpVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMtsGrp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMtsGrp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMtsGrp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMtsGrp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMtsGrp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMtsGrp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMtsGrp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMtsGrp_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IMtsGrp_Item(This,lIndex,ppUnkDispatcher)	\
    (This)->lpVtbl -> Item(This,lIndex,ppUnkDispatcher)

#define IMtsGrp_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IMtsGrp_get_Count_Proxy( 
    IMtsGrp __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IMtsGrp_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMtsGrp_Item_Proxy( 
    IMtsGrp __RPC_FAR * This,
    /* [in] */ long lIndex,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkDispatcher);


void __RPC_STUB IMtsGrp_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IMtsGrp_Refresh_Proxy( 
    IMtsGrp __RPC_FAR * This);


void __RPC_STUB IMtsGrp_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMtsGrp_INTERFACE_DEFINED__ */


#ifndef __IMessageMover_INTERFACE_DEFINED__
#define __IMessageMover_INTERFACE_DEFINED__

/* interface IMessageMover */
/* [unique][dual][nonextensible][oleautomation][hidden][object][helpstring][uuid] */ 


EXTERN_C const IID IID_IMessageMover;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("588A085A-B795-11D1-8054-00C04FC340EE")
    IMessageMover : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SourcePath( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_SourcePath( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_DestPath( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_DestPath( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_CommitBatchSize( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_CommitBatchSize( 
            /* [in] */ long newVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE MoveMessages( 
            /* [retval][out] */ long __RPC_FAR *plMessagesMoved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMessageMoverVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMessageMover __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMessageMover __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IMessageMover __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourcePath )( 
            IMessageMover __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourcePath )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DestPath )( 
            IMessageMover __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DestPath )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommitBatchSize )( 
            IMessageMover __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommitBatchSize )( 
            IMessageMover __RPC_FAR * This,
            /* [in] */ long newVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveMessages )( 
            IMessageMover __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plMessagesMoved);
        
        END_INTERFACE
    } IMessageMoverVtbl;

    interface IMessageMover
    {
        CONST_VTBL struct IMessageMoverVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMessageMover_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMessageMover_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMessageMover_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMessageMover_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IMessageMover_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IMessageMover_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IMessageMover_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IMessageMover_get_SourcePath(This,pVal)	\
    (This)->lpVtbl -> get_SourcePath(This,pVal)

#define IMessageMover_put_SourcePath(This,newVal)	\
    (This)->lpVtbl -> put_SourcePath(This,newVal)

#define IMessageMover_get_DestPath(This,pVal)	\
    (This)->lpVtbl -> get_DestPath(This,pVal)

#define IMessageMover_put_DestPath(This,newVal)	\
    (This)->lpVtbl -> put_DestPath(This,newVal)

#define IMessageMover_get_CommitBatchSize(This,pVal)	\
    (This)->lpVtbl -> get_CommitBatchSize(This,pVal)

#define IMessageMover_put_CommitBatchSize(This,newVal)	\
    (This)->lpVtbl -> put_CommitBatchSize(This,newVal)

#define IMessageMover_MoveMessages(This,plMessagesMoved)	\
    (This)->lpVtbl -> MoveMessages(This,plMessagesMoved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IMessageMover_get_SourcePath_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IMessageMover_get_SourcePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IMessageMover_put_SourcePath_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IMessageMover_put_SourcePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IMessageMover_get_DestPath_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IMessageMover_get_DestPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IMessageMover_put_DestPath_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IMessageMover_put_DestPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IMessageMover_get_CommitBatchSize_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IMessageMover_get_CommitBatchSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IMessageMover_put_CommitBatchSize_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [in] */ long newVal);


void __RPC_STUB IMessageMover_put_CommitBatchSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IMessageMover_MoveMessages_Proxy( 
    IMessageMover __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plMessagesMoved);


void __RPC_STUB IMessageMover_MoveMessages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMessageMover_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0302 */
/* [local] */ 

#ifndef GUID_STRING_SIZE
#define GUID_STRING_SIZE				40	    // a couple over.
#endif
typedef /* [hidden] */ struct CAppStatistics
    {
    DWORD m_cTotalCalls;
    DWORD m_cTotalInstances;
    DWORD m_cTotalClasses;
    DWORD m_cCallsPerSecond;
    }	APPSTATISTICS;

typedef /* [hidden] */ struct CAppData
    {
    DWORD m_idApp;
    WCHAR m_szAppGuid[ 40 ];
    DWORD m_dwAppProcessId;
    APPSTATISTICS m_AppStatistics;
    }	APPDATA;

typedef /* [hidden] */ struct CCLSIDData
    {
    CLSID m_clsid;
    DWORD m_cReferences;
    DWORD m_cBound;
    DWORD m_cPooled;
    DWORD m_cInCall;
    DWORD m_dwRespTime;
    DWORD m_cCallsCompleted;
    DWORD m_cCallsFailed;
    }	CLSIDDATA;

//
// Dispenser Manager interfaces
//
//  Copyright (C) 1995-1999 Microsoft Corporation.  All rights reserved.
 
#ifndef DECLSPEC_UUID
#if _MSC_VER >= 1100
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif
typedef DWORD_PTR RESTYPID;

typedef DWORD_PTR RESID;

typedef LPOLESTR SRESID;

typedef LPCOLESTR constSRESID;

typedef DWORD RESOURCERATING;

typedef long TIMEINSECS;

typedef DWORD_PTR INSTID;

typedef DWORD_PTR TRANSID;



//
// Error Codes
//
#define MTXDM_E_ENLISTRESOURCEFAILED 0x8004E100 // return from EnlistResource, is then returned by AllocResource
 
//
// IDispenserManager
// Implemented by Dispenser Manager, called by all Dispensers.
//


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0302_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0302_v0_0_s_ifspec;

#ifndef __IDispenserManager_INTERFACE_DEFINED__
#define __IDispenserManager_INTERFACE_DEFINED__

/* interface IDispenserManager */
/* [unique][hidden][local][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDispenserManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5cb31e10-2b5f-11cf-be10-00aa00a2fa25")
    IDispenserManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterDispenser( 
            /* [in] */ IDispenserDriver __RPC_FAR *__MIDL_0011,
            /* [in] */ LPCOLESTR szDispenserName,
            /* [out] */ IHolder __RPC_FAR *__RPC_FAR *__MIDL_0012) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContext( 
            /* [out] */ INSTID __RPC_FAR *__MIDL_0013,
            /* [out] */ TRANSID __RPC_FAR *__MIDL_0014) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDispenserManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDispenserManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDispenserManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDispenserManager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterDispenser )( 
            IDispenserManager __RPC_FAR * This,
            /* [in] */ IDispenserDriver __RPC_FAR *__MIDL_0011,
            /* [in] */ LPCOLESTR szDispenserName,
            /* [out] */ IHolder __RPC_FAR *__RPC_FAR *__MIDL_0012);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContext )( 
            IDispenserManager __RPC_FAR * This,
            /* [out] */ INSTID __RPC_FAR *__MIDL_0013,
            /* [out] */ TRANSID __RPC_FAR *__MIDL_0014);
        
        END_INTERFACE
    } IDispenserManagerVtbl;

    interface IDispenserManager
    {
        CONST_VTBL struct IDispenserManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDispenserManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDispenserManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDispenserManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDispenserManager_RegisterDispenser(This,__MIDL_0011,szDispenserName,__MIDL_0012)	\
    (This)->lpVtbl -> RegisterDispenser(This,__MIDL_0011,szDispenserName,__MIDL_0012)

#define IDispenserManager_GetContext(This,__MIDL_0013,__MIDL_0014)	\
    (This)->lpVtbl -> GetContext(This,__MIDL_0013,__MIDL_0014)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDispenserManager_RegisterDispenser_Proxy( 
    IDispenserManager __RPC_FAR * This,
    /* [in] */ IDispenserDriver __RPC_FAR *__MIDL_0011,
    /* [in] */ LPCOLESTR szDispenserName,
    /* [out] */ IHolder __RPC_FAR *__RPC_FAR *__MIDL_0012);


void __RPC_STUB IDispenserManager_RegisterDispenser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDispenserManager_GetContext_Proxy( 
    IDispenserManager __RPC_FAR * This,
    /* [out] */ INSTID __RPC_FAR *__MIDL_0013,
    /* [out] */ TRANSID __RPC_FAR *__MIDL_0014);


void __RPC_STUB IDispenserManager_GetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDispenserManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0306 */
/* [local] */ 

//
// IHolder
// Implemented by Dispenser Manager, called by one Dispenser.
//


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0306_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0306_v0_0_s_ifspec;

#ifndef __IHolder_INTERFACE_DEFINED__
#define __IHolder_INTERFACE_DEFINED__

/* interface IHolder */
/* [unique][hidden][local][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bf6a1850-2b45-11cf-be10-00aa00a2fa25")
    IHolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AllocResource( 
            /* [in] */ const RESTYPID __MIDL_0015,
            /* [out] */ RESID __RPC_FAR *__MIDL_0016) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FreeResource( 
            /* [in] */ const RESID __MIDL_0017) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TrackResource( 
            /* [in] */ const RESID __MIDL_0018) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TrackResourceS( 
            /* [in] */ constSRESID __MIDL_0019) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UntrackResource( 
            /* [in] */ const RESID __MIDL_0020,
            /* [in] */ const BOOL __MIDL_0021) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UntrackResourceS( 
            /* [in] */ constSRESID __MIDL_0022,
            /* [in] */ const BOOL __MIDL_0023) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RequestDestroyResource( 
            /* [in] */ const RESID __MIDL_0024) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHolder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHolder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHolder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AllocResource )( 
            IHolder __RPC_FAR * This,
            /* [in] */ const RESTYPID __MIDL_0015,
            /* [out] */ RESID __RPC_FAR *__MIDL_0016);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FreeResource )( 
            IHolder __RPC_FAR * This,
            /* [in] */ const RESID __MIDL_0017);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TrackResource )( 
            IHolder __RPC_FAR * This,
            /* [in] */ const RESID __MIDL_0018);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TrackResourceS )( 
            IHolder __RPC_FAR * This,
            /* [in] */ constSRESID __MIDL_0019);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UntrackResource )( 
            IHolder __RPC_FAR * This,
            /* [in] */ const RESID __MIDL_0020,
            /* [in] */ const BOOL __MIDL_0021);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UntrackResourceS )( 
            IHolder __RPC_FAR * This,
            /* [in] */ constSRESID __MIDL_0022,
            /* [in] */ const BOOL __MIDL_0023);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IHolder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RequestDestroyResource )( 
            IHolder __RPC_FAR * This,
            /* [in] */ const RESID __MIDL_0024);
        
        END_INTERFACE
    } IHolderVtbl;

    interface IHolder
    {
        CONST_VTBL struct IHolderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHolder_AllocResource(This,__MIDL_0015,__MIDL_0016)	\
    (This)->lpVtbl -> AllocResource(This,__MIDL_0015,__MIDL_0016)

#define IHolder_FreeResource(This,__MIDL_0017)	\
    (This)->lpVtbl -> FreeResource(This,__MIDL_0017)

#define IHolder_TrackResource(This,__MIDL_0018)	\
    (This)->lpVtbl -> TrackResource(This,__MIDL_0018)

#define IHolder_TrackResourceS(This,__MIDL_0019)	\
    (This)->lpVtbl -> TrackResourceS(This,__MIDL_0019)

#define IHolder_UntrackResource(This,__MIDL_0020,__MIDL_0021)	\
    (This)->lpVtbl -> UntrackResource(This,__MIDL_0020,__MIDL_0021)

#define IHolder_UntrackResourceS(This,__MIDL_0022,__MIDL_0023)	\
    (This)->lpVtbl -> UntrackResourceS(This,__MIDL_0022,__MIDL_0023)

#define IHolder_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IHolder_RequestDestroyResource(This,__MIDL_0024)	\
    (This)->lpVtbl -> RequestDestroyResource(This,__MIDL_0024)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHolder_AllocResource_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ const RESTYPID __MIDL_0015,
    /* [out] */ RESID __RPC_FAR *__MIDL_0016);


void __RPC_STUB IHolder_AllocResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_FreeResource_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ const RESID __MIDL_0017);


void __RPC_STUB IHolder_FreeResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_TrackResource_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ const RESID __MIDL_0018);


void __RPC_STUB IHolder_TrackResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_TrackResourceS_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ constSRESID __MIDL_0019);


void __RPC_STUB IHolder_TrackResourceS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_UntrackResource_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ const RESID __MIDL_0020,
    /* [in] */ const BOOL __MIDL_0021);


void __RPC_STUB IHolder_UntrackResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_UntrackResourceS_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ constSRESID __MIDL_0022,
    /* [in] */ const BOOL __MIDL_0023);


void __RPC_STUB IHolder_UntrackResourceS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_Close_Proxy( 
    IHolder __RPC_FAR * This);


void __RPC_STUB IHolder_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHolder_RequestDestroyResource_Proxy( 
    IHolder __RPC_FAR * This,
    /* [in] */ const RESID __MIDL_0024);


void __RPC_STUB IHolder_RequestDestroyResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHolder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0307 */
/* [local] */ 

//
// IDispenserDriver
// Implemented by a Dispenser, called by Dispenser Manager.
//


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0307_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0307_v0_0_s_ifspec;

#ifndef __IDispenserDriver_INTERFACE_DEFINED__
#define __IDispenserDriver_INTERFACE_DEFINED__

/* interface IDispenserDriver */
/* [unique][hidden][local][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDispenserDriver;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("208b3651-2b48-11cf-be10-00aa00a2fa25")
    IDispenserDriver : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateResource( 
            /* [in] */ const RESTYPID ResTypId,
            /* [out] */ RESID __RPC_FAR *pResId,
            /* [out] */ TIMEINSECS __RPC_FAR *pSecsFreeBeforeDestroy) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RateResource( 
            /* [in] */ const RESTYPID ResTypId,
            /* [in] */ const RESID ResId,
            /* [in] */ const BOOL fRequiresTransactionEnlistment,
            /* [out] */ RESOURCERATING __RPC_FAR *pRating) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnlistResource( 
            /* [in] */ const RESID ResId,
            /* [in] */ const TRANSID TransId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetResource( 
            /* [in] */ const RESID ResId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyResource( 
            /* [in] */ const RESID ResId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyResourceS( 
            /* [in] */ constSRESID ResId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDispenserDriverVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDispenserDriver __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDispenserDriver __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateResource )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ const RESTYPID ResTypId,
            /* [out] */ RESID __RPC_FAR *pResId,
            /* [out] */ TIMEINSECS __RPC_FAR *pSecsFreeBeforeDestroy);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RateResource )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ const RESTYPID ResTypId,
            /* [in] */ const RESID ResId,
            /* [in] */ const BOOL fRequiresTransactionEnlistment,
            /* [out] */ RESOURCERATING __RPC_FAR *pRating);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnlistResource )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ const RESID ResId,
            /* [in] */ const TRANSID TransId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResetResource )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ const RESID ResId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestroyResource )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ const RESID ResId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestroyResourceS )( 
            IDispenserDriver __RPC_FAR * This,
            /* [in] */ constSRESID ResId);
        
        END_INTERFACE
    } IDispenserDriverVtbl;

    interface IDispenserDriver
    {
        CONST_VTBL struct IDispenserDriverVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDispenserDriver_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDispenserDriver_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDispenserDriver_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDispenserDriver_CreateResource(This,ResTypId,pResId,pSecsFreeBeforeDestroy)	\
    (This)->lpVtbl -> CreateResource(This,ResTypId,pResId,pSecsFreeBeforeDestroy)

#define IDispenserDriver_RateResource(This,ResTypId,ResId,fRequiresTransactionEnlistment,pRating)	\
    (This)->lpVtbl -> RateResource(This,ResTypId,ResId,fRequiresTransactionEnlistment,pRating)

#define IDispenserDriver_EnlistResource(This,ResId,TransId)	\
    (This)->lpVtbl -> EnlistResource(This,ResId,TransId)

#define IDispenserDriver_ResetResource(This,ResId)	\
    (This)->lpVtbl -> ResetResource(This,ResId)

#define IDispenserDriver_DestroyResource(This,ResId)	\
    (This)->lpVtbl -> DestroyResource(This,ResId)

#define IDispenserDriver_DestroyResourceS(This,ResId)	\
    (This)->lpVtbl -> DestroyResourceS(This,ResId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDispenserDriver_CreateResource_Proxy( 
    IDispenserDriver __RPC_FAR * This,
    /* [in] */ const RESTYPID ResTypId,
    /* [out] */ RESID __RPC_FAR *pResId,
    /* [out] */ TIMEINSECS __RPC_FAR *pSecsFreeBeforeDestroy);


void __RPC_STUB IDispenserDriver_CreateResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDispenserDriver_RateResource_Proxy( 
    IDispenserDriver __RPC_FAR * This,
    /* [in] */ const RESTYPID ResTypId,
    /* [in] */ const RESID ResId,
    /* [in] */ const BOOL fRequiresTransactionEnlistment,
    /* [out] */ RESOURCERATING __RPC_FAR *pRating);


void __RPC_STUB IDispenserDriver_RateResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDispenserDriver_EnlistResource_Proxy( 
    IDispenserDriver __RPC_FAR * This,
    /* [in] */ const RESID ResId,
    /* [in] */ const TRANSID TransId);


void __RPC_STUB IDispenserDriver_EnlistResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDispenserDriver_ResetResource_Proxy( 
    IDispenserDriver __RPC_FAR * This,
    /* [in] */ const RESID ResId);


void __RPC_STUB IDispenserDriver_ResetResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDispenserDriver_DestroyResource_Proxy( 
    IDispenserDriver __RPC_FAR * This,
    /* [in] */ const RESID ResId);


void __RPC_STUB IDispenserDriver_DestroyResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDispenserDriver_DestroyResourceS_Proxy( 
    IDispenserDriver __RPC_FAR * This,
    /* [in] */ constSRESID ResId);


void __RPC_STUB IDispenserDriver_DestroyResourceS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDispenserDriver_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0308 */
/* [local] */ 

#ifdef USE_UUIDOF_FOR_IID_
#define  IID_IHolder             __uuidof(IIHolder)
#define  IID_IDispenserManager   __uuidof(IDispenserManager)
#define  IID_IDispenserDriver    __uuidof(IDispenserDriver)
#endif
EXTERN_C const CLSID CLSID_MTSPackage;


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0308_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0308_v0_0_s_ifspec;

#ifndef __IObjectContext_INTERFACE_DEFINED__
#define __IObjectContext_INTERFACE_DEFINED__

/* interface IObjectContext */
/* [object][unique][uuid][local] */ 


EXTERN_C const IID IID_IObjectContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372ae0-cae7-11cf-be81-00aa00a2fa25")
    IObjectContext : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [retval][iid_is][out] */ LPVOID __RPC_FAR *ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetComplete( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAbort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableCommit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DisableCommit( void) = 0;
        
        virtual BOOL STDMETHODCALLTYPE IsInTransaction( void) = 0;
        
        virtual BOOL STDMETHODCALLTYPE IsSecurityEnabled( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsCallerInRole( 
            /* [in] */ BSTR bstrRole,
            /* [retval][out] */ BOOL __RPC_FAR *pfIsInRole) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectContext __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectContext __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateInstance )( 
            IObjectContext __RPC_FAR * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ REFIID riid,
            /* [retval][iid_is][out] */ LPVOID __RPC_FAR *ppv);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetComplete )( 
            IObjectContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetAbort )( 
            IObjectContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableCommit )( 
            IObjectContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DisableCommit )( 
            IObjectContext __RPC_FAR * This);
        
        BOOL ( STDMETHODCALLTYPE __RPC_FAR *IsInTransaction )( 
            IObjectContext __RPC_FAR * This);
        
        BOOL ( STDMETHODCALLTYPE __RPC_FAR *IsSecurityEnabled )( 
            IObjectContext __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsCallerInRole )( 
            IObjectContext __RPC_FAR * This,
            /* [in] */ BSTR bstrRole,
            /* [retval][out] */ BOOL __RPC_FAR *pfIsInRole);
        
        END_INTERFACE
    } IObjectContextVtbl;

    interface IObjectContext
    {
        CONST_VTBL struct IObjectContextVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectContext_CreateInstance(This,rclsid,riid,ppv)	\
    (This)->lpVtbl -> CreateInstance(This,rclsid,riid,ppv)

#define IObjectContext_SetComplete(This)	\
    (This)->lpVtbl -> SetComplete(This)

#define IObjectContext_SetAbort(This)	\
    (This)->lpVtbl -> SetAbort(This)

#define IObjectContext_EnableCommit(This)	\
    (This)->lpVtbl -> EnableCommit(This)

#define IObjectContext_DisableCommit(This)	\
    (This)->lpVtbl -> DisableCommit(This)

#define IObjectContext_IsInTransaction(This)	\
    (This)->lpVtbl -> IsInTransaction(This)

#define IObjectContext_IsSecurityEnabled(This)	\
    (This)->lpVtbl -> IsSecurityEnabled(This)

#define IObjectContext_IsCallerInRole(This,bstrRole,pfIsInRole)	\
    (This)->lpVtbl -> IsCallerInRole(This,bstrRole,pfIsInRole)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectContext_CreateInstance_Proxy( 
    IObjectContext __RPC_FAR * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ REFIID riid,
    /* [retval][iid_is][out] */ LPVOID __RPC_FAR *ppv);


void __RPC_STUB IObjectContext_CreateInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContext_SetComplete_Proxy( 
    IObjectContext __RPC_FAR * This);


void __RPC_STUB IObjectContext_SetComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContext_SetAbort_Proxy( 
    IObjectContext __RPC_FAR * This);


void __RPC_STUB IObjectContext_SetAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContext_EnableCommit_Proxy( 
    IObjectContext __RPC_FAR * This);


void __RPC_STUB IObjectContext_EnableCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContext_DisableCommit_Proxy( 
    IObjectContext __RPC_FAR * This);


void __RPC_STUB IObjectContext_DisableCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IObjectContext_IsInTransaction_Proxy( 
    IObjectContext __RPC_FAR * This);


void __RPC_STUB IObjectContext_IsInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IObjectContext_IsSecurityEnabled_Proxy( 
    IObjectContext __RPC_FAR * This);


void __RPC_STUB IObjectContext_IsSecurityEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContext_IsCallerInRole_Proxy( 
    IObjectContext __RPC_FAR * This,
    /* [in] */ BSTR bstrRole,
    /* [retval][out] */ BOOL __RPC_FAR *pfIsInRole);


void __RPC_STUB IObjectContext_IsCallerInRole_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectContext_INTERFACE_DEFINED__ */


#ifndef __IObjectControl_INTERFACE_DEFINED__
#define __IObjectControl_INTERFACE_DEFINED__

/* interface IObjectControl */
/* [object][unique][uuid][local] */ 


EXTERN_C const IID IID_IObjectControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372aec-cae7-11cf-be81-00aa00a2fa25")
    IObjectControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Activate( void) = 0;
        
        virtual void STDMETHODCALLTYPE Deactivate( void) = 0;
        
        virtual BOOL STDMETHODCALLTYPE CanBePooled( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Activate )( 
            IObjectControl __RPC_FAR * This);
        
        void ( STDMETHODCALLTYPE __RPC_FAR *Deactivate )( 
            IObjectControl __RPC_FAR * This);
        
        BOOL ( STDMETHODCALLTYPE __RPC_FAR *CanBePooled )( 
            IObjectControl __RPC_FAR * This);
        
        END_INTERFACE
    } IObjectControlVtbl;

    interface IObjectControl
    {
        CONST_VTBL struct IObjectControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectControl_Activate(This)	\
    (This)->lpVtbl -> Activate(This)

#define IObjectControl_Deactivate(This)	\
    (This)->lpVtbl -> Deactivate(This)

#define IObjectControl_CanBePooled(This)	\
    (This)->lpVtbl -> CanBePooled(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectControl_Activate_Proxy( 
    IObjectControl __RPC_FAR * This);


void __RPC_STUB IObjectControl_Activate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IObjectControl_Deactivate_Proxy( 
    IObjectControl __RPC_FAR * This);


void __RPC_STUB IObjectControl_Deactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


BOOL STDMETHODCALLTYPE IObjectControl_CanBePooled_Proxy( 
    IObjectControl __RPC_FAR * This);


void __RPC_STUB IObjectControl_CanBePooled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectControl_INTERFACE_DEFINED__ */


#ifndef __IEnumNames_INTERFACE_DEFINED__
#define __IEnumNames_INTERFACE_DEFINED__

/* interface IEnumNames */
/* [object][unique][uuid][hidden][local] */ 


EXTERN_C const IID IID_IEnumNames;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372af2-cae7-11cf-be81-00aa00a2fa25")
    IEnumNames : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ unsigned long celt,
            /* [size_is][out] */ BSTR __RPC_FAR *rgname,
            /* [retval][out] */ unsigned long __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ unsigned long celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumNames __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumNamesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumNames __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumNames __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumNames __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumNames __RPC_FAR * This,
            /* [in] */ unsigned long celt,
            /* [size_is][out] */ BSTR __RPC_FAR *rgname,
            /* [retval][out] */ unsigned long __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumNames __RPC_FAR * This,
            /* [in] */ unsigned long celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumNames __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumNames __RPC_FAR * This,
            /* [retval][out] */ IEnumNames __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumNamesVtbl;

    interface IEnumNames
    {
        CONST_VTBL struct IEnumNamesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumNames_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumNames_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumNames_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumNames_Next(This,celt,rgname,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgname,pceltFetched)

#define IEnumNames_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumNames_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumNames_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumNames_Next_Proxy( 
    IEnumNames __RPC_FAR * This,
    /* [in] */ unsigned long celt,
    /* [size_is][out] */ BSTR __RPC_FAR *rgname,
    /* [retval][out] */ unsigned long __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumNames_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumNames_Skip_Proxy( 
    IEnumNames __RPC_FAR * This,
    /* [in] */ unsigned long celt);


void __RPC_STUB IEnumNames_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumNames_Reset_Proxy( 
    IEnumNames __RPC_FAR * This);


void __RPC_STUB IEnumNames_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumNames_Clone_Proxy( 
    IEnumNames __RPC_FAR * This,
    /* [retval][out] */ IEnumNames __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumNames_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumNames_INTERFACE_DEFINED__ */


#ifndef __ISecurityProperty_INTERFACE_DEFINED__
#define __ISecurityProperty_INTERFACE_DEFINED__

/* interface ISecurityProperty */
/* [object][unique][uuid][local] */ 


EXTERN_C const IID IID_ISecurityProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372aea-cae7-11cf-be81-00aa00a2fa25")
    ISecurityProperty : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDirectCreatorSID( 
            PSID __RPC_FAR *pSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalCreatorSID( 
            PSID __RPC_FAR *pSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDirectCallerSID( 
            PSID __RPC_FAR *pSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOriginalCallerSID( 
            PSID __RPC_FAR *pSID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseSID( 
            PSID pSID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISecurityPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISecurityProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISecurityProperty __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISecurityProperty __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectCreatorSID )( 
            ISecurityProperty __RPC_FAR * This,
            PSID __RPC_FAR *pSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOriginalCreatorSID )( 
            ISecurityProperty __RPC_FAR * This,
            PSID __RPC_FAR *pSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDirectCallerSID )( 
            ISecurityProperty __RPC_FAR * This,
            PSID __RPC_FAR *pSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOriginalCallerSID )( 
            ISecurityProperty __RPC_FAR * This,
            PSID __RPC_FAR *pSID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReleaseSID )( 
            ISecurityProperty __RPC_FAR * This,
            PSID pSID);
        
        END_INTERFACE
    } ISecurityPropertyVtbl;

    interface ISecurityProperty
    {
        CONST_VTBL struct ISecurityPropertyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISecurityProperty_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISecurityProperty_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISecurityProperty_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISecurityProperty_GetDirectCreatorSID(This,pSID)	\
    (This)->lpVtbl -> GetDirectCreatorSID(This,pSID)

#define ISecurityProperty_GetOriginalCreatorSID(This,pSID)	\
    (This)->lpVtbl -> GetOriginalCreatorSID(This,pSID)

#define ISecurityProperty_GetDirectCallerSID(This,pSID)	\
    (This)->lpVtbl -> GetDirectCallerSID(This,pSID)

#define ISecurityProperty_GetOriginalCallerSID(This,pSID)	\
    (This)->lpVtbl -> GetOriginalCallerSID(This,pSID)

#define ISecurityProperty_ReleaseSID(This,pSID)	\
    (This)->lpVtbl -> ReleaseSID(This,pSID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISecurityProperty_GetDirectCreatorSID_Proxy( 
    ISecurityProperty __RPC_FAR * This,
    PSID __RPC_FAR *pSID);


void __RPC_STUB ISecurityProperty_GetDirectCreatorSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISecurityProperty_GetOriginalCreatorSID_Proxy( 
    ISecurityProperty __RPC_FAR * This,
    PSID __RPC_FAR *pSID);


void __RPC_STUB ISecurityProperty_GetOriginalCreatorSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISecurityProperty_GetDirectCallerSID_Proxy( 
    ISecurityProperty __RPC_FAR * This,
    PSID __RPC_FAR *pSID);


void __RPC_STUB ISecurityProperty_GetDirectCallerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISecurityProperty_GetOriginalCallerSID_Proxy( 
    ISecurityProperty __RPC_FAR * This,
    PSID __RPC_FAR *pSID);


void __RPC_STUB ISecurityProperty_GetOriginalCallerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISecurityProperty_ReleaseSID_Proxy( 
    ISecurityProperty __RPC_FAR * This,
    PSID pSID);


void __RPC_STUB ISecurityProperty_ReleaseSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISecurityProperty_INTERFACE_DEFINED__ */


#ifndef __ObjectControl_INTERFACE_DEFINED__
#define __ObjectControl_INTERFACE_DEFINED__

/* interface ObjectControl */
/* [version][helpcontext][helpstring][oleautomation][uuid][local][object] */ 


EXTERN_C const IID IID_ObjectControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7DC41850-0C31-11d0-8B79-00AA00B8A790")
    ObjectControl : public IUnknown
    {
    public:
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE Activate( void) = 0;
        
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE Deactivate( void) = 0;
        
        virtual /* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE CanBePooled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbPoolable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ObjectControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ObjectControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ObjectControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ObjectControl __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Activate )( 
            ObjectControl __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Deactivate )( 
            ObjectControl __RPC_FAR * This);
        
        /* [helpstring][helpcontext] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CanBePooled )( 
            ObjectControl __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbPoolable);
        
        END_INTERFACE
    } ObjectControlVtbl;

    interface ObjectControl
    {
        CONST_VTBL struct ObjectControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ObjectControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ObjectControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ObjectControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ObjectControl_Activate(This)	\
    (This)->lpVtbl -> Activate(This)

#define ObjectControl_Deactivate(This)	\
    (This)->lpVtbl -> Deactivate(This)

#define ObjectControl_CanBePooled(This,pbPoolable)	\
    (This)->lpVtbl -> CanBePooled(This,pbPoolable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ObjectControl_Activate_Proxy( 
    ObjectControl __RPC_FAR * This);


void __RPC_STUB ObjectControl_Activate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ObjectControl_Deactivate_Proxy( 
    ObjectControl __RPC_FAR * This);


void __RPC_STUB ObjectControl_Deactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext] */ HRESULT STDMETHODCALLTYPE ObjectControl_CanBePooled_Proxy( 
    ObjectControl __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbPoolable);


void __RPC_STUB ObjectControl_CanBePooled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ObjectControl_INTERFACE_DEFINED__ */


#ifndef __ISharedProperty_INTERFACE_DEFINED__
#define __ISharedProperty_INTERFACE_DEFINED__

/* interface ISharedProperty */
/* [object][unique][helpcontext][helpstring][dual][uuid] */ 


EXTERN_C const IID IID_ISharedProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A005C01-A5DE-11CF-9E66-00AA00A3F464")
    ISharedProperty : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT val) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISharedPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISharedProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISharedProperty __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISharedProperty __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISharedProperty __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISharedProperty __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISharedProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISharedProperty __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ISharedProperty __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][helpcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ISharedProperty __RPC_FAR * This,
            /* [in] */ VARIANT val);
        
        END_INTERFACE
    } ISharedPropertyVtbl;

    interface ISharedProperty
    {
        CONST_VTBL struct ISharedPropertyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISharedProperty_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISharedProperty_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISharedProperty_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISharedProperty_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISharedProperty_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISharedProperty_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISharedProperty_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISharedProperty_get_Value(This,pVal)	\
    (This)->lpVtbl -> get_Value(This,pVal)

#define ISharedProperty_put_Value(This,val)	\
    (This)->lpVtbl -> put_Value(This,val)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ISharedProperty_get_Value_Proxy( 
    ISharedProperty __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB ISharedProperty_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propput] */ HRESULT STDMETHODCALLTYPE ISharedProperty_put_Value_Proxy( 
    ISharedProperty __RPC_FAR * This,
    /* [in] */ VARIANT val);


void __RPC_STUB ISharedProperty_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISharedProperty_INTERFACE_DEFINED__ */


#ifndef __ISharedPropertyGroup_INTERFACE_DEFINED__
#define __ISharedPropertyGroup_INTERFACE_DEFINED__

/* interface ISharedPropertyGroup */
/* [object][unique][helpcontext][helpstring][dual][uuid] */ 


EXTERN_C const IID IID_ISharedPropertyGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A005C07-A5DE-11CF-9E66-00AA00A3F464")
    ISharedPropertyGroup : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CreatePropertyByPosition( 
            /* [in] */ int Index,
            /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProp) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_PropertyByPosition( 
            /* [in] */ int Index,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProperty) = 0;
        
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CreateProperty( 
            /* [in] */ BSTR Name,
            /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProp) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Property( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProperty) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISharedPropertyGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISharedPropertyGroup __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISharedPropertyGroup __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePropertyByPosition )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ int Index,
            /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProp);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PropertyByPosition )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ int Index,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProperty);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateProperty )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProp);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Property )( 
            ISharedPropertyGroup __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProperty);
        
        END_INTERFACE
    } ISharedPropertyGroupVtbl;

    interface ISharedPropertyGroup
    {
        CONST_VTBL struct ISharedPropertyGroupVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISharedPropertyGroup_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISharedPropertyGroup_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISharedPropertyGroup_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISharedPropertyGroup_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISharedPropertyGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISharedPropertyGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISharedPropertyGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISharedPropertyGroup_CreatePropertyByPosition(This,Index,fExists,ppProp)	\
    (This)->lpVtbl -> CreatePropertyByPosition(This,Index,fExists,ppProp)

#define ISharedPropertyGroup_get_PropertyByPosition(This,Index,ppProperty)	\
    (This)->lpVtbl -> get_PropertyByPosition(This,Index,ppProperty)

#define ISharedPropertyGroup_CreateProperty(This,Name,fExists,ppProp)	\
    (This)->lpVtbl -> CreateProperty(This,Name,fExists,ppProp)

#define ISharedPropertyGroup_get_Property(This,Name,ppProperty)	\
    (This)->lpVtbl -> get_Property(This,Name,ppProperty)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroup_CreatePropertyByPosition_Proxy( 
    ISharedPropertyGroup __RPC_FAR * This,
    /* [in] */ int Index,
    /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
    /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProp);


void __RPC_STUB ISharedPropertyGroup_CreatePropertyByPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroup_get_PropertyByPosition_Proxy( 
    ISharedPropertyGroup __RPC_FAR * This,
    /* [in] */ int Index,
    /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProperty);


void __RPC_STUB ISharedPropertyGroup_get_PropertyByPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroup_CreateProperty_Proxy( 
    ISharedPropertyGroup __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
    /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProp);


void __RPC_STUB ISharedPropertyGroup_CreateProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroup_get_Property_Proxy( 
    ISharedPropertyGroup __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ ISharedProperty __RPC_FAR *__RPC_FAR *ppProperty);


void __RPC_STUB ISharedPropertyGroup_get_Property_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISharedPropertyGroup_INTERFACE_DEFINED__ */


#ifndef __ISharedPropertyGroupManager_INTERFACE_DEFINED__
#define __ISharedPropertyGroupManager_INTERFACE_DEFINED__

/* interface ISharedPropertyGroupManager */
/* [object][unique][helpcontext][helpstring][dual][uuid] */ 


EXTERN_C const IID IID_ISharedPropertyGroupManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A005C0D-A5DE-11CF-9E66-00AA00A3F464")
    ISharedPropertyGroupManager : public IDispatch
    {
    public:
        virtual /* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE CreatePropertyGroup( 
            /* [in] */ BSTR Name,
            /* [out][in] */ LONG __RPC_FAR *dwIsoMode,
            /* [out][in] */ LONG __RPC_FAR *dwRelMode,
            /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
            /* [retval][out] */ ISharedPropertyGroup __RPC_FAR *__RPC_FAR *ppGroup) = 0;
        
        virtual /* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Group( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ ISharedPropertyGroup __RPC_FAR *__RPC_FAR *ppGroup) = 0;
        
        virtual /* [helpstring][helpcontext][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISharedPropertyGroupManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISharedPropertyGroupManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISharedPropertyGroupManager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][helpcontext][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePropertyGroup )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [out][in] */ LONG __RPC_FAR *dwIsoMode,
            /* [out][in] */ LONG __RPC_FAR *dwRelMode,
            /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
            /* [retval][out] */ ISharedPropertyGroup __RPC_FAR *__RPC_FAR *ppGroup);
        
        /* [helpstring][helpcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Group )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ ISharedPropertyGroup __RPC_FAR *__RPC_FAR *ppGroup);
        
        /* [helpstring][helpcontext][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ISharedPropertyGroupManager __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);
        
        END_INTERFACE
    } ISharedPropertyGroupManagerVtbl;

    interface ISharedPropertyGroupManager
    {
        CONST_VTBL struct ISharedPropertyGroupManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISharedPropertyGroupManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISharedPropertyGroupManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISharedPropertyGroupManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISharedPropertyGroupManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISharedPropertyGroupManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISharedPropertyGroupManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISharedPropertyGroupManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISharedPropertyGroupManager_CreatePropertyGroup(This,Name,dwIsoMode,dwRelMode,fExists,ppGroup)	\
    (This)->lpVtbl -> CreatePropertyGroup(This,Name,dwIsoMode,dwRelMode,fExists,ppGroup)

#define ISharedPropertyGroupManager_get_Group(This,Name,ppGroup)	\
    (This)->lpVtbl -> get_Group(This,Name,ppGroup)

#define ISharedPropertyGroupManager_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][helpcontext][id] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroupManager_CreatePropertyGroup_Proxy( 
    ISharedPropertyGroupManager __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [out][in] */ LONG __RPC_FAR *dwIsoMode,
    /* [out][in] */ LONG __RPC_FAR *dwRelMode,
    /* [out] */ VARIANT_BOOL __RPC_FAR *fExists,
    /* [retval][out] */ ISharedPropertyGroup __RPC_FAR *__RPC_FAR *ppGroup);


void __RPC_STUB ISharedPropertyGroupManager_CreatePropertyGroup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][propget] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroupManager_get_Group_Proxy( 
    ISharedPropertyGroupManager __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ ISharedPropertyGroup __RPC_FAR *__RPC_FAR *ppGroup);


void __RPC_STUB ISharedPropertyGroupManager_get_Group_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][helpcontext][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ISharedPropertyGroupManager_get__NewEnum_Proxy( 
    ISharedPropertyGroupManager __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB ISharedPropertyGroupManager_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISharedPropertyGroupManager_INTERFACE_DEFINED__ */


#ifndef __IObjectConstruct_INTERFACE_DEFINED__
#define __IObjectConstruct_INTERFACE_DEFINED__

/* interface IObjectConstruct */
/* [uuid][helpstring][unique][object][local] */ 


EXTERN_C const IID IID_IObjectConstruct;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("41C4F8B3-7439-11D2-98CB-00C04F8EE1C4")
    IObjectConstruct : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Construct( 
            /* [in] */ IDispatch __RPC_FAR *pCtorObj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectConstructVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectConstruct __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectConstruct __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectConstruct __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Construct )( 
            IObjectConstruct __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pCtorObj);
        
        END_INTERFACE
    } IObjectConstructVtbl;

    interface IObjectConstruct
    {
        CONST_VTBL struct IObjectConstructVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectConstruct_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectConstruct_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectConstruct_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectConstruct_Construct(This,pCtorObj)	\
    (This)->lpVtbl -> Construct(This,pCtorObj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectConstruct_Construct_Proxy( 
    IObjectConstruct __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pCtorObj);


void __RPC_STUB IObjectConstruct_Construct_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectConstruct_INTERFACE_DEFINED__ */


#ifndef __IObjectConstructString_INTERFACE_DEFINED__
#define __IObjectConstructString_INTERFACE_DEFINED__

/* interface IObjectConstructString */
/* [uuid][helpstring][dual][unique][object][local] */ 


EXTERN_C const IID IID_IObjectConstructString;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("41C4F8B2-7439-11D2-98CB-00C04F8EE1C4")
    IObjectConstructString : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConstructString( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectConstructStringVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectConstructString __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectConstructString __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectConstructString __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IObjectConstructString __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IObjectConstructString __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IObjectConstructString __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IObjectConstructString __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConstructString )( 
            IObjectConstructString __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        END_INTERFACE
    } IObjectConstructStringVtbl;

    interface IObjectConstructString
    {
        CONST_VTBL struct IObjectConstructStringVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectConstructString_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectConstructString_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectConstructString_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectConstructString_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IObjectConstructString_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IObjectConstructString_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IObjectConstructString_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IObjectConstructString_get_ConstructString(This,pVal)	\
    (This)->lpVtbl -> get_ConstructString(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IObjectConstructString_get_ConstructString_Proxy( 
    IObjectConstructString __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IObjectConstructString_get_ConstructString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectConstructString_INTERFACE_DEFINED__ */


#ifndef __IObjectContextActivity_INTERFACE_DEFINED__
#define __IObjectContextActivity_INTERFACE_DEFINED__

/* interface IObjectContextActivity */
/* [object][unique][uuid][local] */ 


EXTERN_C const IID IID_IObjectContextActivity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372afc-cae7-11cf-be81-00aa00a2fa25")
    IObjectContextActivity : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetActivityId( 
            /* [out] */ GUID __RPC_FAR *pGUID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectContextActivityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectContextActivity __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectContextActivity __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectContextActivity __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetActivityId )( 
            IObjectContextActivity __RPC_FAR * This,
            /* [out] */ GUID __RPC_FAR *pGUID);
        
        END_INTERFACE
    } IObjectContextActivityVtbl;

    interface IObjectContextActivity
    {
        CONST_VTBL struct IObjectContextActivityVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectContextActivity_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectContextActivity_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectContextActivity_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectContextActivity_GetActivityId(This,pGUID)	\
    (This)->lpVtbl -> GetActivityId(This,pGUID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectContextActivity_GetActivityId_Proxy( 
    IObjectContextActivity __RPC_FAR * This,
    /* [out] */ GUID __RPC_FAR *pGUID);


void __RPC_STUB IObjectContextActivity_GetActivityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectContextActivity_INTERFACE_DEFINED__ */


#ifndef __IObjectContextInfo_INTERFACE_DEFINED__
#define __IObjectContextInfo_INTERFACE_DEFINED__

/* interface IObjectContextInfo */
/* [uuid][unique][object][local] */ 


EXTERN_C const IID IID_IObjectContextInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("75B52DDB-E8ED-11d1-93AD-00AA00BA3258")
    IObjectContextInfo : public IUnknown
    {
    public:
        virtual BOOL STDMETHODCALLTYPE IsInTransaction( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTransaction( 
            IUnknown __RPC_FAR *__RPC_FAR *pptrans) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTransactionId( 
            /* [out] */ GUID __RPC_FAR *pGuid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetActivityId( 
            /* [out] */ GUID __RPC_FAR *pGUID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContextId( 
            /* [out] */ GUID __RPC_FAR *pGuid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectContextInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectContextInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectContextInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectContextInfo __RPC_FAR * This);
        
        BOOL ( STDMETHODCALLTYPE __RPC_FAR *IsInTransaction )( 
            IObjectContextInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTransaction )( 
            IObjectContextInfo __RPC_FAR * This,
            IUnknown __RPC_FAR *__RPC_FAR *pptrans);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTransactionId )( 
            IObjectContextInfo __RPC_FAR * This,
            /* [out] */ GUID __RPC_FAR *pGuid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetActivityId )( 
            IObjectContextInfo __RPC_FAR * This,
            /* [out] */ GUID __RPC_FAR *pGUID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContextId )( 
            IObjectContextInfo __RPC_FAR * This,
            /* [out] */ GUID __RPC_FAR *pGuid);
        
        END_INTERFACE
    } IObjectContextInfoVtbl;

    interface IObjectContextInfo
    {
        CONST_VTBL struct IObjectContextInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectContextInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectContextInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectContextInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectContextInfo_IsInTransaction(This)	\
    (This)->lpVtbl -> IsInTransaction(This)

#define IObjectContextInfo_GetTransaction(This,pptrans)	\
    (This)->lpVtbl -> GetTransaction(This,pptrans)

#define IObjectContextInfo_GetTransactionId(This,pGuid)	\
    (This)->lpVtbl -> GetTransactionId(This,pGuid)

#define IObjectContextInfo_GetActivityId(This,pGUID)	\
    (This)->lpVtbl -> GetActivityId(This,pGUID)

#define IObjectContextInfo_GetContextId(This,pGuid)	\
    (This)->lpVtbl -> GetContextId(This,pGuid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



BOOL STDMETHODCALLTYPE IObjectContextInfo_IsInTransaction_Proxy( 
    IObjectContextInfo __RPC_FAR * This);


void __RPC_STUB IObjectContextInfo_IsInTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContextInfo_GetTransaction_Proxy( 
    IObjectContextInfo __RPC_FAR * This,
    IUnknown __RPC_FAR *__RPC_FAR *pptrans);


void __RPC_STUB IObjectContextInfo_GetTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContextInfo_GetTransactionId_Proxy( 
    IObjectContextInfo __RPC_FAR * This,
    /* [out] */ GUID __RPC_FAR *pGuid);


void __RPC_STUB IObjectContextInfo_GetTransactionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContextInfo_GetActivityId_Proxy( 
    IObjectContextInfo __RPC_FAR * This,
    /* [out] */ GUID __RPC_FAR *pGUID);


void __RPC_STUB IObjectContextInfo_GetActivityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectContextInfo_GetContextId_Proxy( 
    IObjectContextInfo __RPC_FAR * This,
    /* [out] */ GUID __RPC_FAR *pGuid);


void __RPC_STUB IObjectContextInfo_GetContextId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectContextInfo_INTERFACE_DEFINED__ */


#ifndef __IObjectContextTip_INTERFACE_DEFINED__
#define __IObjectContextTip_INTERFACE_DEFINED__

/* interface IObjectContextTip */
/* [object][uuid][unique][local] */ 


EXTERN_C const IID IID_IObjectContextTip;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("92FD41CA-BAD9-11d2-9A2D-00C04F797BC9")
    IObjectContextTip : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetTipUrl( 
            /* [retval][out] */ BSTR __RPC_FAR *pTipUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectContextTipVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjectContextTip __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjectContextTip __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjectContextTip __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTipUrl )( 
            IObjectContextTip __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pTipUrl);
        
        END_INTERFACE
    } IObjectContextTipVtbl;

    interface IObjectContextTip
    {
        CONST_VTBL struct IObjectContextTipVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectContextTip_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectContextTip_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectContextTip_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectContextTip_GetTipUrl(This,pTipUrl)	\
    (This)->lpVtbl -> GetTipUrl(This,pTipUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectContextTip_GetTipUrl_Proxy( 
    IObjectContextTip __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pTipUrl);


void __RPC_STUB IObjectContextTip_GetTipUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectContextTip_INTERFACE_DEFINED__ */


#ifndef __IPlaybackControl_INTERFACE_DEFINED__
#define __IPlaybackControl_INTERFACE_DEFINED__

/* interface IPlaybackControl */
/* [object][unique][uuid] */ 


EXTERN_C const IID IID_IPlaybackControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372afd-cae7-11cf-be81-00aa00a2fa25")
    IPlaybackControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FinalClientRetry( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FinalServerRetry( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPlaybackControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPlaybackControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPlaybackControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPlaybackControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FinalClientRetry )( 
            IPlaybackControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FinalServerRetry )( 
            IPlaybackControl __RPC_FAR * This);
        
        END_INTERFACE
    } IPlaybackControlVtbl;

    interface IPlaybackControl
    {
        CONST_VTBL struct IPlaybackControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPlaybackControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPlaybackControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPlaybackControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPlaybackControl_FinalClientRetry(This)	\
    (This)->lpVtbl -> FinalClientRetry(This)

#define IPlaybackControl_FinalServerRetry(This)	\
    (This)->lpVtbl -> FinalServerRetry(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPlaybackControl_FinalClientRetry_Proxy( 
    IPlaybackControl __RPC_FAR * This);


void __RPC_STUB IPlaybackControl_FinalClientRetry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPlaybackControl_FinalServerRetry_Proxy( 
    IPlaybackControl __RPC_FAR * This);


void __RPC_STUB IPlaybackControl_FinalServerRetry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPlaybackControl_INTERFACE_DEFINED__ */


#ifndef __IGetContextProperties_INTERFACE_DEFINED__
#define __IGetContextProperties_INTERFACE_DEFINED__

/* interface IGetContextProperties */
/* [object][unique][uuid][local] */ 


EXTERN_C const IID IID_IGetContextProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51372af4-cae7-11cf-be81-00aa00a2fa25")
    IGetContextProperties : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Count( 
            /* [retval][out] */ long __RPC_FAR *plCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pProperty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumNames( 
            /* [retval][out] */ IEnumNames __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGetContextPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGetContextProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGetContextProperties __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGetContextProperties __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Count )( 
            IGetContextProperties __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plCount);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetProperty )( 
            IGetContextProperties __RPC_FAR * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT __RPC_FAR *pProperty);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumNames )( 
            IGetContextProperties __RPC_FAR * This,
            /* [retval][out] */ IEnumNames __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IGetContextPropertiesVtbl;

    interface IGetContextProperties
    {
        CONST_VTBL struct IGetContextPropertiesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGetContextProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGetContextProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGetContextProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGetContextProperties_Count(This,plCount)	\
    (This)->lpVtbl -> Count(This,plCount)

#define IGetContextProperties_GetProperty(This,name,pProperty)	\
    (This)->lpVtbl -> GetProperty(This,name,pProperty)

#define IGetContextProperties_EnumNames(This,ppenum)	\
    (This)->lpVtbl -> EnumNames(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IGetContextProperties_Count_Proxy( 
    IGetContextProperties __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plCount);


void __RPC_STUB IGetContextProperties_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IGetContextProperties_GetProperty_Proxy( 
    IGetContextProperties __RPC_FAR * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT __RPC_FAR *pProperty);


void __RPC_STUB IGetContextProperties_GetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IGetContextProperties_EnumNames_Proxy( 
    IGetContextProperties __RPC_FAR * This,
    /* [retval][out] */ IEnumNames __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IGetContextProperties_EnumNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGetContextProperties_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0324 */
/* [local] */ 

typedef 
enum tagTransactionVote
    {	TxCommit	= 0,
	TxAbort	= TxCommit + 1
    }	TransactionVote;



extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0324_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0324_v0_0_s_ifspec;

#ifndef __IContextState_INTERFACE_DEFINED__
#define __IContextState_INTERFACE_DEFINED__

/* interface IContextState */
/* [uuid][unique][object][local] */ 


EXTERN_C const IID IID_IContextState;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3C05E54B-A42A-11d2-AFC4-00C04F8EE1C4")
    IContextState : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetDeactivateOnReturn( 
            VARIANT_BOOL bDeactivate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeactivateOnReturn( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pbDeactivate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMyTransactionVote( 
            TransactionVote txVote) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMyTransactionVote( 
            /* [out] */ TransactionVote __RPC_FAR *ptxVote) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContextStateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IContextState __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IContextState __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IContextState __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDeactivateOnReturn )( 
            IContextState __RPC_FAR * This,
            VARIANT_BOOL bDeactivate);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDeactivateOnReturn )( 
            IContextState __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pbDeactivate);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMyTransactionVote )( 
            IContextState __RPC_FAR * This,
            TransactionVote txVote);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMyTransactionVote )( 
            IContextState __RPC_FAR * This,
            /* [out] */ TransactionVote __RPC_FAR *ptxVote);
        
        END_INTERFACE
    } IContextStateVtbl;

    interface IContextState
    {
        CONST_VTBL struct IContextStateVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContextState_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IContextState_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IContextState_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IContextState_SetDeactivateOnReturn(This,bDeactivate)	\
    (This)->lpVtbl -> SetDeactivateOnReturn(This,bDeactivate)

#define IContextState_GetDeactivateOnReturn(This,pbDeactivate)	\
    (This)->lpVtbl -> GetDeactivateOnReturn(This,pbDeactivate)

#define IContextState_SetMyTransactionVote(This,txVote)	\
    (This)->lpVtbl -> SetMyTransactionVote(This,txVote)

#define IContextState_GetMyTransactionVote(This,ptxVote)	\
    (This)->lpVtbl -> GetMyTransactionVote(This,ptxVote)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IContextState_SetDeactivateOnReturn_Proxy( 
    IContextState __RPC_FAR * This,
    VARIANT_BOOL bDeactivate);


void __RPC_STUB IContextState_SetDeactivateOnReturn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IContextState_GetDeactivateOnReturn_Proxy( 
    IContextState __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pbDeactivate);


void __RPC_STUB IContextState_GetDeactivateOnReturn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IContextState_SetMyTransactionVote_Proxy( 
    IContextState __RPC_FAR * This,
    TransactionVote txVote);


void __RPC_STUB IContextState_SetMyTransactionVote_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IContextState_GetMyTransactionVote_Proxy( 
    IContextState __RPC_FAR * This,
    /* [out] */ TransactionVote __RPC_FAR *ptxVote);


void __RPC_STUB IContextState_GetMyTransactionVote_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IContextState_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0325 */
/* [local] */ 

#if (_WIN32_WINNT >= 0x0500)
#define GetObjectContext(ppIOC) (CoGetObjectContext(IID_IObjectContext, (void **) (ppIOC)) == S_OK ? S_OK : CONTEXT_E_NOCONTEXT)
#else
extern HRESULT __cdecl GetObjectContext (IObjectContext** ppInstanceContext);
#endif
extern void* __cdecl SafeRef(REFIID rid, IUnknown* pUnk);


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0325_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0325_v0_0_s_ifspec;

#ifndef __ICrmLogControl_INTERFACE_DEFINED__
#define __ICrmLogControl_INTERFACE_DEFINED__

/* interface ICrmLogControl */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICrmLogControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A0E174B3-D26E-11d2-8F84-00805FC7BCD9")
    ICrmLogControl : public IUnknown
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TransactionUOW( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterCompensator( 
            /* [in] */ LPCWSTR lpcwstrProgIdCompensator,
            /* [in] */ LPCWSTR lpcwstrDescription,
            /* [in] */ LONG lCrmRegFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteLogRecordVariants( 
            /* [in] */ VARIANT __RPC_FAR *pLogRecord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ForceLog( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ForgetLogRecord( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ForceTransactionToAbort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteLogRecord( 
            /* [size_is][in] */ BLOB __RPC_FAR rgBlob[  ],
            /* [in] */ ULONG cBlob) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmLogControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmLogControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmLogControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmLogControl __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransactionUOW )( 
            ICrmLogControl __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterCompensator )( 
            ICrmLogControl __RPC_FAR * This,
            /* [in] */ LPCWSTR lpcwstrProgIdCompensator,
            /* [in] */ LPCWSTR lpcwstrDescription,
            /* [in] */ LONG lCrmRegFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteLogRecordVariants )( 
            ICrmLogControl __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pLogRecord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ForceLog )( 
            ICrmLogControl __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ForgetLogRecord )( 
            ICrmLogControl __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ForceTransactionToAbort )( 
            ICrmLogControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteLogRecord )( 
            ICrmLogControl __RPC_FAR * This,
            /* [size_is][in] */ BLOB __RPC_FAR rgBlob[  ],
            /* [in] */ ULONG cBlob);
        
        END_INTERFACE
    } ICrmLogControlVtbl;

    interface ICrmLogControl
    {
        CONST_VTBL struct ICrmLogControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmLogControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmLogControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmLogControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmLogControl_get_TransactionUOW(This,pVal)	\
    (This)->lpVtbl -> get_TransactionUOW(This,pVal)

#define ICrmLogControl_RegisterCompensator(This,lpcwstrProgIdCompensator,lpcwstrDescription,lCrmRegFlags)	\
    (This)->lpVtbl -> RegisterCompensator(This,lpcwstrProgIdCompensator,lpcwstrDescription,lCrmRegFlags)

#define ICrmLogControl_WriteLogRecordVariants(This,pLogRecord)	\
    (This)->lpVtbl -> WriteLogRecordVariants(This,pLogRecord)

#define ICrmLogControl_ForceLog(This)	\
    (This)->lpVtbl -> ForceLog(This)

#define ICrmLogControl_ForgetLogRecord(This)	\
    (This)->lpVtbl -> ForgetLogRecord(This)

#define ICrmLogControl_ForceTransactionToAbort(This)	\
    (This)->lpVtbl -> ForceTransactionToAbort(This)

#define ICrmLogControl_WriteLogRecord(This,rgBlob,cBlob)	\
    (This)->lpVtbl -> WriteLogRecord(This,rgBlob,cBlob)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICrmLogControl_get_TransactionUOW_Proxy( 
    ICrmLogControl __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB ICrmLogControl_get_TransactionUOW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmLogControl_RegisterCompensator_Proxy( 
    ICrmLogControl __RPC_FAR * This,
    /* [in] */ LPCWSTR lpcwstrProgIdCompensator,
    /* [in] */ LPCWSTR lpcwstrDescription,
    /* [in] */ LONG lCrmRegFlags);


void __RPC_STUB ICrmLogControl_RegisterCompensator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmLogControl_WriteLogRecordVariants_Proxy( 
    ICrmLogControl __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pLogRecord);


void __RPC_STUB ICrmLogControl_WriteLogRecordVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmLogControl_ForceLog_Proxy( 
    ICrmLogControl __RPC_FAR * This);


void __RPC_STUB ICrmLogControl_ForceLog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmLogControl_ForgetLogRecord_Proxy( 
    ICrmLogControl __RPC_FAR * This);


void __RPC_STUB ICrmLogControl_ForgetLogRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmLogControl_ForceTransactionToAbort_Proxy( 
    ICrmLogControl __RPC_FAR * This);


void __RPC_STUB ICrmLogControl_ForceTransactionToAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmLogControl_WriteLogRecord_Proxy( 
    ICrmLogControl __RPC_FAR * This,
    /* [size_is][in] */ BLOB __RPC_FAR rgBlob[  ],
    /* [in] */ ULONG cBlob);


void __RPC_STUB ICrmLogControl_WriteLogRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmLogControl_INTERFACE_DEFINED__ */


#ifndef __ICrmCompensatorVariants_INTERFACE_DEFINED__
#define __ICrmCompensatorVariants_INTERFACE_DEFINED__

/* interface ICrmCompensatorVariants */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICrmCompensatorVariants;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F0BAF8E4-7804-11d1-82E9-00A0C91EEDE9")
    ICrmCompensatorVariants : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLogControlVariants( 
            /* [in] */ ICrmLogControl __RPC_FAR *pLogControl) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginPrepareVariants( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PrepareRecordVariants( 
            /* [in] */ VARIANT __RPC_FAR *pLogRecord,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndPrepareVariants( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbOkToPrepare) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginCommitVariants( 
            /* [in] */ VARIANT_BOOL bRecovery) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CommitRecordVariants( 
            /* [in] */ VARIANT __RPC_FAR *pLogRecord,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndCommitVariants( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginAbortVariants( 
            /* [in] */ VARIANT_BOOL bRecovery) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AbortRecordVariants( 
            /* [in] */ VARIANT __RPC_FAR *pLogRecord,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndAbortVariants( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmCompensatorVariantsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmCompensatorVariants __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmCompensatorVariants __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLogControlVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ ICrmLogControl __RPC_FAR *pLogControl);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginPrepareVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareRecordVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pLogRecord,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndPrepareVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbOkToPrepare);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginCommitVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bRecovery);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitRecordVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pLogRecord,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndCommitVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginAbortVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL bRecovery);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbortRecordVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pLogRecord,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndAbortVariants )( 
            ICrmCompensatorVariants __RPC_FAR * This);
        
        END_INTERFACE
    } ICrmCompensatorVariantsVtbl;

    interface ICrmCompensatorVariants
    {
        CONST_VTBL struct ICrmCompensatorVariantsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmCompensatorVariants_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmCompensatorVariants_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmCompensatorVariants_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmCompensatorVariants_SetLogControlVariants(This,pLogControl)	\
    (This)->lpVtbl -> SetLogControlVariants(This,pLogControl)

#define ICrmCompensatorVariants_BeginPrepareVariants(This)	\
    (This)->lpVtbl -> BeginPrepareVariants(This)

#define ICrmCompensatorVariants_PrepareRecordVariants(This,pLogRecord,pbForget)	\
    (This)->lpVtbl -> PrepareRecordVariants(This,pLogRecord,pbForget)

#define ICrmCompensatorVariants_EndPrepareVariants(This,pbOkToPrepare)	\
    (This)->lpVtbl -> EndPrepareVariants(This,pbOkToPrepare)

#define ICrmCompensatorVariants_BeginCommitVariants(This,bRecovery)	\
    (This)->lpVtbl -> BeginCommitVariants(This,bRecovery)

#define ICrmCompensatorVariants_CommitRecordVariants(This,pLogRecord,pbForget)	\
    (This)->lpVtbl -> CommitRecordVariants(This,pLogRecord,pbForget)

#define ICrmCompensatorVariants_EndCommitVariants(This)	\
    (This)->lpVtbl -> EndCommitVariants(This)

#define ICrmCompensatorVariants_BeginAbortVariants(This,bRecovery)	\
    (This)->lpVtbl -> BeginAbortVariants(This,bRecovery)

#define ICrmCompensatorVariants_AbortRecordVariants(This,pLogRecord,pbForget)	\
    (This)->lpVtbl -> AbortRecordVariants(This,pLogRecord,pbForget)

#define ICrmCompensatorVariants_EndAbortVariants(This)	\
    (This)->lpVtbl -> EndAbortVariants(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_SetLogControlVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [in] */ ICrmLogControl __RPC_FAR *pLogControl);


void __RPC_STUB ICrmCompensatorVariants_SetLogControlVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_BeginPrepareVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This);


void __RPC_STUB ICrmCompensatorVariants_BeginPrepareVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_PrepareRecordVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pLogRecord,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget);


void __RPC_STUB ICrmCompensatorVariants_PrepareRecordVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_EndPrepareVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbOkToPrepare);


void __RPC_STUB ICrmCompensatorVariants_EndPrepareVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_BeginCommitVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bRecovery);


void __RPC_STUB ICrmCompensatorVariants_BeginCommitVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_CommitRecordVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pLogRecord,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget);


void __RPC_STUB ICrmCompensatorVariants_CommitRecordVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_EndCommitVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This);


void __RPC_STUB ICrmCompensatorVariants_EndCommitVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_BeginAbortVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL bRecovery);


void __RPC_STUB ICrmCompensatorVariants_BeginAbortVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_AbortRecordVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pLogRecord,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbForget);


void __RPC_STUB ICrmCompensatorVariants_AbortRecordVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmCompensatorVariants_EndAbortVariants_Proxy( 
    ICrmCompensatorVariants __RPC_FAR * This);


void __RPC_STUB ICrmCompensatorVariants_EndAbortVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmCompensatorVariants_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0327 */
/* [local] */ 

#ifndef _tagCrmLogRecordRead_
#define _tagCrmLogRecordRead_
typedef struct tagCrmLogRecordRead
    {
    DWORD dwCrmFlags;
    DWORD dwSequenceNumber;
    BLOB blobUserData;
    }	CrmLogRecordRead;

#endif _tagCrmLogRecordRead_


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0327_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0327_v0_0_s_ifspec;

#ifndef __ICrmCompensator_INTERFACE_DEFINED__
#define __ICrmCompensator_INTERFACE_DEFINED__

/* interface ICrmCompensator */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICrmCompensator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BBC01830-8D3B-11d1-82EC-00A0C91EEDE9")
    ICrmCompensator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetLogControl( 
            /* [in] */ ICrmLogControl __RPC_FAR *pLogControl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeginPrepare( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PrepareRecord( 
            /* [in] */ CrmLogRecordRead crmLogRec,
            /* [retval][out] */ BOOL __RPC_FAR *pfForget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndPrepare( 
            /* [retval][out] */ BOOL __RPC_FAR *pfOkToPrepare) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeginCommit( 
            /* [in] */ BOOL fRecovery) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitRecord( 
            /* [in] */ CrmLogRecordRead crmLogRec,
            /* [retval][out] */ BOOL __RPC_FAR *pfForget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndCommit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeginAbort( 
            /* [in] */ BOOL fRecovery) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AbortRecord( 
            /* [in] */ CrmLogRecordRead crmLogRec,
            /* [retval][out] */ BOOL __RPC_FAR *pfForget) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndAbort( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmCompensatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmCompensator __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmCompensator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetLogControl )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ ICrmLogControl __RPC_FAR *pLogControl);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginPrepare )( 
            ICrmCompensator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareRecord )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ CrmLogRecordRead crmLogRec,
            /* [retval][out] */ BOOL __RPC_FAR *pfForget);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndPrepare )( 
            ICrmCompensator __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfOkToPrepare);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginCommit )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ BOOL fRecovery);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitRecord )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ CrmLogRecordRead crmLogRec,
            /* [retval][out] */ BOOL __RPC_FAR *pfForget);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndCommit )( 
            ICrmCompensator __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BeginAbort )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ BOOL fRecovery);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbortRecord )( 
            ICrmCompensator __RPC_FAR * This,
            /* [in] */ CrmLogRecordRead crmLogRec,
            /* [retval][out] */ BOOL __RPC_FAR *pfForget);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndAbort )( 
            ICrmCompensator __RPC_FAR * This);
        
        END_INTERFACE
    } ICrmCompensatorVtbl;

    interface ICrmCompensator
    {
        CONST_VTBL struct ICrmCompensatorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmCompensator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmCompensator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmCompensator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmCompensator_SetLogControl(This,pLogControl)	\
    (This)->lpVtbl -> SetLogControl(This,pLogControl)

#define ICrmCompensator_BeginPrepare(This)	\
    (This)->lpVtbl -> BeginPrepare(This)

#define ICrmCompensator_PrepareRecord(This,crmLogRec,pfForget)	\
    (This)->lpVtbl -> PrepareRecord(This,crmLogRec,pfForget)

#define ICrmCompensator_EndPrepare(This,pfOkToPrepare)	\
    (This)->lpVtbl -> EndPrepare(This,pfOkToPrepare)

#define ICrmCompensator_BeginCommit(This,fRecovery)	\
    (This)->lpVtbl -> BeginCommit(This,fRecovery)

#define ICrmCompensator_CommitRecord(This,crmLogRec,pfForget)	\
    (This)->lpVtbl -> CommitRecord(This,crmLogRec,pfForget)

#define ICrmCompensator_EndCommit(This)	\
    (This)->lpVtbl -> EndCommit(This)

#define ICrmCompensator_BeginAbort(This,fRecovery)	\
    (This)->lpVtbl -> BeginAbort(This,fRecovery)

#define ICrmCompensator_AbortRecord(This,crmLogRec,pfForget)	\
    (This)->lpVtbl -> AbortRecord(This,crmLogRec,pfForget)

#define ICrmCompensator_EndAbort(This)	\
    (This)->lpVtbl -> EndAbort(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICrmCompensator_SetLogControl_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [in] */ ICrmLogControl __RPC_FAR *pLogControl);


void __RPC_STUB ICrmCompensator_SetLogControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_BeginPrepare_Proxy( 
    ICrmCompensator __RPC_FAR * This);


void __RPC_STUB ICrmCompensator_BeginPrepare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_PrepareRecord_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [in] */ CrmLogRecordRead crmLogRec,
    /* [retval][out] */ BOOL __RPC_FAR *pfForget);


void __RPC_STUB ICrmCompensator_PrepareRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_EndPrepare_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfOkToPrepare);


void __RPC_STUB ICrmCompensator_EndPrepare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_BeginCommit_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [in] */ BOOL fRecovery);


void __RPC_STUB ICrmCompensator_BeginCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_CommitRecord_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [in] */ CrmLogRecordRead crmLogRec,
    /* [retval][out] */ BOOL __RPC_FAR *pfForget);


void __RPC_STUB ICrmCompensator_CommitRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_EndCommit_Proxy( 
    ICrmCompensator __RPC_FAR * This);


void __RPC_STUB ICrmCompensator_EndCommit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_BeginAbort_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [in] */ BOOL fRecovery);


void __RPC_STUB ICrmCompensator_BeginAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_AbortRecord_Proxy( 
    ICrmCompensator __RPC_FAR * This,
    /* [in] */ CrmLogRecordRead crmLogRec,
    /* [retval][out] */ BOOL __RPC_FAR *pfForget);


void __RPC_STUB ICrmCompensator_AbortRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICrmCompensator_EndAbort_Proxy( 
    ICrmCompensator __RPC_FAR * This);


void __RPC_STUB ICrmCompensator_EndAbort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmCompensator_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_autosvcs_0328 */
/* [local] */ 

#ifndef _tagCrmTransactionState_
#define _tagCrmTransactionState_
typedef 
enum tagCrmTransactionState
    {	TxState_Active	= 0,
	TxState_Committed	= TxState_Active + 1,
	TxState_Aborted	= TxState_Committed + 1,
	TxState_Indoubt	= TxState_Aborted + 1
    }	CrmTransactionState;

#endif _tagCrmTransactionState_


extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0328_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_autosvcs_0328_v0_0_s_ifspec;

#ifndef __ICrmMonitorLogRecords_INTERFACE_DEFINED__
#define __ICrmMonitorLogRecords_INTERFACE_DEFINED__

/* interface ICrmMonitorLogRecords */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICrmMonitorLogRecords;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70C8E441-C7ED-11d1-82FB-00A0C91EEDE9")
    ICrmMonitorLogRecords : public IUnknown
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TransactionState( 
            /* [retval][out] */ CrmTransactionState __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StructuredRecords( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogRecord( 
            /* [in] */ DWORD dwIndex,
            /* [out][in] */ CrmLogRecordRead __RPC_FAR *pCrmLogRec) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogRecordVariants( 
            /* [in] */ VARIANT IndexNumber,
            /* [retval][out] */ LPVARIANT pLogRecord) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmMonitorLogRecordsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmMonitorLogRecords __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmMonitorLogRecords __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmMonitorLogRecords __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ICrmMonitorLogRecords __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TransactionState )( 
            ICrmMonitorLogRecords __RPC_FAR * This,
            /* [retval][out] */ CrmTransactionState __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_StructuredRecords )( 
            ICrmMonitorLogRecords __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLogRecord )( 
            ICrmMonitorLogRecords __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [out][in] */ CrmLogRecordRead __RPC_FAR *pCrmLogRec);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLogRecordVariants )( 
            ICrmMonitorLogRecords __RPC_FAR * This,
            /* [in] */ VARIANT IndexNumber,
            /* [retval][out] */ LPVARIANT pLogRecord);
        
        END_INTERFACE
    } ICrmMonitorLogRecordsVtbl;

    interface ICrmMonitorLogRecords
    {
        CONST_VTBL struct ICrmMonitorLogRecordsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmMonitorLogRecords_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmMonitorLogRecords_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmMonitorLogRecords_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmMonitorLogRecords_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define ICrmMonitorLogRecords_get_TransactionState(This,pVal)	\
    (This)->lpVtbl -> get_TransactionState(This,pVal)

#define ICrmMonitorLogRecords_get_StructuredRecords(This,pVal)	\
    (This)->lpVtbl -> get_StructuredRecords(This,pVal)

#define ICrmMonitorLogRecords_GetLogRecord(This,dwIndex,pCrmLogRec)	\
    (This)->lpVtbl -> GetLogRecord(This,dwIndex,pCrmLogRec)

#define ICrmMonitorLogRecords_GetLogRecordVariants(This,IndexNumber,pLogRecord)	\
    (This)->lpVtbl -> GetLogRecordVariants(This,IndexNumber,pLogRecord)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICrmMonitorLogRecords_get_Count_Proxy( 
    ICrmMonitorLogRecords __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB ICrmMonitorLogRecords_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICrmMonitorLogRecords_get_TransactionState_Proxy( 
    ICrmMonitorLogRecords __RPC_FAR * This,
    /* [retval][out] */ CrmTransactionState __RPC_FAR *pVal);


void __RPC_STUB ICrmMonitorLogRecords_get_TransactionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICrmMonitorLogRecords_get_StructuredRecords_Proxy( 
    ICrmMonitorLogRecords __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB ICrmMonitorLogRecords_get_StructuredRecords_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorLogRecords_GetLogRecord_Proxy( 
    ICrmMonitorLogRecords __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [out][in] */ CrmLogRecordRead __RPC_FAR *pCrmLogRec);


void __RPC_STUB ICrmMonitorLogRecords_GetLogRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorLogRecords_GetLogRecordVariants_Proxy( 
    ICrmMonitorLogRecords __RPC_FAR * This,
    /* [in] */ VARIANT IndexNumber,
    /* [retval][out] */ LPVARIANT pLogRecord);


void __RPC_STUB ICrmMonitorLogRecords_GetLogRecordVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmMonitorLogRecords_INTERFACE_DEFINED__ */


#ifndef __ICrmMonitorClerks_INTERFACE_DEFINED__
#define __ICrmMonitorClerks_INTERFACE_DEFINED__

/* interface ICrmMonitorClerks */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICrmMonitorClerks;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70C8E442-C7ED-11d1-82FB-00A0C91EEDE9")
    ICrmMonitorClerks : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem) = 0;
        
        virtual /* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ProgIdCompensator( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Description( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TransactionUOW( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ActivityId( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmMonitorClerksVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmMonitorClerks __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmMonitorClerks __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem);
        
        /* [restricted][helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ProgIdCompensator )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Description )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TransactionUOW )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ActivityId )( 
            ICrmMonitorClerks __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem);
        
        END_INTERFACE
    } ICrmMonitorClerksVtbl;

    interface ICrmMonitorClerks
    {
        CONST_VTBL struct ICrmMonitorClerksVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmMonitorClerks_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmMonitorClerks_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmMonitorClerks_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmMonitorClerks_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICrmMonitorClerks_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICrmMonitorClerks_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICrmMonitorClerks_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICrmMonitorClerks_Item(This,Index,pItem)	\
    (This)->lpVtbl -> Item(This,Index,pItem)

#define ICrmMonitorClerks_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define ICrmMonitorClerks_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define ICrmMonitorClerks_ProgIdCompensator(This,Index,pItem)	\
    (This)->lpVtbl -> ProgIdCompensator(This,Index,pItem)

#define ICrmMonitorClerks_Description(This,Index,pItem)	\
    (This)->lpVtbl -> Description(This,Index,pItem)

#define ICrmMonitorClerks_TransactionUOW(This,Index,pItem)	\
    (This)->lpVtbl -> TransactionUOW(This,Index,pItem)

#define ICrmMonitorClerks_ActivityId(This,Index,pItem)	\
    (This)->lpVtbl -> ActivityId(This,Index,pItem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_Item_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPVARIANT pItem);


void __RPC_STUB ICrmMonitorClerks_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_get__NewEnum_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [retval][out] */ LPUNKNOWN __RPC_FAR *pVal);


void __RPC_STUB ICrmMonitorClerks_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_get_Count_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB ICrmMonitorClerks_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_ProgIdCompensator_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPVARIANT pItem);


void __RPC_STUB ICrmMonitorClerks_ProgIdCompensator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_Description_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPVARIANT pItem);


void __RPC_STUB ICrmMonitorClerks_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_TransactionUOW_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPVARIANT pItem);


void __RPC_STUB ICrmMonitorClerks_TransactionUOW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitorClerks_ActivityId_Proxy( 
    ICrmMonitorClerks __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPVARIANT pItem);


void __RPC_STUB ICrmMonitorClerks_ActivityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmMonitorClerks_INTERFACE_DEFINED__ */


#ifndef __ICrmMonitor_INTERFACE_DEFINED__
#define __ICrmMonitor_INTERFACE_DEFINED__

/* interface ICrmMonitor */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICrmMonitor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70C8E443-C7ED-11d1-82FB-00A0C91EEDE9")
    ICrmMonitor : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetClerks( 
            /* [retval][out] */ ICrmMonitorClerks __RPC_FAR *__RPC_FAR *pClerks) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HoldClerk( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmMonitorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmMonitor __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmMonitor __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmMonitor __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetClerks )( 
            ICrmMonitor __RPC_FAR * This,
            /* [retval][out] */ ICrmMonitorClerks __RPC_FAR *__RPC_FAR *pClerks);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HoldClerk )( 
            ICrmMonitor __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPVARIANT pItem);
        
        END_INTERFACE
    } ICrmMonitorVtbl;

    interface ICrmMonitor
    {
        CONST_VTBL struct ICrmMonitorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmMonitor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmMonitor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmMonitor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmMonitor_GetClerks(This,pClerks)	\
    (This)->lpVtbl -> GetClerks(This,pClerks)

#define ICrmMonitor_HoldClerk(This,Index,pItem)	\
    (This)->lpVtbl -> HoldClerk(This,Index,pItem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitor_GetClerks_Proxy( 
    ICrmMonitor __RPC_FAR * This,
    /* [retval][out] */ ICrmMonitorClerks __RPC_FAR *__RPC_FAR *pClerks);


void __RPC_STUB ICrmMonitor_GetClerks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmMonitor_HoldClerk_Proxy( 
    ICrmMonitor __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPVARIANT pItem);


void __RPC_STUB ICrmMonitor_HoldClerk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmMonitor_INTERFACE_DEFINED__ */


#ifndef __ICrmFormatLogRecords_INTERFACE_DEFINED__
#define __ICrmFormatLogRecords_INTERFACE_DEFINED__

/* interface ICrmFormatLogRecords */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICrmFormatLogRecords;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9C51D821-C98B-11d1-82FB-00A0C91EEDE9")
    ICrmFormatLogRecords : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetColumnCount( 
            /* [out] */ long __RPC_FAR *plColumnCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetColumnHeaders( 
            /* [out] */ LPVARIANT pHeaders) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetColumn( 
            /* [in] */ CrmLogRecordRead CrmLogRec,
            /* [out] */ LPVARIANT pFormattedLogRecord) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetColumnVariants( 
            /* [in] */ VARIANT LogRecord,
            /* [out] */ LPVARIANT pFormattedLogRecord) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICrmFormatLogRecordsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICrmFormatLogRecords __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICrmFormatLogRecords __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICrmFormatLogRecords __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColumnCount )( 
            ICrmFormatLogRecords __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *plColumnCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColumnHeaders )( 
            ICrmFormatLogRecords __RPC_FAR * This,
            /* [out] */ LPVARIANT pHeaders);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColumn )( 
            ICrmFormatLogRecords __RPC_FAR * This,
            /* [in] */ CrmLogRecordRead CrmLogRec,
            /* [out] */ LPVARIANT pFormattedLogRecord);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetColumnVariants )( 
            ICrmFormatLogRecords __RPC_FAR * This,
            /* [in] */ VARIANT LogRecord,
            /* [out] */ LPVARIANT pFormattedLogRecord);
        
        END_INTERFACE
    } ICrmFormatLogRecordsVtbl;

    interface ICrmFormatLogRecords
    {
        CONST_VTBL struct ICrmFormatLogRecordsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICrmFormatLogRecords_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICrmFormatLogRecords_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICrmFormatLogRecords_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICrmFormatLogRecords_GetColumnCount(This,plColumnCount)	\
    (This)->lpVtbl -> GetColumnCount(This,plColumnCount)

#define ICrmFormatLogRecords_GetColumnHeaders(This,pHeaders)	\
    (This)->lpVtbl -> GetColumnHeaders(This,pHeaders)

#define ICrmFormatLogRecords_GetColumn(This,CrmLogRec,pFormattedLogRecord)	\
    (This)->lpVtbl -> GetColumn(This,CrmLogRec,pFormattedLogRecord)

#define ICrmFormatLogRecords_GetColumnVariants(This,LogRecord,pFormattedLogRecord)	\
    (This)->lpVtbl -> GetColumnVariants(This,LogRecord,pFormattedLogRecord)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmFormatLogRecords_GetColumnCount_Proxy( 
    ICrmFormatLogRecords __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *plColumnCount);


void __RPC_STUB ICrmFormatLogRecords_GetColumnCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmFormatLogRecords_GetColumnHeaders_Proxy( 
    ICrmFormatLogRecords __RPC_FAR * This,
    /* [out] */ LPVARIANT pHeaders);


void __RPC_STUB ICrmFormatLogRecords_GetColumnHeaders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmFormatLogRecords_GetColumn_Proxy( 
    ICrmFormatLogRecords __RPC_FAR * This,
    /* [in] */ CrmLogRecordRead CrmLogRec,
    /* [out] */ LPVARIANT pFormattedLogRecord);


void __RPC_STUB ICrmFormatLogRecords_GetColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICrmFormatLogRecords_GetColumnVariants_Proxy( 
    ICrmFormatLogRecords __RPC_FAR * This,
    /* [in] */ VARIANT LogRecord,
    /* [out] */ LPVARIANT pFormattedLogRecord);


void __RPC_STUB ICrmFormatLogRecords_GetColumnVariants_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICrmFormatLogRecords_INTERFACE_DEFINED__ */



#ifndef __COMSVCSLib_LIBRARY_DEFINED__
#define __COMSVCSLib_LIBRARY_DEFINED__

/* library COMSVCSLib */
/* [helpfile][helpstring][version][uuid] */ 


typedef /* [public][helpcontext][helpstring] */ 
enum __MIDL___MIDL_itf_autosvcs_0332_0001
    {	mtsErrCtxAborted	= 0x8004e002,
	mtsErrCtxAborting	= 0x8004e003,
	mtsErrCtxNoContext	= 0x8004e004,
	mtsErrCtxNotRegistered	= 0x8004e005,
	mtsErrCtxSynchTimeout	= 0x8004e006,
	mtsErrCtxOldReference	= 0x8004e007,
	mtsErrCtxRoleNotFound	= 0x8004e00c,
	mtsErrCtxNoSecurity	= 0x8004e00d,
	mtsErrCtxWrongThread	= 0x8004e00e,
	mtsErrCtxTMNotAvailable	= 0x8004e00f,
	comQCErrApplicationNotQueued	= 0x80110600,
	comQCErrNoQueueableInterfaces	= 0x80110601,
	comQCErrQueuingServiceNotAvailable	= 0x80110602,
	comQCErrQueueTransactMismatch	= 0x80110603,
	comqcErrRecorderMarshalled	= 0x80110604,
	comqcErrOutParam	= 0x80110605,
	comqcErrRecorderNotTrusted	= 0x80110606,
	comqcErrPSLoad	= 0x80110607,
	comqcErrMarshaledObjSameTxn	= 0x80110608,
	comqcErrInvalidMessage	= 0x80110650,
	comqcErrMsmqSidUnavailable	= 0x80110651,
	comqcErrWrongMsgExtension	= 0x80110652,
	comqcErrMsmqServiceUnavailable	= 0x80110653,
	comqcErrMsgNotAuthenticated	= 0x80110654,
	comqcErrMsmqConnectorUsed	= 0x80110655,
	comqcErrBadMarshaledObject	= 0x80110656
    }	Error_Constants;


typedef /* [public] */ 
enum __MIDL___MIDL_itf_autosvcs_0332_0002
    {	LockSetGet	= 0,
	LockMethod	= LockSetGet + 1
    }	LockModes;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_autosvcs_0332_0003
    {	Standard	= 0,
	Process	= Standard + 1
    }	ReleaseModes;

#ifndef _tagCrmFlags_
#define _tagCrmFlags_
typedef 
enum tagCRMFLAGS
    {	CRMFLAG_FORGETTARGET	= 0x1,
	CRMFLAG_WRITTENDURINGPREPARE	= 0x2,
	CRMFLAG_WRITTENDURINGCOMMIT	= 0x4,
	CRMFLAG_WRITTENDURINGABORT	= 0x8,
	CRMFLAG_WRITTENDURINGRECOVERY	= 0x10,
	CRMFLAG_WRITTENDURINGREPLAY	= 0x20,
	CRMFLAG_REPLAYINPROGRESS	= 0x40
    }	CRMFLAGS;

#endif _tagCrmFlags_
#ifndef _tagCrmRegFlags_
#define _tagCrmRegFlags_
typedef 
enum tagCRMREGFLAGS
    {	CRMREGFLAG_PREPAREPHASE	= 0x1,
	CRMREGFLAG_COMMITPHASE	= 0x2,
	CRMREGFLAG_ABORTPHASE	= 0x4,
	CRMREGFLAG_ALLPHASES	= 0x7,
	CRMREGFLAG_FAILIFINDOUBTSREMAIN	= 0x10
    }	CRMREGFLAGS;

#endif _tagCrmRegFlags_

EXTERN_C const IID LIBID_COMSVCSLib;

EXTERN_C const CLSID CLSID_SecurityIdentity;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0a5-7f19-11d2-978e-0000f8757e2a")
SecurityIdentity;
#endif

EXTERN_C const CLSID CLSID_SecurityCallers;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0a6-7f19-11d2-978e-0000f8757e2a")
SecurityCallers;
#endif

EXTERN_C const CLSID CLSID_SecurityCallContext;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0a7-7f19-11d2-978e-0000f8757e2a")
SecurityCallContext;
#endif

EXTERN_C const CLSID CLSID_GetSecurityCallContextAppObject;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0a8-7f19-11d2-978e-0000f8757e2a")
GetSecurityCallContextAppObject;
#endif

EXTERN_C const CLSID CLSID_Dummy30040732;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0a9-7f19-11d2-978e-0000f8757e2a")
Dummy30040732;
#endif

EXTERN_C const CLSID CLSID_TransactionContext;

#ifdef __cplusplus

class DECLSPEC_UUID("7999FC25-D3C6-11CF-ACAB-00A024A55AEF")
TransactionContext;
#endif

EXTERN_C const CLSID CLSID_TransactionContextEx;

#ifdef __cplusplus

class DECLSPEC_UUID("5cb66670-d3d4-11cf-acab-00a024a55aef")
TransactionContextEx;
#endif

EXTERN_C const CLSID CLSID_SharedProperty;

#ifdef __cplusplus

class DECLSPEC_UUID("2A005C05-A5DE-11CF-9E66-00AA00A3F464")
SharedProperty;
#endif

EXTERN_C const CLSID CLSID_SharedPropertyGroup;

#ifdef __cplusplus

class DECLSPEC_UUID("2A005C0B-A5DE-11CF-9E66-00AA00A3F464")
SharedPropertyGroup;
#endif

EXTERN_C const CLSID CLSID_SharedPropertyGroupManager;

#ifdef __cplusplus

class DECLSPEC_UUID("2A005C11-A5DE-11CF-9E66-00AA00A3F464")
SharedPropertyGroupManager;
#endif

EXTERN_C const CLSID CLSID_COMEvents;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0ab-7f19-11d2-978e-0000f8757e2a")
COMEvents;
#endif

EXTERN_C const CLSID CLSID_CoMTSLocator;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0ac-7f19-11d2-978e-0000f8757e2a")
CoMTSLocator;
#endif

EXTERN_C const CLSID CLSID_MtsGrp;

#ifdef __cplusplus

class DECLSPEC_UUID("4B2E958D-0393-11D1-B1AB-00AA00BA3258")
MtsGrp;
#endif

EXTERN_C const CLSID CLSID_ComServiceEvents;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0c3-7f19-11d2-978e-0000f8757e2a")
ComServiceEvents;
#endif

EXTERN_C const CLSID CLSID_CRMClerk;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0bd-7f19-11d2-978e-0000f8757e2a")
CRMClerk;
#endif

EXTERN_C const CLSID CLSID_CRMRecoveryClerk;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0be-7f19-11d2-978e-0000f8757e2a")
CRMRecoveryClerk;
#endif

EXTERN_C const CLSID CLSID_LBEvents;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0c1-7f19-11d2-978e-0000f8757e2a")
LBEvents;
#endif

EXTERN_C const CLSID CLSID_MessageMover;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0bf-7f19-11d2-978e-0000f8757e2a")
MessageMover;
#endif

EXTERN_C const CLSID CLSID_DispenserManager;

#ifdef __cplusplus

class DECLSPEC_UUID("ecabb0c0-7f19-11d2-978e-0000f8757e2a")
DispenserManager;
#endif
#endif /* __COMSVCSLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


