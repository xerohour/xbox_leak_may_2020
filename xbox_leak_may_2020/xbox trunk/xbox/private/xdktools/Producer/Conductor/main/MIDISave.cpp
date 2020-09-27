#include "stdafx.h"
#include "MIDISave.h"
#include "resource.h"


static writevarlen( FILE *file, LONG value )

{
    long buffer ;
    char out ;

    buffer = value & 0x7F ;

    while( (value >>= 7) > 0 ) {
        buffer <<= 8 ;
        buffer |= 0x80 ;
        buffer += (value & 0x7F) ;
    }

    for( ; ; ) {
        out = (char)buffer ;
        if( fwrite(&out,1,1,file) < 1 )
            return( 1 ) ;
        if( buffer & 0x80 )
            buffer >>= 8 ;
        else
            break ;
    }

    return( 0 ) ;

} 


static long calcvarlen( long value )

{
    long lLen = 1 ;

    while( (value >>= 7) > 0 )
    {
        lLen++ ;
    }

    return( lLen ) ;

}   

static writebyte( FILE *file, unsigned char c )

{
    return( fwrite(&c,1,1,file) < 1 ) ;
}   

static writeword( FILE *file, unsigned short w )

{
    writebyte( file, (unsigned char) ((unsigned short)(w >> 8)));
    return( writebyte( file, (unsigned char) w )) ;
}   

static writelong( FILE *file, long l )

{
    writeword( file, (unsigned short) ((long)(l >> 16)));
    return( writeword( file, (unsigned short) l)) ;
}

DWORD MIDIList::CalcTrack( MUSIC_TIME mtEnd )

{
    MIDIEvent *     pEvent;
    unsigned char   bLaststatus = 0;
    DWORD           dwTotal = 0;
    MUSIC_TIME      mtLasttime = 0;

    if (m_pName)
    {
        dwTotal = calcvarlen( strlen(m_pName) );
        dwTotal += strlen(m_pName);
        dwTotal += 3;
    }

    for(pEvent = GetHead();pEvent;pEvent=pEvent->GetNext() )
    {
        if( pEvent->m_mtTime < 0 ) pEvent->m_mtTime = 0;
        dwTotal   += calcvarlen( pEvent->m_mtTime - mtLasttime );
        mtLasttime = pEvent->m_mtTime;
        {
            if( pEvent->m_bStatus != bLaststatus )
            {
                dwTotal++;
                bLaststatus = pEvent->m_bStatus;
            }
            if ((bLaststatus & 0xF0) == 0xF0) 
            {
                bLaststatus = 0;
            }
			// If a SysEx event
			if( pEvent->m_bStatus == MIDI_SYSX )
			{
				// Add the bytes used for storing the SysEx event length
				ASSERT( pEvent->m_bLength < 128 );
				dwTotal++;
			}
            dwTotal += pEvent->m_bLength;
        }
    }

    if( mtEnd < mtLasttime )
        dwTotal++;
    else
        dwTotal += calcvarlen( mtEnd - mtLasttime );

    return( dwTotal + 3L );

}   

#define ID_MTHD     0x4D546864
#define ID_MTRK     0x4D54726B

HRESULT MIDIList::SaveTrack( FILE *file, MUSIC_TIME mtEnd )
{
    MIDIEvent *     pEvent;
    MUSIC_TIME      mtLasttime = 0;
    DWORD           dwId       = ID_MTRK;  
    unsigned char   bLaststatus = 0;

    writelong( file, dwId );    // Track header
    dwId = CalcTrack( mtEnd );  
    writelong( file, dwId );    // Length of track chunk
    if (m_pName)
    {
        writebyte( file, 0 );   // 0 time until track name 
        writebyte( file, 0xFF );
        writebyte( file, 3 );
        writevarlen( file, strlen(m_pName) );
        fwrite( m_pName, strlen(m_pName), 1, file );
    }
    for(pEvent = GetHead();pEvent;pEvent=pEvent->GetNext() )
    {
        if( writevarlen(file,pEvent->m_mtTime - mtLasttime) )
        {
            return( E_FAIL );
        }
        mtLasttime = pEvent->m_mtTime;
        {
            if (pEvent->m_bStatus != bLaststatus)
            {
                bLaststatus = pEvent->m_bStatus;
                fwrite( &pEvent->m_bStatus, 1, 1, file );
            }
            if ((bLaststatus & 0xF0) == 0xF0) 
            {
                bLaststatus = 0;
            }
			if( pEvent->m_bStatus == MIDI_SYSX )
			{
				// Need to write length of SysEx data chunk
				if( writevarlen(file,pEvent->m_bLength) )
				{
					return( E_FAIL );
				}
			}
			fwrite( &pEvent->m_bData[0], pEvent->m_bLength, 1, file );
        }
    }

    if( mtEnd < mtLasttime )
        writevarlen( file, 0 );
    else
        writevarlen( file, mtEnd - mtLasttime );

    writebyte( file, 0xFF );
    writebyte( file, 0x2F );
    writebyte( file, 0x0 );

    return( 0 );
}   

static MIDIEvent * sortevents( MIDIEvent * pEvents, long lLen )

