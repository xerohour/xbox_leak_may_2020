/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    main.cpp

Description:

    Direct3D Build Verification Tests.

*******************************************************************************/

#include <xtl.h>

extern void                 ExecuteTests(HINSTANCE hInstance, HANDLE hLog);

//******************************************************************************
// WinMain
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     WinMain
//
// Description:
//
//     Entry point for the application.
//
// Arguments:
//
//     HINSTANCE hInstance      - Application instance handle
//
//     HINSTANCE hPrevInstance  - Previous instance of the application (always
//                                NULL for Win32 applications)
//
//     LPSTR szCmdLine          - Pointer to a string specifying the command
//                                line used in launching the application
//                                (excluding the program name)
//
//     int nCmdShow             - State specifying how the window is to be 
//                                displayed on creation
//
// Return Value:
//
//     0 on success, -1 on failure.
//
//******************************************************************************
#ifndef UNDER_XBOX
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow) {
    ExecuteTests(hInstance, INVALID_HANDLE_VALUE);
    return 0;
}
#else
void __cdecl main() {
    ExecuteTests(NULL, INVALID_HANDLE_VALUE);
}
#endif // UNDER_XBOX
