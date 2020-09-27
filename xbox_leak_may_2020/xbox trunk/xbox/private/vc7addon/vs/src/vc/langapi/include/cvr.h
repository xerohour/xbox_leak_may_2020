// CVR: CodeView Record utilities

#ifndef _VC_VER_INC
#include "..\include\vcver.h"
#endif

#ifndef __CVR_INCLUDED__
#define __CVR_INCLUDED__

#include <pdb.h>
#include <cvinfo.h>
#include <stddef.h>
#ifndef _WINDOWS_
// get rid of baggage we don't need from windows.h
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#define NOUSER
#define NONLS
#include "windows.h"
#endif

typedef BYTE* PB;
typedef long CB;
typedef char* SZ;       // zero terminated string
typedef wchar_t* USZ;   // unicode zero terminated string
typedef char* ST;       // length prefixed string
typedef SYMTYPE* PSYM;
typedef SYMTYPE UNALIGNED * PSYMUNALIGNED;
typedef TYPTYPE* PTYPE;

//////////////////////////////////////////////////////////////////////////////
// TII (type index iterator) implementation declarations

typedef ptrdiff_t IB;
typedef TI*     (*PfnTyti)(PTYPE, int, PB*, PB);
typedef TI16*   (*PfnTyti16)(PTYPE, int, PB*, PB);

struct TYTI { // position of type indices within a type record with the given leaf
    unsigned leaf;
    SZ sz;                      // leaf type name
    int cib;
    const IB* rgibTI;
    union {                     // fn to call if cib == cibFunction
        PfnTyti     pfn;
        PfnTyti16   pfn16;
    };
    PB  (*pfnPbAfter)(void* pv);      // end of record fn to call for elements of a field list
};

// all pointers to a TYTI are pointing to const TYTIs
typedef const TYTI *    PTYTI;

struct SYTI { // position of symbol indices within a symbol recoord with the given rectyp
    unsigned rectyp;
    SZ sz;                      // symbol rectyp name
    IB  ibName;                 // position of symbol name
    ST (*pfnstName)(PSYM psym); // function to call if name offset variable
    BOOL isGlobal;              // symbol is global
    int cib;
    const IB* rgibTI;
};

// all pointers to a SYTI are pointing to const SYTIs
typedef const SYTI *    PSYTI;

#if defined(PDB_LIBRARY)
#define CVR_EXPORT
#else
#if defined(CVR_IMP)
#define CVR_EXPORT  __declspec(dllexport)
#else
#define CVR_EXPORT  __declspec(dllimport)
#endif
#endif

#ifndef CVRAPI
#define CVRAPI   __cdecl
#endif

// functions that are in widenti.cpp
unsigned __fastcall
MapLeaf16To32 ( unsigned leaf16 );

unsigned __fastcall
MapSymRec16To32 ( unsigned rectyp16 );

#ifdef LNGNM

// New data type

typedef char *UTFSZ;

// functions from sttosz.cpp

CVR_EXPORT unsigned CVRAPI
MapLeafStToSz ( unsigned leafST );

CVR_EXPORT unsigned CVRAPI
MapSymRecStToSz ( unsigned rectypST );

inline bool fNeedsSzConversion( PSYM psym )
{
    return MapSymRecStToSz( psym->rectyp ) != psym->rectyp;
}

inline bool fIsStFieldList(PTYPE);
inline bool fIsStRefSym(PTYPE);

inline bool fNeedsSzConversion( PTYPE ptype )
{
    if (ptype->leaf == LF_FIELDLIST)
        return fIsStFieldList(ptype);
    else if (ptype->leaf == LF_REFSYM)
        return fIsStRefSym(ptype);
    else
        return MapLeafStToSz( ptype->leaf ) != ptype->leaf;
}

CVR_EXPORT BOOL CVRAPI fConvertSymRecStToSz(PSYM psymST);
CVR_EXPORT BOOL CVRAPI fConvertTypeRecStToSz(PTYPE ptype);
CVR_EXPORT BOOL CVRAPI fConvertSymRecStToSz(PB pbSrc, CB cbSrc, PB pbDest, CB *pcbDest);
CVR_EXPORT BOOL CVRAPI fConvertSymRecStToSzWithSig(PB pbSrc, CB cbSrc, PB pbDest, CB *pcbDest);
CVR_EXPORT BOOL CVRAPI fConvertSymRecStToSzInPlace(PB pbSyms, CB cbSyms);
CVR_EXPORT BOOL CVRAPI fConvertTypeRecsStToSz(PB pbSrc, PB pbDest, CB *pcb);

// It's just too bad putting #ifdef LNGNM ... #endif
// around simple statements like assert, a little better way ...

