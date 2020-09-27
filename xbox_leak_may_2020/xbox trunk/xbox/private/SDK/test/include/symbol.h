// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/symbol.h 5     5/09/96 1:25p Erikruf $
// $NoKeywords: $
//
// 


#ifndef INC_SYMBOL_H 
#define INC_SYMBOL_H

// only one of these may be set on any given symbol; we assign each
// a unique bit value so we can OR them together to specify sets
// of SymbolKinds in searches.
enum SymbolKind {
    skVariable     = 0x001,   // variables, parameters, static data members
    skField        = 0x002,   // data member
    skTypedef      = 0x004,
    skTag          = 0x008,   // class, struct, union, enum
    skFilename     = 0x010,
    skFunction     = 0x020,
    skImport       = 0x040,
    skWeakImport   = 0x080,
    skEnumConstant = 0x100,
    skLabel        = 0x200,
    skCTemplate    = 0x400,   // class template
    skFTemplate    = 0x800,   // function template

    // mask for all of the above
    skAny          = 0xfff
};

typedef int SymbolKindMask;
typedef unsigned long SymbolKey; 
class TypeMap;

///////////////////////////////////////////////////////////////////////////////
// Class structure for symbols
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

class Symbol;
class   SymbolVariable;
class   SymbolField;
class   SymbolTypedef;
class   SymbolTag;
class   SymbolLabel;
class   SymbolCommonFunctionStuff;
class     SymbolFunction;
class     SymbolImport;
class       SymbolWeakImport;
class   SymbolEnumConstant;
class   SymbolFilename;
class   SymbolCTemplate;
class   SymbolFTemplate;

class SymbolList;
struct SymbolListIterator;

class SymbolTable;
struct SymbolTableEntryIterator;
struct SymbolTableMaskedEntryIterator;
struct SymbolTableChildIterator;
struct SymbolTableTreeIterator;

class SymbolManager;
struct ModuleFunctionIterator;

////END////

AST_DEFINE_ATTRIBUTE_CLASSES(Symbol, Symbol, SymbolManager);

///////////////////////////////////////////////////////////////////////////////
// Symbol Tables

struct SSR;
class AstBlock;
class AstManager;

typedef OpenHashedVoidPointerMap SymbolMap;
typedef OpenHashedVoidPointerMapIterator SymbolMapIterator;

const int GlobalScopeLevel = 0;

class SymbolTable: public Object
{
  public:
    ASTAPI SymbolTable * GetParent()   { return parent; }
    ASTAPI int           GetLevel()    { return level; }
    ASTAPI Bool          FGlobal()     { return level == GlobalScopeLevel; }
    ASTAPI AstBlock *    GetAstBlock() { return astBlock; }
    ASTAPI Symbol *      LookupName(char *name, Bool fDecorated=FALSE, SymbolKindMask mask=skAny);
    ASTAPI Symbol *      LookupNameLocally(char *name, Bool fDecorated=FALSE, SymbolKindMask mask = skAny);
    ASTAPI void          AddSymbol(Symbol *symbol, SymbolManager *symbolManager);
    // ^^ the key is in the symbol. resolvetypes has gone away.
    ASTAPI SymbolTableChildIterator       GetChildIterator();
    ASTAPI SymbolTableTreeIterator        GetTreeIterator();
    ASTAPI SymbolTableEntryIterator       GetEntryIterator();
    ASTAPI SymbolTableEntryIterator       GetTagTypeIterator();
    ASTAPI SymbolTableMaskedEntryIterator GetMaskedEntryIterator(SymbolKindMask mask, Bool fIncludeCGSymbols = FALSE);
    ASTAPI void Print(OutputStream *stream, Bool fComment=FALSE, SymbolAttribute *attribute = NULL);
    // overrides
    ASTAPI virtual void DumpMethod(DumpContext *pDC);
    ASTAPI static void * operator new(size_t size, SymbolManager *symbolManager);

  internal:
    Symbol *      LookupSymbol(SymbolKey symbolKey);
    Symbol *      LookupSymbolLocally(SymbolKey symbolKey);
    SymbolTable * GetNext()                       { return nextScope; }
    void          SetAstBlock(AstBlock *astBlock) { this->astBlock = astBlock; }

