/*++

Copyright (c) 2000 Microsoft Corporation

    smevents.cpp

Abstract:

    Handle event processing for the state machine.

Revision History:

    07-17-00    vadimg      created

--*/

#include "precomp.h"

/***************************************************************************\
* FsmUp
*
\***************************************************************************/

VOID FsmUp(DWORD CpIndex)
{
    if (CpIndex == CP_LCP) {
        gInfo.Phase = PPP_LCP;
    }

    switch (gInfo.State) {
    case FSM_INITIAL:
        gInfo.State = FSM_CLOSED;
        break;

    case FSM_STARTING:
        InitRestartCounters();

        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }

        gInfo.State = FSM_REQ_SENT;
        break;
    }
}

/***************************************************************************\
* FsmOpen
*
\***************************************************************************/

VOID FsmOpen(DWORD CpIndex)
{
    switch (gInfo.State) {
    case FSM_INITIAL:
        if (!FsmThisLayerStarted(CpIndex)) {
            return;
        }

        gInfo.State = FSM_STARTING;
        break;

    case FSM_STARTING:
    case FSM_REQ_SENT:
    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
        break;

    case FSM_CLOSING:
        gInfo.State = FSM_STOPPING;
        break;

    case FSM_OPENED:
    case FSM_STOPPED:
    case FSM_STOPPING:
        break;

    case FSM_CLOSED:
        InitRestartCounters();

        if (!FsmSendConfigReq(CpIndex)) {
            return;
        }

        gInfo.State = FSM_REQ_SENT;
        break;
    }
}

/***************************************************************************\
* FsmDown
*
\***************************************************************************/

VOID FsmDown(DWORD CpIndex)
{
    switch (gInfo.State) {
    case FSM_CLOSED:
    case FSM_CLOSING:
        gInfo.State = FSM_INITIAL;
        break;

    case FSM_OPENED:

        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        // fall through

    case FSM_REQ_SENT:
    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
    case FSM_STOPPING:
        if (!FsmReset(CpIndex)) {
            return;
        }

        gInfo.State = FSM_STARTING;
        break;

    case FSM_STOPPED:

        if (!FsmThisLayerStarted(CpIndex)) {
            return;
        }

        if (!FsmReset(CpIndex)) {
            return;
        }

        gInfo.State = FSM_STARTING;
        break;

    }

    if (CpIndex == CP_LCP) {
        gInfo.Phase = PPP_LCP;
    }
}

/***************************************************************************\
* FsmClose
*
\***************************************************************************/

VOID FsmClose(DWORD CpIndex)
{
    if (CpIndex == CP_LCP) {
        gInfo.Phase = PPP_LCP;
    }

    switch (gInfo.State) {
    case FSM_STARTING:
        gInfo.State = FSM_INITIAL;

        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }
        break;

    case FSM_STOPPED:
        gInfo.State = FSM_CLOSED;

        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }
        break;

    case FSM_STOPPING:
        gInfo.State = FSM_CLOSING;

        if (!FsmThisLayerFinished(CpIndex)) {
            return;
        }
        break;

    case FSM_REQ_SENT:
    case FSM_OPENED:
        if (!FsmThisLayerDown(CpIndex)) {
            return;
        }

        // fall through

    case FSM_ACK_RCVD:
    case FSM_ACK_SENT:
        gInfo.State = FSM_CLOSING;
        InitRestartCounters();
        FsmSendTermReq(CpIndex);
        break;

    case FSM_CLOSING:
    case FSM_CLOSED:
    case FSM_INITIAL:
        FsmThisLayerFinished(CpIndex);
        break;
    }
}

