
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
// (C) Copyright SGS-THOMSON Microelectronics Inc., 1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NV10wtch.C                                                        *
*   Winice .N Debug Interface. NV10 code                                    *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Vito 2/6/98
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"        // put this first. we redefine macro SF_BIT in nvrm.h
#include <nv_ref.h>
#include <nv10_ref.h>
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
#include "nv10_hal.h"

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
#define CONTEXT_USE_PUT     0
#define CONTEXT_USE_GET     1

//****NV CONTEXT MENU******************************************************************************

void NvWatch_NV10_Dump_SubChannelContext(PHWINFO pDev, U032 p_channel)
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
          if(!NvWatch_GetObjectFromInst(pDev, tempv, &DHandle)){
              DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nERROR: Object NOT FOUND !\n");
          }else{  
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

void NvWatch_NV10_ContextInfo_help(PHWINFO pDev)
{

/////  NvWatch_channel = REG_RD32(NV_PGRAPH_CTX_USER);
/////  NvWatch_channel >>= 24;
/////  NvWatch_channel &= NUM_FIFOS-1; 
  
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCommands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      s   - Query SubChannel Contexts (Currently supports active channel ONLY\n\r");
//  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "            Active Channel = ",NvWatch_channel);
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      ?   - ???\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      q   - quit this\n\r\n\r");
}

VOID NvWatch_NV10_Query_DeviceContext(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  U032 m_channel=0;
  char pstring[]="NvWatch Context";

  NvWatch_NV10_ContextInfo_help(pDev);
  
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
          NvWatch_NV10_Dump_SubChannelContext(pDev, m_channel);
          // Restore the NV_PGRAPH_FIFO.
          REG_WR32(NV_PGRAPH_FIFO, save_pgraph_fifo);
        }  
        break;
      case 'c': // Dump hash table
      case 'C': // Dump hash table
        break;
      default: // Help
        NvWatch_NV10_ContextInfo_help(pDev);
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
void NvWatch_NV10_DumpFifoContents(PHWINFO pDev)
{

  U032 save_puller, save_pusher;
  U032 nvw_fifo_get,nvw_fifo_put;
  U032  fifo_count;
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
    
    if(( ( (nvw_fifo_get%(128*4)) /4) == fifo_count)&&(( (nvw_fifo_put%(128*4)) /4) == fifo_count)){
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

// Specify a device, channel and some format info.
void NvWatch_NV10_DumpPushBuffer(PHWINFO pDev, U032 p_channel, U032 ref_ptr, U032 bytes_before_ptr, U032 bytes_after_ptr)
{
    U032 page_ref, ref_pte, ref_offset;
    U032 dmacontext_inst, *dmacontext_inst_ptr;
    U032 *instance_fc_ptr;
    U032 *pte_ptr;

    // if this is the p_current channel.
    if(p_channel == (REG_RD32(NV_PFIFO_CACHE1_PUSH1) & 0x1F)){
      if(ref_ptr == CONTEXT_USE_PUT)
        ref_ptr = REG_RD32(NV_PFIFO_CACHE1_PUT);
      else
        ref_ptr = REG_RD32(NV_PFIFO_CACHE1_GET);

      dmacontext_inst = REG_RD32(NV_PFIFO_CACHE1_DMA_INSTANCE);
    }else{
      // Get the DMA Context for the channels pusher. (Just grab it from the FC_Context in instance memory.
      instance_fc_ptr = (U032 *) (INSTANCE_MEM_SPACE/*nvAddr*/ + ((pDev->Pram.HalInfo.PraminOffset+0x10000)/4) + 0x4000);

      // Point to the correct context.
      instance_fc_ptr += p_channel*8;   // 8 dwords per context.
      // Get the 3rd DWORD from the fifo contexts.
      dmacontext_inst = *(instance_fc_ptr+2);
      
      if(ref_ptr == CONTEXT_USE_PUT)
        ref_ptr = (U032) *(instance_fc_ptr);
      else
        ref_ptr = (U032) *(instance_fc_ptr+1);
      
    }
    // Get a pointer to the ContextDma for the push buffer.
    dmacontext_inst &= 0xFFFF;
    dmacontext_inst <<=4;
    dmacontext_inst += (U032) INSTANCE_MEM_SPACE/*nvAddr*/ + (U032) pDev->Pram.HalInfo.PraminOffset;
    dmacontext_inst_ptr = (U032 *) dmacontext_inst;

    // Get the Offset and the 4KB aligned PTE.
    ref_offset = ref_ptr & 0xfff;
    page_ref = ref_ptr >> 12;

    // Get the PTE from the instance data. Map it and add the offset to get the linear ptr.
    ref_pte = *(dmacontext_inst_ptr + 2 + page_ref);


    pte_ptr = (U032 *) vmmMapPhysToLinear(ref_pte, 0x02000, 0);
    pte_ptr += (ref_offset/4);

    ///// // The pointers should now be set up to fetch the data. 
    ///// DBG_PRINT_STRING(DEBUGLEVEL_ERRORS," DmaPush Buffer Dump for channel ");
    ///// DBG_PRINT_VALUE(DEBUGLEVEL_ERRORS, REG_RD32(fifo_count);
}

/////void NvWatch_NV10_DmaPushAndFifo_help(PHWINFO pDev)
/////{
/////  U032 x;
/////
/////
/////  if(REG_RD32(NV_PFIFO_CACHE1_PUSH1) & (NV_PFIFO_CACHE1_PUSH1_MODE_DMA<<8)){
/////      DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  The current channel is PUSH Channel # ",((REG_RD32(NV_PFIFO_CACHE1_PUSH1))&0x1F) );
/////  }else{
/////      DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "  The current channel is PIO Channel # ", ((REG_RD32(NV_PFIFO_CACHE1_PUSH1))&0x1F) );
/////  }
/////
/////  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCommands as follows:\n\r");
/////  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      f   - Dump Fifo Contents\n\r");
/////  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       " ALL ITEMS BELOW ARE CURRENTLY NOT SUPPORTED\n\r");
/////  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      d   - Dump DmaPusher for Current Channel\n\r\n\r");
/////
/////  // Get the number of dmapush channels.
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      0   - Dump DmaPusher for Push Channel 0\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<1))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      1   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<2))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      2   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<3))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      3   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<4))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      4   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<5))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      5   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<6))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      6   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<7))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      7   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<8))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      8   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<9))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      9   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<10))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      a   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<11))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      b   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<12))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      c   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<13))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      d   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<14))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      e   - Dump DmaPusher for Push Channel 1\n\r");
/////  if(REG_RD32(NV_PFIFO_MODE) & (NV_PFIFO_MODE_CHANNEL_0_DMA<<15))
/////    DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      f   - Dump DmaPusher for Push Channel 1\n\r");
/////  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit this\n\r\n\r");
/////}

