
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
// (C) Copyright SGS-THOMSON Microelectronics Inc., 1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NV3Watch.C                                                        *
*   Winice .N Debug Interface.                                              *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Vito 9/23/98 - wrote it.                                        *
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"        // put this first. we redefine macro SF_BIT in nvrm.h
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <nvromdat.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <fifo.h>
#include <gr.h>
#include <dac.h>
#include <heap.h>
#include <os.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include "vpicd.h"
#include "vdd.h"
#include "vnvrmd.h"
#include "oswin.h"
#include "vmm2.h"       // more vmm services
#include "vwin32.h"

//
// These defines are in the chip-dependent headers.  Rather than
// create a big mess by trying to #include them here, just pull what
// we need.
//

// hash table offset
#define NV_PFIFO_RAMHT                                   0x00002210 /* RW-4R */

#ifdef DEBUG


//
// prototypes
//
RM_STATUS btreeNodeValidate                 (PNODE);
VOID    NvWatch_MTRR_RegisterStuff          (VOID);
VOID    NvWatch_QuickDiag_fifo              (PHWINFO, PNVWATCHDIAGSTRUCT);
VOID    NvWatch_QuickDiag_master            (PHWINFO, PNVWATCHDIAGSTRUCT);
U032    NvWatch_NV04_GetHashSize            (PHWINFO);
VOID    NvWatch_NV04_Dump_Fifo              (PHWINFO);
VOID    NvWatch_NV04_Dump_Graphics          (PHWINFO);
VOID    NvWatch_NV04_Dump_Master            (PHWINFO);
VOID    NvWatch_NV04_Dump_Video             (PHWINFO);
VOID    NvWatch_NV04_Query_DeviceContext    (PHWINFO);
int     NvWatch_NV04_DmaPushAndFifo         (PHWINFO);
VOID    NvWatch_NV04_FillDiagStruct         (PHWINFO, PNVWATCHDIAGSTRUCT);
VOID    NvWatch_NV04_DumpGrTableContents    (PHWINFO, U032);
VOID    NvWatch_NV10_DumpGrTableContents    (PHWINFO, U032);
U032    NvWatch_NV10_GetHashSize            (PHWINFO);
VOID    NvWatch_NV10_Dump_Fifo              (PHWINFO);
VOID    NvWatch_NV10_Dump_Graphics          (PHWINFO);
VOID    NvWatch_NV10_Dump_Master            (PHWINFO);
VOID    NvWatch_NV10_Dump_Video             (PHWINFO);
VOID    NvWatch_NV10_Query_DeviceContext    (PHWINFO);
int     NvWatch_NV10_DmaPushAndFifo         (PHWINFO);


// Crush11 will have 512 64KB Mapping regions. 32MB Maximum.								  
// 16 4KB PTE's will define the physical memory	for each of the 64KB blocks.
#define NVWATCH_MAX_MEM_BLOCKS 512        
#define NUM_PAGES_FOR_A_64KB_BLOCK  0x10
#define NVWATCH_MAX_PAGES_PER_BLOCK NUM_PAGES_FOR_A_64KB_BLOCK

void *nvwatch_block_buffers[NVWATCH_MAX_MEM_BLOCKS]; /* up to 64K blocks*/
U032 nvwatch_PageArray[NVWATCH_MAX_MEM_BLOCKS][NVWATCH_MAX_PAGES_PER_BLOCK];
U032 nvwatch_crush11_failed_data;
U032 NvWatch_num_objects; 
//U032 NvWatch_channel;
char NvWatch_ObjectStrings[][33]={"NV01_NULL_OBJECT                ", /*    (0x00000000) */
                                  "NV01_CLASS                      ", /*    (0x00000001) */
                                  "NV01_CONTEXT_DMA_FROM_MEMORY    ", /*    (0x00000002) */
                                  "NV01_CONTEXT_DMA_TO_MEMORY      ", /*    (0x00000003) */
                                  "NV01_TIMER                      ", /*    (0x00000004) */
                                  "UNDEFINED_CLASS_Nv005           ", /*                 */
                                  "UNDEFINED_CLASS_Nv006           ", /*                 */
                                  "UNDEFINED_CLASS_Nv007           ", /*                 */
                                  "UNDEFINED_CLASS_Nv008           ", /*                 */
                                  "UNDEFINED_CLASS_Nv009           ", /*                 */
                                  "UNDEFINED_CLASS_Nv00a           ", /*                 */
                                  "UNDEFINED_CLASS_Nv00b           ", /*                 */
                                  "UNDEFINED_CLASS_Nv00c           ", /*                 */
                                  "UNDEFINED_CLASS_Nv00d           ", /*                 */
                                  "UNDEFINED_CLASS_Nv00e           ", /*                 */
                                  "UNDEFINED_CLASS_Nv00f           ", /*                 */
                                  "UNDEFINED_CLASS_Nv010           ", /*                 */
                                  "UNDEFINED_CLASS_Nv011           ", /*                 */
                                  "NV01_BETA_SOLID                 ", /*    (0x00000012) */
                                  "UNDEFINED_CLASS_Nv013           ", /*                 */
                                  "UNDEFINED_CLASS_Nv014           ", /*                 */
                                  "UNDEFINED_CLASS_Nv015           ", /*                 */
                                  "UNDEFINED_CLASS_Nv016           ", /*                 */
                                  "NV01_IMAGE_SOLID                ", /*    (0x00000017) */
                                  "NV01_IMAGE_PATTERN              ", /*    (0x00000018) */
                                  "NV01_IMAGE_BLACK_RECTANGLE      ", /*    (0x00000019) */
                                  "UNDEFINED_CLASS_Nv01a           ", /*                 */
                                  "UNDEFINED_CLASS_Nv01b           ", /*                 */
                                  "NV01_RENDER_SOLID_LIN           ", /*    (0x0000001C) */
                                  "NV01_RENDER_SOLID_TRIANGLE      ", /*    (0x0000001D) */
                                  "NV01_RENDER_SOLID_RECTANGLE     ", /*    (0x0000001E) */
                                  "NV01_IMAGE_BLIT                 ", /*    (0x0000001F) */
                                  "UNDEFINED_CLASS_Nv020           ", /*                 */
                                  "NV01_IMAGE_FROM_CPU             ", /*    (0x00000021) */
                                  "UNDEFINED_CLASS_Nv022           ", /*                 */
                                  "UNDEFINED_CLASS_Nv023           ", /*                 */
                                  "UNDEFINED_CLASS_Nv024           ", /*                 */
                                  "UNDEFINED_CLASS_Nv025           ", /*                 */
                                  "UNDEFINED_CLASS_Nv026           ", /*                 */
                                  "UNDEFINED_CLASS_Nv027           ", /*                 */
                                  "UNDEFINED_CLASS_Nv028           ", /*                 */
                                  "UNDEFINED_CLASS_Nv029           ", /*                 */
                                  "UNDEFINED_CLASS_Nv02a           ", /*                 */
                                  "UNDEFINED_CLASS_Nv02b           ", /*                 */
                                  "UNDEFINED_CLASS_Nv02c           ", /*                 */
                                  "UNDEFINED_CLASS_Nv02d           ", /*                 */
                                  "UNDEFINED_CLASS_Nv02e           ", /*                 */
                                  "UNDEFINED_CLASS_Nv02f           ", /*                 */
                                  "NV01_NULL                       ", /*    (0x00000030) */
                                  "UNDEFINED_CLASS_Nv031           ", /*                 */
                                  "UNDEFINED_CLASS_Nv032           ", /*                 */
                                  "UNDEFINED_CLASS_Nv033           ", /*                 */
                                  "UNDEFINED_CLASS_Nv034           ", /*                 */
                                  "UNDEFINED_CLASS_Nv035           ", /*                 */
                                  "NV03_STRETCHED_IMAGE_FROM_CPU   ", /*    (0x00000036) */
                                  "NV03_SCALED_IMAGE_FROM_MEMORY   ", /*    (0x00000037) */
                                  "NV04_DVD_SUBPICTURE             ", /*    (0x00000038) */
                                  "NV03_MEMORY_TO_MEMORY_FORMAT    ", /*    (0x00000039) */
                                  "UNDEFINED_CLASS_Nv03a           ", /*                 */
                                  "UNDEFINED_CLASS_Nv03b           ", /*                 */
                                  "UNDEFINED_CLASS_Nv03c           ", /*                 */
                                  "NV01_CONTEXT_DMA_IN_MEMORY      ", /*    (0x0000003D) */
                                  "NV01_CONTEXT_ERROR_TO_MEMORY    ", /*    (0x0000003E) */
                                  "NV01_MEMORY_PRIVILEGED          ", /*    (0x0000003F) */
                                  "NV01_MEMORY_USER                ", /*    (0x00000040) */
                                  "UNDEFINED_CLASS_Nv041           ", /*                 */
                                  "NV04_CONTEXT_SURFACES_2D        ", /*    (0x00000042) */
                                  "NV03_CONTEXT_ROP                ", /*    (0x00000043) */
                                  "NV04_CONTEXT_PATTERN            ", /*    (0x00000044) */
                                  "UNDEFINED_CLASS_Nv045           ", /*                 */
                                  "NV04_VIDEO_LUT_CURSOR_DAC       ", /*    (0x00000046) */
                                  "UNDEFINED_CLASS_Nv047           ", /*                 */
                                  "NV03_DX3_TEXTURED_TRIANGLE      ", /*    (0x00000048) */
                                  "UNDEFINED_CLASS_Nv049           ", /*                 */
                                  "NV04_GDI_RECTANGLE_TEXT         ", /*    (0x0000004A) */
                                  "NV03_GDI_RECTANGLE_TEXT         ", /*    (0x0000004B) */
                                  "UNDEFINED_CLASS_Nv04c           ", /*                 */
                                  "NV03_EXTERNAL_VIDEO_DECODER     ", /*    (0x0000004D) */
                                  "NV03_EXTERNAL_VIDEO_DECOMPRESSOR", /*    (0x0000004E) */
                                  "NV01_EXTERNAL_PARALLEL_BUS      ", /*    (0x0000004F) */
                                  "NV03_EXTERNAL_MONITOR_BUS       ", /*    (0x00000050) */
                                  "NV03_EXTERNAL_SERIAL_BUS        ", /*    (0x00000051) */
                                  "NV04_CONTEXT_SURFACE_SWIZZLED   ", /*    (0x00000052) */
                                  "NV04_CONTEXT_SURFACES_3D        ", /*    (0x00000053) */
                                  "NV04_DX5_TEXTURED_TRIANGLE      ", /*    (0x00000054) */
                                  "NV04_DX6_MULTI_TEXTURE_TRIANGLE ", /*    (0x00000055) */
                                  "UNDEFINED_CLASS_Nv056           ", /*                 */
                                  "NV04_CONTEXT_COLOR_KEY          ", /*    (0x00000057) */
                                  "NV03_CONTEXT_SURFACE_0          ", /*    (0x00000058) */
                                  "NV03_CONTEXT_SURFACE_1          ", /*    (0x00000059) */
                                  "NV03_CONTEXT_SURFACE_2          ", /*    (0x0000005A) */
                                  "NV03_CONTEXT_SURFACE_3          ", /*    (0x0000005B) */
                                  "NV04_RENDER_SOLID_LIN           ", /*    (0x0000005C) */
                                  "NV04_RENDER_SOLID_TRIANGLE      ", /*    (0x0000005D) */
                                  "NV04_RENDER_SOLID_RECTANGLE     ", /*    (0x0000005E) */
                                  "NV04_IMAGE_BLIT                 ", /*    (0x0000005F) */
                                  "NV04_INDEXED_IMAGE_FROM_CPU     ", /*    (0x00000060) */
                                  "NV04_IMAGE_FROM_CPU             ", /*    (0x00000061) */
                                  "NV010_CONTEXT_SURFACES_2D       ", /*                 */
                                  "NV05_SCALED_IMAGE_FROM_MEMORY   ", /*    (0x00000063) */
                                  "NV05_INDEXED_IMAGE_FROM_CPU     ", /*    (0x00000064) */
                                  "NV05_IMAGE_FROM_CPU             ", /*    (0x00000065) */
                                  "NV05_STRETCHED_IMAGE_FROM_CPU   ", /*    (0x00000066) */
                                  "NV10_VIDEO_LUT_CURSOR_DAC       ", /*                 */
                                  "UNDEFINED_CLASS_Nv068           ", /*                 */
                                  "UNDEFINED_CLASS_Nv069           ", /*                 */
                                  "NV03_CHANNEL_PIO                ", /*    (0x0000006A) */
                                  "NV03_CHANNEL_DMA                ", /*    (0x0000006B) */
                                  "NV04_CHANNEL_DMA                ", /*    (0x0000006C) */
                                  "UNDEFINED_CLASS_Nv06d           ", /*                 */
                                  "NV10_CHANNEL_DMA                ", /*                 */
                                  "UNDEFINED_CLASS_Nv06f           ", /*                 */
                                  "UNDEFINED_CLASS_Nv070           ", /*                 */
                                  "UNDEFINED_CLASS_Nv071           ", /*                 */
                                  "NV04_CONTEXT_BETA               ", /*    (0x00000072) */
                                  "UNDEFINED_CLASS_Nv073           ", /*                 */
                                  "UNDEFINED_CLASS_Nv074           ", /*                 */
                                  "UNDEFINED_CLASS_Nv075           ", /*                 */
                                  "NV04_STRETCHED_IMAGE_FROM_CPU   ", /*    (0x00000076) */
                                  "NV04_SCALED_IMAGE_FROM_MEMORY   ", /*    (0x00000077) */
                                  "UNDEFINED_CLASS_Nv078           ", /*                 */
                                  "UNDEFINED_CLASS_Nv079           ", /*                 */
                                  "NV10_VIDEO_OVERLAY              ", /*                 */
                                  "NV10_TEXTURE_FROM_CPU           ", /*                 */
                                  "UNDEFINED_CLASS_Nv07c           ", /*                 */
                                  "UNDEFINED_CLASS_Nv07d           ", /*                 */
                                  "UNDEFINED_CLASS_Nv07e           ", /*                 */
                                  "UNDEFINED_CLASS_Nv07f           ", /*                 */
                                  "NV01_DEVICE_0                   ", /*    (0x00000080) */
                                  "NV01_DEVICE_1                   ", /*    (0x00000081) */
                                  "NV01_DEVICE_2                   ", /*    (0x00000082) */
                                  "NV01_DEVICE_3                   ", /*    (0x00000083) */
                                  "NV01_DEVICE_4                   ", /*    (0x00000084) */
                                  "NV01_DEVICE_5                   ", /*    (0x00000085) */
                                  "NV01_DEVICE_6                   ", /*    (0x00000086) */
                                  "NV01_DEVICE_7                   ", /*    (0x00000087) */
                                  "NV10_DVD_SUBPICTURE             ", /*    (0x00000088) */
                                  "NV10_SCALED_IMAGE_FROM_MEMORY   ", /*    (0x00000089) */
                                  "UNDEFINED_CLASS_Nv08a           ", /*                 */
                                  "UNDEFINED_CLASS_Nv08b           ", /*                 */
                                  "UNDEFINED_CLASS_Nv08c           ", /*                 */
                                  "UNDEFINED_CLASS_Nv08d           ", /*                 */
                                  "UNDEFINED_CLASS_Nv08e           ", /*                 */
                                  "UNDEFINED_CLASS_Nv08f           ", /*                 */
                                  "UNDEFINED_CLASS_Nv090           ", /*                 */
                                  "UNDEFINED_CLASS_Nv091           ", /*                 */
                                  "UNDEFINED_CLASS_Nv092           ", /*                 */
                                  "NV10_CONTEXT_SURFACES_3D        ", /*    (0x00000093) */
                                  "NV10_DX5_TEXTURED_TRIANGLE      ", /*    (0x00000094) */
                                  "NV10_DX6_MULTI_TEXTURE_TRIANGLE ", /*    (0x00000095) */
                                  "ERROR: CLASS # TOO LARGE !!!    "}; /*    (>95) */

