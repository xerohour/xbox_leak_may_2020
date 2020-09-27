#include "globals.h"
#include "help_suite.h"
#include "help_suitelist.h"

#define CHECK_SUITELIST_INITIALIZED {                                                       \
    if (!m_bInitialized)                                                                 \
    {                                                                                   \
        Log(ABORTLOGLEVEL, "%s, line %d: Init failed: SUITELIST %08X is not initialized (loaded)!", __FILE__, __LINE__, this);    \
        hr = E_FAIL;                                                                    \
    }                                                                                   \
}


/********************************************************************************
********************************************************************************/
SUITELIST::SUITELIST(void)
{
    m_bInitialized = FALSE;
    m_pSuite = NULL;
    memset(m_szAppName, 0, MAX_PATH);
    m_dwMaxSuites = 0;
};

/********************************************************************************
********************************************************************************/
SUITELIST::~SUITELIST(void)
{
    delete []m_pSuite;
    m_pSuite = NULL;
};




/********************************************************************************
Reads in and loads up a bunch of SUITEs, which will print out their own information.
********************************************************************************/
HRESULT SUITELIST::Load(LPCSTR cszAppName, const TESTCASE *pTestCases, DWORD dwTestArraySize)
{
    HRESULT hr = S_OK;
    LPCSTR cszSuites = "Suites";
    DWORD dwMax = 0;
    CHAR *szTemp = NULL;
    DWORD i = 0;

    CHAR *szFullSectionKey  = NULL;
    CHAR *szSectionData     = NULL;

    szFullSectionKey        = new CHAR[MAX_PATH];
    CHECKALLOC(szFullSectionKey);
    szSectionData           = new CHAR[MAX_PROFILESECTION_SIZE];
    CHECKALLOC(szSectionData   );

    if (SUCCEEDED(hr))
    {
        m_dwMaxSuites = 0;
        strncpy(m_szAppName, cszAppName, MAX_PATH -1);
    }

    //Load up the list of suites into szSectionData.
    if (SUCCEEDED(hr))
    {
        sprintf(szFullSectionKey, "%s_%s", cszAppName, cszSuites);
        dwMax = GetProfileSection(szFullSectionKey, szSectionData, MAX_PROFILESECTION_SIZE);
        if (dwMax == MAX_PROFILESECTION_SIZE  - 2)
        {
            Log(ABORTLOGLEVEL, "ERROR: Filled up entire %d characters, need to increase MAX_PROFILESECTION_SIZE ", MAX_PROFILESECTION_SIZE);
            hr = E_FAIL;
        }
    }

    //If we didn't find any suites under "DMTEST1_Suites" (or whatever the section key was) then we'll use "DMTEST1" for the suite area, and
    //  (therefore) "DMTest1_TestAdd"/"DMTest1_TestOmit" for the subarea.  This is to support the original bug files.
    if (SUCCEEDED(hr) && 0==dwMax)
    {
        sprintf(szSectionData, "%s", cszAppName);
        CHAR *szEnd = szSectionData;
        while (*szEnd)
            szEnd++;
        *(++szEnd) = NULL;
        dwMax = 1;
    }


    //Get all test cases and attempt to add them in.
    if (SUCCEEDED(hr))
    {
        szTemp = szSectionData;
        if (!szTemp[0])
        {
            Log(ABORTLOGLEVEL, "No Suites found in suite list [%s]", szFullSectionKey);
            hr = E_FAIL;
        }
    }
    
    //Count 'em
    if (SUCCEEDED(hr))
    {
        m_dwMaxSuites = 0;
        for (i=0;;i++)
        {
            if (szTemp[i]==0 && i)
                m_dwMaxSuites++;
            //break on \0\0
            if (szTemp[i]==0 && szTemp[i+1] == 0)
                break;
        }
    }

    //Allocate an array of suites.
    if (SUCCEEDED(hr))
    {
        m_pSuite = new SUITE[m_dwMaxSuites];
        if (!m_pSuite)
        {
            hr = E_FAIL;
        }
    }

    //Now load 'em into the array.
    if (SUCCEEDED(hr))
    {
        DWORD dwSuite = 0;
        CHAR *szLastString;
        szTemp = szSectionData;
        szLastString = szTemp;
        for (i=0; SUCCEEDED(hr);i++)
        {
            if (szTemp[i]==0 && i)
            {
                //The name of our test suite is in szLastString.  Initialize the suite with it.
                CHECKRUN(m_pSuite[dwSuite].Init(szLastString, pTestCases, dwTestArraySize));
                CHECKRUN(m_pSuite[dwSuite].Load());
                if (SUCCEEDED(hr))
                {
                    szLastString = &szTemp[i+1];
                    dwSuite++;
                }
            }

            //break on \0\0
            if (szTemp[i]==0 && szTemp[i+1] == 0)
                break;
        }
    }

    if (SUCCEEDED(hr))
    {
        m_bInitialized = TRUE;
    }
    
    delete []szFullSectionKey;
    delete []szSectionData   ;

    if (FAILED(hr))
    {
        Log(ABORTLOGLEVEL, "Error: Could not load suite %s", cszAppName);
    }
    return hr;

};

