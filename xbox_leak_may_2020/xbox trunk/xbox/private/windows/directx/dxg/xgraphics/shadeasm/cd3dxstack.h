///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       stacks.h
//  Content:    Stacks
//
///////////////////////////////////////////////////////////////////////////

namespace XGRAPHICS {

//----------------------------------------------------------------------------
// XAsmCD3DXDwStack
//----------------------------------------------------------------------------


class XAsmCD3DXDwStack
{
    DWORD *m_pdw;

    UINT m_cdw;
    UINT m_cdwLim;

    HRESULT m_hr;

public:
    XAsmCD3DXDwStack();
    ~XAsmCD3DXDwStack();

    HRESULT Push(DWORD dw);
    HRESULT Pop (DWORD *pdw);

    HRESULT GetLastError();
};


//----------------------------------------------------------------------------
// XAsmCD3DXSzStack
//----------------------------------------------------------------------------

class XAsmCD3DXSzStack
{
    char **m_ppsz;
    UINT m_cpsz;
    UINT m_cpszLim;
    HRESULT m_hr;

public:
    XAsmCD3DXSzStack();
    ~XAsmCD3DXSzStack();

    HRESULT Push(char *psz);
    HRESULT Pop (char **ppsz);

    HRESULT GetLastError();
};

} // namespace XGRAPHICS