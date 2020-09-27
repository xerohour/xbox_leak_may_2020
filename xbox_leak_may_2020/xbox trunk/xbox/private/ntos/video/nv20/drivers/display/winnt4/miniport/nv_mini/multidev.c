// this module is compiled in DDK environment cause we need to deal with ddk.h only structs to walk 
// thru the list of devices!!!
// for this all routines have standard windows parameters like pvoid, ulong etc.

#include <ntddk.h>
#include <windef.h>
#undef WIN32
#include <nvos.h>
#include <nvntioctl.h>


// 
// ulQueryNumDevices walks thru the linked list of device objects and counts all devices which belong to one driver
// NOTE : a DeviceObject is the same than a hDriver in the Display driver
//
ULONG ulQueryNumDevices(PVOID pvDeviceObject)
{
	ULONG			ulNumDevices = 1;  // default is one device
	DEVICE_OBJECT   *pDevObj;
	DEVICE_OBJECT   *pRootDevObj;
	DRIVER_OBJECT   *pDrvObj;

	pDevObj =(DEVICE_OBJECT *)pvDeviceObject;
	if (pDevObj)
	{
		pDrvObj = pDevObj->DriverObject;
		if (pDrvObj)
		{
			pRootDevObj = pDrvObj->DeviceObject;
			pDevObj = pRootDevObj;
			while (pDevObj && pDevObj->NextDevice)
			{
				pDevObj = pDevObj->NextDevice;
				ulNumDevices++;
			}
		}
	}
	return ulNumDevices;
}

//
// bGetDevices walks thru the list of device objects and exports them thru ppvDevObjs
//
BOOLEAN bGetDevices(PVOID pvDeviceObject, PVOID *ppvDevObjs)
{
	BOOLEAN			bRet = FALSE;
	int				i = 0;
	DEVICE_OBJECT *pDevObj;
	DEVICE_OBJECT *pRootDevObj;
	DRIVER_OBJECT *pDrvObj;

	pDevObj =(DEVICE_OBJECT *)pvDeviceObject;
	if (pDevObj)
	{
		pDrvObj = pDevObj->DriverObject;
		if (pDrvObj)
		{
			bRet = TRUE;

			pRootDevObj = pDrvObj->DeviceObject;
			pDevObj = pRootDevObj;

			ppvDevObjs[i++] = (PVOID)pDevObj;

			while (pDevObj && pDevObj->NextDevice)
			{
				pDevObj = pDevObj->NextDevice;
				ppvDevObjs[i++] = (PVOID)pDevObj;
			}
		}
	}
	return bRet;
}