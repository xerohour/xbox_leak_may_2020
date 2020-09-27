
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Thu Apr 20 21:17:15 2000
 */
/* Compiler settings for wiascr.idl:
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

#ifndef __wiascr_h__
#define __wiascr_h__

/* Forward Declarations */ 

#ifndef __ISample_FWD_DEFINED__
#define __ISample_FWD_DEFINED__
typedef interface ISample ISample;
#endif 	/* __ISample_FWD_DEFINED__ */


#ifndef __ICollection_FWD_DEFINED__
#define __ICollection_FWD_DEFINED__
typedef interface ICollection ICollection;
#endif 	/* __ICollection_FWD_DEFINED__ */


#ifndef __IWia_FWD_DEFINED__
#define __IWia_FWD_DEFINED__
typedef interface IWia IWia;
#endif 	/* __IWia_FWD_DEFINED__ */


#ifndef ___IWiaEvents_FWD_DEFINED__
#define ___IWiaEvents_FWD_DEFINED__
typedef interface _IWiaEvents _IWiaEvents;
#endif 	/* ___IWiaEvents_FWD_DEFINED__ */


#ifndef __IWiaDeviceInfo_FWD_DEFINED__
#define __IWiaDeviceInfo_FWD_DEFINED__
typedef interface IWiaDeviceInfo IWiaDeviceInfo;
#endif 	/* __IWiaDeviceInfo_FWD_DEFINED__ */


#ifndef __IWiaDispatchItem_FWD_DEFINED__
#define __IWiaDispatchItem_FWD_DEFINED__
typedef interface IWiaDispatchItem IWiaDispatchItem;
#endif 	/* __IWiaDispatchItem_FWD_DEFINED__ */


#ifndef __Sample_FWD_DEFINED__
#define __Sample_FWD_DEFINED__

#ifdef __cplusplus
typedef class Sample Sample;
#else
typedef struct Sample Sample;
#endif /* __cplusplus */

#endif 	/* __Sample_FWD_DEFINED__ */


#ifndef __Wia_FWD_DEFINED__
#define __Wia_FWD_DEFINED__

#ifdef __cplusplus
typedef class Wia Wia;
#else
typedef struct Wia Wia;
#endif /* __cplusplus */

#endif 	/* __Wia_FWD_DEFINED__ */


#ifndef __WiaProtocol_FWD_DEFINED__
#define __WiaProtocol_FWD_DEFINED__

#ifdef __cplusplus
typedef class WiaProtocol WiaProtocol;
#else
typedef struct WiaProtocol WiaProtocol;
#endif /* __cplusplus */

#endif 	/* __WiaProtocol_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __NDMEDIALib_LIBRARY_DEFINED__
#define __NDMEDIALib_LIBRARY_DEFINED__

/* library NDMEDIALib */
/* [helpstring][version][uuid] */ 

EXTERN_C const GUID DIID__IWiaEvents;

EXTERN_C const IID LIBID_NDMEDIALib;

#ifndef __ISample_INTERFACE_DEFINED__
#define __ISample_INTERFACE_DEFINED__

