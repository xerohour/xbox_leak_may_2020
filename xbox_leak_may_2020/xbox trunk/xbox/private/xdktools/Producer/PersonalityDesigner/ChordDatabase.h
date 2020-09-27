#ifndef CHORDDATABASE_H
#define CHORDDATABASE_H

#include "Chord.h"
#include "PersonalityRIFF.h"

HRESULT DMLoadChordData(ChordEntryList& chordEntryList, IStream* pIStream);

// pattern for single data chunk objects:
// To save to a riffstream: XChunk myChunk(pData); myChunk.Write(pRiffStream);
// To extract from a riffstream: extern XChunk myChunk; mChunk.Read(pRiffStream); myChunk.Insert(pData);


class SignPostChunk : public ioSignPost
{
public:
	SignPostChunk() {}
	SignPostChunk(IDMUSProdRIFFStream* pRiffStream)
	{
		Read(pRiffStream);
	}
	SignPostChunk( const SignPost* pSignPost);
	HRESULT	Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT	Read(IDMUSProdRIFFStream* pRiffStream);
	HRESULT	Extract(const SignPost* pSignPost);
	HRESULT Insert( SignPost* pSignPost);
};

class NextChordChunk: public ioNextChord
{
public:
	NextChordChunk() {}
	NextChordChunk(const NextChord* pNext);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream);
	HRESULT	Extract(const NextChord* pNext);
	HRESULT Insert( NextChord* pNext);
};

class ChordEntryChunk: public ioChordEntry
{
public:
	ChordEntryChunk() {}
	ChordEntryChunk(const ChordEntry* pChord);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream);
	HRESULT	Extract(const ChordEntry* pChord);
	HRESULT Insert( ChordEntry* pChord);
};

// chord entry edit info helpers
HRESULT WriteChordEntryEditChunk(IDMUSProdRIFFStream* pRiffStream, ChordEntryList* plist);
HRESULT ReadChordEntryEditChunk(IDMUSProdRIFFStream* pRiffStream, ChordEntryList* plist, MMCKINFO* pckInfo);

class ChordEntryEditChunk : public ioChordEntryEdit
{
public:
	ChordEntryEditChunk() {}
	ChordEntryEditChunk(const ChordEntry* pChord);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Extract(const ChordEntry* pChord);
	HRESULT Insert(ChordEntry* pChord);
};

class DMSubChordSet;

