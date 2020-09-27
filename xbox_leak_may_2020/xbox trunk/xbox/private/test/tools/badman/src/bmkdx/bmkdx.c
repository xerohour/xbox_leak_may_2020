/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    bmkdx.c

Abstract:

    This module is kernel debugger extension for badman. It contains useful
    commands for debugging badman engine

Author:

    Sakphong Chanbai (schanbai) 15-Apr-2000

Environment:

    XBox

Revision History:

--*/


#include "precomp.h"
#include <xboxverp.h>
#include <badman.h>
#include <parsedef.h>


WINDBG_EXTENSION_APIS ExtensionApis;
EXT_API_VERSION ApiVersion = { 5, 0, EXT_API_VERSION_NUMBER, 0 };


USHORT SavedMajorVersion;
USHORT SavedMinorVersion;
BOOL   ChkTarget;            // is debuggee a CHK build?


BOOL
GetData(
    LPVOID ptr,
    PVOID dwAddress,
    ULONG size,
    PCSTR type
    )

/*++

Routine Description:

    Get 'size' bytes from the debuggee program at 'dwAddress' and place it
    in our address space at 'ptr'.  Use 'type' in an error printout if necessary

Arguments:

Return Value:

--*/

{
    BOOL b;
    ULONG BytesRead;
    ULONG count = size;
    ULONG_PTR dwAddr = (ULONG_PTR)dwAddress;

    while( size > 0 ) {

    if (count >= 3000)
        count = 3000;

        b = ReadMemory(dwAddr, ptr, count, &BytesRead );

        if (!b || BytesRead != count ) {
            dprintf( "Unable to read %u bytes at %X, for %s\n", size, dwAddress, type );
            return FALSE;
        }

        dwAddr += count;
        size -= count;
        ptr = (LPVOID)((ULONG_PTR)ptr + count);
    }

    return TRUE;
}


BOOL
GetString(
    IN  LPSTR dwAddress,
    OUT LPSTR buf,
    IN  ULONG MaxChars
    )
{
    do {
        if( !GetData( buf, dwAddress, sizeof( *buf ), "Character" ) )
            return FALSE;

        dwAddress += sizeof( *buf );

    } while( (--MaxChars) != 0 && *buf++ != '\0' );

    return TRUE;
}


BOOL
GetStringFromLPSTR(
    LPVOID dwAddress,
    LPSTR buf,
    ULONG MaxChars
    )
{
    LPSTR lpAddress;
    *buf = 0;

    if ( !GetData( &lpAddress, dwAddress, sizeof(LPVOID), "LPVOID" ) ) {
        return FALSE;
    }

    return GetString( lpAddress, buf, MaxChars );
}


DECLARE_API( help )
{
    dprintf( "BADMAN kernel debugger extensions:\n" );
    dprintf( "  genparm    <ChildEBP>\n" );
}


DECLARE_API( genparm )
{
    S_PARMS * ParmList;
    APIType * API;
    DefineType * VarInfo;
    ULONG ChildEBP, NumParms, i;
    int * ParmGoodBad;
    char APIName[128];

    if ( !args || !*args || (ChildEBP = GetExpression( args )) == 0 ) {
        dprintf( "genparm  <ChildEBP>, example: genparm 0xdeadbeef\n" );
        return;
    }

    ChildEBP += 4;

    if ( !GetData( &ParmList, (PVOID)(ChildEBP+(1*4)), sizeof(S_PARMS *), "S_PARMS" ) ||
         !GetData( &NumParms, (PVOID)(ChildEBP+(3*4)), sizeof(ULONG), "NumParms" ) ||
         !GetData( &API, (PVOID)(ChildEBP+(13*4)), sizeof(APIType *), "APIType" ) ||
         !GetData( &VarInfo, (PVOID)(ChildEBP+(12*4)), sizeof(DefineType *), "DefineType" ) ||
         !GetData( &ParmGoodBad, (PVOID)(ChildEBP+(15*4)), sizeof(int *), "ParmGoodBad" ) ||
         !GetStringFromLPSTR( &API->Name, APIName, sizeof(APIName) )
       ) {
        return;
    }

    dprintf( "Parameters list (S_PARMS) = %p\n", ParmList );
    dprintf( "Number of parameters      = %d\n", NumParms );
    dprintf( "API type (APIType)        = %p\n", API );
    dprintf( "Define type (DefineType)  = %p\n", VarInfo );
    dprintf( "\n" );
    dprintf("%s(\n", APIName );

    for ( i=0; i<NumParms; i++) {

        int GoodBad;
        char ParmListName[256];
        char VarInfoSymbol[256];
        char VarInfoComment[512];
        DefineType DefStruct;
        S_PARMS ParmStruct;


        if ( !GetData( &GoodBad, &ParmGoodBad[i], sizeof(GoodBad), "int" ) ||
             !GetData( &ParmStruct, &ParmList[i], sizeof(ParmStruct), "S_PARMS" ) ||
             !GetData( &DefStruct, &VarInfo[i], sizeof(DefStruct), "DefineType" ) ||
             !GetStringFromLPSTR( &ParmStruct.next->Name, ParmListName, sizeof(ParmListName) ) ||
             !GetStringFromLPSTR( &DefStruct.next->Symbol, VarInfoSymbol, sizeof(VarInfoSymbol) ) ||
             !GetStringFromLPSTR( &DefStruct.next->Comment, VarInfoComment, sizeof(VarInfoComment) )
           ) {
            break;
        }

        //
        // +'s around good parms, -'s around bad parms
        //

        if (GoodBad & CLOPT_BAD) {
            dprintf( "    -%-25s %s -->%s", ParmListName, VarInfoSymbol, VarInfoComment );
        } else {
            dprintf( "    +%-25s %s -->%s", ParmListName, VarInfoSymbol, VarInfoComment );
        }
    }

    dprintf( "    );\n\n" );
}


VOID
WinDbgExtensionDllInit(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT MajorVersion,
    USHORT MinorVersion
    )
{
    ExtensionApis = *lpExtensionApis;
    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;
    ChkTarget = SavedMajorVersion == 0x0c ? TRUE : FALSE;
}


DECLARE_API( version )
{
#if DBG
    PCSTR kind = "Checked";
#else
    PCSTR kind = "Free";
#endif

    dprintf(
        "%s BADMAN extension dll for build %d debugging %s kernel for build %d\n",
        kind,
        VER_PRODUCTBUILD,
        SavedMajorVersion == 0x0c ? "checked" : "free",
        SavedMinorVersion
    );
}


VOID
CheckVersion(
    VOID
    )
{
#if DBG
    if ((SavedMajorVersion != 0x0c) || (SavedMinorVersion != VER_PRODUCTBUILD)) {
        dprintf("\r\n*** Extension DLL(%d Checked) does not match target system(%d %s)\r\n\r\n",
                VER_PRODUCTBUILD, SavedMinorVersion, (SavedMajorVersion==0x0f) ? "Free" : "Checked" );
    }
#else
    if ((SavedMajorVersion != 0x0f) || (SavedMinorVersion != VER_PRODUCTBUILD)) {
        dprintf("\r\n*** Extension DLL(%d Free) does not match target system(%d %s)\r\n\r\n",
                VER_PRODUCTBUILD, SavedMinorVersion, (SavedMajorVersion==0x0f) ? "Free" : "Checked" );
    }
#endif
}


LPEXT_API_VERSION
ExtensionApiVersion(
    VOID
    )
{
    return &ApiVersion;
}
