/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: vulcanapi.h
*
* File Comments:
*
*
***********************************************************************/

#pragma once

#ifndef __VULCANAPI__
#define __VULCANAPI__

//
// Vulcan DLL version number.  Note that this constant was not in version
// 1.0, if it's defined at all then you've got at least version 1.01.
//
#define VULCANVER 0x200

#if     defined(VULCANDLL__)           //  Building the Vulcan DLL

#undef  __INLINE
#define __INLINE     inline

#undef  VULCANDLL
#define VULCANDLL    __declspec(dllexport)

#elif   defined(VULCANI__)             //  Building the Vulcan static library

#undef  __INLINE
#define __INLINE

#undef  VULCANDLL
#define VULCANDLL

#elif   defined(VULCANS__)             //  Building a client using the Vulcan static library

#undef  __INLINE
#define __INLINE     inline

#undef  VULCANDLL
#define VULCANDLL

#else                                  //  Building a Vulcan client

#undef  __INLINE
#define __INLINE     inline

#undef  VULCANDLL
#define VULCANDLL    __declspec(dllimport)

#if     !defined(BBTIRT)
#pragma comment(lib, "vulcan.lib")
#endif

#endif

#define VULCANCALL   __stdcall

typedef unsigned char    BYTE;
typedef unsigned short   WORD;
typedef unsigned long    DWORD;
typedef unsigned __int64 DWORDLONG;
typedef DWORDLONG        ADDR;
typedef unsigned __int64 VULHANDLE;

//UNDONE: remove this code once VC7 is required
#ifndef _INTPTR_T_DEFINED
#ifdef  _WIN64
typedef __int64    intptr_t;
#else
typedef int        intptr_t;
#endif
#define _INTPTR_T_DEFINED
#endif

// Classes defined in this file (vulcanapi.h)
class VSystem;
class VProg;
class VComp;
class VProc;
class VBlock;
class VInst;
class VProto;
class VProtoList;


// Classes defined in vulcanex.h
class VSrcInfo;
class VSrcIter;
class VSect;
class VReloc;
class VOperand;
class VAddress;
class VExport;
class VImport;
class VUserData;
struct VAddrRange;
class VAnnotation;
class VAnnotationIter;

// Classes defined in vulcancmdfile.h
class SZDICTIONARY;

// Classes defined in vulcanbmp.h
class CBmp;

// From <stdio.h>
struct _iobuf;                  // a FILE

#include <exception>
#include <stdarg.h>

#include "vulcanienum.h"
#include "vulcandlist.h"
#include "vulcankdlist.h"
#include "vulcanex.h"
#include "vulcanerr.h"

//   There are 4 important objects to understand for the Vulcan API.
//   Comps,Procs,Blocks,Insts:  are 4 types and represent the program structures.  They are queried or modified.
//   System and Prog are optional
//   [VSystem]
//       [VProg]
//           VComp
//               VProc
//                   VBlock
//                       VInst
//   The above is read to say that a VProg contains a list of VComps and VComps contain a list of VProcs and so on.

class VSystem
{
public:
    // Open a machine.  For now, szMachine should remain NULL
    static VULCANDLL VSystem * VULCANCALL Open( const char * szMachine = NULL, DWORD OpenFlags = 0);

    // Refresh the set of Progs.  This will only add new processes - the return is the number added
    virtual size_t RefreshProgs() = 0;

    // Get the first program on this system
    virtual VProg* FirstProg() = 0;

    // Get the current number of program on this system
    virtual size_t CountProg() = 0;

    // Free up this system component
    virtual void Destroy() = 0;
};

class VProg : public CDListElem_VProg
{
public:
    // Open a program.  This interface subsumes all functionality of the
    // other Open/OpenEx functions, via the OpenFlags parameter (made by
    // or'ing together values from enum OpenFlags, from vulcanienum.h).
    static VULCANDLL VProg* VULCANCALL Open(const char *DLLorEXEfileName, const char *CmdFileName, DWORD OpenFlags);

    // Open a program.  DoDllClosure will open all dlls that we can find
    // from this program.
    //
    // This API is obsolete and may be removed in a future version.  The
    // `OpenFlags' version of VProg::Open should be used instead.
    static VULCANDLL VProg* VULCANCALL Open( const char* DLLorEXEfileName, bool DoDllClosure = false, const char* CmdFileName = NULL);

    // Same as above but has an extra parameter to ignore missing CV info.
    // Please note that this is handy for experimental purposes but is not
    // safe in a production environment.  Please use this with care.
    //
    // This API is obsolete and may be removed in a future version.  The
    // `OpenFlags' version of VProg::Open should be used instead.
    static VULCANDLL VProg* VULCANCALL OpenEx( const char* DLLorEXEfileName, bool DoDllClosure = false, const char* CmdFileName = NULL, bool IgnoreCvError = false );

    // Open a program that is running.  -1 means the current process
    static VULCANDLL VProg* VULCANCALL OpenDynamic( DWORD idProcess, DWORD OpenFlags, const char *szMachine = NULL );

    // Open a program in dynamic mode by launching it.
    static VULCANDLL VProg* VULCANCALL OpenDynamic( const char *szCmdLine, DWORD OpenFlags, const char *szMachine = NULL);

    // Get an iterator for the components of this program
    virtual VComp* FirstComp() = 0;
    virtual VComp* LastComp() = 0;

    // Add to first/last
    virtual void InsertFirstComp(VComp*) = 0;
    virtual void InsertLastComp(VComp*) = 0;

