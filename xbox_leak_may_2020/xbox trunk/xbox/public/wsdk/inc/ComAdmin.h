
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0328 */
/* Compiler settings for comadmin.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
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

#ifndef __comadmin_h__
#define __comadmin_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRunningAppCollection_FWD_DEFINED__
#define __IRunningAppCollection_FWD_DEFINED__
typedef interface IRunningAppCollection IRunningAppCollection;
#endif 	/* __IRunningAppCollection_FWD_DEFINED__ */


#ifndef __IRunningAppInfo_FWD_DEFINED__
#define __IRunningAppInfo_FWD_DEFINED__
typedef interface IRunningAppInfo IRunningAppInfo;
#endif 	/* __IRunningAppInfo_FWD_DEFINED__ */


#ifndef __ICOMAdminCatalog_FWD_DEFINED__
#define __ICOMAdminCatalog_FWD_DEFINED__
typedef interface ICOMAdminCatalog ICOMAdminCatalog;
#endif 	/* __ICOMAdminCatalog_FWD_DEFINED__ */


#ifndef __ICOMAdminCatalog2_FWD_DEFINED__
#define __ICOMAdminCatalog2_FWD_DEFINED__
typedef interface ICOMAdminCatalog2 ICOMAdminCatalog2;
#endif 	/* __ICOMAdminCatalog2_FWD_DEFINED__ */


#ifndef __ICatalogObject_FWD_DEFINED__
#define __ICatalogObject_FWD_DEFINED__
typedef interface ICatalogObject ICatalogObject;
#endif 	/* __ICatalogObject_FWD_DEFINED__ */


#ifndef __ICatalogCollection_FWD_DEFINED__
#define __ICatalogCollection_FWD_DEFINED__
typedef interface ICatalogCollection ICatalogCollection;
#endif 	/* __ICatalogCollection_FWD_DEFINED__ */


#ifndef __IRunningAppCollection_FWD_DEFINED__
#define __IRunningAppCollection_FWD_DEFINED__
typedef interface IRunningAppCollection IRunningAppCollection;
#endif 	/* __IRunningAppCollection_FWD_DEFINED__ */


#ifndef __IRunningAppInfo_FWD_DEFINED__
#define __IRunningAppInfo_FWD_DEFINED__
typedef interface IRunningAppInfo IRunningAppInfo;
#endif 	/* __IRunningAppInfo_FWD_DEFINED__ */


#ifndef __COMAdminCatalog_FWD_DEFINED__
#define __COMAdminCatalog_FWD_DEFINED__

#ifdef __cplusplus
typedef class COMAdminCatalog COMAdminCatalog;
#else
typedef struct COMAdminCatalog COMAdminCatalog;
#endif /* __cplusplus */

#endif 	/* __COMAdminCatalog_FWD_DEFINED__ */


#ifndef __COMAdminCatalogObject_FWD_DEFINED__
#define __COMAdminCatalogObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class COMAdminCatalogObject COMAdminCatalogObject;
#else
typedef struct COMAdminCatalogObject COMAdminCatalogObject;
#endif /* __cplusplus */

#endif 	/* __COMAdminCatalogObject_FWD_DEFINED__ */


#ifndef __COMAdminCatalogCollection_FWD_DEFINED__
#define __COMAdminCatalogCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class COMAdminCatalogCollection COMAdminCatalogCollection;
#else
typedef struct COMAdminCatalogCollection COMAdminCatalogCollection;
#endif /* __cplusplus */

#endif 	/* __COMAdminCatalogCollection_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_comadmin_0000 */
/* [local] */ 

// -----------------------------------------------------------------------	
// comadmin.h  -- COM Administration Programming Interfaces					
//																			
// This file provides the prototypes for the APIs and COM interfaces			
// used by Microsoft COM applications.										
//																			
// Copyright (c) 1995-1999 Microsoft Corporation.  All Rights Reserved.		
// -----------------------------------------------------------------------	
#include <objbase.h>
#ifndef DECLSPEC_UUID
#if _MSC_VER >= 1100
#define DECLSPEC_UUID(x)    __declspec(uuid(x))
#else
#define DECLSPEC_UUID(x)
#endif
#endif


extern RPC_IF_HANDLE __MIDL_itf_comadmin_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comadmin_0000_v0_0_s_ifspec;

#ifndef __IRunningAppCollection_INTERFACE_DEFINED__
#define __IRunningAppCollection_INTERFACE_DEFINED__

/* interface IRunningAppCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IRunningAppCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ab9d3261-d6ea-4fbd-80f6-cf7bad0732f3")
    IRunningAppCollection : public IDispatch
    {
    public:
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVariant) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long lIndex,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppAppObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRunningAppCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRunningAppCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRunningAppCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVariant);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IRunningAppCollection __RPC_FAR * This,
            /* [in] */ long lIndex,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppAppObject);
        
        END_INTERFACE
    } IRunningAppCollectionVtbl;

    interface IRunningAppCollection
    {
        CONST_VTBL struct IRunningAppCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRunningAppCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRunningAppCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRunningAppCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRunningAppCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRunningAppCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRunningAppCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRunningAppCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRunningAppCollection_get__NewEnum(This,ppEnumVariant)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnumVariant)

