/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    capdata.cpp

Abstract:

    This module implements routines to dump call-attributed profile (CAP)
    data for Xbox.

--*/

#include "stdafx.h"

CThreadProfileData::CThreadProfileData(DWORD tid, ULONGLONG tick, bool dpcflag) :
    m_nextthread(0), m_threadid(tid), m_prevthread(0),
    m_starttime(tick), m_lastactivetime(tick), m_inactivetime(0), m_timeadjust(0),
    m_totalcalls(0), m_calllevel(0),
    m_dataentry(0), m_usedentries(0), m_totalentries(0), m_inactiveentry(0),
    m_percalloverhead(PER_CALL_OVERHEAD), m_isdpc(dpcflag)
{
    memset(m_callstack, 0, sizeof(m_callstack));
}

CThreadProfileData::~CThreadProfileData()
{
    if (m_dataentry) {
        VirtualFree(m_dataentry, 0, MEM_RELEASE);
    }
}

CallDataEntry* CThreadProfileData::CreateNewEntry(size_t* index)
{
    // commit more memory for call data entry as needed
    if (m_usedentries == m_totalentries && !GrowDataEntryBy(8192)) {
        checkerr(true, "Too much profile data.\n");
        return NULL;
    }

    if (index) {
        *index = m_usedentries;
    }

    return &m_dataentry[m_usedentries++];
}

bool CThreadProfileData::GrowDataEntryBy(size_t elements)
{
    CallDataEntry* entry = NULL;
    size_t newsize = (elements + m_totalentries) * sizeof(CallDataEntry);

    // REVIEW: reserve 32MB (hard-coded) virtual address space for each thread
    if (!m_dataentry) {
        m_dataentry = (CallDataEntry*)VirtualAlloc(0, MAX_MEM_RESERVED, MEM_RESERVE, PAGE_NOACCESS);
    }

    // commit memory as needed
    if (newsize <= MAX_MEM_RESERVED) {
        entry = (CallDataEntry*)VirtualAlloc(m_dataentry, newsize, MEM_COMMIT, PAGE_READWRITE);
        if (entry) {
            m_totalentries += elements;
        }
    }

    return entry != NULL;
}

void CThreadProfileData::AdjustCallOverhead(ULONGLONG tick)
{
    if (InsideCall()) {
        LastCall()->timeadjust += tick;
    } else {
        m_timeadjust += tick;
    }
}

void CThreadProfileData::CallBegin(ULONGLONG tick, ULONG_PTR caller, ULONG_PTR callee, DWORD calltype)
{
    checkerr(m_calllevel >= MAX_CALL_LEVELS, "Too many levels of nested calls.\n");

    size_t index;
    CallDataEntry* entry = CreateNewEntry(&index);

    entry->starttime = tick;
    entry->type = ENTRYTYPE_FUNCCALL;
    entry->flags = ENTRYFLAG_NOEND;
    entry->level = (WORD)m_calllevel;
    entry->callin = 1;

    if (caller == 0 && IsDpcThread()) {
        entry->flags |= ENTRYFLAG_DPC;
        caller = callee;
    } else if (InsideCall()) {
        if (calltype == XPROFREC_FASTCAP_START && caller != LastCall()->callee) {
            entry->flags |= ENTRYFLAG_ORPHAN;
        }
        m_dataentry[m_callstack[m_calllevel-1].entryindex].callout++;
    }

    entry->caller = caller;
    entry->callee = calltype == XPROFREC_FASTCAP_START ? callee : 0;
    entry->calltype = calltype;

    FunctionCall* call = &m_callstack[m_calllevel];
    call->caller = caller;
    call->callee = callee;
    call->calltype = calltype;
    call->entryindex = index;
    call->timeadjust = m_percalloverhead;

    m_calllevel++;
}

