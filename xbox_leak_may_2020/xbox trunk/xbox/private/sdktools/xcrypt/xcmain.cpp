/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    xcmain.cpp

Abstract:

    xcrypt command line tool


--*/

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <rsa.h>
#include <sha.h>
#include <rc4.h>
#include <xcrypt.h>

#include "xctool.h"
#include "key.h"

#include <string.h>
#include "xcmain.h"
#include <xboxverp.h>

// We need to manually define this crt method because it does not exist in the XBOX version
// of the crt and therefore does not exist in the headers this method includes.  
//
extern "C" _CRTIMP void __cdecl exit(int);


// 
// main object for tracking global state
//
CXCryptMain g_ib;

//
// property names
//
#define PROP_HELP            TEXT("Help")
#define PROP_CONFIG          TEXT("Config")
#define PROP_GENKEY          TEXT("GenKey")
#define PROP_KEYOUT          TEXT("KeyOut")
#define PROP_KEYOUTENC       TEXT("KeyOutEnc")
#define PROP_KEYIN           TEXT("KeyIn")
#define PROP_DUMPTYPE        TEXT("DumpType")



// 
// command table. NOTE: Must be sorted by lpCmdLine
//
CMDTABLE CmdTable[] =
{
    {"?",            PROP_HELP,         CMDTABLE::VAR_NONE,       NULL},
    {"CONFIG:",      PROP_CONFIG,       CMDTABLE::VAR_NONE,       NULL},
    {"DUMPTYPE:",    PROP_DUMPTYPE,     CMDTABLE::VAR_NONE,       NULL},
    {"GENKEY:",      PROP_GENKEY,       CMDTABLE::VAR_NONE,       NULL},
    {"KEYIN:",       PROP_KEYIN,        CMDTABLE::VAR_NONE,       NULL},
    {"KEYOUT:",      PROP_KEYOUT,       CMDTABLE::VAR_NONE,       NULL},
    {"KEYOUTENC:",   PROP_KEYOUTENC,    CMDTABLE::VAR_NONE,       NULL},
    {"V:",           "",                CMDTABLE::VAR_INT,        &g_ib.m_nTraceLevel}
};



// 
// CXCryptMain class 
//

void
CXCryptMain::PrintUsage()
{
    fprintf(stderr,
        "Usage: XCRYPT [switches] [cfgfile]\n\n"
        "  /GENKEY:{PUB|SYM}\tGenerate key PUB=Public, SYM=Symmetric\n"
        "  /KEYOUT:file\t\tKey file to be created (plain text)\n"
        "  /KEYOUTENC:file\tKey file to be created (cipher text)\n"
        "  /KEYIN:file\t\tKey file to be read\n"
        "  /DUMPTYPE:{BIN|ASM|C}\tDump type\n"
        "  /?\t\t\tDisplay this message\n"
        "\n\nEx: Generate public keypair plain text file:\n"
        "  XCRYPT /GENKEY:PUB /KEYOUT:test.xck\n"
        "\nEx: Generate symmetric key in cipher text file:\n"
        "  XCRYPT /GENKEY:SYM /KEYOUTENC:test.key\n"
        "\nEx: Dump a key file in ASM format:\n"
        "  XCRYPT /KEYIN:test.key /DUMPTYPE:ASM\n"
        "\nEx: Create a new cipher text file by copying an existing file:\n"
        "  XCRYPT /KEYIN:test.key /KEYOUTENC:test2.key\n"
        );
}



//  CXCryptMain::Initialize
//      In lieu of constructor, called to set up everything for an image run.
//      Assumed to return success. Any failure will cause the initialize function
//      to force an exit.
void
CXCryptMain::Initialize(
    int argc,
    char** argv
    )
{
    printf("Microsoft (R) XCRYPT - MICROSOFT CONFIDENTIAL %s\n"
           "Copyright (C) Microsoft Corporation 2000. All rights reserved.\n\n", 
           VER_PRODUCTVERSION_STR);

    //
    // read switches from command line
    //
    for (int i = 1 ; i < argc ; i++)
    {
        if (*argv[i] == '-' || *argv[i] == '/')
        {
            ParseSwitch(argv[i]);
        }
        else
        {
            // 
            // not a switch. See if we already have an exe file. If we do, this is an error
            //
            if (g_ib.m_prop.Get(PROP_CONFIG))
            {
                g_ib.PrintUsage();
                g_ib.Exit(-1, "Only one config file can be specified at a time");
            }
            g_ib.m_prop.Set(PROP_CONFIG, argv[i]);

        }
    }

    // 
    // if a config file is specified, read switches from it
    //
    if (g_ib.m_prop.Get(PROP_CONFIG))
    {
        ReadSwitchesFromFile(g_ib.m_prop.Get(PROP_CONFIG));
    }

}


