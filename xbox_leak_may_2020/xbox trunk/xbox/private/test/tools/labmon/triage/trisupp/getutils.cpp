#include <windows.h>
#include <stdio.h>
#include "utils.h"
#include "IsDoUtils.h"
#include "owner.h"
#include "failure.h"
#include "stacks.h"
#include "GetUtils.h"

extern Cfailure *Failure;

BOOL GetESI(ULARGE_INTEGER *Value)
    {
    CHAR *buffer = NULL;
    CHAR *tmp = NULL;
    
    if ((Failure->Architecture()) != MACH_X86)
        return FALSE;
    
    if (!Failure->SendCommand("resi\n", &buffer))
        return FALSE;
    
    if (tmp = strstr(buffer, "esi="))
        {
        if (1 == sscanf(tmp, "esi=%8x", &(Value->LowPart)))
            return TRUE;
        }
    
    return FALSE;
    
    }

BOOL GetAssertDataFromStack(const CHAR *buffer)
    {
    CHAR Procname[64];
    CHAR tmpbuff[512];
    ULARGE_INTEGER Address = {0,0};
    
    Procname[0] = '\0';
    if (strstr(buffer, "RtlAssert"))
        strcpy(Procname, "RtlAssert");
    else if (strstr(buffer, "DsysAssertEx"))
        strcpy(Procname, "DsysAssertEx");
    else if (strstr(buffer, "_DsysAssertEx"))
        strcpy(Procname, "_DsysAssertEx");
    else if (strstr(buffer, "RDAssertReport"))
        strcpy(Procname, "RDAssertReport");
    
    if (Procname[0] != '\0')
        {
        // Get the Assert
        if (!GetParamFromStackBuffer(Procname, PARAM3, &Address))
            return FALSE;
        
        if (!GetStringAtAddress(Address, tmpbuff, (size_t)512, FALSE))
            return FALSE;
        
        if (!strstr(tmpbuff, "Assertion Failed:"))
            Failure->AppendMisc("Assertion Failed: ");
        
        Failure->AppendMisc(tmpbuff);
        Failure->AppendMisc("\n");
        
        // Get the source File
        if (!GetParamFromStackBuffer(Procname, PARAM2, &Address))
            return FALSE;
        
        if (!GetStringAtAddress(Address, tmpbuff, (size_t)512, FALSE))
            return FALSE;
        
        if (!strstr(tmpbuff, "Source File:"))
            Failure->AppendMisc("Source File: ");
        
        if ((tmpbuff)[strlen(tmpbuff) - 1] == '\n')
            (tmpbuff)[strlen(tmpbuff) - 1] = '\0';
        
        Failure->AppendMisc(tmpbuff);
        
        // Get the line number
        if (!GetParamFromStackBuffer(Procname, PARAM1, &Address))
            return FALSE;
        
        Failure->AppendMisc(", line ");
        
        sprintf(tmpbuff, "%u", Address.LowPart);
        
        Failure->AppendMisc(tmpbuff);
        Failure->AppendMisc("\n");
        }
    return TRUE;
    
    }

DWORD ScanStackForFailureType(const CHAR *buffer)
    {
    CHAR Name[128] = "\0";
    CHAR Desc[256] = "\0";
    CHAR *tbuf = NULL;
    
    if (stristr(buffer, "bugcheck")) 
        {
        if (stristr(buffer, "ViBugcheckPrompt"))
            {
            Failure->SetDescription("WDM Driver Error");
            return FT_WDM_BUGCHECK;
            }
        else if (stristr(buffer, "WdBugCheck"))
            {
            Failure->SetDescription("WdBugCheck");
            }
        else
            {
            Failure->SetDescription("BugCheck");
            }
        return FT_BUGCHECK;
        } 
    else if ((stristr(buffer, "!UnhandledExceptionFilter")) || 
        (stristr(buffer, "!RtlUnhandledExceptionFilter")) ||
        (stristr(buffer, "Unhandled exception")) ||             // XBOX Test harness
        (stristr(buffer, "HarnessUnhandledExceptionFilter")) || // XBOX Test harness
        (stristr(buffer, "!.UnhandledExceptionFilter")) ||
        (stristr(buffer, "!__CxxUnhandledExceptionFilter")) ||
        (stristr(buffer, "!.RtlUnhandledExceptionFilter")))
        {
        //disabled because of slowness in !process because of the new symbol handler
        /*	    if (GetProcessName(Name))
        {
        if (!strcmp(Name, "services.exe"))
        {
        Failure->SetDescription("Unhandled Exception hit in services.exe");
        return FT_UNH_EXCEPTION_SERVICES;
        }
        else if (!strcmp(Name, "lsass.exe"))
        {
        Failure->SetDescription("LSA Top level exception handler hit");
        return FT_LSA_TOP_LEVEL_EXCEPTION;
        }
        else if (!strcmp(Name, "winlogon.exe"))
        {
        Failure->SetDescription("Unhandled Exception hit in winlogon");
        return FT_UNH_EXCEPTION_WINLOGON;
        }
        else if (!strcmp(Name, "svchost.exe"))
        {
        Failure->SetDescription("Unhandled Exception hit in svchost.exe");
        return FT_UNH_EXCEPTION_SVCHOST;
        }
        else if (!strcmp(Name, "csrss.exe"))
        {
        Failure->SetDescription("Unhandled Exception hit in csrss.exe");
        return FT_UNH_EXCEPTION_CSRSS;
        }
        else
        {
        Failure->SetDescription("Unhandled Exception in ");
        Failure->AppendDescription(Name);
        return FT_UNK;
        }
        }
        else
        */        {
        Failure->SetDescription("Unhandled Exception hit");
        return FT_UNH_EXCEPTION;
        }
        }
    else if (stristr(buffer, "assert")) 
        {        
        Failure->SetDescription("Assertion Failure");
        return FT_ASSERTION_FAILURE;
        } 
    else if ((stristr(buffer, "ExpWaitForResource"))||
        (stristr(buffer, "ExAcquireResourceExclusiveLite")))
        {
        Failure->SetDescription("Resource Timeout");
        return FT_RESOURCE_TIMEOUT;
        } 
    else if (stristr(buffer, "WaitForCriticalSection")) 
        {
        Failure->SetDescription("Critical Section Timeout");
        return FT_CRITICAL_SECTION_TIMEOUT;
        } 
    else if (stristr(buffer, "NMI")) 
        {
        Failure->SetDescription("Hardware Failure");
        return FT_HARDWARE_FAILURE;
        }
    else if (((stristr(buffer, "KeUpdateSystemTime"))&&
        ((stristr(buffer, "KiIdleLoop")) || (stristr(buffer, "DbgBreakPointWithStatus"))))||
        (stristr(buffer, "KeBreakinBreakpoint")))
        {
        Failure->SetDescription("User hit Ctrl-c");
        Failure->SetStack("If you did this because the machine was hung,\nplease do a '!vm' and a '!poolused 2' and send it to xstress.\nOtherwise hit 'g' to go.\n");
        Failure->SetFollowup("Machine Owner");
        return FT_HIT_CTRL_C;
        }
    else if ((stristr(buffer, "I8042KeyboardInterruptService"))&&
        ((stristr(buffer, "KiInterruptTemplateDispatch"))||
        (stristr(buffer, "KiInterruptDispatch")))) 
        {
        Failure->SetDescription("User hit SysRq/Print Screen");
        Failure->SetStack("If you did this because the machine was hung,\nplease do a '!vm' and a '!poolused 2' and send it to xstress.\nOtherwise hit 'g' to go.\n");		
        Failure->SetFollowup("Machine Owner");
        return FT_HIT_SYSRQ;
        }
    else if ((stristr(buffer, "DbgBreakPoint"))&&
        ((stristr(buffer, "BaseAttachComplete"))&&
        (stristr(buffer, "BaseAttachCompleteThunk")))&&
        (Failure->DebuggerType() == PROMPT_NTSD)) 
        {
        Failure->SetDescription("User hit SysRq/Print Screen");
        Failure->SetStack("If you did this because the machine was hung,\nplease do a '!vm' and a '!poolused 2' and send it to xstress.\nOtherwise hit 'g' to go.\n");		
        Failure->SetFollowup("Machine Owner");
        return FT_HIT_SYSRQ;
        }
    else if ((stristr(buffer, "!SrvActivateDebugger "))||
        (stristr(buffer, "!.SrvActivateDebugger ")))
        {
        Failure->SetDescription("User hit Shift-f12");
        Failure->SetStack("If you did this because the machine was hung,\nplease do a '!vm' and a '!poolused 2' and send it to xstress.\nOtherwise hit 'g' to go.\n");		
        Failure->SetFollowup("Machine Owner");
        return FT_HIT_SHIFTF12;
        }
    else
        {
        if (GetProcessName(Name))
            {
            strcpy(Desc, "Breakpoint in ");
            strcat(Desc, Name);
            Failure->SetDescription(Desc);
            }
        }
    
    return FT_UNK;
}


