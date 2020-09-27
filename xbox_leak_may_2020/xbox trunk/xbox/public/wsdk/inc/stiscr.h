
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Thu Aug 19 14:46:35 1999
 */
/* Compiler settings for stiscr.idl:
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

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __stiscr_h__
#define __stiscr_h__

/* Forward Declarations */ 

#ifndef __IWiaDevMgrDispatch_FWD_DEFINED__
#define __IWiaDevMgrDispatch_FWD_DEFINED__
typedef interface IWiaDevMgrDispatch IWiaDevMgrDispatch;
#endif 	/* __IWiaDevMgrDispatch_FWD_DEFINED__ */


#ifndef __IEnumWiaDevInfoDispatch_FWD_DEFINED__
#define __IEnumWiaDevInfoDispatch_FWD_DEFINED__
typedef interface IEnumWiaDevInfoDispatch IEnumWiaDevInfoDispatch;
#endif 	/* __IEnumWiaDevInfoDispatch_FWD_DEFINED__ */


#ifndef __IPropStorageDispatch_FWD_DEFINED__
#define __IPropStorageDispatch_FWD_DEFINED__
typedef interface IPropStorageDispatch IPropStorageDispatch;
#endif 	/* __IPropStorageDispatch_FWD_DEFINED__ */


#ifndef __IEnumStatPropStgDispatch_FWD_DEFINED__
#define __IEnumStatPropStgDispatch_FWD_DEFINED__
typedef interface IEnumStatPropStgDispatch IEnumStatPropStgDispatch;
#endif 	/* __IEnumStatPropStgDispatch_FWD_DEFINED__ */


#ifndef __IWiaItemDispatch_FWD_DEFINED__
#define __IWiaItemDispatch_FWD_DEFINED__
typedef interface IWiaItemDispatch IWiaItemDispatch;
#endif 	/* __IWiaItemDispatch_FWD_DEFINED__ */


#ifndef __IEnumWiaItemDispatch_FWD_DEFINED__
#define __IEnumWiaItemDispatch_FWD_DEFINED__
typedef interface IEnumWiaItemDispatch IEnumWiaItemDispatch;
#endif 	/* __IEnumWiaItemDispatch_FWD_DEFINED__ */


#ifndef __IEnumWiaDevCapsDispatch_FWD_DEFINED__
#define __IEnumWiaDevCapsDispatch_FWD_DEFINED__
typedef interface IEnumWiaDevCapsDispatch IEnumWiaDevCapsDispatch;
#endif 	/* __IEnumWiaDevCapsDispatch_FWD_DEFINED__ */


#ifndef __WiaDevMgrDispatch_FWD_DEFINED__
#define __WiaDevMgrDispatch_FWD_DEFINED__

#ifdef __cplusplus
typedef class WiaDevMgrDispatch WiaDevMgrDispatch;
#else
typedef struct WiaDevMgrDispatch WiaDevMgrDispatch;
#endif /* __cplusplus */

#endif 	/* __WiaDevMgrDispatch_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "oaidl.h"
#include "propidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_stiscr_0000 */
/* [local] */ 










extern RPC_IF_HANDLE __MIDL_itf_stiscr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_stiscr_0000_v0_0_s_ifspec;

#ifndef __IWiaDevMgrDispatch_INTERFACE_DEFINED__
#define __IWiaDevMgrDispatch_INTERFACE_DEFINED__

