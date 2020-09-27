//-----------------------------------------------------------------------------
// File: PortProxy.cpp
//
// Desc: Methods to proxy a DirectMusic synth onto the Xbox
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <XBApp.h>
#include <XBUtil.h>
#include <XBFont.h>
#include <XBHelp.h>
#include "dmusicip.h"
#include "DebugCmd.h"
#include "DMConsole.h"
#include <xbdm.h>
#include "dowork.h"
#include "..\..\..\dmusic\dmime\dmperf.h"
#include <dmusbuff.h>

#define XBOX_SYNTH_NAME "Xbox Synthesizer"

extern CXBoxSample g_xbApp;

struct PChannelInfo {
	DWORD dwGroup;
	DWORD dwMChannel;
    long  lBendRange;
	short nCurrentRPN;
	short nLastPBend;
//    short nCount;
};

#define MIDI_PBEND          0xE0 
#define MIDI_SYSX           0xF0
#define CC_DATAENTRYMSB 0x06
#define CC_DATAENTRYLSB 0x26
#define CC_NRPN_LSB    0x62
#define CC_NRPN_MSB    0x63
#define CC_RPN_LSB     0x64
#define CC_RPN_MSB     0x65
#define RPN_PITCHBEND  0x00

static REFERENCE_TIME s_rtBufferStart = 0;
static REFERENCE_TIME s_rtTimingOffset = 0;

static IDirectMusicAudioPath *s_pIDirectMusicAudioPath = NULL;
static DWORD s_dwNumPChannels = 0;
//static DWORD s_dwTempNumPChannels = 0;
static DWORD s_dwDefaultAudioPathID = DMUS_APATH_SHARED_STEREOPLUSREVERB;
static PChannelInfo *s_pPChannelInfo = NULL;
//static PChannelInfo *s_pTempPChannelInfo = NULL;
//static BOOL fChanged = FALSE;
//static DWORD s_dwOldAudioPathID = DMUS_APATH_SHARED_STEREOPLUSREVERB;

//static DWORD  s_dwNextAudioPathID = 0;
//        s_dwDefaultAudioPathID = dwPathID;
//        hr = RecreateDefaultAudioPath(dwPathID,s_dwNumPChannels);

//HRESULT RecreateDefaultAudioPath(DWORD dwAudioPathID, DWORD dwNumPChannels);


/*void DumpPChannelInfo()

{
    if (s_dwNextAudioPathID != s_dwDefaultAudioPathID)
    {
        s_dwDefaultAudioPathID = s_dwNextAudioPathID;
        RecreateDefaultAudioPath(s_dwDefaultAudioPathID,s_dwNumPChannels);
    }
    if (fChanged)
    {
        if (s_pTempPChannelInfo)
        {
            DbgPrint("Audiopath: %lx\nPCh\tG\tCh\tNotes Played\n",s_dwOldAudioPathID);
            DWORD dwX;
            for (dwX = 0; dwX < s_dwTempNumPChannels; dwX++)
            {
                DbgPrint("%ld\t%ld\t%ld\t%ld\n",dwX,s_pTempPChannelInfo[dwX].dwGroup,
                    s_pTempPChannelInfo[dwX].dwMChannel,(long) s_pTempPChannelInfo[dwX].nCount);
            }
            DbgPrint("\n");
            s_dwOldAudioPathID = s_dwDefaultAudioPathID;
        }
        fChanged = FALSE;
    }
}
*/

