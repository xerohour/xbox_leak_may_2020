#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <dbghelp.h>

// We need to manually define this crt method because it does not exist in the XBOX version
// of the crt and therefore does not exist in the headers this method includes.  
//
extern "C" _CRTIMP void __cdecl exit(int);


PVOID 
ReadFileToMemZ(LPCSTR pszFN, DWORD* pdwSize)
{
    HANDLE hFile;
    PVOID pMem = NULL;
    DWORD dwSize = 0;
    DWORD dwBytes;

    hFile = CreateFile(pszFN, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        dwSize = GetFileSize(hFile, NULL) + 1;

        if (dwSize > 0)
        {
            pMem = malloc(dwSize);
            if (pMem != NULL)
            {
                if (!ReadFile(hFile, pMem, dwSize - 1, &dwBytes, NULL))
                {
                    free(pMem);
                    pMem = NULL;
                }

                ((char*)pMem)[dwSize - 1] = '\0';
            }
        }
        CloseHandle(hFile);
    }
    *pdwSize = dwSize;
    return pMem;
}


BOOL
IsBlank(char* str)
{
    return str[0] == '\0';
}


ULONG
GetHexNum(char* str, ULONG ofs, ULONG len)
{
    char* begptr = (str + ofs);
    char* endptr = (str + ofs + len - 1);

    return strtoul(begptr, &endptr, 16);
}


enum States  
{
    Starting, Table1, Table2Hdr, Table2HdrBlank, Table2, EntryPoint, EntryPointBlank, Ending
};

struct SecInfo
{
    ULONG Seg;
    ULONG Start;
    ULONG Length;
};


struct StaticInfo
{
    ULONG Seg;
    ULONG Start;
    char FuncName[256];
};

#define MAX_STATICS     2048

SecInfo* Sec;
ULONG SecCount = 0;
char* StaticPtr = NULL;
StaticInfo* Stat;
ULONG StatCount = 0;
char Filter[512];
ULONG TotalSize;


ULONG
GetSecLimit(ULONG Seg, ULONG rva)
{
    for (ULONG i = 0; i < SecCount; i++)
    {
        ULONG Limit = Sec[i].Start + Sec[i].Length;
        if (Seg == Sec[i].Seg &&
            rva >= Sec[i].Start &&
            rva <= Limit)
        {
            return Limit;
        }
    }
    return 0;
}


VOID
BuildStatics()
{

    char* Line; 
    char* EPtr;
    char FuncName[512];
    char OrgName[512];


    Line = StaticPtr;    
    for (;;)
    {
        EPtr = strchr(Line, '\r');
        if (EPtr != NULL)
        {
            *EPtr = '\0';
            if (*(EPtr + 1) == '\n')
            {
                EPtr++;                
            }
        }

        if (IsBlank(Line))
        {
            break;            
        }
    
        Stat[StatCount].Seg = GetHexNum(Line, 1, 4);
        Stat[StatCount].Start = GetHexNum(Line, 6, 8);

        char* p = (Line + 21);
        char* ep = strchr(p, ' ');
        if (ep == NULL)
        {
            printf("Error parsing\n");
        }
    
        memset(OrgName, 0, sizeof(OrgName));
        memcpy(OrgName, p, ep - p);
    
        UnDecorateSymbolName(OrgName, FuncName, sizeof(FuncName), 
                             UNDNAME_NAME_ONLY | 
                             UNDNAME_NO_ALLOCATION_MODEL |
                             UNDNAME_NO_ALLOCATION_LANGUAGE);

        strcpy(Stat[StatCount].FuncName, FuncName);

        if (StatCount < MAX_STATICS) 
        {
            StatCount++;
        }


        if (EPtr == NULL)
        {
            break;            
        }
        Line = EPtr + 1;
    }
}



VOID
CheckStatics(ULONG ChkFuncSeg, ULONG ChkFuncStart, ULONG ChkFuncLength, char* buf)
{

    buf[0] = '\0';
    for (ULONG i = 0; i < StatCount; i++)
    {

        if (Stat[i].Seg == ChkFuncSeg &&
            Stat[i].Start >= ChkFuncStart &&
            Stat[i].Start <= (ChkFuncStart + ChkFuncLength))
        {

            strcat(buf, Stat[i].FuncName);
            strcat(buf, " ");
        }
    }
}



VOID
Out(ULONG FuncLength, char* FuncModule, char* FuncName, ULONG FuncStart, ULONG FuncSeg)
{

    char buf[32 * 1024];

    if (Filter[0] != '\0')
    {
        strcpy(buf, FuncName);
        strcat(buf, " ");
        strcat(buf, FuncModule);
        _strupr(buf);

        if (strstr(buf, Filter) == NULL)
        {
            return;
        }
    }

    TotalSize += FuncLength;

    CheckStatics(FuncSeg, FuncStart, FuncLength, buf);

    printf("%6d, %-40s, %-20s", FuncLength, FuncName, FuncModule);

    if (buf[0] != '\0')
    {
        printf(",\"Includes Statics: %s\"", buf);        
    }
    printf("\n");

        
}


