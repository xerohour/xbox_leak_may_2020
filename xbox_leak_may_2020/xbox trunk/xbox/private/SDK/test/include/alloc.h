// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/alloc.h 9     5/09/96 1:25p Erikruf $
// $NoKeywords: $
//
// 


#ifndef INC_ALLOC_H 
#define INC_ALLOC_H

//
// Forward declarations
//

///////////////////////////////////////////////////////////////////////////////
// Class structure (forward declarations)
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

class Allocator;
class   HeapAllocator;
class   ArenaAllocator;
class     UnalignedArenaAllocator;
class WithArena;
class WithOwnArena;
class WithOwnUnalignedArena;

////END////

//
// Allocator (abstract class)
//
class Allocator: public Object
{
  public: 
    ASTAPI virtual ~Allocator() { }

#ifndef _CRTDBG_MAP_ALLOC
#  define ALLOC_FORMALS(KIND,FILE,LINE)
#  define ALLOC_ACTUALS(KIND,FILE,LINE)
#  define ALLOC_OVERRIDE(PARENT) \
    ASTAPI virtual void * Alloc(size_t size)
#else
#  define ALLOC_FORMALS(KIND,FILE,LINE) , int KIND, const char *FILE, int LINE
#  define ALLOC_ACTUALS(KIND,FILE,LINE) , KIND, FILE, LINE
#  define ALLOC_OVERRIDE(PARENT) \
    using PARENT::Alloc;  /* so as not to break overloading */  \
    ASTAPI virtual void * Alloc(size_t size ALLOC_FORMALS(kind,file,line))

        // this would work better as a #define, but...
    ASTAPI inline void *  Alloc(size_t size) 
      { return Alloc(size, _NORMAL_BLOCK, __FILE__, __LINE__); }
#endif // _CRTDBG_MAP_ALLOC

    ASTAPI virtual void * Alloc(size_t size ALLOC_FORMALS(kind,file,line)) = 0;
    ASTAPI virtual void   Dealloc(void *obj) = 0;
    ASTAPI static void    Initialize();
    ASTAPI static void    Finalize();
    ASTAPI static HeapAllocator *heapAllocator;
    ASTAPI static HeapAllocator *GetHeapAllocator();
  protected:
    ASTAPI Allocator(): arenaSpace(0) { }
  internal:
    size_t arenaSpace;          // total space in use by child arenas
};

// Teach the global "new" operator how to allocate using Allocators
// We need this because we may want to allocate an array of objects
// using an Allocator, and (until VC decides to support operator new[] 
// overloading on a per-class basis) invokes the (*&%]&$#* global 
// allocator!
//
// Please note that objects allocated using "new" with an Allocator
// placement operand may not be deallocatable using "delete".  (As of
// this writing, delete happens to work for HeapAllocator, but this
// may change).
//
// REVIEW: export this?
ASTAPI void * operator new(size_t size, Allocator *baseAllocator
                           ALLOC_FORMALS(kind,file,line));
#ifdef _CRTDBG_MAP_ALLOC
ASTAPI inline void * operator new(size_t size, Allocator *baseAllocator)
{ return ::operator new(size, baseAllocator, _NORMAL_BLOCK, __FILE__, __LINE__); }
#endif

//
// Direct delegation to CRT malloc
//
class HeapAllocator: public Allocator
{
  public: // client entry points
    ASTAPI HeapAllocator(): 
        numAllocs(0), numDeallocs(0), numAllocBytes(0) 
      { }
    ASTAPI virtual ~HeapAllocator();
    // overrides
    ALLOC_OVERRIDE(Allocator);
    ASTAPI virtual void   Dealloc(void *obj);
    ASTAPI virtual void   DumpMethod(DumpContext *dc);
  private:
    int numAllocs, numDeallocs, numAllocBytes;
};


//
// Allocation in arenas
//

#define AST_ARENA_GRANULARITY 4            // must be power of 2
#define AST_ARENA_DEFAULT_BLOCKSIZE (16384-16)  // must be multiple of granularity

