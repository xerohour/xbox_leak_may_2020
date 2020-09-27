/*
 *
 * device.cpp
 *
 * Device emulation
 *
 */

#include "emup.h"
#include <stdio.h>

enum {
    TRAYSTATE_CLOSED,
    TRAYSTATE_OPEN,
    TRAYSTATE_UNLOADING,
    TRAYSTATE_OPENING,
    TRAYSTATE_NOMEDIA,
    TRAYSTATE_CLOSING,
    TRAYSTATE_DETECT,
    TRAYSTATE_RESET
};

// Traystate Request Queue
typedef struct _TRQ {
    struct _TRQ *ptrqNext;
    int nTrayState;
    CEmulationFilesystem *pfsys;
    HANDLE hevt;
    HRESULT hr;
} TRQ, *PTRQ;

// Global details
static CEmulationFilesystem *g_pfsys, *g_pfsysDiscInsert;
static BOOL fStartedEmulation, fEmulationRunning, fTerminate;
static HANDLE hthrTray;
static HANDLE hevtTrayQ;
static HANDLE hevtEject;
static HANDLE hevtTrayTimer;
static HANDLE hevtTerminateEmulation;
static int nTrayState;
static CRITICAL_SECTION csFsys, csTray;
static BOOL fHaveCSTray;
static PTRQ ptrqHead, *pptrqTail;
static UCHAR nSpindleSpeed;

// Filesystem data
static PHYSICAL_GEOMETRY geom;
static DVDX2_AUTHENTICATION_PAGE dvdx2page;

static ULONG PsnFromLba(ULONG lba)
{
    ULONG psn;

    EnterCriticalSection(&csFsys);
    if(g_pfsys) {
        if(lba < geom.Layer0SectorCount)
            psn = geom.Layer0StartPSN + lba;
        else {
            lba -= geom.Layer0SectorCount;
            if(lba < geom.Layer1SectorCount)
                psn = 0x1000000 - geom.Layer0SectorCount -
                    geom.Layer0StartPSN + lba;
            else
                psn = 0;
        }
    } else
        psn = 0;
    LeaveCriticalSection(&csFsys);

    return psn;
}

void AccumulateSeekDelay(ULONG psn, PULARGE_INTEGER pluDelay)
{
    pluDelay->QuadPart = 0;
}

BOOL DefaultControlData(PUCHAR pb, ULONG cb, PULARGE_INTEGER pluDelay)
{
	return FALSE;
}

BOOL DefaultChallengeResponse(UCHAR id, PULONG pluChallenge,
    PULONG pluResponse, PULARGE_INTEGER pluDelay)
{
    pluDelay->QuadPart = 0;
    return FALSE;
}

static void NoMediaError(void)
{
    switch(nTrayState) {
    case TRAYSTATE_DETECT:
        SetErrorSense(SCSI_SENSE_NOT_READY,
            SCSI_ADSENSE_NO_MEDIA_IN_DEVICE, 0);
        break;
    default:
        SetErrorSense(SCSI_SENSE_NOT_READY, SCSI_ADSENSE_LUN_NOT_READY, 0);
        break;
    }
}

BOOL DeviceReadCapacity(ULONG *plbaEnd, ULONG *pcbBlk)
{
    BOOL fRet;

    EnterCriticalSection(&csFsys);
    if(g_pfsys) {
        *plbaEnd = geom.Layer0SectorCount + geom.Layer1SectorCount;
        *pcbBlk = 2048;
        fRet = TRUE;
    } else {
        NoMediaError();
        fRet = FALSE;
    }
    LeaveCriticalSection(&csFsys);
    return fRet;
}

