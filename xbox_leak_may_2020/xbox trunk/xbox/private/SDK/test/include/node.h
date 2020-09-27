// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved.
//
// $Header: /ast/sdk/lib/node.h 7     5/09/96 1:25p Erikruf $
// $NoKeywords: $
//
//


#ifndef INC_NODE_H
#define INC_NODE_H

///////////////////////////////////////////////////////////////////////////////
// Class structure (forward declarations)
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

class AstNode;
class   AstFunction;
class     AstConstructor;
class   AstExpression;
class     AstNullExpr;
class     AstConstant;
class     AstSymbol;
class     AstIntrinsic;
class     AstNew;
class       AstNewObject;
class       AstNewArray;
class     AstDelete;
class     AstFunctionCall;
class     AstConstructorFn;
class       AstRealConstructorFn;
class     AstUnary;
class       AstCast;
class     AstBinary;
class     AstAssignment;
class     AstQuestion;
class     AstSizeof;
class     AstThrow;
class     AstInitList;
class     AstMemberAddress;
class   AstDeclStat;
class     AstLint;
class     AstDeclare;
class       AstDeclaration;
class         AstInitialization;
class         AstFuncDecl;
class       AstFuncDefn;
class     AstStatement;
class       AstExpr;
class       AstNullStmt;
class       AstBlock;
class       AstIf;
class       AstSwitch;
class       AstFor;
class       AstWhile;
class       AstDo;
class       AstGoto;
class       AstLabelStmt;
class         AstLabel;
class         AstCase;
class         AstDefault;
class       AstBreak;
class       AstContinue;
class       AstReturn;
class       AstAsmBlock;
class       AstAsmStatement;
class         AstAsmStmt;
class         AstAsmLabel;
class         AstAsmAlign;
class       AstSehTryExcept;
class       AstSehTryFinally;
class       AstSehLeave;
class       AstTry;
class       AstCatch;

class AstManager;

////END////

AST_DEFINE_ATTRIBUTE_CLASSES(Ast, AstNode, AstManager);

// Forward declaration
struct IlTree;

///////////////////////////////////////////////////////////////////////////////
// Opcodes

enum OpCode {
#define OPDAT(name, class, friendly) name,
#include "opcode.h"
#undef OPDAT
};

///////////////////////////////////////////////////////////////////////////////
// Intrinsics

enum IntrinsicKind {
    ikStart, ikSetjmp, ikSetjmpex, ikAlloca, ikDisable, ikOutp, ikMemset,
    ikEnable, ikOutpw, ikOutpd, ikFabs, ikStrcat, ikInp, ikRotl,
    ikAbs, ikStrcmp, ikInpw, ikInpd, ikRotr, ikMemcmp, ikStrcpy,
    ikLrotl, ikStrset, ikMemcpy, ikStrlen, ikLrotr, ikAcos,
    ikCosh, ikPow, ikTanh, ikAsin, ikFmod, ikSinh, ikAtan,
    ikExp, ikLog10, ikSqrt, ikAtan2, ikLog, ikSin, ikTan,
    ikCos, ikReturnAddress, ikExceptionCode, ikExceptionInfo,
    ikAbnormalTermination, ikInterlockedDecrement, 
#if CC_VERSION60
    ikCxxThrow,
#endif
    ikEnd
};

///////////////////////////////////////////////////////////////////////////////
// Node Keys
typedef unsigned long NodeKey;

///////////////////////////////////////////////////////////////////////////////
// Nodes

class AstNode: public Object
{
  public:
    ASTAPI AttribKey    GetAttribKey()       { return (AttribKey)nodeKey; }
    ASTAPI OpCode       GetOpCode()          { return opCode; }
    ASTAPI AstNode **   GetInputVector()     { return inputVector; }
    ASTAPI int          GetInputCount()      { return inputCount; }
    ASTAPI SFA *        GetSFA()             { return &sfa; }
    ASTAPI AstNode *    GetParent()          { return parent; }
    ASTAPI void         ReplaceChild(AstNode* oldChild, AstNode *newChild);
    ASTAPI void         PrefixPrint(
        IndentingStream *pStream = Debug::pMessageStream,
        AstAttribute *attribute = NULL
      );
    ASTAPI char *       FriendlyOpCodeName();
    ASTAPI Bool         FCompilerGenerated() { return fCompilerGenerated; }
    ASTAPI Bool         FToolGenerated()     { return fToolGenerated; }
    ASTAPI virtual Bool FExpression()        { return FALSE; }
    ASTAPI virtual Bool FStatement()         { return FALSE; }
    ASTAPI virtual Bool FDeclaration()       { return FALSE; }
    ASTAPI virtual Bool FFunction()          { return FALSE; }
    ASTAPI static void * ASTCALL operator new(size_t size, AstManager *astManager);
  internal:
    void InstallInputs(AstNode **inputs, int count);
    void SetParent(AstNode *astNodeParent) {parent = astNodeParent;}
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    void PrefixPrintInternal(IndentingStream *pStream, AstAttribute *attribute);
    void SetSFA(SFA *sfa);

    void Install0Inputs();
    void InstallInput(AstNode *input1, AstManager*);
    void InstallInputs(AstNode *input1, AstNode *input2, AstManager*);
    void InstallInputs(AstNode *input1, AstNode *input2, AstNode *input3,
                       AstManager*);
    // For evil hacks
    //void SetOpCode(OpCode newOpCode) { opCode = newOpCode; }
    // overrides
    virtual void DumpMethod(DumpContext *pDC);
    virtual void DumpStandardPrefix(DumpContext *dc);
    virtual void PrefixPrintMethod(IndentingStream *pStream);
  protected:
    AstNode(AstManager *astManager, OpCode opCode, SFA sfa); // for tool nodes
    AstNode(AstManager *astManager, OpCode opCode = ocError); // for IL nodes
    ASTAPI ~AstNode() {}

    NodeKey nodeKey;
    OpCode opCode           : 16;
    Bool fCompilerGenerated : 1;
    Bool fToolGenerated     : 1;

    AstNode *parent;
    AstNode **inputVector;
    int inputCount;
    SFA sfa;
};

