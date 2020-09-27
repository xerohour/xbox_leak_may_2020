//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xstring.cpp
//
//  Description:    Implement XString class.
//
//  History:
//      01/02/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

XString::XString(const CHAR *szSrc)
{
    m_szData = xstrdup(szSrc);
}

XString::~XString()
{
    if (m_szData)
        XFree((void *)m_szData);
}


const XString& XString::operator=(const CHAR *szSrc)
{
    if (m_szData)
        XFree((void *)m_szData);

    m_szData = xstrdup(szSrc);

    return *this;
}

#undef DPF_MODNAME
#define DPF_MODNAME "xstrdup"
CHAR *xstrdup(const CHAR *s)
{
    CHAR *d = NULL;

    if (s) {
        int n = (xstrlen(s) + 1) * sizeof(CHAR);

        if (SUCCEEDED(XMalloc((void **) &d, n)))
            memcpy(d, s, n);
        else
            DPF_ERR("Failed to allocate space for string");
    }

    return d;
}