ULONG DeviceReadData(ULONG cblk, ULONG lba, PUCHAR pbData, PULARGE_INTEGER
    pluDelay)
{
    ULONG psn;
    int err;
    ULONG cbRet = 0;
    ULARGE_INTEGER luDelay;
    ULONG iblk;

    pluDelay->QuadPart = 0;
    EnterCriticalSection(&csFsys);
    if(g_pfsys) {
        /* Make sure we're looking at the right partition.  If we're emulating
         * X2 and we haven't switched partitions yet, then we'll return
         * LBA out of range if the filesystem doesn't contain the standard
		 * video partition */
        if(dvdx2page.CDFValid && !dvdx2page.PartitionArea &&
			!geom.SupportsMultiplePartitions)
		{
            err = EMU_ERR_INVALID_LSN;
        } else {
            /* Loop over the PSNs of this request and accumulate read delay
             * times.  As we hit every PSN, we'll also check for placeholder
             * or out-of-bounds reads */
#if 0
            err = nSpindleSpeed < 2 ? EMU_ERR_SUCCESS :
                EMU_ERR_UNEXPECTED_ERROR;
#endif
            err = EMU_ERR_SUCCESS;
            for(iblk = 0; iblk < cblk; ++iblk) {
                psn = PsnFromLba(lba + iblk);
                if(psn == 0)
                    err = EMU_ERR_INVALID_LSN;
                else // placholders NYI
                {
                    AccumulateSeekDelay(psn, &luDelay);
                    pluDelay->QuadPart += luDelay.QuadPart;
                }
                if(err != EMU_ERR_SUCCESS)
                    break;
            }
        }

        /* If all is successful so far, we issue the read request to the
         * filesystem */
        if(err == EMU_ERR_SUCCESS)
            err = g_pfsys->GetSectorData(lba, cblk, pbData);
        if(err == EMU_ERR_SUCCESS)
            cbRet = cblk * 2048;
        else {
            cbRet = 0;
            switch(err) {
            case EMU_ERR_UNEXPECTED_ERROR:
                /* 2-sec read error, then fail */
                pluDelay->QuadPart = 2 * 1000 * 10000;
                SetErrorSense(SCSI_SENSE_MEDIUM_ERROR,
                    DVDX2_ADSENSE_GENERAL_READ_ERROR, 0);
                break;
            case EMU_ERR_PLACEHOLDER:
                pluDelay->QuadPart = 0;    
                SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                    DVDX2_ADSENSE_COMMAND_ERROR, 0);
                break;
            case EMU_ERR_INVALID_LSN:
                pluDelay->QuadPart = 0;    
                SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                    SCSI_ADSENSE_ILLEGAL_BLOCK, 0);
                break;
            }
        }
    } else {
        NoMediaError();
        cbRet = 0;
    }
    LeaveCriticalSection(&csFsys);

    return cbRet;
}

BOOL DeviceReadControlData(BOOL fXControl, PUCHAR pb, ULONG cb,
	PULARGE_INTEGER pluDelay)
{
	BOOL fRet = FALSE;

	EnterCriticalSection(&csFsys);
	if(g_pfsys) {
		if(geom.IsX2Disc) {
			if(g_pfsys->GetControlData(fXControl, pb, cb, pluDelay))
				fRet = TRUE;
			else {
				SetErrorSense(SCSI_SENSE_MEDIUM_ERROR,
					DVDEMU_ADSENSE_READ_ERROR, 0);
			}
		} else {
			SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
				SCSI_ADSENSE_INVALID_MEDIA, SCSI_SENSEQ_INCOMPATIBLE_FORMAT);
		}
	} else
		NoMediaError();
	LeaveCriticalSection(&csFsys);

	return fRet;
}

static void SetTrayState(int nState)
{
    if(fEmulationRunning) {
        SetDeviceTrayState((UCHAR)nState);
		nTrayState = nState;
	}
}

static void FinishMediaDetect(void)
{
    /* Reset to the fast spindle speed */
    nSpindleSpeed = 2;

    /* Build up the DVDX2 authentication page */
    memset(&dvdx2page, 0, sizeof dvdx2page);
    dvdx2page.PageCode = DVDX2_MODE_PAGE_AUTHENTICATION;
    dvdx2page.PageLength = sizeof dvdx2page - 2;
    dvdx2page.DrivePhaseLevel = 1;

    if(g_pfsys) {
        g_pfsys->GetPhysicalGeometry(&geom, FALSE);
        dvdx2page.DiscCategoryAndVersion = 0xD1;
        if(geom.IsX2Disc)
            dvdx2page.CDFValid = TRUE;
    }
}

