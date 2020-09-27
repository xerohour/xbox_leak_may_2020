// chord.h

#include "DWList.h"
#include "ChordMapStripMgr.h"

#ifndef __CHORD_H__
#define __CHORD_H__

#pragma pack(1)

#define FIELDOFFSET(s,f)     (long)(& (((s __RPC_FAR *)0)->f))


#define MakeID(a,b,c,d)  ( (LONG)(a)<<24L | (LONG)(b)<<16L | (c)<<8 | (d) )

#define ID_CHORDENTRY   MakeID('s','C','E','N')
#define ID_NEXTLIST     MakeID('s','N','X','L')
#define ID_PERSONALITY  MakeID('s','P','E','R')
#define ID_PERSONREF    MakeID('s','P','R','F')
#define ID_NEWSTYLE     MakeID('s','S','T','Y')
#define ID_CHORDPALETTE MakeID('s','C','P','L')
#define ID_TEMPLATE     MakeID('s','T','P','L')
#define ID_TEMPLATELIST MakeID('s','T','L','S')
#define ID_SIGNPOST     MakeID('s','S','N','P')
#define ID_INSTRUMENTS  MakeID('s','I','N','S')
#define ID_COMMAND      MakeID('s','C','M','D')
#define ID_GROOVENAME   MakeID('s','G','R','N')

// Large versions for editing:
#define ID_EPERSONALITY MakeID('s','C','M','P')
#define ID_ECHORDENTRY  MakeID('e','C','E','N')
#define ID_ENEXTLIST    MakeID('e','N','X','L')
#define ID_ESTYLE       MakeID('e','S','T','Y')
#define ID_EPERSONREF   MakeID('e','P','R','F')

// new DM ids
#define ID_DMCHORDENTRY		MakeID('x','C','E','N')
#define ID_DMNEXTLIST		MakeID('x','N','X','L')
#define ID_DMSIGNPOST		MakeID('x','S','N','P')
#define ID_DMCHORDPALETTE	MakeID('x','C','P','L')
#define ID_DMPERSONALITY	MakeID('x','P','E','R')

// extended version for Jazz
#define ID_JPERSONALITY MakeID('j','C','M','P')
#define ID_J4PERSONALITY MakeID('4', 'C', 'M', 'P')
#define ID_J5PERSONALITY MakeID('5', 'C', 'M', 'P')

/*  SCTchord bBits flags ===============================================*/
#define CHORD_INVERT  0x10      /* This chord may be inverted           */
#define CHORD_FOUR    0x20      /* This should be a 4 note chord        */
#define CHORD_UPPER   0x40      /* Shift upper octave down              */
#define CHORD_SIMPLE  0x80      /* This is a simple chord               */
#define CHORD_COUNT   0x0F      /* Number of notes in chord (up to 15)  */

#define CHORD_CONNECTION_TOCHORD 0x4000 /* chord is "to" part of a connection */
#define CHORD_CONNECTION_FROMCHORD 0x2000 /* chord is "from" part of a connection */

// enums for selection type (matches Chordmapmgr enums
#define CHORD_NO_SELECTION						0
#define CHORD_PALETTE_SELECTED					1
#define CHORD_MAP_SELECTED							2
#define CHORD_CONNECTION_SELECTED			3

#define ID_CHRD        0x43485244
#define ID_KBRD        0x4B425244

void swaplong(char data[]);
void WriteID( IStream *pIStream, DWORD id );
void WriteSize( IStream *pIStream, long size );
long ReadID( IStream *pIStream );
long ReadSize( IStream *pIStream );


inline long Rotate24(long val, long shift)
{
	__int64 mask64 = 0xffffff;
	__int64 val64 = val;

	ASSERT(shift > -24 && shift < 24);
	if(shift < 0)
	{
		shift = 24 + shift;
	}
	val64 = (val64 & mask64) << shift;
	__int64 excess = (val64 >> 24) & mask64;	// rotate
	val64 |= (excess & ~(1<<shift));
	val =  long(val64 & mask64);
	return val;
}


