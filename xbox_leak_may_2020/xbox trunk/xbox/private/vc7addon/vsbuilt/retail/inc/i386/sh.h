
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sat Jan 05 01:51:35 2002
 */
/* Compiler settings for sh.idl:
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

#ifndef __sh_h__
#define __sh_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDebugSymbolProvider_FWD_DEFINED__
#define __IDebugSymbolProvider_FWD_DEFINED__
typedef interface IDebugSymbolProvider IDebugSymbolProvider;
#endif 	/* __IDebugSymbolProvider_FWD_DEFINED__ */


#ifndef __IDebugComPlusSymbolProvider_FWD_DEFINED__
#define __IDebugComPlusSymbolProvider_FWD_DEFINED__
typedef interface IDebugComPlusSymbolProvider IDebugComPlusSymbolProvider;
#endif 	/* __IDebugComPlusSymbolProvider_FWD_DEFINED__ */


#ifndef __IDebugNativeSymbolProvider_FWD_DEFINED__
#define __IDebugNativeSymbolProvider_FWD_DEFINED__
typedef interface IDebugNativeSymbolProvider IDebugNativeSymbolProvider;
#endif 	/* __IDebugNativeSymbolProvider_FWD_DEFINED__ */


#ifndef __IDebugField_FWD_DEFINED__
#define __IDebugField_FWD_DEFINED__
typedef interface IDebugField IDebugField;
#endif 	/* __IDebugField_FWD_DEFINED__ */


#ifndef __IDebugContainerField_FWD_DEFINED__
#define __IDebugContainerField_FWD_DEFINED__
typedef interface IDebugContainerField IDebugContainerField;
#endif 	/* __IDebugContainerField_FWD_DEFINED__ */


#ifndef __IDebugMethodField_FWD_DEFINED__
#define __IDebugMethodField_FWD_DEFINED__
typedef interface IDebugMethodField IDebugMethodField;
#endif 	/* __IDebugMethodField_FWD_DEFINED__ */


#ifndef __IDebugClassField_FWD_DEFINED__
#define __IDebugClassField_FWD_DEFINED__
typedef interface IDebugClassField IDebugClassField;
#endif 	/* __IDebugClassField_FWD_DEFINED__ */


#ifndef __IDebugPropertyField_FWD_DEFINED__
#define __IDebugPropertyField_FWD_DEFINED__
typedef interface IDebugPropertyField IDebugPropertyField;
#endif 	/* __IDebugPropertyField_FWD_DEFINED__ */


#ifndef __IDebugArrayField_FWD_DEFINED__
#define __IDebugArrayField_FWD_DEFINED__
typedef interface IDebugArrayField IDebugArrayField;
#endif 	/* __IDebugArrayField_FWD_DEFINED__ */


#ifndef __IDebugPointerField_FWD_DEFINED__
#define __IDebugPointerField_FWD_DEFINED__
typedef interface IDebugPointerField IDebugPointerField;
#endif 	/* __IDebugPointerField_FWD_DEFINED__ */


#ifndef __IDebugEnumField_FWD_DEFINED__
#define __IDebugEnumField_FWD_DEFINED__
typedef interface IDebugEnumField IDebugEnumField;
#endif 	/* __IDebugEnumField_FWD_DEFINED__ */


#ifndef __IDebugBitField_FWD_DEFINED__
#define __IDebugBitField_FWD_DEFINED__
typedef interface IDebugBitField IDebugBitField;
#endif 	/* __IDebugBitField_FWD_DEFINED__ */


#ifndef __IDebugDynamicField_FWD_DEFINED__
#define __IDebugDynamicField_FWD_DEFINED__
typedef interface IDebugDynamicField IDebugDynamicField;
#endif 	/* __IDebugDynamicField_FWD_DEFINED__ */


#ifndef __IDebugDynamicFieldCOMPlus_FWD_DEFINED__
#define __IDebugDynamicFieldCOMPlus_FWD_DEFINED__
typedef interface IDebugDynamicFieldCOMPlus IDebugDynamicFieldCOMPlus;
#endif 	/* __IDebugDynamicFieldCOMPlus_FWD_DEFINED__ */


#ifndef __IDebugEngineSymbolProviderServices_FWD_DEFINED__
#define __IDebugEngineSymbolProviderServices_FWD_DEFINED__
typedef interface IDebugEngineSymbolProviderServices IDebugEngineSymbolProviderServices;
#endif 	/* __IDebugEngineSymbolProviderServices_FWD_DEFINED__ */


#ifndef __IDebugAddress_FWD_DEFINED__
#define __IDebugAddress_FWD_DEFINED__
typedef interface IDebugAddress IDebugAddress;
#endif 	/* __IDebugAddress_FWD_DEFINED__ */


#ifndef __IEnumDebugFields_FWD_DEFINED__
#define __IEnumDebugFields_FWD_DEFINED__
typedef interface IEnumDebugFields IEnumDebugFields;
#endif 	/* __IEnumDebugFields_FWD_DEFINED__ */


#ifndef __IEnumDebugAddresses_FWD_DEFINED__
#define __IEnumDebugAddresses_FWD_DEFINED__
typedef interface IEnumDebugAddresses IEnumDebugAddresses;
#endif 	/* __IEnumDebugAddresses_FWD_DEFINED__ */


#ifndef __IDebugCustomAttributeQuery_FWD_DEFINED__
#define __IDebugCustomAttributeQuery_FWD_DEFINED__
typedef interface IDebugCustomAttributeQuery IDebugCustomAttributeQuery;
#endif 	/* __IDebugCustomAttributeQuery_FWD_DEFINED__ */


#ifndef __SHManaged_FWD_DEFINED__
#define __SHManaged_FWD_DEFINED__

#ifdef __cplusplus
typedef class SHManaged SHManaged;
#else
typedef struct SHManaged SHManaged;
#endif /* __cplusplus */

#endif 	/* __SHManaged_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "msdbg.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_sh_0000 */
/* [local] */ 


















// Symbol provider HRESULTs
//
// HRESULTs: General
static const int E_SH_SYMBOL_STORE_NOT_INITIALIZED			= MAKE_HRESULT(1, FACILITY_ITF, 0x0001);
static const int E_SH_SYMBOL_STORE_ALREADY_INITIALIZED		= MAKE_HRESULT(1, FACILITY_ITF, 0x0002);
// HRESULTs: GetContainerField and GetTypeFromAddress
static const int E_SH_INVALID_ADDRESS						= MAKE_HRESULT(1, FACILITY_ITF, 0x0003);
// HRESULTs: GetAddressesFromPosition
static const int E_SH_NO_SYMBOLS_FOR_POSITION				= MAKE_HRESULT(1, FACILITY_ITF, 0x0004);
static const int E_SH_INVALID_POSITION						= MAKE_HRESULT(1, FACILITY_ITF, 0x0005);
// HRESULTs: GetContextFromAddress
static const int E_SH_NO_SYMBOLS_FOR_ADDRESS					= MAKE_HRESULT(1, FACILITY_ITF, 0x0006);
// HRESULTs: GetAddress
static const int E_SH_NO_ADDRESS								= MAKE_HRESULT(1, FACILITY_ITF, 0x0007);
// HRESULTs: GetType
static const int E_SH_NO_TYPE								= MAKE_HRESULT(1, FACILITY_ITF, 0x0008);
static const int E_SH_DYNAMIC_TYPE							= MAKE_HRESULT(1, FACILITY_ITF, 0x0009);
// HRESULTs: GetContainer
static const int S_SH_NO_CONTAINER							= MAKE_HRESULT(0, FACILITY_ITF, 0x000a);
// HRESULTs: GetSize
static const int S_SH_NO_SIZE								= MAKE_HRESULT(0, FACILITY_ITF, 0x000b);
static const int E_SH_DYNAMIC_SIZE							= MAKE_HRESULT(1, FACILITY_ITF, 0x000c);
// HRESULTs: EnumFields
static const int S_SH_NO_FIELDS								= MAKE_HRESULT(0, FACILITY_ITF, 0x000d);
// HRESULTs: GetThis
static const int S_SH_METHOD_NO_THIS							= MAKE_HRESULT(0, FACILITY_ITF, 0x000e);
// HRESULTs: EnumBaseClasses
static const int S_SH_NO_BASE_CLASSES						= MAKE_HRESULT(0, FACILITY_ITF, 0x000f);
static const int E_SH_FILE_NOT_FOUND						    = MAKE_HRESULT(1, FACILITY_ITF, 0x0010);
extern GUID guidSymStoreMetaPDB;
extern GUID guidConstantValue;
extern GUID guidConstantType;
extern GUID guidIntPtr;
// HRESULTS: GetContextInfo
static const int E_SH_NO_DOC_CONTEXT						    = MAKE_HRESULT(1, FACILITY_ITF, 0x0011);
// HRESULTS: ClassRefToClassDef
static const int E_SH_CLASSDEFINITION_NOT_LOADED				= MAKE_HRESULT(1, FACILITY_ITF, 0x0012);
typedef INT32 _mdToken;

typedef 
enum NameMatchOptions
    {	nmNone	= 0,
	nmCaseSensitive	= nmNone + 1,
	nmCaseInsensitive	= nmCaseSensitive + 1
    } 	NAME_MATCH;



extern RPC_IF_HANDLE __MIDL_itf_sh_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_sh_0000_v0_0_s_ifspec;

#ifndef __IDebugSymbolProvider_INTERFACE_DEFINED__
#define __IDebugSymbolProvider_INTERFACE_DEFINED__

