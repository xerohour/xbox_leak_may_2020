// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/stream.h 9     5/01/96 4:40p Erikruf $
// $NoKeywords: $
//

// Simple text output streams


#ifndef INC_STREAM_H 
#define INC_STREAM_H

#include <stdio.h>
#include <stdarg.h>

///////////////////////////////////////////////////////////////////////////////
// Class structure (forward declarations) for CFG nodes
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

class OutputStream;
class   DebugOutputStream;
class   FileOutputStream;
class   StringStream;
class   NullOutputStream;
class   IndentingStream;
class     DumpContext;

class Indentor;

////END////

//
// General output streams (abstract class)
//
class OutputStream: public Object
{
  public: 
    ASTAPI virtual      ~OutputStream() {}
    ASTAPI virtual void Putc(char c);
    ASTAPI virtual void Puts(const char *buf) = 0;
    ASTAPI void         Printf(const char *format, ...);
    ASTAPI virtual void Vprintf(const char *format, va_list data);
    ASTAPI virtual void Flush()         = 0;
    ASTAPI void         Newline();
  protected:
    ASTAPI OutputStream() {};
};

//
// Debug output streams
// These write either to stderr or the debugger transcript window.
//
class DebugOutputStream: public OutputStream
{
  public: 
    enum DebugOutputMode {toStderr, toDebugger};

    ASTAPI         DebugOutputStream(DebugOutputMode m = toStderr) : mode(m) {}
    ASTAPI virtual ~DebugOutputStream()                                      {}

    ASTAPI void         SetMode(DebugOutputMode m) { mode = m; }
    // overrides
    ASTAPI virtual void Puts(char *buf);
    ASTAPI virtual void Flush();
  private:
    DebugOutputMode mode;
};

//
// File output streams
// These write to a specified stdio file descriptor.
//
class FileOutputStream: public OutputStream
{
  public:
    ASTAPI         FileOutputStream(FILE *f): file(f) {}
    ASTAPI virtual ~FileOutputStream()                {}

    ASTAPI void         Redirect(FILE *f); 
    // overrides
    ASTAPI virtual void Puts(const char *buf);
    ASTAPI virtual void Putc(char c);
    ASTAPI virtual void Vprintf(const char *format, va_list data);
    ASTAPI virtual void Flush();
  private: 
    FILE *file;
};

//
// String streams
// These write to an extensible string
//
// This implementation uses a small initial non-heap buffer and moves
// the string into the heap if the buffer overflows.  This makes
// StringStreams quite efficient for making small formatted strings.


class StringStream: public OutputStream
{
  public:
    ASTAPI StringStream();
    ASTAPI virtual ~StringStream();

    // Additional operations
    ASTAPI unsigned int GetLength() { return cursor; }
    ASTAPI const char* GetString();

    ASTAPI void Reset();  // "empties" string stream
    ASTAPI void Set(const char *contents); // copies
    ASTAPI void Prepend(const char *buf);
    ASTAPI void PrePrintf(const char *format, ...);

    //overrides
    ASTAPI virtual void Putc(char c);
    ASTAPI virtual void Puts(const char *buf);
    ASTAPI virtual void Vprintf(const char *format, va_list data);
    ASTAPI virtual void Flush() {}

  private:
    void Ensure(unsigned int needed_length);
    void Grow(unsigned int needed_length);
    static char* Alloc(unsigned int bytes);
    ASTAPI void  FreeAccumulator();

    unsigned int length;
    unsigned int cursor;
    char *accumulator;

    enum { initial_length = 128 };
    char initial_accumulator[initial_length];
};


// inlined StringStream methods

__inline StringStream::StringStream()
    : length(initial_length),
      accumulator(initial_accumulator),
      cursor(0)
{}

__inline StringStream::~StringStream()
{
    if (accumulator != initial_accumulator)
      FreeAccumulator();
}

__inline const char *StringStream::GetString()
{
    // safe assignment: see Ensure.
    accumulator[cursor] = '\0';
    return accumulator;
}

__inline void StringStream::Reset() { cursor = 0; }

//
// /dev/null output stream
// This doesn't actually write anything anywhere.
//
class NullOutputStream : public OutputStream
{
  public:
    // ASTAPI           NullOutputStream()               {}
    ASTAPI virtual      ~NullOutputStream()              {}

    // overrides
    ASTAPI virtual void Puts(const char *buf)                     {}
    ASTAPI virtual void Putc(char c)                              {}
    ASTAPI virtual void Vprintf(const char *format, va_list data) {}
    ASTAPI virtual void Flush()                                   {}
};

// 
// Indenting streams
//
class IndentingStream: public OutputStream
{
  public: 
    ASTAPI         IndentingStream(OutputStream *baseStream);
    ASTAPI virtual ~IndentingStream();

    ASTAPI void         Indent(int incr = 1);
    ASTAPI void         Unindent(int decr = 1);
    ASTAPI void         ResetIndent();
    // overrides
    ASTAPI virtual void Puts(const char *buf);
    ASTAPI virtual void Flush();

  private: 
    OutputStream *pBaseStream;
    int currentIndent;
    Bool pendingIndent;
};

//
// Convenient way to indent/unindent a stream
//
// Declare an Indentor variable and let the constructor/destructor do the
// indenting and unindenting.
// 
class Indentor
{
  public: 
    ASTAPI Indentor(IndentingStream *baseStream, int incr=1);
    ASTAPI ~Indentor();
  private:
    IndentingStream *baseStream;
    int incr;
};      


#ifdef AST_INCLUDE_NON_ESSENTIAL
 #define INDENT(pDC) AST_NAMESPACE_IZE(Indentor) theBogusIndentVariableName(pDC)
 #define INDENT2(pDC, amount) AST_NAMESPACE_IZE(Indentor) theBogusIndentVariableName((pDC), (amount))
#endif

//
// Dump Contexts
//
class DumpContext: public IndentingStream
{
  public: 
    ASTAPI DumpContext(OutputStream *baseStream, int depth)
        : IndentingStream(baseStream), depth(depth) {} 
    ASTAPI virtual ~DumpContext() {} 
    int depth;
};

#endif // INC_STREAM_H

