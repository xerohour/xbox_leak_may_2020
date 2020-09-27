
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0266 */
/* at Thu Jun 03 10:51:13 1999
 */
/* Compiler settings for adsp.odl:
    Os (OptLev=s), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifndef __iadsp_h__
#define __iadsp_h__

/* Forward Declarations */ 

#ifndef __IADsValue_FWD_DEFINED__
#define __IADsValue_FWD_DEFINED__
typedef interface IADsValue IADsValue;
#endif 	/* __IADsValue_FWD_DEFINED__ */


#ifndef __IADsObjOptPrivate_FWD_DEFINED__
#define __IADsObjOptPrivate_FWD_DEFINED__
typedef interface IADsObjOptPrivate IADsObjOptPrivate;
#endif 	/* __IADsObjOptPrivate_FWD_DEFINED__ */


#ifndef __IADsPathnameProvider_FWD_DEFINED__
#define __IADsPathnameProvider_FWD_DEFINED__
typedef interface IADsPathnameProvider IADsPathnameProvider;
#endif 	/* __IADsPathnameProvider_FWD_DEFINED__ */


#ifndef __PathnameProvider_FWD_DEFINED__
#define __PathnameProvider_FWD_DEFINED__

#ifdef __cplusplus
typedef class PathnameProvider PathnameProvider;
#else
typedef struct PathnameProvider PathnameProvider;
#endif /* __cplusplus */

#endif 	/* __PathnameProvider_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_adsp_0000 */
/* [local] */ 

#define IID_IDirectoryAttrMgmt IID_IDirectorySchemaMgmt
#define IDirectoryAttrMgmt IDirectorySchemaMgmt


extern RPC_IF_HANDLE __MIDL_itf_adsp_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_adsp_0000_v0_0_s_ifspec;


#ifndef __ActiveDsP_LIBRARY_DEFINED__
#define __ActiveDsP_LIBRARY_DEFINED__

/* library ActiveDsP */
/* [helpstring][version][uuid] */ 

typedef /* [public] */ 
enum __MIDL___MIDL_itf_adsp_0000_0001
    {	ADS_PRIVATE_OPTION_SPECIFIC_SERVER	= 101,
	ADS_PRIVATE_OPTION_KEEP_HANDLES	= 102
    }	ADS_PRIVATE_OPTION_ENUM;

typedef struct _path_component
    {
    LPTSTR szComponent;
    LPTSTR szValue;
    }	PATH_COMPONENT;

typedef struct _path_component __RPC_FAR *PPATH_COMPONENT;

typedef struct _path_objectinfo
    {
    LPTSTR ProviderName;
    LPTSTR ServerName;
    LPTSTR DisplayServerName;
    DWORD dwPathType;
    DWORD NumComponents;
    PATH_COMPONENT ComponentArray[ 64 ];
    PATH_COMPONENT DisplayComponentArray[ 64 ];
    PATH_COMPONENT ProvSpecComponentArray[ 64 ];
    }	PATH_OBJECTINFO;

typedef struct _path_objectinfo __RPC_FAR *PPATH_OBJECTINFO;


enum __MIDL___MIDL_itf_adsp_0109_0001
    {	ADS_PARSE_FULL	= 1,
	ADS_PARSE_DN	= 2,
	ADS_PARSE_COMPONENT	= 3
    };

enum __MIDL___MIDL_itf_adsp_0109_0002
    {	ADS_PATHTYPE_ROOTFIRST	= 1,
	ADS_PATHTYPE_LEAFFIRST	= 2
    };

enum __MIDL___MIDL_itf_adsp_0109_0003
    {	ADS_CONSTRUCT_NAMINGATTRIBUTE	= 1
    };

EXTERN_C const IID LIBID_ActiveDsP;

#ifndef __IADsValue_INTERFACE_DEFINED__
#define __IADsValue_INTERFACE_DEFINED__

/* interface IADsValue */
/* [object][uuid] */ 