void
CXCryptMain::ParseSwitch(
    LPCTSTR pszArg
    )
{
    LPCTSTR lpKey;
    CMDTABLE* lpCommand;


    // Search for the command in the command table, bail if we don't find it
    lpKey = pszArg + 1;
    lpCommand = (CMDTABLE*)bsearch((char*)&lpKey, (char*)&CmdTable[0],
        ARRAYSIZE(CmdTable), sizeof (CMDTABLE), 
        CMDTABLE::Compare);
    if (!lpCommand)
    {
        g_ib.PrintUsage();
        ERROR_OUT("Invalid switch '%s'", lpKey);
        g_ib.Exit(-1, NULL);
    }

    // Note that the CRT handles quoted (")
    // command line arguments and puts them in argv correctly and strips the quotes
    lpKey = pszArg + strlen(lpCommand->lpCmdLine) + 1;

    // if a property name is specified, add the command and value as a property. 
    if (lpCommand->lpPropName[0] != '\0')
    {
        g_ib.m_prop.Set(lpCommand->lpPropName, lpKey);
    }

    // if there's a variable, set it
    if (lpCommand->lpVar && lpCommand->vartype != CMDTABLE::VAR_NONE)
    {
        switch (lpCommand->vartype)
        {
            case CMDTABLE::VAR_STRING:
                *(char**)lpCommand->lpVar = (char*)lpKey;
                break;

            case CMDTABLE::VAR_INT:
                *(int*)lpCommand->lpVar = atoi(lpKey);
                break;

            case CMDTABLE::VAR_STRINGLIST:
                BLOCK
                {
                    CStrNode* pStrNode;

                    pStrNode = new CStrNode;
                    pStrNode->SetValue(lpKey);
                
                    // add it to the list
                    pStrNode->Link((CListHead*)(lpCommand->lpVar), NULL);
                }
                break;


        }  // switch
    }
}



void
CXCryptMain::ReadSwitchesFromFile(
    LPCSTR pszFN
    )
{
    FILE* f;
    char szLine[255];
    int ret;

    f = fopen(pszFN, "r");
    if (f != NULL)
    {
        while (!feof(f))
        {
            ret = fscanf(f, "%s", szLine);
            if (ret == 0 || ret == EOF)
                break;

            if (szLine[0] == '-' || szLine[0] == '/')
            {
                ParseSwitch(szLine);
            }
            else
            {
                g_ib.Exit(-1, "Invalid entry in config file");
            }
        }
        fclose(f);
    }
    else
    {
        g_ib.Exit(-1, "Could not open config file");
    }
        
}



//  CXCryptMain::Exit
//      Called to force the application to exit. All exits, fatal or not, should be done through this
//      method. Pass in NULL if there is no fatal error message.
void
CXCryptMain::Exit(
    int nReturnCode,
    LPCSTR lpszFatalError
    )
{
    // 
    // spew the final messages if any
    //

    TRACE_OUT(TRACE_ALWAYS, "");

    for (CNode* pNode = m_Warnings.GetHead(); pNode != NULL; pNode = pNode->Next())
    {
        CStrNode* pStr = (CStrNode*)pNode;

        WARNING_OUT(pStr->GetValue());
    }

    if (lpszFatalError)
    {
        ERROR_OUT(lpszFatalError);
    }
    if (m_pszExitMsg)
    {
        ERROR_OUT(m_pszExitMsg);
        
        // if exit msg was specified, force return code to be -1
        nReturnCode = -1;
    }


    if (nReturnCode == -1)
    {
        //
        // we had a fatal error, delete the out file because it is invalid
        //
    }


    exit(nReturnCode);
}


