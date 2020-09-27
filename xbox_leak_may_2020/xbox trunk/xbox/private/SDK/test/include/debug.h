// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved.
//
// $Header: /ast/sdk/lib/debug.h 3     5/01/96 4:40p Erikruf $
// $NoKeywords: $
//

// Debugging support: assertions, breakpoints, messages

#ifndef INC_DEBUG_H
#define INC_DEBUG_H

#ifndef DECLSPEC_NORETURN
#if (_MSC_VER >= 1200)
#define DECLSPEC_NORETURN __declspec(noreturn)
#else
#define DECLSPEC_NORETURN
#endif
#endif

// forward decls
class FileOutputStream;
class IndentingStream;

class Debug
{
  public:
    // debugging support
    typedef void (*BreakAction)();
    ASTAPI static void        Breakpoint();
    ASTAPI static BreakAction GetBreakpointAction()                  { return breakAction; }
    ASTAPI static void        SetBreakpointAction(BreakAction newBA) { breakAction = newBA; }
    ASTAPI DECLSPEC_NORETURN static void Malfunction();
    ASTAPI DECLSPEC_NORETURN static void UserError(int errcode = 1) { exit(errcode); }
    ASTAPI DECLSPEC_NORETURN static void UserError(char *fmt, ...); // errcode == 1
    ASTAPI DECLSPEC_NORETURN static void UserError(int errcode, char *fmt, ...);
    ASTAPI static void InternalWarning(char *fmt, ...);
    ASTAPI static void InternalWarning(struct SFA *sfa, char *fmt, ...);

    ASTAPI DECLSPEC_NORETURN static void FailAssert(const char *fileName,
                                                    int line,
                                                    const char *details = 0,
                                                    const char *kind = 0);
    ASTAPI static void FailAssertWarn(const char *fileName, int line,
                                      const char *details = 0,
                                      const char *kind = 0);
    // message support
    ASTAPI static void Message(char *fmt, ...);
    ASTAPI static void Message(Bool fDisplay, char *fmt, ...);
    ASTAPI static IndentingStream *pMessageStream;
    ASTAPI static OutputStream *pOutputStream;
    ASTAPI static void SetMessageMode(Bool fEnabled);
    ASTAPI static bool GetMessageMode();

  internal:
    static void Initialize(FILE *messageFile, Bool fEnableMsgs);
    static void Finalize();
    static Bool fMessagesEnabled;
    static BreakAction breakAction;
};

//
// Indent the message stream
//
class MessageIndentor: public Indentor
{
  public:
    ASTAPI MessageIndentor(int incr=1): Indentor(Debug::pMessageStream, incr) {}
    ASTAPI ~MessageIndentor() {}
};

#ifdef AST_INCLUDE_NON_ESSENTIAL
#define MESSAGE_INDENT MessageIndentor theBogusMessageIndentVariableName()
#endif

#ifdef AST_INCLUDE_NON_ESSENTIAL

///////////////////////////////////////////////////////////////////////////////
// "Assert" and friends

// inherit from IDE's notion of debug build
#ifdef _DEBUG
#define AST_DEBUG
#endif

// the usual ; trick
#define AstDoStatement(S)      do S; while (0)
#define AstDoStatementBlock(S) do { S } while (0)

#ifdef AST_DEBUG

#define AstDebug(x)         x
#define AstAssertWarn(b)    \
 AstDoStatement(if (!(b)) AST_NAMESPACE_IZE(Debug::FailAssertWarn)(__FILE__, __LINE__, #b, 0))
#define AstDenyWarn(b)      \
 AstDoStatement(if (b)  AST_NAMESPACE_IZE(Debug::FailAssertWarn)(__FILE__, __LINE__, #b, "ASTLIB deny warning"))

#else // AST_DEBUG

#define AstDebug(x)
#define AstAssertWarn(b)
#define AstDenyWarn(b)

#endif // AST_DEBUG

#define AstAssert(b)        AstDoStatement(if (!(b)) \
                                AST_NAMESPACE_IZE(Debug::FailAssert)(__FILE__, __LINE__, #b, 0))
#define AstDeny(b)          AstDoStatement(if (b) \
                                AST_NAMESPACE_IZE(Debug::FailAssert)(__FILE__, __LINE__, #b, "ASTLIB deny failed"))
#define AstAssertDo(b)      AstAssert(b)

#define AST_NOT_REACHED     AstDoStatement(AST_NAMESPACE_IZE(Debug::FailAssert)(__FILE__, __LINE__, "Unreachable code", 0))
#define AST_NOT_IMPLEMENTED AstDoStatement(AST_NAMESPACE_IZE(Debug::FailAssert)(__FILE__, __LINE__, "Unimplemented code", 0))


// And yes, we know asserts should disappear in production code.
// Someday, we'll figure out how to keep windoze from
// popping up those f@@@ing dialog boxes whenever we GPF
// and then we'll actually be able to TEST our production code
// without going insane.  
// We also need to go through and fix the various asserts
// that really shouldn't be asserts (i.e., that *do* belong
// in the production version).
// So, for now, most of the asserts, i.e.,
// those that aren't mere warnings, are staying in.  --rfc 3/6/98

#if 0

// Eventual definitions for our "retail" code.
#define AstAssert(b)
#define AstDeny(b)
#define AstAssertDo(b)      AstDoStatement(b)
#define AST_NOT_REACHED
#define AST_NOT_IMPLEMENTED

#endif


///////////////////////////////////////////////////////////////////////////////
// C++ RTTI stuff.

// We only want to use RTTI in debug mode, so we'll define our own
// casting operator that resolves to either dynamic_cast or static_cast

#ifdef AST_DEBUG
#define cast dynamic_cast
#else
#define cast static_cast
#endif // AST_DEBUG

// Friendly macros/defs for debugging

#define MESSAGE     AST_NAMESPACE_IZE(Debug::Message)
#define BREAKPOINT  AST_NAMESPACE_IZE(Debug::Breakpoint())
#define MALFUNCTION AST_NAMESPACE_IZE(Debug::Malfunction())
#define USER_ERROR  AST_NAMESPACE_IZE(Debug::UserError)

ASTAPI void D(Object *pObj);

#endif // AST_INCLUDE_NON_ESSENTIAL


#ifdef  AST_UNPREFIX_MACROS
#define Statement(S)        AstDoStatement(S)
#define StatementBlock(S)   AstDoStatementBlock(S)
#define Debug(x)            AstDebug(x)
#define Assert(b)           AstAssert(b)
#define Deny(b)             AstDeny(b)
#define AssertDo(b)         AstAssertDo(b)
#define AssertWarn(b)       AstAssertWarn(b)
#define DenyWarn(b)         AstDenyWarn(b)
#define NOT_REACHED         AST_NOT_REACHED
#define NOT_IMPLEMENTED     AST_NOT_IMPLEMENTED
#endif

#endif // INC_DEBUG_H