void NvWatch_NV10_DumpUserDmaBuffersAtGet(PHWINFO pDev)
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

void NvWatch_NV10_DmaPushAndFifo_help(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\nCommands as follows:\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      f   - Dump Fifo Contents\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      c   - Get Physical address of DmaBuffers at Current Get ptr.(DmaPush channels only)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,       "      d   - Dump DmaPusher Info (Not Implemented)\n\r");
  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "      q   - quit this\n\r\n\r");
}

int NvWatch_NV10_DmaPushAndFifo(PHWINFO pDev)
{
  BOOL keep_running=TRUE;
  char in_char;
  // Go with 6 dwords pre and post.
  U032 bytes_before_ptr = 48;
  U032 bytes_after_ptr = 48;
  char pstring[]="NvWatch Fifo";
 
  NvWatch_NV10_DmaPushAndFifo_help(pDev);

  while(keep_running)
  {
    NvWatch_display_prompt(pDev, pstring);
  
    in_char = get_winice_input();
    switch(in_char){
      case 'q':
      case 'Q':
        keep_running = FALSE;
        break;
      case 'F': // Dump fifo 
      case 'f': // Dump fifo 
        NvWatch_NV10_DumpFifoContents(pDev);
        break;
      case 'd':
        //p_channel = REG_RD32(NV_PFIFO_CACHE1_PUSH1) & 0x1F;
        //ref_ptr = CONTEXT_USE_GET;
        //NvWatch_NV10_DumpPushBuffer(device, p_channel, ref_ptr, bytes_before_ptr, bytes_after_ptr)   
        break;
      case 'c': 
      case 'C': 
        NvWatch_NV10_DumpUserDmaBuffersAtGet(pDev);
        break;
      default: // Help
        NvWatch_NV10_DmaPushAndFifo_help(pDev);
        break;  
    }
  }

  return (RM_OK);
}

