#ifndef _DMA_H_
#define _DMA_H_
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
/********************************* DMA Manager *****************************\
*                                                                           *
* Module: DMA.H                                                             *
*       DMA object/engine management.                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//
// Include graphics object defines.
//
#ifndef _GR_H_
#include <gr.h>
#endif // _GR_H_

//---------------------------------------------------------------------------
//
//  Memory page defines.
//
//  These correspond to the granularity understood by the hardware
//  for address mapping; the system page size can be larger.
// 
//---------------------------------------------------------------------------

#define RM_PAGE_SIZE    4096
#define RM_PAGE_MASK    0x0FFF
#define RM_PAGE_SHIFT   12

#define PTE_CACHE_SIZE  64

//---------------------------------------------------------------------------
//
//  DMA object states.
//
//---------------------------------------------------------------------------

#define DMA_SET_DESC_0  0x00000001
#define DMA_SET_DESC_1  0x00000002
#define DMA_SET_DESC_2  0x00000004
#define DMA_RELOAD      0x80000000

//---------------------------------------------------------------------------
//
//  Buffer states.
//
//---------------------------------------------------------------------------

#define BUFFER_IDLE     0
#define BUFFER_BUSY     1
#define BUFFER_NOTIFY_PENDING 2

//---------------------------------------------------------------------------
//
//  NV PTE state
//
//---------------------------------------------------------------------------
typedef U032       NV_PTE_STATE;
// Definition of state bit fields
#define PTE_STATE_PRESENT	0x00000001
                                 // Bit which indicates whether or not there
                                 // is a PTE currently allocated.

//---------------------------------------------------------------------------
//
//  DMA class defines.
//
//---------------------------------------------------------------------------

typedef struct _def_dma_user_object DMAUSEROBJECT, *PDMAUSEROBJECT;
/* typedef */ struct _def_dma_user_object
{
    PCOMMONOBJECT  UserObject;
    PDMAUSEROBJECT Next;
};
 
struct _def_dma_object
{
    OBJECT Base;
    U032   State;
    U032   Valid;
    U032   DescSelector;
    VOID*  DescAddr;
    VOID*  DescOffset;
    U032   DescLimit;
    U032   LockCount;
    VOID*  LockHandle;
    DMAHALOBJINFO HalInfo;
    U032   NotifyActionHandle;     // An OS specific handle to be used for
	                               // notification action if this DMA object
								   // is used for notification.

    U032   ClientHandle;           // The global DmaList keeps DMA objects
                                   // across clients, so save the client
                                   // handle to differentiate the same ID
                                   // used between multiple clients.
    struct _def_dma_object *Next;
};

//#define GETDMA_ADDRSPACE(dma)       (dma->HalInfo.AddressSpace)
//#define GETDMAHALINFO(dma,f)        (dma->HalInfo.f)

// _def_client_dma_info is a subset of the _def_dma_object structure.
// A list of this structure is used to iterate over all the context DMAs
// allocated in this client.
typedef struct _def_client_dma_info* PCLI_DMA_INFO;
typedef struct _def_client_dma_info
{
	U032   				Handle;
	U032				Client;
	U032				Class;
	U032				Flags;
	U032				Access;
	U032				Physicality;
	U032				Locked;
	U032				Coherency;
    //OBJECT 			Base;
    //U032   			State;
    U032   				Valid;
    //U032   			Instance;
    //U032   			UseCount;
    //U032   			XferCount;
    //PDMAUSEROBJECT 	Users;
    U032   				DescSelector;	// selector for hobbled OSs (always 0 for NT)
    VOID*   			DescOffset;		// buffer address from the client (flat address for NT)
    U032   				DescLimit;		// buffer length - 1
    VOID*  				DescAddr;		// actual DMA buffer address
    VOID*  				BufferBase;
    U032   				BufferSize;
    //U032   				LockCount;
    VOID*  				LockHandle;
    //NV_PTE_STATE   	PteState;
    U032   				PteCount;
    //U032   				PteOffset;
    U032   				PteAdjust;		// offset into the current DMA page
    U032   				PteLimit;		// buffer size rounded to DMA page
    U032*				PteArray;
	//U032*				CurPte;
    //U032   			PteCache[PTE_CACHE_SIZE];
    //U032   			NotifyActionHandle;
	NV_ADDRESS_SPACE	AddressSpace;
    struct _def_client_dma_info* Next;
    struct _def_client_dma_info* Prev;

} CLI_DMA_INFO;

