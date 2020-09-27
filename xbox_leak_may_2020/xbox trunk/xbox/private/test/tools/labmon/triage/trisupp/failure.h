#ifndef _FAILURE_
	#define _FAILURE_ 

#include <time.h>
#include <stdlib.h>

#define TIMEOUTPERIOD (time_t)210

#define MACH_X86		0
#define MACH_ALPHA		1
#define MACH_IA64		2
#define MACH_ALPHA64	3

#define BUILD_FREE		0
#define BUILD_CHECKED	1

#define PROMPT_KD         0
#define PROMPT_WINDBG     1
#define PROMPT_NTSD       2
#define PROMPT_ASSERT     3
#define PROMPT_USERASSERT 4
#define PROMPT_NOPROMPT   5
#define PROMPT_UNK        6

#define CPU_UNKNOWN     0
#define CPU_INTEL       1
#define CPU_AMD	        2
#define CPU_CYRIX       3

#define FT_UNK                      0
#define FT_BREAKPOINT               1
#define FT_ASSERTION_FAILURE        2
#define FT_ACCESS_VIOLATION         3
#define FT_INPAGE_IO_ERROR          4
#define FT_RESOURCE_TIMEOUT         5
#define FT_CRITICAL_SECTION_TIMEOUT 6
#define FT_POSSIBLE_DEADLOCK        7
#define FT_LSA_TOP_LEVEL_EXCEPTION  8
#define FT_NTSD_EXCEPTION           9
#define FT_BUGCHECK                 10
#define FT_UNH_EXCEPTION_WINLOGON   11
#define FT_UNH_EXCEPTION_SERVICES   12
#define FT_UNH_EXCEPTION_SVCHOST    13
#define FT_UNH_EXCEPTION_CSRSS      14
#define FT_HARDWARE_FAILURE         15
#define FT_MACHINE_SHUTDOWN         16
#define FT_CACHE_COHERENCY_BP       17
#define FT_MISALIGN                 18
#define FT_SPARSE_FILE_CORRUPT      19
#define FT_SYSCACHE_FAILURE         20
#define FT_UNKNOWN_EXCEPTION        21
#define FT_HIT_CTRL_C               22
#define FT_HIT_SYSRQ                23
#define FT_HIT_SHIFTF12             24
#define FT_INVALID_HANDLE           25
#define FT_HEAP_ASSERT              26
#define FT_INVALID_INSTRUCTION      27
#define FT_STACK_OVERFLOW           28
#define FT_WDM_BUGCHECK             29
#define FT_UNH_EXCEPTION            30
#define FT_UNRESPONSIVE             31


#define SAFEFREE(a) \
	{ \
		if (a != NULL) \
			free(a); \
		a = NULL; \
	}

class Cfailure
{
private:
    BOOL    fConnected;
    CHAR *  pszDebugger;
    CHAR *  pszPipe;

    BOOL    fFixSymbols;
    CHAR *  pszIniFile;

    DWORD   dwFailType;
    DWORD   dwWeBrokeIn;
    DWORD   dwDebugger;

    DWORD   dwBuild;
    DWORD   dwSP;
    DWORD   dwBuildDot;
    DWORD   dwVBL;
    BOOL    fLB6RI;
    CHAR *  pszVBL_Stamp;
    DWORD   dwBuildType;
    DWORD   dwArchitecture;
    DWORD   dwCPUType;
    
    CHAR *  pszFirstBuffer;
    CHAR *  pszCurrentStackBuffer;

    BOOL    fTwoPartCommand;
    CHAR *  pszStackCommand;

    CHAR *  pszDescription;
	CHAR *  pszMisc;
    CHAR *  pszStack;
    BOOL    fIsAssigned;
    CHAR *  pszFollowup;

    VOID SetPromptType(CHAR *buffer);
    BOOL SetTitleOnLastEvent();
    VOID TryForAssertData();
    BOOL GetMeSystemInfo();
    VOID FindIniFile(CHAR *pName);
    DWORD InitRemote(CHAR *pszNewDebugger, CHAR *pszNewPipe, CHAR *IniFile, DWORD dwFlags);
    DWORD GetSessionData();
    VOID DoNecessaryStackFixUps();

public:
    Cfailure();
    ~Cfailure();
    
    DWORD Init(CHAR *pszNewDebugger, CHAR *pszNewPipe, CHAR *IniFile, DWORD dwFlags = 0);