///////////////////////////////////////////////////////////////////////////////
// Definition nodes (functions and constructors)

class AstFunction: public AstNode
{
  public:
    ASTAPI AstFunction(AstManager *astManager, OpCode opCode,
                       AstSymbol *name, AstBlock *body,
                       AstSymbol **formalVector, int formalCount,
                       SFA sfa = SFANULL);
    ASTAPI SymbolTable * GetScope();
    ASTAPI AstSymbol *   GetName() { return (AstSymbol *) inputVector[0]; }
    ASTAPI AstBlock *    GetBody() { return (AstBlock *) inputVector[1]; }

    ASTAPI int           GetFormalCount() { return formalCount; }
    ASTAPI AstSymbol  ** GetFormalVector();
    // overrides:
    ASTAPI Bool          FFunction() { return TRUE; }
  internal:
    AstFunction(AstManager *astManager, int formalCount, OpCode opCode):
        formalCount(formalCount), AstNode(astManager, opCode) {}
  protected:
    AstFunction(AstManager *astManager, OpCode opCode, SFA sfa):
        AstNode(astManager, opCode, sfa) {}
    int formalCount;
  private:
    SymbolTable *scope;
};

class AstConstructor: public AstFunction
{
  public:
    ASTAPI AstConstructor(AstManager *astManager, AstSymbol *name,
                          AstBlock *body, AstSymbol **formalVector,
                          int formalCount, AstExpression **initializerVector,
                          int initializerCount, SFA sfa = SFANULL);
    ASTAPI int GetInitializerCount()  { return inputCount - formalCount - 2; }
    ASTAPI AstExpression ** GetInitializerVector()
      {return reinterpret_cast<AstExpression **>(&inputVector[formalCount+2]);}
  internal:
    AstConstructor(AstManager *astManager, int formalCount):
        AstFunction(astManager, formalCount, ocConstructor) { }
};

///////////////////////////////////////////////////////////////////////////////
// Expression nodes

class AstExpression: public AstNode
{
  public:
    ASTAPI Type *     GetReturnType()  { return returnType; }
    // overrides
    ASTAPI Bool       FExpression()    { return TRUE; }
  internal:
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    // For evil hacks
    void SetReturnType(Type *newType) { returnType = newType; }
  protected:
    // Used for nodes corresponding to IL
    AstExpression(AstManager *astManager, OpCode opCode):
        AstNode(astManager, opCode) {}
    AstExpression(AstManager *astManager, OpCode opCode,
		  Type *returnType, SFA sfa);
    virtual void DumpStandardPrefix(DumpContext *dc);
    Type *returnType;
};

class AstNullExpr: public AstExpression
{
  public:
    ASTAPI AstNullExpr(AstManager *astManager, SFA sfa = SFANULL):
        AstExpression(astManager, ocNullExpr) {}
  internal:
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symtab);
};

class AstConstant: public AstExpression
{
  public:
    ASTAPI AstConstant(AstManager *astManager, ConstantValue *constantValue,
                       Type *returnType, SFA sfa = SFANULL);
    ASTAPI ConstantValue *GetConstantValue() { return constantValue; }
  internal:
    AstConstant(AstManager *astManager):
        AstExpression(astManager, ocConstant) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    virtual void DumpMethod(DumpContext *pDC);
    virtual void PrefixPrintMethod(IndentingStream *pStream);
  private:
    ConstantValue *constantValue;
};

class AstSymbol: public AstExpression
{
  public:
    ASTAPI AstSymbol(AstManager *astManager, Symbol *symbol, Type *returnType,
                     SFA sfa = SFANULL);
    ASTAPI Symbol *  GetSymbol() { return symbol; }
    ASTAPI SymbolKey GetKey()    { return GetSymbol()->GetKey(); }
    ASTAPI char *    GetName()   { return GetSymbol()->GetName(); }
  internal:
    AstSymbol(AstManager *astManager): AstExpression(astManager, ocSymbol) {}
    void InitFromSymbol(Symbol *symbol, IlTree *ilTree, SymbolTable *symTab);
    void SetSymbol(Symbol *to) { symbol = to; }
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    virtual void DumpMethod(DumpContext *pDC);
    virtual void PrefixPrintMethod(IndentingStream *pStream);
  private:
    Symbol *symbol;
};

class AstFunctionCall: public AstExpression
{
  public:
    ASTAPI AstFunctionCall(AstManager *astManager, OpCode opCode,
                           AstExpression *function,
                           AstExpression **actualVector, int actualCount,
                           int firstDefault, Type *returnType,
                           SFA sfa = SFANULL);
    ASTAPI AstExpression *  GetFunction()
      { return (AstExpression *) inputVector[0]; }
    ASTAPI int              GetActualCount()  { return inputCount-1; }
    ASTAPI AstExpression ** GetActualVector();
    ASTAPI int              GetFirstDefault() { return firstDefault; }
    // Returns: first default arg, -1 for none (first arg is numbered 0)
  internal:
    AstFunctionCall(AstManager *astManager, OpCode opCode, int firstDefault) :
        AstExpression(astManager, opCode), firstDefault(firstDefault) {}
  private:
    int firstDefault;
};

// Used for default constructor and copy functions
class AstConstructorFn: public AstExpression
{
  public:
    ASTAPI AstConstructorFn(AstManager *astManager, OpCode opCode,
                            AstSymbol *classSymbol, Type *returnType,
                            SFA sfa = SFANULL);
    ASTAPI AstSymbol *GetClass() { return (AstSymbol *) inputVector[0]; }
  internal:
    AstConstructorFn(AstManager *astManager, OpCode opCode):
        AstExpression(astManager, opCode) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  protected:
    AstConstructorFn(AstManager *astManager, OpCode opCode,
                     Type *returnType, SFA sfa):
        AstExpression(astManager, opCode, returnType, sfa) { }
};