    // Get the name of the program
    virtual const char* Name() = 0;

    // Delete this program
    virtual void Destroy() = 0;

    // How many components in the program?
    virtual size_t CountComps() = 0;

    // Get the next prog in the system (returns NULL if you opened a Prog)
    virtual VProg *Next() = 0;

    // Get the parent system (only works if you started with one)
    virtual VSystem *ParentSystem() = 0;

    // Get the process handle/id of the attached process (NULL for none)
    virtual VULHANDLE ProcessHandle() = 0;
    virtual DWORD  ProcessId() = 0;

    // Check whether this program is attached to a live process (false if GetProcessHandle() returns NULL)
    virtual bool IsProcessLive() = 0;

    // Allocate memory in the remote process, returning the address.
    // This memory will be read/write/execute
    virtual ADDR AllocateMemory(size_t cb) = 0;

    // Write memory in the process (this may fail... especially if the process has exited)
    virtual bool WriteMemory( ADDR addr, size_t cb, void *pv ) = 0;

    // Read memory in the process
    virtual bool ReadMemory( ADDR addr, size_t cb, void *pv ) = 0;

    // Stop/Restart the live process (this is reference counted!)
    virtual void Suspend() = 0;
    virtual void Resume() = 0;

    // Get/set the user data for this program
    inline void SetUserData(void*, VUserData*);
    inline void* GetUserData(VUserData*);
};

class VComp : public CDListElem_VComp
{
public:
    // Open a component.  `flags' is bits from OpenFlags, from vulcanienum.h.
    static VULCANDLL VComp* VULCANCALL Open(char *DLLorEXEfileName, DWORD flags = 0);

    // Gets the level used to open the image.  See OpenFlags
    virtual void GetOpenLevel(DWORD *pdwOpenFlags) = 0;

    // Get the program that this component is a member of (may return NULL)
    virtual VProg* ParentProg() = 0;

    // Get the first/last/prev/next component (returns NULL for none)
    virtual VComp* Next() = 0;
    virtual VComp* Prev() = 0;
    virtual VComp* First() = 0;
    virtual VComp* Last() = 0;

    // Insert a component before/after this component
    virtual void InsertPrev(VComp*) = 0;
    virtual void InsertNext(VComp*) = 0;

    // Remove this component from the program
    virtual void Remove() = 0;

    // Get the first/last code procedure in this component
    virtual VProc* FirstProc() = 0;
    virtual VProc* LastProc() = 0;

    // Get the first/last procedure in this component (either code or data)
    virtual VProc* FirstAllProc() = 0;
    virtual VProc* LastAllProc() = 0;

    // Add the first/last proc to the component
    virtual void InsertFirstProc(VProc*) = 0;
    virtual void InsertLastProc(VProc*) = 0;

    // How many procs in the comp
    virtual size_t CountProcs() = 0;

    // How many procs (code or data) in this section
    virtual size_t CountAllProcs() = 0;

    // How many sections in the comp
    virtual size_t CountSects() = 0;

    enum ECompType{
            CompNotUsed=0,
            PE,
            PE64,
            PECOM,
    };


    // Get the type of the input component
    virtual VComp::ECompType InputType() = 0;

    // Get the type of the output component
    virtual VComp::ECompType OutputType() = 0;

    // Get the input component name
    virtual const char* InputName() = 0;

    // Set command file (and dictionary): move to PE-specific?
    virtual void SetCommandFile(const char* szFile, SZDICTIONARY* pszdictCmd = NULL) = 0;


    // Is the PDB filename going to be output?
    virtual bool CanWritePdb() = 0;

    // Is the binary emitted by BBT tools?
    virtual bool IsEmittedByBBT() = 0;

    // Is the binary emitted by BBT tools?
    virtual bool IsEmittedByVulcan() = 0;

    // Is the binary emitted by BBINSTR?
    virtual bool IsInstrumentedByBBT() = 0;

    // Is this Vulcan emitted component be parsed again?
    virtual bool IsRereadable() = 0;

    // Get/set the timestamp of this component
    virtual DWORD TimeStamp() = 0;

    // Reads the exe/dll and creates IR (if not already created)
    virtual void Build() = 0;

    // Writes this component to outputname (defaults to foo.vulcan.exe, foo.vulcan.pdb, none, true )
    virtual void Write(const char *szOutputName = NULL, const char *szOutputPdbName = NULL, const char *szMapFile = NULL, bool fRereadable = false) = 0;
    virtual void Write(DWORD WriteFlags, const char *szOutputName, const char *szOutputPdbName, const char *szMapFile) = 0;

    // Same as Write above only MUCH faster.  Does not compact the exe.  Does not check Intel Errata 42 (a problem for a VERY small number of Intel chips).
    virtual void WriteFast(const char *szOutputName = NULL, const char *szOutputPdbName = NULL, const char *szMapFile = NULL, bool fRereadable = false) = 0;
    virtual void WriteFast(DWORD WriteFlags, const char *szOutputName = NULL, const char *szOutputPdbName = NULL, const char *szMapFile = NULL) = 0;

    // dump the printed VULCAN instructions.
    virtual void Print( const char *fileName ) = 0;

    // dump out the asmemit just like dmpdb /asmemit output.  Use only after calling write.
    virtual void PrintAsm(const char* fileName) = 0;

    // Get the primary entry point for this component (main, WinMain, DllMain...)
    virtual VBlock* EntryBlock() = 0;

    // Set the primary entry point for this component (you should call the old entry point yourself from p)
    virtual void SetEntryBlock( VBlock* pBlk ) = 0;