#define MAX_CLASS_NUMBER_SUPPORTED 0x95      
// This would be different for NV3.
#define INSTANCE_MEM_SPACE        nvAddr 

#define NVWATCH_SIMPLE 0
#define NVWATCH_COMPLEX 1
#define CurrentPatch    upatch.CurrentPatchNV10

// Internal dispatch defines.
#define NVWATCH_FUNC_GETHASHSIZE         0
#define NVWATCH_FUNC_DUMP_FIFO           1
#define NVWATCH_FUNC_DUMP_GRAPHICS       2
#define NVWATCH_FUNC_DUMP_MASTER         3
#define NVWATCH_FUNC_DUMP_VIDEO          4
#define NVWATCH_FUNC_QUERYDEVICECONTEXT  5
#define NVWATCH_FUNC_DMAPUSH_AND_FIFO    6

// JJV - Have this function echo the input.
char get_winice_input(void)
{
  char return_val;
  char echo_string[] = "x";

  _asm  mov  ax,1
  _asm  int  41h
  _asm  mov  return_val, al
  
  // Echo the keystrokes to the terminal.
  if (1){
    echo_string[0] = return_val;
    _asm  push ss
    _asm  pop  ds
    _asm  mov  ax, 2
    _asm  lea  esi, echo_string
    _asm  int  41h
  }
  
  return(return_val);
}

// Returns status.
DWORD NvWatch_ascii2hex(char *ascii_string, DWORD *ret_val)
{
  int i;
  BYTE temp_b;
  DWORD return_val=0;

  for(i=0;i<8;i++){
    temp_b = ascii_string[i];
    if( (temp_b < 0x30)||((temp_b > 0x39)&&(temp_b < 0x41))||((temp_b > 0x46)&&(temp_b < 0x61))|| \
        (temp_b > 0x66) ){
      //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Invalid register offset entered. (Must be upper case ! sorry)");
      return(0);
    }
    
    // Deal with the lower case 1st.
    if(temp_b >= 0x61)
      temp_b -= 0x20;
    
    if(temp_b > 0x39)
      temp_b -= 0x37;
    else
      temp_b -= 0x30;
      
    _asm  mov  eax, return_val
    _asm  or   al, temp_b
    _asm  ror  eax,4
    _asm  mov  return_val, eax
  }   
  
  *ret_val = return_val;
  return(1);   /* return success*/
}

DWORD NvWatch_GetInputDword(DWORD *dword_read)
{
  int i,j;
  char instring[]="00000000";
  char inchar;
  BYTE got_cr=FALSE;
  //DWORD inchar_dword;
  
  for(i=0;i<9;i++){
    // Get a character
    inchar = get_winice_input();
    
    if(inchar == '\r'){
      got_cr = TRUE;
    }else{
      // Get the character ... up to 8.
      if(i<8){
        // shift all the characters to the left.
        for(j=7;j>0;j--)
          instring[j] = instring[j-1];
        // put the character in the bottom
        instring[0] = inchar;
      }  
    }
    
    if(got_cr){
      if(i==0){
        //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Got a cr ... but no number entered");
        return(FALSE);
      }
      // Try to translate the thing.
      if(!NvWatch_ascii2hex(instring, dword_read)){
        //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Got a cr ... but can't decifer the value");
        return(FALSE);
      }else
        return(TRUE);
    }else{
      if(i==8){
        // exit with an error.
        //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Never got a  cr");
        return(FALSE);
      }  
    } 
  }  
}

void NvWatch_display_prompt(PHWINFO pDev, char *instring)
{
  U032 device_id_chip;
  U032 device;
  int i;

  for(i=0;i<MAX_INSTANCE;i++){
      if(pDev == NvDBPtr_Table[i]){
          device=i;
          break;
      }
  }

  device_id_chip = REG_RD32(NV_PBUS_PCI_NV_0);
  device_id_chip = (U032) ((device_id_chip >> DRF_SHIFT(NV_CONFIG_PCI_NV_0_DEVICE_ID)) &
      DRF_MASK(NV_CONFIG_PCI_NV_0_DEVICE_ID));

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, instring);
  
  switch(device_id_chip)
  {
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV04:
          switch(device)
          {
            case 0:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device0(NV04)>"); break;
            case 1:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device1(NV04)>"); break;
            case 2:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device2(NV04)>"); break;
            case 3:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device3(NV04)>"); break;
            case 4:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device4(NV04)>"); break;
            case 5:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device5(NV04)>"); break;
            case 6:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device6(NV04)>"); break;
            case 7:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device7(NV04)>"); break;
            case 8:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device8(NV04)>"); break;
          }
          break;
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID0:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID1:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID2:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID3:
          switch(device)
          {
            case 0:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device0(NV05)>"); break;
            case 1:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device1(NV05)>"); break;
            case 2:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device2(NV05)>"); break;
            case 3:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device3(NV05)>"); break;
            case 4:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device4(NV05)>"); break;
            case 5:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device5(NV05)>"); break;
            case 6:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device6(NV05)>"); break;
            case 7:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device7(NV05)>"); break;
            case 8:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device8(NV05)>"); break;
          }
          break;

      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID0:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID1:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID2:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID3:
          switch(device)
          {
            case 0:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device0(NV0A)>"); break;
            case 1:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device1(NV0A)>"); break;
            case 2:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device2(NV0A)>"); break;
            case 3:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device3(NV0A)>"); break;
            case 4:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device4(NV0A)>"); break;
            case 5:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device5(NV0A)>"); break;
            case 6:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device6(NV0A)>"); break;
            case 7:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device7(NV0A)>"); break;
            case 8:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device8(NV0A)>"); break;
          }
          break;

      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID0:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID1:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID2:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID3:
          switch(device)
          {
            case 0:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device0(NV10)>"); break;
            case 1:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device1(NV10)>"); break;
            case 2:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device2(NV10)>"); break;
            case 3:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device3(NV10)>"); break;
            case 4:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device4(NV10)>"); break;
            case 5:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device5(NV10)>"); break;
            case 6:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device6(NV10)>"); break;
            case 7:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device7(NV10)>"); break;
            case 8:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device8(NV10)>"); break;
          }
          break;

      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID0:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID1:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID2:
      case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID3:
          switch(device)
          {
            case 0:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device0(NV15)>"); break;
            case 1:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device1(NV15)>"); break;
            case 2:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device2(NV15)>"); break;
            case 3:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device3(NV15)>"); break;
            case 4:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device4(NV15)>"); break;
            case 5:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device5(NV15)>"); break;
            case 6:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device6(NV15)>"); break;
            case 7:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device7(NV15)>"); break;
            case 8:                                             
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device8(NV15)>"); break;
          }                                                     
          break;

      default:
          switch(device)
          {
            case 0:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device0(Disabled or Unknown)>"); break;
            case 1:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device1(Disabled or Unknown)>"); break;
            case 2:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device2(Disabled or Unknown)>"); break;
            case 3:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device3(Disabled or Unknown)>"); break;
            case 4:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device4(Disabled or Unknown)>"); break;
            case 5:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device5(Disabled or Unknown)>"); break;
            case 6:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device6(Disabled or Unknown)>"); break;
            case 7:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device7(Disabled or Unknown)>"); break;
            case 8:
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Device8(Disabled or Unknown)>"); break;
          }
          break;
  }
}

//****INSTANCE MEMORY INFO MENU******************************************************************************

U032 NvWatch_GetObjectFromInst(PHWINFO pDev, U032 inst, U032 *DHandle)
{
  U032 object_count, temp_d;
  U032 *hash_table_ptr;
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    if((U032) *(hash_table_ptr+1) != 0){
      temp_d = (U032) *(hash_table_ptr+1);
      temp_d &= 0xFFFF;
      if (temp_d == inst){
        *DHandle = (U032) *(hash_table_ptr);
        return(1);
      }
    }    
    hash_table_ptr+=2;
  } 
  // Error ... Not Found.
  return(0); 
}

