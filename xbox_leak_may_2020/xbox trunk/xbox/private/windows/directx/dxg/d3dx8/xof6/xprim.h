//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xprim.h
//
//  Description:    XPrimitive and XPrimitiveList classes.
//
//  History:
//      01/08/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XPRIM_H_
#define _XPRIM_H_

typedef enum _XPrimType {
    X_Word,             // 16 bits (unsigned short)
    X_DWord,            // 32 bits (unsigned long)
    X_Float,            // 32 bits (float)
    X_Double,           // 64 bits (double)
    X_Char,             // 8 bits (char)
    X_UChar,            // 8 bits (unsigned char)
    X_SWord,            // (short)
    X_SDWord,           // (long)
    X_Lpstr,            // Null terminated string
    X_Cstring,          // C string
    X_Unicode,          // Unicode string
    X_ULongLong         // 64 bit int
} XPrimType;

class XPrimitive {
public:
    LPCSTR   name;
    XPrimType   type;
    DWORD       size;
};

const XPrimitive *XPrimitiveFromName(LPCSTR szName);

#endif // _XPRIM_H_
