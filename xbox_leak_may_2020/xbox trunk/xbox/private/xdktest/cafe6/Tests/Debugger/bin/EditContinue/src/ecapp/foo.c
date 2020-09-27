#include <string.h>
#include "foo.h"
#include "chain4.h"

#ifdef hasLOCALS
#define  DeclareLocals   int Sum;
#define  RESULT          Sum
#else
#define  DeclareLocals
#define  RESULT          Global
#endif

#ifdef hasPARAMS
#define  fooAdd          paramA+paramB
#define  fooArgs  param1, param2
#else
#define  fooAdd          Global+3
#define  fooArgs
#endif

#ifdef returnsVALUE
#define  ReturnStmt      return Global
#define  fooRet  Result=
#else
#define  ReturnStmt
#define  fooRet
#endif

#define FOO  fooRet foo(fooArgs);

void Cxx_Tests();

int param1=1;
int param2=2;
int index=0;

extern int Global; // =0;
extern int Result; // =0;
extern char * String; // =" Some sample string";

int gi[5] = {0, 0, 0, 0, 0};

fooReturn	FUNCTYPE foo2(fooParams)
{
   Global=3;
   Global = fooAdd;
   ReturnStmt;
};

int OtherMain()
{
	int local, i;

	local=255;

	FOO;

	BPWithLengthTest();

	for(i=0; i<9; i++)
	{
		Cxx_Tests();
	}

	(void)foo2(7, 1);

	// CALL_RUNTIME:;
	Result=strlen(String);

	return 1;
}


void BPWithLengthTest(void)
{
	gi[4]=99;
	gi[2]=99;  // breakpoint should fire and stop 
	//BREAK:     // at the next line of code tagged with the BREAK label
	gi[2]=0; gi[3]=99;  
	return;
}
