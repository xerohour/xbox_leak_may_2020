// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/object.h 6     5/09/96 1:25p Erikruf $
// $NoKeywords: $
//

// A base class for our objects

#ifndef INC_OBJECT_H 
#define INC_OBJECT_H


//
// forward declarations
//

class Object;
class DumpContext;
class Allocator;

//
// The root of all evil 
//

class Object
{
  public: 
    // A bunch of custom allocation methods
    // 1. We want to be able to use an Allocator placement operand
    //    (at the moment, this is reundant because alloc.h/alloc.cpp
    //    hacks the GLOBAL "new" to do this, but that will go away once
    //    operator new[] comes online).
    ASTAPI void * ASTCALL operator new(size_t size, Allocator *baseAllocator);
    // 2. We also need to override the normal "new" and "delete" to force
    // library clients to use the DLL's malloc/delete (see VC bug Q122675).
    ASTAPI void * ASTCALL operator new(size_t size);
    ASTAPI void ASTCALL operator delete(void *pMem);
    // Return a string denoting the instance's class
    ASTAPI virtual const char *ClassName(void);
    // Dump something printable onto a dump context
    ASTAPI void Dump(DumpContext *dc = NULL);
    ASTAPI void RecursiveDump(DumpContext *dc);
  protected:
    // The protected methods need to have the export bit set because 
    // client classes may inherit them, and because library classes
    // may have inline methods (e.g. constructors) referencing them.

    // Subclasses override this dump method
    ASTAPI virtual void DumpMethod(DumpContext *dc);
    // Utility fcns visible to subclasses
    ASTAPI virtual void DumpStandardPrefix(DumpContext *dc);
    ASTAPI Object() {};
    ASTAPI ~Object() {};
  private:
};

#endif // INC_OBJECT_H