U032 NvWatch_Process_Hash_Entry(PHWINFO pDev, U032 *hash_table_ptr, U032 verbose)
{

  U032 temp_var,t_channel;
  U032 error_found=FALSE;
  U032 *inst_ptr;
  U032 is_software=FALSE;
  OBJECT t_object,*pt_object;

  pt_object = &t_object;

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nHandle=");
  DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) *hash_table_ptr);
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Hash entry data=");
  DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) *(hash_table_ptr+1));
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Hash entry offset=");
  DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) hash_table_ptr);
  
  // Print the engine.
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nEngine=");
  temp_var = (U032) *(hash_table_ptr+1);
  temp_var >>= 16;
  temp_var &= 3;
  switch(temp_var){
    case 0:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Software"); is_software=TRUE; break;
    case 1:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Graphics"); break;
    case 2:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DVD     "); break;
    default:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR!! "); error_found = TRUE; break;
  }  
  
  // Print the channel ID.
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Channel ID=");
  temp_var = (U032) *(hash_table_ptr+1);
  temp_var >>= 24;
  temp_var &= 0xf;
  DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) temp_var);
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Status=");
  temp_var = (U032) *(hash_table_ptr+1);
  temp_var >>= 31;
  if(temp_var)
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "VALID");
  else
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "INVALID");
    
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Instance offset=");
  temp_var = (U032) *(hash_table_ptr+1);
  temp_var &= 0xFFFF;
  temp_var <<=4;
  temp_var += (U032) INSTANCE_MEM_SPACE/*nvAddr*/ + (U032) pDev->Pram.HalInfo.PraminOffset;
  DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) temp_var);
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
  
  if(verbose){
    // Parse the Instance Memory also. (temp_var has the offset to the Instance)
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Instance Dump: DATA=");
    inst_ptr = (U032 *) temp_var;
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) *inst_ptr);
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ");
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) *(inst_ptr+1));
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ");
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) *(inst_ptr+2));
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ");
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) *(inst_ptr+3));
    temp_var = (U032) *inst_ptr;
    temp_var &= 0xFFF;
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nClass=");

    // Special case software objects from DMA_CONTEXT objects.
    if( (is_software)&&(temp_var != NV01_CONTEXT_DMA_TO_MEMORY)&& \
        (temp_var != NV01_CONTEXT_DMA_FROM_MEMORY)&&(temp_var != NV01_CONTEXT_DMA_IN_MEMORY) ){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Software Object Class=");
        // Search the fifo database for this guy. (Since instance memory tells us nothing)
        for(t_channel=0;t_channel<pDev->Fifo.HalInfo.Count;t_channel++){
            // Do not call fifoSearchObject on an channel that has not been initialized.
            if(((U032)pDev->DBfifoTable[t_channel].ObjectTree) == 0x69696969){
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"ERROR: NOT FOUND ??");
                break;
            }
            if(fifoSearchObject((PHWINFO) pDev,(U032) *hash_table_ptr, (U032) t_channel, (POBJECT *) &pt_object)==RM_OK){
                temp_var =(U032) pt_object->Class->Type;
                if(temp_var>MAX_CLASS_NUMBER_SUPPORTED)
                  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,NvWatch_ObjectStrings[MAX_CLASS_NUMBER_SUPPORTED+1]);
                else  
                  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,NvWatch_ObjectStrings[temp_var]);
                break;
            }
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"ERROR: NOT FOUND ??");
        }
    }
    else{   
      if(temp_var>MAX_CLASS_NUMBER_SUPPORTED)
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,NvWatch_ObjectStrings[MAX_CLASS_NUMBER_SUPPORTED+1]);
      else  
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,NvWatch_ObjectStrings[temp_var]);
    }
      
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
    
    switch(temp_var){
      case NV01_CONTEXT_DMA_TO_MEMORY:
      case NV01_CONTEXT_DMA_FROM_MEMORY:
      case NV01_CONTEXT_DMA_IN_MEMORY: 
        if(((U032) *inst_ptr) & 0x1000)
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DMA_PAGE_TABLE_PRESENT");
        else  
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DMA_PAGE_TABLE_NOT_PRESENT");
        if(((U032) *inst_ptr) & 0x2000)
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " DMA_PAGE_ENTRY_LINEAR");
        else  
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " DMA_PAGE_ENTRY_NOT_LINEAR");
        temp_var = (U032) *inst_ptr;
        temp_var >>= 16;
        temp_var &= 3;
        switch(temp_var){
          case 0: DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " TARGET_NODE_NVM"); break;
          case 2: DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " TARGET_NODE_PCI"); break;
          case 3: DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " TARGET_NODE_AGP"); break;
          default:
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ERROR!"); error_found=TRUE; break;
        }
        temp_var = (U032) *inst_ptr;
        temp_var >>= 20;
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nAdjust=");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) temp_var);
        
        temp_var = (U032) *(inst_ptr+1);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Limit=");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) temp_var);
        
        if(((U032) *(inst_ptr+2)) & 2)
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " RW ");
        else
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " R only ");
          
        temp_var = (U032) *(inst_ptr+2);
        temp_var >>= 12;
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " Frame_Addr=");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) temp_var);
        
        break;
      default:
        {
          U032 save_offset, DHandle; 
          
          if(!is_software){
            temp_var = (U032) *(inst_ptr+1);
            temp_var >>= 16;
            save_offset = temp_var;
            save_offset <<=4;
            save_offset += (U032) INSTANCE_MEM_SPACE/*nvAddr*/ + (U032) pDev->Pram.HalInfo.PraminOffset;
            
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NOTIFY CONTEXT Handle=");
            if(!temp_var)
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "INVALID(0x0000)");
            else{
              if(!NvWatch_GetObjectFromInst(pDev, temp_var, &DHandle))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Not found!!!");
              else{  
                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) DHandle);
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  NOTIFY INSTANCE offset=");
                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) save_offset);
              }  
            }  
            
            temp_var = (U032) *(inst_ptr+2);
            temp_var &= 0xFFFF;
            save_offset = temp_var;
            save_offset <<=4;
            save_offset += (U032) INSTANCE_MEM_SPACE/*nvAddr*/ + (U032) pDev->Pram.HalInfo.PraminOffset;
            
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nDMA_CONTEXT0 Handle=");
            if(!temp_var)
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "INVALID(0x0000)");
            else{
              if(!NvWatch_GetObjectFromInst(pDev, temp_var, &DHandle))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Not found!!!");
              else{  
                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) DHandle);
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  DMA_CONTEXT0 INSTANCE offset=");
                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) save_offset);
              }  
            }  
          
            temp_var = (U032) *(inst_ptr+2);
            temp_var >>= 16;
            save_offset = temp_var;
            save_offset <<=4;
            save_offset += (U032) INSTANCE_MEM_SPACE/*nvAddr*/ + (U032) pDev->Pram.HalInfo.PraminOffset;
            
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nDMA_CONTEXT1 Handle=");
            if(!temp_var)
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "INVALID(0x0000)");
            else{
              if(!NvWatch_GetObjectFromInst(pDev, temp_var, &DHandle))
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Not found!!!");
              else{  
                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) DHandle);
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  DMA_CONTEXT1 INSTANCE offset=");
                DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (int) save_offset);
              }  
            }  
          }  
        }
        break;
    }    
  }
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
  return(!error_found);
}


void NvWatch_Dump_HT_Objects(PHWINFO pDev, U032 verbose)
{
 
  U032 object_count;
  //U032 hash_size, object_count;
  U032 object_counter=0; 
  U032 *hash_table_ptr;
  //U032 *hash_table_ptr, *inst_ptr;
  //U032 temp_var, error_found=FALSE,is_a_dmacontext;
  U032 error_found=FALSE;
  
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe MAX number of objects possible is = ", NvWatch_num_objects);
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    // Get the object entry. Look for a !NULL NV_RAMHT_HANDLE
    if((U032) *(hash_table_ptr+1) != 0){
      if(!NvWatch_Process_Hash_Entry(pDev, hash_table_ptr, verbose))
        error_found = TRUE;
      object_counter++;
    }               
    hash_table_ptr+=2;
  }  
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe total number of objects found is = ", object_counter);
  if(error_found)
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR !! ERROR !! BAD Data in the hash table");
  else 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "No errors were found parsing the hash table");
  
}

U032 NvWatch_Dump_HT_Object_From_Channel(PHWINFO pDev, U032 channelv)
{

  U032 object_count;
  U032 *hash_table_ptr;
  U032 channel_found=FALSE,error_found=FALSE;
  U032 temp_var;
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    if((U032) *(hash_table_ptr+1) != 0){
      temp_var = (U032) *(hash_table_ptr+1);
      temp_var >>= 24;
      temp_var &= 0xf;
      if(temp_var == channelv){
        channel_found = TRUE;
        if(!NvWatch_Process_Hash_Entry(pDev, hash_table_ptr, 1)){
          error_found = TRUE;
        }  
      }
    }    
    hash_table_ptr+=2;
  }  

  if(channel_found)
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Cool ... found an object");
  else 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Object with this Channel NOT found\n");
   
  if(error_found){
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR !! ERROR !! BAD Data in the hash table");
   return(0);
  }else{ 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " No errors were found parsing the hash table");
   if(channel_found)
     return(2);
   else  
     return(1);
  }   
}

U032 NvWatch_Dump_HT_Object_From_Engine(PHWINFO pDev, U032 engine)
{

  U032 object_count;
  U032 *hash_table_ptr;
  U032 engine_found=FALSE,error_found=FALSE;
  U032 temp_var;
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    if((U032) *(hash_table_ptr+1) != 0){
      temp_var = (U032) *(hash_table_ptr+1);
      temp_var >>= 16;
      temp_var &= 3;
      if(temp_var == engine){
        engine_found = TRUE;
        if(!NvWatch_Process_Hash_Entry(pDev, hash_table_ptr, 1)){
          error_found = TRUE;
        }  
      }
    }    
    hash_table_ptr+=2;
  }  

  if(engine_found)
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Cool ... found an object");
  else 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Object with this engine NOT found\n");
   
  if(error_found){
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR !! ERROR !! BAD Data in the hash table");
   return(0);
  }else{ 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " No errors were found parsing the hash table");
   if(engine_found)
     return(2);
   else  
     return(1);
  }   
}

U032 NvWatch_Dump_HT_Object_From_Class(PHWINFO pDev, U032 classv)
{

  U032 object_count;
  U032 *hash_table_ptr;
  U032 class_found=FALSE,error_found=FALSE;
  U032 temp_var, *inst_ptr;
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    if((U032) *(hash_table_ptr+1) != 0){
      temp_var = (U032) *(hash_table_ptr+1);
      temp_var &= 0xFFFF;
      temp_var <<=4;
      temp_var += (U032) INSTANCE_MEM_SPACE/*nvAddr*/ + (U032) pDev->Pram.HalInfo.PraminOffset;
      inst_ptr = (U032 *) temp_var;
      temp_var = (U032) *inst_ptr;
      temp_var &= 0xFFF;
      if(temp_var == classv){
        class_found = TRUE;
        if(!NvWatch_Process_Hash_Entry(pDev, hash_table_ptr, 1)){
          error_found = TRUE;
        }  
      }
    }    
    hash_table_ptr+=2;
  }  

  if(class_found)
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Cool ... found an object");
  else 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Object with this class NOT found\n");
   
  if(error_found){
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR !! ERROR !! BAD Data in the hash table");
   return(0);
  }else{ 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " No errors were found parsing the hash table");
   if(class_found)
     return(2);
   else  
     return(1);
  }   
}

// Return 0=Error in hash, 1=Object Not found, 2=Success
U032 NvWatch_Dump_HT_Object_From_Handle(PHWINFO pDev, U032 handle)
{

  U032 object_count;
  U032 *hash_table_ptr;
  U032 handle_found=FALSE,error_found=FALSE;
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    if((U032) *(hash_table_ptr+1) != 0){
      if((U032) *(hash_table_ptr) == handle){
        handle_found = TRUE;
        if(!NvWatch_Process_Hash_Entry(pDev, hash_table_ptr, 1)){
          error_found = TRUE;
        }  
      }
    }    
    hash_table_ptr+=2;
  }  

  if(handle_found)
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Cool ... found an object");
  else 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Object with this handle NOT found\n");
   
  if(error_found){
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR !! ERROR !! BAD Data in the hash table");
   return(0);
  }else{ 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " No errors were found parsing the hash table");
   if(handle_found)
     return(2);
   else  
     return(1);
  }   
}


U032 NvWatch_Dump_HT_Object_From_Instance(PHWINFO pDev, U032 inst)
{

  U032 object_count, temp_inst;
  U032 *hash_table_ptr;
  U032 inst_found=FALSE,error_found=FALSE;
  
  // Start from the beginning of Instance Memory
  hash_table_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset + 0x10000)/4));
  // traverse the HT searching for objects.
  for(object_count=0;object_count<NvWatch_num_objects;object_count++){
    if((U032) *(hash_table_ptr+1) != 0){
      temp_inst = (U032) *(hash_table_ptr+1);
      temp_inst &= 0xFFFF;
      if(temp_inst == inst){
        inst_found = TRUE;
        if(!NvWatch_Process_Hash_Entry(pDev, hash_table_ptr, 1)){
          error_found = TRUE;
        }  
      }
    }    
    hash_table_ptr+=2;
  }  

  if(inst_found)
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Cool ... found an object");
  else 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Object at this INSTANCE NOT found\n");
   
  if(error_found){
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR !! ERROR !! BAD Data in the hash table");
   return(0);
  }else{ 
   DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " No errors were found parsing the hash table");
   if(inst_found)
     return(2);
   else  
     return(1);
  }   
}

