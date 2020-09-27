//****************************Module*Header***********************************
//
// Module Name: memstruc.h
//
//****************************************************************************

/*****************************************************************************
 *                                                                           *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.             *
 *                                                                           *
 ****************************************************************************/

//----------------------------------------------------------------------------
// WARNING:  For now, make sure this structure agrees with the one defined in MCD.H
//           We'll eventually move over to exclusively using THIS file,
//           so that the display driver can also use the memory_t structure.
//----------------------------------------------------------------------------

typedef struct _memory_t
{
    struct _memory_t    *prev;
    struct _memory_t    *next;
    struct _memory_t    *nextEmpty;
    ULONG               blkSize;
    ULONG               memBase;
} memory_t;

#define EMPTY               0x1
#define ALIGN_SIZE          64

#define IS_EMPTY(pBlk)      (((ULONG_PTR)(pBlk)->prev) & EMPTY )
#define SET_EMPTY(pBlk)     (pBlk)->prev = (memory_t *)(((BYTE *)(pBlk)->prev) + EMPTY)
#define SET_BUSY(pBlk)      (pBlk)->prev = (memory_t *)(((BYTE *)(pBlk)->prev) - EMPTY)
#define EMPTY_VALUE(pBlk)   ((memory_t *) (((BYTE *)(pBlk)) + EMPTY))
#define BUSY_VALUE(pBlk)    ((memory_t *) (((BYTE *)(pBlk)) - EMPTY))

