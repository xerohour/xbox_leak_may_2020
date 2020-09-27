/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    green.cpp

Abstract:

    This module implements routine to display green background on TV

--*/

#include "greenp.h"

CXBoxVideo g_Video;

void __cdecl main()
{
    g_Video.Initialize(640, 480);
    g_Video.ClearScreen(COLOR_MEDIUM_GREEN);
    g_Video.ShowScreen();
    Sleep(INFINITE);
}
