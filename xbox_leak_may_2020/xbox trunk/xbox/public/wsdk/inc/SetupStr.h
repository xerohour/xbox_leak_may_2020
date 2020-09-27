/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 20:05:36 1998
 */
/* Compiler settings for setupstr.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __setupstr_h__
#define __setupstr_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISetupStore_FWD_DEFINED__
#define __ISetupStore_FWD_DEFINED__
typedef interface ISetupStore ISetupStore;
#endif 	/* __ISetupStore_FWD_DEFINED__ */


#ifndef __SetupStore_FWD_DEFINED__
#define __SetupStore_FWD_DEFINED__

#ifdef __cplusplus
typedef class SetupStore SetupStore;
#else
typedef struct SetupStore SetupStore;
#endif /* __cplusplus */

#endif 	/* __SetupStore_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_setupstr_0000
 * at Wed Apr 22 20:05:36 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


DEFINE_GUID(IID_ISetupStore, 0x9CB46565, 0xDDFA, 0x11D0, 0x9B, 0xD7, 0x00, 0xC0, 0x4F, 0xC2, 0xE0, 0xD3);
DEFINE_GUID(CLSID_SetupStore, 0x9CB46566, 0xDDFA, 0x11D0, 0x9B, 0xD7, 0x00, 0xC0, 0x4F, 0xC2, 0xE0, 0xD3);
DEFINE_GUID(LIBID_SETUPSTRLib, 0x9CB46558, 0xDDFA, 0x11D0, 0x9B, 0xD7, 0x00, 0xC0, 0x4F, 0xC2, 0xE0, 0xD3);


extern RPC_IF_HANDLE __MIDL_itf_setupstr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_setupstr_0000_v0_0_s_ifspec;

#ifndef __ISetupStore_INTERFACE_DEFINED__
#define __ISetupStore_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISetupStore
 * at Wed Apr 22 20:05:36 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISetupStore;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("9CB46565-DDFA-11D0-9BD7-00C04FC2E0D3")
    ISetupStore : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateSQLDB( 
            /* [in] */ BSTR bstrServerName,
            /* [in] */ BSTR bstrDatabaseName,
            /* [in] */ BSTR bstrDirectoryName,
            /* [in] */ BSTR bstrDBUserName,
            /* [in] */ BSTR bstrDBPassword,
            /* [in] */ BSTR bstrSuperDSPassword,
            /* [in] */ BOOLEAN fUseU2Security) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateJetDB( 
            /* [in] */ BSTR bstrFileName,
            /* [in] */ BSTR bstrDirectoryName,
            /* [in] */ BSTR bstrDBUserName,
            /* [in] */ BSTR bstrDBPassword,
            /* [in] */ BSTR bstrSuperDSPassword,
            /* [in] */ BOOLEAN fUseU2Security) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreatePartition( 
            /* [in] */ BSTR bstrServerName,
            /* [in] */ BSTR bstrDatabaseName,
            /* [in] */ BSTR bstrDBUserName,
            /* [in] */ BSTR bstrDBPassword) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDirectoryPropertiesPage( 
            /* [in] */ long lInstanceId,
            /* [in] */ BSTR szRealm,
            /* [in] */ BSTR szDnPrefix) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ValidateJetFileName( 
            /* [in] */ BSTR bstrFileName,
            /* [in] */ BOOL fWantNew,
            /* [retval][out] */ BOOL __RPC_FAR *pfValid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GenerateJetFileName( 
            /* [in] */ DWORD dwID,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LocalPathToFullPath( 
            /* [in] */ BSTR bstrLocal,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFull) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDriveInformation( 
            /* [in] */ BSTR bstrPath,
            /* [retval][out] */ DWORD __RPC_FAR *pdwdt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISetupStoreVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISetupStore __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISetupStore __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISetupStore __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateSQLDB )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ BSTR bstrServerName,
            /* [in] */ BSTR bstrDatabaseName,
            /* [in] */ BSTR bstrDirectoryName,
            /* [in] */ BSTR bstrDBUserName,
            /* [in] */ BSTR bstrDBPassword,
            /* [in] */ BSTR bstrSuperDSPassword,
            /* [in] */ BOOLEAN fUseU2Security);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateJetDB )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName,
            /* [in] */ BSTR bstrDirectoryName,
            /* [in] */ BSTR bstrDBUserName,
            /* [in] */ BSTR bstrDBPassword,
            /* [in] */ BSTR bstrSuperDSPassword,
            /* [in] */ BOOLEAN fUseU2Security);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreatePartition )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ BSTR bstrServerName,
            /* [in] */ BSTR bstrDatabaseName,
            /* [in] */ BSTR bstrDBUserName,
            /* [in] */ BSTR bstrDBPassword);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDirectoryPropertiesPage )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ long lInstanceId,
            /* [in] */ BSTR szRealm,
            /* [in] */ BSTR szDnPrefix);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ValidateJetFileName )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ BSTR bstrFileName,
            /* [in] */ BOOL fWantNew,
            /* [retval][out] */ BOOL __RPC_FAR *pfValid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GenerateJetFileName )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ DWORD dwID,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LocalPathToFullPath )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ BSTR bstrLocal,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFull);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDriveInformation )( 
            ISetupStore __RPC_FAR * This,
            /* [in] */ BSTR bstrPath,
            /* [retval][out] */ DWORD __RPC_FAR *pdwdt);
        
        END_INTERFACE
    } ISetupStoreVtbl;

    interface ISetupStore
    {
        CONST_VTBL struct ISetupStoreVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISetupStore_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISetupStore_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISetupStore_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISetupStore_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISetupStore_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISetupStore_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISetupStore_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISetupStore_CreateSQLDB(This,bstrServerName,bstrDatabaseName,bstrDirectoryName,bstrDBUserName,bstrDBPassword,bstrSuperDSPassword,fUseU2Security)	\
    (This)->lpVtbl -> CreateSQLDB(This,bstrServerName,bstrDatabaseName,bstrDirectoryName,bstrDBUserName,bstrDBPassword,bstrSuperDSPassword,fUseU2Security)

#define ISetupStore_CreateJetDB(This,bstrFileName,bstrDirectoryName,bstrDBUserName,bstrDBPassword,bstrSuperDSPassword,fUseU2Security)	\
    (This)->lpVtbl -> CreateJetDB(This,bstrFileName,bstrDirectoryName,bstrDBUserName,bstrDBPassword,bstrSuperDSPassword,fUseU2Security)

#define ISetupStore_CreatePartition(This,bstrServerName,bstrDatabaseName,bstrDBUserName,bstrDBPassword)	\
    (This)->lpVtbl -> CreatePartition(This,bstrServerName,bstrDatabaseName,bstrDBUserName,bstrDBPassword)

#define ISetupStore_SetDirectoryPropertiesPage(This,lInstanceId,szRealm,szDnPrefix)	\
    (This)->lpVtbl -> SetDirectoryPropertiesPage(This,lInstanceId,szRealm,szDnPrefix)

#define ISetupStore_ValidateJetFileName(This,bstrFileName,fWantNew,pfValid)	\
    (This)->lpVtbl -> ValidateJetFileName(This,bstrFileName,fWantNew,pfValid)

#define ISetupStore_GenerateJetFileName(This,dwID,pbstrFileName)	\
    (This)->lpVtbl -> GenerateJetFileName(This,dwID,pbstrFileName)

#define ISetupStore_LocalPathToFullPath(This,bstrLocal,pbstrFull)	\
    (This)->lpVtbl -> LocalPathToFullPath(This,bstrLocal,pbstrFull)

#define ISetupStore_GetDriveInformation(This,bstrPath,pdwdt)	\
    (This)->lpVtbl -> GetDriveInformation(This,bstrPath,pdwdt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_CreateSQLDB_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ BSTR bstrServerName,
    /* [in] */ BSTR bstrDatabaseName,
    /* [in] */ BSTR bstrDirectoryName,
    /* [in] */ BSTR bstrDBUserName,
    /* [in] */ BSTR bstrDBPassword,
    /* [in] */ BSTR bstrSuperDSPassword,
    /* [in] */ BOOLEAN fUseU2Security);


void __RPC_STUB ISetupStore_CreateSQLDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_CreateJetDB_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ BSTR bstrFileName,
    /* [in] */ BSTR bstrDirectoryName,
    /* [in] */ BSTR bstrDBUserName,
    /* [in] */ BSTR bstrDBPassword,
    /* [in] */ BSTR bstrSuperDSPassword,
    /* [in] */ BOOLEAN fUseU2Security);