/* interface IWiaDevMgrDispatch */
/* [oleautomation][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IWiaDevMgrDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f768d350-f9a4-11d1-9a0b-00c04fa36145")
    IWiaDevMgrDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImageClipboard( 
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lDeviceType,
            /* [in] */ LONG lFlags,
            /* [in] */ LONG lIntent,
            /* [in] */ BSTR bstrFormat,
            /* [unique][in] */ IDispatch __RPC_FAR *pItem,
            /* [retval][out] */ LONG __RPC_FAR *lResult) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImageFile( 
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lDeviceType,
            /* [in] */ LONG lFlags,
            /* [in] */ LONG lIntent,
            /* [in] */ BSTR bstrFormat,
            /* [unique][in] */ IDispatch __RPC_FAR *pItem,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumDeviceInfo( 
            /* [in] */ LONG lFlag,
            /* [retval][out] */ IEnumWiaDevInfoDispatch __RPC_FAR *__RPC_FAR *ppIEnum) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateDevice( 
            /* [in] */ BSTR bstrDeviceID,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectDevice( 
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lDeviceType,
            /* [in] */ LONG lFlags,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterEventCallbackInterface( 
            /* [in] */ LONG lFlags,
            /* [in] */ BSTR bstrDevID,
            /* [in] */ BSTR guidEventID,
            /* [in] */ IDispatch __RPC_FAR *pIDisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWiaDevMgrDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWiaDevMgrDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWiaDevMgrDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImageClipboard )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lDeviceType,
            /* [in] */ LONG lFlags,
            /* [in] */ LONG lIntent,
            /* [in] */ BSTR bstrFormat,
            /* [unique][in] */ IDispatch __RPC_FAR *pItem,
            /* [retval][out] */ LONG __RPC_FAR *lResult);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetImageFile )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lDeviceType,
            /* [in] */ LONG lFlags,
            /* [in] */ LONG lIntent,
            /* [in] */ BSTR bstrFormat,
            /* [unique][in] */ IDispatch __RPC_FAR *pItem,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumDeviceInfo )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlag,
            /* [retval][out] */ IEnumWiaDevInfoDispatch __RPC_FAR *__RPC_FAR *ppIEnum);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateDevice )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ BSTR bstrDeviceID,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelectDevice )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lDeviceType,
            /* [in] */ LONG lFlags,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterEventCallbackInterface )( 
            IWiaDevMgrDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlags,
            /* [in] */ BSTR bstrDevID,
            /* [in] */ BSTR guidEventID,
            /* [in] */ IDispatch __RPC_FAR *pIDisp);
        
        END_INTERFACE
    } IWiaDevMgrDispatchVtbl;

    interface IWiaDevMgrDispatch
    {
        CONST_VTBL struct IWiaDevMgrDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWiaDevMgrDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWiaDevMgrDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWiaDevMgrDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWiaDevMgrDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWiaDevMgrDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWiaDevMgrDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWiaDevMgrDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWiaDevMgrDispatch_GetImageClipboard(This,hwndParent,lDeviceType,lFlags,lIntent,bstrFormat,pItem,lResult)	\
    (This)->lpVtbl -> GetImageClipboard(This,hwndParent,lDeviceType,lFlags,lIntent,bstrFormat,pItem,lResult)

#define IWiaDevMgrDispatch_GetImageFile(This,hwndParent,lDeviceType,lFlags,lIntent,bstrFormat,pItem,pbstrFileName)	\
    (This)->lpVtbl -> GetImageFile(This,hwndParent,lDeviceType,lFlags,lIntent,bstrFormat,pItem,pbstrFileName)

#define IWiaDevMgrDispatch_EnumDeviceInfo(This,lFlag,ppIEnum)	\
    (This)->lpVtbl -> EnumDeviceInfo(This,lFlag,ppIEnum)

#define IWiaDevMgrDispatch_CreateDevice(This,bstrDeviceID,ppItem)	\
    (This)->lpVtbl -> CreateDevice(This,bstrDeviceID,ppItem)

#define IWiaDevMgrDispatch_SelectDevice(This,hwndParent,lDeviceType,lFlags,ppItem)	\
    (This)->lpVtbl -> SelectDevice(This,hwndParent,lDeviceType,lFlags,ppItem)

#define IWiaDevMgrDispatch_RegisterEventCallbackInterface(This,lFlags,bstrDevID,guidEventID,pIDisp)	\
    (This)->lpVtbl -> RegisterEventCallbackInterface(This,lFlags,bstrDevID,guidEventID,pIDisp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDevMgrDispatch_GetImageClipboard_Proxy( 
    IWiaDevMgrDispatch __RPC_FAR * This,
    /* [in] */ LONG hwndParent,
    /* [in] */ LONG lDeviceType,
    /* [in] */ LONG lFlags,
    /* [in] */ LONG lIntent,
    /* [in] */ BSTR bstrFormat,
    /* [unique][in] */ IDispatch __RPC_FAR *pItem,
    /* [retval][out] */ LONG __RPC_FAR *lResult);


void __RPC_STUB IWiaDevMgrDispatch_GetImageClipboard_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDevMgrDispatch_GetImageFile_Proxy( 
    IWiaDevMgrDispatch __RPC_FAR * This,
    /* [in] */ LONG hwndParent,
    /* [in] */ LONG lDeviceType,
    /* [in] */ LONG lFlags,
    /* [in] */ LONG lIntent,
    /* [in] */ BSTR bstrFormat,
    /* [unique][in] */ IDispatch __RPC_FAR *pItem,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);


void __RPC_STUB IWiaDevMgrDispatch_GetImageFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDevMgrDispatch_EnumDeviceInfo_Proxy( 
    IWiaDevMgrDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlag,
    /* [retval][out] */ IEnumWiaDevInfoDispatch __RPC_FAR *__RPC_FAR *ppIEnum);


void __RPC_STUB IWiaDevMgrDispatch_EnumDeviceInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDevMgrDispatch_CreateDevice_Proxy( 
    IWiaDevMgrDispatch __RPC_FAR * This,
    /* [in] */ BSTR bstrDeviceID,
    /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppItem);


void __RPC_STUB IWiaDevMgrDispatch_CreateDevice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDevMgrDispatch_SelectDevice_Proxy( 
    IWiaDevMgrDispatch __RPC_FAR * This,
    /* [in] */ LONG hwndParent,
    /* [in] */ LONG lDeviceType,
    /* [in] */ LONG lFlags,
    /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppItem);


void __RPC_STUB IWiaDevMgrDispatch_SelectDevice_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaDevMgrDispatch_RegisterEventCallbackInterface_Proxy( 
    IWiaDevMgrDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlags,
    /* [in] */ BSTR bstrDevID,
    /* [in] */ BSTR guidEventID,
    /* [in] */ IDispatch __RPC_FAR *pIDisp);


void __RPC_STUB IWiaDevMgrDispatch_RegisterEventCallbackInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWiaDevMgrDispatch_INTERFACE_DEFINED__ */


#ifndef __IEnumWiaDevInfoDispatch_INTERFACE_DEFINED__
#define __IEnumWiaDevInfoDispatch_INTERFACE_DEFINED__

/* interface IEnumWiaDevInfoDispatch */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumWiaDevInfoDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8e25d848-8949-11d2-b081-00c04fa36145")
    IEnumWiaDevInfoDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *pProp,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumWiaDevInfoDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *pProp,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumWiaDevInfoDispatch __RPC_FAR * This,
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);
        
        END_INTERFACE
    } IEnumWiaDevInfoDispatchVtbl;

    interface IEnumWiaDevInfoDispatch
    {
        CONST_VTBL struct IEnumWiaDevInfoDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumWiaDevInfoDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumWiaDevInfoDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumWiaDevInfoDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumWiaDevInfoDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumWiaDevInfoDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumWiaDevInfoDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumWiaDevInfoDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumWiaDevInfoDispatch_get__NewEnum(This,ppEnumVar)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnumVar)

