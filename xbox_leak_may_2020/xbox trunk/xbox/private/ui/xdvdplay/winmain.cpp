//----------------------------------------------------------------------------
//
// Implement WinMain()
//
// Copyright (C) Microsoft Corporation, 1999.
//
//----------------------------------------------------------------------------

#include "pch.cpp"
#pragma hdrstop

#define APP_NAME   "XBox DVD Player"

LRESULT CALLBACK
StaticWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    XDvdPlayer *pPlayer;

    pPlayer = (XDvdPlayer *)GetWindowLong(hWnd, GWLP_USERDATA);

    if (pPlayer)
    {
        return pPlayer->WndProc(hWnd, uMessage, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, uMessage, wParam, lParam);
    }
}

int WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR pCmdLine,
        int iCmdShow)
{
    if (!hPrevInstance)
    {
        WNDCLASS wc;

        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = StaticWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = NULL;
        wc.hCursor = NULL;
        wc.hbrBackground = (HBRUSH) (1 + COLOR_WINDOW);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = APP_NAME;

        RegisterClass(&wc);
    }

    int iScreenWidth, iScreenHeight;
    iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hWnd;
    hWnd = CreateWindow(
        APP_NAME,
        APP_NAME,
        WS_POPUP,
        0,
        0,
        0,
        0,
        NULL,
        NULL,
        hInstance,
        NULL);

    XDvdPlayer player;
    if (FAILED(player.Initialize(hWnd)))
    {
        DestroyWindow(hWnd);
        return 0;
    }

    SetWindowLong(hWnd, GWL_USERDATA, (LONG)&player);
    ShowWindow(hWnd, iCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
