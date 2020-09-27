#include "xtl.h"
#include "xbox.h"
#include "tsc.h"
//#include "device.h"
#include "mmu.h"


extern "C" void DebugPrint(char *, ...);
extern UINT64					gLastUpdate		[];			//last update of each slot
extern UINT64					gTimeStart;					//beginning of time for this program

CHAR gMuDrive[32];


BOOL muInit()
{
	int i;
	DWORD insert;
	int port, slot;
	DWORD temp;

	DebugPrint("Getting MU devices...\n");
	insert = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

	for(i = 0; i < 32; i++) {
		port = i & 15;
		slot = i < 16 ? XDEVICE_TOP_SLOT : XDEVICE_BOTTOM_SLOT;
		if(insert & (1 << i)) {
			gLastUpdate[port] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
			DebugPrint("MU inserted in slot %x --", i);
			temp = XMountMU(port, slot, &gMuDrive[i]);
			DebugPrint("XMountMU: %d -- ", temp);
			DebugPrint("Drive: %c\n", gMuDrive[i]);
		} else {
			gMuDrive[i] = '\0';
		}
	}

	return TRUE;
}

BOOL muDeinit()
{
	int i;
	for(i = 0; i < 32; i++) {
		if(gMuDrive[i] != '\0') {
			XUnmountMU(i & 15, i < 16 ? XDEVICE_TOP_SLOT : XDEVICE_BOTTOM_SLOT);
		}
	}
	return TRUE;
}

void muGetDeviceMessages()
{
	DWORD insert;
	DWORD remove;

	int port, slot;

	int i, j;
	int temp;

//	DebugPrint("Getting MU device changes\n");
	if(XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &insert, &remove)) {
		for (i = 0; i < 32; i++) {
			port = i & 15;
			slot = i < 16 ? XDEVICE_TOP_SLOT : XDEVICE_BOTTOM_SLOT;
			if(remove & (1 << i)) { 
				DebugPrint("MU removed from slot %x.\n", i);
				gLastUpdate[port] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
				if(gMuDrive[i] != '\0') XUnmountMU(port, slot);
				gMuDrive[i] = '\0';
			}

			if(insert & (1 << i)) {
				gLastUpdate[port] = TscTicksToUSeconds(TscCurrentTime(gTimeStart));
				DebugPrint("MU inserted in slot %x --", i);
				temp = XMountMU(port, slot, &gMuDrive[i]);
				if(temp != 0) gMuDrive[i] = '\0';
				DebugPrint("XMountMU: %d -- ", temp);
				DebugPrint("Drive: %c\n", gMuDrive[i]);
			}
		}
	}
}

CHAR muGetDriveFromPortSlot(int port, int slot)
{
	return gMuDrive[port+ (slot?16:0)];
}
