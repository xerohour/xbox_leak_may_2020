/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Nov 20 16:29:19 1998
 */
/* Compiler settings for .\vcc.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifndef __vcc_h__
#define __vcc_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IATLPropertyBrowser_FWD_DEFINED__
#define __IATLPropertyBrowser_FWD_DEFINED__
typedef interface IATLPropertyBrowser IATLPropertyBrowser;
#endif 	/* __IATLPropertyBrowser_FWD_DEFINED__ */


#ifndef __ATLPropertyBrowser_FWD_DEFINED__
#define __ATLPropertyBrowser_FWD_DEFINED__

#ifdef __cplusplus
typedef class ATLPropertyBrowser ATLPropertyBrowser;
#else
typedef struct ATLPropertyBrowser ATLPropertyBrowser;
#endif /* __cplusplus */

#endif 	/* __ATLPropertyBrowser_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IATLPropertyBrowser_INTERFACE_DEFINED__
#define __IATLPropertyBrowser_INTERFACE_DEFINED__

/* interface IATLPropertyBrowser */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IATLPropertyBrowser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A144CF60-B137-11d1-AB87-004005352C49")
    IATLPropertyBrowser : public IDispatch
    {
    public:
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Dispatch( 
            /* [in] */ IDispatch __RPC_FAR *pDispatch) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Dispatch( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowDescription( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowDescription( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IATLPropertyBrowserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IATLPropertyBrowser __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IATLPropertyBrowser __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Dispatch )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pDispatch);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Dispatch )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowDescription )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowDescription )( 
            IATLPropertyBrowser __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IATLPropertyBrowserVtbl;

    interface IATLPropertyBrowser
    {
        CONST_VTBL struct IATLPropertyBrowserVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IATLPropertyBrowser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IATLPropertyBrowser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IATLPropertyBrowser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IATLPropertyBrowser_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IATLPropertyBrowser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IATLPropertyBrowser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IATLPropertyBrowser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IATLPropertyBrowser_put_Dispatch(This,pDispatch)	\
    (This)->lpVtbl -> put_Dispatch(This,pDispatch)

#define IATLPropertyBrowser_get_Dispatch(This,ppDispatch)	\
    (This)->lpVtbl -> get_Dispatch(This,ppDispatch)

#define IATLPropertyBrowser_get_ShowDescription(This,pVal)	\
    (This)->lpVtbl -> get_ShowDescription(This,pVal)

#define IATLPropertyBrowser_put_ShowDescription(This,newVal)	\
    (This)->lpVtbl -> put_ShowDescription(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IATLPropertyBrowser_put_Dispatch_Proxy( 
    IATLPropertyBrowser __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pDispatch);


void __RPC_STUB IATLPropertyBrowser_put_Dispatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IATLPropertyBrowser_get_Dispatch_Proxy( 
    IATLPropertyBrowser __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);


void __RPC_STUB IATLPropertyBrowser_get_Dispatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IATLPropertyBrowser_get_ShowDescription_Proxy( 
    IATLPropertyBrowser __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IATLPropertyBrowser_get_ShowDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IATLPropertyBrowser_put_ShowDescription_Proxy( 
    IATLPropertyBrowser __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IATLPropertyBrowser_put_ShowDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IATLPropertyBrowser_INTERFACE_DEFINED__ */



#ifndef __VCCTRLSLib_LIBRARY_DEFINED__
#define __VCCTRLSLib_LIBRARY_DEFINED__

/* library VCCTRLSLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VCCTRLSLib;

EXTERN_C const CLSID CLSID_ATLPropertyBrowser;

#ifdef __cplusplus

class DECLSPEC_UUID("561955ec-49fe-11d2-aa7f-00c04f990180")
ATLPropertyBrowser;
#endif
#endif /* __VCCTRLSLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
