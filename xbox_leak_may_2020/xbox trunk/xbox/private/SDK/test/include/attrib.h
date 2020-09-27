// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/proto.h 2     3/08/96 10:12a Erikruf $
// $NoKeywords: $
//
//

#ifndef INC_ATTRIB_H
#define INC_ATTRIB_H

///////////////////////////////////////////////////////////////////////////////
// Provides mappings from keys to values and uses these mappings
// to provide attributes on AST nodes.
// 
//   VoidPointerMap:              MapIndex -> void*
//   Attribute<OBJ, OBJMANAGER>:  OBJ -> void*
//   PointerMap<KEY, VALUE>:      KEY -> VALUE
// 
// (... and yes there should be a version of Attribute that
//      uses the PointerMap template... --rfc)

typedef unsigned long MapIndex;

//
// Class structure (forward declarations)
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

class VoidPointerMap;
class   DirectVoidPointerMap;
class     ExpandingDirectVoidPointerMap;
class   OpenHashedVoidPointerMap;
class VoidPointerMapIterator;
class   DirectVoidPointerMapIterator;
class   OpenHashedVoidPointerMapIterator;

////END////


///////////////////////////////////////////////////////////////////////////////
// VoidPointerMap 
        
// provides a MapIndex -> void* mapping.  
// This abstract class has essentially 2 implementations:
// 
//   (1) DirectVoidPointerMap
//       ExpandingDirectVoidPointerMap 
//         implement this mapping as an array and should be used whenever one
//         is planning to insert values for most or all possible index values
//         in the range 0..capacity-1.  
// 
//         For DirectVoidPointerMap, the capacity must be given at construction
//         time, is immutable and all indices must thereafter be within this range.
//         For ExpandingDirectVoidPointerMap, the capacity will be doubled
//         on demand (i.e., upon insertion of values at indices >= capacity).
// 
//   (2) OpenHashedVoidPointerMap
//         uses an open hashtable and should be used if a relatively small
//         proportion of the available indices will have values or if the
//         time/space overhead of a hashtable is not an issue.
// 
//      The constructor's capacity argument is the size of the hashtable and
//         should be somewhat greater than the number of indices expected to be
//         used (i.e., have values inserted for them), however the hashtable
//         will grow on demand.
// 

#define AST_OPENHASH_MIN 2 // must be a power of 2

// find the next power of two >= N and >= PREV
// PREV is assumed to be a power of 2.
static inline size_t
nextTwoPower(size_t n, size_t prev = AST_OPENHASH_MIN)
{
    for (; prev < n; prev*=2)
      AstAssert(prev);
    return prev;
}

class VoidPointerMapIterator;

// Abstract class for mapping
class VoidPointerMap: public Object
{
  public:
    ASTAPI virtual ~VoidPointerMap() {}

    // return true iff index has been assigned
    ASTAPI virtual Bool   FSet(MapIndex index) = 0;

    // return value assigned to index; error if none
    ASTAPI virtual void * Get(MapIndex index) = 0;

    // assign value to index
    ASTAPI virtual void   Set(MapIndex index, void * value) = 0;

    // assign value to index; error if already set
    ASTAPI virtual void   SetNew(MapIndex index, void * value);

    // return value assigned to index; defaultValue if none
    ASTAPI virtual void * GetDefaulting(MapIndex index, void *defaultValue) = 0;

    // return a fresh iterator; caller must delete it
    ASTAPI virtual        VoidPointerMapIterator *NewIterator() = 0;

    // make index be unassigned
    ASTAPI virtual void   UnSet(MapIndex index);

  protected:
    ASTAPI VoidPointerMap() {}
  private:
    // don't even THINK about copying one of these!
    VoidPointerMap(const VoidPointerMap &); // leave undefined!
    VoidPointerMap & operator = (const VoidPointerMap &);  // leave undefined!
};

class VoidPointerMapIterator
{
  public:
    ASTAPI virtual ~VoidPointerMapIterator() { }

    // advance iterator
    ASTAPI virtual Bool     FNext()    = 0;
    ASTAPI virtual void *   Next()     { return FNext() ? GetValue() : NULL; }
    ASTAPI virtual MapIndex GetIndex() = 0;
    ASTAPI virtual void *   GetValue() = 0;
  protected:
    ASTAPI VoidPointerMapIterator() { }
};

