/*
 *
 * ide.c
 *
 * IDE channel emulation
 *
 */

#include "emup.h"
#include <stdio.h>

static HANDLE hthrIDEWorker;
static HANDLE hevtIDEWorker;
static BOOL fTerminateIDE;
static BOOL fEmulationStarted;
static BOOL fEmulating;

typedef struct _ATAPKT {
    ULONG cbSize;
    HANDLE hevtDone;
    struct _ATAPKT *ppktNextFree;
} ATAPKT, *PATAPKT;

static PATAPKT ppktGlobal;
static PATAPKT ppktFree;
static CRITICAL_SECTION csFreeList;
static ULONG cpktsOutstanding;
static UCHAR rgbSenseData[4];

/* Standard packet responses */
static const UCHAR rgbReadTOC[] = {
	1, 1,
	0, 20, 1, 0, 0, 0, 2, 0,
	0, 20, 0xaa, 0, 0, 99, 59, 74
};

__inline __declspec(naked) static USHORT __fastcall SwapUshort(PUSHORT pus)
{
    _asm {
        xor eax, eax
        mov al, BYTE PTR [ecx + 1]
        mov ah, BYTE PTR [ecx]
        ret
    }
}

__inline __declspec(naked) static ULONG __fastcall SwapUlong(PULONG pul)
{
    _asm {
        xor eax, eax
        mov al, BYTE PTR [ecx + 1]
        mov ah, BYTE PTR [ecx]
        shl eax, 16
        mov al, BYTE PTR [ecx + 3]
        mov ah, BYTE PTR [ecx + 2]
        ret
    }
}


static void QueueForFree(PATAPKT ppkt)
{
    if(ppkt) {
        /* Put this packet on the free list */
        EnterCriticalSection(&csFreeList);
        ppkt->ppktNextFree = ppktFree;
        ppktFree = ppkt;
        LeaveCriticalSection(&csFreeList);

        /* Make sure the worker thread knows that there's work to do */
        SetEvent(hevtIDEWorker);
    }
}

static PATAPKT PpktAcquirePacket(ULONG cb)
{
    PATAPKT ppkt;

    /* If we have a global packet that's not in use, we'll use it */
    ppkt = (PATAPKT)InterlockedExchange((PLONG)&ppktGlobal, NULL);
    if(ppkt) {
        if(WaitForSingleObject(ppkt->hevtDone, 0) == WAIT_TIMEOUT) {
            /* In use.  Queue it for free and get a new packet */
            QueueForFree(ppkt);
            ppkt = NULL;
        } else if(cb > ppkt->cbSize) {
            /* Too small.  Free it and get a new packet */
            CloseHandle(ppkt->hevtDone);
            LocalFree(ppkt);
            InterlockedDecrement((PLONG)&cpktsOutstanding);
            ppkt = NULL;
        }
    }

    if(!ppkt) {
        /* No good packet so far.  Allocate a new one, unless we've been told
         * to stop allocating */
        if(!fEmulating)
            return NULL;
        ppkt = (PATAPKT)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
			cb + sizeof *ppkt);
        if(!ppkt)
            return NULL;
        ppkt->hevtDone = CreateEvent(NULL, FALSE, FALSE, NULL);
        if(!ppkt) {
            LocalFree(ppkt);
            return NULL;
        }
        ppkt->cbSize = cb;
        InterlockedIncrement((PLONG)&cpktsOutstanding);
    }

    /* This packet is acceptable.  Make it global and send back the data
     * pointer */
    QueueForFree((PATAPKT)InterlockedExchange((PLONG)&ppktGlobal, (LONG)ppkt));
    return ppkt;
}

static PVOID PvFromPpkt(PATAPKT ppkt)
{
    return ppkt + 1;
}

void SetErrorSense(UCHAR b0, UCHAR b1, UCHAR b2)
{
    rgbSenseData[0] = b0;
    rgbSenseData[1] = b1;
    rgbSenseData[2] = b2;
    rgbSenseData[3] = 0;
}

