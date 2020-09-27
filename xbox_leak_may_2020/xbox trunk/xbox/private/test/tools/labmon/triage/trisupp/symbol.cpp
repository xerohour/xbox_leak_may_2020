#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "failure.h"
#include "utils.h"
#include "symbol.h"

extern Cfailure *Failure;

BOOL DoReload()
    {
    CHAR *buffer;
    
    if ((Failure->SendCommand("!reload\n", NULL)))
        {
        return TRUE;
        }
    
    if (!Failure->SendCommand(NULL, &buffer))
        return FALSE;
    
    free(buffer);
    return TRUE;
    }

BOOL StayLocal()
    {
    CHAR *buffer = NULL;
    
    if (!Failure->SendCommand("|\n", &buffer))
        return TRUE;
    
    if ((stristr(buffer, "lsass.exe"))||(stristr(buffer, "pdwssp.dll"))||
        (stristr(buffer, "csrss.exe"))||(stristr(buffer, "csrsrv.dll"))||
        (stristr(buffer, "webclnt.exe")))
        {
        free (buffer);
        return TRUE;
        }
    
    free (buffer);
    return FALSE;
    }

CHAR *GetUserPath()
    {
    CHAR UserPath[MAX_PATH + 1];
    
    GetPrivateProfileString("PATHS",
        "USERPATH", 
        DEFAULT_USERPATH,
        UserPath, 
        sizeof(UserPath), 
        Failure->IniFile());
    
    if (UserPath[strlen(UserPath) - 1] == ';')
        UserPath[strlen(UserPath) - 1] = '\0';
    
    return _strdup(UserPath);
    }

CHAR *GetSymbolServerPath()
    {
    return _strdup("symsrv*symsrv.dll*\\\\symbols\\symbols");
    }

CHAR *GetVBLPath()
    {
    CHAR buff[48];
    CHAR UserPath[MAX_PATH * 2];
    CHAR pathbuffer[MAX_PATH *2];
    CHAR *tmp = NULL, *tmp2 = NULL;
    
    UserPath[0] = '\0';
    
    switch(Failure->VBL())
        {
        case 1:
            if ((Failure->Architecture()) == MACH_X86)	
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL1X86CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL1X86FRE, sizeof(UserPath)-1);
                }
            else if ((Failure->Architecture()) == MACH_IA64)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL1IA64CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL1IA64FRE, sizeof(UserPath)-1);
                }
            else
                return NULL;
            break;
        case 2:
            if ((Failure->Architecture()) == MACH_X86)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL2X86CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL2X86FRE, sizeof(UserPath)-1);
                }
            else if ((Failure->Architecture()) == MACH_IA64)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL2IA64CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL2IA64FRE, sizeof(UserPath)-1);
                }
            else
                return NULL;
            break;
        case 3:
            if ((Failure->Architecture()) == MACH_X86)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL3X86CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL3X86FRE, sizeof(UserPath)-1);
                }
            else if ((Failure->Architecture()) == MACH_IA64)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL3IA64CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL3IA64FRE, sizeof(UserPath)-1);
                }
            else
                return NULL;
            break;
        case 4:
            if ((Failure->Architecture()) == MACH_X86)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL4X86CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL4X86FRE, sizeof(UserPath)-1);
                }
            else if ((Failure->Architecture()) == MACH_IA64)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL4IA64CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL4IA64FRE, sizeof(UserPath)-1);
                }
            else
                return NULL;
            break;
        case 6:
            if ((Failure->Architecture()) == MACH_X86)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL6X86CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL6X86FRE, sizeof(UserPath)-1);
                }
            else if ((Failure->Architecture()) == MACH_IA64)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL6IA64CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL6IA64FRE, sizeof(UserPath)-1);
                }
            else
                return NULL;
            break;
        case 7:
            if ((Failure->Architecture()) == MACH_X86)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL7X86CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL7X86FRE, sizeof(UserPath)-1);
                }
            else if ((Failure->Architecture()) == MACH_IA64)
                {
                if ((Failure->BuildType()) == BUILD_CHECKED)
                    strncpy(UserPath, VBL7IA64CHK, sizeof(UserPath)-1);
                else 
                    strncpy(UserPath, VBL7IA64FRE, sizeof(UserPath)-1);
                }
            else
                return NULL;
            break;
            
        default:
            return NULL;
    }
    
    if (UserPath[0] == '\0')
        return NULL;
    
    UserPath[sizeof(UserPath) - 1] = '\0';
    
    strcpy(pathbuffer, UserPath);
    
    while (tmp = strstr(pathbuffer, "##BLD##"))
        {
        tmp2 = strstr(UserPath, "##BLD##") + strlen("##BLD##");
        tmp[0] = '\0';
        
        if ((Failure->BuildNumber()) == 0)
            Failure->SetBuildNumber(2195);
        
        _itoa(Failure->BuildNumber(), buff, 10);
        strcat(pathbuffer, buff);
        
        if (((Failure->SP()) <= 1) && (Failure->BuildDot() > 0))
            {
            strcat(pathbuffer, ".");
            sprintf(buff, "%03u", Failure->BuildDot());
            strcat(pathbuffer, buff);
            }
        
        strcat(pathbuffer, tmp2);
        
        tmp2 = strstr(UserPath, "##BLD##");
        
        tmp2[0] = '\0';
        strcat(tmp2, tmp);
        }
    
    while (tmp = strstr(pathbuffer, "##STMP##"))
        {
        tmp2 = strstr(UserPath, "##STMP##") + strlen("##STMP##");
        tmp[0] = '\0';
        
        strcat(pathbuffer, Failure->VBLString());
        
        strcat(pathbuffer, tmp2);
        
        tmp2 = strstr(UserPath, "##STMP##");
        tmp2[0] = '\0';
        strcat(tmp2, tmp);
        }
    
    if ((Failure->IsLB6RI()) == TRUE)
        {
        while (tmp = strstr(pathbuffer, "Lab06_N"))
            {
            tmp2 = strstr(UserPath, "Lab06_N") + strlen("Lab06_N");
            tmp[0] = '\0';
            
            strcat(pathbuffer, "LB6RI");
            
            strcat(pathbuffer, tmp2);
            
            tmp2 = strstr(UserPath, "Lab06_N");
            tmp2[0] = '\0';
            strcat(tmp2, tmp);
            }
        }
    
    return _strdup(pathbuffer);
}

