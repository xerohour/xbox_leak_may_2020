/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    capdump.cpp

Abstract:

    Dump binary profile data to human-readable text format

--*/

#include "xbcap.h"
#include <stddef.h>
#include <dbghelp.h>

typedef ULONG NTSTATUS;

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif

#ifndef _XBOX_ENABLE_PROFILING
#define _XBOX_ENABLE_PROFILING
#endif

#include "xprofp.h"

BOOL verbose = FALSE;
BOOL dumpdpc = FALSE;
BOOL rawdump = FALSE;
BOOL noidle = FALSE;
HANDLE process = NULL;
CHAR* symbolpath = NULL;
UINT symcnt;
struct SymbolFile {
    DWORD loadaddr;
    DWORD size;
    DWORD baseaddr;
    CHAR name[XPROF_MAX_MODULE_NAMELEN];
    CHAR symname[MAX_PATH+16];
} symfiles[XPROF_MAX_MODULES];

//
// Spew error debug message if verbose mode is enabled
//
#define VERBOSE !verbose ? (void)0 : errmsg


//
// A simple hash table for maintaining recently used symbol information
//
struct SymEntry {
    SymEntry* next;
    UINT module;
    DWORD address;
    CHAR name[1];
};

class SymbolTable {
public:
    SymbolTable() {
        for (UINT i=0; i < SYMTABLE_SIZE; i++) {
            entries[i] = NULL;
        }
    }

    ~SymbolTable() {
        for (UINT i=0; i < SYMTABLE_SIZE; i++) {
            SymEntry* symentry = entries[i];
            while (symentry) {
                VOID* p = symentry;
                symentry = symentry->next;
                free(p);
            }
        }
    }

    SymEntry* Lookup(DWORD address) {
        //
        // Simple function to map from an address to a hash bucket
        //
        DWORD hash = address % SYMTABLE_SIZE;
        SymEntry* symentry = entries[hash];

        //
        // If the address is already in the symbol table,
        // return the symbol entry now.
        //
        while (symentry) {
            if (symentry->address == address)
                return symentry;
            symentry = symentry->next;
        }

        //
        // Lookup the address in the symbol files
        // and map it to a symbol name
        //
        struct {
            IMAGEHLP_SYMBOL s;
            CHAR extrabuf[MAX_PATH+16];
        } syminfo;

        syminfo.s.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
        syminfo.s.MaxNameLength = MAX_PATH;

        DWORD disp = 0;
        BOOL ok = SymGetSymFromAddr(
                    process,
                    address,
                    &disp,
                    &syminfo.s);

        CHAR* name = syminfo.s.Name;

        if (ok) {
            // Address is successfully mapped to a symbol
            //  add displacement if needed
            if (disp) {
                sprintf(name + strlen(name), "+%x", disp);
            }
        } else {
            // Address cannot be mapped to a symbol
            // Just dump the hexdecimal string
            sprintf(name, "%x", address);
        }

        //
        // Add the new address-to-symbol mapping to symbol table
        //
        UINT n = strlen(name) + 1;
        symentry = (SymEntry*) malloc(offsetof(SymEntry, name) + n);
        checkerr(symentry == NULL, "Out of memory\n");

        symentry->next = entries[hash];
        entries[hash] = symentry;

        symentry->address = address;
        memcpy(symentry->name, name, n);

        //
        // Figure out which module the symbol belongs to
        //
        symentry->module = 0;
        for (n=0; n < symcnt; n++) {
            if (address >= symfiles[n].baseaddr &&
                address < symfiles[n].baseaddr + symfiles[n].size) {
                symentry->module = n;
                break;
            }
        }
        if (n == symcnt) {
            VERBOSE("Address not in range of any module: %x\n", address);
        }

        return symentry;
    }

private:
    // Maximum symbol table size (should be a prime number)
    enum { SYMTABLE_SIZE = 4091 };

    SymEntry* entries[SYMTABLE_SIZE];
};

SymbolTable* symtable;
inline CHAR* lookupsymname(DWORD addr) {
    SymEntry* symentry = symtable->Lookup(addr);
    return symentry->name;
}


