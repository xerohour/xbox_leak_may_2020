
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0338 */
/* at Fri Feb 16 07:54:24 2001
 */
/* Compiler settings for atlpkg.idl:
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

#ifndef __atlpkg_h__
#define __atlpkg_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAtlpkgService_FWD_DEFINED__
#define __IAtlpkgService_FWD_DEFINED__
typedef interface IAtlpkgService IAtlpkgService;
#endif 	/* __IAtlpkgService_FWD_DEFINED__ */


#ifndef __IAtlpkgDocHandler_FWD_DEFINED__
#define __IAtlpkgDocHandler_FWD_DEFINED__
typedef interface IAtlpkgDocHandler IAtlpkgDocHandler;
#endif 	/* __IAtlpkgDocHandler_FWD_DEFINED__ */


#ifndef __atlpkgPackage_FWD_DEFINED__
#define __atlpkgPackage_FWD_DEFINED__

#ifdef __cplusplus
typedef class atlpkgPackage atlpkgPackage;
#else
typedef struct atlpkgPackage atlpkgPackage;
#endif /* __cplusplus */

#endif 	/* __atlpkgPackage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "vsshell.h"
#include "mshtml.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IAtlpkgService_INTERFACE_DEFINED__
#define __IAtlpkgService_INTERFACE_DEFINED__

/* interface IAtlpkgService */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_IAtlpkgService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("44BC8227-0C52-4b6d-825C-AA05CC812DE3")
    IAtlpkgService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDocumentHandler( 
            /* [in] */ BSTR szPath,
            /* [out] */ IOleCommandTarget **ppCmdTarget) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAtlpkgServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAtlpkgService * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAtlpkgService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAtlpkgService * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentHandler )( 
            IAtlpkgService * This,
            /* [in] */ BSTR szPath,
            /* [out] */ IOleCommandTarget **ppCmdTarget);
        
        END_INTERFACE
    } IAtlpkgServiceVtbl;

    interface IAtlpkgService
    {
        CONST_VTBL struct IAtlpkgServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAtlpkgService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAtlpkgService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAtlpkgService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAtlpkgService_GetDocumentHandler(This,szPath,ppCmdTarget)	\
    (This)->lpVtbl -> GetDocumentHandler(This,szPath,ppCmdTarget)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAtlpkgService_GetDocumentHandler_Proxy( 
    IAtlpkgService * This,
    /* [in] */ BSTR szPath,
    /* [out] */ IOleCommandTarget **ppCmdTarget);


void __RPC_STUB IAtlpkgService_GetDocumentHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAtlpkgService_INTERFACE_DEFINED__ */


#ifndef __IAtlpkgDocHandler_INTERFACE_DEFINED__
#define __IAtlpkgDocHandler_INTERFACE_DEFINED__

/* interface IAtlpkgDocHandler */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_IAtlpkgDocHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6AE2D7A7-5D22-4bf9-A96F-F6309D87B6A6")
    IAtlpkgDocHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetDocTextView( 
            /* [in] */ IVsTextView *pView) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHTMLDoc( 
            /* [in] */ IHTMLDocument2 *pDoc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AtlQueryStatus( 
            /* [in] */ ULONG cCmds,
            /* [in] */ OLECMD prgCmds[  ],
            /* [in] */ short nEditorType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateSubContext( 
            /* [in] */ IVsUserContext *pContext,
            /* [in] */ short nEditorType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAtlpkgDocHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAtlpkgDocHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAtlpkgDocHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAtlpkgDocHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetDocTextView )( 
            IAtlpkgDocHandler * This,
            /* [in] */ IVsTextView *pView);
        
        HRESULT ( STDMETHODCALLTYPE *SetHTMLDoc )( 
            IAtlpkgDocHandler * This,
            /* [in] */ IHTMLDocument2 *pDoc);
        
        HRESULT ( STDMETHODCALLTYPE *AtlQueryStatus )( 
            IAtlpkgDocHandler * This,
            /* [in] */ ULONG cCmds,
            /* [in] */ OLECMD prgCmds[  ],
            /* [in] */ short nEditorType);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateSubContext )( 
            IAtlpkgDocHandler * This,
            /* [in] */ IVsUserContext *pContext,
            /* [in] */ short nEditorType);
        
        END_INTERFACE
    } IAtlpkgDocHandlerVtbl;

    interface IAtlpkgDocHandler
    {
        CONST_VTBL struct IAtlpkgDocHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAtlpkgDocHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAtlpkgDocHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAtlpkgDocHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAtlpkgDocHandler_SetDocTextView(This,pView)	\
    (This)->lpVtbl -> SetDocTextView(This,pView)

#define IAtlpkgDocHandler_SetHTMLDoc(This,pDoc)	\
    (This)->lpVtbl -> SetHTMLDoc(This,pDoc)

#define IAtlpkgDocHandler_AtlQueryStatus(This,cCmds,prgCmds,nEditorType)	\
    (This)->lpVtbl -> AtlQueryStatus(This,cCmds,prgCmds,nEditorType)

#define IAtlpkgDocHandler_UpdateSubContext(This,pContext,nEditorType)	\
    (This)->lpVtbl -> UpdateSubContext(This,pContext,nEditorType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAtlpkgDocHandler_SetDocTextView_Proxy( 
    IAtlpkgDocHandler * This,
    /* [in] */ IVsTextView *pView);


void __RPC_STUB IAtlpkgDocHandler_SetDocTextView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAtlpkgDocHandler_SetHTMLDoc_Proxy( 
    IAtlpkgDocHandler * This,
    /* [in] */ IHTMLDocument2 *pDoc);


void __RPC_STUB IAtlpkgDocHandler_SetHTMLDoc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAtlpkgDocHandler_AtlQueryStatus_Proxy( 
    IAtlpkgDocHandler * This,
    /* [in] */ ULONG cCmds,
    /* [in] */ OLECMD prgCmds[  ],
    /* [in] */ short nEditorType);


void __RPC_STUB IAtlpkgDocHandler_AtlQueryStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAtlpkgDocHandler_UpdateSubContext_Proxy( 
    IAtlpkgDocHandler * This,
    /* [in] */ IVsUserContext *pContext,
    /* [in] */ short nEditorType);


void __RPC_STUB IAtlpkgDocHandler_UpdateSubContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAtlpkgDocHandler_INTERFACE_DEFINED__ */



#ifndef __atlpkgLib_LIBRARY_DEFINED__
#define __atlpkgLib_LIBRARY_DEFINED__

/* library atlpkgLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_atlpkgLib;

EXTERN_C const CLSID CLSID_atlpkgPackage;

#ifdef __cplusplus

class DECLSPEC_UUID("6B8ED2AF-529A-4c81-9E94-045A3371346E")
atlpkgPackage;
#endif
#endif /* __atlpkgLib_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_atlpkg_1078 */
/* [local] */ 

#define SID_AtlpkgService IID_IAtlpkgService
#define ATLPKG_DESIGNVIEW_ID 0
#define ATLPKG_HTMLVIEW_ID 1


extern RPC_IF_HANDLE __MIDL_itf_atlpkg_1078_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_atlpkg_1078_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