#define IRunningAppCollection_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IRunningAppCollection_get_Item(This,lIndex,ppAppObject)	\
    (This)->lpVtbl -> get_Item(This,lIndex,ppAppObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppCollection_get__NewEnum_Proxy( 
    IRunningAppCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVariant);


void __RPC_STUB IRunningAppCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppCollection_get_Count_Proxy( 
    IRunningAppCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IRunningAppCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppCollection_get_Item_Proxy( 
    IRunningAppCollection __RPC_FAR * This,
    /* [in] */ long lIndex,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppAppObject);


void __RPC_STUB IRunningAppCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRunningAppCollection_INTERFACE_DEFINED__ */


#ifndef __IRunningAppInfo_INTERFACE_DEFINED__
#define __IRunningAppInfo_INTERFACE_DEFINED__

/* interface IRunningAppInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IRunningAppInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1fd6f178-bfb9-4629-93c7-4ca9a2724efd")
    IRunningAppInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ApplicationID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAppID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InstanceID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrInstanceID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProcessID( 
            /* [retval][out] */ long __RPC_FAR *plProcessID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Paused( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbPaused) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRunningAppInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRunningAppInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRunningAppInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ApplicationID )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrAppID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InstanceID )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrInstanceID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProcessID )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plProcessID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Paused )( 
            IRunningAppInfo __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbPaused);
        
        END_INTERFACE
    } IRunningAppInfoVtbl;

    interface IRunningAppInfo
    {
        CONST_VTBL struct IRunningAppInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRunningAppInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRunningAppInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRunningAppInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRunningAppInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IRunningAppInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IRunningAppInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IRunningAppInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IRunningAppInfo_get_ApplicationID(This,pbstrAppID)	\
    (This)->lpVtbl -> get_ApplicationID(This,pbstrAppID)

#define IRunningAppInfo_get_InstanceID(This,pbstrInstanceID)	\
    (This)->lpVtbl -> get_InstanceID(This,pbstrInstanceID)

#define IRunningAppInfo_get_ProcessID(This,plProcessID)	\
    (This)->lpVtbl -> get_ProcessID(This,plProcessID)

#define IRunningAppInfo_get_Paused(This,pbPaused)	\
    (This)->lpVtbl -> get_Paused(This,pbPaused)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppInfo_get_ApplicationID_Proxy( 
    IRunningAppInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrAppID);


void __RPC_STUB IRunningAppInfo_get_ApplicationID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppInfo_get_InstanceID_Proxy( 
    IRunningAppInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrInstanceID);


void __RPC_STUB IRunningAppInfo_get_InstanceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppInfo_get_ProcessID_Proxy( 
    IRunningAppInfo __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plProcessID);


void __RPC_STUB IRunningAppInfo_get_ProcessID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IRunningAppInfo_get_Paused_Proxy( 
    IRunningAppInfo __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pbPaused);


void __RPC_STUB IRunningAppInfo_get_Paused_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRunningAppInfo_INTERFACE_DEFINED__ */


#ifndef __ICOMAdminCatalog_INTERFACE_DEFINED__
#define __ICOMAdminCatalog_INTERFACE_DEFINED__

/* interface ICOMAdminCatalog */
/* [unique][helpstring][dual][uuid][object] */ 




EXTERN_C const IID IID_ICOMAdminCatalog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DD662187-DFC2-11d1-A2CF-00805FC79235")
    ICOMAdminCatalog : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCollection( 
            /* [in] */ BSTR bstrCollName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ BSTR bstrConnectString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MajorVersion( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MinorVersion( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCollectionByQuery( 
            /* [in] */ BSTR bstrCollName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *aQuery,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ImportComponent( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstallComponent( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrDLL,
            /* [in] */ BSTR bstrTLB,
            /* [in] */ BSTR bstrPSDLL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShutdownApplication( 
            /* [in] */ BSTR bstrApplIdOrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExportApplication( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrApplicationFile,
            /* [in] */ long lOptions) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstallApplication( 
            /* [in] */ BSTR bstrApplicationFile,
            /* [optional][in] */ BSTR bstrDestinationDirectory,
            /* [optional][in] */ long lOptions,
            /* [optional][in] */ BSTR bstrUserId,
            /* [optional][in] */ BSTR bstrPassword,
            /* [optional][in] */ BSTR bstrRSN) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StopRouter( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RefreshRouter( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartRouter( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reserved1( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reserved2( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstallMultipleComponents( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMultipleComponentsInfo( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varClassNames,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileFlags,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varComponentFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RefreshComponents( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BackupREGDB( 
            /* [in] */ BSTR bstrBackupFilePath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RestoreREGDB( 
            /* [in] */ BSTR bstrBackupFilePath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryApplicationFile( 
            /* [in] */ BSTR bstrApplicationFile,
            /* [out] */ BSTR __RPC_FAR *bstrApplicationName,
            /* [out] */ BSTR __RPC_FAR *bstrApplicationDescription,
            /* [out] */ VARIANT_BOOL __RPC_FAR *bHasUsers,
            /* [out] */ VARIANT_BOOL __RPC_FAR *bIsProxy,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartApplication( 
            /* [in] */ BSTR bstrApplIdOrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ServiceCheck( 
            /* [in] */ long lService,
            /* [retval][out] */ long __RPC_FAR *pulStatus) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstallMultipleEventClasses( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstallEventClass( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrDLL,
            /* [in] */ BSTR bstrTLB,
            /* [in] */ BSTR bstrPSDLL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEventClassesForIID( 
            /* [in] */ BSTR bstrIID,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varProgIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varDescriptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOMAdminCatalogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCollection )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrCollName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrConnectString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MajorVersion )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MinorVersion )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCollectionByQuery )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrCollName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *aQuery,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ImportComponent )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallComponent )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrDLL,
            /* [in] */ BSTR bstrTLB,
            /* [in] */ BSTR bstrPSDLL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownApplication )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExportApplication )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrApplicationFile,
            /* [in] */ long lOptions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallApplication )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplicationFile,
            /* [optional][in] */ BSTR bstrDestinationDirectory,
            /* [optional][in] */ long lOptions,
            /* [optional][in] */ BSTR bstrUserId,
            /* [optional][in] */ BSTR bstrPassword,
            /* [optional][in] */ BSTR bstrRSN);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopRouter )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RefreshRouter )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartRouter )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallMultipleComponents )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMultipleComponentsInfo )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varClassNames,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileFlags,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varComponentFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RefreshComponents )( 
            ICOMAdminCatalog __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BackupREGDB )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrBackupFilePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreREGDB )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrBackupFilePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryApplicationFile )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplicationFile,
            /* [out] */ BSTR __RPC_FAR *bstrApplicationName,
            /* [out] */ BSTR __RPC_FAR *bstrApplicationDescription,
            /* [out] */ VARIANT_BOOL __RPC_FAR *bHasUsers,
            /* [out] */ VARIANT_BOOL __RPC_FAR *bIsProxy,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartApplication )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ServiceCheck )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ long lService,
            /* [retval][out] */ long __RPC_FAR *pulStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallMultipleEventClasses )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallEventClass )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrDLL,
            /* [in] */ BSTR bstrTLB,
            /* [in] */ BSTR bstrPSDLL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventClassesForIID )( 
            ICOMAdminCatalog __RPC_FAR * This,
            /* [in] */ BSTR bstrIID,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varProgIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varDescriptions);
        
        END_INTERFACE
    } ICOMAdminCatalogVtbl;

    interface ICOMAdminCatalog
    {
        CONST_VTBL struct ICOMAdminCatalogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOMAdminCatalog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOMAdminCatalog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOMAdminCatalog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOMAdminCatalog_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICOMAdminCatalog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICOMAdminCatalog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICOMAdminCatalog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICOMAdminCatalog_GetCollection(This,bstrCollName,ppCatalogCollection)	\
    (This)->lpVtbl -> GetCollection(This,bstrCollName,ppCatalogCollection)

#define ICOMAdminCatalog_Connect(This,bstrConnectString,ppCatalogCollection)	\
    (This)->lpVtbl -> Connect(This,bstrConnectString,ppCatalogCollection)

#define ICOMAdminCatalog_get_MajorVersion(This,retval)	\
    (This)->lpVtbl -> get_MajorVersion(This,retval)

#define ICOMAdminCatalog_get_MinorVersion(This,retval)	\
    (This)->lpVtbl -> get_MinorVersion(This,retval)

#define ICOMAdminCatalog_GetCollectionByQuery(This,bstrCollName,aQuery,ppCatalogCollection)	\
    (This)->lpVtbl -> GetCollectionByQuery(This,bstrCollName,aQuery,ppCatalogCollection)

#define ICOMAdminCatalog_ImportComponent(This,bstrApplIdOrName,bstrCLSIDOrProgId)	\
    (This)->lpVtbl -> ImportComponent(This,bstrApplIdOrName,bstrCLSIDOrProgId)

#define ICOMAdminCatalog_InstallComponent(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)	\
    (This)->lpVtbl -> InstallComponent(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)

#define ICOMAdminCatalog_ShutdownApplication(This,bstrApplIdOrName)	\
    (This)->lpVtbl -> ShutdownApplication(This,bstrApplIdOrName)

#define ICOMAdminCatalog_ExportApplication(This,bstrApplIdOrName,bstrApplicationFile,lOptions)	\
    (This)->lpVtbl -> ExportApplication(This,bstrApplIdOrName,bstrApplicationFile,lOptions)

#define ICOMAdminCatalog_InstallApplication(This,bstrApplicationFile,bstrDestinationDirectory,lOptions,bstrUserId,bstrPassword,bstrRSN)	\
    (This)->lpVtbl -> InstallApplication(This,bstrApplicationFile,bstrDestinationDirectory,lOptions,bstrUserId,bstrPassword,bstrRSN)

#define ICOMAdminCatalog_StopRouter(This)	\
    (This)->lpVtbl -> StopRouter(This)

#define ICOMAdminCatalog_RefreshRouter(This)	\
    (This)->lpVtbl -> RefreshRouter(This)

#define ICOMAdminCatalog_StartRouter(This)	\
    (This)->lpVtbl -> StartRouter(This)

#define ICOMAdminCatalog_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define ICOMAdminCatalog_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define ICOMAdminCatalog_InstallMultipleComponents(This,bstrApplIdOrName,varFileNames,varCLSIDS)	\
    (This)->lpVtbl -> InstallMultipleComponents(This,bstrApplIdOrName,varFileNames,varCLSIDS)

#define ICOMAdminCatalog_GetMultipleComponentsInfo(This,bstrApplIdOrName,varFileNames,varCLSIDs,varClassNames,varFileFlags,varComponentFlags)	\
    (This)->lpVtbl -> GetMultipleComponentsInfo(This,bstrApplIdOrName,varFileNames,varCLSIDs,varClassNames,varFileFlags,varComponentFlags)

#define ICOMAdminCatalog_RefreshComponents(This)	\
    (This)->lpVtbl -> RefreshComponents(This)

#define ICOMAdminCatalog_BackupREGDB(This,bstrBackupFilePath)	\
    (This)->lpVtbl -> BackupREGDB(This,bstrBackupFilePath)

#define ICOMAdminCatalog_RestoreREGDB(This,bstrBackupFilePath)	\
    (This)->lpVtbl -> RestoreREGDB(This,bstrBackupFilePath)

#define ICOMAdminCatalog_QueryApplicationFile(This,bstrApplicationFile,bstrApplicationName,bstrApplicationDescription,bHasUsers,bIsProxy,varFileNames)	\
    (This)->lpVtbl -> QueryApplicationFile(This,bstrApplicationFile,bstrApplicationName,bstrApplicationDescription,bHasUsers,bIsProxy,varFileNames)

#define ICOMAdminCatalog_StartApplication(This,bstrApplIdOrName)	\
    (This)->lpVtbl -> StartApplication(This,bstrApplIdOrName)

#define ICOMAdminCatalog_ServiceCheck(This,lService,pulStatus)	\
    (This)->lpVtbl -> ServiceCheck(This,lService,pulStatus)

#define ICOMAdminCatalog_InstallMultipleEventClasses(This,bstrApplIdOrName,varFileNames,varCLSIDS)	\
    (This)->lpVtbl -> InstallMultipleEventClasses(This,bstrApplIdOrName,varFileNames,varCLSIDS)

#define ICOMAdminCatalog_InstallEventClass(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)	\
    (This)->lpVtbl -> InstallEventClass(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)

#define ICOMAdminCatalog_GetEventClassesForIID(This,bstrIID,varCLSIDs,varProgIDs,varDescriptions)	\
    (This)->lpVtbl -> GetEventClassesForIID(This,bstrIID,varCLSIDs,varProgIDs,varDescriptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_GetCollection_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrCollName,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);


void __RPC_STUB ICOMAdminCatalog_GetCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_Connect_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrConnectString,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);


