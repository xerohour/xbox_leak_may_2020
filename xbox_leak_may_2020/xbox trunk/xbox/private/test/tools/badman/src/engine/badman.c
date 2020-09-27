/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    dllmain.c

Abstract:

    This is the main entrypoint for BadMan. for documentation of how the
    program is called, see the ReadCommandLine function in this file.

Author:

    John Miller (johnmil) 20-Jan-1992

Environment:

    XBox

Revision History:

    01-Apr-2000     schanbai

        Ported to XBox and made the code more readable

--*/

#include "precomp.h"
#include <parsedef.h>
#include <teststat.h>


UINT DefaultErrorMode = 0;
char BMRoot[MAX_PATH];


void
__cdecl
badman_main(
    int    argc,
    char   **argv,
    HANDLE LogHandle
    )
{
    HANDLE hFile, hLog;
    int BMLen, i, NumVar;
    char buff[MAX_PATH];
    WCHAR wbuffer[MAX_PATH];
    CLType CLOptions;                               // Command Line Options as entered on the command line
    DefineType Hash[HASH_SIZE] = { 0 };             // List of definitions for cases
    MacroType MacroHash[MACRO_HASH_SIZE] = { 0 };   // List of Macros for cases
    StringType DataTypes[DATA_HASH_SIZE] = { 0 };   // Valid data types for API's parameters
    WIN32_FIND_DATA FDHold;

    ModType BadmanModules[BADMAN_MODULE_COUNT] = {  // Names of libraries to look-up API's and functions from
        { "apicln.dll",   NULL },
        { "setcln.dll",   NULL },
        { "validate.dll", NULL }
    };
    
    ModType OtherModules[MAX_OTHER_MODULES] = { 0 };

    hLog = LogHandle;
    ASSERT( hLog != INVALID_HANDLE_VALUE && hLog != NULL );

    BMRoot[0] = XTestLibGetTestNameSpace();
    BMRoot[1] = ':';
    BMRoot[2] = 0;
    
    //
    // ReadCommandLine returns FALSE if bad options specified
    //
    
    if ( !ReadCommandLine( argc, argv, &CLOptions ) ) {
        DbgPrint( "BADMAN: Bad option(s) in command line, check your testini.ini\n" );
        ExitThread( EXIT_FAILURE );
    }
    
    //
    // Initialize statistics
    //
    if ( CLOptions.Flags & CLOPT_PERFORMANCE ) {
        
        if ( TestStatOpen(
                (USHORT)CLOptions.MinPerfIterations,
                (USHORT)CLOptions.MaxPerfIterations ) ) {

            DbgPrint( "BADMAN: Unable to initialize Statistics, exiting...\n" );
            ExitThread( EXIT_FAILURE );
        }
    }
    
    strcpy( buff, BMRoot );
    BMLen = strlen(BMRoot);
    
    //
    // Read in the data types into the data hash table. This will be all files
    // in \inc named "*.bmd"
    //
    
    strcpy( &buff[BMLen], "\\inc\\*.bmd" );
    hFile = FindFirstFile( buff, &FDHold );
    
    if ( hFile != INVALID_HANDLE_VALUE ) {
        do {
            strcpy( &buff[ BMLen + sizeof("\\inc\\")-1 ], FDHold.cFileName );
            ReadStringHash( buff, DataTypes, DATA_HASH_SIZE );
        } while ( FindNextFile( hFile, &FDHold) );
    }
    
    FindClose(hFile);
    
    //
    // Now we need to read in all the case names and defines from the cases
    // files. This will read in all files named *.bmh in \inc
    //
    
    strcpy( &buff[BMLen], "\\inc\\*.bmh" );
    hFile = FindFirstFile( buff, &FDHold );

    if ( hFile != INVALID_HANDLE_VALUE) {
        do {
            strcpy( &buff[BMLen + strlen("\\inc\\")], FDHold.cFileName );
            ReadDefines( buff, Hash, MacroHash );
        } while ( FindNextFile( hFile, &FDHold ) );
    }

    FindClose(hFile);

    //
    // Get the handles to the libraries
    //

    for ( i=0; i<BADMAN_MODULE_COUNT; i++ ) {

        swprintf( wbuffer, L"%S", BadmanModules[i].Name );
        BadmanModules[i].h = XLoadModule( wbuffer );
        
        if ( BadmanModules[i].h == NULL ) {
            
            DbgPrint(
                "BADMAN: Unable to load library %s, LastError = %d (%s)\n",
                buff,
                GetLastError(),
                WinErrorSymbolicName(GetLastError())
                );
            
            ExitThread( EXIT_FAILURE );
        }
    }

    //
    // Parse the test script and execute tests as parsed
    //

    NumVar = Parse(
        Hash,
        MacroHash,
        DataTypes,
        &CLOptions,
        BadmanModules,
        OtherModules,
        hLog
        );
    
    //
    // Make sure we kick those nasty libraries out of memory
    //
    
    for ( i=0; i<BADMAN_MODULE_COUNT; i++ ) {
        if ( BadmanModules[i].h ) {
            XFreeModule( BadmanModules[i].h );
        }
    }
    
    for ( i=0; i<MAX_OTHER_MODULES; i++ ) {
        if ( OtherModules[i].h ) {
            XFreeModule( OtherModules[i].h );
        }
        if ( OtherModules[i].Name ) {
            free( OtherModules[i].Name );
        }
    }

    for ( i=0; i<HASH_SIZE; i++ ) {
        FreeDefineType( &Hash[i] );
    }

    for ( i=0; i<DATA_HASH_SIZE; i++ ) {
        FreeStringType( &DataTypes[i] );
    }

    for ( i=0; i<MACRO_HASH_SIZE; i++ ) {
        FreeMacroType( &MacroHash[i] );
    }

    //
    // Get rid of the test stats stuff
    //
    
    if ( CLOptions.Flags & CLOPT_PERFORMANCE ) {
        TestStatClose();
    }
}


