/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 19:39:10 1998
 */
/* Compiler settings for auo.idl:
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

#ifndef __auo_h__
#define __auo_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IAuoConfig_FWD_DEFINED__
#define __IAuoConfig_FWD_DEFINED__
typedef interface IAuoConfig IAuoConfig;
#endif 	/* __IAuoConfig_FWD_DEFINED__ */


#ifndef __IUserObjects_FWD_DEFINED__
#define __IUserObjects_FWD_DEFINED__
typedef interface IUserObjects IUserObjects;
#endif 	/* __IUserObjects_FWD_DEFINED__ */


#ifndef __ISchemaObjects_FWD_DEFINED__
#define __ISchemaObjects_FWD_DEFINED__
typedef interface ISchemaObjects ISchemaObjects;
#endif 	/* __ISchemaObjects_FWD_DEFINED__ */


#ifndef __UserObjects_FWD_DEFINED__
#define __UserObjects_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserObjects UserObjects;
#else
typedef struct UserObjects UserObjects;
#endif /* __cplusplus */

#endif 	/* __UserObjects_FWD_DEFINED__ */


#ifndef __SchemaObjects_FWD_DEFINED__
#define __SchemaObjects_FWD_DEFINED__

#ifdef __cplusplus
typedef class SchemaObjects SchemaObjects;
#else
typedef struct SchemaObjects SchemaObjects;
#endif /* __cplusplus */

#endif 	/* __SchemaObjects_FWD_DEFINED__ */


#ifndef __AuoConfig_FWD_DEFINED__
#define __AuoConfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class AuoConfig AuoConfig;
#else
typedef struct AuoConfig AuoConfig;
#endif /* __cplusplus */

#endif 	/* __AuoConfig_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_auo_0000
 * at Wed Apr 22 19:39:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


