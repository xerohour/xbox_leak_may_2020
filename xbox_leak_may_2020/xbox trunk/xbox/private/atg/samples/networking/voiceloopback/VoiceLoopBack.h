//-----------------------------------------------------------------------------
// File: VoiceLoopBack.h
//
// Desc: Demonstrates the usage of the Xbox communicator.
//       This sample naively sends any input from the first
//       headset it finds connected to the system out to the
//       same headset with a 1 second delay.
//
// Hist: 08.14.01 - New for Aug M1 release 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBStopWatch.h>
#include <cassert>
#include <algorithm>
#include "DelayVoice.h"

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD COLOR_HIGHLIGHT   = 0xffffff00; // Yellow
const DWORD COLOR_GREEN       = 0xff00ff00;
const DWORD COLOR_NORMAL      = 0xffffffff;
const DWORD MAX_ERROR_STR     = 64;
const DWORD MAX_STATUS_STR    = 128;


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont             m_Font;                         // game font
    WCHAR               m_strError[ MAX_ERROR_STR ];    // error message
    WCHAR               m_strStatus[ MAX_STATUS_STR ];  // status

    // Voice Related Data
    HANDLE              m_hVoiceThread;
    HANDLE              m_hVoiceDeleteEvent;
    DWORD               m_dwVoiceThreadId;
    CDelayVoiceUnit    *m_pVoiceUnit;                   // The single headset

public:
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

    CXBoxSample();

    // Voice Related
    HRESULT VoiceThreadProc();
    static DWORD WINAPI StaticVoiceThreadProc(LPVOID pParameter);

private:
    VOID Init();

    // Voice related functions
    VOID  KillVoiceProcessing();
    int   FindHeadsetPort();
    VOID  StartVoiceProcessing();
};
