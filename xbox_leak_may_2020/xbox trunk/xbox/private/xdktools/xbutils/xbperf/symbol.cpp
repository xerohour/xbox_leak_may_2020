/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    symbol.cpp

Abstract:

    This module contains routines to look up debug symbols.

--*/

#include "stdafx.h"

UINT symcnt;
CSymbolTable* symtable;
char* symbolpath;
struct SymbolFile symfiles[XPROF_MAX_MODULES];

CSymbolTable::CSymbolTable()
{
    for (UINT i=0; i<SYMTABLE_SIZE; i++) {
        entries[i] = NULL;
    }
}

CSymbolTable::~CSymbolTable()
{
    for (UINT i=0; i<SYMTABLE_SIZE; i++) {
        SymEntry* symentry = entries[i];
        while (symentry) {
            VOID* p = symentry;
            symentry = symentry->next;
            free(p);
        }
    }
}

SymEntry* CSymbolTable::Lookup(ULONG address, bool bDisplacement)
{
    // simple function to map from an address to a hash bucket
    DWORD hash = address % SYMTABLE_SIZE;
    SymEntry* symentry = entries[hash];

    // return the symbol entry if the address is already in the symbol table
    while (symentry) {
        if (symentry->address == address) {
            return symentry;
        }
        symentry = symentry->next;
    }

    // look up the address in the symbol files and map it to a symbol name
    struct {
        IMAGEHLP_SYMBOL s;
        CHAR extrabuf[MAX_PATH+16];
    } syminfo;

    syminfo.s.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    syminfo.s.MaxNameLength = MAX_PATH;

    DWORD disp = 0;
    BOOL ok = SymGetSymFromAddr(GetCurrentProcess(), address, &disp, &syminfo.s);

    CHAR* name = syminfo.s.Name;

    if (ok) {
        if (disp && bDisplacement) {
            sprintf(name + strlen(name), "+0x%X", disp);
        }
    } else {
        sprintf(name, "0x%X", address);
    }

    // add the new address-to-symbol mapping to symbol table
    UINT n = strlen(name) + 1;
    symentry = (SymEntry*)malloc(offsetof(SymEntry, name) + n);

    if (!symentry) {
        return NULL;
    }

    symentry->next = entries[hash];
    entries[hash] = symentry;

    symentry->address = address;
    memcpy(symentry->name, name, n);

    // figure out which module the symbol belongs to
    symentry->module = 0;

    for (n=0; n<symcnt; n++) {
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

void loadsymfiles()
{
    BOOL ok = SymInitialize(GetCurrentProcess(), symbolpath, FALSE);
    checkerr(!ok, "Failed to initialize DBGHELP.DLL\n");

    SymSetOptions(SYMOPT_UNDNAME);
    SymbolFile* symfile = symfiles;

    for (UINT i=0; i<symcnt; i++, symfile++) {
        symfile->baseaddr = SymLoadModule(GetCurrentProcess(), NULL,
            symfile->name, NULL, symfile->loadaddr, symfile->size);

        if (symfile->baseaddr == 0) {
            VERBOSE("Warning: can't load symbols for image file %s (error code %d)\n", symfile->name, GetLastError());
            continue;
        }

        if (symfile->loadaddr != symfile->baseaddr) {
            VERBOSE("loadaddr %x != baseaddr %x?\n", symfile->loadaddr, symfile->baseaddr);
        }

        IMAGEHLP_MODULE modinfo;
        modinfo.SizeOfStruct = sizeof(modinfo);

        if (SymGetModuleInfo(GetCurrentProcess(), symfile->baseaddr, &modinfo)) {
            sprintf(symfile->symname, "%s", modinfo.LoadedImageName);
        }
    }
}

void unloadsymfiles()
{
    for (UINT i=0; i < symcnt; i++) {
        if (symfiles[i].baseaddr) {
            SymUnloadModule(GetCurrentProcess(), symfiles[i].baseaddr);
        }
    }
    SymCleanup(GetCurrentProcess());
}

