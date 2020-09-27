/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Nov 29 16:13:05 1999
 */
/* Compiler settings for C:\dx8\dmusic\dmime\imediaobjectparams.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifdef XBOX
#include <xtl.h>
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define _WINGDI_
#else // XBOX
#include <windows.h>
#endif // XBOX

#ifndef _DUMMY_LPMSG
#define _DUMMY_LPMSG
typedef void* LPMSG;
#endif
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __imediaobjectparams_h__
#define __imediaobjectparams_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IMediaParamInfo_FWD_DEFINED__
#define __IMediaParamInfo_FWD_DEFINED__
typedef interface IMediaParamInfo IMediaParamInfo;
#endif 	/* __IMediaParamInfo_FWD_DEFINED__ */


#ifndef __IMediaParams_FWD_DEFINED__
#define __IMediaParams_FWD_DEFINED__
typedef interface IMediaParams IMediaParams;
#endif 	/* __IMediaParams_FWD_DEFINED__ */


#ifndef __IMediaParamsRecordNotify_FWD_DEFINED__
#define __IMediaParamsRecordNotify_FWD_DEFINED__
typedef interface IMediaParamsRecordNotify IMediaParamsRecordNotify;
#endif 	/* __IMediaParamsRecordNotify_FWD_DEFINED__ */


#ifndef __IMediaParamsRecord_FWD_DEFINED__
#define __IMediaParamsRecord_FWD_DEFINED__
typedef interface IMediaParamsRecord IMediaParamsRecord;
#endif 	/* __IMediaParamsRecord_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "strmif.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_imediaobjectparams_0000 */
/* [local] */ 

typedef float MP_DATA;

typedef 
enum _MP_Type
    {	MPT_INT	= 0,
	MPT_FLOAT	= MPT_INT + 1,
	MPT_BOOL	= MPT_FLOAT + 1,
	MPT_ENUM	= MPT_BOOL + 1,
	MPT_MAX	= MPT_ENUM + 1
    }	MP_TYPE;

#define	MPBOOL_TRUE	( 1 )

#define	MPBOOL_FALSE	( 0 )

typedef 
enum _MP_CURVE_TYPE
    {	MP_CURVE_JUMP	= 0x1,
	MP_CURVE_LINEAR	= 0x2,
	MP_CURVE_SQUARE	= 0x4,
	MP_CURVE_INVSQUARE	= 0x8,
	MP_CURVE_SINE	= 0x10,
	MP_CURVE_CURRENTVAL	= 0x8000
    }	MP_CURVE_TYPE;

typedef DWORD MP_CAPS;

#define	MP_CAPS_CURVE_JUMP	( MP_CURVE_JUMP )

#define	MP_CAPS_CURVE_LINEAR	( MP_CURVE_LINEAR )

#define	MP_CAPS_CURVE_SQUARE	( MP_CURVE_SQUARE )

#define	MP_CAPS_CURVE_INVSQUARE	( MP_CURVE_INVSQUARE )

#define	MP_CAPS_CURVE_SINE	( MP_CURVE_SINE )

typedef struct  _MP_PARAMINFO
    {
    MP_TYPE mpType;
    MP_CAPS mopCaps;
    MP_DATA mpdMinValue;
    MP_DATA mpdMaxValue;
    MP_DATA mpdNeutralValue;
    WCHAR szUnitText[ 32 ];
    WCHAR szLabel[ 32 ];
    }	MP_PARAMINFO;

typedef DWORD DWORD;

#define	DWORD_ALLPARAMS	( -1 )

typedef DWORD MP_TIMEDATA;

DEFINE_GUID(GUID_TIME_REFERNCE,
0x93ad712b, 0xdaa0, 0x4ffe, 0xbc, 0x81, 0xb0, 0xce, 0x50, 0xf, 0xcd, 0xd9);
DEFINE_GUID(GUID_TIME_MUSIC,
0x574c49d, 0x5b04, 0x4b15, 0xa5, 0x42, 0xae, 0x28, 0x20, 0x30, 0x11, 0x7b);
DEFINE_GUID(GUID_TIME_SAMPLES,
0xa8593d05, 0xc43, 0x4984, 0x9a, 0x63, 0x97, 0xaf, 0x9e, 0x2, 0xc4, 0xc0);
typedef DWORD MP_FLAGS;

#define	MPF_ENVLP_STANDARD	( 0 )

#define	MPF_ENVLP_BEGIN_CURRENTVAL	( 0x1 )

