/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    xbperf.cpp

Abstract:

    This module implements routines to dump call-attributed profile (CAP)
    data for Xbox.

--*/

#include "stdafx.h"
#include "xboxverp.h"

#define optequ(s1, s2) (_stricmp(s1, s2) == 0)
#define XBOX_CAP_FILENAME "e:\\xboxcap.dat"

#define CALL_TYPE(type) (entry->calltype == XPROFREC_FASTCAP_START ? 'F' : \
    (entry->calltype == XPROFREC_CALLCAP_START ? 'C' : 'P'))

enum {
    INVALIDCMD = 0x0000,
    STARTCMD   = 0x0001,
    STOPCMD    = 0x0002,
    DUMPCMD    = 0x0004
};

// sort key table: must be sorted
struct _sortkeytable {
    const char* key;
    CFunctionTable::sortkey sortkey;
};

const _sortkeytable sortkeytable[] = {
    "address", CFunctionTable::sortby_address,
    "callin", CFunctionTable::sortby_callin,
    "callout", CFunctionTable::sortby_callout,
    "excltime", CFunctionTable::sortby_excltime,
    "incltime", CFunctionTable::sortby_incltime,
    "name", CFunctionTable::sortby_name,
};

PCHAR ProgramName;

bool verbose, dumpdpc, noidle;
bool rawdump = true;
CFunctionTable::sortkey sortby = CFunctionTable::sortby_unknown;

bool summaryonly, csvformat;
CFunctionTable functable;

ULONGLONG capstarttick, capstoptick;

void checkerr(BOOL err, CHAR* format, ...)
{
    if (!err) return;
    va_list arglist;

    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);

    exit(-1);
}

void errmsg(CHAR* format, ...)
{
    va_list arglist;

    va_start(arglist, format);
    vfprintf(stderr, format, arglist);
    va_end(arglist);
}

void usage()
{
    fprintf(stderr,
        "Microsoft Xbox Call-Attributed Profiler Version " VER_PRODUCTVERSION_STR "\n"
        "Copyright (c) Microsoft Corporation.  All rights reserved.\n\n"
        "Usage:\t%s options ...\n\n"
        "\t-x xboxname\tSpecifies the target Xbox name.\n\n"
        "\t-s symbol-path\tSpecifies the symbol search path.  You can specify\n"
        "\t\t\tmultiple directories by separating them with a\n"
        "\t\t\tsemicolon.  If -s option isn't present, _NT_SYMBOL_PATH\n"
        "\t\t\tenvironment variable is used.\n\n"
        "\t-idle\t\tDo not dump the time spent in the idle thread.\n"
        "\t\t\tInstead, the time is attributed to the thread that was\n"
        "\t\t\trunning before the system switched to the idle thread.\n\n"
        "\t-summary\tDump only summary report.\n\n"
        "\t-sort column\tSort summary report by column.  Column can be one of\n"
        "\t\t\tthe following:\n\n"
        "\t\t\taddress, callin, callout, incltime, excltime or name\n\n"
        "\t-csv\t\tGenerate .csv format summary report for Microsoft Excel\n\n"
        "\tstart\t\tStart profile data collection on the Xbox.\n\n"
        "\tstop logfile\tStop profile data collection on the Xbox, and retrieve\n"
        "\t\t\tthe profile data and store it in the specified file.\n\n"
        "\tdump logfile [textfile]\n"
        "\t\t\tDump the profile data to a human-readable text file or\n"
        "\t\t\tstandard output if the textfile is not specified.\n",
        ProgramName);

    exit(1);
}

void* mapfile(CHAR* filename, DWORD* size)
{
    HANDLE file, filemap;
    VOID* fileview = NULL;

    file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
                OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (file == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    *size = GetFileSize(file, NULL);
    if (*size == ~0UL) {
        CloseHandle(file);
        return NULL;
    }

    filemap = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);

    if (filemap != NULL) {
        fileview = MapViewOfFile(filemap, FILE_MAP_READ, 0, 0, 0);
        CloseHandle(filemap);
    }

    CloseHandle(file);
    return fileview;
}

