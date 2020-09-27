/*DOC***
**
** Suite : bvt
** Test  : dec2.c
**
** Purpose : test Codeview stepping model
**
** General : stepping
** Primary : redirect
** Secondy : file comparison
**
** Dependencies : none (no screen dumps)
**
** Products : CV410
**
** Revision History :
**
** Mxxx	dd-Mon-yy	email name
** - description
** M000	28-Jun-92	waltcr
** - created
** M001 30-Jun-92	waltcr
** - add local var
**
**
**
**DOC***/

#if defined( WIN )
#include <windows.h>
#endif //WIN

#include "dec.h"
#include "inc.h"
#include "pow.h"
#include "cvtest.h"

typedef double (APIENTRY *PFNDLL)(int,int);

void WINAPI Dec( LPWORD wBar ) {

	WORD wLocal;
	HANDLE  hDLL;
	PFNDLL dpfnPow;
	double dValue;
	char sz[80];

SAVELABEL( TagDecRet );
SAVELABEL( TagAfterGetProcAddr );
SAVELABEL( TagCallPow );

	wLocal = ++(*wBar);
	--(*wBar);
	Inc(wBar);   // call another implib'd dll.

	/* do a loadlibrary of another dll from this dll */
	TagLoadLib: hDLL = LoadLibrary("POWDLL.DLL");
	if( hDLL != NULL )
	{
		dpfnPow = (PFNDLL) GetProcAddress(hDLL, "dPow");
TagAfterGetProcAddr:
		if (dpfnPow)
		{
TagCallPow:		dValue = dpfnPow( 10, 3 );
TagAfterPow:	dValue = dValue+1;
		}
		FreeLibrary(hDLL);
	}

TagDecRet: ; } /* Dec */


INT WINAPI DllEntryPoint(HANDLE hInst, ULONG ulRbc, LPVOID lpReserved) {
// ulRbc is Reason Being Called
	return 1;
} /* DllEntryPoint */
