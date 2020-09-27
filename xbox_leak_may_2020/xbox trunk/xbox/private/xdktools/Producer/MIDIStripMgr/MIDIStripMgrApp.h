// MIDIStripMgrApp.h : Declaration of the CMIDIStripMgrApp

#ifndef __MIDISTRIPMGRAPP_H_
#define __MIDISTRIPMGRAPP_H_


#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

interface IDMUSProdPropPageManager;
class CFont;

class CMIDIStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	IDMUSProdPropPageManager*	m_pIPageManager;
	UINT		m_cfNoteFormat;					// CF_MUSICNOTE clipboard format
	UINT		m_cfAllEventFormat;				// CF_MUSICNOTEANDCURVE clipboard format
	UINT		m_cfCurve;						// CF_CURVE clipboard format
	UINT		m_cfSeqTrack;					// CF_SEQUENCELIST clipboard format
	UINT		m_cfSeqCurves;					// CF_CURVELIST clipboard format (for Seq strip)
	UINT		m_cfMIDIFile;					// CF_MIDIFILE clipboard format
	UINT		m_cfStyleMarker;				// CF_STYLEMARKER clipboard format
	CFont*		m_pCurveStripFont;				// Curve strip font
};

#define MAX_GRIDS_PER_BEAT_ENTRIES	48

extern const UINT g_nGridsPerBeatBitmaps[];

extern class CMIDIStripMgrApp theApp;

#endif //__MIDISTRIPMGRAPP_H_
