#if !defined(AFX_PERSONALITYDESIGNER_H__D433F964_B588_11D0_9EDC_00AA00A21BA9__INCLUDED_)
#define AFX_PERSONALITYDESIGNER_H__D433F964_B588_11D0_9EDC_00AA00A21BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// PersonalityDesigner.h : main header file for PERSONALITYDESIGNER.DLL
#define RELEASE(x) if( (x) ) (x)->Release()

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#include "DMUSProd.h"
#include "dmusici.h"
#include "Conductor.h"		// Conductor interface and CLSID
#include "StyleDesigner.h"
#include "ChordMapDesigner.h"


/////////////////////////////////////////////////////////////////////////////
// DLL externs
//

#ifdef __cplusplus
extern "C"{
#endif 

extern const CLSID CLSID_PersonalityPropPage;

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256
#define TOOLBAR_HEIGHT	30
#define WM_MYPROPVERB	WM_USER+1267 

#define MAX_LENGTH_PERSONALITYUSER_NAME	19	// excluding NULL (NULL makes 20)


/////////////////////////////////////////////////////////////////////////////
// CPersonalityDesignerApp : See PersonalityDesigner.cpp for implementation.

class CPersonalityDesignerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
	BOOL GetHelpFileName( CString& strHelpFileName );
	void SetNodePointers( IDMUSProdNode* pINode, IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pIParentNode );

public:
	IDMUSProdPropPageManager*	m_pIPageManager;
	BOOL GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize );
};

extern CPersonalityDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSONALITYDESIGNER_H__D433F964_B588_11D0_9EDC_00AA00A21BA9__INCLUDED)