    // Get the block corresponding to the image header
    virtual VBlock* ImageHeaderBlock() = 0;

    // Create a new, empty procedure in this component (inserts at beginning of first code section)
    virtual VProc* NewProc( const char* name ) = 0;
    virtual VProc* NewProc() = 0;

    // Adds an import.  Block returned can be used to as call target to call new import
    virtual VBlock* CreateImport(const char* dll,const char* func, bool = false) = 0;

    // Get the first/last section for this component
    virtual VSect* FirstSect() = 0;
    virtual VSect* LastSect() = 0;

    // Create a new section (flags are ors of SectionFlags)
    virtual VSect* NewSect( const char* name, DWORD flags ) = 0;

    // Gives the block corresponding to a (public) mangled name - pib is out for offset into block
    virtual VBlock *BlockFromSymName(const char *sz, DWORD *pib = NULL) = 0;

    // Get the imports for this component
    virtual VImport* FirstImport() = 0;
    virtual size_t   CountImports() = 0;
    virtual VImport* FindImport(const char *dllName, const char *procName) = 0;
    virtual VImport* FindImport(const char *dllName, DWORD ordinal) = 0;

    // Merge this component with another component.  The first argument
    // specifies the string name of the component you want to merge with.
    // The second argument is a string that specifies the routine that
    // would be executed when the app is terminated.  This is needed mostly
    // for the case where a DLL is merged into an EXE and there is code in
    // DllMain to do some work when the DLL receives a PROCESS_DETACH
    // message.
    virtual void MergeIR(const char *, const char * = NULL) = 0;

    // Get the exports for this component
    virtual VExport* FirstExport() = 0;
    virtual size_t   CountExports() = 0;
    virtual VExport* FindExport(const char *exportName) = 0;
    virtual VExport* FindExport(DWORD ordinal) = 0;

    // Turn on block follower mode
    virtual void UseBlockFollowers() = 0;

    // Redirect an import/external procedure to another import/external procedure
    virtual bool RedirectImport(const char* DllFrom, const char* nameFrom, const char* DllTo, const char* NameTo) = 0;
    virtual bool RedirectImport(VImport * pimportFrom, const char* DllTo, const char* NameTo) = 0;

    // Redirect an import/external procedure to a procedure.
    // The procedure can be created by CreateImportThunk.
    virtual bool RedirectImport(const char* DllFrom, const char* nameFrom, VProc *pprocTo) = 0;
    virtual bool RedirectImport(VImport * pimportFrom, VProc *pprocTo) = 0;

    // Redirect an export to a block
    virtual VBlock * RedirectExport(const char* nameFrom, VBlock *pblkTo) = 0;
    virtual VBlock * RedirectExport(VExport * pexportFrom, VBlock *pblkTo) = 0;

    // Redirect a local procedure to another local procedure
    // Place a jump at the beginning of this procedure which goes to another address.
    // The returned block will be the entry point for the old routine.
    virtual VBlock * RedirectProc(const char* procName, VProc *pprocTo) = 0;
    virtual VBlock * RedirectProc(VProc * pprocFrom, VProc *pprocTo) = 0;

    // Redirect a local procedure to an import
    // Place a jump at the beginning of this procedure which goes to another address.
    // The returned block will be the entry point for the old routine.
    virtual VBlock * RedirectProc(const char* procName, const char* szDllTo, const char* szFuncTo) = 0;
    virtual VBlock * RedirectProc(VProc *pprocFrom, const char* szDllTo, const char* szFuncTo) = 0;

    // Delete this component
    virtual void Destroy() = 0;

    // Get/set the user data for this comp
    inline void  SetUserData(void*, VUserData*);
    inline void* GetUserData(VUserData*);

    // Debugging support (Print the components name and return it)
    virtual const char * DbgPrint() = 0;

    // Returns iterator over all the symbols for this component.
    virtual VSymIter * AllSymbols() = 0;   // Call Destroy() after use.

    // IF argument is NULL, Return the number of address ranges of asm code to keep together.
    // ELSE fill given array with address ranges.
    //
    virtual size_t GetAddrrangeAsmCodeKeepTogether(VAddrRange []=0) = 0;

    // IF argument is NULL, Return the number of address ranges to keep together.
    // ELSE fill given array with address ranges.
    //
    virtual size_t GetAddrrangeKeepTogether(VAddrRange []=0) = 0;

    // Does this VComp represent a "dll"
    virtual bool IsDll() = 0;

    // Does this VComp represent a "driver"
    virtual bool IsDriver() = 0;

    // Get the version for this component from (resources)
    // 3.75.0.31 would be encoded as 0x00030075 0x00000031
    virtual bool FGetVersion( DWORD *pdwFileMS, DWORD *pdwFileLS, DWORD *pdwProdMS, DWORD *pdwProdLS) = 0;

    // Get the checksum for this component (this is not always set)
    virtual DWORD Checksum() = 0;

    // Create a new export.
    // IF this component previously had exports, the existing export module name is used.
    // ELSE the output name of the component is used.
    // It is the caller's responsiblity to ensure that there are no name and/or ordinal conflicts.
    virtual VExport *CreateExport(const char *szName, ORD16 ordinal, VBlock *) = 0;

    // Create a forwarder export.
    virtual VExport *CreateExport(const char *szName, ORD16 ordinal, const char *szDest) = 0;

    // Create Alternate path to PDB
    virtual void SetPdbAlternatePath(const char *szPath) = 0;

    // Create stripped PDB when linking
    virtual void SetPdbStrippedPath(const char *) = 0;

