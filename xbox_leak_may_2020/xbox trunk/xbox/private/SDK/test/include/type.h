// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header$
// $NoKeywords: $
//
// 
 
#ifndef INC_TYPE_H 
#define INC_TYPE_H

///////////////////////////////////////////////////////////////////////////////
// Class structure for types
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

// Type manager
class TypeManager;

class BasicType;
// Indexed types
class   Type;
// Compiler Types
class     TypeVTableShape;
// Named Types
class     TypeTypedef;
// Basic Types
class     TypePrimitive;
class     TypePointer;
class     TypeArray;
// Declared types
class     TypeDeclared;
class       TypeEnum;
class       TypeCSU;
class         TypeStructClass;
class           TypeStruct;
class           TypeClass;
class         TypeUnion;
// Function types
class     TypeFunction;
class       TypeProcedure;
class       TypeMemberFunction;
// Derived types
class     TypeModifier;
class     TypeBitField;
class     TypeDefaultArgument;
// Type lists
class     TypeList;
class       TypeArgumentList;
class       TypeFieldList;
class       TypeMethodList;
// Type list elements
class   TypeField;
class     TypeFieldBaseClass;
class       TypeFieldVirtualBaseClass;
class         TypeFieldIndirectVirtualBaseClass;
class     TypeFieldEnumerate;
class     TypeFieldFriendFunction;
class     TypeFieldFriendClass;
class     TypeFieldMember;
class       TypeFieldStaticMember;
class     TypeFieldMethod;
class     TypeFieldOneMethod;
class     TypeFieldTypeDefinition;
class       TypeFieldTypeDefinitionExt;
class     TypeFieldMemberModification;

// Type list CONS cells;
struct TypeArgumentListPair;
struct TypeFieldListPair;
struct TypeMethodListPair;

// Iterators
struct TypeArgumentListIterator;
struct TypeFieldListIterator;
struct   TypeBaseClassIterator;
struct   TypeDataFieldIterator;
struct TypeMethodListIterator;
class TypeMap;

////END////

// forward declarations

class Symbol;
class SymbolTypedef;
class SymbolManager;
typedef unsigned long SymbolKey; 
class IlPath;
class IlStream;
enum FrontEndType;
class AstBinary;
class AstUnary;
class AstNode;
class AstExpression;
class TypeMap;

//////////////////////////////////////////////////////////////////////////////

enum TypeMemberProtection { prNone, prProtected, prPrivate, prPublic };

enum MethodKind { mkPlain, mkVirtual, mkStatic, mkFriend, mkPureVirtual };

enum FieldMethodKind { fmNormal, fmConstructor, fmDestructor, fmConversion,
                       fmUnknown };

class TypeManager: public Object, public WithArena
{
  public:
    ASTAPI TypeManager(ArenaAllocator *pAllocator);
    ASTAPI ~TypeManager();
    //ASTAPI FrontEndType GetLanguage() { return language; }
    // The routines for retyping symbols must be public so retyping clients can use them.
    // DEPRECATED
    ASTAPI Type *       RetypeSymbol(Symbol *psymbol);
    ASTAPI TypeTypedef *RetypeTypeSymbol(Symbol *psymbol);
    ASTAPI static Bool  SameTypeIgnoringSpecialCases(Type *t1, Type *t2); //not for abstract types.

  internal:
    FrontEndType language;
    WithArena::pAllocator;
};

struct TypeFieldListPair
{
    TypeField *type;
    struct TypeFieldListPair *next;
};

struct TypeArgumentListPair
{
    union {
        TypeIndex typeIndex;    // Only used during AST construction
        Type *type;             // Valid when AST has been constructed
    };
    struct TypeArgumentListPair *next;
};

struct TypeMethodListPair
{
    MethodKind kind                 : 8;
    TypeMemberProtection protection : 8;
    union {
        TypeIndex typeIndex;    // Only used during AST construction
        TypeFunction *type;     // Valid when AST has been constructed
    };
    struct TypeMethodListPair *next;
};

struct TypeArgumentListIterator
{
  public:
    ASTAPI TypeArgumentListIterator(struct TypeArgumentListPair *firstPair):
        current(firstPair), ordinal(-1) { }
    // Cursor must be advanced at least once before getting a field or asking FDone.
    ASTAPI Bool   FNext();      // Advance cursor; return success status
    ASTAPI Type * NextType();   // Advance cursor; return new field
    ASTAPI Type * GetType();    // Return current field
    ASTAPI Bool   FLast();      // Will FNext() will return false?
    ASTAPI Bool   FDone();      // Has iterator been traversed?
    ASTAPI int    GetOrdinal() { return ordinal; }
  private:
    int ordinal;
    struct TypeArgumentListPair *current;
};


struct TypeFieldListIterator
{
  public:
    ASTAPI TypeFieldListIterator(struct TypeFieldListPair *firstPair):
        current(firstPair), fInitialized(FALSE) { }
    // Cursor must be advanced at least once before getting a field or asking FDone.
    ASTAPI virtual Bool        FNext();     // Advance cursor; return success
    ASTAPI virtual TypeField * NextField(); // Advance cursor; return new field
    ASTAPI         TypeField * GetField();  // Return current field
    ASTAPI virtual Bool        FLast();     // will FNext() will return false?
    ASTAPI         Bool        FDone();     // Has iterator been traversed?
  protected:
    Bool fInitialized;  // This flag handles the startup boundary condition.
                        // By convention, the iterator's client must start by advancing the conceptual cursor, but
                        // the physical cursor stays put since the list doesn't have a fake header.
    struct TypeFieldListPair *current;
};

struct TypeBaseClassIterator: public TypeFieldListIterator
{
  public:
    ASTAPI TypeBaseClassIterator(struct TypeFieldListPair *firstPair):
        TypeFieldListIterator(firstPair) { }
    // overrides
    ASTAPI Bool                 FNext();
    ASTAPI Bool                 FLast();
    ASTAPI Bool                 FDone();      // Has iterator been traversed?
    ASTAPI TypeFieldBaseClass * NextBaseClass();
};

struct TypeDataFieldIterator: public TypeFieldListIterator
{
  public:
    ASTAPI TypeDataFieldIterator(struct TypeFieldListPair *firstPair):
        TypeFieldListIterator(firstPair) { }
    // overrides
    ASTAPI Bool        FNext();
    ASTAPI Bool        FLast();
    ASTAPI Bool        FDone();      // Has iterator been traversed?
    ASTAPI TypeField * NextField();
};

