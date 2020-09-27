//-----------------------------------------------------------------------------
// File: PerfTest.cpp
//
// Desc: Functions for gathering performance statistics. Note: this code is
//       only valid for DEBUG builds
//
// Note: The simplest way to use the Perf Counters API is to run your app,
//       hit a breakpoint, and then directly modify D3D::g_PerfCounters in 
//       a debugger watch window, setting the m_FrameRateIntervalTime member
//       for how often the internal perftest functions should dump perftest
//       information. (Or, instead of using the debugger window, you could call
//       the D3DPERF_SetShowFrameRateInterval() function in your code.) By
//       default, the perftest functions dump information to the debug port.
//
//       Alternatively, you can use the m_pfnDumpFrameRateInfoHandler member
//       to override the funtion, having it call a function in your app
//       instead. This latter method is used in this app, just to demonstrate
//       it. In many cases, the information dumped to the debug port will be
//       all you need to do performance testing on your app.
//
// Hist: 05.14.01 - New
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <xgraphics.h>
#include <D3D8Perf.h>



// Menu structures and definitions for the perf test options
struct MenuItem
{
    WCHAR* strLabel;
    DWORD  dwSelection;
    WCHAR* strOptions[10];
};

MenuItem g_Menus[] = 
{
    { L"Test duration:",        2, { L"1 second", L"2 seconds", L"5 seconds", NULL }, },
    { L"Output method:",        0, { L"Onscreen", L"Debug port", NULL }, },
    { L"Enable perf profiler:", 0, { L"Yes", L"No", NULL }, },
    { NULL }
};

#define MENU_TESTDURATION          0
#define MENU_TESTDURATION_1SECOND     0
#define MENU_TESTDURATION_2SECONDS    1
#define MENU_TESTDURATION_5SECONDS    2

#define MENU_OUTPUTMETHOD          1
#define MENU_OUTPUTMETHOD_ONSCREEN     0
#define MENU_OUTPUTMETHOD_DEBUGPORT    1

#define MENU_PERFPROFILER          2
#define MENU_PERFPROFILER_ENABLED      0
#define MENU_PERFPROFILER_DISABLED     1


// Menu control
BOOL  g_bDisplayPerfTestMenu = FALSE;
DWORD g_dwSelectedMenu       = 0;


// Perf test options
FLOAT g_fPerfTestTime       = 5.0f;
BOOL  g_bPerfTestRunning    = FALSE;
BOOL  g_bEnablePerfProfiler = FALSE;
BOOL  g_bDumpToScreen       = FALSE;
BOOL  g_bDisplayPerfResults = FALSE;


// Global perf test reuslts
FLOAT g_fTime;
DWORD g_dwNumFrames;
FLOAT g_fFPS;
FLOAT g_fPushBufferBytes;
FLOAT g_fVertices;
FLOAT g_fMinFrameTime;
FLOAT g_fAvgFrameTime;
FLOAT g_fMaxFrameTime;
DWORD g_dwNumProfileSamples;
DWORD g_dwNumGPUSamples;
DWORD g_dwNumFrontEndSamples;
DWORD g_dwNumBackEndSamples;
DWORD g_dwObjectLockWaits;
FLOAT g_fObjectLockWaitTime;
FLOAT g_fObjectLockTimePerWait;
DWORD g_dwPushBufferWaits;
FLOAT g_fPushBufferWaitTime;
FLOAT g_fPushBufferTimePerWait;
DWORD g_dwPresentWaits;
FLOAT g_fPresentWaitTime;
FLOAT g_fPresentTimePerWait;


// Perf testing function protocols
VOID StartPerfTest();
VOID StopPerfTest();
VOID PerfTestCallback();
VOID GetPerfStatistics();
VOID DisplayPerfResults( CXBFont* pFont );
VOID DumpPerfResults();


// Helper for getting the current time
__forceinline ULONGLONG ReadTimeStampCounter()
{
    _asm rdtsc
}


    
    
//-----------------------------------------------------------------------------
// Name: UpdateMenus()
// Desc: Update the perf test options in a menu. The menus are for the user to
//       be able to select various perf testing options.
//-----------------------------------------------------------------------------
VOID UpdateMenus( XBGAMEPAD* pGamepad )
{
    if( pGamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN )
    {
        if( g_Menus[g_dwSelectedMenu+1].strLabel )
            g_dwSelectedMenu++;
    }

    if( pGamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP )
    {
        if( g_dwSelectedMenu > 0 )
            g_dwSelectedMenu--;
    }

    if( pGamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT )
    {
        if( g_Menus[g_dwSelectedMenu].strOptions[g_Menus[g_dwSelectedMenu].dwSelection+1] )
            g_Menus[g_dwSelectedMenu].dwSelection++;
    }

    if( pGamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT )
    {
        if( g_Menus[g_dwSelectedMenu].dwSelection > 0 )
            g_Menus[g_dwSelectedMenu].dwSelection--;
    }
}