    // Return an iterator that can be used to find annotations in the component.
    virtual VAnnotationIter *AnnotationIter(const char * = NULL) = 0;

    // Return the VProc that "contains" the specified address (i.e. the
    // address is >= the VProc's Addr(), and < the VProc's "Addr() +
    // Size()".  This function is not terribly efficient: it simply
    // iterates over all the VProcs in the VComp testing their addresses
    // and sizes.  Returns NULL if no matching VProc is found.
    virtual VProc *FindProc(ADDR addr) = 0;

    // Set this as the "current working" component - this will be a default
    virtual void SetCurrentWorking() = 0;

    // Create import thunk creates a procedure which jumps to the import.
    // Use this with VComp::RedirectProc or VComp::RedirectImport.
    virtual VProc* CreateImportThunk(const char* dll,const char* func,bool = false) = 0;

    // Mark all imports from a module to be delayed load or regular imports depending
    // on the specified flag.  If it is true, all imports entries from this module
    // will be delay-load.  If it is false, they will all turned into regular imports.
    virtual void SetImportModuleForDelayLoad(const char *, bool fDelayLoad = true) = 0;

    // Redirect all references to redirected imports to go to the new import blocks
    virtual void ProcessRedirectedImportTargets() = 0;

    // For dynamic mode, identify the path to the local binary so that we don't have to
    // download the remote binary to build the IR.
    virtual void DynamicSetLocalModuleName( const char *szLocalImage ) = 0;
};

class VProc : public CDListElem_VProc
{
public:
    // Create a new proc (not inserted automatically - the name becomes a sym in pComp )
    static VULCANDLL VProc* VULCANCALL Create(VComp* pComp, const char* name = NULL);

    // Get the component that this procedure is in
    virtual VComp* ParentComp() = 0;

    // Returns the next/prev/first/last code procedure in this component
    virtual VProc* Next() = 0;
    virtual VProc* Prev() = 0;
    virtual VProc* First() = 0;
    virtual VProc* Last() = 0;

    // Returns the next/prev/first/last procedure in this component
    virtual VProc* NextAll() = 0;
    virtual VProc* PrevAll() = 0;
    virtual VProc* FirstAll() = 0;
    virtual VProc* LastAll() = 0;

    // Returns the next/prev/first/last code procedure in this _section_
    virtual VProc* NextSectProc() = 0;
    virtual VProc* PrevSectProc() = 0;
    virtual VProc* FirstSectProc() = 0;
    virtual VProc* LastSectProc() = 0;

    // Returns the next/prev/first/last procedure in this _section_
    virtual VProc* NextAllSectProc() = 0;
    virtual VProc* PrevAllSectProc() = 0;
    virtual VProc* FirstAllSectProc() = 0;
    virtual VProc* LastAllSectProc() = 0;

    // Add a proc before/after this proc
    virtual void InsertNext(VProc*) = 0;
    virtual void InsertPrev(VProc*) = 0;

    // Remove this proc from the component (without destroying it)
    virtual void Remove() = 0;

    // Returns the first live code block in the proc
    virtual VBlock* FirstBlock() = 0;
    virtual VBlock* LastBlock() = 0;
    virtual size_t  CountBlocks() = 0;

    // Returns the first block in the proc (code, data, or unreachable)
    virtual VBlock* FirstAllBlock() = 0;
    virtual VBlock* LastAllBlock() = 0;
    virtual size_t  CountAllBlocks() = 0;

    // Add a block to the first/last of the proc
    virtual void InsertFirstBlock(VBlock*) = 0;
    virtual void InsertLastBlock(VBlock*) = 0;

    // Get the friendly name of this procedure
    virtual const char* Name() = 0;

    // Set the friendly name of this procedure - this only affects the name in Vulcan
    virtual void SetName( const char* name, VComp *pComp ) = 0;


    // Get the unique name of this procedure into szbuf, returns required size when szbuf is NULL
    virtual size_t SymName(char *buf, size_t bufsize, VComp *pComp) = 0;

    // Get the address of this procedure
    virtual ADDR Addr() = 0;

    // Get the size of this procedure
    virtual size_t Size() = 0;

    // Get a pointer to the raw bytes of the procedure
    virtual const void* Raw(VComp *pComp) = 0;

    // Reverse the order of the blocks in this procedure
    virtual void Reverse() = 0;

    // Fills a buffer with the printed VULCAN instruction in this proc (if the return is > than sizeBuf, the it failed)
    virtual size_t Print( char* szBuf, size_t sizeBuf ) = 0;

    // Fills a buffer with the printed DISASM instruction in this proc (if the return is > than sizeBuf, the it failed)
    virtual size_t PrintAsm( char* szBuf, size_t sizeBuf, VComp * ) = 0;

    // Is this procedure exported
    virtual bool IsExported(VComp *pComp) = 0;

    // Get an iterator for the exports of the procedure
    virtual VExport* FirstExport(VComp *pComp) = 0;

    // Get the section that this proc is in
    virtual VSect* ParentSection() = 0;

    // Returns an iterator for the source information of this procedure
    virtual VSrcIter FirstSrc(VComp *pComp) = 0;

    // Is this a thunk?
    virtual bool IsThunk() = 0;

    // Is this an import thunk? Call FirstBlock()->ImportTarget() to find out where it goes.
    virtual bool IsImportThunk() = 0;

    // Delete this Proc
    virtual void Destroy() = 0;

    // Was this proc generated from Masm?
    virtual bool IsMasm() = 0;

