/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    AnsiBstr.cpp

Abstract:

    

Author:

    Josh Poley (jpoley) 1-1-1999

Revision History:

*****************************************************************************/
#include "stdafx.h"
#include "ansibstr.h"

/*****************************************************************************

Routine Description:

    AnsiBstr::AnsiBstr
    Default Constructor

Arguments:

    none

Return Value:

    none

Notes:

*****************************************************************************/
AnsiBstr::AnsiBstr()
    {
    astr = NULL;
    bstr = NULL;
    }

/*****************************************************************************

Routine Description:

    AnsiBstr::AnsiBstr
    Constructor with a BSTR input

Arguments:

    BSTR b - pointer to a null terminated wide char string

Return Value:

    none

Notes:

*****************************************************************************/
AnsiBstr::AnsiBstr(BSTR b)
    {
    astr = CreateAnsiFromBstr(b);
    bstr = CreateBstrFromAnsi(astr);
    }

/*****************************************************************************

Routine Description:

    AnsiBstr::AnsiBstr
    Constructor with a char* input

Arguments:

    PCHAR a - pointer to a null terminated char string

Return Value:

    none

Notes:

*****************************************************************************/
AnsiBstr::AnsiBstr(PCHAR a)
    {
    bstr = CreateBstrFromAnsi(a);
    astr = CreateAnsiFromBstr(bstr);
    }

/*****************************************************************************

Routine Description:

    AnsiBstr::~AnsiBstr
    Destructor

Arguments:

    none

Return Value:

    none

Notes:

*****************************************************************************/
AnsiBstr::~AnsiBstr()
    {
    if(astr) LocalFree(astr);
    if(bstr) SysFreeString(bstr);
    }

/*****************************************************************************

Routine Description:

    AnsiBstr::SetStr
    Changes the values of the strings (deletes old ones if necessary)

Arguments:

    BSTR b - pointer to a null terminated wide char string

Return Value:

    none

Notes:

*****************************************************************************/
void AnsiBstr::SetStr(BSTR b)
    {
    if(astr) LocalFree(astr);
    if(bstr) SysFreeString(bstr);
    astr = CreateAnsiFromBstr(b);
    bstr = CreateBstrFromAnsi(astr);
    }

/*****************************************************************************

Routine Description:

    AnsiBstr::SetStr
    Changes the values of the strings (deletes old ones if necessary)

Arguments:

    PCHAR a - pointer to a null terminated char string

Return Value:

    none

Notes:

*****************************************************************************/
void AnsiBstr::SetStr(PCHAR a)
    {
    if(astr) LocalFree(astr);
    if(bstr) SysFreeString(bstr);
    bstr = CreateBstrFromAnsi(a);
    astr = CreateAnsiFromBstr(bstr);
    }


/*****************************************************************************

Routine Description:

    CreateAnsiFromBstr
    Allocates memory and converts from a BSTR

Arguments:

    BSTR bstr - pointer to a null terminated wide char string

Return Value:

    PCHAR - pointer to the newly allocated string

Notes:

*****************************************************************************/
PCHAR CreateAnsiFromBstr(BSTR bstr)
    {
    if(!bstr) return NULL;

    size_t bstrLen, pszLen;
    PCHAR psz = NULL;

    bstrLen = wcslen(bstr);
    pszLen = bstrLen * sizeof(CHAR);

    psz = (PCHAR)LocalAlloc(LPTR, pszLen + sizeof(CHAR));

    if(!psz) return NULL;

    WideCharToMultiByte(CP_ACP, 0, bstr, bstrLen, psz, pszLen + sizeof(CHAR), NULL, NULL);

    return psz;
    }