    BOOL SendCommand(const CHAR *pszCommand, CHAR **ppszRetBuffer, time_t tWaitTime = TIMEOUTPERIOD);
    BOOL SendStackCommand(const CHAR *pszCommand, BOOL fTwoPart = FALSE, BOOL fDoFixups = TRUE);
    BOOL SendCXRStack(ULARGE_INTEGER uliCxr);
    BOOL SendDotThreadStack(DWORD dwThread);
    BOOL SendBangThreadStack(ULARGE_INTEGER dwThread);
    BOOL SendBangThreadStack(DWORD dwThread, DWORD dwHighPart=0);
    BOOL SendUserThreadStack(DWORD dwUserThread);

    BOOL DoKernelBreakin();

    BOOL CloseRemote();
    BOOL GetReturnBuffer(CHAR ** ppRetBuf);

    BOOL ShutOffVerbose();
    BOOL DoAResync();

    DWORD FailureType()     {return dwFailType;}
    DWORD DebuggerType()    {return dwDebugger;}
    DWORD BuildNumber()     {return dwBuild;}
    DWORD SP()              {return dwSP;}
    DWORD BuildDot()        {return dwBuildDot;}
    DWORD VBL()             {return dwVBL;}
    DWORD BuildType()       {return dwBuildType;}
    DWORD Architecture()    {return dwArchitecture;}
    DWORD CPUType()         {return dwCPUType;}
    
    BOOL FixSymbols()       {return fFixSymbols;}
    BOOL IsStackAssigned()  {return fIsAssigned;}
    BOOL IsLB6RI()          {return fLB6RI;}

    const CHAR *StackCommand()  {return pszStackCommand;}
    const CHAR *VBLString()     {return pszVBL_Stamp;}
    const CHAR *BackSpew()      {return pszFirstBuffer;}
    const CHAR *StackBuffer()   {return pszCurrentStackBuffer;}
    const CHAR *IniFile()       {return pszIniFile;}

    const CHAR *Description()   {return pszDescription;}
    const CHAR *Misc()          {return pszMisc;}
    const CHAR *Stack()         {return pszStack;}
    const CHAR *Followup()      {return pszFollowup;}

    VOID SetFailureType(DWORD NewFT)   {dwFailType = NewFT;}
    VOID SetDebuggerType(DWORD NewDT)   {dwDebugger = NewDT;}
    VOID SetBuildNumber(DWORD NewBN)    {dwBuild = NewBN;}
    VOID SetSP(DWORD NewSP)             {dwSP = NewSP;}
    VOID SetBuildDot(DWORD NewDB)       {dwBuildDot = NewDB;}
    VOID SetVBL(DWORD NewVBL)           {dwVBL = NewVBL;}
    VOID SetLB6RI(BOOL NewRI)           {fLB6RI = NewRI;}
    VOID SetBuildType(DWORD NewBT)      {dwBuildType = NewBT;}
    VOID SetArchitecture(DWORD NewArch) {dwArchitecture = NewArch;}
    VOID SetCPUType(DWORD NewCPU)       {dwCPUType = NewCPU;}
    
    VOID SetFixSymbols(BOOL fFix)           {fFixSymbols = fFix;}

    VOID SetStackCommand(const CHAR *NewValue)  {SAFEFREE(pszStackCommand);pszStackCommand = _strdup(NewValue);}
    VOID SetVBLString(CHAR *NewValue)     {SAFEFREE(pszVBL_Stamp);pszVBL_Stamp = _strdup(NewValue); if ((pszVBL_Stamp) && (strlen(pszVBL_Stamp) >= 11)) pszVBL_Stamp[11] = '\0';}
    VOID SetBackSpew(CHAR *NewValue)      {SAFEFREE(pszFirstBuffer);pszFirstBuffer = _strdup(NewValue);}
    VOID SetStackBuffer(CHAR *NewValue)   {SAFEFREE(pszCurrentStackBuffer);pszCurrentStackBuffer = _strdup(NewValue);}
    VOID SetIniFile(CHAR *NewValue)       {SAFEFREE(pszIniFile);pszIniFile = _strdup(NewValue);}

    VOID SetDescription(CHAR *NewValue)   {SAFEFREE(pszDescription);pszDescription = _strdup(NewValue);}
    VOID SetMisc(CHAR *NewValue)          {SAFEFREE(pszMisc);pszMisc = _strdup(NewValue);}
    VOID SetStack(CHAR *NewValue)         {SAFEFREE(pszStack);pszStack = _strdup(NewValue);}
    VOID SetFollowup(CHAR *NewValue)      {SAFEFREE(pszFollowup);pszFollowup = _strdup(NewValue);fIsAssigned = TRUE;}

    VOID AppendDescription(CHAR *Addition);    
    VOID AppendMisc(CHAR *Addition);
};

#endif