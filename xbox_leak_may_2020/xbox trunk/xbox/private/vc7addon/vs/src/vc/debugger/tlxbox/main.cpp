/*++

Copyright (c) Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    Implementation of the Xbox Debugger "Transport Layer" for VS7

    This is not really a transport layer in the same sense at tlloc, tlnet, tlgambit are for VC.  In those cases,
    the transport layer acts a bridge to a debug monitor layer that has the same implementation either running
    locally or across a wire.  There is more than one debugger monitor and they are interchangeable with the different
    transport layers.

    On Xbox the debugger monitor is xbdm.dll, which runs on the xbox.  xboxdbg.dll is a sort of transport layer which
    communicates over the wire to it.  This code is an adaptor module.  Firstly, it exposes the transport layer
    interface to connect to VS.  Secondly, it translates VS\VC debug monitor requests into calls xboxdbg.dll.
    
Author:
    mitchd

--*/

#include <precomp.h>

DMDBG_NO_RECURSION_DECLARE

// Declare DM globals
DM_GLOBALS g_DmGlobals = {0};


int
WINAPI
DllMain(
    HINSTANCE   hModule,
    DWORD       dwReason,
    DWORD       dwReserved
    )
/*++
  Routine Description:
    DllMain initializes the library.  Not much to do right now.
--*/
{
    if(DLL_PROCESS_ATTACH==dwReason)
    {
        DMDBG_NO_RECURSION_ALLOC
        DisableThreadLibraryCalls (hModule);
    } else if(DLL_PROCESS_DETACH==dwReason)     
    {
        DMDBG_NO_RECURSION_FREE
    }
    return TRUE;
}

XOSD
WINAPI
TLFunc(
    TLF     wCommand,
    HPID    hpid,
    WPARAM  wParam,
    LPARAM  lParam
    )
/*++

Routine Description:

    Main entry point.  All requests from the EM come in through here.

Arguments:

    wCommand    - Supplies the command to be executed.
    hpid        - Supplies the hpid for which the command is to be executed.
    wParam      - Supplies information about the command.
    lParam      - Supplies information about the command.

Return Value:

    XOSD error code.  xosdNone means that no errors occured.  Other error
    codes are defined in osdebug\include\od.h.

--*/

{
  XOSD xosd = xosdNone;

  switch (wCommand)
  {
    case tlfInit:
      //Init the DM
      xosd = DMInit(reinterpret_cast<TLCALLBACKTYPE>(lParam), reinterpret_cast<LPDBF>(wParam));
      break;
    case tlfLoadDM:
      // For Xbox the DM is already loaded, we are it.
      // Most of the state initialization will be done on dmfInit.
      break;
    case tlfDestroy:
      //TODO: Clean ourselves up
      break;
    case tlfConnect:

      break;
    case tlfDisconnect:

      break;

    case tlfSetEMBuffer:
      g_DmGlobals.pfnGetEmWBBuffer = reinterpret_cast<LPTLEMBUFFERMSG>(lParam)->pEmFunc;
      g_DmGlobals.cbEmWBBuffer     = reinterpret_cast<LPTLEMBUFFERMSG>(lParam)->cbBuffer;
      break;

    case tlfReply:
        // Place EM replies in the receive buffer.  This ought to get called in the
        // context of a thread that called the EM and is waiting for a reply.  Thus there
        // is no need to signal a reply.
        _ASSERTE(wParam <= INITIAL_DMSENDBUFFERSIZE);
        _fmemcpy (g_DmGlobals.bReceiveBuffer, (LPBYTE) lParam, wParam );
        break;
    
    case tlfDebugPacket:
    case tlfRequest:
        xosd = DispatchRequest(reinterpret_cast<LPDBB>(lParam),wParam);
        break;
 
    default:
        _ASSERTE ( FALSE );
        break;
    }
    return xosd;
}


