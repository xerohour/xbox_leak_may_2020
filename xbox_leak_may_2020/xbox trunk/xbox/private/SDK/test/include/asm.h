// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/alloc.cpp 8     5/02/96 4:43p Rusa $
// $NoKeywords: $
//
// 

#ifndef INC_ASM_H
#define INC_ASM_H

///////////////////////////////////////////////////////////////////////////////
// Class structure (forward declarations)

class AsmInstruction;
class  AsmGenericInstruction;
class  Asm386Instruction;

///////////////////////////////////////////////////////////////////////////////
// Exterior definitions

#ifdef AST_LIBRARY

enum Asm386OpCode {
#define CC_ASM 1
#define _VC_VER_INC
#if CC_VERSION60
#define CC_P5P6ASM 1
#endif
#define DAT(op, dope, num_oprs, osize, byte2, name, ACLS, ASM, template, XX, machine) ASM_O ## op,
#include "386\asmdat.h"
#undef CC_ASM
#undef _VC_VER_INC
#if CC_VERSION60
#undef CC_P5P6ASM
#endif
#undef DAT
};
#include "md.h"
typedef e_asmmode Asm386Mode;

#else AST_LIBRARY

enum Asm386OpCode;
enum Asm386Mode;

#endif AST_LIBRARY

///////////////////////////////////////////////////////////////////////////////
// Nodes

class AsmInstruction: public Object
{
  public:
    enum JumpKind { jkNoJump, jkUncond, jkCond, jkReturn, jkCall, jkUnknown };
    ASTAPI virtual void Disassemble(StringStream& stream)  = 0;
    ASTAPI virtual JumpKind GetJumpKind()                         = 0;
    ASTAPI virtual Symbol * JumpDestination()                     = 0;
  internal:
    AsmInstruction(Byte *codeBytes, int length, SymbolTable *symbolTable) {}
};

class AsmGenericInstruction : public AsmInstruction
{
  public:
    ASTAPI void  Disassemble(StringStream& stream);
    ASTAPI JumpKind GetJumpKind()     { return jkUnknown; }
    ASTAPI Symbol * JumpDestination() { return NULL; }
  internal:
    AsmGenericInstruction(Byte *codeBytes, int length, SymbolTable *symbolTable);
  private:
    Byte *codeBytes;
    int codeLength;
};

class Asm386Instruction : public AsmInstruction
{
  public:
    ASTAPI void Disassemble(StringStream& stream);
    ASTAPI JumpKind GetJumpKind();
    ASTAPI Symbol * JumpDestination();
  internal:
    Asm386Instruction(Byte *codeBytes, int length, SymbolTable *symbolTable);
  private:
    union SizeCC {
        Byte CC;
        Byte Size;
    };
    struct Asm386Operand {
        Asm386Mode mode;
        union {
            Symbol *symbol;
            Byte reg;
        };
        Byte segreg;
        Byte basereg;
        Byte indexreg;
        Byte scaling;
        Byte prefixreg;
        Symbol *prefixSymbol;
        union {
            long displacement;
            long offset;
            long immediate;
        };
    };

    static void DisassembleOperand(const Asm386Operand& operand,
                                   Byte size,
                                   StringStream& stream);
    static void DisassembleFloatOperand(const Asm386Operand& operand,
                                        Byte size,
                                        StringStream& stream);
    Asm386OpCode asmOpCode;
    SizeCC sizeCC;
    int operandCount;
    Asm386Operand operands[ 3 ];
};

#endif // INC_ASM_H