void __RPC_STUB ICOMAdminCatalog_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_get_MajorVersion_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ICOMAdminCatalog_get_MajorVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_get_MinorVersion_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ICOMAdminCatalog_get_MinorVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_GetCollectionByQuery_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrCollName,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *aQuery,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);


void __RPC_STUB ICOMAdminCatalog_GetCollectionByQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_ImportComponent_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ BSTR bstrCLSIDOrProgId);


void __RPC_STUB ICOMAdminCatalog_ImportComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_InstallComponent_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ BSTR bstrDLL,
    /* [in] */ BSTR bstrTLB,
    /* [in] */ BSTR bstrPSDLL);


void __RPC_STUB ICOMAdminCatalog_InstallComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_ShutdownApplication_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName);


void __RPC_STUB ICOMAdminCatalog_ShutdownApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_ExportApplication_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ BSTR bstrApplicationFile,
    /* [in] */ long lOptions);


void __RPC_STUB ICOMAdminCatalog_ExportApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_InstallApplication_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplicationFile,
    /* [optional][in] */ BSTR bstrDestinationDirectory,
    /* [optional][in] */ long lOptions,
    /* [optional][in] */ BSTR bstrUserId,
    /* [optional][in] */ BSTR bstrPassword,
    /* [optional][in] */ BSTR bstrRSN);


void __RPC_STUB ICOMAdminCatalog_InstallApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_StopRouter_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog_StopRouter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_RefreshRouter_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog_RefreshRouter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_StartRouter_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog_StartRouter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_Reserved1_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog_Reserved1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_Reserved2_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog_Reserved2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_InstallMultipleComponents_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS);


void __RPC_STUB ICOMAdminCatalog_InstallMultipleComponents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_GetMultipleComponentsInfo_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varClassNames,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileFlags,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varComponentFlags);


void __RPC_STUB ICOMAdminCatalog_GetMultipleComponentsInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_RefreshComponents_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog_RefreshComponents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_BackupREGDB_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrBackupFilePath);


void __RPC_STUB ICOMAdminCatalog_BackupREGDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_RestoreREGDB_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrBackupFilePath);


void __RPC_STUB ICOMAdminCatalog_RestoreREGDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_QueryApplicationFile_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplicationFile,
    /* [out] */ BSTR __RPC_FAR *bstrApplicationName,
    /* [out] */ BSTR __RPC_FAR *bstrApplicationDescription,
    /* [out] */ VARIANT_BOOL __RPC_FAR *bHasUsers,
    /* [out] */ VARIANT_BOOL __RPC_FAR *bIsProxy,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames);


void __RPC_STUB ICOMAdminCatalog_QueryApplicationFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_StartApplication_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName);


void __RPC_STUB ICOMAdminCatalog_StartApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_ServiceCheck_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ long lService,
    /* [retval][out] */ long __RPC_FAR *pulStatus);


void __RPC_STUB ICOMAdminCatalog_ServiceCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_InstallMultipleEventClasses_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS);


void __RPC_STUB ICOMAdminCatalog_InstallMultipleEventClasses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_InstallEventClass_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ BSTR bstrDLL,
    /* [in] */ BSTR bstrTLB,
    /* [in] */ BSTR bstrPSDLL);


void __RPC_STUB ICOMAdminCatalog_InstallEventClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog_GetEventClassesForIID_Proxy( 
    ICOMAdminCatalog __RPC_FAR * This,
    /* [in] */ BSTR bstrIID,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varProgIDs,
    /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varDescriptions);


void __RPC_STUB ICOMAdminCatalog_GetEventClassesForIID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOMAdminCatalog_INTERFACE_DEFINED__ */


#ifndef __ICOMAdminCatalog2_INTERFACE_DEFINED__
#define __ICOMAdminCatalog2_INTERFACE_DEFINED__