//-----------------------------------------------------------------------------
// Name: DisplayMenus()
// Desc: Display the perf test options in a menu. The menus are for the user to
//       be able to select various perf testing options.
//-----------------------------------------------------------------------------
VOID DisplayMenus( CXBFont* pFont )
{
    // Draw the menus
    for( DWORD i=0; i<1000; i++ )
    {
        if( NULL == g_Menus[i].strLabel )
            break;

        // Show the selected menu in a different color
        DWORD dwColor = 0xff00ff00;
        if( i == g_dwSelectedMenu )
            dwColor = 0xffffffff;

        // Draw the text
        pFont->DrawText( 310.0f, 133.0f+i*25.0f, dwColor, g_Menus[i].strLabel, XBFONT_RIGHT );
        pFont->DrawText( 330.0f, 133.0f+i*25.0f, dwColor, g_Menus[i].strOptions[g_Menus[i].dwSelection] );
    }
}




//-----------------------------------------------------------------------------
// Name: StartPerfTest()
// Desc: Starts the performance testing by setting the m_FrameRateIntervalTime
//       to a non-zero amount in milliseconds. Note that this "typical" way to
//       do this is simply by modifying the global D3D::g_PerfCounters directly
//       in a debugger watch window.
//-----------------------------------------------------------------------------
VOID StartPerfTest()
{
    // Get perf test options from the in-app menu selections
    if( g_Menus[MENU_TESTDURATION].dwSelection == MENU_TESTDURATION_1SECOND )
        g_fPerfTestTime = 1.0f;
    if( g_Menus[MENU_TESTDURATION].dwSelection == MENU_TESTDURATION_2SECONDS )
        g_fPerfTestTime = 2.0f;
    if( g_Menus[MENU_TESTDURATION].dwSelection == MENU_TESTDURATION_5SECONDS )
        g_fPerfTestTime = 5.0f;

    if( g_Menus[MENU_OUTPUTMETHOD].dwSelection == MENU_OUTPUTMETHOD_ONSCREEN )
        g_bDumpToScreen = TRUE;
    else 
        g_bDumpToScreen = FALSE;

    if( g_Menus[MENU_PERFPROFILER].dwSelection == MENU_PERFPROFILER_ENABLED )
        g_bEnablePerfProfiler = TRUE;
    else 
        g_bEnablePerfProfiler = FALSE;

    // Start the perf test by setting the FrameRateIntervalTime
    D3DPERF_Reset();
    D3DPERF* pPerfCounters   = D3DPERF_GetStatistics();
    pPerfCounters->m_pfnDumpFrameRateInfoHandler = (PFNDumpFrameRateInfoHandler)PerfTestCallback;
    pPerfCounters->m_FrameRateIntervalTime       = (DWORD)g_fPerfTestTime*1000;
    if( g_bEnablePerfProfiler )
        D3DPERF_StartPerfProfile();

    g_bPerfTestRunning = TRUE;
}




//-----------------------------------------------------------------------------
// Name: StopPerfTest()
// Desc: Stops the performance testing by setting the m_FrameRateIntervalTime
//       to zero. Note that this "typical" way to do this is simply by
//       modifying the global D3D::g_PerfCounters directly in a debugger watch
//       window.
//-----------------------------------------------------------------------------
VOID StopPerfTest()
{
    // Access the perf counters
    D3DPERF* pPerfCounters = D3DPERF_GetStatistics();

    // Turn off the perf test by setting the FrameRateIntervalTime to zero
    pPerfCounters->m_FrameRateIntervalTime = 0;
    if( g_bEnablePerfProfiler )
        D3DPERF_StopPerfProfile();

    g_bPerfTestRunning = FALSE;
}




