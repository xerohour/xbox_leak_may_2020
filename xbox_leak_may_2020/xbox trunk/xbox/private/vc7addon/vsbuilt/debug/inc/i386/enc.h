
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sat Jan 05 01:23:50 2002
 */
/* Compiler settings for enc.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data , no_format_optimization
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

#ifndef __enc_h__
#define __enc_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDebugENC_FWD_DEFINED__
#define __IDebugENC_FWD_DEFINED__
typedef interface IDebugENC IDebugENC;
#endif 	/* __IDebugENC_FWD_DEFINED__ */


#ifndef __IDebugENCLineMap_FWD_DEFINED__
#define __IDebugENCLineMap_FWD_DEFINED__
typedef interface IDebugENCLineMap IDebugENCLineMap;
#endif 	/* __IDebugENCLineMap_FWD_DEFINED__ */


#ifndef __IDebugENCInfo2_FWD_DEFINED__
#define __IDebugENCInfo2_FWD_DEFINED__
typedef interface IDebugENCInfo2 IDebugENCInfo2;
#endif 	/* __IDebugENCInfo2_FWD_DEFINED__ */


#ifndef __IEnumDebugENCInfo2_FWD_DEFINED__
#define __IEnumDebugENCInfo2_FWD_DEFINED__
typedef interface IEnumDebugENCInfo2 IEnumDebugENCInfo2;
#endif 	/* __IEnumDebugENCInfo2_FWD_DEFINED__ */


#ifndef __IDebugENCRelinkInfo2_FWD_DEFINED__
#define __IDebugENCRelinkInfo2_FWD_DEFINED__
typedef interface IDebugENCRelinkInfo2 IDebugENCRelinkInfo2;
#endif 	/* __IDebugENCRelinkInfo2_FWD_DEFINED__ */


#ifndef __IEnumDebugENCRelinkInfo2_FWD_DEFINED__
#define __IEnumDebugENCRelinkInfo2_FWD_DEFINED__
typedef interface IEnumDebugENCRelinkInfo2 IEnumDebugENCRelinkInfo2;
#endif 	/* __IEnumDebugENCRelinkInfo2_FWD_DEFINED__ */


#ifndef __IDebugIDBInfo2_FWD_DEFINED__
#define __IDebugIDBInfo2_FWD_DEFINED__
typedef interface IDebugIDBInfo2 IDebugIDBInfo2;
#endif 	/* __IDebugIDBInfo2_FWD_DEFINED__ */


#ifndef __IEnumDebugIDBInfo2_FWD_DEFINED__
#define __IEnumDebugIDBInfo2_FWD_DEFINED__
typedef interface IEnumDebugIDBInfo2 IEnumDebugIDBInfo2;
#endif 	/* __IEnumDebugIDBInfo2_FWD_DEFINED__ */


#ifndef __IDebugENCBuildInfo2_FWD_DEFINED__
#define __IDebugENCBuildInfo2_FWD_DEFINED__
typedef interface IDebugENCBuildInfo2 IDebugENCBuildInfo2;
#endif 	/* __IDebugENCBuildInfo2_FWD_DEFINED__ */


#ifndef __IDebugENCUpdateOnRelinkEvent2_FWD_DEFINED__
#define __IDebugENCUpdateOnRelinkEvent2_FWD_DEFINED__
typedef interface IDebugENCUpdateOnRelinkEvent2 IDebugENCUpdateOnRelinkEvent2;
#endif 	/* __IDebugENCUpdateOnRelinkEvent2_FWD_DEFINED__ */


#ifndef __IDebugENCUpdateOnStaleCodeEvent2_FWD_DEFINED__
#define __IDebugENCUpdateOnStaleCodeEvent2_FWD_DEFINED__
typedef interface IDebugENCUpdateOnStaleCodeEvent2 IDebugENCUpdateOnStaleCodeEvent2;
#endif 	/* __IDebugENCUpdateOnStaleCodeEvent2_FWD_DEFINED__ */


#ifndef __IDebugENCUpdate_FWD_DEFINED__
#define __IDebugENCUpdate_FWD_DEFINED__
typedef interface IDebugENCUpdate IDebugENCUpdate;
#endif 	/* __IDebugENCUpdate_FWD_DEFINED__ */


#ifndef __IDebugENCSnapshot2_FWD_DEFINED__
#define __IDebugENCSnapshot2_FWD_DEFINED__
typedef interface IDebugENCSnapshot2 IDebugENCSnapshot2;
#endif 	/* __IDebugENCSnapshot2_FWD_DEFINED__ */


#ifndef __IEnumDebugENCSnapshots2_FWD_DEFINED__
#define __IEnumDebugENCSnapshots2_FWD_DEFINED__
typedef interface IEnumDebugENCSnapshots2 IEnumDebugENCSnapshots2;
#endif 	/* __IEnumDebugENCSnapshots2_FWD_DEFINED__ */


#ifndef __IEnumDebugErrorInfos2_FWD_DEFINED__
#define __IEnumDebugErrorInfos2_FWD_DEFINED__
typedef interface IEnumDebugErrorInfos2 IEnumDebugErrorInfos2;
#endif 	/* __IEnumDebugErrorInfos2_FWD_DEFINED__ */


#ifndef __IDebugComPlusSnapshot2_FWD_DEFINED__
#define __IDebugComPlusSnapshot2_FWD_DEFINED__
typedef interface IDebugComPlusSnapshot2 IDebugComPlusSnapshot2;
#endif 	/* __IDebugComPlusSnapshot2_FWD_DEFINED__ */


#ifndef __IDebugNativeSnapshot2_FWD_DEFINED__
#define __IDebugNativeSnapshot2_FWD_DEFINED__
typedef interface IDebugNativeSnapshot2 IDebugNativeSnapshot2;
#endif 	/* __IDebugNativeSnapshot2_FWD_DEFINED__ */


#ifndef __IDebugENCStackFrame2_FWD_DEFINED__
#define __IDebugENCStackFrame2_FWD_DEFINED__
typedef interface IDebugENCStackFrame2 IDebugENCStackFrame2;
#endif 	/* __IDebugENCStackFrame2_FWD_DEFINED__ */


#ifndef __IDebugMetaDataEmit2_FWD_DEFINED__
#define __IDebugMetaDataEmit2_FWD_DEFINED__
typedef interface IDebugMetaDataEmit2 IDebugMetaDataEmit2;
#endif 	/* __IDebugMetaDataEmit2_FWD_DEFINED__ */


#ifndef __IDebugMetaDataDebugEmit2_FWD_DEFINED__
#define __IDebugMetaDataDebugEmit2_FWD_DEFINED__
typedef interface IDebugMetaDataDebugEmit2 IDebugMetaDataDebugEmit2;
#endif 	/* __IDebugMetaDataDebugEmit2_FWD_DEFINED__ */


#ifndef __IDebugENCStateEvents_FWD_DEFINED__
#define __IDebugENCStateEvents_FWD_DEFINED__
typedef interface IDebugENCStateEvents IDebugENCStateEvents;
#endif 	/* __IDebugENCStateEvents_FWD_DEFINED__ */


#ifndef __EncMgr_FWD_DEFINED__
#define __EncMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class EncMgr EncMgr;
#else
typedef struct EncMgr EncMgr;
#endif /* __cplusplus */

#endif 	/* __EncMgr_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"
#include "msdbg.h"
#include "sh.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_enc_0000 */
/* [local] */ 





















static const int E_ENC_REBUILD_FAIL = MAKE_HRESULT(1, FACILITY_ITF, 0x0001);
static const int E_VB_ENC_REBUILD_FAIL = MAKE_HRESULT(1, FACILITY_ITF, 0x0011);
static const int E_ENC_REBUILD_FAIL_MODULE_NOT_LOADED = MAKE_HRESULT(1, FACILITY_ITF, 0x0101);
static const int E_ENC_COMMIT_FAIL = MAKE_HRESULT(1, FACILITY_ITF, 0x0002);

enum tagENCSTATE
    {	ENCSTATE_DISABLED	= 0,
	ENCSTATE_ENABLED	= ENCSTATE_DISABLED + 1,
	ENCSTATE_MANAGED_ENC_NOT_SUPPORTED	= ENCSTATE_ENABLED + 1
    } ;
typedef enum tagENCSTATE ENCSTATE;


enum tagApplyCodeChangesResult
    {	ACCR_SUCCESS	= 0,
	ACCR_BUILDERROR	= ACCR_SUCCESS + 1,
	ACCR_CANCOMMITERROR	= ACCR_BUILDERROR + 1,
	ACCR_COMMITERROR	= ACCR_CANCOMMITERROR + 1
    } ;
typedef enum tagApplyCodeChangesResult ApplyCodeChangesResult;



extern RPC_IF_HANDLE __MIDL_itf_enc_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_enc_0000_v0_0_s_ifspec;

#ifndef __IDebugENC_INTERFACE_DEFINED__
#define __IDebugENC_INTERFACE_DEFINED__