    void Parse(IlPath *ilPath, 
               AstManager *astManager,
               TypeManager *typeManager,
               TypeIO *typeIO,
               SymbolManager *symbolManager);
    int ParseStreams(AstManager *astManager,
                     TypeManager *typeManager,
                     TypeIO *typeIO,
                     SymbolManager *symbolManager,
                     SymbolTable **labelsTableHandle,
                     IlStream *ilStream,
                     int functionLexicalLevel);
    void PrintInternal(IndentingStream *stream, SymbolAttribute *attribute);
    //Functions which Ralph added
    void DumpSymbols(SymbolManager *, TypeIO *, char *);
    void IterateDopeVector (int, TypeIO *, IlStream *, Symbol *, SymbolManager *);
    int IterateFunctionSymbolTable (SymbolTable *, TypeIO *,IlStream *); 

    // used to 'scope' unscoped pdb types
    void  SetType(TypeIndex, Symbol *declaredSymbol, Type*);
    Type* FindTypeLocally(TypeIndex index, Symbol *declaredSymbol);
    TypeMap *typeMap;  // TypeIndex -> Type*
    Type *FindType(TypeIndex index, Symbol *declaredSymbol);

  private: // private state
    SymbolTable(SymbolTable *parent, int level);
    friend struct SymbolTableChildIterator;
    friend struct SymbolTableTreeIterator;
    friend struct SymbolTableEntryIterator;
    friend struct SymbolTableMaskedEntryIterator;
    friend class SymbolManager;

    static SymbolTable *lastScope; // dynamically changed by parser

    SymbolTable *parent;
    SymbolMap *symbolMap;
    SymbolTable *nextScope;
    int level;
    AstBlock *astBlock;
};

///////////////////////////////////////////////////////////////////////////////
// Symbols

class AstNode;
class AstFunction;

//
// Base class for symbols (not instantiable)                        
//
class Symbol: public Object
{
  public:
    ASTAPI SymbolKind    GetKind()          { return symbolKind; }
    ASTAPI SymbolKey     GetKey()           { return symbolKey; }
    ASTAPI AttribKey     GetAttribKey()     { return (AttribKey)symbolKey; }
    ASTAPI Type *        GetType()          { return type; }
    //ASTAPI Type *        GetForwardType()   { return forwardType; }
    ASTAPI Bool          FCompilerGenerated() { return fCompilerGenerated; }
    ASTAPI Bool          FToolGenerated()   { return fToolGenerated; }

    // Every symbol has both a name and a decorated name.
    // They may be the same.
    ASTAPI char *        GetName()          { return name; }
    ASTAPI char *        GetDecoratedName() { return decoratedName; }
    ASTAPI char *        GetLintSpec()      { return lintSpec; }
    ASTAPI SFA *         GetSFA()           { return &sfa; }
    ASTAPI AstNode *     GetDeclaration()   { return declaration; }
    ASTAPI SymbolTable * GetScope()         { return scope; }
    ASTAPI void          DumpRecord(TypeIO *, IlStream *);

    // The visited flag
    ASTAPI Bool          GetVisited()       { return fVisited; }
    ASTAPI void          SetVisited()       { fVisited = TRUE; }
    ASTAPI void          ClearVisited()     { fVisited = FALSE; }

    // A symbol has an owning class (or struct, union, enum or
    // template class) if it names a member or definition local to
    // that "owner".  If the symbol is not "owned", e.g. a plain
    // function or a function parameter, GetOwningClass returns NULL.
    ASTAPI SymbolTag *   GetOwningClass()   { return owningClass; }

    // Downcasts/predicates
    //  (Implementation at end)
    ASTAPI SymbolVariable*  asSymbolVariable();
    ASTAPI SymbolField*     asSymbolField();
    ASTAPI SymbolTypedef*   asSymbolTypedef();
    ASTAPI SymbolTag*       asSymbolTag();
    ASTAPI SymbolLabel*     asSymbolLabel();
    ASTAPI SymbolCommonFunctionStuff*  asSymbolCommonFunctionStuff();
    ASTAPI SymbolFunction*  asSymbolFunction();
    ASTAPI SymbolImport*    asSymbolImport();
    ASTAPI SymbolWeakImport*  asSymbolWeakImport();
    ASTAPI SymbolEnumConstant*  asSymbolEnumConstant();
    ASTAPI SymbolFilename*   asSymbolFilename();
    ASTAPI SymbolCTemplate*  asSymbolCTemplate();
    ASTAPI SymbolFTemplate*  asSymbolFTemplate();