void NvWatch_Dump_InstanceFifoContext(PHWINFO pDev)
{

    U032 temp_data,channel_counter;
    U032 *ptr_fifocontextarea;

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n*********************************************");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nDumping fifo contexts from instance memory\n");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,   "*********************************************\n");

    // Print the Hash Table size and the Instance Memory size.
    temp_data = REG_RD32(NV_PFIFO_RAMHT);
    temp_data >>= 16;
    temp_data &= 0x3;
    switch(temp_data){
      case 0:
        ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x1000) /4) );
        break;
      case 1:
        ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x2000) /4) );
        break;
      case 2:
        ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x4000) /4) );
        break;
      case 3:
        ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x8000) /4) );
        break;
    }

    for(channel_counter=0;channel_counter<NUM_FIFOS;channel_counter++){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Channel ", (int) channel_counter);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  DMA PUT      = ", (int) *(ptr_fifocontextarea+0+(channel_counter*8)));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  DMA GET      = ", (int) *(ptr_fifocontextarea+1+(channel_counter*8)));

        if(IsNV10orBetter(pDev)){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Ref Count    = ", (int) *(ptr_fifocontextarea+2+(channel_counter*8)));
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  DMA Instance = ", (int) *(ptr_fifocontextarea+3+(channel_counter*8)));
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  DMA Instance = ", (int) *(ptr_fifocontextarea+2+(channel_counter*8)));
        }
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
    }
}

void NvWatch_InstanceMem_help(PHWINFO pDev)
{

  U032 temp_data;
  U032 device;
  int i;

  for(i=0;i<MAX_INSTANCE;i++){
      if(pDev == NvDBPtr_Table[i]){
          device=i;
          break;
      }
  }
  
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nInstance Memory options for device ", device);
  
  // Print the Hash Table size and the Instance Memory size.
  temp_data = REG_RD32(NV_PFIFO_RAMHT);
  temp_data >>= 16;
  temp_data &= 0x3;
  switch(temp_data){
    case 0:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Hash table size is 4K\n\r");
      break;
    case 1:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Hash table size is 8K\n\r");
      break;
    case 2:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Hash table size is 16K\n\r");
      break;
    case 3:
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Hash table size is 32K\n\r");
      break;
  } 
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCommands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      h   - Dump objects from the Hash table (simple ... just the hash entries)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      v   - Dump objects (Verbose ... Hash entries and Instance data)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      o   - Dump Objects by Handle\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      c   - Dump Objects by Class \n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      e   - Dump Objects by Engine\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      n   - Dump Objects by Channel\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      i   - Dump Object by Instance (eg. XXXX)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      f   - Fifo Context Area\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit this\n\r\n\r");
}

VOID NvWatch_Query_InstanceMemory(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  char pstring[]="NvWatch InstanceMem";
  
  NvWatch_InstanceMem_help(pDev);
  
  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
  
    in_char = get_winice_input();
    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE;
        break;
      case 'f': // Dump hash table
      case 'F': // Dump hash table
        NvWatch_Dump_InstanceFifoContext(pDev);
        break;
      case 'h': // Dump hash table
      case 'H': // Dump hash table
        NvWatch_Dump_HT_Objects(pDev,0);
        break;
      case 'v': // Dump hash table
      case 'V': // Dump hash table
        NvWatch_Dump_HT_Objects(pDev,1);
        break;
      case 'o': // Object dump by handle
      case 'O': // Object dump by handle
        {
          U032 o_handle;
          
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nEnter the handle of the object->");
          NvWatch_GetInputDword(&o_handle);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
          NvWatch_Dump_HT_Object_From_Handle(pDev, o_handle);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
        }  
        break;
      case 'c': // Object dump by class
      case 'C': // Object dump by class
        {
          U032 o_class;
          
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nEnter the class of the object->");
          NvWatch_GetInputDword(&o_class);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
          NvWatch_Dump_HT_Object_From_Class(pDev, o_class);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
        }  
        break;
      case 'e': // Object dump by engine
      case 'E': // Object dump by engine
        {
          U032 o_engine;
          
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nEngine types: Software=0 Graphics=1 DVD=2");
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nEnter the engine type of the object->");
          NvWatch_GetInputDword(&o_engine);
          if((o_engine<0)||(o_engine>2)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR: Invalid Engine # entered !");
            break;
          }  
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
          NvWatch_Dump_HT_Object_From_Engine(pDev, o_engine);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
        }  
        break;
      case 'n': // Object dump by channel
      case 'N': // Object dump by channel
        {
          U032 o_channel;
          
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nEnter the channel of the objects->");
          NvWatch_GetInputDword(&o_channel);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
          NvWatch_Dump_HT_Object_From_Channel(pDev, o_channel);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
        }  
        break;
      case 'i': // Object dump by instance
      case 'I': // Object dump by instance
        {
          U032 o_inst;
          
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nEnter the instance of the objects->");
          NvWatch_GetInputDword(&o_inst);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
          NvWatch_Dump_HT_Object_From_Instance(pDev, o_inst);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
        }  
        break;
      default: // Help
        NvWatch_InstanceMem_help(pDev);
        break;  
    }
  }
}

//****RM DATA MENU******************************************************************************

void NvWatch_spaceit(U032 level)
{
  U032 i;

  for (i = 0; i < level; i++)
  {
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  ");
  }
}

#define BTREE_OBJS_ONLY 0x2
#define MAX_CLASS_VALUE 0xFFF
RM_STATUS NvWatch_btreeDumpBranch
(
    PNODE Node,
    U032  Level,
    U032  dump_type // bit0=0=Simple bit0=1=Complex bit2=0=ALL bit2=1=Objects(no classes)
)
{
  if (Node)
  {
      //VALIDATE_NODE(Node);
      if (btreeNodeValidate(Node) != RM_OK){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: Btree Node Validation Failed!!");
        return (RM_ERROR);
      }  
      
      NvWatch_btreeDumpBranch(Node->RightBranch, Level + 1, 3);
      //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM_BTREE: ");
      //for (i = 0; i < Level; i++)
      //{
      //    //DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, ".");
      //    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  ");
      //}
      
      if(dump_type & BTREE_OBJS_ONLY){
        if(Node->Value > MAX_CLASS_VALUE){ 
          //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Node         = ", (U032)Node);
          NvWatch_spaceit(Level);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Node         = ");
          DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032)Node);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
          
          //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Value        = ", Node->Value);
          NvWatch_spaceit(Level);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Value        = ");
          DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, Node->Value);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
          
          //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Left count   = ", Node->LeftCount);
          NvWatch_spaceit(Level);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Left count   = ");
          DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, Node->LeftCount);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
          
          //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Right count  = ", Node->RightCount);
          NvWatch_spaceit(Level);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Right Count  = ");
          DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, Node->RightCount);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
          
          //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Left branch  = ", (U032)Node->LeftBranch);
          NvWatch_spaceit(Level);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Left Branch  = ");
          DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032)Node->LeftBranch);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
          
          //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Right branch = ", (U032)Node->RightBranch);
          NvWatch_spaceit(Level);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Right Branch = ");
          DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032)Node->RightBranch);
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
        }  
      }else{    
        //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Node         = ", (U032)Node);
        NvWatch_spaceit(Level);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Node         = ");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032)Node);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
        
        //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Value        = ", Node->Value);
        NvWatch_spaceit(Level);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Value        = ");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, Node->Value);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
        
        //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Left count   = ", Node->LeftCount);
        NvWatch_spaceit(Level);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Left count   = ");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, Node->LeftCount);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
        
        //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Right count  = ", Node->RightCount);
        NvWatch_spaceit(Level);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Right Count  = ");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, Node->RightCount);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
        
        //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Left branch  = ", (U032)Node->LeftBranch);
        NvWatch_spaceit(Level);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Left Branch  = ");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032)Node->LeftBranch);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
        
        //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Right branch = ", (U032)Node->RightBranch);
        NvWatch_spaceit(Level);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,            "Right Branch = ");
        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032)Node->RightBranch);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
      }    
      NvWatch_btreeDumpBranch(Node->LeftBranch, Level + 1, 3);
  }       
}           
            
void NvWatch_DumpBtreeContents(PHWINFO pDev)
{
  U032 i;
    
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nDumping the BTREE ! ");         

  for(i=0;i<NUM_FIFOS;i++){
  
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\n     Dumping FIFO # ", i);      
    if (pDev->DBfifoTable[i].InUse == FALSE)
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Fifo not InUse !\n\r");
    else{  
      if (pDev->DBfifoTable[i].ObjectTree == NULL)
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Fifo InUse but =NULL\n\r");
      else
        NvWatch_btreeDumpBranch(pDev->DBfifoTable[i].ObjectTree, 0, 3);
    } 
  }
} 

void NvWatch_Dump_NvInfo(PHWINFO pDev)
{
  PWIN9XHWINFO pOsHwInfo = (PWIN9XHWINFO) pDev->pOsHwInfo;

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nNvInfo State Variables:\r\n");

  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\r\npDev->Mapping.nvPhys        ",pDev->Mapping.nvPhys);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Mapping.fbPhys            ",pDev->Mapping.fbPhys);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Mapping.nvIRQ             ",pDev->Mapping.nvIRQ);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Mapping.nvBusDeviceFunc   ",pDev->Mapping.nvBusDeviceFunc);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Mapping.hpicIRQ           ",pDev->Mapping.hpicIRQ);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Mapping.hDev              ",pDev->Mapping.hDev);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Mapping.osDeviceHandle  ",pOsHwInfo->osDeviceHandle);
  
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Framebuffer.RamSizeMb   ",pDev->Framebuffer.HalInfo.RamSizeMb);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->Framebuffer.RamSize     ",pDev->Framebuffer.HalInfo.RamSize);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "nvAddr                          ",(int) nvAddr);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "romAddr                         ",(int) romAddr);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "fbAddr                          ",(int) fbAddr);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "pDev->DBagpAddr                 ",(int) pDev->DBagpAddr);
  
}

void NvWatch_DisplayClientInfo(PHWINFO pDev, U032 client_counter, U032 verbose)
{
  U032 device_counter, fifo_counter;
  PCLI_DMA_INFO dmalist_ptr;
  
  if(clientInfo[client_counter].InUse == TRUE){
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Client found at index = ",client_counter);
     DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Handle        = ",clientInfo[client_counter].Handle);
     if(verbose){
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Class         = ",clientInfo[client_counter].Class);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  InUse         = ",clientInfo[client_counter].InUse);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  AppID         = ",clientInfo[client_counter].AppID);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  ProcID        = ",clientInfo[client_counter].ProcID);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  ProcContext   = ",clientInfo[client_counter].ProcContext);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  FifoCount     = ",clientInfo[client_counter].FifoCount);
     }
     DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  CliDmaList    = ",(int) clientInfo[client_counter].CliDmaList);
     DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  CliDeviceList = ",(int) clientInfo[client_counter].CliDeviceList);
     DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  CliMemoryList = ",(int) clientInfo[client_counter].CliMemoryList);
     DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r");
    
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Traversing DMA Context List ...\n");
    dmalist_ptr = clientInfo[client_counter].CliDmaList;
    while(dmalist_ptr){
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  DMA Context Found\n");
     DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Handle       = ",dmalist_ptr->Handle);
     if(verbose){
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Client       = ",dmalist_ptr->Client);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Class        = ",dmalist_ptr->Class);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Flags        = ",dmalist_ptr->Flags);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Access       = ",dmalist_ptr->Access);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Physicality  = ",dmalist_ptr->Physicality);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Locked       = ",dmalist_ptr->Locked);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Coherency    = ",dmalist_ptr->Coherency);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Valid        = ",dmalist_ptr->Valid);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DescSelector = ",dmalist_ptr->DescSelector);  
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DescOffset   = ",(int) dmalist_ptr->DescOffset);      
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DescLimit    = ",dmalist_ptr->DescLimit);   
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DescAddr     = ",(int) dmalist_ptr->DescAddr);    
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    BufferBase   = ",(int) dmalist_ptr->BufferBase);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    BufferSize   = ",dmalist_ptr->BufferSize);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    LockHandle   = ",(int) dmalist_ptr->LockHandle);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    PteCount     = ",dmalist_ptr->PteCount);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    PteAdjust    = ",dmalist_ptr->PteAdjust);   
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    PteLimit     = ",dmalist_ptr->PteLimit);    
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    PteArray     = ",(int) dmalist_ptr->PteArray);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Next         = ",(int) dmalist_ptr->Next);
         DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Prev         = ",(int) dmalist_ptr->Prev);
     }
     DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    AddressSpace = ",dmalist_ptr->AddressSpace);
    dmalist_ptr = dmalist_ptr->Next;
    }
    
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nTraversing DeviceList ...\n");
    for(device_counter=0;device_counter<NUM_DEVICES;device_counter++){
      if(clientInfo[client_counter].CliDeviceList[device_counter].InUse == TRUE){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Device in use found = ",device_counter);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Handle        = ",clientInfo[client_counter].CliDeviceList[device_counter].Handle);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Client        = ",clientInfo[client_counter].CliDeviceList[device_counter].Client);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Class         = ",clientInfo[client_counter].CliDeviceList[device_counter].Class);
        if(verbose){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    InUse         = ",clientInfo[client_counter].CliDeviceList[device_counter].InUse);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    AppID         = ",clientInfo[client_counter].CliDeviceList[device_counter].AppID);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DevID         = ",clientInfo[client_counter].CliDeviceList[device_counter].DevID);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DevFifoList   = ",(int)clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList);  
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    DevMemoryList = ",(int)clientInfo[client_counter].CliDeviceList[device_counter].DevMemoryList);
        }
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n    Traversing Fifolist for this device ...\n");
        for(fifo_counter=0;fifo_counter<NUM_FIFOS;fifo_counter++){
          if(clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].InUse == TRUE){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "     Fifo in use found = ",fifo_counter);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         Handle           = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].Handle);           
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         Device           = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].Device);           
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         Class            = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].Class);            
            if(verbose){
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         ErrorDmaContext  = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].ErrorDmaContext); 
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         DataDmaContext   = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].DataDmaContext);   
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         FifoPtr          = ",(int) clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].FifoPtr);      
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         DevID            = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].DevID);            
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         InUse            = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].InUse);            
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         ChID             = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].ChID);             
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         AppID            = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].AppID);            
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         Selector         = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].Selector);         
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         Flat             = ",(int) clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].Flat);             
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         heventWait       = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].heventWait);       
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         hwndNotify       = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].hwndNotify);       
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         hwndError        = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].hwndError);        
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         msgNotify        = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].msgNotify);        
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         msgError         = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].msgError);         
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         lpSysCallback    = ",(int) clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].lpSysCallback);    
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         eventNotify      = ",clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].eventNotify);  
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "         pDmaObject       = ",(int) clientInfo[client_counter].CliDeviceList[device_counter].DevFifoList[fifo_counter].pDmaObject);       
            }
          }
        }  
      }  
    }
  }
}

