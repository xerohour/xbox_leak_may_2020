/*DOC************************************************************************
**
** Suite : Callstack
** File  : foodll.c       DLL calls
**
** Purpose : test Codeview callstack
**
** General : callstack
** Primary : redirect
** Secondy : file comparison
**
** Dependencies : none (no screen dumps)
**
** Products : CV420
**
** Revision History :
**
**DOC*************************************************************************/
#include <windows.h>
#include "main.h"
#include "dll.h"

int Global;

///////////////////////////////////////////////////////////////////////////
//INT WINAPI DllEntryPoint(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved) {
int  WINAPI DllEntryPoint(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved) {
// ulRbc is reason bieng called
    return 1;
} /* DllEntryPoint */


////////////////////////////////////////////////////////////////////////////

int FAR  foo( int paramA, int paramB )
{
   int Sum;
START:
   Global=3;
   Sum = paramA+paramB;
   return Global;
END:;
};


///////////////////////////////////////////////////////////////////////////
int  FAR recursive_dll(int lim , int reset)
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
int FAR callback(CallBackFCN lpfnCallBack, int x)
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