//-----------------------------------------------------------------------------
// Name: GetPerfStatistics()
// Desc: Extracts perfromance statistics from the API and stores them in
//       external variables. This code is only used by the optional callback
//       functions which display the results of the perf testing. (The callback
//       functions are optional because the internal perf testing API, by
//       default, dumps similiar information to the debug port.)
//-----------------------------------------------------------------------------
VOID GetPerfStatistics()
{
    // Access the perf counters
    D3DPERF* pPerfCounters = D3DPERF_GetStatistics();

    // Get time (in microseconds), number of frames, and framerate
    g_fTime       = (FLOAT)(ReadTimeStampCounter() - pPerfCounters->m_TSCLastResetVal) / 733.0f;
    g_dwNumFrames = pPerfCounters->m_APICounters[API_D3DDEVICE_PRESENT];
    g_fFPS        = 1000000.0f * g_dwNumFrames / g_fTime;

    // Get pushbuffer and vertex stats for frames
    D3DPUSHBUFFERINFO PushBufferInfo;
    D3DPERF_GetPushBufferInfo( &PushBufferInfo );
    g_fPushBufferBytes = ((FLOAT)PushBufferInfo.PushBufferBytesWritten)/g_dwNumFrames;
    g_fVertices        = ((FLOAT)pPerfCounters->m_PerformanceCounters[PERF_VERTICES].Count)/g_dwNumFrames;

    // Calc avg, min and max frame times
    g_fMinFrameTime = (FLOAT)(pPerfCounters->m_FrameTimeMin.Cycles)/733000.0f;
    g_fAvgFrameTime = 0.001f * g_fTime / g_dwNumFrames;
    g_fMaxFrameTime = (FLOAT)(pPerfCounters->m_FrameTimeMax.Cycles)/733000.0f;

    // Get profile results
    g_dwNumProfileSamples  = pPerfCounters->m_ProfileSamples;
    g_dwNumGPUSamples      = pPerfCounters->m_ProfileBusyCounts[0];
    g_dwNumFrontEndSamples = pPerfCounters->m_ProfileBusyCounts[1];
    g_dwNumBackEndSamples  = pPerfCounters->m_ProfileBusyCounts[2];

    g_dwObjectLockWaits      = pPerfCounters->m_PerformanceCounters[PERF_OBJECTLOCK_WAITS].Count;
    g_fObjectLockWaitTime    = (FLOAT)(pPerfCounters->m_PerformanceCounters[PERF_OBJECTLOCK_WAITS].Cycles) / 733.0f;
    g_fObjectLockTimePerWait = g_fObjectLockWaitTime / g_dwObjectLockWaits;

    g_dwPushBufferWaits      = pPerfCounters->m_PerformanceCounters[PERF_PUSHBUFFER_WAITS].Count;
    g_fPushBufferWaitTime    = (FLOAT)(pPerfCounters->m_PerformanceCounters[PERF_PUSHBUFFER_WAITS].Cycles) / 733.0f;
    g_fPushBufferTimePerWait = g_fPushBufferWaitTime / g_dwPushBufferWaits;
    
    g_dwPresentWaits         = pPerfCounters->m_PerformanceCounters[PERF_PRESENT_WAITS].Count;
    g_fPresentWaitTime       = (FLOAT)(pPerfCounters->m_PerformanceCounters[PERF_PRESENT_WAITS].Cycles) / 733.0f;
    g_fPresentTimePerWait    = g_fPresentWaitTime / g_dwPresentWaits;
}




//-----------------------------------------------------------------------------
// Name: PerfTestCallback()
// Desc: This function is called by the D3D8Perf API. We set this callback
//       function by setting the m_pfnDumpFrameRateInfoHandler member in the
//       D3DPERF structure. If we don't set this member, then the internal
//       performance testing functions will simply dump information repeatedly
//       to the debug port, which is adequate for most uses of perf testing.
//       The only reason we supply a callback is to demonstrate it in case an
//       app would want to customize the formatting of the results, or display
//       the information on screen.
//-----------------------------------------------------------------------------
VOID PerfTestCallback()
{
    // Get the perf stats
    GetPerfStatistics();

    // Turn off the perf test. We could let the performance testing run
    // indefinately or, as we use it here, treat it as a one-shot deal.
    StopPerfTest();

    if( g_bDumpToScreen )
    {
        // Flag the app to display the perf results on the screen
        g_bDisplayPerfResults = TRUE;
    }
    else
    {
        // Do an immediate dump of the perf results to the debug port
        DumpPerfResults();
    }
}




