#ifndef PERSONALITYRIFF_H
#define PERSONALITYRIFF_H

#pragma warning(disable:4201)


#define MAKE_TIMESIG( bpm, beat ) ( (long)( ((DWORD)(WORD)beat) | ( ((DWORD)(WORD)bpm) << 16 ) ) )
#define TIMESIG_BEAT( tsig ) ( (short)tsig )
#define TIMESIG_BPM( tsig ) ( (WORD)( ((DWORD)tsig) >> 16 ) )

#define CHORD_KEYDOWN 0x8000/* chord is selected (used in upper bit of ioChordEntryEdit::page) */


// this should match enums in iochord.h
enum
{
//	CSF_KEYDOWN = 	1,	// key currently held down in sjam kybd
//	CSF_INSCALE = 	2,	// member of scale
//	CSF_FLAT =		4,	// display with flat
//	CSF_SIMPLE =	8,	// simple chord, display at top of sjam list
//	CSF_SELECTED = 0x80, // this chord is selected
//	CSF_SIGNPOST = 0x40, // this chord is from a signpost
	CSF_SHIFTED = 0x20	// this chord has been altered from an illegal chord
						// used to make chord unique so that Load subchord database code works correctly
						// (see use in ChordDatabase.cpp)
};


// runtime chunks
#define FOURCC_PERSONALITY	mmioFOURCC('D','M','P','R')
#define FOURCC_IOPERSONALITY		mmioFOURCC('p','e','r','h')
#define FOURCC_GUID					mmioFOURCC('g','u','i','d')
#define FOURCC_SUBCHORD				mmioFOURCC('c','h','d','t')
#define FOURCC_CHORDENTRY			mmioFOURCC('c','h','e','h')
#define FOURCC_SUBCHORDID			mmioFOURCC('s','b','c','n')
#define FOURCC_IONEXTCHORD			mmioFOURCC('n','c','r','d')
#define FOURCC_NEXTCHORDSEQ		  mmioFOURCC('n','c','s','q')
#define FOURCC_IOSIGNPOST			mmioFOURCC('s','p','s','h')
#define FOURCC_CHORDNAME			mmioFOURCC('I','N','A','M')
#define FOURCC_PERSONALITYSTRUCT	mmioFOURCC('p','s','t','r')

// runtime list chunks
#define FOURCC_LISTCHORDENTRY		mmioFOURCC('c','h','o','e')
#define FOURCC_LISTCHORDMAP			mmioFOURCC('c','m','a','p')
#define FOURCC_LISTCHORD			mmioFOURCC('c','h','r','d')
#define FOURCC_LISTCHORDPALETTE		mmioFOURCC('c','h','p','l')
#define FOURCC_LISTCADENCE			mmioFOURCC('c','a','d','e')
#define FOURCC_LISTSIGNPOSTITEM			mmioFOURCC('s','p','s','t')

#define FOURCC_SIGNPOSTLIST		mmioFOURCC('s','p','s','q')


// design time chunks
#define FOURCC_PERSONALITYKEY		 mmioFOURCC('p','k','e','y')
#define FOURCC_PERSONALITYEDIT		mmioFOURCC('p','e','d','t')
#define FOURCC_CHORDENTRYEDIT		mmioFOURCC('c','e','e','d')
#define FOURCC_CHORDEDIT				  mmioFOURCC('c','h','e','d')
#define DM_FOURCC_GUID_CHUNK        mmioFOURCC('g','u','i','d')
#define DM_FOURCC_INFO_LIST	        mmioFOURCC('I','N','F','O')
#define DM_FOURCC_UNFO_LIST	        mmioFOURCC('U','N','F','O')
#define DM_FOURCC_CATEGORY_CHUNK    mmioFOURCC('c','a','t','g')
#define DM_FOURCC_VERSION_CHUNK     mmioFOURCC('v','e','r','s')

// constants
const int MaxSubChords = 4;