static void SetUnexpectedErrorSense(void)
{
    SetErrorSense(SCSI_SENSE_HARDWARE_ERROR, 0, 0);
}

static void SetTransactionResponse(PATAPI_TRANSACTION ptrans, PATAPKT ppkt)
{
    ptrans->Response = (PUCHAR)PvFromPpkt(ppkt);
    ptrans->ResponseEvent = ppkt->hevtDone;
    SetErrorSense(0, 0, 0);
}

static void PrepareStockResponse(PATAPI_TRANSACTION ptrans, const UCHAR *pbResp,
	ULONG cb)
{
	PUCHAR pb;
	PATAPKT ppkt;
	USHORT us;

	ppkt = PpktAcquirePacket(cb + 2);
	pb = (PUCHAR)PvFromPpkt(ppkt);
	if(pbResp)
		memcpy(pb + 2, pbResp, cb);
	else
		memset(pb + 2, 0, cb);
	us = (USHORT)cb;
	us = SwapUshort(&us);
	memcpy(pb, &us, sizeof us);
	ptrans->ResponseSize = cb + 2;
	SetTransactionResponse(ptrans, ppkt);
}

static ULONG HandleATAPIPacket(PATAPI_TRANSACTION ptrans)
{
    PATAPKT ppkt;
    PMODE_PARAMETER_HEADER10 ph10;
    PDVDX2_ADVANCED_DRIVE_CONTROL_PAGE padcp;
    PSENSE_DATA psense;
    PREAD_CAPACITY_DATA pcap;
    int cblk;
    DWORD cb;
    USHORT us;
    PUCHAR pb;
	BOOL fXControl;

    ptrans->ResponseDelay.QuadPart = 0;
    switch(ptrans->CommandBlock.Cdb.CDB6GENERIC.OperationCode) {
    case SCSIOP_MODE_SENSE10:
        /* We only support a PC of 0 */
        if(ptrans->CommandBlock.Cdb.MODE_SENSE10.Pc != 0) {
            SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                DVDEMU_ADSENSE_SAVING_NOT_SUPPORTED, 0);
            return HANDLER_RETURN_ERROR;
        }

        /* Figure out how much data we're expected to return and make sure
         * it's enough to at least include the parameter header */
        ptrans->ResponseSize =
            SwapUshort((PUSHORT)ptrans->CommandBlock.Cdb.MODE_SENSE10.AllocationLength);
        if(ptrans->ResponseSize < sizeof *ph10) {
            SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH, 0);
            return HANDLER_RETURN_ERROR;
        }

        /* Get a buffer */
        ppkt = PpktAcquirePacket(ptrans->ResponseSize);

        /* Fill in the mode page data */
        ph10 = (PMODE_PARAMETER_HEADER10)PvFromPpkt(ppkt);
        memset(ph10, 0, sizeof *ph10);
        us = DeviceSensePage(ptrans->CommandBlock.Cdb.MODE_SENSE10.PageCode,
            ptrans->ResponseSize - sizeof *ph10, (PUCHAR)(ph10 + 1),
            &ptrans->ResponseDelay);

        /* Send back the data we have */
        if(us) {
            us += sizeof *ph10 - 2;
            us = SwapUshort(&us);
            memcpy(ph10->ModeDataLength, &us, sizeof us);
            SetTransactionResponse(ptrans, ppkt);
            return HANDLER_RETURN_DATA;
        }

        /* No data, so return an error */
        SetEvent(ppkt->hevtDone);
        return HANDLER_RETURN_ERROR;

    case SCSIOP_MODE_SELECT10:
        /* Make sure we have enough data */
        us = SwapUshort((PUSHORT)&ptrans->CommandBlock.Cdb.MODE_SELECT10.ParameterListLength);
        if(ptrans->DataSize < us || us < sizeof *ph10) {
            SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH, 0);
            return HANDLER_RETURN_ERROR;
        }

        /* Find the mode parameter header and make sure we have as much
         * data as it demands */
        ph10 = (PMODE_PARAMETER_HEADER10)ptrans->Data;
        cb = SwapUshort((PUSHORT)ph10->ModeDataLength);
        if(us < cb) {
            SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH, 0);
            return HANDLER_RETURN_ERROR;
        }
        cb -= sizeof *ph10 - 2;
        pb = (PUCHAR)(ph10 + 1);

        /* Now loop over all of the mode pages present and satisfy as many
         * as we can */
        while(cb) {
            /* Require PS to be 0 */
            if(*pb & 0xC0) {
                SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                    DVDEMU_ADSENSE_SAVING_NOT_SUPPORTED, 0);
                return HANDLER_RETURN_ERROR;
            }

            us = DeviceSelectPage(cb, pb, &ptrans->ResponseDelay);
            if(!us)
                /* We got an error of some sort, so bail */
                return HANDLER_RETURN_ERROR;

            /* Devour the page data and move on */
            cb -= us;
            pb += us;
        }
        return 0;

    case SCSIOP_REQUEST_SENSE:
        ppkt = PpktAcquirePacket(sizeof(SENSE_DATA));
        if(!ppkt) {
            SetUnexpectedErrorSense();
            return HANDLER_RETURN_ERROR;
        }
        psense = (PSENSE_DATA)PvFromPpkt(ppkt);
        memset(psense, 0, sizeof *psense);

        /* Fill in the sense data we have */
        psense->SenseKey = rgbSenseData[0];
        psense->AdditionalSenseCode = rgbSenseData[1];
        psense->AdditionalSenseCodeQualifier = rgbSenseData[2];
        if(rgbSenseData[3])
            psense->IncorrectLength = -1;

        /* Send it back */
        ptrans->ResponseSize = sizeof *psense;
        SetTransactionResponse(ptrans, ppkt);
        return HANDLER_RETURN_DATA;

    case SCSIOP_READ:
        /* Assume that we need a packet big enough to return the requested
         * data */
        cblk = SwapUshort((PUSHORT)&ptrans->CommandBlock.Cdb.CDB10.TransferBlocks);
        ppkt = PpktAcquirePacket(2048 * cblk);
        if(!ppkt) {
            SetUnexpectedErrorSense();
            return HANDLER_RETURN_ERROR;
        }

        /* Now ask the device to satisfy the request.  If it encounters an
         * error, it will set the sense data */
        ptrans->ResponseSize = DeviceReadData(cblk,
            SwapUlong(&ptrans->CommandBlock.Cdb.CDB10.LogicalBlock),
            (PUCHAR)PvFromPpkt(ppkt), &ptrans->ResponseDelay);
        if(ptrans->ResponseSize) {
            /* We got back data, so return it */
            SetTransactionResponse(ptrans, ppkt);
            return HANDLER_RETURN_DATA;
        }

        /* If we got here, there was an error.  We need to release the packet
         * and signal the error */
        SetEvent(ppkt->hevtDone);
        return HANDLER_RETURN_ERROR;

    case SCSIOP_READ_CD:
        /* We don't support CD data */
        SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
            SCSI_ADSENSE_INVALID_MEDIA, SCSI_SENSEQ_INCOMPATIBLE_FORMAT);
        return HANDLER_RETURN_ERROR;

    case SCSIOP_READ_DVD_STRUCTURE:
		switch(ptrans->CommandBlock.Cdb.READ_DVD_STRUCTURE.Format) {
		case 0:
			/* Physical data.  Get either the control or xcontrol frame */
			cblk = SwapUlong((PULONG)ptrans->CommandBlock.Cdb.READ_DVD_STRUCTURE.RMDBlockNumber);
	        if((ptrans->CommandBlock.Cdb.READ_DVD_STRUCTURE.Control & 0xC0) ==
				0xC0 && (UCHAR)ptrans->CommandBlock.Cdb.READ_DVD_STRUCTURE.LayerNumber ==
				(UCHAR)~DVDX2_CONTROL_DATA_LAYER && cblk ==
				~DVDX2_CONTROL_DATA_BLOCK_NUMBER)
			{
				fXControl = TRUE;
			} else if((UCHAR)ptrans->CommandBlock.Cdb.READ_DVD_STRUCTURE.LayerNumber ==
				0 && cblk == 0)
			{
				fXControl = FALSE;
			} else {
				SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
					SCSI_ADSENSE_INVALID_CDB, 0);
	            return HANDLER_RETURN_ERROR;
			}

			/* Allocate a packet for the control data frame */
			us = SwapUshort((PUSHORT)ptrans->CommandBlock.Cdb.READ_DVD_STRUCTURE.AllocationLength);
			ppkt = PpktAcquirePacket(us);
			pb = (PUCHAR)PvFromPpkt(ppkt);
			memset(pb, 0, 4);
		
			/* Fill in the control data */
			if(DeviceReadControlData(fXControl, pb + 4, us - 4,
				&ptrans->ResponseDelay))
			{
				/* Send back the data */
				ptrans->ResponseSize = us;
				SetTransactionResponse(ptrans, ppkt);
				us -= 2;
				us = SwapUshort(&us);
				memcpy(ptrans->Response, &us, 2);
				return HANDLER_RETURN_DATA;
			}

			/* If we got here, there was an error.  We need to release the
			 * packet */
			SetEvent(ppkt->hevtDone);

			break;

		case 1:
			/* Read copyright info */
			PrepareStockResponse(ptrans, NULL, 6);
			return HANDLER_RETURN_DATA;

		default:
			/* Unknown request */
			SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
				SCSI_ADSENSE_INVALID_CDB, 0);
			break;
		}

		return HANDLER_RETURN_ERROR;

    case SCSIOP_READ_TOC:
		PrepareStockResponse(ptrans, rgbReadTOC, sizeof rgbReadTOC);
        return HANDLER_RETURN_DATA;

    case SCSIOP_TEST_UNIT_READY:
        SetErrorSense(0, 0, 0);
        return 0;

    case SCSIOP_READ_CAPACITY:
        /* First assume we have to return some data */
        ppkt = PpktAcquirePacket(sizeof(READ_CAPACITY_DATA));
        if(!ppkt) {
            SetUnexpectedErrorSense();
            return HANDLER_RETURN_ERROR;
        }
        pcap = (PREAD_CAPACITY_DATA)PvFromPpkt(ppkt);
        /* Ask the device how many blocks it has.  If no media is present,
         * the device will indicate an error */
        if(DeviceReadCapacity(&pcap->LogicalBlockAddress,
            &pcap->BytesPerBlock))
        {
            pcap->LogicalBlockAddress = SwapUlong(&pcap->LogicalBlockAddress);
            pcap->BytesPerBlock = SwapUlong(&pcap->BytesPerBlock);
            ptrans->ResponseSize = sizeof *pcap;
            SetTransactionResponse(ptrans, ppkt);
            return HANDLER_RETURN_DATA;
        }

        /* We've had an error, so clean up */
        SetEvent(ppkt->hevtDone);
        return HANDLER_RETURN_ERROR;

    case SCSIOP_REPORT_KEY:
		/* Always return an AGID of zero */
		PrepareStockResponse(ptrans, NULL, 6);
		return HANDLER_RETURN_DATA;

    case SCSIOP_SEND_KEY:
		/* Accept everything */
		SetErrorSense(0, 0, 0);
		return 0;

    default:
        SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
            SCSI_ADSENSE_ILLEGAL_COMMAND, 0);
        return HANDLER_RETURN_ERROR;
    }
}

