#include <windows.h>
#include "utils.h"
#include "failure.h"
#include "dbgr.h"
#include "direct.h"


DWORD DrctGetMachineType(VOID)
{
	ULONG Type = DbgGetProcType();

	//IMAGE_FILE_MACHINE_I386              0x014c
	//IMAGE_FILE_MACHINE_IA64              0x0200
	//IMAGE_FILE_MACHINE_ALPHA             0x0184
	//IMAGE_FILE_MACHINE_ALPHA64           0x0284

	if (Type == IMAGE_FILE_MACHINE_IA64)
		return MACH_IA64;
	else if (Type == IMAGE_FILE_MACHINE_ALPHA64)
		return MACH_ALPHA64;
	else if (Type == IMAGE_FILE_MACHINE_ALPHA)
		return MACH_ALPHA;
	else
		return MACH_X86;
}