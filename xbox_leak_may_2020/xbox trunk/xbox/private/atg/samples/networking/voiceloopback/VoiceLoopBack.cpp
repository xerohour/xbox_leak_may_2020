//-----------------------------------------------------------------------------
// File: VoiceLoopBack.cpp
//
// Desc: Demonstrates basic usage of the Xbox communicator.
//       This sample just monitors the microphone of each connected
//       communicator and routes the data directly to that communicator's
//       headphone
//
// Hist: 08.14.01 - New for August M1 Online XDK release
//       02.18.02 - Cleaned up for March XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XButil.h>
#include <XBFont.h>
#include <XBStopWatch.h>
#include <cassert>
#include <algorithm>
#include <xvoice.h>




//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
const DWORD COLOR_HIGHLIGHT     = 0xffffff00;     // Yellow
const DWORD COLOR_GREEN         = 0xff00ff00;     // Green
const DWORD COLOR_NORMAL        = 0xffffffff;     // White
const DWORD MAX_ERROR_STR       = 64;
const DWORD MAX_STATUS_STR      = 128;


// Voice processing: 8kHz, 16 bit samples, 10 packets, 40ms each 
const DWORD VOICE_SAMPLE_RATE   = 8000;
const DWORD BYTES_PER_SAMPLE    = 2;
const DWORD NUM_PACKETS         = 1000;
const DWORD PACKET_SIZE         = VOICE_SAMPLE_RATE * BYTES_PER_SAMPLE / 25;



//-----------------------------------------------------------------------------
// Name: class CLoopbackCommunicator
// Desc: This class represents one instance of a Xbox Communicator performing
//          basic loopback from the microphone to the headphone
//-----------------------------------------------------------------------------
class CLoopbackCommunicator
{
public:
    CLoopbackCommunicator();
    ~CLoopbackCommunicator();

    HRESULT Initialize( DWORD dwPort );
    HRESULT Inserted();
    HRESULT Removed();
    HRESULT Process();

private:
    DWORD           m_dwControllerPort;

    // Microphone-related data
    BYTE*           m_pMicrophoneBuffer;
    DWORD           m_adwMicrophoneStatus[NUM_PACKETS];
    DWORD           m_dwMicrophonePacket;
    XMediaObject*   m_pMicrophoneXMO;
    
    // Headphone-related data
    BYTE*           m_pHeadphoneBuffer;
    DWORD           m_adwHeadphoneStatus[NUM_PACKETS];
    DWORD           m_dwHeadphonePacket;
    XMediaObject*   m_pHeadphoneXMO;
};




//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    CXBFont             m_Font;                         // game font

    CLoopbackCommunicator m_aCommunicators[ XGetPortCount() ];
    DWORD               m_dwMicrophoneState;
    DWORD               m_dwHeadphoneState;
    DWORD               m_dwConnectedCommunicators;

public:
    CXBoxSample();

    virtual HRESULT Initialize();
    virtual HRESULT FrameMove();
    virtual HRESULT Render();

    HRESULT CheckCommunicatorStatus();  // Handle device insertion/removal
};



//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;

    xbApp.Run();
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample()
{
}