void NvWatch_Dump_ClientInfo(PHWINFO pDev, U032 client_num, U032 verbose)
{
  int client_counter;
  

  if(client_num == 0xffffffff){
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r");
      for(client_counter=0;client_counter<NUM_CLIENTS;client_counter++){
        NvWatch_DisplayClientInfo(pDev, client_counter, verbose);
      }
  }else{
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r");
      NvWatch_DisplayClientInfo(pDev, client_num, verbose);
  }

}

void NvWatch_DumpGrTableContents(PHWINFO pDev, U032 verbose_flag)
{
  
  if(IsNV10orBetter(pDev)){
      NvWatch_NV10_DumpGrTableContents(pDev, verbose_flag);
  }else{
      NvWatch_NV04_DumpGrTableContents(pDev, verbose_flag);
  }
}

void NvWatch_DumpRegistryStrings(PHWINFO pDev)
{
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r\n\rGlobal Path = ");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, strRegistryGlobalParameters);
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Device Path = ");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, pDev->Registry.DBstrDevNodeDisplay);
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r\n\r");
}
void NvWatch_RM_help(void)
{
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Resource Manager Internal Data Structures\n\r\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Commands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      i   - Dump NvInfo data\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      c   - Dump Client data\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      d   - Dump Client data (VERBOSE)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      b   - Dump BTREE Contents\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      r   - Dump Registry Path Info\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      g   - Dump Graphics Contexts\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      v   - Dump Graphics Contexts (VERBOSE)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      m   - CPU MTRR Registers (use n option for R/W)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      n   - CPU MSR Registers R/W\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      o   - CPU CR0 R/W\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit\n\r\n\r");
}

#define CPUID   __asm _emit 0x0F __asm _emit 0xA2
#define RDMSR   __asm _emit 0x0F __asm _emit 0x32
#define WRMSR   __asm _emit 0x0F __asm _emit 0x30
VOID NvWatch_MTRR_RegisterStuff(void)
{
    U032 mtrrAddr, numRanges = 0;
    U032 BaseHi, BaseLo, MaskHi, MaskLo;
    U032 i;

    CLI
    __asm
    {
        ; save state
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd
        pop     eax
        mov     ebx, eax
        xor     eax, 0x00200000
        push    eax
        popfd
        pushfd
        pop     eax
        cmp     eax, ebx
        je      Exit                // CPUID inst not supported

        mov     eax, 1
        CPUID
        and     edx, 0x1000         // check for MTRR support
        jz      Exit

        mov     ecx, 254            // read the MTRRcap MSR to determine number of ranges.
        RDMSR
        test    eax, 0x400          // check that WC as a type is supported (should be 1).
        jz      Exit

        and     eax, 0xFF           // save number of ranges
        mov     numRanges, eax
Exit:    
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }

    // Now determine if we find a Write Combined match in the MTRRs
    for (i = 0, mtrrAddr = 512; i < numRanges; i++, mtrrAddr += 2)
    {
        __asm {

            push    eax
            push    ebx
            push    ecx
            push    edx

            mov     ecx, mtrrAddr
            RDMSR

            mov     BaseHi, edx
            mov     BaseLo, eax

            mov     ecx, mtrrAddr
            add     ecx, 1
            RDMSR

            mov     MaskHi, edx
            mov     MaskLo, eax

            pop     edx
            pop     ecx
            pop     ebx
            pop     eax
        }

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: MTRR ", i);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:      Base ", BaseLo);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM:      Mask ", MaskLo);

        /////if (!(MaskLo & 0x800))
        /////   continue;               // not a valid range
        /////MaskLo &= ~0xFFF;          // mask off the valid bit
        /////
        /////if ((BaseLo & 0xFF) != 0x1)
        /////   continue;               // not a Write Combined range
        /////BaseLo &= ~0xFF;           // mask off the type bits
        /////
        /////if ((BaseLo >= AGPBase) && ((BaseLo + ~MaskLo) < (AGPBase + AGPSize)))
        /////   pDev->Mapping.AGPLimit = ~MaskLo;  // the WC MTRR is within the aperture
    }

    STI
}

#define MAX_MSR_REGISTER 0x413

NvU64 NvWatch_MSRREAD(U032 reg_num)
{
    NvU64 return_val={0xFFFFFFFF,0xFFFFFFFF};

    if(reg_num > MAX_MSR_REGISTER){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR!: This offset exceeds the maximun MSR Register.");
        return return_val;
    }

    CLI
    __asm
    {
        ; save state
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd
        pop     eax
        mov     ebx, eax
        xor     eax, 0x00200000
        push    eax
        popfd
        pushfd
        pop     eax
        cmp     eax, ebx
        je      Exit                // CPUID inst not supported

        mov     eax, 1
        CPUID
        and     edx, 0x1000         // check for MTRR support
        jz      Exit

        mov     ecx, reg_num         // read the MTRRcap MSR to determine number of ranges.
        RDMSR
        mov     return_val.low,eax
        mov     return_val.high,edx

Exit:    
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    STI
    return return_val;
}

NvU64 NvWatch_MSRWRITE(U032 reg_num, NvU64 data)
{
    NvU64 return_val={0xFFFFFFFF,0xFFFFFFFF};

    if(reg_num > MAX_MSR_REGISTER){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR!: This offset exceeds the maximun MSR Register.");
        return return_val;
    }

    CLI
    __asm
    {
        ; save state
        push    eax
        push    ebx
        push    ecx
        push    edx

        pushfd
        pop     eax
        mov     ebx, eax
        xor     eax, 0x00200000
        push    eax
        popfd
        pushfd
        pop     eax
        cmp     eax, ebx
        je      Exit                // CPUID inst not supported

        mov     eax, 1
        CPUID
        and     edx, 0x1000         // check for MTRR support
        jz      Exit

        mov     ecx, reg_num         // read the MTRRcap MSR to determine number of ranges.
        mov     eax,data.low
        mov     edx,data.high
        WBINVD
        WRMSR

Exit:    
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
    }
    STI
    return return_val;
}

int NvWatch_MSR_Registers()
{
    char instring[]="00000000";
    DWORD ddata=0, ddata2=0, daddr=0, save_offset;
    NvU64 msr_data;

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nEnter the Offset of the MSR Register to query.");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \n->");

    if(!NvWatch_GetInputDword(&daddr)){
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
      return(0);
    }  
    save_offset = daddr; 

    msr_data = NvWatch_MSRREAD(daddr);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe low 32 bits read are ", (int) msr_data.low);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The high 32 bits read are ", (int) msr_data.high);

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nEnter the new value for the low 32 bits ->");
    
    if(!NvWatch_GetInputDword(&ddata)){
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
      return(0);
    }  
    
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nEnter the new value for the high 32 bits ->");
    
    if(!NvWatch_GetInputDword(&ddata2)){
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
      return(0);
    }  

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe low 32 bits to write are ", (int) ddata);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The high 32 bits to write are ", (int) ddata2);
    msr_data.low = ddata;
    msr_data.high = ddata2;
    NvWatch_MSRWRITE(daddr, msr_data);
}

int NvWatch_CR0_Control()
{
    U032 ddata=0,cr0_save;
    _asm{
        mov  eax,cr0
        mov  cr0_save,eax 
    }
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe value of CR0 is ", (int) cr0_save);
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nEnter a new value for CR0 ->");

    if(!NvWatch_GetInputDword(&ddata)){
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
      return(0);
    }  

    _asm{
        mov  eax,ddata
        WBINVD
        mov  cr0,eax 
    }
    _asm{
        mov  eax,cr0
        mov  cr0_save,eax 
    }
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe new value of CR0 is ", (int) cr0_save);
}

void NvWatch_heapDump
(
    PHEAP heap
)
{
    U032      i;
    U032      free;
    PMEMBLOCK block;

    if (!heap) return;
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\nHeap dump.  Size = ", heap->total);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,  "            Free = ", heap->free);
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"=================================================================\r\n");
    for (i = 0; i < heap->numBanks; i++)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"Bank: ", i);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tType flags = ", heap->Bank[i].flags);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tOffset     = ", heap->Bank[i].offset);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tSize       = ", heap->Bank[i].size);
    }
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Block List Forward:\r\n");
    block = heap->pBlockList;
    do
    {
        if (block->owner == FREE_BLOCK)
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\tOwner = FREE\r\n");
        else
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tOwner = ", block->owner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tType  = ", block->u0.type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tHwres = ", block->u1.hwres);
        }
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tBegin = ", block->begin);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tEnd   = ", block->end);
        block = block->next;
    } while (block != heap->pBlockList);
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Block List Reverse:\r\n");
    block = heap->pBlockList;
    do
    {
        block = block->prev;
        if (block->owner == FREE_BLOCK)
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\tOwner = FREE\r\n");
        else
        {
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tOwner = ", block->owner);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tType  = ", block->u0.type);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tHwres  = ", block->u1.hwres);
        }
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tBegin = ", block->begin);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tEnd   = ", block->end);
    } while (block != heap->pBlockList);
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"FREE Block List Forward:\r\n");
    free  = 0;
    if ((block = heap->pFreeBlockList))
        do
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\tOwner = FREE\r\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tBegin = ", block->begin);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tEnd   = ", block->end);
            free += block->end - block->begin + 1;
            block = block->u1.nextFree;
        } while (block != heap->pFreeBlockList);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tCalculated free count = ", free);
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"FREE Block List Reverse:\r\n");
    free  = 0;
    if ((block = heap->pFreeBlockList))
        do
        {
            block = block->u0.prevFree;
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\tOwner = FREE\r\n");
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tBegin = ", block->begin);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\t\tEnd   = ", block->end);
            free += block->end - block->begin + 1;
        } while (block != heap->pFreeBlockList);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"\tCalculated free count = ", free);

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n\nKey to Types");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_IMAGE       = 0x00000001");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_DEPTH       = 0x00000002");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_TEXTURE     = 0x00000004");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_OVERLAY     = 0x00000008");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_FONT        = 0x00000010");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_CURSOR      = 0x00000020");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_DMA         = 0x00000040");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_INSTANCE    = 0x00000080");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_PRIMARY     = 0x00000100");
	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\nMEM_FLAG_IMAGE_TILED = 0x00000200");
}

int NvWatch_RMData(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  char pstring[]="NvWatch Resource Manager Data Structure Info";
 
  NvWatch_RM_help();
  
  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
  
    in_char = get_winice_input();
    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE;
        break;
      case 'i': // NvInfo Data
      case 'I': // NvInfo Data
        NvWatch_Dump_NvInfo(pDev);
        break;
      case 'c': // Client Data       
      case 'C': // Client Data       
        NvWatch_Dump_ClientInfo(pDev,0xFFFFFFFF,0);
        break;
      case 'd': // Client Data       
      case 'D': // Client Data       
        NvWatch_Dump_ClientInfo(pDev,0xFFFFFFFF,1);
        break;
      case 'b': // Btree Dump
      case 'B': // Btree Dump
        NvWatch_DumpBtreeContents(pDev);
        break;
      case 'r': // Btree Dump
      case 'R': // Btree Dump
        NvWatch_DumpRegistryStrings(pDev);
        break;
      case 'g': // grTable Dump
      case 'G': // grTable Dump
        NvWatch_DumpGrTableContents(pDev, NVWATCH_SIMPLE);
        break;
      case 'v': // grTable Dump
      case 'V': // grTable Dump
        NvWatch_DumpGrTableContents(pDev, NVWATCH_COMPLEX);
        break;
	  case 'h': // heap Dump
	  case 'H': // heap Dump
		NvWatch_heapDump((PHEAP)pDev->Framebuffer.HeapHandle);
		break;
      case 'm': // MTRR Stuff
      case 'M': // 
        NvWatch_MTRR_RegisterStuff();
        break;
      case 'n': // MTRR Stuff
      case 'N': // 
        NvWatch_MSR_Registers();
        break;
      case 'o': // MTRR Stuff
      case 'O': // 
        NvWatch_CR0_Control();
        break;
      default: // Help
        NvWatch_RM_help();
        break;  
    }
  }

    return (RM_OK);
}

