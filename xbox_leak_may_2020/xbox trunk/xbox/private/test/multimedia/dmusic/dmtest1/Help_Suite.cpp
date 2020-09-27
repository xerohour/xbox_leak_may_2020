#include "globals.h"
#include "help_suite.h"
#include "help_dowork.h"
#include "help_worsttimes.h"
#include "help_FileIOStress.h"
#include "help_statsmemoryuse.h"

#define CHECK_SUITE_INITIALIZED {                                                            \
    if (!m_bInitialized)                                                                 \
    {                                                                                   \
        ::Log(ABORTLOGLEVEL, "ERROR (%s, line %d): SUITE %08X is not initialized!", __FILE__, __LINE__, this);    \
        hr = E_FAIL;                                                                    \
    }                                                                                   \
}

static BOOL g_bDMusicAlreadyInitialized = FALSE;

/********************************************************************************
********************************************************************************/
SUITE::SUITE(void)
{
    m_bInitialized = FALSE;
};

/********************************************************************************
********************************************************************************/
HRESULT SUITE::Init(LPCSTR cszSectionName, const TESTCASE *pTestCases, DWORD dwTestArraySize)
{
    HRESULT hr = S_OK;
    const CHAR *szAppNamePosition= NULL;
    CHAR szAppNamePrefix[MAX_PATH] = {0};
    ZeroMemory(&m_TestParams, sizeof(m_TestParams));
    m_pbRunTest = NULL;        
    ZeroMemory(m_szSuiteName, sizeof(CHAR) * MAX_PATH);
    m_pTestCases = NULL;
    m_dwTestArraySize = 0;
    m_pSuite = NULL;
    m_dwPassed = 0;
    m_dwFailed = 0;

    //Add an '_'.
    sprintf(szAppNamePrefix, "%s_", g_szAppName);
    
    //Make sure we're not already initialized.
    if (m_bInitialized)
    {   
        ::Log(ABORTLOGLEVEL, "Init failed: SUITE %08X is already initialized!", this);
        hr = E_FAIL;
    }

    //Verification: make sure our name follows the rules:
    //  A) Must contain "DMTEST1_" at the beginning, or
    //  B) Must contain only "DMTEST1";

    if (SUCCEEDED(hr))
    {
        szAppNamePosition = strstr(cszSectionName, szAppNamePrefix);
        if (!szAppNamePosition)
        {
            if (_strcmpi(g_szAppName, cszSectionName))
            {
                ::Log(ABORTLOGLEVEL, "Error: %s was not found in %s", szAppNamePrefix, cszSectionName);
                hr = E_FAIL;
            }
            else
            {
                szAppNamePosition = cszSectionName;
            }
        }
    }
    
    if (SUCCEEDED(hr))
    {
        if (szAppNamePosition != cszSectionName)
        {
            ::Log(ABORTLOGLEVEL, "Error: %s was not found at beginning of %s", szAppNamePrefix, cszSectionName);
            hr = E_FAIL;            
        }
    }

    m_dwTestArraySize = dwTestArraySize;
    m_pTestCases      = pTestCases;
    
    //Allocate and clear the array
    CHECK(m_pbRunTest = new BOOL[m_dwTestArraySize]);
    CHECKALLOC(m_pbRunTest);
    CHECK(ZeroMemory(m_pbRunTest, m_dwTestArraySize * sizeof(BOOL)));

    //Clear out our testparams
    CHECK(ZeroMemory((void*)&m_TestParams, sizeof(m_TestParams)));

    //Save our good name.
    CHECK(strncpy(m_szSuiteName, cszSectionName, MAX_PATH - 1));

    if (SUCCEEDED(hr))
        m_bInitialized = TRUE;

    return hr;
};

/********************************************************************************
********************************************************************************/
SUITE::~SUITE(void)
{    
    if (m_bInitialized)
    {
        if (m_pbRunTest)
        {
            memset(m_pbRunTest, 0xFFFF, m_dwTestArraySize);
            delete []m_pbRunTest;
        }
        m_pbRunTest = NULL;
    }
};


#define SUFFIXES 3

