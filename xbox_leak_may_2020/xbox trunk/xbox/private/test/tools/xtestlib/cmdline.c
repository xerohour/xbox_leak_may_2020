/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    cmdline.c

Abstract:

    Command line processing routines

Author:

    Sakphong Chanbai (schanbai) 08-Mar-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"


static
VOID
Parse_CmdlineW(
    IN LPWSTR cmdstart,
    IN LPWSTR * argv OPTIONAL,
    OUT LPWSTR lpstr OPTIONAL,
    OUT INT *numargs,
    OUT INT *numbytes
    )

/*++

Routine Description:

    Parses the command line and sets up the Unicode argv[] array.
    On entry, cmdstart should point to the command line,
    argv should point to memory for the argv array, lpstr
    points to memory to place the text of the arguments.
    If these are NULL, then no storing (only counting)
    is done.  On exit, *numargs has the number of
    arguments (plus one for a final NULL argument),
    and *numbytes has the number of bytes used in the buffer
    pointed to by args.

Arguments:

    cmdstart - pointer to command line of the form <progname><nul><args><nul>
    argv - where to build argv array; NULL means don't build array
    lpstr - where to place argument text; NULL means don't store text
    numargs - returns number of argv entries created
    numbytes - number of bytes used in args buffer

Return Value:

    None

--*/

{
    LPWSTR p;
    WCHAR c;
    INT inquote;                    // 1 = inside quotes
    INT copychar;                   // 1 = copy char to *args
    WORD numslash;                  // Num of backslashes seen

    *numbytes = 0;
    *numargs = 1;                   // The program name at least

    // First scan the program name, copy it, and count the bytes
    p = cmdstart;
    if (argv) {
        *argv++ = lpstr;
    }

    //
    // A quoted program name is handled here. The handling is much
    // simpler than for other arguments. Basically, whatever lies
    // between the leading double-quote and next one, or a terminal null
    // character is simply accepted. Fancier handling is not required
    // because the program name must be a legal NTFS/HPFS file name.
    // Note that the double-quote characters are not copied, nor do they
    // contribute to numbytes.
    //
    if (*p == L'\"') {
        //
        // Scan from just past the first double-quote through the next
        // double-quote, or up to a null, whichever comes first
        //
        while ((*(++p) != L'\"') && (*p != L'\0')) {
            *numbytes += sizeof(WCHAR);
            if (lpstr) {
                *lpstr++ = *p;
            }
        }

        // Append the terminating null
        *numbytes += sizeof(WCHAR);
        if (lpstr) {
            *lpstr++ = L'\0';
        }

        // If we stopped on a double-quote (usual case), skip over it
        if (*p == L'\"') {
            p++;
        }
    } else {
        // Not a quoted program name
        do {
            *numbytes += sizeof(WCHAR);
            if (lpstr) {
                *lpstr++ = *p;
            }

            c = (WCHAR) *p++;

        } while (c > L' ');

        if (c == L'\0') {
            p--;
        } else {
            if (lpstr) {
                *(lpstr - 1) = L'\0';
            }
        }
    }

    inquote = 0;

    // Loop on each argument
    for ( ; ; ) {
        if (*p) {
            while (*p == L' ' || *p == L'\t') {
                ++p;
            }
        }

        if (*p == L'\0') {
            break;                  // End of args
        }

        // Scan an argument
        if (argv) {
            *argv++ = lpstr;        // Store ptr to arg
        }

        ++*numargs;

        // Loop through scanning one argument
        for ( ; ; ) {
            copychar = 1;

            //
            // Rules: 2N backslashes + " ==> N backslashes and begin/end quote
            //        2N+1 backslashes + " ==> N backslashes + literal "
            //        N backslashes ==> N backslashes
            //
            numslash = 0;
            while (*p == L'\\') {
                // Count number of backslashes for use below
                ++p;
                ++numslash;
            }

            if (*p == L'\"') {
                // if 2N backslashes before, start/end quote, otherwise copy literally
                if (numslash % 2 == 0) {
                    if (inquote) {
                        if (p[1] == L'\"') {
                            p++;        // Double quote inside quoted string
                        } else {        // skip first quote char and copy second
                            copychar = 0;
                        }
                    } else {
                        copychar = 0;   // Don't copy quote
                    }

                    inquote = !inquote;
                }
                numslash /= 2;          // Divide numslash by two
            }

            // Copy slashes
            while (numslash--) {
                if (lpstr) {
                    *lpstr++ = L'\\';
                }
                *numbytes += sizeof(WCHAR);
            }

            // If at end of arg, break loop
            if (*p == L'\0' || (!inquote && (*p == L' ' || *p == L'\t'))) {
                break;
            }

            // copy character into argument
            if (copychar) {
                if (lpstr) {
                    *lpstr++ = *p;
                }
                *numbytes += sizeof(WCHAR);
            }
            ++p;
        }

        // Null-terminate the argument
        if (lpstr) {
            *lpstr++ = L'\0';         // terminate string
        }
        *numbytes += sizeof(WCHAR);
    }

}


