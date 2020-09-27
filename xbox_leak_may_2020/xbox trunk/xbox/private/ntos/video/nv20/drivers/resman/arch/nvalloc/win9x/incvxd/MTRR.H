
#ifndef	_MTRR_H_
#define	_MTRR_H_

#define	MTRR_Ver_Major	0x01
#define	MTRR_Ver_Minor	0x00

#define	MTRR_DEVICE_ID	DDS_DEVICE_ID

/*INT32*/

/*XLATOFF*/
#define MTRR_Service	Declare_Service
#define MTRR_StdCall_Service Declare_SCService
#pragma	warning (disable:4003)

/*XLATON*/

#define		MmNonCached			0
#define		MmCached			1
#define		MmFrameBufferCached		2
#define		MmHardwareCoherentCached        3
#define		MmMaximumCacheType		4

/*MACROS*/
Begin_Service_Table(MTRR, VxD)

MTRR_Service		(_MTRR_Get_Version, LOCAL)
MTRR_StdCall_Service    (MTRRSetPhysicalCacheTypeRange, 4)

End_Service_Table(MTRR, VxD)

/*ENDMACROS*/

/*XLATOFF*/

WORD VXDINLINE
MTRRGetVersion() {

   WORD w;
   VxDCall(_MTRR_Get_Version);
   _asm mov [w], ax;
   return (w);

}

ULONG VXDINLINE
MTRRSetPhysicalCacheTypeRange(
   PVOID PhysicalAddress,
   ULONG NumberOfBytes,
   MEMORY_CACHING_TYPE CacheType) {

   ULONG	ulResult;

   _asm push CacheType;
   _asm push NumberOfBytes;
   _asm push 0;
   _asm push PhysicalAddress;
   VxDCall(MTRRSetPhysicalCacheTypeRange);
   _asm	mov [ulResult], eax;
   
   return (ulResult);
}

/*XLATON*/

#endif // ifndef _MTRR_H_
