#if !defined(AFX_BANDEDITOR_H__D824B38C_804F_11D0_89AC_00A0C9054129__INCLUDED_)
#define AFX_BANDEDITOR_H__D824B38C_804F_11D0_89AC_00A0C9054129__INCLUDED_

// BandEditorDLL.h : main header file for BANDEDITOR.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#include "BandEditor.h"
#include "DMUSProd.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "StyleRiffId.h"	// Style file RIFF chunk id's
#include "DllBasePropPageManager.h"
#include "DllJazzDataObject.h"


/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256
#define TOOLBAR_HEIGHT	30
#define WM_MYPROPVERB	WM_USER+1267 

#define NBR_IMA_INSTRUMENTS			16

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0


/////////////////////////////////////////////////////////////////////////////
// DLL externs
//

#ifdef __cplusplus
extern "C"{
#endif 

extern const CLSID CLSID_BandPropPage;

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CBandEditorApp : See BandEditor.cpp for implementation.

class CBandEditorApp : public COleControlModule
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
	IDMUSProdFramework*			m_pIFramework;
	IDMUSProdPropPageManager*	m_pIPageManager;
};

extern CBandEditorApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BANDEDITOR_H__D824B38C_804F_11D0_89AC_00A0C9054129__INCLUDED)
