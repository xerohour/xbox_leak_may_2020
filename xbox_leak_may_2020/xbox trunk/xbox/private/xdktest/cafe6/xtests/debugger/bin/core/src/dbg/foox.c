/*DOC*
**
** Suite : 
** File  : foox.c
**
** Revision History :
** M001 2-Feb-1994
** - created from foox.c from old callstack sources.
**
**DOC*/

#include "foo.h"
#include "foox.h"

int Global;


////////////////////////////////////////////////////////////////////////////
//
// foo()
//
//
//      foo is a chamelion function it return value, calling convention,
//      paramters, and use of locals is determined by defining certain
//      FLAGS at compile time (ie hasLOCALS, hasPARAMS, returnsVALUE, DLL)
//
//
//
////////////////////////////////////////////////////////////////////////////
#ifdef hasLOCALS
#define  DeclareLocals   int Sum;
#define  RESULT          Sum
#else
#define  DeclareLocals
#define  RESULT          Global
#endif

#ifdef hasPARAMS
#define  fooAdd          paramA+paramB
#else
#define  fooAdd          Global+3
#endif

#ifdef returnsVALUE
#define  ReturnStmt      return Global
#else
#define  ReturnStmt
#endif

fooReturn	FUNCTYPE foo2(fooParams)
{
   DeclareLocals;
START:
   Global=3;
   RESULT = fooAdd;
   ReturnStmt;
END:;
};