/* interface ISample */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISample;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C7A4EFC9-017E-45AB-BEF0-9A8218BC1D40")
    ISample : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SampleProperty( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSample) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SampleProperty( 
            /* [in] */ BSTR bstrSample) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SampleMethod( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISampleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISample __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISample __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISample __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISample __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISample __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISample __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISample __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SampleProperty )( 
            ISample __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSample);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SampleProperty )( 
            ISample __RPC_FAR * This,
            /* [in] */ BSTR bstrSample);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SampleMethod )( 
            ISample __RPC_FAR * This);
        
        END_INTERFACE
    } ISampleVtbl;

    interface ISample
    {
        CONST_VTBL struct ISampleVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISample_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISample_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISample_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISample_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISample_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISample_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISample_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISample_get_SampleProperty(This,pbstrSample)	\
    (This)->lpVtbl -> get_SampleProperty(This,pbstrSample)

#define ISample_put_SampleProperty(This,bstrSample)	\
    (This)->lpVtbl -> put_SampleProperty(This,bstrSample)

#define ISample_SampleMethod(This)	\
    (This)->lpVtbl -> SampleMethod(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISample_get_SampleProperty_Proxy( 
    ISample __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSample);


void __RPC_STUB ISample_get_SampleProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE ISample_put_SampleProperty_Proxy( 
    ISample __RPC_FAR * This,
    /* [in] */ BSTR bstrSample);


void __RPC_STUB ISample_put_SampleProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISample_SampleMethod_Proxy( 
    ISample __RPC_FAR * This);


void __RPC_STUB ISample_SampleMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISample_INTERFACE_DEFINED__ */


#ifndef __ICollection_INTERFACE_DEFINED__
#define __ICollection_INTERFACE_DEFINED__

/* interface ICollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ICollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C34C8CE7-B253-4f89-AA25-8A24AD71D0C0")
    ICollection : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Length( 
            /* [retval][out] */ unsigned long __RPC_FAR *plLength) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ unsigned long lItem,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ICollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Length )( 
            ICollection __RPC_FAR * This,
            /* [retval][out] */ unsigned long __RPC_FAR *plLength);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ICollection __RPC_FAR * This,
            /* [in] */ unsigned long lItem,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            ICollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);
        
        END_INTERFACE
    } ICollectionVtbl;

    interface ICollection
    {
        CONST_VTBL struct ICollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICollection_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define ICollection_get_Length(This,plLength)	\
    (This)->lpVtbl -> get_Length(This,plLength)

#define ICollection_get_Item(This,lItem,ppDispItem)	\
    (This)->lpVtbl -> get_Item(This,lItem,ppDispItem)

#define ICollection_get__NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ICollection_get_Count_Proxy( 
    ICollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB ICollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICollection_get_Length_Proxy( 
    ICollection __RPC_FAR * This,
    /* [retval][out] */ unsigned long __RPC_FAR *plLength);


void __RPC_STUB ICollection_get_Length_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICollection_get_Item_Proxy( 
    ICollection __RPC_FAR * This,
    /* [in] */ unsigned long lItem,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispItem);


void __RPC_STUB ICollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ICollection_get__NewEnum_Proxy( 
    ICollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB ICollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICollection_INTERFACE_DEFINED__ */


#ifndef __IWia_INTERFACE_DEFINED__
#define __IWia_INTERFACE_DEFINED__

/* interface IWia */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWia;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B10BA1BC-3713-4EC0-8EEA-690EBD2CED8A")
    IWia : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _DebugDialog( 
            BOOL fWait) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Devices( 
            /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCol) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [unique][in] */ VARIANT __RPC_FAR *pvaConnect,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDevice) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWiaVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWia __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWia __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWia __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWia __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWia __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWia __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWia __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_DebugDialog )( 
            IWia __RPC_FAR * This,
            BOOL fWait);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Devices )( 
            IWia __RPC_FAR * This,
            /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCol);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            IWia __RPC_FAR * This,
            /* [unique][in] */ VARIANT __RPC_FAR *pvaConnect,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDevice);
        
        END_INTERFACE
    } IWiaVtbl;

    interface IWia
    {
        CONST_VTBL struct IWiaVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWia_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWia_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWia_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWia_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWia_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWia_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWia_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWia__DebugDialog(This,fWait)	\
    (This)->lpVtbl -> _DebugDialog(This,fWait)

#define IWia_get_Devices(This,ppCol)	\
    (This)->lpVtbl -> get_Devices(This,ppCol)

#define IWia_Create(This,pvaConnect,ppDevice)	\
    (This)->lpVtbl -> Create(This,pvaConnect,ppDevice)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWia__DebugDialog_Proxy( 
    IWia __RPC_FAR * This,
    BOOL fWait);


void __RPC_STUB IWia__DebugDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWia_get_Devices_Proxy( 
    IWia __RPC_FAR * This,
    /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCol);


void __RPC_STUB IWia_get_Devices_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWia_Create_Proxy( 
    IWia __RPC_FAR * This,
    /* [unique][in] */ VARIANT __RPC_FAR *pvaConnect,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDevice);


void __RPC_STUB IWia_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWia_INTERFACE_DEFINED__ */


#ifndef ___IWiaEvents_DISPINTERFACE_DEFINED__
#define ___IWiaEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IWiaEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IWiaEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("E5F04D72-6C16-42e2-BCCA-F8D0DB4ADE06")
    _IWiaEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IWiaEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            _IWiaEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            _IWiaEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            _IWiaEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            _IWiaEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            _IWiaEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            _IWiaEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            _IWiaEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } _IWiaEventsVtbl;

    interface _IWiaEvents
    {
        CONST_VTBL struct _IWiaEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IWiaEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IWiaEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IWiaEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IWiaEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IWiaEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IWiaEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IWiaEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IWiaEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IWiaDeviceInfo_INTERFACE_DEFINED__
#define __IWiaDeviceInfo_INTERFACE_DEFINED__

/* interface IWiaDeviceInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWiaDeviceInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5267FF5E-7CAF-4769-865D-17A25968525E")
    IWiaDeviceInfo : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDeviceId) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Port( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPort) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UIClsid( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidUI) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Manufacturer( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrVendor) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Create( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDevice) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPropById( 
            /* [in] */ DWORD propid,
            /* [retval][out] */ VARIANT __RPC_FAR *pvaOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWiaDeviceInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWiaDeviceInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWiaDeviceInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Id )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDeviceId);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Port )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPort);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UIClsid )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidUI);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Manufacturer )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrVendor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDevice);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPropById )( 
            IWiaDeviceInfo __RPC_FAR * This,
            /* [in] */ DWORD propid,
            /* [retval][out] */ VARIANT __RPC_FAR *pvaOut);
        
        END_INTERFACE
    } IWiaDeviceInfoVtbl;

    interface IWiaDeviceInfo
    {
        CONST_VTBL struct IWiaDeviceInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWiaDeviceInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWiaDeviceInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWiaDeviceInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWiaDeviceInfo_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWiaDeviceInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWiaDeviceInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWiaDeviceInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWiaDeviceInfo_get_Id(This,pbstrDeviceId)	\
    (This)->lpVtbl -> get_Id(This,pbstrDeviceId)

#define IWiaDeviceInfo_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IWiaDeviceInfo_get_Type(This,pbstrType)	\
    (This)->lpVtbl -> get_Type(This,pbstrType)

#define IWiaDeviceInfo_get_Port(This,pbstrPort)	\
    (This)->lpVtbl -> get_Port(This,pbstrPort)

#define IWiaDeviceInfo_get_UIClsid(This,pbstrGuidUI)	\
    (This)->lpVtbl -> get_UIClsid(This,pbstrGuidUI)

#define IWiaDeviceInfo_get_Manufacturer(This,pbstrVendor)	\
    (This)->lpVtbl -> get_Manufacturer(This,pbstrVendor)

#define IWiaDeviceInfo_Create(This,ppDevice)	\
    (This)->lpVtbl -> Create(This,ppDevice)

#define IWiaDeviceInfo_GetPropById(This,propid,pvaOut)	\
    (This)->lpVtbl -> GetPropById(This,propid,pvaOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_get_Id_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDeviceId);


void __RPC_STUB IWiaDeviceInfo_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_get_Name_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IWiaDeviceInfo_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_get_Type_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrType);