USHORT DeviceSelectPage(ULONG cb, PUCHAR pbData, PULARGE_INTEGER pluDelay)
{
    USHORT cbRet = 0;
    DVDX2_AUTHENTICATION_PAGE dvdx2pageSelect;

    switch(*pbData & 0x3F) {
    case DVDX2_MODE_PAGE_AUTHENTICATION:
        EnterCriticalSection(&csFsys);
        if(g_pfsys) {
            /* Make sure we've got enough data here */
            if(pbData[1] + 2 < sizeof dvdx2pageSelect) {
                SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                    DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH, 0);
			} else if(!geom.IsX2Disc) {
				SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
					SCSI_ADSENSE_INVALID_MEDIA,
					SCSI_SENSEQ_INCOMPATIBLE_FORMAT);
            } else {
                memcpy(&dvdx2pageSelect, pbData, sizeof dvdx2pageSelect);
                /* Attempt to do a challenge */
                if(dvdx2page.PartitionArea) {
                    /* We've already switched to the Xbox partition, so no
                     * further authentication is permitted */
                    SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                        DVDX2_ADSENSE_COPY_PROTECTION_FAILURE, 2);
                } else {
                    cbRet = pbData[1] + 2;
                    /* Either this is simply a request to change to the Xbox
                     * partition or we're actually going to do a challenge */
                    if(!dvdx2pageSelect.PartitionArea &&
                        !g_pfsys->DiscChallengeResponse(dvdx2pageSelect.ChallengeID,
                        &dvdx2pageSelect.ChallengeValue,
                        &dvdx2page.ResponseValue, pluDelay))
                    {
                        SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                            DVDX2_ADSENSE_COPY_PROTECTION_FAILURE, 3);
                        cbRet = 0;
                    }

                    /* If we didn't fail the authentication, then we copy the
                     * authentication and partition flags to the global page */
                    if(cbRet) {
                        dvdx2page.Authentication =
                            dvdx2pageSelect.Authentication;
						if(geom.SupportsMultiplePartitions &&
							dvdx2page.PartitionArea !=
							dvdx2pageSelect.PartitionArea)
						{
							/* We're switching partitions on a multi-partition
							 * filesystem, so we need to process the geometry
							 * for the new partition */
							g_pfsys->GetPhysicalGeometry(&geom,
								dvdx2pageSelect.PartitionArea);
	                        dvdx2page.PartitionArea =
		                        dvdx2pageSelect.PartitionArea;
						}
                    }
                }
            }
        } else
            /* Return no media */
            NoMediaError();
        LeaveCriticalSection(&csFsys);
        break;

    case DVDX2_MODE_PAGE_ADVANCED_DRIVE_CONTROL:
        if(pbData[1] < 10 || cb < 12) {
            /* Not enough data */
            SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH, 0);
        } else if(pbData[2] > 2) {
            /* Invalid spindle speed */
            SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                DVDEMU_ADSENSE_INVALID_PARAMETER_FIELD, 0);
        } else {
            nSpindleSpeed = pbData[2];
            cbRet = pbData[1] + 2;
        }
        break;

    default:
        /* Not a page we recognize, so return an error */
        SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
            DVDEMU_ADSENSE_INVALID_PARAMETER_FIELD, 0);
        break;

    }

    return cbRet;
}

USHORT DeviceSensePage(UCHAR bPage, ULONG cb, PUCHAR pbData,
    PULARGE_INTEGER pluDelay)
{
    USHORT cbRet = 0;

    switch(bPage) {
    case DVDX2_MODE_PAGE_AUTHENTICATION:

        EnterCriticalSection(&csFsys);
        if(g_pfsys) {
            /* Make sure we have enough space for the data returned */
            if(cb < sizeof dvdx2page) {
                SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
                    DVDEMU_ADSENSE_PARAMETER_LIST_LENGTH, 0);
            } else {
                /* Copy the mode page data */
                memcpy(pbData, &dvdx2page, sizeof dvdx2page);
                cbRet = sizeof dvdx2page;
            }
        } else
            /* Return no media */
            NoMediaError();
        LeaveCriticalSection(&csFsys);
        break;

    default:
        /* Not a page we recognize, so return an error */
        SetErrorSense(SCSI_SENSE_ILLEGAL_REQUEST,
            DVDEMU_ADSENSE_INVALID_PARAMETER_FIELD, 0);
        break;

    }

    return cbRet;
}

void DeviceReset(void)
{

}

void DeviceEject(void)
{
    EnterCriticalSection(&csTray);
    if(fEmulationRunning)
        SetEvent(hevtEject);
    LeaveCriticalSection(&csTray);
}

