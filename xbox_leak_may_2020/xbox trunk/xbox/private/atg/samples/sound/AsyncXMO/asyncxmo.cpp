//-----------------------------------------------------------------------------
// File: AsyncXMO.cpp
//
// Desc: A sample to show how to stream data between 2 XMOs( 1 async source, 
//       one async target).
//
// Hist: 02.20.01 - New for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "dsstdfx.h"




//-----------------------------------------------------------------------------
// Callouts for labelling the gamepad on the help screen
//-----------------------------------------------------------------------------
XBHELP_CALLOUT g_HelpCallouts[] = 
{
    { XBHELP_BACK_BUTTON,  XBHELP_PLACEMENT_2, L"Display help" },
    { XBHELP_A_BUTTON,     XBHELP_PLACEMENT_2, L"Pause" },
};

#define NUM_HELP_CALLOUTS 2


//
// Debug macros
//
#define ASSERT(w) {if(w){}}
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp)
#define DBG_ERROR(_exp)
#define DBG_WARN(_exp)


//
// Simple macro to verify that the packet size is OK for a particular XMO
//
inline HRESULT XMOFILTER_VERIFY_PACKET_SIZE( XMEDIAINFO& xMediaInfo, DWORD dwPacketSize )
{
    if( xMediaInfo.dwOutputSize > dwPacketSize )
        return E_FAIL;
    if( xMediaInfo.dwOutputSize ) 
        if( dwPacketSize % xMediaInfo.dwOutputSize != 0 )
            return E_FAIL;
    if( dwPacketSize < xMediaInfo.dwMaxLookahead )
        return E_FAIL;

    return S_OK;
}


//
// Linked list structure for tracking our media packet contexts
//
struct LINKED_LIST 
{
    LINKED_LIST* pNext;
    LINKED_LIST* pPrev;

    inline VOID Initialize()         { pNext = pPrev = this; }
    inline BOOL IsListEmpty()        { return pNext == this; }
    inline LINKED_LIST* RemoveHead() { pNext->Remove(); }
    inline LINKED_LIST* RemoveTail() { pPrev->Remove(); }

    inline VOID Remove() 
    {
        LINKED_LIST* pOldNext = pNext;
        LINKED_LIST* pOldPrev = pPrev;
        pOldPrev->pNext = pOldNext;
        pOldNext->pPrev = pOldPrev;
    }

    inline VOID Add( LINKED_LIST* pEntry ) 
    {
        LINKED_LIST* pOldHead = this;
        LINKED_LIST* pOldPrev = this->pPrev;
        pEntry->pNext   = pOldHead;
        pEntry->pPrev   = pOldPrev;
        pOldPrev->pNext = pEntry;
        pOldHead->pPrev = pEntry;
    }
};


//
// We arrived at this packet size based on the requirements of the XMOs.
// The idea is to pick an arbitrary packet size, go through the Init function
// once and then if any asserts fires while verifying the packet sizes, change
// the packet to satisfy all requirements
//
#define PACKET_CNT      4
#define PACKET_SIZE     0x1000*2
#define MAXBUFSIZE      (PACKET_SIZE*PACKET_CNT)

// This structure keeps track of our packet status, buffer data, etc.
struct MEDIA_PACKET_CTX
{
    LINKED_LIST ListEntry;
    DWORD       dwStatus;
    DWORD       dwCompletedSize;
    BYTE*       pBuffer;
};


