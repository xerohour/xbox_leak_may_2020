#include <windows.h>
#include "rmtcli.h"
#include "utils.h"
#include "triage.h"
#include "stacks.h"
#include "failure.h"

VOID Cfailure::AppendDescription(CHAR *Addition)
    {
    DynamicAppend(&pszDescription, Addition);
    }

VOID Cfailure::AppendMisc(CHAR *Addition)       
    {
    DynamicAppend(&pszMisc, Addition);
    }

BOOL Cfailure::ShutOffVerbose()
    {
    CHAR *pszBuffer = NULL;
    BOOL fReturn = TRUE;

    /* // commented out because verbose is off by default on xbox
    // '\026' gives you a ctrl-v character
    fReturn = SendMyCommand("\026\n", &pszBuffer, 20);
    if (pszBuffer == NULL)
    {
    return FALSE;
    }
    
      if (strstr(pszBuffer, " ON"))
      {
      free(pszBuffer);
      SendMyCommand("\026\n", &pszBuffer, 20);
      }
      free(pszBuffer);

    // Lets turn off that annoying noist symbols stuff!!!
    SendMyCommand("!sym quiet\n", NULL, 30);
    */
    
    return fReturn;
    }

BOOL Cfailure::DoAResync()
    {
    BOOL fReturn = TRUE;
    CHAR *pszBuffer = NULL;
    
    SendMyCommand("\04\n", NULL, 5);
    
    // '\022' gives you a ctrl-r character in octal
    if (!SendMyCommand("\022\n", &pszBuffer, 20))
        {
        SendMyCommand("\04\n", NULL, 5);
        return FALSE;
        }
    
    if (pszBuffer != NULL)
        {
        if (stristr(pszBuffer, "SYNCTARGET: Timeout."))
            fReturn = FALSE;
        
        free(pszBuffer);
        }
    else
        fReturn = FALSE;
    
    SendMyCommand("\04\n", NULL, 5);
    
    return fReturn;
    }

BOOL Cfailure::SetTitleOnLastEvent()
    {
    CHAR *pszBuffer = NULL;
    BOOL fReturn = TRUE;
    /* 
    // no .lastevent on xbox debugger
    fReturn = SendMyCommand(".lastevent\n", &pszBuffer, TIMEOUTPERIOD);
    if ((pszBuffer == NULL) || (!fReturn))
        {
        return FALSE;
        }
    
    if (stristr(pszBuffer, "C000001D"))
        {
        dwFailType = FT_INVALID_INSTRUCTION;
        SetDescription("Invalid Instruction");
        }
    else if (stristr(pszBuffer, "C0000005"))
        {
        dwFailType = FT_ACCESS_VIOLATION;
        SetDescription("Access Violation");
        }
    else if (stristr(pszBuffer, "C0000008"))
        {
        dwFailType = FT_INVALID_HANDLE;
        SetDescription("Invalid Handle");
        }
    else if (stristr(pszBuffer, "C0000096"))
        {
        dwFailType = FT_INVALID_INSTRUCTION;
        SetDescription("PRIVILEGED INSTRUCTION");
        }
    else if (stristr(pszBuffer, "C00000FD"))
        {
        dwFailType = FT_STACK_OVERFLOW;
        SetDescription("Stack OverFlow");
        }
    else if (stristr(pszBuffer, "C0000194"))
        {
        dwFailType = FT_CRITICAL_SECTION_TIMEOUT;
        SetDescription("Critical Section Timeout");
        }
    else
        fReturn = FALSE;
    
    free(pszBuffer);
    return fReturn;
    */
    return FALSE;
    }


