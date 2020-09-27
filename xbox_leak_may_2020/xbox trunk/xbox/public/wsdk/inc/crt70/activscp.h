
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0334 */
/* Compiler settings for activscp.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
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

#ifndef __activscp_h__
#define __activscp_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IActiveScriptSite_FWD_DEFINED__
#define __IActiveScriptSite_FWD_DEFINED__
typedef interface IActiveScriptSite IActiveScriptSite;
#endif 	/* __IActiveScriptSite_FWD_DEFINED__ */


#ifndef __IActiveScriptError_FWD_DEFINED__
#define __IActiveScriptError_FWD_DEFINED__
typedef interface IActiveScriptError IActiveScriptError;
#endif 	/* __IActiveScriptError_FWD_DEFINED__ */


#ifndef __IActiveScriptSiteWindow_FWD_DEFINED__
#define __IActiveScriptSiteWindow_FWD_DEFINED__
typedef interface IActiveScriptSiteWindow IActiveScriptSiteWindow;
#endif 	/* __IActiveScriptSiteWindow_FWD_DEFINED__ */


#ifndef __IActiveScriptSiteInterruptPoll_FWD_DEFINED__
#define __IActiveScriptSiteInterruptPoll_FWD_DEFINED__
typedef interface IActiveScriptSiteInterruptPoll IActiveScriptSiteInterruptPoll;
#endif 	/* __IActiveScriptSiteInterruptPoll_FWD_DEFINED__ */


#ifndef __IActiveScript_FWD_DEFINED__
#define __IActiveScript_FWD_DEFINED__
typedef interface IActiveScript IActiveScript;
#endif 	/* __IActiveScript_FWD_DEFINED__ */


#ifndef __IActiveScriptParse_FWD_DEFINED__
#define __IActiveScriptParse_FWD_DEFINED__
typedef interface IActiveScriptParse IActiveScriptParse;
#endif 	/* __IActiveScriptParse_FWD_DEFINED__ */


#ifndef __IActiveScriptParseProcedureOld_FWD_DEFINED__
#define __IActiveScriptParseProcedureOld_FWD_DEFINED__
typedef interface IActiveScriptParseProcedureOld IActiveScriptParseProcedureOld;
#endif 	/* __IActiveScriptParseProcedureOld_FWD_DEFINED__ */


#ifndef __IActiveScriptParseProcedure_FWD_DEFINED__
#define __IActiveScriptParseProcedure_FWD_DEFINED__
typedef interface IActiveScriptParseProcedure IActiveScriptParseProcedure;
#endif 	/* __IActiveScriptParseProcedure_FWD_DEFINED__ */


#ifndef __IActiveScriptParseProcedure2_FWD_DEFINED__
#define __IActiveScriptParseProcedure2_FWD_DEFINED__
typedef interface IActiveScriptParseProcedure2 IActiveScriptParseProcedure2;
#endif 	/* __IActiveScriptParseProcedure2_FWD_DEFINED__ */


#ifndef __IActiveScriptEncode_FWD_DEFINED__
#define __IActiveScriptEncode_FWD_DEFINED__
typedef interface IActiveScriptEncode IActiveScriptEncode;
#endif 	/* __IActiveScriptEncode_FWD_DEFINED__ */


#ifndef __IActiveScriptHostEncode_FWD_DEFINED__
#define __IActiveScriptHostEncode_FWD_DEFINED__
typedef interface IActiveScriptHostEncode IActiveScriptHostEncode;
#endif 	/* __IActiveScriptHostEncode_FWD_DEFINED__ */


#ifndef __IBindEventHandler_FWD_DEFINED__
#define __IBindEventHandler_FWD_DEFINED__
typedef interface IBindEventHandler IBindEventHandler;
#endif 	/* __IBindEventHandler_FWD_DEFINED__ */


#ifndef __IActiveScriptStats_FWD_DEFINED__
#define __IActiveScriptStats_FWD_DEFINED__
typedef interface IActiveScriptStats IActiveScriptStats;
#endif 	/* __IActiveScriptStats_FWD_DEFINED__ */


#ifndef __IActiveScriptProperty_FWD_DEFINED__
#define __IActiveScriptProperty_FWD_DEFINED__
typedef interface IActiveScriptProperty IActiveScriptProperty;
#endif 	/* __IActiveScriptProperty_FWD_DEFINED__ */


#ifndef __ITridentEventSink_FWD_DEFINED__
#define __ITridentEventSink_FWD_DEFINED__
typedef interface ITridentEventSink ITridentEventSink;
#endif 	/* __ITridentEventSink_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_activscp_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// ActivScp.h
//=--------------------------------------------------------------------------=
// (C) Copyright 1996 Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=
//
#pragma comment(lib,"uuid.lib")
//
// Declarations for ActiveX Scripting host applications and script engines.
//

#ifndef __ActivScp_h
#define __ActivScp_h

/* GUIDs
 ********/

