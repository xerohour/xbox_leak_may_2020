/*++

Copyright (c) 1992-2000  Microsoft Corporation

Module Name:

    parsefun.c

Abstract:

    This module contains helper functions for the Parser for BADMAN

Author:

    John Miller (johnmil) 14-Jan-1992

Environment:

    XBox

Revision History:

    31-Mar-1995     TimF

        Made it readable

    01-Apr-2000     schanbai

        Ported to XBox and made the code more readable

--*/


#include    "precomp.h"
#include    <parsedef.h>
#include    <ctype.h>


void
DestroyParmDep(
    ParmDepType *PD
    )
{
    ParmDepType *PDHold;

    if ( PD != NULL ) {
        PD = PD->next;
    }

    while ( PD != NULL ) {

        PDHold = PD;
        PD = PD->next;
        free( PDHold );
    }
}


int
HashVal(
    char *ToHash,
    int  HashSize
    )

/*++

Routine Description:

    HashVal will take a string as its only parameter, and return a number from
    0 to HASH_SIZE that reflects the value of the hashing function when used on
    the string.  This multiplies the ascii for the entire symbol, modulo HASH_SIZE.

Arguments:

Return Value:

--*/

{
    int hold;
    char *trav;

    hold = 1;
    for ( trav=ToHash; *trav != '\0'; trav++ ) {
        hold = (hold + *trav) % HashSize;
    }

    return hold;
}


DefineType *
NewDefine(
    char *Name,
    char *Comment,
    int  Val,
    DefineType *Next
    )

/*++

Routine Description:

    NewDefine creates a new DefineType variable

Arguments:

Return Value:

--*/

{
    DefineType *Temp;

    Temp = (DefineType *)malloc( sizeof(DefineType) );

    if ( Temp == NULL ) {
        DbgPrint("BADMAN: NewDefine: Insufficient Memory to create structure.\n");
        ExitThread( EXIT_FAILURE );
    }

    Temp->Symbol = NewCharString(Name);
    Temp->Comment = NewCharString(Comment);
    Temp->Case = Val;
    Temp->next = Next;

    return Temp;
}


void
ReadDefines(
    char       *FileName,
    DefineType Hash[HASH_SIZE],
    MacroType  MacroHash[MACRO_HASH_SIZE]
    )

/*++

Routine Description:

    Reads in all of the defines in a file.  Note that this set of defines
    should ONLY be integer defines, under the current constrains of the
    system.  Note that it assumes the hash table has been allocated and
    initialized to have all entries pointing to NULL for their next.  For now,
    it is only able to read integral defines, IE a single number.  Defines are
    inserted into the front of the proper hash bucket list without bothering to
    search to see if an identical name already exists.  Since any use of this
    list will involve a search through the entries in the bucket until the
    required define is found, and will quit after finding the first entry, this
    would be redundant.

Arguments:

Return Value:

--*/