/* interface ICOMAdminCatalog2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICOMAdminCatalog2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D81AB10D-2EE4-48db-9C90-54110998B105")
    ICOMAdminCatalog2 : public ICOMAdminCatalog
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShutdownProcess( 
            /* [in] */ BSTR bstrApplInstanceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PauseProcess( 
            /* [in] */ BSTR bstrApplInstanceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResumeProcess( 
            /* [in] */ BSTR bstrApplInstanceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsProcessPaused( 
            /* [in] */ BSTR bstrApplInstanceId,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bPaused) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DumpProcess( 
            /* [in] */ BSTR bstrApplInstanceId,
            /* [in] */ BSTR bstrDirectory,
            /* [in] */ DWORD dwMaxImages,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDumpFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsProcessDumpSupported( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetApplicationPartition( 
            /* [in] */ BSTR bstrApplPartitionIdOrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentApplicationPartitionId( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrApplPartitionId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentApplicationPartitionName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrApplPartitionName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRunningApps( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppRunningAppCollection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateServiceForApplication( 
            /* [in] */ BSTR bstrAppIdOrName,
            /* [in] */ BSTR bstrServiceName,
            /* [in] */ BSTR bstrStartType,
            /* [in] */ BSTR bstrErrorControl,
            /* [in] */ BSTR bstrDependencies,
            /* [in] */ BSTR bstrRunAs,
            /* [in] */ BSTR bstrPassword,
            /* [in] */ VARIANT_BOOL bDesktopOk) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteServiceForApplication( 
            /* [in] */ BSTR bstrAppIdOrName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBaseApplicationPartitionId( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBasePartitionId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyApplications( 
            /* [in] */ BSTR bstrSourcePartitionId,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varApplIds) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyComponent( 
            /* [in] */ BSTR bstrSourcePartitionId,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ BSTR bstrNewApplId,
            /* [in] */ BSTR bstrNewClsid,
            /* [in] */ BSTR bstrNewProgId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ImportComponentAsLegacy( 
            /* [in] */ BSTR bstrAppIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId,
            /* [in] */ long lComponentType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PromoteLegacyComponent( 
            /* [in] */ BSTR bstrAppIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAppPartition( 
            /* [in] */ BSTR bstrAppId,
            /* [out] */ BSTR __RPC_FAR *pbstrPartitionId,
            /* [out] */ BSTR __RPC_FAR *pbstrPartitionName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ImportComponentByType( 
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId,
            /* [in] */ long lComponentType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Is64BitCatalogServer( 
            /* [out] */ VARIANT_BOOL __RPC_FAR *pbIs64Bit) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveComponent( 
            /* [in] */ BSTR bstrSourcePartitionId,
            /* [in] */ BSTR bstrDestinationPartitionId,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ BSTR bstrDestinationApplId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetInstanceIDFromPID( 
            /* [in] */ DWORD dwPID,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrInstanceID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICOMAdminCatalog2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCollection )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrCollName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrConnectString,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MajorVersion )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MinorVersion )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCollectionByQuery )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrCollName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *aQuery,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ImportComponent )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallComponent )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrDLL,
            /* [in] */ BSTR bstrTLB,
            /* [in] */ BSTR bstrPSDLL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownApplication )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExportApplication )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrApplicationFile,
            /* [in] */ long lOptions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallApplication )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplicationFile,
            /* [optional][in] */ BSTR bstrDestinationDirectory,
            /* [optional][in] */ long lOptions,
            /* [optional][in] */ BSTR bstrUserId,
            /* [optional][in] */ BSTR bstrPassword,
            /* [optional][in] */ BSTR bstrRSN);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopRouter )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RefreshRouter )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartRouter )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallMultipleComponents )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMultipleComponentsInfo )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varClassNames,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileFlags,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varComponentFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RefreshComponents )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BackupREGDB )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrBackupFilePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RestoreREGDB )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrBackupFilePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryApplicationFile )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplicationFile,
            /* [out] */ BSTR __RPC_FAR *bstrApplicationName,
            /* [out] */ BSTR __RPC_FAR *bstrApplicationDescription,
            /* [out] */ VARIANT_BOOL __RPC_FAR *bHasUsers,
            /* [out] */ VARIANT_BOOL __RPC_FAR *bIsProxy,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartApplication )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ServiceCheck )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ long lService,
            /* [retval][out] */ long __RPC_FAR *pulStatus);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallMultipleEventClasses )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varFileNames,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDS);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InstallEventClass )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrDLL,
            /* [in] */ BSTR bstrTLB,
            /* [in] */ BSTR bstrPSDLL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEventClassesForIID )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrIID,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varCLSIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varProgIDs,
            /* [out] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varDescriptions);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShutdownProcess )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplInstanceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PauseProcess )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplInstanceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ResumeProcess )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplInstanceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsProcessPaused )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplInstanceId,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bPaused);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DumpProcess )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplInstanceId,
            /* [in] */ BSTR bstrDirectory,
            /* [in] */ DWORD dwMaxImages,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDumpFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsProcessDumpSupported )( 
            ICOMAdminCatalog2 __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetApplicationPartition )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplPartitionIdOrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentApplicationPartitionId )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrApplPartitionId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentApplicationPartitionName )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrApplPartitionName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRunningApps )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppRunningAppCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateServiceForApplication )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrAppIdOrName,
            /* [in] */ BSTR bstrServiceName,
            /* [in] */ BSTR bstrStartType,
            /* [in] */ BSTR bstrErrorControl,
            /* [in] */ BSTR bstrDependencies,
            /* [in] */ BSTR bstrRunAs,
            /* [in] */ BSTR bstrPassword,
            /* [in] */ VARIANT_BOOL bDesktopOk);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteServiceForApplication )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrAppIdOrName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBaseApplicationPartitionId )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrBasePartitionId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyApplications )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrSourcePartitionId,
            /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varApplIds);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CopyComponent )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrSourcePartitionId,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ BSTR bstrNewApplId,
            /* [in] */ BSTR bstrNewClsid,
            /* [in] */ BSTR bstrNewProgId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ImportComponentAsLegacy )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrAppIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId,
            /* [in] */ long lComponentType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PromoteLegacyComponent )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrAppIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAppPartition )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrAppId,
            /* [out] */ BSTR __RPC_FAR *pbstrPartitionId,
            /* [out] */ BSTR __RPC_FAR *pbstrPartitionName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ImportComponentByType )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrApplIdOrName,
            /* [in] */ BSTR bstrCLSIDOrProgId,
            /* [in] */ long lComponentType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Is64BitCatalogServer )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [out] */ VARIANT_BOOL __RPC_FAR *pbIs64Bit);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MoveComponent )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ BSTR bstrSourcePartitionId,
            /* [in] */ BSTR bstrDestinationPartitionId,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ BSTR bstrDestinationApplId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInstanceIDFromPID )( 
            ICOMAdminCatalog2 __RPC_FAR * This,
            /* [in] */ DWORD dwPID,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrInstanceID);
        
        END_INTERFACE
    } ICOMAdminCatalog2Vtbl;

    interface ICOMAdminCatalog2
    {
        CONST_VTBL struct ICOMAdminCatalog2Vtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICOMAdminCatalog2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICOMAdminCatalog2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICOMAdminCatalog2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICOMAdminCatalog2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICOMAdminCatalog2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICOMAdminCatalog2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICOMAdminCatalog2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICOMAdminCatalog2_GetCollection(This,bstrCollName,ppCatalogCollection)	\
    (This)->lpVtbl -> GetCollection(This,bstrCollName,ppCatalogCollection)

#define ICOMAdminCatalog2_Connect(This,bstrConnectString,ppCatalogCollection)	\
    (This)->lpVtbl -> Connect(This,bstrConnectString,ppCatalogCollection)

#define ICOMAdminCatalog2_get_MajorVersion(This,retval)	\
    (This)->lpVtbl -> get_MajorVersion(This,retval)

#define ICOMAdminCatalog2_get_MinorVersion(This,retval)	\
    (This)->lpVtbl -> get_MinorVersion(This,retval)

#define ICOMAdminCatalog2_GetCollectionByQuery(This,bstrCollName,aQuery,ppCatalogCollection)	\
    (This)->lpVtbl -> GetCollectionByQuery(This,bstrCollName,aQuery,ppCatalogCollection)

#define ICOMAdminCatalog2_ImportComponent(This,bstrApplIdOrName,bstrCLSIDOrProgId)	\
    (This)->lpVtbl -> ImportComponent(This,bstrApplIdOrName,bstrCLSIDOrProgId)

#define ICOMAdminCatalog2_InstallComponent(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)	\
    (This)->lpVtbl -> InstallComponent(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)

#define ICOMAdminCatalog2_ShutdownApplication(This,bstrApplIdOrName)	\
    (This)->lpVtbl -> ShutdownApplication(This,bstrApplIdOrName)

#define ICOMAdminCatalog2_ExportApplication(This,bstrApplIdOrName,bstrApplicationFile,lOptions)	\
    (This)->lpVtbl -> ExportApplication(This,bstrApplIdOrName,bstrApplicationFile,lOptions)

#define ICOMAdminCatalog2_InstallApplication(This,bstrApplicationFile,bstrDestinationDirectory,lOptions,bstrUserId,bstrPassword,bstrRSN)	\
    (This)->lpVtbl -> InstallApplication(This,bstrApplicationFile,bstrDestinationDirectory,lOptions,bstrUserId,bstrPassword,bstrRSN)

#define ICOMAdminCatalog2_StopRouter(This)	\
    (This)->lpVtbl -> StopRouter(This)

#define ICOMAdminCatalog2_RefreshRouter(This)	\
    (This)->lpVtbl -> RefreshRouter(This)

#define ICOMAdminCatalog2_StartRouter(This)	\
    (This)->lpVtbl -> StartRouter(This)

#define ICOMAdminCatalog2_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define ICOMAdminCatalog2_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define ICOMAdminCatalog2_InstallMultipleComponents(This,bstrApplIdOrName,varFileNames,varCLSIDS)	\
    (This)->lpVtbl -> InstallMultipleComponents(This,bstrApplIdOrName,varFileNames,varCLSIDS)

#define ICOMAdminCatalog2_GetMultipleComponentsInfo(This,bstrApplIdOrName,varFileNames,varCLSIDs,varClassNames,varFileFlags,varComponentFlags)	\
    (This)->lpVtbl -> GetMultipleComponentsInfo(This,bstrApplIdOrName,varFileNames,varCLSIDs,varClassNames,varFileFlags,varComponentFlags)

#define ICOMAdminCatalog2_RefreshComponents(This)	\
    (This)->lpVtbl -> RefreshComponents(This)

#define ICOMAdminCatalog2_BackupREGDB(This,bstrBackupFilePath)	\
    (This)->lpVtbl -> BackupREGDB(This,bstrBackupFilePath)

#define ICOMAdminCatalog2_RestoreREGDB(This,bstrBackupFilePath)	\
    (This)->lpVtbl -> RestoreREGDB(This,bstrBackupFilePath)

#define ICOMAdminCatalog2_QueryApplicationFile(This,bstrApplicationFile,bstrApplicationName,bstrApplicationDescription,bHasUsers,bIsProxy,varFileNames)	\
    (This)->lpVtbl -> QueryApplicationFile(This,bstrApplicationFile,bstrApplicationName,bstrApplicationDescription,bHasUsers,bIsProxy,varFileNames)

#define ICOMAdminCatalog2_StartApplication(This,bstrApplIdOrName)	\
    (This)->lpVtbl -> StartApplication(This,bstrApplIdOrName)

#define ICOMAdminCatalog2_ServiceCheck(This,lService,pulStatus)	\
    (This)->lpVtbl -> ServiceCheck(This,lService,pulStatus)

#define ICOMAdminCatalog2_InstallMultipleEventClasses(This,bstrApplIdOrName,varFileNames,varCLSIDS)	\
    (This)->lpVtbl -> InstallMultipleEventClasses(This,bstrApplIdOrName,varFileNames,varCLSIDS)

#define ICOMAdminCatalog2_InstallEventClass(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)	\
    (This)->lpVtbl -> InstallEventClass(This,bstrApplIdOrName,bstrDLL,bstrTLB,bstrPSDLL)

#define ICOMAdminCatalog2_GetEventClassesForIID(This,bstrIID,varCLSIDs,varProgIDs,varDescriptions)	\
    (This)->lpVtbl -> GetEventClassesForIID(This,bstrIID,varCLSIDs,varProgIDs,varDescriptions)


#define ICOMAdminCatalog2_ShutdownProcess(This,bstrApplInstanceId)	\
    (This)->lpVtbl -> ShutdownProcess(This,bstrApplInstanceId)

#define ICOMAdminCatalog2_PauseProcess(This,bstrApplInstanceId)	\
    (This)->lpVtbl -> PauseProcess(This,bstrApplInstanceId)

#define ICOMAdminCatalog2_ResumeProcess(This,bstrApplInstanceId)	\
    (This)->lpVtbl -> ResumeProcess(This,bstrApplInstanceId)

#define ICOMAdminCatalog2_IsProcessPaused(This,bstrApplInstanceId,bPaused)	\
    (This)->lpVtbl -> IsProcessPaused(This,bstrApplInstanceId,bPaused)

#define ICOMAdminCatalog2_DumpProcess(This,bstrApplInstanceId,bstrDirectory,dwMaxImages,pbstrDumpFile)	\
    (This)->lpVtbl -> DumpProcess(This,bstrApplInstanceId,bstrDirectory,dwMaxImages,pbstrDumpFile)

#define ICOMAdminCatalog2_IsProcessDumpSupported(This)	\
    (This)->lpVtbl -> IsProcessDumpSupported(This)

#define ICOMAdminCatalog2_SetApplicationPartition(This,bstrApplPartitionIdOrName)	\
    (This)->lpVtbl -> SetApplicationPartition(This,bstrApplPartitionIdOrName)

#define ICOMAdminCatalog2_GetCurrentApplicationPartitionId(This,pbstrApplPartitionId)	\
    (This)->lpVtbl -> GetCurrentApplicationPartitionId(This,pbstrApplPartitionId)

#define ICOMAdminCatalog2_GetCurrentApplicationPartitionName(This,pbstrApplPartitionName)	\
    (This)->lpVtbl -> GetCurrentApplicationPartitionName(This,pbstrApplPartitionName)

#define ICOMAdminCatalog2_GetRunningApps(This,bstrApplIdOrName,ppRunningAppCollection)	\
    (This)->lpVtbl -> GetRunningApps(This,bstrApplIdOrName,ppRunningAppCollection)

#define ICOMAdminCatalog2_CreateServiceForApplication(This,bstrAppIdOrName,bstrServiceName,bstrStartType,bstrErrorControl,bstrDependencies,bstrRunAs,bstrPassword,bDesktopOk)	\
    (This)->lpVtbl -> CreateServiceForApplication(This,bstrAppIdOrName,bstrServiceName,bstrStartType,bstrErrorControl,bstrDependencies,bstrRunAs,bstrPassword,bDesktopOk)

#define ICOMAdminCatalog2_DeleteServiceForApplication(This,bstrAppIdOrName)	\
    (This)->lpVtbl -> DeleteServiceForApplication(This,bstrAppIdOrName)

#define ICOMAdminCatalog2_GetBaseApplicationPartitionId(This,pbstrBasePartitionId)	\
    (This)->lpVtbl -> GetBaseApplicationPartitionId(This,pbstrBasePartitionId)

#define ICOMAdminCatalog2_CopyApplications(This,bstrSourcePartitionId,varApplIds)	\
    (This)->lpVtbl -> CopyApplications(This,bstrSourcePartitionId,varApplIds)

#define ICOMAdminCatalog2_CopyComponent(This,bstrSourcePartitionId,bstrClsid,bstrNewApplId,bstrNewClsid,bstrNewProgId)	\
    (This)->lpVtbl -> CopyComponent(This,bstrSourcePartitionId,bstrClsid,bstrNewApplId,bstrNewClsid,bstrNewProgId)

#define ICOMAdminCatalog2_ImportComponentAsLegacy(This,bstrAppIdOrName,bstrCLSIDOrProgId,lComponentType)	\
    (This)->lpVtbl -> ImportComponentAsLegacy(This,bstrAppIdOrName,bstrCLSIDOrProgId,lComponentType)

#define ICOMAdminCatalog2_PromoteLegacyComponent(This,bstrAppIdOrName,bstrCLSIDOrProgId)	\
    (This)->lpVtbl -> PromoteLegacyComponent(This,bstrAppIdOrName,bstrCLSIDOrProgId)

#define ICOMAdminCatalog2_GetAppPartition(This,bstrAppId,pbstrPartitionId,pbstrPartitionName)	\
    (This)->lpVtbl -> GetAppPartition(This,bstrAppId,pbstrPartitionId,pbstrPartitionName)

#define ICOMAdminCatalog2_ImportComponentByType(This,bstrApplIdOrName,bstrCLSIDOrProgId,lComponentType)	\
    (This)->lpVtbl -> ImportComponentByType(This,bstrApplIdOrName,bstrCLSIDOrProgId,lComponentType)

#define ICOMAdminCatalog2_Is64BitCatalogServer(This,pbIs64Bit)	\
    (This)->lpVtbl -> Is64BitCatalogServer(This,pbIs64Bit)

#define ICOMAdminCatalog2_MoveComponent(This,bstrSourcePartitionId,bstrDestinationPartitionId,bstrClsid,bstrDestinationApplId)	\
    (This)->lpVtbl -> MoveComponent(This,bstrSourcePartitionId,bstrDestinationPartitionId,bstrClsid,bstrDestinationApplId)

#define ICOMAdminCatalog2_GetInstanceIDFromPID(This,dwPID,pbstrInstanceID)	\
    (This)->lpVtbl -> GetInstanceIDFromPID(This,dwPID,pbstrInstanceID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_ShutdownProcess_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplInstanceId);


void __RPC_STUB ICOMAdminCatalog2_ShutdownProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_PauseProcess_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplInstanceId);


