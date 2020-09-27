; This file should mirror info in Surfaces.h ,vpcallnk.h, and VidTex.h where necessary

ifndef __SURFACES_INCLUDED__
__SURFACES_INCLUDED__   EQU     1


VDD_IOCTL_NV_FILLTABLE     EQU     20000667H
VDD_IOCTL_NV_GETSETCALLBACK	EQU     20000668H		
VDD_IOCTL_NV_SETVIDTEXSURF	EQU     20000669H		
 		

_NVVDDPROCTABLE STRUC
dwPDDDriver					DWORD   <>
getStoragePointer   		DD      ?
handleBufferInterrupts	DD      ?

_NVVDDPROCTABLE ENDS
LPNVVDDPROCTABLE STRUC  
        DD      ?
LPNVVDDPROCTABLE ENDS


endif