//-----------------------------------------------------------------------------
// Name: class XFilterGraph
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CFilterGraph : public CXBApplication
{
public:
    LPDIRECTSOUND8 m_pDSound;               // DirectSound object
    XFileMediaObject* m_pSourceXMO;         // Source XMO - the WMA file
    XMediaObject* m_pTargetXMO;             // Target XMO - DSound

    DWORD         m_dwSourceLength;         // Size of source
    DWORD         m_dwBytesRead;            // Bytes of stream processed

    DWORD         m_dwMinPacketSize;        // For verifying our packet size
    DWORD         m_dwPacketSize;           // Packet size
    DWORD         m_dwMaxBufferCount;       // # of buffers

    WAVEFORMATEX  m_wfxAudioFormat;         // Audio format

    LINKED_LIST   m_SourcePendingList;      // Media Contexts in source list
    LINKED_LIST   m_TargetPendingList;      // Media Contexts in target list

    BYTE*         m_pTransferBuffer;        // Buffer data
    BYTE*         m_pPacketContextPool;     // Packet contexts

    CXBFont       m_Font;                   // Font renderer
    CXBHelp       m_Help;                   // Help object
    BOOL          m_bDrawHelp;              // Should we draw help?

    HRESULT       m_hOpenResult;            // Error code from WMAStream::Initialize()
    BOOL          m_bPaused;                // Paused?

    // Virtual calls from CXBApplication
    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();
    virtual HRESULT Cleanup();

    HRESULT DownloadEffectsImage(PCHAR pszScratchFile);  // downloads a default DSP image to the GP

    // These functions attach a media context packet to an XMO by 
    // setting up the XMEDIAPACKET struct, calling Process(), and
    // then adding our media packet context to the appropriate list
    HRESULT AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx );
    HRESULT AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx );

    // Handles transferring packets through our filter graph
    HRESULT TransferData();
};




//-----------------------------------------------------------------------------
// Name: AttachPacketToSourceXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       source XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToSourceXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the source XMO (first xmo in the chain)
    hr = m_pSourceXMO->Process( NULL, &xmb );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our source list
    m_SourcePendingList.Add( &pCtx->ListEntry );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: AttachPacketToTargetXMO
