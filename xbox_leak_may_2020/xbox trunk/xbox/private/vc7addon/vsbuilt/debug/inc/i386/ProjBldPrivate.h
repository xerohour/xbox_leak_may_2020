
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Jan 21 14:46:34 2002
 */
/* Compiler settings for projbldprivate.idl:
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

#ifndef __projbldprivate_h__
#define __projbldprivate_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVCCfg_FWD_DEFINED__
#define __IVCCfg_FWD_DEFINED__
typedef interface IVCCfg IVCCfg;
#endif 	/* __IVCCfg_FWD_DEFINED__ */


#ifndef __File_FWD_DEFINED__
#define __File_FWD_DEFINED__
typedef interface File File;
#endif 	/* __File_FWD_DEFINED__ */


#ifndef __Filter_FWD_DEFINED__
#define __Filter_FWD_DEFINED__
typedef interface Filter Filter;
#endif 	/* __Filter_FWD_DEFINED__ */


#ifndef ___VCProject_FWD_DEFINED__
#define ___VCProject_FWD_DEFINED__
typedef interface _VCProject _VCProject;
#endif 	/* ___VCProject_FWD_DEFINED__ */


#ifndef __FileNodeProps_FWD_DEFINED__
#define __FileNodeProps_FWD_DEFINED__

#ifdef __cplusplus
typedef class FileNodeProps FileNodeProps;
#else
typedef struct FileNodeProps FileNodeProps;
#endif /* __cplusplus */

#endif 	/* __FileNodeProps_FWD_DEFINED__ */


#ifndef __FolderNodeProps_FWD_DEFINED__
#define __FolderNodeProps_FWD_DEFINED__

#ifdef __cplusplus
typedef class FolderNodeProps FolderNodeProps;
#else
typedef struct FolderNodeProps FolderNodeProps;
#endif /* __cplusplus */

#endif 	/* __FolderNodeProps_FWD_DEFINED__ */


#ifndef __ProjectNodeProps_FWD_DEFINED__
#define __ProjectNodeProps_FWD_DEFINED__

#ifdef __cplusplus
typedef class ProjectNodeProps ProjectNodeProps;
#else
typedef struct ProjectNodeProps ProjectNodeProps;
#endif /* __cplusplus */

#endif 	/* __ProjectNodeProps_FWD_DEFINED__ */


#ifndef __BuildPackage_FWD_DEFINED__
#define __BuildPackage_FWD_DEFINED__

#ifdef __cplusplus
typedef class BuildPackage BuildPackage;
#else
typedef struct BuildPackage BuildPackage;
#endif /* __cplusplus */

#endif 	/* __BuildPackage_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_projbldprivate_0000 */
/* [local] */ 


extern "C" const __declspec(selectany) GUID SID_SProjectEngine = { 0x531ADA6BL,0x0EEA,0x11D3,0x8D,0x31,0x00,0xC0,0x4F,0x8E,0xEA,0x30 };
extern "C" const __declspec(selectany) GUID SID_SENCBuildSpawner = { 0x39642745L,0x908C,0x11D3,0xB9,0x9A,0x00,0xC0,0x4F,0x68,0x5D,0x26 };
extern "C" const __declspec(selectany) GUID SID_SBuildErrorContext = { 0x39642745L,0x908C,0x11D3,0xB9,0x9A,0x00,0xC0,0x4F,0x68,0x5D,0x27 };
extern "C" const __declspec(selectany) GUID GUID_ItemType_VC = { 0x31df8af1L,0xe859,0x43eb,0xac,0xfb,0x23,0x8d,0x73,0x74,0x27,0x1f };

enum __VCPROPID
    {	VCPROPID_NIL	= -1,
	VCPROPID_FIRST	= 1000,
	VCPROPID_ProjBldObject	= 1000,
	VCPROPID_ProjBldProject	= 1001,
	VCPROPID_ProjBldActiveProjConfig	= 1002,
	VCPROPID_ProjBldActiveFileConfig	= 1003,
	VCPROPID_ProjBldIncludePath	= 1004,
	VCPROPID_ProjBldBscFile	= 1005,
	VCPROPID_LAST	= 1005
    } ;
typedef LONG VCPROPID;



extern RPC_IF_HANDLE __MIDL_itf_projbldprivate_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_projbldprivate_0000_v0_0_s_ifspec;

#ifndef __IVCCfg_INTERFACE_DEFINED__
#define __IVCCfg_INTERFACE_DEFINED__