{
    char buff[1024];
    char Comment[1024];
    char DefString[256];
    char junk[256];
    char MacroName[256];
    char name[256];
    DefineType *DTrav, *DTrav2, *hTrav;
    FILE *inFPtr;
    int  CPos, DefValue, HashEntry, LineCount;
    MacroType *MTrav, *MTrav2;
    MacroValType *MVTrav, *MVTrav2;
    unsigned int i;
    
    //
    // Open the requested data file.
    //
    inFPtr = fopen( FileName, "r" );
    
    if ( inFPtr == NULL ) {
        DbgPrint("BADMAN: ReadDefines: Unable to open %s\n", FileName);
        ExitThread( EXIT_FAILURE );
    }

    LineCount = 1;
    Comment[0] = '\0';
    CPos = 0;

    while ( fgets(buff, sizeof(buff), inFPtr) != NULL ) {
        
        //
        // Find all records with #define in them in the file
        //
        if ( !strncmp( buff, "#define", sizeof("#define")-1 ) ) {

            //
            // Read them, then insert them into the correct place.
            //
            Comment[CPos] = '\0';
            CPos = 0;
            
            sscanf( buff, "%s %s %s", junk, name, DefString);
            
            //
            // Decide whether define value is a # or a name.
            //
            
            if ( isdigit(DefString[0]) ) {
                
                //
                // If a number, read it in.
                //
                sscanf( DefString, "%d", &DefValue );

            } else {

                //
                // If a name, attempt to translate it
                //
                DefValue = TranslateDefine( DefString, Hash );
                
                if ( DefValue == BAD_DEFINE ) {
                    
                    DbgPrint(
                        "BADMAN: [%s:line %d] %s is not defined, Definition for %s ignored\n",
                        FileName,
                        LineCount,
                        DefString,
                        name
                        );
                }
            }

            if ( DefValue != BAD_DEFINE ) {
                HashEntry = HashVal( name, HASH_SIZE );
                hTrav = &Hash[HashEntry];

                //
                // Defines are inserted at the front of the bucket
                //
                hTrav->next = NewDefine( name, Comment, DefValue, hTrav->next );
            }
        } else {
            if ( !_strnicmp( buff, "MACRO:", strlen("MACRO:")) ) {

                sscanf( &buff[sizeof("MACRO:")-1], "%s", MacroName );
                HashEntry = HashVal( MacroName, MACRO_HASH_SIZE );

                MTrav = &MacroHash[HashEntry];
                MTrav->next = NewMacro( MacroName, NULL, MTrav->next );
                MTrav = MTrav->next;

                MVTrav = &MTrav->M;

                while ( fgets( buff, sizeof(buff), inFPtr) != NULL &&
                        _strnicmp( buff, "ENDMACRO:", sizeof("ENDMACRO:")-1 ) ) {

                    sscanf( buff, "%s", name );
                    MTrav2 = FindMacro( name, MacroHash );

                    //
                    // If the name is ANOTHER macro, expand it in
                    //
                    if ( MTrav2 != NULL ) {

                        MVTrav2 = &MTrav2->M;

                        while ( MVTrav2->next != NULL ) {

                            MVTrav2 = MVTrav2->next;
                            MVTrav -> next = NewMacroVal(
                                                MVTrav2->V, MVTrav2 -> L,
                                                MVTrav2->LastError,
                                                NULL
                                                );

                            MVTrav = MVTrav->next;
                        }
                    } else {
                        DTrav = FindDefine( name, Hash );

                        if ( DTrav == NULL ) {
                            DbgPrint(
                                "BADMAN: [%s:line %d] %s is not defined, %s excluded from macro %s\n",
                                FileName,
                                LineCount,
                                name,
                                name,
                                MTrav->Name
                                );
                        } else {
                            for ( i=0; i<strlen(buff); i++) {
                                if ( buff[i] == '=') {
                                    break;
                                }
                            }
                            
                            DTrav2 = NULL;
                            DefValue = BAD_DEFINE;

                            if ( buff[i] == '=' ) {
                                sscanf( &buff[i + 1], "%s", DefString );

                                if ( isdigit(DefString[0]) ) {
                                    
                                    //
                                    // If a number, read it in.
                                    //
                                    sscanf( DefString, "%d", &DefValue );
                                } else {
                                    
                                    //
                                    // If a name, attempt to translate it
                                    //
                                    DefValue = TranslateDefine( DefString, Hash );

                                    if ( DefValue == BAD_DEFINE ) {
                                        
                                        DbgPrint(
                                            "BADMAN: [%s:line %d] %s is not defined, %s excluded from macro %s\n",
                                            FileName,
                                            LineCount,
                                            DefString,
                                            name,
                                            MTrav->Name
                                            );
                                    } else {
                                        DTrav2 = FindDefine( DefString, Hash );
                                    }
                                }
                            }

                            MVTrav->next = NewMacroVal( DTrav, DTrav2, DefValue, MVTrav->next );
                            MVTrav = MVTrav->next;
                        }
                    }

                    LineCount++;
                }
            } else {
                i = 0;

                while ( buff[i] != '\0' ) {

                    if ( buff[i] == '/' && buff[i + 1] == '/' ) {
                        i += 2;
                        break;
                    }

                    i++;
                }
                
                if ( buff[i] == '\0' ) {
                    CPos = 0;
                }

                while ( buff[i] != '\0' ) {
                    Comment[CPos++] = buff[i++];
                }
            }
        }

        LineCount++;
    }

    fclose( inFPtr );
}


int
TranslateDefine(
    char *DefineName,
    DefineType Hash[HASH_SIZE]
    )

/*++

Routine Description:

    Translate the define named by DefineName into its corresponding integer
    ("case number").  If there are multiple Defines of the same number in the
    hash table, only the first will be returned.  If the define name is not in
    the hash table, then BAD_DEFINE is returned.

Arguments:

Return Value:

--*/

