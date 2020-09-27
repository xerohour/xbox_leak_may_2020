/*DOC*
**
** Suite : 
** File  : foo0x.c
**
** Revision History :
** M001 2-Feb-1994
** - created from foo0x.c from old callstack sources.
**
**DOC*/

#include <string.h>
#include "foo.h"
#include "foo0x.h"

// #if TEST == cBASIC
// #ifdef DLL                                    // define DLL to enable dll tests
#include "chain4.h"
// #else
#include "foox.h"
// #endif //DLL
// #endif //cbasic

#ifdef hasPARAMS
#define  fooArgs  param1, param2
#else
#define  fooArgs
#endif

#ifdef returnsVALUE
#define  fooRet  Result=
#else
#define  fooRet
#endif

#define FOO  fooRet foo(fooArgs);

// #if TEST == cCXX
  void Cxx_Tests();
// #endif

int param1=1;
int param2=2;
int index=0;


extern int Global; // =0;
extern int Result; // =0;
extern char * String; // =" Some sample string";

int gi[5] = {0, 0, 0, 0, 0};

int OtherMain() {
int local;

  local=255;

#if TEST == cBASIC
  callFOO:

  FOO;

  RETURN:;

#endif // TEST == cBASIC



  BPWithLengthTest();
  callCXX:
  Cxx_Tests();
  (void)foo2(7, 1);
  TagStepEH: DoCppException();


  CALL_RUNTIME:;
  Result=strlen(String);


DONE:
  return 1;
};


void BPWithLengthTest(void)
{
	START:	
	gi[4]=99;
	gi[2]=99;  // breakpoint should fire and stop 
	BREAK:     // at the next line of code tagged with the BREAK label
	gi[2]=0; gi[3]=99;  
	END:
	return;
}