struct TypeMethodListIterator
{
 public:
    ASTAPI TypeMethodListIterator(TypeMethodListPair *firstPair):
        current(firstPair), fInitialized(FALSE) {}
    // Cursor must be advanced at least once before getting a field or asking FDone.
    ASTAPI Bool                 FNext(); // Advance cursor; return success
    ASTAPI TypeFunction *       NextMethod(); // Advance cursor; return method
    ASTAPI Bool                 FLast();         // Will FNext() return false?
    ASTAPI Bool                 FDone();      // Has iterator been traversed?
    ASTAPI TypeFunction *       GetMethodType(); // Return current method
    ASTAPI TypeMemberProtection GetProtection(); // Return current protection
    ASTAPI MethodKind           GetMethodKind(); // Return current MethodKind
 private:
    Bool fInitialized;
    struct TypeMethodListPair *current;
};

class BasicType: public Object
{
  public:

  internal:
    // Initialize replaces type indices with pointers to type objects.
    virtual void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO) = 0;
    static inline void * operator new(size_t size, TypeManager *typeManager)
      { return typeManager->Alloc(size); }

  protected:
    BasicType() { }
  private:
    BasicType(BasicType&) { }
};


// Typedefs are a way of generating many types that are names for the
// same type.  A Type's PlainType is the type with no
// TypeTypedefs.  This is different from SkipTypedefs, which just
// removes the immediately accessible typedefs.
//
//  typedef int INT;
//  typedef INT *P;
//  P p;
//
//  The type of "p" (lets call it p.type) is
//     "P"-which-is  pointer-to  "INT"-which-is "int".
//     (TypeTypedef  TypePointer TypeTypedef    TypePrimitive)
//  p.type->PlainType() is pointer-to "int".
//  p.type->SkipTypedefs() is pointer-to "INT"-which-is "int".
//
//  There is an exception to this rule.  The types of struct (etc)
//  members retain their typedefs.

enum TypeKind
{
    tkPrimitive, tkVTableShape, tkPointer, tkArray, tkStruct,
    tkClass, tkUnion, tkEnum, tkProcedure, tkMemberFunction,
    tkModifier, tkBitField, tkDefaultArgument, tkTypedef,
    tkError
};

class Type: public BasicType
{
  public:
    ASTAPI TypeIndex        GetIndex()                 { return index; }
    ASTAPI virtual TypeKind GetTypeKind()              = 0;
    //
    ASTAPI         Type*    PlainType()                { return plainType; }
    ASTAPI virtual Type *   SkipModifiers()            { return this; }
    ASTAPI virtual Type *   SkipTypedefs()             { return this; }
    ASTAPI virtual Type *   SkipModifiersAndTypedefs() { return this; }

    ASTAPI virtual size_t   GetSize()                  = 0;
    ASTAPI virtual Bool     FHasSize()                 { return FALSE; }

    ASTAPI         Bool     FPlain() { return this == PlainType(); }

  public:
    // "Downcasts".
    // These member functions return NULL if the type is not of the
    // appropriate kind, otherwise they act as a downcast.

    ASTAPI virtual TypePrimitive* asPrimitive() { return 0; }
    ASTAPI virtual TypeModifier*  asModifier() { return 0; }

    // asPointer: it is a simple pointer (NOT reference or member-pointer)
    ASTAPI virtual TypePointer* asPointer()    { return 0; }
    ASTAPI virtual TypePointer* asReference()  { return 0; }
    ASTAPI virtual TypePointer* asPointerToMember()         { return 0; }
    ASTAPI virtual TypePointer* asPointerToMemberFunction() { return 0; }
    ASTAPI virtual TypePointer* asPointerToDataMember()     { return 0; }

    ASTAPI virtual TypeDeclared*    asTypeDeclared() { return 0; }
    ASTAPI virtual TypeEnum*        asEnum()         { return 0; }
    ASTAPI virtual TypeCSU*         asCSU()          { return 0; }
    ASTAPI virtual TypeUnion*       asUnion()        { return 0; }
    ASTAPI virtual TypeStructClass* asStructClass()  { return 0; }
    ASTAPI virtual TypeStruct*      asStruct()       { return 0; }
    ASTAPI virtual TypeClass*       asClass()        { return 0; }

    ASTAPI virtual TypeTypedef*     asTypedef()      { return 0; }

    ASTAPI virtual TypeArray*       asArray()        { return 0; }

    ASTAPI virtual TypeFunction*       asFunction()       { return 0; }
    ASTAPI virtual TypeProcedure*      asProcedure()      { return 0; }
    ASTAPI virtual TypeMemberFunction* asMemberFunction() { return 0; }

    ASTAPI virtual TypeBitField*    asBitField() { return 0; }

    // These never occur in practice:
    ASTAPI virtual TypeVTableShape*     asVTableShape()     { return 0; }
    ASTAPI virtual TypeDefaultArgument* asDefaultArgument() { return 0; }

    // "Missing Downcasts". There are no downcasts for TypeList,
    // TypeArgumentList etc because the list is always part of
    // something else, e.g. a TypeStruct or TypeProcedure.

  public:
    // Useful types that are always available

    ASTAPI static TypePrimitive * const  Char;
    ASTAPI static TypePrimitive * const  SChar;
    ASTAPI static TypePrimitive * const  UChar;
    ASTAPI static TypePrimitive * const  SShort;
    ASTAPI static TypePrimitive * const  UShort;
    ASTAPI static TypePrimitive * const  SInt;
    ASTAPI static TypePrimitive * const  UInt;
    ASTAPI static TypePrimitive * const  SLong;
    ASTAPI static TypePrimitive * const  ULong;
    ASTAPI static TypePrimitive * const  SInt64;
    ASTAPI static TypePrimitive * const  UInt64;
    ASTAPI static TypePrimitive * const  Float;
    ASTAPI static TypePrimitive * const  Double;
    ASTAPI static TypePrimitive * const  LDouble;
    ASTAPI static TypePrimitive * const  Void;
    ASTAPI static TypePrimitive * const  WChar;

    ASTAPI static TypePrimitive * const  NoType;   // used for ellipsis (...)
    ASTAPI static TypePrimitive * const  Unknown;  // used for templates

  public:
    ASTAPI void WriteDescription(OutputStream&,
                                 Bool includeObjectAddresses = FALSE);

