#if !defined(AFX_AUDIOPATHDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_)
#define AFX_AUDIOPATHDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AudioPathDesigner.h : main header file for AUDIOPATHDESIGNER.DLL

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
#include "AudioPathDesigner.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "DllJazzDataObject.h"
#include "DllBasePropPageManager.h"
#include "JazzUndoMan.h"


/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define CF_AUDIOPATH "DMUSProd v.1 AudioPath"
#define CF_AUDIOPATHLIST "DMUSProd v.1 AudioPath List"

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256

#define FIRST_AUDIOPATH_IMAGE		0
#define FIRST_AUDIOPATHREF_IMAGE	2

class CAudioPathComponent;


/////////////////////////////////////////////////////////////////////////////
// CAudioPathDesignerApp : See AudioPathDesigner.cpp for implementation.

class CAudioPathDesignerApp : public COleControlModule
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
	CAudioPathComponent*			m_pAudioPathComponent;
};

extern CAudioPathDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOPATHDESIGNER_H__D135DB5E_66ED_11D3_B45D_00105A2796DE__INCLUDED)
