
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Wed Sep 15 03:28:31 1999
 */
/* Compiler settings for eventsys.idl:
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

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __eventsys_h__
#define __eventsys_h__

/* Forward Declarations */ 

#ifndef __IEventSystem_FWD_DEFINED__
#define __IEventSystem_FWD_DEFINED__
typedef interface IEventSystem IEventSystem;
#endif 	/* __IEventSystem_FWD_DEFINED__ */


#ifndef __IEventPublisher_FWD_DEFINED__
#define __IEventPublisher_FWD_DEFINED__
typedef interface IEventPublisher IEventPublisher;
#endif 	/* __IEventPublisher_FWD_DEFINED__ */


#ifndef __IEventClass_FWD_DEFINED__
#define __IEventClass_FWD_DEFINED__
typedef interface IEventClass IEventClass;
#endif 	/* __IEventClass_FWD_DEFINED__ */


#ifndef __IEventSubscription_FWD_DEFINED__
#define __IEventSubscription_FWD_DEFINED__
typedef interface IEventSubscription IEventSubscription;
#endif 	/* __IEventSubscription_FWD_DEFINED__ */


#ifndef __IFiringControl_FWD_DEFINED__
#define __IFiringControl_FWD_DEFINED__
typedef interface IFiringControl IFiringControl;
#endif 	/* __IFiringControl_FWD_DEFINED__ */


#ifndef __IPublisherFilter_FWD_DEFINED__
#define __IPublisherFilter_FWD_DEFINED__
typedef interface IPublisherFilter IPublisherFilter;
#endif 	/* __IPublisherFilter_FWD_DEFINED__ */


#ifndef __IMultiInterfacePublisherFilter_FWD_DEFINED__
#define __IMultiInterfacePublisherFilter_FWD_DEFINED__
typedef interface IMultiInterfacePublisherFilter IMultiInterfacePublisherFilter;
#endif 	/* __IMultiInterfacePublisherFilter_FWD_DEFINED__ */


#ifndef __IEventObjectChange_FWD_DEFINED__
#define __IEventObjectChange_FWD_DEFINED__
typedef interface IEventObjectChange IEventObjectChange;
#endif 	/* __IEventObjectChange_FWD_DEFINED__ */


#ifndef __IEnumEventObject_FWD_DEFINED__
#define __IEnumEventObject_FWD_DEFINED__
typedef interface IEnumEventObject IEnumEventObject;
#endif 	/* __IEnumEventObject_FWD_DEFINED__ */


#ifndef __IEventObjectCollection_FWD_DEFINED__
#define __IEventObjectCollection_FWD_DEFINED__
typedef interface IEventObjectCollection IEventObjectCollection;
#endif 	/* __IEventObjectCollection_FWD_DEFINED__ */


#ifndef __IEventProperty_FWD_DEFINED__
#define __IEventProperty_FWD_DEFINED__
typedef interface IEventProperty IEventProperty;
#endif 	/* __IEventProperty_FWD_DEFINED__ */


#ifndef __IEventControl_FWD_DEFINED__
#define __IEventControl_FWD_DEFINED__
typedef interface IEventControl IEventControl;
#endif 	/* __IEventControl_FWD_DEFINED__ */


#ifndef __IMultiInterfaceEventControl_FWD_DEFINED__
#define __IMultiInterfaceEventControl_FWD_DEFINED__
typedef interface IMultiInterfaceEventControl IMultiInterfaceEventControl;
#endif 	/* __IMultiInterfaceEventControl_FWD_DEFINED__ */


#ifndef __CEventSystem_FWD_DEFINED__
#define __CEventSystem_FWD_DEFINED__

#ifdef __cplusplus
typedef class CEventSystem CEventSystem;
#else
typedef struct CEventSystem CEventSystem;
#endif /* __cplusplus */

#endif 	/* __CEventSystem_FWD_DEFINED__ */


#ifndef __CEventPublisher_FWD_DEFINED__
#define __CEventPublisher_FWD_DEFINED__

#ifdef __cplusplus
typedef class CEventPublisher CEventPublisher;
#else
typedef struct CEventPublisher CEventPublisher;
#endif /* __cplusplus */

#endif 	/* __CEventPublisher_FWD_DEFINED__ */


#ifndef __CEventClass_FWD_DEFINED__
#define __CEventClass_FWD_DEFINED__

#ifdef __cplusplus
typedef class CEventClass CEventClass;
#else
typedef struct CEventClass CEventClass;
#endif /* __cplusplus */

#endif 	/* __CEventClass_FWD_DEFINED__ */


#ifndef __CEventSubscription_FWD_DEFINED__
#define __CEventSubscription_FWD_DEFINED__

#ifdef __cplusplus
typedef class CEventSubscription CEventSubscription;
#else
typedef struct CEventSubscription CEventSubscription;
#endif /* __cplusplus */

#endif 	/* __CEventSubscription_FWD_DEFINED__ */


#ifndef __EventObjectChange_FWD_DEFINED__
#define __EventObjectChange_FWD_DEFINED__

#ifdef __cplusplus
typedef class EventObjectChange EventObjectChange;
#else
typedef struct EventObjectChange EventObjectChange;
#endif /* __cplusplus */

#endif 	/* __EventObjectChange_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_eventsys_0000 */
/* [local] */ 

#define PROGID_EventSystem OLESTR("EventSystem.EventSystem")
#define PROGID_EventPublisher OLESTR("EventSystem.EventPublisher")
#define PROGID_EventClass OLESTR("EventSystem.EventClass")
#define PROGID_EventSubscription OLESTR("EventSystem.EventSubscription")
#define PROGID_EventPublisherCollection OLESTR("EventSystem.EventPublisherCollection")
#define PROGID_EventClassCollection OLESTR("EventSystem.EventClassCollection")
#define PROGID_EventSubscriptionCollection OLESTR("EventSystem.EventSubscriptionCollection")
#define PROGID_EventSubsystem OLESTR("EventSystem.EventSubsystem")
#define EVENTSYSTEM_PUBLISHER_ID OLESTR("{d0564c30-9df4-11d1-a281-00c04fca0aa7}")
#define EVENTSYSTEM_SUBSYSTEM_CLSID OLESTR("{503c1fd8-b605-11d2-a92d-006008c60e24}")






extern RPC_IF_HANDLE __MIDL_itf_eventsys_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_eventsys_0000_v0_0_s_ifspec;

#ifndef __IEventSystem_INTERFACE_DEFINED__
#define __IEventSystem_INTERFACE_DEFINED__

/* interface IEventSystem */
/* [unique][helpstring][dual][uuid][object] */ 

// *****************************************************************
// This is a Deprecated interface - Use COMAdmin interfaces instead.
// *****************************************************************

