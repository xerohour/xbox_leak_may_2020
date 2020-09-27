
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun Jan 27 21:49:06 2002
 */
/* Compiler settings for ProjBld2.idl:
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

#ifndef __ProjBld2_h__
#define __ProjBld2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVCBuildOptionsPage_FWD_DEFINED__
#define __IVCBuildOptionsPage_FWD_DEFINED__
typedef interface IVCBuildOptionsPage IVCBuildOptionsPage;
#endif 	/* __IVCBuildOptionsPage_FWD_DEFINED__ */


#ifndef __VCProjectConfigurationProperties_FWD_DEFINED__
#define __VCProjectConfigurationProperties_FWD_DEFINED__
typedef interface VCProjectConfigurationProperties VCProjectConfigurationProperties;
#endif 	/* __VCProjectConfigurationProperties_FWD_DEFINED__ */


#ifndef __VCFileConfigurationProperties_FWD_DEFINED__
#define __VCFileConfigurationProperties_FWD_DEFINED__
typedef interface VCFileConfigurationProperties VCFileConfigurationProperties;
#endif 	/* __VCFileConfigurationProperties_FWD_DEFINED__ */


#ifndef __VCProjectConfigurationProperties_FWD_DEFINED__
#define __VCProjectConfigurationProperties_FWD_DEFINED__
typedef interface VCProjectConfigurationProperties VCProjectConfigurationProperties;
#endif 	/* __VCProjectConfigurationProperties_FWD_DEFINED__ */


#ifndef __VCFileConfigurationProperties_FWD_DEFINED__
#define __VCFileConfigurationProperties_FWD_DEFINED__
typedef interface VCFileConfigurationProperties VCFileConfigurationProperties;
#endif 	/* __VCFileConfigurationProperties_FWD_DEFINED__ */


#ifndef __IVCBuildOptionsPage_FWD_DEFINED__
#define __IVCBuildOptionsPage_FWD_DEFINED__
typedef interface IVCBuildOptionsPage IVCBuildOptionsPage;
#endif 	/* __IVCBuildOptionsPage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "vcpbobjects.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IVCBuildOptionsPage_INTERFACE_DEFINED__
#define __IVCBuildOptionsPage_INTERFACE_DEFINED__

/* interface IVCBuildOptionsPage */
/* [unique][dual][uuid][object] */ 