/* interface IDebugENC */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENC;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B104D8B7-AF19-11d2-922C-00A02448799A")
    IDebugENC : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetRegistryRoot( 
            /* [in] */ LPCOLESTR in_szRegistryRoot) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnterDebuggingSession( 
            /* [in] */ IServiceProvider *in_pServiceProvider) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetENCProjectBuildOption( 
            /* [in] */ REFGUID in_guidOption,
            /* [in] */ LPCOLESTR in_szOptionValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InquireENCState( 
            /* [in] */ ENCSTATE *in_pENCSTATE,
            /* [in] */ BOOL fOnContinue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InquireENCRelinkState( 
            /* [in] */ BOOL *in_pbENCRelinking) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapToEdited( 
            /* [in] */ LPCOLESTR in_szFile,
            /* [in] */ ULONG in_LineNo,
            /* [in] */ ULONG in_ColumnNo,
            /* [out] */ ULONG *out_pLineNo,
            /* [out] */ ULONG *out_pColumnNo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapToSuperceded( 
            /* [in] */ LPCOLESTR in_szFile,
            /* [in] */ ULONG in_LineNo,
            /* [in] */ ULONG in_ColumnNo,
            /* [out] */ ULONG *out_pLineNo,
            /* [out] */ ULONG *out_pColumnNo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyCodeChanges( 
            /* [in] */ IDebugSession2 *in_pSession,
            /* [in] */ BOOL in_fOnContinue,
            /* [out] */ ApplyCodeChangesResult *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelApplyCodeChanges( 
            /* [in] */ IDebugProgram2 *in_pProgram) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LeaveDebuggingSession( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AdviseENCStateEvents( 
            /* [in] */ IDebugENCStateEvents *in_pENCStateEvents) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnadviseENCStateEvents( 
            /* [in] */ IDebugENCStateEvents *in_pENCStateEvents) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileName( 
            /* [in] */ LPCOLESTR in_szURL,
            /* [out] */ BSTR *out_pbstrFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFileDisplayName( 
            /* [in] */ LPCOLESTR in_szURL,
            /* [out] */ BSTR *out_pbstrDisplayFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearENCState( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENC * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENC * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENC * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetRegistryRoot )( 
            IDebugENC * This,
            /* [in] */ LPCOLESTR in_szRegistryRoot);
        
        HRESULT ( STDMETHODCALLTYPE *EnterDebuggingSession )( 
            IDebugENC * This,
            /* [in] */ IServiceProvider *in_pServiceProvider);
        
        HRESULT ( STDMETHODCALLTYPE *SetENCProjectBuildOption )( 
            IDebugENC * This,
            /* [in] */ REFGUID in_guidOption,
            /* [in] */ LPCOLESTR in_szOptionValue);
        
        HRESULT ( STDMETHODCALLTYPE *InquireENCState )( 
            IDebugENC * This,
            /* [in] */ ENCSTATE *in_pENCSTATE,
            /* [in] */ BOOL fOnContinue);
        
        HRESULT ( STDMETHODCALLTYPE *InquireENCRelinkState )( 
            IDebugENC * This,
            /* [in] */ BOOL *in_pbENCRelinking);
        
        HRESULT ( STDMETHODCALLTYPE *MapToEdited )( 
            IDebugENC * This,
            /* [in] */ LPCOLESTR in_szFile,
            /* [in] */ ULONG in_LineNo,
            /* [in] */ ULONG in_ColumnNo,
            /* [out] */ ULONG *out_pLineNo,
            /* [out] */ ULONG *out_pColumnNo);
        
        HRESULT ( STDMETHODCALLTYPE *MapToSuperceded )( 
            IDebugENC * This,
            /* [in] */ LPCOLESTR in_szFile,
            /* [in] */ ULONG in_LineNo,
            /* [in] */ ULONG in_ColumnNo,
            /* [out] */ ULONG *out_pLineNo,
            /* [out] */ ULONG *out_pColumnNo);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyCodeChanges )( 
            IDebugENC * This,
            /* [in] */ IDebugSession2 *in_pSession,
            /* [in] */ BOOL in_fOnContinue,
            /* [out] */ ApplyCodeChangesResult *result);
        
        HRESULT ( STDMETHODCALLTYPE *CancelApplyCodeChanges )( 
            IDebugENC * This,
            /* [in] */ IDebugProgram2 *in_pProgram);
        
        HRESULT ( STDMETHODCALLTYPE *LeaveDebuggingSession )( 
            IDebugENC * This);
        
        HRESULT ( STDMETHODCALLTYPE *AdviseENCStateEvents )( 
            IDebugENC * This,
            /* [in] */ IDebugENCStateEvents *in_pENCStateEvents);
        
        HRESULT ( STDMETHODCALLTYPE *UnadviseENCStateEvents )( 
            IDebugENC * This,
            /* [in] */ IDebugENCStateEvents *in_pENCStateEvents);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileName )( 
            IDebugENC * This,
            /* [in] */ LPCOLESTR in_szURL,
            /* [out] */ BSTR *out_pbstrFileName);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileDisplayName )( 
            IDebugENC * This,
            /* [in] */ LPCOLESTR in_szURL,
            /* [out] */ BSTR *out_pbstrDisplayFileName);
        
        HRESULT ( STDMETHODCALLTYPE *ClearENCState )( 
            IDebugENC * This);
        
        END_INTERFACE
    } IDebugENCVtbl;

    interface IDebugENC
    {
        CONST_VTBL struct IDebugENCVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENC_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENC_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENC_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENC_SetRegistryRoot(This,in_szRegistryRoot)	\
    (This)->lpVtbl -> SetRegistryRoot(This,in_szRegistryRoot)

#define IDebugENC_EnterDebuggingSession(This,in_pServiceProvider)	\
    (This)->lpVtbl -> EnterDebuggingSession(This,in_pServiceProvider)

#define IDebugENC_SetENCProjectBuildOption(This,in_guidOption,in_szOptionValue)	\
    (This)->lpVtbl -> SetENCProjectBuildOption(This,in_guidOption,in_szOptionValue)

#define IDebugENC_InquireENCState(This,in_pENCSTATE,fOnContinue)	\
    (This)->lpVtbl -> InquireENCState(This,in_pENCSTATE,fOnContinue)

#define IDebugENC_InquireENCRelinkState(This,in_pbENCRelinking)	\
    (This)->lpVtbl -> InquireENCRelinkState(This,in_pbENCRelinking)

#define IDebugENC_MapToEdited(This,in_szFile,in_LineNo,in_ColumnNo,out_pLineNo,out_pColumnNo)	\
    (This)->lpVtbl -> MapToEdited(This,in_szFile,in_LineNo,in_ColumnNo,out_pLineNo,out_pColumnNo)

#define IDebugENC_MapToSuperceded(This,in_szFile,in_LineNo,in_ColumnNo,out_pLineNo,out_pColumnNo)	\
    (This)->lpVtbl -> MapToSuperceded(This,in_szFile,in_LineNo,in_ColumnNo,out_pLineNo,out_pColumnNo)

#define IDebugENC_ApplyCodeChanges(This,in_pSession,in_fOnContinue,result)	\
    (This)->lpVtbl -> ApplyCodeChanges(This,in_pSession,in_fOnContinue,result)

#define IDebugENC_CancelApplyCodeChanges(This,in_pProgram)	\
    (This)->lpVtbl -> CancelApplyCodeChanges(This,in_pProgram)

#define IDebugENC_LeaveDebuggingSession(This)	\
    (This)->lpVtbl -> LeaveDebuggingSession(This)

#define IDebugENC_AdviseENCStateEvents(This,in_pENCStateEvents)	\
    (This)->lpVtbl -> AdviseENCStateEvents(This,in_pENCStateEvents)

#define IDebugENC_UnadviseENCStateEvents(This,in_pENCStateEvents)	\
    (This)->lpVtbl -> UnadviseENCStateEvents(This,in_pENCStateEvents)

#define IDebugENC_GetFileName(This,in_szURL,out_pbstrFileName)	\
    (This)->lpVtbl -> GetFileName(This,in_szURL,out_pbstrFileName)

#define IDebugENC_GetFileDisplayName(This,in_szURL,out_pbstrDisplayFileName)	\
    (This)->lpVtbl -> GetFileDisplayName(This,in_szURL,out_pbstrDisplayFileName)

#define IDebugENC_ClearENCState(This)	\
    (This)->lpVtbl -> ClearENCState(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENC_SetRegistryRoot_Proxy( 
    IDebugENC * This,
    /* [in] */ LPCOLESTR in_szRegistryRoot);


void __RPC_STUB IDebugENC_SetRegistryRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_EnterDebuggingSession_Proxy( 
    IDebugENC * This,
    /* [in] */ IServiceProvider *in_pServiceProvider);


void __RPC_STUB IDebugENC_EnterDebuggingSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_SetENCProjectBuildOption_Proxy( 
    IDebugENC * This,
    /* [in] */ REFGUID in_guidOption,
    /* [in] */ LPCOLESTR in_szOptionValue);


void __RPC_STUB IDebugENC_SetENCProjectBuildOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_InquireENCState_Proxy( 
    IDebugENC * This,
    /* [in] */ ENCSTATE *in_pENCSTATE,
    /* [in] */ BOOL fOnContinue);


void __RPC_STUB IDebugENC_InquireENCState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_InquireENCRelinkState_Proxy( 
    IDebugENC * This,
    /* [in] */ BOOL *in_pbENCRelinking);


void __RPC_STUB IDebugENC_InquireENCRelinkState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_MapToEdited_Proxy( 
    IDebugENC * This,
    /* [in] */ LPCOLESTR in_szFile,
    /* [in] */ ULONG in_LineNo,
    /* [in] */ ULONG in_ColumnNo,
    /* [out] */ ULONG *out_pLineNo,
    /* [out] */ ULONG *out_pColumnNo);


void __RPC_STUB IDebugENC_MapToEdited_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_MapToSuperceded_Proxy( 
    IDebugENC * This,
    /* [in] */ LPCOLESTR in_szFile,
    /* [in] */ ULONG in_LineNo,
    /* [in] */ ULONG in_ColumnNo,
    /* [out] */ ULONG *out_pLineNo,
    /* [out] */ ULONG *out_pColumnNo);


void __RPC_STUB IDebugENC_MapToSuperceded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_ApplyCodeChanges_Proxy( 
    IDebugENC * This,
    /* [in] */ IDebugSession2 *in_pSession,
    /* [in] */ BOOL in_fOnContinue,
    /* [out] */ ApplyCodeChangesResult *result);


void __RPC_STUB IDebugENC_ApplyCodeChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_CancelApplyCodeChanges_Proxy( 
    IDebugENC * This,
    /* [in] */ IDebugProgram2 *in_pProgram);


void __RPC_STUB IDebugENC_CancelApplyCodeChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_LeaveDebuggingSession_Proxy( 
    IDebugENC * This);


void __RPC_STUB IDebugENC_LeaveDebuggingSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_AdviseENCStateEvents_Proxy( 
    IDebugENC * This,
    /* [in] */ IDebugENCStateEvents *in_pENCStateEvents);


void __RPC_STUB IDebugENC_AdviseENCStateEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_UnadviseENCStateEvents_Proxy( 
    IDebugENC * This,
    /* [in] */ IDebugENCStateEvents *in_pENCStateEvents);


void __RPC_STUB IDebugENC_UnadviseENCStateEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_GetFileName_Proxy( 
    IDebugENC * This,
    /* [in] */ LPCOLESTR in_szURL,
    /* [out] */ BSTR *out_pbstrFileName);


void __RPC_STUB IDebugENC_GetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_GetFileDisplayName_Proxy( 
    IDebugENC * This,
    /* [in] */ LPCOLESTR in_szURL,
    /* [out] */ BSTR *out_pbstrDisplayFileName);


void __RPC_STUB IDebugENC_GetFileDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENC_ClearENCState_Proxy( 
    IDebugENC * This);


void __RPC_STUB IDebugENC_ClearENCState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENC_INTERFACE_DEFINED__ */


#ifndef __IDebugENCLineMap_INTERFACE_DEFINED__
#define __IDebugENCLineMap_INTERFACE_DEFINED__

/* interface IDebugENCLineMap */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCLineMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8706233B-BD4C-11d2-9238-00A02448799A")
    IDebugENCLineMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEditedSource( 
            /* [out] */ BSTR *out_pbstrEditedSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSupercededSource( 
            /* [out] */ BSTR *out_pbstrSupercededSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsLineModified( 
            /* [in] */ ULONG in_LineNoFromSupercededSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LineMap( 
            /* [in] */ ULONG in_LineNoFromSupercededSource,
            /* [out] */ ULONG *out_pLineNoFromEditedSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReverseLineMap( 
            /* [in] */ ULONG in_LineNoFromEditedSource,
            /* [out] */ ULONG *out_pLineNoFromSupercededSource) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCLineMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCLineMap * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCLineMap * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCLineMap * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetEditedSource )( 
            IDebugENCLineMap * This,
            /* [out] */ BSTR *out_pbstrEditedSource);
        
        HRESULT ( STDMETHODCALLTYPE *GetSupercededSource )( 
            IDebugENCLineMap * This,
            /* [out] */ BSTR *out_pbstrSupercededSource);
        
        HRESULT ( STDMETHODCALLTYPE *IsLineModified )( 
            IDebugENCLineMap * This,
            /* [in] */ ULONG in_LineNoFromSupercededSource);
        
        HRESULT ( STDMETHODCALLTYPE *LineMap )( 
            IDebugENCLineMap * This,
            /* [in] */ ULONG in_LineNoFromSupercededSource,
            /* [out] */ ULONG *out_pLineNoFromEditedSource);
        
        HRESULT ( STDMETHODCALLTYPE *ReverseLineMap )( 
            IDebugENCLineMap * This,
            /* [in] */ ULONG in_LineNoFromEditedSource,
            /* [out] */ ULONG *out_pLineNoFromSupercededSource);
        
        END_INTERFACE
    } IDebugENCLineMapVtbl;

    interface IDebugENCLineMap
    {
        CONST_VTBL struct IDebugENCLineMapVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCLineMap_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCLineMap_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCLineMap_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCLineMap_GetEditedSource(This,out_pbstrEditedSource)	\
    (This)->lpVtbl -> GetEditedSource(This,out_pbstrEditedSource)

#define IDebugENCLineMap_GetSupercededSource(This,out_pbstrSupercededSource)	\
    (This)->lpVtbl -> GetSupercededSource(This,out_pbstrSupercededSource)

#define IDebugENCLineMap_IsLineModified(This,in_LineNoFromSupercededSource)	\
    (This)->lpVtbl -> IsLineModified(This,in_LineNoFromSupercededSource)

#define IDebugENCLineMap_LineMap(This,in_LineNoFromSupercededSource,out_pLineNoFromEditedSource)	\
    (This)->lpVtbl -> LineMap(This,in_LineNoFromSupercededSource,out_pLineNoFromEditedSource)

#define IDebugENCLineMap_ReverseLineMap(This,in_LineNoFromEditedSource,out_pLineNoFromSupercededSource)	\
    (This)->lpVtbl -> ReverseLineMap(This,in_LineNoFromEditedSource,out_pLineNoFromSupercededSource)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCLineMap_GetEditedSource_Proxy( 
    IDebugENCLineMap * This,
    /* [out] */ BSTR *out_pbstrEditedSource);


void __RPC_STUB IDebugENCLineMap_GetEditedSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCLineMap_GetSupercededSource_Proxy( 
    IDebugENCLineMap * This,
    /* [out] */ BSTR *out_pbstrSupercededSource);


void __RPC_STUB IDebugENCLineMap_GetSupercededSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCLineMap_IsLineModified_Proxy( 
    IDebugENCLineMap * This,
    /* [in] */ ULONG in_LineNoFromSupercededSource);


void __RPC_STUB IDebugENCLineMap_IsLineModified_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCLineMap_LineMap_Proxy( 
    IDebugENCLineMap * This,
    /* [in] */ ULONG in_LineNoFromSupercededSource,
    /* [out] */ ULONG *out_pLineNoFromEditedSource);


void __RPC_STUB IDebugENCLineMap_LineMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCLineMap_ReverseLineMap_Proxy( 
    IDebugENCLineMap * This,
    /* [in] */ ULONG in_LineNoFromEditedSource,
    /* [out] */ ULONG *out_pLineNoFromSupercededSource);


void __RPC_STUB IDebugENCLineMap_ReverseLineMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCLineMap_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_enc_0373 */
/* [local] */ 


enum __MIDL___MIDL_itf_enc_0373_0001
    {	ENCINFO_STACKFRAME	= 0x1,
	ENCINFO_HRESULT_FROM_DE	= 0x2,
	ENCINFO_ERROR_NO	= 0x4,
	ENCINFO_ERROR_BSTR	= 0x8,
	ENCINFO_CODE_CONTEXT	= 0x10,
	ENCINFO_EXTENDED_INFO	= 0x20
    } ;
typedef DWORD ENCINFO_FLAGS;

typedef struct tagENCINFO
    {
    ENCINFO_FLAGS m_dwValidFields;
    IDebugStackFrame2 *m_pStackFrame;
    HRESULT m_HRFromDE;
    DWORD m_dwErrorNo;
    BSTR m_bstrError;
    IDebugCodeContext2 *m_pCodeContext;
    IUnknown *m_pExtendedInfo;
    } 	ENCINFO;



extern RPC_IF_HANDLE __MIDL_itf_enc_0373_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_enc_0373_v0_0_s_ifspec;

#ifndef __IDebugENCInfo2_INTERFACE_DEFINED__
#define __IDebugENCInfo2_INTERFACE_DEFINED__

/* interface IDebugENCInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6B56106F-BD51-11d2-9238-00A02448799A")
    IDebugENCInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [out] */ ENCINFO *out_pENCINFO) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugENCInfo2 * This,
            /* [out] */ ENCINFO *out_pENCINFO);
        
        END_INTERFACE
    } IDebugENCInfo2Vtbl;

    interface IDebugENCInfo2
    {
        CONST_VTBL struct IDebugENCInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCInfo2_GetInfo(This,out_pENCINFO)	\
    (This)->lpVtbl -> GetInfo(This,out_pENCINFO)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCInfo2_GetInfo_Proxy( 
    IDebugENCInfo2 * This,
    /* [out] */ ENCINFO *out_pENCINFO);


void __RPC_STUB IDebugENCInfo2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugENCInfo2_INTERFACE_DEFINED__
#define __IEnumDebugENCInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugENCInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugENCInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7B076AD1-BD51-11d2-9238-00A02448799A")
    IEnumDebugENCInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG in_NoOfElementsRequested,
            /* [length_is][size_is][out] */ IDebugENCInfo2 **out_ArrayOfpENCInfo,
            /* [out] */ ULONG *out_pNoOfElementsFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG in_NoOfElements) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *out_pCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugENCInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugENCInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugENCInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugENCInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugENCInfo2 * This,
            /* [in] */ ULONG in_NoOfElementsRequested,
            /* [length_is][size_is][out] */ IDebugENCInfo2 **out_ArrayOfpENCInfo,
            /* [out] */ ULONG *out_pNoOfElementsFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugENCInfo2 * This,
            /* [in] */ ULONG in_NoOfElements);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugENCInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugENCInfo2 * This,
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugENCInfo2 * This,
            /* [out] */ ULONG *out_pCount);
        
        END_INTERFACE
    } IEnumDebugENCInfo2Vtbl;

    interface IEnumDebugENCInfo2
    {
        CONST_VTBL struct IEnumDebugENCInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugENCInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugENCInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugENCInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugENCInfo2_Next(This,in_NoOfElementsRequested,out_ArrayOfpENCInfo,out_pNoOfElementsFetched)	\
    (This)->lpVtbl -> Next(This,in_NoOfElementsRequested,out_ArrayOfpENCInfo,out_pNoOfElementsFetched)

#define IEnumDebugENCInfo2_Skip(This,in_NoOfElements)	\
    (This)->lpVtbl -> Skip(This,in_NoOfElements)

#define IEnumDebugENCInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugENCInfo2_Clone(This,out_ppEnumENCInfo)	\
    (This)->lpVtbl -> Clone(This,out_ppEnumENCInfo)

#define IEnumDebugENCInfo2_GetCount(This,out_pCount)	\
    (This)->lpVtbl -> GetCount(This,out_pCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugENCInfo2_Next_Proxy( 
    IEnumDebugENCInfo2 * This,
    /* [in] */ ULONG in_NoOfElementsRequested,
    /* [length_is][size_is][out] */ IDebugENCInfo2 **out_ArrayOfpENCInfo,
    /* [out] */ ULONG *out_pNoOfElementsFetched);


void __RPC_STUB IEnumDebugENCInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCInfo2_Skip_Proxy( 
    IEnumDebugENCInfo2 * This,
    /* [in] */ ULONG in_NoOfElements);


void __RPC_STUB IEnumDebugENCInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCInfo2_Reset_Proxy( 
    IEnumDebugENCInfo2 * This);


void __RPC_STUB IEnumDebugENCInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCInfo2_Clone_Proxy( 
    IEnumDebugENCInfo2 * This,
    /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);


void __RPC_STUB IEnumDebugENCInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCInfo2_GetCount_Proxy( 
    IEnumDebugENCInfo2 * This,
    /* [out] */ ULONG *out_pCount);


void __RPC_STUB IEnumDebugENCInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugENCInfo2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCRelinkInfo2_INTERFACE_DEFINED__
#define __IDebugENCRelinkInfo2_INTERFACE_DEFINED__

/* interface IDebugENCRelinkInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCRelinkInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CBB63A8D-BD57-11d2-9238-00A02448799A")
    IDebugENCRelinkInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [out] */ BSTR *out_pbstrWorkingDir,
            /* [out] */ BSTR *out_pbstrCommand,
            /* [out] */ BSTR *out_pbstrOutFile,
            /* [out] */ BSTR *out_pbstrDebugFile,
            /* [out] */ BOOL *out_pbEditFromLib) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCRelinkInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCRelinkInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCRelinkInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCRelinkInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugENCRelinkInfo2 * This,
            /* [out] */ BSTR *out_pbstrWorkingDir,
            /* [out] */ BSTR *out_pbstrCommand,
            /* [out] */ BSTR *out_pbstrOutFile,
            /* [out] */ BSTR *out_pbstrDebugFile,
            /* [out] */ BOOL *out_pbEditFromLib);
        
        END_INTERFACE
    } IDebugENCRelinkInfo2Vtbl;

    interface IDebugENCRelinkInfo2
    {
        CONST_VTBL struct IDebugENCRelinkInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCRelinkInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCRelinkInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCRelinkInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCRelinkInfo2_GetInfo(This,out_pbstrWorkingDir,out_pbstrCommand,out_pbstrOutFile,out_pbstrDebugFile,out_pbEditFromLib)	\
    (This)->lpVtbl -> GetInfo(This,out_pbstrWorkingDir,out_pbstrCommand,out_pbstrOutFile,out_pbstrDebugFile,out_pbEditFromLib)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCRelinkInfo2_GetInfo_Proxy( 
    IDebugENCRelinkInfo2 * This,
    /* [out] */ BSTR *out_pbstrWorkingDir,
    /* [out] */ BSTR *out_pbstrCommand,
    /* [out] */ BSTR *out_pbstrOutFile,
    /* [out] */ BSTR *out_pbstrDebugFile,
    /* [out] */ BOOL *out_pbEditFromLib);


void __RPC_STUB IDebugENCRelinkInfo2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCRelinkInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugENCRelinkInfo2_INTERFACE_DEFINED__
#define __IEnumDebugENCRelinkInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugENCRelinkInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugENCRelinkInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E51BE743-BD57-11d2-9238-00A02448799A")
    IEnumDebugENCRelinkInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG in_NoOfElementsRequested,
            /* [length_is][size_is][out] */ IDebugENCRelinkInfo2 **out_ArrayOfpENCInfo,
            /* [out] */ ULONG *out_pNoOfElementsFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG in_NoOfElements) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugENCRelinkInfo2 **out_ppEnumENCRelinkInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *out_pCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugENCRelinkInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugENCRelinkInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugENCRelinkInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugENCRelinkInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugENCRelinkInfo2 * This,
            /* [in] */ ULONG in_NoOfElementsRequested,
            /* [length_is][size_is][out] */ IDebugENCRelinkInfo2 **out_ArrayOfpENCInfo,
            /* [out] */ ULONG *out_pNoOfElementsFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugENCRelinkInfo2 * This,
            /* [in] */ ULONG in_NoOfElements);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugENCRelinkInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugENCRelinkInfo2 * This,
            /* [out] */ IEnumDebugENCRelinkInfo2 **out_ppEnumENCRelinkInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugENCRelinkInfo2 * This,
            /* [out] */ ULONG *out_pCount);
        
        END_INTERFACE
    } IEnumDebugENCRelinkInfo2Vtbl;

    interface IEnumDebugENCRelinkInfo2
    {
        CONST_VTBL struct IEnumDebugENCRelinkInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugENCRelinkInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugENCRelinkInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugENCRelinkInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugENCRelinkInfo2_Next(This,in_NoOfElementsRequested,out_ArrayOfpENCInfo,out_pNoOfElementsFetched)	\
    (This)->lpVtbl -> Next(This,in_NoOfElementsRequested,out_ArrayOfpENCInfo,out_pNoOfElementsFetched)

#define IEnumDebugENCRelinkInfo2_Skip(This,in_NoOfElements)	\
    (This)->lpVtbl -> Skip(This,in_NoOfElements)

#define IEnumDebugENCRelinkInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugENCRelinkInfo2_Clone(This,out_ppEnumENCRelinkInfo)	\
    (This)->lpVtbl -> Clone(This,out_ppEnumENCRelinkInfo)

#define IEnumDebugENCRelinkInfo2_GetCount(This,out_pCount)	\
    (This)->lpVtbl -> GetCount(This,out_pCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugENCRelinkInfo2_Next_Proxy( 
    IEnumDebugENCRelinkInfo2 * This,
    /* [in] */ ULONG in_NoOfElementsRequested,
    /* [length_is][size_is][out] */ IDebugENCRelinkInfo2 **out_ArrayOfpENCInfo,
    /* [out] */ ULONG *out_pNoOfElementsFetched);


void __RPC_STUB IEnumDebugENCRelinkInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCRelinkInfo2_Skip_Proxy( 
    IEnumDebugENCRelinkInfo2 * This,
    /* [in] */ ULONG in_NoOfElements);


void __RPC_STUB IEnumDebugENCRelinkInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCRelinkInfo2_Reset_Proxy( 
    IEnumDebugENCRelinkInfo2 * This);


void __RPC_STUB IEnumDebugENCRelinkInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCRelinkInfo2_Clone_Proxy( 
    IEnumDebugENCRelinkInfo2 * This,
    /* [out] */ IEnumDebugENCRelinkInfo2 **out_ppEnumENCRelinkInfo);


void __RPC_STUB IEnumDebugENCRelinkInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCRelinkInfo2_GetCount_Proxy( 
    IEnumDebugENCRelinkInfo2 * This,
    /* [out] */ ULONG *out_pCount);


void __RPC_STUB IEnumDebugENCRelinkInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugENCRelinkInfo2_INTERFACE_DEFINED__ */


#ifndef __IDebugIDBInfo2_INTERFACE_DEFINED__
#define __IDebugIDBInfo2_INTERFACE_DEFINED__

/* interface IDebugIDBInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugIDBInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9B7DE9A9-BD59-11d2-9238-00A02448799A")
    IDebugIDBInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [out] */ BSTR *out_pbstrIDBFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugIDBInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugIDBInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugIDBInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugIDBInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugIDBInfo2 * This,
            /* [out] */ BSTR *out_pbstrIDBFile);
        
        END_INTERFACE
    } IDebugIDBInfo2Vtbl;

    interface IDebugIDBInfo2
    {
        CONST_VTBL struct IDebugIDBInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugIDBInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugIDBInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugIDBInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugIDBInfo2_GetInfo(This,out_pbstrIDBFile)	\
    (This)->lpVtbl -> GetInfo(This,out_pbstrIDBFile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugIDBInfo2_GetInfo_Proxy( 
    IDebugIDBInfo2 * This,
    /* [out] */ BSTR *out_pbstrIDBFile);


void __RPC_STUB IDebugIDBInfo2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugIDBInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugIDBInfo2_INTERFACE_DEFINED__
#define __IEnumDebugIDBInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugIDBInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugIDBInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B34E469B-BD59-11d2-9238-00A02448799A")
    IEnumDebugIDBInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG in_NoOfElementsRequested,
            /* [length_is][size_is][out] */ IDebugIDBInfo2 **out_ArrayOfpIDBInfo2,
            /* [out] */ ULONG *out_pNoOfElementsFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG in_NoOfElements) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugIDBInfo2 **out_ppEnumIDBInfo2) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *out_pCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugIDBInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugIDBInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugIDBInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugIDBInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugIDBInfo2 * This,
            /* [in] */ ULONG in_NoOfElementsRequested,
            /* [length_is][size_is][out] */ IDebugIDBInfo2 **out_ArrayOfpIDBInfo2,
            /* [out] */ ULONG *out_pNoOfElementsFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugIDBInfo2 * This,
            /* [in] */ ULONG in_NoOfElements);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugIDBInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugIDBInfo2 * This,
            /* [out] */ IEnumDebugIDBInfo2 **out_ppEnumIDBInfo2);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugIDBInfo2 * This,
            /* [out] */ ULONG *out_pCount);
        
        END_INTERFACE
    } IEnumDebugIDBInfo2Vtbl;

    interface IEnumDebugIDBInfo2
    {
        CONST_VTBL struct IEnumDebugIDBInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugIDBInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugIDBInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugIDBInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugIDBInfo2_Next(This,in_NoOfElementsRequested,out_ArrayOfpIDBInfo2,out_pNoOfElementsFetched)	\
    (This)->lpVtbl -> Next(This,in_NoOfElementsRequested,out_ArrayOfpIDBInfo2,out_pNoOfElementsFetched)

#define IEnumDebugIDBInfo2_Skip(This,in_NoOfElements)	\
    (This)->lpVtbl -> Skip(This,in_NoOfElements)

#define IEnumDebugIDBInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugIDBInfo2_Clone(This,out_ppEnumIDBInfo2)	\
    (This)->lpVtbl -> Clone(This,out_ppEnumIDBInfo2)

#define IEnumDebugIDBInfo2_GetCount(This,out_pCount)	\
    (This)->lpVtbl -> GetCount(This,out_pCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugIDBInfo2_Next_Proxy( 
    IEnumDebugIDBInfo2 * This,
    /* [in] */ ULONG in_NoOfElementsRequested,
    /* [length_is][size_is][out] */ IDebugIDBInfo2 **out_ArrayOfpIDBInfo2,
    /* [out] */ ULONG *out_pNoOfElementsFetched);


void __RPC_STUB IEnumDebugIDBInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugIDBInfo2_Skip_Proxy( 
    IEnumDebugIDBInfo2 * This,
    /* [in] */ ULONG in_NoOfElements);


void __RPC_STUB IEnumDebugIDBInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugIDBInfo2_Reset_Proxy( 
    IEnumDebugIDBInfo2 * This);


void __RPC_STUB IEnumDebugIDBInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugIDBInfo2_Clone_Proxy( 
    IEnumDebugIDBInfo2 * This,
    /* [out] */ IEnumDebugIDBInfo2 **out_ppEnumIDBInfo2);


void __RPC_STUB IEnumDebugIDBInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugIDBInfo2_GetCount_Proxy( 
    IEnumDebugIDBInfo2 * This,
    /* [out] */ ULONG *out_pCount);


void __RPC_STUB IEnumDebugIDBInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugIDBInfo2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCBuildInfo2_INTERFACE_DEFINED__
#define __IDebugENCBuildInfo2_INTERFACE_DEFINED__

/* interface IDebugENCBuildInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCBuildInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EA70281B-BD58-11d2-9238-00A02448799A")
    IDebugENCBuildInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetTargetBuildInfo( 
            /* [in] */ LPCOLESTR in_szTargetPath,
            /* [out] */ BSTR *out_pbstrSourcePath,
            /* [out] */ BSTR *out_pbstrCommand,
            /* [out] */ BSTR *out_pbstrCurrentdir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsTargetEligible( 
            /* [in] */ LPCOLESTR in_szTargetPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumDebugIDBInfo( 
            /* [out] */ IEnumDebugIDBInfo2 **out_ppEnumIDBInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCBuildInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCBuildInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCBuildInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCBuildInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTargetBuildInfo )( 
            IDebugENCBuildInfo2 * This,
            /* [in] */ LPCOLESTR in_szTargetPath,
            /* [out] */ BSTR *out_pbstrSourcePath,
            /* [out] */ BSTR *out_pbstrCommand,
            /* [out] */ BSTR *out_pbstrCurrentdir);
        
        HRESULT ( STDMETHODCALLTYPE *IsTargetEligible )( 
            IDebugENCBuildInfo2 * This,
            /* [in] */ LPCOLESTR in_szTargetPath);
        
        HRESULT ( STDMETHODCALLTYPE *EnumDebugIDBInfo )( 
            IDebugENCBuildInfo2 * This,
            /* [out] */ IEnumDebugIDBInfo2 **out_ppEnumIDBInfo);
        
        END_INTERFACE
    } IDebugENCBuildInfo2Vtbl;

    interface IDebugENCBuildInfo2
    {
        CONST_VTBL struct IDebugENCBuildInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCBuildInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCBuildInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCBuildInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCBuildInfo2_GetTargetBuildInfo(This,in_szTargetPath,out_pbstrSourcePath,out_pbstrCommand,out_pbstrCurrentdir)	\
    (This)->lpVtbl -> GetTargetBuildInfo(This,in_szTargetPath,out_pbstrSourcePath,out_pbstrCommand,out_pbstrCurrentdir)

#define IDebugENCBuildInfo2_IsTargetEligible(This,in_szTargetPath)	\
    (This)->lpVtbl -> IsTargetEligible(This,in_szTargetPath)

#define IDebugENCBuildInfo2_EnumDebugIDBInfo(This,out_ppEnumIDBInfo)	\
    (This)->lpVtbl -> EnumDebugIDBInfo(This,out_ppEnumIDBInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCBuildInfo2_GetTargetBuildInfo_Proxy( 
    IDebugENCBuildInfo2 * This,
    /* [in] */ LPCOLESTR in_szTargetPath,
    /* [out] */ BSTR *out_pbstrSourcePath,
    /* [out] */ BSTR *out_pbstrCommand,
    /* [out] */ BSTR *out_pbstrCurrentdir);


void __RPC_STUB IDebugENCBuildInfo2_GetTargetBuildInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCBuildInfo2_IsTargetEligible_Proxy( 
    IDebugENCBuildInfo2 * This,
    /* [in] */ LPCOLESTR in_szTargetPath);


void __RPC_STUB IDebugENCBuildInfo2_IsTargetEligible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCBuildInfo2_EnumDebugIDBInfo_Proxy( 
    IDebugENCBuildInfo2 * This,
    /* [out] */ IEnumDebugIDBInfo2 **out_ppEnumIDBInfo);


void __RPC_STUB IDebugENCBuildInfo2_EnumDebugIDBInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCBuildInfo2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCUpdateOnRelinkEvent2_INTERFACE_DEFINED__
#define __IDebugENCUpdateOnRelinkEvent2_INTERFACE_DEFINED__

/* interface IDebugENCUpdateOnRelinkEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCUpdateOnRelinkEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0EBF1959-BD57-11d2-9238-00A02448799A")
    IDebugENCUpdateOnRelinkEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [out] */ IEnumDebugENCRelinkInfo2 **out_ppEnumENCRelinkInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCUpdateOnRelinkEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCUpdateOnRelinkEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCUpdateOnRelinkEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCUpdateOnRelinkEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugENCUpdateOnRelinkEvent2 * This,
            /* [out] */ IEnumDebugENCRelinkInfo2 **out_ppEnumENCRelinkInfo);
        
        END_INTERFACE
    } IDebugENCUpdateOnRelinkEvent2Vtbl;

    interface IDebugENCUpdateOnRelinkEvent2
    {
        CONST_VTBL struct IDebugENCUpdateOnRelinkEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCUpdateOnRelinkEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCUpdateOnRelinkEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCUpdateOnRelinkEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCUpdateOnRelinkEvent2_GetInfo(This,out_ppEnumENCRelinkInfo)	\
    (This)->lpVtbl -> GetInfo(This,out_ppEnumENCRelinkInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCUpdateOnRelinkEvent2_GetInfo_Proxy( 
    IDebugENCUpdateOnRelinkEvent2 * This,
    /* [out] */ IEnumDebugENCRelinkInfo2 **out_ppEnumENCRelinkInfo);


void __RPC_STUB IDebugENCUpdateOnRelinkEvent2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCUpdateOnRelinkEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCUpdateOnStaleCodeEvent2_INTERFACE_DEFINED__
#define __IDebugENCUpdateOnStaleCodeEvent2_INTERFACE_DEFINED__

/* interface IDebugENCUpdateOnStaleCodeEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCUpdateOnStaleCodeEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2F01EB29-BD57-11d2-9238-00A02448799A")
    IDebugENCUpdateOnStaleCodeEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHResult( 
            /* [out] */ HRESULT *out_pHResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCUpdateOnStaleCodeEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCUpdateOnStaleCodeEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCUpdateOnStaleCodeEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCUpdateOnStaleCodeEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugENCUpdateOnStaleCodeEvent2 * This,
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetHResult )( 
            IDebugENCUpdateOnStaleCodeEvent2 * This,
            /* [out] */ HRESULT *out_pHResult);
        
        END_INTERFACE
    } IDebugENCUpdateOnStaleCodeEvent2Vtbl;

    interface IDebugENCUpdateOnStaleCodeEvent2
    {
        CONST_VTBL struct IDebugENCUpdateOnStaleCodeEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCUpdateOnStaleCodeEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCUpdateOnStaleCodeEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCUpdateOnStaleCodeEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCUpdateOnStaleCodeEvent2_GetInfo(This,out_ppEnumENCInfo)	\
    (This)->lpVtbl -> GetInfo(This,out_ppEnumENCInfo)

#define IDebugENCUpdateOnStaleCodeEvent2_GetHResult(This,out_pHResult)	\
    (This)->lpVtbl -> GetHResult(This,out_pHResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCUpdateOnStaleCodeEvent2_GetInfo_Proxy( 
    IDebugENCUpdateOnStaleCodeEvent2 * This,
    /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);


void __RPC_STUB IDebugENCUpdateOnStaleCodeEvent2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCUpdateOnStaleCodeEvent2_GetHResult_Proxy( 
    IDebugENCUpdateOnStaleCodeEvent2 * This,
    /* [out] */ HRESULT *out_pHResult);


void __RPC_STUB IDebugENCUpdateOnStaleCodeEvent2_GetHResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCUpdateOnStaleCodeEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCUpdate_INTERFACE_DEFINED__
#define __IDebugENCUpdate_INTERFACE_DEFINED__

/* interface IDebugENCUpdate */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCUpdate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("978BAEE7-BD4C-11d2-9238-00A02448799A")
    IDebugENCUpdate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumENCSnapshots( 
            /* [in] */ LPCOLESTR pszModule,
            /* [out] */ IEnumDebugENCSnapshots2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumENCSnapshotsByGuid( 
            /* [in] */ REFGUID guidModule,
            /* [out] */ IEnumDebugENCSnapshots2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelENC( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnENCAttemptComplete( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCUpdateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCUpdate * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCUpdate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCUpdate * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumENCSnapshots )( 
            IDebugENCUpdate * This,
            /* [in] */ LPCOLESTR pszModule,
            /* [out] */ IEnumDebugENCSnapshots2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumENCSnapshotsByGuid )( 
            IDebugENCUpdate * This,
            /* [in] */ REFGUID guidModule,
            /* [out] */ IEnumDebugENCSnapshots2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *CancelENC )( 
            IDebugENCUpdate * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnENCAttemptComplete )( 
            IDebugENCUpdate * This);
        
        END_INTERFACE
    } IDebugENCUpdateVtbl;

    interface IDebugENCUpdate
    {
        CONST_VTBL struct IDebugENCUpdateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCUpdate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCUpdate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCUpdate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCUpdate_EnumENCSnapshots(This,pszModule,ppEnum)	\
    (This)->lpVtbl -> EnumENCSnapshots(This,pszModule,ppEnum)

#define IDebugENCUpdate_EnumENCSnapshotsByGuid(This,guidModule,ppEnum)	\
    (This)->lpVtbl -> EnumENCSnapshotsByGuid(This,guidModule,ppEnum)

#define IDebugENCUpdate_CancelENC(This)	\
    (This)->lpVtbl -> CancelENC(This)

#define IDebugENCUpdate_OnENCAttemptComplete(This)	\
    (This)->lpVtbl -> OnENCAttemptComplete(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCUpdate_EnumENCSnapshots_Proxy( 
    IDebugENCUpdate * This,
    /* [in] */ LPCOLESTR pszModule,
    /* [out] */ IEnumDebugENCSnapshots2 **ppEnum);


void __RPC_STUB IDebugENCUpdate_EnumENCSnapshots_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCUpdate_EnumENCSnapshotsByGuid_Proxy( 
    IDebugENCUpdate * This,
    /* [in] */ REFGUID guidModule,
    /* [out] */ IEnumDebugENCSnapshots2 **ppEnum);


void __RPC_STUB IDebugENCUpdate_EnumENCSnapshotsByGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCUpdate_CancelENC_Proxy( 
    IDebugENCUpdate * This);


void __RPC_STUB IDebugENCUpdate_CancelENC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCUpdate_OnENCAttemptComplete_Proxy( 
    IDebugENCUpdate * This);


void __RPC_STUB IDebugENCUpdate_OnENCAttemptComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCUpdate_INTERFACE_DEFINED__ */


#ifndef __IDebugENCSnapshot2_INTERFACE_DEFINED__
#define __IDebugENCSnapshot2_INTERFACE_DEFINED__

/* interface IDebugENCSnapshot2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugENCSnapshot2_0001
    {	ENC_SNAPSHOT_TYPE_COMPLUS	= 0x1,
	ENC_SNAPSHOT_TYPE_CPP	= 0x2
    } ;
typedef DWORD ENC_SNAPSHOT_TYPE;

typedef struct _ENC_SNAPSHOT_COMPLUS
    {
    IDebugComPlusSnapshot2 *pcpSnapshot;
    } 	ENC_SNAPSHOT_COMPLUS;

typedef struct _ENC_SNAPSHOT_CPP
    {
    DWORD dwNYI;
    } 	ENC_SNAPSHOT_CPP;

typedef struct _ENC_SNAPSHOT
    {
    ENC_SNAPSHOT_TYPE ssType;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IDebugENCSnapshot2_0002
        {
        /* [case()] */ ENC_SNAPSHOT_COMPLUS encComPlus;
        /* [case()] */ ENC_SNAPSHOT_CPP encCpp;
        /* [default] */ DWORD unused;
        } 	encSnapshot;
    } 	ENC_SNAPSHOT;

typedef struct _ENC_SNAPSHOT_INFO
    {
    IDebugProgram2 *pProgram;
    ENC_SNAPSHOT encSnapshot;
    } 	ENC_SNAPSHOT_INFO;


EXTERN_C const IID IID_IDebugENCSnapshot2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f6f94d0e-78c2-11d2-8ffe-00c04fa38314")
    IDebugENCSnapshot2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetENCSnapshotInfo( 
            /* [out] */ ENC_SNAPSHOT_INFO *pSnapshotInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyCodeChange( 
            /* [in] */ ULONG in_NoOfLineMaps,
            /* [size_is][in] */ IDebugENCLineMap **in_ArrayOfLineMaps,
            /* [in] */ IServiceProvider *in_pServiceProvider,
            /* [in] */ BOOL in_fOnContinue,
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitChange( 
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCSnapshot2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCSnapshot2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCSnapshot2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCSnapshot2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetENCSnapshotInfo )( 
            IDebugENCSnapshot2 * This,
            /* [out] */ ENC_SNAPSHOT_INFO *pSnapshotInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyCodeChange )( 
            IDebugENCSnapshot2 * This,
            /* [in] */ ULONG in_NoOfLineMaps,
            /* [size_is][in] */ IDebugENCLineMap **in_ArrayOfLineMaps,
            /* [in] */ IServiceProvider *in_pServiceProvider,
            /* [in] */ BOOL in_fOnContinue,
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);
        
        HRESULT ( STDMETHODCALLTYPE *CommitChange )( 
            IDebugENCSnapshot2 * This,
            /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);
        
        END_INTERFACE
    } IDebugENCSnapshot2Vtbl;

    interface IDebugENCSnapshot2
    {
        CONST_VTBL struct IDebugENCSnapshot2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCSnapshot2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCSnapshot2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCSnapshot2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCSnapshot2_GetENCSnapshotInfo(This,pSnapshotInfo)	\
    (This)->lpVtbl -> GetENCSnapshotInfo(This,pSnapshotInfo)

#define IDebugENCSnapshot2_ApplyCodeChange(This,in_NoOfLineMaps,in_ArrayOfLineMaps,in_pServiceProvider,in_fOnContinue,out_ppEnumENCInfo)	\
    (This)->lpVtbl -> ApplyCodeChange(This,in_NoOfLineMaps,in_ArrayOfLineMaps,in_pServiceProvider,in_fOnContinue,out_ppEnumENCInfo)

#define IDebugENCSnapshot2_CommitChange(This,out_ppEnumENCInfo)	\
    (This)->lpVtbl -> CommitChange(This,out_ppEnumENCInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCSnapshot2_GetENCSnapshotInfo_Proxy( 
    IDebugENCSnapshot2 * This,
    /* [out] */ ENC_SNAPSHOT_INFO *pSnapshotInfo);


void __RPC_STUB IDebugENCSnapshot2_GetENCSnapshotInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCSnapshot2_ApplyCodeChange_Proxy( 
    IDebugENCSnapshot2 * This,
    /* [in] */ ULONG in_NoOfLineMaps,
    /* [size_is][in] */ IDebugENCLineMap **in_ArrayOfLineMaps,
    /* [in] */ IServiceProvider *in_pServiceProvider,
    /* [in] */ BOOL in_fOnContinue,
    /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);


void __RPC_STUB IDebugENCSnapshot2_ApplyCodeChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugENCSnapshot2_CommitChange_Proxy( 
    IDebugENCSnapshot2 * This,
    /* [out] */ IEnumDebugENCInfo2 **out_ppEnumENCInfo);


void __RPC_STUB IDebugENCSnapshot2_CommitChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCSnapshot2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugENCSnapshots2_INTERFACE_DEFINED__
#define __IEnumDebugENCSnapshots2_INTERFACE_DEFINED__

/* interface IEnumDebugENCSnapshots2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugENCSnapshots2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f6f94d1a-78c2-11d2-8ffe-00c04fa38314")
    IEnumDebugENCSnapshots2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugENCSnapshot2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugENCSnapshots2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugENCSnapshots2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugENCSnapshots2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugENCSnapshots2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugENCSnapshots2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugENCSnapshots2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugENCSnapshot2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugENCSnapshots2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugENCSnapshots2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugENCSnapshots2 * This,
            /* [out] */ IEnumDebugENCSnapshots2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugENCSnapshots2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugENCSnapshots2Vtbl;

    interface IEnumDebugENCSnapshots2
    {
        CONST_VTBL struct IEnumDebugENCSnapshots2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugENCSnapshots2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugENCSnapshots2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugENCSnapshots2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugENCSnapshots2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugENCSnapshots2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugENCSnapshots2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugENCSnapshots2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugENCSnapshots2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugENCSnapshots2_Next_Proxy( 
    IEnumDebugENCSnapshots2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugENCSnapshot2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugENCSnapshots2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCSnapshots2_Skip_Proxy( 
    IEnumDebugENCSnapshots2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugENCSnapshots2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCSnapshots2_Reset_Proxy( 
    IEnumDebugENCSnapshots2 * This);


void __RPC_STUB IEnumDebugENCSnapshots2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCSnapshots2_Clone_Proxy( 
    IEnumDebugENCSnapshots2 * This,
    /* [out] */ IEnumDebugENCSnapshots2 **ppEnum);


void __RPC_STUB IEnumDebugENCSnapshots2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugENCSnapshots2_GetCount_Proxy( 
    IEnumDebugENCSnapshots2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugENCSnapshots2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugENCSnapshots2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugErrorInfos2_INTERFACE_DEFINED__
#define __IEnumDebugErrorInfos2_INTERFACE_DEFINED__

/* interface IEnumDebugErrorInfos2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugErrorInfos2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f6f94d23-78c2-11d2-8ffe-00c04fa38314")
    IEnumDebugErrorInfos2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IErrorInfo **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugErrorInfos2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugErrorInfos2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugErrorInfos2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugErrorInfos2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugErrorInfos2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugErrorInfos2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IErrorInfo **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugErrorInfos2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugErrorInfos2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugErrorInfos2 * This,
            /* [out] */ IEnumDebugErrorInfos2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugErrorInfos2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugErrorInfos2Vtbl;

    interface IEnumDebugErrorInfos2
    {
        CONST_VTBL struct IEnumDebugErrorInfos2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugErrorInfos2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugErrorInfos2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugErrorInfos2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugErrorInfos2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugErrorInfos2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugErrorInfos2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugErrorInfos2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugErrorInfos2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugErrorInfos2_Next_Proxy( 
    IEnumDebugErrorInfos2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IErrorInfo **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugErrorInfos2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorInfos2_Skip_Proxy( 
    IEnumDebugErrorInfos2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugErrorInfos2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorInfos2_Reset_Proxy( 
    IEnumDebugErrorInfos2 * This);


void __RPC_STUB IEnumDebugErrorInfos2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorInfos2_Clone_Proxy( 
    IEnumDebugErrorInfos2 * This,
    /* [out] */ IEnumDebugErrorInfos2 **ppEnum);


void __RPC_STUB IEnumDebugErrorInfos2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorInfos2_GetCount_Proxy( 
    IEnumDebugErrorInfos2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugErrorInfos2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugErrorInfos2_INTERFACE_DEFINED__ */


#ifndef __IDebugComPlusSnapshot2_INTERFACE_DEFINED__
#define __IDebugComPlusSnapshot2_INTERFACE_DEFINED__

/* interface IDebugComPlusSnapshot2 */
/* [unique][uuid][object] */ 

typedef struct _IL_MAP
    {
    ULONG32 oldOffset;
    ULONG32 newOffset;
    BOOL fAccurate;
    } 	IL_MAP;


EXTERN_C const IID IID_IDebugComPlusSnapshot2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f6f94d20-78c2-11d2-8ffe-00c04fa38314")
    IDebugComPlusSnapshot2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CopyMetaData( 
            /* [in] */ IStream *pIStream,
            /* [out] */ GUID *pMvid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMvid( 
            /* [out] */ GUID *pMvid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRoDataRVA( 
            /* [out] */ ULONG32 *pRoDataRVA) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRwDataRVA( 
            /* [out] */ ULONG32 *pRwDataRVA) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPEBytes( 
            /* [length_is][size_is][in] */ BYTE *pBytes,
            /* [in] */ DWORD dwBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetILMap( 
            /* [in] */ DWORD mdFunction,
            /* [in] */ ULONG cMapSize,
            /* [size_is][in] */ IL_MAP map[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSymbolBytes( 
            /* [length_is][size_is][in] */ BYTE *pBytes,
            /* [in] */ DWORD dwBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSymbolProvider( 
            /* [out] */ IDebugComPlusSymbolProvider **ppSym) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAppDomainAndModuleIDs( 
            /* [out] */ ULONG32 *pulAppDomainID,
            /* [out] */ GUID *pguidModule) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RequestILMap( 
            /* [in] */ DWORD mdFunction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateILMaps( 
            /* [in] */ ULONG in_NoOfLineMaps,
            /* [size_is][in] */ IDebugENCLineMap **in_ArrayOfLineMaps) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugComPlusSnapshot2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugComPlusSnapshot2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugComPlusSnapshot2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugComPlusSnapshot2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CopyMetaData )( 
            IDebugComPlusSnapshot2 * This,
            /* [in] */ IStream *pIStream,
            /* [out] */ GUID *pMvid);
        
        HRESULT ( STDMETHODCALLTYPE *GetMvid )( 
            IDebugComPlusSnapshot2 * This,
            /* [out] */ GUID *pMvid);
        
        HRESULT ( STDMETHODCALLTYPE *GetRoDataRVA )( 
            IDebugComPlusSnapshot2 * This,
            /* [out] */ ULONG32 *pRoDataRVA);
        
        HRESULT ( STDMETHODCALLTYPE *GetRwDataRVA )( 
            IDebugComPlusSnapshot2 * This,
            /* [out] */ ULONG32 *pRwDataRVA);
        
        HRESULT ( STDMETHODCALLTYPE *SetPEBytes )( 
            IDebugComPlusSnapshot2 * This,
            /* [length_is][size_is][in] */ BYTE *pBytes,
            /* [in] */ DWORD dwBytes);
        
        HRESULT ( STDMETHODCALLTYPE *SetILMap )( 
            IDebugComPlusSnapshot2 * This,
            /* [in] */ DWORD mdFunction,
            /* [in] */ ULONG cMapSize,
            /* [size_is][in] */ IL_MAP map[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *SetSymbolBytes )( 
            IDebugComPlusSnapshot2 * This,
            /* [length_is][size_is][in] */ BYTE *pBytes,
            /* [in] */ DWORD dwBytes);
        
        HRESULT ( STDMETHODCALLTYPE *GetSymbolProvider )( 
            IDebugComPlusSnapshot2 * This,
            /* [out] */ IDebugComPlusSymbolProvider **ppSym);
        
        HRESULT ( STDMETHODCALLTYPE *GetAppDomainAndModuleIDs )( 
            IDebugComPlusSnapshot2 * This,
            /* [out] */ ULONG32 *pulAppDomainID,
            /* [out] */ GUID *pguidModule);
        
        HRESULT ( STDMETHODCALLTYPE *RequestILMap )( 
            IDebugComPlusSnapshot2 * This,
            /* [in] */ DWORD mdFunction);
        
        HRESULT ( STDMETHODCALLTYPE *CreateILMaps )( 
            IDebugComPlusSnapshot2 * This,
            /* [in] */ ULONG in_NoOfLineMaps,
            /* [size_is][in] */ IDebugENCLineMap **in_ArrayOfLineMaps);
        
        END_INTERFACE
    } IDebugComPlusSnapshot2Vtbl;

    interface IDebugComPlusSnapshot2
    {
        CONST_VTBL struct IDebugComPlusSnapshot2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugComPlusSnapshot2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugComPlusSnapshot2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugComPlusSnapshot2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugComPlusSnapshot2_CopyMetaData(This,pIStream,pMvid)	\
    (This)->lpVtbl -> CopyMetaData(This,pIStream,pMvid)

#define IDebugComPlusSnapshot2_GetMvid(This,pMvid)	\
    (This)->lpVtbl -> GetMvid(This,pMvid)

#define IDebugComPlusSnapshot2_GetRoDataRVA(This,pRoDataRVA)	\
    (This)->lpVtbl -> GetRoDataRVA(This,pRoDataRVA)

#define IDebugComPlusSnapshot2_GetRwDataRVA(This,pRwDataRVA)	\
    (This)->lpVtbl -> GetRwDataRVA(This,pRwDataRVA)

#define IDebugComPlusSnapshot2_SetPEBytes(This,pBytes,dwBytes)	\
    (This)->lpVtbl -> SetPEBytes(This,pBytes,dwBytes)

#define IDebugComPlusSnapshot2_SetILMap(This,mdFunction,cMapSize,map)	\
    (This)->lpVtbl -> SetILMap(This,mdFunction,cMapSize,map)

#define IDebugComPlusSnapshot2_SetSymbolBytes(This,pBytes,dwBytes)	\
    (This)->lpVtbl -> SetSymbolBytes(This,pBytes,dwBytes)

#define IDebugComPlusSnapshot2_GetSymbolProvider(This,ppSym)	\
    (This)->lpVtbl -> GetSymbolProvider(This,ppSym)

#define IDebugComPlusSnapshot2_GetAppDomainAndModuleIDs(This,pulAppDomainID,pguidModule)	\
    (This)->lpVtbl -> GetAppDomainAndModuleIDs(This,pulAppDomainID,pguidModule)

#define IDebugComPlusSnapshot2_RequestILMap(This,mdFunction)	\
    (This)->lpVtbl -> RequestILMap(This,mdFunction)

#define IDebugComPlusSnapshot2_CreateILMaps(This,in_NoOfLineMaps,in_ArrayOfLineMaps)	\
    (This)->lpVtbl -> CreateILMaps(This,in_NoOfLineMaps,in_ArrayOfLineMaps)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_CopyMetaData_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [in] */ IStream *pIStream,
    /* [out] */ GUID *pMvid);


void __RPC_STUB IDebugComPlusSnapshot2_CopyMetaData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_GetMvid_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [out] */ GUID *pMvid);


void __RPC_STUB IDebugComPlusSnapshot2_GetMvid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_GetRoDataRVA_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [out] */ ULONG32 *pRoDataRVA);


void __RPC_STUB IDebugComPlusSnapshot2_GetRoDataRVA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_GetRwDataRVA_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [out] */ ULONG32 *pRwDataRVA);


void __RPC_STUB IDebugComPlusSnapshot2_GetRwDataRVA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_SetPEBytes_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [length_is][size_is][in] */ BYTE *pBytes,
    /* [in] */ DWORD dwBytes);


void __RPC_STUB IDebugComPlusSnapshot2_SetPEBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_SetILMap_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [in] */ DWORD mdFunction,
    /* [in] */ ULONG cMapSize,
    /* [size_is][in] */ IL_MAP map[  ]);


void __RPC_STUB IDebugComPlusSnapshot2_SetILMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_SetSymbolBytes_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [length_is][size_is][in] */ BYTE *pBytes,
    /* [in] */ DWORD dwBytes);


void __RPC_STUB IDebugComPlusSnapshot2_SetSymbolBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_GetSymbolProvider_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [out] */ IDebugComPlusSymbolProvider **ppSym);


void __RPC_STUB IDebugComPlusSnapshot2_GetSymbolProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_GetAppDomainAndModuleIDs_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [out] */ ULONG32 *pulAppDomainID,
    /* [out] */ GUID *pguidModule);


void __RPC_STUB IDebugComPlusSnapshot2_GetAppDomainAndModuleIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_RequestILMap_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [in] */ DWORD mdFunction);


void __RPC_STUB IDebugComPlusSnapshot2_RequestILMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugComPlusSnapshot2_CreateILMaps_Proxy( 
    IDebugComPlusSnapshot2 * This,
    /* [in] */ ULONG in_NoOfLineMaps,
    /* [size_is][in] */ IDebugENCLineMap **in_ArrayOfLineMaps);


void __RPC_STUB IDebugComPlusSnapshot2_CreateILMaps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugComPlusSnapshot2_INTERFACE_DEFINED__ */


#ifndef __IDebugNativeSnapshot2_INTERFACE_DEFINED__
#define __IDebugNativeSnapshot2_INTERFACE_DEFINED__

/* interface IDebugNativeSnapshot2 */
/* [unique][uuid][object] */ 

typedef 
enum _ENC_NOTIFY
    {	ENC_NOTIFY_COMPILE_START	= 0,
	ENC_NOTIFY_COMPILE_END	= ENC_NOTIFY_COMPILE_START + 1
    } 	ENC_NOTIFY;


EXTERN_C const IID IID_IDebugNativeSnapshot2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("461fda3e-bba5-11d2-b10f-00c04f72dc32")
    IDebugNativeSnapshot2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE HasDependentTargets( 
            /* [in] */ LPCOLESTR pszSourcePath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumDependentImages( 
            /* [in] */ LPCOLESTR pszSourcePath,
            /* [out] */ IEnumString **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumDependentTargets( 
            /* [in] */ ULONG cSrc,
            /* [size_is][in] */ LPCOLESTR pszSourcePath[  ],
            /* [out] */ IEnumString **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTargetBuildInfo( 
            /* [in] */ LPCOLESTR pszTargetPath,
            /* [out] */ BSTR *pbstrSourcePath,
            /* [out] */ BSTR *pbstrCommand,
            /* [out] */ BSTR *pbstrCommandArgs,
            /* [out] */ BSTR *pbstrCurrentDir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Notify( 
            /* [in] */ ENC_NOTIFY encnotify) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsTargetEligible( 
            /* [in] */ LPCOLESTR pszTargetPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddRecompiledTarget( 
            /* [in] */ LPCOLESTR in_szTargetPath,
            /* [in] */ LPCOLESTR in_szSavedTargetPath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugNativeSnapshot2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugNativeSnapshot2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugNativeSnapshot2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *HasDependentTargets )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ LPCOLESTR pszSourcePath);
        
        HRESULT ( STDMETHODCALLTYPE *EnumDependentImages )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ LPCOLESTR pszSourcePath,
            /* [out] */ IEnumString **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumDependentTargets )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ ULONG cSrc,
            /* [size_is][in] */ LPCOLESTR pszSourcePath[  ],
            /* [out] */ IEnumString **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetTargetBuildInfo )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ LPCOLESTR pszTargetPath,
            /* [out] */ BSTR *pbstrSourcePath,
            /* [out] */ BSTR *pbstrCommand,
            /* [out] */ BSTR *pbstrCommandArgs,
            /* [out] */ BSTR *pbstrCurrentDir);
        
        HRESULT ( STDMETHODCALLTYPE *Notify )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ ENC_NOTIFY encnotify);
        
        HRESULT ( STDMETHODCALLTYPE *IsTargetEligible )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ LPCOLESTR pszTargetPath);
        
        HRESULT ( STDMETHODCALLTYPE *AddRecompiledTarget )( 
            IDebugNativeSnapshot2 * This,
            /* [in] */ LPCOLESTR in_szTargetPath,
            /* [in] */ LPCOLESTR in_szSavedTargetPath);
        
        END_INTERFACE
    } IDebugNativeSnapshot2Vtbl;

    interface IDebugNativeSnapshot2
    {
        CONST_VTBL struct IDebugNativeSnapshot2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugNativeSnapshot2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugNativeSnapshot2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugNativeSnapshot2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugNativeSnapshot2_HasDependentTargets(This,pszSourcePath)	\
    (This)->lpVtbl -> HasDependentTargets(This,pszSourcePath)

#define IDebugNativeSnapshot2_EnumDependentImages(This,pszSourcePath,ppEnum)	\
    (This)->lpVtbl -> EnumDependentImages(This,pszSourcePath,ppEnum)

#define IDebugNativeSnapshot2_EnumDependentTargets(This,cSrc,pszSourcePath,ppEnum)	\
    (This)->lpVtbl -> EnumDependentTargets(This,cSrc,pszSourcePath,ppEnum)

#define IDebugNativeSnapshot2_GetTargetBuildInfo(This,pszTargetPath,pbstrSourcePath,pbstrCommand,pbstrCommandArgs,pbstrCurrentDir)	\
    (This)->lpVtbl -> GetTargetBuildInfo(This,pszTargetPath,pbstrSourcePath,pbstrCommand,pbstrCommandArgs,pbstrCurrentDir)

#define IDebugNativeSnapshot2_Notify(This,encnotify)	\
    (This)->lpVtbl -> Notify(This,encnotify)

#define IDebugNativeSnapshot2_IsTargetEligible(This,pszTargetPath)	\
    (This)->lpVtbl -> IsTargetEligible(This,pszTargetPath)

#define IDebugNativeSnapshot2_AddRecompiledTarget(This,in_szTargetPath,in_szSavedTargetPath)	\
    (This)->lpVtbl -> AddRecompiledTarget(This,in_szTargetPath,in_szSavedTargetPath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_HasDependentTargets_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ LPCOLESTR pszSourcePath);


void __RPC_STUB IDebugNativeSnapshot2_HasDependentTargets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_EnumDependentImages_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ LPCOLESTR pszSourcePath,
    /* [out] */ IEnumString **ppEnum);


void __RPC_STUB IDebugNativeSnapshot2_EnumDependentImages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_EnumDependentTargets_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ ULONG cSrc,
    /* [size_is][in] */ LPCOLESTR pszSourcePath[  ],
    /* [out] */ IEnumString **ppEnum);


void __RPC_STUB IDebugNativeSnapshot2_EnumDependentTargets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_GetTargetBuildInfo_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ LPCOLESTR pszTargetPath,
    /* [out] */ BSTR *pbstrSourcePath,
    /* [out] */ BSTR *pbstrCommand,
    /* [out] */ BSTR *pbstrCommandArgs,
    /* [out] */ BSTR *pbstrCurrentDir);


void __RPC_STUB IDebugNativeSnapshot2_GetTargetBuildInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_Notify_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ ENC_NOTIFY encnotify);


void __RPC_STUB IDebugNativeSnapshot2_Notify_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_IsTargetEligible_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ LPCOLESTR pszTargetPath);


void __RPC_STUB IDebugNativeSnapshot2_IsTargetEligible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugNativeSnapshot2_AddRecompiledTarget_Proxy( 
    IDebugNativeSnapshot2 * This,
    /* [in] */ LPCOLESTR in_szTargetPath,
    /* [in] */ LPCOLESTR in_szSavedTargetPath);


void __RPC_STUB IDebugNativeSnapshot2_AddRecompiledTarget_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugNativeSnapshot2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCStackFrame2_INTERFACE_DEFINED__
#define __IDebugENCStackFrame2_INTERFACE_DEFINED__

/* interface IDebugENCStackFrame2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCStackFrame2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B3C64D7F-DB9D-47c7-B479-C579C7F07103")
    IDebugENCStackFrame2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAllLocalsProperty( 
            /* [out] */ IDebugProperty2 **ppProperty) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCStackFrame2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCStackFrame2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCStackFrame2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCStackFrame2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAllLocalsProperty )( 
            IDebugENCStackFrame2 * This,
            /* [out] */ IDebugProperty2 **ppProperty);
        
        END_INTERFACE
    } IDebugENCStackFrame2Vtbl;

    interface IDebugENCStackFrame2
    {
        CONST_VTBL struct IDebugENCStackFrame2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCStackFrame2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCStackFrame2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCStackFrame2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCStackFrame2_GetAllLocalsProperty(This,ppProperty)	\
    (This)->lpVtbl -> GetAllLocalsProperty(This,ppProperty)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCStackFrame2_GetAllLocalsProperty_Proxy( 
    IDebugENCStackFrame2 * This,
    /* [out] */ IDebugProperty2 **ppProperty);


void __RPC_STUB IDebugENCStackFrame2_GetAllLocalsProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCStackFrame2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_enc_0389 */
/* [local] */ 

typedef INT32 _mdToken;



extern RPC_IF_HANDLE __MIDL_itf_enc_0389_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_enc_0389_v0_0_s_ifspec;

#ifndef __IDebugMetaDataEmit2_INTERFACE_DEFINED__
#define __IDebugMetaDataEmit2_INTERFACE_DEFINED__

/* interface IDebugMetaDataEmit2 */
/* [unique][uuid][object] */ 

typedef struct _FIELD_OFFSET
    {
    _mdToken ridOfField;
    ULONG ulOffset;
    } 	FIELD_OFFSET;

typedef struct _IMAGE_FIXUPENTRY
    {
    ULONG ulRVA;
    ULONG Count;
    } 	IMAGE_FIXUPENTRY;


EXTERN_C const IID IID_IDebugMetaDataEmit2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f6f94d21-78c2-11d2-8ffe-00c04fa38314")
    IDebugMetaDataEmit2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetModuleProps( 
            /* [in] */ LPOLESTR szName,
            /* [in] */ GUID *ppid,
            /* [in] */ LCID lcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ LPOLESTR szFile,
            /* [in] */ DWORD dwSaveFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveToStream( 
            /* [in] */ IStream *pIStream,
            /* [in] */ DWORD dwSaveFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSaveSize( 
            /* [in] */ DWORD fSave,
            /* [out] */ DWORD *pdwSaveSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineCustomValueAsBlob( 
            /* [in] */ _mdToken tkObj,
            /* [in] */ LPOLESTR szName,
            /* [size_is][in] */ BYTE *pCustomValue,
            /* [in] */ ULONG cbCustomValue,
            /* [in] */ _mdToken *pcv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineTypeDef( 
            /* [in] */ LPOLESTR szNamespace,
            /* [in] */ LPOLESTR szTypeDef,
            /* [in] */ GUID *pguid,
            /* [in] */ INT64 *pVer,
            /* [in] */ DWORD dwTypeDefFlags,
            /* [in] */ _mdToken tkExtends,
            /* [in] */ DWORD dwExtendsFlags,
            /* [in] */ DWORD dwImplements,
            /* [length_is][size_is][out][in] */ _mdToken rtkImplements[  ],
            /* [in] */ DWORD dwEvents,
            /* [length_is][size_is][out][in] */ _mdToken rtkEvents[  ],
            /* [out] */ _mdToken *ptd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTypeDefProps( 
            /* [in] */ _mdToken td,
            /* [in] */ INT64 *pVer,
            /* [in] */ DWORD dwTypeDefFlags,
            /* [in] */ _mdToken tkExtends,
            /* [in] */ DWORD dwExtendsFlags,
            /* [in] */ DWORD dwImplements,
            /* [length_is][size_is][in] */ _mdToken rtkImplements[  ],
            /* [in] */ DWORD dwEvents,
            /* [length_is][size_is][in] */ _mdToken rtkEvents[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClassSvcsContext( 
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwClassActivateAttr,
            /* [in] */ DWORD dwClassThreadAttr,
            /* [in] */ DWORD dwXactionAttr,
            /* [in] */ DWORD dwSynchAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineTypeRefByGUID( 
            /* [in] */ GUID *pguid,
            /* [out] */ _mdToken *ptr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetModuleReg( 
            /* [in] */ DWORD dwModuleRegAttr,
            /* [in] */ GUID *pguid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClassReg( 
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szProgID,
            /* [in] */ LPOLESTR szVIProgID,
            /* [in] */ LPOLESTR szIconURL,
            /* [in] */ ULONG ulIconResource,
            /* [in] */ LPOLESTR szSmallIconURL,
            /* [in] */ ULONG ulSmallIconResource,
            /* [in] */ LPOLESTR szDefaultDispName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIfaceReg( 
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwIfaceSvcs,
            /* [in] */ GUID *proxyStub) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCategoryImpl( 
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwImpl,
            /* [length_is][size_is][in] */ GUID rGuidCoCatImpl[  ],
            /* [in] */ DWORD dwReqd,
            /* [length_is][size_is][in] */ GUID rGuidCoCatReqd[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRedirectProgID( 
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwProgIds,
            /* [length_is][size_is][in] */ LPOLESTR rszRedirectProgID[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMimeTypeImpl( 
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwTypes,
            /* [length_is][size_is][in] */ LPOLESTR rszMimeType[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFormatImpl( 
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwSupported,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatSupported[  ],
            /* [in] */ DWORD dwFrom,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatConvertsFrom[  ],
            /* [in] */ DWORD dwTo,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatConvertsTo[  ],
            /* [in] */ DWORD dwDefault,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatDefault[  ],
            /* [in] */ DWORD dwExt,
            /* [length_is][size_is][in] */ LPOLESTR rszFileExt[  ],
            /* [in] */ DWORD dwType,
            /* [length_is][size_is][in] */ LPOLESTR rszFileType[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRoleCheck( 
            /* [in] */ _mdToken tk,
            /* [in] */ DWORD dwNames,
            /* [length_is][size_is][in] */ LPOLESTR rszName[  ],
            /* [in] */ DWORD dwFlags,
            /* [length_is][size_is][in] */ DWORD rdwRoleFlags[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineMethod( 
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szName,
            /* [in] */ DWORD dwMethodFlags,
            /* [size_is][in] */ BYTE *pvSigBlob,
            /* [in] */ ULONG cbSigBlob,
            /* [in] */ ULONG ulSlot,
            /* [in] */ ULONG ulCodeRVA,
            /* [in] */ DWORD dwImplFlags,
            /* [out] */ _mdToken *pmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineField( 
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szName,
            /* [in] */ DWORD dwFieldFlags,
            /* [size_is][in] */ BYTE *pvSigBlob,
            /* [in] */ ULONG cbSigBlob,
            /* [in] */ DWORD dwCPlusTypeFlag,
            /* [size_is][in] */ BYTE *pValue,
            /* [in] */ ULONG cbValue,
            /* [out] */ _mdToken *pmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParamProps( 
            /* [in] */ _mdToken md,
            /* [in] */ ULONG ulParamSeq,
            /* [in] */ LPOLESTR szName,
            /* [in] */ DWORD dwParamFlags,
            /* [in] */ DWORD dwCPlusTypeFlag,
            /* [size_is][in] */ BYTE *pValue,
            /* [in] */ ULONG cbValue,
            /* [out] */ _mdToken *ppd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineMethodImpl( 
            /* [in] */ _mdToken td,
            /* [in] */ _mdToken tk,
            /* [in] */ ULONG ulCodeRVA,
            /* [in] */ DWORD dwImplFlags,
            /* [out] */ _mdToken *pmi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRVA( 
            /* [in] */ _mdToken md,
            /* [in] */ ULONG ulCodeRVA,
            /* [in] */ DWORD dwImplFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineTypeRefByName( 
            /* [in] */ LPOLESTR szNamespace,
            /* [in] */ LPOLESTR szType,
            /* [out] */ _mdToken *ptr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTypeRefBind( 
            /* [in] */ _mdToken tr,
            /* [in] */ DWORD dwBindFlags,
            /* [in] */ DWORD dwMinVersion,
            /* [in] */ DWORD dwMaxVersion,
            /* [in] */ LPOLESTR szCodebase) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineMemberRef( 
            /* [in] */ _mdToken tkImport,
            /* [in] */ LPOLESTR szName,
            /* [size_is][in] */ BYTE *pvSigBlob,
            /* [in] */ ULONG cbSigBlob,
            /* [out] */ _mdToken *pmr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineException( 
            /* [in] */ _mdToken mb,
            /* [in] */ _mdToken tk,
            /* [out] */ _mdToken *pex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineProperty( 
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szProperty,
            /* [in] */ DWORD dwPropFlags,
            /* [size_is][in] */ BYTE *pvSig,
            /* [in] */ ULONG cbSig,
            /* [in] */ DWORD dwCPlusTypeFlag,
            /* [size_is][in] */ BYTE *pValue,
            /* [in] */ ULONG cbValue,
            /* [in] */ _mdToken mdSetter,
            /* [in] */ _mdToken mdGetter,
            /* [in] */ _mdToken mdReset,
            /* [in] */ _mdToken mdTestDefault,
            /* [in] */ DWORD dwOthers,
            /* [length_is][size_is][in] */ _mdToken rmdOtherMethods[  ],
            /* [in] */ _mdToken evNotifyChanging,
            /* [in] */ _mdToken evNotifyChanged,
            /* [in] */ _mdToken fdBackingField,
            /* [out] */ _mdToken *pmdProp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineEvent( 
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szEvent,
            /* [in] */ DWORD dwEventFlags,
            /* [in] */ _mdToken tkEventType,
            /* [in] */ _mdToken mdAddOn,
            /* [in] */ _mdToken mdRemoveOn,
            /* [in] */ _mdToken mdFire,
            /* [in] */ DWORD dwOthers,
            /* [length_is][size_is][in] */ _mdToken rmdOtherMethods[  ],
            /* [out] */ _mdToken *pmdEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFieldMarshal( 
            /* [in] */ _mdToken tk,
            /* [size_is][in] */ BYTE *pvNativeType,
            /* [in] */ ULONG cbNativeType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefinePermissionSet( 
            /* [in] */ _mdToken tk,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pvPermission,
            /* [in] */ ULONG cbPermission,
            /* [out] */ _mdToken *ppm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMemberIndex( 
            /* [in] */ _mdToken md,
            /* [in] */ ULONG ulIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenFromSig( 
            /* [size_is][in] */ BYTE *pvSig,
            /* [in] */ ULONG cbSig,
            /* [in] */ _mdToken *pmsig) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineModuleRef( 
            /* [in] */ LPOLESTR szName,
            /* [in] */ GUID *pguid,
            /* [in] */ GUID *pmvid,
            /* [out] */ _mdToken *pmur) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetParent( 
            /* [in] */ _mdToken mr,
            /* [in] */ _mdToken tk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTokenFromArraySpec( 
            /* [size_is][in] */ BYTE *pvSig,
            /* [in] */ ULONG cbSig,
            /* [out] */ _mdToken *parrspec) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMetaDataEmit2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMetaDataEmit2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMetaDataEmit2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetModuleProps )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ LPOLESTR szName,
            /* [in] */ GUID *ppid,
            /* [in] */ LCID lcid);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ LPOLESTR szFile,
            /* [in] */ DWORD dwSaveFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SaveToStream )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ IStream *pIStream,
            /* [in] */ DWORD dwSaveFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetSaveSize )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ DWORD fSave,
            /* [out] */ DWORD *pdwSaveSize);
        
        HRESULT ( STDMETHODCALLTYPE *DefineCustomValueAsBlob )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken tkObj,
            /* [in] */ LPOLESTR szName,
            /* [size_is][in] */ BYTE *pCustomValue,
            /* [in] */ ULONG cbCustomValue,
            /* [in] */ _mdToken *pcv);
        
        HRESULT ( STDMETHODCALLTYPE *DefineTypeDef )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ LPOLESTR szNamespace,
            /* [in] */ LPOLESTR szTypeDef,
            /* [in] */ GUID *pguid,
            /* [in] */ INT64 *pVer,
            /* [in] */ DWORD dwTypeDefFlags,
            /* [in] */ _mdToken tkExtends,
            /* [in] */ DWORD dwExtendsFlags,
            /* [in] */ DWORD dwImplements,
            /* [length_is][size_is][out][in] */ _mdToken rtkImplements[  ],
            /* [in] */ DWORD dwEvents,
            /* [length_is][size_is][out][in] */ _mdToken rtkEvents[  ],
            /* [out] */ _mdToken *ptd);
        
        HRESULT ( STDMETHODCALLTYPE *SetTypeDefProps )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ INT64 *pVer,
            /* [in] */ DWORD dwTypeDefFlags,
            /* [in] */ _mdToken tkExtends,
            /* [in] */ DWORD dwExtendsFlags,
            /* [in] */ DWORD dwImplements,
            /* [length_is][size_is][in] */ _mdToken rtkImplements[  ],
            /* [in] */ DWORD dwEvents,
            /* [length_is][size_is][in] */ _mdToken rtkEvents[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *SetClassSvcsContext )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwClassActivateAttr,
            /* [in] */ DWORD dwClassThreadAttr,
            /* [in] */ DWORD dwXactionAttr,
            /* [in] */ DWORD dwSynchAttr);
        
        HRESULT ( STDMETHODCALLTYPE *DefineTypeRefByGUID )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ GUID *pguid,
            /* [out] */ _mdToken *ptr);
        
        HRESULT ( STDMETHODCALLTYPE *SetModuleReg )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ DWORD dwModuleRegAttr,
            /* [in] */ GUID *pguid);
        
        HRESULT ( STDMETHODCALLTYPE *SetClassReg )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szProgID,
            /* [in] */ LPOLESTR szVIProgID,
            /* [in] */ LPOLESTR szIconURL,
            /* [in] */ ULONG ulIconResource,
            /* [in] */ LPOLESTR szSmallIconURL,
            /* [in] */ ULONG ulSmallIconResource,
            /* [in] */ LPOLESTR szDefaultDispName);
        
        HRESULT ( STDMETHODCALLTYPE *SetIfaceReg )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwIfaceSvcs,
            /* [in] */ GUID *proxyStub);
        
        HRESULT ( STDMETHODCALLTYPE *SetCategoryImpl )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwImpl,
            /* [length_is][size_is][in] */ GUID rGuidCoCatImpl[  ],
            /* [in] */ DWORD dwReqd,
            /* [length_is][size_is][in] */ GUID rGuidCoCatReqd[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *SetRedirectProgID )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwProgIds,
            /* [length_is][size_is][in] */ LPOLESTR rszRedirectProgID[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *SetMimeTypeImpl )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwTypes,
            /* [length_is][size_is][in] */ LPOLESTR rszMimeType[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *SetFormatImpl )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ DWORD dwSupported,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatSupported[  ],
            /* [in] */ DWORD dwFrom,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatConvertsFrom[  ],
            /* [in] */ DWORD dwTo,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatConvertsTo[  ],
            /* [in] */ DWORD dwDefault,
            /* [length_is][size_is][in] */ LPOLESTR rszFormatDefault[  ],
            /* [in] */ DWORD dwExt,
            /* [length_is][size_is][in] */ LPOLESTR rszFileExt[  ],
            /* [in] */ DWORD dwType,
            /* [length_is][size_is][in] */ LPOLESTR rszFileType[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *SetRoleCheck )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken tk,
            /* [in] */ DWORD dwNames,
            /* [length_is][size_is][in] */ LPOLESTR rszName[  ],
            /* [in] */ DWORD dwFlags,
            /* [length_is][size_is][in] */ DWORD rdwRoleFlags[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *DefineMethod )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szName,
            /* [in] */ DWORD dwMethodFlags,
            /* [size_is][in] */ BYTE *pvSigBlob,
            /* [in] */ ULONG cbSigBlob,
            /* [in] */ ULONG ulSlot,
            /* [in] */ ULONG ulCodeRVA,
            /* [in] */ DWORD dwImplFlags,
            /* [out] */ _mdToken *pmd);
        
        HRESULT ( STDMETHODCALLTYPE *DefineField )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szName,
            /* [in] */ DWORD dwFieldFlags,
            /* [size_is][in] */ BYTE *pvSigBlob,
            /* [in] */ ULONG cbSigBlob,
            /* [in] */ DWORD dwCPlusTypeFlag,
            /* [size_is][in] */ BYTE *pValue,
            /* [in] */ ULONG cbValue,
            /* [out] */ _mdToken *pmd);
        
        HRESULT ( STDMETHODCALLTYPE *SetParamProps )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken md,
            /* [in] */ ULONG ulParamSeq,
            /* [in] */ LPOLESTR szName,
            /* [in] */ DWORD dwParamFlags,
            /* [in] */ DWORD dwCPlusTypeFlag,
            /* [size_is][in] */ BYTE *pValue,
            /* [in] */ ULONG cbValue,
            /* [out] */ _mdToken *ppd);
        
        HRESULT ( STDMETHODCALLTYPE *DefineMethodImpl )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ _mdToken tk,
            /* [in] */ ULONG ulCodeRVA,
            /* [in] */ DWORD dwImplFlags,
            /* [out] */ _mdToken *pmi);
        
        HRESULT ( STDMETHODCALLTYPE *SetRVA )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken md,
            /* [in] */ ULONG ulCodeRVA,
            /* [in] */ DWORD dwImplFlags);
        
        HRESULT ( STDMETHODCALLTYPE *DefineTypeRefByName )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ LPOLESTR szNamespace,
            /* [in] */ LPOLESTR szType,
            /* [out] */ _mdToken *ptr);
        
        HRESULT ( STDMETHODCALLTYPE *SetTypeRefBind )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken tr,
            /* [in] */ DWORD dwBindFlags,
            /* [in] */ DWORD dwMinVersion,
            /* [in] */ DWORD dwMaxVersion,
            /* [in] */ LPOLESTR szCodebase);
        
        HRESULT ( STDMETHODCALLTYPE *DefineMemberRef )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken tkImport,
            /* [in] */ LPOLESTR szName,
            /* [size_is][in] */ BYTE *pvSigBlob,
            /* [in] */ ULONG cbSigBlob,
            /* [out] */ _mdToken *pmr);
        
        HRESULT ( STDMETHODCALLTYPE *DefineException )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken mb,
            /* [in] */ _mdToken tk,
            /* [out] */ _mdToken *pex);
        
        HRESULT ( STDMETHODCALLTYPE *DefineProperty )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szProperty,
            /* [in] */ DWORD dwPropFlags,
            /* [size_is][in] */ BYTE *pvSig,
            /* [in] */ ULONG cbSig,
            /* [in] */ DWORD dwCPlusTypeFlag,
            /* [size_is][in] */ BYTE *pValue,
            /* [in] */ ULONG cbValue,
            /* [in] */ _mdToken mdSetter,
            /* [in] */ _mdToken mdGetter,
            /* [in] */ _mdToken mdReset,
            /* [in] */ _mdToken mdTestDefault,
            /* [in] */ DWORD dwOthers,
            /* [length_is][size_is][in] */ _mdToken rmdOtherMethods[  ],
            /* [in] */ _mdToken evNotifyChanging,
            /* [in] */ _mdToken evNotifyChanged,
            /* [in] */ _mdToken fdBackingField,
            /* [out] */ _mdToken *pmdProp);
        
        HRESULT ( STDMETHODCALLTYPE *DefineEvent )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken td,
            /* [in] */ LPOLESTR szEvent,
            /* [in] */ DWORD dwEventFlags,
            /* [in] */ _mdToken tkEventType,
            /* [in] */ _mdToken mdAddOn,
            /* [in] */ _mdToken mdRemoveOn,
            /* [in] */ _mdToken mdFire,
            /* [in] */ DWORD dwOthers,
            /* [length_is][size_is][in] */ _mdToken rmdOtherMethods[  ],
            /* [out] */ _mdToken *pmdEvent);
        
        HRESULT ( STDMETHODCALLTYPE *SetFieldMarshal )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken tk,
            /* [size_is][in] */ BYTE *pvNativeType,
            /* [in] */ ULONG cbNativeType);
        
        HRESULT ( STDMETHODCALLTYPE *DefinePermissionSet )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken tk,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pvPermission,
            /* [in] */ ULONG cbPermission,
            /* [out] */ _mdToken *ppm);
        
        HRESULT ( STDMETHODCALLTYPE *SetMemberIndex )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken md,
            /* [in] */ ULONG ulIndex);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenFromSig )( 
            IDebugMetaDataEmit2 * This,
            /* [size_is][in] */ BYTE *pvSig,
            /* [in] */ ULONG cbSig,
            /* [in] */ _mdToken *pmsig);
        
        HRESULT ( STDMETHODCALLTYPE *DefineModuleRef )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ LPOLESTR szName,
            /* [in] */ GUID *pguid,
            /* [in] */ GUID *pmvid,
            /* [out] */ _mdToken *pmur);
        
        HRESULT ( STDMETHODCALLTYPE *SetParent )( 
            IDebugMetaDataEmit2 * This,
            /* [in] */ _mdToken mr,
            /* [in] */ _mdToken tk);
        
        HRESULT ( STDMETHODCALLTYPE *GetTokenFromArraySpec )( 
            IDebugMetaDataEmit2 * This,
            /* [size_is][in] */ BYTE *pvSig,
            /* [in] */ ULONG cbSig,
            /* [out] */ _mdToken *parrspec);
        
        END_INTERFACE
    } IDebugMetaDataEmit2Vtbl;

    interface IDebugMetaDataEmit2
    {
        CONST_VTBL struct IDebugMetaDataEmit2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMetaDataEmit2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMetaDataEmit2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMetaDataEmit2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMetaDataEmit2_SetModuleProps(This,szName,ppid,lcid)	\
    (This)->lpVtbl -> SetModuleProps(This,szName,ppid,lcid)

#define IDebugMetaDataEmit2_Save(This,szFile,dwSaveFlags)	\
    (This)->lpVtbl -> Save(This,szFile,dwSaveFlags)

#define IDebugMetaDataEmit2_SaveToStream(This,pIStream,dwSaveFlags)	\
    (This)->lpVtbl -> SaveToStream(This,pIStream,dwSaveFlags)

#define IDebugMetaDataEmit2_GetSaveSize(This,fSave,pdwSaveSize)	\
    (This)->lpVtbl -> GetSaveSize(This,fSave,pdwSaveSize)

#define IDebugMetaDataEmit2_DefineCustomValueAsBlob(This,tkObj,szName,pCustomValue,cbCustomValue,pcv)	\
    (This)->lpVtbl -> DefineCustomValueAsBlob(This,tkObj,szName,pCustomValue,cbCustomValue,pcv)

#define IDebugMetaDataEmit2_DefineTypeDef(This,szNamespace,szTypeDef,pguid,pVer,dwTypeDefFlags,tkExtends,dwExtendsFlags,dwImplements,rtkImplements,dwEvents,rtkEvents,ptd)	\
    (This)->lpVtbl -> DefineTypeDef(This,szNamespace,szTypeDef,pguid,pVer,dwTypeDefFlags,tkExtends,dwExtendsFlags,dwImplements,rtkImplements,dwEvents,rtkEvents,ptd)

#define IDebugMetaDataEmit2_SetTypeDefProps(This,td,pVer,dwTypeDefFlags,tkExtends,dwExtendsFlags,dwImplements,rtkImplements,dwEvents,rtkEvents)	\
    (This)->lpVtbl -> SetTypeDefProps(This,td,pVer,dwTypeDefFlags,tkExtends,dwExtendsFlags,dwImplements,rtkImplements,dwEvents,rtkEvents)

#define IDebugMetaDataEmit2_SetClassSvcsContext(This,td,dwClassActivateAttr,dwClassThreadAttr,dwXactionAttr,dwSynchAttr)	\
    (This)->lpVtbl -> SetClassSvcsContext(This,td,dwClassActivateAttr,dwClassThreadAttr,dwXactionAttr,dwSynchAttr)

#define IDebugMetaDataEmit2_DefineTypeRefByGUID(This,pguid,ptr)	\
    (This)->lpVtbl -> DefineTypeRefByGUID(This,pguid,ptr)

#define IDebugMetaDataEmit2_SetModuleReg(This,dwModuleRegAttr,pguid)	\
    (This)->lpVtbl -> SetModuleReg(This,dwModuleRegAttr,pguid)

#define IDebugMetaDataEmit2_SetClassReg(This,td,szProgID,szVIProgID,szIconURL,ulIconResource,szSmallIconURL,ulSmallIconResource,szDefaultDispName)	\
    (This)->lpVtbl -> SetClassReg(This,td,szProgID,szVIProgID,szIconURL,ulIconResource,szSmallIconURL,ulSmallIconResource,szDefaultDispName)

#define IDebugMetaDataEmit2_SetIfaceReg(This,td,dwIfaceSvcs,proxyStub)	\
    (This)->lpVtbl -> SetIfaceReg(This,td,dwIfaceSvcs,proxyStub)

#define IDebugMetaDataEmit2_SetCategoryImpl(This,td,dwImpl,rGuidCoCatImpl,dwReqd,rGuidCoCatReqd)	\
    (This)->lpVtbl -> SetCategoryImpl(This,td,dwImpl,rGuidCoCatImpl,dwReqd,rGuidCoCatReqd)

#define IDebugMetaDataEmit2_SetRedirectProgID(This,td,dwProgIds,rszRedirectProgID)	\
    (This)->lpVtbl -> SetRedirectProgID(This,td,dwProgIds,rszRedirectProgID)

#define IDebugMetaDataEmit2_SetMimeTypeImpl(This,td,dwTypes,rszMimeType)	\
    (This)->lpVtbl -> SetMimeTypeImpl(This,td,dwTypes,rszMimeType)

#define IDebugMetaDataEmit2_SetFormatImpl(This,td,dwSupported,rszFormatSupported,dwFrom,rszFormatConvertsFrom,dwTo,rszFormatConvertsTo,dwDefault,rszFormatDefault,dwExt,rszFileExt,dwType,rszFileType)	\
    (This)->lpVtbl -> SetFormatImpl(This,td,dwSupported,rszFormatSupported,dwFrom,rszFormatConvertsFrom,dwTo,rszFormatConvertsTo,dwDefault,rszFormatDefault,dwExt,rszFileExt,dwType,rszFileType)

#define IDebugMetaDataEmit2_SetRoleCheck(This,tk,dwNames,rszName,dwFlags,rdwRoleFlags)	\
    (This)->lpVtbl -> SetRoleCheck(This,tk,dwNames,rszName,dwFlags,rdwRoleFlags)

#define IDebugMetaDataEmit2_DefineMethod(This,td,szName,dwMethodFlags,pvSigBlob,cbSigBlob,ulSlot,ulCodeRVA,dwImplFlags,pmd)	\
    (This)->lpVtbl -> DefineMethod(This,td,szName,dwMethodFlags,pvSigBlob,cbSigBlob,ulSlot,ulCodeRVA,dwImplFlags,pmd)

#define IDebugMetaDataEmit2_DefineField(This,td,szName,dwFieldFlags,pvSigBlob,cbSigBlob,dwCPlusTypeFlag,pValue,cbValue,pmd)	\
    (This)->lpVtbl -> DefineField(This,td,szName,dwFieldFlags,pvSigBlob,cbSigBlob,dwCPlusTypeFlag,pValue,cbValue,pmd)

#define IDebugMetaDataEmit2_SetParamProps(This,md,ulParamSeq,szName,dwParamFlags,dwCPlusTypeFlag,pValue,cbValue,ppd)	\
    (This)->lpVtbl -> SetParamProps(This,md,ulParamSeq,szName,dwParamFlags,dwCPlusTypeFlag,pValue,cbValue,ppd)

#define IDebugMetaDataEmit2_DefineMethodImpl(This,td,tk,ulCodeRVA,dwImplFlags,pmi)	\
    (This)->lpVtbl -> DefineMethodImpl(This,td,tk,ulCodeRVA,dwImplFlags,pmi)

#define IDebugMetaDataEmit2_SetRVA(This,md,ulCodeRVA,dwImplFlags)	\
    (This)->lpVtbl -> SetRVA(This,md,ulCodeRVA,dwImplFlags)

#define IDebugMetaDataEmit2_DefineTypeRefByName(This,szNamespace,szType,ptr)	\
    (This)->lpVtbl -> DefineTypeRefByName(This,szNamespace,szType,ptr)

#define IDebugMetaDataEmit2_SetTypeRefBind(This,tr,dwBindFlags,dwMinVersion,dwMaxVersion,szCodebase)	\
    (This)->lpVtbl -> SetTypeRefBind(This,tr,dwBindFlags,dwMinVersion,dwMaxVersion,szCodebase)

#define IDebugMetaDataEmit2_DefineMemberRef(This,tkImport,szName,pvSigBlob,cbSigBlob,pmr)	\
    (This)->lpVtbl -> DefineMemberRef(This,tkImport,szName,pvSigBlob,cbSigBlob,pmr)

#define IDebugMetaDataEmit2_DefineException(This,mb,tk,pex)	\
    (This)->lpVtbl -> DefineException(This,mb,tk,pex)

#define IDebugMetaDataEmit2_DefineProperty(This,td,szProperty,dwPropFlags,pvSig,cbSig,dwCPlusTypeFlag,pValue,cbValue,mdSetter,mdGetter,mdReset,mdTestDefault,dwOthers,rmdOtherMethods,evNotifyChanging,evNotifyChanged,fdBackingField,pmdProp)	\
    (This)->lpVtbl -> DefineProperty(This,td,szProperty,dwPropFlags,pvSig,cbSig,dwCPlusTypeFlag,pValue,cbValue,mdSetter,mdGetter,mdReset,mdTestDefault,dwOthers,rmdOtherMethods,evNotifyChanging,evNotifyChanged,fdBackingField,pmdProp)

#define IDebugMetaDataEmit2_DefineEvent(This,td,szEvent,dwEventFlags,tkEventType,mdAddOn,mdRemoveOn,mdFire,dwOthers,rmdOtherMethods,pmdEvent)	\
    (This)->lpVtbl -> DefineEvent(This,td,szEvent,dwEventFlags,tkEventType,mdAddOn,mdRemoveOn,mdFire,dwOthers,rmdOtherMethods,pmdEvent)

#define IDebugMetaDataEmit2_SetFieldMarshal(This,tk,pvNativeType,cbNativeType)	\
    (This)->lpVtbl -> SetFieldMarshal(This,tk,pvNativeType,cbNativeType)

#define IDebugMetaDataEmit2_DefinePermissionSet(This,tk,dwAction,pvPermission,cbPermission,ppm)	\
    (This)->lpVtbl -> DefinePermissionSet(This,tk,dwAction,pvPermission,cbPermission,ppm)

#define IDebugMetaDataEmit2_SetMemberIndex(This,md,ulIndex)	\
    (This)->lpVtbl -> SetMemberIndex(This,md,ulIndex)

#define IDebugMetaDataEmit2_GetTokenFromSig(This,pvSig,cbSig,pmsig)	\
    (This)->lpVtbl -> GetTokenFromSig(This,pvSig,cbSig,pmsig)

#define IDebugMetaDataEmit2_DefineModuleRef(This,szName,pguid,pmvid,pmur)	\
    (This)->lpVtbl -> DefineModuleRef(This,szName,pguid,pmvid,pmur)

#define IDebugMetaDataEmit2_SetParent(This,mr,tk)	\
    (This)->lpVtbl -> SetParent(This,mr,tk)

#define IDebugMetaDataEmit2_GetTokenFromArraySpec(This,pvSig,cbSig,parrspec)	\
    (This)->lpVtbl -> GetTokenFromArraySpec(This,pvSig,cbSig,parrspec)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetModuleProps_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ GUID *ppid,
    /* [in] */ LCID lcid);


void __RPC_STUB IDebugMetaDataEmit2_SetModuleProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_Save_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ LPOLESTR szFile,
    /* [in] */ DWORD dwSaveFlags);


void __RPC_STUB IDebugMetaDataEmit2_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SaveToStream_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ IStream *pIStream,
    /* [in] */ DWORD dwSaveFlags);


void __RPC_STUB IDebugMetaDataEmit2_SaveToStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_GetSaveSize_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ DWORD fSave,
    /* [out] */ DWORD *pdwSaveSize);


void __RPC_STUB IDebugMetaDataEmit2_GetSaveSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineCustomValueAsBlob_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken tkObj,
    /* [in] */ LPOLESTR szName,
    /* [size_is][in] */ BYTE *pCustomValue,
    /* [in] */ ULONG cbCustomValue,
    /* [in] */ _mdToken *pcv);


void __RPC_STUB IDebugMetaDataEmit2_DefineCustomValueAsBlob_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineTypeDef_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ LPOLESTR szNamespace,
    /* [in] */ LPOLESTR szTypeDef,
    /* [in] */ GUID *pguid,
    /* [in] */ INT64 *pVer,
    /* [in] */ DWORD dwTypeDefFlags,
    /* [in] */ _mdToken tkExtends,
    /* [in] */ DWORD dwExtendsFlags,
    /* [in] */ DWORD dwImplements,
    /* [length_is][size_is][out][in] */ _mdToken rtkImplements[  ],
    /* [in] */ DWORD dwEvents,
    /* [length_is][size_is][out][in] */ _mdToken rtkEvents[  ],
    /* [out] */ _mdToken *ptd);


void __RPC_STUB IDebugMetaDataEmit2_DefineTypeDef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetTypeDefProps_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ INT64 *pVer,
    /* [in] */ DWORD dwTypeDefFlags,
    /* [in] */ _mdToken tkExtends,
    /* [in] */ DWORD dwExtendsFlags,
    /* [in] */ DWORD dwImplements,
    /* [length_is][size_is][in] */ _mdToken rtkImplements[  ],
    /* [in] */ DWORD dwEvents,
    /* [length_is][size_is][in] */ _mdToken rtkEvents[  ]);


void __RPC_STUB IDebugMetaDataEmit2_SetTypeDefProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetClassSvcsContext_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ DWORD dwClassActivateAttr,
    /* [in] */ DWORD dwClassThreadAttr,
    /* [in] */ DWORD dwXactionAttr,
    /* [in] */ DWORD dwSynchAttr);


void __RPC_STUB IDebugMetaDataEmit2_SetClassSvcsContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineTypeRefByGUID_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ GUID *pguid,
    /* [out] */ _mdToken *ptr);


void __RPC_STUB IDebugMetaDataEmit2_DefineTypeRefByGUID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetModuleReg_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ DWORD dwModuleRegAttr,
    /* [in] */ GUID *pguid);


void __RPC_STUB IDebugMetaDataEmit2_SetModuleReg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetClassReg_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ LPOLESTR szProgID,
    /* [in] */ LPOLESTR szVIProgID,
    /* [in] */ LPOLESTR szIconURL,
    /* [in] */ ULONG ulIconResource,
    /* [in] */ LPOLESTR szSmallIconURL,
    /* [in] */ ULONG ulSmallIconResource,
    /* [in] */ LPOLESTR szDefaultDispName);


void __RPC_STUB IDebugMetaDataEmit2_SetClassReg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetIfaceReg_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ DWORD dwIfaceSvcs,
    /* [in] */ GUID *proxyStub);


void __RPC_STUB IDebugMetaDataEmit2_SetIfaceReg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetCategoryImpl_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ DWORD dwImpl,
    /* [length_is][size_is][in] */ GUID rGuidCoCatImpl[  ],
    /* [in] */ DWORD dwReqd,
    /* [length_is][size_is][in] */ GUID rGuidCoCatReqd[  ]);


