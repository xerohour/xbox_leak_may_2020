/* Copyright (c) 1998 Microsoft Corporation */
/*
 * @Doc DMusic16
 *
 * @module 
 * TimerWnd.C -
 *
 * Create and manage user mode timer callbacks based on a window callback |
 *
 */
#pragma warning(disable:4704)       /* Inline assembly */

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmddk.h>

#include "dmusic16.h"
#include "debug.h"

STATIC VOID FAR PASCAL __loadds TimerTask(DWORD dwInstance);
STATIC BOOL NEAR PASCAL CreateTimerWindow(VOID);
LRESULT CALLBACK __loadds timerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

STATIC SZCODE gszTimerWindowClass[] = "DM16TW";
STATIC HTASK  ghtaskTimer;
STATIC HWND   ghwndTimer;
STATIC UINT   guTimerID;

/* Timer window functions called by message crackers
 */
BOOL NEAR PASCAL  Timer_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
VOID NEAR PASCAL  Timer_OnTimer(HWND hwnd, UINT id);
VOID NEAR PASCAL  Timer_OnClose(HWND hwnd);

/* @func
 *
 * @comm
 */
BOOL PASCAL
CreateTimerTask(
    VOID)
{
    UINT rc;
    /* Already up and running?
     */
    if (IsWindow(ghwndTimer))
    {
        return TRUE;
    }

    /* In some undefined state?
     */
    if (IsTask(ghtaskTimer))
    {
        DPF(0, "CreateTimerTask: In some weird state; bailing");
        return FALSE;
    }

    /* The task is not already running. Create it.
     */
    ghwndTimer = NULL;
    ghtaskTimer = NULL;
    rc = mmTaskCreate(TimerTask, NULL, 0); 
    if (rc)
    {
        DPF(0, "CreateTimerTask: mmTaskCreate() -> %u", rc);
        return FALSE;
    }

    /* Give away CPU to other tasks until the timer task starts up.
     * It will set ghtaskTimer to non-zero when it is up or if it fails.
     */
    while (!ghtaskTimer)
    {
        Yield();
    }

    /* Did it fail?
     */
    if (ghtaskTimer != (HTASK)-1)
    {
        return TRUE;
    }

    DPF(0, "CreateTimerTask: Task startup failed.");

    ghwndTimer = NULL;
    ghtaskTimer = NULL;

    DPF(1, "CreateTimerTask: htask %.4X hwnd %.4x",
        (WORD)ghtaskTimer,
        (WORD)ghwndTimer);
    
    /* Something went wrong and the task terminated.
     */
    return FALSE;
}

/* @func
 *
 * @comm
 */
VOID PASCAL
DestroyTimerTask(
    VOID)
{
    if (!IsWindow(ghwndTimer))
    {
        /* Nothing to kill.
         */
        return;
    }

    //FORWARD_WM_CLOSE(ghwndTimer, PostMessage);
    SendMessage(ghwndTimer, WM_CLOSE, (WPARAM)0, (LPARAM)0);

    /* Wait for task to terminate
     */
    DPF(2, "About to wait for MMTASK(%04X) to go away...", (WORD)ghtaskTimer);
    while (IsTask(ghtaskTimer))
    {
        Yield();
    }
    DPF(2, "Done waiting for MMTASK to go away...");
    
    ghwndTimer = NULL;
}

/* @func
 *
 * @comm
 */
STATIC VOID FAR PASCAL __loadds
TimerTask(
    DWORD dwInstance)
{
    MSG msg;

    /* Create the hidden callback window
     */
    if (!CreateTimerWindow())
    {
        ghtaskTimer = (HTASK)-1;
        return;
    }

    /* On success, set our task handle and tell the system we're a
     * service process
     */
    ghtaskTimer = mmGetCurrentTask();
//    RegisterServiceProcess(NULL, RSP_SIMPLE_SERVICE);

    /* The ever-present message loop
     */
    while (GetMessage(&msg, NULL, 0, 0))
    {
        DispatchMessage(&msg);
    }
    DPF(2, "Out of MMTASK!");

    /* Someome threw us a WM_CLOSE.
     */
//   RegisterServiceProcess(NULL, RSP_UNREGISTER_SERVICE);
//    UnregisterClass(gszTimerWindowClass, ghInst);

//    WaitEvent(ghtaskTimer);
}

STATIC BOOL NEAR PASCAL
CreateTimerWindow(
    VOID)
{
    WNDCLASS cls;

    cls.hCursor        = NULL;
    cls.hIcon          = NULL;
    cls.lpszMenuName   = NULL;
    cls.lpszClassName  = gszTimerWindowClass;
    cls.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    cls.hInstance      = ghInst;
    cls.style          = CS_GLOBALCLASS;
    cls.lpfnWndProc    = (WNDPROC)timerWndProc;
    cls.cbWndExtra     = 0;
    cls.cbClsExtra     = 0;

    if (RegisterClass(&cls))
    {
        ghwndTimer = CreateWindowEx(0, gszTimerWindowClass, NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL, ghInst, NULL);
        if (ghwndTimer)
        {
            DPF(1, "Timer Window (%.4Xh)", ghwndTimer);
            return TRUE;
        }
    }

    UnregisterClass(gszTimerWindowClass, ghInst);

    return FALSE;
}

LRESULT CALLBACK __loadds
timerWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch(msg)
    {
        HANDLE_MSG(hwnd, WM_CREATE,         Timer_OnCreate);
        HANDLE_MSG(hwnd, WM_TIMER,          Timer_OnTimer);
        HANDLE_MSG(hwnd, WM_CLOSE,          Timer_OnClose);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL NEAR PASCAL
Timer_OnCreate(
    HWND hwnd,
    LPCREATESTRUCT lpCreateStruct)
{
    DPF(2, "Timer_OnCreate");
//    guTimerID = SetTimer(hwnd, 1, MS_USERMODE, NULL);

    return /*(BOOL)(guTimerID != 0);*/ TRUE; 
}

VOID NEAR PASCAL
Timer_OnTimer(
    HWND hwnd,
    UINT id)
{
    MidiInRefillFreeLists();
}


VOID NEAR PASCAL
Timer_OnClose(
    HWND hwnd)
{
    DPF(2,"Timer_OnClose");
//    KillTimer(hwnd, guTimerID);
    DestroyWindow(hwnd);
    PostQuitMessage(0);
}