class AstRealConstructorFn: public AstConstructorFn
{
  public:
    ASTAPI AstRealConstructorFn(AstManager *astManager, AstSymbol *classSymbol,
                                AstSymbol *fnSymbol, Type *returnType,
                                SFA sfa = SFANULL);
    ASTAPI AstSymbol *GetConstructor() { return (AstSymbol *) inputVector[1]; }
  internal:
    AstRealConstructorFn(AstManager *astManager):
        AstConstructorFn(astManager, ocRealConstructorFn) {}
};



// REVIEW: describe which AstNew variant one gets for different expressions.
//  e.g.  new int(n+1)  // initialized but no ctor

// AstNew       (ocNew)
//   no initialization e.g. new int;
// AstNewObject (ocNewObject)
//   e.g. new int(n+1);  
//   e.g. new MyClass(n+1);  
// AstNewArray  (ocNewArray)
//   e.g. new int[3];
//   e.g. new MyCLass[3];

class AstNew: public AstExpression
{
  public:
    ASTAPI AstNew(AstManager *astManager, OpCode opCode,
                  AstFunctionCall *newCall, Type *returnType,
                  SFA sfa = SFANULL);
    ASTAPI AstFunctionCall *GetNewCall()
      { return (AstFunctionCall *) inputVector[0]; }
  internal:
    AstNew(AstManager *astManager, OpCode opCode):
        AstExpression(astManager, opCode) {}
  protected:
    AstNew(AstManager *astManager, OpCode opCode, Type *returnType, SFA sfa):
        AstExpression(astManager, opCode, returnType, sfa) {}
};

class AstNewObject: public AstNew
{
  public:
    ASTAPI AstNewObject(AstManager *astManager, AstFunctionCall *newCall,
                        AstExpression *initializer, Type *returnType,
                        SFA sfa = SFANULL);
    ASTAPI AstExpression *GetConstructor()
      { return (AstExpression *) inputVector[1]; }
  internal:
    AstNewObject(AstManager *astManager): AstNew(astManager, ocNewObject) {}
};

class AstNewArray: public AstNew
{
  public:
    ASTAPI AstNewArray(AstManager *astManager, AstFunctionCall *newCall,
                       AstExpression *size, Type *returnType,
                       SFA sfa = SFANULL);
    ASTAPI AstExpression *GetSize();
  internal:
    AstNewArray(AstManager *astManager): AstNew(astManager, ocNewArray) {}
};

class AstDelete: public AstExpression
{
  public:
    ASTAPI AstDelete(AstManager *astManager, AstSymbol *deleteOperator,
                     AstExpression *expression, Bool fIsArray,
                     Type *returnType, SFA sfa = SFANULL);
    ASTAPI AstSymbol *     GetDeleteOperator()
      { return (AstSymbol *) inputVector[0]; }
    ASTAPI AstExpression * GetExpression()
      { return (AstExpression *) inputVector[1]; }
    Bool                   FIsArray()      { return fIsArray; }
  internal:
    AstDelete(AstManager *astManager, Bool fIsArray):
        AstExpression(astManager, ocDelete), fIsArray(fIsArray) {}
  private:
    Bool fIsArray;
};

class AstIntrinsic: public AstExpression
{
  public:
    ASTAPI AstIntrinsic(AstManager *astManager, TypeManager *typeManager, 
                        TypeMap *typeMap,
                        IntrinsicKind kind, AstExpression **actualVector,
                        int actualCount, Type *returnType, SFA sfa = SFANULL);
    ASTAPI IntrinsicKind    GetOrdinal()     { return kind; }
    ASTAPI char *           GetName()        { return name; }
    ASTAPI int              GetActualCount() { return inputCount-1; }
    ASTAPI AstExpression ** GetActualVector();
    ASTAPI static char *    GetName(IntrinsicKind kind);
  internal:
    AstIntrinsic(AstManager *astManager):
        AstExpression(astManager, ocIntrinsic) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
    IntrinsicKind kind;
    char *name;
    static struct tuple {int ordinal; IntrinsicKind kind; char *name;}iTable[];
};

class AstUnary: public AstExpression
{
  public:
    ASTAPI AstUnary(AstManager *astManager, OpCode opCode,
                    AstExpression *operand, Type *returnType,
                    SFA sfa = SFANULL);
    ASTAPI AstExpression *GetOperand()
      { return (AstExpression *) inputVector[0]; }
  internal:
    AstUnary(AstManager *astManager, OpCode opCode):
        AstExpression(astManager, opCode) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    virtual void PrefixPrintMethod(IndentingStream *pStream);
};

class AstCast: public AstUnary
{
  public:
    ASTAPI AstCast(AstManager *astManager, OpCode opCode,
                   AstExpression *operand, Type *returnType,
                   SFA sfa = SFANULL);
  internal:
    AstCast(AstManager *astManager, OpCode opCode):
        AstUnary(astManager, opCode) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    //virtual void DumpStandardPrefix(DumpContext *dc);
  private:
};

