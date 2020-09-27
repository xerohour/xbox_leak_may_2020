#pragma once
#include "globals.h"
#include "help_suite.h"
#include "help_testcases.h"

class SUITELIST
{
public:
    SUITELIST(void);
    ~SUITELIST(void);
    HRESULT Load(LPCSTR cszAppName, const TESTCASE *pTestCases, DWORD dwTestArraySize);
    HRESULT RunSuites(DWORD *pdwPassed, DWORD *pdwFailed);
    HRESULT CheckSuites(void);

private:
    SUITE *m_pSuite;
    BOOL m_bInitialized;
    CHAR m_szAppName[MAX_PATH];
    DWORD m_dwMaxSuites;
};