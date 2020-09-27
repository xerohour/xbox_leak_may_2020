/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Tue May 19 19:41:41 1998
 */
/* Compiler settings for tvdisp.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __tvdisp_h__
#define __tvdisp_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ITVViewer_FWD_DEFINED__
#define __ITVViewer_FWD_DEFINED__
typedef interface ITVViewer ITVViewer;
#endif 	/* __ITVViewer_FWD_DEFINED__ */


#ifndef __ITVControl_FWD_DEFINED__
#define __ITVControl_FWD_DEFINED__
typedef interface ITVControl ITVControl;
#endif 	/* __ITVControl_FWD_DEFINED__ */


#ifndef __TVViewer_FWD_DEFINED__
#define __TVViewer_FWD_DEFINED__

#ifdef __cplusplus
typedef class TVViewer TVViewer;
#else
typedef struct TVViewer TVViewer;
#endif /* __cplusplus */

#endif 	/* __TVViewer_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __TVDisp_LIBRARY_DEFINED__
#define __TVDisp_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: TVDisp
 * at Tue May 19 19:41:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [version][uuid] */ 



EXTERN_C const IID LIBID_TVDisp;

#ifndef __ITVViewer_INTERFACE_DEFINED__
#define __ITVViewer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ITVViewer
 * at Tue May 19 19:41:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [object][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_ITVViewer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("3F8A2EA1-C171-11cf-868C-00805F2C11CE")
    ITVViewer : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetTVMode( 
            /* [in] */ boolean fTVMode) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsTVMode( 
            /* [retval][out] */ boolean __RPC_FAR *pfTVMode) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsChannelBarUp( 
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsModalDialogUp( 
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsLoaderActive( 
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GlobalStartTime( 
            /* [retval][out] */ DATE __RPC_FAR *pdate) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GlobalEndTime( 
            /* [retval][out] */ DATE __RPC_FAR *pdate) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ViewerID( 
            /* [retval][out] */ int __RPC_FAR *pidViewer) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE WantKeys( 
            /* [in] */ int nWantKeys) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Tune( 
            /* [in] */ long lTuningSpace,
            /* [in] */ long lChannelNumber,
            /* [in] */ long lVideoStream,
            /* [in] */ long lAudioStream,
            /* [in] */ BSTR bsIPStream) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCurrentTuningInfo( 
            /* [out] */ long __RPC_FAR *lTuningSpace,
            /* [out] */ long __RPC_FAR *lChannelNumber,
            /* [out] */ long __RPC_FAR *lVideoStream,
            /* [out] */ long __RPC_FAR *lAudioStream,
            /* [out] */ BSTR __RPC_FAR *pbsIPAddress) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPreviousTuningInfo( 
            /* [out] */ long __RPC_FAR *lTuningSpace,
            /* [out] */ long __RPC_FAR *lChannelNumber,
            /* [out] */ long __RPC_FAR *lVideoStream,
            /* [out] */ long __RPC_FAR *lAudioStream,
            /* [out] */ BSTR __RPC_FAR *pbsIPAddress) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetReminder( 
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [in] */ boolean bRecord) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE HasReminder( 
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [in] */ boolean bRecord,
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DeleteReminder( 
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [in] */ boolean bRecord) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE HasEnhancement( 
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsCC( 
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITVViewerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITVViewer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITVViewer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITVViewer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetTVMode )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ boolean fTVMode);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsTVMode )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pfTVMode);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsChannelBarUp )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsModalDialogUp )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsLoaderActive )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GlobalStartTime )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pdate);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GlobalEndTime )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ DATE __RPC_FAR *pdate);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewerID )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pidViewer);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WantKeys )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ int nWantKeys);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Tune )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ long lTuningSpace,
            /* [in] */ long lChannelNumber,
            /* [in] */ long lVideoStream,
            /* [in] */ long lAudioStream,
            /* [in] */ BSTR bsIPStream);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentTuningInfo )( 
            ITVViewer __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lTuningSpace,
            /* [out] */ long __RPC_FAR *lChannelNumber,
            /* [out] */ long __RPC_FAR *lVideoStream,
            /* [out] */ long __RPC_FAR *lAudioStream,
            /* [out] */ BSTR __RPC_FAR *pbsIPAddress);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPreviousTuningInfo )( 
            ITVViewer __RPC_FAR * This,
            /* [out] */ long __RPC_FAR *lTuningSpace,
            /* [out] */ long __RPC_FAR *lChannelNumber,
            /* [out] */ long __RPC_FAR *lVideoStream,
            /* [out] */ long __RPC_FAR *lAudioStream,
            /* [out] */ BSTR __RPC_FAR *pbsIPAddress);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetReminder )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [in] */ boolean bRecord);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HasReminder )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [in] */ boolean bRecord,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteReminder )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [in] */ boolean bRecord);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HasEnhancement )( 
            ITVViewer __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pEpisode,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsCC )( 
            ITVViewer __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        END_INTERFACE
    } ITVViewerVtbl;

    interface ITVViewer
    {
        CONST_VTBL struct ITVViewerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITVViewer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITVViewer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITVViewer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITVViewer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITVViewer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITVViewer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITVViewer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITVViewer_SetTVMode(This,fTVMode)	\
    (This)->lpVtbl -> SetTVMode(This,fTVMode)

#define ITVViewer_IsTVMode(This,pfTVMode)	\
    (This)->lpVtbl -> IsTVMode(This,pfTVMode)

#define ITVViewer_IsChannelBarUp(This,pf)	\
    (This)->lpVtbl -> IsChannelBarUp(This,pf)

#define ITVViewer_IsModalDialogUp(This,pf)	\
    (This)->lpVtbl -> IsModalDialogUp(This,pf)

#define ITVViewer_IsLoaderActive(This,pf)	\
    (This)->lpVtbl -> IsLoaderActive(This,pf)

#define ITVViewer_GlobalStartTime(This,pdate)	\
    (This)->lpVtbl -> GlobalStartTime(This,pdate)

#define ITVViewer_GlobalEndTime(This,pdate)	\
    (This)->lpVtbl -> GlobalEndTime(This,pdate)

#define ITVViewer_ViewerID(This,pidViewer)	\
    (This)->lpVtbl -> ViewerID(This,pidViewer)

#define ITVViewer_WantKeys(This,nWantKeys)	\
    (This)->lpVtbl -> WantKeys(This,nWantKeys)

#define ITVViewer_Tune(This,lTuningSpace,lChannelNumber,lVideoStream,lAudioStream,bsIPStream)	\
    (This)->lpVtbl -> Tune(This,lTuningSpace,lChannelNumber,lVideoStream,lAudioStream,bsIPStream)

#define ITVViewer_GetCurrentTuningInfo(This,lTuningSpace,lChannelNumber,lVideoStream,lAudioStream,pbsIPAddress)	\
    (This)->lpVtbl -> GetCurrentTuningInfo(This,lTuningSpace,lChannelNumber,lVideoStream,lAudioStream,pbsIPAddress)

#define ITVViewer_GetPreviousTuningInfo(This,lTuningSpace,lChannelNumber,lVideoStream,lAudioStream,pbsIPAddress)	\
    (This)->lpVtbl -> GetPreviousTuningInfo(This,lTuningSpace,lChannelNumber,lVideoStream,lAudioStream,pbsIPAddress)

#define ITVViewer_SetReminder(This,pEpisode,bRecord)	\
    (This)->lpVtbl -> SetReminder(This,pEpisode,bRecord)

#define ITVViewer_HasReminder(This,pEpisode,bRecord,pf)	\
    (This)->lpVtbl -> HasReminder(This,pEpisode,bRecord,pf)

#define ITVViewer_DeleteReminder(This,pEpisode,bRecord)	\
    (This)->lpVtbl -> DeleteReminder(This,pEpisode,bRecord)

#define ITVViewer_HasEnhancement(This,pEpisode,pf)	\
    (This)->lpVtbl -> HasEnhancement(This,pEpisode,pf)

#define ITVViewer_IsCC(This,pf)	\
    (This)->lpVtbl -> IsCC(This,pf)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_SetTVMode_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ boolean fTVMode);


