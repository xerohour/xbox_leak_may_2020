
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Mon Sep 20 16:32:19 1999
 */
/* Compiler settings for irtc.idl:
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

#ifndef __irtc_h__
#define __irtc_h__

/* Forward Declarations */ 

#ifndef __IRTC_FWD_DEFINED__
#define __IRTC_FWD_DEFINED__
typedef interface IRTC IRTC;
#endif 	/* __IRTC_FWD_DEFINED__ */


/* header files for imported files */
#include "nmFinder.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_irtc_0000 */
/* [local] */ 

#define	DEFAULT_RTC_BUFFER_SIZE	( 0x100000 )

#define	RTC_FRAME_SIZE_FULL	( 0 )



extern RPC_IF_HANDLE __MIDL_itf_irtc_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_irtc_0000_v0_0_s_ifspec;

#ifndef __IRTC_INTERFACE_DEFINED__
#define __IRTC_INTERFACE_DEFINED__

/* interface IRTC */
/* [local][unique][uuid][object] */ 


EXTERN_C const IID IID_IRTC;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4811EA40-B582-11ce-B5AF-00AA006CB37D")
    IRTC : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ HBLOB hInputBlob,
            /* [in] */ LPVOID StatusCallbackProc,
            /* [in] */ LPVOID FramesCallbackProc,
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

    typedef struct IRTCVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IRTC __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IRTC __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IRTC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IRTC __RPC_FAR * This,
            /* [in] */ HBLOB hInputBlob,
            /* [in] */ LPVOID StatusCallbackProc,
            /* [in] */ LPVOID FramesCallbackProc,
            /* [in] */ LPVOID UserContext,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            IRTC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStatus )( 
            IRTC __RPC_FAR * This,
            /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Configure )( 
            IRTC __RPC_FAR * This,
            /* [in] */ HBLOB hConfigurationBlob,
            /* [out] */ HBLOB hErrorBlob);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            IRTC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pause )( 
            IRTC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Resume )( 
            IRTC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            IRTC __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetControlState )( 
            IRTC __RPC_FAR * This,
            /* [out] */ BOOL __RPC_FAR *IsRunnning,
            /* [out] */ BOOL __RPC_FAR *IsPaused);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTotalStatistics )( 
            IRTC __RPC_FAR * This,
            /* [out] */ LPSTATISTICS lpStats,
            /* [in] */ BOOL fClearAfterReading);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetConversationStatistics )( 
            IRTC __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *nSessions,
            /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
            /* [out] */ DWORD __RPC_FAR *nStations,
            /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
            /* [in] */ BOOL fClearAfterReading);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InsertSpecialFrame )( 
            IRTC __RPC_FAR * This,
            /* [in] */ DWORD FrameType,
            /* [in] */ DWORD Flags,
            /* [in] */ LPBYTE pUserData,
            /* [in] */ DWORD UserDataLength);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryStations )( 
            IRTC __RPC_FAR * This,
            /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);
        
        END_INTERFACE
    } IRTCVtbl;

    interface IRTC
    {
        CONST_VTBL struct IRTCVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRTC_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRTC_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRTC_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRTC_Connect(This,hInputBlob,StatusCallbackProc,FramesCallbackProc,UserContext,hErrorBlob)	\
    (This)->lpVtbl -> Connect(This,hInputBlob,StatusCallbackProc,FramesCallbackProc,UserContext,hErrorBlob)

#define IRTC_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define IRTC_QueryStatus(This,pNetworkStatus)	\
    (This)->lpVtbl -> QueryStatus(This,pNetworkStatus)

#define IRTC_Configure(This,hConfigurationBlob,hErrorBlob)	\
    (This)->lpVtbl -> Configure(This,hConfigurationBlob,hErrorBlob)

#define IRTC_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define IRTC_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IRTC_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IRTC_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IRTC_GetControlState(This,IsRunnning,IsPaused)	\
    (This)->lpVtbl -> GetControlState(This,IsRunnning,IsPaused)

#define IRTC_GetTotalStatistics(This,lpStats,fClearAfterReading)	\
    (This)->lpVtbl -> GetTotalStatistics(This,lpStats,fClearAfterReading)

#define IRTC_GetConversationStatistics(This,nSessions,lpSessionStats,nStations,lpStationStats,fClearAfterReading)	\
    (This)->lpVtbl -> GetConversationStatistics(This,nSessions,lpSessionStats,nStations,lpStationStats,fClearAfterReading)

#define IRTC_InsertSpecialFrame(This,FrameType,Flags,pUserData,UserDataLength)	\
    (This)->lpVtbl -> InsertSpecialFrame(This,FrameType,Flags,pUserData,UserDataLength)

#define IRTC_QueryStations(This,lpQueryTable)	\
    (This)->lpVtbl -> QueryStations(This,lpQueryTable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRTC_Connect_Proxy( 
    IRTC __RPC_FAR * This,
    /* [in] */ HBLOB hInputBlob,
    /* [in] */ LPVOID StatusCallbackProc,
    /* [in] */ LPVOID FramesCallbackProc,
    /* [in] */ LPVOID UserContext,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IRTC_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_Disconnect_Proxy( 
    IRTC __RPC_FAR * This);


void __RPC_STUB IRTC_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_QueryStatus_Proxy( 
    IRTC __RPC_FAR * This,
    /* [out] */ NETWORKSTATUS __RPC_FAR *pNetworkStatus);


void __RPC_STUB IRTC_QueryStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_Configure_Proxy( 
    IRTC __RPC_FAR * This,
    /* [in] */ HBLOB hConfigurationBlob,
    /* [out] */ HBLOB hErrorBlob);


void __RPC_STUB IRTC_Configure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_Start_Proxy( 
    IRTC __RPC_FAR * This);


void __RPC_STUB IRTC_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_Pause_Proxy( 
    IRTC __RPC_FAR * This);


void __RPC_STUB IRTC_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_Resume_Proxy( 
    IRTC __RPC_FAR * This);


void __RPC_STUB IRTC_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_Stop_Proxy( 
    IRTC __RPC_FAR * This);


void __RPC_STUB IRTC_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_GetControlState_Proxy( 
    IRTC __RPC_FAR * This,
    /* [out] */ BOOL __RPC_FAR *IsRunnning,
    /* [out] */ BOOL __RPC_FAR *IsPaused);


void __RPC_STUB IRTC_GetControlState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_GetTotalStatistics_Proxy( 
    IRTC __RPC_FAR * This,
    /* [out] */ LPSTATISTICS lpStats,
    /* [in] */ BOOL fClearAfterReading);


void __RPC_STUB IRTC_GetTotalStatistics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_GetConversationStatistics_Proxy( 
    IRTC __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *nSessions,
    /* [size_is][out] */ LPSESSIONSTATS lpSessionStats,
    /* [out] */ DWORD __RPC_FAR *nStations,
    /* [size_is][out] */ LPSTATIONSTATS lpStationStats,
    /* [in] */ BOOL fClearAfterReading);


void __RPC_STUB IRTC_GetConversationStatistics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_InsertSpecialFrame_Proxy( 
    IRTC __RPC_FAR * This,
    /* [in] */ DWORD FrameType,
    /* [in] */ DWORD Flags,
    /* [in] */ LPBYTE pUserData,
    /* [in] */ DWORD UserDataLength);


void __RPC_STUB IRTC_InsertSpecialFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRTC_QueryStations_Proxy( 
    IRTC __RPC_FAR * This,
    /* [out][in] */ QUERYTABLE __RPC_FAR *lpQueryTable);


void __RPC_STUB IRTC_QueryStations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRTC_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


