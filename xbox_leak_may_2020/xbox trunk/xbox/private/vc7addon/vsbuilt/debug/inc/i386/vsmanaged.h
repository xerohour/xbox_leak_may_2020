
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Jan 08 04:19:59 2002
 */
/* Compiler settings for vsmanaged.idl:
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

#ifndef __vsmanaged_h__
#define __vsmanaged_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVSMDCodeDomCreator_FWD_DEFINED__
#define __IVSMDCodeDomCreator_FWD_DEFINED__
typedef interface IVSMDCodeDomCreator IVSMDCodeDomCreator;
#endif 	/* __IVSMDCodeDomCreator_FWD_DEFINED__ */


#ifndef __IVSMDCodeDomProvider_FWD_DEFINED__
#define __IVSMDCodeDomProvider_FWD_DEFINED__
typedef interface IVSMDCodeDomProvider IVSMDCodeDomProvider;
#endif 	/* __IVSMDCodeDomProvider_FWD_DEFINED__ */


#ifndef __IVSMDDesigner_FWD_DEFINED__
#define __IVSMDDesigner_FWD_DEFINED__
typedef interface IVSMDDesigner IVSMDDesigner;
#endif 	/* __IVSMDDesigner_FWD_DEFINED__ */


#ifndef __IVSMDDesignerLoader_FWD_DEFINED__
#define __IVSMDDesignerLoader_FWD_DEFINED__
typedef interface IVSMDDesignerLoader IVSMDDesignerLoader;
#endif 	/* __IVSMDDesignerLoader_FWD_DEFINED__ */


#ifndef __IVSMDDesignerService_FWD_DEFINED__
#define __IVSMDDesignerService_FWD_DEFINED__
typedef interface IVSMDDesignerService IVSMDDesignerService;
#endif 	/* __IVSMDDesignerService_FWD_DEFINED__ */


#ifndef __IVSMDPropertyGrid_FWD_DEFINED__
#define __IVSMDPropertyGrid_FWD_DEFINED__
typedef interface IVSMDPropertyGrid IVSMDPropertyGrid;
#endif 	/* __IVSMDPropertyGrid_FWD_DEFINED__ */


#ifndef __IVSMDPropertyBrowser_FWD_DEFINED__
#define __IVSMDPropertyBrowser_FWD_DEFINED__
typedef interface IVSMDPropertyBrowser IVSMDPropertyBrowser;
#endif 	/* __IVSMDPropertyBrowser_FWD_DEFINED__ */


#ifndef __IVSMDPerPropertyBrowsing_FWD_DEFINED__
#define __IVSMDPerPropertyBrowsing_FWD_DEFINED__
typedef interface IVSMDPerPropertyBrowsing IVSMDPerPropertyBrowsing;
#endif 	/* __IVSMDPerPropertyBrowsing_FWD_DEFINED__ */


/* header files for imported files */
#include "servprov.h"
#include "vsshell.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_vsmanaged_0000 */
/* [local] */ 



enum __VSHPROPID_ASPX
    {	VSHPROPID_ProjAspxLanguage	= 5000,
	VSHPROPID_ProjAspxCodeBehindExt	= 5001,
	VSHPROPID_ProjAspxAutoEventWireup	= 5002
    } ;
extern const __declspec(selectany) GUID CLSID_VSMDResxGenerator = { 0x9abb9d6c, 0x3fe7, 0x44bb, {0xa6, 0xb1, 0xef, 0x38, 0xfd, 0xf2, 0xed, 0xdd } };


extern RPC_IF_HANDLE __MIDL_itf_vsmanaged_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_vsmanaged_0000_v0_0_s_ifspec;


#ifndef __VSManagedDesigner_LIBRARY_DEFINED__
#define __VSManagedDesigner_LIBRARY_DEFINED__

/* library VSManagedDesigner */
/* [helpstring][version][uuid] */ 

#define SID_SVSMDCodeDomProvider IID_IVSMDCodeDomProvider
#define SID_SVSMDDesignerService IID_IVSMDDesignerService
typedef 
enum _PROPERTYGRIDSORT
    {	PGSORT_NOSORT	= 0,
	PGSORT_ALPHABETICAL	= 1,
	PGSORT_CATEGORIZED	= 2
    } 	PROPERTYGRIDSORT;