EXTERN_C const IID IID_IVCBuildOptionsPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9F367026-33C6-11D3-8D52-00C04F8ECDB7")
    IVCBuildOptionsPage : public IDispatch
    {
    public:
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_BuildLogging( 
            /* [retval][out] */ VARIANT_BOOL *pbLog) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_BuildLogging( 
            /* [in] */ VARIANT_BOOL bLog) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_BuildTiming( 
            /* [retval][out] */ VARIANT_BOOL *pbTime) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_BuildTiming( 
            /* [in] */ VARIANT_BOOL bTime) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CPPExtensions( 
            /* [retval][out] */ BSTR *pbstrExtensions) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_CPPExtensions( 
            /* [in] */ BSTR bstrExtensions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVCBuildOptionsPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVCBuildOptionsPage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVCBuildOptionsPage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVCBuildOptionsPage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVCBuildOptionsPage * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVCBuildOptionsPage * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVCBuildOptionsPage * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVCBuildOptionsPage * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BuildLogging )( 
            IVCBuildOptionsPage * This,
            /* [retval][out] */ VARIANT_BOOL *pbLog);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BuildLogging )( 
            IVCBuildOptionsPage * This,
            /* [in] */ VARIANT_BOOL bLog);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BuildTiming )( 
            IVCBuildOptionsPage * This,
            /* [retval][out] */ VARIANT_BOOL *pbTime);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BuildTiming )( 
            IVCBuildOptionsPage * This,
            /* [in] */ VARIANT_BOOL bTime);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CPPExtensions )( 
            IVCBuildOptionsPage * This,
            /* [retval][out] */ BSTR *pbstrExtensions);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CPPExtensions )( 
            IVCBuildOptionsPage * This,
            /* [in] */ BSTR bstrExtensions);
        
        END_INTERFACE
    } IVCBuildOptionsPageVtbl;

    interface IVCBuildOptionsPage
    {
        CONST_VTBL struct IVCBuildOptionsPageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVCBuildOptionsPage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVCBuildOptionsPage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVCBuildOptionsPage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVCBuildOptionsPage_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVCBuildOptionsPage_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVCBuildOptionsPage_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVCBuildOptionsPage_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVCBuildOptionsPage_get_BuildLogging(This,pbLog)	\
    (This)->lpVtbl -> get_BuildLogging(This,pbLog)

#define IVCBuildOptionsPage_put_BuildLogging(This,bLog)	\
    (This)->lpVtbl -> put_BuildLogging(This,bLog)

#define IVCBuildOptionsPage_get_BuildTiming(This,pbTime)	\
    (This)->lpVtbl -> get_BuildTiming(This,pbTime)

#define IVCBuildOptionsPage_put_BuildTiming(This,bTime)	\
    (This)->lpVtbl -> put_BuildTiming(This,bTime)

#define IVCBuildOptionsPage_get_CPPExtensions(This,pbstrExtensions)	\
    (This)->lpVtbl -> get_CPPExtensions(This,pbstrExtensions)

#define IVCBuildOptionsPage_put_CPPExtensions(This,bstrExtensions)	\
    (This)->lpVtbl -> put_CPPExtensions(This,bstrExtensions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IVCBuildOptionsPage_get_BuildLogging_Proxy( 
    IVCBuildOptionsPage * This,
    /* [retval][out] */ VARIANT_BOOL *pbLog);


void __RPC_STUB IVCBuildOptionsPage_get_BuildLogging_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IVCBuildOptionsPage_put_BuildLogging_Proxy( 
    IVCBuildOptionsPage * This,
    /* [in] */ VARIANT_BOOL bLog);


void __RPC_STUB IVCBuildOptionsPage_put_BuildLogging_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IVCBuildOptionsPage_get_BuildTiming_Proxy( 
    IVCBuildOptionsPage * This,
    /* [retval][out] */ VARIANT_BOOL *pbTime);


void __RPC_STUB IVCBuildOptionsPage_get_BuildTiming_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IVCBuildOptionsPage_put_BuildTiming_Proxy( 
    IVCBuildOptionsPage * This,
    /* [in] */ VARIANT_BOOL bTime);


void __RPC_STUB IVCBuildOptionsPage_put_BuildTiming_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE IVCBuildOptionsPage_get_CPPExtensions_Proxy( 
    IVCBuildOptionsPage * This,
    /* [retval][out] */ BSTR *pbstrExtensions);


void __RPC_STUB IVCBuildOptionsPage_get_CPPExtensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE IVCBuildOptionsPage_put_CPPExtensions_Proxy( 
    IVCBuildOptionsPage * This,
    /* [in] */ BSTR bstrExtensions);


void __RPC_STUB IVCBuildOptionsPage_put_CPPExtensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVCBuildOptionsPage_INTERFACE_DEFINED__ */


#ifndef __VCProjectConfigurationProperties_INTERFACE_DEFINED__
#define __VCProjectConfigurationProperties_INTERFACE_DEFINED__

/* interface VCProjectConfigurationProperties */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_VCProjectConfigurationProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("24DAA1C6-298A-4320-BC4B-C36B3F259E40")
    VCProjectConfigurationProperties : public IDispatch
    {
    public:
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_OutputDirectory( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_OutputDirectory( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_IntermediateDirectory( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_IntermediateDirectory( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConfigurationType( 
            /* [retval][out] */ ConfigurationTypes *ConfigType) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConfigurationType( 
            /* [in] */ ConfigurationTypes ConfigType) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_BuildBrowserInformation( 
            /* [retval][out] */ VARIANT_BOOL *Bsc) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_BuildBrowserInformation( 
            /* [in] */ VARIANT_BOOL Bsc) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_UseOfMFC( 
            /* [retval][out] */ useOfMfc *useMfc) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_UseOfMFC( 
            /* [in] */ useOfMfc useMfc) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_UseOfATL( 
            /* [retval][out] */ useOfATL *useATL) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_UseOfATL( 
            /* [in] */ useOfATL useATL) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ATLMinimizesCRunTimeLibraryUsage( 
            /* [retval][out] */ VARIANT_BOOL *UseCRT) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ATLMinimizesCRunTimeLibraryUsage( 
            /* [in] */ VARIANT_BOOL UseCRT) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CharacterSet( 
            /* [retval][out] */ charSet *optSetting) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_CharacterSet( 
            /* [in] */ charSet optSetting) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ManagedExtensions( 
            /* [retval][out] */ VARIANT_BOOL *Managed) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ManagedExtensions( 
            /* [in] */ VARIANT_BOOL Managed) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeleteExtensionsOnClean( 
            /* [retval][out] */ BSTR *Ext) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DeleteExtensionsOnClean( 
            /* [in] */ BSTR Ext) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_WholeProgramOptimization( 
            /* [retval][out] */ VARIANT_BOOL *Optimize) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_WholeProgramOptimization( 
            /* [in] */ VARIANT_BOOL Optimize) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Command( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Command( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_WorkingDirectory( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_WorkingDirectory( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_CommandArguments( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_CommandArguments( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Attach( 
            /* [retval][out] */ VARIANT_BOOL *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Attach( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Remote( 
            /* [retval][out] */ RemoteDebuggerType *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Remote( 
            /* [in] */ RemoteDebuggerType newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteMachine( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_RemoteMachine( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DebuggerType( 
            /* [retval][out] */ TypeOfDebugger *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DebuggerType( 
            /* [in] */ TypeOfDebugger newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_RemoteCommand( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_RemoteCommand( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_HttpUrl( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_HttpUrl( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_PDBPath( 
            /* [retval][out] */ BSTR *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_PDBPath( 
            /* [in] */ BSTR NewVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SQLDebugging( 
            /* [retval][out] */ VARIANT_BOOL *Val) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SQLDebugging( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct VCProjectConfigurationPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            VCProjectConfigurationProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            VCProjectConfigurationProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            VCProjectConfigurationProperties * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OutputDirectory )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_OutputDirectory )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IntermediateDirectory )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_IntermediateDirectory )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ConfigurationType )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ ConfigurationTypes *ConfigType);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ConfigurationType )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ ConfigurationTypes ConfigType);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_BuildBrowserInformation )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *Bsc);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_BuildBrowserInformation )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL Bsc);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UseOfMFC )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ useOfMfc *useMfc);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UseOfMFC )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ useOfMfc useMfc);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UseOfATL )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ useOfATL *useATL);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UseOfATL )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ useOfATL useATL);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ATLMinimizesCRunTimeLibraryUsage )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *UseCRT);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ATLMinimizesCRunTimeLibraryUsage )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL UseCRT);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CharacterSet )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ charSet *optSetting);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CharacterSet )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ charSet optSetting);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ManagedExtensions )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *Managed);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ManagedExtensions )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL Managed);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeleteExtensionsOnClean )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Ext);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DeleteExtensionsOnClean )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR Ext);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WholeProgramOptimization )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *Optimize);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_WholeProgramOptimization )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL Optimize);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Command )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Command )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WorkingDirectory )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_WorkingDirectory )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CommandArguments )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CommandArguments )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Attach )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Attach )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Remote )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ RemoteDebuggerType *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Remote )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ RemoteDebuggerType newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteMachine )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteMachine )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DebuggerType )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ TypeOfDebugger *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DebuggerType )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ TypeOfDebugger newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RemoteCommand )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RemoteCommand )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HttpUrl )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_HttpUrl )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PDBPath )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ BSTR *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PDBPath )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ BSTR NewVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SQLDebugging )( 
            VCProjectConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *Val);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SQLDebugging )( 
            VCProjectConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } VCProjectConfigurationPropertiesVtbl;

    interface VCProjectConfigurationProperties
    {
        CONST_VTBL struct VCProjectConfigurationPropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define VCProjectConfigurationProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define VCProjectConfigurationProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define VCProjectConfigurationProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define VCProjectConfigurationProperties_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define VCProjectConfigurationProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define VCProjectConfigurationProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define VCProjectConfigurationProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define VCProjectConfigurationProperties_get_OutputDirectory(This,Val)	\
    (This)->lpVtbl -> get_OutputDirectory(This,Val)

#define VCProjectConfigurationProperties_put_OutputDirectory(This,newVal)	\
    (This)->lpVtbl -> put_OutputDirectory(This,newVal)

#define VCProjectConfigurationProperties_get_IntermediateDirectory(This,Val)	\
    (This)->lpVtbl -> get_IntermediateDirectory(This,Val)

#define VCProjectConfigurationProperties_put_IntermediateDirectory(This,newVal)	\
    (This)->lpVtbl -> put_IntermediateDirectory(This,newVal)

#define VCProjectConfigurationProperties_get_ConfigurationType(This,ConfigType)	\
    (This)->lpVtbl -> get_ConfigurationType(This,ConfigType)

#define VCProjectConfigurationProperties_put_ConfigurationType(This,ConfigType)	\
    (This)->lpVtbl -> put_ConfigurationType(This,ConfigType)

#define VCProjectConfigurationProperties_get_BuildBrowserInformation(This,Bsc)	\
    (This)->lpVtbl -> get_BuildBrowserInformation(This,Bsc)

#define VCProjectConfigurationProperties_put_BuildBrowserInformation(This,Bsc)	\
    (This)->lpVtbl -> put_BuildBrowserInformation(This,Bsc)

#define VCProjectConfigurationProperties_get_UseOfMFC(This,useMfc)	\
    (This)->lpVtbl -> get_UseOfMFC(This,useMfc)

#define VCProjectConfigurationProperties_put_UseOfMFC(This,useMfc)	\
    (This)->lpVtbl -> put_UseOfMFC(This,useMfc)

#define VCProjectConfigurationProperties_get_UseOfATL(This,useATL)	\
    (This)->lpVtbl -> get_UseOfATL(This,useATL)

#define VCProjectConfigurationProperties_put_UseOfATL(This,useATL)	\
    (This)->lpVtbl -> put_UseOfATL(This,useATL)

#define VCProjectConfigurationProperties_get_ATLMinimizesCRunTimeLibraryUsage(This,UseCRT)	\
    (This)->lpVtbl -> get_ATLMinimizesCRunTimeLibraryUsage(This,UseCRT)

#define VCProjectConfigurationProperties_put_ATLMinimizesCRunTimeLibraryUsage(This,UseCRT)	\
    (This)->lpVtbl -> put_ATLMinimizesCRunTimeLibraryUsage(This,UseCRT)

#define VCProjectConfigurationProperties_get_CharacterSet(This,optSetting)	\
    (This)->lpVtbl -> get_CharacterSet(This,optSetting)

#define VCProjectConfigurationProperties_put_CharacterSet(This,optSetting)	\
    (This)->lpVtbl -> put_CharacterSet(This,optSetting)

#define VCProjectConfigurationProperties_get_ManagedExtensions(This,Managed)	\
    (This)->lpVtbl -> get_ManagedExtensions(This,Managed)

#define VCProjectConfigurationProperties_put_ManagedExtensions(This,Managed)	\
    (This)->lpVtbl -> put_ManagedExtensions(This,Managed)

#define VCProjectConfigurationProperties_get_DeleteExtensionsOnClean(This,Ext)	\
    (This)->lpVtbl -> get_DeleteExtensionsOnClean(This,Ext)

#define VCProjectConfigurationProperties_put_DeleteExtensionsOnClean(This,Ext)	\
    (This)->lpVtbl -> put_DeleteExtensionsOnClean(This,Ext)

#define VCProjectConfigurationProperties_get_WholeProgramOptimization(This,Optimize)	\
    (This)->lpVtbl -> get_WholeProgramOptimization(This,Optimize)

#define VCProjectConfigurationProperties_put_WholeProgramOptimization(This,Optimize)	\
    (This)->lpVtbl -> put_WholeProgramOptimization(This,Optimize)

#define VCProjectConfigurationProperties_get_Command(This,Val)	\
    (This)->lpVtbl -> get_Command(This,Val)

#define VCProjectConfigurationProperties_put_Command(This,newVal)	\
    (This)->lpVtbl -> put_Command(This,newVal)

#define VCProjectConfigurationProperties_get_WorkingDirectory(This,Val)	\
    (This)->lpVtbl -> get_WorkingDirectory(This,Val)

#define VCProjectConfigurationProperties_put_WorkingDirectory(This,newVal)	\
    (This)->lpVtbl -> put_WorkingDirectory(This,newVal)

#define VCProjectConfigurationProperties_get_CommandArguments(This,Val)	\
    (This)->lpVtbl -> get_CommandArguments(This,Val)

#define VCProjectConfigurationProperties_put_CommandArguments(This,newVal)	\
    (This)->lpVtbl -> put_CommandArguments(This,newVal)

#define VCProjectConfigurationProperties_get_Attach(This,Val)	\
    (This)->lpVtbl -> get_Attach(This,Val)

#define VCProjectConfigurationProperties_put_Attach(This,newVal)	\
    (This)->lpVtbl -> put_Attach(This,newVal)

#define VCProjectConfigurationProperties_get_Remote(This,Val)	\
    (This)->lpVtbl -> get_Remote(This,Val)

#define VCProjectConfigurationProperties_put_Remote(This,newVal)	\
    (This)->lpVtbl -> put_Remote(This,newVal)

#define VCProjectConfigurationProperties_get_RemoteMachine(This,Val)	\
    (This)->lpVtbl -> get_RemoteMachine(This,Val)

#define VCProjectConfigurationProperties_put_RemoteMachine(This,newVal)	\
    (This)->lpVtbl -> put_RemoteMachine(This,newVal)

#define VCProjectConfigurationProperties_get_DebuggerType(This,Val)	\
    (This)->lpVtbl -> get_DebuggerType(This,Val)

#define VCProjectConfigurationProperties_put_DebuggerType(This,newVal)	\
    (This)->lpVtbl -> put_DebuggerType(This,newVal)

#define VCProjectConfigurationProperties_get_RemoteCommand(This,Val)	\
    (This)->lpVtbl -> get_RemoteCommand(This,Val)

#define VCProjectConfigurationProperties_put_RemoteCommand(This,newVal)	\
    (This)->lpVtbl -> put_RemoteCommand(This,newVal)

#define VCProjectConfigurationProperties_get_HttpUrl(This,Val)	\
    (This)->lpVtbl -> get_HttpUrl(This,Val)

#define VCProjectConfigurationProperties_put_HttpUrl(This,newVal)	\
    (This)->lpVtbl -> put_HttpUrl(This,newVal)

#define VCProjectConfigurationProperties_get_PDBPath(This,Val)	\
    (This)->lpVtbl -> get_PDBPath(This,Val)

#define VCProjectConfigurationProperties_put_PDBPath(This,NewVal)	\
    (This)->lpVtbl -> put_PDBPath(This,NewVal)

#define VCProjectConfigurationProperties_get_SQLDebugging(This,Val)	\
    (This)->lpVtbl -> get_SQLDebugging(This,Val)

#define VCProjectConfigurationProperties_put_SQLDebugging(This,newVal)	\
    (This)->lpVtbl -> put_SQLDebugging(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_OutputDirectory_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_OutputDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_OutputDirectory_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_OutputDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_IntermediateDirectory_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_IntermediateDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_IntermediateDirectory_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_IntermediateDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_ConfigurationType_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ ConfigurationTypes *ConfigType);


void __RPC_STUB VCProjectConfigurationProperties_get_ConfigurationType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_ConfigurationType_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ ConfigurationTypes ConfigType);