//
//  A couple of macros to help us during development
//
#define DMF_NOT_YET_IMPLEMENTED(_dmfValue_)                 \
    case _dmfValue_:                                        \
      DPRINT(0, ("Not Yet Implemented: %s", #_dmfValue_));  \
      break

#if DBG
#define DMF_NOT_CALLED(_dmfValue_)                      \
    case _dmfValue_:                                    \
      DPRINT(0, ("Not Expecting: %s", #_dmfValue_));    \
      _ASSERTE(false);                                  \
      xosd = xosdUnsupported;                           \
      break
#else
    #define DMF_NOT_CALLED(_dmfValue_)
#endif



XOSD DispatchRequest(LPDBB lpdbb, DWORD cb)
/*++

Routine Description:

    This routine dispatch requests and debug packets.  This takes dmf
    message packets from the debugger and handles them, usually
    by dispatching to a worker function.

Arguments:

    cb      - supplies size of data packet

    lpdbb   - supplies pointer to packet

Return Value:

--*/
{
    
    //  The EM needs a reply.  Rather than pass in a pointer to an out-parameter (like a normal
    //  caller) it tells the TLFunc (using tlfSetEMBuffer) about a function to call to get a 
    //  "Reply Buffer".  (No doubt there is some history here.)  The reply buffer is per thread,
    //  but can be changed (resized even?) between calls.  We will just get it here.  Aside effect
    //  of this, is that we would be in trouble if we rentered this dispatch routine from the same
    //  thread (a.k.a. recursion).  The nice DMDBG_NO_RECURSION_XXX macros work in debug builds to
    //  detect and assert any coding errors we make in this respect.
    DMDBG_NO_RECURSION_ENTER
    EMBUFFERINFO *lpBufInfo = g_DmGlobals.pfnGetEmWBBuffer();
    LPDM_MSG lpDmMsg = reinterpret_cast<LPDM_MSG>(lpBufInfo->lpEmWBBuf);
    memset(lpBufInfo->lpEmWBBuf, 0, lpBufInfo->cbEmWBBuf);

    DMF   dmf = (DMF) (lpdbb->dmf & 0xffff);
    XOSD  xosd = xosdNone;
    CXboxProcess *pProcess = CXboxProcess::GetProcess(lpdbb->hpid);

    // The case statements are in the same order that they are declared in emdm.h, except
    // for commands that are nops on Xbox, in which case they are at the end.
    switch ( dmf ) {

      //dmfInit - NOP
      //dmfUnInit - NOP
      DMF_NOT_CALLED(dmfSelect);
      DMF_NOT_CALLED(dmfConnect);

      case dmfCreatePid:
        // Create a process to go along with the HPID
        _ASSERTE(NULL == pProcess);
        xosd = CXboxProcess::Create(lpdbb->hpid);
        break;

      case dmfDestroyPid:
        // Destroy the process.
        _ASSERTE(pProcess);
        xosd = pProcess->Destroy();
        break;

      DMF_NOT_YET_IMPLEMENTED(dmfSetPath);
      DMF_NOT_YET_IMPLEMENTED(dmfSpawnOrphan);  //ProcessSpawnOprhanCmd(lpdbb);
  
      case dmfProgLoad:
        xosd = pProcess->ProgramLoad(reinterpret_cast<LPPROGLOAD>(lpdbb->rgbVar));
        break;
      
      DMF_NOT_YET_IMPLEMENTED(dmfLoadCrashDump);
      DMF_NOT_YET_IMPLEMENTED(dmfProgFree);
      DMF_NOT_YET_IMPLEMENTED(dmfDebugActive);
      DMF_NOT_YET_IMPLEMENTED(dmfTerm);
      DMF_NOT_YET_IMPLEMENTED(dmfDetach);

      DMF_NOT_YET_IMPLEMENTED(dmfGo);
      DMF_NOT_YET_IMPLEMENTED(dmfStop);
      DMF_NOT_YET_IMPLEMENTED(dmfNextReadableMemory);
      DMF_NOT_YET_IMPLEMENTED(dmfThreadManipulate);
      DMF_NOT_YET_IMPLEMENTED(dmfLockUnLock);

      DMF_NOT_YET_IMPLEMENTED(dmfSingleStep);
      DMF_NOT_YET_IMPLEMENTED(dmfReturnStep);
      DMF_NOT_YET_IMPLEMENTED(dmfNonLocalGoto);
      DMF_NOT_YET_IMPLEMENTED(dmfRangeStep);
      DMF_NOT_YET_IMPLEMENTED(dmfGoToReturn);

      DMF_NOT_YET_IMPLEMENTED(dmfBreakpoint);
      case dmfGetExceptionState:
        if(pProcess)
            xosd = pProcess->GetExceptionInfo(reinterpret_cast<LPEXCMD>(lpdbb->rgbVar),
                                              reinterpret_cast<EXCEPTION_DESCRIPTION *>(lpDmMsg->rgb)
                                              );
        else
            xosd = CXboxProcess::GetDefaultExceptionInfo(reinterpret_cast<LPEXCMD>(lpdbb->rgbVar),
                                                         reinterpret_cast<EXCEPTION_DESCRIPTION *>(lpDmMsg->rgb)
                                                         );
        break;
      DMF_NOT_YET_IMPLEMENTED(dmfSetExceptionState);

      DMF_NOT_YET_IMPLEMENTED(dmfReadMem);
      DMF_NOT_YET_IMPLEMENTED(dmfReadReg);
      DMF_NOT_YET_IMPLEMENTED(dmfReadFrameReg);
      DMF_NOT_YET_IMPLEMENTED(dmfWriteMem);
      DMF_NOT_YET_IMPLEMENTED(dmfWriteReg);
      DMF_NOT_YET_IMPLEMENTED(dmfWriteFrameReg);
      DMF_NOT_YET_IMPLEMENTED(dmfVirtualAllocEx);
      DMF_NOT_YET_IMPLEMENTED(dmfGetFP);
      DMF_NOT_YET_IMPLEMENTED(dmfSetFP);
      DMF_NOT_YET_IMPLEMENTED(dmfThreadStatus);
      DMF_NOT_YET_IMPLEMENTED(dmfProcessStatus);
      DMF_NOT_YET_IMPLEMENTED(dmfQueryTlsBase);
      DMF_NOT_YET_IMPLEMENTED(dmfQuerySelector);
      DMF_NOT_YET_IMPLEMENTED(dmfVirtualQuery);
      DMF_NOT_YET_IMPLEMENTED(dmfReadRegEx);
      DMF_NOT_YET_IMPLEMENTED(dmfWriteRegEx);
      DMF_NOT_YET_IMPLEMENTED(dmfGetSections);

      DMF_NOT_YET_IMPLEMENTED(dmfInit32SegValues);

      case dmfSelLim:
        // OSD want to know the maximum legal address, Xbox uses
        // a flat memory model, so the answer is always 0xFFFFFFFF.
        reinterpret_cast<DWORD *>(lpDmMsg->rgb)[0] = 0xFFFFFFFF;
        break;

      case dmfSetMulti:

#ifndef MULTI_XBOX_DEBUGGING    //notice there is no else, MULTI_XBOX_DEBUGGING is
        xosd = xosdUnsupported; //not yet (perhaps never) supported.
#endif
        break;

      //NOP - dmfClearMulti
      //NOP - (dmfDebugger);
      DMF_NOT_YET_IMPLEMENTED(dmfSync);
      DMF_NOT_YET_IMPLEMENTED(dmfSystemService);
      DMF_NOT_YET_IMPLEMENTED(dmfGetPrompt);
      DMF_NOT_YET_IMPLEMENTED(dmfSendChar);
      
      case dmfGetDmInfo:
        GetDebuggerInfo(reinterpret_cast<DMINFO *>(lpDmMsg->rgb));
        break;

      DMF_NOT_YET_IMPLEMENTED(dmfSetupExecute);
      DMF_NOT_YET_IMPLEMENTED(dmfStartExecute);
      DMF_NOT_YET_IMPLEMENTED(dmfCleanUpExecute);

      DMF_NOT_YET_IMPLEMENTED(dmfRemoteUtility);       // for the mfile utility

      DMF_NOT_YET_IMPLEMENTED(dmfGetSpecialReg);
      DMF_NOT_YET_IMPLEMENTED(dmfSetSpecialReg);

      //
      // File I/O
      //
      DMF_NOT_YET_IMPLEMENTED(dmfGetImageInformation);
      DMF_NOT_YET_IMPLEMENTED(dmfFindFirstFile);
      DMF_NOT_YET_IMPLEMENTED(dmfFindNextFile);
      DMF_NOT_YET_IMPLEMENTED(dmfCloseFindFile);
      DMF_NOT_YET_IMPLEMENTED(dmfOpenFile);
      DMF_NOT_YET_IMPLEMENTED(dmfReadFile);
      DMF_NOT_YET_IMPLEMENTED(dmfSeekFile);
      DMF_NOT_YET_IMPLEMENTED(dmfCloseFile);
      DMF_NOT_YET_IMPLEMENTED(dmfWriteCrashDump);

      DMF_NOT_YET_IMPLEMENTED(dmfGetDebuggableTaskList);
    
      // Load Library
      DMF_NOT_YET_IMPLEMENTED(dmfLoadLibrary);

      // Engine is ready for interop
      DMF_NOT_YET_IMPLEMENTED(dmfEngineReady);
      
      //**
      //**  Everything below here is a NOP
      //**
      case dmfInit:
      case dmfUnInit:
      case dmfClearMulti:
      case dmfDebugger:
        break;
    default:
        DPRINT(5, (_T("Unknown\n")));
        _ASSERTE(FALSE);
        xosd = xosdUnsupported;
        break;
    }
    
    //Set the error code
    lpDmMsg->xosdRet = xosd;

    DMDBG_NO_RECURSION_LEAVE
    return xosd;
}

XOSD DMInit(TLCALLBACKTYPE pfnEntryPoint, LPDBF pdbf)
{
    XOSD xosd = xosdNone;

    // Save the debugger helper
    g_DmGlobals.pDebugFunctions = pdbf;
    // Save the EM's entry points for calling it from the DM.
    g_DmGlobals.pfnCallEm = pfnEntryPoint;
    
    // Initialize the process list
    xosd = CXboxProcess::InitializeCXboxProcessClass();

    /* Define a false single step event */
    /*
    falseSSEvent.dwDebugEventCode = EXCEPTION_DEBUG_EVENT;
    falseSSEvent.u.Exception.ExceptionRecord.ExceptionCode
          = EXCEPTION_SINGLE_STEP;

    falseBPEvent.dwDebugEventCode = BREAKPOINT_DEBUG_EVENT;
    falseBPEvent.u.Exception.ExceptionRecord.ExceptionCode
          = EXCEPTION_BREAKPOINT;

    FuncExitEvent.dwDebugEventCode = FUNC_EXIT_EVENT;
    FuncExitEvent.u.Exception.ExceptionRecord.ExceptionCode
          = EXCEPTION_SINGLE_STEP;
    */
    /* Define the standard notification method */
    /*
    EMNotifyMethod.notifyFunction = ConsumeThreadEventsAndNotifyEM;
    EMNotifyMethod.lparam     = (LPVOID)0;
    */
/*  
    SearchPathString[0] = _T('\0');
    SearchPathSet       = FALSE;
    

    InitEventQueue();
    InitDEQ();

    //
    // initialize data breakpoint handler
    //
    ExprBPInitialize();

    // Xbox has known hardware
    g_ProcFlags = PROCESSOR_FLAGS_EXTENDED | PROCESSOR_FLAGS_I386_MMX | PROCESSOR_FLAGS_I386_MMX2 | PROCESSOR_FLAGS_I386_MMX3;

 */
    return xosd;
}

void GetDebuggerInfo(OUT DMINFO *pDebuggerInfo)
{
    pDebuggerInfo->Processor.Type = mptix86;
    pDebuggerInfo->Processor.Level = 6;
    pDebuggerInfo->Processor.Endian = endLittle;
    pDebuggerInfo->Processor.processorFlags = PROCESSOR_FLAGS_EXTENDED|PROCESSOR_FLAGS_I386_MMX|
                                            PROCESSOR_FLAGS_I386_MMX2|PROCESSOR_FLAGS_I386_MMX3;
    pDebuggerInfo->Processor.pageSize = 4096;
}