EXTERN_C const IID IID_IEventSystem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4E14FB9F-2E22-11D1-9964-00C04FBBB345")
    IEventSystem : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Query( 
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria,
            /* [out] */ int __RPC_FAR *errorIndex,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Store( 
            /* [in] */ BSTR ProgID,
            /* [in] */ IUnknown __RPC_FAR *pInterface) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria,
            /* [out] */ int __RPC_FAR *errorIndex) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_EventObjectChangeEventClassID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryS( 
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveS( 
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventSystemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventSystem __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventSystem __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventSystem __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Query )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria,
            /* [out] */ int __RPC_FAR *errorIndex,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Store )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ BSTR ProgID,
            /* [in] */ IUnknown __RPC_FAR *pInterface);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria,
            /* [out] */ int __RPC_FAR *errorIndex);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventObjectChangeEventClassID )( 
            IEventSystem __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryS )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveS )( 
            IEventSystem __RPC_FAR * This,
            /* [in] */ BSTR progID,
            /* [in] */ BSTR queryCriteria);
        
        END_INTERFACE
    } IEventSystemVtbl;

    interface IEventSystem
    {
        CONST_VTBL struct IEventSystemVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventSystem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventSystem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventSystem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventSystem_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventSystem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventSystem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventSystem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventSystem_Query(This,progID,queryCriteria,errorIndex,ppInterface)	\
    (This)->lpVtbl -> Query(This,progID,queryCriteria,errorIndex,ppInterface)

#define IEventSystem_Store(This,ProgID,pInterface)	\
    (This)->lpVtbl -> Store(This,ProgID,pInterface)

#define IEventSystem_Remove(This,progID,queryCriteria,errorIndex)	\
    (This)->lpVtbl -> Remove(This,progID,queryCriteria,errorIndex)

#define IEventSystem_get_EventObjectChangeEventClassID(This,pbstrEventClassID)	\
    (This)->lpVtbl -> get_EventObjectChangeEventClassID(This,pbstrEventClassID)

#define IEventSystem_QueryS(This,progID,queryCriteria,ppInterface)	\
    (This)->lpVtbl -> QueryS(This,progID,queryCriteria,ppInterface)

#define IEventSystem_RemoveS(This,progID,queryCriteria)	\
    (This)->lpVtbl -> RemoveS(This,progID,queryCriteria)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSystem_Query_Proxy( 
    IEventSystem __RPC_FAR * This,
    /* [in] */ BSTR progID,
    /* [in] */ BSTR queryCriteria,
    /* [out] */ int __RPC_FAR *errorIndex,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface);


void __RPC_STUB IEventSystem_Query_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSystem_Store_Proxy( 
    IEventSystem __RPC_FAR * This,
    /* [in] */ BSTR ProgID,
    /* [in] */ IUnknown __RPC_FAR *pInterface);


void __RPC_STUB IEventSystem_Store_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSystem_Remove_Proxy( 
    IEventSystem __RPC_FAR * This,
    /* [in] */ BSTR progID,
    /* [in] */ BSTR queryCriteria,
    /* [out] */ int __RPC_FAR *errorIndex);


void __RPC_STUB IEventSystem_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSystem_get_EventObjectChangeEventClassID_Proxy( 
    IEventSystem __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID);


void __RPC_STUB IEventSystem_get_EventObjectChangeEventClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSystem_QueryS_Proxy( 
    IEventSystem __RPC_FAR * This,
    /* [in] */ BSTR progID,
    /* [in] */ BSTR queryCriteria,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface);


void __RPC_STUB IEventSystem_QueryS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSystem_RemoveS_Proxy( 
    IEventSystem __RPC_FAR * This,
    /* [in] */ BSTR progID,
    /* [in] */ BSTR queryCriteria);


void __RPC_STUB IEventSystem_RemoveS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventSystem_INTERFACE_DEFINED__ */


#ifndef __IEventPublisher_INTERFACE_DEFINED__
#define __IEventPublisher_INTERFACE_DEFINED__

/* interface IEventPublisher */
/* [unique][helpstring][dual][uuid][object] */ 

// ********************************************
// This is a Deprecated interface - Do Not Use.
// ********************************************

EXTERN_C const IID IID_IEventPublisher;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E341516B-2E32-11D1-9964-00C04FBBB345")
    IEventPublisher : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherID( 
            /* [in] */ BSTR bstrPublisherID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherName( 
            /* [in] */ BSTR bstrPublisherName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherType( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherType) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherType( 
            /* [in] */ BSTR bstrPublisherType) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OwnerSID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OwnerSID( 
            /* [in] */ BSTR bstrOwnerSID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR bstrDescription) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDefaultProperty( 
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutDefaultProperty( 
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveDefaultProperty( 
            /* [in] */ BSTR bstrPropertyName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDefaultPropertyCollection( 
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventPublisherVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventPublisher __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventPublisher __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventPublisher __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PublisherID )( 
            IEventPublisher __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PublisherID )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrPublisherID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PublisherName )( 
            IEventPublisher __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PublisherName )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrPublisherName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PublisherType )( 
            IEventPublisher __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherType);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PublisherType )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrPublisherType);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OwnerSID )( 
            IEventPublisher __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OwnerSID )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrOwnerSID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IEventPublisher __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrDescription);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDefaultProperty )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PutDefaultProperty )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveDefaultProperty )( 
            IEventPublisher __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetDefaultPropertyCollection )( 
            IEventPublisher __RPC_FAR * This,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection);
        
        END_INTERFACE
    } IEventPublisherVtbl;

    interface IEventPublisher
    {
        CONST_VTBL struct IEventPublisherVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventPublisher_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventPublisher_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventPublisher_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventPublisher_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventPublisher_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventPublisher_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventPublisher_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventPublisher_get_PublisherID(This,pbstrPublisherID)	\
    (This)->lpVtbl -> get_PublisherID(This,pbstrPublisherID)

#define IEventPublisher_put_PublisherID(This,bstrPublisherID)	\
    (This)->lpVtbl -> put_PublisherID(This,bstrPublisherID)

#define IEventPublisher_get_PublisherName(This,pbstrPublisherName)	\
    (This)->lpVtbl -> get_PublisherName(This,pbstrPublisherName)

#define IEventPublisher_put_PublisherName(This,bstrPublisherName)	\
    (This)->lpVtbl -> put_PublisherName(This,bstrPublisherName)

#define IEventPublisher_get_PublisherType(This,pbstrPublisherType)	\
    (This)->lpVtbl -> get_PublisherType(This,pbstrPublisherType)

#define IEventPublisher_put_PublisherType(This,bstrPublisherType)	\
    (This)->lpVtbl -> put_PublisherType(This,bstrPublisherType)

#define IEventPublisher_get_OwnerSID(This,pbstrOwnerSID)	\
    (This)->lpVtbl -> get_OwnerSID(This,pbstrOwnerSID)

#define IEventPublisher_put_OwnerSID(This,bstrOwnerSID)	\
    (This)->lpVtbl -> put_OwnerSID(This,bstrOwnerSID)

#define IEventPublisher_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IEventPublisher_put_Description(This,bstrDescription)	\
    (This)->lpVtbl -> put_Description(This,bstrDescription)

#define IEventPublisher_GetDefaultProperty(This,bstrPropertyName,propertyValue)	\
    (This)->lpVtbl -> GetDefaultProperty(This,bstrPropertyName,propertyValue)

#define IEventPublisher_PutDefaultProperty(This,bstrPropertyName,propertyValue)	\
    (This)->lpVtbl -> PutDefaultProperty(This,bstrPropertyName,propertyValue)

#define IEventPublisher_RemoveDefaultProperty(This,bstrPropertyName)	\
    (This)->lpVtbl -> RemoveDefaultProperty(This,bstrPropertyName)

#define IEventPublisher_GetDefaultPropertyCollection(This,collection)	\
    (This)->lpVtbl -> GetDefaultPropertyCollection(This,collection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventPublisher_get_PublisherID_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherID);


void __RPC_STUB IEventPublisher_get_PublisherID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventPublisher_put_PublisherID_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrPublisherID);


void __RPC_STUB IEventPublisher_put_PublisherID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventPublisher_get_PublisherName_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherName);


void __RPC_STUB IEventPublisher_get_PublisherName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventPublisher_put_PublisherName_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrPublisherName);


void __RPC_STUB IEventPublisher_put_PublisherName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventPublisher_get_PublisherType_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherType);


void __RPC_STUB IEventPublisher_get_PublisherType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventPublisher_put_PublisherType_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrPublisherType);


void __RPC_STUB IEventPublisher_put_PublisherType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventPublisher_get_OwnerSID_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID);


void __RPC_STUB IEventPublisher_get_OwnerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventPublisher_put_OwnerSID_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrOwnerSID);


void __RPC_STUB IEventPublisher_put_OwnerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventPublisher_get_Description_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);


void __RPC_STUB IEventPublisher_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventPublisher_put_Description_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrDescription);


void __RPC_STUB IEventPublisher_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventPublisher_GetDefaultProperty_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventPublisher_GetDefaultProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventPublisher_PutDefaultProperty_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [in] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventPublisher_PutDefaultProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventPublisher_RemoveDefaultProperty_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName);