{
    MIDIEvent * pLeft;
    MIDIEvent * pRight ;
    long        lCut ;
    MIDIEvent * pTop ;

    if( lLen < 3 )
    {
        if( !pEvents )
            return( 0 ) ;
        if( lLen == 1 )
            return( pEvents ) ;
        pLeft  = pEvents ;
        pRight = pEvents->GetNext() ;
        if( !pRight )
            return( pLeft ) ;
        if( (pLeft->m_mtTime > pRight->m_mtTime)
		|| ((pLeft->m_mtTime == pRight->m_mtTime) && ((pRight->m_bStatus & 0xF0) != MIDI_NOTEON)) )
        {
            pLeft->SetNext(NULL) ;
            pRight->SetNext(pLeft) ;
            return( pRight ) ;
        }
        return( pLeft ) ;
    }

    lCut    = lLen >> 1 ;
    pLeft   = pEvents ;
    pEvents = pEvents->GetItem( lCut-1 ) ;
    pRight  = sortevents( pEvents->GetNext(), lLen-lCut ) ;
    pEvents->SetNext(NULL) ;
    pLeft   = sortevents( pLeft, lCut ) ;
    pTop    = NULL ;

    for( ;  pLeft && pRight ;  )
    {
        if( (pLeft->m_mtTime < pRight->m_mtTime)
		|| ((pLeft->m_mtTime == pRight->m_mtTime) && ((pLeft->m_bStatus & 0xF0) != MIDI_NOTEON)) )
        {
            if( !pTop )
                pTop = pLeft ;
            else
                pEvents->SetNext(pLeft) ;
            pEvents = pLeft ;
            pLeft   = pEvents->GetNext() ;
        }
        else
        {
            if( !pTop )
                pTop = pRight ;
            else
                pEvents->SetNext(pRight) ;
            pEvents = pRight ;
            pRight  = pEvents->GetNext() ;
        }
    }

    if( pLeft )
        pEvents->SetNext(pLeft) ;
    else
        pEvents->SetNext(pRight) ;

    return( pTop ) ;

}   

void MIDIList::Sort() 

{
    m_pHead = sortevents(GetHead(), GetCount()) ;
}   

void MIDIList::Clear()

{
    MIDIEvent *pEvent = RemoveHead();
    while (pEvent)
    {
        delete pEvent;
		pEvent = RemoveHead();
    }
    m_nTranspose = 0;
}

MIDIList::MIDIList()

{
    m_nTranspose = 0;
    m_pName = NULL;
}

MIDIList::~MIDIList()

{
    if (m_pName) delete m_pName;
    Clear();
}

void MIDIList::SetName(char *pName)

{
    if (pName)
    {
        if (m_pName) delete m_pName;
        m_pName = new char[strlen(pName)+1];
        if (m_pName) strcpy(m_pName,pName);
    }
}

DWORD MIDIList::PrepToSave(MUSIC_TIME mtStartOffset, MUSIC_TIME *pmtLength)

{
    MIDIEvent *pEvent = GetHead();
    if (!pEvent) return 0;
    for (;pEvent;pEvent = pEvent->GetNext())
    {
        pEvent->m_mtTime -= mtStartOffset;
        if (pEvent->m_mtTime > *pmtLength)
        {
            *pmtLength = pEvent->m_mtTime;
        }
    }
    Sort();
    return 1;
}

HRESULT MIDISaveTool::SaveMIDISong( FILE *file, MUSIC_TIME mtLength, DWORD dwTracks)
{
    long    dwID = ID_MTHD ;

    if( writelong(file,dwID) )
        return( E_FAIL ) ;

    dwID = 6;
    writelong( file, dwID ) ;   // Length of header  
    writeword( file, 1 ) ;      // MIDI format 1 
    writeword( file, (unsigned short)(1 + dwTracks) ) ; // Number of tracks
    writeword( file, 768 ) ;    // PPQ

    if(FAILED( m_ControlTrack.SaveTrack(file,mtLength) ))
        return( E_FAIL ) ;
    DWORD dwTrack;
    if (m_ppChannelTracks)
    {
        for (dwTrack = 0; dwTrack < m_dwChannels; dwTrack++)
        {
            if (m_ppChannelTracks[dwTrack] && 
                m_ppChannelTracks[dwTrack]->GetCount())
            {
                if (FAILED(m_ppChannelTracks[dwTrack]->SaveTrack(file,mtLength) ))
                    return( E_FAIL ) ;
            }
        }
    }
    return S_OK;
}   

HRESULT MIDISaveTool::SaveMIDIFile(LPCTSTR pFileName, 
                                   IDirectMusicSegmentState *pState, 
                                   BOOL fExtraMeasure,
                                   BOOL fRMID)

