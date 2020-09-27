/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2000 Microsoft Corporation

Module Name:

  main.c

Abstract:

  This module synchronizes test machines

Author:

  Steven Kehrli (steveke) 17-Jul-2000

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#include <xtl.h>



int WINAPI NetsyncMain (HINSTANCE, HINSTANCE, LPWSTR, int);

void __cdecl main()
{
    NetsyncMain(NULL, NULL, NULL, 0);
}
