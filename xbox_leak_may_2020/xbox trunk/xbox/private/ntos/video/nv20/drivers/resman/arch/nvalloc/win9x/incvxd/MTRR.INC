ifndef  _MTRR_H_
_MTRR_H_        EQU     1
MTRR_Ver_Major  EQU     01H
MTRR_Ver_Minor  EQU     00H
MTRR_DEVICE_ID  EQU     <DDS_DEVICE_ID>
MmNonCached     EQU     0
MmCached        EQU     1
MmFrameBufferCached     EQU     2
MmHardwareCoherentCached        EQU     3
MmMaximumCacheType      EQU     4
Begin_Service_Table MTRR, VxD
MTRR_Service             _MTRR_Get_Version, LOCAL
MTRR_StdCall_Service     MTRRSetPhysicalCacheTypeRange, 4
End_Service_Table MTRR, VxD
endif
