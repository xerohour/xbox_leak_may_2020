/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.59 */
/* at Wed Dec 11 12:17:38 1996
 */
/* Compiler settings for dtgapi.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __dtgapi_h__
#define __dtgapi_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IDataService_FWD_DEFINED__
#define __IDataService_FWD_DEFINED__
typedef interface IDataService IDataService;
#endif 	/* __IDataService_FWD_DEFINED__ */


#ifndef __IDispDataService_FWD_DEFINED__
#define __IDispDataService_FWD_DEFINED__
typedef interface IDispDataService IDispDataService;
#endif 	/* __IDispDataService_FWD_DEFINED__ */


/* header files for imported files */
#include "oleidl.h"
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_dtgapi_0000
 * at Wed Dec 11 12:17:38 1996
 * using MIDL 3.01.59
 ****************************************/
/* [local] */ 


//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1993 - 1996.
//
//--------------------------------------------------------------------------


extern RPC_IF_HANDLE __MIDL_itf_dtgapi_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dtgapi_0000_v0_0_s_ifspec;


#ifndef __DSDataService_LIBRARY_DEFINED__
#define __DSDataService_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: DSDataService
 * at Wed Dec 11 12:17:38 1996
 * using MIDL 3.01.59
 ****************************************/
/* [version][lcid][helpstring][uuid] */ 


/////////////////////////////////////////////////////////////////////////
// Dual interface for IDataService
// {C92A228B-E3DD-11cf-A96F-00A0C90F55D6}
DEFINE_GUID(IID_IDataService, 
0xc92a228b, 0xe3dd, 0x11cf, 0xa9, 0x6f, 0x0, 0xa0, 0xc9, 0xf, 0x55, 0xd6);
// IDispatch Interface for IDataService
// {C92A228C-E3DD-11cf-A96F-00A0C90F55D6}
DEFINE_GUID(IID_IDispDataService, 
0xc92a228c, 0xe3dd, 0x11cf, 0xa9, 0x6f, 0x0, 0xa0, 0xc9, 0xf, 0x55, 0xd6);

EXTERN_C const IID LIBID_DSDataService;

