/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Thu Apr 23 13:09:34 1998
 */
/* Compiler settings for ssevent.idl:
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

#ifndef __ssevent_h__
#define __ssevent_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISiteServerEvent_FWD_DEFINED__
#define __ISiteServerEvent_FWD_DEFINED__
typedef interface ISiteServerEvent ISiteServerEvent;
#endif 	/* __ISiteServerEvent_FWD_DEFINED__ */


#ifndef __ISiteServerEventProperties_FWD_DEFINED__
#define __ISiteServerEventProperties_FWD_DEFINED__
typedef interface ISiteServerEventProperties ISiteServerEventProperties;
#endif 	/* __ISiteServerEventProperties_FWD_DEFINED__ */


#ifndef __SiteServerEvent_FWD_DEFINED__
#define __SiteServerEvent_FWD_DEFINED__

#ifdef __cplusplus
typedef class SiteServerEvent SiteServerEvent;
#else
typedef struct SiteServerEvent SiteServerEvent;
#endif /* __cplusplus */

#endif 	/* __SiteServerEvent_FWD_DEFINED__ */


#ifndef __SiteServerEnumEventProperties_FWD_DEFINED__
#define __SiteServerEnumEventProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class SiteServerEnumEventProperties SiteServerEnumEventProperties;
#else
typedef struct SiteServerEnumEventProperties SiteServerEnumEventProperties;
#endif /* __cplusplus */

#endif 	/* __SiteServerEnumEventProperties_FWD_DEFINED__ */


#ifndef __SiteServerEventProperties_FWD_DEFINED__
#define __SiteServerEventProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class SiteServerEventProperties SiteServerEventProperties;
#else
typedef struct SiteServerEventProperties SiteServerEventProperties;
#endif /* __cplusplus */

#endif 	/* __SiteServerEventProperties_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ISiteServerEvent_INTERFACE_DEFINED__
#define __ISiteServerEvent_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISiteServerEvent
 * at Thu Apr 23 13:09:34 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_ISiteServerEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("F03E09B3-CCAC-11D0-BAF6-00C04FD7082F")
    ISiteServerEvent : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [string][in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvPropertyVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Write( 
            /* [string][in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *pvPropertyVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_NumProperties( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_PropertyNames( 
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISiteServerEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISiteServerEvent __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISiteServerEvent __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [string][in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *pvPropertyVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Write )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [string][in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *pvPropertyVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NumProperties )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PropertyNames )( 
            ISiteServerEvent __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        END_INTERFACE
    } ISiteServerEventVtbl;

    interface ISiteServerEvent
    {
        CONST_VTBL struct ISiteServerEventVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISiteServerEvent_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISiteServerEvent_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISiteServerEvent_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISiteServerEvent_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISiteServerEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISiteServerEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISiteServerEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISiteServerEvent_Read(This,bstrPropertyName,pvPropertyVal)	\
    (This)->lpVtbl -> Read(This,bstrPropertyName,pvPropertyVal)

#define ISiteServerEvent_Write(This,bstrPropertyName,pvPropertyVal)	\
    (This)->lpVtbl -> Write(This,bstrPropertyName,pvPropertyVal)

#define ISiteServerEvent_get_NumProperties(This,pVal)	\
    (This)->lpVtbl -> get_NumProperties(This,pVal)

#define ISiteServerEvent_get_PropertyNames(This,pVal)	\
    (This)->lpVtbl -> get_PropertyNames(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISiteServerEvent_Read_Proxy( 
    ISiteServerEvent __RPC_FAR * This,
    /* [string][in] */ BSTR bstrPropertyName,
    /* [retval][out] */ VARIANT __RPC_FAR *pvPropertyVal);


void __RPC_STUB ISiteServerEvent_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISiteServerEvent_Write_Proxy( 
    ISiteServerEvent __RPC_FAR * This,
    /* [string][in] */ BSTR bstrPropertyName,
    /* [in] */ VARIANT __RPC_FAR *pvPropertyVal);


void __RPC_STUB ISiteServerEvent_Write_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISiteServerEvent_get_NumProperties_Proxy( 
    ISiteServerEvent __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB ISiteServerEvent_get_NumProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISiteServerEvent_get_PropertyNames_Proxy( 
    ISiteServerEvent __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB ISiteServerEvent_get_PropertyNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISiteServerEvent_INTERFACE_DEFINED__ */


#ifndef __ISiteServerEventProperties_INTERFACE_DEFINED__
#define __ISiteServerEventProperties_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ISiteServerEventProperties
 * at Thu Apr 23 13:09:34 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][uuid][object] */ 



EXTERN_C const IID IID_ISiteServerEventProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("73CF5FFE-129B-11D1-8C86-00C04FC2D3B9")
    ISiteServerEventProperties : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ long Index,
            /* [retval][out] */ VARIANT __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *retval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISiteServerEventPropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISiteServerEventProperties __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISiteServerEventProperties __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [in] */ long Index,
            /* [retval][out] */ VARIANT __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *_NewEnum )( 
            ISiteServerEventProperties __RPC_FAR * This,
            /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *retval);
        
        END_INTERFACE
    } ISiteServerEventPropertiesVtbl;

    interface ISiteServerEventProperties
    {
        CONST_VTBL struct ISiteServerEventPropertiesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISiteServerEventProperties_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISiteServerEventProperties_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISiteServerEventProperties_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISiteServerEventProperties_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISiteServerEventProperties_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISiteServerEventProperties_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISiteServerEventProperties_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISiteServerEventProperties_get_Count(This,retval)	\
    (This)->lpVtbl -> get_Count(This,retval)

#define ISiteServerEventProperties_get_Item(This,Index,retval)	\
    (This)->lpVtbl -> get_Item(This,Index,retval)

#define ISiteServerEventProperties__NewEnum(This,retval)	\
    (This)->lpVtbl -> _NewEnum(This,retval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISiteServerEventProperties_get_Count_Proxy( 
    ISiteServerEventProperties __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB ISiteServerEventProperties_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISiteServerEventProperties_get_Item_Proxy( 
    ISiteServerEventProperties __RPC_FAR * This,
    /* [in] */ long Index,
    /* [retval][out] */ VARIANT __RPC_FAR *retval);


void __RPC_STUB ISiteServerEventProperties_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISiteServerEventProperties__NewEnum_Proxy( 
    ISiteServerEventProperties __RPC_FAR * This,
    /* [retval][out] */ IEnumVARIANT __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB ISiteServerEventProperties__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISiteServerEventProperties_INTERFACE_DEFINED__ */



#ifndef __SSEVENTLib_LIBRARY_DEFINED__
#define __SSEVENTLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: SSEVENTLib
 * at Thu Apr 23 13:09:34 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_SSEVENTLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_SiteServerEvent;

class DECLSPEC_UUID("F03E09B4-CCAC-11D0-BAF6-00C04FD7082F")
SiteServerEvent;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_SiteServerEnumEventProperties;

class DECLSPEC_UUID("548D442C-101E-11D1-8C85-00C04FC2D3B9")
SiteServerEnumEventProperties;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_SiteServerEventProperties;

class DECLSPEC_UUID("73CF5FFF-129B-11D1-8C86-00C04FC2D3B9")
SiteServerEventProperties;
#endif
#endif /* __SSEVENTLib_LIBRARY_DEFINED__ */

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