class ArenaAllocator: public Allocator
{
  public:
    ASTAPI ArenaAllocator(
        Allocator *baseAllocator = NULL, 
        size_t blockSize         = AST_ARENA_DEFAULT_BLOCKSIZE
      );
    ASTAPI virtual ~ArenaAllocator();
    ASTAPI void    Reset(void);
    ASTAPI void    SetBlockSize(size_t size);
    // overrides
    ALLOC_OVERRIDE(Allocator);
    ASTAPI virtual void   Dealloc(void *obj);
    ASTAPI virtual void   DumpMethod(DumpContext *dc);
  protected:
    inline size_t RoundUp(size_t);
    void *        RawAlloc(size_t);
    int numAllocs, numDeallocs, numAllocBytes;
    int numFragBytesItem, numFragBytesBlock;
  private: 
    Allocator *baseAllocator;
    size_t blockSize;
    struct Block;
    struct Block *rootBlock;
    Byte *freeByte;
    size_t bytesRemaining;

    size_t totalSize;           // of all allocated blocks in arena
};


// Version of ArenaAllocator to use if you don't care about 
// the alignment of what new/Alloc returns.
class UnalignedArenaAllocator : public ArenaAllocator
{
  public:
    ASTAPI UnalignedArenaAllocator(
        Allocator *baseAllocator = NULL, 
        size_t blockSize         = AST_ARENA_DEFAULT_BLOCKSIZE
      ): 
        ArenaAllocator(baseAllocator, blockSize) { }
    // overrides
    ALLOC_OVERRIDE(ArenaAllocator);
};    

// Mixin for objects that want to use an existing ArenaAllocator
//
// Subclass constructors must invoke the WithArena constructor on the
// existing ArenaAllocator.
//
// Subclass gains protected members
// - pAllocator holds the Allocator
// Subclass gains public members
// - Alloc(size_t size) allocates using pAllocator

class WithArena
{
  public:    
    ASTAPI void *Alloc(size_t size)
      { return pAllocator->Alloc(size); }

  protected:
    ASTAPI WithArena(ArenaAllocator *pAllocator):
        pAllocator(pAllocator) { }
    ASTAPI ~WithArena() { }
    ArenaAllocator *pAllocator;
};


// Mixin for objects that want to create a new ArenaAllocator on creation,
// and free it on destruction
//
// Subclass constructors must invoke the WithOwnArena constructor on
// the desired base Allocator and block size.  If not specified, these
// default to the default base/size for arena Allocators as specified in
// class ArenaAllocator.
//
// Subclass gains protected members
// - pAllocator holds the Allocator
// Subclass gains public members
// - Alloc(size_t size) allocates using pAllocator

class WithOwnArena
{
  public:
    ASTAPI void *Alloc(size_t size)
      { return pAllocator->Alloc(size); }
  protected: 
    ASTAPI WithOwnArena(
        Allocator *baseAllocator = NULL,
        size_t blockSize         = AST_ARENA_DEFAULT_BLOCKSIZE
      ): 
        myAllocator(baseAllocator, blockSize),
        pAllocator(&myAllocator) 
    { }
    ASTAPI ~WithOwnArena() { }
    ArenaAllocator *pAllocator;
  private: 
    ArenaAllocator myAllocator;
};    


class WithOwnUnalignedArena
{
  public:
    ASTAPI void *Alloc(size_t size) 
      { return pAllocator->Alloc(size); }
  protected: 
    ASTAPI WithOwnUnalignedArena(
        Allocator *baseAllocator = NULL,
        size_t blockSize         = AST_ARENA_DEFAULT_BLOCKSIZE
      ): 
        myAllocator(baseAllocator, blockSize),
        pAllocator(&myAllocator) 
      { }
    ASTAPI ~WithOwnUnalignedArena() { }
    UnalignedArenaAllocator *pAllocator;
  private: 
    UnalignedArenaAllocator myAllocator;
};    

#endif // INC_ALLOC_H