void __RPC_STUB IDebugMetaDataEmit2_SetCategoryImpl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetRedirectProgID_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ DWORD dwProgIds,
    /* [length_is][size_is][in] */ LPOLESTR rszRedirectProgID[  ]);


void __RPC_STUB IDebugMetaDataEmit2_SetRedirectProgID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetMimeTypeImpl_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ DWORD dwTypes,
    /* [length_is][size_is][in] */ LPOLESTR rszMimeType[  ]);


void __RPC_STUB IDebugMetaDataEmit2_SetMimeTypeImpl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetFormatImpl_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ DWORD dwSupported,
    /* [length_is][size_is][in] */ LPOLESTR rszFormatSupported[  ],
    /* [in] */ DWORD dwFrom,
    /* [length_is][size_is][in] */ LPOLESTR rszFormatConvertsFrom[  ],
    /* [in] */ DWORD dwTo,
    /* [length_is][size_is][in] */ LPOLESTR rszFormatConvertsTo[  ],
    /* [in] */ DWORD dwDefault,
    /* [length_is][size_is][in] */ LPOLESTR rszFormatDefault[  ],
    /* [in] */ DWORD dwExt,
    /* [length_is][size_is][in] */ LPOLESTR rszFileExt[  ],
    /* [in] */ DWORD dwType,
    /* [length_is][size_is][in] */ LPOLESTR rszFileType[  ]);