/********************************************************************************
********************************************************************************/
HRESULT SUITE::Load(void)
{
HRESULT hr = S_OK;
CHECK_SUITE_INITIALIZED;

//Local vars and const strings.
DWORD i;
DWORD dwMax = 0;
DWORD dwPurpose = 0;
LPCSTR szCountKey[SUFFIXES] = {"maxtestomit", "maxtestadd", "defaultmediafilesadd"};
LPCSTR szSuffix[SUFFIXES] = {"testomit", "testadd", "defaultmediafiles"};
LPCSTR szDefaultDirectory = "T:\\DMTest1\\Default\\";
LPSTR szTemp = NULL;


//Allocate a temporary string.  If this fails, just bail.
szTemp = new CHAR[MAX_PROFILESECTION_SIZE];
CHECKALLOC(szTemp);
if (FAILED(hr))
    return hr;


//Set up the final names, such as [DMTEST1_SUITE2_TESTADD]
CHAR szSectionNameAddOmit[SUFFIXES][MAX_PATH];
for (i=0; i<SUFFIXES; i++)
{
    sprintf(szSectionNameAddOmit[i], "%s_%s",  m_szSuiteName, szSuffix[i]);
}


    //Read all the main test params.
    m_TestParams.bWait  =           GetProfileInt(m_szSuiteName, "wait",   TRUE);
    m_TestParams.bBVT   =           GetProfileInt(m_szSuiteName, "bvt",    FALSE);
    m_TestParams.bValid =           GetProfileInt(m_szSuiteName, "valid",  FALSE);
    m_TestParams.bInvalid =         GetProfileInt(m_szSuiteName, "invalid",FALSE);
    m_TestParams.bFatal =           GetProfileInt(m_szSuiteName, "fatal",  FALSE);
    m_TestParams.bPerf  =           GetProfileInt(m_szSuiteName, "perf",   FALSE);
    m_TestParams.dwPerfWait=        GetProfileInt(m_szSuiteName, "perfwait",10);
    m_TestParams.bStress=           GetProfileInt(m_szSuiteName, "stress", FALSE);
    m_TestParams.bLogToScreen   =   GetProfileInt(m_szSuiteName, "logtoscreen", FALSE);    
    m_TestParams.dwWaitBetweenTests=GetProfileInt(m_szSuiteName, "waitbetweentests", 0);
    m_TestParams.bWaitAtTestEnd =   GetProfileInt(m_szSuiteName, "waitattestend", FALSE);
    GetProfileString(m_szSuiteName, "initperformance", "Once", m_TestParams.szInitPerformance, szInitPerformance_SIZE);
    m_TestParams.bSkipUserInput    =GetProfileInt(m_szSuiteName, "skipuserinput", FALSE);
    m_TestParams.bSuppressMemInfo  =GetProfileInt(m_szSuiteName, "suppressmeminfo", FALSE);
    m_TestParams.bUseGM_DLS        =GetProfileInt(m_szSuiteName, "usegm_dls", FALSE);
    m_TestParams.dwDebugLevel =     GetProfileInt(m_szSuiteName, "debuglevel", 1);
    m_TestParams.dwRIPLevel =       GetProfileInt(m_szSuiteName, "riplevel", 0);
    m_TestParams.dwLocalLogLevel   =GetProfileInt(m_szSuiteName, "localloglevel", FYILOGLEVEL);
    m_TestParams.dwMLDebugLevel    =GetProfileInt(m_szSuiteName, "MLDebugLevel",       2);
    m_TestParams.bDoWorkLocalThread=GetProfileInt(m_szSuiteName, "doworklocalthread", FALSE);
    m_TestParams.dwDoWorkFrequency =GetProfileInt(m_szSuiteName, "doworkfrequency", 0);
    m_TestParams.dwDMVoiceCount    =GetProfileInt(m_szSuiteName, "dmvoicecount", 128);
    m_TestParams.dwDecreaseMemory=  GetProfileInt(m_szSuiteName, "decreasememory", 0);
    m_TestParams.dwStressFileIODuration  =GetProfileInt(m_szSuiteName, "stressfileioduration", 0);
                                    GetProfileString(m_szSuiteName, "stressfileiothreadpri",    "THREAD_PRIORITY_NORMAL", m_TestParams.szStressFileIOThreadPri, MAX_PATH);
                                    GetProfileString(m_szSuiteName, "DSScratchImage",           "Default",                m_TestParams.szDSScratchImage, MAX_PATH);
                                    GetProfileString(m_szSuiteName, "DSHRTF",                   "FULL",                   m_TestParams.szDSHRTF, MAX_PATH);
                                    GetProfileString(m_szSuiteName, "InitialTestCase",          "",                       m_TestParams.szInitialTestCase, MAX_PATH);

    DWORD dwFixedSizeHeapDefaultNorm = 4 * 1024 * 1024;
    DWORD dwFixedSizeHeapDefaultPhys = 1024 * 1024 / 2;
    LPCSTR szDMDefaultHeap = "Tracking";
    GetProfileString(m_szSuiteName, "DMHeap", szDMDefaultHeap, m_TestParams.szDMHeap, MAX_PATH);
    BOOL bFixedSizeHeap = (_strcmpi(m_TestParams.szDMHeap, "FixedUser") == 0);
    m_TestParams.dwDMFixedHeapSizeNorm  = GetProfileInt(m_szSuiteName, "DMFixedHeapSizeNorm",   bFixedSizeHeap ? dwFixedSizeHeapDefaultNorm  : 0);
    m_TestParams.dwDMFixedHeapSizePhys  = GetProfileInt(m_szSuiteName, "DMFixedHeapSizePhys",   bFixedSizeHeap ? dwFixedSizeHeapDefaultPhys  : 0);

    if (!m_TestParams.bDoWorkLocalThread && m_TestParams.dwDoWorkFrequency)
    {
        ::Log(ABORTLOGLEVEL, "Warning: You're not running a local DoWork() thread but you've set DoWorkFrequency() to %d.  This # will be ignored.", m_TestParams.dwDoWorkFrequency); 
    }
   

    //Print out the test parameters.
    Log();

    //Enable/Disable tests of each type according to parameters.
    if (m_TestParams.bBVT)
        EnableAllTestsOfType("BVT");
    if (m_TestParams.bValid)
        EnableAllTestsOfType("Valid");
    if (m_TestParams.bInvalid)
        EnableAllTestsOfType("Invalid");
    if (m_TestParams.bFatal)
        EnableAllTestsOfType("Fatal");
    if (m_TestParams.bPerf)
        EnableAllTestsOfType("PerfMem");    
    
    
    MLSetErrorLevel(m_TestParams.dwMLDebugLevel);
    DirectMusicSetDebugLevel(m_TestParams.dwDebugLevel, m_TestParams.dwRIPLevel);
    if (m_TestParams.dwDebugLevel < m_TestParams.dwRIPLevel)
        ::Log(ABORTLOGLEVEL, "Warning: Debug level is %d but RIP level is higher (%d).");

    // Initialize DMusic here.  We can only do it once, even if we delete and re-create
    //  the performance object many times.
    if (!g_bDMusicAlreadyInitialized)
    {
        CHECKRUN(CreateHeaps(m_TestParams.szDMHeap, m_TestParams.dwDMFixedHeapSizeNorm, m_TestParams.dwDMFixedHeapSizePhys));
        CHECKRUN(InitializeDMusicWithCreatedHeaps());
        CHECK   (g_bDMusicAlreadyInitialized = TRUE);
    }
    else
    {
        if (_strcmpi(m_TestParams.szDMHeap, szDMDefaultHeap))
        {
            ::Log(ABORTLOGLEVEL, "Error: Specified a heap (%s), but DMusic heaps have already been initialized.", m_TestParams.szDMHeap);
        }        
        if (m_TestParams.dwDMFixedHeapSizeNorm != dwFixedSizeHeapDefaultNorm)
        {
            ::Log(ABORTLOGLEVEL, "Error: Specified a value (%u) DMFixedHeapSizeNorm, but DMusic heaps have already been initialized.", m_TestParams.dwDMFixedHeapSizeNorm);
        }
        if (m_TestParams.dwDMFixedHeapSizePhys != dwFixedSizeHeapDefaultPhys)
        {
            ::Log(ABORTLOGLEVEL, "Error: Specified a value (%u) DMFixedHeapSizePhys, but DMusic heaps have already been initialized.", m_TestParams.dwDMFixedHeapSizePhys);
        }
    }
 
    //Make sure we chose a valid type of HRTF function.
    if (_strcmpi(m_TestParams.szDSHRTF, "full") &&
        _strcmpi(m_TestParams.szDSHRTF, "light")
        )
    {
        ::Log(ABORTLOGLEVEL, "Error: Specified %s for m_TestParams.szDSHRTF, but need to specify ""FULL"" or ""LIGHT""\n", m_TestParams.szDSHRTF);
    }

    if (_strcmpi(m_TestParams.szInitPerformance, "EveryTest") &&
        _strcmpi(m_TestParams.szInitPerformance, "Once") &&
        _strcmpi(m_TestParams.szInitPerformance, "Never")
        )
    {
        ::Log(ABORTLOGLEVEL, "Error: Specified %s for m_TestParams.szInitPerformance, but need to specify ""EveryTest"", ""Once"", or ""Never""\n", m_TestParams.szInitPerformance);
    }

    if (_strcmpi(m_TestParams.szStressFileIOThreadPri, "THREAD_PRIORITY_IDLE") &&
        _strcmpi(m_TestParams.szInitPerformance, "Once") &&
        _strcmpi(m_TestParams.szInitPerformance, "Never")
        )
    {
        ::Log(ABORTLOGLEVEL, "Error: Specified %s for m_TestParams.szInitPerformance, but need to specify ""EveryTest"", ""Once"", or ""Never""\n", m_TestParams.szDSHRTF);
    }

    if //(_strcmpi(m_TestParams.szDMHeap, "Default") &&
        (_strcmpi(m_TestParams.szDMHeap, "Tracking") &&
        _strcmpi(m_TestParams.szDMHeap, "FixedUser")
        //_strcmpi(m_TestParams.szDMHeap, "FixedDefault")

        )
    {
        ::Log(ABORTLOGLEVEL, "Error: Specified %s for m_TestParams.szDMHeap; need to specify ""Tracking"" or ""FixedUser""\n", m_TestParams.szDMHeap);
        hr = E_FAIL;
    }

    if (FAILED(ThreadPriFromString(m_TestParams.szStressFileIOThreadPri, NULL)))
    {
        ::Log(ABORTLOGLEVEL, "Error: Specified invalid string (%s) for m_TestParams.szStressFileIOThreadPri", m_TestParams.szStressFileIOThreadPri);
    }
    
    //Now go through our array of test cases and check for omissions or additions.  If a test has been added or omitted via
    //  the addition of all tests of a certain type (say "valid" tests), this will override that.
    for (dwPurpose = 0; dwPurpose < SUFFIXES; dwPurpose++)
    {
        dwMax = GetProfileInt(m_szSuiteName, szCountKey[dwPurpose], 0);
        CHAR szKey[MAX_PATH] = {0};
        CHAR szTestName[MAX_PATH];
        for (i=0; i<=dwMax; i++)
        {
            sprintf(szKey, "%s%04d", szSuffix[dwPurpose], i);
            GetProfileString(m_szSuiteName, szKey, "NULL", szTestName, MAX_PATH);        
            if (strcmp("NULL", szTestName) == 0)
                continue;
            
            if (dwPurpose < 2)
                AddOmitTestCase(szTestName, dwPurpose);
            else
                {
                    hr = MediaCopyFile(szTestName, szDefaultDirectory, COPY_IF_NEWER);
                    if (FAILED(hr))
                    {
                        ::Log(ABORTLOGLEVEL, "Error: Failed to copy %s to %s (%s)", szTestName, szDefaultDirectory, tdmXlatHRESULT(hr));
                    }
                }
        }
    }

    //Added support for doing this w/o the numbers.
    CHAR *szLastString= NULL;
    for (dwPurpose = 0; dwPurpose < 2; dwPurpose++)
    {
        //dwMax = GetPrivateProfileSectionA(szSectionNameAddOmit[dwPurpose], szTemp, MAX_PROFILESECTION_SIZE , szININame);
        dwMax = GetProfileSection(szSectionNameAddOmit[dwPurpose], szTemp, MAX_PROFILESECTION_SIZE );
        if (dwMax == MAX_PROFILESECTION_SIZE  - 2)
        {
            ::Log(ABORTLOGLEVEL, "ERROR: Filled up entire %d characters, need to increase MAX_PROFILESECTION_SIZE ", MAX_PROFILESECTION_SIZE );
        }

        //Get all test cases and attempt to add them in.
        if (!szTemp[0])
            continue;

        szLastString = szTemp;
        for (i=0;;i++)
        {
            if (szTemp[i]==0 && i)
            {
                                
                if (dwPurpose < 2)
                    AddOmitTestCase(szLastString, dwPurpose);
                else
                {
                    hr = MediaCopyFile(szLastString, szDefaultDirectory, COPY_IF_NEWER);
                    if (FAILED(hr))
                    {
                        ::Log(ABORTLOGLEVEL, "Error: Failed to copy %s to %s (%s)", szLastString, szDefaultDirectory, tdmXlatHRESULT(hr));
                    }
                }

                szLastString = &szTemp[i+1];
            }

            //break on \0\0
            if (szTemp[i]==0 && szTemp[i+1] == 0)
                break;
        }
    }

    //Add two lines of space below list of test cases.
    ::Log(ABORTLOGLEVEL, "");
    ::Log(ABORTLOGLEVEL, "");






    //Dealing with the "default" media is kinda complex:
    //1) Copy the default piece of media to the default directory.
    CHAR szDefaultNetworkMedia[MAX_PATH] = {0};
    CHAR szDefaultMediaName   [MAX_PATH] = {0};
    if (SUCCEEDED(hr))
    {
        GetProfileString(m_szSuiteName, "defaultmedia", "SGT/test.sgt", szDefaultNetworkMedia, MAX_PATH);
        CHECKRUN(MediaCopyFile(szDefaultNetworkMedia, szDefaultDirectory, COPY_IF_NEWER));
    }

    //2) Figure out the file's local path.
    //2a) Figure out the file's name
    if (SUCCEEDED(hr))
    {
        CHECK(ChopPath(szDefaultNetworkMedia, NULL, szDefaultMediaName));    
        ASSERT(szDefaultMediaName[0]);
    }

    //2b) Tack that name onto the local file path.
    CHECK   (sprintf(m_TestParams.szDefaultMedia, "%s%s", szDefaultDirectory, szDefaultMediaName));

    //If we're using "test.sgt" then also copy over MAIN1.DLS.
    if (SUCCEEDED(hr))
    {
        if (strstr(m_TestParams.szDefaultMedia, "test.sgt"))
            CHECKRUN(MediaCopyFile("DLS/Main1.DLS", szDefaultDirectory, COPY_IF_NEWER));
    }
        

    //Free allocations.
    delete[]szTemp;
    return hr;
};


