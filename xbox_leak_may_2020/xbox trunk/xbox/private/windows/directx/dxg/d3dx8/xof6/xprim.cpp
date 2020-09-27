//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xprim.cpp
//
//  Description:    Implement XPrimitiveList class.
//
//  History:
//      01/08/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

static const XPrimitive aPrimitives[] = {
    "WORD",      X_Word,      sizeof(unsigned short),
    "DWORD",     X_DWord,     sizeof(unsigned long),
    "FLOAT",     X_Float,     sizeof(float),
    "DOUBLE",    X_Double,    sizeof(double),
    "CHAR",      X_Char,      sizeof(char),
    "UCHAR",     X_UChar,     sizeof(unsigned char),
    "BYTE",      X_UChar,     sizeof(unsigned char),
    "SWORD",     X_SWord,     sizeof(short),
    "SDWORD",    X_SDWord,    sizeof(long),
    "STRING",    X_Lpstr,     sizeof(char *),
    "CSTRING",   X_Cstring,   sizeof(char *),
    "UNICODE",   X_Unicode,   sizeof(short *),
    "ULONGLONG", X_ULongLong, sizeof(__int64)
};

static const cPrimitives = sizeof(aPrimitives)/sizeof(XPrimitive);

const XPrimitive *XPrimitiveFromName(LPCSTR szName)
{
    for (DWORD i = 0; i < cPrimitives; i++) {
        if (!xstricmp(szName, aPrimitives[i].name))
            return &aPrimitives[i];
    }
    return NULL;
}
