#if !defined(AFX_SONGDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
#define AFX_SONGDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SongDesigner.h : main header file for SONGDESIGNER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusicc.h>
#include <dmusici.h>
#include <dmusicp.h>
#pragma warning( pop )
#include "DMUSProd.h"
#include "SongDesigner.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "DllJazzDataObject.h"
#include "DllBasePropPageManager.h"
#include "JazzUndoMan.h"


/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256

#define FIRST_SONG_IMAGE		0
#define FIRST_SONGREF_IMAGE		2
#define FIRST_FOLDER_IMAGE		4


/////////////////////////////////////////////////////////////////////////////
// DLL GUIDs
//

// {075A61AE-D843-4737-B9F7-78DEFDC3AC15}
DEFINE_GUID(GUID_VirtualSegmentPropPageManager, 0x75a61ae,0xd843,0x4737,0xb9,0xf7,0x78,0xde,0xfd,0xc3,0xac,0x15);

// {05EA4449-FE9C-4f6d-AB15-915CE3B19DEE}
DEFINE_GUID(GUID_TransitionPropPageManager, 0x5ea4449,0xfe9c,0x4f6d,0xab,0x15,0x91,0x5c,0xe3,0xb1,0x9d,0xee);

// {D4A61C50-AE39-41e3-9ABC-F180CC1D1EAB}
DEFINE_GUID(GUID_TrackPropPageManager, 0xd4a61c50,0xae39,0x41e3,0x9a,0xbc,0xf1,0x80,0xcc,0x1d,0x1e,0xab);

// {EE021440-A628-4ac3-85FC-12DD887BF61F}
DEFINE_GUID(GUID_SaveSelectedVirtualSegments, 0xee021440, 0xa628, 0x4ac3, 0x85, 0xfc, 0x12, 0xdd, 0x88, 0x7b, 0xf6, 0x1f);

// {91441052-C205-4d74-98B8-DCE24D230586}
DEFINE_GUID(GUID_SaveSelectedTracks, 0x91441052, 0xc205, 0x4d74, 0x98, 0xb8, 0xdc, 0xe2, 0x4d, 0x23, 0x5, 0x86);

// {4E4285FE-BE0E-49cf-B9C3-E67A54EDDECE}
DEFINE_GUID(GUID_SaveSelectedTransitions,  0x4e4285fe, 0xbe0e, 0x49cf, 0xb9, 0xc3, 0xe6, 0x7a, 0x54, 0xed, 0xde, 0xce);

class CSongComponent;


/////////////////////////////////////////////////////////////////////////////
// CSongDesignerApp : See SongDesigner.cpp for implementation.

class CSongDesignerApp : public COleControlModule
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
	CSongComponent*				m_pSongComponent;
};

extern CSongDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SONGDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED)