class AstBinary: public AstExpression
{
  public:
    ASTAPI AstBinary(AstManager *astManager, OpCode opCode,
                     AstExpression *leftOperand, AstExpression *rightOperand,
                     Type *returnType, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetLeftOperand()
      { return (AstExpression *) inputVector[0]; }
    ASTAPI AstExpression * GetRightOperand()
      { return (AstExpression *) inputVector[1]; }
  internal:
    AstBinary(AstManager *astManager, OpCode opCode):
        AstExpression(astManager, opCode) {}
};

class AstAssignment: public AstExpression
{
  public:
    ASTAPI AstAssignment(AstManager *astManager, OpCode opCode,
                         AstExpression *target, AstExpression *value,
                         Type *returnType, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetTarget()
      { return (AstExpression *) inputVector[0]; }
    ASTAPI AstExpression * GetNewValue()
      { return GetInputCount() == 2
            ? (AstExpression *) inputVector[1]
            : NULL; }
  internal:
    AstAssignment(AstManager *astManager, OpCode opCode):
        AstExpression(astManager, opCode) {}
};

class AstQuestion: public AstExpression
{
  public:
    ASTAPI AstQuestion(AstManager *astManager, AstExpression *predicate,
                       AstExpression *consequent, AstExpression *alternative,
                       Type *returnType, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetPredicate()
      { return (AstExpression *) inputVector[0]; }
    ASTAPI AstExpression * GetConsequent()
      { return (AstExpression *) inputVector[1]; }
    ASTAPI AstExpression * GetAlternative()
      { return (AstExpression *) inputVector[2]; }
  internal:
    AstQuestion(AstManager *astManager):
        AstExpression(astManager, ocQuestion) {}
};

class AstSizeof: public AstExpression
{
  public:
    ASTAPI AstSizeof(AstManager *astManager, __int64 size, Type *typeExpr,
                     AstExpression *expression, Type *returnType,
                     SFA sfa = SFANULL);
    ASTAPI __int64         GetSize() { return size; }
    ASTAPI Type          * GetType() { return typeExpression; }
    ASTAPI AstExpression * GetExpression(); // may be NULL
  internal:
    AstSizeof(AstManager *astManager): AstExpression(astManager, ocSizeof) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
    __int64 size;
    Type *typeExpression;
};

class AstThrow: public AstExpression
{
  public:
    ASTAPI AstThrow(AstManager *astManager, AstExpression *expression,
                    Type *returnType, SFA sfa = SFANULL);
    ASTAPI AstExpression *GetOperand();  // may be NULL
  internal:
    AstThrow(AstManager *astManager): AstExpression(astManager, ocThrow) {}
};

class AstInitList: public AstExpression
{
  public:
    ASTAPI AstInitList(AstManager *astManager, AstExpression **initVector,
                       int initCount, Type *returnType, SFA sfa = SFANULL);
    ASTAPI AstExpression ** GetInitVector()
      { return reinterpret_cast<AstExpression **>(inputVector); }
    ASTAPI int              GetInitCount() { return inputCount; }
    ASTAPI void             AddToInitList(AstExpression *astExpression);
 internal:
    AstInitList(AstManager *astManager);
  private:
    int vectorLength;
    ArenaAllocator *allocator;
};

class AstMemberAddress: public AstExpression
{
  public:
    ASTAPI AstMemberAddress(AstManager *astManager, Symbol *class, 
                         Symbol *member, Type *returnType, SFA sfa = SFANULL);
    ASTAPI Symbol * GetClassSymbol()  { return classSymbol; }
    ASTAPI Symbol * GetMemberSymbol() { return memberSymbol; }
  internal:
    AstMemberAddress(AstManager *astManager): AstExpression(astManager, ocMemberAddress) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
    Symbol *classSymbol;
    Symbol *memberSymbol;
};

///////////////////////////////////////////////////////////////////////////////
// Declaration and statement nodes

class AstDeclStat: public AstNode
{
  protected:
    AstDeclStat(AstManager *astManager, OpCode opCode, SFA sfa):
        AstNode(astManager, opCode, sfa) {} // for tool nodes
    AstDeclStat(AstManager *astManager, OpCode opCode):
        AstNode(astManager, opCode) {} // for IL nodes
};

class AstLint: public AstDeclStat
{
  public:
    ASTAPI AstLint(AstManager *astManager, char *lintSpec,
                   SFA sfa = SFANULL):
        AstDeclStat(astManager, ocLint, sfa), lintSpec(lintSpec) {}
    ASTAPI char * GetLintSpec()  { return lintSpec; }
    // overrides
    ASTAPI Bool   FDeclaration() { return TRUE; }
  internal:
    AstLint(AstManager *astManager): AstDeclStat(astManager, ocLint) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
    char *lintSpec;
};

///////////////////////////////////////////////////////////////////////////////
// Declaration nodes (special, since C and C++ differ in classification)
//
// This is how various delarations map onto AstDeclare and its derived classes:
//
//   typedef int INT;   -- AstDeclaration.
//   struct S;          -- AstDeclaration, forward.
//   extern int x;      -- AstDeclaration, extern.
//   extern int x = 2;  -- AstInitialization, extern.
//   void f();          -- AstFuncDecl
//   extern void f();   -- AstFuncDecl, extern.
//   void f() {};       -- AstFuncDefn.
//
// AstDeclaration::FForward() is always false for AstInitialization.
// Only AstDeclarations of types (enum, classes, structs and unions)
// can be foreward.
//
// The difference between AstDeclaration and AstFuncDecl is not
// whether the thing being declared is a function or not, but whether
// it was in the function part of a declarator.  Thus typedefs and
// variables can be declared via AstFuncDecls, and functions might be
// declared via AstDeclarations:
//
//   typedef int F(float);         -- AstFuncDecl declares a typedef
//   F f1;                         -- AstDeclaration declares a function
//   typedef int (**PPF)(float);   -- AstFuncDecl declares another typedef
//   int (*pf)(int);               -- AstFuncDecl declared a variable
//   PPF f2;                       -- AstDeclaration declares a variable


class AstDeclare: public AstDeclStat
{
  public:
    ASTAPI AstSymbol * GetDeclared()  { return (AstSymbol *) inputVector[0]; }
    // overrides
    ASTAPI Bool        FDeclaration() { return TRUE; }
  protected:
    AstDeclare(AstManager *astManager, OpCode opCode, SFA sfa):
        AstDeclStat(astManager, opCode, sfa) {}
    AstDeclare(AstManager *astManager, OpCode opCode):
        AstDeclStat(astManager, opCode) {}
};

class AstDeclaration: public AstDeclare
{
  public:
    ASTAPI AstDeclaration(AstManager *astManager, AstSymbol *declaredSymbol,
                          Bool fForward = FALSE, Bool fExtern = FALSE,
                          SFA sfa = SFANULL);
    ASTAPI Bool FForward() { return fForward; }
    ASTAPI Bool FExtern()  { return fExtern; }


  internal:
    AstDeclaration(AstManager *astManager)
        : AstDeclare(astManager, ocDecl),
          fExtern(FALSE), fForward(FALSE)
      {}
    void InitFromSymbol(AstSymbol *symbol, IlTree *ilTree, SymbolTable *symTab);
  protected:
    AstDeclaration(AstManager *astManager, OpCode opCode)
        : AstDeclare(astManager, opCode) {}

