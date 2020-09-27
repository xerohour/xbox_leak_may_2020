#if !defined(AFX_DLSDESIGNER_H__BC964E9C_96F7_11D0_89AA_00A0C9054129__INCLUDED_)
#define AFX_DLSDESIGNER_H__BC964E9C_96F7_11D0_89AA_00A0C9054129__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DLSDesigner.h : main header file for DLSDESIGNER.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"		// main symbols
#include "DMUSProd.h"
#include "Conductor.h"		// Conductor interface and CLSID

#define SMALL_BUFFER	32
#define MID_BUFFER		100
#define MAX_BUFFER		256
#define TOOLBAR_HEIGHT	29
#define WM_MYPROPVERB	WM_USER+1267 

#define RELEASE(x) if( (x) ) (x)->Release(); (x) = 0

#ifndef CHUNK_ALIGN
#define SIZE_ALIGN	sizeof(BYTE *)
#define CHUNK_ALIGN(x) (((x) + SIZE_ALIGN - 1) & ~(SIZE_ALIGN - 1))
#endif

extern const CLSID CLSID_CollectionPropPage;
extern const CLSID CLSID_InstrumentPropPage;
extern const CLSID CLSID_RegionPropPage;
extern const CLSID CLSID_WavePropPage;
extern const CLSID CLSID_ArticulationPropPage;

/////////////////////////////////////////////////////////////////////////////
// CDLSDesignerApp : See DLSDesigner.cpp for implementation.

class CDLSDesignerApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
	BOOL GetHelpFileName( CString& strHelpFileName );
	void SetRootNode( IDMUSProdNode* pNode, IDMUSProdNode* pRootNode );
	void SetNodePointers( IDMUSProdNode* pNode, IDMUSProdNode* pRootNode, IDMUSProdNode* pParentNode );
	void SetFramework(IDMUSProdFramework * pFramework);
	IDMUSProdFramework *GetFramework();

	BOOL PutDataInClipboard( IDataObject* pIDataObject, void* pClipboardObject );
	void FlushClipboard( void* pClipboardObject );


private:
	IDataObject*	m_pIClipboardDataObject;	// Data copied into clipboard
	void*			m_pClipboardObject;			// Object responsible for data copied into clipboard

public:	
	HANDLE			m_hKeyProgressBar;			// Used to manage Framework's progress bar
	DWORD			m_dwProgressBarIncrement;	
	DWORD			m_dwProgressBarTotalBytes;
	DWORD			m_dwProgressBarRemainingBytes;

	IDMUSProdFramework* m_pFramework;
};

extern CDLSDesignerApp NEAR theApp;

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLSDESIGNER_H__BC964E9C_96F7_11D0_89AA_00A0C9054129__INCLUDED)