#define SZSYMIDX(n)         (MapSymRecStToSz(n))
#define SZLEAFIDX(n)        (MapLeafStToSz(n))

#define IS_SZ_FORMAT_PDB(p) (p->fIsSZPDB())

#else

#define SZSYMIDX(n)         (n)
#define SZLEAFIDX(n)        (n)

#endif

// utility functions for determining if type/symbol records may/must be
// converted from 16 to 32 bits.
inline bool fIs16bitTypRec(unsigned leaf)
{
    return leaf < LF_TI16_MAX;
}

inline bool fIs16bitSymRec(unsigned rectyp)
{
    return rectyp < S_TI16_MAX;
}

inline bool fIs16bitRec(PTYPE ptype)
{
    return fIs16bitTypRec ( ptype->leaf );
}

inline bool fIs16bitRec(PSYM psym)
{
    return psym->rectyp < S_TI16_MAX;
}

inline bool fNeeds16bitConversion(PSYM psym)
{
    unsigned rectyp = psym->rectyp;
    return
        rectyp < S_TI16_MAX &&
        MapSymRec16To32(rectyp) != rectyp;
}

inline bool fNeeds16bitConversion(PTYPE ptype)
{
    unsigned leaf = ptype->leaf;
    if (leaf < LF_TI16_MAX) {
        if (leaf == LF_REFSYM) {
            return fNeeds16bitConversion(PSYM(((lfRefSym*)(&ptype->leaf))->Sym));
        }
        return MapLeaf16To32(leaf) != leaf;
    }
    return false;
}

class SymTiIter { // type indices within symbol record iterator
public:
    CVR_EXPORT  SymTiIter(PSYM psym_);
    inline TI&  rti();
    inline BOOL next();

    inline TI16& rti16();
    inline BOOL f16Ti();
    inline unsigned cti();
    inline PB pbEndRecordSansPad();

private:
    PSYM psym;          // current symbol
    int  iib;           // index of current TI in this symbol record
    PSYTI psyti;        // address of symbol->ti-info for current symbol record
};

inline TI& SymTiIter::rti()
{
    return *(TI*)((PB)psym + psyti->rgibTI[iib]);
}

inline TI16& SymTiIter::rti16()
{
    return *(TI16*)((PB)psym + psyti->rgibTI[iib]);
}

inline BOOL SymTiIter::f16Ti()
{
    return psyti->rectyp < S_TI16_MAX;
}

inline unsigned SymTiIter::cti()
{
    return psyti->cib;
}


inline BOOL SymTiIter::next()
{
    return ++iib < psyti->cib;
}

inline PB SymTiIter::pbEndRecordSansPad()
{
    if (psyti->ibName) {
        PB  pbT = PB(psym) + psyti->ibName;
        return pbT + *pbT + 1;
    }
    else if (psyti->pfnstName) {
        PB  pbT = PB((psyti->pfnstName)(psym));
        return pbT + *pbT + 1;
    }
    else
        return 0;
}

class TypeTiIter { // type indices within type record iterator
public:
    CVR_EXPORT  TypeTiIter(TYPTYPE* ptype);

    inline TI&
    rti();

    CVR_EXPORT  BOOL
    next();

    PB
    pbFindField(unsigned leaf);

    inline TI16&
    rti16();

    inline BOOL
    f16Ti();

    inline PB
    pbCurField();

    inline void
    leafChanged();

    CVR_EXPORT  BOOL
    nextField();

    CB
    cbPadForCurField();

    PB
    pbEndCurFieldSansPad();

    CVR_EXPORT  PB
    pbEndRecordSansPad();

private:
    void
    init();

    PTYPE ptype;        // current type
    USHORT* pleaf;      // leaf part of current type
    unsigned    leaf;   // cached, aligned, no op-size override version of leaf
    PB   pbFnState;     // private state of current iterating fn (iff ptyti->cib == cibFunction)
    PB   pbEnd;         // pointer just past end of type record
    int  iib;           // index of current TI in this type record
    BOOL isFieldList;   // TRUE if this type record is a LF_FIELDLIST
    TI*  pti;           // address of current TI
    PTYTI ptyti;        // address of type->ti-info for current type record
};

inline TI& TypeTiIter::rti()
{
    return *pti;
}

inline TI16& TypeTiIter::rti16()
{
    return *PTi16(pti);
}

inline BOOL TypeTiIter::f16Ti()
{
    return ptyti->leaf < LF_TI16_MAX;
}

inline PB TypeTiIter::pbCurField()
{
    if (isFieldList && PB(pleaf) < pbEnd)
        return PB(pleaf);
    return 0;
}

// when we widen types, we need to reset the data since we change the
// leaf value of field lists out from under the iterator
inline void TypeTiIter::leafChanged()
{
    leaf = *pleaf;
    init();
}