    // Does this proc not return
    virtual bool IsNoReturn() = 0;

    // Was this proc created for reasons other than a CodeView CvProc symbol?
    virtual bool IsPseudoProc() = 0;

    // Does this proc register an expection?
    virtual bool HasExcept(VComp *pComp) = 0;

    // Get/set the user data for this proc
    inline void SetUserData(void*, VUserData*);
    inline void* GetUserData(VUserData*);

    // Debugging support (Print the proc's name, addr and return it)
    virtual const char * DbgPrint() = 0;

    // Returns iterator over all the symbols for this proc start address.
    virtual VSymIter   * Symbols(VComp *) = 0; // Call Destroy() after use.

    enum CCPROC {ccUnknown, ccCdecl, ccFastcall, ccStdcall, ccThiscall};
    virtual CCPROC CallConv(VComp *) = 0;

    // Function Parameter Enumeration
    virtual VSymbol *FirstParam(VComp *) = 0;

    // Function Local Enumeration
    virtual VSymbol *FirstLocal(VComp *) = 0;

    // Function return type
    virtual VSymbol *ReturnSymbol(VComp *) = 0;

    // Get the complete function prototype as a string
    virtual size_t Prototype(char *szBuf, size_t cchBuf, VComp * ) = 0;

    // Check if this proc user's flag is set
    virtual bool IsUserFlagSet() = 0;
    virtual void SetUserFlag() = 0;
    virtual void ResetUserFlag() = 0;

    // Return the VBlock that "contains" the specified address (i.e. the
    // address is >= the VBlock's Addr(), and < the VBlock's "Addr() +
    // Size()".  This function is not terribly efficient: it simply
    // iterates over all the VBlocks in the VProc testing their addresses
    // and sizes.  Returns NULL if no matching VBlock is found.
    virtual VBlock *FindBlock( ADDR addr ) = 0;

    // Get the Entry block (This is often the first block except after re-ordering)
    virtual VBlock *EntryBlock() = 0;

    // Write this procedure to memory (dynamic mode only)
    virtual ADDR Commit(VComp *pComp) = 0;

    // Get the original address of this procedure (shortcut for EntryBlock()->OrigAddr(pComp) )
    virtual ADDR OrigAddr( VComp *pComp ) = 0;

    // Create a copy of this procedure.  In dynamic mode, this new routine can be modified and then commited.
    // The optional VUserData provides a map from the old blocks to the new blocks and back.
    virtual VProc *Copy( VComp *pComp, VUserData *pData = NULL ) = 0;

    // Emit this procedure and convert it into a single data block with relocs
    // Be careful because anyone calling a block other than the entry in this routine
    // will now fail to link.
    virtual VBlock *ConvertProcToDataBlock( VComp *pComp, ADDR addrStart ) = 0;
};


class VBlock : public CDListElem_VBlock
{
public:
    // Returns the procedure that this block is in
    VULCANDLL VProc* ParentProc();

    // Get the next/prev code block in the procedure
    inline VBlock* Next();
    inline VBlock* Prev();
    inline VBlock* First();
    inline VBlock* Last();

    // Get the next/prev block in the procedure
    inline VBlock* NextAll();
    inline VBlock* PrevAll();
    inline VBlock* FirstAll();
    inline VBlock* LastAll();

    // Remove this block from the proc
    inline void Remove();

    // Insert a block before/after this block
    inline void InsertPrev(VBlock*);
    inline void InsertNext(VBlock*);

    // Get an the first/last instruction in this block
    VULCANDLL VInst* FirstInst();
    VULCANDLL VInst* LastInst();
    VULCANDLL size_t CountInsts();

    // Call this once you're done with all the insts.  Can be a huge memory win.
    VULCANDLL void ReleaseSymbolic();

    // Insert the first/last instruction in this block
    VULCANDLL void InsertFirstInst(VInst*);
    VULCANDLL void InsertLastInst(VInst*);

    // Returns a mangled symbol name at offset ib within the block (pass pComp for speed)
    VULCANDLL size_t SymName(char *buf, size_t bufsize, VComp* pComp, int ib = 0, bool fNearest = false);

    // Creates a new code block - you must insert in into a procedure if you want it output (VComp is used to assign id)
    static VULCANDLL VBlock* VULCANCALL CreateCodeBlock(VComp*);

    // Creates a new code block with just the paltformtype
    static VULCANDLL VBlock* VULCANCALL CreateCodeBlock(PlatformType);

    // Creates a new data block - you must insert in into a procedure if you want it output (VComp is used to assign id)
    static VULCANDLL VBlock* VULCANCALL CreateDataBlock(VComp*, BYTE* pbData,size_t cb);

    // Get/set the alignment on this block (in bytes)
    // Valid values are 0 (default/none), 2 (WORD), 4 (DWORD), 8(DWORDLONG), 16, 32, 64, ... 8192)
    VULCANDLL size_t AlignmentSize();
    VULCANDLL void SetAlignmentSize(size_t align);

    // Set the data in this block (data blocks only)
    VULCANDLL void SetData(BYTE* data,size_t size);

    // Get the unique identifier of this block
    VULCANDLL DWORD BlockId();

    // Set the unique identifier of this block
    VULCANDLL void SetBlockId( DWORD blockId );

    // Get the address of this block
    VULCANDLL ADDR Addr();

    // Get the compiler-emmited address of this block (pass in a pComp for speed)
    VULCANDLL ADDR OrigAddr(VComp *pComp);

    // Get the size of this block
    VULCANDLL size_t Size();

