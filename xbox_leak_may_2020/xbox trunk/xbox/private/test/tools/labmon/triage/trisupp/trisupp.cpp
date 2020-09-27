#include <windows.h>
#include "stacks.h"
#include "triage.h"

Cfailure *Failure;
HANDLE ghMutex = NULL;
WCHAR *gOpReturnBuffer = NULL;
CHAR *gszReturnBuffer = NULL; 

//---------------------------------------------------------------------
// DllEntryPoint()
//---------------------------------------------------------------------
BOOL __cdecl DllMain(
                     HINSTANCE	  Instance,
                     DWORD		  Reason,
                     PVOID		  Reserved
                     )
    {
    switch(Reason)
        {
        case DLL_PROCESS_ATTACH:
            break;
            
        case DLL_PROCESS_DETACH:
            break;
            
        case DLL_THREAD_ATTACH:
            break;
            
        case DLL_THREAD_DETACH:
            break;
            
        default:
            break;
        }
    
    return(TRUE);
    }


CHAR *TriageEx(CHAR *Debugger, CHAR *Machine, DWORD dwFlags, CHAR *inifile)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0;
    CHAR Followup[128] = {0};
    DWORD FollowupSize = sizeof(Followup);
    
    Failure = new Cfailure;
    
    if ((Failure->Init(Debugger, 
        Machine, 
        (inifile == NULL) ? "triage.ini" : inifile, 
        dwFlags)) != ERROR_SUCCESS)
        goto exit;
    
    if ((Failure->FixSymbols()) == TRUE)
        FixSymbolPath();
    
    if (!Failure->SendStackCommand(NULL))
        goto exit;
    
    // if there was no reasonable failure type description
    if ((Failure->FailureType()) == FT_UNK) 
        {
        // lets see if a stack gives us a clue.
        Failure->SetFailureType(ScanStackForFailureType(Failure->StackBuffer()));
        
        if ((Failure->FailureType()) == FT_UNK)
            Failure->SetFailureType(GetProblemTitle());
        }
    
    // at this point we should;
    //	1. know the failure type - might still be unknown
    //	2. have good symbols 
    //	3. know the debugger type, architecture, build number, build type
    //	4. have no unbroken asserts
    //	5. Have verbose off
    //	6. Still have the original buffer saved
    
    switch(Failure->FailureType())
        {
        case FT_HIT_SHIFTF12:
        case FT_HIT_CTRL_C:
        case FT_HIT_SYSRQ:
            goto exit;
            break;
            
        case FT_SPARSE_FILE_CORRUPT:
        case FT_SYSCACHE_FAILURE:
            if ((Failure->FailureType()) == FT_SYSCACHE_FAILURE)
                GetSpecial("syscachefail", Followup, FollowupSize);
            else
                GetSpecial("sparsecorrupt", Followup, FollowupSize);
            Failure->SetFollowup(Followup);
            break;
            
        case FT_UNKNOWN_EXCEPTION:
        case FT_INPAGE_IO_ERROR:
            if (GetSpecial(Failure->Description(), Followup, FollowupSize))
                Failure->SetFollowup(Followup);    
            break;
            
        case FT_BUGCHECK:
            HandleBugCheck();
            break;
            
        case FT_WDM_BUGCHECK:
            HandleWDMBugCheck();
            break;
            
        case FT_ASSERTION_FAILURE:
            GetAssertDataFromStack(Failure->StackBuffer());
            break;
            
        case FT_UNH_EXCEPTION:
        case FT_UNH_EXCEPTION_SVCHOST:
        case FT_UNH_EXCEPTION_WINLOGON:
        case FT_LSA_TOP_LEVEL_EXCEPTION:
        case FT_UNH_EXCEPTION_SERVICES:
        case FT_UNH_EXCEPTION_CSRSS:
            {
            ULARGE_INTEGER exr = {0,0};
            ULARGE_INTEGER cxr = {0,0};
            
            if (GetEXRAndCXR(&exr, &cxr))
                {
                GetExceptionBuffer(exr, cxr);
                
                //DoEXR(exr);
                Failure->SendCXRStack(cxr);
                }
            else
                {
                CHAR Name[128];
                if (GetProcessName(Name))
                    {
                    Failure->SetDescription("Breakpoint in ");
                    Failure->AppendDescription(Name);
                    }
                
                }
            
            GetDeeperStacks();
            }
            break;
            
        case FT_RESOURCE_TIMEOUT:
            {
            DWORD thread = 0;
            
            if (GetResTimeoutThreadFromBuffer(&thread))
                {
                if (!Failure->SendBangThreadStack(thread))
                    break;
                }
            GetDeeperStacks();	
            }	
            break;
            
        case FT_CRITICAL_SECTION_TIMEOUT:
        case FT_POSSIBLE_DEADLOCK:
            {
            if (IsUserFailure())
                {
                DWORD Pid =0, OwnerTid = 0, UserThread = 0;
                if (GetCriticalSectionInfoFromBuffer(&Pid, &OwnerTid))
                    {
                    if (GetUserThreadId(Pid, OwnerTid, &UserThread))
                        Failure->SendUserThreadStack(UserThread);
                    }
                DoTilde();
                
                GetGPResUserStack();
                }	//Is user failure
            
            GetDeeperStacks();	
            }
            break;
            
        default:
            GetDeeperStacks();	
            break;
            
    }
    /*
    if (!Failure->IsStackAssigned())
        {
        if (DoSpecialCasesBasedOnStack(Followup, FollowupSize))
            Failure->SetFollowup(Followup);
        }
    */
    buffer = _strdup(Failure->StackBuffer());
    numLines = GetLinesFromString(buffer, LineHeads);
    StripStack(LineHeads, &numLines);
    Failure->SetStack(StoreStack(&(LineHeads[0]), numLines));
    /*
    if (!Failure->IsStackAssigned())
        {
        // get assigned based on the stack we currently have stored in buffer
        if (AssignStack(&(LineHeads[0]),
            (UINT) numLines, 
            Followup, FollowupSize))
            
            Failure->SetFollowup(Followup);
        }*/