// Use an array for the mapping
class DirectVoidPointerMap: public VoidPointerMap
{
  public:
    ASTAPI DirectVoidPointerMap(
        size_t capacity,
        Allocator *allocator = Allocator::heapAllocator
      );
    ASTAPI virtual ~DirectVoidPointerMap();
    // overrides
    ASTAPI virtual Bool   FSet(MapIndex index);
    ASTAPI virtual void * Get(MapIndex index);
    ASTAPI virtual void   Set(MapIndex index, void * value);
    ASTAPI virtual void   SetNew(MapIndex index, void * value);
    ASTAPI virtual void   UnSet(MapIndex index);
    ASTAPI virtual void * GetDefaulting(MapIndex index, void *defaultValue);
    ASTAPI virtual VoidPointerMapIterator *NewIterator();
  protected:
    friend class DirectVoidPointerMapIterator;
    Allocator *allocator;
    MapIndex capacity;
    void **mapArray;
};

class ExpandingDirectVoidPointerMap: public DirectVoidPointerMap
{
  public:
    ASTAPI ExpandingDirectVoidPointerMap(
        size_t initialCapacity,
        Allocator *allocator = Allocator::heapAllocator
      );
    // overrides
    //             void * Get(MapIndex index) is the same;
    ASTAPI virtual Bool   FSet(MapIndex index);
    ASTAPI virtual void   Set(MapIndex index, void * value);
    ASTAPI virtual void   SetNew(MapIndex index, void * value);
    ASTAPI virtual void   UnSet(MapIndex index);
    ASTAPI virtual void * GetDefaulting(MapIndex index, void *defaultValue);
  private:
    void CheckExpand(MapIndex index);
};

class DirectVoidPointerMapIterator : public VoidPointerMapIterator
{
  public:
    DirectVoidPointerMapIterator(DirectVoidPointerMap *map) :
        map(map), position(0) { }

    //overrides
    ASTAPI virtual Bool     FNext();
    ASTAPI virtual void *   Next();
    ASTAPI virtual MapIndex GetIndex() { return position-1; }
    ASTAPI virtual void *   GetValue() { return map->mapArray[position-1]; }

  private:
    DirectVoidPointerMap *map;
    MapIndex position;
};

// Use an open hash table for the mapping


class OpenHashedVoidPointerMap: public VoidPointerMap
{
  public:
    ASTAPI OpenHashedVoidPointerMap(
        size_t initialCapacity,
        Allocator *allocator = Allocator::heapAllocator
      );
    ASTAPI virtual ~OpenHashedVoidPointerMap();
    // overrides
    ASTAPI virtual Bool   FSet(MapIndex index);
    ASTAPI virtual void * Get(MapIndex index);
    ASTAPI virtual void   Set(MapIndex index, void * value);
    ASTAPI virtual void   SetNew(MapIndex index, void * value);
    ASTAPI virtual void   UnSet(MapIndex index);
    ASTAPI virtual void * GetDefaulting(MapIndex index, void *defaultValue);
    ASTAPI virtual VoidPointerMapIterator *NewIterator();
  private:
    friend class OpenHashedVoidPointerMapIterator;
    Allocator *allocator;
    size_t capacity;            // number of slots
    size_t freeSpace;           // number of never-used slots
    size_t elements;            // number of live slots
    struct Pair {
        MapIndex index;
        void *value;
    };
    Pair *hashArray;
    void Rehash();
    Pair *MakeEmptyHashArray(size_t capacity);
    Pair *Find(MapIndex mapIndex);
    // size_t H(MapIndex mapIndex, int iter);
    // static MapIndex bogusIndex;
};

class OpenHashedVoidPointerMapIterator : public VoidPointerMapIterator
{
  public:
    OpenHashedVoidPointerMapIterator(OpenHashedVoidPointerMap *map):
        map(map), position(0) {}

    // overrides
    ASTAPI virtual Bool     FNext();
    ASTAPI virtual void *   Next();
    ASTAPI virtual MapIndex GetIndex() { return map->hashArray[position-1].index; }
    ASTAPI virtual void *   GetValue() { return map->hashArray[position-1].value; }

  private:
    OpenHashedVoidPointerMap *map;
    MapIndex position;
};

