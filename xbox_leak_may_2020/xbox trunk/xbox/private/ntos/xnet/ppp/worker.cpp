/*++

Copyright (c) 2000 Microsoft Corporation

    worker.cpp

Abstract:

    PPP worker thread.

Revision History:

    07-11-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* TimerProc
*
\***************************************************************************/

VOID TimerProc(VOID)
{
    if (gInfo.Phase != PPP_LCP && gInfo.Phase != PPP_NCP) {
        return;
    }

    switch (gInfo.State) {
    case FSM_REQ_SENT:
    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
        FsmSendConfigReq(gInfo.Phase);

        if (gInfo.State != FSM_ACK_SENT) {
            gInfo.State = FSM_REQ_SENT;
        }
        break;
    }
}

/***************************************************************************\
* SetTimeout
*
\***************************************************************************/

VOID SetTimeout(VOID)
{
    gTimer.Timeout = 4000;
    gTimer.Expire = GetTickCount() + gTimer.Timeout;
}

/***************************************************************************\
* ClearTimeout
*
\***************************************************************************/

VOID ClearTimeout(VOID)
{
    gTimer.Timeout = 0;
}

/***************************************************************************\
* ProcessTimer
*
\***************************************************************************/

VOID ProcessTimer(VOID)
{
    if (gTimer.Timeout == 0) {
        return;
    }

    if (GetTickCount() >= gTimer.Expire) {
        TimerProc();

        gTimer.Expire = GetTickCount() + gTimer.Timeout;
    }
}

/***************************************************************************\
* DumpFrame
*
\***************************************************************************/

VOID DumpFrame(PBYTE pBuffer, DWORD cb)
{
    DWORD i;

    for (i = 0; i < cb; i++) {

        if ((i % 16) == 0) {
            DbgPrint("\n");
        }

        DbgPrint("%02x ", pBuffer[i]);
    }

    DbgPrint("\n\n");
}

/***************************************************************************\
* SendFrame
*
\***************************************************************************/

BOOL SendFrame(DWORD dwLength)
{
    DWORD cb, i = 0;
    BYTE frame[PPP_FRAME_MAX_LEN];
    
    dwLength += 2;

    DbgPrint("s:");
    DumpFrame((PBYTE)gInfo.pFrame, dwLength);

    EncodeFrame((PBYTE)gInfo.pFrame, frame, &dwLength);

    while (dwLength > 0) {

        cb = min(dwLength, MODEM_BUFFER_SIZE);
        if (!ModemWrite(&frame[i], &cb)) {
            return FALSE;
        }

        dwLength -= cb;
        i += cb;
    }

    return TRUE;
}

/***************************************************************************\
* UnlinkReceived
*
\***************************************************************************/

VOID UnlinkReceived(PPP_RECEIVED *p)
{
    if (gQueue.pReceivedFirst == p && gQueue.pReceivedLast == p) {
        gQueue.pReceivedFirst = NULL;
        gQueue.pReceivedLast = NULL;
    } else if (gQueue.pReceivedFirst == p) {
        gQueue.pReceivedFirst = p->pNext;
        gQueue.pReceivedFirst->pPrev = NULL;
    } else if (gQueue.pReceivedLast == p) {
        gQueue.pReceivedLast = p->pPrev;
        gQueue.pReceivedLast->pNext = NULL;
    } else {
        p->pPrev->pNext = p->pNext;
        p->pNext->pPrev = p->pPrev;
    }
}

/***************************************************************************\
* LinkReceived
*
\***************************************************************************/

VOID LinkReceived(PPP_RECEIVED *p)
{
    if (gQueue.pReceivedFirst == NULL) {
        gQueue.pReceivedFirst = p;
        gQueue.pReceivedLast = p;
    } else {
        p->pPrev = gQueue.pReceivedLast;
        gQueue.pReceivedLast->pNext = p;
        gQueue.pReceivedLast = p;
    }
}

/***************************************************************************\
* DumpReceived
*
\***************************************************************************/

VOID ProcessReceived(VOID)
{
    PPP_RECEIVED *pNext;
    PPP_RECEIVED *p = gQueue.pReceivedFirst;
    PPP_PACKET *pPacket;
    DWORD dwProtocol;

    while (p != NULL) {

        DbgPrint("r:");
        DumpFrame(p->pkt->buf, p->cb);

        pPacket = (PPP_PACKET*)((PPP_FRAME*)p->pkt->buf)->Data;
        dwProtocol = WireToHostFormat16(pPacket->Protocol);

        if (dwProtocol == (PPP_IPCP_PROTOCOL - 0x8000)) {
            KIRQL irql = KeRaiseIrqlToDpcLevel();
            IpReceivePacket(p->pkt);
            KeLowerIrql(irql);
        } else {
            FsmReceive(pPacket, p->cb - PPP_FRAME_HDR_LEN);
            XnetFreePacket(p->pkt);
        }

        pNext = p->pNext;

        UnlinkReceived(p);
        Free(p);

        p = pNext;
    }
}

