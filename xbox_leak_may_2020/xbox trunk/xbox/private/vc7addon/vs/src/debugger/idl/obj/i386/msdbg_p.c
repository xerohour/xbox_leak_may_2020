
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Apr 29 23:13:59 2002
 */
/* Compiler settings for msdbg.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "msdbg.h"

#define TYPE_FORMAT_STRING_SIZE   5453                              
#define PROC_FORMAT_STRING_SIZE   9271                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMachine2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMachine2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMachineEx2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMachineEx2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPortSupplier2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPortSupplier2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPort2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPort2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPortRequest2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPortRequest2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPortNotify2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPortNotify2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPortEvents2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPortEvents2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMDMUtil2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMDMUtil2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugSession2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugSession2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEngine2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEngine2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEngineLaunch2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEngineLaunch2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEventCallback2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEventCallback2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugSessionCreateEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugSessionCreateEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugSessionDestroyEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugSessionDestroyEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEngineCreateEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEngineCreateEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProcessCreateEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProcessCreateEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProcessDestroyEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProcessDestroyEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgramCreateEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgramCreateEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgramDestroyEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgramDestroyEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugThreadCreateEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugThreadCreateEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugThreadDestroyEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugThreadDestroyEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugLoadCompleteEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugLoadCompleteEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEntryPointEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEntryPointEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugStepCompleteEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugStepCompleteEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugCanStopEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugCanStopEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakpointEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugExceptionEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugExceptionEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugOutputStringEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugOutputStringEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugModuleLoadEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugModuleLoadEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPropertyCreateEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPropertyCreateEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPropertyDestroyEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPropertyDestroyEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakpointBoundEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointBoundEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakpointUnboundEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointUnboundEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakpointErrorEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointErrorEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugExpressionEvaluationCompleteEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugExpressionEvaluationCompleteEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugReturnValueEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugReturnValueEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugFindSymbolEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugFindSymbolEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugNoSymbolsEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugNoSymbolsEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgramNameChangedEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgramNameChangedEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugThreadNameChangedEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugThreadNameChangedEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMessageEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMessageEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugActivateDocumentEvent2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugActivateDocumentEvent2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProcess2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProcess2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgram2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgram2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugEngineProgram2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugEngineProgram2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgramHost2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgramHost2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgramNode2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgramNode2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProgramEngines2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProgramEngines2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugThread2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugThread2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugLogicalThread2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugLogicalThread2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugProperty2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugProperty2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugReference2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugReference2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugStackFrame2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugStackFrame2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMemoryContext2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMemoryContext2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugCodeContext2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugCodeContext2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugMemoryBytes2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugMemoryBytes2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDisassemblyStream2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDisassemblyStream2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDocumentContext2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDocumentContext2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugExpressionContext2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugExpressionContext2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakpointRequest2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointRequest2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBreakpointResolution2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointResolution2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugErrorBreakpointResolution2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugErrorBreakpointResolution2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugBoundBreakpoint2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugBoundBreakpoint2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugPendingBreakpoint2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugPendingBreakpoint2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugErrorBreakpoint2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugErrorBreakpoint2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugExpression2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugExpression2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugModule2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugModule2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugModuleManaged_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugModuleManaged_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDocument2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDocument2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDocumentText2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDocumentText2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDocumentPosition2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDocumentPosition2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDocumentPositionOffset2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDocumentPositionOffset2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugFunctionPosition2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugFunctionPosition2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugDocumentTextEvents2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugDocumentTextEvents2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IDebugQueryEngine2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IDebugQueryEngine2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugProcesses2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugProcesses2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugPrograms2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugPrograms2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugThreads2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugThreads2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugStackFrames2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugStackFrames2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugCodeContexts2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugCodeContexts2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugBoundBreakpoints2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugBoundBreakpoints2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugPendingBreakpoints2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugPendingBreakpoints2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugErrorBreakpoints2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugErrorBreakpoints2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugMachines2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugMachines2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugExceptionInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugExceptionInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugFrameInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugFrameInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumCodePaths2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumCodePaths2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugModules2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugModules2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugPortSuppliers2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugPortSuppliers2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugPorts2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugPorts2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugPropertyInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugPropertyInfo2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEnumDebugReferenceInfo2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEnumDebugReferenceInfo2_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure GetInfo */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x3 ),	/* 3 */
/*  8 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter Fields */

/* 16 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pMachineInfo */

/* 22 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	NdrFcShort( 0x28 ),	/* Type Offset=40 */

	/* Return value */

/* 28 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 30 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 32 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */


	/* Procedure GetName */


	/* Procedure GetExceptionDescription */


	/* Procedure GetName */

/* 34 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 36 */	NdrFcLong( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x4 ),	/* 4 */
/* 42 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 44 */	NdrFcShort( 0x0 ),	/* 0 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pbstrName */


	/* Parameter pbstrName */


	/* Parameter pbstrDescription */


	/* Parameter pbstrName */

/* 50 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 52 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 54 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 56 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 58 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 60 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPortSupplier */

/* 62 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 64 */	NdrFcLong( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x5 ),	/* 5 */
/* 70 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 72 */	NdrFcShort( 0x44 ),	/* 68 */
/* 74 */	NdrFcShort( 0x8 ),	/* 8 */
/* 76 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter guidPortSupplier */

/* 78 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 80 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 82 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter ppPortSupplier */

/* 84 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 86 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 88 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 90 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 92 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 94 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPort */

/* 96 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 98 */	NdrFcLong( 0x0 ),	/* 0 */
/* 102 */	NdrFcShort( 0x6 ),	/* 6 */
/* 104 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 106 */	NdrFcShort( 0x44 ),	/* 68 */
/* 108 */	NdrFcShort( 0x8 ),	/* 8 */
/* 110 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter guidPort */

/* 112 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 114 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 116 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter ppPort */

/* 118 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 120 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 122 */	NdrFcShort( 0x68 ),	/* Type Offset=104 */

	/* Return value */

/* 124 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 126 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPorts */

/* 130 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 132 */	NdrFcLong( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x7 ),	/* 7 */
/* 138 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x8 ),	/* 8 */
/* 144 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 146 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 148 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 150 */	NdrFcShort( 0x7e ),	/* Type Offset=126 */

	/* Return value */

/* 152 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 154 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPortSuppliers */

/* 158 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 166 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x8 ),	/* 8 */
/* 172 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 174 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 178 */	NdrFcShort( 0x94 ),	/* Type Offset=148 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMachineUtilities */

/* 186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x9 ),	/* 9 */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 200 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppUtil */

/* 202 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 204 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 206 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 210 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnableAutoAttachOnProgramCreate */

/* 214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0x3 ),	/* 3 */
/* 222 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 224 */	NdrFcShort( 0x44 ),	/* 68 */
/* 226 */	NdrFcShort( 0x24 ),	/* 36 */
/* 228 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pszProcessNames */

/* 230 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 232 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 234 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter guidEngine */

/* 236 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 238 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 240 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter pszSessionId */

/* 242 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 244 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 246 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter pdwCookie */

/* 248 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 250 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 254 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 256 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure Skip */


	/* Procedure SetResponse */


	/* Procedure CanStop */


	/* Procedure DisableAutoAttachOnEvent */

/* 260 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 266 */	NdrFcShort( 0x4 ),	/* 4 */
/* 268 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 274 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter celt */


	/* Parameter dwResponse */


	/* Parameter fCanStop */


	/* Parameter dwCookie */

/* 276 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 278 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 280 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 282 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 284 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 286 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPortSupplierEx */

/* 288 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 290 */	NdrFcLong( 0x0 ),	/* 0 */
/* 294 */	NdrFcShort( 0x5 ),	/* 5 */
/* 296 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 298 */	NdrFcShort( 0x44 ),	/* 68 */
/* 300 */	NdrFcShort( 0x8 ),	/* 8 */
/* 302 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter wstrRegistryRoot */

/* 304 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 306 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 308 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter guidPortSupplier */

/* 310 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 312 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 314 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter ppPortSupplier */

/* 316 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 318 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 320 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 324 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPortEx */

/* 328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 334 */	NdrFcShort( 0x6 ),	/* 6 */
/* 336 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 338 */	NdrFcShort( 0x44 ),	/* 68 */
/* 340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 342 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter wstrRegistryRoot */

/* 344 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 346 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 348 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter guidPort */

/* 350 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 352 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 354 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter ppPort */

/* 356 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 358 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 360 */	NdrFcShort( 0x68 ),	/* Type Offset=104 */

	/* Return value */

/* 362 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 364 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPortsEx */

/* 368 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x7 ),	/* 7 */
/* 376 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 380 */	NdrFcShort( 0x8 ),	/* 8 */
/* 382 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter wstrRegistryRoot */

/* 384 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 388 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter ppEnum */

/* 390 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 394 */	NdrFcShort( 0x7e ),	/* Type Offset=126 */

	/* Return value */

/* 396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 398 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPortSuppliersEx */

/* 402 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 408 */	NdrFcShort( 0x8 ),	/* 8 */
/* 410 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 416 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter wstrRegistryRoot */

/* 418 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 420 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 422 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter ppEnum */

/* 424 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 426 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 428 */	NdrFcShort( 0x94 ),	/* Type Offset=148 */

	/* Return value */

/* 430 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 432 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetFunctionName */


	/* Procedure GetFileName */


	/* Procedure GetName */


	/* Procedure GetName */


	/* Procedure GetProgramName */


	/* Procedure GetString */


	/* Procedure GetName */


	/* Procedure GetPortName */


	/* Procedure GetPortName */


	/* Procedure GetPortSupplierName */

/* 436 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 438 */	NdrFcLong( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x3 ),	/* 3 */
/* 444 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 446 */	NdrFcShort( 0x0 ),	/* 0 */
/* 448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 450 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pbstrFunctionName */


	/* Parameter pbstrFileName */


	/* Parameter pbstrName */


	/* Parameter pbstrName */


	/* Parameter pbstrProgramName */


	/* Parameter pbstrString */


	/* Parameter pbstrName */


	/* Parameter pbstrPortName */


	/* Parameter pbstrName */


	/* Parameter pbstrName */

/* 452 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 454 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 456 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 458 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentClassId */


	/* Procedure GetPortId */


	/* Procedure GetPortSupplierId */

/* 464 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 466 */	NdrFcLong( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x4 ),	/* 4 */
/* 472 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x4c ),	/* 76 */
/* 478 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pclsid */


	/* Parameter pguidPort */


	/* Parameter pguidPortSupplier */

/* 480 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 482 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 484 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 486 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 488 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPort */

/* 492 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 494 */	NdrFcLong( 0x0 ),	/* 0 */
/* 498 */	NdrFcShort( 0x5 ),	/* 5 */
/* 500 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 502 */	NdrFcShort( 0x44 ),	/* 68 */
/* 504 */	NdrFcShort( 0x8 ),	/* 8 */
/* 506 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter guidPort */

/* 508 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 510 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 512 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter ppPort */

/* 514 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 516 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 518 */	NdrFcShort( 0x68 ),	/* Type Offset=104 */

	/* Return value */

/* 520 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 522 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 524 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPorts */

/* 526 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 528 */	NdrFcLong( 0x0 ),	/* 0 */
/* 532 */	NdrFcShort( 0x6 ),	/* 6 */
/* 534 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 536 */	NdrFcShort( 0x0 ),	/* 0 */
/* 538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 540 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 542 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 544 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 546 */	NdrFcShort( 0x7e ),	/* Type Offset=126 */

	/* Return value */

/* 548 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 550 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 552 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Terminate */


	/* Procedure CanAddPort */

/* 554 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x7 ),	/* 7 */
/* 562 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 564 */	NdrFcShort( 0x0 ),	/* 0 */
/* 566 */	NdrFcShort( 0x8 ),	/* 8 */
/* 568 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 570 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 572 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPort */

/* 576 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 578 */	NdrFcLong( 0x0 ),	/* 0 */
/* 582 */	NdrFcShort( 0x8 ),	/* 8 */
/* 584 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 586 */	NdrFcShort( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x8 ),	/* 8 */
/* 590 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pRequest */

/* 592 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 594 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 596 */	NdrFcShort( 0xcc ),	/* Type Offset=204 */

	/* Parameter ppPort */

/* 598 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 600 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 602 */	NdrFcShort( 0x68 ),	/* Type Offset=104 */

	/* Return value */

/* 604 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 606 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 608 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemovePort */

/* 610 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 612 */	NdrFcLong( 0x0 ),	/* 0 */
/* 616 */	NdrFcShort( 0x9 ),	/* 9 */
/* 618 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x8 ),	/* 8 */
/* 624 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pPort */

/* 626 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 628 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 630 */	NdrFcShort( 0x6c ),	/* Type Offset=108 */

	/* Return value */

/* 632 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 634 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPortRequest */

/* 638 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 640 */	NdrFcLong( 0x0 ),	/* 0 */
/* 644 */	NdrFcShort( 0x5 ),	/* 5 */
/* 646 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 648 */	NdrFcShort( 0x0 ),	/* 0 */
/* 650 */	NdrFcShort( 0x8 ),	/* 8 */
/* 652 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppRequest */

/* 654 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 656 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 658 */	NdrFcShort( 0xde ),	/* Type Offset=222 */

	/* Return value */

/* 660 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 662 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPortSupplier */

/* 666 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 668 */	NdrFcLong( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0x6 ),	/* 6 */
/* 674 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 676 */	NdrFcShort( 0x0 ),	/* 0 */
/* 678 */	NdrFcShort( 0x8 ),	/* 8 */
/* 680 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppSupplier */

/* 682 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 684 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 686 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */

	/* Return value */

/* 688 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 690 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 692 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProcess */

/* 694 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 696 */	NdrFcLong( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x7 ),	/* 7 */
/* 702 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	NdrFcShort( 0x8 ),	/* 8 */
/* 708 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter ProcessId */

/* 710 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 712 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 714 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Parameter ppProcess */

/* 716 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 718 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 720 */	NdrFcShort( 0xf6 ),	/* Type Offset=246 */

	/* Return value */

/* 722 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 724 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 726 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumProcesses */

/* 728 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 730 */	NdrFcLong( 0x0 ),	/* 0 */
/* 734 */	NdrFcShort( 0x8 ),	/* 8 */
/* 736 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 738 */	NdrFcShort( 0x0 ),	/* 0 */
/* 740 */	NdrFcShort( 0x8 ),	/* 8 */
/* 742 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 744 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 746 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 748 */	NdrFcShort( 0x10c ),	/* Type Offset=268 */

	/* Return value */

/* 750 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 752 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 754 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddProgramNode */

/* 756 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 758 */	NdrFcLong( 0x0 ),	/* 0 */
/* 762 */	NdrFcShort( 0x3 ),	/* 3 */
/* 764 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */
/* 768 */	NdrFcShort( 0x8 ),	/* 8 */
/* 770 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProgramNode */

/* 772 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 774 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 776 */	NdrFcShort( 0x122 ),	/* Type Offset=290 */

	/* Return value */

/* 778 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 780 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 782 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveProgramNode */

/* 784 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 786 */	NdrFcLong( 0x0 ),	/* 0 */
/* 790 */	NdrFcShort( 0x4 ),	/* 4 */
/* 792 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 794 */	NdrFcShort( 0x0 ),	/* 0 */
/* 796 */	NdrFcShort( 0x8 ),	/* 8 */
/* 798 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProgramNode */

/* 800 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 802 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 804 */	NdrFcShort( 0x122 ),	/* Type Offset=290 */

	/* Return value */

/* 806 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 808 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Event */

/* 812 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 814 */	NdrFcLong( 0x0 ),	/* 0 */
/* 818 */	NdrFcShort( 0x3 ),	/* 3 */
/* 820 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 822 */	NdrFcShort( 0x44 ),	/* 68 */
/* 824 */	NdrFcShort( 0x8 ),	/* 8 */
/* 826 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter pMachine */

/* 828 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 830 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 832 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Parameter pPort */

/* 834 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 836 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 838 */	NdrFcShort( 0x146 ),	/* Type Offset=326 */

	/* Parameter pProcess */

/* 840 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 842 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 844 */	NdrFcShort( 0x158 ),	/* Type Offset=344 */

	/* Parameter pProgram */

/* 846 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 848 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 850 */	NdrFcShort( 0x16a ),	/* Type Offset=362 */

	/* Parameter pEvent */

/* 852 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 854 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 856 */	NdrFcShort( 0x17c ),	/* Type Offset=380 */

	/* Parameter riidEvent */

/* 858 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 860 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 862 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 864 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 866 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 868 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPIDToIgnore */

/* 870 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 872 */	NdrFcLong( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x3 ),	/* 3 */
/* 878 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 880 */	NdrFcShort( 0x4c ),	/* 76 */
/* 882 */	NdrFcShort( 0x8 ),	/* 8 */
/* 884 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 886 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 888 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 890 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwPid */

/* 892 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 894 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 896 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 898 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 900 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 902 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemovePIDToIgnore */

/* 904 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 906 */	NdrFcLong( 0x0 ),	/* 0 */
/* 910 */	NdrFcShort( 0x4 ),	/* 4 */
/* 912 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 914 */	NdrFcShort( 0x4c ),	/* 76 */
/* 916 */	NdrFcShort( 0x8 ),	/* 8 */
/* 918 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 920 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 922 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 924 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwPid */

/* 926 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 928 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 932 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 934 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 936 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddPIDToDebug */

/* 938 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 940 */	NdrFcLong( 0x0 ),	/* 0 */
/* 944 */	NdrFcShort( 0x5 ),	/* 5 */
/* 946 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 948 */	NdrFcShort( 0x4c ),	/* 76 */
/* 950 */	NdrFcShort( 0x8 ),	/* 8 */
/* 952 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 954 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 956 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 958 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwPid */

/* 960 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 962 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 968 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemovePIDToDebug */

/* 972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0x6 ),	/* 6 */
/* 980 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 982 */	NdrFcShort( 0x4c ),	/* 76 */
/* 984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 986 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 988 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 990 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 992 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwPid */

/* 994 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 996 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1000 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1002 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1004 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetDynamicDebuggingFlags */

/* 1006 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1008 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1012 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1014 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1016 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1020 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 1022 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1024 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1026 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwFlags */

/* 1028 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1030 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1034 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1036 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1038 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDynamicDebuggingFlags */

/* 1040 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1042 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1048 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1050 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1052 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1054 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 1056 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1058 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1060 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter pdwFlags */

/* 1062 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1064 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1068 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1070 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetDefaultJITServer */

/* 1074 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1076 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1080 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1082 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1084 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1086 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1088 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter clsidJITServer */

/* 1090 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1092 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1094 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 1096 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1098 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProgramId */


	/* Procedure GetDefaultJITServer */

/* 1102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1108 */	NdrFcShort( 0xa ),	/* 10 */
/* 1110 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1114 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1116 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pguidProgramId */


	/* Parameter pClsidJITServer */

/* 1118 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 1120 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1122 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */


	/* Return value */

/* 1124 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1126 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterJITDebugEngines */

/* 1130 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1132 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1136 */	NdrFcShort( 0xb ),	/* 11 */
/* 1138 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1140 */	NdrFcShort( 0x54 ),	/* 84 */
/* 1142 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1144 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter clsidJITServer */

/* 1146 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1148 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1150 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter arrguidEngines */

/* 1152 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1154 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1156 */	NdrFcShort( 0x192 ),	/* Type Offset=402 */

	/* Parameter arrRemoteFlags */

/* 1158 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1160 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1162 */	NdrFcShort( 0x1a0 ),	/* Type Offset=416 */

	/* Parameter celtEngs */

/* 1164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1166 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fRegister */

/* 1170 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1172 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1174 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1176 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1178 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CanDebugPID */

/* 1182 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1184 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1188 */	NdrFcShort( 0xc ),	/* 12 */
/* 1190 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1192 */	NdrFcShort( 0x4c ),	/* 76 */
/* 1194 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1196 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter guidEngine */

/* 1198 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1200 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1202 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter pid */

/* 1204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1206 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1208 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1212 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetName */

/* 1216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1222 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1224 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1226 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1228 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1230 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszName */

/* 1232 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1234 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1236 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Return value */

/* 1238 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1240 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1242 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumProcesses */

/* 1244 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1246 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1250 */	NdrFcShort( 0x5 ),	/* 5 */
/* 1252 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1256 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1258 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 1260 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1262 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1264 */	NdrFcShort( 0x10c ),	/* Type Offset=268 */

	/* Return value */

/* 1266 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1268 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Launch */

/* 1272 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1274 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1278 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1280 */	NdrFcShort( 0x48 ),	/* x86 Stack size/offset = 72 */
/* 1282 */	NdrFcShort( 0x6c ),	/* 108 */
/* 1284 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1286 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x11,		/* 17 */

	/* Parameter pszMachine */

/* 1288 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1290 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1292 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pPort */

/* 1294 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1296 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1298 */	NdrFcShort( 0x146 ),	/* Type Offset=326 */

	/* Parameter pszExe */

/* 1300 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1302 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1304 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pszArgs */

/* 1306 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1308 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1310 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pszDir */

/* 1312 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1314 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1316 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter bstrEnv */

/* 1318 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1320 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1322 */	NdrFcShort( 0x1b6 ),	/* Type Offset=438 */

	/* Parameter pszOptions */

/* 1324 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1326 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1328 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter dwLaunchFlags */

/* 1330 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1332 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hStdInput */

/* 1336 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1338 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hStdOutput */

/* 1342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1344 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hStdError */

/* 1348 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1350 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 1352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidLaunchingEngine */

/* 1354 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1356 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1358 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter pCallback */

/* 1360 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1362 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 1364 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter rgguidSpecificEngines */

/* 1366 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1368 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 1370 */	NdrFcShort( 0x1d6 ),	/* Type Offset=470 */

	/* Parameter celtSpecificEngines */

/* 1372 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1374 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 1376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppProcess */

/* 1378 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1380 */	NdrFcShort( 0x40 ),	/* x86 Stack size/offset = 64 */
/* 1382 */	NdrFcShort( 0x1e4 ),	/* Type Offset=484 */

	/* Return value */

/* 1384 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1386 */	NdrFcShort( 0x44 ),	/* x86 Stack size/offset = 68 */
/* 1388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterJITServer */

/* 1390 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1398 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1400 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1402 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1404 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter clsidJITServer */

/* 1406 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1408 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1410 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 1412 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1414 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onUpdateDocumentAttributes */


	/* Procedure Enable */


	/* Procedure SetHitCount */


	/* Procedure Terminate */

/* 1418 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1420 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1424 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1426 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1428 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1430 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1432 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter textdocattr */


	/* Parameter fEnable */


	/* Parameter dwHitCount */


	/* Parameter fForce */

/* 1434 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1436 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1440 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1442 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DetachDebugger */


	/* Procedure Detach */


	/* Procedure CanDetach */


	/* Procedure Detach */

/* 1446 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1448 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1452 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1454 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1456 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1458 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1460 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 1462 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1464 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1466 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Detach */


	/* Procedure CauseBreak */

/* 1468 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1470 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1474 */	NdrFcShort( 0xa ),	/* 10 */
/* 1476 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1480 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1482 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 1484 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1486 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1488 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreatePendingBreakpoint */

/* 1490 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1492 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0xb ),	/* 11 */
/* 1498 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1500 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1502 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1504 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pBPRequest */

/* 1506 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1508 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1510 */	NdrFcShort( 0x1e8 ),	/* Type Offset=488 */

	/* Parameter ppPendingBP */

/* 1512 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1514 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1516 */	NdrFcShort( 0x1fa ),	/* Type Offset=506 */

	/* Return value */

/* 1518 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1520 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPendingBreakpoints */

/* 1524 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1526 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1530 */	NdrFcShort( 0xc ),	/* 12 */
/* 1532 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1538 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pProgram */

/* 1540 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1542 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1544 */	NdrFcShort( 0x16a ),	/* Type Offset=362 */

	/* Parameter pszProgram */

/* 1546 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1548 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1550 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter ppEnumBPs */

/* 1552 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1554 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1556 */	NdrFcShort( 0x210 ),	/* Type Offset=528 */

	/* Return value */

/* 1558 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1560 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1562 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumMachines */

/* 1564 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1566 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1570 */	NdrFcShort( 0xd ),	/* 13 */
/* 1572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1574 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1578 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 1580 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1582 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1584 */	NdrFcShort( 0x226 ),	/* Type Offset=550 */

	/* Return value */

/* 1586 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1588 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1590 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AddMachine */

/* 1592 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1594 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1598 */	NdrFcShort( 0xe ),	/* 14 */
/* 1600 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1604 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1606 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszMachine */

/* 1608 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1610 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1612 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter ppMachine */

/* 1614 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1616 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1618 */	NdrFcShort( 0x23c ),	/* Type Offset=572 */

	/* Return value */

/* 1620 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1622 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveMachine */

/* 1626 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1628 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1632 */	NdrFcShort( 0xf ),	/* 15 */
/* 1634 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1636 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1638 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1640 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pMachine */

/* 1642 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1644 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1646 */	NdrFcShort( 0x134 ),	/* Type Offset=308 */

	/* Return value */

/* 1648 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1650 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1652 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShutdownSession */

/* 1654 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1656 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1660 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1662 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1664 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1666 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1668 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 1670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1672 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCodeContexts */

/* 1676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1682 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1684 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1690 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pProgram */

/* 1692 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1696 */	NdrFcShort( 0x16a ),	/* Type Offset=362 */

	/* Parameter pDocPos */

/* 1698 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1702 */	NdrFcShort( 0x240 ),	/* Type Offset=576 */

	/* Parameter ppEnum */

/* 1704 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1706 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1708 */	NdrFcShort( 0x252 ),	/* Type Offset=594 */

	/* Return value */

/* 1710 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1712 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1714 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetException */

/* 1716 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1718 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1722 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1724 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1726 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1728 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1730 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pException */

/* 1732 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1734 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1736 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Return value */

/* 1738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1740 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumSetExceptions */

/* 1744 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1750 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1752 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1754 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1756 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1758 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pProgram */

/* 1760 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1762 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1764 */	NdrFcShort( 0x16a ),	/* Type Offset=362 */

	/* Parameter pszProgram */

/* 1766 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1768 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1770 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter guidType */

/* 1772 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1774 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1776 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter ppEnum */

/* 1778 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1780 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1782 */	NdrFcShort( 0x288 ),	/* Type Offset=648 */

	/* Return value */

/* 1784 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1786 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1788 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveSetException */

/* 1790 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1792 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1796 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1798 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1800 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1802 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1804 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pException */

/* 1806 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1808 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1810 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Return value */

/* 1812 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1814 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1816 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveAllSetExceptions */

/* 1818 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1820 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1824 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1826 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1828 */	NdrFcShort( 0x44 ),	/* 68 */
/* 1830 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1832 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter guidType */

/* 1834 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 1836 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1838 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 1840 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1842 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumDefaultExceptions */

/* 1846 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1848 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1852 */	NdrFcShort( 0x16 ),	/* 22 */
/* 1854 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1856 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1858 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1860 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pParentException */

/* 1862 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1864 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1866 */	NdrFcShort( 0x29e ),	/* Type Offset=670 */

	/* Parameter ppEnum */

/* 1868 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1870 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1872 */	NdrFcShort( 0x288 ),	/* Type Offset=648 */

	/* Return value */

/* 1874 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1876 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1878 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetENCUpdate */

/* 1880 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1882 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1886 */	NdrFcShort( 0x17 ),	/* 23 */
/* 1888 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1890 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1892 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1894 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pProgram */

/* 1896 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1898 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1900 */	NdrFcShort( 0x16a ),	/* Type Offset=362 */

	/* Parameter ppUpdate */

/* 1902 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 1904 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1906 */	NdrFcShort( 0x2a2 ),	/* Type Offset=674 */

	/* Return value */

/* 1908 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1910 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1912 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetLocale */

/* 1914 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1916 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1920 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1922 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1924 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1926 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1928 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter wLangID */

/* 1930 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1932 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1934 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 1936 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1938 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetRegistryRoot */

/* 1942 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 1944 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1948 */	NdrFcShort( 0x19 ),	/* 25 */
/* 1950 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1956 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszRegistryRoot */

/* 1958 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 1960 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1962 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Return value */

/* 1964 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1966 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsAlive */

/* 1970 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1972 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1976 */	NdrFcShort( 0x1a ),	/* 26 */
/* 1978 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1982 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1984 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 1986 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1988 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1990 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ClearAllSessionThreadStackFrames */

/* 1992 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1994 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1998 */	NdrFcShort( 0x1b ),	/* 27 */
/* 2000 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2002 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2004 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2006 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 2008 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2010 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSessionId */

/* 2014 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2016 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2020 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2022 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2024 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2026 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2028 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter pCallback */

/* 2030 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2032 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2034 */	NdrFcShort( 0x1c0 ),	/* Type Offset=448 */

	/* Parameter rgguidSpecificEngines */

/* 2036 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2038 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2040 */	NdrFcShort( 0x2b8 ),	/* Type Offset=696 */

	/* Parameter celtSpecificEngines */

/* 2042 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2044 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2046 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszStartPageUrl */

/* 2048 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2050 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2052 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pbstrSessionId */

/* 2054 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 2056 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2058 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 2060 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2062 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetEngineMetric */

/* 2066 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2068 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2072 */	NdrFcShort( 0x1d ),	/* 29 */
/* 2074 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2076 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2078 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2080 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter guidEngine */

/* 2082 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2084 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2086 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter pszMetric */

/* 2088 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2090 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2092 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter varValue */

/* 2094 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2096 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2098 */	NdrFcShort( 0x6a4 ),	/* Type Offset=1700 */

	/* Return value */

/* 2100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2102 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetStoppingModel */

/* 2106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2112 */	NdrFcShort( 0x1e ),	/* 30 */
/* 2114 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2116 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2120 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter dwStoppingModel */

/* 2122 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2124 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2130 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStoppingModel */

/* 2134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2140 */	NdrFcShort( 0x1f ),	/* 31 */
/* 2142 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2146 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2148 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwStoppingModel */

/* 2150 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2152 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2156 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2158 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterSessionWithServer */

/* 2162 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2164 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2168 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2170 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2172 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2174 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2176 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pwszServerName */

/* 2178 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2180 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2182 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Return value */

/* 2184 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2186 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2188 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPrograms */

/* 2190 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2192 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2196 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2198 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2202 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2204 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 2206 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2208 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2210 */	NdrFcShort( 0x6ae ),	/* Type Offset=1710 */

	/* Return value */

/* 2212 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2214 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Attach */

/* 2218 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2220 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2224 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2226 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2228 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2230 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2232 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter rgpPrograms */

/* 2234 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2236 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2238 */	NdrFcShort( 0x6da ),	/* Type Offset=1754 */

	/* Parameter rgpProgramNodes */

/* 2240 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2242 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2244 */	NdrFcShort( 0x702 ),	/* Type Offset=1794 */

	/* Parameter celtPrograms */

/* 2246 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2248 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2250 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCallback */

/* 2252 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2254 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2256 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Parameter dwReason */

/* 2258 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2260 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2262 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2266 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreatePendingBreakpoint */

/* 2270 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2272 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2276 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2278 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2282 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2284 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pBPRequest */

/* 2286 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2288 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2290 */	NdrFcShort( 0x726 ),	/* Type Offset=1830 */

	/* Parameter ppPendingBP */

/* 2292 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2294 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2296 */	NdrFcShort( 0x738 ),	/* Type Offset=1848 */

	/* Return value */

/* 2298 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2300 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetException */

/* 2304 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2306 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2310 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2312 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2314 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2316 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2318 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pException */

/* 2320 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2322 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2324 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Return value */

/* 2326 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2328 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2330 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveSetException */

/* 2332 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2334 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2338 */	NdrFcShort( 0x7 ),	/* 7 */
/* 2340 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2342 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2344 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2346 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pException */

/* 2348 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2350 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2352 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Return value */

/* 2354 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2356 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2358 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RemoveAllSetExceptions */

/* 2360 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2362 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2366 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2368 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2370 */	NdrFcShort( 0x44 ),	/* 68 */
/* 2372 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2374 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter guidType */

/* 2376 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2378 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2380 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 2382 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2384 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2386 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEngineId */

/* 2388 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2390 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2394 */	NdrFcShort( 0x9 ),	/* 9 */
/* 2396 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2400 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2402 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pguidEngine */

/* 2404 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 2406 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2408 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 2410 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2412 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DestroyProgram */

/* 2416 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2422 */	NdrFcShort( 0xa ),	/* 10 */
/* 2424 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2426 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2428 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2430 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProgram */

/* 2432 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2434 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2436 */	NdrFcShort( 0x6c8 ),	/* Type Offset=1736 */

	/* Return value */

/* 2438 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2440 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ContinueFromSynchronousEvent */

/* 2444 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2446 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2450 */	NdrFcShort( 0xb ),	/* 11 */
/* 2452 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2454 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2456 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2458 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pEvent */

/* 2460 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2462 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2464 */	NdrFcShort( 0x74e ),	/* Type Offset=1870 */

	/* Return value */

/* 2466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2468 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetLocale */

/* 2472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2478 */	NdrFcShort( 0xc ),	/* 12 */
/* 2480 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2482 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2486 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter wLangID */

/* 2488 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2490 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2492 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 2494 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2496 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetRegistryRoot */

/* 2500 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2506 */	NdrFcShort( 0xd ),	/* 13 */
/* 2508 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2512 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2514 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszRegistryRoot */

/* 2516 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2518 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2520 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Return value */

/* 2522 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2524 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMetric */

/* 2528 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2530 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2534 */	NdrFcShort( 0xe ),	/* 14 */
/* 2536 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2540 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2542 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszMetric */

/* 2544 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 2546 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2548 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter varValue */

/* 2550 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2552 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2554 */	NdrFcShort( 0x6a4 ),	/* Type Offset=1700 */

	/* Return value */

/* 2556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2558 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CauseBreak */


	/* Procedure CauseBreak */


	/* Procedure CauseBreak */

/* 2562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2568 */	NdrFcShort( 0xf ),	/* 15 */
/* 2570 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2576 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 2578 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LaunchSuspended */

/* 2584 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 2586 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2590 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2592 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 2594 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2596 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2598 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0xe,		/* 14 */

	/* Parameter pszMachine */

/* 2600 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2602 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2604 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pPort */

/* 2606 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2608 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2610 */	NdrFcShort( 0x760 ),	/* Type Offset=1888 */

	/* Parameter pszExe */

/* 2612 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2614 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2616 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pszArgs */

/* 2618 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2620 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2622 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pszDir */

/* 2624 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2626 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2628 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter bstrEnv */

/* 2630 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 2632 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2634 */	NdrFcShort( 0x1b6 ),	/* Type Offset=438 */

	/* Parameter pszOptions */

/* 2636 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2638 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2640 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter dwLaunchFlags */

/* 2642 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2644 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hStdInput */

/* 2648 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2650 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2652 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hStdOutput */

/* 2654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2656 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 2658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter hStdError */

/* 2660 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2662 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 2664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCallback */

/* 2666 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2668 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 2670 */	NdrFcShort( 0x714 ),	/* Type Offset=1812 */

	/* Parameter ppProcess */

/* 2672 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2674 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 2676 */	NdrFcShort( 0x772 ),	/* Type Offset=1906 */

	/* Return value */

/* 2678 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2680 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 2682 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ResumeProcess */

/* 2684 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2686 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2690 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2692 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2694 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2696 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2698 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProcess */

/* 2700 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2702 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2704 */	NdrFcShort( 0x776 ),	/* Type Offset=1910 */

	/* Return value */

/* 2706 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2708 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2710 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CanTerminateProcess */

/* 2712 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2714 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2718 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2720 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2724 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2726 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProcess */

/* 2728 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2730 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2732 */	NdrFcShort( 0x776 ),	/* Type Offset=1910 */

	/* Return value */

/* 2734 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2736 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2738 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure TerminateProcess */

/* 2740 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2742 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2746 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2748 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2750 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2752 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2754 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProcess */

/* 2756 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2758 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2760 */	NdrFcShort( 0x776 ),	/* Type Offset=1910 */

	/* Return value */

/* 2762 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2764 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2766 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Event */

/* 2768 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2770 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2774 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2776 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 2778 */	NdrFcShort( 0x4c ),	/* 76 */
/* 2780 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2782 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter pEngine */

/* 2784 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2786 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2788 */	NdrFcShort( 0x788 ),	/* Type Offset=1928 */

	/* Parameter pProcess */

/* 2790 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2792 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2794 */	NdrFcShort( 0x776 ),	/* Type Offset=1910 */

	/* Parameter pProgram */

/* 2796 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2798 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2800 */	NdrFcShort( 0x6c8 ),	/* Type Offset=1736 */

	/* Parameter pThread */

/* 2802 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2804 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 2806 */	NdrFcShort( 0x79a ),	/* Type Offset=1946 */

	/* Parameter pEvent */

/* 2808 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 2810 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2812 */	NdrFcShort( 0x7ac ),	/* Type Offset=1964 */

	/* Parameter riidEvent */

/* 2814 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 2816 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 2818 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwAttrib */

/* 2820 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 2822 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 2824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 2826 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2828 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 2830 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBreakpointType */


	/* Procedure GetBreakpointType */


	/* Procedure GetLocationType */


	/* Procedure GetReason */


	/* Procedure GetExitCode */


	/* Procedure GetExitCode */


	/* Procedure GetAttributes */

/* 2832 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2834 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2838 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2840 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2842 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2844 */	NdrFcShort( 0x24 ),	/* 36 */
/* 2846 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pBPType */


	/* Parameter pBPType */


	/* Parameter pBPLocationType */


	/* Parameter pcr */


	/* Parameter pdwExit */


	/* Parameter pdwExit */


	/* Parameter pdwAttrib */

/* 2848 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 2850 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2852 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 2854 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2856 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEngine */

/* 2860 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2862 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2866 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2868 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2872 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2874 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pEngine */

/* 2876 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2878 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2880 */	NdrFcShort( 0x7be ),	/* Type Offset=1982 */

	/* Return value */

/* 2882 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2884 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentContext */

/* 2888 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2894 */	NdrFcShort( 0x5 ),	/* 5 */
/* 2896 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2898 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2902 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDocCxt */

/* 2904 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2906 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2908 */	NdrFcShort( 0x7c2 ),	/* Type Offset=1986 */

	/* Return value */

/* 2910 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2912 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCodeContext */

/* 2916 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2918 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2922 */	NdrFcShort( 0x6 ),	/* 6 */
/* 2924 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2928 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2930 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppCodeContext */

/* 2932 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2936 */	NdrFcShort( 0x7d8 ),	/* Type Offset=2008 */

	/* Return value */

/* 2938 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2940 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumBreakpoints */

/* 2944 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2946 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2950 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2952 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2956 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2958 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 2960 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 2962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2964 */	NdrFcShort( 0x7ee ),	/* Type Offset=2030 */

	/* Return value */

/* 2966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2968 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2970 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetException */

/* 2972 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 2974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 2978 */	NdrFcShort( 0x3 ),	/* 3 */
/* 2980 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2984 */	NdrFcShort( 0x8 ),	/* 8 */
/* 2986 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pExceptionInfo */

