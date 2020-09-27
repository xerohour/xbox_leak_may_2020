///////////////////////////////////////////////////////////////////////////////                                                                  /////////////////////////////////////////////////////////////////////////////
// rawstrng.cpp
//
// email	date		change
// jimgries	3/30/95     created
//
// copyright 1995 Microsoft

// Implementation of the CRawListString class.
#include "stdafx.h"
#include "rawstrng.h"

#define new DEBUG_NEW

CRawListString::CRawListString() : CString()
{
    m_nLen = 0;
    m_pStartPos = 0;
    m_bParsed = FALSE;
}

CRawListString::CRawListString(const CString &str) : CString(str)
{
    m_nLen = GetLength();
    m_pStartPos = GetBuffer(m_nLen);
    m_bParsed = FALSE;
}

CRawListString::~CRawListString()
{
    if (m_bParsed)
    {
        // Convert all null's back into spaces.
        for (int i = 0; i < m_nLen; i++)
            if (m_pStartPos[i] == '\0')
                m_pStartPos[i] = ' ';
    }

    ReleaseBuffer();
}

void CRawListString::Reset()
{
    if (m_bParsed)
    {
        // Convert all null's back into spaces.
        for (int i = 0; i < m_nLen; i++)
            if (m_pStartPos[i] == '\0')
                m_pStartPos[i] = ' ';

        ReleaseBuffer();
        m_pStartPos = GetBuffer(m_nLen);

        m_bParsed = FALSE;
    }
}

CRawListString::operator LPCSTR() const
{
    return m_pchData;
}

CRawListString& operator>>(CRawListString& rls, int& x)
{
    if (rls.m_bParsed == FALSE)
    {
        rls.m_nLen = rls.GetLength();
        rls.m_pStartPos = rls.GetBuffer(rls.m_nLen);
    }

    x =  atoi(strtok(rls.m_bParsed? NULL: rls.m_pStartPos, " \t"));
    rls.m_bParsed = TRUE;
    return rls;
}

CRawListString& operator>>(CRawListString& rls, CString& str)
{
    if (rls.m_bParsed == FALSE)
    {
        rls.m_nLen = rls.GetLength();
        rls.m_pStartPos = rls.GetBuffer(rls.m_nLen);
    }

    str = strtok(rls.m_bParsed? NULL: rls.m_pStartPos, " \t");
    rls.m_bParsed = TRUE;
    return rls;
}
