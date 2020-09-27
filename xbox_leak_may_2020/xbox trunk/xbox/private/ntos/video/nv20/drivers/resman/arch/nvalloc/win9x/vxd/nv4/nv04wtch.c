
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
// (C) Copyright SGS-THOMSON Microelectronics Inc., 1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NV04wtch.C                                                        *
*   Winice .N Debug Interface. NV04 code                                    *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Vito 2/6/98
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"        // put this first. we redefine macro SF_BIT in nvrm.h
#include <nv_ref.h>
#include <nv4_ref.h>
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
#include "nv4_hal.h"

#ifdef DEBUG

//
// prototypes
//
U032 NvWatch_GetObjectFromInst  (PHWINFO, U032, U032 *);
U032 NvWatch_Dump_HT_Object_From_Handle(PHWINFO, U032);
VOID NvWatch_display_prompt     (PHWINFO, char *);
char get_winice_input           (VOID);

//
// defines
//
#define NVWATCH_SIMPLE      0
#define NVWATCH_COMPLEX     1
#define INSTANCE_MEM_SPACE  nvAddr 

//****NV CONTEXT MENU******************************************************************************

void NvWatch_NV04_Dump_SubChannelContext(PHWINFO pDev, U032 p_channel)
{
  U032  sub_counter;
  U032  nvwatch_fifo_engine_cache1;
  U032  enginet, *inst_ptr, tempv;

  nvwatch_fifo_engine_cache1 = REG_RD32(NV_PFIFO_CACHE1_ENGINE);
  // For each subchannel (Dump the Engine, then the object).
  for(sub_counter=0;sub_counter<NUM_SUBCHANNELS;sub_counter++){
    enginet = nvwatch_fifo_engine_cache1;
    enginet &= 3;
    switch(enginet){
      case 0:
        // We need to get the channel.
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Software Object in Fifo cache0 for Subchannel ",sub_counter);
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Accessing pDev->DBfifoTable ...");
        if (pDev->DBfifoTable[p_channel].InUse == FALSE)
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "ERROR: pDev->DBfifoTable NOT InUse !??");
        else{
          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Object Name = ",pDev->DBfifoTable[p_channel].SubchannelContext[sub_counter]->Name);
          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Object Class = ",pDev->DBfifoTable[p_channel].SubchannelContext[sub_counter]->Class->Type);
          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "CHID = ",pDev->DBfifoTable[p_channel].SubchannelContext[sub_counter]->ChID);
          DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Subchannel = ",pDev->DBfifoTable[p_channel].SubchannelContext[sub_counter]->Subchannel);
//////    U032       Name;
//////    PCLASS     Class;
//////    U032       ChID;
//////    U032       Subchannel;
//////    NODE       Node[NUM_FIFOS];
//////    POBJECT    Next;
//////    PDMAOBJECT NotifyXlate;
//////    U032       NotifyAction;
//////    U032       NotifyTrigger;
//////    PEVENTNOTIFICATION NotifyEvent;
        }  
        break;
      case 1:
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Graphics Object in Fifo cache0 for Subchannel ",sub_counter);
        
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "   NV_PGRAPH_CTX_CACHE1 = ",REG_RD32(NV_PGRAPH_CTX_CACHE1(sub_counter)));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "   NV_PGRAPH_CTX_CACHE2 = ",REG_RD32(NV_PGRAPH_CTX_CACHE2(sub_counter)));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "   NV_PGRAPH_CTX_CACHE3 = ",REG_RD32(NV_PGRAPH_CTX_CACHE3(sub_counter)));
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "   NV_PGRAPH_CTX_CACHE4 = ",REG_RD32(NV_PGRAPH_CTX_CACHE4(sub_counter)));
        
        tempv = (U032) REG_RD32( NV_PGRAPH_CTX_CACHE4(sub_counter) );
        tempv &= 0xFFFF;
        tempv <<= 4;
        tempv += (U032) (INSTANCE_MEM_SPACE + (pDev->Pram.HalInfo.PraminOffset/4));
        inst_ptr = (U032 *) tempv;
        
        {
          U032 DHandle;
          
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nDumping the CTX_CACHE4 Object ...");
          tempv = (U032) REG_RD32( NV_PGRAPH_CTX_CACHE4(sub_counter) );
          tempv &= 0xFFFF;
          if(!NvWatch_GetObjectFromInst(pDev, tempv, &DHandle))
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR: Object NOT FOUND !\n");
          else{  
            NvWatch_Dump_HT_Object_From_Handle(pDev, DHandle);
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n\n");
          }  
        }
        break;
      case 2:
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "DVD Object in Fifo cache0 for Subchannel ",sub_counter);
        break;
      default:
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR: INVALID Object in Fifo cache0!");
        break;
    }
    nvwatch_fifo_engine_cache1 >>= 4;
  }
}

void NvWatch_NV04_ContextInfo_help(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCommands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      s   - Query SubChannel Contexts (Currently supports active channel ONLY\n\r");
//  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "            Active Channel = ",NvWatch_channel);
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      ?   - ???\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      q   - quit this\n\r\n\r");
}

VOID NvWatch_NV04_Query_DeviceContext(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  U032 m_channel=0;
  char pstring[]="NvWatch Context";

  NvWatch_NV04_ContextInfo_help(pDev);
  
  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
  
    in_char = get_winice_input();
    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE;
        break;
      case 's':
      case 'S':
        {
          U032 save_pgraph_fifo;
          
          // Clear the NV_PGRAPH_FIFO so that it can be read.
          save_pgraph_fifo = REG_RD32(NV_PGRAPH_FIFO);
          REG_WR32(NV_PGRAPH_FIFO, 0);
          NvWatch_NV04_Dump_SubChannelContext(pDev, m_channel);
          // Restore the NV_PGRAPH_FIFO.
          REG_WR32(NV_PGRAPH_FIFO, save_pgraph_fifo);
        }  
        break;
      case 'c': // Dump hash table
      case 'C': // Dump hash table
        break;
      default: // Help
        NvWatch_NV04_ContextInfo_help(pDev);
        break;  
    }
  }
}

