
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Thu Jan 10 16:28:31 2002
 */
/* Compiler settings for vcbuild.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
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

#ifndef __vcbuild_h__
#define __vcbuild_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVCBuildErrorContext_FWD_DEFINED__
#define __IVCBuildErrorContext_FWD_DEFINED__
typedef interface IVCBuildErrorContext IVCBuildErrorContext;
#endif 	/* __IVCBuildErrorContext_FWD_DEFINED__ */


#ifndef __IENCBuildSpawner_FWD_DEFINED__
#define __IENCBuildSpawner_FWD_DEFINED__
typedef interface IENCBuildSpawner IENCBuildSpawner;
#endif 	/* __IENCBuildSpawner_FWD_DEFINED__ */


#ifndef __IENCBuildSpawnerCallback_FWD_DEFINED__
#define __IENCBuildSpawnerCallback_FWD_DEFINED__
typedef interface IENCBuildSpawnerCallback IENCBuildSpawnerCallback;
#endif 	/* __IENCBuildSpawnerCallback_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IVCBuildErrorContext_INTERFACE_DEFINED__
#define __IVCBuildErrorContext_INTERFACE_DEFINED__

/* interface IVCBuildErrorContext */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IVCBuildErrorContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A8F1429B-1E96-11D3-B96D-00C04F685D26")
    IVCBuildErrorContext : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddError( 
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrHelp,
            /* [in] */ BSTR bstrFile,
            /* [in] */ long nLine,
            /* [in] */ BSTR bstrFull) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddWarning( 
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrHelp,
            /* [in] */ BSTR bstrFile,
            /* [in] */ long nLine,
            /* [in] */ BSTR bstrFull) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddInfo( 
            /* [in] */ BSTR bstrMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddLine( 
            /* [in] */ BSTR bstrMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteLog( 
            /* [in] */ BSTR bstrMessage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ActivateWindow( 
            /* [in] */ VARIANT_BOOL bForce) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ActivateTaskList( 
            /* [in] */ VARIANT_BOOL bForce) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearWindow( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Warnings( 
            /* [retval][out] */ long *pnErr) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Errors( 
            /* [retval][out] */ long *pnWrn) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BaseDirectory( 
            /* [retval][out] */ BSTR *pbstrBaseDir) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BaseDirectory( 
            /* [in] */ BSTR bstrBaseDir) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProjectName( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ProjectName( 
            /* [in] */ BSTR bstrBaseName) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AssociatedBuildEngine( 
            /* [retval][out] */ IDispatch **ppBldEngine) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_AssociatedBuildEngine( 
            /* [in] */ IDispatch *pBldEngine) = 0;
        
        virtual /* [propget][hidden][nonbrowsable] */ HRESULT STDMETHODCALLTYPE get_ShowOutput( 
            /* [retval][out] */ VARIANT_BOOL *pbShow) = 0;
        
        virtual /* [propput][hidden][nonbrowsable] */ HRESULT STDMETHODCALLTYPE put_ShowOutput( 
            /* [in] */ VARIANT_BOOL bShow) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVCBuildErrorContextVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVCBuildErrorContext * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVCBuildErrorContext * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVCBuildErrorContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVCBuildErrorContext * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVCBuildErrorContext * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVCBuildErrorContext * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVCBuildErrorContext * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE *AddError )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrHelp,
            /* [in] */ BSTR bstrFile,
            /* [in] */ long nLine,
            /* [in] */ BSTR bstrFull);
        
        HRESULT ( STDMETHODCALLTYPE *AddWarning )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrHelp,
            /* [in] */ BSTR bstrFile,
            /* [in] */ long nLine,
            /* [in] */ BSTR bstrFull);
        
        HRESULT ( STDMETHODCALLTYPE *AddInfo )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrMessage);
        
        HRESULT ( STDMETHODCALLTYPE *AddLine )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrMessage);
        
        HRESULT ( STDMETHODCALLTYPE *WriteLog )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrMessage);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateWindow )( 
            IVCBuildErrorContext * This,
            /* [in] */ VARIANT_BOOL bForce);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateTaskList )( 
            IVCBuildErrorContext * This,
            /* [in] */ VARIANT_BOOL bForce);
        
        HRESULT ( STDMETHODCALLTYPE *ClearWindow )( 
            IVCBuildErrorContext * This);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IVCBuildErrorContext * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Warnings )( 
            IVCBuildErrorContext * This,
            /* [retval][out] */ long *pnErr);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Errors )( 
            IVCBuildErrorContext * This,
            /* [retval][out] */ long *pnWrn);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_BaseDirectory )( 
            IVCBuildErrorContext * This,
            /* [retval][out] */ BSTR *pbstrBaseDir);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_BaseDirectory )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrBaseDir);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProjectName )( 
            IVCBuildErrorContext * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ProjectName )( 
            IVCBuildErrorContext * This,
            /* [in] */ BSTR bstrBaseName);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_AssociatedBuildEngine )( 
            IVCBuildErrorContext * This,
            /* [retval][out] */ IDispatch **ppBldEngine);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_AssociatedBuildEngine )( 
            IVCBuildErrorContext * This,
            /* [in] */ IDispatch *pBldEngine);
        
        /* [propget][hidden][nonbrowsable] */ HRESULT ( STDMETHODCALLTYPE *get_ShowOutput )( 
            IVCBuildErrorContext * This,
            /* [retval][out] */ VARIANT_BOOL *pbShow);
        
        /* [propput][hidden][nonbrowsable] */ HRESULT ( STDMETHODCALLTYPE *put_ShowOutput )( 
            IVCBuildErrorContext * This,
            /* [in] */ VARIANT_BOOL bShow);
        
        END_INTERFACE
    } IVCBuildErrorContextVtbl;

    interface IVCBuildErrorContext
    {
        CONST_VTBL struct IVCBuildErrorContextVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVCBuildErrorContext_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVCBuildErrorContext_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVCBuildErrorContext_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVCBuildErrorContext_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVCBuildErrorContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVCBuildErrorContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVCBuildErrorContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVCBuildErrorContext_AddError(This,bstrMessage,bstrHelp,bstrFile,nLine,bstrFull)	\
    (This)->lpVtbl -> AddError(This,bstrMessage,bstrHelp,bstrFile,nLine,bstrFull)

#define IVCBuildErrorContext_AddWarning(This,bstrMessage,bstrHelp,bstrFile,nLine,bstrFull)	\
    (This)->lpVtbl -> AddWarning(This,bstrMessage,bstrHelp,bstrFile,nLine,bstrFull)

#define IVCBuildErrorContext_AddInfo(This,bstrMessage)	\
    (This)->lpVtbl -> AddInfo(This,bstrMessage)

#define IVCBuildErrorContext_AddLine(This,bstrMessage)	\
    (This)->lpVtbl -> AddLine(This,bstrMessage)

#define IVCBuildErrorContext_WriteLog(This,bstrMessage)	\
    (This)->lpVtbl -> WriteLog(This,bstrMessage)

#define IVCBuildErrorContext_ActivateWindow(This,bForce)	\
    (This)->lpVtbl -> ActivateWindow(This,bForce)

#define IVCBuildErrorContext_ActivateTaskList(This,bForce)	\
    (This)->lpVtbl -> ActivateTaskList(This,bForce)

#define IVCBuildErrorContext_ClearWindow(This)	\
    (This)->lpVtbl -> ClearWindow(This)

#define IVCBuildErrorContext_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IVCBuildErrorContext_get_Warnings(This,pnErr)	\
    (This)->lpVtbl -> get_Warnings(This,pnErr)

#define IVCBuildErrorContext_get_Errors(This,pnWrn)	\
    (This)->lpVtbl -> get_Errors(This,pnWrn)

#define IVCBuildErrorContext_get_BaseDirectory(This,pbstrBaseDir)	\
    (This)->lpVtbl -> get_BaseDirectory(This,pbstrBaseDir)

#define IVCBuildErrorContext_put_BaseDirectory(This,bstrBaseDir)	\
    (This)->lpVtbl -> put_BaseDirectory(This,bstrBaseDir)

#define IVCBuildErrorContext_get_ProjectName(This,pbstrName)	\
    (This)->lpVtbl -> get_ProjectName(This,pbstrName)

#define IVCBuildErrorContext_put_ProjectName(This,bstrBaseName)	\
    (This)->lpVtbl -> put_ProjectName(This,bstrBaseName)

#define IVCBuildErrorContext_get_AssociatedBuildEngine(This,ppBldEngine)	\
    (This)->lpVtbl -> get_AssociatedBuildEngine(This,ppBldEngine)

#define IVCBuildErrorContext_put_AssociatedBuildEngine(This,pBldEngine)	\
    (This)->lpVtbl -> put_AssociatedBuildEngine(This,pBldEngine)

#define IVCBuildErrorContext_get_ShowOutput(This,pbShow)	\
    (This)->lpVtbl -> get_ShowOutput(This,pbShow)

#define IVCBuildErrorContext_put_ShowOutput(This,bShow)	\
    (This)->lpVtbl -> put_ShowOutput(This,bShow)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_AddError_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrMessage,
    /* [in] */ BSTR bstrHelp,
    /* [in] */ BSTR bstrFile,
    /* [in] */ long nLine,
    /* [in] */ BSTR bstrFull);