{
    DefineType *hTrav;
    int RetVal;

    RetVal = BAD_DEFINE;
    hTrav = &Hash[HashVal( DefineName, HASH_SIZE )];

    while ( hTrav->next != NULL ) {
        hTrav = hTrav->next;

        if ( !strcmp( DefineName, hTrav->Symbol ) ) {
            RetVal = hTrav->Case;
            break;
        }
    }

    return RetVal;
}


DefineType *
FindDefine(
    char *token,
    DefineType Hash[HASH_SIZE]
    )

/*++

Routine Description:

    Looks for a define with a given name, then returns a pointer to the data
    structure entry for it in the hash table.

Arguments:

Return Value:

--*/

{
    DefineType *hTrav;

    hTrav = &Hash[ HashVal(token, HASH_SIZE) ];
    
    while ( hTrav->next != NULL ) {
        hTrav = hTrav->next;
        if ( !strcmp(token, hTrav -> Symbol) ) {
            return hTrav;
        }
    }

    return (DefineType *)NULL;
}


MacroType *
FindMacro(
    char      *token,
    MacroType MacroHash[MACRO_HASH_SIZE]
    )

/*++

Routine Description:

    Looks for a macro with the name supplied by token, then returns a pointer
    to it, else NULL if it wasn't found.

Arguments:

Return Value:

--*/

{
    MacroType *MTrav;

    MTrav = &MacroHash[ HashVal(token, MACRO_HASH_SIZE) ];

    while ( MTrav->next != NULL ) {
        MTrav = MTrav->next;

        if ( !strcmp(token, MTrav->Name) ) {
            return MTrav;
        }
    }

    return (MacroType *)NULL;
}


StringType *
FindString(
    char *token,
    StringType *SList,
    int HashSize
    )

/*++

Routine Description:

    Try to find the token in its list of Strings, SList.  If the string is
    found, a pointer is returned to that entry.  Otherwise, NULL is returned.
    Note that SList is actually a hash table, so the hashing function must be
    called on the token.

Arguments:

Return Value:

--*/

{
    StringType *STrav;

    STrav = &SList[ HashVal(token, HashSize) ];

    while ( STrav -> next != NULL ) {
        STrav = STrav->next;

        if ( !strcmp( STrav->Name, token) ) {
            return STrav;
        }
    }

    return (StringType *)NULL;
}


char *
NewCharString(
    char *Text
    )

/*++

Routine Description:

    Take the given text argument, and copies into a freshly allocated block,
    whose pointer is returned.

Arguments:

Return Value:

--*/

{
    char *Hold;
    
    Hold = malloc( sizeof(char) * (strlen(Text) + 1) );
    
    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewCharString: Unable to allocate %d bytes.\n",
            sizeof(char) * (strlen(Text) + 1)
            );
    } else {
        strcpy( Hold, Text );
    }

    return Hold;
}


ParmDepType *
NewParmDep(
    S_PARMS     *P,
    ParmDepType *next
    )

/*++

Routine Description:

    Allocate a new Parameter Dependency entry, then returns a pointer to the
    initialized structure.

Arguments:

Return Value:

--*/

{
    ParmDepType *Hold;

    Hold = malloc( sizeof(ParmDepType) );
    
    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewParmDep: Unable to allocate %d bytes.\n",
            sizeof(ParmDepType)
            );
    } else {
        Hold->P = P;
        Hold->next = next;
    }

    return Hold;
}


S_PARMS *
NewParm(
    StringType  *Type,
    char        *Name,
    BOOL        bIsEndCase,
    BOOL        NoPush,
    GoodValType *G,
    BadValType  *B,
    S_PARMS     *Depend,
    S_PARMS     *next
    )

/*++

Routine Description:

    Allocate a new Parameter entry, then returns a pointer to the initialized
    structure.

Arguments:

Return Value:

--*/

