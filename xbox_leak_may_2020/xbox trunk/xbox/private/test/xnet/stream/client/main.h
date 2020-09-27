/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  main.h

Abstract:

  This module contains the definitions for main.c

Author:

  Steven Kehrli (steveke) 13-Feb-2002

------------------------------------------------------------------------------*/

#pragma once



namespace XNetStreamNamespace {



// Function prototypes

VOID
WINAPI
StartTest(
    IN HANDLE  hLog
);

VOID
WINAPI
EndTest(
);

} // namespace XNetStreamNamespace
