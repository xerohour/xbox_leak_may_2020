/*++

Copyright (C) 1999 Microsoft Corporation

Module Name:

    AnsiBstr.cpp

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

--*/
#include "stdafx.h"
#include "ansibstr.h"

/*++

Routine Description:

    AnsiBstr::AnsiBstr
    Default Constructor

Arguments:

    none

Return Value:

    none

Notes:

--*/
AnsiBstr::AnsiBstr()
    {
    astr = NULL;
    bstr = NULL;
    }

/*++

Routine Description:

    AnsiBstr::AnsiBstr
    Constructor with a BSTR input

Arguments:

    BSTR b - pointer to a null terminated wide char string

Return Value:

    none

Notes:

--*/
AnsiBstr::AnsiBstr(BSTR b)
    {
    astr = CreateAnsiFromBstr(b);
    bstr = CreateBstrFromAnsi(astr);
    }

/*++

Routine Description:

    AnsiBstr::AnsiBstr
    Constructor with a char* input

Arguments:

    PCHAR a - pointer to a null terminated char string

Return Value:

    none

Notes:

--*/
AnsiBstr::AnsiBstr(PCHAR a)
    {
    bstr = CreateBstrFromAnsi(a);
    astr = CreateAnsiFromBstr(bstr);
    }

/*++

Routine Description:

    AnsiBstr::~AnsiBstr
    Destructor

Arguments:

    none

Return Value:

    none

Notes:

--*/
AnsiBstr::~AnsiBstr()
    {
    if(astr) LocalFree(astr);
    if(bstr) SysFreeString(bstr);
    }

/*++

Routine Description:

    AnsiBstr::SetStr
    Changes the values of the strings (deletes old ones if necessary)

Arguments:

    BSTR b - pointer to a null terminated wide char string

Return Value:

    none

Notes:

--*/
void AnsiBstr::SetStr(BSTR b)
    {
    if(astr) LocalFree(astr);
    if(bstr) SysFreeString(bstr);
    astr = CreateAnsiFromBstr(b);
    bstr = CreateBstrFromAnsi(astr);
    }

/*++

Routine Description:

    AnsiBstr::SetStr
    Changes the values of the strings (deletes old ones if necessary)

Arguments:

    PCHAR a - pointer to a null terminated char string

Return Value:

    none

Notes:

--*/
void AnsiBstr::SetStr(PCHAR a)
    {
    if(astr) LocalFree(astr);
    if(bstr) SysFreeString(bstr);
    bstr = CreateBstrFromAnsi(a);
    astr = CreateAnsiFromBstr(bstr);
    }


/*++

Routine Description:

    CreateAnsiFromBstr
    Allocates memory and converts from a BSTR

Arguments:

    BSTR bstr - pointer to a null terminated wide char string

Return Value:

    PCHAR - pointer to the newly allocated string

Notes:

--*/
PCHAR CreateAnsiFromBstr(BSTR bstr)
    {
    if(!bstr) return NULL;

    UINT bstrLen, pszLen;
    PCHAR psz = NULL;

    bstrLen = wcslen(bstr);
    pszLen = bstrLen * sizeof(CHAR);

    psz = (PCHAR)LocalAlloc(LPTR, pszLen + sizeof(CHAR));

    if(!psz) return NULL;

    WideCharToMultiByte(CP_ACP, 0, bstr, bstrLen, psz, pszLen + sizeof(CHAR), NULL, NULL);

    return psz;
    }

/*++

Routine Description:

    CreateBstrFromAnsi
    Allocates memory and converts from a PCHAR

Arguments:

    PCHAR pszText - pointer to a null terminated char string

Return Value:

    BSTR - return pointer to the new string

Notes:

--*/
BSTR CreateBstrFromAnsi(PCHAR pszText)
    {
    if(!pszText) return NULL;

    int TextLen, pszTextLen;

    pszTextLen = strlen(pszText);
    TextLen = pszTextLen * sizeof(WCHAR);

    BSTR pbstr = SysAllocStringByteLen(NULL, TextLen);

    if(!pbstr) return NULL;

    MultiByteToWideChar(CP_ACP, 0, pszText, pszTextLen, pbstr, TextLen + sizeof(WCHAR));

    return pbstr;
    }
