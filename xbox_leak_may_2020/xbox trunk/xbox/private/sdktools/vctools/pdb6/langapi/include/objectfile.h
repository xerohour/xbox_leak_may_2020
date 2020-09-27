//-----------------------------------------------------------------------------
//	ObjectFile.h
//
//  Copyright (C) 1997, Microsoft Corporation
//
//  Purpose:
//		Define the classes for reading and groking object files
//
//  Revision History:
//
//	[]		10-Feb-1997 Dans	Created
//
//-----------------------------------------------------------------------------
#pragma once

#if !defined(_objectfile_h)
#define _objectfile_h 1

#include "crefobj.h"
#include "ref.h"
//#include "buffer.h"
#include "simparray.h"

#if !defined(OBJECTFILE_IMPL)
#define IMPORT_EXPORT   __declspec(dllimport)
#else
#define IMPORT_EXPORT   __declspec(dllexport)
#endif

typedef BYTE *                  PB;
typedef const PB                PCB;

namespace objf {

typedef IMAGE_ARCHIVE_MEMBER_HEADER ImgArchMbrHdr;
typedef ImgArchMbrHdr *         PImgArchMbrHdr;
typedef const ImgArchMbrHdr *   PCImgArchMbrHdr;

typedef IMAGE_FILE_HEADER       ImgFileHdr;
typedef ImgFileHdr *            PImgFileHdr;
typedef const PImgFileHdr       PCImgFileHdr;

typedef IMAGE_SECTION_HEADER    ImgSectHdr;
typedef ImgSectHdr *            PImgSectHdr;
typedef const ImgSectHdr *      PCImgSectHdr;

typedef IMAGE_RELOCATION        ImgReloc;
typedef ImgReloc *              PImgReloc;
typedef const ImgReloc *        PCImgReloc;

typedef IMAGE_LINENUMBER        ImgLineNo;
typedef PIMAGE_LINENUMBER       PImgLineNo;

typedef IMAGE_SYMBOL            ImgSym;
typedef ImgSym *                PImgSym;
typedef const ImgSym *          PCImgSym;

typedef IMAGE_AUX_SYMBOL        ImgAuxSym;
typedef ImgAuxSym *             PImgAuxSym;
typedef const ImgAuxSym *       PCImgAuxSym;

// symbol index (not COFF symbol index)
typedef DWORD                   SYMI;
const SYMI                      symiNil = 0;

// coff symbol table index
typedef DWORD                   COFFSYMI;
const COFFSYMI                  coffsymiNil = COFFSYMI(-1);


// munged info for fixups.  
struct FixupMap {
    DWORD   off;    // offset of fixup (in the section)
    DWORD   cb;     // how many bytes the fixup consumes in the data
    };

typedef SimpleArray<BYTE>       RGBYTE;
typedef SimpleArray<FixupMap>   RGFIXUPMAP;
typedef SimpleString            RGCH;

// 
struct SectionStat {
    bool     fComdat : 1;
    bool     fAssoc : 1;
    bool     fFunc : 1;
    bool     fExternal : 1;
    COFFSYMI csymi;
    bool     fImgSec : 1;
    };


#if !defined(pure)
#define pure = 0
#endif

#define Align(p, x)                (((x) & ((p)-1)) ? (((x) & ~((p)-1)) + p) : (x))

class ObjectCode;
class Section;
typedef RefPtr< ObjectCode >    RefObj;
typedef RefPtr< Section >       RefSec;
typedef RefPtr<SimpleString>    RefStr;

PdbInterface EnumSection;
PdbInterface EnumRelocation;
PdbInterface EnumImgSymbol;
PdbInterface EnumExSymbol;
typedef int             ISection;
const ISection          iSectionNil = 0;

const DWORD dwOrdNil = 0;       // 0 is an invalid ordinal number


// Dump flags
enum ODF {          // Object Dump Flags
    odfHdrs     = 0x01,
    odfSects    = 0x02,
    odfFixups   = 0x04,
    odfSyms     = 0x08,
    odfLineNum  = 0x10,
    odfAll      = (odfHdrs | odfSects | odfFixups | odfSyms | odfLineNum),
    odfFmtLong  = 0x20,
    odfFmtShort = 0x40,
    odfFmtByte  = 0x80
    };


// This interface is used by the objfile object to query the caller
// for the location of symbols at runtime.  Used in Section::FApplyFixups, for example.

typedef DWORD CRC;

PdbInterface RunTimeSymTab
{
    // Given a symbol that's undefined in the current module, returns its address,
    // section, and offset.
    virtual bool FAddrFromName( const char*, ISECT*, OFF*, DWORD* ) pure;

    // this returns the location where an object's contribution was loaded, given a name that that
    // contribution exports
    virtual bool FSectAddrFromName( const char*, ISECT*, OFF*, DWORD* ) pure;

    // this returns the location where an object contribution was loaded, given the CRC's that 
    // identify the component - used for statics that aren't defined in the same section
    // as some external.
    virtual bool FSectAddrFromCrc( CRC crcData, CRC crcReloc, DWORD cb, ISECT*, OFF*, DWORD* ) pure;

    // update the callee with line number information
    virtual bool FAddLines ( const char *, ISection, long, CB, long, USHORT, BYTE *, CB ) pure;
};

typedef RunTimeSymTab * PRunTimeSymTab;

// define the interface class to the rest of the world.
class ObjectCode : public CRefCountedObj {
    
public:
    // provide static creation methods so we don't have to expose operator
    // new/delete semantics
    IMPORT_EXPORT static bool __cdecl
    FCreate ( RefObj &, DWORD dwMachineTypeExpected, LPCTSTR szFileName, LPCTSTR szMemberName = 0 );