void
CXCryptMain::SetExitMsg(
    HRESULT hr,
    LPCSTR pszExitMsg
    )
{
    LPSTR pszMsg;
    char szMsg[512];
    const char szNULL[] = "NULL";
    int l;

    if (pszExitMsg == NULL)
    {
        pszExitMsg = szNULL;
    }

    if (hr == E_UNEXPECTED)
    {
        sprintf(szMsg, "%s (Unexpected error occurred)", pszExitMsg);
    }
    else
    {
        if (FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
            (LPSTR)&pszMsg,
            0,
            NULL) != 0)
        {
            sprintf(szMsg, "%s (%s", pszExitMsg, pszMsg);

            LocalFree(pszMsg);

            // strip the end of line characters
            l = strlen(szMsg);
            if (l >= 2 && szMsg[l - 2] == '\r')
            {
                szMsg[l - 2] = '\0';
            }

            strcat(szMsg, ")");
        }
        else
        {
            sprintf(szMsg, "%s", pszExitMsg);
        }
    }

    SetExitMsg(szMsg);
}


void
CXCryptMain::SetExitMsg(
    LPCSTR pszMsg
    )
{

    m_pszExitMsg = _strdup(pszMsg);
}



//  CXCryptMain::TraceOut
//      Called by TRACE_OUT macro. Don't put in line feeds.
void
CXCryptMain::TraceOut(
    int nLevel,
    LPCSTR lpszFormat,
    va_list vararg
    )
{
    char szOutput[4096];

    // Make sure we're at an appropriate trace level
    if (nLevel > g_ib.m_nTraceLevel)
        return;

    vsprintf((char*)szOutput, (char*)lpszFormat, vararg);

    // Spew it out
    printf(TEXT("%s\n"), szOutput);
}


void
CXCryptMain::ErrorOut(
    BOOL bErr,
    LPCSTR lpszFormat,
    va_list vararg
    )
{
    char szOutput[4096];

    vsprintf((char*)szOutput, (char*)lpszFormat, vararg);

    if (bErr)
    {
        fprintf(stderr, "XCRYPT : Error : %s\n", szOutput);
    }
    else
    {
        fprintf(stderr, "XCRYPT : Warning : %s\n", szOutput);
    }
}



void
CXCryptMain::AddWarning(
    LPCSTR pszFormat, ...
    )
{
    CStrNode* pStrNode;
    char szBuf[512];
    va_list ArgList;
    
    va_start(ArgList, pszFormat);
    wvsprintf(szBuf, pszFormat, ArgList);
    va_end(ArgList);

    pStrNode = new CStrNode;
    pStrNode->SetValue(szBuf);

    // add it to the list
    pStrNode->Link(&m_Warnings, NULL);
   
}



int
_cdecl
CMDTABLE::Compare(
    const void* p1, 
    const void* p2
    )
{
    char** pkey = (char**)p1;
    CMDTABLE* ptab = (CMDTABLE*)p2;

    return _strnicmp(*pkey, ptab->lpCmdLine, strlen(ptab->lpCmdLine));
}



//
// CXCMainTool
//
class CXCMainTool : public CXCTool
{
public:
    void Out(LPCSTR pszFormat, ...);

};


void 
CXCMainTool::Out(
    LPCSTR pszFormat, 
    ...
    )
/*++

Routine Description:

    printf functality

Arguments:

Return Value:

    None

--*/
{
    char szBuf[2048];
    va_list ArgList;
    
    va_start(ArgList, pszFormat);
    wvsprintf(szBuf, pszFormat, ArgList);
    va_end(ArgList);

    printf(szBuf);
}



