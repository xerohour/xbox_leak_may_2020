///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       stacks.h
//  Content:    Stacks
//
///////////////////////////////////////////////////////////////////////////



//----------------------------------------------------------------------------
// CD3DXDwStack
//----------------------------------------------------------------------------


class CD3DXDwStack
{
    DWORD *m_pdw;

    UINT m_cdw;
    UINT m_cdwLim;

    HRESULT m_hr;

public:
    CD3DXDwStack();
    ~CD3DXDwStack();

    HRESULT Push(DWORD dw);
    HRESULT Pop (DWORD *pdw);

    HRESULT GetLastError();
};


//----------------------------------------------------------------------------
// CD3DXSzStack
//----------------------------------------------------------------------------

class CD3DXSzStack
{
    char **m_ppsz;
    UINT m_cpsz;
    UINT m_cpszLim;
    HRESULT m_hr;

public:
    CD3DXSzStack();
    ~CD3DXSzStack();

    HRESULT Push(char *psz);
    HRESULT Pop (char **ppsz);

    HRESULT GetLastError();
};