#define IEnumWiaDevInfoDispatch_get_Item(This,lIndex,retVal)	\
    (This)->lpVtbl -> get_Item(This,lIndex,retVal)

#define IEnumWiaDevInfoDispatch_Next(This,celt,pProp,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,pProp,pceltFetched)

#define IEnumWiaDevInfoDispatch_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumWiaDevInfoDispatch_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumWiaDevInfoDispatch_Clone(This,ppIEnum)	\
    (This)->lpVtbl -> Clone(This,ppIEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevInfoDispatch_get__NewEnum_Proxy( 
    IEnumWiaDevInfoDispatch __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);


void __RPC_STUB IEnumWiaDevInfoDispatch_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevInfoDispatch_get_Item_Proxy( 
    IEnumWiaDevInfoDispatch __RPC_FAR * This,
    /* [in] */ LONG lIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);


void __RPC_STUB IEnumWiaDevInfoDispatch_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevInfoDispatch_Next_Proxy( 
    IEnumWiaDevInfoDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ VARIANT __RPC_FAR *pProp,
    /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumWiaDevInfoDispatch_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevInfoDispatch_Skip_Proxy( 
    IEnumWiaDevInfoDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumWiaDevInfoDispatch_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevInfoDispatch_Reset_Proxy( 
    IEnumWiaDevInfoDispatch __RPC_FAR * This);


void __RPC_STUB IEnumWiaDevInfoDispatch_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevInfoDispatch_Clone_Proxy( 
    IEnumWiaDevInfoDispatch __RPC_FAR * This,
    /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);


void __RPC_STUB IEnumWiaDevInfoDispatch_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumWiaDevInfoDispatch_INTERFACE_DEFINED__ */


#ifndef __IPropStorageDispatch_INTERFACE_DEFINED__
#define __IPropStorageDispatch_INTERFACE_DEFINED__

/* interface IPropStorageDispatch */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IPropStorageDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a2cef640-8f9e-11d2-b083-00c04fa36145")
    IPropStorageDispatch : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReadMultiple( 
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [retval][out] */ VARIANT __RPC_FAR *rgpropvar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteMultiple( 
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [in] */ VARIANT __RPC_FAR *rgpropvar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReadPropertyNames( 
            /* [in] */ VARIANT __RPC_FAR *pvPropSpec,
            /* [retval][out] */ VARIANT __RPC_FAR *pvPropNames) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enum( 
            /* [retval][out] */ IEnumStatPropStgDispatch __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WritePropertyNames( 
            /* [in] */ VARIANT __RPC_FAR *rgpropid,
            /* [out] */ VARIANT __RPC_FAR *rglpwstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPropertyAttributes( 
            /* [in] */ VARIANT cpspec,
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [out] */ VARIANT __RPC_FAR *rgflags,
            /* [out] */ VARIANT __RPC_FAR *rgpropvar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ VARIANT __RPC_FAR *pulNumProps) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropStorageDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPropStorageDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPropStorageDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadMultiple )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [retval][out] */ VARIANT __RPC_FAR *rgpropvar);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteMultiple )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [in] */ VARIANT __RPC_FAR *rgpropvar);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadPropertyNames )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *pvPropSpec,
            /* [retval][out] */ VARIANT __RPC_FAR *pvPropNames);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Enum )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [retval][out] */ IEnumStatPropStgDispatch __RPC_FAR *__RPC_FAR *ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WritePropertyNames )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *rgpropid,
            /* [out] */ VARIANT __RPC_FAR *rglpwstrName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPropertyAttributes )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [in] */ VARIANT cpspec,
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [out] */ VARIANT __RPC_FAR *rgflags,
            /* [out] */ VARIANT __RPC_FAR *rgpropvar);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCount )( 
            IPropStorageDispatch __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *pulNumProps);
        
        END_INTERFACE
    } IPropStorageDispatchVtbl;

    interface IPropStorageDispatch
    {
        CONST_VTBL struct IPropStorageDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropStorageDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropStorageDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropStorageDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropStorageDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPropStorageDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPropStorageDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPropStorageDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPropStorageDispatch_ReadMultiple(This,rgpspec,rgpropvar)	\
    (This)->lpVtbl -> ReadMultiple(This,rgpspec,rgpropvar)

#define IPropStorageDispatch_WriteMultiple(This,rgpspec,rgpropvar)	\
    (This)->lpVtbl -> WriteMultiple(This,rgpspec,rgpropvar)

#define IPropStorageDispatch_ReadPropertyNames(This,pvPropSpec,pvPropNames)	\
    (This)->lpVtbl -> ReadPropertyNames(This,pvPropSpec,pvPropNames)

#define IPropStorageDispatch_Enum(This,ppEnum)	\
    (This)->lpVtbl -> Enum(This,ppEnum)

#define IPropStorageDispatch_WritePropertyNames(This,rgpropid,rglpwstrName)	\
    (This)->lpVtbl -> WritePropertyNames(This,rgpropid,rglpwstrName)

#define IPropStorageDispatch_GetPropertyAttributes(This,cpspec,rgpspec,rgflags,rgpropvar)	\
    (This)->lpVtbl -> GetPropertyAttributes(This,cpspec,rgpspec,rgflags,rgpropvar)

#define IPropStorageDispatch_GetCount(This,pulNumProps)	\
    (This)->lpVtbl -> GetCount(This,pulNumProps)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPropStorageDispatch_ReadMultiple_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *rgpspec,
    /* [retval][out] */ VARIANT __RPC_FAR *rgpropvar);