typedef 
enum _PROPERTYGRIDOPTION
    {	PGOPT_HOTCOMMANDS	= 0,
	PGOPT_HELP	= 1,
	PGOPT_TOOLBAR	= 2
    } 	PROPERTYGRIDOPTION;

#define SID_SVSMDPropertyBrowser IID_IVSMDPropertyBrowser

EXTERN_C const IID LIBID_VSManagedDesigner;

#ifndef __IVSMDCodeDomCreator_INTERFACE_DEFINED__
#define __IVSMDCodeDomCreator_INTERFACE_DEFINED__

/* interface IVSMDCodeDomCreator */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDCodeDomCreator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CC03BF7-4D89-4198-8E4D-17E217CA07B2")
    IVSMDCodeDomCreator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateCodeDomProvider( 
            /* [in] */ IVsHierarchy *pHier,
            /* [in] */ VSITEMID itemid,
            /* [retval][out] */ IVSMDCodeDomProvider **ppProvider) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDCodeDomCreatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDCodeDomCreator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDCodeDomCreator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDCodeDomCreator * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateCodeDomProvider )( 
            IVSMDCodeDomCreator * This,
            /* [in] */ IVsHierarchy *pHier,
            /* [in] */ VSITEMID itemid,
            /* [retval][out] */ IVSMDCodeDomProvider **ppProvider);
        
        END_INTERFACE
    } IVSMDCodeDomCreatorVtbl;

    interface IVSMDCodeDomCreator
    {
        CONST_VTBL struct IVSMDCodeDomCreatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDCodeDomCreator_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDCodeDomCreator_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDCodeDomCreator_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDCodeDomCreator_CreateCodeDomProvider(This,pHier,itemid,ppProvider)	\
    (This)->lpVtbl -> CreateCodeDomProvider(This,pHier,itemid,ppProvider)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IVSMDCodeDomCreator_CreateCodeDomProvider_Proxy( 
    IVSMDCodeDomCreator * This,
    /* [in] */ IVsHierarchy *pHier,
    /* [in] */ VSITEMID itemid,
    /* [retval][out] */ IVSMDCodeDomProvider **ppProvider);


void __RPC_STUB IVSMDCodeDomCreator_CreateCodeDomProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDCodeDomCreator_INTERFACE_DEFINED__ */


#ifndef __IVSMDCodeDomProvider_INTERFACE_DEFINED__
#define __IVSMDCodeDomProvider_INTERFACE_DEFINED__

/* interface IVSMDCodeDomProvider */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDCodeDomProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("73E59688-C7C4-4a85-AF64-A538754784C5")
    IVSMDCodeDomProvider : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CodeDomProvider( 
            /* [retval][out] */ IDispatch **ppProvider) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDCodeDomProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDCodeDomProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDCodeDomProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDCodeDomProvider * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CodeDomProvider )( 
            IVSMDCodeDomProvider * This,
            /* [retval][out] */ IDispatch **ppProvider);
        
        END_INTERFACE
    } IVSMDCodeDomProviderVtbl;

    interface IVSMDCodeDomProvider
    {
        CONST_VTBL struct IVSMDCodeDomProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDCodeDomProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDCodeDomProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDCodeDomProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDCodeDomProvider_get_CodeDomProvider(This,ppProvider)	\
    (This)->lpVtbl -> get_CodeDomProvider(This,ppProvider)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDCodeDomProvider_get_CodeDomProvider_Proxy( 
    IVSMDCodeDomProvider * This,
    /* [retval][out] */ IDispatch **ppProvider);


void __RPC_STUB IVSMDCodeDomProvider_get_CodeDomProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDCodeDomProvider_INTERFACE_DEFINED__ */


#ifndef __IVSMDDesigner_INTERFACE_DEFINED__
#define __IVSMDDesigner_INTERFACE_DEFINED__

/* interface IVSMDDesigner */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDDesigner;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7494682A-37A0-11d2-A273-00C04F8EF4FF")
    IVSMDDesigner : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CommandGuid( 
            /* [retval][out] */ GUID *pguidCmdId) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_View( 
            /* [retval][out] */ IUnknown **pView) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SelectionContainer( 
            /* [retval][out] */ IUnknown **ppSelCon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Dispose( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Flush( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLoadError( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDDesignerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDDesigner * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDDesigner * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDDesigner * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CommandGuid )( 
            IVSMDDesigner * This,
            /* [retval][out] */ GUID *pguidCmdId);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_View )( 
            IVSMDDesigner * This,
            /* [retval][out] */ IUnknown **pView);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_SelectionContainer )( 
            IVSMDDesigner * This,
            /* [retval][out] */ IUnknown **ppSelCon);
        
        HRESULT ( STDMETHODCALLTYPE *Dispose )( 
            IVSMDDesigner * This);
        
        HRESULT ( STDMETHODCALLTYPE *Flush )( 
            IVSMDDesigner * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLoadError )( 
            IVSMDDesigner * This);
        
        END_INTERFACE
    } IVSMDDesignerVtbl;

    interface IVSMDDesigner
    {
        CONST_VTBL struct IVSMDDesignerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDDesigner_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDDesigner_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDDesigner_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDDesigner_get_CommandGuid(This,pguidCmdId)	\
    (This)->lpVtbl -> get_CommandGuid(This,pguidCmdId)

#define IVSMDDesigner_get_View(This,pView)	\
    (This)->lpVtbl -> get_View(This,pView)

#define IVSMDDesigner_get_SelectionContainer(This,ppSelCon)	\
    (This)->lpVtbl -> get_SelectionContainer(This,ppSelCon)

#define IVSMDDesigner_Dispose(This)	\
    (This)->lpVtbl -> Dispose(This)

#define IVSMDDesigner_Flush(This)	\
    (This)->lpVtbl -> Flush(This)

#define IVSMDDesigner_GetLoadError(This)	\
    (This)->lpVtbl -> GetLoadError(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDDesigner_get_CommandGuid_Proxy( 
    IVSMDDesigner * This,
    /* [retval][out] */ GUID *pguidCmdId);


void __RPC_STUB IVSMDDesigner_get_CommandGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDDesigner_get_View_Proxy( 
    IVSMDDesigner * This,
    /* [retval][out] */ IUnknown **pView);


void __RPC_STUB IVSMDDesigner_get_View_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDDesigner_get_SelectionContainer_Proxy( 
    IVSMDDesigner * This,
    /* [retval][out] */ IUnknown **ppSelCon);


void __RPC_STUB IVSMDDesigner_get_SelectionContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesigner_Dispose_Proxy( 
    IVSMDDesigner * This);


void __RPC_STUB IVSMDDesigner_Dispose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesigner_Flush_Proxy( 
    IVSMDDesigner * This);


void __RPC_STUB IVSMDDesigner_Flush_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesigner_GetLoadError_Proxy( 
    IVSMDDesigner * This);


void __RPC_STUB IVSMDDesigner_GetLoadError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDDesigner_INTERFACE_DEFINED__ */


#ifndef __IVSMDDesignerLoader_INTERFACE_DEFINED__
#define __IVSMDDesignerLoader_INTERFACE_DEFINED__

/* interface IVSMDDesignerLoader */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDDesignerLoader;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74946834-37A0-11d2-A273-00C04F8EF4FF")
    IVSMDDesignerLoader : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Dispose( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEditorCaption( 
            /* [in] */ READONLYSTATUS status,
            /* [retval][out] */ BSTR *pbstrCaption) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IServiceProvider *pSp,
            /* [in] */ IVsHierarchy *pHier,
            /* [in] */ VSITEMID itemid,
            /* [in] */ IUnknown *pDocData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBaseEditorCaption( 
            /* [in] */ LPCOLESTR pwszCaption) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDDesignerLoaderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDDesignerLoader * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDDesignerLoader * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDDesignerLoader * This);
        
        HRESULT ( STDMETHODCALLTYPE *Dispose )( 
            IVSMDDesignerLoader * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetEditorCaption )( 
            IVSMDDesignerLoader * This,
            /* [in] */ READONLYSTATUS status,
            /* [retval][out] */ BSTR *pbstrCaption);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IVSMDDesignerLoader * This,
            /* [in] */ IServiceProvider *pSp,
            /* [in] */ IVsHierarchy *pHier,
            /* [in] */ VSITEMID itemid,
            /* [in] */ IUnknown *pDocData);
        
        HRESULT ( STDMETHODCALLTYPE *SetBaseEditorCaption )( 
            IVSMDDesignerLoader * This,
            /* [in] */ LPCOLESTR pwszCaption);
        
        END_INTERFACE
    } IVSMDDesignerLoaderVtbl;

    interface IVSMDDesignerLoader
    {
        CONST_VTBL struct IVSMDDesignerLoaderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDDesignerLoader_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDDesignerLoader_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDDesignerLoader_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDDesignerLoader_Dispose(This)	\
    (This)->lpVtbl -> Dispose(This)

#define IVSMDDesignerLoader_GetEditorCaption(This,status,pbstrCaption)	\
    (This)->lpVtbl -> GetEditorCaption(This,status,pbstrCaption)

#define IVSMDDesignerLoader_Initialize(This,pSp,pHier,itemid,pDocData)	\
    (This)->lpVtbl -> Initialize(This,pSp,pHier,itemid,pDocData)

#define IVSMDDesignerLoader_SetBaseEditorCaption(This,pwszCaption)	\
    (This)->lpVtbl -> SetBaseEditorCaption(This,pwszCaption)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IVSMDDesignerLoader_Dispose_Proxy( 
    IVSMDDesignerLoader * This);


void __RPC_STUB IVSMDDesignerLoader_Dispose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerLoader_GetEditorCaption_Proxy( 
    IVSMDDesignerLoader * This,
    /* [in] */ READONLYSTATUS status,
    /* [retval][out] */ BSTR *pbstrCaption);


void __RPC_STUB IVSMDDesignerLoader_GetEditorCaption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerLoader_Initialize_Proxy( 
    IVSMDDesignerLoader * This,
    /* [in] */ IServiceProvider *pSp,
    /* [in] */ IVsHierarchy *pHier,
    /* [in] */ VSITEMID itemid,
    /* [in] */ IUnknown *pDocData);


void __RPC_STUB IVSMDDesignerLoader_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerLoader_SetBaseEditorCaption_Proxy( 
    IVSMDDesignerLoader * This,
    /* [in] */ LPCOLESTR pwszCaption);


void __RPC_STUB IVSMDDesignerLoader_SetBaseEditorCaption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDDesignerLoader_INTERFACE_DEFINED__ */


#ifndef __IVSMDDesignerService_INTERFACE_DEFINED__
#define __IVSMDDesignerService_INTERFACE_DEFINED__

/* interface IVSMDDesignerService */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDDesignerService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74946829-37A0-11d2-A273-00C04F8EF4FF")
    IVSMDDesignerService : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DesignViewAttribute( 
            /* [retval][out] */ BSTR *pbstrAttribute) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDesigner( 
            /* [in] */ IServiceProvider *pSp,
            /* [in] */ IUnknown *pDesignerLoader,
            /* [retval][out] */ IVSMDDesigner **ppDesigner) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDesignerForClass( 
            /* [in] */ IServiceProvider *pSp,
            /* [in] */ LPCOLESTR pwszComponentClass,
            /* [retval][out] */ IVSMDDesigner **ppDesigner) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateDesignerLoader( 
            /* [in] */ LPCOLESTR pwszCodeStreamClass,
            /* [retval][out] */ IUnknown **ppCodeStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDesignerLoaderClassForFile( 
            /* [in] */ LPCOLESTR pwszFileName,
            /* [retval][out] */ BSTR *pbstrDesignerLoaderClass) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterDesignViewAttribute( 
            /* [in] */ IVsHierarchy *pHier,
            /* [in] */ VSITEMID itemid,
            /* [in] */ int dwClass,
            /* [in] */ LPOLESTR pwszAttributeValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDDesignerServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDDesignerService * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDDesignerService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDDesignerService * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_DesignViewAttribute )( 
            IVSMDDesignerService * This,
            /* [retval][out] */ BSTR *pbstrAttribute);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDesigner )( 
            IVSMDDesignerService * This,
            /* [in] */ IServiceProvider *pSp,
            /* [in] */ IUnknown *pDesignerLoader,
            /* [retval][out] */ IVSMDDesigner **ppDesigner);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDesignerForClass )( 
            IVSMDDesignerService * This,
            /* [in] */ IServiceProvider *pSp,
            /* [in] */ LPCOLESTR pwszComponentClass,
            /* [retval][out] */ IVSMDDesigner **ppDesigner);
        
        HRESULT ( STDMETHODCALLTYPE *CreateDesignerLoader )( 
            IVSMDDesignerService * This,
            /* [in] */ LPCOLESTR pwszCodeStreamClass,
            /* [retval][out] */ IUnknown **ppCodeStream);
        
        HRESULT ( STDMETHODCALLTYPE *GetDesignerLoaderClassForFile )( 
            IVSMDDesignerService * This,
            /* [in] */ LPCOLESTR pwszFileName,
            /* [retval][out] */ BSTR *pbstrDesignerLoaderClass);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterDesignViewAttribute )( 
            IVSMDDesignerService * This,
            /* [in] */ IVsHierarchy *pHier,
            /* [in] */ VSITEMID itemid,
            /* [in] */ int dwClass,
            /* [in] */ LPOLESTR pwszAttributeValue);
        
        END_INTERFACE
    } IVSMDDesignerServiceVtbl;

    interface IVSMDDesignerService
    {
        CONST_VTBL struct IVSMDDesignerServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDDesignerService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDDesignerService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDDesignerService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDDesignerService_get_DesignViewAttribute(This,pbstrAttribute)	\
    (This)->lpVtbl -> get_DesignViewAttribute(This,pbstrAttribute)

#define IVSMDDesignerService_CreateDesigner(This,pSp,pDesignerLoader,ppDesigner)	\
    (This)->lpVtbl -> CreateDesigner(This,pSp,pDesignerLoader,ppDesigner)

#define IVSMDDesignerService_CreateDesignerForClass(This,pSp,pwszComponentClass,ppDesigner)	\
    (This)->lpVtbl -> CreateDesignerForClass(This,pSp,pwszComponentClass,ppDesigner)

#define IVSMDDesignerService_CreateDesignerLoader(This,pwszCodeStreamClass,ppCodeStream)	\
    (This)->lpVtbl -> CreateDesignerLoader(This,pwszCodeStreamClass,ppCodeStream)

#define IVSMDDesignerService_GetDesignerLoaderClassForFile(This,pwszFileName,pbstrDesignerLoaderClass)	\
    (This)->lpVtbl -> GetDesignerLoaderClassForFile(This,pwszFileName,pbstrDesignerLoaderClass)

#define IVSMDDesignerService_RegisterDesignViewAttribute(This,pHier,itemid,dwClass,pwszAttributeValue)	\
    (This)->lpVtbl -> RegisterDesignViewAttribute(This,pHier,itemid,dwClass,pwszAttributeValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDDesignerService_get_DesignViewAttribute_Proxy( 
    IVSMDDesignerService * This,
    /* [retval][out] */ BSTR *pbstrAttribute);


void __RPC_STUB IVSMDDesignerService_get_DesignViewAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerService_CreateDesigner_Proxy( 
    IVSMDDesignerService * This,
    /* [in] */ IServiceProvider *pSp,
    /* [in] */ IUnknown *pDesignerLoader,
    /* [retval][out] */ IVSMDDesigner **ppDesigner);


void __RPC_STUB IVSMDDesignerService_CreateDesigner_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerService_CreateDesignerForClass_Proxy( 
    IVSMDDesignerService * This,
    /* [in] */ IServiceProvider *pSp,
    /* [in] */ LPCOLESTR pwszComponentClass,
    /* [retval][out] */ IVSMDDesigner **ppDesigner);


void __RPC_STUB IVSMDDesignerService_CreateDesignerForClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerService_CreateDesignerLoader_Proxy( 
    IVSMDDesignerService * This,
    /* [in] */ LPCOLESTR pwszCodeStreamClass,
    /* [retval][out] */ IUnknown **ppCodeStream);


void __RPC_STUB IVSMDDesignerService_CreateDesignerLoader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerService_GetDesignerLoaderClassForFile_Proxy( 
    IVSMDDesignerService * This,
    /* [in] */ LPCOLESTR pwszFileName,
    /* [retval][out] */ BSTR *pbstrDesignerLoaderClass);


void __RPC_STUB IVSMDDesignerService_GetDesignerLoaderClassForFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDDesignerService_RegisterDesignViewAttribute_Proxy( 
    IVSMDDesignerService * This,
    /* [in] */ IVsHierarchy *pHier,
    /* [in] */ VSITEMID itemid,
    /* [in] */ int dwClass,
    /* [in] */ LPOLESTR pwszAttributeValue);


void __RPC_STUB IVSMDDesignerService_RegisterDesignViewAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDDesignerService_INTERFACE_DEFINED__ */


#ifndef __IVSMDPropertyGrid_INTERFACE_DEFINED__
#define __IVSMDPropertyGrid_INTERFACE_DEFINED__

/* interface IVSMDPropertyGrid */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDPropertyGrid;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74946837-37A0-11d2-A273-00C04F8EF4FF")
    IVSMDPropertyGrid : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CommandsVisible( 
            /* [retval][out] */ VARIANT_BOOL *pfVisible) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Handle( 
            /* [retval][out] */ HWND *phwnd) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_GridSort( 
            /* [retval][out] */ PROPERTYGRIDSORT *pSort) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_GridSort( 
            /* [in] */ PROPERTYGRIDSORT sort) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SelectedPropertyName( 
            /* [retval][out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Dispose( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOption( 
            /* [in] */ PROPERTYGRIDOPTION option,
            /* [retval][out] */ VARIANT *pvtOption) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOption( 
            /* [in] */ PROPERTYGRIDOPTION option,
            /* [in] */ VARIANT vtOption) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSelectedObjects( 
            int cObjects,
            /* [size_is][in] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDPropertyGridVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDPropertyGrid * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDPropertyGrid * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDPropertyGrid * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CommandsVisible )( 
            IVSMDPropertyGrid * This,
            /* [retval][out] */ VARIANT_BOOL *pfVisible);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Handle )( 
            IVSMDPropertyGrid * This,
            /* [retval][out] */ HWND *phwnd);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_GridSort )( 
            IVSMDPropertyGrid * This,
            /* [retval][out] */ PROPERTYGRIDSORT *pSort);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_GridSort )( 
            IVSMDPropertyGrid * This,
            /* [in] */ PROPERTYGRIDSORT sort);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_SelectedPropertyName )( 
            IVSMDPropertyGrid * This,
            /* [retval][out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *Dispose )( 
            IVSMDPropertyGrid * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetOption )( 
            IVSMDPropertyGrid * This,
            /* [in] */ PROPERTYGRIDOPTION option,
            /* [retval][out] */ VARIANT *pvtOption);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IVSMDPropertyGrid * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetOption )( 
            IVSMDPropertyGrid * This,
            /* [in] */ PROPERTYGRIDOPTION option,
            /* [in] */ VARIANT vtOption);
        
        HRESULT ( STDMETHODCALLTYPE *SetSelectedObjects )( 
            IVSMDPropertyGrid * This,
            int cObjects,
            /* [size_is][in] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } IVSMDPropertyGridVtbl;

    interface IVSMDPropertyGrid
    {
        CONST_VTBL struct IVSMDPropertyGridVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDPropertyGrid_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDPropertyGrid_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDPropertyGrid_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDPropertyGrid_get_CommandsVisible(This,pfVisible)	\
    (This)->lpVtbl -> get_CommandsVisible(This,pfVisible)

#define IVSMDPropertyGrid_get_Handle(This,phwnd)	\
    (This)->lpVtbl -> get_Handle(This,phwnd)

#define IVSMDPropertyGrid_get_GridSort(This,pSort)	\
    (This)->lpVtbl -> get_GridSort(This,pSort)

#define IVSMDPropertyGrid_put_GridSort(This,sort)	\
    (This)->lpVtbl -> put_GridSort(This,sort)

#define IVSMDPropertyGrid_get_SelectedPropertyName(This,pbstrName)	\
    (This)->lpVtbl -> get_SelectedPropertyName(This,pbstrName)

#define IVSMDPropertyGrid_Dispose(This)	\
    (This)->lpVtbl -> Dispose(This)

#define IVSMDPropertyGrid_GetOption(This,option,pvtOption)	\
    (This)->lpVtbl -> GetOption(This,option,pvtOption)

#define IVSMDPropertyGrid_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IVSMDPropertyGrid_SetOption(This,option,vtOption)	\
    (This)->lpVtbl -> SetOption(This,option,vtOption)

#define IVSMDPropertyGrid_SetSelectedObjects(This,cObjects,ppUnk)	\
    (This)->lpVtbl -> SetSelectedObjects(This,cObjects,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_get_CommandsVisible_Proxy( 
    IVSMDPropertyGrid * This,
    /* [retval][out] */ VARIANT_BOOL *pfVisible);