    // overrides
    ASTAPI virtual void  DumpMethod(DumpContext *pDC);
    ASTAPI static void * operator new(size_t size, SymbolManager *symbolManager);
  internal:

    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager,  TypeIO *typeIO) = 0;
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager,
                            TypeIO *typeIO, SymbolKey declaredTypeKey);
    virtual void PrintMethod(IndentingStream *stream) = 0;
    void         Print(IndentingStream *stream, SymbolAttribute *attribute);
    void         SetDeclaration(AstNode *decl) { declaration = decl; }
    void         SetScope(SymbolTable *sc)  { scope = sc; }
    void SetType(Type *ty) { type = ty; }
    //void SetForwardType(Type *ty) {forwardType = ty; }
    //Type *ResolveSymbolType(TypeManager *, TypeIndex, TypeIO *, SymbolTable *);
    TypeIndex GetTypeIndex() { return typeIndex; }
    TypeIndex GetForwardTypeIndex() { return forwardTypeIndex; }

  protected:
    Symbol(SymbolManager *symbolManager, SymbolKind sk, Type *type, char *name,
           char *decoratedName, SymbolTag *owningClass, SFA sfa, char *lintSpec);
    Symbol(SymbolKind sk, SSR *ssr); // constructor used while parsing IL

    SymbolKind symbolKind   : 16;

    Bool fCompilerGenerated : 1 ;
    Bool fToolGenerated     : 1 ;
    Bool fVisited           : 1 ;
    Bool fInitialized       : 1 ;  //needed for SymbolTypedefs when reading PDB

    SymbolKey symbolKey;
    union {
        TypeIndex typeIndex;    // Only used during AST construction
        Type *type;             // Valid when AST has been constructed
    };
    TypeIndex forwardTypeIndex; // Only used during AST construction
    char *name;
    char *decoratedName;        // often same as name (e.g. in C code)
    char *lintSpec;
    SFA sfa;

    AstNode *declaration;
    SymbolTable *scope;
    union
    {
        SymbolKey   owningClassKey;
        SymbolTag*  owningClass;    // NULL if symbol is not a member of a ESU
    };

    friend class SymbolManager;
    friend class SymbolTypedef;
    friend class TypeIO;
};


// Mix-in for symbols which have a "declared type", i.e. a typedef
// symbol which occured in the declaration

class WithDeclaredType
{
  protected:
    WithDeclaredType(SymbolKey key) : declaredTypeKey(key) {};
    WithDeclaredType(Symbol* symbol) : declaredType(symbol) {};
    WithDeclaredType() {};
    union
    {
        SymbolKey declaredTypeKey;
        Symbol *      declaredType;
    };
};

// Answers the question "In what function is this symbol defined".
// Returns the Ast tree for the function in which the symbol is
// defined.  Returns NULL for symbols not defined in some function
// (e.g. global symbols).

ASTAPI AstFunction* AstFunctionWhereSymbolIsDefined(Symbol*);

