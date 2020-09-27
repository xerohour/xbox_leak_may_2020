
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Mon Sep 20 16:32:20 1999
 */
/* Compiler settings for istats.idl:
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

#ifndef __istats_h__
#define __istats_h__

/* Forward Declarations */ 

#ifndef __IStats_FWD_DEFINED__
#define __IStats_FWD_DEFINED__
typedef interface IStats IStats;
#endif 	/* __IStats_FWD_DEFINED__ */


/* header files for imported files */
#include "NPPTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IStats_INTERFACE_DEFINED__
#define __IStats_INTERFACE_DEFINED__

/* interface IStats */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_IStats;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("944AD530-B09D-11ce-B59C-00AA006CB37D")
    IStats : public IUnknown
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
        
        virtual HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetControlState( 
            /* [out] */ BOOL __RPC_FAR *IsRunnning,
            /* [out] */ BOOL __RPC_FAR *IsPaused) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTotalStatistics( 
            /* [out] */ LPSTATISTICS lpStats,
            /* [in] */ BOOL fClearAfterReading) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetConversationStatistics( 
            /* [out] */ DWORD __RPC_FAR *nSessions,
            /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
            /* [out] */ DWORD __RPC_FAR *nStations,
            /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
            /* [in] */ BOOL fClearAfterReading) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertSpecialFrame( 
            /* [in] */ DWORD FrameType,
            /* [in] */ DWORD Flags,
            /* [in] */ LPBYTE pUserData,
            /* [in] */ DWORD UserDataLength) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryStations( 
            /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IStatsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IStats __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IStats __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IStats __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IStats __RPC_FAR * This,
            /* [in] */ HBLOB hInputBlob,
            /* [in] */ LPVOID StatusCallbackProc,
            /* [in] */ LPVOID UserContext,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IStats __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStatus )( 
            IStats __RPC_FAR * This,
            /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Configure )( 
            IStats __RPC_FAR * This,
            /* [in] */ HBLOB hConfigurationBlob,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            IStats __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IStats __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            IStats __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IStats __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetControlState )( 
            IStats __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *IsRunnning,
            /* [out] */ BOOL __RPC_FAR *IsPaused);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalStatistics )( 
            IStats __RPC_FAR * This,
            /* [out] */ LPSTATISTICS lpStats,
            /* [in] */ BOOL fClearAfterReading);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConversationStatistics )( 
            IStats __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *nSessions,
            /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
            /* [out] */ DWORD __RPC_FAR *nStations,
            /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
            /* [in] */ BOOL fClearAfterReading);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertSpecialFrame )( 
            IStats __RPC_FAR * This,
            /* [in] */ DWORD FrameType,
            /* [in] */ DWORD Flags,
            /* [in] */ LPBYTE pUserData,
            /* [in] */ DWORD UserDataLength);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStations )( 
            IStats __RPC_FAR * This,
            /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);
        
        END_INTERFACE
    } IStatsVtbl;

    interface IStats
    {
        CONST_VTBL struct IStatsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IStats_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IStats_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IStats_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IStats_Connect(This,hInputBlob,StatusCallbackProc,UserContext,hErrorBlob)	\
    (This)->lpVtbl -> Connect(This,hInputBlob,StatusCallbackProc,UserContext,hErrorBlob)

#define IStats_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IStats_QueryStatus(This,pNetworkStatus)	\
    (This)->lpVtbl -> QueryStatus(This,pNetworkStatus)

#define IStats_Configure(This,hConfigurationBlob,hErrorBlob)	\
    (This)->lpVtbl -> Configure(This,hConfigurationBlob,hErrorBlob)

#define IStats_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IStats_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IStats_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IStats_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IStats_GetControlState(This,IsRunnning,IsPaused)	\
    (This)->lpVtbl -> GetControlState(This,IsRunnning,IsPaused)

#define IStats_GetTotalStatistics(This,lpStats,fClearAfterReading)	\
    (This)->lpVtbl -> GetTotalStatistics(This,lpStats,fClearAfterReading)

#define IStats_GetConversationStatistics(This,nSessions,lpSessionStats,nStations,lpStationStats,fClearAfterReading)	\
    (This)->lpVtbl -> GetConversationStatistics(This,nSessions,lpSessionStats,nStations,lpStationStats,fClearAfterReading)

#define IStats_InsertSpecialFrame(This,FrameType,Flags,pUserData,UserDataLength)	\
    (This)->lpVtbl -> InsertSpecialFrame(This,FrameType,Flags,pUserData,UserDataLength)

#define IStats_QueryStations(This,lpQueryTable)	\
    (This)->lpVtbl -> QueryStations(This,lpQueryTable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IStats_Connect_Proxy( 
    IStats __RPC_FAR * This,
    /* [in] */ HBLOB hInputBlob,
    /* [in] */ LPVOID StatusCallbackProc,
    /* [in] */ LPVOID UserContext,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IStats_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_Disconnect_Proxy( 
    IStats __RPC_FAR * This);


void __RPC_STUB IStats_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_QueryStatus_Proxy( 
    IStats __RPC_FAR * This,
    /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);


void __RPC_STUB IStats_QueryStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_Configure_Proxy( 
    IStats __RPC_FAR * This,
    /* [in] */ HBLOB hConfigurationBlob,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IStats_Configure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_Start_Proxy( 
    IStats __RPC_FAR * This);


void __RPC_STUB IStats_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_Pause_Proxy( 
    IStats __RPC_FAR * This);


void __RPC_STUB IStats_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_Resume_Proxy( 
    IStats __RPC_FAR * This);


void __RPC_STUB IStats_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_Stop_Proxy( 
    IStats __RPC_FAR * This);


void __RPC_STUB IStats_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_GetControlState_Proxy( 
    IStats __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *IsRunnning,
    /* [out] */ BOOL __RPC_FAR *IsPaused);


void __RPC_STUB IStats_GetControlState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_GetTotalStatistics_Proxy( 
    IStats __RPC_FAR * This,
    /* [out] */ LPSTATISTICS lpStats,
    /* [in] */ BOOL fClearAfterReading);


void __RPC_STUB IStats_GetTotalStatistics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_GetConversationStatistics_Proxy( 
    IStats __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *nSessions,
    /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
    /* [out] */ DWORD __RPC_FAR *nStations,
    /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
    /* [in] */ BOOL fClearAfterReading);


void __RPC_STUB IStats_GetConversationStatistics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_InsertSpecialFrame_Proxy( 
    IStats __RPC_FAR * This,
    /* [in] */ DWORD FrameType,
    /* [in] */ DWORD Flags,
    /* [in] */ LPBYTE pUserData,
    /* [in] */ DWORD UserDataLength);


void __RPC_STUB IStats_InsertSpecialFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IStats_QueryStations_Proxy( 
    IStats __RPC_FAR * This,
    /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);


void __RPC_STUB IStats_QueryStations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IStats_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