VOID Cfailure::FindIniFile(CHAR *pName)
    {
    CHAR szIniFileName[MAX_PATH];
    CHAR *slash = NULL;
    
    // The buffersize - (the length of the filename + "\" + "\n")
    if (GetCurrentDirectory((sizeof(szIniFileName)) - strlen(pName) - 2,
        szIniFileName)) 
        {
        strcat(szIniFileName,"\\");
        strcat(szIniFileName, pName);
        // if its 0xffffffff then we had file not found or another error
        if (GetFileAttributes(szIniFileName) == 0xffffffff) 
            {
            // This will cause it to check the dir the exe is in if they are not the same
            if (GetModuleFileName(NULL, szIniFileName, MAX_PATH)) 
                {
                slash = strrchr(szIniFileName, '\\');
                
                if (slash) 
                    {
                    slash[1]= '\0';
                    strcat(szIniFileName, pName);
                    SetIniFile(szIniFileName);
                    return;
                    }
                }
            } 
        else 
            {
            SetIniFile(szIniFileName);
            return;
            }
        }
    strcpy(szIniFileName, pName);
    SetIniFile(szIniFileName);
    }

Cfailure::Cfailure()
    {
    fConnected          = FALSE;
    pszDebugger         = NULL;
    pszPipe             = NULL;
    
    fFixSymbols         = TRUE;
    pszIniFile          = NULL;
    
    dwFailType          = FT_UNK;
    dwWeBrokeIn         = 0;
    dwDebugger          = PROMPT_UNK;
    
    dwBuild             = 0;
    dwSP                = 0;
    dwBuildDot          = 0;
    dwVBL               = 0;
    fLB6RI              = FALSE;
    pszVBL_Stamp        = NULL;
    dwBuildType         = BUILD_FREE;
    dwArchitecture      = MACH_X86;
    dwCPUType           = CPU_UNKNOWN;
    
    pszFirstBuffer      = NULL;
    pszCurrentStackBuffer    = NULL;
    pszStackCommand     = NULL;
    
    pszDescription      = NULL;
    pszMisc             = NULL;
    pszStack            = NULL;
    
    fIsAssigned         = FALSE;
    pszFollowup         = NULL;
    }

Cfailure::~Cfailure()
    {
    SAFEFREE(pszVBL_Stamp);
    SAFEFREE(pszFirstBuffer);
    SAFEFREE(pszCurrentStackBuffer);
    SAFEFREE(pszStackCommand);;
    SAFEFREE(pszDescription);
    SAFEFREE(pszMisc);
    SAFEFREE(pszStack);
    SAFEFREE(pszFollowup);
    SAFEFREE(pszIniFile);
    
    CloseRemote();
    }

DWORD Cfailure::Init(CHAR *pszNewDebugger, CHAR *pszNewPipe, CHAR *IniFile, DWORD dwFlags)
    {
    DWORD dwReturn = ERROR_SUCCESS;
    
    dwReturn = InitRemote(pszNewDebugger, 
        pszNewPipe, 
        IniFile, 
        dwFlags);
    /*
    if (dwReturn == ERROR_SUCCESS)
        dwReturn = GetSessionData();
    */
    return dwReturn;
    }