{
    S_PARMS *Hold;

    UNREFERENCED_PARAMETER( bIsEndCase );

    Hold = malloc( sizeof(S_PARMS) );
    
    if (Hold == NULL) {
        DbgPrint( "BADMAN: NewParm: Unable to allocate %d bytes.\n", sizeof(S_PARMS) );
    } else {
        Hold->Type = Type;
        Hold->Name = NewCharString(Name);
        Hold->bIsEndCase = TRUE;
        Hold->NoPush = NoPush;
        Hold->Good.next = G;
        Hold->Bad.next = B;
        Hold->Depend = malloc( sizeof(S_PARMS) );
        
        if ( Hold->Depend == NULL ) {
            DbgPrint( "BADMAN: NewParm: Unable to allocate %d bytes.\n", sizeof(S_PARMS) );
            return NULL;
        } else {
            Hold->Depend->next = Depend;
        }
        
        Hold->next = next;
    }

    return Hold;
}


BOOL
ValidParmName(
    char *Name
    )

/*++

Routine Description:

    Check to make sure that the name given is a valid variable name.  As far as
    I know, this means the first character must be alphabetic or an underscore,
    and following characters must be alphanumeric or the underscore.

Arguments:

Return Value:

--*/

{
    BOOL Hold;

    Hold = TRUE;

    if ( *Name == '_' || isalpha(*Name) ) {
        Name++;

        while ( *Name != '\0' ) {
            if ( !( *Name == '_' || isalnum(*Name) ) ) {
                Hold = FALSE;
            }

            Name++;
        }
    } else {
        Hold = FALSE;
    }

    return Hold;
}


GoodValType *
NewGoodVal(
    DefineType  *Def,
    GoodValType *next
    )

/*++

Routine Description:

    Create a GoodValType variable, and fill it with the appropriate information.

Arguments:

Return Value:

--*/

{
    GoodValType *Hold;

    Hold = malloc(sizeof (GoodValType));

    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewGoodVal: Unable to allocate %d bytes.\n",
            sizeof(GoodValType)
            );
    } else {
        Hold->V = Def;
        Hold->next = next;
    }

    return Hold;
}


BadValType *
NewBadVal(
    DefineType *Def,
    DefineType *LDef,
    int        LastError,
    BadValType *next
    )

/*++

Routine Description:

    Create a BadValType variable, and fills it with the appropriate information

Arguments:

Return Value:

--*/

{
    BadValType *Hold;

    Hold = malloc( sizeof(BadValType) );
    
    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewBadVal: Unable to allocate %d bytes.\n",
            sizeof(BadValType)
            );
    } else {
        Hold->V = Def;
        Hold->L = LDef;
        Hold->LastError = LastError;
        Hold->next = next;
    }

    return Hold;
}


MacroValType *
NewMacroVal(
    DefineType   *Def,
    DefineType   *LDef,
    int          LastError,
    MacroValType *next
    )

/*++

Routine Description:

    Create a MacroValType variable, and fills it with the appropriate information

Arguments:

Return Value:

--*/

{
    MacroValType *Hold;

    Hold = malloc( sizeof(MacroValType) );

    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewMacroVal: Unable to allocate %d bytes.\n",
            sizeof(MacroValType)
            );
    } else {
        Hold->V = Def;
        Hold->L = LDef;
        Hold->LastError = LastError;
        Hold->next = next;
    }

    return Hold;
}


MacroType *
NewMacro(
    char          *Name,
    MacroValType  *M,
    MacroType     *next
    )

/*++

Routine Description:

    Create a new macro type, and initializes it.  Note that it only makes a
    copy of the M parm.

Arguments:

Return Value:

--*/

{
    MacroType     *Hold;
    MacroValType  *MVTrav, *MVTrav2;

    Hold = malloc( sizeof(MacroType) );

    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewMacro: Unable to allocate %d bytes.\n",
            sizeof(MacroType)
            );
    } else {
        Hold->Name = NewCharString(Name);
        Hold->next = next;

        //
        // Make a copy of the M parm.
        //
        Hold->M.next = NULL;

        if ( M != NULL ) {
            MVTrav = &Hold->M;
            MVTrav2 = M;

            while ( MVTrav2->next != NULL ) {
                MVTrav2 = MVTrav2->next;
                
                // BUGBUG:  No cleanup if this fails...
                MVTrav->next = NewMacroVal(
                                    MVTrav2->V,
                                    MVTrav2->L,
                                    MVTrav2->LastError,
                                    NULL
                                    );

                MVTrav = MVTrav->next;
            }
        }
    }

    return Hold;
}


BOOL
ExistingParmName(
    char     *Name,
    S_PARMS  *PList
    )

