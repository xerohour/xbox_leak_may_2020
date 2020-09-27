#if !defined(AFX_TOOLGRAPHDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
#define AFX_TOOLGRAPHDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ToolGraphDesignerDLL.h : main header file for TOOLGRAPHDESIGNER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusicc.h>
#include <dmusici.h>
#pragma warning( pop )
#include "DMUSProd.h"
#include "ToolGraphDesigner.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "DllJazzDataObject.h"
#include "DllBasePropPageManager.h"
#include "JazzUndoMan.h"


/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define CF_GRAPH "DMUSProd v.1 Graph"
#define CF_GRAPHLIST "DMUSProd v.1 Graph List"

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256

#define FIRST_GRAPH_IMAGE		0
#define FIRST_GRAPHREF_IMAGE	2


// {0A1A99C6-CC76-4613-A1DF-B167518B1414}
DEFINE_GUID( GUID_SaveSelectedTools, 0xa1a99c6, 0xcc76, 0x4613, 0xa1, 0xdf, 0xb1, 0x67, 0x51, 0x8b, 0x14, 0x14);

class CGraphComponent;


/////////////////////////////////////////////////////////////////////////////
// CGraphDesignerApp : See ToolGraphDesignerDLL.cpp for implementation.

class CGraphDesignerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
	BOOL GetHelpFileName( CString& strHelpFileName );
	void SetNodePointers( IDMUSProdNode* pINode, IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pIParentNode );
	BOOL GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize );
	BOOL PutDataInClipboard( IDataObject* pIDataObject, void* pClipboardObject );
	void FlushClipboard( void* pClipboardObject );
	BOOL IsInClipboard( void* pClipboardObject );

private:
	IDataObject*	m_pIClipboardDataObject;	// Data copied into clipboard
	void*			m_pClipboardObject;			// Object responsible for data copied into clipboard

public:
	IDMUSProdPropPageManager*	m_pIPageManager;
	CGraphComponent*			m_pGraphComponent;
};

extern CGraphDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLGRAPHDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
