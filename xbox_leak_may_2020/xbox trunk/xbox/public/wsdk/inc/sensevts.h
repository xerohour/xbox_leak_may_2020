
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Sep 24 19:28:15 1999
 */
/* Compiler settings for sensevts.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
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

#ifndef __sensevts_h__
#define __sensevts_h__

/* Forward Declarations */ 

#ifndef __ISensNetwork_FWD_DEFINED__
#define __ISensNetwork_FWD_DEFINED__
typedef interface ISensNetwork ISensNetwork;
#endif 	/* __ISensNetwork_FWD_DEFINED__ */


#ifndef __ISensOnNow_FWD_DEFINED__
#define __ISensOnNow_FWD_DEFINED__
typedef interface ISensOnNow ISensOnNow;
#endif 	/* __ISensOnNow_FWD_DEFINED__ */


#ifndef __ISensLogon_FWD_DEFINED__
#define __ISensLogon_FWD_DEFINED__
typedef interface ISensLogon ISensLogon;
#endif 	/* __ISensLogon_FWD_DEFINED__ */


#ifndef __SENS_FWD_DEFINED__
#define __SENS_FWD_DEFINED__

#ifdef __cplusplus
typedef class SENS SENS;
#else
typedef struct SENS SENS;
#endif /* __cplusplus */

#endif 	/* __SENS_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __SensEvents_LIBRARY_DEFINED__
#define __SensEvents_LIBRARY_DEFINED__

/* library SensEvents */
/* [helpstring][version][uuid] */ 

typedef /* [uuid] */  DECLSPEC_UUID("d597fad1-5b9f-11d1-8dd2-00aa004abd5e") struct SENS_QOCINFO
    {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwOutSpeed;
    DWORD dwInSpeed;
    }	SENS_QOCINFO;

typedef SENS_QOCINFO __RPC_FAR *LPSENS_QOCINFO;


EXTERN_C const IID LIBID_SensEvents;

#ifndef __ISensNetwork_INTERFACE_DEFINED__
#define __ISensNetwork_INTERFACE_DEFINED__

/* interface ISensNetwork */
/* [dual][helpstring][version][uuid][object] */ 