/* interface IDebugSymbolProvider */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugSymbolProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eae-8b9d-11d2-9014-00c04fa38338")
    IDebugSymbolProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IDebugEngineSymbolProviderServices *pServices) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Uninitialize( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContainerField( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugContainerField **ppContainerField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetField( 
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ IDebugAddress *pAddressCur,
            /* [out] */ IDebugField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAddressesFromPosition( 
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAddressesFromContext( 
            /* [in] */ IDebugDocumentContext2 *pDocContext,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContextFromAddress( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugDocumentContext2 **ppDocContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLanguage( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ GUID *pguidLanguage,
            /* [out] */ GUID *pguidLanguageVendor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGlobalContainer( 
            /* [out] */ IDebugContainerField **pField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMethodFieldsByName( 
            /* [full][in] */ LPCOLESTR pszFullName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClassTypeByName( 
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugClassField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNamespacesUsedAtAddress( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTypeByName( 
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextAddress( 
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IDebugAddress **ppAddress) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugSymbolProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugSymbolProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugSymbolProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugSymbolProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugEngineSymbolProviderServices *pServices);
        
        HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            IDebugSymbolProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerField )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetField )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ IDebugAddress *pAddressCur,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesFromPosition )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesFromContext )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugDocumentContext2 *pDocContext,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetContextFromAddress )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugDocumentContext2 **ppDocContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetLanguage )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ GUID *pguidLanguage,
            /* [out] */ GUID *pguidLanguageVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetGlobalContainer )( 
            IDebugSymbolProvider * This,
            /* [out] */ IDebugContainerField **pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodFieldsByName )( 
            IDebugSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszFullName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassTypeByName )( 
            IDebugSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugClassField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetNamespacesUsedAtAddress )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeByName )( 
            IDebugSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextAddress )( 
            IDebugSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IDebugAddress **ppAddress);
        
        END_INTERFACE
    } IDebugSymbolProviderVtbl;

    interface IDebugSymbolProvider
    {
        CONST_VTBL struct IDebugSymbolProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugSymbolProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugSymbolProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugSymbolProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugSymbolProvider_Initialize(This,pServices)	\
    (This)->lpVtbl -> Initialize(This,pServices)

#define IDebugSymbolProvider_Uninitialize(This)	\
    (This)->lpVtbl -> Uninitialize(This)

#define IDebugSymbolProvider_GetContainerField(This,pAddress,ppContainerField)	\
    (This)->lpVtbl -> GetContainerField(This,pAddress,ppContainerField)

#define IDebugSymbolProvider_GetField(This,pAddress,pAddressCur,ppField)	\
    (This)->lpVtbl -> GetField(This,pAddress,pAddressCur,ppField)

#define IDebugSymbolProvider_GetAddressesFromPosition(This,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesFromPosition(This,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugSymbolProvider_GetAddressesFromContext(This,pDocContext,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesFromContext(This,pDocContext,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugSymbolProvider_GetContextFromAddress(This,pAddress,ppDocContext)	\
    (This)->lpVtbl -> GetContextFromAddress(This,pAddress,ppDocContext)

#define IDebugSymbolProvider_GetLanguage(This,pAddress,pguidLanguage,pguidLanguageVendor)	\
    (This)->lpVtbl -> GetLanguage(This,pAddress,pguidLanguage,pguidLanguageVendor)

#define IDebugSymbolProvider_GetGlobalContainer(This,pField)	\
    (This)->lpVtbl -> GetGlobalContainer(This,pField)

#define IDebugSymbolProvider_GetMethodFieldsByName(This,pszFullName,nameMatch,ppEnum)	\
    (This)->lpVtbl -> GetMethodFieldsByName(This,pszFullName,nameMatch,ppEnum)

#define IDebugSymbolProvider_GetClassTypeByName(This,pszClassName,nameMatch,ppField)	\
    (This)->lpVtbl -> GetClassTypeByName(This,pszClassName,nameMatch,ppField)

#define IDebugSymbolProvider_GetNamespacesUsedAtAddress(This,pAddress,ppEnum)	\
    (This)->lpVtbl -> GetNamespacesUsedAtAddress(This,pAddress,ppEnum)

#define IDebugSymbolProvider_GetTypeByName(This,pszClassName,nameMatch,ppField)	\
    (This)->lpVtbl -> GetTypeByName(This,pszClassName,nameMatch,ppField)

#define IDebugSymbolProvider_GetNextAddress(This,pAddress,fStatmentOnly,ppAddress)	\
    (This)->lpVtbl -> GetNextAddress(This,pAddress,fStatmentOnly,ppAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_Initialize_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugEngineSymbolProviderServices *pServices);


void __RPC_STUB IDebugSymbolProvider_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_Uninitialize_Proxy( 
    IDebugSymbolProvider * This);


void __RPC_STUB IDebugSymbolProvider_Uninitialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetContainerField_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ IDebugContainerField **ppContainerField);


void __RPC_STUB IDebugSymbolProvider_GetContainerField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetField_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [in] */ IDebugAddress *pAddressCur,
    /* [out] */ IDebugField **ppField);


void __RPC_STUB IDebugSymbolProvider_GetField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetAddressesFromPosition_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugDocumentPosition2 *pDocPos,
    /* [in] */ BOOL fStatmentOnly,
    /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
    /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);


void __RPC_STUB IDebugSymbolProvider_GetAddressesFromPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetAddressesFromContext_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugDocumentContext2 *pDocContext,
    /* [in] */ BOOL fStatmentOnly,
    /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
    /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);


void __RPC_STUB IDebugSymbolProvider_GetAddressesFromContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetContextFromAddress_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ IDebugDocumentContext2 **ppDocContext);


void __RPC_STUB IDebugSymbolProvider_GetContextFromAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetLanguage_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ GUID *pguidLanguage,
    /* [out] */ GUID *pguidLanguageVendor);


void __RPC_STUB IDebugSymbolProvider_GetLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetGlobalContainer_Proxy( 
    IDebugSymbolProvider * This,
    /* [out] */ IDebugContainerField **pField);