void __RPC_STUB IDebugMetaDataEmit2_SetFormatImpl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetRoleCheck_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken tk,
    /* [in] */ DWORD dwNames,
    /* [length_is][size_is][in] */ LPOLESTR rszName[  ],
    /* [in] */ DWORD dwFlags,
    /* [length_is][size_is][in] */ DWORD rdwRoleFlags[  ]);


void __RPC_STUB IDebugMetaDataEmit2_SetRoleCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineMethod_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ LPOLESTR szName,
    /* [in] */ DWORD dwMethodFlags,
    /* [size_is][in] */ BYTE *pvSigBlob,
    /* [in] */ ULONG cbSigBlob,
    /* [in] */ ULONG ulSlot,
    /* [in] */ ULONG ulCodeRVA,
    /* [in] */ DWORD dwImplFlags,
    /* [out] */ _mdToken *pmd);


void __RPC_STUB IDebugMetaDataEmit2_DefineMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineField_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ LPOLESTR szName,
    /* [in] */ DWORD dwFieldFlags,
    /* [size_is][in] */ BYTE *pvSigBlob,
    /* [in] */ ULONG cbSigBlob,
    /* [in] */ DWORD dwCPlusTypeFlag,
    /* [size_is][in] */ BYTE *pValue,
    /* [in] */ ULONG cbValue,
    /* [out] */ _mdToken *pmd);