{
    FILE *pFile;
    MUSIC_TIME mtLength = 0;
    DWORD dwIndex;
    DWORD dwTrackCount = 0;
    MUSIC_TIME mtStartOffset;
	HRESULT hr = S_OK;

    if (!pState)
    {
        return E_FAIL;
    }
    EnterCriticalSection(&m_CrSec);
    StopRecording();
    pState->GetStartTime(&mtStartOffset);
    if (fExtraMeasure) // Insert a measure at start and give it the time signature.
    {
        mtStartOffset -= ( DMUS_PPQ * 4 );
        MIDIEvent *pEvent = new MIDIEvent;
        if (pEvent)
        {
            pEvent->m_bStatus = 0xFF;   // Meta event marker.
            pEvent->m_bData[0] = 0x58;  // Time Signature
            pEvent->m_bData[1] = 0x4;   // 4 bytes to follow.
            pEvent->m_bData[2] = 4;     // 4 beats per measure.
            pEvent->m_bData[3] = 2;     // Quarter note.
            pEvent->m_bData[4] = 24;
            pEvent->m_bData[5] = 8;
            pEvent->m_mtTime = mtStartOffset;
            pEvent->m_bLength = 6;
            m_ControlTrack.AddHead(pEvent);
        }
    }
    if (m_ppChannelTracks)
    {
        for (dwIndex = 0;dwIndex < m_dwChannels; dwIndex++)
        {
            if (m_ppChannelTracks[dwIndex])
            {
                dwTrackCount += m_ppChannelTracks[dwIndex]->PrepToSave(mtStartOffset,&mtLength);
            }
        }
    }
    m_ControlTrack.PrepToSave(mtStartOffset,&mtLength);   
    pFile = fopen( pFileName, "wb" ) ;
    if( pFile > 0 )
    {
        if (fRMID)
        {
            fwrite("RIFF    RMIDdata    ",20,1,pFile);
        }
        SaveMIDISong( pFile, mtLength, dwTrackCount);
        if (fRMID)
        {
            long lEndPos = ftell( pFile );
            lEndPos = (lEndPos+1) & 0xFFFFFFFE;
            fseek(pFile,4,SEEK_SET);
            lEndPos -= 8;
            fwrite(&lEndPos,4,1,pFile);
            fseek(pFile,16,SEEK_SET);
            lEndPos -= 12;
            fwrite(&lEndPos,4,1,pFile);
        }
        fclose( pFile ) ;
    }
    LeaveCriticalSection(&m_CrSec);

	if( pFile <= 0 )
	{
		CString strMsg;
		LPVOID lpMessageBuffer;
		
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					   NULL, GetLastError(),
					   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
					   (LPTSTR)&lpMessageBuffer, 0, NULL );

		HINSTANCE hInstanceOld = AfxGetResourceHandle();
		AfxSetResourceHandle( _Module.GetResourceInstance() );

		AfxFormatString1( strMsg, IDS_SYSERR_SAVE, pFileName );
		strMsg = strMsg + (LPTSTR)lpMessageBuffer;
		AfxMessageBox( strMsg );

		AfxSetResourceHandle( hInstanceOld );

		LocalFree( lpMessageBuffer );
		hr = E_FAIL;
	}

    return hr;
}

HRESULT MIDISaveTool::SetChannelName(DWORD dwTrack, char *pName)

{
    if (dwTrack < m_dwChannels)
    {
        if (m_ppChannelTracks && m_ppChannelTracks[dwTrack])
        {
            m_ppChannelTracks[dwTrack]->SetName( pName );
            return S_OK;
        }
    }
    return E_FAIL;
}


HRESULT MIDISaveTool::SetChannels(DWORD dwChannels)

{
    DWORD dwTrack;
    EnterCriticalSection(&m_CrSec);
    if (m_dwChannels != dwChannels)
    {
        MIDIList ** ppChannelTracks = new MIDIList *[dwChannels];
		if (ppChannelTracks)
		{
			for (dwTrack = 0; dwTrack < dwChannels; dwTrack++)
			{
				ppChannelTracks[dwTrack] = NULL;
			}
			if (m_dwChannels < dwChannels)
			{
				for (dwTrack = 0; dwTrack < m_dwChannels; dwTrack++)
				{
					ppChannelTracks[dwTrack] = m_ppChannelTracks[dwTrack];
				}
				for (;dwTrack < dwChannels; dwTrack++)
				{
				    ppChannelTracks[dwTrack] = new MIDIList;
					if (ppChannelTracks[dwTrack])
					{
                        char strName[20];
                        wsprintf( strName, "Track %ld",dwTrack+1);
                        ppChannelTracks[dwTrack]->SetName(strName);
                    }
				}
			}
			else
			{
				for (dwTrack = 0; dwTrack < dwChannels; dwTrack++)
				{
					ppChannelTracks[dwTrack] = m_ppChannelTracks[dwTrack];
				}
				for (; dwTrack < m_dwChannels; dwTrack++)
				{
					if (m_ppChannelTracks[dwTrack])
					{
						delete m_ppChannelTracks[dwTrack];
					}
				}
			}
			if (m_ppChannelTracks)
			{
				delete m_ppChannelTracks;
			}
		}
        else 
        {
            dwChannels = 0;
        }
	    m_ppChannelTracks = ppChannelTracks;
	    m_dwChannels = dwChannels;    
    }
    LeaveCriticalSection(&m_CrSec);
    return S_OK;
}

HRESULT MIDISaveTool::StartRecording()

{
    DWORD dwTrack;
    EnterCriticalSection(&m_CrSec);
    if (m_ppChannelTracks)
    {
        for (dwTrack = 0; dwTrack < m_dwChannels; dwTrack++)
        {
            if (m_ppChannelTracks[dwTrack])
            {
                m_ppChannelTracks[dwTrack]->Clear();
            }
        }
    }
    m_ControlTrack.Clear();
    m_fRecordEnabled = TRUE;
    LeaveCriticalSection(&m_CrSec);
    return S_OK;
}

HRESULT MIDISaveTool::StopRecording()

{
    m_fRecordEnabled = FALSE;
    return S_OK;
}

MIDISaveTool::MIDISaveTool()
{
    m_fRecordEnabled = FALSE;
    m_cRef = 1; // set to 1 so one call to Release() will free this
    InitializeCriticalSection(&m_CrSec);
    m_ControlTrack.SetName("Control Track");
    m_dwChannels = 0;
    m_ppChannelTracks = NULL;
}

MIDISaveTool::~MIDISaveTool()
{
    DWORD dwTrack;
    if (m_ppChannelTracks)
    {
        for (dwTrack = 0; dwTrack < m_dwChannels; dwTrack++)
        {
            if (m_ppChannelTracks[dwTrack])
            {
                delete m_ppChannelTracks[dwTrack];
            }
        }
        delete m_ppChannelTracks;
    }
    DeleteCriticalSection(&m_CrSec);
}