// utility function protos
CVR_EXPORT BOOL CVRAPI fGetSymName(PSYM psym, OUT ST* pst);
           BOOL fSymIsGlobal(PSYM psym);
           bool fSymIsData(PSYM);
           BOOL fGetTypeLeafName(PTYPE ptype, OUT SZ* psz);
CVR_EXPORT BOOL CVRAPI fGetSymRecTypName(PSYM psym, OUT SZ* psz);

#ifdef LNGNM
// utility functions for unicode names
CVR_EXPORT bool CVRAPI fNameFromSym( PSYM psym, OUT wchar_t* szName, IN OUT unsigned long* pcch);
CVR_EXPORT bool CVRAPI fNameFromType( PTYPE ptype, OUT wchar_t* szName, IN OUT unsigned long* pcch);

inline bool fNameIsSz( PSYM psym )
{
    return psym->rectyp > S_ST_MAX;
}

inline bool fIsStFieldList(PTYPE ptype)
{
    TypeTiIter tii(ptype);

    while (tii.pbCurField() != NULL)
    {
        lfEasy *plf = (lfEasy *) tii.pbCurField();
        if (MapLeafStToSz(plf->leaf) != plf->leaf)
            return TRUE;
        if ( plf->leaf > LF_ST_MAX )    // a new leaf, cannot be st.
            return FALSE;
        if (!tii.nextField())
            break;
    }

    return FALSE;
}

inline bool fIsStRefSym(PTYPE ptype)
{
    return fNeedsSzConversion(PSYM(((lfRefSym *)&ptype->leaf)->Sym));
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Inline utility functions.

// Return the number of bytes in an ST
inline CB cbForSt(ST st)
{
    return *(PB)st + 1;
}

// Return the number of bytes the type record occupies.
//
inline CB cbForType(PTYPE ptype)
{
    return ptype->len + sizeof(ptype->len);
}

// Return a pointer to the byte just past the end of the type record.
//
inline PB pbEndType(PTYPE ptype)
{
    return (PB)ptype + cbForType(ptype);
}

// Return the number of bytes the symbol record occupies.
//
#define MDALIGNTYPE_    DWORD

inline CB cbAlign_(CB cb)
{
    return ((cb + sizeof(MDALIGNTYPE_) - 1)) & ~(sizeof(MDALIGNTYPE_) - 1);
}

inline bool fIsRefSym(PSYMUNALIGNED psym)
{
    unsigned    rectyp = SZSYMIDX(psym->rectyp);
    return
        rectyp == S_PROCREF ||
        rectyp == S_DATAREF ||
        rectyp == S_LPROCREF ||
        rectyp == S_ANNOTATIONREF
#if defined(LNGNM)
        ||
        rectyp == S_TOKENREF
#endif
        ;
}

inline bool fIsThunkSym(PSYMUNALIGNED psym)
{
    unsigned rectyp = SZSYMIDX(psym->rectyp);
    return rectyp == S_THUNK32
#ifdef LNGNM
        || rectyp == S_TRAMPOLINE
#endif
    ;
}

inline CB cbForSym(PSYMUNALIGNED psym)
{
    CB cb = psym->reclen + sizeof(psym->reclen);
    // REFSYMs also have a hidden length preceeded name following the record,
    // REFSYM2s do not, as they have a first class name field.
    // S_ANNOTATIONREF symbols are REFSYM2s, even in 6.0 PDBs.
    //
    if (fIsRefSym(psym)) {
        unsigned    rectypCur = psym->rectyp;

#if defined(LNGNM)
        unsigned    rectypNew = MapSymRecStToSz(rectypCur);

        // S_ANNOTATIONREF syms are the same in old and new,
        // so this check will keep us from adding in bytes to
        // the length on the REFSYM2 for the S_ANNOTATIONREF.
        if (rectypNew != rectypCur)
#else
        if (rectypCur != S_ANNOTATIONREF)
#endif
        {
            cb += cbAlign_(cbForSt((ST)((PB)psym + cb)));
        }
    }
    return cb;
}

// Return a pointer to the byte just past the end of the symbol record.
//
inline PB pbEndSym(PSYM psym)
{
    return (PB)psym + cbForSym(psym);
}

inline BOOL fIntroducingVirtual ( CV_methodprop_e mprop )
{
    return mprop == CV_MTintro || mprop == CV_MTpureintro;
}

inline BOOL fVirtual ( CV_methodprop_e mprop )
{
    return
        fIntroducingVirtual ( mprop ) ||
        mprop == CV_MTvirtual ||
        mprop == CV_MTpurevirt
        ;
}


#endif // __CVR_INCLUDED__