void __RPC_STUB IDebugMetaDataEmit2_DefineField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetParamProps_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken md,
    /* [in] */ ULONG ulParamSeq,
    /* [in] */ LPOLESTR szName,
    /* [in] */ DWORD dwParamFlags,
    /* [in] */ DWORD dwCPlusTypeFlag,
    /* [size_is][in] */ BYTE *pValue,
    /* [in] */ ULONG cbValue,
    /* [out] */ _mdToken *ppd);


void __RPC_STUB IDebugMetaDataEmit2_SetParamProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineMethodImpl_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ _mdToken tk,
    /* [in] */ ULONG ulCodeRVA,
    /* [in] */ DWORD dwImplFlags,
    /* [out] */ _mdToken *pmi);


void __RPC_STUB IDebugMetaDataEmit2_DefineMethodImpl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetRVA_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken md,
    /* [in] */ ULONG ulCodeRVA,
    /* [in] */ DWORD dwImplFlags);


void __RPC_STUB IDebugMetaDataEmit2_SetRVA_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineTypeRefByName_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ LPOLESTR szNamespace,
    /* [in] */ LPOLESTR szType,
    /* [out] */ _mdToken *ptr);


void __RPC_STUB IDebugMetaDataEmit2_DefineTypeRefByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetTypeRefBind_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken tr,
    /* [in] */ DWORD dwBindFlags,
    /* [in] */ DWORD dwMinVersion,
    /* [in] */ DWORD dwMaxVersion,
    /* [in] */ LPOLESTR szCodebase);