inline unsigned long Canonical24(unsigned long pattern, unsigned long root)
//
// convert pattern (scale or chord) to a canonical representation where octave 
// equivalences are filled in.
//
{
	if(!(pattern & 0xf00000))
	{
		// dup at octave
		pattern |= pattern << 12;
	}
	pattern = Rotate24(pattern, root%12);
	// fill in lower bits
	pattern |= pattern>>12;
	return pattern;
}

inline DWORD BlueBits(DWORD& cp, DWORD cr, DWORD& sp, DWORD sr, bool bReturnCanonicalForm = true)
// ~(chord -> scale) = ~(scale | ~chord) = ~scale & chord
{
	DWORD scale, chord;
	scale = Canonical24(sp,sr);
	chord = Canonical24(cp,cr);
	DWORD pattern = ~scale  & chord;
	if(bReturnCanonicalForm)
	{
		sp = scale;
		cp = chord;
	}
	return pattern;
}


inline bool InScale(char root, long chordpattern, long scalepattern)
{
	bool match = (Rotate24(chordpattern, (long)root) | scalepattern) == scalepattern;
	return match;
}


// structs for loading old personalities
struct OldChordEntry
{
	// ChordSelection = 38
	long time;
	long pattern;
	char name[12];
	char keydown;
	char root;
	char inscale;
	char flat;
	short varflags;
	short measure;
	char beat;
	BYTE bits;
	long scalepattern;
	long melodypattern;
	// SmallChordEntry = 44
	DWORD dwflags;
	short nid;
	// extended = 54
	short rect[4];
	short page;
	static DWORD	SizeExtendedChordEntry();
	static DWORD	SizeChordSelection();
	static DWORD	SizeSmallChordEntry();
};

struct OldNextChord
{
	// small
	DWORD	dwflags;
	short	nweight;
	short	nminbeats;
	short	nmaxbeats;
	short	nid;
	// extended
	short	rect[4];
};

// DM structures
// DMChord represents a subchord in a multi chord structure
class DMPolyChord;
class DMChord
{
	DWORD	m_dwChordPattern;
	DWORD	m_dwScalePattern;
	DWORD	m_dwInvertPattern;
	DWORD	m_dwLevels;		// which levels of multi chord this subchord supports
	WORD	m_wFlags;
	BYTE	m_bChordRoot;
	BYTE	m_bScaleRoot;
	BYTE	m_bFlat;
	BYTE	m_bBits;
	bool	m_bInUse;
public:
	DWORD&	ChordPattern()
	{
		return m_dwChordPattern;
	}
	DWORD ChordPattern() const
	{
		return m_dwChordPattern;
	}
	DWORD&	ScalePattern()
	{
		return m_dwScalePattern;
	}
	DWORD	ScalePattern() const
	{
		return m_dwScalePattern;
	}
	DWORD&	InvertPattern()
	{
		return m_dwInvertPattern;
	}
	DWORD	InvertPattern() const
	{
		return m_dwInvertPattern;
	}
	DWORD& Levels()
	{
		return m_dwLevels;
	}
	DWORD Levels() const
	{
		return m_dwLevels;
	}
	WORD&	Flags()
	{
		return m_wFlags;
	}
	WORD	Flags() const
	{
		return m_wFlags;
	}
	BYTE&	ChordRoot()
	{
		return m_bChordRoot;
	}
	BYTE	ChordRoot() const
	{
		return m_bChordRoot;
	}
	BYTE&	ScaleRoot()
	{
		return m_bScaleRoot;
	}
	BYTE	ScaleRoot() const
	{
		return m_bScaleRoot;
	}
	BYTE&	UseFlat()
	{
		return m_bFlat;
	}
	BYTE	UseFlat() const
	{
		return m_bFlat;
	}
	BYTE&	Bits()
	{
		return m_bBits;
	}
	BYTE	Bits() const
	{
		return m_bBits;
	}
	bool&	InUse()
	{
		return m_bInUse;
	}
	bool	InUse() const
	{
		return m_bInUse;
	}
	DMChord();
	DMChord(const DMChord& chord)
	{
		memcpy(this, &chord, sizeof(DMChord));
	}
	DMChord& operator = (const DMChord& chord)
	{
		memcpy(this, &chord, sizeof(DMChord));
		return *this;
	}
	int BassNote();
	int Third(int * bass);
	int Fifth(int * bass, int * third);
	int Seventh(int* bass, int* third, int* fifth);
	void ChordNotes(int& bass, int& third, int& fifth, int& seventh);
	int NoteCount();	// number of notes in chord
};