    Bool fForward       : 1 ;
    Bool fExtern        : 1 ;
};

class AstInitialization: public AstDeclaration
{
  public:
    ASTAPI AstInitialization(AstManager *astManager, AstSymbol *declaredSymbol,
                             AstExpression *initializer, Bool fExtern = FALSE,
                             SFA sfa = SFANULL);
    ASTAPI AstExpression *GetInitializer()       
      { return (AstExpression *) inputVector[1]; }
  internal:
    AstInitialization(AstManager *astManager)
        : AstDeclaration(astManager, ocInit) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
};

class AstFuncDecl: public AstDeclaration
{
  // This node describes function prototypes.  Besides the obvious, two pieces
  // of info are kept for each paramter: the declared type (eg class name) and
  // lint spec string.  Either may be null.
  public:
    ASTAPI AstFuncDecl * GetNextFuncDecl()   { return nextFuncDecl; }
    ASTAPI int           GetParameterCount() { return parameterCount; }
    ASTAPI Symbol **     GetParameterDeclaredTypeVector() 
                                             { return parameterDeclaredTypeVector; }
    ASTAPI char **       GetParameterLintSpecVector() 
                                             { return parameterLintSpecVector; }
    ASTAPI Bool          FVoidParamList()    { return fVoidParamList; }
    ASTAPI Bool          FVarArgs()          { return fVarArgs; }

  internal:
    AstFuncDecl(AstManager *astManager)
        : AstDeclaration(astManager, ocFuncDecl), parameterCount(0),
          nextFuncDecl(0), parameterDeclaredTypeVector(NULL),
          parameterLintSpecVector(NULL),
          fVoidParamList(FALSE), fVarArgs(FALSE)
      { AstAssert(opCode == ocFuncDecl); }
    void InitFromSymbol(AstManager *astManager, AstSymbol *symbol, IlTree *ilTree, SymbolTable *symTab);