void __RPC_STUB ICOMAdminCatalog2_PauseProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_ResumeProcess_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplInstanceId);


void __RPC_STUB ICOMAdminCatalog2_ResumeProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_IsProcessPaused_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplInstanceId,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *bPaused);


void __RPC_STUB ICOMAdminCatalog2_IsProcessPaused_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_DumpProcess_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplInstanceId,
    /* [in] */ BSTR bstrDirectory,
    /* [in] */ DWORD dwMaxImages,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDumpFile);


void __RPC_STUB ICOMAdminCatalog2_DumpProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_IsProcessDumpSupported_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This);


void __RPC_STUB ICOMAdminCatalog2_IsProcessDumpSupported_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_SetApplicationPartition_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplPartitionIdOrName);


void __RPC_STUB ICOMAdminCatalog2_SetApplicationPartition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_GetCurrentApplicationPartitionId_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrApplPartitionId);


void __RPC_STUB ICOMAdminCatalog2_GetCurrentApplicationPartitionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_GetCurrentApplicationPartitionName_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrApplPartitionName);


void __RPC_STUB ICOMAdminCatalog2_GetCurrentApplicationPartitionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_GetRunningApps_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppRunningAppCollection);


void __RPC_STUB ICOMAdminCatalog2_GetRunningApps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_CreateServiceForApplication_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrAppIdOrName,
    /* [in] */ BSTR bstrServiceName,
    /* [in] */ BSTR bstrStartType,
    /* [in] */ BSTR bstrErrorControl,
    /* [in] */ BSTR bstrDependencies,
    /* [in] */ BSTR bstrRunAs,
    /* [in] */ BSTR bstrPassword,
    /* [in] */ VARIANT_BOOL bDesktopOk);


void __RPC_STUB ICOMAdminCatalog2_CreateServiceForApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_DeleteServiceForApplication_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrAppIdOrName);


void __RPC_STUB ICOMAdminCatalog2_DeleteServiceForApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_GetBaseApplicationPartitionId_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrBasePartitionId);


void __RPC_STUB ICOMAdminCatalog2_GetBaseApplicationPartitionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_CopyApplications_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrSourcePartitionId,
    /* [in] */ SAFEARRAY __RPC_FAR * __RPC_FAR *varApplIds);


void __RPC_STUB ICOMAdminCatalog2_CopyApplications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_CopyComponent_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrSourcePartitionId,
    /* [in] */ BSTR bstrClsid,
    /* [in] */ BSTR bstrNewApplId,
    /* [in] */ BSTR bstrNewClsid,
    /* [in] */ BSTR bstrNewProgId);


void __RPC_STUB ICOMAdminCatalog2_CopyComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_ImportComponentAsLegacy_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrAppIdOrName,
    /* [in] */ BSTR bstrCLSIDOrProgId,
    /* [in] */ long lComponentType);


void __RPC_STUB ICOMAdminCatalog2_ImportComponentAsLegacy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_PromoteLegacyComponent_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrAppIdOrName,
    /* [in] */ BSTR bstrCLSIDOrProgId);


void __RPC_STUB ICOMAdminCatalog2_PromoteLegacyComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_GetAppPartition_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrAppId,
    /* [out] */ BSTR __RPC_FAR *pbstrPartitionId,
    /* [out] */ BSTR __RPC_FAR *pbstrPartitionName);


void __RPC_STUB ICOMAdminCatalog2_GetAppPartition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_ImportComponentByType_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrApplIdOrName,
    /* [in] */ BSTR bstrCLSIDOrProgId,
    /* [in] */ long lComponentType);