void __RPC_STUB VCProjectConfigurationProperties_put_ConfigurationType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_BuildBrowserInformation_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *Bsc);


void __RPC_STUB VCProjectConfigurationProperties_get_BuildBrowserInformation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_BuildBrowserInformation_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL Bsc);


void __RPC_STUB VCProjectConfigurationProperties_put_BuildBrowserInformation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_UseOfMFC_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ useOfMfc *useMfc);


void __RPC_STUB VCProjectConfigurationProperties_get_UseOfMFC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_UseOfMFC_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ useOfMfc useMfc);


void __RPC_STUB VCProjectConfigurationProperties_put_UseOfMFC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_UseOfATL_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ useOfATL *useATL);


void __RPC_STUB VCProjectConfigurationProperties_get_UseOfATL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_UseOfATL_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ useOfATL useATL);


void __RPC_STUB VCProjectConfigurationProperties_put_UseOfATL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_ATLMinimizesCRunTimeLibraryUsage_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *UseCRT);


void __RPC_STUB VCProjectConfigurationProperties_get_ATLMinimizesCRunTimeLibraryUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_ATLMinimizesCRunTimeLibraryUsage_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL UseCRT);


void __RPC_STUB VCProjectConfigurationProperties_put_ATLMinimizesCRunTimeLibraryUsage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_CharacterSet_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ charSet *optSetting);


