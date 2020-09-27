#ifndef _INCLUDE_MULTIDEV
#define _INCLUDE_MULTIDEV


ULONG	ulQueryNumDevices(PVOID pvDeviceObject);
BOOLEAN bGetDevices(PVOID pvDeviceObject, PVOID *ppvDevObjs);

#endif // _INCLUDE_MULTIDEV
