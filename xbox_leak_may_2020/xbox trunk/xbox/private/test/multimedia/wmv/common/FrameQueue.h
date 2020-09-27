/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	FrameQueue.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 04-Sep-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	04-Sep-2001 jeffsul
		Initial Version

--*/

#ifndef __FRAMEQUEUE_H__
#define __FRAMEQUEUE_H__

#include <xtl.h>

// Define a video frame structure:
typedef struct _VIDEOFRAME {
    struct _VIDEOFRAME *pNext;          // Next pending video frame
    LPDIRECT3DTEXTURE8  pTexture;       // Texture data
    REFERENCE_TIME      rtTimeStamp;    // Time stamp of video frame
    DWORD               dwSetAtVBlank;  // VBlank count at which frame was started
    BYTE *              pBits;          // Pointer to texture data
    DWORD               dwFrameNumber;  // Frame sequence number (debug)
} VIDEOFRAME;

//-------------------------------------------------------------------------
// Name: class CFrameQueue
// Desc: Maintains a queue of video frames
//-------------------------------------------------------------------------
class CFrameQueue
{
public:
	VIDEOFRAME * m_pHead;
	VIDEOFRAME * m_pTail;
	
	CFrameQueue();

	BOOL	
	IsEmpty();

	VOID	
	Enqueue( VIDEOFRAME* pFrame );

	/*VOID
	Destroy();*/

	VIDEOFRAME* 
	Dequeue(); 
};

#endif // #ifndef __FRAMEQUEUE_H__