DWORD Cfailure::InitRemote(CHAR *pszNewDebugger, CHAR *pszNewPipe, CHAR *IniFile, DWORD dwFlags)
    {
    DWORD dwReturn = ERROR_SUCCESS;
    CHAR *pszBuffer = NULL;
    
    SAFEFREE(pszDebugger);
    SAFEFREE(pszPipe);
    fConnected = FALSE;
    
    fFixSymbols = !(dwFlags & FLAG_NO_SYMBOLS);
    
    FindIniFile(IniFile);
    
    // Connect to the machine
    dwReturn = rmConnectToRemote(pszNewDebugger, pszNewPipe);
    
    if (dwReturn != ERROR_SUCCESS) 
        {
        CHAR szErrCode[8];
        
        _snprintf(szErrCode, sizeof(szErrCode), "%u", GetLastError());
        
        SetFailureType(FT_UNRESPONSIVE);
        SetDescription("Unable to connect");
        SetStack(szErrCode);
        SetFollowup("Machine owner");
        
        return dwReturn;
        }
    else
        {
        pszDebugger = _strdup(pszNewDebugger);
        pszPipe = _strdup(pszNewPipe);
        fConnected = TRUE;
        }
    
    // Lets get the initial screen spew
    SAFEFREE(pszFirstBuffer);
    SendCommand(NULL, &pszFirstBuffer, (time_t)20);
    
    // figure out weather its KD or NTSD
    SendCommand("\n", NULL, (time_t)1); // clear out our connect message so we just see the prompt
    SetPromptType(pszFirstBuffer);
    
    // Asserts need to be broken before we can do anything more with them..
    if (dwDebugger == PROMPT_ASSERT)
        {
        dwFailType = FT_ASSERTION_FAILURE;
        SetDescription("Assertion Failure");
        
        // Not quite sure why I do this?
        SendCommand("\n", NULL, (time_t)1);
        if (SendCommand("b\n", &pszBuffer))
            {
            SendCommand(" \n", NULL);
            SetPromptType(pszBuffer);
            SAFEFREE(pszBuffer);
            }
        
        // See if we can get some assert spew.
        TryForAssertData(); 
        }
    
    // Make sure we have a prompt and there is nothing in the command buffer
    if (!SendMyCommand(" \n", &pszBuffer, 20)) 
        {
        if (!DoAResync())
            {
            SetFailureType(FT_UNRESPONSIVE);
            SetDescription("Not broken into debugger");
            SetStack("Try a ctrl-r");
            SetFollowup("xstress");
            return ERROR_BAD_ENVIRONMENT;
            }
        else
            {
            CHAR *pszTempBuffer = NULL;
            
            if (!SendCommand(" \n", &pszTempBuffer))
                {
                if (NULL != pszTempBuffer)
                    free(pszTempBuffer);
                
                SetFailureType(FT_UNRESPONSIVE);
                SetDescription("Not broken into debugger");
                SetStack("Try a ctrl-r");
                SetFollowup("xstress");
                return ERROR_BAD_ENVIRONMENT;
                }
            
            if (NULL != pszTempBuffer)
                {
                SetPromptType(pszTempBuffer);
                free(pszTempBuffer);
                }
            }
        }
    
    if (dwDebugger == PROMPT_UNK)
        SetPromptType(pszBuffer);
    
    SAFEFREE(pszBuffer);
    
    if (!ShutOffVerbose())
        {
        SetFailureType(FT_UNRESPONSIVE);
        SetDescription("Not broken into debugger");
        SetStack("Try a ctrl-r");
        SetFollowup("xstress");
        return ERROR_BAD_ENVIRONMENT;
        }
    
    // Something strange going on if we can't tell by here
    if (dwDebugger == PROMPT_UNK)
        {
        SetFailureType(FT_UNRESPONSIVE);
        SetDescription("Unknown Prompt");
        SetStack("");
        SetFollowup("xstress");
        return ERROR_BAD_ENVIRONMENT;
        }
    return dwReturn;
}

VOID Cfailure::SetPromptType(CHAR *buffer)
    {
    CHAR *rbuf = buffer;
    CHAR *cbuf = NULL;
    
    if (buffer == NULL)
        return;
    
    cbuf = strrchr(rbuf, '>');
    if (cbuf)
        {
        cbuf--;
        if ((*cbuf) == 'd')
            {
            if (*(--cbuf) == 'k')
                {
                dwDebugger = PROMPT_KD;
                }
            }
        else if (isdigit(*cbuf))
            {
            while (isdigit(*cbuf))
                {
                cbuf--;
                }
            if ((*cbuf) == ':')
                {
                cbuf--;
                if (isdigit(*cbuf))
                    {
                    dwDebugger = PROMPT_NTSD;
                    }
                }
            }
        
        if (dwDebugger != PROMPT_UNK)
            {
            cbuf = strrchr(rbuf, '>') + 1;	
            }
        }
    
    while ((cbuf = strstr(rbuf, "bipt"))||
        (cbuf = strstr(rbuf, "gbwxpft?"))||
        (cbuf = strstr(rbuf, "boipt"))||
        (cbuf = strstr(rbuf, "bizro"))||
        (cbuf = strstr(rbuf, "bizrd"))||
        (cbuf = strstr(rbuf, "Assertion failed")))  
        {
        rbuf = cbuf + 1;
        dwDebugger = PROMPT_ASSERT;
        } 
    }