// DMPolyChord represents a multi chord
class DMPolyChord
{
public:
	void InitLevels();
	enum { ALL = -1, MAX_POLY = 4, MAX_NAME = 12 };
	DMPolyChord();
	DMPolyChord(const DMPolyChord&);
	DMChord* const	operator[] (int index)
	{
		if(index >= MAX_POLY 
			|| index < 0)
		{
			return 0;
		}
		else
		{
			return &m_Chords[index];
		}
	}
	DMChord* const SubChord(int index)
	{
		if(index >= MAX_POLY 
			|| index < 0)
		{
			return 0;
		}
		else
		{
			return &m_Chords[index];
		}
	}
	DWORD&	Time()
	{
		return m_dwTime;
	}
	DWORD	Time() const
	{
		return m_dwTime;
	}
	short&	Measure()
	{
		return m_nMeasure;
	}
	short Measure() const
	{
		return m_nMeasure;
	}
	BYTE&	Beat()
	{
		return m_bBeat;
	}
	BYTE	Beat() const
	{
		return m_bBeat;
	}
	TCHAR* const Name()
	{
		return m_szName;
	}
	DMPolyChord& operator = (const DMPolyChord&);
	enum { INVALID = 255 };	// invalid chord (set root index to this to indicate)
	BYTE& RootIndex()
	{
		return m_bRootIndex;
	}
	void Load(OldChordEntry& oce);
	void Save(OldChordEntry& oce);
	bool& KeyDown() 
	{
		return m_keydown;
	}
	// some convenience funcs for old code
	DMChord* const Base()
	{
		if(m_bRootIndex == INVALID)
			return SubChord(0);
		else
			return SubChord(m_bRootIndex);
	}
	DMChord* operator->()
	{
		return Base();
	}

	void SetBase(int v)
	{
		m_bRootIndex = static_cast<BYTE>(v);
	}

	// mass copy functions, from base to other subchords
	void PropagateChordPattern();
	void PropagateScalePattern();
	void PropagateInvertPattern();
	void PropagateLevels();
	void PropagateFlags();
	void PropagateChordRoot();
	void PropagateScaleRoot();
	void PropagateUseFlat();
	void PropagateBits();
	void PropagateInUse();
	void PropagateAll();

protected:
	DMChord	m_Chords[MAX_POLY];
	DWORD	m_dwTime;
	short	m_nMeasure;
	BYTE	m_bBeat;
	BYTE	m_bRootIndex;	// identifies which chord in m_Chords is the root
	TCHAR	m_szName[12];
	bool	m_keydown;
};




class ChordSelection : public DWListItem, public DMPolyChord {
public:
            ChordSelection();
			ChordSelection(const ChordSelection& cs) : DMPolyChord(cs)
			{
				SetNext(0);
			}
    void    SetBits(int nSubChord);		// nSubChord == -1 --> match all subchords
//    void    SetMelody(int nSubChord);        // Converts pattern into melodypattern. 
											// nSubChord == -1 --> match all subchords
    BOOL    Equals(ChordSelection *second, int nSubChord);	// nSubChord == -1 --> match all subchords
    void    GetName(char *string);
    void    RootToString(char *string); // Convert from root to text name.

    void    LoadFile(char *filename);
    void    SaveFile(char *filename);
	void SetChordPattern(DWORD dwPattern, int nSubChord);
	void SetScalePattern(DWORD dwPattern, int nSubChord);
	void SetChordRoot(BYTE root, int nSubChord);
	void SetScaleRoot(BYTE root, int nSubChord);
	void SetUseFlats(BYTE bFlat, int nSubChord);
};


