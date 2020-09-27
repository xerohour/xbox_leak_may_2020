#include "xkdctest.h"

#include <time.h>

#include "accounts.h"

#include "vector.h"
#include "dlist.h"
#include "config.h"
#include "kerberrorcompare.h"

#include "xktest.h"
#include "xktxonlinelogon.h"
#include "xktxmacscreate.h"


extern XKTVerbose g_XKTVerbose;

/*
void MakeTacticsTestCase(CTestData &testdata, Config &cfg, char *szTestName, BOOL fSuccess)
{
    testdata.m_nTestCase = cfg.getInt(szTestName, "number");
    testdata.m_nConfig = cfg.getInt("logging", "machinetype");
    testdata.m_nMethod = cfg.getInt("logging", "testmethod");
    testdata.m_nMilestone = cfg.getInt("logging", "milestone");
    testdata.m_nType = cfg.getInt("logging", "testtype");

    char *sz = cfg.getStr("logging", "build");
    if (sz)
    {
        memcpy(testdata.m_szBuild, sz, strlen(sz));
    }

    sz = cfg.getStr("logging", "tacticscomment");
    if (sz)
    {
        memcpy(testdata.m_szComments, sz, strlen(sz));
    }

    testdata.m_nStatus = fSuccess ? 1 : 2;
}
*/


HRESULT RunTestFile(IN CXoTest *xo, IN Vector<XKTest*> &vctTests, IN Config &cfg, IN char *szTestSectionName, IN Vector<char*> &vctszTestsToRun, OUT Vector<char*> &vctszFailedTests, OUT INT &nTestsRun)
{
    HRESULT hr = S_OK;
    
    Vector<char*> vctSections;
    cfg.getSections(&vctSections);

    BOOL fFilterTests = vctszTestsToRun.GetCount() > 0;
    
    // Loop through all of the sections and run the test that corresponds
    for (INT j = 0; j < vctSections.GetCount(); j++)
    {
        BOOL fInTestsToRunList = !fFilterTests;

        for (INT l = 0; l < vctszTestsToRun.GetCount(); l++)
        {
            if (!_stricmp(vctSections[j], vctszTestsToRun[l]))
            {
                char *szT = vctszTestsToRun[l];
                vctszTestsToRun.Remove(l);
                delete szT;
                fInTestsToRunList = TRUE;
            }
        }

        if (fInTestsToRunList)
        {
            // Run all tests that start with "test_"
            if (!_strnicmp(vctSections[j], "test_", 5))
            {
                char *szTestType = cfg.getStr(vctSections[j], "test");
                if (!szTestType)
                {
                    TestMsg(XKTVerbose_Status, "Can not run %s because it does not have a test type.\n", vctSections[j]);
                }
                else
                {
                    // Loop through all of the tests to find the one to execute now
                    // This is inefficient, but it's not worth improving now
                    for (INT k = 0; k < vctTests.GetCount(); k++)
                    {
                        HRESULT hrT = S_OK;

                        char szTestName[256];
                        vctTests[k]->getName(szTestName);
                        // Check to see if this is the test we are to run
                        if (!_stricmp(szTestType, szTestName))
                        {
/*
                            CTestData testdata;
*/
                            TestMsg(XKTVerbose_Status, "Running %s...\n", vctSections[j]);

                            // Run the test
                            if (SUCCEEDED(hrT = vctTests[k]->runTest(xo, vctSections[j], cfg)))
                            {
                                TestMsg(XKTVerbose_Status, "<succeeded>\n\n");
/*
                                MakeTacticsTestCase(testdata, cfg, vctSections[j], TRUE);
*/
                            }
                            else
                            {
                                char *szFailureMsg = cfg.getStr(vctSections[j], "failuremsg");
                                if (szFailureMsg)
                                {
                                    TestMsg(XKTVerbose_Status, "<failed> - %s\n\n", szFailureMsg);
                                }
                                else
                                {
                                    TestMsg(XKTVerbose_Status, "<failed>\n\n");
                                }
/*
                                MakeTacticsTestCase(testdata, cfg, vctSections[j], FALSE);
*/
                                char *szFailedTest = new char[strlen(szTestSectionName) + strlen(vctSections[j]) + 3];
                                strcpy(szFailedTest, szTestSectionName);
                                if (strlen(szTestSectionName) > 0)
                                {
                                    strcat(szFailedTest, "::");
                                }
                                strcat(szFailedTest, vctSections[j]);
                                vctszFailedTests.Add(szFailedTest);

                                hr = hrT;
                            }

                            nTestsRun++;
/*
                            if (testdata.m_nTestCase != 0 && cfg.getBool("logging", "logtotactics"))
                            {
                                char *sz = cfg.getStr("logging", "tester");
                                WCHAR wcs[100];
                                if (!sz || !MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, sz, strlen(sz), wcs, 100))
                                {
                                    printf("MultiByteToWideChar failed.\n");
                                    ASSERT(FALSE);
                                    hrFinal = E_FAIL;
                                }
                                else
                                {
                                    if(pTLDBRoutines && FAILED(hrFinal = pTLDBRoutines->AddTestResult(&testdata, wcs)))
                                    {
                                        printf("TacticsLog failed to add test case result for case: %d.  Error code %x.\n", testdata.m_nTestCase, hr);
                                    }

                                }
                            }
*/
                        }
                    }
                }
            }
        }
    }

    return hr;
}

