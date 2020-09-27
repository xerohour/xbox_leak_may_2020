#include "chain1.h"
#include "chain4.h"

int Global;

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

fooReturn FUNCTYPE foo( fooParams )
{
   DeclareLocals;
   Global=3;
   RESULT = fooAdd;
   ReturnStmt;
};


///////////////////////////////////////////////////////////////////////////
int chain_4(int x)
{
    int Sum_4;
    Sum_4=x+1;
	x=chain_5(x)/2;
    return Sum_4;
};