    IMPORT_EXPORT static bool __cdecl
    FCreate2 ( RefObj &, DWORD dwMachineTypeExpected, LPCTSTR szFileName,
              LPCTSTR szMemberName = 0, DWORD offset =0, DWORD cbObj = 0 );

    virtual COFFSYMI
    CoffSymiFromSymi ( SYMI symi ) const pure;

    // get the symbol index for a particular name
    virtual SYMI
    SymiFromSz ( LPCTSTR szName ) const pure;

    // get the name from a particular symbol index
    virtual LPCTSTR
    SzFromSymi ( SYMI symi ) const pure;

    virtual SYMI
    SymiFromCoffsymi ( COFFSYMI coffsymi ) const pure;

    // get a PImgSym from a coff symbol index
    virtual PCImgSym
    PCImgSymFromCoffsymi ( COFFSYMI coffsymi ) const pure;

    // determine whether the object represents an import
    virtual bool
    FImport ( ) const pure;

    // if the object's an import, return the dll name
    virtual char *
    SzDllName ( RefStr& ) const pure;

    // if the object is an import, returns the ordinal, or
    // dwOrdNil and its exported name
    virtual DWORD
    DwExportOrdinalName ( RefStr& ) const pure;

    virtual bool
    FSectionFromSzSymbol (
        LPCTSTR             szSymbolName,
        PCImgSectHdr &      pcImgSectHdr,
        RefPtr<RGBYTE> &    rgbSectdata,
        RefPtr<RGFIXUPMAP> &rgfixup
        ) const pure;

    virtual bool
    FSectionFromSzSymbol (
        LPCTSTR             szSymbolName,
        RefSec &            rSection
        ) const pure;

    // get the count of sections
    virtual DWORD
    CSections ( ) const pure;

    // get the count of image symbols
    virtual DWORD
    CImgSym ( ) const pure;

    // get a section enumerator
    virtual bool
    FGetEnumSection ( EnumSection ** ) const pure;

    // get a symbol enumerator
    virtual bool
    FGetEnumImgSymbol ( EnumImgSymbol ** ) const pure;

    // get the name of the object
    virtual char*
    SzName ( RefStr& ) const pure;

    // get the name of a symbol
    virtual LPCTSTR
    SzSymbol ( PCImgSym ) const pure;

    // get comdat symbol name defined by isection
    virtual void
    SzSymForIsec ( RGCH&, ISection ) const pure;

    virtual const char *
    SzRelocationType ( WORD, WORD *, bool * ) const pure;

    // emit an object
    virtual bool
    FEmitObject ( LPCTSTR ) pure;

    // emit an object, excluding a list of sections
    virtual bool
    FEmitObject ( LPCTSTR, LPCTSTR *, int ) pure;

    // dump an object
    virtual void
    Dump ( ODF ) const pure;

   };


// define the Section interface class to the rest of the world.
class Section : public CRefCountedObj {

public:

    virtual DWORD
    CRelocation ( ) const pure;

    // get a relocation enumerator
    virtual bool
    FGetEnumRelocation ( EnumRelocation ** ) const pure;

    // get the name of the section
    virtual char *
    SzName ( RefPtr<RGCH>& szName ) const pure;

    virtual DWORD
    CLineNum ( ) const pure;

    virtual DWORD
    CbRawData ( ) const pure;

    virtual DWORD
    dwCharacteristics ( ) const pure;

    virtual CB 
    cbAlignment() const pure;

    virtual bool
    FRawData( RefPtr<RGBYTE>& rgbSectdata ) const pure;

