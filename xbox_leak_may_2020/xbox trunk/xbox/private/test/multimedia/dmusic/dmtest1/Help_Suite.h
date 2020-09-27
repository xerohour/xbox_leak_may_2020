#pragma once

#include "globals.h"
#include "help_testcases.h"


class SUITE
{   
public:
    SUITE(void);
    ~SUITE(void);
    HRESULT Init(LPCSTR cszSectionName, const TESTCASE *pTestCases, DWORD dwTestArraySize);
    HRESULT Load(void);
    void RunTestArray     (void);
    HRESULT SetTestParams (TESTPARAMS &TestParams);
    HRESULT GetTestParams (TESTPARAMS *pTestParams);
    HRESULT GetTestResults(DWORD *pdwPassed, DWORD *pdwFailed);
    HRESULT GetSuiteName  (CHAR *pszSuiteName);


private:

    //methods
    HRESULT Log                 (void);
    void EnableAllTestsOfType   (LPSTR szType);
    void AddOmitTestCase        (LPSTR szTestName, BOOL bAdd);
    HRESULT RunTestCase            (CtIDirectMusicPerformance8 *ptPerf8, TESTCASE TestCase);

    //variables
    BOOL m_bInitialized;
    TESTPARAMS m_TestParams;
    BOOL *m_pbRunTest;    
    
    CHAR m_szSuiteName[MAX_PATH];
    const TESTCASE *m_pTestCases;
    DWORD m_dwTestArraySize;
    SUITE *m_pSuite;

    DWORD m_dwPassed;
    DWORD m_dwFailed;
};