static void HandleBusReset(void)
{
    DeviceReset();
}

static ULONG HandleDeviceReset(void)
{
    HandleBusReset();
    return 0;
}

static ULONG HandleIdentifyDevice(PIDE_IDENTIFY_DATA pidd)
{
    /* TODO: IdentifyDevice */
    memset(pidd, 0, sizeof *pidd);
    return HANDLER_RETURN_DATA;
}

static DWORD WINAPI IDEWorkerThread(LPVOID unused)
{
    HANDLE rghevt[MAXIMUM_WAIT_OBJECTS];
    PATAPKT rgppkt[MAXIMUM_WAIT_OBJECTS - 1];
    int cpkt;
    PATAPKT ppkt;
    int ihevt;

    rghevt[0] = hevtIDEWorker;
    cpkt = 0;

    while(!fTerminateIDE || cpktsOutstanding) {
        /* Wait for something to do */
        ihevt = WaitForMultipleObjects(cpkt + 1, rghevt, FALSE, INFINITE) -
            WAIT_OBJECT_0;
        if(ihevt == 0) {
            /* We've got work to do.  Walk the free list and add everything on
             * it to the work list.  If we're terminating, make sure the
             * active packet shows up on the free list */
            EnterCriticalSection(&csFreeList);
            if(!fEmulating)
                QueueForFree((PATAPKT)InterlockedExchange((PLONG)&ppktGlobal,
                    NULL));
            ppkt = ppktFree;
            while(ppkt && cpkt < MAXIMUM_WAIT_OBJECTS - 1) {
                rgppkt[cpkt] = ppkt;
                rghevt[++cpkt] = ppkt->hevtDone;
                ppkt = ppkt->ppktNextFree;
            }
            ppktFree = ppkt;
            LeaveCriticalSection(&csFreeList);
        } else {
            /* One of our packets is no longer in use.  Free it */
            CloseHandle(rghevt[ihevt]);
            rghevt[ihevt--] = rghevt[cpkt--];
            LocalFree(rgppkt[ihevt]);
            rgppkt[ihevt] = rgppkt[cpkt];
            InterlockedDecrement((PLONG)&cpktsOutstanding);
        }
    }

    return 0;
}