///////////////////////////////////////////////////////////////////////////////
// PointerMap
//
// provides a KEY -> ITEM* mapping
//        
// ITEM is a class of data items that are to be stored in the map.
// Any ITEM class must have an associated KEY class that contains
// unique identifiers for the ITEM instances stored in the map. Valid
// KEY types are pointers or MapIndex values.  When inserting a new
// element using Set or SetNew, allocate a new key in the appropriate
// arena, as this key is stored in the hash table without being
// duplicated.  It should be valid as long as the hash table is valid
// (so, use the same arena as that of the hash table). (The key could
// also be stored as a field of the associated ITEM object for which
// it is the unique identifier.) For search operations, using a local
// KEY and passing its address should suffice.
//
// Hash and equality operations must be provided by the KEY class, as defined below:
//
//   MapIndex KEY::KeyHash() : returns a MapIndex (possibly non-unique) that is the
//                             result of applying a hash function to the key
//
//   Bool operator==(KEY key) : returns TRUE iff this key and key identify the same item
//
// - Manuvir
////////////


// Sample implementation:
//
// class SymbolHashKey
// {
//     Symbol *symInfo;
//
//     MapIndex KeyHash()                 { return (MapIndex)symInfo; }
//     Bool operator==(SymbolHashKey key) { return symInfo==key.GetSymInfo(); }
// };
//
// class SymbolNode : public Object
// {
//     ...
//     SymbolHashKey key;
//     ...
// };
//
// OpenHashedPointerMap<SymbolNode,SymbolHashKey *> table;
//
//////
// 
// Sample implementation:
//
// class NameHashKey
// {
//     char *name;
//
//     MapIndex KeyHash()               { return StringToNumberHash(name); // your hash function }
//     Bool operator==(NameHashKey key) { return !strcmp(name,key->GetName()); }
// };
//
// class SymbolNode : public Object
// {
//     ...
//     NameHashKey key;
//     ...
// };
//
// OpenHashedPointerMap<SymbolNode,NameHashKey *> table;
// 

// The code below is almost identical to the code for the non-templatized maps.
// The only differences are wrt the use of template parameters.

#define ASTTAPI   AST_TEMPLATE_API
#define FLegalIndex(index)     (!((MapIndex)(index) & 0xC0000000))
#define FNeverUsedIndex(index)   ((MapIndex)(index) & 0x80000000)
#define FDeletedIndex(index)     ((MapIndex)(index) & 0x40000000)

#define SET_NEVERUSED(index)     (index = (KEY)0x80000000)
#define SET_DELETED(index)       (index = (KEY)0x40000000)

template<class ITEM, class KEY> class PointerMapIterator;
template<class ITEM, class KEY> class OpenHashedPointerMapIterator;

template<class ITEM, class KEY>
class PointerMap: public Object
{
  public:
    ASTTAPI virtual ~PointerMap() {}
    ASTTAPI virtual Bool   FSet(KEY index) = 0;
    ASTTAPI virtual ITEM * Get(KEY index) = 0;
    ASTTAPI virtual void   Set(KEY index, ITEM * value) = 0;
    ASTTAPI virtual void   SetNew(KEY index, ITEM * value);
    ASTTAPI virtual ITEM * GetDefaulting(KEY index, ITEM *defaultValue) = 0;
    ASTTAPI virtual        PointerMapIterator<ITEM,KEY> *NewIterator() = 0;
    ASTTAPI virtual void   UnSet(KEY index);

  protected:
    ASTTAPI PointerMap() {}

  private:
    PointerMap(const PointerMap &);
    PointerMap & operator = (const PointerMap &);
};

template<class ITEM, class KEY>
class PointerMapIterator
{
  public:
    ASTTAPI virtual ~PointerMapIterator() { }
    ASTTAPI virtual Bool   FNext()    = 0;
    ASTTAPI virtual ITEM * Next()     { return FNext() ? GetValue() : NULL; }
    ASTTAPI virtual KEY    GetIndex() = 0;
    ASTTAPI virtual ITEM * GetValue() = 0;
  protected:
    ASTTAPI PointerMapIterator() { }
};

template<class ITEM, class KEY>
class OpenHashedPointerMap: public PointerMap<ITEM,KEY>
{
  public:
    ASTTAPI OpenHashedPointerMap(
        size_t initialCapacity,
        Allocator *allocator = Allocator::heapAllocator
      );
    ASTTAPI virtual ~OpenHashedPointerMap();

    // overrides
    ASTTAPI virtual Bool   FSet(KEY index);
    ASTTAPI virtual ITEM * Get(KEY index);
    ASTTAPI virtual void   Set(KEY index, ITEM * value);
    ASTTAPI virtual void   SetNew(KEY index, ITEM * value);
    ASTTAPI virtual void   UnSet(KEY index);
    ASTTAPI virtual ITEM * GetDefaulting(KEY index, ITEM * defaultValue);
    ASTTAPI virtual PointerMapIterator<ITEM,KEY> *NewIterator();
  private:
    friend class OpenHashedPointerMapIterator<ITEM,KEY>;
    Allocator * allocator;
    size_t capacity;
    size_t freeSpace; 
    size_t elements;
    struct Pair {
        KEY index;
        ITEM * value;
    };
    Pair * hashArray;
    void Rehash();
    Pair * MakeEmptyHashArray(size_t capacity);
    Pair * Find(KEY mapIndex);
};