void __RPC_STUB IWiaDeviceInfo_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_get_Port_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPort);


void __RPC_STUB IWiaDeviceInfo_get_Port_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_get_UIClsid_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrGuidUI);


void __RPC_STUB IWiaDeviceInfo_get_UIClsid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_get_Manufacturer_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrVendor);


void __RPC_STUB IWiaDeviceInfo_get_Manufacturer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_Create_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDevice);


void __RPC_STUB IWiaDeviceInfo_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDeviceInfo_GetPropById_Proxy( 
    IWiaDeviceInfo __RPC_FAR * This,
    /* [in] */ DWORD propid,
    /* [retval][out] */ VARIANT __RPC_FAR *pvaOut);


void __RPC_STUB IWiaDeviceInfo_GetPropById_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWiaDeviceInfo_INTERFACE_DEFINED__ */


#ifndef __IWiaDispatchItem_INTERFACE_DEFINED__
#define __IWiaDispatchItem_INTERFACE_DEFINED__

/* interface IWiaDispatchItem */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWiaDispatchItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D06D503F-4B71-40f3-94A7-66478F732BC9")
    IWiaDispatchItem : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetItemsFromUI( 
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwIntent,
            /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCollection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPropById( 
            /* [in] */ DWORD propid,
            /* [retval][out] */ VARIANT __RPC_FAR *pvaOut) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Transfer( 
            /* [in] */ BSTR pbstrFilename,
            BOOL bAsyncTransfer) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Children( 
            /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCollection) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ItemType( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectStatus( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrStatus) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Time( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTime) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FirmwareVersion( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrVersion) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFullName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwWidth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwHeight) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ThumbWidth( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwWidth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ThumbHeight( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwHeight) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Thumbnail( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPath) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PictureWidth( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwWidth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PictureHeight( 
            /* [retval][out] */ DWORD __RPC_FAR *pdwHeight) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWiaDispatchItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWiaDispatchItem __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWiaDispatchItem __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemsFromUI )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwIntent,
            /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPropById )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ DWORD propid,
            /* [retval][out] */ VARIANT __RPC_FAR *pvaOut);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Transfer )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [in] */ BSTR pbstrFilename,
            BOOL bAsyncTransfer);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Children )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCollection);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ItemType )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectStatus )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrStatus);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Time )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTime);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FirmwareVersion )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrVersion);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFullName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwWidth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwHeight);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ThumbWidth )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwWidth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ThumbHeight )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwHeight);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Thumbnail )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPath);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PictureWidth )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwWidth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PictureHeight )( 
            IWiaDispatchItem __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pdwHeight);
        
        END_INTERFACE
    } IWiaDispatchItemVtbl;

    interface IWiaDispatchItem
    {
        CONST_VTBL struct IWiaDispatchItemVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWiaDispatchItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWiaDispatchItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWiaDispatchItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWiaDispatchItem_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWiaDispatchItem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWiaDispatchItem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWiaDispatchItem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWiaDispatchItem_GetItemsFromUI(This,dwFlags,dwIntent,ppCollection)	\
    (This)->lpVtbl -> GetItemsFromUI(This,dwFlags,dwIntent,ppCollection)

#define IWiaDispatchItem_GetPropById(This,propid,pvaOut)	\
    (This)->lpVtbl -> GetPropById(This,propid,pvaOut)

#define IWiaDispatchItem_Transfer(This,pbstrFilename,bAsyncTransfer)	\
    (This)->lpVtbl -> Transfer(This,pbstrFilename,bAsyncTransfer)

#define IWiaDispatchItem_get_Children(This,ppCollection)	\
    (This)->lpVtbl -> get_Children(This,ppCollection)

#define IWiaDispatchItem_get_ItemType(This,pbstrType)	\
    (This)->lpVtbl -> get_ItemType(This,pbstrType)

#define IWiaDispatchItem_get_ConnectStatus(This,pbstrStatus)	\
    (This)->lpVtbl -> get_ConnectStatus(This,pbstrStatus)

#define IWiaDispatchItem_get_Time(This,pbstrTime)	\
    (This)->lpVtbl -> get_Time(This,pbstrTime)

#define IWiaDispatchItem_get_FirmwareVersion(This,pbstrVersion)	\
    (This)->lpVtbl -> get_FirmwareVersion(This,pbstrVersion)

#define IWiaDispatchItem_get_Name(This,pbstrName)	\
    (This)->lpVtbl -> get_Name(This,pbstrName)

#define IWiaDispatchItem_get_FullName(This,pbstrFullName)	\
    (This)->lpVtbl -> get_FullName(This,pbstrFullName)

#define IWiaDispatchItem_get_Width(This,pdwWidth)	\
    (This)->lpVtbl -> get_Width(This,pdwWidth)

#define IWiaDispatchItem_get_Height(This,pdwHeight)	\
    (This)->lpVtbl -> get_Height(This,pdwHeight)

#define IWiaDispatchItem_get_ThumbWidth(This,pdwWidth)	\
    (This)->lpVtbl -> get_ThumbWidth(This,pdwWidth)

#define IWiaDispatchItem_get_ThumbHeight(This,pdwHeight)	\
    (This)->lpVtbl -> get_ThumbHeight(This,pdwHeight)

#define IWiaDispatchItem_get_Thumbnail(This,pbstrPath)	\
    (This)->lpVtbl -> get_Thumbnail(This,pbstrPath)

#define IWiaDispatchItem_get_PictureWidth(This,pdwWidth)	\
    (This)->lpVtbl -> get_PictureWidth(This,pdwWidth)

#define IWiaDispatchItem_get_PictureHeight(This,pdwHeight)	\
    (This)->lpVtbl -> get_PictureHeight(This,pdwHeight)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_GetItemsFromUI_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwIntent,
    /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCollection);


