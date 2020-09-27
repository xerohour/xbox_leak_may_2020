#if !defined(AFX_SEGMENTDESIGNER_H__8F8F5C6C_A6AB_11D0_8C10_00A0C92E1CAC__INCLUDED_)
#define AFX_SEGMENTDESIGNER_H__8F8F5C6C_A6AB_11D0_8C10_00A0C92E1CAC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// SegmentDesignerDLL.h : main header file for SEGMENTDESIGNER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "Timeline.h"
#include "StripMgr.h"
#include "SegmentDesigner.h"

/////////////////////////////////////////////////////////////////////////////
// DLL defines
//

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256
#define TOOLBAR_HEIGHT	30
#define WM_MYPROPVERB	WM_USER+1267 
#define WM_WNDACTIVATE	WM_USER+1268

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

interface IDMUSProdNode;
interface IDMUSProdFramework;

/////////////////////////////////////////////////////////////////////////////
// CSegmentDesignerApp : See SegmentDesigner.cpp for implementation.

class CSegmentDesignerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
	BOOL GetHelpFileName( CString& strHelpFileName );
	void SetNodePointers( IDMUSProdNode* pINode, IDMUSProdNode* pIDocRootNode, IDMUSProdNode* pIParentNode );
	BOOL GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize );

	IDMUSProdFramework *m_pIFramework;
};

extern CSegmentDesignerApp NEAR theApp;
extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEGMENTDESIGNER_H__8F8F5C6C_A6AB_11D0_8C10_00A0C92E1CAC__INCLUDED)