VOID Cfailure::TryForAssertData()
    {
    CHAR *lowbuff = pszFirstBuffer;
    CHAR *curbuff = NULL;
    CHAR Desc[256] = "\0";
    CHAR xbuff[2048] = "\0";
    CHAR *it = NULL;
    CHAR save;
    
    
    if (pszFirstBuffer == NULL)
        return;    
    
    while (curbuff = stristr(lowbuff, "bipt"))
        {
        lowbuff = curbuff + 1;
        }
    
    while (curbuff = stristr(lowbuff, "boipt"))
        {
        lowbuff = curbuff + 1;
        }
    
    while (curbuff = stristr(lowbuff, "gbwxpft?"))
        {
        lowbuff = curbuff + 1;
        }
    
    while (curbuff = stristr(lowbuff, "Assertion failed"))
        {
        lowbuff = curbuff + 1;
        }

    while (curbuff = stristr(lowbuff, "(bizro)?"))
        {
        lowbuff = curbuff + 1;
        if ((it = strchr(curbuff, '\r'))||(it = strchr(curbuff, '\n')))
            {
            CHAR *startptr = curbuff;
            DWORD Count = 5;
            
            save = *it;
            *it = '\0';
            // rewindto get the previous 4 lines
            while ((startptr >= pszFirstBuffer) && (Count > 0))
                {
                startptr--;
                if (startptr[0] == '\r')
                    Count--;
                }
            
            while ((strchr(curbuff, '\r'))||(strchr(curbuff, '\n')))
                startptr++;
            
            strcpy(xbuff, startptr);
            *it = save;
            }
        } 
    
    // leave this last
    while (curbuff = stristr(lowbuff, "(bizrd)?"))
        {
        lowbuff = curbuff + 1;
        dwFailType = FT_WDM_BUGCHECK;
        SetDescription("WDM Driver Error");
        }
    
    if (strlen(xbuff))
        SetMisc(xbuff);
    }