void __RPC_STUB IEventPublisher_RemoveDefaultProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventPublisher_GetDefaultPropertyCollection_Proxy( 
    IEventPublisher __RPC_FAR * This,
    /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection);


void __RPC_STUB IEventPublisher_GetDefaultPropertyCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventPublisher_INTERFACE_DEFINED__ */


#ifndef __IEventClass_INTERFACE_DEFINED__
#define __IEventClass_INTERFACE_DEFINED__

/* interface IEventClass */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventClass;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fb2b72a0-7a68-11d1-88f9-0080c7d771bf")
    IEventClass : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventClassID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventClassID( 
            /* [in] */ BSTR bstrEventClassID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventClassName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventClassName( 
            /* [in] */ BSTR bstrEventClassName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OwnerSID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OwnerSID( 
            /* [in] */ BSTR bstrOwnerSID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FiringInterfaceID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFiringInterfaceID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FiringInterfaceID( 
            /* [in] */ BSTR bstrFiringInterfaceID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR bstrDescription) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CustomConfigCLSID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCustomConfigCLSID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CustomConfigCLSID( 
            /* [in] */ BSTR bstrCustomConfigCLSID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TypeLib( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTypeLib) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TypeLib( 
            /* [in] */ BSTR bstrTypeLib) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventClassVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventClass __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventClass __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventClass __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventClassID )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventClassID )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrEventClassID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventClassName )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventClassName )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrEventClassName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OwnerSID )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OwnerSID )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrOwnerSID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FiringInterfaceID )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrFiringInterfaceID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FiringInterfaceID )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrFiringInterfaceID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrDescription);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CustomConfigCLSID )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCustomConfigCLSID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CustomConfigCLSID )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrCustomConfigCLSID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TypeLib )( 
            IEventClass __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrTypeLib);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TypeLib )( 
            IEventClass __RPC_FAR * This,
            /* [in] */ BSTR bstrTypeLib);
        
        END_INTERFACE
    } IEventClassVtbl;

    interface IEventClass
    {
        CONST_VTBL struct IEventClassVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventClass_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventClass_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventClass_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventClass_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventClass_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventClass_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventClass_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventClass_get_EventClassID(This,pbstrEventClassID)	\
    (This)->lpVtbl -> get_EventClassID(This,pbstrEventClassID)

#define IEventClass_put_EventClassID(This,bstrEventClassID)	\
    (This)->lpVtbl -> put_EventClassID(This,bstrEventClassID)

#define IEventClass_get_EventClassName(This,pbstrEventClassName)	\
    (This)->lpVtbl -> get_EventClassName(This,pbstrEventClassName)

#define IEventClass_put_EventClassName(This,bstrEventClassName)	\
    (This)->lpVtbl -> put_EventClassName(This,bstrEventClassName)

#define IEventClass_get_OwnerSID(This,pbstrOwnerSID)	\
    (This)->lpVtbl -> get_OwnerSID(This,pbstrOwnerSID)

#define IEventClass_put_OwnerSID(This,bstrOwnerSID)	\
    (This)->lpVtbl -> put_OwnerSID(This,bstrOwnerSID)

#define IEventClass_get_FiringInterfaceID(This,pbstrFiringInterfaceID)	\
    (This)->lpVtbl -> get_FiringInterfaceID(This,pbstrFiringInterfaceID)

#define IEventClass_put_FiringInterfaceID(This,bstrFiringInterfaceID)	\
    (This)->lpVtbl -> put_FiringInterfaceID(This,bstrFiringInterfaceID)

#define IEventClass_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IEventClass_put_Description(This,bstrDescription)	\
    (This)->lpVtbl -> put_Description(This,bstrDescription)

#define IEventClass_get_CustomConfigCLSID(This,pbstrCustomConfigCLSID)	\
    (This)->lpVtbl -> get_CustomConfigCLSID(This,pbstrCustomConfigCLSID)

#define IEventClass_put_CustomConfigCLSID(This,bstrCustomConfigCLSID)	\
    (This)->lpVtbl -> put_CustomConfigCLSID(This,bstrCustomConfigCLSID)

#define IEventClass_get_TypeLib(This,pbstrTypeLib)	\
    (This)->lpVtbl -> get_TypeLib(This,pbstrTypeLib)

#define IEventClass_put_TypeLib(This,bstrTypeLib)	\
    (This)->lpVtbl -> put_TypeLib(This,bstrTypeLib)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_EventClassID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID);


void __RPC_STUB IEventClass_get_EventClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_EventClassID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrEventClassID);


void __RPC_STUB IEventClass_put_EventClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_EventClassName_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassName);


void __RPC_STUB IEventClass_get_EventClassName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_EventClassName_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrEventClassName);


void __RPC_STUB IEventClass_put_EventClassName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_OwnerSID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID);


void __RPC_STUB IEventClass_get_OwnerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_OwnerSID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrOwnerSID);


void __RPC_STUB IEventClass_put_OwnerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_FiringInterfaceID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrFiringInterfaceID);


void __RPC_STUB IEventClass_get_FiringInterfaceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_FiringInterfaceID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrFiringInterfaceID);


void __RPC_STUB IEventClass_put_FiringInterfaceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_Description_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);


void __RPC_STUB IEventClass_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_Description_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrDescription);


void __RPC_STUB IEventClass_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_CustomConfigCLSID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCustomConfigCLSID);


void __RPC_STUB IEventClass_get_CustomConfigCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_CustomConfigCLSID_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrCustomConfigCLSID);


void __RPC_STUB IEventClass_put_CustomConfigCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventClass_get_TypeLib_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrTypeLib);


void __RPC_STUB IEventClass_get_TypeLib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventClass_put_TypeLib_Proxy( 
    IEventClass __RPC_FAR * This,
    /* [in] */ BSTR bstrTypeLib);


void __RPC_STUB IEventClass_put_TypeLib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventClass_INTERFACE_DEFINED__ */


#ifndef __IEventSubscription_INTERFACE_DEFINED__
#define __IEventSubscription_INTERFACE_DEFINED__