static
VOID
Parse_CmdlineA(
    IN LPSTR cmdstart,
    IN LPSTR * argv OPTIONAL,
    OUT LPSTR lpstr OPTIONAL,
    OUT INT *numargs,
    OUT INT *numbytes
    )

/*++

Routine Description:

    Parses the command line and sets up the ANSI argv[] array.
    On entry, cmdstart should point to the command line,
    argv should point to memory for the argv array, lpstr
    points to memory to place the text of the arguments.
    If these are NULL, then no storing (only counting)
    is done.  On exit, *numargs has the number of
    arguments (plus one for a final NULL argument),
    and *numbytes has the number of bytes used in the buffer
    pointed to by args.

Arguments:

    cmdstart - pointer to command line of the form <progname><nul><args><nul>
    argv - where to build argv array; NULL means don't build array
    lpstr - where to place argument text; NULL means don't store text
    numargs - returns number of argv entries created
    numbytes - number of bytes used in args buffer

Return Value:

    None

--*/

{
    LPSTR p;
    CHAR c;
    INT inquote;                    // 1 = inside quotes
    INT copychar;                   // 1 = copy char to *args
    WORD numslash;                  // Num of backslashes seen

    *numbytes = 0;
    *numargs = 1;                   // The program name at least

    // First scan the program name, copy it, and count the bytes
    p = cmdstart;
    if (argv) {
        *argv++ = lpstr;
    }

    //
    // A quoted program name is handled here. The handling is much
    // simpler than for other arguments. Basically, whatever lies
    // between the leading double-quote and next one, or a terminal null
    // character is simply accepted. Fancier handling is not required
    // because the program name must be a legal NTFS/HPFS file name.
    // Note that the double-quote characters are not copied, nor do they
    // contribute to numbytes.
    //
    if (*p == '\"') {
        //
        // Scan from just past the first double-quote through the next
        // double-quote, or up to a null, whichever comes first
        //
        while ((*(++p) != '\"') && (*p != '\0')) {
            *numbytes += sizeof(CHAR);
            if (lpstr) {
                *lpstr++ = *p;
            }
        }

        // Append the terminating null
        *numbytes += sizeof(CHAR);
        if (lpstr) {
            *lpstr++ = '\0';
        }

        // If we stopped on a double-quote (usual case), skip over it
        if (*p == '\"') {
            p++;
        }
    } else {
        // Not a quoted program name
        do {
            *numbytes += sizeof(CHAR);
            if (lpstr) {
                *lpstr++ = *p;
            }

            c = (CHAR) *p++;

        } while (c > ' ');

        if (c == '\0') {
            p--;
        } else {
            if (lpstr) {
                *(lpstr - 1) = '\0';
            }
        }
    }

    inquote = 0;

    // Loop on each argument
    for ( ; ; ) {
        if (*p) {
            while (*p == ' ' || *p == '\t') {
                ++p;
            }
        }

        if (*p == '\0') {
            break;                  // End of args
        }

        // Scan an argument
        if (argv) {
            *argv++ = lpstr;        // Store ptr to arg
        }

        ++*numargs;

        // Loop through scanning one argument
        for ( ; ; ) {
            copychar = 1;

            //
            // Rules: 2N backslashes + " ==> N backslashes and begin/end quote
            //        2N+1 backslashes + " ==> N backslashes + literal "
            //        N backslashes ==> N backslashes
            //
            numslash = 0;
            while (*p == '\\') {
                // Count number of backslashes for use below
                ++p;
                ++numslash;
            }

            if (*p == '\"') {
                // if 2N backslashes before, start/end quote, otherwise copy literally
                if (numslash % 2 == 0) {
                    if (inquote) {
                        if (p[1] == '\"') {
                            p++;        // Double quote inside quoted string
                        } else {        // skip first quote char and copy second
                            copychar = 0;
                        }
                    } else {
                        copychar = 0;   // Don't copy quote
                    }

                    inquote = !inquote;
                }
                numslash /= 2;          // Divide numslash by two
            }

            // Copy slashes
            while (numslash--) {
                if (lpstr) {
                    *lpstr++ = '\\';
                }
                *numbytes += sizeof(CHAR);
            }

            // If at end of arg, break loop
            if (*p == '\0' || (!inquote && (*p == ' ' || *p == '\t'))) {
                break;
            }

            // copy character into argument
            if (copychar) {
                if (lpstr) {
                    *lpstr++ = *p;
                }
                *numbytes += sizeof(CHAR);
            }
            ++p;
        }

        // Null-terminate the argument
        if (lpstr) {
            *lpstr++ = '\0';         // terminate string
        }
        *numbytes += sizeof(CHAR);
    }

}