/*++

Routine Description:

    Indicate whether or not the parameter name in question has already been
    entered. This lets us determine if entries are correct. Note that this
    test is case sensitive.

Arguments:

Return Value:

--*/

{
    S_PARMS  *PTrav;

    PTrav = PList;

    while ( PTrav->next != NULL ) {
        PTrav = PTrav->next;

        if ( !strcmp( PTrav->Name, Name ) ) {
            return TRUE;
        }
    }

    return FALSE;
}


void
ReadStringHash(
    char        *FileName,
    StringType  Hash[],
    int         HashSize
    )

/*++

Routine Description:

    Read in the entries in FileName token by token, hashing them into Hash.

Arguments:

Return Value:

--*/

{
    FILE  *inFPtr;
    char  Buff[1024];
    StringType  *STrav;

    if ( NULL == (inFPtr = fopen( FileName, "r" )) ) {
        DbgPrint( "BADMAN: ReadStringHash: Unable to open %s\n", FileName );
        ExitThread( EXIT_FAILURE );
    }

    while ( fscanf( inFPtr, "%s", Buff) != EOF ) {
        ASSERT( strlen(Buff) < sizeof(Buff) );
        STrav = &Hash[ HashVal(Buff, HashSize) ];
        STrav->next = NewString( Buff, STrav->next );
    }

    fclose( inFPtr );
}


StringType
*NewString(
    char        *token,
    StringType  *next
    )

/*++

Routine Description:

    Allocate and fill a new string

Arguments:

Return Value:

--*/

{
    StringType  *Hold;

    Hold = malloc( sizeof(StringType) );
    
    if ( Hold == NULL ) {
        DbgPrint( "BADMAN: NewString: Unable to allocate %d bytes.\n", sizeof(StringType) );
    } else {
        Hold->Name = NewCharString(token);
        Hold->next = next;
    }

    return Hold;
}


void
DumpDefines(
    DefineType Hash[HASH_SIZE]
    )

/*++

Routine Description:

    Print out a list of the defines by bucket.

Arguments:

Return Value:

--*/

{
    int i;
    DefineType *DTrav;
    
    DbgPrint("BADMAN: ------- Defines -------\n");
    
    for ( i=0; i<HASH_SIZE; i++) {
        if ( Hash[i].next != NULL ) {
            DTrav = Hash[i].next;

            while ( DTrav != NULL ) {
                DbgPrint("BADMAN: %s [%s = %d]\n",
                    DTrav -> Comment,
                    DTrav -> Symbol,
                    DTrav -> Case
                    );

                DTrav = DTrav->next;
            }
        }
    }
}


void
DumpStrings(
    StringType  Hash[],
    int         HashSize
    )

/*++

Routine Description:

    Print out a list of strings by bucket

Arguments:

Return Value:

--*/

{
    int  i;
    StringType  *DTrav;
    
    DbgPrint("BADMAN: ------- Strings -------\n");
    
    for ( i=0; i<HashSize; i++) {
        if ( Hash[i].next != NULL ) {
            DTrav = Hash[i].next;
            while ( DTrav != NULL ) {
                DbgPrint("BADMAN: [%s]", DTrav->Name);
                DTrav = DTrav->next;
            }
        }
    }
}


void
DumpParms(
    S_PARMS  *P
    )

/*++

Routine Description:

    Print out the names of parms and their dependencies.

Arguments:

Return Value:

--*/

{
    S_PARMS  *PTrav;

    if (!P) {
        return;
    }

    PTrav = P;
    
    while (PTrav != NULL) {
        DbgPrint("BADMAN: (%s ", PTrav->Name);
        DumpParms(PTrav->Depend->next);
        PTrav = PTrav->next;
    }

    DbgPrint(")\n");
}


S_PARMS
*DestroyParm(
    S_PARMS  *P
    )

/*++

Routine Description:

    Delete a S_PARMS  data structure and returns its NEXT field

Arguments:

Return Value:

--*/

