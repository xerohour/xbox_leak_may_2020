/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    symbol.h

Abstract:

    This module contains routines to look up debug symbols.

--*/

#if !defined(AFX_SYMBOL_H__C08868ED_292D_49F8_9C84_1EAC03C9C1C9__INCLUDED_)
#define AFX_SYMBOL_H__C08868ED_292D_49F8_9C84_1EAC03C9C1C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// a simple hash table for maintaining recently used symbol information
struct SymEntry {
    SymEntry* next;
    UINT module;
    ULONG address;
    CHAR name[1];
};

struct SymbolFile {
    DWORD loadaddr;
    DWORD size;
    DWORD baseaddr;
    CHAR name[XPROF_MAX_MODULE_NAMELEN];
    CHAR symname[MAX_PATH+16];
};

class CSymbolTable {
public:
    SymEntry* Lookup(ULONG address, bool bDisplacement = false);
    CSymbolTable();
    virtual ~CSymbolTable();

private:

    // maximum symbol table size (should be a prime number)
    enum { SYMTABLE_SIZE = 4091 };

    SymEntry* entries[SYMTABLE_SIZE];
};

VOID loadsymfiles();
VOID unloadsymfiles();

extern CSymbolTable* symtable;
extern char* symbolpath;
extern UINT symcnt;
extern struct SymbolFile symfiles[XPROF_MAX_MODULES];

inline CHAR* lookupsymname(DWORD addr, bool bDisplacement = false)
{
    SymEntry* symentry = symtable->Lookup(addr);
    return symentry->name;
}

#endif // !defined(AFX_SYMBOL_H__C08868ED_292D_49F8_9C84_1EAC03C9C1C9__INCLUDED_)