LPWSTR *
NTAPI
CommandLineToArgvW(
    IN LPCWSTR lpCmdLine,
    OUT int * pNumArgs
    )

/*++

Routine Description:

    Read the command line and create the argv array for C programs.

Arguments:

    lpCmdLine - [in] Pointer to a null-terminated Unicode command-line string.
        An application will usually directly pass on the value returned by a 
        call to the GetCommandLineW function. If this parameter is the empty 
        string, "", the function returns the path to the current executable file. 

    pNumArgs - [out] Pointer to an integer variable that receives the count of 
        arguments parsed. 

Return Value:

    If the function succeeds, the return value is a non-NULL pointer to the 
    constructed argument list, which is an array of Unicode strings. 
    
    If the function fails, the return value is NULL. 
    To get extended error information, call GetLastError. 

--*/

{
    LPWSTR * argv_U;
    LPWSTR   cmdstart;  // start of command line to parse
    INT      numbytes;
    WCHAR    pgmname[MAX_PATH];

    if (pNumArgs == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    // BUGBUG: remove comment below when xapi is ready for us
    // Get the program name pointer from Win32 Base
    //GetModuleFileName(NULL, pgmname, sizeof(pgmname) / sizeof(WCHAR));

    //
    // If there's no command line at all (won't happen from cmd.exe, but
    //   possibly another program), then we use pgmname as the command line
    //   to parse, so that argv[0] is initialized to the program name
    //
    cmdstart = (*lpCmdLine == TEXT('\0')) ? pgmname : (LPWSTR)lpCmdLine;

    // First find out how much space is needed to store args
    Parse_CmdlineW(cmdstart, NULL, NULL, pNumArgs, &numbytes);

    // Allocate space for argv[] vector and strings
    argv_U = (LPWSTR*)GlobalAlloc(LMEM_ZEROINIT, (*pNumArgs+1) * sizeof(LPWSTR) + numbytes);

    if (!argv_U) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    // Store args and argv ptrs in just allocated block
    Parse_CmdlineW(cmdstart, argv_U,
                  (LPWSTR)(((LPBYTE)argv_U) + *pNumArgs * sizeof(LPWSTR)),
                  pNumArgs, &numbytes
                 );

    return argv_U;
}


LPSTR *
NTAPI
CommandLineToArgvA(
    IN LPCSTR lpCmdLine,
    OUT int * pNumArgs
    )

/*++

Routine Description:

    Read the command line and create the argv array for C programs.

Arguments:

    lpCmdLine - [in] Pointer to a null-terminated ANSI command-line string.
        An application will usually directly pass on the value returned by a 
        call to the GetCommandLineA function. If this parameter is the empty 
        string, "", the function returns the path to the current executable file. 

    pNumArgs - [out] Pointer to an integer variable that receives the count of 
        arguments parsed. 

Return Value:

    If the function succeeds, the return value is a non-NULL pointer to the 
    constructed argument list, which is an array of ANSI strings. 
    
    If the function fails, the return value is NULL. 
    To get extended error information, call GetLastError. 

--*/

{
    LPSTR * argv_U;
    LPSTR   cmdstart;  // start of command line to parse
    INT     numbytes;
    CHAR    pgmname[MAX_PATH];

    if (pNumArgs == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    // BUGBUG: remove comment below when xapi is ready for us
    // Get the program name pointer from Win32 Base
    //GetModuleFileName(NULL, pgmname, sizeof(pgmname) / sizeof(WCHAR));

    //
    // If there's no command line at all (won't happen from cmd.exe, but
    //   possibly another program), then we use pgmname as the command line
    //   to parse, so that argv[0] is initialized to the program name
    //
    cmdstart = (*lpCmdLine == '\0') ? pgmname : (LPSTR)lpCmdLine;

    // First find out how much space is needed to store args
    Parse_CmdlineA(cmdstart, NULL, NULL, pNumArgs, &numbytes);

    // Allocate space for argv[] vector and strings
    argv_U = (LPSTR*)GlobalAlloc(LMEM_ZEROINIT, (*pNumArgs+1) * sizeof(LPSTR) + numbytes);

    if (!argv_U) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    // Store args and argv ptrs in just allocated block
    Parse_CmdlineA(cmdstart, argv_U,
                  (LPSTR)(((LPBYTE)argv_U) + *pNumArgs * sizeof(LPSTR)),
                  pNumArgs, &numbytes
                 );

    return argv_U;
}