CThreadProfileData* capparse(VOID* fileview, DWORD filesize)
{
    checkerr(filesize < XPROF_FILE_HEADER_SIZE || (filesize & 3) != 0,
        "File size error: too small or not a multiple of 4\n");

    XProfpFileHeader* fileheader = (XProfpFileHeader*)fileview;

    if (fileheader->version != XPROF_FILE_VERSION) {
        errmsg("File version mismatch: %x != %x\n", fileheader->version,
               XPROF_FILE_VERSION);
    }

    symcnt = fileheader->module_count;

    for (UINT i=0; i < symcnt; i++) {
        symfiles[i].loadaddr = fileheader->modules[i].loadaddr;
        symfiles[i].size = fileheader->modules[i].size;
        strcpy(symfiles[i].name, fileheader->modules[i].name);
    }

    loadsymfiles();

    fileview = fileheader+1;
    filesize -= XPROF_FILE_HEADER_SIZE;

    UINT count = filesize >> 2;
    ULONG* buf = (ULONG*) fileview;
    CThreadProfileData* threads = NULL;
    CThreadProfileData* curthread = NULL;
    CThreadProfileData* dpcthread = NULL;
    ULONGLONG firsttick, lasttick;

    firsttick = lasttick = 0;

    while (count) {
        if (count < 2) goto badrec;

        ULONGLONG tick;
        ULARGE_INTEGER* uli = (ULARGE_INTEGER*) &tick;
        uli->HighPart = buf[0] & ~XPROFREC_TYPEMASK;
        uli->LowPart = buf[1];
        lasttick = tick;

        DWORD rectype = buf[0] & XPROFREC_TYPEMASK;
        DWORD recsize;

        checkerr(
            (rectype != XPROFREC_CONTROL || buf[2] != XPROF_START) &&
            (curthread == NULL),
            "The first record must be a START record.\n");

        switch (rectype) {
        case XPROFREC_FASTCAP_START:
        case XPROFREC_PENTER:
            recsize = 4;
            if (count < recsize) goto badrec;
            checkerr(buf[2] == 0 || buf[3] == 0, "Invalid function address 0.\n");
            curthread->CallBegin(tick, buf[2], buf[3], rectype);
            break;

        case XPROFREC_CALLCAP_START:
            recsize = 3;
            if (count < recsize) goto badrec;
            checkerr(buf[2] == 0, "Invalid function address 0.\n");
            curthread->CallBegin(tick, buf[2], 0, rectype);
            break;

        case XPROFREC_FASTCAP_END:
        case XPROFREC_CALLCAP_END:
            recsize = 3;
            if (count < recsize) goto badrec;
            curthread->CallEnd(tick, buf[2], rectype);
            break;

        case XPROFREC_PEXIT:
            recsize = 4;
            if (count < recsize) goto badrec;
            curthread->CallEnd(tick, buf[3], rectype);
            break;

        case XPROFREC_CONTROL:
            recsize = 4;
            if (count < recsize) goto badrec;

            switch (buf[2]) {
            case XPROF_START:
                // must be the first record of the profile data file
                if (curthread) {
                    VERBOSE("Second START record treated as STOP.\n");
                    recsize = count;
                } else {
                    firsttick = tick;
                    curthread = threads = new CThreadProfileData(buf[3], tick);
                    checkerr(curthread == NULL, "Out of memory\n");
                }
                break;

            case XPROF_STOP:
                if (count > recsize) {
                    VERBOSE("Ignore data after STOP record.\n");
                }
                recsize = count;
                break;

            case XPROF_COMMENT:
                curthread->Comment(tick, buf[3]);
                break;

            case XPROF_THREAD_SWITCH:
                checkerr(curthread->IsDpcThread(),
                    "Thread switch in the middle of a DPC routine!\n");

                {
                    DWORD newtid = buf[3];

                    // deactivate the current thread
                    if (newtid == curthread->ThreadId()) {
                        errmsg("Switching to the same thread: %x?!\n", newtid);
                    }
                    curthread->Activate(tick, THREAD_INACTIVE, newtid);

                    // check if the new thread is already present
                    CThreadProfileData* oldthread = curthread;
                    curthread = threads;
                    while (curthread && curthread->ThreadId() != newtid) {
                        curthread = curthread->NextThread();
                    }

                    if (curthread) {
                        // cctivate the new thread if already present
                        curthread->Activate(tick, THREAD_ACTIVE);
                    } else {
                        // otherwise, create the new thread
                        curthread = new CThreadProfileData(newtid, tick);
                        checkerr(curthread == NULL, "Out of memory\n");

                        curthread->SetNextThread(threads);
                        threads = curthread;
                    }
                    curthread->SetPreviousThread(oldthread);
                }
                break;

            case XPROF_DPC_BEGIN:
                checkerr(curthread->IsDpcThread(), "Nested DPC call not expected!\n");
                curthread->Activate(tick, THREAD_INACTIVE, -1);

                if (dpcthread == NULL) {
                    dpcthread = new CThreadProfileData(-1, tick, TRUE);
                    checkerr(dpcthread == NULL, "Out of memory\n");
                } else {
                    dpcthread->Activate(tick, THREAD_ACTIVE);
                }

                dpcthread->SetNextThread(curthread);
                curthread = dpcthread;
                curthread->CallBegin(tick, 0, buf[3], rectype);
                break;

            case XPROF_DPC_END:
                if (curthread == dpcthread) {
                    checkerr(dpcthread->CallLevel() != 1, "Unbalanced DPC call.\n");
                    dpcthread->CallEnd(tick, buf[3], rectype);

                    dpcthread->Activate(tick, THREAD_INACTIVE);
                    curthread = dpcthread->NextThread();
                    curthread->Activate(tick, THREAD_ACTIVE);
                } else {
                    VERBOSE("Unexpected DPC-END record.\n");
                }
                break;

            default:
                goto badrec;
            }
            break;

        default:
            goto badrec;
        }

        count -= recsize;
        buf += recsize;
    }

    if (dpcthread) {
        if (dumpdpc) {
            dpcthread->SetNextThread(threads);
            threads = dpcthread;
        } else {
            delete dpcthread;
        }
    }

    // revert the thread list and perform postprocessing
    curthread = threads;
    threads = NULL;
    while (curthread) {
        CThreadProfileData* p = curthread;
        curthread = p->NextThread();
        p->SetNextThread(threads);
        threads = p;
        p->PostProcessing(firsttick, lasttick);
    }

    capstarttick = firsttick;
    capstoptick = lasttick;
    return threads;

badrec:
    errmsg("Bad record at offset: %x\n", (BYTE*) buf - (BYTE*) fileview);
    exit(-1);
}

