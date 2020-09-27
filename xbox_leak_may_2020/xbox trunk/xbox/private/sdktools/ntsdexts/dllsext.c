VOID DumpImage(
    ULONG_PTR xBase,
    BOOL DoHeaders,
    BOOL DoSections
    );

typedef enum {
    Memory = 1,
    Load = 2,
    Init = 3
} ELOAD_ORDER;

VOID
DllsExtension(
    PCSTR lpArgumentString,
    PPEB ProcessPeb
    )
{
    BOOL b;
    PLDR_DATA_TABLE_ENTRY pLdrEntry;
    LDR_DATA_TABLE_ENTRY LdrEntry;
    PEB_LDR_DATA PebLdrData;
    PLIST_ENTRY Next;
    WCHAR StringData[MAX_PATH+1];
    BOOL SingleEntry;
    BOOL DoHeaders;
    BOOL DoSections;
    BOOL DoAll;
    PSTR lpArgs = (PSTR)lpArgumentString;
    PSTR p;
    INT_PTR addrContaining = 0;
    ELOAD_ORDER OrderList = Load;
    PLIST_ENTRY OrderModuleListStart;

    SingleEntry = FALSE;
    DoAll = FALSE;
    DoHeaders = FALSE;
    DoSections = FALSE;

#if 0
    while ( lpArgumentString != NULL && *lpArgumentString ) {
        if (*lpArgumentString != ' ') {
            sscanf(lpArgumentString,"%lx",&pLdrEntry);
            SingleEntry = TRUE;
            goto dumpsingleentry;
            }

        lpArgumentString++;
        }
#endif

    while (*lpArgs) {

        while (isspace(*lpArgs)) {
            lpArgs++;
            }

        if (*lpArgs == '/' || *lpArgs == '-') {

            // process switch

            switch (*++lpArgs) {

                case 'a':   // dump everything we can
                case 'A':
                    ++lpArgs;
                    DoAll = TRUE;
                    break;

                case 'c':   // dump only the dll containing the specified address
                case 'C':
                    lpArgs += 2;    // step over the c and the space.
                    addrContaining = GetExpression(lpArgs);

                    while (*lpArgs && (!isspace(*lpArgs))) {
                        lpArgs++;
                    }

                    if (addrContaining != 0) {
                        dprintf("Dump dll containing 0x%p:\n", addrContaining);
                    } else {
                        dprintf("-c flag requires and address arguement\n");
                        return;
                    }
                    break;

                default: // invalid switch

                case 'h':   // help
                case 'H':
                case '?':

                    dprintf("Usage: dlls [options] [address]\n");
                    dprintf("\n");
                    dprintf("Displays loader table entries.  Optionally\n");
                    dprintf("dumps image and section headers.\n");
                    dprintf("\n");
                    dprintf("Options:\n");
                    dprintf("\n");
                    dprintf("   -a      Dump everything\n");
                    dprintf("   -c nnn  Dump dll containing address nnn\n");
                    dprintf("   -f      Dump file headers\n");
                    dprintf("   -i      Dump dll's in Init order\n");
                    dprintf("   -l      Dump dll's in Load order (the default)\n");
                    dprintf("   -m      Dump dll's in Memory order\n");
                    dprintf("   -s      Dump section headers\n");
                    dprintf("\n");

                    return;

                case 'f':
                case 'F':
                    ++lpArgs;
                    DoAll = FALSE;
                    DoHeaders = TRUE;
                    break;

                case 'm':   // dump in memory order
                case 'M':
                    ++lpArgs;
                    OrderList = Memory;
                    break;

                case 'i':   // dump in init order
                case 'I':
                    ++lpArgs;
                    OrderList = Init;
                    break;

                case 'l':   // dump in load order
                case 'L':
                    ++lpArgs;
                    OrderList = Load;
                    break;

                case 's':
                case 'S':
                    ++lpArgs;
                    DoAll = FALSE;
                    DoSections = TRUE;
                    break;

                }

            }
        else if (*lpArgs) {
            CHAR c;

            if (SingleEntry) {
                dprintf("Invalid extra argument\n");
                return;
                }

            p = lpArgs;
            while (*p && !isspace(*p)) {
                p++;
                }
            c = *p;
            *p = 0;

            pLdrEntry = (PLDR_DATA_TABLE_ENTRY)GetExpression(lpArgs);
            SingleEntry = TRUE;

            *p = c;
            lpArgs=p;

            }

        }

    if (SingleEntry) {
        goto dumpsingleentry;
        }

    //
    // Capture PebLdrData
    //

    b = ReadMemory( (ULONG_PTR)(ProcessPeb->Ldr),
                    &PebLdrData,
                    sizeof( PebLdrData ),
                    NULL
                  );
    if (!b) {
        dprintf( "    Unabled to read PebLdrData\n" );
        return;
        }

    //
    // Walk through the loaded module table and display all ldr data
    //

    switch (OrderList) {
        case Memory:
            OrderModuleListStart = &ProcessPeb->Ldr->InMemoryOrderModuleList;
            Next = (PLIST_ENTRY)PebLdrData.InMemoryOrderModuleList.Flink;
            break;

        case Init:
            OrderModuleListStart = &ProcessPeb->Ldr->InInitializationOrderModuleList;
            Next = (PLIST_ENTRY)PebLdrData.InInitializationOrderModuleList.Flink;
            break;

        default:
        case Load:
            OrderModuleListStart = &ProcessPeb->Ldr->InLoadOrderModuleList;
            Next = (PLIST_ENTRY)PebLdrData.InLoadOrderModuleList.Flink;
            break;
    }

    while (Next != OrderModuleListStart) {
        if (CheckControlC()) {
            return;
            }

        switch (OrderList) {
            case Memory:
                pLdrEntry = CONTAINING_RECORD(Next,LDR_DATA_TABLE_ENTRY,InMemoryOrderLinks);
                break;

            case Init:
                pLdrEntry = CONTAINING_RECORD(Next,LDR_DATA_TABLE_ENTRY,InInitializationOrderLinks);
                break;

            default:
            case Load:
                pLdrEntry = CONTAINING_RECORD(Next,LDR_DATA_TABLE_ENTRY,InLoadOrderLinks);
                break;
        }

        //
        // Capture LdrEntry
        //
dumpsingleentry:

        b = ReadMemory( (ULONG_PTR)pLdrEntry,
                        &LdrEntry,
                        sizeof( LdrEntry ),
                        NULL
                      );

        if (!b) {
            dprintf( "    Unabled to read Ldr Entry at %x\n", pLdrEntry );
            return;
        }

        ZeroMemory( StringData, sizeof( StringData ) );
        b = ReadMemory( (ULONG_PTR)(LdrEntry.FullDllName.Buffer),
                        StringData,
                        LdrEntry.FullDllName.Length,
                        NULL
                      );
        if (!b) {
            dprintf( "    Unabled to read Module Name\n" );
            ZeroMemory( StringData, sizeof( StringData ) );
        }

        //
        // Dump the ldr entry data
        // (dump all the entries if no containing address specified)
        //
        if ((addrContaining == 0) ||
            (((DWORD_PTR)LdrEntry.DllBase <= (DWORD_PTR)addrContaining) &&
             ((DWORD_PTR)addrContaining <= ((DWORD_PTR)LdrEntry.DllBase + (DWORD_PTR)LdrEntry.SizeOfImage))
            )
           ) {
            dprintf( "\n" );
            dprintf( "0x%08x: %ws\n", pLdrEntry, StringData[0] ? StringData : L"Unknown Module" );
            dprintf( "      Base   0x%08x  EntryPoint  0x%08x  Size        0x%08x\n",
                     LdrEntry.DllBase,
                     LdrEntry.EntryPoint,
                     LdrEntry.SizeOfImage
                   );
            dprintf( "      Flags  0x%08x  LoadCount   0x%08x  TlsIndex    0x%08x\n",
                     LdrEntry.Flags,
                     LdrEntry.LoadCount,
                     LdrEntry.TlsIndex
                   );

            if (LdrEntry.Flags & LDRP_STATIC_LINK) {
                dprintf( "             LDRP_STATIC_LINK\n" );
                }
            if (LdrEntry.Flags & LDRP_IMAGE_DLL) {
                dprintf( "             LDRP_IMAGE_DLL\n" );
                }
            if (LdrEntry.Flags & LDRP_LOAD_IN_PROGRESS) {
                dprintf( "             LDRP_LOAD_IN_PROGRESS\n" );
                }
            if (LdrEntry.Flags & LDRP_UNLOAD_IN_PROGRESS) {
                dprintf( "             LDRP_UNLOAD_IN_PROGRESS\n" );
                }
            if (LdrEntry.Flags & LDRP_ENTRY_PROCESSED) {
                dprintf( "             LDRP_ENTRY_PROCESSED\n" );
                }
            if (LdrEntry.Flags & LDRP_ENTRY_INSERTED) {
                dprintf( "             LDRP_ENTRY_INSERTED\n" );
                }
            if (LdrEntry.Flags & LDRP_CURRENT_LOAD) {
                dprintf( "             LDRP_CURRENT_LOAD\n" );
                }
            if (LdrEntry.Flags & LDRP_FAILED_BUILTIN_LOAD) {
                dprintf( "             LDRP_FAILED_BUILTIN_LOAD\n" );
                }
            if (LdrEntry.Flags & LDRP_DONT_CALL_FOR_THREADS) {
                dprintf( "             LDRP_DONT_CALL_FOR_THREADS\n" );
                }
            if (LdrEntry.Flags & LDRP_PROCESS_ATTACH_CALLED) {
                dprintf( "             LDRP_PROCESS_ATTACH_CALLED\n" );
                }
            if (LdrEntry.Flags & LDRP_DEBUG_SYMBOLS_LOADED) {
                dprintf( "             LDRP_DEBUG_SYMBOLS_LOADED\n" );
                }
            if (LdrEntry.Flags & LDRP_IMAGE_NOT_AT_BASE) {
                dprintf( "             LDRP_IMAGE_NOT_AT_BASE\n" );
                }
            if (LdrEntry.Flags & LDRP_WX86_IGNORE_MACHINETYPE) {
                dprintf( "             LDRP_WX86_IGNORE_MACHINETYPE\n" );
                }
        }

        if (DoAll || DoHeaders || DoSections) {
            DumpImage( (ULONG_PTR)LdrEntry.DllBase,
                       DoAll || DoHeaders,
                       DoAll || DoSections );
        }

        if (SingleEntry) {
            return;
        }

        switch (OrderList) {
            case Memory:
                Next = LdrEntry.InMemoryOrderLinks.Flink;
                break;

            case Init:
                Next = LdrEntry.InInitializationOrderLinks.Flink;
                break;

            default:
            case Load:
                Next = LdrEntry.InLoadOrderLinks.Flink;
                break;
        }
    }
}
