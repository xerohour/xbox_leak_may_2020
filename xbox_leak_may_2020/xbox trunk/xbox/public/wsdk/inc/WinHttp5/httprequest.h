
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0345 */
/* Compiler settings for httprequest.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
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

#ifndef __httprequest_h__
#define __httprequest_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWinHttpRequest_FWD_DEFINED__
#define __IWinHttpRequest_FWD_DEFINED__
typedef interface IWinHttpRequest IWinHttpRequest;
#endif 	/* __IWinHttpRequest_FWD_DEFINED__ */


#ifndef __IWinHttpRequestEvents_FWD_DEFINED__
#define __IWinHttpRequestEvents_FWD_DEFINED__
typedef interface IWinHttpRequestEvents IWinHttpRequestEvents;
#endif 	/* __IWinHttpRequestEvents_FWD_DEFINED__ */


#ifndef __WinHttpRequest_FWD_DEFINED__
#define __WinHttpRequest_FWD_DEFINED__

#ifdef __cplusplus
typedef class WinHttpRequest WinHttpRequest;
#else
typedef struct WinHttpRequest WinHttpRequest;
#endif /* __cplusplus */

#endif 	/* __WinHttpRequest_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_httprequest_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2000.
//
//--------------------------------------------------------------------------


extern RPC_IF_HANDLE __MIDL_itf_httprequest_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_httprequest_0000_v0_0_s_ifspec;


#ifndef __WinHttp_LIBRARY_DEFINED__
#define __WinHttp_LIBRARY_DEFINED__

/* library WinHttp */
/* [version][lcid][helpstring][uuid] */ 

typedef /* [public] */ long HTTPREQUEST_PROXY_SETTING;

#define	HTTPREQUEST_PROXYSETTING_DEFAULT	( 0 )

#define	HTTPREQUEST_PROXYSETTING_PRECONFIG	( 0 )

#define	HTTPREQUEST_PROXYSETTING_DIRECT	( 0x1 )

#define	HTTPREQUEST_PROXYSETTING_PROXY	( 0x2 )

typedef /* [public] */ long HTTPREQUEST_SETCREDENTIALS_FLAGS;

#define	HTTPREQUEST_SETCREDENTIALS_FOR_SERVER	( 0 )

#define	HTTPREQUEST_SETCREDENTIALS_FOR_PROXY	( 0x1 )

typedef /* [public][public][public][helpstring] */ 
enum __MIDL___MIDL_itf_httprequest_0000_0001
    {	WinHttpRequestOption_UserAgentString	= 0,
	WinHttpRequestOption_URL	= WinHttpRequestOption_UserAgentString + 1,
	WinHttpRequestOption_URLCodePage	= WinHttpRequestOption_URL + 1,
	WinHttpRequestOption_EscapePercentInURL	= WinHttpRequestOption_URLCodePage + 1
    } 	WinHttpRequestOption;


EXTERN_C const IID LIBID_WinHttp;

#ifndef __IWinHttpRequest_INTERFACE_DEFINED__
#define __IWinHttpRequest_INTERFACE_DEFINED__