template<class ITEM, class KEY>
class OpenHashedPointerMapIterator : public PointerMapIterator<ITEM,KEY>
{
  public:
    OpenHashedPointerMapIterator(OpenHashedPointerMap<ITEM,KEY> *map):
        map(map), position(0) {}

    // overrides
    ASTTAPI virtual Bool   FNext();
    ASTTAPI virtual ITEM * Next();
    ASTTAPI virtual KEY    GetIndex() { return map->hashArray[position-1].index; }
    ASTTAPI virtual ITEM * GetValue() { return map->hashArray[position-1].value; }

  private:
    OpenHashedPointerMap<ITEM,KEY> *map;
    MapIndex position;
};

//
// PointerMap methods
//

template<class ITEM, class KEY>
void
PointerMap<ITEM,KEY>::UnSet(KEY index)
{
    USER_ERROR("This version of VoidPointerMap does not support UnSet!");;
}

template<class ITEM, class KEY>
void
PointerMap<ITEM,KEY>::SetNew(KEY index, ITEM *value)
{
    AstDeny(FSet(index));
    Set(index,value);
}

//
// OpenHashedPointerMap methods
//

template<class ITEM, class KEY>
OpenHashedPointerMap<ITEM,KEY>::OpenHashedPointerMap(size_t initialCapacity, Allocator *allocator):
    allocator(allocator), elements(0)
{
    capacity  = nextTwoPower(initialCapacity);
    freeSpace = capacity;
    hashArray = MakeEmptyHashArray(capacity);
}

template<class ITEM, class KEY>
OpenHashedPointerMap<ITEM,KEY>::~OpenHashedPointerMap()
{
    allocator->Dealloc(hashArray);
}

template<class ITEM, class KEY>
Bool
OpenHashedPointerMap<ITEM,KEY>::FSet(KEY mapIndex)
{
    AstAssert(FLegalIndex(mapIndex));
    Pair *p = Find(mapIndex);
    return FLegalIndex(p->index);
}

template<class ITEM, class KEY>
ITEM *
OpenHashedPointerMap<ITEM,KEY>::Get(KEY mapIndex)
{
    AstAssert(FLegalIndex(mapIndex));
    Pair *p = Find(mapIndex);
    AstAssert(FLegalIndex(p->index));
    return p->value;
}

template<class ITEM, class KEY>
ITEM *
OpenHashedPointerMap<ITEM,KEY>::GetDefaulting(KEY mapIndex, ITEM *defaultValue)
{
    AstAssert(FLegalIndex(mapIndex));
    Pair *p = Find(mapIndex);
    return FLegalIndex(p->index) ? p->value : defaultValue;
}

template<class ITEM, class KEY>
void
OpenHashedPointerMap<ITEM,KEY>::Set(KEY mapIndex, ITEM *value)
{
    AstAssert(FLegalIndex(mapIndex));

    if ((freeSpace - 1) * 5 < capacity)
      Rehash();

    Pair *p = Find(mapIndex);
    if (!FLegalIndex(p->index)) {
        elements++;
        if (FNeverUsedIndex(p->index))
          freeSpace--;
    }
    p->index = mapIndex;
    p->value = value;
}

template<class ITEM, class KEY>
void
OpenHashedPointerMap<ITEM,KEY>::SetNew(KEY mapIndex, ITEM *value)
{
    AstAssert(FLegalIndex(mapIndex));

    if ((freeSpace - 1) * 5 < capacity)
      Rehash();

    Pair *p = Find(mapIndex);
    AstDeny(FLegalIndex(p->index));

    elements++;
    if (FNeverUsedIndex(p->index))
      freeSpace--;

    p->index = mapIndex;
    p->value = value;
}

template<class ITEM, class KEY>
void
OpenHashedPointerMap<ITEM,KEY>::UnSet(KEY mapIndex)
{
    AstAssert(FLegalIndex(mapIndex));
    Pair *p = Find(mapIndex);
    if (FLegalIndex(p->index)) {
        SET_DELETED(p->index);
        elements--;
    }
}