/* interface IEventSubscription */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventSubscription;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4A6B0E15-2E38-11D1-9965-00C04FBBB345")
    IEventSubscription : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriptionID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriptionID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SubscriptionID( 
            /* [in] */ BSTR bstrSubscriptionID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriptionName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriptionName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SubscriptionName( 
            /* [in] */ BSTR bstrSubscriptionName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PublisherID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PublisherID( 
            /* [in] */ BSTR bstrPublisherID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EventClassID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EventClassID( 
            /* [in] */ BSTR bstrEventClassID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MethodName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMethodName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MethodName( 
            /* [in] */ BSTR bstrMethodName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriberCLSID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriberCLSID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SubscriberCLSID( 
            /* [in] */ BSTR bstrSubscriberCLSID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriberInterface( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSubscriberInterface) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SubscriberInterface( 
            /* [in] */ IUnknown __RPC_FAR *pSubscriberInterface) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PerUser( 
            /* [retval][out] */ BOOL __RPC_FAR *pfPerUser) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PerUser( 
            /* [in] */ BOOL fPerUser) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OwnerSID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OwnerSID( 
            /* [in] */ BSTR bstrOwnerSID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Enabled( 
            /* [retval][out] */ BOOL __RPC_FAR *pfEnabled) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Enabled( 
            /* [in] */ BOOL fEnabled) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR bstrDescription) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MachineName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MachineName( 
            /* [in] */ BSTR bstrMachineName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPublisherProperty( 
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutPublisherProperty( 
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemovePublisherProperty( 
            /* [in] */ BSTR bstrPropertyName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPublisherPropertyCollection( 
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubscriberProperty( 
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PutSubscriberProperty( 
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveSubscriberProperty( 
            /* [in] */ BSTR bstrPropertyName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubscriberPropertyCollection( 
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_InterfaceID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrInterfaceID) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_InterfaceID( 
            /* [in] */ BSTR bstrInterfaceID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventSubscriptionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventSubscription __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventSubscription __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventSubscription __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubscriptionID )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriptionID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SubscriptionID )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrSubscriptionID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubscriptionName )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriptionName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SubscriptionName )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrSubscriptionName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PublisherID )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PublisherID )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPublisherID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventClassID )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventClassID )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrEventClassID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MethodName )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMethodName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MethodName )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrMethodName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubscriberCLSID )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriberCLSID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SubscriberCLSID )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrSubscriberCLSID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubscriberInterface )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSubscriberInterface);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SubscriberInterface )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pSubscriberInterface);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PerUser )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfPerUser);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PerUser )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BOOL fPerUser);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OwnerSID )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OwnerSID )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrOwnerSID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Enabled )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfEnabled);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Enabled )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BOOL fEnabled);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrDescription);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MachineName )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MachineName )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrMachineName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPublisherProperty )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PutPublisherProperty )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemovePublisherProperty )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPublisherPropertyCollection )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubscriberProperty )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PutSubscriberProperty )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveSubscriberProperty )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrPropertyName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubscriberPropertyCollection )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InterfaceID )( 
            IEventSubscription __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrInterfaceID);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InterfaceID )( 
            IEventSubscription __RPC_FAR * This,
            /* [in] */ BSTR bstrInterfaceID);
        
        END_INTERFACE
    } IEventSubscriptionVtbl;

    interface IEventSubscription
    {
        CONST_VTBL struct IEventSubscriptionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventSubscription_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventSubscription_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventSubscription_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventSubscription_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventSubscription_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventSubscription_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventSubscription_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventSubscription_get_SubscriptionID(This,pbstrSubscriptionID)	\
    (This)->lpVtbl -> get_SubscriptionID(This,pbstrSubscriptionID)

#define IEventSubscription_put_SubscriptionID(This,bstrSubscriptionID)	\
    (This)->lpVtbl -> put_SubscriptionID(This,bstrSubscriptionID)

#define IEventSubscription_get_SubscriptionName(This,pbstrSubscriptionName)	\
    (This)->lpVtbl -> get_SubscriptionName(This,pbstrSubscriptionName)

#define IEventSubscription_put_SubscriptionName(This,bstrSubscriptionName)	\
    (This)->lpVtbl -> put_SubscriptionName(This,bstrSubscriptionName)

#define IEventSubscription_get_PublisherID(This,pbstrPublisherID)	\
    (This)->lpVtbl -> get_PublisherID(This,pbstrPublisherID)

#define IEventSubscription_put_PublisherID(This,bstrPublisherID)	\
    (This)->lpVtbl -> put_PublisherID(This,bstrPublisherID)

#define IEventSubscription_get_EventClassID(This,pbstrEventClassID)	\
    (This)->lpVtbl -> get_EventClassID(This,pbstrEventClassID)

#define IEventSubscription_put_EventClassID(This,bstrEventClassID)	\
    (This)->lpVtbl -> put_EventClassID(This,bstrEventClassID)

#define IEventSubscription_get_MethodName(This,pbstrMethodName)	\
    (This)->lpVtbl -> get_MethodName(This,pbstrMethodName)

#define IEventSubscription_put_MethodName(This,bstrMethodName)	\
    (This)->lpVtbl -> put_MethodName(This,bstrMethodName)

#define IEventSubscription_get_SubscriberCLSID(This,pbstrSubscriberCLSID)	\
    (This)->lpVtbl -> get_SubscriberCLSID(This,pbstrSubscriberCLSID)

#define IEventSubscription_put_SubscriberCLSID(This,bstrSubscriberCLSID)	\
    (This)->lpVtbl -> put_SubscriberCLSID(This,bstrSubscriberCLSID)

#define IEventSubscription_get_SubscriberInterface(This,ppSubscriberInterface)	\
    (This)->lpVtbl -> get_SubscriberInterface(This,ppSubscriberInterface)

#define IEventSubscription_put_SubscriberInterface(This,pSubscriberInterface)	\
    (This)->lpVtbl -> put_SubscriberInterface(This,pSubscriberInterface)

#define IEventSubscription_get_PerUser(This,pfPerUser)	\
    (This)->lpVtbl -> get_PerUser(This,pfPerUser)

#define IEventSubscription_put_PerUser(This,fPerUser)	\
    (This)->lpVtbl -> put_PerUser(This,fPerUser)

#define IEventSubscription_get_OwnerSID(This,pbstrOwnerSID)	\
    (This)->lpVtbl -> get_OwnerSID(This,pbstrOwnerSID)

#define IEventSubscription_put_OwnerSID(This,bstrOwnerSID)	\
    (This)->lpVtbl -> put_OwnerSID(This,bstrOwnerSID)

#define IEventSubscription_get_Enabled(This,pfEnabled)	\
    (This)->lpVtbl -> get_Enabled(This,pfEnabled)

#define IEventSubscription_put_Enabled(This,fEnabled)	\
    (This)->lpVtbl -> put_Enabled(This,fEnabled)

#define IEventSubscription_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IEventSubscription_put_Description(This,bstrDescription)	\
    (This)->lpVtbl -> put_Description(This,bstrDescription)

#define IEventSubscription_get_MachineName(This,pbstrMachineName)	\
    (This)->lpVtbl -> get_MachineName(This,pbstrMachineName)

#define IEventSubscription_put_MachineName(This,bstrMachineName)	\
    (This)->lpVtbl -> put_MachineName(This,bstrMachineName)

#define IEventSubscription_GetPublisherProperty(This,bstrPropertyName,propertyValue)	\
    (This)->lpVtbl -> GetPublisherProperty(This,bstrPropertyName,propertyValue)

#define IEventSubscription_PutPublisherProperty(This,bstrPropertyName,propertyValue)	\
    (This)->lpVtbl -> PutPublisherProperty(This,bstrPropertyName,propertyValue)

#define IEventSubscription_RemovePublisherProperty(This,bstrPropertyName)	\
    (This)->lpVtbl -> RemovePublisherProperty(This,bstrPropertyName)

#define IEventSubscription_GetPublisherPropertyCollection(This,collection)	\
    (This)->lpVtbl -> GetPublisherPropertyCollection(This,collection)

#define IEventSubscription_GetSubscriberProperty(This,bstrPropertyName,propertyValue)	\
    (This)->lpVtbl -> GetSubscriberProperty(This,bstrPropertyName,propertyValue)

#define IEventSubscription_PutSubscriberProperty(This,bstrPropertyName,propertyValue)	\
    (This)->lpVtbl -> PutSubscriberProperty(This,bstrPropertyName,propertyValue)

#define IEventSubscription_RemoveSubscriberProperty(This,bstrPropertyName)	\
    (This)->lpVtbl -> RemoveSubscriberProperty(This,bstrPropertyName)

#define IEventSubscription_GetSubscriberPropertyCollection(This,collection)	\
    (This)->lpVtbl -> GetSubscriberPropertyCollection(This,collection)

#define IEventSubscription_get_InterfaceID(This,pbstrInterfaceID)	\
    (This)->lpVtbl -> get_InterfaceID(This,pbstrInterfaceID)

#define IEventSubscription_put_InterfaceID(This,bstrInterfaceID)	\
    (This)->lpVtbl -> put_InterfaceID(This,bstrInterfaceID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_SubscriptionID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriptionID);


void __RPC_STUB IEventSubscription_get_SubscriptionID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_SubscriptionID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrSubscriptionID);


void __RPC_STUB IEventSubscription_put_SubscriptionID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_SubscriptionName_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriptionName);


void __RPC_STUB IEventSubscription_get_SubscriptionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_SubscriptionName_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrSubscriptionName);


void __RPC_STUB IEventSubscription_put_SubscriptionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_PublisherID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrPublisherID);


void __RPC_STUB IEventSubscription_get_PublisherID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_PublisherID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPublisherID);


void __RPC_STUB IEventSubscription_put_PublisherID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_EventClassID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrEventClassID);


void __RPC_STUB IEventSubscription_get_EventClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_EventClassID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrEventClassID);


void __RPC_STUB IEventSubscription_put_EventClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_MethodName_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMethodName);


void __RPC_STUB IEventSubscription_get_MethodName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_MethodName_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrMethodName);