void CThreadProfileData::CallEnd(ULONGLONG tick, DWORD caller, DWORD calltype)
{
    if (!InsideCall()) {
        VERBOSE("Unmatched Call-End record: %s\n", lookupsymname(caller));
        return;
    }

    FunctionCall* call = LastCall();
    m_calllevel--;

    _ASSERTE(call->calltype == m_dataentry[call->entryindex].calltype);

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
            "    thread: 0x%02X\n"
            "    actual %s\n"
            "    expect %s %s\n",
            m_threadid,
            lookupsymname(caller),
            lookupsymname(call->caller),
            lookupsymname(call->callee));
        return;
    }

    // calculate inclusive time spend in this function call
    CallDataEntry* entry = &m_dataentry[call->entryindex];
    entry->duration = tick - entry->starttime - call->timeadjust;
    entry->flags &= ~ENTRYFLAG_NOEND;

    // calculate time spend outside previous function call
    //   note the m_calllevel-- above
    if (InsideCall()) {
        m_dataentry[LastCall()->entryindex].callouttime += entry->duration;
    }

    AdjustCallOverhead(call->timeadjust + m_percalloverhead);
}

void CThreadProfileData::Comment(ULONGLONG tick, DWORD param)
{
    CallDataEntry* entry = CreateNewEntry();

    entry->starttime = tick;
    entry->type = ENTRYTYPE_COMMENT;
    entry->level = (WORD)m_calllevel;
    entry->param = param;
}

void CThreadProfileData::Activate(ULONGLONG tick, INT action, DWORD newtid)
{
    if (action == THREAD_ACTIVE) {
        if (IsActive()) {
            VERBOSE("Thread is already active: 0x%02X!\n", m_threadid);
        } else {
            m_lastactivetime = tick;
            tick -= m_inactivetime;
            m_inactiveentry->duration = tick;
            m_inactiveentry = NULL;
            AdjustCallOverhead(tick);
        }
    } else {
        if (IsActive()) {
            m_inactivetime = tick;

            if (action == THREAD_INACTIVE) {
                CallDataEntry* entry = CreateNewEntry();
                entry->starttime = tick;
                entry->level = 0;
                entry->newtid = newtid;
                entry->type = ENTRYTYPE_THREADSWITCH;
                m_inactiveentry = entry;

                if (noidle && m_threadid == 0 && m_prevthread) {
                    // note: we're attributing the time spent in idle thread
                    // to the last thread that was running.
                    m_prevthread->AdjustCallOverhead(m_lastactivetime-m_inactivetime);
                }
            }
        } else {
            VERBOSE("Thread is already inactive: 0x%02X!\n", m_threadid);
        }
    }
}

void CThreadProfileData::PostProcessing(ULONGLONG firsttick, ULONGLONG lasttick)
{
    // if this thread is still active, deactivate it
    if (IsActive()) {
        Activate(lasttick, THREAD_STOP, 0);
    }

    if (m_calllevel) {
        VERBOSE("CALL-BEGIN record without matching CALL-END: 0x%02X\n", m_threadid);
        while (m_calllevel) {
            FunctionCall* call = &m_callstack[--m_calllevel];
            VERBOSE("  %s - %s\n",
                lookupsymname(call->caller),
                lookupsymname(call->callee));

            // for incomplete call, forget about overhead adjustment
            call->timeadjust -= m_percalloverhead;

            CallDataEntry* entry = &m_dataentry[call->entryindex];
            entry->duration = m_inactivetime - entry->starttime - call->timeadjust;
            AdjustCallOverhead(call->timeadjust);
        }
    }

    // count the total number of function calls
    ULONGLONG totalcalltime = 0;
    CallDataEntry* entry = m_dataentry;

    for (size_t i=0; i<m_usedentries; i++, entry++) {
        if (entry->type == ENTRYTYPE_FUNCCALL) {
            if (entry->level == 0) totalcalltime += entry->duration;
            m_totalcalls++;
        }
        entry->starttime -= firsttick;
    }

    if (IsDpcThread()) {
        m_timeadjust = m_inactivetime - m_starttime - totalcalltime;
    }
}

const CallDataEntry* CThreadProfileData::GetCallDataEntry(size_t position) const
{
    if (position >= m_usedentries) {
        return NULL;
    }

    return &m_dataentry[position];
}

CFunctionTable::CFunctionTable() :
    m_dataentry(0), m_tablesize(0)
{
}

