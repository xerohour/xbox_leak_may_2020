/*
 *
 * xapi0dat.c
 *
 * Xapi initializers
 *
 */

#include "dllp.h"

typedef void (__cdecl *PVFV)(void);
typedef int (__cdecl *PIFV)(void);

#pragma data_seg(".CRT$XCA")
PVFV __xc_a[] = { NULL };

#pragma data_seg(".CRT$XCZ")
PVFV __xc_z[] = { NULL };

#pragma data_seg(".CRT$XIA")
PIFV __xi_a[] = { NULL };

#pragma data_seg(".CRT$XIZ")
PIFV __xi_z[] = { NULL };

#pragma data_seg(".CRT$RIA")
PIFV __xri_a[] = { NULL };

#pragma data_seg(".CRT$RIZ")
PIFV __xri_z[] = { NULL };

#ifdef _DEBUG
#pragma data_seg(".CRT$RII15")
extern int __cdecl _RTC_Initialize(void);
void *__rtc_init = _RTC_Initialize;
#endif


#pragma data_seg()

PVFV _FPinit;

#pragma comment(linker, "/merge:.CRT=.data")

static void _initterm(PVFV *a, PVFV *z)
{
	for(; a < z; ++a)
		if(*a != NULL && *a != (PVFV)-1)
			(**a)();
}

static void _initterm_e(PIFV *a, PIFV *z)
{
	for(; a < z; ++a) {
		if(*a != NULL && *a != (PIFV)-1)
			if(0 != (**a)()) {
                RIP("Runtime initialization failed!");
            }
    }
}

void _cinit(void)
{
	/* Init floating point */
	if(_FPinit)
		(*_FPinit)();

	/* Do the initializers */
	_initterm_e(__xi_a, __xi_z);

	/* Now do the constructors */
	_initterm(__xc_a, __xc_z);
}

void _rtinit(void)
{
	/* Do the CRT initializers */
	_initterm_e(__xri_a, __xri_z);
}
