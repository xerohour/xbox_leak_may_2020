//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/***************************************************************************\
*                                                                           *
*               Hardware Reference Manual extracted defines.                *
*                                                                           *
\***************************************************************************/
#ifndef _NV_REF_H_

#ifdef NV1_HW
//
// VGA hardware is supported on NV1
//
#define NV_VGA	1
//#include <nv1b_ref.h>
#include <nv1c_ref.h>
#endif
#ifdef NV2_HW
//
// NV2_REF.H Plus (defines which should be in NV2_REF.H)
// These defines are specific to NV2
//
#define NV_PCART_NVM_TRANSFER_SIZE_MAX	0x1000

#define NV_PGRAPH_CTX_SWITCH_COLOR_BUFFER_R5G5B5	0
#define NV_PGRAPH_CTX_SWITCH_COLOR_BUFFER_R8G8B8	1
#define NV_PGRAPH_CTX_SWITCH_COLOR_BUFFER_R10G10B10	2
#define NV_PGRAPH_CTX_SWITCH_COLOR_BUFFER_Y8	    3
#define NV_PGRAPH_CTX_SWITCH_COLOR_BUFFER_Y16	    4

//
// VGA hardware is not supported on NV2
//
#include <nv2_ref.h>
#endif

#ifdef NV3_HW
//
// VGA hardware is supported on NV1
//
#define NV_VGA	1
#include <nv3_ref.h>

#define NV_PFIFO_CACHE1_SIZE								  32
#else 
//
// NV2_REF.H Plus (defines which should be in NV2_REF.H)
// These defines are common to both NV1 and NV2
//
#define NV_PGRAPH_TRAPPED_ADDR_CLASS                          22:16
#define NV_PFIFO_CACHE1_SIZE								  32
#endif // NV3_HW


//*****************************************************************************
// 
// NV1/NV3/NV4/ conventions....
//
//   This modeset code was originally designed to be compiled for each
//   version of the chip (NV1/NV3/NV4).  However, this miniport was designed
//   to work with ALL versions of the chip.  That is, the original modeset
//   code had to be compiled for each chip.  But since this miniport
//   determines which chip it's running on at RUN time (i.e. we don't
//   compile several versions of the miniport) we need to use the following 
//   convention:
//
//      MODESET.C SHOULD ONLY CONTAIN NV3 SPECIFIC CODE
//      (OR CODE THAT HAS *NOT* CHANGED ACROSS CHIPS) !!
//
//   Mainly because NV_REF.H already includes NV3_REF.H.
//   For code which is different from NV3, that code
//   should exist separately in NV4.C/NV5.C/...etc...
//   
//   We'll follow the convention that the Miniport decides at runtime
//   which chip it's running on (i.e. only ONE miniport instead of 
//   compiling several versions for each type of chip).  
//
//*****************************************************************************


#ifdef BM_BUG
//
// There is a bug in the RevB hardware when bus mastering on certain chip sets.
// These macros enable & disable bus mastering around certain registers that
// could be corrupted by the bug.
//
extern unsigned long pciConfigMasterEnable;
extern unsigned long pciConfigMasterDisable;
extern unsigned long pciBusMasterBug;
#define BUG_BM_OFF()                                                        \
    if (pciBusMasterBug){                                                   \
        REG_WR32(NV_PBUS_PCI_NV_1, pciConfigMasterDisable);                 \
    }
#define BUG_BM_ON()                                                         \
    if (pciBusMasterBug){                                                   \
        REG_WR32(NV_PBUS_PCI_NV_1, pciConfigMasterEnable);                  \
    }
#else
#define BUG_BM_OFF()
#define BUG_BM_ON()
#endif // BM_BUG
#endif // _NV_REF_H_
