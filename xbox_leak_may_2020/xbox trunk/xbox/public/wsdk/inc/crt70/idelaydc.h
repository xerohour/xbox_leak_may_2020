
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Mon Sep 20 16:32:25 1999
 */
/* Compiler settings for idelaydc.idl:
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

#ifndef __idelaydc_h__
#define __idelaydc_h__

/* Forward Declarations */ 

#ifndef __IDelaydC_FWD_DEFINED__
#define __IDelaydC_FWD_DEFINED__
typedef interface IDelaydC IDelaydC;
#endif 	/* __IDelaydC_FWD_DEFINED__ */


/* header files for imported files */
#include "NPPTypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_idelaydc_0000 */
/* [local] */ 

#define	DEFAULT_DELAYED_BUFFER_SIZE	( 1 )

#define	USE_DEFAULT_DRIVE_LETTER	( 0 )

#define	RTC_FRAME_SIZE_FULL	( 0 )



extern RPC_IF_HANDLE __MIDL_itf_idelaydc_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_idelaydc_0000_v0_0_s_ifspec;

#ifndef __IDelaydC_INTERFACE_DEFINED__
#define __IDelaydC_INTERFACE_DEFINED__

/* interface IDelaydC */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_IDelaydC;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BFF9C030-B58F-11ce-B5B0-00AA006CB37D")
    IDelaydC : public IUnknown
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
            /* [out] */ char __RPC_FAR *pFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( 
            /* [out] */ LPSTATISTICS lpStats) = 0;
        
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

    typedef struct IDelaydCVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDelaydC __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDelaydC __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDelaydC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IDelaydC __RPC_FAR * This,
            /* [in] */ HBLOB hInputBlob,
            /* [in] */ LPVOID StatusCallbackProc,
            /* [in] */ LPVOID UserContext,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IDelaydC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStatus )( 
            IDelaydC __RPC_FAR * This,
            /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Configure )( 
            IDelaydC __RPC_FAR * This,
            /* [in] */ HBLOB hConfigurationBlob,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            IDelaydC __RPC_FAR * This,
            /* [out] */ char __RPC_FAR *pFileName);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IDelaydC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            IDelaydC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IDelaydC __RPC_FAR * This,
            /* [out] */ LPSTATISTICS lpStats);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetControlState )( 
            IDelaydC __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *IsRunnning,
            /* [out] */ BOOL __RPC_FAR *IsPaused);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalStatistics )( 
            IDelaydC __RPC_FAR * This,
            /* [out] */ LPSTATISTICS lpStats,
            /* [in] */ BOOL fClearAfterReading);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConversationStatistics )( 
            IDelaydC __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *nSessions,
            /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
            /* [out] */ DWORD __RPC_FAR *nStations,
            /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
            /* [in] */ BOOL fClearAfterReading);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertSpecialFrame )( 
            IDelaydC __RPC_FAR * This,
            /* [in] */ DWORD FrameType,
            /* [in] */ DWORD Flags,
            /* [in] */ LPBYTE pUserData,
            /* [in] */ DWORD UserDataLength);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStations )( 
            IDelaydC __RPC_FAR * This,
            /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);
        
        END_INTERFACE
    } IDelaydCVtbl;

    interface IDelaydC
    {
        CONST_VTBL struct IDelaydCVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDelaydC_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDelaydC_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDelaydC_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDelaydC_Connect(This,hInputBlob,StatusCallbackProc,UserContext,hErrorBlob)	\
    (This)->lpVtbl -> Connect(This,hInputBlob,StatusCallbackProc,UserContext,hErrorBlob)

#define IDelaydC_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IDelaydC_QueryStatus(This,pNetworkStatus)	\
    (This)->lpVtbl -> QueryStatus(This,pNetworkStatus)

#define IDelaydC_Configure(This,hConfigurationBlob,hErrorBlob)	\
    (This)->lpVtbl -> Configure(This,hConfigurationBlob,hErrorBlob)

#define IDelaydC_Start(This,pFileName)	\
    (This)->lpVtbl -> Start(This,pFileName)

#define IDelaydC_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IDelaydC_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IDelaydC_Stop(This,lpStats)	\
    (This)->lpVtbl -> Stop(This,lpStats)

#define IDelaydC_GetControlState(This,IsRunnning,IsPaused)	\
    (This)->lpVtbl -> GetControlState(This,IsRunnning,IsPaused)

#define IDelaydC_GetTotalStatistics(This,lpStats,fClearAfterReading)	\
    (This)->lpVtbl -> GetTotalStatistics(This,lpStats,fClearAfterReading)

#define IDelaydC_GetConversationStatistics(This,nSessions,lpSessionStats,nStations,lpStationStats,fClearAfterReading)	\
    (This)->lpVtbl -> GetConversationStatistics(This,nSessions,lpSessionStats,nStations,lpStationStats,fClearAfterReading)

#define IDelaydC_InsertSpecialFrame(This,FrameType,Flags,pUserData,UserDataLength)	\
    (This)->lpVtbl -> InsertSpecialFrame(This,FrameType,Flags,pUserData,UserDataLength)

#define IDelaydC_QueryStations(This,lpQueryTable)	\
    (This)->lpVtbl -> QueryStations(This,lpQueryTable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDelaydC_Connect_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [in] */ HBLOB hInputBlob,
    /* [in] */ LPVOID StatusCallbackProc,
    /* [in] */ LPVOID UserContext,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IDelaydC_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_Disconnect_Proxy( 
    IDelaydC __RPC_FAR * This);


void __RPC_STUB IDelaydC_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_QueryStatus_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);


void __RPC_STUB IDelaydC_QueryStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_Configure_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [in] */ HBLOB hConfigurationBlob,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IDelaydC_Configure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_Start_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out] */ char __RPC_FAR *pFileName);


void __RPC_STUB IDelaydC_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_Pause_Proxy( 
    IDelaydC __RPC_FAR * This);


void __RPC_STUB IDelaydC_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_Resume_Proxy( 
    IDelaydC __RPC_FAR * This);


void __RPC_STUB IDelaydC_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_Stop_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out] */ LPSTATISTICS lpStats);


void __RPC_STUB IDelaydC_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_GetControlState_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *IsRunnning,
    /* [out] */ BOOL __RPC_FAR *IsPaused);


void __RPC_STUB IDelaydC_GetControlState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_GetTotalStatistics_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out] */ LPSTATISTICS lpStats,
    /* [in] */ BOOL fClearAfterReading);


void __RPC_STUB IDelaydC_GetTotalStatistics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_GetConversationStatistics_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *nSessions,
    /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
    /* [out] */ DWORD __RPC_FAR *nStations,
    /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
    /* [in] */ BOOL fClearAfterReading);


void __RPC_STUB IDelaydC_GetConversationStatistics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_InsertSpecialFrame_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [in] */ DWORD FrameType,
    /* [in] */ DWORD Flags,
    /* [in] */ LPBYTE pUserData,
    /* [in] */ DWORD UserDataLength);


void __RPC_STUB IDelaydC_InsertSpecialFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDelaydC_QueryStations_Proxy( 
    IDelaydC __RPC_FAR * This,
    /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);


void __RPC_STUB IDelaydC_QueryStations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDelaydC_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