//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize device-dependant objects
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
    // Create a font
    if( FAILED( m_Font.Create( m_pd3dDevice, "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

    // In order to handle an Xbox Communicator being inserted between the two
    // calls to XGetDevices (or XGetDeviceChanges), we track the state of
    // microphone devices and headphone devices separately.
    m_dwConnectedCommunicators = 0;
    m_dwMicrophoneState = XGetDevices( XDEVICE_TYPE_VOICE_MICROPHONE );
    m_dwHeadphoneState  = XGetDevices( XDEVICE_TYPE_VOICE_HEADPHONE );

    for( int i = 0; i < XGetPortCount(); i++ )
    {
        // Tell the CLoopbackCommunicator which port it owns.  This doesn't
        // mean that a Communicator is inserted there - we'll call Inserted() 
        // when that happens in CheckCommunicatorStatus
        m_aCommunicators[i].Initialize( i );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
    // Check for insertion and removal of communicators
    CheckCommunicatorStatus();

    // For each active communicator, process I/O
    for( int i = 0; i < XGetPortCount(); i++ )
    {
        if( m_dwConnectedCommunicators & ( 1 << i ) )
        {
            m_aCommunicators[i].Process();
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                         0x000A0A6A, 1.0f, 0L );

    m_Font.DrawText(  64, 50, 0xffffffff, L"VoiceLoopBack" );

    // Display a status message
    if( m_dwConnectedCommunicators == 0 )
    {
        m_Font.DrawText( 100, 184, COLOR_GREEN, L"Please insert an Xbox Communicator\ninto one or more controllers." );
    }
    else
    {
        m_Font.DrawText( 100, 184, COLOR_GREEN, L"Speak into the microphone of your Xbox\nCommunicator, and you should hear your\nvoice through the headphone." );
        for( int i = 0; i < XGetPortCount(); i++ )
        {
            if( m_dwConnectedCommunicators & ( 1 << i ) )
            {
                WCHAR str[100];
                swprintf( str, L"Communicator connected to port %d", i );
                m_Font.DrawText( 100.0f, 300.0f + 30 * i, COLOR_GREEN, str );
            }
        }
    }

    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CheckCommunicatorStatus()
// Desc: Handles any changes in the status of Xbox Communicators.  In order
//          to handle the possibility that a device could be inserted 
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::CheckCommunicatorStatus()
{
    // Check the microphones
    DWORD dwMicrophoneInsertions;
    DWORD dwMicrophoneRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_VOICE_MICROPHONE, 
                       &dwMicrophoneInsertions,
                       &dwMicrophoneRemovals );

    // Check the headphones
    DWORD dwHeadphoneInsertions;
    DWORD dwHeadphoneRemovals;
    XGetDeviceChanges( XDEVICE_TYPE_VOICE_HEADPHONE, 
                       &dwHeadphoneInsertions,
                       &dwHeadphoneRemovals );

    // Update our internal state for removals
    m_dwMicrophoneState &= ~( dwMicrophoneRemovals );
    m_dwHeadphoneState  &= ~( dwHeadphoneRemovals );

    // Update state for new insertions
    m_dwMicrophoneState |= dwMicrophoneInsertions;
    m_dwHeadphoneState  |= dwHeadphoneInsertions;

    for( int i = 0; i < XGetPortCount(); i++ )
    {
        // If either the microphone or the headphone was
        // removed since last call, remove the communicator
        if( ( m_dwConnectedCommunicators & ( 1 << i ) ) &&
            ( ( dwMicrophoneRemovals & ( 1 << i ) ) ||
              ( dwHeadphoneRemovals  & ( 1 << i ) ) ) )
        {
            // Remove the Communicator
            m_aCommunicators[i].Removed();
            m_dwConnectedCommunicators &= ~( 1 << i );
        }

        // If both microphone and headphone are present, and
        // we didn't have a communicator here last frame,
        // register the insertion
        if( ( m_dwMicrophoneState & ( 1 << i ) ) &&
            ( m_dwHeadphoneState  & ( 1 << i ) ) &&
            !( m_dwConnectedCommunicators & ( 1 << i ) ) )
        {
            // Insert the headset
            if( SUCCEEDED( m_aCommunicators[i].Inserted() ) )
                m_dwConnectedCommunicators |= ( 1 << i );
        }
    }

    return S_OK;
}





//-----------------------------------------------------------------------------
// Name: CLoopbackCommunicator (ctor)
// Desc: Initializes member variables
//-----------------------------------------------------------------------------
CLoopbackCommunicator::CLoopbackCommunicator()
{
    m_dwControllerPort = -1;
    m_pMicrophoneBuffer= NULL;
    m_pHeadphoneBuffer= NULL;
    m_pMicrophoneXMO   = NULL;
    m_pHeadphoneXMO    = NULL;
}




//-----------------------------------------------------------------------------
// Name: ~CLoopbackCommunicator (Dtor)
// Desc: Frees up any resources
//-----------------------------------------------------------------------------
CLoopbackCommunicator::~CLoopbackCommunicator()
{
    Removed();
}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Initializes the communicator to a specific port
//-----------------------------------------------------------------------------
HRESULT CLoopbackCommunicator::Initialize( DWORD dwPort )
{
    m_dwControllerPort = dwPort;

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: Inserted
// Desc: Handles insertion of a communicator
//-----------------------------------------------------------------------------
HRESULT CLoopbackCommunicator::Inserted()
{
    HRESULT hr;

    OUTPUT_DEBUG_STRING( "Detected communicator insertion\n" );

    // Allocate a buffer for PCM sample data
    m_pMicrophoneBuffer = new BYTE[ PACKET_SIZE * NUM_PACKETS ];
    if( !m_pMicrophoneBuffer )
    {
        Removed();
        return E_OUTOFMEMORY;
    }

    // Allocate a buffer for PCM sample data
    m_pHeadphoneBuffer = new BYTE[ PACKET_SIZE * NUM_PACKETS ];
    if( !m_pHeadphoneBuffer )
    {
        Removed();
        return E_OUTOFMEMORY;
    }

    // Fill out a waveformat structure
    WAVEFORMATEX wfx;
    wfx.wFormatTag      = WAVE_FORMAT_PCM;
    wfx.cbSize          = 0;
    wfx.nChannels       = 1;
    wfx.nSamplesPerSec  = VOICE_SAMPLE_RATE;
    wfx.wBitsPerSample  = BYTES_PER_SAMPLE * 8;
    wfx.nBlockAlign     = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    //  Create the microphone device
    hr = XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_MICROPHONE, 
                                  m_dwControllerPort, 
                                  NUM_PACKETS,
                                  &wfx, 
                                  &m_pMicrophoneXMO );
    if( FAILED( hr ) )
    {
        OUTPUT_DEBUG_STRING( "Couldn't create microphone device\n" );
        Removed();
        return hr;
    }

    // Create the headphone device
    hr = XVoiceCreateMediaObject( XDEVICE_TYPE_VOICE_HEADPHONE, 
                                  m_dwControllerPort, 
                                  NUM_PACKETS,
                                  &wfx, 
                                  &m_pHeadphoneXMO );
    if( FAILED( hr ) )
    {
        OUTPUT_DEBUG_STRING( "Couldn't create headphone device\n" );
        Removed();
        return hr;
    }

    for( int i = 0; i < NUM_PACKETS; i++ )
    {
        // Seed the microphone device with all our media packets
        XMEDIAPACKET xmp;
        xmp.dwMaxSize           = PACKET_SIZE;
        xmp.pdwCompletedSize    = NULL;
        xmp.pContext            = NULL;
        xmp.prtTimestamp        = NULL;
        xmp.pvBuffer            = m_pMicrophoneBuffer + i * PACKET_SIZE;
        xmp.pdwStatus           = &m_adwMicrophoneStatus[i];
        *(xmp.pdwStatus)        = XMEDIAPACKET_STATUS_PENDING;

        m_pMicrophoneXMO->Process( NULL, &xmp );

        // Initialize all the headphone packets to be available
        m_adwHeadphoneStatus[i] = XMEDIAPACKET_STATUS_SUCCESS;
    }

    m_dwMicrophonePacket = 0;
    m_dwHeadphonePacket = 0;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Removed
// Desc: Handles removal (or failed insertion) of a communicator
//-----------------------------------------------------------------------------
HRESULT CLoopbackCommunicator::Removed()
{
    OUTPUT_DEBUG_STRING( "Detected communicator removal or failed insertion.\n" );

    delete[] m_pMicrophoneBuffer;
    m_pMicrophoneBuffer = NULL;

    delete[] m_pHeadphoneBuffer;
    m_pHeadphoneBuffer = NULL;

    if( m_pMicrophoneXMO )
    {
        m_pMicrophoneXMO->Release();
        m_pMicrophoneXMO = NULL;
    }

    if( m_pHeadphoneXMO )
    {
        m_pHeadphoneXMO->Release();
        m_pHeadphoneXMO = NULL;
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Process
// Desc: Processes the Microphone and Headphone XMOs to handle loopback voice
//          communication
//-----------------------------------------------------------------------------
HRESULT CLoopbackCommunicator::Process()
{
    while( m_adwMicrophoneStatus[ m_dwMicrophonePacket ] != XMEDIAPACKET_STATUS_PENDING &&
           m_adwHeadphoneStatus[ m_dwHeadphonePacket ] != XMEDIAPACKET_STATUS_PENDING )
    {
        // Copy the data into the headphone buffer
        memcpy( m_pHeadphoneBuffer + m_dwHeadphonePacket * PACKET_SIZE,
                m_pMicrophoneBuffer + m_dwMicrophonePacket * PACKET_SIZE,
                PACKET_SIZE );

        // Set the basic packet fields
        XMEDIAPACKET xmp;
        xmp.dwMaxSize           = PACKET_SIZE;
        xmp.pdwCompletedSize    = NULL;
        xmp.pContext            = NULL;
        xmp.prtTimestamp        = NULL;

        // Resubmit the microphone packet
        xmp.pvBuffer            = m_pMicrophoneBuffer + m_dwMicrophonePacket * PACKET_SIZE;
        xmp.pdwStatus           = &m_adwMicrophoneStatus[ m_dwMicrophonePacket ];
        (*xmp.pdwStatus)        = XMEDIAPACKET_STATUS_PENDING;
        m_pMicrophoneXMO->Process( NULL, &xmp );

        // Submit the copied data to the headphone
        xmp.pvBuffer            = m_pHeadphoneBuffer + m_dwHeadphonePacket * PACKET_SIZE;
        xmp.pdwStatus           = &m_adwHeadphoneStatus[ m_dwHeadphonePacket ];
        (*xmp.pdwStatus)        = XMEDIAPACKET_STATUS_PENDING;
        m_pHeadphoneXMO->Process( &xmp, NULL );

        m_dwMicrophonePacket = ( m_dwMicrophonePacket + 1 ) % NUM_PACKETS ;
        m_dwHeadphonePacket = ( m_dwHeadphonePacket + 1 ) % NUM_PACKETS ;
    }

    return S_OK;
}