// simple riff read/writers
inline HRESULT ReadWord(IDMUSProdRIFFStream* pIRiffStream, WORD& val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Read(&val, sizeof(WORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

inline HRESULT ReadDword(IDMUSProdRIFFStream* pIRiffStream, DWORD& val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Read(&val, sizeof(DWORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

inline HRESULT WriteWord(IDMUSProdRIFFStream* pIRiffStream, WORD val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Write(&val, sizeof(WORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}

inline HRESULT WriteDWord(IDMUSProdRIFFStream* pIRiffStream, DWORD val)
{
	ASSERT(pIRiffStream);
	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT(pIStream);
	if(pIStream)
	{
		HRESULT hr = pIStream->Write(&val, sizeof(DWORD), 0);
		pIStream->Release();
		return hr;
	}
	else
	{
		return E_FAIL;
	}
}


// chunk navigators
class WriteChunk
{
	MMCKINFO m_ck;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	WriteChunk(IDMUSProdRIFFStream* pRiffStream, FOURCC id)
	{
		m_ck.ckid = id;
		m_pRiffStream = pRiffStream;
		m_hr = pRiffStream->CreateChunk( &m_ck, 0 );
	}
	HRESULT	State(MMCKINFO* pck = 0)
	{
		if(pck)
		{
			memcpy(pck,  &m_ck, sizeof(MMCKINFO));
		}
		return m_hr;
	}
	FOURCC Id()
	{
		return m_ck.ckid;
	}
	~WriteChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
};

class ReadChunk
{
	MMCKINFO m_ck;
	MMCKINFO* m_pckParent;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	ReadChunk(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent) : m_pRiffStream(pRiffStream)
	{
		m_pckParent = pckParent;
		m_hr = pRiffStream->Descend( &m_ck,  m_pckParent, 0 );
	}
	~ReadChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
	HRESULT	State(MMCKINFO* pck=0)
	{
		if(pck)
		{
			memcpy(pck, &m_ck, sizeof(MMCKINFO));
		}
		return m_hr;
	}
	FOURCC Id()
	{
		if(m_ck.ckid == FOURCC_LIST)
		{
			return m_ck.fccType;
		}
		else
		{
			return m_ck.ckid;
		}
	}
};


// list navigators
class WriteListChunk
{
	MMCKINFO m_ck;
	IDMUSProdRIFFStream* m_pRiffStream;
	HRESULT m_hr;
public:
	WriteListChunk(IDMUSProdRIFFStream* pRiffStream, FOURCC id)
	{
		m_ck.fccType = id;
		m_pRiffStream = pRiffStream;
 		m_hr = pRiffStream->CreateChunk( &m_ck, MMIO_CREATELIST );
	}
	HRESULT	State(MMCKINFO* pck=0)
	{
		if(pck)
		{
			memcpy(pck, &m_ck, sizeof(MMCKINFO));
		}
		return m_hr;
	}
	FOURCC Id()
	{
		return m_ck.ckid;
	}
	~WriteListChunk()
	{
		if(m_hr == 0)
		{
			m_hr = m_pRiffStream->Ascend(&m_ck, 0);
		}
	}
};


// run time data structs
struct ioPersonality
{
	char	szLoadName[20];
	DWORD	dwScalePattern;
	DWORD	dwFlags;
	ioPersonality()
	{
		memset(this, 0, sizeof(ioPersonality));
	}
};

struct ioSubChord
{
	DWORD	dwChordPattern;
	DWORD	dwScalePattern;
	DWORD	dwInvertPattern;
	BYTE	bChordRoot;
	BYTE	bScaleRoot;
	WORD	wCFlags;
	DWORD	dwLevels;	// parts or which subchord levels this chord supports
	ioSubChord()
	{
		memset(this, 0, sizeof(ioSubChord));
	}
	ioSubChord(DMChord& chord)
	{
		dwChordPattern = chord.ChordPattern();
		dwScalePattern = chord.ScalePattern();
		dwInvertPattern = chord.InvertPattern();
		dwLevels = chord.Levels();
		wCFlags = chord.Flags();
		bChordRoot = chord.ChordRoot();
		bScaleRoot = chord.ScaleRoot();
	}
	void CopyToDMChord(DMChord& chord)
	{
		chord.ChordPattern() = dwChordPattern;
		chord.ScalePattern() = dwScalePattern;
		chord.InvertPattern() = dwInvertPattern;
		chord.Levels() = dwLevels;
		chord.Flags() = wCFlags;
		chord.ChordRoot() = bChordRoot;
		chord.ScaleRoot() = bScaleRoot;
	}
	BOOL operator==(const ioSubChord& sub)
	{
		return !(memcmp(this, &sub, sizeof(ioSubChord)));
	}
	operator unsigned long()
	{
		return dwChordPattern;
	}
};

struct ioChordEntry
{
	DWORD	dwFlags;
	WORD	wConnectionID;	// replaces runtime "pointer to this"
	ioChordEntry()
	{
		memset(this, 0, sizeof(ioChordEntry));
	}
};

struct ioNextChord
{
	DWORD	dwFlags;
	WORD	nWeight;
	WORD	wMinBeats;
	WORD	wMaxBeats;
	WORD	wConnectionID;	// points to an ioChordEntry
	ioNextChord()
	{
		memset(this, 0, sizeof(ioNextChord));
	}
};

struct ioSignPost
{
	DWORD	dwChords;	// 1bit per group
	DWORD	dwFlags;
	ioSignPost()
	{
		memset(this, 0, sizeof(ioSignPost));
	}
};


// design time data structs
struct ioPersonalityEdit
{
	WCHAR	wchUserName[20];
	WCHAR	wchDescription[80];
	BYTE		bVariableNotFixed; // variable or fixed grid chordmap
	WORD	wChordMapLength;
	BOOL	  fUseTimeSignature;
	LONG	lTimeSignature;
	BYTE		bKeySig;
	WORD	wSplitterYPos;
	WORD	wScrollHorPos;
	WORD	wScrollVertPos;
	BYTE		selection;
	WORD	wConnectionIdSelectedChord;
	// next two fields identify a selected connection
	WORD	wConnectionIdSelectedFromChord;
	WORD	wConnectionIdSelectedToChord;
	WORD	wSelectedSignpostRow;
	WORD	wSelectedSignpostCol;
	ioPersonalityEdit()
	{
		memset(this,0,sizeof(ioPersonalityEdit));
	}
};

struct ioChordEntryEdit
{
	BYTE	bBits[MaxSubChords];
	BYTE	 bUseFlat[MaxSubChords];
	WORD	wMeasure;
	BYTE	 bBeat;
	// mrect
	short		left;
	short		top;
	short		right;
	short		bottom;
    short		page;
	WORD	wConnectionId;	// maplet to ioChordEntry
	ioChordEntryEdit()
	{
		memset(this,0,sizeof(ioChordEntryEdit));
	}
};

struct DMChordEdit
{
	BYTE	bBits[MaxSubChords];
	BYTE	 bUseFlat[MaxSubChords];
	DMChordEdit()
	{
		memset(this,0,sizeof(DMChordEdit));
	}
	DMChordEdit(const DMPolyChord& chord)
	{
		for(int i = 0; i < MaxSubChords; i++)
		{
			DMChord* pchord = const_cast<DMPolyChord&>(chord).SubChord(i);
			bBits[i] = pchord->Bits();
			bUseFlat[i] = pchord->UseFlat();
		}
	}
	void Extract(const DMPolyChord& chord)
	{
		for(int i = 0; i < MaxSubChords; i++)
		{
			DMChord* pchord = const_cast<DMPolyChord&>(chord).SubChord(i);
			bBits[i] = pchord->Bits();
			bUseFlat[i] = pchord->UseFlat();
		}
	}
	void Insert(DMPolyChord& chord)
	{
		for(int i = 0; i < MaxSubChords; i++)
		{
			chord.SubChord(i)->Bits() = bBits[i];
			chord.SubChord(i)->UseFlat() = bUseFlat[i];
		}
	}
};

struct PersonalityStructure
{
	char	bVariableNotFixed;
	char	bKey;
	char	bUseTimeSignature;
	char	bReserved1;
	short  wChordMapLength;
	short	wReserved1;
	long	lTimeSig;
	PersonalityStructure()
	{
		bVariableNotFixed = FALSE;
		bKey = 0;
		bUseTimeSignature = TRUE;
		bReserved1 = 0;
		wChordMapLength = 20;
		wReserved1 = 0;
		lTimeSig = MAKE_TIMESIG(4,4) ;
	}
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream)
	{
		IStream* pIStream;
		HRESULT hr;
		DWORD dwBytesWritten;
		// Write  header
		WriteChunk chunk(pRiffStream, FOURCC_PERSONALITYSTRUCT);
		hr = chunk.State();
		if(  hr  == 0 )
		{
			pIStream = pRiffStream->GetStream();
			ASSERT( pIStream != NULL );

			// Write Signpost chunk data
			hr = pIStream->Write( this, sizeof(PersonalityStructure), &dwBytesWritten) == 0
				&& dwBytesWritten == sizeof(PersonalityStructure) ? S_OK : E_FAIL;
			pIStream->Release();
		}
		return hr;
	}

	HRESULT Read(IDMUSProdRIFFStream* pIRiffStream)
	{
		ASSERT(pIRiffStream);
		HRESULT hr;
		DWORD dwBytesRead;
		IStream* pIStream = pIRiffStream->GetStream();
		ASSERT(pIStream != NULL);
		if(pIStream == NULL)
		{
			return E_FAIL;
		}
		hr = pIStream->Read(this, sizeof(PersonalityStructure), &dwBytesRead) == 0
			&& dwBytesRead == sizeof(PersonalityStructure) ? S_OK : E_FAIL;
		pIStream->Release();
		return hr;
	}

};

struct ioPersonalityVersion
{
	enum {PersonalityVersionMS = 0x0001000, PersonalityVersionLS = 0x00000000};
	DWORD	m_dwVersionMS;
	DWORD	m_dwVersionLS;
	ioPersonalityVersion()
	{
		m_dwVersionMS = PersonalityVersionMS;
		m_dwVersionLS = PersonalityVersionLS;
	}
};


#ifdef XXXXTryAndCompileThisXXXX

RIFF
(
	'DMPR'
	<perh-ck>			// Personality header chunk
	<pstr-ck>			// personality structual info
	[<guid-ck>]			// guid chunk
	[<vers-ck>]			// version chunk (two DWORDS)
	<INFO-list>		  // standard MS Info chunk
	<chdt-ck>		   // subchord database
	<chpl-list>			// chord palette
	<cmap-list>		  // chord map
	<spst-list>			// signpost list
	[<ceed-ck>]		// optional chordmap position data
 )

 <chdt> ::= chdt(<cbChordSize::WORD>  <ioSubChord> ... )

<chpl-list> ::= LIST('chpl' 
								<chrd-list> ... // chord definition
							 )

<chrd-list> ::= LIST('chrd' 
								<INAM-ck> // name of chord in wide char format
								<sbcn-ck>	// list of subchords composing chord
								[<ched-ck>]   //  optional chord edit flags
								)

<cmap-list> ::= LIST('cmap' <choe-list> )

<choe-list> ::= LIST('choe'
								<cheh-ck>	// chord entry data
								<chrd-list>	// chord definition
								<ncrd-ck>	// connecting(next) chords
								)

<spst-list> ::= LIST('spst'
							 <spsh-ck>
							 <chrd-list>
							 [<cade-list>]
							 )

<cade-list> ::= LIST('cade' <chrd-list> ...)
								
<sbcn-ck> ::= sbcn(<cSubChordID:WORD>)

<ceed-ck> ::= ceed(ioChordEntryEdit)

<ched-ck> ::= ched(DMChordEdit)

<cheh-ck> ::= cheh(i<ioChordEntry>)

<ncrd-ck> ::= ncrd(<ioNextChord>)

<spsh-ck> ::= spsh(<ioSignPost>)

#endif


#endif