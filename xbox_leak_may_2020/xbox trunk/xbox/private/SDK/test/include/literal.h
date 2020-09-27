// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/literal.h 4     5/02/96 4:43p Erikruf $
// $NoKeywords: $
//
// 

#ifndef INC_LITERAL_H 
#define INC_LITERAL_H

enum ConstantKind
{
    ckIntegral,
    ckString,
    ckWString,
    ckFloating
};

class ConstantValue: public Object
{
  public:
    ASTAPI ConstantKind   GetKind()  { return kind; }
    ASTAPI virtual char * AsString() = 0;

  protected:
    ASTAPI ConstantValue(ConstantKind kind);
    ConstantKind kind;
};

class IntegralValue: public ConstantValue
{
  public:
    // Use Make to create an IntegralValue. Common values (e.g. 0) are
    // pre-allocated and shared.  Other values are allocated on demand.
    ASTAPI static IntegralValue *Make(Allocator*, __int64 value, Bool fSigned);
    ASTAPI __int64 GetValue() { return value; }
    ASTAPI Bool    FSigned()  { return fSigned; }
    ASTAPI virtual char *AsString();
  private: 
    IntegralValue(__int64 value, Bool fSigned);
    __int64 value;
    Bool fSigned;
};

class FloatingValue: public ConstantValue
{
  public:
    ASTAPI FloatingValue(long double value): 
        ConstantValue(ckFloating), value(value) 
      { }
    ASTAPI long double    GetValue() { return value; }
    ASTAPI virtual char * AsString();
  private:
    long double value;
};

class StringValue: public ConstantValue
{
  public:
    ASTAPI StringValue(int cBytes, char *pBytes): // N.B.: doesn't copy string!
        ConstantValue(ckString), cBytes(cBytes), pBytes(pBytes),
        isWide(FALSE) {}
    ASTAPI int            GetCBytes() { return cBytes; }
    ASTAPI char *         GetPBytes() { return pBytes; }
    ASTAPI virtual char * AsString();
    ASTAPI Bool           FWide()     { return isWide; } // TRUE for wide strings
  internal:
    void SetWide() { kind = ckWString; }
    void Unparse(class OutputStream *);
  private:
    int cBytes;
    char *pBytes;
    Bool isWide;
};

class AsmValue: public Object
{
  public:
    ASTAPI AsmValue(int length, Byte *codeBytes): // N.B. doesn't copy code bytes!
        length(length), codeBytes(codeBytes) {}
    ASTAPI int    GetLength()    { return length; }
    ASTAPI Byte * GetCodeBytes() { return codeBytes; }
  private:
    int length;
    Byte *codeBytes;
};

#endif // INC_LITERAL_H