//---------------------------------------------------------------------------
//
//  DMA instance structure.
//
//---------------------------------------------------------------------------

typedef struct _def_dma_instance
{
   U016 Adjust;
   U016 Present;
   U032 Limit;
   U032 AddressPresentAccess[1];
} DMAINSTANCE, *PDMAINSTANCE;

//---------------------------------------------------------------------------
//
//  Notification buffer structure.
//
//---------------------------------------------------------------------------

typedef struct _def_notification_buffer
{
    U032 TimeLo;
    U032 TimeHi;
    V032 OtherInfo32;
    V016 OtherInfo16;
    V016 Status;
} NOTIFICATION, *PNOTIFICATION;


//---------------------------------------------------------------------------
//
//  Event Notification structure.
//
//---------------------------------------------------------------------------

struct _def_event_notification
{
	U032                Handle;       // This handle is not currently used
    U032                NotifyIndex;
    U032                NotifyType;
    U064                Data;
    PEVENTNOTIFICATION  Next;   
};

//---------------------------------------------------------------------------
//
//  Dual buffer structure and object.
//
//---------------------------------------------------------------------------

//
// Buffer structure.
//
typedef struct _def_buffer
{
    PDMAOBJECT         Xlate;
    PDMAOBJECT         NotifyXlate;
    PDMAOBJECT         PosXlate;
	U032               NotifyMethod;      // The method to be used on notification
	                                      // of completion of this buffer.
    U032               NotifyAction;
    U032               NotifyCount;
    U032               NotifyCurrentCount;
    U032               Start;
    U032               Length;
    U032               State;
    U032               Pitch;
    U032               Width;
} BUFFER, *PBUFFER;
typedef struct _def_buffer_object BUFFEROBJECT, *PBUFFEROBJECT;
//
// Buffer specific routine to change sample rate.
//
typedef RM_STATUS (*SAMPLEPROC)(PBUFFEROBJECT);
//
// Time sampled information.
//
typedef struct _def_sample
{
    U032       NumChannels;
    U032       Format;
    U032       Size;
    U032       Rate;
    U032       RateAdjust;
    SAMPLEPROC RateUpdate;
} SAMPLEINFO, *PSAMPLEINFO;
//
// Buffer specific routine to start and complete transferring buffer data.
//
typedef RM_STATUS (*BUFFERSTARTPROC)(PHWINFO, PBUFFEROBJECT, U032);
typedef RM_STATUS (*BUFFERPOSPROC)(PHWINFO, PBUFFEROBJECT, U032, U032 *);
typedef RM_STATUS (*BUFFERENDPROC)(PHWINFO, PBUFFEROBJECT, U032, V032, RM_STATUS);
//
// Generic buffer object.
//
struct _def_buffer_object
{
    OBJECT          Base;
    U032            Valid;
    SAMPLEINFO      Sample;
    BUFFERSTARTPROC BufferXfer;
    BUFFERPOSPROC   BufferPos;
    BUFFERENDPROC   BufferComplete;
    BUFFER          Buffer[2];
};

// Definitions of generic buffer object substructures
#define BBase           BufferObj.Base
#define BValid          BufferObj.Valid
#define BSample         BufferObj.Sample
#define BBufferXfer     BufferObj.BufferXfer
#define BBufferPos      BufferObj.BufferPos
#define BBufferComplete BufferObj.BufferComplete
#define BBuffer         BufferObj.Buffer