//-----------------------------------------------------------------------------
// Name: RCmdSynthPlayBuffer
// Desc: plays a buffer of MIDI events
//-----------------------------------------------------------------------------
HRESULT RCmdSynthPlayBuffer(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 4 )
    {
        DCCMDPrintf("Need to specify the buffer time and size, and the \"now\" time\n");
		hr = E_INVALIDARG;
    }

	DWORD dwSize = 0;
	REFERENCE_TIME rtPCNow = 0;
	if( SUCCEEDED(hr) )
	{
		s_rtBufferStart = _atoi64(argv[1]);
		rtPCNow = _atoi64(argv[2]);
		dwSize = atoi(argv[3]);

		if( dwSize == 0 )
		{
			hr = E_INVALIDARG;
		}
	}

	// Set the timing offset
	/* Should no longer need this timing offset
	if( SUCCEEDED(hr)
	&&	(0 == s_rtTimingOffset) )
	{
		REFERENCE_TIME rtXboxNow = 0;
		if( SUCCEEDED( g_pPerformance->GetTime( &rtXboxNow, NULL ) ) )
		{
			s_rtTimingOffset = rtXboxNow - rtPCNow;
		}
	}
	*/

	// Allocate the buffer to receive the data in
	BYTE *pbData = NULL;
	if( SUCCEEDED(hr) )
	{
		pbData = new BYTE[dwSize];

		if( pbData == NULL )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if( SUCCEEDED(hr) )
	{
		pdmcc->HandlingFunction = SynthReceiveBuffer;
		pdmcc->DataSize = 0;
		pdmcc->Buffer = pbData;
		pdmcc->BufferSize = dwSize;
		pdmcc->CustomData = 0;
		pdmcc->BytesRemaining = dwSize;

		hr = XBDM_READYFORBIN;
	}

	return hr;
}

void GMReset( void )
{
	if( s_pPChannelInfo )
	{
		for( DWORD i = 0; i < s_dwNumPChannels; i++ )
		{
			s_pPChannelInfo[i].lBendRange = 200;
			s_pPChannelInfo[i].nCurrentRPN = 0x3FFF;
			s_pPChannelInfo[i].nLastPBend = 0x2000;
		}
	}
}

/*void PlayMIDI()

{
    IDirectMusicSynthX *pSynth;
    HRESULT hr = g_pPerformance->GetSynth(&pSynth);

	if( SUCCEEDED(hr) )
	{
        CDownloadItem *pDownloadItem = (CDownloadItem *) g_xbApp.m_lstDownloads.GetHead();
        for (; pDownloadItem; pDownloadItem = pDownloadItem->GetNext() )
        {
            if (pDownloadItem->m_hHandle == NULL)
		    {
                BOOL fFree;
                hr = pSynth->Download(&pDownloadItem->m_hHandle,
                    pDownloadItem->m_pbBuffer,&fFree);  
            }
        }
        pSynth->Release();
    }
}

void QueueMIDI(DWORD dwMsg)

{
    dwBuffer[dwWrite++] = dwMsg;
    if (dwWrite >= 100) dwWrite = 0;
}*/

