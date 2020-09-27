#ifndef __SEGMENTPPGMGR_H__
#define __SEGMENTPPGMGR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <DMUSProd.h>
#include "Timeline.h"
#include "DllBasePropPageManager.h"

// SegmentPPGMgr.h: interface for the CSegmentPPGMgr class.
//
//////////////////////////////////////////////////////////////////////

#define SEGMENT_MAX_MEASURES 999

// {DFCE8607-A6FA-11d1-8881-00C04FBF8D15}
static const GUID GUID_SegmentPPGMgr = 
{ 0xdfce8607, 0xa6fa, 0x11d1, { 0x88, 0x81, 0x0, 0xc0, 0x4f, 0xbf, 0x8d, 0x15 } };

struct _DMUS_IO_SEGMENT_DESIGN;

// Define constants for dwFlags
#define PPGT_VALIDSEGMENT		0x1
#define PPGT_VALIDLOOP			0x2
#define PPGT_VALIDINFO			0x4
//#define PPGT_WARNUSER			0x8
#define PPGT_HAVEAUDIOPATH		0x10

//#define PPGT_NONVALIDFLAGS		(PPGT_WARNUSER | PPGT_HAVEAUDIOPATH)
#define PPGT_NONVALIDFLAGS		(PPGT_HAVEAUDIOPATH)

// Define a constant for 'no activity level'
#define ACTIVITY_NONE			0x8000

typedef struct PPGSegment
{
	DWORD			dwPageIndex;
	CString			strSegmentName;
	DWORD			dwMeasureLength;
	MUSIC_TIME		mtSegmentLength; // Informational only
	REFERENCE_TIME	rtSegmentLength;
    REFERENCE_TIME  rtLoopStart;
    REFERENCE_TIME  rtLoopEnd;
	WORD			wSegmentRepeats;
	DWORD			dwResolution;
	DWORD			dwPlayStartMeasure;
	BYTE			bPlayStartBeat;
	WORD			wPlayStartGrid;
	WORD			wPlayStartTick;
	DWORD			dwTrackGroup;
	DWORD			dwExtraBars;
	BOOL			fPickupBar;
	DWORD			dwSegmentFlags;


	interface IDMUSProdTimeline	*pITimelineCtl;

	DWORD			dwLoopStartMeasure;
	BYTE			bLoopStartBeat;
	WORD			wLoopStartGrid;
	WORD			wLoopStartTick;
	DWORD			dwLoopEndMeasure;
	BYTE			bLoopEndBeat;
	WORD			wLoopEndGrid;
	WORD			wLoopEndTick;
	BOOL			fPad;
	DWORD			dwLoopRepeats;

	CString			strSubject;
	CString			strAuthor;
	CString			strCopyright;
	WORD			wVersion1;
	WORD			wVersion2;
	WORD			wVersion3;
	WORD			wVersion4;
	CString			strInfo;
	GUID			guidSegment;

	DWORD			dwFlags;

	PPGSegment();
	~PPGSegment();

	void Copy( const PPGSegment* pSegment );
	void Import( const _DMUS_IO_SEGMENT_DESIGN* pSegmentDesign );
	void Export( _DMUS_IO_SEGMENT_DESIGN* pSegmentDesign ) const;
} PPGSegment;

class CSegmentPPG;
class CLoopPPG;
class CInfoPPG;
class CTabBoundaryFlags;

//////////////////////////////////////////////////////////////////////
//  CSegmentPPGMgr

class CSegmentPPGMgr : public CDllBasePropPageManager 
{
friend CSegmentPPG;
friend CLoopPPG;
friend CInfoPPG;
public:
	CSegmentPPGMgr();
	virtual ~CSegmentPPGMgr();

    // IDMUSProdPropPageManager functions
    HRESULT STDMETHODCALLTYPE SetObject( IDMUSProdPropPageObject* pINewPropPageObject );
	HRESULT STDMETHODCALLTYPE RemoveObject( IDMUSProdPropPageObject* pIPropPageObject );
    HRESULT STDMETHODCALLTYPE GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText );
    HRESULT STDMETHODCALLTYPE GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages );
	HRESULT STDMETHODCALLTYPE OnRemoveFromPropertySheet();
    HRESULT STDMETHODCALLTYPE RefreshData();

	// Member variables
private:
	CSegmentPPG		*m_pSegmentPPG;
	CLoopPPG		*m_pLoopPPG;
	CInfoPPG		*m_pInfoPPG;
	CTabBoundaryFlags *m_pBoundaryFlags;

public:
	static short	sm_nActiveTab;
};

#endif //__SEGMENTPPGMGR_H__