/********************************************************************************
Print out all the test parameters.
********************************************************************************/
HRESULT SUITE::Log(void)
{
    HRESULT hr = S_OK;
    CHECK_SUITE_INITIALIZED;

    ::Log(FYILOGLEVEL, "%s TEST PARAMETERS", m_szSuiteName);
    ::Log(FYILOGLEVEL, "----------------------------------");
    ::Log(FYILOGLEVEL, "bWait            : %s", m_TestParams.bWait  ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bBVT             : %s", m_TestParams.bBVT   ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bValid           : %s", m_TestParams.bValid ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bInvalid         : %s", m_TestParams.bInvalid?"TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bFatal           : %s", m_TestParams.bFatal ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bPerf            : %s", m_TestParams.bPerf  ? "TRUE" : "FALSE");    
    ::Log(FYILOGLEVEL, "dwWaitBetweenTsts: %d", m_TestParams.dwWaitBetweenTests);
    ::Log(FYILOGLEVEL, "dwPerfWait       : %d", m_TestParams.dwPerfWait);
    ::Log(FYILOGLEVEL, "bStress          : %s", m_TestParams.bStress        ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bWaitAtTestEnd   : %s", m_TestParams.bWaitAtTestEnd ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bLogToScreen     : %s", m_TestParams.bLogToScreen   ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "dwDebugLevel     : %d", m_TestParams.dwDebugLevel);
    ::Log(FYILOGLEVEL, "dwRIPLevel       : %d", m_TestParams.dwRIPLevel);
    ::Log(FYILOGLEVEL, "dwMLDebugLevel   : %d", m_TestParams.dwMLDebugLevel);
    ::Log(FYILOGLEVEL, "szInitPerformance: %s", m_TestParams.szInitPerformance);
    ::Log(FYILOGLEVEL, "bSkipUserInput   : %s", m_TestParams.bSkipUserInput  ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bUseGM_DLS:      : %s", m_TestParams.bUseGM_DLS  ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "bSuppressMemInfo : %s", m_TestParams.bSuppressMemInfo ? "TRUE" : "FALSE");
    ::Log(FYILOGLEVEL, "dwStressFileIODuration: %d", m_TestParams.dwStressFileIODuration);
    ::Log(FYILOGLEVEL, "dwLocalLogLevel    : %d", m_TestParams.dwLocalLogLevel);    
    ::Log(FYILOGLEVEL, "bDoWorkLocalThread: %s", m_TestParams.bDoWorkLocalThread? "TRUE" : "FALSE");    
    
    if (m_TestParams.dwDoWorkFrequency)
        ::Log(FYILOGLEVEL, "dwDoWorkFrequency: %d times / second", m_TestParams.dwDoWorkFrequency);
    else
        ::Log(FYILOGLEVEL, "dwDoWorkFrequency: NEVER");

    ::Log(FYILOGLEVEL, "dwVoiceCount     : %d", m_TestParams.dwDMVoiceCount);    
    ::Log(FYILOGLEVEL, "dwDecreaseMemory : %d", m_TestParams.dwDecreaseMemory);    


    if (m_TestParams.dwStressFileIODuration)
    {
        ::Log(FYILOGLEVEL, "dwStressFileIODuration:  %d ms", m_TestParams.dwStressFileIODuration);
        ::Log(FYILOGLEVEL, "dwStressFileIOThreadPri: %s ms", m_TestParams.szStressFileIOThreadPri);
    }
    else
        ::Log(FYILOGLEVEL, "dwStressFileIODuration: NO FILE IO STRESS");
    ::Log(FYILOGLEVEL, "szDefaultMedia   : %s", m_TestParams.szDefaultMedia);
    ::Log(FYILOGLEVEL, "szDSScratchImage : %s", m_TestParams.szDSScratchImage);
    ::Log(FYILOGLEVEL, "szDSHRTF         : %s", m_TestParams.szDSHRTF);
    ::Log(FYILOGLEVEL, "szInitialTestCase: %s", m_TestParams.szInitialTestCase);
    ::Log(FYILOGLEVEL, "szDMHeap                : %s", m_TestParams.szDMHeap);
    ::Log(FYILOGLEVEL, "dwDMFixedHeapSizeNorm   : %u", m_TestParams.dwDMFixedHeapSizeNorm);
    ::Log(FYILOGLEVEL, "dwDMFixedHeapSizePhys   : %u", m_TestParams.dwDMFixedHeapSizePhys);

    return S_OK;
};


/********************************************************************************
********************************************************************************/
HRESULT SUITE::GetTestParams(TESTPARAMS *pTestParams)
{
    HRESULT hr = S_OK;
    CHECK_SUITE_INITIALIZED;

    if (!pTestParams)
        return E_INVALIDARG;

    *pTestParams = m_TestParams;
    return hr;
};


/********************************************************************************
********************************************************************************/
HRESULT SUITE::SetTestParams(TESTPARAMS &TestParams)
{
    HRESULT hr = S_OK;
    CHECK_SUITE_INITIALIZED;

    m_TestParams = TestParams;
    return hr;
};


/********************************************************************************
Loop through all tests and enable those of a certain type.
********************************************************************************/
void SUITE::EnableAllTestsOfType(LPSTR szType)
{
HRESULT hr = S_OK;
CHECK_SUITE_INITIALIZED;

DWORD i = 0;
    for (i=0; i<m_dwTestArraySize; i++)
    {
        if (strcmp(szType, m_pTestCases[i].szTestDesc) == 0)
        {
            m_pbRunTest[i] = TRUE;
        }
    }
};


/********************************************************************************
********************************************************************************/
void SUITE::AddOmitTestCase(LPSTR szTestName, BOOL bAdd)
{
HRESULT hr = S_OK;
CHECK_SUITE_INITIALIZED;

    BOOL bFound = FALSE;
    DWORD i = 0;
    DWORD dwPurpose = bAdd ? 1 : 0;
    CHAR *szPurpose[2]  = {"omitted", "added"};

    //We found a test to add/omit.  Go through our list of cases and turn on/off the "run" flag.
    for (i=0; i<m_dwTestArraySize; i++)
    {
        if (_strcmpi(m_pTestCases[i].szTestName, szTestName) == 0)
        {
            
            //Check to see if the test case was already in this state.  If so, warn the user.
            //No need to fail though.
            if (m_pbRunTest[i] == dwPurpose)
            {
                ::Log(FYILOGLEVEL, "Note: Test parameters requested %s to be %s, but it's already %s.", szTestName, szPurpose[dwPurpose], dwPurpose ? "there" : "absent");
            }
            
            m_pbRunTest[i] = dwPurpose; //0 = omit, 1 = add.
            ::Log(FYILOGLEVEL,"  -> %s %s %s set of test cases.", szPurpose[dwPurpose], szTestName, dwPurpose ? "to" : "from");
            bFound = TRUE;
            break;
        }
    }

    //Print an error if this test we were trying to add or omit wasn't found.
    if (!bFound)
    {
        ::Log(ABORTLOGLEVEL, "ReadAndProcessTestParams Error!!:");
        ::Log(ABORTLOGLEVEL, "Test Parameters requested that the ""%s"" test be %s, but that test doesn't exist", szTestName, szPurpose[dwPurpose]);
    }
}




#define MAX_MEMORY_OBJECTS 100
static    LONG lLastMemoryArray[MAX_MEMORY_OBJECTS] = {-1};
static    LONG lCurrentMemoryArray[MAX_MEMORY_OBJECTS]= {-1};



/******************************************************************************************
Runs through a whole array of tests.
******************************************************************************************/
void SUITE::RunTestArray(void)
{
    DWORD i;
    HRESULT hr = S_OK;
    HRESULT hrMemoryStats = S_OK;
    CtIDirectMusicPerformance8 *ptPerf8 = NULL;
    IDirectSound *pDSound = NULL;
    LPVOID        pMemory = NULL;

    //Copy our test suite's data to the global variable before anything else happens.  
    //THIS IS HORRIBLE DESIGN!!!!!!!  Only a quick hack because all the tests reference this structure.
    //TODO: Remove all references to g_TestParams!!    
    g_TestParams = m_TestParams;

    //Initresults
    m_dwPassed = 0;
    m_dwFailed = 0;

    //Steal memory, if requested.
    if (g_TestParams.dwDecreaseMemory)
    {
        pMemory = malloc(g_TestParams.dwDecreaseMemory);
        CHECKALLOC(pMemory);
    }

    //Initialize our high-score table of the worst test times.
    CHECKRUN(InitializeWorstTimes());

    //Initialize D3D ::Logging to screen.
    CHECKRUN(LogInit())

    //Initialize DSound (download scratch image)
    CHECKRUN(InitializeDSound(&pDSound));

    //Set DMusic's debug level.
    CHECK(LogSetDbgLevel(g_TestParams.dwLocalLogLevel));

    //Commence threads.
    //(DMusic's worker thread simulates a game running at (x=g_TestParams.dwDoWorkFrequency) FPS.)
    CHECKRUN(StartDoDMWorkThreadProc());
    CHECKRUN(StartFileIOStressThreadProc());

    //Initialize memory data.
    if (SUCCEEDED(hr))
    {
        memset((void *)lLastMemoryArray, 0, sizeof(LONG) * MAX_MEMORY_OBJECTS);
        memset((void *)lCurrentMemoryArray, 0, sizeof(LONG) * MAX_MEMORY_OBJECTS);
    }

    //Create the performance.
    if (InitPerformanceOnce())
    {
        CHECKRUN(CreateAndInitPerformance(&ptPerf8));
        if (FAILED(hr))
            ::Log(ABORTLOGLEVEL, "CreateAndInitPerformance returned %s (%08X)", dmthXlatHRESULT(hr), hr);
    }

    do
    {
        
        //Set "i" to either the beginning of the test array, or the specified test case, depending on the variable szInitialTestCase.
        if (_strcmpi(g_TestParams.szInitialTestCase, "") == 0)
        {
            i = 0;
        }
        else
        {

            BOOL bFound = FALSE;
            for (i=0; i<m_dwTestArraySize; i++)
            {
                if (_strcmpi(g_TestParams.szInitialTestCase, m_pTestCases[i].szTestName) == 0)
                {
                    bFound = TRUE;
                    break;
                }
            }
            if (!bFound)
            {
                ::Log(FYILOGLEVEL, "ERROR: Initial test case %s wasn't found - starting at index 0.", g_TestParams.szInitialTestCase);
                i = 0;
            }
        }
        
        //Loop through the array.
        for (; i<m_dwTestArraySize; i++)
        {
            HRESULT hrCall = S_OK;
            DWORD dwStartTime = 0;
            DWORD dwTotalTime = 0;

            if (!m_pbRunTest[i])
                continue;

            //Start the timer
            dwStartTime = timeGetTime();

            hrCall = RunTestCase(ptPerf8, m_pTestCases[i]);

            //Stop the timer.
            dwTotalTime = timeGetTime() - dwStartTime;

            ProcessTime(dwTotalTime, m_pTestCases[i].szTestName);
            
            
            if (S_FALSE == hrCall)
                hrCall = S_OK;
            if (SUCCEEDED(hrCall))
                m_dwPassed++;
            else
                m_dwFailed++;

            Wait(g_TestParams.dwWaitBetweenTests * 1000);
            //Take another reading of the current memory stats.
            if (!g_TestParams.bSuppressMemInfo)
            {
                GetMemoryStats(lCurrentMemoryArray, MAX_MEMORY_OBJECTS);
                CompareMemoryStats(lLastMemoryArray, lCurrentMemoryArray, MAX_MEMORY_OBJECTS, m_pTestCases[i].szTestName);
                DirectMusicMemDump(); 
                memcpy(lLastMemoryArray, lCurrentMemoryArray, sizeof(LONG) * MAX_MEMORY_OBJECTS);
            }
        }

    }
    while (g_TestParams.bStress);

    
    //Close down the performance.
    if (InitPerformanceOnce())
    {
        hr = ptPerf8->CloseDown();
        SAFE_RELEASE(ptPerf8);
    }

    //Shut down threads
    CHECKRUN(StopFileIOStressThreadProc());
    CHECKRUN(StopDoDMWorkThreadProc());

    
    //Release DSound
    RELEASE(pDSound);

    //Kill the D3D ::Logging to screen.
    CHECKRUN(LogFree());

    //Print and then Uninitialize our high-score table of the worst test times.
    CHECKRUN(PrintTimes());
    CHECKRUN(UnInitializeWorstTimes());


    //Release the wasted memory
    if (pMemory)
    {
        free(pMemory);
        pMemory = NULL;
    }

};





/********************************************************************************
********************************************************************************/
HRESULT SUITE::RunTestCase(CtIDirectMusicPerformance8 *ptPerf8_In, TESTCASE TestCase)
{
HRESULT hr = S_OK;
HRESULT hrTemp = S_OK;
CtIDirectMusicPerformance8 *ptPerf8 = NULL;

    //If a performance was passed in.
    if (InitPerformanceEveryTest())
    {
        ASSERT(!ptPerf8_In);
        hrTemp = CreateAndInitPerformance(&ptPerf8);
        if (FAILED(hrTemp))
            ::Log(ABORTLOGLEVEL, "CreateAndInitPerformance returned %s (%08X)", dmthXlatHRESULT(hrTemp), hrTemp);
    }
    else if (InitPerformanceOnce())
    {
        ASSERT(ptPerf8_In);
        ptPerf8 = ptPerf8_In;
    }
    //else if (InitPerformanceNever()) we'll expect the test to create the performance itself.  Only tests that expect this
    //  behavior should be run with this setting.

    
    //Call the test function and ::Log the result.
    SETLOG(g_hLog, "danhaff", "DMusic", "DMTest1", TestCase.szTestName);
    ::Log(1, "RUNNING %s", TestCase.szTestName);
    hr = TestCase.pTestProc(ptPerf8, TestCase.dwParam1, TestCase.dwParam2);
    if ( FAILED( hr ) )                                              
    {                                                                
        DbgPrint("%s returned %s (0x%x)\n", TestCase.szTestName, tdmXlatHRESULT(hr), hr );                
        xLog( g_hLog, XLL_FAIL, "%s returned %s (0x%x)\n", TestCase.szTestName, tdmXlatHRESULT(hr), hr );  
        ::Log(1, "FAIL: %s returned %s (0x%x)", TestCase.szTestName, tdmXlatHRESULT(hr), hr );  
        ::Log(1, "---------------------------------------------------------------------------");  
        ::Log(1, "");
    }                                                                
    else                                                             
    {                                                                
        
        xLog( g_hLog, XLL_PASS, "%s returned: %s (0x%x)\n", TestCase.szTestName, tdmXlatHRESULT(hr), hr );  
        ::Log(1, "PASS: %s returned %s (0x%x)", TestCase.szTestName, tdmXlatHRESULT(hr), hr );  
        ::Log(1, "---------------------------------------------------------------------------");  
        ::Log(1, "");
    }                                                                

    //If we initialized this, then close it down.
    if (InitPerformanceEveryTest())
    {
        hrTemp = S_OK;        
        hrTemp = ptPerf8->CloseDown();
        if (FAILED(hrTemp))
            ::Log(ABORTLOGLEVEL, "ERROR: CloseDown returned %s (%08X)", tdmXlatHRESULT(hrTemp), hrTemp);
        SAFE_RELEASE(ptPerf8);

    }
    else if (InitPerformanceOnce())
    {
        ASSERT(ptPerf8_In);
    }
    //else do nothing.

    return hr;

}


/********************************************************************************
********************************************************************************/
HRESULT SUITE::GetSuiteName(CHAR *pszSuiteName)
{
    strcpy(pszSuiteName, m_szSuiteName);
    return S_OK;
};


/********************************************************************************
********************************************************************************/
HRESULT SUITE::GetTestResults(DWORD *pdwPassed, DWORD *pdwFailed)
{
HRESULT hr = S_OK;
CHECK_SUITE_INITIALIZED

    if (pdwPassed)
        *pdwPassed = m_dwPassed;

    if (pdwFailed)
        *pdwFailed = m_dwFailed;

return hr;
};