void __RPC_STUB IPropStorageDispatch_ReadMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropStorageDispatch_WriteMultiple_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *rgpspec,
    /* [in] */ VARIANT __RPC_FAR *rgpropvar);


void __RPC_STUB IPropStorageDispatch_WriteMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropStorageDispatch_ReadPropertyNames_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *pvPropSpec,
    /* [retval][out] */ VARIANT __RPC_FAR *pvPropNames);


void __RPC_STUB IPropStorageDispatch_ReadPropertyNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropStorageDispatch_Enum_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [retval][out] */ IEnumStatPropStgDispatch __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IPropStorageDispatch_Enum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropStorageDispatch_WritePropertyNames_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *rgpropid,
    /* [out] */ VARIANT __RPC_FAR *rglpwstrName);


void __RPC_STUB IPropStorageDispatch_WritePropertyNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropStorageDispatch_GetPropertyAttributes_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [in] */ VARIANT cpspec,
    /* [in] */ VARIANT __RPC_FAR *rgpspec,
    /* [out] */ VARIANT __RPC_FAR *rgflags,
    /* [out] */ VARIANT __RPC_FAR *rgpropvar);


void __RPC_STUB IPropStorageDispatch_GetPropertyAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropStorageDispatch_GetCount_Proxy( 
    IPropStorageDispatch __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *pulNumProps);