#define KEY_EQUAL(a,b)          FLegalIndex(a)?(*a==*b):FALSE

template<class ITEM, class KEY>
OpenHashedPointerMap<ITEM,KEY>::Pair *
OpenHashedPointerMap<ITEM,KEY>::Find(KEY mapIndex)
{
    size_t i = 0;
    Pair *thisSlot = hashArray + mapIndex->KeyHash() % capacity;
    Pair *openSlot = NULL;
    for (;;) {
        if (KEY_EQUAL(thisSlot->index,mapIndex)) {
            if (openSlot) {
                openSlot->index = thisSlot->index;
                openSlot->value = thisSlot->value;
                SET_DELETED(thisSlot->index);
                return openSlot;
            }
            else
              return thisSlot;
        }
        else if (FNeverUsedIndex(thisSlot->index)) {
            return openSlot ? openSlot : thisSlot;
        }
        if ((!openSlot) && FDeletedIndex(thisSlot->index))
          openSlot = thisSlot;
        ++i;
        AstAssert (i < capacity);

        thisSlot += ((thisSlot < hashArray + i) ? capacity : 0) - i;
    }
    AST_NOT_REACHED;
    return NULL;
}

template<class ITEM, class KEY>
OpenHashedPointerMap<ITEM,KEY>::Pair *
OpenHashedPointerMap<ITEM,KEY>::MakeEmptyHashArray(size_t capacity)
{
    Pair *result = (Pair *)allocator->Alloc(capacity * sizeof(Pair));
    size_t i;
    for (i=0; i<capacity; i++)
      SET_NEVERUSED(result[i].index);
    return result;
}

template<class ITEM, class KEY>
void
OpenHashedPointerMap<ITEM,KEY>::Rehash()
{
    Pair *oldHashArray = hashArray;
    size_t oldCapacity = capacity;

    if (2*(elements+1) > capacity)
      capacity *= 2;
    freeSpace = capacity;
    elements  = 0;

    hashArray = MakeEmptyHashArray(capacity);
    Pair *op  = oldHashArray + oldCapacity;
    do {
        --op;
        if (FLegalIndex(op->index))
          Set(op->index, op->value);
    } while (op > oldHashArray);

    allocator->Dealloc(oldHashArray);
}

template<class ITEM, class KEY>
Bool
OpenHashedPointerMapIterator<ITEM,KEY>::FNext()
{
    if (map != NULL)
      while (position < map->capacity)
        if (FLegalIndex(map->hashArray[position++].index))
          return TRUE;
    return FALSE;
}

template<class ITEM, class KEY>
ITEM *
OpenHashedPointerMapIterator<ITEM,KEY>::Next()
{
    if (map != NULL)
      for (; position < map->capacity; ++position)
        if (FLegalIndex(map->hashArray[position].index))
          return map->hashArray[position++].value;
    return NULL;
}

template<class ITEM, class KEY>
PointerMapIterator<ITEM,KEY> *
OpenHashedPointerMap<ITEM,KEY>::NewIterator()
{
    return new OpenHashedPointerMapIterator<ITEM,KEY>(this);
}

#undef FLegalIndex
#undef FNeverUsedIndex
#undef FDeletedIndex
#undef SET_NEVERUSED
#undef SET_DELETED

///////////////////////////////////////////////////////////////////////////////
// Attribute
// 
// provides an OBJ -> void* mapping, where all of the applicable OBJs
// were assigned keys by the same MANAGER object (the one provided to
// the constructor for the Attribute object)
// 
//     OBJ     must define   MapIndex GetAttribKey();
//     MANAGER must define   MapIndex GetMaxAttribKey();
// 
// DenseAttribute is implemented using DirectVoidPointerMap 
// (array size == manager->GetMaxAttribKey())
// while SparseAttribute is implemented using OpenHashedVoidPointerMap 
// (table size == manager->GetMaxAttribKey()/capacityFactor)
// 
// Rather than using the templates directly, Attribute classes are often
// defined using the AST_DEFINE_ATTRIBUTE_CLASSES macro, e.g.,
// 
//     AST_DEFINE_ATTRIBUTE_CLASSES(Wombat, WombatNode, WombatManager);
// 
//     void TormentWombats(WombatManager *wManager)
//     {
//         WombatAttribute frobnicity(wManager, someAllocator);
//         Wombat *w;
// 
//         w = ...
// 
//         frobnicity->Set(w, (void *)(...value...));
// 
//         ... frobnicity->Get(w) ...;
//     }


typedef MapIndex AttribKey;