extern "C"
int
_cdecl
main(
    int argc,
    char** argv
    )
{
    ULONG Size;
    char* Data;
    char* Line; 
    char* EPtr;
    char* LineU;

    States State = Starting;
    char OrgName[512];

    ULONG FuncSeg = 0;
    ULONG FuncStart = 0;
    char FuncName[512];
    char FuncModule[512];
    ULONG FuncLength;
    ULONG SecLimit;
    
    ULONG CurrentFuncStart;
    ULONG CurrentFuncSeg;

    BOOL PrevFuncValid = FALSE;

    if (argc < 2)
    {
        printf("Usage: mapsize file.map [filter]\n");
        return -1;
    }

    Filter[0] = '\0';
    TotalSize = 0;

    if (argc >= 3)
    {
        strcpy(Filter, argv[2]);
        _strupr(Filter);
    }

    Data = (char*)ReadFileToMemZ(argv[1], &Size);

    if (Data == NULL)
    {
        printf("Cannot read map file %s\n", argv[1]);
        return -1;
    }

    printf("%6s, %-40s, %-20s\n", "[SIZE]", "[FUNCTION]", "[MODULE]");


    Sec = (SecInfo*)malloc(1024 * sizeof(SecInfo));
    Stat = (StaticInfo*)malloc(MAX_STATICS * sizeof(StaticInfo));
    LineU = (char*)malloc(8192);


    StaticPtr = strstr(Data, "Static symbols\r");
    if (StaticPtr != NULL)
    {
        StaticPtr += 18;

        BuildStatics();
    }

    Line = Data;    
    for (;;)
    {
        EPtr = strchr(Line, '\r');
        if (EPtr != NULL)
        {
            *EPtr = '\0';
            if (*(EPtr + 1) == '\n')
            {
                EPtr++;                
            }
        }

        switch (State)
        {
        
            case Starting:
                strcpy(LineU, Line);
                _strupr(LineU);

                if ((strstr(LineU, "START ") != NULL) && 
                    (strstr(LineU, "   CLASS") != NULL))
                {
                    State = Table1;
                }
                break;

            case Table1:
                if (!IsBlank(Line))
                {
                    Sec[SecCount].Seg = GetHexNum(Line, 1, 4);
                    Sec[SecCount].Start = GetHexNum(Line, 6, 8);
                    Sec[SecCount].Length = GetHexNum(Line, 15, 8);
                    SecCount++;
                
                }
                else
                {
                    State = Table2Hdr;
                }
                break;

            case Table2Hdr:
                State = Table2HdrBlank;
                break;

            case Table2HdrBlank:
                State = Table2;
                break;

            case Table2:

                if (!IsBlank(Line))
                {

                    CurrentFuncSeg = GetHexNum(Line, 1, 4);
                    CurrentFuncStart = GetHexNum(Line, 6, 8);

                    if (PrevFuncValid)
                    {
                        if (CurrentFuncStart == 0)
                        {
                            SecLimit = GetSecLimit(FuncSeg, FuncStart);

                            FuncLength = SecLimit - FuncStart;
                        }
                        else
                        {
                            FuncLength = CurrentFuncStart - FuncStart;
                        }


                        Out(FuncLength, FuncModule, FuncName, FuncStart, FuncSeg);                    
                    }
                    
                    FuncSeg = GetHexNum(Line, 1, 4);
                    FuncStart = GetHexNum(Line, 6, 8);

                    char* p = (Line + 21);
                    char* ep = strchr(p, ' ');
                    if (ep == NULL)
                    {
                        printf("Error parsing\n");
                        return -1;
                    }
                
                    memset(OrgName, 0, sizeof(OrgName));
                    memcpy(OrgName, p, ep - p);

                    while (*ep == ' ')
                    {
                        ep++;
                    }
                    ep += 13;

                    strcpy(FuncModule, ep);

                    UnDecorateSymbolName(OrgName, FuncName, sizeof(FuncName), 
                                         UNDNAME_NAME_ONLY | 
                                         UNDNAME_NO_ALLOCATION_MODEL |
                                         UNDNAME_NO_ALLOCATION_LANGUAGE);



                    PrevFuncValid = TRUE;

                }
                else
                {

                    if (PrevFuncValid)
                    {
                        SecLimit = GetSecLimit(FuncSeg, FuncStart);

                        FuncLength = SecLimit - FuncStart;
                        
                        Out(FuncLength, FuncModule, FuncName, FuncStart, FuncSeg);                    
                        
                    }
                    State = Ending;

                    PrevFuncValid = FALSE;
                }
                break;

        }
       
        if (EPtr == NULL)
        {
            break;            
        }
        Line = EPtr + 1;
    }

    printf("\n\nTotal=%d\n", TotalSize);


    return 0;
}



