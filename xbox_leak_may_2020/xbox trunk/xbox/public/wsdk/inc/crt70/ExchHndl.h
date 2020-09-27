/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Mar 27 08:32:24 1998
 */
/* Compiler settings for J:\events\src\SS\exchhndl.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __exchhndl_h__
#define __exchhndl_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IExchangeEventHandler_FWD_DEFINED__
#define __IExchangeEventHandler_FWD_DEFINED__
typedef interface IExchangeEventHandler IExchangeEventHandler;
#endif 	/* __IExchangeEventHandler_FWD_DEFINED__ */


#ifndef __IEventSource_FWD_DEFINED__
#define __IEventSource_FWD_DEFINED__
typedef interface IEventSource IEventSource;
#endif 	/* __IEventSource_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_exchhndl_0000
 * at Fri Mar 27 08:32:24 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright 1997 - 1999 Microsoft Corporation.
//
//--------------------------------------------------------------------------


extern RPC_IF_HANDLE __MIDL_itf_exchhndl_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_exchhndl_0000_v0_0_s_ifspec;

#ifndef __IExchangeEventHandler_INTERFACE_DEFINED__
#define __IExchangeEventHandler_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IExchangeEventHandler
 * at Fri Mar 27 08:32:24 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IExchangeEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("69E54156-B371-11D0-BCD9-00AA00C1AB1C")
    IExchangeEventHandler : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecuteEvent( 
            /* [in] */ IDispatch __RPC_FAR *pEventSource) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExchangeEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IExchangeEventHandler __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IExchangeEventHandler __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IExchangeEventHandler __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExecuteEvent )( 
            IExchangeEventHandler __RPC_FAR * This,
            /* [in] */ IDispatch __RPC_FAR *pEventSource);
        
        END_INTERFACE
    } IExchangeEventHandlerVtbl;

    interface IExchangeEventHandler
    {
        CONST_VTBL struct IExchangeEventHandlerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExchangeEventHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IExchangeEventHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IExchangeEventHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IExchangeEventHandler_ExecuteEvent(This,pEventSource)	\
    (This)->lpVtbl -> ExecuteEvent(This,pEventSource)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IExchangeEventHandler_ExecuteEvent_Proxy( 
    IExchangeEventHandler __RPC_FAR * This,
    /* [in] */ IDispatch __RPC_FAR *pEventSource);


void __RPC_STUB IExchangeEventHandler_ExecuteEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IExchangeEventHandler_INTERFACE_DEFINED__ */


#ifndef __IEventSource_INTERFACE_DEFINED__
#define __IEventSource_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEventSource
 * at Fri Mar 27 08:32:24 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][oleautomation][helpstring][uuid][object] */ 