static void ChangeTrayState(int nState)
{
    DWORD tickStart;
    DWORD dtickDelay;
	LARGE_INTEGER liTimer;

    /* Change the tray state and invoke the appropriate timer */
    tickStart = GetTickCount();

    switch(nState) {
    case TRAYSTATE_OPEN:
        /* We're going to open the tray.  If we have media loaded, unload it
         * first, otherwise open the tray immediately */
        EnterCriticalSection(&csFsys);
        if(g_pfsys) {
            g_pfsys->MountUnmount(FALSE);
            g_pfsys = NULL;
            nState = TRAYSTATE_UNLOADING;
        } else
            nState = TRAYSTATE_OPENING;
        LeaveCriticalSection(&csFsys);
        break;

    case TRAYSTATE_CLOSED:
        /* We're going to close the tray, so begin the close */
        nState = TRAYSTATE_CLOSING;
        break;

    case -1:
        /* We're going to follow the complete state machine */
        switch(nTrayState) {
        case TRAYSTATE_OPENING:
            nState = TRAYSTATE_OPEN;
            break;

        case TRAYSTATE_CLOSING:
            nState = TRAYSTATE_CLOSED;
            break;

        case TRAYSTATE_UNLOADING:
            nState = TRAYSTATE_OPENING;
            break;

        case TRAYSTATE_CLOSED:
            /* Check to see whether we have media and signal appropriately */
            if(g_pfsysDiscInsert) {
                nState = TRAYSTATE_DETECT;
                EnterCriticalSection(&csFsys);
                g_pfsys = g_pfsysDiscInsert;
                g_pfsys->MountUnmount(TRUE);
                FinishMediaDetect();
                LeaveCriticalSection(&csFsys);
            } else
                nState = TRAYSTATE_NOMEDIA;
            break;
        }
        break;

    default:
        /* No other state changes are acceptable */
        nState = -1;
        break;
    }

    if(nState >= 0) {
        /* Switch to the new state and set a timer to force another state
         * change later if necessary */
        switch(nState) {
        case TRAYSTATE_OPENING:
        case TRAYSTATE_CLOSING:
            /* One second tray motion */
            dtickDelay = 1000;
            break;
        case TRAYSTATE_UNLOADING:
            /* Half-second unload time */
            dtickDelay = 500;
            break;
        case TRAYSTATE_CLOSED:
            /* Four seconds for media detect */
            dtickDelay = 4000;
            break;
        default:
            /* No more changes for other states */
            dtickDelay = 0;
            break;
        }
        CancelWaitableTimer(hevtTrayTimer);
        ResetEvent(hevtTrayTimer);
        if(dtickDelay) {
			liTimer.QuadPart = dtickDelay;
			liTimer.QuadPart *= -10000;
            SetWaitableTimer(hevtTrayTimer, &liTimer, 0, NULL, NULL, TRUE);
        }
        SetTrayState(nState);

        /* After any state change, if there's a tray request in the queue,
         * we need to try to process it */
        if(ptrqHead)
            SetEvent(hevtTrayQ);
    }
}

static void SatisfyTrq(PTRQ ptrq, HRESULT hr)
{
    if(ptrq->hevt) {
        ptrq->hr = hr;
        SetEvent(ptrq->hevt);
    } else
        LocalFree(ptrq);
}