DWORD GetProblemTitle()
    {
    const CHAR *lowbuff = Failure->BackSpew();
    CHAR *curbuff = NULL;
    DWORD dwFail = FT_UNK;
    CHAR Desc[256] = "\0";
    CHAR xbuff[1024] = "\0";
    CHAR *it = NULL;
    CHAR save;
    
    if (lowbuff == NULL)
        return dwFail;    
    
    while (curbuff = stristr(lowbuff, "waiting to reconnect")) 
        {
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "THIS IS NOT A BUG OR A SYSTEM CRASH")) 
        {
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Invalid Address specified to RtlFreeHeap")) 
        {
        dwFail = FT_HEAP_ASSERT;
        strcpy(Desc, "Heap Assert");
        lowbuff = curbuff + 1;
        } 
    
    //   Access Violation - code: c0000005  (second chance)804138a7 6683790c00
    while (curbuff = stristr(lowbuff, "Access Violation")) 
        {
        dwFail = FT_ACCESS_VIOLATION;
        strcpy(Desc, "Access Violation");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Resource @")) 
        {
        dwFail = FT_RESOURCE_TIMEOUT;
        strcpy(Desc, "Resource Timeout");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Critical Section Timeout")) 
        {
        dwFail = FT_CRITICAL_SECTION_TIMEOUT;
        strcpy(Desc, "Critical Section Timeout");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Invalid handle")) 
        {
        dwFail = FT_INVALID_HANDLE;
        strcpy(Desc, "Invalid Handle");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "deadlock")) 
        {
        dwFail = FT_POSSIBLE_DEADLOCK;
        strcpy(Desc, "Possible Deadlock");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "maka")) 
        {
        dwFail = FT_CACHE_COHERENCY_BP;
        strcpy(Desc, "Cache Coherency Breakpoint");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "*** Fatal System Error:")) 
        {
        dwFail = FT_BUGCHECK;
        strcpy(Desc, "BugCheck");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "LSA Top level exception handler hit")) 
        {
        dwFail = FT_LSA_TOP_LEVEL_EXCEPTION;
        strcpy(Desc, "LSA Top level exception handler hit");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Unhandled Exception hit in winlogon")) 
        {
        dwFail = FT_UNH_EXCEPTION_WINLOGON;
        strcpy(Desc, "Unhandled Exception hit in winlogon");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Unhandled Exception hit in services.exe")) 
        {
        dwFail = FT_UNH_EXCEPTION_SERVICES;
        strcpy(Desc, "Unhandled Exception hit in services.exe");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Unhandled Exception hit in csrss.exe")) 
        {
        dwFail = FT_UNH_EXCEPTION_CSRSS;
        strcpy(Desc, "Unhandled Exception hit in csrss.exe");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Unhandled Exception hit in svchost.exe")) 
        {
        dwFail = FT_UNH_EXCEPTION_SVCHOST;
        strcpy(Desc, "Unhandled Exception hit in svchost.exe");
        lowbuff = curbuff + 1;
        } 
    
    while ((curbuff = stristr(lowbuff, "Sparse files,"))
        &&(it = stristr(curbuff, "found corruption!"))) 
        {
        dwFail = FT_SPARSE_FILE_CORRUPT;
        strcpy(Desc, "Sparse File Corruption");
        
        
        it[strlen("found corruption")] = '\0';
        
        Failure->AppendMisc(curbuff);
        Failure->AppendMisc("!\n");
        
        it[strlen("found corruption")] = '!';
        
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "SYSCACHE failure on a Remote volume"))
        {
        dwFail = FT_SYSCACHE_FAILURE;
        strcpy(Desc, "SysCache Failure");
        
        Failure->AppendMisc("SYSCACHE failure on a Remote volume!\n");
        
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "SYSCACHE failure on a Local volume"))
        {
        dwFail = FT_SYSCACHE_FAILURE;
        strcpy(Desc, "SysCache Failure");
        
        Failure->AppendMisc("SYSCACHE failure on a Local volume!\n");
        
        lowbuff = curbuff + 1;
        } 
    
    while ((curbuff = stristr(lowbuff, "in page io error"))||
        (curbuff = stristr(lowbuff, "In-page I/O error"))) 
        {
        dwFail = FT_INPAGE_IO_ERROR;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            save = *it;
            *it = '\0';
            strcpy(Desc, curbuff);
            *it = save;
            }
        else
            strcpy(Desc, "In Page IO Error");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Unknown Exception")) 
        {
        dwFail = FT_UNKNOWN_EXCEPTION;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            save = *it;
            *it = '\0';
            strcpy(Desc, curbuff);
            *it = save;
            Desc[34] = '\0';
            }
        else
            strcpy(Desc, "Unknown Exception Code");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "exception number")) 
        {
        dwFail = FT_NTSD_EXCEPTION;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            save = *it;
            *it = '\0';
            strcpy(Desc, curbuff);
            *it = save;
            }
        else
            strcpy(Desc, "NTSD Exception");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "misalign ")) 
        {
        dwFail = FT_MISALIGN;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            save = *it;
            *it = '\0';
            strcpy(Desc, curbuff);
            *it = save;
            }
        else
            strcpy(Desc, "Misalign");
        lowbuff = curbuff + 1;
        } 
    
    while (curbuff = stristr(lowbuff, "Assertion")) 
        {
        dwFail = FT_ASSERTION_FAILURE;
        xbuff[0] = '\0';
        strcpy(Desc, "Assertion Failure");
        lowbuff = curbuff + 1;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            save = *it;
            *it = '\0';
            strcpy(xbuff, curbuff);
            *it = save;
            
            if (curbuff = stristr(lowbuff, "Source File"))
                {
                if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
                    {
                    save = *it;
                    *it = '\0';
                    strcat(xbuff, curbuff);
                    *it = save;
                    lowbuff = it;
                    }
                }
            
            }
        } 
    
    while (curbuff = stristr(lowbuff, "(bizro)?")) 
        {
        dwFail = FT_ASSERTION_FAILURE;
        strcpy(Desc, "Assertion Failure");
        lowbuff = curbuff + 1;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            CHAR *startptr = curbuff;
            DWORD Count = 5;
            save = *it;
            *it = '\0';
            // rewindto get the previous 4 lines
            while ((startptr >= Failure->BackSpew()) && (Count > 0))
                {
                startptr--;
                if (startptr[0] == '\r')
                    {
                    Count--;
                    }
                }
            while ((strchr(curbuff, '\r'))||(strchr(curbuff, '\n')))
                startptr++;
            
            strcpy(xbuff, startptr);
            *it = save;
            
            }
        } 
    
    while (curbuff = stristr(lowbuff, "(bizrd)?")) 
        {
        dwFail = FT_ASSERTION_FAILURE;
        strcpy(Desc, "Assertion Failure");
        lowbuff = curbuff + 1;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            CHAR *startptr = curbuff;
            DWORD Count = 5;
            save = *it;
            *it = '\0';
            // rewindto get the previous 4 lines
            while ((startptr >= Failure->BackSpew()) && (Count > 0))
                {
                startptr--;
                if (startptr[0] == '\r')
                    {
                    Count--;
                    }
                }
            while ((strchr(curbuff, '\r'))||(strchr(curbuff, '\n')))
                startptr++;
            
            strcpy(xbuff, startptr);
            *it = save;
            
            }
        } 
    
    if (dwFail != FT_UNK)
        {
        if (Desc[0] != '\0')
            Failure->SetDescription(Desc);
        
        if (strlen(xbuff))
            Failure->AppendMisc(xbuff);
        }
    
    return dwFail;
}