BOOL
ReadCommandLine(
    int    argc,
    char   **argv,
    CLType *CLOptions
    )

/*++

Routine Description:

    Parse the command line with which BadMan was invoked.  If there are any
    errors, the value FALSE will be returned, and BadMan will probably exit
    without ever bothering to go any further.

Arguments:

Return Value:

--*/

{
    BOOL bRet;
    int  i, j;
    
    //
    // Establish the defaults for the CL options.
    //
    
    CLOptions->Flags = CLOPT_DEFAULT | CLOPT_NTLOG;
    CLOptions->APIName[0] = '\0';
    CLOptions->VarNum = -1;
    CLOptions->MinPerfIterations = 10;
    CLOptions->MaxPerfIterations = 100;
    
    strcpy(CLOptions->RunFile, BMRoot);
    strcat(CLOptions->RunFile, "\\scripts\\test.bms");
    
    bRet = TRUE;
    
    //
    // If there is a param with no other params, it's the script
    // Note: it gets set here, not in the for loop.
    //
    if ( (argc > 1) && (argv[1][0] != '-') ) {
        i = 2;
        if ( (argv[1][0] && argv[1][1] != ':') || argv[1][0] != '\\' ) {
            strcpy( CLOptions->RunFile, BMRoot );
            strcat( CLOptions->RunFile, "\\scripts\\" );
        }
        strcat( CLOptions->RunFile, argv[1]);
    } else {
        i = 1;
    }
    
    //
    // Loop through and process each token in the CL i was initialized above.
    //
    for ( ; bRet && (i < argc); i++) {
        
        if ( (argv[i][0] != '-') || (argv[i][2] != '\0') ) {
            bRet = FALSE;
        } else {
            switch ( argv[i][1] ) {

            case 'a': // Specify a particular API to test
            case 'A':
                CLOptions->Flags |= CLOPT_API;
                
                if ( i+1 == argc ) {
                    bRet = FALSE;
                } else {
                    strcpy( CLOptions->APIName, argv[++i] );
                }
                
                break;

            case 'b': // Specify bad parameter testing
            case 'B':
                CLOptions->Flags |= CLOPT_BAD;
                break;
                
            case 'c': // Specify we should only parse the script for correctness
            case 'C':
                CLOptions->Flags |= CLOPT_PARSEONLY;
                break;

            case 'd': // Execute DebugBreak() before API Test calls
            case 'D':
                CLOptions->Flags |= CLOPT_DBGBREAK;
                break;
                
            case 'e': // Cases generating exceptions are to be treated as successes
            case 'E':
                CLOptions->Flags |= CLOPT_NOEXCEPT;
                break;

            case 'f': // Generate performance data
            case 'F':
                CLOptions->Flags |= CLOPT_PERFORMANCE;
                
                if ( (argc > i + 2) && !_strnicmp( argv[i + 1], "-min", 4) ) {

                    j = atoi( argv[i + 2] );

                    if ( j < MIN_TIMING_ITERATIONS ) {
                        DbgPrint("BADMAN: Parameter error: -f -min must be >= %d\n", MIN_TIMING_ITERATIONS );
                        bRet = FALSE;
                    } else {
                        CLOptions->MinPerfIterations = j;
                    }

                    i += 2;
                }
                
                if ( (argc > i + 2) && !_strnicmp(argv[i + 1], "-max", 4) ) {

                    j = atoi( argv[i + 2] );

                    if ( j > MAX_TIMING_ITERATIONS) {
                        DbgPrint("BADMAN: Parameter error: -f -max must be <= %d\n", MAX_TIMING_ITERATIONS );
                        bRet = FALSE;
                    } else {
                        CLOptions->MaxPerfIterations = j;
                    }

                    i += 2;
                }

                break;

            case 'g': // Specify good parameter testing, shut off DEFAULT bad parameter testing
            case 'G':
                CLOptions->Flags |= CLOPT_GOOD;
                CLOptions->Flags &= ~CLOPT_BAD;

                //
                // If bad is explicitly requested, turn it back on.
                //

                for ( j = 1; j < argc; j++ ) {
                    
                    if ( argv[j][0] == '-' &&  argv[j][2] == '\0' &&
                         (argv[j][1] == 'b' || argv[j][1] == 'B') ) { 

                        CLOptions->Flags |= CLOPT_BAD;
                    }
                }

                break;

            case 'p': // Perform permutatively rather than linearly
            case 'P':
                CLOptions->Flags |= CLOPT_PERMUTE;
                break;

            case 's': // Specify the script file explicitly
            case 'S':
                if ( (i + 1) == argc ) {
                    bRet = FALSE;
                } else {
                    strcpy( CLOptions->RunFile, argv[++i] );
                }
                break;

            case 't':
            case 'T':
                CLOptions->Flags |= CLOPT_TEST;
                break;

            case 'v': // Specify a variation number to test
            case 'V':
                CLOptions->Flags |= CLOPT_VARIATION;
                
                if ( (i + 1) == argc ) {
                    bRet = FALSE;
                } else {
                    CLOptions->VarNum = atoi(argv[++i]);
                }
                break;

            case 'x': // Set documentation flag, turn off default testing flag
            case 'X':
                CLOptions->Flags |= CLOPT_DOCUMENT;
                CLOptions->Flags &= ~CLOPT_TEST;

                //
                // If testing is explicitly requested, turn it back on.
                //
                for (j = 1; j < argc; j++) {
                    if ( argv[j][0] == '-' && argv[j][2] == '\0' &&
                         (argv[j][1] == 't' || argv[j][1] == 'T') ) {

                        CLOptions -> Flags |= CLOPT_TEST;
                    }
                }

                break;

            case 'y': // Perform Verification testing if available
            case 'Y':
                CLOptions->Flags |= CLOPT_VALIDATE;
                break;

            case 'z': // Print out the api params/results for success as well as bad cases
            case 'Z':
                CLOptions->Flags |= CLOPT_PRINT_ALL_CALLS;
                break;
                
            default:
                bRet = FALSE;
                break;
            }
        }
    }
    
    //
    // If there was an error (or -?, which creates an error), print a
    // description of how BadMan works.
    //
    
    if ( !bRet ) {

        DbgPrint("BADMAN: ================================================================\n");
        DbgPrint("BADMAN: A program to perform bad parameter testing\n");
        DbgPrint("BADMAN: Usage: BadMan [<ScriptName>] {flags}\n");
        DbgPrint("BADMAN: Valid Flags:\n");
        DbgPrint("BADMAN:     -?             See this message\n");
        DbgPrint("BADMAN:     -a <API>       Test only script entries for <API>\n");
        DbgPrint("BADMAN:     -b             Perform Bad parameter testing\n");
        DbgPrint("BADMAN:     -c             Parse file ONLY for correctness\n");
        DbgPrint("BADMAN:     -d             DebugBreak() before API test call\n");
        DbgPrint("BADMAN:     -e             Treat exceptions as successes\n");
        DbgPrint("BADMAN:     -f [-min <int>] [-max <int>]\n");
        DbgPrint("BADMAN:                    Generate perFormance Data\n");
        DbgPrint("BADMAN:                        -min = specify minimum # of iterations\n");
        DbgPrint("BADMAN:                        -max = specify maximum # of iterations\n");
        DbgPrint("BADMAN:     -g             Perform Good parameter testing\n");
        DbgPrint("BADMAN:                        (-g cancels default -b)\n");
        DbgPrint("BADMAN:     -p             Test permutations rather than linear combos\n");
        DbgPrint("BADMAN:     -s <FName>     Specify Script File\n");
        DbgPrint("BADMAN:     -t             Perform testing\n");
        DbgPrint("BADMAN:     -v <VNum>      Run variation # <VNum> for selected entries\n");
        DbgPrint("BADMAN:     -x             Print documentation of coverage\n");
        DbgPrint("BADMAN:                        (-x cancels default -t)\n");
        DbgPrint("BADMAN:     -y             verifY result of API call if available\n");
        DbgPrint("BADMAN:     -z             Print params for good cases as well as bad\n");
        DbgPrint("BADMAN: Defaults: Test.bms -b -t\n");
        DbgPrint("BADMAN: ================================================================\n");
        
    }

    return bRet;
}