  private:
    int parameterCount;
    Symbol **parameterDeclaredTypeVector;
    char **parameterLintSpecVector;
    AstFuncDecl *nextFuncDecl;
    Bool fVoidParamList  : 1 ;
    Bool fVarArgs        : 1 ;
};


class AstFuncDefn: public AstDeclare
{
  public:
    ASTAPI AstFuncDefn(AstManager *astManager, AstSymbol *declaredSymbol,
                       Bool fForward = FALSE, Bool fExtern = FALSE,
                       SFA sfa = SFANULL);
    ASTAPI AstFunction*  GetFunction();
  internal:
    AstFuncDefn(AstManager *astManager): AstDeclare(astManager, ocFuncDefn) {}
};


///////////////////////////////////////////////////////////////////////////////
// Statement nodes

class AstStatement: public AstDeclStat
{
  public:
    // overrides
    ASTAPI Bool FStatement() { return TRUE; }
  protected:
    AstStatement(AstManager *astManager, OpCode opCode):
        AstDeclStat(astManager, opCode) {}
    AstStatement(AstManager *astManager, OpCode opCode, SFA sfa):
        AstDeclStat(astManager, opCode, sfa) {}
};

class AstExpr: public AstStatement
{
  public:
    ASTAPI AstExpr(AstManager *astManager, AstExpression *expression,
                   SFA sfa = SFANULL);
    ASTAPI AstExpression *GetExpression()
      { return (AstExpression *) inputVector[0]; }
  internal:
    AstExpr(AstManager *astManager):
        AstStatement(astManager, ocExpr) {}
};

class AstNullStmt: public AstStatement
{
  public:
    ASTAPI AstNullStmt(AstManager *astManager, SFA sfa = SFANULL):
        AstStatement(astManager, ocNullStmt, sfa) {}
};


class AstBlock: public AstStatement
{
  public:
    ASTAPI AstBlock(AstManager *astManager, AstDeclStat **declStatVector,
                    int declStatCount, SFA sfa = SFANULL);
    ASTAPI SymbolTable *   GetScope()         { return symbolTable; }
    ASTAPI AstDeclStat **  GetDeclStatVector()
      { return reinterpret_cast<AstDeclStat **>(inputVector); }
    ASTAPI AstStatement ** GetStatementVector(); // deprecated; going away soon
    ASTAPI int             GetDeclStatCount() { return inputCount; }
    ASTAPI int             GetStatementCount(); // deprecated; going away soon
    ASTAPI int             GetChildIndex(AstDeclStat *child);
    ASTAPI void            AddDeclStats(AstManager *astManager,
                                        AstDeclStat **declStatVector,
                                        int declStatCount, int position);
  internal:
    AstBlock(AstManager *astManager): AstStatement(astManager, ocBlock) {}
    void SetScope(SymbolTable *symTab) { symbolTable = symTab; }
    // overrides
    virtual void PrefixPrintMethod(IndentingStream *pStream);
  private:
    SymbolTable *symbolTable;
};

class AstIf: public AstStatement
{
  public:
    ASTAPI AstIf(AstManager *astManager, AstExpression *predicate,
                 AstStatement *consequent, AstStatement *alternative = NULL,
                 SFA sfa = SFANULL);
    ASTAPI AstExpression* GetPredicate()
      { return (AstExpression*) inputVector[0]; }
    ASTAPI AstStatement *  GetConsequent()
      { return (AstStatement*) inputVector[1]; }
    ASTAPI Bool            FHasAlternative() { return (inputCount == 3); }
    ASTAPI AstStatement *  GetAlternative();
    ASTAPI void SetPredicate(AstExpression *newPredicate);
    ASTAPI void SetConsequent(AstStatement *newConsequent);
    ASTAPI void SetAlternative(AstStatement *newAlternative);
  internal:
    AstIf(AstManager *astManager): AstStatement(astManager, ocIf) {}
};

class AstSwitch: public AstStatement
{
  public:
    ASTAPI AstSwitch(AstManager *astManager, AstExpression *switchExpression,
                     AstStatement *body, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetSwitch()
      { return (AstExpression *) inputVector[0]; }
    ASTAPI AstStatement *  GetBody()
      { return (AstStatement *) inputVector[1]; }
  internal:
    AstSwitch(AstManager *astManager): AstStatement(astManager, ocSwitch) {}
};

class AstFor: public AstStatement
{
  public:
    ASTAPI AstFor(AstManager *astManager, AstStatement *initializer,
                  AstExpression *test, AstExpression *increment,
                  AstStatement *body, SFA sfa = SFANULL);
    ASTAPI AstStatement * GetInit() { return (AstStatement *) inputVector[0]; }
    ASTAPI void SetInit(AstStatement *newInit)
      { inputVector[0] = newInit;  inputVector[0]->SetParent(this); }
    ASTAPI AstExpression * GetTest()
      { return (AstExpression *) inputVector[1]; }
    ASTAPI void SetTest(AstExpression *newTest) { inputVector[1] = newTest; }
    ASTAPI AstExpression * GetIncrement()
      { return (AstExpression *) inputVector[2]; }
    ASTAPI void SetIncrement(AstExpression *newIncrement)
      { inputVector[2] = newIncrement; }
    ASTAPI AstStatement * GetBody() { return (AstStatement *) inputVector[3];}
    ASTAPI void SetBody(AstStatement *newBody) { inputVector[3] = newBody; }
  internal:
    AstFor(AstManager *astManager): AstStatement(astManager, ocFor) {}
};

class AstWhile: public AstStatement
{
  public:
    ASTAPI AstWhile(AstManager *astManager, AstExpression *test,
                    AstStatement *body, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetTest()
      { return (AstExpression *) inputVector[0]; }
    ASTAPI AstStatement *  GetBody()
      { return (AstStatement *) inputVector[1]; }
  internal:
    AstWhile(AstManager *astManager): AstStatement(astManager, ocWhile) {}
};

class AstDo: public AstStatement
{
  public:
    ASTAPI AstDo(AstManager *astManager, AstExpression *test,
                 AstStatement *body, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetTest()
      { return (AstExpression *) inputVector[1]; }
    ASTAPI AstStatement *  GetBody()
      { return (AstStatement *) inputVector[0]; }
  internal:
    AstDo(AstManager *astManager): AstStatement(astManager, ocDo) {}
};

class AstGoto: public AstStatement
{
  public:
    ASTAPI AstGoto(AstManager *astManager, AstSymbol *destination,
                   SFA sfa = SFANULL);
    ASTAPI AstSymbol * GetLabel() { return (AstSymbol *) inputVector[0]; }
  internal:
    AstGoto(AstManager *astManager): AstStatement(astManager, ocGoto) {}
};

class AstLabelStmt: public AstStatement
{
  public:
    ASTAPI AstStatement * GetReferent()
      { return (AstStatement *) inputVector[0]; }
    ASTAPI void SetReferent(AstStatement *stmt) { inputVector[0] = stmt; }
  protected:
    AstLabelStmt(AstManager *astManager, OpCode opCode):
        AstStatement(astManager, opCode) {}
    AstLabelStmt(AstManager *astManager, OpCode opCode, SFA sfa):
        AstStatement(astManager, opCode, sfa) {}
};

class AstLabel: public AstLabelStmt
{
  public:
    ASTAPI AstLabel(AstManager *astManager, AstSymbol *label,
                    AstStatement *referent, SFA sfa = SFANULL);
    ASTAPI AstSymbol * GetLabel()
      { return (AstSymbol *) inputVector[1]; }
  internal:
    AstLabel(AstManager *astManager): AstLabelStmt(astManager, ocLabel) {}
};

class AstDefault: public AstLabelStmt
{
  public:
    ASTAPI AstDefault(AstManager *astManager, AstStatement *referent,
                      SFA sfa = SFANULL);
  internal:
    AstDefault(AstManager *astManager): AstLabelStmt(astManager, ocDefault) {}
};

class AstCase: public AstLabelStmt
{
  public:
    ASTAPI AstCase(AstManager *astManager, AstExpression *constant,
                   AstStatement *referent, SFA sfa = SFANULL);
    ASTAPI AstExpression * GetConstant()
      { return (AstExpression *) inputVector[1]; }
  internal:
    AstCase(AstManager *astManager): AstLabelStmt(astManager, ocCase) {}
};

class AstBreak: public AstStatement
{
  public:
    AstBreak(AstManager *astManager, SFA sfa = SFANULL):
        AstStatement(astManager, ocBreak, sfa) { }
};

class AstContinue: public AstStatement
{
  public:
    AstContinue(AstManager *astManager, SFA sfa = SFANULL):
    AstStatement(astManager, ocContinue, sfa) { }
};

class AstReturn: public AstStatement
{
  public:
    ASTAPI AstReturn(AstManager *astManager,
                     AstExpression *returnValue /* may be NULL */,
                     SFA sfa = SFANULL);
    ASTAPI Bool            FReturnsValue();
    ASTAPI AstExpression * GetReturnValue(); // May be NULL
  internal:
    AstReturn(AstManager *astManager): AstStatement(astManager, ocReturn) {}
};

class AstAsmBlock: public AstStatement
{
  public:
    // public constructor not yet implemented
    ASTAPI AstAsmStatement ** GetStatementVector()
      { return (AstAsmStatement **) inputVector; }
    ASTAPI int                GetStatementCount() { return inputCount; }
  internal:
    AstAsmBlock(AstManager *astManager):
        AstStatement(astManager, ocAsmBlock) {}
};

class AstAsmStatement: public AstStatement
{
  internal:
    // public constructor not yet implemented
    AstAsmStatement(AstManager *astManager, OpCode opCode):
        AstStatement(astManager, opCode) {}
};

class AstAsmStmt: public AstAsmStatement
{
  public:
    // public constructor not yet implemented
    ASTAPI AsmValue *       GetAsmValue()       { return asmValue; }
    ASTAPI AsmInstruction * GetAsmInstruction() { return asmInstruction; }
  internal:
    AstAsmStmt(AstManager *astManager):
        AstAsmStatement(astManager, ocAsmStmt) {}
    void ParseAsmIl(SymbolTable *symbolTable, AstManager *astManager);
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
    AsmValue *asmValue;
    AsmInstruction *asmInstruction;
};

class AstAsmLabel: public AstAsmStatement
{
  public:
    // public constructor not yet implemented
    ASTAPI AstSymbol * GetLabel() { return (AstSymbol *) inputVector[0]; }
  internal:
    AstAsmLabel(AstManager *astManager):
        AstAsmStatement(astManager, ocAsmLabel) {}
};

class AstAsmAlign: public AstAsmStatement
{
  public:
    // public constructor not yet implemented
    ASTAPI UInt32       GetAsmAlign()       { return asmAlign; }
  internal:
    AstAsmAlign(AstManager *astManager):
        AstAsmStatement(astManager, ocAsmAlign) {}
    // overrides
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
  private:
    UInt32 asmAlign;
};

class AstSehTryExcept: public AstStatement
{
  public:
    ASTAPI AstSehTryExcept(AstManager *astManager, AstStatement *body,
                           AstStatement *handler, AstExpression *filter,
                           SFA sfa = SFANULL);
    ASTAPI AstStatement *GetBody()
      { return (AstStatement *) inputVector[0]; }
    ASTAPI AstStatement *GetHandler()
      { return (AstStatement *) inputVector[1]; }
    ASTAPI AstExpression *GetFilter() // the guard
      { return (AstExpression *) inputVector[2]; }
  internal:
    AstSehTryExcept(AstManager *astManager):
        AstStatement(astManager, ocSehTryExcept) {}
};

class AstSehTryFinally: public AstStatement
{
  public:
    ASTAPI AstSehTryFinally(AstManager *astManager, AstStatement *body,
                            AstStatement *handler, SFA sfa = SFANULL);
    ASTAPI AstStatement * GetBody()
      { return (AstStatement *) inputVector[0]; }
    ASTAPI AstStatement * GetHandler()
      { return (AstStatement *) inputVector[1]; }
  internal:
    AstSehTryFinally(AstManager *astManager): AstStatement(astManager, ocSehTryFinally) {}
};

class AstSehLeave: public AstStatement
{
  public:
    AstSehLeave(AstManager *astManager, SFA sfa = SFANULL):
        AstStatement(astManager, ocSehLeave, sfa) { }
};

class AstTry: public AstStatement
{
  public:
    ASTAPI AstTry(AstManager *astManager, AstBlock *body,
                  AstCatch **handlerVector, int handlerCount,
                  SFA sfa = SFANULL);
    ASTAPI AstBlock * GetBody()         { return (AstBlock *) inputVector[0]; }
    ASTAPI int        GetHandlerCount() { return inputCount - 1; }
    ASTAPI AstCatch **GetHandlerVector(); // REVIEW: was AstBlock ** --RFC
  internal:
    AstTry(AstManager *astManager): AstStatement(astManager, ocTry) {}
};

class AstCatch: public AstStatement
{
  public:
    ASTAPI AstCatch(AstManager *astManager, AstSymbol *caughtSymbol,
                    Type *caughtType, AstBlock *body, SFA sfa = SFANULL);
    ASTAPI AstSymbol * GetCaughtSymbol(); // NULL if no symbol
    ASTAPI Type * GetCaughtType() { return caughtType; } // NULL if catch(...)
    ASTAPI AstBlock * GetBody()   { return (AstBlock *) inputVector[0]; }
  internal:
    AstCatch(AstManager *astManager):
        AstStatement(astManager, ocCatch) {}
    void InitFromIl(IlTree *ilTree, SymbolTable *symTab);
    void SetCaughtType(Type *type) { caughtType = type; }
  private:
    Type *caughtType;
};

///////////////////////////////////////////////////////////////////////////////
// AstManager

#ifndef AST_LIBRARY
typedef unsigned long symkey_t;
#endif

class AstManager: public Object, public WithArena
{
  public:
    ASTAPI AstManager(ArenaAllocator *pAllocator, FrontEndType language);
    ASTAPI ~AstManager();
    ASTAPI AttribKey GetMaxAttribKey() { return (AttribKey)nextKey-1; }
  internal:
    void ParseSy(long syOffset, Bool fInPch, SymbolTable *symbolTable,
                 SymbolTable **labelsTableHandle, TypeManager *typeManager,
                 TypeIO *, SymbolManager *symbolManager);
    AstNode *ParseEx(long exOffset, Bool fInPch, SymbolTable *globalScope,
                     SymbolTable *formalsScope, TypeManager *typeManager,
                     TypeIO *typeIO);
    AstBlock *ParseIn(IlPath *ilModule, SymbolTable *globalScope,
                      TypeManager *typeManager, TypeIO *typeIO);
    void InitializeExParser(IlPath *ilPath);
    void FinalizeExParser();
    void InitializePchParser(long pchChecksym, char* pchFileName);
    ArenaAllocator * GetAllocator()     { return pAllocator; }
    NodeKey NextKey();
#ifdef AST_LIBRARY
    friend bool missingLhsSymbol (IlTree *, AstManager *);
#endif
  private:
    Symbol *LookupSymbol(SymbolKey);
    bool missingLhsSymbol(IlTree *ilTree);