void __RPC_STUB IEventSubscription_put_MethodName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_SubscriberCLSID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSubscriberCLSID);


void __RPC_STUB IEventSubscription_get_SubscriberCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_SubscriberCLSID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrSubscriberCLSID);


void __RPC_STUB IEventSubscription_put_SubscriberCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_SubscriberInterface_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppSubscriberInterface);


void __RPC_STUB IEventSubscription_get_SubscriberInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_SubscriberInterface_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pSubscriberInterface);


void __RPC_STUB IEventSubscription_put_SubscriberInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_PerUser_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfPerUser);


void __RPC_STUB IEventSubscription_get_PerUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_PerUser_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BOOL fPerUser);


void __RPC_STUB IEventSubscription_put_PerUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_OwnerSID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrOwnerSID);


void __RPC_STUB IEventSubscription_get_OwnerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_OwnerSID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrOwnerSID);


void __RPC_STUB IEventSubscription_put_OwnerSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_Enabled_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfEnabled);


void __RPC_STUB IEventSubscription_get_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_Enabled_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BOOL fEnabled);


void __RPC_STUB IEventSubscription_put_Enabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_Description_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrDescription);


void __RPC_STUB IEventSubscription_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_Description_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrDescription);


void __RPC_STUB IEventSubscription_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_MachineName_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrMachineName);


void __RPC_STUB IEventSubscription_get_MachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_MachineName_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrMachineName);


void __RPC_STUB IEventSubscription_put_MachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_GetPublisherProperty_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventSubscription_GetPublisherProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_PutPublisherProperty_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [in] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventSubscription_PutPublisherProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_RemovePublisherProperty_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName);


void __RPC_STUB IEventSubscription_RemovePublisherProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_GetPublisherPropertyCollection_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection);


void __RPC_STUB IEventSubscription_GetPublisherPropertyCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_GetSubscriberProperty_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventSubscription_GetSubscriberProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_PutSubscriberProperty_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [in] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventSubscription_PutSubscriberProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_RemoveSubscriberProperty_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrPropertyName);


void __RPC_STUB IEventSubscription_RemoveSubscriberProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_GetSubscriberPropertyCollection_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *collection);


void __RPC_STUB IEventSubscription_GetSubscriberPropertyCollection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_get_InterfaceID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrInterfaceID);


void __RPC_STUB IEventSubscription_get_InterfaceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSubscription_put_InterfaceID_Proxy( 
    IEventSubscription __RPC_FAR * This,
    /* [in] */ BSTR bstrInterfaceID);


void __RPC_STUB IEventSubscription_put_InterfaceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventSubscription_INTERFACE_DEFINED__ */


#ifndef __IFiringControl_INTERFACE_DEFINED__
#define __IFiringControl_INTERFACE_DEFINED__