void __RPC_STUB IPropStorageDispatch_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropStorageDispatch_INTERFACE_DEFINED__ */


#ifndef __IEnumStatPropStgDispatch_INTERFACE_DEFINED__
#define __IEnumStatPropStgDispatch_INTERFACE_DEFINED__

/* interface IEnumStatPropStgDispatch */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumStatPropStgDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("340bdb90-9f44-11d2-b08a-00c04fa36145")
    IEnumStatPropStgDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *pProp,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumStatPropStgDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumStatPropStgDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumStatPropStgDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *pProp,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumStatPropStgDispatch __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumStatPropStgDispatch __RPC_FAR * This,
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);
        
        END_INTERFACE
    } IEnumStatPropStgDispatchVtbl;

    interface IEnumStatPropStgDispatch
    {
        CONST_VTBL struct IEnumStatPropStgDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumStatPropStgDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumStatPropStgDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumStatPropStgDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumStatPropStgDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumStatPropStgDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumStatPropStgDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumStatPropStgDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumStatPropStgDispatch_get__NewEnum(This,ppEnumVar)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnumVar)

#define IEnumStatPropStgDispatch_get_Item(This,lIndex,retVal)	\
    (This)->lpVtbl -> get_Item(This,lIndex,retVal)

#define IEnumStatPropStgDispatch_Next(This,celt,pProp,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,pProp,pceltFetched)

