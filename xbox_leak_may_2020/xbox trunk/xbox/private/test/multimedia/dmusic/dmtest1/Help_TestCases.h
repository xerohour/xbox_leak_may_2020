#pragma once

typedef HRESULT (*TESTPROC)(CtIDirectMusicPerformance*, DWORD, DWORD);

struct TESTCASE
{
    LPSTR szTestName;
    LPSTR szTestDesc;
    TESTPROC pTestProc;
    DWORD dwParam1;
    DWORD dwParam2;
};

extern const TESTCASE g_TestCases[];
extern DWORD g_dwNumTestCases;

HRESULT SanityCheckTestCases(void);