EXTERN_C const IID IID_IEventSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("69E54152-B371-11D0-BCD9-00AA00C1AB1C")
    IEventSource : public IUnknown
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Source( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pSource) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Source( 
            /* [in] */ IUnknown __RPC_FAR *varSource) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SourceID( 
            /* [retval][out] */ BSTR __RPC_FAR *pSourceID) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_SourceID( 
            /* [in] */ BSTR varSourceID) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MessageID( 
            /* [retval][out] */ BSTR __RPC_FAR *pMessageID) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MessageID( 
            /* [in] */ BSTR varMessageID) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Session( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pSession) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Session( 
            /* [in] */ IUnknown __RPC_FAR *varSession) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_EventType( 
            /* [retval][out] */ DWORD __RPC_FAR *pEventType) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_EventType( 
            /* [in] */ DWORD varEventType) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Response( 
            /* [retval][out] */ VARIANT __RPC_FAR *pResponse) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Response( 
            /* [in] */ VARIANT varResponse) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MaxExecutionTime( 
            /* [retval][out] */ DWORD __RPC_FAR *pMaxExecutionTime) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MaxExecutionTime( 
            /* [in] */ DWORD varMaxExecutionTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEventSource __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEventSource __RPC_FAR * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Source )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pSource);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Source )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *varSource);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SourceID )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pSourceID);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SourceID )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ BSTR varSourceID);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MessageID )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pMessageID);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MessageID )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ BSTR varMessageID);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Session )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pSession);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Session )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *varSession);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EventType )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pEventType);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EventType )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ DWORD varEventType);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Response )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pResponse);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Response )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ VARIANT varResponse);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxExecutionTime )( 
            IEventSource __RPC_FAR * This,
            /* [retval][out] */ DWORD __RPC_FAR *pMaxExecutionTime);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxExecutionTime )( 
            IEventSource __RPC_FAR * This,
            /* [in] */ DWORD varMaxExecutionTime);
        
        END_INTERFACE
    } IEventSourceVtbl;

    interface IEventSource
    {
        CONST_VTBL struct IEventSourceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventSource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventSource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventSource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventSource_get_Source(This,pSource)	\
    (This)->lpVtbl -> get_Source(This,pSource)

#define IEventSource_put_Source(This,varSource)	\
    (This)->lpVtbl -> put_Source(This,varSource)

#define IEventSource_get_SourceID(This,pSourceID)	\
    (This)->lpVtbl -> get_SourceID(This,pSourceID)

#define IEventSource_put_SourceID(This,varSourceID)	\
    (This)->lpVtbl -> put_SourceID(This,varSourceID)

#define IEventSource_get_MessageID(This,pMessageID)	\
    (This)->lpVtbl -> get_MessageID(This,pMessageID)

#define IEventSource_put_MessageID(This,varMessageID)	\
    (This)->lpVtbl -> put_MessageID(This,varMessageID)

#define IEventSource_get_Session(This,pSession)	\
    (This)->lpVtbl -> get_Session(This,pSession)

#define IEventSource_put_Session(This,varSession)	\
    (This)->lpVtbl -> put_Session(This,varSession)

#define IEventSource_get_EventType(This,pEventType)	\
    (This)->lpVtbl -> get_EventType(This,pEventType)

#define IEventSource_put_EventType(This,varEventType)	\
    (This)->lpVtbl -> put_EventType(This,varEventType)

#define IEventSource_get_Response(This,pResponse)	\
    (This)->lpVtbl -> get_Response(This,pResponse)

#define IEventSource_put_Response(This,varResponse)	\
    (This)->lpVtbl -> put_Response(This,varResponse)

#define IEventSource_get_MaxExecutionTime(This,pMaxExecutionTime)	\
    (This)->lpVtbl -> get_MaxExecutionTime(This,pMaxExecutionTime)

#define IEventSource_put_MaxExecutionTime(This,varMaxExecutionTime)	\
    (This)->lpVtbl -> put_MaxExecutionTime(This,varMaxExecutionTime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_Source_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pSource);


void __RPC_STUB IEventSource_get_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_Source_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *varSource);


void __RPC_STUB IEventSource_put_Source_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_SourceID_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pSourceID);


void __RPC_STUB IEventSource_get_SourceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_SourceID_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ BSTR varSourceID);


void __RPC_STUB IEventSource_put_SourceID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_MessageID_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pMessageID);


void __RPC_STUB IEventSource_get_MessageID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_MessageID_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ BSTR varMessageID);


void __RPC_STUB IEventSource_put_MessageID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_Session_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pSession);


void __RPC_STUB IEventSource_get_Session_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_Session_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *varSession);


void __RPC_STUB IEventSource_put_Session_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_EventType_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pEventType);


void __RPC_STUB IEventSource_get_EventType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_EventType_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ DWORD varEventType);


void __RPC_STUB IEventSource_put_EventType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_Response_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pResponse);


void __RPC_STUB IEventSource_get_Response_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_Response_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ VARIANT varResponse);


void __RPC_STUB IEventSource_put_Response_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IEventSource_get_MaxExecutionTime_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [retval][out] */ DWORD __RPC_FAR *pMaxExecutionTime);


void __RPC_STUB IEventSource_get_MaxExecutionTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IEventSource_put_MaxExecutionTime_Proxy( 
    IEventSource __RPC_FAR * This,
    /* [in] */ DWORD varMaxExecutionTime);


void __RPC_STUB IEventSource_put_MaxExecutionTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventSource_INTERFACE_DEFINED__ */


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