void __RPC_STUB IVSMDPropertyGrid_get_CommandsVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_get_Handle_Proxy( 
    IVSMDPropertyGrid * This,
    /* [retval][out] */ HWND *phwnd);


void __RPC_STUB IVSMDPropertyGrid_get_Handle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_get_GridSort_Proxy( 
    IVSMDPropertyGrid * This,
    /* [retval][out] */ PROPERTYGRIDSORT *pSort);


void __RPC_STUB IVSMDPropertyGrid_get_GridSort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_put_GridSort_Proxy( 
    IVSMDPropertyGrid * This,
    /* [in] */ PROPERTYGRIDSORT sort);


void __RPC_STUB IVSMDPropertyGrid_put_GridSort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_get_SelectedPropertyName_Proxy( 
    IVSMDPropertyGrid * This,
    /* [retval][out] */ BSTR *pbstrName);


void __RPC_STUB IVSMDPropertyGrid_get_SelectedPropertyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_Dispose_Proxy( 
    IVSMDPropertyGrid * This);


void __RPC_STUB IVSMDPropertyGrid_Dispose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_GetOption_Proxy( 
    IVSMDPropertyGrid * This,
    /* [in] */ PROPERTYGRIDOPTION option,
    /* [retval][out] */ VARIANT *pvtOption);