void __RPC_STUB IDebugSymbolProvider_GetGlobalContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetMethodFieldsByName_Proxy( 
    IDebugSymbolProvider * This,
    /* [full][in] */ LPCOLESTR pszFullName,
    /* [in] */ NAME_MATCH nameMatch,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugSymbolProvider_GetMethodFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetClassTypeByName_Proxy( 
    IDebugSymbolProvider * This,
    /* [full][in] */ LPCOLESTR pszClassName,
    /* [in] */ NAME_MATCH nameMatch,
    /* [out] */ IDebugClassField **ppField);


void __RPC_STUB IDebugSymbolProvider_GetClassTypeByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetNamespacesUsedAtAddress_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugSymbolProvider_GetNamespacesUsedAtAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetTypeByName_Proxy( 
    IDebugSymbolProvider * This,
    /* [full][in] */ LPCOLESTR pszClassName,
    /* [in] */ NAME_MATCH nameMatch,
    /* [out] */ IDebugField **ppField);


void __RPC_STUB IDebugSymbolProvider_GetTypeByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSymbolProvider_GetNextAddress_Proxy( 
    IDebugSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [in] */ BOOL fStatmentOnly,
    /* [out] */ IDebugAddress **ppAddress);


void __RPC_STUB IDebugSymbolProvider_GetNextAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugSymbolProvider_INTERFACE_DEFINED__ */


#ifndef __IDebugComPlusSymbolProvider_INTERFACE_DEFINED__
#define __IDebugComPlusSymbolProvider_INTERFACE_DEFINED__

/* interface IDebugComPlusSymbolProvider */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugComPlusSymbolProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eaf-8b9d-11d2-9014-00c04fa38338")
    IDebugComPlusSymbolProvider : public IDebugSymbolProvider
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LoadSymbols( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ ULONGLONG baseAddress,
            /* [in] */ IUnknown *pUnkMetadataImport,
            /* [in] */ BSTR bstrModuleName,
            /* [in] */ BSTR bstrSymSearchPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnloadSymbols( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEntryPoint( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [out] */ IDebugAddress **ppAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTypeFromAddress( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateSymbols( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ IStream *pUpdateStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateTypeFromPrimitive( 
            /* [in] */ DWORD dwPrimType,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ IDebugField **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFunctionLineOffset( 
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ DWORD dwLine,
            /* [out] */ IDebugAddress **ppNewAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAddressesInModuleFromPosition( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetArrayTypeFromAddress( 
            /* [in] */ IDebugAddress *pAddress,
            /* [length_is][size_is][in] */ BYTE *pSig,
            /* [in] */ DWORD dwSigLength,
            /* [out] */ IDebugField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymAttribute( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ _mdToken tokParent,
            /* [in] */ LPOLESTR pstrName,
            /* [in] */ ULONG32 cBuffer,
            /* [out] */ ULONG32 *pcBuffer,
            /* [length_is][size_is][out] */ BYTE buffer[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplaceSymbols( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ IStream *pStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AreSymbolsLoaded( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LoadSymbolsFromStream( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ ULONGLONG baseAddress,
            /* [in] */ IUnknown *pUnkMetadataImport,
            /* [in] */ IStream *pStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymUnmanagedReader( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [out] */ IUnknown **ppSymUnmanagedReader) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributedClassesinModule( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ LPOLESTR pstrAttribute,
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributedClassesForLanguage( 
            /* [in] */ GUID guidLanguage,
            /* [in] */ LPOLESTR pstrAttribute,
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsHiddenCode( 
            /* [in] */ IDebugAddress *pAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsFunctionDeleted( 
            /* [in] */ IDebugAddress *pAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNameFromToken( 
            /* [in] */ IUnknown *pMetadataImport,
            /* [in] */ DWORD dwToken,
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsFunctionStale( 
            /* [in] */ IDebugAddress *pAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocalVariablelayout( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ ULONG32 cMethods,
            /* [size_is][in] */ _mdToken rgMethodTokens[  ],
            /* [out] */ IStream **pStreamLayout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAssemblyName( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [out] */ BSTR *pbstrName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugComPlusSymbolProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugComPlusSymbolProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugComPlusSymbolProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugEngineSymbolProviderServices *pServices);
        
        HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            IDebugComPlusSymbolProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerField )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetField )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ IDebugAddress *pAddressCur,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesFromPosition )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesFromContext )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugDocumentContext2 *pDocContext,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetContextFromAddress )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugDocumentContext2 **ppDocContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetLanguage )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ GUID *pguidLanguage,
            /* [out] */ GUID *pguidLanguageVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetGlobalContainer )( 
            IDebugComPlusSymbolProvider * This,
            /* [out] */ IDebugContainerField **pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodFieldsByName )( 
            IDebugComPlusSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszFullName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassTypeByName )( 
            IDebugComPlusSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugClassField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetNamespacesUsedAtAddress )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeByName )( 
            IDebugComPlusSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextAddress )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *LoadSymbols )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ ULONGLONG baseAddress,
            /* [in] */ IUnknown *pUnkMetadataImport,
            /* [in] */ BSTR bstrModuleName,
            /* [in] */ BSTR bstrSymSearchPath);
        
        HRESULT ( STDMETHODCALLTYPE *UnloadSymbols )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule);
        
        HRESULT ( STDMETHODCALLTYPE *GetEntryPoint )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeFromAddress )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateSymbols )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ IStream *pUpdateStream);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTypeFromPrimitive )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ DWORD dwPrimType,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetFunctionLineOffset )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ DWORD dwLine,
            /* [out] */ IDebugAddress **ppNewAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesInModuleFromPosition )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetArrayTypeFromAddress )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [length_is][size_is][in] */ BYTE *pSig,
            /* [in] */ DWORD dwSigLength,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetSymAttribute )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ _mdToken tokParent,
            /* [in] */ LPOLESTR pstrName,
            /* [in] */ ULONG32 cBuffer,
            /* [out] */ ULONG32 *pcBuffer,
            /* [length_is][size_is][out] */ BYTE buffer[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *ReplaceSymbols )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ IStream *pStream);
        
        HRESULT ( STDMETHODCALLTYPE *AreSymbolsLoaded )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule);
        
        HRESULT ( STDMETHODCALLTYPE *LoadSymbolsFromStream )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ ULONGLONG baseAddress,
            /* [in] */ IUnknown *pUnkMetadataImport,
            /* [in] */ IStream *pStream);
        
        HRESULT ( STDMETHODCALLTYPE *GetSymUnmanagedReader )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [out] */ IUnknown **ppSymUnmanagedReader);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributedClassesinModule )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ LPOLESTR pstrAttribute,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributedClassesForLanguage )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ GUID guidLanguage,
            /* [in] */ LPOLESTR pstrAttribute,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *IsHiddenCode )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress);
        
        HRESULT ( STDMETHODCALLTYPE *IsFunctionDeleted )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetNameFromToken )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IUnknown *pMetadataImport,
            /* [in] */ DWORD dwToken,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *IsFunctionStale )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocalVariablelayout )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ ULONG32 cMethods,
            /* [size_is][in] */ _mdToken rgMethodTokens[  ],
            /* [out] */ IStream **pStreamLayout);
        
        HRESULT ( STDMETHODCALLTYPE *GetAssemblyName )( 
            IDebugComPlusSymbolProvider * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [out] */ BSTR *pbstrName);
        
        END_INTERFACE
    } IDebugComPlusSymbolProviderVtbl;

    interface IDebugComPlusSymbolProvider
    {
        CONST_VTBL struct IDebugComPlusSymbolProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugComPlusSymbolProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugComPlusSymbolProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugComPlusSymbolProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugComPlusSymbolProvider_Initialize(This,pServices)	\
    (This)->lpVtbl -> Initialize(This,pServices)

#define IDebugComPlusSymbolProvider_Uninitialize(This)	\
    (This)->lpVtbl -> Uninitialize(This)

#define IDebugComPlusSymbolProvider_GetContainerField(This,pAddress,ppContainerField)	\
    (This)->lpVtbl -> GetContainerField(This,pAddress,ppContainerField)

#define IDebugComPlusSymbolProvider_GetField(This,pAddress,pAddressCur,ppField)	\
    (This)->lpVtbl -> GetField(This,pAddress,pAddressCur,ppField)

#define IDebugComPlusSymbolProvider_GetAddressesFromPosition(This,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesFromPosition(This,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugComPlusSymbolProvider_GetAddressesFromContext(This,pDocContext,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesFromContext(This,pDocContext,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugComPlusSymbolProvider_GetContextFromAddress(This,pAddress,ppDocContext)	\
    (This)->lpVtbl -> GetContextFromAddress(This,pAddress,ppDocContext)

#define IDebugComPlusSymbolProvider_GetLanguage(This,pAddress,pguidLanguage,pguidLanguageVendor)	\
    (This)->lpVtbl -> GetLanguage(This,pAddress,pguidLanguage,pguidLanguageVendor)

#define IDebugComPlusSymbolProvider_GetGlobalContainer(This,pField)	\
    (This)->lpVtbl -> GetGlobalContainer(This,pField)

#define IDebugComPlusSymbolProvider_GetMethodFieldsByName(This,pszFullName,nameMatch,ppEnum)	\
    (This)->lpVtbl -> GetMethodFieldsByName(This,pszFullName,nameMatch,ppEnum)

#define IDebugComPlusSymbolProvider_GetClassTypeByName(This,pszClassName,nameMatch,ppField)	\
    (This)->lpVtbl -> GetClassTypeByName(This,pszClassName,nameMatch,ppField)

#define IDebugComPlusSymbolProvider_GetNamespacesUsedAtAddress(This,pAddress,ppEnum)	\
    (This)->lpVtbl -> GetNamespacesUsedAtAddress(This,pAddress,ppEnum)

#define IDebugComPlusSymbolProvider_GetTypeByName(This,pszClassName,nameMatch,ppField)	\
    (This)->lpVtbl -> GetTypeByName(This,pszClassName,nameMatch,ppField)

#define IDebugComPlusSymbolProvider_GetNextAddress(This,pAddress,fStatmentOnly,ppAddress)	\
    (This)->lpVtbl -> GetNextAddress(This,pAddress,fStatmentOnly,ppAddress)


#define IDebugComPlusSymbolProvider_LoadSymbols(This,ulAppDomainID,guidModule,baseAddress,pUnkMetadataImport,bstrModuleName,bstrSymSearchPath)	\
    (This)->lpVtbl -> LoadSymbols(This,ulAppDomainID,guidModule,baseAddress,pUnkMetadataImport,bstrModuleName,bstrSymSearchPath)

#define IDebugComPlusSymbolProvider_UnloadSymbols(This,ulAppDomainID,guidModule)	\
    (This)->lpVtbl -> UnloadSymbols(This,ulAppDomainID,guidModule)

#define IDebugComPlusSymbolProvider_GetEntryPoint(This,ulAppDomainID,guidModule,ppAddress)	\
    (This)->lpVtbl -> GetEntryPoint(This,ulAppDomainID,guidModule,ppAddress)

#define IDebugComPlusSymbolProvider_GetTypeFromAddress(This,pAddress,ppField)	\
    (This)->lpVtbl -> GetTypeFromAddress(This,pAddress,ppField)

#define IDebugComPlusSymbolProvider_UpdateSymbols(This,ulAppDomainID,guidModule,pUpdateStream)	\
    (This)->lpVtbl -> UpdateSymbols(This,ulAppDomainID,guidModule,pUpdateStream)

#define IDebugComPlusSymbolProvider_CreateTypeFromPrimitive(This,dwPrimType,pAddress,ppType)	\
    (This)->lpVtbl -> CreateTypeFromPrimitive(This,dwPrimType,pAddress,ppType)

#define IDebugComPlusSymbolProvider_GetFunctionLineOffset(This,pAddress,dwLine,ppNewAddress)	\
    (This)->lpVtbl -> GetFunctionLineOffset(This,pAddress,dwLine,ppNewAddress)

#define IDebugComPlusSymbolProvider_GetAddressesInModuleFromPosition(This,ulAppDomainID,guidModule,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesInModuleFromPosition(This,ulAppDomainID,guidModule,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugComPlusSymbolProvider_GetArrayTypeFromAddress(This,pAddress,pSig,dwSigLength,ppField)	\
    (This)->lpVtbl -> GetArrayTypeFromAddress(This,pAddress,pSig,dwSigLength,ppField)

#define IDebugComPlusSymbolProvider_GetSymAttribute(This,ulAppDomainID,guidModule,tokParent,pstrName,cBuffer,pcBuffer,buffer)	\
    (This)->lpVtbl -> GetSymAttribute(This,ulAppDomainID,guidModule,tokParent,pstrName,cBuffer,pcBuffer,buffer)

#define IDebugComPlusSymbolProvider_ReplaceSymbols(This,ulAppDomainID,guidModule,pStream)	\
    (This)->lpVtbl -> ReplaceSymbols(This,ulAppDomainID,guidModule,pStream)

#define IDebugComPlusSymbolProvider_AreSymbolsLoaded(This,ulAppDomainID,guidModule)	\
    (This)->lpVtbl -> AreSymbolsLoaded(This,ulAppDomainID,guidModule)

#define IDebugComPlusSymbolProvider_LoadSymbolsFromStream(This,ulAppDomainID,guidModule,baseAddress,pUnkMetadataImport,pStream)	\
    (This)->lpVtbl -> LoadSymbolsFromStream(This,ulAppDomainID,guidModule,baseAddress,pUnkMetadataImport,pStream)

#define IDebugComPlusSymbolProvider_GetSymUnmanagedReader(This,ulAppDomainID,guidModule,ppSymUnmanagedReader)	\
    (This)->lpVtbl -> GetSymUnmanagedReader(This,ulAppDomainID,guidModule,ppSymUnmanagedReader)

#define IDebugComPlusSymbolProvider_GetAttributedClassesinModule(This,ulAppDomainID,guidModule,pstrAttribute,ppEnum)	\
    (This)->lpVtbl -> GetAttributedClassesinModule(This,ulAppDomainID,guidModule,pstrAttribute,ppEnum)

#define IDebugComPlusSymbolProvider_GetAttributedClassesForLanguage(This,guidLanguage,pstrAttribute,ppEnum)	\
    (This)->lpVtbl -> GetAttributedClassesForLanguage(This,guidLanguage,pstrAttribute,ppEnum)

#define IDebugComPlusSymbolProvider_IsHiddenCode(This,pAddress)	\
    (This)->lpVtbl -> IsHiddenCode(This,pAddress)

#define IDebugComPlusSymbolProvider_IsFunctionDeleted(This,pAddress)	\
    (This)->lpVtbl -> IsFunctionDeleted(This,pAddress)

#define IDebugComPlusSymbolProvider_GetNameFromToken(This,pMetadataImport,dwToken,pbstrName)	\
    (This)->lpVtbl -> GetNameFromToken(This,pMetadataImport,dwToken,pbstrName)

#define IDebugComPlusSymbolProvider_IsFunctionStale(This,pAddress)	\
    (This)->lpVtbl -> IsFunctionStale(This,pAddress)

#define IDebugComPlusSymbolProvider_GetLocalVariablelayout(This,ulAppDomainID,guidModule,cMethods,rgMethodTokens,pStreamLayout)	\
    (This)->lpVtbl -> GetLocalVariablelayout(This,ulAppDomainID,guidModule,cMethods,rgMethodTokens,pStreamLayout)

#define IDebugComPlusSymbolProvider_GetAssemblyName(This,ulAppDomainID,guidModule,pbstrName)	\
    (This)->lpVtbl -> GetAssemblyName(This,ulAppDomainID,guidModule,pbstrName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_LoadSymbols_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ ULONGLONG baseAddress,
    /* [in] */ IUnknown *pUnkMetadataImport,
    /* [in] */ BSTR bstrModuleName,
    /* [in] */ BSTR bstrSymSearchPath);


void __RPC_STUB IDebugComPlusSymbolProvider_LoadSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_UnloadSymbols_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule);


void __RPC_STUB IDebugComPlusSymbolProvider_UnloadSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetEntryPoint_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [out] */ IDebugAddress **ppAddress);


void __RPC_STUB IDebugComPlusSymbolProvider_GetEntryPoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetTypeFromAddress_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ IDebugField **ppField);


void __RPC_STUB IDebugComPlusSymbolProvider_GetTypeFromAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_UpdateSymbols_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ IStream *pUpdateStream);


void __RPC_STUB IDebugComPlusSymbolProvider_UpdateSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_CreateTypeFromPrimitive_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ DWORD dwPrimType,
    /* [in] */ IDebugAddress *pAddress,
    /* [in] */ IDebugField **ppType);


void __RPC_STUB IDebugComPlusSymbolProvider_CreateTypeFromPrimitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetFunctionLineOffset_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [in] */ DWORD dwLine,
    /* [out] */ IDebugAddress **ppNewAddress);


void __RPC_STUB IDebugComPlusSymbolProvider_GetFunctionLineOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetAddressesInModuleFromPosition_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ IDebugDocumentPosition2 *pDocPos,
    /* [in] */ BOOL fStatmentOnly,
    /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
    /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);


void __RPC_STUB IDebugComPlusSymbolProvider_GetAddressesInModuleFromPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetArrayTypeFromAddress_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [length_is][size_is][in] */ BYTE *pSig,
    /* [in] */ DWORD dwSigLength,
    /* [out] */ IDebugField **ppField);


void __RPC_STUB IDebugComPlusSymbolProvider_GetArrayTypeFromAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetSymAttribute_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ _mdToken tokParent,
    /* [in] */ LPOLESTR pstrName,
    /* [in] */ ULONG32 cBuffer,
    /* [out] */ ULONG32 *pcBuffer,
    /* [length_is][size_is][out] */ BYTE buffer[  ]);


void __RPC_STUB IDebugComPlusSymbolProvider_GetSymAttribute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_ReplaceSymbols_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ IStream *pStream);


void __RPC_STUB IDebugComPlusSymbolProvider_ReplaceSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_AreSymbolsLoaded_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule);


void __RPC_STUB IDebugComPlusSymbolProvider_AreSymbolsLoaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_LoadSymbolsFromStream_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ ULONGLONG baseAddress,
    /* [in] */ IUnknown *pUnkMetadataImport,
    /* [in] */ IStream *pStream);


void __RPC_STUB IDebugComPlusSymbolProvider_LoadSymbolsFromStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetSymUnmanagedReader_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [out] */ IUnknown **ppSymUnmanagedReader);