typedef struct  _MP_ENVELOPE
    {
    REFERENCE_TIME rtStart;
    REFERENCE_TIME rtEnd;
    MP_DATA valStart;
    MP_DATA valEnd;
    MP_CURVE_TYPE iCurve;
    MP_FLAGS flags;
    }	MP_ENVELOPE;

#define	MPF_PUNCHIN_REFTIME	( 0 )

#define	MPF_PUNCHIN_NOW	( 0x1 )



extern RPC_IF_HANDLE __MIDL_itf_imediaobjectparams_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_imediaobjectparams_0000_v0_0_s_ifspec;

#ifndef __IMediaParamInfo_INTERFACE_DEFINED__
#define __IMediaParamInfo_INTERFACE_DEFINED__

/* interface IMediaParamInfo */
/* [version][uuid][object] */ 


EXTERN_C const IID IID_IMediaParamInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6d6cbb60-a223-44aa-842f-a2f06750be6d")
    IMediaParamInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetParamCount( 
            /* [out] */ DWORD __RPC_FAR *pdwParams) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParamInfo( 
            /* [in] */ DWORD dwParamIndex,
            /* [out] */ MP_PARAMINFO __RPC_FAR *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParamText( 
            /* [in] */ DWORD dwParamIndex,
            /* [out] */ WCHAR __RPC_FAR *__RPC_FAR *ppwchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNumTimeFormats( 
            /* [out] */ DWORD __RPC_FAR *pdwNumTimeFormats) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSupportedTimeFormat( 
            /* [in] */ DWORD dwFormatIndex,
            /* [out] */ GUID __RPC_FAR *pguidTimeFormat) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentTimeFormat( 
            /* [out] */ GUID __RPC_FAR *pguidTimeFormat,
            /* [out] */ MP_TIMEDATA __RPC_FAR *pTimeData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaParamInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaParamInfo __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaParamInfo __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParamCount )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwParams);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParamInfo )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [in] */ DWORD dwParamIndex,
            /* [out] */ MP_PARAMINFO __RPC_FAR *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParamText )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [in] */ DWORD dwParamIndex,
            /* [out] */ WCHAR __RPC_FAR *__RPC_FAR *ppwchText);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetNumTimeFormats )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [out] */ DWORD __RPC_FAR *pdwNumTimeFormats);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSupportedTimeFormat )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [in] */ DWORD dwFormatIndex,
            /* [out] */ GUID __RPC_FAR *pguidTimeFormat);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCurrentTimeFormat )( 
            IMediaParamInfo __RPC_FAR * This,
            /* [out] */ GUID __RPC_FAR *pguidTimeFormat,
            /* [out] */ MP_TIMEDATA __RPC_FAR *pTimeData);
        
        END_INTERFACE
    } IMediaParamInfoVtbl;

    interface IMediaParamInfo
    {
        CONST_VTBL struct IMediaParamInfoVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaParamInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaParamInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaParamInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaParamInfo_GetParamCount(This,pdwParams)	\
    (This)->lpVtbl -> GetParamCount(This,pdwParams)

#define IMediaParamInfo_GetParamInfo(This,dwParamIndex,pInfo)	\
    (This)->lpVtbl -> GetParamInfo(This,dwParamIndex,pInfo)

#define IMediaParamInfo_GetParamText(This,dwParamIndex,ppwchText)	\
    (This)->lpVtbl -> GetParamText(This,dwParamIndex,ppwchText)

#define IMediaParamInfo_GetNumTimeFormats(This,pdwNumTimeFormats)	\
    (This)->lpVtbl -> GetNumTimeFormats(This,pdwNumTimeFormats)

#define IMediaParamInfo_GetSupportedTimeFormat(This,dwFormatIndex,pguidTimeFormat)	\
    (This)->lpVtbl -> GetSupportedTimeFormat(This,dwFormatIndex,pguidTimeFormat)

#define IMediaParamInfo_GetCurrentTimeFormat(This,pguidTimeFormat,pTimeData)	\
    (This)->lpVtbl -> GetCurrentTimeFormat(This,pguidTimeFormat,pTimeData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaParamInfo_GetParamCount_Proxy( 
    IMediaParamInfo __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwParams);


void __RPC_STUB IMediaParamInfo_GetParamCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamInfo_GetParamsInfo_Proxy( 
    IMediaParamInfo __RPC_FAR * This,
    /* [in] */ DWORD dwParamIndex,
    /* [out] */ MP_PARAMINFO __RPC_FAR *pInfo);


void __RPC_STUB IMediaParamInfo_GetParamsInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamInfo_GetParamText_Proxy( 
    IMediaParamInfo __RPC_FAR * This,
    /* [in] */ DWORD dwParamIndex,
    /* [out] */ WCHAR __RPC_FAR *__RPC_FAR *ppwchText);


void __RPC_STUB IMediaParamInfo_GetParamText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamInfo_GetNumTimeFormats_Proxy( 
    IMediaParamInfo __RPC_FAR * This,
    /* [out] */ DWORD __RPC_FAR *pdwNumTimeFormats);


void __RPC_STUB IMediaParamInfo_GetNumTimeFormats_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamInfo_GetSupportedTimeFormat_Proxy( 
    IMediaParamInfo __RPC_FAR * This,
    /* [in] */ DWORD dwFormatIndex,
    /* [out] */ GUID __RPC_FAR *pguidTimeFormat);


void __RPC_STUB IMediaParamInfo_GetSupportedTimeFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamInfo_GetCurrentTimeFormat_Proxy( 
    IMediaParamInfo __RPC_FAR * This,
    /* [out] */ GUID __RPC_FAR *pguidTimeFormat,
    /* [out] */ MP_TIMEDATA __RPC_FAR *pTimeData);


void __RPC_STUB IMediaParamInfo_GetCurrentTimeFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaParamInfo_INTERFACE_DEFINED__ */


#ifndef __IMediaParams_INTERFACE_DEFINED__
#define __IMediaParams_INTERFACE_DEFINED__

/* interface IMediaParams */
/* [version][uuid][object] */ 


EXTERN_C const IID IID_IMediaParams;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6d6cbb61-a223-44aa-842f-a2f06750be6e")
    IMediaParams : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetParam( 
            /* [in] */ DWORD dwParamIndex,
            /* [out] */ MP_DATA __RPC_FAR *pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParam( 
            /* [in] */ DWORD dwParamIndex,
            /* [in] */ MP_DATA value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddEnvelope( 
            /* [in] */ DWORD dwParamIndex,
            /* [in] */ DWORD cPoints,
            /* [in] */ MP_ENVELOPE __RPC_FAR *pEnvelope,
            /* [out] */ DWORD __RPC_FAR *pdwIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FlushEnvelope( 
            /* [in] */ DWORD dwParamIndex,
            /* [in] */ REFERENCE_TIME refTimeStart,
            /* [in] */ REFERENCE_TIME refTimeEnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTimeFormat( 
            /* [in] */ GUID guidTimeFormat,
            /* [in] */ MP_TIMEDATA mpTimeData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaParamsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaParams __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaParams __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaParams __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetParam )( 
            IMediaParams __RPC_FAR * This,
            /* [in] */ DWORD dwParamIndex,
            /* [out] */ MP_DATA __RPC_FAR *pValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetParam )( 
            IMediaParams __RPC_FAR * This,
            /* [in] */ DWORD dwParamIndex,
            /* [in] */ MP_DATA value);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddEnvelope )( 
            IMediaParams __RPC_FAR * This,
            /* [in] */ DWORD dwParamIndex,
            /* [in] */ DWORD cPoints,
            /* [in] */ MP_ENVELOPE __RPC_FAR *pEnvelope,
            /* [out] */ DWORD __RPC_FAR *pdwIndex);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FlushEnvelope )( 
            IMediaParams __RPC_FAR * This,
            /* [in] */ DWORD dwParamIndex,
            /* [in] */ REFERENCE_TIME refTimeStart,
            /* [in] */ REFERENCE_TIME refTimeEnd);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetTimeFormat )( 
            IMediaParams __RPC_FAR * This,
            /* [in] */ GUID guidTimeFormat,
            /* [in] */ MP_TIMEDATA mpTimeData);
        
        END_INTERFACE
    } IMediaParamsVtbl;

    interface IMediaParams
    {
        CONST_VTBL struct IMediaParamsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaParams_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaParams_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaParams_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaParams_GetParam(This,dwParamIndex,pValue)	\
    (This)->lpVtbl -> GetParam(This,dwParamIndex,pValue)

#define IMediaParams_SetParam(This,dwParamIndex,value)	\
    (This)->lpVtbl -> SetParam(This,dwParamIndex,value)

#define IMediaParams_AddEnvelope(This,dwParamIndex,cPoints,pEnvelope,pdwIndex)	\
    (This)->lpVtbl -> AddEnvelope(This,dwParamIndex,cPoints,pEnvelope,pdwIndex)

#define IMediaParams_FlushEnvelope(This,dwParamIndex,refTimeStart,refTimeEnd)	\
    (This)->lpVtbl -> FlushEnvelope(This,dwParamIndex,refTimeStart,refTimeEnd)

#define IMediaParams_SetTimeFormat(This,guidTimeFormat,mpTimeData)	\
    (This)->lpVtbl -> SetTimeFormat(This,guidTimeFormat,mpTimeData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaParams_GetParam_Proxy( 
    IMediaParams __RPC_FAR * This,
    /* [in] */ DWORD dwParamIndex,
    /* [out] */ MP_DATA __RPC_FAR *pValue);


void __RPC_STUB IMediaParams_GetParam_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParams_SetParam_Proxy( 
    IMediaParams __RPC_FAR * This,
    /* [in] */ DWORD dwParamIndex,
    /* [in] */ MP_DATA value);


void __RPC_STUB IMediaParams_SetParam_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParams_AddEnvelope_Proxy( 
    IMediaParams __RPC_FAR * This,
    /* [in] */ DWORD dwParamIndex,
    /* [in] */ DWORD cPoints,
    /* [in] */ MP_ENVELOPE __RPC_FAR *pEnvelope,
    /* [out] */ DWORD __RPC_FAR *pdwIndex);


void __RPC_STUB IMediaParams_AddEnvelope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParams_FlushEnvelope_Proxy( 
    IMediaParams __RPC_FAR * This,
    /* [in] */ DWORD dwParamIndex,
    /* [in] */ REFERENCE_TIME refTimeStart,
    /* [in] */ REFERENCE_TIME refTimeEnd);


void __RPC_STUB IMediaParams_FlushEnvelope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParams_SetTimeFormat_Proxy( 
    IMediaParams __RPC_FAR * This,
    /* [in] */ GUID guidTimeFormat,
    /* [in] */ MP_TIMEDATA mpTimeData);


void __RPC_STUB IMediaParams_SetTimeFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaParams_INTERFACE_DEFINED__ */


#ifndef __IMediaParamsRecordNotify_INTERFACE_DEFINED__
#define __IMediaParamsRecordNotify_INTERFACE_DEFINED__

/* interface IMediaParamsRecordNotify */
/* [version][uuid][object] */ 


EXTERN_C const IID IID_IMediaParamsRecordNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fea74878-4e39-4267-8a17-6aaf0536ff7c")
    IMediaParamsRecordNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitRecording( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PunchedIn( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PunchedOut( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndRecording( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParamChanged( 
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD cNumPoints,
            /* [in] */ MP_ENVELOPE __RPC_FAR *pEnvelope) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaParamsRecordNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaParamsRecordNotify __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaParamsRecordNotify __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaParamsRecordNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitRecording )( 
            IMediaParamsRecordNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PunchedIn )( 
            IMediaParamsRecordNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PunchedOut )( 
            IMediaParamsRecordNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndRecording )( 
            IMediaParamsRecordNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ParamChanged )( 
            IMediaParamsRecordNotify __RPC_FAR * This,
            /* [in] */ DWORD dwIndex,
            /* [in] */ DWORD cNumPoints,
            /* [in] */ MP_ENVELOPE __RPC_FAR *pEnvelope);
        
        END_INTERFACE
    } IMediaParamsRecordNotifyVtbl;

    interface IMediaParamsRecordNotify
    {
        CONST_VTBL struct IMediaParamsRecordNotifyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaParamsRecordNotify_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaParamsRecordNotify_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaParamsRecordNotify_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaParamsRecordNotify_InitRecording(This)	\
    (This)->lpVtbl -> InitRecording(This)

#define IMediaParamsRecordNotify_PunchedIn(This)	\
    (This)->lpVtbl -> PunchedIn(This)

#define IMediaParamsRecordNotify_PunchedOut(This)	\
    (This)->lpVtbl -> PunchedOut(This)

#define IMediaParamsRecordNotify_EndRecording(This)	\
    (This)->lpVtbl -> EndRecording(This)

#define IMediaParamsRecordNotify_ParamChanged(This,dwIndex,cNumPoints,pEnvelope)	\
    (This)->lpVtbl -> ParamChanged(This,dwIndex,cNumPoints,pEnvelope)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaParamsRecordNotify_InitRecording_Proxy( 
    IMediaParamsRecordNotify __RPC_FAR * This);


void __RPC_STUB IMediaParamsRecordNotify_InitRecording_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecordNotify_PunchedIn_Proxy( 
    IMediaParamsRecordNotify __RPC_FAR * This);


void __RPC_STUB IMediaParamsRecordNotify_PunchedIn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecordNotify_PunchedOut_Proxy( 
    IMediaParamsRecordNotify __RPC_FAR * This);


void __RPC_STUB IMediaParamsRecordNotify_PunchedOut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecordNotify_EndRecording_Proxy( 
    IMediaParamsRecordNotify __RPC_FAR * This);


void __RPC_STUB IMediaParamsRecordNotify_EndRecording_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecordNotify_ParamChanged_Proxy( 
    IMediaParamsRecordNotify __RPC_FAR * This,
    /* [in] */ DWORD dwIndex,
    /* [in] */ DWORD cNumPoints,
    /* [in] */ MP_ENVELOPE __RPC_FAR *pEnvelope);


void __RPC_STUB IMediaParamsRecordNotify_ParamChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaParamsRecordNotify_INTERFACE_DEFINED__ */


#ifndef __IMediaParamsRecord_INTERFACE_DEFINED__
#define __IMediaParamsRecord_INTERFACE_DEFINED__

/* interface IMediaParamsRecord */
/* [version][uuid][object] */ 


EXTERN_C const IID IID_IMediaParamsRecord;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("21b64d1a-8e24-40f6-8797-44cc021b2a0a")
    IMediaParamsRecord : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitRecording( 
            /* [in] */ IMediaParamsRecordNotify __RPC_FAR *pINotify) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PunchIn( 
            /* [in] */ REFERENCE_TIME refTimeStart,
            /* [in] */ MP_FLAGS flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PunchOut( 
            /* [in] */ REFERENCE_TIME refTimeStopped) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndRecording( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMediaParamsRecordVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IMediaParamsRecord __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IMediaParamsRecord __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IMediaParamsRecord __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InitRecording )( 
            IMediaParamsRecord __RPC_FAR * This,
            /* [in] */ IMediaParamsRecordNotify __RPC_FAR *pINotify);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PunchIn )( 
            IMediaParamsRecord __RPC_FAR * This,
            /* [in] */ REFERENCE_TIME refTimeStart,
            /* [in] */ MP_FLAGS flags);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PunchOut )( 
            IMediaParamsRecord __RPC_FAR * This,
            /* [in] */ REFERENCE_TIME refTimeStopped);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EndRecording )( 
            IMediaParamsRecord __RPC_FAR * This);
        
        END_INTERFACE
    } IMediaParamsRecordVtbl;

    interface IMediaParamsRecord
    {
        CONST_VTBL struct IMediaParamsRecordVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMediaParamsRecord_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMediaParamsRecord_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMediaParamsRecord_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMediaParamsRecord_InitRecording(This,pINotify)	\
    (This)->lpVtbl -> InitRecording(This,pINotify)

#define IMediaParamsRecord_PunchIn(This,refTimeStart,flags)	\
    (This)->lpVtbl -> PunchIn(This,refTimeStart,flags)

#define IMediaParamsRecord_PunchOut(This,refTimeStopped)	\
    (This)->lpVtbl -> PunchOut(This,refTimeStopped)

#define IMediaParamsRecord_EndRecording(This)	\
    (This)->lpVtbl -> EndRecording(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMediaParamsRecord_InitRecording_Proxy( 
    IMediaParamsRecord __RPC_FAR * This,
    /* [in] */ IMediaParamsRecordNotify __RPC_FAR *pINotify);


void __RPC_STUB IMediaParamsRecord_InitRecording_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecord_PunchIn_Proxy( 
    IMediaParamsRecord __RPC_FAR * This,
    /* [in] */ REFERENCE_TIME refTimeStart,
    /* [in] */ MP_FLAGS flags);


void __RPC_STUB IMediaParamsRecord_PunchIn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecord_PunchOut_Proxy( 
    IMediaParamsRecord __RPC_FAR * This,
    /* [in] */ REFERENCE_TIME refTimeStopped);


void __RPC_STUB IMediaParamsRecord_PunchOut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMediaParamsRecord_EndRecording_Proxy( 
    IMediaParamsRecord __RPC_FAR * This);


void __RPC_STUB IMediaParamsRecord_EndRecording_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMediaParamsRecord_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
