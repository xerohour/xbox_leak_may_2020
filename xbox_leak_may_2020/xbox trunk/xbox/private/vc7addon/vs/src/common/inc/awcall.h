// awcall.h - Ansi/Wide calls based on HWND type
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//
// Created October 24, 1999
// Paul Chase Dempsey [paulde]
//-----------------------------------------------------------------
// These are simple wrappers that call the A or W version of 
// selected APIs, based on whether the given HWND is Ansi or
// Unicode (respectively).
//
// WARNING: ARGUMENTS ARE NOT CONVERTED.  These simply match the 
//          API used to the A/W type of the window.  Make sure 
//          that this is what you want.
//
// Messages with Text Parameters:
//
// If you are sending messages with text parameters, make sure that 
// this is the right thing to do for your window type. 
//
// This is the correct for "RichEdit20W" class windows conditionally 
// created as A/W based on the platform (as happens if you use the 
// UniApi or VsWin9x window creation wrappers.
//
// This is NOT correct for "EDIT" class windows and most other windows. 
// Use the UniApi or VsWin9x GetWindowText/SetWindowText for EDIT and 
// other controls.
//
//-----------------------------------------------------------------
#pragma once

#define AWC(hwnd,fn) (IsWindowUnicode(hwnd) ? fn##W : fn##A)

inline LRESULT CallWindowProcAW (WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return AWC(hWnd, CallWindowProc)(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
}

inline LRESULT DefWindowProcAW(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return AWC(hWnd, DefWindowProc)(hWnd, Msg, wParam, lParam);
}

// Use this to preserve the ANSI/Unicode type of a window when you subclass
inline LONG SetWindowLongAW (HWND hWnd, int nIndex, LONG dwNewLong)
{
  return AWC(hWnd, SetWindowLong)(hWnd, nIndex, dwNewLong);
}

inline LONG_PTR SetWindowLongPtrAW(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
  return AWC(hWnd, SetWindowLongPtr)(hWnd, nIndex, dwNewLong);
}

inline LONG_PTR GetWindowLongPtrAW(HWND hWnd, int nIndex)
{
  return AWC(hWnd, GetWindowLongPtr)(hWnd, nIndex);
}

inline LONG_PTR SetClassLongPtrAW(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
  return AWC(hWnd, SetClassLongPtr)(hWnd, nIndex, dwNewLong);
}

inline LONG_PTR GetClassLongPtrAW(HWND hWnd, int nIndex)
{
  return AWC(hWnd, GetClassLongPtr)(hWnd, nIndex);
}

inline LRESULT SendMessageAW (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  return AWC(hWnd, SendMessage)(hWnd, Msg, wParam, lParam);
}

inline BOOL IsDialogMessageAW(HWND hDlg, LPMSG lpMsg)
{
  return AWC(hDlg, IsDialogMessage)(hDlg, lpMsg);
}