//
// Represents SSR_NAME, SSR_ARRAY
//
class SymbolVariable: public Symbol, WithDeclaredType
{
  public:
    ASTAPI SymbolVariable(SymbolManager *symbolManager, Type *type,
                          char *name, Bool fGlobal, Bool fFormal, Bool fLocal,
                          char *decoratedName = NULL,
                          SymbolTag *owningClass = NULL,
                          Symbol *declaredType = NULL, Bool fDefined = FALSE,
                          Bool fRegister = FALSE, Bool fThreadStorage = FALSE,
                          Bool fInstantiate = FALSE, Bool fComdate = FALSE,
                          Bool fDllImport = FALSE, Bool fDllExport = FALSE,
                          Bool fStatic = FALSE, 
                          SFA sfa = SFANULL, char *lintSpec = NULL);
    // Miscellaneous info
    ASTAPI Bool        FRegister()       { return fRegister; }
    ASTAPI Bool        FThreadStorage()  { return fThreadStorage; }
    ASTAPI Bool        FInstantiate()    { return fInstantiate; } //COMDAT only
    ASTAPI Bool        FComdat()         { return fComdat; } // COMDAT symbol
    ASTAPI Bool        FDll()            { return fDllImport || fDllExport; }
    ASTAPI Bool        FDllImport()      { return fDllImport; }
    ASTAPI Bool        FDllExport()      { return fDllExport; }
    ASTAPI Bool        FStatic()         { return fStatic; }
    ASTAPI Bool        FDefined()        { return fDefined; }
    ASTAPI Bool        FGlobal()         { return fGlobal; }
    ASTAPI Bool        FFormal()         { return fFormal; }
    ASTAPI Bool        FLocal()          { return fLocal; }
    ASTAPI Bool        FStaticMember()   { return (owningClass != NULL); }
    ASTAPI Symbol *    GetDeclaredType() { return declaredType; }
  internal:
    SymbolVariable(SSR *ssr);        // Standard constructor used while parsing
    SymbolVariable(SSR *ssr, SymbolVariable *symbolVariable); // for COMEQU
    SymbolVariable   * GetAliasSymbol() { return aliasSymbol;} // for COMEQU
    void               SetAliasSymbol(SymbolVariable *); // for COMEQU
    virtual void PrintMethod(IndentingStream *stream);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
    Bool        FClassLoc()         { return fClassLoc; }
  private:
    Bool fRegister      : 1;
    Bool fThreadStorage : 1;
    Bool fInstantiate   : 1;
    Bool fComdat        : 1;
    Bool fDllImport     : 1;
    Bool fDllExport     : 1;
    Bool fStatic        : 1;
    Bool fDefined       : 1;
    Bool fGlobal        : 1;
    Bool fFormal        : 1;
    Bool fLocal         : 1;
    Bool fClassLoc      : 1;  // need to disambiguate local statics
                              // promoted to global scope
    SymbolVariable * aliasSymbol;
};

//
// Represents SSR_FIELD
//
class SymbolField: public Symbol, WithDeclaredType
{
  public:
    ASTAPI SymbolField(SymbolManager *symbolManager, Type *type, char *name,
                       Symbol *declaredType, SymbolTag *owningClass = NULL,
                       char *decoratedName = NULL, SFA sfa = SFANULL, 
                       char *lintSpec = NULL);
    ASTAPI Symbol *   GetDeclaredType() { return declaredType; }
  internal:
    SymbolField(SSR *ssr);   // Standard constructor used while parsing
    virtual void PrintMethod(IndentingStream *stream);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
  private:
    friend class SymbolManager;

    // Note: while static fields are a bit like ordinary variables, we
    // don't need the Alias/COMEQU feature since C++ forbids static
    // data members in locally defined classes.
};

//
// Represents SSR_TYPEDEF
//

class SymbolTypedef: public Symbol, WithDeclaredType
{
  public:
    ASTAPI SymbolTypedef(SymbolManager *symbolManager,
                         Type *type,
                         char *name,
                         Symbol *declaredType = NULL, 
                         char *decoratedName = NULL, 
                         SymbolTag *owningClass = NULL,
                         SFA sfa = SFANULL,
                         char *lintSpec = NULL);
    ASTAPI Symbol *      GetDeclaredType() { return declaredType; }
  internal:
    SymbolTypedef(SSR *ssr); // Standard constructor used while parsing
    virtual void PrintMethod(IndentingStream *stream);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
    //void SetDeclaredType(Symbol * sy) {declaredType = sy; }
  private:
    // For handling, e.g.,  typedef int FOO; typedef FOO BAR.
    //  ^WithDeclaredType
};

//
// Represents SSR_TAG
//
class SymbolTag: public Symbol, WithDeclaredType
{
  public:
    ASTAPI SymbolTag(SymbolManager *symbolManager, Type *type, char *name,
                     Symbol *declaredType = NULL, char *decoratedName = NULL,
                     SymbolTag *owningClass = NULL,
                     SFA sfa = SFANULL, char *lintSpec = NULL);
    ASTAPI Symbol     * GetDeclaredType() { return declaredType; }
    ASTAPI SymbolList * GetMemberList()   { return memberList; }
    ASTAPI void         AddTagMember (Symbol *, SymbolManager *);
    ASTAPI virtual void  DumpMethod(DumpContext *pDC);
  internal:
    SymbolTag(SSR *ssr); // used while parsing only
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
    virtual void PrintMethod(IndentingStream *stream);
  private:
    SymbolList *memberList;
    SymbolList *lastMember;
};