class ChordListChunk
{
	DMChordEdit m_chordedit;
	bool m_bIncludeEditInfo;
public:
	enum {NameSize = DMPolyChord::MAX_NAME};
	ChordListChunk() {m_bIncludeEditInfo = false;  memset(name, 0, sizeof(name));}
	ChordListChunk(DMPolyChord& pChord, DMSubChordSet& chordset, bool bIncludeEditInfo);
	enum { maxsubchords = MaxSubChords};
	WCHAR	name[NameSize];
	WORD	subchordids[maxsubchords];
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract( DMPolyChord& pChord, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Insert( DMPolyChord& pChord, const DMSubChordSet& subchords);
};


class ChordEntryListItem
{
	bool m_bIncludeEditInfo;
public:
	ChordEntryListItem() {m_bIncludeEditInfo = false;}
	ChordEntryListItem(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
	{
		Read(pRiffStream, pckParent);
	}
	ChordEntryListItem( ChordEntry* pChord, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract(ChordEntry* pChord, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Insert( ChordEntry* pChord, const DMSubChordSet& subchords);
private:
	ChordEntryChunk	m_chordentry;
	ChordListChunk	  m_chordlist;
	CList<NextChordChunk, NextChordChunk&> m_nextchordlist;
};


class CadenceListChunk
{
	bool m_bIncludeEditInfo;
public:
	CadenceListChunk() {m_bHasCadenceChord1 = false; m_bHasCadenceChord2 = false; m_bIncludeEditInfo = false;};
	CadenceListChunk( SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract( SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Insert( SignPost* pSignPost, const DMSubChordSet& subchords);
	BOOL& HasCadenceChord1()
	{
		return m_bHasCadenceChord1;
	}
	BOOL& HasCadenceChord2()
	{
		return m_bHasCadenceChord2;
	}
private:
	ChordListChunk	m_CadenceChord1;
	ChordListChunk	m_CadenceChord2;
	BOOL					m_bHasCadenceChord1;
	BOOL					m_bHasCadenceChord2;
};

class SignPostListItemChunk
{
	bool m_bIncludeEditInfo;
public:
	SignPostListItemChunk() {m_bIncludeEditInfo = false;}
	SignPostListItemChunk(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
	{
		ASSERT(pRiffStream);
		Read(pRiffStream, pckParent);
	}
	SignPostListItemChunk( SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract( SignPost* pSignPost, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Insert( SignPost* pSignPost, const DMSubChordSet& subchords);
private:
	SignPostChunk	m_signpost;
	ChordListChunk	m_signpostchord;
	CadenceListChunk	m_cadencelist;
};

class SignPostListChunk : public 	CList<SignPostListItemChunk*, SignPostListItemChunk*&>
{
	bool m_bIncludeEditInfo;
public:
	SignPostListChunk() {m_bIncludeEditInfo = false;}
	SignPostListChunk(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
	{
		ASSERT(pRiffStream);
		Read(pRiffStream, pckParent);
	}
	SignPostListChunk( SignPostList& signpostlist, DMSubChordSet& subchords, bool bIncludeEditInfo);
	~SignPostListChunk();
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract( SignPostList& signpostlist, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Insert( SignPostList& signpostlist, const DMSubChordSet& subchords);
};

class ChordPaletteList
{
public:
	ChordPaletteList() {}
	ChordPaletteList(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
	{
		ASSERT(pRiffStream);
		Read(pRiffStream, pckParent);
	}
	ChordPaletteList( ChordPalette& chordpalette, DMSubChordSet& subchords);
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract( ChordPalette& chordpalette, DMSubChordSet& subchords);
	HRESULT Insert(ChordPalette& chordpalette, const DMSubChordSet& subchords);
private:
	ChordListChunk	m_chordlist[24];
};

class ChordMapList : public CMap<int, int, ChordEntryListItem*, ChordEntryListItem*&>
{
	bool m_bIncludeEditInfo;
public:
	ChordMapList() {m_bIncludeEditInfo = false;}
	ChordMapList(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent)
	{
		ASSERT(pRiffStream);
		Read(pRiffStream, pckParent);
	}
	ChordMapList( ChordEntryList& chordlist, DMSubChordSet& subchords, bool bIncludeEditInfo);
	~ChordMapList();
	HRESULT Write(IDMUSProdRIFFStream* pRiffStream);
	HRESULT Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pckParent);
	HRESULT	Extract( ChordEntryList& chordlist, DMSubChordSet& subchords, bool bIncludeEditInfo);
	HRESULT Insert(ChordEntryList& chordlist, const DMSubChordSet& subchords);

};



inline BOOL	operator == (ioSubChord p1, ioSubChord p2)
{
	return !memcmp(&p1, &p2, sizeof(ioSubChord));
}


class DMSubChordSet : public CMap<ioSubChord, ioSubChord&, WORD, WORD&>
//
// This class used to prepare subChords to be saved.  Add screens for duplications.
//
{
	WORD	wNextKey;
	ioSubChord*  m_aSubChord;
	WORD		 m_cSubChord;
	void		 BuildSubChordArray();
public:
	DMSubChordSet() { wNextKey = 0; m_cSubChord = 0; m_aSubChord = 0;}
	~DMSubChordSet();
	// access functions
	HRESULT Add( DMPolyChord& Chord, ChordListChunk& chordlist);
	HRESULT Retrieve(DMPolyChord& Chord, const ChordListChunk& chordlist) ;
	// storage functions
	HRESULT	Write(IDMUSProdRIFFStream* pRiffStream);				// writes chord data to pStream
	HRESULT	Read(IDMUSProdRIFFStream* pRiffStream, MMCKINFO* pck);	// reads chord data to pStream
};



#endif