/* interface IWinHttpRequest */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IWinHttpRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("06f29373-5c5a-4b54-b025-6ef1bf8abf0e")
    IWinHttpRequest : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetProxy( 
            /* [in] */ HTTPREQUEST_PROXY_SETTING ProxySetting,
            /* [optional][in] */ VARIANT varProxyServer,
            /* [optional][in] */ VARIANT varBypassList) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCredentials( 
            /* [in] */ BSTR bstrUserName,
            /* [in] */ BSTR bstrPassword,
            /* [in] */ HTTPREQUEST_SETCREDENTIALS_FLAGS Flags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ BSTR bstrMethod,
            /* [in] */ BSTR bstrUrl,
            /* [optional][in] */ VARIANT varAsync) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRequestHeader( 
            /* [in] */ BSTR bstrHeader,
            /* [in] */ BSTR bstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetResponseHeader( 
            /* [in] */ BSTR bstrHeader,
            /* [retval][out] */ BSTR *pbstrValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllResponseHeaders( 
            /* [retval][out] */ BSTR *pbstrHeaders) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Send( 
            /* [optional][in] */ VARIANT varBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Status( 
            /* [retval][out] */ long *plStatus) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StatusText( 
            /* [retval][out] */ BSTR *pbstrStatus) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResponseText( 
            /* [retval][out] */ BSTR *pbstrBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResponseBody( 
            /* [retval][out] */ VARIANT *pvarBody) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResponseStream( 
            /* [retval][out] */ VARIANT *pvarBody) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Option( 
            /* [in] */ WinHttpRequestOption Option,
            /* [retval][out] */ VARIANT *Value) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Option( 
            /* [in] */ WinHttpRequestOption Option,
            /* [in] */ VARIANT Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitForResponse( 
            /* [optional][in] */ VARIANT Timeout,
            /* [retval][out] */ VARIANT_BOOL *Succeeded) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetTimeouts( 
            /* [in] */ long ResolveTimeout,
            /* [in] */ long ConnectTimeout,
            /* [in] */ long SendTimeout,
            /* [in] */ long ReceiveTimeout) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinHttpRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinHttpRequest * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinHttpRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinHttpRequest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWinHttpRequest * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWinHttpRequest * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWinHttpRequest * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWinHttpRequest * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetProxy )( 
            IWinHttpRequest * This,
            /* [in] */ HTTPREQUEST_PROXY_SETTING ProxySetting,
            /* [optional][in] */ VARIANT varProxyServer,
            /* [optional][in] */ VARIANT varBypassList);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCredentials )( 
            IWinHttpRequest * This,
            /* [in] */ BSTR bstrUserName,
            /* [in] */ BSTR bstrPassword,
            /* [in] */ HTTPREQUEST_SETCREDENTIALS_FLAGS Flags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IWinHttpRequest * This,
            /* [in] */ BSTR bstrMethod,
            /* [in] */ BSTR bstrUrl,
            /* [optional][in] */ VARIANT varAsync);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRequestHeader )( 
            IWinHttpRequest * This,
            /* [in] */ BSTR bstrHeader,
            /* [in] */ BSTR bstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetResponseHeader )( 
            IWinHttpRequest * This,
            /* [in] */ BSTR bstrHeader,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetAllResponseHeaders )( 
            IWinHttpRequest * This,
            /* [retval][out] */ BSTR *pbstrHeaders);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Send )( 
            IWinHttpRequest * This,
            /* [optional][in] */ VARIANT varBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Status )( 
            IWinHttpRequest * This,
            /* [retval][out] */ long *plStatus);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StatusText )( 
            IWinHttpRequest * This,
            /* [retval][out] */ BSTR *pbstrStatus);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ResponseText )( 
            IWinHttpRequest * This,
            /* [retval][out] */ BSTR *pbstrBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ResponseBody )( 
            IWinHttpRequest * This,
            /* [retval][out] */ VARIANT *pvarBody);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ResponseStream )( 
            IWinHttpRequest * This,
            /* [retval][out] */ VARIANT *pvarBody);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Option )( 
            IWinHttpRequest * This,
            /* [in] */ WinHttpRequestOption Option,
            /* [retval][out] */ VARIANT *Value);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Option )( 
            IWinHttpRequest * This,
            /* [in] */ WinHttpRequestOption Option,
            /* [in] */ VARIANT Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitForResponse )( 
            IWinHttpRequest * This,
            /* [optional][in] */ VARIANT Timeout,
            /* [retval][out] */ VARIANT_BOOL *Succeeded);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IWinHttpRequest * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetTimeouts )( 
            IWinHttpRequest * This,
            /* [in] */ long ResolveTimeout,
            /* [in] */ long ConnectTimeout,
            /* [in] */ long SendTimeout,
            /* [in] */ long ReceiveTimeout);
        
        END_INTERFACE
    } IWinHttpRequestVtbl;

    interface IWinHttpRequest
    {
        CONST_VTBL struct IWinHttpRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinHttpRequest_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinHttpRequest_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinHttpRequest_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinHttpRequest_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWinHttpRequest_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWinHttpRequest_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWinHttpRequest_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWinHttpRequest_SetProxy(This,ProxySetting,varProxyServer,varBypassList)	\
    (This)->lpVtbl -> SetProxy(This,ProxySetting,varProxyServer,varBypassList)

#define IWinHttpRequest_SetCredentials(This,bstrUserName,bstrPassword,Flags)	\
    (This)->lpVtbl -> SetCredentials(This,bstrUserName,bstrPassword,Flags)

#define IWinHttpRequest_Open(This,bstrMethod,bstrUrl,varAsync)	\
    (This)->lpVtbl -> Open(This,bstrMethod,bstrUrl,varAsync)

#define IWinHttpRequest_SetRequestHeader(This,bstrHeader,bstrValue)	\
    (This)->lpVtbl -> SetRequestHeader(This,bstrHeader,bstrValue)

#define IWinHttpRequest_GetResponseHeader(This,bstrHeader,pbstrValue)	\
    (This)->lpVtbl -> GetResponseHeader(This,bstrHeader,pbstrValue)

#define IWinHttpRequest_GetAllResponseHeaders(This,pbstrHeaders)	\
    (This)->lpVtbl -> GetAllResponseHeaders(This,pbstrHeaders)

#define IWinHttpRequest_Send(This,varBody)	\
    (This)->lpVtbl -> Send(This,varBody)

#define IWinHttpRequest_get_Status(This,plStatus)	\
    (This)->lpVtbl -> get_Status(This,plStatus)

#define IWinHttpRequest_get_StatusText(This,pbstrStatus)	\
    (This)->lpVtbl -> get_StatusText(This,pbstrStatus)

#define IWinHttpRequest_get_ResponseText(This,pbstrBody)	\
    (This)->lpVtbl -> get_ResponseText(This,pbstrBody)

#define IWinHttpRequest_get_ResponseBody(This,pvarBody)	\
    (This)->lpVtbl -> get_ResponseBody(This,pvarBody)

#define IWinHttpRequest_get_ResponseStream(This,pvarBody)	\
    (This)->lpVtbl -> get_ResponseStream(This,pvarBody)

#define IWinHttpRequest_get_Option(This,Option,Value)	\
    (This)->lpVtbl -> get_Option(This,Option,Value)

#define IWinHttpRequest_put_Option(This,Option,Value)	\
    (This)->lpVtbl -> put_Option(This,Option,Value)

#define IWinHttpRequest_WaitForResponse(This,Timeout,Succeeded)	\
    (This)->lpVtbl -> WaitForResponse(This,Timeout,Succeeded)

#define IWinHttpRequest_Abort(This)	\
    (This)->lpVtbl -> Abort(This)

#define IWinHttpRequest_SetTimeouts(This,ResolveTimeout,ConnectTimeout,SendTimeout,ReceiveTimeout)	\
    (This)->lpVtbl -> SetTimeouts(This,ResolveTimeout,ConnectTimeout,SendTimeout,ReceiveTimeout)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_SetProxy_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ HTTPREQUEST_PROXY_SETTING ProxySetting,
    /* [optional][in] */ VARIANT varProxyServer,
    /* [optional][in] */ VARIANT varBypassList);