//
// Represents SSR_CTEMPLATE
//
class SymbolCTemplate: public Symbol
{
  internal:
    SymbolCTemplate(SSR *ssr);       // Standard constructor used while parsing
    virtual void PrintMethod(IndentingStream *stream);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
  private:
};

//
// Represents SSR_FTEMPLATE
//
class SymbolFTemplate: public Symbol
{
  internal:
    SymbolFTemplate(SSR *ssr);
    virtual void PrintMethod(IndentingStream *stream);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
  private:
};

//
// Represents SSR_LABEL
//
class SymbolLabel: public Symbol
{
  public:
    SymbolLabel(SymbolManager *symbolManager, Type *type, char *name,
                char *decoratedName = NULL, SymbolTag *owningClass = NULL,
                SFA sfa = SFANULL,
                char *lintSpec = NULL);
  internal:
    SymbolLabel(SSR *ssr);   // Standard constructor used while parsing
    void Initialize(SymbolTable *, TypeManager *,  TypeIO *);
    virtual void PrintMethod(IndentingStream *stream);
  private:
};

// 
// Base class for function symbols (not instantiable)
//
class SymbolCommonFunctionStuff: public Symbol //
{
  public:
    ASTAPI Bool FStatic()    { return fStatic; }
    ASTAPI Bool FDll()       { return fDllImport || fDllExport; }
    ASTAPI Bool FDllImport() { return fDllImport; }
    ASTAPI Bool FDllExport() { return fDllExport; }
    ASTAPI Bool FSysapi()    { return fSysapi; } // declared as __sysapi
    ASTAPI Bool FNaked()     { return fNaked; } // _declspec(naked)
    ASTAPI Bool FNoThrow()   { return fNoThrow; }
    // ie. declared with __declspec(nothrow) or throw() exception-specification
    ASTAPI Bool FNoReturn()  { return fNoReturn; } // __declspec(noreturn)
    ASTAPI Bool FVoidParamList() { return fVoidParamList; }
    // ie. declared  int f(void);  rather than  int f();
  protected:
    SymbolCommonFunctionStuff(SymbolManager *symbolManager, SymbolKind sk,
                              Type *type, char *name, char *decoratedName,
                              SymbolTag *owningClass,
                              Bool fVoidParamList, Bool fStatic,
                              Bool fDllImport, Bool fDllExport, Bool fSysapi,
                              Bool fNaked, Bool fNoThrow, Bool fNoReturn,
                              SFA sfa,
                              char *lintSpec);
    SymbolCommonFunctionStuff(SymbolKind sk, SSR *ssr);
    Bool fVoidParamList : 1;
  private:
    Bool fStatic        : 1;
    Bool fDllImport     : 1;
    Bool fDllExport     : 1;
    Bool fSysapi        : 1;
    Bool fNaked         : 1;
    Bool fNoThrow       : 1;
    Bool fNoReturn      : 1;
};

//
// Represents SSR_EXTENTRY (defined, externally visible functions)
//

class SymbolFunction: public SymbolCommonFunctionStuff, WithDeclaredType
{
  public:
    ASTAPI SymbolFunction(SymbolManager *symbolManager, Type *type, char *name,
                          Symbol *declaredType, SymbolTable *formalsScope,
                          char *decoratedName = NULL,
                          SymbolTag *owningClass = NULL,
                          Bool fVoidParamList = FALSE, Bool fStatic = FALSE,
                          Bool fDllImport = FALSE, Bool fDllExport = FALSE,
                          Bool fSysapi = FALSE, Bool fNaked = FALSE,
                          Bool fNoThrow = FALSE, Bool fNoReturn = FALSE,
                          Bool fInline = FALSE,
                          SFA sfa = SFANULL, char *lintSpec = NULL);
    ASTAPI AstFunction * GetAstFunction();
    ASTAPI Bool          FInline()         { return fInline; }
    ASTAPI Bool          FInPch()          { return fInPch; }
    ASTAPI Symbol *      GetDeclaredType() { return declaredType; }
    // overrides
    ASTAPI virtual void  DumpMethod(DumpContext *pDC);
    ASTAPI SymbolTable * GetFormalsSymbolTable() { return formalsScope; }
  internal:
    SymbolFunction(SSR *ssr);        // Standard constructor used while parsing
    void ParseBodySymbols(SymbolTable *prevScope,
                          SymbolTable *parent,
                          AstManager *astManager,
                          TypeManager *typeManager,
                          TypeIO *typeIO,
                          SymbolManager *symbolManager);
    void ParseBodyCode(AstManager *astManager, 
                       TypeManager *typeManager,
                       TypeIO *typeIO);
    virtual void PrintMethod(IndentingStream *stream);
    //Symbol *GetRealFunction(); // returns functions this is an alias for
                               // or NULL.
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
    long GetEXOffset() {return exOffset;}
    long GetSYOffset() {return syOffset;}
    void SetEXOffset(long newOf);
    void SetSYOffset(long newOf);
  private:
    SymbolTable *formalsScope;
    long exOffset;
    long syOffset;
    AstNode *astDefinition; // for thunks, points to real symbol
    Bool fInline : 1;
    Bool fInPch  : 1;
};    