DEFINE_GUID(IID_IAuoConfig, 0x4b928606,0xb685,0x11d0,0xbb,0xd7,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(IID_IUserObjects, 0x77BD6B36,0xB1B5,0x11D0,0xBB,0xD6,0x00,0xC0,0x4F,0xB6,0x15,0xE5);
DEFINE_GUID(IID_ISchemaObjects, 0x4ca434b4,0xb684,0x11d0,0xbb,0xd7,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(LIBID_AUOLib, 0x77BD6B27,0xB1B5,0x11D0,0xBB,0xD6,0x00,0xC0,0x4F,0xB6,0x15,0xE5);
DEFINE_GUID(CLSID_UserObjects, 0x77BD6B37,0xB1B5,0x11D0,0xBB,0xD6,0x00,0xC0,0x4F,0xB6,0x15,0xE5);
DEFINE_GUID(CLSID_SchemaObjects, 0xc642753e,0xb74b,0x11d0,0xbb,0xd7,0x00,0xc0,0x4f,0xb6,0x15,0xe5);
DEFINE_GUID(CLSID_AuoConfig, 0x0b9ae1d6,0xb696,0x11d0,0xbb,0xd7,0x00,0xc0,0x4f,0xb6,0x15,0xe5);


extern RPC_IF_HANDLE __MIDL_itf_auo_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_auo_0000_v0_0_s_ifspec;

#ifndef __IAuoConfig_INTERFACE_DEFINED__
#define __IAuoConfig_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IAuoConfig
 * at Wed Apr 22 19:39:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IAuoConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("4b928606-b685-11d0-bbd7-00c04fb615e5")
    IAuoConfig : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            LONG lVirtServId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetInfo( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEntry( 
            /* [in] */ BSTR bszName,
            /* [in] */ BSTR bszADsPathPrefix,
            /* [in] */ BSTR bszSchema,
            /* [in] */ BSTR bszClass,
            /* [in] */ LONG lSuffix,
            /* [in] */ BSTR bszDepObject,
            /* [in] */ BSTR bszDepProp,
            /* [in] */ BSTR bszBindAsName,
            /* [in] */ BSTR bszBindAsPassword) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEntry( 
            /* [in] */ BSTR bszName,
            /* [out][in] */ VARIANT __RPC_FAR *pbszADsPathPrefix,
            /* [out][in] */ VARIANT __RPC_FAR *pbszSchema,
            /* [out][in] */ VARIANT __RPC_FAR *pbszClass,
            /* [out][in] */ VARIANT __RPC_FAR *plSuffix,
            /* [out][in] */ VARIANT __RPC_FAR *pbszDepObject,
            /* [out][in] */ VARIANT __RPC_FAR *pbszDepProp,
            /* [out][in] */ VARIANT __RPC_FAR *pbszBindAsName,
            /* [out][in] */ VARIANT __RPC_FAR *pbszBindAsPassword) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveEntry( 
            /* [in] */ BSTR bszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEntries( 
            /* [out][in] */ VARIANT __RPC_FAR *pNames,
            /* [out][in] */ VARIANT __RPC_FAR *pProviders) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteInstance( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAuoConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAuoConfig __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAuoConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IAuoConfig __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetInfo )( 
            IAuoConfig __RPC_FAR * This,
            LONG lVirtServId);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetInfo )( 
            IAuoConfig __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetEntry )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ BSTR bszName,
            /* [in] */ BSTR bszADsPathPrefix,
            /* [in] */ BSTR bszSchema,
            /* [in] */ BSTR bszClass,
            /* [in] */ LONG lSuffix,
            /* [in] */ BSTR bszDepObject,
            /* [in] */ BSTR bszDepProp,
            /* [in] */ BSTR bszBindAsName,
            /* [in] */ BSTR bszBindAsPassword);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEntry )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ BSTR bszName,
            /* [out][in] */ VARIANT __RPC_FAR *pbszADsPathPrefix,
            /* [out][in] */ VARIANT __RPC_FAR *pbszSchema,
            /* [out][in] */ VARIANT __RPC_FAR *pbszClass,
            /* [out][in] */ VARIANT __RPC_FAR *plSuffix,
            /* [out][in] */ VARIANT __RPC_FAR *pbszDepObject,
            /* [out][in] */ VARIANT __RPC_FAR *pbszDepProp,
            /* [out][in] */ VARIANT __RPC_FAR *pbszBindAsName,
            /* [out][in] */ VARIANT __RPC_FAR *pbszBindAsPassword);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveEntry )( 
            IAuoConfig __RPC_FAR * This,
            /* [in] */ BSTR bszName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEntries )( 
            IAuoConfig __RPC_FAR * This,
            /* [out][in] */ VARIANT __RPC_FAR *pNames,
            /* [out][in] */ VARIANT __RPC_FAR *pProviders);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteInstance )( 
            IAuoConfig __RPC_FAR * This);
        
        END_INTERFACE
    } IAuoConfigVtbl;

    interface IAuoConfig
    {
        CONST_VTBL struct IAuoConfigVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAuoConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAuoConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAuoConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAuoConfig_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAuoConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAuoConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAuoConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAuoConfig_GetInfo(This,lVirtServId)	\
    (This)->lpVtbl -> GetInfo(This,lVirtServId)

#define IAuoConfig_SetInfo(This)	\
    (This)->lpVtbl -> SetInfo(This)

#define IAuoConfig_SetEntry(This,bszName,bszADsPathPrefix,bszSchema,bszClass,lSuffix,bszDepObject,bszDepProp,bszBindAsName,bszBindAsPassword)	\
    (This)->lpVtbl -> SetEntry(This,bszName,bszADsPathPrefix,bszSchema,bszClass,lSuffix,bszDepObject,bszDepProp,bszBindAsName,bszBindAsPassword)

#define IAuoConfig_GetEntry(This,bszName,pbszADsPathPrefix,pbszSchema,pbszClass,plSuffix,pbszDepObject,pbszDepProp,pbszBindAsName,pbszBindAsPassword)	\
    (This)->lpVtbl -> GetEntry(This,bszName,pbszADsPathPrefix,pbszSchema,pbszClass,plSuffix,pbszDepObject,pbszDepProp,pbszBindAsName,pbszBindAsPassword)

#define IAuoConfig_RemoveEntry(This,bszName)	\
    (This)->lpVtbl -> RemoveEntry(This,bszName)

#define IAuoConfig_GetEntries(This,pNames,pProviders)	\
    (This)->lpVtbl -> GetEntries(This,pNames,pProviders)

#define IAuoConfig_DeleteInstance(This)	\
    (This)->lpVtbl -> DeleteInstance(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAuoConfig_GetInfo_Proxy( 
    IAuoConfig __RPC_FAR * This,
    LONG lVirtServId);


void __RPC_STUB IAuoConfig_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuoConfig_SetInfo_Proxy( 
    IAuoConfig __RPC_FAR * This);


void __RPC_STUB IAuoConfig_SetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuoConfig_SetEntry_Proxy( 
    IAuoConfig __RPC_FAR * This,
    /* [in] */ BSTR bszName,
    /* [in] */ BSTR bszADsPathPrefix,
    /* [in] */ BSTR bszSchema,
    /* [in] */ BSTR bszClass,
    /* [in] */ LONG lSuffix,
    /* [in] */ BSTR bszDepObject,
    /* [in] */ BSTR bszDepProp,
    /* [in] */ BSTR bszBindAsName,
    /* [in] */ BSTR bszBindAsPassword);


void __RPC_STUB IAuoConfig_SetEntry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuoConfig_GetEntry_Proxy( 
    IAuoConfig __RPC_FAR * This,
    /* [in] */ BSTR bszName,
    /* [out][in] */ VARIANT __RPC_FAR *pbszADsPathPrefix,
    /* [out][in] */ VARIANT __RPC_FAR *pbszSchema,
    /* [out][in] */ VARIANT __RPC_FAR *pbszClass,
    /* [out][in] */ VARIANT __RPC_FAR *plSuffix,
    /* [out][in] */ VARIANT __RPC_FAR *pbszDepObject,
    /* [out][in] */ VARIANT __RPC_FAR *pbszDepProp,
    /* [out][in] */ VARIANT __RPC_FAR *pbszBindAsName,
    /* [out][in] */ VARIANT __RPC_FAR *pbszBindAsPassword);


void __RPC_STUB IAuoConfig_GetEntry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuoConfig_RemoveEntry_Proxy( 
    IAuoConfig __RPC_FAR * This,
    /* [in] */ BSTR bszName);


void __RPC_STUB IAuoConfig_RemoveEntry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuoConfig_GetEntries_Proxy( 
    IAuoConfig __RPC_FAR * This,
    /* [out][in] */ VARIANT __RPC_FAR *pNames,
    /* [out][in] */ VARIANT __RPC_FAR *pProviders);


void __RPC_STUB IAuoConfig_GetEntries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAuoConfig_DeleteInstance_Proxy( 
    IAuoConfig __RPC_FAR * This);


void __RPC_STUB IAuoConfig_DeleteInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAuoConfig_INTERFACE_DEFINED__ */


#ifndef __IUserObjects_INTERFACE_DEFINED__
#define __IUserObjects_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IUserObjects
 * at Wed Apr 22 19:39:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IUserObjects;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("77BD6B36-B1B5-11D0-BBD6-00C04FB615E5")
    IUserObjects : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnStartPage( 
            IUnknown __RPC_FAR *pContext) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Init( 
            BSTR bszHost,
            BSTR bszUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetUserName( 
            BSTR bszUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BindAs( 
            BSTR bszAlias,
            BSTR bszUserName,
            BSTR bszPassword) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetObjectEx( 
            /* [in] */ BSTR bszEntryName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppEntry) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUserObjectsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IUserObjects __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IUserObjects __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IUserObjects __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IUserObjects __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IUserObjects __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IUserObjects __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IUserObjects __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnStartPage )( 
            IUserObjects __RPC_FAR * This,
            IUnknown __RPC_FAR *pContext);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnEndPage )( 
            IUserObjects __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            IUserObjects __RPC_FAR * This,
            BSTR bszHost,
            BSTR bszUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetUserName )( 
            IUserObjects __RPC_FAR * This,
            BSTR bszUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindAs )( 
            IUserObjects __RPC_FAR * This,
            BSTR bszAlias,
            BSTR bszUserName,
            BSTR bszPassword);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetObjectEx )( 
            IUserObjects __RPC_FAR * This,
            /* [in] */ BSTR bszEntryName,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppEntry);
        
        END_INTERFACE
    } IUserObjectsVtbl;

    interface IUserObjects
    {
        CONST_VTBL struct IUserObjectsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserObjects_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUserObjects_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUserObjects_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUserObjects_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IUserObjects_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IUserObjects_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IUserObjects_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IUserObjects_OnStartPage(This,pContext)	\
    (This)->lpVtbl -> OnStartPage(This,pContext)

#define IUserObjects_OnEndPage(This)	\
    (This)->lpVtbl -> OnEndPage(This)

#define IUserObjects_Init(This,bszHost,bszUserName)	\
    (This)->lpVtbl -> Init(This,bszHost,bszUserName)

#define IUserObjects_SetUserName(This,bszUserName)	\
    (This)->lpVtbl -> SetUserName(This,bszUserName)

#define IUserObjects_BindAs(This,bszAlias,bszUserName,bszPassword)	\
    (This)->lpVtbl -> BindAs(This,bszAlias,bszUserName,bszPassword)

#define IUserObjects_GetObjectEx(This,bszEntryName,ppEntry)	\
    (This)->lpVtbl -> GetObjectEx(This,bszEntryName,ppEntry)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IUserObjects_OnStartPage_Proxy( 
    IUserObjects __RPC_FAR * This,
    IUnknown __RPC_FAR *pContext);


void __RPC_STUB IUserObjects_OnStartPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IUserObjects_OnEndPage_Proxy( 
    IUserObjects __RPC_FAR * This);


void __RPC_STUB IUserObjects_OnEndPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IUserObjects_Init_Proxy( 
    IUserObjects __RPC_FAR * This,
    BSTR bszHost,
    BSTR bszUserName);


void __RPC_STUB IUserObjects_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IUserObjects_SetUserName_Proxy( 
    IUserObjects __RPC_FAR * This,
    BSTR bszUserName);


void __RPC_STUB IUserObjects_SetUserName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IUserObjects_BindAs_Proxy( 
    IUserObjects __RPC_FAR * This,
    BSTR bszAlias,
    BSTR bszUserName,
    BSTR bszPassword);


void __RPC_STUB IUserObjects_BindAs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IUserObjects_GetObjectEx_Proxy( 
    IUserObjects __RPC_FAR * This,
    /* [in] */ BSTR bszEntryName,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppEntry);


void __RPC_STUB IUserObjects_GetObjectEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUserObjects_INTERFACE_DEFINED__ */


#ifndef __ISchemaObjects_INTERFACE_DEFINED__
#define __ISchemaObjects_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISchemaObjects
 * at Wed Apr 22 19:39:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISchemaObjects;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("4ca434b4-b684-11d0-bbd7-00c04fb615e5")
    ISchemaObjects : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnStartPage( 
            IUnknown __RPC_FAR *pContext) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ BSTR bszHostName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetRelativeName( 
            /* [in] */ IUnknown __RPC_FAR *pUnk,
            /* [retval][out] */ BSTR __RPC_FAR *pbszName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetEntrySchemaPath( 
            /* [in] */ BSTR bszEntryName,
            /* [retval][out] */ BSTR __RPC_FAR *pbszSchemaPath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE InitEx( 
            /* [in] */ BSTR bszHostName,
            /* [in] */ VARIANT_BOOL fPromptCredentials) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BindAs( 
            BSTR bszAlias,
            BSTR bszUserName,
            BSTR bszPassword) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISchemaObjectsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISchemaObjects __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISchemaObjects __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISchemaObjects __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnStartPage )( 
            ISchemaObjects __RPC_FAR * This,
            IUnknown __RPC_FAR *pContext);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnEndPage )( 
            ISchemaObjects __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ BSTR bszHostName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRelativeName )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pUnk,
            /* [retval][out] */ BSTR __RPC_FAR *pbszName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEntrySchemaPath )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ BSTR bszEntryName,
            /* [retval][out] */ BSTR __RPC_FAR *pbszSchemaPath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitEx )( 
            ISchemaObjects __RPC_FAR * This,
            /* [in] */ BSTR bszHostName,
            /* [in] */ VARIANT_BOOL fPromptCredentials);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BindAs )( 
            ISchemaObjects __RPC_FAR * This,
            BSTR bszAlias,
            BSTR bszUserName,
            BSTR bszPassword);
        
        END_INTERFACE
    } ISchemaObjectsVtbl;

    interface ISchemaObjects
    {
        CONST_VTBL struct ISchemaObjectsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISchemaObjects_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISchemaObjects_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISchemaObjects_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISchemaObjects_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISchemaObjects_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISchemaObjects_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISchemaObjects_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISchemaObjects_OnStartPage(This,pContext)	\
    (This)->lpVtbl -> OnStartPage(This,pContext)

#define ISchemaObjects_OnEndPage(This)	\
    (This)->lpVtbl -> OnEndPage(This)

#define ISchemaObjects_Init(This,bszHostName)	\
    (This)->lpVtbl -> Init(This,bszHostName)

#define ISchemaObjects_GetRelativeName(This,pUnk,pbszName)	\
    (This)->lpVtbl -> GetRelativeName(This,pUnk,pbszName)

#define ISchemaObjects_GetEntrySchemaPath(This,bszEntryName,pbszSchemaPath)	\
    (This)->lpVtbl -> GetEntrySchemaPath(This,bszEntryName,pbszSchemaPath)

#define ISchemaObjects_InitEx(This,bszHostName,fPromptCredentials)	\
    (This)->lpVtbl -> InitEx(This,bszHostName,fPromptCredentials)

#define ISchemaObjects_BindAs(This,bszAlias,bszUserName,bszPassword)	\
    (This)->lpVtbl -> BindAs(This,bszAlias,bszUserName,bszPassword)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_OnStartPage_Proxy( 
    ISchemaObjects __RPC_FAR * This,
    IUnknown __RPC_FAR *pContext);


void __RPC_STUB ISchemaObjects_OnStartPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_OnEndPage_Proxy( 
    ISchemaObjects __RPC_FAR * This);


void __RPC_STUB ISchemaObjects_OnEndPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_Init_Proxy( 
    ISchemaObjects __RPC_FAR * This,
    /* [in] */ BSTR bszHostName);


void __RPC_STUB ISchemaObjects_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_GetRelativeName_Proxy( 
    ISchemaObjects __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pUnk,
    /* [retval][out] */ BSTR __RPC_FAR *pbszName);


void __RPC_STUB ISchemaObjects_GetRelativeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_GetEntrySchemaPath_Proxy( 
    ISchemaObjects __RPC_FAR * This,
    /* [in] */ BSTR bszEntryName,
    /* [retval][out] */ BSTR __RPC_FAR *pbszSchemaPath);


void __RPC_STUB ISchemaObjects_GetEntrySchemaPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_InitEx_Proxy( 
    ISchemaObjects __RPC_FAR * This,
    /* [in] */ BSTR bszHostName,
    /* [in] */ VARIANT_BOOL fPromptCredentials);


void __RPC_STUB ISchemaObjects_InitEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISchemaObjects_BindAs_Proxy( 
    ISchemaObjects __RPC_FAR * This,
    BSTR bszAlias,
    BSTR bszUserName,
    BSTR bszPassword);


void __RPC_STUB ISchemaObjects_BindAs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISchemaObjects_INTERFACE_DEFINED__ */



#ifndef __AUOLib_LIBRARY_DEFINED__
#define __AUOLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: AUOLib
 * at Wed Apr 22 19:39:10 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_AUOLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_UserObjects;

class DECLSPEC_UUID("77BD6B37-B1B5-11D0-BBD6-00C04FB615E5")
UserObjects;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_SchemaObjects;

class DECLSPEC_UUID("c642753e-b74b-11d0-bbd7-00c04fb615e5")
SchemaObjects;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_AuoConfig;

class DECLSPEC_UUID("0b9ae1d6-b696-11d0-bbd7-00c04fb615e5")
AuoConfig;
#endif
#endif /* __AUOLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