void __RPC_STUB ICOMAdminCatalog2_ImportComponentByType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_Is64BitCatalogServer_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [out] */ VARIANT_BOOL __RPC_FAR *pbIs64Bit);


void __RPC_STUB ICOMAdminCatalog2_Is64BitCatalogServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_MoveComponent_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ BSTR bstrSourcePartitionId,
    /* [in] */ BSTR bstrDestinationPartitionId,
    /* [in] */ BSTR bstrClsid,
    /* [in] */ BSTR bstrDestinationApplId);


void __RPC_STUB ICOMAdminCatalog2_MoveComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICOMAdminCatalog2_GetInstanceIDFromPID_Proxy( 
    ICOMAdminCatalog2 __RPC_FAR * This,
    /* [in] */ DWORD dwPID,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrInstanceID);


void __RPC_STUB ICOMAdminCatalog2_GetInstanceIDFromPID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICOMAdminCatalog2_INTERFACE_DEFINED__ */


#ifndef __ICatalogObject_INTERFACE_DEFINED__
#define __ICatalogObject_INTERFACE_DEFINED__

/* interface ICatalogObject */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICatalogObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6eb22871-8a19-11d0-81b6-00a0c9231c29")
    ICatalogObject : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR bstrPropName,
            /* [in] */ VARIANT val) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Key( 
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsPropertyReadOnly( 
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Valid( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsPropertyWriteOnly( 
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatalogObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICatalogObject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICatalogObject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICatalogObject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ BSTR bstrPropName,
            /* [in] */ VARIANT val);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Key )( 
            ICatalogObject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ICatalogObject __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsPropertyReadOnly )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Valid )( 
            ICatalogObject __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsPropertyWriteOnly )( 
            ICatalogObject __RPC_FAR * This,
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);
        
        END_INTERFACE
    } ICatalogObjectVtbl;

    interface ICatalogObject
    {
        CONST_VTBL struct ICatalogObjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatalogObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatalogObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatalogObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatalogObject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICatalogObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICatalogObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICatalogObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICatalogObject_get_Value(This,bstrPropName,retval)	\
    (This)->lpVtbl -> get_Value(This,bstrPropName,retval)

#define ICatalogObject_put_Value(This,bstrPropName,val)	\
    (This)->lpVtbl -> put_Value(This,bstrPropName,val)

#define ICatalogObject_get_Key(This,retval)	\
    (This)->lpVtbl -> get_Key(This,retval)

#define ICatalogObject_get_Name(This,retval)	\
    (This)->lpVtbl -> get_Name(This,retval)

#define ICatalogObject_IsPropertyReadOnly(This,bstrPropName,retval)	\
    (This)->lpVtbl -> IsPropertyReadOnly(This,bstrPropName,retval)

#define ICatalogObject_get_Valid(This,retval)	\
    (This)->lpVtbl -> get_Valid(This,retval)

#define ICatalogObject_IsPropertyWriteOnly(This,bstrPropName,retval)	\
    (This)->lpVtbl -> IsPropertyWriteOnly(This,bstrPropName,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogObject_get_Value_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [in] */ BSTR bstrPropName,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB ICatalogObject_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ICatalogObject_put_Value_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [in] */ BSTR bstrPropName,
    /* [in] */ VARIANT val);


void __RPC_STUB ICatalogObject_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogObject_get_Key_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB ICatalogObject_get_Key_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogObject_get_Name_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB ICatalogObject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogObject_IsPropertyReadOnly_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [in] */ BSTR bstrPropName,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);


void __RPC_STUB ICatalogObject_IsPropertyReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogObject_get_Valid_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);


void __RPC_STUB ICatalogObject_get_Valid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogObject_IsPropertyWriteOnly_Proxy( 
    ICatalogObject __RPC_FAR * This,
    /* [in] */ BSTR bstrPropName,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);


void __RPC_STUB ICatalogObject_IsPropertyWriteOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatalogObject_INTERFACE_DEFINED__ */


#ifndef __ICatalogCollection_INTERFACE_DEFINED__
#define __ICatalogCollection_INTERFACE_DEFINED__

/* interface ICatalogCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICatalogCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6eb22872-8a19-11d0-81b6-00a0c9231c29")
    ICatalogCollection : public IDispatch
    {
    public:
        virtual /* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVariant) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long lIndex,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogObject) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ long lIndex) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogObject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Populate( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveChanges( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCollection( 
            /* [in] */ BSTR bstrCollName,
            /* [in] */ VARIANT varObjectKey,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AddEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoveEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetUtilInterface( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUtil) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DataStoreMajorVersion( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DataStoreMinorVersion( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PopulateByKey( 
            /* [in] */ SAFEARRAY __RPC_FAR * aKeys) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PopulateByQuery( 
            /* [in] */ BSTR bstrQueryString,
            /* [in] */ long lQueryType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatalogCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICatalogCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICatalogCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICatalogCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVariant);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ long lIndex,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogObject);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ long lIndex);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogObject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Populate )( 
            ICatalogCollection __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveChanges )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCollection )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ BSTR bstrCollName,
            /* [in] */ VARIANT varObjectKey,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AddEnabled )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RemoveEnabled )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUtilInterface )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUtil);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataStoreMajorVersion )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataStoreMinorVersion )( 
            ICatalogCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PopulateByKey )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ SAFEARRAY __RPC_FAR * aKeys);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PopulateByQuery )( 
            ICatalogCollection __RPC_FAR * This,
            /* [in] */ BSTR bstrQueryString,
            /* [in] */ long lQueryType);
        
        END_INTERFACE
    } ICatalogCollectionVtbl;

    interface ICatalogCollection
    {
        CONST_VTBL struct ICatalogCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatalogCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatalogCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatalogCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatalogCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICatalogCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICatalogCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICatalogCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICatalogCollection_get__NewEnum(This,ppEnumVariant)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnumVariant)

#define ICatalogCollection_get_Item(This,lIndex,ppCatalogObject)	\
    (This)->lpVtbl -> get_Item(This,lIndex,ppCatalogObject)

#define ICatalogCollection_get_Count(This,retval)	\
    (This)->lpVtbl -> get_Count(This,retval)

#define ICatalogCollection_Remove(This,lIndex)	\
    (This)->lpVtbl -> Remove(This,lIndex)

#define ICatalogCollection_Add(This,ppCatalogObject)	\
    (This)->lpVtbl -> Add(This,ppCatalogObject)

#define ICatalogCollection_Populate(This)	\
    (This)->lpVtbl -> Populate(This)

#define ICatalogCollection_SaveChanges(This,retval)	\
    (This)->lpVtbl -> SaveChanges(This,retval)

#define ICatalogCollection_GetCollection(This,bstrCollName,varObjectKey,ppCatalogCollection)	\
    (This)->lpVtbl -> GetCollection(This,bstrCollName,varObjectKey,ppCatalogCollection)

#define ICatalogCollection_get_Name(This,retval)	\
    (This)->lpVtbl -> get_Name(This,retval)

#define ICatalogCollection_get_AddEnabled(This,retval)	\
    (This)->lpVtbl -> get_AddEnabled(This,retval)

#define ICatalogCollection_get_RemoveEnabled(This,retval)	\
    (This)->lpVtbl -> get_RemoveEnabled(This,retval)

#define ICatalogCollection_GetUtilInterface(This,ppUtil)	\
    (This)->lpVtbl -> GetUtilInterface(This,ppUtil)

#define ICatalogCollection_get_DataStoreMajorVersion(This,retval)	\
    (This)->lpVtbl -> get_DataStoreMajorVersion(This,retval)

#define ICatalogCollection_get_DataStoreMinorVersion(This,retval)	\
    (This)->lpVtbl -> get_DataStoreMinorVersion(This,retval)

#define ICatalogCollection_PopulateByKey(This,aKeys)	\
    (This)->lpVtbl -> PopulateByKey(This,aKeys)

#define ICatalogCollection_PopulateByQuery(This,bstrQueryString,lQueryType)	\
    (This)->lpVtbl -> PopulateByQuery(This,bstrQueryString,lQueryType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][restricted][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get__NewEnum_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVariant);


void __RPC_STUB ICatalogCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_Item_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [in] */ long lIndex,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogObject);


void __RPC_STUB ICatalogCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_Count_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_Remove_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [in] */ long lIndex);


void __RPC_STUB ICatalogCollection_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_Add_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogObject);


void __RPC_STUB ICatalogCollection_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_Populate_Proxy( 
    ICatalogCollection __RPC_FAR * This);


