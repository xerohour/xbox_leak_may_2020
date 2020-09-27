/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    procapi.c

Abstract:

    This module contains information related to processing an API, including
    locating calls to parameter setup and cleanup, and locating the API to be
    called.

Author:

    John Miller (johnmil) 16-Jan-1992

Environment:

    XBox

Revision History:

    10-Apr-1992     JohnMil

        Cganhed comment to indented comments

    31-Mar-1995     TimF

        Made it readable

    01-Apr-2000     schanbai

        Ported to XBox and made the code more readable

--*/


#include "precomp.h"
#include "parsedef.h"
#include "Timing.h"
#include "teststat.h"


DWORD
CallAPI(
    int             iNumParms,
    FARDPROC        pfnAPI,
    DWORD           *dwParms
);


int
ProcessAPI(
    APIType         *API,
    DefineType      Hash[HASH_SIZE],
    StringType      DataTypes[DATA_HASH_SIZE],
    CLType          *CLOptions,
    ModType         BadmanModules[BADMAN_MODULE_COUNT],
    ModType         OtherModules[MAX_OTHER_MODULES],
    HANDLE          hLog,
    HANDLE          hConOut
)
{
    // ParmList the list of parameters to pass on.
    S_PARMS         ParmList[MAXNOPARMS] = {0},
                    *PTrav,
                    *PTrav2;
    ParmDepType     *PDHold,
                    ParmDep;
    FARFNSETUP      pfnSetup[MAXNOPARMS];
    FARFNCLEANUP    pfnCleanup[MAXNOPARMS];
    FARFNVALIDATE   pfnValidate = NULL;
    FARFNAPICLN     pfnAPICln;
    FARDPROC        pfnAPI = NULL;
    int             i,
                    ParmGoodBad[MAXNOPARMS],
                    NumParms = 0,
                    NumVar = 0,
                    Variation = 0;
    char            FunName[132];
    BOOL            DoPerm,
                    Found = FALSE;
    DefineType      VarInfo[MAXNOPARMS];
    SeenType        Seen,
                    *SHold;

    DoPerm = CLOptions->Flags & CLOPT_PERMUTE;

    //
    // First, we need to determine the base set of parms. This is all the
    // parms, in order, until one in the list is repeated.  Note that these
    // are the defaults for each param, and when dependencies and whatnot are
    // inserted, they'll be put before the default in the bucket for that parm#
    //

    PTrav = &API->Parms;

    while ((PTrav->next != NULL) && !Found) {
        PTrav = PTrav->next;
        PTrav2 = &API->Parms;

        while (PTrav2->next != PTrav) {
            PTrav2 = PTrav2->next;

            if (!strcmp(PTrav->Name, PTrav2->Name)) {
                Found = TRUE;
            }
        }

        if (!Found) {
            //
            // The default will always be the last entry in the list for each
            // parameter #.
            //

            ParmList[NumParms++].next = NewParm(
                                            PTrav->Type,
                                            PTrav->Name,
                                            PTrav->bIsEndCase,
                                            PTrav->NoPush,
                                            PTrav->Good.next,
                                            PTrav->Bad.next,
                                            PTrav->Depend,
                                            NULL
                                            );
        }
    }

    //
    // PD will be used to keep us in line insofar as dependency lists goes: as
    // each dependency list is entered, PD.next will point to that list. As it
    // is exited, PDep.next will be destroyed, and the previous PD.next will
    // move into position.
    //
    // The defaults have been entered. Now we need to find the address of the
    // API, and of each parameters setup and cleanup functions.
    //
    // First, search through the modules for The API's address
    //

    for (i = 0; i < MAX_OTHER_MODULES; i++) {
        if (OtherModules[i].h) {
            if ((pfnAPI = (FARDPROC)GetProcAddress(OtherModules[i].h, API->Name)) != NULL) {
                break;
            }
        } else {
            break;
        }
    }

    if (pfnAPI ==  NULL) {
        xSetFunctionName( hLog, API->Name ); 
        xStartVariation( hLog, API->Name );
        xLog( hLog, XLL_BLOCK, "%s was not found as an API", API->Name );
        xEndVariation( hLog );
        return 0;
    }

    if (CLOptions->Flags & CLOPT_VALIDATE) {
        sprintf(FunName, "%sValidate", API->Name);
        pfnValidate = (FARFNVALIDATE) \
            GetProcAddress(
                BadmanModules[BADMAN_MODULE_NUM_VALIDATE].h,
                FunName
                );
    }

    if (CLOptions->Flags & CLOPT_VALIDATE) {
        if (!pfnValidate) {
            KdPrint(("BADMAN: Validation function not found, %s\n", FunName));
        }
    }

    //
    // Get API Cleanup routine address
    //

    sprintf(FunName, "%sAPICleanup", API->Name);

    pfnAPICln = (FARFNAPICLN) \
        GetProcAddress(BadmanModules[BADMAN_MODULE_NUM_APICLN].h, FunName);

    //
    // Next, find each of the setup and cleanup functions for the Parameters.
    //

    for (i=0; i < NumParms; i++) {
        sprintf(FunName, "%sSetup", ParmList[i].next->Type->Name);

        pfnSetup[i] = (FARFNSETUP) \
            GetProcAddress(BadmanModules[BADMAN_MODULE_NUM_SETCLN].h, FunName);

        if ( !pfnSetup[i] ) {

            KdPrint((
                "BADMAN: setup function was missing for %s (%s)\n",
                ParmList[i].next->Type->Name, 
                FunName
                ));

            xSetFunctionName( hLog, API->Name ); 
            xStartVariation( hLog, API->Name );
            xLog(
                hLog,
                XLL_BLOCK,
                "Setup function was missing for %s",
                ParmList[i].next->Type->Name
                );
            xEndVariation( hLog );

            ExitThread( EXIT_FAILURE );
        }

        sprintf(FunName, "%sCleanup", ParmList[i].next->Type->Name);

        pfnCleanup[i] = (FARFNCLEANUP) \
            GetProcAddress(BadmanModules[BADMAN_MODULE_NUM_SETCLN].h, FunName);

        if ( !pfnCleanup[i] ) {

            KdPrint((
                "BADMAN: cleanup function was missing for %s (%s)\n",
                ParmList[i].next->Type->Name, 
                FunName
                ));

            xSetFunctionName( hLog, API->Name ); 
            xStartVariation( hLog, API->Name );
            xLog(
                hLog,
                XLL_BLOCK,
                "Cleanup function was missing for %s",
                ParmList[i].next->Type->Name
                );
            xEndVariation( hLog );

            ExitThread( EXIT_FAILURE );
        }
    }

    //
    // Now that we have all the setup and cleanup functions, we can begin
    // processing.
    //

    i = CLOPT_GOOD;

    do {
        NumVar += GenParm(
                    ParmList, 0,
                    NumParms,
                    pfnSetup,
                    pfnCleanup,
                    pfnAPI,
                    pfnValidate,
                    pfnAPICln,
                    DoPerm ? (CLOptions->Flags & (CLOPT_GOOD | CLOPT_BAD)) : i,
                    &Variation,
                    CLOptions,
                    VarInfo,
                    API,
                    0,
                    ParmGoodBad,
                    hLog,
                    hConOut
                    );

        i += (CLOPT_BAD - CLOPT_GOOD);

    } while ((i == CLOPT_BAD) && !DoPerm);

    //
    // We want to loop through all the different dependencies and variations.
    // For an explanation, consult the dox. This is COMPLICATED.
    //

    Seen.next = NewSeen(NULL);
    ParmDep.next = NewParmDep(PTrav, NULL);

    if (Found) {
        do {
            PTrav = ParmDep.next->P;

            //
            // If we're at the end of a dependency level
            //

            if (PTrav == NULL) {
                for (i = 0; i < MAXNOPARMS; i++) {
                    if (Seen.next->S[i] == TRUE) {
                        ParmList[i].next = DestroyParm(ParmList[i].next);
                    }
                }

                SHold = Seen.next;
                Seen.next = SHold->next;
                free(SHold);

                PDHold = ParmDep.next;
                ParmDep.next = PDHold->next;
                free(PDHold);

                //
                // However, since the Dependency indicated the end of a case,
                // we need to clear out the claptrap.
                //

                for (i = 0; (i < MAXNOPARMS) && (Seen.next != NULL); i++) {
                    if (Seen.next->S[i]) {
                        ParmList[i].next = DestroyParm(ParmList[i].next);
                        Seen.next->S[i] = FALSE;
                    }
                }
            } else {
                //
                // Else we're NOT at the end of a dependency level.
                //
                // Find the entry, or a previous parameter seen
                //

                for (i = 0; strcmp(ParmList[i].next->Name, PTrav->Name) && (i < MAXNOPARMS); i++) {
                    if (Seen.next->S[i]) {
                        break;
                    }
                }

                //
                // if it hasn't been seen, insert it.
                //

                if (!Seen.next->S[i]) {
                    ParmList[i].next = NewParm(
                                            PTrav->Type,
                                            PTrav->Name,
                                            PTrav->bIsEndCase,
                                            PTrav->NoPush,
                                            PTrav->Good.next,
                                            PTrav->Bad.next,
                                            NULL,
                                            ParmList[i].next
                                            );

                    Seen.next->S[i] = TRUE;

                    //
                    // If we're at the end of a dependency list, test API
                    //

                    if ((PTrav->next == NULL) && (PTrav->Depend->next == NULL)) {

                        i = CLOPT_GOOD;

                        do {
                            NumVar += GenParm(
                                        ParmList,
                                        0,
                                        NumParms,
                                        pfnSetup,
                                        pfnCleanup,
                                        pfnAPI,
                                        pfnValidate,
                                        pfnAPICln,
                                        DoPerm ? (CLOptions->Flags & (CLOPT_GOOD | CLOPT_BAD)) : i,
                                        &Variation,
                                        CLOptions,
                                        VarInfo,
                                        API,
                                        0,
                                        ParmGoodBad,
                                        hLog,
                                        hConOut
                                        );

                            i += (CLOPT_BAD - CLOPT_GOOD);

                        } while ((i == CLOPT_BAD) && !DoPerm);
                    }

                    ParmDep.next->P = ParmDep.next->P->next;

                    if (PTrav->Depend->next != NULL) {
                        Seen.next = NewSeen(Seen.next);
                        ParmDep.next = NewParmDep(PTrav->Depend->next, ParmDep.next);
                    }
                } else {
                    //
                    // Parameter has already been seen, so time to call test
                    //

                    i = CLOPT_GOOD;

                    do {
                        NumVar += GenParm(
                                    ParmList,
                                    0,
                                    NumParms,
                                    pfnSetup,
                                    pfnCleanup,
                                    pfnAPI,
                                    pfnValidate,
                                    pfnAPICln,
                                    DoPerm ? (CLOptions->Flags & (CLOPT_GOOD | CLOPT_BAD)) : i,
                                    &Variation,
                                    CLOptions,
                                    VarInfo,
                                    API,
                                    0,
                                    ParmGoodBad,
                                    hLog,
                                    hConOut
                                    );

                        i += (CLOPT_BAD - CLOPT_GOOD);

                    } while ((i == CLOPT_BAD) && !DoPerm);

                    for ( i=0; i<MAXNOPARMS; i++ ) {
                        if (Seen.next->S[i]) {
                            ParmList[i].next = DestroyParm(ParmList[i].next);
                            Seen.next->S[i] = FALSE;
                        }
                    }
                }
            }
        } while ( ParmDep.next != NULL );
    }

    DestroyParmDep( &ParmDep );

    while ( Seen.next ) {
        PVOID p = Seen.next;
        Seen.next = Seen.next->next;
        free( p );
    }

    for (i = 0; i < MAXNOPARMS; i++) {
        while (ParmList[i].next) {
            ParmList[i].next = DestroyParm(ParmList[i].next);
        }
    }

    DataTypes;    // -W3 warning elimination
    Hash;         // -W3 warning elimination

    return NumVar;
}