/* 2988 */	NdrFcShort( 0xa113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=40 */
/* 2990 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2992 */	NdrFcShort( 0x26c ),	/* Type Offset=620 */

	/* Return value */

/* 2994 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 2996 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2998 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure Reset */


	/* Procedure CanPassToDebuggee */

/* 3000 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3002 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3006 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3008 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3012 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3014 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 3016 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3018 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure PassToDebuggee */

/* 3022 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3024 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3028 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3030 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3032 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3034 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3036 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter fPass */

/* 3038 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3040 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3044 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3046 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetModule */

/* 3050 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3052 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3056 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3058 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3060 */	NdrFcShort( 0x1c ),	/* 28 */
/* 3062 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3064 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pModule */

/* 3066 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3068 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3070 */	NdrFcShort( 0x808 ),	/* Type Offset=2056 */

	/* Parameter pbstrDebugMessage */

/* 3072 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 3074 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3076 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter pbLoad */

/* 3078 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 3080 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3082 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3084 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3086 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3088 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDebugProperty */


	/* Procedure GetDebugProperty */

/* 3090 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3092 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3096 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3098 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3100 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3102 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3104 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppProperty */


	/* Parameter ppProperty */

/* 3106 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3110 */	NdrFcShort( 0x826 ),	/* Type Offset=2086 */

	/* Return value */


	/* Return value */

/* 3112 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3114 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPendingBreakpoint */

/* 3118 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3120 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3124 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3126 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3128 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3130 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3132 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppPendingBP */

/* 3134 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3136 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3138 */	NdrFcShort( 0x738 ),	/* Type Offset=1848 */

	/* Return value */

/* 3140 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3142 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3144 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumBoundBreakpoints */

/* 3146 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3148 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3152 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3154 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3158 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3160 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 3162 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3164 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3166 */	NdrFcShort( 0x83c ),	/* Type Offset=2108 */

	/* Return value */

/* 3168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3170 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBreakpoint */

/* 3174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3180 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3188 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppBP */

/* 3190 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3192 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3194 */	NdrFcShort( 0x852 ),	/* Type Offset=2130 */

	/* Return value */

/* 3196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3198 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetState */


	/* Procedure GetReason */

/* 3202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3208 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3210 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3212 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3214 */	NdrFcShort( 0x24 ),	/* 36 */
/* 3216 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pState */


	/* Parameter pdwUnboundReason */

/* 3218 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3220 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 3224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3226 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetErrorBreakpoint */

/* 3230 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3236 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3238 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3242 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3244 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppErrorBP */

/* 3246 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3248 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3250 */	NdrFcShort( 0x868 ),	/* Type Offset=2152 */

	/* Return value */

/* 3252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3254 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExpression */

/* 3258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3264 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3266 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3270 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3272 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppExpr */

/* 3274 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3276 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3278 */	NdrFcShort( 0x87e ),	/* Type Offset=2174 */

	/* Return value */

/* 3280 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3282 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3284 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetResult */

/* 3286 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3288 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3292 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3294 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3296 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3298 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3300 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppResult */

/* 3302 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3304 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3306 */	NdrFcShort( 0x894 ),	/* Type Offset=2196 */

	/* Return value */

/* 3308 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3310 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3312 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetReturnValue */

/* 3314 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3316 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3320 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3322 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3326 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3328 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppReturnValue */

/* 3330 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3334 */	NdrFcShort( 0x894 ),	/* Type Offset=2196 */

	/* Return value */

/* 3336 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3338 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SearchWithPath */

/* 3342 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3344 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3348 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3350 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3354 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3356 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszSymbolSearchURL */

/* 3358 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3360 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3362 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Return value */

/* 3364 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3366 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3368 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMessage */

/* 3370 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3372 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3376 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3378 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 3380 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3382 */	NdrFcShort( 0x5c ),	/* 92 */
/* 3384 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x6,		/* 6 */

	/* Parameter pMessageType */

/* 3386 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3388 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3390 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrMessage */

/* 3392 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 3394 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3396 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter pdwType */

/* 3398 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3400 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrHelpFileName */

/* 3404 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 3406 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3408 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter pdwHelpId */

/* 3410 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 3412 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3414 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 3416 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3418 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3420 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocument */

/* 3422 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3424 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3428 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3430 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3432 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3434 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3436 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDoc */

/* 3438 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3440 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3442 */	NdrFcShort( 0x8aa ),	/* Type Offset=2218 */

	/* Return value */

/* 3444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3446 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentContext */

/* 3450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3456 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3464 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDocContext */

/* 3466 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3468 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3470 */	NdrFcShort( 0x8c0 ),	/* Type Offset=2240 */

	/* Return value */

/* 3472 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3474 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3476 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 3478 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3480 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3484 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3486 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3492 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter Fields */

/* 3494 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3496 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pProcessInfo */

/* 3500 */	NdrFcShort( 0xe113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=56 */
/* 3502 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3504 */	NdrFcShort( 0x8da ),	/* Type Offset=2266 */

	/* Return value */

/* 3506 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3508 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPrograms */

/* 3512 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3514 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3518 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3520 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3522 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3524 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3526 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 3528 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3530 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3532 */	NdrFcShort( 0x8fe ),	/* Type Offset=2302 */

	/* Return value */

/* 3534 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3536 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3538 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */

/* 3540 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3542 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3546 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3548 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3550 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3552 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3554 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter gnType */

/* 3556 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3558 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrName */

/* 3562 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 3564 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3566 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 3568 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3572 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMachine */

/* 3574 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3576 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3580 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3582 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3584 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3586 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3588 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMachine */

/* 3590 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3592 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3594 */	NdrFcShort( 0x914 ),	/* Type Offset=2324 */

	/* Return value */

/* 3596 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3598 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3600 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Attach */

/* 3602 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3604 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3608 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3610 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 3612 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3614 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3616 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pCallback */

/* 3618 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3620 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3622 */	NdrFcShort( 0x92a ),	/* Type Offset=2346 */

	/* Parameter rgguidSpecificEngines */

/* 3624 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 3626 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3628 */	NdrFcShort( 0x2bc ),	/* Type Offset=700 */

	/* Parameter celtSpecificEngines */

/* 3630 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 3632 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3634 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rghrEngineAttach */

/* 3636 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 3638 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3640 */	NdrFcShort( 0x944 ),	/* Type Offset=2372 */

	/* Return value */

/* 3642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3644 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPhysicalProcessId */

/* 3648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3654 */	NdrFcShort( 0xb ),	/* 11 */
/* 3656 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3662 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProcessId */

/* 3664 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 3666 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3668 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Return value */

/* 3670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3672 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProcessId */

/* 3676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3682 */	NdrFcShort( 0xc ),	/* 12 */
/* 3684 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3688 */	NdrFcShort( 0x4c ),	/* 76 */
/* 3690 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pguidProcessId */

/* 3692 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 3694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3696 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 3698 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetAttachedSessionName */

/* 3704 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3710 */	NdrFcShort( 0xd ),	/* 13 */
/* 3712 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3716 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3718 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pbstrSessionName */

/* 3720 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 3722 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3724 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 3726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3728 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumThreads */

/* 3732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3738 */	NdrFcShort( 0xe ),	/* 14 */
/* 3740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3742 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3746 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 3748 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3750 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3752 */	NdrFcShort( 0x956 ),	/* Type Offset=2390 */

	/* Return value */

/* 3754 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPort */

/* 3760 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3766 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3768 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3770 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3774 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppPort */

/* 3776 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3778 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3780 */	NdrFcShort( 0x96c ),	/* Type Offset=2412 */

	/* Return value */

/* 3782 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3784 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumThreads */

/* 3788 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3790 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3794 */	NdrFcShort( 0x3 ),	/* 3 */
/* 3796 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3798 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3800 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3802 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 3804 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3806 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3808 */	NdrFcShort( 0x982 ),	/* Type Offset=2434 */

	/* Return value */

/* 3810 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3812 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProcess */

/* 3816 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3818 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3822 */	NdrFcShort( 0x5 ),	/* 5 */
/* 3824 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3826 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3828 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3830 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppProcess */

/* 3832 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3834 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3836 */	NdrFcShort( 0x998 ),	/* Type Offset=2456 */

	/* Return value */

/* 3838 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3840 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Terminate */

/* 3844 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3846 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3850 */	NdrFcShort( 0x6 ),	/* 6 */
/* 3852 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3856 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3858 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 3860 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3862 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3864 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Attach */

/* 3866 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3868 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3872 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3874 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3880 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pCallback */

/* 3882 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3884 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3886 */	NdrFcShort( 0x9ae ),	/* Type Offset=2478 */

	/* Return value */

/* 3888 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3890 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CanDetach */

/* 3894 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3896 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3902 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3906 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3908 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 3910 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3912 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDebugProperty */

/* 3916 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3918 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3922 */	NdrFcShort( 0xb ),	/* 11 */
/* 3924 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3926 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3928 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3930 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppProperty */

/* 3932 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 3934 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3936 */	NdrFcShort( 0x9c0 ),	/* Type Offset=2496 */

	/* Return value */

/* 3938 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3940 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3942 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Execute */

/* 3944 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3946 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3950 */	NdrFcShort( 0xc ),	/* 12 */
/* 3952 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3956 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3958 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 3960 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3962 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Continue */

/* 3966 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3968 */	NdrFcLong( 0x0 ),	/* 0 */
/* 3972 */	NdrFcShort( 0xd ),	/* 13 */
/* 3974 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3978 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3980 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pThread */

/* 3982 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 3984 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3986 */	NdrFcShort( 0x9d6 ),	/* Type Offset=2518 */

	/* Return value */

/* 3988 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 3990 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3992 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Step */

/* 3994 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 3996 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4000 */	NdrFcShort( 0xe ),	/* 14 */
/* 4002 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4004 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4006 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4008 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pThread */

/* 4010 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4012 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4014 */	NdrFcShort( 0x9d6 ),	/* Type Offset=2518 */

	/* Parameter sk */

/* 4016 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4018 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4020 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter step */

/* 4022 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4024 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4026 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4028 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4030 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4032 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEngineInfo */

/* 4034 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4036 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4040 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4042 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4046 */	NdrFcShort( 0x4c ),	/* 76 */
/* 4048 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pbstrEngine */

/* 4050 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 4052 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4054 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter pguidEngine */

/* 4056 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 4058 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4060 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 4062 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4064 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4066 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCodeContexts */

/* 4068 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4070 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4074 */	NdrFcShort( 0x11 ),	/* 17 */
/* 4076 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4078 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4080 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4082 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pDocPos */

/* 4084 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4086 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4088 */	NdrFcShort( 0x9e8 ),	/* Type Offset=2536 */

	/* Parameter ppEnum */

/* 4090 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4092 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4094 */	NdrFcShort( 0x9fa ),	/* Type Offset=2554 */

	/* Return value */

/* 4096 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4098 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMemoryBytes */

/* 4102 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4108 */	NdrFcShort( 0x12 ),	/* 18 */
/* 4110 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4112 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4114 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4116 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMemoryBytes */

/* 4118 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4120 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4122 */	NdrFcShort( 0xa10 ),	/* Type Offset=2576 */

	/* Return value */

/* 4124 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4126 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDisassemblyStream */

/* 4130 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4132 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4136 */	NdrFcShort( 0x13 ),	/* 19 */
/* 4138 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4142 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4144 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter dwScope */

/* 4146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4148 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCodeContext */

/* 4152 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4154 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4156 */	NdrFcShort( 0xa26 ),	/* Type Offset=2598 */

	/* Parameter ppDisassemblyStream */

/* 4158 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4160 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4162 */	NdrFcShort( 0xa38 ),	/* Type Offset=2616 */

	/* Return value */

/* 4164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4166 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumModules */

/* 4170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4176 */	NdrFcShort( 0x14 ),	/* 20 */
/* 4178 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4182 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4184 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 4186 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4188 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4190 */	NdrFcShort( 0xa4e ),	/* Type Offset=2638 */

	/* Return value */

/* 4192 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4194 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4196 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetENCUpdate */

/* 4198 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4200 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4204 */	NdrFcShort( 0x15 ),	/* 21 */
/* 4206 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4210 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4212 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppUpdate */

/* 4214 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4216 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4218 */	NdrFcShort( 0xa64 ),	/* Type Offset=2660 */

	/* Return value */

/* 4220 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4222 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4224 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCodePaths */

/* 4226 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 4228 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4232 */	NdrFcShort( 0x16 ),	/* 22 */
/* 4234 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 4236 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4238 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4240 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter pszHint */

/* 4242 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4244 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4246 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pStart */

/* 4248 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4250 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4252 */	NdrFcShort( 0xa7a ),	/* Type Offset=2682 */

	/* Parameter pFrame */

/* 4254 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4256 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4258 */	NdrFcShort( 0xa8c ),	/* Type Offset=2700 */

	/* Parameter fSource */

/* 4260 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4262 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4264 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 4266 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4268 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4270 */	NdrFcShort( 0xa9e ),	/* Type Offset=2718 */

	/* Parameter ppSafety */

/* 4272 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4274 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4276 */	NdrFcShort( 0xab4 ),	/* Type Offset=2740 */

	/* Return value */

/* 4278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4280 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteDump */

/* 4284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4290 */	NdrFcShort( 0x17 ),	/* 23 */
/* 4292 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4294 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4298 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter DumpType */

/* 4300 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4302 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pszDumpUrl */

/* 4306 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 4308 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4310 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Return value */

/* 4312 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4314 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onDestroy */


	/* Procedure Stop */

/* 4318 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4320 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4324 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4326 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4330 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4332 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 4334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4336 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WatchForThreadStep */

/* 4340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4346 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4348 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4350 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4354 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter pOriginatingProgram */

/* 4356 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4358 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4360 */	NdrFcShort( 0xab8 ),	/* Type Offset=2744 */

	/* Parameter dwTid */

/* 4362 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4364 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter fWatch */

/* 4368 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4370 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4372 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwFrame */

/* 4374 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4376 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4382 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WatchForExpressionEvaluationOnThread */

/* 4386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4392 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4394 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 4396 */	NdrFcShort( 0x18 ),	/* 24 */
/* 4398 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4400 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter pOriginatingProgram */

/* 4402 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4406 */	NdrFcShort( 0xab8 ),	/* Type Offset=2744 */

	/* Parameter dwTid */

/* 4408 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4412 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwEvalFlags */

/* 4414 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4416 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pExprCallback */

/* 4420 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4422 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4424 */	NdrFcShort( 0x92a ),	/* Type Offset=2346 */

	/* Parameter fWatch */

/* 4426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4428 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4430 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4432 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4434 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 4436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */


	/* Procedure GetHostName */

/* 4438 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4440 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4444 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4446 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4448 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4450 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4452 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter gnType */


	/* Parameter dwType */

/* 4454 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4456 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrFileName */


	/* Parameter pbstrHostName */

/* 4460 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 4462 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4464 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */


	/* Return value */

/* 4466 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4468 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4470 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHostId */

/* 4472 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4478 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4480 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4484 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4486 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pProcessId */

/* 4488 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 4490 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4492 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Return value */

/* 4494 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4496 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4498 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */


	/* Procedure GetHostMachineName */

/* 4500 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4502 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4506 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4508 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4512 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4514 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pbstrName */


	/* Parameter pbstrHostMachineName */

/* 4516 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 4518 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4520 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */


	/* Return value */

/* 4522 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4524 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetName */


	/* Procedure GetHostName */

/* 4528 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4530 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4534 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4536 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4540 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4542 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter gnType */


	/* Parameter dwHostNameType */

/* 4544 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4546 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4548 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pbstrFileName */


	/* Parameter pbstrHostName */

/* 4550 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 4552 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4554 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */


	/* Return value */

/* 4556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4558 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHostPid */

/* 4562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4568 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4576 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pHostProcessId */

/* 4578 */	NdrFcShort( 0x6113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=24 */
/* 4580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4582 */	NdrFcShort( 0xe2 ),	/* Type Offset=226 */

	/* Return value */

/* 4584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHostMachineName */

/* 4590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4596 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4598 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4602 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4604 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter pbstrHostMachineName */

/* 4606 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 4608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4610 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 4612 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Attach */

/* 4618 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4620 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4624 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4626 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4628 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4630 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4632 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pMDMProgram */

/* 4634 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4636 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4638 */	NdrFcShort( 0xab8 ),	/* Type Offset=2744 */

	/* Parameter pCallback */

/* 4640 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4642 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4644 */	NdrFcShort( 0x92a ),	/* Type Offset=2346 */

	/* Parameter dwReason */

/* 4646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4648 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4652 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4654 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEngineInfo */

/* 4658 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4660 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4664 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4666 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4668 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4670 */	NdrFcShort( 0x4c ),	/* 76 */
/* 4672 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter pbstrEngine */

/* 4674 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 4676 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4678 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter pguidEngine */

/* 4680 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 4682 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4684 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 4686 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4688 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumPossibleEngines */

/* 4692 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 4694 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4698 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4700 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4702 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4704 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4706 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter celtBuffer */

/* 4708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4710 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4712 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgguidEngines */

/* 4714 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 4716 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4718 */	NdrFcShort( 0xaca ),	/* Type Offset=2762 */

	/* Parameter pceltEngines */

/* 4720 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 4722 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4728 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetEngine */

/* 4732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4738 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4742 */	NdrFcShort( 0x44 ),	/* 68 */
/* 4744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4746 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter guidEngine */

/* 4748 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 4750 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4752 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 4754 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumFrameInfo */


	/* Procedure EnumFrameInfo */

/* 4760 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4766 */	NdrFcShort( 0x3 ),	/* 3 */
/* 4768 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4770 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4774 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter dwFieldSpec */


	/* Parameter dwFieldSpec */

/* 4776 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4778 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nRadix */


	/* Parameter nRadix */

/* 4782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 4784 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */


	/* Parameter ppEnum */

/* 4788 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4790 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4792 */	NdrFcShort( 0xae0 ),	/* Type Offset=2784 */

	/* Return value */


	/* Return value */

/* 4794 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4796 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetThreadName */

/* 4800 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4802 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4806 */	NdrFcShort( 0x5 ),	/* 5 */
/* 4808 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4810 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4812 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4814 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pszName */

/* 4816 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 4818 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4820 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Return value */

/* 4822 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4824 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProgram */

/* 4828 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4834 */	NdrFcShort( 0x6 ),	/* 6 */
/* 4836 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4840 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4842 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppProgram */

/* 4844 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 4846 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4848 */	NdrFcShort( 0xaf6 ),	/* Type Offset=2806 */

	/* Return value */

/* 4850 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4852 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4854 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CanSetNextStatement */

/* 4856 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4858 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4862 */	NdrFcShort( 0x7 ),	/* 7 */
/* 4864 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4866 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4868 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4870 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pStackFrame */

/* 4872 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4874 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4876 */	NdrFcShort( 0xafa ),	/* Type Offset=2810 */

	/* Parameter pCodeContext */

/* 4878 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4880 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4882 */	NdrFcShort( 0xa26 ),	/* Type Offset=2598 */

	/* Return value */

/* 4884 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4886 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetNextStatement */

/* 4890 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4892 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4896 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4898 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 4900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4902 */	NdrFcShort( 0x8 ),	/* 8 */
/* 4904 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pStackFrame */

/* 4906 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4908 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4910 */	NdrFcShort( 0xafa ),	/* Type Offset=2810 */

	/* Parameter pCodeContext */

/* 4912 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 4914 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4916 */	NdrFcShort( 0xa26 ),	/* Type Offset=2598 */

	/* Return value */

/* 4918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4920 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetScope */


	/* Procedure GetThreadId */

/* 4924 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4930 */	NdrFcShort( 0x9 ),	/* 9 */
/* 4932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4936 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4938 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwScope */


	/* Parameter pdwThreadId */

/* 4940 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4942 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4944 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 4946 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4948 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Suspend */

/* 4952 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4954 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4958 */	NdrFcShort( 0xa ),	/* 10 */
/* 4960 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4964 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4966 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwSuspendCount */

/* 4968 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4970 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 4974 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 4976 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 4978 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSize */


	/* Procedure GetSize */


	/* Procedure Resume */

/* 4980 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 4982 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4986 */	NdrFcShort( 0xb ),	/* 11 */
/* 4988 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4992 */	NdrFcShort( 0x24 ),	/* 36 */
/* 4994 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwSize */


	/* Parameter pdwSize */


	/* Parameter pdwSuspendCount */

/* 4996 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 4998 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */

/* 5002 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5004 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetThreadProperties */

/* 5008 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5010 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5014 */	NdrFcShort( 0xc ),	/* 12 */
/* 5016 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5018 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5022 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 5024 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5026 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5028 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ptp */

/* 5030 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 5032 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5034 */	NdrFcShort( 0xb10 ),	/* Type Offset=2832 */

	/* Return value */

/* 5036 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5038 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5040 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLogicalThread */

/* 5042 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5044 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5048 */	NdrFcShort( 0xd ),	/* 13 */
/* 5050 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5054 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5056 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pStackFrame */

/* 5058 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5060 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5062 */	NdrFcShort( 0xafa ),	/* Type Offset=2810 */

	/* Parameter ppLogicalThread */

/* 5064 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5066 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5068 */	NdrFcShort( 0xb2a ),	/* Type Offset=2858 */

	/* Return value */

/* 5070 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5072 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5074 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPropertyInfo */

/* 5076 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 5078 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5082 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5084 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5086 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5088 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5090 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter dwFields */

/* 5092 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5094 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5096 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwRadix */

/* 5098 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5100 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5102 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwTimeout */

/* 5104 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5106 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpArgs */

/* 5110 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5112 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5114 */	NdrFcShort( 0xb40 ),	/* Type Offset=2880 */

	/* Parameter dwArgCount */

/* 5116 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5118 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pPropertyInfo */

/* 5122 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 5124 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5126 */	NdrFcShort( 0xb7e ),	/* Type Offset=2942 */

	/* Return value */

/* 5128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5130 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValueAsString */


	/* Procedure SetValueAsString */

/* 5134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5140 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5142 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5144 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5148 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pszValue */


	/* Parameter pszValue */

/* 5150 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5152 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5154 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter dwRadix */


	/* Parameter dwRadix */

/* 5156 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5158 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5160 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwTimeout */


	/* Parameter dwTimeout */

/* 5162 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5164 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 5168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5170 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValueAsReference */

/* 5174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 5176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5180 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5182 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5184 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5188 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter rgpArgs */

/* 5190 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5192 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5194 */	NdrFcShort( 0xb9e ),	/* Type Offset=2974 */

	/* Parameter dwArgCount */

/* 5196 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5198 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 5202 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5204 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5206 */	NdrFcShort( 0xb44 ),	/* Type Offset=2884 */

	/* Parameter dwTimeout */

/* 5208 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5210 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5214 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5216 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5218 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumChildren */

/* 5220 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 5222 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5226 */	NdrFcShort( 0x6 ),	/* 6 */
/* 5228 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 5230 */	NdrFcShort( 0x6c ),	/* 108 */
/* 5232 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5234 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter dwFields */

/* 5236 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5238 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwRadix */

/* 5242 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5244 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidFilter */

/* 5248 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5250 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5252 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwAttribFilter */

/* 5254 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5256 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5258 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pszNameFilter */

/* 5260 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5262 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5264 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter dwTimeout */

/* 5266 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5268 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5270 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 5272 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5274 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5276 */	NdrFcShort( 0xbb4 ),	/* Type Offset=2996 */

	/* Return value */

/* 5278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5280 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 5282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetParent */

/* 5284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5290 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5292 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5296 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5298 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppParent */

/* 5300 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5302 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5304 */	NdrFcShort( 0xbca ),	/* Type Offset=3018 */

	/* Return value */

/* 5306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5308 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDerivedMostProperty */

/* 5312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5320 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5326 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDerivedMost */

/* 5328 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5330 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5332 */	NdrFcShort( 0xbca ),	/* Type Offset=3018 */

	/* Return value */

/* 5334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5336 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMemoryBytes */

/* 5340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5346 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5348 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5354 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMemoryBytes */

/* 5356 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5358 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5360 */	NdrFcShort( 0xa10 ),	/* Type Offset=2576 */

	/* Return value */

/* 5362 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5364 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMemoryContext */

/* 5368 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5374 */	NdrFcShort( 0xa ),	/* 10 */
/* 5376 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5380 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5382 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMemory */

/* 5384 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5388 */	NdrFcShort( 0xbce ),	/* Type Offset=3022 */

	/* Return value */

/* 5390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetReference */

/* 5396 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5402 */	NdrFcShort( 0xc ),	/* 12 */
/* 5404 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5406 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5408 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5410 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppReference */

/* 5412 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5414 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5416 */	NdrFcShort( 0xbe4 ),	/* Type Offset=3044 */

	/* Return value */

/* 5418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5420 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExtendedInfo */

/* 5424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5430 */	NdrFcShort( 0xd ),	/* 13 */
/* 5432 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5434 */	NdrFcShort( 0x44 ),	/* 68 */
/* 5436 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5438 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter guidExtendedInfo */

/* 5440 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 5442 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5444 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter pExtendedInfo */

/* 5446 */	NdrFcShort( 0x4113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=16 */
/* 5448 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5450 */	NdrFcShort( 0xc02 ),	/* Type Offset=3074 */

	/* Return value */

/* 5452 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5454 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetReferenceInfo */

/* 5458 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5460 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5464 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5466 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5468 */	NdrFcShort( 0x20 ),	/* 32 */
/* 5470 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5472 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter dwFields */

/* 5474 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwRadix */

/* 5480 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwTimeout */

/* 5486 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5488 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5490 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpArgs */

/* 5492 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5494 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5496 */	NdrFcShort( 0xc10 ),	/* Type Offset=3088 */

	/* Parameter dwArgCount */

/* 5498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5500 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5502 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pReferenceInfo */

/* 5504 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 5506 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5508 */	NdrFcShort( 0xc26 ),	/* Type Offset=3110 */

	/* Return value */

/* 5510 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5512 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetValueAsReference */

/* 5516 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5518 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5522 */	NdrFcShort( 0x5 ),	/* 5 */
/* 5524 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5526 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5528 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5530 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter rgpArgs */

/* 5532 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 5534 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5536 */	NdrFcShort( 0xc46 ),	/* Type Offset=3142 */

	/* Parameter dwArgCount */

/* 5538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5540 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pValue */

/* 5544 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5548 */	NdrFcShort( 0xbe8 ),	/* Type Offset=3048 */

	/* Parameter dwTimeout */

/* 5550 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5552 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5558 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumChildren */

/* 5562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 5564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5568 */	NdrFcShort( 0x6 ),	/* 6 */
/* 5570 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 5572 */	NdrFcShort( 0x28 ),	/* 40 */
/* 5574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5576 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter dwFields */

/* 5578 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwRadix */

/* 5584 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwAttribFilter */

/* 5590 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5592 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5594 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter pszNameFilter */

/* 5596 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5598 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5600 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter dwTimeout */

/* 5602 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5604 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 5606 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 5608 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5610 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 5612 */	NdrFcShort( 0xc58 ),	/* Type Offset=3160 */

	/* Return value */

/* 5614 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5616 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 5618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetParent */

/* 5620 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5622 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5626 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5628 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5632 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5634 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppParent */

/* 5636 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5638 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5640 */	NdrFcShort( 0xbe4 ),	/* Type Offset=3044 */

	/* Return value */

/* 5642 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5644 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5646 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDerivedMostReference */

/* 5648 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5650 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5654 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5656 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5660 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5662 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDerivedMost */

/* 5664 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5666 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5668 */	NdrFcShort( 0xbe4 ),	/* Type Offset=3044 */

	/* Return value */

/* 5670 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5672 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5674 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMemoryBytes */

/* 5676 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5678 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5682 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5684 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5688 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5690 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMemoryBytes */

/* 5692 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5694 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5696 */	NdrFcShort( 0xc6e ),	/* Type Offset=3182 */

	/* Return value */

/* 5698 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5700 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMemoryContext */

/* 5704 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5706 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5710 */	NdrFcShort( 0xa ),	/* 10 */
/* 5712 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5714 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5716 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5718 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppMemory */

/* 5720 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5722 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5724 */	NdrFcShort( 0xc84 ),	/* Type Offset=3204 */

	/* Return value */

/* 5726 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5728 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetReferenceType */

/* 5732 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5734 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5738 */	NdrFcShort( 0xc ),	/* 12 */
/* 5740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5742 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5744 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5746 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter dwRefType */

/* 5748 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5750 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5752 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 5754 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5756 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Compare */

/* 5760 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5762 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5766 */	NdrFcShort( 0xd ),	/* 13 */
/* 5768 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5770 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5772 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5774 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwCompare */

/* 5776 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5778 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5780 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pReference */

/* 5782 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 5784 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5786 */	NdrFcShort( 0xc9a ),	/* Type Offset=3226 */

	/* Return value */

/* 5788 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5790 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCodeContext */

/* 5794 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5796 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5800 */	NdrFcShort( 0x3 ),	/* 3 */
/* 5802 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5804 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5806 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5808 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppCodeCxt */

/* 5810 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5812 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5814 */	NdrFcShort( 0xcac ),	/* Type Offset=3244 */

	/* Return value */

/* 5816 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5818 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5820 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentContext */

/* 5822 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5824 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5828 */	NdrFcShort( 0x4 ),	/* 4 */
/* 5830 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5832 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5834 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5836 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppCxt */

/* 5838 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5840 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5842 */	NdrFcShort( 0xcc2 ),	/* Type Offset=3266 */

	/* Return value */

/* 5844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5846 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 5850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5856 */	NdrFcShort( 0x6 ),	/* 6 */
/* 5858 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 5860 */	NdrFcShort( 0x10 ),	/* 16 */
/* 5862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5864 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter dwFieldSpec */

/* 5866 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5868 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5870 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nRadix */

/* 5872 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 5874 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pFrameInfo */

/* 5878 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 5880 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5882 */	NdrFcShort( 0xd00 ),	/* Type Offset=3328 */

	/* Return value */

/* 5884 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5886 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5888 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPhysicalStackRange */

/* 5890 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5892 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5896 */	NdrFcShort( 0x7 ),	/* 7 */
/* 5898 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5900 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5902 */	NdrFcShort( 0x50 ),	/* 80 */
/* 5904 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter paddrMin */

/* 5906 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5908 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5910 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter paddrMax */

/* 5912 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 5914 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5916 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 5918 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5920 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5922 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetExpressionContext */

/* 5924 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5926 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5930 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5932 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5934 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5936 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5938 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppExprCxt */

/* 5940 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 5942 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5944 */	NdrFcShort( 0xd30 ),	/* Type Offset=3376 */

	/* Return value */

/* 5946 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5948 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLanguageInfo */


	/* Procedure GetLanguageInfo */

/* 5952 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 5954 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5958 */	NdrFcShort( 0x9 ),	/* 9 */
/* 5960 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 5962 */	NdrFcShort( 0x44 ),	/* 68 */
/* 5964 */	NdrFcShort( 0x4c ),	/* 76 */
/* 5966 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pbstrLanguage */


	/* Parameter pbstrLanguage */

/* 5968 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 5970 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5972 */	NdrFcShort( 0xd46 ),	/* Type Offset=3398 */

	/* Parameter pguidLanguage */


	/* Parameter pguidLanguage */

/* 5974 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 5976 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 5978 */	NdrFcShort( 0xd4a ),	/* Type Offset=3402 */

	/* Return value */


	/* Return value */

/* 5980 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 5982 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5984 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDebugProperty */

/* 5986 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 5988 */	NdrFcLong( 0x0 ),	/* 0 */
/* 5992 */	NdrFcShort( 0xa ),	/* 10 */
/* 5994 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5996 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5998 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6000 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppProperty */

/* 6002 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6004 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6006 */	NdrFcShort( 0xd4e ),	/* Type Offset=3406 */

	/* Return value */

/* 6008 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6010 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumProperties */

/* 6014 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6016 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6020 */	NdrFcShort( 0xb ),	/* 11 */
/* 6022 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 6024 */	NdrFcShort( 0x5c ),	/* 92 */
/* 6026 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6028 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x7,		/* 7 */

	/* Parameter dwFields */

/* 6030 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6032 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6034 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nRadix */

/* 6036 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6038 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6040 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter guidFilter */

/* 6042 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 6044 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6046 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter dwTimeout */

/* 6048 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6050 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6052 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pcelt */

/* 6054 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6056 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6058 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 6060 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6062 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6064 */	NdrFcShort( 0xd64 ),	/* Type Offset=3428 */

	/* Return value */

/* 6066 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6068 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6070 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetThread */

/* 6072 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6074 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6078 */	NdrFcShort( 0xc ),	/* 12 */
/* 6080 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6084 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6086 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppThread */

/* 6088 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6090 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6092 */	NdrFcShort( 0xd7a ),	/* Type Offset=3450 */

	/* Return value */

/* 6094 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6096 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 6100 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6106 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6108 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6110 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6114 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 6116 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6118 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pInfo */

/* 6122 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 6124 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6126 */	NdrFcShort( 0xd94 ),	/* Type Offset=3476 */

	/* Return value */

/* 6128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6130 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Add */

/* 6134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6140 */	NdrFcShort( 0x5 ),	/* 5 */
/* 6142 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6144 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6146 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6148 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwCount */

/* 6150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6152 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6154 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter ppMemCxt */

/* 6156 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6158 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6160 */	NdrFcShort( 0xdb6 ),	/* Type Offset=3510 */

	/* Return value */

/* 6162 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6164 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Subtract */

/* 6168 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6170 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6174 */	NdrFcShort( 0x6 ),	/* 6 */
/* 6176 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6178 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6182 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwCount */

/* 6184 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6186 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6188 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter ppMemCxt */

/* 6190 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6192 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6194 */	NdrFcShort( 0xdb6 ),	/* Type Offset=3510 */

	/* Return value */

/* 6196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6198 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Compare */

/* 6202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6208 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6210 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6212 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6214 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6216 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter compare */

/* 6218 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6220 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpMemoryContextSet */

/* 6224 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6226 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6228 */	NdrFcShort( 0xdd0 ),	/* Type Offset=3536 */

	/* Parameter dwMemoryContextSetLen */

/* 6230 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6232 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwMemoryContext */

/* 6236 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6238 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6242 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6244 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocumentContext */

/* 6248 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6250 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6256 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6260 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6262 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppSrcCxt */

/* 6264 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6266 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6268 */	NdrFcShort( 0xde2 ),	/* Type Offset=3554 */

	/* Return value */

/* 6270 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6272 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6274 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReadAt */

/* 6276 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 6278 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6282 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6284 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6286 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6288 */	NdrFcShort( 0x40 ),	/* 64 */
/* 6290 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter pStartContext */

/* 6292 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6294 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6296 */	NdrFcShort( 0xdba ),	/* Type Offset=3514 */

	/* Parameter dwCount */

/* 6298 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6300 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6302 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgbMemory */

/* 6304 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 6306 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6308 */	NdrFcShort( 0xdfc ),	/* Type Offset=3580 */

	/* Parameter pdwRead */

/* 6310 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6312 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6314 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwUnreadable */

/* 6316 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 6318 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6320 */	NdrFcShort( 0xe0a ),	/* Type Offset=3594 */

	/* Return value */

/* 6322 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6324 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6326 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure WriteAt */

/* 6328 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6330 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6334 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6336 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6340 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6342 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x4,		/* 4 */

	/* Parameter pStartContext */

/* 6344 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6346 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6348 */	NdrFcShort( 0xdba ),	/* Type Offset=3514 */

	/* Parameter dwCount */

/* 6350 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6352 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgbMemory */

/* 6356 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6358 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6360 */	NdrFcShort( 0xe12 ),	/* Type Offset=3602 */

	/* Return value */

/* 6362 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6364 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSize */

/* 6368 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6374 */	NdrFcShort( 0x5 ),	/* 5 */
/* 6376 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6378 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6380 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6382 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pqwSize */

/* 6384 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6388 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Read */

/* 6396 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6402 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6404 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6406 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6408 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6410 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwInstructions */

/* 6412 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6414 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6416 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwFields */

/* 6418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6420 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwInstructionsRead */

/* 6424 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6426 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6428 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter prgDisassembly */

/* 6430 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 6432 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6434 */	NdrFcShort( 0xe58 ),	/* Type Offset=3672 */

	/* Return value */

/* 6436 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6438 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6440 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Seek */

/* 6442 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6444 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6448 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6450 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 6452 */	NdrFcShort( 0x28 ),	/* 40 */
/* 6454 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6456 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwSeekStart */

/* 6458 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6460 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6462 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pCodeContext */

/* 6464 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6466 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6468 */	NdrFcShort( 0xe6a ),	/* Type Offset=3690 */

	/* Parameter uCodeLocationId */

/* 6470 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6472 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6474 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter iInstructions */

/* 6476 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6478 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6480 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6482 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6484 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCodeLocationId */

/* 6488 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6490 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6494 */	NdrFcShort( 0x5 ),	/* 5 */
/* 6496 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6498 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6500 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6502 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pCodeContext */

/* 6504 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 6506 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6508 */	NdrFcShort( 0xe6a ),	/* Type Offset=3690 */

	/* Parameter puCodeLocationId */

/* 6510 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6512 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6514 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6518 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCodeContext */

/* 6522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6528 */	NdrFcShort( 0x6 ),	/* 6 */
/* 6530 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6532 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6534 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6536 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter uCodeLocationId */

/* 6538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6540 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6542 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Parameter ppCodeContext */

/* 6544 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6548 */	NdrFcShort( 0xe7c ),	/* Type Offset=3708 */

	/* Return value */

/* 6550 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6552 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCurrentLocation */

/* 6556 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6558 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6562 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6564 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6566 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6568 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6570 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter puCodeLocationId */

/* 6572 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6574 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6576 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6578 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6580 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocument */

/* 6584 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6586 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6590 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6592 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6594 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6596 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6598 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter bstrDocumentUrl */

/* 6600 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 6602 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6604 */	NdrFcShort( 0x1b6 ),	/* Type Offset=438 */

	/* Parameter ppDocument */

/* 6606 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6608 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6610 */	NdrFcShort( 0xe80 ),	/* Type Offset=3712 */

	/* Return value */

/* 6612 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6614 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSize */

/* 6618 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6620 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6624 */	NdrFcShort( 0xa ),	/* 10 */
/* 6626 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6630 */	NdrFcShort( 0x2c ),	/* 44 */
/* 6632 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pnSize */

/* 6634 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6636 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6638 */	0xb,		/* FC_HYPER */
			0x0,		/* 0 */

	/* Return value */

/* 6640 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6642 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6644 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocument */

/* 6646 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6648 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6652 */	NdrFcShort( 0x3 ),	/* 3 */
/* 6654 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6656 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6658 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6660 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDocument */

/* 6662 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6664 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6666 */	NdrFcShort( 0xe80 ),	/* Type Offset=3712 */

	/* Return value */

/* 6668 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6670 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumCodeContexts */

/* 6674 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6676 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6680 */	NdrFcShort( 0x5 ),	/* 5 */
/* 6682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 6686 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6688 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnumCodeCxts */

/* 6690 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6692 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6694 */	NdrFcShort( 0xe96 ),	/* Type Offset=3734 */

	/* Return value */

/* 6696 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6698 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLanguageInfo */

/* 6702 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 6704 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6708 */	NdrFcShort( 0x6 ),	/* 6 */
/* 6710 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6712 */	NdrFcShort( 0x44 ),	/* 68 */
/* 6714 */	NdrFcShort( 0x4c ),	/* 76 */
/* 6716 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pbstrLanguage */

/* 6718 */	NdrFcShort( 0x1b ),	/* Flags:  must size, must free, in, out, */
/* 6720 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6722 */	NdrFcShort( 0xd46 ),	/* Type Offset=3398 */

	/* Parameter pguidLanguage */

/* 6724 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 6726 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6728 */	NdrFcShort( 0xd4a ),	/* Type Offset=3402 */

	/* Return value */

/* 6730 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6732 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetStatementRange */

/* 6736 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 6738 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6742 */	NdrFcShort( 0x7 ),	/* 7 */
/* 6744 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6746 */	NdrFcShort( 0x58 ),	/* 88 */
/* 6748 */	NdrFcShort( 0x60 ),	/* 96 */
/* 6750 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pBegPosition */

/* 6752 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 6754 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6756 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Parameter pEndPosition */

/* 6758 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 6760 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6762 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Return value */

/* 6764 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6766 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6768 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSourceRange */

/* 6770 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 6772 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6776 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6778 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6780 */	NdrFcShort( 0x58 ),	/* 88 */
/* 6782 */	NdrFcShort( 0x60 ),	/* 96 */
/* 6784 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pBegPosition */

/* 6786 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 6788 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6790 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Parameter pEndPosition */

/* 6792 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 6794 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6796 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Return value */

/* 6798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6800 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Compare */

/* 6804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6810 */	NdrFcShort( 0x9 ),	/* 9 */
/* 6812 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6814 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6816 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6818 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter compare */

/* 6820 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6822 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgpDocContextSet */

/* 6826 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6828 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6830 */	NdrFcShort( 0xec6 ),	/* Type Offset=3782 */

	/* Parameter dwDocContextSetLen */

/* 6832 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6834 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6836 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pdwDocContext */

/* 6838 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6840 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6842 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6844 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6846 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6848 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Seek */

/* 6850 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6852 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6856 */	NdrFcShort( 0xa ),	/* 10 */
/* 6858 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6860 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6864 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter nCount */

/* 6866 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6868 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6870 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppDocContext */

/* 6872 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6874 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6876 */	NdrFcShort( 0xed8 ),	/* Type Offset=3800 */

	/* Return value */

/* 6878 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6880 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ParseText */

/* 6884 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6886 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6890 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6892 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 6894 */	NdrFcShort( 0x10 ),	/* 16 */
/* 6896 */	NdrFcShort( 0x24 ),	/* 36 */
/* 6898 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x7,		/* 7 */

	/* Parameter pszCode */

/* 6900 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 6902 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6904 */	NdrFcShort( 0xc2 ),	/* Type Offset=194 */

	/* Parameter dwFlags */

/* 6906 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6908 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6910 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nRadix */

/* 6912 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6914 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6916 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppExpr */

/* 6918 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 6920 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6922 */	NdrFcShort( 0xedc ),	/* Type Offset=3804 */

	/* Parameter pbstrError */

/* 6924 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 6926 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 6928 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Parameter pichError */

/* 6930 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 6932 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 6934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 6936 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6938 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 6940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRequestInfo */

/* 6942 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6944 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6948 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6950 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6952 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6954 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6956 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 6958 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6960 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBPRequestInfo */

/* 6964 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 6966 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 6968 */	NdrFcShort( 0x100a ),	/* Type Offset=4106 */

	/* Return value */

/* 6970 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 6972 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 6974 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetResolutionInfo */

/* 6976 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 6978 */	NdrFcLong( 0x0 ),	/* 0 */
/* 6982 */	NdrFcShort( 0x4 ),	/* 4 */
/* 6984 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 6986 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6988 */	NdrFcShort( 0x8 ),	/* 8 */
/* 6990 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 6992 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 6994 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 6996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pBPResolutionInfo */

/* 6998 */	NdrFcShort( 0x8113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=32 */
/* 7000 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7002 */	NdrFcShort( 0x1072 ),	/* Type Offset=4210 */

	/* Return value */

/* 7004 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7006 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7008 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetResolutionInfo */

/* 7010 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7012 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7016 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7018 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7020 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7022 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7024 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 7026 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7028 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pErrorResolutionInfo */

/* 7032 */	NdrFcShort( 0xa113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=40 */
/* 7034 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7036 */	NdrFcShort( 0x108c ),	/* Type Offset=4236 */

	/* Return value */

/* 7038 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7040 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7042 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPendingBreakpoint */

/* 7044 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7046 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7050 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7052 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7056 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7058 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppPendingBreakpoint */

/* 7060 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7062 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7064 */	NdrFcShort( 0x10a8 ),	/* Type Offset=4264 */

	/* Return value */

/* 7066 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7068 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7070 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetHitCount */

/* 7072 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7074 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7078 */	NdrFcShort( 0x5 ),	/* 5 */
/* 7080 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7082 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7084 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7086 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pdwHitCount */

/* 7088 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7090 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7092 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7094 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7096 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBreakpointResolution */

/* 7100 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7106 */	NdrFcShort( 0x6 ),	/* 6 */
/* 7108 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7114 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppBPResolution */

/* 7116 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7118 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7120 */	NdrFcShort( 0x10be ),	/* Type Offset=4286 */

	/* Return value */

/* 7122 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7124 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7126 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Virtualize */


	/* Procedure Enable */

/* 7128 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7130 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7134 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7136 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7138 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7140 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7142 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter fVirtualize */


	/* Parameter fEnable */

/* 7144 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7146 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 7150 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7152 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetCondition */


	/* Procedure SetCondition */

/* 7156 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7158 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7162 */	NdrFcShort( 0x9 ),	/* 9 */
/* 7164 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 7166 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7168 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7170 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter bpCondition */


	/* Parameter bpCondition */

/* 7172 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 7174 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7176 */	NdrFcShort( 0xff2 ),	/* Type Offset=4082 */

	/* Return value */


	/* Return value */

/* 7178 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7180 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7182 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetPassCount */


	/* Procedure SetPassCount */

/* 7184 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7186 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7190 */	NdrFcShort( 0xa ),	/* 10 */
/* 7192 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7194 */	NdrFcShort( 0x18 ),	/* 24 */
/* 7196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7198 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter bpPassCount */


	/* Parameter bpPassCount */

/* 7200 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7202 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7204 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Return value */


	/* Return value */

/* 7206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7208 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 7212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7218 */	NdrFcShort( 0xb ),	/* 11 */
/* 7220 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7224 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7226 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 7228 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7230 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7232 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CanBind */

/* 7234 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7236 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7240 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7242 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7244 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7246 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7248 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppErrorEnum */

/* 7250 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7252 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7254 */	NdrFcShort( 0x10c2 ),	/* Type Offset=4290 */

	/* Return value */

/* 7256 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7258 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7260 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Abort */


	/* Procedure Bind */

/* 7262 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7264 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7268 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7270 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7274 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7276 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */


	/* Return value */

/* 7278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7280 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetState */

/* 7284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7290 */	NdrFcShort( 0x5 ),	/* 5 */
/* 7292 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7294 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7296 */	NdrFcShort( 0x34 ),	/* 52 */
/* 7298 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pState */

/* 7300 */	NdrFcShort( 0x2112 ),	/* Flags:  must free, out, simple ref, srv alloc size=8 */
/* 7302 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7304 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Return value */

/* 7306 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7308 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7310 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBreakpointRequest */

/* 7312 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7314 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7318 */	NdrFcShort( 0x6 ),	/* 6 */
/* 7320 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7322 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7324 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7326 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppBPRequest */

/* 7328 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7330 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7332 */	NdrFcShort( 0x10dc ),	/* Type Offset=4316 */

	/* Return value */

/* 7334 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7336 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7338 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumBoundBreakpoints */

/* 7340 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7342 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7346 */	NdrFcShort( 0xb ),	/* 11 */
/* 7348 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7352 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7354 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 7356 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7358 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7360 */	NdrFcShort( 0x10f2 ),	/* Type Offset=4338 */

	/* Return value */

/* 7362 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7364 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7366 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EnumErrorBreakpoints */

/* 7368 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7370 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7374 */	NdrFcShort( 0xc ),	/* 12 */
/* 7376 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7378 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7380 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7382 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter bpErrorType */

/* 7384 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7386 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ppEnum */

/* 7390 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7392 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7394 */	NdrFcShort( 0x1108 ),	/* Type Offset=4360 */

	/* Return value */

/* 7396 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7398 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7400 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Delete */

/* 7402 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7404 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7408 */	NdrFcShort( 0xd ),	/* 13 */
/* 7410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7412 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7414 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7416 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 7418 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7420 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetPendingBreakpoint */

/* 7424 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7426 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7430 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7432 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7434 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7436 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7438 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppPendingBreakpoint */

/* 7440 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7442 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7444 */	NdrFcShort( 0x111e ),	/* Type Offset=4382 */

	/* Return value */

/* 7446 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7448 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBreakpointResolution */

/* 7452 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7454 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7458 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7460 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7464 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7466 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppErrorResolution */

/* 7468 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7470 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7472 */	NdrFcShort( 0x1134 ),	/* Type Offset=4404 */

	/* Return value */

/* 7474 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7476 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EvaluateAsync */

/* 7480 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7482 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7486 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7488 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7492 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7494 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFlags */

/* 7496 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7498 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7500 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pExprCallback */

/* 7502 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7504 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7506 */	NdrFcShort( 0x114a ),	/* Type Offset=4426 */

	/* Return value */

/* 7508 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7510 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7512 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure EvaluateSync */

/* 7514 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7516 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7520 */	NdrFcShort( 0x5 ),	/* 5 */
/* 7522 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7524 */	NdrFcShort( 0x10 ),	/* 16 */
/* 7526 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7528 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter dwFlags */

/* 7530 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7532 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dwTimeout */

/* 7536 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7538 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pExprCallback */

/* 7542 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7544 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7546 */	NdrFcShort( 0x114a ),	/* Type Offset=4426 */

	/* Parameter ppResult */

/* 7548 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7550 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7552 */	NdrFcShort( 0x115c ),	/* Type Offset=4444 */

	/* Return value */

/* 7554 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7556 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7558 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetInfo */

/* 7560 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7562 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7566 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7568 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7570 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7572 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7574 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x3,		/* 3 */

	/* Parameter dwFields */

/* 7576 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7578 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pInfo */

/* 7582 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 7584 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7586 */	NdrFcShort( 0x1176 ),	/* Type Offset=4470 */

	/* Return value */

/* 7588 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7590 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7592 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ReloadSymbols */

/* 7594 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 7596 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7600 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7602 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7606 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7608 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pszUrlToSymbols */

/* 7610 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7612 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7614 */	NdrFcShort( 0x1ae ),	/* Type Offset=430 */

	/* Parameter pbstrDebugMessage */

/* 7616 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 7618 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7620 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */

	/* Return value */

/* 7622 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7624 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7626 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMvid */

/* 7628 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7630 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7634 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7636 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7640 */	NdrFcShort( 0x4c ),	/* 76 */
/* 7642 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter mvid */

/* 7644 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 7646 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7648 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Return value */

/* 7650 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7652 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7654 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSize */

/* 7656 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 7658 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7662 */	NdrFcShort( 0x5 ),	/* 5 */
/* 7664 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7666 */	NdrFcShort( 0x38 ),	/* 56 */
/* 7668 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7670 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pcNumLines */

/* 7672 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7674 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7676 */	NdrFcShort( 0xe0a ),	/* Type Offset=3594 */

	/* Parameter pcNumChars */

/* 7678 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7680 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7682 */	NdrFcShort( 0xe0a ),	/* Type Offset=3594 */

	/* Return value */

/* 7684 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7686 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7688 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetText */

/* 7690 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7692 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7696 */	NdrFcShort( 0x6 ),	/* 6 */
/* 7698 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 7700 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7702 */	NdrFcShort( 0x24 ),	/* 36 */
/* 7704 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x5,		/* 5 */

	/* Parameter pos */

/* 7706 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7708 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7710 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Parameter cMaxChars */

/* 7712 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7714 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7716 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pText */

/* 7718 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 7720 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7722 */	NdrFcShort( 0x11a2 ),	/* Type Offset=4514 */

	/* Parameter pcNumChars */

/* 7724 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 7726 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7728 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7730 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7732 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 7734 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetDocument */

/* 7736 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7738 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7742 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7744 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7746 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7748 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7750 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppDoc */

/* 7752 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 7754 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7756 */	NdrFcShort( 0x11b0 ),	/* Type Offset=4528 */

	/* Return value */

/* 7758 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7760 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7762 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsPositionInDocument */

/* 7764 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7766 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7770 */	NdrFcShort( 0x5 ),	/* 5 */
/* 7772 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 7776 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7778 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter pDoc */

/* 7780 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 7782 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7784 */	NdrFcShort( 0x11b4 ),	/* Type Offset=4532 */

	/* Return value */

/* 7786 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7788 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRange */

/* 7792 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 7794 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7798 */	NdrFcShort( 0x6 ),	/* 6 */
/* 7800 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7802 */	NdrFcShort( 0x58 ),	/* 88 */
/* 7804 */	NdrFcShort( 0x60 ),	/* 96 */
/* 7806 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pBegPosition */

/* 7808 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7810 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7812 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Parameter pEndPosition */

/* 7814 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7816 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7818 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Return value */

/* 7820 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7822 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7824 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetRange */

/* 7826 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 7828 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7832 */	NdrFcShort( 0x3 ),	/* 3 */
/* 7834 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7836 */	NdrFcShort( 0x38 ),	/* 56 */
/* 7838 */	NdrFcShort( 0x40 ),	/* 64 */
/* 7840 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pdwBegOffset */

/* 7842 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7844 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7846 */	NdrFcShort( 0xe0a ),	/* Type Offset=3594 */

	/* Parameter pdwEndOffset */

/* 7848 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7850 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7852 */	NdrFcShort( 0xe0a ),	/* Type Offset=3594 */

	/* Return value */

/* 7854 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7856 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7858 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetOffset */

/* 7860 */	0x33,		/* FC_AUTO_HANDLE */
			0x6d,		/* Old Flags:  full ptr, object, Oi2 */
/* 7862 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7866 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7868 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7870 */	NdrFcShort( 0x2c ),	/* 44 */
/* 7872 */	NdrFcShort( 0x34 ),	/* 52 */
/* 7874 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pPosition */

/* 7876 */	NdrFcShort( 0x1a ),	/* Flags:  must free, in, out, */
/* 7878 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7880 */	NdrFcShort( 0xeac ),	/* Type Offset=3756 */

	/* Return value */

/* 7882 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7884 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 7886 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onInsertText */

/* 7888 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7890 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7894 */	NdrFcShort( 0x4 ),	/* 4 */
/* 7896 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7898 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7902 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pos */

/* 7904 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7906 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7908 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Parameter dwNumToInsert */

/* 7910 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7912 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7914 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7916 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7918 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7920 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onRemoveText */

/* 7922 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7924 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7928 */	NdrFcShort( 0x5 ),	/* 5 */
/* 7930 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7932 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7934 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7936 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pos */

/* 7938 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7940 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7942 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Parameter dwNumToRemove */

/* 7944 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7946 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7948 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7950 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7952 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7954 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onReplaceText */

/* 7956 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7958 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7962 */	NdrFcShort( 0x6 ),	/* 6 */
/* 7964 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 7966 */	NdrFcShort( 0x20 ),	/* 32 */
/* 7968 */	NdrFcShort( 0x8 ),	/* 8 */
/* 7970 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pos */

/* 7972 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 7974 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 7976 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Parameter dwNumToReplace */

/* 7978 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 7980 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 7982 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 7984 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 7986 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 7988 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure onUpdateTextAttributes */

/* 7990 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 7992 */	NdrFcLong( 0x0 ),	/* 0 */
/* 7996 */	NdrFcShort( 0x7 ),	/* 7 */
/* 7998 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8000 */	NdrFcShort( 0x20 ),	/* 32 */
/* 8002 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8004 */	0x4,		/* Oi2 Flags:  has return, */
			0x3,		/* 3 */

	/* Parameter pos */

/* 8006 */	NdrFcShort( 0x8a ),	/* Flags:  must free, in, by val, */
/* 8008 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8010 */	NdrFcShort( 0x61e ),	/* Type Offset=1566 */

	/* Parameter dwNumToUpdate */

/* 8012 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8014 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8016 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8018 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8020 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8022 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEngineInterface */

/* 8024 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8026 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8030 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8032 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8036 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8038 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppUnk */

/* 8040 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8042 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8044 */	NdrFcShort( 0x11c6 ),	/* Type Offset=4550 */

	/* Return value */

/* 8046 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8048 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8050 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8052 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8054 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8058 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8060 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8062 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8064 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8066 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8070 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8074 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8076 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8078 */	NdrFcShort( 0x11f2 ),	/* Type Offset=4594 */

	/* Parameter pceltFetched */

/* 8080 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8082 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8084 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8086 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8088 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8090 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8092 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8094 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8098 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8100 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8102 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8104 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8106 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8108 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8112 */	NdrFcShort( 0x1204 ),	/* Type Offset=4612 */

	/* Return value */

/* 8114 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8118 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */


	/* Procedure GetCount */

/* 8120 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8122 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8126 */	NdrFcShort( 0x7 ),	/* 7 */
/* 8128 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8132 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8134 */	0x4,		/* Oi2 Flags:  has return, */
			0x2,		/* 2 */

	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */


	/* Parameter pcelt */

/* 8136 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 8138 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */


	/* Return value */

/* 8142 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8146 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8148 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8150 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8154 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8156 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8158 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8160 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8162 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8164 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8166 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8170 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8172 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8174 */	NdrFcShort( 0x1230 ),	/* Type Offset=4656 */

	/* Parameter pceltFetched */

/* 8176 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8178 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8180 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8182 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8184 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8186 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8188 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8190 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8194 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8196 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8198 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8200 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8202 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8204 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8208 */	NdrFcShort( 0x1242 ),	/* Type Offset=4674 */

	/* Return value */

/* 8210 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8212 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8214 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8216 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8218 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8222 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8224 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8226 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8228 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8230 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8232 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8234 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8236 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8238 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8240 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8242 */	NdrFcShort( 0x126e ),	/* Type Offset=4718 */

	/* Parameter pceltFetched */

/* 8244 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8246 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8248 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8250 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8252 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8254 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8256 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8258 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8262 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8264 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8266 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8268 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8270 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8272 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8274 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8276 */	NdrFcShort( 0x1280 ),	/* Type Offset=4736 */

	/* Return value */

/* 8278 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8280 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8282 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8284 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8290 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8292 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8294 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8296 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8298 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8300 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8302 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8306 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8308 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8310 */	NdrFcShort( 0x12ac ),	/* Type Offset=4780 */

	/* Parameter pceltFetched */

/* 8312 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8314 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8316 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8318 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8320 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8324 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8326 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8330 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8332 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8334 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8338 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8340 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8342 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8344 */	NdrFcShort( 0x12be ),	/* Type Offset=4798 */

	/* Return value */

/* 8346 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8348 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8350 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetIndex */

/* 8352 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8354 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8358 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8360 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8362 */	NdrFcShort( 0x1c ),	/* 28 */
/* 8364 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8366 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x3,		/* 3 */

	/* Parameter pStackFrame */

/* 8368 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 8370 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8372 */	NdrFcShort( 0x129a ),	/* Type Offset=4762 */

	/* Parameter pIndex */

/* 8374 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8376 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8378 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8380 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8382 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8386 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8388 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8392 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8394 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8396 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8398 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8400 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8402 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8404 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8408 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8410 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8412 */	NdrFcShort( 0x12ea ),	/* Type Offset=4842 */

	/* Parameter pceltFetched */

/* 8414 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8416 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8420 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8422 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8424 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8426 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8428 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8432 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8434 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8436 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8438 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8440 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8442 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8444 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8446 */	NdrFcShort( 0x12fc ),	/* Type Offset=4860 */

	/* Return value */

/* 8448 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8450 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8452 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8454 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8456 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8460 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8462 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8464 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8466 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8468 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8470 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8472 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8474 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8476 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8478 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8480 */	NdrFcShort( 0x1328 ),	/* Type Offset=4904 */

	/* Parameter pceltFetched */

/* 8482 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8484 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8486 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8488 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8490 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8492 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8494 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8496 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8500 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8502 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8506 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8508 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8510 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8512 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8514 */	NdrFcShort( 0x133a ),	/* Type Offset=4922 */

	/* Return value */

/* 8516 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8518 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8520 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8522 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8528 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8530 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8532 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8534 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8536 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8538 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8540 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8542 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8544 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8546 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8548 */	NdrFcShort( 0x1354 ),	/* Type Offset=4948 */

	/* Parameter pceltFetched */

/* 8550 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8552 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8554 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8556 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8558 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8560 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8562 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8564 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8568 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8570 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8574 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8576 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8578 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8580 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8582 */	NdrFcShort( 0x1366 ),	/* Type Offset=4966 */

	/* Return value */

/* 8584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8596 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8598 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8600 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8602 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8604 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8612 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8616 */	NdrFcShort( 0x1392 ),	/* Type Offset=5010 */

	/* Parameter pceltFetched */

/* 8618 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8620 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8624 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8626 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8628 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8630 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8632 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8636 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8638 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8642 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8644 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8646 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8648 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8650 */	NdrFcShort( 0x1108 ),	/* Type Offset=4360 */

	/* Return value */

/* 8652 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8654 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8656 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8658 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8660 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8664 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8666 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8668 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8670 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8672 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8674 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8676 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8678 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8680 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8682 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8684 */	NdrFcShort( 0x13ba ),	/* Type Offset=5050 */

	/* Parameter pceltFetched */

/* 8686 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8688 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8690 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8692 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8694 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8698 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8700 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8704 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8706 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8710 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8712 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8714 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8716 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8718 */	NdrFcShort( 0x13cc ),	/* Type Offset=5068 */

	/* Return value */

/* 8720 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8722 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8724 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8726 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8728 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8732 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8734 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8736 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8738 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8740 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8742 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8744 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8746 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8748 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8750 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8752 */	NdrFcShort( 0x13e6 ),	/* Type Offset=5094 */

	/* Parameter pceltFetched */

/* 8754 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8756 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8758 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8760 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8762 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8764 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8766 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8768 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8772 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8774 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8776 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8778 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8780 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8782 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8784 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8786 */	NdrFcShort( 0x13f8 ),	/* Type Offset=5112 */

	/* Return value */

/* 8788 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8790 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8792 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8794 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8796 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8800 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8802 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8804 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8806 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8808 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8810 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8812 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8814 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8816 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8818 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8820 */	NdrFcShort( 0x1412 ),	/* Type Offset=5138 */

	/* Parameter pceltFetched */

/* 8822 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8824 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8826 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8828 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8830 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8834 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8836 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8840 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8842 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8848 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8850 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8852 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8854 */	NdrFcShort( 0xae0 ),	/* Type Offset=2784 */

	/* Return value */

/* 8856 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8858 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8860 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8862 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8864 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8868 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8870 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8872 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8874 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8876 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8878 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8880 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8884 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8886 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8888 */	NdrFcShort( 0x143a ),	/* Type Offset=5178 */

	/* Parameter pceltFetched */

/* 8890 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8892 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8894 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8896 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8898 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8900 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8902 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8904 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8908 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8910 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8912 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8914 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8916 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8918 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8922 */	NdrFcShort( 0xa9e ),	/* Type Offset=2718 */

	/* Return value */

/* 8924 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8926 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8928 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8930 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8932 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8936 */	NdrFcShort( 0x3 ),	/* 3 */
/* 8938 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 8940 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8942 */	NdrFcShort( 0x24 ),	/* 36 */
/* 8944 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 8946 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 8948 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8950 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 8952 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 8954 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8956 */	NdrFcShort( 0x1450 ),	/* Type Offset=5200 */

	/* Parameter pceltFetched */

/* 8958 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 8960 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8962 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 8964 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8966 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 8968 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 8970 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 8972 */	NdrFcLong( 0x0 ),	/* 0 */
/* 8976 */	NdrFcShort( 0x6 ),	/* 6 */
/* 8978 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 8980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 8982 */	NdrFcShort( 0x8 ),	/* 8 */
/* 8984 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 8986 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 8988 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 8990 */	NdrFcShort( 0x1462 ),	/* Type Offset=5218 */

	/* Return value */

/* 8992 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 8994 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 8996 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 8998 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9000 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9004 */	NdrFcShort( 0x3 ),	/* 3 */
/* 9006 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 9008 */	NdrFcShort( 0x24 ),	/* 36 */
/* 9010 */	NdrFcShort( 0x24 ),	/* 36 */
/* 9012 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 9014 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 9016 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 9020 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 9022 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9024 */	NdrFcShort( 0x148e ),	/* Type Offset=5262 */

	/* Parameter pceltFetched */

/* 9026 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 9028 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9030 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 9032 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9034 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 9036 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 9038 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9040 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9044 */	NdrFcShort( 0x6 ),	/* 6 */
/* 9046 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9048 */	NdrFcShort( 0x0 ),	/* 0 */
/* 9050 */	NdrFcShort( 0x8 ),	/* 8 */
/* 9052 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 9054 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 9056 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9058 */	NdrFcShort( 0x14a0 ),	/* Type Offset=5280 */

	/* Return value */

/* 9060 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9062 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9064 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 9066 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9068 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9072 */	NdrFcShort( 0x3 ),	/* 3 */
/* 9074 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 9076 */	NdrFcShort( 0x24 ),	/* 36 */
/* 9078 */	NdrFcShort( 0x24 ),	/* 36 */
/* 9080 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 9082 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 9084 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9086 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 9088 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 9090 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9092 */	NdrFcShort( 0x14cc ),	/* Type Offset=5324 */

	/* Parameter pceltFetched */

/* 9094 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 9096 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9098 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 9100 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9102 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 9104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 9106 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9108 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9112 */	NdrFcShort( 0x6 ),	/* 6 */
/* 9114 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 9118 */	NdrFcShort( 0x8 ),	/* 8 */
/* 9120 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 9122 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 9124 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9126 */	NdrFcShort( 0x14de ),	/* Type Offset=5342 */

	/* Return value */

/* 9128 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9130 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9132 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 9134 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9136 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9140 */	NdrFcShort( 0x3 ),	/* 3 */
/* 9142 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 9144 */	NdrFcShort( 0x8 ),	/* 8 */
/* 9146 */	NdrFcShort( 0x24 ),	/* 36 */
/* 9148 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 9150 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 9152 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9154 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 9156 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 9158 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9160 */	NdrFcShort( 0x14f8 ),	/* Type Offset=5368 */

	/* Parameter pceltFetched */

/* 9162 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 9164 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 9168 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9170 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 9172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 9174 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9176 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9180 */	NdrFcShort( 0x6 ),	/* 6 */
/* 9182 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9184 */	NdrFcShort( 0x0 ),	/* 0 */
/* 9186 */	NdrFcShort( 0x8 ),	/* 8 */
/* 9188 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 9190 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 9192 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9194 */	NdrFcShort( 0x150a ),	/* Type Offset=5386 */

	/* Return value */

/* 9196 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9198 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9200 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Next */

/* 9202 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9204 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9208 */	NdrFcShort( 0x3 ),	/* 3 */
/* 9210 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 9212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 9214 */	NdrFcShort( 0x24 ),	/* 36 */
/* 9216 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x4,		/* 4 */

	/* Parameter celt */

/* 9218 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 9220 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9222 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter rgelt */

/* 9224 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 9226 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9228 */	NdrFcShort( 0x1524 ),	/* Type Offset=5412 */

	/* Parameter pceltFetched */

/* 9230 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 9232 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9234 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 9236 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9238 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 9240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Clone */

/* 9242 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 9244 */	NdrFcLong( 0x0 ),	/* 0 */
/* 9248 */	NdrFcShort( 0x6 ),	/* 6 */
/* 9250 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 9252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 9254 */	NdrFcShort( 0x8 ),	/* 8 */
/* 9256 */	0x5,		/* Oi2 Flags:  srv must size, has return, */
			0x2,		/* 2 */

	/* Parameter ppEnum */

/* 9258 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 9260 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 9262 */	NdrFcShort( 0x1536 ),	/* Type Offset=5430 */

	/* Return value */

/* 9264 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 9266 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 9268 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  4 */	NdrFcShort( 0x24 ),	/* Offset= 36 (40) */
/*  6 */	
			0x13, 0x0,	/* FC_OP */
/*  8 */	NdrFcShort( 0xc ),	/* Offset= 12 (20) */
/* 10 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 12 */	NdrFcShort( 0x2 ),	/* 2 */
/* 14 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 16 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 18 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 20 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 22 */	NdrFcShort( 0x8 ),	/* 8 */
/* 24 */	NdrFcShort( 0xfffffff2 ),	/* Offset= -14 (10) */
/* 26 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 28 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 30 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x4 ),	/* 4 */
/* 36 */	NdrFcShort( 0x0 ),	/* 0 */
/* 38 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (6) */
/* 40 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 42 */	NdrFcShort( 0xc ),	/* 12 */
/* 44 */	NdrFcShort( 0x0 ),	/* 0 */
/* 46 */	NdrFcShort( 0x0 ),	/* Offset= 0 (46) */
/* 48 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 50 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffeb ),	/* Offset= -21 (30) */
			0x8,		/* FC_LONG */
/* 54 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 56 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 58 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (30) */
/* 60 */	
			0x11, 0x0,	/* FC_RP */
/* 62 */	NdrFcShort( 0x8 ),	/* Offset= 8 (70) */
/* 64 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 66 */	NdrFcShort( 0x8 ),	/* 8 */
/* 68 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 70 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 72 */	NdrFcShort( 0x10 ),	/* 16 */
/* 74 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 76 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 78 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffff1 ),	/* Offset= -15 (64) */
			0x5b,		/* FC_END */
/* 82 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 84 */	NdrFcShort( 0x2 ),	/* Offset= 2 (86) */
/* 86 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 88 */	NdrFcLong( 0x53f68191 ),	/* 1408663953 */
/* 92 */	NdrFcShort( 0x7b2f ),	/* 31535 */
/* 94 */	NdrFcShort( 0x4f14 ),	/* 20244 */
/* 96 */	0x8e,		/* 142 */
			0x55,		/* 85 */
/* 98 */	0x40,		/* 64 */
			0xb1,		/* 177 */
/* 100 */	0xb6,		/* 182 */
			0xe5,		/* 229 */
/* 102 */	0xdf,		/* 223 */
			0x66,		/* 102 */
/* 104 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 106 */	NdrFcShort( 0x2 ),	/* Offset= 2 (108) */
/* 108 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 110 */	NdrFcLong( 0x79293cc8 ),	/* 2032745672 */
/* 114 */	NdrFcShort( 0xd9d9 ),	/* -9767 */
/* 116 */	NdrFcShort( 0x43f5 ),	/* 17397 */
/* 118 */	0x97,		/* 151 */
			0xad,		/* 173 */
/* 120 */	0xb,		/* 11 */
			0xcc,		/* 204 */
/* 122 */	0x5a,		/* 90 */
			0x68,		/* 104 */
/* 124 */	0x87,		/* 135 */
			0x76,		/* 118 */
/* 126 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 128 */	NdrFcShort( 0x2 ),	/* Offset= 2 (130) */
/* 130 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 132 */	NdrFcLong( 0xbc827c5e ),	/* -1132299170 */
/* 136 */	NdrFcShort( 0x99ae ),	/* -26194 */
/* 138 */	NdrFcShort( 0x4ac8 ),	/* 19144 */
/* 140 */	0x83,		/* 131 */
			0xad,		/* 173 */
/* 142 */	0x2e,		/* 46 */
			0xa5,		/* 165 */
/* 144 */	0xc2,		/* 194 */
			0x3,		/* 3 */
/* 146 */	0x43,		/* 67 */
			0x33,		/* 51 */
/* 148 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 150 */	NdrFcShort( 0x2 ),	/* Offset= 2 (152) */
/* 152 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 154 */	NdrFcLong( 0x59c9dc99 ),	/* 1506401433 */
/* 158 */	NdrFcShort( 0x3eff ),	/* 16127 */
/* 160 */	NdrFcShort( 0x4ff3 ),	/* 20467 */
/* 162 */	0xb2,		/* 178 */
			0x1,		/* 1 */
/* 164 */	0x98,		/* 152 */
			0xac,		/* 172 */
/* 166 */	0xd0,		/* 208 */
			0x1b,		/* 27 */
/* 168 */	0xd,		/* 13 */
			0x87,		/* 135 */
/* 170 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 172 */	NdrFcShort( 0x2 ),	/* Offset= 2 (174) */
/* 174 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 176 */	NdrFcLong( 0xf3062547 ),	/* -217701049 */
/* 180 */	NdrFcShort( 0x43d8 ),	/* 17368 */
/* 182 */	NdrFcShort( 0x4dc2 ),	/* 19906 */
/* 184 */	0xb1,		/* 177 */
			0x8e,		/* 142 */
/* 186 */	0xe1,		/* 225 */
			0x46,		/* 70 */
/* 188 */	0xf,		/* 15 */
			0xf2,		/* 242 */
/* 190 */	0xc4,		/* 196 */
			0x22,		/* 34 */
/* 192 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 194 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 196 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 198 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 200 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 202 */	NdrFcShort( 0xffffff7c ),	/* Offset= -132 (70) */
/* 204 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 206 */	NdrFcLong( 0x8d36beb8 ),	/* -1925792072 */
/* 210 */	NdrFcShort( 0x9bfe ),	/* -25602 */
/* 212 */	NdrFcShort( 0x47dd ),	/* 18397 */
/* 214 */	0xa1,		/* 161 */
			0x1b,		/* 27 */
/* 216 */	0x7b,		/* 123 */
			0xa1,		/* 161 */
/* 218 */	0xde,		/* 222 */
			0x18,		/* 24 */
/* 220 */	0xe4,		/* 228 */
			0x49,		/* 73 */
/* 222 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 224 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (204) */
/* 226 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 228 */	NdrFcShort( 0x10 ),	/* 16 */
/* 230 */	NdrFcShort( 0x2 ),	/* 2 */
/* 232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 238 */	NdrFcLong( 0x1 ),	/* 1 */
/* 242 */	NdrFcShort( 0xffffff54 ),	/* Offset= -172 (70) */
/* 244 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 246 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 248 */	NdrFcShort( 0x2 ),	/* Offset= 2 (250) */
/* 250 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 252 */	NdrFcLong( 0x43286fea ),	/* 1126723562 */
/* 256 */	NdrFcShort( 0x6997 ),	/* 27031 */
/* 258 */	NdrFcShort( 0x4543 ),	/* 17731 */
/* 260 */	0x80,		/* 128 */
			0x3e,		/* 62 */
/* 262 */	0x60,		/* 96 */
			0xa2,		/* 162 */
/* 264 */	0xc,		/* 12 */
			0x47,		/* 71 */
/* 266 */	0x3d,		/* 61 */
			0xe5,		/* 229 */
/* 268 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 270 */	NdrFcShort( 0x2 ),	/* Offset= 2 (272) */
/* 272 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 274 */	NdrFcLong( 0x96c74ef4 ),	/* -1765323020 */
/* 278 */	NdrFcShort( 0x185d ),	/* 6237 */
/* 280 */	NdrFcShort( 0x4f9a ),	/* 20378 */
/* 282 */	0x8a,		/* 138 */
			0x43,		/* 67 */
/* 284 */	0x4d,		/* 77 */
			0x27,		/* 39 */
/* 286 */	0x23,		/* 35 */
			0x75,		/* 117 */
/* 288 */	0x8e,		/* 142 */
			0xa,		/* 10 */
/* 290 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 292 */	NdrFcLong( 0x426e255c ),	/* 1114514780 */
/* 296 */	NdrFcShort( 0xf1ce ),	/* -3634 */
/* 298 */	NdrFcShort( 0x4d02 ),	/* 19714 */
/* 300 */	0xa9,		/* 169 */
			0x31,		/* 49 */
/* 302 */	0xf9,		/* 249 */
			0xa2,		/* 162 */
/* 304 */	0x54,		/* 84 */
			0xbf,		/* 191 */
/* 306 */	0x7f,		/* 127 */
			0xf,		/* 15 */
/* 308 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 310 */	NdrFcLong( 0x17bf8fa3 ),	/* 398430115 */
/* 314 */	NdrFcShort( 0x4c5a ),	/* 19546 */
/* 316 */	NdrFcShort( 0x49a3 ),	/* 18851 */
/* 318 */	0xb2,		/* 178 */
			0xf8,		/* 248 */
/* 320 */	0x59,		/* 89 */
			0x42,		/* 66 */
/* 322 */	0xe1,		/* 225 */
			0xea,		/* 234 */
/* 324 */	0x28,		/* 40 */
			0x7e,		/* 126 */
/* 326 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 328 */	NdrFcLong( 0x79293cc8 ),	/* 2032745672 */
/* 332 */	NdrFcShort( 0xd9d9 ),	/* -9767 */
/* 334 */	NdrFcShort( 0x43f5 ),	/* 17397 */
/* 336 */	0x97,		/* 151 */
			0xad,		/* 173 */
/* 338 */	0xb,		/* 11 */
			0xcc,		/* 204 */
/* 340 */	0x5a,		/* 90 */
			0x68,		/* 104 */
/* 342 */	0x87,		/* 135 */
			0x76,		/* 118 */
/* 344 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 346 */	NdrFcLong( 0x43286fea ),	/* 1126723562 */
/* 350 */	NdrFcShort( 0x6997 ),	/* 27031 */
/* 352 */	NdrFcShort( 0x4543 ),	/* 17731 */
/* 354 */	0x80,		/* 128 */
			0x3e,		/* 62 */
/* 356 */	0x60,		/* 96 */
			0xa2,		/* 162 */
/* 358 */	0xc,		/* 12 */
			0x47,		/* 71 */
/* 360 */	0x3d,		/* 61 */
			0xe5,		/* 229 */
/* 362 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 364 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 368 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 370 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 372 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 374 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 376 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 378 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 380 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 382 */	NdrFcLong( 0x423238d6 ),	/* 1110587606 */
/* 386 */	NdrFcShort( 0xda42 ),	/* -9662 */
/* 388 */	NdrFcShort( 0x4989 ),	/* 18825 */
/* 390 */	0x96,		/* 150 */
			0xfb,		/* 251 */
/* 392 */	0x6b,		/* 107 */
			0xba,		/* 186 */
/* 394 */	0x26,		/* 38 */
			0xe7,		/* 231 */
/* 396 */	0x2e,		/* 46 */
			0x9,		/* 9 */
/* 398 */	
			0x11, 0x0,	/* FC_RP */
/* 400 */	NdrFcShort( 0x2 ),	/* Offset= 2 (402) */
/* 402 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 404 */	NdrFcShort( 0x10 ),	/* 16 */
/* 406 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 408 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 410 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 412 */	NdrFcShort( 0xfffffeaa ),	/* Offset= -342 (70) */
/* 414 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 416 */	
			0x14, 0x0,	/* FC_FP */
/* 418 */	NdrFcShort( 0x2 ),	/* Offset= 2 (420) */
/* 420 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 422 */	NdrFcShort( 0x4 ),	/* 4 */
/* 424 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 426 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 428 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 430 */	
			0x14, 0x8,	/* FC_FP [simple_pointer] */
/* 432 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 434 */	
			0x12, 0x0,	/* FC_UP */
/* 436 */	NdrFcShort( 0xfffffe60 ),	/* Offset= -416 (20) */
/* 438 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 440 */	NdrFcShort( 0x0 ),	/* 0 */
/* 442 */	NdrFcShort( 0x4 ),	/* 4 */
/* 444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 446 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (434) */
/* 448 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 450 */	NdrFcLong( 0xade2eeb9 ),	/* -1377636679 */
/* 454 */	NdrFcShort( 0xfc85 ),	/* -891 */
/* 456 */	NdrFcShort( 0x4f5b ),	/* 20315 */
/* 458 */	0xb5,		/* 181 */
			0xd9,		/* 217 */
/* 460 */	0xd4,		/* 212 */
			0x31,		/* 49 */
/* 462 */	0xb4,		/* 180 */
			0xaa,		/* 170 */
/* 464 */	0xc3,		/* 195 */
			0x1a,		/* 26 */
/* 466 */	
			0x11, 0x0,	/* FC_RP */
/* 468 */	NdrFcShort( 0x2 ),	/* Offset= 2 (470) */
/* 470 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 472 */	NdrFcShort( 0x10 ),	/* 16 */
/* 474 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 476 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 478 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 480 */	NdrFcShort( 0xfffffe66 ),	/* Offset= -410 (70) */
/* 482 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 484 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 486 */	NdrFcShort( 0xffffff72 ),	/* Offset= -142 (344) */
/* 488 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 490 */	NdrFcLong( 0x6015fd18 ),	/* 1612053784 */
/* 494 */	NdrFcShort( 0x8257 ),	/* -32169 */
/* 496 */	NdrFcShort( 0x4df3 ),	/* 19955 */
/* 498 */	0xac,		/* 172 */
			0x42,		/* 66 */
/* 500 */	0xf0,		/* 240 */
			0x74,		/* 116 */
/* 502 */	0xde,		/* 222 */
			0xdd,		/* 221 */
/* 504 */	0x4c,		/* 76 */
			0xbd,		/* 189 */
/* 506 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 508 */	NdrFcShort( 0x2 ),	/* Offset= 2 (510) */
/* 510 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 512 */	NdrFcLong( 0x6e215ef3 ),	/* 1847680755 */
/* 516 */	NdrFcShort( 0xe44c ),	/* -7092 */
/* 518 */	NdrFcShort( 0x44d1 ),	/* 17617 */
/* 520 */	0xb7,		/* 183 */
			0xba,		/* 186 */
/* 522 */	0xb2,		/* 178 */
			0x40,		/* 64 */
/* 524 */	0x1f,		/* 31 */
			0x7d,		/* 125 */
/* 526 */	0xc2,		/* 194 */
			0x3d,		/* 61 */
/* 528 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 530 */	NdrFcShort( 0x2 ),	/* Offset= 2 (532) */
/* 532 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 534 */	NdrFcLong( 0x70d2dc1e ),	/* 1892867102 */
/* 538 */	NdrFcShort( 0x4dcc ),	/* 19916 */
/* 540 */	NdrFcShort( 0x4786 ),	/* 18310 */
/* 542 */	0xa0,		/* 160 */
			0x72,		/* 114 */
/* 544 */	0x9a,		/* 154 */
			0x3b,		/* 59 */
/* 546 */	0x60,		/* 96 */
			0xc,		/* 12 */
/* 548 */	0x21,		/* 33 */
			0x6b,		/* 107 */
/* 550 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 552 */	NdrFcShort( 0x2 ),	/* Offset= 2 (554) */
/* 554 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 556 */	NdrFcLong( 0x61d986ec ),	/* 1641645804 */
/* 560 */	NdrFcShort( 0x1eac ),	/* 7852 */
/* 562 */	NdrFcShort( 0x46b6 ),	/* 18102 */
/* 564 */	0x90,		/* 144 */
			0xff,		/* 255 */
/* 566 */	0x40,		/* 64 */
			0x2a,		/* 42 */
/* 568 */	0x0,		/* 0 */
			0x8f,		/* 143 */
/* 570 */	0x15,		/* 21 */
			0xd1,		/* 209 */
/* 572 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 574 */	NdrFcShort( 0xfffffef6 ),	/* Offset= -266 (308) */
/* 576 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 578 */	NdrFcLong( 0xbdde0eee ),	/* -1109520658 */
/* 582 */	NdrFcShort( 0x3b8d ),	/* 15245 */
/* 584 */	NdrFcShort( 0x4c82 ),	/* 19586 */
/* 586 */	0xb5,		/* 181 */
			0x29,		/* 41 */
/* 588 */	0x33,		/* 51 */
			0xf1,		/* 241 */
/* 590 */	0x6b,		/* 107 */
			0x42,		/* 66 */
/* 592 */	0x83,		/* 131 */
			0x2e,		/* 46 */
/* 594 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 596 */	NdrFcShort( 0x2 ),	/* Offset= 2 (598) */
/* 598 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 600 */	NdrFcLong( 0xad47a80b ),	/* -1387812853 */
/* 604 */	NdrFcShort( 0xeda7 ),	/* -4697 */
/* 606 */	NdrFcShort( 0x459e ),	/* 17822 */
/* 608 */	0xaf,		/* 175 */
			0x82,		/* 130 */
/* 610 */	0x64,		/* 100 */
			0x7c,		/* 124 */
/* 612 */	0xc9,		/* 201 */
			0xfb,		/* 251 */
/* 614 */	0xaa,		/* 170 */
			0x50,		/* 80 */
/* 616 */	0x11, 0x0,	/* FC_RP */
/* 618 */	NdrFcShort( 0x2 ),	/* Offset= 2 (620) */
/* 620 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 622 */	NdrFcShort( 0x24 ),	/* 36 */
/* 624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 626 */	NdrFcShort( 0x16 ),	/* Offset= 22 (648) */
/* 628 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 630 */	NdrFcShort( 0xfffffef4 ),	/* Offset= -268 (362) */
/* 632 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 634 */	NdrFcShort( 0xffffff3c ),	/* Offset= -196 (438) */
/* 636 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 638 */	NdrFcShort( 0xffffff38 ),	/* Offset= -200 (438) */
/* 640 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 642 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 644 */	NdrFcShort( 0xfffffdc2 ),	/* Offset= -574 (70) */
/* 646 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 648 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 650 */	NdrFcShort( 0x2 ),	/* Offset= 2 (652) */
/* 652 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 654 */	NdrFcLong( 0x8e4bbd34 ),	/* -1907638988 */
/* 658 */	NdrFcShort( 0xa2f4 ),	/* -23820 */
/* 660 */	NdrFcShort( 0x41ef ),	/* 16879 */
/* 662 */	0x87,		/* 135 */
			0xb5,		/* 181 */
/* 664 */	0xc5,		/* 197 */
			0x63,		/* 99 */
/* 666 */	0xb4,		/* 180 */
			0xad,		/* 173 */
/* 668 */	0x6e,		/* 110 */
			0xe7,		/* 231 */
/* 670 */	
			0x14, 0x0,	/* FC_FP */
/* 672 */	NdrFcShort( 0xffffffcc ),	/* Offset= -52 (620) */
/* 674 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 676 */	NdrFcShort( 0x2 ),	/* Offset= 2 (678) */
/* 678 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 680 */	NdrFcLong( 0x978baee7 ),	/* -1752453401 */
/* 684 */	NdrFcShort( 0xbd4c ),	/* -17076 */
/* 686 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 688 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 690 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 692 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 694 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 696 */	
			0x14, 0x0,	/* FC_FP */
/* 698 */	NdrFcShort( 0x2 ),	/* Offset= 2 (700) */
/* 700 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 702 */	NdrFcShort( 0x10 ),	/* 16 */
/* 704 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 706 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 708 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 710 */	NdrFcShort( 0xfffffd80 ),	/* Offset= -640 (70) */
/* 712 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 714 */	
			0x12, 0x0,	/* FC_UP */
/* 716 */	NdrFcShort( 0x3c4 ),	/* Offset= 964 (1680) */
/* 718 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 720 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 722 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 724 */	NdrFcShort( 0x2 ),	/* Offset= 2 (726) */
/* 726 */	NdrFcShort( 0x10 ),	/* 16 */
/* 728 */	NdrFcShort( 0x2f ),	/* 47 */
/* 730 */	NdrFcLong( 0x14 ),	/* 20 */
/* 734 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 736 */	NdrFcLong( 0x3 ),	/* 3 */
/* 740 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 742 */	NdrFcLong( 0x11 ),	/* 17 */
/* 746 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 748 */	NdrFcLong( 0x2 ),	/* 2 */
/* 752 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 754 */	NdrFcLong( 0x4 ),	/* 4 */
/* 758 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 760 */	NdrFcLong( 0x5 ),	/* 5 */
/* 764 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 766 */	NdrFcLong( 0xb ),	/* 11 */
/* 770 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 772 */	NdrFcLong( 0xa ),	/* 10 */
/* 776 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 778 */	NdrFcLong( 0x6 ),	/* 6 */
/* 782 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (1014) */
/* 784 */	NdrFcLong( 0x7 ),	/* 7 */
/* 788 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 790 */	NdrFcLong( 0x8 ),	/* 8 */
/* 794 */	NdrFcShort( 0xfffffe98 ),	/* Offset= -360 (434) */
/* 796 */	NdrFcLong( 0xd ),	/* 13 */
/* 800 */	NdrFcShort( 0xdc ),	/* Offset= 220 (1020) */
/* 802 */	NdrFcLong( 0x9 ),	/* 9 */
/* 806 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (1038) */
/* 808 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 812 */	NdrFcShort( 0xf4 ),	/* Offset= 244 (1056) */
/* 814 */	NdrFcLong( 0x24 ),	/* 36 */
/* 818 */	NdrFcShort( 0x314 ),	/* Offset= 788 (1606) */
/* 820 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 824 */	NdrFcShort( 0x30e ),	/* Offset= 782 (1606) */
/* 826 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 830 */	NdrFcShort( 0x30c ),	/* Offset= 780 (1610) */
/* 832 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 836 */	NdrFcShort( 0x30a ),	/* Offset= 778 (1614) */
/* 838 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 842 */	NdrFcShort( 0x308 ),	/* Offset= 776 (1618) */
/* 844 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 848 */	NdrFcShort( 0x306 ),	/* Offset= 774 (1622) */
/* 850 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 854 */	NdrFcShort( 0x304 ),	/* Offset= 772 (1626) */
/* 856 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 860 */	NdrFcShort( 0x302 ),	/* Offset= 770 (1630) */
/* 862 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 866 */	NdrFcShort( 0x2ec ),	/* Offset= 748 (1614) */
/* 868 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 872 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (1618) */
/* 874 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 878 */	NdrFcShort( 0x2f4 ),	/* Offset= 756 (1634) */
/* 880 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 884 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (1630) */
/* 886 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 890 */	NdrFcShort( 0x2ec ),	/* Offset= 748 (1638) */
/* 892 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 896 */	NdrFcShort( 0x2ea ),	/* Offset= 746 (1642) */
/* 898 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 902 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (1646) */
/* 904 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 908 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1650) */
/* 910 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 914 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (1654) */
/* 916 */	NdrFcLong( 0x10 ),	/* 16 */
/* 920 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 922 */	NdrFcLong( 0x12 ),	/* 18 */
/* 926 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 928 */	NdrFcLong( 0x13 ),	/* 19 */
/* 932 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 934 */	NdrFcLong( 0x15 ),	/* 21 */
/* 938 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 940 */	NdrFcLong( 0x16 ),	/* 22 */
/* 944 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 946 */	NdrFcLong( 0x17 ),	/* 23 */
/* 950 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 952 */	NdrFcLong( 0xe ),	/* 14 */
/* 956 */	NdrFcShort( 0x2c2 ),	/* Offset= 706 (1662) */
/* 958 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 962 */	NdrFcShort( 0x2c6 ),	/* Offset= 710 (1672) */
/* 964 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 968 */	NdrFcShort( 0x2c4 ),	/* Offset= 708 (1676) */
/* 970 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 974 */	NdrFcShort( 0x280 ),	/* Offset= 640 (1614) */
/* 976 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 980 */	NdrFcShort( 0x27e ),	/* Offset= 638 (1618) */
/* 982 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 986 */	NdrFcShort( 0x27c ),	/* Offset= 636 (1622) */
/* 988 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 992 */	NdrFcShort( 0x272 ),	/* Offset= 626 (1618) */
/* 994 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 998 */	NdrFcShort( 0x26c ),	/* Offset= 620 (1618) */
/* 1000 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1004) */
/* 1006 */	NdrFcLong( 0x1 ),	/* 1 */
/* 1010 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1010) */
/* 1012 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1011) */
/* 1014 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1016 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1018 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1020 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1028 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1030 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1032 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1034 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1036 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1038 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1040 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 1044 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1046 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1048 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1050 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1052 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1054 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1056 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1058 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1060) */
/* 1060 */	
			0x12, 0x0,	/* FC_UP */
/* 1062 */	NdrFcShort( 0x20e ),	/* Offset= 526 (1588) */
/* 1064 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 1066 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1068 */	NdrFcShort( 0xa ),	/* 10 */
/* 1070 */	NdrFcLong( 0x8 ),	/* 8 */
/* 1074 */	NdrFcShort( 0x58 ),	/* Offset= 88 (1162) */
/* 1076 */	NdrFcLong( 0xd ),	/* 13 */
/* 1080 */	NdrFcShort( 0x8a ),	/* Offset= 138 (1218) */
/* 1082 */	NdrFcLong( 0x9 ),	/* 9 */
/* 1086 */	NdrFcShort( 0xb8 ),	/* Offset= 184 (1270) */
/* 1088 */	NdrFcLong( 0xc ),	/* 12 */
/* 1092 */	NdrFcShort( 0xe0 ),	/* Offset= 224 (1316) */
/* 1094 */	NdrFcLong( 0x24 ),	/* 36 */
/* 1098 */	NdrFcShort( 0x138 ),	/* Offset= 312 (1410) */
/* 1100 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 1104 */	NdrFcShort( 0x142 ),	/* Offset= 322 (1426) */
/* 1106 */	NdrFcLong( 0x10 ),	/* 16 */
/* 1110 */	NdrFcShort( 0x15a ),	/* Offset= 346 (1456) */
/* 1112 */	NdrFcLong( 0x2 ),	/* 2 */
/* 1116 */	NdrFcShort( 0x172 ),	/* Offset= 370 (1486) */
/* 1118 */	NdrFcLong( 0x3 ),	/* 3 */
/* 1122 */	NdrFcShort( 0x18a ),	/* Offset= 394 (1516) */
/* 1124 */	NdrFcLong( 0x14 ),	/* 20 */
/* 1128 */	NdrFcShort( 0x1a2 ),	/* Offset= 418 (1546) */
/* 1130 */	NdrFcShort( 0xffffffff ),	/* Offset= -1 (1129) */
/* 1132 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1136 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1140 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1142 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1144 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1146 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1148 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1150 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1154 */	0x12, 0x0,	/* FC_UP */
/* 1156 */	NdrFcShort( 0xfffffb90 ),	/* Offset= -1136 (20) */
/* 1158 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1160 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1162 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1164 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1166 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1168 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1170 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1172 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1174 */	0x11, 0x0,	/* FC_RP */
/* 1176 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (1132) */
/* 1178 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1180 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1182 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1184 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1192 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1194 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1196 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1198 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1200 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1204 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1208 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1212 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1214 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1182) */
/* 1216 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1218 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1220 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1222 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1224 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1230) */
/* 1226 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1228 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1230 */	
			0x11, 0x0,	/* FC_RP */
/* 1232 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1200) */
/* 1234 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1236 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 1240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1244 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1246 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1248 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1250 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1252 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1254 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1256 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1258 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1260 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 1264 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1266 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1234) */
/* 1268 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1270 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1272 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1274 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1276 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1282) */
/* 1278 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1280 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1282 */	
			0x11, 0x0,	/* FC_RP */
/* 1284 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1252) */
/* 1286 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1288 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1290 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1292 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1294 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1296 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1298 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1300 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1302 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1304 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1306 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1308 */	0x12, 0x0,	/* FC_UP */
/* 1310 */	NdrFcShort( 0x172 ),	/* Offset= 370 (1680) */
/* 1312 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1314 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1316 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1320 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1322 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1328) */
/* 1324 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1326 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1328 */	
			0x11, 0x0,	/* FC_RP */
/* 1330 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (1286) */
/* 1332 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1334 */	NdrFcLong( 0x2f ),	/* 47 */
/* 1338 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1340 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1342 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 1344 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1346 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 1348 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 1350 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1352 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1354 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1356 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1358 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1360 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1362 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1366 */	NdrFcShort( 0xa ),	/* Offset= 10 (1376) */
/* 1368 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1370 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1372 */	NdrFcShort( 0xffffffd8 ),	/* Offset= -40 (1332) */
/* 1374 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 1376 */	
			0x12, 0x0,	/* FC_UP */
/* 1378 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (1350) */
/* 1380 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1382 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1384 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1386 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1388 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1390 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 1392 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1394 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1396 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1398 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1400 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1402 */	0x12, 0x0,	/* FC_UP */
/* 1404 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (1360) */
/* 1406 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1408 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1410 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1412 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1416 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1422) */
/* 1418 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1420 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1422 */	
			0x11, 0x0,	/* FC_RP */
/* 1424 */	NdrFcShort( 0xffffffd4 ),	/* Offset= -44 (1380) */
/* 1426 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1428 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1430 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1432 */	NdrFcShort( 0xa ),	/* Offset= 10 (1442) */
/* 1434 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 1436 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1438 */	NdrFcShort( 0xfffffaa8 ),	/* Offset= -1368 (70) */
/* 1440 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1442 */	
			0x11, 0x0,	/* FC_RP */
/* 1444 */	NdrFcShort( 0xffffff0c ),	/* Offset= -244 (1200) */
/* 1446 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 1448 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1450 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1452 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1454 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 1456 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1458 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1460 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1462 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1464 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1466 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1468 */	0x12, 0x0,	/* FC_UP */
/* 1470 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (1446) */
/* 1472 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1474 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1476 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 1478 */	NdrFcShort( 0x2 ),	/* 2 */
/* 1480 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1482 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1484 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 1486 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1488 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1490 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1492 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1494 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1496 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1498 */	0x12, 0x0,	/* FC_UP */
/* 1500 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (1476) */
/* 1502 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1504 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1506 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1508 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1510 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1512 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1514 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1516 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1518 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1520 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1522 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1524 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1526 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1528 */	0x12, 0x0,	/* FC_UP */
/* 1530 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (1506) */
/* 1532 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1534 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1536 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 1538 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1540 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 1542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1544 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1546 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 1548 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1550 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 1552 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 1554 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1556 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1558 */	0x12, 0x0,	/* FC_UP */
/* 1560 */	NdrFcShort( 0xffffffe8 ),	/* Offset= -24 (1536) */
/* 1562 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 1564 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 1566 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 1568 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1570 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1572 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1574 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 1576 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1578 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 1580 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 1582 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1584 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (1566) */
/* 1586 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1588 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 1590 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1592 */	NdrFcShort( 0xffffffee ),	/* Offset= -18 (1574) */
/* 1594 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1594) */
/* 1596 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1598 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1600 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1602 */	NdrFcShort( 0xfffffde6 ),	/* Offset= -538 (1064) */
/* 1604 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1606 */	
			0x12, 0x0,	/* FC_UP */
/* 1608 */	NdrFcShort( 0xffffff08 ),	/* Offset= -248 (1360) */
/* 1610 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1612 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 1614 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1616 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1618 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1620 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1622 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1624 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 1626 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1628 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 1630 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1632 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 1634 */	
			0x12, 0x0,	/* FC_UP */
/* 1636 */	NdrFcShort( 0xfffffd92 ),	/* Offset= -622 (1014) */
/* 1638 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1640 */	NdrFcShort( 0xfffffb4a ),	/* Offset= -1206 (434) */
/* 1642 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1644 */	NdrFcShort( 0xfffffe32 ),	/* Offset= -462 (1182) */
/* 1646 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1648 */	NdrFcShort( 0xfffffe62 ),	/* Offset= -414 (1234) */
/* 1650 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1652 */	NdrFcShort( 0xfffffdac ),	/* Offset= -596 (1056) */
/* 1654 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 1656 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1658) */
/* 1658 */	
			0x12, 0x0,	/* FC_UP */
/* 1660 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1680) */
/* 1662 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 1664 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1666 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1668 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1670 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1672 */	
			0x12, 0x0,	/* FC_UP */
/* 1674 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (1662) */
/* 1676 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1678 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1680 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1682 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1686 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1686) */
/* 1688 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1690 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1692 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1694 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1696 */	NdrFcShort( 0xfffffc2e ),	/* Offset= -978 (718) */
/* 1698 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1700 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1702 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1704 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1708 */	NdrFcShort( 0xfffffc1e ),	/* Offset= -994 (714) */
/* 1710 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1712 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1714) */
/* 1714 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1716 */	NdrFcLong( 0x8d14bca6 ),	/* -1928020826 */
/* 1720 */	NdrFcShort( 0x34ce ),	/* 13518 */
/* 1722 */	NdrFcShort( 0x4efe ),	/* 20222 */
/* 1724 */	0xac,		/* 172 */
			0x7e,		/* 126 */
/* 1726 */	0xa,		/* 10 */
			0xbc,		/* 188 */
/* 1728 */	0x61,		/* 97 */
			0xda,		/* 218 */
/* 1730 */	0xdb,		/* 219 */
			0x20,		/* 32 */
/* 1732 */	
			0x11, 0x0,	/* FC_RP */
/* 1734 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1754) */
/* 1736 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1738 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 1742 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 1744 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 1746 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 1748 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 1750 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 1752 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 1754 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1756 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1758 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1760 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1762 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1764 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1766 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1768 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1736) */
/* 1770 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1772 */	
			0x11, 0x0,	/* FC_RP */
/* 1774 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1794) */
/* 1776 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1778 */	NdrFcLong( 0x426e255c ),	/* 1114514780 */
/* 1782 */	NdrFcShort( 0xf1ce ),	/* -3634 */
/* 1784 */	NdrFcShort( 0x4d02 ),	/* 19714 */
/* 1786 */	0xa9,		/* 169 */
			0x31,		/* 49 */
/* 1788 */	0xf9,		/* 249 */
			0xa2,		/* 162 */
/* 1790 */	0x54,		/* 84 */
			0xbf,		/* 191 */
/* 1792 */	0x7f,		/* 127 */
			0xf,		/* 15 */
/* 1794 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 1796 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1798 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1800 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1802 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 1804 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1806 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1808 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (1776) */
/* 1810 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1812 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1814 */	NdrFcLong( 0xade2eeb9 ),	/* -1377636679 */
/* 1818 */	NdrFcShort( 0xfc85 ),	/* -891 */
/* 1820 */	NdrFcShort( 0x4f5b ),	/* 20315 */
/* 1822 */	0xb5,		/* 181 */
			0xd9,		/* 217 */
/* 1824 */	0xd4,		/* 212 */
			0x31,		/* 49 */
/* 1826 */	0xb4,		/* 180 */
			0xaa,		/* 170 */
/* 1828 */	0xc3,		/* 195 */
			0x1a,		/* 26 */
/* 1830 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1832 */	NdrFcLong( 0x6015fd18 ),	/* 1612053784 */
/* 1836 */	NdrFcShort( 0x8257 ),	/* -32169 */
/* 1838 */	NdrFcShort( 0x4df3 ),	/* 19955 */
/* 1840 */	0xac,		/* 172 */
			0x42,		/* 66 */
/* 1842 */	0xf0,		/* 240 */
			0x74,		/* 116 */
/* 1844 */	0xde,		/* 222 */
			0xdd,		/* 221 */
/* 1846 */	0x4c,		/* 76 */
			0xbd,		/* 189 */
/* 1848 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1850 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1852) */
/* 1852 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1854 */	NdrFcLong( 0x6e215ef3 ),	/* 1847680755 */
/* 1858 */	NdrFcShort( 0xe44c ),	/* -7092 */
/* 1860 */	NdrFcShort( 0x44d1 ),	/* 17617 */
/* 1862 */	0xb7,		/* 183 */
			0xba,		/* 186 */
/* 1864 */	0xb2,		/* 178 */
			0x40,		/* 64 */
/* 1866 */	0x1f,		/* 31 */
			0x7d,		/* 125 */
/* 1868 */	0xc2,		/* 194 */
			0x3d,		/* 61 */
/* 1870 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1872 */	NdrFcLong( 0x423238d6 ),	/* 1110587606 */
/* 1876 */	NdrFcShort( 0xda42 ),	/* -9662 */
/* 1878 */	NdrFcShort( 0x4989 ),	/* 18825 */
/* 1880 */	0x96,		/* 150 */
			0xfb,		/* 251 */
/* 1882 */	0x6b,		/* 107 */
			0xba,		/* 186 */
/* 1884 */	0x26,		/* 38 */
			0xe7,		/* 231 */
/* 1886 */	0x2e,		/* 46 */
			0x9,		/* 9 */
/* 1888 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1890 */	NdrFcLong( 0x79293cc8 ),	/* 2032745672 */
/* 1894 */	NdrFcShort( 0xd9d9 ),	/* -9767 */
/* 1896 */	NdrFcShort( 0x43f5 ),	/* 17397 */
/* 1898 */	0x97,		/* 151 */
			0xad,		/* 173 */
/* 1900 */	0xb,		/* 11 */
			0xcc,		/* 204 */
/* 1902 */	0x5a,		/* 90 */
			0x68,		/* 104 */
/* 1904 */	0x87,		/* 135 */
			0x76,		/* 118 */
/* 1906 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1908 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1910) */
/* 1910 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1912 */	NdrFcLong( 0x43286fea ),	/* 1126723562 */
/* 1916 */	NdrFcShort( 0x6997 ),	/* 27031 */
/* 1918 */	NdrFcShort( 0x4543 ),	/* 17731 */
/* 1920 */	0x80,		/* 128 */
			0x3e,		/* 62 */
/* 1922 */	0x60,		/* 96 */
			0xa2,		/* 162 */
/* 1924 */	0xc,		/* 12 */
			0x47,		/* 71 */
/* 1926 */	0x3d,		/* 61 */
			0xe5,		/* 229 */
/* 1928 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1930 */	NdrFcLong( 0xba105b52 ),	/* -1173333166 */
/* 1934 */	NdrFcShort( 0x12f1 ),	/* 4849 */
/* 1936 */	NdrFcShort( 0x4038 ),	/* 16440 */
/* 1938 */	0xae,		/* 174 */
			0x64,		/* 100 */
/* 1940 */	0xd9,		/* 217 */
			0x57,		/* 87 */
/* 1942 */	0x85,		/* 133 */
			0x87,		/* 135 */
/* 1944 */	0x4c,		/* 76 */
			0x47,		/* 71 */
/* 1946 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1948 */	NdrFcLong( 0xd5168050 ),	/* -719945648 */
/* 1952 */	NdrFcShort( 0xa57a ),	/* -23174 */
/* 1954 */	NdrFcShort( 0x465c ),	/* 18012 */
/* 1956 */	0xbe,		/* 190 */
			0xa9,		/* 169 */
/* 1958 */	0x97,		/* 151 */
			0x4f,		/* 79 */
/* 1960 */	0x40,		/* 64 */
			0x5e,		/* 94 */
/* 1962 */	0xba,		/* 186 */
			0x13,		/* 19 */
/* 1964 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1966 */	NdrFcLong( 0x423238d6 ),	/* 1110587606 */
/* 1970 */	NdrFcShort( 0xda42 ),	/* -9662 */
/* 1972 */	NdrFcShort( 0x4989 ),	/* 18825 */
/* 1974 */	0x96,		/* 150 */
			0xfb,		/* 251 */
/* 1976 */	0x6b,		/* 107 */
			0xba,		/* 186 */
/* 1978 */	0x26,		/* 38 */
			0xe7,		/* 231 */
/* 1980 */	0x2e,		/* 46 */
			0x9,		/* 9 */
/* 1982 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1984 */	NdrFcShort( 0xffffffc8 ),	/* Offset= -56 (1928) */
/* 1986 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 1988 */	NdrFcShort( 0x2 ),	/* Offset= 2 (1990) */
/* 1990 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1992 */	NdrFcLong( 0x931516ad ),	/* -1827334483 */
/* 1996 */	NdrFcShort( 0xb600 ),	/* -18944 */
/* 1998 */	NdrFcShort( 0x419c ),	/* 16796 */
/* 2000 */	0x88,		/* 136 */
			0xfc,		/* 252 */
/* 2002 */	0xdc,		/* 220 */
			0xf5,		/* 245 */
/* 2004 */	0x18,		/* 24 */
			0x3b,		/* 59 */
/* 2006 */	0x5f,		/* 95 */
			0xa9,		/* 169 */
/* 2008 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2010 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2012) */
/* 2012 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2014 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 2018 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 2020 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 2022 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 2024 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 2026 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 2028 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 2030 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2032 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2034) */
/* 2034 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2036 */	NdrFcLong( 0xf6b37e0 ),	/* 258684896 */
/* 2040 */	NdrFcShort( 0xfcfe ),	/* -770 */
/* 2042 */	NdrFcShort( 0x44d9 ),	/* 17625 */
/* 2044 */	0x91,		/* 145 */
			0x12,		/* 18 */
/* 2046 */	0x39,		/* 57 */
			0x4c,		/* 76 */
/* 2048 */	0xa9,		/* 169 */
			0xb9,		/* 185 */
/* 2050 */	0x21,		/* 33 */
			0x14,		/* 20 */
/* 2052 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 2054 */	NdrFcShort( 0xfffffa66 ),	/* Offset= -1434 (620) */
/* 2056 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2058 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2060) */
/* 2060 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2062 */	NdrFcLong( 0xfc1cd9a ),	/* 264359322 */
/* 2066 */	NdrFcShort( 0xb912 ),	/* -18158 */
/* 2068 */	NdrFcShort( 0x405c ),	/* 16476 */
/* 2070 */	0xa0,		/* 160 */
			0x4c,		/* 76 */
/* 2072 */	0x43,		/* 67 */
			0xce,		/* 206 */
/* 2074 */	0x2,		/* 2 */
			0xcd,		/* 205 */
/* 2076 */	0x7d,		/* 125 */
			0xf2,		/* 242 */
/* 2078 */	
			0x11, 0x0,	/* FC_RP */
/* 2080 */	NdrFcShort( 0xfffff7fe ),	/* Offset= -2050 (30) */
/* 2082 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 2084 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 2086 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2088 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2090) */
/* 2090 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2092 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 2096 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 2098 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 2100 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 2102 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 2104 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 2106 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 2108 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2110 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2112) */
/* 2112 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2114 */	NdrFcLong( 0xf6b37e0 ),	/* 258684896 */
/* 2118 */	NdrFcShort( 0xfcfe ),	/* -770 */
/* 2120 */	NdrFcShort( 0x44d9 ),	/* 17625 */
/* 2122 */	0x91,		/* 145 */
			0x12,		/* 18 */
/* 2124 */	0x39,		/* 57 */
			0x4c,		/* 76 */
/* 2126 */	0xa9,		/* 169 */
			0xb9,		/* 185 */
/* 2128 */	0x21,		/* 33 */
			0x14,		/* 20 */
/* 2130 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2132 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2134) */
/* 2134 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2136 */	NdrFcLong( 0xd533d975 ),	/* -718022283 */
/* 2140 */	NdrFcShort( 0x3f32 ),	/* 16178 */
/* 2142 */	NdrFcShort( 0x4876 ),	/* 18550 */
/* 2144 */	0xab,		/* 171 */
			0xd0,		/* 208 */
/* 2146 */	0x6d,		/* 109 */
			0x37,		/* 55 */
/* 2148 */	0xfd,		/* 253 */
			0xa5,		/* 165 */
/* 2150 */	0x63,		/* 99 */
			0xe7,		/* 231 */
/* 2152 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2154 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2156) */
/* 2156 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2158 */	NdrFcLong( 0x74570ef7 ),	/* 1951862519 */
/* 2162 */	NdrFcShort( 0x2486 ),	/* 9350 */
/* 2164 */	NdrFcShort( 0x4089 ),	/* 16521 */
/* 2166 */	0x80,		/* 128 */
			0xc,		/* 12 */
/* 2168 */	0x56,		/* 86 */
			0xe3,		/* 227 */
/* 2170 */	0x82,		/* 130 */
			0x9b,		/* 155 */
/* 2172 */	0x5c,		/* 92 */
			0xa4,		/* 164 */
/* 2174 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2176 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2178) */
/* 2178 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2180 */	NdrFcLong( 0xf7473fd0 ),	/* -146325552 */
/* 2184 */	NdrFcShort( 0x7f75 ),	/* 32629 */
/* 2186 */	NdrFcShort( 0x478d ),	/* 18317 */
/* 2188 */	0x8d,		/* 141 */
			0x85,		/* 133 */
/* 2190 */	0xa4,		/* 164 */
			0x85,		/* 133 */
/* 2192 */	0x20,		/* 32 */
			0x4e,		/* 78 */
/* 2194 */	0x7a,		/* 122 */
			0x2d,		/* 45 */
/* 2196 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2198 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2200) */
/* 2200 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2202 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 2206 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 2208 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 2210 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 2212 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 2214 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 2216 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 2218 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2220 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2222) */
/* 2222 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2224 */	NdrFcLong( 0x1606dd73 ),	/* 369548659 */
/* 2228 */	NdrFcShort( 0x5d5f ),	/* 23903 */
/* 2230 */	NdrFcShort( 0x405c ),	/* 16476 */
/* 2232 */	0xb4,		/* 180 */
			0xf4,		/* 244 */
/* 2234 */	0xce,		/* 206 */
			0x32,		/* 50 */
/* 2236 */	0xba,		/* 186 */
			0xba,		/* 186 */
/* 2238 */	0x25,		/* 37 */
			0x1,		/* 1 */
/* 2240 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2242 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2244) */
/* 2244 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2246 */	NdrFcLong( 0x931516ad ),	/* -1827334483 */
/* 2250 */	NdrFcShort( 0xb600 ),	/* -18944 */
/* 2252 */	NdrFcShort( 0x419c ),	/* 16796 */
/* 2254 */	0x88,		/* 136 */
			0xfc,		/* 252 */
/* 2256 */	0xdc,		/* 220 */
			0xf5,		/* 245 */
/* 2258 */	0x18,		/* 24 */
			0x3b,		/* 59 */
/* 2260 */	0x5f,		/* 95 */
			0xa9,		/* 169 */
/* 2262 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 2264 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2266) */
/* 2266 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2268 */	NdrFcShort( 0x38 ),	/* 56 */
/* 2270 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2272 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2272) */
/* 2274 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2276 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff739 ),	/* Offset= -2247 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2280 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff735 ),	/* Offset= -2251 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2284 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff731 ),	/* Offset= -2255 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2288 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff7f1 ),	/* Offset= -2063 (226) */
			0x8,		/* FC_LONG */
/* 2292 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2294 */	NdrFcShort( 0xfffff728 ),	/* Offset= -2264 (30) */
/* 2296 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2298 */	NdrFcShort( 0xfffffd24 ),	/* Offset= -732 (1566) */
/* 2300 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2302 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2304 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2306) */
/* 2306 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2308 */	NdrFcLong( 0x8d14bca6 ),	/* -1928020826 */
/* 2312 */	NdrFcShort( 0x34ce ),	/* 13518 */
/* 2314 */	NdrFcShort( 0x4efe ),	/* 20222 */
/* 2316 */	0xac,		/* 172 */
			0x7e,		/* 126 */
/* 2318 */	0xa,		/* 10 */
			0xbc,		/* 188 */
/* 2320 */	0x61,		/* 97 */
			0xda,		/* 218 */
/* 2322 */	0xdb,		/* 219 */
			0x20,		/* 32 */
/* 2324 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2326 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2328) */
/* 2328 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2330 */	NdrFcLong( 0x17bf8fa3 ),	/* 398430115 */
/* 2334 */	NdrFcShort( 0x4c5a ),	/* 19546 */
/* 2336 */	NdrFcShort( 0x49a3 ),	/* 18851 */
/* 2338 */	0xb2,		/* 178 */
			0xf8,		/* 248 */
/* 2340 */	0x59,		/* 89 */
			0x42,		/* 66 */
/* 2342 */	0xe1,		/* 225 */
			0xea,		/* 234 */
/* 2344 */	0x28,		/* 40 */
			0x7e,		/* 126 */
/* 2346 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2348 */	NdrFcLong( 0xade2eeb9 ),	/* -1377636679 */
/* 2352 */	NdrFcShort( 0xfc85 ),	/* -891 */
/* 2354 */	NdrFcShort( 0x4f5b ),	/* 20315 */
/* 2356 */	0xb5,		/* 181 */
			0xd9,		/* 217 */
/* 2358 */	0xd4,		/* 212 */
			0x31,		/* 49 */
/* 2360 */	0xb4,		/* 180 */
			0xaa,		/* 170 */
/* 2362 */	0xc3,		/* 195 */
			0x1a,		/* 26 */
/* 2364 */	
			0x11, 0x0,	/* FC_RP */
/* 2366 */	NdrFcShort( 0xfffff97e ),	/* Offset= -1666 (700) */
/* 2368 */	
			0x11, 0x0,	/* FC_RP */
/* 2370 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2372) */
/* 2372 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 2374 */	NdrFcShort( 0x4 ),	/* 4 */
/* 2376 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2378 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2380 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2382 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2384 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 2386 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 2388 */	NdrFcShort( 0xfffff78e ),	/* Offset= -2162 (226) */
/* 2390 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2392 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2394) */
/* 2394 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2396 */	NdrFcLong( 0xd30dc12 ),	/* 221305874 */
/* 2400 */	NdrFcShort( 0xc4f8 ),	/* -15112 */
/* 2402 */	NdrFcShort( 0x433d ),	/* 17213 */
/* 2404 */	0x9f,		/* 159 */
			0xcc,		/* 204 */
/* 2406 */	0x9f,		/* 159 */
			0xf1,		/* 241 */
/* 2408 */	0x17,		/* 23 */
			0xe5,		/* 229 */
/* 2410 */	0xe5,		/* 229 */
			0xf4,		/* 244 */
/* 2412 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2414 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2416) */
/* 2416 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2418 */	NdrFcLong( 0x79293cc8 ),	/* 2032745672 */
/* 2422 */	NdrFcShort( 0xd9d9 ),	/* -9767 */
/* 2424 */	NdrFcShort( 0x43f5 ),	/* 17397 */
/* 2426 */	0x97,		/* 151 */
			0xad,		/* 173 */
/* 2428 */	0xb,		/* 11 */
			0xcc,		/* 204 */
/* 2430 */	0x5a,		/* 90 */
			0x68,		/* 104 */
/* 2432 */	0x87,		/* 135 */
			0x76,		/* 118 */
/* 2434 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2436 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2438) */
/* 2438 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2440 */	NdrFcLong( 0xd30dc12 ),	/* 221305874 */
/* 2444 */	NdrFcShort( 0xc4f8 ),	/* -15112 */
/* 2446 */	NdrFcShort( 0x433d ),	/* 17213 */
/* 2448 */	0x9f,		/* 159 */
			0xcc,		/* 204 */
/* 2450 */	0x9f,		/* 159 */
			0xf1,		/* 241 */
/* 2452 */	0x17,		/* 23 */
			0xe5,		/* 229 */
/* 2454 */	0xe5,		/* 229 */
			0xf4,		/* 244 */
/* 2456 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2458 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2460) */
/* 2460 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2462 */	NdrFcLong( 0x43286fea ),	/* 1126723562 */
/* 2466 */	NdrFcShort( 0x6997 ),	/* 27031 */
/* 2468 */	NdrFcShort( 0x4543 ),	/* 17731 */
/* 2470 */	0x80,		/* 128 */
			0x3e,		/* 62 */
/* 2472 */	0x60,		/* 96 */
			0xa2,		/* 162 */
/* 2474 */	0xc,		/* 12 */
			0x47,		/* 71 */
/* 2476 */	0x3d,		/* 61 */
			0xe5,		/* 229 */
/* 2478 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2480 */	NdrFcLong( 0xade2eeb9 ),	/* -1377636679 */
/* 2484 */	NdrFcShort( 0xfc85 ),	/* -891 */
/* 2486 */	NdrFcShort( 0x4f5b ),	/* 20315 */
/* 2488 */	0xb5,		/* 181 */
			0xd9,		/* 217 */
/* 2490 */	0xd4,		/* 212 */
			0x31,		/* 49 */
/* 2492 */	0xb4,		/* 180 */
			0xaa,		/* 170 */
/* 2494 */	0xc3,		/* 195 */
			0x1a,		/* 26 */
/* 2496 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2498 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2500) */
/* 2500 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2502 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 2506 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 2508 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 2510 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 2512 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 2514 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 2516 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 2518 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2520 */	NdrFcLong( 0xd5168050 ),	/* -719945648 */
/* 2524 */	NdrFcShort( 0xa57a ),	/* -23174 */
/* 2526 */	NdrFcShort( 0x465c ),	/* 18012 */
/* 2528 */	0xbe,		/* 190 */
			0xa9,		/* 169 */
/* 2530 */	0x97,		/* 151 */
			0x4f,		/* 79 */
/* 2532 */	0x40,		/* 64 */
			0x5e,		/* 94 */
/* 2534 */	0xba,		/* 186 */
			0x13,		/* 19 */
/* 2536 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2538 */	NdrFcLong( 0xbdde0eee ),	/* -1109520658 */
/* 2542 */	NdrFcShort( 0x3b8d ),	/* 15245 */
/* 2544 */	NdrFcShort( 0x4c82 ),	/* 19586 */
/* 2546 */	0xb5,		/* 181 */
			0x29,		/* 41 */
/* 2548 */	0x33,		/* 51 */
			0xf1,		/* 241 */
/* 2550 */	0x6b,		/* 107 */
			0x42,		/* 66 */
/* 2552 */	0x83,		/* 131 */
			0x2e,		/* 46 */
/* 2554 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2556 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2558) */
/* 2558 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2560 */	NdrFcLong( 0xad47a80b ),	/* -1387812853 */
/* 2564 */	NdrFcShort( 0xeda7 ),	/* -4697 */
/* 2566 */	NdrFcShort( 0x459e ),	/* 17822 */
/* 2568 */	0xaf,		/* 175 */
			0x82,		/* 130 */
/* 2570 */	0x64,		/* 100 */
			0x7c,		/* 124 */
/* 2572 */	0xc9,		/* 201 */
			0xfb,		/* 251 */
/* 2574 */	0xaa,		/* 170 */
			0x50,		/* 80 */
/* 2576 */	0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2578 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2580) */
/* 2580 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2582 */	NdrFcLong( 0x925837d1 ),	/* -1839712303 */
/* 2586 */	NdrFcShort( 0x3aa1 ),	/* 15009 */
/* 2588 */	NdrFcShort( 0x451a ),	/* 17690 */
/* 2590 */	0xb7,		/* 183 */
			0xfe,		/* 254 */
/* 2592 */	0xcc,		/* 204 */
			0x4,		/* 4 */
/* 2594 */	0xbb,		/* 187 */
			0x42,		/* 66 */
/* 2596 */	0xcf,		/* 207 */
			0xb8,		/* 184 */
/* 2598 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2600 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 2604 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 2606 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 2608 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 2610 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 2612 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 2614 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 2616 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2618 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2620) */
/* 2620 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2622 */	NdrFcLong( 0xe5b017fe ),	/* -441444354 */
/* 2626 */	NdrFcShort( 0xdfb0 ),	/* -8272 */
/* 2628 */	NdrFcShort( 0x411c ),	/* 16668 */
/* 2630 */	0x82,		/* 130 */
			0x66,		/* 102 */
/* 2632 */	0x7c,		/* 124 */
			0x64,		/* 100 */
/* 2634 */	0xd6,		/* 214 */
			0xf5,		/* 245 */
/* 2636 */	0x19,		/* 25 */
			0xf8,		/* 248 */
/* 2638 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2640 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2642) */
/* 2642 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2644 */	NdrFcLong( 0x4c4a2835 ),	/* 1279928373 */
/* 2648 */	NdrFcShort( 0x682e ),	/* 26670 */
/* 2650 */	NdrFcShort( 0x4ce1 ),	/* 19681 */
/* 2652 */	0xae,		/* 174 */
			0xbc,		/* 188 */
/* 2654 */	0x1e,		/* 30 */
			0x6b,		/* 107 */
/* 2656 */	0x3a,		/* 58 */
			0x16,		/* 22 */
/* 2658 */	0x5b,		/* 91 */
			0x44,		/* 68 */
/* 2660 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2662 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2664) */
/* 2664 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2666 */	NdrFcLong( 0x978baee7 ),	/* -1752453401 */
/* 2670 */	NdrFcShort( 0xbd4c ),	/* -17076 */
/* 2672 */	NdrFcShort( 0x11d2 ),	/* 4562 */
/* 2674 */	0x92,		/* 146 */
			0x38,		/* 56 */
/* 2676 */	0x0,		/* 0 */
			0xa0,		/* 160 */
/* 2678 */	0x24,		/* 36 */
			0x48,		/* 72 */
/* 2680 */	0x79,		/* 121 */
			0x9a,		/* 154 */
/* 2682 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2684 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 2688 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 2690 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 2692 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 2694 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 2696 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 2698 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 2700 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2702 */	NdrFcLong( 0x1412926f ),	/* 336761455 */
/* 2706 */	NdrFcShort( 0x5dd6 ),	/* 24022 */
/* 2708 */	NdrFcShort( 0x4e58 ),	/* 20056 */
/* 2710 */	0xb6,		/* 182 */
			0x48,		/* 72 */
/* 2712 */	0xe1,		/* 225 */
			0xc6,		/* 198 */
/* 2714 */	0x3e,		/* 62 */
			0x1,		/* 1 */
/* 2716 */	0x3d,		/* 61 */
			0x51,		/* 81 */
/* 2718 */	0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2720 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2722) */
/* 2722 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2724 */	NdrFcLong( 0x9b13f80d ),	/* -1693190131 */
/* 2728 */	NdrFcShort( 0xcfc6 ),	/* -12346 */
/* 2730 */	NdrFcShort( 0x4b78 ),	/* 19320 */
/* 2732 */	0x81,		/* 129 */
			0xef,		/* 239 */
/* 2734 */	0x1f,		/* 31 */
			0x7c,		/* 124 */
/* 2736 */	0xc3,		/* 195 */
			0x3f,		/* 63 */
/* 2738 */	0x76,		/* 118 */
			0x39,		/* 57 */
/* 2740 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2742 */	NdrFcShort( 0xffffffc4 ),	/* Offset= -60 (2682) */
/* 2744 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2746 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 2750 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 2752 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 2754 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 2756 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 2758 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 2760 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 2762 */	
			0x14, 0x0,	/* FC_FP */
/* 2764 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2766) */
/* 2766 */	
			0x1c,		/* FC_CVARRAY */
			0x3,		/* 3 */
/* 2768 */	NdrFcShort( 0x10 ),	/* 16 */
/* 2770 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2772 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 2774 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 2776 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 2778 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2780 */	NdrFcShort( 0xfffff56a ),	/* Offset= -2710 (70) */
/* 2782 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2784 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2786 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2788) */
/* 2788 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2790 */	NdrFcLong( 0x98bbba48 ),	/* -1732527544 */
/* 2794 */	NdrFcShort( 0x4c4d ),	/* 19533 */
/* 2796 */	NdrFcShort( 0x4fff ),	/* 20479 */
/* 2798 */	0x83,		/* 131 */
			0x40,		/* 64 */
/* 2800 */	0x60,		/* 96 */
			0x97,		/* 151 */
/* 2802 */	0xbe,		/* 190 */
			0xc9,		/* 201 */
/* 2804 */	0xc8,		/* 200 */
			0x94,		/* 148 */
/* 2806 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2808 */	NdrFcShort( 0xffffffc0 ),	/* Offset= -64 (2744) */
/* 2810 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2812 */	NdrFcLong( 0x1412926f ),	/* 336761455 */
/* 2816 */	NdrFcShort( 0x5dd6 ),	/* 24022 */
/* 2818 */	NdrFcShort( 0x4e58 ),	/* 20056 */
/* 2820 */	0xb6,		/* 182 */
			0x48,		/* 72 */
/* 2822 */	0xe1,		/* 225 */
			0xc6,		/* 198 */
/* 2824 */	0x3e,		/* 62 */
			0x1,		/* 1 */
/* 2826 */	0x3d,		/* 61 */
			0x51,		/* 81 */
/* 2828 */	0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 2830 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2832) */
/* 2832 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 2834 */	NdrFcShort( 0x1c ),	/* 28 */
/* 2836 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2838 */	NdrFcShort( 0x0 ),	/* Offset= 0 (2838) */
/* 2840 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2842 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 2844 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2846 */	NdrFcShort( 0xfffff500 ),	/* Offset= -2816 (30) */
/* 2848 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2850 */	NdrFcShort( 0xfffff4fc ),	/* Offset= -2820 (30) */
/* 2852 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2854 */	NdrFcShort( 0xfffff4f8 ),	/* Offset= -2824 (30) */
/* 2856 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2858 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2860 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2862) */
/* 2862 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2864 */	NdrFcLong( 0x88d2f75b ),	/* -1999440037 */
/* 2868 */	NdrFcShort( 0xd329 ),	/* -11479 */
/* 2870 */	NdrFcShort( 0x4e03 ),	/* 19971 */
/* 2872 */	0x9b,		/* 155 */
			0x75,		/* 117 */
/* 2874 */	0x20,		/* 32 */
			0x1f,		/* 31 */
/* 2876 */	0x77,		/* 119 */
			0x82,		/* 130 */
/* 2878 */	0xd8,		/* 216 */
			0xbd,		/* 189 */
/* 2880 */	
			0x14, 0x0,	/* FC_FP */
/* 2882 */	NdrFcShort( 0x14 ),	/* Offset= 20 (2902) */
/* 2884 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2886 */	NdrFcLong( 0x10b793ac ),	/* 280466348 */
/* 2890 */	NdrFcShort( 0xc47 ),	/* 3143 */
/* 2892 */	NdrFcShort( 0x4679 ),	/* 18041 */
/* 2894 */	0x84,		/* 132 */
			0x54,		/* 84 */
/* 2896 */	0xad,		/* 173 */
			0xb3,		/* 179 */
/* 2898 */	0x6f,		/* 111 */
			0x29,		/* 41 */
/* 2900 */	0xf8,		/* 248 */
			0x2,		/* 2 */
/* 2902 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2904 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2906 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2908 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2910 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2912 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 2914 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2916 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (2884) */
/* 2918 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2920 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 2922 */	NdrFcShort( 0x14 ),	/* Offset= 20 (2942) */
/* 2924 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 2926 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 2930 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 2932 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 2934 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 2936 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 2938 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 2940 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 2942 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 2944 */	NdrFcShort( 0x20 ),	/* 32 */
/* 2946 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2948 */	NdrFcShort( 0x1a ),	/* Offset= 26 (2974) */
/* 2950 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2952 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff495 ),	/* Offset= -2923 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2956 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff491 ),	/* Offset= -2927 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2960 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff48d ),	/* Offset= -2931 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2964 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff489 ),	/* Offset= -2935 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 2968 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffd3 ),	/* Offset= -45 (2924) */
			0xb,		/* FC_HYPER */
/* 2972 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2974 */	
			0x14, 0x0,	/* FC_FP */
/* 2976 */	NdrFcShort( 0x2 ),	/* Offset= 2 (2978) */
/* 2978 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 2980 */	NdrFcShort( 0x0 ),	/* 0 */
/* 2982 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2984 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2986 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 2988 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 2990 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 2992 */	NdrFcShort( 0xffffff94 ),	/* Offset= -108 (2884) */
/* 2994 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 2996 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 2998 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3000) */
/* 3000 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3002 */	NdrFcLong( 0x6c7072c3 ),	/* 1819308739 */
/* 3006 */	NdrFcShort( 0x3ac4 ),	/* 15044 */
/* 3008 */	NdrFcShort( 0x408f ),	/* 16527 */
/* 3010 */	0xa6,		/* 166 */
			0x80,		/* 128 */
/* 3012 */	0xfc,		/* 252 */
			0x5a,		/* 90 */
/* 3014 */	0x2f,		/* 47 */
			0x96,		/* 150 */
/* 3016 */	0x90,		/* 144 */
			0x3e,		/* 62 */
/* 3018 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3020 */	NdrFcShort( 0xffffffa0 ),	/* Offset= -96 (2924) */
/* 3022 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3024 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3026) */
/* 3026 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3028 */	NdrFcLong( 0x1ab276dd ),	/* 447903453 */
/* 3032 */	NdrFcShort( 0xf27b ),	/* -3461 */
/* 3034 */	NdrFcShort( 0x4445 ),	/* 17477 */
/* 3036 */	0x82,		/* 130 */
			0x5d,		/* 93 */
/* 3038 */	0x5d,		/* 93 */
			0xf0,		/* 240 */
/* 3040 */	0xb4,		/* 180 */
			0xa0,		/* 160 */
/* 3042 */	0x4a,		/* 74 */
			0x3a,		/* 58 */
/* 3044 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3046 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3048) */
/* 3048 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3050 */	NdrFcLong( 0x10b793ac ),	/* 280466348 */
/* 3054 */	NdrFcShort( 0xc47 ),	/* 3143 */
/* 3056 */	NdrFcShort( 0x4679 ),	/* 18041 */
/* 3058 */	0x84,		/* 132 */
			0x54,		/* 84 */
/* 3060 */	0xad,		/* 173 */
			0xb3,		/* 179 */
/* 3062 */	0x6f,		/* 111 */
			0x29,		/* 41 */
/* 3064 */	0xf8,		/* 248 */
			0x2,		/* 2 */
/* 3066 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3068 */	NdrFcShort( 0x6 ),	/* Offset= 6 (3074) */
/* 3070 */	
			0x13, 0x0,	/* FC_OP */
/* 3072 */	NdrFcShort( 0xfffffa90 ),	/* Offset= -1392 (1680) */
/* 3074 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 3076 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3078 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3080 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3082 */	NdrFcShort( 0xfffffff4 ),	/* Offset= -12 (3070) */
/* 3084 */	
			0x11, 0x0,	/* FC_RP */
/* 3086 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3088) */
/* 3088 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3090 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3092 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3094 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3096 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3098 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 3100 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3102 */	NdrFcShort( 0xffffffca ),	/* Offset= -54 (3048) */
/* 3104 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3106 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3108 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3110) */
/* 3110 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 3112 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3116 */	NdrFcShort( 0x16 ),	/* Offset= 22 (3138) */
/* 3118 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3120 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff3ed ),	/* Offset= -3091 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3124 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff3e9 ),	/* Offset= -3095 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3128 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff3e5 ),	/* Offset= -3099 (30) */
			0xb,		/* FC_HYPER */
/* 3132 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3134 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffa9 ),	/* Offset= -87 (3048) */
			0x5b,		/* FC_END */
/* 3138 */	
			0x11, 0x0,	/* FC_RP */
/* 3140 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3142) */
/* 3142 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3146 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3148 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3150 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3152 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3154 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3156 */	NdrFcShort( 0xffffff94 ),	/* Offset= -108 (3048) */
/* 3158 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3160 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3162 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3164) */
/* 3164 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3166 */	NdrFcLong( 0xe459dd12 ),	/* -463872750 */
/* 3170 */	NdrFcShort( 0x864f ),	/* -31153 */
/* 3172 */	NdrFcShort( 0x4aaa ),	/* 19114 */
/* 3174 */	0xab,		/* 171 */
			0xc1,		/* 193 */
/* 3176 */	0xdc,		/* 220 */
			0xec,		/* 236 */
/* 3178 */	0xbc,		/* 188 */
			0x26,		/* 38 */
/* 3180 */	0x7f,		/* 127 */
			0x4,		/* 4 */
/* 3182 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3184 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3186) */
/* 3186 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3188 */	NdrFcLong( 0x925837d1 ),	/* -1839712303 */
/* 3192 */	NdrFcShort( 0x3aa1 ),	/* 15009 */
/* 3194 */	NdrFcShort( 0x451a ),	/* 17690 */
/* 3196 */	0xb7,		/* 183 */
			0xfe,		/* 254 */
/* 3198 */	0xcc,		/* 204 */
			0x4,		/* 4 */
/* 3200 */	0xbb,		/* 187 */
			0x42,		/* 66 */
/* 3202 */	0xcf,		/* 207 */
			0xb8,		/* 184 */
/* 3204 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3206 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3208) */
/* 3208 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3210 */	NdrFcLong( 0x1ab276dd ),	/* 447903453 */
/* 3214 */	NdrFcShort( 0xf27b ),	/* -3461 */
/* 3216 */	NdrFcShort( 0x4445 ),	/* 17477 */
/* 3218 */	0x82,		/* 130 */
			0x5d,		/* 93 */
/* 3220 */	0x5d,		/* 93 */
			0xf0,		/* 240 */
/* 3222 */	0xb4,		/* 180 */
			0xa0,		/* 160 */
/* 3224 */	0x4a,		/* 74 */
			0x3a,		/* 58 */
/* 3226 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3228 */	NdrFcLong( 0x10b793ac ),	/* 280466348 */
/* 3232 */	NdrFcShort( 0xc47 ),	/* 3143 */
/* 3234 */	NdrFcShort( 0x4679 ),	/* 18041 */
/* 3236 */	0x84,		/* 132 */
			0x54,		/* 84 */
/* 3238 */	0xad,		/* 173 */
			0xb3,		/* 179 */
/* 3240 */	0x6f,		/* 111 */
			0x29,		/* 41 */
/* 3242 */	0xf8,		/* 248 */
			0x2,		/* 2 */
/* 3244 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3246 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3248) */
/* 3248 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3250 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 3254 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 3256 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 3258 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 3260 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 3262 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 3264 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 3266 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3268 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3270) */
/* 3270 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3272 */	NdrFcLong( 0x931516ad ),	/* -1827334483 */
/* 3276 */	NdrFcShort( 0xb600 ),	/* -18944 */
/* 3278 */	NdrFcShort( 0x419c ),	/* 16796 */
/* 3280 */	0x88,		/* 136 */
			0xfc,		/* 252 */
/* 3282 */	0xdc,		/* 220 */
			0xf5,		/* 245 */
/* 3284 */	0x18,		/* 24 */
			0x3b,		/* 59 */
/* 3286 */	0x5f,		/* 95 */
			0xa9,		/* 169 */
/* 3288 */	
			0x11, 0x0,	/* FC_RP */
/* 3290 */	NdrFcShort( 0x26 ),	/* Offset= 38 (3328) */
/* 3292 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3294 */	NdrFcLong( 0x1412926f ),	/* 336761455 */
/* 3298 */	NdrFcShort( 0x5dd6 ),	/* 24022 */
/* 3300 */	NdrFcShort( 0x4e58 ),	/* 20056 */
/* 3302 */	0xb6,		/* 182 */
			0x48,		/* 72 */
/* 3304 */	0xe1,		/* 225 */
			0xc6,		/* 198 */
/* 3306 */	0x3e,		/* 62 */
			0x1,		/* 1 */
/* 3308 */	0x3d,		/* 61 */
			0x51,		/* 81 */
/* 3310 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3312 */	NdrFcLong( 0xfc1cd9a ),	/* 264359322 */
/* 3316 */	NdrFcShort( 0xb912 ),	/* -18158 */
/* 3318 */	NdrFcShort( 0x405c ),	/* 16476 */
/* 3320 */	0xa0,		/* 160 */
			0x4c,		/* 76 */
/* 3322 */	0x43,		/* 67 */
			0xce,		/* 206 */
/* 3324 */	0x2,		/* 2 */
			0xcd,		/* 205 */
/* 3326 */	0x7d,		/* 125 */
			0xf2,		/* 242 */
/* 3328 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 3330 */	NdrFcShort( 0x40 ),	/* 64 */
/* 3332 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3334 */	NdrFcShort( 0x26 ),	/* Offset= 38 (3372) */
/* 3336 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3338 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff313 ),	/* Offset= -3309 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3342 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff30f ),	/* Offset= -3313 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3346 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff30b ),	/* Offset= -3317 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3350 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff307 ),	/* Offset= -3321 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3354 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff303 ),	/* Offset= -3325 (30) */
			0xb,		/* FC_HYPER */
/* 3358 */	0xb,		/* FC_HYPER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3360 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffbb ),	/* Offset= -69 (3292) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3364 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc9 ),	/* Offset= -55 (3310) */
			0x8,		/* FC_LONG */
/* 3368 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 3370 */	0x40,		/* FC_STRUCTPAD4 */
			0x5b,		/* FC_END */
/* 3372 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 3374 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 3376 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3378 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3380) */
/* 3380 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3382 */	NdrFcLong( 0x37a44580 ),	/* 933512576 */
/* 3386 */	NdrFcShort( 0xd5fc ),	/* -10756 */
/* 3388 */	NdrFcShort( 0x473e ),	/* 18238 */
/* 3390 */	0xa0,		/* 160 */
			0x48,		/* 72 */
/* 3392 */	0x21,		/* 33 */
			0x70,		/* 112 */
/* 3394 */	0x2e,		/* 46 */
			0xbf,		/* 191 */
/* 3396 */	0xc4,		/* 196 */
			0x66,		/* 102 */
/* 3398 */	
			0x14, 0x0,	/* FC_FP */
/* 3400 */	NdrFcShort( 0xfffff2d6 ),	/* Offset= -3370 (30) */
/* 3402 */	
			0x14, 0x0,	/* FC_FP */
/* 3404 */	NdrFcShort( 0xfffff2fa ),	/* Offset= -3334 (70) */
/* 3406 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3408 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3410) */
/* 3410 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3412 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 3416 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 3418 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 3420 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 3422 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 3424 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 3426 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 3428 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3430 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3432) */
/* 3432 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3434 */	NdrFcLong( 0x6c7072c3 ),	/* 1819308739 */
/* 3438 */	NdrFcShort( 0x3ac4 ),	/* 15044 */
/* 3440 */	NdrFcShort( 0x408f ),	/* 16527 */
/* 3442 */	0xa6,		/* 166 */
			0x80,		/* 128 */
/* 3444 */	0xfc,		/* 252 */
			0x5a,		/* 90 */
/* 3446 */	0x2f,		/* 47 */
			0x96,		/* 150 */
/* 3448 */	0x90,		/* 144 */
			0x3e,		/* 62 */
/* 3450 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3452 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3454) */
/* 3454 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3456 */	NdrFcLong( 0xd5168050 ),	/* -719945648 */
/* 3460 */	NdrFcShort( 0xa57a ),	/* -23174 */
/* 3462 */	NdrFcShort( 0x465c ),	/* 18012 */
/* 3464 */	0xbe,		/* 190 */
			0xa9,		/* 169 */
/* 3466 */	0x97,		/* 151 */
			0x4f,		/* 79 */
/* 3468 */	0x40,		/* 64 */
			0x5e,		/* 94 */
/* 3470 */	0xba,		/* 186 */
			0x13,		/* 19 */
/* 3472 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 3474 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3476) */
/* 3476 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3478 */	NdrFcShort( 0x20 ),	/* 32 */
/* 3480 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3482 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3482) */
/* 3484 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3486 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff27f ),	/* Offset= -3457 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3490 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff27b ),	/* Offset= -3461 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3494 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff877 ),	/* Offset= -1929 (1566) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3498 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff273 ),	/* Offset= -3469 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3502 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff26f ),	/* Offset= -3473 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3506 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff26b ),	/* Offset= -3477 (30) */
			0x5b,		/* FC_END */
/* 3510 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3512 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3514) */
/* 3514 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3516 */	NdrFcLong( 0x1ab276dd ),	/* 447903453 */
/* 3520 */	NdrFcShort( 0xf27b ),	/* -3461 */
/* 3522 */	NdrFcShort( 0x4445 ),	/* 17477 */
/* 3524 */	0x82,		/* 130 */
			0x5d,		/* 93 */
/* 3526 */	0x5d,		/* 93 */
			0xf0,		/* 240 */
/* 3528 */	0xb4,		/* 180 */
			0xa0,		/* 160 */
/* 3530 */	0x4a,		/* 74 */
			0x3a,		/* 58 */
/* 3532 */	
			0x11, 0x0,	/* FC_RP */
/* 3534 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3536) */
/* 3536 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3540 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3542 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3544 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3546 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3548 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3550 */	NdrFcShort( 0xffffffdc ),	/* Offset= -36 (3514) */
/* 3552 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3554 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3556 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3558) */
/* 3558 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3560 */	NdrFcLong( 0x931516ad ),	/* -1827334483 */
/* 3564 */	NdrFcShort( 0xb600 ),	/* -18944 */
/* 3566 */	NdrFcShort( 0x419c ),	/* 16796 */
/* 3568 */	0x88,		/* 136 */
			0xfc,		/* 252 */
/* 3570 */	0xdc,		/* 220 */
			0xf5,		/* 245 */
/* 3572 */	0x18,		/* 24 */
			0x3b,		/* 59 */
/* 3574 */	0x5f,		/* 95 */
			0xa9,		/* 169 */
/* 3576 */	
			0x11, 0x0,	/* FC_RP */
/* 3578 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3580) */
/* 3580 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 3582 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3584 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3586 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3588 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 3590 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 3592 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 3594 */	
			0x14, 0x8,	/* FC_FP [simple_pointer] */
/* 3596 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 3598 */	
			0x11, 0x0,	/* FC_RP */
/* 3600 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3602) */
/* 3602 */	
			0x1c,		/* FC_CVARRAY */
			0x0,		/* 0 */
/* 3604 */	NdrFcShort( 0x1 ),	/* 1 */
/* 3606 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3608 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3610 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3612 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 3614 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 3616 */	
			0x11, 0x0,	/* FC_RP */
/* 3618 */	NdrFcShort( 0x36 ),	/* Offset= 54 (3672) */
/* 3620 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 3622 */	NdrFcShort( 0x48 ),	/* 72 */
/* 3624 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3626 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3626) */
/* 3628 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3630 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1ef ),	/* Offset= -3601 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3634 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1eb ),	/* Offset= -3605 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3638 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1e7 ),	/* Offset= -3609 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3642 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1e3 ),	/* Offset= -3613 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3646 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1df ),	/* Offset= -3617 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3650 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1db ),	/* Offset= -3621 (30) */
			0x40,		/* FC_STRUCTPAD4 */
/* 3654 */	0xb,		/* FC_HYPER */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3656 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff7d5 ),	/* Offset= -2091 (1566) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3660 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff7d1 ),	/* Offset= -2095 (1566) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 3664 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff1cd ),	/* Offset= -3635 (30) */
			0x8,		/* FC_LONG */
/* 3668 */	0x8,		/* FC_LONG */
			0x40,		/* FC_STRUCTPAD4 */
/* 3670 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3672 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 3674 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3676 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3678 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 3680 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 3682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3684 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3686 */	NdrFcShort( 0xffffffbe ),	/* Offset= -66 (3620) */
/* 3688 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3690 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3692 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 3696 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 3698 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 3700 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 3702 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 3704 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 3706 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 3708 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3710 */	NdrFcShort( 0xffffffec ),	/* Offset= -20 (3690) */
/* 3712 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3714 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3716) */
/* 3716 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3718 */	NdrFcLong( 0x1606dd73 ),	/* 369548659 */
/* 3722 */	NdrFcShort( 0x5d5f ),	/* 23903 */
/* 3724 */	NdrFcShort( 0x405c ),	/* 16476 */
/* 3726 */	0xb4,		/* 180 */
			0xf4,		/* 244 */
/* 3728 */	0xce,		/* 206 */
			0x32,		/* 50 */
/* 3730 */	0xba,		/* 186 */
			0xba,		/* 186 */
/* 3732 */	0x25,		/* 37 */
			0x1,		/* 1 */
/* 3734 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3736 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3738) */
/* 3738 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3740 */	NdrFcLong( 0xad47a80b ),	/* -1387812853 */
/* 3744 */	NdrFcShort( 0xeda7 ),	/* -4697 */
/* 3746 */	NdrFcShort( 0x459e ),	/* 17822 */
/* 3748 */	0xaf,		/* 175 */
			0x82,		/* 130 */
/* 3750 */	0x64,		/* 100 */
			0x7c,		/* 124 */
/* 3752 */	0xc9,		/* 201 */
			0xfb,		/* 251 */
/* 3754 */	0xaa,		/* 170 */
			0x50,		/* 80 */
/* 3756 */	0x14, 0x0,	/* FC_FP */
/* 3758 */	NdrFcShort( 0xfffff770 ),	/* Offset= -2192 (1566) */
/* 3760 */	
			0x11, 0x0,	/* FC_RP */
/* 3762 */	NdrFcShort( 0x14 ),	/* Offset= 20 (3782) */
/* 3764 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3766 */	NdrFcLong( 0x931516ad ),	/* -1827334483 */
/* 3770 */	NdrFcShort( 0xb600 ),	/* -18944 */
/* 3772 */	NdrFcShort( 0x419c ),	/* 16796 */
/* 3774 */	0x88,		/* 136 */
			0xfc,		/* 252 */
/* 3776 */	0xdc,		/* 220 */
			0xf5,		/* 245 */
/* 3778 */	0x18,		/* 24 */
			0x3b,		/* 59 */
/* 3780 */	0x5f,		/* 95 */
			0xa9,		/* 169 */
/* 3782 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 3784 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3786 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3788 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3790 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 3792 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 3794 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3796 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (3764) */
/* 3798 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3800 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3802 */	NdrFcShort( 0xffffffda ),	/* Offset= -38 (3764) */
/* 3804 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 3806 */	NdrFcShort( 0x2 ),	/* Offset= 2 (3808) */
/* 3808 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3810 */	NdrFcLong( 0xf7473fd0 ),	/* -146325552 */
/* 3814 */	NdrFcShort( 0x7f75 ),	/* 32629 */
/* 3816 */	NdrFcShort( 0x478d ),	/* 18317 */
/* 3818 */	0x8d,		/* 141 */
			0x85,		/* 133 */
/* 3820 */	0xa4,		/* 164 */
			0x85,		/* 133 */
/* 3822 */	0x20,		/* 32 */
			0x4e,		/* 78 */
/* 3824 */	0x7a,		/* 122 */
			0x2d,		/* 45 */
/* 3826 */	
			0x11, 0x0,	/* FC_RP */
/* 3828 */	NdrFcShort( 0x116 ),	/* Offset= 278 (4106) */
/* 3830 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 3832 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3834 */	NdrFcShort( 0x7 ),	/* 7 */
/* 3836 */	NdrFcLong( 0x10001 ),	/* 65537 */
/* 3840 */	NdrFcShort( 0x3a ),	/* Offset= 58 (3898) */
/* 3842 */	NdrFcLong( 0x20001 ),	/* 131073 */
/* 3846 */	NdrFcShort( 0x58 ),	/* Offset= 88 (3934) */
/* 3848 */	NdrFcLong( 0x30001 ),	/* 196609 */
/* 3852 */	NdrFcShort( 0x64 ),	/* Offset= 100 (3952) */
/* 3854 */	NdrFcLong( 0x40001 ),	/* 262145 */
/* 3858 */	NdrFcShort( 0x6c ),	/* Offset= 108 (3966) */
/* 3860 */	NdrFcLong( 0x50001 ),	/* 327681 */
/* 3864 */	NdrFcShort( 0x78 ),	/* Offset= 120 (3984) */
/* 3866 */	NdrFcLong( 0x40002 ),	/* 262146 */
/* 3870 */	NdrFcShort( 0x8c ),	/* Offset= 140 (4010) */
/* 3872 */	NdrFcLong( 0x60000 ),	/* 393216 */
/* 3876 */	NdrFcShort( 0xae ),	/* Offset= 174 (4050) */
/* 3878 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 3880 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3882 */	NdrFcLong( 0xbdde0eee ),	/* -1109520658 */
/* 3886 */	NdrFcShort( 0x3b8d ),	/* 15245 */
/* 3888 */	NdrFcShort( 0x4c82 ),	/* 19586 */
/* 3890 */	0xb5,		/* 181 */
			0x29,		/* 41 */
/* 3892 */	0x33,		/* 51 */
			0xf1,		/* 241 */
/* 3894 */	0x6b,		/* 107 */
			0x42,		/* 66 */
/* 3896 */	0x83,		/* 131 */
			0x2e,		/* 46 */
/* 3898 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3900 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3904 */	NdrFcShort( 0xc ),	/* Offset= 12 (3916) */
/* 3906 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3908 */	NdrFcShort( 0xfffff0da ),	/* Offset= -3878 (30) */
/* 3910 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3912 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (3880) */
/* 3914 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3916 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 3918 */	NdrFcLong( 0x1ede3b4b ),	/* 517880651 */
/* 3922 */	NdrFcShort( 0x35e7 ),	/* 13799 */
/* 3924 */	NdrFcShort( 0x4b97 ),	/* 19351 */
/* 3926 */	0x81,		/* 129 */
			0x33,		/* 51 */
/* 3928 */	0x2,		/* 2 */
			0x84,		/* 132 */
/* 3930 */	0x5d,		/* 93 */
			0x60,		/* 96 */
/* 3932 */	0x1,		/* 1 */
			0x74,		/* 116 */
/* 3934 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3936 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3938 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3940 */	NdrFcShort( 0xc ),	/* Offset= 12 (3952) */
/* 3942 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3944 */	NdrFcShort( 0xfffff0b6 ),	/* Offset= -3914 (30) */
/* 3946 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3948 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (3916) */
/* 3950 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3952 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3954 */	NdrFcShort( 0x4 ),	/* 4 */
/* 3956 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3958 */	NdrFcShort( 0x8 ),	/* Offset= 8 (3966) */
/* 3960 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3962 */	NdrFcShort( 0xfffffef0 ),	/* Offset= -272 (3690) */
/* 3964 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3966 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3968 */	NdrFcShort( 0x8 ),	/* 8 */
/* 3970 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3972 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3972) */
/* 3974 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3976 */	NdrFcShort( 0xfffff096 ),	/* Offset= -3946 (30) */
/* 3978 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3980 */	NdrFcShort( 0xfffff092 ),	/* Offset= -3950 (30) */
/* 3982 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 3984 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 3986 */	NdrFcShort( 0x10 ),	/* 16 */
/* 3988 */	NdrFcShort( 0x0 ),	/* 0 */
/* 3990 */	NdrFcShort( 0x0 ),	/* Offset= 0 (3990) */
/* 3992 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3994 */	NdrFcShort( 0xfffff084 ),	/* Offset= -3964 (30) */
/* 3996 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 3998 */	NdrFcShort( 0xfffff080 ),	/* Offset= -3968 (30) */
/* 4000 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4002 */	NdrFcShort( 0xfffff07c ),	/* Offset= -3972 (30) */
/* 4004 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4006 */	NdrFcShort( 0xfffff078 ),	/* Offset= -3976 (30) */
/* 4008 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4010 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4012 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4014 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4016 */	NdrFcShort( 0x10 ),	/* Offset= 16 (4032) */
/* 4018 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4020 */	NdrFcShort( 0xfffffdca ),	/* Offset= -566 (3454) */
/* 4022 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4024 */	NdrFcShort( 0xfffff066 ),	/* Offset= -3994 (30) */
/* 4026 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4028 */	NdrFcShort( 0xfffff062 ),	/* Offset= -3998 (30) */
/* 4030 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 4032 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4034 */	NdrFcLong( 0xb7e66f28 ),	/* -1209635032 */
/* 4038 */	NdrFcShort( 0x35a ),	/* 858 */
/* 4040 */	NdrFcShort( 0x401a ),	/* 16410 */
/* 4042 */	0xaf,		/* 175 */
			0xc7,		/* 199 */
/* 4044 */	0x2e,		/* 46 */
			0x30,		/* 48 */
/* 4046 */	0xb,		/* 11 */
			0xd2,		/* 210 */
/* 4048 */	0x97,		/* 151 */
			0x11,		/* 17 */
/* 4050 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4052 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4054 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4056 */	NdrFcShort( 0x8 ),	/* Offset= 8 (4064) */
/* 4058 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4060 */	NdrFcShort( 0xffffffe4 ),	/* Offset= -28 (4032) */
/* 4062 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4064 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4066 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 4070 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 4072 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 4074 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 4076 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 4078 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 4080 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 4082 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4084 */	NdrFcShort( 0x14 ),	/* 20 */
/* 4086 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4088 */	NdrFcShort( 0x12 ),	/* Offset= 18 (4106) */
/* 4090 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4092 */	NdrFcShort( 0xfffffd82 ),	/* Offset= -638 (3454) */
/* 4094 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4096 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff01d ),	/* Offset= -4067 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4100 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff019 ),	/* Offset= -4071 (30) */
			0x8,		/* FC_LONG */
/* 4104 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4106 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4108 */	NdrFcShort( 0x58 ),	/* 88 */
/* 4110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4112 */	NdrFcShort( 0x26 ),	/* Offset= 38 (4150) */
/* 4114 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4116 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff031 ),	/* Offset= -4047 (70) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4120 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffedd ),	/* Offset= -291 (3830) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4124 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc3 ),	/* Offset= -61 (4064) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4128 */	0x0,		/* 0 */
			NdrFcShort( 0xffffeffd ),	/* Offset= -4099 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4132 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffd59 ),	/* Offset= -679 (3454) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4136 */	0x0,		/* 0 */
			NdrFcShort( 0xffffeff5 ),	/* Offset= -4107 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4140 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffc5 ),	/* Offset= -59 (4082) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4144 */	0x0,		/* 0 */
			NdrFcShort( 0xfffff5ed ),	/* Offset= -2579 (1566) */
			0x8,		/* FC_LONG */
/* 4148 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4150 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 4152 */	NdrFcShort( 0x3a ),	/* Offset= 58 (4210) */
/* 4154 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 4156 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4158 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4160 */	NdrFcLong( 0x1 ),	/* 1 */
/* 4164 */	NdrFcShort( 0xa ),	/* Offset= 10 (4174) */
/* 4166 */	NdrFcLong( 0x2 ),	/* 2 */
/* 4170 */	NdrFcShort( 0x12 ),	/* Offset= 18 (4188) */
/* 4172 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 4174 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4176 */	NdrFcShort( 0x4 ),	/* 4 */
/* 4178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4180 */	NdrFcShort( 0x8 ),	/* Offset= 8 (4188) */
/* 4182 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4184 */	NdrFcShort( 0xfffffe12 ),	/* Offset= -494 (3690) */
/* 4186 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4188 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4190 */	NdrFcShort( 0x10 ),	/* 16 */
/* 4192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4194 */	NdrFcShort( 0x0 ),	/* Offset= 0 (4194) */
/* 4196 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4198 */	NdrFcShort( 0xffffefb8 ),	/* Offset= -4168 (30) */
/* 4200 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4202 */	NdrFcShort( 0xffffefb4 ),	/* Offset= -4172 (30) */
/* 4204 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4206 */	NdrFcShort( 0xffffefb0 ),	/* Offset= -4176 (30) */
/* 4208 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 4210 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4212 */	NdrFcShort( 0x20 ),	/* 32 */
/* 4214 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4216 */	NdrFcShort( 0x10 ),	/* Offset= 16 (4232) */
/* 4218 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4220 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffbd ),	/* Offset= -67 (4154) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4224 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff5f ),	/* Offset= -161 (4064) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4228 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffcf9 ),	/* Offset= -775 (3454) */
			0x5b,		/* FC_END */
/* 4232 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 4234 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4236) */
/* 4236 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 4238 */	NdrFcShort( 0x28 ),	/* 40 */
/* 4240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4242 */	NdrFcShort( 0x16 ),	/* Offset= 22 (4264) */
/* 4244 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4246 */	0x0,		/* 0 */
			NdrFcShort( 0xffffffa3 ),	/* Offset= -93 (4154) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4250 */	0x0,		/* 0 */
			NdrFcShort( 0xffffff45 ),	/* Offset= -187 (4064) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4254 */	0x0,		/* 0 */
			NdrFcShort( 0xfffffcdf ),	/* Offset= -801 (3454) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4258 */	0x0,		/* 0 */
			NdrFcShort( 0xffffef7b ),	/* Offset= -4229 (30) */
			0x8,		/* FC_LONG */
/* 4262 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4264 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4266 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4268) */
/* 4268 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4270 */	NdrFcLong( 0x6e215ef3 ),	/* 1847680755 */
/* 4274 */	NdrFcShort( 0xe44c ),	/* -7092 */
/* 4276 */	NdrFcShort( 0x44d1 ),	/* 17617 */
/* 4278 */	0xb7,		/* 183 */
			0xba,		/* 186 */
/* 4280 */	0xb2,		/* 178 */
			0x40,		/* 64 */
/* 4282 */	0x1f,		/* 31 */
			0x7d,		/* 125 */
/* 4284 */	0xc2,		/* 194 */
			0x3d,		/* 61 */
/* 4286 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4288 */	NdrFcShort( 0xffffff00 ),	/* Offset= -256 (4032) */
/* 4290 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4292 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4294) */
/* 4294 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4296 */	NdrFcLong( 0xe158f5aa ),	/* -514263638 */
/* 4300 */	NdrFcShort( 0x31fe ),	/* 12798 */
/* 4302 */	NdrFcShort( 0x491b ),	/* 18715 */
/* 4304 */	0xa9,		/* 169 */
			0xf6,		/* 246 */
/* 4306 */	0xcf,		/* 207 */
			0xf9,		/* 249 */
/* 4308 */	0x34,		/* 52 */
			0xb0,		/* 176 */
/* 4310 */	0x3a,		/* 58 */
			0x1,		/* 1 */
/* 4312 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 4314 */	NdrFcShort( 0xfffff544 ),	/* Offset= -2748 (1566) */
/* 4316 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4318 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4320) */
/* 4320 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4322 */	NdrFcLong( 0x6015fd18 ),	/* 1612053784 */
/* 4326 */	NdrFcShort( 0x8257 ),	/* -32169 */
/* 4328 */	NdrFcShort( 0x4df3 ),	/* 19955 */
/* 4330 */	0xac,		/* 172 */
			0x42,		/* 66 */
/* 4332 */	0xf0,		/* 240 */
			0x74,		/* 116 */
/* 4334 */	0xde,		/* 222 */
			0xdd,		/* 221 */
/* 4336 */	0x4c,		/* 76 */
			0xbd,		/* 189 */
/* 4338 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4340 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4342) */
/* 4342 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4344 */	NdrFcLong( 0xf6b37e0 ),	/* 258684896 */
/* 4348 */	NdrFcShort( 0xfcfe ),	/* -770 */
/* 4350 */	NdrFcShort( 0x44d9 ),	/* 17625 */
/* 4352 */	0x91,		/* 145 */
			0x12,		/* 18 */
/* 4354 */	0x39,		/* 57 */
			0x4c,		/* 76 */
/* 4356 */	0xa9,		/* 169 */
			0xb9,		/* 185 */
/* 4358 */	0x21,		/* 33 */
			0x14,		/* 20 */
/* 4360 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4362 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4364) */
/* 4364 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4366 */	NdrFcLong( 0xe158f5aa ),	/* -514263638 */
/* 4370 */	NdrFcShort( 0x31fe ),	/* 12798 */
/* 4372 */	NdrFcShort( 0x491b ),	/* 18715 */
/* 4374 */	0xa9,		/* 169 */
			0xf6,		/* 246 */
/* 4376 */	0xcf,		/* 207 */
			0xf9,		/* 249 */
/* 4378 */	0x34,		/* 52 */
			0xb0,		/* 176 */
/* 4380 */	0x3a,		/* 58 */
			0x1,		/* 1 */
/* 4382 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4384 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4386) */
/* 4386 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4388 */	NdrFcLong( 0x6e215ef3 ),	/* 1847680755 */
/* 4392 */	NdrFcShort( 0xe44c ),	/* -7092 */
/* 4394 */	NdrFcShort( 0x44d1 ),	/* 17617 */
/* 4396 */	0xb7,		/* 183 */
			0xba,		/* 186 */
/* 4398 */	0xb2,		/* 178 */
			0x40,		/* 64 */
/* 4400 */	0x1f,		/* 31 */
			0x7d,		/* 125 */
/* 4402 */	0xc2,		/* 194 */
			0x3d,		/* 61 */
/* 4404 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4406 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4408) */
/* 4408 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4410 */	NdrFcLong( 0x603aedf8 ),	/* 1614474744 */
/* 4414 */	NdrFcShort( 0x9575 ),	/* -27275 */
/* 4416 */	NdrFcShort( 0x4d30 ),	/* 19760 */
/* 4418 */	0xb8,		/* 184 */
			0xca,		/* 202 */
/* 4420 */	0x12,		/* 18 */
			0x4d,		/* 77 */
/* 4422 */	0x1c,		/* 28 */
			0x98,		/* 152 */
/* 4424 */	0xeb,		/* 235 */
			0xd8,		/* 216 */
/* 4426 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4428 */	NdrFcLong( 0xade2eeb9 ),	/* -1377636679 */
/* 4432 */	NdrFcShort( 0xfc85 ),	/* -891 */
/* 4434 */	NdrFcShort( 0x4f5b ),	/* 20315 */
/* 4436 */	0xb5,		/* 181 */
			0xd9,		/* 217 */
/* 4438 */	0xd4,		/* 212 */
			0x31,		/* 49 */
/* 4440 */	0xb4,		/* 180 */
			0xaa,		/* 170 */
/* 4442 */	0xc3,		/* 195 */
			0x1a,		/* 26 */
/* 4444 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4446 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4448) */
/* 4448 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4450 */	NdrFcLong( 0xa7ee3e7e ),	/* -1477558658 */
/* 4454 */	NdrFcShort( 0x2dd2 ),	/* 11730 */
/* 4456 */	NdrFcShort( 0x4ad7 ),	/* 19159 */
/* 4458 */	0x96,		/* 150 */
			0x97,		/* 151 */
/* 4460 */	0xf4,		/* 244 */
			0xaa,		/* 170 */
/* 4462 */	0xe3,		/* 227 */
			0x42,		/* 66 */
/* 4464 */	0x77,		/* 119 */
			0x62,		/* 98 */
/* 4466 */	
			0x11, 0x0,	/* FC_RP */
/* 4468 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4470) */
/* 4470 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 4472 */	NdrFcShort( 0x40 ),	/* 64 */
/* 4474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4476 */	NdrFcShort( 0x0 ),	/* Offset= 0 (4476) */
/* 4478 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4480 */	0x0,		/* 0 */
			NdrFcShort( 0xffffee9d ),	/* Offset= -4451 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4484 */	0x0,		/* 0 */
			NdrFcShort( 0xffffee99 ),	/* Offset= -4455 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4488 */	0x0,		/* 0 */
			NdrFcShort( 0xffffee95 ),	/* Offset= -4459 (30) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 4492 */	0x0,		/* 0 */
			NdrFcShort( 0xffffee91 ),	/* Offset= -4463 (30) */
			0x40,		/* FC_STRUCTPAD4 */
/* 4496 */	0xb,		/* FC_HYPER */
			0xb,		/* FC_HYPER */
/* 4498 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 4500 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4502 */	NdrFcShort( 0xfffff488 ),	/* Offset= -2936 (1566) */
/* 4504 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4506 */	NdrFcShort( 0xffffee84 ),	/* Offset= -4476 (30) */
/* 4508 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 4510 */	
			0x11, 0x0,	/* FC_RP */
/* 4512 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4514) */
/* 4514 */	
			0x1c,		/* FC_CVARRAY */
			0x1,		/* 1 */
/* 4516 */	NdrFcShort( 0x2 ),	/* 2 */
/* 4518 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4520 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4522 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4524 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 4526 */	0x5,		/* FC_WCHAR */
			0x5b,		/* FC_END */
/* 4528 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4530 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4532) */
/* 4532 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4534 */	NdrFcLong( 0x1606dd73 ),	/* 369548659 */
/* 4538 */	NdrFcShort( 0x5d5f ),	/* 23903 */
/* 4540 */	NdrFcShort( 0x405c ),	/* 16476 */
/* 4542 */	0xb4,		/* 180 */
			0xf4,		/* 244 */
/* 4544 */	0xce,		/* 206 */
			0x32,		/* 50 */
/* 4546 */	0xba,		/* 186 */
			0xba,		/* 186 */
/* 4548 */	0x25,		/* 37 */
			0x1,		/* 1 */
/* 4550 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4552 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4554) */
/* 4554 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 4560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4564 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 4566 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 4568 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 4570 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 4572 */	
			0x11, 0x0,	/* FC_RP */
/* 4574 */	NdrFcShort( 0x14 ),	/* Offset= 20 (4594) */
/* 4576 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4578 */	NdrFcLong( 0x43286fea ),	/* 1126723562 */
/* 4582 */	NdrFcShort( 0x6997 ),	/* 27031 */
/* 4584 */	NdrFcShort( 0x4543 ),	/* 17731 */
/* 4586 */	0x80,		/* 128 */
			0x3e,		/* 62 */
/* 4588 */	0x60,		/* 96 */
			0xa2,		/* 162 */
/* 4590 */	0xc,		/* 12 */
			0x47,		/* 71 */
/* 4592 */	0x3d,		/* 61 */
			0xe5,		/* 229 */
/* 4594 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4596 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4598 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4600 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4602 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4604 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4606 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4608 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4576) */
/* 4610 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4612 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4614 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4616) */
/* 4616 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4618 */	NdrFcLong( 0x96c74ef4 ),	/* -1765323020 */
/* 4622 */	NdrFcShort( 0x185d ),	/* 6237 */
/* 4624 */	NdrFcShort( 0x4f9a ),	/* 20378 */
/* 4626 */	0x8a,		/* 138 */
			0x43,		/* 67 */
/* 4628 */	0x4d,		/* 77 */
			0x27,		/* 39 */
/* 4630 */	0x23,		/* 35 */
			0x75,		/* 117 */
/* 4632 */	0x8e,		/* 142 */
			0xa,		/* 10 */
/* 4634 */	
			0x11, 0x0,	/* FC_RP */
/* 4636 */	NdrFcShort( 0x14 ),	/* Offset= 20 (4656) */
/* 4638 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4640 */	NdrFcLong( 0x69d172ef ),	/* 1775334127 */
/* 4644 */	NdrFcShort( 0xf2c4 ),	/* -3388 */
/* 4646 */	NdrFcShort( 0x44e1 ),	/* 17633 */
/* 4648 */	0x89,		/* 137 */
			0xf7,		/* 247 */
/* 4650 */	0xc8,		/* 200 */
			0x62,		/* 98 */
/* 4652 */	0x31,		/* 49 */
			0xe7,		/* 231 */
/* 4654 */	0x6,		/* 6 */
			0xe9,		/* 233 */
/* 4656 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4658 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4660 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4662 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4664 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4666 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4668 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4670 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4638) */
/* 4672 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4674 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4676 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4678) */
/* 4678 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4680 */	NdrFcLong( 0x8d14bca6 ),	/* -1928020826 */
/* 4684 */	NdrFcShort( 0x34ce ),	/* 13518 */
/* 4686 */	NdrFcShort( 0x4efe ),	/* 20222 */
/* 4688 */	0xac,		/* 172 */
			0x7e,		/* 126 */
/* 4690 */	0xa,		/* 10 */
			0xbc,		/* 188 */
/* 4692 */	0x61,		/* 97 */
			0xda,		/* 218 */
/* 4694 */	0xdb,		/* 219 */
			0x20,		/* 32 */
/* 4696 */	
			0x11, 0x0,	/* FC_RP */
/* 4698 */	NdrFcShort( 0x14 ),	/* Offset= 20 (4718) */
/* 4700 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4702 */	NdrFcLong( 0xd5168050 ),	/* -719945648 */
/* 4706 */	NdrFcShort( 0xa57a ),	/* -23174 */
/* 4708 */	NdrFcShort( 0x465c ),	/* 18012 */
/* 4710 */	0xbe,		/* 190 */
			0xa9,		/* 169 */
/* 4712 */	0x97,		/* 151 */
			0x4f,		/* 79 */
/* 4714 */	0x40,		/* 64 */
			0x5e,		/* 94 */
/* 4716 */	0xba,		/* 186 */
			0x13,		/* 19 */
/* 4718 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4722 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4724 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4726 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4728 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4730 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4732 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4700) */
/* 4734 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4736 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4738 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4740) */
/* 4740 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4742 */	NdrFcLong( 0xd30dc12 ),	/* 221305874 */
/* 4746 */	NdrFcShort( 0xc4f8 ),	/* -15112 */
/* 4748 */	NdrFcShort( 0x433d ),	/* 17213 */
/* 4750 */	0x9f,		/* 159 */
			0xcc,		/* 204 */
/* 4752 */	0x9f,		/* 159 */
			0xf1,		/* 241 */
/* 4754 */	0x17,		/* 23 */
			0xe5,		/* 229 */
/* 4756 */	0xe5,		/* 229 */
			0xf4,		/* 244 */
/* 4758 */	
			0x11, 0x0,	/* FC_RP */
/* 4760 */	NdrFcShort( 0x14 ),	/* Offset= 20 (4780) */
/* 4762 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4764 */	NdrFcLong( 0x1412926f ),	/* 336761455 */
/* 4768 */	NdrFcShort( 0x5dd6 ),	/* 24022 */
/* 4770 */	NdrFcShort( 0x4e58 ),	/* 20056 */
/* 4772 */	0xb6,		/* 182 */
			0x48,		/* 72 */
/* 4774 */	0xe1,		/* 225 */
			0xc6,		/* 198 */
/* 4776 */	0x3e,		/* 62 */
			0x1,		/* 1 */
/* 4778 */	0x3d,		/* 61 */
			0x51,		/* 81 */
/* 4780 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4782 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4784 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4786 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4788 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4790 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4792 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4794 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4762) */
/* 4796 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4798 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4800 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4802) */
/* 4802 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4804 */	NdrFcLong( 0xcd39102b ),	/* -851898325 */
/* 4808 */	NdrFcShort( 0x4b69 ),	/* 19305 */
/* 4810 */	NdrFcShort( 0x4495 ),	/* 17557 */
/* 4812 */	0x8f,		/* 143 */
			0x29,		/* 41 */
/* 4814 */	0xe0,		/* 224 */
			0xb2,		/* 178 */
/* 4816 */	0x5c,		/* 92 */
			0x4a,		/* 74 */
/* 4818 */	0x88,		/* 136 */
			0x55,		/* 85 */
/* 4820 */	
			0x11, 0x0,	/* FC_RP */
/* 4822 */	NdrFcShort( 0x14 ),	/* Offset= 20 (4842) */
/* 4824 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4826 */	NdrFcLong( 0xac17b76b ),	/* -1407731861 */
/* 4830 */	NdrFcShort( 0x2b09 ),	/* 11017 */
/* 4832 */	NdrFcShort( 0x419a ),	/* 16794 */
/* 4834 */	0xad,		/* 173 */
			0x5f,		/* 95 */
/* 4836 */	0x7d,		/* 125 */
			0x74,		/* 116 */
/* 4838 */	0x2,		/* 2 */
			0xda,		/* 218 */
/* 4840 */	0x88,		/* 136 */
			0x75,		/* 117 */
/* 4842 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4844 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4846 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4848 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4850 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4852 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4854 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4856 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4824) */
/* 4858 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4860 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4862 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4864) */
/* 4864 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4866 */	NdrFcLong( 0xad47a80b ),	/* -1387812853 */
/* 4870 */	NdrFcShort( 0xeda7 ),	/* -4697 */
/* 4872 */	NdrFcShort( 0x459e ),	/* 17822 */
/* 4874 */	0xaf,		/* 175 */
			0x82,		/* 130 */
/* 4876 */	0x64,		/* 100 */
			0x7c,		/* 124 */
/* 4878 */	0xc9,		/* 201 */
			0xfb,		/* 251 */
/* 4880 */	0xaa,		/* 170 */
			0x50,		/* 80 */
/* 4882 */	0x11, 0x0,	/* FC_RP */
/* 4884 */	NdrFcShort( 0x14 ),	/* Offset= 20 (4904) */
/* 4886 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4888 */	NdrFcLong( 0xd533d975 ),	/* -718022283 */
/* 4892 */	NdrFcShort( 0x3f32 ),	/* 16178 */
/* 4894 */	NdrFcShort( 0x4876 ),	/* 18550 */
/* 4896 */	0xab,		/* 171 */
			0xd0,		/* 208 */
/* 4898 */	0x6d,		/* 109 */
			0x37,		/* 55 */
/* 4900 */	0xfd,		/* 253 */
			0xa5,		/* 165 */
/* 4902 */	0x63,		/* 99 */
			0xe7,		/* 231 */
/* 4904 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4906 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4908 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4910 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4912 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4914 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4916 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4918 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4886) */
/* 4920 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4922 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4924 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4926) */
/* 4926 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4928 */	NdrFcLong( 0xf6b37e0 ),	/* 258684896 */
/* 4932 */	NdrFcShort( 0xfcfe ),	/* -770 */
/* 4934 */	NdrFcShort( 0x44d9 ),	/* 17625 */
/* 4936 */	0x91,		/* 145 */
			0x12,		/* 18 */
/* 4938 */	0x39,		/* 57 */
			0x4c,		/* 76 */
/* 4940 */	0xa9,		/* 169 */
			0xb9,		/* 185 */
/* 4942 */	0x21,		/* 33 */
			0x14,		/* 20 */
/* 4944 */	
			0x11, 0x0,	/* FC_RP */
/* 4946 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4948) */
/* 4948 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 4950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 4952 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 4954 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 4956 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 4958 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 4960 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 4962 */	NdrFcShort( 0xfffffdc0 ),	/* Offset= -576 (4386) */
/* 4964 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 4966 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 4968 */	NdrFcShort( 0x2 ),	/* Offset= 2 (4970) */
/* 4970 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4972 */	NdrFcLong( 0x70d2dc1e ),	/* 1892867102 */
/* 4976 */	NdrFcShort( 0x4dcc ),	/* 19916 */
/* 4978 */	NdrFcShort( 0x4786 ),	/* 18310 */
/* 4980 */	0xa0,		/* 160 */
			0x72,		/* 114 */
/* 4982 */	0x9a,		/* 154 */
			0x3b,		/* 59 */
/* 4984 */	0x60,		/* 96 */
			0xc,		/* 12 */
/* 4986 */	0x21,		/* 33 */
			0x6b,		/* 107 */
/* 4988 */	
			0x11, 0x0,	/* FC_RP */
/* 4990 */	NdrFcShort( 0x14 ),	/* Offset= 20 (5010) */
/* 4992 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 4994 */	NdrFcLong( 0x74570ef7 ),	/* 1951862519 */
/* 4998 */	NdrFcShort( 0x2486 ),	/* 9350 */
/* 5000 */	NdrFcShort( 0x4089 ),	/* 16521 */
/* 5002 */	0x80,		/* 128 */
			0xc,		/* 12 */
/* 5004 */	0x56,		/* 86 */
			0xe3,		/* 227 */
/* 5006 */	0x82,		/* 130 */
			0x9b,		/* 155 */
/* 5008 */	0x5c,		/* 92 */
			0xa4,		/* 164 */
/* 5010 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5012 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5014 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5016 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5018 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5020 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5022 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5024 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (4992) */
/* 5026 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5028 */	
			0x11, 0x0,	/* FC_RP */
/* 5030 */	NdrFcShort( 0x14 ),	/* Offset= 20 (5050) */
/* 5032 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5034 */	NdrFcLong( 0x17bf8fa3 ),	/* 398430115 */
/* 5038 */	NdrFcShort( 0x4c5a ),	/* 19546 */
/* 5040 */	NdrFcShort( 0x49a3 ),	/* 18851 */
/* 5042 */	0xb2,		/* 178 */
			0xf8,		/* 248 */
/* 5044 */	0x59,		/* 89 */
			0x42,		/* 66 */
/* 5046 */	0xe1,		/* 225 */
			0xea,		/* 234 */
/* 5048 */	0x28,		/* 40 */
			0x7e,		/* 126 */
/* 5050 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5052 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5054 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5056 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5058 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5060 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5062 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5064 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (5032) */
/* 5066 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5068 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5070 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5072) */
/* 5072 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5074 */	NdrFcLong( 0x61d986ec ),	/* 1641645804 */
/* 5078 */	NdrFcShort( 0x1eac ),	/* 7852 */
/* 5080 */	NdrFcShort( 0x46b6 ),	/* 18102 */
/* 5082 */	0x90,		/* 144 */
			0xff,		/* 255 */
/* 5084 */	0x40,		/* 64 */
			0x2a,		/* 42 */
/* 5086 */	0x0,		/* 0 */
			0x8f,		/* 143 */
/* 5088 */	0x15,		/* 21 */
			0xd1,		/* 209 */
/* 5090 */	
			0x11, 0x0,	/* FC_RP */
/* 5092 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5094) */
/* 5094 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5096 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5098 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5100 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5102 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5104 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5106 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5108 */	NdrFcShort( 0xffffee78 ),	/* Offset= -4488 (620) */
/* 5110 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5112 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5114 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5116) */
/* 5116 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5118 */	NdrFcLong( 0x8e4bbd34 ),	/* -1907638988 */
/* 5122 */	NdrFcShort( 0xa2f4 ),	/* -23820 */
/* 5124 */	NdrFcShort( 0x41ef ),	/* 16879 */
/* 5126 */	0x87,		/* 135 */
			0xb5,		/* 181 */
/* 5128 */	0xc5,		/* 197 */
			0x63,		/* 99 */
/* 5130 */	0xb4,		/* 180 */
			0xad,		/* 173 */
/* 5132 */	0x6e,		/* 110 */
			0xe7,		/* 231 */
/* 5134 */	
			0x11, 0x0,	/* FC_RP */
/* 5136 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5138) */
/* 5138 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 5140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5142 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5144 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5146 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5148 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5150 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5152 */	NdrFcShort( 0xfffff8e0 ),	/* Offset= -1824 (3328) */
/* 5154 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5156 */	
			0x11, 0x0,	/* FC_RP */
/* 5158 */	NdrFcShort( 0x14 ),	/* Offset= 20 (5178) */
/* 5160 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 5162 */	NdrFcShort( 0x8 ),	/* 8 */
/* 5164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5166 */	NdrFcShort( 0xc ),	/* Offset= 12 (5178) */
/* 5168 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5170 */	NdrFcShort( 0xffffebec ),	/* Offset= -5140 (30) */
/* 5172 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5174 */	NdrFcShort( 0xfffff3a6 ),	/* Offset= -3162 (2012) */
/* 5176 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5178 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5180 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5182 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5184 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5186 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5188 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5190 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5192 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (5160) */
/* 5194 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5196 */	
			0x11, 0x0,	/* FC_RP */
/* 5198 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5200) */
/* 5200 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5202 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5204 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5206 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5208 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5210 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5212 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5214 */	NdrFcShort( 0xfffff3ae ),	/* Offset= -3154 (2060) */
/* 5216 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5218 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5220 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5222) */
/* 5222 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5224 */	NdrFcLong( 0x4c4a2835 ),	/* 1279928373 */
/* 5228 */	NdrFcShort( 0x682e ),	/* 26670 */
/* 5230 */	NdrFcShort( 0x4ce1 ),	/* 19681 */
/* 5232 */	0xae,		/* 174 */
			0xbc,		/* 188 */
/* 5234 */	0x1e,		/* 30 */
			0x6b,		/* 107 */
/* 5236 */	0x3a,		/* 58 */
			0x16,		/* 22 */
/* 5238 */	0x5b,		/* 91 */
			0x44,		/* 68 */
/* 5240 */	
			0x11, 0x0,	/* FC_RP */
/* 5242 */	NdrFcShort( 0x14 ),	/* Offset= 20 (5262) */
/* 5244 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5246 */	NdrFcLong( 0x53f68191 ),	/* 1408663953 */
/* 5250 */	NdrFcShort( 0x7b2f ),	/* 31535 */
/* 5252 */	NdrFcShort( 0x4f14 ),	/* 20244 */
/* 5254 */	0x8e,		/* 142 */
			0x55,		/* 85 */
/* 5256 */	0x40,		/* 64 */
			0xb1,		/* 177 */
/* 5258 */	0xb6,		/* 182 */
			0xe5,		/* 229 */
/* 5260 */	0xdf,		/* 223 */
			0x66,		/* 102 */
/* 5262 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5264 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5266 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5268 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5270 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5272 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5274 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5276 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (5244) */
/* 5278 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5280 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5282 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5284) */
/* 5284 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5286 */	NdrFcLong( 0x59c9dc99 ),	/* 1506401433 */
/* 5290 */	NdrFcShort( 0x3eff ),	/* 16127 */
/* 5292 */	NdrFcShort( 0x4ff3 ),	/* 20467 */
/* 5294 */	0xb2,		/* 178 */
			0x1,		/* 1 */
/* 5296 */	0x98,		/* 152 */
			0xac,		/* 172 */
/* 5298 */	0xd0,		/* 208 */
			0x1b,		/* 27 */
/* 5300 */	0xd,		/* 13 */
			0x87,		/* 135 */
/* 5302 */	
			0x11, 0x0,	/* FC_RP */
/* 5304 */	NdrFcShort( 0x14 ),	/* Offset= 20 (5324) */
/* 5306 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5308 */	NdrFcLong( 0x79293cc8 ),	/* 2032745672 */
/* 5312 */	NdrFcShort( 0xd9d9 ),	/* -9767 */
/* 5314 */	NdrFcShort( 0x43f5 ),	/* 17397 */
/* 5316 */	0x97,		/* 151 */
			0xad,		/* 173 */
/* 5318 */	0xb,		/* 11 */
			0xcc,		/* 204 */
/* 5320 */	0x5a,		/* 90 */
			0x68,		/* 104 */
/* 5322 */	0x87,		/* 135 */
			0x76,		/* 118 */
/* 5324 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 5326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5328 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5330 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5332 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5334 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5336 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5338 */	NdrFcShort( 0xffffffe0 ),	/* Offset= -32 (5306) */
/* 5340 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5342 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5344 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5346) */
/* 5346 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5348 */	NdrFcLong( 0xbc827c5e ),	/* -1132299170 */
/* 5352 */	NdrFcShort( 0x99ae ),	/* -26194 */
/* 5354 */	NdrFcShort( 0x4ac8 ),	/* 19144 */
/* 5356 */	0x83,		/* 131 */
			0xad,		/* 173 */
/* 5358 */	0x2e,		/* 46 */
			0xa5,		/* 165 */
/* 5360 */	0xc2,		/* 194 */
			0x3,		/* 3 */
/* 5362 */	0x43,		/* 67 */
			0x33,		/* 51 */
/* 5364 */	
			0x11, 0x0,	/* FC_RP */
/* 5366 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5368) */
/* 5368 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 5370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5372 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5376 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5378 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5380 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5382 */	NdrFcShort( 0xfffff678 ),	/* Offset= -2440 (2942) */
/* 5384 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5386 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5388 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5390) */
/* 5390 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5392 */	NdrFcLong( 0x6c7072c3 ),	/* 1819308739 */
/* 5396 */	NdrFcShort( 0x3ac4 ),	/* 15044 */
/* 5398 */	NdrFcShort( 0x408f ),	/* 16527 */
/* 5400 */	0xa6,		/* 166 */
			0x80,		/* 128 */
/* 5402 */	0xfc,		/* 252 */
			0x5a,		/* 90 */
/* 5404 */	0x2f,		/* 47 */
			0x96,		/* 150 */
/* 5406 */	0x90,		/* 144 */
			0x3e,		/* 62 */
/* 5408 */	
			0x11, 0x0,	/* FC_RP */
/* 5410 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5412) */
/* 5412 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x7,		/* 7 */
/* 5414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 5416 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 5418 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 5420 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x54,		/* FC_DEREFERENCE */
/* 5422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 5424 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 5426 */	NdrFcShort( 0xfffff6f4 ),	/* Offset= -2316 (3110) */
/* 5428 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 5430 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 5432 */	NdrFcShort( 0x2 ),	/* Offset= 2 (5434) */
/* 5434 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 5436 */	NdrFcLong( 0xe459dd12 ),	/* -463872750 */
/* 5440 */	NdrFcShort( 0x864f ),	/* -31153 */
/* 5442 */	NdrFcShort( 0x4aaa ),	/* 19114 */
/* 5444 */	0xab,		/* 171 */
			0xc1,		/* 193 */
/* 5446 */	0xdc,		/* 220 */
			0xec,		/* 236 */
/* 5448 */	0xbc,		/* 188 */
			0x26,		/* 38 */
/* 5450 */	0x7f,		/* 127 */
			0x4,		/* 4 */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            },
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            }

        };