  internal:
    Type(TypeIndex index);
    virtual void InstallNestedTypeLink();
    virtual void InstallUnparseName(TypeManager *typeManager);

    void DumpStandardPrefix(DumpContext *dc);

  protected:
    Type();
    TypeIndex index;
    Type *plainType;

    //friend class TypeModifier;
};


class TypeTypedef: public Type
{
  public:
    ASTAPI static TypeTypedef* Make(Symbol *typedefSymbol, Type *type,
                                    TypeManager*);
    ASTAPI TypeKind GetTypeKind()              { return tkTypedef; }
    //ASTAPI Type *   PlainType()                { return plainType; }
    ASTAPI Type *   GetType()                  { return type; }
    ASTAPI Symbol * GetSymbol()                { return typedefSymbol; }
    ASTAPI size_t   GetSize()                  { return type->GetSize(); }
    ASTAPI Bool     FHasSize()                 { return type->FHasSize(); }
    ASTAPI Type *   SkipTypedefs()             { return type->SkipTypedefs(); }
    ASTAPI Type *   SkipModifiersAndTypedefs() { return type->SkipModifiersAndTypedefs(); }
    ASTAPI TypeTypedef* asTypedef();

    ASTAPI void     DumpMethod(DumpContext *pDC);

  internal:
    void     Initialize(TypeManager *typeManager, 
                        SymbolTable *symbolTable, 
                        TypeIO *typeIO);
  private:
    // The first argument must be a SymbolTypedef or a SymbolTag.
    TypeTypedef(Symbol *typedefSymbol, Type *type);

    Symbol *typedefSymbol;  
    Type *type;  // What the typedef means.  (May be another typedef.)
};

class TypeVTableShape: public Type
{
  public:
    enum VTableShapeKind {
        vtsNear, vtsFar, vtsThin, vtsOuter,
        vtsMeta, vtsNear32, vtsFar32, vtsError
    };
    ASTAPI TypeVTableShape* asVTableShape();

    ASTAPI TypeKind          GetTypeKind()    { return tkVTableShape; }
    ASTAPI size_t            GetSize()        { return 4 * count; }
    ASTAPI Bool              FHasSize()       { return TRUE; }
    ASTAPI unsigned short    GetCount()       { return count; }
    ASTAPI VTableShapeKind * GetShapeVector() { return shapeVector; }

  protected:
    // Used while parsing types
    TypeVTableShape(TypeIndex typeIndex): Type(typeIndex) { }
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO) { }

    ASTAPI TypeVTableShape(unsigned short count, VTableShapeKind *shapeVector);
  private:
    unsigned short count;
    VTableShapeKind *shapeVector;

    friend class TypeIO;
};


enum PrimitiveKind
{
    ptChar, ptSChar, ptUChar, ptSShort, ptUShort,
    ptSInt, ptUInt, ptSLong, ptULong, ptSInt64, ptUInt64,
    ptFloat, ptDouble, ptLDouble,
    ptVoid, ptWChar,
    ptNoType,  // used for Varargs, elipsis catch expressions etc
    // workaround for frontEnd bug;
    // unresolved template parameters will be of this type --rfc
    ptUnknown
};

class TypePrimitive: public Type
{
  public:
    ASTAPI static TypePrimitive* Make(PrimitiveKind pk);
    ASTAPI TypeKind      GetTypeKind() { return tkPrimitive; }
    ASTAPI TypePrimitive* asPrimitive();
    ASTAPI Bool          FReal();
    ASTAPI Bool          FUnsigned();
    ASTAPI Bool          FSigned();
    // ASTAPI Bool       FComplex();
    // ASTAPI Bool       FBoolean();
    ASTAPI size_t        GetSize();
    ASTAPI Bool          FHasSize();
    ASTAPI PrimitiveKind GetPrimitiveKind() { return kind; }
    ASTAPI const char *  GetName();
    ASTAPI void          DumpMethod(DumpContext *pDC);

  internal:
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO) { }
    static TypePrimitive* MakeFromIndex(TypeIndex index);
    TypePrimitive(PrimitiveKind kind, TypeIndex index);

  protected:
    PrimitiveKind kind : 8;

    friend void InitializeTypes();
};
    
class TypeModifier: public Type
{
  public:
    ASTAPI virtual TypeModifier*  asModifier();

    ASTAPI static TypeModifier* Make(Bool fConst, Bool fVolatile,
                                     Bool fUnaligned, Bool fRestricted,
                                     Type *base,
                                     TypeManager*);
    ASTAPI static TypeModifier* MakeConst(Type *base, TypeManager*);

    ASTAPI TypeKind GetTypeKind()   { return tkModifier; }
    ASTAPI Type *   SkipModifiers() { return baseType->SkipModifiers(); }
    ASTAPI Type *   SkipModifiersAndTypedefs() {return baseType->SkipModifiersAndTypedefs();}
    ASTAPI Type *   GetBaseType()   { return baseType; }
    ASTAPI size_t   GetSize()       { return baseType->GetSize(); }
    ASTAPI Bool     FHasSize()      { return baseType->FHasSize(); }
    ASTAPI Bool     FConst()        { return fConst; }
    ASTAPI Bool     FVolatile()     { return fVolatile; }
    ASTAPI Bool     FUnaligned()    { return fUnaligned; }
    ASTAPI Bool     FRestricted()   { return fRestricted; }

  internal:
  private:
    static TypeModifier* Make(TypeIndex myIndex,
                              Bool fConst, Bool fVolatile,
                              Bool fUnaligned, Bool fRestricted,
                              Type *base,
                              TypeManager*);
    TypeModifier(TypeIndex baseTypeIndex, Type *baseType,
                 Bool fConst, Bool fVolatile,
                 Bool fUnaligned, Bool fRestricted,
                 TypeManager*);

    // Only used during type parsing
    TypeModifier(TypeIndex typeIndex);
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    void SetPlainType(TypeManager*);

    Bool fConst      : 1 ;
    Bool fVolatile   : 1 ;
    Bool fUnaligned  : 1 ;
    Bool fRestricted : 1 ;
    union {
        TypeIndex baseIndex;    // Only used during AST construction
        Type *baseType;         // Valid when AST has been constructed
    };

    friend class TypeIO;
};


enum PointerAttribute { paNormal, paBasedGlobal, paBasedVoid };

