//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xstring.h
//
//  Description:    XString class header file.
//
//  History:
//      01/02/98    CongpaY     Created
//
//**************************************************************************

#ifndef _XSTRING_H_
#define _XSTRING_H_

class XString
{
    CHAR *m_szData;

public:
    XString(const CHAR *szSrc = NULL);

    ~XString();

    const XString& operator=(const CHAR *szSrc);

    operator const CHAR *() const { return m_szData; }
};

#define xstricmp _stricmp
#define xstrcmp strcmp
#define xstrlen strlen
CHAR *xstrdup(const CHAR *s);

#endif // _XSTRING_H_