HRESULT RunTests(IN CXoTest *xo, IN Vector<XKTest*> &vctTests, IN Config &cfg, IN Vector<char*> &vctszTestsToRun, OUT Vector<char*> &vctszFailedTests, OUT INT &nTestsRun)
{
    HRESULT hr = S_OK;
    HRESULT hrFinal = S_OK;
    nTestsRun = 0;

    char *szVerbose = cfg.getStr("general", "verbose");

    if (!szVerbose)
    {
        g_XKTVerbose = XKTVerbose_All;
    }
    else
    {
        if (!_stricmp(szVerbose, "none"))
        {
            g_XKTVerbose = XKTVerbose_None;
        }
        else if (!_stricmp(szVerbose, "status"))
        {
            g_XKTVerbose = XKTVerbose_Status;
        }
        else if (!_stricmp(szVerbose, "process"))
        {
            g_XKTVerbose = XKTVerbose_Process;
        }
        else
        {
            g_XKTVerbose = XKTVerbose_All;
        }
    }

/*
    // TacticsLog
    CTLDBRoutines *pTLDBRoutines = NULL;

    if (cfg.getBool("logging", "logtotactics"))
    {
        pTLDBRoutines = new CTLDBRoutines();
        if (pTLDBRoutines)
        {
            hrFinal = pTLDBRoutines->Init();
        }
        else
        {
            hrFinal = E_FAIL;
        }
    }
*/

    if (FAILED(hrFinal))
    {
        printf("TacticsLog failed to initialize.  Error code %x.\n", hrFinal);
    }
    else
    {
        Vector<char*> vctszTestFiles;
        cfg.getEntries("tests", &vctszTestFiles);

        if (vctszTestFiles.GetCount() == 0)
        {
            hrFinal = RunTestFile(xo, vctTests, cfg, "", vctszTestsToRun, vctszFailedTests, nTestsRun);
        }

        // Loop through all of the keys in the tests section and run all tests in the indicated files
        for (INT i = 0; i < vctszTestFiles.GetCount(); i++)
        {
            Config cfgTestFile;
            cfgTestFile.initialize(cfg.getStr("tests", vctszTestFiles[i]));
            cfgTestFile.chainConfig(&cfg);

            HRESULT hr = RunTestFile(xo, vctTests, cfgTestFile, vctszTestFiles[i], vctszTestsToRun, vctszFailedTests, nTestsRun);
            if (FAILED(hr))
            {
                hrFinal = hr;
            }
        }
    }

    return hrFinal;
}


HRESULT ParseTestsToRun(DList<char*> &dlstszArgs, Vector<char*> &vctszTestsToRun)
{
    HRESULT hr = S_FALSE;

    dlstszArgs.CurReset();
    while(!dlstszArgs.CurIsAtEnd())
    {
        char *szT;
        BOOL f = dlstszArgs.CurGetData(szT);
        if (!f)
        {
            hr = E_FAIL;
            break;
        }
        else
        {
            if (!_stricmp(szT, "-r") || !_stricmp(szT, "/r"))
            {
                f = dlstszArgs.CurNext() && dlstszArgs.CurGetData(szT);
                if (!f)
                {
                    // -r comes at the end of the argument list, which is illegal
                    hr = E_FAIL;
                    break;
                }
                else
                {
                    vctszTestsToRun.Add(_strdup(szT));
                    if (dlstszArgs.CurPrev() && dlstszArgs.CurRemove() && dlstszArgs.CurRemove())
                    {
                        hr = S_OK;
                    }
                    else
                    {
                        hr = E_FAIL;
                        break;
                    }
                }
            }
            else
            {
                if (!dlstszArgs.CurNext())
                {
                    hr = E_FAIL;
                    break;
                }
            }
        }
    }

    return hr;
}


HRESULT VerifyConfig(Config &cfg)
{
    HRESULT hr = S_OK;

    Vector<char*> vctszSections;
    cfg.getSections(&vctszSections);
    for (INT i = 0; i < vctszSections.GetCount(); i++)
    {
        if (!strcmp(vctszSections[i], "general"))
        {
            break;
        }
    }
    if (i == vctszSections.GetCount())
    {
        TestMsg(XKTVerbose_Status, "There is no \"general\" section in the config file.\n");
        hr = E_INVALIDARG;
    }

    return hr;
}