    // Get a pointer to the bytes of this block (read-only)
    VULCANDLL const void* Raw(VComp *pComp);

    // Assemble this block to pb, returning the size
    VULCANDLL size_t Emit(BYTE* pb = NULL);

    // Get/set the follower (fall-through) block (NULL for none)
    VULCANDLL VBlock* BlockFollower();
    VULCANDLL bool SetBlockFollower(VBlock*);

    // Get the target block of this block (call, jmp, conditional jmp) (NULL for none)
    VULCANDLL VBlock* BlockTarget();
    VULCANDLL bool SetBlockTarget(VBlock*);

    // Get the target proc of this block (call)
    VULCANDLL VProc* ProcTarget();
    VULCANDLL bool SetProcTarget(VProc*);

    // Get the target import of this block (indirect call)
    VULCANDLL VImport* ImportTarget(VComp *pComp);
    VULCANDLL bool SetImportTarget(VImport*);

    // Get the reloc iter (empty for code blocks)
    VULCANDLL VRelocIter FirstReloc();

    // Returns an iterator for the source information of this block
    VULCANDLL VSrcIter FirstSrc(VComp *pComp);

    // Does this block end with a RET?
    VULCANDLL bool Returns();

    // Is this a data block?
    VULCANDLL bool IsDataBlock();

    // What is the termination type of this block
    VULCANDLL BlockTermType BlockTerminationType();

    // Does this block end with a CALL
    VULCANDLL bool HasCall();

    // Does this block end with a Conditional branch?
    VULCANDLL bool HasCBranch();

    // Has this block been inserted
    VULCANDLL bool IsInserted();

    // Is this block unreachable?
    VULCANDLL bool IsUnreachable();

    // Is this block obsolete?
    VULCANDLL bool IsObsolete();

    // Is this block a call target? (direct calls only)
    VULCANDLL bool IsCallTarget();

    // Is this a possible entry block for the procedure (IsCallTarget || IsAddrTakenGlobal)
    VULCANDLL bool IsEntryBlock();

    // Does somebody outside this
    VULCANDLL bool IsAddrTakenGlobal();

    // Is this block an jump table branch target or other intra-proc addr taken?
    VULCANDLL bool IsAddrTakenLocal();

    // Is this block instrumentable?  Can we insert new code into the block?
    VULCANDLL bool IsInstrumentable();

    // Have we been told to not split this block?
    VULCANDLL bool IsNoSplit();

    // Fills a buffer with the printed Vulcan instruction in this block (if the return is > than sizeBuf, the it failed)
    VULCANDLL size_t Print( char* szBuf, size_t sizeBuf );

    // Fills a buffer with the printed DISASM instruction in this block (if the return is > than sizeBuf, the it failed)
    VULCANDLL size_t PrintAsm( char* szBuf, size_t sizeBuf, VComp *pComp );

    // To delete
    VULCANDLL void Destroy();

    // Get/set the user data for this block
    inline void SetUserData(void*, VUserData*);
    inline void* GetUserData(VUserData*);

    // Debugging support (Print the blocks blkid, addr and return it)
    VULCANDLL const char * DbgPrint();

    // Returns iterator over all the symbols for this block.
    VULCANDLL VSymIter   * Symbols(VComp *);   // Call Destroy() after use.

    // Check if this block user's flag is set
    VULCANDLL bool IsUserFlagSet();
    VULCANDLL void SetUserFlag();
    VULCANDLL void ResetUserFlag();

    // Set/Get the platform types
    VULCANDLL void          SetPlatformT(PlatformType);
    VULCANDLL PlatformType  PlatformT();

    // Add symbol at this block
    VULCANDLL bool AddPublicSymbol(char* szName, VComp *pComp);

    // Copy this block into pBlkCopy (or optionally make a new block)
    VULCANDLL VBlock *Copy( VComp *pComp, VBlock *pBlkCopy = NULL);

    // For dynamic mode, replace the contents of this block with the contents of pBlk
    // The original instructions are moved to either pBlkOldCopy or the returned block and committed
    VULCANDLL VBlock *ReplaceInstructions( VBlock *pBlk, VComp *pComp, VBlock *pBlkOldCopy = NULL);

    // For dynamic mode, commit this block to memory
    VULCANDLL ADDR Commit( VComp *pComp );

    // Output statistics about the number of blocks.  In retail builds of
    // Vulcan this function will do nothing.  Each line of output will be
    // preceded by the string `pcszPrefix'.
    static VULCANDLL void VULCANCALL PrintBlockStats( const char *pcszPrefix );

    // Return the VInst that "contains" the specified address (i.e. the
    // address is >= the VInst's Addr(), and < the VInst's "Addr() +
    // Size()".  This function is not terribly efficient: it simply
    // iterates over all the VInsts in the VBlock testing their addresses
    // and sizes.  Returns NULL if no matching VInst is found.
    VULCANDLL VInst *FindInst( ADDR );

protected:
    VBlock() {}
    ~VBlock() {}
};

class VInst : public CDListElem_VInst
{
public:
    // Create an empty instruction, you must add it to a block yourself
    static VULCANDLL VInst*  VULCANCALL Create();

