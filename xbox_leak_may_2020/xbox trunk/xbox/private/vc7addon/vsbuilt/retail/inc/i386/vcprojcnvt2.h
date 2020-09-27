
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun Jan 27 21:49:32 2002
 */
/* Compiler settings for vcprojcnvt2.idl:
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

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __vcprojcnvt2_h__
#define __vcprojcnvt2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVCProjConvert_FWD_DEFINED__
#define __IVCProjConvert_FWD_DEFINED__
typedef interface IVCProjConvert IVCProjConvert;
#endif 	/* __IVCProjConvert_FWD_DEFINED__ */


#ifndef __VCProjConvert_FWD_DEFINED__
#define __VCProjConvert_FWD_DEFINED__

#ifdef __cplusplus
typedef class VCProjConvert VCProjConvert;
#else
typedef struct VCProjConvert VCProjConvert;
#endif /* __cplusplus */

#endif 	/* __VCProjConvert_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IVCProjConvert_INTERFACE_DEFINED__
#define __IVCProjConvert_INTERFACE_DEFINED__

/* interface IVCProjConvert */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVCProjConvert;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7D8AE8C4-3FD3-11D3-8D52-00C04F8ECDB6")
    IVCProjConvert : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenProject( 
            BSTR bstrProjectPath,
            IUnknown *pProjEngine,
            IUnknown *pUnkServiceProvider) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            BSTR bstrSourceFilesString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPackage( 
            long **ppPackage) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProject( 
            IDispatch **ppProject) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HasProjectDependencies( 
            /* [retval][out] */ VARIANT_BOOL *pbHasDeps) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProjectDependenciesCount( 
            /* [retval][out] */ long *plNumDeps) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNextProjectDependency( 
            /* [out] */ BSTR *pbstrProject,
            /* [out] */ BSTR *pbstrDependentProject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetProjectDependencyEnum( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVCProjConvertVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVCProjConvert * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVCProjConvert * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVCProjConvert * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenProject )( 
            IVCProjConvert * This,
            BSTR bstrProjectPath,
            IUnknown *pProjEngine,
            IUnknown *pUnkServiceProvider);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IVCProjConvert * This,
            BSTR bstrSourceFilesString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPackage )( 
            IVCProjConvert * This,
            long **ppPackage);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProject )( 
            IVCProjConvert * This,
            IDispatch **ppProject);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasProjectDependencies )( 
            IVCProjConvert * This,
            /* [retval][out] */ VARIANT_BOOL *pbHasDeps);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProjectDependenciesCount )( 
            IVCProjConvert * This,
            /* [retval][out] */ long *plNumDeps);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNextProjectDependency )( 
            IVCProjConvert * This,
            /* [out] */ BSTR *pbstrProject,
            /* [out] */ BSTR *pbstrDependentProject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetProjectDependencyEnum )( 
            IVCProjConvert * This);
        
        END_INTERFACE
    } IVCProjConvertVtbl;

    interface IVCProjConvert
    {
        CONST_VTBL struct IVCProjConvertVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVCProjConvert_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVCProjConvert_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVCProjConvert_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVCProjConvert_OpenProject(This,bstrProjectPath,pProjEngine,pUnkServiceProvider)	\
    (This)->lpVtbl -> OpenProject(This,bstrProjectPath,pProjEngine,pUnkServiceProvider)

#define IVCProjConvert_Initialize(This,bstrSourceFilesString)	\
    (This)->lpVtbl -> Initialize(This,bstrSourceFilesString)

#define IVCProjConvert_GetPackage(This,ppPackage)	\
    (This)->lpVtbl -> GetPackage(This,ppPackage)

#define IVCProjConvert_GetProject(This,ppProject)	\
    (This)->lpVtbl -> GetProject(This,ppProject)

#define IVCProjConvert_get_HasProjectDependencies(This,pbHasDeps)	\
    (This)->lpVtbl -> get_HasProjectDependencies(This,pbHasDeps)

#define IVCProjConvert_get_ProjectDependenciesCount(This,plNumDeps)	\
    (This)->lpVtbl -> get_ProjectDependenciesCount(This,plNumDeps)

#define IVCProjConvert_GetNextProjectDependency(This,pbstrProject,pbstrDependentProject)	\
    (This)->lpVtbl -> GetNextProjectDependency(This,pbstrProject,pbstrDependentProject)

#define IVCProjConvert_ResetProjectDependencyEnum(This)	\
    (This)->lpVtbl -> ResetProjectDependencyEnum(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_OpenProject_Proxy( 
    IVCProjConvert * This,
    BSTR bstrProjectPath,
    IUnknown *pProjEngine,
    IUnknown *pUnkServiceProvider);


void __RPC_STUB IVCProjConvert_OpenProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_Initialize_Proxy( 
    IVCProjConvert * This,
    BSTR bstrSourceFilesString);


void __RPC_STUB IVCProjConvert_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_GetPackage_Proxy( 
    IVCProjConvert * This,
    long **ppPackage);


void __RPC_STUB IVCProjConvert_GetPackage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_GetProject_Proxy( 
    IVCProjConvert * This,
    IDispatch **ppProject);


void __RPC_STUB IVCProjConvert_GetProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_get_HasProjectDependencies_Proxy( 
    IVCProjConvert * This,
    /* [retval][out] */ VARIANT_BOOL *pbHasDeps);


void __RPC_STUB IVCProjConvert_get_HasProjectDependencies_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_get_ProjectDependenciesCount_Proxy( 
    IVCProjConvert * This,
    /* [retval][out] */ long *plNumDeps);


void __RPC_STUB IVCProjConvert_get_ProjectDependenciesCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_GetNextProjectDependency_Proxy( 
    IVCProjConvert * This,
    /* [out] */ BSTR *pbstrProject,
    /* [out] */ BSTR *pbstrDependentProject);


void __RPC_STUB IVCProjConvert_GetNextProjectDependency_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IVCProjConvert_ResetProjectDependencyEnum_Proxy( 
    IVCProjConvert * This);


void __RPC_STUB IVCProjConvert_ResetProjectDependencyEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVCProjConvert_INTERFACE_DEFINED__ */



#ifndef __VCPROJCNVTLib_LIBRARY_DEFINED__
#define __VCPROJCNVTLib_LIBRARY_DEFINED__

/* library VCPROJCNVTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VCPROJCNVTLib;

EXTERN_C const CLSID CLSID_VCProjConvert;

#ifdef __cplusplus

class DECLSPEC_UUID("7D8AE8C5-3FD3-11D3-8D52-00C04F8ECDB6")
VCProjConvert;
#endif
#endif /* __VCPROJCNVTLib_LIBRARY_DEFINED__ */

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


