/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    capdata.h

Abstract:

    This module implements routines to dump call-attributed profile (CAP)
    data for Xbox.

--*/

#if !defined(_CAPDATA_INCLUDED)
#define _CAPDATA_INCLUDED

#include "ptrmap.h"

#define THREAD_ACTIVE   1
#define THREAD_INACTIVE 0
#define THREAD_STOP     (-1)

struct CallDataEntry {
    ULONGLONG starttime;
    ULONGLONG duration;
    ULONGLONG callouttime;
    BYTE type;
    BYTE flags;
    WORD level;
    union {
        struct {            // for ENTRYTYPE_FUNCCALL
            DWORD caller;
            DWORD callee;
            DWORD calltype;
            DWORD callin;
            DWORD callout;
        };
        DWORD param;        // for ENTRYTYPE_COMMENT
        DWORD newtid;       // for ENTRYTYPE_THREADSWITCH
        DWORD filler[5];
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

class CThreadProfileData {
private:
    // need this to prevent the compiler from using default copy ctor
    CThreadProfileData(const CThreadProfileData&);
    CThreadProfileData& operator=(const CThreadProfileData& rhs);

public:
    CThreadProfileData(DWORD tid, ULONGLONG tick, bool dpcflag = false);
    virtual ~CThreadProfileData();

    void CallBegin(ULONGLONG tick, ULONG_PTR caller, ULONG_PTR callee, DWORD calltype);
    void CallEnd(ULONGLONG tick, ULONG_PTR caller, DWORD calltype);
    void Comment(ULONGLONG tick, DWORD param);
    void Activate(ULONGLONG tick, INT action, DWORD newid = 0);
    void PostProcessing(ULONGLONG firsttick, ULONGLONG lasttick);

    const CallDataEntry* GetCallDataEntry(size_t position) const;

    bool IsDpcThread() const { return m_isdpc; }
    UINT CallLevel() const { return m_calllevel; }
    UINT TotalCalls() const { return m_totalcalls; }
    DWORD ThreadId() const { return m_threadid; }
    ULONGLONG ThreadTime() const { return m_inactivetime - m_starttime - m_timeadjust; }

    CThreadProfileData* NextThread() const { return m_nextthread; }
    void SetPreviousThread(CThreadProfileData* prevthread) { m_prevthread = prevthread; }
    void SetNextThread(CThreadProfileData* nextthread) { m_nextthread = nextthread; }

private:

    struct FunctionCall {
        DWORD caller;
        DWORD callee;
        size_t entryindex;
        DWORD calltype;
        ULONGLONG timeadjust;
    };

    enum {
        MAX_CALL_LEVELS = 1024,
        MAX_MEM_RESERVED = 32 * 1024UL * 1024UL,
        PER_CALL_OVERHEAD = 59  // This number is obtained by timing
    };

    bool m_isdpc;
    UINT m_calllevel;
    DWORD m_threadid;
    CThreadProfileData* m_nextthread;
    CThreadProfileData* m_prevthread;
    FunctionCall m_callstack[MAX_CALL_LEVELS];

    ULONGLONG m_starttime;
    ULONGLONG m_lastactivetime;
    ULONGLONG m_inactivetime;
    ULONGLONG m_timeadjust;

    CallDataEntry* m_inactiveentry;

    size_t m_usedentries, m_totalentries;
    CallDataEntry* m_dataentry;
    CallDataEntry* CreateNewEntry(size_t* index = 0);
    bool GrowDataEntryBy(size_t elements);

    UINT m_totalcalls;
    UINT m_percalloverhead;

    bool IsActive() { return m_inactiveentry == NULL; }
    bool InsideCall() { return m_calllevel != 0; }
    bool NestedCall() { return m_calllevel > 1; }
    FunctionCall* LastCall() { return &m_callstack[m_calllevel-1]; }

    void AdjustCallOverhead(ULONGLONG tick);
};

class CFunctionTable {
public:
    CFunctionTable();
    virtual ~CFunctionTable();

    bool add(const CallDataEntry* entry);
    int getcount() const { return m_functions.GetCount(); }
    const CallDataEntry* getentry(int index) const;

    // simple sorting interface: must be zero-based index
    typedef enum {
        sortby_address = 0,
        sortby_callin,
        sortby_callout,
        sortby_incltime,
        sortby_excltime,
        sortby_name,
        sortby_unknown
    } sortkey;

    bool sort(sortkey key);

protected:
    bool grow(int elements);
    int gettablesize() const { return m_tablesize; }
    bool lookup(ULONG_PTR address, CallDataEntry*& entry) const;

private:
    int m_tablesize;
    CallDataEntry** m_dataentry;    // array of call data entry for fast sorting
    CMapPtrToPtr m_functions;       // simple hash for fast searching

    // sorting functions
    static int __cdecl _sortby_address(const CallDataEntry** e1, const CallDataEntry** e2);
    static int __cdecl _sortby_callin(const CallDataEntry** e1, const CallDataEntry** e2);
    static int __cdecl _sortby_callout(const CallDataEntry** e1, const CallDataEntry** e2);
    static int __cdecl _sortby_incltime(const CallDataEntry** e1, const CallDataEntry** e2);
    static int __cdecl _sortby_excltime(const CallDataEntry** e1, const CallDataEntry** e2);
    static int __cdecl _sortby_name(const CallDataEntry** e1, const CallDataEntry** e2);
    static int (__cdecl *_sort_ftable[])(const CallDataEntry**, const CallDataEntry**);
};

#endif // !defined(_CAPDATA_INCLUDED)