//
// Map SYM_TYPE to a readable string
//
CHAR* symtypestr(SYM_TYPE symtype) {
    return (symtype == SymPdb) ? "PDB" :
           (symtype == SymCoff) ? "COFF" :
           (symtype == SymCv) ? "CodeView" :
           (symtype == SymExport) ? "Export" :
           (symtype == SymDeferred) ? "Deferred" :
           (symtype == SymSym) ? "SYM" : "NONE";
}

//
// Load / unload symbol files
//
VOID loadsymfiles()
{
    BOOL ok = SymInitialize(process, symbolpath, FALSE);
    checkerr(!ok, "Failed to initialize DBGHELP.DLL\n");

    SymSetOptions(SYMOPT_UNDNAME);

    SymbolFile* symfile = symfiles;
    for (UINT i=0; i < symcnt; i++, symfile++) {
        symfile->baseaddr = SymLoadModule(
                                    process,
                                    NULL,
                                    symfile->name,
                                    NULL,
                                    symfile->loadaddr,
                                    symfile->size);

        if (symfile->baseaddr == 0) {
            errmsg("Can't load symbols for image file %s: %d\n", symfile->name, GetLastError());
            continue;
        }

        if (symfile->loadaddr != symfile->baseaddr) {
            VERBOSE("Odd, loadaddr %x != baseaddr %x?\n", symfile->loadaddr, symfile->baseaddr);
        }

        IMAGEHLP_MODULE modinfo;
        modinfo.SizeOfStruct = sizeof(modinfo);
        if (SymGetModuleInfo(process, symfile->baseaddr, &modinfo)) {
            sprintf(symfile->symname,
                    "%s (%s)",
                    modinfo.LoadedImageName,
                    symtypestr(modinfo.SymType));
        }
    }
}

VOID
unloadsymfiles()
{
    for (UINT i=0; i < symcnt; i++) {
        if (symfiles[i].baseaddr) {
            SymUnloadModule(process, symfiles[i].baseaddr);
        }
    }
    SymCleanup(process);
}


//
// Function call-tree data specific to each thread
//
class ThreadData {
public:
    ThreadData(DWORD tid, ULONGLONG tick, BOOL dpcflag = FALSE);
    ~ThreadData();
    DWORD ThreadId() { return threadid; }

    UINT Dump(FILE* fout);
    UINT RawDump(FILE* fout);
    VOID CallBegin(ULONGLONG tick, DWORD caller, DWORD callee, DWORD calltype);
    VOID CallEnd(ULONGLONG tick, DWORD caller, DWORD calltype);
    VOID Comment(ULONGLONG tick, DWORD param);
    VOID Activate(ULONGLONG tick, INT action, DWORD newtid = 0);
    VOID PostProcessing(ULONGLONG firsttick, ULONGLONG lasttick);

    BOOL IsDpcThread() { return isdpc; }
    UINT CallLevel() { return calllevel; }
    VOID SetPriorThread(ThreadData* priorthread) { this->priorthread = priorthread; }

public:
    ThreadData* next;

private:
    // NOTE: Don't change the size of this structure
    struct DataEntry {
        ULONGLONG starttime;
        ULONGLONG duration;
        BYTE type;
        BYTE flags;
        WORD level;
        union {
            struct {            // for ENTRYTYPE_FUNCCALL
                DWORD caller;
                DWORD callee;
            };
            DWORD param;        // for ENTRYTYPE_COMMENT
            DWORD newtid;       // for ENTRYTYPE_THREADSWITCH
            DWORD filler[3];
        };
    };

    enum {
        ENTRYTYPE_FUNCCALL,
        ENTRYTYPE_COMMENT,
        ENTRYTYPE_THREADSWITCH,

        ENTRYFLAG_NOEND = 0x01,
        ENTRYFLAG_ORPHAN = 0x02,
        ENTRYFLAG_DPC = 0x04,
    };

    enum {
        MAX_CHUNKS = 1024,
        CHUNK_SIZE = 64*1024,
        ENTRY_SIZE = sizeof(DataEntry)
    };

    VOID* chunks[MAX_CHUNKS];
    UINT numchunks, freespace;
    DataEntry* nextentry;

    DataEntry* CreateNewEntry();