void PlayBufferOnDefaultPort( BYTE *pbBuffer, DWORD cbBuffer, REFERENCE_TIME rtBufferStart )
{
	HRESULT hr = E_FAIL;

    IDirectMusicSynthX *pSynth;
    hr = g_pPerformance->GetSynth(&pSynth);

	if( SUCCEEDED(hr) )
	{

		class MIDIEVENT : public DMUS_EVENTHEADER {
		public:
			 BYTE  abEvent[4];           // Actual event data, rounded up to be an even number
										 // of QWORD's (8 bytes)
		};

		typedef class MIDIEVENT FAR  *LPMIDIEVENT;

		while( cbBuffer )
		{
			// Skip events that are too short
			if (cbBuffer < sizeof(DMUS_EVENTHEADER))
			{
				break;
			}

			LPMIDIEVENT lpEventHdr = (LPMIDIEVENT)pbBuffer;
			DWORD cbEvent = DMUS_EVENT_SIZE(lpEventHdr->cbEvent);
			if (cbEvent > cbBuffer)
			{
				break;
			}

			pbBuffer += cbEvent;
			cbBuffer -= cbEvent;

			const BYTE bStatus = lpEventHdr->abEvent[0];
			DWORD dwChannelGroup = lpEventHdr->dwChannelGroup;
			DWORD dwOrigPChannelBase = max( 0, dwChannelGroup-1) * 16;

			// If a normal MIDI event (non-sysex)
			if( (lpEventHdr->cbEvent < 4)
			&&	(bStatus & 0x80)
			&&	(bStatus & 0xF0) != 0xF0 )
			{
				DWORD dwOrigPChannel = dwOrigPChannelBase + (bStatus & 0xF);

				if( dwOrigPChannel < s_dwNumPChannels )
				{
					lpEventHdr->abEvent[0] = BYTE((bStatus & 0xF0) | BYTE(s_pPChannelInfo[dwOrigPChannel].dwMChannel & 0xF));
					dwChannelGroup = s_pPChannelInfo[dwOrigPChannel].dwGroup;
//                    s_pPChannelInfo[dwOrigPChannel].nCount++;
				}
				else
				{
					// Invalid PChannel
					assert(FALSE);
					continue;
				}

				if( (bStatus & 0xF0) == MIDI_PBEND )
				{
					long lBend = lpEventHdr->abEvent[1] | (lpEventHdr->abEvent[2] << 7);
					s_pPChannelInfo[dwOrigPChannel].nLastPBend = short(lBend);
					lBend -= 0x2000;
					lBend *= s_pPChannelInfo[dwOrigPChannel].lBendRange;
					lBend += 0x1000;        // handle rounding up.
					lBend /= 0x2000;
					lBend += 0x2000;
					lpEventHdr->abEvent[1] = (BYTE) lBend & 0x7F;
					lpEventHdr->abEvent[2] = (BYTE) (lBend >> 7) & 0x7F;
				}
				else if( (bStatus & 0xF0) == MIDI_CCHANGE )
				{
					switch( lpEventHdr->abEvent[1] )
					{
					case CC_RPN_LSB:
						s_pPChannelInfo[dwOrigPChannel].nCurrentRPN = (s_pPChannelInfo[dwOrigPChannel].nCurrentRPN & 0x3f80) + lpEventHdr->abEvent[2];
						break; 
					case CC_RPN_MSB:
						s_pPChannelInfo[dwOrigPChannel].nCurrentRPN = (s_pPChannelInfo[dwOrigPChannel].nCurrentRPN & 0x7f) + (lpEventHdr->abEvent[2] << 7);
						break;
					case CC_NRPN_LSB :
					case CC_NRPN_MSB :
						s_pPChannelInfo[dwOrigPChannel].nCurrentRPN = 0x3FFF;
						break;
					case CC_DATAENTRYMSB:
						switch (s_pPChannelInfo[dwOrigPChannel].nCurrentRPN)
						{
						case RPN_PITCHBEND:
							// Special case for a range of 82 semitones, because this must
							// really be an attempt to have the pitch bend map directly to
							// pitch cents.
							if (lpEventHdr->abEvent[2] == 82)
							{
								s_pPChannelInfo[dwOrigPChannel].lBendRange = 8192;
							}
							else
							{
								s_pPChannelInfo[dwOrigPChannel].lBendRange = lpEventHdr->abEvent[2] * 100;
							}

							// Change this to a pitch bend message (since the synth ignores the RPN)
							{
								long lBend = s_pPChannelInfo[dwOrigPChannel].nLastPBend;
								lBend -= 0x2000;
								lBend *= s_pPChannelInfo[dwOrigPChannel].lBendRange;
								lBend += 0x1000;        // handle rounding up.
								lBend /= 0x2000;
								lBend += 0x2000;
								lpEventHdr->abEvent[1] = (BYTE) lBend & 0x7F;
								lpEventHdr->abEvent[2] = (BYTE) (lBend >> 7) & 0x7F;
								lpEventHdr->abEvent[0] = (lpEventHdr->abEvent[0] & 0xF) | MIDI_PBEND;
							}

							break;
						}
					}
				}
                DWORD dwMsg = lpEventHdr->abEvent[0] + (lpEventHdr->abEvent[1] << 8) + (lpEventHdr->abEvent[2] << 16);
                pSynth->SendShortMsg(rtBufferStart + lpEventHdr->rtDelta,dwChannelGroup, dwMsg);
			}
			else
			{
				// Just change the channel group
				if( dwOrigPChannelBase < s_dwNumPChannels )
				{
					dwChannelGroup = s_pPChannelInfo[dwOrigPChannelBase].dwGroup;
				}
				else
				{
					// Invalid PChannel
					assert(FALSE);
					continue;
				}

				if(	(MIDI_SYSX == bStatus)
				&&	(lpEventHdr->cbEvent >= 6))
				{
					switch( lpEventHdr->abEvent[1] )
					{
					case 0x7E : // General purpose ID
						if (lpEventHdr->abEvent[3] == 0x09) 
						{
							GMReset();
						}
						break;
					case 0x41 : // Roland
						if (lpEventHdr->cbEvent < 11) 
						{
							break;
						}
						if (lpEventHdr->abEvent[3] != 0x42) break;
						if (lpEventHdr->abEvent[4] != 0x12) break;

						// GS Reset
						if( 0x40007F == ((lpEventHdr->abEvent[5] << 16) |
							((lpEventHdr->abEvent[6] & 0xF0) << 8) | lpEventHdr->abEvent[7]) )
						{
							GMReset();
						}
						break;
					case 0x43 : // Yamaha
						if ((lpEventHdr->abEvent[3] == 0x4C) &&
							(lpEventHdr->abEvent[4] == 0) &&
							(lpEventHdr->abEvent[5] == 0) &&
							(lpEventHdr->abEvent[6] == 0x7E) &&
							(lpEventHdr->abEvent[7] == 0))
						{   // XG System On
							GMReset();
						}
						break;
					}
				}
                pSynth->SendLongMsg(rtBufferStart + lpEventHdr->rtDelta,dwChannelGroup, 
                    lpEventHdr->abEvent, lpEventHdr->cbEvent);
			}

			g_xbApp.m_fMIDICounter+=0.4F;
			if( g_xbApp.m_fMIDICounter > 100.0 )
			{
				g_xbApp.m_fMIDICounter = 1.0;
			}

		}
        pSynth->Release();
	}
}

