// SpanSet.h - Implementation of IVsTextSpanSet
//
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//
//================================================================
// Created March 9 1999, Paul Chase Dempsey [paulde]
//
#pragma once
#include "textfind.h"
#include "textspan.h"

class CVsTextSpanSet :   
    public IVsTextSpanSet,
    public IVsTextImageEvents,
    public IVsTextLinesEvents
{
public:
    CVsTextSpanSet();
    ~CVsTextSpanSet();

    // IUnknown
    DECLARE_STD_UNKNOWN

    STDMETHOD (AttachTextImage)(IUnknown * pText);
    STDMETHOD (Detach         )(void);

    STDMETHOD (SuspendTracking)(void); // stop tracking without unadvising change events
    STDMETHOD (ResumeTracking )(void); // resume tracking

    STDMETHOD (Add          )(LONG celt, const TextSpan * pSpan);
    STDMETHOD (GetCount     )(LONG * pcelt);
    STDMETHOD (GetAt        )(LONG iEl, TextSpan * pSpan);
    STDMETHOD (RemoveAll    )(void); 
    STDMETHOD (Sort         )(DWORD SortOptions);
    STDMETHOD (AddFromEnum  )(IVsEnumTextSpans *pEnum);

    // IVsTextImageEvents
    STDMETHOD_(void, OnTextChange) (TextAddress taStart, TextAddress taEnd, TextAddress taNewEnd);

    // IVsTextLinesEvents
    STDMETHOD_(void, OnChangeLineText)(const TextLineChange *pTextLineChange, BOOL fLast);
    STDMETHOD_(void, OnChangeLineAttributes)(long iFirstLine, long iLastLine);

    HRESULT     AttachSpans (LONG celt, TextSpan * pSpans);

private:
    ULONG                       m_cRef;
    CTextSpan *                 m_pSpans;
    int                         m_cel;
    BOOL                        m_cTracking;
    DWORD                       m_dwCookie;
    IVsTextImage *              m_pTextImage;
    IConnectionPoint *          m_pTextLinesCP;

    HRESULT SinkTextImageEvents (void);
    HRESULT SinkTextLinesEvents (IUnknown * pText);
};


