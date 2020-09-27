#ifndef _DMAPUSH_H_
#define _DMAPUSH_H_
/**************************** DMA Pusher *****************************\
*                                                                           *
* Module: DMAPUSH.H                                                         *
*       Control method implementation.                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Bruce Thompson (brucet)    02/22/96 - wrote it.                     *
*                                                                           *
\***************************************************************************/

#include "dma.h"

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Miscellaneous structure definitions.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Control object defines.
//
//---------------------------------------------------------------------------
typedef struct _def_dmapush_object  DMAPUSHER,  *PDMAPUSHER;

// The object which represents the state of the DMA pusher
struct _def_dmapush_object
{
    BUFFEROBJECT    BufferObject;
	U032            CurrentBuffer;
	U032            AddressSpace;
	U032            TlbPtBase;
	U032            Offset;
};

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------

// Allocate and init new DMA pusher object
RM_STATUS dmaCreateDmaPusher
(
    PDMAPUSHER *DmaPushObject,    // Ptr to object to initialize
	U032       ChID               // Channel this object will be used on
);

// Delete a DMA pusher object
RM_STATUS dmaDeleteDmaPusher
(
    PDMAPUSHER DmaPushObject      // Ptr to object to delete
);

// Start a DMA pusher transfer
RM_STATUS dmaPushStartXfer
(
    PBUFFEROBJECT BuffObject,     // The buffer object to use
    U032          BuffNum         // The buffer to start transfer on
);

// Start/Continue a DMA pusher transfer
RM_STATUS dmaPushXfer
(
   PBUFFER  pBuffer               // The buffer used for transfer
);

// Set the DMA pusher Dma specification object
RM_STATUS mthdSetPushBufferCtxDma
(
    POBJECT Object,               // Ptr to DMA push object for this channel
    PMETHOD Method,               // Ptr to method structure
    U032    Offset,               // Method offset
    V032    Data                  // Method data
);

// Set the DMA pusher notify Dma specification object
RM_STATUS mthdSetPushBufferNotifyCtxDma
(
    POBJECT Object,               // Ptr to DMA push object for this channel
    PMETHOD Method,               // Ptr to method structure
    U032    Offset,               // Method offset
    V032    Data                  // Method data
);

// Set the start offset for this DMA push operation
RM_STATUS mthdSetPushBufferStart
(
    POBJECT Object,               // Ptr to DMA push object for this channel
    PMETHOD Method,               // Ptr to method structure
    U032    Offset,               // Method offset
    V032    Data                  // Method data
);

// Set the length for this DMA push operation
RM_STATUS mthdSetPushBufferLength
(
    POBJECT Object,               // Ptr to DMA push object for this channel
    PMETHOD Method,               // Ptr to method structure
    U032    Offset,               // Method offset
    V032    Data                  // Method data
);

// Start the DMA push operation
RM_STATUS mthdSetPushBufferNotify
(
    POBJECT Object,               // Ptr to DMA push object for this channel
    PMETHOD Method,               // Ptr to method structure
    U032    Offset,               // Method offset
    V032    Data                  // Method data
);


#endif // _DMAPUSH_H_