/* interface IFiringControl */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IFiringControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e0498c93-4efe-11d1-9971-00c04fbbb345")
    IFiringControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FireSubscription( 
            /* [in] */ IEventSubscription __RPC_FAR *subscription) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFiringControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IFiringControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IFiringControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IFiringControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IFiringControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IFiringControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IFiringControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IFiringControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FireSubscription )( 
            IFiringControl __RPC_FAR * This,
            /* [in] */ IEventSubscription __RPC_FAR *subscription);
        
        END_INTERFACE
    } IFiringControlVtbl;

    interface IFiringControl
    {
        CONST_VTBL struct IFiringControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFiringControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFiringControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFiringControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFiringControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFiringControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFiringControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFiringControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFiringControl_FireSubscription(This,subscription)	\
    (This)->lpVtbl -> FireSubscription(This,subscription)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFiringControl_FireSubscription_Proxy( 
    IFiringControl __RPC_FAR * This,
    /* [in] */ IEventSubscription __RPC_FAR *subscription);


void __RPC_STUB IFiringControl_FireSubscription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFiringControl_INTERFACE_DEFINED__ */


#ifndef __IPublisherFilter_INTERFACE_DEFINED__
#define __IPublisherFilter_INTERFACE_DEFINED__

/* interface IPublisherFilter */
/* [unique][helpstring][uuid][object] */ 

// ****************************************************************************
// This is a Deprecated interface - Use IMultiInterfacePublisherFilter instead.
// ****************************************************************************

EXTERN_C const IID IID_IPublisherFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("465e5cc0-7b26-11d1-88fb-0080c7d771bf")
    IPublisherFilter : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ BSTR methodName,
            /* [unique][in] */ IDispatch __RPC_FAR *dispUserDefined) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PrepareToFire( 
            /* [in] */ BSTR methodName,
            /* [in] */ IFiringControl __RPC_FAR *firingControl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPublisherFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPublisherFilter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPublisherFilter __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPublisherFilter __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            IPublisherFilter __RPC_FAR * This,
            /* [in] */ BSTR methodName,
            /* [unique][in] */ IDispatch __RPC_FAR *dispUserDefined);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareToFire )( 
            IPublisherFilter __RPC_FAR * This,
            /* [in] */ BSTR methodName,
            /* [in] */ IFiringControl __RPC_FAR *firingControl);
        
        END_INTERFACE
    } IPublisherFilterVtbl;

    interface IPublisherFilter
    {
        CONST_VTBL struct IPublisherFilterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPublisherFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPublisherFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPublisherFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPublisherFilter_Initialize(This,methodName,dispUserDefined)	\
    (This)->lpVtbl -> Initialize(This,methodName,dispUserDefined)

#define IPublisherFilter_PrepareToFire(This,methodName,firingControl)	\
    (This)->lpVtbl -> PrepareToFire(This,methodName,firingControl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPublisherFilter_Initialize_Proxy( 
    IPublisherFilter __RPC_FAR * This,
    /* [in] */ BSTR methodName,
    /* [unique][in] */ IDispatch __RPC_FAR *dispUserDefined);


void __RPC_STUB IPublisherFilter_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IPublisherFilter_PrepareToFire_Proxy( 
    IPublisherFilter __RPC_FAR * This,
    /* [in] */ BSTR methodName,
    /* [in] */ IFiringControl __RPC_FAR *firingControl);


void __RPC_STUB IPublisherFilter_PrepareToFire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPublisherFilter_INTERFACE_DEFINED__ */


#ifndef __IMultiInterfacePublisherFilter_INTERFACE_DEFINED__
#define __IMultiInterfacePublisherFilter_INTERFACE_DEFINED__

/* interface IMultiInterfacePublisherFilter */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMultiInterfacePublisherFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("465e5cc1-7b26-11d1-88fb-0080c7d771bf")
    IMultiInterfacePublisherFilter : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IMultiInterfaceEventControl __RPC_FAR *pEIC) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PrepareToFire( 
            /* [in] */ REFIID iid,
            /* [in] */ BSTR methodName,
            /* [in] */ IFiringControl __RPC_FAR *firingControl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMultiInterfacePublisherFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMultiInterfacePublisherFilter __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMultiInterfacePublisherFilter __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMultiInterfacePublisherFilter __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            IMultiInterfacePublisherFilter __RPC_FAR * This,
            /* [in] */ IMultiInterfaceEventControl __RPC_FAR *pEIC);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareToFire )( 
            IMultiInterfacePublisherFilter __RPC_FAR * This,
            /* [in] */ REFIID iid,
            /* [in] */ BSTR methodName,
            /* [in] */ IFiringControl __RPC_FAR *firingControl);
        
        END_INTERFACE
    } IMultiInterfacePublisherFilterVtbl;

    interface IMultiInterfacePublisherFilter
    {
        CONST_VTBL struct IMultiInterfacePublisherFilterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMultiInterfacePublisherFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMultiInterfacePublisherFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMultiInterfacePublisherFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMultiInterfacePublisherFilter_Initialize(This,pEIC)	\
    (This)->lpVtbl -> Initialize(This,pEIC)

#define IMultiInterfacePublisherFilter_PrepareToFire(This,iid,methodName,firingControl)	\
    (This)->lpVtbl -> PrepareToFire(This,iid,methodName,firingControl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IMultiInterfacePublisherFilter_Initialize_Proxy( 
    IMultiInterfacePublisherFilter __RPC_FAR * This,
    /* [in] */ IMultiInterfaceEventControl __RPC_FAR *pEIC);


void __RPC_STUB IMultiInterfacePublisherFilter_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IMultiInterfacePublisherFilter_PrepareToFire_Proxy( 
    IMultiInterfacePublisherFilter __RPC_FAR * This,
    /* [in] */ REFIID iid,
    /* [in] */ BSTR methodName,
    /* [in] */ IFiringControl __RPC_FAR *firingControl);


void __RPC_STUB IMultiInterfacePublisherFilter_PrepareToFire_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMultiInterfacePublisherFilter_INTERFACE_DEFINED__ */


#ifndef __IEventObjectChange_INTERFACE_DEFINED__
#define __IEventObjectChange_INTERFACE_DEFINED__

/* interface IEventObjectChange */
/* [unique][helpstring][uuid][object] */ 

typedef /* [public][public][public][public] */ 
enum __MIDL_IEventObjectChange_0001
    {	EOC_NewObject	= 0,
	EOC_ModifiedObject	= EOC_NewObject + 1,
	EOC_DeletedObject	= EOC_ModifiedObject + 1
    }	EOC_ChangeType;


EXTERN_C const IID IID_IEventObjectChange;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F4A07D70-2E25-11D1-9964-00C04FBBB345")
    IEventObjectChange : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ChangedSubscription( 
            /* [in] */ EOC_ChangeType changeType,
            /* [in] */ BSTR bstrSubscriptionID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ChangedEventClass( 
            /* [in] */ EOC_ChangeType changeType,
            /* [in] */ BSTR bstrEventClassID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ChangedPublisher( 
            /* [in] */ EOC_ChangeType changeType,
            /* [in] */ BSTR bstrPublisherID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventObjectChangeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventObjectChange __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventObjectChange __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventObjectChange __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangedSubscription )( 
            IEventObjectChange __RPC_FAR * This,
            /* [in] */ EOC_ChangeType changeType,
            /* [in] */ BSTR bstrSubscriptionID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangedEventClass )( 
            IEventObjectChange __RPC_FAR * This,
            /* [in] */ EOC_ChangeType changeType,
            /* [in] */ BSTR bstrEventClassID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChangedPublisher )( 
            IEventObjectChange __RPC_FAR * This,
            /* [in] */ EOC_ChangeType changeType,
            /* [in] */ BSTR bstrPublisherID);
        
        END_INTERFACE
    } IEventObjectChangeVtbl;

    interface IEventObjectChange
    {
        CONST_VTBL struct IEventObjectChangeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventObjectChange_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventObjectChange_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventObjectChange_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventObjectChange_ChangedSubscription(This,changeType,bstrSubscriptionID)	\
    (This)->lpVtbl -> ChangedSubscription(This,changeType,bstrSubscriptionID)

#define IEventObjectChange_ChangedEventClass(This,changeType,bstrEventClassID)	\
    (This)->lpVtbl -> ChangedEventClass(This,changeType,bstrEventClassID)

#define IEventObjectChange_ChangedPublisher(This,changeType,bstrPublisherID)	\
    (This)->lpVtbl -> ChangedPublisher(This,changeType,bstrPublisherID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEventObjectChange_ChangedSubscription_Proxy( 
    IEventObjectChange __RPC_FAR * This,
    /* [in] */ EOC_ChangeType changeType,
    /* [in] */ BSTR bstrSubscriptionID);


void __RPC_STUB IEventObjectChange_ChangedSubscription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEventObjectChange_ChangedEventClass_Proxy( 
    IEventObjectChange __RPC_FAR * This,
    /* [in] */ EOC_ChangeType changeType,
    /* [in] */ BSTR bstrEventClassID);


void __RPC_STUB IEventObjectChange_ChangedEventClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEventObjectChange_ChangedPublisher_Proxy( 
    IEventObjectChange __RPC_FAR * This,
    /* [in] */ EOC_ChangeType changeType,
    /* [in] */ BSTR bstrPublisherID);


void __RPC_STUB IEventObjectChange_ChangedPublisher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventObjectChange_INTERFACE_DEFINED__ */


#ifndef __IEnumEventObject_INTERFACE_DEFINED__
#define __IEnumEventObject_INTERFACE_DEFINED__

/* interface IEnumEventObject */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumEventObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F4A07D63-2E25-11D1-9964-00C04FBBB345")
    IEnumEventObject : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumEventObject __RPC_FAR *__RPC_FAR *ppInterface) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG cReqElem,
            /* [length_is][size_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface,
            /* [out] */ ULONG __RPC_FAR *cRetElem) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG cSkipElem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumEventObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumEventObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumEventObject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumEventObject __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumEventObject __RPC_FAR * This,
            /* [out] */ IEnumEventObject __RPC_FAR *__RPC_FAR *ppInterface);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumEventObject __RPC_FAR * This,
            /* [in] */ ULONG cReqElem,
            /* [length_is][size_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface,
            /* [out] */ ULONG __RPC_FAR *cRetElem);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumEventObject __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumEventObject __RPC_FAR * This,
            /* [in] */ ULONG cSkipElem);
        
        END_INTERFACE
    } IEnumEventObjectVtbl;

    interface IEnumEventObject
    {
        CONST_VTBL struct IEnumEventObjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumEventObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumEventObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumEventObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumEventObject_Clone(This,ppInterface)	\
    (This)->lpVtbl -> Clone(This,ppInterface)

#define IEnumEventObject_Next(This,cReqElem,ppInterface,cRetElem)	\
    (This)->lpVtbl -> Next(This,cReqElem,ppInterface,cRetElem)

#define IEnumEventObject_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumEventObject_Skip(This,cSkipElem)	\
    (This)->lpVtbl -> Skip(This,cSkipElem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumEventObject_Clone_Proxy( 
    IEnumEventObject __RPC_FAR * This,
    /* [out] */ IEnumEventObject __RPC_FAR *__RPC_FAR *ppInterface);


void __RPC_STUB IEnumEventObject_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumEventObject_Next_Proxy( 
    IEnumEventObject __RPC_FAR * This,
    /* [in] */ ULONG cReqElem,
    /* [length_is][size_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppInterface,
    /* [out] */ ULONG __RPC_FAR *cRetElem);


void __RPC_STUB IEnumEventObject_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumEventObject_Reset_Proxy( 
    IEnumEventObject __RPC_FAR * This);


void __RPC_STUB IEnumEventObject_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEnumEventObject_Skip_Proxy( 
    IEnumEventObject __RPC_FAR * This,
    /* [in] */ ULONG cSkipElem);


void __RPC_STUB IEnumEventObject_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumEventObject_INTERFACE_DEFINED__ */


#ifndef __IEventObjectCollection_INTERFACE_DEFINED__
#define __IEventObjectCollection_INTERFACE_DEFINED__

/* interface IEventObjectCollection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventObjectCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f89ac270-d4eb-11d1-b682-00805fc79216")
    IEventObjectCollection : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkEnum) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Item( 
            /* [in] */ BSTR objectID,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_NewEnum( 
            /* [retval][out] */ IEnumEventObject __RPC_FAR *__RPC_FAR *ppEnum) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ VARIANT __RPC_FAR *item,
            /* [in] */ BSTR objectID) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ BSTR objectID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventObjectCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventObjectCollection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventObjectCollection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][restricted][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkEnum);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Item )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ BSTR objectID,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NewEnum )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [retval][out] */ IEnumEventObject __RPC_FAR *__RPC_FAR *ppEnum);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *item,
            /* [in] */ BSTR objectID);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IEventObjectCollection __RPC_FAR * This,
            /* [in] */ BSTR objectID);
        
        END_INTERFACE
    } IEventObjectCollectionVtbl;

    interface IEventObjectCollection
    {
        CONST_VTBL struct IEventObjectCollectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventObjectCollection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventObjectCollection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventObjectCollection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventObjectCollection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventObjectCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventObjectCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventObjectCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventObjectCollection_get__NewEnum(This,ppUnkEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,ppUnkEnum)

#define IEventObjectCollection_get_Item(This,objectID,pItem)	\
    (This)->lpVtbl -> get_Item(This,objectID,pItem)

#define IEventObjectCollection_get_NewEnum(This,ppEnum)	\
    (This)->lpVtbl -> get_NewEnum(This,ppEnum)

#define IEventObjectCollection_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IEventObjectCollection_Add(This,item,objectID)	\
    (This)->lpVtbl -> Add(This,item,objectID)

#define IEventObjectCollection_Remove(This,objectID)	\
    (This)->lpVtbl -> Remove(This,objectID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][propget][id] */ HRESULT STDMETHODCALLTYPE IEventObjectCollection_get__NewEnum_Proxy( 
    IEventObjectCollection __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppUnkEnum);


void __RPC_STUB IEventObjectCollection_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IEventObjectCollection_get_Item_Proxy( 
    IEventObjectCollection __RPC_FAR * This,
    /* [in] */ BSTR objectID,
    /* [retval][out] */ VARIANT __RPC_FAR *pItem);


void __RPC_STUB IEventObjectCollection_get_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventObjectCollection_get_NewEnum_Proxy( 
    IEventObjectCollection __RPC_FAR * This,
    /* [retval][out] */ IEnumEventObject __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEventObjectCollection_get_NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventObjectCollection_get_Count_Proxy( 
    IEventObjectCollection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IEventObjectCollection_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventObjectCollection_Add_Proxy( 
    IEventObjectCollection __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *item,
    /* [in] */ BSTR objectID);


void __RPC_STUB IEventObjectCollection_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventObjectCollection_Remove_Proxy( 
    IEventObjectCollection __RPC_FAR * This,
    /* [in] */ BSTR objectID);


void __RPC_STUB IEventObjectCollection_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventObjectCollection_INTERFACE_DEFINED__ */


#ifndef __IEventProperty_INTERFACE_DEFINED__
#define __IEventProperty_INTERFACE_DEFINED__

/* interface IEventProperty */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("da538ee2-f4de-11d1-b6bb-00805fc79216")
    IEventProperty : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *propertyName) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR propertyName) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ VARIANT __RPC_FAR *propertyValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventProperty __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventProperty __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventProperty __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventProperty __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventProperty __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventProperty __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IEventProperty __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *propertyName);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IEventProperty __RPC_FAR * This,
            /* [in] */ BSTR propertyName);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IEventProperty __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            IEventProperty __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *propertyValue);
        
        END_INTERFACE
    } IEventPropertyVtbl;

    interface IEventProperty
    {
        CONST_VTBL struct IEventPropertyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventProperty_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventProperty_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventProperty_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventProperty_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventProperty_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventProperty_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventProperty_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventProperty_get_Name(This,propertyName)	\
    (This)->lpVtbl -> get_Name(This,propertyName)

#define IEventProperty_put_Name(This,propertyName)	\
    (This)->lpVtbl -> put_Name(This,propertyName)

#define IEventProperty_get_Value(This,propertyValue)	\
    (This)->lpVtbl -> get_Value(This,propertyValue)

#define IEventProperty_put_Value(This,propertyValue)	\
    (This)->lpVtbl -> put_Value(This,propertyValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventProperty_get_Name_Proxy( 
    IEventProperty __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *propertyName);


void __RPC_STUB IEventProperty_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventProperty_put_Name_Proxy( 
    IEventProperty __RPC_FAR * This,
    /* [in] */ BSTR propertyName);


void __RPC_STUB IEventProperty_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventProperty_get_Value_Proxy( 
    IEventProperty __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventProperty_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventProperty_put_Value_Proxy( 
    IEventProperty __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *propertyValue);


void __RPC_STUB IEventProperty_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventProperty_INTERFACE_DEFINED__ */


#ifndef __IEventControl_INTERFACE_DEFINED__
#define __IEventControl_INTERFACE_DEFINED__

/* interface IEventControl */
/* [unique][helpstring][dual][uuid][object] */ 

// *************************************************************************
// This is a Deprecated interface - Use IMultiInterfaceEventControl instead.
// *************************************************************************

EXTERN_C const IID IID_IEventControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0343e2f4-86f6-11d1-b760-00c04fb926af")
    IEventControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetPublisherFilter( 
            /* [in] */ BSTR methodName,
            /* [unique][in] */ IPublisherFilter __RPC_FAR *pPublisherFilter) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AllowInprocActivation( 
            /* [retval][out] */ BOOL __RPC_FAR *pfAllowInprocActivation) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AllowInprocActivation( 
            /* [in] */ BOOL fAllowInprocActivation) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSubscriptions( 
            /* [in] */ BSTR methodName,
            /* [unique][in] */ BSTR optionalCriteria,
            /* [unique][in] */ int __RPC_FAR *optionalErrorIndex,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *ppCollection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetDefaultQuery( 
            /* [in] */ BSTR methodName,
            /* [in] */ BSTR criteria,
            /* [retval][out] */ int __RPC_FAR *errorIndex) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IEventControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPublisherFilter )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ BSTR methodName,
            /* [unique][in] */ IPublisherFilter __RPC_FAR *pPublisherFilter);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowInprocActivation )( 
            IEventControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfAllowInprocActivation);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowInprocActivation )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ BOOL fAllowInprocActivation);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubscriptions )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ BSTR methodName,
            /* [unique][in] */ BSTR optionalCriteria,
            /* [unique][in] */ int __RPC_FAR *optionalErrorIndex,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *ppCollection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultQuery )( 
            IEventControl __RPC_FAR * This,
            /* [in] */ BSTR methodName,
            /* [in] */ BSTR criteria,
            /* [retval][out] */ int __RPC_FAR *errorIndex);
        
        END_INTERFACE
    } IEventControlVtbl;

    interface IEventControl
    {
        CONST_VTBL struct IEventControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventControl_SetPublisherFilter(This,methodName,pPublisherFilter)	\
    (This)->lpVtbl -> SetPublisherFilter(This,methodName,pPublisherFilter)

#define IEventControl_get_AllowInprocActivation(This,pfAllowInprocActivation)	\
    (This)->lpVtbl -> get_AllowInprocActivation(This,pfAllowInprocActivation)

#define IEventControl_put_AllowInprocActivation(This,fAllowInprocActivation)	\
    (This)->lpVtbl -> put_AllowInprocActivation(This,fAllowInprocActivation)

#define IEventControl_GetSubscriptions(This,methodName,optionalCriteria,optionalErrorIndex,ppCollection)	\
    (This)->lpVtbl -> GetSubscriptions(This,methodName,optionalCriteria,optionalErrorIndex,ppCollection)

#define IEventControl_SetDefaultQuery(This,methodName,criteria,errorIndex)	\
    (This)->lpVtbl -> SetDefaultQuery(This,methodName,criteria,errorIndex)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventControl_SetPublisherFilter_Proxy( 
    IEventControl __RPC_FAR * This,
    /* [in] */ BSTR methodName,
    /* [unique][in] */ IPublisherFilter __RPC_FAR *pPublisherFilter);


void __RPC_STUB IEventControl_SetPublisherFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IEventControl_get_AllowInprocActivation_Proxy( 
    IEventControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfAllowInprocActivation);


void __RPC_STUB IEventControl_get_AllowInprocActivation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IEventControl_put_AllowInprocActivation_Proxy( 
    IEventControl __RPC_FAR * This,
    /* [in] */ BOOL fAllowInprocActivation);


void __RPC_STUB IEventControl_put_AllowInprocActivation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventControl_GetSubscriptions_Proxy( 
    IEventControl __RPC_FAR * This,
    /* [in] */ BSTR methodName,
    /* [unique][in] */ BSTR optionalCriteria,
    /* [unique][in] */ int __RPC_FAR *optionalErrorIndex,
    /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *ppCollection);


void __RPC_STUB IEventControl_GetSubscriptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventControl_SetDefaultQuery_Proxy( 
    IEventControl __RPC_FAR * This,
    /* [in] */ BSTR methodName,
    /* [in] */ BSTR criteria,
    /* [retval][out] */ int __RPC_FAR *errorIndex);


void __RPC_STUB IEventControl_SetDefaultQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventControl_INTERFACE_DEFINED__ */


#ifndef __IMultiInterfaceEventControl_INTERFACE_DEFINED__
#define __IMultiInterfaceEventControl_INTERFACE_DEFINED__

/* interface IMultiInterfaceEventControl */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IMultiInterfaceEventControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0343e2f5-86f6-11d1-b760-00c04fb926af")
    IMultiInterfaceEventControl : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetMultiInterfacePublisherFilter( 
            /* [unique][in] */ IMultiInterfacePublisherFilter __RPC_FAR *classFilter) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSubscriptions( 
            /* [in] */ REFIID eventIID,
            /* [in] */ BSTR bstrMethodName,
            /* [unique][in] */ BSTR optionalCriteria,
            /* [unique][in] */ int __RPC_FAR *optionalErrorIndex,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *ppCollection) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetDefaultQuery( 
            /* [in] */ REFIID eventIID,
            /* [in] */ BSTR bstrMethodName,
            /* [in] */ BSTR bstrCriteria,
            /* [retval][out] */ int __RPC_FAR *errorIndex) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AllowInprocActivation( 
            /* [retval][out] */ BOOL __RPC_FAR *pfAllowInprocActivation) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_AllowInprocActivation( 
            /* [in] */ BOOL fAllowInprocActivation) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FireInParallel( 
            /* [retval][out] */ BOOL __RPC_FAR *pfFireInParallel) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_FireInParallel( 
            /* [in] */ BOOL fFireInParallel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMultiInterfaceEventControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMultiInterfaceEventControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMultiInterfaceEventControl __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetMultiInterfacePublisherFilter )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [unique][in] */ IMultiInterfacePublisherFilter __RPC_FAR *classFilter);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSubscriptions )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [in] */ REFIID eventIID,
            /* [in] */ BSTR bstrMethodName,
            /* [unique][in] */ BSTR optionalCriteria,
            /* [unique][in] */ int __RPC_FAR *optionalErrorIndex,
            /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *ppCollection);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetDefaultQuery )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [in] */ REFIID eventIID,
            /* [in] */ BSTR bstrMethodName,
            /* [in] */ BSTR bstrCriteria,
            /* [retval][out] */ int __RPC_FAR *errorIndex);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllowInprocActivation )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfAllowInprocActivation);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AllowInprocActivation )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [in] */ BOOL fAllowInprocActivation);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FireInParallel )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfFireInParallel);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FireInParallel )( 
            IMultiInterfaceEventControl __RPC_FAR * This,
            /* [in] */ BOOL fFireInParallel);
        
        END_INTERFACE
    } IMultiInterfaceEventControlVtbl;

    interface IMultiInterfaceEventControl
    {
        CONST_VTBL struct IMultiInterfaceEventControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMultiInterfaceEventControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMultiInterfaceEventControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMultiInterfaceEventControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMultiInterfaceEventControl_SetMultiInterfacePublisherFilter(This,classFilter)	\
    (This)->lpVtbl -> SetMultiInterfacePublisherFilter(This,classFilter)

#define IMultiInterfaceEventControl_GetSubscriptions(This,eventIID,bstrMethodName,optionalCriteria,optionalErrorIndex,ppCollection)	\
    (This)->lpVtbl -> GetSubscriptions(This,eventIID,bstrMethodName,optionalCriteria,optionalErrorIndex,ppCollection)

#define IMultiInterfaceEventControl_SetDefaultQuery(This,eventIID,bstrMethodName,bstrCriteria,errorIndex)	\
    (This)->lpVtbl -> SetDefaultQuery(This,eventIID,bstrMethodName,bstrCriteria,errorIndex)

#define IMultiInterfaceEventControl_get_AllowInprocActivation(This,pfAllowInprocActivation)	\
    (This)->lpVtbl -> get_AllowInprocActivation(This,pfAllowInprocActivation)

#define IMultiInterfaceEventControl_put_AllowInprocActivation(This,fAllowInprocActivation)	\
    (This)->lpVtbl -> put_AllowInprocActivation(This,fAllowInprocActivation)

#define IMultiInterfaceEventControl_get_FireInParallel(This,pfFireInParallel)	\
    (This)->lpVtbl -> get_FireInParallel(This,pfFireInParallel)

#define IMultiInterfaceEventControl_put_FireInParallel(This,fFireInParallel)	\
    (This)->lpVtbl -> put_FireInParallel(This,fFireInParallel)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_SetMultiInterfacePublisherFilter_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [unique][in] */ IMultiInterfacePublisherFilter __RPC_FAR *classFilter);