exit:
        
        Failure->GetReturnBuffer(&gszReturnBuffer);
        
        SAFEFREE(buffer);
        
        delete(Failure);
        return(gszReturnBuffer);
}

__declspec( dllexport ) PCHAR TriageA(PCHAR Debugger, PCHAR Machine, DWORD dwFlags, PCHAR inifile)
    {
    if (!ghMutex)
        ghMutex = CreateMutex(NULL, TRUE, NULL);
    
    if (ghMutex)
        WaitForSingleObject(ghMutex, INFINITE);
    
    return TriageEx(Debugger, Machine, dwFlags, inifile);
    }

__declspec( dllexport ) PWCHAR TriageW(PWCHAR Debugger, PWCHAR Machine, DWORD dwFlags, PWCHAR inifile)
    {
    CHAR aDebugger[MAX_PATH];
    CHAR aMachine[MAX_PATH];
    CHAR *ainifile = NULL;
    CHAR *retbuf = NULL;
    
    if (!ghMutex)
        ghMutex = CreateMutex(NULL, TRUE, NULL);
    
    if (ghMutex)
        WaitForSingleObject(ghMutex, INFINITE);
    
    wcstombs(aDebugger, Debugger, MAX_PATH);
    wcstombs(aMachine, Machine, MAX_PATH);
    if (inifile)
        {
        ainifile = (CHAR *)malloc((wcslen(inifile) + 1));
        wcstombs(ainifile, inifile, (wcslen(inifile) + 1));
        }
    
    retbuf = TriageEx(aDebugger, aMachine, dwFlags, ainifile);
    
    gOpReturnBuffer = (WCHAR *)malloc((strlen(retbuf) + 1)*sizeof(WCHAR));
    
    if (gOpReturnBuffer)
        {
        mbstowcs(gOpReturnBuffer, retbuf, (strlen(retbuf) + 1));
        }
    
    SAFEFREE(ainifile);
    
    return gOpReturnBuffer;
    }


//extern "C"
__declspec( dllexport ) void TriageCleanup()
    {
    
    SAFEFREE(gszReturnBuffer);
    SAFEFREE(gOpReturnBuffer);
    
    if (ghMutex)
        ReleaseMutex(ghMutex);
    }