void __RPC_STUB ISetupStore_CreateJetDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_CreatePartition_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ BSTR bstrServerName,
    /* [in] */ BSTR bstrDatabaseName,
    /* [in] */ BSTR bstrDBUserName,
    /* [in] */ BSTR bstrDBPassword);


void __RPC_STUB ISetupStore_CreatePartition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_SetDirectoryPropertiesPage_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ long lInstanceId,
    /* [in] */ BSTR szRealm,
    /* [in] */ BSTR szDnPrefix);


void __RPC_STUB ISetupStore_SetDirectoryPropertiesPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_ValidateJetFileName_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ BSTR bstrFileName,
    /* [in] */ BOOL fWantNew,
    /* [retval][out] */ BOOL __RPC_FAR *pfValid);


void __RPC_STUB ISetupStore_ValidateJetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_GenerateJetFileName_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ DWORD dwID,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFileName);


void __RPC_STUB ISetupStore_GenerateJetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_LocalPathToFullPath_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ BSTR bstrLocal,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFull);


void __RPC_STUB ISetupStore_LocalPathToFullPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISetupStore_GetDriveInformation_Proxy( 
    ISetupStore __RPC_FAR * This,
    /* [in] */ BSTR bstrPath,
    /* [retval][out] */ DWORD __RPC_FAR *pdwdt);


void __RPC_STUB ISetupStore_GetDriveInformation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISetupStore_INTERFACE_DEFINED__ */



#ifndef __SETUPSTRLib_LIBRARY_DEFINED__
#define __SETUPSTRLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: SETUPSTRLib
 * at Wed Apr 22 20:05:36 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_SETUPSTRLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_SetupStore;

class DECLSPEC_UUID("9CB46566-DDFA-11D0-9BD7-00C04FC2E0D3")
SetupStore;
#endif
#endif /* __SETUPSTRLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
