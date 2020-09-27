// header file for nvrmcomm.c

BOOL NvRmOpen();
BOOL NvRmClose();
BOOL NvRmIoControl(PVIDEO_REQUEST_PACKET RequestPacket);
BOOL NvrmIoctl(PVIDEO_REQUEST_PACKET RequestPacket);