void NvWatch_NV10_Dump_Graphics(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nGraphics Engine State:\r\n");
 
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_0             = ", REG_RD32(   NV_PGRAPH_DEBUG_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_1             = ", REG_RD32(   NV_PGRAPH_DEBUG_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_2             = ", REG_RD32(   NV_PGRAPH_DEBUG_2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DEBUG_3             = ", REG_RD32(   NV_PGRAPH_DEBUG_3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_INTR                = ", REG_RD32(   NV_PGRAPH_INTR                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_NSOURCE             = ", REG_RD32(   NV_PGRAPH_NSOURCE               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_INTR_EN             = ", REG_RD32(   NV_PGRAPH_INTR_EN               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_SWITCH1         = ", REG_RD32(   NV_PGRAPH_CTX_SWITCH1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_SWITCH2         = ", REG_RD32(   NV_PGRAPH_CTX_SWITCH2           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_SWITCH3         = ", REG_RD32(   NV_PGRAPH_CTX_SWITCH3           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_CONTROL         = ", REG_RD32(   NV_PGRAPH_CTX_CONTROL           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CTX_USER            = ", REG_RD32(   NV_PGRAPH_CTX_USER              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FIFO                = ", REG_RD32(   NV_PGRAPH_FIFO                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FFINTFC_ST2         = ", REG_RD32(   NV_PGRAPH_FFINTFC_ST2           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_STATUS              = ", REG_RD32(   NV_PGRAPH_STATUS                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_TRAPPED_ADDR        = ", REG_RD32(   NV_PGRAPH_TRAPPED_ADDR          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_TRAPPED_DATA        = ", REG_RD32(   NV_PGRAPH_TRAPPED_DATA_LOW      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_SURFACE             = ", REG_RD32(   NV_PGRAPH_SURFACE               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_NOTIFY              = ", REG_RD32(   NV_PGRAPH_NOTIFY                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET0            = ", REG_RD32(   NV_PGRAPH_BOFFSET0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET1            = ", REG_RD32(   NV_PGRAPH_BOFFSET1              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET2            = ", REG_RD32(   NV_PGRAPH_BOFFSET2              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET3            = ", REG_RD32(   NV_PGRAPH_BOFFSET3              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET4            = ", REG_RD32(   NV_PGRAPH_BOFFSET4              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BOFFSET5            = ", REG_RD32(   NV_PGRAPH_BOFFSET5              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE0              = ", REG_RD32(   NV_PGRAPH_BBASE0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE1              = ", REG_RD32(   NV_PGRAPH_BBASE1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE2              = ", REG_RD32(   NV_PGRAPH_BBASE2                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE3              = ", REG_RD32(   NV_PGRAPH_BBASE3                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE4              = ", REG_RD32(   NV_PGRAPH_BBASE4                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BBASE5              = ", REG_RD32(   NV_PGRAPH_BBASE5                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH0             = ", REG_RD32(   NV_PGRAPH_BPITCH0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH1             = ", REG_RD32(   NV_PGRAPH_BPITCH1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH2             = ", REG_RD32(   NV_PGRAPH_BPITCH2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH3             = ", REG_RD32(   NV_PGRAPH_BPITCH3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPITCH4             = ", REG_RD32(   NV_PGRAPH_BPITCH4               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT0             = ", REG_RD32(   NV_PGRAPH_BLIMIT0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT1             = ", REG_RD32(   NV_PGRAPH_BLIMIT1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT2             = ", REG_RD32(   NV_PGRAPH_BLIMIT2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT3             = ", REG_RD32(   NV_PGRAPH_BLIMIT3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT4             = ", REG_RD32(   NV_PGRAPH_BLIMIT4               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLIMIT5             = ", REG_RD32(   NV_PGRAPH_BLIMIT5               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BSWIZZLE2           = ", REG_RD32(   NV_PGRAPH_BSWIZZLE2             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BSWIZZLE5           = ", REG_RD32(   NV_PGRAPH_BSWIZZLE5             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BPIXEL              = ", REG_RD32(   NV_PGRAPH_BPIXEL                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_LIMIT_VIOL_Z        = ", REG_RD32(   NV_PGRAPH_LIMIT_VIOL_Z          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_STATE               = ", REG_RD32(   NV_PGRAPH_STATE                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CACHE_INDEX         = ", REG_RD32(   NV_PGRAPH_CACHE_INDEX           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CACHE_RAM           = ", REG_RD32(   NV_PGRAPH_CACHE_RAM             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_PITCH           = ", REG_RD32(   NV_PGRAPH_DMA_PITCH             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DVD_COLORFMT        = ", REG_RD32(   NV_PGRAPH_DVD_COLORFMT          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_SCALED_FORMAT       = ", REG_RD32(   NV_PGRAPH_SCALED_FORMAT         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PATT_COLOR0         = ", REG_RD32(   NV_PGRAPH_PATT_COLOR0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PATT_COLOR1         = ", REG_RD32(   NV_PGRAPH_PATT_COLOR1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PATTERN_SHAPE       = ", REG_RD32(   NV_PGRAPH_PATTERN_SHAPE         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MONO_COLOR0         = ", REG_RD32(   NV_PGRAPH_MONO_COLOR0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ROP3                = ", REG_RD32(   NV_PGRAPH_ROP3                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CHROMA              = ", REG_RD32(   NV_PGRAPH_CHROMA                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BETA_AND            = ", REG_RD32(   NV_PGRAPH_BETA_AND              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BETA_PREMULT        = ", REG_RD32(   NV_PGRAPH_BETA_PREMULT          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CONTROL0            = ", REG_RD32(   NV_PGRAPH_CONTROL0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CONTROL2            = ", REG_RD32(   NV_PGRAPH_CONTROL2              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_BLEND               = ", REG_RD32(   NV_PGRAPH_BLEND                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DPRAM_INDEX         = ", REG_RD32(   NV_PGRAPH_DPRAM_INDEX           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DPRAM_DATA          = ", REG_RD32(   NV_PGRAPH_DPRAM_DATA            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_STORED_FMT          = ", REG_RD32(   NV_PGRAPH_STORED_FMT            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FORMATS             = ", REG_RD32(   NV_PGRAPH_FORMATS               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC0      = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC0        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC1      = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC1        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC2      = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC2        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_XY_LOGIC_MISC3      = ", REG_RD32(   NV_PGRAPH_XY_LOGIC_MISC3        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_X_MISC              = ", REG_RD32(   NV_PGRAPH_X_MISC                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_Y_MISC              = ", REG_RD32(   NV_PGRAPH_Y_MISC                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_XMIN      = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_XMIN        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_XMAX      = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_XMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_YMIN      = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_YMIN        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIP_YMAX      = ", REG_RD32(   NV_PGRAPH_ABS_UCLIP_YMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_XMIN     = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_XMIN       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_XMAX     = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_XMAX       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_YMIN     = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_YMIN       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_UCLIPA_YMAX     = ", REG_RD32(   NV_PGRAPH_ABS_UCLIPA_YMAX       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_SOURCE_COLOR        = ", REG_RD32(   NV_PGRAPH_SOURCE_COLOR          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_VALID1              = ", REG_RD32(   NV_PGRAPH_VALID1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_VALID2              = ", REG_RD32(   NV_PGRAPH_VALID2                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_ICLIP_XMAX      = ", REG_RD32(   NV_PGRAPH_ABS_ICLIP_XMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_ABS_ICLIP_YMAX      = ", REG_RD32(   NV_PGRAPH_ABS_ICLIP_YMAX        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPX_0             = ", REG_RD32(   NV_PGRAPH_CLIPX_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPX_1             = ", REG_RD32(   NV_PGRAPH_CLIPX_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPY_0             = ", REG_RD32(   NV_PGRAPH_CLIPY_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_CLIPY_1             = ", REG_RD32(   NV_PGRAPH_CLIPY_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MISC24_0            = ", REG_RD32(   NV_PGRAPH_MISC24_0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MISC24_1            = ", REG_RD32(   NV_PGRAPH_MISC24_1              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_MISC24_2            = ", REG_RD32(   NV_PGRAPH_MISC24_2              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PASSTHRU_0          = ", REG_RD32(   NV_PGRAPH_PASSTHRU_0            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PASSTHRU_1          = ", REG_RD32(   NV_PGRAPH_PASSTHRU_1            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_PASSTHRU_2          = ", REG_RD32(   NV_PGRAPH_PASSTHRU_2            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_XY              = ", REG_RD32(   NV_PGRAPH_D3D_XY                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_U0              = ", REG_RD32(   NV_PGRAPH_D3D_U0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_V0              = ", REG_RD32(   NV_PGRAPH_D3D_V0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_U1              = ", REG_RD32(   NV_PGRAPH_D3D_U1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_V1              = ", REG_RD32(   NV_PGRAPH_D3D_V1                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_ZETA            = ", REG_RD32(   NV_PGRAPH_D3D_ZETA              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_RGB             = ", REG_RD32(   NV_PGRAPH_D3D_RGB               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_S               = ", REG_RD32(   NV_PGRAPH_D3D_S                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_D3D_M               = ", REG_RD32(   NV_PGRAPH_D3D_M                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FORMAT0             = ", REG_RD32(   NV_PGRAPH_FORMAT0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FORMAT1             = ", REG_RD32(   NV_PGRAPH_FORMAT1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FILTER0             = ", REG_RD32(   NV_PGRAPH_FILTER0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_FILTER1             = ", REG_RD32(   NV_PGRAPH_FILTER1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE0ALPHA       = ", REG_RD32(   NV_PGRAPH_COMBINE0ALPHA         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE0COLOR       = ", REG_RD32(   NV_PGRAPH_COMBINE0COLOR         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE1ALPHA       = ", REG_RD32(   NV_PGRAPH_COMBINE1ALPHA         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_COMBINE1COLOR       = ", REG_RD32(   NV_PGRAPH_COMBINE1COLOR         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_START_0         = ", REG_RD32(   NV_PGRAPH_DMA_START_0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_START_1         = ", REG_RD32(   NV_PGRAPH_DMA_START_1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_LENGTH          = ", REG_RD32(   NV_PGRAPH_DMA_LENGTH            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_MISC            = ", REG_RD32(   NV_PGRAPH_DMA_MISC              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_DATA_0          = ", REG_RD32(   NV_PGRAPH_DMA_DATA_0            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_DATA_1          = ", REG_RD32(   NV_PGRAPH_DMA_DATA_1            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_RM              = ", REG_RD32(   NV_PGRAPH_DMA_RM                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_XLATE_INST    = ", REG_RD32(   NV_PGRAPH_DMA_A_XLATE_INST      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_CONTROL       = ", REG_RD32(   NV_PGRAPH_DMA_A_CONTROL         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_LIMIT         = ", REG_RD32(   NV_PGRAPH_DMA_A_LIMIT           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_TLB_PTE       = ", REG_RD32(   NV_PGRAPH_DMA_A_TLB_PTE         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_TLB_TAG       = ", REG_RD32(   NV_PGRAPH_DMA_A_TLB_TAG         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_ADJ_OFFSET    = ", REG_RD32(   NV_PGRAPH_DMA_A_ADJ_OFFSET      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_OFFSET        = ", REG_RD32(   NV_PGRAPH_DMA_A_OFFSET          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_SIZE          = ", REG_RD32(   NV_PGRAPH_DMA_A_SIZE            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_A_Y_SIZE        = ", REG_RD32(   NV_PGRAPH_DMA_A_Y_SIZE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_XLATE_INST    = ", REG_RD32(   NV_PGRAPH_DMA_B_XLATE_INST      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_CONTROL       = ", REG_RD32(   NV_PGRAPH_DMA_B_CONTROL         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_LIMIT         = ", REG_RD32(   NV_PGRAPH_DMA_B_LIMIT           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_TLB_PTE       = ", REG_RD32(   NV_PGRAPH_DMA_B_TLB_PTE         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_TLB_TAG       = ", REG_RD32(   NV_PGRAPH_DMA_B_TLB_TAG         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_ADJ_OFFSET    = ", REG_RD32(   NV_PGRAPH_DMA_B_ADJ_OFFSET      ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_OFFSET        = ", REG_RD32(   NV_PGRAPH_DMA_B_OFFSET          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_SIZE          = ", REG_RD32(   NV_PGRAPH_DMA_B_SIZE            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PGRAPH_DMA_B_Y_SIZE        = ", REG_RD32(   NV_PGRAPH_DMA_B_Y_SIZE          ));
}  

void NvWatch_NV10_Dump_Fifo(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nFifo State:\r\n");
  
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DELAY_0              = ", REG_RD32(   NV_PFIFO_DELAY_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DMA_TIMESLICE        = ", REG_RD32(   NV_PFIFO_DMA_TIMESLICE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_PIO_TIMESLICE        = ", REG_RD32(   NV_PFIFO_PIO_TIMESLICE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_TIMESLICE            = ", REG_RD32(   NV_PFIFO_TIMESLICE              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_NEXT_CHANNEL         = ", REG_RD32(   NV_PFIFO_NEXT_CHANNEL           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DEBUG_0              = ", REG_RD32(   NV_PFIFO_DEBUG_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_INTR_0               = ", REG_RD32(   NV_PFIFO_INTR_0                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_INTR_EN_0            = ", REG_RD32(   NV_PFIFO_INTR_EN_0              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RAMHT                = ", REG_RD32(   NV_PFIFO_RAMHT                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RAMFC                = ", REG_RD32(   NV_PFIFO_RAMFC                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RAMRO                = ", REG_RD32(   NV_PFIFO_RAMRO                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHES               = ", REG_RD32(   NV_PFIFO_CACHES                 ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_MODE                 = ", REG_RD32(   NV_PFIFO_MODE                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_DMA                  = ", REG_RD32(   NV_PFIFO_DMA                    ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_SIZE                 = ", REG_RD32(   NV_PFIFO_SIZE                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PUSH0         = ", REG_RD32(   NV_PFIFO_CACHE0_PUSH0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PUSH0         = ", REG_RD32(   NV_PFIFO_CACHE1_PUSH0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PUSH1         = ", REG_RD32(   NV_PFIFO_CACHE0_PUSH1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PUSH1         = ", REG_RD32(   NV_PFIFO_CACHE1_PUSH1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_PUSH      = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_PUSH        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_FETCH     = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_FETCH       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_PUT       = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_PUT         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_GET       = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_GET         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_STATE     = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_STATE       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_INSTANCE  = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_INSTANCE    ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_CTL       = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_CTL         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_LIMIT     = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_LIMIT       ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_TLB_TAG   = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_TLB_TAG     ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_DMA_TLB_PTE   = ", REG_RD32(   NV_PFIFO_CACHE1_DMA_TLB_PTE     ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PULL0         = ", REG_RD32(   NV_PFIFO_CACHE0_PULL0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PULL0         = ", REG_RD32(   NV_PFIFO_CACHE1_PULL0           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PULL1         = ", REG_RD32(   NV_PFIFO_CACHE0_PULL1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PULL1         = ", REG_RD32(   NV_PFIFO_CACHE1_PULL1           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_HASH          = ", REG_RD32(   NV_PFIFO_CACHE0_HASH            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_HASH          = ", REG_RD32(   NV_PFIFO_CACHE1_HASH            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_STATUS        = ", REG_RD32(   NV_PFIFO_CACHE0_STATUS          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_STATUS        = ", REG_RD32(   NV_PFIFO_CACHE1_STATUS          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_STATUS1       = ", REG_RD32(   NV_PFIFO_CACHE1_STATUS1         ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_PUT           = ", REG_RD32(   NV_PFIFO_CACHE0_PUT             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_PUT           = ", REG_RD32(   NV_PFIFO_CACHE1_PUT             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_GET           = ", REG_RD32(   NV_PFIFO_CACHE0_GET             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_GET           = ", REG_RD32(   NV_PFIFO_CACHE1_GET             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE0_ENGINE        = ", REG_RD32(   NV_PFIFO_CACHE0_ENGINE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_CACHE1_ENGINE        = ", REG_RD32(   NV_PFIFO_CACHE1_ENGINE          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RUNOUT_STATUS        = ", REG_RD32(   NV_PFIFO_RUNOUT_STATUS          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RUNOUT_PUT           = ", REG_RD32(   NV_PFIFO_RUNOUT_PUT             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PFIFO_RUNOUT_GET           = ", REG_RD32(   NV_PFIFO_RUNOUT_GET             ));
}

void NvWatch_NV10_Dump_Master(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nMaster Control State:\r\n");
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_BOOT_0                 = ", REG_RD32(   NV_PMC_BOOT_0                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_INTR_0                 = ", REG_RD32(   NV_PMC_INTR_0                   ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_INTR_EN_0              = ", REG_RD32(   NV_PMC_INTR_EN_0                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PMC_INTR_READ_0            = ", REG_RD32(   NV_PMC_INTR_READ_0              ));
}

void NvWatch_NV10_Dump_Video(PHWINFO pDev)
{

  DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "\r\nVideo State:\r\n");

  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_0               = ", REG_RD32(NV_PVIDEO_DEBUG_0               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_1               = ", REG_RD32(NV_PVIDEO_DEBUG_1               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_2               = ", REG_RD32(NV_PVIDEO_DEBUG_2               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_3               = ", REG_RD32(NV_PVIDEO_DEBUG_3               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_4               = ", REG_RD32(NV_PVIDEO_DEBUG_4               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_5               = ", REG_RD32(NV_PVIDEO_DEBUG_5               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_6               = ", REG_RD32(NV_PVIDEO_DEBUG_6               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_7               = ", REG_RD32(NV_PVIDEO_DEBUG_7               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_8               = ", REG_RD32(NV_PVIDEO_DEBUG_8               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DEBUG_9               = ", REG_RD32(NV_PVIDEO_DEBUG_9               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_INTR                  = ", REG_RD32(NV_PVIDEO_INTR                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_INTR_EN               = ", REG_RD32(NV_PVIDEO_INTR_EN               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BUFFER                = ", REG_RD32(NV_PVIDEO_BUFFER                ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_STOP                  = ", REG_RD32(NV_PVIDEO_STOP                  ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BASE(0)               = ", REG_RD32(NV_PVIDEO_BASE(0)               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_BASE(1)               = ", REG_RD32(NV_PVIDEO_BASE(1)               ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_LIMIT(0)              = ", REG_RD32(NV_PVIDEO_LIMIT(0)              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_LIMIT(1)              = ", REG_RD32(NV_PVIDEO_LIMIT(1)              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_LUMINANCE(0)          = ", REG_RD32(NV_PVIDEO_LUMINANCE(0)          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_LUMINANCE(1)          = ", REG_RD32(NV_PVIDEO_LUMINANCE(1)          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_CHROMINANCE(0)        = ", REG_RD32(NV_PVIDEO_CHROMINANCE(0)        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_CHROMINANCE(1)        = ", REG_RD32(NV_PVIDEO_CHROMINANCE(1)        ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_OFFSET(0)             = ", REG_RD32(NV_PVIDEO_OFFSET(0)             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_OFFSET(1)             = ", REG_RD32(NV_PVIDEO_OFFSET(1)             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_SIZE_IN(0)            = ", REG_RD32(NV_PVIDEO_SIZE_IN(0)            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_SIZE_IN(1)            = ", REG_RD32(NV_PVIDEO_SIZE_IN(1)            ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_POINT_IN(0)           = ", REG_RD32(NV_PVIDEO_POINT_IN(0)           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_POINT_IN(1)           = ", REG_RD32(NV_PVIDEO_POINT_IN(1)           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DS_DX(0)              = ", REG_RD32(NV_PVIDEO_DS_DX(0)              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DS_DX(1)              = ", REG_RD32(NV_PVIDEO_DS_DX(1)              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DT_DY(0)              = ", REG_RD32(NV_PVIDEO_DT_DY(0)              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_DT_DY(1)              = ", REG_RD32(NV_PVIDEO_DT_DY(1)              ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_POINT_OUT(0)          = ", REG_RD32(NV_PVIDEO_POINT_OUT(0)          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_POINT_OUT(1)          = ", REG_RD32(NV_PVIDEO_POINT_OUT(1)          ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_SIZE_OUT(0)           = ", REG_RD32(NV_PVIDEO_SIZE_OUT(0)           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_SIZE_OUT(1)           = ", REG_RD32(NV_PVIDEO_SIZE_OUT(1)           ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_FORMAT(0)             = ", REG_RD32(NV_PVIDEO_FORMAT(0)             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_FORMAT(1)             = ", REG_RD32(NV_PVIDEO_FORMAT(1)             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_COLOR_KEY             = ", REG_RD32(NV_PVIDEO_COLOR_KEY             ));
  DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NV_PVIDEO_TEST                  = ", REG_RD32(NV_PVIDEO_TEST                  ));
}

U032 NvWatch_NV10_GetHashSize(PHWINFO pDev)
{
  U032 return_val;
  
  return_val = REG_RD32(NV_PFIFO_RAMHT);

  return(return_val);
}

void NvWatch_NV10_FillDiagStruct(PHWINFO pDev, PNVWATCHDIAGSTRUCT pdiagstruct)
{
    pdiagstruct->nv_pfifo_intr_0       =  REG_RD32(NV_PFIFO_INTR_0      );
    pdiagstruct->nv_pfifo_intr_en_0    =  REG_RD32(NV_PFIFO_INTR_EN_0   );
    pdiagstruct->nv_pfifo_caches       =  REG_RD32(NV_PFIFO_CACHES      );
    pdiagstruct->nv_pfifo_mode         =  REG_RD32(NV_PFIFO_MODE        );
    pdiagstruct->nv_pfifo_cache1_push0 =  REG_RD32(NV_PFIFO_CACHE1_PUSH0);
    pdiagstruct->nv_pfifo_cache1_push1 =  REG_RD32(NV_PFIFO_CACHE1_PUSH1);
    pdiagstruct->nv_pfifo_cache1_pull0 =  REG_RD32(NV_PFIFO_CACHE1_PULL0);
    pdiagstruct->nv_pfifo_dma_put      =  REG_RD32(NV_PFIFO_CACHE1_PUT     );
    pdiagstruct->nv_pfifo_dma_get      =  REG_RD32(NV_PFIFO_CACHE1_GET     );
    pdiagstruct->nv_pmc_intr_0         =  REG_RD32(NV_PMC_INTR_0        );
    pdiagstruct->nv_pmc_intr_en_0      =  REG_RD32(NV_PMC_INTR_EN_0     );
    pdiagstruct->nv_pmc_intr_read_0    =  REG_RD32(NV_PMC_INTR_READ_0   );
    pdiagstruct->nv_pmc_enable         =  REG_RD32(NV_PMC_ENABLE        );
}

void NvWatch_NV10_DumpGrTableContents(PHWINFO pDev, U032 verbose_flag)
{
    PHALHWINFO pHalHwInfo = &pDev->halHwInfo;
    PGRHALINFO_NV10 pGrHalInfo;
    PFIFOHALINFO_NV10 pFifoHalInfo;
    U032 ChID;
    U032 i;

    pHalHwInfo = &pDev->halHwInfo;
    pGrHalInfo = (PGRHALINFO_NV10)pHalHwInfo->pGrHalInfo;
    pFifoHalInfo = (PFIFOHALINFO_NV10)pHalHwInfo->pFifoHalInfo;

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
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Control3           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Control3              );
            
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Blend              = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.Blend                 );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.BlendColor         = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.BlendColor            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.SetupRaster        = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.SetupRaster           );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.FogColor           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.FogColor              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ColorKeyColor0     = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.ColorKeyColor0        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ColorKeyColor1     = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.ColorKeyColor1        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.PointSize          = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.PointSize             );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ZOffsetFactor      = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.ZOffsetFactor         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ZOffsetBias        = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.ZOffsetBias           );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ZClipMin           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.ZClipMin              );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ZClipMax           = ",           pGrHalInfo->grChannels[ChID].CurrentPatch.ZClipMax              );

            for (i = 0; i < 8; i++)
            {
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.WinClipHorz[i]   = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.WinClipHorz[i]    );
                DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.WinClipVert[i]   = ",             pGrHalInfo->grChannels[ChID].CurrentPatch.WinClipVert[i]    );
            }

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XFMode[0]            = ",         pGrHalInfo->grChannels[ChID].CurrentPatch.XFMode[0]             );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.XFMode[1]            = ",         pGrHalInfo->grChannels[ChID].CurrentPatch.XFMode[1]             );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.GlobalState0         = ",         pGrHalInfo->grChannels[ChID].CurrentPatch.GlobalState0          );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.GlobalState1         = ",         pGrHalInfo->grChannels[ChID].CurrentPatch.GlobalState1          );

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
                                                                                                                                                              
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DXMin      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DXMin        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DXMax      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DXMax        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DYMin      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DYMin        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DYMax      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.AbsUClip3DYMax        );
                                                                                                                                                              
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

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0AlphaICW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0AlphaICW      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1AlphaICW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1AlphaICW      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0ColorICW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0ColorICW      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1ColorICW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1ColorICW      );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0Factor      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0Factor        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1Factor      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1Factor        );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0AlphaOCW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0AlphaOCW      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1AlphaOCW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1AlphaOCW      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0ColorOCW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine0ColorOCW      );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1ColorOCW    = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Combine1ColorOCW      );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.CombineSpecFogCW0   = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.CombineSpecFogCW0     );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.CombineSpecFogCW1   = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.CombineSpecFogCW1     );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TextureOffset0      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TextureOffset0        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TextureOffset1      = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TextureOffset1        );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexturePalette0     = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexturePalette0       );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexturePalette1     = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexturePalette1       );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexFormat0          = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexFormat0            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexFormat1          = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexFormat1            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl0_0       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl0_0         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl0_1       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl0_1         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl1_0       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl1_0         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl1_1       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl1_1         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl2_0       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl2_0         );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl2_1       = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.TexControl2_1         );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ImageRect0          = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ImageRect0            );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.ImageRect1          = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.ImageRect1            );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Filter0             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Filter0               );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Filter1             = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Filter1               );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru0           = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru0             );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru1           = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru1             );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru2           = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.Passthru2             );

            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.DimxTexture         = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.DimxTexture           );
            DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "    pGrHalInfo->grChannels[ChID].CurrentPatch.WdimxTexture        = ",          pGrHalInfo->grChannels[ChID].CurrentPatch.WdimxTexture          );

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
