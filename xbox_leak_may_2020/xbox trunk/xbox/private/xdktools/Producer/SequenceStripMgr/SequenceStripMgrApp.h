// SequenceStripMgrApp.h : Declaration of the CSequenceStripMgrApp

#ifndef __SequenceSTRIPMGRAPP_H_
#define __SequenceSTRIPMGRAPP_H_

#include <afxwin.h>

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0


interface IDMUSProdPropPageManager;

class CSequenceStripMgrApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public:
	IDMUSProdPropPageManager*	m_pIPageManager;
	UINT		m_cfCurve;						// CF_CURVE clipboard format
	CFont*		m_pCurveStripFont;				// Curve strip font
};

#define MAX_GRIDS_PER_BEAT_ENTRIES	48

extern const UINT g_nGridsPerBeatBitmaps[];

#endif //__SequenceSTRIPMGRAPP_H_
