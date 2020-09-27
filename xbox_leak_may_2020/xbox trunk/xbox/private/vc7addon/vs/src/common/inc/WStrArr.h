// WStrArr.h - WCHAR CPtrArray type and helpers

#pragma once
#include "util.h"
#include "vsmem.h"
#include "awconv.h"

typedef CPtrArray<WCHAR> CWStrArray;
typedef int (__cdecl *WSTRARRCMP)(WCHAR **p1, WCHAR **p2);

inline void     CWStrArray_SortNoCase (CWStrArray & ar) { ar.Sort((WSTRARRCMP)QCompareNoCase); }
inline void     CWStrArray_SortCase   (CWStrArray & ar) { ar.Sort((WSTRARRCMP)QCompareCase); }
inline int      CWStrArray_FindCase   (CWStrArray & ar, PCWSTR sz) { return ar.BSearch(sz, (WSTRARRCMP)BCompareCase); }
inline int      CWStrArray_FindNoCase (CWStrArray & ar, PCWSTR sz) { return ar.BSearch(sz, (WSTRARRCMP)BCompareNoCase); }
inline HRESULT  CWStrArray_Add        (CWStrArray & ar, PCWSTR sz)
{
    PWSTR p;
    HRESULT hr = StrDupW(sz, &p);
    if (FAILED(hr)) return hr;
    if (ar.Add(p))
        return S_OK;
    else 
    {
        VSFree(p);
        return E_OUTOFMEMORY;
    }
}
inline void     CWStrArray_Empty      (CWStrArray & ar)
{
    for (int i = 0; i < ar.Count(); i++)
        VSFree(ar.GetAt(i));
    ar.RemoveAll();
}