/********************************************************************************
********************************************************************************/
HRESULT SUITELIST::RunSuites(DWORD *pdwPassed, DWORD *pdwFailed)
{  
HRESULT hr = S_OK;
DWORD i = 0;
DWORD dwPassed      = 0;
DWORD dwFailed      = 0;
DWORD dwPassedSuite = 0;
DWORD dwFailedSuite = 0;
CHAR szSuiteName[MAX_PATH] = {0};
CHECK_SUITELIST_INITIALIZED

    for (i=0; i<m_dwMaxSuites && SUCCEEDED(hr); i++)
    {
        dwPassedSuite = 0;
        dwFailedSuite = 0;
        ZeroMemory(szSuiteName, MAX_PATH);
        CHECKRUN(m_pSuite[i].GetSuiteName(szSuiteName));
        CHECK(m_pSuite[i].RunTestArray());
        CHECKRUN(m_pSuite[i].GetTestResults(&dwPassedSuite, &dwFailedSuite))
        if (SUCCEEDED(hr))
        {
            Log(ABORTLOGLEVEL, "SUITE: %s", szSuiteName);
            Log(ABORTLOGLEVEL, "---------------------------------------");
            Log(ABORTLOGLEVEL, "PASSED: %d", dwPassedSuite);
            Log(ABORTLOGLEVEL, "FAILED: %d", dwFailedSuite);
            Log(ABORTLOGLEVEL, "---------------------------------------");
            Log(ABORTLOGLEVEL, "");
            Log(ABORTLOGLEVEL, "");
        }
        else
        {
            Log(ABORTLOGLEVEL, "ERROR: Failed running suite #%d (%s)", i, szSuiteName);
        }
    }

    //Now that we're all done running the tests, print out the results again, this time in column form.
        Log(ABORTLOGLEVEL, "Suite                              Passed     Failed");
        Log(ABORTLOGLEVEL, "------------------------------     ------     ------");
        //                  30                            5    6     5    6       
    for (i=0; i<m_dwMaxSuites && SUCCEEDED(hr); i++)
    {
        CHECKRUN(m_pSuite[i].GetSuiteName(szSuiteName));
        CHECKRUN(m_pSuite[i].GetTestResults(&dwPassedSuite, &dwFailedSuite))
        Log(ABORTLOGLEVEL, "%-30s     %6u     %6u", szSuiteName, dwPassedSuite, dwFailedSuite);
        dwPassed += dwPassedSuite;
        dwFailed += dwFailedSuite;
    }

        Log(ABORTLOGLEVEL, "                                                    ");
        Log(ABORTLOGLEVEL, "TOTAL                              %6u     %6u", dwPassed, dwFailed);


    if (pdwPassed)
        *pdwPassed = dwPassed;
    if (pdwFailed)
        *pdwFailed = dwFailed;

    return hr;
};




/********************************************************************************
Purpose:
    Print warnings about potential problems across the suites.  i.e.
    1) There's no reason for suites to have different screen logging settings.
    2) A suite with stress mode on (making it loop forever) prevents subsequent suites from running.


TODO:

Note: This function is bad OOP design because it couples SUITELIST to the specific
      data parts of the SUITE object.  A better solution would be to either aggregate
      or contain (wrap) the SUITE object such that the user of SUITELIST would
      be required to process this information, not SUITELIST itself.  This change
      would also put the results printout code into the user of SUITELIST.
********************************************************************************/
HRESULT SUITELIST::CheckSuites(void)
{
    HRESULT hr = S_OK;
    DWORD i = 0;
    TESTPARAMS TestParams = {0};
    CHAR szSuiteName[MAX_PATH] = {0};
    BOOL bWarnings = FALSE;

    CHECK_SUITELIST_INITIALIZED 

    // 1) There's no reason for suites to have different screen logging settings.
    {
        BOOL bLogToScreen;
        CHAR szSuiteNameFirst[MAX_PATH] = {0};

        for (i=0; i<m_dwMaxSuites && SUCCEEDED(hr); i++)
        {        

            CHECKRUN(m_pSuite[i].GetTestParams(&TestParams));
            CHECKRUN(m_pSuite[i].GetSuiteName(szSuiteName));
            if (SUCCEEDED(hr))
            {
                if (0==i)
                {
                    bLogToScreen = TestParams.bLogToScreen;
                    CHECKRUN(m_pSuite[i].GetSuiteName(szSuiteNameFirst));
                }
                else
                {
                    if (TestParams.bLogToScreen != bLogToScreen)
                    {
                        Log(ABORTLOGLEVEL, "Error: Suite #%d (%s) has bLogToScreen = %s, but suite #%d (%s) has bLogToScreen = %s",
                                            0, szSuiteNameFirst, bLogToScreen ? "TRUE" : "FALSE",
                                            i, szSuiteName     , TestParams.bLogToScreen ? "TRUE" : "FALSE");
                        bWarnings = TRUE;
                    }

                }
            }
        }
    }

    // 2) A suite with stress mode on (making it loop forever) prevents subsequent suites from running.
    for (i=0; i<m_dwMaxSuites-1 && SUCCEEDED(hr); i++)
    {
        CHECKRUN(m_pSuite[i].GetTestParams(&TestParams));
        CHECKRUN(m_pSuite[i].GetSuiteName(szSuiteName));
        if (SUCCEEDED(hr))
        {
            if (TestParams.bStress)
            {
                Log(ABORTLOGLEVEL, "Error: Suite #%d (%s) has bStress = TRUE although suites follow it.", i, szSuiteName);
                bWarnings = TRUE;
            }
        }
    }

    if (bWarnings)
    {
        Log(ABORTLOGLEVEL,"");
        Log(ABORTLOGLEVEL,"");
    }

    return hr;
}