    NodeKey nextKey;

    // IL reader methods and state
    FrontEndType language;
    struct AstStackFrame;

    static SymbolTable *symbolTable;    // to be dynamically changed
    static AstStackFrame *astStackTop;  // to be dynamically changed
    ArenaAllocator *pTempAllocator;     // to be dynamically changed
    IlStream *exStream;
    IlStream *syStream;
    FILE *pchFile;
    IlStream *pchExStream;
    IlStream *pchSyStream;

    void Push(AstNode *astNode);
    AstNode *Pop();
    AstNode *Top();
    AstNode *SecondTop();
    AstExpression *TopExpression();
    void ReverseTopN(int n);
    enum NeedDeref { needLvalue, needRvalue };
    enum HaveDeref { Dereferenced , PlainValue };
    AstNode *ParseEntryBlock(IlTree *ilTree, SymbolTable *globalScope, SymbolTable *formalsScope);
    void ParseIlFunction(IlTree *ilTree);
    void ParseIlStmt(IlTree *ilTree);
    void ParseLhsExpr(IlTree *ilTree, Type *derefType);
    void ParseAsmBlockIlStmt(IlTree *ilTree);
    void ParseBinaryChildren(IlTree *ilTree, NeedDeref needDerefLeft, Type *derefType);
    void ParseHybridList(IlTree *ilTree, Bool fSkip1st, int *childCount);
    void ParseHybridChildren(IlTree *ilTree, int *childCount);
    void ParseAdditionalChildren(IlTree *ilTree, int *childCount);
    void ParseAsmBlockChildren(IlTree *ilTree, int *childCount);
    void InstallChildren(AstNode *node, int count);
    void InstallFunctionChildren(AstNode *node, int count, int formalCount);
    void InstallChildrenSwapped(AstNode *node);
    void InstallChildrenSEHExcept(AstNode *node);
    Bool FIsSymbol(IlTree *ilTree, char *name);
    Bool FIsReturnUdt(IlTree *ilTree);
    Bool FIsInitVBases(IlTree *ilTree);
    Bool FDtoractionTree(IlTree *ilTree);
    TypeFunction *GetFunctionType(Type *fnNodeType);
    // Pushing nodes on the stack
    AstSymbol *PushSymbol(IlTree *symTree);
    void PushClassSymbol(Type *cls, IlTree *iltree);
    void PushFunctionCall(IlTree *call, OpCode callOpCode, int childCount,
                          int firstDefault);
    void PushDefaultConstructor(Type *cls, IlTree *iltree);
    void PushNew(IlTree *newCall);
    void PushNewObject(IlTree *newCall);
    void PushNewArray(IlTree *newCall);
    void PushDelete(IlTree *ilTree, Bool fIsArray);
    void PushAssignment(IlTree *ilTree, OpCode opCode, int childCount);
    void PushNullStmt(IlTree *ilTree);
    void PushNullExpr(IlTree *ilTree);
    void PushUnary(IlTree *ilTree, OpCode opCode);
    void PushCast(IlTree *ilTree, OpCode opCode = ocCast);
    void PushBinary(IlTree *ilTree, OpCode opCode);
    void PushInitialization(IlTree *ilTree);
    void PushExpr(IlTree *ilTree);
    void PushMemberAddress(IlTree *ilTree);

