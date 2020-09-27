#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "owner.h"
#include "failure.h"

extern Cfailure *Failure;

#define INIDELTA 256
#define MAX_CANDIDATES 8
#define MAX_OWNER_SIZE 64

typedef struct _OwnerCandidate 
    {
    CHAR Name[MAX_OWNER_SIZE];
    int count;
    } OwnerCandidate;


BOOL OOF(CHAR *name, int cBuf)
    {
    return FALSE;

    /* // owners looked up in db no ini file
    BOOL ret=FALSE;
    CHAR *tmpbuf;
    
    if (!name || !cBuf) 
        {
        return FALSE;
        }
    if (tmpbuf=(CHAR *)malloc(cBuf+1)) 
        {
        if (ret=GetPrivateProfileString("OOF", name, "", tmpbuf, cBuf, Failure->IniFile())) 
            {
            strcpy(name, tmpbuf);
            }
        free(tmpbuf);
        }
    return ret;
    */
    }

CHAR *GetSubOwner(CHAR *modName, CHAR *funcName, CHAR *ownerBuf, int cBuf)
    {
    CHAR *iniSection=NULL,*bptr = NULL, *aftereq = NULL, *pPattern = NULL, *pSym = NULL;
    int iniSize=0,actualSize=0;
    
    
    if (NULL==modName || NULL==funcName || NULL==ownerBuf || 0==cBuf) 
        {
        return NULL;
        }
    *ownerBuf='\0';
    
    while ('_'==*funcName || '.'==*funcName) 
        {
        funcName++;
        }
    
    while (actualSize>=iniSize-2) 
        {
        if (iniSection) 
            free(iniSection);
        iniSize+=INIDELTA;
        if (iniSection=(CHAR *)malloc(iniSize)) 
            {
            actualSize=GetPrivateProfileSection(modName, iniSection, iniSize, Failure->IniFile());
            } 
        else 
            {
            return NULL;
            }
        }
    
    bptr=iniSection;
    if (bptr == NULL)
        return NULL;
    
    while (*bptr != 0) 
        {
        if (aftereq=strchr(bptr,'=')) 
            {
            *aftereq='\0';
            pSym=funcName;
            pPattern=bptr;
#ifdef UNICODE
            while (*pPattern!='\0' && towlower(*pPattern)==towlower(*pSym)) 
#else
                while (*pPattern!='\0' && tolower(*pPattern)==tolower(*pSym)) 
#endif			
                    {
                    pPattern++;
                    pSym++;
                    }
                if ('\0'==*pPattern) 
                    {
                    strncpy(ownerBuf, aftereq+1, cBuf);
                    free(iniSection);
                    return ownerBuf;
                    }
                *aftereq='=';
            }
        bptr+=strlen(bptr)+1;
        }
    
    free(iniSection);
    if (GetPrivateProfileString(modName, "default", "", ownerBuf, cBuf, Failure->IniFile())) 
        {
        return ownerBuf;
        } 
    else 
        {
        return NULL;
        }
    }

BOOL UnknownExcept(const CHAR * desc)
    {
    if (desc == NULL)
        return FALSE;
    
    if (strstr(desc, "Unknown Exception - code:")) 
        {
        return TRUE;
        }
    else
        return FALSE;
    }

BOOL InPageIo(const CHAR * desc)
    {
    if (desc == NULL)
        return FALSE;
    
    if ((strstr(desc, "NTSD: in page io error"))||
        (strstr(desc, "In Page IO Error"))||
        (strstr(desc, "In-page I/O error"))) 
        {
        return TRUE;
        }
    else
        return FALSE;
    }

BOOL Bugcheck(const CHAR * desc)
    {
    if (desc == NULL)
        return FALSE;
    
    if (strstr(desc, "Bugcheck ")) 
        {
        return TRUE;
        }
    else
        return FALSE;
    }

CHAR *GetOwner(CHAR *symbol, CHAR *ownerBuf, int cBuf)
    {
    CHAR *after = NULL;
    CHAR replaced='\0';
    
    if (NULL==symbol || NULL==ownerBuf || 0==cBuf) 
        {
        return NULL;
        }

    // we look up owners from the db, so just return the default
    strcpy(ownerBuf, "xstress");
    return ownerBuf;

/*
    *ownerBuf='\0';
    if ((after=strchr(symbol, replaced='!')) || (after=strchr(symbol, replaced='+'))) 
        {
        *after='\0';
        if (0==GetPrivateProfileString("owners", symbol, "", ownerBuf, cBuf, Failure->IniFile())) 
            {
            GetPrivateProfileString("owners", "default", "", ownerBuf, cBuf, Failure->IniFile());
            } 
        else 
            {
            if ('['==ownerBuf[0]) 
                {
                CHAR *tmpBuf;
                if (tmpBuf=(CHAR *)malloc(cBuf)) 
                    {
                    ownerBuf[strlen(ownerBuf)-1]='\0';
                    GetSubOwner(ownerBuf+1, after+1, tmpBuf, cBuf);
                    strcpy(ownerBuf, tmpBuf);
                    free(tmpBuf);
                    }
                }
            }
        *after=replaced;
        }
    else
        {
        if (0==GetPrivateProfileString("owners", symbol, "", ownerBuf, cBuf, Failure->IniFile())) 
            {
            GetPrivateProfileString("owners", "default", "", ownerBuf, cBuf, Failure->IniFile());
            } 
        }
    
    if (*ownerBuf) 
        {
        return ownerBuf;
        } 
    else 
        {
        return NULL;
        }
        */
    }