void __RPC_STUB ITVViewer_SetTVMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_IsTVMode_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pfTVMode);


void __RPC_STUB ITVViewer_IsTVMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_IsChannelBarUp_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVViewer_IsChannelBarUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_IsModalDialogUp_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVViewer_IsModalDialogUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_IsLoaderActive_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVViewer_IsLoaderActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_GlobalStartTime_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pdate);


void __RPC_STUB ITVViewer_GlobalStartTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_GlobalEndTime_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ DATE __RPC_FAR *pdate);


void __RPC_STUB ITVViewer_GlobalEndTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_ViewerID_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pidViewer);


void __RPC_STUB ITVViewer_ViewerID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_WantKeys_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ int nWantKeys);


void __RPC_STUB ITVViewer_WantKeys_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_Tune_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ long lTuningSpace,
    /* [in] */ long lChannelNumber,
    /* [in] */ long lVideoStream,
    /* [in] */ long lAudioStream,
    /* [in] */ BSTR bsIPStream);


void __RPC_STUB ITVViewer_Tune_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_GetCurrentTuningInfo_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *lTuningSpace,
    /* [out] */ long __RPC_FAR *lChannelNumber,
    /* [out] */ long __RPC_FAR *lVideoStream,
    /* [out] */ long __RPC_FAR *lAudioStream,
    /* [out] */ BSTR __RPC_FAR *pbsIPAddress);


