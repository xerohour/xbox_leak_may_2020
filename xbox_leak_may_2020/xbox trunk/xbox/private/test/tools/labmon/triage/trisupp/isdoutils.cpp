#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "failure.h"
#include "stacks.h"
#include "IsDoUtils.h"

extern Cfailure *Failure;

BOOL DoEXR(ULARGE_INTEGER exr)
    {
    CHAR command[24];
    CHAR *TempBuffer = NULL;
    
    sprintf(command, "!exr %I64x\n", exr.QuadPart);
    if (!(Failure->SendCommand(command, &TempBuffer)))
        return FALSE;
    
    if (strstr(TempBuffer, "c0000005"))
        {
        Failure->SetDescription("Access Violation");
        Failure->SetFailureType(FT_ACCESS_VIOLATION);
        }
    else if (strstr(TempBuffer, "c0000194"))
        {
        Failure->SetDescription("Critical Section Timeout");
        Failure->SetFailureType(FT_CRITICAL_SECTION_TIMEOUT);
        }
    else if (strstr(TempBuffer, "c0000008"))
        {
        Failure->SetDescription("Invalid Handle");
        Failure->SetFailureType(FT_INVALID_HANDLE);
        }
    else if (strstr(TempBuffer, "c00000fd"))
        Failure->SetDescription("Stack Overflow");
    
    free(TempBuffer);
    return TRUE;
    }

BOOL ResetCXR()
    {
    return Failure->SendCommand("!cxr\n", NULL);
    }

BOOL DoTilde()
    {
    return Failure->SendCommand("~\n", NULL);
    }

BOOL ReloadWin32k()
    {
    if (!(Failure->SendCommand("!reload win32k.sys=a0000000\n", NULL)))
        return FALSE;
    
    return TRUE;
    }

BOOL IsUserFailure()
    {
    // no user code on xbox
    return FALSE;
    /*
    if ((Failure->StackBuffer()) != NULL)
        {
        if ((stristr(Failure->StackBuffer(), "winsrv")) || 
            (stristr((Failure->StackBuffer()), "csrsrv")) || 
            (stristr((Failure->StackBuffer()), "user32")))
            {
            if (((Failure->DebuggerType()) == PROMPT_NTSD)&&
                (!(stristr((Failure->StackBuffer()), "ntdll!RtlpWaitForCriticalSection"))))
                return TRUE;
            }
        }
    return FALSE;
    */
    }

BOOL IsWaitingOnCriticalSection()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!RtlpWaitForCriticalSection"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsWaitingOnLoaderLock()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (stristr(Failure->StackBuffer(), "!BaseCreateThreadPoolThread"))
        if ((((Failure->FailureType()) == FT_CRITICAL_SECTION_TIMEOUT)||
            ((Failure->FailureType()) == FT_RESOURCE_TIMEOUT)))
            return TRUE;
        
        return FALSE;
    }

BOOL IsWaitingOnLPC()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "Waiting for reply to LPC MessageId"))
        return TRUE;
    
    return FALSE;
    }


BOOL IsAMutant()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "Mutant - owning thread"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsAquireResourceExclusive()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if ((strstr(Failure->StackBuffer(), "!ExpAcquireResourceExclusiveLite"))||
        (strstr(Failure->StackBuffer(), "!ExAcquireResourceExclusiveLite")))
        return TRUE;
    
    return FALSE;
    }

BOOL IsRtlEnterCriticalSection()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!RtlEnterCriticalSection"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsAquireResourceShared()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!ExAcquireResourceSharedLite"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsVerifierAquireResource()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!VerifierExAcquireResourceExclusive"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsRtlAquireResourceExclusive()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!RtlAcquireResourceExclusive"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsGreLockDisplay()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!GreLockDisplay"))
        return TRUE;
    
    return FALSE;
    }


BOOL IsContaingW32EnterCritical()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "win32k!EnterCrit"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsStuckInDuplicateObject()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!NtDuplicateObject"))
        return TRUE;
    
    return FALSE;
    }

BOOL IsStuckInChainedDispatch()
    {
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (strstr(Failure->StackBuffer(), "!KiChainedDispatch"))
        return TRUE;
    
    return FALSE;
    }



DWORD GetCPUType()
    {
    CHAR *buffer = NULL;
    DWORD mytype = UNKNOWN_CPU;
    
    if (!(Failure->SendCommand("!cpuinfo\n", &buffer)))
        return UNKNOWN_CPU;
    
    if (strstr(buffer, "GenuineIntel"))
        mytype = INTEL;
    else if (strstr(buffer, "AuthenticAMD"))
        mytype = AMD;
    else if (strstr(buffer, "CyrixInstead"))
        mytype = CYRIX;
    
    free(buffer);
    return mytype;
    }

BOOL Is1541HostBridge()
    {
    CHAR *buffer = NULL;
    
    if (!(Failure->SendCommand("!pci 0 0 0\n", &buffer)))
        return FALSE;
    
    if (strstr(buffer, ":1541"))
        {
        free(buffer);
        return TRUE;
        }
    
    free(buffer);
    return FALSE;
    }


