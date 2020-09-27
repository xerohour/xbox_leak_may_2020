#ifndef MIDIFILEIO_H
#define MIDIFILEIO_H

class CSequenceItem;

HRESULT	WriteMidiSequenceToStream(IStream* pIStream, DWORD dwPChannel,
								  CTypedPtrList<CPtrList, CSequenceItem*>& lstSequences);


HRESULT ReadMidiSequenceFromStream(LPSTREAM pStream,
								  CTypedPtrList<CPtrList, CSequenceItem*>& lstSequences,
								  CTypedPtrList<CPtrList, CCurveItem*>& lstCurves,
								  long lOffsetTime,
								  DWORD& dwLength);

HRESULT WriteSMFHeader(IStream* pStream, WORD nTracks);

DWORD ReadEvent( LPSTREAM pStream, DWORD dwTime, FullSeqEvent** plstEvent, struct _DMUS_IO_PATCH_ITEM** pplstPatchEvent);
WORD GetVarLength( LPSTREAM pStream, DWORD& rfdwValue );
FullSeqEvent* SortEventList( FullSeqEvent* lstEvent );
FullSeqEvent* CompressEventList( FullSeqEvent* lstEvent );

inline BYTE Status(BYTE x)
{
	return BYTE(x & 0xF0);
}

inline BYTE Channel(BYTE x)
{
	return BYTE(x & 0x0F);
}

// this function gets a short that is formatted the correct way
// i.e. the motorola way as opposed to the intel way
BOOL __inline GetMShort( LPSTREAM pStream, short& n )
{
	union uShort
	{
	unsigned char buf[2];
	short n;
	} u;

	if( S_OK != pStream->Read( u.buf, 2, NULL ) )
	{
	return FALSE;
	}

#ifndef _MAC
	// swap bytes
	unsigned char ch;
	ch = u.buf[0];
	u.buf[0] = u.buf[1];
	u.buf[1] = ch;
#endif

	n = u.n;
	return TRUE;
}

#ifndef _GetMLong_Defined_
#define _GetMLong_Defined_

// this function gets a long that is formatted the correct way
// i.e. the motorola way as opposed to the intel way
BOOL __inline GetMLong( LPSTREAM pStream, DWORD& dw )
{
	union uLong
	{
		unsigned char buf[4];
	DWORD dw;
	} u;

	if( S_OK != pStream->Read( u.buf, 4, NULL ) )
	{
	return FALSE;
	}


#ifndef _MAC
	// swap bytes
	unsigned char ch;
	ch = u.buf[0];
	u.buf[0] = u.buf[3];
	u.buf[3] = ch;

	ch = u.buf[1];
	u.buf[1] = u.buf[2];
	u.buf[2] = ch;
#endif

	dw = u.dw;
	return TRUE;
}

#endif

//////////////////////////////////////// Track class
class CSMFTrack
{
public:
	CTypedPtrList<CPtrList, CSequenceItem*> m_notes;
	CTypedPtrList<CPtrList, CSequenceItem*> m_curves; // Doesn't add note offs to this list

	CSMFTrack() {}
	~CSMFTrack(); /* list management by user not this class, all this does is remove ptrs */
	HRESULT	Write(IStream*);
	HRESULT Read(IStream*);
	void AddItem(CSequenceItem* pItem);
	void AddCurveItem(CSequenceItem* pItem); // Doesn't add note offs
	BOOL IsEmpty()
	{
		return m_notes.IsEmpty() && m_curves.IsEmpty();
	}

protected:
	CSMFTrack(const CSMFTrack&);
	CSMFTrack& operator = (const CSMFTrack&);
};


#endif