    // Create an instruction that only has an opcode (eg. NOP)
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode);

    // Create an instruction that has an opcode and a register (eg. PUSHD  EAX); (also use for POP EAX!)
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, ERegister source);

    // Create an instruction that has an opcode and an immediate (eg. PUSHD, 42);
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, long immediate);

    // Create an instruction that has an opcode and 2 registers (eg. MOVD, EAX, ESP);
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, ERegister dest, ERegister source);

    // Create an instruction that has an opcode, and register, and an immediate (eg. MOVD, EAX, 42);
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, ERegister dest, long immediate);

    // Create an instruction that has an opcode, and three registers (eg. ADDD, ESP, ESP, EAX);
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, ERegister dest, ERegister source1, ERegister source2);

    // Create an instruction that has an opcode, two registers, and an immediate (eg. ADDD, ESP, ESP, 4);
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, ERegister dest, ERegister source, long immediate);

    // Create an instruction that has an opcode, and a target block (eg. JMP, pblock );
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, VBlock* pBlockTarget );

    // Create an instruction that has an opcode, a destination, and an address (eg. LEA, eax, [ESP + 4] );
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, ERegister dest, const VAddress*  pAddr );

    // Create an instruction that has an opcode, a dest address (eg. PUSH, [ESP + 4] );
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, const VAddress*  pAddr );

    // Creata an instruciton that has an opcode, a dest address, and a constant (eg. MOV [pblock + offset], 1)
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, const VAddress*  pAddr, long immediate );

    // Create an instruction that has an opcode, a destination, and an address (eg. MOVD, [ESP + 4, eax );
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, const VAddress* pAddr, ERegister reg );

    // Assembles from bytes, moves ppbSrc forward to end of instruction
    static VULCANDLL VInst*  VULCANCALL Create(BYTE** ppbSrc, BYTE* pbEnd);

    // Create an instruction using 3 VOperand's
    static VULCANDLL VInst*  VULCANCALL Create(EOpcodes opcode, VOperand &dest, VOperand &src1, VOperand &src2);

    // Copy an instruction
    static VULCANDLL VInst*  VULCANCALL Create(VInst*);

    // Get the block that this instruction is in.
    VULCANDLL VBlock* ParentBlock();

    // Get the first/last/next/prev instruction in this block
    inline VInst* Next();
    inline VInst* Prev();
    inline VInst* First();
    inline VInst* Last();

    // Insert an instruction before/after this instruction
    inline void InsertPrev(VInst*);
    inline void InsertNext(VInst*);

    // remove this instruction from the block
    inline void Remove();

    // Compute the address of this instruction (block's addr + offset)
    VULCANDLL ADDR Addr();

    // Get the source info for this instruction
    VULCANDLL VSrcIter FirstSrc(VComp *pComp);

    // Fills a buffer with the printed Vulcan instruction (if the return is > than sizeBuf, the it failed)
    VULCANDLL size_t Print( char* szBuf, size_t sizeBuf );

    // Fills a buffer with the printed DISASM instruction (if the return is > than sizeBuf, the it failed)
    VULCANDLL size_t PrintAsm( char* szBuf, size_t sizeBuf, VComp * );

    // Get/set the opcode of this instruction (eg. CALL,BNE, LSL, ...)
    VULCANDLL EOpcodes Opcode();
    VULCANDLL void     SetOpcode(EOpcodes eopcode);

    // Get the string representation of this instruction
    VULCANDLL const char* OpcodeStr();

    // Get the Opcode group of this instruction (eg. GCALL,GB,GILSL)
    VULCANDLL EOpGrp OpcodeGrp();

    // Get the size of the "primary" operand (0 (NOP), 8 (MOVB), 16, 32, 64...)
    VULCANDLL size_t OpSize();

    // Get the Opcode group as a string
    VULCANDLL const char* OpcodeGroupStr();

    // How many bytes is this instruction?
    VULCANDLL size_t Size();

    // What is the target of this instruction (if call/jmp), otherwise NULL
    VULCANDLL VBlock* BlockTarget();
    VULCANDLL bool SetBlockTarget( VBlock*  );

    // Get the target proc of this block (call)
    VULCANDLL VProc* ProcTarget();
    VULCANDLL bool SetProcTarget(VProc*);

    // Get the target import of this block (indirect calls)
    VULCANDLL VImport* ImportTarget(VComp* pComp);
    VULCANDLL bool SetImportTarget(VImport*);

    // Does this instruction represent data?
    VULCANDLL bool IsData();

    // Does this instruction read memory?
    VULCANDLL bool ReadsMemory();

    // Does this instruction write memory?
    VULCANDLL bool WritesMemory();

    // Does this instruction reference an offset of ESP?
    VULCANDLL bool StackMemory();

    // Is this instruction not from the original binary?
    VULCANDLL bool IsInserted();

    //Which registers does this instruction defined/use?
    //(fNoCover == false gives AL, AH, AX, EAH, and EAX for EAX - and so forth)
    VULCANDLL void RegsDef( CBmp *pBmp, bool fNoCover = false );
    VULCANDLL void RegsUse( CBmp *pBmp, bool fNoCover = false );

    //Is this instruction valid (can it be emitted under the current arch)?
    VULCANDLL bool IsValid( VBlock *pBlk );

    // Get/set the user data for this block
    inline void SetUserData(void*, VUserData*);
    inline void* GetUserData(VUserData*);

    // To delete
    VULCANDLL void Destroy();

    // Get/Set the X86-specific prefix of this instruction
    VULCANDLL Inst::Ex86Prefix X86Prefix();
    VULCANDLL void SetX86Prefix( Inst::Ex86Prefix );

    // Get the operand as a string (this interface will change)
    VULCANDLL const char* OperandStr(Inst::EOpParam epar);

    // Get the operands to the instruction (this interface will change)
    VULCANDLL VOperand Operand(Inst::EOpParam);

    // More complex instructions - build by setting params (this interface will change)
    VULCANDLL void SetOperand( Inst::EOpParam, VOperand &op );

    // Debugging support (Print the instruction and return it)
    VULCANDLL const char * DbgPrint();

    // Access to instruction completers (currently, IA64 only)
    VULCANDLL void SetCompleter(ECompleters value);
    VULCANDLL bool TestCompleter(ECompleters value) const;

    // Access to cycle break information (IA64 only)
    VULCANDLL bool IsCycleBreak() const;
    VULCANDLL void SetCycleBreak(bool value);

    // Test for IA64 marker instruction (cannot delete/replace)
    VULCANDLL bool IsMarker() const;

