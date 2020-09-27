// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/module.h 7     5/07/96 5:15p Erikruf $
// $NoKeywords: $
//
// 


#ifndef INC_MODULE_H 
#define INC_MODULE_H

class Module: public Object, public WithOwnArena
{
  public: // client methods
    ASTAPI Module(IlPath *ilPath);
    ASTAPI ~Module();
    ASTAPI void Read();   
    ASTAPI void Write(char *outpath);
    ASTAPI SymbolTable *   GetGlobalSymbolTable() { return globalSymbolTable; }
    ASTAPI AstBlock *      GetInitBlock()         { return initBlock; }
    ASTAPI SymbolManager * GetSymbolManager()     { return symbolManager; }
    ASTAPI TypeManager *   GetTypeManager()       { return typeManager; }
    ASTAPI AstManager *    GetAstManager()        { return astManager; }

    ASTAPI FrontEndType    GetLanguage()          { return ilPath->GetLanguage(); }

    ASTAPI ModuleFunctionIterator GetFunctionIterator();

    // DEPRECATED:
    ASTAPI Type * LookupType(TypeIndex);
        // used for program mutation (modification)

  internal:
    // for debugging, we want to make the allocator visible;
    WithOwnArena::pAllocator;

    static Module *current;  // fluid bound during reading.

    ASTAPI Module(IlPath *ilPath, bool usesPCH);

    Bool FUsesPCH() { return fUsesPCH; }
    void SetfUsesPCH( Bool usesPCH ) { fUsesPCH = usesPCH; }

  private:
    // bookkeeping stuff
    SymbolManager *symbolManager;
    TypeManager *typeManager;
    AstManager *astManager;
    TypeIO *typeIO;  // reads/writes type info to/from .pdb/.db
    // state
    IlPath *ilPath;
    SymbolTable *globalSymbolTable;
    AstBlock *initBlock;
    bool fUsesPCH;  // compiled with -Yu

};


#endif // INC_MODULE_H