BOOL Cfailure::GetMeSystemInfo()
    {
    return TRUE;
    CHAR *buffer = NULL;
    CHAR *temp = NULL;
    CHAR *temp2 = NULL;
    
    if (!SendCommand("version\n", &buffer))
        return FALSE;
    
    if ((!strstr(buffer, "Built by:"))&&(!strstr(buffer, "Kernel Version 2195")))
        {
        dwDebugger = PROMPT_NTSD;
        }
    else 
        dwDebugger = PROMPT_KD;
    
    // Get the achitecture
    if (strstr(buffer, "x86 compatible"))
        {
        dwArchitecture = MACH_X86;
        }
    else if (strstr(buffer, "Intel IA64"))
        {
        dwArchitecture = MACH_IA64;
        }
    
    // Checked or free
    if ((strstr(buffer, "Checked Intel")) || (strstr(buffer, "Checked x86")))
        dwBuildType = BUILD_CHECKED;
    
    if (dwDebugger != PROMPT_NTSD)
        {
        if (temp = strstr(buffer, "Kernel Version"))
            {
            if (!(1 == sscanf(temp, "Kernel Version %4d", &dwBuild)))
                dwBuild = 0;	
            }
        
        if (temp = strstr(buffer, "Service Pack"))
            {
            if (temp = strstr(buffer, "RC "))
                {
                if (!(2 == sscanf(temp, "RC %d.%d", &dwSP, &dwBuildDot)))
                    dwBuildDot = 0;
                }
            }
        else if (dwBuild == 2195)
            {
            temp = strstr(buffer, "2195.");
            if (temp)
                {
                if (!(1 == sscanf(temp, "2195.%d\\", &dwBuildDot)))
                    dwBuildDot = 0;
                }
            }
        
        if ((stristr(buffer, "built by:")) && (temp = stristr(buffer, ".lab")))
            {
            CHAR *stuff = NULL;
            temp = temp + 2;
            
            if ((1 == sscanf(temp, "ab%d_n.", &dwVBL)) || (1 == _stscanf(temp, "ab%d_N.", &dwVBL)))
                {
                if (stuff = stristr(temp, "_n."))
                    SetVBLString(stuff + 3);
                else 
                    {
                    dwVBL = 0;
                    fFixSymbols = FALSE;
                    }
                }
            else
                {
                dwVBL = 0;
                fFixSymbols = FALSE;
                }
            }
        else if (stristr(buffer, "LB6RI."))
            {
            CHAR *stuff = NULL;
            
            dwVBL = 6;
            
            if (stuff = strstr(buffer, "LB6RI."))
                {
                SetVBLString(stuff + 6);
                SetLB6RI(TRUE);
                }
            else 
                {
                dwVBL = 0;
                fFixSymbols = FALSE;
                }
            }
        
        }
    else
        {
        if (temp = strstr(buffer, "Build"))
            {
            temp2 = temp + strlen("Build ");
            while ((temp2[0] >= '0') && (temp2[0] <= '9'))
                temp2++;
            temp2[0] = '\0';
            
            if (!(1 == sscanf(temp, "Build %d", &dwBuild)))
                dwBuild = 0;
            }
        else if ((temp = strstr(buffer, "Whistler Version")))
            {
            if (!(1 == sscanf(temp, "Whistler Version %4d", &dwBuild)))
                dwBuild = 0;	
            }
        
        
        if ((stristr(buffer, "kernel32.dll version:"))&&(temp = stristr(buffer, "(lab")))
            {
            CHAR *stuff = NULL;
            
            temp = temp + 2;
            
            if ((1 == sscanf(temp, "ab%d_N.", &dwVBL)) || (1 == sscanf(temp, "ab%d_n.", &dwVBL)))
                {
                if (stuff = stristr(temp, "_n."))
                    SetVBLString(stuff + 3);
                else 
                    {
                    dwVBL = 0;
                    fFixSymbols = FALSE;
                    }
                
                }
            else
                {
                dwVBL = 0;
                fFixSymbols = FALSE;
                }
            }
        else if (stristr(buffer, "LB6RI."))
            {
            CHAR *stuff = NULL;
            
            dwVBL = 6;
            
            if (stuff = strstr(buffer, "LB6RI."))
                {
                SetVBLString(stuff + 6);
                SetLB6RI(TRUE);
                }
            else 
                {
                dwVBL = 0;
                fFixSymbols = FALSE;
                }
            }
        
        }
    
    free(buffer);
    return TRUE;
}

DWORD Cfailure::GetSessionData()
    {
    // Get out machines stats
    if (!GetMeSystemInfo())
        {
        return ERROR_BAD_ENVIRONMENT;
        }
    
   	if (!SendCommand("!cxr\n", NULL))
        {
        return ERROR_BAD_ENVIRONMENT;
        }
    
    return ERROR_SUCCESS;
    }

BOOL Cfailure::CloseRemote() 
    {
    SAFEFREE(pszDebugger);
    SAFEFREE(pszPipe);
    fConnected = FALSE;
    
    return rmCloseRemote();
    }


BOOL Cfailure::SendCommand(const CHAR *pszCommand, CHAR **ppszRetBuffer, time_t tWaitTime)
    {
    BOOL fReturn = TRUE;
    
    if (!fConnected)
        {
        SetDescription("Connection Not Initialized!");
        SetStack("Retry connection or contact xstress");
        SetFollowup("xstress");
        return FALSE;
        }
    
    fReturn = SendMyCommand(pszCommand, ppszRetBuffer, tWaitTime);
    
    if ((fReturn == FALSE) && (pszCommand != NULL) && (0 == dwBuild))
        {
        SetDescription("Debugger Unresponsive");
        SetStack("Try a ctrl-r");
        }
    
    return fReturn;
    }