#define IEnumStatPropStgDispatch_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumStatPropStgDispatch_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumStatPropStgDispatch_Clone(This,ppIEnum)	\
    (This)->lpVtbl -> Clone(This,ppIEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumStatPropStgDispatch_get__NewEnum_Proxy( 
    IEnumStatPropStgDispatch __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);


void __RPC_STUB IEnumStatPropStgDispatch_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumStatPropStgDispatch_get_Item_Proxy( 
    IEnumStatPropStgDispatch __RPC_FAR * This,
    /* [in] */ LONG lIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);


void __RPC_STUB IEnumStatPropStgDispatch_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumStatPropStgDispatch_Next_Proxy( 
    IEnumStatPropStgDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ VARIANT __RPC_FAR *pProp,
    /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumStatPropStgDispatch_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumStatPropStgDispatch_Skip_Proxy( 
    IEnumStatPropStgDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumStatPropStgDispatch_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumStatPropStgDispatch_Reset_Proxy( 
    IEnumStatPropStgDispatch __RPC_FAR * This);


void __RPC_STUB IEnumStatPropStgDispatch_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumStatPropStgDispatch_Clone_Proxy( 
    IEnumStatPropStgDispatch __RPC_FAR * This,
    /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);


void __RPC_STUB IEnumStatPropStgDispatch_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumStatPropStgDispatch_INTERFACE_DEFINED__ */


#ifndef __IWiaItemDispatch_INTERFACE_DEFINED__
#define __IWiaItemDispatch_INTERFACE_DEFINED__

/* interface IWiaItemDispatch */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IWiaItemDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f4097433-9e68-11d2-b08a-00c04fa36145")
    IWiaItemDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetItemType( 
            /* [retval][out] */ LONG __RPC_FAR *plItemType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AnalyzeItem( 
            /* [in] */ LONG lFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumChildItems( 
            /* [retval][out] */ IEnumWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIEnumWiaItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteItem( 
            /* [in] */ LONG lFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateChildItem( 
            /* [in] */ LONG lFlags,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppNewItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeviceCommand( 
            /* [in] */ LONG lFlags,
            /* [in] */ BSTR bstrCommand,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *pIWiaItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeviceDlg( 
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lFlags,
            /* [in] */ LONG lIntent,
            /* [retval][out] */ VARIANT __RPC_FAR *pIWiaItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindItemByName( 
            /* [in] */ LONG lFlags,
            /* [in] */ BSTR bstrFullItemName,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIWiaItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRootItem( 
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIWiaItem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadMultiple( 
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [retval][out] */ VARIANT __RPC_FAR *rgpropvar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteMultiple( 
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [in] */ VARIANT __RPC_FAR *rgpropvar) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumWiaDevCaps( 
            /* [in] */ ULONG ulFlags,
            /* [retval][out] */ IEnumWiaDevCapsDispatch __RPC_FAR *__RPC_FAR *ppIEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWiaItemDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWiaItemDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWiaItemDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemType )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [retval][out] */ LONG __RPC_FAR *plItemType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AnalyzeItem )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumChildItems )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [retval][out] */ IEnumWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIEnumWiaItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteItem )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateChildItem )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlags,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppNewItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeviceCommand )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlags,
            /* [in] */ BSTR bstrCommand,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *pIWiaItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeviceDlg )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG hwndParent,
            /* [in] */ LONG lFlags,
            /* [in] */ LONG lIntent,
            /* [retval][out] */ VARIANT __RPC_FAR *pIWiaItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FindItemByName )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG lFlags,
            /* [in] */ BSTR bstrFullItemName,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIWiaItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRootItem )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIWiaItem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReadMultiple )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [retval][out] */ VARIANT __RPC_FAR *rgpropvar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WriteMultiple )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *rgpspec,
            /* [in] */ VARIANT __RPC_FAR *rgpropvar);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumWiaDevCaps )( 
            IWiaItemDispatch __RPC_FAR * This,
            /* [in] */ ULONG ulFlags,
            /* [retval][out] */ IEnumWiaDevCapsDispatch __RPC_FAR *__RPC_FAR *ppIEnum);
        
        END_INTERFACE
    } IWiaItemDispatchVtbl;

    interface IWiaItemDispatch
    {
        CONST_VTBL struct IWiaItemDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWiaItemDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWiaItemDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWiaItemDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWiaItemDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWiaItemDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWiaItemDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWiaItemDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWiaItemDispatch_GetItemType(This,plItemType)	\
    (This)->lpVtbl -> GetItemType(This,plItemType)

#define IWiaItemDispatch_AnalyzeItem(This,lFlags)	\
    (This)->lpVtbl -> AnalyzeItem(This,lFlags)

#define IWiaItemDispatch_EnumChildItems(This,ppIEnumWiaItem)	\
    (This)->lpVtbl -> EnumChildItems(This,ppIEnumWiaItem)

#define IWiaItemDispatch_DeleteItem(This,lFlags)	\
    (This)->lpVtbl -> DeleteItem(This,lFlags)

#define IWiaItemDispatch_CreateChildItem(This,lFlags,ppNewItem)	\
    (This)->lpVtbl -> CreateChildItem(This,lFlags,ppNewItem)

#define IWiaItemDispatch_DeviceCommand(This,lFlags,bstrCommand,pIWiaItem)	\
    (This)->lpVtbl -> DeviceCommand(This,lFlags,bstrCommand,pIWiaItem)

#define IWiaItemDispatch_DeviceDlg(This,hwndParent,lFlags,lIntent,pIWiaItem)	\
    (This)->lpVtbl -> DeviceDlg(This,hwndParent,lFlags,lIntent,pIWiaItem)

#define IWiaItemDispatch_FindItemByName(This,lFlags,bstrFullItemName,ppIWiaItem)	\
    (This)->lpVtbl -> FindItemByName(This,lFlags,bstrFullItemName,ppIWiaItem)

#define IWiaItemDispatch_GetRootItem(This,ppIWiaItem)	\
    (This)->lpVtbl -> GetRootItem(This,ppIWiaItem)

#define IWiaItemDispatch_ReadMultiple(This,rgpspec,rgpropvar)	\
    (This)->lpVtbl -> ReadMultiple(This,rgpspec,rgpropvar)

#define IWiaItemDispatch_WriteMultiple(This,rgpspec,rgpropvar)	\
    (This)->lpVtbl -> WriteMultiple(This,rgpspec,rgpropvar)

#define IWiaItemDispatch_EnumWiaDevCaps(This,ulFlags,ppIEnum)	\
    (This)->lpVtbl -> EnumWiaDevCaps(This,ulFlags,ppIEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_GetItemType_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [retval][out] */ LONG __RPC_FAR *plItemType);


void __RPC_STUB IWiaItemDispatch_GetItemType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_AnalyzeItem_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlags);


