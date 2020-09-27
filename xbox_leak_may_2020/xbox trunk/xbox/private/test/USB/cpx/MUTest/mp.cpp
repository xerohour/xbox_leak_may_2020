#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdddisk.h>
#include <stdio.h>
#include <xtl.h>
extern "C" {
#include <ntos.h>
#include <xapip.h>  //private XAPI methods for low-level tests
}
#include "mutest.h"




BOOL FormatMU(int port, int slot)
{
    CHAR device[32]; // (mu port numbers are hex values, 1 based)
    CHAR deviceNameBuffer[64];
    STRING deviceName;

	// set failure status
    BOOL fStatus = TRUE;
    
    //
    // Initialize the device name buffer
    //
    deviceName.Buffer = deviceNameBuffer;
    deviceName.Length = 0;
    deviceName.MaximumLength = 63;

    DebugPrint("Formatting Device\n");
    NTSTATUS status = MU_CreateDeviceObject(port, slot, &deviceName);
    if(NT_ERROR(status))
    {
        DebugPrint("Couldn't create device object\n");
        goto do_nothing;
    }

    if(XapiFormatFATVolume(&deviceName)) {
		DebugPrint("Format succeeded\n");
        fStatus = FALSE;
	} else {
		DebugPrint("Format failed (ec: %lu)\n", GetLastError());
	}

	MU_CloseDeviceObject(port, slot);

do_nothing:
   
    return fStatus;
}