RM_STATUS NvWatch_osAllocMem
(
    VOID **pAddress,
    U032   Size
)
{
    RM_STATUS status;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osAllocMem. Enter *pAddress =",(int) *pAddress);

    {
        DESCRIPTOR desc;
        U032       nvwatch_nPages;
        U032       nvwatch_pType=PG_SYS;
        U032       nvwatch_VM=0;
        U032       nvwatch_AlignMask;
        U032       nvwatch_minPhys=0;
        U032       nvwatch_maxPhys=0;
        U032       nvwatch_PhysAddr=0;
        //U032       nvwatch_flags=0;
        U032       nvwatch_flags=(PAGECONTIG|PAGEFIXED);
        //U032       nvwatch_flags=(PAGECONTIG|PAGEFIXED|PAGEUSEALIGN);

        nvwatch_nPages    = Size >> 12;
        // Align on size boundary.
        nvwatch_AlignMask = 0;
        //nvwatch_AlignMask = Size;
        
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osAllocMem. nvwatch_nPages =",(int) nvwatch_nPages);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osAllocMem. nvwatch_AlignMask =",(int) nvwatch_AlignMask);
        vmmPageAllocate(
                nvwatch_nPages, 
                nvwatch_pType, 
                nvwatch_VM, 
                nvwatch_AlignMask, 
                nvwatch_minPhys, 
                nvwatch_maxPhys, 
                nvwatch_PhysAddr, 
                nvwatch_flags, 
                &desc.Desc64);
        *pAddress = (VOID *)desc.Desc32[0];
    }
//////  from osallocpages.
//////  vmmPageAllocate(PageCount, PG_SYS, 0, 0, 0, 0, 0, 0, &desc.Desc64);
//////  *pAddress = (VOID *)desc.Desc32[0];

    if (*pAddress != NULL)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osAllocMem. Exit *pAddress =",(int) *pAddress);
        status = RM_OK;
    }
    else
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osAllocMem. ERROR ! *pAddress =",(int) *pAddress);
        status = RM_ERR_NO_FREE_MEM;
    }
    return (status);
}

RM_STATUS NvWatch_osFreeMem
(
    VOID *pAddress
)
{
    RM_STATUS status;


    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osFreeMem. pAddress =",(int) pAddress);
    if(!pAddress)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_osFreeMem. pAddress is Invalid=",(int) pAddress);
        return (RM_OK);
    }    

    {
         U032 nvwatch_hMem;
         U032 nvwatch_flags=0;

         nvwatch_hMem = (U032)pAddress;
         if (!vmmPageFree(nvwatch_hMem, 0)){
             status = RM_ERROR;
         }
         else{
             status = RM_OK;
         }
    }
    return (status);
}


U032 NvWatch_Crush11_MemFree_Block(PHWINFO pDev, U032 block_index)
{
    int i;

    if(nvwatch_block_buffers[block_index] == NULL){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_MemFree_Blocks. Block entry is NULL. block_index=",(int)block_index);
                nvwatch_crush11_failed_data = block_index;
                return(RM_ERROR);
    }
    if(NvWatch_osFreeMem((VOID *)nvwatch_block_buffers[block_index]) != RM_OK){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_MemFree_Blocks. osFreeMem Failed! block_index=",(int)block_index);
                nvwatch_crush11_failed_data = block_index;
                return(RM_ERROR);
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_MemFree_Blocks. osFreeMem SUCCESS! block_index=",(int)block_index);
        nvwatch_block_buffers[block_index] == NULL;
        for(i=0;i<NVWATCH_MAX_PAGES_PER_BLOCK;i++){
                nvwatch_PageArray[block_index][i];
        }
    }

    return(RM_OK);
}

U032 NvWatch_Crush11_UnLock_Block(PHWINFO pDev, U032 bsize, U032 block_index)
{
    U032 PageBase, LinAddr, PageCount, status;
    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_UnLock_Blocks. block_index=",(int)block_index);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_UnLock_Blocks. bsize=",(int)bsize);
    if(nvwatch_block_buffers[block_index] == NULL){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_UnLock_Blocks Block NOT allocated",(int)block_index);
        nvwatch_crush11_failed_data = block_index;
        return(RM_ERROR);
    }
    
    LinAddr = (U032) nvwatch_block_buffers[block_index];
    PageBase = LinAddr >> PAGESHIFT;
    PageCount = bsize >> PAGESHIFT;
    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_UnLock_Block Linear Address=",(int) LinAddr);
    
    status = !vmmLinPageUnLock(PageBase, PageCount, PAGEMAPGLOBAL) ? RM_ERR_DMA_MEM_NOT_UNLOCKED : RM_OK;
    if(status != RM_OK){
        nvwatch_crush11_failed_data = block_index;
    }

    return(status);

}

U032 NvWatch_Crush11_Lock_Block(PHWINFO pDev, U032 bsize, U032 block_index)
{

    U032 PageBase, LinAddr, PageCount;
    U032 LinAddr_save;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Lock_Blocks. block_index=",(int)block_index);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Lock_Blocks. bsize=",(int)bsize);

    LinAddr = (U032) nvwatch_block_buffers[block_index];
    LinAddr_save = (U032) LinAddr;
    PageBase = LinAddr >> PAGESHIFT;
    PageCount = bsize >> PAGESHIFT;

    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Lock_Block Linear Address=",(int) LinAddr);
    
    LinAddr = (U032)vmmLinPageLock(PageBase, PageCount, PAGEMAPGLOBAL);
    
    if (!LinAddr)
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Lock_Block Unable to lock pages. Block=",(int) block_index);
        nvwatch_crush11_failed_data = block_index;
        return (RM_ERR_DMA_MEM_NOT_LOCKED);
    }
    if (!vmmCopyPageTable(LinAddr >> PAGESHIFT, PageCount, (U032)&nvwatch_PageArray[block_index][0], 0))
    {
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Lock_Block Unable to copy page table. Block=",(int) block_index);
        nvwatch_crush11_failed_data = block_index;
        return (RM_ERR_PAGE_TABLE_NOT_AVAIL);
    }
    LinAddr += (LinAddr_save & 0xFFF);
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Lock_Block New Linear Address=",(int) LinAddr);
    nvwatch_block_buffers[block_index] = (void *) LinAddr;
    return(RM_OK);
}

U032 NvWatch_Crush11_Unlock_and_Free_Block(PHWINFO pDev, U032 bsize, U032 block_index)
{
    int status=RM_OK;

    if(NvWatch_Crush11_UnLock_Block(pDev, bsize, block_index) != RM_OK){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block UnLocking FAILED ! Block=",(int)block_index);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block Try to free it anyway. Block=",(int)block_index);
        nvwatch_crush11_failed_data = block_index;
        status=RM_ERROR;
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block UnLocking SUCCESS ! Block=",(int)block_index);
    }

    if(NvWatch_Crush11_MemFree_Block(pDev, block_index)!=RM_OK){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block Freeing FAILED ! Block=",(int)block_index);
        nvwatch_crush11_failed_data = block_index;
        status=RM_ERROR;
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block Freeing SUCCESS ! Block=",(int)block_index);
    }

    return(status);
}

U032 NvWatch_Crush11_Allocate_and_Lock_Block(PHWINFO pDev, U032 bsize, U032 block_index)
{

    if(osAllocMem((VOID **)&nvwatch_block_buffers[block_index], bsize) != RM_OK){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Lock_Block Alloc FAILED ! Block=",(int)block_index);
        nvwatch_crush11_failed_data = block_index;
        return(RM_ERROR);
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Lock_Block Alloc SUCCESS ! Block=",(int)block_index);
        if(NvWatch_Crush11_Lock_Block(pDev, bsize, block_index) != RM_OK){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Lock_Block Locking FAILED ! Block=",(int)block_index);
            if(NvWatch_Crush11_MemFree_Block(pDev, block_index) != RM_OK){
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Lock_Block Freeing the block FAILED also ! Block=",(int)block_index);
            }
            nvwatch_crush11_failed_data = block_index;
            return(RM_ERROR);
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Lock_Block Locking SUCCESS ! Block=",(int)block_index);
            return(RM_OK);
        }
    }
    return(RM_OK);
}

U032 NvWatch_Crush11_Allocate_Blocks(PHWINFO pDev, U032 bsize, U032 num_blocks)
{
    U032 i;
    int status;

    for(i=0;i<num_blocks;i++){
        if(NvWatch_osAllocMem((VOID **)&nvwatch_block_buffers[i], bsize) != RM_OK){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_Blocks Alloc FAILED ! Block=",(int)i);
            nvwatch_crush11_failed_data = i;
            status=RM_ERROR;
            break;
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_Blocks Alloc SUCCESS ! Block=",(int)i);
            status=RM_OK;
        }
    }
//////    for(i=0;i<num_blocks;i++){
//////        if(NvWatch_Crush11_Allocate_and_Lock_Block(pDev, bsize, i)!=RM_OK){
//////            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_Blocks FAILED. num_blocks=",(int)num_blocks);
//////          nvwatch_crush11_failed_data = i;
//////            return(RM_ERROR);
//////        }
//////    }
    return(status);

}

U032 NvWatch_Crush11_Free_Blocks(PHWINFO pDev, U032 bsize, U032 num_blocks)
{
    U032 i;
    int status;

    for(i=0;i<num_blocks;i++){
        if(NvWatch_Crush11_MemFree_Block(pDev, i)!=RM_OK){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block Freeing FAILED ! Block=",(int)i);
            nvwatch_crush11_failed_data = i;
            status=RM_ERROR;
            break;
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Unlock_and_Free_Block Freeing SUCCESS ! Block=",(int)i);
            status=RM_OK;
        }
    }
//////    for(i=0;i<num_blocks;i++){
//////        if(NvWatch_Crush11_Unlock_and_Free_Block(pDev, bsize, i)!=RM_OK){
//////            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Free_Blocks FAILED. num_blocks=",(int)num_blocks);
//////          nvwatch_crush11_failed_data = i;
//////            return(RM_ERROR);
//////        }
//////    }
    return(status);
}

U032 NvWatch_Crush11_Allocate_and_Free_Blocks(PHWINFO pDev, U032 bsize, U032 num_blocks)
{
    int ret_val1,ret_val2;

    if((ret_val1=NvWatch_Crush11_Allocate_Blocks(pDev, bsize, num_blocks))!=RM_OK){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Free_Blocks FAILED. Failed block=",(int)ret_val1);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Free_Blocks. Freeing all allocations",(int)ret_val1);
        if((ret_val2=NvWatch_Crush11_Free_Blocks(pDev, bsize, ret_val1))!=RM_OK){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Free_Blocks. Failure Freeing after alloc failure. Free failure block=",(int)ret_val2);
        }
        return(ret_val1);
    }
    if(NvWatch_Crush11_Free_Blocks(pDev, bsize, num_blocks)!=RM_OK){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Allocate_and_Free_Blocks FAILED. num_blocks=",(int)num_blocks);
        return(RM_ERROR);
    }
    return(num_blocks);
}

void NvWatch_Crush11_Buffer_Entry_Dump(PHWINFO pDev, U032 buffer_index, U032 verbose)
{
    int i;

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
    for(i=0;i<20;i++){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Memory_Page_Dump=",(int) nvwatch_block_buffers[i]);
    }
}

void NvWatch_Crush11_Memory_Page_Dump(PHWINFO pDev, U032 buffer_index, U032 verbose)
{
    int i;

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
    for(i=0;i<20;i++){
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVWATCH:NvWatch_Crush11_Memory_Page_Dump=",(int) nvwatch_PageArray[buffer_index][i]);
    }
}

void NvWatch_Crush11_Memory_Page_Info(PHWINFO pDev, U032 buffer_index, U032 page_index, U032 verbose)
{

}