void __RPC_STUB VCProjectConfigurationProperties_get_CharacterSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_CharacterSet_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ charSet optSetting);


void __RPC_STUB VCProjectConfigurationProperties_put_CharacterSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_ManagedExtensions_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *Managed);


void __RPC_STUB VCProjectConfigurationProperties_get_ManagedExtensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_ManagedExtensions_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL Managed);


void __RPC_STUB VCProjectConfigurationProperties_put_ManagedExtensions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_DeleteExtensionsOnClean_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Ext);


void __RPC_STUB VCProjectConfigurationProperties_get_DeleteExtensionsOnClean_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_DeleteExtensionsOnClean_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR Ext);


void __RPC_STUB VCProjectConfigurationProperties_put_DeleteExtensionsOnClean_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_WholeProgramOptimization_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *Optimize);


void __RPC_STUB VCProjectConfigurationProperties_get_WholeProgramOptimization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_WholeProgramOptimization_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL Optimize);


void __RPC_STUB VCProjectConfigurationProperties_put_WholeProgramOptimization_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_Command_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_Command_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_WorkingDirectory_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_WorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_WorkingDirectory_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_WorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_CommandArguments_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_CommandArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_CommandArguments_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_CommandArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_Attach_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_Attach_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_Remote_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ RemoteDebuggerType *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_Remote_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_Remote_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ RemoteDebuggerType newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_Remote_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_RemoteMachine_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_RemoteMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_RemoteMachine_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_RemoteMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_DebuggerType_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ TypeOfDebugger *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_DebuggerType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_DebuggerType_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ TypeOfDebugger newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_DebuggerType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_RemoteCommand_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_RemoteCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_RemoteCommand_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_RemoteCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_HttpUrl_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_HttpUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_HttpUrl_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_HttpUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_PDBPath_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ BSTR *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_PDBPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_PDBPath_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ BSTR NewVal);


