/////////////////////////////////////////////////////////////////////////////
// rawstrng.h
//
// email	date		change
// jimgries	3/30/95     created
//
// copyright 1995 Microsoft

// Interface of the CRawListString class.

// Derive privately because we don't want people using this kind of
// item as a string.
class CRawListString : private CString
{                                                               
    int m_nLen;
    char* m_pStartPos;
    BOOL m_bParsed;

public:
    CRawListString();
    CRawListString(const CString& str);
    virtual ~CRawListString();

    void Reset();
    operator LPCSTR() const;

    friend CRawListString& operator>>(CRawListString&, int&);
    friend CRawListString& operator>>(CRawListString&, CString&);
    friend class CSuiteDoc;
};

CRawListString& operator>>(CRawListString&, int&);
CRawListString& operator>>(CRawListString&, CString&);