void __RPC_STUB IDebugComPlusSymbolProvider_GetSymUnmanagedReader_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetAttributedClassesinModule_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ LPOLESTR pstrAttribute,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugComPlusSymbolProvider_GetAttributedClassesinModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetAttributedClassesForLanguage_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ GUID guidLanguage,
    /* [in] */ LPOLESTR pstrAttribute,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugComPlusSymbolProvider_GetAttributedClassesForLanguage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_IsHiddenCode_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress);


void __RPC_STUB IDebugComPlusSymbolProvider_IsHiddenCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_IsFunctionDeleted_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress);


void __RPC_STUB IDebugComPlusSymbolProvider_IsFunctionDeleted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetNameFromToken_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IUnknown *pMetadataImport,
    /* [in] */ DWORD dwToken,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugComPlusSymbolProvider_GetNameFromToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_IsFunctionStale_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ IDebugAddress *pAddress);


void __RPC_STUB IDebugComPlusSymbolProvider_IsFunctionStale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetLocalVariablelayout_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ ULONG32 cMethods,
    /* [size_is][in] */ _mdToken rgMethodTokens[  ],
    /* [out] */ IStream **pStreamLayout);


void __RPC_STUB IDebugComPlusSymbolProvider_GetLocalVariablelayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSymbolProvider_GetAssemblyName_Proxy( 
    IDebugComPlusSymbolProvider * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugComPlusSymbolProvider_GetAssemblyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugComPlusSymbolProvider_INTERFACE_DEFINED__ */


#ifndef __IDebugNativeSymbolProvider_INTERFACE_DEFINED__
#define __IDebugNativeSymbolProvider_INTERFACE_DEFINED__

/* interface IDebugNativeSymbolProvider */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugNativeSymbolProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb0-8b9d-11d2-9014-00c04fa38338")
    IDebugNativeSymbolProvider : public IDebugSymbolProvider
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LoadSymbols( 
            /* [in] */ LPCOLESTR pszFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugNativeSymbolProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugNativeSymbolProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugNativeSymbolProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugEngineSymbolProviderServices *pServices);
        
        HRESULT ( STDMETHODCALLTYPE *Uninitialize )( 
            IDebugNativeSymbolProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainerField )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetField )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ IDebugAddress *pAddressCur,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesFromPosition )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddressesFromContext )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugDocumentContext2 *pDocContext,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IEnumDebugAddresses **ppEnumBegAddresses,
            /* [out] */ IEnumDebugAddresses **ppEnumEndAddresses);
        
        HRESULT ( STDMETHODCALLTYPE *GetContextFromAddress )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IDebugDocumentContext2 **ppDocContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetLanguage )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ GUID *pguidLanguage,
            /* [out] */ GUID *pguidLanguageVendor);
        
        HRESULT ( STDMETHODCALLTYPE *GetGlobalContainer )( 
            IDebugNativeSymbolProvider * This,
            /* [out] */ IDebugContainerField **pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetMethodFieldsByName )( 
            IDebugNativeSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszFullName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassTypeByName )( 
            IDebugNativeSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugClassField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetNamespacesUsedAtAddress )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeByName )( 
            IDebugNativeSymbolProvider * This,
            /* [full][in] */ LPCOLESTR pszClassName,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextAddress )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [in] */ BOOL fStatmentOnly,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *LoadSymbols )( 
            IDebugNativeSymbolProvider * This,
            /* [in] */ LPCOLESTR pszFileName);
        
        END_INTERFACE
    } IDebugNativeSymbolProviderVtbl;

    interface IDebugNativeSymbolProvider
    {
        CONST_VTBL struct IDebugNativeSymbolProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugNativeSymbolProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugNativeSymbolProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugNativeSymbolProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugNativeSymbolProvider_Initialize(This,pServices)	\
    (This)->lpVtbl -> Initialize(This,pServices)

#define IDebugNativeSymbolProvider_Uninitialize(This)	\
    (This)->lpVtbl -> Uninitialize(This)

#define IDebugNativeSymbolProvider_GetContainerField(This,pAddress,ppContainerField)	\
    (This)->lpVtbl -> GetContainerField(This,pAddress,ppContainerField)

#define IDebugNativeSymbolProvider_GetField(This,pAddress,pAddressCur,ppField)	\
    (This)->lpVtbl -> GetField(This,pAddress,pAddressCur,ppField)

#define IDebugNativeSymbolProvider_GetAddressesFromPosition(This,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesFromPosition(This,pDocPos,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugNativeSymbolProvider_GetAddressesFromContext(This,pDocContext,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)	\
    (This)->lpVtbl -> GetAddressesFromContext(This,pDocContext,fStatmentOnly,ppEnumBegAddresses,ppEnumEndAddresses)

#define IDebugNativeSymbolProvider_GetContextFromAddress(This,pAddress,ppDocContext)	\
    (This)->lpVtbl -> GetContextFromAddress(This,pAddress,ppDocContext)

#define IDebugNativeSymbolProvider_GetLanguage(This,pAddress,pguidLanguage,pguidLanguageVendor)	\
    (This)->lpVtbl -> GetLanguage(This,pAddress,pguidLanguage,pguidLanguageVendor)

#define IDebugNativeSymbolProvider_GetGlobalContainer(This,pField)	\
    (This)->lpVtbl -> GetGlobalContainer(This,pField)

#define IDebugNativeSymbolProvider_GetMethodFieldsByName(This,pszFullName,nameMatch,ppEnum)	\
    (This)->lpVtbl -> GetMethodFieldsByName(This,pszFullName,nameMatch,ppEnum)

#define IDebugNativeSymbolProvider_GetClassTypeByName(This,pszClassName,nameMatch,ppField)	\
    (This)->lpVtbl -> GetClassTypeByName(This,pszClassName,nameMatch,ppField)

#define IDebugNativeSymbolProvider_GetNamespacesUsedAtAddress(This,pAddress,ppEnum)	\
    (This)->lpVtbl -> GetNamespacesUsedAtAddress(This,pAddress,ppEnum)

#define IDebugNativeSymbolProvider_GetTypeByName(This,pszClassName,nameMatch,ppField)	\
    (This)->lpVtbl -> GetTypeByName(This,pszClassName,nameMatch,ppField)

#define IDebugNativeSymbolProvider_GetNextAddress(This,pAddress,fStatmentOnly,ppAddress)	\
    (This)->lpVtbl -> GetNextAddress(This,pAddress,fStatmentOnly,ppAddress)


#define IDebugNativeSymbolProvider_LoadSymbols(This,pszFileName)	\
    (This)->lpVtbl -> LoadSymbols(This,pszFileName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugNativeSymbolProvider_LoadSymbols_Proxy( 
    IDebugNativeSymbolProvider * This,
    /* [in] */ LPCOLESTR pszFileName);


void __RPC_STUB IDebugNativeSymbolProvider_LoadSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugNativeSymbolProvider_INTERFACE_DEFINED__ */


#ifndef __IDebugField_INTERFACE_DEFINED__
#define __IDebugField_INTERFACE_DEFINED__

/* interface IDebugField */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugField_0001
    {	FIELD_MOD_NONE	= 0,
	FIELD_MOD_ACCESS_NONE	= 0x1,
	FIELD_MOD_ACCESS_PUBLIC	= 0x2,
	FIELD_MOD_ACCESS_PROTECTED	= 0x4,
	FIELD_MOD_ACCESS_PRIVATE	= 0x8,
	FIELD_MOD_NOMODIFIERS	= 0x10,
	FIELD_MOD_STATIC	= 0x20,
	FIELD_MOD_CONSTANT	= 0x40,
	FIELD_MOD_TRANSIENT	= 0x80,
	FIELD_MOD_VOLATILE	= 0x100,
	FIELD_MOD_ABSTRACT	= 0x200,
	FIELD_MOD_NATIVE	= 0x400,
	FIELD_MOD_SYNCHRONIZED	= 0x800,
	FIELD_MOD_VIRTUAL	= 0x1000,
	FIELD_MOD_INTERFACE	= 0x2000,
	FIELD_MOD_FINAL	= 0x4000,
	FIELD_MOD_SENTINEL	= 0x8000,
	FIELD_MOD_INNERCLASS	= 0x10000,
	FIELD_MOD_OPTIONAL	= 0x20000,
	FIELD_MOD_BYREF	= 0x40000,
	FIELD_MOD_HIDDEN	= 0x80000,
	FIELD_MOD_MARSHALASOBJECT	= 0x100000,
	FIELD_MOD_WRITEONLY	= 0x80000000,
	FIELD_MOD_ACCESS_MASK	= 0xff,
	FIELD_MOD_MASK	= 0xffffff00,
	FIELD_MOD_ALL	= 0x7fffffff
    } ;
typedef DWORD FIELD_MODIFIERS;


enum __MIDL_IDebugField_0002
    {	FIELD_KIND_NONE	= 0,
	FIELD_KIND_TYPE	= 0x1,
	FIELD_KIND_SYMBOL	= 0x2,
	FIELD_TYPE_PRIMITIVE	= 0x10,
	FIELD_TYPE_STRUCT	= 0x20,
	FIELD_TYPE_CLASS	= 0x40,
	FIELD_TYPE_INTERFACE	= 0x80,
	FIELD_TYPE_UNION	= 0x100,
	FIELD_TYPE_ARRAY	= 0x200,
	FIELD_TYPE_METHOD	= 0x400,
	FIELD_TYPE_BLOCK	= 0x800,
	FIELD_TYPE_POINTER	= 0x1000,
	FIELD_TYPE_ENUM	= 0x2000,
	FIELD_TYPE_LABEL	= 0x4000,
	FIELD_TYPE_TYPEDEF	= 0x8000,
	FIELD_TYPE_BITFIELD	= 0x10000,
	FIELD_TYPE_NAMESPACE	= 0x20000,
	FIELD_TYPE_MODULE	= 0x40000,
	FIELD_TYPE_DYNAMIC	= 0x80000,
	FIELD_TYPE_PROP	= 0x100000,
	FIELD_TYPE_INNERCLASS	= 0x200000,
	FIELD_TYPE_REFERENCE	= 0x400000,
	FIELD_TYPE_EXTENDED	= 0x800000,
	FIELD_SYM_MEMBER	= 0x1000000,
	FIELD_SYM_LOCAL	= 0x2000000,
	FIELD_SYM_PARAM	= 0x4000000,
	FIELD_SYM_THIS	= 0x8000000,
	FIELD_SYM_GLOBAL	= 0x10000000,
	FIELD_SYM_PROP_GETTER	= 0x20000000,
	FIELD_SYM_PROP_SETTER	= 0x40000000,
	FIELD_SYM_EXTENED	= 0x80000000,
	FIELD_KIND_MASK	= 0xf,
	FIELD_TYPE_MASK	= 0xfffff0,
	FIELD_SYM_MASK	= 0xff000000,
	FIELD_KIND_ALL	= 0xffffffff
    } ;
typedef DWORD FIELD_KIND;


enum __MIDL_IDebugField_0003
    {	FIF_FULLNAME	= 0x1,
	FIF_NAME	= 0x2,
	FIF_TYPE	= 0x4,
	FIF_MODIFIERS	= 0x8,
	FIF_ALL	= 0xffffffff
    } ;
typedef DWORD FIELD_INFO_FIELDS;

typedef struct _tagFieldInfo
    {
    FIELD_INFO_FIELDS dwFields;
    BSTR bstrFullName;
    BSTR bstrName;
    BSTR bstrType;
    FIELD_MODIFIERS dwModifiers;
    } 	FIELD_INFO;


enum __MIDL_IDebugField_0004
    {	TYPE_KIND_METADATA	= 0x1,
	TYPE_KIND_PDB	= 0x2
    } ;
typedef DWORD dwTYPE_KIND;

typedef struct _tagTYPE_METADATA
    {
    ULONG32 ulAppDomainID;
    GUID guidModule;
    _mdToken tokClass;
    } 	METADATA_TYPE;

typedef struct _tagTYPE_PDB
    {
    ULONG32 ulAppDomainID;
    GUID guidModule;
    DWORD symid;
    } 	PDB_TYPE;

typedef struct _tagTYPE_INFO_UNION
    {
    dwTYPE_KIND dwKind;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IDebugField_0005
        {
        /* [case()] */ METADATA_TYPE typeMeta;
        /* [case()] */ PDB_TYPE typePdb;
        /* [default] */ DWORD unused;
        } 	type;
    } 	TYPE_INFO;


EXTERN_C const IID IID_IDebugField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb1-8b9d-11d2-9014-00c04fa38338")
    IDebugField : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKind( 
            /* [out] */ FIELD_KIND *pdwKind) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetType( 
            /* [out] */ IDebugField **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContainer( 
            /* [out] */ IDebugContainerField **ppContainerField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAddress( 
            /* [out] */ IDebugAddress **ppAddress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ DWORD *pdwSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtendedInfo( 
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Equal( 
            /* [in] */ IDebugField *pField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
            /* [out] */ TYPE_INFO *pTypeInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        END_INTERFACE
    } IDebugFieldVtbl;

    interface IDebugField
    {
        CONST_VTBL struct IDebugFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugField_GetInfo_Proxy( 
    IDebugField * This,
    /* [in] */ FIELD_INFO_FIELDS dwFields,
    /* [out] */ FIELD_INFO *pFieldInfo);


void __RPC_STUB IDebugField_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetKind_Proxy( 
    IDebugField * This,
    /* [out] */ FIELD_KIND *pdwKind);


void __RPC_STUB IDebugField_GetKind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetType_Proxy( 
    IDebugField * This,
    /* [out] */ IDebugField **ppType);


void __RPC_STUB IDebugField_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetContainer_Proxy( 
    IDebugField * This,
    /* [out] */ IDebugContainerField **ppContainerField);


void __RPC_STUB IDebugField_GetContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetAddress_Proxy( 
    IDebugField * This,
    /* [out] */ IDebugAddress **ppAddress);


void __RPC_STUB IDebugField_GetAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetSize_Proxy( 
    IDebugField * This,
    /* [out] */ DWORD *pdwSize);


void __RPC_STUB IDebugField_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetExtendedInfo_Proxy( 
    IDebugField * This,
    /* [in] */ REFGUID guidExtendedInfo,
    /* [length_is][size_is][out] */ BYTE **prgBuffer,
    /* [out][in] */ DWORD *pdwLen);


void __RPC_STUB IDebugField_GetExtendedInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_Equal_Proxy( 
    IDebugField * This,
    /* [in] */ IDebugField *pField);


void __RPC_STUB IDebugField_Equal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugField_GetTypeInfo_Proxy( 
    IDebugField * This,
    /* [out] */ TYPE_INFO *pTypeInfo);


void __RPC_STUB IDebugField_GetTypeInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugField_INTERFACE_DEFINED__ */


#ifndef __IDebugContainerField_INTERFACE_DEFINED__
#define __IDebugContainerField_INTERFACE_DEFINED__

/* interface IDebugContainerField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugContainerField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb2-8b9d-11d2-9014-00c04fa38338")
    IDebugContainerField : public IDebugField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumFields( 
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugContainerFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugContainerField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugContainerField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugContainerField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugContainerField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugContainerField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugContainerField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugContainerField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugContainerField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugContainerField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugContainerField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugContainerField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugContainerField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugContainerField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        END_INTERFACE
    } IDebugContainerFieldVtbl;

    interface IDebugContainerField
    {
        CONST_VTBL struct IDebugContainerFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugContainerField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugContainerField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugContainerField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugContainerField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugContainerField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugContainerField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugContainerField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugContainerField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugContainerField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugContainerField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugContainerField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugContainerField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugContainerField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugContainerField_EnumFields_Proxy( 
    IDebugContainerField * This,
    /* [in] */ FIELD_KIND dwKindFilter,
    /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
    /* [full][in] */ LPCOLESTR pszNameFilter,
    /* [in] */ NAME_MATCH nameMatch,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugContainerField_EnumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugContainerField_INTERFACE_DEFINED__ */


#ifndef __IDebugMethodField_INTERFACE_DEFINED__
#define __IDebugMethodField_INTERFACE_DEFINED__

/* interface IDebugMethodField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugMethodField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb4-8b9d-11d2-9014-00c04fa38338")
    IDebugMethodField : public IDebugContainerField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumParameters( 
            /* [out] */ IEnumDebugFields **ppParams) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThis( 
            /* [out] */ IDebugClassField **ppClass) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAllLocals( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppLocals) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumLocals( 
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppLocals) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsCustomAttributeDefined( 
            /* [full][in] */ LPCOLESTR pszCustomAttributeName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumStaticLocals( 
            /* [out] */ IEnumDebugFields **ppLocals) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetGlobalContainer( 
            /* [out] */ IDebugClassField **ppClass) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumArguments( 
            /* [out] */ IEnumDebugFields **ppParams) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMethodFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMethodField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMethodField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMethodField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugMethodField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugMethodField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugMethodField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugMethodField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugMethodField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugMethodField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugMethodField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugMethodField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugMethodField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugMethodField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumParameters )( 
            IDebugMethodField * This,
            /* [out] */ IEnumDebugFields **ppParams);
        
        HRESULT ( STDMETHODCALLTYPE *GetThis )( 
            IDebugMethodField * This,
            /* [out] */ IDebugClassField **ppClass);
        
        HRESULT ( STDMETHODCALLTYPE *EnumAllLocals )( 
            IDebugMethodField * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppLocals);
        
        HRESULT ( STDMETHODCALLTYPE *EnumLocals )( 
            IDebugMethodField * This,
            /* [in] */ IDebugAddress *pAddress,
            /* [out] */ IEnumDebugFields **ppLocals);
        
        HRESULT ( STDMETHODCALLTYPE *IsCustomAttributeDefined )( 
            IDebugMethodField * This,
            /* [full][in] */ LPCOLESTR pszCustomAttributeName);
        
        HRESULT ( STDMETHODCALLTYPE *EnumStaticLocals )( 
            IDebugMethodField * This,
            /* [out] */ IEnumDebugFields **ppLocals);
        
        HRESULT ( STDMETHODCALLTYPE *GetGlobalContainer )( 
            IDebugMethodField * This,
            /* [out] */ IDebugClassField **ppClass);
        
        HRESULT ( STDMETHODCALLTYPE *EnumArguments )( 
            IDebugMethodField * This,
            /* [out] */ IEnumDebugFields **ppParams);
        
        END_INTERFACE
    } IDebugMethodFieldVtbl;

    interface IDebugMethodField
    {
        CONST_VTBL struct IDebugMethodFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMethodField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMethodField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMethodField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMethodField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugMethodField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugMethodField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugMethodField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugMethodField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugMethodField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugMethodField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugMethodField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugMethodField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugMethodField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)


#define IDebugMethodField_EnumParameters(This,ppParams)	\
    (This)->lpVtbl -> EnumParameters(This,ppParams)

#define IDebugMethodField_GetThis(This,ppClass)	\
    (This)->lpVtbl -> GetThis(This,ppClass)

#define IDebugMethodField_EnumAllLocals(This,pAddress,ppLocals)	\
    (This)->lpVtbl -> EnumAllLocals(This,pAddress,ppLocals)

#define IDebugMethodField_EnumLocals(This,pAddress,ppLocals)	\
    (This)->lpVtbl -> EnumLocals(This,pAddress,ppLocals)

#define IDebugMethodField_IsCustomAttributeDefined(This,pszCustomAttributeName)	\
    (This)->lpVtbl -> IsCustomAttributeDefined(This,pszCustomAttributeName)

#define IDebugMethodField_EnumStaticLocals(This,ppLocals)	\
    (This)->lpVtbl -> EnumStaticLocals(This,ppLocals)

#define IDebugMethodField_GetGlobalContainer(This,ppClass)	\
    (This)->lpVtbl -> GetGlobalContainer(This,ppClass)

#define IDebugMethodField_EnumArguments(This,ppParams)	\
    (This)->lpVtbl -> EnumArguments(This,ppParams)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMethodField_EnumParameters_Proxy( 
    IDebugMethodField * This,
    /* [out] */ IEnumDebugFields **ppParams);


void __RPC_STUB IDebugMethodField_EnumParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_GetThis_Proxy( 
    IDebugMethodField * This,
    /* [out] */ IDebugClassField **ppClass);


void __RPC_STUB IDebugMethodField_GetThis_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_EnumAllLocals_Proxy( 
    IDebugMethodField * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ IEnumDebugFields **ppLocals);


void __RPC_STUB IDebugMethodField_EnumAllLocals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_EnumLocals_Proxy( 
    IDebugMethodField * This,
    /* [in] */ IDebugAddress *pAddress,
    /* [out] */ IEnumDebugFields **ppLocals);


void __RPC_STUB IDebugMethodField_EnumLocals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_IsCustomAttributeDefined_Proxy( 
    IDebugMethodField * This,
    /* [full][in] */ LPCOLESTR pszCustomAttributeName);


void __RPC_STUB IDebugMethodField_IsCustomAttributeDefined_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_EnumStaticLocals_Proxy( 
    IDebugMethodField * This,
    /* [out] */ IEnumDebugFields **ppLocals);


void __RPC_STUB IDebugMethodField_EnumStaticLocals_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_GetGlobalContainer_Proxy( 
    IDebugMethodField * This,
    /* [out] */ IDebugClassField **ppClass);


void __RPC_STUB IDebugMethodField_GetGlobalContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMethodField_EnumArguments_Proxy( 
    IDebugMethodField * This,
    /* [out] */ IEnumDebugFields **ppParams);


void __RPC_STUB IDebugMethodField_EnumArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMethodField_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_sh_0379 */
/* [local] */ 

typedef 
enum ConstructorMatchOptions
    {	crAll	= 0,
	crNonStatic	= crAll + 1,
	crStatic	= crNonStatic + 1
    } 	CONSTRUCTOR_ENUM;



extern RPC_IF_HANDLE __MIDL_itf_sh_0379_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_sh_0379_v0_0_s_ifspec;

#ifndef __IDebugClassField_INTERFACE_DEFINED__
#define __IDebugClassField_INTERFACE_DEFINED__

/* interface IDebugClassField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugClassField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb5-8b9d-11d2-9014-00c04fa38338")
    IDebugClassField : public IDebugContainerField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumBaseClasses( 
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoesInterfaceExist( 
            /* [full][in] */ LPCOLESTR pszInterfaceName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumNestedClasses( 
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEnclosingClass( 
            /* [out] */ IDebugClassField **ppClassField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumInterfacesImplemented( 
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumConstructors( 
            /* [in] */ CONSTRUCTOR_ENUM cMatch,
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultIndexer( 
            /* [out] */ BSTR *pbstrIndexer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumNestedEnums( 
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugClassFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugClassField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugClassField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugClassField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugClassField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugClassField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugClassField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugClassField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugClassField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugClassField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugClassField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugClassField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugClassField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugClassField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumBaseClasses )( 
            IDebugClassField * This,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *DoesInterfaceExist )( 
            IDebugClassField * This,
            /* [full][in] */ LPCOLESTR pszInterfaceName);
        
        HRESULT ( STDMETHODCALLTYPE *EnumNestedClasses )( 
            IDebugClassField * This,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnclosingClass )( 
            IDebugClassField * This,
            /* [out] */ IDebugClassField **ppClassField);
        
        HRESULT ( STDMETHODCALLTYPE *EnumInterfacesImplemented )( 
            IDebugClassField * This,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumConstructors )( 
            IDebugClassField * This,
            /* [in] */ CONSTRUCTOR_ENUM cMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultIndexer )( 
            IDebugClassField * This,
            /* [out] */ BSTR *pbstrIndexer);
        
        HRESULT ( STDMETHODCALLTYPE *EnumNestedEnums )( 
            IDebugClassField * This,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        END_INTERFACE
    } IDebugClassFieldVtbl;

    interface IDebugClassField
    {
        CONST_VTBL struct IDebugClassFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugClassField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugClassField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugClassField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugClassField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugClassField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugClassField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugClassField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugClassField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugClassField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugClassField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugClassField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugClassField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugClassField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)


#define IDebugClassField_EnumBaseClasses(This,ppEnum)	\
    (This)->lpVtbl -> EnumBaseClasses(This,ppEnum)

#define IDebugClassField_DoesInterfaceExist(This,pszInterfaceName)	\
    (This)->lpVtbl -> DoesInterfaceExist(This,pszInterfaceName)

#define IDebugClassField_EnumNestedClasses(This,ppEnum)	\
    (This)->lpVtbl -> EnumNestedClasses(This,ppEnum)

#define IDebugClassField_GetEnclosingClass(This,ppClassField)	\
    (This)->lpVtbl -> GetEnclosingClass(This,ppClassField)

#define IDebugClassField_EnumInterfacesImplemented(This,ppEnum)	\
    (This)->lpVtbl -> EnumInterfacesImplemented(This,ppEnum)

#define IDebugClassField_EnumConstructors(This,cMatch,ppEnum)	\
    (This)->lpVtbl -> EnumConstructors(This,cMatch,ppEnum)

#define IDebugClassField_GetDefaultIndexer(This,pbstrIndexer)	\
    (This)->lpVtbl -> GetDefaultIndexer(This,pbstrIndexer)

#define IDebugClassField_EnumNestedEnums(This,ppEnum)	\
    (This)->lpVtbl -> EnumNestedEnums(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugClassField_EnumBaseClasses_Proxy( 
    IDebugClassField * This,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugClassField_EnumBaseClasses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_DoesInterfaceExist_Proxy( 
    IDebugClassField * This,
    /* [full][in] */ LPCOLESTR pszInterfaceName);


void __RPC_STUB IDebugClassField_DoesInterfaceExist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_EnumNestedClasses_Proxy( 
    IDebugClassField * This,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugClassField_EnumNestedClasses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_GetEnclosingClass_Proxy( 
    IDebugClassField * This,
    /* [out] */ IDebugClassField **ppClassField);


void __RPC_STUB IDebugClassField_GetEnclosingClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_EnumInterfacesImplemented_Proxy( 
    IDebugClassField * This,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugClassField_EnumInterfacesImplemented_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_EnumConstructors_Proxy( 
    IDebugClassField * This,
    /* [in] */ CONSTRUCTOR_ENUM cMatch,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugClassField_EnumConstructors_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_GetDefaultIndexer_Proxy( 
    IDebugClassField * This,
    /* [out] */ BSTR *pbstrIndexer);


void __RPC_STUB IDebugClassField_GetDefaultIndexer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugClassField_EnumNestedEnums_Proxy( 
    IDebugClassField * This,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IDebugClassField_EnumNestedEnums_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugClassField_INTERFACE_DEFINED__ */


#ifndef __IDebugPropertyField_INTERFACE_DEFINED__
#define __IDebugPropertyField_INTERFACE_DEFINED__

/* interface IDebugPropertyField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPropertyField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb6-8b9d-11d2-9014-00c04fa38338")
    IDebugPropertyField : public IDebugContainerField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPropertyGetter( 
            /* [out] */ IDebugMethodField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPropertySetter( 
            /* [out] */ IDebugMethodField **ppField) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPropertyFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPropertyField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPropertyField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPropertyField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugPropertyField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugPropertyField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugPropertyField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugPropertyField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugPropertyField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugPropertyField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugPropertyField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugPropertyField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugPropertyField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugPropertyField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetPropertyGetter )( 
            IDebugPropertyField * This,
            /* [out] */ IDebugMethodField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetPropertySetter )( 
            IDebugPropertyField * This,
            /* [out] */ IDebugMethodField **ppField);
        
        END_INTERFACE
    } IDebugPropertyFieldVtbl;

    interface IDebugPropertyField
    {
        CONST_VTBL struct IDebugPropertyFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPropertyField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPropertyField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPropertyField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPropertyField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugPropertyField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugPropertyField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugPropertyField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugPropertyField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugPropertyField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugPropertyField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugPropertyField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugPropertyField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugPropertyField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)


#define IDebugPropertyField_GetPropertyGetter(This,ppField)	\
    (This)->lpVtbl -> GetPropertyGetter(This,ppField)

#define IDebugPropertyField_GetPropertySetter(This,ppField)	\
    (This)->lpVtbl -> GetPropertySetter(This,ppField)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPropertyField_GetPropertyGetter_Proxy( 
    IDebugPropertyField * This,
    /* [out] */ IDebugMethodField **ppField);


void __RPC_STUB IDebugPropertyField_GetPropertyGetter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPropertyField_GetPropertySetter_Proxy( 
    IDebugPropertyField * This,
    /* [out] */ IDebugMethodField **ppField);


void __RPC_STUB IDebugPropertyField_GetPropertySetter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPropertyField_INTERFACE_DEFINED__ */


#ifndef __IDebugArrayField_INTERFACE_DEFINED__
#define __IDebugArrayField_INTERFACE_DEFINED__

/* interface IDebugArrayField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugArrayField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb7-8b9d-11d2-9014-00c04fa38338")
    IDebugArrayField : public IDebugContainerField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetNumberOfElements( 
            /* [out] */ DWORD *pdwNumElements) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetElementType( 
            /* [out] */ IDebugField **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRank( 
            /* [out] */ DWORD *pdwRank) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugArrayFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugArrayField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugArrayField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugArrayField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugArrayField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugArrayField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugArrayField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugArrayField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugArrayField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugArrayField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugArrayField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugArrayField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugArrayField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugArrayField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetNumberOfElements )( 
            IDebugArrayField * This,
            /* [out] */ DWORD *pdwNumElements);
        
        HRESULT ( STDMETHODCALLTYPE *GetElementType )( 
            IDebugArrayField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetRank )( 
            IDebugArrayField * This,
            /* [out] */ DWORD *pdwRank);
        
        END_INTERFACE
    } IDebugArrayFieldVtbl;

    interface IDebugArrayField
    {
        CONST_VTBL struct IDebugArrayFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugArrayField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugArrayField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugArrayField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugArrayField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugArrayField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugArrayField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugArrayField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugArrayField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugArrayField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugArrayField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugArrayField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugArrayField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugArrayField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)


#define IDebugArrayField_GetNumberOfElements(This,pdwNumElements)	\
    (This)->lpVtbl -> GetNumberOfElements(This,pdwNumElements)

#define IDebugArrayField_GetElementType(This,ppType)	\
    (This)->lpVtbl -> GetElementType(This,ppType)

#define IDebugArrayField_GetRank(This,pdwRank)	\
    (This)->lpVtbl -> GetRank(This,pdwRank)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugArrayField_GetNumberOfElements_Proxy( 
    IDebugArrayField * This,
    /* [out] */ DWORD *pdwNumElements);


void __RPC_STUB IDebugArrayField_GetNumberOfElements_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugArrayField_GetElementType_Proxy( 
    IDebugArrayField * This,
    /* [out] */ IDebugField **ppType);


void __RPC_STUB IDebugArrayField_GetElementType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugArrayField_GetRank_Proxy( 
    IDebugArrayField * This,
    /* [out] */ DWORD *pdwRank);


void __RPC_STUB IDebugArrayField_GetRank_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugArrayField_INTERFACE_DEFINED__ */


#ifndef __IDebugPointerField_INTERFACE_DEFINED__
#define __IDebugPointerField_INTERFACE_DEFINED__

/* interface IDebugPointerField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPointerField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb8-8b9d-11d2-9014-00c04fa38338")
    IDebugPointerField : public IDebugContainerField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDereferencedField( 
            /* [out] */ IDebugField **ppField) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPointerFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPointerField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPointerField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPointerField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugPointerField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugPointerField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugPointerField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugPointerField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugPointerField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugPointerField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugPointerField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugPointerField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugPointerField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugPointerField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetDereferencedField )( 
            IDebugPointerField * This,
            /* [out] */ IDebugField **ppField);
        
        END_INTERFACE
    } IDebugPointerFieldVtbl;

    interface IDebugPointerField
    {
        CONST_VTBL struct IDebugPointerFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPointerField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPointerField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPointerField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPointerField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugPointerField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugPointerField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugPointerField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugPointerField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugPointerField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugPointerField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugPointerField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugPointerField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugPointerField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)


#define IDebugPointerField_GetDereferencedField(This,ppField)	\
    (This)->lpVtbl -> GetDereferencedField(This,ppField)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPointerField_GetDereferencedField_Proxy( 
    IDebugPointerField * This,
    /* [out] */ IDebugField **ppField);


void __RPC_STUB IDebugPointerField_GetDereferencedField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPointerField_INTERFACE_DEFINED__ */


#ifndef __IDebugEnumField_INTERFACE_DEFINED__
#define __IDebugEnumField_INTERFACE_DEFINED__

/* interface IDebugEnumField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugEnumField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eb9-8b9d-11d2-9014-00c04fa38338")
    IDebugEnumField : public IDebugContainerField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetUnderlyingSymbol( 
            /* [out] */ IDebugField **ppField) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStringFromValue( 
            /* [in] */ ULONGLONG value,
            /* [out] */ BSTR *pbstrValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValueFromString( 
            /* [full][in] */ LPCOLESTR pszValue,
            /* [out] */ ULONGLONG *pvalue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValueFromStringCaseInsensitive( 
            /* [full][in] */ LPCOLESTR pszValue,
            /* [out] */ ULONGLONG *pvalue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEnumFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEnumField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEnumField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEnumField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugEnumField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugEnumField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugEnumField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugEnumField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugEnumField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugEnumField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugEnumField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugEnumField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugEnumField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFields )( 
            IDebugEnumField * This,
            /* [in] */ FIELD_KIND dwKindFilter,
            /* [in] */ FIELD_MODIFIERS dwModifiersFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ NAME_MATCH nameMatch,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetUnderlyingSymbol )( 
            IDebugEnumField * This,
            /* [out] */ IDebugField **ppField);
        
        HRESULT ( STDMETHODCALLTYPE *GetStringFromValue )( 
            IDebugEnumField * This,
            /* [in] */ ULONGLONG value,
            /* [out] */ BSTR *pbstrValue);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueFromString )( 
            IDebugEnumField * This,
            /* [full][in] */ LPCOLESTR pszValue,
            /* [out] */ ULONGLONG *pvalue);
        
        HRESULT ( STDMETHODCALLTYPE *GetValueFromStringCaseInsensitive )( 
            IDebugEnumField * This,
            /* [full][in] */ LPCOLESTR pszValue,
            /* [out] */ ULONGLONG *pvalue);
        
        END_INTERFACE
    } IDebugEnumFieldVtbl;

    interface IDebugEnumField
    {
        CONST_VTBL struct IDebugEnumFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEnumField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEnumField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEnumField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEnumField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugEnumField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugEnumField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugEnumField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugEnumField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugEnumField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugEnumField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugEnumField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugEnumField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugEnumField_EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)	\
    (This)->lpVtbl -> EnumFields(This,dwKindFilter,dwModifiersFilter,pszNameFilter,nameMatch,ppEnum)


#define IDebugEnumField_GetUnderlyingSymbol(This,ppField)	\
    (This)->lpVtbl -> GetUnderlyingSymbol(This,ppField)

#define IDebugEnumField_GetStringFromValue(This,value,pbstrValue)	\
    (This)->lpVtbl -> GetStringFromValue(This,value,pbstrValue)

#define IDebugEnumField_GetValueFromString(This,pszValue,pvalue)	\
    (This)->lpVtbl -> GetValueFromString(This,pszValue,pvalue)

#define IDebugEnumField_GetValueFromStringCaseInsensitive(This,pszValue,pvalue)	\
    (This)->lpVtbl -> GetValueFromStringCaseInsensitive(This,pszValue,pvalue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEnumField_GetUnderlyingSymbol_Proxy( 
    IDebugEnumField * This,
    /* [out] */ IDebugField **ppField);


void __RPC_STUB IDebugEnumField_GetUnderlyingSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEnumField_GetStringFromValue_Proxy( 
    IDebugEnumField * This,
    /* [in] */ ULONGLONG value,
    /* [out] */ BSTR *pbstrValue);


void __RPC_STUB IDebugEnumField_GetStringFromValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEnumField_GetValueFromString_Proxy( 
    IDebugEnumField * This,
    /* [full][in] */ LPCOLESTR pszValue,
    /* [out] */ ULONGLONG *pvalue);


void __RPC_STUB IDebugEnumField_GetValueFromString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEnumField_GetValueFromStringCaseInsensitive_Proxy( 
    IDebugEnumField * This,
    /* [full][in] */ LPCOLESTR pszValue,
    /* [out] */ ULONGLONG *pvalue);


void __RPC_STUB IDebugEnumField_GetValueFromStringCaseInsensitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEnumField_INTERFACE_DEFINED__ */


#ifndef __IDebugBitField_INTERFACE_DEFINED__
#define __IDebugBitField_INTERFACE_DEFINED__

/* interface IDebugBitField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBitField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34eba-8b9d-11d2-9014-00c04fa38338")
    IDebugBitField : public IDebugField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetStart( 
            /* [out] */ DWORD *pdwBitOffset) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBitFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBitField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBitField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBitField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugBitField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugBitField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugBitField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugBitField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugBitField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugBitField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugBitField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugBitField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugBitField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetStart )( 
            IDebugBitField * This,
            /* [out] */ DWORD *pdwBitOffset);
        
        END_INTERFACE
    } IDebugBitFieldVtbl;

    interface IDebugBitField
    {
        CONST_VTBL struct IDebugBitFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBitField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBitField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBitField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBitField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugBitField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugBitField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugBitField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugBitField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugBitField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugBitField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugBitField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugBitField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#define IDebugBitField_GetStart(This,pdwBitOffset)	\
    (This)->lpVtbl -> GetStart(This,pdwBitOffset)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBitField_GetStart_Proxy( 
    IDebugBitField * This,
    /* [out] */ DWORD *pdwBitOffset);


void __RPC_STUB IDebugBitField_GetStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBitField_INTERFACE_DEFINED__ */


#ifndef __IDebugDynamicField_INTERFACE_DEFINED__
#define __IDebugDynamicField_INTERFACE_DEFINED__

/* interface IDebugDynamicField */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugDynamicField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B5A2A5EA-D5AB-11d2-9033-00C04FA302A1")
    IDebugDynamicField : public IDebugField
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugDynamicFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDynamicField * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDynamicField * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDynamicField * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugDynamicField * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugDynamicField * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugDynamicField * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugDynamicField * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugDynamicField * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugDynamicField * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugDynamicField * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugDynamicField * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugDynamicField * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        END_INTERFACE
    } IDebugDynamicFieldVtbl;

    interface IDebugDynamicField
    {
        CONST_VTBL struct IDebugDynamicFieldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDynamicField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDynamicField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDynamicField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDynamicField_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugDynamicField_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugDynamicField_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugDynamicField_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugDynamicField_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugDynamicField_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugDynamicField_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugDynamicField_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugDynamicField_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugDynamicField_INTERFACE_DEFINED__ */


#ifndef __IDebugDynamicFieldCOMPlus_INTERFACE_DEFINED__
#define __IDebugDynamicFieldCOMPlus_INTERFACE_DEFINED__

/* interface IDebugDynamicFieldCOMPlus */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugDynamicFieldCOMPlus;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B5B20820-E233-11d2-9037-00C04FA302A1")
    IDebugDynamicFieldCOMPlus : public IDebugDynamicField
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetTypeFromPrimitive( 
            /* [in] */ DWORD dwCorElementType,
            /* [out] */ IDebugField **ppType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTypeFromTypeDef( 
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ _mdToken tokClass,
            /* [out] */ IDebugField **ppType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDynamicFieldCOMPlusVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDynamicFieldCOMPlus * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDynamicFieldCOMPlus * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [in] */ FIELD_INFO_FIELDS dwFields,
            /* [out] */ FIELD_INFO *pFieldInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetKind )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [out] */ FIELD_KIND *pdwKind);
        
        HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [out] */ IDebugContainerField **ppContainerField);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [out] */ IDebugAddress **ppAddress);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [length_is][size_is][out] */ BYTE **prgBuffer,
            /* [out][in] */ DWORD *pdwLen);
        
        HRESULT ( STDMETHODCALLTYPE *Equal )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [in] */ IDebugField *pField);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [out] */ TYPE_INFO *pTypeInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeFromPrimitive )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [in] */ DWORD dwCorElementType,
            /* [out] */ IDebugField **ppType);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeFromTypeDef )( 
            IDebugDynamicFieldCOMPlus * This,
            /* [in] */ ULONG32 ulAppDomainID,
            /* [in] */ GUID guidModule,
            /* [in] */ _mdToken tokClass,
            /* [out] */ IDebugField **ppType);
        
        END_INTERFACE
    } IDebugDynamicFieldCOMPlusVtbl;

    interface IDebugDynamicFieldCOMPlus
    {
        CONST_VTBL struct IDebugDynamicFieldCOMPlusVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDynamicFieldCOMPlus_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDynamicFieldCOMPlus_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDynamicFieldCOMPlus_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDynamicFieldCOMPlus_GetInfo(This,dwFields,pFieldInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pFieldInfo)

#define IDebugDynamicFieldCOMPlus_GetKind(This,pdwKind)	\
    (This)->lpVtbl -> GetKind(This,pdwKind)

#define IDebugDynamicFieldCOMPlus_GetType(This,ppType)	\
    (This)->lpVtbl -> GetType(This,ppType)

#define IDebugDynamicFieldCOMPlus_GetContainer(This,ppContainerField)	\
    (This)->lpVtbl -> GetContainer(This,ppContainerField)

#define IDebugDynamicFieldCOMPlus_GetAddress(This,ppAddress)	\
    (This)->lpVtbl -> GetAddress(This,ppAddress)

#define IDebugDynamicFieldCOMPlus_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugDynamicFieldCOMPlus_GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,prgBuffer,pdwLen)

#define IDebugDynamicFieldCOMPlus_Equal(This,pField)	\
    (This)->lpVtbl -> Equal(This,pField)

#define IDebugDynamicFieldCOMPlus_GetTypeInfo(This,pTypeInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,pTypeInfo)



#define IDebugDynamicFieldCOMPlus_GetTypeFromPrimitive(This,dwCorElementType,ppType)	\
    (This)->lpVtbl -> GetTypeFromPrimitive(This,dwCorElementType,ppType)

#define IDebugDynamicFieldCOMPlus_GetTypeFromTypeDef(This,ulAppDomainID,guidModule,tokClass,ppType)	\
    (This)->lpVtbl -> GetTypeFromTypeDef(This,ulAppDomainID,guidModule,tokClass,ppType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDynamicFieldCOMPlus_GetTypeFromPrimitive_Proxy( 
    IDebugDynamicFieldCOMPlus * This,
    /* [in] */ DWORD dwCorElementType,
    /* [out] */ IDebugField **ppType);


void __RPC_STUB IDebugDynamicFieldCOMPlus_GetTypeFromPrimitive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDynamicFieldCOMPlus_GetTypeFromTypeDef_Proxy( 
    IDebugDynamicFieldCOMPlus * This,
    /* [in] */ ULONG32 ulAppDomainID,
    /* [in] */ GUID guidModule,
    /* [in] */ _mdToken tokClass,
    /* [out] */ IDebugField **ppType);


void __RPC_STUB IDebugDynamicFieldCOMPlus_GetTypeFromTypeDef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDynamicFieldCOMPlus_INTERFACE_DEFINED__ */


#ifndef __IDebugEngineSymbolProviderServices_INTERFACE_DEFINED__
#define __IDebugEngineSymbolProviderServices_INTERFACE_DEFINED__

/* interface IDebugEngineSymbolProviderServices */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugEngineSymbolProviderServices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("83919262-ACD6-11d2-9028-00C04FA302A1")
    IDebugEngineSymbolProviderServices : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumCodeContexts( 
            /* [length_is][size_is][in] */ IDebugAddress **rgpAddresses,
            /* [in] */ DWORD celtAddresses,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEngineSymbolProviderServicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEngineSymbolProviderServices * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEngineSymbolProviderServices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEngineSymbolProviderServices * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCodeContexts )( 
            IDebugEngineSymbolProviderServices * This,
            /* [length_is][size_is][in] */ IDebugAddress **rgpAddresses,
            /* [in] */ DWORD celtAddresses,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum);
        
        END_INTERFACE
    } IDebugEngineSymbolProviderServicesVtbl;

    interface IDebugEngineSymbolProviderServices
    {
        CONST_VTBL struct IDebugEngineSymbolProviderServicesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEngineSymbolProviderServices_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEngineSymbolProviderServices_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEngineSymbolProviderServices_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEngineSymbolProviderServices_EnumCodeContexts(This,rgpAddresses,celtAddresses,ppEnum)	\
    (This)->lpVtbl -> EnumCodeContexts(This,rgpAddresses,celtAddresses,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEngineSymbolProviderServices_EnumCodeContexts_Proxy( 
    IDebugEngineSymbolProviderServices * This,
    /* [length_is][size_is][in] */ IDebugAddress **rgpAddresses,
    /* [in] */ DWORD celtAddresses,
    /* [out] */ IEnumDebugCodeContexts2 **ppEnum);


void __RPC_STUB IDebugEngineSymbolProviderServices_EnumCodeContexts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEngineSymbolProviderServices_INTERFACE_DEFINED__ */


#ifndef __IDebugAddress_INTERFACE_DEFINED__
#define __IDebugAddress_INTERFACE_DEFINED__

/* interface IDebugAddress */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugAddress_0001
    {	ADDRESS_KIND_NATIVE	= 0x1,
	ADDRESS_KIND_UNMANAGED_THIS_RELATIVE	= 0x2,
	ADDRESS_KIND_UNMANAGED_PHYSICAL	= 0x5,
	ADDRESS_KIND_METADATA_METHOD	= 0x10,
	ADDRESS_KIND_METADATA_FIELD	= 0x11,
	ADDRESS_KIND_METADATA_LOCAL	= 0x12,
	ADDRESS_KIND_METADATA_PARAM	= 0x13,
	ADDRESS_KIND_METADATA_ARRAYELEM	= 0x14,
	ADDRESS_KIND_METADATA_RETVAL	= 0x15
    } ;
typedef DWORD ADDRESS_KIND;

typedef struct _tagNATIVE_ADDRESS
    {
    DWORD unknown;
    } 	NATIVE_ADDRESS;

typedef struct _tagUNMANAGED_THIS_RELATIVE
    {
    DWORD dwOffset;
    DWORD dwBitOffset;
    DWORD dwBitLength;
    } 	UNMANAGED_ADDRESS_THIS_RELATIVE;

typedef struct _tagUNMANAGED_ADDRESS_PHYSICAL
    {
    ULONGLONG offset;
    } 	UNMANAGED_ADDRESS_PHYSICAL;

typedef struct _tagMETADATA_ADDRESS_METHOD
    {
    _mdToken tokMethod;
    DWORD dwOffset;
    DWORD dwVersion;
    } 	METADATA_ADDRESS_METHOD;

typedef struct _tagMETADATA_ADDRESS_FIELD
    {
    _mdToken tokField;
    } 	METADATA_ADDRESS_FIELD;

typedef struct _tagMETADATA_ADDRESS_LOCAL
    {
    _mdToken tokMethod;
    IUnknown *pLocal;
    DWORD dwIndex;
    } 	METADATA_ADDRESS_LOCAL;

typedef struct _tagMETADATA_ADDRESS_PARAM
    {
    _mdToken tokMethod;
    _mdToken tokParam;
    DWORD dwIndex;
    } 	METADATA_ADDRESS_PARAM;

typedef struct _tagMETADATA_ADDRESS_ARRAYELEM
    {
    _mdToken tokMethod;
    DWORD dwIndex;
    } 	METADATA_ADDRESS_ARRAYELEM;

typedef struct _tagMETADATA_ADDRESS_RETVAL
    {
    _mdToken tokMethod;
    DWORD dwCorType;
    DWORD dwSigSize;
    BYTE rgSig[ 10 ];
    } 	METADATA_ADDRESS_RETVAL;

typedef struct _tagDEBUG_ADDRESS_UNION
    {
    ADDRESS_KIND dwKind;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IDebugAddress_0002
        {
        /* [case()] */ NATIVE_ADDRESS addrNative;
        /* [case()] */ UNMANAGED_ADDRESS_THIS_RELATIVE addrThisRel;
        /* [case()] */ UNMANAGED_ADDRESS_PHYSICAL addrUPhysical;
        /* [case()] */ METADATA_ADDRESS_METHOD addrMethod;
        /* [case()] */ METADATA_ADDRESS_FIELD addrField;
        /* [case()] */ METADATA_ADDRESS_LOCAL addrLocal;
        /* [case()] */ METADATA_ADDRESS_PARAM addrParam;
        /* [case()] */ METADATA_ADDRESS_ARRAYELEM addrArrayElem;
        /* [case()] */ METADATA_ADDRESS_RETVAL addrRetVal;
        /* [default] */ DWORD unused;
        } 	addr;
    } 	DEBUG_ADDRESS_UNION;

typedef struct _tagDEBUG_ADDRESS
    {
    ULONG32 ulAppDomainID;
    GUID guidModule;
    _mdToken tokClass;
    DEBUG_ADDRESS_UNION addr;
    } 	DEBUG_ADDRESS;


EXTERN_C const IID IID_IDebugAddress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34ebb-8b9d-11d2-9014-00c04fa38338")
    IDebugAddress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAddress( 
            /* [out] */ DEBUG_ADDRESS *pAddress) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugAddressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugAddress * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugAddress * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugAddress * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAddress )( 
            IDebugAddress * This,
            /* [out] */ DEBUG_ADDRESS *pAddress);
        
        END_INTERFACE
    } IDebugAddressVtbl;

    interface IDebugAddress
    {
        CONST_VTBL struct IDebugAddressVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugAddress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugAddress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugAddress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugAddress_GetAddress(This,pAddress)	\
    (This)->lpVtbl -> GetAddress(This,pAddress)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugAddress_GetAddress_Proxy( 
    IDebugAddress * This,
    /* [out] */ DEBUG_ADDRESS *pAddress);


void __RPC_STUB IDebugAddress_GetAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugAddress_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugFields_INTERFACE_DEFINED__
#define __IEnumDebugFields_INTERFACE_DEFINED__

/* interface IEnumDebugFields */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugFields;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34ebc-8b9d-11d2-9014-00c04fa38338")
    IEnumDebugFields : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugField **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugFields **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugFieldsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugFields * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugFields * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugFields * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugFields * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugField **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugFields * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugFields * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugFields * This,
            /* [out] */ IEnumDebugFields **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugFields * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugFieldsVtbl;

    interface IEnumDebugFields
    {
        CONST_VTBL struct IEnumDebugFieldsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugFields_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugFields_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugFields_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugFields_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugFields_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugFields_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugFields_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugFields_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugFields_Next_Proxy( 
    IEnumDebugFields * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugField **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugFields_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFields_Skip_Proxy( 
    IEnumDebugFields * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugFields_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFields_Reset_Proxy( 
    IEnumDebugFields * This);


void __RPC_STUB IEnumDebugFields_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFields_Clone_Proxy( 
    IEnumDebugFields * This,
    /* [out] */ IEnumDebugFields **ppEnum);


void __RPC_STUB IEnumDebugFields_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFields_GetCount_Proxy( 
    IEnumDebugFields * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugFields_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugFields_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugAddresses_INTERFACE_DEFINED__
#define __IEnumDebugAddresses_INTERFACE_DEFINED__

/* interface IEnumDebugAddresses */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugAddresses;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c2e34ebd-8b9d-11d2-9014-00c04fa38338")
    IEnumDebugAddresses : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugAddress **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugAddresses **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugAddressesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugAddresses * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugAddresses * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugAddresses * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugAddresses * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugAddress **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugAddresses * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugAddresses * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugAddresses * This,
            /* [out] */ IEnumDebugAddresses **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugAddresses * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugAddressesVtbl;

    interface IEnumDebugAddresses
    {
        CONST_VTBL struct IEnumDebugAddressesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugAddresses_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugAddresses_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugAddresses_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugAddresses_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugAddresses_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugAddresses_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugAddresses_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugAddresses_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugAddresses_Next_Proxy( 
    IEnumDebugAddresses * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugAddress **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugAddresses_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugAddresses_Skip_Proxy( 
    IEnumDebugAddresses * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugAddresses_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugAddresses_Reset_Proxy( 
    IEnumDebugAddresses * This);


void __RPC_STUB IEnumDebugAddresses_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugAddresses_Clone_Proxy( 
    IEnumDebugAddresses * This,
    /* [out] */ IEnumDebugAddresses **ppEnum);


void __RPC_STUB IEnumDebugAddresses_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugAddresses_GetCount_Proxy( 
    IEnumDebugAddresses * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugAddresses_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugAddresses_INTERFACE_DEFINED__ */


#ifndef __IDebugCustomAttributeQuery_INTERFACE_DEFINED__
#define __IDebugCustomAttributeQuery_INTERFACE_DEFINED__

/* interface IDebugCustomAttributeQuery */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugCustomAttributeQuery;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DFD37B5A-1E3A-4f15-8098-220ABADC620B")
    IDebugCustomAttributeQuery : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsCustomAttributeDefined( 
            /* [full][in] */ LPCOLESTR pszCustomAttributeName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCustomAttributeByName( 
            /* [full][in] */ LPCOLESTR pszCustomAttributeName,
            /* [length_is][size_is][out][in] */ BYTE *ppBlob,
            /* [out][in] */ DWORD *pdwLen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugCustomAttributeQueryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugCustomAttributeQuery * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugCustomAttributeQuery * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugCustomAttributeQuery * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsCustomAttributeDefined )( 
            IDebugCustomAttributeQuery * This,
            /* [full][in] */ LPCOLESTR pszCustomAttributeName);
        
        HRESULT ( STDMETHODCALLTYPE *GetCustomAttributeByName )( 
            IDebugCustomAttributeQuery * This,
            /* [full][in] */ LPCOLESTR pszCustomAttributeName,
            /* [length_is][size_is][out][in] */ BYTE *ppBlob,
            /* [out][in] */ DWORD *pdwLen);
        
        END_INTERFACE
    } IDebugCustomAttributeQueryVtbl;

    interface IDebugCustomAttributeQuery
    {
        CONST_VTBL struct IDebugCustomAttributeQueryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugCustomAttributeQuery_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugCustomAttributeQuery_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugCustomAttributeQuery_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugCustomAttributeQuery_IsCustomAttributeDefined(This,pszCustomAttributeName)	\
    (This)->lpVtbl -> IsCustomAttributeDefined(This,pszCustomAttributeName)

#define IDebugCustomAttributeQuery_GetCustomAttributeByName(This,pszCustomAttributeName,ppBlob,pdwLen)	\
    (This)->lpVtbl -> GetCustomAttributeByName(This,pszCustomAttributeName,ppBlob,pdwLen)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugCustomAttributeQuery_IsCustomAttributeDefined_Proxy( 
    IDebugCustomAttributeQuery * This,
    /* [full][in] */ LPCOLESTR pszCustomAttributeName);


void __RPC_STUB IDebugCustomAttributeQuery_IsCustomAttributeDefined_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugCustomAttributeQuery_GetCustomAttributeByName_Proxy( 
    IDebugCustomAttributeQuery * This,
    /* [full][in] */ LPCOLESTR pszCustomAttributeName,
    /* [length_is][size_is][out][in] */ BYTE *ppBlob,
    /* [out][in] */ DWORD *pdwLen);


void __RPC_STUB IDebugCustomAttributeQuery_GetCustomAttributeByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugCustomAttributeQuery_INTERFACE_DEFINED__ */



#ifndef __SHLib_LIBRARY_DEFINED__
#define __SHLib_LIBRARY_DEFINED__

/* library SHLib */
/* [uuid] */ 


EXTERN_C const IID LIBID_SHLib;

EXTERN_C const CLSID CLSID_SHManaged;

#ifdef __cplusplus

class DECLSPEC_UUID("c2e34ebf-8b9d-11d2-9014-00c04fa38338")
SHManaged;
#endif
#endif /* __SHLib_LIBRARY_DEFINED__ */

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


