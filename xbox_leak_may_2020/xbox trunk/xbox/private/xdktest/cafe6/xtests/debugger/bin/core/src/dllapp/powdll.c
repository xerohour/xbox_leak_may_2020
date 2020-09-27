/*DOC***
**
** Suite : 
** Test  : 
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
#include "pow.h"
#include "cvtest.h"

double WINAPI dPow(int base, int power) {
	
	int i;
	double result;
	/* note: don't handle zero's */
	if (power==0)
		return 1;
	if (base==0)
		return 0;
	for (result=base,i=1;i<power;i++)
		result *= base;

	return result;
} /* pow */


INT WINAPI DllMain(HMODULE hInst, DWORD ulRbc, LPVOID lpReserved) {
// ulRbc is Reason Being Called
	return 1;
} /* DllMain */