int
GenParm(
    S_PARMS        ParmList[MAXNOPARMS],
    int            ParmNo,
    int            NumParms,
    FARFNSETUP     pfnSetup[MAXNOPARMS],
    FARFNCLEANUP   pfnCleanup[MAXNOPARMS],
    FARDPROC       pfnAPI,
    FARFNVALIDATE  pfnValidate,
    FARFNAPICLN    pfnAPICln,
    int            GoodBad,
    int            *Variation,
    CLType         *CLOptions,
    DefineType     VarInfo[MAXNOPARMS],
    APIType        *API,
    int            MadeParm,
    int            ParmGoodBad[MAXNOPARMS],
    HANDLE         hLog,
    HANDLE         hConOut
    )

/*++

Routine Description:

    GenParm is a recursive routine, which will step through all combinations of
    the values for each parameter in ParmList. If GoodBad is set to CLOPT_GOOD,
    then only valid combinations are tested. If GoodBad is set to CLOPT_BAD,
    then only invalid combinations are tested. For testing, each Good or Bad
    value (depending upon which we're testing)is tried once for each
    parameter, with the rest of the parameters fixed to their first good value.
    So suppose we have 3 parameters, P1 P2 and P3, with Gx meaning a good value
    for that parm, and Bx meaning a bad value.

       P1 = G1 G2 B1 B2 B3
       P2 = G1 G2 G3 B1 B2 B3
       P3 = G1 G2 G3 B1 B2

    Then if we want to generate the Good combinations, our variations would be:

       P1    P2  P3
       --------------
       G1    G1  G1
       G1    G1  G2
       G1    G1  G3
       G1    G2  G1
       G1    G3  G1
       G2    G1  G1

    and if we wanted to generate the Bad combinations:

       P1    P2  P3
       --------------
       G1    G1  B1
       G1    G1  B2
       G1    B1  G1
       G1    B2  G1
       G1    B3  G1
       B1    G1  G1
       B2    G1  G1
       B3    G1  G1

    After the last parameter is determined for each variation, documentation
    and/or testing is called.

Arguments:

Return Value:

--*/