HRESULT LoadArgsIntoDList(INT argc, char **argv, DList<char*> &dlstszArgs)
{
    HRESULT hr = S_OK;

    Assert(argc && argv);

    for (INT i = 1; i < argc; i++)
    {
        if (!dlstszArgs.PushBack(argv[i]))
        {
            hr = E_FAIL;
            break;
        }
    }

    return hr;
}


INT __cdecl main(INT argc, char **argv)
{
    HRESULT hr = E_FAIL;

//    LARGE_INTEGER qwTime;
//    NtQuerySystemTime(&qwTime);
//    qwTime.QuadPart += 60;
//    NtSetSystemTime(&qwTime, NULL);
//    return 0;

    DList<char*> dlstszArgs;
    hr = LoadArgsIntoDList(argc, argv, dlstszArgs);

    dlstszArgs.CurReset();
    char *szT;
    BOOL f = dlstszArgs.CurGetData(szT);
    if (f && (!_stricmp(szT, "-hash") || !_stricmp(szT, "/hash")))
    {
        f = dlstszArgs.CurNext() && dlstszArgs.CurGetData(szT);
        if (f)
        {
            HashAndPrintKeyHex(szT);
            hr = S_OK;
        }
    }

    if (f)
    {
        f = dlstszArgs.CurGetData(szT);
        if (f && (!_stricmp(szT, "-?") || !_stricmp(szT, "/?")))
        {
            printf("usage: %s <testfile>\n", argv[0]);
            hr = E_INVALIDARG;
        }
    }

    if (f)
    {
        Vector<char*> vctszTestsToRun;
        hr = ParseTestsToRun(dlstszArgs, vctszTestsToRun);

        if (SUCCEEDED(hr))
        {
            srand(time(NULL));
    
            Config cfg;
            f = dlstszArgs.PopFront(szT);

            if (!f || FAILED(hr = cfg.initialize(szT)))
            {
                printf("Not able to open file \"%s\".\n\n", szT);
                printf("usage: %s <testfile>\n", argv[0]);
            }
            else
            {
                // Do some simple verification on config file
                if (FAILED(VerifyConfig(cfg)))
                {
                    printf("\n");
                    printf("usage: %s <testfile>\n", argv[0]);
                }
                else
                {
                    CXoTest xon(cfg.getStr("general", "xboxname"));

                    if (SUCCEEDED(xon.XOnlineStartup(NULL)))
                    {
                        XNADDR xnaddr;

                        DWORD dwT = 0;
                        //Wait for DHCP to succeed

                        do
                        {
                            dwT = xon.XNetGetTitleXnAddr(&xnaddr);
                            Sleep(100);
                        } 
                        while (dwT == XNET_GET_XNADDR_PENDING);

                        Vector<XKTest*> vctTests;

                        vctTests.Add(new XKTXOnlineLogon());
                        vctTests.Add(new XKTXmacsCreate());

                        INT nTestsRun;
                        Vector<char*> vctszFailedTests;

                        if ((hr = RunTests(&xon, vctTests, cfg, vctszTestsToRun, vctszFailedTests, nTestsRun)) == S_OK)
                        {
                            TestMsg(XKTVerbose_Status, "\n");
                            if (nTestsRun == 1)
                            {
                                TestMsg(XKTVerbose_Status, " ===== Test succeeded =====\n");
                            }
                            else
                            {
                                TestMsg(XKTVerbose_Status, " ===== All %d tests succeeded =====\n", nTestsRun);
                            }
                        }
                        else
                        {
                            if (nTestsRun > 0)
                            {
                                TestMsg(XKTVerbose_Status, "\n");
                                TestMsg(XKTVerbose_Status, " ************************\n");
                                if (nTestsRun == 1)
                                {
                                    TestMsg(XKTVerbose_Status, " *    Test failed:\n");
                                }
                                else
                                {
                                    TestMsg(XKTVerbose_Status, " *    %d/%d tests failed:\n", vctszFailedTests.GetCount(), nTestsRun);
                                }
                                TestMsg(XKTVerbose_Status, " *\n");
                                for (INT i = 0; i < vctszFailedTests.GetCount(); i++)
                                {
                                    TestMsg(XKTVerbose_Status, " * %s\n", vctszFailedTests[i]);
                                }
                                TestMsg(XKTVerbose_Status, " *\n");
                                TestMsg(XKTVerbose_Status, " ************************\n");
                            }
                        }

                        vctTests.DeleteAll();
                        vctszFailedTests.DeleteAll();

                        hr = S_OK;

                        xon.XOnlineCleanup();
                    }
                }
            }
        }

        if (vctszTestsToRun.GetCount() > 0)
        {
            TestMsg(XKTVerbose_Status, "Test(s) specified on command line not found.\n");
            vctszTestsToRun.DeleteAll();
        }
    }

    dlstszArgs.DeleteAll();

    return SUCCEEDED(hr) ? 0 : 1;
}