enum PointerKind { pkPointer, pkReference,
                   pkDataMemberPointer, pkMethodPointer };

class TypePointer: public Type
{
  public:
    ASTAPI TypeKind GetTypeKind()   { return tkPointer; }
    ASTAPI size_t   GetSize()       { return size; }
    ASTAPI Bool     FHasSize()      { return TRUE; }
    ASTAPI Bool     FVolatile()     { return fVolatile; }
    ASTAPI Bool     FConst()        { return fConst; }
    ASTAPI Bool     FUnaligned()    { return fUnaligned; }
    ASTAPI Bool     FRestricted()   { return fRestricted; }
    ASTAPI Type *   GetTargetType() { return targetType; }
    ASTAPI PointerAttribute GetPointerAttribute()
      { return pointerAttribute; }

    ASTAPI virtual TypePointer* asPointer();
    ASTAPI virtual TypePointer* asReference();
    ASTAPI virtual TypePointer* asPointerToMember();
    ASTAPI virtual TypePointer* asPointerToMemberFunction();
    ASTAPI virtual TypePointer* asPointerToDataMember();

    // "Constructors"
    ASTAPI static TypePointer *MakePointer(Type *baseType, TypeManager*);
    ASTAPI static TypePointer *MakeReference(Type *baseType, TypeManager*);
    ASTAPI static TypePointer *MakePointerToMemberFunction(Type* classType,
                                                           Type* memberType,
                                                           TypeManager*);
    ASTAPI static TypePointer *MakePointerToDataMember(Type* classType,
                                                       Type* memberType,
                                                       TypeManager*);

    ASTAPI PointerKind       GetPointerKind() { return pointerKind; }
    ASTAPI Symbol *          GetBasedGlobal(SymbolManager *symbolManager);
    ASTAPI TypeCSU*          GetMemberClass();
    ASTAPI void     DumpMethod(DumpContext *pDC);
    // DEPRECATED:
    ASTAPI TypePointer *NewTargetType(Type *newTarget, TypeManager *typeManager);  // creates pointers with new targets.

  internal:
  protected:    // Used only during type parsing
    TypePointer(TypeIndex typeIndex): Type(typeIndex) { }

    TypePointer(TypeIndex index, Type *targetType, size_t size,
                Bool fVolatile, Bool fConst,
                Bool fUnaligned, Bool fRestricted,
                PointerAttribute pointerAttribute,
                PointerKind pointerKind,
                TypeManager *typeManager);

    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    void SetPlainType(TypeManager*);

    // used in making types with index < CV_FIRST_NONPRIM
    static TypePointer *MakePointer(TypeIndex index, Type *baseType, short size, TypeManager*);

  private:
    PointerAttribute pointerAttribute : 8;
    PointerKind pointerKind           : 8;
    short size;
    Bool fVolatile   : 1;
    Bool fConst      : 1;
    Bool fUnaligned  : 1;
    Bool fRestricted : 1 ;
    union {
        TypeIndex targetTypeIndex; // Only used during AST construction
        Type *targetType;          // Valid when AST has been constructed
    };

    union {
        // Valid only when __based on a global
        struct {
            Bool fDone;
            union {
                char *name;
                Symbol *symbol;
            };
        };
        // Valid only when __based on type (e.g. __based(void))
        union {
            TypeIndex basedPtrTypeIndex;
            Type *basedPtrType;
        };
        // Valid only for normal (unbased) pointers)
        //   Pointer to data member or pointer to method
        struct {
            Int16 format;
            union {
                TypeIndex memberClassIndex; // Only used for AST construction
                TypeCSU *memberClassType; // Use this
            };
        };
    };

    friend class TypeIO;
};

class TypeArray: public Type
{
  public:
    ASTAPI static TypeArray* Make(Type *elemType, size_t length, TypeManager*);
    ASTAPI TypeArray* asArray();

    ASTAPI TypeKind GetTypeKind() { return tkArray; }
    ASTAPI size_t   GetSize()     { return size; }
    ASTAPI Bool     FHasSize()    { return TRUE; }
    ASTAPI Type *   GetElemType() { return elemType; }
    ASTAPI size_t   GetLength()   { return length; }

  internal:
  protected:
    TypeArray(TypeIndex typeIndex): Type(typeIndex) { } // Used only during type parsing
    TypeArray(TypeIndex myIndex, Type* elemType, size_t length, TypeManager*);
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    void SetPlainType(TypeManager*);
  private:
    union {
        TypeIndex elemTypeIndex; // Only used during AST construction
        Type *elemType;          // Valid when AST has been constructed
    };
    size_t size;
    size_t length;

    friend class TypeIO;
};

class TypeDeclared: public Type
{
  public:
    ASTAPI Type *                SkipModifiers()  { return this; }
    ASTAPI Type *                SkipModifiersAndTypedefs() {return this; }
    // FForwardRef() is true iff the type is only declared but never defined
    ASTAPI Bool                  FForwardRef()    { return fForwardref; }
    ASTAPI Bool                  FIsNested()      { return fIsNested; }
    ASTAPI Bool                  FIsUnnamed()     { return parseName == NULL; }
    // FIsUnnamed, e.g. enum { A=1 }; struct { int i; }
    ASTAPI const char *          GetParseName()   { return parseName; }
    ASTAPI const char *          GetUnparseName() { return unparseName; }
    ASTAPI unsigned short        GetCount()       { return count; }
    ASTAPI TypeFieldList *       GetFieldList()   { return fieldListType; }

    // Should only be called if FForwardRef() is false:
    ASTAPI TypeFieldListIterator GetFieldListIterator();

    ASTAPI char                  GetPackSize()    { return packSize; }
    ASTAPI TypeDeclared *        GetParentType()  { return parentType; }
    ASTAPI Symbol *              GetTagSymbol()   { return tagSymbol; }
    ASTAPI TypeField *           FindNamedField(char *name);

    ASTAPI virtual TypeDeclared* asTypeDeclared();

  internal:
    TypeDeclared(TypeIndex typeIndex);

    void ReadFields(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);

    void SetPackSize(char size) { packSize = size; }
    void SetParentType(TypeDeclared *declared);
    void InstallUnparseName(TypeManager *typeManager);
    void SetTagSymbol(Symbol *sy) { tagSymbol = sy; }

    virtual void DumpMethod(DumpContext *pDC);
    //virtual void DumpStandardPrefix(DumpContext *dc);