CFunctionTable::~CFunctionTable()
{
    CallDataEntry* entry;
    ULONG_PTR address;
    POSITION position = m_functions.GetStartPosition();

    while (position) {
        m_functions.GetNextAssoc(position, (void*&)address, (void*&)entry);
        delete entry;
    }

    if (m_dataentry) {
        LocalFree(m_dataentry);
    }
}

bool CFunctionTable::lookup(ULONG_PTR address, CallDataEntry*& entry) const
{
    return m_functions.Lookup((void*)address, (void*&)entry);
}

bool CFunctionTable::grow(int elements)
{
    CallDataEntry** entry;
    size_t newsize = (m_tablesize + elements) * sizeof(CallDataEntry*);

    if (m_dataentry) {
        entry = (CallDataEntry**)LocalReAlloc(m_dataentry, newsize, LPTR);
    } else {
        entry = (CallDataEntry**)LocalAlloc(LPTR, newsize);
    }

    if (entry) {
        m_dataentry = entry;
        m_tablesize += elements;
    }

    return entry != NULL;
}

bool CFunctionTable::add(const CallDataEntry* entry)
{
    CallDataEntry* e;
    ULONG_PTR address = entry->callee ? entry->callee : entry->caller;

    // update call entry data if the function already exists
    if (lookup(address, e)) {

        if (e->starttime > entry->starttime) {
            e->starttime = entry->starttime;
        }

        e->duration += entry->duration;
        e->callouttime += entry->callouttime;
        e->callin += entry->callin;
        e->callout += entry->callout;
        return true;
    }

    // create new call entry data and add to the function table
    e = new CallDataEntry;

    if (!e) {
        return false;
    }

    if (getcount() == m_tablesize && !grow(4096)) {
        delete e;
        e = NULL;
    } else {
        m_dataentry[getcount()] = e;
        CopyMemory(e, entry, sizeof(*entry));
        m_functions.SetAt((void*)address, e);
    }

    return e != NULL;
}

const CallDataEntry* CFunctionTable::getentry(int index) const
{
    if (index >= getcount()) {
        return NULL;
    }

    return m_dataentry[index];
}

int __cdecl CFunctionTable::_sortby_address(const CallDataEntry** e1, const CallDataEntry** e2)
{
    int c1 = (int)((*e1)->callee ? (*e1)->callee : (*e1)->caller);
    int c2 = (int)((*e2)->callee ? (*e2)->callee : (*e2)->caller);
    return c1 - c2;
}

int __cdecl CFunctionTable::_sortby_callin(const CallDataEntry** e1, const CallDataEntry** e2)
{
    return (int)(*e1)->callin - (int)(*e2)->callin;
}

int __cdecl CFunctionTable::_sortby_callout(const CallDataEntry** e1, const CallDataEntry** e2)
{
    return (int)(*e1)->callout - (int)(*e2)->callout;
}

int __cdecl CFunctionTable::_sortby_excltime(const CallDataEntry** e1, const CallDataEntry** e2)
{
    int et1 = (int)((*e1)->duration - (*e1)->callouttime);
    int et2 = (int)((*e2)->duration - (*e2)->callouttime);
    return et1 - et2;
}

int __cdecl CFunctionTable::_sortby_incltime(const CallDataEntry** e1, const CallDataEntry** e2)
{
    return (int)(*e1)->duration - (int)(*e2)->duration;
}

int __cdecl CFunctionTable::_sortby_name(const CallDataEntry** e1, const CallDataEntry** e2)
{
    ULONG_PTR c1 = (*e1)->callee ? (*e1)->callee : (*e1)->caller;
    ULONG_PTR c2 = (*e2)->callee ? (*e2)->callee : (*e2)->caller;
    return strcmp(lookupsymname(c1), lookupsymname(c2));
}

int (__cdecl *CFunctionTable::_sort_ftable[])(const CallDataEntry**, const CallDataEntry**) = {
    _sortby_address, _sortby_callin, _sortby_callout,
    _sortby_incltime, _sortby_excltime, _sortby_name
};

bool CFunctionTable::sort(sortkey key)
{
    if (getcount() == 0 || key >= sortby_unknown) {
        return false;
    }

    qsort(m_dataentry, getcount(), sizeof(CallDataEntry*),
          (int (__cdecl *)(const void*, const void*))_sort_ftable[key]);
    return true;
}