BOOL Cfailure::SendStackCommand(const CHAR *pszCommand, BOOL fTwoPart, BOOL fDoFixups)
    {
    BOOL fReturn = TRUE;
    CHAR *pszRetBuffer = NULL;
    
    if (pszCommand == NULL)
        {
        if (StackCommand() == NULL)
            {
            SetStackCommand("kb\n");
            fTwoPartCommand = FALSE;
            }
        
        fReturn = SendCommand(StackCommand(), 
            &pszRetBuffer, 
            (dwDebugger == PROMPT_NTSD) ? (time_t)(TIMEOUTPERIOD * 1.5) : TIMEOUTPERIOD);
        }
    else
        {
        fReturn = SendCommand(pszCommand, 
            &pszRetBuffer, 
            (dwDebugger == PROMPT_NTSD) ? (time_t)(TIMEOUTPERIOD * 1.5) : TIMEOUTPERIOD);
        
        
        if (fReturn == TRUE)
            {
            SetStackCommand(pszCommand);
            fTwoPartCommand = fTwoPart;
            }
        }
    
    if ((fReturn == TRUE) && (fTwoPartCommand == TRUE))
        {
        if (pszRetBuffer != NULL)
            SAFEFREE(pszRetBuffer);
        
        if(strncmp(pszCommand, "!cxr", 4) == 0)
            {
            fReturn = SendCommand("!kb\n", 
                &pszRetBuffer, 
                (dwDebugger == PROMPT_NTSD) ? (time_t)(TIMEOUTPERIOD * 1.5) : TIMEOUTPERIOD);
            }
        else
            {
            fReturn = SendCommand("kb\n", 
                &pszRetBuffer, 
                (dwDebugger == PROMPT_NTSD) ? (time_t)(TIMEOUTPERIOD * 1.5) : TIMEOUTPERIOD);
            }
        }
    
    if (fReturn == TRUE)
        {
        SetStackBuffer(pszRetBuffer);
        SAFEFREE(pszRetBuffer);
        
        if (fDoFixups == TRUE)
            DoNecessaryStackFixUps();
        }
    return fReturn;
    }

BOOL Cfailure::SendUserThreadStack(DWORD dwUserThread)
    {
    CHAR command[16];
    
    if (dwDebugger != PROMPT_NTSD)
        return FALSE;
    
    sprintf(command, "~%ukb\n", dwUserThread);
    return SendStackCommand(command, TRUE);
    }

BOOL Cfailure::SendDotThreadStack(DWORD dwThread)
    {
    CHAR command[18];
    
    sprintf(command, ".thread %x\n", dwThread);
    return SendStackCommand(command, TRUE);
    }

BOOL Cfailure::SendBangThreadStack(ULARGE_INTEGER dwThread)
    {
    CHAR command[26];
    
    sprintf(command, "!thread %I64x\n", dwThread.QuadPart);
    
    return SendStackCommand(command, FALSE);
    }

BOOL Cfailure::SendBangThreadStack(DWORD dwThread, DWORD dwHighPart)
    {
    CHAR command[26];
    
    if (dwHighPart == 0)
        sprintf(command, "!thread %x\n", dwThread);
    else
        sprintf(command, "!thread %x%x\n", dwHighPart, dwThread);
    
    return SendStackCommand(command, FALSE);
    }

BOOL Cfailure::SendCXRStack(ULARGE_INTEGER uliCxr)
    {
    CHAR command[24];
    
    sprintf(command, "!cxr %I64x\n", uliCxr.QuadPart);
    return SendStackCommand(command, TRUE);
    }