  protected:
    //ASTAPI TypeDeclared *        GetForwardType() { return forwardType; }
    TypeDeclared(Bool fForwardRef, Bool fIsNested, char packSize,
                 unsigned short fieldCount, char *parseName,
                 TypeIndex fieldListIndex);
    TypeDeclared(Bool fForwardRef, Bool fIsNested, char packSize,
                 unsigned short fieldCount, char *parseName,
                 TypeFieldList *fieldListType);

    Bool fForwardref : 1;
    Bool fIsNested   : 1;
    char packSize;
    unsigned short count;
    char *parseName;            // tag name from input program (possibly "")
    char *unparseName;          // tag name usable for output (never "")
    TypeDeclared *parentType;
    Symbol *tagSymbol;          // symbol node for this tag
    //Type *abstractType;         // abstract version of the type for anonymous structs/unions.
                                // Normally the abstract type is associated with a symbol.
    union {
        TypeIndex fieldListIndex; // Only used during AST construction
        TypeFieldList *fieldListType; // Valid only for non-forward types
        //TypeDeclared *forwardType; // "real" type for which this is a forward 
    };
  private:
};


class TypeCSU: public TypeDeclared  // class, struct or union
{
  public:
    ASTAPI TypeCSU *             asCSU();
    ASTAPI size_t                GetSize();
    ASTAPI Bool                  FHasSize();
    ASTAPI TypeDataFieldIterator GetDataFieldIterator();

  protected:
    TypeCSU(TypeIndex typeIndex): TypeDeclared(typeIndex) { }
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);

    TypeCSU(Bool fForwardRef, Bool fIsNested, char packSize,
            unsigned short fieldCount, char *parseName,
            TypeIndex fieldListIndex, size_t size);
    TypeCSU(Bool fForwardRef, Bool fIsNested, char packSize,
            unsigned short fieldCount, char *parseName,
            TypeFieldList *fieldListType, size_t size);

  internal:
    size_t length;

    friend class TypeIO;
};

class TypeStructClass: public TypeCSU
{
  public:
    ASTAPI TypeStructClass* asStructClass();

    ASTAPI TypeBaseClassIterator GetBaseClassIterator();
    ASTAPI Bool                  FHasVirtualBaseClass();
  internal:
    // Unly used during parsing
    TypeStructClass(TypeIndex typeIndex): TypeCSU(typeIndex) { }

  protected:
    TypeStructClass(Bool fForwardRef, Bool fIsNested, char packSize,
                    unsigned short fieldCount, char *parseName,
                    TypeIndex fieldListIndex, size_t size);
    TypeStructClass(Bool fForwardRef, Bool fIsNested, char packSize,
                    unsigned short fieldCount, char *parseName,
                    TypeFieldList *fieldListType, size_t size);
  internal:
    friend class TypeIO;
};

class TypeStruct: public TypeStructClass
{
  public:
    ASTAPI TypeKind  GetTypeKind() { return tkStruct; }
    ASTAPI TypeStruct* asStruct();

  protected:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeStruct(Bool fForwardRef, Bool fIsNested, char packSize,
                      unsigned short fieldCount, char *parseName,
                      TypeIndex fieldListIndex, size_t size);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeStruct(Bool fForwardRef, Bool fIsNested, char packSize,
                      unsigned short fieldCount, char *parseName,
                      TypeFieldList *fieldListType, size_t size);
    // Only used for parsing
    TypeStruct(TypeIndex typeIndex): TypeStructClass(typeIndex) { }

    friend class TypeIO;
};

class TypeClass: public TypeStructClass
{
  public:
    ASTAPI TypeKind GetTypeKind() { return tkClass; }
    ASTAPI TypeClass* asClass();

  protected:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeClass(Bool fForwardRef, Bool fIsNested, char packSize,
                     unsigned short fieldCount, char *parseName,
                     TypeIndex fieldListIndex, size_t size);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeClass(Bool fForwardRef, Bool fIsNested, char packSize,
                     unsigned short fieldCount, char *parseName,
                     TypeFieldList *fieldListType, size_t size);

    // Only used for parsing
    TypeClass(TypeIndex typeIndex): TypeStructClass(typeIndex) { }

    friend class TypeIO;
};

class TypeUnion: public TypeCSU
{
  public:
    ASTAPI TypeUnion* asUnion();
    ASTAPI TypeKind GetTypeKind() { return tkUnion; }

  internal:
  protected:
    // Only used for type parsing
    TypeUnion(TypeIndex typeIndex): TypeCSU(typeIndex) { }

    // Users of this constructor must call both Initialize and Fixup later
    TypeUnion(Bool fForwardRef, Bool fIsNested, char packSize,
              unsigned short fieldCount, char *parseName,
              TypeIndex fieldListIndex, size_t size);
    // Users of this constructor must call Fixup later (if necessary)
    TypeUnion(Bool fForwardRef, Bool fIsNested, char packSize,
              unsigned short fieldCount, char *parseName,
              TypeFieldList *fieldListType, size_t size);

    friend class TypeIO;
};

class TypeEnum: public TypeDeclared
{
  public:
    ASTAPI TypeEnum* asEnum();

    ASTAPI TypeKind GetTypeKind() { return tkEnum; }
    ASTAPI size_t   GetSize();
    ASTAPI Bool     FHasSize();
    ASTAPI Type *   GetBaseType() { return baseType; }
  internal:
  protected:
    // Only used for type parsing
    TypeEnum(TypeIndex typeIndex): TypeDeclared(typeIndex) { }

    // Users of this constructor must call both Initialize and Fixup later
    TypeEnum(Bool fForwardRef, Bool fIsNested, char packSize,
             unsigned short fieldCount, char *parseName,
             TypeIndex fieldListIndex, 
             TypeIndex baseTypeIndex);
    // Users of this constructor must call Fixup later (if necessary)
    TypeEnum(Bool fForwardRef, Bool fIsNested, char packSize,
             unsigned short fieldCount, char *parseName,
             TypeFieldList *fieldListType,
             Type *baseType);

    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);

  private:
    union {
        TypeIndex baseTypeIndex; // Only used during AST construction
        Type *baseType;         // Valid when AST has been constructed
    };

    friend class TypeIO;
};

enum CallingConvention
{
    ccC, ccPascal, ccFastCall, ccStdCall, ccSysCall,
    ccThisCall, ccMIPSCall, ccAlphaCall, ccPPCCall,
    ccGenericCall
};