#ifndef __IDataService_INTERFACE_DEFINED__
#define __IDataService_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IDataService
 * at Wed Dec 11 12:17:38 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][hidden][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_IDataService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("C92A228B-E3DD-11cf-A96F-00A0C90F55D6")
    IDataService : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentSelection( 
            /* [retval][out] */ BSTR __RPC_FAR *CurrentUrl) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateProject( 
            /* [in] */ BSTR ProjName,
            /* [in] */ BSTR Dir,
            /* [in] */ VARIANT_BOOL UseCurrentWkspaceDoc,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Project) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenProject( 
            /* [in] */ BSTR ProjName,
            /* [in] */ BSTR Dir,
            /* [in] */ VARIANT_BOOL UseCurrentWkspaceDoc,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Project) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoesProjectFileExist( 
            /* [in] */ BSTR ProjName,
            /* [in] */ BSTR Dir,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Bool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateProjectFile( 
            /* [in] */ BSTR ProjectName,
            /* [in] */ BSTR Dir,
            /* [in] */ BSTR DSN,
            /* [in] */ BSTR ConnStr,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Bool) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDataServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDataService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDataService __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDataService __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDataService __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDataService __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDataService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDataService __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentSelection )( 
            IDataService __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *CurrentUrl);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IDataService __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IDataService __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateProject )( 
            IDataService __RPC_FAR * This,
            /* [in] */ BSTR ProjName,
            /* [in] */ BSTR Dir,
            /* [in] */ VARIANT_BOOL UseCurrentWkspaceDoc,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Project);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenProject )( 
            IDataService __RPC_FAR * This,
            /* [in] */ BSTR ProjName,
            /* [in] */ BSTR Dir,
            /* [in] */ VARIANT_BOOL UseCurrentWkspaceDoc,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Project);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DoesProjectFileExist )( 
            IDataService __RPC_FAR * This,
            /* [in] */ BSTR ProjName,
            /* [in] */ BSTR Dir,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Bool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateProjectFile )( 
            IDataService __RPC_FAR * This,
            /* [in] */ BSTR ProjectName,
            /* [in] */ BSTR Dir,
            /* [in] */ BSTR DSN,
            /* [in] */ BSTR ConnStr,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Bool);
        
        END_INTERFACE
    } IDataServiceVtbl;

    interface IDataService
    {
        CONST_VTBL struct IDataServiceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDataService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDataService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDataService_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDataService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDataService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDataService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDataService_get_CurrentSelection(This,CurrentUrl)	\
    (This)->lpVtbl -> get_CurrentSelection(This,CurrentUrl)

#define IDataService_get_Application(This,Application)	\
    (This)->lpVtbl -> get_Application(This,Application)

#define IDataService_get_Parent(This,Parent)	\
    (This)->lpVtbl -> get_Parent(This,Parent)

#define IDataService_CreateProject(This,ProjName,Dir,UseCurrentWkspaceDoc,Project)	\
    (This)->lpVtbl -> CreateProject(This,ProjName,Dir,UseCurrentWkspaceDoc,Project)

#define IDataService_OpenProject(This,ProjName,Dir,UseCurrentWkspaceDoc,Project)	\
    (This)->lpVtbl -> OpenProject(This,ProjName,Dir,UseCurrentWkspaceDoc,Project)

#define IDataService_DoesProjectFileExist(This,ProjName,Dir,Bool)	\
    (This)->lpVtbl -> DoesProjectFileExist(This,ProjName,Dir,Bool)

#define IDataService_CreateProjectFile(This,ProjectName,Dir,DSN,ConnStr,Bool)	\
    (This)->lpVtbl -> CreateProjectFile(This,ProjectName,Dir,DSN,ConnStr,Bool)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDataService_get_CurrentSelection_Proxy( 
    IDataService __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *CurrentUrl);


void __RPC_STUB IDataService_get_CurrentSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDataService_get_Application_Proxy( 
    IDataService __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application);


void __RPC_STUB IDataService_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDataService_get_Parent_Proxy( 
    IDataService __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent);


void __RPC_STUB IDataService_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDataService_CreateProject_Proxy( 
    IDataService __RPC_FAR * This,
    /* [in] */ BSTR ProjName,
    /* [in] */ BSTR Dir,
    /* [in] */ VARIANT_BOOL UseCurrentWkspaceDoc,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Project);


void __RPC_STUB IDataService_CreateProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDataService_OpenProject_Proxy( 
    IDataService __RPC_FAR * This,
    /* [in] */ BSTR ProjName,
    /* [in] */ BSTR Dir,
    /* [in] */ VARIANT_BOOL UseCurrentWkspaceDoc,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Project);


void __RPC_STUB IDataService_OpenProject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDataService_DoesProjectFileExist_Proxy( 
    IDataService __RPC_FAR * This,
    /* [in] */ BSTR ProjName,
    /* [in] */ BSTR Dir,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Bool);


void __RPC_STUB IDataService_DoesProjectFileExist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDataService_CreateProjectFile_Proxy( 
    IDataService __RPC_FAR * This,
    /* [in] */ BSTR ProjectName,
    /* [in] */ BSTR Dir,
    /* [in] */ BSTR DSN,
    /* [in] */ BSTR ConnStr,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *Bool);


void __RPC_STUB IDataService_CreateProjectFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDataService_INTERFACE_DEFINED__ */


#ifndef __IDispDataService_DISPINTERFACE_DEFINED__
#define __IDispDataService_DISPINTERFACE_DEFINED__

/****************************************
 * Generated header for dispinterface: IDispDataService
 * at Wed Dec 11 12:17:38 1996
 * using MIDL 3.01.59
 ****************************************/
/* [hidden][uuid] */ 



EXTERN_C const IID DIID_IDispDataService;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("C92A228C-E3DD-11cf-A96F-00A0C90F55D6")
    IDispDataService : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDispDataServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDispDataService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDispDataService __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDispDataService __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDispDataService __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDispDataService __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDispDataService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDispDataService __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IDispDataServiceVtbl;

    interface IDispDataService
    {
        CONST_VTBL struct IDispDataServiceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDispDataService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDispDataService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDispDataService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDispDataService_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDispDataService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDispDataService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDispDataService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDispDataService_DISPINTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_DataService;

class DECLSPEC_UUID("C92A228D-E3DD-11cf-A96F-00A0C90F55D6")
DataService;
#endif
#endif /* __DSDataService_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
