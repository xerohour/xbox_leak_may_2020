/*

Copyright (c) 2000 Microsoft Corporation

Module Name:

    MUTGen.h

Abstract:

    This module is used to generate usb traffic for load analysis
Author:

    Dennis Krueger (a-denkru)

Environment:

    XBox

Revision History:
    7-21-2000  Created

Notes:

*/

// sizes
#define KILOBYTE 1024
#define MEGABYTE (KILOBYTE*KILOBYTE)
#define PAGESIZE (4*KILOBYTE)

void Initialize(void);
#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif

#define DO_WRITE 1
#define DO_READ  2

void InputDukeInsertions(DWORD add, DWORD remove=0);
void InputMUInsertions(DWORD add, DWORD remove=0);
bool InputCheckExitCombo(void);

typedef struct {
	char			drive;
	unsigned int	port;
	unsigned int	slot;
	char *			pSourceBuffer;
} THREAD_PARAMS, *PTHREAD_PARAMS;

void DoDiskIO(PTHREAD_PARAMS pParams );
void UpdateScreen();

static const DWORD USBPortMasks[] = {
    XDEVICE_PORT0_MASK,
    XDEVICE_PORT1_MASK,
    XDEVICE_PORT2_MASK,
    XDEVICE_PORT3_MASK
    };
static const DWORD USBSlotMasks[] = {
    XDEVICE_PORT0_BOTTOM_MASK,
    XDEVICE_PORT0_TOP_MASK,
    XDEVICE_PORT1_BOTTOM_MASK,
    XDEVICE_PORT1_TOP_MASK,
    XDEVICE_PORT2_BOTTOM_MASK,
    XDEVICE_PORT2_TOP_MASK,
    XDEVICE_PORT3_BOTTOM_MASK,
    XDEVICE_PORT3_TOP_MASK
    };