void __RPC_STUB IVSMDPropertyGrid_GetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_Refresh_Proxy( 
    IVSMDPropertyGrid * This);


void __RPC_STUB IVSMDPropertyGrid_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_SetOption_Proxy( 
    IVSMDPropertyGrid * This,
    /* [in] */ PROPERTYGRIDOPTION option,
    /* [in] */ VARIANT vtOption);


void __RPC_STUB IVSMDPropertyGrid_SetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyGrid_SetSelectedObjects_Proxy( 
    IVSMDPropertyGrid * This,
    int cObjects,
    /* [size_is][in] */ IUnknown **ppUnk);


void __RPC_STUB IVSMDPropertyGrid_SetSelectedObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDPropertyGrid_INTERFACE_DEFINED__ */


#ifndef __IVSMDPropertyBrowser_INTERFACE_DEFINED__
#define __IVSMDPropertyBrowser_INTERFACE_DEFINED__

/* interface IVSMDPropertyBrowser */
/* [uuid][object] */ 


EXTERN_C const IID IID_IVSMDPropertyBrowser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74946810-37A0-11d2-A273-00C04F8EF4FF")
    IVSMDPropertyBrowser : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_WindowGlyphResourceID( 
            /* [retval][out] */ DWORD *pdwResID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreatePropertyGrid( 
            /* [retval][out] */ IVSMDPropertyGrid **ppGrid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDPropertyBrowserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDPropertyBrowser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDPropertyBrowser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDPropertyBrowser * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_WindowGlyphResourceID )( 
            IVSMDPropertyBrowser * This,
            /* [retval][out] */ DWORD *pdwResID);
        
        HRESULT ( STDMETHODCALLTYPE *CreatePropertyGrid )( 
            IVSMDPropertyBrowser * This,
            /* [retval][out] */ IVSMDPropertyGrid **ppGrid);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IVSMDPropertyBrowser * This);
        
        END_INTERFACE
    } IVSMDPropertyBrowserVtbl;

    interface IVSMDPropertyBrowser
    {
        CONST_VTBL struct IVSMDPropertyBrowserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDPropertyBrowser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDPropertyBrowser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDPropertyBrowser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDPropertyBrowser_get_WindowGlyphResourceID(This,pdwResID)	\
    (This)->lpVtbl -> get_WindowGlyphResourceID(This,pdwResID)

#define IVSMDPropertyBrowser_CreatePropertyGrid(This,ppGrid)	\
    (This)->lpVtbl -> CreatePropertyGrid(This,ppGrid)

#define IVSMDPropertyBrowser_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget] */ HRESULT STDMETHODCALLTYPE IVSMDPropertyBrowser_get_WindowGlyphResourceID_Proxy( 
    IVSMDPropertyBrowser * This,
    /* [retval][out] */ DWORD *pdwResID);