#define ID_CHRD        0x43485244
#define ID_KBRD        0x4B425244

#define CHORD_WIDTH     80
#define CHORD_HEIGHT    26
#define CENTER_X        (CHORD_WIDTH >> 1)
#define CENTER_Y        (CHORD_HEIGHT >> 1)
#define NEXT_HEIGHT     14
#define NEXT_INDENT     10

class Personality;
//class MyFile;
class NextChord;
class SearchInfo;

class NextList : public DWList {
public:
    NextChord *     GetHead() { return(NextChord*)DWList::GetHead();};
	NextChord *		RemoveHead() { return (NextChord*)DWList::RemoveHead();}
    void            ClearList();
    void			Save( IStream *pIStream, BOOL bSmall );
    long            Size(BOOL bSmall);
    void            Load( IStream *pIStream,long lsize);
};

class SmallChordEntry : public DWListItem {
public:
	SmallChordEntry()
	{
		memset(this, 0, sizeof(SmallChordEntry));
	}
	SmallChordEntry(const SmallChordEntry& );
    NextList        m_nextchordlist;// List of chords to go to next.
    ChordSelection  m_chordsel;     // Chord definition.
    unsigned long   m_dwflags;      // Various flags.
    short           m_nid;          // ID for pointer maintenance.
};


//#define SmallChordEntryFileSize  (OldChord - sizeof(DWListItem) + sizeof(unsigned long) + sizeof(short));

#define SmallChordEntryFileSize (OldChordEntry::SizeSmallChordEntry())


#define CE_SELECTED 1               // This is the active chord.
#define CE_START    2
#define CE_END      4
#define CE_MEASURE  8
#define CE_PATH     16
#define CE_TREE     32

#define CE_MATCHED			(1 << 8)	// if signpost chord, indicates chord is matched by chord in signpost list
#define CE_DRAGSELECT		(1 << 9)
#define CE_DROPSELECT		(1 << 10)



class CChordItem;
class CPropChord;
class ChordEntry : public SmallChordEntry {
public:
                    ChordEntry();
					ChordEntry(const ChordEntry& ce);
                    ~ChordEntry();
	bool			IsSignPost()
	{
		return (m_dwflags & CE_START) || (m_dwflags & CE_END);
	}
	void			CopyNextList(ChordEntry& sce, bool bCopyOnlySelected);
	ChordEntry *    GetNext(){ return (ChordEntry *)(DWListItem::GetNext()); };
    void            Save( IStream *pIStream, BOOL bSmall );
    long            Size(BOOL bSmall, long version = 0);
    static ChordEntry * Load( IStream *pIStream,long filesize, long id);
    void            Draw(CDC *pDC,short page);
    void            CleanupNexts();
	void			LoadSmall(OldChordEntry& oce);
	void			LoadExtended(OldChordEntry& oce);
	void			SaveSmall(OldChordEntry& oce);
	void			SaveExtended(OldChordEntry& oce);
    void            Debug();
    void            AssignParents();
    void            MarkTree();
	void			LoadFromChordItem(CChordItem&);
	void			LoadFromPropChord(CPropChord& chord);
	void			SaveToPropChord(CPropChord& chord);
    struct
	{
		short		left;
		short		top;
		short		right;
		short		bottom;
	} m_rect;
    short           m_page;
	struct
	{
		short		left;
		short		top;
		short		right;
		short		bottom;
	} m_lastrect;
    Personality     *m_parent;      // The parent chord map.
};

//#define ChordEntryFileSize (SmallChordEntryFileSize + (sizeof(short) * 4) + sizeof(short))

#define ChordEntryFileSize (OldChordEntry::SizeExtendedChordEntry())

class ChordEntryList : public DWList {
public:
    ChordEntry *    GetHead() {return(ChordEntry *)DWList::GetHead();};
	ChordEntry *	RemoveHead() {return (ChordEntry *)DWList::RemoveHead();};
	void			ClearList();
};

#define PAGE_1      1
#define PAGE_2      2
#define PAGE_3      4
#define PAGE_4      8
#define PAGE_5      0x10
#define PAGE_6      0x20
#define PAGE_7      0x40
#define PAGE_8      0x80



