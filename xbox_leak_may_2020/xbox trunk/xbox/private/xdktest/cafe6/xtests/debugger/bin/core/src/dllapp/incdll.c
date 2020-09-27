/*DOC***
**
** Suite : 
** Test  : incdll.c
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

#include "inc.h"
#include "cvtest.h"

void WINAPI Inc( LPWORD wBar ) {

	WORD wLocal;

	wLocal = ++(*wBar);
	++(*wBar);

} /* Inc */


INT WINAPI DllMain(HMODULE hInst, DWORD ulRbc, LPVOID lpReserved) {
// ulRbc is Reason Being Called
	return 1;
} /* IncDllEntryPoint */