void __RPC_STUB IMultiInterfaceEventControl_SetMultiInterfacePublisherFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_GetSubscriptions_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [in] */ REFIID eventIID,
    /* [in] */ BSTR bstrMethodName,
    /* [unique][in] */ BSTR optionalCriteria,
    /* [unique][in] */ int __RPC_FAR *optionalErrorIndex,
    /* [retval][out] */ IEventObjectCollection __RPC_FAR *__RPC_FAR *ppCollection);


void __RPC_STUB IMultiInterfaceEventControl_GetSubscriptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_SetDefaultQuery_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [in] */ REFIID eventIID,
    /* [in] */ BSTR bstrMethodName,
    /* [in] */ BSTR bstrCriteria,
    /* [retval][out] */ int __RPC_FAR *errorIndex);


void __RPC_STUB IMultiInterfaceEventControl_SetDefaultQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_get_AllowInprocActivation_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfAllowInprocActivation);


void __RPC_STUB IMultiInterfaceEventControl_get_AllowInprocActivation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_put_AllowInprocActivation_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [in] */ BOOL fAllowInprocActivation);


void __RPC_STUB IMultiInterfaceEventControl_put_AllowInprocActivation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_get_FireInParallel_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfFireInParallel);


void __RPC_STUB IMultiInterfaceEventControl_get_FireInParallel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IMultiInterfaceEventControl_put_FireInParallel_Proxy( 
    IMultiInterfaceEventControl __RPC_FAR * This,
    /* [in] */ BOOL fFireInParallel);