// TypeFunction represents both ordinary functions and member functions
class TypeFunction: public Type
{
  public:
    ASTAPI TypeFunction*     asFunction();
    ASTAPI size_t            GetSize(); // Will cause an error if called
    ASTAPI Type *            GetReturnType()        { return returnType; }
    ASTAPI CallingConvention GetCallingConvention() { return callingConvention; }
    ASTAPI unsigned short    GetArgumentCount()     { return parameterCount; }
    ASTAPI TypeArgumentList *GetArgumentListType()  { return parameterListType; }
    ASTAPI TypeArgumentListIterator GetArgumentIterator();
    ASTAPI Bool              FVarArgs();

  internal:
    TypeFunction(TypeIndex typeIndex): Type(typeIndex) { } // Only used while parsing types
  protected:
    TypeFunction(TypeIndex returnTypeIndex,
                 CallingConvention callingConvention,
                 TypeIndex parameterListIndex, int parameterCount);
                 // General use constructor
    TypeFunction(Type *returnType, CallingConvention callingConvention,
                 TypeArgumentList *parameterList, int parameterCount);
                 // General use constructor
    union {
        TypeIndex returnTypeIndex;
        Type *returnType;
    };
    CallingConvention callingConvention : 8;
    unsigned short parameterCount;
    union {
        TypeIndex parameterListIndex;
        TypeArgumentList *parameterListType;
    };
};

class TypeProcedure: public TypeFunction
{
  public:
    ASTAPI TypeProcedure* asProcedure();
    ASTAPI TypeKind GetTypeKind() { return tkProcedure; }

  internal:
  protected:
     // Only used while parsing
    TypeProcedure(TypeIndex typeIndex): TypeFunction(typeIndex) { }

    TypeProcedure(TypeIndex myIndex, Type *returnType,
                  CallingConvention callingConvention,
                  TypeArgumentList *parameterList, int parameterCount,
                  TypeManager*);

    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    void SetPlainType(TypeManager*);

    friend class TypeIO;
};

class TypeMemberFunction: public TypeFunction
{
  public:
    ASTAPI TypeKind          GetTypeKind() { return tkMemberFunction; }
    ASTAPI TypeMemberFunction *asMemberFunction();
    ASTAPI TypeCSU*          GetClass()    { return classType; }
    ASTAPI Type *            GetThisType() { return thisType; }
    ASTAPI Bool              FConst();
    ASTAPI Bool              FVolatile();
    ASTAPI Bool              FUnaligned();
  internal:
  protected:
    // Only used while parsing
    TypeMemberFunction(TypeIndex typeIndex): TypeFunction(typeIndex) { }

    TypeMemberFunction(TypeIndex myIndex, Type *returnType,
                       CallingConvention callingConvention,
                       TypeArgumentList *parameterList,
                       int parameterCount,
                       TypeCSU *classType, Type *thisType,
                       TypeManager*);

    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    void SetPlainType(TypeManager*);

  private:
    union {
        TypeIndex classTypeIndex;
        TypeCSU *classType;
    };
    union {
        TypeIndex thisTypeIndex;
        Type *thisType;
    };

    friend class TypeIO;
};

class TypeBitField: public Type
{
  public:
    static ASTAPI TypeBitField *Make(Int8 length, Int8 startPosition,
                                     Type* fieldType,
                                     TypeManager *typeManager);
    ASTAPI TypeBitField* asBitField();
    ASTAPI TypeKind GetTypeKind() { return tkBitField; }
    ASTAPI size_t   GetSize()     { return fieldType->GetSize(); }
    ASTAPI Bool     FHasSize()    { return fieldType->FHasSize(); }
    ASTAPI Int8     GetLength()   { return length; }
    ASTAPI Int8     GetPosition() { return startPosition; }
    ASTAPI Type *   GetBaseType() { return fieldType; }

  private:
    TypeBitField(TypeIndex typeIndex): Type(typeIndex) { }
    //TypeBitField(Int8 length, Int8 startPosition, TypeIndex fieldTypeIndex);

    TypeBitField(Int8 length, Int8 startPos, Type *fieldType, TypeManager*);
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    void SetPlainType(TypeManager* typeManager);

    Int8 length;
    Int8 startPosition;
    union {
        TypeIndex fieldTypeIndex;
        Type *fieldType;
    };

    friend class TypeIO;
};

class TypeDefaultArgument: public Type
{
  public:
    ASTAPI TypeDefaultArgument* asDefaultArgument();
    ASTAPI TypeKind GetTypeKind()     { return tkDefaultArgument; }
    ASTAPI size_t   GetSize();
    ASTAPI Bool     FHasSize();
    ASTAPI Type *   GetType()         { return argumentType; }
    ASTAPI char *   GetDefaultValue() { return expression; }

  protected:
    TypeDefaultArgument(TypeIndex typeIndex);    // Only used while parsing

    //ASTAPI TypeDefaultArgument(TypeIndex argumentTypeIndex,
    //                           char *expression);
    //ASTAPI TypeDefaultArgument(Type *argumentType, char *expression);
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);

  private:
    union {
        TypeIndex argumentTypeIndex;
        Type *argumentType;
    };
    char *expression;

    friend class TypeIO;
};


class TypeList: public Type
{
  public:
    ASTAPI TypeKind        GetTypeKind() { return tkError; }
    ASTAPI size_t          GetSize(); // Will cause an error if called
    ASTAPI unsigned long   GetCount()    { return count; }
  protected:
     // Only used while parsing types
    TypeList(TypeIndex typeIndex): Type(typeIndex) { }
    TypeList(int count);
    unsigned long count;
};


class TypeArgumentList: public TypeList
{
  public:
    ASTAPI Bool          FVarArgs() { return fVarArgs; }
    // Note: not pointer to iterator; permits stack allocation of structure:
    ASTAPI TypeArgumentListIterator GetIterator()
      { return TypeArgumentListIterator(firstPair); }

  protected:
    // Only used while parsing types
    TypeArgumentList(TypeIndex typeIndex): TypeList(typeIndex) { }

    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);

    TypeArgumentList(int count, struct TypeArgumentListPair *firstPair,
                     Bool fVarArgs);
  private:
    struct TypeArgumentListPair *firstPair;
    Bool fVarArgs;

    friend class TypeIO;
};