void __RPC_STUB IVCBuildErrorContext_AddError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_AddWarning_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrMessage,
    /* [in] */ BSTR bstrHelp,
    /* [in] */ BSTR bstrFile,
    /* [in] */ long nLine,
    /* [in] */ BSTR bstrFull);


void __RPC_STUB IVCBuildErrorContext_AddWarning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_AddInfo_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrMessage);


void __RPC_STUB IVCBuildErrorContext_AddInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_AddLine_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrMessage);


void __RPC_STUB IVCBuildErrorContext_AddLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_WriteLog_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrMessage);


void __RPC_STUB IVCBuildErrorContext_WriteLog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_ActivateWindow_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ VARIANT_BOOL bForce);


void __RPC_STUB IVCBuildErrorContext_ActivateWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_ActivateTaskList_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ VARIANT_BOOL bForce);


void __RPC_STUB IVCBuildErrorContext_ActivateTaskList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_ClearWindow_Proxy( 
    IVCBuildErrorContext * This);


void __RPC_STUB IVCBuildErrorContext_ClearWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_Close_Proxy( 
    IVCBuildErrorContext * This);


void __RPC_STUB IVCBuildErrorContext_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_get_Warnings_Proxy( 
    IVCBuildErrorContext * This,
    /* [retval][out] */ long *pnErr);


