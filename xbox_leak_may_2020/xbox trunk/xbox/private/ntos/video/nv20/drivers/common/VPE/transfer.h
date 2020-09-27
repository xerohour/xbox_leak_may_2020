/***************************************************************************\
* (C) Copyright NVIDIA Corporation Inc.,                                    *
*  1996,1997, 1998. All rights reserved.                                    *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
* Module: transfer.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       schaefer           8/17/98                                          *
*                                                                           *
\***************************************************************************/
// Implements a FIFO scheme to queue up transfer which the MTM engine will use

#ifndef TRANSFER_H
#define TRANSFER_H


#define MAXTRANSFERS	(10)
#define TRANSFERNOTINUSE (0xFEEDC0DE)

#ifdef DECENTENUMS
enum {
	QUEUED	= 30,
	PROGRAMMED,
	FINISHED,
	
} TRANSSTAT;
#else

#define TRANSSTAT U032
#define QUEUED      (30)
#define PROGRAMMED  (31)
#define FINISHED    (32)
#define ERRORED     (33)

#endif //DECENTENUMS

typedef struct _aTransfer
{
	U032			TransferID;		// if not equal to TRANSFERNOTINUSE then in use!
	TRANSSTAT	transferStatus;
	U032*			pNextTransfer;
	// "real" data from MS
	U032	SurfaceData;
	U032	StartLine;
	U032  EndLine;
	U032	TransferFlags;
	PMDL	DestMDL;
	
} aTransfer;

typedef aTransfer*	pTransfer;

// adds a transfer to the next position on the list
pTransfer AddTransfer(U032 TransferID);

pTransfer GetNextTransfer();

pTransfer FindTransfer(U032 TransferID);

U032 RemoveTransfer(U032 TransferID);

void CheckForLaunch(void);
U032 SetupMTM(void);
void TearDownMTM(void);

U032 checkNvAllocArchError(U032 error);

U032 UpdateStatus(void);

U032 CancelTransfer(DDTRANSFERININFO* pTransferInInfo);


#endif //TRANSFER_H