void __RPC_STUB ITVViewer_GetCurrentTuningInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_GetPreviousTuningInfo_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [out] */ long __RPC_FAR *lTuningSpace,
    /* [out] */ long __RPC_FAR *lChannelNumber,
    /* [out] */ long __RPC_FAR *lVideoStream,
    /* [out] */ long __RPC_FAR *lAudioStream,
    /* [out] */ BSTR __RPC_FAR *pbsIPAddress);


void __RPC_STUB ITVViewer_GetPreviousTuningInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_SetReminder_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pEpisode,
    /* [in] */ boolean bRecord);


void __RPC_STUB ITVViewer_SetReminder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_HasReminder_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pEpisode,
    /* [in] */ boolean bRecord,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVViewer_HasReminder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_DeleteReminder_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pEpisode,
    /* [in] */ boolean bRecord);


void __RPC_STUB ITVViewer_DeleteReminder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_HasEnhancement_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pEpisode,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVViewer_HasEnhancement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVViewer_IsCC_Proxy( 
    ITVViewer __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVViewer_IsCC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITVViewer_INTERFACE_DEFINED__ */


#ifndef __ITVControl_INTERFACE_DEFINED__
#define __ITVControl_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ITVControl
 * at Tue May 19 19:41:41 1998
 * using MIDL 3.01.75
 ****************************************/
/* [object][helpstring][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_ITVControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("3F8A2EA3-C171-11cf-868C-00805F2C11CE")
    ITVControl : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnIdle( 
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Tune( 
            /* [in] */ long ltsNew,
            /* [in] */ long lcnNew,
            /* [in] */ long lvsNew,
            /* [in] */ long lasNew,
            /* [in] */ BSTR bsIPNew,
            /* [in] */ long ltsPrev,
            /* [in] */ long lcnPrev,
            /* [in] */ long lvsPrev,
            /* [in] */ long lasPrev,
            /* [in] */ BSTR bsIPPrev) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TearDown( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SyncEvent( 
            /* [in] */ int iEvent,
            /* [in] */ BSTR pParm1,
            /* [in] */ BSTR pParm2) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EpisodeStatusChanged( 
            /* [in] */ int iChange,
            /* [in] */ IUnknown __RPC_FAR *pEpi) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PowerChange( 
            /* [in] */ boolean bPowerOn,
            /* [in] */ boolean bUIAllowed) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnTVFocus( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetOutput( 
            /* [in] */ BSTR bsDeviceName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCC( 
            /* [retval][out] */ boolean __RPC_FAR *pf) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetCC( 
            /* [in] */ boolean bCC) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnableVideo( 
            /* [in] */ boolean bEnable,
            /* [in] */ int iReason) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITVControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITVControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITVControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ITVControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnIdle )( 
            ITVControl __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Tune )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ long ltsNew,
            /* [in] */ long lcnNew,
            /* [in] */ long lvsNew,
            /* [in] */ long lasNew,
            /* [in] */ BSTR bsIPNew,
            /* [in] */ long ltsPrev,
            /* [in] */ long lcnPrev,
            /* [in] */ long lvsPrev,
            /* [in] */ long lasPrev,
            /* [in] */ BSTR bsIPPrev);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TearDown )( 
            ITVControl __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SyncEvent )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ int iEvent,
            /* [in] */ BSTR pParm1,
            /* [in] */ BSTR pParm2);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EpisodeStatusChanged )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ int iChange,
            /* [in] */ IUnknown __RPC_FAR *pEpi);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PowerChange )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ boolean bPowerOn,
            /* [in] */ boolean bUIAllowed);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnTVFocus )( 
            ITVControl __RPC_FAR * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetOutput )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ BSTR bsDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCC )( 
            ITVControl __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pf);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetCC )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ boolean bCC);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableVideo )( 
            ITVControl __RPC_FAR * This,
            /* [in] */ boolean bEnable,
            /* [in] */ int iReason);
        
        END_INTERFACE
    } ITVControlVtbl;

    interface ITVControl
    {
        CONST_VTBL struct ITVControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITVControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITVControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITVControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITVControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ITVControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ITVControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ITVControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ITVControl_OnIdle(This,pf)	\
    (This)->lpVtbl -> OnIdle(This,pf)

#define ITVControl_Tune(This,ltsNew,lcnNew,lvsNew,lasNew,bsIPNew,ltsPrev,lcnPrev,lvsPrev,lasPrev,bsIPPrev)	\
    (This)->lpVtbl -> Tune(This,ltsNew,lcnNew,lvsNew,lasNew,bsIPNew,ltsPrev,lcnPrev,lvsPrev,lasPrev,bsIPPrev)

#define ITVControl_TearDown(This)	\
    (This)->lpVtbl -> TearDown(This)

#define ITVControl_SyncEvent(This,iEvent,pParm1,pParm2)	\
    (This)->lpVtbl -> SyncEvent(This,iEvent,pParm1,pParm2)

#define ITVControl_EpisodeStatusChanged(This,iChange,pEpi)	\
    (This)->lpVtbl -> EpisodeStatusChanged(This,iChange,pEpi)

#define ITVControl_PowerChange(This,bPowerOn,bUIAllowed)	\
    (This)->lpVtbl -> PowerChange(This,bPowerOn,bUIAllowed)

#define ITVControl_OnTVFocus(This)	\
    (This)->lpVtbl -> OnTVFocus(This)

#define ITVControl_SetOutput(This,bsDeviceName)	\
    (This)->lpVtbl -> SetOutput(This,bsDeviceName)

#define ITVControl_GetCC(This,pf)	\
    (This)->lpVtbl -> GetCC(This,pf)

#define ITVControl_SetCC(This,bCC)	\
    (This)->lpVtbl -> SetCC(This,bCC)

#define ITVControl_EnableVideo(This,bEnable,iReason)	\
    (This)->lpVtbl -> EnableVideo(This,bEnable,iReason)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_OnIdle_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVControl_OnIdle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_Tune_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ long ltsNew,
    /* [in] */ long lcnNew,
    /* [in] */ long lvsNew,
    /* [in] */ long lasNew,
    /* [in] */ BSTR bsIPNew,
    /* [in] */ long ltsPrev,
    /* [in] */ long lcnPrev,
    /* [in] */ long lvsPrev,
    /* [in] */ long lasPrev,
    /* [in] */ BSTR bsIPPrev);