void __RPC_STUB IMultiInterfaceEventControl_put_FireInParallel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMultiInterfaceEventControl_INTERFACE_DEFINED__ */



#ifndef __DummyEventSystemLib_LIBRARY_DEFINED__
#define __DummyEventSystemLib_LIBRARY_DEFINED__

/* library DummyEventSystemLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DummyEventSystemLib;

EXTERN_C const CLSID CLSID_CEventSystem;

#ifdef __cplusplus

class DECLSPEC_UUID("4E14FBA2-2E22-11D1-9964-00C04FBBB345")
CEventSystem;
#endif

EXTERN_C const CLSID CLSID_CEventPublisher;

#ifdef __cplusplus

class DECLSPEC_UUID("ab944620-79c6-11d1-88f9-0080c7d771bf")
CEventPublisher;
#endif

EXTERN_C const CLSID CLSID_CEventClass;

#ifdef __cplusplus

class DECLSPEC_UUID("cdbec9c0-7a68-11d1-88f9-0080c7d771bf")
CEventClass;
#endif

EXTERN_C const CLSID CLSID_CEventSubscription;

#ifdef __cplusplus

class DECLSPEC_UUID("7542e960-79c7-11d1-88f9-0080c7d771bf")
CEventSubscription;
#endif

EXTERN_C const CLSID CLSID_EventObjectChange;

#ifdef __cplusplus

class DECLSPEC_UUID("d0565000-9df4-11d1-a281-00c04fca0aa7")
EventObjectChange;
#endif
#endif /* __DummyEventSystemLib_LIBRARY_DEFINED__ */

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