/* interface IVCCfg */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVCCfg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("694C76BC-3EF4-11d3-B278-0050041DB12A")
    IVCCfg : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE get_Tool( 
            /* [in] */ VARIANT toolIndex,
            /* [retval][out] */ IUnknown **ppTool) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE get_Object( 
            /* [retval][out] */ IDispatch **ppObj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVCCfgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVCCfg * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVCCfg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVCCfg * This);
        
        HRESULT ( STDMETHODCALLTYPE *get_Tool )( 
            IVCCfg * This,
            /* [in] */ VARIANT toolIndex,
            /* [retval][out] */ IUnknown **ppTool);
        
        HRESULT ( STDMETHODCALLTYPE *get_Object )( 
            IVCCfg * This,
            /* [retval][out] */ IDispatch **ppObj);
        
        END_INTERFACE
    } IVCCfgVtbl;

    interface IVCCfg
    {
        CONST_VTBL struct IVCCfgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVCCfg_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVCCfg_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVCCfg_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVCCfg_get_Tool(This,toolIndex,ppTool)	\
    (This)->lpVtbl -> get_Tool(This,toolIndex,ppTool)

#define IVCCfg_get_Object(This,ppObj)	\
    (This)->lpVtbl -> get_Object(This,ppObj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IVCCfg_get_Tool_Proxy( 
    IVCCfg * This,
    /* [in] */ VARIANT toolIndex,
    /* [retval][out] */ IUnknown **ppTool);


void __RPC_STUB IVCCfg_get_Tool_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCCfg_get_Object_Proxy( 
    IVCCfg * This,
    /* [retval][out] */ IDispatch **ppObj);


void __RPC_STUB IVCCfg_get_Object_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVCCfg_INTERFACE_DEFINED__ */


#ifndef __File_INTERFACE_DEFINED__
#define __File_INTERFACE_DEFINED__

/* interface File */
/* [unique][helpstringcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_File;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2D5D650E-3E0D-11d3-9415-00C04F68FB62")
    File : public IDispatch
    {
    public:
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_FullPath( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_RelativePath( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_RelativePath( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_DeploymentContent( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_DeploymentContent( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_Extender( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_ExtenderNames( 
            /* [retval][out] */ VARIANT *pvarNames) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_ExtenderCATID( 
            /* [retval][out] */ BSTR *pbstrGuid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FileVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            File * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            File * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            File * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            File * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            File * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            File * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            File * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            File * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FullPath )( 
            File * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_RelativePath )( 
            File * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_RelativePath )( 
            File * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DeploymentContent )( 
            File * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DeploymentContent )( 
            File * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Extender )( 
            File * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtenderNames )( 
            File * This,
            /* [retval][out] */ VARIANT *pvarNames);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtenderCATID )( 
            File * This,
            /* [retval][out] */ BSTR *pbstrGuid);
        
        END_INTERFACE
    } FileVtbl;

    interface File
    {
        CONST_VTBL struct FileVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define File_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define File_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define File_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define File_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define File_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define File_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define File_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define File_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define File_get_FullPath(This,pVal)	\
    (This)->lpVtbl -> get_FullPath(This,pVal)

#define File_get_RelativePath(This,pVal)	\
    (This)->lpVtbl -> get_RelativePath(This,pVal)

#define File_put_RelativePath(This,newVal)	\
    (This)->lpVtbl -> put_RelativePath(This,newVal)

#define File_get_DeploymentContent(This,pVal)	\
    (This)->lpVtbl -> get_DeploymentContent(This,pVal)

#define File_put_DeploymentContent(This,newVal)	\
    (This)->lpVtbl -> put_DeploymentContent(This,newVal)

#define File_get_Extender(This,bstrName,ppDisp)	\
    (This)->lpVtbl -> get_Extender(This,bstrName,ppDisp)

#define File_get_ExtenderNames(This,pvarNames)	\
    (This)->lpVtbl -> get_ExtenderNames(This,pvarNames)

#define File_get_ExtenderCATID(This,pbstrGuid)	\
    (This)->lpVtbl -> get_ExtenderCATID(This,pbstrGuid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE File_get_Name_Proxy( 
    File * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB File_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE File_get_FullPath_Proxy( 
    File * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB File_get_FullPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE File_get_RelativePath_Proxy( 
    File * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB File_get_RelativePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE File_put_RelativePath_Proxy( 
    File * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB File_put_RelativePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE File_get_DeploymentContent_Proxy( 
    File * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB File_get_DeploymentContent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE File_put_DeploymentContent_Proxy( 
    File * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB File_put_DeploymentContent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE File_get_Extender_Proxy( 
    File * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB File_get_Extender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE File_get_ExtenderNames_Proxy( 
    File * This,
    /* [retval][out] */ VARIANT *pvarNames);


void __RPC_STUB File_get_ExtenderNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE File_get_ExtenderCATID_Proxy( 
    File * This,
    /* [retval][out] */ BSTR *pbstrGuid);


void __RPC_STUB File_get_ExtenderCATID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __File_INTERFACE_DEFINED__ */


#ifndef __Filter_INTERFACE_DEFINED__
#define __Filter_INTERFACE_DEFINED__

/* interface Filter */
/* [unique][helpstringcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_Filter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2111B8B8-4150-11d3-9416-00C04F68FB62")
    Filter : public IDispatch
    {
    public:
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Filter( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Filter( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParseFiles( 
            /* [retval][out] */ VARIANT_BOOL *pbParse) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParseFiles( 
            /* [in] */ VARIANT_BOOL bParse) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_SourceControlFiles( 
            /* [retval][out] */ VARIANT_BOOL *pbSCC) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_SourceControlFiles( 
            /* [in] */ VARIANT_BOOL bSCC) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_Extender( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_ExtenderNames( 
            /* [retval][out] */ VARIANT *pvarNames) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_ExtenderCATID( 
            /* [retval][out] */ BSTR *pbstrGuid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Filter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Filter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Filter * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Filter * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Filter * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Filter * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Filter * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            Filter * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            Filter * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Filter )( 
            Filter * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Filter )( 
            Filter * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParseFiles )( 
            Filter * This,
            /* [retval][out] */ VARIANT_BOOL *pbParse);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ParseFiles )( 
            Filter * This,
            /* [in] */ VARIANT_BOOL bParse);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SourceControlFiles )( 
            Filter * This,
            /* [retval][out] */ VARIANT_BOOL *pbSCC);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SourceControlFiles )( 
            Filter * This,
            /* [in] */ VARIANT_BOOL bSCC);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Extender )( 
            Filter * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtenderNames )( 
            Filter * This,
            /* [retval][out] */ VARIANT *pvarNames);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtenderCATID )( 
            Filter * This,
            /* [retval][out] */ BSTR *pbstrGuid);
        
        END_INTERFACE
    } FilterVtbl;

    interface Filter
    {
        CONST_VTBL struct FilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Filter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Filter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Filter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Filter_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Filter_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Filter_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Filter_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Filter_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define Filter_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define Filter_get_Filter(This,pVal)	\
    (This)->lpVtbl -> get_Filter(This,pVal)

#define Filter_put_Filter(This,newVal)	\
    (This)->lpVtbl -> put_Filter(This,newVal)

#define Filter_get_ParseFiles(This,pbParse)	\
    (This)->lpVtbl -> get_ParseFiles(This,pbParse)

#define Filter_put_ParseFiles(This,bParse)	\
    (This)->lpVtbl -> put_ParseFiles(This,bParse)

#define Filter_get_SourceControlFiles(This,pbSCC)	\
    (This)->lpVtbl -> get_SourceControlFiles(This,pbSCC)

#define Filter_put_SourceControlFiles(This,bSCC)	\
    (This)->lpVtbl -> put_SourceControlFiles(This,bSCC)

#define Filter_get_Extender(This,bstrName,ppDisp)	\
    (This)->lpVtbl -> get_Extender(This,bstrName,ppDisp)

#define Filter_get_ExtenderNames(This,pvarNames)	\
    (This)->lpVtbl -> get_ExtenderNames(This,pvarNames)

#define Filter_get_ExtenderCATID(This,pbstrGuid)	\
    (This)->lpVtbl -> get_ExtenderCATID(This,pbstrGuid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_Name_Proxy( 
    Filter * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB Filter_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Filter_put_Name_Proxy( 
    Filter * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB Filter_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_Filter_Proxy( 
    Filter * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB Filter_get_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Filter_put_Filter_Proxy( 
    Filter * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB Filter_put_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_ParseFiles_Proxy( 
    Filter * This,
    /* [retval][out] */ VARIANT_BOOL *pbParse);


void __RPC_STUB Filter_get_ParseFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Filter_put_ParseFiles_Proxy( 
    Filter * This,
    /* [in] */ VARIANT_BOOL bParse);


void __RPC_STUB Filter_put_ParseFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_SourceControlFiles_Proxy( 
    Filter * This,
    /* [retval][out] */ VARIANT_BOOL *pbSCC);


void __RPC_STUB Filter_get_SourceControlFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE Filter_put_SourceControlFiles_Proxy( 
    Filter * This,
    /* [in] */ VARIANT_BOOL bSCC);


void __RPC_STUB Filter_put_SourceControlFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_Extender_Proxy( 
    Filter * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB Filter_get_Extender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_ExtenderNames_Proxy( 
    Filter * This,
    /* [retval][out] */ VARIANT *pvarNames);


void __RPC_STUB Filter_get_ExtenderNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE Filter_get_ExtenderCATID_Proxy( 
    Filter * This,
    /* [retval][out] */ BSTR *pbstrGuid);


void __RPC_STUB Filter_get_ExtenderCATID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Filter_INTERFACE_DEFINED__ */


#ifndef ___VCProject_INTERFACE_DEFINED__
#define ___VCProject_INTERFACE_DEFINED__

/* interface _VCProject */
/* [unique][helpstringcontext][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID__VCProject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2111B8BA-4150-11d3-9416-00C04F68FB62")
    _VCProject : public IDispatch
    {
    public:
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProjectFile( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProjectDependencies( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_Extender( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_ExtenderNames( 
            /* [retval][out] */ VARIANT *pvarNames) = 0;
        
        virtual /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE get_ExtenderCATID( 
            /* [retval][out] */ BSTR *pbstrGuid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct _VCProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _VCProject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _VCProject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _VCProject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _VCProject * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _VCProject * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _VCProject * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _VCProject * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            _VCProject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            _VCProject * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProjectFile )( 
            _VCProject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ProjectDependencies )( 
            _VCProject * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Extender )( 
            _VCProject * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtenderNames )( 
            _VCProject * This,
            /* [retval][out] */ VARIANT *pvarNames);
        
        /* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ExtenderCATID )( 
            _VCProject * This,
            /* [retval][out] */ BSTR *pbstrGuid);
        
        END_INTERFACE
    } _VCProjectVtbl;

    interface _VCProject
    {
        CONST_VTBL struct _VCProjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _VCProject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _VCProject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _VCProject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _VCProject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _VCProject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _VCProject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _VCProject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define _VCProject_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define _VCProject_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define _VCProject_get_ProjectFile(This,pVal)	\
    (This)->lpVtbl -> get_ProjectFile(This,pVal)

#define _VCProject_get_ProjectDependencies(This,pVal)	\
    (This)->lpVtbl -> get_ProjectDependencies(This,pVal)

#define _VCProject_get_Extender(This,bstrName,ppDisp)	\
    (This)->lpVtbl -> get_Extender(This,bstrName,ppDisp)

#define _VCProject_get_ExtenderNames(This,pvarNames)	\
    (This)->lpVtbl -> get_ExtenderNames(This,pvarNames)

#define _VCProject_get_ExtenderCATID(This,pbstrGuid)	\
    (This)->lpVtbl -> get_ExtenderCATID(This,pbstrGuid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _VCProject_get_Name_Proxy( 
    _VCProject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB _VCProject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propput] */ HRESULT STDMETHODCALLTYPE _VCProject_put_Name_Proxy( 
    _VCProject * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB _VCProject_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _VCProject_get_ProjectFile_Proxy( 
    _VCProject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB _VCProject_get_ProjectFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][propget] */ HRESULT STDMETHODCALLTYPE _VCProject_get_ProjectDependencies_Proxy( 
    _VCProject * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB _VCProject_get_ProjectDependencies_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE _VCProject_get_Extender_Proxy( 
    _VCProject * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB _VCProject_get_Extender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE _VCProject_get_ExtenderNames_Proxy( 
    _VCProject * This,
    /* [retval][out] */ VARIANT *pvarNames);


void __RPC_STUB _VCProject_get_ExtenderNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstringcontext][id][hidden][nonbrowsable][propget] */ HRESULT STDMETHODCALLTYPE _VCProject_get_ExtenderCATID_Proxy( 
    _VCProject * This,
    /* [retval][out] */ BSTR *pbstrGuid);


void __RPC_STUB _VCProject_get_ExtenderCATID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* ___VCProject_INTERFACE_DEFINED__ */



#ifndef __VCPrivateProjectLibrary_LIBRARY_DEFINED__
#define __VCPrivateProjectLibrary_LIBRARY_DEFINED__

/* library VCPrivateProjectLibrary */
/* [helpstringdll][helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_VCPrivateProjectLibrary;

EXTERN_C const CLSID CLSID_FileNodeProps;

#ifdef __cplusplus

class DECLSPEC_UUID("2D5D650F-3E0D-11d3-9415-00C04F68FB62")
FileNodeProps;
#endif

EXTERN_C const CLSID CLSID_FolderNodeProps;

#ifdef __cplusplus

class DECLSPEC_UUID("2111B8B9-4150-11d3-9416-00C04F68FB62")
FolderNodeProps;
#endif

EXTERN_C const CLSID CLSID_ProjectNodeProps;

#ifdef __cplusplus

class DECLSPEC_UUID("2111B8BB-4150-11d3-9416-00C04F68FB62")
ProjectNodeProps;
#endif

EXTERN_C const CLSID CLSID_BuildPackage;

#ifdef __cplusplus

class DECLSPEC_UUID("F1C25864-3097-11D2-A5C5-00C04F7968B4")
BuildPackage;
#endif
#endif /* __VCPrivateProjectLibrary_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