void NvWatch_Crush11_Tests_help(void)
{
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Crush11 Tests \n\r\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Commands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      a   - Allocate blocks of memory.\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      f   - Free blocks of memory.\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      t   - Allocate available blocks until failure.\n\r");
//  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      f   - Free all allocated blocks.\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      x   - Allocate a number of blocks. (includes freeing of blocks)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      i   - Page Info on allocated blocks.\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      b   - Block Info on allocated blocks.\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit\n\r\n\r");
}

void NvWatch_Crush11_Tests(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  char pstring[]="NvWatch_CrushTests";
  static U032 bsize=0x10000;
  U032 num_buffers=0;
  
  NvWatch_Crush11_Tests_help();
  
  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
  
    in_char = get_winice_input();
    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE;
        break;

      case 'i': // NvInfo Data
      case 'I': // NvInfo Data
          {
              NvWatch_Crush11_Memory_Page_Dump(pDev, 0, 0);
          }
          break;
      case 'b': // NvInfo Data
      case 'B': // NvInfo Data
          {
            NvWatch_Crush11_Buffer_Entry_Dump(pDev, 0, 0);
          }
          break;
      case 'a': // NvInfo Data
      case 'A': // NvInfo Data
          {
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nEnter the number of the buffers to allocate. (hex bytes)");
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \n->");
              if(!NvWatch_GetInputDword(&num_buffers)){
                  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
                  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Setting number of buffers to 0");
                  num_buffers = 0;
              }else{
                  if(num_buffers > NVWATCH_MAX_MEM_BLOCKS){
                      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Buffer number too high");
                      DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\n\n Setting to max buffer number of ",(int) NVWATCH_MAX_MEM_BLOCKS);
                      num_buffers = NVWATCH_MAX_MEM_BLOCKS;
                  }else{
                      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n***Allocating ");
                      DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032) num_buffers);
                      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ");
                      DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032) bsize);
                      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " byte buffers*** \n\n");
                  }
                  {
                      U032 max_block;

                      if(NvWatch_Crush11_Allocate_Blocks(pDev, bsize, num_buffers)!=RM_OK){
                          max_block=nvwatch_crush11_failed_data-1;
                      }else{
                          max_block=num_buffers;
                      }

                      if(max_block==num_buffers){
                          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nAllocation Successful ! Total memory locked was ", (int) (num_buffers*bsize));
                      }else{
                          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nAllocation Failure ! Total memory requested was ", (int) (num_buffers*bsize));
                          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nTotal memory locked was ", (int) (max_block*bsize));
                      }
                  }
              }
          }
          break;
      case 'f': // NvInfo Data
      case 'F': // NvInfo Data
            {
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nEnter the number of the buffers to allocate. (hex bytes)");
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \n->");
                if(!NvWatch_GetInputDword(&num_buffers)){
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Setting number of buffers to 0");
                    num_buffers = 0;
                }else{
                    if(num_buffers > NVWATCH_MAX_MEM_BLOCKS){
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Buffer number too high");
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\n\n Setting to max buffer number of ",(int) NVWATCH_MAX_MEM_BLOCKS);
                        num_buffers = NVWATCH_MAX_MEM_BLOCKS;
                    }else{
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n***Freeing ");
                        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032) num_buffers);
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ");
                        DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032) bsize);
                        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " byte buffers*** \n\n");
                    }
                    {
                        U032 max_block;

                        if(NvWatch_Crush11_Free_Blocks(pDev, bsize, num_buffers)!=RM_OK){
                            max_block=nvwatch_crush11_failed_data-1;
                        }else{
                            max_block=num_buffers;
                        }

                        if(max_block==num_buffers){
                            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nMemory Free Successful ! Total memory freed was ", (int) (num_buffers*bsize));
                        }else{
                            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nMemory Free Failure ! Total memory requested to free was ", (int) (num_buffers*bsize));
                            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nTotal memory freed was ", (int) (max_block*bsize));
                        }
                    }
                }
            }
            break;
      case 'x': // NvInfo Data
      case 'X': // NvInfo Data
        {
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nEnter the number of the buffers to allocate. (hex bytes)");
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \n->");
            if(!NvWatch_GetInputDword(&num_buffers)){
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Setting number of buffers to 0");
                num_buffers = 0;
            }else{
                if(num_buffers > NVWATCH_MAX_MEM_BLOCKS){
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Buffer number too high");
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\n\n Setting to max buffer number of ",(int) NVWATCH_MAX_MEM_BLOCKS);
                    num_buffers = NVWATCH_MAX_MEM_BLOCKS;
                }else{
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n***Allocating ");
                    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032) num_buffers);
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " ");
                    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, (U032) bsize);
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, " byte buffers*** \n\n");
                }
                {
                    U032 max_block;

                    max_block=NvWatch_Crush11_Allocate_and_Free_Blocks(pDev, bsize, num_buffers);
                    if(max_block==num_buffers){
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nAllocation Successful ! Total memory locked was ", (int) (num_buffers*bsize));
                    }else{
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nAllocation Failure ! Total memory requested was ", (int) (num_buffers*bsize));
                        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nTotal memory locked was ", (int) (max_block*bsize));
                    }
                }
            }
        }
        break;
//////      case 's': // Client Data       
//////      case 'S': // Client Data       
//////        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nEnter the size of the buffers to allocate. (hex bytes)");
//////        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \n->");
//////        if(!NvWatch_GetInputDword(&bsize)){
//////            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
//////            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Size set to default 64KB");
//////            bsize = 0x10000;
//////
//////        }else{
//////            bsize &= 0xFFFFF000;
//////            if(bsize == 0){
//////                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nError: Invalid size");
//////                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n Size set to default 64KB");
//////                bsize = 0x10000;
//////            }
//////            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe buffer size is set to ", (int) bsize);
//////        }
//////        break;
      default: // Help
        NvWatch_Crush11_Tests_help();
        break;  
    }
  }

}

/////void NvWatch_Crush11_Tests_Api(PHWINFO pDev, U032* func_params)
/////{
/////
/////    switch(func_params->function){
/////     case NVWATCH_CRUSH_API_ALLOCMEM:
/////         {
/////             U032 max_block;
/////             U032 num_buffers;
/////
/////             num_buffers=func_params->param1;
/////             bsize=func_params->param2;
/////
/////             if(NvWatch_Crush11_Allocate_Blocks(pDev, bsize, num_buffers)!=RM_OK){
/////                 max_block=nvwatch_crush11_failed_data-1;
/////             }else{
/////                 max_block=num_buffers;
/////             }
/////
/////             if(max_block==num_buffers){
/////                 DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nAllocation Successful ! Total memory locked was ", (int) (num_buffers*bsize));
/////             }else{
/////                 DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nAllocation Failure ! Total memory requested was ", (int) (num_buffers*bsize));
/////                 DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nTotal memory locked was ", (int) (max_block*bsize));
/////             }
/////         }
/////         break;
/////       case NVWATCH_CRUSH_API_FREEMEM:
/////           {
/////               U032 max_block;
/////               U032 num_buffers;
/////
/////               num_buffers=func_params->param1;
/////               bsize=func_params->param2;
/////
/////               if(NvWatch_Crush11_Free_Blocks(pDev, bsize, num_buffers)!=RM_OK){
/////                   max_block=nvwatch_crush11_failed_data-1;
/////               }else{
/////                   max_block=num_buffers;
/////               }
/////
/////               if(max_block==num_buffers){
/////                   DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nMemory Free Successful ! Total memory freed was ", (int) (num_buffers*bsize));
/////               }else{
/////                   DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nMemory Free Failure ! Total memory requested to free was ", (int) (num_buffers*bsize));
/////                   DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nTotal memory freed was ", (int) (max_block*bsize));
/////               }
/////           }
/////           break;
/////      default: // Help
/////        NvWatch_Crush11_Tests_help();
/////        break;  
/////    }
/////}
/////

void NvWatch_Testwc(void)
{
  char command_string[] = "wc;lines 25;";
  
  _asm  push ss
  _asm  pop  ds
  _asm  mov  ax, 0x7d
  _asm  mov  cx,20
  _asm  lea  esi, command_string
  _asm  int  41h
}

VOID NvWatch_QuickDiag_fifo(PHWINFO pDev, PNVWATCHDIAGSTRUCT pdiagstruct)
{
// Check the fifo first.
/* Key items:
        - Is the fifo enabled and running ?
             --check for any interrupts/errors pending.
             --check that the pusher and puller are enabled.
             --check the reassign.
             --what channel is currently running. 
             --check the mode.
             --how does the fifo context look ? 
*/

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n**********************************************************\n");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Running Fifo diagnostic ...");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n**********************************************************\n");

    if(pdiagstruct->nv_pfifo_intr_0){       
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Fifo has Interrupts pending. NV_PFIFO_INTR_0=", (int) pdiagstruct->nv_pfifo_intr_0);
        if(!pdiagstruct->nv_pfifo_intr_en_0){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Fifo has Interrupts disabled. NV_PFIFO_INTR_EN_0=", (int) pdiagstruct->nv_pfifo_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING !!!--- The fifo will be stalled under these conditions.\n");
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Fifo has Interrupts enabled. NV_PFIFO_INTR_EN_0=", (int) pdiagstruct->nv_pfifo_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING !!!--- The fifo appears to be waiting for the RM for service.\n");
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "                  If this condition persists ... There could be a system problem.\n");
        }
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Fifo has NO Interrupts pending. NV_PFIFO_INTR_0=", (int) pdiagstruct->nv_pfifo_intr_0);
        if(!pdiagstruct->nv_pfifo_intr_en_0){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe Fifo has Interrupts disabled. NV_PFIFO_INTR_EN_0=", (int) pdiagstruct->nv_pfifo_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  -The fifo is in an idle state. (disabled)\n");
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Fifo has Interrupts enabled. NV_PFIFO_INTR_EN_0=", (int) pdiagstruct->nv_pfifo_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  -All appears to be normal here.\n\n");
        }
    }

    if(pdiagstruct->nv_pfifo_caches & NV_PFIFO_CACHES_REASSIGN_ENABLED){      
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "The Fifo REASSIGN is ENABLED allowing for lightweight fifo context switching.\n");
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  -NV_PFIFO_CACHES=", (int) pdiagstruct->nv_pfifo_caches);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Fifo REASSIGN is DISABLED. NV_PFIFO_CACHES=", (int) pdiagstruct->nv_pfifo_caches);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING !!!---The fifo CANNOT context switch under this condition.\n\n");
    }

    if(pdiagstruct->nv_pfifo_cache1_push0 && pdiagstruct->nv_pfifo_cache1_pull0){ 
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The fifo pusher and puller are both ENABLED. NV_PFIFO_CACHE1_PUSH0=", (int) pdiagstruct->nv_pfifo_cache1_push0);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                             NV_PFIFO_CACHE1_PULL0=", (int) pdiagstruct->nv_pfifo_cache1_pull0);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "This is the standard running condition.\n\n");
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Either the fifo pusher or puller are DISABLED. NV_PFIFO_CACHE1_PUSH0=", (int) pdiagstruct->nv_pfifo_cache1_push0);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                               NV_PFIFO_CACHE1_PULL0=", (int) pdiagstruct->nv_pfifo_cache1_pull0);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING---The fifo will be STALLED under these conditions.\n\n");
    }
    
    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The current channel context in the fifo is channel ", (int) (pdiagstruct->nv_pfifo_cache1_push1 & 0xFF));
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "running in ");
    if(pdiagstruct->nv_pfifo_cache1_push1 & 0x100){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "DMA mode.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "PIO mode.\n");
    }
    
    if(IsNV4(pDev) || IsNV5(pDev) || IsNV0A(pDev)){
        if((pdiagstruct->nv_pfifo_cache1_push1 & 0xFF)==0xf){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING !!!--- This channel context indicates the fifo may be in an uninitialized state\n");
        }
    }else{
        if(IsNV10orBetter(pDev)){
            if((pdiagstruct->nv_pfifo_cache1_push1 & 0xFF)==0x1f){
                DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING !!!--- This channel context indicates the fifo may be in an uninitialized state\n");
            }
        }
    }
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
    
    if(pdiagstruct->nv_pfifo_dma_put == pdiagstruct->nv_pfifo_dma_get){      
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "DMA PUT and GET are equal indicating the fifo is empty. ",(int) pdiagstruct->nv_pfifo_dma_get);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  -Use the d option from the main menu to view the fifo contents.\n");
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "DMA PUT and GET are NOT equal indicating the fifo is NOT empty. NV_PFIFO_CACHE1_GET = ",(int) pdiagstruct->nv_pfifo_dma_get);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "                                                                NV_PFIFO_CACHE1_PUT = ",(int) pdiagstruct->nv_pfifo_dma_put);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "   -If this condition persists the fifo may be disabled or stalled.\n");
    }
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n**********************************************************\n");
}