void __RPC_STUB IVCBuildErrorContext_get_Warnings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_get_Errors_Proxy( 
    IVCBuildErrorContext * This,
    /* [retval][out] */ long *pnWrn);


void __RPC_STUB IVCBuildErrorContext_get_Errors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_get_BaseDirectory_Proxy( 
    IVCBuildErrorContext * This,
    /* [retval][out] */ BSTR *pbstrBaseDir);


void __RPC_STUB IVCBuildErrorContext_get_BaseDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_put_BaseDirectory_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrBaseDir);


void __RPC_STUB IVCBuildErrorContext_put_BaseDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_get_ProjectName_Proxy( 
    IVCBuildErrorContext * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IVCBuildErrorContext_get_ProjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_put_ProjectName_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ BSTR bstrBaseName);


void __RPC_STUB IVCBuildErrorContext_put_ProjectName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_get_AssociatedBuildEngine_Proxy( 
    IVCBuildErrorContext * This,
    /* [retval][out] */ IDispatch **ppBldEngine);


void __RPC_STUB IVCBuildErrorContext_get_AssociatedBuildEngine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_put_AssociatedBuildEngine_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ IDispatch *pBldEngine);


void __RPC_STUB IVCBuildErrorContext_put_AssociatedBuildEngine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][hidden][nonbrowsable] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_get_ShowOutput_Proxy( 
    IVCBuildErrorContext * This,
    /* [retval][out] */ VARIANT_BOOL *pbShow);


void __RPC_STUB IVCBuildErrorContext_get_ShowOutput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][hidden][nonbrowsable] */ HRESULT STDMETHODCALLTYPE IVCBuildErrorContext_put_ShowOutput_Proxy( 
    IVCBuildErrorContext * This,
    /* [in] */ VARIANT_BOOL bShow);


void __RPC_STUB IVCBuildErrorContext_put_ShowOutput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVCBuildErrorContext_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_vcbuild_0255 */
/* [local] */ 





extern RPC_IF_HANDLE __MIDL_itf_vcbuild_0255_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_vcbuild_0255_v0_0_s_ifspec;

#ifndef __IENCBuildSpawner_INTERFACE_DEFINED__
#define __IENCBuildSpawner_INTERFACE_DEFINED__