class SmallNextChord : public DWListItem {
public:
    ChordEntry      *m_nextchord;   // Destination chord.
	BOOL			IsSelected()
	{
		return !(m_nextchord == NULL);
	}
    unsigned long   m_dwflags;
    short           m_nweight;      // Importance of destination chord.
    short           m_nminbeats;    // Min beats to wait till chord.
    short           m_nmaxbeats;    // Max beats to wait till chord.
    short           m_nid;          // ID of destination chord.
};

#define SmallNextChordFileSize (sizeof(SmallNextChord) - sizeof(ChordEntry *))

class NextChord : public SmallNextChord {
public:
                    NextChord();
					NextChord(const NextChord& nc)
					{
						m_nextchord = 0;
						m_parent = nc.m_parent;
						SetNext(0);
						m_dwflags = nc.m_dwflags;
						m_nweight = nc.m_nweight;
						m_nminbeats = nc.m_nminbeats;
						m_nmaxbeats = nc.m_nmaxbeats;
						m_nid = nc.m_nid;
						memcpy(&m_rect, &(nc.m_rect), sizeof(m_rect));
					}
    NextChord *     GetNext() {return(NextChord *)DWListItem::GetNext();};
    void            Debug();
    void            MarkTree();
    void            Draw(CDC *pDC,short page);
//    CRect           m_rect;         // Box in display.
	struct
	{
		short		left;
		short		top;
		short		right;
		short		bottom;
	} m_rect;
    ChordEntry      *m_parent;      // Source chord.
};

#define NextChordFileSize (SmallNextChordFileSize + sizeof(ChordEntry *))

#define NC_SELECTED 1               // This is the active connector.
#define NC_PATH     2               // For walking the tree.
#define NC_NOPATH   4               // Failed tree walk.
#define NC_TREE     8               // For displaying a tree.
#define NC_DELETE	16

class TempCommand;
class PlayChord;

class FailSearch {
public:
                    FailSearch()
                    {
                        m_toomanybeats = 0;
                        m_toofewbeats = 0;
                        m_toomanychords = 0;
                        m_toofewchords = 0;
                    }
    short           m_toomanybeats;
    short           m_toofewbeats;
    short           m_toomanychords;
    short           m_toofewchords;
};

class SearchInfo {
public:
                    SearchInfo()
                    {
                        m_pfirstchord = NULL;
                        m_pplaychord = NULL;
                    };
    ChordSelection  m_start;
    ChordSelection  m_end;
    PlayChord *     m_pplaychord;
    ChordEntry *    m_pfirstchord;
    short           m_beats;
    short           m_minbeats;
    short           m_maxbeats;
    short           m_chords;
    short           m_minchords;
    short           m_maxchords;
    short           m_activity;
    FailSearch      m_fail;
};

#define ACTIVITY_SLOW   2
#define ACTIVITY_MEDIUM 1
#define ACTIVITY_FAST   0

class ChordPalette {
public:
                    ChordPalette();
    void            InitChords(BOOL major,BOOL allnotes,BOOL fat,BOOL flat);
    BOOL            LoadFile(char *filename);
    BOOL            SaveFile(char *filename);
	HRESULT			Load(IStream* pStream, long version = 0);
	HRESULT			Save(IStream*, long version = 0);
	static long		Size(long version);
	ChordSelection&	Chord(int i) { return m_chords[i]; }
    ChordSelection  m_chords[24];
};



#define PF_FILL     1           /* Fill pattern.                    */
#define PF_INTRO    2
#define PF_WHOLE    4           /* Handles chords on measures.      */
#define PF_HALF     8           /* Chords every two beats.          */
#define PF_QUARTER  0x10        /* Chords on beats.                 */
#define PF_BREAK    0x20
#define PF_END      0x40
#define PF_A        0x80
#define PF_B        0x100
#define PF_C        0x200
#define PF_D        0x400
#define PF_E        0x800
#define PF_F        0x1000
#define PF_G        0x2000
#define PF_H        0x10000
#define PF_STOPNOW  0x4000
#define PF_INRIFF   0x8000
#define PF_BEATS    (PF_WHOLE | PF_HALF | PF_QUARTER)
#define PF_RIFF     (PF_INTRO | PF_BREAK | PF_FILL | PF_END)
#define PF_GROOVE   (PF_A | PF_B | PF_C | PF_D | PF_E | PF_F | PF_G | PF_H)