{
    BadValType          *BTrav;
    BOOL                GLPass,
                        VarPass,
                        Converge,
                        DoTime,
                        DoLog,
                        DoExcept,
                        CallExcepted,
                        IsGoodCall;
    char                TimeBuff[132];
    char                VariationBuffer[256];
    DWORD               dwRetVal=0,
                        dwExpRetVal,
                        dwParms[MAXNOPARMS];
    GoodValType         *GTrav;
    int                 CurPass=0,
                        i,
                        j,
                        GLVal=0,
                        NumPasses = 0,
                        NumVar = 0,
                        TimeSum = 0;
    LPVOID              lpvSCInfo[MAXNOPARMS];
    PULONG              pulData=NULL;     // Used for performance
    SHORT               hTimer=0;
    USHORT              usElementsInArray,
                        usDiscardedElements;

    //DbgPrint("\n--------\nParmNo %d\nNumParms %d\n----------", ParmNo, NumParms);

    //
    // If we've figured out the parm combination, we need to generate the
    // parms and test.
    //

    if (ParmNo == NumParms) {
        (*Variation)++;

        if ( (!(CLOptions->Flags & CLOPT_VARIATION)) || (CLOptions->VarNum == *Variation) ) {

            //
            // If we're supposed to document, print it out.
            //

            if ( (CLOptions->Flags & CLOPT_DOCUMENT) &&
                 (CLOptions->Flags & GoodBad) ) {

                KdPrint(("BADMAN: Variation %d:\n", *Variation));

                //
                // Print each of the parm define names
                //

                for ( i=0; i<NumParms; i++) {
                    //
                    // +'s around good parms, -'s around bad parms
                    //

                    if (ParmGoodBad[i] & CLOPT_BAD) {
                        KdPrint((
                            "BADMAN:  -%s-: %s - %s",
                            ParmList[i].next->Name,
                            VarInfo[i].next->Symbol,
                            VarInfo[i].next->Comment
                            ));
                    } else {
                        KdPrint((
                            "BADMAN:  +%s+: %s - %s",
                            ParmList[i].next->Name,
                            VarInfo[i].next->Symbol,
                            VarInfo[i].next->Comment
                            ));
                    }
                }
            }

            if (CLOptions->Flags & GoodBad) {
                NumVar++;
            }

            //
            // If we're supposed to test this api, then do so.  Use
            // try-excepts around all possible problem areas
            //

            if ((CLOptions->Flags & CLOPT_TEST) && (CLOptions->Flags & GoodBad)) {

                //
                // **** This is the API CALL Section ****
                //

                DoLog = CLOptions->Flags & CLOPT_NTLOG;
                DoTime = CLOptions->Flags & CLOPT_PERFORMANCE;
                DoExcept = CLOptions->Flags & CLOPT_NOEXCEPT;

                if (DoLog) {
                    xSetFunctionName( hLog, API->Name );

                    for ( i=0; i<NumParms; i++ ) {
                        if ( ParmGoodBad[i] & CLOPT_BAD ) {
                            sprintf( VariationBuffer, "p%d=%s", i, VarInfo[i].next->Symbol );
                        }
                    }

                    xStartVariation( hLog, VariationBuffer );
                }

                Converge = FALSE;
                CallExcepted = FALSE;

                if (DoTime) {
                    TimerOpen(&hTimer, MICROSECONDS);

                    TimeSum = NumPasses = 0;

                    TestStatInit();

                    pulData = calloc(CLOptions->MaxPerfIterations, sizeof (ULONG));

                    if (pulData == NULL) {
                        DbgPrint("BADMAN: Insufficient memory for performance. aborting.\n");
                        xLog( hLog, XLL_BLOCK, "Insufficient memory for performance. aborting." );
                        ExitThread( EXIT_FAILURE );
                    }
                }

                do {
                    //
                    // Set up the parms.  We'd really rather not do this here,
                    // but inside the "if (SiftAgain)" region, except that
                    // doing so makes parameter setup subject to sifting as
                    // well, which we don't want.
                    //

                    ULONG NoPushParms = 0;

                    for ( i=0; i<NumParms; i++) {

                        // zero-init SCInfo each time.
                        lpvSCInfo[i] = NULL;

                        try {
                            dwParms[i] = (DWORD)pfnSetup[i](
                                                    VarInfo[i].next->Case,
                                                    &lpvSCInfo[i],
                                                    hLog,
                                                    hConOut
                                                    );
                        } except( EXCEPTION_EXECUTE_HANDLER ) {
                            DbgPrint(
                                "BADMAN: Exception generated in setup for param %2d, %s, case name: %s\n",
                                i,
                                ParmList[i].next->Name,
                                VarInfo[i].next->Symbol
                                );
                        }

                        // BUGBUG: This is dangerous, because it requires that NOPUSH parameters 
                        //         come after pushable ones, and we don't enforce that.

                        NoPushParms += ParmList[i].next->NoPush ? 1 : 0;
                    }

                    //
                    // Call the actual test
                    //

                    if (CLOptions->Flags & CLOPT_DBGBREAK) {
                        DebugBreak();
                    }

                    try {
                        if (DoTime) {
                            TimerInit(hTimer);
                        }

                        //
                        // Technically we shouldn't call SetLastError inside
                        // the timed region, but it's the only way we can be
                        // certain that the timer-init code won't screw it...
                        //
                        // If your wondering how much it affects timing, on a
                        // xbox it's a few simple instruction:
                        //
                        // xapi!SetLastError:
                        // bfe6650c 55               push    ebp
                        // bfe6650d 8bec             mov     ebp,esp
                        // bfe6650f 64a124010000     mov     eax,fs:[00000124]
                        // bfe66515 8b80c0010000     mov     eax,[eax+0x1c0]
                        // bfe6651b 8b4d08           mov     ecx,[ebp+0x8]
                        // bfe6651e 8908             mov     [eax],ecx
                        // bfe66520 5d               pop     ebp
                        // bfe66521 c20400           ret     0x4
                        //

                        SetLastError(ERROR_SUCCESS);
                        //XapiGetCurrentThread()->LastError = ERROR_SUCCESS;

                        dwRetVal = CallAPI(NumParms - NoPushParms, pfnAPI, dwParms);

                        if (DoTime) {
                            CurPass = TimerRead(hTimer);
                        }

                        GLVal = GetLastError();
                        //GLVal = XapiGetCurrentThread()->LastError;

                    } except( EXCEPTION_EXECUTE_HANDLER ) {
                        if (DoTime) {
                            CurPass = pulData[NumPasses] = TimerRead(hTimer);
                        }

                        CallExcepted = TRUE;
                    }

                    try {
                        //
                        // Call to Validation routine.
                        //
                        if (pfnValidate != NULL) {
                            pfnValidate(VarInfo, dwParms, ParmGoodBad, dwRetVal, hLog, hConOut);
                        }
                    } except( EXCEPTION_EXECUTE_HANDLER ) {
                        DbgPrint(
                            "BADMAN: Exception generated in %sValidate, variation %d\n",
                            API->Name,
                            *Variation
                            );
                    }

                    try {
                        //
                        // Call to API Cleanup, if it exists.
                        //

                        if (pfnAPICln != NULL) {
                            pfnAPICln(dwRetVal, dwParms, hConOut, hLog);
                        }
                    } except( EXCEPTION_EXECUTE_HANDLER ) {
                        DbgPrint(
                            "BADMAN: Exception generated in %sAPICln, variation %d\n",
                            API->Name,
                            *Variation
                            );
                    }

                    //
                    // Destroy "clean up" the parms
                    //

                    for ( i=0; i<NumParms; i++ ) {
                        try {

                            pfnCleanup[i](VarInfo[i].next->Case, &lpvSCInfo[i], hLog, hConOut);

                        } except( EXCEPTION_EXECUTE_HANDLER ) {

                            DbgPrint(
                                "BADMAN: Exception generated in CleanUp for parm %d, %s, case name: %s\n",
                                i,
                                ParmList[i].next->Name,
                                VarInfo[i].next->Symbol
                                );
                        }
                    }

                    //
                    // Check for convergence for timing operations
                    //

                    if (DoTime) {
                        Converge = TestStatConverge(CurPass);

                        if (!(NumPasses % 7)) {
                            DbgPrint("\n");
                        }

                        NumPasses++;

                        DbgPrint(" %9d", CurPass);

                        TimeSum += CurPass;

                        if (NumPasses > CLOptions->MaxPerfIterations) {
                            DbgPrint("\n[Max Timing Iterations reached]");

                            if (DoLog) {
                                xLog(
                                    hLog,
                                    XLL_WARN,
                                    "%s Var %d -- MaxTiming Iterations reached",
                                    API->Name,
                                    *Variation
                                    );
                            }

                            break;
                        }
                    } else {
                        Converge = TRUE;
                    }
                } while (!Converge);

                if (DoTime) {
                    usElementsInArray = (USHORT)NumPasses;
                    usDiscardedElements = 0;

                    TestStatValues(
                        TimeBuff,
                        (USHORT)0,
                        &pulData,
                        &usElementsInArray,
                        &usDiscardedElements
                        );

                    DbgPrint("\nMode       Mean    Minimum    Maximum    #iter  pr.    st. dev. #out  oc");
                    DbgPrint("\n%s", TimeBuff);

                    if (DoLog) {
                        xLog(
                            hLog,
                            XLL_INFO,
                            "\nMode       Mean    Minimum    Maximum    #iter  pr.    st. dev. #out  oc"
                            );

                        xLog(hLog, XLL_INFO, "%s", TimeBuff);
                    }

                    TimerClose(hTimer);
                    free(pulData);
                }

                //
                // **** End of the API CALL Section ****
                //

                //
                // Evaluate GetLastError results
                //

                GLPass = FALSE;

                //
                // If any bad parms have return values specified, we will have
                // to investigate further. Otherwise GetLastError "passes".
                // We'll use j to detect if any parms are bad
                //

                for (i = 0, j = 1; i < NumParms; i++) {
                    if (ParmGoodBad[i] & CLOPT_BAD) {
                        //
                        // We need to find the current value in the bad list
                        //

                        j = 0;
                        BTrav = ParmList[i].next->Bad.next;

                        while(BTrav->next != NULL) {
                            BTrav = BTrav->next;

                            if (BTrav->V == VarInfo[i].next) {
                                break;
                            }
                        }

                        //
                        // If there's a GetLastError value, we need to check
                        // it.  Note that in the case of multiple bad
                        // parameters, ANY bad parameter missing a
                        // GetLastError will pass the variation!
                        //

                        if (BTrav->LastError == BAD_DEFINE) {
                            GLPass = TRUE;
                        } else if (GLVal == BTrav->LastError) {
                            GLPass = TRUE;
                        }
                    }
                }

                //
                // If there were only good parms, we don't worry about the
                // return value of GetLastError for now.
                //

                if (j) {
                    GLPass = TRUE;
                }

                //
                // Verify that the case passed.
                //

                dwExpRetVal = (API->RValue.Type & RET_PARM) ? dwParms[API->RValue.Value] : API->RValue.Value;

                if (API->RValue.Type & RET_NONE) {
                    VarPass = TRUE;
                } else {
                    VarPass = ((!j) == (dwRetVal == dwExpRetVal));

                    if (API->RValue.Type & RET_NOTEQUAL) {
                        VarPass = !VarPass;
                    }
                }

                VarPass = VarPass && GLPass;
                IsGoodCall = TRUE;

                for (i = 0; i < NumParms; i++) {
                    if (ParmGoodBad[i] & CLOPT_BAD) {
                        IsGoodCall = FALSE;
                        break;
                    }
                }

                if (CallExcepted) {
                    //
                    // if we're ignoring exceptions, count as a pass
                    //

                    if (DoExcept && !IsGoodCall) {
                        VarPass = TRUE;
                    } else {
                        //
                        // otherwise, an exception is a failure.
                        //

                        VarPass = FALSE;

                        //if (hLog != NULL) {
                            //xLog( hLog, XLL_FAIL, "The API call generated an exception." );
                        //}
                    }
                }

                if (VarPass) {
                    if (CLOptions->Flags & CLOPT_PRINT_ALL_CALLS) {
                        PrintAPICall(
                            hLog,
                            API, 
                            ParmList,
                            VarInfo,
                            dwParms,
                            dwRetVal,
                            *Variation,
                            NumParms,
                            ParmGoodBad,
                            GLPass,
                            VarPass,
                            CallExcepted,
                            GLVal
                            );
                    } else {
                        //DbgPrint(".");
                    }
                } else {
                    PrintAPICall(
                        hLog,
                        API,
                        ParmList,
                        VarInfo,
                        dwParms,
                        dwRetVal,
                        *Variation,
                        NumParms,
                        ParmGoodBad,
                        GLPass,
                        VarPass,
                        CallExcepted,
                        GLVal
                        );
                }

                if (DoLog) {

                    if (VarPass) {
                        xLog( hLog, XLL_PASS, "pass" );
                    }
                    //else {
                        //xLog(hLog, XLL_FAIL, "Variation Fails");
                    //}

                    //xLog( hLog, XLL_INFO, "%s Var %d", API->Name, *Variation );
                    xEndVariation( hLog );
                }
            }
        }
    } else {
        //
        // If we are NOT done generating all the parms, we need to.  Made parm
        // indicates whether or not we've reached the parm to loop through
        // (the rest fixed)
        //

        if (CLOptions->Flags & CLOPT_PERMUTE) {
            //
            // If we're going to do permutations, loop through permutation
            // values for this parameter, first through all good, then through
            // all bad (as applic.)
            //

            if (GoodBad & CLOPT_GOOD) {
                GTrav = &ParmList[ParmNo].next->Good;

                ParmGoodBad[ParmNo] = CLOPT_GOOD;

                while (GTrav->next != NULL) {
                    GTrav = GTrav->next;

                    VarInfo[ParmNo].next = GTrav->V;

                    NumVar += GenParm(
                                ParmList,
                                ParmNo + 1,
                                NumParms,
                                pfnSetup,
                                pfnCleanup,
                                pfnAPI,
                                pfnValidate,
                                pfnAPICln,
                                GoodBad,
                                Variation,
                                CLOptions,
                                VarInfo,
                                API,
                                MadeParm,
                                ParmGoodBad,
                                hLog,
                                hConOut
                                );
                }
            }

            if (GoodBad & CLOPT_BAD) {
                BTrav = &ParmList[ParmNo].next->Bad;
                ParmGoodBad[ParmNo] = CLOPT_BAD;

                while (BTrav->next != NULL) {
                    BTrav = BTrav->next;
                    VarInfo[ParmNo].next = BTrav->V;

                    NumVar += GenParm(
                                ParmList,
                                ParmNo + 1,
                                NumParms,
                                pfnSetup,
                                pfnCleanup,
                                pfnAPI,
                                pfnValidate,
                                pfnAPICln,
                                GoodBad,
                                Variation,
                                CLOptions,
                                VarInfo,
                                API,
                                MadeParm,
                                ParmGoodBad,
                                hLog,
                                hConOut
                                );
                }
            }
        } else {
            if (!MadeParm) {
                if (ParmNo != (NumParms - 1)) {
                    ParmGoodBad[ParmNo] = CLOPT_GOOD;

                    if (ParmList[ParmNo].next->Good.next != NULL) {

                        VarInfo[ParmNo].next = ParmList[ParmNo].next->Good.next->V;

                        NumVar += GenParm(
                                    ParmList,
                                    ParmNo + 1,
                                    NumParms,
                                    pfnSetup,
                                    pfnCleanup,
                                    pfnAPI,
                                    pfnValidate,
                                    pfnAPICln,
                                    GoodBad,
                                    Variation,
                                    CLOptions,
                                    VarInfo,
                                    API,
                                    MadeParm,
                                    ParmGoodBad,
                                    hLog,
                                    hConOut
                                    );
                    }
                }

                MadeParm = ParmNo + 1;

                if (GoodBad & CLOPT_GOOD) {
                    ParmGoodBad[ParmNo] = CLOPT_GOOD;
                    GTrav = &ParmList[ParmNo].next->Good;

                    if (ParmNo != (NumParms - 1)) {
                        GTrav = GTrav->next;
                    }

                    while ((GTrav != NULL) && (GTrav->next != NULL)) {
                        GTrav = GTrav->next;
                        VarInfo[ParmNo].next = GTrav->V;

                        NumVar += GenParm(
                                    ParmList,
                                    ParmNo + 1,
                                    NumParms,
                                    pfnSetup,
                                    pfnCleanup,
                                    pfnAPI,
                                    pfnValidate,
                                    pfnAPICln,
                                    GoodBad,
                                    Variation,
                                    CLOptions,
                                    VarInfo,
                                    API,
                                    MadeParm,
                                    ParmGoodBad,
                                    hLog,
                                    hConOut
                                    );
                    }
                }

                if (GoodBad & CLOPT_BAD ) {

                    BTrav = &ParmList[ParmNo].next->Bad;
                    ParmGoodBad[ParmNo] = CLOPT_BAD;

                    while ((BTrav != NULL) && (BTrav->next != NULL)) {

                        BTrav = BTrav->next;
                        VarInfo[ParmNo].next = BTrav->V;

                        NumVar += GenParm(
                                    ParmList,
                                    ParmNo + 1,
                                    NumParms,
                                    pfnSetup,
                                    pfnCleanup,
                                    pfnAPI,
                                    pfnValidate,
                                    pfnAPICln,
                                    GoodBad,
                                    Variation,
                                    CLOptions,
                                    VarInfo,
                                    API,
                                    MadeParm,
                                    ParmGoodBad,
                                    hLog,
                                    hConOut
                                    );
                    }
                }
            } else {
                ParmGoodBad[ParmNo] = CLOPT_GOOD;

                if (ParmList[ParmNo].next->Good.next != NULL) {

                    VarInfo[ParmNo].next = ParmList[ParmNo].next->Good.next->V;

                    NumVar += GenParm(
                                ParmList,
                                ParmNo + 1,
                                NumParms,
                                pfnSetup,
                                pfnCleanup,
                                pfnAPI,
                                pfnValidate,
                                pfnAPICln,
                                GoodBad,
                                Variation,
                                CLOptions,
                                VarInfo,
                                API,
                                MadeParm,
                                ParmGoodBad,
                                hLog,
                                hConOut
                                );
                }
            }
        }
    }

    return NumVar;
}


