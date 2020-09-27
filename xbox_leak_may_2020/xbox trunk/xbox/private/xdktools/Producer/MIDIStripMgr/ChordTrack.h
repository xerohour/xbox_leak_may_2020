#ifndef __CHORDTRACK_H__
#define __CHORDTRACK_H__ 1

#pragma warning( push )
#pragma warning( disable : 4201 )
#include <mmsystem.h>
#pragma warning( pop )
interface IDMUSProdStripMgr;
interface IPersistStream;

class CChordTrack
{
friend class CMIDIMgr;

public:
	CChordTrack();
	~CChordTrack();
	void SetStripMgr( IDMUSProdStripMgr* punkStripMgr );
	void GetStripMgr( IDMUSProdStripMgr** ppunkStripMgr );

	DWORD		m_dwPosition;		// Position in track list
	DWORD		m_dwGroupBits;		// Group bits for track
	FOURCC      m_ckid;				// chunk ID of track's data chunk if 0 fccType valid.
	FOURCC      m_fccType;			// list type if NULL ckid valid

private:
	IDMUSProdStripMgr*	m_pIStripMgr;		// Strip Manager interface
};

#endif //__CHORDTRACK_H__