#define SP_A        1
#define SP_B        2
#define SP_C        4
#define SP_D        8
#define SP_E        0x10
#define SP_F        0x20
#define SP_LETTER   (SP_A | SP_B | SP_C | SP_D | SP_E | SP_F)
#define SP_1        0x100
#define SP_2        0x200
#define SP_3        0x400
#define SP_4        0x800
#define SP_5        0x1000
#define SP_6        0x2000
#define SP_7        0x4000
#define SP_ROOT     (SP_1 | SP_2 | SP_3 | SP_4 | SP_5 | SP_6 | SP_7)
#define SP_CADENCE  0x8000

// Structure shape:

#define STR_FALLING     1
#define STR_LEVEL       2
#define STR_LOOPABLE    3
#define STR_LOUD        4
#define STR_QUIET       5
#define STR_PEAKING     6
#define STR_RANDOM      7
#define STR_RISING      8
#define STR_SONG        9  

/*
class Template : public DWListItem {
public:
                    Template();
                    ~Template();
    Template *      GetNext() { return(Template *) DWListItem::GetNext();};
    void            Save(MyFile *pfile);
    long            Size();
    static Template *Load(MyFile *pfile,long lsize);
    static Template *LoadFile(char *filename);
    BOOL            SaveFile(char *filename);
    void            CopyInto(Template *);
    void            MakeName(char *string,short max);
    void            CommandsToString(char *string,short max);
    void            ExpandCommands();
    void            CompressCommands();
    void            CreateSignPosts();
    void            CopyCommands(Template *psource,short measure);
    void            CreateEmbellishments(short shape);
    void            AddChord(short nmeasure,DWORD dwchord);
    void            AddCommand(short nmeasure,DWORD dwcommand);
    void            InsertCommand(Command *pcommand,BOOL iscommand);
    void            CutCommands(Template *pdest,short measure,short length);
    void            PasteCommands(Template *psource,short measure);
    short             NextSignPost(short measure,DWORD signpost,short quantize);
    DWORD           GetFirstSignPost();
    void            Dump();
//  void            StringToNodes(char *string,short max);
    char            m_name[20];
    char            m_type[20];
    short           m_measures;
    CommandList     m_commandlist;
};
*/
/*
class TemplateList : public DWList {
public:
                    TemplateList();
    void            ClearList();
    Template *      GetHead() { return (Template *) DWList::GetHead();};
    void            Save(MyFile *pfile);
    long            Size();
    static TemplateList *Load(MyFile *pfile,long lsize);
    static TemplateList *LoadFile(char *filename);
    BOOL            SaveFile(char *filename);
    void            CopyInto(TemplateList *);
    void            CompressCommands();
};
*/

class SignPost : public DWListItem {
public:
	enum { MAX_CADENCE = 2 };
                    SignPost();
    SignPost *      GetNext() {return(SignPost *) DWListItem::GetNext();};
    void            Save( IStream *pIStream );
    static long            Size(long version = 0);
    static SignPost *Load( IStream *pIStream,long lsize);
    void            SaveFile(char *filename);
    static SignPost *LoadFile(char *filename);
    void            MakeName(char *string);
    ChordSelection  m_chord;            // Chord for sign post.
    ChordSelection  m_cadence[MAX_CADENCE];       // Chords for cadence.
    DWORD           m_chords;       // Which kinds of signpost supported.
    DWORD           m_dwflags;
    DWORD           m_tempflags;
};

class SignPostList : public IndexList {
public:
    SignPost *      GetHead() {return(SignPost *) IndexList::GetHead();};
};

#define SPOST_CADENCE1  2   // Use the first cadence chord.
#define SPOST_CADENCE2  4   // Use the second cadence chord.