void DumpThreadName(FILE* fout, DWORD tid)
{
    if (tid == ~0UL) {
        fprintf(fout, "[DPC]");
    } else if (tid == 0) {
        fprintf(fout, "[IDLE]");
    } else {
        fprintf(fout, "0x%02X", tid);
    }
}

void DumpIndent(FILE* fout, UINT level)
{
    for (UINT i=0; i<level; i++) {
        fprintf(fout, "%-2d ", i);
    }
}

UINT OpfDump(FILE* fout, CThreadProfileData* thread)
{
    fprintf(fout, "0\t0\tThread ");
    DumpThreadName(fout, thread->ThreadId());
    fprintf(fout, "\t0\t%I64u\t \t0\n", thread->ThreadTime()/733UI64);

    const CallDataEntry* entry;
    size_t position = 0;

    while ((entry = thread->GetCallDataEntry(position)) != NULL) {

        if (entry->type != ENTRYTYPE_FUNCCALL) {
            position++;
            continue;
        }

        SymEntry* symentry = symtable->Lookup(entry->callee ? entry->callee : entry->caller);

        fprintf(fout, "%d\t%d\t%s\t%d\t%I64u\t%c%s\n", 1, entry->level+1, symentry->name,
            symentry->module, entry->duration/733UI64, CALL_TYPE(entry->calltype),
            (entry->flags & ENTRYFLAG_NOEND) ? "\tIncomplete call" :
            (entry->flags & ENTRYFLAG_ORPHAN) ? "\tCallback?" : "");

        position++;
    }

    return thread->TotalCalls();
}