#ifndef _NO_SCRIPT_GUIDS
// {F0B7A1A1-9847-11cf-8F20-00805F2CD064}
DEFINE_GUID(CATID_ActiveScript, 0xf0b7a1a1, 0x9847, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {F0B7A1A2-9847-11cf-8F20-00805F2CD064}
DEFINE_GUID(CATID_ActiveScriptParse, 0xf0b7a1a2, 0x9847, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {F0B7A1A3-9847-11cf-8F20-00805F2CD064}
DEFINE_GUID(CATID_ActiveScriptEncode, 0xf0b7a1a3, 0x9847, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {BB1A2AE1-A4F9-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IActiveScript, 0xbb1a2ae1, 0xa4f9, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {BB1A2AE2-A4F9-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IActiveScriptParse, 0xbb1a2ae2, 0xa4f9, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {BB1A2AE3-A4F9-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IActiveScriptEncode, 0xbb1a2ae3, 0xa4f9, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {BEE9B76E-CFE3-11d1-B747-00C04FC2B085}
DEFINE_GUID(IID_IActiveScriptHostEncode, 0xbee9b76e, 0xcfe3, 0x11d1, 0xb7, 0x47, 0x0, 0xc0, 0x4f, 0xc2, 0xb0, 0x85);

// {1CFF0050-6FDD-11d0-9328-00A0C90DCAA9}
DEFINE_GUID(IID_IActiveScriptParseProcedureOld, 0x1cff0050, 0x6fdd, 0x11d0, 0x93, 0x28, 0x0, 0xa0, 0xc9, 0xd, 0xca, 0xa9);

// {AA5B6A80-B834-11d0-932F-00A0C90DCAA9}
DEFINE_GUID(IID_IActiveScriptParseProcedure, 0xaa5b6a80, 0xb834, 0x11d0, 0x93, 0x2f, 0x0, 0xa0, 0xc9, 0xd, 0xca, 0xa9);

// {71EE5B20-FB04-11d1-B3A8-00A0C911E8B2}
DEFINE_GUID(IID_IActiveScriptParseProcedure2, 0x71ee5b20, 0xfb04, 0x11d1, 0xb3, 0xa8, 0x0, 0xa0, 0xc9, 0x11, 0xe8, 0xb2);

// {DB01A1E3-A42B-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IActiveScriptSite, 0xdb01a1e3, 0xa42b, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {D10F6761-83E9-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IActiveScriptSiteWindow, 0xd10f6761, 0x83e9, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {539698A0-CDCA-11CF-A5EB-00AA0047A063}
DEFINE_GUID(IID_IActiveScriptSiteInterruptPoll, 0x539698a0, 0xcdca, 0x11cf, 0xa5, 0xeb, 0x00, 0xaa, 0x00, 0x47, 0xa0, 0x63);

// {EAE1BA61-A4ED-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IActiveScriptError, 0xeae1ba61, 0xa4ed, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);

// {63CDBCB0-C1B1-11d0-9336-00A0C90DCAA9}
DEFINE_GUID(IID_IBindEventHandler, 0x63cdbcb0, 0xc1b1, 0x11d0, 0x93, 0x36, 0x0, 0xa0, 0xc9, 0xd, 0xca, 0xa9);

// {B8DA6310-E19B-11d0-933C-00A0C90DCAA9}
DEFINE_GUID(IID_IActiveScriptStats, 0xb8da6310, 0xe19b, 0x11d0, 0x93, 0x3c, 0x0, 0xa0, 0xc9, 0xd, 0xca, 0xa9);

// {4954E0D0-FBC7-11D1-8410-006008C3FBFC}
DEFINE_GUID(IID_IActiveScriptProperty, 0x4954E0D0, 0xFBC7, 0x11D1, 0x84, 0x10, 0x00, 0x60, 0x08, 0xC3, 0xFB, 0xFC);

// {1DC9CA50-06EF-11d2-8415-006008C3FBFC}
DEFINE_GUID(IID_ITridentEventSink, 0x1dc9ca50, 0x6ef, 0x11d2, 0x84, 0x15, 0x0, 0x60, 0x8, 0xc3, 0xfb, 0xfc);

#endif // _NO_SCRIPT_GUIDS

// Constants used by ActiveX Scripting:
//

/* IActiveScript::AddNamedItem() input flags */

#define SCRIPTITEM_ISVISIBLE            0x00000002
#define SCRIPTITEM_ISSOURCE             0x00000004
#define SCRIPTITEM_GLOBALMEMBERS        0x00000008
#define SCRIPTITEM_ISPERSISTENT         0x00000040
#define SCRIPTITEM_CODEONLY             0x00000200
#define SCRIPTITEM_NOCODE               0x00000400

#define SCRIPTITEM_ALL_FLAGS            (SCRIPTITEM_ISSOURCE | \
                                         SCRIPTITEM_ISVISIBLE | \
                                         SCRIPTITEM_ISPERSISTENT | \
                                         SCRIPTITEM_GLOBALMEMBERS | \
                                         SCRIPTITEM_NOCODE | \
                                         SCRIPTITEM_CODEONLY)

/* IActiveScript::AddTypeLib() input flags */

#define SCRIPTTYPELIB_ISCONTROL         0x00000010
#define SCRIPTTYPELIB_ISPERSISTENT      0x00000040
#define SCRIPTTYPELIB_ALL_FLAGS         (SCRIPTTYPELIB_ISCONTROL | SCRIPTTYPELIB_ISPERSISTENT)

/* IActiveScriptParse::AddScriptlet() and IActiveScriptParse::ParseScriptText() input flags */

#define SCRIPTTEXT_DELAYEXECUTION       0x00000001
#define SCRIPTTEXT_ISVISIBLE            0x00000002
#define SCRIPTTEXT_ISEXPRESSION         0x00000020
#define SCRIPTTEXT_ISPERSISTENT         0x00000040
#define SCRIPTTEXT_HOSTMANAGESSOURCE    0x00000080
#define SCRIPTTEXT_ALL_FLAGS            (SCRIPTTEXT_DELAYEXECUTION | \
                                         SCRIPTTEXT_ISVISIBLE | \
                                         SCRIPTTEXT_ISEXPRESSION | \
                                         SCRIPTTEXT_ISPERSISTENT | \
                                         SCRIPTTEXT_HOSTMANAGESSOURCE)

/* IActiveScriptParseProcedure::ParseProcedureText() input flags */

#define SCRIPTPROC_ISEXPRESSION         0x00000020
#define SCRIPTPROC_HOSTMANAGESSOURCE    0x00000080
#define SCRIPTPROC_IMPLICIT_THIS        0x00000100
#define SCRIPTPROC_IMPLICIT_PARENTS     0x00000200
#define SCRIPTPROC_ALL_FLAGS            (SCRIPTPROC_HOSTMANAGESSOURCE | \
                                         SCRIPTPROC_ISEXPRESSION | \
                                         SCRIPTPROC_IMPLICIT_THIS | \
                                         SCRIPTPROC_IMPLICIT_PARENTS)

/* IActiveScriptSite::GetItemInfo() input flags */

#define SCRIPTINFO_IUNKNOWN             0x00000001
#define SCRIPTINFO_ITYPEINFO            0x00000002
#define SCRIPTINFO_ALL_FLAGS            (SCRIPTINFO_IUNKNOWN | \
                                         SCRIPTINFO_ITYPEINFO)

/* IActiveScript::Interrupt() Flags */

#define SCRIPTINTERRUPT_DEBUG           0x00000001
#define SCRIPTINTERRUPT_RAISEEXCEPTION  0x00000002
#define SCRIPTINTERRUPT_ALL_FLAGS       (SCRIPTINTERRUPT_DEBUG | \
                                         SCRIPTINTERRUPT_RAISEEXCEPTION)

/* IActiveScriptStats::GetStat() values */

#define SCRIPTSTAT_STATEMENT_COUNT       1
#define SCRIPTSTAT_INSTRUCTION_COUNT     2
#define SCRIPTSTAT_INTSTRUCTION_TIME     3
#define SCRIPTSTAT_TOTAL_TIME            4

/* IActiveScriptEncode::AddSection() input flags */

#define SCRIPT_ENCODE_SECTION         0x00000001

#define SCRIPT_ENCODE_DEFAULT_LANGUAGE        0x00000001

/* Properties for IActiveScriptProperty */
#define SCRIPTPROP_NAME                     0x00000000
#define SCRIPTPROP_MAJORVERSION             0x00000001
#define SCRIPTPROP_MINORVERSION             0x00000002
#define SCRIPTPROP_BUILDNUMBER              0x00000003

#define SCRIPTPROP_DELAYEDEVENTSINKING      0x00001000
#define SCRIPTPROP_CATCHEXCEPTION           0x00001001

#define SCRIPTPROP_DEBUGGER                 0x00001100
#define SCRIPTPROP_JITDEBUG                 0x00001101

// These properties are defined and available, but are not
// officially supported.
#define SCRIPTPROP_HACK_FIBERSUPPORT        0x70000000
#define SCRIPTPROP_HACK_TRIDENTEVENTSINK    0x70000001

typedef 
enum tagSCRIPTSTATE
    {	SCRIPTSTATE_UNINITIALIZED	= 0,
	SCRIPTSTATE_INITIALIZED	= 5,
	SCRIPTSTATE_STARTED	= 1,
	SCRIPTSTATE_CONNECTED	= 2,
	SCRIPTSTATE_DISCONNECTED	= 3,
	SCRIPTSTATE_CLOSED	= 4
    } 	SCRIPTSTATE;


/* script thread state values */

typedef 
enum tagSCRIPTTHREADSTATE
    {	SCRIPTTHREADSTATE_NOTINSCRIPT	= 0,
	SCRIPTTHREADSTATE_RUNNING	= 1
    } 	SCRIPTTHREADSTATE;


/* Thread IDs */

typedef DWORD SCRIPTTHREADID;


#define SCRIPTTHREADID_CURRENT  ((SCRIPTTHREADID)-1)
#define SCRIPTTHREADID_BASE     ((SCRIPTTHREADID)-2)
#define SCRIPTTHREADID_ALL      ((SCRIPTTHREADID)-3)

/* Structures */

/* Interfaces
 *************/











extern RPC_IF_HANDLE __MIDL_itf_activscp_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_activscp_0000_v0_0_s_ifspec;

#ifndef __IActiveScriptSite_INTERFACE_DEFINED__
#define __IActiveScriptSite_INTERFACE_DEFINED__

/* interface IActiveScriptSite */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DB01A1E3-A42B-11cf-8F20-00805F2CD064")
    IActiveScriptSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLCID( 
            /* [out] */ LCID *plcid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetItemInfo( 
            /* [in] */ LPCOLESTR pstrName,
            /* [in] */ DWORD dwReturnMask,
            /* [out] */ IUnknown **ppiunkItem,
            /* [out] */ ITypeInfo **ppti) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocVersionString( 
            /* [out] */ BSTR *pbstrVersion) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnScriptTerminate( 
            /* [in] */ const VARIANT *pvarResult,
            /* [in] */ const EXCEPINFO *pexcepinfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStateChange( 
            /* [in] */ SCRIPTSTATE ssScriptState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnScriptError( 
            /* [in] */ IActiveScriptError *pscripterror) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEnterScript( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnLeaveScript( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLCID )( 
            IActiveScriptSite * This,
            /* [out] */ LCID *plcid);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemInfo )( 
            IActiveScriptSite * This,
            /* [in] */ LPCOLESTR pstrName,
            /* [in] */ DWORD dwReturnMask,
            /* [out] */ IUnknown **ppiunkItem,
            /* [out] */ ITypeInfo **ppti);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocVersionString )( 
            IActiveScriptSite * This,
            /* [out] */ BSTR *pbstrVersion);
        
        HRESULT ( STDMETHODCALLTYPE *OnScriptTerminate )( 
            IActiveScriptSite * This,
            /* [in] */ const VARIANT *pvarResult,
            /* [in] */ const EXCEPINFO *pexcepinfo);
        
        HRESULT ( STDMETHODCALLTYPE *OnStateChange )( 
            IActiveScriptSite * This,
            /* [in] */ SCRIPTSTATE ssScriptState);
        
        HRESULT ( STDMETHODCALLTYPE *OnScriptError )( 
            IActiveScriptSite * This,
            /* [in] */ IActiveScriptError *pscripterror);
        
        HRESULT ( STDMETHODCALLTYPE *OnEnterScript )( 
            IActiveScriptSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnLeaveScript )( 
            IActiveScriptSite * This);
        
        END_INTERFACE
    } IActiveScriptSiteVtbl;

    interface IActiveScriptSite
    {
        CONST_VTBL struct IActiveScriptSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptSite_GetLCID(This,plcid)	\
    (This)->lpVtbl -> GetLCID(This,plcid)

#define IActiveScriptSite_GetItemInfo(This,pstrName,dwReturnMask,ppiunkItem,ppti)	\
    (This)->lpVtbl -> GetItemInfo(This,pstrName,dwReturnMask,ppiunkItem,ppti)

#define IActiveScriptSite_GetDocVersionString(This,pbstrVersion)	\
    (This)->lpVtbl -> GetDocVersionString(This,pbstrVersion)

#define IActiveScriptSite_OnScriptTerminate(This,pvarResult,pexcepinfo)	\
    (This)->lpVtbl -> OnScriptTerminate(This,pvarResult,pexcepinfo)

#define IActiveScriptSite_OnStateChange(This,ssScriptState)	\
    (This)->lpVtbl -> OnStateChange(This,ssScriptState)

#define IActiveScriptSite_OnScriptError(This,pscripterror)	\
    (This)->lpVtbl -> OnScriptError(This,pscripterror)

#define IActiveScriptSite_OnEnterScript(This)	\
    (This)->lpVtbl -> OnEnterScript(This)

#define IActiveScriptSite_OnLeaveScript(This)	\
    (This)->lpVtbl -> OnLeaveScript(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptSite_GetLCID_Proxy( 
    IActiveScriptSite * This,
    /* [out] */ LCID *plcid);


void __RPC_STUB IActiveScriptSite_GetLCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_GetItemInfo_Proxy( 
    IActiveScriptSite * This,
    /* [in] */ LPCOLESTR pstrName,
    /* [in] */ DWORD dwReturnMask,
    /* [out] */ IUnknown **ppiunkItem,
    /* [out] */ ITypeInfo **ppti);


void __RPC_STUB IActiveScriptSite_GetItemInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_GetDocVersionString_Proxy( 
    IActiveScriptSite * This,
    /* [out] */ BSTR *pbstrVersion);


void __RPC_STUB IActiveScriptSite_GetDocVersionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_OnScriptTerminate_Proxy( 
    IActiveScriptSite * This,
    /* [in] */ const VARIANT *pvarResult,
    /* [in] */ const EXCEPINFO *pexcepinfo);


void __RPC_STUB IActiveScriptSite_OnScriptTerminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_OnStateChange_Proxy( 
    IActiveScriptSite * This,
    /* [in] */ SCRIPTSTATE ssScriptState);


void __RPC_STUB IActiveScriptSite_OnStateChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_OnScriptError_Proxy( 
    IActiveScriptSite * This,
    /* [in] */ IActiveScriptError *pscripterror);


void __RPC_STUB IActiveScriptSite_OnScriptError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_OnEnterScript_Proxy( 
    IActiveScriptSite * This);


void __RPC_STUB IActiveScriptSite_OnEnterScript_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSite_OnLeaveScript_Proxy( 
    IActiveScriptSite * This);


void __RPC_STUB IActiveScriptSite_OnLeaveScript_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptSite_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptError_INTERFACE_DEFINED__
#define __IActiveScriptError_INTERFACE_DEFINED__

/* interface IActiveScriptError */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptError;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EAE1BA61-A4ED-11cf-8F20-00805F2CD064")
    IActiveScriptError : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetExceptionInfo( 
            /* [out] */ EXCEPINFO *pexcepinfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourcePosition( 
            /* [out] */ DWORD *pdwSourceContext,
            /* [out] */ ULONG *pulLineNumber,
            /* [out] */ LONG *plCharacterPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceLineText( 
            /* [out] */ BSTR *pbstrSourceLine) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptErrorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptError * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptError * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptError * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetExceptionInfo )( 
            IActiveScriptError * This,
            /* [out] */ EXCEPINFO *pexcepinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourcePosition )( 
            IActiveScriptError * This,
            /* [out] */ DWORD *pdwSourceContext,
            /* [out] */ ULONG *pulLineNumber,
            /* [out] */ LONG *plCharacterPosition);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceLineText )( 
            IActiveScriptError * This,
            /* [out] */ BSTR *pbstrSourceLine);
        
        END_INTERFACE
    } IActiveScriptErrorVtbl;

    interface IActiveScriptError
    {
        CONST_VTBL struct IActiveScriptErrorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptError_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptError_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptError_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptError_GetExceptionInfo(This,pexcepinfo)	\
    (This)->lpVtbl -> GetExceptionInfo(This,pexcepinfo)

#define IActiveScriptError_GetSourcePosition(This,pdwSourceContext,pulLineNumber,plCharacterPosition)	\
    (This)->lpVtbl -> GetSourcePosition(This,pdwSourceContext,pulLineNumber,plCharacterPosition)

#define IActiveScriptError_GetSourceLineText(This,pbstrSourceLine)	\
    (This)->lpVtbl -> GetSourceLineText(This,pbstrSourceLine)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptError_GetExceptionInfo_Proxy( 
    IActiveScriptError * This,
    /* [out] */ EXCEPINFO *pexcepinfo);


void __RPC_STUB IActiveScriptError_GetExceptionInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptError_GetSourcePosition_Proxy( 
    IActiveScriptError * This,
    /* [out] */ DWORD *pdwSourceContext,
    /* [out] */ ULONG *pulLineNumber,
    /* [out] */ LONG *plCharacterPosition);


void __RPC_STUB IActiveScriptError_GetSourcePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptError_GetSourceLineText_Proxy( 
    IActiveScriptError * This,
    /* [out] */ BSTR *pbstrSourceLine);


void __RPC_STUB IActiveScriptError_GetSourceLineText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptError_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptSiteWindow_INTERFACE_DEFINED__
#define __IActiveScriptSiteWindow_INTERFACE_DEFINED__

/* interface IActiveScriptSiteWindow */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptSiteWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D10F6761-83E9-11cf-8F20-00805F2CD064")
    IActiveScriptSiteWindow : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [out] */ HWND *phwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptSiteWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptSiteWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptSiteWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptSiteWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IActiveScriptSiteWindow * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IActiveScriptSiteWindow * This,
            /* [in] */ BOOL fEnable);
        
        END_INTERFACE
    } IActiveScriptSiteWindowVtbl;

    interface IActiveScriptSiteWindow
    {
        CONST_VTBL struct IActiveScriptSiteWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptSiteWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptSiteWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptSiteWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptSiteWindow_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IActiveScriptSiteWindow_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptSiteWindow_GetWindow_Proxy( 
    IActiveScriptSiteWindow * This,
    /* [out] */ HWND *phwnd);


void __RPC_STUB IActiveScriptSiteWindow_GetWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptSiteWindow_EnableModeless_Proxy( 
    IActiveScriptSiteWindow * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IActiveScriptSiteWindow_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptSiteWindow_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptSiteInterruptPoll_INTERFACE_DEFINED__
#define __IActiveScriptSiteInterruptPoll_INTERFACE_DEFINED__

/* interface IActiveScriptSiteInterruptPoll */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptSiteInterruptPoll;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("539698A0-CDCA-11CF-A5EB-00AA0047A063")
    IActiveScriptSiteInterruptPoll : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryContinue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptSiteInterruptPollVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptSiteInterruptPoll * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptSiteInterruptPoll * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptSiteInterruptPoll * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryContinue )( 
            IActiveScriptSiteInterruptPoll * This);
        
        END_INTERFACE
    } IActiveScriptSiteInterruptPollVtbl;

    interface IActiveScriptSiteInterruptPoll
    {
        CONST_VTBL struct IActiveScriptSiteInterruptPollVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptSiteInterruptPoll_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptSiteInterruptPoll_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptSiteInterruptPoll_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptSiteInterruptPoll_QueryContinue(This)	\
    (This)->lpVtbl -> QueryContinue(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptSiteInterruptPoll_QueryContinue_Proxy( 
    IActiveScriptSiteInterruptPoll * This);


void __RPC_STUB IActiveScriptSiteInterruptPoll_QueryContinue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptSiteInterruptPoll_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_activscp_0250 */
/* [local] */ 

typedef IActiveScriptSite *PIActiveScriptSite;


extern RPC_IF_HANDLE __MIDL_itf_activscp_0250_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_activscp_0250_v0_0_s_ifspec;

#ifndef __IActiveScript_INTERFACE_DEFINED__
#define __IActiveScript_INTERFACE_DEFINED__

/* interface IActiveScript */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScript;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB1A2AE1-A4F9-11cf-8F20-00805F2CD064")
    IActiveScript : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetScriptSite( 
            /* [in] */ IActiveScriptSite *pass) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScriptSite( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetScriptState( 
            /* [in] */ SCRIPTSTATE ss) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScriptState( 
            /* [out] */ SCRIPTSTATE *pssState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddNamedItem( 
            /* [in] */ LPCOLESTR pstrName,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddTypeLib( 
            /* [in] */ REFGUID rguidTypeLib,
            /* [in] */ DWORD dwMajor,
            /* [in] */ DWORD dwMinor,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScriptDispatch( 
            /* [in] */ LPCOLESTR pstrItemName,
            /* [out] */ IDispatch **ppdisp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentScriptThreadID( 
            /* [out] */ SCRIPTTHREADID *pstidThread) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScriptThreadID( 
            /* [in] */ DWORD dwWin32ThreadId,
            /* [out] */ SCRIPTTHREADID *pstidThread) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScriptThreadState( 
            /* [in] */ SCRIPTTHREADID stidThread,
            /* [out] */ SCRIPTTHREADSTATE *pstsState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InterruptScriptThread( 
            /* [in] */ SCRIPTTHREADID stidThread,
            /* [in] */ const EXCEPINFO *pexcepinfo,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IActiveScript **ppscript) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScript * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScript * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScript * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetScriptSite )( 
            IActiveScript * This,
            /* [in] */ IActiveScriptSite *pass);
        
        HRESULT ( STDMETHODCALLTYPE *GetScriptSite )( 
            IActiveScript * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        HRESULT ( STDMETHODCALLTYPE *SetScriptState )( 
            IActiveScript * This,
            /* [in] */ SCRIPTSTATE ss);
        
        HRESULT ( STDMETHODCALLTYPE *GetScriptState )( 
            IActiveScript * This,
            /* [out] */ SCRIPTSTATE *pssState);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IActiveScript * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddNamedItem )( 
            IActiveScript * This,
            /* [in] */ LPCOLESTR pstrName,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *AddTypeLib )( 
            IActiveScript * This,
            /* [in] */ REFGUID rguidTypeLib,
            /* [in] */ DWORD dwMajor,
            /* [in] */ DWORD dwMinor,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetScriptDispatch )( 
            IActiveScript * This,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [out] */ IDispatch **ppdisp);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentScriptThreadID )( 
            IActiveScript * This,
            /* [out] */ SCRIPTTHREADID *pstidThread);
        
        HRESULT ( STDMETHODCALLTYPE *GetScriptThreadID )( 
            IActiveScript * This,
            /* [in] */ DWORD dwWin32ThreadId,
            /* [out] */ SCRIPTTHREADID *pstidThread);
        
        HRESULT ( STDMETHODCALLTYPE *GetScriptThreadState )( 
            IActiveScript * This,
            /* [in] */ SCRIPTTHREADID stidThread,
            /* [out] */ SCRIPTTHREADSTATE *pstsState);
        
        HRESULT ( STDMETHODCALLTYPE *InterruptScriptThread )( 
            IActiveScript * This,
            /* [in] */ SCRIPTTHREADID stidThread,
            /* [in] */ const EXCEPINFO *pexcepinfo,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IActiveScript * This,
            /* [out] */ IActiveScript **ppscript);
        
        END_INTERFACE
    } IActiveScriptVtbl;

    interface IActiveScript
    {
        CONST_VTBL struct IActiveScriptVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScript_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScript_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScript_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScript_SetScriptSite(This,pass)	\
    (This)->lpVtbl -> SetScriptSite(This,pass)

#define IActiveScript_GetScriptSite(This,riid,ppvObject)	\
    (This)->lpVtbl -> GetScriptSite(This,riid,ppvObject)

#define IActiveScript_SetScriptState(This,ss)	\
    (This)->lpVtbl -> SetScriptState(This,ss)

#define IActiveScript_GetScriptState(This,pssState)	\
    (This)->lpVtbl -> GetScriptState(This,pssState)

#define IActiveScript_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define IActiveScript_AddNamedItem(This,pstrName,dwFlags)	\
    (This)->lpVtbl -> AddNamedItem(This,pstrName,dwFlags)

#define IActiveScript_AddTypeLib(This,rguidTypeLib,dwMajor,dwMinor,dwFlags)	\
    (This)->lpVtbl -> AddTypeLib(This,rguidTypeLib,dwMajor,dwMinor,dwFlags)

#define IActiveScript_GetScriptDispatch(This,pstrItemName,ppdisp)	\
    (This)->lpVtbl -> GetScriptDispatch(This,pstrItemName,ppdisp)

#define IActiveScript_GetCurrentScriptThreadID(This,pstidThread)	\
    (This)->lpVtbl -> GetCurrentScriptThreadID(This,pstidThread)

#define IActiveScript_GetScriptThreadID(This,dwWin32ThreadId,pstidThread)	\
    (This)->lpVtbl -> GetScriptThreadID(This,dwWin32ThreadId,pstidThread)

#define IActiveScript_GetScriptThreadState(This,stidThread,pstsState)	\
    (This)->lpVtbl -> GetScriptThreadState(This,stidThread,pstsState)

#define IActiveScript_InterruptScriptThread(This,stidThread,pexcepinfo,dwFlags)	\
    (This)->lpVtbl -> InterruptScriptThread(This,stidThread,pexcepinfo,dwFlags)

#define IActiveScript_Clone(This,ppscript)	\
    (This)->lpVtbl -> Clone(This,ppscript)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScript_SetScriptSite_Proxy( 
    IActiveScript * This,
    /* [in] */ IActiveScriptSite *pass);


void __RPC_STUB IActiveScript_SetScriptSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_GetScriptSite_Proxy( 
    IActiveScript * This,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvObject);


void __RPC_STUB IActiveScript_GetScriptSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_SetScriptState_Proxy( 
    IActiveScript * This,
    /* [in] */ SCRIPTSTATE ss);


void __RPC_STUB IActiveScript_SetScriptState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_GetScriptState_Proxy( 
    IActiveScript * This,
    /* [out] */ SCRIPTSTATE *pssState);


void __RPC_STUB IActiveScript_GetScriptState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_Close_Proxy( 
    IActiveScript * This);


void __RPC_STUB IActiveScript_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_AddNamedItem_Proxy( 
    IActiveScript * This,
    /* [in] */ LPCOLESTR pstrName,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IActiveScript_AddNamedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_AddTypeLib_Proxy( 
    IActiveScript * This,
    /* [in] */ REFGUID rguidTypeLib,
    /* [in] */ DWORD dwMajor,
    /* [in] */ DWORD dwMinor,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IActiveScript_AddTypeLib_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_GetScriptDispatch_Proxy( 
    IActiveScript * This,
    /* [in] */ LPCOLESTR pstrItemName,
    /* [out] */ IDispatch **ppdisp);


void __RPC_STUB IActiveScript_GetScriptDispatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_GetCurrentScriptThreadID_Proxy( 
    IActiveScript * This,
    /* [out] */ SCRIPTTHREADID *pstidThread);


void __RPC_STUB IActiveScript_GetCurrentScriptThreadID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_GetScriptThreadID_Proxy( 
    IActiveScript * This,
    /* [in] */ DWORD dwWin32ThreadId,
    /* [out] */ SCRIPTTHREADID *pstidThread);


void __RPC_STUB IActiveScript_GetScriptThreadID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_GetScriptThreadState_Proxy( 
    IActiveScript * This,
    /* [in] */ SCRIPTTHREADID stidThread,
    /* [out] */ SCRIPTTHREADSTATE *pstsState);


void __RPC_STUB IActiveScript_GetScriptThreadState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_InterruptScriptThread_Proxy( 
    IActiveScript * This,
    /* [in] */ SCRIPTTHREADID stidThread,
    /* [in] */ const EXCEPINFO *pexcepinfo,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IActiveScript_InterruptScriptThread_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScript_Clone_Proxy( 
    IActiveScript * This,
    /* [out] */ IActiveScript **ppscript);


void __RPC_STUB IActiveScript_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScript_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_activscp_0251 */
/* [local] */ 

typedef IActiveScript *PIActiveScript;


extern RPC_IF_HANDLE __MIDL_itf_activscp_0251_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_activscp_0251_v0_0_s_ifspec;

#ifndef __IActiveScriptParse_INTERFACE_DEFINED__
#define __IActiveScriptParse_INTERFACE_DEFINED__

/* interface IActiveScriptParse */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptParse;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB1A2AE2-A4F9-11cf-8F20-00805F2CD064")
    IActiveScriptParse : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitNew( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddScriptlet( 
            /* [in] */ LPCOLESTR pstrDefaultName,
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ LPCOLESTR pstrSubItemName,
            /* [in] */ LPCOLESTR pstrEventName,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ BSTR *pbstrName,
            /* [out] */ EXCEPINFO *pexcepinfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseScriptText( 
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ VARIANT *pvarResult,
            /* [out] */ EXCEPINFO *pexcepinfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptParseVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptParse * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptParse * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptParse * This);
        
        HRESULT ( STDMETHODCALLTYPE *InitNew )( 
            IActiveScriptParse * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddScriptlet )( 
            IActiveScriptParse * This,
            /* [in] */ LPCOLESTR pstrDefaultName,
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ LPCOLESTR pstrSubItemName,
            /* [in] */ LPCOLESTR pstrEventName,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ BSTR *pbstrName,
            /* [out] */ EXCEPINFO *pexcepinfo);
        
        HRESULT ( STDMETHODCALLTYPE *ParseScriptText )( 
            IActiveScriptParse * This,
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ VARIANT *pvarResult,
            /* [out] */ EXCEPINFO *pexcepinfo);
        
        END_INTERFACE
    } IActiveScriptParseVtbl;

    interface IActiveScriptParse
    {
        CONST_VTBL struct IActiveScriptParseVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptParse_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptParse_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptParse_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptParse_InitNew(This)	\
    (This)->lpVtbl -> InitNew(This)

#define IActiveScriptParse_AddScriptlet(This,pstrDefaultName,pstrCode,pstrItemName,pstrSubItemName,pstrEventName,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,pbstrName,pexcepinfo)	\
    (This)->lpVtbl -> AddScriptlet(This,pstrDefaultName,pstrCode,pstrItemName,pstrSubItemName,pstrEventName,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,pbstrName,pexcepinfo)

#define IActiveScriptParse_ParseScriptText(This,pstrCode,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,pvarResult,pexcepinfo)	\
    (This)->lpVtbl -> ParseScriptText(This,pstrCode,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,pvarResult,pexcepinfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptParse_InitNew_Proxy( 
    IActiveScriptParse * This);


void __RPC_STUB IActiveScriptParse_InitNew_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptParse_AddScriptlet_Proxy( 
    IActiveScriptParse * This,
    /* [in] */ LPCOLESTR pstrDefaultName,
    /* [in] */ LPCOLESTR pstrCode,
    /* [in] */ LPCOLESTR pstrItemName,
    /* [in] */ LPCOLESTR pstrSubItemName,
    /* [in] */ LPCOLESTR pstrEventName,
    /* [in] */ LPCOLESTR pstrDelimiter,
    /* [in] */ DWORD dwSourceContextCookie,
    /* [in] */ ULONG ulStartingLineNumber,
    /* [in] */ DWORD dwFlags,
    /* [out] */ BSTR *pbstrName,
    /* [out] */ EXCEPINFO *pexcepinfo);


void __RPC_STUB IActiveScriptParse_AddScriptlet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptParse_ParseScriptText_Proxy( 
    IActiveScriptParse * This,
    /* [in] */ LPCOLESTR pstrCode,
    /* [in] */ LPCOLESTR pstrItemName,
    /* [in] */ IUnknown *punkContext,
    /* [in] */ LPCOLESTR pstrDelimiter,
    /* [in] */ DWORD dwSourceContextCookie,
    /* [in] */ ULONG ulStartingLineNumber,
    /* [in] */ DWORD dwFlags,
    /* [out] */ VARIANT *pvarResult,
    /* [out] */ EXCEPINFO *pexcepinfo);


void __RPC_STUB IActiveScriptParse_ParseScriptText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptParse_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_activscp_0252 */
/* [local] */ 


typedef IActiveScriptParse *PIActiveScriptParse;



extern RPC_IF_HANDLE __MIDL_itf_activscp_0252_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_activscp_0252_v0_0_s_ifspec;

#ifndef __IActiveScriptParseProcedureOld_INTERFACE_DEFINED__
#define __IActiveScriptParseProcedureOld_INTERFACE_DEFINED__

/* interface IActiveScriptParseProcedureOld */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptParseProcedureOld;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1CFF0050-6FDD-11d0-9328-00A0C90DCAA9")
    IActiveScriptParseProcedureOld : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseProcedureText( 
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrFormalParams,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ IDispatch **ppdisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptParseProcedureOldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptParseProcedureOld * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptParseProcedureOld * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptParseProcedureOld * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseProcedureText )( 
            IActiveScriptParseProcedureOld * This,
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrFormalParams,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ IDispatch **ppdisp);
        
        END_INTERFACE
    } IActiveScriptParseProcedureOldVtbl;

    interface IActiveScriptParseProcedureOld
    {
        CONST_VTBL struct IActiveScriptParseProcedureOldVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptParseProcedureOld_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptParseProcedureOld_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptParseProcedureOld_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptParseProcedureOld_ParseProcedureText(This,pstrCode,pstrFormalParams,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,ppdisp)	\
    (This)->lpVtbl -> ParseProcedureText(This,pstrCode,pstrFormalParams,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,ppdisp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptParseProcedureOld_ParseProcedureText_Proxy( 
    IActiveScriptParseProcedureOld * This,
    /* [in] */ LPCOLESTR pstrCode,
    /* [in] */ LPCOLESTR pstrFormalParams,
    /* [in] */ LPCOLESTR pstrItemName,
    /* [in] */ IUnknown *punkContext,
    /* [in] */ LPCOLESTR pstrDelimiter,
    /* [in] */ DWORD dwSourceContextCookie,
    /* [in] */ ULONG ulStartingLineNumber,
    /* [in] */ DWORD dwFlags,
    /* [out] */ IDispatch **ppdisp);


void __RPC_STUB IActiveScriptParseProcedureOld_ParseProcedureText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptParseProcedureOld_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptParseProcedure_INTERFACE_DEFINED__
#define __IActiveScriptParseProcedure_INTERFACE_DEFINED__

/* interface IActiveScriptParseProcedure */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptParseProcedure;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AA5B6A80-B834-11d0-932F-00A0C90DCAA9")
    IActiveScriptParseProcedure : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseProcedureText( 
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrFormalParams,
            /* [in] */ LPCOLESTR pstrProcedureName,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ IDispatch **ppdisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptParseProcedureVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptParseProcedure * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptParseProcedure * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptParseProcedure * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseProcedureText )( 
            IActiveScriptParseProcedure * This,
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrFormalParams,
            /* [in] */ LPCOLESTR pstrProcedureName,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ IDispatch **ppdisp);
        
        END_INTERFACE
    } IActiveScriptParseProcedureVtbl;

    interface IActiveScriptParseProcedure
    {
        CONST_VTBL struct IActiveScriptParseProcedureVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptParseProcedure_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptParseProcedure_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptParseProcedure_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptParseProcedure_ParseProcedureText(This,pstrCode,pstrFormalParams,pstrProcedureName,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,ppdisp)	\
    (This)->lpVtbl -> ParseProcedureText(This,pstrCode,pstrFormalParams,pstrProcedureName,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,ppdisp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptParseProcedure_ParseProcedureText_Proxy( 
    IActiveScriptParseProcedure * This,
    /* [in] */ LPCOLESTR pstrCode,
    /* [in] */ LPCOLESTR pstrFormalParams,
    /* [in] */ LPCOLESTR pstrProcedureName,
    /* [in] */ LPCOLESTR pstrItemName,
    /* [in] */ IUnknown *punkContext,
    /* [in] */ LPCOLESTR pstrDelimiter,
    /* [in] */ DWORD dwSourceContextCookie,
    /* [in] */ ULONG ulStartingLineNumber,
    /* [in] */ DWORD dwFlags,
    /* [out] */ IDispatch **ppdisp);


void __RPC_STUB IActiveScriptParseProcedure_ParseProcedureText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptParseProcedure_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptParseProcedure2_INTERFACE_DEFINED__
#define __IActiveScriptParseProcedure2_INTERFACE_DEFINED__

/* interface IActiveScriptParseProcedure2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptParseProcedure2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("71EE5B20-FB04-11d1-B3A8-00A0C911E8B2")
    IActiveScriptParseProcedure2 : public IActiveScriptParseProcedure
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptParseProcedure2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptParseProcedure2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptParseProcedure2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptParseProcedure2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseProcedureText )( 
            IActiveScriptParseProcedure2 * This,
            /* [in] */ LPCOLESTR pstrCode,
            /* [in] */ LPCOLESTR pstrFormalParams,
            /* [in] */ LPCOLESTR pstrProcedureName,
            /* [in] */ LPCOLESTR pstrItemName,
            /* [in] */ IUnknown *punkContext,
            /* [in] */ LPCOLESTR pstrDelimiter,
            /* [in] */ DWORD dwSourceContextCookie,
            /* [in] */ ULONG ulStartingLineNumber,
            /* [in] */ DWORD dwFlags,
            /* [out] */ IDispatch **ppdisp);
        
        END_INTERFACE
    } IActiveScriptParseProcedure2Vtbl;

    interface IActiveScriptParseProcedure2
    {
        CONST_VTBL struct IActiveScriptParseProcedure2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptParseProcedure2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptParseProcedure2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptParseProcedure2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptParseProcedure2_ParseProcedureText(This,pstrCode,pstrFormalParams,pstrProcedureName,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,ppdisp)	\
    (This)->lpVtbl -> ParseProcedureText(This,pstrCode,pstrFormalParams,pstrProcedureName,pstrItemName,punkContext,pstrDelimiter,dwSourceContextCookie,ulStartingLineNumber,dwFlags,ppdisp)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IActiveScriptParseProcedure2_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptEncode_INTERFACE_DEFINED__
#define __IActiveScriptEncode_INTERFACE_DEFINED__

/* interface IActiveScriptEncode */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptEncode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB1A2AE3-A4F9-11cf-8F20-00805F2CD064")
    IActiveScriptEncode : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EncodeSection( 
            /* [in] */ LPCOLESTR pchIn,
            /* [in] */ DWORD cchIn,
            /* [out][in] */ LPOLESTR pchOut,
            /* [in] */ DWORD cchOut,
            /* [out][in] */ DWORD *pcchRet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DecodeScript( 
            /* [in] */ LPCOLESTR pchIn,
            /* [in] */ DWORD cchIn,
            /* [out][in] */ LPOLESTR pchOut,
            /* [in] */ DWORD cchOut,
            /* [out][in] */ DWORD *pcchRet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEncodeProgId( 
            /* [out][in] */ BSTR *pbstrOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptEncodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptEncode * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptEncode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptEncode * This);
        
        HRESULT ( STDMETHODCALLTYPE *EncodeSection )( 
            IActiveScriptEncode * This,
            /* [in] */ LPCOLESTR pchIn,
            /* [in] */ DWORD cchIn,
            /* [out][in] */ LPOLESTR pchOut,
            /* [in] */ DWORD cchOut,
            /* [out][in] */ DWORD *pcchRet);
        
        HRESULT ( STDMETHODCALLTYPE *DecodeScript )( 
            IActiveScriptEncode * This,
            /* [in] */ LPCOLESTR pchIn,
            /* [in] */ DWORD cchIn,
            /* [out][in] */ LPOLESTR pchOut,
            /* [in] */ DWORD cchOut,
            /* [out][in] */ DWORD *pcchRet);
        
        HRESULT ( STDMETHODCALLTYPE *GetEncodeProgId )( 
            IActiveScriptEncode * This,
            /* [out][in] */ BSTR *pbstrOut);
        
        END_INTERFACE
    } IActiveScriptEncodeVtbl;

    interface IActiveScriptEncode
    {
        CONST_VTBL struct IActiveScriptEncodeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptEncode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptEncode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptEncode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptEncode_EncodeSection(This,pchIn,cchIn,pchOut,cchOut,pcchRet)	\
    (This)->lpVtbl -> EncodeSection(This,pchIn,cchIn,pchOut,cchOut,pcchRet)

#define IActiveScriptEncode_DecodeScript(This,pchIn,cchIn,pchOut,cchOut,pcchRet)	\
    (This)->lpVtbl -> DecodeScript(This,pchIn,cchIn,pchOut,cchOut,pcchRet)

#define IActiveScriptEncode_GetEncodeProgId(This,pbstrOut)	\
    (This)->lpVtbl -> GetEncodeProgId(This,pbstrOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptEncode_EncodeSection_Proxy( 
    IActiveScriptEncode * This,
    /* [in] */ LPCOLESTR pchIn,
    /* [in] */ DWORD cchIn,
    /* [out][in] */ LPOLESTR pchOut,
    /* [in] */ DWORD cchOut,
    /* [out][in] */ DWORD *pcchRet);


void __RPC_STUB IActiveScriptEncode_EncodeSection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptEncode_DecodeScript_Proxy( 
    IActiveScriptEncode * This,
    /* [in] */ LPCOLESTR pchIn,
    /* [in] */ DWORD cchIn,
    /* [out][in] */ LPOLESTR pchOut,
    /* [in] */ DWORD cchOut,
    /* [out][in] */ DWORD *pcchRet);


void __RPC_STUB IActiveScriptEncode_DecodeScript_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptEncode_GetEncodeProgId_Proxy( 
    IActiveScriptEncode * This,
    /* [out][in] */ BSTR *pbstrOut);


void __RPC_STUB IActiveScriptEncode_GetEncodeProgId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptEncode_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptHostEncode_INTERFACE_DEFINED__
#define __IActiveScriptHostEncode_INTERFACE_DEFINED__

/* interface IActiveScriptHostEncode */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptHostEncode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BEE9B76E-CFE3-11d1-B747-00C04FC2B085")
    IActiveScriptHostEncode : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EncodeScriptHostFile( 
            /* [in] */ BSTR bstrInFile,
            /* [out][in] */ BSTR *pbstrOutFile,
            /* [in] */ unsigned long cFlags,
            /* [in] */ BSTR bstrDefaultLang) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptHostEncodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptHostEncode * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptHostEncode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptHostEncode * This);
        
        HRESULT ( STDMETHODCALLTYPE *EncodeScriptHostFile )( 
            IActiveScriptHostEncode * This,
            /* [in] */ BSTR bstrInFile,
            /* [out][in] */ BSTR *pbstrOutFile,
            /* [in] */ unsigned long cFlags,
            /* [in] */ BSTR bstrDefaultLang);
        
        END_INTERFACE
    } IActiveScriptHostEncodeVtbl;

    interface IActiveScriptHostEncode
    {
        CONST_VTBL struct IActiveScriptHostEncodeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptHostEncode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptHostEncode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptHostEncode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptHostEncode_EncodeScriptHostFile(This,bstrInFile,pbstrOutFile,cFlags,bstrDefaultLang)	\
    (This)->lpVtbl -> EncodeScriptHostFile(This,bstrInFile,pbstrOutFile,cFlags,bstrDefaultLang)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptHostEncode_EncodeScriptHostFile_Proxy( 
    IActiveScriptHostEncode * This,
    /* [in] */ BSTR bstrInFile,
    /* [out][in] */ BSTR *pbstrOutFile,
    /* [in] */ unsigned long cFlags,
    /* [in] */ BSTR bstrDefaultLang);


void __RPC_STUB IActiveScriptHostEncode_EncodeScriptHostFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptHostEncode_INTERFACE_DEFINED__ */


#ifndef __IBindEventHandler_INTERFACE_DEFINED__
#define __IBindEventHandler_INTERFACE_DEFINED__

/* interface IBindEventHandler */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IBindEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("63CDBCB0-C1B1-11d0-9336-00A0C90DCAA9")
    IBindEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BindHandler( 
            /* [in] */ LPCOLESTR pstrEvent,
            /* [in] */ IDispatch *pdisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBindEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBindEventHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBindEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBindEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *BindHandler )( 
            IBindEventHandler * This,
            /* [in] */ LPCOLESTR pstrEvent,
            /* [in] */ IDispatch *pdisp);
        
        END_INTERFACE
    } IBindEventHandlerVtbl;

    interface IBindEventHandler
    {
        CONST_VTBL struct IBindEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindEventHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBindEventHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBindEventHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBindEventHandler_BindHandler(This,pstrEvent,pdisp)	\
    (This)->lpVtbl -> BindHandler(This,pstrEvent,pdisp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBindEventHandler_BindHandler_Proxy( 
    IBindEventHandler * This,
    /* [in] */ LPCOLESTR pstrEvent,
    /* [in] */ IDispatch *pdisp);


void __RPC_STUB IBindEventHandler_BindHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBindEventHandler_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptStats_INTERFACE_DEFINED__
#define __IActiveScriptStats_INTERFACE_DEFINED__

/* interface IActiveScriptStats */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptStats;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B8DA6310-E19B-11d0-933C-00A0C90DCAA9")
    IActiveScriptStats : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetStat( 
            /* [in] */ DWORD stid,
            /* [out] */ ULONG *pluHi,
            /* [out] */ ULONG *pluLo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStatEx( 
            /* [in] */ REFGUID guid,
            /* [out] */ ULONG *pluHi,
            /* [out] */ ULONG *pluLo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetStats( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptStatsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptStats * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptStats * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptStats * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetStat )( 
            IActiveScriptStats * This,
            /* [in] */ DWORD stid,
            /* [out] */ ULONG *pluHi,
            /* [out] */ ULONG *pluLo);
        
        HRESULT ( STDMETHODCALLTYPE *GetStatEx )( 
            IActiveScriptStats * This,
            /* [in] */ REFGUID guid,
            /* [out] */ ULONG *pluHi,
            /* [out] */ ULONG *pluLo);
        
        HRESULT ( STDMETHODCALLTYPE *ResetStats )( 
            IActiveScriptStats * This);
        
        END_INTERFACE
    } IActiveScriptStatsVtbl;

    interface IActiveScriptStats
    {
        CONST_VTBL struct IActiveScriptStatsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptStats_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptStats_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptStats_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptStats_GetStat(This,stid,pluHi,pluLo)	\
    (This)->lpVtbl -> GetStat(This,stid,pluHi,pluLo)

#define IActiveScriptStats_GetStatEx(This,guid,pluHi,pluLo)	\
    (This)->lpVtbl -> GetStatEx(This,guid,pluHi,pluLo)

#define IActiveScriptStats_ResetStats(This)	\
    (This)->lpVtbl -> ResetStats(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptStats_GetStat_Proxy( 
    IActiveScriptStats * This,
    /* [in] */ DWORD stid,
    /* [out] */ ULONG *pluHi,
    /* [out] */ ULONG *pluLo);


void __RPC_STUB IActiveScriptStats_GetStat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptStats_GetStatEx_Proxy( 
    IActiveScriptStats * This,
    /* [in] */ REFGUID guid,
    /* [out] */ ULONG *pluHi,
    /* [out] */ ULONG *pluLo);


void __RPC_STUB IActiveScriptStats_GetStatEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptStats_ResetStats_Proxy( 
    IActiveScriptStats * This);


void __RPC_STUB IActiveScriptStats_ResetStats_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptStats_INTERFACE_DEFINED__ */


#ifndef __IActiveScriptProperty_INTERFACE_DEFINED__
#define __IActiveScriptProperty_INTERFACE_DEFINED__

/* interface IActiveScriptProperty */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IActiveScriptProperty;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4954E0D0-FBC7-11D1-8410-006008C3FBFC")
    IActiveScriptProperty : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ DWORD dwProperty,
            /* [in] */ VARIANT *pvarIndex,
            /* [out] */ VARIANT *pvarValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetProperty( 
            /* [in] */ DWORD dwProperty,
            /* [in] */ VARIANT *pvarIndex,
            /* [in] */ VARIANT *pvarValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActiveScriptPropertyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActiveScriptProperty * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActiveScriptProperty * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActiveScriptProperty * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IActiveScriptProperty * This,
            /* [in] */ DWORD dwProperty,
            /* [in] */ VARIANT *pvarIndex,
            /* [out] */ VARIANT *pvarValue);
        
        HRESULT ( STDMETHODCALLTYPE *SetProperty )( 
            IActiveScriptProperty * This,
            /* [in] */ DWORD dwProperty,
            /* [in] */ VARIANT *pvarIndex,
            /* [in] */ VARIANT *pvarValue);
        
        END_INTERFACE
    } IActiveScriptPropertyVtbl;

    interface IActiveScriptProperty
    {
        CONST_VTBL struct IActiveScriptPropertyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActiveScriptProperty_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActiveScriptProperty_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActiveScriptProperty_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActiveScriptProperty_GetProperty(This,dwProperty,pvarIndex,pvarValue)	\
    (This)->lpVtbl -> GetProperty(This,dwProperty,pvarIndex,pvarValue)

#define IActiveScriptProperty_SetProperty(This,dwProperty,pvarIndex,pvarValue)	\
    (This)->lpVtbl -> SetProperty(This,dwProperty,pvarIndex,pvarValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActiveScriptProperty_GetProperty_Proxy( 
    IActiveScriptProperty * This,
    /* [in] */ DWORD dwProperty,
    /* [in] */ VARIANT *pvarIndex,
    /* [out] */ VARIANT *pvarValue);


void __RPC_STUB IActiveScriptProperty_GetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActiveScriptProperty_SetProperty_Proxy( 
    IActiveScriptProperty * This,
    /* [in] */ DWORD dwProperty,
    /* [in] */ VARIANT *pvarIndex,
    /* [in] */ VARIANT *pvarValue);


void __RPC_STUB IActiveScriptProperty_SetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActiveScriptProperty_INTERFACE_DEFINED__ */


#ifndef __ITridentEventSink_INTERFACE_DEFINED__
#define __ITridentEventSink_INTERFACE_DEFINED__

/* interface ITridentEventSink */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITridentEventSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1DC9CA50-06EF-11d2-8415-006008C3FBFC")
    ITridentEventSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FireEvent( 
            /* [in] */ LPCOLESTR pstrEvent,
            /* [in] */ DISPPARAMS *pdp,
            /* [out] */ VARIANT *pvarRes,
            /* [out] */ EXCEPINFO *pei) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITridentEventSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITridentEventSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITridentEventSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITridentEventSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *FireEvent )( 
            ITridentEventSink * This,
            /* [in] */ LPCOLESTR pstrEvent,
            /* [in] */ DISPPARAMS *pdp,
            /* [out] */ VARIANT *pvarRes,
            /* [out] */ EXCEPINFO *pei);
        
        END_INTERFACE
    } ITridentEventSinkVtbl;

    interface ITridentEventSink
    {
        CONST_VTBL struct ITridentEventSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITridentEventSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITridentEventSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITridentEventSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITridentEventSink_FireEvent(This,pstrEvent,pdp,pvarRes,pei)	\
    (This)->lpVtbl -> FireEvent(This,pstrEvent,pdp,pvarRes,pei)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITridentEventSink_FireEvent_Proxy( 
    ITridentEventSink * This,
    /* [in] */ LPCOLESTR pstrEvent,
    /* [in] */ DISPPARAMS *pdp,
    /* [out] */ VARIANT *pvarRes,
    /* [out] */ EXCEPINFO *pei);


void __RPC_STUB ITridentEventSink_FireEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITridentEventSink_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_activscp_0261 */
/* [local] */ 


#endif  // __ActivScp_h



extern RPC_IF_HANDLE __MIDL_itf_activscp_0261_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_activscp_0261_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