EXTERN_C const IID IID_ISensNetwork;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d597bab1-5b9f-11d1-8dd2-00aa004abd5e")
    ISensNetwork : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConnectionMade( 
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType,
            /* [in] */ LPSENS_QOCINFO lpQOCInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConnectionMadeNoQOCInfo( 
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConnectionLost( 
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DestinationReachable( 
            /* [in] */ BSTR bstrDestination,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType,
            /* [in] */ LPSENS_QOCINFO lpQOCInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DestinationReachableNoQOCInfo( 
            /* [in] */ BSTR bstrDestination,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISensNetworkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISensNetwork __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISensNetwork __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISensNetwork __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectionMade )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType,
            /* [in] */ LPSENS_QOCINFO lpQOCInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectionMadeNoQOCInfo )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ConnectionLost )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestinationReachable )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ BSTR bstrDestination,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType,
            /* [in] */ LPSENS_QOCINFO lpQOCInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DestinationReachableNoQOCInfo )( 
            ISensNetwork __RPC_FAR * This,
            /* [in] */ BSTR bstrDestination,
            /* [in] */ BSTR bstrConnection,
            /* [in] */ ULONG ulType);
        
        END_INTERFACE
    } ISensNetworkVtbl;

    interface ISensNetwork
    {
        CONST_VTBL struct ISensNetworkVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISensNetwork_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISensNetwork_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISensNetwork_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISensNetwork_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISensNetwork_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISensNetwork_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISensNetwork_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISensNetwork_ConnectionMade(This,bstrConnection,ulType,lpQOCInfo)	\
    (This)->lpVtbl -> ConnectionMade(This,bstrConnection,ulType,lpQOCInfo)

#define ISensNetwork_ConnectionMadeNoQOCInfo(This,bstrConnection,ulType)	\
    (This)->lpVtbl -> ConnectionMadeNoQOCInfo(This,bstrConnection,ulType)

#define ISensNetwork_ConnectionLost(This,bstrConnection,ulType)	\
    (This)->lpVtbl -> ConnectionLost(This,bstrConnection,ulType)

#define ISensNetwork_DestinationReachable(This,bstrDestination,bstrConnection,ulType,lpQOCInfo)	\
    (This)->lpVtbl -> DestinationReachable(This,bstrDestination,bstrConnection,ulType,lpQOCInfo)

#define ISensNetwork_DestinationReachableNoQOCInfo(This,bstrDestination,bstrConnection,ulType)	\
    (This)->lpVtbl -> DestinationReachableNoQOCInfo(This,bstrDestination,bstrConnection,ulType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ISensNetwork_ConnectionMade_Proxy( 
    ISensNetwork __RPC_FAR * This,
    /* [in] */ BSTR bstrConnection,
    /* [in] */ ULONG ulType,
    /* [in] */ LPSENS_QOCINFO lpQOCInfo);


void __RPC_STUB ISensNetwork_ConnectionMade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensNetwork_ConnectionMadeNoQOCInfo_Proxy( 
    ISensNetwork __RPC_FAR * This,
    /* [in] */ BSTR bstrConnection,
    /* [in] */ ULONG ulType);


void __RPC_STUB ISensNetwork_ConnectionMadeNoQOCInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensNetwork_ConnectionLost_Proxy( 
    ISensNetwork __RPC_FAR * This,
    /* [in] */ BSTR bstrConnection,
    /* [in] */ ULONG ulType);


void __RPC_STUB ISensNetwork_ConnectionLost_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensNetwork_DestinationReachable_Proxy( 
    ISensNetwork __RPC_FAR * This,
    /* [in] */ BSTR bstrDestination,
    /* [in] */ BSTR bstrConnection,
    /* [in] */ ULONG ulType,
    /* [in] */ LPSENS_QOCINFO lpQOCInfo);


void __RPC_STUB ISensNetwork_DestinationReachable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensNetwork_DestinationReachableNoQOCInfo_Proxy( 
    ISensNetwork __RPC_FAR * This,
    /* [in] */ BSTR bstrDestination,
    /* [in] */ BSTR bstrConnection,
    /* [in] */ ULONG ulType);


void __RPC_STUB ISensNetwork_DestinationReachableNoQOCInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISensNetwork_INTERFACE_DEFINED__ */


#ifndef __ISensOnNow_INTERFACE_DEFINED__
#define __ISensOnNow_INTERFACE_DEFINED__

/* interface ISensOnNow */
/* [dual][helpstring][version][uuid][object] */ 


EXTERN_C const IID IID_ISensOnNow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d597bab2-5b9f-11d1-8dd2-00aa004abd5e")
    ISensOnNow : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnACPower( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnBatteryPower( 
            /* [in] */ DWORD dwBatteryLifePercent) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE BatteryLow( 
            /* [in] */ DWORD dwBatteryLifePercent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISensOnNowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISensOnNow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISensOnNow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISensOnNow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISensOnNow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISensOnNow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISensOnNow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISensOnNow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnACPower )( 
            ISensOnNow __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnBatteryPower )( 
            ISensOnNow __RPC_FAR * This,
            /* [in] */ DWORD dwBatteryLifePercent);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BatteryLow )( 
            ISensOnNow __RPC_FAR * This,
            /* [in] */ DWORD dwBatteryLifePercent);
        
        END_INTERFACE
    } ISensOnNowVtbl;

    interface ISensOnNow
    {
        CONST_VTBL struct ISensOnNowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISensOnNow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISensOnNow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISensOnNow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISensOnNow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISensOnNow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISensOnNow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISensOnNow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISensOnNow_OnACPower(This)	\
    (This)->lpVtbl -> OnACPower(This)

#define ISensOnNow_OnBatteryPower(This,dwBatteryLifePercent)	\
    (This)->lpVtbl -> OnBatteryPower(This,dwBatteryLifePercent)

#define ISensOnNow_BatteryLow(This,dwBatteryLifePercent)	\
    (This)->lpVtbl -> BatteryLow(This,dwBatteryLifePercent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ISensOnNow_OnACPower_Proxy( 
    ISensOnNow __RPC_FAR * This);


void __RPC_STUB ISensOnNow_OnACPower_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensOnNow_OnBatteryPower_Proxy( 
    ISensOnNow __RPC_FAR * This,
    /* [in] */ DWORD dwBatteryLifePercent);


void __RPC_STUB ISensOnNow_OnBatteryPower_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensOnNow_BatteryLow_Proxy( 
    ISensOnNow __RPC_FAR * This,
    /* [in] */ DWORD dwBatteryLifePercent);


void __RPC_STUB ISensOnNow_BatteryLow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISensOnNow_INTERFACE_DEFINED__ */


#ifndef __ISensLogon_INTERFACE_DEFINED__
#define __ISensLogon_INTERFACE_DEFINED__

/* interface ISensLogon */
/* [dual][helpstring][version][uuid][object] */ 


EXTERN_C const IID IID_ISensLogon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d597bab3-5b9f-11d1-8dd2-00aa004abd5e")
    ISensLogon : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Logon( 
            /* [in] */ BSTR bstrUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Logoff( 
            /* [in] */ BSTR bstrUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE StartShell( 
            /* [in] */ BSTR bstrUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DisplayLock( 
            /* [in] */ BSTR bstrUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DisplayUnlock( 
            /* [in] */ BSTR bstrUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE StartScreenSaver( 
            /* [in] */ BSTR bstrUserName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE StopScreenSaver( 
            /* [in] */ BSTR bstrUserName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISensLogonVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISensLogon __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISensLogon __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISensLogon __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Logon )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Logoff )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartShell )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DisplayLock )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DisplayUnlock )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartScreenSaver )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StopScreenSaver )( 
            ISensLogon __RPC_FAR * This,
            /* [in] */ BSTR bstrUserName);
        
        END_INTERFACE
    } ISensLogonVtbl;

    interface ISensLogon
    {
        CONST_VTBL struct ISensLogonVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISensLogon_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISensLogon_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISensLogon_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISensLogon_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISensLogon_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISensLogon_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISensLogon_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISensLogon_Logon(This,bstrUserName)	\
    (This)->lpVtbl -> Logon(This,bstrUserName)

#define ISensLogon_Logoff(This,bstrUserName)	\
    (This)->lpVtbl -> Logoff(This,bstrUserName)

#define ISensLogon_StartShell(This,bstrUserName)	\
    (This)->lpVtbl -> StartShell(This,bstrUserName)

#define ISensLogon_DisplayLock(This,bstrUserName)	\
    (This)->lpVtbl -> DisplayLock(This,bstrUserName)

#define ISensLogon_DisplayUnlock(This,bstrUserName)	\
    (This)->lpVtbl -> DisplayUnlock(This,bstrUserName)

#define ISensLogon_StartScreenSaver(This,bstrUserName)	\
    (This)->lpVtbl -> StartScreenSaver(This,bstrUserName)

#define ISensLogon_StopScreenSaver(This,bstrUserName)	\
    (This)->lpVtbl -> StopScreenSaver(This,bstrUserName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_Logon_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_Logon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_Logoff_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_Logoff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_StartShell_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_StartShell_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_DisplayLock_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_DisplayLock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_DisplayUnlock_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_DisplayUnlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_StartScreenSaver_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_StartScreenSaver_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISensLogon_StopScreenSaver_Proxy( 
    ISensLogon __RPC_FAR * This,
    /* [in] */ BSTR bstrUserName);


void __RPC_STUB ISensLogon_StopScreenSaver_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISensLogon_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SENS;

#ifdef __cplusplus

class DECLSPEC_UUID("d597cafe-5b9f-11d1-8dd2-00aa004abd5e")
SENS;
#endif
#endif /* __SensEvents_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


