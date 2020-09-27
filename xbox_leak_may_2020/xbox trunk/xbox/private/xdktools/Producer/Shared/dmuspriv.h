/************************************************************************
*                                                                       *
*   dmuspriv.h -- This module defines the DirectMusic file formats       *
*                 used privately by DMUSProducer                        *
*   Copyright (c) 1998, Microsoft Corp. All rights reserved.            *
*                                                                       *
************************************************************************/

#ifndef DMUSPRIV_H
#define DMUSPRIV_H


typedef struct _DMUS_IO_BANKSELECT_ITEM
{
    BYTE    byLSB;
    BYTE    byMSB;
    BYTE    byPad[2];
} DMUS_IO_BANKSELECT_ITEM;


typedef struct _DMUS_IO_PATCH_ITEM
{
    MUSIC_TIME                  lTime;
    BYTE                        byStatus;
    BYTE                        byPChange;
    BYTE                        byMSB;
    BYTE                        byLSB;
    DWORD                       dwFlags;
    IDirectMusicCollection*     pIDMCollection;
    struct _DMUS_IO_PATCH_ITEM* pNext;  
} DMUS_IO_PATCH_ITEM;



#endif