CHAR *AssignStack(CHAR **StackEntries, UINT numEntries, CHAR *ownerBuf, int cBuf)
    {
    OwnerCandidate Owners[MAX_CANDIDATES];
    UINT numOwners=0, i, j;
    UINT NumMatches, MaxLinesToRead, NumSuggestions;
    
    if (!StackEntries || !numEntries || !StackEntries[numEntries-1] || !ownerBuf || !cBuf) 
        {
        return NULL;
        }
    NumMatches=GetPrivateProfileInt("sensitivity", "nummatches", numEntries, Failure->IniFile());
    MaxLinesToRead=GetPrivateProfileInt("sensitivity", "MaxLines", numEntries, Failure->IniFile());
    NumSuggestions=GetPrivateProfileInt("sensitivity", "suggestions", 1, Failure->IniFile());
    
    for (i=0;i<numEntries && i<MaxLinesToRead;i++) 
        {
        GetOwner(StackEntries[i], Owners[numOwners].Name, MAX_OWNER_SIZE);
        if (Owners[numOwners].Name[0] && strcmp(Owners[numOwners].Name, "ignore")) 
            {
            Owners[numOwners].count=1;
            if (0==numOwners) 
                {
                numOwners++;
                } 
            else if (strstr(Owners[numOwners].Name, "maybe")) 
                {
                continue;
                } 
            else if (1==numOwners && strstr(Owners[0].Name, "maybe")) 
                {
                Owners[0]=Owners[numOwners];
                } 
            else 
                {
                for (j=0;j<numOwners;j++) 
                    {
                    if (0==strcmp(Owners[j].Name, Owners[numOwners].Name)) 
                        {
                        Owners[j].count++;
                        break;
                        }
                    }
                if (j==numOwners) 
                    {
                    numOwners++;
                    }
                }
            }
        }
    
    if (0==numOwners) 
        {
        GetPrivateProfileString("owners", "default", "", ownerBuf, cBuf, Failure->IniFile());
        } 
    else 
        {
        j=0;
        for (i=1;i<numOwners && i<NumMatches;i++) 
            {
            if (Owners[i].count>Owners[j].count) 
                {
                j=i;
                }
            }
        
        if (strstr(Owners[j].Name, "maybe")) 
            {
            if (1 != sscanf(Owners[j].Name, "maybe %s", ownerBuf))
                strncpy(ownerBuf, Owners[j].Name, cBuf);
            } 
        else 
            {
            strncpy(ownerBuf, Owners[j].Name, cBuf);
            }
        }
    OOF(ownerBuf, cBuf);
    return ownerBuf;
    }

BOOL GetSpecial(const CHAR *symbol, CHAR *ownerBuf, int cBuf)
    {
    
    if ((symbol == NULL) || (ownerBuf == NULL) || (cBuf == 0)) 
        {
        return FALSE;
        }
    
    if (InPageIo(symbol))
        {
        CHAR mystring[9] = {'\0'};
        if (strstr(symbol, "error "))
            {
            strncpy(mystring, strstr(symbol, "error ") + strlen("error "), 8);
            mystring[8] = '\0';
            if (GetPrivateProfileString("ioerrors", mystring, "", ownerBuf, cBuf, Failure->IniFile()) == 0) 
                return FALSE;
            else
                return TRUE;
            }
        else
            return FALSE;
        
        }
    else if (UnknownExcept(symbol))
        {
        CHAR mystring[9] = {'\0'};
        if (strstr(symbol, "- code: "))
            {
            strncpy(mystring, strstr(symbol, "- code: ") + strlen("- code: "), 8);
            mystring[8] = '\0';
            if (GetPrivateProfileString("ioerrors", mystring, "", ownerBuf, cBuf, Failure->IniFile()) == 0) 
                return FALSE;
            else
                return TRUE;
            }
        else
            return FALSE;
        }
    else if (Bugcheck(symbol))
        {
        //Bugcheck cc - PAGE_FAULT_BEYOND_END_OF_ALLOCATION fd365fe0 00000001 804fca53 00000001
        CHAR mystring[3] = {'\0'};
        if (strstr(symbol, "Bugcheck "))
            {
            strncpy(mystring, strstr(symbol, "Bugcheck ") + strlen("Bugcheck "), 2);
            mystring[2] = '\0';
            if (GetPrivateProfileString("bugchecks", mystring, "", ownerBuf, cBuf, Failure->IniFile()) == 0) 
                return FALSE;
            else
                return TRUE;
            }
        else
            return FALSE;
        }
    else
        {
        if (GetPrivateProfileString("owners", symbol, "", ownerBuf, cBuf, Failure->IniFile()) == 0) 
            return FALSE;
        else
            return TRUE;
        }
    
    return FALSE;
    }


