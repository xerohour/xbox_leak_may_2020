/***************************************************************************
 *
 *  Copyright (C) 11/6/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       console.cpp
 *  Content:    Wave Bundler Win32 console front-end.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/6/2001   dereks  Created.
 *
 ****************************************************************************/

#define EXENAME "WAVBNDLR"

#include "wavbndli.h"
#include <conio.h>

//
// Switch values
//

#define SWITCH_NOOVERWRITE  0x00000001
#define SWITCH_NOLOGO       0x00000002

//
// Class to implement overwrite printing to the console
//

class CPrintBoy
{
protected:
    UINT                    m_nLength;      // Length of the last print operation

public:
    CPrintBoy(void);

public:
    void Print(LPCSTR pszString);
    void Clear(void);
};

//
// Status callback
//

class CConsoleCallback
    : public CWaveBankCallback
{
public:
    CPrintBoy &             m_PrintBoy;     // Console print tool

public:
    CConsoleCallback(CPrintBoy &PrintBoy);

public:
    virtual BOOL OpenBank(LPCSTR pszFile);
    virtual BOOL OpenHeader(LPCSTR pszFile);
    virtual BOOL BeginEntry(LPCSTR pszFile, DWORD dwFlags);
};


/****************************************************************************
 *
 *  CPrintBoy
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPrintBoy::CPrintBoy"

CPrintBoy::CPrintBoy
(
    void
)
:   m_nLength(0)
{
}


/****************************************************************************
 *
 *  Print
 *
 *  Description:
 *      Clears the existing console text line and prints over the top of it.
 *
 *  Arguments:
 *      LPCSTR [in]: string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPrintBoy::Print"

void
CPrintBoy::Print
(
    LPCSTR                  pszString
)
{
    static const int        nCharLimit  = 79;
    int                     nLength;
    int                     nDiff;
    int                     i;

    //
    // Truncate the string to 79 characters
    //

    for(i = 0; i < (int)m_nLength; i++)
    {
        _putch('\b');
    }

    for(nLength = 0; *pszString && (nLength < nCharLimit); nLength++, pszString++)
    {
        _putch(*pszString);
    }

    if((nDiff = m_nLength - nLength) > 0)
    {
        for(i = 0; i < nDiff; i++)
        {
            _putch(' ');
        }

        for(i = 0; i < nDiff; i++)
        {
            _putch('\b');
        }
    }
    
    m_nLength = nLength;
}


/****************************************************************************
 *
 *  Clear
 *
 *  Description:
 *      Clears the existing console text line and prints over the top of it.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CPrintBoy::Clear"

void
CPrintBoy::Clear
(
    void
)
{
    Print("");

    while(m_nLength)
    {
        _putch('\b');

        m_nLength--;
    }
}


/****************************************************************************
 *
 *  CConsoleCallback
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      CPrintBoy & [in]: console print tool.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CConsoleCallback::CConsoleCallback"

CConsoleCallback::CConsoleCallback
(
    CPrintBoy &             PrintBoy
)
:   m_PrintBoy(PrintBoy)
{
}


/****************************************************************************
 *
 *  OpenBank
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      LPCSTR [in]: bank file name. 
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CConsoleCallback::OpenBank"

BOOL
CConsoleCallback::OpenBank
(
    LPCSTR                  pszFile
)
{
    m_PrintBoy.Clear();
    printf("Generating wave bank file %s\r\n", pszFile);

    return TRUE;
}


/****************************************************************************
 *
 *  OpenHeader
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      LPCSTR [in]: header file name. 
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CConsoleCallback::OpenHeader"

BOOL
CConsoleCallback::OpenHeader
(
    LPCSTR                  pszFile
)
{
    m_PrintBoy.Clear();
    printf("Generating header file %s\r\n", pszFile);

    return TRUE;
}


/****************************************************************************
 *
 *  BeginEntry
 *
 *  Description:
 *      Callback function.
 *
 *  Arguments:
 *      CWaveBankEntry * [in]: entry.
 *      DWORD [in]: entry flags.
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CConsoleCallback::BeginEntry"

BOOL
CConsoleCallback::BeginEntry
(
    LPCSTR                  pszFile,
    DWORD                   dwFlags
)
{
    CHAR                    szText[MAX_PATH + 0x100];

    strcpy(szText, "Processing ");
    _splitpath(pszFile, NULL, NULL, &szText[strlen(szText)], NULL);

    m_PrintBoy.Print(szText);

    return TRUE;
}


/****************************************************************************
 *
 *  Banner
 *
 *  Description:
 *      Prints the "hey, look at me!" banner.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "Banner"

void
Banner
(
    void
)
{
    CHAR                    szVersion[0x100];

    GetWaveBundlerVersion(szVersion);
    
    printf("\r\n");
    printf(EXENAME " version %s\r\n", szVersion);
    printf("Copyright (c) 2001 Microsoft Corporation.  All rights reserved.\r\n");
    printf("Creates wave banks for use on the Xbox.\r\n");
    printf("\r\n");
}


/****************************************************************************
 *
 *  Usage
 *
 *  Description:
 *      Prints usage information.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      int: -1.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "Usage"

int
Usage
(
    void
)
{
    Banner();
    
    printf(EXENAME " [/N] [/L] <project>\r\n");
    printf("\r\n");
    printf("Usage:\r\n");
    printf("\r\n");
    printf("  project       Specifies the project file to build.\r\n");
    printf("  /N            Prevents destination files from being overwritten if they\r\n");
    printf("                already exist.\r\n");
    printf("  /L            Does not print the banner.\r\n");
    printf("\r\n");

    return -1;
}


/****************************************************************************
 *
 *  DebugCallback
 *
 *  Description:
 *      Callback function for debug messages.
 *
 *  Arguments:
 *      DWORD [in]: debug level.
 *      LPCSTR [in]: debug string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "DebugCallback"

void CALLBACK
DebugCallback
(
    DWORD                   dwLevel,
    LPCSTR                  pszString
)
{
    if(dwLevel <= DPFLVL_WARNING)
    {
        printf("\r\n");

        if(dwLevel < DPFLVL_WARNING)
        {
            printf("Error: %s", pszString);
        }
        else
        {
            printf("Warning: %s", pszString);
        }
        
        printf("\r\n");
    }
}


/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      int [in]: argument count.
 *      char ** [in]: arguments.
 *
 *  Returns:  
 *      int: 0 on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "main"

int __cdecl
main
(
    int                     argc,
    char **                 argv
)
{
    DWORD                   dwSwitches          = 0;
    LPSTR                   pszPrjFile          = NULL;
    HRESULT                 hr                  = S_OK;
    CWaveBankProject        Project;
    CPrintBoy               PrintBoy;
    CConsoleCallback        Callback(PrintBoy);
    LPCSTR                  psz;
    int                     i;

    //
    // Initialize globals
    //
    
    CDebug::m_pfnDpfCallback = DebugCallback;

    //
    // Parse the command-line
    //

    for(i = 1; (i < argc) && SUCCEEDED(hr); i++)
    {
        if(('/' == argv[i][0]) || ('-' == argv[i][0]))
        {
            for(psz = &argv[i][1]; *psz && SUCCEEDED(hr); psz++)
            {
                switch(*psz)
                {
                    case 'N':
                    case 'n':
                        dwSwitches |= SWITCH_NOOVERWRITE;
                        break;

                    case 'L':
                    case 'l':
                        dwSwitches |= SWITCH_NOLOGO;
                        break;

                    default:
                        hr = Usage();
                        break;
                }
            }
        }
        else if(!pszPrjFile)
        {
            pszPrjFile = argv[i];
        }
        else
        {
            hr = Usage();
        }
    }

    if(SUCCEEDED(hr) && !pszPrjFile)
    {
        hr = Usage();
    }

    if(SUCCEEDED(hr) && !(dwSwitches & SWITCH_NOLOGO))
    {
        Banner();
    }

    //
    // Initialize the wave bank project
    //

    if(SUCCEEDED(hr))
    {
        hr = Project.ReadProjectData(pszPrjFile);
    }

    //
    // Generate files
    //

    if(SUCCEEDED(hr))
    {
        hr = Project.Generate(&Callback, !(dwSwitches & SWITCH_NOOVERWRITE));
    }

    //
    // Success
    //

    if(SUCCEEDED(hr) && !(dwSwitches & SWITCH_NOLOGO))
    {
        printf("\r\nSuccess!\r\n\r\n");
    }

    return hr;
}