void __RPC_STUB ICatalogCollection_Populate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_SaveChanges_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_SaveChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_GetCollection_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [in] */ BSTR bstrCollName,
    /* [in] */ VARIANT varObjectKey,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppCatalogCollection);


void __RPC_STUB ICatalogCollection_GetCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_Name_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_AddEnabled_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_get_AddEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_RemoveEnabled_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_get_RemoveEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_GetUtilInterface_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppUtil);


void __RPC_STUB ICatalogCollection_GetUtilInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_DataStoreMajorVersion_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_get_DataStoreMajorVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_get_DataStoreMinorVersion_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ICatalogCollection_get_DataStoreMinorVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_PopulateByKey_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [in] */ SAFEARRAY __RPC_FAR * aKeys);


void __RPC_STUB ICatalogCollection_PopulateByKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ICatalogCollection_PopulateByQuery_Proxy( 
    ICatalogCollection __RPC_FAR * This,
    /* [in] */ BSTR bstrQueryString,
    /* [in] */ long lQueryType);


void __RPC_STUB ICatalogCollection_PopulateByQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatalogCollection_INTERFACE_DEFINED__ */



#ifndef __COMAdmin_LIBRARY_DEFINED__
#define __COMAdmin_LIBRARY_DEFINED__

/* library COMAdmin */
/* [helpstring][version][uuid] */ 



typedef /* [helpstring] */ 
enum COMAdminComponentType
    {	COMAdminUseAnyComponent	= 0x1,
	COMAdminWOWComponent	= 0x2,
	COMAdminNativeComponent	= 0x3
    } 	COMAdminComponentType;

typedef /* [helpstring] */ 
enum COMAdminApplicationInstallOptions
    {	COMAdminInstallNoUsers	= 0,
	COMAdminInstallUsers	= 1,
	COMAdminInstallForceOverwriteOfFiles	= 2
    } 	COMAdminApplicationInstallOptions;

typedef /* [helpstring] */ 
enum COMAdminApplicationExportOptions
    {	COMAdminExportNoUsers	= 0,
	COMAdminExportUsers	= 1,
	COMAdminExportApplicationProxy	= 2,
	COMAdminExportForceOverwriteOfFiles	= 4,
	COMAdminExportIn10Format	= 16
    } 	COMAdminApplicationExportOptions;

typedef /* [helpstring] */ 
enum COMAdminThreadingModels
    {	COMAdminThreadingModelApartment	= 0,
	COMAdminThreadingModelFree	= 1,
	COMAdminThreadingModelMain	= 2,
	COMAdminThreadingModelBoth	= 3,
	COMAdminThreadingModelNeutral	= 4,
	COMAdminThreadingModelNotSpecified	= 5
    } 	COMAdminThreadingModels;

typedef /* [helpstring] */ 
enum COMAdminTransactionOptions
    {	COMAdminTransactionIgnored	= 0,
	COMAdminTransactionNone	= COMAdminTransactionIgnored + 1,
	COMAdminTransactionSupported	= COMAdminTransactionNone + 1,
	COMAdminTransactionRequired	= COMAdminTransactionSupported + 1,
	COMAdminTransactionRequiresNew	= COMAdminTransactionRequired + 1
    } 	COMAdminTransactionOptions;

typedef /* [helpstring] */ 
enum COMAdminTxIsolationLevelOptions
    {	COMAdminTxIsolationLevelNone	= 0,
	COMAdminTxIsolationLevelReadUnCommitted	= COMAdminTxIsolationLevelNone + 1,
	COMAdminTxIsolationLevelReadCommitted	= COMAdminTxIsolationLevelReadUnCommitted + 1,
	COMAdminTxIsolationLevelRepeatableRead	= COMAdminTxIsolationLevelReadCommitted + 1,
	COMAdminTxIsolationLevelSerializable	= COMAdminTxIsolationLevelRepeatableRead + 1
    } 	COMAdminTxIsolationLevelOptions;

typedef /* [helpstring] */ 
enum COMAdminSynchronizationOptions
    {	COMAdminSynchronizationIgnored	= 0,
	COMAdminSynchronizationNone	= COMAdminSynchronizationIgnored + 1,
	COMAdminSynchronizationSupported	= COMAdminSynchronizationNone + 1,
	COMAdminSynchronizationRequired	= COMAdminSynchronizationSupported + 1,
	COMAdminSynchronizationRequiresNew	= COMAdminSynchronizationRequired + 1
    } 	COMAdminSynchronizationOptions;

typedef /* [helpstring] */ 
enum COMAdminActivationOptions
    {	COMAdminActivationInproc	= 0,
	COMAdminActivationLocal	= COMAdminActivationInproc + 1
    } 	COMAdminActivationOptions;

typedef /* [helpstring] */ 
enum COMAdminAccessChecksLevelOptions
    {	COMAdminAccessChecksApplicationLevel	= 0,
	COMAdminAccessChecksApplicationComponentLevel	= 1
    } 	COMAdminAccessChecksLevelOptions;

typedef /* [helpstring] */ 
enum COMAdminAuthenticationLevelOptions
    {	COMAdminAuthenticationDefault	= 0,
	COMAdminAuthenticationNone	= COMAdminAuthenticationDefault + 1,
	COMAdminAuthenticationConnect	= COMAdminAuthenticationNone + 1,
	COMAdminAuthenticationCall	= COMAdminAuthenticationConnect + 1,
	COMAdminAuthenticationPacket	= COMAdminAuthenticationCall + 1,
	COMAdminAuthenticationIntegrity	= COMAdminAuthenticationPacket + 1,
	COMAdminAuthenticationPrivacy	= COMAdminAuthenticationIntegrity + 1
    } 	COMAdminAuthenticationLevelOptions;

typedef /* [helpstring] */ 
enum COMAdminImpersonationLevelOptions
    {	COMAdminImpersonationAnonymous	= 1,
	COMAdminImpersonationIdentify	= COMAdminImpersonationAnonymous + 1,
	COMAdminImpersonationImpersonate	= COMAdminImpersonationIdentify + 1,
	COMAdminImpersonationDelegate	= COMAdminImpersonationImpersonate + 1
    } 	COMAdminImpersonationLevelOptions;

typedef /* [helpstring] */ 
enum COMAdminAuthenticationCapabilitiesOptions
    {	COMAdminAuthenticationCapabilitiesNone	= 0,
	COMAdminAuthenticationCapabilitiesStaticCloaking	= 0x20,
	COMAdminAuthenticationCapabilitiesDynamicCloaking	= 0x40,
	COMAdminAuthenticationCapabilitiesSecureReference	= 0x2
    } 	COMAdminAuthenticationCapabilitiesOptions;

typedef /* [helpstring] */ 
enum COMAdminOS
    {	COMAdminOSWindows3_1	= 1,
	COMAdminOSWindows9x	= 2,
	COMAdminOSWindowsNT	= 3,
	COMAdminOSWindowsNTEnterprise	= 4
    } 	COMAdminOS;

typedef /* [helpstring] */ 
enum COMAdminServiceOptions
    {	COMAdminServiceLoadBalanceRouter	= 1
    } 	COMAdminServiceOptions;

typedef /* [helpstring] */ 
enum COMAdminServiceStatusOptions
    {	COMAdminServiceStopped	= 0,
	COMAdminServiceStartPending	= COMAdminServiceStopped + 1,
	COMAdminServiceStopPending	= COMAdminServiceStartPending + 1,
	COMAdminServiceRunning	= COMAdminServiceStopPending + 1,
	COMAdminServiceContinuePending	= COMAdminServiceRunning + 1,
	COMAdminServicePausePending	= COMAdminServiceContinuePending + 1,
	COMAdminServicePaused	= COMAdminServicePausePending + 1,
	COMAdminServiceUnknownState	= COMAdminServicePaused + 1
    } 	COMAdminServiceStatusOptions;

typedef /* [helpstring] */ 
enum COMAdminFileFlags
    {	COMAdminFileFlagLoadable	= 0x1,
	COMAdminFileFlagCOM	= 0x2,
	COMAdminFileFlagContainsPS	= 0x4,
	COMAdminFileFlagContainsComp	= 0x8,
	COMAdminFileFlagContainsTLB	= 0x10,
	COMAdminFileFlagSelfReg	= 0x20,
	COMAdminFileFlagSelfUnReg	= 0x40,
	COMAdminFileFlagUnloadableDLL	= 0x80,
	COMAdminFileFlagDoesNotExist	= 0x100,
	COMAdminFileFlagAlreadyInstalled	= 0x200,
	COMAdminFileFlagBadTLB	= 0x400,
	COMAdminFileFlagGetClassObjFailed	= 0x800,
	COMAdminFileFlagClassNotAvailable	= 0x1000,
	COMAdminFileFlagRegistrar	= 0x2000,
	COMAdminFileFlagNoRegistrar	= 0x4000,
	COMAdminFileFlagDLLRegsvrFailed	= 0x8000,
	COMAdminFileFlagRegTLBFailed	= 0x10000,
	COMAdminFileFlagRegistrarFailed	= 0x20000,
	COMAdminFileFlagError	= 0x40000
    } 	COMAdminFileFlags;

