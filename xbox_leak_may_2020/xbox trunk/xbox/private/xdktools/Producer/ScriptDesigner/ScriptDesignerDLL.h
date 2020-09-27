#if !defined(AFX_SCRIPTDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
#define AFX_SCRIPTDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ScriptDesigner.h : main header file for SCRIPTDESIGNER.DLL

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
#include "ScriptDesigner.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "DllJazzDataObject.h"
#include "DllBasePropPageManager.h"
#include "JazzUndoMan.h"
#include <DLSDesigner.h>


/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define CF_SCRIPT "DMUSProd v.1 Script"
#define CF_SCRIPTLIST "DMUSProd v.1 Script List"

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256

#define FIRST_SCRIPT_IMAGE		0
#define FIRST_SCRIPTREF_IMAGE	2

#define WM_DISPLAY_MESSAGE_TEXT		(WM_USER + 0xF5)
#define WM_END_EDIT_VALUE			(WM_USER + 0xF6)
#define WM_CANCEL_EDIT_VALUE		(WM_USER + 0xF7)


class CScriptComponent;


/////////////////////////////////////////////////////////////////////////////
// CScriptDesignerApp : See ScriptDesigner.cpp for implementation.

class CScriptDesignerApp : public COleControlModule
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
	CScriptComponent*			m_pScriptComponent;
};

extern CScriptDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED)
