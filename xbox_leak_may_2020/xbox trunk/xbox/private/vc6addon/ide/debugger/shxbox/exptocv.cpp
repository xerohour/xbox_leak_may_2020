/*++


Copyright (c) 1997  Microsoft Corporation

Module Name:

    exptocv.c

Abstract:

    This module handles the conversion activities requires for converting
    Exports or COFF info to CODEVIEW debug data. Based on SymCvt then updated
	to use the ImageHlp APIs.

Author:

    Andy Pennell (apennell) 15-July-1997

Environment:

    Win32, User Mode

--*/

#include "shinc.hpp"
#pragma hdrstop
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//#include "cv.h"
#include "symcvt.h"
#include "cvcommon.h"

#include <imagehlp.h>

typedef struct tagENUMINFO {
	PIMAGE_DEBUG_INFORMATION	pDebugInfo;
	HANDLE				hProcess;
	IMAGEHLP_MODULE		IModule;
	DWORD				numsyms;
	DATASYM32*			dataSym;
	PPOINTERS			p;
	PIMAGE_NT_HEADERS	pNtHeader;
} ENUMINFO, *PENUMINFO;


DWORD  CreateModulesFromExps( PPOINTERS p, ENUMINFO * );
DWORD  CreatePublicsFromExps( PPOINTERS p, ENUMINFO * );
DWORD  CreateSegMapFromExps( PPOINTERS p, ENUMINFO * );


BOOL InitReadExports(char *fname, ENUMINFO *pEnumInfo, SHE *pShe)
{
    char       szDrive    [_MAX_DRIVE];
    char       szDir      [_MAX_DIR];
    char       szFname    [_MAX_FNAME];
    char       szExt      [_MAX_EXT];
    char       szPath     [MAX_PATH];
	PIMAGE_DEBUG_INFORMATION pDebugInfo;
	BOOL bSymInit = FALSE;
	BOOL bSymLoad = FALSE;
	IMAGEHLP_MODULE IModule;
	HANDLE hProcess = 0;
	DWORD dwBase;

	SymSetOptions( 0 );						// get them all as raw as possible

	// the search path is the location of the file *only*
    _splitpath( fname, szDrive, szDir, szFname, szExt );
	_makepath( szPath, szDrive, szDir, NULL, NULL );
	if (szPath[strlen(szPath)-1]=='\\')
		szPath[strlen(szPath)-1] = 0;			// remove traling \ else will fail sometimes

	pDebugInfo = MapDebugInformation( NULL, fname, szPath, 0 );
	if (pDebugInfo==NULL)
		return FALSE;

	bSymInit = SymInitialize( hProcess, szPath, FALSE );

	if (!bSymInit)
		goto exit;

	dwBase = (DWORD)pDebugInfo->MappedBase;

	bSymLoad = SymLoadModule( hProcess, NULL, fname, NULL, dwBase, 0L );

	if (!bSymLoad)
		goto exit;

	pEnumInfo->IModule.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

	if (!SymGetModuleInfo( hProcess, dwBase, &pEnumInfo->IModule ))
		goto exit;

	switch (pEnumInfo->IModule.SymType)
	{
		case SymExport:
			*pShe = sheExportsConverted;
			break;
		case SymCoff:
			*pShe = sheSymbolsConverted;
			break;
		case SymSym:
			// ImageHlp has yet to give us a SymSym value due to its broken
			// checking of the SYM header so we don't read those
			// assert(!"Got SYM file, cant do those");
			goto exit;
		case SymNone:
			// this happens on ODBCINT.DLL which is a resource-only DLL for example
			// assert(!"Got NO debug, shame");
			goto exit;
		default:
			goto exit;
	}

	pEnumInfo->numsyms = 0;

	pEnumInfo->pDebugInfo = pDebugInfo;
	pEnumInfo->hProcess = hProcess;
	pEnumInfo->pNtHeader = ImageNtHeader( pDebugInfo->MappedBase );

	return TRUE;

exit:
	if (bSymLoad)
		SymUnloadModule( hProcess, dwBase );

	if (bSymInit)
		SymCleanup( hProcess );

	if (pDebugInfo)
		UnmapDebugInformation( pDebugInfo );

	return FALSE;
}


LONG
GuardPageFilterFunction(
    DWORD                ec,
    LPEXCEPTION_POINTERS lpep
    )

