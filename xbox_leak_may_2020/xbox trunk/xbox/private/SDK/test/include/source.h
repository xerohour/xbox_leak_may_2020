// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/source.h 6     5/02/96 4:43p Erikruf $
// $NoKeywords: $
//
// Source file address support


#ifndef INC_SOURCE_H 
#define INC_SOURCE_H

typedef unsigned long LineNumber;
typedef unsigned short LinePosition;
class SymbolFilename;
class SymbolTable;

//
// Source File Address (SFA)
// SFAs may be "NULL".
//
struct SFA
{
  public: 
    ASTAPI SFA(SymbolFilename *file, LineNumber line, LinePosition pos)
        : fileSymbol(file), lineNumber(line), position(pos)
      {}

    ASTAPI const char*    GetFileName() const;
    // Note: may return NULL if SFA is "NULL".
    // Guarantee: if two SFAs point into the same file then
    // GetFileName will return the same pointer (so pointer comparison
    // can be used instead of strcmp)

    ASTAPI LineNumber     GetLineNumber()   const { return lineNumber; }
    ASTAPI LinePosition   GetLinePosition() const { return position; }
    ASTAPI Bool           Equal(const SFA *other) const;

    ASTAPI Bool IsNull() const { return  fileSymbol==NULL; }
    ASTAPI static SFA GetNull() { return SFA(NULL,0,0);}

    // DEPRECATED: use GetFileName()
    ASTAPI SymbolFilename * GetFileSymbol(void) const  { return fileSymbol; }

  internal:
    SFA() : fileSymbol(0), lineNumber(0), position(0) {}
#if AST_LIBRARY
    SFA(ILSFA ilSFA, SymbolTable *symbolTable);
#endif
    //~SFA() {}
    // contrary to our usual scheme, this class can be copied/assigned
    // we'll just use the default copy constructor and assignment operator!
  private: 
    SymbolFilename *fileSymbol;
    LineNumber lineNumber;
    LinePosition position;

    friend Bool Equal(const SFA*, const struct ILSFA&);
};

ASTAPI extern const SFA SFANULL;

 
#endif // INC_SOURCE_H

