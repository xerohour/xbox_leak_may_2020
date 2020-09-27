//-----------------------------------------------------------------------------
// File: Tool.cpp
//
// Desc: Implementation file for CDispayTool class
//
// Hist: 3.13.01 - New for April XDK
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "tool.h"
#include <stdio.h>
#include <tchar.h>

//-----------------------------------------------------------------------------
// Name: CDisplayTool::CDisplayTool()
// Desc: Constructor
//-----------------------------------------------------------------------------
CDisplayTool::CDisplayTool( DMUS_NOTE_PMSG * pNotes )
{
    m_cRef = 1;                 // Set to 1 so one call to Release() will free this
    m_pNotes = pNotes;
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::~CDisplayTool()
// Desc: Destructor
//-----------------------------------------------------------------------------
CDisplayTool::~CDisplayTool()
{
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::QueryInterface()
// Desc: Per IUnknown interface
//-----------------------------------------------------------------------------
STDMETHODIMP CDisplayTool::QueryInterface(const IID &iid, void **ppv)
{
    if (iid == IID_IUnknown || iid == IID_IDirectMusicTool)
    {
        *ppv = static_cast<IDirectMusicTool*>(this);
    } 
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::AddRef()
// Desc: Per IUnknown interface
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CDisplayTool::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::Release()
// Desc: Per IUnknown interface
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CDisplayTool::Release()
{
    if( 0 == InterlockedDecrement(&m_cRef) )
    {
        delete this;
        return 0;
    }

    return m_cRef;
}



//-----------------------------------------------------------------------------
// Name: CDisplayTool::Init()
// Desc: Performs any needed initialization for the tool
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CDisplayTool::Init( IDirectMusicGraph* pGraph )
{
    // This tool has no need to do any type of initialization.
    return E_NOTIMPL;
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::GetMsgDeliveryType()
// Desc: Tells DirectMusic when we want to receive messages
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CDisplayTool::GetMsgDeliveryType( DWORD* pdwDeliveryType )
{
    // This tool wants messages before they're played, so
    // we return DMUS_PMSGF_TOOL_IMMEDIATE.  The other options are
    // DMUS_PMSGF_TOOL_QUEUE, or DMUS_PMSGF_TOOL_ATTIME.
    
    *pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::GetMediaTypeArraySize()
// Desc: Returns the number of message types we're interested in
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CDisplayTool::GetMediaTypeArraySize( DWORD* pdwNumElements )
{
    // This tool only wants note messages, patch messages, sysex, 
    // and MIDI messages, so set *pdwNumElements to 4.
    
    *pdwNumElements = 1;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::GetMediaTypes()
// Desc: Returns message types we're interested in
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CDisplayTool::GetMediaTypes( DWORD** padwMediaTypes, 
                                                    DWORD dwNumElements )
{
    // Fill in the array padwMediaTypes with the type of
    // messages this tool wants to process. In this case,
    // dwNumElements will be 1, since that is what this
    // tool returns from GetMediaTypeArraySize().
    
    if( dwNumElements == 1 )
    {
        // Set the elements in the array to DMUS_PMSGT_NOTE,
        (*padwMediaTypes)[0] = DMUS_PMSGT_NOTE;
        return S_OK;
    }
    else
    {
        // This should never happen
        return E_FAIL;
    }
}




//-----------------------------------------------------------------------------
// Name: CDisplayTool::ProcessPMsg()
// Desc: Handles processing of the message
//       We intercept messages at DMUS_PMSGF_TOOL_IMMEDIATE time, because
//       intercepting them at play time would delay the playback of the note.
//       To coordinate the timing of the various notes, we clone the message
//       and send it back to ourselves, stamped to show up at play time.
//       This way we can take our time with the copied message and not disturb
//       playback.
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CDisplayTool::ProcessPMsg( IDirectMusicPerformance* pPerf, 
                                                  DMUS_PMSG* pPMsg )
{
    DMUS_NOTE_PMSG * pNote = (DMUS_NOTE_PMSG *)pPMsg;

    if (pPMsg->dwFlags & DMUS_PMSGF_TOOL_IMMEDIATE)
    {
        // This came in from the segment. 
        // Make a copy of the PMsg and send that back to this tool, but marked
        // with DMUS_PMSGF_TOOL_ATTIME, so it will arrive at the actual time stamped
        // in the PMsg. Meanwhile, the original PMsgs should pass straight through, 
        // undelayed, so it will play properly on the synth.
        DMUS_PMSG *pCopy;
        if (SUCCEEDED(pPerf->ClonePMsg(pPMsg,&pCopy)))
        {
            pCopy->dwFlags &= ~DMUS_PMSGF_TOOL_IMMEDIATE;
            pCopy->dwFlags |= DMUS_PMSGF_TOOL_ATTIME;
            pPerf->SendPMsg(pCopy); // Since the msg wasn't changed, this will come
                                    // right back, but at the time in the time stamp.
        }

        // Now, call StampPMsg() to send the original PMsgs through to the synth.
        if(( NULL == pPMsg->pGraph ) ||
            FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
        {
            return DMUS_S_FREE;
        }

        // Return DMUS_S_REQUEUE so the original message is requeued
        return DMUS_S_REQUEUE;
    }
    else 
    {
        // This must be the DMUS_PMSGF_TOOL_ATTIME copy we went ourselves. 
        // Copy the note into our buffer so we trigger the display at the 
        // exact right time.
        m_pNotes[ pNote->dwPChannel * NUM_NOTES + pNote->bMidiValue ] = *pNote;

        // We're done with this, so free it.
        return DMUS_S_FREE;
    }
}





//-----------------------------------------------------------------------------
// Name: CDisplayTool::Flush()
// Desc: Not implemented
//-----------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE CDisplayTool::Flush( IDirectMusicPerformance* pPerf, 
                                            DMUS_PMSG* pDMUS_PMSG,
                                            REFERENCE_TIME rt)
{
    // This tool does not need to flush.
    return E_NOTIMPL;
}