void
PrintAPICall(
    HANDLE       hLog,
    APIType      *API,
    S_PARMS      *ParmList,
    DefineType   *VarInfo,
    DWORD        *dwParms,
    DWORD        dwRetVal,
    int          Variation,
    int          NumParms,
    int          *ParmGoodBad,
    BOOL         GLPass,
    BOOL         VarPass,
    BOOL         VarExcept,
    DWORD        LastError
    )

/*++

Routine Description:

    Print a pretty version of the API call with its parameter names and
    expected return value. If ParmList is not null, then it will additionally
    print information pertaining to the value of each parameter, and the actual
    return value of the call.

Arguments:

Return Value:

--*/

{
    int i;
    char ApiParamsBuffer[1024];
    char TextBuffer[1024+256];

    UNREFERENCED_PARAMETER( dwParms );
    UNREFERENCED_PARAMETER( ParmList );
    UNREFERENCED_PARAMETER( Variation );

    ApiParamsBuffer[0] = '(';
    ApiParamsBuffer[1] = 0;

    for ( i=0; i<NumParms; i++) {

        if ( i != 0 ) {
            strcat( ApiParamsBuffer, ";" );
        }

        if ( ParmGoodBad[i] & CLOPT_BAD ) {
            sprintf( TextBuffer, "%s", VarInfo[i].next->Symbol );
        } else {
            sprintf( TextBuffer, "p%d", VarInfo[i].next->Case );
        }

        strcat( ApiParamsBuffer, TextBuffer );
    }

    strcat( ApiParamsBuffer, ")" );

    ASSERT( strlen( ApiParamsBuffer ) < sizeof( ApiParamsBuffer ) );

    if ( VarExcept ) {

        sprintf(
            TextBuffer,
            "Exception in %s%s",
            API->Name,
            ApiParamsBuffer
            );

        xLog( hLog, XLL_EXCEPTION, TextBuffer );

    } else {
        if ( !VarPass ) {

            sprintf(
                TextBuffer,
                "Bad return from %s%s,expected=0x%x,returned=0x%x",
                API->Name,
                ApiParamsBuffer,
                API->RValue.Value,
                dwRetVal
                );

            xLog( hLog, XLL_FAIL, TextBuffer );

        }
    }

    //KdPrint(( "BADMAN: %s\n", TextBuffer ));

    if ( !GLPass ) {
        sprintf(TextBuffer, "Incorrect last error 0x%x (%d)", LastError, LastError );
        //KdPrint(( "BADMAN: %s\n", TextBuffer ));
        xLog( hLog, XLL_FAIL, TextBuffer );
    }

    /* 
    
    **** Keep old code for a while, just for ref only ****
    
    sprintf(RetStr, ")");

    if (API->RValue.Type & RET_NOTEQUAL) {
        cBang = '!';
    } else {
        cBang = ' ';
    }

    if (API->RValue.Type & RET_VALUE) {
        sprintf(&RetStr[1], " %c= %d", cBang, API->RValue.Value);
    }

    if (API->RValue.Type & RET_PARM) {
        PTrav = API->Parms.next;

        for (i = API->RValue.Value; i > 0; i--) {
            PTrav = PTrav->next;
        }

        sprintf(&RetStr[1], " %c= %s", cBang, PTrav->Name);
    }

    RetLen = strlen(RetStr);

    sl[0] = el[0] = '\0';
    PTrav = &API->Parms;
    Found = FALSE;
    i = 0;

    ANLen = strlen(API->Name);

    if (ANLen > 19) {
        ANLen = 20;
    }

    NumCols = 79 - ANLen - RetLen;

    //
    // ParmList is NULL for calls for documentation which aren't the
    // result of an actual test call.
    //

    if (ParmList != NULL) {
        if (VarExcept) {
            DbgPrint("badman: +--------------EXCEPTION GENERATED IN API CALL---------------------------+\n");
        } else {
            if (!VarPass) {
                DbgPrint("badman: +-------------------Incorrect Return Value-------------------------------+\n");
                // why not log here ??
            } else {
                DbgPrint("badman: +---------------------------Success--------------------------------------+\n");
            }
        }

        strcpy(sl, "| ");
        strcpy(el, " |");
        NumCols -= 4;
    }

    DbgPrint("badman: %s%s(", sl, API->Name);
    sprintf(logbuff, "%s(", API->Name);
    lbPos = strlen(logbuff);

    if (ANLen == 20) {
        DbgPrint("\n                    ");
    }

    col = ANLen;

    while ((PTrav->next != NULL) && !Found) {
        PTrav = PTrav->next;
        PTrav2 = &API->Parms;
        while (PTrav2->next != PTrav) {
            PTrav2 = PTrav2->next;
            if (!strcmp(PTrav->Name, PTrav2->Name)) {
                Found = TRUE;
            }
        }

        if (!Found) {
            if (i) {
#define SEPARATOR   ", "
                DbgPrint(SEPARATOR);
                sprintf(&logbuff[lbPos], SEPARATOR);
                lbPos += sizeof SEPARATOR;
#undef  SEPARATOR
            }

            i++;
            col++;
            j = strlen(PTrav->Name);

            if ((col + j)>= NumCols) {
                if (ParmList != NULL) {
                    for (k = col + j; k < 76; k++) {
                        DbgPrint(" ");
                    }

                    DbgPrint("%s", el);
                }

                DbgPrint("\nbadman: %s", sl);
                col = ANLen + 1;

                for (k = 0; k < ANLen; k++) {
                    DbgPrint(" ");
                }
            }

            col += j;

            DbgPrint("%s", PTrav->Name);

            if (dwParms != NULL) {
                sprintf(
                    &logbuff[lbPos],
                    "%s 0x%lx",
                    PTrav->Type->Name,
                    dwParms[i - 1]
                    );

                lbPos = strlen(logbuff);
            }
        }
    }

    DbgPrint("%s", RetStr);

    sprintf(&logbuff[lbPos], ")= 0x%lx", dwRetVal);

    if (hLog != NULL) {
        xLog(hLog, XLL_WARN, "%s", logbuff);
        KdPrint(( "badman: %s\n", logbuff ));
    }

    col += RetLen;

    if (ParmList != NULL) {
        for (k = col; k < 70; k++) {
            DbgPrint(" ");
        }

        DbgPrint(" |");
        IsGoodCall = TRUE;

        for (i = 0; i < NumParms; i++) {
            if (ParmGoodBad[i] & CLOPT_BAD) {
                IsGoodCall = FALSE;
            }
        }

        DbgPrint(
            "\nbadman: |                  %4salid Parameters>                                  |",
            IsGoodCall ? "<V" : "<Inv"
            );

        DbgPrint(
            "\nbadman: |                 Variation #:  %-5d                                    |",
            Variation
            );

        if (!GLPass) {
            DbgPrint("\nbadman: |                 GetLastError()was incorrect                           |");

            if (hLog != NULL) {
                xLog(hLog, XLL_WARN, "GetLastError() was incorrect.");
                KdPrint(( "badman: GetLastError() was incorrect\n" ));
            }
        }

        if (IsGoodCall) {
            cBang = (char)((cBang == ' ') ? '!' : ' ');
        }

        DbgPrint(
            "\nbadman: |       Expected return value: %c0x%-10lX                             |",
            cBang,
            (API->RValue.Type & RET_VALUE) ?
            API->RValue.Value : dwParms[API->RValue.Value]
            );

        if (VarExcept) {
            DbgPrint("\nbadman: |         Actual return value:  <exception>                          |");
        } else {
            DbgPrint("\nbadman: |         Actual return value:  0x%-10lX                             |", dwRetVal);
        }

        //
        // ParamList is NULL if this is a documentation call... thus, don't
        // spew LastError.
        //

        if ((ParmList != NULL) && !VarPass) {
            DbgPrint("\nbadman: |\tLastError = %-10u                                           |", LastError);
        }

        if (hLog != NULL) {
            xLog(
                hLog,
                XLL_WARN,
                "Expected return value: %c0x%-10lx",
                cBang,
                API->RValue.Value
                );
            KdPrint(( "badman: Expected return value: %c0x%-10lx\n", cBang, API->RValue.Value ));

            if (!VarPass) {
                xLog(hLog, XLL_INFO, "LastError = %d", LastError);
                KdPrint(( "badman: LastError = %d\n", LastError ));
            }
        }

        DbgPrint("\nbadman: |         --------Parameters--------                                     |");

        for (i = 0; i < NumParms; i++) {
            if (ParmGoodBad[i] & CLOPT_GOOD) {
                sprintf(buff, "+%s+", ParmList[i].next->Name);
            } else {
                sprintf(buff, "-%s-", ParmList[i].next->Name);
            }

            sprintf(buff2, "(%s)", VarInfo[i].next->Symbol);

            DbgPrint(
                "\nbadman: |   %15s :  0x%-10lX %-36s |",
                buff,
                dwParms[i],
                buff2
                );
        }
    }*/
}