/*****************************************************************************

Routine Description:

    CreateBstrFromAnsi
    Allocates memory and converts from a PCHAR

Arguments:

    PCHAR pszText - pointer to a null terminated char string

Return Value:

    BSTR - return pointer to the new string

Notes:

*****************************************************************************/
BSTR CreateBstrFromAnsi(const char *pszText)
    {
    if(!pszText) return NULL;

    size_t TextLen, pszTextLen;

    pszTextLen = strlen(pszText);
    TextLen = pszTextLen * sizeof(WCHAR);

    BSTR pbstr = SysAllocStringByteLen(NULL, TextLen);

    if(!pbstr) return NULL;

    MultiByteToWideChar(CP_ACP, 0, pszText, pszTextLen, pbstr, TextLen + sizeof(WCHAR));

    return pbstr;
    }


/*****************************************************************************

Routine Description:

    CreateBstrFromAnsi
    Allocates memory and converts from a PCHAR

Arguments:

    PCHAR pszText - pointer to a null terminated char string

Return Value:

    BSTR - return pointer to the new string

Notes:

*****************************************************************************/
BSTR CreateBstrFromAnsi(const char *pszText, size_t length)
    {
    if(!pszText) return NULL;

    size_t TextLen, pszTextLen;

    pszTextLen = length;
    TextLen = pszTextLen * sizeof(WCHAR);

    BSTR pbstr = SysAllocStringByteLen(pszText, TextLen);

    if(!pbstr) return NULL;

    pbstr = (BSTR)Ansi2UnicodeHack((char*)pbstr, length);

    pbstr[length] = '\0';

    return pbstr;
    }


/*****************************************************************************

Routine Description:

    Ansi2UnicodeHack

    In-place Pseudo Ansi to Unicode (char to wide char) conversion.

Arguments:

    IN char* str - char string to convert to wide char string

Return Value:

    char* - pointer to Unicode string

Note:
    
    Because a bunch of Unicode functions expect a char string to be on an
    even boundry, the returned string may be moved 1 character over.

*****************************************************************************/
char* Ansi2UnicodeHack(char *str, size_t length)
    {
    if(!str) return NULL;

    int align = 0;
    int len = length;

    // put string on an even boundry because some freak put a bunch of ASSERTs
    // that check for even boundries in Unicode functions like 
    // RtlEqualUnicodeString()
    if(((unsigned long)str & 1) != 0)
        {
        align = 1;
        }

    for(; len>=0; len--)
        {
        str[len*2+align] = str[len];
        str[len*2+align+1] = '\0';
        }

    str += align;

    return (char*)str;
    }

/*****************************************************************************

Routine Description:

    Ansi2UnicodeHack

    In-place Pseudo Ansi to Unicode (char to wide char) conversion.

Arguments:

    IN char* str - char string to convert to wide char string

Return Value:

    char* - pointer to Unicode string

Note:
    
    Because a bunch of Unicode functions expect a char string to be on an
    even boundry, the returned string may be moved 1 character over.

*****************************************************************************/
char* Ansi2UnicodeHack(char *str)
    {
    if(!str) return NULL;

    int align = 0;
    int len = strlen(str)+1;

    // put string on an even boundry because some freak put a bunch of ASSERTs
    // that check for even boundries in Unicode functions like 
    // RtlEqualUnicodeString()
    if(((unsigned long)str & 1) != 0)
        {
        align = 1;
        }

    for(; len>=0; len--)
        {
        str[len*2+align] = str[len];
        str[len*2+align+1] = '\0';
        }

    str += align;

    return (char*)str;
    }


/*****************************************************************************

Routine Description:

    Unicode2AnsiHack

    In-place Pseudo Unicode to Ansi (wide char to char) conversion.

Arguments:

    IN unsigned short* str - wide char string to convert to char string

Return Value:

    char* - pointer to ANSI string

*****************************************************************************/
char* Unicode2AnsiHack(unsigned short *str)
    {
    if(!str) return NULL;
    char *str2 = (char*)str;

    size_t len = wcslen(str)+1;
    for(size_t i=0; i<len; i++)
        {
        str2[i] = str2[i*2];
        }

    return (char*)str;
    }