    ThreadData* priorthread;
    ULONGLONG starttime;
    ULONGLONG lastactivetime;
    ULONGLONG inactivetime;
    ULONGLONG timeadjust;
    DWORD threadid;
    BOOL isdpc;
    DataEntry* inactiveentry;
    UINT totalcalls;
    UINT percalloverhead;

    struct FunctionCall {
        DWORD caller;
        DWORD callee;
        DataEntry* entry;
        DWORD calltype;
        ULONGLONG timeadjust;
    };

    enum {
        MAX_CALL_LEVELS = 1024,

        // This number is obtained by timing
        // _CAP_Start_Profiling and _CAP_End_Profiling functions.
        PER_CALL_OVERHEAD = 59
    };

    FunctionCall callstack[MAX_CALL_LEVELS];
    UINT calllevel;

    BOOL IsActive() { return (inactiveentry == NULL); }
    BOOL InsideCall() { return (calllevel != 0); }
    FunctionCall* LastCall() { return &callstack[calllevel-1]; }
    VOID AdjustCallOverhead(ULONGLONG tick) {
        if (InsideCall()) {
            LastCall()->timeadjust += tick;
        } else {
            this->timeadjust += tick;
        }
    }

    static VOID DumpThreadName(FILE* fout, DWORD tid) {
        if (tid == 0xffffffff) {
            fprintf(fout, "[DPC]");
        } else if (tid == 0) {
            fprintf(fout, "[IDLE]");
        } else {
            fprintf(fout, "%x", tid);
        }
    }

    static VOID DumpIndent(FILE* fout, UINT level) {
        for (UINT i=0; i < level; i++) {
            fprintf(fout, "%-2d ", i);
        }
    }
};


//
// Initialize per-thread call-tree data
//
ThreadData::ThreadData(
    DWORD tid,
    ULONGLONG tick,
    BOOL dpcflag
    )
{
    next = NULL;
    threadid = tid;
    priorthread = NULL;
    starttime = lastactivetime = tick;
    inactivetime = timeadjust = 0;
    isdpc = dpcflag;
    inactiveentry = NULL;
    totalcalls = 0;
    percalloverhead = rawdump ? 0 : PER_CALL_OVERHEAD;

    freespace = numchunks = 0;
    memset(chunks, 0, sizeof(chunks));

    calllevel = 0;
    memset(callstack, 0, sizeof(callstack));
}


//
// Cleanup per-thread call-tree data
//
ThreadData::~ThreadData()
{
    for (UINT i=0; i < numchunks; i++) {
        VirtualFree(chunks[i], 0, MEM_RELEASE);
    }
}