void __RPC_STUB IWinHttpRequest_SetProxy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_SetCredentials_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ BSTR bstrUserName,
    /* [in] */ BSTR bstrPassword,
    /* [in] */ HTTPREQUEST_SETCREDENTIALS_FLAGS Flags);


void __RPC_STUB IWinHttpRequest_SetCredentials_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_Open_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ BSTR bstrMethod,
    /* [in] */ BSTR bstrUrl,
    /* [optional][in] */ VARIANT varAsync);


void __RPC_STUB IWinHttpRequest_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_SetRequestHeader_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ BSTR bstrHeader,
    /* [in] */ BSTR bstrValue);


void __RPC_STUB IWinHttpRequest_SetRequestHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_GetResponseHeader_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ BSTR bstrHeader,
    /* [retval][out] */ BSTR *pbstrValue);


void __RPC_STUB IWinHttpRequest_GetResponseHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_GetAllResponseHeaders_Proxy( 
    IWinHttpRequest * This,
    /* [retval][out] */ BSTR *pbstrHeaders);


void __RPC_STUB IWinHttpRequest_GetAllResponseHeaders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_Send_Proxy( 
    IWinHttpRequest * This,
    /* [optional][in] */ VARIANT varBody);


void __RPC_STUB IWinHttpRequest_Send_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_get_Status_Proxy( 
    IWinHttpRequest * This,
    /* [retval][out] */ long *plStatus);


void __RPC_STUB IWinHttpRequest_get_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_get_StatusText_Proxy( 
    IWinHttpRequest * This,
    /* [retval][out] */ BSTR *pbstrStatus);


void __RPC_STUB IWinHttpRequest_get_StatusText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_get_ResponseText_Proxy( 
    IWinHttpRequest * This,
    /* [retval][out] */ BSTR *pbstrBody);


void __RPC_STUB IWinHttpRequest_get_ResponseText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_get_ResponseBody_Proxy( 
    IWinHttpRequest * This,
    /* [retval][out] */ VARIANT *pvarBody);


void __RPC_STUB IWinHttpRequest_get_ResponseBody_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_get_ResponseStream_Proxy( 
    IWinHttpRequest * This,
    /* [retval][out] */ VARIANT *pvarBody);