static EMU_HANDLERS Handlers = {
    DeviceEject,
    HandleBusReset,
    HandleDeviceReset,
    HandleIdentifyDevice,
    HandleATAPIPacket
};

void StopIDEWorker(void)
{
    /* Callers should make sure we actually have a valid, active worker
     * thread */
    fTerminateIDE = TRUE;
    SetEvent(hevtIDEWorker);
    WaitForSingleObject(hthrIDEWorker, INFINITE);
    CloseHandle(hevtIDEWorker);
    CloseHandle(hthrIDEWorker);
    DeleteCriticalSection(&csFreeList);
}

HRESULT StartIDE(void)
{
    HRESULT hr;

    for(;;) {
        if(!InterlockedExchange((PLONG)&fEmulationStarted, TRUE)) {
            /* Spin up a worker thread */
            fTerminateIDE = FALSE;
            InitializeCriticalSection(&csFreeList);
            hevtIDEWorker = CreateEvent(NULL, FALSE, FALSE, NULL);
            if(!hevtIDEWorker) {
                hr = E_OUTOFMEMORY;
                break;
            }
            hthrIDEWorker = CreateThread(NULL, 0, IDEWorkerThread, NULL, 0,
                NULL);
            if(!hthrIDEWorker) {
                hr = E_OUTOFMEMORY;
                break;
            }
            /* Try to start the IDE emulator */
            hr = StartEmulationConversation(&Handlers);
            if(FAILED(hr)) {
                StopIDEWorker();
                fEmulationStarted = FALSE;
                return hr;
            }
            fEmulating = TRUE;
            return S_OK;
        } else {
            /* Somebody else started emulation already.  If it's already
             * running, we return that it's already running.  If not, we
             * wait until either it's running or the previous start attempt
             * is complete */
            if(fEmulating)
                return E_INVALIDARG;
            Sleep(100);
        }
    }

    /* We failed to start.  Clean up */
    if(hevtIDEWorker)
        CloseHandle(hevtIDEWorker);
    DeleteCriticalSection(&csFreeList);
    fEmulationStarted = FALSE;
    return hr;
}

void StopIDE(void)
{
    if(InterlockedExchange((PLONG)&fEmulating, FALSE)) {
        EndEmulationConversation();
        StopIDEWorker();
        fEmulationStarted = FALSE;
    }
}