void __RPC_STUB IWiaItemDispatch_AnalyzeItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_EnumChildItems_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [retval][out] */ IEnumWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIEnumWiaItem);


void __RPC_STUB IWiaItemDispatch_EnumChildItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_DeleteItem_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlags);


void __RPC_STUB IWiaItemDispatch_DeleteItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_CreateChildItem_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlags,
    /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppNewItem);


void __RPC_STUB IWiaItemDispatch_CreateChildItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_DeviceCommand_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlags,
    /* [in] */ BSTR bstrCommand,
    /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *pIWiaItem);


void __RPC_STUB IWiaItemDispatch_DeviceCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_DeviceDlg_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG hwndParent,
    /* [in] */ LONG lFlags,
    /* [in] */ LONG lIntent,
    /* [retval][out] */ VARIANT __RPC_FAR *pIWiaItem);


void __RPC_STUB IWiaItemDispatch_DeviceDlg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_FindItemByName_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG lFlags,
    /* [in] */ BSTR bstrFullItemName,
    /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIWiaItem);


void __RPC_STUB IWiaItemDispatch_FindItemByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_GetRootItem_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [retval][out] */ IWiaItemDispatch __RPC_FAR *__RPC_FAR *ppIWiaItem);


void __RPC_STUB IWiaItemDispatch_GetRootItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_ReadMultiple_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *rgpspec,
    /* [retval][out] */ VARIANT __RPC_FAR *rgpropvar);


void __RPC_STUB IWiaItemDispatch_ReadMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_WriteMultiple_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *rgpspec,
    /* [in] */ VARIANT __RPC_FAR *rgpropvar);


void __RPC_STUB IWiaItemDispatch_WriteMultiple_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaItemDispatch_EnumWiaDevCaps_Proxy( 
    IWiaItemDispatch __RPC_FAR * This,
    /* [in] */ ULONG ulFlags,
    /* [retval][out] */ IEnumWiaDevCapsDispatch __RPC_FAR *__RPC_FAR *ppIEnum);


void __RPC_STUB IWiaItemDispatch_EnumWiaDevCaps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWiaItemDispatch_INTERFACE_DEFINED__ */


#ifndef __IEnumWiaItemDispatch_INTERFACE_DEFINED__
#define __IEnumWiaItemDispatch_INTERFACE_DEFINED__

/* interface IEnumWiaItemDispatch */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumWiaItemDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ff0bb94d-9e68-11d2-b08a-00c04fa36145")
    IEnumWiaItemDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *pItem,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumWiaItemDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumWiaItemDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumWiaItemDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *pItem,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumWiaItemDispatch __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumWiaItemDispatch __RPC_FAR * This,
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);
        
        END_INTERFACE
    } IEnumWiaItemDispatchVtbl;

    interface IEnumWiaItemDispatch
    {
        CONST_VTBL struct IEnumWiaItemDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumWiaItemDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumWiaItemDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumWiaItemDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumWiaItemDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumWiaItemDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumWiaItemDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumWiaItemDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumWiaItemDispatch_get__NewEnum(This,ppEnumVar)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnumVar)

#define IEnumWiaItemDispatch_get_Item(This,lIndex,retVal)	\
    (This)->lpVtbl -> get_Item(This,lIndex,retVal)

#define IEnumWiaItemDispatch_Next(This,celt,pItem,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,pItem,pceltFetched)