    void RemovePointer(AstExpression *expression);
    void RemovePointerHack(AstExpression *expression);

    void ParseFnArguments(IlTree *ilTree, TypeFunction *calledType,
                          Bool fSkip1st, Bool fSkip1stType,
                          Bool f1stCGisNotOptional,
                          Bool fSkipLast,
                          int *childCount, int *firstDefault);
    void ParseNewCall(IlTree *newCall);
    Bool ParseNewCount(IlTree *newCall);
    void ParseNew(IlTree *newCall, IlTree *constructor);
    // Statement parser functions
    void ParseOPpragma(IlTree *ilTree);
    void ParseOPlabel(IlTree *ilTree);
    void ParseOPasmlabel(IlTree *ilTree);
    void ParseOPgoto(IlTree *ilTree);
    void ParseOPblock(IlTree *ilTree);

    int ParseBlock(IlTree *ilTree,
                   Bool fReducible,
                   Bool fFileScopeInitializer,
                   Bool fConstructor);
    int ParseBlockNoCatch(IlTree *ilTree, IlTree *kid,
                          Bool fReducible,
                          Bool fFileScopeInitializer,
                          Bool fConstructor);
    Bool FIsBaseClassInitializer(IlTree *functionPtr, IlTree *objPtr);
    Bool FIsFieldOfThis(IlTree *objPtr); // OPfield(OPextract("this"),k)
    Bool match_InitVBasesTest(IlTree *ilTree, symkey_t *endLabel);
    Bool match_HiddenVBaseStmt(IlTree *ilStmt);
    Bool match_ctor_init_of_member_with_dtor_but_no_ctor(IlTree *ilStmt);
    Bool match_CG_Call_atexit(IlTree *ilTree);
    void PopSecondKeepingLabels();

    void ParseOPpushstate(IlTree *ilTree);
    void ParseOPtry(IlTree *ilTree);
    void ParseOPdecl(IlTree *ilTree);
    void ParseOPfuncdecl(IlTree *ilTree);
    void ParseOPfuncdefn(IlTree *ilTree);
    void ParseOPinit(IlTree *ilTree);
    void ParseOPentry(IlTree *ilTree);
    void ParseOPreturn(IlTree *ilTree);
    void ParseOPcomma(IlTree *ilTree);
    void ParseOPfor(IlTree *ilTree);
    void ParseOPif(IlTree *ilTree);
    void ParseOPdo(IlTree *ilTree);

    // File scope declarations parser statments
    //   Each returns a count of declarartions parsed.
    int ParseToplevelDecls(IlTree *ilTree);
    int ParseInitializationThunk(Symbol *thunkName);

    // Expression parser functions
    HaveDeref ParseIlExpr(IlTree *ilTree, NeedDeref needDeref, Type *derefType);
    HaveDeref ParseUnaryChild(IlTree *ilTree, NeedDeref needDeref, Type *derefType);
    HaveDeref ParseOPconstant(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPextract(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPquestion(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPpushstate(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPaddress(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPsizeof(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPfield(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPindex(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPmfunc(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPvfunc(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPfunction(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPcomma(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParsePTMFComparisonArgument (IlTree* ilTree);

    HaveDeref ParseOPassign(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPinit(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPcast(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPdyncast(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPplus(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPminus(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPpluseq(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPminuseq(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPintrinsic(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPargument(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);
    HaveDeref ParseOPname(IlTree *ilTree, NeedDeref *needDeref, Type *derefType);

    void MissingOPaddress(IlTree* ilTree, const char *raid_number);


    bool AstManager::match_60NewCall(IlTree *ilTree, IlTree **callTree,
                                     Type **type);
    AstManager::HaveDeref
    AstManager::Parse60NewCall(IlTree *newCall,
                               Type *type,
                               IlTree *rhs,
                               NeedDeref *needDeref,
                               Type *derefType);

};


#endif // INC_NODE_H