CHAR *GetStressPath()
    {
    CHAR buff[48];
    CHAR UserPath[MAX_PATH + 1];
    CHAR pathbuffer[MAX_PATH + 1];
    CHAR *tmp = NULL, *tmp2 = NULL;
    
    if (((Failure->BuildDot()) > 0) && ((Failure->SP()) > 0))
        {
        if ((Failure->BuildType()) == BUILD_CHECKED)
            {
            strcpy(UserPath, "\\\\spstress\\symbols\\##ARCH##\\##BLD##sp##SP##.chk\\##SP##.##DOT##");
            }
        else
            {
            strcpy(UserPath, "\\\\spstress\\symbols\\##ARCH##\\##BLD##sp##SP##\\##SP##.##DOT##");
            }
        }
    else
        {
        if ((Failure->BuildType()) == BUILD_CHECKED)
            {
            GetPrivateProfileString("PATHS", 
                "CHKKERNELPATH", 
                DEFAULT_CHKKERNEL,
                UserPath, 
                sizeof(UserPath), 
                Failure->IniFile());
            }
        else
            {
            GetPrivateProfileString("PATHS", 
                "FREEKERNELPATH", 
                DEFAULT_FREKERNEL,
                UserPath, 
                sizeof(UserPath), 
                Failure->IniFile());
            }
        }
    
    strncpy(pathbuffer, UserPath, sizeof(pathbuffer) - 1);
    
    while (tmp = strstr(pathbuffer, "##ARCH##"))
        {
        tmp2 = strstr(UserPath, "##ARCH##") + strlen("##ARCH##");
        tmp[0] = '\0';
        
        if ((Failure->Architecture()) == MACH_IA64)
            strcat(pathbuffer, "ia64");
        else
            strcat(pathbuffer, "x86");
        
        strcat(pathbuffer, tmp2);
        
        tmp2 = strstr(UserPath, "##ARCH##");
        tmp2[0] = '\0';
        strcat(tmp2, tmp);
        }
    
    while (tmp = strstr(pathbuffer, "##BLD##"))
        {
        tmp2 = strstr(UserPath, "##BLD##") + strlen("##BLD##");
        tmp[0] = '\0';
        
        if ((Failure->BuildNumber()) == 0)
            Failure->SetBuildNumber(2195);
        
        _itoa(Failure->BuildNumber(), buff, 10);
        strcat(pathbuffer, buff);
        
        
        if (((Failure->SP()) == 0) && ((Failure->BuildDot()) > 0))
            {
            strcat(pathbuffer, ".");
            sprintf(buff, "%03u", Failure->BuildDot());
            strcat(pathbuffer, buff);
            }
        
        strcat(pathbuffer, tmp2);
        
        tmp2 = strstr(UserPath, "##BLD##");
        
        tmp2[0] = '\0';
        strcat(tmp2, tmp);
        }
    
    while (tmp = strstr(pathbuffer, "##SP##"))
        {
        tmp2 = strstr(UserPath, "##SP##") + strlen("##SP##");
        tmp[0] = '\0';
        
        if ((Failure->SP()) == 0)
            Failure->SetSP(2);
        
        _itoa(Failure->SP(), buff, 10);
        strcat(pathbuffer, buff);
        
        strcat(pathbuffer, tmp2);
        
        tmp2 = strstr(UserPath, "##SP##");
        
        tmp2[0] = '\0';
        strcat(tmp2, tmp);
        }
    
    while (tmp = strstr(pathbuffer, "##DOT##"))
        {
        tmp2 = strstr(UserPath, "##DOT##") + strlen("##DOT##");
        tmp[0] = '\0';
        
        sprintf(buff, "%03u", Failure->BuildDot());
        strcat(pathbuffer, buff);
        
        tmp2 = strstr(UserPath, "##DOT##");
        
        tmp2[0] = '\0';
        strcat(tmp2, tmp);
        }
    
    return _strdup(pathbuffer);
}