//
// Represents SSR_ENTRY (externally defined (imported) functions)
//
class SymbolImport: public SymbolCommonFunctionStuff, WithDeclaredType
{
  public:
    ASTAPI SymbolImport(SymbolManager *symbolManager, Type *type, char *name,
                        Symbol *declaredType, char *decoratedName = NULL,
                        SymbolTag *owningClass = NULL,
                        Bool fVoidParamList = FALSE, Bool fStatic = FALSE,
                        Bool fDllImport = FALSE, Bool fDllExport = FALSE,
                        Bool fSysapi = FALSE, Bool fNaked = FALSE,
                        Bool fNoThrow = FALSE, Bool fNoReturn = FALSE,
                        SFA sfa = SFANULL,
                        char *lintSpec = NULL);
    ASTAPI Symbol *GetDeclaredType() { return declaredType; }
  internal:
    SymbolImport(SSR *ssr);  // Standard constructor used while parsing
    SymbolImport(SymbolKind sk, SSR *ssr); // Symbolkind is specifiable
    virtual void PrintMethod(IndentingStream *stream);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
  private:
};

//
// Represents SSR_WEAKENTRY
// (externally defined (imported) functions, which need not be user defined)
//
class SymbolWeakImport: public SymbolImport
{
  public:
    ASTAPI SymbolWeakImport(SymbolManager *symbolManager, Type *type,
                            char *name, Symbol *declaredType,
                            SymbolKey weakExternKey,
                            char *decoratedName = NULL,
                            SymbolTag *owningClass = NULL,
                            Bool fVoidParamList = FALSE, Bool fStatic = FALSE,
                            Bool fDllImport = FALSE, Bool fDllExport = FALSE,
                            Bool fSysapi = FALSE, Bool fNaked = FALSE,
                            Bool fNoThrow = FALSE, Bool fNoReturn = FALSE,
                            SFA sfa = SFANULL,
                            char *lintSpec = NULL);
    ASTAPI SymbolKey GetWeakExternKey() { return weakExternKey; }
  internal:
    SymbolWeakImport(SSR *ssr);      // Standard constructor used while parsing
    virtual void PrintMethod(IndentingStream *stream);
  private:
    SymbolKey weakExternKey;
};

//
// Represents SSR_CONSTANT
//

// The owning class of a SymbolEnumConstant is the SymbolTag which
// names the enum type, e.g.
//
//    class C { enum E { A, B }; };
//
// The owning class of "A" is "E", and the owning class of "E" is "C".

class SymbolEnumConstant: public Symbol
{
  public:
    ASTAPI SymbolEnumConstant(SymbolManager *symbolManager, Type *type,
                              char *name, long value,
                              char *decoratedName = NULL, 
                              SymbolTag *owningClass = NULL,
                              SFA sfa = SFANULL,
                              char *lintSpec = NULL);
    ASTAPI long GetValue() { return value; }
  internal:
    SymbolEnumConstant(SSR *ssr); // Standard constructor used while parsing
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
    virtual void PrintMethod(IndentingStream *stream);
  private:
    long value;
};


//
// Represents SSR_FILENAME
//

// SymbolFilenames are a bit wierd - they have all the symbol
// attributes but the only useful thing about them is the name, which
// is a source file name.

class SymbolFilename: public Symbol
{
  public:
  internal:
    ASTAPI SymbolFilename(SymbolManager *symbolManager, Type *type, char *name,
                          SFA sfa = SFANULL);
    SymbolFilename(SSR *ssr);
    virtual void Initialize(SymbolTable *symTable, TypeManager *typeManager, TypeIO *typeIO);
    virtual void PrintMethod(IndentingStream *stream);
  private:
};

