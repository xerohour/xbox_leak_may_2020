#ifndef __mmu_h_
#define __mmu_h_

BOOL muInit();
BOOL muDeinit();
void muGetDeviceMessages();
CHAR muGetDriveFromPortSlot(int port, int slot);

#endif