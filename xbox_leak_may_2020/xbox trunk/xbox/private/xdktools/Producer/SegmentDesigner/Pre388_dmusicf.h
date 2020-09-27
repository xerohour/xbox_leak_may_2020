/************************************************************************
*                                                                       *
*   Pre388_dmusicf.h -- This module defines the DirectMusic				*
*   sequence related structs used previous to build 388 of DirectMusic  *
*   Copyright (c) 1998, Microsoft Corp. All rights reserved.            *
*                                                                       *
************************************************************************/

#ifndef PRE388_DMUSICF_
#define PRE388_DMUSICF_

#include <dmusici.h>
#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>

#include <mmsystem.h>

#include <pshpack8.h>

#ifdef __cplusplus
extern "C" {
#endif



/* The following structures are used by the Tracks, and are the packed structures */
/* that are passed to the Tracks inside the IStream. */

typedef struct PRE388_DMUS_IO_SEQ_ITEM
{
    MUSIC_TIME    lTime;
    MUSIC_TIME    lDuration;
    BYTE          bEventType;
    BYTE          bStatus;
    BYTE          bByte1;
    BYTE          bByte2;
    BYTE          bType;
    BYTE          bPad[3];
} PRE388_DMUS_IO_SEQ_ITEM;


typedef struct PRE388_DMUS_IO_CURVE_ITEM
{
    MUSIC_TIME  mtStart;
    MUSIC_TIME  mtDuration;
    MUSIC_TIME  mtResetDuration;
    DWORD       dwPChannel;
    short       nStartValue;
    short       nEndValue;
    short       nResetValue;
    BYTE        bType;
    BYTE        bCurveShape;
    BYTE        bCCData;
    BYTE        bFlags;
    BYTE        bPad[2];
} PRE388_DMUS_IO_CURVE_ITEM;



#define DMUS_FOURCC_PRE388_SEQ_LIST        mmioFOURCC('s','e','q','l')
#define DMUS_FOURCC_PRE388_CURVE_LIST      mmioFOURCC('c','r','v','l')

/*
    // sequence track
    'seqt'
    (
        // pre388 sequence list
        'seql'
        (
            // sizeof PRE388_DMUS_IO_SEQ_ITEM: DWORD
            <PRE388_DMUS_IO_SEQ_ITEM>...
        )
        // pre388 curve list
        'crvl'
        (
            // sizeof PRE388_DMUS_IO_CURVE_ITEM: DWORD
            <PRE388_DMUS_IO_CURVE_ITEM>...
        )
    )
*/



#ifdef __cplusplus
}; /* extern "C" */
#endif

#include <poppack.h>

#endif /* #ifndef _DMUSICF_ */