//
// Allocate space for a new data entry
//
ThreadData::DataEntry*
ThreadData::CreateNewEntry()
{
    if (freespace < ENTRY_SIZE) {
        checkerr(numchunks >= MAX_CHUNKS, "Too much profile data.\n");

        // NOTE: memory returned by VirutalAlloc is already zeroed
        VOID* p = VirtualAlloc(NULL, CHUNK_SIZE, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
        checkerr(p == NULL, "Out of memory\n");

        chunks[numchunks++] = p;
        nextentry = (DataEntry*) p;
        freespace = CHUNK_SIZE;
    }

    freespace -= ENTRY_SIZE;
    return nextentry++;
}


//
// Process a CALL-BEGIN data entry
//
VOID
ThreadData::CallBegin(
    ULONGLONG tick,
    DWORD caller,
    DWORD callee,
    DWORD calltype
    )
{
    checkerr(calllevel >= MAX_CALL_LEVELS, "Too many levels of nested calls.\n");

    DataEntry* entry = CreateNewEntry();

    entry->starttime = tick;
    entry->type = ENTRYTYPE_FUNCCALL;
    entry->flags = ENTRYFLAG_NOEND;
    entry->level = (WORD)calllevel;

    if (caller == 0 && IsDpcThread()) {
        entry->flags |= ENTRYFLAG_DPC;
        caller = callee;
    } else if (InsideCall() && calltype == XPROFREC_FASTCAP_START && caller != LastCall()->callee) {
        entry->flags |= ENTRYFLAG_ORPHAN;
    }

    entry->caller = caller;
    entry->callee = calltype == XPROFREC_FASTCAP_START ? callee : 0;

    FunctionCall* call = &callstack[calllevel];
    call->caller = caller;
    call->callee = callee;
    call->calltype = calltype;
    call->entry = entry;
    call->timeadjust = percalloverhead;

    calllevel++;
}

//
// Process a Call-End data entry
//
VOID
ThreadData::CallEnd(
    ULONGLONG tick,
    DWORD caller,
    DWORD calltype
    )
{
    if (!InsideCall()) {
        VERBOSE("Unmatched Call-End record: %s\n", lookupsymname(caller));
        return;
    }

    FunctionCall* call = LastCall();
    calllevel--;

    DWORD ExpectedCallType = call->calltype;
    DWORD Match;

    switch (call->calltype) {
    case XPROFREC_FASTCAP_START:
        ExpectedCallType = XPROFREC_FASTCAP_END;
        Match = call->caller;
        break;

    case XPROFREC_CALLCAP_START:
        ExpectedCallType = XPROFREC_CALLCAP_END;
        Match = call->caller;
        break;

    case XPROFREC_PENTER:
        ExpectedCallType = XPROFREC_PEXIT;
        Match = call->callee;
        break;

    default:
        ExpectedCallType = 0;
        Match = 0;
    }

    if (ExpectedCallType != calltype || Match != caller) {
        errmsg(
            "Call-End record doesn't match the last Call-Begin:\n"
            "    thread: %x\n"
            "    actual %s\n"
            "    expect %s %s\n",
            threadid,
            lookupsymname(caller),
            lookupsymname(call->caller),
            lookupsymname(call->callee));
        return;
    }

    DataEntry* entry = call->entry;
    entry->duration = tick - entry->starttime - call->timeadjust;
    entry->flags &= ~ENTRYFLAG_NOEND;

    AdjustCallOverhead(call->timeadjust + percalloverhead);
}


//
// Process a Comment data entry
//
VOID
ThreadData::Comment(
    ULONGLONG tick,
    DWORD param
    )
{
    DataEntry* entry = CreateNewEntry();

    entry->starttime = tick;
    entry->type = ENTRYTYPE_COMMENT;
    entry->level = (WORD) calllevel;
    entry->param = param;
}


//
// Activate or deactivate the thread
//
#define THREAD_ACTIVE   1
#define THREAD_INACTIVE 0
#define THREAD_STOP     (-1)

VOID
ThreadData::Activate(
    ULONGLONG tick,
    INT action,
    DWORD newtid
    )
{
    if (action == THREAD_ACTIVE) {
        if (IsActive()) {
            VERBOSE("Thread is already active: %x!\n", threadid);
        } else {
            lastactivetime = tick;
            tick -= inactivetime;
            inactiveentry->duration = tick;
            inactiveentry = NULL;
            AdjustCallOverhead(tick);
        }
    } else {
        if (IsActive()) {
            inactivetime = tick;

            if (action == THREAD_INACTIVE) {
                DataEntry* entry = CreateNewEntry();
                entry->starttime = tick;
                entry->level = 0;
                entry->newtid = newtid;
                entry->type = ENTRYTYPE_THREADSWITCH;
                inactiveentry = entry;

                if (noidle && threadid == 0 && priorthread) {
                    // NOTE: We're attributing the time spent in idle thread
                    // to the last thread that was running.
                    priorthread->AdjustCallOverhead(lastactivetime-inactivetime);
                }
            }
        } else {
            VERBOSE("Thread is already inactive: %x!\n", threadid);
        }
    }
}


//
// Perform per-thread post-processing after
// we finish parsing the profile data file.
//
VOID
ThreadData::PostProcessing(
    ULONGLONG firsttick,
    ULONGLONG lasttick
    )
{
    // If this thread is still active, deactivate it
    if (IsActive()) {
        Activate(lasttick, THREAD_STOP, 0);
    }

    if (calllevel) {
        VERBOSE("CALL-BEGIN record without matching CALL-END: %x\n", threadid);
        while (calllevel) {
            FunctionCall* call = &callstack[--calllevel];
            VERBOSE("  %s => %s\n",
                lookupsymname(call->caller),
                lookupsymname(call->callee));

            // For incomplete call, forget about overhead adjustment
            call->timeadjust -= percalloverhead;

            DataEntry* entry = call->entry;
            entry->duration = inactivetime - entry->starttime - call->timeadjust;
            AdjustCallOverhead(call->timeadjust);
        }
    }

    // Count the total number of function calls
    ULONGLONG totalcalltime = 0;
    for (UINT i=0; i < numchunks; i++) {
        DataEntry* entry = (DataEntry*) chunks[i];
        UINT count = CHUNK_SIZE - ((i == numchunks-1) ? freespace : 0);

        for (count /= ENTRY_SIZE; count--; entry++) {
            if (entry->type == ENTRYTYPE_FUNCCALL) {
                if (entry->level == 0) totalcalltime += entry->duration;
                totalcalls++;
            }
            entry->starttime -= firsttick;
        }
    }

    if (IsDpcThread()) {
        timeadjust = inactivetime - starttime - totalcalltime;
    }
}


//
// Dump out per-thread call-tree data in .OPF format
//
UINT
ThreadData::Dump(
    FILE* fout
    )
{
    fprintf(fout, "0\t0\tThread ");
    DumpThreadName(fout, threadid);
    fprintf(fout, "\t0\t%I64u\t \t0\n", inactivetime - starttime - timeadjust);

    for (UINT i=0; i < numchunks; i++) {
        DataEntry* entry = (DataEntry*) chunks[i];
        UINT count = CHUNK_SIZE - ((i == numchunks-1) ? freespace : 0);

        for (count /= ENTRY_SIZE; count--; entry++) {
            if (entry->type != ENTRYTYPE_FUNCCALL) continue;

            SymEntry* symentry = symtable->Lookup(entry->callee ? entry->callee : entry->caller);

            fprintf(fout,
                "%d\t%d\t%s\t%d\t%I64u\t%c%s\n",
                1,
                entry->level+1,
                symentry->name,
                symentry->module,
                entry->duration,
                'F',
                (entry->flags & ENTRYFLAG_NOEND) ? "\tIncomplete call" :
                (entry->flags & ENTRYFLAG_ORPHAN) ? "\tCallback?" : "");
        }
    }

    return totalcalls;
}


//
// Dump out per-thread call tree data in raw text format
//
UINT
ThreadData::RawDump(
    FILE* fout
    )
{
    fprintf(fout, "Thread ");
    DumpThreadName(fout, threadid);
    fprintf(fout, " : time = %I64u\n", inactivetime - starttime - timeadjust);

    fprintf(fout, "%12s %12s   function\n", "start time", "duration");

    for (UINT i=0; i < numchunks; i++) {
        DataEntry* entry = (DataEntry*) chunks[i];
        UINT count = CHUNK_SIZE - ((i == numchunks-1) ? freespace : 0);

        for (count /= ENTRY_SIZE; count--; entry++) {
            if (entry->type == ENTRYTYPE_THREADSWITCH && IsDpcThread())
                continue;

            fprintf(fout, "%12I64u %12I64u ", entry->starttime, entry->duration);
            fprintf(fout, "%c ", (entry->flags & ENTRYFLAG_NOEND) ? '-' :
                     (entry->flags & ENTRYFLAG_ORPHAN) ? '?' : ' ');

            if (entry->type == ENTRYTYPE_FUNCCALL) {
                SymEntry* symentry;
                if (entry->level == 0) {
                    if (isdpc) {
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
                fprintf(fout, "comment %x\n", entry->param);
            } else {
                fprintf(fout, "*** unknown entry type: %x\n", entry->type);
            }
        }
    }

    fprintf(fout, "\n");
    return totalcalls;
}


//
// Parse profile data file and
// generate per-thread function call tree
//
ULONGLONG capstarttick, capstoptick;

ThreadData*
capparse(
    VOID* fileview,
    DWORD filesize
    )
{
    checkerr(
        filesize < XPROF_FILE_HEADER_SIZE ||
        (filesize & 3) != 0,
        "File size error: too small or not a multiple of 4\n");

    XProfpFileHeader* fileheader = (XProfpFileHeader*) fileview;
    if (fileheader->version != XPROF_FILE_VERSION) {
        errmsg("File version mismatch: %x != %x\n",
            fileheader->version,
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
    ThreadData* threads = NULL;
    ThreadData* curthread = NULL;
    ThreadData* dpcthread = NULL;
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
            //
            // Call-Begin record:
            //  params[0] = caller
            //  params[1] = callee
            //
            recsize = 4;
            if (count < recsize) goto badrec;
            checkerr(buf[2] == 0 || buf[3] == 0, "Invalid function address 0.\n");
            curthread->CallBegin(tick, buf[2], buf[3], rectype);
            break;

        case XPROFREC_CALLCAP_START:
            //
            // Call-Begin record:
            //  params[0] = caller
            //
            recsize = 3;
            if (count < recsize) goto badrec;
            checkerr(buf[2] == 0, "Invalid function address 0.\n");
            curthread->CallBegin(tick, buf[2], 0, rectype);
            break;

        case XPROFREC_FASTCAP_END:
        case XPROFREC_CALLCAP_END:
            //
            // Call-End record:
            //  params[0] = caller
            //
            recsize = 3;
            if (count < recsize) goto badrec;
            curthread->CallEnd(tick, buf[2], rectype);
            break;

        case XPROFREC_PEXIT:
            //
            // Call-End record:
            //  params[0] = caller
            //  params[1] = ESP
            //
            recsize = 4;
            if (count < recsize) goto badrec;
            curthread->CallEnd(tick, buf[3], rectype);
            break;

        case XPROFREC_CONTROL:
            //
            // Control record:
            //  params[0] = type
            //  params[1] = type-specific parameter
            //
            recsize = 4;
            if (count < recsize) goto badrec;

            switch (buf[2]) {
            case XPROF_START:
                //
                // Must be the first record of the profile data file
                //  params[1] = calling thread id
                //
                if (curthread) {
                    VERBOSE("Second START record treated as STOP.\n");
                    recsize = count;
                } else {
                    firsttick = tick;
                    curthread = threads = new ThreadData(buf[3], tick);
                    checkerr(curthread == NULL, "Out of memory\n");
                }
                break;

            case XPROF_STOP:
                //
                //  params[1] = calling thread id
                //
                if (count > recsize) {
                    VERBOSE("Ignore data after STOP record.\n");
                }
                recsize = count;
                break;

            case XPROF_COMMENT:
                //
                //  params[1] = user-defined parameter
                //
                curthread->Comment(tick, buf[3]);
                break;

            case XPROF_THREAD_SWITCH:
                //
                //  params[1] = new thread id
                //
                checkerr(curthread->IsDpcThread(),
                    "Thread switch in the middle of a DPC routine!\n");

                {
                    DWORD newtid = buf[3];

                    // Deactivate the current thread
                    if (newtid == curthread->ThreadId()) {
                        errmsg("Switching to the same thread: %x?!\n", newtid);
                    }
                    curthread->Activate(tick, THREAD_INACTIVE, newtid);

                    // Check if the new thread is already present
                    ThreadData* oldthread = curthread;
                    curthread = threads;
                    while (curthread && curthread->ThreadId() != newtid) {
                        curthread = curthread->next;
                    }

                    if (curthread) {
                        // Activate the new thread if already present
                        curthread->Activate(tick, THREAD_ACTIVE);
                    } else {
                        // Otherwise, create the new thread
                        curthread = new ThreadData(newtid, tick);
                        checkerr(curthread == NULL, "Out of memory\n");

                        curthread->next = threads;
                        threads = curthread;
                    }
                    curthread->SetPriorThread(oldthread);
                }
                break;

            case XPROF_DPC_BEGIN:
                //
                //  params[1] = DPC routine address
                //
                checkerr(curthread->IsDpcThread(), "Nested DPC call not expected!\n");
                curthread->Activate(tick, THREAD_INACTIVE, -1);

                if (dpcthread == NULL) {
                    dpcthread = new ThreadData(-1, tick, TRUE);
                    checkerr(dpcthread == NULL, "Out of memory\n");
                } else {
                    dpcthread->Activate(tick, THREAD_ACTIVE);
                }

                dpcthread->next = curthread;
                curthread = dpcthread;
                curthread->CallBegin(tick, 0, buf[3], rectype);
                break;

            case XPROF_DPC_END:
                //
                //  params[1] = DPC routine address
                //
                if (curthread == dpcthread) {
                    checkerr(dpcthread->CallLevel() != 1, "Unbalanced DPC call.\n");
                    dpcthread->CallEnd(tick, buf[3], rectype);

                    dpcthread->Activate(tick, THREAD_INACTIVE);
                    curthread = dpcthread->next;
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
            dpcthread->next = threads;
            threads = dpcthread;
        } else {
            delete dpcthread;
        }
    }

    // Revert the thread list and perform postprocessing
    curthread = threads;
    threads = NULL;
    while (curthread) {
        ThreadData* p = curthread;
        curthread = p->next;
        p->next = threads;
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


//
// Map a file into memory
//
VOID*
mapfile(
    CHAR* filename,
    DWORD* size
    )
{
    HANDLE file, filemap;
    VOID* fileview = NULL;

    file = CreateFile(
                filename,
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_SEQUENTIAL_SCAN,
                NULL);

    if (file == INVALID_HANDLE_VALUE)
        return NULL;

    *size = GetFileSize(file, NULL);
    if (*size == 0xFFFFFFFF) {
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


//
// Dump binary profile data to human-readable text format
//
VOID
capdump(
    CHAR* cap_filename,
    CHAR* dump_filename
    )
{
    process = GetCurrentProcess();

    // Map the profile data file into memory
    VOID* fileview;
    DWORD filesize;
    ThreadData* threads;

    fileview = mapfile(cap_filename, &filesize);
    checkerr(fileview == NULL,
        "Couldn't open profile data file\n",
        cap_filename);

    symtable = new SymbolTable();

    // Parse the profile data file
    __try {
        threads = capparse(fileview, filesize);
        UnmapViewOfFile(fileview);
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        checkerr(TRUE, "Error reading profile data\n");
    }

    // Dump the profile data file in text format
    FILE* fout = fopen(dump_filename, "w");
    checkerr(fout == NULL,
        "Can't open dump file %s\n",
        dump_filename);

    if (!rawdump) {
        fprintf(fout, "Calls\tLevel\tFunction\tModule\tTime\tType\tWarnings\n");
    }

    UINT totalcalls = 0;
    UINT threadcnt = 0;
    ThreadData* p;
    for (p = threads; p; p = p->next) {
        if (noidle && p->ThreadId() == 0) continue;
        UINT n = rawdump ? p->RawDump(fout) : p->Dump(fout);
        totalcalls += n;
        threadcnt++;
    }

    fprintf(fout, "\n");
    fprintf(fout, "MODULES:\n");

    SymbolFile* symfile = symfiles;
    for (UINT i=0; i < symcnt; i++, symfile++) {
        fprintf(fout, "#%d %s\n", i, symfile->name);
        fprintf(fout, "Path = %s\n", symfile->name);
        fprintf(fout, "Symbols = %s\n", symfile->symname);
        fprintf(fout, "Base Address = 0x%08x Size = 0x%x\n\n", symfile->baseaddr, symfile->size);
    }

    fprintf(fout, "INSTRUMENTATION:\n");
    fprintf(fout, "F = /fastcap 0 0\n");
    fprintf(fout, "start   : %12I64u\n", capstarttick);
    fprintf(fout, "stop    : %12I64u\n", capstoptick);
    fprintf(fout, "duration: %12I64u\n\n", capstoptick-capstarttick);

    fprintf(fout, "STATISTICS:\n");
    fprintf(fout, "Total Threads = %d\n", threadcnt);
    fprintf(fout, "Maximum Concurrent Threads = %d\n", threadcnt);
    fprintf(fout, "Total Function Nodes = %d\n", totalcalls);
    fprintf(fout, "Total Function Calls = %d\n", totalcalls);

    delete symtable;
    unloadsymfiles();
    fclose(fout);

    // Cleanup

    while (threads) {
        p = threads;
        threads = p->next;
        delete p;
    }
}

