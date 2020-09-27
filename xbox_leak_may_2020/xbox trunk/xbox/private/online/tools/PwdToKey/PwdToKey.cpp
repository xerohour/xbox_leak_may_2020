//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "md4.h"

BOOL
KerbPasswordToKey(
    IN LPSTR pszPassword,
    OUT PUCHAR Key
    );

void PrintUsage()
{
    printf("Usage: PwdToKey password\n");
}

extern "C"
int
_cdecl
main(
    int argc,
    char** argv
    )
{
    int i;
    BYTE key[MD4_LEN];
    
    if (argc != 2)
    {
        PrintUsage();
        return 1;
    }

    if (!KerbPasswordToKey( argv[1], key ))
    {
        printf("PwdToKey: error in KerbPasswordToKey\n");
        return 1;
    }

    printf("PwdToKey: key is 0x");
    for (i=0; i<MD4_LEN; ++i)
    {
        printf("%02X", key[i]);
    }
    printf("\n");
    
    printf("key is \"");
    for (i=0; i<MD4_LEN; ++i)
    {
        printf("\\x%02X", key[i]);
    }
    printf("\"\n");
    
    return 0;
}