protected:
    // you can't create or delete these!
    VInst() {}
    ~VInst() {}
};

enum Where {BEFORE,AFTER, PREV=BEFORE, NEXT=AFTER};

class VProto
{
public:
    // AddCall(pComp, BEFORE...) places the call before the first instruction of the entry point
    // AddCall(pComp, AFTER...) places a call before each ret of the entry proc
    virtual void __cdecl AddCall(VComp* pComp, Where w, ...) = 0;

    // AddCall(pProc, BEFORE...) places the call before the first instructions in the proc
    // AddCall(pProc, AFTER...) places a call before each ret in the proc
    virtual void __cdecl AddCall(VProc* pProc, Where w, ...) = 0;

    // AddCall(pBlk, BEFORE...) places the call before the instructions in the block
    // AddCall(pBlk, AFTER...) places the call at the last point gaurenteed to execute in the block
    virtual void __cdecl AddCall(VBlock* pBlk,Where w, ...) = 0;

    // AddCall(pInst, BEFORE...) places the call before the instruction
    // AddCall(pInst, AFTER...) places the call after the instruction (it will never execute in pInst is a RET!)
    virtual void __cdecl AddCall(VInst* pInst, Where w, ...) = 0;

// Examples  // note the last line in each example belongs in "foodll.dll"
    // Int, short, char
    // VProto* bbProto = bbProtoList.CreateProto(vcomp,"foodll.dll","SendNum(int)");
    // bbProto->AddCall(pblock,BEFORE,5);
    // extern "C" __declspec(dllexport) void SendNum(int num)
    // int[],short[],char[]
    // sprintf(szproto"SendNums(int[%d])",arraysize);  // sprintf used to create a dynamically sized array
    // VProto* bbProto = bbProtoList.CreateProto(vcomp,"foodll.dll",szproto);
    // bbProto->AddCall(pblock,BEFORE,array);  // array being a int[arraysize]
    // extern "C" __declspec(dllexport) void SendNum(int* array);   // Note it is common to send the arraysize in a different function call
    // register
    // VProto* bbProto = bbProtoList.CreateProto(vcomp,"foodll.dll","SendReg(reg)");
    // bbProto->AddCall(pblock,BEFORE,X86Reg::EAX);
    // extern "C" __declspec(dllexport) void SendReg(int regvalue)
    // Effective address or the memory location of a load/store
    // VProto* iiProto = iiProtoList.CreateProto(vcomp,"foodll.dll","SendAddr(eff)");
    // iiProto->AddCall(pinst,BEFORE,Eff::ANY);  // should only use on a memory instruction
    // extern "C" __declspec(dllexport) void SendEff(int addr)
    // BlockAddr
    // VProto* bbProto = bbProtoList.CreateProto(vcomp,"foodll.dll","SendAddr(blockaddr)");
    // bbProto->AddCall(pblk,BEFORE,VBlock);
    // extern "C" __declspec(dllexport) void SendAddr(int Addr)
    // InstAddr
    // VProto* bbProto = bbProtoList.CreateProto(vcomp,"foodll.dll","SendAddr(instaddr)");
    // bbProto->AddCall(pblk,BEFORE,VInst);
    // extern "C" __declspec(dllexport) void SendAddr(int Addr)
    // CondCode
    // VProto* bbProto = bbProtoList.CreateProto(vcomp,"foodll.dll","Sendcond(condcode)");
    // bbProto->AddCall(pblk,BEFORE,EOp::ISETULT);
    // extern "C" __declspec(dllexport) void Sendcond(int cond)  // cond is either 0 or 1

    virtual VComp* Comp() = 0;
    virtual VBlock* Export() = 0;
};

class VProtoList
{
public:
    // Create a new set of prototypes
    static VULCANDLL VProtoList * VULCANCALL CreateProtoList();

    // Create a new import that you want to call from pComp into szDll at the export named szProto
    // SzProto needs to be declared as extern "C" __declspec(dllexport) ...
    // fUseWrapper means that the registers are saved and restored around the call (so you can use them all)
    virtual VProto* CreateProto( VComp* pComp, const char* szDll, const char* szProto, bool fUseWrapper = true) = 0;

    // Commit the protos that were created from this list
    // The order is COMP, then Proc, then Block, then Inst.
    // Within each group, the order is the order AddCall was called.
    virtual void Commit() = 0;

    // Uncommit the protos that were cread from this list
    virtual void Revert() = 0;

    //To Delete
    virtual void Destroy() = 0;
};

#include "vulcan.inl"

//backward compatibility
typedef VBlock VBBlock;

// This should only ever be called after all other Vulcan objects have been deallocated
// This will go through and clean up all outstanding allocations, and will return the memory
// usage back to that before any Vulcan calls were made.
VULCANDLL void VULCANCALL VulcanCleanUpAllAllocations(void);

#endif