UINT RawDump(FILE* fout, CThreadProfileData* thread)
{
    fprintf(fout, "Thread ");
    DumpThreadName(fout, thread->ThreadId());
    fprintf(fout, " : time = %I64u\n", thread->ThreadTime()/733UI64);

    fprintf(fout, "%12s %12s   function\n", "start time", "duration");

    const CallDataEntry* entry;
    size_t position = 0;

    while ((entry = thread->GetCallDataEntry(position)) != NULL) {

        if (entry->type == ENTRYTYPE_THREADSWITCH && thread->IsDpcThread()) {
            position++;
            continue;
        }

        fprintf(fout, "%12I64u %12I64u ", entry->starttime/733UI64, entry->duration/733UI64);
        fprintf(fout, "%c ", (entry->flags & ENTRYFLAG_NOEND) ? '-' :
                 (entry->flags & ENTRYFLAG_ORPHAN) ? '?' : ' ');

        if (entry->type == ENTRYTYPE_FUNCCALL) {
            SymEntry* symentry;
            if (entry->level == 0) {
                if (thread->IsDpcThread()) {
                    fprintf(fout, "[DPC] ");
                } else if (entry->callee) {
                    symentry = symtable->Lookup(entry->caller);
                    fprintf(fout, "%s => ", symentry->name);
                }
            } else {
                DumpIndent(fout, entry->level);
                if (entry->callee && (entry->flags & ENTRYFLAG_ORPHAN)) {
                    symentry = symtable->Lookup(entry->caller);
                    fprintf(fout, "%s => ", symentry->name);
                }
            }

            symentry = symtable->Lookup(entry->callee ? entry->callee : entry->caller);
            fprintf(fout, "%s\n", symentry->name);
        } else if (entry->type == ENTRYTYPE_THREADSWITCH) {
            fprintf(fout, "thread-switch ");
            DumpThreadName(fout, entry->newtid);
            fprintf(fout, "\n");
        } else if (entry->type == ENTRYTYPE_COMMENT) {
            DumpIndent(fout, entry->level);
            fprintf(fout, "comment 0x%02X\n", entry->param);
        } else {
            fprintf(fout, "*** unknown entry type: 0x%02X\n", entry->type);
        }

        position++;
    }

    fprintf(fout, "\n");
    return thread->TotalCalls();
}

UINT AddSummary(CThreadProfileData* thread)
{
    const CallDataEntry* entry;
    size_t position = 0;

    while ((entry = thread->GetCallDataEntry(position)) != NULL) {

        if (entry->type != ENTRYTYPE_FUNCCALL) {
            position++;
            continue;
        }

        if (entry->type == ENTRYTYPE_FUNCCALL) {
            functable.add(entry);
        }

        position++;
    }

    return thread->TotalCalls();
}

void SummaryDump(FILE* fout)
{
    int i, count;

    if ((count = functable.getcount()) != 0) {
        fprintf(fout, csvformat ? "%s,%s,%s,%s,%s,%s\n" : "%-8s %7s %7s %9s %9s %s\n",
            "FuncAddr", " CallIn", "CallOut", "InclTime", "ExclTime", "Function Name");
    }

    if (sortby != CFunctionTable::sortby_unknown) {
        functable.sort(sortby);
    }

    for (i=0; i<count; i++) {
        const CallDataEntry* entry = functable.getentry(i);
        ULONG_PTR address = entry->callee ? entry->callee : entry->caller;
        fprintf(fout, csvformat ? "0x%X,%u,%u,%I64u,%I64u,%s\n" : \
            "%08X %7u %7u %9I64u %9I64u %s\n", address, entry->callin,
            entry->callout, entry->duration/733UI64,
            (entry->duration - entry->callouttime)/733UI64,
            lookupsymname(address));
    }
}

void capdump(CHAR* cap_filename, CHAR* dump_filename)
{
    // map the profile data file into memory
    VOID* fileview;
    DWORD filesize;
    CThreadProfileData* threads;

    fileview = mapfile(cap_filename, &filesize);
    checkerr(fileview == NULL, "Couldn't open profile data file\n", cap_filename);

    symtable = new CSymbolTable();

    // parse the profile data file
    __try {
        threads = capparse(fileview, filesize);
        UnmapViewOfFile(fileview);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        checkerr(TRUE, "Error reading profile data\n");
    }

    // dump the profile data file in text format
    FILE* fout = dump_filename ? fopen(dump_filename, "w") : stdout;
    checkerr(fout == NULL, "Can't open dump file %s\n", dump_filename);

    if (!summaryonly && !rawdump) {
        fprintf(fout, "Calls\tLevel\tFunction\tModule\tTime\tType\tWarnings\n");
    }

    UINT totalcalls = 0;
    UINT threadcnt = 0;
    CThreadProfileData* p;

    for (p=threads; p; p=p->NextThread()) {

        if (noidle && p->ThreadId() == 0) {
            continue;
        }

        if (summaryonly) {
            totalcalls += AddSummary(p);
        } else if (rawdump) {
            totalcalls += RawDump(fout, p);
        } else {
            totalcalls += OpfDump(fout, p);
        }

        threadcnt++;
    }

    if (summaryonly) {
        SummaryDump(fout);
    }

    if (!csvformat) {
        fprintf(fout, "\n");
        fprintf(fout, "MODULES:\n");

        SymbolFile* symfile = symfiles;

        for (UINT i=0; i < symcnt; i++, symfile++) {
            fprintf(fout, "#%d %s\n", i, symfile->name);
            fprintf(fout, "Path = %s\n", symfile->name);
            fprintf(fout, "Symbols = %s\n", symfile->symname);
            fprintf(fout, "Base Address = 0x%08X Size = 0x%X\n\n", symfile->baseaddr, symfile->size);
        }

        fprintf(fout, "INSTRUMENTATION:\n");
        fprintf(fout, "start   : %I64u\n", capstarttick/733UI64);
        fprintf(fout, "stop    : %I64u\n", capstoptick/733UI64);
        fprintf(fout, "duration: %I64u microseconds\n\n", (capstoptick-capstarttick)/733UI64);

        fprintf(fout, "STATISTICS:\n");
        fprintf(fout, "Total Threads = %d\n", threadcnt);
        fprintf(fout, "Maximum Concurrent Threads = %d\n", threadcnt);
        fprintf(fout, "Total Function Nodes = %d\n", totalcalls);
        fprintf(fout, "Total Function Calls = %d\n", totalcalls);
    }

    delete symtable;
    unloadsymfiles();

    if (fout != stdout) {
        fclose(fout);
    }

    while (threads) {
        p = threads;
        threads = p->NextThread();
        delete p;
    }
}

