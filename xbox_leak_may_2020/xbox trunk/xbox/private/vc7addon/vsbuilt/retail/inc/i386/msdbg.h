
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sat Jan 05 01:51:28 2002
 */
/* Compiler settings for msdbg.idl:
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

#ifndef __msdbg_h__
#define __msdbg_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDebugMachine2_FWD_DEFINED__
#define __IDebugMachine2_FWD_DEFINED__
typedef interface IDebugMachine2 IDebugMachine2;
#endif 	/* __IDebugMachine2_FWD_DEFINED__ */


#ifndef __IDebugMachineEx2_FWD_DEFINED__
#define __IDebugMachineEx2_FWD_DEFINED__
typedef interface IDebugMachineEx2 IDebugMachineEx2;
#endif 	/* __IDebugMachineEx2_FWD_DEFINED__ */


#ifndef __IDebugPortSupplier2_FWD_DEFINED__
#define __IDebugPortSupplier2_FWD_DEFINED__
typedef interface IDebugPortSupplier2 IDebugPortSupplier2;
#endif 	/* __IDebugPortSupplier2_FWD_DEFINED__ */


#ifndef __IDebugPort2_FWD_DEFINED__
#define __IDebugPort2_FWD_DEFINED__
typedef interface IDebugPort2 IDebugPort2;
#endif 	/* __IDebugPort2_FWD_DEFINED__ */


#ifndef __IDebugPortRequest2_FWD_DEFINED__
#define __IDebugPortRequest2_FWD_DEFINED__
typedef interface IDebugPortRequest2 IDebugPortRequest2;
#endif 	/* __IDebugPortRequest2_FWD_DEFINED__ */


#ifndef __IDebugPortNotify2_FWD_DEFINED__
#define __IDebugPortNotify2_FWD_DEFINED__
typedef interface IDebugPortNotify2 IDebugPortNotify2;
#endif 	/* __IDebugPortNotify2_FWD_DEFINED__ */


#ifndef __IDebugPortEvents2_FWD_DEFINED__
#define __IDebugPortEvents2_FWD_DEFINED__
typedef interface IDebugPortEvents2 IDebugPortEvents2;
#endif 	/* __IDebugPortEvents2_FWD_DEFINED__ */


#ifndef __IDebugMDMUtil2_FWD_DEFINED__
#define __IDebugMDMUtil2_FWD_DEFINED__
typedef interface IDebugMDMUtil2 IDebugMDMUtil2;
#endif 	/* __IDebugMDMUtil2_FWD_DEFINED__ */


#ifndef __IDebugSession2_FWD_DEFINED__
#define __IDebugSession2_FWD_DEFINED__
typedef interface IDebugSession2 IDebugSession2;
#endif 	/* __IDebugSession2_FWD_DEFINED__ */


#ifndef __IDebugEngine2_FWD_DEFINED__
#define __IDebugEngine2_FWD_DEFINED__
typedef interface IDebugEngine2 IDebugEngine2;
#endif 	/* __IDebugEngine2_FWD_DEFINED__ */


#ifndef __IDebugEngineLaunch2_FWD_DEFINED__
#define __IDebugEngineLaunch2_FWD_DEFINED__
typedef interface IDebugEngineLaunch2 IDebugEngineLaunch2;
#endif 	/* __IDebugEngineLaunch2_FWD_DEFINED__ */


#ifndef __IDebugEventCallback2_FWD_DEFINED__
#define __IDebugEventCallback2_FWD_DEFINED__
typedef interface IDebugEventCallback2 IDebugEventCallback2;
#endif 	/* __IDebugEventCallback2_FWD_DEFINED__ */


#ifndef __IDebugEvent2_FWD_DEFINED__
#define __IDebugEvent2_FWD_DEFINED__
typedef interface IDebugEvent2 IDebugEvent2;
#endif 	/* __IDebugEvent2_FWD_DEFINED__ */


#ifndef __IDebugSessionCreateEvent2_FWD_DEFINED__
#define __IDebugSessionCreateEvent2_FWD_DEFINED__
typedef interface IDebugSessionCreateEvent2 IDebugSessionCreateEvent2;
#endif 	/* __IDebugSessionCreateEvent2_FWD_DEFINED__ */


#ifndef __IDebugSessionDestroyEvent2_FWD_DEFINED__
#define __IDebugSessionDestroyEvent2_FWD_DEFINED__
typedef interface IDebugSessionDestroyEvent2 IDebugSessionDestroyEvent2;
#endif 	/* __IDebugSessionDestroyEvent2_FWD_DEFINED__ */


#ifndef __IDebugEngineCreateEvent2_FWD_DEFINED__
#define __IDebugEngineCreateEvent2_FWD_DEFINED__
typedef interface IDebugEngineCreateEvent2 IDebugEngineCreateEvent2;
#endif 	/* __IDebugEngineCreateEvent2_FWD_DEFINED__ */


#ifndef __IDebugProcessCreateEvent2_FWD_DEFINED__
#define __IDebugProcessCreateEvent2_FWD_DEFINED__
typedef interface IDebugProcessCreateEvent2 IDebugProcessCreateEvent2;
#endif 	/* __IDebugProcessCreateEvent2_FWD_DEFINED__ */


#ifndef __IDebugProcessDestroyEvent2_FWD_DEFINED__
#define __IDebugProcessDestroyEvent2_FWD_DEFINED__
typedef interface IDebugProcessDestroyEvent2 IDebugProcessDestroyEvent2;
#endif 	/* __IDebugProcessDestroyEvent2_FWD_DEFINED__ */


#ifndef __IDebugProgramCreateEvent2_FWD_DEFINED__
#define __IDebugProgramCreateEvent2_FWD_DEFINED__
typedef interface IDebugProgramCreateEvent2 IDebugProgramCreateEvent2;
#endif 	/* __IDebugProgramCreateEvent2_FWD_DEFINED__ */


#ifndef __IDebugProgramDestroyEvent2_FWD_DEFINED__
#define __IDebugProgramDestroyEvent2_FWD_DEFINED__
typedef interface IDebugProgramDestroyEvent2 IDebugProgramDestroyEvent2;
#endif 	/* __IDebugProgramDestroyEvent2_FWD_DEFINED__ */


#ifndef __IDebugThreadCreateEvent2_FWD_DEFINED__
#define __IDebugThreadCreateEvent2_FWD_DEFINED__
typedef interface IDebugThreadCreateEvent2 IDebugThreadCreateEvent2;
#endif 	/* __IDebugThreadCreateEvent2_FWD_DEFINED__ */


#ifndef __IDebugThreadDestroyEvent2_FWD_DEFINED__
#define __IDebugThreadDestroyEvent2_FWD_DEFINED__
typedef interface IDebugThreadDestroyEvent2 IDebugThreadDestroyEvent2;
#endif 	/* __IDebugThreadDestroyEvent2_FWD_DEFINED__ */


#ifndef __IDebugLoadCompleteEvent2_FWD_DEFINED__
#define __IDebugLoadCompleteEvent2_FWD_DEFINED__
typedef interface IDebugLoadCompleteEvent2 IDebugLoadCompleteEvent2;
#endif 	/* __IDebugLoadCompleteEvent2_FWD_DEFINED__ */


#ifndef __IDebugEntryPointEvent2_FWD_DEFINED__
#define __IDebugEntryPointEvent2_FWD_DEFINED__
typedef interface IDebugEntryPointEvent2 IDebugEntryPointEvent2;
#endif 	/* __IDebugEntryPointEvent2_FWD_DEFINED__ */


#ifndef __IDebugStepCompleteEvent2_FWD_DEFINED__
#define __IDebugStepCompleteEvent2_FWD_DEFINED__
typedef interface IDebugStepCompleteEvent2 IDebugStepCompleteEvent2;
#endif 	/* __IDebugStepCompleteEvent2_FWD_DEFINED__ */


#ifndef __IDebugCanStopEvent2_FWD_DEFINED__
#define __IDebugCanStopEvent2_FWD_DEFINED__
typedef interface IDebugCanStopEvent2 IDebugCanStopEvent2;
#endif 	/* __IDebugCanStopEvent2_FWD_DEFINED__ */


#ifndef __IDebugBreakEvent2_FWD_DEFINED__
#define __IDebugBreakEvent2_FWD_DEFINED__
typedef interface IDebugBreakEvent2 IDebugBreakEvent2;
#endif 	/* __IDebugBreakEvent2_FWD_DEFINED__ */


#ifndef __IDebugBreakpointEvent2_FWD_DEFINED__
#define __IDebugBreakpointEvent2_FWD_DEFINED__
typedef interface IDebugBreakpointEvent2 IDebugBreakpointEvent2;
#endif 	/* __IDebugBreakpointEvent2_FWD_DEFINED__ */


#ifndef __IDebugExceptionEvent2_FWD_DEFINED__
#define __IDebugExceptionEvent2_FWD_DEFINED__
typedef interface IDebugExceptionEvent2 IDebugExceptionEvent2;
#endif 	/* __IDebugExceptionEvent2_FWD_DEFINED__ */


#ifndef __IDebugOutputStringEvent2_FWD_DEFINED__
#define __IDebugOutputStringEvent2_FWD_DEFINED__
typedef interface IDebugOutputStringEvent2 IDebugOutputStringEvent2;
#endif 	/* __IDebugOutputStringEvent2_FWD_DEFINED__ */


#ifndef __IDebugModuleLoadEvent2_FWD_DEFINED__
#define __IDebugModuleLoadEvent2_FWD_DEFINED__
typedef interface IDebugModuleLoadEvent2 IDebugModuleLoadEvent2;
#endif 	/* __IDebugModuleLoadEvent2_FWD_DEFINED__ */


#ifndef __IDebugPropertyCreateEvent2_FWD_DEFINED__
#define __IDebugPropertyCreateEvent2_FWD_DEFINED__
typedef interface IDebugPropertyCreateEvent2 IDebugPropertyCreateEvent2;
#endif 	/* __IDebugPropertyCreateEvent2_FWD_DEFINED__ */


#ifndef __IDebugPropertyDestroyEvent2_FWD_DEFINED__
#define __IDebugPropertyDestroyEvent2_FWD_DEFINED__
typedef interface IDebugPropertyDestroyEvent2 IDebugPropertyDestroyEvent2;
#endif 	/* __IDebugPropertyDestroyEvent2_FWD_DEFINED__ */


#ifndef __IDebugBreakpointBoundEvent2_FWD_DEFINED__
#define __IDebugBreakpointBoundEvent2_FWD_DEFINED__
typedef interface IDebugBreakpointBoundEvent2 IDebugBreakpointBoundEvent2;
#endif 	/* __IDebugBreakpointBoundEvent2_FWD_DEFINED__ */


#ifndef __IDebugBreakpointUnboundEvent2_FWD_DEFINED__
#define __IDebugBreakpointUnboundEvent2_FWD_DEFINED__
typedef interface IDebugBreakpointUnboundEvent2 IDebugBreakpointUnboundEvent2;
#endif 	/* __IDebugBreakpointUnboundEvent2_FWD_DEFINED__ */


#ifndef __IDebugBreakpointErrorEvent2_FWD_DEFINED__
#define __IDebugBreakpointErrorEvent2_FWD_DEFINED__
typedef interface IDebugBreakpointErrorEvent2 IDebugBreakpointErrorEvent2;
#endif 	/* __IDebugBreakpointErrorEvent2_FWD_DEFINED__ */


#ifndef __IDebugExpressionEvaluationCompleteEvent2_FWD_DEFINED__
#define __IDebugExpressionEvaluationCompleteEvent2_FWD_DEFINED__
typedef interface IDebugExpressionEvaluationCompleteEvent2 IDebugExpressionEvaluationCompleteEvent2;
#endif 	/* __IDebugExpressionEvaluationCompleteEvent2_FWD_DEFINED__ */


#ifndef __IDebugReturnValueEvent2_FWD_DEFINED__
#define __IDebugReturnValueEvent2_FWD_DEFINED__
typedef interface IDebugReturnValueEvent2 IDebugReturnValueEvent2;
#endif 	/* __IDebugReturnValueEvent2_FWD_DEFINED__ */


#ifndef __IDebugFindSymbolEvent2_FWD_DEFINED__
#define __IDebugFindSymbolEvent2_FWD_DEFINED__
typedef interface IDebugFindSymbolEvent2 IDebugFindSymbolEvent2;
#endif 	/* __IDebugFindSymbolEvent2_FWD_DEFINED__ */


#ifndef __IDebugNoSymbolsEvent2_FWD_DEFINED__
#define __IDebugNoSymbolsEvent2_FWD_DEFINED__
typedef interface IDebugNoSymbolsEvent2 IDebugNoSymbolsEvent2;
#endif 	/* __IDebugNoSymbolsEvent2_FWD_DEFINED__ */


#ifndef __IDebugProgramNameChangedEvent2_FWD_DEFINED__
#define __IDebugProgramNameChangedEvent2_FWD_DEFINED__
typedef interface IDebugProgramNameChangedEvent2 IDebugProgramNameChangedEvent2;
#endif 	/* __IDebugProgramNameChangedEvent2_FWD_DEFINED__ */


#ifndef __IDebugThreadNameChangedEvent2_FWD_DEFINED__
#define __IDebugThreadNameChangedEvent2_FWD_DEFINED__
typedef interface IDebugThreadNameChangedEvent2 IDebugThreadNameChangedEvent2;
#endif 	/* __IDebugThreadNameChangedEvent2_FWD_DEFINED__ */


#ifndef __IDebugMessageEvent2_FWD_DEFINED__
#define __IDebugMessageEvent2_FWD_DEFINED__
typedef interface IDebugMessageEvent2 IDebugMessageEvent2;
#endif 	/* __IDebugMessageEvent2_FWD_DEFINED__ */


#ifndef __IDebugActivateDocumentEvent2_FWD_DEFINED__
#define __IDebugActivateDocumentEvent2_FWD_DEFINED__
typedef interface IDebugActivateDocumentEvent2 IDebugActivateDocumentEvent2;
#endif 	/* __IDebugActivateDocumentEvent2_FWD_DEFINED__ */


#ifndef __IDebugProcess2_FWD_DEFINED__
#define __IDebugProcess2_FWD_DEFINED__
typedef interface IDebugProcess2 IDebugProcess2;
#endif 	/* __IDebugProcess2_FWD_DEFINED__ */


#ifndef __IDebugProgram2_FWD_DEFINED__
#define __IDebugProgram2_FWD_DEFINED__
typedef interface IDebugProgram2 IDebugProgram2;
#endif 	/* __IDebugProgram2_FWD_DEFINED__ */


#ifndef __IDebugEngineProgram2_FWD_DEFINED__
#define __IDebugEngineProgram2_FWD_DEFINED__
typedef interface IDebugEngineProgram2 IDebugEngineProgram2;
#endif 	/* __IDebugEngineProgram2_FWD_DEFINED__ */


#ifndef __IDebugProgramHost2_FWD_DEFINED__
#define __IDebugProgramHost2_FWD_DEFINED__
typedef interface IDebugProgramHost2 IDebugProgramHost2;
#endif 	/* __IDebugProgramHost2_FWD_DEFINED__ */


#ifndef __IDebugProgramNode2_FWD_DEFINED__
#define __IDebugProgramNode2_FWD_DEFINED__
typedef interface IDebugProgramNode2 IDebugProgramNode2;
#endif 	/* __IDebugProgramNode2_FWD_DEFINED__ */


#ifndef __IDebugProgramEngines2_FWD_DEFINED__
#define __IDebugProgramEngines2_FWD_DEFINED__
typedef interface IDebugProgramEngines2 IDebugProgramEngines2;
#endif 	/* __IDebugProgramEngines2_FWD_DEFINED__ */


#ifndef __IDebugThread2_FWD_DEFINED__
#define __IDebugThread2_FWD_DEFINED__
typedef interface IDebugThread2 IDebugThread2;
#endif 	/* __IDebugThread2_FWD_DEFINED__ */


#ifndef __IDebugLogicalThread2_FWD_DEFINED__
#define __IDebugLogicalThread2_FWD_DEFINED__
typedef interface IDebugLogicalThread2 IDebugLogicalThread2;
#endif 	/* __IDebugLogicalThread2_FWD_DEFINED__ */


#ifndef __IDebugProperty2_FWD_DEFINED__
#define __IDebugProperty2_FWD_DEFINED__
typedef interface IDebugProperty2 IDebugProperty2;
#endif 	/* __IDebugProperty2_FWD_DEFINED__ */


#ifndef __IDebugReference2_FWD_DEFINED__
#define __IDebugReference2_FWD_DEFINED__
typedef interface IDebugReference2 IDebugReference2;
#endif 	/* __IDebugReference2_FWD_DEFINED__ */


#ifndef __IDebugStackFrame2_FWD_DEFINED__
#define __IDebugStackFrame2_FWD_DEFINED__
typedef interface IDebugStackFrame2 IDebugStackFrame2;
#endif 	/* __IDebugStackFrame2_FWD_DEFINED__ */


#ifndef __IDebugMemoryContext2_FWD_DEFINED__
#define __IDebugMemoryContext2_FWD_DEFINED__
typedef interface IDebugMemoryContext2 IDebugMemoryContext2;
#endif 	/* __IDebugMemoryContext2_FWD_DEFINED__ */


#ifndef __IDebugCodeContext2_FWD_DEFINED__
#define __IDebugCodeContext2_FWD_DEFINED__
typedef interface IDebugCodeContext2 IDebugCodeContext2;
#endif 	/* __IDebugCodeContext2_FWD_DEFINED__ */


#ifndef __IDebugMemoryBytes2_FWD_DEFINED__
#define __IDebugMemoryBytes2_FWD_DEFINED__
typedef interface IDebugMemoryBytes2 IDebugMemoryBytes2;
#endif 	/* __IDebugMemoryBytes2_FWD_DEFINED__ */


#ifndef __IDebugDisassemblyStream2_FWD_DEFINED__
#define __IDebugDisassemblyStream2_FWD_DEFINED__
typedef interface IDebugDisassemblyStream2 IDebugDisassemblyStream2;
#endif 	/* __IDebugDisassemblyStream2_FWD_DEFINED__ */


#ifndef __IDebugDocumentContext2_FWD_DEFINED__
#define __IDebugDocumentContext2_FWD_DEFINED__
typedef interface IDebugDocumentContext2 IDebugDocumentContext2;
#endif 	/* __IDebugDocumentContext2_FWD_DEFINED__ */


#ifndef __IDebugExpressionContext2_FWD_DEFINED__
#define __IDebugExpressionContext2_FWD_DEFINED__
typedef interface IDebugExpressionContext2 IDebugExpressionContext2;
#endif 	/* __IDebugExpressionContext2_FWD_DEFINED__ */


#ifndef __IDebugBreakpointRequest2_FWD_DEFINED__
#define __IDebugBreakpointRequest2_FWD_DEFINED__
typedef interface IDebugBreakpointRequest2 IDebugBreakpointRequest2;
#endif 	/* __IDebugBreakpointRequest2_FWD_DEFINED__ */


#ifndef __IDebugBreakpointResolution2_FWD_DEFINED__
#define __IDebugBreakpointResolution2_FWD_DEFINED__
typedef interface IDebugBreakpointResolution2 IDebugBreakpointResolution2;
#endif 	/* __IDebugBreakpointResolution2_FWD_DEFINED__ */


#ifndef __IDebugErrorBreakpointResolution2_FWD_DEFINED__
#define __IDebugErrorBreakpointResolution2_FWD_DEFINED__
typedef interface IDebugErrorBreakpointResolution2 IDebugErrorBreakpointResolution2;
#endif 	/* __IDebugErrorBreakpointResolution2_FWD_DEFINED__ */


#ifndef __IDebugBoundBreakpoint2_FWD_DEFINED__
#define __IDebugBoundBreakpoint2_FWD_DEFINED__
typedef interface IDebugBoundBreakpoint2 IDebugBoundBreakpoint2;
#endif 	/* __IDebugBoundBreakpoint2_FWD_DEFINED__ */


#ifndef __IDebugPendingBreakpoint2_FWD_DEFINED__
#define __IDebugPendingBreakpoint2_FWD_DEFINED__
typedef interface IDebugPendingBreakpoint2 IDebugPendingBreakpoint2;
#endif 	/* __IDebugPendingBreakpoint2_FWD_DEFINED__ */


#ifndef __IDebugErrorBreakpoint2_FWD_DEFINED__
#define __IDebugErrorBreakpoint2_FWD_DEFINED__
typedef interface IDebugErrorBreakpoint2 IDebugErrorBreakpoint2;
#endif 	/* __IDebugErrorBreakpoint2_FWD_DEFINED__ */


#ifndef __IDebugExpression2_FWD_DEFINED__
#define __IDebugExpression2_FWD_DEFINED__
typedef interface IDebugExpression2 IDebugExpression2;
#endif 	/* __IDebugExpression2_FWD_DEFINED__ */


#ifndef __IDebugModule2_FWD_DEFINED__
#define __IDebugModule2_FWD_DEFINED__
typedef interface IDebugModule2 IDebugModule2;
#endif 	/* __IDebugModule2_FWD_DEFINED__ */


#ifndef __IDebugModuleManaged_FWD_DEFINED__
#define __IDebugModuleManaged_FWD_DEFINED__
typedef interface IDebugModuleManaged IDebugModuleManaged;
#endif 	/* __IDebugModuleManaged_FWD_DEFINED__ */


#ifndef __IDebugDocument2_FWD_DEFINED__
#define __IDebugDocument2_FWD_DEFINED__
typedef interface IDebugDocument2 IDebugDocument2;
#endif 	/* __IDebugDocument2_FWD_DEFINED__ */


#ifndef __IDebugDocumentText2_FWD_DEFINED__
#define __IDebugDocumentText2_FWD_DEFINED__
typedef interface IDebugDocumentText2 IDebugDocumentText2;
#endif 	/* __IDebugDocumentText2_FWD_DEFINED__ */


#ifndef __IDebugDocumentPosition2_FWD_DEFINED__
#define __IDebugDocumentPosition2_FWD_DEFINED__
typedef interface IDebugDocumentPosition2 IDebugDocumentPosition2;
#endif 	/* __IDebugDocumentPosition2_FWD_DEFINED__ */


#ifndef __IDebugDocumentPositionOffset2_FWD_DEFINED__
#define __IDebugDocumentPositionOffset2_FWD_DEFINED__
typedef interface IDebugDocumentPositionOffset2 IDebugDocumentPositionOffset2;
#endif 	/* __IDebugDocumentPositionOffset2_FWD_DEFINED__ */


#ifndef __IDebugFunctionPosition2_FWD_DEFINED__
#define __IDebugFunctionPosition2_FWD_DEFINED__
typedef interface IDebugFunctionPosition2 IDebugFunctionPosition2;
#endif 	/* __IDebugFunctionPosition2_FWD_DEFINED__ */


#ifndef __IDebugDocumentTextEvents2_FWD_DEFINED__
#define __IDebugDocumentTextEvents2_FWD_DEFINED__
typedef interface IDebugDocumentTextEvents2 IDebugDocumentTextEvents2;
#endif 	/* __IDebugDocumentTextEvents2_FWD_DEFINED__ */


#ifndef __IDebugQueryEngine2_FWD_DEFINED__
#define __IDebugQueryEngine2_FWD_DEFINED__
typedef interface IDebugQueryEngine2 IDebugQueryEngine2;
#endif 	/* __IDebugQueryEngine2_FWD_DEFINED__ */


#ifndef __IEnumDebugProcesses2_FWD_DEFINED__
#define __IEnumDebugProcesses2_FWD_DEFINED__
typedef interface IEnumDebugProcesses2 IEnumDebugProcesses2;
#endif 	/* __IEnumDebugProcesses2_FWD_DEFINED__ */


#ifndef __IEnumDebugPrograms2_FWD_DEFINED__
#define __IEnumDebugPrograms2_FWD_DEFINED__
typedef interface IEnumDebugPrograms2 IEnumDebugPrograms2;
#endif 	/* __IEnumDebugPrograms2_FWD_DEFINED__ */


#ifndef __IEnumDebugThreads2_FWD_DEFINED__
#define __IEnumDebugThreads2_FWD_DEFINED__
typedef interface IEnumDebugThreads2 IEnumDebugThreads2;
#endif 	/* __IEnumDebugThreads2_FWD_DEFINED__ */


#ifndef __IEnumDebugStackFrames2_FWD_DEFINED__
#define __IEnumDebugStackFrames2_FWD_DEFINED__
typedef interface IEnumDebugStackFrames2 IEnumDebugStackFrames2;
#endif 	/* __IEnumDebugStackFrames2_FWD_DEFINED__ */


#ifndef __IEnumDebugCodeContexts2_FWD_DEFINED__
#define __IEnumDebugCodeContexts2_FWD_DEFINED__
typedef interface IEnumDebugCodeContexts2 IEnumDebugCodeContexts2;
#endif 	/* __IEnumDebugCodeContexts2_FWD_DEFINED__ */


#ifndef __IEnumDebugBoundBreakpoints2_FWD_DEFINED__
#define __IEnumDebugBoundBreakpoints2_FWD_DEFINED__
typedef interface IEnumDebugBoundBreakpoints2 IEnumDebugBoundBreakpoints2;
#endif 	/* __IEnumDebugBoundBreakpoints2_FWD_DEFINED__ */


#ifndef __IEnumDebugPendingBreakpoints2_FWD_DEFINED__
#define __IEnumDebugPendingBreakpoints2_FWD_DEFINED__
typedef interface IEnumDebugPendingBreakpoints2 IEnumDebugPendingBreakpoints2;
#endif 	/* __IEnumDebugPendingBreakpoints2_FWD_DEFINED__ */


#ifndef __IEnumDebugErrorBreakpoints2_FWD_DEFINED__
#define __IEnumDebugErrorBreakpoints2_FWD_DEFINED__
typedef interface IEnumDebugErrorBreakpoints2 IEnumDebugErrorBreakpoints2;
#endif 	/* __IEnumDebugErrorBreakpoints2_FWD_DEFINED__ */


#ifndef __IEnumDebugMachines2_FWD_DEFINED__
#define __IEnumDebugMachines2_FWD_DEFINED__
typedef interface IEnumDebugMachines2 IEnumDebugMachines2;
#endif 	/* __IEnumDebugMachines2_FWD_DEFINED__ */


#ifndef __IEnumDebugExceptionInfo2_FWD_DEFINED__
#define __IEnumDebugExceptionInfo2_FWD_DEFINED__
typedef interface IEnumDebugExceptionInfo2 IEnumDebugExceptionInfo2;
#endif 	/* __IEnumDebugExceptionInfo2_FWD_DEFINED__ */


#ifndef __IEnumDebugFrameInfo2_FWD_DEFINED__
#define __IEnumDebugFrameInfo2_FWD_DEFINED__
typedef interface IEnumDebugFrameInfo2 IEnumDebugFrameInfo2;
#endif 	/* __IEnumDebugFrameInfo2_FWD_DEFINED__ */


#ifndef __IEnumCodePaths2_FWD_DEFINED__
#define __IEnumCodePaths2_FWD_DEFINED__
typedef interface IEnumCodePaths2 IEnumCodePaths2;
#endif 	/* __IEnumCodePaths2_FWD_DEFINED__ */


#ifndef __IEnumDebugModules2_FWD_DEFINED__
#define __IEnumDebugModules2_FWD_DEFINED__
typedef interface IEnumDebugModules2 IEnumDebugModules2;
#endif 	/* __IEnumDebugModules2_FWD_DEFINED__ */


#ifndef __IEnumDebugPortSuppliers2_FWD_DEFINED__
#define __IEnumDebugPortSuppliers2_FWD_DEFINED__
typedef interface IEnumDebugPortSuppliers2 IEnumDebugPortSuppliers2;
#endif 	/* __IEnumDebugPortSuppliers2_FWD_DEFINED__ */


#ifndef __IEnumDebugPorts2_FWD_DEFINED__
#define __IEnumDebugPorts2_FWD_DEFINED__
typedef interface IEnumDebugPorts2 IEnumDebugPorts2;
#endif 	/* __IEnumDebugPorts2_FWD_DEFINED__ */


#ifndef __IEnumDebugPropertyInfo2_FWD_DEFINED__
#define __IEnumDebugPropertyInfo2_FWD_DEFINED__
typedef interface IEnumDebugPropertyInfo2 IEnumDebugPropertyInfo2;
#endif 	/* __IEnumDebugPropertyInfo2_FWD_DEFINED__ */


#ifndef __IEnumDebugReferenceInfo2_FWD_DEFINED__
#define __IEnumDebugReferenceInfo2_FWD_DEFINED__
typedef interface IEnumDebugReferenceInfo2 IEnumDebugReferenceInfo2;
#endif 	/* __IEnumDebugReferenceInfo2_FWD_DEFINED__ */


#ifndef __SDMServer_FWD_DEFINED__
#define __SDMServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class SDMServer SDMServer;
#else
typedef struct SDMServer SDMServer;
#endif /* __cplusplus */

#endif 	/* __SDMServer_FWD_DEFINED__ */


#ifndef __MsMachineDebugManager_FWD_DEFINED__
#define __MsMachineDebugManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class MsMachineDebugManager MsMachineDebugManager;
#else
typedef struct MsMachineDebugManager MsMachineDebugManager;
#endif /* __cplusplus */

#endif 	/* __MsMachineDebugManager_FWD_DEFINED__ */


#ifndef __MDMUtilServer_FWD_DEFINED__
#define __MDMUtilServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class MDMUtilServer MDMUtilServer;
#else
typedef struct MDMUtilServer MDMUtilServer;
#endif /* __cplusplus */

#endif 	/* __MDMUtilServer_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"
#include "enc.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_msdbg_0000 */
/* [local] */ 

#ifndef _BASETSD_H_
#pragma once
typedef signed char INT8;

typedef signed char *PINT8;

typedef short INT16;

typedef short *PINT16;

typedef int INT32;

typedef int *PINT32;

typedef __int64 INT64;

typedef __int64 *PINT64;

typedef unsigned char UINT8;

typedef unsigned char *PUINT8;

typedef unsigned short UINT16;

typedef unsigned short *PUINT16;

typedef unsigned int UINT32;

typedef unsigned int *PUINT32;

typedef unsigned __int64 UINT64;

typedef unsigned __int64 *PUINT64;

typedef int LONG32;

typedef int *PLONG32;

typedef unsigned int ULONG32;

typedef unsigned int *PULONG32;

typedef unsigned int DWORD32;

typedef unsigned int *PDWORD32;

typedef /* [public] */ __int3264 INT_PTR;

typedef /* [public] */ __int3264 *PINT_PTR;

typedef /* [public] */ unsigned __int3264 UINT_PTR;

typedef /* [public] */ unsigned __int3264 *PUINT_PTR;

typedef /* [public] */ __int3264 LONG_PTR;

typedef /* [public] */ __int3264 *PLONG_PTR;

typedef /* [public] */ unsigned __int3264 ULONG_PTR;

typedef /* [public] */ unsigned __int3264 *PULONG_PTR;

typedef unsigned short UHALF_PTR;

typedef unsigned short *PUHALF_PTR;

typedef short HALF_PTR;

typedef short *PHALF_PTR;

typedef long SHANDLE_PTR;

typedef unsigned long HANDLE_PTR;

typedef ULONG_PTR SIZE_T;

typedef ULONG_PTR *PSIZE_T;

typedef LONG_PTR SSIZE_T;

typedef LONG_PTR *PSSIZE_T;

typedef ULONG_PTR DWORD_PTR;

typedef ULONG_PTR *PDWORD_PTR;

typedef __int64 LONG64;

typedef __int64 *PLONG64;

typedef unsigned __int64 ULONG64;

typedef unsigned __int64 *PULONG64;

typedef unsigned __int64 DWORD64;

typedef unsigned __int64 *PDWORD64;

typedef ULONG_PTR KAFFINITY;

typedef KAFFINITY *PKAFFINITY;

#endif // _BASETSD_H_
#define	MSDBG_VERSION	( 7126 )

#define E_ATTACH_DEBUGGER_ALREADY_ATTACHED						MAKE_HRESULT(1, FACILITY_ITF, 0x0001)
#define E_ATTACH_DEBUGGEE_PROCESS_SECURITY_VIOLATION				MAKE_HRESULT(1, FACILITY_ITF, 0x0002)
#define E_ATTACH_CANNOT_ATTACH_TO_DESKTOP						MAKE_HRESULT(1, FACILITY_ITF, 0x0003)
#define S_ATTACH_DEFERRED										MAKE_HRESULT(0, FACILITY_ITF, 0x0004)
#define E_LAUNCH_NO_INTEROP                                      MAKE_HRESULT(1, FACILITY_ITF, 0x0005)
#define E_LAUNCH_DEBUGGING_NOT_POSSIBLE                          MAKE_HRESULT(1, FACILITY_ITF, 0x0006)
#define E_LAUNCH_KERNEL_DEBUGGER_ENABLED                         MAKE_HRESULT(1, FACILITY_ITF, 0x0007)
#define E_LAUNCH_KERNEL_DEBUGGER_PRESENT                         MAKE_HRESULT(1, FACILITY_ITF, 0x0008)
#define E_INTEROP_NOT_SUPPORTED                                  MAKE_HRESULT(1, FACILITY_ITF, 0x0009)
#define E_TOO_MANY_PROCESSES                                     MAKE_HRESULT(1, FACILITY_ITF, 0x000A)
#define E_MACHINE_NOT_AVAILABLE									MAKE_HRESULT(1, FACILITY_ITF, 0x0010)
#define E_MACHINE_DOES_NOT_EXIST									MAKE_HRESULT(1, FACILITY_ITF, 0x0020)
#define E_MACHINE_ACCESS_DENIED									MAKE_HRESULT(1, FACILITY_ITF, 0x0021)
#define E_MACHINE_MDM_EXEC_FAILURE								MAKE_HRESULT(1, FACILITY_ITF, 0x0022)
#define E_MACHINE_DEBUGGER_NOT_REGISTERED_PROPERLY				MAKE_HRESULT(1, FACILITY_ITF, 0x0023)
#define E_EVALUATE_BUSY_WITH_EVALUATION							MAKE_HRESULT(1, FACILITY_ITF, 0x0030)
#define E_EVALUATE_TIMEOUT										MAKE_HRESULT(1, FACILITY_ITF, 0x0031)
#define E_MEMORY_NOTSUPPORTED									MAKE_HRESULT(1, FACILITY_ITF, 0x0040)
#define E_DISASM_NOTSUPPORTED									MAKE_HRESULT(1, FACILITY_ITF, 0x0041)
#define E_DISASM_BADADDRESS                                      MAKE_HRESULT(1, FACILITY_ITF, 0x0042)
#define E_DISASM_NOTAVAILABLE                                    MAKE_HRESULT(1, FACILITY_ITF, 0x0043)
#define E_BP_DELETED												MAKE_HRESULT(1, FACILITY_ITF, 0x0060)
#define E_PROCESS_DESTROYED										MAKE_HRESULT(1, FACILITY_ITF, 0x0070)
#define E_PROCESS_DEBUGGER_IS_DEBUGGEE							MAKE_HRESULT(1, FACILITY_ITF, 0x0071)
#define E_PORTSUPPLIER_NO_PORT									MAKE_HRESULT(1, FACILITY_ITF, 0x0080)
#define E_PORT_NO_REQUEST										MAKE_HRESULT(1, FACILITY_ITF, 0x0090)
#define E_COMPARE_CANNOT_COMPARE									MAKE_HRESULT(1, FACILITY_ITF, 0x00a0)
#define S_JIT_USERCANCELLED										MAKE_HRESULT(0, FACILITY_ITF, 0x00b0)
#define E_JIT_INVALID_PID										MAKE_HRESULT(1, FACILITY_ITF, 0x00b1)
#define E_JIT_REMOTE_FAILED										MAKE_HRESULT(0, FACILITY_ITF, 0x00b2)
#define S_TERMINATE_PROCESSES_STILL_DETACHING					MAKE_HRESULT(0, FACILITY_ITF, 0x00c0)
#define E_DETACH_NO_PROXY								        MAKE_HRESULT(1, FACILITY_ITF, 0x00d0)
#define E_DETACH_TS_UNSUPPORTED						            MAKE_HRESULT(1, FACILITY_ITF, 0x00e0)
#define E_DETACH_IMPERSONATE_FAILURE						        MAKE_HRESULT(1, FACILITY_ITF, 0x00f0)
#define E_CANNOT_SET_NEXT_STATEMENT_ON_NONLEAF_FRAME				MAKE_HRESULT(1, FACILITY_ITF, 0x0100)
#define E_CANNOT_SETIP_TO_DIFFERENT_FUNCTION                     MAKE_HRESULT(1, FACILITY_ITF, 0x0104)
#define E_CANNOT_SET_NEXT_STATEMENT_ON_EXCEPTION                 MAKE_HRESULT(1, FACILITY_ITF, 0x0105)
#define E_CANNOT_DEBUG_MDM										MAKE_HRESULT(1, FACILITY_ITF, 0x0110)
#define E_TARGET_FILE_MISSING									MAKE_HRESULT(1, FACILITY_ITF, 0x0100)
#define E_TARGET_FILE_MISMATCH									MAKE_HRESULT(1, FACILITY_ITF, 0x0101)
#define E_IMAGE_NOT_LOADED										MAKE_HRESULT(1, FACILITY_ITF, 0x0102)
#define E_FIBER_NOT_SUPPORTED									MAKE_HRESULT(1, FACILITY_ITF, 0x0103)
#define E_ASYNCBREAK_NO_PROGRAMS									MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0110)
#define E_ASYNCBREAK_DEBUGGEE_NOT_INITIALIZED					MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0111)
#define E_CANNOT_GET_UNMANAGED_MEMORY_CONTEXT					MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0561)
#define E_DESTROYED												MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 1)
#define E_REMOTE_NOMSVCMON										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 2)
#define E_REMOTE_BADIPADDRESS									MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 3)
#define E_REMOTE_MACHINEDOWN										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 4)
#define E_REMOTE_MACHINEUNSPECIFIED								MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 5)
#define E_CRASHDUMP_ACTIVE										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 6)
#define E_ALL_THREADS_SUSPENDED									MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 7)
#define E_LOAD_DLL_TL											MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 8)
#define E_LOAD_DLL_SH											MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 9)
#define E_LOAD_DLL_EM											MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 10)
#define E_LOAD_DLL_EE											MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 11)
#define E_LOAD_DLL_DM											MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 12)
#define E_LOAD_DLL_MD											MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 13)
#define E_IOREDIR_BADFILE										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 14)
#define E_IOREDIR_BADSYNTAX										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 15)
#define E_REMOTE_BADVERSION										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 16)
#define E_CRASHDUMP_UNSUPPORTED									MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 17)
#define E_UNSUPPORTED_BINARY										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 21)
#define E_DEBUGGEE_BLOCKED										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 22)
#define E_REMOTE_NOUSERMSVCMON									MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 23)
#define E_STEP_WIN9xSYSCODE                                      MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 24)
#define E_INTEROP_ORPC_INIT 										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 25)
#define E_CANNOT_DEBUG_WIN32										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 27)
#define E_CANNOT_DEBUG_WIN64										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 28)
#define E_MINIDUMP_READ_WIN9X                                    MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 29)
#define E_CROSS_TSSESSION_ATTACH 								MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 30)
#define E_STEP_BP_SET_FAILED										MAKE_HRESULT (SEVERITY_ERROR, FACILITY_ITF, 0x200 + 31)
#define E_REFERENCE_NOT_VALID									MAKE_HRESULT(1, FACILITY_ITF, 0x0501)
#define E_PROPERTY_NOT_VALID										MAKE_HRESULT(1, FACILITY_ITF, 0x0511)
#define E_SETVALUE_VALUE_CANNOT_BE_SET							MAKE_HRESULT(1, FACILITY_ITF, 0x0521)
#define E_SETVALUE_VALUE_IS_READONLY								MAKE_HRESULT(1, FACILITY_ITF, 0x0522)
#define E_SETVALUEASREFERENCE_NOTSUPPORTED						MAKE_HRESULT(1, FACILITY_ITF, 0x0523)
#define S_GETPARENT_NO_PARENT									MAKE_HRESULT(0, FACILITY_ITF, 0x0531)
#define S_GETDERIVEDMOST_NO_DERIVED_MOST							MAKE_HRESULT(0, FACILITY_ITF, 0x0541)
#define S_GETMEMORYBYTES_NO_MEMORY_BYTES							MAKE_HRESULT(0, FACILITY_ITF, 0x0551)
#define S_GETMEMORYCONTEXT_NO_MEMORY_CONTEXT						MAKE_HRESULT(0, FACILITY_ITF, 0x0561)
#define S_GETSIZE_NO_SIZE										MAKE_HRESULT(0, FACILITY_ITF, 0x0571)
#define E_GETREFERENCE_NO_REFERENCE								MAKE_HRESULT(1, FACILITY_ITF, 0x0581)
#define S_GETEXTENDEDINFO_NO_EXTENDEDINFO						MAKE_HRESULT(0, FACILITY_ITF, 0x0591)
#define E_CODE_CONTEXT_OUT_OF_SCOPE								MAKE_HRESULT(1, FACILITY_ITF, 0x05A1)
#define E_INVALID_SESSIONID										MAKE_HRESULT(1, FACILITY_ITF, 0x05A2)
#define E_SERVER_UNAVAILABLE_ON_CALLBACK							MAKE_HRESULT(1, FACILITY_ITF, 0x05A3)
#define E_ACCESS_DENIED_ON_CALLBACK								MAKE_HRESULT(1, FACILITY_ITF, 0x05A4)
#define E_UNKNOWN_AUTHN_SERVICE_ON_CALLBACK						MAKE_HRESULT(1, FACILITY_ITF, 0x05A5)
#define E_NO_SESSION_AVAILABLE									MAKE_HRESULT(1, FACILITY_ITF, 0x05A6)
#define E_CLIENT_NOT_LOGGED_ON                                   MAKE_HRESULT(1, FACILITY_ITF, 0x05A7)
#define E_OTHER_USERS_SESSION                                    MAKE_HRESULT(1, FACILITY_ITF, 0x05A8)
#define E_USER_LEVEL_ACCESS_CONTROL_REQUIRED                     MAKE_HRESULT(1, FACILITY_ITF, 0x05A9)
#define E_SCRIPT_CLR_EE_DISABLED                                 MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x5B0)
#define E_HTTP_SERVERERROR										MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x700)
#define E_HTTP_PERMISSION_DENIED									MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x701)
#define E_HTTP_SENDREQUEST_FAILED								MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x702)
#define E_HTTP_FORBIDDEN											MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x703)
#define E_HTTP_NOT_SUPPORTED										MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x704)
#define E_HTTP_NO_CONTENT										MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x705)
#define E_FUNCTION_NOT_JITTED                                    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x800)
#define E_NO_CODE_CONTEXT                                        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x801)
#define E_AUTOATTACH_ACCESS_DENIED                               MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x900)
#define E_AUTOATTACH_WEBSERVER_NOT_FOUND                         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x901)





















































extern GUID guidVBLang;
extern GUID guidVBScriptLang;
extern GUID guidJScriptLang;
extern GUID guidCLang;
extern GUID guidCPPLang;
extern GUID guidSQLLang;
extern GUID guidScriptLang;
extern GUID guidSafeCLang;
extern GUID guidManagedCLang;
extern GUID guidManagedCPPLang;
extern GUID guidScriptEng;
extern GUID guidSQLEng;
extern GUID guidCOMPlusNativeEng;
extern GUID guidCOMPlusOnlyEng;
extern GUID guidNativeOnlyEng;
extern GUID guidMsOrclEng;
extern GUID guidEmbeddedCLREng;
extern GUID guidMicrosoftVendor;
extern GUID guidLocalPortSupplier;
extern GUID guidNativePortSupplier;
extern GUID guidEmbeddedCLRPortSupplier;
extern GUID guidFilterLocals;
extern GUID guidFilterAllLocals;
extern GUID guidFilterArgs;
extern GUID guidFilterLocalsPlusArgs;
extern GUID guidFilterAllLocalsPlusArgs;
extern GUID guidFilterRegisters;
extern GUID guidFilterThis;
// GUIDs for GetExtendedInfo
extern GUID guidDocument;
extern GUID guidCodeContext;
extern GUID guidExtendedInfoSlot;
extern GUID guidExtendedInfoSignature;

enum __MIDL___MIDL_itf_msdbg_0000_0001
    {	GN_NAME	= 0,
	GN_FILENAME	= GN_NAME + 1,
	GN_BASENAME	= GN_FILENAME + 1,
	GN_MONIKERNAME	= GN_BASENAME + 1,
	GN_URL	= GN_MONIKERNAME + 1,
	GN_TITLE	= GN_URL + 1,
	GN_STARTPAGEURL	= GN_TITLE + 1
    } ;
typedef DWORD GETNAME_TYPE;


enum __MIDL___MIDL_itf_msdbg_0000_0002
    {	TEXT_POSITION_MAX_LINE	= 0xffffffff,
	TEXT_POSITION_MAX_COLUMN	= 0xffffffff
    } ;
typedef struct _tagTEXT_POSITION
    {
    DWORD dwLine;
    DWORD dwColumn;
    } 	TEXT_POSITION;



extern RPC_IF_HANDLE __MIDL_itf_msdbg_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msdbg_0000_v0_0_s_ifspec;

#ifndef __IDebugMachine2_INTERFACE_DEFINED__
#define __IDebugMachine2_INTERFACE_DEFINED__

/* interface IDebugMachine2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugMachine2_0003
    {	MCIFLAG_TERMINAL_SERVICES_AVAILABLE	= 0x1
    } ;
typedef DWORD MACHINE_INFO_FLAGS;


enum __MIDL_IDebugMachine2_0004
    {	MCIF_NAME	= 0x1,
	MCIF_FLAGS	= 0x2,
	MCIF_ALL	= 0x3
    } ;
typedef DWORD MACHINE_INFO_FIELDS;

typedef struct tagMACHINE_INFO
    {
    MACHINE_INFO_FIELDS Fields;
    BSTR bstrName;
    MACHINE_INFO_FLAGS Flags;
    } 	MACHINE_INFO;


EXTERN_C const IID IID_IDebugMachine2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("17bf8fa3-4c5a-49a3-b2f8-5942e1ea287e")
    IDebugMachine2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [in] */ MACHINE_INFO_FIELDS Fields,
            /* [out] */ MACHINE_INFO *pMachineInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortSupplier( 
            /* [in] */ REFGUID guidPortSupplier,
            /* [out] */ IDebugPortSupplier2 **ppPortSupplier) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPort( 
            /* [in] */ REFGUID guidPort,
            /* [out] */ IDebugPort2 **ppPort) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPorts( 
            /* [out] */ IEnumDebugPorts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPortSuppliers( 
            /* [out] */ IEnumDebugPortSuppliers2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMachineUtilities( 
            /* [out] */ IDebugMDMUtil2 **ppUtil) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMachine2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMachine2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMachine2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMachine2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugMachine2 * This,
            /* [in] */ MACHINE_INFO_FIELDS Fields,
            /* [out] */ MACHINE_INFO *pMachineInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugMachine2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortSupplier )( 
            IDebugMachine2 * This,
            /* [in] */ REFGUID guidPortSupplier,
            /* [out] */ IDebugPortSupplier2 **ppPortSupplier);
        
        HRESULT ( STDMETHODCALLTYPE *GetPort )( 
            IDebugMachine2 * This,
            /* [in] */ REFGUID guidPort,
            /* [out] */ IDebugPort2 **ppPort);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPorts )( 
            IDebugMachine2 * This,
            /* [out] */ IEnumDebugPorts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPortSuppliers )( 
            IDebugMachine2 * This,
            /* [out] */ IEnumDebugPortSuppliers2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetMachineUtilities )( 
            IDebugMachine2 * This,
            /* [out] */ IDebugMDMUtil2 **ppUtil);
        
        END_INTERFACE
    } IDebugMachine2Vtbl;

    interface IDebugMachine2
    {
        CONST_VTBL struct IDebugMachine2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMachine2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMachine2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMachine2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMachine2_GetInfo(This,Fields,pMachineInfo)	\
    (This)->lpVtbl -> GetInfo(This,Fields,pMachineInfo)

#define IDebugMachine2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugMachine2_GetPortSupplier(This,guidPortSupplier,ppPortSupplier)	\
    (This)->lpVtbl -> GetPortSupplier(This,guidPortSupplier,ppPortSupplier)

#define IDebugMachine2_GetPort(This,guidPort,ppPort)	\
    (This)->lpVtbl -> GetPort(This,guidPort,ppPort)

#define IDebugMachine2_EnumPorts(This,ppEnum)	\
    (This)->lpVtbl -> EnumPorts(This,ppEnum)

#define IDebugMachine2_EnumPortSuppliers(This,ppEnum)	\
    (This)->lpVtbl -> EnumPortSuppliers(This,ppEnum)

#define IDebugMachine2_GetMachineUtilities(This,ppUtil)	\
    (This)->lpVtbl -> GetMachineUtilities(This,ppUtil)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMachine2_GetInfo_Proxy( 
    IDebugMachine2 * This,
    /* [in] */ MACHINE_INFO_FIELDS Fields,
    /* [out] */ MACHINE_INFO *pMachineInfo);


void __RPC_STUB IDebugMachine2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachine2_GetName_Proxy( 
    IDebugMachine2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugMachine2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachine2_GetPortSupplier_Proxy( 
    IDebugMachine2 * This,
    /* [in] */ REFGUID guidPortSupplier,
    /* [out] */ IDebugPortSupplier2 **ppPortSupplier);


void __RPC_STUB IDebugMachine2_GetPortSupplier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachine2_GetPort_Proxy( 
    IDebugMachine2 * This,
    /* [in] */ REFGUID guidPort,
    /* [out] */ IDebugPort2 **ppPort);


void __RPC_STUB IDebugMachine2_GetPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachine2_EnumPorts_Proxy( 
    IDebugMachine2 * This,
    /* [out] */ IEnumDebugPorts2 **ppEnum);


void __RPC_STUB IDebugMachine2_EnumPorts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachine2_EnumPortSuppliers_Proxy( 
    IDebugMachine2 * This,
    /* [out] */ IEnumDebugPortSuppliers2 **ppEnum);


void __RPC_STUB IDebugMachine2_EnumPortSuppliers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachine2_GetMachineUtilities_Proxy( 
    IDebugMachine2 * This,
    /* [out] */ IDebugMDMUtil2 **ppUtil);


void __RPC_STUB IDebugMachine2_GetMachineUtilities_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMachine2_INTERFACE_DEFINED__ */


#ifndef __IDebugMachineEx2_INTERFACE_DEFINED__
#define __IDebugMachineEx2_INTERFACE_DEFINED__

/* interface IDebugMachineEx2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugMachineEx2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ae27b230-a0bf-47ff-a2d1-22c29a178eac")
    IDebugMachineEx2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnableAutoAttachOnProgramCreate( 
            /* [in] */ LPCWSTR pszProcessNames,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ LPCWSTR pszSessionId,
            /* [out] */ DWORD *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DisableAutoAttachOnEvent( 
            /* [in] */ DWORD dwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortSupplierEx( 
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [in] */ REFGUID guidPortSupplier,
            /* [out] */ IDebugPortSupplier2 **ppPortSupplier) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortEx( 
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [in] */ REFGUID guidPort,
            /* [out] */ IDebugPort2 **ppPort) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPortsEx( 
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [out] */ IEnumDebugPorts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPortSuppliersEx( 
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [out] */ IEnumDebugPortSuppliers2 **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMachineEx2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMachineEx2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMachineEx2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMachineEx2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnableAutoAttachOnProgramCreate )( 
            IDebugMachineEx2 * This,
            /* [in] */ LPCWSTR pszProcessNames,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ LPCWSTR pszSessionId,
            /* [out] */ DWORD *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *DisableAutoAttachOnEvent )( 
            IDebugMachineEx2 * This,
            /* [in] */ DWORD dwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortSupplierEx )( 
            IDebugMachineEx2 * This,
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [in] */ REFGUID guidPortSupplier,
            /* [out] */ IDebugPortSupplier2 **ppPortSupplier);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortEx )( 
            IDebugMachineEx2 * This,
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [in] */ REFGUID guidPort,
            /* [out] */ IDebugPort2 **ppPort);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPortsEx )( 
            IDebugMachineEx2 * This,
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [out] */ IEnumDebugPorts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPortSuppliersEx )( 
            IDebugMachineEx2 * This,
            /* [in] */ LPCOLESTR wstrRegistryRoot,
            /* [out] */ IEnumDebugPortSuppliers2 **ppEnum);
        
        END_INTERFACE
    } IDebugMachineEx2Vtbl;

    interface IDebugMachineEx2
    {
        CONST_VTBL struct IDebugMachineEx2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMachineEx2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMachineEx2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMachineEx2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMachineEx2_EnableAutoAttachOnProgramCreate(This,pszProcessNames,guidEngine,pszSessionId,pdwCookie)	\
    (This)->lpVtbl -> EnableAutoAttachOnProgramCreate(This,pszProcessNames,guidEngine,pszSessionId,pdwCookie)

#define IDebugMachineEx2_DisableAutoAttachOnEvent(This,dwCookie)	\
    (This)->lpVtbl -> DisableAutoAttachOnEvent(This,dwCookie)

#define IDebugMachineEx2_GetPortSupplierEx(This,wstrRegistryRoot,guidPortSupplier,ppPortSupplier)	\
    (This)->lpVtbl -> GetPortSupplierEx(This,wstrRegistryRoot,guidPortSupplier,ppPortSupplier)

#define IDebugMachineEx2_GetPortEx(This,wstrRegistryRoot,guidPort,ppPort)	\
    (This)->lpVtbl -> GetPortEx(This,wstrRegistryRoot,guidPort,ppPort)

#define IDebugMachineEx2_EnumPortsEx(This,wstrRegistryRoot,ppEnum)	\
    (This)->lpVtbl -> EnumPortsEx(This,wstrRegistryRoot,ppEnum)

#define IDebugMachineEx2_EnumPortSuppliersEx(This,wstrRegistryRoot,ppEnum)	\
    (This)->lpVtbl -> EnumPortSuppliersEx(This,wstrRegistryRoot,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMachineEx2_EnableAutoAttachOnProgramCreate_Proxy( 
    IDebugMachineEx2 * This,
    /* [in] */ LPCWSTR pszProcessNames,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ LPCWSTR pszSessionId,
    /* [out] */ DWORD *pdwCookie);


void __RPC_STUB IDebugMachineEx2_EnableAutoAttachOnProgramCreate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachineEx2_DisableAutoAttachOnEvent_Proxy( 
    IDebugMachineEx2 * This,
    /* [in] */ DWORD dwCookie);


void __RPC_STUB IDebugMachineEx2_DisableAutoAttachOnEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachineEx2_GetPortSupplierEx_Proxy( 
    IDebugMachineEx2 * This,
    /* [in] */ LPCOLESTR wstrRegistryRoot,
    /* [in] */ REFGUID guidPortSupplier,
    /* [out] */ IDebugPortSupplier2 **ppPortSupplier);


void __RPC_STUB IDebugMachineEx2_GetPortSupplierEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachineEx2_GetPortEx_Proxy( 
    IDebugMachineEx2 * This,
    /* [in] */ LPCOLESTR wstrRegistryRoot,
    /* [in] */ REFGUID guidPort,
    /* [out] */ IDebugPort2 **ppPort);


void __RPC_STUB IDebugMachineEx2_GetPortEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachineEx2_EnumPortsEx_Proxy( 
    IDebugMachineEx2 * This,
    /* [in] */ LPCOLESTR wstrRegistryRoot,
    /* [out] */ IEnumDebugPorts2 **ppEnum);


void __RPC_STUB IDebugMachineEx2_EnumPortsEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMachineEx2_EnumPortSuppliersEx_Proxy( 
    IDebugMachineEx2 * This,
    /* [in] */ LPCOLESTR wstrRegistryRoot,
    /* [out] */ IEnumDebugPortSuppliers2 **ppEnum);


void __RPC_STUB IDebugMachineEx2_EnumPortSuppliersEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMachineEx2_INTERFACE_DEFINED__ */


#ifndef __IDebugPortSupplier2_INTERFACE_DEFINED__
#define __IDebugPortSupplier2_INTERFACE_DEFINED__

/* interface IDebugPortSupplier2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPortSupplier2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53f68191-7b2f-4f14-8e55-40b1b6e5df66")
    IDebugPortSupplier2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPortSupplierName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortSupplierId( 
            /* [out] */ GUID *pguidPortSupplier) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPort( 
            /* [in] */ REFGUID guidPort,
            /* [out] */ IDebugPort2 **ppPort) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPorts( 
            /* [out] */ IEnumDebugPorts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanAddPort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddPort( 
            /* [in] */ IDebugPortRequest2 *pRequest,
            /* [out] */ IDebugPort2 **ppPort) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemovePort( 
            /* [in] */ IDebugPort2 *pPort) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPortSupplier2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPortSupplier2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPortSupplier2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPortSupplier2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortSupplierName )( 
            IDebugPortSupplier2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortSupplierId )( 
            IDebugPortSupplier2 * This,
            /* [out] */ GUID *pguidPortSupplier);
        
        HRESULT ( STDMETHODCALLTYPE *GetPort )( 
            IDebugPortSupplier2 * This,
            /* [in] */ REFGUID guidPort,
            /* [out] */ IDebugPort2 **ppPort);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPorts )( 
            IDebugPortSupplier2 * This,
            /* [out] */ IEnumDebugPorts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *CanAddPort )( 
            IDebugPortSupplier2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPort )( 
            IDebugPortSupplier2 * This,
            /* [in] */ IDebugPortRequest2 *pRequest,
            /* [out] */ IDebugPort2 **ppPort);
        
        HRESULT ( STDMETHODCALLTYPE *RemovePort )( 
            IDebugPortSupplier2 * This,
            /* [in] */ IDebugPort2 *pPort);
        
        END_INTERFACE
    } IDebugPortSupplier2Vtbl;

    interface IDebugPortSupplier2
    {
        CONST_VTBL struct IDebugPortSupplier2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPortSupplier2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPortSupplier2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPortSupplier2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPortSupplier2_GetPortSupplierName(This,pbstrName)	\
    (This)->lpVtbl -> GetPortSupplierName(This,pbstrName)

#define IDebugPortSupplier2_GetPortSupplierId(This,pguidPortSupplier)	\
    (This)->lpVtbl -> GetPortSupplierId(This,pguidPortSupplier)

#define IDebugPortSupplier2_GetPort(This,guidPort,ppPort)	\
    (This)->lpVtbl -> GetPort(This,guidPort,ppPort)

#define IDebugPortSupplier2_EnumPorts(This,ppEnum)	\
    (This)->lpVtbl -> EnumPorts(This,ppEnum)

#define IDebugPortSupplier2_CanAddPort(This)	\
    (This)->lpVtbl -> CanAddPort(This)

#define IDebugPortSupplier2_AddPort(This,pRequest,ppPort)	\
    (This)->lpVtbl -> AddPort(This,pRequest,ppPort)

#define IDebugPortSupplier2_RemovePort(This,pPort)	\
    (This)->lpVtbl -> RemovePort(This,pPort)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_GetPortSupplierName_Proxy( 
    IDebugPortSupplier2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugPortSupplier2_GetPortSupplierName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_GetPortSupplierId_Proxy( 
    IDebugPortSupplier2 * This,
    /* [out] */ GUID *pguidPortSupplier);


void __RPC_STUB IDebugPortSupplier2_GetPortSupplierId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_GetPort_Proxy( 
    IDebugPortSupplier2 * This,
    /* [in] */ REFGUID guidPort,
    /* [out] */ IDebugPort2 **ppPort);


void __RPC_STUB IDebugPortSupplier2_GetPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_EnumPorts_Proxy( 
    IDebugPortSupplier2 * This,
    /* [out] */ IEnumDebugPorts2 **ppEnum);


void __RPC_STUB IDebugPortSupplier2_EnumPorts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_CanAddPort_Proxy( 
    IDebugPortSupplier2 * This);


void __RPC_STUB IDebugPortSupplier2_CanAddPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_AddPort_Proxy( 
    IDebugPortSupplier2 * This,
    /* [in] */ IDebugPortRequest2 *pRequest,
    /* [out] */ IDebugPort2 **ppPort);


void __RPC_STUB IDebugPortSupplier2_AddPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortSupplier2_RemovePort_Proxy( 
    IDebugPortSupplier2 * This,
    /* [in] */ IDebugPort2 *pPort);


void __RPC_STUB IDebugPortSupplier2_RemovePort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPortSupplier2_INTERFACE_DEFINED__ */


#ifndef __IDebugPort2_INTERFACE_DEFINED__
#define __IDebugPort2_INTERFACE_DEFINED__

/* interface IDebugPort2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugPort2_0001
    {	AD_PROCESS_ID_SYSTEM	= 0,
	AD_PROCESS_ID_GUID	= AD_PROCESS_ID_SYSTEM + 1
    } ;
typedef DWORD AD_PROCESS_ID_TYPE;

typedef struct _AD_PROCESS_ID
    {
    AD_PROCESS_ID_TYPE ProcessIdType;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IDebugPort2_0002
        {
        /* [case()] */ DWORD dwProcessId;
        /* [case()] */ GUID guidProcessId;
        /* [default] */ DWORD dwUnused;
        } 	ProcessId;
    } 	AD_PROCESS_ID;


EXTERN_C const IID IID_IDebugPort2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79293cc8-d9d9-43f5-97ad-0bcc5a688776")
    IDebugPort2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPortName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortId( 
            /* [out] */ GUID *pguidPort) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortRequest( 
            /* [out] */ IDebugPortRequest2 **ppRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPortSupplier( 
            /* [out] */ IDebugPortSupplier2 **ppSupplier) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProcess( 
            /* [in] */ AD_PROCESS_ID ProcessId,
            /* [out] */ IDebugProcess2 **ppProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumProcesses( 
            /* [out] */ IEnumDebugProcesses2 **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPort2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPort2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPort2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPort2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortName )( 
            IDebugPort2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortId )( 
            IDebugPort2 * This,
            /* [out] */ GUID *pguidPort);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortRequest )( 
            IDebugPort2 * This,
            /* [out] */ IDebugPortRequest2 **ppRequest);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortSupplier )( 
            IDebugPort2 * This,
            /* [out] */ IDebugPortSupplier2 **ppSupplier);
        
        HRESULT ( STDMETHODCALLTYPE *GetProcess )( 
            IDebugPort2 * This,
            /* [in] */ AD_PROCESS_ID ProcessId,
            /* [out] */ IDebugProcess2 **ppProcess);
        
        HRESULT ( STDMETHODCALLTYPE *EnumProcesses )( 
            IDebugPort2 * This,
            /* [out] */ IEnumDebugProcesses2 **ppEnum);
        
        END_INTERFACE
    } IDebugPort2Vtbl;

    interface IDebugPort2
    {
        CONST_VTBL struct IDebugPort2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPort2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPort2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPort2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPort2_GetPortName(This,pbstrName)	\
    (This)->lpVtbl -> GetPortName(This,pbstrName)

#define IDebugPort2_GetPortId(This,pguidPort)	\
    (This)->lpVtbl -> GetPortId(This,pguidPort)

#define IDebugPort2_GetPortRequest(This,ppRequest)	\
    (This)->lpVtbl -> GetPortRequest(This,ppRequest)

#define IDebugPort2_GetPortSupplier(This,ppSupplier)	\
    (This)->lpVtbl -> GetPortSupplier(This,ppSupplier)

#define IDebugPort2_GetProcess(This,ProcessId,ppProcess)	\
    (This)->lpVtbl -> GetProcess(This,ProcessId,ppProcess)

#define IDebugPort2_EnumProcesses(This,ppEnum)	\
    (This)->lpVtbl -> EnumProcesses(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPort2_GetPortName_Proxy( 
    IDebugPort2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugPort2_GetPortName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPort2_GetPortId_Proxy( 
    IDebugPort2 * This,
    /* [out] */ GUID *pguidPort);


void __RPC_STUB IDebugPort2_GetPortId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPort2_GetPortRequest_Proxy( 
    IDebugPort2 * This,
    /* [out] */ IDebugPortRequest2 **ppRequest);


void __RPC_STUB IDebugPort2_GetPortRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPort2_GetPortSupplier_Proxy( 
    IDebugPort2 * This,
    /* [out] */ IDebugPortSupplier2 **ppSupplier);


void __RPC_STUB IDebugPort2_GetPortSupplier_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPort2_GetProcess_Proxy( 
    IDebugPort2 * This,
    /* [in] */ AD_PROCESS_ID ProcessId,
    /* [out] */ IDebugProcess2 **ppProcess);


void __RPC_STUB IDebugPort2_GetProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPort2_EnumProcesses_Proxy( 
    IDebugPort2 * This,
    /* [out] */ IEnumDebugProcesses2 **ppEnum);


void __RPC_STUB IDebugPort2_EnumProcesses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPort2_INTERFACE_DEFINED__ */


#ifndef __IDebugPortRequest2_INTERFACE_DEFINED__
#define __IDebugPortRequest2_INTERFACE_DEFINED__

/* interface IDebugPortRequest2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPortRequest2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8d36beb8-9bfe-47dd-a11b-7ba1de18e449")
    IDebugPortRequest2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPortName( 
            /* [out] */ BSTR *pbstrPortName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPortRequest2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPortRequest2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPortRequest2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPortRequest2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPortName )( 
            IDebugPortRequest2 * This,
            /* [out] */ BSTR *pbstrPortName);
        
        END_INTERFACE
    } IDebugPortRequest2Vtbl;

    interface IDebugPortRequest2
    {
        CONST_VTBL struct IDebugPortRequest2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPortRequest2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPortRequest2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPortRequest2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPortRequest2_GetPortName(This,pbstrPortName)	\
    (This)->lpVtbl -> GetPortName(This,pbstrPortName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPortRequest2_GetPortName_Proxy( 
    IDebugPortRequest2 * This,
    /* [out] */ BSTR *pbstrPortName);


void __RPC_STUB IDebugPortRequest2_GetPortName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPortRequest2_INTERFACE_DEFINED__ */


#ifndef __IDebugPortNotify2_INTERFACE_DEFINED__
#define __IDebugPortNotify2_INTERFACE_DEFINED__

/* interface IDebugPortNotify2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPortNotify2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fb8d2032-2858-414c-83d9-f732664e0c7a")
    IDebugPortNotify2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddProgramNode( 
            /* [in] */ IDebugProgramNode2 *pProgramNode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveProgramNode( 
            /* [in] */ IDebugProgramNode2 *pProgramNode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPortNotify2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPortNotify2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPortNotify2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPortNotify2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddProgramNode )( 
            IDebugPortNotify2 * This,
            /* [in] */ IDebugProgramNode2 *pProgramNode);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveProgramNode )( 
            IDebugPortNotify2 * This,
            /* [in] */ IDebugProgramNode2 *pProgramNode);
        
        END_INTERFACE
    } IDebugPortNotify2Vtbl;

    interface IDebugPortNotify2
    {
        CONST_VTBL struct IDebugPortNotify2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPortNotify2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPortNotify2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPortNotify2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPortNotify2_AddProgramNode(This,pProgramNode)	\
    (This)->lpVtbl -> AddProgramNode(This,pProgramNode)

#define IDebugPortNotify2_RemoveProgramNode(This,pProgramNode)	\
    (This)->lpVtbl -> RemoveProgramNode(This,pProgramNode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPortNotify2_AddProgramNode_Proxy( 
    IDebugPortNotify2 * This,
    /* [in] */ IDebugProgramNode2 *pProgramNode);


void __RPC_STUB IDebugPortNotify2_AddProgramNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPortNotify2_RemoveProgramNode_Proxy( 
    IDebugPortNotify2 * This,
    /* [in] */ IDebugProgramNode2 *pProgramNode);


void __RPC_STUB IDebugPortNotify2_RemoveProgramNode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPortNotify2_INTERFACE_DEFINED__ */


#ifndef __IDebugPortEvents2_INTERFACE_DEFINED__
#define __IDebugPortEvents2_INTERFACE_DEFINED__

/* interface IDebugPortEvents2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPortEvents2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("564fa275-12e1-4b5f-8316-4d79bcef7246")
    IDebugPortEvents2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Event( 
            /* [in] */ IDebugMachine2 *pMachine,
            /* [in] */ IDebugPort2 *pPort,
            /* [in] */ IDebugProcess2 *pProcess,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [in] */ IDebugEvent2 *pEvent,
            /* [in] */ REFIID riidEvent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPortEvents2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPortEvents2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPortEvents2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPortEvents2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Event )( 
            IDebugPortEvents2 * This,
            /* [in] */ IDebugMachine2 *pMachine,
            /* [in] */ IDebugPort2 *pPort,
            /* [in] */ IDebugProcess2 *pProcess,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [in] */ IDebugEvent2 *pEvent,
            /* [in] */ REFIID riidEvent);
        
        END_INTERFACE
    } IDebugPortEvents2Vtbl;

    interface IDebugPortEvents2
    {
        CONST_VTBL struct IDebugPortEvents2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPortEvents2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPortEvents2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPortEvents2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPortEvents2_Event(This,pMachine,pPort,pProcess,pProgram,pEvent,riidEvent)	\
    (This)->lpVtbl -> Event(This,pMachine,pPort,pProcess,pProgram,pEvent,riidEvent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPortEvents2_Event_Proxy( 
    IDebugPortEvents2 * This,
    /* [in] */ IDebugMachine2 *pMachine,
    /* [in] */ IDebugPort2 *pPort,
    /* [in] */ IDebugProcess2 *pProcess,
    /* [in] */ IDebugProgram2 *pProgram,
    /* [in] */ IDebugEvent2 *pEvent,
    /* [in] */ REFIID riidEvent);


void __RPC_STUB IDebugPortEvents2_Event_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPortEvents2_INTERFACE_DEFINED__ */


#ifndef __IDebugMDMUtil2_INTERFACE_DEFINED__
#define __IDebugMDMUtil2_INTERFACE_DEFINED__

/* interface IDebugMDMUtil2 */
/* [unique][uuid][object] */ 

typedef DWORD DYNDEBUGFLAGS;


enum __MIDL_IDebugMDMUtil2_0001
    {	DYNDEBUG_ATTACH	= 1,
	DYNDEBUG_JIT	= 2,
	DYNDEBUG_REMOTEJIT	= 4
    } ;
#define	S_UNKNOWN	( 0x3 )


EXTERN_C const IID IID_IDebugMDMUtil2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f3062547-43d8-4dc2-b18e-e1460ff2c422")
    IDebugMDMUtil2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddPIDToIgnore( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemovePIDToIgnore( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddPIDToDebug( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemovePIDToDebug( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDynamicDebuggingFlags( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DYNDEBUGFLAGS dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDynamicDebuggingFlags( 
            /* [in] */ REFGUID guidEngine,
            /* [out] */ DYNDEBUGFLAGS *pdwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDefaultJITServer( 
            /* [in] */ REFCLSID clsidJITServer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultJITServer( 
            /* [out] */ CLSID *pClsidJITServer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterJITDebugEngines( 
            /* [in] */ REFCLSID clsidJITServer,
            /* [size_is][in] */ GUID *arrguidEngines,
            /* [size_is][full][in] */ BOOL *arrRemoteFlags,
            /* [in] */ DWORD celtEngs,
            /* [in] */ BOOL fRegister) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanDebugPID( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD pid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMDMUtil2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMDMUtil2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMDMUtil2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPIDToIgnore )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid);
        
        HRESULT ( STDMETHODCALLTYPE *RemovePIDToIgnore )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid);
        
        HRESULT ( STDMETHODCALLTYPE *AddPIDToDebug )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid);
        
        HRESULT ( STDMETHODCALLTYPE *RemovePIDToDebug )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD dwPid);
        
        HRESULT ( STDMETHODCALLTYPE *SetDynamicDebuggingFlags )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DYNDEBUGFLAGS dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetDynamicDebuggingFlags )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [out] */ DYNDEBUGFLAGS *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetDefaultJITServer )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFCLSID clsidJITServer);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultJITServer )( 
            IDebugMDMUtil2 * This,
            /* [out] */ CLSID *pClsidJITServer);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterJITDebugEngines )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFCLSID clsidJITServer,
            /* [size_is][in] */ GUID *arrguidEngines,
            /* [size_is][full][in] */ BOOL *arrRemoteFlags,
            /* [in] */ DWORD celtEngs,
            /* [in] */ BOOL fRegister);
        
        HRESULT ( STDMETHODCALLTYPE *CanDebugPID )( 
            IDebugMDMUtil2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ DWORD pid);
        
        END_INTERFACE
    } IDebugMDMUtil2Vtbl;

    interface IDebugMDMUtil2
    {
        CONST_VTBL struct IDebugMDMUtil2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMDMUtil2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMDMUtil2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMDMUtil2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMDMUtil2_AddPIDToIgnore(This,guidEngine,dwPid)	\
    (This)->lpVtbl -> AddPIDToIgnore(This,guidEngine,dwPid)

#define IDebugMDMUtil2_RemovePIDToIgnore(This,guidEngine,dwPid)	\
    (This)->lpVtbl -> RemovePIDToIgnore(This,guidEngine,dwPid)

#define IDebugMDMUtil2_AddPIDToDebug(This,guidEngine,dwPid)	\
    (This)->lpVtbl -> AddPIDToDebug(This,guidEngine,dwPid)

#define IDebugMDMUtil2_RemovePIDToDebug(This,guidEngine,dwPid)	\
    (This)->lpVtbl -> RemovePIDToDebug(This,guidEngine,dwPid)

#define IDebugMDMUtil2_SetDynamicDebuggingFlags(This,guidEngine,dwFlags)	\
    (This)->lpVtbl -> SetDynamicDebuggingFlags(This,guidEngine,dwFlags)

#define IDebugMDMUtil2_GetDynamicDebuggingFlags(This,guidEngine,pdwFlags)	\
    (This)->lpVtbl -> GetDynamicDebuggingFlags(This,guidEngine,pdwFlags)

#define IDebugMDMUtil2_SetDefaultJITServer(This,clsidJITServer)	\
    (This)->lpVtbl -> SetDefaultJITServer(This,clsidJITServer)

#define IDebugMDMUtil2_GetDefaultJITServer(This,pClsidJITServer)	\
    (This)->lpVtbl -> GetDefaultJITServer(This,pClsidJITServer)

#define IDebugMDMUtil2_RegisterJITDebugEngines(This,clsidJITServer,arrguidEngines,arrRemoteFlags,celtEngs,fRegister)	\
    (This)->lpVtbl -> RegisterJITDebugEngines(This,clsidJITServer,arrguidEngines,arrRemoteFlags,celtEngs,fRegister)

#define IDebugMDMUtil2_CanDebugPID(This,guidEngine,pid)	\
    (This)->lpVtbl -> CanDebugPID(This,guidEngine,pid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_AddPIDToIgnore_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ DWORD dwPid);


void __RPC_STUB IDebugMDMUtil2_AddPIDToIgnore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_RemovePIDToIgnore_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ DWORD dwPid);


void __RPC_STUB IDebugMDMUtil2_RemovePIDToIgnore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_AddPIDToDebug_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ DWORD dwPid);


void __RPC_STUB IDebugMDMUtil2_AddPIDToDebug_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_RemovePIDToDebug_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ DWORD dwPid);


void __RPC_STUB IDebugMDMUtil2_RemovePIDToDebug_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_SetDynamicDebuggingFlags_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ DYNDEBUGFLAGS dwFlags);


void __RPC_STUB IDebugMDMUtil2_SetDynamicDebuggingFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_GetDynamicDebuggingFlags_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [out] */ DYNDEBUGFLAGS *pdwFlags);


void __RPC_STUB IDebugMDMUtil2_GetDynamicDebuggingFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_SetDefaultJITServer_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFCLSID clsidJITServer);


void __RPC_STUB IDebugMDMUtil2_SetDefaultJITServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_GetDefaultJITServer_Proxy( 
    IDebugMDMUtil2 * This,
    /* [out] */ CLSID *pClsidJITServer);


void __RPC_STUB IDebugMDMUtil2_GetDefaultJITServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_RegisterJITDebugEngines_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFCLSID clsidJITServer,
    /* [size_is][in] */ GUID *arrguidEngines,
    /* [size_is][full][in] */ BOOL *arrRemoteFlags,
    /* [in] */ DWORD celtEngs,
    /* [in] */ BOOL fRegister);


void __RPC_STUB IDebugMDMUtil2_RegisterJITDebugEngines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMDMUtil2_CanDebugPID_Proxy( 
    IDebugMDMUtil2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ DWORD pid);


void __RPC_STUB IDebugMDMUtil2_CanDebugPID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMDMUtil2_INTERFACE_DEFINED__ */


#ifndef __IDebugSession2_INTERFACE_DEFINED__
#define __IDebugSession2_INTERFACE_DEFINED__

/* interface IDebugSession2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugSession2_0001
    {	LAUNCH_DEBUG	= 0,
	LAUNCH_NODEBUG	= 0x1,
	LAUNCH_ENABLE_ENC	= 0x2
    } ;
typedef DWORD LAUNCH_FLAGS;


enum __MIDL_IDebugSession2_0002
    {	EXCEPTION_NONE	= 0,
	EXCEPTION_STOP_FIRST_CHANCE	= 0x1,
	EXCEPTION_STOP_SECOND_CHANCE	= 0x2,
	EXCEPTION_STOP_FIRST_CHANCE_USE_PARENT	= 0x4,
	EXCEPTION_STOP_SECOND_CHANCE_USE_PARENT	= 0x8
    } ;
typedef DWORD EXCEPTION_STATE;

typedef struct tagEXCEPTION_INFO
    {
    IDebugProgram2 *pProgram;
    BSTR bstrProgramName;
    BSTR bstrExceptionName;
    DWORD dwCode;
    EXCEPTION_STATE dwState;
    GUID guidType;
    } 	EXCEPTION_INFO;


enum __MIDL_IDebugSession2_0003
    {	STOPPING_MODEL_STOP_ALL	= 0x1,
	STOPPING_MODEL_STOP_ONE	= 0x2
    } ;
typedef DWORD STOPPING_MODEL;


EXTERN_C const IID IID_IDebugSession2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8948300f-8bd5-4728-a1d8-83d172295a9d")
    IDebugSession2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetName( 
            /* [in] */ LPOLESTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumProcesses( 
            /* [out] */ IEnumDebugProcesses2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Launch( 
            /* [full][in] */ LPCOLESTR pszMachine,
            /* [in] */ IDebugPort2 *pPort,
            /* [full][in] */ LPCOLESTR pszExe,
            /* [full][in] */ LPCOLESTR pszArgs,
            /* [full][in] */ LPCOLESTR pszDir,
            /* [full][in] */ BSTR bstrEnv,
            /* [full][in] */ LPCOLESTR pszOptions,
            /* [in] */ LAUNCH_FLAGS dwLaunchFlags,
            /* [in] */ DWORD hStdInput,
            /* [in] */ DWORD hStdOutput,
            /* [in] */ DWORD hStdError,
            /* [in] */ REFGUID guidLaunchingEngine,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [size_is][in] */ GUID *rgguidSpecificEngines,
            /* [in] */ DWORD celtSpecificEngines,
            /* [out] */ IDebugProcess2 **ppProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterJITServer( 
            /* [in] */ REFCLSID clsidJITServer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Terminate( 
            /* [in] */ BOOL fForce) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Detach( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CauseBreak( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreatePendingBreakpoint( 
            /* [in] */ IDebugBreakpointRequest2 *pBPRequest,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPendingBreakpoints( 
            /* [in] */ IDebugProgram2 *pProgram,
            /* [full][in] */ LPCOLESTR pszProgram,
            /* [out] */ IEnumDebugPendingBreakpoints2 **ppEnumBPs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumMachines( 
            /* [out] */ IEnumDebugMachines2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddMachine( 
            /* [full][in] */ LPCOLESTR pszMachine,
            /* [out] */ IDebugMachine2 **ppMachine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveMachine( 
            /* [in] */ IDebugMachine2 *pMachine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShutdownSession( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCodeContexts( 
            /* [in] */ IDebugProgram2 *pProgram,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetException( 
            /* [in] */ EXCEPTION_INFO *pException) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumSetExceptions( 
            /* [in] */ IDebugProgram2 *pProgram,
            /* [full][in] */ LPCOLESTR pszProgram,
            /* [in] */ REFGUID guidType,
            /* [out] */ IEnumDebugExceptionInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveSetException( 
            /* [in] */ EXCEPTION_INFO *pException) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAllSetExceptions( 
            /* [in] */ REFGUID guidType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumDefaultExceptions( 
            /* [full][in] */ EXCEPTION_INFO *pParentException,
            /* [out] */ IEnumDebugExceptionInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetENCUpdate( 
            /* [in] */ IDebugProgram2 *pProgram,
            /* [out] */ IDebugENCUpdate **ppUpdate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLocale( 
            /* [in] */ WORD wLangID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRegistryRoot( 
            /* [full][in] */ LPCOLESTR pszRegistryRoot) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsAlive( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearAllSessionThreadStackFrames( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSessionId( 
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [full][size_is][in] */ GUID *rgguidSpecificEngines,
            /* [in] */ DWORD celtSpecificEngines,
            /* [full][in] */ LPCOLESTR pszStartPageUrl,
            /* [out] */ BSTR *pbstrSessionId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEngineMetric( 
            /* [in] */ REFGUID guidEngine,
            /* [in] */ LPCOLESTR pszMetric,
            /* [in] */ VARIANT varValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStoppingModel( 
            /* [in] */ STOPPING_MODEL dwStoppingModel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStoppingModel( 
            /* [out] */ STOPPING_MODEL *pdwStoppingModel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterSessionWithServer( 
            /* [in] */ LPCOLESTR pwszServerName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugSession2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugSession2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugSession2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugSession2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugSession2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *SetName )( 
            IDebugSession2 * This,
            /* [in] */ LPOLESTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *EnumProcesses )( 
            IDebugSession2 * This,
            /* [out] */ IEnumDebugProcesses2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *Launch )( 
            IDebugSession2 * This,
            /* [full][in] */ LPCOLESTR pszMachine,
            /* [in] */ IDebugPort2 *pPort,
            /* [full][in] */ LPCOLESTR pszExe,
            /* [full][in] */ LPCOLESTR pszArgs,
            /* [full][in] */ LPCOLESTR pszDir,
            /* [full][in] */ BSTR bstrEnv,
            /* [full][in] */ LPCOLESTR pszOptions,
            /* [in] */ LAUNCH_FLAGS dwLaunchFlags,
            /* [in] */ DWORD hStdInput,
            /* [in] */ DWORD hStdOutput,
            /* [in] */ DWORD hStdError,
            /* [in] */ REFGUID guidLaunchingEngine,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [size_is][in] */ GUID *rgguidSpecificEngines,
            /* [in] */ DWORD celtSpecificEngines,
            /* [out] */ IDebugProcess2 **ppProcess);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterJITServer )( 
            IDebugSession2 * This,
            /* [in] */ REFCLSID clsidJITServer);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            IDebugSession2 * This,
            /* [in] */ BOOL fForce);
        
        HRESULT ( STDMETHODCALLTYPE *Detach )( 
            IDebugSession2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CauseBreak )( 
            IDebugSession2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreatePendingBreakpoint )( 
            IDebugSession2 * This,
            /* [in] */ IDebugBreakpointRequest2 *pBPRequest,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPendingBreakpoints )( 
            IDebugSession2 * This,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [full][in] */ LPCOLESTR pszProgram,
            /* [out] */ IEnumDebugPendingBreakpoints2 **ppEnumBPs);
        
        HRESULT ( STDMETHODCALLTYPE *EnumMachines )( 
            IDebugSession2 * This,
            /* [out] */ IEnumDebugMachines2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *AddMachine )( 
            IDebugSession2 * This,
            /* [full][in] */ LPCOLESTR pszMachine,
            /* [out] */ IDebugMachine2 **ppMachine);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveMachine )( 
            IDebugSession2 * This,
            /* [in] */ IDebugMachine2 *pMachine);
        
        HRESULT ( STDMETHODCALLTYPE *ShutdownSession )( 
            IDebugSession2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCodeContexts )( 
            IDebugSession2 * This,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *SetException )( 
            IDebugSession2 * This,
            /* [in] */ EXCEPTION_INFO *pException);
        
        HRESULT ( STDMETHODCALLTYPE *EnumSetExceptions )( 
            IDebugSession2 * This,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [full][in] */ LPCOLESTR pszProgram,
            /* [in] */ REFGUID guidType,
            /* [out] */ IEnumDebugExceptionInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveSetException )( 
            IDebugSession2 * This,
            /* [in] */ EXCEPTION_INFO *pException);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAllSetExceptions )( 
            IDebugSession2 * This,
            /* [in] */ REFGUID guidType);
        
        HRESULT ( STDMETHODCALLTYPE *EnumDefaultExceptions )( 
            IDebugSession2 * This,
            /* [full][in] */ EXCEPTION_INFO *pParentException,
            /* [out] */ IEnumDebugExceptionInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetENCUpdate )( 
            IDebugSession2 * This,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [out] */ IDebugENCUpdate **ppUpdate);
        
        HRESULT ( STDMETHODCALLTYPE *SetLocale )( 
            IDebugSession2 * This,
            /* [in] */ WORD wLangID);
        
        HRESULT ( STDMETHODCALLTYPE *SetRegistryRoot )( 
            IDebugSession2 * This,
            /* [full][in] */ LPCOLESTR pszRegistryRoot);
        
        HRESULT ( STDMETHODCALLTYPE *IsAlive )( 
            IDebugSession2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *ClearAllSessionThreadStackFrames )( 
            IDebugSession2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSessionId )( 
            IDebugSession2 * This,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [full][size_is][in] */ GUID *rgguidSpecificEngines,
            /* [in] */ DWORD celtSpecificEngines,
            /* [full][in] */ LPCOLESTR pszStartPageUrl,
            /* [out] */ BSTR *pbstrSessionId);
        
        HRESULT ( STDMETHODCALLTYPE *SetEngineMetric )( 
            IDebugSession2 * This,
            /* [in] */ REFGUID guidEngine,
            /* [in] */ LPCOLESTR pszMetric,
            /* [in] */ VARIANT varValue);
        
        HRESULT ( STDMETHODCALLTYPE *SetStoppingModel )( 
            IDebugSession2 * This,
            /* [in] */ STOPPING_MODEL dwStoppingModel);
        
        HRESULT ( STDMETHODCALLTYPE *GetStoppingModel )( 
            IDebugSession2 * This,
            /* [out] */ STOPPING_MODEL *pdwStoppingModel);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterSessionWithServer )( 
            IDebugSession2 * This,
            /* [in] */ LPCOLESTR pwszServerName);
        
        END_INTERFACE
    } IDebugSession2Vtbl;

    interface IDebugSession2
    {
        CONST_VTBL struct IDebugSession2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugSession2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugSession2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugSession2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugSession2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugSession2_SetName(This,pszName)	\
    (This)->lpVtbl -> SetName(This,pszName)

#define IDebugSession2_EnumProcesses(This,ppEnum)	\
    (This)->lpVtbl -> EnumProcesses(This,ppEnum)

#define IDebugSession2_Launch(This,pszMachine,pPort,pszExe,pszArgs,pszDir,bstrEnv,pszOptions,dwLaunchFlags,hStdInput,hStdOutput,hStdError,guidLaunchingEngine,pCallback,rgguidSpecificEngines,celtSpecificEngines,ppProcess)	\
    (This)->lpVtbl -> Launch(This,pszMachine,pPort,pszExe,pszArgs,pszDir,bstrEnv,pszOptions,dwLaunchFlags,hStdInput,hStdOutput,hStdError,guidLaunchingEngine,pCallback,rgguidSpecificEngines,celtSpecificEngines,ppProcess)

#define IDebugSession2_RegisterJITServer(This,clsidJITServer)	\
    (This)->lpVtbl -> RegisterJITServer(This,clsidJITServer)

#define IDebugSession2_Terminate(This,fForce)	\
    (This)->lpVtbl -> Terminate(This,fForce)

#define IDebugSession2_Detach(This)	\
    (This)->lpVtbl -> Detach(This)

#define IDebugSession2_CauseBreak(This)	\
    (This)->lpVtbl -> CauseBreak(This)

#define IDebugSession2_CreatePendingBreakpoint(This,pBPRequest,ppPendingBP)	\
    (This)->lpVtbl -> CreatePendingBreakpoint(This,pBPRequest,ppPendingBP)

#define IDebugSession2_EnumPendingBreakpoints(This,pProgram,pszProgram,ppEnumBPs)	\
    (This)->lpVtbl -> EnumPendingBreakpoints(This,pProgram,pszProgram,ppEnumBPs)

#define IDebugSession2_EnumMachines(This,ppEnum)	\
    (This)->lpVtbl -> EnumMachines(This,ppEnum)

#define IDebugSession2_AddMachine(This,pszMachine,ppMachine)	\
    (This)->lpVtbl -> AddMachine(This,pszMachine,ppMachine)

#define IDebugSession2_RemoveMachine(This,pMachine)	\
    (This)->lpVtbl -> RemoveMachine(This,pMachine)

#define IDebugSession2_ShutdownSession(This)	\
    (This)->lpVtbl -> ShutdownSession(This)

#define IDebugSession2_EnumCodeContexts(This,pProgram,pDocPos,ppEnum)	\
    (This)->lpVtbl -> EnumCodeContexts(This,pProgram,pDocPos,ppEnum)

#define IDebugSession2_SetException(This,pException)	\
    (This)->lpVtbl -> SetException(This,pException)

#define IDebugSession2_EnumSetExceptions(This,pProgram,pszProgram,guidType,ppEnum)	\
    (This)->lpVtbl -> EnumSetExceptions(This,pProgram,pszProgram,guidType,ppEnum)

#define IDebugSession2_RemoveSetException(This,pException)	\
    (This)->lpVtbl -> RemoveSetException(This,pException)

#define IDebugSession2_RemoveAllSetExceptions(This,guidType)	\
    (This)->lpVtbl -> RemoveAllSetExceptions(This,guidType)

#define IDebugSession2_EnumDefaultExceptions(This,pParentException,ppEnum)	\
    (This)->lpVtbl -> EnumDefaultExceptions(This,pParentException,ppEnum)

#define IDebugSession2_GetENCUpdate(This,pProgram,ppUpdate)	\
    (This)->lpVtbl -> GetENCUpdate(This,pProgram,ppUpdate)

#define IDebugSession2_SetLocale(This,wLangID)	\
    (This)->lpVtbl -> SetLocale(This,wLangID)

#define IDebugSession2_SetRegistryRoot(This,pszRegistryRoot)	\
    (This)->lpVtbl -> SetRegistryRoot(This,pszRegistryRoot)

#define IDebugSession2_IsAlive(This)	\
    (This)->lpVtbl -> IsAlive(This)

#define IDebugSession2_ClearAllSessionThreadStackFrames(This)	\
    (This)->lpVtbl -> ClearAllSessionThreadStackFrames(This)

#define IDebugSession2_GetSessionId(This,pCallback,rgguidSpecificEngines,celtSpecificEngines,pszStartPageUrl,pbstrSessionId)	\
    (This)->lpVtbl -> GetSessionId(This,pCallback,rgguidSpecificEngines,celtSpecificEngines,pszStartPageUrl,pbstrSessionId)

#define IDebugSession2_SetEngineMetric(This,guidEngine,pszMetric,varValue)	\
    (This)->lpVtbl -> SetEngineMetric(This,guidEngine,pszMetric,varValue)

#define IDebugSession2_SetStoppingModel(This,dwStoppingModel)	\
    (This)->lpVtbl -> SetStoppingModel(This,dwStoppingModel)

#define IDebugSession2_GetStoppingModel(This,pdwStoppingModel)	\
    (This)->lpVtbl -> GetStoppingModel(This,pdwStoppingModel)

#define IDebugSession2_RegisterSessionWithServer(This,pwszServerName)	\
    (This)->lpVtbl -> RegisterSessionWithServer(This,pwszServerName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugSession2_GetName_Proxy( 
    IDebugSession2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugSession2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_SetName_Proxy( 
    IDebugSession2 * This,
    /* [in] */ LPOLESTR pszName);


void __RPC_STUB IDebugSession2_SetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_EnumProcesses_Proxy( 
    IDebugSession2 * This,
    /* [out] */ IEnumDebugProcesses2 **ppEnum);


void __RPC_STUB IDebugSession2_EnumProcesses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_Launch_Proxy( 
    IDebugSession2 * This,
    /* [full][in] */ LPCOLESTR pszMachine,
    /* [in] */ IDebugPort2 *pPort,
    /* [full][in] */ LPCOLESTR pszExe,
    /* [full][in] */ LPCOLESTR pszArgs,
    /* [full][in] */ LPCOLESTR pszDir,
    /* [full][in] */ BSTR bstrEnv,
    /* [full][in] */ LPCOLESTR pszOptions,
    /* [in] */ LAUNCH_FLAGS dwLaunchFlags,
    /* [in] */ DWORD hStdInput,
    /* [in] */ DWORD hStdOutput,
    /* [in] */ DWORD hStdError,
    /* [in] */ REFGUID guidLaunchingEngine,
    /* [in] */ IDebugEventCallback2 *pCallback,
    /* [size_is][in] */ GUID *rgguidSpecificEngines,
    /* [in] */ DWORD celtSpecificEngines,
    /* [out] */ IDebugProcess2 **ppProcess);


void __RPC_STUB IDebugSession2_Launch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_RegisterJITServer_Proxy( 
    IDebugSession2 * This,
    /* [in] */ REFCLSID clsidJITServer);


void __RPC_STUB IDebugSession2_RegisterJITServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_Terminate_Proxy( 
    IDebugSession2 * This,
    /* [in] */ BOOL fForce);


void __RPC_STUB IDebugSession2_Terminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_Detach_Proxy( 
    IDebugSession2 * This);


void __RPC_STUB IDebugSession2_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_CauseBreak_Proxy( 
    IDebugSession2 * This);


void __RPC_STUB IDebugSession2_CauseBreak_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_CreatePendingBreakpoint_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugBreakpointRequest2 *pBPRequest,
    /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP);


void __RPC_STUB IDebugSession2_CreatePendingBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_EnumPendingBreakpoints_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugProgram2 *pProgram,
    /* [full][in] */ LPCOLESTR pszProgram,
    /* [out] */ IEnumDebugPendingBreakpoints2 **ppEnumBPs);


void __RPC_STUB IDebugSession2_EnumPendingBreakpoints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_EnumMachines_Proxy( 
    IDebugSession2 * This,
    /* [out] */ IEnumDebugMachines2 **ppEnum);


void __RPC_STUB IDebugSession2_EnumMachines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_AddMachine_Proxy( 
    IDebugSession2 * This,
    /* [full][in] */ LPCOLESTR pszMachine,
    /* [out] */ IDebugMachine2 **ppMachine);


void __RPC_STUB IDebugSession2_AddMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_RemoveMachine_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugMachine2 *pMachine);


void __RPC_STUB IDebugSession2_RemoveMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_ShutdownSession_Proxy( 
    IDebugSession2 * This);


void __RPC_STUB IDebugSession2_ShutdownSession_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_EnumCodeContexts_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugProgram2 *pProgram,
    /* [in] */ IDebugDocumentPosition2 *pDocPos,
    /* [out] */ IEnumDebugCodeContexts2 **ppEnum);


void __RPC_STUB IDebugSession2_EnumCodeContexts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_SetException_Proxy( 
    IDebugSession2 * This,
    /* [in] */ EXCEPTION_INFO *pException);


void __RPC_STUB IDebugSession2_SetException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_EnumSetExceptions_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugProgram2 *pProgram,
    /* [full][in] */ LPCOLESTR pszProgram,
    /* [in] */ REFGUID guidType,
    /* [out] */ IEnumDebugExceptionInfo2 **ppEnum);


void __RPC_STUB IDebugSession2_EnumSetExceptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_RemoveSetException_Proxy( 
    IDebugSession2 * This,
    /* [in] */ EXCEPTION_INFO *pException);


void __RPC_STUB IDebugSession2_RemoveSetException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_RemoveAllSetExceptions_Proxy( 
    IDebugSession2 * This,
    /* [in] */ REFGUID guidType);


void __RPC_STUB IDebugSession2_RemoveAllSetExceptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_EnumDefaultExceptions_Proxy( 
    IDebugSession2 * This,
    /* [full][in] */ EXCEPTION_INFO *pParentException,
    /* [out] */ IEnumDebugExceptionInfo2 **ppEnum);


void __RPC_STUB IDebugSession2_EnumDefaultExceptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_GetENCUpdate_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugProgram2 *pProgram,
    /* [out] */ IDebugENCUpdate **ppUpdate);


void __RPC_STUB IDebugSession2_GetENCUpdate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_SetLocale_Proxy( 
    IDebugSession2 * This,
    /* [in] */ WORD wLangID);


void __RPC_STUB IDebugSession2_SetLocale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_SetRegistryRoot_Proxy( 
    IDebugSession2 * This,
    /* [full][in] */ LPCOLESTR pszRegistryRoot);


void __RPC_STUB IDebugSession2_SetRegistryRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_IsAlive_Proxy( 
    IDebugSession2 * This);


void __RPC_STUB IDebugSession2_IsAlive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_ClearAllSessionThreadStackFrames_Proxy( 
    IDebugSession2 * This);


void __RPC_STUB IDebugSession2_ClearAllSessionThreadStackFrames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_GetSessionId_Proxy( 
    IDebugSession2 * This,
    /* [in] */ IDebugEventCallback2 *pCallback,
    /* [full][size_is][in] */ GUID *rgguidSpecificEngines,
    /* [in] */ DWORD celtSpecificEngines,
    /* [full][in] */ LPCOLESTR pszStartPageUrl,
    /* [out] */ BSTR *pbstrSessionId);


void __RPC_STUB IDebugSession2_GetSessionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_SetEngineMetric_Proxy( 
    IDebugSession2 * This,
    /* [in] */ REFGUID guidEngine,
    /* [in] */ LPCOLESTR pszMetric,
    /* [in] */ VARIANT varValue);


void __RPC_STUB IDebugSession2_SetEngineMetric_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_SetStoppingModel_Proxy( 
    IDebugSession2 * This,
    /* [in] */ STOPPING_MODEL dwStoppingModel);


void __RPC_STUB IDebugSession2_SetStoppingModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_GetStoppingModel_Proxy( 
    IDebugSession2 * This,
    /* [out] */ STOPPING_MODEL *pdwStoppingModel);


void __RPC_STUB IDebugSession2_GetStoppingModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugSession2_RegisterSessionWithServer_Proxy( 
    IDebugSession2 * This,
    /* [in] */ LPCOLESTR pwszServerName);


void __RPC_STUB IDebugSession2_RegisterSessionWithServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugSession2_INTERFACE_DEFINED__ */


#ifndef __IDebugEngine2_INTERFACE_DEFINED__
#define __IDebugEngine2_INTERFACE_DEFINED__

/* interface IDebugEngine2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugEngine2_0001
    {	ATTACH_REASON_LAUNCH	= 0x1,
	ATTACH_REASON_USER	= 0x2,
	ATTACH_REASON_AUTO	= 0x3
    } ;
typedef DWORD ATTACH_REASON;


EXTERN_C const IID IID_IDebugEngine2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ba105b52-12f1-4038-ae64-d95785874c47")
    IDebugEngine2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumPrograms( 
            /* [out] */ IEnumDebugPrograms2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Attach( 
            /* [length_is][size_is][in] */ IDebugProgram2 **rgpPrograms,
            /* [length_is][size_is][in] */ IDebugProgramNode2 **rgpProgramNodes,
            /* [in] */ DWORD celtPrograms,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [in] */ ATTACH_REASON dwReason) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreatePendingBreakpoint( 
            /* [in] */ IDebugBreakpointRequest2 *pBPRequest,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetException( 
            /* [in] */ EXCEPTION_INFO *pException) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveSetException( 
            /* [in] */ EXCEPTION_INFO *pException) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAllSetExceptions( 
            /* [in] */ REFGUID guidType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEngineId( 
            /* [out] */ GUID *pguidEngine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyProgram( 
            /* [in] */ IDebugProgram2 *pProgram) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ContinueFromSynchronousEvent( 
            /* [in] */ IDebugEvent2 *pEvent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLocale( 
            /* [in] */ WORD wLangID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRegistryRoot( 
            /* [full][in] */ LPCOLESTR pszRegistryRoot) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMetric( 
            /* [in] */ LPCOLESTR pszMetric,
            /* [in] */ VARIANT varValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CauseBreak( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEngine2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEngine2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEngine2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEngine2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPrograms )( 
            IDebugEngine2 * This,
            /* [out] */ IEnumDebugPrograms2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *Attach )( 
            IDebugEngine2 * This,
            /* [length_is][size_is][in] */ IDebugProgram2 **rgpPrograms,
            /* [length_is][size_is][in] */ IDebugProgramNode2 **rgpProgramNodes,
            /* [in] */ DWORD celtPrograms,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [in] */ ATTACH_REASON dwReason);
        
        HRESULT ( STDMETHODCALLTYPE *CreatePendingBreakpoint )( 
            IDebugEngine2 * This,
            /* [in] */ IDebugBreakpointRequest2 *pBPRequest,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP);
        
        HRESULT ( STDMETHODCALLTYPE *SetException )( 
            IDebugEngine2 * This,
            /* [in] */ EXCEPTION_INFO *pException);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveSetException )( 
            IDebugEngine2 * This,
            /* [in] */ EXCEPTION_INFO *pException);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAllSetExceptions )( 
            IDebugEngine2 * This,
            /* [in] */ REFGUID guidType);
        
        HRESULT ( STDMETHODCALLTYPE *GetEngineId )( 
            IDebugEngine2 * This,
            /* [out] */ GUID *pguidEngine);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyProgram )( 
            IDebugEngine2 * This,
            /* [in] */ IDebugProgram2 *pProgram);
        
        HRESULT ( STDMETHODCALLTYPE *ContinueFromSynchronousEvent )( 
            IDebugEngine2 * This,
            /* [in] */ IDebugEvent2 *pEvent);
        
        HRESULT ( STDMETHODCALLTYPE *SetLocale )( 
            IDebugEngine2 * This,
            /* [in] */ WORD wLangID);
        
        HRESULT ( STDMETHODCALLTYPE *SetRegistryRoot )( 
            IDebugEngine2 * This,
            /* [full][in] */ LPCOLESTR pszRegistryRoot);
        
        HRESULT ( STDMETHODCALLTYPE *SetMetric )( 
            IDebugEngine2 * This,
            /* [in] */ LPCOLESTR pszMetric,
            /* [in] */ VARIANT varValue);
        
        HRESULT ( STDMETHODCALLTYPE *CauseBreak )( 
            IDebugEngine2 * This);
        
        END_INTERFACE
    } IDebugEngine2Vtbl;

    interface IDebugEngine2
    {
        CONST_VTBL struct IDebugEngine2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEngine2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEngine2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEngine2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEngine2_EnumPrograms(This,ppEnum)	\
    (This)->lpVtbl -> EnumPrograms(This,ppEnum)

#define IDebugEngine2_Attach(This,rgpPrograms,rgpProgramNodes,celtPrograms,pCallback,dwReason)	\
    (This)->lpVtbl -> Attach(This,rgpPrograms,rgpProgramNodes,celtPrograms,pCallback,dwReason)

#define IDebugEngine2_CreatePendingBreakpoint(This,pBPRequest,ppPendingBP)	\
    (This)->lpVtbl -> CreatePendingBreakpoint(This,pBPRequest,ppPendingBP)

#define IDebugEngine2_SetException(This,pException)	\
    (This)->lpVtbl -> SetException(This,pException)

#define IDebugEngine2_RemoveSetException(This,pException)	\
    (This)->lpVtbl -> RemoveSetException(This,pException)

#define IDebugEngine2_RemoveAllSetExceptions(This,guidType)	\
    (This)->lpVtbl -> RemoveAllSetExceptions(This,guidType)

#define IDebugEngine2_GetEngineId(This,pguidEngine)	\
    (This)->lpVtbl -> GetEngineId(This,pguidEngine)

#define IDebugEngine2_DestroyProgram(This,pProgram)	\
    (This)->lpVtbl -> DestroyProgram(This,pProgram)

#define IDebugEngine2_ContinueFromSynchronousEvent(This,pEvent)	\
    (This)->lpVtbl -> ContinueFromSynchronousEvent(This,pEvent)

#define IDebugEngine2_SetLocale(This,wLangID)	\
    (This)->lpVtbl -> SetLocale(This,wLangID)

#define IDebugEngine2_SetRegistryRoot(This,pszRegistryRoot)	\
    (This)->lpVtbl -> SetRegistryRoot(This,pszRegistryRoot)

#define IDebugEngine2_SetMetric(This,pszMetric,varValue)	\
    (This)->lpVtbl -> SetMetric(This,pszMetric,varValue)

#define IDebugEngine2_CauseBreak(This)	\
    (This)->lpVtbl -> CauseBreak(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEngine2_EnumPrograms_Proxy( 
    IDebugEngine2 * This,
    /* [out] */ IEnumDebugPrograms2 **ppEnum);


void __RPC_STUB IDebugEngine2_EnumPrograms_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_Attach_Proxy( 
    IDebugEngine2 * This,
    /* [length_is][size_is][in] */ IDebugProgram2 **rgpPrograms,
    /* [length_is][size_is][in] */ IDebugProgramNode2 **rgpProgramNodes,
    /* [in] */ DWORD celtPrograms,
    /* [in] */ IDebugEventCallback2 *pCallback,
    /* [in] */ ATTACH_REASON dwReason);


void __RPC_STUB IDebugEngine2_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_CreatePendingBreakpoint_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ IDebugBreakpointRequest2 *pBPRequest,
    /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP);


void __RPC_STUB IDebugEngine2_CreatePendingBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_SetException_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ EXCEPTION_INFO *pException);


void __RPC_STUB IDebugEngine2_SetException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_RemoveSetException_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ EXCEPTION_INFO *pException);


void __RPC_STUB IDebugEngine2_RemoveSetException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_RemoveAllSetExceptions_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ REFGUID guidType);


void __RPC_STUB IDebugEngine2_RemoveAllSetExceptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_GetEngineId_Proxy( 
    IDebugEngine2 * This,
    /* [out] */ GUID *pguidEngine);


void __RPC_STUB IDebugEngine2_GetEngineId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_DestroyProgram_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ IDebugProgram2 *pProgram);


void __RPC_STUB IDebugEngine2_DestroyProgram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_ContinueFromSynchronousEvent_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ IDebugEvent2 *pEvent);


void __RPC_STUB IDebugEngine2_ContinueFromSynchronousEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_SetLocale_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ WORD wLangID);


void __RPC_STUB IDebugEngine2_SetLocale_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_SetRegistryRoot_Proxy( 
    IDebugEngine2 * This,
    /* [full][in] */ LPCOLESTR pszRegistryRoot);


void __RPC_STUB IDebugEngine2_SetRegistryRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_SetMetric_Proxy( 
    IDebugEngine2 * This,
    /* [in] */ LPCOLESTR pszMetric,
    /* [in] */ VARIANT varValue);


void __RPC_STUB IDebugEngine2_SetMetric_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngine2_CauseBreak_Proxy( 
    IDebugEngine2 * This);


void __RPC_STUB IDebugEngine2_CauseBreak_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEngine2_INTERFACE_DEFINED__ */


#ifndef __IDebugEngineLaunch2_INTERFACE_DEFINED__
#define __IDebugEngineLaunch2_INTERFACE_DEFINED__

/* interface IDebugEngineLaunch2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugEngineLaunch2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c7c1462f-9736-466c-b2c1-b6b2dedbf4a7")
    IDebugEngineLaunch2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LaunchSuspended( 
            /* [full][in] */ LPCOLESTR pszMachine,
            /* [in] */ IDebugPort2 *pPort,
            /* [full][in] */ LPCOLESTR pszExe,
            /* [full][in] */ LPCOLESTR pszArgs,
            /* [full][in] */ LPCOLESTR pszDir,
            /* [full][in] */ BSTR bstrEnv,
            /* [full][in] */ LPCOLESTR pszOptions,
            /* [in] */ LAUNCH_FLAGS dwLaunchFlags,
            /* [in] */ DWORD hStdInput,
            /* [in] */ DWORD hStdOutput,
            /* [in] */ DWORD hStdError,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [out] */ IDebugProcess2 **ppProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResumeProcess( 
            /* [in] */ IDebugProcess2 *pProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanTerminateProcess( 
            /* [in] */ IDebugProcess2 *pProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TerminateProcess( 
            /* [in] */ IDebugProcess2 *pProcess) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEngineLaunch2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEngineLaunch2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEngineLaunch2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEngineLaunch2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *LaunchSuspended )( 
            IDebugEngineLaunch2 * This,
            /* [full][in] */ LPCOLESTR pszMachine,
            /* [in] */ IDebugPort2 *pPort,
            /* [full][in] */ LPCOLESTR pszExe,
            /* [full][in] */ LPCOLESTR pszArgs,
            /* [full][in] */ LPCOLESTR pszDir,
            /* [full][in] */ BSTR bstrEnv,
            /* [full][in] */ LPCOLESTR pszOptions,
            /* [in] */ LAUNCH_FLAGS dwLaunchFlags,
            /* [in] */ DWORD hStdInput,
            /* [in] */ DWORD hStdOutput,
            /* [in] */ DWORD hStdError,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [out] */ IDebugProcess2 **ppProcess);
        
        HRESULT ( STDMETHODCALLTYPE *ResumeProcess )( 
            IDebugEngineLaunch2 * This,
            /* [in] */ IDebugProcess2 *pProcess);
        
        HRESULT ( STDMETHODCALLTYPE *CanTerminateProcess )( 
            IDebugEngineLaunch2 * This,
            /* [in] */ IDebugProcess2 *pProcess);
        
        HRESULT ( STDMETHODCALLTYPE *TerminateProcess )( 
            IDebugEngineLaunch2 * This,
            /* [in] */ IDebugProcess2 *pProcess);
        
        END_INTERFACE
    } IDebugEngineLaunch2Vtbl;

    interface IDebugEngineLaunch2
    {
        CONST_VTBL struct IDebugEngineLaunch2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEngineLaunch2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEngineLaunch2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEngineLaunch2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEngineLaunch2_LaunchSuspended(This,pszMachine,pPort,pszExe,pszArgs,pszDir,bstrEnv,pszOptions,dwLaunchFlags,hStdInput,hStdOutput,hStdError,pCallback,ppProcess)	\
    (This)->lpVtbl -> LaunchSuspended(This,pszMachine,pPort,pszExe,pszArgs,pszDir,bstrEnv,pszOptions,dwLaunchFlags,hStdInput,hStdOutput,hStdError,pCallback,ppProcess)

#define IDebugEngineLaunch2_ResumeProcess(This,pProcess)	\
    (This)->lpVtbl -> ResumeProcess(This,pProcess)

#define IDebugEngineLaunch2_CanTerminateProcess(This,pProcess)	\
    (This)->lpVtbl -> CanTerminateProcess(This,pProcess)

#define IDebugEngineLaunch2_TerminateProcess(This,pProcess)	\
    (This)->lpVtbl -> TerminateProcess(This,pProcess)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEngineLaunch2_LaunchSuspended_Proxy( 
    IDebugEngineLaunch2 * This,
    /* [full][in] */ LPCOLESTR pszMachine,
    /* [in] */ IDebugPort2 *pPort,
    /* [full][in] */ LPCOLESTR pszExe,
    /* [full][in] */ LPCOLESTR pszArgs,
    /* [full][in] */ LPCOLESTR pszDir,
    /* [full][in] */ BSTR bstrEnv,
    /* [full][in] */ LPCOLESTR pszOptions,
    /* [in] */ LAUNCH_FLAGS dwLaunchFlags,
    /* [in] */ DWORD hStdInput,
    /* [in] */ DWORD hStdOutput,
    /* [in] */ DWORD hStdError,
    /* [in] */ IDebugEventCallback2 *pCallback,
    /* [out] */ IDebugProcess2 **ppProcess);


void __RPC_STUB IDebugEngineLaunch2_LaunchSuspended_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngineLaunch2_ResumeProcess_Proxy( 
    IDebugEngineLaunch2 * This,
    /* [in] */ IDebugProcess2 *pProcess);


void __RPC_STUB IDebugEngineLaunch2_ResumeProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngineLaunch2_CanTerminateProcess_Proxy( 
    IDebugEngineLaunch2 * This,
    /* [in] */ IDebugProcess2 *pProcess);


void __RPC_STUB IDebugEngineLaunch2_CanTerminateProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngineLaunch2_TerminateProcess_Proxy( 
    IDebugEngineLaunch2 * This,
    /* [in] */ IDebugProcess2 *pProcess);


void __RPC_STUB IDebugEngineLaunch2_TerminateProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEngineLaunch2_INTERFACE_DEFINED__ */


#ifndef __IDebugEventCallback2_INTERFACE_DEFINED__
#define __IDebugEventCallback2_INTERFACE_DEFINED__

/* interface IDebugEventCallback2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugEventCallback2_0001
    {	EVENT_ASYNCHRONOUS	= 0,
	EVENT_SYNCHRONOUS	= 0x1,
	EVENT_STOPPING	= 0x2,
	EVENT_ASYNC_STOP	= 0x2,
	EVENT_SYNC_STOP	= 0x3,
	EVENT_IMMEDIATE	= 0x4
    } ;
typedef DWORD EVENTATTRIBUTES;


EXTERN_C const IID IID_IDebugEventCallback2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ade2eeb9-fc85-4f5b-b5d9-d431b4aac31a")
    IDebugEventCallback2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Event( 
            /* [in] */ IDebugEngine2 *pEngine,
            /* [in] */ IDebugProcess2 *pProcess,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [in] */ IDebugThread2 *pThread,
            /* [in] */ IDebugEvent2 *pEvent,
            /* [in] */ REFIID riidEvent,
            /* [in] */ DWORD dwAttrib) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEventCallback2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEventCallback2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEventCallback2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEventCallback2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Event )( 
            IDebugEventCallback2 * This,
            /* [in] */ IDebugEngine2 *pEngine,
            /* [in] */ IDebugProcess2 *pProcess,
            /* [in] */ IDebugProgram2 *pProgram,
            /* [in] */ IDebugThread2 *pThread,
            /* [in] */ IDebugEvent2 *pEvent,
            /* [in] */ REFIID riidEvent,
            /* [in] */ DWORD dwAttrib);
        
        END_INTERFACE
    } IDebugEventCallback2Vtbl;

    interface IDebugEventCallback2
    {
        CONST_VTBL struct IDebugEventCallback2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEventCallback2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEventCallback2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEventCallback2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEventCallback2_Event(This,pEngine,pProcess,pProgram,pThread,pEvent,riidEvent,dwAttrib)	\
    (This)->lpVtbl -> Event(This,pEngine,pProcess,pProgram,pThread,pEvent,riidEvent,dwAttrib)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEventCallback2_Event_Proxy( 
    IDebugEventCallback2 * This,
    /* [in] */ IDebugEngine2 *pEngine,
    /* [in] */ IDebugProcess2 *pProcess,
    /* [in] */ IDebugProgram2 *pProgram,
    /* [in] */ IDebugThread2 *pThread,
    /* [in] */ IDebugEvent2 *pEvent,
    /* [in] */ REFIID riidEvent,
    /* [in] */ DWORD dwAttrib);


void __RPC_STUB IDebugEventCallback2_Event_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEventCallback2_INTERFACE_DEFINED__ */


#ifndef __IDebugEvent2_INTERFACE_DEFINED__
#define __IDebugEvent2_INTERFACE_DEFINED__

/* interface IDebugEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("423238d6-da42-4989-96fb-6bba26e72e09")
    IDebugEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAttributes( 
            /* [out] */ DWORD *pdwAttrib) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributes )( 
            IDebugEvent2 * This,
            /* [out] */ DWORD *pdwAttrib);
        
        END_INTERFACE
    } IDebugEvent2Vtbl;

    interface IDebugEvent2
    {
        CONST_VTBL struct IDebugEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEvent2_GetAttributes(This,pdwAttrib)	\
    (This)->lpVtbl -> GetAttributes(This,pdwAttrib)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEvent2_GetAttributes_Proxy( 
    IDebugEvent2 * This,
    /* [out] */ DWORD *pdwAttrib);


void __RPC_STUB IDebugEvent2_GetAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugSessionCreateEvent2_INTERFACE_DEFINED__
#define __IDebugSessionCreateEvent2_INTERFACE_DEFINED__

/* interface IDebugSessionCreateEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugSessionCreateEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2c2b15b7-fc6d-45b3-9622-29665d964a76")
    IDebugSessionCreateEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugSessionCreateEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugSessionCreateEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugSessionCreateEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugSessionCreateEvent2 * This);
        
        END_INTERFACE
    } IDebugSessionCreateEvent2Vtbl;

    interface IDebugSessionCreateEvent2
    {
        CONST_VTBL struct IDebugSessionCreateEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugSessionCreateEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugSessionCreateEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugSessionCreateEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugSessionCreateEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugSessionDestroyEvent2_INTERFACE_DEFINED__
#define __IDebugSessionDestroyEvent2_INTERFACE_DEFINED__

/* interface IDebugSessionDestroyEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugSessionDestroyEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f199b2c2-88fe-4c5d-a0fd-aa046b0dc0dc")
    IDebugSessionDestroyEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugSessionDestroyEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugSessionDestroyEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugSessionDestroyEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugSessionDestroyEvent2 * This);
        
        END_INTERFACE
    } IDebugSessionDestroyEvent2Vtbl;

    interface IDebugSessionDestroyEvent2
    {
        CONST_VTBL struct IDebugSessionDestroyEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugSessionDestroyEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugSessionDestroyEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugSessionDestroyEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugSessionDestroyEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugEngineCreateEvent2_INTERFACE_DEFINED__
#define __IDebugEngineCreateEvent2_INTERFACE_DEFINED__

/* interface IDebugEngineCreateEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugEngineCreateEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fe5b734c-759d-4e59-ab04-f103343bdd06")
    IDebugEngineCreateEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEngine( 
            /* [out] */ IDebugEngine2 **pEngine) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEngineCreateEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEngineCreateEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEngineCreateEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEngineCreateEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetEngine )( 
            IDebugEngineCreateEvent2 * This,
            /* [out] */ IDebugEngine2 **pEngine);
        
        END_INTERFACE
    } IDebugEngineCreateEvent2Vtbl;

    interface IDebugEngineCreateEvent2
    {
        CONST_VTBL struct IDebugEngineCreateEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEngineCreateEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEngineCreateEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEngineCreateEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEngineCreateEvent2_GetEngine(This,pEngine)	\
    (This)->lpVtbl -> GetEngine(This,pEngine)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEngineCreateEvent2_GetEngine_Proxy( 
    IDebugEngineCreateEvent2 * This,
    /* [out] */ IDebugEngine2 **pEngine);


void __RPC_STUB IDebugEngineCreateEvent2_GetEngine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEngineCreateEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugProcessCreateEvent2_INTERFACE_DEFINED__
#define __IDebugProcessCreateEvent2_INTERFACE_DEFINED__

/* interface IDebugProcessCreateEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProcessCreateEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bac3780f-04da-4726-901c-ba6a4633e1ca")
    IDebugProcessCreateEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugProcessCreateEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProcessCreateEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProcessCreateEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProcessCreateEvent2 * This);
        
        END_INTERFACE
    } IDebugProcessCreateEvent2Vtbl;

    interface IDebugProcessCreateEvent2
    {
        CONST_VTBL struct IDebugProcessCreateEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProcessCreateEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProcessCreateEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProcessCreateEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugProcessCreateEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugProcessDestroyEvent2_INTERFACE_DEFINED__
#define __IDebugProcessDestroyEvent2_INTERFACE_DEFINED__

/* interface IDebugProcessDestroyEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProcessDestroyEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3e2a0832-17e1-4886-8c0e-204da242995f")
    IDebugProcessDestroyEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugProcessDestroyEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProcessDestroyEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProcessDestroyEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProcessDestroyEvent2 * This);
        
        END_INTERFACE
    } IDebugProcessDestroyEvent2Vtbl;

    interface IDebugProcessDestroyEvent2
    {
        CONST_VTBL struct IDebugProcessDestroyEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProcessDestroyEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProcessDestroyEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProcessDestroyEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugProcessDestroyEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgramCreateEvent2_INTERFACE_DEFINED__
#define __IDebugProgramCreateEvent2_INTERFACE_DEFINED__

/* interface IDebugProgramCreateEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProgramCreateEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("96cd11ee-ecd4-4e89-957e-b5d496fc4139")
    IDebugProgramCreateEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgramCreateEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgramCreateEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgramCreateEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgramCreateEvent2 * This);
        
        END_INTERFACE
    } IDebugProgramCreateEvent2Vtbl;

    interface IDebugProgramCreateEvent2
    {
        CONST_VTBL struct IDebugProgramCreateEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgramCreateEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgramCreateEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgramCreateEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugProgramCreateEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgramDestroyEvent2_INTERFACE_DEFINED__
#define __IDebugProgramDestroyEvent2_INTERFACE_DEFINED__

/* interface IDebugProgramDestroyEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProgramDestroyEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e147e9e3-6440-4073-a7b7-a65592c714b5")
    IDebugProgramDestroyEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetExitCode( 
            /* [out] */ DWORD *pdwExit) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgramDestroyEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgramDestroyEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgramDestroyEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgramDestroyEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetExitCode )( 
            IDebugProgramDestroyEvent2 * This,
            /* [out] */ DWORD *pdwExit);
        
        END_INTERFACE
    } IDebugProgramDestroyEvent2Vtbl;

    interface IDebugProgramDestroyEvent2
    {
        CONST_VTBL struct IDebugProgramDestroyEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgramDestroyEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgramDestroyEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgramDestroyEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProgramDestroyEvent2_GetExitCode(This,pdwExit)	\
    (This)->lpVtbl -> GetExitCode(This,pdwExit)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProgramDestroyEvent2_GetExitCode_Proxy( 
    IDebugProgramDestroyEvent2 * This,
    /* [out] */ DWORD *pdwExit);


void __RPC_STUB IDebugProgramDestroyEvent2_GetExitCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProgramDestroyEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugThreadCreateEvent2_INTERFACE_DEFINED__
#define __IDebugThreadCreateEvent2_INTERFACE_DEFINED__

/* interface IDebugThreadCreateEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugThreadCreateEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2090ccfc-70c5-491d-a5e8-bad2dd9ee3ea")
    IDebugThreadCreateEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugThreadCreateEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugThreadCreateEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugThreadCreateEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugThreadCreateEvent2 * This);
        
        END_INTERFACE
    } IDebugThreadCreateEvent2Vtbl;

    interface IDebugThreadCreateEvent2
    {
        CONST_VTBL struct IDebugThreadCreateEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugThreadCreateEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugThreadCreateEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugThreadCreateEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugThreadCreateEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugThreadDestroyEvent2_INTERFACE_DEFINED__
#define __IDebugThreadDestroyEvent2_INTERFACE_DEFINED__

/* interface IDebugThreadDestroyEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugThreadDestroyEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2c3b7532-a36f-4a6e-9072-49be649b8541")
    IDebugThreadDestroyEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetExitCode( 
            /* [out] */ DWORD *pdwExit) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugThreadDestroyEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugThreadDestroyEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugThreadDestroyEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugThreadDestroyEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetExitCode )( 
            IDebugThreadDestroyEvent2 * This,
            /* [out] */ DWORD *pdwExit);
        
        END_INTERFACE
    } IDebugThreadDestroyEvent2Vtbl;

    interface IDebugThreadDestroyEvent2
    {
        CONST_VTBL struct IDebugThreadDestroyEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugThreadDestroyEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugThreadDestroyEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugThreadDestroyEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugThreadDestroyEvent2_GetExitCode(This,pdwExit)	\
    (This)->lpVtbl -> GetExitCode(This,pdwExit)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugThreadDestroyEvent2_GetExitCode_Proxy( 
    IDebugThreadDestroyEvent2 * This,
    /* [out] */ DWORD *pdwExit);


void __RPC_STUB IDebugThreadDestroyEvent2_GetExitCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugThreadDestroyEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugLoadCompleteEvent2_INTERFACE_DEFINED__
#define __IDebugLoadCompleteEvent2_INTERFACE_DEFINED__

/* interface IDebugLoadCompleteEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugLoadCompleteEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b1844850-1349-45d4-9f12-495212f5eb0b")
    IDebugLoadCompleteEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugLoadCompleteEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugLoadCompleteEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugLoadCompleteEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugLoadCompleteEvent2 * This);
        
        END_INTERFACE
    } IDebugLoadCompleteEvent2Vtbl;

    interface IDebugLoadCompleteEvent2
    {
        CONST_VTBL struct IDebugLoadCompleteEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugLoadCompleteEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugLoadCompleteEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugLoadCompleteEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugLoadCompleteEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugEntryPointEvent2_INTERFACE_DEFINED__
#define __IDebugEntryPointEvent2_INTERFACE_DEFINED__

/* interface IDebugEntryPointEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugEntryPointEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e8414a3e-1642-48ec-829e-5f4040e16da9")
    IDebugEntryPointEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugEntryPointEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEntryPointEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEntryPointEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEntryPointEvent2 * This);
        
        END_INTERFACE
    } IDebugEntryPointEvent2Vtbl;

    interface IDebugEntryPointEvent2
    {
        CONST_VTBL struct IDebugEntryPointEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEntryPointEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEntryPointEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEntryPointEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugEntryPointEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugStepCompleteEvent2_INTERFACE_DEFINED__
#define __IDebugStepCompleteEvent2_INTERFACE_DEFINED__

/* interface IDebugStepCompleteEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugStepCompleteEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0f7f24c1-74d9-4ea6-a3ea-7edb2d81441d")
    IDebugStepCompleteEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugStepCompleteEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugStepCompleteEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugStepCompleteEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugStepCompleteEvent2 * This);
        
        END_INTERFACE
    } IDebugStepCompleteEvent2Vtbl;

    interface IDebugStepCompleteEvent2
    {
        CONST_VTBL struct IDebugStepCompleteEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugStepCompleteEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugStepCompleteEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugStepCompleteEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugStepCompleteEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugCanStopEvent2_INTERFACE_DEFINED__
#define __IDebugCanStopEvent2_INTERFACE_DEFINED__

/* interface IDebugCanStopEvent2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugCanStopEvent2_0001
    {	CANSTOP_ENTRYPOINT	= 0,
	CANSTOP_STEPIN	= 0x1
    } ;
typedef DWORD CANSTOP_REASON;


EXTERN_C const IID IID_IDebugCanStopEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b5b0d747-d4d2-4e2d-872d-74da22037826")
    IDebugCanStopEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetReason( 
            /* [out] */ CANSTOP_REASON *pcr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanStop( 
            /* [in] */ BOOL fCanStop) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocumentContext( 
            /* [out] */ IDebugDocumentContext2 **ppDocCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCodeContext( 
            /* [out] */ IDebugCodeContext2 **ppCodeContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugCanStopEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugCanStopEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugCanStopEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugCanStopEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetReason )( 
            IDebugCanStopEvent2 * This,
            /* [out] */ CANSTOP_REASON *pcr);
        
        HRESULT ( STDMETHODCALLTYPE *CanStop )( 
            IDebugCanStopEvent2 * This,
            /* [in] */ BOOL fCanStop);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentContext )( 
            IDebugCanStopEvent2 * This,
            /* [out] */ IDebugDocumentContext2 **ppDocCxt);
        
        HRESULT ( STDMETHODCALLTYPE *GetCodeContext )( 
            IDebugCanStopEvent2 * This,
            /* [out] */ IDebugCodeContext2 **ppCodeContext);
        
        END_INTERFACE
    } IDebugCanStopEvent2Vtbl;

    interface IDebugCanStopEvent2
    {
        CONST_VTBL struct IDebugCanStopEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugCanStopEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugCanStopEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugCanStopEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugCanStopEvent2_GetReason(This,pcr)	\
    (This)->lpVtbl -> GetReason(This,pcr)

#define IDebugCanStopEvent2_CanStop(This,fCanStop)	\
    (This)->lpVtbl -> CanStop(This,fCanStop)

#define IDebugCanStopEvent2_GetDocumentContext(This,ppDocCxt)	\
    (This)->lpVtbl -> GetDocumentContext(This,ppDocCxt)

#define IDebugCanStopEvent2_GetCodeContext(This,ppCodeContext)	\
    (This)->lpVtbl -> GetCodeContext(This,ppCodeContext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugCanStopEvent2_GetReason_Proxy( 
    IDebugCanStopEvent2 * This,
    /* [out] */ CANSTOP_REASON *pcr);


void __RPC_STUB IDebugCanStopEvent2_GetReason_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugCanStopEvent2_CanStop_Proxy( 
    IDebugCanStopEvent2 * This,
    /* [in] */ BOOL fCanStop);


void __RPC_STUB IDebugCanStopEvent2_CanStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugCanStopEvent2_GetDocumentContext_Proxy( 
    IDebugCanStopEvent2 * This,
    /* [out] */ IDebugDocumentContext2 **ppDocCxt);


void __RPC_STUB IDebugCanStopEvent2_GetDocumentContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugCanStopEvent2_GetCodeContext_Proxy( 
    IDebugCanStopEvent2 * This,
    /* [out] */ IDebugCodeContext2 **ppCodeContext);


void __RPC_STUB IDebugCanStopEvent2_GetCodeContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugCanStopEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugBreakEvent2_INTERFACE_DEFINED__
#define __IDebugBreakEvent2_INTERFACE_DEFINED__

/* interface IDebugBreakEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBreakEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c7405d1d-e24b-44e0-b707-d8a5a4e1641b")
    IDebugBreakEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakEvent2 * This);
        
        END_INTERFACE
    } IDebugBreakEvent2Vtbl;

    interface IDebugBreakEvent2
    {
        CONST_VTBL struct IDebugBreakEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugBreakEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugBreakpointEvent2_INTERFACE_DEFINED__
#define __IDebugBreakpointEvent2_INTERFACE_DEFINED__

/* interface IDebugBreakpointEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBreakpointEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("501c1e21-c557-48b8-ba30-a1eab0bc4a74")
    IDebugBreakpointEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumBreakpoints( 
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakpointEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakpointEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakpointEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakpointEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumBreakpoints )( 
            IDebugBreakpointEvent2 * This,
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);
        
        END_INTERFACE
    } IDebugBreakpointEvent2Vtbl;

    interface IDebugBreakpointEvent2
    {
        CONST_VTBL struct IDebugBreakpointEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakpointEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakpointEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakpointEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBreakpointEvent2_EnumBreakpoints(This,ppEnum)	\
    (This)->lpVtbl -> EnumBreakpoints(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBreakpointEvent2_EnumBreakpoints_Proxy( 
    IDebugBreakpointEvent2 * This,
    /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);


void __RPC_STUB IDebugBreakpointEvent2_EnumBreakpoints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBreakpointEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugExceptionEvent2_INTERFACE_DEFINED__
#define __IDebugExceptionEvent2_INTERFACE_DEFINED__

/* interface IDebugExceptionEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugExceptionEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("51a94113-8788-4a54-ae15-08b74ff922d0")
    IDebugExceptionEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetException( 
            /* [out] */ EXCEPTION_INFO *pExceptionInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExceptionDescription( 
            /* [out] */ BSTR *pbstrDescription) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanPassToDebuggee( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PassToDebuggee( 
            /* [in] */ BOOL fPass) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugExceptionEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugExceptionEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugExceptionEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugExceptionEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetException )( 
            IDebugExceptionEvent2 * This,
            /* [out] */ EXCEPTION_INFO *pExceptionInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetExceptionDescription )( 
            IDebugExceptionEvent2 * This,
            /* [out] */ BSTR *pbstrDescription);
        
        HRESULT ( STDMETHODCALLTYPE *CanPassToDebuggee )( 
            IDebugExceptionEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *PassToDebuggee )( 
            IDebugExceptionEvent2 * This,
            /* [in] */ BOOL fPass);
        
        END_INTERFACE
    } IDebugExceptionEvent2Vtbl;

    interface IDebugExceptionEvent2
    {
        CONST_VTBL struct IDebugExceptionEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugExceptionEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugExceptionEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugExceptionEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugExceptionEvent2_GetException(This,pExceptionInfo)	\
    (This)->lpVtbl -> GetException(This,pExceptionInfo)

#define IDebugExceptionEvent2_GetExceptionDescription(This,pbstrDescription)	\
    (This)->lpVtbl -> GetExceptionDescription(This,pbstrDescription)

#define IDebugExceptionEvent2_CanPassToDebuggee(This)	\
    (This)->lpVtbl -> CanPassToDebuggee(This)

#define IDebugExceptionEvent2_PassToDebuggee(This,fPass)	\
    (This)->lpVtbl -> PassToDebuggee(This,fPass)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugExceptionEvent2_GetException_Proxy( 
    IDebugExceptionEvent2 * This,
    /* [out] */ EXCEPTION_INFO *pExceptionInfo);


void __RPC_STUB IDebugExceptionEvent2_GetException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExceptionEvent2_GetExceptionDescription_Proxy( 
    IDebugExceptionEvent2 * This,
    /* [out] */ BSTR *pbstrDescription);


void __RPC_STUB IDebugExceptionEvent2_GetExceptionDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExceptionEvent2_CanPassToDebuggee_Proxy( 
    IDebugExceptionEvent2 * This);


void __RPC_STUB IDebugExceptionEvent2_CanPassToDebuggee_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExceptionEvent2_PassToDebuggee_Proxy( 
    IDebugExceptionEvent2 * This,
    /* [in] */ BOOL fPass);


void __RPC_STUB IDebugExceptionEvent2_PassToDebuggee_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugExceptionEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugOutputStringEvent2_INTERFACE_DEFINED__
#define __IDebugOutputStringEvent2_INTERFACE_DEFINED__

/* interface IDebugOutputStringEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugOutputStringEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("569c4bb1-7b82-46fc-ae28-4536ddad753e")
    IDebugOutputStringEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetString( 
            /* [out] */ BSTR *pbstrString) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugOutputStringEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugOutputStringEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugOutputStringEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugOutputStringEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetString )( 
            IDebugOutputStringEvent2 * This,
            /* [out] */ BSTR *pbstrString);
        
        END_INTERFACE
    } IDebugOutputStringEvent2Vtbl;

    interface IDebugOutputStringEvent2
    {
        CONST_VTBL struct IDebugOutputStringEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugOutputStringEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugOutputStringEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugOutputStringEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugOutputStringEvent2_GetString(This,pbstrString)	\
    (This)->lpVtbl -> GetString(This,pbstrString)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugOutputStringEvent2_GetString_Proxy( 
    IDebugOutputStringEvent2 * This,
    /* [out] */ BSTR *pbstrString);


void __RPC_STUB IDebugOutputStringEvent2_GetString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugOutputStringEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugModuleLoadEvent2_INTERFACE_DEFINED__
#define __IDebugModuleLoadEvent2_INTERFACE_DEFINED__

/* interface IDebugModuleLoadEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugModuleLoadEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("989db083-0d7c-40d1-a9d9-921bf611a4b2")
    IDebugModuleLoadEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetModule( 
            /* [out] */ IDebugModule2 **pModule,
            /* [out][in] */ BSTR *pbstrDebugMessage,
            /* [out][in] */ BOOL *pbLoad) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugModuleLoadEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugModuleLoadEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugModuleLoadEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugModuleLoadEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetModule )( 
            IDebugModuleLoadEvent2 * This,
            /* [out] */ IDebugModule2 **pModule,
            /* [out][in] */ BSTR *pbstrDebugMessage,
            /* [out][in] */ BOOL *pbLoad);
        
        END_INTERFACE
    } IDebugModuleLoadEvent2Vtbl;

    interface IDebugModuleLoadEvent2
    {
        CONST_VTBL struct IDebugModuleLoadEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugModuleLoadEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugModuleLoadEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugModuleLoadEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugModuleLoadEvent2_GetModule(This,pModule,pbstrDebugMessage,pbLoad)	\
    (This)->lpVtbl -> GetModule(This,pModule,pbstrDebugMessage,pbLoad)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugModuleLoadEvent2_GetModule_Proxy( 
    IDebugModuleLoadEvent2 * This,
    /* [out] */ IDebugModule2 **pModule,
    /* [out][in] */ BSTR *pbstrDebugMessage,
    /* [out][in] */ BOOL *pbLoad);


void __RPC_STUB IDebugModuleLoadEvent2_GetModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugModuleLoadEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugPropertyCreateEvent2_INTERFACE_DEFINED__
#define __IDebugPropertyCreateEvent2_INTERFACE_DEFINED__

/* interface IDebugPropertyCreateEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPropertyCreateEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ded6d613-a3db-4e35-bb5b-a92391133f03")
    IDebugPropertyCreateEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDebugProperty( 
            /* [out] */ IDebugProperty2 **ppProperty) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPropertyCreateEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPropertyCreateEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPropertyCreateEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPropertyCreateEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDebugProperty )( 
            IDebugPropertyCreateEvent2 * This,
            /* [out] */ IDebugProperty2 **ppProperty);
        
        END_INTERFACE
    } IDebugPropertyCreateEvent2Vtbl;

    interface IDebugPropertyCreateEvent2
    {
        CONST_VTBL struct IDebugPropertyCreateEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPropertyCreateEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPropertyCreateEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPropertyCreateEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPropertyCreateEvent2_GetDebugProperty(This,ppProperty)	\
    (This)->lpVtbl -> GetDebugProperty(This,ppProperty)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPropertyCreateEvent2_GetDebugProperty_Proxy( 
    IDebugPropertyCreateEvent2 * This,
    /* [out] */ IDebugProperty2 **ppProperty);


void __RPC_STUB IDebugPropertyCreateEvent2_GetDebugProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPropertyCreateEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugPropertyDestroyEvent2_INTERFACE_DEFINED__
#define __IDebugPropertyDestroyEvent2_INTERFACE_DEFINED__

/* interface IDebugPropertyDestroyEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugPropertyDestroyEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f3765f18-f395-4b8c-8e95-dcb3fe8e7ec8")
    IDebugPropertyDestroyEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDebugProperty( 
            /* [out] */ IDebugProperty2 **ppProperty) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPropertyDestroyEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPropertyDestroyEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPropertyDestroyEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPropertyDestroyEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDebugProperty )( 
            IDebugPropertyDestroyEvent2 * This,
            /* [out] */ IDebugProperty2 **ppProperty);
        
        END_INTERFACE
    } IDebugPropertyDestroyEvent2Vtbl;

    interface IDebugPropertyDestroyEvent2
    {
        CONST_VTBL struct IDebugPropertyDestroyEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPropertyDestroyEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPropertyDestroyEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPropertyDestroyEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPropertyDestroyEvent2_GetDebugProperty(This,ppProperty)	\
    (This)->lpVtbl -> GetDebugProperty(This,ppProperty)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPropertyDestroyEvent2_GetDebugProperty_Proxy( 
    IDebugPropertyDestroyEvent2 * This,
    /* [out] */ IDebugProperty2 **ppProperty);


void __RPC_STUB IDebugPropertyDestroyEvent2_GetDebugProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPropertyDestroyEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugBreakpointBoundEvent2_INTERFACE_DEFINED__
#define __IDebugBreakpointBoundEvent2_INTERFACE_DEFINED__

/* interface IDebugBreakpointBoundEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBreakpointBoundEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1dddb704-cf99-4b8a-b746-dabb01dd13a0")
    IDebugBreakpointBoundEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPendingBreakpoint( 
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumBoundBreakpoints( 
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakpointBoundEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakpointBoundEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakpointBoundEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakpointBoundEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPendingBreakpoint )( 
            IDebugBreakpointBoundEvent2 * This,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP);
        
        HRESULT ( STDMETHODCALLTYPE *EnumBoundBreakpoints )( 
            IDebugBreakpointBoundEvent2 * This,
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);
        
        END_INTERFACE
    } IDebugBreakpointBoundEvent2Vtbl;

    interface IDebugBreakpointBoundEvent2
    {
        CONST_VTBL struct IDebugBreakpointBoundEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakpointBoundEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakpointBoundEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakpointBoundEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBreakpointBoundEvent2_GetPendingBreakpoint(This,ppPendingBP)	\
    (This)->lpVtbl -> GetPendingBreakpoint(This,ppPendingBP)

#define IDebugBreakpointBoundEvent2_EnumBoundBreakpoints(This,ppEnum)	\
    (This)->lpVtbl -> EnumBoundBreakpoints(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBreakpointBoundEvent2_GetPendingBreakpoint_Proxy( 
    IDebugBreakpointBoundEvent2 * This,
    /* [out] */ IDebugPendingBreakpoint2 **ppPendingBP);


void __RPC_STUB IDebugBreakpointBoundEvent2_GetPendingBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBreakpointBoundEvent2_EnumBoundBreakpoints_Proxy( 
    IDebugBreakpointBoundEvent2 * This,
    /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);


void __RPC_STUB IDebugBreakpointBoundEvent2_EnumBoundBreakpoints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBreakpointBoundEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugBreakpointUnboundEvent2_INTERFACE_DEFINED__
#define __IDebugBreakpointUnboundEvent2_INTERFACE_DEFINED__

/* interface IDebugBreakpointUnboundEvent2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugBreakpointUnboundEvent2_0001
    {	BPUR_UNKNOWN	= 0x1,
	BPUR_CODE_UNLOADED	= 0x2,
	BPUR_BREAKPOINT_REBIND	= 0x3,
	BPUR_BREAKPOINT_ERROR	= 0x4
    } ;
typedef DWORD BP_UNBOUND_REASON;


EXTERN_C const IID IID_IDebugBreakpointUnboundEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("78d1db4f-c557-4dc5-a2dd-5369d21b1c8c")
    IDebugBreakpointUnboundEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBreakpoint( 
            /* [out] */ IDebugBoundBreakpoint2 **ppBP) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetReason( 
            /* [out] */ BP_UNBOUND_REASON *pdwUnboundReason) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakpointUnboundEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakpointUnboundEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakpointUnboundEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakpointUnboundEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBreakpoint )( 
            IDebugBreakpointUnboundEvent2 * This,
            /* [out] */ IDebugBoundBreakpoint2 **ppBP);
        
        HRESULT ( STDMETHODCALLTYPE *GetReason )( 
            IDebugBreakpointUnboundEvent2 * This,
            /* [out] */ BP_UNBOUND_REASON *pdwUnboundReason);
        
        END_INTERFACE
    } IDebugBreakpointUnboundEvent2Vtbl;

    interface IDebugBreakpointUnboundEvent2
    {
        CONST_VTBL struct IDebugBreakpointUnboundEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakpointUnboundEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakpointUnboundEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakpointUnboundEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBreakpointUnboundEvent2_GetBreakpoint(This,ppBP)	\
    (This)->lpVtbl -> GetBreakpoint(This,ppBP)

#define IDebugBreakpointUnboundEvent2_GetReason(This,pdwUnboundReason)	\
    (This)->lpVtbl -> GetReason(This,pdwUnboundReason)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBreakpointUnboundEvent2_GetBreakpoint_Proxy( 
    IDebugBreakpointUnboundEvent2 * This,
    /* [out] */ IDebugBoundBreakpoint2 **ppBP);


void __RPC_STUB IDebugBreakpointUnboundEvent2_GetBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBreakpointUnboundEvent2_GetReason_Proxy( 
    IDebugBreakpointUnboundEvent2 * This,
    /* [out] */ BP_UNBOUND_REASON *pdwUnboundReason);


void __RPC_STUB IDebugBreakpointUnboundEvent2_GetReason_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBreakpointUnboundEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugBreakpointErrorEvent2_INTERFACE_DEFINED__
#define __IDebugBreakpointErrorEvent2_INTERFACE_DEFINED__

/* interface IDebugBreakpointErrorEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBreakpointErrorEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("abb0ca42-f82b-4622-84e4-6903ae90f210")
    IDebugBreakpointErrorEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetErrorBreakpoint( 
            /* [out] */ IDebugErrorBreakpoint2 **ppErrorBP) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakpointErrorEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakpointErrorEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakpointErrorEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakpointErrorEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetErrorBreakpoint )( 
            IDebugBreakpointErrorEvent2 * This,
            /* [out] */ IDebugErrorBreakpoint2 **ppErrorBP);
        
        END_INTERFACE
    } IDebugBreakpointErrorEvent2Vtbl;

    interface IDebugBreakpointErrorEvent2
    {
        CONST_VTBL struct IDebugBreakpointErrorEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakpointErrorEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakpointErrorEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakpointErrorEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBreakpointErrorEvent2_GetErrorBreakpoint(This,ppErrorBP)	\
    (This)->lpVtbl -> GetErrorBreakpoint(This,ppErrorBP)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBreakpointErrorEvent2_GetErrorBreakpoint_Proxy( 
    IDebugBreakpointErrorEvent2 * This,
    /* [out] */ IDebugErrorBreakpoint2 **ppErrorBP);


void __RPC_STUB IDebugBreakpointErrorEvent2_GetErrorBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBreakpointErrorEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugExpressionEvaluationCompleteEvent2_INTERFACE_DEFINED__
#define __IDebugExpressionEvaluationCompleteEvent2_INTERFACE_DEFINED__

/* interface IDebugExpressionEvaluationCompleteEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugExpressionEvaluationCompleteEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c0e13a85-238a-4800-8315-d947c960a843")
    IDebugExpressionEvaluationCompleteEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetExpression( 
            /* [out] */ IDebugExpression2 **ppExpr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResult( 
            /* [out] */ IDebugProperty2 **ppResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugExpressionEvaluationCompleteEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugExpressionEvaluationCompleteEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugExpressionEvaluationCompleteEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugExpressionEvaluationCompleteEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetExpression )( 
            IDebugExpressionEvaluationCompleteEvent2 * This,
            /* [out] */ IDebugExpression2 **ppExpr);
        
        HRESULT ( STDMETHODCALLTYPE *GetResult )( 
            IDebugExpressionEvaluationCompleteEvent2 * This,
            /* [out] */ IDebugProperty2 **ppResult);
        
        END_INTERFACE
    } IDebugExpressionEvaluationCompleteEvent2Vtbl;

    interface IDebugExpressionEvaluationCompleteEvent2
    {
        CONST_VTBL struct IDebugExpressionEvaluationCompleteEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugExpressionEvaluationCompleteEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugExpressionEvaluationCompleteEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugExpressionEvaluationCompleteEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugExpressionEvaluationCompleteEvent2_GetExpression(This,ppExpr)	\
    (This)->lpVtbl -> GetExpression(This,ppExpr)

#define IDebugExpressionEvaluationCompleteEvent2_GetResult(This,ppResult)	\
    (This)->lpVtbl -> GetResult(This,ppResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugExpressionEvaluationCompleteEvent2_GetExpression_Proxy( 
    IDebugExpressionEvaluationCompleteEvent2 * This,
    /* [out] */ IDebugExpression2 **ppExpr);


void __RPC_STUB IDebugExpressionEvaluationCompleteEvent2_GetExpression_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExpressionEvaluationCompleteEvent2_GetResult_Proxy( 
    IDebugExpressionEvaluationCompleteEvent2 * This,
    /* [out] */ IDebugProperty2 **ppResult);


void __RPC_STUB IDebugExpressionEvaluationCompleteEvent2_GetResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugExpressionEvaluationCompleteEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugReturnValueEvent2_INTERFACE_DEFINED__
#define __IDebugReturnValueEvent2_INTERFACE_DEFINED__

/* interface IDebugReturnValueEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugReturnValueEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0da4d4cc-2d0b-410f-8d5d-b6b73a5d35d8")
    IDebugReturnValueEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetReturnValue( 
            /* [out] */ IDebugProperty2 **ppReturnValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugReturnValueEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugReturnValueEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugReturnValueEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugReturnValueEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetReturnValue )( 
            IDebugReturnValueEvent2 * This,
            /* [out] */ IDebugProperty2 **ppReturnValue);
        
        END_INTERFACE
    } IDebugReturnValueEvent2Vtbl;

    interface IDebugReturnValueEvent2
    {
        CONST_VTBL struct IDebugReturnValueEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugReturnValueEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugReturnValueEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugReturnValueEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugReturnValueEvent2_GetReturnValue(This,ppReturnValue)	\
    (This)->lpVtbl -> GetReturnValue(This,ppReturnValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugReturnValueEvent2_GetReturnValue_Proxy( 
    IDebugReturnValueEvent2 * This,
    /* [out] */ IDebugProperty2 **ppReturnValue);


void __RPC_STUB IDebugReturnValueEvent2_GetReturnValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugReturnValueEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugFindSymbolEvent2_INTERFACE_DEFINED__
#define __IDebugFindSymbolEvent2_INTERFACE_DEFINED__

/* interface IDebugFindSymbolEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugFindSymbolEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4072ae37-3739-43c9-ac88-ca2ea79ed32b")
    IDebugFindSymbolEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SearchWithPath( 
            /* [in] */ LPCOLESTR pszSymbolSearchURL) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugFindSymbolEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugFindSymbolEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugFindSymbolEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugFindSymbolEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *SearchWithPath )( 
            IDebugFindSymbolEvent2 * This,
            /* [in] */ LPCOLESTR pszSymbolSearchURL);
        
        END_INTERFACE
    } IDebugFindSymbolEvent2Vtbl;

    interface IDebugFindSymbolEvent2
    {
        CONST_VTBL struct IDebugFindSymbolEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugFindSymbolEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugFindSymbolEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugFindSymbolEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugFindSymbolEvent2_SearchWithPath(This,pszSymbolSearchURL)	\
    (This)->lpVtbl -> SearchWithPath(This,pszSymbolSearchURL)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugFindSymbolEvent2_SearchWithPath_Proxy( 
    IDebugFindSymbolEvent2 * This,
    /* [in] */ LPCOLESTR pszSymbolSearchURL);


void __RPC_STUB IDebugFindSymbolEvent2_SearchWithPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugFindSymbolEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugNoSymbolsEvent2_INTERFACE_DEFINED__
#define __IDebugNoSymbolsEvent2_INTERFACE_DEFINED__

/* interface IDebugNoSymbolsEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugNoSymbolsEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3ad4fb48-647e-4b03-9c1e-52754e80c880")
    IDebugNoSymbolsEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugNoSymbolsEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugNoSymbolsEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugNoSymbolsEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugNoSymbolsEvent2 * This);
        
        END_INTERFACE
    } IDebugNoSymbolsEvent2Vtbl;

    interface IDebugNoSymbolsEvent2
    {
        CONST_VTBL struct IDebugNoSymbolsEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugNoSymbolsEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugNoSymbolsEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugNoSymbolsEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugNoSymbolsEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgramNameChangedEvent2_INTERFACE_DEFINED__
#define __IDebugProgramNameChangedEvent2_INTERFACE_DEFINED__

/* interface IDebugProgramNameChangedEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProgramNameChangedEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e05c2dfd-59d5-46d3-a71c-5d07665d85af")
    IDebugProgramNameChangedEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgramNameChangedEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgramNameChangedEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgramNameChangedEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgramNameChangedEvent2 * This);
        
        END_INTERFACE
    } IDebugProgramNameChangedEvent2Vtbl;

    interface IDebugProgramNameChangedEvent2
    {
        CONST_VTBL struct IDebugProgramNameChangedEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgramNameChangedEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgramNameChangedEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgramNameChangedEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugProgramNameChangedEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugThreadNameChangedEvent2_INTERFACE_DEFINED__
#define __IDebugThreadNameChangedEvent2_INTERFACE_DEFINED__

/* interface IDebugThreadNameChangedEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugThreadNameChangedEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1ef4ef78-2c44-4b7a-8473-8f4357611729")
    IDebugThreadNameChangedEvent2 : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDebugThreadNameChangedEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugThreadNameChangedEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugThreadNameChangedEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugThreadNameChangedEvent2 * This);
        
        END_INTERFACE
    } IDebugThreadNameChangedEvent2Vtbl;

    interface IDebugThreadNameChangedEvent2
    {
        CONST_VTBL struct IDebugThreadNameChangedEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugThreadNameChangedEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugThreadNameChangedEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugThreadNameChangedEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDebugThreadNameChangedEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugMessageEvent2_INTERFACE_DEFINED__
#define __IDebugMessageEvent2_INTERFACE_DEFINED__

/* interface IDebugMessageEvent2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugMessageEvent2_0001
    {	MT_OUTPUTSTRING	= 0x1,
	MT_MESSAGEBOX	= 0x2
    } ;
typedef DWORD MESSAGETYPE;


EXTERN_C const IID IID_IDebugMessageEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3bdb28cf-dbd2-4d24-af03-01072b67eb9e")
    IDebugMessageEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMessage( 
            /* [out] */ MESSAGETYPE *pMessageType,
            /* [out] */ BSTR *pbstrMessage,
            /* [out] */ DWORD *pdwType,
            /* [out] */ BSTR *pbstrHelpFileName,
            /* [out] */ DWORD *pdwHelpId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetResponse( 
            /* [in] */ DWORD dwResponse) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMessageEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMessageEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMessageEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMessageEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMessage )( 
            IDebugMessageEvent2 * This,
            /* [out] */ MESSAGETYPE *pMessageType,
            /* [out] */ BSTR *pbstrMessage,
            /* [out] */ DWORD *pdwType,
            /* [out] */ BSTR *pbstrHelpFileName,
            /* [out] */ DWORD *pdwHelpId);
        
        HRESULT ( STDMETHODCALLTYPE *SetResponse )( 
            IDebugMessageEvent2 * This,
            /* [in] */ DWORD dwResponse);
        
        END_INTERFACE
    } IDebugMessageEvent2Vtbl;

    interface IDebugMessageEvent2
    {
        CONST_VTBL struct IDebugMessageEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMessageEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMessageEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMessageEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMessageEvent2_GetMessage(This,pMessageType,pbstrMessage,pdwType,pbstrHelpFileName,pdwHelpId)	\
    (This)->lpVtbl -> GetMessage(This,pMessageType,pbstrMessage,pdwType,pbstrHelpFileName,pdwHelpId)

#define IDebugMessageEvent2_SetResponse(This,dwResponse)	\
    (This)->lpVtbl -> SetResponse(This,dwResponse)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMessageEvent2_GetMessage_Proxy( 
    IDebugMessageEvent2 * This,
    /* [out] */ MESSAGETYPE *pMessageType,
    /* [out] */ BSTR *pbstrMessage,
    /* [out] */ DWORD *pdwType,
    /* [out] */ BSTR *pbstrHelpFileName,
    /* [out] */ DWORD *pdwHelpId);


void __RPC_STUB IDebugMessageEvent2_GetMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMessageEvent2_SetResponse_Proxy( 
    IDebugMessageEvent2 * This,
    /* [in] */ DWORD dwResponse);


void __RPC_STUB IDebugMessageEvent2_SetResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMessageEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugActivateDocumentEvent2_INTERFACE_DEFINED__
#define __IDebugActivateDocumentEvent2_INTERFACE_DEFINED__

/* interface IDebugActivateDocumentEvent2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugActivateDocumentEvent2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("58f36c3d-7d07-4eba-a041-62f63e188037")
    IDebugActivateDocumentEvent2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDocument( 
            /* [out] */ IDebugDocument2 **ppDoc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocumentContext( 
            /* [out] */ IDebugDocumentContext2 **ppDocContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugActivateDocumentEvent2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugActivateDocumentEvent2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugActivateDocumentEvent2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugActivateDocumentEvent2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocument )( 
            IDebugActivateDocumentEvent2 * This,
            /* [out] */ IDebugDocument2 **ppDoc);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentContext )( 
            IDebugActivateDocumentEvent2 * This,
            /* [out] */ IDebugDocumentContext2 **ppDocContext);
        
        END_INTERFACE
    } IDebugActivateDocumentEvent2Vtbl;

    interface IDebugActivateDocumentEvent2
    {
        CONST_VTBL struct IDebugActivateDocumentEvent2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugActivateDocumentEvent2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugActivateDocumentEvent2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugActivateDocumentEvent2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugActivateDocumentEvent2_GetDocument(This,ppDoc)	\
    (This)->lpVtbl -> GetDocument(This,ppDoc)

#define IDebugActivateDocumentEvent2_GetDocumentContext(This,ppDocContext)	\
    (This)->lpVtbl -> GetDocumentContext(This,ppDocContext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugActivateDocumentEvent2_GetDocument_Proxy( 
    IDebugActivateDocumentEvent2 * This,
    /* [out] */ IDebugDocument2 **ppDoc);


void __RPC_STUB IDebugActivateDocumentEvent2_GetDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugActivateDocumentEvent2_GetDocumentContext_Proxy( 
    IDebugActivateDocumentEvent2 * This,
    /* [out] */ IDebugDocumentContext2 **ppDocContext);


void __RPC_STUB IDebugActivateDocumentEvent2_GetDocumentContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugActivateDocumentEvent2_INTERFACE_DEFINED__ */


#ifndef __IDebugProcess2_INTERFACE_DEFINED__
#define __IDebugProcess2_INTERFACE_DEFINED__

/* interface IDebugProcess2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugProcess2_0001
    {	PIFLAG_SYSTEM_PROCESS	= 0x1,
	PIFLAG_DEBUGGER_ATTACHED	= 0x2
    } ;
typedef DWORD PROCESS_INFO_FLAGS;


enum __MIDL_IDebugProcess2_0002
    {	PIF_FILE_NAME	= 0x1,
	PIF_BASE_NAME	= 0x2,
	PIF_TITLE	= 0x4,
	PIF_PROCESS_ID	= 0x8,
	PIF_SESSION_ID	= 0x10,
	PIF_ATTACHED_SESSION_NAME	= 0x20,
	PIF_CREATION_TIME	= 0x40,
	PIF_FLAGS	= 0x80,
	PIF_ALL	= 0xff
    } ;
typedef DWORD PROCESS_INFO_FIELDS;

typedef struct tagPROCESS_INFO
    {
    PROCESS_INFO_FIELDS Fields;
    BSTR bstrFileName;
    BSTR bstrBaseName;
    BSTR bstrTitle;
    AD_PROCESS_ID ProcessId;
    DWORD dwSessionId;
    BSTR bstrAttachedSessionName;
    FILETIME CreationTime;
    PROCESS_INFO_FLAGS Flags;
    } 	PROCESS_INFO;


EXTERN_C const IID IID_IDebugProcess2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("43286fea-6997-4543-803e-60a20c473de5")
    IDebugProcess2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [in] */ PROCESS_INFO_FIELDS Fields,
            /* [out] */ PROCESS_INFO *pProcessInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumPrograms( 
            /* [out] */ IEnumDebugPrograms2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMachine( 
            /* [out] */ IDebugMachine2 **ppMachine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Terminate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Attach( 
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [size_is][in] */ GUID *rgguidSpecificEngines,
            /* [in] */ DWORD celtSpecificEngines,
            /* [length_is][size_is][out] */ HRESULT *rghrEngineAttach) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanDetach( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Detach( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPhysicalProcessId( 
            /* [out] */ AD_PROCESS_ID *pProcessId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProcessId( 
            /* [out] */ GUID *pguidProcessId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttachedSessionName( 
            /* [out] */ BSTR *pbstrSessionName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumThreads( 
            /* [out] */ IEnumDebugThreads2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CauseBreak( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPort( 
            /* [out] */ IDebugPort2 **ppPort) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProcess2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProcess2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProcess2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProcess2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugProcess2 * This,
            /* [in] */ PROCESS_INFO_FIELDS Fields,
            /* [out] */ PROCESS_INFO *pProcessInfo);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPrograms )( 
            IDebugProcess2 * This,
            /* [out] */ IEnumDebugPrograms2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugProcess2 * This,
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetMachine )( 
            IDebugProcess2 * This,
            /* [out] */ IDebugMachine2 **ppMachine);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            IDebugProcess2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Attach )( 
            IDebugProcess2 * This,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [size_is][in] */ GUID *rgguidSpecificEngines,
            /* [in] */ DWORD celtSpecificEngines,
            /* [length_is][size_is][out] */ HRESULT *rghrEngineAttach);
        
        HRESULT ( STDMETHODCALLTYPE *CanDetach )( 
            IDebugProcess2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Detach )( 
            IDebugProcess2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPhysicalProcessId )( 
            IDebugProcess2 * This,
            /* [out] */ AD_PROCESS_ID *pProcessId);
        
        HRESULT ( STDMETHODCALLTYPE *GetProcessId )( 
            IDebugProcess2 * This,
            /* [out] */ GUID *pguidProcessId);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttachedSessionName )( 
            IDebugProcess2 * This,
            /* [out] */ BSTR *pbstrSessionName);
        
        HRESULT ( STDMETHODCALLTYPE *EnumThreads )( 
            IDebugProcess2 * This,
            /* [out] */ IEnumDebugThreads2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *CauseBreak )( 
            IDebugProcess2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPort )( 
            IDebugProcess2 * This,
            /* [out] */ IDebugPort2 **ppPort);
        
        END_INTERFACE
    } IDebugProcess2Vtbl;

    interface IDebugProcess2
    {
        CONST_VTBL struct IDebugProcess2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProcess2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProcess2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProcess2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProcess2_GetInfo(This,Fields,pProcessInfo)	\
    (This)->lpVtbl -> GetInfo(This,Fields,pProcessInfo)

#define IDebugProcess2_EnumPrograms(This,ppEnum)	\
    (This)->lpVtbl -> EnumPrograms(This,ppEnum)

#define IDebugProcess2_GetName(This,gnType,pbstrName)	\
    (This)->lpVtbl -> GetName(This,gnType,pbstrName)

#define IDebugProcess2_GetMachine(This,ppMachine)	\
    (This)->lpVtbl -> GetMachine(This,ppMachine)

#define IDebugProcess2_Terminate(This)	\
    (This)->lpVtbl -> Terminate(This)

#define IDebugProcess2_Attach(This,pCallback,rgguidSpecificEngines,celtSpecificEngines,rghrEngineAttach)	\
    (This)->lpVtbl -> Attach(This,pCallback,rgguidSpecificEngines,celtSpecificEngines,rghrEngineAttach)

#define IDebugProcess2_CanDetach(This)	\
    (This)->lpVtbl -> CanDetach(This)

#define IDebugProcess2_Detach(This)	\
    (This)->lpVtbl -> Detach(This)

#define IDebugProcess2_GetPhysicalProcessId(This,pProcessId)	\
    (This)->lpVtbl -> GetPhysicalProcessId(This,pProcessId)

#define IDebugProcess2_GetProcessId(This,pguidProcessId)	\
    (This)->lpVtbl -> GetProcessId(This,pguidProcessId)

#define IDebugProcess2_GetAttachedSessionName(This,pbstrSessionName)	\
    (This)->lpVtbl -> GetAttachedSessionName(This,pbstrSessionName)

#define IDebugProcess2_EnumThreads(This,ppEnum)	\
    (This)->lpVtbl -> EnumThreads(This,ppEnum)

#define IDebugProcess2_CauseBreak(This)	\
    (This)->lpVtbl -> CauseBreak(This)

#define IDebugProcess2_GetPort(This,ppPort)	\
    (This)->lpVtbl -> GetPort(This,ppPort)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProcess2_GetInfo_Proxy( 
    IDebugProcess2 * This,
    /* [in] */ PROCESS_INFO_FIELDS Fields,
    /* [out] */ PROCESS_INFO *pProcessInfo);


void __RPC_STUB IDebugProcess2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_EnumPrograms_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ IEnumDebugPrograms2 **ppEnum);


void __RPC_STUB IDebugProcess2_EnumPrograms_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_GetName_Proxy( 
    IDebugProcess2 * This,
    /* [in] */ GETNAME_TYPE gnType,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugProcess2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_GetMachine_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ IDebugMachine2 **ppMachine);


void __RPC_STUB IDebugProcess2_GetMachine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_Terminate_Proxy( 
    IDebugProcess2 * This);


void __RPC_STUB IDebugProcess2_Terminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_Attach_Proxy( 
    IDebugProcess2 * This,
    /* [in] */ IDebugEventCallback2 *pCallback,
    /* [size_is][in] */ GUID *rgguidSpecificEngines,
    /* [in] */ DWORD celtSpecificEngines,
    /* [length_is][size_is][out] */ HRESULT *rghrEngineAttach);


void __RPC_STUB IDebugProcess2_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_CanDetach_Proxy( 
    IDebugProcess2 * This);


void __RPC_STUB IDebugProcess2_CanDetach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_Detach_Proxy( 
    IDebugProcess2 * This);


void __RPC_STUB IDebugProcess2_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_GetPhysicalProcessId_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ AD_PROCESS_ID *pProcessId);


void __RPC_STUB IDebugProcess2_GetPhysicalProcessId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_GetProcessId_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ GUID *pguidProcessId);


void __RPC_STUB IDebugProcess2_GetProcessId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_GetAttachedSessionName_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ BSTR *pbstrSessionName);


void __RPC_STUB IDebugProcess2_GetAttachedSessionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_EnumThreads_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ IEnumDebugThreads2 **ppEnum);


void __RPC_STUB IDebugProcess2_EnumThreads_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_CauseBreak_Proxy( 
    IDebugProcess2 * This);


void __RPC_STUB IDebugProcess2_CauseBreak_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProcess2_GetPort_Proxy( 
    IDebugProcess2 * This,
    /* [out] */ IDebugPort2 **ppPort);


void __RPC_STUB IDebugProcess2_GetPort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProcess2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgram2_INTERFACE_DEFINED__
#define __IDebugProgram2_INTERFACE_DEFINED__

/* interface IDebugProgram2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugProgram2_0001
    {	STEP_INTO	= 0,
	STEP_OVER	= STEP_INTO + 1,
	STEP_OUT	= STEP_OVER + 1,
	STEP_BACKWARDS	= STEP_OUT + 1
    } ;
typedef DWORD STEPKIND;


enum __MIDL_IDebugProgram2_0002
    {	STEP_STATEMENT	= 0,
	STEP_LINE	= STEP_STATEMENT + 1,
	STEP_INSTRUCTION	= STEP_LINE + 1
    } ;
typedef DWORD STEPUNIT;


enum __MIDL_IDebugProgram2_0003
    {	DSS_HUGE	= 0x10000000,
	DSS_FUNCTION	= 0x1,
	DSS_MODULE	= DSS_HUGE | 0x2,
	DSS_ALL	= DSS_HUGE | 0x3
    } ;
typedef DWORD DISASSEMBLY_STREAM_SCOPE;

typedef struct tagCODE_PATH
    {
    BSTR bstrName;
    IDebugCodeContext2 *pCode;
    } 	CODE_PATH;


enum __MIDL_IDebugProgram2_0004
    {	DUMP_MINIDUMP	= 0,
	DUMP_FULLDUMP	= DUMP_MINIDUMP + 1
    } ;
typedef DWORD DUMPTYPE;


EXTERN_C const IID IID_IDebugProgram2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("69d172ef-f2c4-44e1-89f7-c86231e706e9")
    IDebugProgram2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumThreads( 
            /* [out] */ IEnumDebugThreads2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProcess( 
            /* [out] */ IDebugProcess2 **ppProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Terminate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Attach( 
            /* [in] */ IDebugEventCallback2 *pCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanDetach( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Detach( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProgramId( 
            /* [out] */ GUID *pguidProgramId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDebugProperty( 
            /* [out] */ IDebugProperty2 **ppProperty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Execute( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Continue( 
            /* [in] */ IDebugThread2 *pThread) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Step( 
            /* [in] */ IDebugThread2 *pThread,
            /* [in] */ STEPKIND sk,
            /* [in] */ STEPUNIT step) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CauseBreak( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEngineInfo( 
            /* [out] */ BSTR *pbstrEngine,
            /* [out] */ GUID *pguidEngine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCodeContexts( 
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMemoryBytes( 
            /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisassemblyStream( 
            /* [in] */ DISASSEMBLY_STREAM_SCOPE dwScope,
            /* [in] */ IDebugCodeContext2 *pCodeContext,
            /* [out] */ IDebugDisassemblyStream2 **ppDisassemblyStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumModules( 
            /* [out] */ IEnumDebugModules2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetENCUpdate( 
            /* [out] */ IDebugENCUpdate **ppUpdate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCodePaths( 
            /* [full][in] */ LPCOLESTR pszHint,
            /* [in] */ IDebugCodeContext2 *pStart,
            /* [in] */ IDebugStackFrame2 *pFrame,
            /* [in] */ BOOL fSource,
            /* [out] */ IEnumCodePaths2 **ppEnum,
            /* [out] */ IDebugCodeContext2 **ppSafety) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteDump( 
            /* [in] */ DUMPTYPE DumpType,
            /* [in] */ LPCOLESTR pszDumpUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgram2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgram2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgram2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumThreads )( 
            IDebugProgram2 * This,
            /* [out] */ IEnumDebugThreads2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugProgram2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetProcess )( 
            IDebugProgram2 * This,
            /* [out] */ IDebugProcess2 **ppProcess);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            IDebugProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Attach )( 
            IDebugProgram2 * This,
            /* [in] */ IDebugEventCallback2 *pCallback);
        
        HRESULT ( STDMETHODCALLTYPE *CanDetach )( 
            IDebugProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Detach )( 
            IDebugProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProgramId )( 
            IDebugProgram2 * This,
            /* [out] */ GUID *pguidProgramId);
        
        HRESULT ( STDMETHODCALLTYPE *GetDebugProperty )( 
            IDebugProgram2 * This,
            /* [out] */ IDebugProperty2 **ppProperty);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            IDebugProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Continue )( 
            IDebugProgram2 * This,
            /* [in] */ IDebugThread2 *pThread);
        
        HRESULT ( STDMETHODCALLTYPE *Step )( 
            IDebugProgram2 * This,
            /* [in] */ IDebugThread2 *pThread,
            /* [in] */ STEPKIND sk,
            /* [in] */ STEPUNIT step);
        
        HRESULT ( STDMETHODCALLTYPE *CauseBreak )( 
            IDebugProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetEngineInfo )( 
            IDebugProgram2 * This,
            /* [out] */ BSTR *pbstrEngine,
            /* [out] */ GUID *pguidEngine);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCodeContexts )( 
            IDebugProgram2 * This,
            /* [in] */ IDebugDocumentPosition2 *pDocPos,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetMemoryBytes )( 
            IDebugProgram2 * This,
            /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisassemblyStream )( 
            IDebugProgram2 * This,
            /* [in] */ DISASSEMBLY_STREAM_SCOPE dwScope,
            /* [in] */ IDebugCodeContext2 *pCodeContext,
            /* [out] */ IDebugDisassemblyStream2 **ppDisassemblyStream);
        
        HRESULT ( STDMETHODCALLTYPE *EnumModules )( 
            IDebugProgram2 * This,
            /* [out] */ IEnumDebugModules2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetENCUpdate )( 
            IDebugProgram2 * This,
            /* [out] */ IDebugENCUpdate **ppUpdate);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCodePaths )( 
            IDebugProgram2 * This,
            /* [full][in] */ LPCOLESTR pszHint,
            /* [in] */ IDebugCodeContext2 *pStart,
            /* [in] */ IDebugStackFrame2 *pFrame,
            /* [in] */ BOOL fSource,
            /* [out] */ IEnumCodePaths2 **ppEnum,
            /* [out] */ IDebugCodeContext2 **ppSafety);
        
        HRESULT ( STDMETHODCALLTYPE *WriteDump )( 
            IDebugProgram2 * This,
            /* [in] */ DUMPTYPE DumpType,
            /* [in] */ LPCOLESTR pszDumpUrl);
        
        END_INTERFACE
    } IDebugProgram2Vtbl;

    interface IDebugProgram2
    {
        CONST_VTBL struct IDebugProgram2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgram2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgram2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgram2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProgram2_EnumThreads(This,ppEnum)	\
    (This)->lpVtbl -> EnumThreads(This,ppEnum)

#define IDebugProgram2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugProgram2_GetProcess(This,ppProcess)	\
    (This)->lpVtbl -> GetProcess(This,ppProcess)

#define IDebugProgram2_Terminate(This)	\
    (This)->lpVtbl -> Terminate(This)

#define IDebugProgram2_Attach(This,pCallback)	\
    (This)->lpVtbl -> Attach(This,pCallback)

#define IDebugProgram2_CanDetach(This)	\
    (This)->lpVtbl -> CanDetach(This)

#define IDebugProgram2_Detach(This)	\
    (This)->lpVtbl -> Detach(This)

#define IDebugProgram2_GetProgramId(This,pguidProgramId)	\
    (This)->lpVtbl -> GetProgramId(This,pguidProgramId)

#define IDebugProgram2_GetDebugProperty(This,ppProperty)	\
    (This)->lpVtbl -> GetDebugProperty(This,ppProperty)

#define IDebugProgram2_Execute(This)	\
    (This)->lpVtbl -> Execute(This)

#define IDebugProgram2_Continue(This,pThread)	\
    (This)->lpVtbl -> Continue(This,pThread)

#define IDebugProgram2_Step(This,pThread,sk,step)	\
    (This)->lpVtbl -> Step(This,pThread,sk,step)

#define IDebugProgram2_CauseBreak(This)	\
    (This)->lpVtbl -> CauseBreak(This)

#define IDebugProgram2_GetEngineInfo(This,pbstrEngine,pguidEngine)	\
    (This)->lpVtbl -> GetEngineInfo(This,pbstrEngine,pguidEngine)

#define IDebugProgram2_EnumCodeContexts(This,pDocPos,ppEnum)	\
    (This)->lpVtbl -> EnumCodeContexts(This,pDocPos,ppEnum)

#define IDebugProgram2_GetMemoryBytes(This,ppMemoryBytes)	\
    (This)->lpVtbl -> GetMemoryBytes(This,ppMemoryBytes)

#define IDebugProgram2_GetDisassemblyStream(This,dwScope,pCodeContext,ppDisassemblyStream)	\
    (This)->lpVtbl -> GetDisassemblyStream(This,dwScope,pCodeContext,ppDisassemblyStream)

#define IDebugProgram2_EnumModules(This,ppEnum)	\
    (This)->lpVtbl -> EnumModules(This,ppEnum)

#define IDebugProgram2_GetENCUpdate(This,ppUpdate)	\
    (This)->lpVtbl -> GetENCUpdate(This,ppUpdate)

#define IDebugProgram2_EnumCodePaths(This,pszHint,pStart,pFrame,fSource,ppEnum,ppSafety)	\
    (This)->lpVtbl -> EnumCodePaths(This,pszHint,pStart,pFrame,fSource,ppEnum,ppSafety)

#define IDebugProgram2_WriteDump(This,DumpType,pszDumpUrl)	\
    (This)->lpVtbl -> WriteDump(This,DumpType,pszDumpUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProgram2_EnumThreads_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ IEnumDebugThreads2 **ppEnum);


void __RPC_STUB IDebugProgram2_EnumThreads_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetName_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugProgram2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetProcess_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ IDebugProcess2 **ppProcess);


void __RPC_STUB IDebugProgram2_GetProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_Terminate_Proxy( 
    IDebugProgram2 * This);


void __RPC_STUB IDebugProgram2_Terminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_Attach_Proxy( 
    IDebugProgram2 * This,
    /* [in] */ IDebugEventCallback2 *pCallback);


void __RPC_STUB IDebugProgram2_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_CanDetach_Proxy( 
    IDebugProgram2 * This);


void __RPC_STUB IDebugProgram2_CanDetach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_Detach_Proxy( 
    IDebugProgram2 * This);


void __RPC_STUB IDebugProgram2_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetProgramId_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ GUID *pguidProgramId);


void __RPC_STUB IDebugProgram2_GetProgramId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetDebugProperty_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ IDebugProperty2 **ppProperty);


void __RPC_STUB IDebugProgram2_GetDebugProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_Execute_Proxy( 
    IDebugProgram2 * This);


void __RPC_STUB IDebugProgram2_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_Continue_Proxy( 
    IDebugProgram2 * This,
    /* [in] */ IDebugThread2 *pThread);


void __RPC_STUB IDebugProgram2_Continue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_Step_Proxy( 
    IDebugProgram2 * This,
    /* [in] */ IDebugThread2 *pThread,
    /* [in] */ STEPKIND sk,
    /* [in] */ STEPUNIT step);


void __RPC_STUB IDebugProgram2_Step_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_CauseBreak_Proxy( 
    IDebugProgram2 * This);


void __RPC_STUB IDebugProgram2_CauseBreak_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetEngineInfo_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ BSTR *pbstrEngine,
    /* [out] */ GUID *pguidEngine);


void __RPC_STUB IDebugProgram2_GetEngineInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_EnumCodeContexts_Proxy( 
    IDebugProgram2 * This,
    /* [in] */ IDebugDocumentPosition2 *pDocPos,
    /* [out] */ IEnumDebugCodeContexts2 **ppEnum);


void __RPC_STUB IDebugProgram2_EnumCodeContexts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetMemoryBytes_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes);


void __RPC_STUB IDebugProgram2_GetMemoryBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetDisassemblyStream_Proxy( 
    IDebugProgram2 * This,
    /* [in] */ DISASSEMBLY_STREAM_SCOPE dwScope,
    /* [in] */ IDebugCodeContext2 *pCodeContext,
    /* [out] */ IDebugDisassemblyStream2 **ppDisassemblyStream);


void __RPC_STUB IDebugProgram2_GetDisassemblyStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_EnumModules_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ IEnumDebugModules2 **ppEnum);


void __RPC_STUB IDebugProgram2_EnumModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_GetENCUpdate_Proxy( 
    IDebugProgram2 * This,
    /* [out] */ IDebugENCUpdate **ppUpdate);


void __RPC_STUB IDebugProgram2_GetENCUpdate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_EnumCodePaths_Proxy( 
    IDebugProgram2 * This,
    /* [full][in] */ LPCOLESTR pszHint,
    /* [in] */ IDebugCodeContext2 *pStart,
    /* [in] */ IDebugStackFrame2 *pFrame,
    /* [in] */ BOOL fSource,
    /* [out] */ IEnumCodePaths2 **ppEnum,
    /* [out] */ IDebugCodeContext2 **ppSafety);


void __RPC_STUB IDebugProgram2_EnumCodePaths_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgram2_WriteDump_Proxy( 
    IDebugProgram2 * This,
    /* [in] */ DUMPTYPE DumpType,
    /* [in] */ LPCOLESTR pszDumpUrl);


void __RPC_STUB IDebugProgram2_WriteDump_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProgram2_INTERFACE_DEFINED__ */


#ifndef __IDebugEngineProgram2_INTERFACE_DEFINED__
#define __IDebugEngineProgram2_INTERFACE_DEFINED__

/* interface IDebugEngineProgram2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugEngineProgram2_0001
    {	WATCHFOREVAL_LEAF_PROGRAM	= 0x10000000
    } ;

EXTERN_C const IID IID_IDebugEngineProgram2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7ce3e768-654d-4ba7-8d95-cdaac642b141")
    IDebugEngineProgram2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WatchForThreadStep( 
            /* [in] */ IDebugProgram2 *pOriginatingProgram,
            /* [in] */ DWORD dwTid,
            /* [in] */ BOOL fWatch,
            /* [in] */ DWORD dwFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WatchForExpressionEvaluationOnThread( 
            /* [in] */ IDebugProgram2 *pOriginatingProgram,
            /* [in] */ DWORD dwTid,
            /* [in] */ DWORD dwEvalFlags,
            /* [in] */ IDebugEventCallback2 *pExprCallback,
            /* [in] */ BOOL fWatch) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugEngineProgram2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugEngineProgram2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugEngineProgram2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugEngineProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IDebugEngineProgram2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *WatchForThreadStep )( 
            IDebugEngineProgram2 * This,
            /* [in] */ IDebugProgram2 *pOriginatingProgram,
            /* [in] */ DWORD dwTid,
            /* [in] */ BOOL fWatch,
            /* [in] */ DWORD dwFrame);
        
        HRESULT ( STDMETHODCALLTYPE *WatchForExpressionEvaluationOnThread )( 
            IDebugEngineProgram2 * This,
            /* [in] */ IDebugProgram2 *pOriginatingProgram,
            /* [in] */ DWORD dwTid,
            /* [in] */ DWORD dwEvalFlags,
            /* [in] */ IDebugEventCallback2 *pExprCallback,
            /* [in] */ BOOL fWatch);
        
        END_INTERFACE
    } IDebugEngineProgram2Vtbl;

    interface IDebugEngineProgram2
    {
        CONST_VTBL struct IDebugEngineProgram2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugEngineProgram2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugEngineProgram2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugEngineProgram2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugEngineProgram2_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IDebugEngineProgram2_WatchForThreadStep(This,pOriginatingProgram,dwTid,fWatch,dwFrame)	\
    (This)->lpVtbl -> WatchForThreadStep(This,pOriginatingProgram,dwTid,fWatch,dwFrame)

#define IDebugEngineProgram2_WatchForExpressionEvaluationOnThread(This,pOriginatingProgram,dwTid,dwEvalFlags,pExprCallback,fWatch)	\
    (This)->lpVtbl -> WatchForExpressionEvaluationOnThread(This,pOriginatingProgram,dwTid,dwEvalFlags,pExprCallback,fWatch)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugEngineProgram2_Stop_Proxy( 
    IDebugEngineProgram2 * This);


void __RPC_STUB IDebugEngineProgram2_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngineProgram2_WatchForThreadStep_Proxy( 
    IDebugEngineProgram2 * This,
    /* [in] */ IDebugProgram2 *pOriginatingProgram,
    /* [in] */ DWORD dwTid,
    /* [in] */ BOOL fWatch,
    /* [in] */ DWORD dwFrame);


void __RPC_STUB IDebugEngineProgram2_WatchForThreadStep_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugEngineProgram2_WatchForExpressionEvaluationOnThread_Proxy( 
    IDebugEngineProgram2 * This,
    /* [in] */ IDebugProgram2 *pOriginatingProgram,
    /* [in] */ DWORD dwTid,
    /* [in] */ DWORD dwEvalFlags,
    /* [in] */ IDebugEventCallback2 *pExprCallback,
    /* [in] */ BOOL fWatch);


void __RPC_STUB IDebugEngineProgram2_WatchForExpressionEvaluationOnThread_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugEngineProgram2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgramHost2_INTERFACE_DEFINED__
#define __IDebugProgramHost2_INTERFACE_DEFINED__

/* interface IDebugProgramHost2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProgramHost2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c99d588f-778c-44fe-8b2e-40124a738891")
    IDebugProgramHost2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetHostName( 
            /* [in] */ DWORD dwType,
            /* [out] */ BSTR *pbstrHostName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHostId( 
            /* [out] */ AD_PROCESS_ID *pProcessId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHostMachineName( 
            /* [out] */ BSTR *pbstrHostMachineName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgramHost2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgramHost2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgramHost2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgramHost2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostName )( 
            IDebugProgramHost2 * This,
            /* [in] */ DWORD dwType,
            /* [out] */ BSTR *pbstrHostName);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostId )( 
            IDebugProgramHost2 * This,
            /* [out] */ AD_PROCESS_ID *pProcessId);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostMachineName )( 
            IDebugProgramHost2 * This,
            /* [out] */ BSTR *pbstrHostMachineName);
        
        END_INTERFACE
    } IDebugProgramHost2Vtbl;

    interface IDebugProgramHost2
    {
        CONST_VTBL struct IDebugProgramHost2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgramHost2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgramHost2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgramHost2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProgramHost2_GetHostName(This,dwType,pbstrHostName)	\
    (This)->lpVtbl -> GetHostName(This,dwType,pbstrHostName)

#define IDebugProgramHost2_GetHostId(This,pProcessId)	\
    (This)->lpVtbl -> GetHostId(This,pProcessId)

#define IDebugProgramHost2_GetHostMachineName(This,pbstrHostMachineName)	\
    (This)->lpVtbl -> GetHostMachineName(This,pbstrHostMachineName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProgramHost2_GetHostName_Proxy( 
    IDebugProgramHost2 * This,
    /* [in] */ DWORD dwType,
    /* [out] */ BSTR *pbstrHostName);


void __RPC_STUB IDebugProgramHost2_GetHostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramHost2_GetHostId_Proxy( 
    IDebugProgramHost2 * This,
    /* [out] */ AD_PROCESS_ID *pProcessId);


void __RPC_STUB IDebugProgramHost2_GetHostId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramHost2_GetHostMachineName_Proxy( 
    IDebugProgramHost2 * This,
    /* [out] */ BSTR *pbstrHostMachineName);


void __RPC_STUB IDebugProgramHost2_GetHostMachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProgramHost2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgramNode2_INTERFACE_DEFINED__
#define __IDebugProgramNode2_INTERFACE_DEFINED__

/* interface IDebugProgramNode2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugProgramNode2_0001
    {	GHN_FRIENDLY_NAME	= 0,
	GHN_FILE_NAME	= GHN_FRIENDLY_NAME + 1
    } ;
typedef DWORD GETHOSTNAME_TYPE;


EXTERN_C const IID IID_IDebugProgramNode2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("426e255c-f1ce-4d02-a931-f9a254bf7f0f")
    IDebugProgramNode2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetProgramName( 
            /* [out] */ BSTR *pbstrProgramName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHostName( 
            /* [in] */ GETHOSTNAME_TYPE dwHostNameType,
            /* [out] */ BSTR *pbstrHostName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHostPid( 
            /* [out] */ AD_PROCESS_ID *pHostProcessId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHostMachineName( 
            /* [out] */ BSTR *pbstrHostMachineName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Attach( 
            /* [in] */ IDebugProgram2 *pMDMProgram,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [in] */ DWORD dwReason) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEngineInfo( 
            /* [out] */ BSTR *pbstrEngine,
            /* [out] */ GUID *pguidEngine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DetachDebugger( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgramNode2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgramNode2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgramNode2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgramNode2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProgramName )( 
            IDebugProgramNode2 * This,
            /* [out] */ BSTR *pbstrProgramName);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostName )( 
            IDebugProgramNode2 * This,
            /* [in] */ GETHOSTNAME_TYPE dwHostNameType,
            /* [out] */ BSTR *pbstrHostName);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostPid )( 
            IDebugProgramNode2 * This,
            /* [out] */ AD_PROCESS_ID *pHostProcessId);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostMachineName )( 
            IDebugProgramNode2 * This,
            /* [out] */ BSTR *pbstrHostMachineName);
        
        HRESULT ( STDMETHODCALLTYPE *Attach )( 
            IDebugProgramNode2 * This,
            /* [in] */ IDebugProgram2 *pMDMProgram,
            /* [in] */ IDebugEventCallback2 *pCallback,
            /* [in] */ DWORD dwReason);
        
        HRESULT ( STDMETHODCALLTYPE *GetEngineInfo )( 
            IDebugProgramNode2 * This,
            /* [out] */ BSTR *pbstrEngine,
            /* [out] */ GUID *pguidEngine);
        
        HRESULT ( STDMETHODCALLTYPE *DetachDebugger )( 
            IDebugProgramNode2 * This);
        
        END_INTERFACE
    } IDebugProgramNode2Vtbl;

    interface IDebugProgramNode2
    {
        CONST_VTBL struct IDebugProgramNode2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgramNode2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgramNode2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgramNode2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProgramNode2_GetProgramName(This,pbstrProgramName)	\
    (This)->lpVtbl -> GetProgramName(This,pbstrProgramName)

#define IDebugProgramNode2_GetHostName(This,dwHostNameType,pbstrHostName)	\
    (This)->lpVtbl -> GetHostName(This,dwHostNameType,pbstrHostName)

#define IDebugProgramNode2_GetHostPid(This,pHostProcessId)	\
    (This)->lpVtbl -> GetHostPid(This,pHostProcessId)

#define IDebugProgramNode2_GetHostMachineName(This,pbstrHostMachineName)	\
    (This)->lpVtbl -> GetHostMachineName(This,pbstrHostMachineName)

#define IDebugProgramNode2_Attach(This,pMDMProgram,pCallback,dwReason)	\
    (This)->lpVtbl -> Attach(This,pMDMProgram,pCallback,dwReason)

#define IDebugProgramNode2_GetEngineInfo(This,pbstrEngine,pguidEngine)	\
    (This)->lpVtbl -> GetEngineInfo(This,pbstrEngine,pguidEngine)

#define IDebugProgramNode2_DetachDebugger(This)	\
    (This)->lpVtbl -> DetachDebugger(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProgramNode2_GetProgramName_Proxy( 
    IDebugProgramNode2 * This,
    /* [out] */ BSTR *pbstrProgramName);


void __RPC_STUB IDebugProgramNode2_GetProgramName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramNode2_GetHostName_Proxy( 
    IDebugProgramNode2 * This,
    /* [in] */ GETHOSTNAME_TYPE dwHostNameType,
    /* [out] */ BSTR *pbstrHostName);


void __RPC_STUB IDebugProgramNode2_GetHostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramNode2_GetHostPid_Proxy( 
    IDebugProgramNode2 * This,
    /* [out] */ AD_PROCESS_ID *pHostProcessId);


void __RPC_STUB IDebugProgramNode2_GetHostPid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramNode2_GetHostMachineName_Proxy( 
    IDebugProgramNode2 * This,
    /* [out] */ BSTR *pbstrHostMachineName);


void __RPC_STUB IDebugProgramNode2_GetHostMachineName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramNode2_Attach_Proxy( 
    IDebugProgramNode2 * This,
    /* [in] */ IDebugProgram2 *pMDMProgram,
    /* [in] */ IDebugEventCallback2 *pCallback,
    /* [in] */ DWORD dwReason);


void __RPC_STUB IDebugProgramNode2_Attach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramNode2_GetEngineInfo_Proxy( 
    IDebugProgramNode2 * This,
    /* [out] */ BSTR *pbstrEngine,
    /* [out] */ GUID *pguidEngine);


void __RPC_STUB IDebugProgramNode2_GetEngineInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramNode2_DetachDebugger_Proxy( 
    IDebugProgramNode2 * This);


void __RPC_STUB IDebugProgramNode2_DetachDebugger_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProgramNode2_INTERFACE_DEFINED__ */


#ifndef __IDebugProgramEngines2_INTERFACE_DEFINED__
#define __IDebugProgramEngines2_INTERFACE_DEFINED__

/* interface IDebugProgramEngines2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProgramEngines2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fda24a6b-b142-447d-bbbc-8654a3d84f80")
    IDebugProgramEngines2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumPossibleEngines( 
            /* [in] */ DWORD celtBuffer,
            /* [length_is][size_is][full][out][in] */ GUID *rgguidEngines,
            /* [out][in] */ DWORD *pceltEngines) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEngine( 
            /* [in] */ REFGUID guidEngine) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProgramEngines2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProgramEngines2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProgramEngines2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProgramEngines2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumPossibleEngines )( 
            IDebugProgramEngines2 * This,
            /* [in] */ DWORD celtBuffer,
            /* [length_is][size_is][full][out][in] */ GUID *rgguidEngines,
            /* [out][in] */ DWORD *pceltEngines);
        
        HRESULT ( STDMETHODCALLTYPE *SetEngine )( 
            IDebugProgramEngines2 * This,
            /* [in] */ REFGUID guidEngine);
        
        END_INTERFACE
    } IDebugProgramEngines2Vtbl;

    interface IDebugProgramEngines2
    {
        CONST_VTBL struct IDebugProgramEngines2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProgramEngines2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProgramEngines2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProgramEngines2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProgramEngines2_EnumPossibleEngines(This,celtBuffer,rgguidEngines,pceltEngines)	\
    (This)->lpVtbl -> EnumPossibleEngines(This,celtBuffer,rgguidEngines,pceltEngines)

#define IDebugProgramEngines2_SetEngine(This,guidEngine)	\
    (This)->lpVtbl -> SetEngine(This,guidEngine)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProgramEngines2_EnumPossibleEngines_Proxy( 
    IDebugProgramEngines2 * This,
    /* [in] */ DWORD celtBuffer,
    /* [length_is][size_is][full][out][in] */ GUID *rgguidEngines,
    /* [out][in] */ DWORD *pceltEngines);


void __RPC_STUB IDebugProgramEngines2_EnumPossibleEngines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProgramEngines2_SetEngine_Proxy( 
    IDebugProgramEngines2 * This,
    /* [in] */ REFGUID guidEngine);


void __RPC_STUB IDebugProgramEngines2_SetEngine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProgramEngines2_INTERFACE_DEFINED__ */


#ifndef __IDebugThread2_INTERFACE_DEFINED__
#define __IDebugThread2_INTERFACE_DEFINED__

/* interface IDebugThread2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugThread2_0001
    {	THREADSTATE_RUNNING	= 0x1,
	THREADSTATE_STOPPED	= 0x2,
	THREADSTATE_FRESH	= 0x3,
	THREADSTATE_DEAD	= 0x4,
	THREADSTATE_FROZEN	= 0x5
    } ;
typedef DWORD THREADSTATE;


enum __MIDL_IDebugThread2_0002
    {	TPF_ID	= 0x1,
	TPF_SUSPENDCOUNT	= 0x2,
	TPF_STATE	= 0x4,
	TPF_PRIORITY	= 0x8,
	TPF_NAME	= 0x10,
	TPF_LOCATION	= 0x20,
	TPF_ALLFIELDS	= 0xffffffff
    } ;
typedef DWORD THREADPROPERTY_FIELDS;

typedef struct _tagTHREADPROPERTIES
    {
    THREADPROPERTY_FIELDS dwFields;
    DWORD dwThreadId;
    DWORD dwSuspendCount;
    DWORD dwThreadState;
    BSTR bstrPriority;
    BSTR bstrName;
    BSTR bstrLocation;
    } 	THREADPROPERTIES;


enum __MIDL_IDebugThread2_0003
    {	FIF_FUNCNAME	= 0x1,
	FIF_RETURNTYPE	= 0x2,
	FIF_ARGS	= 0x4,
	FIF_LANGUAGE	= 0x8,
	FIF_MODULE	= 0x10,
	FIF_STACKRANGE	= 0x20,
	FIF_FRAME	= 0x40,
	FIF_DEBUGINFO	= 0x80,
	FIF_STALECODE	= 0x100,
	FIF_ANNOTATEDFRAME	= 0x200,
	FIF_DEBUG_MODULEP	= 0x400,
	FIF_FUNCNAME_FORMAT	= 0x1000,
	FIF_FUNCNAME_RETURNTYPE	= 0x2000,
	FIF_FUNCNAME_ARGS	= 0x4000,
	FIF_FUNCNAME_LANGUAGE	= 0x8000,
	FIF_FUNCNAME_MODULE	= 0x10000,
	FIF_FUNCNAME_LINES	= 0x20000,
	FIF_FUNCNAME_OFFSET	= 0x40000,
	FIF_FUNCNAME_ARGS_TYPES	= 0x100000,
	FIF_FUNCNAME_ARGS_NAMES	= 0x200000,
	FIF_FUNCNAME_ARGS_VALUES	= 0x400000,
	FIF_FUNCNAME_ARGS_ALL	= 0x700000,
	FIF_ARGS_TYPES	= 0x1000000,
	FIF_ARGS_NAMES	= 0x2000000,
	FIF_ARGS_VALUES	= 0x4000000,
	FIF_ARGS_ALL	= 0x7000000,
	FIF_ARGS_NOFORMAT	= 0x8000000
    } ;
typedef DWORD FRAMEINFO_FLAGS;

typedef struct tagFRAMEINFO
    {
    FRAMEINFO_FLAGS m_dwValidFields;
    BSTR m_bstrFuncName;
    BSTR m_bstrReturnType;
    BSTR m_bstrArgs;
    BSTR m_bstrLanguage;
    BSTR m_bstrModule;
    UINT64 m_addrMin;
    UINT64 m_addrMax;
    IDebugStackFrame2 *m_pFrame;
    IDebugModule2 *m_pModule;
    BOOL m_fHasDebugInfo;
    BOOL m_fStaleCode;
    BOOL m_fAnnotatedFrame;
    } 	FRAMEINFO;


EXTERN_C const IID IID_IDebugThread2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d5168050-a57a-465c-bea9-974f405eba13")
    IDebugThread2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumFrameInfo( 
            /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
            /* [in] */ UINT nRadix,
            /* [out] */ IEnumDebugFrameInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetThreadName( 
            /* [in] */ LPCOLESTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetProgram( 
            /* [out] */ IDebugProgram2 **ppProgram) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanSetNextStatement( 
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [in] */ IDebugCodeContext2 *pCodeContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNextStatement( 
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [in] */ IDebugCodeContext2 *pCodeContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThreadId( 
            /* [out] */ DWORD *pdwThreadId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Suspend( 
            /* [out] */ DWORD *pdwSuspendCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( 
            /* [out] */ DWORD *pdwSuspendCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThreadProperties( 
            /* [in] */ THREADPROPERTY_FIELDS dwFields,
            /* [out] */ THREADPROPERTIES *ptp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLogicalThread( 
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [out] */ IDebugLogicalThread2 **ppLogicalThread) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugThread2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugThread2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugThread2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugThread2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFrameInfo )( 
            IDebugThread2 * This,
            /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
            /* [in] */ UINT nRadix,
            /* [out] */ IEnumDebugFrameInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugThread2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *SetThreadName )( 
            IDebugThread2 * This,
            /* [in] */ LPCOLESTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetProgram )( 
            IDebugThread2 * This,
            /* [out] */ IDebugProgram2 **ppProgram);
        
        HRESULT ( STDMETHODCALLTYPE *CanSetNextStatement )( 
            IDebugThread2 * This,
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [in] */ IDebugCodeContext2 *pCodeContext);
        
        HRESULT ( STDMETHODCALLTYPE *SetNextStatement )( 
            IDebugThread2 * This,
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [in] */ IDebugCodeContext2 *pCodeContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetThreadId )( 
            IDebugThread2 * This,
            /* [out] */ DWORD *pdwThreadId);
        
        HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IDebugThread2 * This,
            /* [out] */ DWORD *pdwSuspendCount);
        
        HRESULT ( STDMETHODCALLTYPE *Resume )( 
            IDebugThread2 * This,
            /* [out] */ DWORD *pdwSuspendCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetThreadProperties )( 
            IDebugThread2 * This,
            /* [in] */ THREADPROPERTY_FIELDS dwFields,
            /* [out] */ THREADPROPERTIES *ptp);
        
        HRESULT ( STDMETHODCALLTYPE *GetLogicalThread )( 
            IDebugThread2 * This,
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [out] */ IDebugLogicalThread2 **ppLogicalThread);
        
        END_INTERFACE
    } IDebugThread2Vtbl;

    interface IDebugThread2
    {
        CONST_VTBL struct IDebugThread2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugThread2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugThread2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugThread2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugThread2_EnumFrameInfo(This,dwFieldSpec,nRadix,ppEnum)	\
    (This)->lpVtbl -> EnumFrameInfo(This,dwFieldSpec,nRadix,ppEnum)

#define IDebugThread2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugThread2_SetThreadName(This,pszName)	\
    (This)->lpVtbl -> SetThreadName(This,pszName)

#define IDebugThread2_GetProgram(This,ppProgram)	\
    (This)->lpVtbl -> GetProgram(This,ppProgram)

#define IDebugThread2_CanSetNextStatement(This,pStackFrame,pCodeContext)	\
    (This)->lpVtbl -> CanSetNextStatement(This,pStackFrame,pCodeContext)

#define IDebugThread2_SetNextStatement(This,pStackFrame,pCodeContext)	\
    (This)->lpVtbl -> SetNextStatement(This,pStackFrame,pCodeContext)

#define IDebugThread2_GetThreadId(This,pdwThreadId)	\
    (This)->lpVtbl -> GetThreadId(This,pdwThreadId)

#define IDebugThread2_Suspend(This,pdwSuspendCount)	\
    (This)->lpVtbl -> Suspend(This,pdwSuspendCount)

#define IDebugThread2_Resume(This,pdwSuspendCount)	\
    (This)->lpVtbl -> Resume(This,pdwSuspendCount)

#define IDebugThread2_GetThreadProperties(This,dwFields,ptp)	\
    (This)->lpVtbl -> GetThreadProperties(This,dwFields,ptp)

#define IDebugThread2_GetLogicalThread(This,pStackFrame,ppLogicalThread)	\
    (This)->lpVtbl -> GetLogicalThread(This,pStackFrame,ppLogicalThread)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugThread2_EnumFrameInfo_Proxy( 
    IDebugThread2 * This,
    /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
    /* [in] */ UINT nRadix,
    /* [out] */ IEnumDebugFrameInfo2 **ppEnum);


void __RPC_STUB IDebugThread2_EnumFrameInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_GetName_Proxy( 
    IDebugThread2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugThread2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_SetThreadName_Proxy( 
    IDebugThread2 * This,
    /* [in] */ LPCOLESTR pszName);


void __RPC_STUB IDebugThread2_SetThreadName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_GetProgram_Proxy( 
    IDebugThread2 * This,
    /* [out] */ IDebugProgram2 **ppProgram);


void __RPC_STUB IDebugThread2_GetProgram_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_CanSetNextStatement_Proxy( 
    IDebugThread2 * This,
    /* [in] */ IDebugStackFrame2 *pStackFrame,
    /* [in] */ IDebugCodeContext2 *pCodeContext);


void __RPC_STUB IDebugThread2_CanSetNextStatement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_SetNextStatement_Proxy( 
    IDebugThread2 * This,
    /* [in] */ IDebugStackFrame2 *pStackFrame,
    /* [in] */ IDebugCodeContext2 *pCodeContext);


void __RPC_STUB IDebugThread2_SetNextStatement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_GetThreadId_Proxy( 
    IDebugThread2 * This,
    /* [out] */ DWORD *pdwThreadId);


void __RPC_STUB IDebugThread2_GetThreadId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_Suspend_Proxy( 
    IDebugThread2 * This,
    /* [out] */ DWORD *pdwSuspendCount);


void __RPC_STUB IDebugThread2_Suspend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_Resume_Proxy( 
    IDebugThread2 * This,
    /* [out] */ DWORD *pdwSuspendCount);


void __RPC_STUB IDebugThread2_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_GetThreadProperties_Proxy( 
    IDebugThread2 * This,
    /* [in] */ THREADPROPERTY_FIELDS dwFields,
    /* [out] */ THREADPROPERTIES *ptp);


void __RPC_STUB IDebugThread2_GetThreadProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugThread2_GetLogicalThread_Proxy( 
    IDebugThread2 * This,
    /* [in] */ IDebugStackFrame2 *pStackFrame,
    /* [out] */ IDebugLogicalThread2 **ppLogicalThread);


void __RPC_STUB IDebugThread2_GetLogicalThread_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugThread2_INTERFACE_DEFINED__ */


#ifndef __IDebugLogicalThread2_INTERFACE_DEFINED__
#define __IDebugLogicalThread2_INTERFACE_DEFINED__

/* interface IDebugLogicalThread2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugLogicalThread2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88d2f75b-d329-4e03-9b75-201f7782d8bd")
    IDebugLogicalThread2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumFrameInfo( 
            /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
            /* [in] */ UINT nRadix,
            /* [out] */ IEnumDebugFrameInfo2 **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugLogicalThread2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugLogicalThread2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugLogicalThread2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugLogicalThread2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumFrameInfo )( 
            IDebugLogicalThread2 * This,
            /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
            /* [in] */ UINT nRadix,
            /* [out] */ IEnumDebugFrameInfo2 **ppEnum);
        
        END_INTERFACE
    } IDebugLogicalThread2Vtbl;

    interface IDebugLogicalThread2
    {
        CONST_VTBL struct IDebugLogicalThread2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugLogicalThread2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugLogicalThread2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugLogicalThread2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugLogicalThread2_EnumFrameInfo(This,dwFieldSpec,nRadix,ppEnum)	\
    (This)->lpVtbl -> EnumFrameInfo(This,dwFieldSpec,nRadix,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugLogicalThread2_EnumFrameInfo_Proxy( 
    IDebugLogicalThread2 * This,
    /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
    /* [in] */ UINT nRadix,
    /* [out] */ IEnumDebugFrameInfo2 **ppEnum);


void __RPC_STUB IDebugLogicalThread2_EnumFrameInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugLogicalThread2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_msdbg_0350 */
/* [local] */ 

#define DBG_ATTRIB_NONE					0x0000000000000000
#define DBG_ATTRIB_ALL					0xffffffffffffffff
#define DBG_ATTRIB_OBJ_IS_EXPANDABLE		0x0000000000000001
#define DBG_ATTRIB_VALUE_READONLY		0x0000000000000010
#define DBG_ATTRIB_VALUE_ERROR			0x0000000000000020
#define DBG_ATTRIB_VALUE_SIDE_EFFECT		0x0000000000000040
#define DBG_ATTRIB_OVERLOADED_CONTAINER	0x0000000000000080
#define DBG_ATTRIB_VALUE_BOOLEAN			0x0000000000000100
#define DBG_ATTRIB_VALUE_BOOLEAN_TRUE	0x0000000000000200
#define DBG_ATTRIB_VALUE_INVALID			0x0000000000000400
#define DBG_ATTRIB_VALUE_NAT				0x0000000000000800
#define DBG_ATTRIB_VALUE_AUTOEXPANDED	0x0000000000001000
#define DBG_ATTRIB_ACCESS_NONE			0x0000000000010000
#define DBG_ATTRIB_ACCESS_PUBLIC			0x0000000000020000
#define DBG_ATTRIB_ACCESS_PRIVATE		0x0000000000040000
#define DBG_ATTRIB_ACCESS_PROTECTED		0x0000000000080000
#define DBG_ATTRIB_ACCESS_FINAL			0x0000000000100000
#define DBG_ATTRIB_ACCESS_ALL			0x00000000001f0000
#define DBG_ATTRIB_STORAGE_NONE			0x0000000001000000
#define DBG_ATTRIB_STORAGE_GLOBAL		0x0000000002000000
#define DBG_ATTRIB_STORAGE_STATIC		0x0000000004000000
#define DBG_ATTRIB_STORAGE_REGISTER		0x0000000008000000
#define DBG_ATTRIB_STORAGE_ALL			0x000000000f000000
#define DBG_ATTRIB_TYPE_NONE				0x0000000100000000
#define DBG_ATTRIB_TYPE_VIRTUAL			0x0000000200000000
#define DBG_ATTRIB_TYPE_CONSTANT			0x0000000400000000
#define DBG_ATTRIB_TYPE_SYNCHRONIZED		0x0000000800000000
#define DBG_ATTRIB_TYPE_VOLATILE			0x0000001000000000
#define DBG_ATTRIB_TYPE_ALL				0x0000001f00000000
#define DBG_ATTRIB_DATA					0x0000010000000000
#define DBG_ATTRIB_METHOD				0x0000020000000000
#define DBG_ATTRIB_PROPERTY				0x0000040000000000
#define DBG_ATTRIB_CLASS					0x0000080000000000
#define DBG_ATTRIB_BASECLASS				0x0000100000000000
#define DBG_ATTRIB_INTERFACE				0x0000200000000000
#define DBG_ATTRIB_INNERCLASS			0x0000400000000000
#define DBG_ATTRIB_MOSTDERIVEDCLASS		0x0000800000000000
#define DBG_ATTRIB_CHILD_ALL				0x0000ff0000000000
typedef UINT64 DBG_ATTRIB_FLAGS;


enum __MIDL___MIDL_itf_msdbg_0350_0001
    {	DEBUGPROP_INFO_FULLNAME	= 0x1,
	DEBUGPROP_INFO_NAME	= 0x2,
	DEBUGPROP_INFO_TYPE	= 0x4,
	DEBUGPROP_INFO_VALUE	= 0x8,
	DEBUGPROP_INFO_ATTRIB	= 0x10,
	DEBUGPROP_INFO_PROP	= 0x20,
	DEBUGPROP_INFO_VALUE_AUTOEXPAND	= 0x10000,
	DEBUGPROP_INFO_NOFUNCEVAL	= 0x20000,
	DEBUGPROP_INFO_NONE	= 0,
	DEBUGPROP_INFO_STANDARD	= DEBUGPROP_INFO_ATTRIB | DEBUGPROP_INFO_NAME | DEBUGPROP_INFO_TYPE | DEBUGPROP_INFO_VALUE,
	DEBUGPROP_INFO_ALL	= 0xffffffff
    } ;
typedef DWORD DEBUGPROP_INFO_FLAGS;

typedef struct tagDEBUG_PROPERTY_INFO
    {
    DEBUGPROP_INFO_FLAGS dwFields;
    BSTR bstrFullName;
    BSTR bstrName;
    BSTR bstrType;
    BSTR bstrValue;
    IDebugProperty2 *pProperty;
    DBG_ATTRIB_FLAGS dwAttrib;
    } 	DEBUG_PROPERTY_INFO;



extern RPC_IF_HANDLE __MIDL_itf_msdbg_0350_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msdbg_0350_v0_0_s_ifspec;

#ifndef __IDebugProperty2_INTERFACE_DEFINED__
#define __IDebugProperty2_INTERFACE_DEFINED__

/* interface IDebugProperty2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugProperty2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a7ee3e7e-2dd2-4ad7-9697-f4aae3427762")
    IDebugProperty2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPropertyInfo( 
            /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout,
            /* [length_is][size_is][full][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [out] */ DEBUG_PROPERTY_INFO *pPropertyInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValueAsString( 
            /* [in] */ LPCOLESTR pszValue,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValueAsReference( 
            /* [length_is][size_is][full][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [in] */ IDebugReference2 *pValue,
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumChildren( 
            /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ REFGUID guidFilter,
            /* [in] */ DBG_ATTRIB_FLAGS dwAttribFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ DWORD dwTimeout,
            /* [out] */ IEnumDebugPropertyInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ IDebugProperty2 **ppParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDerivedMostProperty( 
            /* [out] */ IDebugProperty2 **ppDerivedMost) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMemoryBytes( 
            /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMemoryContext( 
            /* [out] */ IDebugMemoryContext2 **ppMemory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ DWORD *pdwSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetReference( 
            /* [out] */ IDebugReference2 **ppReference) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtendedInfo( 
            /* [in] */ REFGUID guidExtendedInfo,
            /* [out] */ VARIANT *pExtendedInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugProperty2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugProperty2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugProperty2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugProperty2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPropertyInfo )( 
            IDebugProperty2 * This,
            /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout,
            /* [length_is][size_is][full][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [out] */ DEBUG_PROPERTY_INFO *pPropertyInfo);
        
        HRESULT ( STDMETHODCALLTYPE *SetValueAsString )( 
            IDebugProperty2 * This,
            /* [in] */ LPCOLESTR pszValue,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE *SetValueAsReference )( 
            IDebugProperty2 * This,
            /* [length_is][size_is][full][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [in] */ IDebugReference2 *pValue,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE *EnumChildren )( 
            IDebugProperty2 * This,
            /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ REFGUID guidFilter,
            /* [in] */ DBG_ATTRIB_FLAGS dwAttribFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ DWORD dwTimeout,
            /* [out] */ IEnumDebugPropertyInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IDebugProperty2 * This,
            /* [out] */ IDebugProperty2 **ppParent);
        
        HRESULT ( STDMETHODCALLTYPE *GetDerivedMostProperty )( 
            IDebugProperty2 * This,
            /* [out] */ IDebugProperty2 **ppDerivedMost);
        
        HRESULT ( STDMETHODCALLTYPE *GetMemoryBytes )( 
            IDebugProperty2 * This,
            /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes);
        
        HRESULT ( STDMETHODCALLTYPE *GetMemoryContext )( 
            IDebugProperty2 * This,
            /* [out] */ IDebugMemoryContext2 **ppMemory);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugProperty2 * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetReference )( 
            IDebugProperty2 * This,
            /* [out] */ IDebugReference2 **ppReference);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtendedInfo )( 
            IDebugProperty2 * This,
            /* [in] */ REFGUID guidExtendedInfo,
            /* [out] */ VARIANT *pExtendedInfo);
        
        END_INTERFACE
    } IDebugProperty2Vtbl;

    interface IDebugProperty2
    {
        CONST_VTBL struct IDebugProperty2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugProperty2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugProperty2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugProperty2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugProperty2_GetPropertyInfo(This,dwFields,dwRadix,dwTimeout,rgpArgs,dwArgCount,pPropertyInfo)	\
    (This)->lpVtbl -> GetPropertyInfo(This,dwFields,dwRadix,dwTimeout,rgpArgs,dwArgCount,pPropertyInfo)

#define IDebugProperty2_SetValueAsString(This,pszValue,dwRadix,dwTimeout)	\
    (This)->lpVtbl -> SetValueAsString(This,pszValue,dwRadix,dwTimeout)

#define IDebugProperty2_SetValueAsReference(This,rgpArgs,dwArgCount,pValue,dwTimeout)	\
    (This)->lpVtbl -> SetValueAsReference(This,rgpArgs,dwArgCount,pValue,dwTimeout)

#define IDebugProperty2_EnumChildren(This,dwFields,dwRadix,guidFilter,dwAttribFilter,pszNameFilter,dwTimeout,ppEnum)	\
    (This)->lpVtbl -> EnumChildren(This,dwFields,dwRadix,guidFilter,dwAttribFilter,pszNameFilter,dwTimeout,ppEnum)

#define IDebugProperty2_GetParent(This,ppParent)	\
    (This)->lpVtbl -> GetParent(This,ppParent)

#define IDebugProperty2_GetDerivedMostProperty(This,ppDerivedMost)	\
    (This)->lpVtbl -> GetDerivedMostProperty(This,ppDerivedMost)

#define IDebugProperty2_GetMemoryBytes(This,ppMemoryBytes)	\
    (This)->lpVtbl -> GetMemoryBytes(This,ppMemoryBytes)

#define IDebugProperty2_GetMemoryContext(This,ppMemory)	\
    (This)->lpVtbl -> GetMemoryContext(This,ppMemory)

#define IDebugProperty2_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugProperty2_GetReference(This,ppReference)	\
    (This)->lpVtbl -> GetReference(This,ppReference)

#define IDebugProperty2_GetExtendedInfo(This,guidExtendedInfo,pExtendedInfo)	\
    (This)->lpVtbl -> GetExtendedInfo(This,guidExtendedInfo,pExtendedInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugProperty2_GetPropertyInfo_Proxy( 
    IDebugProperty2 * This,
    /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
    /* [in] */ DWORD dwRadix,
    /* [in] */ DWORD dwTimeout,
    /* [length_is][size_is][full][in] */ IDebugReference2 **rgpArgs,
    /* [in] */ DWORD dwArgCount,
    /* [out] */ DEBUG_PROPERTY_INFO *pPropertyInfo);


void __RPC_STUB IDebugProperty2_GetPropertyInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_SetValueAsString_Proxy( 
    IDebugProperty2 * This,
    /* [in] */ LPCOLESTR pszValue,
    /* [in] */ DWORD dwRadix,
    /* [in] */ DWORD dwTimeout);


void __RPC_STUB IDebugProperty2_SetValueAsString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_SetValueAsReference_Proxy( 
    IDebugProperty2 * This,
    /* [length_is][size_is][full][in] */ IDebugReference2 **rgpArgs,
    /* [in] */ DWORD dwArgCount,
    /* [in] */ IDebugReference2 *pValue,
    /* [in] */ DWORD dwTimeout);


void __RPC_STUB IDebugProperty2_SetValueAsReference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_EnumChildren_Proxy( 
    IDebugProperty2 * This,
    /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
    /* [in] */ DWORD dwRadix,
    /* [in] */ REFGUID guidFilter,
    /* [in] */ DBG_ATTRIB_FLAGS dwAttribFilter,
    /* [full][in] */ LPCOLESTR pszNameFilter,
    /* [in] */ DWORD dwTimeout,
    /* [out] */ IEnumDebugPropertyInfo2 **ppEnum);


void __RPC_STUB IDebugProperty2_EnumChildren_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetParent_Proxy( 
    IDebugProperty2 * This,
    /* [out] */ IDebugProperty2 **ppParent);


void __RPC_STUB IDebugProperty2_GetParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetDerivedMostProperty_Proxy( 
    IDebugProperty2 * This,
    /* [out] */ IDebugProperty2 **ppDerivedMost);


void __RPC_STUB IDebugProperty2_GetDerivedMostProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetMemoryBytes_Proxy( 
    IDebugProperty2 * This,
    /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes);


void __RPC_STUB IDebugProperty2_GetMemoryBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetMemoryContext_Proxy( 
    IDebugProperty2 * This,
    /* [out] */ IDebugMemoryContext2 **ppMemory);


void __RPC_STUB IDebugProperty2_GetMemoryContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetSize_Proxy( 
    IDebugProperty2 * This,
    /* [out] */ DWORD *pdwSize);


void __RPC_STUB IDebugProperty2_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetReference_Proxy( 
    IDebugProperty2 * This,
    /* [out] */ IDebugReference2 **ppReference);


void __RPC_STUB IDebugProperty2_GetReference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugProperty2_GetExtendedInfo_Proxy( 
    IDebugProperty2 * This,
    /* [in] */ REFGUID guidExtendedInfo,
    /* [out] */ VARIANT *pExtendedInfo);


void __RPC_STUB IDebugProperty2_GetExtendedInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugProperty2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_msdbg_0351 */
/* [local] */ 


enum __MIDL___MIDL_itf_msdbg_0351_0001
    {	REF_TYPE_WEAK	= 0x1,
	REF_TYPE_STRONG	= 0x2
    } ;
typedef DWORD REFERENCE_TYPE;


enum __MIDL___MIDL_itf_msdbg_0351_0002
    {	DEBUGREF_INFO_NAME	= 0x1,
	DEBUGREF_INFO_TYPE	= 0x2,
	DEBUGREF_INFO_VALUE	= 0x4,
	DEBUGREF_INFO_ATTRIB	= 0x8,
	DEBUGREF_INFO_REFTYPE	= 0x10,
	DEBUGREF_INFO_REF	= 0x20,
	DEBUGREF_INFO_VALUE_AUTOEXPAND	= 0x10000,
	DEBUGREF_INFO_NONE	= 0,
	DEBUGREF_INFO_ALL	= 0xffffffff
    } ;
typedef DWORD DEBUGREF_INFO_FLAGS;

typedef struct tagDEBUG_REFERENCE_INFO
    {
    DEBUGREF_INFO_FLAGS dwFields;
    BSTR bstrName;
    BSTR bstrType;
    BSTR bstrValue;
    DBG_ATTRIB_FLAGS dwAttrib;
    REFERENCE_TYPE dwRefType;
    IDebugReference2 *pReference;
    } 	DEBUG_REFERENCE_INFO;



extern RPC_IF_HANDLE __MIDL_itf_msdbg_0351_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msdbg_0351_v0_0_s_ifspec;

#ifndef __IDebugReference2_INTERFACE_DEFINED__
#define __IDebugReference2_INTERFACE_DEFINED__

/* interface IDebugReference2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugReference2_0003
    {	REF_COMPARE_EQUAL	= 0x1,
	REF_COMPARE_LESS_THAN	= 0x2,
	REF_COMPARE_GREATER_THAN	= 0x3
    } ;
typedef DWORD REFERENCE_COMPARE;


EXTERN_C const IID IID_IDebugReference2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("10b793ac-0c47-4679-8454-adb36f29f802")
    IDebugReference2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetReferenceInfo( 
            /* [in] */ DEBUGREF_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout,
            /* [length_is][size_is][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [out] */ DEBUG_REFERENCE_INFO *pReferenceInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValueAsString( 
            /* [in] */ LPCOLESTR pszValue,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValueAsReference( 
            /* [length_is][size_is][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [in] */ IDebugReference2 *pValue,
            /* [in] */ DWORD dwTimeout) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumChildren( 
            /* [in] */ DEBUGREF_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DBG_ATTRIB_FLAGS dwAttribFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ DWORD dwTimeout,
            /* [out] */ IEnumDebugReferenceInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ IDebugReference2 **ppParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDerivedMostReference( 
            /* [out] */ IDebugReference2 **ppDerivedMost) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMemoryBytes( 
            /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMemoryContext( 
            /* [out] */ IDebugMemoryContext2 **ppMemory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ DWORD *pdwSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetReferenceType( 
            /* [in] */ REFERENCE_TYPE dwRefType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Compare( 
            /* [in] */ REFERENCE_COMPARE dwCompare,
            /* [in] */ IDebugReference2 *pReference) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugReference2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugReference2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugReference2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugReference2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetReferenceInfo )( 
            IDebugReference2 * This,
            /* [in] */ DEBUGREF_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout,
            /* [length_is][size_is][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [out] */ DEBUG_REFERENCE_INFO *pReferenceInfo);
        
        HRESULT ( STDMETHODCALLTYPE *SetValueAsString )( 
            IDebugReference2 * This,
            /* [in] */ LPCOLESTR pszValue,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE *SetValueAsReference )( 
            IDebugReference2 * This,
            /* [length_is][size_is][in] */ IDebugReference2 **rgpArgs,
            /* [in] */ DWORD dwArgCount,
            /* [in] */ IDebugReference2 *pValue,
            /* [in] */ DWORD dwTimeout);
        
        HRESULT ( STDMETHODCALLTYPE *EnumChildren )( 
            IDebugReference2 * This,
            /* [in] */ DEBUGREF_INFO_FLAGS dwFields,
            /* [in] */ DWORD dwRadix,
            /* [in] */ DBG_ATTRIB_FLAGS dwAttribFilter,
            /* [full][in] */ LPCOLESTR pszNameFilter,
            /* [in] */ DWORD dwTimeout,
            /* [out] */ IEnumDebugReferenceInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IDebugReference2 * This,
            /* [out] */ IDebugReference2 **ppParent);
        
        HRESULT ( STDMETHODCALLTYPE *GetDerivedMostReference )( 
            IDebugReference2 * This,
            /* [out] */ IDebugReference2 **ppDerivedMost);
        
        HRESULT ( STDMETHODCALLTYPE *GetMemoryBytes )( 
            IDebugReference2 * This,
            /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes);
        
        HRESULT ( STDMETHODCALLTYPE *GetMemoryContext )( 
            IDebugReference2 * This,
            /* [out] */ IDebugMemoryContext2 **ppMemory);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugReference2 * This,
            /* [out] */ DWORD *pdwSize);
        
        HRESULT ( STDMETHODCALLTYPE *SetReferenceType )( 
            IDebugReference2 * This,
            /* [in] */ REFERENCE_TYPE dwRefType);
        
        HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IDebugReference2 * This,
            /* [in] */ REFERENCE_COMPARE dwCompare,
            /* [in] */ IDebugReference2 *pReference);
        
        END_INTERFACE
    } IDebugReference2Vtbl;

    interface IDebugReference2
    {
        CONST_VTBL struct IDebugReference2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugReference2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugReference2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugReference2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugReference2_GetReferenceInfo(This,dwFields,dwRadix,dwTimeout,rgpArgs,dwArgCount,pReferenceInfo)	\
    (This)->lpVtbl -> GetReferenceInfo(This,dwFields,dwRadix,dwTimeout,rgpArgs,dwArgCount,pReferenceInfo)

#define IDebugReference2_SetValueAsString(This,pszValue,dwRadix,dwTimeout)	\
    (This)->lpVtbl -> SetValueAsString(This,pszValue,dwRadix,dwTimeout)

#define IDebugReference2_SetValueAsReference(This,rgpArgs,dwArgCount,pValue,dwTimeout)	\
    (This)->lpVtbl -> SetValueAsReference(This,rgpArgs,dwArgCount,pValue,dwTimeout)

#define IDebugReference2_EnumChildren(This,dwFields,dwRadix,dwAttribFilter,pszNameFilter,dwTimeout,ppEnum)	\
    (This)->lpVtbl -> EnumChildren(This,dwFields,dwRadix,dwAttribFilter,pszNameFilter,dwTimeout,ppEnum)

#define IDebugReference2_GetParent(This,ppParent)	\
    (This)->lpVtbl -> GetParent(This,ppParent)

#define IDebugReference2_GetDerivedMostReference(This,ppDerivedMost)	\
    (This)->lpVtbl -> GetDerivedMostReference(This,ppDerivedMost)

#define IDebugReference2_GetMemoryBytes(This,ppMemoryBytes)	\
    (This)->lpVtbl -> GetMemoryBytes(This,ppMemoryBytes)

#define IDebugReference2_GetMemoryContext(This,ppMemory)	\
    (This)->lpVtbl -> GetMemoryContext(This,ppMemory)

#define IDebugReference2_GetSize(This,pdwSize)	\
    (This)->lpVtbl -> GetSize(This,pdwSize)

#define IDebugReference2_SetReferenceType(This,dwRefType)	\
    (This)->lpVtbl -> SetReferenceType(This,dwRefType)

#define IDebugReference2_Compare(This,dwCompare,pReference)	\
    (This)->lpVtbl -> Compare(This,dwCompare,pReference)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugReference2_GetReferenceInfo_Proxy( 
    IDebugReference2 * This,
    /* [in] */ DEBUGREF_INFO_FLAGS dwFields,
    /* [in] */ DWORD dwRadix,
    /* [in] */ DWORD dwTimeout,
    /* [length_is][size_is][in] */ IDebugReference2 **rgpArgs,
    /* [in] */ DWORD dwArgCount,
    /* [out] */ DEBUG_REFERENCE_INFO *pReferenceInfo);


void __RPC_STUB IDebugReference2_GetReferenceInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_SetValueAsString_Proxy( 
    IDebugReference2 * This,
    /* [in] */ LPCOLESTR pszValue,
    /* [in] */ DWORD dwRadix,
    /* [in] */ DWORD dwTimeout);


void __RPC_STUB IDebugReference2_SetValueAsString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_SetValueAsReference_Proxy( 
    IDebugReference2 * This,
    /* [length_is][size_is][in] */ IDebugReference2 **rgpArgs,
    /* [in] */ DWORD dwArgCount,
    /* [in] */ IDebugReference2 *pValue,
    /* [in] */ DWORD dwTimeout);


void __RPC_STUB IDebugReference2_SetValueAsReference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_EnumChildren_Proxy( 
    IDebugReference2 * This,
    /* [in] */ DEBUGREF_INFO_FLAGS dwFields,
    /* [in] */ DWORD dwRadix,
    /* [in] */ DBG_ATTRIB_FLAGS dwAttribFilter,
    /* [full][in] */ LPCOLESTR pszNameFilter,
    /* [in] */ DWORD dwTimeout,
    /* [out] */ IEnumDebugReferenceInfo2 **ppEnum);


void __RPC_STUB IDebugReference2_EnumChildren_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_GetParent_Proxy( 
    IDebugReference2 * This,
    /* [out] */ IDebugReference2 **ppParent);


void __RPC_STUB IDebugReference2_GetParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_GetDerivedMostReference_Proxy( 
    IDebugReference2 * This,
    /* [out] */ IDebugReference2 **ppDerivedMost);


void __RPC_STUB IDebugReference2_GetDerivedMostReference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_GetMemoryBytes_Proxy( 
    IDebugReference2 * This,
    /* [out] */ IDebugMemoryBytes2 **ppMemoryBytes);


void __RPC_STUB IDebugReference2_GetMemoryBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_GetMemoryContext_Proxy( 
    IDebugReference2 * This,
    /* [out] */ IDebugMemoryContext2 **ppMemory);


void __RPC_STUB IDebugReference2_GetMemoryContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_GetSize_Proxy( 
    IDebugReference2 * This,
    /* [out] */ DWORD *pdwSize);


void __RPC_STUB IDebugReference2_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_SetReferenceType_Proxy( 
    IDebugReference2 * This,
    /* [in] */ REFERENCE_TYPE dwRefType);


void __RPC_STUB IDebugReference2_SetReferenceType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugReference2_Compare_Proxy( 
    IDebugReference2 * This,
    /* [in] */ REFERENCE_COMPARE dwCompare,
    /* [in] */ IDebugReference2 *pReference);


void __RPC_STUB IDebugReference2_Compare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugReference2_INTERFACE_DEFINED__ */


#ifndef __IDebugStackFrame2_INTERFACE_DEFINED__
#define __IDebugStackFrame2_INTERFACE_DEFINED__

/* interface IDebugStackFrame2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugStackFrame2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1412926f-5dd6-4e58-b648-e1c63e013d51")
    IDebugStackFrame2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCodeContext( 
            /* [out] */ IDebugCodeContext2 **ppCodeCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocumentContext( 
            /* [out] */ IDebugDocumentContext2 **ppCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
            /* [in] */ UINT nRadix,
            /* [out] */ FRAMEINFO *pFrameInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPhysicalStackRange( 
            /* [out] */ UINT64 *paddrMin,
            /* [out] */ UINT64 *paddrMax) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExpressionContext( 
            /* [out] */ IDebugExpressionContext2 **ppExprCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLanguageInfo( 
            /* [full][out][in] */ BSTR *pbstrLanguage,
            /* [full][out][in] */ GUID *pguidLanguage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDebugProperty( 
            /* [out] */ IDebugProperty2 **ppProperty) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumProperties( 
            /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
            /* [in] */ UINT nRadix,
            /* [in] */ REFGUID guidFilter,
            /* [in] */ DWORD dwTimeout,
            /* [out] */ ULONG *pcelt,
            /* [out] */ IEnumDebugPropertyInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetThread( 
            /* [out] */ IDebugThread2 **ppThread) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugStackFrame2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugStackFrame2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugStackFrame2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugStackFrame2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCodeContext )( 
            IDebugStackFrame2 * This,
            /* [out] */ IDebugCodeContext2 **ppCodeCxt);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentContext )( 
            IDebugStackFrame2 * This,
            /* [out] */ IDebugDocumentContext2 **ppCxt);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugStackFrame2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugStackFrame2 * This,
            /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
            /* [in] */ UINT nRadix,
            /* [out] */ FRAMEINFO *pFrameInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetPhysicalStackRange )( 
            IDebugStackFrame2 * This,
            /* [out] */ UINT64 *paddrMin,
            /* [out] */ UINT64 *paddrMax);
        
        HRESULT ( STDMETHODCALLTYPE *GetExpressionContext )( 
            IDebugStackFrame2 * This,
            /* [out] */ IDebugExpressionContext2 **ppExprCxt);
        
        HRESULT ( STDMETHODCALLTYPE *GetLanguageInfo )( 
            IDebugStackFrame2 * This,
            /* [full][out][in] */ BSTR *pbstrLanguage,
            /* [full][out][in] */ GUID *pguidLanguage);
        
        HRESULT ( STDMETHODCALLTYPE *GetDebugProperty )( 
            IDebugStackFrame2 * This,
            /* [out] */ IDebugProperty2 **ppProperty);
        
        HRESULT ( STDMETHODCALLTYPE *EnumProperties )( 
            IDebugStackFrame2 * This,
            /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
            /* [in] */ UINT nRadix,
            /* [in] */ REFGUID guidFilter,
            /* [in] */ DWORD dwTimeout,
            /* [out] */ ULONG *pcelt,
            /* [out] */ IEnumDebugPropertyInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetThread )( 
            IDebugStackFrame2 * This,
            /* [out] */ IDebugThread2 **ppThread);
        
        END_INTERFACE
    } IDebugStackFrame2Vtbl;

    interface IDebugStackFrame2
    {
        CONST_VTBL struct IDebugStackFrame2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugStackFrame2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugStackFrame2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugStackFrame2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugStackFrame2_GetCodeContext(This,ppCodeCxt)	\
    (This)->lpVtbl -> GetCodeContext(This,ppCodeCxt)

#define IDebugStackFrame2_GetDocumentContext(This,ppCxt)	\
    (This)->lpVtbl -> GetDocumentContext(This,ppCxt)

#define IDebugStackFrame2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugStackFrame2_GetInfo(This,dwFieldSpec,nRadix,pFrameInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFieldSpec,nRadix,pFrameInfo)

#define IDebugStackFrame2_GetPhysicalStackRange(This,paddrMin,paddrMax)	\
    (This)->lpVtbl -> GetPhysicalStackRange(This,paddrMin,paddrMax)

#define IDebugStackFrame2_GetExpressionContext(This,ppExprCxt)	\
    (This)->lpVtbl -> GetExpressionContext(This,ppExprCxt)

#define IDebugStackFrame2_GetLanguageInfo(This,pbstrLanguage,pguidLanguage)	\
    (This)->lpVtbl -> GetLanguageInfo(This,pbstrLanguage,pguidLanguage)

#define IDebugStackFrame2_GetDebugProperty(This,ppProperty)	\
    (This)->lpVtbl -> GetDebugProperty(This,ppProperty)

#define IDebugStackFrame2_EnumProperties(This,dwFields,nRadix,guidFilter,dwTimeout,pcelt,ppEnum)	\
    (This)->lpVtbl -> EnumProperties(This,dwFields,nRadix,guidFilter,dwTimeout,pcelt,ppEnum)

#define IDebugStackFrame2_GetThread(This,ppThread)	\
    (This)->lpVtbl -> GetThread(This,ppThread)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetCodeContext_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ IDebugCodeContext2 **ppCodeCxt);


void __RPC_STUB IDebugStackFrame2_GetCodeContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetDocumentContext_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ IDebugDocumentContext2 **ppCxt);


void __RPC_STUB IDebugStackFrame2_GetDocumentContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetName_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugStackFrame2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetInfo_Proxy( 
    IDebugStackFrame2 * This,
    /* [in] */ FRAMEINFO_FLAGS dwFieldSpec,
    /* [in] */ UINT nRadix,
    /* [out] */ FRAMEINFO *pFrameInfo);


void __RPC_STUB IDebugStackFrame2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetPhysicalStackRange_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ UINT64 *paddrMin,
    /* [out] */ UINT64 *paddrMax);


void __RPC_STUB IDebugStackFrame2_GetPhysicalStackRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetExpressionContext_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ IDebugExpressionContext2 **ppExprCxt);


void __RPC_STUB IDebugStackFrame2_GetExpressionContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetLanguageInfo_Proxy( 
    IDebugStackFrame2 * This,
    /* [full][out][in] */ BSTR *pbstrLanguage,
    /* [full][out][in] */ GUID *pguidLanguage);


void __RPC_STUB IDebugStackFrame2_GetLanguageInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetDebugProperty_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ IDebugProperty2 **ppProperty);


void __RPC_STUB IDebugStackFrame2_GetDebugProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_EnumProperties_Proxy( 
    IDebugStackFrame2 * This,
    /* [in] */ DEBUGPROP_INFO_FLAGS dwFields,
    /* [in] */ UINT nRadix,
    /* [in] */ REFGUID guidFilter,
    /* [in] */ DWORD dwTimeout,
    /* [out] */ ULONG *pcelt,
    /* [out] */ IEnumDebugPropertyInfo2 **ppEnum);


void __RPC_STUB IDebugStackFrame2_EnumProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugStackFrame2_GetThread_Proxy( 
    IDebugStackFrame2 * This,
    /* [out] */ IDebugThread2 **ppThread);


void __RPC_STUB IDebugStackFrame2_GetThread_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugStackFrame2_INTERFACE_DEFINED__ */


#ifndef __IDebugMemoryContext2_INTERFACE_DEFINED__
#define __IDebugMemoryContext2_INTERFACE_DEFINED__

/* interface IDebugMemoryContext2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugMemoryContext2_0001
    {	CONTEXT_EQUAL	= 0x1,
	CONTEXT_LESS_THAN	= 0x2,
	CONTEXT_GREATER_THAN	= 0x3,
	CONTEXT_LESS_THAN_OR_EQUAL	= 0x4,
	CONTEXT_GREATER_THAN_OR_EQUAL	= 0x5,
	CONTEXT_SAME_SCOPE	= 0x6,
	CONTEXT_SAME_FUNCTION	= 0x7,
	CONTEXT_SAME_MODULE	= 0x8,
	CONTEXT_SAME_PROCESS	= 0x9
    } ;
typedef DWORD CONTEXT_COMPARE;


enum __MIDL_IDebugMemoryContext2_0002
    {	CIF_MODULEURL	= 0x1,
	CIF_FUNCTION	= 0x2,
	CIF_FUNCTIONOFFSET	= 0x4,
	CIF_ADDRESS	= 0x8,
	CIF_ADDRESSOFFSET	= 0x10,
	CIF_ADDRESSABSOLUTE	= 0x20,
	CIF_ALLFIELDS	= 0x3f
    } ;
typedef DWORD CONTEXT_INFO_FIELDS;

typedef struct _tagCONTEXT_INFO
    {
    CONTEXT_INFO_FIELDS dwFields;
    BSTR bstrModuleUrl;
    BSTR bstrFunction;
    TEXT_POSITION posFunctionOffset;
    BSTR bstrAddress;
    BSTR bstrAddressOffset;
    BSTR bstrAddressAbsolute;
    } 	CONTEXT_INFO;


EXTERN_C const IID IID_IDebugMemoryContext2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1ab276dd-f27b-4445-825d-5df0b4a04a3a")
    IDebugMemoryContext2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [in] */ CONTEXT_INFO_FIELDS dwFields,
            /* [out] */ CONTEXT_INFO *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ UINT64 dwCount,
            /* [out] */ IDebugMemoryContext2 **ppMemCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Subtract( 
            /* [in] */ UINT64 dwCount,
            /* [out] */ IDebugMemoryContext2 **ppMemCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Compare( 
            /* [in] */ CONTEXT_COMPARE compare,
            /* [length_is][size_is][in] */ IDebugMemoryContext2 **rgpMemoryContextSet,
            /* [in] */ DWORD dwMemoryContextSetLen,
            /* [out] */ DWORD *pdwMemoryContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMemoryContext2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMemoryContext2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMemoryContext2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMemoryContext2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugMemoryContext2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugMemoryContext2 * This,
            /* [in] */ CONTEXT_INFO_FIELDS dwFields,
            /* [out] */ CONTEXT_INFO *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *Add )( 
            IDebugMemoryContext2 * This,
            /* [in] */ UINT64 dwCount,
            /* [out] */ IDebugMemoryContext2 **ppMemCxt);
        
        HRESULT ( STDMETHODCALLTYPE *Subtract )( 
            IDebugMemoryContext2 * This,
            /* [in] */ UINT64 dwCount,
            /* [out] */ IDebugMemoryContext2 **ppMemCxt);
        
        HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IDebugMemoryContext2 * This,
            /* [in] */ CONTEXT_COMPARE compare,
            /* [length_is][size_is][in] */ IDebugMemoryContext2 **rgpMemoryContextSet,
            /* [in] */ DWORD dwMemoryContextSetLen,
            /* [out] */ DWORD *pdwMemoryContext);
        
        END_INTERFACE
    } IDebugMemoryContext2Vtbl;

    interface IDebugMemoryContext2
    {
        CONST_VTBL struct IDebugMemoryContext2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMemoryContext2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMemoryContext2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMemoryContext2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMemoryContext2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugMemoryContext2_GetInfo(This,dwFields,pInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pInfo)

#define IDebugMemoryContext2_Add(This,dwCount,ppMemCxt)	\
    (This)->lpVtbl -> Add(This,dwCount,ppMemCxt)

#define IDebugMemoryContext2_Subtract(This,dwCount,ppMemCxt)	\
    (This)->lpVtbl -> Subtract(This,dwCount,ppMemCxt)

#define IDebugMemoryContext2_Compare(This,compare,rgpMemoryContextSet,dwMemoryContextSetLen,pdwMemoryContext)	\
    (This)->lpVtbl -> Compare(This,compare,rgpMemoryContextSet,dwMemoryContextSetLen,pdwMemoryContext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMemoryContext2_GetName_Proxy( 
    IDebugMemoryContext2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugMemoryContext2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMemoryContext2_GetInfo_Proxy( 
    IDebugMemoryContext2 * This,
    /* [in] */ CONTEXT_INFO_FIELDS dwFields,
    /* [out] */ CONTEXT_INFO *pInfo);


void __RPC_STUB IDebugMemoryContext2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMemoryContext2_Add_Proxy( 
    IDebugMemoryContext2 * This,
    /* [in] */ UINT64 dwCount,
    /* [out] */ IDebugMemoryContext2 **ppMemCxt);


void __RPC_STUB IDebugMemoryContext2_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMemoryContext2_Subtract_Proxy( 
    IDebugMemoryContext2 * This,
    /* [in] */ UINT64 dwCount,
    /* [out] */ IDebugMemoryContext2 **ppMemCxt);


void __RPC_STUB IDebugMemoryContext2_Subtract_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMemoryContext2_Compare_Proxy( 
    IDebugMemoryContext2 * This,
    /* [in] */ CONTEXT_COMPARE compare,
    /* [length_is][size_is][in] */ IDebugMemoryContext2 **rgpMemoryContextSet,
    /* [in] */ DWORD dwMemoryContextSetLen,
    /* [out] */ DWORD *pdwMemoryContext);


void __RPC_STUB IDebugMemoryContext2_Compare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMemoryContext2_INTERFACE_DEFINED__ */


#ifndef __IDebugCodeContext2_INTERFACE_DEFINED__
#define __IDebugCodeContext2_INTERFACE_DEFINED__

/* interface IDebugCodeContext2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugCodeContext2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ac17b76b-2b09-419a-ad5f-7d7402da8875")
    IDebugCodeContext2 : public IDebugMemoryContext2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDocumentContext( 
            /* [out] */ IDebugDocumentContext2 **ppSrcCxt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLanguageInfo( 
            /* [full][out][in] */ BSTR *pbstrLanguage,
            /* [full][out][in] */ GUID *pguidLanguage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugCodeContext2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugCodeContext2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugCodeContext2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugCodeContext2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugCodeContext2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugCodeContext2 * This,
            /* [in] */ CONTEXT_INFO_FIELDS dwFields,
            /* [out] */ CONTEXT_INFO *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *Add )( 
            IDebugCodeContext2 * This,
            /* [in] */ UINT64 dwCount,
            /* [out] */ IDebugMemoryContext2 **ppMemCxt);
        
        HRESULT ( STDMETHODCALLTYPE *Subtract )( 
            IDebugCodeContext2 * This,
            /* [in] */ UINT64 dwCount,
            /* [out] */ IDebugMemoryContext2 **ppMemCxt);
        
        HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IDebugCodeContext2 * This,
            /* [in] */ CONTEXT_COMPARE compare,
            /* [length_is][size_is][in] */ IDebugMemoryContext2 **rgpMemoryContextSet,
            /* [in] */ DWORD dwMemoryContextSetLen,
            /* [out] */ DWORD *pdwMemoryContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentContext )( 
            IDebugCodeContext2 * This,
            /* [out] */ IDebugDocumentContext2 **ppSrcCxt);
        
        HRESULT ( STDMETHODCALLTYPE *GetLanguageInfo )( 
            IDebugCodeContext2 * This,
            /* [full][out][in] */ BSTR *pbstrLanguage,
            /* [full][out][in] */ GUID *pguidLanguage);
        
        END_INTERFACE
    } IDebugCodeContext2Vtbl;

    interface IDebugCodeContext2
    {
        CONST_VTBL struct IDebugCodeContext2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugCodeContext2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugCodeContext2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugCodeContext2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugCodeContext2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugCodeContext2_GetInfo(This,dwFields,pInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pInfo)

#define IDebugCodeContext2_Add(This,dwCount,ppMemCxt)	\
    (This)->lpVtbl -> Add(This,dwCount,ppMemCxt)

#define IDebugCodeContext2_Subtract(This,dwCount,ppMemCxt)	\
    (This)->lpVtbl -> Subtract(This,dwCount,ppMemCxt)

#define IDebugCodeContext2_Compare(This,compare,rgpMemoryContextSet,dwMemoryContextSetLen,pdwMemoryContext)	\
    (This)->lpVtbl -> Compare(This,compare,rgpMemoryContextSet,dwMemoryContextSetLen,pdwMemoryContext)


#define IDebugCodeContext2_GetDocumentContext(This,ppSrcCxt)	\
    (This)->lpVtbl -> GetDocumentContext(This,ppSrcCxt)

#define IDebugCodeContext2_GetLanguageInfo(This,pbstrLanguage,pguidLanguage)	\
    (This)->lpVtbl -> GetLanguageInfo(This,pbstrLanguage,pguidLanguage)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugCodeContext2_GetDocumentContext_Proxy( 
    IDebugCodeContext2 * This,
    /* [out] */ IDebugDocumentContext2 **ppSrcCxt);


void __RPC_STUB IDebugCodeContext2_GetDocumentContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugCodeContext2_GetLanguageInfo_Proxy( 
    IDebugCodeContext2 * This,
    /* [full][out][in] */ BSTR *pbstrLanguage,
    /* [full][out][in] */ GUID *pguidLanguage);


void __RPC_STUB IDebugCodeContext2_GetLanguageInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugCodeContext2_INTERFACE_DEFINED__ */


#ifndef __IDebugMemoryBytes2_INTERFACE_DEFINED__
#define __IDebugMemoryBytes2_INTERFACE_DEFINED__

/* interface IDebugMemoryBytes2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugMemoryBytes2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("925837d1-3aa1-451a-b7fe-cc04bb42cfb8")
    IDebugMemoryBytes2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReadAt( 
            /* [in] */ IDebugMemoryContext2 *pStartContext,
            /* [in] */ DWORD dwCount,
            /* [length_is][size_is][out] */ BYTE *rgbMemory,
            /* [out] */ DWORD *pdwRead,
            /* [full][out][in] */ DWORD *pdwUnreadable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WriteAt( 
            /* [in] */ IDebugMemoryContext2 *pStartContext,
            /* [in] */ DWORD dwCount,
            /* [length_is][size_is][in] */ BYTE *rgbMemory) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ UINT64 *pqwSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugMemoryBytes2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugMemoryBytes2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugMemoryBytes2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugMemoryBytes2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReadAt )( 
            IDebugMemoryBytes2 * This,
            /* [in] */ IDebugMemoryContext2 *pStartContext,
            /* [in] */ DWORD dwCount,
            /* [length_is][size_is][out] */ BYTE *rgbMemory,
            /* [out] */ DWORD *pdwRead,
            /* [full][out][in] */ DWORD *pdwUnreadable);
        
        HRESULT ( STDMETHODCALLTYPE *WriteAt )( 
            IDebugMemoryBytes2 * This,
            /* [in] */ IDebugMemoryContext2 *pStartContext,
            /* [in] */ DWORD dwCount,
            /* [length_is][size_is][in] */ BYTE *rgbMemory);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugMemoryBytes2 * This,
            /* [out] */ UINT64 *pqwSize);
        
        END_INTERFACE
    } IDebugMemoryBytes2Vtbl;

    interface IDebugMemoryBytes2
    {
        CONST_VTBL struct IDebugMemoryBytes2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugMemoryBytes2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugMemoryBytes2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugMemoryBytes2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugMemoryBytes2_ReadAt(This,pStartContext,dwCount,rgbMemory,pdwRead,pdwUnreadable)	\
    (This)->lpVtbl -> ReadAt(This,pStartContext,dwCount,rgbMemory,pdwRead,pdwUnreadable)

#define IDebugMemoryBytes2_WriteAt(This,pStartContext,dwCount,rgbMemory)	\
    (This)->lpVtbl -> WriteAt(This,pStartContext,dwCount,rgbMemory)

#define IDebugMemoryBytes2_GetSize(This,pqwSize)	\
    (This)->lpVtbl -> GetSize(This,pqwSize)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugMemoryBytes2_ReadAt_Proxy( 
    IDebugMemoryBytes2 * This,
    /* [in] */ IDebugMemoryContext2 *pStartContext,
    /* [in] */ DWORD dwCount,
    /* [length_is][size_is][out] */ BYTE *rgbMemory,
    /* [out] */ DWORD *pdwRead,
    /* [full][out][in] */ DWORD *pdwUnreadable);


void __RPC_STUB IDebugMemoryBytes2_ReadAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMemoryBytes2_WriteAt_Proxy( 
    IDebugMemoryBytes2 * This,
    /* [in] */ IDebugMemoryContext2 *pStartContext,
    /* [in] */ DWORD dwCount,
    /* [length_is][size_is][in] */ BYTE *rgbMemory);


void __RPC_STUB IDebugMemoryBytes2_WriteAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugMemoryBytes2_GetSize_Proxy( 
    IDebugMemoryBytes2 * This,
    /* [out] */ UINT64 *pqwSize);


void __RPC_STUB IDebugMemoryBytes2_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugMemoryBytes2_INTERFACE_DEFINED__ */


#ifndef __IDebugDisassemblyStream2_INTERFACE_DEFINED__
#define __IDebugDisassemblyStream2_INTERFACE_DEFINED__

/* interface IDebugDisassemblyStream2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugDisassemblyStream2_0001
    {	DSF_ADDRESS	= 0x1,
	DSF_ADDRESSOFFSET	= 0x2,
	DSF_CODEBYTES	= 0x4,
	DSF_OPCODE	= 0x8,
	DSF_OPERANDS	= 0x10,
	DSF_SYMBOL	= 0x20,
	DSF_CODELOCATIONID	= 0x40,
	DSF_POSITION	= 0x80,
	DSF_DOCUMENTURL	= 0x100,
	DSF_BYTEOFFSET	= 0x200,
	DSF_FLAGS	= 0x400,
	DSF_OPERANDS_SYMBOLS	= 0x10000,
	DSF_ALL	= 0x107ff
    } ;
typedef DWORD DISASSEMBLY_STREAM_FIELDS;


enum __MIDL_IDebugDisassemblyStream2_0002
    {	DF_DOCUMENTCHANGE	= 0x1,
	DF_DISABLED	= 0x2,
	DF_INSTRUCTION_ACTIVE	= 0x4,
	DF_DATA	= 0x8,
	DF_HASSOURCE	= 0x10
    } ;
typedef DWORD DISASSEMBLY_FLAGS;

typedef struct tagDisassemblyData
    {
    DISASSEMBLY_STREAM_FIELDS dwFields;
    BSTR bstrAddress;
    BSTR bstrAddressOffset;
    BSTR bstrCodeBytes;
    BSTR bstrOpcode;
    BSTR bstrOperands;
    BSTR bstrSymbol;
    UINT64 uCodeLocationId;
    TEXT_POSITION posBeg;
    TEXT_POSITION posEnd;
    BSTR bstrDocumentUrl;
    DWORD dwByteOffset;
    DISASSEMBLY_FLAGS dwFlags;
    } 	DisassemblyData;


enum __MIDL_IDebugDisassemblyStream2_0003
    {	SEEK_START_BEGIN	= 0x1,
	SEEK_START_END	= 0x2,
	SEEK_START_CURRENT	= 0x3,
	SEEK_START_CODECONTEXT	= 0x4,
	SEEK_START_CODELOCID	= 0x5
    } ;
typedef DWORD SEEK_START;


EXTERN_C const IID IID_IDebugDisassemblyStream2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e5b017fe-dfb0-411c-8266-7c64d6f519f8")
    IDebugDisassemblyStream2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ DWORD dwInstructions,
            /* [in] */ DISASSEMBLY_STREAM_FIELDS dwFields,
            /* [out] */ DWORD *pdwInstructionsRead,
            /* [length_is][size_is][out] */ DisassemblyData *prgDisassembly) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ SEEK_START dwSeekStart,
            /* [in] */ IDebugCodeContext2 *pCodeContext,
            /* [in] */ UINT64 uCodeLocationId,
            /* [in] */ INT64 iInstructions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCodeLocationId( 
            /* [in] */ IDebugCodeContext2 *pCodeContext,
            /* [out] */ UINT64 *puCodeLocationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCodeContext( 
            /* [in] */ UINT64 uCodeLocationId,
            /* [out] */ IDebugCodeContext2 **ppCodeContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentLocation( 
            /* [out] */ UINT64 *puCodeLocationId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocument( 
            /* [in] */ BSTR bstrDocumentUrl,
            /* [out] */ IDebugDocument2 **ppDocument) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetScope( 
            /* [out] */ DISASSEMBLY_STREAM_SCOPE *pdwScope) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ UINT64 *pnSize) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDisassemblyStream2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDisassemblyStream2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDisassemblyStream2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDisassemblyStream2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Read )( 
            IDebugDisassemblyStream2 * This,
            /* [in] */ DWORD dwInstructions,
            /* [in] */ DISASSEMBLY_STREAM_FIELDS dwFields,
            /* [out] */ DWORD *pdwInstructionsRead,
            /* [length_is][size_is][out] */ DisassemblyData *prgDisassembly);
        
        HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IDebugDisassemblyStream2 * This,
            /* [in] */ SEEK_START dwSeekStart,
            /* [in] */ IDebugCodeContext2 *pCodeContext,
            /* [in] */ UINT64 uCodeLocationId,
            /* [in] */ INT64 iInstructions);
        
        HRESULT ( STDMETHODCALLTYPE *GetCodeLocationId )( 
            IDebugDisassemblyStream2 * This,
            /* [in] */ IDebugCodeContext2 *pCodeContext,
            /* [out] */ UINT64 *puCodeLocationId);
        
        HRESULT ( STDMETHODCALLTYPE *GetCodeContext )( 
            IDebugDisassemblyStream2 * This,
            /* [in] */ UINT64 uCodeLocationId,
            /* [out] */ IDebugCodeContext2 **ppCodeContext);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentLocation )( 
            IDebugDisassemblyStream2 * This,
            /* [out] */ UINT64 *puCodeLocationId);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocument )( 
            IDebugDisassemblyStream2 * This,
            /* [in] */ BSTR bstrDocumentUrl,
            /* [out] */ IDebugDocument2 **ppDocument);
        
        HRESULT ( STDMETHODCALLTYPE *GetScope )( 
            IDebugDisassemblyStream2 * This,
            /* [out] */ DISASSEMBLY_STREAM_SCOPE *pdwScope);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugDisassemblyStream2 * This,
            /* [out] */ UINT64 *pnSize);
        
        END_INTERFACE
    } IDebugDisassemblyStream2Vtbl;

    interface IDebugDisassemblyStream2
    {
        CONST_VTBL struct IDebugDisassemblyStream2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDisassemblyStream2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDisassemblyStream2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDisassemblyStream2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDisassemblyStream2_Read(This,dwInstructions,dwFields,pdwInstructionsRead,prgDisassembly)	\
    (This)->lpVtbl -> Read(This,dwInstructions,dwFields,pdwInstructionsRead,prgDisassembly)

#define IDebugDisassemblyStream2_Seek(This,dwSeekStart,pCodeContext,uCodeLocationId,iInstructions)	\
    (This)->lpVtbl -> Seek(This,dwSeekStart,pCodeContext,uCodeLocationId,iInstructions)

#define IDebugDisassemblyStream2_GetCodeLocationId(This,pCodeContext,puCodeLocationId)	\
    (This)->lpVtbl -> GetCodeLocationId(This,pCodeContext,puCodeLocationId)

#define IDebugDisassemblyStream2_GetCodeContext(This,uCodeLocationId,ppCodeContext)	\
    (This)->lpVtbl -> GetCodeContext(This,uCodeLocationId,ppCodeContext)

#define IDebugDisassemblyStream2_GetCurrentLocation(This,puCodeLocationId)	\
    (This)->lpVtbl -> GetCurrentLocation(This,puCodeLocationId)

#define IDebugDisassemblyStream2_GetDocument(This,bstrDocumentUrl,ppDocument)	\
    (This)->lpVtbl -> GetDocument(This,bstrDocumentUrl,ppDocument)

#define IDebugDisassemblyStream2_GetScope(This,pdwScope)	\
    (This)->lpVtbl -> GetScope(This,pdwScope)

#define IDebugDisassemblyStream2_GetSize(This,pnSize)	\
    (This)->lpVtbl -> GetSize(This,pnSize)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_Read_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [in] */ DWORD dwInstructions,
    /* [in] */ DISASSEMBLY_STREAM_FIELDS dwFields,
    /* [out] */ DWORD *pdwInstructionsRead,
    /* [length_is][size_is][out] */ DisassemblyData *prgDisassembly);


void __RPC_STUB IDebugDisassemblyStream2_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_Seek_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [in] */ SEEK_START dwSeekStart,
    /* [in] */ IDebugCodeContext2 *pCodeContext,
    /* [in] */ UINT64 uCodeLocationId,
    /* [in] */ INT64 iInstructions);


void __RPC_STUB IDebugDisassemblyStream2_Seek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_GetCodeLocationId_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [in] */ IDebugCodeContext2 *pCodeContext,
    /* [out] */ UINT64 *puCodeLocationId);


void __RPC_STUB IDebugDisassemblyStream2_GetCodeLocationId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_GetCodeContext_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [in] */ UINT64 uCodeLocationId,
    /* [out] */ IDebugCodeContext2 **ppCodeContext);


void __RPC_STUB IDebugDisassemblyStream2_GetCodeContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_GetCurrentLocation_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [out] */ UINT64 *puCodeLocationId);


void __RPC_STUB IDebugDisassemblyStream2_GetCurrentLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_GetDocument_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [in] */ BSTR bstrDocumentUrl,
    /* [out] */ IDebugDocument2 **ppDocument);


void __RPC_STUB IDebugDisassemblyStream2_GetDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_GetScope_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [out] */ DISASSEMBLY_STREAM_SCOPE *pdwScope);


void __RPC_STUB IDebugDisassemblyStream2_GetScope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDisassemblyStream2_GetSize_Proxy( 
    IDebugDisassemblyStream2 * This,
    /* [out] */ UINT64 *pnSize);


void __RPC_STUB IDebugDisassemblyStream2_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDisassemblyStream2_INTERFACE_DEFINED__ */


#ifndef __IDebugDocumentContext2_INTERFACE_DEFINED__
#define __IDebugDocumentContext2_INTERFACE_DEFINED__

/* interface IDebugDocumentContext2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugDocumentContext2_0001
    {	DOCCONTEXT_EQUAL	= 0x1,
	DOCCONTEXT_LESS_THAN	= 0x2,
	DOCCONTEXT_GREATER_THAN	= 0x3,
	DOCCONTEXT_SAME_DOCUMENT	= 0x4
    } ;
typedef DWORD DOCCONTEXT_COMPARE;


EXTERN_C const IID IID_IDebugDocumentContext2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("931516ad-b600-419c-88fc-dcf5183b5fa9")
    IDebugDocumentContext2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDocument( 
            /* [out] */ IDebugDocument2 **ppDocument) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCodeContexts( 
            /* [out] */ IEnumDebugCodeContexts2 **ppEnumCodeCxts) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLanguageInfo( 
            /* [full][out][in] */ BSTR *pbstrLanguage,
            /* [full][out][in] */ GUID *pguidLanguage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetStatementRange( 
            /* [full][out][in] */ TEXT_POSITION *pBegPosition,
            /* [full][out][in] */ TEXT_POSITION *pEndPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceRange( 
            /* [full][out][in] */ TEXT_POSITION *pBegPosition,
            /* [full][out][in] */ TEXT_POSITION *pEndPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Compare( 
            /* [in] */ DOCCONTEXT_COMPARE compare,
            /* [length_is][size_is][in] */ IDebugDocumentContext2 **rgpDocContextSet,
            /* [in] */ DWORD dwDocContextSetLen,
            /* [out] */ DWORD *pdwDocContext) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ int nCount,
            /* [out] */ IDebugDocumentContext2 **ppDocContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDocumentContext2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDocumentContext2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDocumentContext2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDocumentContext2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocument )( 
            IDebugDocumentContext2 * This,
            /* [out] */ IDebugDocument2 **ppDocument);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugDocumentContext2 * This,
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrFileName);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCodeContexts )( 
            IDebugDocumentContext2 * This,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnumCodeCxts);
        
        HRESULT ( STDMETHODCALLTYPE *GetLanguageInfo )( 
            IDebugDocumentContext2 * This,
            /* [full][out][in] */ BSTR *pbstrLanguage,
            /* [full][out][in] */ GUID *pguidLanguage);
        
        HRESULT ( STDMETHODCALLTYPE *GetStatementRange )( 
            IDebugDocumentContext2 * This,
            /* [full][out][in] */ TEXT_POSITION *pBegPosition,
            /* [full][out][in] */ TEXT_POSITION *pEndPosition);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceRange )( 
            IDebugDocumentContext2 * This,
            /* [full][out][in] */ TEXT_POSITION *pBegPosition,
            /* [full][out][in] */ TEXT_POSITION *pEndPosition);
        
        HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IDebugDocumentContext2 * This,
            /* [in] */ DOCCONTEXT_COMPARE compare,
            /* [length_is][size_is][in] */ IDebugDocumentContext2 **rgpDocContextSet,
            /* [in] */ DWORD dwDocContextSetLen,
            /* [out] */ DWORD *pdwDocContext);
        
        HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IDebugDocumentContext2 * This,
            /* [in] */ int nCount,
            /* [out] */ IDebugDocumentContext2 **ppDocContext);
        
        END_INTERFACE
    } IDebugDocumentContext2Vtbl;

    interface IDebugDocumentContext2
    {
        CONST_VTBL struct IDebugDocumentContext2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDocumentContext2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDocumentContext2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDocumentContext2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDocumentContext2_GetDocument(This,ppDocument)	\
    (This)->lpVtbl -> GetDocument(This,ppDocument)

#define IDebugDocumentContext2_GetName(This,gnType,pbstrFileName)	\
    (This)->lpVtbl -> GetName(This,gnType,pbstrFileName)

#define IDebugDocumentContext2_EnumCodeContexts(This,ppEnumCodeCxts)	\
    (This)->lpVtbl -> EnumCodeContexts(This,ppEnumCodeCxts)

#define IDebugDocumentContext2_GetLanguageInfo(This,pbstrLanguage,pguidLanguage)	\
    (This)->lpVtbl -> GetLanguageInfo(This,pbstrLanguage,pguidLanguage)

#define IDebugDocumentContext2_GetStatementRange(This,pBegPosition,pEndPosition)	\
    (This)->lpVtbl -> GetStatementRange(This,pBegPosition,pEndPosition)

#define IDebugDocumentContext2_GetSourceRange(This,pBegPosition,pEndPosition)	\
    (This)->lpVtbl -> GetSourceRange(This,pBegPosition,pEndPosition)

#define IDebugDocumentContext2_Compare(This,compare,rgpDocContextSet,dwDocContextSetLen,pdwDocContext)	\
    (This)->lpVtbl -> Compare(This,compare,rgpDocContextSet,dwDocContextSetLen,pdwDocContext)

#define IDebugDocumentContext2_Seek(This,nCount,ppDocContext)	\
    (This)->lpVtbl -> Seek(This,nCount,ppDocContext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_GetDocument_Proxy( 
    IDebugDocumentContext2 * This,
    /* [out] */ IDebugDocument2 **ppDocument);


void __RPC_STUB IDebugDocumentContext2_GetDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_GetName_Proxy( 
    IDebugDocumentContext2 * This,
    /* [in] */ GETNAME_TYPE gnType,
    /* [out] */ BSTR *pbstrFileName);


void __RPC_STUB IDebugDocumentContext2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_EnumCodeContexts_Proxy( 
    IDebugDocumentContext2 * This,
    /* [out] */ IEnumDebugCodeContexts2 **ppEnumCodeCxts);


void __RPC_STUB IDebugDocumentContext2_EnumCodeContexts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_GetLanguageInfo_Proxy( 
    IDebugDocumentContext2 * This,
    /* [full][out][in] */ BSTR *pbstrLanguage,
    /* [full][out][in] */ GUID *pguidLanguage);


void __RPC_STUB IDebugDocumentContext2_GetLanguageInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_GetStatementRange_Proxy( 
    IDebugDocumentContext2 * This,
    /* [full][out][in] */ TEXT_POSITION *pBegPosition,
    /* [full][out][in] */ TEXT_POSITION *pEndPosition);


void __RPC_STUB IDebugDocumentContext2_GetStatementRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_GetSourceRange_Proxy( 
    IDebugDocumentContext2 * This,
    /* [full][out][in] */ TEXT_POSITION *pBegPosition,
    /* [full][out][in] */ TEXT_POSITION *pEndPosition);


void __RPC_STUB IDebugDocumentContext2_GetSourceRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_Compare_Proxy( 
    IDebugDocumentContext2 * This,
    /* [in] */ DOCCONTEXT_COMPARE compare,
    /* [length_is][size_is][in] */ IDebugDocumentContext2 **rgpDocContextSet,
    /* [in] */ DWORD dwDocContextSetLen,
    /* [out] */ DWORD *pdwDocContext);


void __RPC_STUB IDebugDocumentContext2_Compare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentContext2_Seek_Proxy( 
    IDebugDocumentContext2 * This,
    /* [in] */ int nCount,
    /* [out] */ IDebugDocumentContext2 **ppDocContext);


void __RPC_STUB IDebugDocumentContext2_Seek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDocumentContext2_INTERFACE_DEFINED__ */


#ifndef __IDebugExpressionContext2_INTERFACE_DEFINED__
#define __IDebugExpressionContext2_INTERFACE_DEFINED__

/* interface IDebugExpressionContext2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugExpressionContext2_0001
    {	PARSE_EXPRESSION	= 0x1,
	PARSE_FUNCTION_AS_ADDRESS	= 0x2
    } ;
typedef DWORD PARSEFLAGS;


EXTERN_C const IID IID_IDebugExpressionContext2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37a44580-d5fc-473e-a048-21702ebfc466")
    IDebugExpressionContext2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ParseText( 
            /* [in] */ LPCOLESTR pszCode,
            /* [in] */ PARSEFLAGS dwFlags,
            /* [in] */ UINT nRadix,
            /* [out] */ IDebugExpression2 **ppExpr,
            /* [out] */ BSTR *pbstrError,
            /* [out] */ UINT *pichError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugExpressionContext2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugExpressionContext2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugExpressionContext2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugExpressionContext2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugExpressionContext2 * This,
            /* [out] */ BSTR *pbstrName);
        
        HRESULT ( STDMETHODCALLTYPE *ParseText )( 
            IDebugExpressionContext2 * This,
            /* [in] */ LPCOLESTR pszCode,
            /* [in] */ PARSEFLAGS dwFlags,
            /* [in] */ UINT nRadix,
            /* [out] */ IDebugExpression2 **ppExpr,
            /* [out] */ BSTR *pbstrError,
            /* [out] */ UINT *pichError);
        
        END_INTERFACE
    } IDebugExpressionContext2Vtbl;

    interface IDebugExpressionContext2
    {
        CONST_VTBL struct IDebugExpressionContext2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugExpressionContext2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugExpressionContext2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugExpressionContext2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugExpressionContext2_GetName(This,pbstrName)	\
    (This)->lpVtbl -> GetName(This,pbstrName)

#define IDebugExpressionContext2_ParseText(This,pszCode,dwFlags,nRadix,ppExpr,pbstrError,pichError)	\
    (This)->lpVtbl -> ParseText(This,pszCode,dwFlags,nRadix,ppExpr,pbstrError,pichError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugExpressionContext2_GetName_Proxy( 
    IDebugExpressionContext2 * This,
    /* [out] */ BSTR *pbstrName);


void __RPC_STUB IDebugExpressionContext2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExpressionContext2_ParseText_Proxy( 
    IDebugExpressionContext2 * This,
    /* [in] */ LPCOLESTR pszCode,
    /* [in] */ PARSEFLAGS dwFlags,
    /* [in] */ UINT nRadix,
    /* [out] */ IDebugExpression2 **ppExpr,
    /* [out] */ BSTR *pbstrError,
    /* [out] */ UINT *pichError);


void __RPC_STUB IDebugExpressionContext2_ParseText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugExpressionContext2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_msdbg_0359 */
/* [local] */ 


enum __MIDL___MIDL_itf_msdbg_0359_0001
    {	BPT_NONE	= 0,
	BPT_CODE	= 0x1,
	BPT_DATA	= 0x2,
	BPT_SPECIAL	= 0x3
    } ;
typedef DWORD BP_TYPE;


enum __MIDL___MIDL_itf_msdbg_0359_0002
    {	BPLT_NONE	= 0,
	BPLT_FILE_LINE	= 0x10000,
	BPLT_FUNC_OFFSET	= 0x20000,
	BPLT_CONTEXT	= 0x30000,
	BPLT_STRING	= 0x40000,
	BPLT_ADDRESS	= 0x50000,
	BPLT_RESOLUTION	= 0x60000,
	BPLT_CODE_FILE_LINE	= BPT_CODE | BPLT_FILE_LINE,
	BPLT_CODE_FUNC_OFFSET	= BPT_CODE | BPLT_FUNC_OFFSET,
	BPLT_CODE_CONTEXT	= BPT_CODE | BPLT_CONTEXT,
	BPLT_CODE_STRING	= BPT_CODE | BPLT_STRING,
	BPLT_CODE_ADDRESS	= BPT_CODE | BPLT_ADDRESS,
	BPLT_DATA_STRING	= BPT_DATA | BPLT_STRING,
	BPLT_TYPE_MASK	= 0xffff,
	BPLT_LOCATION_TYPE_MASK	= 0xffff0000
    } ;
typedef DWORD BP_LOCATION_TYPE;

typedef struct _BP_LOCATION_CODE_FILE_LINE
    {
    BSTR bstrContext;
    IDebugDocumentPosition2 *pDocPos;
    } 	BP_LOCATION_CODE_FILE_LINE;

typedef struct _BP_LOCATION_CODE_FUNC_OFFSET
    {
    BSTR bstrContext;
    IDebugFunctionPosition2 *pFuncPos;
    } 	BP_LOCATION_CODE_FUNC_OFFSET;

typedef struct _BP_LOCATION_CODE_CONTEXT
    {
    IDebugCodeContext2 *pCodeContext;
    } 	BP_LOCATION_CODE_CONTEXT;

typedef struct _BP_LOCATION_CODE_STRING
    {
    BSTR bstrContext;
    BSTR bstrCodeExpr;
    } 	BP_LOCATION_CODE_STRING;

typedef struct _BP_LOCATION_CODE_ADDRESS
    {
    BSTR bstrContext;
    BSTR bstrModuleUrl;
    BSTR bstrFunction;
    BSTR bstrAddress;
    } 	BP_LOCATION_CODE_ADDRESS;

typedef struct _BP_LOCATION_DATA_STRING
    {
    IDebugThread2 *pThread;
    BSTR bstrContext;
    BSTR bstrDataExpr;
    DWORD dwNumElements;
    } 	BP_LOCATION_DATA_STRING;

typedef struct _BP_LOCATION_RESOLUTION
    {
    IDebugBreakpointResolution2 *pResolution;
    } 	BP_LOCATION_RESOLUTION;

typedef struct _BP_LOCATION
    {
    BP_LOCATION_TYPE bpLocationType;
    /* [switch_is] */ /* [switch_type] */ union __MIDL___MIDL_itf_msdbg_0359_0003
        {
        /* [case()] */ BP_LOCATION_CODE_FILE_LINE bplocCodeFileLine;
        /* [case()] */ BP_LOCATION_CODE_FUNC_OFFSET bplocCodeFuncOffset;
        /* [case()] */ BP_LOCATION_CODE_CONTEXT bplocCodeContext;
        /* [case()] */ BP_LOCATION_CODE_STRING bplocCodeString;
        /* [case()] */ BP_LOCATION_CODE_ADDRESS bplocCodeAddress;
        /* [case()] */ BP_LOCATION_DATA_STRING bplocDataString;
        /* [case()] */ BP_LOCATION_RESOLUTION bplocResolution;
        /* [default] */ DWORD unused;
        } 	bpLocation;
    } 	BP_LOCATION;


enum __MIDL___MIDL_itf_msdbg_0359_0004
    {	BP_PASSCOUNT_NONE	= 0,
	BP_PASSCOUNT_EQUAL	= 0x1,
	BP_PASSCOUNT_EQUAL_OR_GREATER	= 0x2,
	BP_PASSCOUNT_MOD	= 0x3
    } ;
typedef DWORD BP_PASSCOUNT_STYLE;

typedef struct _BP_PASSCOUNT
    {
    DWORD dwPassCount;
    BP_PASSCOUNT_STYLE stylePassCount;
    } 	BP_PASSCOUNT;


enum __MIDL___MIDL_itf_msdbg_0359_0005
    {	BP_COND_NONE	= 0,
	BP_COND_WHEN_TRUE	= 0x1,
	BP_COND_WHEN_CHANGED	= 0x2
    } ;
typedef DWORD BP_COND_STYLE;

typedef struct _BP_CONDITION
    {
    IDebugThread2 *pThread;
    BP_COND_STYLE styleCondition;
    BSTR bstrContext;
    BSTR bstrCondition;
    UINT nRadix;
    } 	BP_CONDITION;


enum __MIDL___MIDL_itf_msdbg_0359_0006
    {	BP_FLAG_NONE	= 0,
	BP_FLAG_MAP_DOCPOSITION	= 0x1
    } ;
typedef DWORD BP_FLAGS;


enum __MIDL___MIDL_itf_msdbg_0359_0007
    {	BPREQI_BPLOCATION	= 0x1,
	BPREQI_LANGUAGE	= 0x2,
	BPREQI_PROGRAM	= 0x4,
	BPREQI_PROGRAMNAME	= 0x8,
	BPREQI_THREAD	= 0x10,
	BPREQI_THREADNAME	= 0x20,
	BPREQI_PASSCOUNT	= 0x40,
	BPREQI_CONDITION	= 0x80,
	BPREQI_FLAGS	= 0x100,
	BPREQI_ALLFIELDS	= 0xffffffff
    } ;
typedef DWORD BPREQI_FIELDS;

typedef struct _BP_REQUEST_INFO
    {
    BPREQI_FIELDS dwFields;
    GUID guidLanguage;
    BP_LOCATION bpLocation;
    IDebugProgram2 *pProgram;
    BSTR bstrProgramName;
    IDebugThread2 *pThread;
    BSTR bstrThreadName;
    BP_CONDITION bpCondition;
    BP_PASSCOUNT bpPassCount;
    BP_FLAGS dwFlags;
    } 	BP_REQUEST_INFO;



extern RPC_IF_HANDLE __MIDL_itf_msdbg_0359_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msdbg_0359_v0_0_s_ifspec;

#ifndef __IDebugBreakpointRequest2_INTERFACE_DEFINED__
#define __IDebugBreakpointRequest2_INTERFACE_DEFINED__

/* interface IDebugBreakpointRequest2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBreakpointRequest2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6015fd18-8257-4df3-ac42-f074dedd4cbd")
    IDebugBreakpointRequest2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLocationType( 
            /* [out] */ BP_LOCATION_TYPE *pBPLocationType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRequestInfo( 
            /* [in] */ BPREQI_FIELDS dwFields,
            /* [out] */ BP_REQUEST_INFO *pBPRequestInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakpointRequest2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakpointRequest2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakpointRequest2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakpointRequest2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocationType )( 
            IDebugBreakpointRequest2 * This,
            /* [out] */ BP_LOCATION_TYPE *pBPLocationType);
        
        HRESULT ( STDMETHODCALLTYPE *GetRequestInfo )( 
            IDebugBreakpointRequest2 * This,
            /* [in] */ BPREQI_FIELDS dwFields,
            /* [out] */ BP_REQUEST_INFO *pBPRequestInfo);
        
        END_INTERFACE
    } IDebugBreakpointRequest2Vtbl;

    interface IDebugBreakpointRequest2
    {
        CONST_VTBL struct IDebugBreakpointRequest2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakpointRequest2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakpointRequest2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakpointRequest2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBreakpointRequest2_GetLocationType(This,pBPLocationType)	\
    (This)->lpVtbl -> GetLocationType(This,pBPLocationType)

#define IDebugBreakpointRequest2_GetRequestInfo(This,dwFields,pBPRequestInfo)	\
    (This)->lpVtbl -> GetRequestInfo(This,dwFields,pBPRequestInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBreakpointRequest2_GetLocationType_Proxy( 
    IDebugBreakpointRequest2 * This,
    /* [out] */ BP_LOCATION_TYPE *pBPLocationType);


void __RPC_STUB IDebugBreakpointRequest2_GetLocationType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBreakpointRequest2_GetRequestInfo_Proxy( 
    IDebugBreakpointRequest2 * This,
    /* [in] */ BPREQI_FIELDS dwFields,
    /* [out] */ BP_REQUEST_INFO *pBPRequestInfo);


void __RPC_STUB IDebugBreakpointRequest2_GetRequestInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBreakpointRequest2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_msdbg_0360 */
/* [local] */ 

typedef struct _BP_RESOLUTION_CODE
    {
    IDebugCodeContext2 *pCodeContext;
    } 	BP_RESOLUTION_CODE;


enum __MIDL___MIDL_itf_msdbg_0360_0001
    {	BP_RES_DATA_EMULATED	= 0x1
    } ;
typedef DWORD BP_RES_DATA_FLAGS;

typedef struct _BP_RESOLUTION_DATA
    {
    BSTR bstrDataExpr;
    BSTR bstrFunc;
    BSTR bstrImage;
    BP_RES_DATA_FLAGS dwFlags;
    } 	BP_RESOLUTION_DATA;

typedef struct _BP_RESOLUTION_LOCATION
    {
    BP_TYPE bpType;
    /* [switch_is] */ /* [switch_type] */ union __MIDL___MIDL_itf_msdbg_0360_0002
        {
        /* [case()] */ BP_RESOLUTION_CODE bpresCode;
        /* [case()] */ BP_RESOLUTION_DATA bpresData;
        /* [default] */ int unused;
        } 	bpResLocation;
    } 	BP_RESOLUTION_LOCATION;


enum __MIDL___MIDL_itf_msdbg_0360_0003
    {	BPRESI_BPRESLOCATION	= 0x1,
	BPRESI_PROGRAM	= 0x2,
	BPRESI_THREAD	= 0x4,
	BPRESI_ALLFIELDS	= 0xffffffff
    } ;
typedef DWORD BPRESI_FIELDS;

typedef struct _BP_RESOLUTION_INFO
    {
    BPRESI_FIELDS dwFields;
    BP_RESOLUTION_LOCATION bpResLocation;
    IDebugProgram2 *pProgram;
    IDebugThread2 *pThread;
    } 	BP_RESOLUTION_INFO;



extern RPC_IF_HANDLE __MIDL_itf_msdbg_0360_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msdbg_0360_v0_0_s_ifspec;

#ifndef __IDebugBreakpointResolution2_INTERFACE_DEFINED__
#define __IDebugBreakpointResolution2_INTERFACE_DEFINED__

/* interface IDebugBreakpointResolution2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugBreakpointResolution2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b7e66f28-035a-401a-afc7-2e300bd29711")
    IDebugBreakpointResolution2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBreakpointType( 
            /* [out] */ BP_TYPE *pBPType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResolutionInfo( 
            /* [in] */ BPRESI_FIELDS dwFields,
            /* [out] */ BP_RESOLUTION_INFO *pBPResolutionInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBreakpointResolution2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBreakpointResolution2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBreakpointResolution2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBreakpointResolution2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBreakpointType )( 
            IDebugBreakpointResolution2 * This,
            /* [out] */ BP_TYPE *pBPType);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolutionInfo )( 
            IDebugBreakpointResolution2 * This,
            /* [in] */ BPRESI_FIELDS dwFields,
            /* [out] */ BP_RESOLUTION_INFO *pBPResolutionInfo);
        
        END_INTERFACE
    } IDebugBreakpointResolution2Vtbl;

    interface IDebugBreakpointResolution2
    {
        CONST_VTBL struct IDebugBreakpointResolution2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBreakpointResolution2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBreakpointResolution2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBreakpointResolution2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBreakpointResolution2_GetBreakpointType(This,pBPType)	\
    (This)->lpVtbl -> GetBreakpointType(This,pBPType)

#define IDebugBreakpointResolution2_GetResolutionInfo(This,dwFields,pBPResolutionInfo)	\
    (This)->lpVtbl -> GetResolutionInfo(This,dwFields,pBPResolutionInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBreakpointResolution2_GetBreakpointType_Proxy( 
    IDebugBreakpointResolution2 * This,
    /* [out] */ BP_TYPE *pBPType);


void __RPC_STUB IDebugBreakpointResolution2_GetBreakpointType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBreakpointResolution2_GetResolutionInfo_Proxy( 
    IDebugBreakpointResolution2 * This,
    /* [in] */ BPRESI_FIELDS dwFields,
    /* [out] */ BP_RESOLUTION_INFO *pBPResolutionInfo);


void __RPC_STUB IDebugBreakpointResolution2_GetResolutionInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBreakpointResolution2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_msdbg_0361 */
/* [local] */ 


enum __MIDL___MIDL_itf_msdbg_0361_0001
    {	BPET_NONE	= 0,
	BPET_TYPE_WARNING	= 0x1,
	BPET_TYPE_ERROR	= 0x2,
	BPET_SEV_HIGH	= 0xf000000,
	BPET_SEV_GENERAL	= 0x7000000,
	BPET_SEV_LOW	= 0x1000000,
	BPET_TYPE_MASK	= 0xffff,
	BPET_SEV_MASK	= 0xffff0000,
	BPET_GENERAL_WARNING	= BPET_SEV_GENERAL | BPET_TYPE_WARNING,
	BPET_GENERAL_ERROR	= BPET_SEV_GENERAL | BPET_TYPE_ERROR,
	BPET_ALL	= 0xffffffff
    } ;
typedef DWORD BP_ERROR_TYPE;


enum __MIDL___MIDL_itf_msdbg_0361_0002
    {	BPERESI_BPRESLOCATION	= 0x1,
	BPERESI_PROGRAM	= 0x2,
	BPERESI_THREAD	= 0x4,
	BPERESI_MESSAGE	= 0x8,
	BPERESI_TYPE	= 0x10,
	BPERESI_ALLFIELDS	= 0xffffffff
    } ;
typedef DWORD BPERESI_FIELDS;

typedef struct _BP_ERROR_RESOLUTION_INFO
    {
    BPERESI_FIELDS dwFields;
    BP_RESOLUTION_LOCATION bpResLocation;
    IDebugProgram2 *pProgram;
    IDebugThread2 *pThread;
    BSTR bstrMessage;
    BP_ERROR_TYPE dwType;
    } 	BP_ERROR_RESOLUTION_INFO;



extern RPC_IF_HANDLE __MIDL_itf_msdbg_0361_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_msdbg_0361_v0_0_s_ifspec;

#ifndef __IDebugErrorBreakpointResolution2_INTERFACE_DEFINED__
#define __IDebugErrorBreakpointResolution2_INTERFACE_DEFINED__

/* interface IDebugErrorBreakpointResolution2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugErrorBreakpointResolution2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("603aedf8-9575-4d30-b8ca-124d1c98ebd8")
    IDebugErrorBreakpointResolution2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBreakpointType( 
            /* [out] */ BP_TYPE *pBPType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResolutionInfo( 
            /* [in] */ BPERESI_FIELDS dwFields,
            /* [out] */ BP_ERROR_RESOLUTION_INFO *pErrorResolutionInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugErrorBreakpointResolution2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugErrorBreakpointResolution2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugErrorBreakpointResolution2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugErrorBreakpointResolution2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBreakpointType )( 
            IDebugErrorBreakpointResolution2 * This,
            /* [out] */ BP_TYPE *pBPType);
        
        HRESULT ( STDMETHODCALLTYPE *GetResolutionInfo )( 
            IDebugErrorBreakpointResolution2 * This,
            /* [in] */ BPERESI_FIELDS dwFields,
            /* [out] */ BP_ERROR_RESOLUTION_INFO *pErrorResolutionInfo);
        
        END_INTERFACE
    } IDebugErrorBreakpointResolution2Vtbl;

    interface IDebugErrorBreakpointResolution2
    {
        CONST_VTBL struct IDebugErrorBreakpointResolution2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugErrorBreakpointResolution2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugErrorBreakpointResolution2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugErrorBreakpointResolution2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugErrorBreakpointResolution2_GetBreakpointType(This,pBPType)	\
    (This)->lpVtbl -> GetBreakpointType(This,pBPType)

#define IDebugErrorBreakpointResolution2_GetResolutionInfo(This,dwFields,pErrorResolutionInfo)	\
    (This)->lpVtbl -> GetResolutionInfo(This,dwFields,pErrorResolutionInfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugErrorBreakpointResolution2_GetBreakpointType_Proxy( 
    IDebugErrorBreakpointResolution2 * This,
    /* [out] */ BP_TYPE *pBPType);


void __RPC_STUB IDebugErrorBreakpointResolution2_GetBreakpointType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugErrorBreakpointResolution2_GetResolutionInfo_Proxy( 
    IDebugErrorBreakpointResolution2 * This,
    /* [in] */ BPERESI_FIELDS dwFields,
    /* [out] */ BP_ERROR_RESOLUTION_INFO *pErrorResolutionInfo);


void __RPC_STUB IDebugErrorBreakpointResolution2_GetResolutionInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugErrorBreakpointResolution2_INTERFACE_DEFINED__ */


#ifndef __IDebugBoundBreakpoint2_INTERFACE_DEFINED__
#define __IDebugBoundBreakpoint2_INTERFACE_DEFINED__

/* interface IDebugBoundBreakpoint2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugBoundBreakpoint2_0001
    {	BPS_NONE	= 0,
	BPS_DELETED	= 0x1,
	BPS_DISABLED	= 0x2,
	BPS_ENABLED	= 0x3
    } ;
typedef DWORD BP_STATE;


EXTERN_C const IID IID_IDebugBoundBreakpoint2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d533d975-3f32-4876-abd0-6d37fda563e7")
    IDebugBoundBreakpoint2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPendingBreakpoint( 
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBreakpoint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [out] */ BP_STATE *pState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHitCount( 
            /* [out] */ DWORD *pdwHitCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBreakpointResolution( 
            /* [out] */ IDebugBreakpointResolution2 **ppBPResolution) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enable( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHitCount( 
            /* [in] */ DWORD dwHitCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCondition( 
            /* [in] */ BP_CONDITION bpCondition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPassCount( 
            /* [in] */ BP_PASSCOUNT bpPassCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugBoundBreakpoint2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugBoundBreakpoint2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugBoundBreakpoint2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugBoundBreakpoint2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPendingBreakpoint )( 
            IDebugBoundBreakpoint2 * This,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBreakpoint);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IDebugBoundBreakpoint2 * This,
            /* [out] */ BP_STATE *pState);
        
        HRESULT ( STDMETHODCALLTYPE *GetHitCount )( 
            IDebugBoundBreakpoint2 * This,
            /* [out] */ DWORD *pdwHitCount);
        
        HRESULT ( STDMETHODCALLTYPE *GetBreakpointResolution )( 
            IDebugBoundBreakpoint2 * This,
            /* [out] */ IDebugBreakpointResolution2 **ppBPResolution);
        
        HRESULT ( STDMETHODCALLTYPE *Enable )( 
            IDebugBoundBreakpoint2 * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *SetHitCount )( 
            IDebugBoundBreakpoint2 * This,
            /* [in] */ DWORD dwHitCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetCondition )( 
            IDebugBoundBreakpoint2 * This,
            /* [in] */ BP_CONDITION bpCondition);
        
        HRESULT ( STDMETHODCALLTYPE *SetPassCount )( 
            IDebugBoundBreakpoint2 * This,
            /* [in] */ BP_PASSCOUNT bpPassCount);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IDebugBoundBreakpoint2 * This);
        
        END_INTERFACE
    } IDebugBoundBreakpoint2Vtbl;

    interface IDebugBoundBreakpoint2
    {
        CONST_VTBL struct IDebugBoundBreakpoint2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugBoundBreakpoint2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugBoundBreakpoint2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugBoundBreakpoint2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugBoundBreakpoint2_GetPendingBreakpoint(This,ppPendingBreakpoint)	\
    (This)->lpVtbl -> GetPendingBreakpoint(This,ppPendingBreakpoint)

#define IDebugBoundBreakpoint2_GetState(This,pState)	\
    (This)->lpVtbl -> GetState(This,pState)

#define IDebugBoundBreakpoint2_GetHitCount(This,pdwHitCount)	\
    (This)->lpVtbl -> GetHitCount(This,pdwHitCount)

#define IDebugBoundBreakpoint2_GetBreakpointResolution(This,ppBPResolution)	\
    (This)->lpVtbl -> GetBreakpointResolution(This,ppBPResolution)

#define IDebugBoundBreakpoint2_Enable(This,fEnable)	\
    (This)->lpVtbl -> Enable(This,fEnable)

#define IDebugBoundBreakpoint2_SetHitCount(This,dwHitCount)	\
    (This)->lpVtbl -> SetHitCount(This,dwHitCount)

#define IDebugBoundBreakpoint2_SetCondition(This,bpCondition)	\
    (This)->lpVtbl -> SetCondition(This,bpCondition)

#define IDebugBoundBreakpoint2_SetPassCount(This,bpPassCount)	\
    (This)->lpVtbl -> SetPassCount(This,bpPassCount)

#define IDebugBoundBreakpoint2_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_GetPendingBreakpoint_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [out] */ IDebugPendingBreakpoint2 **ppPendingBreakpoint);


void __RPC_STUB IDebugBoundBreakpoint2_GetPendingBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_GetState_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [out] */ BP_STATE *pState);


void __RPC_STUB IDebugBoundBreakpoint2_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_GetHitCount_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [out] */ DWORD *pdwHitCount);


void __RPC_STUB IDebugBoundBreakpoint2_GetHitCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_GetBreakpointResolution_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [out] */ IDebugBreakpointResolution2 **ppBPResolution);


void __RPC_STUB IDebugBoundBreakpoint2_GetBreakpointResolution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_Enable_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IDebugBoundBreakpoint2_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_SetHitCount_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [in] */ DWORD dwHitCount);


void __RPC_STUB IDebugBoundBreakpoint2_SetHitCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_SetCondition_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [in] */ BP_CONDITION bpCondition);


void __RPC_STUB IDebugBoundBreakpoint2_SetCondition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_SetPassCount_Proxy( 
    IDebugBoundBreakpoint2 * This,
    /* [in] */ BP_PASSCOUNT bpPassCount);


void __RPC_STUB IDebugBoundBreakpoint2_SetPassCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugBoundBreakpoint2_Delete_Proxy( 
    IDebugBoundBreakpoint2 * This);


void __RPC_STUB IDebugBoundBreakpoint2_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugBoundBreakpoint2_INTERFACE_DEFINED__ */


#ifndef __IDebugPendingBreakpoint2_INTERFACE_DEFINED__
#define __IDebugPendingBreakpoint2_INTERFACE_DEFINED__

/* interface IDebugPendingBreakpoint2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugPendingBreakpoint2_0001
    {	PBPS_NONE	= 0,
	PBPS_DELETED	= 0x1,
	PBPS_DISABLED	= 0x2,
	PBPS_ENABLED	= 0x3
    } ;
typedef DWORD PENDING_BP_STATE;


enum __MIDL_IDebugPendingBreakpoint2_0002
    {	PBPSF_NONE	= 0,
	PBPSF_VIRTUALIZED	= 0x1
    } ;
typedef DWORD PENDING_BP_STATE_FLAGS;

typedef struct _tagPENDING_BP_STATE_INFO
    {
    PENDING_BP_STATE state;
    PENDING_BP_STATE_FLAGS flags;
    } 	PENDING_BP_STATE_INFO;


EXTERN_C const IID IID_IDebugPendingBreakpoint2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6e215ef3-e44c-44d1-b7ba-b2401f7dc23d")
    IDebugPendingBreakpoint2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CanBind( 
            /* [out] */ IEnumDebugErrorBreakpoints2 **ppErrorEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Bind( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [out] */ PENDING_BP_STATE_INFO *pState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBreakpointRequest( 
            /* [out] */ IDebugBreakpointRequest2 **ppBPRequest) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Virtualize( 
            /* [in] */ BOOL fVirtualize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enable( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCondition( 
            /* [in] */ BP_CONDITION bpCondition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPassCount( 
            /* [in] */ BP_PASSCOUNT bpPassCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumBoundBreakpoints( 
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumErrorBreakpoints( 
            /* [in] */ BP_ERROR_TYPE bpErrorType,
            /* [out] */ IEnumDebugErrorBreakpoints2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugPendingBreakpoint2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugPendingBreakpoint2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugPendingBreakpoint2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugPendingBreakpoint2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CanBind )( 
            IDebugPendingBreakpoint2 * This,
            /* [out] */ IEnumDebugErrorBreakpoints2 **ppErrorEnum);
        
        HRESULT ( STDMETHODCALLTYPE *Bind )( 
            IDebugPendingBreakpoint2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IDebugPendingBreakpoint2 * This,
            /* [out] */ PENDING_BP_STATE_INFO *pState);
        
        HRESULT ( STDMETHODCALLTYPE *GetBreakpointRequest )( 
            IDebugPendingBreakpoint2 * This,
            /* [out] */ IDebugBreakpointRequest2 **ppBPRequest);
        
        HRESULT ( STDMETHODCALLTYPE *Virtualize )( 
            IDebugPendingBreakpoint2 * This,
            /* [in] */ BOOL fVirtualize);
        
        HRESULT ( STDMETHODCALLTYPE *Enable )( 
            IDebugPendingBreakpoint2 * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *SetCondition )( 
            IDebugPendingBreakpoint2 * This,
            /* [in] */ BP_CONDITION bpCondition);
        
        HRESULT ( STDMETHODCALLTYPE *SetPassCount )( 
            IDebugPendingBreakpoint2 * This,
            /* [in] */ BP_PASSCOUNT bpPassCount);
        
        HRESULT ( STDMETHODCALLTYPE *EnumBoundBreakpoints )( 
            IDebugPendingBreakpoint2 * This,
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *EnumErrorBreakpoints )( 
            IDebugPendingBreakpoint2 * This,
            /* [in] */ BP_ERROR_TYPE bpErrorType,
            /* [out] */ IEnumDebugErrorBreakpoints2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IDebugPendingBreakpoint2 * This);
        
        END_INTERFACE
    } IDebugPendingBreakpoint2Vtbl;

    interface IDebugPendingBreakpoint2
    {
        CONST_VTBL struct IDebugPendingBreakpoint2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugPendingBreakpoint2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugPendingBreakpoint2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugPendingBreakpoint2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugPendingBreakpoint2_CanBind(This,ppErrorEnum)	\
    (This)->lpVtbl -> CanBind(This,ppErrorEnum)

#define IDebugPendingBreakpoint2_Bind(This)	\
    (This)->lpVtbl -> Bind(This)

#define IDebugPendingBreakpoint2_GetState(This,pState)	\
    (This)->lpVtbl -> GetState(This,pState)

#define IDebugPendingBreakpoint2_GetBreakpointRequest(This,ppBPRequest)	\
    (This)->lpVtbl -> GetBreakpointRequest(This,ppBPRequest)

#define IDebugPendingBreakpoint2_Virtualize(This,fVirtualize)	\
    (This)->lpVtbl -> Virtualize(This,fVirtualize)

#define IDebugPendingBreakpoint2_Enable(This,fEnable)	\
    (This)->lpVtbl -> Enable(This,fEnable)

#define IDebugPendingBreakpoint2_SetCondition(This,bpCondition)	\
    (This)->lpVtbl -> SetCondition(This,bpCondition)

#define IDebugPendingBreakpoint2_SetPassCount(This,bpPassCount)	\
    (This)->lpVtbl -> SetPassCount(This,bpPassCount)

#define IDebugPendingBreakpoint2_EnumBoundBreakpoints(This,ppEnum)	\
    (This)->lpVtbl -> EnumBoundBreakpoints(This,ppEnum)

#define IDebugPendingBreakpoint2_EnumErrorBreakpoints(This,bpErrorType,ppEnum)	\
    (This)->lpVtbl -> EnumErrorBreakpoints(This,bpErrorType,ppEnum)

#define IDebugPendingBreakpoint2_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_CanBind_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [out] */ IEnumDebugErrorBreakpoints2 **ppErrorEnum);


void __RPC_STUB IDebugPendingBreakpoint2_CanBind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_Bind_Proxy( 
    IDebugPendingBreakpoint2 * This);


void __RPC_STUB IDebugPendingBreakpoint2_Bind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_GetState_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [out] */ PENDING_BP_STATE_INFO *pState);


void __RPC_STUB IDebugPendingBreakpoint2_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_GetBreakpointRequest_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [out] */ IDebugBreakpointRequest2 **ppBPRequest);


void __RPC_STUB IDebugPendingBreakpoint2_GetBreakpointRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_Virtualize_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [in] */ BOOL fVirtualize);


void __RPC_STUB IDebugPendingBreakpoint2_Virtualize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_Enable_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IDebugPendingBreakpoint2_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_SetCondition_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [in] */ BP_CONDITION bpCondition);


void __RPC_STUB IDebugPendingBreakpoint2_SetCondition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_SetPassCount_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [in] */ BP_PASSCOUNT bpPassCount);


void __RPC_STUB IDebugPendingBreakpoint2_SetPassCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_EnumBoundBreakpoints_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);


void __RPC_STUB IDebugPendingBreakpoint2_EnumBoundBreakpoints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_EnumErrorBreakpoints_Proxy( 
    IDebugPendingBreakpoint2 * This,
    /* [in] */ BP_ERROR_TYPE bpErrorType,
    /* [out] */ IEnumDebugErrorBreakpoints2 **ppEnum);


void __RPC_STUB IDebugPendingBreakpoint2_EnumErrorBreakpoints_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugPendingBreakpoint2_Delete_Proxy( 
    IDebugPendingBreakpoint2 * This);


void __RPC_STUB IDebugPendingBreakpoint2_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugPendingBreakpoint2_INTERFACE_DEFINED__ */


#ifndef __IDebugErrorBreakpoint2_INTERFACE_DEFINED__
#define __IDebugErrorBreakpoint2_INTERFACE_DEFINED__

/* interface IDebugErrorBreakpoint2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugErrorBreakpoint2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("74570ef7-2486-4089-800c-56e3829b5ca4")
    IDebugErrorBreakpoint2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPendingBreakpoint( 
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBreakpoint) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBreakpointResolution( 
            /* [out] */ IDebugErrorBreakpointResolution2 **ppErrorResolution) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugErrorBreakpoint2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugErrorBreakpoint2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugErrorBreakpoint2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugErrorBreakpoint2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPendingBreakpoint )( 
            IDebugErrorBreakpoint2 * This,
            /* [out] */ IDebugPendingBreakpoint2 **ppPendingBreakpoint);
        
        HRESULT ( STDMETHODCALLTYPE *GetBreakpointResolution )( 
            IDebugErrorBreakpoint2 * This,
            /* [out] */ IDebugErrorBreakpointResolution2 **ppErrorResolution);
        
        END_INTERFACE
    } IDebugErrorBreakpoint2Vtbl;

    interface IDebugErrorBreakpoint2
    {
        CONST_VTBL struct IDebugErrorBreakpoint2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugErrorBreakpoint2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugErrorBreakpoint2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugErrorBreakpoint2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugErrorBreakpoint2_GetPendingBreakpoint(This,ppPendingBreakpoint)	\
    (This)->lpVtbl -> GetPendingBreakpoint(This,ppPendingBreakpoint)

#define IDebugErrorBreakpoint2_GetBreakpointResolution(This,ppErrorResolution)	\
    (This)->lpVtbl -> GetBreakpointResolution(This,ppErrorResolution)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugErrorBreakpoint2_GetPendingBreakpoint_Proxy( 
    IDebugErrorBreakpoint2 * This,
    /* [out] */ IDebugPendingBreakpoint2 **ppPendingBreakpoint);


void __RPC_STUB IDebugErrorBreakpoint2_GetPendingBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugErrorBreakpoint2_GetBreakpointResolution_Proxy( 
    IDebugErrorBreakpoint2 * This,
    /* [out] */ IDebugErrorBreakpointResolution2 **ppErrorResolution);


void __RPC_STUB IDebugErrorBreakpoint2_GetBreakpointResolution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugErrorBreakpoint2_INTERFACE_DEFINED__ */


#ifndef __IDebugExpression2_INTERFACE_DEFINED__
#define __IDebugExpression2_INTERFACE_DEFINED__

/* interface IDebugExpression2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugExpression2_0001
    {	EVAL_RETURNVALUE	= 0x2,
	EVAL_NOSIDEEFFECTS	= 0x4,
	EVAL_ALLOWBPS	= 0x8,
	EVAL_ALLOWERRORREPORT	= 0x10,
	EVAL_FUNCTION_AS_ADDRESS	= 0x40,
	EVAL_NOFUNCEVAL	= 0x80,
	EVAL_NOEVENTS	= 0x1000
    } ;
typedef DWORD EVALFLAGS;


EXTERN_C const IID IID_IDebugExpression2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f7473fd0-7f75-478d-8d85-a485204e7a2d")
    IDebugExpression2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EvaluateAsync( 
            /* [in] */ EVALFLAGS dwFlags,
            /* [in] */ IDebugEventCallback2 *pExprCallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EvaluateSync( 
            /* [in] */ EVALFLAGS dwFlags,
            /* [in] */ DWORD dwTimeout,
            /* [in] */ IDebugEventCallback2 *pExprCallback,
            /* [out] */ IDebugProperty2 **ppResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugExpression2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugExpression2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugExpression2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugExpression2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EvaluateAsync )( 
            IDebugExpression2 * This,
            /* [in] */ EVALFLAGS dwFlags,
            /* [in] */ IDebugEventCallback2 *pExprCallback);
        
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IDebugExpression2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *EvaluateSync )( 
            IDebugExpression2 * This,
            /* [in] */ EVALFLAGS dwFlags,
            /* [in] */ DWORD dwTimeout,
            /* [in] */ IDebugEventCallback2 *pExprCallback,
            /* [out] */ IDebugProperty2 **ppResult);
        
        END_INTERFACE
    } IDebugExpression2Vtbl;

    interface IDebugExpression2
    {
        CONST_VTBL struct IDebugExpression2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugExpression2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugExpression2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugExpression2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugExpression2_EvaluateAsync(This,dwFlags,pExprCallback)	\
    (This)->lpVtbl -> EvaluateAsync(This,dwFlags,pExprCallback)

#define IDebugExpression2_Abort(This)	\
    (This)->lpVtbl -> Abort(This)

#define IDebugExpression2_EvaluateSync(This,dwFlags,dwTimeout,pExprCallback,ppResult)	\
    (This)->lpVtbl -> EvaluateSync(This,dwFlags,dwTimeout,pExprCallback,ppResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugExpression2_EvaluateAsync_Proxy( 
    IDebugExpression2 * This,
    /* [in] */ EVALFLAGS dwFlags,
    /* [in] */ IDebugEventCallback2 *pExprCallback);


void __RPC_STUB IDebugExpression2_EvaluateAsync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExpression2_Abort_Proxy( 
    IDebugExpression2 * This);


void __RPC_STUB IDebugExpression2_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugExpression2_EvaluateSync_Proxy( 
    IDebugExpression2 * This,
    /* [in] */ EVALFLAGS dwFlags,
    /* [in] */ DWORD dwTimeout,
    /* [in] */ IDebugEventCallback2 *pExprCallback,
    /* [out] */ IDebugProperty2 **ppResult);


void __RPC_STUB IDebugExpression2_EvaluateSync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugExpression2_INTERFACE_DEFINED__ */


#ifndef __IDebugModule2_INTERFACE_DEFINED__
#define __IDebugModule2_INTERFACE_DEFINED__

/* interface IDebugModule2 */
/* [unique][uuid][object] */ 


enum __MIDL_IDebugModule2_0001
    {	MODULE_FLAG_NONE	= 0,
	MODULE_FLAG_SYSTEM	= 0x1,
	MODULE_FLAG_SYMBOLS	= 0x2,
	MODULE_FLAG_64BIT	= 0x4
    } ;
typedef DWORD MODULE_FLAGS;


enum __MIDL_IDebugModule2_0002
    {	MIF_NONE	= 0,
	MIF_NAME	= 0x1,
	MIF_URL	= 0x2,
	MIF_VERSION	= 0x4,
	MIF_DEBUGMESSAGE	= 0x8,
	MIF_LOADADDRESS	= 0x10,
	MIF_PREFFEREDADDRESS	= 0x20,
	MIF_SIZE	= 0x40,
	MIF_LOADORDER	= 0x80,
	MIF_TIMESTAMP	= 0x100,
	MIF_URLSYMBOLLOCATION	= 0x200,
	MIF_FLAGS	= 0x400,
	MIF_ALLFIELDS	= 0x7ff
    } ;
typedef DWORD MODULE_INFO_FIELDS;

typedef struct _tagMODULE_INFO
    {
    MODULE_INFO_FIELDS dwValidFields;
    BSTR m_bstrName;
    BSTR m_bstrUrl;
    BSTR m_bstrVersion;
    BSTR m_bstrDebugMessage;
    UINT64 m_addrLoadAddress;
    UINT64 m_addrPreferredLoadAddress;
    DWORD m_dwSize;
    DWORD m_dwLoadOrder;
    FILETIME m_TimeStamp;
    BSTR m_bstrUrlSymbolLocation;
    MODULE_FLAGS m_dwModuleFlags;
    } 	MODULE_INFO;


EXTERN_C const IID IID_IDebugModule2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0fc1cd9a-b912-405c-a04c-43ce02cd7df2")
    IDebugModule2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInfo( 
            /* [in] */ MODULE_INFO_FIELDS dwFields,
            /* [out] */ MODULE_INFO *pInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReloadSymbols( 
            /* [full][in] */ LPCOLESTR pszUrlToSymbols,
            /* [out] */ BSTR *pbstrDebugMessage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugModule2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugModule2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugModule2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugModule2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInfo )( 
            IDebugModule2 * This,
            /* [in] */ MODULE_INFO_FIELDS dwFields,
            /* [out] */ MODULE_INFO *pInfo);
        
        HRESULT ( STDMETHODCALLTYPE *ReloadSymbols )( 
            IDebugModule2 * This,
            /* [full][in] */ LPCOLESTR pszUrlToSymbols,
            /* [out] */ BSTR *pbstrDebugMessage);
        
        END_INTERFACE
    } IDebugModule2Vtbl;

    interface IDebugModule2
    {
        CONST_VTBL struct IDebugModule2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugModule2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugModule2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugModule2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugModule2_GetInfo(This,dwFields,pInfo)	\
    (This)->lpVtbl -> GetInfo(This,dwFields,pInfo)

#define IDebugModule2_ReloadSymbols(This,pszUrlToSymbols,pbstrDebugMessage)	\
    (This)->lpVtbl -> ReloadSymbols(This,pszUrlToSymbols,pbstrDebugMessage)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugModule2_GetInfo_Proxy( 
    IDebugModule2 * This,
    /* [in] */ MODULE_INFO_FIELDS dwFields,
    /* [out] */ MODULE_INFO *pInfo);


void __RPC_STUB IDebugModule2_GetInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugModule2_ReloadSymbols_Proxy( 
    IDebugModule2 * This,
    /* [full][in] */ LPCOLESTR pszUrlToSymbols,
    /* [out] */ BSTR *pbstrDebugMessage);


void __RPC_STUB IDebugModule2_ReloadSymbols_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugModule2_INTERFACE_DEFINED__ */


#ifndef __IDebugModuleManaged_INTERFACE_DEFINED__
#define __IDebugModuleManaged_INTERFACE_DEFINED__

/* interface IDebugModuleManaged */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugModuleManaged;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("232397F8-B232-479d-B1BB-2F044C70A0F9")
    IDebugModuleManaged : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetMvid( 
            /* [out] */ GUID *mvid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugModuleManagedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugModuleManaged * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugModuleManaged * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugModuleManaged * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMvid )( 
            IDebugModuleManaged * This,
            /* [out] */ GUID *mvid);
        
        END_INTERFACE
    } IDebugModuleManagedVtbl;

    interface IDebugModuleManaged
    {
        CONST_VTBL struct IDebugModuleManagedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugModuleManaged_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugModuleManaged_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugModuleManaged_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugModuleManaged_GetMvid(This,mvid)	\
    (This)->lpVtbl -> GetMvid(This,mvid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugModuleManaged_GetMvid_Proxy( 
    IDebugModuleManaged * This,
    /* [out] */ GUID *mvid);


void __RPC_STUB IDebugModuleManaged_GetMvid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugModuleManaged_INTERFACE_DEFINED__ */


#ifndef __IDebugDocument2_INTERFACE_DEFINED__
#define __IDebugDocument2_INTERFACE_DEFINED__

/* interface IDebugDocument2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugDocument2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1606dd73-5d5f-405c-b4f4-ce32baba2501")
    IDebugDocument2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetName( 
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocumentClassId( 
            /* [out] */ CLSID *pclsid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDocument2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDocument2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDocument2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDocument2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugDocument2 * This,
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrFileName);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentClassId )( 
            IDebugDocument2 * This,
            /* [out] */ CLSID *pclsid);
        
        END_INTERFACE
    } IDebugDocument2Vtbl;

    interface IDebugDocument2
    {
        CONST_VTBL struct IDebugDocument2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDocument2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDocument2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDocument2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDocument2_GetName(This,gnType,pbstrFileName)	\
    (This)->lpVtbl -> GetName(This,gnType,pbstrFileName)

#define IDebugDocument2_GetDocumentClassId(This,pclsid)	\
    (This)->lpVtbl -> GetDocumentClassId(This,pclsid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDocument2_GetName_Proxy( 
    IDebugDocument2 * This,
    /* [in] */ GETNAME_TYPE gnType,
    /* [out] */ BSTR *pbstrFileName);


void __RPC_STUB IDebugDocument2_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocument2_GetDocumentClassId_Proxy( 
    IDebugDocument2 * This,
    /* [out] */ CLSID *pclsid);


void __RPC_STUB IDebugDocument2_GetDocumentClassId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDocument2_INTERFACE_DEFINED__ */


#ifndef __IDebugDocumentText2_INTERFACE_DEFINED__
#define __IDebugDocumentText2_INTERFACE_DEFINED__

/* interface IDebugDocumentText2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugDocumentText2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4b0645aa-08ef-4cb9-adb9-0395d6edad35")
    IDebugDocumentText2 : public IDebugDocument2
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSize( 
            /* [full][out][in] */ ULONG *pcNumLines,
            /* [full][out][in] */ ULONG *pcNumChars) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetText( 
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ ULONG cMaxChars,
            /* [size_is][length_is][out] */ WCHAR *pText,
            /* [out] */ ULONG *pcNumChars) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDocumentText2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDocumentText2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDocumentText2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDocumentText2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IDebugDocumentText2 * This,
            /* [in] */ GETNAME_TYPE gnType,
            /* [out] */ BSTR *pbstrFileName);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocumentClassId )( 
            IDebugDocumentText2 * This,
            /* [out] */ CLSID *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSize )( 
            IDebugDocumentText2 * This,
            /* [full][out][in] */ ULONG *pcNumLines,
            /* [full][out][in] */ ULONG *pcNumChars);
        
        HRESULT ( STDMETHODCALLTYPE *GetText )( 
            IDebugDocumentText2 * This,
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ ULONG cMaxChars,
            /* [size_is][length_is][out] */ WCHAR *pText,
            /* [out] */ ULONG *pcNumChars);
        
        END_INTERFACE
    } IDebugDocumentText2Vtbl;

    interface IDebugDocumentText2
    {
        CONST_VTBL struct IDebugDocumentText2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDocumentText2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDocumentText2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDocumentText2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDocumentText2_GetName(This,gnType,pbstrFileName)	\
    (This)->lpVtbl -> GetName(This,gnType,pbstrFileName)

#define IDebugDocumentText2_GetDocumentClassId(This,pclsid)	\
    (This)->lpVtbl -> GetDocumentClassId(This,pclsid)


#define IDebugDocumentText2_GetSize(This,pcNumLines,pcNumChars)	\
    (This)->lpVtbl -> GetSize(This,pcNumLines,pcNumChars)

#define IDebugDocumentText2_GetText(This,pos,cMaxChars,pText,pcNumChars)	\
    (This)->lpVtbl -> GetText(This,pos,cMaxChars,pText,pcNumChars)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDocumentText2_GetSize_Proxy( 
    IDebugDocumentText2 * This,
    /* [full][out][in] */ ULONG *pcNumLines,
    /* [full][out][in] */ ULONG *pcNumChars);


void __RPC_STUB IDebugDocumentText2_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentText2_GetText_Proxy( 
    IDebugDocumentText2 * This,
    /* [in] */ TEXT_POSITION pos,
    /* [in] */ ULONG cMaxChars,
    /* [size_is][length_is][out] */ WCHAR *pText,
    /* [out] */ ULONG *pcNumChars);


void __RPC_STUB IDebugDocumentText2_GetText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDocumentText2_INTERFACE_DEFINED__ */


#ifndef __IDebugDocumentPosition2_INTERFACE_DEFINED__
#define __IDebugDocumentPosition2_INTERFACE_DEFINED__

/* interface IDebugDocumentPosition2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugDocumentPosition2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bdde0eee-3b8d-4c82-b529-33f16b42832e")
    IDebugDocumentPosition2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFileName( 
            /* [out] */ BSTR *pbstrFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocument( 
            /* [out] */ IDebugDocument2 **ppDoc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsPositionInDocument( 
            /* [in] */ IDebugDocument2 *pDoc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRange( 
            /* [full][out][in] */ TEXT_POSITION *pBegPosition,
            /* [full][out][in] */ TEXT_POSITION *pEndPosition) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDocumentPosition2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDocumentPosition2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDocumentPosition2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDocumentPosition2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFileName )( 
            IDebugDocumentPosition2 * This,
            /* [out] */ BSTR *pbstrFileName);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocument )( 
            IDebugDocumentPosition2 * This,
            /* [out] */ IDebugDocument2 **ppDoc);
        
        HRESULT ( STDMETHODCALLTYPE *IsPositionInDocument )( 
            IDebugDocumentPosition2 * This,
            /* [in] */ IDebugDocument2 *pDoc);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IDebugDocumentPosition2 * This,
            /* [full][out][in] */ TEXT_POSITION *pBegPosition,
            /* [full][out][in] */ TEXT_POSITION *pEndPosition);
        
        END_INTERFACE
    } IDebugDocumentPosition2Vtbl;

    interface IDebugDocumentPosition2
    {
        CONST_VTBL struct IDebugDocumentPosition2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDocumentPosition2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDocumentPosition2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDocumentPosition2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDocumentPosition2_GetFileName(This,pbstrFileName)	\
    (This)->lpVtbl -> GetFileName(This,pbstrFileName)

#define IDebugDocumentPosition2_GetDocument(This,ppDoc)	\
    (This)->lpVtbl -> GetDocument(This,ppDoc)

#define IDebugDocumentPosition2_IsPositionInDocument(This,pDoc)	\
    (This)->lpVtbl -> IsPositionInDocument(This,pDoc)

#define IDebugDocumentPosition2_GetRange(This,pBegPosition,pEndPosition)	\
    (This)->lpVtbl -> GetRange(This,pBegPosition,pEndPosition)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDocumentPosition2_GetFileName_Proxy( 
    IDebugDocumentPosition2 * This,
    /* [out] */ BSTR *pbstrFileName);


void __RPC_STUB IDebugDocumentPosition2_GetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentPosition2_GetDocument_Proxy( 
    IDebugDocumentPosition2 * This,
    /* [out] */ IDebugDocument2 **ppDoc);


void __RPC_STUB IDebugDocumentPosition2_GetDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentPosition2_IsPositionInDocument_Proxy( 
    IDebugDocumentPosition2 * This,
    /* [in] */ IDebugDocument2 *pDoc);


void __RPC_STUB IDebugDocumentPosition2_IsPositionInDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentPosition2_GetRange_Proxy( 
    IDebugDocumentPosition2 * This,
    /* [full][out][in] */ TEXT_POSITION *pBegPosition,
    /* [full][out][in] */ TEXT_POSITION *pEndPosition);


void __RPC_STUB IDebugDocumentPosition2_GetRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDocumentPosition2_INTERFACE_DEFINED__ */


#ifndef __IDebugDocumentPositionOffset2_INTERFACE_DEFINED__
#define __IDebugDocumentPositionOffset2_INTERFACE_DEFINED__

/* interface IDebugDocumentPositionOffset2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugDocumentPositionOffset2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("037edd0f-8551-4f7f-8ca0-04d9e29f532d")
    IDebugDocumentPositionOffset2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRange( 
            /* [full][out][in] */ DWORD *pdwBegOffset,
            /* [full][out][in] */ DWORD *pdwEndOffset) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDocumentPositionOffset2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDocumentPositionOffset2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDocumentPositionOffset2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDocumentPositionOffset2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IDebugDocumentPositionOffset2 * This,
            /* [full][out][in] */ DWORD *pdwBegOffset,
            /* [full][out][in] */ DWORD *pdwEndOffset);
        
        END_INTERFACE
    } IDebugDocumentPositionOffset2Vtbl;

    interface IDebugDocumentPositionOffset2
    {
        CONST_VTBL struct IDebugDocumentPositionOffset2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDocumentPositionOffset2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDocumentPositionOffset2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDocumentPositionOffset2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDocumentPositionOffset2_GetRange(This,pdwBegOffset,pdwEndOffset)	\
    (This)->lpVtbl -> GetRange(This,pdwBegOffset,pdwEndOffset)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDocumentPositionOffset2_GetRange_Proxy( 
    IDebugDocumentPositionOffset2 * This,
    /* [full][out][in] */ DWORD *pdwBegOffset,
    /* [full][out][in] */ DWORD *pdwEndOffset);


void __RPC_STUB IDebugDocumentPositionOffset2_GetRange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDocumentPositionOffset2_INTERFACE_DEFINED__ */


#ifndef __IDebugFunctionPosition2_INTERFACE_DEFINED__
#define __IDebugFunctionPosition2_INTERFACE_DEFINED__

/* interface IDebugFunctionPosition2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugFunctionPosition2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1ede3b4b-35e7-4b97-8133-02845d600174")
    IDebugFunctionPosition2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFunctionName( 
            /* [out] */ BSTR *pbstrFunctionName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOffset( 
            /* [full][out][in] */ TEXT_POSITION *pPosition) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugFunctionPosition2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugFunctionPosition2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugFunctionPosition2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugFunctionPosition2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFunctionName )( 
            IDebugFunctionPosition2 * This,
            /* [out] */ BSTR *pbstrFunctionName);
        
        HRESULT ( STDMETHODCALLTYPE *GetOffset )( 
            IDebugFunctionPosition2 * This,
            /* [full][out][in] */ TEXT_POSITION *pPosition);
        
        END_INTERFACE
    } IDebugFunctionPosition2Vtbl;

    interface IDebugFunctionPosition2
    {
        CONST_VTBL struct IDebugFunctionPosition2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugFunctionPosition2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugFunctionPosition2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugFunctionPosition2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugFunctionPosition2_GetFunctionName(This,pbstrFunctionName)	\
    (This)->lpVtbl -> GetFunctionName(This,pbstrFunctionName)

#define IDebugFunctionPosition2_GetOffset(This,pPosition)	\
    (This)->lpVtbl -> GetOffset(This,pPosition)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugFunctionPosition2_GetFunctionName_Proxy( 
    IDebugFunctionPosition2 * This,
    /* [out] */ BSTR *pbstrFunctionName);


void __RPC_STUB IDebugFunctionPosition2_GetFunctionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugFunctionPosition2_GetOffset_Proxy( 
    IDebugFunctionPosition2 * This,
    /* [full][out][in] */ TEXT_POSITION *pPosition);


void __RPC_STUB IDebugFunctionPosition2_GetOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugFunctionPosition2_INTERFACE_DEFINED__ */


#ifndef __IDebugDocumentTextEvents2_INTERFACE_DEFINED__
#define __IDebugDocumentTextEvents2_INTERFACE_DEFINED__

/* interface IDebugDocumentTextEvents2 */
/* [unique][uuid][object] */ 

typedef DWORD TEXT_DOC_ATTR_2;

#define	TEXT_DOC_ATTR_READONLY_2	( 0x1 )


EXTERN_C const IID IID_IDebugDocumentTextEvents2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("33ec72e3-002f-4966-b91c-5ce2f7ba5124")
    IDebugDocumentTextEvents2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE onDestroy( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE onInsertText( 
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToInsert) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE onRemoveText( 
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToRemove) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE onReplaceText( 
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToReplace) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE onUpdateTextAttributes( 
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToUpdate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE onUpdateDocumentAttributes( 
            /* [in] */ TEXT_DOC_ATTR_2 textdocattr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugDocumentTextEvents2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugDocumentTextEvents2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugDocumentTextEvents2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugDocumentTextEvents2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *onDestroy )( 
            IDebugDocumentTextEvents2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *onInsertText )( 
            IDebugDocumentTextEvents2 * This,
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToInsert);
        
        HRESULT ( STDMETHODCALLTYPE *onRemoveText )( 
            IDebugDocumentTextEvents2 * This,
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToRemove);
        
        HRESULT ( STDMETHODCALLTYPE *onReplaceText )( 
            IDebugDocumentTextEvents2 * This,
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToReplace);
        
        HRESULT ( STDMETHODCALLTYPE *onUpdateTextAttributes )( 
            IDebugDocumentTextEvents2 * This,
            /* [in] */ TEXT_POSITION pos,
            /* [in] */ DWORD dwNumToUpdate);
        
        HRESULT ( STDMETHODCALLTYPE *onUpdateDocumentAttributes )( 
            IDebugDocumentTextEvents2 * This,
            /* [in] */ TEXT_DOC_ATTR_2 textdocattr);
        
        END_INTERFACE
    } IDebugDocumentTextEvents2Vtbl;

    interface IDebugDocumentTextEvents2
    {
        CONST_VTBL struct IDebugDocumentTextEvents2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugDocumentTextEvents2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugDocumentTextEvents2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugDocumentTextEvents2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugDocumentTextEvents2_onDestroy(This)	\
    (This)->lpVtbl -> onDestroy(This)

#define IDebugDocumentTextEvents2_onInsertText(This,pos,dwNumToInsert)	\
    (This)->lpVtbl -> onInsertText(This,pos,dwNumToInsert)

#define IDebugDocumentTextEvents2_onRemoveText(This,pos,dwNumToRemove)	\
    (This)->lpVtbl -> onRemoveText(This,pos,dwNumToRemove)

#define IDebugDocumentTextEvents2_onReplaceText(This,pos,dwNumToReplace)	\
    (This)->lpVtbl -> onReplaceText(This,pos,dwNumToReplace)

#define IDebugDocumentTextEvents2_onUpdateTextAttributes(This,pos,dwNumToUpdate)	\
    (This)->lpVtbl -> onUpdateTextAttributes(This,pos,dwNumToUpdate)

#define IDebugDocumentTextEvents2_onUpdateDocumentAttributes(This,textdocattr)	\
    (This)->lpVtbl -> onUpdateDocumentAttributes(This,textdocattr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugDocumentTextEvents2_onDestroy_Proxy( 
    IDebugDocumentTextEvents2 * This);


void __RPC_STUB IDebugDocumentTextEvents2_onDestroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentTextEvents2_onInsertText_Proxy( 
    IDebugDocumentTextEvents2 * This,
    /* [in] */ TEXT_POSITION pos,
    /* [in] */ DWORD dwNumToInsert);


void __RPC_STUB IDebugDocumentTextEvents2_onInsertText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentTextEvents2_onRemoveText_Proxy( 
    IDebugDocumentTextEvents2 * This,
    /* [in] */ TEXT_POSITION pos,
    /* [in] */ DWORD dwNumToRemove);


void __RPC_STUB IDebugDocumentTextEvents2_onRemoveText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentTextEvents2_onReplaceText_Proxy( 
    IDebugDocumentTextEvents2 * This,
    /* [in] */ TEXT_POSITION pos,
    /* [in] */ DWORD dwNumToReplace);


void __RPC_STUB IDebugDocumentTextEvents2_onReplaceText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentTextEvents2_onUpdateTextAttributes_Proxy( 
    IDebugDocumentTextEvents2 * This,
    /* [in] */ TEXT_POSITION pos,
    /* [in] */ DWORD dwNumToUpdate);


void __RPC_STUB IDebugDocumentTextEvents2_onUpdateTextAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDebugDocumentTextEvents2_onUpdateDocumentAttributes_Proxy( 
    IDebugDocumentTextEvents2 * This,
    /* [in] */ TEXT_DOC_ATTR_2 textdocattr);


void __RPC_STUB IDebugDocumentTextEvents2_onUpdateDocumentAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugDocumentTextEvents2_INTERFACE_DEFINED__ */


#ifndef __IDebugQueryEngine2_INTERFACE_DEFINED__
#define __IDebugQueryEngine2_INTERFACE_DEFINED__

/* interface IDebugQueryEngine2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IDebugQueryEngine2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c989adc9-f305-4ef5-8ca2-20898e8d0e28")
    IDebugQueryEngine2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetEngineInterface( 
            /* [out] */ IUnknown **ppUnk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDebugQueryEngine2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDebugQueryEngine2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDebugQueryEngine2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDebugQueryEngine2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetEngineInterface )( 
            IDebugQueryEngine2 * This,
            /* [out] */ IUnknown **ppUnk);
        
        END_INTERFACE
    } IDebugQueryEngine2Vtbl;

    interface IDebugQueryEngine2
    {
        CONST_VTBL struct IDebugQueryEngine2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDebugQueryEngine2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDebugQueryEngine2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDebugQueryEngine2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDebugQueryEngine2_GetEngineInterface(This,ppUnk)	\
    (This)->lpVtbl -> GetEngineInterface(This,ppUnk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDebugQueryEngine2_GetEngineInterface_Proxy( 
    IDebugQueryEngine2 * This,
    /* [out] */ IUnknown **ppUnk);


void __RPC_STUB IDebugQueryEngine2_GetEngineInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDebugQueryEngine2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugProcesses2_INTERFACE_DEFINED__
#define __IEnumDebugProcesses2_INTERFACE_DEFINED__

/* interface IEnumDebugProcesses2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugProcesses2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("96c74ef4-185d-4f9a-8a43-4d2723758e0a")
    IEnumDebugProcesses2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugProcess2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugProcesses2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugProcesses2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugProcesses2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugProcesses2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugProcesses2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugProcesses2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugProcess2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugProcesses2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugProcesses2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugProcesses2 * This,
            /* [out] */ IEnumDebugProcesses2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugProcesses2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugProcesses2Vtbl;

    interface IEnumDebugProcesses2
    {
        CONST_VTBL struct IEnumDebugProcesses2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugProcesses2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugProcesses2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugProcesses2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugProcesses2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugProcesses2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugProcesses2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugProcesses2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugProcesses2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugProcesses2_Next_Proxy( 
    IEnumDebugProcesses2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugProcess2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugProcesses2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugProcesses2_Skip_Proxy( 
    IEnumDebugProcesses2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugProcesses2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugProcesses2_Reset_Proxy( 
    IEnumDebugProcesses2 * This);


void __RPC_STUB IEnumDebugProcesses2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugProcesses2_Clone_Proxy( 
    IEnumDebugProcesses2 * This,
    /* [out] */ IEnumDebugProcesses2 **ppEnum);


void __RPC_STUB IEnumDebugProcesses2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugProcesses2_GetCount_Proxy( 
    IEnumDebugProcesses2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugProcesses2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugProcesses2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugPrograms2_INTERFACE_DEFINED__
#define __IEnumDebugPrograms2_INTERFACE_DEFINED__

/* interface IEnumDebugPrograms2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugPrograms2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8d14bca6-34ce-4efe-ac7e-0abc61dadb20")
    IEnumDebugPrograms2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugProgram2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugPrograms2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugPrograms2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugPrograms2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugPrograms2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugPrograms2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugPrograms2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugProgram2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugPrograms2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugPrograms2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugPrograms2 * This,
            /* [out] */ IEnumDebugPrograms2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugPrograms2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugPrograms2Vtbl;

    interface IEnumDebugPrograms2
    {
        CONST_VTBL struct IEnumDebugPrograms2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugPrograms2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugPrograms2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugPrograms2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugPrograms2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugPrograms2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugPrograms2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugPrograms2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugPrograms2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugPrograms2_Next_Proxy( 
    IEnumDebugPrograms2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugProgram2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugPrograms2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPrograms2_Skip_Proxy( 
    IEnumDebugPrograms2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugPrograms2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPrograms2_Reset_Proxy( 
    IEnumDebugPrograms2 * This);


void __RPC_STUB IEnumDebugPrograms2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPrograms2_Clone_Proxy( 
    IEnumDebugPrograms2 * This,
    /* [out] */ IEnumDebugPrograms2 **ppEnum);


void __RPC_STUB IEnumDebugPrograms2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPrograms2_GetCount_Proxy( 
    IEnumDebugPrograms2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugPrograms2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugPrograms2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugThreads2_INTERFACE_DEFINED__
#define __IEnumDebugThreads2_INTERFACE_DEFINED__

/* interface IEnumDebugThreads2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugThreads2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0d30dc12-c4f8-433d-9fcc-9ff117e5e5f4")
    IEnumDebugThreads2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugThread2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugThreads2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugThreads2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugThreads2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugThreads2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugThreads2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugThreads2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugThread2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugThreads2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugThreads2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugThreads2 * This,
            /* [out] */ IEnumDebugThreads2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugThreads2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugThreads2Vtbl;

    interface IEnumDebugThreads2
    {
        CONST_VTBL struct IEnumDebugThreads2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugThreads2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugThreads2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugThreads2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugThreads2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugThreads2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugThreads2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugThreads2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugThreads2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugThreads2_Next_Proxy( 
    IEnumDebugThreads2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugThread2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugThreads2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugThreads2_Skip_Proxy( 
    IEnumDebugThreads2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugThreads2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugThreads2_Reset_Proxy( 
    IEnumDebugThreads2 * This);


void __RPC_STUB IEnumDebugThreads2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugThreads2_Clone_Proxy( 
    IEnumDebugThreads2 * This,
    /* [out] */ IEnumDebugThreads2 **ppEnum);


void __RPC_STUB IEnumDebugThreads2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugThreads2_GetCount_Proxy( 
    IEnumDebugThreads2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugThreads2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugThreads2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugStackFrames2_INTERFACE_DEFINED__
#define __IEnumDebugStackFrames2_INTERFACE_DEFINED__

/* interface IEnumDebugStackFrames2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugStackFrames2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cd39102b-4b69-4495-8f29-e0b25c4a8855")
    IEnumDebugStackFrames2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugStackFrame2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugStackFrames2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIndex( 
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [out][in] */ ULONG *pIndex) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugStackFrames2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugStackFrames2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugStackFrames2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugStackFrames2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugStackFrames2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugStackFrame2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugStackFrames2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugStackFrames2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugStackFrames2 * This,
            /* [out] */ IEnumDebugStackFrames2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugStackFrames2 * This,
            /* [out] */ ULONG *pcelt);
        
        HRESULT ( STDMETHODCALLTYPE *GetIndex )( 
            IEnumDebugStackFrames2 * This,
            /* [in] */ IDebugStackFrame2 *pStackFrame,
            /* [out][in] */ ULONG *pIndex);
        
        END_INTERFACE
    } IEnumDebugStackFrames2Vtbl;

    interface IEnumDebugStackFrames2
    {
        CONST_VTBL struct IEnumDebugStackFrames2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugStackFrames2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugStackFrames2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugStackFrames2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugStackFrames2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugStackFrames2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugStackFrames2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugStackFrames2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugStackFrames2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#define IEnumDebugStackFrames2_GetIndex(This,pStackFrame,pIndex)	\
    (This)->lpVtbl -> GetIndex(This,pStackFrame,pIndex)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugStackFrames2_Next_Proxy( 
    IEnumDebugStackFrames2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugStackFrame2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugStackFrames2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugStackFrames2_Skip_Proxy( 
    IEnumDebugStackFrames2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugStackFrames2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugStackFrames2_Reset_Proxy( 
    IEnumDebugStackFrames2 * This);


void __RPC_STUB IEnumDebugStackFrames2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugStackFrames2_Clone_Proxy( 
    IEnumDebugStackFrames2 * This,
    /* [out] */ IEnumDebugStackFrames2 **ppEnum);


void __RPC_STUB IEnumDebugStackFrames2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugStackFrames2_GetCount_Proxy( 
    IEnumDebugStackFrames2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugStackFrames2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugStackFrames2_GetIndex_Proxy( 
    IEnumDebugStackFrames2 * This,
    /* [in] */ IDebugStackFrame2 *pStackFrame,
    /* [out][in] */ ULONG *pIndex);


void __RPC_STUB IEnumDebugStackFrames2_GetIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugStackFrames2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugCodeContexts2_INTERFACE_DEFINED__
#define __IEnumDebugCodeContexts2_INTERFACE_DEFINED__

/* interface IEnumDebugCodeContexts2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugCodeContexts2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ad47a80b-eda7-459e-af82-647cc9fbaa50")
    IEnumDebugCodeContexts2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugCodeContext2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugCodeContexts2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugCodeContexts2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugCodeContexts2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugCodeContexts2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugCodeContexts2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugCodeContext2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugCodeContexts2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugCodeContexts2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugCodeContexts2 * This,
            /* [out] */ IEnumDebugCodeContexts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugCodeContexts2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugCodeContexts2Vtbl;

    interface IEnumDebugCodeContexts2
    {
        CONST_VTBL struct IEnumDebugCodeContexts2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugCodeContexts2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugCodeContexts2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugCodeContexts2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugCodeContexts2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugCodeContexts2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugCodeContexts2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugCodeContexts2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugCodeContexts2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugCodeContexts2_Next_Proxy( 
    IEnumDebugCodeContexts2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugCodeContext2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugCodeContexts2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugCodeContexts2_Skip_Proxy( 
    IEnumDebugCodeContexts2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugCodeContexts2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugCodeContexts2_Reset_Proxy( 
    IEnumDebugCodeContexts2 * This);


void __RPC_STUB IEnumDebugCodeContexts2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugCodeContexts2_Clone_Proxy( 
    IEnumDebugCodeContexts2 * This,
    /* [out] */ IEnumDebugCodeContexts2 **ppEnum);


void __RPC_STUB IEnumDebugCodeContexts2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugCodeContexts2_GetCount_Proxy( 
    IEnumDebugCodeContexts2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugCodeContexts2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugCodeContexts2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugBoundBreakpoints2_INTERFACE_DEFINED__
#define __IEnumDebugBoundBreakpoints2_INTERFACE_DEFINED__

/* interface IEnumDebugBoundBreakpoints2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugBoundBreakpoints2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0f6b37e0-fcfe-44d9-9112-394ca9b92114")
    IEnumDebugBoundBreakpoints2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugBoundBreakpoint2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugBoundBreakpoints2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugBoundBreakpoints2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugBoundBreakpoints2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugBoundBreakpoints2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugBoundBreakpoints2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugBoundBreakpoint2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugBoundBreakpoints2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugBoundBreakpoints2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugBoundBreakpoints2 * This,
            /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugBoundBreakpoints2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugBoundBreakpoints2Vtbl;

    interface IEnumDebugBoundBreakpoints2
    {
        CONST_VTBL struct IEnumDebugBoundBreakpoints2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugBoundBreakpoints2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugBoundBreakpoints2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugBoundBreakpoints2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugBoundBreakpoints2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugBoundBreakpoints2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugBoundBreakpoints2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugBoundBreakpoints2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugBoundBreakpoints2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugBoundBreakpoints2_Next_Proxy( 
    IEnumDebugBoundBreakpoints2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugBoundBreakpoint2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugBoundBreakpoints2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugBoundBreakpoints2_Skip_Proxy( 
    IEnumDebugBoundBreakpoints2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugBoundBreakpoints2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugBoundBreakpoints2_Reset_Proxy( 
    IEnumDebugBoundBreakpoints2 * This);


void __RPC_STUB IEnumDebugBoundBreakpoints2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugBoundBreakpoints2_Clone_Proxy( 
    IEnumDebugBoundBreakpoints2 * This,
    /* [out] */ IEnumDebugBoundBreakpoints2 **ppEnum);


void __RPC_STUB IEnumDebugBoundBreakpoints2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugBoundBreakpoints2_GetCount_Proxy( 
    IEnumDebugBoundBreakpoints2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugBoundBreakpoints2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugBoundBreakpoints2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugPendingBreakpoints2_INTERFACE_DEFINED__
#define __IEnumDebugPendingBreakpoints2_INTERFACE_DEFINED__

/* interface IEnumDebugPendingBreakpoints2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugPendingBreakpoints2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70d2dc1e-4dcc-4786-a072-9a3b600c216b")
    IEnumDebugPendingBreakpoints2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugPendingBreakpoint2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugPendingBreakpoints2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugPendingBreakpoints2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugPendingBreakpoints2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugPendingBreakpoints2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugPendingBreakpoints2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugPendingBreakpoints2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugPendingBreakpoint2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugPendingBreakpoints2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugPendingBreakpoints2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugPendingBreakpoints2 * This,
            /* [out] */ IEnumDebugPendingBreakpoints2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugPendingBreakpoints2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugPendingBreakpoints2Vtbl;

    interface IEnumDebugPendingBreakpoints2
    {
        CONST_VTBL struct IEnumDebugPendingBreakpoints2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugPendingBreakpoints2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugPendingBreakpoints2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugPendingBreakpoints2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugPendingBreakpoints2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugPendingBreakpoints2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugPendingBreakpoints2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugPendingBreakpoints2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugPendingBreakpoints2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugPendingBreakpoints2_Next_Proxy( 
    IEnumDebugPendingBreakpoints2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugPendingBreakpoint2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugPendingBreakpoints2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPendingBreakpoints2_Skip_Proxy( 
    IEnumDebugPendingBreakpoints2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugPendingBreakpoints2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPendingBreakpoints2_Reset_Proxy( 
    IEnumDebugPendingBreakpoints2 * This);


void __RPC_STUB IEnumDebugPendingBreakpoints2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPendingBreakpoints2_Clone_Proxy( 
    IEnumDebugPendingBreakpoints2 * This,
    /* [out] */ IEnumDebugPendingBreakpoints2 **ppEnum);


void __RPC_STUB IEnumDebugPendingBreakpoints2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPendingBreakpoints2_GetCount_Proxy( 
    IEnumDebugPendingBreakpoints2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugPendingBreakpoints2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugPendingBreakpoints2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugErrorBreakpoints2_INTERFACE_DEFINED__
#define __IEnumDebugErrorBreakpoints2_INTERFACE_DEFINED__

/* interface IEnumDebugErrorBreakpoints2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugErrorBreakpoints2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e158f5aa-31fe-491b-a9f6-cff934b03a01")
    IEnumDebugErrorBreakpoints2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugErrorBreakpoint2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugErrorBreakpoints2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugErrorBreakpoints2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugErrorBreakpoints2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugErrorBreakpoints2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugErrorBreakpoints2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugErrorBreakpoints2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugErrorBreakpoint2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugErrorBreakpoints2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugErrorBreakpoints2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugErrorBreakpoints2 * This,
            /* [out] */ IEnumDebugErrorBreakpoints2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugErrorBreakpoints2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugErrorBreakpoints2Vtbl;

    interface IEnumDebugErrorBreakpoints2
    {
        CONST_VTBL struct IEnumDebugErrorBreakpoints2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugErrorBreakpoints2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugErrorBreakpoints2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugErrorBreakpoints2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugErrorBreakpoints2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugErrorBreakpoints2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugErrorBreakpoints2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugErrorBreakpoints2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugErrorBreakpoints2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugErrorBreakpoints2_Next_Proxy( 
    IEnumDebugErrorBreakpoints2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugErrorBreakpoint2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugErrorBreakpoints2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorBreakpoints2_Skip_Proxy( 
    IEnumDebugErrorBreakpoints2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugErrorBreakpoints2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorBreakpoints2_Reset_Proxy( 
    IEnumDebugErrorBreakpoints2 * This);


void __RPC_STUB IEnumDebugErrorBreakpoints2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorBreakpoints2_Clone_Proxy( 
    IEnumDebugErrorBreakpoints2 * This,
    /* [out] */ IEnumDebugErrorBreakpoints2 **ppEnum);


void __RPC_STUB IEnumDebugErrorBreakpoints2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugErrorBreakpoints2_GetCount_Proxy( 
    IEnumDebugErrorBreakpoints2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugErrorBreakpoints2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugErrorBreakpoints2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugMachines2_INTERFACE_DEFINED__
#define __IEnumDebugMachines2_INTERFACE_DEFINED__

/* interface IEnumDebugMachines2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugMachines2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("61d986ec-1eac-46b6-90ff-402a008f15d1")
    IEnumDebugMachines2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugMachine2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugMachines2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugMachines2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugMachines2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugMachines2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugMachines2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugMachines2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugMachine2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugMachines2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugMachines2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugMachines2 * This,
            /* [out] */ IEnumDebugMachines2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugMachines2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugMachines2Vtbl;

    interface IEnumDebugMachines2
    {
        CONST_VTBL struct IEnumDebugMachines2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugMachines2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugMachines2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugMachines2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugMachines2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugMachines2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugMachines2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugMachines2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugMachines2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugMachines2_Next_Proxy( 
    IEnumDebugMachines2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugMachine2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugMachines2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugMachines2_Skip_Proxy( 
    IEnumDebugMachines2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugMachines2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugMachines2_Reset_Proxy( 
    IEnumDebugMachines2 * This);


void __RPC_STUB IEnumDebugMachines2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugMachines2_Clone_Proxy( 
    IEnumDebugMachines2 * This,
    /* [out] */ IEnumDebugMachines2 **ppEnum);


void __RPC_STUB IEnumDebugMachines2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugMachines2_GetCount_Proxy( 
    IEnumDebugMachines2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugMachines2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugMachines2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugExceptionInfo2_INTERFACE_DEFINED__
#define __IEnumDebugExceptionInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugExceptionInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugExceptionInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8e4bbd34-a2f4-41ef-87b5-c563b4ad6ee7")
    IEnumDebugExceptionInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ EXCEPTION_INFO *rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugExceptionInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugExceptionInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugExceptionInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugExceptionInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugExceptionInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugExceptionInfo2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ EXCEPTION_INFO *rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugExceptionInfo2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugExceptionInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugExceptionInfo2 * This,
            /* [out] */ IEnumDebugExceptionInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugExceptionInfo2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugExceptionInfo2Vtbl;

    interface IEnumDebugExceptionInfo2
    {
        CONST_VTBL struct IEnumDebugExceptionInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugExceptionInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugExceptionInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugExceptionInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugExceptionInfo2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugExceptionInfo2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugExceptionInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugExceptionInfo2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugExceptionInfo2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugExceptionInfo2_Next_Proxy( 
    IEnumDebugExceptionInfo2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ EXCEPTION_INFO *rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugExceptionInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugExceptionInfo2_Skip_Proxy( 
    IEnumDebugExceptionInfo2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugExceptionInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugExceptionInfo2_Reset_Proxy( 
    IEnumDebugExceptionInfo2 * This);


void __RPC_STUB IEnumDebugExceptionInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugExceptionInfo2_Clone_Proxy( 
    IEnumDebugExceptionInfo2 * This,
    /* [out] */ IEnumDebugExceptionInfo2 **ppEnum);


void __RPC_STUB IEnumDebugExceptionInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugExceptionInfo2_GetCount_Proxy( 
    IEnumDebugExceptionInfo2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugExceptionInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugExceptionInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugFrameInfo2_INTERFACE_DEFINED__
#define __IEnumDebugFrameInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugFrameInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugFrameInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("98bbba48-4c4d-4fff-8340-6097bec9c894")
    IEnumDebugFrameInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ FRAMEINFO *rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugFrameInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugFrameInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugFrameInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugFrameInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugFrameInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugFrameInfo2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ FRAMEINFO *rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugFrameInfo2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugFrameInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugFrameInfo2 * This,
            /* [out] */ IEnumDebugFrameInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugFrameInfo2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugFrameInfo2Vtbl;

    interface IEnumDebugFrameInfo2
    {
        CONST_VTBL struct IEnumDebugFrameInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugFrameInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugFrameInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugFrameInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugFrameInfo2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugFrameInfo2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugFrameInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugFrameInfo2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugFrameInfo2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugFrameInfo2_Next_Proxy( 
    IEnumDebugFrameInfo2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ FRAMEINFO *rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugFrameInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFrameInfo2_Skip_Proxy( 
    IEnumDebugFrameInfo2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugFrameInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFrameInfo2_Reset_Proxy( 
    IEnumDebugFrameInfo2 * This);


void __RPC_STUB IEnumDebugFrameInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFrameInfo2_Clone_Proxy( 
    IEnumDebugFrameInfo2 * This,
    /* [out] */ IEnumDebugFrameInfo2 **ppEnum);


void __RPC_STUB IEnumDebugFrameInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugFrameInfo2_GetCount_Proxy( 
    IEnumDebugFrameInfo2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugFrameInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugFrameInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumCodePaths2_INTERFACE_DEFINED__
#define __IEnumCodePaths2_INTERFACE_DEFINED__

/* interface IEnumCodePaths2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumCodePaths2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9b13f80d-cfc6-4b78-81ef-1f7cc33f7639")
    IEnumCodePaths2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ CODE_PATH *rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumCodePaths2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCodePaths2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumCodePaths2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumCodePaths2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumCodePaths2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumCodePaths2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ CODE_PATH *rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumCodePaths2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumCodePaths2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumCodePaths2 * This,
            /* [out] */ IEnumCodePaths2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumCodePaths2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumCodePaths2Vtbl;

    interface IEnumCodePaths2
    {
        CONST_VTBL struct IEnumCodePaths2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCodePaths2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCodePaths2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCodePaths2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCodePaths2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumCodePaths2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCodePaths2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCodePaths2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumCodePaths2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCodePaths2_Next_Proxy( 
    IEnumCodePaths2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ CODE_PATH *rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumCodePaths2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCodePaths2_Skip_Proxy( 
    IEnumCodePaths2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumCodePaths2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCodePaths2_Reset_Proxy( 
    IEnumCodePaths2 * This);


void __RPC_STUB IEnumCodePaths2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCodePaths2_Clone_Proxy( 
    IEnumCodePaths2 * This,
    /* [out] */ IEnumCodePaths2 **ppEnum);


void __RPC_STUB IEnumCodePaths2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCodePaths2_GetCount_Proxy( 
    IEnumCodePaths2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumCodePaths2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCodePaths2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugModules2_INTERFACE_DEFINED__
#define __IEnumDebugModules2_INTERFACE_DEFINED__

/* interface IEnumDebugModules2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugModules2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4c4a2835-682e-4ce1-aebc-1e6b3a165b44")
    IEnumDebugModules2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugModule2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugModules2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugModules2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugModules2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugModules2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugModules2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugModules2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugModule2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugModules2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugModules2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugModules2 * This,
            /* [out] */ IEnumDebugModules2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugModules2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugModules2Vtbl;

    interface IEnumDebugModules2
    {
        CONST_VTBL struct IEnumDebugModules2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugModules2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugModules2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugModules2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugModules2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugModules2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugModules2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugModules2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugModules2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugModules2_Next_Proxy( 
    IEnumDebugModules2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugModule2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugModules2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugModules2_Skip_Proxy( 
    IEnumDebugModules2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugModules2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugModules2_Reset_Proxy( 
    IEnumDebugModules2 * This);


void __RPC_STUB IEnumDebugModules2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugModules2_Clone_Proxy( 
    IEnumDebugModules2 * This,
    /* [out] */ IEnumDebugModules2 **ppEnum);


void __RPC_STUB IEnumDebugModules2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugModules2_GetCount_Proxy( 
    IEnumDebugModules2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugModules2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugModules2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugPortSuppliers2_INTERFACE_DEFINED__
#define __IEnumDebugPortSuppliers2_INTERFACE_DEFINED__

/* interface IEnumDebugPortSuppliers2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugPortSuppliers2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("59c9dc99-3eff-4ff3-b201-98acd01b0d87")
    IEnumDebugPortSuppliers2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugPortSupplier2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugPortSuppliers2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugPortSuppliers2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugPortSuppliers2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugPortSuppliers2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugPortSuppliers2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugPortSuppliers2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugPortSupplier2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugPortSuppliers2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugPortSuppliers2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugPortSuppliers2 * This,
            /* [out] */ IEnumDebugPortSuppliers2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugPortSuppliers2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugPortSuppliers2Vtbl;

    interface IEnumDebugPortSuppliers2
    {
        CONST_VTBL struct IEnumDebugPortSuppliers2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugPortSuppliers2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugPortSuppliers2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugPortSuppliers2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugPortSuppliers2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugPortSuppliers2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugPortSuppliers2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugPortSuppliers2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugPortSuppliers2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugPortSuppliers2_Next_Proxy( 
    IEnumDebugPortSuppliers2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugPortSupplier2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugPortSuppliers2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPortSuppliers2_Skip_Proxy( 
    IEnumDebugPortSuppliers2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugPortSuppliers2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPortSuppliers2_Reset_Proxy( 
    IEnumDebugPortSuppliers2 * This);


void __RPC_STUB IEnumDebugPortSuppliers2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPortSuppliers2_Clone_Proxy( 
    IEnumDebugPortSuppliers2 * This,
    /* [out] */ IEnumDebugPortSuppliers2 **ppEnum);


void __RPC_STUB IEnumDebugPortSuppliers2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPortSuppliers2_GetCount_Proxy( 
    IEnumDebugPortSuppliers2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugPortSuppliers2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugPortSuppliers2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugPorts2_INTERFACE_DEFINED__
#define __IEnumDebugPorts2_INTERFACE_DEFINED__

/* interface IEnumDebugPorts2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugPorts2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bc827c5e-99ae-4ac8-83ad-2ea5c2034333")
    IEnumDebugPorts2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugPort2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugPorts2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugPorts2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugPorts2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugPorts2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugPorts2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugPorts2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IDebugPort2 **rgelt,
            /* [out][in] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugPorts2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugPorts2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugPorts2 * This,
            /* [out] */ IEnumDebugPorts2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugPorts2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugPorts2Vtbl;

    interface IEnumDebugPorts2
    {
        CONST_VTBL struct IEnumDebugPorts2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugPorts2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugPorts2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugPorts2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugPorts2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugPorts2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugPorts2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugPorts2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugPorts2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugPorts2_Next_Proxy( 
    IEnumDebugPorts2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IDebugPort2 **rgelt,
    /* [out][in] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugPorts2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPorts2_Skip_Proxy( 
    IEnumDebugPorts2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugPorts2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPorts2_Reset_Proxy( 
    IEnumDebugPorts2 * This);


void __RPC_STUB IEnumDebugPorts2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPorts2_Clone_Proxy( 
    IEnumDebugPorts2 * This,
    /* [out] */ IEnumDebugPorts2 **ppEnum);


void __RPC_STUB IEnumDebugPorts2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPorts2_GetCount_Proxy( 
    IEnumDebugPorts2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugPorts2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugPorts2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugPropertyInfo2_INTERFACE_DEFINED__
#define __IEnumDebugPropertyInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugPropertyInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugPropertyInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6c7072c3-3ac4-408f-a680-fc5a2f96903e")
    IEnumDebugPropertyInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ DEBUG_PROPERTY_INFO *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugPropertyInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugPropertyInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugPropertyInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugPropertyInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugPropertyInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugPropertyInfo2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ DEBUG_PROPERTY_INFO *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugPropertyInfo2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugPropertyInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugPropertyInfo2 * This,
            /* [out] */ IEnumDebugPropertyInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugPropertyInfo2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugPropertyInfo2Vtbl;

    interface IEnumDebugPropertyInfo2
    {
        CONST_VTBL struct IEnumDebugPropertyInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugPropertyInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugPropertyInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugPropertyInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugPropertyInfo2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugPropertyInfo2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugPropertyInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugPropertyInfo2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugPropertyInfo2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugPropertyInfo2_Next_Proxy( 
    IEnumDebugPropertyInfo2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ DEBUG_PROPERTY_INFO *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugPropertyInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPropertyInfo2_Skip_Proxy( 
    IEnumDebugPropertyInfo2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugPropertyInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPropertyInfo2_Reset_Proxy( 
    IEnumDebugPropertyInfo2 * This);


void __RPC_STUB IEnumDebugPropertyInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPropertyInfo2_Clone_Proxy( 
    IEnumDebugPropertyInfo2 * This,
    /* [out] */ IEnumDebugPropertyInfo2 **ppEnum);


void __RPC_STUB IEnumDebugPropertyInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugPropertyInfo2_GetCount_Proxy( 
    IEnumDebugPropertyInfo2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugPropertyInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugPropertyInfo2_INTERFACE_DEFINED__ */


#ifndef __IEnumDebugReferenceInfo2_INTERFACE_DEFINED__
#define __IEnumDebugReferenceInfo2_INTERFACE_DEFINED__

/* interface IEnumDebugReferenceInfo2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IEnumDebugReferenceInfo2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e459dd12-864f-4aaa-abc1-dcecbc267f04")
    IEnumDebugReferenceInfo2 : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ DEBUG_REFERENCE_INFO *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumDebugReferenceInfo2 **ppEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ ULONG *pcelt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumDebugReferenceInfo2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumDebugReferenceInfo2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumDebugReferenceInfo2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumDebugReferenceInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumDebugReferenceInfo2 * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ DEBUG_REFERENCE_INFO *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumDebugReferenceInfo2 * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumDebugReferenceInfo2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumDebugReferenceInfo2 * This,
            /* [out] */ IEnumDebugReferenceInfo2 **ppEnum);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IEnumDebugReferenceInfo2 * This,
            /* [out] */ ULONG *pcelt);
        
        END_INTERFACE
    } IEnumDebugReferenceInfo2Vtbl;

    interface IEnumDebugReferenceInfo2
    {
        CONST_VTBL struct IEnumDebugReferenceInfo2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumDebugReferenceInfo2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumDebugReferenceInfo2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumDebugReferenceInfo2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumDebugReferenceInfo2_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumDebugReferenceInfo2_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumDebugReferenceInfo2_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumDebugReferenceInfo2_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumDebugReferenceInfo2_GetCount(This,pcelt)	\
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumDebugReferenceInfo2_Next_Proxy( 
    IEnumDebugReferenceInfo2 * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ DEBUG_REFERENCE_INFO *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumDebugReferenceInfo2_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugReferenceInfo2_Skip_Proxy( 
    IEnumDebugReferenceInfo2 * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumDebugReferenceInfo2_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugReferenceInfo2_Reset_Proxy( 
    IEnumDebugReferenceInfo2 * This);


void __RPC_STUB IEnumDebugReferenceInfo2_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugReferenceInfo2_Clone_Proxy( 
    IEnumDebugReferenceInfo2 * This,
    /* [out] */ IEnumDebugReferenceInfo2 **ppEnum);


void __RPC_STUB IEnumDebugReferenceInfo2_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumDebugReferenceInfo2_GetCount_Proxy( 
    IEnumDebugReferenceInfo2 * This,
    /* [out] */ ULONG *pcelt);


void __RPC_STUB IEnumDebugReferenceInfo2_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumDebugReferenceInfo2_INTERFACE_DEFINED__ */



#ifndef __AD2Lib_LIBRARY_DEFINED__
#define __AD2Lib_LIBRARY_DEFINED__

/* library AD2Lib */
/* [uuid] */ 


EXTERN_C const IID LIBID_AD2Lib;

EXTERN_C const CLSID CLSID_SDMServer;

#ifdef __cplusplus

class DECLSPEC_UUID("5eb7d9f7-af21-400e-a2c4-7fd6396f8641")
SDMServer;
#endif

EXTERN_C const CLSID CLSID_MsMachineDebugManager;

#ifdef __cplusplus

class DECLSPEC_UUID("73b25ffd-f501-437b-8b11-7f0de383964f")
MsMachineDebugManager;
#endif

EXTERN_C const CLSID CLSID_MDMUtilServer;

#ifdef __cplusplus

class DECLSPEC_UUID("b20e899d-b079-479d-a4dc-10f758d9cd9a")
MDMUtilServer;
#endif
#endif /* __AD2Lib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


