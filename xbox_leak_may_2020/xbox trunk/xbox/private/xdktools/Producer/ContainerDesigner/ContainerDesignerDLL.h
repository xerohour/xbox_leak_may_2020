#if !defined(AFX_CONTAINERDESIGNER_H__778A0B95_6F81_11D3_B45F_00105A2796DE__INCLUDED_)
#define AFX_CONTAINERDESIGNER_H__778A0B95_6F81_11D3_B45F_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContainerDesignerDLL.h : main header file for CONTAINERDESIGNER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusicc.h>
#include <dmusici.h>
#include <dmusicf.h>
#pragma warning( pop )
#include "DMUSProd.h"
#include "ContainerDesigner.h"
#include "SegmentDesigner.h"
#include "StyleDesigner.h"
#include "ScriptDesigner.h"
#include "ChordMapDesigner.h"
#include "DLSDesigner.h"
#include "BandEditor.h"
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

#define FIRST_CONTAINER_IMAGE		0
#define FIRST_CONTAINERREF_IMAGE	2
#define FIRST_FOLDER_IMAGE			4
#define FIRST_FOLDER_GRAY_IMAGE		6


class CContainerComponent;

/////////////////////////////////////////////////////////////////////////////
// CContainerDesignerApp : See ContainerDesignerDLL.cpp for implementation.

class CContainerDesignerApp : public COleControlModule
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
	CContainerComponent*		m_pContainerComponent;
};

extern CContainerDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTAINERDESIGNER_H__778A0B95_6F81_11D3_B45F_00105A2796DE__INCLUDED)