static DWORD WINAPI TrayStateThread(LPVOID unused)
{
    int ievt;
    HANDLE rgh[4];
    CEmulationFilesystem *pfsysDetect;
    BOOL fTrayOpen;
    PTRQ ptrq;
    HRESULT hr;

    rgh[0] = hevtTerminateEmulation;
    rgh[1] = hevtEject;
    rgh[2] = hevtTrayQ;
    rgh[3] = hevtTrayTimer;

    /* We need to avoid blocking on csTray in this function or we could
     * deadlock */
    for(;;) {
        ievt = fTerminate ? WAIT_OBJECT_0 : WaitForMultipleObjects(4, rgh,
            FALSE, INFINITE);
        switch(ievt) {
        case WAIT_OBJECT_0 + 1:
            /* Eject button got pressed */
            switch(nTrayState) {
            case TRAYSTATE_OPENING:
            case TRAYSTATE_OPEN:
                ChangeTrayState(TRAYSTATE_CLOSED);
                break;
            default:
                ChangeTrayState(TRAYSTATE_OPEN);
                break;
            }
            break;

        case WAIT_OBJECT_0 + 2:
            /* There's a tray request in the queue.  Process it if the
             * tray is not in transit */
            switch(nTrayState) {
            case TRAYSTATE_OPENING:
            case TRAYSTATE_CLOSING:
            case TRAYSTATE_UNLOADING:
                continue;
            case TRAYSTATE_OPEN:
                fTrayOpen = TRUE;
                break;
            default:
                fTrayOpen = FALSE;
                break;
            }
            if(!TryEnterCriticalSection(&csTray)) {
                /* Can't get the critical section, so go back and tray again */
                if(ptrqHead)
                    SetEvent(hevtTrayQ);
                continue;
            }
            ptrq = ptrqHead;
            if(ptrq) {
                ptrqHead = ptrq->ptrqNext;
                if(ptrqHead)
                    SetEvent(hevtTrayQ);
                if(pptrqTail == &ptrq->ptrqNext)
                    pptrqTail = &ptrqHead;
            }
            LeaveCriticalSection(&csTray);
            if(ptrq) {
                switch(ptrq->nTrayState) {
                case TRAYSTATE_OPEN:
                    hr = fTrayOpen ? E_INVALIDARG : S_OK;
                    break;
                case TRAYSTATE_CLOSED:
                    if(fTrayOpen) {
                        g_pfsysDiscInsert = ptrq->pfsys;
                        hr = S_OK;
                    } else
                        hr = E_INVALIDARG;
                    break;
                default:
                    hr = E_INVALIDARG;
                    break;
                }
                if(SUCCEEDED(hr))
                    ChangeTrayState(ptrq->nTrayState);
                SatisfyTrq(ptrq, hr);
            }
            break;

        case WAIT_OBJECT_0 + 3:
            /* Tray state change */
            ChangeTrayState(-1);
            break;
        default:
            /* Terminate.  Remove the filesystem and simulate tray closed */
            EnterCriticalSection(&csFsys);
            if(g_pfsys) {
                g_pfsys->MountUnmount(FALSE);
                g_pfsys = NULL;
            }
            LeaveCriticalSection(&csFsys);
			/* Move the traystate to no media along a valid set of states */
			switch(nTrayState) {
			case TRAYSTATE_NOMEDIA:
				/* We're already in the right state */
				break;

			case TRAYSTATE_DETECT:
				SetTrayState(TRAYSTATE_UNLOADING);
				Sleep(100);
				// fall through

			default:
				SetTrayState(TRAYSTATE_OPENING);
				Sleep(100);
				// fall through

			case TRAYSTATE_OPENING:
			case TRAYSTATE_OPEN:
				SetTrayState(TRAYSTATE_CLOSING);
				Sleep(100);
				// fall through

			case TRAYSTATE_CLOSING:
				SetTrayState(TRAYSTATE_CLOSED);
				Sleep(100);
				// fall through

			case TRAYSTATE_CLOSED:
	            SetTrayState(TRAYSTATE_NOMEDIA);
				break;
			}

            return 0;
        }
    }
}

void StopTrayState(void)
{
    PTRQ ptrq, ptrqT;

    /* Shut down the thread */
    fTerminate = TRUE;
    SetEvent(hevtTerminateEmulation);
    WaitForSingleObject(hthrTray, INFINITE);

    /* Flush the tray request queue */
    EnterCriticalSection(&csTray);
    ptrq = ptrqHead;
    ptrqHead = NULL;
    LeaveCriticalSection(&csTray);
    while(ptrq) {
        ptrqT = ptrq;
        ptrq = ptrq->ptrqNext;
        SatisfyTrq(ptrqT, E_INVALIDARG);
    }

    /* Clean up */
    CloseHandle(hthrTray);
    CloseHandle(hevtEject);
    CloseHandle(hevtTrayQ);
    CloseHandle(hevtTrayTimer);
    CloseHandle(hevtTerminateEmulation);
}