EXTERN_C const IID IID_IADsValue;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1e3ef0aa-aef5-11d0-8537-00c04fd8d503")
    IADsValue : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ConvertADsValueToPropertyValue( 
            PADSVALUE pADsValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConvertPropertyValueToADsValue( 
            PADSVALUE pADsValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConvertPropertyValueToADsValue2( 
            PADSVALUE pADsValue,
            BSTR pszServerName,
            BSTR userName,
            BSTR passWord,
            LONG flags,
            BOOL fNTDSType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IADsValueVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IADsValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IADsValue __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IADsValue __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertADsValueToPropertyValue )( 
            IADsValue __RPC_FAR * This,
            PADSVALUE pADsValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertPropertyValueToADsValue )( 
            IADsValue __RPC_FAR * This,
            PADSVALUE pADsValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConvertPropertyValueToADsValue2 )( 
            IADsValue __RPC_FAR * This,
            PADSVALUE pADsValue,
            BSTR pszServerName,
            BSTR userName,
            BSTR passWord,
            LONG flags,
            BOOL fNTDSType);
        
        END_INTERFACE
    } IADsValueVtbl;

    interface IADsValue
    {
        CONST_VTBL struct IADsValueVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IADsValue_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IADsValue_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IADsValue_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IADsValue_ConvertADsValueToPropertyValue(This,pADsValue)	\
    (This)->lpVtbl -> ConvertADsValueToPropertyValue(This,pADsValue)

#define IADsValue_ConvertPropertyValueToADsValue(This,pADsValue)	\
    (This)->lpVtbl -> ConvertPropertyValueToADsValue(This,pADsValue)

#define IADsValue_ConvertPropertyValueToADsValue2(This,pADsValue,pszServerName,userName,passWord,flags,fNTDSType)	\
    (This)->lpVtbl -> ConvertPropertyValueToADsValue2(This,pADsValue,pszServerName,userName,passWord,flags,fNTDSType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IADsValue_ConvertADsValueToPropertyValue_Proxy( 
    IADsValue __RPC_FAR * This,
    PADSVALUE pADsValue);


void __RPC_STUB IADsValue_ConvertADsValueToPropertyValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IADsValue_ConvertPropertyValueToADsValue_Proxy( 
    IADsValue __RPC_FAR * This,
    PADSVALUE pADsValue);


void __RPC_STUB IADsValue_ConvertPropertyValueToADsValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IADsValue_ConvertPropertyValueToADsValue2_Proxy( 
    IADsValue __RPC_FAR * This,
    PADSVALUE pADsValue,
    BSTR pszServerName,
    BSTR userName,
    BSTR passWord,
    LONG flags,
    BOOL fNTDSType);


void __RPC_STUB IADsValue_ConvertPropertyValueToADsValue2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IADsValue_INTERFACE_DEFINED__ */


#ifndef __IADsObjOptPrivate_INTERFACE_DEFINED__
#define __IADsObjOptPrivate_INTERFACE_DEFINED__

/* interface IADsObjOptPrivate */
/* [object][uuid] */ 


EXTERN_C const IID IID_IADsObjOptPrivate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("46f14fda-232b-11d1-a808-00c04fd8d5a8")
    IADsObjOptPrivate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetOption( 
            DWORD dwOption,
            void __RPC_FAR *pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetOption( 
            DWORD dwOption,
            void __RPC_FAR *pValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IADsObjOptPrivateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IADsObjOptPrivate __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IADsObjOptPrivate __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IADsObjOptPrivate __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetOption )( 
            IADsObjOptPrivate __RPC_FAR * This,
            DWORD dwOption,
            void __RPC_FAR *pValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetOption )( 
            IADsObjOptPrivate __RPC_FAR * This,
            DWORD dwOption,
            void __RPC_FAR *pValue);
        
        END_INTERFACE
    } IADsObjOptPrivateVtbl;

    interface IADsObjOptPrivate
    {
        CONST_VTBL struct IADsObjOptPrivateVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IADsObjOptPrivate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IADsObjOptPrivate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IADsObjOptPrivate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IADsObjOptPrivate_GetOption(This,dwOption,pValue)	\
    (This)->lpVtbl -> GetOption(This,dwOption,pValue)

#define IADsObjOptPrivate_SetOption(This,dwOption,pValue)	\
    (This)->lpVtbl -> SetOption(This,dwOption,pValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IADsObjOptPrivate_GetOption_Proxy( 
    IADsObjOptPrivate __RPC_FAR * This,
    DWORD dwOption,
    void __RPC_FAR *pValue);


void __RPC_STUB IADsObjOptPrivate_GetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IADsObjOptPrivate_SetOption_Proxy( 
    IADsObjOptPrivate __RPC_FAR * This,
    DWORD dwOption,
    void __RPC_FAR *pValue);


void __RPC_STUB IADsObjOptPrivate_SetOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IADsObjOptPrivate_INTERFACE_DEFINED__ */


#ifndef __IADsPathnameProvider_INTERFACE_DEFINED__
#define __IADsPathnameProvider_INTERFACE_DEFINED__

/* interface IADsPathnameProvider */
/* [object][uuid] */ 


EXTERN_C const IID IID_IADsPathnameProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("aacd1d30-8bd0-11d2-92a9-00c04f79f834")
    IADsPathnameProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParsePath( 
            /* [in] */ BSTR bstrPath,
            /* [in] */ DWORD dwType,
            /* [in] */ PPATH_OBJECTINFO pObjectInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ConstructPath( 
            /* [in] */ PPATH_OBJECTINFO pObjectInfo,
            /* [in] */ DWORD dwType,
            /* [in] */ DWORD dwFlag,
            /* [in] */ DWORD dwEscapedMode,
            /* [out] */ BSTR __RPC_FAR *pbstrPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEscapedElement( 
            /* [in] */ LONG lnReserved,
            /* [in] */ BSTR bstrInStr,
            /* [out] */ BSTR __RPC_FAR *pbstrOutStr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IADsPathnameProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IADsPathnameProvider __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IADsPathnameProvider __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IADsPathnameProvider __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ParsePath )( 
            IADsPathnameProvider __RPC_FAR * This,
            /* [in] */ BSTR bstrPath,
            /* [in] */ DWORD dwType,
            /* [in] */ PPATH_OBJECTINFO pObjectInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConstructPath )( 
            IADsPathnameProvider __RPC_FAR * This,
            /* [in] */ PPATH_OBJECTINFO pObjectInfo,
            /* [in] */ DWORD dwType,
            /* [in] */ DWORD dwFlag,
            /* [in] */ DWORD dwEscapedMode,
            /* [out] */ BSTR __RPC_FAR *pbstrPath);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetEscapedElement )( 
            IADsPathnameProvider __RPC_FAR * This,
            /* [in] */ LONG lnReserved,
            /* [in] */ BSTR bstrInStr,
            /* [out] */ BSTR __RPC_FAR *pbstrOutStr);
        
        END_INTERFACE
    } IADsPathnameProviderVtbl;

    interface IADsPathnameProvider
    {
        CONST_VTBL struct IADsPathnameProviderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IADsPathnameProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IADsPathnameProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IADsPathnameProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IADsPathnameProvider_ParsePath(This,bstrPath,dwType,pObjectInfo)	\
    (This)->lpVtbl -> ParsePath(This,bstrPath,dwType,pObjectInfo)

#define IADsPathnameProvider_ConstructPath(This,pObjectInfo,dwType,dwFlag,dwEscapedMode,pbstrPath)	\
    (This)->lpVtbl -> ConstructPath(This,pObjectInfo,dwType,dwFlag,dwEscapedMode,pbstrPath)

#define IADsPathnameProvider_GetEscapedElement(This,lnReserved,bstrInStr,pbstrOutStr)	\
    (This)->lpVtbl -> GetEscapedElement(This,lnReserved,bstrInStr,pbstrOutStr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IADsPathnameProvider_ParsePath_Proxy( 
    IADsPathnameProvider __RPC_FAR * This,
    /* [in] */ BSTR bstrPath,
    /* [in] */ DWORD dwType,
    /* [in] */ PPATH_OBJECTINFO pObjectInfo);


void __RPC_STUB IADsPathnameProvider_ParsePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IADsPathnameProvider_ConstructPath_Proxy( 
    IADsPathnameProvider __RPC_FAR * This,
    /* [in] */ PPATH_OBJECTINFO pObjectInfo,
    /* [in] */ DWORD dwType,
    /* [in] */ DWORD dwFlag,
    /* [in] */ DWORD dwEscapedMode,
    /* [out] */ BSTR __RPC_FAR *pbstrPath);


void __RPC_STUB IADsPathnameProvider_ConstructPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IADsPathnameProvider_GetEscapedElement_Proxy( 
    IADsPathnameProvider __RPC_FAR * This,
    /* [in] */ LONG lnReserved,
    /* [in] */ BSTR bstrInStr,
    /* [out] */ BSTR __RPC_FAR *pbstrOutStr);


void __RPC_STUB IADsPathnameProvider_GetEscapedElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IADsPathnameProvider_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PathnameProvider;

#ifdef __cplusplus

class DECLSPEC_UUID("b4f5e650-8bd0-11d2-92a9-00c04f79f834")
PathnameProvider;
#endif
#endif /* __ActiveDsP_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