#define IEnumWiaItemDispatch_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumWiaItemDispatch_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumWiaItemDispatch_Clone(This,ppIEnum)	\
    (This)->lpVtbl -> Clone(This,ppIEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaItemDispatch_get__NewEnum_Proxy( 
    IEnumWiaItemDispatch __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);


void __RPC_STUB IEnumWiaItemDispatch_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaItemDispatch_get_Item_Proxy( 
    IEnumWiaItemDispatch __RPC_FAR * This,
    /* [in] */ LONG lIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);


void __RPC_STUB IEnumWiaItemDispatch_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaItemDispatch_Next_Proxy( 
    IEnumWiaItemDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ VARIANT __RPC_FAR *pItem,
    /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumWiaItemDispatch_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaItemDispatch_Skip_Proxy( 
    IEnumWiaItemDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumWiaItemDispatch_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaItemDispatch_Reset_Proxy( 
    IEnumWiaItemDispatch __RPC_FAR * This);


void __RPC_STUB IEnumWiaItemDispatch_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaItemDispatch_Clone_Proxy( 
    IEnumWiaItemDispatch __RPC_FAR * This,
    /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);


void __RPC_STUB IEnumWiaItemDispatch_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumWiaItemDispatch_INTERFACE_DEFINED__ */


#ifndef __IEnumWiaDevCapsDispatch_INTERFACE_DEFINED__
#define __IEnumWiaDevCapsDispatch_INTERFACE_DEFINED__

/* interface IEnumWiaDevCapsDispatch */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumWiaDevCapsDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("05B40A3C-E6BC-11d2-B5C2-00C04F68C956")
    IEnumWiaDevCapsDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *DevCap,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumWiaDevCapsDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ LONG lIndex,
            /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [out] */ VARIANT __RPC_FAR *DevCap,
            /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumWiaDevCapsDispatch __RPC_FAR * This,
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);
        
        END_INTERFACE
    } IEnumWiaDevCapsDispatchVtbl;

    interface IEnumWiaDevCapsDispatch
    {
        CONST_VTBL struct IEnumWiaDevCapsDispatchVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumWiaDevCapsDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumWiaDevCapsDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumWiaDevCapsDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumWiaDevCapsDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEnumWiaDevCapsDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEnumWiaDevCapsDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEnumWiaDevCapsDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEnumWiaDevCapsDispatch_get__NewEnum(This,ppEnumVar)	\
    (This)->lpVtbl -> get__NewEnum(This,ppEnumVar)

#define IEnumWiaDevCapsDispatch_get_Item(This,lIndex,retVal)	\
    (This)->lpVtbl -> get_Item(This,lIndex,retVal)

#define IEnumWiaDevCapsDispatch_Next(This,celt,DevCap,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,DevCap,pceltFetched)

#define IEnumWiaDevCapsDispatch_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumWiaDevCapsDispatch_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumWiaDevCapsDispatch_Clone(This,ppIEnum)	\
    (This)->lpVtbl -> Clone(This,ppIEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevCapsDispatch_get__NewEnum_Proxy( 
    IEnumWiaDevCapsDispatch __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppEnumVar);


void __RPC_STUB IEnumWiaDevCapsDispatch_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevCapsDispatch_get_Item_Proxy( 
    IEnumWiaDevCapsDispatch __RPC_FAR * This,
    /* [in] */ LONG lIndex,
    /* [retval][out] */ VARIANT __RPC_FAR *__RPC_FAR *retVal);


void __RPC_STUB IEnumWiaDevCapsDispatch_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevCapsDispatch_Next_Proxy( 
    IEnumWiaDevCapsDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [out] */ VARIANT __RPC_FAR *DevCap,
    /* [unique][out][in] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumWiaDevCapsDispatch_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevCapsDispatch_Skip_Proxy( 
    IEnumWiaDevCapsDispatch __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumWiaDevCapsDispatch_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevCapsDispatch_Reset_Proxy( 
    IEnumWiaDevCapsDispatch __RPC_FAR * This);


void __RPC_STUB IEnumWiaDevCapsDispatch_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumWiaDevCapsDispatch_Clone_Proxy( 
    IEnumWiaDevCapsDispatch __RPC_FAR * This,
    /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *ppIEnum);


void __RPC_STUB IEnumWiaDevCapsDispatch_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumWiaDevCapsDispatch_INTERFACE_DEFINED__ */



#ifndef __WiaDevMgrDispatch_LIBRARY_DEFINED__
#define __WiaDevMgrDispatch_LIBRARY_DEFINED__

/* library WiaDevMgrDispatch */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_WiaDevMgrDispatch;

EXTERN_C const CLSID CLSID_WiaDevMgrDispatch;

#ifdef __cplusplus

class DECLSPEC_UUID("4b97598e-006e-11d2-87ea-0060081ed811")
WiaDevMgrDispatch;
#endif
#endif /* __WiaDevMgrDispatch_LIBRARY_DEFINED__ */

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