DWORD
CallAPI(
    int        iNumParms,
    FARDPROC   pfnAPI,
    DWORD      *dwParms
)

/*++

Routine Description:

    CallAPI is a rather disgusting way around quirky calling conventions where
    the caller cleans up the stack...

Arguments:

Return Value:

--*/

{
    DWORD  dwRet;

    switch (iNumParms) {

    case 0:
        dwRet = pfnAPI();
        break;
        
    case 1:
        dwRet = pfnAPI(dwParms[0]);
        break;
        
    case 2:    
        dwRet = pfnAPI(dwParms[0], dwParms[1]);
        break;
        
    case 3:
        dwRet = pfnAPI(dwParms[0], dwParms[1], dwParms[2]);
        break;
        
    case 4:
        dwRet = pfnAPI(dwParms[0], dwParms[1], dwParms[2], dwParms[3]);
        
        break;
        
    case 5:
        dwRet = pfnAPI(dwParms[0], dwParms[1], dwParms[2], dwParms[3], dwParms[4]);
        break;
        
    case 6:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5]
                    );
        break;
        
    case 7:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6]
                    );
        break;
        
    case 8:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7]
                    );
        break;
        
    case 9:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8]
                    );
        break;
        
    case 10:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9]
                    );
        break;
        
    case 11:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10]
                    );
        break;
        
    case 12:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11]
                    );
        break;
        
    case 13:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12]
                    );
        break;
        
    case 14:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13]
                    );
        break;
        
    case 15:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13],
                    dwParms[14]
                    );
        break;
        
    case 16:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13],
                    dwParms[14],
                    dwParms[15]
                    );
        break;
        
    case 17:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13],
                    dwParms[14],
                    dwParms[15],
                    dwParms[16]
                    );        
        break;
        
    case 18:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13],
                    dwParms[14],
                    dwParms[15],
                    dwParms[16],
                    dwParms[17]
                    );
        break;
        
    case 19:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13],
                    dwParms[14],
                    dwParms[15],
                    dwParms[16],
                    dwParms[17],
                    dwParms[18]
                    );
        break;
        
    case 20:
        dwRet = pfnAPI(
                    dwParms[0],
                    dwParms[1],
                    dwParms[2],
                    dwParms[3],
                    dwParms[4],
                    dwParms[5],
                    dwParms[6],
                    dwParms[7],
                    dwParms[8],
                    dwParms[9],
                    dwParms[10],
                    dwParms[11],
                    dwParms[12],
                    dwParms[13],
                    dwParms[14],
                    dwParms[15],
                    dwParms[16],
                    dwParms[17],
                    dwParms[18],
                    dwParms[19]
                    );
        break;

    default:
        dwRet = 0;
    }

    return dwRet;
}