BOOL
CXCTool::ReadPassword(
    LPCSTR pszPrompt, 
    LPSTR pszPassword
    )
{
    int c;
    int n = 0;

    printf(pszPrompt);

    for (;;)
    {
         c = _getch();

         //
         // ^C
         //
         if (c == 0x3)
         {
             putchar('\n');
             ExitProcess(0);
         }

         //
         // Backspace
         //
         if (c == 0x8)
         {
             if (n)
             {
                n--;
                _putch(0x8);
                _putch(' ');
                _putch(0x8);
             }
             continue;
         }

         //
         // Return
         //
         if (c == '\r')
             break;

         if ((n + 1) < PASSWORD_MAXSIZE) 
         {
             pszPassword[n++] = (char)c;
             _putch('*');
         }
    }

    pszPassword[n] = 0;
    putchar('\n');

    if (strlen(pszPassword) < PASSWORD_MINLEN) 
    {
        ERROR_OUT("Password must at least be %d characters", PASSWORD_MINLEN);
        pszPassword[0] = 0;
        return FALSE;        
    }

    return TRUE;
} 

 
void
CStrNode::Dump()
{
    TRACE_OUT(TRACE_ALWAYS, m_pszValue);
}



const ULONG CRandom::rgulMults[] = {
    0x52F690D5,
    0x534D7DDE,
    0x5B71A70F,
    0x66793320,
    0x9B7E5ED5,
    0xA465265E,
    0xA53F1D11,
    0xB154430F,
};

CRandom::CRandom(void)
{
    FILETIME ft;

    /* If this call fails, we still have random data in ft */
    GetSystemTimeAsFileTime(&ft);
    Seed(ft.dwHighDateTime ^ ft.dwLowDateTime);
}

void CRandom::Seed(ULONG ulSeed)
{
    m_ulMask = 0;
    m_ulMult = rgulMults[ulSeed & 7];
    m_ulCur = ulSeed;
    m_ulMask = Rand();
}

ULONG CRandom::Rand(void)
{
    ULARGE_INTEGER li;

    /* We're not a true 32-bit generator, since 2^32+1 isn't prime, so we use
     * 2^32-5 instead */
    li.QuadPart = m_ulCur;
    li.QuadPart += 1;
    li.QuadPart *= m_ulMult;
    li.QuadPart %= 0xFFFFFFFB;
    m_ulCur = li.LowPart;
    return m_ulCur ^ m_ulMask;
}

void CRandom::RandBytes(LPBYTE pb, DWORD cb)
{
    PUSHORT pus;

    if(cb == 0)
        return;
    if((ULONG)pb & 1) {
        *pb++ = (BYTE)(Rand() >> 14);
        --cb;
    }
    pus = (PUSHORT)pb;
    while(cb > 1) {
        *pus++ = (USHORT)(Rand() >> 8);
        cb -= 2;
    }
    if(cb)
        *pb = (BYTE)(Rand() >> 12);
}