class PlayChord : public DWListItem {
public:
                    PlayChord();
    PlayChord *     GetNext() {return (PlayChord *)DWListItem::GetNext();};
    ChordSelection *m_pchord;       // Chord to perform.
    NextChord *     m_pnext;        // Transition pointer to chord.
    unsigned long   m_dwflags;
    short           m_measure;
    short           m_beat;
    short           m_minbeats;
    short           m_maxbeats;
};

class PlayList : public DWList {
public:
    void            ClearList();
    PlayChord *     GetHead() {return(PlayChord *) DWList::GetHead();};
};


class PersonalityRef {
public:
    char            m_stylename[20];// Unique name for UI display.
    char            m_name[20];     // Personality name for internal use.
    char            m_filename[9];  // Unique name for file.
    char            m_isdefault;    // Set for default personality.
};

class Personality : public DWListItem {
public:
                    Personality();
                    ~Personality();
    Personality *   GetNext() {return(Personality *)DWListItem::GetNext();};

    void            Save( IStream *pIStream, long version = 0 );	// save just personality
    void			Load( IStream *pIStream,long lsize, long version = 0); // load just personality

    void            SaveRef( IStream *pIStream,BOOL bSmall);
    long			TotalSize(BOOL bSmall, long version = 0);	// includes contained structs
	static long		PersonalitySize(long version = 0);		// just size of this structure
    static Personality *LoadRef( IStream *pIStream,long lsize,BOOL bSmall);
    BOOL            LoadFile(char *filename);
    BOOL            SaveFile(char *filename,BOOL bSmall);
    void            Debug();
    void            AssignParents();
    void            Draw(CDC *pDC,short page);
    void            MarkTree(ChordEntry *pchord);
    BOOL            IsValidChord(ChordEntry *ptest);
    BOOL            IsValidNext(NextChord *ptest);
    void            ClearChordFlags(unsigned long flag);
    void            ClearNextFlags(unsigned long flag);
    ChordEntry *    FindXYChord(CPoint point,short page);
    NextChord *     FindXYNext(CPoint point,short page);
    void            AssignIDs();    // Make sure all chords have IDs.
    void            ResolveConnections(); // Resolve pointers.
    BOOL            PlayStop();
    BOOL            CalcViewSize();
    BOOL            GetChord(ChordSelection *pchord,DWORD flag,short index);
    short             GetChordCount(DWORD flag);
    short             GetChordIndex(ChordSelection *pcsel,DWORD flag);
    void            InsertChords(short leftedge,DWORD flags);
    void            BuildNeededChords(ChordEntryList *list,DWORD flags);
    ChordEntryList  m_chordlist;    // All chords in the map.
    SignPostList    m_signpostlist; // All available sign posts.
    long            m_scalepattern; // Scale for map.
    char            m_name[20];     // ID Name (not used by application.) Musical Name
    char            m_description[80];
    char			m_username[20]; // Generic name for display.
    ChordPalette    m_chordpalette; // Palette of Chords for static Map.
    unsigned long   m_dwflags;
    PlayList        m_playlist;     // Collection of NextChords for playback.
    ChordEntry *    m_pfirstchord;
    PersonalityRef  m_personref;    // Used to track name and file io.
    short			m_cx;
	short			m_cy;				// CSize           m_size;
};

#define CM_FLAT     1               // Display scale and chords with flats.
#define CM_DEFAULT  2               // Default personality.
#define CM_EMBED    4               // Used in Style Packager.

class PersonalityList : public IndexList {
public:
    Personality *   GetHead() {return(Personality *) IndexList::GetHead();};
    Personality *   GetIndexedItem()
                    {
                        return(Personality *) IndexList::GetIndexedItem();
                    };
    void            ClearList();
};

#define INST_LEAD       0
#define INST_STRINGS    1
#define INST_GUITAR     2
#define INST_PIANO      3
#define INST_BASS       4
#define INST_DRUMS      5