//****DMA PUSHER AND FIFO MENU******************************************************************************

// THE FIFO: Actually 2X the HW size. A second alias image exists.
//-----------------------------------------
//|0                                      | <-- PFIFO_PUT (word pointer)
//|---------------------------------------| <-- PFIFO_GET ( "     "    )
//|1                                      |
//|---------------------------------------| If PFIFO_PUT==PFIFO_GET then the Fifo is empty
//|2                                      |
//|---------------------------------------| If (PFIFO_PUT+128)%256==PFIFO_GET then the Fifo is full
//|...                                    |
//|---------------------------------------| 0 <= PFIFO_PUT, PFIFO_GET <= 255*4
//|---------------------------------------|
//|127                                    |
//|---------------------------------------|
//|128 == 0 alias                         |
//|---------------------------------------|
//|129 == 1 alias                         |
//|---------------------------------------|
//|...                                    |
//|---------------------------------------|
//|---------------------------------------|
//|255 == 127 alias                       |
//-----------------------------------------
void NvWatch_NV04_DumpFifoContents(PHWINFO pDev)
{
  U032 save_puller, save_pusher;
  U032 nvw_fifo_get,nvw_fifo_put;
  U032 fifo_count;
  BOOL fifo_is_empty=FALSE,fifo_is_full=FALSE;
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
  
  // In order to dump the fifo the puller and pusher must be disabled.
  save_puller = REG_RD32(NV_PFIFO_CACHE1_PULL0);
  save_pusher = REG_RD32(NV_PFIFO_CACHE1_PUSH0);
  REG_WR32(NV_PFIFO_CACHE1_PULL0,0);
  REG_WR32(NV_PFIFO_CACHE1_PUSH0,0);

  // The Fifo is 128 entries deep.
  nvw_fifo_get = REG_RD32(NV_PFIFO_CACHE1_GET);
  nvw_fifo_put = REG_RD32(NV_PFIFO_CACHE1_PUT);

  for(fifo_count=0;fifo_count<128;fifo_count++){
    
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, fifo_count);
    
    if( ( ( (nvw_fifo_get%(128*4)) /4) == fifo_count) && (( (nvw_fifo_put%(128*4)) /4) == fifo_count)){
      if(nvw_fifo_get == nvw_fifo_put)
        fifo_is_empty = TRUE;
      else  
        fifo_is_full = TRUE;
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,    " PFIFO_GET->\n");
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,    "           PFIFO_PUT-> Method = ");
    }else{
      if(( (nvw_fifo_get%(128*4)) /4) == fifo_count)
        DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,  " PFIFO_GET-> Method = ");
      else{  
        if(( (nvw_fifo_put%(128*4)) /4) == fifo_count)
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS," PFIFO_PUT-> Method = ");
        else
          DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"             Method = ");
      }  
    }  
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, REG_RD32(NV_PFIFO_CACHE1_METHOD(fifo_count)) );
    
    
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"             Data = ");
    DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, REG_RD32(NV_PFIFO_CACHE1_DATA(fifo_count)) );
    
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"\n");
  }

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "Fifo Methods decode as 15:13-Subchannel 12:2-Method offset.\n");

  if(fifo_is_empty){
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nThe Fifo is EMPTY\n\r");
  }else{
    if(fifo_is_full){
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nThe Fifo is FULL\n\r");
    }else{
      // This should be an error !!! Never full and Empty!
      DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR: The Fifo is Neither FULL or EMPTY!!!\n\r");
    }
  }  
  
  // Restore the pusher and puller registers.
  REG_WR32(NV_PFIFO_CACHE1_PULL0,save_puller);
  REG_WR32(NV_PFIFO_CACHE1_PUSH0,save_pusher);
}