HRESULT StartEmulating(void)
{
    HRESULT hr;
    HANDLE hevt;

    for(;;) {
        if(!InterlockedExchange((PLONG)&fStartedEmulation, TRUE)) {
            /* Create some global objects */
            if(!fHaveCSTray) {
                InitializeCriticalSection(&csTray);
                InitializeCriticalSection(&csFsys);
                fHaveCSTray = TRUE;
                pptrqTail = &ptrqHead;
            }
            fTerminate = FALSE;
            hevtEject = CreateEvent(NULL, FALSE, FALSE, NULL);
            hevtTrayQ = CreateEvent(NULL, FALSE, FALSE, NULL);
            hevtTerminateEmulation = CreateEvent(NULL, FALSE, FALSE, NULL);
            hevtTrayTimer = CreateWaitableTimer(NULL, FALSE, NULL);
            hthrTray = NULL;
            if(!hevtEject || !hevtTrayQ || !hevtTerminateEmulation)
                break;

            /* Start up the traystate thread */
            hthrTray = CreateThread(NULL, 0, TrayStateThread, NULL, 0, NULL);
            if(!hthrTray)
                break;

            /* Now start the IDE session */
            EnterCriticalSection(&csTray);
            hr = StartIDE();
            if(FAILED(hr)) {
                StopTrayState();
                fStartedEmulation = FALSE;
                LeaveCriticalSection(&csTray);
                return hr;
            }
            SetTrayState(TRAYSTATE_NOMEDIA);
            fEmulationRunning = TRUE;
            LeaveCriticalSection(&csTray);
            return S_OK;
        } else {
            if(fEmulationRunning)
                return E_INVALIDARG;
            Sleep(100);
        }
    }

    if(hevtEject)
        CloseHandle(hevtEject);
    if(hevtTrayQ)
        CloseHandle(hevtTrayQ);
    if(hevtTerminateEmulation)
        CloseHandle(hevtTerminateEmulation);
    if(hevtTrayTimer)
        CloseHandle(hevtTrayTimer);
    DeleteCriticalSection(&csFsys);
    return E_OUTOFMEMORY;
}

HRESULT StopEmulating(void)
{
    HRESULT hr;

    if(!fHaveCSTray)
        hr = E_INVALIDARG;
    else {
        EnterCriticalSection(&csTray);
        if(fEmulationRunning) {
            fEmulationRunning = FALSE;
            /* The thread goes first so that we can stabilize the tray state */
            StopTrayState();
            /* Then we stop the IDE session */
            StopIDE();
            fStartedEmulation = FALSE;
            hr = S_OK;
        } else
            hr = E_INVALIDARG;
        LeaveCriticalSection(&csTray);
    }
    return hr;
}

static HRESULT QueueTrq(PTRQ ptrqIn, BOOL fWait)
{
    HRESULT hr;
    PTRQ ptrq;

    if(!fHaveCSTray)
        hr = E_INVALIDARG;
    else {
        EnterCriticalSection(&csTray);
        
        /* Queue a TRQ if there's somebody to service it */
        if(fEmulationRunning) {
            if(fWait) {
                ptrq = ptrqIn;
                ptrq->hevt = CreateEvent(NULL, FALSE, FALSE, NULL);
                if(!ptrq->hevt) {
                    ptrq = NULL;
                    hr = E_OUTOFMEMORY;
                }
            } else {
                ptrq = (PTRQ)LocalAlloc(LMEM_FIXED, sizeof *ptrq);
                memcpy(ptrq, ptrqIn, sizeof *ptrq);
                ptrq->hevt = NULL;
            }
            if(ptrq) {
                ptrq->ptrqNext = NULL;
                *pptrqTail = ptrq;
                pptrqTail = &ptrq->ptrqNext;
            }
        } else {
            ptrq = NULL;
            hr = E_INVALIDARG;
        }
        LeaveCriticalSection(&csTray);

        /* If we queued a TRQ, then generate the appropriate status.  If
         * this is an asynchronous request, the status is OK; if synchronous,
         * we have to wait for it */
        if(ptrq) {
            SetEvent(hevtTrayQ);
            if(fWait) {
                WaitForSingleObject(ptrq->hevt, INFINITE);
                CloseHandle(ptrq->hevt);
                hr = ptrq->hr;
            } else
                hr = S_OK;
        }
    }

    return hr;
}

HRESULT EmulationOpenTray(BOOL fWait)
{
    TRQ trq;

    trq.nTrayState = TRAYSTATE_OPEN;
    return QueueTrq(&trq, fWait);
}

HRESULT EmulationCloseTray(CEmulationFilesystem *pfsys, BOOL fWait)
{
    TRQ trq;

    trq.nTrayState = TRAYSTATE_CLOSED;
    trq.pfsys = pfsys;
    return QueueTrq(&trq, fWait);
}