// Memory to memory object definitions
// The memory to memory object is the same as the buffer object
typedef struct _def_buffer_object MEMTOMEMOBJECT, *PMEMTOMEMOBJECT;

// Buffer 0 in the memory to memory object is used for the from
// memory description.
#define MEM_TO_MEM_IN_BUFFER	0
// Buffer 1 in the memory to memory object is used for the to
// memory description.
#define MEM_TO_MEM_OUT_BUFFER	1

//---------------------------------------------------------------------------
//
//  NULL DMA Descriptor.
//
//---------------------------------------------------------------------------

extern U032 dmaNullInstance;

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------
RM_STATUS dmaCreateObj(PHWINFO, PCLASSOBJECT, U032, POBJECT *);
RM_STATUS dmaDestroyObj(PHWINFO, POBJECT);
RM_STATUS dmaCreate(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS dmaDelete(VOID*, POBJECT);
RM_STATUS dmaContextInstanceToObject(PHWINFO, U032, U032, PDMAOBJECT *);
RM_STATUS dmaValidateObjectName(PHWINFO, U032, V032, PDMAOBJECT *);
RM_STATUS dmaFindContext(PHWINFO, U032, U032, PDMAOBJECT *);
RM_STATUS dmaValidateXlate(PDMAOBJECT, U032, U032);
RM_STATUS dmaGetMappedAddress(PHWINFO, PDMAOBJECT, U032, U032, VOID **);
RM_STATUS dmaAllocInstance(PHWINFO, PDMAOBJECT);
RM_STATUS dmaFreeInstance(PHWINFO, PDMAOBJECT);
RM_STATUS dmaAttach(PDMAOBJECT);
RM_STATUS dmaDetach(PDMAOBJECT);
RM_STATUS dmaBeginXfer(PDMAOBJECT, PCOMMONOBJECT);
RM_STATUS dmaEndXfer(PDMAOBJECT, PCOMMONOBJECT);
RM_STATUS dmaAllocate(PHWINFO, PDMAOBJECT);
RM_STATUS dmaDeallocate(PHWINFO, PDMAOBJECT);
RM_STATUS dmaSetBufferCtxDma(PHWINFO, PBUFFEROBJECT, PBUFFER, U032, V032);
RM_STATUS dmaSetBufferStart(PBUFFEROBJECT, PBUFFER, U032, V032);
RM_STATUS dmaSetBufferLength(PBUFFEROBJECT, PBUFFER, U032, V032);
RM_STATUS dmaSetBufferPitch(PBUFFEROBJECT, PBUFFER, U032, U032);
RM_STATUS notifySetBufferNotifyCtxDma(PHWINFO, PBUFFEROBJECT, PBUFFER, U032, V032);
RM_STATUS notifySetBufferNotify(PHWINFO, PBUFFEROBJECT, U032, U032, V032);
RM_STATUS notifyDefaultError(PHWINFO, POBJECT, U032, V032, U032, RM_STATUS);
RM_STATUS notifyMethodComplete(PHWINFO, POBJECT, U032, V032, RM_STATUS);
RM_STATUS notifyBufferHWComplete(PHWINFO, POBJECT, U032, V032, U032, RM_STATUS);
RM_STATUS notifyBufferComplete(PHWINFO, PBUFFEROBJECT, U032, V032, RM_STATUS);
RM_STATUS notifyFillNotifier(PHWINFO, PDMAOBJECT, V032, V016, RM_STATUS);
RM_STATUS notifyFillNotifierArray(PHWINFO, PDMAOBJECT, V032, V016, RM_STATUS, U032);
V032      dmaService(VOID);

RM_STATUS dmaRegisterToDevice(PHWINFO, PCLI_DMA_INFO, PDMAOBJECT *);
RM_STATUS dmaUnregisterFromDevice(PHWINFO, PDMAOBJECT);
RM_STATUS dmaBindToChannel(PHWINFO, PDMAOBJECT, U032);
RM_STATUS dmaUnbindFromChannel(PHWINFO, PDMAOBJECT, U032);

#endif // _DMA_H_
