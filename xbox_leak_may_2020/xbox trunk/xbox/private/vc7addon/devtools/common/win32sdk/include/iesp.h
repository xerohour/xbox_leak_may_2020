
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Mon Sep 20 16:32:17 1999
 */
/* Compiler settings for iesp.idl:
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

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __iesp_h__
#define __iesp_h__

/* Forward Declarations */ 

#ifndef __IESP_FWD_DEFINED__
#define __IESP_FWD_DEFINED__
typedef interface IESP IESP;
#endif 	/* __IESP_FWD_DEFINED__ */


/* header files for imported files */
#include "NPPTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IESP_INTERFACE_DEFINED__
#define __IESP_INTERFACE_DEFINED__

/* interface IESP */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_IESP;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E99A04AA-AB95-11d0-BE96-00A0C94989DE")
    IESP : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ HBLOB hInputBlob,
            /* [in] */ LPVOID StatusCallbackProc,
            /* [in] */ LPVOID UserContext,
            /* [out] */ HBLOB hErrorBlob) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryStatus( 
            /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Configure( 
            /* [in] */ HBLOB hConfigurationBlob,
            /* [out] */ HBLOB hErrorBlob) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Start( 
            /* [out][string] */ char __RPC_FAR *pFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( 
            /* [out] */ LPSTATISTICS lpStats) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( 
            /* [out] */ LPSTATISTICS lpStats) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetControlState( 
            /* [out] */ BOOL __RPC_FAR *IsRunnning,
            /* [out] */ BOOL __RPC_FAR *IsPaused) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryStations( 
            /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IESPVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IESP __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IESP __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IESP __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IESP __RPC_FAR * This,
            /* [in] */ HBLOB hInputBlob,
            /* [in] */ LPVOID StatusCallbackProc,
            /* [in] */ LPVOID UserContext,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IESP __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStatus )( 
            IESP __RPC_FAR * This,
            /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Configure )( 
            IESP __RPC_FAR * This,
            /* [in] */ HBLOB hConfigurationBlob,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            IESP __RPC_FAR * This,
            /* [out][string] */ char __RPC_FAR *pFileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IESP __RPC_FAR * This,
            /* [out] */ LPSTATISTICS lpStats);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            IESP __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IESP __RPC_FAR * This,
            /* [out] */ LPSTATISTICS lpStats);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetControlState )( 
            IESP __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *IsRunnning,
            /* [out] */ BOOL __RPC_FAR *IsPaused);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStations )( 
            IESP __RPC_FAR * This,
            /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);
        
        END_INTERFACE
    } IESPVtbl;

    interface IESP
    {
        CONST_VTBL struct IESPVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IESP_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IESP_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IESP_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IESP_Connect(This,hInputBlob,StatusCallbackProc,UserContext,hErrorBlob)	\
    (This)->lpVtbl -> Connect(This,hInputBlob,StatusCallbackProc,UserContext,hErrorBlob)

#define IESP_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IESP_QueryStatus(This,pNetworkStatus)	\
    (This)->lpVtbl -> QueryStatus(This,pNetworkStatus)

#define IESP_Configure(This,hConfigurationBlob,hErrorBlob)	\
    (This)->lpVtbl -> Configure(This,hConfigurationBlob,hErrorBlob)

#define IESP_Start(This,pFileName)	\
    (This)->lpVtbl -> Start(This,pFileName)

#define IESP_Pause(This,lpStats)	\
    (This)->lpVtbl -> Pause(This,lpStats)

#define IESP_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IESP_Stop(This,lpStats)	\
    (This)->lpVtbl -> Stop(This,lpStats)

#define IESP_GetControlState(This,IsRunnning,IsPaused)	\
    (This)->lpVtbl -> GetControlState(This,IsRunnning,IsPaused)

#define IESP_QueryStations(This,lpQueryTable)	\
    (This)->lpVtbl -> QueryStations(This,lpQueryTable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IESP_Connect_Proxy( 
    IESP __RPC_FAR * This,
    /* [in] */ HBLOB hInputBlob,
    /* [in] */ LPVOID StatusCallbackProc,
    /* [in] */ LPVOID UserContext,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IESP_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_Disconnect_Proxy( 
    IESP __RPC_FAR * This);


void __RPC_STUB IESP_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_QueryStatus_Proxy( 
    IESP __RPC_FAR * This,
    /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);


void __RPC_STUB IESP_QueryStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_Configure_Proxy( 
    IESP __RPC_FAR * This,
    /* [in] */ HBLOB hConfigurationBlob,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IESP_Configure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_Start_Proxy( 
    IESP __RPC_FAR * This,
    /* [out][string] */ char __RPC_FAR *pFileName);


void __RPC_STUB IESP_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_Pause_Proxy( 
    IESP __RPC_FAR * This,
    /* [out] */ LPSTATISTICS lpStats);


void __RPC_STUB IESP_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_Resume_Proxy( 
    IESP __RPC_FAR * This);


void __RPC_STUB IESP_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_Stop_Proxy( 
    IESP __RPC_FAR * This,
    /* [out] */ LPSTATISTICS lpStats);


void __RPC_STUB IESP_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_GetControlState_Proxy( 
    IESP __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *IsRunnning,
    /* [out] */ BOOL __RPC_FAR *IsPaused);


void __RPC_STUB IESP_GetControlState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IESP_QueryStations_Proxy( 
    IESP __RPC_FAR * This,
    /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);


void __RPC_STUB IESP_QueryStations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IESP_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