VOID NvWatch_QuickDiag_master(PHWINFO pDev, PNVWATCHDIAGSTRUCT pdiagstruct)
{

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n**********************************************************\n");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Running Master Control diagnostic ...");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n**********************************************************\n");

    if(pdiagstruct->nv_pmc_intr_0){       
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Chip has Interrupts pending. NV_PMC_INTR_0=", (int) pdiagstruct->nv_pmc_intr_0);
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PMEDIA, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Media Port engine INT is PENDING.\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PFIFO, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Fifo engine INT is       PENDING.\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Graphics engine INT is   PENDING.\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PVIDEO, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Video engine INT is      PENDING.\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PTIMER, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Timer engine INT is      PENDING.\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "CRTC INT is              PENDING. (VBLANK)\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _PBUS, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Bus engine INT is        PENDING.\n");
        }
        if(pdiagstruct->nv_pmc_intr_0 & DRF_DEF(_PMC, _INTR_0, _SOFTWARE, _PENDING)){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Software engine INT is   PENDING.\n");
        }

        if(!pdiagstruct->nv_pmc_intr_en_0){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Chip has Interrupts disabled. NV_PMC_INTR_EN_0=", (int) pdiagstruct->nv_pmc_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "---WARNING !!!--- The Chip will be stalled under these conditions.\n\n");
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Chip has Interrupts enabled. NV_PMC_INTR_EN_0=", (int) pdiagstruct->nv_pmc_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  -All appears to be normal here.\n\n");
        }
    }else{
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Chip has NO Interrupts pending. NV_PMC_INTR_0=", (int) pdiagstruct->nv_pmc_intr_0);
        if (!pDev->Vga.Enabled){
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "---WARNING !!!---This would not be expected in hires mode.\n");
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "  -The CRTC should need to service VBLANK.\n");

        }
        if(!pdiagstruct->nv_pmc_intr_en_0){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe Chip has Interrupts disabled. NV_PMC_INTR_EN_0=", (int) pdiagstruct->nv_pmc_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  -The Chip is in an idle state. (disabled)\n\n");
        }else{
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The Chip has Interrupts enabled. NV_PMC_INTR_EN_0=", (int) pdiagstruct->nv_pmc_intr_en_0);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  -All appears to be normal here.\n\n");
        }
    }
    
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PMEDIA, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Media Port Engine is                    ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Media Port Engine is                    DISABLED.\n");
    }
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Fifo Engine is                          ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Fifo Engine is                          DISABLED.\n");
    }
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Graphics Engine is                      ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Graphics Engine is                      DISABLED.\n");
    }
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PPMI, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Timer and bus mastering logic Engine is ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Timer and bus mastering logic Engine is DISABLED.\n");
    }
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PFB, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Frame Buffer Engine is                  ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Frame Buffer Engine is                  DISABLED.\n");
    }
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PCRTC, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Dac and CRTC devices are                ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Dac and CRTC devices are                DISABLED.\n");
    }
    if(pdiagstruct->nv_pmc_enable & DRF_DEF(_PMC, _ENABLE, _PVIDEO, _ENABLED)){
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Video Scalar device is                  ENABLED.\n");
    }else{
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Video Scalar device is                  DISABLED.\n");
    }

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n**********************************************************\n");
}

int NvWatch_QuickDiag(PHWINFO pDev)
{
    NVWATCHDIAGSTRUCT diagstruct,*pdiagstruct;

    pdiagstruct = (PNVWATCHDIAGSTRUCT) &diagstruct;

    if(IsNV4(pDev) || IsNV5(pDev) || IsNV0A(pDev)){
        NvWatch_NV04_FillDiagStruct(pDev, pdiagstruct);
    }else{
         if(IsNV10orBetter(pDev)){
             NvWatch_NV04_FillDiagStruct(pDev, pdiagstruct);
         }else{
             DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR: Aborting ... device is disabled.\n");
             return(-1);
         }
    }
  
  NvWatch_QuickDiag_master(pDev, pdiagstruct);
  NvWatch_QuickDiag_fifo(pDev, pdiagstruct);
  //NvWatch_QuickDiag_graphics(pDev, pdiagstruct);
  //NvWatch_QuickDiag_instancemem(pDev, pdiagstruct);
}

//****TOP LEVEL MENU***************************************************************************

U032 NvWatch_Edit_NvRegs(PHWINFO pDev)
{
  char instring[]="00000000";
  DWORD ddata=0, daddr=0, save_offset;
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \nEnter the Offset of the Nv Register to query.");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  \n->");

  if(!NvWatch_GetInputDword(&daddr)){
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
    return(0);
  }  
  save_offset = daddr; 
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe data read is ", REG_RD32(daddr));
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Enter the new value for this register ->");

  if(!NvWatch_GetInputDword(&ddata)){
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  Error: Abort action");
    return(0);
  }  

  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\nThe new value written is ", ddata);
  REG_WR32(save_offset, ddata);
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The data read from the register is ", REG_RD32(save_offset));
}

void NvWatch_help(void)
{
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Resource Manager System View Extension for NV4\n\r\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Commands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      sx  - Set Device Context for Queries (Default is Device0)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      f   - Dump Fifo state\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      g   - Dump Graphics state\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      m   - Dump Master Control state\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      v   - Dump Video state\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      c   - Dump CRTC registers\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      n   - Instance Memory Stuff\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      x   - Context Information\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      d   - Fifo Info\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      e   - Edit Nv Registers\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      r   - Resource Manager Data Structures\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      t   - Quick Diag\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit\n\r\n\r");
}

void NvWatch_DumpCRTCRegs(PHWINFO pDev)
{
    U008 oldCR1F;
    U008 data08;
    U032 index;

    U032 Head = 0;  // To get this to compile, I'm hardcoding to Head 0. Need to add Head # to user interface, dump multiple sets.

    // save off the contents of CR1F
    CRTC_RD(NV_CIO_SR_LOCK_INDEX, oldCR1F, Head);

    // Unlock extended crtc registers.
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RO_VALUE, Head);

    // print the 64 CRTC registers.
    for (index = 0; index < 64; index++)
    {
        if ((index % 16) == 0)
        {
            // go to next line
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r");
        }

        CRTC_RD(index, data08, Head);
        vmmOutDebugStringValue("#AL", data08);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "  ");
    }

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\r");

    // restore contents of CR1F
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, oldCR1F, Head);
}


// This routine could be removed and the functions added to the pDev.
// For now this should be fine.
U032 NvWatch_Dispatch(PHWINFO pDev, U032 function)
{
    if(IsNV10orBetter(pDev)){
        switch(function){
            case NVWATCH_FUNC_GETHASHSIZE:
                return(NvWatch_NV10_GetHashSize(pDev)); break;
            case NVWATCH_FUNC_DUMP_FIFO:
                NvWatch_NV10_Dump_Fifo(pDev);             return(0); break;
            case NVWATCH_FUNC_DUMP_GRAPHICS:
                NvWatch_NV10_Dump_Graphics(pDev);         return(0); break;
            case NVWATCH_FUNC_DUMP_MASTER:
                NvWatch_NV10_Dump_Master(pDev);           return(0); break;
            case NVWATCH_FUNC_DUMP_VIDEO:
                NvWatch_NV10_Dump_Video(pDev);            return(0); break;
            case NVWATCH_FUNC_QUERYDEVICECONTEXT:
                NvWatch_NV10_Query_DeviceContext(pDev);   return(0); break;
            case NVWATCH_FUNC_DMAPUSH_AND_FIFO:
                NvWatch_NV10_DmaPushAndFifo(pDev);(pDev); return(0); break;
        }
    }else{
        switch(function){
            case NVWATCH_FUNC_GETHASHSIZE:
                return(NvWatch_NV04_GetHashSize(pDev)); break;
            case NVWATCH_FUNC_DUMP_FIFO:
                NvWatch_NV04_Dump_Fifo(pDev);             return(0); break;
            case NVWATCH_FUNC_DUMP_GRAPHICS:              
                NvWatch_NV04_Dump_Graphics(pDev);         return(0); break;
            case NVWATCH_FUNC_DUMP_MASTER:                
                NvWatch_NV04_Dump_Master(pDev);           return(0); break;
            case NVWATCH_FUNC_DUMP_VIDEO:                 
                NvWatch_NV04_Dump_Video(pDev);            return(0); break;
            case NVWATCH_FUNC_QUERYDEVICECONTEXT:         
                NvWatch_NV04_Query_DeviceContext(pDev);   return(0); break;
            case NVWATCH_FUNC_DMAPUSH_AND_FIFO:
                NvWatch_NV04_DmaPushAndFifo(pDev);(pDev); return(0); break;
        }
    }
}

#endif

// JJV - Just leave the prototype so that this can link. Basically no ifdef DEBUG in VXDSTUB.ASM.
void DotN_cmd_c(void)
{
#ifdef DEBUG
  BOOL keep_running=TRUE;
  char in_char;
  U032 dword_char;
  U032 hash_size;
  char pstring[]="NvWatch";
  PHWINFO pDev;

  pDev = NvDBPtr_Table[0];
 
  NvWatch_help();

  hash_size = NvWatch_Dispatch(pDev, NVWATCH_FUNC_GETHASHSIZE);
  hash_size >>= 16;
  hash_size &= 0x3;
  
  switch(hash_size){
    case 0:
      NvWatch_num_objects = 0x1000/8;
      break;
    case 1:
      NvWatch_num_objects = 0x2000/8;
      break;
    case 2:
      NvWatch_num_objects = 0x4000/8;
      break;
    case 3:
      NvWatch_num_objects = 0x8000/8;
      break;
  }
  
  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
    in_char = get_winice_input();
    dword_char = (U032) in_char;

    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE; break;
      case 'f': // Fifo Registers
      case 'F': // Fifo Registers
        NvWatch_Dispatch(pDev, NVWATCH_FUNC_DUMP_FIFO); break;
      case 'g': // Graphics Registers
      case 'G': // Graphics Registers
        NvWatch_Dispatch(pDev,NVWATCH_FUNC_DUMP_GRAPHICS); break;
      case 'm': // Master Control Registers
      case 'M': // Master Control Registers
        NvWatch_Dispatch(pDev,NVWATCH_FUNC_DUMP_MASTER); break;
      case 'v': // Video Registers
      case 'V': // Video Registers
        NvWatch_Dispatch(pDev,NVWATCH_FUNC_DUMP_VIDEO); break;
      case 'e': // Edit Data
      case 'E': // Edit Data
        NvWatch_Edit_NvRegs(pDev);
        break;
      case 'n': // Instance Memory stuff
      case 'N': // Instance Memory stuff
        NvWatch_Query_InstanceMemory(pDev);
        break;
      case 'x': // Context Data
      case 'X': // Context Data
        NvWatch_Dispatch(pDev,NVWATCH_FUNC_QUERYDEVICECONTEXT);
      case 'r': // RM Stuff
      case 'R': // RM Stuff
        NvWatch_RMData(pDev);
        break;
      case 'd': // DmaPush and Fifo
      case 'D': // DmaPush and Fifo
        NvWatch_Dispatch(pDev,NVWATCH_FUNC_DMAPUSH_AND_FIFO); break;
      case 't': // Chip Diag
      case 'T': 
        NvWatch_QuickDiag(pDev);
        break;
      case 'p': // Crush11 Tests
      case 'P': 
        NvWatch_Crush11_Tests(pDev);
        break;
      case 's': // Switch device
      case 'S': // Switch Device
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "    Enter the Device to query. (0-8)");
        in_char = get_winice_input();
        switch (in_char)
        {
          case '0':
            pDev = NvDBPtr_Table[0]; break;
            //NvWatch_Device = 0; break;
          case '1':
            pDev = NvDBPtr_Table[1]; break;
            //NvWatch_Device = 1; break;
          case '2':
            pDev = NvDBPtr_Table[2]; break;
            //NvWatch_Device = 2; break;
          case '3':
            pDev = NvDBPtr_Table[3]; break;
            //NvWatch_Device = 3; break;
          case '4':
            pDev = NvDBPtr_Table[4]; break;
            //NvWatch_Device = 4; break;
          case '5':
            pDev = NvDBPtr_Table[5]; break;
            //NvWatch_Device = 5; break;
          case '6':
            pDev = NvDBPtr_Table[6]; break;
            //NvWatch_Device = 6; break;
          case '7':
            pDev = NvDBPtr_Table[7]; break;
            //NvWatch_Device = 7; break;
          case '8':
            pDev = NvDBPtr_Table[8]; break;
            //NvWatch_Device = 8; break;
        }

        if(pDev == NULL)
        {
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nError: Not a valid device. setting default ...\r\n");
          pDev = NvDBPtr_Table[0];
          //NvWatch_Device = 0;
        }
          
        break;
      case 'c':
      case 'C':
          // Dump crtc registers
          NvWatch_DumpCRTCRegs(pDev);
          break;

      default: // Help
        NvWatch_help();
        break;  
    }
  }
#endif
}