//-----------------------------------------------------------------------------
// Name: DumpPerfResults()
// Desc: Dumps the perf results to the debug port. This function mimics the
//       default behavior of how the perftest internal functions will dump info
//       to the debug port. We mimic it here to demonstate the use of the user-
//       supplied callback function.
//-----------------------------------------------------------------------------
VOID DumpPerfResults()
{
    // Display header
    CHAR strBuffer[200];
    sprintf( strBuffer, "\nPERFORMANCE TEST RESULTS\n"
                        "---------------------------------------------\n" );
    OUTPUT_DEBUG_STRING( strBuffer );
    
    // Display test stats
    sprintf( strBuffer, "Test stats:\n" );
    OUTPUT_DEBUG_STRING( strBuffer );
    sprintf( strBuffer, "   Test ran for %1.3f ms\n", g_fTime*0.001f );
    OUTPUT_DEBUG_STRING( strBuffer );
    sprintf( strBuffer, "   Test covered %ld frames\n", g_dwNumFrames );
    OUTPUT_DEBUG_STRING( strBuffer );
    sprintf( strBuffer, "   %1.0f bytes/frame written to pushbuffer\n", g_fPushBufferBytes );
    OUTPUT_DEBUG_STRING( strBuffer );
    sprintf( strBuffer, "   %1.0f vertices/frame\n", g_fVertices );
    OUTPUT_DEBUG_STRING( strBuffer );
    OUTPUT_DEBUG_STRING( "\n" );

    // Output avg, min and max frame times
    sprintf( strBuffer, "Frame stats:\n"
                        "   Frame rate was %1.2f fps\n"
                        "   Min frame time was %1.3f ms\n"
                        "   Avg frame time was %1.3f ms\n"
                        "   Max frame time was %1.3f ms\n",
                        g_fFPS,
                        g_fMinFrameTime,
                        g_fAvgFrameTime,
                        g_fMaxFrameTime );
    OUTPUT_DEBUG_STRING( strBuffer );

    // Dump profile results
    if( g_bEnablePerfProfiler && g_dwNumProfileSamples )
    {
        sprintf( strBuffer, "Profiling:\n" );
        OUTPUT_DEBUG_STRING( strBuffer );
        sprintf( strBuffer, "   %ld samples for GPU (%ld%% busy)\n", 
                            g_dwNumGPUSamples,
                            g_dwNumGPUSamples * 100 / g_dwNumProfileSamples );
        OUTPUT_DEBUG_STRING( strBuffer );
        sprintf( strBuffer, "   %ld samples for vertex processing (%ld%% busy)\n", 
                            g_dwNumFrontEndSamples,
                            g_dwNumFrontEndSamples * 100 / g_dwNumProfileSamples );
        OUTPUT_DEBUG_STRING( strBuffer );
        sprintf( strBuffer, "   %ld samples for rasterization (%ld%% busy)\n",
                            g_dwNumBackEndSamples,
                            g_dwNumBackEndSamples * 100 / g_dwNumProfileSamples );
        OUTPUT_DEBUG_STRING( strBuffer );
        sprintf( strBuffer, "   %ld total samples\n", g_dwNumProfileSamples );
        OUTPUT_DEBUG_STRING( strBuffer );
    }

    OUTPUT_DEBUG_STRING( "\n" );

    // Output wait info
    sprintf( strBuffer, "Wait info:\n" );
    OUTPUT_DEBUG_STRING( strBuffer );

    // Output number of times we waited while objects were begin locked
    if( g_dwObjectLockWaits )
    {
        sprintf( strBuffer, "   %ld Object lock waits took %1.3f ms (%1.1f waits/frame at %1.3f ms/frame)\n",
                            g_dwObjectLockWaits, g_fObjectLockWaitTime*0.001f, (FLOAT)(g_dwObjectLockWaits)/g_dwNumFrames, 
                            g_fObjectLockWaitTime*0.001f/g_dwNumFrames );
        OUTPUT_DEBUG_STRING( strBuffer );
    }

    // Output number of times we waited for the pushbuffer
    if( g_dwPushBufferWaits )
    {
        sprintf( strBuffer, "   %ld Pushbuffer waits took %1.3f ms (%1.1f waits/frame at %1.3f ms/frame)\n",
                            g_dwPushBufferWaits, g_fPushBufferWaitTime*0.001f, (FLOAT)(g_dwPushBufferWaits)/g_dwNumFrames,
                            g_fPushBufferWaitTime*0.001f/g_dwNumFrames );
        OUTPUT_DEBUG_STRING( strBuffer );
    }

    // Output number of times we waited to present
    if( g_dwPresentWaits )
    {
        sprintf( strBuffer, "   Present waits took %5.2f%% of the total time (%1.1f waits/frame taking %1.3f ms/frame)\n",
                            100*g_fPresentWaitTime/g_fTime, (FLOAT)(g_dwPresentWaits)/g_dwNumFrames,
                            g_fPresentWaitTime*0.001f/g_dwNumFrames );
        OUTPUT_DEBUG_STRING( strBuffer );
    }

    if( g_dwObjectLockWaits==0 && g_dwPushBufferWaits==0 && g_dwPresentWaits==0 )
    {
        sprintf( strBuffer, "   No waits occurred.\n" );
        OUTPUT_DEBUG_STRING( strBuffer );
    }

    OUTPUT_DEBUG_STRING( "\n" );

}