BOOL Cfailure::DoKernelBreakin()
    {
    CHAR *buffer = NULL;
    
    if (dwDebugger == PROMPT_NTSD)
        {
        if (SendCommand(".breakin\n", &buffer))
            {
            if (strstr(buffer, "kd>"))
                {
                free(buffer);
                SetDebuggerType(PROMPT_KD);
                FixSymbolPath();
                dwWeBrokeIn++;
                return TRUE;
                }
            else
                {
                free(buffer);
                if (SendCommand("\n", &buffer))
                    {
                    if (strstr(buffer, "kd>"))
                        {
                        free(buffer);
                        SetDebuggerType(PROMPT_KD);
                        FixSymbolPath();
                        dwWeBrokeIn++;
                        return TRUE;
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
    else
        return TRUE;
    
    return FALSE;
    }

BOOL Cfailure::GetReturnBuffer(CHAR ** ppRetBuf)
    {
    DWORD dwLength = 0;
    CHAR *tp = NULL;
    
    if (ppRetBuf == NULL)
        return FALSE;
    
    *ppRetBuf = NULL;
    
    // Make sure we have a description
    if ((pszDescription == NULL) || (dwFailType == FT_BREAKPOINT) || (dwFailType == FT_UNK))
        SetTitleOnLastEvent();
    
    if (pszDescription == NULL)
        SetDescription("Breakpoint");
    
    // Something went wrong - Need a stack of some sort
    if ((pszStack == NULL) && (pszMisc == NULL))
        SetStack("Couldn't Get Stack.  Manual Triaging Required.");
    
    // Default Followup
    if (pszFollowup == NULL)
        SetFollowup("xstress");
    //if (dwBuild == 1381)
    //    SetFollowup("spstress");
    
    if ((pszDescription == NULL) || ((pszStack == NULL) && (pszMisc == NULL)) || (pszFollowup == NULL))
        return FALSE;
    
    if (dwBuild)
        {
        // Let them know if its a VBL build
        if (dwVBL)
            {
            CHAR tVBL[4];
            
            _itot(dwVBL, tVBL, 10);
            
            // -bbbb VBLnn:- 4 for build num, 4 for ' VBL', 1 for ':', 1 for '.'
            // and 1 for NULL terminate (the nn is strlen(tVBL))
            dwLength = strlen(pszDescription) + strlen(tVBL) + 11;
            if (pszVBL_Stamp != NULL)
                dwLength += strlen(pszVBL_Stamp);
            
            tp = (CHAR *)malloc(dwLength);
            if (tp)
                {
                _itot(dwBuild, tp, 10);
                strcat(tp, " VBL");
                strcat(tp, tVBL);
                strcat(tp, ".");
                strcat(tp, pszVBL_Stamp);
                strcat(tp, ":");
                strcat(tp, pszDescription);
                SetDescription(tp);;
                }
            }
        else
            {
            tp = (CHAR *)malloc(strlen(pszDescription) + 6);
            if (tp)
                {
                _itot(dwBuild, tp, 10);
                strcat(tp, ":");
                strcat(tp, pszDescription);
                SetDescription(tp);;
                }
            }
        }
    
    
    dwLength = strlen(pszDescription) + 
        strlen(pszFollowup) + 17;
    
    if (pszStack != NULL)
        dwLength += strlen(pszStack);
    
    if (pszMisc != NULL)
        dwLength += strlen(pszMisc);
    
    if (*ppRetBuf = (CHAR *)malloc(dwLength)) 
        {
        if (pszMisc == NULL)
            sprintf(*ppRetBuf,"%s\n\n%s\n\nFOLLOWUP: %s\n", //15
            pszDescription, pszStack, pszFollowup);
        else if (pszStack == NULL)
            sprintf(*ppRetBuf,"%s\n\n%s\n\nFOLLOWUP: %s\n", //15
            pszDescription, pszMisc, pszFollowup);
        else
            sprintf(*ppRetBuf,"%s\n\n%s\n%s\n\nFOLLOWUP: %s\n", //16
            pszDescription, pszMisc, pszStack, pszFollowup);
        }
    
    return TRUE;
    }


VOID Cfailure::DoNecessaryStackFixUps()
    {
    ULARGE_INTEGER BadAddress = {0};
    BOOL Again = TRUE;
    
    while (Again == TRUE)
        {
        Again = FALSE;
        BadAddress.QuadPart = 0;
        
        if (DoesStackNeedManualReload(this, &BadAddress))
            {
            if (GetRepairedStack(this, BadAddress))
                Again = TRUE;
            }
        }
    
    }
