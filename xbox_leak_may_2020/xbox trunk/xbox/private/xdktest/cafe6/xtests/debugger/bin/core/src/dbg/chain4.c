/*DOC*
**
** Suite : 
** File  : chain4.c
**
** Revision History :
** M001 2-Feb-1994
** - created from foodll.c from old callstack sources.
**
**DOC*/

#include <xtl.h> //xbox #include <windows.h>
#include "foo.h"
#include "chain1.h"
#include "chain4.h"

int Global;
// external int chain_5();
typedef int PASCAL CallBackFCN();
int callback(CallBackFCN lpfnCallBack, int x);

///////////////////////////////////////////////////////////////////////////
//INT WINAPI DllEntryPoint(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved) {
int  WINAPI DllEntryPoint(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved) {
// ulRbc is reason bieng called
    return 1;
} /* DllEntryPoint */


#if TEST == cBASIC
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

fooReturn FUNCTYPE foo( fooParams )
{
   DeclareLocals;
START:
   Global=3;
   RESULT = fooAdd;
#ifndef _ALPHA_
   ReturnStmt;
#endif
END:;
#ifdef _ALPHA_
// Alpha optimizes away lables without code after them.
   ReturnStmt;
#endif
};
#endif //TEST == BASIC


///////////////////////////////////////////////////////////////////////////
int  recursive_dll(int lim , int reset)
{
    static cnt=0;
    static limit=0;

START:
    if (reset)
    {
        cnt=0;
        limit=lim;
    }
    cnt++;
    if (cnt < limit )
    {
        recursive_dll(cnt, FALSE);
    }
END:;
    return cnt;
}



///////////////////////////////////////////////////////////////////////////
int chain_4(int x)
{
    int Sum_4;
START:
    Sum_4=x+1;
	x=chain_5(x)/2;
END:;
    return Sum_4;
};



int callback(CallBackFCN lpfnCallBack, int x)
{
    int cnt;
    int sum;
    int tmp;

START:
    cnt=0;
    sum=0;
    tmp=0;
    for (cnt=0; cnt < x ; cnt++)
    {
        tmp = (*lpfnCallBack)();
        sum=sum+tmp;
    };
    return sum;
END:;
};