int __cdecl main(int argc, char* argv[])
{
    HRESULT hr;
    ULONG cmd = INVALIDCMD;
    char* cap_filename = NULL;
    char* dump_filename = NULL;

    ProgramName = strrchr(*argv, '\\');

    if (!ProgramName) {
        ProgramName = *argv;
    } else {
        ProgramName++;
    }

    argv++;
    argc--;

    while (argc--) {
        char* opt = *argv++;
        int params = 0;

        if (optequ(opt, "-x") && argc >= 1) {
            hr = DmSetXboxName(*argv);
            checkerr(FAILED(hr), "Error setting Xbox name: %x\n", hr);
            params += 1;
        } else if (optequ(opt, "start") && !cmd) {
            cmd = STARTCMD;
        } else if (optequ(opt, "stop") && !cmd && argc >= 1) {
            cmd = STOPCMD;
            cap_filename = *argv;
            params += 1;
        } else if (optequ(opt, "dump") && !cmd && argc >= 1) {
            cmd = DUMPCMD;
            cap_filename = argv[0];
            dump_filename = argc > 1 ? argv[1] : NULL;
            params += (dump_filename ? 2 : 1);
        } else if (optequ(opt, "-s") && !symbolpath && argc >= 1) {
            symbolpath = *argv;
            params += 1;
        } else if (optequ(opt, "-v")) {
            verbose = true;
        } else if (optequ(opt, "-idle")) {
            noidle = true;
        } else if (optequ(opt, "-opf")) {
            rawdump = false;
        } else if (optequ(opt, "-dpc")) {
            dumpdpc = true;
        } else if (optequ(opt, "-csv")) {
            csvformat = true;
        } else if (optequ(opt, "-sort") && argc >= 1) {
            for (int i=0; i<sizeof(sortkeytable)/sizeof(sortkeytable[0]); i++) {
                if (!_stricmp(sortkeytable[i].key, argv[0])) {
                    sortby = sortkeytable[i].sortkey;
                }
            }
            if (sortby == CFunctionTable::sortby_unknown) {
                usage();
            }
            params += 1;
        } else if (optequ(opt, "-summary")) {
            summaryonly = true;
        } else {
            usage();
        }

        argv += params;
        argc -= params;
    }

    // comma delimited format is valid only in summary report
    if (!summaryonly && (csvformat || sortby != CFunctionTable::sortby_unknown)) {
        usage();
    }

    switch (cmd) {
    case STARTCMD:
    case STOPCMD:
        DmUseSharedConnection(TRUE);
        hr = DmCAPControl(cmd == STARTCMD ? "start" : "stop");
        checkerr(FAILED(hr), "Error sending command to Xbox: %x\n", hr);

        if (cmd == STOPCMD) {
            hr = DmReceiveFile(cap_filename, XBOX_CAP_FILENAME);
            checkerr(FAILED(hr), "Error retrieving profile data: %x\n", hr);
        }

        DmUseSharedConnection(FALSE);
        break;

    case DUMPCMD:
        capdump(cap_filename, dump_filename);
        break;

    default:
        usage();
    }

    return 0;
}