void __RPC_STUB ITVControl_Tune_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_TearDown_Proxy( 
    ITVControl __RPC_FAR * This);


void __RPC_STUB ITVControl_TearDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_SyncEvent_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ int iEvent,
    /* [in] */ BSTR pParm1,
    /* [in] */ BSTR pParm2);


void __RPC_STUB ITVControl_SyncEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_EpisodeStatusChanged_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ int iChange,
    /* [in] */ IUnknown __RPC_FAR *pEpi);


void __RPC_STUB ITVControl_EpisodeStatusChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_PowerChange_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ boolean bPowerOn,
    /* [in] */ boolean bUIAllowed);


void __RPC_STUB ITVControl_PowerChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_OnTVFocus_Proxy( 
    ITVControl __RPC_FAR * This);


void __RPC_STUB ITVControl_OnTVFocus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_SetOutput_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ BSTR bsDeviceName);


void __RPC_STUB ITVControl_SetOutput_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_GetCC_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pf);


void __RPC_STUB ITVControl_GetCC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_SetCC_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ boolean bCC);


void __RPC_STUB ITVControl_SetCC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ITVControl_EnableVideo_Proxy( 
    ITVControl __RPC_FAR * This,
    /* [in] */ boolean bEnable,
    /* [in] */ int iReason);


void __RPC_STUB ITVControl_EnableVideo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITVControl_INTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_TVViewer;

class DECLSPEC_UUID("5543DD10-B41D-11CF-8682-00805F2C11CE")
TVViewer;
#endif
#endif /* __TVDisp_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
