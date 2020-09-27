#pragma once
#ifndef _COMPSYM_H_
#define _COMPSYM_H_

#if !defined(PdbInterface)
#define PdbInterface struct
#endif

#ifndef IMPORT_EXPORT
#ifndef OBJFILE_IMPL
#define IMPORT_EXPORT __declspec(dllimport)
PdbInterface   ObjectCode;
typedef PdbInterface ObjectCode        ObjectCode;
#else
#define IMPORT_EXPORT __declspec(dllexport)
#endif
#endif

PdbInterface   FunctionSymbols;
PdbInterface   EnumStackSyms;
PdbInterface   StackVariable;

typedef PdbInterface FunctionSymbols   FunctionSymbols;
typedef PdbInterface EnumStackSyms     EnumStackSyms;
typedef PdbInterface StackVariable     StackVariable;

#define COMPSYMAPI __cdecl

#ifdef __cplusplus

// C++ Binding

PdbInterface FunctionSymbols {
    IMPORT_EXPORT static bool COMPSYMAPI
    get( char* szName, ObjectCode* pobj, FunctionSymbols** ppfsyms );

    IMPORT_EXPORT static bool COMPSYMAPI
    get( RefSec& rsec, FunctionSymbols** ppfsyms );

    virtual bool  FGetEnumLocals ( EnumStackSyms ** ) const pure;
    virtual bool  FGetEnumParams ( EnumStackSyms ** ) const pure;
    virtual bool  FGetFrameInfo(  
        unsigned long*  cbFrame, 
        unsigned long*  cbPad,
        unsigned long*  offPad ) const pure;
    virtual void release() pure;
    virtual void Dump() const pure;
};

// stack variable enumerator
PdbInterface EnumStackSyms : public Enum {
    virtual void get( StackVariable ** const ) pure;
};

PdbInterface StackVariable
{
    virtual long offBP() const pure;
    virtual unsigned long typeIndex() const pure;
    virtual unsigned long name( char* szName ) const pure;// if szName ==0 return length, then call again with buffer
    virtual void release() pure;
    virtual void Dump() const pure;
};

#endif  // __cplusplus

// ANSI C Binding

#ifdef __cplusplus
extern "C" {
#endif

IMPORT_EXPORT
BOOL COMPSYMAPI ObjectCodeGet(ObjectCode** ppobj, 
                              DWORD dwMachineTypeExpected, 
                              char* szFileName );
IMPORT_EXPORT
void COMPSYMAPI OCRelease( ObjectCode* pobj );

IMPORT_EXPORT 
BOOL COMPSYMAPI FuncSymsGet(char* szName, ObjectCode* pobj, FunctionSymbols** ppfsyms );
IMPORT_EXPORT 
BOOL COMPSYMAPI FSGetFrameInfo(FunctionSymbols* pfuncs, 
                               unsigned long*  cbFrame, 
                               unsigned long*  cbPad,
                               unsigned long*  offPad);
IMPORT_EXPORT 
BOOL COMPSYMAPI FSGetEnumParams(FunctionSymbols* pfuncs, 
                                EnumStackSyms** ppesv);
IMPORT_EXPORT 
BOOL COMPSYMAPI FSGetEnumLocals(FunctionSymbols* pfuncs, 
                                EnumStackSyms** ppesv);
IMPORT_EXPORT 
void COMPSYMAPI FSRelease(FunctionSymbols* pfuncs);

IMPORT_EXPORT 
void COMPSYMAPI ESVRelease(EnumStackSyms* pesv);
IMPORT_EXPORT 
void COMPSYMAPI ESVReset(EnumStackSyms* pesv);
IMPORT_EXPORT 
BOOL COMPSYMAPI ESVNext(EnumStackSyms* pesv);
IMPORT_EXPORT 
void COMPSYMAPI ESVGet(EnumStackSyms* pesv, 
            StackVariable **ppsv);

IMPORT_EXPORT 
long COMPSYMAPI SVOffBP(StackVariable * psv);
IMPORT_EXPORT 
unsigned long COMPSYMAPI SVTypeIndex(StackVariable * psv);
IMPORT_EXPORT 
unsigned long COMPSYMAPI SVName(StackVariable * psv, 
                                char* szName ); // if szName ==0 return length, then call again with buffer
IMPORT_EXPORT 
void COMPSYMAPI SVRelease(StackVariable * psv);

#ifdef __cplusplus
}
#endif

#endif