void __RPC_STUB IDebugMetaDataEmit2_SetTypeRefBind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineMemberRef_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken tkImport,
    /* [in] */ LPOLESTR szName,
    /* [size_is][in] */ BYTE *pvSigBlob,
    /* [in] */ ULONG cbSigBlob,
    /* [out] */ _mdToken *pmr);


void __RPC_STUB IDebugMetaDataEmit2_DefineMemberRef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineException_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken mb,
    /* [in] */ _mdToken tk,
    /* [out] */ _mdToken *pex);


void __RPC_STUB IDebugMetaDataEmit2_DefineException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineProperty_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ LPOLESTR szProperty,
    /* [in] */ DWORD dwPropFlags,
    /* [size_is][in] */ BYTE *pvSig,
    /* [in] */ ULONG cbSig,
    /* [in] */ DWORD dwCPlusTypeFlag,
    /* [size_is][in] */ BYTE *pValue,
    /* [in] */ ULONG cbValue,
    /* [in] */ _mdToken mdSetter,
    /* [in] */ _mdToken mdGetter,
    /* [in] */ _mdToken mdReset,
    /* [in] */ _mdToken mdTestDefault,
    /* [in] */ DWORD dwOthers,
    /* [length_is][size_is][in] */ _mdToken rmdOtherMethods[  ],
    /* [in] */ _mdToken evNotifyChanging,
    /* [in] */ _mdToken evNotifyChanged,
    /* [in] */ _mdToken fdBackingField,
    /* [out] */ _mdToken *pmdProp);


