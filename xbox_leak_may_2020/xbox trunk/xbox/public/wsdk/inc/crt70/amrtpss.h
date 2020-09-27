
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Sat Jul 31 00:49:43 1999
 */
/* Compiler settings for amrtpss.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __amrtpss_h__
#define __amrtpss_h__

/* Forward Declarations */ 

#ifndef __ISilenceSuppressor_FWD_DEFINED__
#define __ISilenceSuppressor_FWD_DEFINED__
typedef interface ISilenceSuppressor ISilenceSuppressor;
#endif 	/* __ISilenceSuppressor_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ISilenceSuppressor_INTERFACE_DEFINED__
#define __ISilenceSuppressor_INTERFACE_DEFINED__

/* interface ISilenceSuppressor */
/* [unique][helpstring][local][uuid][object] */ 


EXTERN_C const IID IID_ISilenceSuppressor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("40C1D160-52F2-11d0-A874-00AA00B5CA1B")
    ISilenceSuppressor : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPostplayTime( 
            /* [out] */ LPDWORD lpdwPostplayBufferTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPostplayTime( 
            /* [in] */ DWORD dwPostplayBufferTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetKeepPlayTime( 
            /* [out] */ LPDWORD lpdwRunningAverageTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetKeepPlayTime( 
            /* [in] */ DWORD dwRunningAverageTime) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThresholdIncrementor( 
            /* [out] */ LPDWORD lpdwThresholdIncrementor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThresholdIncrementor( 
            /* [in] */ DWORD dwThresholdIncrementor) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBaseThreshold( 
            /* [out] */ LPDWORD lpdwBaseThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBaseThreshold( 
            /* [in] */ DWORD dwBaseThreshold) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableEvents( 
            /* [in] */ DWORD dwMask,
            /* [in] */ DWORD dwMinimumInterval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISilenceSuppressorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISilenceSuppressor __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISilenceSuppressor __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPostplayTime )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [out] */ LPDWORD lpdwPostplayBufferTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetPostplayTime )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [in] */ DWORD dwPostplayBufferTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetKeepPlayTime )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [out] */ LPDWORD lpdwRunningAverageTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetKeepPlayTime )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [in] */ DWORD dwRunningAverageTime);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetThresholdIncrementor )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [out] */ LPDWORD lpdwThresholdIncrementor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetThresholdIncrementor )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [in] */ DWORD dwThresholdIncrementor);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBaseThreshold )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [out] */ LPDWORD lpdwBaseThreshold);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetBaseThreshold )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [in] */ DWORD dwBaseThreshold);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableEvents )( 
            ISilenceSuppressor __RPC_FAR * This,
            /* [in] */ DWORD dwMask,
            /* [in] */ DWORD dwMinimumInterval);
        
        END_INTERFACE
    } ISilenceSuppressorVtbl;

    interface ISilenceSuppressor
    {
        CONST_VTBL struct ISilenceSuppressorVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISilenceSuppressor_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISilenceSuppressor_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISilenceSuppressor_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISilenceSuppressor_GetPostplayTime(This,lpdwPostplayBufferTime)	\
    (This)->lpVtbl -> GetPostplayTime(This,lpdwPostplayBufferTime)

#define ISilenceSuppressor_SetPostplayTime(This,dwPostplayBufferTime)	\
    (This)->lpVtbl -> SetPostplayTime(This,dwPostplayBufferTime)

#define ISilenceSuppressor_GetKeepPlayTime(This,lpdwRunningAverageTime)	\
    (This)->lpVtbl -> GetKeepPlayTime(This,lpdwRunningAverageTime)

#define ISilenceSuppressor_SetKeepPlayTime(This,dwRunningAverageTime)	\
    (This)->lpVtbl -> SetKeepPlayTime(This,dwRunningAverageTime)

#define ISilenceSuppressor_GetThresholdIncrementor(This,lpdwThresholdIncrementor)	\
    (This)->lpVtbl -> GetThresholdIncrementor(This,lpdwThresholdIncrementor)

#define ISilenceSuppressor_SetThresholdIncrementor(This,dwThresholdIncrementor)	\
    (This)->lpVtbl -> SetThresholdIncrementor(This,dwThresholdIncrementor)

#define ISilenceSuppressor_GetBaseThreshold(This,lpdwBaseThreshold)	\
    (This)->lpVtbl -> GetBaseThreshold(This,lpdwBaseThreshold)

#define ISilenceSuppressor_SetBaseThreshold(This,dwBaseThreshold)	\
    (This)->lpVtbl -> SetBaseThreshold(This,dwBaseThreshold)

#define ISilenceSuppressor_EnableEvents(This,dwMask,dwMinimumInterval)	\
    (This)->lpVtbl -> EnableEvents(This,dwMask,dwMinimumInterval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISilenceSuppressor_GetPostplayTime_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [out] */ LPDWORD lpdwPostplayBufferTime);


void __RPC_STUB ISilenceSuppressor_GetPostplayTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_SetPostplayTime_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [in] */ DWORD dwPostplayBufferTime);


void __RPC_STUB ISilenceSuppressor_SetPostplayTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_GetKeepPlayTime_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [out] */ LPDWORD lpdwRunningAverageTime);


void __RPC_STUB ISilenceSuppressor_GetKeepPlayTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_SetKeepPlayTime_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [in] */ DWORD dwRunningAverageTime);


void __RPC_STUB ISilenceSuppressor_SetKeepPlayTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_GetThresholdIncrementor_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [out] */ LPDWORD lpdwThresholdIncrementor);


void __RPC_STUB ISilenceSuppressor_GetThresholdIncrementor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_SetThresholdIncrementor_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [in] */ DWORD dwThresholdIncrementor);


void __RPC_STUB ISilenceSuppressor_SetThresholdIncrementor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_GetBaseThreshold_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [out] */ LPDWORD lpdwBaseThreshold);


void __RPC_STUB ISilenceSuppressor_GetBaseThreshold_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_SetBaseThreshold_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [in] */ DWORD dwBaseThreshold);


void __RPC_STUB ISilenceSuppressor_SetBaseThreshold_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISilenceSuppressor_EnableEvents_Proxy( 
    ISilenceSuppressor __RPC_FAR * This,
    /* [in] */ DWORD dwMask,
    /* [in] */ DWORD dwMinimumInterval);


void __RPC_STUB ISilenceSuppressor_EnableEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISilenceSuppressor_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_amrtpss_0105 */
/* [local] */ 

EXTERN_C const CLSID CLSID_SilenceSuppressionFilter;
EXTERN_C const CLSID CLSID_SilenceSuppressionProperties;
#define AGC_EVENTBASE (100000)
typedef 
enum AGC_EVENT
    {	AGC_INCREASE_GAIN	= 0,
	AGC_DECREASE_GAIN	= AGC_INCREASE_GAIN + 1,
	AGC_TALKING	= AGC_DECREASE_GAIN + 1,
	AGC_SILENCE	= AGC_TALKING + 1
    }	AGC_EVENT;



extern RPC_IF_HANDLE __MIDL_itf_amrtpss_0105_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_amrtpss_0105_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


