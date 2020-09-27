/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    Optex.cpp

Abstract:

    This module implements the COptex (optimized mutex) synchronization
    object.

Author:

    Jeffrey M. Richter (v-jeffrr) 01-Aug-1998

--*/

#pragma once

class COptex {
public:
   COptex(LPCSTR pszName,  DWORD dwSpinCount = 4000);
   COptex(LPCWSTR pszName, DWORD dwSpinCount = 4000);
   ~COptex();
   void SetSpinCount(DWORD dwSpinCount);
   void Enter();
   BOOL TryEnter();
   void Leave();

private:
   typedef struct {
      DWORD m_dwSpinCount;
      long  m_lLockCount;
      DWORD m_dwThreadId;
      long  m_lRecurseCount;
   } SHAREDINFO, *PSHAREDINFO;

   BOOL        m_fUniprocessorHost;
   HANDLE      m_hevt;
   HANDLE      m_hfm;
   PSHAREDINFO m_pSharedInfo;

private:
   BOOL CommonConstructor(PVOID pszName, BOOL fUnicode, DWORD dwSpinCount);
};

inline COptex::COptex(LPCSTR pszName, DWORD dwSpinCount)
{
   CommonConstructor((PVOID) pszName, FALSE, dwSpinCount);
}

inline COptex::COptex(LPCWSTR pszName, DWORD dwSpinCount)
{
   CommonConstructor((PVOID) pszName, TRUE, dwSpinCount);
}