class TypeFieldList: public TypeList
{
  public:
    ASTAPI TypeFieldList(int count, struct TypeFieldListPair *firstPair);
    ASTAPI void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    // Note: not pointer to iterator; permits stack allocation of structure:
    ASTAPI TypeFieldListIterator GetIterator()
      { return TypeFieldListIterator(firstPair); }
    ASTAPI TypeBaseClassIterator GetBaseClassIterator()
      { return TypeBaseClassIterator(firstPair); }
    ASTAPI TypeDataFieldIterator GetDataFieldIterator()
      { return TypeDataFieldIterator(firstPair); }
    ASTAPI TypeField *FindNamedField(char *name);
  internal:
    TypeFieldList(TypeIndex typeIndex);

    struct TypeFieldListPair *GetList() { return firstPair; }
    void SetBackLink(TypeDeclared *typeDeclared);
    void InstallNestedTypeLink();
    void DestroyList() { firstPair = NULL; count = 0; }
  private:
    TypeIndex continuationIndex;
    struct TypeFieldListPair *firstPair;
    TypeDeclared *backLink;   // to "owner"

    friend class TypeIO;
};

class TypeMethodList: public TypeList
{
  public:
    ASTAPI TypeMethodList(int count, struct TypeMethodListPair *firstPair);
    ASTAPI void   Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeMethodListIterator GetIterator()
      { return TypeMethodListIterator(firstPair); }
  internal:
    // Only used while parsing types
    TypeMethodList(TypeIndex typeIndex): TypeList(typeIndex) { }

  private:
    struct TypeMethodListPair *firstPair;

    friend class TypeIO;
};

enum TypeFieldKind
{
    tfkBase, tfkVirtualBase, tfkIndirectVirtualBase, tfkEnumerate,
    tfkFriendFunction, tfkFriendClass, tfkMember, tfkStaticMember,
    tfkMethod, tfkOneMethod, tfkType, tfkMemberModification, tfkError
};

class TypeField: public BasicType
{
  public:
    ASTAPI virtual TypeFieldKind GetFieldKind() = 0;
  // These next two virtual functions are so Daniel can avoid multiple random downcasts.
    ASTAPI virtual const char *  GetName()      { return NULL; } // Typefields with no name return NULL
    ASTAPI virtual Type *        GetType()      { 
      USER_ERROR("\nNo Type for this TypeField of kind %i.", this->GetFieldKind());
      return NULL; } // shut up the stupid compiler.
  internal:
    void DumpMethod(DumpContext *pDC);
  protected:
    ASTAPI TypeField();
};

class TypeFieldBaseClass: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldBaseClass(TypeIndex classTypeIndex);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldBaseClass(TypeStructClass *classType, TypeMemberProtection protection);
    ASTAPI void                 Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind        GetFieldKind()  { return tfkBase; }
    ASTAPI TypeStructClass *    GetClassType()  { return classType; }
    ASTAPI TypeMemberProtection GetProtection() { return protection; }
  internal:
    TypeFieldBaseClass(): TypeField() { } // Used only when parsing types

  protected:
    union {
        TypeIndex classTypeIndex;
        TypeStructClass *classType;
    };
    TypeMemberProtection protection : 8;

    friend class TypeIO;
};

class TypeFieldVirtualBaseClass: public TypeFieldBaseClass
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldVirtualBaseClass(TypeIndex classTypeIndex,
                                     TypeIndex classPointerTypeIndex);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldVirtualBaseClass(TypeStructClass *classType,
                                     Type *classPointerType,
                                     TypeMemberProtection protection);
    ASTAPI void          Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind GetFieldKind()        { return tfkVirtualBase; }
    ASTAPI Type *        GetClassPointerType() { return classPointerType; }
  internal:
    TypeFieldVirtualBaseClass(): TypeFieldBaseClass() { } // Only for parsing

  private:
    union {
        TypeIndex classPointerTypeIndex;
        Type *classPointerType;
    };

    friend class TypeIO;
};

class TypeFieldIndirectVirtualBaseClass: public TypeFieldVirtualBaseClass
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldIndirectVirtualBaseClass(TypeIndex classTypeIndex,
                                             TypeIndex classPointerTypeIndex);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldIndirectVirtualBaseClass(TypeStructClass *classType,
                                             Type *classPointerType,
                                             TypeMemberProtection protection);
    ASTAPI TypeFieldKind GetFieldKind() { return tfkIndirectVirtualBase; }
  internal:
    TypeFieldIndirectVirtualBaseClass(): // Only for use when parsing types
        TypeFieldVirtualBaseClass() { }
};

class TypeFieldEnumerate: public TypeField
{
  public:
    ASTAPI TypeFieldEnumerate(Int64 value, char *name);
    ASTAPI void          Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO) { }
    ASTAPI TypeFieldKind GetFieldKind() { return tfkEnumerate; }
    ASTAPI Int64         GetValue()     { return value; }
    ASTAPI const char *  GetName()      { return name; }
  internal:
    TypeFieldEnumerate(): TypeField() { } // Only for use when parsing types

  private:
    Int64 value;
    char *name;

    friend class TypeIO;
};

class TypeFieldFriendFunction: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldFriendFunction(TypeIndex friendFunctionIndex);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldFriendFunction(TypeFunction* friendFunctionType);
    ASTAPI void           Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind  GetFieldKind()    { return tfkFriendFunction; }
    ASTAPI TypeFunction * GetFunctionType() { return friendFunctionType;}
    ASTAPI const char *   GetName()         { return name; }
  internal:
    TypeFieldFriendFunction(): TypeField() { } // Only use when parsing types

  private:
    union {
        TypeIndex friendFunctionIndex;
        TypeFunction *friendFunctionType;
    };
    char *name;

    friend class TypeIO;
};
    
class TypeFieldFriendClass: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldFriendClass(TypeIndex friendClassIndex);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldFriendClass(TypeCSU *friendClassType);
    ASTAPI void              Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind     GetFieldKind() { return tfkFriendClass; }
    ASTAPI TypeCSU*          GetClassType() { return friendClassType; }
    ASTAPI const char *      GetName()      { return friendClassType->GetUnparseName(); }
  internal:
    TypeFieldFriendClass(): TypeField() { } // Only for use when parsing types

  private:
    union {
        TypeIndex friendClassIndex;
        TypeCSU *friendClassType;
    };

    friend class TypeIO;
};

