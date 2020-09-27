//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NV_DIAG.C                                                         *
*         Support module for NV Diagnostic Utility.                         *
*****************************************************************************
*                                                                           *
* History:  Jim Vito (jvito) 3/2/98 - Created.                              *
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"        // put this first. we redefine macro SF_BIT in nvrm.h
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <fifo.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include "vpicd.h"
#include "vdd.h"
#include "vnvrmd.h"
#include "oswin.h"
#include "vmm2.h"       // more vmm services
#include "vwin32.h"

typedef struct _def_NvDumpStruct
{
    V032 nvdump_nvAddr;
    V032 nvdump_romAddr;
    V032 nvdump_fbAddr;
    V032 nvdump_agpAddr;
    V032 nvdump_nvPhys;
    V032 nvdump_fbPhys;
    V032 nvdump_nvIRQ;  
    V032 nvdump_hDev; 
} NV_DUMP_STRUCT;

VOID NvDumpNvData(NV_DUMP_STRUCT *buffer)
{
//////  buffer->nvdump_nvAddr = (U032) nvAddr;
//////  buffer->nvdump_romAddr = (U032) romAddr;
//////  buffer->nvdump_fbAddr = (U032) fbAddr;
//////  buffer->nvdump_agpAddr = (U032) agpAddr;
//////  buffer->nvdump_nvPhys = (U032) nvPhys;
//////  buffer->nvdump_fbPhys = (U032) fbPhys;
//////  buffer->nvdump_nvIRQ = (U032) nvIRQ;  
//////  buffer->nvdump_hDev = (U032) hDev; 
}

VOID NvGetCrtcRegisters(BYTE *buffer)
{
//////  BYTE save_index;
//////  BYTE save_lock_value;
//////  BYTE i;
//////
//////
//////  // Save this register.
//////  _asm mov  dx,0x3c4  
//////  _asm in   al,dx
//////  _asm mov  save_index,al
//////
//////  // Save the lock value.
//////  _asm mov  al,6
//////  _asm out  dx,al
//////  _asm inc  dx
//////  _asm in   al,dx
//////  _asm dec  dx  
//////  _asm mov  save_lock_value,al  
//////
//////  // Unlock the extended registers.
//////  _asm mov  ax,0x5706
//////  _asm out  dx,ax
//////
//////  _asm mov  esi, buffer
//////
//////  for(i=0;i<0x40;i++)
//////  {
//////    _asm mov  al,i
//////    _asm out  dx,al
//////    _asm inc  dx
//////    _asm in   al,dx
//////    _asm dec  dx
//////    _asm mov  [esi], al
//////    _asm inc  esi
//////  }
//////
//////  // Restore the lock value.
//////  _asm mov  al,6
//////  _asm mov  ah,save_lock_value
//////  _asm out  dx,ax
//////
//////  // Restore the CRTC Index.
//////  _asm mov  al,save_index
//////  _asm out  dx,al
//////   
}

VOID NvSetCrtcRegisters(BYTE *buffer)
{
//////  BYTE save_index;
//////  BYTE save_lock_value;
//////  BYTE i;
//////
//////
//////  // Save this register.
//////  _asm mov  dx,0x3c4  
//////  _asm in   al,dx
//////  _asm mov  save_index,al
//////
//////  // Save the lock value.
//////  _asm mov  al,6
//////  _asm out  dx,al
//////  _asm inc  dx
//////  _asm in   al,dx
//////  _asm dec  dx  
//////  _asm mov  save_lock_value,al  
//////
//////  // Unlock the extended registers.
//////  _asm mov  ax,0x5706
//////  _asm out  dx,ax
//////
//////  _asm mov  esi, buffer
//////
//////  for(i=0;i<0x40;i++)
//////  {
//////    // Don't lock these registers
//////    if(i != 6){
//////      _asm mov  al,i
//////      _asm out  dx,al
//////      _asm inc  dx
//////      _asm mov  al,[esi]
//////      _asm out  dx,al
//////      _asm dec  dx
//////      _asm inc  esi
//////    }else
//////      _asm inc  esi
//////  }
//////
//////  // Restore the lock value.
//////  _asm mov  al,6
//////  _asm mov  ah,save_lock_value
//////  _asm out  dx,ax
//////
//////  // Restore the CRTC Index.
//////  _asm mov  al,save_index
//////  _asm out  dx,al
//////   
}

//////DWORD NvIoctlRead(DWORD offset)
//////{
//////
//////  BYTE  *temp1_ptr;
//////  DWORD *temp2_ptr;
//////
//////  temp1_ptr = (BYTE *) nvAddr; 
//////  temp1_ptr += offset;   
//////  temp2_ptr = (DWORD *) temp1_ptr;  
//////  return((DWORD) *temp2_ptr);
//////
//////}

VOID NvIoctlWrite(DWORD offset, DWORD data)
{
//////  BYTE  *temp1_ptr;
//////  DWORD *temp2_ptr;
//////
//////  temp1_ptr = (BYTE *) nvAddr; 
//////  temp1_ptr += offset;   
//////  temp2_ptr = (DWORD *) temp1_ptr;  
//////  *temp2_ptr = data;
}

DWORD OnW32DeviceIoControl(PDIOCPARAMETERS p)
{

//////    switch (p->dwIoControlCode)
//////    {
//////    case 0: 
//////      return 0;
//////    case NVRM_IOCTL_CRTC_GET:   
//////      NvGetCrtcRegisters((BYTE *) p->lpvInBuffer);
//////      return 0;
//////    case NVRM_IOCTL_CRTC_SET:   
//////      NvSetCrtcRegisters((BYTE *) p->lpvInBuffer);
//////      return 0;
//////    case NVRM_IOCTL_DUMPNVDATA:
//////      NvDumpNvData((NV_DUMP_STRUCT *) p->lpvInBuffer);
//////      return 0;
//////    case NVRM_IOCTL_NV_READ:
//////      {
//////        DWORD *tptr;
//////        tptr = (DWORD *) p->cbOutBuffer; 
//////        *tptr = NvIoctlRead(p->cbInBuffer);
//////      }
//////      return 0;
//////    case NVRM_IOCTL_NV_WRITE:
//////      NvIoctlWrite(p->cbInBuffer,p->cbOutBuffer);
//////      return 0;
//////      
//////    case NVRM_IOCTL_CRTC_TEST:
//////      dacEnableCursor(NvDBPtr_Table[0]);
//////    return 0;
//////
//////    default:
//////      return -1;
//////    }

    return (0);
}

