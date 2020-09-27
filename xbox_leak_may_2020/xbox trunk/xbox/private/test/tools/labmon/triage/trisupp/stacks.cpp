#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "getutils.h"
#include "stacks.h"

extern Cfailure *Failure;

DWORD GetDepthNeeded(DWORD numlines)
    {
    if (numlines > 10)
        return 8;
    if (numlines == 10)
        return 7;
    if (numlines == 9)
        return 7;
    if (numlines == 8)
        return 6;
    if (numlines == 7)
        return 5;
    if (numlines == 6)
        return 5;
    if (numlines == 5)
        return 4;
    if (numlines == 4)
        return 3;
    
    return numlines;
    }

BOOL DoesLineNeedReload(CHAR *Line, ULARGE_INTEGER *BadAddress)
    {
    
    if ((!_strnicmp(Line, "0x", 2)) ||
        (!_strnicmp(Line, "+0x", 3)))
        {
        if ((Failure->Architecture()) == MACH_X86)
            {
            if ((sscanf(Line, "0x%x", &(BadAddress->LowPart)))||
                (sscanf(Line, "+0x%x", &(BadAddress->LowPart))))
                return TRUE;
            }
        else
            {
            if ((2 == sscanf(Line, "0x%x`%x", &(BadAddress->HighPart), &(BadAddress->LowPart)))||
                (2 == sscanf(Line, "+0x%x`%x", &(BadAddress->HighPart), &(BadAddress->LowPart))))
                return TRUE;
            
            }
        }
    
    return FALSE;
    }

BOOL DoesStackNeedManualReload(Cfailure * Fail, ULARGE_INTEGER* BadAddress)
    {
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    DWORD DepthNeeded = 0;
    ULARGE_INTEGER NewBad = {0};
    BOOL SeenBefore = FALSE;
    
    if ((Fail->StackBuffer()) == NULL)
        return FALSE;
    
    if (!(buffer = _strdup(Fail->StackBuffer())))	
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    StripStack(LineHeads, &numLines);
    
    DepthNeeded = GetDepthNeeded(numLines);
    
    for (i=0; i < DepthNeeded; i++)
        {
        if ((strstr(LineHeads[i], "ntdll!RtlpWaitForCriticalSection"))||
            (strstr(LineHeads[i], "!ExpAcquireResourceExclusiveLite"))||
            (strstr(LineHeads[i], "!ExAcquireResourceExclusiveLite")))
            {
            BadAddress->QuadPart = 0;
            free(buffer);
            return FALSE;
            }
        
        if (DoesLineNeedReload(LineHeads[i], &NewBad))
            {
            //Avoid printscreen stack stuff
            if (NewBad.QuadPart == 0xffffffff)
                {
                BadAddress->QuadPart = 0;
                free(buffer);
                return FALSE;
                }
            
            
            if (BadAddress->QuadPart == NewBad.QuadPart)
                {
                //Avoid stacks with repeated lines
                if (SeenBefore)
                    {
                    BadAddress->QuadPart = 0;
                    free(buffer);
                    return FALSE;
                    }
                else
                    SeenBefore = TRUE;
                }
            else
                {
                BadAddress->QuadPart = NewBad.QuadPart;
                free(buffer);
                return TRUE;
                }
            }
        }
    
    BadAddress->QuadPart = 0;
    free(buffer);
    return FALSE;
    }

BOOL TrySomeKnownAddresses(Cfailure * Fail, ULARGE_INTEGER Address, CHAR *DllName, ULARGE_INTEGER *Base)
    {
    Base->QuadPart = 0;

    // xbox addresses
    if ((Address.QuadPart > 0x80100000) && (Address.QuadPart < 0x801CABE0))
        {
        strcpy(DllName, "xboxkrnl.exe");
        return TRUE;
        }
    if ((Address.QuadPart > 0xd002a000) && (Address.QuadPart < 0xd0045520))
        {
        strcpy(DllName, "xbdm.dll");
        return TRUE;
        }
    if ((Address.QuadPart > 0xd005d000) && (Address.QuadPart < 0xd005d1e0))
        {
        strcpy(DllName, "cydrive.dxt");
        return TRUE;
        }


    /*
    if (Fail->Architecture() == MACH_X86)
        {
        if ((Fail->BuildType()) == BUILD_FREE)
            {
            if ((Address.QuadPart > 0x77f00000) && (Address.QuadPart < 0x78000000))
                {
                strcpy(DllName, "ntdll.dll");
                return TRUE;
                }
            if ((Address.QuadPart > 0x77e00000) && (Address.QuadPart < 0x77f00000))
                {
                strcpy(DllName, "kernel32.dll");
                return TRUE;
                }
            if ((Address.QuadPart > 0x77d00000) && (Address.QuadPart < 0x77e00000))
                {
                strcpy(DllName, "rpcrt4.dll");
                return TRUE;
                }
            if ((Address.QuadPart > 0x77c00000) && (Address.QuadPart < 0x77d00000))
                {
                strcpy(DllName, "clbcatq.dll");
                return TRUE;
                }
            }
        else
            {
            if ((Address.QuadPart > 0x77f00000) && (Address.QuadPart < 0x78000000))
                {
                strcpy(DllName, "ntdll.dll");
                return TRUE;
                }
            if ((Address.QuadPart > 0x77e00000) && (Address.QuadPart < 0x77f00000))
                {
                strcpy(DllName, "kernel32.dll");
                return TRUE;
                }
            if ((Address.QuadPart > 0x77c50000) && (Address.QuadPart < 0x77e00000))
                {
                strcpy(DllName, "rpcrt4.dll");
                return TRUE;
                }
            if ((Address.QuadPart > 0x77c00000) && (Address.QuadPart < 0x77c50000))
                {
                strcpy(DllName, "clbcatq.dll");
                return TRUE;
                }
            }
        
        if ((Address.QuadPart > 0xa0000000) && (Address.QuadPart < 0xa1000000))
            {
            strcpy(DllName, "win32k.sys");
            return TRUE;
            }
        }
        */
    return FALSE;
    }

