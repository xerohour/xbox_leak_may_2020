/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    utils.cpp

Abstract:

    This module contains the helper functions used by the API tests

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/
#include "stdafx.h"
#include "utils.h"

#ifdef  __cplusplus
    extern "C" {
    #include "ntos.h"
    }
#else
    #include "ntos.h"
#endif

namespace Nickname {

bool CompareFirstNickname(WCHAR *nickname)
    {
    const unsigned MAXSIZE = MAX_NICKNAME * 2;
    WCHAR *buffer = new WCHAR[MAXSIZE];

    HANDLE hFind = XFindFirstNickname(FALSE, buffer, MAX_NICKNAME);
    if(hFind == INVALID_HANDLE_VALUE)
        {
        delete[] buffer;
        return false;
        }

    XFindClose(hFind);

    if(wcscmp(buffer, nickname) == 0) 
        {
        delete[] buffer;
        return true;
        }

    delete[] buffer;
    return false;
    }

bool RemoveNicknameFile(void)
    {
    OBJECT_STRING nicknameFile;
    OBJECT_ATTRIBUTES Obja;
    NTSTATUS status;
    RtlInitObjectString(&nicknameFile, "\\Device\\Harddisk0\\partition1\\UDATA\\NICKNAME.XBN");
    InitializeObjectAttributes(&Obja, &nicknameFile, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtDeleteFile(&Obja);
    if(!NT_SUCCESS(status)) 
        {
        SetLastError(RtlNtStatusToDosError(status));
        return false;
        }

    return true;
    }

} // namespace Nickname
