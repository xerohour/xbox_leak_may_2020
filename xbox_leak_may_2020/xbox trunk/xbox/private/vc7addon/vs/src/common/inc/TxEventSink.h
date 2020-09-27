// TextEventSink.h - template classes for text event proxies
//  IVsTextImageEvents
//  IVsTextLinesEvents
//
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//
//================================================================
// Created March 9 1999, Paul Chase Dempsey [paulde]
//
#pragma once

//------------------------------------------------------------------------------
// class CTextLinesEventSink
//
// Declare ptr like this:
//    CComObject<CTextLinesEventSink<CMyClass> > *  pTextLinesEventSink = NULL;
//
// Create instance and initialize it like this:
//     hr = CComObject<CTextLinesEventSink<CMyClass> >::CreateInstance (&pTextLinesEventSink);
//     if (SUCCEEDED(hr))
//         SetObject (this); // CMyClass*
//
//------------------------------------------------------------------------------
template <class T> class ATL_NO_VTABLE CTextLinesEventSink :
    public CComObjectRoot,
    public IVsTextLinesEvents
{
private:
    T * m_pObj;       // Delegation target

public:
    CTextLinesEventSink  () : m_pObj(NULL) {}
    ~CTextLinesEventSink () {}

    // After creating, set the delegation target
    VOID SetObject (T * pObj) { m_pObj = pObj; }

    BEGIN_COM_MAP(CTextLinesEventSink)
        COM_INTERFACE_ENTRY(IVsTextLinesEvents)
    END_COM_MAP()

    // IVsTextLinesEvents methods
    STDMETHOD_(void, OnChangeLineText)(const TextLineChange *pTextLineChange, BOOL fLast)
        { m_pObj->OnChangeLineText (pTextLineChange, fLast); }

    STDMETHOD_(void, OnChangeLineAttributes)(long iFirstLine, long iLastLine)
        { m_pObj->OnChangeLineAttributes (iFirstLine, iLastLine); }
};

//------------------------------------------------------------------------------
// class CTextImageEventSink
//------------------------------------------------------------------------------
template <class T> class ATL_NO_VTABLE CTextImageEventSink :
    public CComObjectRoot,
    public IVsTextImageEvents
{
private:
    T * m_pObj;       // Owner

public:
    VOID SetObject (T * pObj) { m_pObj = pObj; }

    BEGIN_COM_MAP(CTextImageEventSink)
        COM_INTERFACE_ENTRY(IVsTextImageEvents)
    END_COM_MAP()

    // IVsTextImageEvents methods
    STDMETHOD_(void, OnTextChange) (TextAddress taStart, TextAddress taEnd, TextAddress taNewEnd)
    {
        m_pObj->OnTextChange (taStart, taEnd, taNewEnd);
    }
};