/* Standard interface: __MIDL_itf_msdbg_0000, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDebugMachine2, ver. 0.0,
   GUID={0x17bf8fa3,0x4c5a,0x49a3,{0xb2,0xf8,0x59,0x42,0xe1,0xea,0x28,0x7e}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMachine2_FormatStringOffsetTable[] =
    {
    0,
    34,
    62,
    96,
    130,
    158,
    186
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMachine2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMachine2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMachine2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMachine2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IDebugMachine2ProxyVtbl = 
{
    &IDebugMachine2_ProxyInfo,
    &IID_IDebugMachine2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::GetPortSupplier */ ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::GetPort */ ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::EnumPorts */ ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::EnumPortSuppliers */ ,
    (void *) (INT_PTR) -1 /* IDebugMachine2::GetMachineUtilities */
};

const CInterfaceStubVtbl _IDebugMachine2StubVtbl =
{
    &IID_IDebugMachine2,
    &IDebugMachine2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMachineEx2, ver. 0.0,
   GUID={0xae27b230,0xa0bf,0x47ff,{0xa2,0xd1,0x22,0xc2,0x9a,0x17,0x8e,0xac}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMachineEx2_FormatStringOffsetTable[] =
    {
    214,
    260,
    288,
    328,
    368,
    402
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMachineEx2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMachineEx2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMachineEx2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMachineEx2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IDebugMachineEx2ProxyVtbl = 
{
    &IDebugMachineEx2_ProxyInfo,
    &IID_IDebugMachineEx2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMachineEx2::EnableAutoAttachOnProgramCreate */ ,
    (void *) (INT_PTR) -1 /* IDebugMachineEx2::DisableAutoAttachOnEvent */ ,
    (void *) (INT_PTR) -1 /* IDebugMachineEx2::GetPortSupplierEx */ ,
    (void *) (INT_PTR) -1 /* IDebugMachineEx2::GetPortEx */ ,
    (void *) (INT_PTR) -1 /* IDebugMachineEx2::EnumPortsEx */ ,
    (void *) (INT_PTR) -1 /* IDebugMachineEx2::EnumPortSuppliersEx */
};

const CInterfaceStubVtbl _IDebugMachineEx2StubVtbl =
{
    &IID_IDebugMachineEx2,
    &IDebugMachineEx2_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPortSupplier2, ver. 0.0,
   GUID={0x53f68191,0x7b2f,0x4f14,{0x8e,0x55,0x40,0xb1,0xb6,0xe5,0xdf,0x66}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPortSupplier2_FormatStringOffsetTable[] =
    {
    436,
    464,
    492,
    526,
    554,
    576,
    610
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPortSupplier2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPortSupplier2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPortSupplier2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPortSupplier2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IDebugPortSupplier2ProxyVtbl = 
{
    &IDebugPortSupplier2_ProxyInfo,
    &IID_IDebugPortSupplier2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::GetPortSupplierName */ ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::GetPortSupplierId */ ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::GetPort */ ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::EnumPorts */ ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::CanAddPort */ ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::AddPort */ ,
    (void *) (INT_PTR) -1 /* IDebugPortSupplier2::RemovePort */
};

const CInterfaceStubVtbl _IDebugPortSupplier2StubVtbl =
{
    &IID_IDebugPortSupplier2,
    &IDebugPortSupplier2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPort2, ver. 0.0,
   GUID={0x79293cc8,0xd9d9,0x43f5,{0x97,0xad,0x0b,0xcc,0x5a,0x68,0x87,0x76}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPort2_FormatStringOffsetTable[] =
    {
    436,
    464,
    638,
    666,
    694,
    728
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPort2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPort2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPort2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPort2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IDebugPort2ProxyVtbl = 
{
    &IDebugPort2_ProxyInfo,
    &IID_IDebugPort2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPort2::GetPortName */ ,
    (void *) (INT_PTR) -1 /* IDebugPort2::GetPortId */ ,
    (void *) (INT_PTR) -1 /* IDebugPort2::GetPortRequest */ ,
    (void *) (INT_PTR) -1 /* IDebugPort2::GetPortSupplier */ ,
    (void *) (INT_PTR) -1 /* IDebugPort2::GetProcess */ ,
    (void *) (INT_PTR) -1 /* IDebugPort2::EnumProcesses */
};

const CInterfaceStubVtbl _IDebugPort2StubVtbl =
{
    &IID_IDebugPort2,
    &IDebugPort2_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPortRequest2, ver. 0.0,
   GUID={0x8d36beb8,0x9bfe,0x47dd,{0xa1,0x1b,0x7b,0xa1,0xde,0x18,0xe4,0x49}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPortRequest2_FormatStringOffsetTable[] =
    {
    436
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPortRequest2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPortRequest2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPortRequest2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPortRequest2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugPortRequest2ProxyVtbl = 
{
    &IDebugPortRequest2_ProxyInfo,
    &IID_IDebugPortRequest2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPortRequest2::GetPortName */
};

const CInterfaceStubVtbl _IDebugPortRequest2StubVtbl =
{
    &IID_IDebugPortRequest2,
    &IDebugPortRequest2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPortNotify2, ver. 0.0,
   GUID={0xfb8d2032,0x2858,0x414c,{0x83,0xd9,0xf7,0x32,0x66,0x4e,0x0c,0x7a}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPortNotify2_FormatStringOffsetTable[] =
    {
    756,
    784
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPortNotify2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPortNotify2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPortNotify2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPortNotify2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugPortNotify2ProxyVtbl = 
{
    &IDebugPortNotify2_ProxyInfo,
    &IID_IDebugPortNotify2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPortNotify2::AddProgramNode */ ,
    (void *) (INT_PTR) -1 /* IDebugPortNotify2::RemoveProgramNode */
};

const CInterfaceStubVtbl _IDebugPortNotify2StubVtbl =
{
    &IID_IDebugPortNotify2,
    &IDebugPortNotify2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPortEvents2, ver. 0.0,
   GUID={0x564fa275,0x12e1,0x4b5f,{0x83,0x16,0x4d,0x79,0xbc,0xef,0x72,0x46}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPortEvents2_FormatStringOffsetTable[] =
    {
    812
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPortEvents2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPortEvents2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPortEvents2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPortEvents2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugPortEvents2ProxyVtbl = 
{
    &IDebugPortEvents2_ProxyInfo,
    &IID_IDebugPortEvents2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPortEvents2::Event */
};

const CInterfaceStubVtbl _IDebugPortEvents2StubVtbl =
{
    &IID_IDebugPortEvents2,
    &IDebugPortEvents2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMDMUtil2, ver. 0.0,
   GUID={0xf3062547,0x43d8,0x4dc2,{0xb1,0x8e,0xe1,0x46,0x0f,0xf2,0xc4,0x22}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMDMUtil2_FormatStringOffsetTable[] =
    {
    870,
    904,
    938,
    972,
    1006,
    1040,
    1074,
    1102,
    1130,
    1182
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMDMUtil2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMDMUtil2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMDMUtil2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMDMUtil2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IDebugMDMUtil2ProxyVtbl = 
{
    &IDebugMDMUtil2_ProxyInfo,
    &IID_IDebugMDMUtil2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::AddPIDToIgnore */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::RemovePIDToIgnore */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::AddPIDToDebug */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::RemovePIDToDebug */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::SetDynamicDebuggingFlags */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::GetDynamicDebuggingFlags */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::SetDefaultJITServer */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::GetDefaultJITServer */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::RegisterJITDebugEngines */ ,
    (void *) (INT_PTR) -1 /* IDebugMDMUtil2::CanDebugPID */
};

const CInterfaceStubVtbl _IDebugMDMUtil2StubVtbl =
{
    &IID_IDebugMDMUtil2,
    &IDebugMDMUtil2_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugSession2, ver. 0.0,
   GUID={0x8948300f,0x8bd5,0x4728,{0xa1,0xd8,0x83,0xd1,0x72,0x29,0x5a,0x9d}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugSession2_FormatStringOffsetTable[] =
    {
    436,
    1216,
    1244,
    1272,
    1390,
    1418,
    1446,
    1468,
    1490,
    1524,
    1564,
    1592,
    1626,
    1654,
    1676,
    1716,
    1744,
    1790,
    1818,
    1846,
    1880,
    1914,
    1942,
    1970,
    1992,
    2014,
    2066,
    2106,
    2134,
    2162
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugSession2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugSession2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugSession2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugSession2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(33) _IDebugSession2ProxyVtbl = 
{
    &IDebugSession2_ProxyInfo,
    &IID_IDebugSession2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugSession2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::SetName */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::EnumProcesses */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::Launch */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::RegisterJITServer */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::Terminate */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::Detach */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::CauseBreak */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::CreatePendingBreakpoint */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::EnumPendingBreakpoints */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::EnumMachines */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::AddMachine */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::RemoveMachine */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::ShutdownSession */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::EnumCodeContexts */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::SetException */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::EnumSetExceptions */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::RemoveSetException */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::RemoveAllSetExceptions */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::EnumDefaultExceptions */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::GetENCUpdate */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::SetLocale */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::SetRegistryRoot */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::IsAlive */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::ClearAllSessionThreadStackFrames */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::GetSessionId */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::SetEngineMetric */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::SetStoppingModel */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::GetStoppingModel */ ,
    (void *) (INT_PTR) -1 /* IDebugSession2::RegisterSessionWithServer */
};

const CInterfaceStubVtbl _IDebugSession2StubVtbl =
{
    &IID_IDebugSession2,
    &IDebugSession2_ServerInfo,
    33,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEngine2, ver. 0.0,
   GUID={0xba105b52,0x12f1,0x4038,{0xae,0x64,0xd9,0x57,0x85,0x87,0x4c,0x47}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEngine2_FormatStringOffsetTable[] =
    {
    2190,
    2218,
    2270,
    2304,
    2332,
    2360,
    2388,
    2416,
    2444,
    2472,
    2500,
    2528,
    2562
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEngine2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEngine2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEngine2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEngine2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(16) _IDebugEngine2ProxyVtbl = 
{
    &IDebugEngine2_ProxyInfo,
    &IID_IDebugEngine2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::EnumPrograms */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::Attach */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::CreatePendingBreakpoint */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::SetException */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::RemoveSetException */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::RemoveAllSetExceptions */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::GetEngineId */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::DestroyProgram */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::ContinueFromSynchronousEvent */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::SetLocale */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::SetRegistryRoot */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::SetMetric */ ,
    (void *) (INT_PTR) -1 /* IDebugEngine2::CauseBreak */
};

const CInterfaceStubVtbl _IDebugEngine2StubVtbl =
{
    &IID_IDebugEngine2,
    &IDebugEngine2_ServerInfo,
    16,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEngineLaunch2, ver. 0.0,
   GUID={0xc7c1462f,0x9736,0x466c,{0xb2,0xc1,0xb6,0xb2,0xde,0xdb,0xf4,0xa7}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEngineLaunch2_FormatStringOffsetTable[] =
    {
    2584,
    2684,
    2712,
    2740
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEngineLaunch2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineLaunch2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEngineLaunch2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineLaunch2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDebugEngineLaunch2ProxyVtbl = 
{
    &IDebugEngineLaunch2_ProxyInfo,
    &IID_IDebugEngineLaunch2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEngineLaunch2::LaunchSuspended */ ,
    (void *) (INT_PTR) -1 /* IDebugEngineLaunch2::ResumeProcess */ ,
    (void *) (INT_PTR) -1 /* IDebugEngineLaunch2::CanTerminateProcess */ ,
    (void *) (INT_PTR) -1 /* IDebugEngineLaunch2::TerminateProcess */
};

const CInterfaceStubVtbl _IDebugEngineLaunch2StubVtbl =
{
    &IID_IDebugEngineLaunch2,
    &IDebugEngineLaunch2_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEventCallback2, ver. 0.0,
   GUID={0xade2eeb9,0xfc85,0x4f5b,{0xb5,0xd9,0xd4,0x31,0xb4,0xaa,0xc3,0x1a}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEventCallback2_FormatStringOffsetTable[] =
    {
    2768
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEventCallback2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEventCallback2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEventCallback2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEventCallback2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugEventCallback2ProxyVtbl = 
{
    &IDebugEventCallback2_ProxyInfo,
    &IID_IDebugEventCallback2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEventCallback2::Event */
};

const CInterfaceStubVtbl _IDebugEventCallback2StubVtbl =
{
    &IID_IDebugEventCallback2,
    &IDebugEventCallback2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEvent2, ver. 0.0,
   GUID={0x423238d6,0xda42,0x4989,{0x96,0xfb,0x6b,0xba,0x26,0xe7,0x2e,0x09}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEvent2_FormatStringOffsetTable[] =
    {
    2832
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugEvent2ProxyVtbl = 
{
    &IDebugEvent2_ProxyInfo,
    &IID_IDebugEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEvent2::GetAttributes */
};

const CInterfaceStubVtbl _IDebugEvent2StubVtbl =
{
    &IID_IDebugEvent2,
    &IDebugEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugSessionCreateEvent2, ver. 0.0,
   GUID={0x2c2b15b7,0xfc6d,0x45b3,{0x96,0x22,0x29,0x66,0x5d,0x96,0x4a,0x76}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugSessionCreateEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugSessionCreateEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugSessionCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugSessionCreateEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugSessionCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugSessionCreateEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugSessionCreateEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugSessionCreateEvent2StubVtbl =
{
    &IID_IDebugSessionCreateEvent2,
    &IDebugSessionCreateEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugSessionDestroyEvent2, ver. 0.0,
   GUID={0xf199b2c2,0x88fe,0x4c5d,{0xa0,0xfd,0xaa,0x04,0x6b,0x0d,0xc0,0xdc}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugSessionDestroyEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugSessionDestroyEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugSessionDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugSessionDestroyEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugSessionDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugSessionDestroyEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugSessionDestroyEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugSessionDestroyEvent2StubVtbl =
{
    &IID_IDebugSessionDestroyEvent2,
    &IDebugSessionDestroyEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEngineCreateEvent2, ver. 0.0,
   GUID={0xfe5b734c,0x759d,0x4e59,{0xab,0x04,0xf1,0x03,0x34,0x3b,0xdd,0x06}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEngineCreateEvent2_FormatStringOffsetTable[] =
    {
    2860
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEngineCreateEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEngineCreateEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugEngineCreateEvent2ProxyVtbl = 
{
    &IDebugEngineCreateEvent2_ProxyInfo,
    &IID_IDebugEngineCreateEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEngineCreateEvent2::GetEngine */
};

const CInterfaceStubVtbl _IDebugEngineCreateEvent2StubVtbl =
{
    &IID_IDebugEngineCreateEvent2,
    &IDebugEngineCreateEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProcessCreateEvent2, ver. 0.0,
   GUID={0xbac3780f,0x04da,0x4726,{0x90,0x1c,0xba,0x6a,0x46,0x33,0xe1,0xca}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProcessCreateEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProcessCreateEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProcessCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProcessCreateEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProcessCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugProcessCreateEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugProcessCreateEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugProcessCreateEvent2StubVtbl =
{
    &IID_IDebugProcessCreateEvent2,
    &IDebugProcessCreateEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProcessDestroyEvent2, ver. 0.0,
   GUID={0x3e2a0832,0x17e1,0x4886,{0x8c,0x0e,0x20,0x4d,0xa2,0x42,0x99,0x5f}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProcessDestroyEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProcessDestroyEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProcessDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProcessDestroyEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProcessDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugProcessDestroyEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugProcessDestroyEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugProcessDestroyEvent2StubVtbl =
{
    &IID_IDebugProcessDestroyEvent2,
    &IDebugProcessDestroyEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgramCreateEvent2, ver. 0.0,
   GUID={0x96cd11ee,0xecd4,0x4e89,{0x95,0x7e,0xb5,0xd4,0x96,0xfc,0x41,0x39}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgramCreateEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgramCreateEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgramCreateEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugProgramCreateEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugProgramCreateEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugProgramCreateEvent2StubVtbl =
{
    &IID_IDebugProgramCreateEvent2,
    &IDebugProgramCreateEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgramDestroyEvent2, ver. 0.0,
   GUID={0xe147e9e3,0x6440,0x4073,{0xa7,0xb7,0xa6,0x55,0x92,0xc7,0x14,0xb5}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgramDestroyEvent2_FormatStringOffsetTable[] =
    {
    2832
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgramDestroyEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgramDestroyEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugProgramDestroyEvent2ProxyVtbl = 
{
    &IDebugProgramDestroyEvent2_ProxyInfo,
    &IID_IDebugProgramDestroyEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProgramDestroyEvent2::GetExitCode */
};

const CInterfaceStubVtbl _IDebugProgramDestroyEvent2StubVtbl =
{
    &IID_IDebugProgramDestroyEvent2,
    &IDebugProgramDestroyEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugThreadCreateEvent2, ver. 0.0,
   GUID={0x2090ccfc,0x70c5,0x491d,{0xa5,0xe8,0xba,0xd2,0xdd,0x9e,0xe3,0xea}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugThreadCreateEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugThreadCreateEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugThreadCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugThreadCreateEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugThreadCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugThreadCreateEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugThreadCreateEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugThreadCreateEvent2StubVtbl =
{
    &IID_IDebugThreadCreateEvent2,
    &IDebugThreadCreateEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugThreadDestroyEvent2, ver. 0.0,
   GUID={0x2c3b7532,0xa36f,0x4a6e,{0x90,0x72,0x49,0xbe,0x64,0x9b,0x85,0x41}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugThreadDestroyEvent2_FormatStringOffsetTable[] =
    {
    2832
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugThreadDestroyEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugThreadDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugThreadDestroyEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugThreadDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugThreadDestroyEvent2ProxyVtbl = 
{
    &IDebugThreadDestroyEvent2_ProxyInfo,
    &IID_IDebugThreadDestroyEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugThreadDestroyEvent2::GetExitCode */
};

const CInterfaceStubVtbl _IDebugThreadDestroyEvent2StubVtbl =
{
    &IID_IDebugThreadDestroyEvent2,
    &IDebugThreadDestroyEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugLoadCompleteEvent2, ver. 0.0,
   GUID={0xb1844850,0x1349,0x45d4,{0x9f,0x12,0x49,0x52,0x12,0xf5,0xeb,0x0b}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugLoadCompleteEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugLoadCompleteEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugLoadCompleteEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugLoadCompleteEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugLoadCompleteEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugLoadCompleteEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugLoadCompleteEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugLoadCompleteEvent2StubVtbl =
{
    &IID_IDebugLoadCompleteEvent2,
    &IDebugLoadCompleteEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEntryPointEvent2, ver. 0.0,
   GUID={0xe8414a3e,0x1642,0x48ec,{0x82,0x9e,0x5f,0x40,0x40,0xe1,0x6d,0xa9}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEntryPointEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEntryPointEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEntryPointEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEntryPointEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEntryPointEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugEntryPointEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugEntryPointEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugEntryPointEvent2StubVtbl =
{
    &IID_IDebugEntryPointEvent2,
    &IDebugEntryPointEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugStepCompleteEvent2, ver. 0.0,
   GUID={0x0f7f24c1,0x74d9,0x4ea6,{0xa3,0xea,0x7e,0xdb,0x2d,0x81,0x44,0x1d}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugStepCompleteEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugStepCompleteEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugStepCompleteEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugStepCompleteEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugStepCompleteEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugStepCompleteEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugStepCompleteEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugStepCompleteEvent2StubVtbl =
{
    &IID_IDebugStepCompleteEvent2,
    &IDebugStepCompleteEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugCanStopEvent2, ver. 0.0,
   GUID={0xb5b0d747,0xd4d2,0x4e2d,{0x87,0x2d,0x74,0xda,0x22,0x03,0x78,0x26}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugCanStopEvent2_FormatStringOffsetTable[] =
    {
    2832,
    260,
    2888,
    2916
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugCanStopEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugCanStopEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugCanStopEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugCanStopEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDebugCanStopEvent2ProxyVtbl = 
{
    &IDebugCanStopEvent2_ProxyInfo,
    &IID_IDebugCanStopEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugCanStopEvent2::GetReason */ ,
    (void *) (INT_PTR) -1 /* IDebugCanStopEvent2::CanStop */ ,
    (void *) (INT_PTR) -1 /* IDebugCanStopEvent2::GetDocumentContext */ ,
    (void *) (INT_PTR) -1 /* IDebugCanStopEvent2::GetCodeContext */
};

const CInterfaceStubVtbl _IDebugCanStopEvent2StubVtbl =
{
    &IID_IDebugCanStopEvent2,
    &IDebugCanStopEvent2_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBreakEvent2, ver. 0.0,
   GUID={0xc7405d1d,0xe24b,0x44e0,{0xb7,0x07,0xd8,0xa5,0xa4,0xe1,0x64,0x1b}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugBreakEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugBreakEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugBreakEvent2StubVtbl =
{
    &IID_IDebugBreakEvent2,
    &IDebugBreakEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBreakpointEvent2, ver. 0.0,
   GUID={0x501c1e21,0xc557,0x48b8,{0xba,0x30,0xa1,0xea,0xb0,0xbc,0x4a,0x74}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakpointEvent2_FormatStringOffsetTable[] =
    {
    2944
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakpointEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugBreakpointEvent2ProxyVtbl = 
{
    &IDebugBreakpointEvent2_ProxyInfo,
    &IID_IDebugBreakpointEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointEvent2::EnumBreakpoints */
};

const CInterfaceStubVtbl _IDebugBreakpointEvent2StubVtbl =
{
    &IID_IDebugBreakpointEvent2,
    &IDebugBreakpointEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugExceptionEvent2, ver. 0.0,
   GUID={0x51a94113,0x8788,0x4a54,{0xae,0x15,0x08,0xb7,0x4f,0xf9,0x22,0xd0}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugExceptionEvent2_FormatStringOffsetTable[] =
    {
    2972,
    34,
    3000,
    3022
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugExceptionEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugExceptionEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugExceptionEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugExceptionEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDebugExceptionEvent2ProxyVtbl = 
{
    &IDebugExceptionEvent2_ProxyInfo,
    &IID_IDebugExceptionEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugExceptionEvent2::GetException */ ,
    (void *) (INT_PTR) -1 /* IDebugExceptionEvent2::GetExceptionDescription */ ,
    (void *) (INT_PTR) -1 /* IDebugExceptionEvent2::CanPassToDebuggee */ ,
    (void *) (INT_PTR) -1 /* IDebugExceptionEvent2::PassToDebuggee */
};

const CInterfaceStubVtbl _IDebugExceptionEvent2StubVtbl =
{
    &IID_IDebugExceptionEvent2,
    &IDebugExceptionEvent2_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugOutputStringEvent2, ver. 0.0,
   GUID={0x569c4bb1,0x7b82,0x46fc,{0xae,0x28,0x45,0x36,0xdd,0xad,0x75,0x3e}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugOutputStringEvent2_FormatStringOffsetTable[] =
    {
    436
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugOutputStringEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugOutputStringEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugOutputStringEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugOutputStringEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugOutputStringEvent2ProxyVtbl = 
{
    &IDebugOutputStringEvent2_ProxyInfo,
    &IID_IDebugOutputStringEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugOutputStringEvent2::GetString */
};

const CInterfaceStubVtbl _IDebugOutputStringEvent2StubVtbl =
{
    &IID_IDebugOutputStringEvent2,
    &IDebugOutputStringEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugModuleLoadEvent2, ver. 0.0,
   GUID={0x989db083,0x0d7c,0x40d1,{0xa9,0xd9,0x92,0x1b,0xf6,0x11,0xa4,0xb2}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugModuleLoadEvent2_FormatStringOffsetTable[] =
    {
    3050
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugModuleLoadEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugModuleLoadEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugModuleLoadEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugModuleLoadEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugModuleLoadEvent2ProxyVtbl = 
{
    &IDebugModuleLoadEvent2_ProxyInfo,
    &IID_IDebugModuleLoadEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugModuleLoadEvent2::GetModule */
};

const CInterfaceStubVtbl _IDebugModuleLoadEvent2StubVtbl =
{
    &IID_IDebugModuleLoadEvent2,
    &IDebugModuleLoadEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPropertyCreateEvent2, ver. 0.0,
   GUID={0xded6d613,0xa3db,0x4e35,{0xbb,0x5b,0xa9,0x23,0x91,0x13,0x3f,0x03}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPropertyCreateEvent2_FormatStringOffsetTable[] =
    {
    3090
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPropertyCreateEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPropertyCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPropertyCreateEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPropertyCreateEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugPropertyCreateEvent2ProxyVtbl = 
{
    &IDebugPropertyCreateEvent2_ProxyInfo,
    &IID_IDebugPropertyCreateEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPropertyCreateEvent2::GetDebugProperty */
};

const CInterfaceStubVtbl _IDebugPropertyCreateEvent2StubVtbl =
{
    &IID_IDebugPropertyCreateEvent2,
    &IDebugPropertyCreateEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPropertyDestroyEvent2, ver. 0.0,
   GUID={0xf3765f18,0xf395,0x4b8c,{0x8e,0x95,0xdc,0xb3,0xfe,0x8e,0x7e,0xc8}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPropertyDestroyEvent2_FormatStringOffsetTable[] =
    {
    3090
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPropertyDestroyEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPropertyDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPropertyDestroyEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPropertyDestroyEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugPropertyDestroyEvent2ProxyVtbl = 
{
    &IDebugPropertyDestroyEvent2_ProxyInfo,
    &IID_IDebugPropertyDestroyEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPropertyDestroyEvent2::GetDebugProperty */
};

const CInterfaceStubVtbl _IDebugPropertyDestroyEvent2StubVtbl =
{
    &IID_IDebugPropertyDestroyEvent2,
    &IDebugPropertyDestroyEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBreakpointBoundEvent2, ver. 0.0,
   GUID={0x1dddb704,0xcf99,0x4b8a,{0xb7,0x46,0xda,0xbb,0x01,0xdd,0x13,0xa0}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakpointBoundEvent2_FormatStringOffsetTable[] =
    {
    3118,
    3146
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointBoundEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointBoundEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakpointBoundEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointBoundEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugBreakpointBoundEvent2ProxyVtbl = 
{
    &IDebugBreakpointBoundEvent2_ProxyInfo,
    &IID_IDebugBreakpointBoundEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointBoundEvent2::GetPendingBreakpoint */ ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointBoundEvent2::EnumBoundBreakpoints */
};

const CInterfaceStubVtbl _IDebugBreakpointBoundEvent2StubVtbl =
{
    &IID_IDebugBreakpointBoundEvent2,
    &IDebugBreakpointBoundEvent2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBreakpointUnboundEvent2, ver. 0.0,
   GUID={0x78d1db4f,0xc557,0x4dc5,{0xa2,0xdd,0x53,0x69,0xd2,0x1b,0x1c,0x8c}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakpointUnboundEvent2_FormatStringOffsetTable[] =
    {
    3174,
    3202
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointUnboundEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointUnboundEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakpointUnboundEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointUnboundEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugBreakpointUnboundEvent2ProxyVtbl = 
{
    &IDebugBreakpointUnboundEvent2_ProxyInfo,
    &IID_IDebugBreakpointUnboundEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointUnboundEvent2::GetBreakpoint */ ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointUnboundEvent2::GetReason */
};

const CInterfaceStubVtbl _IDebugBreakpointUnboundEvent2StubVtbl =
{
    &IID_IDebugBreakpointUnboundEvent2,
    &IDebugBreakpointUnboundEvent2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBreakpointErrorEvent2, ver. 0.0,
   GUID={0xabb0ca42,0xf82b,0x4622,{0x84,0xe4,0x69,0x03,0xae,0x90,0xf2,0x10}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakpointErrorEvent2_FormatStringOffsetTable[] =
    {
    3230
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointErrorEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointErrorEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakpointErrorEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointErrorEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugBreakpointErrorEvent2ProxyVtbl = 
{
    &IDebugBreakpointErrorEvent2_ProxyInfo,
    &IID_IDebugBreakpointErrorEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointErrorEvent2::GetErrorBreakpoint */
};

const CInterfaceStubVtbl _IDebugBreakpointErrorEvent2StubVtbl =
{
    &IID_IDebugBreakpointErrorEvent2,
    &IDebugBreakpointErrorEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugExpressionEvaluationCompleteEvent2, ver. 0.0,
   GUID={0xc0e13a85,0x238a,0x4800,{0x83,0x15,0xd9,0x47,0xc9,0x60,0xa8,0x43}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugExpressionEvaluationCompleteEvent2_FormatStringOffsetTable[] =
    {
    3258,
    3286
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugExpressionEvaluationCompleteEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugExpressionEvaluationCompleteEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugExpressionEvaluationCompleteEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugExpressionEvaluationCompleteEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugExpressionEvaluationCompleteEvent2ProxyVtbl = 
{
    &IDebugExpressionEvaluationCompleteEvent2_ProxyInfo,
    &IID_IDebugExpressionEvaluationCompleteEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugExpressionEvaluationCompleteEvent2::GetExpression */ ,
    (void *) (INT_PTR) -1 /* IDebugExpressionEvaluationCompleteEvent2::GetResult */
};

const CInterfaceStubVtbl _IDebugExpressionEvaluationCompleteEvent2StubVtbl =
{
    &IID_IDebugExpressionEvaluationCompleteEvent2,
    &IDebugExpressionEvaluationCompleteEvent2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugReturnValueEvent2, ver. 0.0,
   GUID={0x0da4d4cc,0x2d0b,0x410f,{0x8d,0x5d,0xb6,0xb7,0x3a,0x5d,0x35,0xd8}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugReturnValueEvent2_FormatStringOffsetTable[] =
    {
    3314
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugReturnValueEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugReturnValueEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugReturnValueEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugReturnValueEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugReturnValueEvent2ProxyVtbl = 
{
    &IDebugReturnValueEvent2_ProxyInfo,
    &IID_IDebugReturnValueEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugReturnValueEvent2::GetReturnValue */
};

const CInterfaceStubVtbl _IDebugReturnValueEvent2StubVtbl =
{
    &IID_IDebugReturnValueEvent2,
    &IDebugReturnValueEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugFindSymbolEvent2, ver. 0.0,
   GUID={0x4072ae37,0x3739,0x43c9,{0xac,0x88,0xca,0x2e,0xa7,0x9e,0xd3,0x2b}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugFindSymbolEvent2_FormatStringOffsetTable[] =
    {
    3342
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugFindSymbolEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugFindSymbolEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugFindSymbolEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugFindSymbolEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugFindSymbolEvent2ProxyVtbl = 
{
    &IDebugFindSymbolEvent2_ProxyInfo,
    &IID_IDebugFindSymbolEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugFindSymbolEvent2::SearchWithPath */
};

const CInterfaceStubVtbl _IDebugFindSymbolEvent2StubVtbl =
{
    &IID_IDebugFindSymbolEvent2,
    &IDebugFindSymbolEvent2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugNoSymbolsEvent2, ver. 0.0,
   GUID={0x3ad4fb48,0x647e,0x4b03,{0x9c,0x1e,0x52,0x75,0x4e,0x80,0xc8,0x80}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugNoSymbolsEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugNoSymbolsEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugNoSymbolsEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugNoSymbolsEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugNoSymbolsEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugNoSymbolsEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugNoSymbolsEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugNoSymbolsEvent2StubVtbl =
{
    &IID_IDebugNoSymbolsEvent2,
    &IDebugNoSymbolsEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgramNameChangedEvent2, ver. 0.0,
   GUID={0xe05c2dfd,0x59d5,0x46d3,{0xa7,0x1c,0x5d,0x07,0x66,0x5d,0x85,0xaf}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgramNameChangedEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgramNameChangedEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramNameChangedEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgramNameChangedEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramNameChangedEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugProgramNameChangedEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugProgramNameChangedEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugProgramNameChangedEvent2StubVtbl =
{
    &IID_IDebugProgramNameChangedEvent2,
    &IDebugProgramNameChangedEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugThreadNameChangedEvent2, ver. 0.0,
   GUID={0x1ef4ef78,0x2c44,0x4b7a,{0x84,0x73,0x8f,0x43,0x57,0x61,0x17,0x29}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugThreadNameChangedEvent2_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugThreadNameChangedEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugThreadNameChangedEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugThreadNameChangedEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugThreadNameChangedEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _IDebugThreadNameChangedEvent2ProxyVtbl = 
{
    0,
    &IID_IDebugThreadNameChangedEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IDebugThreadNameChangedEvent2StubVtbl =
{
    &IID_IDebugThreadNameChangedEvent2,
    &IDebugThreadNameChangedEvent2_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMessageEvent2, ver. 0.0,
   GUID={0x3bdb28cf,0xdbd2,0x4d24,{0xaf,0x03,0x01,0x07,0x2b,0x67,0xeb,0x9e}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMessageEvent2_FormatStringOffsetTable[] =
    {
    3370,
    260
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMessageEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMessageEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMessageEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMessageEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugMessageEvent2ProxyVtbl = 
{
    &IDebugMessageEvent2_ProxyInfo,
    &IID_IDebugMessageEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMessageEvent2::GetMessage */ ,
    (void *) (INT_PTR) -1 /* IDebugMessageEvent2::SetResponse */
};

const CInterfaceStubVtbl _IDebugMessageEvent2StubVtbl =
{
    &IID_IDebugMessageEvent2,
    &IDebugMessageEvent2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugActivateDocumentEvent2, ver. 0.0,
   GUID={0x58f36c3d,0x7d07,0x4eba,{0xa0,0x41,0x62,0xf6,0x3e,0x18,0x80,0x37}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugActivateDocumentEvent2_FormatStringOffsetTable[] =
    {
    3422,
    3450
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugActivateDocumentEvent2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugActivateDocumentEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugActivateDocumentEvent2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugActivateDocumentEvent2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugActivateDocumentEvent2ProxyVtbl = 
{
    &IDebugActivateDocumentEvent2_ProxyInfo,
    &IID_IDebugActivateDocumentEvent2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugActivateDocumentEvent2::GetDocument */ ,
    (void *) (INT_PTR) -1 /* IDebugActivateDocumentEvent2::GetDocumentContext */
};

const CInterfaceStubVtbl _IDebugActivateDocumentEvent2StubVtbl =
{
    &IID_IDebugActivateDocumentEvent2,
    &IDebugActivateDocumentEvent2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProcess2, ver. 0.0,
   GUID={0x43286fea,0x6997,0x4543,{0x80,0x3e,0x60,0xa2,0x0c,0x47,0x3d,0xe5}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProcess2_FormatStringOffsetTable[] =
    {
    3478,
    3512,
    3540,
    3574,
    554,
    3602,
    1446,
    1468,
    3648,
    3676,
    3704,
    3732,
    2562,
    3760
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProcess2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProcess2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProcess2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProcess2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _IDebugProcess2ProxyVtbl = 
{
    &IDebugProcess2_ProxyInfo,
    &IID_IDebugProcess2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::EnumPrograms */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetMachine */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::Terminate */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::Attach */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::CanDetach */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::Detach */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetPhysicalProcessId */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetProcessId */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetAttachedSessionName */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::EnumThreads */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::CauseBreak */ ,
    (void *) (INT_PTR) -1 /* IDebugProcess2::GetPort */
};

const CInterfaceStubVtbl _IDebugProcess2StubVtbl =
{
    &IID_IDebugProcess2,
    &IDebugProcess2_ServerInfo,
    17,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgram2, ver. 0.0,
   GUID={0x69d172ef,0xf2c4,0x44e1,{0x89,0xf7,0xc8,0x62,0x31,0xe7,0x06,0xe9}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgram2_FormatStringOffsetTable[] =
    {
    3788,
    34,
    3816,
    3844,
    3866,
    3894,
    1446,
    1102,
    3916,
    3944,
    3966,
    3994,
    2562,
    4034,
    4068,
    4102,
    4130,
    4170,
    4198,
    4226,
    4284
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgram2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgram2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgram2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgram2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(24) _IDebugProgram2ProxyVtbl = 
{
    &IDebugProgram2_ProxyInfo,
    &IID_IDebugProgram2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::EnumThreads */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetProcess */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::Terminate */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::Attach */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::CanDetach */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::Detach */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetProgramId */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetDebugProperty */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::Execute */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::Continue */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::Step */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::CauseBreak */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetEngineInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::EnumCodeContexts */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetMemoryBytes */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetDisassemblyStream */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::EnumModules */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::GetENCUpdate */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::EnumCodePaths */ ,
    (void *) (INT_PTR) -1 /* IDebugProgram2::WriteDump */
};

const CInterfaceStubVtbl _IDebugProgram2StubVtbl =
{
    &IID_IDebugProgram2,
    &IDebugProgram2_ServerInfo,
    24,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugEngineProgram2, ver. 0.0,
   GUID={0x7ce3e768,0x654d,0x4ba7,{0x8d,0x95,0xcd,0xaa,0xc6,0x42,0xb1,0x41}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugEngineProgram2_FormatStringOffsetTable[] =
    {
    4318,
    4340,
    4386
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugEngineProgram2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineProgram2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugEngineProgram2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugEngineProgram2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugEngineProgram2ProxyVtbl = 
{
    &IDebugEngineProgram2_ProxyInfo,
    &IID_IDebugEngineProgram2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugEngineProgram2::Stop */ ,
    (void *) (INT_PTR) -1 /* IDebugEngineProgram2::WatchForThreadStep */ ,
    (void *) (INT_PTR) -1 /* IDebugEngineProgram2::WatchForExpressionEvaluationOnThread */
};

const CInterfaceStubVtbl _IDebugEngineProgram2StubVtbl =
{
    &IID_IDebugEngineProgram2,
    &IDebugEngineProgram2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgramHost2, ver. 0.0,
   GUID={0xc99d588f,0x778c,0x44fe,{0x8b,0x2e,0x40,0x12,0x4a,0x73,0x88,0x91}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgramHost2_FormatStringOffsetTable[] =
    {
    4438,
    4472,
    4500
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgramHost2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramHost2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgramHost2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramHost2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugProgramHost2ProxyVtbl = 
{
    &IDebugProgramHost2_ProxyInfo,
    &IID_IDebugProgramHost2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProgramHost2::GetHostName */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramHost2::GetHostId */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramHost2::GetHostMachineName */
};

const CInterfaceStubVtbl _IDebugProgramHost2StubVtbl =
{
    &IID_IDebugProgramHost2,
    &IDebugProgramHost2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgramNode2, ver. 0.0,
   GUID={0x426e255c,0xf1ce,0x4d02,{0xa9,0x31,0xf9,0xa2,0x54,0xbf,0x7f,0x0f}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgramNode2_FormatStringOffsetTable[] =
    {
    436,
    4528,
    4562,
    4590,
    4618,
    4658,
    1446
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgramNode2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramNode2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgramNode2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramNode2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IDebugProgramNode2ProxyVtbl = 
{
    &IDebugProgramNode2_ProxyInfo,
    &IID_IDebugProgramNode2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::GetProgramName */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::GetHostName */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::GetHostPid */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::GetHostMachineName */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::Attach */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::GetEngineInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramNode2::DetachDebugger */
};

const CInterfaceStubVtbl _IDebugProgramNode2StubVtbl =
{
    &IID_IDebugProgramNode2,
    &IDebugProgramNode2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugProgramEngines2, ver. 0.0,
   GUID={0xfda24a6b,0xb142,0x447d,{0xbb,0xbc,0x86,0x54,0xa3,0xd8,0x4f,0x80}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProgramEngines2_FormatStringOffsetTable[] =
    {
    4692,
    4732
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProgramEngines2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramEngines2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProgramEngines2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProgramEngines2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugProgramEngines2ProxyVtbl = 
{
    &IDebugProgramEngines2_ProxyInfo,
    &IID_IDebugProgramEngines2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProgramEngines2::EnumPossibleEngines */ ,
    (void *) (INT_PTR) -1 /* IDebugProgramEngines2::SetEngine */
};

const CInterfaceStubVtbl _IDebugProgramEngines2StubVtbl =
{
    &IID_IDebugProgramEngines2,
    &IDebugProgramEngines2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugThread2, ver. 0.0,
   GUID={0xd5168050,0xa57a,0x465c,{0xbe,0xa9,0x97,0x4f,0x40,0x5e,0xba,0x13}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugThread2_FormatStringOffsetTable[] =
    {
    4760,
    34,
    4800,
    4828,
    4856,
    4890,
    4924,
    4952,
    4980,
    5008,
    5042
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugThread2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugThread2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugThread2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugThread2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugThread2ProxyVtbl = 
{
    &IDebugThread2_ProxyInfo,
    &IID_IDebugThread2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugThread2::EnumFrameInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::SetThreadName */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::GetProgram */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::CanSetNextStatement */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::SetNextStatement */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::GetThreadId */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::Suspend */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::Resume */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::GetThreadProperties */ ,
    (void *) (INT_PTR) -1 /* IDebugThread2::GetLogicalThread */
};

const CInterfaceStubVtbl _IDebugThread2StubVtbl =
{
    &IID_IDebugThread2,
    &IDebugThread2_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugLogicalThread2, ver. 0.0,
   GUID={0x88d2f75b,0xd329,0x4e03,{0x9b,0x75,0x20,0x1f,0x77,0x82,0xd8,0xbd}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugLogicalThread2_FormatStringOffsetTable[] =
    {
    4760
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugLogicalThread2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugLogicalThread2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugLogicalThread2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugLogicalThread2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugLogicalThread2ProxyVtbl = 
{
    &IDebugLogicalThread2_ProxyInfo,
    &IID_IDebugLogicalThread2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugLogicalThread2::EnumFrameInfo */
};

const CInterfaceStubVtbl _IDebugLogicalThread2StubVtbl =
{
    &IID_IDebugLogicalThread2,
    &IDebugLogicalThread2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_msdbg_0350, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugProperty2, ver. 0.0,
   GUID={0xa7ee3e7e,0x2dd2,0x4ad7,{0x96,0x97,0xf4,0xaa,0xe3,0x42,0x77,0x62}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugProperty2_FormatStringOffsetTable[] =
    {
    5076,
    5134,
    5174,
    5220,
    5284,
    5312,
    5340,
    5368,
    4980,
    5396,
    5424
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugProperty2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugProperty2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugProperty2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugProperty2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugProperty2ProxyVtbl = 
{
    &IDebugProperty2_ProxyInfo,
    &IID_IDebugProperty2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetPropertyInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::SetValueAsString */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::SetValueAsReference */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::EnumChildren */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetParent */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetDerivedMostProperty */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetMemoryBytes */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetMemoryContext */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetReference */ ,
    (void *) (INT_PTR) -1 /* IDebugProperty2::GetExtendedInfo */
};

const CInterfaceStubVtbl _IDebugProperty2StubVtbl =
{
    &IID_IDebugProperty2,
    &IDebugProperty2_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_msdbg_0351, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugReference2, ver. 0.0,
   GUID={0x10b793ac,0x0c47,0x4679,{0x84,0x54,0xad,0xb3,0x6f,0x29,0xf8,0x02}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugReference2_FormatStringOffsetTable[] =
    {
    5458,
    5134,
    5516,
    5562,
    5620,
    5648,
    5676,
    5704,
    4980,
    5732,
    5760
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugReference2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugReference2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugReference2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugReference2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugReference2ProxyVtbl = 
{
    &IDebugReference2_ProxyInfo,
    &IID_IDebugReference2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugReference2::GetReferenceInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::SetValueAsString */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::SetValueAsReference */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::EnumChildren */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::GetParent */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::GetDerivedMostReference */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::GetMemoryBytes */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::GetMemoryContext */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::SetReferenceType */ ,
    (void *) (INT_PTR) -1 /* IDebugReference2::Compare */
};

const CInterfaceStubVtbl _IDebugReference2StubVtbl =
{
    &IID_IDebugReference2,
    &IDebugReference2_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugStackFrame2, ver. 0.0,
   GUID={0x1412926f,0x5dd6,0x4e58,{0xb6,0x48,0xe1,0xc6,0x3e,0x01,0x3d,0x51}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugStackFrame2_FormatStringOffsetTable[] =
    {
    5794,
    5822,
    4500,
    5850,
    5890,
    5924,
    5952,
    5986,
    6014,
    6072
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugStackFrame2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugStackFrame2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugStackFrame2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugStackFrame2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(13) _IDebugStackFrame2ProxyVtbl = 
{
    &IDebugStackFrame2_ProxyInfo,
    &IID_IDebugStackFrame2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetCodeContext */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetDocumentContext */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetPhysicalStackRange */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetExpressionContext */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetLanguageInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetDebugProperty */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::EnumProperties */ ,
    (void *) (INT_PTR) -1 /* IDebugStackFrame2::GetThread */
};

const CInterfaceStubVtbl _IDebugStackFrame2StubVtbl =
{
    &IID_IDebugStackFrame2,
    &IDebugStackFrame2_ServerInfo,
    13,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMemoryContext2, ver. 0.0,
   GUID={0x1ab276dd,0xf27b,0x4445,{0x82,0x5d,0x5d,0xf0,0xb4,0xa0,0x4a,0x3a}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMemoryContext2_FormatStringOffsetTable[] =
    {
    436,
    6100,
    6134,
    6168,
    6202
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMemoryContext2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMemoryContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMemoryContext2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMemoryContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IDebugMemoryContext2ProxyVtbl = 
{
    &IDebugMemoryContext2_ProxyInfo,
    &IID_IDebugMemoryContext2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::Add */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::Subtract */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::Compare */
};

const CInterfaceStubVtbl _IDebugMemoryContext2StubVtbl =
{
    &IID_IDebugMemoryContext2,
    &IDebugMemoryContext2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugCodeContext2, ver. 0.0,
   GUID={0xac17b76b,0x2b09,0x419a,{0xad,0x5f,0x7d,0x74,0x02,0xda,0x88,0x75}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugCodeContext2_FormatStringOffsetTable[] =
    {
    436,
    6100,
    6134,
    6168,
    6202,
    6248,
    5952
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugCodeContext2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugCodeContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugCodeContext2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugCodeContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IDebugCodeContext2ProxyVtbl = 
{
    &IDebugCodeContext2_ProxyInfo,
    &IID_IDebugCodeContext2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::Add */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::Subtract */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryContext2::Compare */ ,
    (void *) (INT_PTR) -1 /* IDebugCodeContext2::GetDocumentContext */ ,
    (void *) (INT_PTR) -1 /* IDebugCodeContext2::GetLanguageInfo */
};

const CInterfaceStubVtbl _IDebugCodeContext2StubVtbl =
{
    &IID_IDebugCodeContext2,
    &IDebugCodeContext2_ServerInfo,
    10,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugMemoryBytes2, ver. 0.0,
   GUID={0x925837d1,0x3aa1,0x451a,{0xb7,0xfe,0xcc,0x04,0xbb,0x42,0xcf,0xb8}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugMemoryBytes2_FormatStringOffsetTable[] =
    {
    6276,
    6328,
    6368
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugMemoryBytes2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugMemoryBytes2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugMemoryBytes2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugMemoryBytes2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugMemoryBytes2ProxyVtbl = 
{
    &IDebugMemoryBytes2_ProxyInfo,
    &IID_IDebugMemoryBytes2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugMemoryBytes2::ReadAt */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryBytes2::WriteAt */ ,
    (void *) (INT_PTR) -1 /* IDebugMemoryBytes2::GetSize */
};

const CInterfaceStubVtbl _IDebugMemoryBytes2StubVtbl =
{
    &IID_IDebugMemoryBytes2,
    &IDebugMemoryBytes2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDisassemblyStream2, ver. 0.0,
   GUID={0xe5b017fe,0xdfb0,0x411c,{0x82,0x66,0x7c,0x64,0xd6,0xf5,0x19,0xf8}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDisassemblyStream2_FormatStringOffsetTable[] =
    {
    6396,
    6442,
    6488,
    6522,
    6556,
    6584,
    4924,
    6618
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDisassemblyStream2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDisassemblyStream2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDisassemblyStream2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDisassemblyStream2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _IDebugDisassemblyStream2ProxyVtbl = 
{
    &IDebugDisassemblyStream2_ProxyInfo,
    &IID_IDebugDisassemblyStream2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::Read */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::Seek */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::GetCodeLocationId */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::GetCodeContext */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::GetCurrentLocation */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::GetDocument */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::GetScope */ ,
    (void *) (INT_PTR) -1 /* IDebugDisassemblyStream2::GetSize */
};

const CInterfaceStubVtbl _IDebugDisassemblyStream2StubVtbl =
{
    &IID_IDebugDisassemblyStream2,
    &IDebugDisassemblyStream2_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDocumentContext2, ver. 0.0,
   GUID={0x931516ad,0xb600,0x419c,{0x88,0xfc,0xdc,0xf5,0x18,0x3b,0x5f,0xa9}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDocumentContext2_FormatStringOffsetTable[] =
    {
    6646,
    4528,
    6674,
    6702,
    6736,
    6770,
    6804,
    6850
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDocumentContext2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDocumentContext2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(11) _IDebugDocumentContext2ProxyVtbl = 
{
    &IDebugDocumentContext2_ProxyInfo,
    &IID_IDebugDocumentContext2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::GetDocument */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::EnumCodeContexts */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::GetLanguageInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::GetStatementRange */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::GetSourceRange */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::Compare */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentContext2::Seek */
};

const CInterfaceStubVtbl _IDebugDocumentContext2StubVtbl =
{
    &IID_IDebugDocumentContext2,
    &IDebugDocumentContext2_ServerInfo,
    11,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugExpressionContext2, ver. 0.0,
   GUID={0x37a44580,0xd5fc,0x473e,{0xa0,0x48,0x21,0x70,0x2e,0xbf,0xc4,0x66}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugExpressionContext2_FormatStringOffsetTable[] =
    {
    436,
    6884
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugExpressionContext2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugExpressionContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugExpressionContext2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugExpressionContext2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugExpressionContext2ProxyVtbl = 
{
    &IDebugExpressionContext2_ProxyInfo,
    &IID_IDebugExpressionContext2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugExpressionContext2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugExpressionContext2::ParseText */
};

const CInterfaceStubVtbl _IDebugExpressionContext2StubVtbl =
{
    &IID_IDebugExpressionContext2,
    &IDebugExpressionContext2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_msdbg_0359, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugBreakpointRequest2, ver. 0.0,
   GUID={0x6015fd18,0x8257,0x4df3,{0xac,0x42,0xf0,0x74,0xde,0xdd,0x4c,0xbd}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakpointRequest2_FormatStringOffsetTable[] =
    {
    2832,
    6942
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointRequest2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointRequest2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakpointRequest2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointRequest2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugBreakpointRequest2ProxyVtbl = 
{
    &IDebugBreakpointRequest2_ProxyInfo,
    &IID_IDebugBreakpointRequest2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointRequest2::GetLocationType */ ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointRequest2::GetRequestInfo */
};

const CInterfaceStubVtbl _IDebugBreakpointRequest2StubVtbl =
{
    &IID_IDebugBreakpointRequest2,
    &IDebugBreakpointRequest2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_msdbg_0360, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugBreakpointResolution2, ver. 0.0,
   GUID={0xb7e66f28,0x035a,0x401a,{0xaf,0xc7,0x2e,0x30,0x0b,0xd2,0x97,0x11}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBreakpointResolution2_FormatStringOffsetTable[] =
    {
    2832,
    6976
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBreakpointResolution2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointResolution2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBreakpointResolution2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBreakpointResolution2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugBreakpointResolution2ProxyVtbl = 
{
    &IDebugBreakpointResolution2_ProxyInfo,
    &IID_IDebugBreakpointResolution2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointResolution2::GetBreakpointType */ ,
    (void *) (INT_PTR) -1 /* IDebugBreakpointResolution2::GetResolutionInfo */
};

const CInterfaceStubVtbl _IDebugBreakpointResolution2StubVtbl =
{
    &IID_IDebugBreakpointResolution2,
    &IDebugBreakpointResolution2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Standard interface: __MIDL_itf_msdbg_0361, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}} */


/* Object interface: IDebugErrorBreakpointResolution2, ver. 0.0,
   GUID={0x603aedf8,0x9575,0x4d30,{0xb8,0xca,0x12,0x4d,0x1c,0x98,0xeb,0xd8}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugErrorBreakpointResolution2_FormatStringOffsetTable[] =
    {
    2832,
    7010
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugErrorBreakpointResolution2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugErrorBreakpointResolution2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugErrorBreakpointResolution2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugErrorBreakpointResolution2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugErrorBreakpointResolution2ProxyVtbl = 
{
    &IDebugErrorBreakpointResolution2_ProxyInfo,
    &IID_IDebugErrorBreakpointResolution2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugErrorBreakpointResolution2::GetBreakpointType */ ,
    (void *) (INT_PTR) -1 /* IDebugErrorBreakpointResolution2::GetResolutionInfo */
};

const CInterfaceStubVtbl _IDebugErrorBreakpointResolution2StubVtbl =
{
    &IID_IDebugErrorBreakpointResolution2,
    &IDebugErrorBreakpointResolution2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugBoundBreakpoint2, ver. 0.0,
   GUID={0xd533d975,0x3f32,0x4876,{0xab,0xd0,0x6d,0x37,0xfd,0xa5,0x63,0xe7}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugBoundBreakpoint2_FormatStringOffsetTable[] =
    {
    7044,
    3202,
    7072,
    7100,
    7128,
    1418,
    7156,
    7184,
    7212
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugBoundBreakpoint2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugBoundBreakpoint2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugBoundBreakpoint2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugBoundBreakpoint2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(12) _IDebugBoundBreakpoint2ProxyVtbl = 
{
    &IDebugBoundBreakpoint2_ProxyInfo,
    &IID_IDebugBoundBreakpoint2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::GetPendingBreakpoint */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::GetState */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::GetHitCount */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::GetBreakpointResolution */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::Enable */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::SetHitCount */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::SetCondition */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::SetPassCount */ ,
    (void *) (INT_PTR) -1 /* IDebugBoundBreakpoint2::Delete */
};

const CInterfaceStubVtbl _IDebugBoundBreakpoint2StubVtbl =
{
    &IID_IDebugBoundBreakpoint2,
    &IDebugBoundBreakpoint2_ServerInfo,
    12,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugPendingBreakpoint2, ver. 0.0,
   GUID={0x6e215ef3,0xe44c,0x44d1,{0xb7,0xba,0xb2,0x40,0x1f,0x7d,0xc2,0x3d}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugPendingBreakpoint2_FormatStringOffsetTable[] =
    {
    7234,
    7262,
    7284,
    7312,
    7128,
    1418,
    7156,
    7184,
    7340,
    7368,
    7402
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugPendingBreakpoint2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugPendingBreakpoint2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugPendingBreakpoint2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugPendingBreakpoint2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IDebugPendingBreakpoint2ProxyVtbl = 
{
    &IDebugPendingBreakpoint2_ProxyInfo,
    &IID_IDebugPendingBreakpoint2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::CanBind */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::Bind */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::GetState */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::GetBreakpointRequest */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::Virtualize */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::Enable */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::SetCondition */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::SetPassCount */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::EnumBoundBreakpoints */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::EnumErrorBreakpoints */ ,
    (void *) (INT_PTR) -1 /* IDebugPendingBreakpoint2::Delete */
};

const CInterfaceStubVtbl _IDebugPendingBreakpoint2StubVtbl =
{
    &IID_IDebugPendingBreakpoint2,
    &IDebugPendingBreakpoint2_ServerInfo,
    14,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugErrorBreakpoint2, ver. 0.0,
   GUID={0x74570ef7,0x2486,0x4089,{0x80,0x0c,0x56,0xe3,0x82,0x9b,0x5c,0xa4}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugErrorBreakpoint2_FormatStringOffsetTable[] =
    {
    7424,
    7452
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugErrorBreakpoint2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugErrorBreakpoint2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugErrorBreakpoint2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugErrorBreakpoint2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugErrorBreakpoint2ProxyVtbl = 
{
    &IDebugErrorBreakpoint2_ProxyInfo,
    &IID_IDebugErrorBreakpoint2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugErrorBreakpoint2::GetPendingBreakpoint */ ,
    (void *) (INT_PTR) -1 /* IDebugErrorBreakpoint2::GetBreakpointResolution */
};

const CInterfaceStubVtbl _IDebugErrorBreakpoint2StubVtbl =
{
    &IID_IDebugErrorBreakpoint2,
    &IDebugErrorBreakpoint2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugExpression2, ver. 0.0,
   GUID={0xf7473fd0,0x7f75,0x478d,{0x8d,0x85,0xa4,0x85,0x20,0x4e,0x7a,0x2d}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugExpression2_FormatStringOffsetTable[] =
    {
    7480,
    7262,
    7514
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugExpression2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugExpression2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugExpression2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugExpression2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(6) _IDebugExpression2ProxyVtbl = 
{
    &IDebugExpression2_ProxyInfo,
    &IID_IDebugExpression2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugExpression2::EvaluateAsync */ ,
    (void *) (INT_PTR) -1 /* IDebugExpression2::Abort */ ,
    (void *) (INT_PTR) -1 /* IDebugExpression2::EvaluateSync */
};

const CInterfaceStubVtbl _IDebugExpression2StubVtbl =
{
    &IID_IDebugExpression2,
    &IDebugExpression2_ServerInfo,
    6,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugModule2, ver. 0.0,
   GUID={0x0fc1cd9a,0xb912,0x405c,{0xa0,0x4c,0x43,0xce,0x02,0xcd,0x7d,0xf2}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugModule2_FormatStringOffsetTable[] =
    {
    7560,
    7594
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugModule2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugModule2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugModule2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugModule2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugModule2ProxyVtbl = 
{
    &IDebugModule2_ProxyInfo,
    &IID_IDebugModule2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugModule2::GetInfo */ ,
    (void *) (INT_PTR) -1 /* IDebugModule2::ReloadSymbols */
};

const CInterfaceStubVtbl _IDebugModule2StubVtbl =
{
    &IID_IDebugModule2,
    &IDebugModule2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugModuleManaged, ver. 0.0,
   GUID={0x232397F8,0xB232,0x479d,{0xB1,0xBB,0x2F,0x04,0x4C,0x70,0xA0,0xF9}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugModuleManaged_FormatStringOffsetTable[] =
    {
    7628
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugModuleManaged_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugModuleManaged_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugModuleManaged_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugModuleManaged_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugModuleManagedProxyVtbl = 
{
    &IDebugModuleManaged_ProxyInfo,
    &IID_IDebugModuleManaged,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugModuleManaged::GetMvid */
};

const CInterfaceStubVtbl _IDebugModuleManagedStubVtbl =
{
    &IID_IDebugModuleManaged,
    &IDebugModuleManaged_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDocument2, ver. 0.0,
   GUID={0x1606dd73,0x5d5f,0x405c,{0xb4,0xf4,0xce,0x32,0xba,0xba,0x25,0x01}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDocument2_FormatStringOffsetTable[] =
    {
    4438,
    464
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDocument2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDocument2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDocument2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDocument2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugDocument2ProxyVtbl = 
{
    &IDebugDocument2_ProxyInfo,
    &IID_IDebugDocument2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDocument2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugDocument2::GetDocumentClassId */
};

const CInterfaceStubVtbl _IDebugDocument2StubVtbl =
{
    &IID_IDebugDocument2,
    &IDebugDocument2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDocumentText2, ver. 0.0,
   GUID={0x4b0645aa,0x08ef,0x4cb9,{0xad,0xb9,0x03,0x95,0xd6,0xed,0xad,0x35}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDocumentText2_FormatStringOffsetTable[] =
    {
    4438,
    464,
    7656,
    7690
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDocumentText2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentText2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDocumentText2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentText2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDebugDocumentText2ProxyVtbl = 
{
    &IDebugDocumentText2_ProxyInfo,
    &IID_IDebugDocumentText2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDocument2::GetName */ ,
    (void *) (INT_PTR) -1 /* IDebugDocument2::GetDocumentClassId */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentText2::GetSize */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentText2::GetText */
};

const CInterfaceStubVtbl _IDebugDocumentText2StubVtbl =
{
    &IID_IDebugDocumentText2,
    &IDebugDocumentText2_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDocumentPosition2, ver. 0.0,
   GUID={0xbdde0eee,0x3b8d,0x4c82,{0xb5,0x29,0x33,0xf1,0x6b,0x42,0x83,0x2e}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDocumentPosition2_FormatStringOffsetTable[] =
    {
    436,
    7736,
    7764,
    7792
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDocumentPosition2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentPosition2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDocumentPosition2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentPosition2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(7) _IDebugDocumentPosition2ProxyVtbl = 
{
    &IDebugDocumentPosition2_ProxyInfo,
    &IID_IDebugDocumentPosition2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDocumentPosition2::GetFileName */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentPosition2::GetDocument */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentPosition2::IsPositionInDocument */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentPosition2::GetRange */
};

const CInterfaceStubVtbl _IDebugDocumentPosition2StubVtbl =
{
    &IID_IDebugDocumentPosition2,
    &IDebugDocumentPosition2_ServerInfo,
    7,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDocumentPositionOffset2, ver. 0.0,
   GUID={0x037edd0f,0x8551,0x4f7f,{0x8c,0xa0,0x04,0xd9,0xe2,0x9f,0x53,0x2d}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDocumentPositionOffset2_FormatStringOffsetTable[] =
    {
    7826
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDocumentPositionOffset2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentPositionOffset2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDocumentPositionOffset2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentPositionOffset2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugDocumentPositionOffset2ProxyVtbl = 
{
    &IDebugDocumentPositionOffset2_ProxyInfo,
    &IID_IDebugDocumentPositionOffset2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDocumentPositionOffset2::GetRange */
};

const CInterfaceStubVtbl _IDebugDocumentPositionOffset2StubVtbl =
{
    &IID_IDebugDocumentPositionOffset2,
    &IDebugDocumentPositionOffset2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugFunctionPosition2, ver. 0.0,
   GUID={0x1ede3b4b,0x35e7,0x4b97,{0x81,0x33,0x02,0x84,0x5d,0x60,0x01,0x74}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugFunctionPosition2_FormatStringOffsetTable[] =
    {
    436,
    7860
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugFunctionPosition2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugFunctionPosition2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugFunctionPosition2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugFunctionPosition2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(5) _IDebugFunctionPosition2ProxyVtbl = 
{
    &IDebugFunctionPosition2_ProxyInfo,
    &IID_IDebugFunctionPosition2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugFunctionPosition2::GetFunctionName */ ,
    (void *) (INT_PTR) -1 /* IDebugFunctionPosition2::GetOffset */
};

const CInterfaceStubVtbl _IDebugFunctionPosition2StubVtbl =
{
    &IID_IDebugFunctionPosition2,
    &IDebugFunctionPosition2_ServerInfo,
    5,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugDocumentTextEvents2, ver. 0.0,
   GUID={0x33ec72e3,0x002f,0x4966,{0xb9,0x1c,0x5c,0xe2,0xf7,0xba,0x51,0x24}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugDocumentTextEvents2_FormatStringOffsetTable[] =
    {
    4318,
    7888,
    7922,
    7956,
    7990,
    1418
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugDocumentTextEvents2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentTextEvents2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugDocumentTextEvents2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugDocumentTextEvents2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IDebugDocumentTextEvents2ProxyVtbl = 
{
    &IDebugDocumentTextEvents2_ProxyInfo,
    &IID_IDebugDocumentTextEvents2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugDocumentTextEvents2::onDestroy */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentTextEvents2::onInsertText */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentTextEvents2::onRemoveText */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentTextEvents2::onReplaceText */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentTextEvents2::onUpdateTextAttributes */ ,
    (void *) (INT_PTR) -1 /* IDebugDocumentTextEvents2::onUpdateDocumentAttributes */
};

const CInterfaceStubVtbl _IDebugDocumentTextEvents2StubVtbl =
{
    &IID_IDebugDocumentTextEvents2,
    &IDebugDocumentTextEvents2_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IDebugQueryEngine2, ver. 0.0,
   GUID={0xc989adc9,0xf305,0x4ef5,{0x8c,0xa2,0x20,0x89,0x8e,0x8d,0x0e,0x28}} */

#pragma code_seg(".orpc")
static const unsigned short IDebugQueryEngine2_FormatStringOffsetTable[] =
    {
    8024
    };

static const MIDL_STUBLESS_PROXY_INFO IDebugQueryEngine2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IDebugQueryEngine2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IDebugQueryEngine2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IDebugQueryEngine2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(4) _IDebugQueryEngine2ProxyVtbl = 
{
    &IDebugQueryEngine2_ProxyInfo,
    &IID_IDebugQueryEngine2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IDebugQueryEngine2::GetEngineInterface */
};

const CInterfaceStubVtbl _IDebugQueryEngine2StubVtbl =
{
    &IID_IDebugQueryEngine2,
    &IDebugQueryEngine2_ServerInfo,
    4,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugProcesses2, ver. 0.0,
   GUID={0x96c74ef4,0x185d,0x4f9a,{0x8a,0x43,0x4d,0x27,0x23,0x75,0x8e,0x0a}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugProcesses2_FormatStringOffsetTable[] =
    {
    8052,
    260,
    3000,
    8092,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugProcesses2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugProcesses2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugProcesses2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugProcesses2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugProcesses2ProxyVtbl = 
{
    &IEnumDebugProcesses2_ProxyInfo,
    &IID_IEnumDebugProcesses2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugProcesses2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugProcesses2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugProcesses2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugProcesses2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugProcesses2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugProcesses2StubVtbl =
{
    &IID_IEnumDebugProcesses2,
    &IEnumDebugProcesses2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugPrograms2, ver. 0.0,
   GUID={0x8d14bca6,0x34ce,0x4efe,{0xac,0x7e,0x0a,0xbc,0x61,0xda,0xdb,0x20}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugPrograms2_FormatStringOffsetTable[] =
    {
    8148,
    260,
    3000,
    8188,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugPrograms2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPrograms2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugPrograms2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPrograms2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugPrograms2ProxyVtbl = 
{
    &IEnumDebugPrograms2_ProxyInfo,
    &IID_IEnumDebugPrograms2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugPrograms2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPrograms2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPrograms2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPrograms2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPrograms2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugPrograms2StubVtbl =
{
    &IID_IEnumDebugPrograms2,
    &IEnumDebugPrograms2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugThreads2, ver. 0.0,
   GUID={0x0d30dc12,0xc4f8,0x433d,{0x9f,0xcc,0x9f,0xf1,0x17,0xe5,0xe5,0xf4}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugThreads2_FormatStringOffsetTable[] =
    {
    8216,
    260,
    3000,
    8256,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugThreads2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugThreads2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugThreads2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugThreads2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugThreads2ProxyVtbl = 
{
    &IEnumDebugThreads2_ProxyInfo,
    &IID_IEnumDebugThreads2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugThreads2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugThreads2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugThreads2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugThreads2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugThreads2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugThreads2StubVtbl =
{
    &IID_IEnumDebugThreads2,
    &IEnumDebugThreads2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugStackFrames2, ver. 0.0,
   GUID={0xcd39102b,0x4b69,0x4495,{0x8f,0x29,0xe0,0xb2,0x5c,0x4a,0x88,0x55}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugStackFrames2_FormatStringOffsetTable[] =
    {
    8284,
    260,
    3000,
    8324,
    8120,
    8352
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugStackFrames2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugStackFrames2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugStackFrames2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugStackFrames2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IEnumDebugStackFrames2ProxyVtbl = 
{
    &IEnumDebugStackFrames2_ProxyInfo,
    &IID_IEnumDebugStackFrames2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugStackFrames2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugStackFrames2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugStackFrames2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugStackFrames2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugStackFrames2::GetCount */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugStackFrames2::GetIndex */
};

const CInterfaceStubVtbl _IEnumDebugStackFrames2StubVtbl =
{
    &IID_IEnumDebugStackFrames2,
    &IEnumDebugStackFrames2_ServerInfo,
    9,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugCodeContexts2, ver. 0.0,
   GUID={0xad47a80b,0xeda7,0x459e,{0xaf,0x82,0x64,0x7c,0xc9,0xfb,0xaa,0x50}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugCodeContexts2_FormatStringOffsetTable[] =
    {
    8386,
    260,
    3000,
    8426,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugCodeContexts2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugCodeContexts2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugCodeContexts2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugCodeContexts2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugCodeContexts2ProxyVtbl = 
{
    &IEnumDebugCodeContexts2_ProxyInfo,
    &IID_IEnumDebugCodeContexts2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugCodeContexts2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugCodeContexts2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugCodeContexts2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugCodeContexts2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugCodeContexts2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugCodeContexts2StubVtbl =
{
    &IID_IEnumDebugCodeContexts2,
    &IEnumDebugCodeContexts2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugBoundBreakpoints2, ver. 0.0,
   GUID={0x0f6b37e0,0xfcfe,0x44d9,{0x91,0x12,0x39,0x4c,0xa9,0xb9,0x21,0x14}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugBoundBreakpoints2_FormatStringOffsetTable[] =
    {
    8454,
    260,
    3000,
    8494,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugBoundBreakpoints2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugBoundBreakpoints2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugBoundBreakpoints2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugBoundBreakpoints2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugBoundBreakpoints2ProxyVtbl = 
{
    &IEnumDebugBoundBreakpoints2_ProxyInfo,
    &IID_IEnumDebugBoundBreakpoints2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugBoundBreakpoints2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugBoundBreakpoints2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugBoundBreakpoints2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugBoundBreakpoints2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugBoundBreakpoints2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugBoundBreakpoints2StubVtbl =
{
    &IID_IEnumDebugBoundBreakpoints2,
    &IEnumDebugBoundBreakpoints2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugPendingBreakpoints2, ver. 0.0,
   GUID={0x70d2dc1e,0x4dcc,0x4786,{0xa0,0x72,0x9a,0x3b,0x60,0x0c,0x21,0x6b}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugPendingBreakpoints2_FormatStringOffsetTable[] =
    {
    8522,
    260,
    3000,
    8562,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugPendingBreakpoints2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPendingBreakpoints2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugPendingBreakpoints2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPendingBreakpoints2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugPendingBreakpoints2ProxyVtbl = 
{
    &IEnumDebugPendingBreakpoints2_ProxyInfo,
    &IID_IEnumDebugPendingBreakpoints2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugPendingBreakpoints2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPendingBreakpoints2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPendingBreakpoints2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPendingBreakpoints2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPendingBreakpoints2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugPendingBreakpoints2StubVtbl =
{
    &IID_IEnumDebugPendingBreakpoints2,
    &IEnumDebugPendingBreakpoints2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugErrorBreakpoints2, ver. 0.0,
   GUID={0xe158f5aa,0x31fe,0x491b,{0xa9,0xf6,0xcf,0xf9,0x34,0xb0,0x3a,0x01}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugErrorBreakpoints2_FormatStringOffsetTable[] =
    {
    8590,
    260,
    3000,
    8630,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugErrorBreakpoints2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugErrorBreakpoints2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugErrorBreakpoints2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugErrorBreakpoints2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugErrorBreakpoints2ProxyVtbl = 
{
    &IEnumDebugErrorBreakpoints2_ProxyInfo,
    &IID_IEnumDebugErrorBreakpoints2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorBreakpoints2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorBreakpoints2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorBreakpoints2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorBreakpoints2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugErrorBreakpoints2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugErrorBreakpoints2StubVtbl =
{
    &IID_IEnumDebugErrorBreakpoints2,
    &IEnumDebugErrorBreakpoints2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugMachines2, ver. 0.0,
   GUID={0x61d986ec,0x1eac,0x46b6,{0x90,0xff,0x40,0x2a,0x00,0x8f,0x15,0xd1}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugMachines2_FormatStringOffsetTable[] =
    {
    8658,
    260,
    3000,
    8698,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugMachines2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugMachines2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugMachines2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugMachines2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugMachines2ProxyVtbl = 
{
    &IEnumDebugMachines2_ProxyInfo,
    &IID_IEnumDebugMachines2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugMachines2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugMachines2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugMachines2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugMachines2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugMachines2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugMachines2StubVtbl =
{
    &IID_IEnumDebugMachines2,
    &IEnumDebugMachines2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugExceptionInfo2, ver. 0.0,
   GUID={0x8e4bbd34,0xa2f4,0x41ef,{0x87,0xb5,0xc5,0x63,0xb4,0xad,0x6e,0xe7}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugExceptionInfo2_FormatStringOffsetTable[] =
    {
    8726,
    260,
    3000,
    8766,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugExceptionInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugExceptionInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugExceptionInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugExceptionInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugExceptionInfo2ProxyVtbl = 
{
    &IEnumDebugExceptionInfo2_ProxyInfo,
    &IID_IEnumDebugExceptionInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugExceptionInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugExceptionInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugExceptionInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugExceptionInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugExceptionInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugExceptionInfo2StubVtbl =
{
    &IID_IEnumDebugExceptionInfo2,
    &IEnumDebugExceptionInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugFrameInfo2, ver. 0.0,
   GUID={0x98bbba48,0x4c4d,0x4fff,{0x83,0x40,0x60,0x97,0xbe,0xc9,0xc8,0x94}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugFrameInfo2_FormatStringOffsetTable[] =
    {
    8794,
    260,
    3000,
    8834,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugFrameInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugFrameInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugFrameInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugFrameInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugFrameInfo2ProxyVtbl = 
{
    &IEnumDebugFrameInfo2_ProxyInfo,
    &IID_IEnumDebugFrameInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugFrameInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFrameInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFrameInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFrameInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugFrameInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugFrameInfo2StubVtbl =
{
    &IID_IEnumDebugFrameInfo2,
    &IEnumDebugFrameInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumCodePaths2, ver. 0.0,
   GUID={0x9b13f80d,0xcfc6,0x4b78,{0x81,0xef,0x1f,0x7c,0xc3,0x3f,0x76,0x39}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumCodePaths2_FormatStringOffsetTable[] =
    {
    8862,
    260,
    3000,
    8902,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumCodePaths2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumCodePaths2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumCodePaths2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumCodePaths2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumCodePaths2ProxyVtbl = 
{
    &IEnumCodePaths2_ProxyInfo,
    &IID_IEnumCodePaths2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumCodePaths2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumCodePaths2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumCodePaths2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumCodePaths2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumCodePaths2::GetCount */
};

const CInterfaceStubVtbl _IEnumCodePaths2StubVtbl =
{
    &IID_IEnumCodePaths2,
    &IEnumCodePaths2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugModules2, ver. 0.0,
   GUID={0x4c4a2835,0x682e,0x4ce1,{0xae,0xbc,0x1e,0x6b,0x3a,0x16,0x5b,0x44}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugModules2_FormatStringOffsetTable[] =
    {
    8930,
    260,
    3000,
    8970,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugModules2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugModules2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugModules2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugModules2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugModules2ProxyVtbl = 
{
    &IEnumDebugModules2_ProxyInfo,
    &IID_IEnumDebugModules2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugModules2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugModules2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugModules2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugModules2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugModules2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugModules2StubVtbl =
{
    &IID_IEnumDebugModules2,
    &IEnumDebugModules2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugPortSuppliers2, ver. 0.0,
   GUID={0x59c9dc99,0x3eff,0x4ff3,{0xb2,0x01,0x98,0xac,0xd0,0x1b,0x0d,0x87}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugPortSuppliers2_FormatStringOffsetTable[] =
    {
    8998,
    260,
    3000,
    9038,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugPortSuppliers2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPortSuppliers2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugPortSuppliers2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPortSuppliers2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugPortSuppliers2ProxyVtbl = 
{
    &IEnumDebugPortSuppliers2_ProxyInfo,
    &IID_IEnumDebugPortSuppliers2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugPortSuppliers2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPortSuppliers2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPortSuppliers2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPortSuppliers2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPortSuppliers2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugPortSuppliers2StubVtbl =
{
    &IID_IEnumDebugPortSuppliers2,
    &IEnumDebugPortSuppliers2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugPorts2, ver. 0.0,
   GUID={0xbc827c5e,0x99ae,0x4ac8,{0x83,0xad,0x2e,0xa5,0xc2,0x03,0x43,0x33}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugPorts2_FormatStringOffsetTable[] =
    {
    9066,
    260,
    3000,
    9106,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugPorts2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPorts2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugPorts2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPorts2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugPorts2ProxyVtbl = 
{
    &IEnumDebugPorts2_ProxyInfo,
    &IID_IEnumDebugPorts2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugPorts2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPorts2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPorts2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPorts2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPorts2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugPorts2StubVtbl =
{
    &IID_IEnumDebugPorts2,
    &IEnumDebugPorts2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugPropertyInfo2, ver. 0.0,
   GUID={0x6c7072c3,0x3ac4,0x408f,{0xa6,0x80,0xfc,0x5a,0x2f,0x96,0x90,0x3e}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugPropertyInfo2_FormatStringOffsetTable[] =
    {
    9134,
    260,
    3000,
    9174,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugPropertyInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPropertyInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugPropertyInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugPropertyInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugPropertyInfo2ProxyVtbl = 
{
    &IEnumDebugPropertyInfo2_ProxyInfo,
    &IID_IEnumDebugPropertyInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugPropertyInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPropertyInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPropertyInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPropertyInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugPropertyInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugPropertyInfo2StubVtbl =
{
    &IID_IEnumDebugPropertyInfo2,
    &IEnumDebugPropertyInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};


/* Object interface: IEnumDebugReferenceInfo2, ver. 0.0,
   GUID={0xe459dd12,0x864f,0x4aaa,{0xab,0xc1,0xdc,0xec,0xbc,0x26,0x7f,0x04}} */

#pragma code_seg(".orpc")
static const unsigned short IEnumDebugReferenceInfo2_FormatStringOffsetTable[] =
    {
    9202,
    260,
    3000,
    9242,
    8120
    };

static const MIDL_STUBLESS_PROXY_INFO IEnumDebugReferenceInfo2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugReferenceInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEnumDebugReferenceInfo2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEnumDebugReferenceInfo2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(8) _IEnumDebugReferenceInfo2ProxyVtbl = 
{
    &IEnumDebugReferenceInfo2_ProxyInfo,
    &IID_IEnumDebugReferenceInfo2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    (void *) (INT_PTR) -1 /* IEnumDebugReferenceInfo2::Next */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugReferenceInfo2::Skip */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugReferenceInfo2::Reset */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugReferenceInfo2::Clone */ ,
    (void *) (INT_PTR) -1 /* IEnumDebugReferenceInfo2::GetCount */
};

const CInterfaceStubVtbl _IEnumDebugReferenceInfo2StubVtbl =
{
    &IID_IEnumDebugReferenceInfo2,
    &IEnumDebugReferenceInfo2_ServerInfo,
    8,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x600015b, /* MIDL Version 6.0.347 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _msdbg_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IDebugBreakpointBoundEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugCodeContexts2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumCodePaths2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugSession2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProcessCreateEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDocumentPositionOffset2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugThreads2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugReferenceInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugExceptionEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPropertyCreateEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPropertyDestroyEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBreakpointRequest2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBreakEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugPendingBreakpoints2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBreakpointEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBreakpointResolution2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugStackFrames2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEngineLaunch2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMachineEx2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProcessDestroyEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPortNotify2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugThreadDestroyEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugExceptionInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugModules2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugFindSymbolEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugActivateDocumentEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEntryPointEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBreakpointErrorEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMDMUtil2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugCanStopEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugFrameInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugNoSymbolsEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugFunctionPosition2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEngineCreateEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBreakpointUnboundEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugLoadCompleteEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugThread2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEngine2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugLogicalThread2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgramNode2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugPorts2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEngineProgram2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgramEngines2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugCodeContext2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugStackFrame2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDocument2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPortEvents2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugBoundBreakpoint2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugThreadNameChangedEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProperty2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugExpressionContext2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugModuleLoadEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugExpressionEvaluationCompleteEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgramHost2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPortSupplier2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugPortSuppliers2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugModule2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMachine2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugPrograms2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDocumentText2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugErrorBreakpoints2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugReference2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDocumentContext2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugOutputStringEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugSessionCreateEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPortRequest2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEventCallback2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugStepCompleteEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugSessionDestroyEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugPropertyInfo2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPort2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugQueryEngine2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugReturnValueEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMessageEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugExpression2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMemoryBytes2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugMemoryContext2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugBoundBreakpoints2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDocumentTextEvents2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgramDestroyEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProcess2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugMachines2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDocumentPosition2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgramCreateEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgram2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugPendingBreakpoint2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEnumDebugProcesses2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugErrorBreakpoint2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugModuleManagedProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugErrorBreakpointResolution2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugThreadCreateEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugProgramNameChangedEvent2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IDebugDisassemblyStream2ProxyVtbl,
    0
};

const CInterfaceStubVtbl * _msdbg_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IDebugBreakpointBoundEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugCodeContexts2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumCodePaths2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugSession2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProcessCreateEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDocumentPositionOffset2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugThreads2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugReferenceInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugExceptionEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPropertyCreateEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPropertyDestroyEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBreakpointRequest2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBreakEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugPendingBreakpoints2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBreakpointEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBreakpointResolution2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugStackFrames2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEngineLaunch2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMachineEx2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProcessDestroyEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPortNotify2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugThreadDestroyEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugExceptionInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugModules2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugFindSymbolEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugActivateDocumentEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEntryPointEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBreakpointErrorEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMDMUtil2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugCanStopEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugFrameInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugNoSymbolsEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugFunctionPosition2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEngineCreateEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBreakpointUnboundEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugLoadCompleteEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugThread2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEngine2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugLogicalThread2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgramNode2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugPorts2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEngineProgram2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgramEngines2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugCodeContext2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugStackFrame2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDocument2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPortEvents2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugBoundBreakpoint2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugThreadNameChangedEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProperty2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugExpressionContext2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugModuleLoadEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugExpressionEvaluationCompleteEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgramHost2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPortSupplier2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugPortSuppliers2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugModule2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMachine2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugPrograms2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDocumentText2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugErrorBreakpoints2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugReference2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDocumentContext2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugOutputStringEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugSessionCreateEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPortRequest2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEventCallback2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugStepCompleteEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugSessionDestroyEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugPropertyInfo2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPort2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugQueryEngine2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugReturnValueEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMessageEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugExpression2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMemoryBytes2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugMemoryContext2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugBoundBreakpoints2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDocumentTextEvents2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgramDestroyEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProcess2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugMachines2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDocumentPosition2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgramCreateEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgram2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugPendingBreakpoint2StubVtbl,
    ( CInterfaceStubVtbl *) &_IEnumDebugProcesses2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugErrorBreakpoint2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugModuleManagedStubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugErrorBreakpointResolution2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugThreadCreateEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugProgramNameChangedEvent2StubVtbl,
    ( CInterfaceStubVtbl *) &_IDebugDisassemblyStream2StubVtbl,
    0
};

PCInterfaceName const _msdbg_InterfaceNamesList[] = 
{
    "IDebugBreakpointBoundEvent2",
    "IEnumDebugCodeContexts2",
    "IEnumCodePaths2",
    "IDebugSession2",
    "IDebugProcessCreateEvent2",
    "IDebugDocumentPositionOffset2",
    "IEnumDebugThreads2",
    "IEnumDebugReferenceInfo2",
    "IDebugExceptionEvent2",
    "IDebugPropertyCreateEvent2",
    "IDebugPropertyDestroyEvent2",
    "IDebugBreakpointRequest2",
    "IDebugBreakEvent2",
    "IEnumDebugPendingBreakpoints2",
    "IDebugBreakpointEvent2",
    "IDebugBreakpointResolution2",
    "IEnumDebugStackFrames2",
    "IDebugEngineLaunch2",
    "IDebugMachineEx2",
    "IDebugProcessDestroyEvent2",
    "IDebugPortNotify2",
    "IDebugThreadDestroyEvent2",
    "IEnumDebugExceptionInfo2",
    "IEnumDebugModules2",
    "IDebugFindSymbolEvent2",
    "IDebugActivateDocumentEvent2",
    "IDebugEntryPointEvent2",
    "IDebugBreakpointErrorEvent2",
    "IDebugMDMUtil2",
    "IDebugCanStopEvent2",
    "IEnumDebugFrameInfo2",
    "IDebugNoSymbolsEvent2",
    "IDebugFunctionPosition2",
    "IDebugEngineCreateEvent2",
    "IDebugBreakpointUnboundEvent2",
    "IDebugLoadCompleteEvent2",
    "IDebugThread2",
    "IDebugEngine2",
    "IDebugLogicalThread2",
    "IDebugProgramNode2",
    "IEnumDebugPorts2",
    "IDebugEngineProgram2",
    "IDebugProgramEngines2",
    "IDebugCodeContext2",
    "IDebugStackFrame2",
    "IDebugDocument2",
    "IDebugPortEvents2",
    "IDebugBoundBreakpoint2",
    "IDebugThreadNameChangedEvent2",
    "IDebugProperty2",
    "IDebugExpressionContext2",
    "IDebugModuleLoadEvent2",
    "IDebugExpressionEvaluationCompleteEvent2",
    "IDebugProgramHost2",
    "IDebugPortSupplier2",
    "IEnumDebugPortSuppliers2",
    "IDebugModule2",
    "IDebugMachine2",
    "IEnumDebugPrograms2",
    "IDebugDocumentText2",
    "IEnumDebugErrorBreakpoints2",
    "IDebugReference2",
    "IDebugDocumentContext2",
    "IDebugOutputStringEvent2",
    "IDebugSessionCreateEvent2",
    "IDebugPortRequest2",
    "IDebugEventCallback2",
    "IDebugStepCompleteEvent2",
    "IDebugSessionDestroyEvent2",
    "IEnumDebugPropertyInfo2",
    "IDebugPort2",
    "IDebugQueryEngine2",
    "IDebugReturnValueEvent2",
    "IDebugMessageEvent2",
    "IDebugExpression2",
    "IDebugMemoryBytes2",
    "IDebugEvent2",
    "IDebugMemoryContext2",
    "IEnumDebugBoundBreakpoints2",
    "IDebugDocumentTextEvents2",
    "IDebugProgramDestroyEvent2",
    "IDebugProcess2",
    "IEnumDebugMachines2",
    "IDebugDocumentPosition2",
    "IDebugProgramCreateEvent2",
    "IDebugProgram2",
    "IDebugPendingBreakpoint2",
    "IEnumDebugProcesses2",
    "IDebugErrorBreakpoint2",
    "IDebugModuleManaged",
    "IDebugErrorBreakpointResolution2",
    "IDebugThreadCreateEvent2",
    "IDebugProgramNameChangedEvent2",
    "IDebugDisassemblyStream2",
    0
};


#define _msdbg_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _msdbg, pIID, n)

int __stdcall _msdbg_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _msdbg, 94, 64 )
    IID_BS_LOOKUP_NEXT_TEST( _msdbg, 32 )
    IID_BS_LOOKUP_NEXT_TEST( _msdbg, 16 )
    IID_BS_LOOKUP_NEXT_TEST( _msdbg, 8 )
    IID_BS_LOOKUP_NEXT_TEST( _msdbg, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _msdbg, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _msdbg, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _msdbg, 94, *pIndex )
    
}

const ExtendedProxyFileInfo msdbg_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _msdbg_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _msdbg_StubVtblList,
    (const PCInterfaceName * ) & _msdbg_InterfaceNamesList,
    0, // no delegation
    & _msdbg_IID_Lookup, 
    94,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