void __RPC_STUB IVSMDPropertyBrowser_get_WindowGlyphResourceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyBrowser_CreatePropertyGrid_Proxy( 
    IVSMDPropertyBrowser * This,
    /* [retval][out] */ IVSMDPropertyGrid **ppGrid);


void __RPC_STUB IVSMDPropertyBrowser_CreatePropertyGrid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVSMDPropertyBrowser_Refresh_Proxy( 
    IVSMDPropertyBrowser * This);


void __RPC_STUB IVSMDPropertyBrowser_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDPropertyBrowser_INTERFACE_DEFINED__ */


#ifndef __IVSMDPerPropertyBrowsing_INTERFACE_DEFINED__
#define __IVSMDPerPropertyBrowsing_INTERFACE_DEFINED__

/* interface IVSMDPerPropertyBrowsing */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IVSMDPerPropertyBrowsing;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7494683C-37A0-11d2-A273-00C04F8EF4FF")
    IVSMDPerPropertyBrowsing : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPropertyAttributes( 
            DISPID dispid,
            /* [out] */ UINT *pceltAttrs,
            /* [size_is][size_is][out] */ BSTR **ppbstrTypeNames,
            /* [size_is][size_is][out] */ VARIANT **ppvarAttrValues) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVSMDPerPropertyBrowsingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVSMDPerPropertyBrowsing * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVSMDPerPropertyBrowsing * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVSMDPerPropertyBrowsing * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPropertyAttributes )( 
            IVSMDPerPropertyBrowsing * This,
            DISPID dispid,
            /* [out] */ UINT *pceltAttrs,
            /* [size_is][size_is][out] */ BSTR **ppbstrTypeNames,
            /* [size_is][size_is][out] */ VARIANT **ppvarAttrValues);
        
        END_INTERFACE
    } IVSMDPerPropertyBrowsingVtbl;

    interface IVSMDPerPropertyBrowsing
    {
        CONST_VTBL struct IVSMDPerPropertyBrowsingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVSMDPerPropertyBrowsing_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVSMDPerPropertyBrowsing_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVSMDPerPropertyBrowsing_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVSMDPerPropertyBrowsing_GetPropertyAttributes(This,dispid,pceltAttrs,ppbstrTypeNames,ppvarAttrValues)	\
    (This)->lpVtbl -> GetPropertyAttributes(This,dispid,pceltAttrs,ppbstrTypeNames,ppvarAttrValues)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVSMDPerPropertyBrowsing_GetPropertyAttributes_Proxy( 
    IVSMDPerPropertyBrowsing * This,
    DISPID dispid,
    /* [out] */ UINT *pceltAttrs,
    /* [size_is][size_is][out] */ BSTR **ppbstrTypeNames,
    /* [size_is][size_is][out] */ VARIANT **ppvarAttrValues);


void __RPC_STUB IVSMDPerPropertyBrowsing_GetPropertyAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVSMDPerPropertyBrowsing_INTERFACE_DEFINED__ */

#endif /* __VSManagedDesigner_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


