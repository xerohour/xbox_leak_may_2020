/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  status.h

Abstract:

  This module defines private data for xnetstressstatus.exe

Author:

  Steven Kehrli (steveke) 8-Feb-2002

------------------------------------------------------------------------------*/

#pragma once



// Structures

typedef struct _CLIENT_OBJECT {
    IN_ADDR  LocalAddr;
    IN_ADDR  RemoteAddr;
    DWORD    dwStressType;
    DWORD    dwStatus;
} CLIENT_OBJECT, *PCLIENT_OBJECT;



// Values

#define MAX_IP_LENGTH         16
#define STATUS_CLASS_NAME     L"XNetStressStatusClassName"



// Dialog messages

#define UM_DIALOG_INITIALIZE  (WM_USER + 1)
#define UM_DIALOG_ENABLE      (WM_USER + 2)
#define UM_DIALOG_ADD         (WM_USER + 3)
#define UM_DIALOG_DELETE      (WM_USER + 4)



// The following enum is used to identify the column indexes

enum eColumnIndex {
    eColumnLocalName = 0,
    eColumnRemoteName,
    eColumnStressType,
    eColumnStatus,
    eColumnIllegal
};



// The following enum is used to identify the sort method

enum eSortMethod {
    eSortLocalNameAscending = 0,
    eSortLocalNameDescending,
    eSortRemoteNameAscending,
    eSortRemoteNameDescending,
    eSortStressTypeAscending,
    eSortStressTypeDescending,
    eSortStatusAscending,
    eSortStatusDescending,
    eSortIllegal
};