void __RPC_STUB IDebugMetaDataEmit2_DefineProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineEvent_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken td,
    /* [in] */ LPOLESTR szEvent,
    /* [in] */ DWORD dwEventFlags,
    /* [in] */ _mdToken tkEventType,
    /* [in] */ _mdToken mdAddOn,
    /* [in] */ _mdToken mdRemoveOn,
    /* [in] */ _mdToken mdFire,
    /* [in] */ DWORD dwOthers,
    /* [length_is][size_is][in] */ _mdToken rmdOtherMethods[  ],
    /* [out] */ _mdToken *pmdEvent);


void __RPC_STUB IDebugMetaDataEmit2_DefineEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetFieldMarshal_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken tk,
    /* [size_is][in] */ BYTE *pvNativeType,
    /* [in] */ ULONG cbNativeType);


void __RPC_STUB IDebugMetaDataEmit2_SetFieldMarshal_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefinePermissionSet_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken tk,
    /* [in] */ DWORD dwAction,
    /* [size_is][in] */ BYTE *pvPermission,
    /* [in] */ ULONG cbPermission,
    /* [out] */ _mdToken *ppm);


void __RPC_STUB IDebugMetaDataEmit2_DefinePermissionSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetMemberIndex_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken md,
    /* [in] */ ULONG ulIndex);