/* interface IENCBuildSpawner */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IENCBuildSpawner;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("66e3a41c-fdb9-11d2-b144-00c04f72dc32")
    IENCBuildSpawner : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE StartSpawn( 
            /* [in] */ LPCOLESTR szCurrentDir,
            /* [in] */ LPCOLESTR szCmdLine,
            /* [in] */ IVCBuildErrorContext *pIVCBuildErrorContext,
            /* [in] */ IENCBuildSpawnerCallback *pCallback,
            /* [in] */ BOOL fUseConsoleCP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopSpawn( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsSysHeader( 
            /* [in] */ LPCOLESTR szIncFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IENCBuildSpawnerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IENCBuildSpawner * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IENCBuildSpawner * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IENCBuildSpawner * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartSpawn )( 
            IENCBuildSpawner * This,
            /* [in] */ LPCOLESTR szCurrentDir,
            /* [in] */ LPCOLESTR szCmdLine,
            /* [in] */ IVCBuildErrorContext *pIVCBuildErrorContext,
            /* [in] */ IENCBuildSpawnerCallback *pCallback,
            /* [in] */ BOOL fUseConsoleCP);
        
        HRESULT ( STDMETHODCALLTYPE *StopSpawn )( 
            IENCBuildSpawner * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsSysHeader )( 
            IENCBuildSpawner * This,
            /* [in] */ LPCOLESTR szIncFileName);
        
        END_INTERFACE
    } IENCBuildSpawnerVtbl;

    interface IENCBuildSpawner
    {
        CONST_VTBL struct IENCBuildSpawnerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IENCBuildSpawner_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IENCBuildSpawner_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IENCBuildSpawner_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IENCBuildSpawner_StartSpawn(This,szCurrentDir,szCmdLine,pIVCBuildErrorContext,pCallback,fUseConsoleCP)	\
    (This)->lpVtbl -> StartSpawn(This,szCurrentDir,szCmdLine,pIVCBuildErrorContext,pCallback,fUseConsoleCP)

#define IENCBuildSpawner_StopSpawn(This)	\
    (This)->lpVtbl -> StopSpawn(This)

#define IENCBuildSpawner_IsSysHeader(This,szIncFileName)	\
    (This)->lpVtbl -> IsSysHeader(This,szIncFileName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IENCBuildSpawner_StartSpawn_Proxy( 
    IENCBuildSpawner * This,
    /* [in] */ LPCOLESTR szCurrentDir,
    /* [in] */ LPCOLESTR szCmdLine,
    /* [in] */ IVCBuildErrorContext *pIVCBuildErrorContext,
    /* [in] */ IENCBuildSpawnerCallback *pCallback,
    /* [in] */ BOOL fUseConsoleCP);


void __RPC_STUB IENCBuildSpawner_StartSpawn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IENCBuildSpawner_StopSpawn_Proxy( 
    IENCBuildSpawner * This);


void __RPC_STUB IENCBuildSpawner_StopSpawn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IENCBuildSpawner_IsSysHeader_Proxy( 
    IENCBuildSpawner * This,
    /* [in] */ LPCOLESTR szIncFileName);


void __RPC_STUB IENCBuildSpawner_IsSysHeader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IENCBuildSpawner_INTERFACE_DEFINED__ */


#ifndef __IENCBuildSpawnerCallback_INTERFACE_DEFINED__
#define __IENCBuildSpawnerCallback_INTERFACE_DEFINED__

/* interface IENCBuildSpawnerCallback */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IENCBuildSpawnerCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4ce8c2c0-fdbb-11d2-b144-00c04f72dc32")
    IENCBuildSpawnerCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Done( 
            /* [in] */ HRESULT hrFromBuild,
            /* [in] */ DWORD dwReportedErrors,
            /* [in] */ DWORD dwReportedWarnings) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IENCBuildSpawnerCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IENCBuildSpawnerCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IENCBuildSpawnerCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IENCBuildSpawnerCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *Done )( 
            IENCBuildSpawnerCallback * This,
            /* [in] */ HRESULT hrFromBuild,
            /* [in] */ DWORD dwReportedErrors,
            /* [in] */ DWORD dwReportedWarnings);
        
        END_INTERFACE
    } IENCBuildSpawnerCallbackVtbl;

    interface IENCBuildSpawnerCallback
    {
        CONST_VTBL struct IENCBuildSpawnerCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IENCBuildSpawnerCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IENCBuildSpawnerCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IENCBuildSpawnerCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IENCBuildSpawnerCallback_Done(This,hrFromBuild,dwReportedErrors,dwReportedWarnings)	\
    (This)->lpVtbl -> Done(This,hrFromBuild,dwReportedErrors,dwReportedWarnings)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IENCBuildSpawnerCallback_Done_Proxy( 
    IENCBuildSpawnerCallback * This,
    /* [in] */ HRESULT hrFromBuild,
    /* [in] */ DWORD dwReportedErrors,
    /* [in] */ DWORD dwReportedWarnings);


void __RPC_STUB IENCBuildSpawnerCallback_Done_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IENCBuildSpawnerCallback_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