void __RPC_STUB IWiaDispatchItem_GetItemsFromUI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_GetPropById_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [in] */ DWORD propid,
    /* [retval][out] */ VARIANT __RPC_FAR *pvaOut);


void __RPC_STUB IWiaDispatchItem_GetPropById_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_Transfer_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [in] */ BSTR pbstrFilename,
    BOOL bAsyncTransfer);


void __RPC_STUB IWiaDispatchItem_Transfer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_Children_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ ICollection __RPC_FAR *__RPC_FAR *ppCollection);


void __RPC_STUB IWiaDispatchItem_get_Children_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_ItemType_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrType);


void __RPC_STUB IWiaDispatchItem_get_ItemType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_ConnectStatus_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrStatus);


void __RPC_STUB IWiaDispatchItem_get_ConnectStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_Time_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTime);


void __RPC_STUB IWiaDispatchItem_get_Time_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_FirmwareVersion_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrVersion);


void __RPC_STUB IWiaDispatchItem_get_FirmwareVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_Name_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrName);


void __RPC_STUB IWiaDispatchItem_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_FullName_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFullName);


void __RPC_STUB IWiaDispatchItem_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_Width_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwWidth);


void __RPC_STUB IWiaDispatchItem_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_Height_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwHeight);


void __RPC_STUB IWiaDispatchItem_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_ThumbWidth_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwWidth);


void __RPC_STUB IWiaDispatchItem_get_ThumbWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_ThumbHeight_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwHeight);


void __RPC_STUB IWiaDispatchItem_get_ThumbHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_Thumbnail_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPath);


void __RPC_STUB IWiaDispatchItem_get_Thumbnail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_PictureWidth_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwWidth);


void __RPC_STUB IWiaDispatchItem_get_PictureWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaDispatchItem_get_PictureHeight_Proxy( 
    IWiaDispatchItem __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pdwHeight);


void __RPC_STUB IWiaDispatchItem_get_PictureHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWiaDispatchItem_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Sample;

#ifdef __cplusplus

class DECLSPEC_UUID("4C43FC25-E748-427F-8021-CB590027396F")
Sample;
#endif

EXTERN_C const CLSID CLSID_Wia;

#ifdef __cplusplus

class DECLSPEC_UUID("0DAD5531-BF31-43AC-A513-1F8926BBF5EC")
Wia;
#endif

EXTERN_C const CLSID CLSID_WiaProtocol;

#ifdef __cplusplus

class DECLSPEC_UUID("13F3EA8B-91D7-4F0A-AD76-D2853AC8BECE")
WiaProtocol;
#endif
#endif /* __NDMEDIALib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