template<class OBJ, class MANAGER>
class Attribute
{
  public:
    // return true iff obj has been assigned a value
    AST_TEMPLATE_API Bool   FSet(OBJ *obj) 
      { return map->FSet(obj->GetAttribKey()); }

    // return value assigned to obj; error if none
    AST_TEMPLATE_API void * Get(OBJ *obj) 
      { return map->Get(obj->GetAttribKey()); }

    // assign value to obj
    AST_TEMPLATE_API void   Set(OBJ *obj, void *val) 
      { map->Set(obj->GetAttribKey(), val); }

    // assign value to obj; error if already set
    AST_TEMPLATE_API void   SetNew(OBJ *obj, void *val) 
      { map->SetNew(obj->GetAttribKey(), val); }

    // make obj be unassigned
    AST_TEMPLATE_API virtual void UnSet(OBJ *obj)
      { map->UnSet(obj->GetAttribKey()); }

    // return value assigned to obj; defaultValue if none
    AST_TEMPLATE_API void * GetDefaulting(OBJ *obj, void *defaultValue) 
      { return map->GetDefaulting(obj->GetAttribKey(), defaultValue); }

    // returns a heap-allocated iterator; caller must delete
    AST_TEMPLATE_API VoidPointerMapIterator *NewIterator() 
      { return map->NewIterator(); }

    AST_TEMPLATE_API void Print(OBJ *obj, IndentingStream *stream)
      {
          if (FSet(obj))
          {
              void *val = Get(obj);
              if (printMethod != NULL)
                (*printMethod)(val, stream);
              else
                stream->Printf("0x%x", val);
          }
      }

  protected:
    typedef void (*PrintMethod)(void *value, IndentingStream *stream);
    AST_TEMPLATE_API Attribute(VoidPointerMap *map, PrintMethod printMethod):
        map(map), printMethod(printMethod)
      { }
    AST_TEMPLATE_API ~Attribute() {}

    VoidPointerMap *map;
    PrintMethod printMethod;
};

template<class OBJ, class MANAGER>
class DenseAttribute: public Attribute<OBJ, MANAGER>
{
  public:
    AST_TEMPLATE_API DenseAttribute(
        MANAGER *manager,
        Allocator *allocator = Allocator::heapAllocator,
        PrintMethod printMethod=NULL
      ):
        Attribute<OBJ, MANAGER>(&denseMap, printMethod),
        denseMap(manager->GetMaxAttribKey()+1, allocator)
      { }
    AST_TEMPLATE_API ~DenseAttribute() {}
  protected:
    DirectVoidPointerMap denseMap;
};

template<class OBJ, class MANAGER>
class SparseAttribute: public Attribute<OBJ, MANAGER>
{
  public:
    AST_TEMPLATE_API SparseAttribute(
        MANAGER *manager,
        Allocator *allocator = Allocator::heapAllocator,
        int capacityFactor=10,
        PrintMethod printMethod=NULL
      ):
        Attribute<OBJ, MANAGER>(&sparseMap, printMethod),
        sparseMap((manager->GetMaxAttribKey()+1)/capacityFactor, allocator)
      { }
    AST_TEMPLATE_API ~SparseAttribute() {}
  protected:
    OpenHashedVoidPointerMap sparseMap;
};

///////////////////////////////////////////////////////////////////////////////
// Attribute instances

// usage:
//   AST_DEFINE_ATTRIBUTE_CLASSES(_NAME_, NODE, MGR)
//
// defines the types
//   _NAME_Attribute       -- abstract attribute class for NODE and MGR
//   Dense_NAME_Attribute  -- implementation using DirectVoidPointerMap
//   Sparse_NAME_Attribute -- implementation using OpenHashedVoidPointerMap
// 
#define AST_DEFINE_ATTRIBUTE_CLASSES(NAME, NODE, MGR) \
 typedef Attribute<NODE, MGR> NAME##Attribute ;\
 typedef DenseAttribute<NODE, MGR> Dense##NAME##Attribute ;\
 typedef SparseAttribute<NODE, MGR> Sparse##NAME##Attribute \

// the following have been moved to node.h, symbol.h, and type.h
//AST_DEFINE_ATTRIBUTE_CLASSES(Ast,    AstNode,    AstManager);
//AST_DEFINE_ATTRIBUTE_CLASSES(Symbol, Symbol,     SymbolManager);
//AST_DEFINE_ATTRIBUTE_CLASSES(Type,   TypeRecord, TypeManager);

#endif // INC_ATTRIB_H