///////////////////////////////////////////////////////////////////////////////
// Symbol List

class SymbolList: public Object {
   public:
     ASTAPI Symbol *GetSymbol() { return symbol; }
   internal:
      SymbolList(Symbol *sy) { symbol = sy; next = NULL; }
      void AddSymbol(Symbol *, SymbolList **, SymbolManager *);
   private:
     friend struct SymbolListIterator;
     friend void SymbolTag::AddTagMember(Symbol *, SymbolManager *);

     SymbolList *next;
     Symbol *symbol;
};

///////////////////////////////////////////////////////////////////////////////
// Symbol Managers
//
// Symbol managers are allocators of symbols as well as the holder of the
// global symbol table.

class SymbolManager: public Object, public WithArena
{
  public:
    ASTAPI SymbolManager(ArenaAllocator *pAllocator);
    ASTAPI ~SymbolManager() {}
    ASTAPI Allocator *   GetAllocator()    { return pAllocator; };
    ASTAPI void          Write(char *, TypeIO *);
    ASTAPI SymbolTable * GetRoot()         { return rootSymbolTable; }
    ASTAPI AttribKey     GetMaxAttribKey() { return (AttribKey)maxKey; }
    ASTAPI short         GetProductID()    { return productID; }
    ASTAPI long          GetIlID()         { return ilID; }
    ASTAPI char          GetTranslatorID() { return translatorID;    }
    // overrides
    // virtual void DumpMethod(DumpContext *pDC);
  internal:
    void RegisterSymbol(Symbol *symbol);
    void RegisterKey(SymbolKey symbolKey);
    void SetProductID(short id) { productID = id; }
    void SetIlID(long id) { ilID = id; }
    void SetTranslatorID(char id) { translatorID = id; }

    // Used to note all functions (globals and methods) in definition order:
    SymbolFunction **allFunctions;
    int allFunctionsCount;
    int allFunctionsLength;
    void AddFunction(SymbolFunction *function);
    SymbolMap *allFunctionsByKey;

    // allTags is used to remember all tags so we can look them up
    // independent of scope.
    // Used in finding correct scope for member functions and static
    // data members.

    OpenHashedVoidPointerMap *allTags;
    Symbol* MakeSymbolFromSSR(SSR& ssr, SymbolTable *symbolTable);
    void InsertSymbolIntoOwnersScope(Symbol* symbol, SymbolKey tagKey);
    void NoteTagNowHasScope(Symbol* tag);
    struct TagInfo* GetTagInfo(SymbolKey tagKey);

    void ResolveTagTypes(TypeIO *typeIO, TypeManager*);
    void FixTagMemberTypes(TypeIO *typeIO, TypeManager*);

    TypeMap *MakeTypeMap();

  private:
    SymbolKey maxKey;
    SymbolTable *rootSymbolTable;
    short productID;
    long ilID;
    char translatorID;
    
    friend class Symbol;
};


// Usage:
//   for (ModuleFunctionIterator iter = module.GetFunctionIterator();
//        iter.FMore();
//        iter.Advance())
//     SymbolFunction *f = iter.GetSymbolFunction();
//
// This delivers the functions in definition order.

struct ModuleFunctionIterator
{
  public:
    ASTAPI Bool FMore();
    ASTAPI void Advance();
  public:
    ASTAPI SymbolFunction* GetSymbolFunction();
  private:
    ASTAPI ModuleFunctionIterator(SymbolManager*);
    friend class Module;
    SymbolManager *symbolManager;
    int index;
};

inline ASTAPI
ModuleFunctionIterator::ModuleFunctionIterator(SymbolManager* symbolManager)
    : symbolManager(symbolManager), index(0)
{
}

inline ASTAPI SymbolFunction* ModuleFunctionIterator::GetSymbolFunction()
{
    return  symbolManager->allFunctions[index];
}

inline ASTAPI Bool ModuleFunctionIterator::FMore()
{
    return  index < symbolManager->allFunctionsCount;
}

inline ASTAPI void ModuleFunctionIterator::Advance()
{
    index++;
}

///////////////////////////////////////////////////////////////////////////////
// Iterators