    virtual void
    LoadData ( void * ) const pure;

    virtual bool
    IsComdat() const pure;

    virtual bool
    IsAssociativeComdat() const pure;

    virtual ISection
    AssociativeSection() const pure;

    virtual bool
    FAssociativeSection( RefSec& rsec ) pure;

    virtual bool
    FAbsentInImage( PRunTimeSymTab ) const pure;

    virtual bool
    FImageSection() const pure;

    // apply fixups to the section's data
    virtual bool
    FApplyFixups ( DWORD vaImgBase, DWORD vaSection, PRunTimeSymTab, RefPtr<RGBYTE>& ) pure;

    // retrieve a block of line numbers
    virtual bool
    FGetLineNumbers ( DWORD, ISection, PRunTimeSymTab ) const pure;

    // get the section's statistics
    virtual void
    GetSectionStatistics( SectionStat& ) const pure;

    virtual void
    LoadFixupMap ( void * ) const pure;

    virtual void
    LoadLineNumbers ( void * ) const pure;

    virtual void
    Dump ( ODF ) const pure;

    // get an external symbol enumerator
    virtual bool
    FGetEnumExSymbol ( EnumExSymbol ** ) const pure;

    // get an associative comdat enumerator
    virtual bool
    FGetEnumComdat ( EnumSection ** ) const pure;

    virtual bool
    FGetCrcs ( CRC* pcrcData, CRC* pcrcReloc ) const pure;

    virtual PImgSectHdr
    GetSectionHeader ( ImgSectHdr& ) const pure;

    virtual void
    SetSectionHeader ( ImgSectHdr& ) pure;
    
    virtual void *
    GetDataPtr ( void ) const pure;
};
    
typedef Section *       PSection;

// section enumerator
PdbInterface EnumSection : public Enum {
    virtual void get( RefPtr<Section>& ) pure;
};

// relocation/fixup enumerator
PdbInterface EnumRelocation : public Enum {
    virtual void get( PImgReloc * const ) pure;
};

// symbol enumerator
PdbInterface EnumImgSymbol : public Enum {
    virtual void get ( PImgSym * const ) pure;
};

// external symbol enumerator
PdbInterface EnumExSymbol : public Enum {
    virtual void get ( RefPtr<RGCH>& szName, OFF* off, bool* fFunc ) pure;
};



PdbInterface Library;
typedef RefPtr<Library> RefLib;

// Object enumerator
PdbInterface EnumObject : public Enum {
    virtual void get( RefObj& ) pure;
};

// symbol enumerator
PdbInterface EnumSymbol : public Enum {
    virtual void get ( RefStr& ) pure;
};

PdbInterface Library : public CRefCountedObj {
    
public:
    // provide static creation methods so we don't have to expose operator
    // new/delete semantics
    IMPORT_EXPORT static bool __cdecl
    FCreate ( RefLib &, DWORD, LPCTSTR );

    virtual bool
    FObjectFromSzSymbol (
        LPCTSTR,
        RefObj&
        ) const pure;

    virtual bool
    FObjectFromSzName (
        LPCTSTR,
        RefObj&
        ) const pure;

    // get the count of objects
    virtual DWORD
    CObjects ( ) const pure;

    // get the count of symbols
    virtual DWORD
    CSymbols ( ) const pure;

    // get an object enumerator
    virtual bool
    FGetEnumObject ( EnumObject ** ) const pure;

    // get a symbol enumerator
    virtual bool
    FGetEnumSymbol ( EnumSymbol ** ) const pure;

    // get the name of the library
    virtual char*
    SzName ( RefStr& ) const pure;

};

    PdbInterface Image;
    typedef RefPtr<Image>     RefImg;

    // import enumerator
    PdbInterface EnumImport : public Enum {
        virtual void get ( RefStr& ) pure;
    };

    PdbInterface Image : public CRefCountedObj 
    {
        IMPORT_EXPORT static bool __cdecl
        FCreate ( RefImg &, DWORD dwMachineTypeExpected, LPCTSTR szFileName );

        virtual bool 
        FExportRvaByOrdinal( DWORD dwOrdinal, DWORD* prva, RefStr&  ) const pure;  // rva of export by ordinal

        virtual bool
        FExportRvaByName( LPCTSTR szName, DWORD* prva, RefStr&  )  const pure;  // rva by export name

        virtual bool
        FGetEnumImport ( EnumImport ** ) const pure;

        virtual void Dump( ODF odf ) const pure;

        // get the name of the image
        virtual char*
        SzName ( RefStr& ) const pure;
    };


};      // end of namespace

#endif
