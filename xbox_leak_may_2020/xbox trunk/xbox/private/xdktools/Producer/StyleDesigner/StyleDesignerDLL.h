#if !defined(STYLEDESIGNER_H__3BD2BA1B_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
#define STYLEDESIGNER_H__3BD2BA1B_46E7_11D0_89AC_00A0C9054129__INCLUDED_

// StyleDesignerDLL.h : main header file for STYLEDESIGNER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"		// main symbols
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusicc.h>
#include <dmusici.h>
#pragma warning( pop )
#include "DMUSProd.h"
#include "StyleDesigner.h"
#include "BandEditor.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "StyleRiffId.h"	// Style file RIFF chunk id's
#include "DllJazzDataObject.h"
#include "DllBasePropPageManager.h"
#include "JazzUndoMan.h"


/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define CF_VARCHOICES "DMUSProd v.1 VarChoices"
#define CF_BANDTRACK "DMUSProd v.1 Band Track"

#define WM_OPEN_VARCHOICES	(WM_USER + 0xE0)
#define WM_SYNC_STYLEEDITOR (WM_USER + 0xE1)

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256
#define TOOLBAR_HEIGHT	30
#define WM_MYPROPVERB	WM_USER+1267 

#define PPQN            192				// IMA pulses per quarter note
#define PPQNx4          ( PPQN << 2 )
#define PPQN_2          ( PPQN >> 1 )

#define DM_PPQN         768				// Direct Music pulses per quarter note
#define DM_PPQNx4       ( DM_PPQN << 2 )
#define DM_PPQN_2       ( DM_PPQN >> 1 )

#define MIN_TEMPO		10
#define MAX_TEMPO		350

#define MIN_GROOVE		1
#define MAX_GROOVE		100

#define MIN_EMB_CUSTOM_ID	100
#define MAX_EMB_CUSTOM_ID	199

#define MIN_PATTERN_LENGTH		1
#define MAX_PATTERN_LENGTH		999

#define MIN_BEATS_PER_MEASURE	1
#define MAX_BEATS_PER_MEASURE	100

#define MIN_GRIDS_PER_BEAT			1
#define MAX_GRIDS_PER_BEAT			24
#define MAX_GRIDS_PER_BEAT_ENTRIES	48

#define SS_STOP_MUSIC			1
#define SS_KEEP_MUSIC			2

#define GRID_BMP_CX				16			
#define GRID_BMP_CX				16			
#define GRID_BMP_COLUMNS		6
#define GRID_BMP_ROWS			4

#define FIRST_FOLDER_IMAGE		0
#define FIRST_STYLE_IMAGE		2
#define FIRST_MOTIF_IMAGE		4
#define FIRST_PATTERN_IMAGE		6
#define FIRST_STYLEREF_IMAGE	8

#define MIDI_NOTEOFF    0x80
#define MIDI_NOTEON     0x90
#define MIDI_PTOUCH     0xA0
#define MIDI_CCHANGE    0xB0
#define MIDI_PCHANGE    0xC0
#define MIDI_MTOUCH     0xD0
#define MIDI_PBEND      0xE0
#define MIDI_SYSX       0xF0
#define MIDI_MTC        0xF1
#define MIDI_SONGPP     0xF2
#define MIDI_SONGS      0xF3
#define MIDI_EOX        0xF7
#define MIDI_CLOCK      0xF8
#define MIDI_START      0xFA
#define MIDI_CONTINUE   0xFB
#define MIDI_STOP       0xFC
#define MIDI_SENSE      0xFE


class CStyleComponent;

/////////////////////////////////////////////////////////////////////////////
// DLL externs
//

#ifdef __cplusplus
extern "C"{
#endif 

#ifdef __cplusplus
}
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleDesignerApp : See StyleDesigner.cpp for implementation.

class CStyleDesignerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
	BOOL GetHelpFileName( CString& strHelpFileName );
	void SetNodePointers( IDMUSProdNode* pINode, IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pIParentNode );
	BOOL GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize );
	BOOL PutDataInClipboard( IDataObject* pIDataObject, void* pClipboardObject );
	void FlushClipboard( void* pClipboardObject );

private:
	IDataObject*	m_pIClipboardDataObject;	// Data copied into clipboard
	void*			m_pClipboardObject;			// Object responsible for data copied into clipboard

public:
	IDMUSProdPropPageManager*	m_pIPageManager;
	CStyleComponent*		m_pStyleComponent;
};

extern CStyleDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

extern const UINT g_nGridsPerBeatBitmaps[];

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(STYLEDESIGNER_H__3BD2BA1B_46E7_11D0_89AC_00A0C9054129__INCLUDED)
