#include "trisupp.h"

void Usage() 
    {
    _tprintf(_T("\nUsage: Triage <debuggername> <pipename> [-nosym] [-ini:inifile]\n\n")
        _T("       Triage is a utility provided by the NtStress team to simplify debugging\n")
        _T("       and problem diagnosis.\n\n")
        _T("       Simply specify debugger name and pipename on the triage command line and\n")
        _T("       it will connect to the remote debugger and return a problem diagnosis,\n")
        _T("       stack trace and suggested followup.\n\n")
        _T("       The problem assignment is based on the module and function names in the\n")
        _T("       stack which are mapped into the triage.ini file (located with the exe).\n\n")
        _T("       -nosym turns off symbols checking and fixing.\n")
        _T("       -ini:filename sets the inifile name to 'filename' default = triage.ini\n\n")
        _T("       For more information on triage or feature requests, please mail NtStress.\n"));
    }


void __cdecl main(int argc, TCHAR **argv)
    {
    PTCHAR TextBuffer = NULL;
    PTCHAR iniFile = NULL;
    DWORD dwFlags = 0;
    DWORD debugarg = 1;
    INT i = 0;
    BOOL DebFound = FALSE;
    
    if ((argc<3)||(argc>5)) 
        {
        Usage();
        return;
        }
    
    for (i=1; i<argc; i++)
        {
        if ((argv[i][0] == _T('-'))||(argv[i][0] == _T('/')))
            {
            if ((!_tcsicmp(_T("-nosym"), argv[i]))||(!_tcsicmp(_T("/nosym"), argv[i])))
                dwFlags = (dwFlags | FLAG_NO_SYMBOLS);
            else if ((!_tcsnicmp(_T("-ini:"), argv[i], 5))||(!_tcsnicmp(_T("/ini:"), argv[i], 5)))
                {
                if ((*((argv[i])+5)) != _T('\0'))
                    iniFile = (argv[i])+5;
                }
            else
                {
                Usage();
                return;
                }
            }
        else
            {
            if (((i+1) < argc)&&(!DebFound))
                {
                debugarg = i;
                i++;
                DebFound = TRUE;
                }
            else
                {
                Usage();
                return;
                }
            }
        }
    
    if (TextBuffer = Triage(argv[debugarg],argv[debugarg +1], dwFlags, iniFile))
        _tprintf("%s\n", TextBuffer);
    else 
        _tprintf(_T("No Buffer Returned from Trisupp.dll\n\n \n\nFOLLOWUP: xstress\n"));
    
    TriageCleanup();
    
    return;
    }