//
// Desc: Helper function that prepares a packet context and attaches it to the
//       target XMO
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::AttachPacketToTargetXMO( MEDIA_PACKET_CTX* pCtx )
{
    HRESULT      hr;
    XMEDIAPACKET xmb;

    // Mark this packet as pending
    pCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

    // Set up our XMEDIAPACKET
    ZeroMemory( &xmb, sizeof(xmb) );
    xmb.pvBuffer         = pCtx->pBuffer;           // Point into buffer         
    xmb.dwMaxSize        = pCtx->dwCompletedSize;   // Size of packet
    xmb.pdwCompletedSize = &pCtx->dwCompletedSize;  // Pointer to completed size
    xmb.pdwStatus        = &pCtx->dwStatus;         // Pointer to status

    // Attach to the target XMO (first xmo in the chain)
    hr = m_pTargetXMO->Process( &xmb, NULL );
    if( FAILED(hr) ) 
    {
        DBG_ERROR(("InsertContext: Xmo %x, Process failed with hr = %x\n",hr));
    }

    // Add this context to our target list
    m_TargetPendingList.Add( &pCtx->ListEntry );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: DownloadEffectsImage
// Desc: Downloads an effects image to the DSP
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::DownloadEffectsImage(PCHAR pszScratchFile)
{
    HANDLE hFile;
    DWORD dwSize;
    PVOID pBuffer = NULL;
    HRESULT hr = S_OK;
    LPDSEFFECTIMAGEDESC pDesc;
    DSEFFECTIMAGELOC EffectLoc;

    // open scratch image file generated by xps2 tool
    hFile = CreateFile( pszScratchFile,
                        GENERIC_READ,
                        0,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL );

    if( hFile == INVALID_HANDLE_VALUE )
    {
        DWORD err;

        err = GetLastError();
        OUTPUT_DEBUG_STRING( "Failed to open the dsp image file.\n" );
        hr = HRESULT_FROM_WIN32(err);
    }

    if( SUCCEEDED(hr) )
    {
        // Determine the size of the scratch image by seeking to
        // the end of the file
        dwSize = SetFilePointer( hFile, 0, NULL, FILE_END );
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
    
        // Allocate memory to read the scratch image from disk
        pBuffer = new BYTE[dwSize];

        // Read the image in
        DWORD dwBytesRead;
        BOOL bResult = ReadFile( hFile,
                                 pBuffer,
                                 dwSize,
                                 &dwBytesRead,
                                 0 );
        
        if (!bResult)
        {
            DWORD err;

            err = GetLastError();
            OUTPUT_DEBUG_STRING( "\n Failed to open the dsp image file.\n" );
            hr = HRESULT_FROM_WIN32(err);
        }

    }

    if( SUCCEEDED(hr) )
    {
        // call dsound api to download the image..
        EffectLoc.dwI3DL2ReverbIndex = I3DL2_CHAIN_I3DL2_REVERB;
        EffectLoc.dwCrosstalkIndex = I3DL2_CHAIN_XTALK;

        hr = m_pDSound->DownloadEffectsImage( pBuffer,
                                              dwSize,
                                              &EffectLoc,
                                              &pDesc );
    }

    delete[] pBuffer;

    if( hFile != INVALID_HANDLE_VALUE ) 
    {
        CloseHandle( hFile );
    }

    return hr;
}



//-----------------------------------------------------------------------------
// Name: Initialize
//
// Desc: Creates a streaming graph between 2 XMOs: a source, and a target. 
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::Initialize()
{
    HRESULT        hr;
    DWORD          dwMinSize;
    XMEDIAINFO     xMediaInfo;
    DSSTREAMDESC   dssd;

    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // Create help
    if( FAILED( m_Help.Create( m_pd3dDevice, "Gamepad.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

    //
    // download the standard DirectSound effecs image
    //
    if( FAILED( DownloadEffectsImage("d:\\media\\dsstdfx.bin") ) )
        return E_FAIL;

    m_bDrawHelp = FALSE;
    m_bPaused = FALSE;

    m_dwBytesRead = 0;
    m_dwPacketSize     = PACKET_SIZE;
    m_dwMaxBufferCount = PACKET_CNT;

    // Create src media object
    m_hOpenResult = WmaCreateDecoder("D:\\MEDIA\\sounds\\test.wma",
                                     NULL,
                                     TRUE,
                                     4096*16,
                                     PACKET_CNT,
                                     0,
                                     &m_wfxAudioFormat,
                                     (XFileMediaObject **)&m_pSourceXMO );

    if( FAILED(m_hOpenResult) ) 
    {
        // If we failed to create a decoder, return success so we keep running.
        // FrameMove and render will check this error code and display an error
        // message instead of attempting to process the WMA
        DBG_ERROR( ("XmoFilter: WmaCreateDecoder failed with %x", m_hOpenResult) );
        return S_OK;
    }

    m_pSourceXMO->GetLength( &m_dwSourceLength );

    // Now create the target XMO, the end part of the streaming chain
    ZeroMemory( &dssd, sizeof(dssd) );
    dssd.dwFlags              = 0;
    dssd.dwMaxAttachedPackets = PACKET_CNT;
    dssd.lpwfxFormat          = &m_wfxAudioFormat;

    hr = DirectSoundCreateStream( &dssd, 
                                  (LPDIRECTSOUNDSTREAM*)&m_pTargetXMO );
    if( FAILED(hr) ) 
    {          
        DBG_ERROR( ("XmoFilter: DirectSoundCreateXMOStream failed with %x", hr) );
        DBG_ERROR( ("Init: Could not open a target audio XMO\n") );
        return hr;
    }

    //
    // At this point we should have all the XMOs.
    // Figure out the minimum packet size we need to pass media buffers
    // in between XMOs. All we do is verifying that our size (PACKET_SIZE)
    // is both larger than the min of all XMOs and its length aligned.
    // As a last check we make sure its also larger than the maxLookahead
    //
    // At the end of all this, m_dwMinPacketSize is the minimum packet size
    // for this filter graph.
    //
    ZeroMemory( &xMediaInfo, sizeof( XMEDIAINFO ) );

    if( m_pSourceXMO )
    {
        m_pSourceXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = xMediaInfo.dwOutputSize;

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    if( m_pTargetXMO )
    {
        m_pTargetXMO->GetInfo( &xMediaInfo );
        m_dwMinPacketSize = max( xMediaInfo.dwOutputSize, m_dwMinPacketSize );

        XMOFILTER_VERIFY_PACKET_SIZE( xMediaInfo, m_dwPacketSize );
    }

    ASSERT( m_pTargetXMO );
    ASSERT( m_pSourceXMO );

    // Initialize the linked lists
    m_SourcePendingList.Initialize();
    m_TargetPendingList.Initialize();

    //
    // Allocate one large buffer for incoming/outgoing data. The buffer is
    // going to be divided among our media context packets, which will be 
    // used to stream data between XMOs
    //
    dwMinSize = MAXBUFSIZE;
    m_pTransferBuffer = (BYTE *)XPhysicalAlloc( dwMinSize,
                                                MAXULONG_PTR,
                                                0,
                                                PAGE_READWRITE | PAGE_NOCACHE );
    ZeroMemory( m_pTransferBuffer, dwMinSize );

    //
    // Allocate a context pool. A context is associated with a packet buffer
    // and used to track the pending packets attached to the source or target
    // XMO.
    dwMinSize = ( PACKET_CNT * sizeof(MEDIA_PACKET_CTX) );
    m_pPacketContextPool = new BYTE[dwMinSize];
    ZeroMemory( m_pPacketContextPool, dwMinSize );

    //
    // We attach all available packet buffers in the beginning to prime the
    // graph: As each packet is completed by the source XMO, the status is
    // udpated in its packet context and we pass the packet to the next XMO.
    //
    for( DWORD i = 0; i < PACKET_CNT; i++ ) 
    {
        MEDIA_PACKET_CTX* pCtx = &((MEDIA_PACKET_CTX*)m_pPacketContextPool)[i];

        ZeroMemory( pCtx, sizeof(MEDIA_PACKET_CTX) );
        pCtx->dwCompletedSize = m_dwPacketSize;
        pCtx->pBuffer         = &m_pTransferBuffer[i*PACKET_SIZE];

        AttachPacketToSourceXMO( pCtx );
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup
// Desc: Clean up resources
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::Cleanup()
{
    // Cleanup: delete any XMO's
    if( m_pSourceXMO )       
        m_pSourceXMO->Release();
    if( m_pTargetXMO )       
        m_pTargetXMO->Release();
    if( m_pDSound )
        m_pDSound->Release();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene - just a progress bar denoting how much of the 
//       source file has been read.
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::Render()
{
    DWORD dwPercentCompleted = 100 * m_dwBytesRead / m_dwSourceLength;
    // Render the scene (which is just the progress bar)
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0xff0000ff, 1.0f, 0L );
    RenderGradientBackground( 0xFFFF0000, 0xFF0000FF );

    DirectSoundDoWork();

    // If didn't open the WMA file
    if( FAILED( m_hOpenResult ) )
    {
        WCHAR strFailure[128];
        wsprintfW( strFailure, L"Failed to load WMA; HRESULT: 0x%X", m_hOpenResult );
        m_Font.DrawText( 64,  50, 0xffffffff, strFailure );
    }
    else
    {
        // Show title, frame rate, and help
        if( m_bDrawHelp )
            m_Help.Render( &m_Font, g_HelpCallouts, NUM_HELP_CALLOUTS );
        else
        {
            // Draw the text
            m_Font.DrawText( 64,  50, 0xffffffff, L"AsyncXMO" );
        }
        WCHAR sz[100];
        swprintf( sz, L"Progress: %s", m_bPaused ? L"(Paused)" : L"" );
        m_Font.DrawText( 64, 375, 0xffffff00, sz);

        // Render a simple progress bar to show the percent completed
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX v[8];
        FLOAT x1 =  64, x2 = x1 + ( 512 * dwPercentCompleted ) / 100, x3 = 64 + 512;
        FLOAT y1 = 400, y2 = y1 + 20;
        v[0].p = D3DXVECTOR4( x1-0.5f, y1-0.5f, 1.0f, 1.0f );  v[0].color = 0xffffff00;
        v[1].p = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[1].color = 0xffffff00;
        v[2].p = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[2].color = 0xffffff00;
        v[3].p = D3DXVECTOR4( x1-0.5f, y2-0.5f, 1.0f, 1.0f );  v[3].color = 0xffffff00;
        v[4].p = D3DXVECTOR4( x2-0.5f, y1-0.5f, 1.0f, 1.0f );  v[4].color = 0xff8080ff;
        v[5].p = D3DXVECTOR4( x3-0.5f, y1-0.5f, 1.0f, 1.0f );  v[5].color = 0xff8080ff;
        v[6].p = D3DXVECTOR4( x3-0.5f, y2-0.5f, 1.0f, 1.0f );  v[6].color = 0xff8080ff;
        v[7].p = D3DXVECTOR4( x2-0.5f, y2-0.5f, 1.0f, 1.0f );  v[7].color = 0xff8080ff;

        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
        m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_QUADLIST, 2, v, sizeof(v[0]) );
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates, such as checking async xmo status, and
//       updating source file position for our progress bar
//-----------------------------------------------------------------------------
HRESULT
CFilterGraph::FrameMove()
{
    // Toggle help
    if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK ) 
    {
        m_bDrawHelp = !m_bDrawHelp;
    }

    // Toggle pause
    if( m_DefaultGamepad.bPressedAnalogButtons[ XINPUT_GAMEPAD_A ] )
    {
        m_bPaused = !m_bPaused;
        ((LPDIRECTSOUNDSTREAM)m_pTargetXMO)->Pause( m_bPaused ? DSSTREAMPAUSE_PAUSE : DSSTREAMPAUSE_RESUME );
    }

   if( !FAILED(m_hOpenResult) ) 
   {
        // Instruct routine to poll the filter graph so completed packets can be
        // moved through the streaming chain.
        if( FAILED( TransferData() ) )
            return E_FAIL;

        // Because of compression, we have to ask the filter how much
        // we've processed
        m_pSourceXMO->Seek( 0, FILE_CURRENT, &m_dwBytesRead  );
   }
        
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: TransferData
// Desc: If we have a packet completed from our source XMO, pass it through
//       the intermediate XMO and on to the target.
//       If we have a packet completed from our target XMO, recycle it back
//       to the source XMO.
//-----------------------------------------------------------------------------
HRESULT CFilterGraph::TransferData()
{
    HRESULT        hr=S_OK;
    DWORD          index = 0;
    XMEDIAPACKET   xmb;
    MEDIA_PACKET_CTX* pSrcCtx = NULL;
    MEDIA_PACKET_CTX* pDstCtx = NULL;
    DWORD             dwSize  = 0;

    // Setup media buffer
    ZeroMemory( &xmb, sizeof(xmb) );

    //
    // Always deal with the oldest packet first.  Oldest packets are at the 
    // head, since we insert at the tail, and remove from head
    //
    if( !m_SourcePendingList.IsListEmpty() ) 
    {
        pSrcCtx = (MEDIA_PACKET_CTX*)m_SourcePendingList.pNext; // Head
    }

    if( !m_TargetPendingList.IsListEmpty() ) 
    {
        pDstCtx = (MEDIA_PACKET_CTX*)m_TargetPendingList.pNext; // Head
    }        

    // Do we have a completed source packet?
    if( pSrcCtx && pSrcCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING ) 
    {
        // A src buffer completed.
        // Remove this context from the Src pending list.
        // Send it to a codec, if present, then send it to the Target xmo
        pSrcCtx->ListEntry.Remove();

        // Before we pass it on, set the context packet to amount of data we received
        xmb.pvBuffer         = pSrcCtx->pBuffer;            
        xmb.dwMaxSize        = pSrcCtx->dwCompletedSize;

        DBG_SPAM(("Context %x received %x bytes, xmb %x\n",pSrcCtx, xmb.dwMaxSize, &xmb));
        ASSERT( xmb.dwMaxSize );

        // Rset the state of our context
        pSrcCtx->dwStatus        = XMEDIAPACKET_STATUS_PENDING;

        // Remember how much data we got from the source XMO
        dwSize = xmb.dwMaxSize;

        if (dwSize < PACKET_SIZE) {
    
            //
            // the source xmo produced less than asked. this signals end of file for wma xmo
            // flush it so it starts at the begininning again...
            // Pass at least a sample worth of data to dsound so it will not RIP
            //
            m_pSourceXMO->Flush();
            pSrcCtx->dwCompletedSize = max(4,dwSize);

        }

        // Pass media buffer to the target xmo.
        AttachPacketToTargetXMO( pSrcCtx );
    }

    // Check if we have a completed packet from the target
    if( pDstCtx && pDstCtx->dwStatus != XMEDIAPACKET_STATUS_PENDING )
    {       
        DBG_SPAM(("Target XMO completed context %x\n",pDstCtx));

        // Remove the entry from the list
        pDstCtx->ListEntry.Remove();

        ASSERT(pDstCtx->dwCompletedSize != 0);

        // The target XMO is done with a buffer.
        pDstCtx->dwCompletedSize = PACKET_SIZE;
        AttachPacketToSourceXMO( pDstCtx );
    } 

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CFilterGraph xbApp;

    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}