extern "C"
int
_cdecl
main(
    int argc,
    char** argv
    )
{

    HRESULT hr;
    CXCMainTool Tool;
    BOOL bUsage = TRUE;
    CXCMainTool::DUMPTYPE DumpType = CXCMainTool::DUMP_NONE;

    g_ib.Initialize(argc, argv);

    LPCSTR pszGenKey = g_ib.m_prop.Get(PROP_GENKEY);
    LPCSTR pszDumpType = g_ib.m_prop.Get(PROP_DUMPTYPE);

    if (pszDumpType != NULL)
    {
        
        if (_stricmp(pszDumpType, "BIN") == 0)
            DumpType = CXCMainTool::DUMP_BIN;
        else if (_stricmp(pszDumpType, "ASM") == 0)
            DumpType = CXCMainTool::DUMP_ASM;
        else if (_stricmp(pszDumpType, "C") == 0)
            DumpType = CXCMainTool::DUMP_C;
        else
        {
            g_ib.SetExitMsg("Invalid dump type");
            goto Cleanup;
        }        
    }

    if (pszGenKey != NULL)
    {
        //
        // genereate key
        //

        if (g_ib.m_prop.Get(PROP_KEYOUT) == NULL && g_ib.m_prop.Get(PROP_KEYOUTENC) == NULL)
        {
            g_ib.SetExitMsg(-1, "Must specify /KEYOUT or /KEYOUTENC with /GENKEY");
        }
        else if (_stricmp(pszGenKey, "PUB") == 0)
        {
            if (g_ib.m_prop.Get(PROP_KEYOUTENC) != NULL)
            {
                //
                // Generate public key pair as cipher text
                //
                hr = Tool.GeneratePubPair(g_ib.m_prop.Get(PROP_KEYOUTENC), TRUE);
            }
            else
            {
                //
                // Generate public key pair as plain text
                //
                hr = Tool.GeneratePubPair(g_ib.m_prop.Get(PROP_KEYOUT), FALSE);

            }
            if (FAILED(hr))
            {
                g_ib.SetExitMsg(hr, "Could not generate public key pairs");
                goto Cleanup;
            }

            if (DumpType != CXCMainTool::DUMP_NONE)
            {
                //
                // Dump out the key
                //
                if (g_ib.m_prop.Get(PROP_KEYOUTENC) != NULL)
                {
                    Tool.DumpKeyFile(g_ib.m_prop.Get(PROP_KEYOUTENC), DumpType);
                }
                else
                {
                    Tool.DumpKeyFile(g_ib.m_prop.Get(PROP_KEYOUT), DumpType);
                }
            }

            bUsage = FALSE;
        }
        else if (_stricmp(pszGenKey, "SYM") == 0)
        {

            if (g_ib.m_prop.Get(PROP_KEYOUTENC) != NULL)
            {
                //
                // Generate symetric key as cipher text
                //
                hr = Tool.GenerateSymKey(g_ib.m_prop.Get(PROP_KEYOUTENC), TRUE);
            }
            else
            {
                //
                // Generate symetric key as plain text
                //
                hr = Tool.GenerateSymKey(g_ib.m_prop.Get(PROP_KEYOUT), FALSE);
            }

            if (FAILED(hr))
            {
                g_ib.SetExitMsg(hr, "Could not generate symmetric key");
                goto Cleanup;
            }

            if (DumpType != CXCMainTool::DUMP_NONE)
            {
                //
                // Dump out the key
                //
                if (g_ib.m_prop.Get(PROP_KEYOUTENC) != NULL)
                {
                    Tool.DumpKeyFile(g_ib.m_prop.Get(PROP_KEYOUTENC), DumpType);
                }
                else
                {
                    Tool.DumpKeyFile(g_ib.m_prop.Get(PROP_KEYOUT), DumpType);
                }
                if (FAILED(hr))
                {
                    g_ib.SetExitMsg(hr, "Could not copy key file");
                    goto Cleanup;
                }
            }

            bUsage = FALSE;
        }
        else
        {
            g_ib.SetExitMsg(-1, "/GENKEY can only be PUB or SYM");
        }
    }
    else if (pszDumpType != NULL)
    {
        //
        // dump key
        //
        if (g_ib.m_prop.Get(PROP_KEYIN) == NULL)
        {
            g_ib.SetExitMsg(-1, "Must specify /KEYIN with /DUMPTYPE");
        }
        else
        {
            Tool.DumpKeyFile(g_ib.m_prop.Get(PROP_KEYIN), DumpType);
            bUsage = FALSE;
        }
    }
    else if (g_ib.m_prop.Get(PROP_KEYOUT) != NULL || g_ib.m_prop.Get(PROP_KEYOUTENC)) 
    {
        //
        // Output a key file from existing keyfile
        //
        if (g_ib.m_prop.Get(PROP_KEYIN) == NULL)
        {
            g_ib.SetExitMsg(-1, "Must specify /KEYIN to copy key to another keyfile");
        }
        
        if (g_ib.m_prop.Get(PROP_KEYOUTENC) != NULL)
        {
            hr = Tool.CopyKeyFile(g_ib.m_prop.Get(PROP_KEYIN), g_ib.m_prop.Get(PROP_KEYOUTENC), TRUE);
        }
        else
        {
            hr = Tool.CopyKeyFile(g_ib.m_prop.Get(PROP_KEYIN), g_ib.m_prop.Get(PROP_KEYOUT), FALSE);
        }
        
        bUsage = FALSE;
    }

Cleanup:
    if (bUsage)
    {
        g_ib.PrintUsage();
    }

      
    // successful exit, does not return
    g_ib.Exit(0, NULL);

    return 0;
}


       