void __RPC_STUB IDebugMetaDataEmit2_SetMemberIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_GetTokenFromSig_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [size_is][in] */ BYTE *pvSig,
    /* [in] */ ULONG cbSig,
    /* [in] */ _mdToken *pmsig);


void __RPC_STUB IDebugMetaDataEmit2_GetTokenFromSig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_DefineModuleRef_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ GUID *pguid,
    /* [in] */ GUID *pmvid,
    /* [out] */ _mdToken *pmur);


void __RPC_STUB IDebugMetaDataEmit2_DefineModuleRef_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_SetParent_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [in] */ _mdToken mr,
    /* [in] */ _mdToken tk);


void __RPC_STUB IDebugMetaDataEmit2_SetParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataEmit2_GetTokenFromArraySpec_Proxy( 
    IDebugMetaDataEmit2 * This,
    /* [size_is][in] */ BYTE *pvSig,
    /* [in] */ ULONG cbSig,
    /* [out] */ _mdToken *parrspec);


void __RPC_STUB IDebugMetaDataEmit2_GetTokenFromArraySpec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMetaDataEmit2_INTERFACE_DEFINED__ */


#ifndef __IDebugMetaDataDebugEmit2_INTERFACE_DEFINED__
#define __IDebugMetaDataDebugEmit2_INTERFACE_DEFINED__

/* interface IDebugMetaDataDebugEmit2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugMetaDataDebugEmit2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f6f94d22-78c2-11d2-8ffe-00c04fa38314")
    IDebugMetaDataDebugEmit2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DefineSourceFile( 
            /* [in] */ LPOLESTR szFileName,
            /* [out] */ _mdToken *psourcefile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineBlock( 
            /* [in] */ _mdToken member,
            /* [in] */ _mdToken sourcefile,
            /* [in] */ BYTE *pAttr,
            /* [in] */ ULONG cbAttr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DefineLocalVarScope( 
            /* [in] */ _mdToken scopeParent,
            /* [in] */ ULONG ulStartLine,
            /* [in] */ ULONG ulEndLine,
            /* [in] */ _mdToken member,
            /* [out] */ _mdToken *plocalvarscope) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMetaDataDebugEmit2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMetaDataDebugEmit2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMetaDataDebugEmit2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMetaDataDebugEmit2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *DefineSourceFile )( 
            IDebugMetaDataDebugEmit2 * This,
            /* [in] */ LPOLESTR szFileName,
            /* [out] */ _mdToken *psourcefile);
        
        HRESULT ( STDMETHODCALLTYPE *DefineBlock )( 
            IDebugMetaDataDebugEmit2 * This,
            /* [in] */ _mdToken member,
            /* [in] */ _mdToken sourcefile,
            /* [in] */ BYTE *pAttr,
            /* [in] */ ULONG cbAttr);
        
        HRESULT ( STDMETHODCALLTYPE *DefineLocalVarScope )( 
            IDebugMetaDataDebugEmit2 * This,
            /* [in] */ _mdToken scopeParent,
            /* [in] */ ULONG ulStartLine,
            /* [in] */ ULONG ulEndLine,
            /* [in] */ _mdToken member,
            /* [out] */ _mdToken *plocalvarscope);
        
        END_INTERFACE
    } IDebugMetaDataDebugEmit2Vtbl;

    interface IDebugMetaDataDebugEmit2
    {
        CONST_VTBL struct IDebugMetaDataDebugEmit2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMetaDataDebugEmit2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMetaDataDebugEmit2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMetaDataDebugEmit2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMetaDataDebugEmit2_DefineSourceFile(This,szFileName,psourcefile)	\
    (This)->lpVtbl -> DefineSourceFile(This,szFileName,psourcefile)

#define IDebugMetaDataDebugEmit2_DefineBlock(This,member,sourcefile,pAttr,cbAttr)	\
    (This)->lpVtbl -> DefineBlock(This,member,sourcefile,pAttr,cbAttr)

#define IDebugMetaDataDebugEmit2_DefineLocalVarScope(This,scopeParent,ulStartLine,ulEndLine,member,plocalvarscope)	\
    (This)->lpVtbl -> DefineLocalVarScope(This,scopeParent,ulStartLine,ulEndLine,member,plocalvarscope)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMetaDataDebugEmit2_DefineSourceFile_Proxy( 
    IDebugMetaDataDebugEmit2 * This,
    /* [in] */ LPOLESTR szFileName,
    /* [out] */ _mdToken *psourcefile);


void __RPC_STUB IDebugMetaDataDebugEmit2_DefineSourceFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataDebugEmit2_DefineBlock_Proxy( 
    IDebugMetaDataDebugEmit2 * This,
    /* [in] */ _mdToken member,
    /* [in] */ _mdToken sourcefile,
    /* [in] */ BYTE *pAttr,
    /* [in] */ ULONG cbAttr);


void __RPC_STUB IDebugMetaDataDebugEmit2_DefineBlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMetaDataDebugEmit2_DefineLocalVarScope_Proxy( 
    IDebugMetaDataDebugEmit2 * This,
    /* [in] */ _mdToken scopeParent,
    /* [in] */ ULONG ulStartLine,
    /* [in] */ ULONG ulEndLine,
    /* [in] */ _mdToken member,
    /* [out] */ _mdToken *plocalvarscope);


void __RPC_STUB IDebugMetaDataDebugEmit2_DefineLocalVarScope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMetaDataDebugEmit2_INTERFACE_DEFINED__ */


#ifndef __IDebugENCStateEvents_INTERFACE_DEFINED__
#define __IDebugENCStateEvents_INTERFACE_DEFINED__

/* interface IDebugENCStateEvents */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugENCStateEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ec80d064-102e-435f-aafb-d37e2a4ef654")
    IDebugENCStateEvents : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStateChange( 
            /* [in] */ ENCSTATE *in_pENCSTATE,
            /* [in] */ BOOL in_fReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugENCStateEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugENCStateEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugENCStateEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugENCStateEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnStateChange )( 
            IDebugENCStateEvents * This,
            /* [in] */ ENCSTATE *in_pENCSTATE,
            /* [in] */ BOOL in_fReserved);
        
        END_INTERFACE
    } IDebugENCStateEventsVtbl;

    interface IDebugENCStateEvents
    {
        CONST_VTBL struct IDebugENCStateEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugENCStateEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugENCStateEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugENCStateEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugENCStateEvents_OnStateChange(This,in_pENCSTATE,in_fReserved)	\
    (This)->lpVtbl -> OnStateChange(This,in_pENCSTATE,in_fReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugENCStateEvents_OnStateChange_Proxy( 
    IDebugENCStateEvents * This,
    /* [in] */ ENCSTATE *in_pENCSTATE,
    /* [in] */ BOOL in_fReserved);


void __RPC_STUB IDebugENCStateEvents_OnStateChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugENCStateEvents_INTERFACE_DEFINED__ */



#ifndef __EncLib_LIBRARY_DEFINED__
#define __EncLib_LIBRARY_DEFINED__

/* library EncLib */
/* [uuid] */ 


EXTERN_C const IID LIBID_EncLib;

EXTERN_C const CLSID CLSID_EncMgr;

#ifdef __cplusplus

class DECLSPEC_UUID("99A426F1-AF1D-11d2-922C-00A02448799A")
EncMgr;
#endif
#endif /* __EncLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