void __RPC_STUB IWinHttpRequest_get_ResponseStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_get_Option_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ WinHttpRequestOption Option,
    /* [retval][out] */ VARIANT *Value);


void __RPC_STUB IWinHttpRequest_get_Option_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_put_Option_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ WinHttpRequestOption Option,
    /* [in] */ VARIANT Value);


void __RPC_STUB IWinHttpRequest_put_Option_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_WaitForResponse_Proxy( 
    IWinHttpRequest * This,
    /* [optional][in] */ VARIANT Timeout,
    /* [retval][out] */ VARIANT_BOOL *Succeeded);


void __RPC_STUB IWinHttpRequest_WaitForResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_Abort_Proxy( 
    IWinHttpRequest * This);


void __RPC_STUB IWinHttpRequest_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWinHttpRequest_SetTimeouts_Proxy( 
    IWinHttpRequest * This,
    /* [in] */ long ResolveTimeout,
    /* [in] */ long ConnectTimeout,
    /* [in] */ long SendTimeout,
    /* [in] */ long ReceiveTimeout);


void __RPC_STUB IWinHttpRequest_SetTimeouts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinHttpRequest_INTERFACE_DEFINED__ */


#ifndef __IWinHttpRequestEvents_INTERFACE_DEFINED__
#define __IWinHttpRequestEvents_INTERFACE_DEFINED__

/* interface IWinHttpRequestEvents */
/* [unique][helpstring][nonextensible][oleautomation][uuid][object] */ 


EXTERN_C const IID IID_IWinHttpRequestEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cff7bd4c-6689-4bbe-91c2-0f559e8b88a7")
    IWinHttpRequestEvents : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE OnResponseStart( 
            /* [in] */ long Status,
            /* [in] */ BSTR ContentType) = 0;
        
        virtual void STDMETHODCALLTYPE OnResponseDataAvailable( 
            /* [in] */ SAFEARRAY * *Data) = 0;
        
        virtual void STDMETHODCALLTYPE OnResponseFinished( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinHttpRequestEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinHttpRequestEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinHttpRequestEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinHttpRequestEvents * This);
        
        void ( STDMETHODCALLTYPE *OnResponseStart )( 
            IWinHttpRequestEvents * This,
            /* [in] */ long Status,
            /* [in] */ BSTR ContentType);
        
        void ( STDMETHODCALLTYPE *OnResponseDataAvailable )( 
            IWinHttpRequestEvents * This,
            /* [in] */ SAFEARRAY * *Data);
        
        void ( STDMETHODCALLTYPE *OnResponseFinished )( 
            IWinHttpRequestEvents * This);
        
        END_INTERFACE
    } IWinHttpRequestEventsVtbl;

    interface IWinHttpRequestEvents
    {
        CONST_VTBL struct IWinHttpRequestEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinHttpRequestEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinHttpRequestEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinHttpRequestEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinHttpRequestEvents_OnResponseStart(This,Status,ContentType)	\
    (This)->lpVtbl -> OnResponseStart(This,Status,ContentType)

#define IWinHttpRequestEvents_OnResponseDataAvailable(This,Data)	\
    (This)->lpVtbl -> OnResponseDataAvailable(This,Data)

#define IWinHttpRequestEvents_OnResponseFinished(This)	\
    (This)->lpVtbl -> OnResponseFinished(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



void STDMETHODCALLTYPE IWinHttpRequestEvents_OnResponseStart_Proxy( 
    IWinHttpRequestEvents * This,
    /* [in] */ long Status,
    /* [in] */ BSTR ContentType);


void __RPC_STUB IWinHttpRequestEvents_OnResponseStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IWinHttpRequestEvents_OnResponseDataAvailable_Proxy( 
    IWinHttpRequestEvents * This,
    /* [in] */ SAFEARRAY * *Data);


void __RPC_STUB IWinHttpRequestEvents_OnResponseDataAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


void STDMETHODCALLTYPE IWinHttpRequestEvents_OnResponseFinished_Proxy( 
    IWinHttpRequestEvents * This);


void __RPC_STUB IWinHttpRequestEvents_OnResponseFinished_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinHttpRequestEvents_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WinHttpRequest;

#ifdef __cplusplus

class DECLSPEC_UUID("91e2ead3-ab7e-4d5c-88de-f7fa382172bc")
WinHttpRequest;
#endif
#endif /* __WinHttp_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