// iterates through a symtab's child symbol tables
//  - child symbol tables have a particular level (= currentlevel + 1)
struct SymbolTableChildIterator
{
  public:
    ASTAPI SymbolTableChildIterator(); // Allows creation of uninitialized vars
    ASTAPI SymbolTable *NextChild();
  internal:
    SymbolTableChildIterator(SymbolTable *parent, SymbolTable *firstChild);
  private:
    SymbolTable *child;
};

struct SymbolTableTreeIterator
{
  public:
    ASTAPI SymbolTableTreeIterator(); // Allows creation of uninitialized vars
    ASTAPI SymbolTable *NextSymbolTable();
  internal:
    SymbolTableTreeIterator(SymbolTable *parent, SymbolTable *firstChild);
  private:
    SymbolTable *child;
    int parentLevel;
};

struct SymbolTableEntryIterator
{
  public:
    ASTAPI SymbolTableEntryIterator(); // Allows creation of uninitialized vars
    ASTAPI inline Bool      FNextEntry() { return iterator.FNext(); }
    ASTAPI inline Symbol *  NextSymbol() { return static_cast<Symbol *>(iterator.Next()); }
    ASTAPI inline SymbolKey GetKey()
      { return static_cast<SymbolKey>(iterator.GetIndex()); }
    ASTAPI inline Symbol *  GetSymbol()
      { return static_cast<Symbol*>(iterator.GetValue()); }
  internal:
    SymbolTableEntryIterator(SymbolMap *symbolMap):
        iterator(SymbolMapIterator(symbolMap)) {};
  private:
    SymbolMapIterator iterator;
};

struct SymbolTableMaskedEntryIterator
{
  public:
    ASTAPI SymbolTableMaskedEntryIterator(); // Allows creation of uninit vars
    ASTAPI Bool             FNextEntry();
    ASTAPI Symbol *         NextSymbol();
    ASTAPI inline SymbolKey GetKey()
      { return static_cast<SymbolKey>(iterator.GetIndex());}
    ASTAPI inline Symbol *  GetSymbol()
      { return static_cast<Symbol*>(iterator.GetValue());}
  internal:
    SymbolTableMaskedEntryIterator(SymbolKindMask mask, SymbolMap *symbolMap,
                                   Bool fIncludeCGSymbols):
        mask(mask),
        iterator(SymbolMapIterator(symbolMap)),
        fIncludeCGSymbols(fIncludeCGSymbols) {};
  private:
    SymbolMapIterator iterator;
    SymbolKindMask mask;
    Bool fIncludeCGSymbols;
};

// iterates through a symbol list
struct SymbolListIterator
{
  public:
    ASTAPI SymbolListIterator(SymbolList *l):
        current(l), fInitialized(FALSE) { }
    // Cursor must be advanced at least once before getting a field.
    ASTAPI Bool    FNext();     // Advance cursor; return success
    ASTAPI Bool    FDone();     // Has iterator been traversed?
    ASTAPI Symbol *NextSymbol(); // Advance cursor; return next symbol
    ASTAPI Symbol *GetSymbol();  // Return current symbol
    ASTAPI Bool    FLast();     // will FNext() will return false?
  protected:
    Bool fInitialized;
    SymbolList *current;
};


//////////////////////////////////////////////////////////////////////////////
//
// (Another way of doing this is to use virtual functions but
//  they don't inline well)

#define DOWNCAST(kind,class)\
inline ASTAPI class* Symbol::as##class()\
{\
     return  symbolKind == kind ? static_cast<class*>(this) : 0;\
}\
\
inline ASTAPI class* as##class (Symbol* symbol)\
{\
     return  symbol == 0 ? 0 : symbol->as##class();\
}

DOWNCAST(skVariable,SymbolVariable)
DOWNCAST(skField,SymbolField)
DOWNCAST(skTypedef,SymbolTypedef)
DOWNCAST(skTag,SymbolTag)
DOWNCAST(skFilename,SymbolFilename)
DOWNCAST(skFunction,SymbolFunction)
DOWNCAST(skImport,SymbolImport)
DOWNCAST(skWeakImport,SymbolWeakImport)
DOWNCAST(skEnumConstant,SymbolEnumConstant)
DOWNCAST(skLabel,SymbolLabel)
DOWNCAST(skCTemplate,SymbolCTemplate)
DOWNCAST(skFTemplate,SymbolFTemplate)
#undef DOWNCAST

#endif // INC_SYMBOL_H