typedef /* [helpstring] */ 
enum COMAdminComponentFlags
    {	COMAdminCompFlagTypeInfoFound	= 0x1,
	COMAdminCompFlagCOMPlusPropertiesFound	= 0x2,
	COMAdminCompFlagProxyFound	= 0x4,
	COMAdminCompFlagInterfacesFound	= 0x8,
	COMAdminCompFlagAlreadyInstalled	= 0x10,
	COMAdminCompFlagNotInApplication	= 0x20
    } 	COMAdminComponentFlags;

#define	COMAdminCollectionRoot	( "Root" )

#define	COMAdminCollectionApplications	( "Applications" )

#define	COMAdminCollectionComponents	( "Components" )

#define	COMAdminCollectionComputerList	( "ComputerList" )

#define	COMAdminCollectionApplicationCluster	( "ApplicationCluster" )

#define	COMAdminCollectionLocalComputer	( "LocalComputer" )

#define	COMAdminCollectionInprocServers	( "InprocServers" )

#define	COMAdminCollectionRelatedCollectionInfo	( "RelatedCollectionInfo" )

#define	COMAdminCollectionPropertyInfo	( "PropertyInfo" )

#define	COMAdminCollectionRoles	( "Roles" )

#define	COMAdminCollectionErrorInfo	( "ErrorInfo" )

#define	COMAdminCollectionInterfacesForComponent	( "InterfacesForComponent" )

#define	COMAdminCollectionRolesForComponent	( "RolesForComponent" )

#define	COMAdminCollectionMethodsForInterface	( "MethodsForInterface" )

#define	COMAdminCollectionRolesForInterface	( "RolesForInterface" )

#define	COMAdminCollectionRolesForMethod	( "RolesForMethod" )

#define	COMAdminCollectionUsersInRole	( "UsersInRole" )

#define	COMAdminCollectionDCOMProtocols	( "DCOMProtocols" )

#define	COMAdminCollectionApplicationPartitions	( "ApplicationPartitions" )

typedef /* [helpstring] */ 
enum COMAdminErrorCodes
    {	COMAdminErrObjectErrors	= ( HRESULT  )0x80110401L,
	COMAdminErrObjectInvalid	= ( HRESULT  )0x80110402L,
	COMAdminErrKeyMissing	= ( HRESULT  )0x80110403L,
	COMAdminErrAlreadyInstalled	= ( HRESULT  )0x80110404L,
	COMAdminErrAppFileWriteFail	= ( HRESULT  )0x80110407L,
	COMAdminErrAppFileReadFail	= ( HRESULT  )0x80110408L,
	COMAdminErrAppFileVersion	= ( HRESULT  )0x80110409L,
	COMAdminErrBadPath	= ( HRESULT  )0x8011040aL,
	COMAdminErrApplicationExists	= ( HRESULT  )0x8011040bL,
	COMAdminErrRoleExists	= ( HRESULT  )0x8011040cL,
	COMAdminErrCantCopyFile	= ( HRESULT  )0x8011040dL,
	COMAdminErrNoUser	= ( HRESULT  )0x8011040fL,
	COMAdminErrInvalidUserids	= ( HRESULT  )0x80110410L,
	COMAdminErrNoRegistryCLSID	= ( HRESULT  )0x80110411L,
	COMAdminErrBadRegistryProgID	= ( HRESULT  )0x80110412L,
	COMAdminErrAuthenticationLevel	= ( HRESULT  )0x80110413L,
	COMAdminErrUserPasswdNotValid	= ( HRESULT  )0x80110414L,
	COMAdminErrCLSIDOrIIDMismatch	= ( HRESULT  )0x80110418L,
	COMAdminErrRemoteInterface	= ( HRESULT  )0x80110419L,
	COMAdminErrDllRegisterServer	= ( HRESULT  )0x8011041aL,
	COMAdminErrNoServerShare	= ( HRESULT  )0x8011041bL,
	COMAdminErrDllLoadFailed	= ( HRESULT  )0x8011041dL,
	COMAdminErrBadRegistryLibID	= ( HRESULT  )0x8011041eL,
	COMAdminErrAppDirNotFound	= ( HRESULT  )0x8011041fL,
	COMAdminErrRegistrarFailed	= ( HRESULT  )0x80110423L,
	COMAdminErrCompFileDoesNotExist	= ( HRESULT  )0x80110424L,
	COMAdminErrCompFileLoadDLLFail	= ( HRESULT  )0x80110425L,
	COMAdminErrCompFileGetClassObj	= ( HRESULT  )0x80110426L,
	COMAdminErrCompFileClassNotAvail	= ( HRESULT  )0x80110427L,
	COMAdminErrCompFileBadTLB	= ( HRESULT  )0x80110428L,
	COMAdminErrCompFileNotInstallable	= ( HRESULT  )0x80110429L,
	COMAdminErrNotChangeable	= ( HRESULT  )0x8011042aL,
	COMAdminErrNotDeletable	= ( HRESULT  )0x8011042bL,
	COMAdminErrSession	= ( HRESULT  )0x8011042cL,
	COMAdminErrCompMoveLocked	= ( HRESULT  )0x8011042dL,
	COMAdminErrCompMoveBadDest	= ( HRESULT  )0x8011042eL,
	COMAdminErrRegisterTLB	= ( HRESULT  )0x80110430L,
	COMAdminErrSystemApp	= ( HRESULT  )0x80110433L,
	COMAdminErrCompFileNoRegistrar	= ( HRESULT  )0x80110434L,
	COMAdminErrCoReqCompInstalled	= ( HRESULT  )0x80110435L,
	COMAdminErrServiceNotInstalled	= ( HRESULT  )0x80110436L,
	COMAdminErrPropertySaveFailed	= ( HRESULT  )0x80110437L,
	COMAdminErrObjectExists	= ( HRESULT  )0x80110438L,
	COMAdminErrRegFileCorrupt	= ( HRESULT  )0x8011043bL,
	COMAdminErrPropertyOverflow	= ( HRESULT  )0x8011043cL,
	COMAdminErrNotInRegistry	= ( HRESULT  )0x8011043eL,
	COMAdminErrObjectNotPoolable	= ( HRESULT  )0x8011043fL,
	COMAdminErrApplidMatchesClsid	= ( HRESULT  )0x80110446L,
	COMAdminErrRoleDoesNotExist	= ( HRESULT  )0x80110447L,
	COMAdminErrStartAppNeedsComponents	= ( HRESULT  )0x80110448L,
	COMAdminErrRequiresDifferentPlatform	= ( HRESULT  )0x80110449L,
	COMAdminErrQueuingServiceNotAvailable	= ( HRESULT  )0x80110602L,
	COMAdminErrObjectParentMissing	= ( HRESULT  )0x80110808L,
	COMAdminErrObjectDoesNotExist	= ( HRESULT  )0x80110809L,
	COMAdminErrCanNotExportAppProxy	= ( HRESULT  )0x8011044aL,
	COMAdminErrCanNotStartApp	= ( HRESULT  )0x8011044bL,
	COMAdminErrCanNotExportSystemApp	= ( HRESULT  )0x8011044cL,
	COMAdminErrCanNotSubscribeToComponent	= ( HRESULT  )0x8011044dL,
	COMAdminErrAppNotRunning	= ( HRESULT  )0x8011080aL,
	COMAdminSomeAlreadyPaused	= ( HRESULT  )0x11080bL,
	COMAdminSomeAlreadyRunning	= ( HRESULT  )0x11080cL
    } 	;


EXTERN_C const IID LIBID_COMAdmin;

EXTERN_C const CLSID CLSID_COMAdminCatalog;

#ifdef __cplusplus

class DECLSPEC_UUID("F618C514-DFB8-11d1-A2CF-00805FC79235")
COMAdminCatalog;
#endif

EXTERN_C const CLSID CLSID_COMAdminCatalogObject;

#ifdef __cplusplus

class DECLSPEC_UUID("F618C515-DFB8-11d1-A2CF-00805FC79235")
COMAdminCatalogObject;
#endif

EXTERN_C const CLSID CLSID_COMAdminCatalogCollection;

#ifdef __cplusplus

class DECLSPEC_UUID("F618C516-DFB8-11d1-A2CF-00805FC79235")
COMAdminCatalogCollection;
#endif
#endif /* __COMAdmin_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long __RPC_FAR *, unsigned long            , LPSAFEARRAY __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long __RPC_FAR *, LPSAFEARRAY __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