/***************************************************************************\
* AddFrameData
*
\***************************************************************************/

VOID AddFrameData(PBYTE frame, DWORD cb, BOOL fComplete)
{
    PPP_RECEIVED *p;
    DWORD i, iFrame;
    DWORD iCopy, iCrc;

    p = gQueue.pReceived;
    if (p != NULL) {

        // add to the end of this frame
        if (frame != NULL) {
            for (i = 0, iFrame = p->cb; i < cb; i++, iFrame++) {
                p->pkt->buf[iFrame] = frame[i];
            }
            p->cb = p->cb + cb;
        }
    } else if (frame != NULL) {

        DecodeFrame(frame, &cb);

        if (cb < 4) {
            gQueue.nHdr++;
            return;
        }

        // handle address and control field compression
        if (frame[0] == 0xFF && frame[1] == 0x03) {
            iCopy = 2;
            iCrc = 0;
        } else if (gFraming.fRecvAcfc) {
            iCopy = 0;
            iCrc = 2;
        } else {
            gQueue.nHdr++;
            return;
        }

        // allocate new frame
        PPP_RECEIVED *p = (PPP_RECEIVED*)Alloc(sizeof(PPP_RECEIVED));
        if (p == NULL) {
            DbgPrint("AddFrameData: out of memory\n");
            return;
        }
        p->pkt = XnetAllocPacket(PPP_FRAME_MAX_LEN, 0);
        if (p->pkt == NULL) {
            DbgPrint("AddFrameData: out of memory\n");
            Free(p);
            return;
        }

        p->pkt->recvifp = gIp.ifp;
        p->pkt->data = &p->pkt->buf[4];
        p->pkt->datalen = p->cb - 6; // 4 (hdr) + 2 (crc)
        p->pkt->buf[0] = 0xFF;
        p->pkt->buf[1] = 0x03;

        // handle protocol field compression
        if (frame[iCopy] & 0x01) {
            if (gFraming.fRecvPfc) {
                p->pkt->buf[2] = 0x00;
                iCrc += 1;
            } else {
                XnetFreePacket(p->pkt);
                Free(p);
                gQueue.nHdr++;
                return;
            }
        } else {
            p->pkt->buf[2] = frame[iCopy];
            iCopy += 1;
        }

        CopyMemory(&p->pkt->buf[3], &frame[iCopy], cb - iCopy);
        p->iCrc = iCrc;
        p->cb = cb;

        gQueue.pReceived = p;
    }

    if (p != NULL && fComplete) {
        if (CheckCrc(&p->pkt->buf[p->iCrc], p->cb)) {
            gQueue.nOk++;
            p->cb += p->iCrc;
            LinkReceived(p);
        } else {
            gQueue.nCrc++;
            XnetFreePacket(p->pkt);
            Free(p);
        }
        gQueue.pReceived = NULL;
    }
}

/***************************************************************************\
* ReadModemData
*
\***************************************************************************/

VOID ReadModemData(VOID)
{
    BYTE buffer[MODEM_BUFFER_SIZE + 1];
    DWORD cbBuffer, iBuffer;
    BYTE frame[PPP_FRAME_MAX_LEN];
    DWORD iFrame;

    iFrame = 0;

    while (TRUE) {

        iBuffer = 0;
        cbBuffer = MODEM_BUFFER_SIZE;
        if (!ModemRead((PUCHAR)buffer, &cbBuffer) || cbBuffer == 0) {

            if (iFrame > 0) {
                AddFrameData(frame, iFrame, FALSE);
            }

            return;
        }

        buffer[cbBuffer] = '\0';
        if (str_search((const char*)buffer, "NO CARRIER") != NULL) {
            AddFrameData(NULL, 0, TRUE);
            DbgPrint("%d ok, %d hdr, %d crc\n", gQueue.nOk, gQueue.nHdr, gQueue.nCrc);
            return;
        }

        while (iBuffer < cbBuffer) {

            if (buffer[iBuffer] == PPP_FLAG_BYTE) {

                if (iFrame == 0) {
                    AddFrameData(NULL, 0, TRUE);
                } else {
                    AddFrameData(frame, iFrame, TRUE);
                    iFrame = 0;
                }

            } else {
                frame[iFrame++] = buffer[iBuffer];
            }

            iBuffer++;
        }

        Sleep(5);
    }
}

/***************************************************************************\
* WorkerThread
*
\***************************************************************************/

DWORD WorkerThread(LPVOID pThreadParameter)
{
    DWORD dwWait;

    while (TRUE) {
        
        dwWait = WaitForSingleObject(gQueue.hEvent, 1000);

        if (dwWait == WAIT_OBJECT_0) {
            ReadModemData();
            ProcessReceived();
        }

        ProcessTimer();
    }

    return 0;
}