BOOL GetDllAndBase(Cfailure * Fail, ULARGE_INTEGER Address, CHAR *DllName, ULARGE_INTEGER *Base)
    {
    CHAR TheString[32];
    CHAR *buffer = NULL;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD numLines = 0, i = 0;
    DWORD tempBase = 0;
    ULARGE_INTEGER excess = {0};
    CHAR tempDllName[MAX_PATH];
    
    if ((Address.QuadPart == 0)||(DllName == NULL)||(Base == NULL))
        return FALSE;
    
    Base->QuadPart = 0;
    DllName[0] = '\0';
    
    if ((Address.QuadPart > 0x1000000) && (Address.QuadPart < 0x2000000))
        {
        GetProcessName(DllName);
        Base->QuadPart = 0x1000000;
        return TRUE;
        }
    
    sprintf(TheString, "!dlls -c %I64x\n", Address.QuadPart);
    
    if (!Fail->SendCommand(TheString, &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if (sscanf(LineHeads[i], " Base 0x%8x", &tempBase) == 1)
            {
            if (i > 0)
                {
                if (sscanf(LineHeads[i-1], "0x%I64x: %s", &(excess.QuadPart), tempDllName) == 2)
                    {
                    CHAR *tptr = NULL;
                    
                    tptr =strrchr(tempDllName, '\\');
                    if (tptr)
                        {
                        strcpy(DllName, (tptr + 1));
                        Base->QuadPart = tempBase;
                        free(buffer);
                        return TRUE;
                        }
                    }
                }
            }
        }
    free(buffer);
    
    return TrySomeKnownAddresses(Fail, Address, DllName, Base);
    }

VOID StripStack(CHAR **StackLines, DWORD *numLines)
    {
    DWORD i = 0, next = 0, firstLine = 0;
    CHAR *symStart = NULL;
    
    if (*numLines == 0)
        return;
    
    for (i = 0; i < *numLines; i++) 
        {
        if ((strstr(StackLines[i], "0x"))||
            (strstr(StackLines[i], "except_handler3"))||
            (strstr(StackLines[i], "Kernel stack not resident."))||
            (strstr(StackLines[i], "ProgressWndProc")))
            {
            if ((symStart = strstr(StackLines[i], "+0x"))||
                (symStart = strstr(StackLines[i], "except_handler3"))||
                (symStart = strstr(StackLines[i], "ProgressWndProc"))) 
                {
                // MSCTF!CStub__`scalar deleting destructor'
                if (strchr(StackLines[i], '!'))
                    symStart = strchr(StackLines[i], '!');
                
                while ((symStart > StackLines[i]) && 
                    (*symStart != ' ')) 
                    {
                    symStart--;
                    }
                
                if (*symStart == ' ') 
                    {
                    symStart++;
                    }
                
                StackLines[i] = symStart;
                
                // bad:5d5ec68b 00000000 00000000 00000000 00000000 0xffffffff`8b0020c2
                // good: MSCTF!CStub__`scalar deleting destructor'+0x8
                if ((symStart = strchr(StackLines[i], '`')) && (!strchr(StackLines[i], '!')))
                    {	    
                    if ((Failure->Architecture()) == MACH_X86)
                        symStart[0] = '\0';
                    }
                
                if (symStart = strstr(StackLines[i], " (FPO"))
                    symStart[0] = '\0';
                if (symStart = strstr(StackLines[i], " ["))
                    symStart[0] = '\0';
                
                }
            else if (symStart = strstr(StackLines[i], "Kernel stack not resident."))
                {
                StackLines[i] = symStart;
                //                for (i =  i + 1; i < *numLines; i++)
                //                    StackLines[i] = NULL;
                }
            else if (symStart = strstr(StackLines[i], "0x"))
                {
                DWORD dwValue = 0;
                
                if ((symStart - 9) < StackLines[i])
                    {
                    StackLines[i] = NULL;
                    continue;
                    }
                else if (stristr(StackLines[i], "Start Address"))
                    {
                    StackLines[i] = NULL;
                    continue;
                    }
                else
                    {
                    if (1 != sscanf((symStart - 9), "%8x", &dwValue))
                        {
                        StackLines[i] = NULL;
                        continue;
                        }
                    }
                
                StackLines[i] = symStart;
                if (symStart = strchr(StackLines[i], '`'))
                    StackLines[i] = (symStart + 1);
                if (symStart = strstr(StackLines[i], " (FPO"))
                    symStart[0] = '\0';
                if (symStart = strstr(StackLines[i], " ["))
                    symStart[0] = '\0';
                if (symStart = strchr(StackLines[i], ' '))
                    symStart[0] = '\0';
                
                if (1 == sscanf(StackLines[i], "0x%x", &dwValue))
                    {
                    if (dwValue < 1000000)
                        StackLines[i] = NULL;
                    }
                }
            else
                StackLines[i] = NULL;
            }
        else
            StackLines[i] = NULL;
        }
    
    // Not overly eliquent but it compacts the linelist
    // make more efficient when you have time
    for (i = 0; i < *numLines; i++) 
        {
        if (StackLines[i] != NULL)
            {
            StackLines[next] = StackLines[i];
            if (i != next)
                StackLines[i] = NULL;
            next++;
            }
        }
    *numLines = next;
    
    firstLine = 0;
    for (i = 0; i < *numLines; i++)
        {
        // this a kludge added for JohnC.  We must return on this because
        // if there is a Trap0E below it we will lose this line.
        if (strstr(StackLines[i], "!MiWaitForInPageComplete"))
            {
            firstLine = i;
            break;
            }
        
        // this a kludge added for GerardoB for popup failures.
        if (strstr(StackLines[i], "ole32!Win4AssertEx"))
            {
            firstLine = i+1;
            break;
            }
        
        if (strstr(StackLines[i], "!KiTrap0E"))
            {
            if ((i == 0)||(!strstr(StackLines[i-1], "!MmAccessFault")))
                {
                firstLine = i+1;
                break;
                }
            }
        
        //  If dont return on these ones because messageBoxA and W
        // seem to come in bunches and I want to get to the lowest one
        // With trap0e sometimes there are 2 and I want to stop at the top one
        // not necessarily with this one but lets try it and see
        // who screams
        if (strstr(StackLines[i], "USER32!MessageBoxA+"))
            {
            firstLine = i+1;
            }
        
        if (strstr(StackLines[i], "USER32!MessageBoxW+"))
            {
            firstLine = i+1;
            }
        }
    
    if (firstLine)
        {
        next = 0;
        for (i=firstLine; i < *numLines; i++)
            {
            StackLines[next++]=StackLines[i];
            }
        *numLines = *numLines - firstLine;
        }
}



//
// Allocates memory for and stores the failures stack
//
CHAR *StoreStack(CHAR **Lines, DWORD numLines)
    {
    UINT i = 0;
    DWORD numBytes = 1;
    CHAR *stackbuff = NULL;
    
    if ((numLines > MAXLINES) || (Lines[0] == NULL))
        {
        return NULL;
        }
    
    for (i = 0; i < numLines; i++) 
        {
        if (Lines[i] != NULL)
            numBytes += strlen(Lines[i])+2;
        }
    
    stackbuff=(CHAR *)malloc(numBytes);
    stackbuff[0]='\0';
    
    for (i = 0; i < numLines; i++) 
        {
        if (Lines[i] != NULL)
            {
            strcat(stackbuff, Lines[i]);
            if (i < numLines - 1) 
                {
                strcat(stackbuff, "\n");
                }
            }
        }
    return stackbuff;
    }


BOOL GetRepairedStack(Cfailure *Fail, ULARGE_INTEGER Address)
    {
    CHAR DllName[MAX_PATH];
    CHAR SendString[MAX_PATH];
    CHAR AddressString[11];
    ULARGE_INTEGER Base = {0};
    CHAR *buffer = NULL;
    
    
    if (!GetDllAndBase(Fail, Address, DllName, &Base))
        return FALSE;
    
    if (Base.QuadPart == 0)
        sprintf(SendString, "!reload %s\n", DllName);
    else
        sprintf(SendString, "!reload %s=%I64x\n", DllName, Base.QuadPart);
    
    if (!Fail->SendCommand(SendString, NULL))
        return FALSE;
    
    if (!Fail->SendStackCommand(NULL, FALSE, FALSE))
        return FALSE;
    
    sprintf(AddressString, "0x%I64x", Address.QuadPart);
    
    if (strstr(Fail->StackBuffer(), AddressString))
        return FALSE;
    
    return TRUE;
    }