STDMETHODIMP MIDISaveTool::QueryInterface(const IID &iid, void **ppv)
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
    
    static_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) MIDISaveTool::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) MIDISaveTool::Release()
{
    if( 0 == InterlockedDecrement(&m_cRef) )
    {
        delete this;
        return 0;
    }

    return m_cRef;
}

/////////////////////////////////////////////////////////////////
// IDirectMusicTool

HRESULT STDMETHODCALLTYPE MIDISaveTool::Init( IDirectMusicGraph* pGraph )
{
 	UNREFERENCED_PARAMETER(pGraph);

   // This tool has no need to do any type of initialization.
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE MIDISaveTool::GetMsgDeliveryType( DWORD* pdwDeliveryType )
{
    // This tool wants messages immediately.
    // This is the default, so returning E_NOTIMPL
    // would work. The other method is to specifically
    // set *pdwDeliveryType to the delivery type, DMUS_PMSGF_TOOL_IMMEDIATE,
    // DMUS_PMSGF_TOOL_QUEUE, or DMUS_PMSGF_TOOL_ATTIME.
    
    *pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE MIDISaveTool::GetMediaTypeArraySize( DWORD* pdwNumElements )
{
    *pdwNumElements = 8;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE MIDISaveTool::GetMediaTypes( DWORD** padwMediaTypes, 
                                                    DWORD dwNumElements )
{
    if( dwNumElements == 8 )
    {
        (*padwMediaTypes)[0] = DMUS_PMSGT_NOTE;
        (*padwMediaTypes)[1] = DMUS_PMSGT_MIDI;
        (*padwMediaTypes)[2] = DMUS_PMSGT_TEMPO;
        (*padwMediaTypes)[3] = DMUS_PMSGT_CURVE;
        (*padwMediaTypes)[4] = DMUS_PMSGT_TIMESIG;
        (*padwMediaTypes)[5] = DMUS_PMSGT_PATCH;
        (*padwMediaTypes)[6] = DMUS_PMSGT_TRANSPOSE;
        (*padwMediaTypes)[7] = DMUS_PMSGT_SYSEX;

        return S_OK;
    }
    else
    {
        // this should never happen
        return E_FAIL;
    }
}

// curve.h

#define CT_MAX  192 // curve table maximum
#define CT_FACTOR	100	// curve table multiplication factor
#define CT_DIVFACTOR	( ( CT_MAX ) * CT_FACTOR ) // curve division factor

// linear curves
static short ganCT_Linear[CT_MAX + 1] = {
	0,100,200,300,400,500,600,700,
	800,900,1000,1100,1200,1300,1400,1500,
	1600,1700,1800,1900,2000,2100,2200,2300,
	2400,2500,2600,2700,2800,2900,3000,3100,
	3200,3300,3400,3500,3600,3700,3800,3900,
	4000,4100,4200,4300,4400,4500,4600,4700,
	4800,4900,5000,5100,5200,5300,5400,5500,
	5600,5700,5800,5900,6000,6100,6200,6300,
	6400,6500,6600,6700,6800,6900,7000,7100,
	7200,7300,7400,7500,7600,7700,7800,7900,
	8000,8100,8200,8300,8400,8500,8600,8700,
	8800,8900,9000,9100,9200,9300,9400,9500,
	9600,9700,9800,9900,10000,10100,10200,10300,
	10400,10500,10600,10700,10800,10900,11000,11100,
	11200,11300,11400,11500,11600,11700,11800,11900,
	12000,12100,12200,12300,12400,12500,12600,12700,
	12800,12900,13000,13100,13200,13300,13400,13500,
	13600,13700,13800,13900,14000,14100,14200,14300,
	14400,14500,14600,14700,14800,14900,15000,15100,
	15200,15300,15400,15500,15600,15700,15800,15900,
	16000,16100,16200,16300,16400,16500,16600,16700,
	16800,16900,17000,17100,17200,17300,17400,17500,
	17600,17700,17800,17900,18000,18100,18200,18300,
	18400,18500,18600,18700,18800,18900,19000,19100,19200 
};

// sine curves
static short ganCT_Sine[CT_MAX + 1] = {
	0,1,5,11,20,32,46,62,
	82,103,128,155,184,216,250,287,
	327,369,413,460,509,561,615,671,
	730,792,855,921,990,1060,1133,1208,
	1286,1365,1447,1531,1617,1706,1796,1889,
	1983,2080,2179,2279,2382,2486,2593,2701,
	2811,2923,3037,3153,3270,3389,3509,3632,
	3755,3881,4008,4136,4266,4397,4530,4664,
	4799,4936,5074,5213,5354,5495,5638,5781,
	5926,6071,6218,6365,6514,6663,6813,6963,
	7115,7267,7420,7573,7727,7881,8036,8191,
	8346,8502,8659,8815,8972,9128,9285,9442,
	9600,9757,9914,10071,10227,10384,10540,10697,
	10853,11008,11163,11318,11472,11626,11779,11932,
	12084,12236,12386,12536,12685,12834,12981,13128,
	13273,13418,13561,13704,13845,13986,14125,14263,
	14399,14535,14669,14802,14933,15063,15191,15318,
	15444,15567,15690,15810,15929,16046,16162,16276,
	16388,16498,16606,16713,16817,16920,17020,17119,
	17216,17310,17403,17493,17582,17668,17752,17834,
	17913,17991,18066,18139,18209,18278,18344,18407,
	18469,18528,18584,18638,18690,18739,18786,18830,
	18872,18912,18949,18983,19015,19044,19071,19096,
	19117,19137,19153,19167,19179,19188,19194,19198,19200 
};

// log curve
static short ganCT_Log[CT_MAX + 1] = {
	0,381,747,1097,1432,1755,2066,2366,
	2655,2934,3205,3467,3721,3967,4207,4439,
	4666,4886,5101,5310,5515,5714,5909,6099,
	6285,6467,6645,6819,6990,7157,7321,7482,
	7640,7795,7947,8096,8243,8387,8529,8668,
	8805,8940,9073,9204,9332,9459,9584,9707,
	9828,9947,10065,10181,10295,10408,10520,10630,
	10738,10845,10951,11056,11159,11261,11361,11461,
	11559,11656,11752,11847,11941,12034,12126,12216,
	12306,12395,12483,12570,12656,12741,12826,12909,
	12992,13074,13155,13235,13315,13394,13472,13549,
	13626,13702,13777,13851,13925,13998,14071,14143,
	14214,14285,14355,14425,14494,14562,14630,14698,
	14764,14831,14896,14962,15026,15091,15154,15218,
	15280,15343,15405,15466,15527,15587,15647,15707,
	15766,15825,15883,15941,15999,16056,16113,16169,
	16225,16281,16336,16391,16446,16500,16554,16607,
	16661,16713,16766,16818,16870,16921,16973,17024,
	17074,17124,17174,17224,17273,17323,17371,17420,
	17468,17516,17564,17611,17658,17705,17752,17798,
	17844,17890,17936,17981,18026,18071,18116,18160,
	18204,18248,18292,18335,18379,18422,18464,18507,
	18549,18592,18634,18675,18717,18758,18799,18840,
	18881,18921,18962,19002,19042,19081,19121,19160,19200 
};

// exponential curve
static short ganCT_Exp[CT_MAX + 1] = {
	0,40,79,119,158,198,238,279,
	319,360,401,442,483,525,566,608,
	651,693,736,778,821,865,908,952,
	996,1040,1084,1129,1174,1219,1264,1310,
	1356,1402,1448,1495,1542,1589,1636,1684,
	1732,1780,1829,1877,1927,1976,2026,2076,
	2126,2176,2227,2279,2330,2382,2434,2487,
	2539,2593,2646,2700,2754,2809,2864,2919,
	2975,3031,3087,3144,3201,3259,3317,3375,
	3434,3493,3553,3613,3673,3734,3795,3857,
	3920,3982,4046,4109,4174,4238,4304,4369,
	4436,4502,4570,4638,4706,4775,4845,4915,
	4986,5057,5129,5202,5275,5349,5423,5498,
	5574,5651,5728,5806,5885,5965,6045,6126,
	6208,6291,6374,6459,6544,6630,6717,6805,
	6894,6984,7074,7166,7259,7353,7448,7544,
	7641,7739,7839,7939,8041,8144,8249,8355,
	8462,8570,8680,8792,8905,9019,9135,9253,
	9372,9493,9616,9741,9868,9996,10127,10260,
	10395,10532,10671,10813,10957,11104,11253,11405,
	11560,11718,11879,12043,12210,12381,12555,12733,
	12915,13101,13291,13486,13685,13890,14099,14314,
	14534,14761,14993,15233,15479,15733,15995,16266,
	16545,16834,17134,17445,17768,18103,18453,18819,19200 
};

static long ComputeCurveTimeSlice(DMUS_CURVE_PMSG* pCurve)
{
    long lTimeIncrement;
    DWORD dwTotalDistance;
    DWORD dwResolution;
    if ((pCurve->bType == DMUS_CURVET_PBCURVE) ||
        (pCurve->bType == DMUS_CURVET_RPNCURVE) ||
        (pCurve->bType == DMUS_CURVET_NRPNCURVE))
    {
        dwResolution = 100;
    }
    else
    {
        dwResolution = 3;
    }
    if (pCurve->nEndValue > pCurve->nStartValue)
        dwTotalDistance = pCurve->nEndValue - pCurve->nStartValue;
    else 
        dwTotalDistance = pCurve->nStartValue - pCurve->nEndValue;
    if (dwTotalDistance == 0) dwTotalDistance = 1;
    lTimeIncrement = (pCurve->mtDuration * dwResolution) / dwTotalDistance;
    // Force to no smaller than 192nd note (10ms at 120 bpm.)
    if( lTimeIncrement < (DMUS_PPQ/48) ) lTimeIncrement = DMUS_PPQ/48;
    return lTimeIncrement;
}

static BOOL ComputeCurve( DMUS_CURVE_PMSG* pCurve, MUSIC_TIME *pmtTime, DWORD *pdwReturnVal  )
{
	short *panTable;
	MUSIC_TIME mtCurrent;
	short nIndex;

	switch( pCurve->bCurveShape )
	{
	case DMUS_CURVES_INSTANT:
	default:
        *pdwReturnVal = (DWORD) pCurve->nEndValue;
		return FALSE;
		break;
	case DMUS_CURVES_LINEAR:
		panTable = &ganCT_Linear[ 0 ];
		break;
	case DMUS_CURVES_EXP:
		panTable = &ganCT_Exp[ 0 ];
		break;
	case DMUS_CURVES_LOG:
		panTable = &ganCT_Log[ 0 ];
		break;
	case DMUS_CURVES_SINE:
		panTable = &ganCT_Sine[ 0 ];
		break;
	}

	// compute index into table
	// there are CT_MAX + 1 elements in the table.
	mtCurrent = *pmtTime - pCurve->mtOriginalStart;
    if( (pCurve->mtDuration == 0) ||
		(*pmtTime - pCurve->mtOriginalStart >= pCurve->mtDuration ))
	{
		*pdwReturnVal = pCurve->nEndValue;
		return FALSE;
	}
	else
	{
		//dblRes = (double)pCurve->mtDuration / (CT_MAX + 1);
		nIndex = short((mtCurrent * (CT_MAX + 1)) / pCurve->mtDuration);

        // find an amount of time to add to the curve event such that there is at
        // least a change by CT_FACTOR. This will be used as the time stamp
        // for the next iteration of the curve.

		// clamp nIndex
		if( nIndex < 0 )
		{
			nIndex = 0;
		}
		if( nIndex >= CT_MAX )
		{
			nIndex = CT_MAX;
			*pdwReturnVal = pCurve->nEndValue;
			return FALSE;
		}
		else
		{
            // Okay, in the curve, so calculate the return value.
            *pdwReturnVal = ((panTable[nIndex] * (pCurve->nEndValue - pCurve->nStartValue)) / 
                CT_DIVFACTOR) + pCurve->nStartValue;
		}
		*pmtTime += ComputeCurveTimeSlice( pCurve );
		if( *pmtTime > pCurve->mtDuration + pCurve->mtOriginalStart )
		{
			*pmtTime = pCurve->mtDuration + pCurve->mtOriginalStart;
		}
	}

	return TRUE;
}

HRESULT STDMETHODCALLTYPE MIDISaveTool::ProcessPMsg( IDirectMusicPerformance* pPerf, 
                                                  DMUS_PMSG* pPMsg )
{
	UNREFERENCED_PARAMETER(pPerf);

    if( pPMsg->pGraph == NULL )
    {
        return DMUS_S_FREE;
    }

    EnterCriticalSection(&m_CrSec);
    if (m_fRecordEnabled && (pPMsg->dwPChannel < m_dwChannels) 
        && m_ppChannelTracks && m_ppChannelTracks[pPMsg->dwPChannel])
    {
        if( pPMsg->dwType == DMUS_PMSGT_MIDI )
        {
            DMUS_MIDI_PMSG * pMIDI;
            pMIDI = (DMUS_MIDI_PMSG*)pPMsg;
            MIDIEvent *pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = (BYTE) 
                    ((pMIDI->bStatus & 0xF0) | (pMIDI->dwPChannel & 0xF));
                switch( pMIDI->bStatus & 0xF0 )
                {
                    case MIDI_NOTEON:
                    case MIDI_NOTEOFF:
                    case MIDI_PTOUCH:
                    case MIDI_CCHANGE:
                    case MIDI_PBEND:
                        pEvent->m_bLength = 2;
                        break;
                    default :
                        pEvent->m_bLength = 1;
                }
                pEvent->m_bData[0] = pMIDI->bByte1;
                pEvent->m_bData[1] = pMIDI->bByte2;
                pEvent->m_mtTime = pMIDI->mtTime;
                m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
            }
        }
        else if( pPMsg->dwType == DMUS_PMSGT_NOTE )
        {
	        DMUS_NOTE_PMSG* pNote;
            pNote = (DMUS_NOTE_PMSG*)pPMsg;
            MIDIEvent *pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = (BYTE) 
                    (MIDI_NOTEON | (pNote->dwPChannel & 0xF));
                pEvent->m_bData[0] = (BYTE)(pNote->bMidiValue +
                    m_ppChannelTracks[pPMsg->dwPChannel]->m_nTranspose);
                pEvent->m_bData[1] = pNote->bVelocity;
                pEvent->m_mtTime = pNote->mtTime;
                pEvent->m_bLength = 2;
                m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
            }
            pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = (BYTE) 
                    (MIDI_NOTEOFF | (pNote->dwPChannel & 0xF));
                pEvent->m_bData[0] = (BYTE)(pNote->bMidiValue +
                    m_ppChannelTracks[pPMsg->dwPChannel]->m_nTranspose);
                pEvent->m_bData[1] = 0;
                pEvent->m_mtTime = pNote->mtTime + pNote->mtDuration;
                pEvent->m_bLength = 2;
                m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
            }
        }
        else if( pPMsg->dwType == DMUS_PMSGT_TRANSPOSE )
        {
            DMUS_TRANSPOSE_PMSG* pTranspose;
            pTranspose = (DMUS_TRANSPOSE_PMSG*)pPMsg;
            m_ppChannelTracks[pPMsg->dwPChannel]->m_nTranspose = 
                pTranspose->nTranspose;
        }
        else if( pPMsg->dwType == DMUS_PMSGT_PATCH )
        {
	        DMUS_PATCH_PMSG* pPatch;
            pPatch = (DMUS_PATCH_PMSG*)pPMsg;
            MIDIEvent *pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = (BYTE)
                    (MIDI_CCHANGE | (pPatch->dwPChannel & 0xF));
                pEvent->m_bData[0] = MIDI_CC_BS_LSB;
                pEvent->m_bData[1] = pPatch->byLSB;
                pEvent->m_mtTime = pPatch->mtTime - 2;
                pEvent->m_bLength = 2;
                m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
            }
            pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = (BYTE)
                    (MIDI_CCHANGE | (pPatch->dwPChannel & 0xF));
                pEvent->m_bData[0] = MIDI_CC_BS_MSB;
                pEvent->m_bData[1] = pPatch->byMSB;
                pEvent->m_mtTime = pPatch->mtTime - 1;
                pEvent->m_bLength = 2;
                m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
            }
            pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = (BYTE)
                    (MIDI_PCHANGE | (pPatch->dwPChannel & 0xF));
                pEvent->m_bData[0] = pPatch->byInstrument;
                pEvent->m_mtTime = pPatch->mtTime;
                pEvent->m_bLength = 1;
                m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
            }
        }
        else if( pPMsg->dwType == DMUS_PMSGT_CURVE )
        {
            DMUS_CURVE_PMSG* pCurve;
            pCurve = (DMUS_CURVE_PMSG*) pPMsg;
            MUSIC_TIME mtTime = pCurve->mtTime;
            pCurve->mtOriginalStart = pCurve->mtTime;
            BOOL fKeepGoing = TRUE;
			DWORD dwLastValue = LONG_MAX;
            for (;fKeepGoing;)
            {
				// Save the current time
				MUSIC_TIME mtOldTime = mtTime;

				// Compute the new value, new time, and whether to keep going or not
                DWORD dwValue;
                fKeepGoing = ComputeCurve( pCurve, &mtTime, &dwValue);

				// Ensure we don't repeat the same value
				if( dwValue == dwLastValue )
				{
					continue;
				}
				dwLastValue = dwValue;

				// Create a new event
                MIDIEvent *pEvent = new MIDIEvent;
                if (pEvent)
                {
	                pEvent->m_mtTime = mtOldTime;
                    m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
                    switch( pCurve->bType )
        	        {
	                case DMUS_CURVET_PBCURVE:
                        pEvent->m_bStatus = (BYTE) 
                            (MIDI_PBEND | (pPMsg->dwPChannel & 0xF));
                        pEvent->m_bData[0] = (BYTE) (dwValue & 0x7F);
                        dwValue >>= 7;
                        pEvent->m_bData[1] = (BYTE) (dwValue & 0x7F);
                        pEvent->m_bLength = 2;
		                break;
	                case DMUS_CURVET_CCCURVE:
                        pEvent->m_bStatus = (BYTE) 
                            (MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
                        pEvent->m_bData[0] = pCurve->bCCData;
                        pEvent->m_bData[1] = (BYTE) (dwValue & 0x7F);
                        pEvent->m_bLength = 2;
		                break;
	                case DMUS_CURVET_MATCURVE:
                        pEvent->m_bStatus = (BYTE) 
                            (MIDI_MTOUCH | (pPMsg->dwPChannel & 0xF));
                        pEvent->m_bData[0] = (BYTE) (dwValue & 0x7F);
                        pEvent->m_bLength = 1;
		                break;
	                case DMUS_CURVET_PATCURVE:
		                pEvent->m_bStatus = (BYTE) 
                            (MIDI_PTOUCH | (pPMsg->dwPChannel & 0xF));
                        pEvent->m_bData[0] = pCurve->bCCData;
                        pEvent->m_bData[1] = (BYTE) (dwValue & 0x7F);
                        pEvent->m_bLength = 2;
		                break;
					case DMUS_CURVET_RPNCURVE:
						pEvent->m_mtTime = mtOldTime - 3;
		                pEvent->m_bStatus = (BYTE) 
                            (MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
                        pEvent->m_bData[0] = MIDI_CC_RPN_LSB;
                        pEvent->m_bData[1] = (BYTE) (pCurve->wParamType & 0x7F);
                        pEvent->m_bLength = 2;
						pEvent = new MIDIEvent;
						if (pEvent)
						{
							pEvent->m_mtTime = mtOldTime - 2;
							m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
							pEvent->m_bStatus = (BYTE) 
								(MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
							pEvent->m_bData[0] = MIDI_CC_RPN_MSB;
							pEvent->m_bData[1] = (BYTE) ((pCurve->wParamType >> 7) & 0x7F);
							pEvent->m_bLength = 2;
							pEvent = new MIDIEvent;
							if (pEvent)
							{
								pEvent->m_mtTime = mtOldTime - 1;
								m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
								pEvent->m_bStatus = (BYTE) 
									(MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
								pEvent->m_bData[0] = MIDI_CC_DATA_MSB;
								pEvent->m_bData[1] = (BYTE) ((dwValue >> 7) & 0x7F);
								pEvent->m_bLength = 2;
								pEvent = new MIDIEvent;
								if (pEvent)
								{
									pEvent->m_mtTime = mtOldTime;
									m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
									pEvent->m_bStatus = (BYTE) 
										(MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
									pEvent->m_bData[0] = MIDI_CC_DATA_LSB;
									pEvent->m_bData[1] = (BYTE) (dwValue & 0x7F);
									pEvent->m_bLength = 2;
								}
							}
						}
						break;
					case DMUS_CURVET_NRPNCURVE:
						pEvent->m_mtTime = mtOldTime - 3;
		                pEvent->m_bStatus = (BYTE) 
                            (MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
                        pEvent->m_bData[0] = MIDI_CC_NRPN_LSB;
                        pEvent->m_bData[1] = (BYTE) (pCurve->wParamType & 0x7F);
                        pEvent->m_bLength = 2;
						pEvent = new MIDIEvent;
						if (pEvent)
						{
							pEvent->m_mtTime = mtOldTime - 2;
							m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
							pEvent->m_bStatus = (BYTE) 
								(MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
							pEvent->m_bData[0] = MIDI_CC_NRPN_MSB;
							pEvent->m_bData[1] = (BYTE) ((pCurve->wParamType >> 7) & 0x7F);
							pEvent->m_bLength = 2;
							pEvent = new MIDIEvent;
							if (pEvent)
							{
								pEvent->m_mtTime = mtOldTime - 1;
								m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
								pEvent->m_bStatus = (BYTE) 
									(MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
								pEvent->m_bData[0] = MIDI_CC_DATA_MSB;
								pEvent->m_bData[1] = (BYTE) ((dwValue >> 7) & 0x7F);
								pEvent->m_bLength = 2;
								pEvent = new MIDIEvent;
								if (pEvent)
								{
									pEvent->m_mtTime = mtOldTime;
									m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
									pEvent->m_bStatus = (BYTE) 
										(MIDI_CCHANGE | (pPMsg->dwPChannel & 0xF));
									pEvent->m_bData[0] = MIDI_CC_DATA_LSB;
									pEvent->m_bData[1] = (BYTE) (dwValue & 0x7F);
									pEvent->m_bLength = 2;
								}
							}
						}
						break;
					default:
						// Unknown curve type - delete the event
	                    delete m_ppChannelTracks[pPMsg->dwPChannel]->RemoveHead();
						ASSERT(FALSE);
						break;
	                }
                }
                else
                {
                    fKeepGoing = FALSE;
                }
            }
            pCurve->mtOriginalStart = 0;
        }
        else if( pPMsg->dwType == DMUS_PMSGT_TEMPO )
        {
	        DMUS_TEMPO_PMSG* pTempo;
            pTempo = (DMUS_TEMPO_PMSG*)pPMsg;
            MIDIEvent *pEvent = new MIDIEvent;
            if (pEvent)
            {
                double dblTempo = 60000000.0 / pTempo->dblTempo;
                DWORD dwTempo = (DWORD) dblTempo;
                pEvent->m_bStatus = 0xFF;   // Meta event marker.
                pEvent->m_bData[0] = 0x51;  // Tempo
                pEvent->m_bData[1] = 0x3;   // 3 bytes to follow.
                pEvent->m_bData[2] = (unsigned char)(dwTempo >> 16L);
                pEvent->m_bData[3] = (unsigned char)(0xFF & (dwTempo >> 8L));
                pEvent->m_bData[4] = (unsigned char)(0xFF & dwTempo);
                pEvent->m_mtTime = pTempo->mtTime;
                pEvent->m_bLength = 5;
                m_ControlTrack.AddHead(pEvent);
            }
        }
        else if( pPMsg->dwType == DMUS_PMSGT_TIMESIG )
        {
	        DMUS_TIMESIG_PMSG* pTimeSig;
            pTimeSig = (DMUS_TIMESIG_PMSG*)pPMsg;
            MIDIEvent *pEvent = new MIDIEvent;
            if (pEvent)
            {
                pEvent->m_bStatus = 0xFF;   // Meta event marker.
                pEvent->m_bData[0] = 0x58;  // Time Signature
                pEvent->m_bData[1] = 0x4;   // 4 bytes to follow.
                pEvent->m_bData[2] = (unsigned char)pTimeSig->bBeatsPerMeasure;
                DWORD dwShift = 0 ;
                DWORD dwMask  = 1 ;
                for( ;  (!(dwMask & pTimeSig->bBeat)) ; dwShift++ )
                {
                    dwMask <<= 1 ;
                }
                pEvent->m_bData[3] = (unsigned char) dwShift;
                dwMask = 768 / pTimeSig->bBeat ;
                pEvent->m_bData[4] = (unsigned char)(dwMask >> 3);
                pEvent->m_bData[5] = 8;
                pEvent->m_mtTime = pTimeSig->mtTime;
                pEvent->m_bLength = 6;
                m_ControlTrack.AddHead(pEvent);
            }
        }
        else if( pPMsg->dwType == DMUS_PMSGT_SYSEX )
        {
			DMUS_SYSEX_PMSG* pSysEx;
			pSysEx = (DMUS_SYSEX_PMSG*)pPMsg;
			if( (pSysEx->dwLen > 1) // Must have at least 0xF0 and 0xF7
			&&	(pSysEx->dwLen <= 13) )
			{
				MIDIEvent *pEvent = new MIDIEvent;
				if (pEvent)
				{
					pEvent->m_bStatus = MIDI_SYSX;
					// Skip the first 0xF0
					ASSERT( pSysEx->abData[0] == 0xF0 );
					memcpy( pEvent->m_bData, &(pSysEx->abData[1]), pSysEx->dwLen - 1 );
					pEvent->m_bLength = (BYTE)(pSysEx->dwLen - 1);
					pEvent->m_mtTime = pSysEx->mtTime;
					m_ppChannelTracks[pPMsg->dwPChannel]->AddHead(pEvent);
				}
			}
       }
    }
    LeaveCriticalSection(&m_CrSec);

	if( FAILED( pPMsg->pGraph->StampPMsg(pPMsg) ) )
    {
        return DMUS_S_FREE;
    }
    return DMUS_S_REQUEUE;
}

HRESULT STDMETHODCALLTYPE MIDISaveTool::Flush( IDirectMusicPerformance* pPerf, 
                                            DMUS_PMSG* pDMUS_PMSG,
                                            REFERENCE_TIME rt)
{
	UNREFERENCED_PARAMETER(pPerf);
	UNREFERENCED_PARAMETER(pDMUS_PMSG);
	UNREFERENCED_PARAMETER(rt);

    // this tool does not need to flush.
    return E_NOTIMPL;
}