class TypeFieldMember: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldMember(TypeIndex memberIndex,
                           TypeMemberProtection protection,
                           size_t offset, char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldMember(Type* memberType,
                           TypeMemberProtection protection,
                           size_t offset, char *name);
    ASTAPI void                 Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind        GetFieldKind()  { return tfkMember; }
    ASTAPI Type *               GetType()       { return memberType; }
    ASTAPI TypeMemberProtection GetProtection() { return protection; }
    ASTAPI size_t               GetOffset()     { return offset; }
    ASTAPI const char *         GetName()       { return name; }
  internal:
    TypeFieldMember(): TypeField() { } // Only use when parsing types
    void SetType(Type *ty) { memberType = ty; }

  protected:
    union {
        TypeIndex memberIndex;
        Type *memberType;
    };
    TypeMemberProtection protection : 8;
    size_t offset;
    char *name;

    friend class TypeIO;
};

class TypeFieldStaticMember: public TypeFieldMember
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldStaticMember(TypeIndex memberIndex,
                                 TypeMemberProtection protection,
                                 size_t offset, char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldStaticMember(Type* memberType,
                                 TypeMemberProtection protection,
                                 size_t offset, char *name);
    ASTAPI TypeFieldKind GetFieldKind() { return tfkStaticMember; }
  internal:
    TypeFieldStaticMember(): TypeFieldMember() { } // For parsing types only
};

class TypeFieldMethod: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldMethod(unsigned short count, TypeIndex methodListIndex,
                           char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldMethod(unsigned short count, TypeMethodList *methodList,
                           char *name);
    ASTAPI void               Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind      GetFieldKind()  { return tfkMethod; }
    ASTAPI unsigned short     GetCount()      { return count; }
    ASTAPI TypeMethodList *   GetMethodList() { return methodListType; }
    ASTAPI const char *       GetName()       { return name; }
    ASTAPI FieldMethodKind    GetFieldMethodKind();
    ASTAPI TypeMethodListIterator GetMethodListIterator()
      { return methodListType->GetIterator(); }
  internal:
    TypeFieldMethod(): TypeField() { } // Only use when parsing types
    TypeIndex GetMethodListIndex() { return methodListIndex; }
    void SetType(Type *ty) { methodListType = ((TypeMethodList *)ty); }

  private:
    unsigned short count;
    union {
        TypeIndex methodListIndex;
        TypeMethodList *methodListType;
    };
    char *name;

    friend class TypeIO;
};

class TypeFieldOneMethod: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldOneMethod(TypeMemberProtection protection,
                              MethodKind methodKind,
                              TypeIndex functionTypeIndex,
                              char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldOneMethod(TypeMemberProtection protection,
                              MethodKind methodKind,
                              TypeFunction* functionType, char *name);
    ASTAPI void                 Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind        GetFieldKind()  { return tfkOneMethod; }
    ASTAPI TypeMemberProtection GetProtection() { return protection; }
    ASTAPI MethodKind           GetMethodKind() { return methodKind; }
    ASTAPI TypeFunction *       GetMethodType() { return functionType; }
    ASTAPI const char *         GetName()       { return name; }
    ASTAPI FieldMethodKind      GetFieldMethodKind();
  internal:
    TypeFieldOneMethod(): TypeField() { } // Use when parsing types only
    void SetType(Type *ty) { functionType = ((TypeFunction *)ty); }
    TypeIndex GetFunctionTypeIndex() { return functionTypeIndex; }

  private:
    TypeMemberProtection protection : 8;
    MethodKind methodKind           : 8;
    union {
        TypeIndex functionTypeIndex;
        TypeFunction *functionType;
    };
    char *name;

    friend class TypeIO;
};

class TypeFieldTypeDefinition: public TypeField
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldTypeDefinition(TypeIndex nestedTypeIndex, char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldTypeDefinition(Type *nestedType, char *name);
    ASTAPI void                 Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);
    ASTAPI TypeFieldKind        GetFieldKind()  { return tfkType; }
    ASTAPI Type *               GetType()       { return nestedType; }
    ASTAPI TypeMemberProtection GetProtection() { return protection; }
    ASTAPI const char *         GetName()       { return name; }
  internal:
    TypeFieldTypeDefinition(): TypeField() { } // For type parsing only

  protected:
    TypeMemberProtection protection : 8;
    union {
        TypeIndex nestedTypeIndex;
        Type *nestedType;
    };
    char *name;

    friend class TypeIO;
};

class TypeFieldTypeDefinitionExt: public TypeFieldTypeDefinition
{
  public:
    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldTypeDefinitionExt(TypeMemberProtection protection,
                                      TypeIndex nestedTypeIndex, char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldTypeDefinitionExt(TypeMemberProtection protection,
                                      Type *nestedType, char *name);
  internal:
    TypeFieldTypeDefinitionExt(): TypeFieldTypeDefinition() { } // Parsing only

    friend class TypeIO;
};

class TypeFieldMemberModification: public TypeField
{
  public:
    ASTAPI TypeFieldKind        GetFieldKind()     { return tfkMemberModification; }
    ASTAPI TypeStructClass *    GetBaseClass()     { return baseClassType; }
    ASTAPI TypeMemberProtection GetNewProtection() { return protection; }
    ASTAPI const char *         GetName()          { return name; }
  internal:
  private:
    TypeFieldMemberModification(): TypeField() { } // For type parsing only

    // Users of this constructor must call both Initialize and Fixup later
    ASTAPI TypeFieldMemberModification(TypeIndex baseClassIndex,
                                       TypeMemberProtection protection,
                                       char *name);
    // Users of this constructor must call Fixup later (if necessary)
    ASTAPI TypeFieldMemberModification(TypeStructClass *baseClassType,
                                       TypeMemberProtection protection,
                                       char *name);
    void Initialize(TypeManager *typeManager, SymbolTable *symbolTable, TypeIO *typeIO);

  private:
    union {
        TypeIndex baseClassIndex;
        TypeStructClass *baseClassType;
    };
    TypeMemberProtection protection : 8;
    char *name;

    friend class TypeIO;
};



class  TypeMap : public Object {
  public:
    //static TypeMap* Make(Allocator*);
    Type *Lookup(TypeIndex, Symbol*);  // returns NULL for no match
    void Set(TypeIndex, Symbol*, Type*);
  protected:
    TypeMap();
};

#endif INC_TYPE_H 

