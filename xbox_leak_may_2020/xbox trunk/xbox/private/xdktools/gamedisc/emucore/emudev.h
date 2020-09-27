/*
*/

#ifndef _EMUDEVICE_H
#define _EMUDEVICE_H

// Device functions
ULONG DeviceReadData(ULONG cblk, ULONG lba, PUCHAR pbData, PULARGE_INTEGER
    pluDelay);
BOOL DeviceReadCapacity(ULONG *plbaEnd, ULONG *pcbBlk);
BOOL DeviceReadControlData(BOOL fXControl, PUCHAR pb, ULONG cb,
	PULARGE_INTEGER pluDelay);
void DeviceReset(void);
void DeviceEject(void);
void DeviceSetSpindleSpeed(UCHAR n);
USHORT DeviceSelectPage(ULONG cb, PUCHAR pbData, PULARGE_INTEGER pluDelay);
USHORT DeviceSensePage(UCHAR bPage, ULONG cb, PUCHAR pbData,
    PULARGE_INTEGER pluDelay);

// Error reporting
void SetErrorSense(UCHAR b0, UCHAR b1, UCHAR b2);

// IDE
HRESULT StartIDE(void);
void StopIDE(void);

#endif // _EMUDEVICE_H
