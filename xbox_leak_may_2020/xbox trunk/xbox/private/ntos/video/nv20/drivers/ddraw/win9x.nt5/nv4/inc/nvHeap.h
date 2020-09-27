#ifndef _NV_HEAP_H
#define _NV_HEAP_H

/*
 * heap manager interface
 */
#ifdef  MULTI_MON
#include "nvrmapi.h"
#include "nvrmarch.inc"
#endif  // MULTI_MON

#ifdef NVHEAPMGR
#define AGP_HEAP 0
#else
#define AGP_HEAP 1
#endif

#ifdef NVHEAPMGR
#ifdef MULTI_MON
static NVIOControl_Param HeapParams;
#else   // !MULTI_MON
static struct
{
  U032 function;
  U032 device;
  U032 owner;
  U032 type;
  U032 depth;
  U032 width;
  U032 height;
  S032 pitch;
  U032 offset;
  U032 size;
  U032 address;
  U032 limit;
  U032 total;
  U032 free;
  V032 status;
} HeapParams;
#endif  // !MULTI_MON
#endif  // NVHEAPMGR

#endif