CHAR *GetKernelPath()
    {
    
    if ((Failure->VBL()) > 0)
        return GetVBLPath();
    else
        return GetStressPath();
    }

CHAR *GetSymbolsPath()
    {
    DWORD dwLength = 0;
    CHAR *pszUserPath = NULL;
    CHAR *pszSymSrvPath = NULL;
    CHAR *pszKernelPath = NULL;
    CHAR *FullPath = NULL;
    
    if ((Failure->DebuggerType()) == PROMPT_NTSD)
        {
        if (pszUserPath = GetUserPath())
            dwLength += (strlen(pszUserPath) + 1);
        
        if (StayLocal())
            return pszUserPath;
        }
    
    if (pszSymSrvPath = GetSymbolServerPath())
        dwLength += (strlen(pszSymSrvPath) + 1);
    
    if (pszKernelPath = GetKernelPath())
        dwLength += (strlen(pszKernelPath) + 1);
    
    
    if (FullPath = (CHAR *)malloc(dwLength))
        {
        FullPath[0] = '\0';
        
        if (pszUserPath != NULL)
            strcat(FullPath, pszUserPath);
        
        if (pszSymSrvPath != NULL)
            {
            if (pszUserPath != NULL)
                strcat(FullPath, ";");
            
            strcat(FullPath, pszSymSrvPath);
            }
        
        if (pszKernelPath != NULL)
            {
            if ((pszUserPath != NULL) || (pszSymSrvPath != NULL))
                strcat(FullPath, ";");
            
            strcat(FullPath, pszKernelPath);            
            }
        }
    
    SAFEFREE(pszUserPath);
    SAFEFREE(pszSymSrvPath);
    SAFEFREE(pszKernelPath);
    
    return (FullPath);
    }

#define PREAMBLE ".sympath "
BOOL FixSymbolPath()
    {
    DWORD dwLength = strlen(PREAMBLE) + 1;
    DWORD dwIterations = 0;
    CHAR *SymPath = NULL;
    CHAR *SymCommand = NULL;
    CHAR *Buffer = NULL;
    
    if (!Failure->SendCommand(".sympath\n", NULL))
        return FALSE;
    
    if (SymPath = GetSymbolsPath())
        {
        dwLength += (strlen(SymPath) + 1);
        if (SymCommand = (CHAR *)malloc(dwLength))
            {
            strcpy(SymCommand, PREAMBLE);
            strcat(SymCommand, SymPath);
            if (SymCommand[strlen(SymCommand) - 1] != '\n')
                strcat(SymCommand, "\n");
            
            SAFEFREE(SymPath);
            
            if (!Failure->SendCommand(SymCommand, &Buffer))
                {
                free(SymCommand);
                return FALSE;
                }
            
            SAFEFREE(SymCommand);
            
            while ((dwIterations < 3) && (strstr(Buffer, "WARNING:")) && (strstr(Buffer, "accessible")))
                {
                if ((Failure->DebuggerType()) == PROMPT_NTSD)
                    {   
                    if (!Failure->SendCommand(".sleep 5000\n", NULL))
                        return FALSE;
                    
                    SAFEFREE(Buffer);
                    if (!Failure->SendCommand(".sympath\n", &Buffer))
                        return FALSE;
                    
                    dwIterations++;
                    }
                else
                    break;
                }
            SAFEFREE(Buffer);
            }
        }
    
    return DoReload();
    }

//
// Determines weather the symbols on a machine are good or bad
//
BOOL GoodSymbols()
    {
    DWORD numLines = 0, i = 0;
    CHAR *LineHeads[MAXLINES] = {NULL, NULL};
    DWORD firstLine = 0;
    CHAR *buffer;
    BOOL done = TRUE;
    
    if (!Failure->SendCommand("lm l\n", &buffer))
        return FALSE;
    
    numLines = GetLinesFromString(buffer, LineHeads);
    
    for (i=0; i < numLines; i++)
        {
        if ((strstr(LineHeads[i], "(no symbolic information)"))||
            (strstr(LineHeads[i], "(export symbols)")))
            {
            free(buffer);
            return FALSE;
            }
        }
    
    free(buffer);
    return TRUE;
    }