HRESULT __stdcall SynthReceiveBuffer(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
	//char strDebug[500];
	//sprintf( strDebug, "Recv: %x %x %x %x %x %x\n", pdmcc->HandlingFunction, pdmcc->DataSize,
	//	pdmcc->Buffer, pdmcc->BufferSize, pdmcc->CustomData, pdmcc->BytesRemaining );
	//OutputDebugStringA(strDebug);
	if( pdmcc->DataSize != pdmcc->BufferSize )
	{
		// We can't handle this
		DebugBreak();
	}
	if( pdmcc->DataSize != 0 )
	{
		//g_xbApp.m_fVolume = min( 256, pdmcc->DataSize ) * 100.0 / 256.0;

		//REFERENCE_TIME rtLatencyTime = 0;
		//g_pPerformance->GetLatencyTime( &rtLatencyTime );

		LPBYTE pbBuffer = (LPBYTE) pdmcc->Buffer;
		PlayBufferOnDefaultPort( pbBuffer, pdmcc->DataSize, s_rtBufferStart + s_rtTimingOffset );
		delete []pbBuffer;
	}

	// We don't want to receive any more buffers
	pdmcc->BytesRemaining = 0;
	return XBDM_NOERR;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthDownload
// Desc: Downloads an instrument
//-----------------------------------------------------------------------------
HRESULT RCmdSynthDownload(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 3 )
    {
        DCCMDPrintf("Need to specify the ID and size\n");
		hr = E_INVALIDARG;
    }

	DWORD dwID = 0xFFFFFFFF;
	DWORD dwSize = 0;
	if( SUCCEEDED(hr) )
	{
		dwID = atoi(argv[1]);
		dwSize = atoi(argv[2]);

		if( dwSize == 0 )
		{
			hr = E_INVALIDARG;
		}
	}

	// Allocate the buffer to receive the data in
	BYTE *pbData = NULL;
	if( SUCCEEDED(hr) )
	{
		pbData = new BYTE[dwSize];

		if( pbData == NULL )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if( SUCCEEDED(hr) )
	{
		pdmcc->HandlingFunction = SynthReceiveDownload;
		pdmcc->DataSize = 0;
		pdmcc->Buffer = pbData;
		pdmcc->BufferSize = dwSize;
		pdmcc->CustomData = (PVOID)dwID;
		pdmcc->BytesRemaining = dwSize;

		hr = XBDM_READYFORBIN;
	}

	return hr;
}


void DownloadToDefaultPort( BYTE *pbBuffer, DWORD cbBuffer, DWORD dwUniqueDownloadID )
{
    IDirectMusicSynthX *pSynth = NULL;
	HRESULT hr = E_FAIL;

	if( g_pPerformance )
    {
        hr = g_pPerformance->GetSynth(&pSynth);
    }
    if (SUCCEEDED(hr))
    {
        HANDLE hDownload = NULL; 
        BOOL fFree;
        hr = pSynth->Download(&hDownload,pbBuffer,&fFree); 
        pSynth->Release();
        if( SUCCEEDED(hr) )
	    {
            if (fFree)
            {
                delete [] pbBuffer;
                pbBuffer = NULL;
            }
		    CDownloadItem *pDownloadItem = new CDownloadItem;
		    if( pDownloadItem )
		    {
			    pDownloadItem->m_hHandle = hDownload;
                pDownloadItem->m_dwUniqueID = dwUniqueDownloadID;
                pDownloadItem->m_pbBuffer = pbBuffer;
			    g_xbApp.m_lstDownloads.AddTail( pDownloadItem );
		    }
	    }
    }
}

HRESULT __stdcall SynthReceiveDownload(PDM_CMDCONT pdmcc, LPSTR szResponse, DWORD cchResponse)
{
	//char strDebug[500];
	//sprintf( strDebug, "Download: %x %x %x %x %x %x\n", pdmcc->HandlingFunction, pdmcc->DataSize,
	//	pdmcc->Buffer, pdmcc->BufferSize, pdmcc->CustomData, pdmcc->BytesRemaining );
	//OutputDebugStringA(strDebug);

	//	pdmcc->DataSize = 0; // Data to process in this call
	//	pdmcc->Buffer = pbData; // Offset into buffer
	//	pdmcc->BufferSize = dwSize; // Size of total buffer
	//	pdmcc->CustomData = pbData; // Download ID
	//	pdmcc->BytesRemaining = dwSize; // Size of buffer left

	// Get a BYTE pointer to the buffer start
	LPBYTE pbSrcBuffer = (LPBYTE) pdmcc->Buffer;

	// Subtract the amount of data we read in
	pdmcc->BytesRemaining -= pdmcc->DataSize;

	// Increment the buffer pointer
	pbSrcBuffer += pdmcc->DataSize;

	// Update the DM_CMDCONT structure's pointer
	pdmcc->Buffer = pbSrcBuffer;

	if( 0 == pdmcc->BytesRemaining )
	{
		// If we're done

		// Compute the buffer start
		LPBYTE pbSrcStart = pbSrcBuffer - pdmcc->BufferSize;

		// Call DownloadToDefaultPort with buffer pointer, buffer size, and buffer ID
		DownloadToDefaultPort( pbSrcStart, pdmcc->BufferSize, (DWORD)pdmcc->CustomData );
	}

	return XBDM_NOERR;
}

HRESULT CALLBACK FreeHandle(HANDLE hHandle, HANDLE hUserData)
{
    BYTE * pbData = (BYTE *)hUserData;
    delete [] pbData;
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthUnload
// Desc: Unloads an instrument buffer
//-----------------------------------------------------------------------------
void UnloadFromDefaultPort( CDownloadItem *pDownloadItem )
{
    IDirectMusicSynthX *pSynth = NULL;
	if( g_pPerformance )
    {
        if (SUCCEEDED(g_pPerformance->GetSynth(&pSynth)))
        {
            pSynth->Unload(pDownloadItem->m_hHandle,FreeHandle,pDownloadItem->m_pbBuffer); 
            pSynth->Release();
        }
    }
}

HRESULT RCmdSynthUnload(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the ID\n");
		hr = E_INVALIDARG;
    }

	CDownloadItem *pDownloadItem = NULL;
	DWORD dwID = 0xFFFFFFFF;
	if( SUCCEEDED(hr) )
	{
		dwID = atoi(argv[1]);

	    // Ensure this download buffer exists
        CDownloadItem *pDownloadTmp = (CDownloadItem *)g_xbApp.m_lstDownloads.GetHead();
        while( pDownloadTmp )
        {
            if( dwID == pDownloadTmp->m_dwUniqueID )
            {
				pDownloadItem = pDownloadTmp;
			    break;
            }
            pDownloadTmp = pDownloadTmp->GetNext();
        }
	}


	if( NULL == pDownloadItem )
	{
		char strDebugString[MAX_PATH];
		sprintf( strDebugString, "RCmdSynthUnload: Download ID %d not found\n", dwID );
		OutputDebugString( strDebugString );
        hr = E_INVALIDARG;
	}

	// Unload the download buffer
	if( SUCCEEDED(hr) )
	{
		UnloadFromDefaultPort( pDownloadItem );
	}

	if( pDownloadItem )
	{
		// Release the download buffer
//		pDownloadItem->m_pIDirectMusicDownload->Release();
//		pDownloadItem->m_pIDirectMusicDownload = NULL;

		// Remove the item from the list
		g_xbApp.m_lstDownloads.Remove( pDownloadItem );

		// Delete the item
		delete pDownloadItem;
	}

	return hr;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthGetTime
// Desc: Gets the current time
//-----------------------------------------------------------------------------
HRESULT RCmdSynthGetTime(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	HRESULT hr = S_OK;

	REFERENCE_TIME rtXboxNow = 0;
	if( cchResp > 30
	&&	SUCCEEDED( g_pPerformance->GetTime( &rtXboxNow, NULL ) ) )
	{
		sprintf( szResp, "%I64d", rtXboxNow );
	}
	else
	{
		hr = E_FAIL;
	}

	return hr;
}


HRESULT RecreateDefaultAudioPath(DWORD dwAudioPathID, DWORD dwNumPChannels)

{
    HRESULT hr;
	IDirectMusicAudioPath *pIDirectMusicAudioPath = NULL;
    hr = g_pPerformance->CreateStandardAudioPath( dwAudioPathID, dwNumPChannels, TRUE, &pIDirectMusicAudioPath );
    s_dwDefaultAudioPathID = dwAudioPathID;
	// Get a pointer to the performance's class
	CPerformance *pCPerformance = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = g_pPerformance->QueryInterface( IID_CPerformance, (void **)&pCPerformance );
	}

	// If dwNumPChannels is zero, release our current audiopath
	if( SUCCEEDED(hr) && !dwNumPChannels )
	{
		if( s_pIDirectMusicAudioPath )
		{
			s_pIDirectMusicAudioPath->Release();
			s_pIDirectMusicAudioPath = NULL;
		}
		delete []s_pPChannelInfo;
		s_pPChannelInfo = NULL;
	}

	PChannelInfo *pPChannelInfo = NULL;
	if( SUCCEEDED(hr) && pIDirectMusicAudioPath )
	{
		pPChannelInfo = new PChannelInfo[dwNumPChannels];
		
		if( !pPChannelInfo )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if( SUCCEEDED(hr) && pPChannelInfo )
	{
		DWORD dwRealPChannel;
		for( DWORD i=0; i < dwNumPChannels; i++ )
		{
			hr = pIDirectMusicAudioPath->ConvertPChannel( i, &dwRealPChannel );
			if( FAILED(hr) )
			{
				break;
			}

			hr = pCPerformance->PChannelInfo( dwRealPChannel, NULL, &pPChannelInfo[i].dwGroup, &pPChannelInfo[i].dwMChannel );
			if( FAILED(hr) )
			{
				break;
			}

			pPChannelInfo[i].lBendRange = 200;
			pPChannelInfo[i].nCurrentRPN = 0x3FFF;
			pPChannelInfo[i].nLastPBend = 0x2000;
		}
	}

	if( SUCCEEDED(hr) && pPChannelInfo )
	{
		if( s_pIDirectMusicAudioPath )
		{
			s_pIDirectMusicAudioPath->Release();
		}
		s_pIDirectMusicAudioPath = pIDirectMusicAudioPath;
		pIDirectMusicAudioPath = NULL;

//		DWORD dwRealPChannel = 0;
//		s_pIDirectMusicAudioPath->ConvertPChannel( 0, &dwRealPChannel );

		s_dwNumPChannels = dwNumPChannels;
		delete []s_pPChannelInfo;
        s_pPChannelInfo = pPChannelInfo;
		pPChannelInfo = NULL;
	}

	if( pPChannelInfo )
	{
		delete []pPChannelInfo;
	}

	if( pCPerformance )
	{
		pCPerformance->Release();
	}

	if( pIDirectMusicAudioPath )
	{
		pIDirectMusicAudioPath->Release();
	}

	return hr;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthSetDefaultAudioPath
// Desc: Sets the default audiopath to perform on
//-----------------------------------------------------------------------------
HRESULT RCmdSynthSetDefaultAudioPath(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the id of the audiopath\n");
		hr = E_INVALIDARG;
    }

	DWORD dwPathID = atoi(argv[1]);
    if (s_dwDefaultAudioPathID != dwPathID)
    {
        s_dwDefaultAudioPathID = dwPathID;
        hr = RecreateDefaultAudioPath(dwPathID,s_dwNumPChannels);
    }
    return hr;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthSetChannelGroups
// Desc: Sets the number of channel groups
//-----------------------------------------------------------------------------
HRESULT RCmdSynthSetChannelGroups(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	HRESULT hr = S_OK;

    // Check our arguments
    if( argc < 2 )
    {
        DCCMDPrintf("Need to specify the number of channel groups\n");
		hr = E_INVALIDARG;
    }

	DWORD dwNumPChannels = 0;
//	IDirectMusicAudioPath *pIDirectMusicAudioPath = NULL;
	if( SUCCEEDED(hr) )
	{
		dwNumPChannels = atoi(argv[1]) * 16;

		// If the number of PChannels changed to a non-zero number
		if( dwNumPChannels
		&&	(dwNumPChannels != s_dwNumPChannels) )
		{
            hr = RecreateDefaultAudioPath(s_dwDefaultAudioPathID, dwNumPChannels);
//			hr = g_pPerformance->CreateStandardAudioPath( DMUS_APATH_SHARED_STEREOPLUSREVERB, dwNumPChannels, TRUE, &pIDirectMusicAudioPath );
		}
		else
		{
			hr = S_FALSE;
		}
	}

	// Get a pointer to the performance's class
/*	CPerformance *pCPerformance = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = g_pPerformance->QueryInterface( IID_CPerformance, (void **)&pCPerformance );
	}

	// If dwNumPChannels is zero, release our current audiopath
	if( SUCCEEDED(hr) && !dwNumPChannels )
	{
		if( s_pIDirectMusicAudioPath )
		{
			s_pIDirectMusicAudioPath->Release();
			s_pIDirectMusicAudioPath = NULL;
		}
		s_dwNumPChannels = 0;
		delete []s_pPChannelInfo;
		s_pPChannelInfo = NULL;
	}

	PChannelInfo *pPChannelInfo = NULL;
	if( SUCCEEDED(hr) && pIDirectMusicAudioPath )
	{
		pPChannelInfo = new PChannelInfo[dwNumPChannels];
		
		if( !pPChannelInfo )
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if( SUCCEEDED(hr) && pPChannelInfo )
	{
		DWORD dwRealPChannel;
		for( DWORD i=0; i < dwNumPChannels; i++ )
		{
			hr = pIDirectMusicAudioPath->ConvertPChannel( i, &dwRealPChannel );
			if( FAILED(hr) )
			{
				break;
			}

			hr = pCPerformance->PChannelInfo( dwRealPChannel, NULL, &pPChannelInfo[i].dwGroup, &pPChannelInfo[i].dwMChannel );
			if( FAILED(hr) )
			{
				break;
			}

			pPChannelInfo[i].lBendRange = 200;
			pPChannelInfo[i].nCurrentRPN = 0x3FFF;
			pPChannelInfo[i].nLastPBend = 0x2000;
		}
	}

	if( SUCCEEDED(hr) && pPChannelInfo )
	{
		if( s_pIDirectMusicAudioPath )
		{
			s_pIDirectMusicAudioPath->Release();
		}
		s_pIDirectMusicAudioPath = pIDirectMusicAudioPath;
		pIDirectMusicAudioPath = NULL;

		DWORD dwRealPChannel = 0;
		s_pIDirectMusicAudioPath->ConvertPChannel( 0, &dwRealPChannel );
//		pCPerformance->PChannelInfo( dwRealPChannel, &s_pSynthProxyPort, NULL, NULL );

		s_dwNumPChannels = dwNumPChannels;
		delete []s_pPChannelInfo;
		s_pPChannelInfo = pPChannelInfo;
		pPChannelInfo = NULL;
	}

	if( pPChannelInfo )
	{
		delete []pPChannelInfo;
	}

	if( pCPerformance )
	{
		pCPerformance->Release();
	}

	if( pIDirectMusicAudioPath )
	{
		pIDirectMusicAudioPath->Release();
	}
*/
	return hr;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthInitialize
// Desc: Initializes the synth proxy
//-----------------------------------------------------------------------------
HRESULT RCmdSynthInitialize(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	RCmdSynthUninitialize( argc, argv, szResp, cchResp, pdmcc );

    g_xbApp.AddConnection(XBOX_SYNTH_NAME);

	g_xbApp.m_fMIDICounter = 0.0;
	g_xbApp.m_bSynthInitialized = TRUE;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RCmdSynthUninitialize
// Desc: Uninitializes the synth proxy
//-----------------------------------------------------------------------------
HRESULT RCmdSynthUninitialize(int argc, char *argv[], LPSTR szResp, DWORD cchResp, PDM_CMDCONT pdmcc)
{
	while( !g_xbApp.m_lstDownloads.IsEmpty() )
	{
		CDownloadItem *pDownloadItem = (CDownloadItem *)g_xbApp.m_lstDownloads.RemoveHead();

		// Release the download buffer
//		if( pDownloadItem->m_pIDirectMusicDownload )
		{
			// Unload the download buffer
			UnloadFromDefaultPort( pDownloadItem );

//			pDownloadItem->m_pIDirectMusicDownload->Release();
//			pDownloadItem->m_pIDirectMusicDownload = NULL;
		}

		// Delete the item
		delete pDownloadItem;
	}

    g_xbApp.RemoveConnection(XBOX_SYNTH_NAME);

	g_xbApp.m_bSynthInitialized = FALSE;

	return S_OK;
}