//-----------------------------------------------------------------------------
// Name: DisplayPerfResults()
// Desc: Displays the perf results on the screen. This function mimics the
//       default behavior of how the perftest internal functions will dump info
//       to the debug port. We mimic it here to demonstate the use of the user-
//       supplied callback function.
//-----------------------------------------------------------------------------
VOID DisplayPerfResults( CXBFont* pFont )
{
    // Access the perf counters
    WCHAR strBuffer[200];
    FLOAT sx =  64;
    FLOAT sy = 133;

    // Display test stats
    swprintf( strBuffer, L"Test ran for %1.3f ms, covering %ld frames\n", g_fTime*0.001f, g_dwNumFrames );
    pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
    swprintf( strBuffer, L"Frame rate was %1.2f fps:\n"
                         L"   Min frame time was %1.3f ms\n"
                         L"   Avg frame time was %1.3f ms\n"
                         L"   Max frame time was %1.3f ms\n",
                         g_fFPS,
                         g_fMinFrameTime,
                         g_fAvgFrameTime,
                         g_fMaxFrameTime );
    pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=95;

    // Dump profile results
    if( g_bEnablePerfProfiler && g_dwNumProfileSamples )
    {
        swprintf( strBuffer, L"Profiling (%ld samples total):\n", g_dwNumProfileSamples );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
        swprintf( strBuffer, L"   %ld samples for GPU (%ld%% busy)\n", 
                             g_dwNumGPUSamples,
                             g_dwNumGPUSamples * 100 / g_dwNumProfileSamples );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
        swprintf( strBuffer, L"   %ld samples for vertex processing (%ld%% busy)\n", 
                             g_dwNumFrontEndSamples,
                             g_dwNumFrontEndSamples * 100 / g_dwNumProfileSamples );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
        swprintf( strBuffer, L"   %ld samples for rasterization (%ld%% busy)\n",
                             g_dwNumBackEndSamples,
                             g_dwNumBackEndSamples * 100 / g_dwNumProfileSamples );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=38;
    }

    // Output wait info
    swprintf( strBuffer, L"Wait info:\n" );
    pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;

    // Output number of times we waited while objects were begin locked
    if( g_dwObjectLockWaits )
    {
        swprintf( strBuffer, L"   Object lock waits took %5.2f%% of the total time (%1.1f waits/frame taking %1.3f ms/frame)\n",
                             100*g_fObjectLockWaitTime/g_fTime, (FLOAT)(g_dwObjectLockWaits)/g_dwNumFrames,
                             g_fObjectLockWaitTime*0.001f/g_dwNumFrames );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
    }

    // Output number of times we waited for the pushbuffer
    if( g_dwPushBufferWaits )
    {
        swprintf( strBuffer, L"   Pushbuffer waits took %5.2f%% of the total time (%1.1f waits/frame taking %1.3f ms/frame)\n",
                             100*g_fPushBufferWaitTime/g_fTime, (FLOAT)(g_dwPushBufferWaits)/g_dwNumFrames,
                             g_fPushBufferWaitTime*0.001f/g_dwNumFrames );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
    }

    // Output number of times we waited to present
    if( g_dwPresentWaits )
    {
        swprintf( strBuffer, L"   %1.1f Present waits/frame took %5.2f%% of the total time\n",
                             (FLOAT)(g_dwPresentWaits)/g_dwNumFrames, 100*g_fPresentWaitTime/g_fTime );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
    }

    if( g_dwObjectLockWaits==0 && g_dwPushBufferWaits==0 && g_dwPresentWaits==0 )
    {
        swprintf( strBuffer, L"   No waits occurred.\n" );
        pFont->DrawText( sx, sy, 0xffffffff, strBuffer ); sy+=19;
    }
}