void __RPC_STUB VCProjectConfigurationProperties_put_PDBPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_get_SQLDebugging_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *Val);


void __RPC_STUB VCProjectConfigurationProperties_get_SQLDebugging_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCProjectConfigurationProperties_put_SQLDebugging_Proxy( 
    VCProjectConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB VCProjectConfigurationProperties_put_SQLDebugging_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __VCProjectConfigurationProperties_INTERFACE_DEFINED__ */


#ifndef __VCFileConfigurationProperties_INTERFACE_DEFINED__
#define __VCFileConfigurationProperties_INTERFACE_DEFINED__

/* interface VCFileConfigurationProperties */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_VCFileConfigurationProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("24DAA1C7-298A-4320-BC4B-C36B3F259E40")
    VCFileConfigurationProperties : public IDispatch
    {
    public:
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExcludedFromBuild( 
            /* [retval][out] */ VARIANT_BOOL *ExcludedFromBuild) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ExcludedFromBuild( 
            /* [in] */ VARIANT_BOOL ExcludedFromBuild) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct VCFileConfigurationPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            VCFileConfigurationProperties * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            VCFileConfigurationProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            VCFileConfigurationProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            VCFileConfigurationProperties * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            VCFileConfigurationProperties * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            VCFileConfigurationProperties * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            VCFileConfigurationProperties * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExcludedFromBuild )( 
            VCFileConfigurationProperties * This,
            /* [retval][out] */ VARIANT_BOOL *ExcludedFromBuild);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ExcludedFromBuild )( 
            VCFileConfigurationProperties * This,
            /* [in] */ VARIANT_BOOL ExcludedFromBuild);
        
        END_INTERFACE
    } VCFileConfigurationPropertiesVtbl;

    interface VCFileConfigurationProperties
    {
        CONST_VTBL struct VCFileConfigurationPropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define VCFileConfigurationProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define VCFileConfigurationProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define VCFileConfigurationProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define VCFileConfigurationProperties_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define VCFileConfigurationProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define VCFileConfigurationProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define VCFileConfigurationProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define VCFileConfigurationProperties_get_ExcludedFromBuild(This,ExcludedFromBuild)	\
    (This)->lpVtbl -> get_ExcludedFromBuild(This,ExcludedFromBuild)

#define VCFileConfigurationProperties_put_ExcludedFromBuild(This,ExcludedFromBuild)	\
    (This)->lpVtbl -> put_ExcludedFromBuild(This,ExcludedFromBuild)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE VCFileConfigurationProperties_get_ExcludedFromBuild_Proxy( 
    VCFileConfigurationProperties * This,
    /* [retval][out] */ VARIANT_BOOL *ExcludedFromBuild);


void __RPC_STUB VCFileConfigurationProperties_get_ExcludedFromBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE VCFileConfigurationProperties_put_ExcludedFromBuild_Proxy( 
    VCFileConfigurationProperties * This,
    /* [in] */ VARIANT_BOOL ExcludedFromBuild);


void __RPC_STUB VCFileConfigurationProperties_put_ExcludedFromBuild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __VCFileConfigurationProperties_INTERFACE_DEFINED__ */



#ifndef __VCProjectLibrary_LIBRARY_DEFINED__
#define __VCProjectLibrary_LIBRARY_DEFINED__

/* library VCProjectLibrary */
/* [helpstringdll][helpstring][version][uuid] */ 





EXTERN_C const IID LIBID_VCProjectLibrary;
#endif /* __VCProjectLibrary_LIBRARY_DEFINED__ */

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