// Does a dd on hte address specifed and returns the requested longword
BOOL GetDxLongNumber(ULARGE_INTEGER Address, DWORD LNum, ULARGE_INTEGER *Value)
    {
    CHAR TheString[32];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD vals[5] = {0,0,0,0,0};
    DWORD numLines = 0, i = 0;
    CHAR x = '\0';
    
    if ((LNum == 0)||(Value == NULL)||(Address.QuadPart == 0)||(LNum > ((MAXLINES-1) * 4)))
        return FALSE;
    
    Value->QuadPart = 0;
    if ((Failure->Architecture()) == MACH_X86)
        sprintf(TheString, "dd %x l%x\n", Address.LowPart, LNum);
    else
        sprintf(TheString, "dq %I64x l%x\n", Address.QuadPart, LNum);
    
    if (!Failure->SendCommand(TheString, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if (sscanf(LineHeads[i], "%8x ", &vals[0]) > 0)
            {
            if (LNum > 4)
                {
                if ((Failure->Architecture()) == MACH_X86)
                    {
                    if (5 == sscanf(LineHeads[i], "%8x %8x %8x %8x %8x", &vals[0], &vals[0],
                        &vals[0], &vals[0], &vals[0]))
                        LNum = LNum - 4;
                    }
                else
                    {
                    if (6 == sscanf(LineHeads[i], "%8x`%8x  %8x`%8x %8x`%8x", &vals[0], &vals[0],
                        &vals[0], &vals[0], &vals[0], &vals[0]))
                        LNum = LNum - 2;
                    }
                }
            else
                {
                if ((Failure->Architecture()) == MACH_X86)
                    {
                    INT iRet = 0;
                    
                    iRet = sscanf(LineHeads[i], "%8x %c", &vals[0], &x);
                    if ((x == '?')||(iRet != 2))
                        {
                        free(buffer);
                        return FALSE;
                        }
                    
                    if ((INT)(LNum + 1) == sscanf(LineHeads[i], "%8x %8x %8x %8x %8x", &vals[0], &vals[1],
                        &vals[2], &vals[3], &vals[4]))
                        {
                        Value->LowPart = vals[LNum];
                        free(buffer);
                        if ((vals[0] == 0) && (vals[1] == 0))  //What was I thinking?
                            return FALSE;
                        return TRUE;
                        }
                    }
                else
                    {
                    INT iRet = 0;
                    iRet = sscanf(LineHeads[i], "%8x`%8x %c", &vals[0], &vals[0], &x);
                    if ((x == '?') || (iRet != 3))
                        {
                        free(buffer);
                        return FALSE;
                        }
                    
                    if ((INT)((LNum * 2) + 2) == sscanf(LineHeads[i], "%8x`%8x  %8x`%8x %8x`%8x", 
                        &vals[0], &vals[0], &vals[1],
                        &vals[2], &vals[3], &vals[4]))
                        {
                        Value->LowPart = vals[(LNum * 2)];
                        Value->HighPart = vals[((LNum * 2) - 1)];
                        free(buffer);
                        return TRUE;
                        }
                    }
                }
            }
        }
    
    free(buffer);
    return FALSE;
    }

// Does a dd on hte address specifed and returns the requested longword
BOOL GetDxLongNumber(DWORD Address, DWORD LNum, DWORD *Value)
    {	
    ULARGE_INTEGER uliAddress = {0,0};
    ULARGE_INTEGER uliValue = {0,0};
    
    uliAddress.LowPart = Address;
    
    if (!GetDxLongNumber(uliAddress, LNum, &uliValue))
        return FALSE;
    
    *Value = uliValue.LowPart;
    
    return TRUE;
    }

//  This does a dd on the string specified and returns the dd asked for.  For example
// dd nwin32k!gpresuser l1 will get you the gpresuser
BOOL GetStringLongNumber(CHAR *StringName, DWORD LNum, ULARGE_INTEGER *Value)
    {
    CHAR TheString[128];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD vals[5] = {0,0,0,0,0};
    DWORD numLines = 0, i = 0;
    CHAR x;
    
    if ((LNum == 0)||(Value == NULL)||(StringName == NULL)||(LNum > ((MAXLINES-1) * 4)))
        return FALSE;
    
    Value->QuadPart = 0;
    
    if ((Failure->Architecture()) == MACH_X86)
        sprintf(TheString, "dd %s l%x\n", StringName, LNum);
    else
        sprintf(TheString, "dq %s l%x\n", StringName, LNum);
    
    if (!Failure->SendCommand(TheString, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if (LNum > 4)
            {
            if ((Failure->Architecture()) == MACH_X86)
                {
                if (5 == sscanf(LineHeads[i], "%8x %8x %8x %8x %8x", &vals[0], &vals[0],
                    &vals[0], &vals[0], &vals[0]))
                    LNum = LNum - 4;
                }
            else
                {
                if (6 == sscanf(LineHeads[i], "%8x`%8x  %8x`%8x %8x`%8x", &vals[0], &vals[0],
                    &vals[0], &vals[0], &vals[0], &vals[0]))
                    LNum = LNum - 2;
                }
            }
        else
            {
            if ((Failure->Architecture()) == MACH_X86)
                {
                INT iRet = 0;
                
                iRet = sscanf(LineHeads[i], "%8x %c", &vals[0], &x);
                if ((x == '?') || (iRet != 2))
                    {
                    free(buffer);
                    return FALSE;
                    }
                
                if ((INT)(LNum + 1) == sscanf(LineHeads[i], "%8x %8x %8x %8x %8x", &vals[0], &vals[1],
                    &vals[2], &vals[3], &vals[4]))
                    {
                    Value->LowPart = vals[LNum];
                    free(buffer);
                    if ((vals[0] == 0) && (vals[1] == 0))  //What was I thinking?
                        return FALSE;
                    return TRUE;
                    }
                }
            else
                {
                INT iRet = 0;
                
                iRet = sscanf(LineHeads[i], "%8x`%8x %c", &vals[0], &vals[0], &x);
                if ((x == '?') || (iRet != 3))
                    {
                    free(buffer);
                    return FALSE;
                    }
                
                if ((INT)((LNum * 2) + 2) == sscanf(LineHeads[i], "%8x`%8x  %8x`%8x %8x`%8x", 
                    &vals[0], &vals[0], &vals[1],
                    &vals[2], &vals[3], &vals[4]))
                    {
                    Value->LowPart = vals[(LNum * 2)];
                    Value->HighPart = vals[((LNum * 2) - 1)];
                    free(buffer);
                    return TRUE;
                    }
                }
            }
        }
    
    free(buffer);
    return FALSE;
    }
// Will provide the thread address of the specified TID in the current process
BOOL GetThreadNumberOfCurrentProcTID(ULARGE_INTEGER TID, ULARGE_INTEGER *Thread)
    {
    CHAR TheString[32];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    
    if ((TID.QuadPart == 0)||(Thread == NULL))
        return FALSE;
    
    Thread->QuadPart = 0;
    if (!Failure->SendCommand("!process -1 4\n", &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    sprintf(TheString, ".%I64x  Teb:", TID.QuadPart);
    
    for (i=0; i < numLines; i++)
        {
        if (strstr(LineHeads[i], TheString))
            {
            if ((Failure->Architecture()) == MACH_X86)
                {
                if (sscanf(LineHeads[i], " THREAD %8x", &(Thread->LowPart)) == 1)
                    {
                    free(buffer);
                    return TRUE;
                    }
                }
            else
                {
                if (sscanf(LineHeads[i], " THREAD %I64x", &(Thread->QuadPart)) == 1)
                    {
                    free(buffer);
                    return TRUE;
                    }
                }
            }
        }
    
    free(buffer);
    return FALSE;
    }

// Will provide the thread address of the specified TID in the specified process
BOOL GetThreadNumberOfPIDsTID(DWORD TID, DWORD PID, DWORD *Thread)
    {
    CHAR TheString[35];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    
    if ((TID == 0)||(Thread == NULL))
        return FALSE;
    
    *Thread = 0;
    
    sprintf(TheString, "!process %x 4\n", PID);
    
    if (!Failure->SendCommand(TheString, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    sprintf(TheString, ".%x  Teb:", TID);
    
    for (i=0; i < numLines; i++)
        {
        if (strstr(LineHeads[i], TheString))
            {
            if (sscanf(LineHeads[i], " THREAD %8x", Thread) == 1)
                {
                free(buffer);
                return TRUE;
                }
            }
        }
    
    free(buffer);
    return FALSE;
    }


BOOL GetProcedureParamNumber(CHAR *Command, CHAR *ProcName, DWORD PNum, DWORD *Value)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    DWORD num[5];
    
    if ((ProcName == NULL)||(Value == NULL)||(Command == NULL)||(PNum < 1)||(PNum > 5))
        return FALSE;
    
    *Value = 0;
    
    if (!Failure->SendCommand(Command, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if (strstr(LineHeads[i], ProcName))
            {
            if (sscanf(LineHeads[i], "%8x %8x %8x %8x %8x ", 
                &num[4], &num[3], &num[2], &num[1], &num[0]) == 5)
                {
                free(buffer);
                *Value = num[PNum-1];
                return TRUE;
                }
            }
        }
    
    free(buffer);
    return FALSE;
    }

BOOL GetParamFromStackBuffer(CHAR *ProcName, DWORD PNum, ULARGE_INTEGER *Value)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    DWORD num[8];
    
    if ((ProcName == NULL)||(Value == NULL)||(PNum < 1)||(PNum > 5))
        return FALSE;
    
    Value->QuadPart = 0;
    
    buffer = _strdup(Failure->StackBuffer());
    if (!buffer)
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if (stristr(LineHeads[i], ProcName))
            {
            if ((Failure->Architecture()) == MACH_X86)
                {
                if (sscanf(LineHeads[i], "%8x %8x %8x %8x %8x ", 
                    &num[4], &num[3], &num[2], &num[1], &num[0]) == 5)
                    {
                    free(buffer);
                    Value->LowPart = num[PNum-1];
                    return TRUE;
                    }
                }
            else
                {
                if (PNum <= 4)
                    {
                    if (sscanf(LineHeads[i], "%8x`%8x : %8x`%8x %8x`%8x %8x`%8x", 
                        &num[7], &num[6], &num[5], &num[4], &num[3], &num[2], &num[1], &num[0]) == 8)
                        {
                        free(buffer);
                        Value->HighPart = num[(PNum * 2) - 1];
                        Value->LowPart = num[(PNum * 2) - 2];
                        return TRUE;
                        }
                    }
                }
            }
        }
    
    free(buffer);
    return FALSE;
    }

BOOL GetParamFromStackBuffer(CHAR *ProcName, DWORD PNum, DWORD *Value)
    {
    ULARGE_INTEGER uliValue = {0,0};
    
    if (!GetParamFromStackBuffer(ProcName, PNum, &uliValue))
        return FALSE;
    
    *Value = uliValue.LowPart;
    
    return TRUE;
    }

BOOL GetThreadHoldingLoaderLock(ULARGE_INTEGER *Thread)
    {	
    ULARGE_INTEGER Value = {0,0};
    
    if (Thread == NULL)
        return FALSE;
    
    Thread->QuadPart = 0;
    
    if (!GetStringLongNumber("ntdll!loaderlock", 4, &Value))
        return FALSE;
    
   	// Get the thread address of the TID
    if (!GetThreadNumberOfCurrentProcTID(Value, Thread))
        return FALSE;
    
    return TRUE;
    }

BOOL GetThreadHoldingLock(DWORD Lock, DWORD *Thread)
    {	
    CHAR Command[32];
    CHAR *pTidStart = NULL;
    CHAR *buffer = NULL;
    DWORD Garb = 0;
    
    if ((Thread == NULL)||(Lock < 2))
        return FALSE;
    
    *Thread = 0;
    
    sprintf(Command, "!locks %08x\n", Lock);
    
    if (!(Failure->SendCommand(Command, &buffer)))
        return FALSE;
    
    if (!(pTidStart = strstr(buffer, "Threads: ")))
        {
        free(buffer);
        return FALSE;
        }
    
    if (sscanf(pTidStart, "Threads: %8x-%2d", Thread, &Garb) == 2)
        {
        free(buffer);
        return TRUE;
        }
    
    free(buffer);
    return FALSE;
    }

BOOL GetEXRAndCXRFromDx(ULARGE_INTEGER Address, ULARGE_INTEGER *Exr, ULARGE_INTEGER *Cxr)
    {
    CHAR TheString[32];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD vals[5] = {0,0,0};
    DWORD numLines = 0, i = 0;
    CHAR x;
    
    if ((Address.QuadPart == 0)||(Exr == NULL)||(Cxr == NULL))
        return FALSE;
    
    Exr->QuadPart = 0;
    Cxr->QuadPart = 0;
    
    if ((Failure->Architecture()) == MACH_X86)
        sprintf(TheString, "dd %x l2\n", Address.LowPart);
    else
        sprintf(TheString, "dq %I64x l2\n", Address.QuadPart);
    
    if (!Failure->SendCommand(TheString, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if ((Failure->Architecture()) == MACH_X86)
            {
            INT iRet = 0;
            
            iRet = sscanf(LineHeads[i], "%8x %c", &vals[0], &x);
            if ((x == '?') || (iRet != 2))
                {
                free(buffer);
                return FALSE;
                }
            
            if (3 == sscanf(LineHeads[i], "%8x %8x %8x", &vals[0], &vals[1], &vals[2]))
                {
                Exr->LowPart = vals[1];
                Cxr->LowPart = vals[2];
                free(buffer);
                return TRUE;
                }
            }
        else
            {
            INT iRet = 0;
            
            iRet = sscanf(LineHeads[i], "%8x`%8x %c", &vals[0], &vals[0], &x);
            if ((x == '?') || (iRet != 3))
                {
                free(buffer);
                return FALSE;
                }
            
            if (6 == sscanf(LineHeads[i], "%8x`%8x  %8x`%8x %8x`%8x", &vals[0], &vals[0],
                &vals[1], &vals[2],
                &vals[3], &vals[4]))
                {
                Exr->LowPart = vals[2];
                Exr->HighPart = vals[1];
                Cxr->LowPart = vals[4];
                Cxr->HighPart = vals[3];
                free(buffer);
                return TRUE;
                }
            }
        
        }
    
    free(buffer);
    return FALSE;
    }

    /*BOOL GetEXRAndCXRFromBackSpew(DWORD *Exr, DWORD *Cxr)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0, j = 0;
    CHAR *tptr = NULL;
    
      if ((Exr == NULL)||(Cxr == NULL))
      return FALSE;
      
        *Exr=0;
        *Cxr=0;
        
          tptr = strstr(Failure->BackSpew(), "!exr");
          if (tptr)
          {
          if (1 == sscanf(Failure->BackSpew(), "!exr %X", Exr))
          {
          tptr = strstr(Failure->BackSpew(), "!cxr");
          if (tptr)
          {
          if (1 == sscanf(Failure->BackSpew(), "!cxr %X", Cxr))
          return TRUE;
          }
          }
          }
          
            buffer = _strdup(Failure->BackSpew());
            
              if (!buffer)
              return FALSE;
              
                numLines = GetLinesFromString(buffer, LineHeads);
                
                  for (i=numLines; i > 0; i--)
                  {
                  // Deal with lsass
                  if (tptr = strstr(LineHeads[i-1], "LSA Top level exception handler hit:"))
                  {
                  DWORD Address = 0;
                  LineHeads[i-1] = tptr;
                  if (1 == sscanf(LineHeads[i-1], 
                  "LSA Top level exception handler hit: Exception record = %8x", &Address))
                  {
                  if (GetEXRAndCXRFromDx(Address, Exr, Cxr))
                  {
                  free(buffer);
                  return TRUE;
                  }
                  }
                  else
                  {
                  free(buffer);
                  return FALSE;
                  }
                  }
                  
                    if (tptr = strstr(LineHeads[i-1], "Unhandled Exception hit in"))
                    {
                    for (j=i; j < numLines; j++)
                    {
                    if (tptr = strstr(LineHeads[j], "first, enter !exr"))
                    {
                    LineHeads[j] = tptr;
                    if (1 == sscanf(LineHeads[j], "first, enter !exr %8x", Exr))
                    {
                    
                      }
                      else
                      {
                      free(buffer);
                      return FALSE;
                      }
                      }
                      
                        if (tptr = strstr(LineHeads[j], "next, enter !cxr"))
                        {
                        LineHeads[j] = tptr;
                        if (1 == sscanf(LineHeads[j], "next, enter !cxr %8x", Cxr))
                        {
                        free(buffer);
                        if (*Exr)
                        return TRUE;
                        else
                        return FALSE;
                        }
                        else
                        {
                        free(buffer);
                        return FALSE;
                        }
                        }
                        }
                        }
                        }
                        
                          free(buffer);
                          return FALSE;
                          }
*/
BOOL GetEXRAndCXR(ULARGE_INTEGER *Exr, ULARGE_INTEGER *Cxr)
    {
    ULARGE_INTEGER Address = {0,0};
    DWORD ParamNumber = 3;
    //	CHAR ReloadString[128];
    //	CHAR ProcString[256];
    CHAR *buffer = NULL;
    
    if ((Exr == NULL)||(Cxr == NULL))
        return FALSE;
    

    // Search for the test harness exception handler help text
    char *info = strstr(Failure->BackSpew(), "type \"!cxr");
    if(info)
        {
        info += 11;
        sscanf(info, "%X", &(Cxr->QuadPart));
        return TRUE;
        }




    if (!stristr(Failure->StackBuffer(), "UnhandledExceptionFilter"))
        {	
        if (!Failure->SendCommand("!reload\n", NULL))
            return FALSE;
        if (!Failure->SendStackCommand("kb\n"))
            return FALSE;
        }
    
    if ((!GetParamFromStackBuffer("MSVCRT!__CxxUnhandledExceptionFilter", 
        (DWORD)3, 
        &Address)) || (Address.QuadPart == 0))
        {
        if ((!GetParamFromStackBuffer("!RtlUnhandledExceptionFilter", 
            (DWORD)3, 
            &Address))||(Address.QuadPart == 0))
            {
            if (!GetParamFromStackBuffer("!UnhandledExceptionFilter", 
                (DWORD)3, 
                &Address))
                {
                free(buffer);
                return FALSE;
                }
            }
        }
    
    free(buffer);
    
    return GetEXRAndCXRFromDx(Address, Exr, Cxr);
    }

BOOL GetGPResUser(DWORD *Lock)
    {
    ULARGE_INTEGER temp = {0,0};
    
    if (!GetStringLongNumber("win32k!gpresUser", (DWORD) 1, &temp))
        {
        if (!ReloadWin32k())
            return FALSE;
        else if (!GetStringLongNumber("win32k!gpresUser", (DWORD) 1, &temp))
            return FALSE;
        }
    
    *Lock = temp.LowPart;
    return TRUE;
    }

BOOL GetViBadDriverAddress(ULARGE_INTEGER *Address)
    {
    ULARGE_INTEGER first = {0,0};
    
    if (!GetStringLongNumber("nt!vibaddriver", (DWORD) 1, &first))
        return FALSE;
    
    if (!GetDxLongNumber(first, (DWORD) 2, Address))
        return FALSE;
    
    return TRUE;
    }

BOOL GetViBadDriverString(CHAR *BadString, DWORD len)
    {
    CHAR Command[128];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    DWORD excess = 0;
    ULARGE_INTEGER Address = {0,0};
    CHAR *tp = NULL;
    
    if (BadString == NULL)
        return FALSE;
    
    if (!GetViBadDriverAddress(&Address))
        return FALSE;
    
    sprintf(Command, "du %I64x\n", Address.QuadPart);
    
    if (!(Failure->SendCommand(Command, &buffer)))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if ((sscanf(LineHeads[i], "%8x \"%s", 
            &excess, Command) == 2) || (sscanf(LineHeads[i], "%8x`%8x \"%s", 
            &excess, &excess, Command) == 3))
            {
            free(buffer);
            strncpy(BadString, Command, len);
            tp = strchr(BadString, '"');
            if (tp)
                tp[0] = '\0';
            sprintf(Command, "!verifier 3 %s\n", BadString);
            Failure->SendCommand(Command, NULL);
            return TRUE;
            }
        }
    
    free(buffer);
    return FALSE;
    }

BOOL GetProcessName(CHAR *Name)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    CHAR tempName[128];
    
    if (Name == NULL)
        return FALSE;
    
    if ((Failure->DebuggerType()) == PROMPT_KD)
        {
        if (!(Failure->SendCommand("!process -1 0\n", &buffer)))
            return FALSE;
        
        numLines = GetLinesFromString(buffer, LineHeads);
        
        for (i=0; i < numLines; i++)
            {
            if (sscanf(LineHeads[i], " Image: %s", 
                tempName) == 1)
                {
                strcpy(Name, tempName);
                free(buffer);
                return TRUE;
                }
            }
        free(buffer);
        }
    else if ((Failure->DebuggerType()) == PROMPT_NTSD)
        {
        DWORD excess =0;
        
        //   0     id: 2bc name: av.exe
        if (!(Failure->SendCommand("|\n", &buffer)))
            return FALSE;
        
        numLines = GetLinesFromString(buffer, LineHeads);
        
        for (i=0; i < numLines; i++)
            {
            if (sscanf(LineHeads[i], " %x id: %x name: %s", &excess, &excess, 
                tempName) == 3)
                {
                strcpy(Name, tempName);
                free(buffer);
                return TRUE;
                }
            }
        free(buffer);
        }
    
    return FALSE;
    }



BOOL GetExceptionBuffer(ULARGE_INTEGER Exr, ULARGE_INTEGER Cxr)
    {
    CHAR TempBuff[256];
    
    if ((Exr.QuadPart == 0)||(Cxr.QuadPart == 0))
        return FALSE;
    
    sprintf(TempBuff, 
        "first, enter !exr %I64x for the exception record\nnext, enter !cxr %I64x for the context\nfollowed by kb to get the faulting stack\n!cxr will reset to the original context\n", 
        Exr.QuadPart, Cxr.QuadPart);
    
    Failure->AppendMisc(TempBuff);
    return TRUE;
    }


BOOL GetThreadHoldingCriticalSectionStack()
    {
    ULARGE_INTEGER HoldingTID = {0,0};
    ULARGE_INTEGER Address = {0,0};
    ULARGE_INTEGER Thread = {0,0};
    CHAR TempCommand[32];
    
    // Get the address from the stack
    if (!GetParamFromStackBuffer("ntdll!RtlpWaitForCriticalSection", 
								(DWORD) 1, 
                                &Address))
                                return FALSE;
    
    // Get the tid from the address
    if (!GetDxLongNumber(Address, (DWORD)4, &HoldingTID))
        return FALSE;
    
    // Get the thread address of the TID
    if (!GetThreadNumberOfCurrentProcTID(HoldingTID, &Thread))
        return FALSE;
    
    // Avoid Thread waiting on themselves
    // Format the command string
    sprintf(TempCommand, "!thread %I64x\n", Thread.QuadPart);
    if (!strcmp(Failure->StackCommand(), TempCommand))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetThreadHoldingLoaderLockStack()
    {
    ULARGE_INTEGER Thread = {0,0};
    CHAR TempCommand[32];
    
    if (!GetThreadHoldingLoaderLock(&Thread))
        return FALSE;
    
    sprintf(TempCommand, "!thread %I64x\n", Thread.QuadPart);
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetThreadHoldingRtlCriticalSectionStack()
    {
    ULARGE_INTEGER HoldingTID = {0,0};
    ULARGE_INTEGER Address = {0,0};
    ULARGE_INTEGER Thread = {0,0};
    CHAR TempCommand[32];
    CHAR *TempBuffer = NULL;
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    // Get the address from the stack
    if (!GetParamFromStackBuffer("ntdll!RtlAcquireResourceExclusive", 
        (DWORD) 3, 
        &Address))
        return FALSE;
    
    // Get the tid from the address
    if (!GetDxLongNumber(Address, (DWORD)12, &HoldingTID))
        return FALSE;
    
    // Get the thread address of the TID
    if (!GetThreadNumberOfCurrentProcTID(HoldingTID, &Thread))
        return FALSE;
    
    // Avoid Thread waiting on themselves
    // Format the command string
    sprintf(TempCommand, "!thread %I64x\n", Thread.QuadPart);
    if (!strcmp(Failure->StackCommand(), TempCommand))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetLPCMessageFromBuffer(const CHAR *buffer, DWORD *Message)
    {
    CHAR *tptr = NULL;
    
    if ((buffer == NULL)||(Message == NULL))
        return FALSE;
    
    if (!(tptr = strstr(buffer, "LPC MessageId")))
        return FALSE;
    
    if (sscanf(tptr, "LPC MessageId %8x:", Message) == 1)
        return TRUE;
    
    return FALSE;
    }

BOOL GetServerThreadFromBuffer(CHAR *buffer, DWORD *Thread)
    {
    CHAR *tptr = NULL;
    
    if ((buffer == NULL)||(Thread == NULL))
        return FALSE;
    
    if (!(tptr = strstr(buffer, "Server thread")))
        return FALSE;
    
    if (sscanf(tptr, "Server thread %8x", Thread) == 1)
        return TRUE;
    
    return FALSE;
    }


BOOL GetLPCThreadStack()
    {
    DWORD MessageId = 0;
    DWORD Thread = 0;
    CHAR TempCommand[32];
    CHAR *TempBuffer = NULL;
    
    if (!GetLPCMessageFromBuffer(Failure->StackBuffer(), &MessageId))
        return FALSE;
    
    // Format the command string
    sprintf(TempCommand, "!lpc message %x\n", MessageId);
    
    // Get our message id
    if (!Failure->SendCommand(TempCommand, &TempBuffer))
        return FALSE;
    
    if (!GetServerThreadFromBuffer(TempBuffer, &Thread))
        {
        free(TempBuffer);
        return FALSE;
        }
    
    free(TempBuffer);
    return Failure->SendBangThreadStack(Thread);   
    }


BOOL GetMutantThreadFromBuffer(DWORD *Thread)
    {
    CHAR *tptr = NULL;
    
    if (((Failure->StackBuffer()) == NULL)||(Thread == NULL))
        return FALSE;
    
    if (!(tptr = strstr(Failure->StackBuffer(), "Mutant - owning thread")))
        return FALSE;
    
    if (sscanf(tptr, "Mutant - owning thread %8x", Thread) == 1)
        return TRUE;
    
    return FALSE;
    }

BOOL GetMutantThreadOwnerStack()
    {
    DWORD Thread = 0;
    CHAR *TempBuffer = NULL;
    
    if (!GetMutantThreadFromBuffer(&Thread))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetThreadHoldingResourceStack()
    {
    DWORD Lock = 0, Object = 0;
    DWORD Thread = 0;
    CHAR TempCommand[32];
    CHAR *TempBuffer = NULL;
    
    // Lets try working from the lock first
    if (GetParamFromStackBuffer("!ExAcquireResourceExclusiveLite", 
								(DWORD) 3, 
                                &Lock))
        {
        if (!GetThreadHoldingLock(Lock, &Thread))
            {
            Lock = 0;
            }
        }
    
    
    if (!Lock)
        {
        if (!GetParamFromStackBuffer("!ExpAcquireResourceExclusiveLite", 
            (DWORD) 3, 
            &Object))
            return FALSE;
        
        if (!GetDxLongNumber(Object, (DWORD)7, &Thread))
            return FALSE;
        }
    
    // Avoid Thread waiting on themselves
    // Format the command string
    sprintf(TempCommand, "!thread %x\n", Thread);
    if (!strcmp(Failure->StackCommand(), TempCommand))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetThreadHoldingSharedResourceStack()
    {
    DWORD Lock = 0, Object = 0;
    DWORD Thread = 0;
    CHAR TempCommand[32];
    CHAR *TempBuffer = NULL;
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    // Lets try working from the lock first
    if (GetParamFromStackBuffer("!ExAcquireResourceSharedLite", 
								(DWORD) 3, 
                                &Lock))
        {
        if (!GetThreadHoldingLock(Lock, &Thread))
            {
            Lock = 0;
            }
        }
    
    
    if (!Lock)
        {
        if (!GetParamFromStackBuffer("!ExAcquireResourceSharedLite", 
            (DWORD) 3, 
            &Object))
            return FALSE;
        
        if (!GetDxLongNumber(Object, (DWORD)7, &Thread))
            return FALSE;
        }
    
    // Avoid Thread waiting on themselves
    // Format the command string
    sprintf(TempCommand, "!thread %x\n", Thread);
    if (!strcmp(Failure->StackCommand(), TempCommand))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetThreadHoldingVerifierResourceStack()
    {
    DWORD Lock = 0, Object = 0;
    DWORD Thread = 0;
    CHAR TempCommand[32];
    CHAR *TempBuffer = NULL;
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    // Lets try working from the lock first
    if (GetParamFromStackBuffer("!VerifierExAcquireResourceExclusive", 
								(DWORD) 3, 
                                &Lock))
        {
        if (!GetThreadHoldingLock(Lock, &Thread))
            {
            Lock = 0;
            }
        }
    
    
    if (!Lock)
        {
        if (!GetParamFromStackBuffer("!VerifierExAcquireResourceExclusive", 
            (DWORD) 3, 
            &Object))
            return FALSE;
        
        if (!GetDxLongNumber(Object, (DWORD)7, &Thread))
            return FALSE;
        }
    
    // Avoid Thread waiting on themselves
    // Format the command string
    sprintf(TempCommand, "!thread %x\n", Thread);
    if (!strcmp(Failure->StackCommand(), TempCommand))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }


BOOL GetInteruptVectorBuffer(DWORD IV, CHAR **outbuffer)
    {
    CHAR TempCommand[256];
    CHAR *buffer = NULL;
    CHAR *pszTemp = NULL;
    
    sprintf(TempCommand, "!idt %x\n", IV);
    
    if (!Failure->SendCommand(TempCommand, &buffer, TIMEOUTPERIOD*2))
        return FALSE;
    
    if (strstr(buffer, "Unable to load"))
        {
        free(buffer);
        return FALSE;
        }
    
    sprintf(TempCommand, "%x", IV);
    
    pszTemp = (strstr(buffer, "kd>") - 1);
    
    while ((pszTemp[0] != '\n')&&(pszTemp > buffer))
        pszTemp--;
    pszTemp[0] = '\0';
    
    pszTemp = strstr(buffer, TempCommand);
    
    *outbuffer = _strdup(pszTemp);
    free(buffer);
    
    if (*outbuffer == NULL)
        return FALSE;
    
    return TRUE;
    }


BOOL TryToAssignVectorBuffer(CHAR *inbuffer, CHAR *Followup, DWORD FollowupSize)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    CHAR *pszTemp = NULL;
    
    if (inbuffer == NULL)
        return FALSE;
    
    buffer = _strdup(inbuffer);
    
    if (strstr(inbuffer, "Hal Internal: "))
        pszTemp = strstr(inbuffer, "Hal Internal: ") + strlen("Hal Internal: ");
    
    if (pszTemp)
        {
        if (GetOwner(pszTemp, Followup, FollowupSize))
            {
            free(buffer);
            return TRUE;
            }
        }
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    if (numLines == 2)
        {
        pszTemp = LineHeads[1];
        
        while ((pszTemp[0] == ' ')||(pszTemp[0] == '\t'))
            pszTemp++;
        
        if (GetOwner(pszTemp, Followup, FollowupSize))
            {
            free(buffer);
            return TRUE;
            }		
        }
    else
        {
        strcpy(Followup, "arbiters");
        free(buffer);
        return TRUE;
        }	
    
    return FALSE;
    }



BOOL DoSpecialCasesBasedOnStack(CHAR *Followup, DWORD FollowupSize)
    {
    BOOL Again = FALSE;
    BOOL bRet = FALSE;
    CHAR *buffer = NULL;
    
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    do 
        {
        if (IsStuckInChainedDispatch())
            {
            DWORD dwInteruptVector = 0;
            
            if (GetParamFromStackBuffer("!KiChainedDispatch", 2, &dwInteruptVector))
                {
                if (GetInteruptVectorBuffer(dwInteruptVector, &buffer))
                    {
                    bRet = TryToAssignVectorBuffer(buffer, Followup, FollowupSize);
                    
                    _strlwr(Followup);
                    if (!strcmp(Followup, "ignore"))
                        bRet = FALSE;
                    
                    if (!strcmp(Followup, "arbiters"))
                        {
                        Failure->AppendMisc(ABITER_MSG);
                        Failure->AppendMisc("\r\n");
                        Failure->AppendMisc(buffer);
                        SAFEFREE(buffer);
                        }
                    }
                }			
            }
        } while (Again);
        
        return bRet;
    }



DWORD GetDeeperStacks()
    {
    BOOL Again = FALSE;
    DWORD BadAddress = 0;
    CHAR Name[128];
    BOOL ExcludeW32Crit = FALSE;
    DWORD dwLoopCount = 0;
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    do 
        {
        dwLoopCount++;
        
        Again = FALSE;
        
        // Check for critical sections
        if (IsWaitingOnCriticalSection())
            {
            if (!GetThreadHoldingCriticalSectionStack())
                break;
            Again = TRUE;
            }
        
        //This must go before IsAquireResourceExclusive
        if ((IsContaingW32EnterCritical())&&(!ExcludeW32Crit))
            {
            if (!GetGPResUserStack())
                break;
            ExcludeW32Crit = TRUE;
            Again = TRUE;
            }
        
        if ((IsRtlEnterCriticalSection()) && (GetProcessName(Name)))
            {
            if (!strcmp(Name, "csrss.exe"))
                {
                if (!GetGPResUserStack())
                    break;
                Again = TRUE;
                }
            }
        
        //This must also go before IsAquireResourceExclusive
        if (IsGreLockDisplay())
            {
            if (!GetLockDisplayStack())
                break;
            Again = TRUE;
            }
        
        if (IsVerifierAquireResource())
            {
            if (!GetThreadHoldingVerifierResourceStack())
                break;
            Again = TRUE;
            }
        
        if (IsRtlAquireResourceExclusive())
            {
            if (!GetThreadHoldingRtlCriticalSectionStack())
                break;
            Again = TRUE;
            }
        
        if (IsAquireResourceExclusive())
            {
            if (!GetThreadHoldingResourceStack())
                break;
            Again = TRUE;
            }
        
        if (IsAquireResourceShared())
            {
            if (!GetThreadHoldingSharedResourceStack())
                break;
            Again = TRUE;
            }
        
        
        // Make sure we done have that Mutant Stuff
        if (IsAMutant())
            {
            if (!GetMutantThreadOwnerStack())
                break;
            Again = TRUE;
            }
        
        // Make sure we done have that LPC Stuff
        if (IsWaitingOnLPC())
            {
            if (!GetLPCThreadStack())
                break;
            Again = TRUE;
            }
        
        // Check for critical sections
        if (IsWaitingOnLoaderLock())
            {
            if (!GetThreadHoldingLoaderLockStack())
                break;
            Again = TRUE;
            }
        
        } while ((Again) && (dwLoopCount < (DEEPER_LOOPCOUNT + 1)));
        
        if ((dwLoopCount - 1) == DEEPER_LOOPCOUNT)
            Failure->AppendMisc("WARNING:May be a circular dependancy!!!\n");
        
        return dwLoopCount - 1;
}



BOOL GetResTimeoutThreadFromBuffer(DWORD *TheThread)
    {
    const CHAR *pResStart = Failure->BackSpew();
    CHAR *pTempPtr = NULL;
    
    if (TheThread == NULL)
        return FALSE;
    
    while (pTempPtr = strstr(pResStart, "Resource @"))
        {
        pResStart = pTempPtr;
        pResStart++;
        }
    
    if (pResStart == Failure->BackSpew())
        return FALSE;
    
    *TheThread = 0;
    
    while ((*TheThread) < 1000)
        {
        if (!(pResStart = strstr(pResStart, "Thread = ")))
            return FALSE;
        
        if (!sscanf(pResStart, "Thread = %8x", TheThread))
            return FALSE;
        
        pResStart++;
        }
    
    if (*TheThread == 0)
        return FALSE;
    else
        return TRUE;
    }

BOOL GetGPResUserStack()
    {
    int loops = 0;
    CHAR *buffer = NULL;
    DWORD Lock = 0;
    DWORD Thread = 0;
    
    if ((Failure->DebuggerType()) == PROMPT_NTSD)
        {
        if (!Failure->DoKernelBreakin())
            return FALSE;
        }
    
    if (!GetGPResUser(&Lock))
        {
        Failure->SendCommand("!locks\n", NULL, 0);
        return FALSE;
        }
    
    if (!GetThreadHoldingLock(Lock, &Thread))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

BOOL GetLockDisplayStack()
    {
    DWORD Lock = 0;
    DWORD Address = 0;
    DWORD Thread = 0;
    CHAR *buffer = NULL;
    
    if ((Failure->StackBuffer()) == NULL)
        return FALSE;
    
    if (!GetParamFromStackBuffer("win32k!GreLockDisplay", 
								(DWORD) 3, 
                                &Address))
                                return FALSE;
    
    // Get the display lock here
    if (!GetDxLongNumber(Address, (DWORD) 11, &Lock))
        return FALSE;
    
    if (!GetThreadHoldingLock(Lock, &Thread))
        return FALSE;
    
    return Failure->SendBangThreadStack(Thread);
    }

// Pid.Tid 7c.444, owner tid 3b8 Critical Section 00C333D8 - ContentionCount == 119
BOOL GetCriticalSectionInfoFromBuffer(DWORD *Pid, DWORD *OwnerTid)
    {
    const CHAR *pCritStart = Failure->BackSpew();
    CHAR *pTempPtr = NULL;
    DWORD Garb = 0, dwRet = 0;
    
    while (pTempPtr = strstr(pCritStart, "Pid.Tid "))
        {
        pCritStart = pTempPtr;
        pCritStart++;
        }
    
    if (pTempPtr == NULL)
        return FALSE;
    
    if (3 != sscanf((pCritStart - 1), "Pid.Tid %x.%x, owner tid %x", 
        Pid, &Garb, OwnerTid))
        return FALSE;
    
    return TRUE;
    }


BOOL GetUserThreadId(DWORD Pid, DWORD Tid, DWORD *UserThreadId)
    {
    CHAR *buffer;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0, dwRet = 0;
    DWORD tmpTid = 0, tmpPid = 0;
    
    if (!(Failure->SendCommand("~\n", &buffer)))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0;i<numLines;i++) 
        {
        if (3 == sscanf(LineHeads[i], "%u  id: %x.%x ", UserThreadId, &tmpPid, &tmpTid))
            {
            if ((tmpTid == Tid)&&(tmpPid == Pid))
                {
                free(buffer);
                return TRUE;
                }
            }
        }
    
    *UserThreadId = 0;
    free(buffer);
    return FALSE;
    }

// 2: kd> !dso kmutant ownerthread nt!ObpInitKillMutant
// Structure KMUTANT - Size: 0x20
// Field: OwnerThread - Offset: 0x18
// Address   Value
// 804748F8  82de1db0 -- if zero, the mutant is available.
BOOL GetKMutantOwnerThread(DWORD *Thread)
    {
    CHAR *buffer;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0, Garb = 0;
    
    if ((Failure->DebuggerType()) == PROMPT_NTSD)
        {
        if (!Failure->DoKernelBreakin())
            return FALSE;
        }
    
    if (!(Failure->SendCommand("!dso kmutant ownerthread nt!ObpInitKillMutant\n", &buffer)))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0;i<numLines;i++) 
        {
        if (2 == sscanf(LineHeads[i], "%8x %8x -- if", &Garb, Thread))
            {
            free(buffer);
            return TRUE;
            }
        }
    free(buffer);
    return FALSE;
    }

BOOL GetStringAtAddress(ULARGE_INTEGER Address, CHAR *StringBuffer, size_t StringLen, BOOL fUnicode)
    {
    CHAR TempLine[33];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    
    StringBuffer[0] = '\0';
    
    // Format the command string
    if (fUnicode == TRUE)
        sprintf(TempLine, "du %I64x\n", Address.QuadPart);
    else
        sprintf(TempLine, "da %I64x\n", Address.QuadPart);
    
    // Get our new stack
    if (!Failure->SendCommand(TempLine, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0;i<numLines;i++) 
        {
        if (ExtractString(LineHeads[i], TempLine, 33))
            strncat(StringBuffer, TempLine, (StringLen - strlen(StringBuffer)));
        else
            break;
        }
    
    StringBuffer[StringLen - 1] = '\0';
    
    free(buffer);
    return TRUE;
    }

BOOL ExtractString(CHAR *InString, CHAR *OutString, size_t OutLen)
    {
    CHAR StringData[33];
    DWORD addr = 0;
    CHAR *first = NULL;
    
    first = strchr(InString, '"');
    if (first != NULL)
        {
        strncpy(OutString, (first + 1), OutLen);
        OutString[OutLen - 1] = '\0';
        if (first = strrchr(OutString, '"'))
            first[0] = '\0';
        }
    else
        {
        if (2 != sscanf(InString, "%8x  \"%s\"", &addr, StringData))
            return FALSE;
        
        if (StringData[strlen(StringData) - 1] == '"')
            StringData[strlen(StringData) - 1] = '\0';
        
        strncpy(OutString, StringData, OutLen);
        OutString[OutLen - 1] = '\0';
        }
    
    return TRUE;
    }