class Instruments : public DWListItem {
public:
                    Instruments();
    Instruments *   GetNext() {return(Instruments *)DWListItem::GetNext();};
    static Instruments * LoadFile(char *filename);
    void            SaveFile(char *filename);
    void            Save( IStream *pIStream );
    long            Size();
    static Instruments * Load( IStream *pIStream,long lsize);
    char            m_name[20];
    unsigned char   m_patch[16];
    char            m_default;
};

class InstrumentsList : public DWList {
public:
    Instruments *   GetHead() {return(Instruments *) DWList::GetHead();};
    void            ClearList();
};

class GrooveName : public DWListItem {
public:
                    GrooveName();
    GrooveName *    GetNext() {return(GrooveName *)DWListItem::GetNext();};
    void            Save( IStream *pIStream );
    long            Size();
    static GrooveName * Load( IStream *pIStream,long lsize);
    char            m_name[20];
    DWORD           m_grooveid;
};

class GrooveNameList : public DWList {
public:
    GrooveName *    GetHead() {return(GrooveName *) DWList::GetHead();};
    void            ClearList();
    void            SetDefaults();
    void            AddGrooveName(char *name,DWORD grooveid);
    GrooveName *    GetGrooveName(DWORD grooveid);
};

class ChordChangeCallback : public IUnknown
{
public:

	ChordChangeCallback() : m_pChordEntry(0), m_callbackType(Unknown), m_dwRef(0) { AddRef();}
	ChordChangeCallback(int ct) : m_pChordEntry(0), m_callbackType(ct), m_dwRef(0) { AddRef();}
	~ChordChangeCallback();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// callback info determination
	enum   { Unknown, ChordMapNewSelection,  ParentShowProps};

//	void SetCallbackType(CallbackType ct) { m_callbackType = ct; }
	int GetCallbackType() { return m_callbackType; }

	void SetChordEntry(ChordEntry* pChordEntry) {ASSERT(pChordEntry); 
											   m_pChordEntry = pChordEntry;}

	ChordEntry* GetChordEntry() {return m_pChordEntry;}
	
private:
	ChordEntry* m_pChordEntry;
	int m_callbackType;
	DWORD		m_dwRef;
};

class PreEditNotification : public IUnknown
{
	DWORD m_dwRef;
public:
	PreEditNotification(CString reason) : m_strReason(reason), m_dwRef(0)
	{
		AddRef();
	}

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	CString	m_strReason;
};

// sent when chordmap detects that orphan status of chords or signposts may need to be rechecked
class CheckForOrphansNotification : public IUnknown
{
	DWORD m_dwRef;
public:
	CheckForOrphansNotification() 
		: m_dwRef(0)
	{
		AddRef();
	}

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
};

// sent when chordmap notifies personality of change of zoom
class ZoomChangeNotification : public IUnknown
{
	DWORD m_dwRef;
public:
	ZoomChangeNotification() 
		: m_dwRef(0)
	{
		AddRef();
	}

	double m_dblZoom;

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv )
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
		if(::IsEqualIID(riid, IID_IUnknown))
		{
			AddRef();
			*ppv = this;
			return S_OK;
		}

		if(::IsEqualIID(riid, IID_ZoomChangeNotification))
		{
			AddRef();
			*ppv = this;
			return S_OK;
		}

		*ppv = NULL;
		return E_NOINTERFACE;
	}
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
};


//////////////////////////////////////////
// ChordExt Structure
// Used to store a Section's single linked chord list.
// Use: CSection::m_paChordChanges.
class ChordExt	: public DMPolyChord
{
//	bool		m_keydown;
public:
	ChordExt()
	{
		m_keydown = false;
		pNext = 0;
	}
    ChordExt		*pNext;
//	bool& KeyDown()
//	{
//		return m_keydown;
//	}
};

typedef struct CommandExt
{
    CommandExt* pNext;
    long        time;       // Time, in clocks
    short       measure;    // Which measure
    DWORD       command;    // Command type
    DWORD       chord;      // Used by composition engine
} CommandExt;



void ChordEntryToChordExt(ChordEntry* pChordEntry, ChordExt* pChordExt);

#pragma pack()

#endif