void NvWatch_NV04_DumpUserDmaBuffersAtGet(PHWINFO pDev)
{
    U032 channel_count;
    U032 current_channel;
    U032 channel_get;
    U032 channel_dma_inst;
    U032 channel_get_page, channel_get_offset,channel_dmabuffer_physical;
    U032 *instance_data_ptr, *instance_pte_ptr;

    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n*********************************************");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCalculating Physical addresses for PushBuffers\n");
    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,   "*********************************************\n");

    current_channel = REG_RD_DRF(_PFIFO, _CACHE1_PUSH1, _CHID);
    for(channel_count=0;channel_count<NUM_FIFOS;channel_count++){
        // Is this channel running in DMA Mode ?
        if((REG_RD32(NV_PFIFO_MODE)>>channel_count)&1){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "DMA Channel=",(int)channel_count);
            // Get the get pointer value.
            // From fifo context or the fifo engine ?
            if(channel_count==current_channel){
                // Go to the fifo engine.
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "This is the current channel=",(int)channel_count);
                channel_get = REG_RD32(NV_PFIFO_CACHE1_DMA_GET);
                channel_dma_inst = REG_RD32(NV_PFIFO_CACHE1_DMA_INSTANCE);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The get pointer from the fifo engine=",(int)         channel_get);
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The dma instance from the fifo engine=",(int)channel_dma_inst);

            }else{
                // Go to the fifo context area in instance memory.
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "This is NOT the current channel=",(int)channel_count);
                {
                   U032 *ptr_fifocontextarea;

                   switch(REG_RD_DRF(_PFIFO, _RAMHT, _SIZE)){
                     case NV_PFIFO_RAMHT_SIZE_4K:
                       ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x1000) /4) );
                       break;
                     case NV_PFIFO_RAMHT_SIZE_8K:
                       ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x2000) /4) );
                       break;
                     case NV_PFIFO_RAMHT_SIZE_16K:
                       ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x4000) /4) );
                       break;
                     case NV_PFIFO_RAMHT_SIZE_32K:
                       ptr_fifocontextarea = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ( (pDev->Pram.HalInfo.PraminOffset+0x10000+0x8000) /4) );
                       break;
                   }
                   channel_get = (U032) *(ptr_fifocontextarea+1+(channel_count*8));
                   channel_dma_inst = (U032) *(ptr_fifocontextarea+3+(channel_count*8));
                   channel_dma_inst &= 0xFFFF;
                   DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The get pointer from the fifo context=",(int)         channel_get);
                   DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "The dma instance from the fifo context=",(int)channel_dma_inst);
                }
            } // equal current channel

            channel_get_page = (U032) channel_get >> 12;
            channel_get_offset = (U032) channel_get & 0xFFF;
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Channel get pointers page=",(int)   channel_get_page);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Channel get pointers offset=",(int) channel_get_offset);


            instance_data_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset+(channel_dma_inst<<4))/4));
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Linear address of Push buffer context=",(int)instance_data_ptr);
            // System memory DMA contexts will have all of the PTE's listed in instance memory.(scatter/gather)
            // AGP and FB DMA Contexts will only require a base DMA context.
            // Check the type bits of the DMA context in instance memory.
            switch( ((*instance_data_ptr)>>16) & 3){
                case 2: /*system memory*/
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "PUSH_BUFFER in SYSTEM MEMORY\n");
                    instance_pte_ptr = (U032 *) instance_data_ptr + (2+channel_get_page);
                    DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Linear address of Push buffer context PTE=",(int)instance_pte_ptr);
                    channel_dmabuffer_physical = (U032) ((*instance_pte_ptr)&0xFFFFF000)+channel_get_offset;
                    break;
                case 3: /*AGP system memory*/
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "PUSH_BUFFER in AGP SYSTEM MEMORY\n");
                    instance_pte_ptr = (U032 *) instance_data_ptr + (2);
                    channel_dmabuffer_physical = (U032) ((*instance_pte_ptr)&0xFFFFF000)+channel_get;
                    break;
                case 0: /*frame buffer NVM*/
                    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "PUSH_BUFFER in FRAME BUFFER MEMORY\n");
                    instance_pte_ptr = (U032 *) instance_data_ptr + (2);
                    channel_dmabuffer_physical = (U032) ((*instance_pte_ptr)&0xFFFFF000)+channel_get;
                    break;
            }
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Physical address of Push buffer at the get pointer=",(int)channel_dmabuffer_physical);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "Don't forget the 12bit ADJUST value for the DMA Context=",(int)((*instance_data_ptr)>>20));
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "ADJUSTED Physical address of Push buffer at the get pointer=",(int)(channel_dmabuffer_physical+((*instance_data_ptr)>>20)));
            DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\n");
        } // DMA Mode
    }
}

void NvWatch_NV04_DmaPushAndFifo_help(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCommands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      f   - Dump Fifo Contents\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      c   - Get Physical address of DmaBuffers at Current Get ptr.(DmaPush channels only)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      d   - Dump DmaPusher Info (Not Implemented)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit this\n\r\n\r");
}

void NvWatch_NV04_DmaPushAndFifo(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  char pstring[]="NvWatch Fifo";

  NvWatch_NV04_DmaPushAndFifo_help(pDev);
  
  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
  
    in_char = get_winice_input();
    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE;
        break;
      case 'f': // Dump fifo 
      case 'F': // Dump fifo 
        NvWatch_NV04_DumpFifoContents(pDev);
        break;
      case 'c': 
      case 'C': 
        NvWatch_NV04_DumpUserDmaBuffersAtGet(pDev);
        break;
      case 'b': 
      case 'B': 
        break;
      default: // Help
        NvWatch_NV04_DmaPushAndFifo_help(pDev);
        break;  
    }
  }
}

