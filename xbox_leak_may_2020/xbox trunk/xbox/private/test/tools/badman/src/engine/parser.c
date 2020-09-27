/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    parser.c

Abstract:

    This module contains helper functions for the Parser for BADMAN

Author:

    John Miller (johnmil) 20-Jan-1992

Environment:

    XBox

Revision History:

    31-Mar-1995     TimF

        Made it readable

    01-Apr-2000     schanbai

        Ported to XBox and made the code more readable

--*/


#include "precomp.h"
#include <ctype.h>
#include <parsedef.h>


int
Parse(
    DefineType  Hash[HASH_SIZE],
    MacroType   MacroHash[MACRO_HASH_SIZE],
    StringType  DataTypes[DATA_HASH_SIZE],
    CLType      *CLOptions,
    ModType     *BadmanModules,
    ModType     *OtherModules,
    HANDLE      hLog

/*++

Routine Description:

    Parse will parse a command script, given the file name and a hash table
    with DEFINE's in it, and call the test procedure after parsing each full
    entry.  An entry looks something like (this is a sample

     HANDLE
     GlobalAlloc(
         DWORD            dwFlags,
         DWORD            dwSize
     )= 0
         dwFlags:
             Good: DWORD_GOODGLOBALFLAG
             Bad: DWORD_BADGLOBALFLAG
         dwSize:
             Good: DWORD_GOODSIZE
             Bad:  DWORD_BADSIZE
         dwFlags:
             Good: DWORD_SPECIALGOODFLAG
             Bad: DWORD_SPECIALBADFLAG
                 Depend:
                     dwSize:
                         Good: DWORD_SPECIALGOODSIZE
                         Bad:  DWORD_SPECIALBADSIZE
                 EndDepend:

    Parse is essentially a state machine. As each token (string)is read in, it
    is treated according to what the state variable is set to. It may be
    sometimes that we are reading in white space, since white space is one of
    the terminators for the SCANF.  In this case, we don't process it.
    Otherwise, the Scanf will read in a string, terminating on special symbols
    and spaces.  The terminator is then read in and put into the TERM variable.
    This is because BAD with a colon as a terminator is perfectly legal, but
    BAD with a '(' as a terminator is not.

Arguments:

Return Value:

--*/

)
{
    APIType       API = {0};        // Stores one record
    BadValType    *BTrav = NULL;    // Traversing pointer to 'bad case' list
    BadValType    *BHold;           // Temporary variable to hold Bad list elements.
    BOOL          NoPush = FALSE;   
    BOOL          bInComment;       // An open-comment was read
    BOOL          space;            // Boolean: is terminator white space?
    BOOL          Found;            
    char          token[1024];      // The current token (word) being parsed
    char          term;             // The current terminator of the current token
    char          *TypeName;        // Name of a datatype
    char          *tTrav;
    char          buff[128];
    char          *NextAPIType = 0; // API type signals end of valid record. But we have to save it for the NEXT record.
    DefineType    *DTrav;           // Temp pointer to a place in the defines list
    FILE          *inFPtr;          // Pointer to the script file being parsed
    GoodValType   *GTrav = NULL;    // Traversing pointer to 'Good case' list
    HANDLE        hConOut;          // Handle to STDOut for DLL output
    PARSER_STATE  state;            // State # the state machine is in
    int           i=0, j, k;        // Counting vars
    int           LineCount;        // Line number being read in the script
    int           NumVar;           // Number of variations documented/run
    MacroType     *MTrav;           // Traversing pointer Used for Parsing macros
    MacroValType  *MVTrav;          // Traversing pointer for Macro case lists
    S_PARMS       *PTrav = NULL;    // Parm traversing pointers
    S_PARMS       *PTrav2;          // Parm traversing pointers
    WCHAR         wbuffer[MAX_PATH];

    //
    // PD contains the current parameter list level.  PD.next is the current
    // level, PD.next->next the next most current, and so on.
    //
    ParmDepType   *PDTrav, PD = { 0 };

    NumVar = 0;
    LineCount = 0;
    state = STATE_BEGIN;

    //
    // Set a handle to INVALID_HANDLE_VALUE. We don't have standard output in XBox
    //

    hConOut = INVALID_HANDLE_VALUE;

    //
    // Open the input file.
    //

    if ( NULL == (inFPtr = fopen( CLOptions->RunFile, "r" )) ) {   
        DbgPrint( "BADMAN: Parse: Unable to open %s for input\n", CLOptions->RunFile );
        ExitThread( EXIT_FAILURE );
    }

    bInComment = FALSE;

    //
    // We're going to loop through the entire file.  a string is terminated by
    // any of \ \t \n, ()= or :  This loop continues until the file is
    // completely read.
    //

    while ( fscanf( inFPtr, "%[^ \t\n,()=:]", token ) != EOF ) {

        ASSERT( strlen(token) < sizeof(token) );

        //
        // The terminator differentiates between different commands.
        //

        //
        // fgetc returns an int, term is a char...  It is possible that the
        // fgetc will return EOF, but it doesn't look like it matters.  TimF
        //

        term = (char)fgetc(inFPtr);

        if ( term == '\n' ) {
            LineCount++;
        }

        //
        // space indicates that the token currently being read was terminated
        // by space space.
        //

        if ( isspace(term) ) {
            space = TRUE;
        } else {
            space = FALSE;
        }

        j = strlen(token) - 1;

        //
        // if we're in a comment, check for an end comment.
        //

        if ( bInComment ) {
            for ( i=0; i<j; i++ ) {
                if ((token[i] == '*') && (token[i + 1] == '/')) {
                    for (k = i + 2; k < j; k++) {
                        token[k - (i + 2)] = token[k];
                    }

                    token[k - (i + 2)] = '\0';

                    bInComment = FALSE;

                    break;
                }
            }
        }

        //
        // If comment wasn't ended, skip processing the token...
        //

        if (bInComment) {
            continue;
        }

        //
        // If we encounter the start of a comment, gotta process...
        //

        if ( !bInComment ) {
            for ( i=0; i<j; i++ ) {
                if ( token[i] == '/' && token[i + 1] == '*' ) {
                    token[i] = '\0';
                    bInComment = TRUE;

                    for ( k=i+2; k<j; k++ ) {
                        if ( token[k] == '*' && token[k+1] == '/') {
                            bInComment = FALSE;
                        }
                    }

                    break;
                }
            }
        }

        //
        // If we have anything except a blank space for our current token, run
        // it through the state machine.  In general, while in a case, we
        // assume the token read will cause an error until we prove it won't:
        // this cuts down on the code path length.
        //

        if ( (token[0] != '\0') || !space ) {
            //DbgPrint( "\nState %d: [%s]<%c>", state, token, term );

            switch ( state ) {

            case STATE_ERROR:
            case STATE_BEGIN:

                //
                // Unless we find the data type for an API, we're going to
                // stay in either STATE_ERROR or STATE_BEGIN.
                //

                if ( space ) {
                    if ( !_stricmp(token, "module") ) {
                        state = STATE_MODULE_ID;
                    } else {
                        API.Type = FindString(token, DataTypes, DATA_HASH_SIZE);

                        if (API.Type != NULL) {
                            state = STATE_API_TYPE;
                        } else {
                            DbgPrint(
                                "BADMAN: script Error[%d]: '%s' is not a valid API Type.\n",
                                LineCount,
                                token
                                );

                            xLog(
                                hLog,
                                XLL_WARN,
                                "Script error[%d]: '%s' is not a valid API Type",
                                LineCount,
                                token
                                );
                        }
                    }

                }

                break;

            case STATE_MODULE_ID:

                //
                // ':' is a 'term' not a token, but that's what we expect, if
                // we don't get one treat this as a type-def'n.
                //

                if (term == ':') {
                    state = STATE_MODULE_NAME;
                } else {
                    API.Type = FindString(token, DataTypes, DATA_HASH_SIZE);

                    if (API.Type != NULL) {
                        state = STATE_API_TYPE;
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: '%s' is not a valid API Type.\n",
                            LineCount,
                            token
                            );

                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: '%s' is not a valid API type",
                            LineCount,
                            token
                            );
                    }
                }

                break;

            case STATE_MODULE_NAME: {

                //
                // We expect that the current token is a module name.
                //
                // Find the first free slot in the modules list.
                //

                int i;

                for ( i=0; i<MAX_OTHER_MODULES; i++) {
                    if ( !OtherModules[i].h ) {
                        break;
                    }
                }

                if ( i == MAX_OTHER_MODULES ) {
                    DbgPrint("BADMAN: Too many modules defined (max = %d)\n", MAX_OTHER_MODULES);
                } else {
                    //
                    // Attempt to load the specified module.
                    //

                    // BUGBUG: Is the name used other than to load the module ?
                    OtherModules[i].Name = malloc(MAX_PATH);

                    strcpy( OtherModules[i].Name, token );
                    
                    xSetComponent( hLog, "Badman", token );

                    if ( !strchr(OtherModules[i].Name, '.') ) {
                        strcat( OtherModules[i].Name, ".dll" );
                    }

                    swprintf( wbuffer, L"%S", OtherModules[i].Name );
                    OtherModules[i].h = XLoadModule( wbuffer );

                    if ( OtherModules[i].h == NULL ) {
                        //
                        // Attempt to load the specified module failed, die.
                        //

                        DbgPrint(
                            "BADMAN: parser unable to load library %s, LastError = %d (%s)\n",
                            OtherModules[i].Name,
                            GetLastError(),
                            WinErrorSymbolicName(GetLastError())
                            );

                        xLog(
                            hLog,
                            XLL_BLOCK,
                            "Parser unable to load library %s, LastError = %d (%s)",
                            OtherModules[i].Name,
                            GetLastError(),
                            WinErrorSymbolicName(GetLastError())
                            );

                        ExitThread( EXIT_FAILURE );
                    }

                    state = STATE_BEGIN;
                }

                break;
            }

            case STATE_API_TYPE:

                //
                // The type of the api has been successfully read in.
                //

                API.Name = "";
                API.RValue.Type = 0;    // no flags set
                API.Parms.next = NULL;

                //DestroyParmDep(&PD);

                PD.next = NULL;

                PTrav = &API.Parms;

                //
                // This is important: PDTrav->P->next will have the first
                // entry in it, not PDTrav->P!  Note also that the way we'll
                // insert the default Parm values, PD will be pointing at the
                // last parm in the list, whose->next will be the First
                // NON-DEFAULT parm.  Since PTrav->next is the last of the
                // default parms that's what we use, not PTrav.
                //

                PD.next = NewParmDep(PTrav, NULL);

                API.Name = NewCharString(token);

                state = STATE_API_NAME;

                if (!space) {
                    if (term == '(') {
                        state = STATE_API_NEXT_PARAM;
                    } else {
                        state = STATE_ERROR;

                        DbgPrint(
                            "BADMAN: Script Error[%d]: Expected ' ' or '(' after %s, read %c\n",
                            LineCount,
                            token,
                            term
                            );

                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: expected ' ' or '(' after %s, read %c\n",
                            LineCount,
                            token,
                            term
                            );
                    }
                }

                break;

            case STATE_API_NAME:

                //
                // The name of the api has been successfully read in, and the
                // next terminator which is not a space MUST be a '('.
                // Further, if something BESIDES a space was read between the
                // API name and '(', there was an error.
                //

                state = STATE_ERROR;

                if ((token[0] == '\0') && (term == '(')) {
                    state = STATE_API_NEXT_PARAM;
                } else {
                    DbgPrint(
                        "BADMAN: Script Error[%d]: Expected '(', read '%s%c'\n",
                        LineCount,
                        token,
                        term
                        );
                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: expected '(', read '%s%c'\n",
                        LineCount,
                        token,
                        term
                        );
                }

                break;

            case STATE_API_NEXT_PARAM_WITH_COMMA:

                //
                // The opening paren for the arg list has been read else we're
                // checking for a new arg list entry First we're going to find
                // and verify validity of all the parm names and types.  We'll
                // make empty parameter structures for them, which will be
                // filled with default values (the first read in for each
                // parm).
                //

                if (!space) {
                    state = STATE_ERROR;

                    DbgPrint(
                        "BADMAN: Script Error[%d]: Unexpected delimeter '%c'\n",
                        LineCount,
                        term
                        );

                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: unexpected delimeter '%c'\n",
                        LineCount,
                        term
                        );

                    break;
                }

                /* FALL-THROUGH */

            case STATE_API_NEXT_PARAM:

                state = STATE_ERROR;

                //
                // Either we're finished, and see the ')'
                //

                if (token[0] == '\0') {
                    if (term == ')') {
                        state = STATE_API_PARAM_DONE;
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: expected Parm Type or ')', read '%c'\n",
                            LineCount,
                            term
                            );

                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: expected Parm Type or ')', read '%c'\n",
                            LineCount,
                            term
                            );
                    }
                } else {
                    //
                    // or we're seeing a valid data type for a parm
                    // declaration, and will need a parm name.
                    //

                    if (space) {
                        //
                        // if token is a valid data type, new param.  We're
                        // making the new parameter by inserting it at front,
                        // so the list will go [head]->P5->P4->P3->P2
                        //->P1->NULL
                        //

                        if (!_stricmp(token, "NOPUSH")) {
                            /*
                             * We allow a "NOPUSH" directive here.
                             *
                             * NOPUSH is used to tell badman that a parameter
                             * should not be pushed onto the stack, even
                             * though we call appropriate setup and cleanup
                             * functions for it.
                             */

                            NoPush = TRUE;

                            state = STATE_API_NEXT_PARAM;
                        } else if ( FindString(token, DataTypes, DATA_HASH_SIZE) != NULL ) {

                            PTrav->next = NewParm(
                                                FindString(token, DataTypes, DATA_HASH_SIZE),
                                                "",
                                                FALSE,
                                                NoPush,
                                                NULL,
                                                NULL,
                                                NULL,
                                                PTrav->next
                                                );

                            PTrav = PTrav->next;

                            state = STATE_API_PARAM_TYPE;
                            NoPush = FALSE;
                        } else {
                            DbgPrint(
                                "BADMAN: Script Error[%d]: Invalid Parameter TYPE '%s'\n",
                                LineCount,
                                token
                                );
                            xLog(
                                hLog,
                                XLL_WARN,
                                "Script error[%d]: invalid parameter TYPE '%s'\n",
                                LineCount,
                                token
                                );
                        }
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Unexpected delimeter '%c'\n",
                            LineCount,
                            term
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: unexpected delimeter '%c'\n",
                            LineCount,
                            term
                            );
                    }
                }

                break;

            case STATE_API_PARAM_TYPE:

                //
                // A type has been read in, and we need a parameter name to
                // match it.  This routine is ONLY valid while we're actually
                // reading the parameter list between the parenthesis.
                //

                state = STATE_ERROR;
                tTrav = token;
                i = 0;

                while ((*tTrav != '\0') && (*tTrav == '*')) {
                    i++;
                    tTrav++;
                }

                if (ValidParmName(tTrav) && (!ExistingParmName(tTrav, &API.Parms))) {

                    if (i) {
                        for(j = 0; j < i; j++) {
                            strcpy(&buff[2 * j], "P_");
                        }

                        strcpy(&buff[2 * j], PTrav->Type->Name);

                        PTrav->Type = FindString(buff, DataTypes, DATA_HASH_SIZE);

                        if (PTrav->Type == NULL) {
                            DbgPrint(
                                "BADMAN: Script Error[%d]: Type '%s' not implemented\n",
                                LineCount,
                                buff
                                );
                            xLog(
                                hLog,
                                XLL_WARN,
                                "Script error[%d]: type '%s' not implemented\n",
                                LineCount,
                                buff
                                );
                            break;
                        }
                    }

                    PTrav->Name = NewCharString(tTrav);

                    if (term == ')') {
                        state = STATE_API_PARAM_DONE;
                    } else if (space) {
                        state = STATE_API_PARAM_NAME;
                    } else if (term == ',') {
                        state = STATE_API_NEXT_PARAM_WITH_COMMA;
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Unexpected delimeter after parm name, '%c'\n",
                            LineCount,
                            term
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: unexpected delimeter after parm name, '%c'\n",
                            LineCount,
                            term
                            );
                    }
                } else {
                    DbgPrint(
                        "BADMAN: Script Error[%d]: Invalid or repeated parm name, '%s'\n",
                        LineCount,
                        token
                        );
                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: invalid or repeated parm name, '%s'\n",
                        LineCount,
                        token
                        );
                }

                break;

            case STATE_API_PARAM_NAME:

                //
                // The name for a parameter was read in, but it was followed
                // by white space, so we don't know whether to read in more
                // params or terminate.
                //

                state = STATE_ERROR;

                if (token[0] == '\0') {
                    if (term == ')') {
                        state = STATE_API_PARAM_DONE;
                    } else if (term == ',') {
                        state = STATE_API_NEXT_PARAM_WITH_COMMA;
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Expected ')' or ',' after Parm Name, not '%c'\n",
                            LineCount,
                            term
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script Error[%d]: expected ')' or ',' after parm name, not '%c'\n",
                            LineCount,
                            term
                            );
                    }
                } else {
                    DbgPrint(
                        "BADMAN: Script Error[%d]: Missing delimeter after Parm Name\n",
                        LineCount
                        );
                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: missing delimeter after parm name\n",
                        LineCount
                        );
                }

                break;

            case STATE_PARAM_LASTERROR:

                //
                // We've seen an '=' in a Bad list, so we know it's for a
                // GetLastError info statement.
                //

                state = STATE_PARAM_BAD_LIST;

                if (space || (term == ',')) {
                    if (isdigit(token[0])) {
                        sscanf(token, "%d", &BTrav->LastError);
                    } else {
                        BTrav->L = FindDefine(token, Hash);

                        if (BTrav->L == NULL) {
                            state = STATE_ERROR;

                            DbgPrint(
                                "BADMAN: Script Error[%d]: Unknown GetLastError define '%s'\n",
                                LineCount,
                                token
                                );
                            xLog(
                                hLog,
                                XLL_WARN,
                                "Script error[%d]: unknown GetLastError define '%s'\n",
                                LineCount,
                                token
                                );
                        } else {
                            BTrav->LastError = TranslateDefine(token, Hash);
                        }
                    }
                } else {
                    state = STATE_ERROR;

                    DbgPrint(
                        "BADMAN: Script Error[%d]: Unexpected delimiter '%c'\n",
                        LineCount,
                        term
                        );
                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: unexpected delimiter '%c'\n",
                        LineCount,
                        term
                        );
                }

                break;

            case STATE_PARAM_BAD_READ:

                //
                // After we've read a bad value, we could possibly see the
                // GetLastError coming next, signaled by an '='.
                //

                if ((term == '=') && (token[0] == '\0')) {
                    state = STATE_PARAM_LASTERROR;
                    break;
                }

                state = STATE_PARAM_BAD_LIST;

                /* FALL-THROUGH */

            case STATE_API_PARAM_DONE:

                //
                // the parameter list is finished. we either get the return
                // value of the API, or the first parm info.  We'll fall
                // through in that case to the general list processing.
                //
                // Assume no return value specified, change if it turns out
                // one is.
                //

                if (state == STATE_API_PARAM_DONE) {
                    API.RValue.Type = RET_NONE;

                    if ((state == STATE_API_PARAM_DONE) && (term == '=')) {
                        if (token[0] == '\0') {
                            API.RValue.Type = RET_ISEQUAL;
                            state = STATE_API_RETURN_ASSIGN;
                            break;
                        }

                        if ((token[0] == '!') && (token[1] == '\0')) {
                            API.RValue.Type = RET_NOTEQUAL;
                            state = STATE_API_RETURN_ASSIGN;
                            break;
                        }
                    }
                }

                /* FALL-THROUGH */

            case STATE_API_RETURN_DONE:
            case STATE_PARAM_BAD_LIST:
            case STATE_PARAM_OMIT_LIST:

                //
                // The API return value was set, and now we need to start
                // reading in the good and bad parameters
                //

                if (token[0] != '\0') {
                    if (term == ':') {
                        //
                        // if a keyword has been seen
                        //

                        if (!_stricmp("DEPEND", token)) {

                            //
                            // when we go into a dependency list, all params
                            // until we see an enddepend should go under PTrav
                            // ->Depend...  Since the next thing we SHOULD
                            // read after this is a parm name, PTrav will be
                            // set to PTrav->Depend, and PTrav->next, the
                            // new parm, will be in the appropriate place.
                            //

                            state = STATE_API_RETURN_DONE;

                            PD.next = NewParmDep(PTrav->Depend, PD.next);
                        }

                        //
                        // if token's not DEPEND, it's ENDDEPEND or a parm.
                        //

                        if ( !_stricmp("enddepend", token)) {

                            if (PD.next->next != NULL) {
                                //
                                // Mark it as an end of case
                                //

                                PTrav = PD.next->P;

                                while (PTrav->next != NULL) {
                                    PTrav = PTrav->next;
                                }

                                PTrav->bIsEndCase = TRUE;

                                //
                                // delete the obsolete PD entry.
                                //

                                PDTrav = PD.next;
                                PD.next = PD.next->next;

                                free(PDTrav);

                                state = STATE_API_RETURN_DONE;
                                break;
                            } else {
                                state = STATE_ERROR;
                                DbgPrint(
                                    "BADMAN: Script Error[%d]: ENDDEPEND without matching DEPEND\n",
                                    LineCount
                                    );
                                xLog(
                                    hLog,
                                    XLL_WARN,
                                    "Script error[%d]: ENDDEPEND without matching DEPEND\n",
                                    LineCount
                                    );
                            }
                        }

                        //
                        // If an ENDCASE keyword is found, we need to note it.
                        //

                        if (!_stricmp("endcase", token)) {

                            PTrav = PD.next->P;

                            while (PTrav->next != NULL) {
                                PTrav = PTrav->next;
                            }

                            PTrav->bIsEndCase = TRUE;
                        }

                        //
                        // The token must be a param if not the previous two
                        // Make sure it's a valid parameter name
                        //

                        if (ExistingParmName(token, &API.Parms)) {

                            //
                            // PTrav will either find that parameter with an
                            // empty slot at this level of Dependency, or the
                            // end of the list.
                            //

                            PTrav = PD.next->P;

                            while (PTrav->next != NULL) {
                                PTrav = PTrav->next;

                                if (!strcmp(PTrav->Name, token)) {
                                    break;
                                }
                            }

                            //
                            // If the parm was found but already filled, go to
                            // end of the parm list for this dependency level.
                            //

                            if (PTrav != PD.next->P) {
                                if ( !strcmp(PTrav->Name, token) &&
                                     PTrav->Good.next != NULL &&
                                     PTrav->Bad.next != NULL ) {

                                    while (PTrav->next != NULL) {
                                        PTrav = PTrav->next;
                                    }
                                }
                            }

                            //
                            // If the existing entry for the parm is already
                            // full, or there wasn't one, we're at the end of
                            // the parm list and need to append a new entry
                            // with this name and type.
                            //

                            if ( PTrav == PD.next->P ||
                                 strcmp(PTrav->Name, token) ||
                                 PTrav->Good.next != NULL ||
                                 PTrav->Bad.next != NULL ) {

                                //
                                // The original parm list has all the types,
                                // so we'll steal the parm type from there.
                                //

                                PTrav2 = &API.Parms;

                                while (PTrav2->next != NULL) {
                                    PTrav2 = PTrav2->next;

                                    if (!strcmp(PTrav2->Name, token)) {
                                        break;
                                    }
                                }

                                PTrav->next = NewParm(
                                                PTrav2->Type,
                                                token,
                                                FALSE,
                                                FALSE,
                                                NULL,
                                                NULL,
                                                NULL,
                                                PTrav->next
                                                );

                                PTrav = PTrav->next;
                            }

                            state = STATE_PARAM_IDENTIFIED;
                        } else {
                            //
                            // If none of these cases was satisfied, we have an error
                            //

                            if ( _stricmp("DEPEND", token) &&
                                 _stricmp("ENDDEPEND", token) &&
                                 _stricmp("ENDCASE", token) ) {

                                state = STATE_ERROR;

                                DbgPrint(
                                    "BADMAN: Script Error[%d]: Keyword '%s' out of context.\n",
                                    LineCount,
                                    token
                                    );
                                xLog(
                                    hLog,
                                    XLL_WARN,
                                    "Script error[%d]: keyword '%s' out of context.\n",
                                    LineCount,
                                    token
                                    );
                            }
                        }
                    }

                    //
                    // It is valid for us to be out of parms and ready to
                    // start on the new API.
                    //

                    if (space || (term == ',') || (term == '=')) {
                        if ((FindString(token, DataTypes, DATA_HASH_SIZE) != NULL) && space) {
                            NextAPIType = NewCharString(token);
                            state = STATE_LOOP_THROUGH_API;
                        } else {
                            //
                            // The other possibility is that we're processing
                            // bad & omit lists.
                            //

                            switch (state) {

                            case STATE_PARAM_BAD_LIST:

                                //
                                // Processing a 'bad case' list
                                //
                                // Expand the current token if it's a macro
                                //

                                if ((MTrav = FindMacro(token, MacroHash)) != NULL) {

                                    MVTrav = &MTrav->M;

                                    while (MVTrav->next != NULL) {
                                        MVTrav = MVTrav->next;

                                        BTrav->next = NewBadVal(
                                                        MVTrav->V,
                                                        MVTrav->L,
                                                        MVTrav->LastError,
                                                        NULL
                                                        );

                                        BTrav = BTrav->next;
                                    }

                                    state = STATE_PARAM_BAD_LIST;
                                } else {
                                    //
                                    // Or check to see if it's a case.  if so,
                                    // add it to the list
                                    //

                                    if ((DTrav = FindDefine(token, Hash)) != NULL) {

                                        BTrav->next = NewBadVal(DTrav, NULL, BAD_DEFINE, NULL);
                                        BTrav = BTrav->next;

                                        if (term == '=') {
                                            state = STATE_PARAM_LASTERROR;
                                        } else {
                                            state = STATE_PARAM_BAD_READ;
                                        }
                                    } else {
                                        //
                                        // if not a macro & not a case, error
                                        //

                                        DbgPrint(
                                            "BADMAN: Script Error[%d]: Unable to translate define '%s'\n",
                                            LineCount,
                                            token
                                            );
                                        xLog(
                                            hLog,
                                            XLL_WARN,
                                            "Script error[%d]: unable to translate define '%s'\n",
                                            LineCount,
                                            token
                                            );

                                        state = STATE_ERROR;
                                    }
                                }

                                break;

                            case STATE_PARAM_OMIT_LIST:

                                //
                                // Processing an 'omit list' to remove non-bad cases.
                                //

                                if (term == '=') {
                                    state = STATE_ERROR;

                                    DbgPrint(
                                        "BADMAN: Script Error[%d]: Unexpected delimiter '%c'\n",
                                        LineCount,
                                        term
                                        );
                                    xLog(
                                        hLog,
                                        XLL_WARN,
                                        "Script error[%d]: unexpected delimiter '%c'\n",
                                        LineCount,
                                        term
                                        );
                                }

                                BTrav = &PTrav->Bad;

                                //
                                // We need to delete this token from the list
                                // of 'bad cases', since this is what Omit is
                                // for.
                                //
                                // Since there COULD be multiple cases with
                                // the same name, we'll zeke all of them.
                                //

                                Found = FALSE;

                                //
                                // Expand a macro, if found.
                                //

                                if ((MTrav = FindMacro(token, MacroHash)) != NULL) {

                                    MVTrav = &MTrav->M;

                                    while (MVTrav->next != NULL) {

                                        MVTrav = MVTrav->next;

                                        while (BTrav->next != NULL) {
                                            if (!strcmp(BTrav->next->V->Symbol, MVTrav->V->Symbol)) {
                                                BHold = BTrav->next;
                                                BTrav->next = BTrav->next->next;
                                                free(BHold);
                                                Found = TRUE;
                                            } else {
                                                BTrav = BTrav->next;
                                            }
                                        }
                                    }
                                } else {
                                    //
                                    // Otherwise see if it's a valid case
                                    //

                                    while (BTrav->next != NULL) {
                                        if (!strcmp(BTrav->next->V->Symbol, token)) {
                                            BHold = BTrav->next;
                                            BTrav->next = BTrav->next->next;
                                            free(BHold);
                                            Found = TRUE;
                                        } else {
                                            BTrav = BTrav->next;
                                        }
                                    }
                                }

                                //
                                // if we didn't find any cases for the tokens,
                                // fall through to an error.
                                //

                                if (Found) {
                                    break;
                                }

                                /* FALL-THROUGH */

                            default:

                                if (state == STATE_PARAM_OMIT_LIST) {
                                    DbgPrint(
                                        "BADMAN: Script Error[%d]: Unable to translate define '%s'\n",
                                        LineCount,
                                        token
                                        );
                                    xLog(
                                        hLog,
                                        XLL_WARN,
                                        "Script error[%d]: unable to translate define '%s'\n",
                                        LineCount,
                                        token
                                        );
                                } else {
                                    DbgPrint(
                                        "BADMAN: Script Error[%d]: Token out of context, '%s'\n",
                                        LineCount,
                                        token
                                        );
                                    xLog(
                                        hLog,
                                        XLL_WARN,
                                        "Script error[%d]: token out of context, '%s'\n",
                                        LineCount,
                                        token
                                        );
                                }

                                state = STATE_ERROR;
                                break;
                            }
                        }
                    } else if (term != ':') {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Unexpected delimeter '%c'\n",
                            LineCount,
                            term
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: unexpected delimeter '%c'\n",
                            LineCount,
                            term
                            );

                        state = STATE_ERROR;
                    }
                }

                break;

            case STATE_API_RETURN_ASSIGN:

                //
                // In this case we've read the '=' sign for the API return
                // value assignment, and need either a value or a valid
                // parameter name for the return value.
                //

                if (space) {
                    if (ExistingParmName(token, &API.Parms)) {

                        API.RValue.Type |= RET_PARM;
                        API.RValue.Value = 0;
                        PTrav = API.Parms.next;

                        while (strcmp(PTrav->Name, token)) {
                            PTrav = PTrav->next;
                            API.RValue.Value++;
                        }

                        state = STATE_API_RETURN_DONE;
                    } else {
                        API.RValue.Type |= RET_VALUE;
                        API.RValue.Value = atoi(token);
                        state = STATE_API_RETURN_DONE;
                    }
                } else {
                    DbgPrint(
                        "BADMAN: Script Error[%d]: Unexpected delimeter '%c'\n",
                        LineCount,
                        term
                        );
                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: unexpected delimeter '%c'\n",
                        LineCount,
                        term
                        );

                    state = STATE_ERROR;
                }

                break;

            case STATE_PARAM_IDENTIFIED:

                //
                // A parameter has been identified for us to get good, bad,
                // unknown, or dependant information on.
                //

                state = STATE_ERROR;

                if (term == ':') {
                    if (!_stricmp("GOOD", token)) {
                        state = STATE_PARAM_GOOD_LIST;
                        GTrav = &PTrav->Good;
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Expected keyword 'Good', not token '%s'\n",
                            LineCount,
                            token
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: expected keyword 'Good', not token '%s'\n",
                            LineCount,
                            token
                            );
                    }
                } else {
                    DbgPrint(
                        "BADMAN: Script Error[%d]: Expected keyword w/ delim ':', actual delim '%c'\n",
                        LineCount,
                        term
                        );
                    xLog(
                        hLog,
                        XLL_WARN,
                        "Script error[%d]: expected keyword w/ delim ':', actual delim '%c'\n",
                        LineCount,
                        term
                        );
                }

                break;

            case STATE_PARAM_GOOD_LIST:

                //
                // We are on the good list for a given parameter, start out by
                // assuming the worst.
                //

                state = STATE_ERROR;

                //
                // On a colon, we need either "Bad" or "omit"
                //

                if (term == ':') {
                    //
                    // if the token is bad, make bad list one by one
                    //

                    if (!_stricmp("BAD", token)) {
                        state = STATE_PARAM_BAD_LIST;
                        BTrav = &PTrav->Bad;
                    } else if (!_stricmp("OMIT", token)) {
                        //
                        // if Omit, all except under Omit which are of the
                        // same type as the parm are bad.
                        //

                        state = STATE_PARAM_OMIT_LIST;
                        BTrav = &PTrav->Bad;

                        //
                        // Insert all relevent defines, then we'll yank out
                        // the ones we don't need.  First, we need the TYPE of
                        // the current parm.
                        //

                        TypeName = PTrav->Type->Name;
                        j = strlen(TypeName);

                        //
                        // Now, we loop through the hash table of defines
                        //

                        for (i = 0; i < HASH_SIZE; i++) {
                            DTrav = &Hash[i];

                            //
                            // And check the list for each bucket
                            //

                            while (DTrav->next != NULL) {
                                DTrav = DTrav->next;

                                if (!strncmp(TypeName, DTrav->Symbol, j)) {

                                    //
                                    // take out all the good ones!
                                    //

                                    GTrav = &PTrav->Good;
                                    Found = FALSE;

                                    while (GTrav->next != NULL) {
                                        GTrav = GTrav->next;

                                        if (!strcmp(GTrav->V->Symbol, DTrav->Symbol)) {
                                            Found = TRUE;
                                            break;            // while
                                        }
                                    }

                                    if (!Found) {
                                        BTrav->next = NewBadVal(
                                                        DTrav,
                                                        NULL,
                                                        BAD_DEFINE,
                                                        BTrav->next
                                                        );

                                        BTrav = BTrav->next;
                                    }
                                }
                            }
                        }
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Expected keyword 'Bad:' or 'Omit:', not '%s:'\n",
                            LineCount,
                            token
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: expected keyword 'Bad:' or 'Omit:', not '%s:'\n",
                            LineCount,
                            token
                            );
                    }
                } else {
                    //
                    // If terminatior is NOT ':'
                    //

                    if (space || (term == ',')) {
                        if ((MTrav = FindMacro(token, MacroHash)) != NULL) {

                            MVTrav = &MTrav->M;

                            while (MVTrav->next != NULL) {
                                MVTrav = MVTrav->next;
                                GTrav->next = NewGoodVal(MVTrav->V, NULL);
                                GTrav = GTrav->next;
                            }

                            state = STATE_PARAM_GOOD_LIST;
                        } else {
                            GTrav->next = NewGoodVal(FindDefine(token, Hash), NULL);
                            GTrav = GTrav->next;

                            if (GTrav->V != NULL) {
                                state = STATE_PARAM_GOOD_LIST;
                            } else {
                                DbgPrint(
                                    "BADMAN: Script Error[%d]: Unknown symbol '%s'\n",
                                    LineCount,
                                    token
                                    );
                                xLog(
                                    hLog,
                                    XLL_WARN,
                                    "Script error[%d]: unknown symbol '%s'\n",
                                    LineCount,
                                    token
                                    );
                            }
                        }
                    } else {
                        DbgPrint(
                            "BADMAN: Script Error[%d]: Unexpected delimeter '%c'\n",
                            LineCount,
                            term
                            );
                        xLog(
                            hLog,
                            XLL_WARN,
                            "Script error[%d]: unexpected delimeter '%c'\n",
                            LineCount,
                            term
                            );
                    }
                }

                break;
            }

            if (state == STATE_LOOP_THROUGH_API) {

                if (!(CLOptions->Flags & CLOPT_PARSEONLY)) {

                    if ( (!(CLOptions->Flags & CLOPT_API)) ||
                         (!strcmp(CLOptions->APIName, API.Name)) ) {

                        if (CLOptions->Flags & CLOPT_DOCUMENT) {
                            PrintAPICall(
                                hLog, &API,
                                NULL, NULL, NULL,
                                0, i, 0, NULL,
                                TRUE, TRUE, FALSE,
                                0
                                );
                        }

                        NumVar += ProcessAPI(
                                    &API,
                                    Hash,
                                    DataTypes,
                                    CLOptions,
                                    BadmanModules,
                                    OtherModules,
                                    hLog,
                                    hConOut
                                    );
                    }
                }

                //
                // We SHOULD have the next API type in NextAPIType.
                //

                state = STATE_BEGIN;

                API.Type = FindString(
                                NextAPIType,
                                DataTypes,
                                DATA_HASH_SIZE
                                );

                if (API.Type != NULL) {
                    state = STATE_API_TYPE;
                }
            }

        }

        token[0] = '\0';
    }

    fclose( inFPtr );

    //
    // After parsing the file, we have to do the last API.
    //

    if ( (!(CLOptions->Flags & CLOPT_PARSEONLY)) && (API.Type != NULL) ) {

        if ( (!(CLOptions->Flags & CLOPT_API)) ||
             (!strcmp(CLOptions->APIName, API.Name)) ) {

            if (CLOptions->Flags & CLOPT_DOCUMENT) {
                PrintAPICall(
                    hLog, &API,
                    NULL, NULL, NULL,
                    0, i, 0,
                    NULL,
                    TRUE, TRUE, FALSE,
                    0
                    );
            }

            NumVar += ProcessAPI(
                        &API,
                        Hash,
                        DataTypes,
                        CLOptions,
                        BadmanModules,
                        OtherModules,
                        hLog,
                        hConOut
                        );
        }
    }

    return NumVar;
}