{
    S_PARMS  *PHold, *PHold2, *PTrav;
    GoodValType  *GTrav, *GHold;
    BadValType  *BTrav, *BHold;
    
    PHold = P->next;
    free(P->Name);

    GTrav = P->Good.next;

    while ( GTrav != NULL ) {
        GHold = GTrav;
        GTrav = GTrav->next;
        free( GHold );
    }

    BTrav = P -> Bad.next;

    while ( BTrav != NULL ) {
        BHold = BTrav;
        BTrav = BTrav->next;
        free( BHold );
    }

    PTrav = P->Depend;

    if ( PTrav != NULL ) {
        PTrav = PTrav->next;

        while ( PTrav != NULL ) {
            PHold2 = PTrav;
            PTrav = PTrav->next;
            DestroyParm( PHold2 );
        }
    }

    free( P->Depend );

    free( P );

    return PHold;
}


SeenType *
NewSeen(
    SeenType  *next
    )

/*++

Routine Description:

    Create and initialize a structure of type SeenType.

Arguments:

Return Value:

--*/

{
    SeenType  *Hold;
    int  i;

    Hold = malloc( sizeof(SeenType) );

    if ( Hold == NULL ) {
        DbgPrint(
            "BADMAN: NewSeenType: Unable to allocate %d bytes\n",
            sizeof(SeenType)
            );
    } else {
        for ( i=0; i<MAXNOPARMS; i++ ) {
            Hold->S[i] = FALSE;
        }

        Hold->next = next;
    }

    return Hold;
}


void
DumpFullParm(
    S_PARMS *P
    )

/*++

Routine Description:

    Print the type, name, and good and bad value lists for the parameter data
    structure passed in

Arguments:

Return Value:

--*/

{
    GoodValType *GTrav;
    BadValType  *BTrav;

    DbgPrint( "BADMAN: %s %s\n", P->Type->Name, P->Name );
    DbgPrint( "BADMAN: Good:\n");

    GTrav = P->Good.next;

    while ( GTrav != NULL ) {
        DbgPrint("BADMAN: %s\n", GTrav->V->Symbol );
        GTrav = GTrav->next;
    }

    DbgPrint("BADMAN: Bad:\n");
    BTrav = P->Bad.next;

    while ( BTrav != NULL ) {
        DbgPrint("BADMAN: %s\n", BTrav->V->Symbol );

        if ( BTrav->L != NULL ) {
            DbgPrint( "BADMAN: = %s", BTrav->L->Symbol );
        }

        BTrav = BTrav->next;
    }
}


void
DumpMacros(
    MacroType  *MacroHash
    )

/*++

Routine Description:

    Print out all the macros which have been read in.

Arguments:

Return Value:

--*/

{
    MacroType     *MTrav;
    MacroValType  *MVTrav;
    int  i;
    
    DbgPrint( "BADMAN: ------ Macros ------\n" );
    
    for ( i=0; i<MACRO_HASH_SIZE; i++ ) {
        MTrav = &MacroHash[i];

        while ( MTrav->next != NULL ) {
            MTrav = MTrav->next;
            DbgPrint( "BADMAN: %s\n", MTrav->Name );

            MVTrav = &MTrav->M;

            while ( MVTrav->next != NULL ) {
                MVTrav = MVTrav->next;

                DbgPrint( "BADMAN: %s\n", MVTrav->V->Symbol );
                
                if ( MVTrav->LastError != BAD_DEFINE ) {
                    DbgPrint("BADMAN: = \n");
                    if ( MVTrav->L != NULL ) {
                        DbgPrint( "BADMAN: %s\n", MVTrav->L->Symbol );
                    } else {
                        DbgPrint("BADMAN: %x\n", MVTrav->LastError );
                    }
                }
            }
        }
    }
}


VOID
FreeDefineType(
    DefineType * ptr
    )
{
    while ( ptr->next ) {
        DefineType * p = ptr->next;
        ptr->next = p->next;
        free( p->Symbol );
        free( p->Comment );
        free( p );
    }
}


VOID
FreeStringType(
    StringType * ptr
    )
{
    while ( ptr->next ) {
        StringType * p = ptr->next;
        ptr->next = p->next;
        free( p->Name );
        free( p );
    }
}


VOID
FreeValType(
    MacroValType * ptr
    )
{
    while ( ptr->next ) {
        MacroValType * p = ptr->next;
        ptr->next = p->next;
        FreeDefineType( p->V );
        FreeDefineType( p->L );
        free( p );
    }
}


VOID
FreeMacroType(
    MacroType * ptr
    )
{
    while ( ptr->next ) {
        MacroType * p = ptr->next;
        ptr->next = p->next;
        free( p->Name );
        FreeValType( &p->M );
        free( p );
    }
}