void NvWatch_NV04_Dump_Graphics(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nGraphics Engine State:\r\n");
 
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_0             0x00400080  RW-4R  = ", REG_RD32(   NV_PGRAPH_DEBUG_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_1             0x00400084  RW-4R  = ", REG_RD32(   NV_PGRAPH_DEBUG_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_2             0x00400088  RW-4R  = ", REG_RD32(   NV_PGRAPH_DEBUG_2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_3             0x0040008C  RW-4R  = ", REG_RD32(   NV_PGRAPH_DEBUG_3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_INTR                0x00400100  RW-4R  = ", REG_RD32(   NV_PGRAPH_INTR                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_NSOURCE             0x00400108  R--4R  = ", REG_RD32(   NV_PGRAPH_NSOURCE               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_INTR_EN             0x00400140  RW-4R  = ", REG_RD32(   NV_PGRAPH_INTR_EN               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_SWITCH1         0x00400160  RW-4R  = ", REG_RD32(   NV_PGRAPH_CTX_SWITCH1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_SWITCH2         0x00400164  RW-4R  = ", REG_RD32(   NV_PGRAPH_CTX_SWITCH2           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_SWITCH3         0x00400168  RW-4R  = ", REG_RD32(   NV_PGRAPH_CTX_SWITCH3           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_CONTROL         0x00400170  RW-4R  = ", REG_RD32(   NV_PGRAPH_CTX_CONTROL           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_USER            0x00400174  RW-4R  = ", REG_RD32(   NV_PGRAPH_CTX_USER              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FIFO                0x00400720  RW-4R  = ", REG_RD32(   NV_PGRAPH_FIFO                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FFINTFC_ST2         0x00400754  RW-4R  = ", REG_RD32(   NV_PGRAPH_FFINTFC_ST2           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_STATUS              0x00400700  R--4R  = ", REG_RD32(   NV_PGRAPH_STATUS                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_TRAPPED_ADDR        0x00400704  R--4R  = ", REG_RD32(   NV_PGRAPH_TRAPPED_ADDR          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_TRAPPED_DATA        0x00400708  R--4R  = ", REG_RD32(   NV_PGRAPH_TRAPPED_DATA          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_SURFACE             0x0040070C  RW-4R  = ", REG_RD32(   NV_PGRAPH_SURFACE               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_NOTIFY              0x00400714  RW-4R  = ", REG_RD32(   NV_PGRAPH_NOTIFY                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET0            0x00400640  RW-4R  = ", REG_RD32(   NV_PGRAPH_BOFFSET0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET1            0x00400644  RW-4R  = ", REG_RD32(   NV_PGRAPH_BOFFSET1              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET2            0x00400648  RW-4R  = ", REG_RD32(   NV_PGRAPH_BOFFSET2              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET3            0x0040064C  RW-4R  = ", REG_RD32(   NV_PGRAPH_BOFFSET3              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET4            0x00400650  RW-4R  = ", REG_RD32(   NV_PGRAPH_BOFFSET4              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET5            0x00400654  RW-4R  = ", REG_RD32(   NV_PGRAPH_BOFFSET5              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE0              0x00400658  RW-4R  = ", REG_RD32(   NV_PGRAPH_BBASE0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE1              0x0040065c  RW-4R  = ", REG_RD32(   NV_PGRAPH_BBASE1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE2              0x00400660  RW-4R  = ", REG_RD32(   NV_PGRAPH_BBASE2                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE3              0x00400664  RW-4R  = ", REG_RD32(   NV_PGRAPH_BBASE3                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE4              0x00400668  RW-4R  = ", REG_RD32(   NV_PGRAPH_BBASE4                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE5              0x0040066C  RW-4R  = ", REG_RD32(   NV_PGRAPH_BBASE5                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH0             0x00400670  RW-4R  = ", REG_RD32(   NV_PGRAPH_BPITCH0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH1             0x00400674  RW-4R  = ", REG_RD32(   NV_PGRAPH_BPITCH1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH2             0x00400678  RW-4R  = ", REG_RD32(   NV_PGRAPH_BPITCH2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH3             0x0040067C  RW-4R  = ", REG_RD32(   NV_PGRAPH_BPITCH3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH4             0x00400680  RW-4R  = ", REG_RD32(   NV_PGRAPH_BPITCH4               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT0             0x00400684  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLIMIT0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT1             0x00400688  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLIMIT1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT2             0x0040068c  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLIMIT2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT3             0x00400690  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLIMIT3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT4             0x00400694  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLIMIT4               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT5             0x00400698  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLIMIT5               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BSWIZZLE2           0x0040069c  RW-4R  = ", REG_RD32(   NV_PGRAPH_BSWIZZLE2             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BSWIZZLE5           0x004006a0  RW-4R  = ", REG_RD32(   NV_PGRAPH_BSWIZZLE5             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPIXEL              0x00400724  RW-4R  = ", REG_RD32(   NV_PGRAPH_BPIXEL                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_LIMIT_VIOL_Z        0x00400614  RW-4R  = ", REG_RD32(   NV_PGRAPH_LIMIT_VIOL_Z          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_STATE               0x00400710  RW-4R  = ", REG_RD32(   NV_PGRAPH_STATE                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CACHE_INDEX         0x00400728  RW-4R  = ", REG_RD32(   NV_PGRAPH_CACHE_INDEX           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CACHE_RAM           0x0040072c  RW-4R  = ", REG_RD32(   NV_PGRAPH_CACHE_RAM             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_PITCH           0x00400760  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_PITCH             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DVD_COLORFMT        0x00400764  RW-4R  = ", REG_RD32(   NV_PGRAPH_DVD_COLORFMT          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_SCALED_FORMAT       0x00400768  RW-4R  = ", REG_RD32(   NV_PGRAPH_SCALED_FORMAT         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PATT_COLOR0         0x00400800  RW-4R  = ", REG_RD32(   NV_PGRAPH_PATT_COLOR0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PATT_COLOR1         0x00400804  RW-4R  = ", REG_RD32(   NV_PGRAPH_PATT_COLOR1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PATTERN_SHAPE       0x00400810  RW-4R  = ", REG_RD32(   NV_PGRAPH_PATTERN_SHAPE         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MONO_COLOR0         0x00400600  RW-4R  = ", REG_RD32(   NV_PGRAPH_MONO_COLOR0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ROP3                0x00400604  RW-4R  = ", REG_RD32(   NV_PGRAPH_ROP3                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CHROMA              0x00400814  RW-4R  = ", REG_RD32(   NV_PGRAPH_CHROMA                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BETA_AND            0x00400608  RW-4R  = ", REG_RD32(   NV_PGRAPH_BETA_AND              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BETA_PREMULT        0x0040060c  RW-4R  = ", REG_RD32(   NV_PGRAPH_BETA_PREMULT          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CONTROL0            0x00400818  RW-4R  = ", REG_RD32(   NV_PGRAPH_CONTROL0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CONTROL2            0x00400820  RW-4R  = ", REG_RD32(   NV_PGRAPH_CONTROL2              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLEND               0x00400824  RW-4R  = ", REG_RD32(   NV_PGRAPH_BLEND                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DPRAM_INDEX         0x00400828  RW-4R  = ", REG_RD32(   NV_PGRAPH_DPRAM_INDEX           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DPRAM_DATA          0x0040082c  RW-4R  = ", REG_RD32(   NV_PGRAPH_DPRAM_DATA            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_STORED_FMT          0x00400830  RW-4R  = ", REG_RD32(   NV_PGRAPH_STORED_FMT            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FORMATS             0x00400618  RW-4R  = ", REG_RD32(   NV_PGRAPH_FORMATS               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC0      0x00400514  RW-4R  = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC0        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC1      0x00400518  RW-4R  = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC1        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC2      0x0040051C  RW-4R  = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC2        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC3      0x00400520  RW-4R  = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC3        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_X_MISC              0x00400500  RW-4R  = ", REG_RD32(   NV_PGRAPH_X_MISC                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_Y_MISC              0x00400504  RW-4R  = ", REG_RD32(   NV_PGRAPH_Y_MISC                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_XMIN      0x0040053C  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_XMIN        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_XMAX      0x00400544  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_XMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_YMIN      0x00400540  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_YMIN        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_YMAX      0x00400548  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_YMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_XMIN     0x00400560  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_XMIN       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_XMAX     0x00400568  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_XMAX       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_YMIN     0x00400564  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_YMIN       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_YMAX     0x0040056C  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_YMAX       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_SOURCE_COLOR        0x0040050C  RW-4R  = ", REG_RD32(   NV_PGRAPH_SOURCE_COLOR          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_VALID1              0x00400508  RW-4R  = ", REG_RD32(   NV_PGRAPH_VALID1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_VALID2              0x00400578  RW-4R  = ", REG_RD32(   NV_PGRAPH_VALID2                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_ICLIP_XMAX      0x00400534  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_ICLIP_XMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_ICLIP_YMAX      0x00400538  RW-4R  = ", REG_RD32(   NV_PGRAPH_ABS_ICLIP_YMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPX_0             0x00400524  RW-4R  = ", REG_RD32(   NV_PGRAPH_CLIPX_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPX_1             0x00400528  RW-4R  = ", REG_RD32(   NV_PGRAPH_CLIPX_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPY_0             0x0040052c  RW-4R  = ", REG_RD32(   NV_PGRAPH_CLIPY_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPY_1             0x00400530  RW-4R  = ", REG_RD32(   NV_PGRAPH_CLIPY_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MISC24_0            0x00400510  RW-4R  = ", REG_RD32(   NV_PGRAPH_MISC24_0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MISC24_1            0x00400570  RW-4R  = ", REG_RD32(   NV_PGRAPH_MISC24_1              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MISC24_2            0x00400574  RW-4R  = ", REG_RD32(   NV_PGRAPH_MISC24_2              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PASSTHRU_0          0x0040057C  RW-4R  = ", REG_RD32(   NV_PGRAPH_PASSTHRU_0            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PASSTHRU_1          0x00400580  RW-4R  = ", REG_RD32(   NV_PGRAPH_PASSTHRU_1            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PASSTHRU_2          0x00400584  RW-4R  = ", REG_RD32(   NV_PGRAPH_PASSTHRU_2            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_XY              0x004005c0  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_XY                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_U0              0x004005c4  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_U0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_V0              0x004005c8  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_V0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_U1              0x004005cc  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_U1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_V1              0x004005d0  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_V1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_ZETA            0x004005d4  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_ZETA              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_RGB             0x004005d8  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_RGB               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_S               0x004005dc  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_S                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_M               0x004005e0  RW-4R  = ", REG_RD32(   NV_PGRAPH_D3D_M                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FORMAT0             0x004005A8  RW-4R  = ", REG_RD32(   NV_PGRAPH_FORMAT0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FORMAT1             0x004005AC  RW-4R  = ", REG_RD32(   NV_PGRAPH_FORMAT1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FILTER0             0x004005B0  RW-4R  = ", REG_RD32(   NV_PGRAPH_FILTER0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FILTER1             0x004005B4  RW-4R  = ", REG_RD32(   NV_PGRAPH_FILTER1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE0ALPHA       0x00400590  RW-4R  = ", REG_RD32(   NV_PGRAPH_COMBINE0ALPHA         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE0COLOR       0x00400594  RW-4R  = ", REG_RD32(   NV_PGRAPH_COMBINE0COLOR         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE1ALPHA       0x00400598  RW-4R  = ", REG_RD32(   NV_PGRAPH_COMBINE1ALPHA         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE1COLOR       0x0040059C  RW-4R  = ", REG_RD32(   NV_PGRAPH_COMBINE1COLOR         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_START_0         0x00401000  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_START_0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_START_1         0x00401004  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_START_1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_LENGTH          0x00401008  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_LENGTH            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_MISC            0x0040100C  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_MISC              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_DATA_0          0x00401020  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_DATA_0            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_DATA_1          0x00401024  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_DATA_1            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_RM              0x00401030  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_RM                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_XLATE_INST    0x00401040  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_XLATE_INST      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_CONTROL       0x00401044  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_CONTROL         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_LIMIT         0x00401048  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_LIMIT           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_TLB_PTE       0x0040104C  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_TLB_PTE         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_TLB_TAG       0x00401050  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_TLB_TAG         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_ADJ_OFFSET    0x00401054  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_ADJ_OFFSET      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_OFFSET        0x00401058  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_OFFSET          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_SIZE          0x0040105C  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_SIZE            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_Y_SIZE        0x00401060  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_A_Y_SIZE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_XLATE_INST    0x00401080  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_XLATE_INST      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_CONTROL       0x00401084  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_CONTROL         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_LIMIT         0x00401088  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_LIMIT           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_TLB_PTE       0x0040108C  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_TLB_PTE         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_TLB_TAG       0x00401090  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_TLB_TAG         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_ADJ_OFFSET    0x00401094  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_ADJ_OFFSET      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_OFFSET        0x00401098  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_OFFSET          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_SIZE          0x0040109C  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_SIZE            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_Y_SIZE        0x004010A0  RW-4R  = ", REG_RD32(   NV_PGRAPH_DMA_B_Y_SIZE          ));
}  

void NvWatch_NV04_Dump_Fifo(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nFifo State:\r\n");
  
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DELAY_0              0x00002040  RW-4R  = ", REG_RD32(   NV_PFIFO_DELAY_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DMA_TIMESLICE        0x00002044  RW-4R  = ", REG_RD32(   NV_PFIFO_DMA_TIMESLICE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_PIO_TIMESLICE        0x00002048  RW-4R  = ", REG_RD32(   NV_PFIFO_PIO_TIMESLICE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_TIMESLICE            0x0000204C  RW-4R  = ", REG_RD32(   NV_PFIFO_TIMESLICE              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_NEXT_CHANNEL         0x00002050  RW-4R  = ", REG_RD32(   NV_PFIFO_NEXT_CHANNEL           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DEBUG_0              0x00002080  R--4R  = ", REG_RD32(   NV_PFIFO_DEBUG_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_INTR_0               0x00002100  RW-4R  = ", REG_RD32(   NV_PFIFO_INTR_0                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_INTR_EN_0            0x00002140  RW-4R  = ", REG_RD32(   NV_PFIFO_INTR_EN_0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RAMHT                0x00002210  RW-4R  = ", REG_RD32(   NV_PFIFO_RAMHT                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RAMFC                0x00002214  RW-4R  = ", REG_RD32(   NV_PFIFO_RAMFC                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RAMRO                0x00002218  RW-4R  = ", REG_RD32(   NV_PFIFO_RAMRO                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHES               0x00002500  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHES                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_MODE                 0x00002504  RW-4R  = ", REG_RD32(   NV_PFIFO_MODE                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DMA                  0x00002508  RW-4R  = ", REG_RD32(   NV_PFIFO_DMA                    ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_SIZE                 0x0000250C  RW-4R  = ", REG_RD32(   NV_PFIFO_SIZE                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PUSH0         0x00003000  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_PUSH0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PUSH0         0x00003200  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_PUSH0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PUSH1         0x00003004  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_PUSH1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PUSH1         0x00003204  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_PUSH1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_PUSH      0x00003220  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_PUSH        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_FETCH     0x00003224  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_FETCH       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_PUT       0x00003240  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_PUT         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_GET       0x00003244  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_GET         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_STATE     0x00003228  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_STATE       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_INSTANCE  0x0000322C  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_INSTANCE    ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_CTL       0x00003230  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_CTL         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_LIMIT     0x00003234  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_LIMIT       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_TLB_TAG   0x00003238  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_TLB_TAG     ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_TLB_PTE   0x0000323C  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_TLB_PTE     ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PULL0         0x00003050  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_PULL0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PULL0         0x00003250  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_PULL0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PULL1         0x00003054  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_PULL1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PULL1         0x00003254  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_PULL1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_HASH          0x00003058  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_HASH            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_HASH          0x00003258  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_HASH            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_STATUS        0x00003014  R--4R  = ", REG_RD32(   NV_PFIFO_CACHE0_STATUS          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_STATUS        0x00003214  R--4R  = ", REG_RD32(   NV_PFIFO_CACHE1_STATUS          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_STATUS1       0x00003218  R--4R  = ", REG_RD32(   NV_PFIFO_CACHE1_STATUS1         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PUT           0x00003010  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_PUT             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PUT           0x00003210  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_PUT             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_GET           0x00003070  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_GET             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_GET           0x00003270  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_GET             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_ENGINE        0x00003080  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE0_ENGINE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_ENGINE        0x00003280  RW-4R  = ", REG_RD32(   NV_PFIFO_CACHE1_ENGINE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RUNOUT_STATUS        0x00002400  R--4R  = ", REG_RD32(   NV_PFIFO_RUNOUT_STATUS          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RUNOUT_PUT           0x00002410  RW-4R  = ", REG_RD32(   NV_PFIFO_RUNOUT_PUT             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RUNOUT_GET           0x00002420  RW-4R  = ", REG_RD32(   NV_PFIFO_RUNOUT_GET             ));
}

void NvWatch_NV04_Dump_Master(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nMaster Control State:\r\n");
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_BOOT_0                 0x00000000  R--4R  = ", REG_RD32(   NV_PMC_BOOT_0                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_INTR_0                 0x00000100  RW-4R  = ", REG_RD32(   NV_PMC_INTR_0                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_INTR_EN_0              0x00000140  RW-4R  = ", REG_RD32(   NV_PMC_INTR_EN_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_INTR_READ_0            0x00000160  R--4R  = ", REG_RD32(   NV_PMC_INTR_READ_0              ));
}

void NvWatch_NV04_Dump_Video(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nVideo State:\r\n");

  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_INTR_0              0x00680100  RWI4R  = ", REG_RD32(   NV_PVIDEO_INTR_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_INTR_EN_0           0x00680140  RWI4R  = ", REG_RD32(   NV_PVIDEO_INTR_EN_0             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_STEP_SIZE           0x00680200  RW-4R  = ", REG_RD32(   NV_PVIDEO_STEP_SIZE             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_CONTROL_Y           0x00680204  RW-4R  = ", REG_RD32(   NV_PVIDEO_CONTROL_Y             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_CONTROL_X           0x00680208  RW-4R  = ", REG_RD32(   NV_PVIDEO_CONTROL_X             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFF0_START         0x0068020c  RW-4R  = ", REG_RD32(   NV_PVIDEO_BUFF0_START           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFF1_START         0x00680210  RW-4R  = ", REG_RD32(   NV_PVIDEO_BUFF1_START           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFF0_PITCH         0x00680214  RW-4R  = ", REG_RD32(   NV_PVIDEO_BUFF0_PITCH           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFF1_PITCH         0x00680218  RW-4R  = ", REG_RD32(   NV_PVIDEO_BUFF1_PITCH           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFF0_OFFSET        0x0068021c  RW-4R  = ", REG_RD32(   NV_PVIDEO_BUFF0_OFFSET          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFF1_OFFSET        0x00680220  RW-4R  = ", REG_RD32(   NV_PVIDEO_BUFF1_OFFSET          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_OE_STATE            0x00680224  RW-4R  = ", REG_RD32(   NV_PVIDEO_OE_STATE              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_SU_STATE            0x00680228  RW-4R  = ", REG_RD32(   NV_PVIDEO_SU_STATE              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_RM_STATE            0x0068022c  RW-4R  = ", REG_RD32(   NV_PVIDEO_RM_STATE              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_WINDOW_START        0x00680230  RW-4R  = ", REG_RD32(   NV_PVIDEO_WINDOW_START          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_WINDOW_SIZE         0x00680234  RW-4R  = ", REG_RD32(   NV_PVIDEO_WINDOW_SIZE           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_FIFO_THRES          0x00680238  RW-4R  = ", REG_RD32(   NV_PVIDEO_FIFO_THRES            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_FIFO_BURST          0x0068023c  RW-4R  = ", REG_RD32(   NV_PVIDEO_FIFO_BURST            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_KEY                 0x00680240  RW-4R  = ", REG_RD32(   NV_PVIDEO_KEY                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_OVERLAY             0x00680244  RWI4R  = ", REG_RD32(   NV_PVIDEO_OVERLAY               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_RED_CSC             0x00680280  RW-4R  = ", REG_RD32(   NV_PVIDEO_RED_CSC               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_GREEN_CSC           0x00680284  RW-4R  = ", REG_RD32(   NV_PVIDEO_GREEN_CSC             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BLUE_CSC            0x00680288  RW-4R  = ", REG_RD32(   NV_PVIDEO_BLUE_CSC              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_CSC_ADJUST          0x0068028c  RW-4R  = ", REG_RD32(   NV_PVIDEO_CSC_ADJUST            ));
}

U032 NvWatch_NV04_GetHashSize(PHWINFO pDev)
{
  U032 return_val;
  
  return_val = REG_RD32(NV_PFIFO_RAMHT);

  return(return_val);
}
  
void NvWatch_NV04_FillDiagStruct(PHWINFO pDev, PNVWATCHDIAGSTRUCT pdiagstruct)
{
    pdiagstruct->nv_pfifo_intr_0       =  REG_RD32(NV_PFIFO_INTR_0      );
    pdiagstruct->nv_pfifo_intr_en_0    =  REG_RD32(NV_PFIFO_INTR_EN_0   );
    pdiagstruct->nv_pfifo_caches       =  REG_RD32(NV_PFIFO_CACHES      );
    pdiagstruct->nv_pfifo_mode         =  REG_RD32(NV_PFIFO_MODE        );
    pdiagstruct->nv_pfifo_cache1_push0 =  REG_RD32(NV_PFIFO_CACHE1_PUSH0);
    pdiagstruct->nv_pfifo_cache1_push1 =  REG_RD32(NV_PFIFO_CACHE1_PUSH1);
    pdiagstruct->nv_pfifo_cache1_pull0 =  REG_RD32(NV_PFIFO_CACHE1_PULL0);
    pdiagstruct->nv_pfifo_dma_put      =  REG_RD32(NV_PFIFO_CACHE1_PUT  );
    pdiagstruct->nv_pfifo_dma_get      =  REG_RD32(NV_PFIFO_CACHE1_GET  );
    pdiagstruct->nv_pmc_intr_0         =  REG_RD32(NV_PMC_INTR_0        );
    pdiagstruct->nv_pmc_intr_en_0      =  REG_RD32(NV_PMC_INTR_EN_0     );
    pdiagstruct->nv_pmc_intr_read_0    =  REG_RD32(NV_PMC_INTR_READ_0   );
    pdiagstruct->nv_pmc_enable         =  REG_RD32(NV_PMC_ENABLE        );

}

void NvWatch_NV04_DumpGrTableContents(PHWINFO pDev, U032 verbose_flag)
{
    PHALHWINFO pHalHwInfo = &pDev->halHwInfo;
    PGRHALINFO_NV04 pGrHalInfo;
    PFIFOHALINFO_NV04 pFifoHalInfo;
    U032 ChID;
    U032 i;

    pHalHwInfo = &pDev->halHwInfo;
    pGrHalInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalInfo;
    pFifoHalInfo = (PFIFOHALINFO_NV04)pHalHwInfo->pFifoHalInfo;

    for(ChID=0;ChID<NUM_FIFOS;ChID++){
        if (pFifoHalInfo->InUse & (1 << ChID))
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "\ngrTable dump of channel ", ChID);

        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Context Switch1 = ", pGrHalInfo->grChannels[ChID].ContextSwitch1);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Context Switch2 = ", pGrHalInfo->grChannels[ChID].ContextSwitch2);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Context Switch3 = ", pGrHalInfo->grChannels[ChID].ContextSwitch3);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Context Switch4 = ", pGrHalInfo->grChannels[ChID].ContextSwitch4);
        DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  Context Switch5 = ", pGrHalInfo->grChannels[ChID].ContextSwitch5);
      
        for (i = 0; i < 8; i++)
        {    
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    Cached Context data for subchannel ", i);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      ContextCache0 = ", pGrHalInfo->grChannels[ChID].ContextCache1[i]);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      ContextCache1 = ", pGrHalInfo->grChannels[ChID].ContextCache2[i]);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      ContextCache2 = ", pGrHalInfo->grChannels[ChID].ContextCache3[i]);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      ContextCache3 = ", pGrHalInfo->grChannels[ChID].ContextCache4[i]);
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "      ContextCache4 = ", pGrHalInfo->grChannels[ChID].ContextCache5[i]);
        }

        if(verbose_flag == NVWATCH_COMPLEX){
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].ContextUser = ",          pGrHalInfo->grChannels[ChID].ContextUser       );                 

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].DmaStart0 = ",        pGrHalInfo->grChannels[ChID].DmaStart0             );             
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].DmaStart1 = ",        pGrHalInfo->grChannels[ChID].DmaStart1             );             
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].DmaLength = ",        pGrHalInfo->grChannels[ChID].DmaLength             );             
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].DmaMisc   = ",        pGrHalInfo->grChannels[ChID].DmaMisc               );             
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].DmaPitch  = ",        pGrHalInfo->grChannels[ChID].DmaPitch              );             

            for (i = 0; i < 6; i++)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferOffset[i]  = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.BufferOffset[i]   );
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferBase[i]    = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.BufferBase[i]     );
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferLimit[i]   = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.BufferLimit[i]    );
            }
            for (i = 0; i < 5; i++)
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferPitch[i] = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.BufferPitch[i]      );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Surface           = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.Surface               );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.State             = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.State                 );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferSwizzle[0]  = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.BufferSwizzle[0]      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferSwizzle[1]  = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.BufferSwizzle[1]      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BufferPixel       = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.BufferPixel           );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Notify            = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.Notify                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.PatternColor0     = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.PatternColor0         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.PatternColor1     = ",            pGrHalInfo->grChannels[ChID].CurrentPatch.PatternColor1         );

            for (i = 0; i < 64; i++)
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.PatternColorRam[i]  = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.PatternColorRam[i]);

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Pattern[0]         = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Pattern[0]            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Pattern[1]         = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Pattern[1]            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.PatternShape       = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.PatternShape          );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.MonoColor0         = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.MonoColor0            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Rop3               = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Rop3                  );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Chroma             = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Chroma                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BetaAnd            = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.BetaAnd               );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BetaPremult        = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.BetaPremult           );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Control0           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Control0              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Control1           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Control1              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Control2           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Control2              );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Blend              = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Blend                 );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.StoredFmt            = ",         pGrHalInfo->grChannels[ChID].CurrentPatch.StoredFmt             );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.SourceColor          = ",         pGrHalInfo->grChannels[ChID].CurrentPatch.SourceColor           );

            for (i = 0; i < 32; i++)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsXRam[i]       = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.AbsXRam[i]        );
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsYRam[i]       = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.AbsYRam[i]        );
            }

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipXMin        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipXMin          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipXMax        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipXMax          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipYMin        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipYMin          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipYMax        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipYMax          );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAXMin       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAXMin         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAXMax       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAXMax         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAYMin       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAYMin         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAYMax       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClipAYMax         );
                                                                                                                                                              
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsIClipXMax        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsIClipXMax          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsIClipYMax        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsIClipYMax          );
                                                                                                                                                              
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc0        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc0          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc1        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc1          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc2        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc2          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc3        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.XYLogicMisc3          );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ClipX0              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ClipX0                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ClipX1              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ClipX1                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ClipY0              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ClipY0                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ClipY1              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ClipY1                );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0Alpha       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0Alpha      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1Alpha       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1Alpha      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0Color       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0Color      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1Color       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1Color      );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Format0             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Format0               );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Format1             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Format1               );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Filter0             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Filter0               );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Filter1             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Filter1               );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_XY              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_XY                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_U0              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_U0                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_V0              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_V0                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_U1              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_U1                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_V1              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_V1                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_ZETA            = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_ZETA              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_RGB             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_RGB               );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_S               = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_S                 );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_M               = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.D3D_M                 );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru0           = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru0             );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru1           = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru1             );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru2           = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru2             );


            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.DVDColorFormat      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.DVDColorFormat        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ScaledFormat        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ScaledFormat          );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Misc24_0            = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Misc24_0              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Misc24_1            = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Misc24_1              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Misc24_2            = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Misc24_2              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XMisc               = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.XMisc                 );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.YMisc               = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.YMisc                 );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Valid1              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Valid1                );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Valid2              = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Valid2                );
        }
    }
}
#endif

