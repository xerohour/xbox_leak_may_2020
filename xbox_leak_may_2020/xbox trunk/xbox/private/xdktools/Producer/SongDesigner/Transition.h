#ifndef __TRANSITION_H_
#define __TRANSITION_H_

#include "RiffStrm.h"

class CVirtualSegment;


#define DMUS_FOURCC_TRANSITION_COPY_PASTE_UI_CHUNK	mmioFOURCC('c','p','t','u')

#pragma pack(2)

typedef struct ioTransitionCopyPasteUI
{
	ioTransitionCopyPasteUI()
	{
		dwToSegmentFlag = DMUS_SONG_ANYSEG;
		pToSegment = NULL;
		pTransitionSegment = NULL;
		dwPlayFlagsDM = 0;
	}

	DWORD				dwToSegmentFlag;
	CVirtualSegment*	pToSegment;				// From Virtual Segment list
	CVirtualSegment*	pTransitionSegment;		// From Virtual Segment list
	DWORD				dwPlayFlagsDM;
} ioTransitionCopyPasteUI;

#pragma pack()


//////////////////////////////////////////////////////////////////////
//  CTransition

class CTransition : public IUnknown
{
friend class CVirtualSegment;
friend class CSongDlg;
friend class CTransitionListBox;
friend class CTransitionPropPageObject;

public:
	CTransition( CDirectMusicSong* pSong );
	virtual ~CTransition();
	void Copy( CTransition* pTransition );

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

	// Additional methods
	void FormatTextUI( CString& strText );

private:
    DWORD					m_dwRef;
	CDirectMusicSong*		m_pSong;

	// UI
	DWORD					m_dwToSegmentFlag;
	bool					m_fResolveToSegmentID;
	DWORD					m_dwToSegmentID;
	bool					m_fResolveTransitionSegmentID;
	DWORD					m_dwTransitionSegmentID;
	DWORD					m_dwBitsUI;

	// Persisted for DirectMusic
	//		DMUS_IO_TRANSITION_DEF
	CVirtualSegment*		m_pToSegment;			// From Virtual Segment list
	CVirtualSegment*		m_pTransitionSegment;	// From Virtual Segment list
	DWORD					m_dwPlayFlagsDM;
};

#endif // __TRANSITION_H_
