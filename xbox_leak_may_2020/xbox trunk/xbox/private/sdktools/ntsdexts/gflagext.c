#include <ntexapi.dbg>

VOID
GFlagExtension(
    PVOID pNtGlobalFlag,
    ULONG ValidBits,
    PCSTR lpArgumentString
    )
{
    ULONG i;
    ULONG OldGlobalFlags;
    ULONG NewGlobalFlagsClear;
    ULONG NewGlobalFlagsSet;
    ULONG NewGlobalFlags;
    LPSTR s, Arg;

    if (pNtGlobalFlag == NULL) {
        dprintf( "Unable to get address of NtGlobalFlag variable" );
        return;
        }

    if (!ReadMemory( (ULONG_PTR) pNtGlobalFlag,
                     &OldGlobalFlags,
                     sizeof( OldGlobalFlags ),
                     NULL
                   )
       ) {
        dprintf( "Unable to read contents of NtGlobalFlag variable at 0x%x", pNtGlobalFlag );
        return;
        }
    OldGlobalFlags &= ValidBits;

    s = (LPSTR)lpArgumentString;
    if (!s)
        s = "";

    NewGlobalFlagsClear = 0;
    NewGlobalFlagsSet = 0;
    while (*s) {
        while (*s && *s <= ' ')
            s += 1;

        Arg = s;
        if (!*s)
            break;

        while (*s && *s > ' ')
            s += 1;


        if (*s)
            *s++ = '\0';

        if (!strcmp( Arg, "-?" )) {
            dprintf( "usage: !gflag [-? | flags]\n" );
            dprintf( "Flags may either be a single hex number that specifies all\n" );
            dprintf( "32-bits of the GlobalFlags value, or it can be one or more\n" );
            dprintf( "arguments, each beginning with a + or -, where the + means\n" );
            dprintf( "to set the corresponding bit(s) in the GlobalFlags and a -\n" );
            dprintf( "means to clear the corresponding bit(s).  After the + or -\n" );
            dprintf( "may be either a hex number or a three letter abbreviation\n" );
            dprintf( "for a GlobalFlag.  Valid abbreviations are:\n" );
            for (i=0; i<32; i++) {
                if ((GlobalFlagInfo[i].Flag & ValidBits) &&
                    GlobalFlagInfo[i].Abbreviation != NULL
                   ) {
                    dprintf( "    %s - %s\n", GlobalFlagInfo[i].Abbreviation,
                                                      GlobalFlagInfo[i].Description
                           );
                    }
                }

            return;
            }

        if (*Arg == '+' || *Arg == '-') {
            if (strlen(Arg+1) == 3) {
                for (i=0; i<32; i++) {
                    if ((GlobalFlagInfo[i].Flag & ValidBits) &&
                        !_stricmp( GlobalFlagInfo[i].Abbreviation, Arg+1 )
                       ) {
                        if (*Arg == '-') {
                            NewGlobalFlagsClear |= GlobalFlagInfo[i].Flag;
                            }
                        else {
                            NewGlobalFlagsSet |= GlobalFlagInfo[i].Flag;
                            }

                        Arg += 4;
                        break;
                        }
                    }

                if (*Arg != '\0') {
                    dprintf( "Invalid flag abbreviation - '%s'\n", Arg );
                    return;
                    }
                }

            if (*Arg != '\0') {
                if (*Arg++ == '-') {
                    NewGlobalFlagsClear |= strtoul( Arg, &Arg, 16 );
                    }
                else {
                    NewGlobalFlagsSet |= strtoul( Arg, &Arg, 16 );
                    }
                }
            }
        else {
            NewGlobalFlagsSet = strtoul( Arg, &Arg, 16 );
            break;
            }
        }

    NewGlobalFlags = (OldGlobalFlags & ~NewGlobalFlagsClear) | NewGlobalFlagsSet;
    NewGlobalFlags &= ValidBits;
    if (NewGlobalFlags != OldGlobalFlags) {
        if (!WriteMemory( (ULONG_PTR)pNtGlobalFlag,
                          &NewGlobalFlags,
                          sizeof( NewGlobalFlags ),
                          NULL
                        )
           ) {
            dprintf( "Unable to store new global flag settings.\n" );
            return;
            }

        dprintf( "New NtGlobalFlag contents: 0x%08x\n", NewGlobalFlags );
        OldGlobalFlags = NewGlobalFlags;
        }
    else {
        dprintf( "Current NtGlobalFlag contents: 0x%08x\n", OldGlobalFlags );
        }
    for (i=0; i<32; i++) {
        if (OldGlobalFlags & GlobalFlagInfo[i].Flag) {
            dprintf( "    %s - %s\n", GlobalFlagInfo[i].Abbreviation, GlobalFlagInfo[i].Description );
            }
        }
}
