//
//  vcclr.h - description???
//      Copyright (C) 2000-2001 Microsoft Corporation
//      All rights reserved.
//

#if _MSC_VER > 1000
#pragma once
#endif

#if !defined(_INC_VCCLR)
#define _INC_VCCLR

#using <mscorlib.dll>
#include <gcroot.h>
inline const wchar_t __gc* PtrToStringChars(String *s) {
        String __pin* pString = s;
        const wchar_t __gc* psc = (wchar_t*)pString + 6;
        return psc;
}

#endif //_INC_VCCLR