/*++

Routine Description:

    This function catches all exceptions from the convertcofftocv function
    and all that it calls.  The purpose of this function is allocate memory
    when it is necessary.  This happens because the cofftocv conversion cannot
    estimate the memory requirements before the conversion takes place.  To
    handle this properly space in the virtual address space is reserved, the
    reservation amount is 10 times the image size.  The first page is commited
    and then the conversion is started.  When an access violation occurs and the
    page that is trying to be access has a protection of noaccess then the
    page is committed.  Any other exception is not handled.

Arguments:

    ec      - the ecxeption code (should be EXCEPTION_ACCESS_VIOLATION)
    lpep    - pointer to the exception record and context record


Return Value:

    EXCEPTION_CONTINUE_EXECUTION    - access violation handled
    EXCEPTION_EXECUTE_HANDLER       - unknown exception and is not handled

--*/

{
    LPVOID                      vaddr;
    SYSTEM_INFO                 si;
    MEMORY_BASIC_INFORMATION    mbi;


    if (ec == EXCEPTION_ACCESS_VIOLATION) {
        vaddr = (LPVOID)lpep->ExceptionRecord->ExceptionInformation[1];
        VirtualQuery( vaddr, &mbi, sizeof(mbi) );
        if (mbi.AllocationProtect == PAGE_NOACCESS) {
            GetSystemInfo( &si );
            VirtualAlloc( vaddr, si.dwPageSize, MEM_COMMIT, PAGE_READWRITE );
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

//  return EXCEPTION_CONTINUE_SEARCH;
    return EXCEPTION_EXECUTE_HANDLER;
}

BOOL
ConvertExpToCv( PPOINTERS p, char *fname, SHE *pSheResult )

/*++

Routine Description:

    This is the control function for the conversion of Exports to CODEVIEW
    debug data.  It calls individual functions for the conversion of
    specific types of debug data.


Arguments:

    fname        - name of component to read


Return Value:

    TRUE     - conversion succeded
    FALSE    - conversion failed

--*/

{
    ENUMINFO            enumInfo;
    DWORD               dwSize;
	BOOL				bResult = FALSE;

	// poor-mans MapInputFile
    memset( p, 0, sizeof(POINTERS) );
    strcpy( p->iptrs.szName, fname );

	if (!InitReadExports( fname, &enumInfo, pSheResult ))
		return FALSE;

	enumInfo.p = p;

	if (enumInfo.IModule.SymType==SymExport)
	    dwSize = enumInfo.pDebugInfo->ExportedNamesSize;
	else
		dwSize = enumInfo.pDebugInfo->SizeOfCoffSymbols;

	// take a guess at the size. Mostly it is good, but sometimes
	// it is very wrong eg when they are a lot of unnamed exports
	// (like shell32 which has over a 1000 of them)
	dwSize += enumInfo.IModule.NumSyms * (sizeof(DATASYM32)+16) + 1024;

	SYSTEM_INFO si;
	GetSystemInfo( &si );

    //
    // reserve all necessary pages
    //
    p->pCvCurr = p->pCvStart.ptr = (LPBYTE)VirtualAlloc( NULL, max ( enumInfo.pDebugInfo->SizeOfImage*10, si.dwPageSize*10 ), MEM_RESERVE, PAGE_NOACCESS );

    //
    // commit the expected pages
    //
    VirtualAlloc( p->pCvCurr, dwSize, MEM_COMMIT, PAGE_READWRITE );

    if (p->pCvStart.ptr)
	    __try {

        CreateSignature( p );
        CreatePublicsFromExps( p, &enumInfo );
        CreateSymbolHashTable( p );
        CreateAddressSortTable( p );
        CreateSegMapFromExps( p, &enumInfo );
        CreateModulesFromExps( p, &enumInfo );
        CreateDirectories( p );
		bResult = TRUE;

    } __except ( GuardPageFilterFunction( GetExceptionCode(), GetExceptionInformation() )) {

        VirtualFree( p->pCvStart.ptr, 0, MEM_RELEASE );
        p->pCvStart.ptr = NULL;
        bResult = FALSE;
    }

    if (bResult) {
        p->pCvCurr = (LPBYTE)LocalAlloc( NONZEROLPTR, p->pCvStart.size );
		if (p->pCvCurr)
	        CopyMemory( p->pCvCurr, p->pCvStart.ptr, p->pCvStart.size );
		else
			bResult = FALSE;
        VirtualFree( p->pCvStart.ptr, 0, MEM_RELEASE );
		if (0)
		{
			char szMsg[256];
			wsprintf(szMsg, "Guess=%08lx, Actual=%08lx, P=%08lx, %s\n", dwSize, p->pCvStart.size, p->pCvStart.ptr, fname );
			OutputDebugString(szMsg);
		}
        p->pCvStart.ptr = p->pCvCurr;
    }

	SymUnloadModule( enumInfo.hProcess, (DWORD)enumInfo.pDebugInfo->MappedBase );
	SymCleanup( enumInfo.hProcess );
	UnmapDebugInformation( enumInfo.pDebugInfo );

	return bResult;
}

// dont use ImageRVAToVa as it uses the wrong Size field
// 0 means error
DWORD FindSection( IMAGE_DEBUG_INFORMATION *pDebugInfo, DWORD *pOffset )
{
	DWORD Address = *pOffset;
	PIMAGE_SECTION_HEADER sh = pDebugInfo->Sections;
	DWORD NumSections = pDebugInfo->NumberOfSections;
    ULONG i;
    for (i=0; i<NumSections; i++)
	{
        if (Address >= sh[i].VirtualAddress &&
            Address <  (sh[i].VirtualAddress + sh[i].Misc.VirtualSize))
		{
			*pOffset -= sh[i].VirtualAddress;
			return i+1;
        }
    }
    return 0;
}


// callback used when enumerating symbols

BOOL EnumExportsToPublics( LPSTR szSymName, ULONG SymbolAddress, ULONG /*SymbolSize*/, PVOID UserContext )
{
	ENUMINFO *const pEnumInfo = (ENUMINFO *const)UserContext;
	IMAGE_DEBUG_INFORMATION * const pDebugInfo = pEnumInfo->pDebugInfo;

	if (SymbolAddress==0)
	{
		// some DLLs give us strange info (eg JSCRIPT.DLL)
		return TRUE;
	}

	// calculate section and offset
	DWORD dwOffset = SymbolAddress - (DWORD)pDebugInfo->MappedBase;
	if (dwOffset==0)
	{
		// some exports (eg a bunch in OLEAUT32) have an Rva of zero, which means we
		// will not find an owner section for them so bail now
		return TRUE;
	}

	DWORD nSection = FindSection( pDebugInfo, &dwOffset );

	if (nSection==0)
	{
		// the symbol wasn't found in a section, so ignore it
		assert(!"Symbol not in section");
		return TRUE;
	}
	assert( (nSection>0) && (nSection<=pDebugInfo->NumberOfSections) );

	// build a symbol record
	DATASYM32 *dataSym = pEnumInfo->dataSym;

    dataSym->seg = (WORD)nSection;			// 1-based
    dataSym->off = dwOffset;


    dataSym->rectyp = S_PUB32;
    dataSym->typind = 0;
    dataSym->name[0] = (BYTE)strlen( szSymName );
    strcpy( (char*)&dataSym->name[1], szSymName );

	DATASYM32 *dataSym2 = NextSym32( dataSym );
    dataSym->reclen = (USHORT) ((DWORD)dataSym2 - (DWORD)dataSym) - 2;
    dataSym = dataSym2;

	pEnumInfo->numsyms++;
	pEnumInfo->dataSym = dataSym;

	return TRUE;
}

DWORD
CreatePublicsFromExps( PPOINTERS p, ENUMINFO *pEnumInfo )
{
    OMFSymHash          *omfSymHash;

    omfSymHash = (OMFSymHash *) p->pCvCurr;
    pEnumInfo->dataSym = (DATASYM32 *) (PUCHAR)((DWORD)omfSymHash + sizeof(OMFSymHash));
	pEnumInfo->numsyms = 0;

	SymEnumerateSymbols( pEnumInfo->hProcess, (DWORD)pEnumInfo->pDebugInfo->MappedBase, EnumExportsToPublics, pEnumInfo );

    UpdatePtrs( p, &p->pCvPublics, (LPVOID)pEnumInfo->dataSym, pEnumInfo->numsyms );

    omfSymHash->cbSymbol = p->pCvPublics.size - sizeof(OMFSymHash);
    omfSymHash->symhash  = 0;
    omfSymHash->addrhash = 0;
    omfSymHash->cbHSym   = 0;
    omfSymHash->cbHAddr  = 0;

    return pEnumInfo->numsyms;
}


DWORD
CreateModulesFromExps( PPOINTERS p, ENUMINFO *pEnumInfo )

/*++

Routine Description:

    Creates the individual CV module records.  There is one CV module
    record.

Arguments:

    p        - pointer to a POINTERS structure (see cofftocv.h)


Return Value:

    The number of modules that were created.

--*/

{
    char                szDrive    [_MAX_DRIVE];
    char                szDir      [_MAX_DIR];
    char                szFname    [_MAX_FNAME];
    char                szExt      [_MAX_EXT];
    OMFModule           *m;
    char *              pb;

    _splitpath( p->iptrs.szName, szDrive, szDir, szFname, szExt );

    m = (OMFModule *) p->pCvCurr;

    m->ovlNumber        = 0;
    m->iLib             = 0;
    m->cSeg             = 1;
    m->Style[0]         = 'C';
    m->Style[1]         = 'V';
    m->SegInfo[0].Seg   = 1;
    m->SegInfo[0].pad   = 0;
    m->SegInfo[0].Off   = 0;
    m->SegInfo[0].cbSeg = pEnumInfo->pDebugInfo->SizeOfImage;

    pb = (char *) &m->SegInfo[1];
    sprintf( &pb[1], "%s.c", szFname );
    pb[0] = (char)strlen( &pb[1] );

    pb = (char *) NextMod(m);

    UpdatePtrs( p, &p->pCvModules, (LPVOID)pb, 1 );

    return 1;
}

DWORD
CreateSegMapFromExps( PPOINTERS p, ENUMINFO *pEnumInfo )

/*++

Routine Description:

    Creates the CV segment map.  The segment map is used by debuggers
    to aid in address lookups.  One segment is created for each 
    section in the image.

Arguments:

    p        - pointer to a POINTERS structure (see cofftocv.h)


Return Value:

    The number of segments in the map.

--*/
{
    DWORD                       i;
    SGM                         *sgm;
    SGI                         *sgi;
    PIMAGE_SECTION_HEADER       sh;
    ULONG                       align;
	PIMAGE_DEBUG_INFORMATION	dbg = pEnumInfo->pDebugInfo;


    sgm = (SGM *) p->pCvCurr;
    sgi = (SGI *) ((DWORD)p->pCvCurr + sizeof(SGM));

    sgm->cSeg = (WORD)dbg->NumberOfSections;
    sgm->cSegLog = (WORD)dbg->NumberOfSections;

    sh = dbg->Sections;

	align = pEnumInfo->pNtHeader ? pEnumInfo->pNtHeader->OptionalHeader.SectionAlignment : 4096;

    for (i=0; i<dbg->NumberOfSections; i++, sh++) {
        sgi->sgf.fRead        = (USHORT) ((sh->Characteristics & IMAGE_SCN_MEM_READ)    == IMAGE_SCN_MEM_READ);
        sgi->sgf.fWrite       = (USHORT) ((sh->Characteristics & IMAGE_SCN_MEM_WRITE)   == IMAGE_SCN_MEM_WRITE);
        sgi->sgf.fExecute     = (USHORT) ((sh->Characteristics & IMAGE_SCN_MEM_EXECUTE) == IMAGE_SCN_MEM_EXECUTE);
        sgi->sgf.f32Bit       = 1;
        sgi->sgf.fSel         = 1;
        sgi->sgf.fAbs         = 0;
        sgi->sgf.fGroup       = 0;
        sgi->iovl             = 0;
        sgi->igr              = 0;
        sgi->isgPhy           = (USHORT) i + 1;
        sgi->isegName         = -1;
        sgi->iclassName       = -1;
        sgi->doffseg          = 0;
        sgi->cbSeg            = ((sh->Misc.VirtualSize + (align-1)) & ~(align-1));
        sgi++;
    }

    UpdatePtrs( p, &p->pCvSegMap, (LPVOID)sgi, i );

    return i;
}


PUCHAR
ConvertSymbolsForImage2(
                       HANDLE      hFile,
                       char *      fname,
					   SHE *       pSheResult
    )
{
	POINTERS   p;
	PUCHAR     rVal;

	*pSheResult = sheNoSymbols;

	ConvertExpToCv( &p, fname, pSheResult );
	rVal = p.pCvStart.ptr;

	return rVal;
}
