//-----------------------------------------------------------------------------
// File: MakeXBG.cpp
//
// Desc: App to convert DirectX geoemtry files (.x) into .xbg geometry files that
//       are more suited for Xbox apps.
//
//       See the XBG.h header file for a better description of .xbg geometry
//       files.
//
// Hist: 02.01.01 - New for March XDK release
//       03.01.01 - Heavily revised for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"
#include "SetMeshFvfDlg.h"
#include "StripifyDlg.h"
#include "LoadOptionsDlg.h"




//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
CMakeXBGApp NEAR g_App;
LPDIRECT3D8      g_pD3D  = NULL;
D3DDISPLAYMODE   g_d3ddm;




//-----------------------------------------------------------------------------
// Name: CMakeXBGApp()
// Desc: Constructor
//-----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMakeXBGApp, CWinApp)
    //{{AFX_MSG_MAP(CMakeXBGApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

CMakeXBGApp::CMakeXBGApp()
{
}

CMakeXBGApp::~CMakeXBGApp()
{
    // Done with the D3D object
    if( g_pD3D )
        g_pD3D->Release();
}




//-----------------------------------------------------------------------------
// Name: InitInstance()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CMakeXBGApp::InitInstance()
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
        return FALSE;

    // Get the current desktop display mode
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &g_d3ddm ) ) )
        return FALSE;

    // Standard initialization
    Enable3dControls();

    // Add a splitter frame with a tree view and a d3d render view
    AddDocTemplate(new CMultiDocTemplate( IDR_DOCTEMPLATETYPE,
                                          RUNTIME_CLASS(CXBGDocument),
                                          RUNTIME_CLASS(CSplitterFrame),
                                          RUNTIME_CLASS(CModelRenderView)) );

    // create main MDI Frame window
    // Please note that for simple MDI Frame windows with no toolbar,
    //   status bar or other special behavior, the CMDIFrameWnd class
    //   can be used directly for the main frame window just as the
    //   CMDIChildWnd can be use for a document frame window.

    CMDIFrameWnd* pMainFrame = new CMDIFrameWnd;

    // Tell MFC to let us have at least a little control over the menus
    pMainFrame->m_bAutoMenuEnable = FALSE;
    
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;

    // Also in this example, there is only one menubar shared between
    //  all the views.  The automatic menu enabling support of MFC
    //  will disable the menu items that don't apply based on the
    //  currently active view.  The one MenuBar is used for all
    //  document types, including when there are no open documents.

    // Now finally show the main menu
    pMainFrame->ShowWindow(m_nCmdShow);
    pMainFrame->UpdateWindow();
    m_pMainWnd = pMainFrame;


    // command line arguments are ignored, create a new (empty) document
    OnFileOpen();

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: OnAppAbout()
// Desc: 
//-----------------------------------------------------------------------------
void CMakeXBGApp::OnAppAbout()
{
    CDialog(IDD_ABOUTBOX).DoModal();
}




//-----------------------------------------------------------------------------
// Name: CXBGDocument()
// Desc: Message map, constructor and destructor
//-----------------------------------------------------------------------------
IMPLEMENT_SERIAL(CXBGDocument, CDocument, 0 /* schema number*/ )

BEGIN_MESSAGE_MAP(CXBGDocument, CDocument)
    //{{AFX_MSG_MAP(CXBGDocument)
    ON_COMMAND(IDM_SETMESHFVF, OnSetMeshFVF)
    ON_COMMAND(IDM_STRIPIFY, OnStripify)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSave)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CXBGDocument::CXBGDocument()
{
    m_pD3DFile         = NULL;
    m_pSelectedFrame   = NULL;
    m_lSelectedSubset  = -1;
}

CXBGDocument::~CXBGDocument()
{
    if( m_pD3DFile )
        delete m_pD3DFile;
}




//-----------------------------------------------------------------------------
// Name: OnOpenDocument()
// Desc: 
//-----------------------------------------------------------------------------
BOOL CXBGDocument::OnOpenDocument( LPCTSTR strFilename )
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    
    m_strFilename   = strFilename;
    m_bCollapseMesh = FALSE;

    // Check if this is a .x file
    TCHAR* strFileType = _tcsrchr( strFilename, _T('.') );
    if( strFileType && !_tcsicmp( strFileType, _T(".x") ) )
    {
        // If so, display an options dialog, to see if the user wants to load
        // the geometry collapsed into one mesh, or to retain the frame
        // hierarchy
        CLoadOptionsDlg dlg;
        if (dlg.DoModal() != IDOK)
            return TRUE;

        m_bCollapseMesh = dlg.m_bCollapseMesh;
    }

    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: Serialize()
// Desc: 
//-----------------------------------------------------------------------------
void CXBGDocument::Serialize(CArchive&)
{
    OnFileSave();
}




//-----------------------------------------------------------------------------
// Name: OnSetMeshFVF()
// Desc: 
//-----------------------------------------------------------------------------
void CXBGDocument::OnSetMeshFVF()
{
    if( m_pSelectedFrame && m_pSelectedFrame->m_dwMeshFVF )
    {
        CSetMeshFvfDlg dlg;
        dlg.m_dwFVF = m_pSelectedFrame->m_dwMeshFVF;

        if (dlg.DoModal() != IDOK)
            return;

        // A new FVF was chosen, so enact it
        m_pSelectedFrame->SetMeshFVF( dlg.m_dwFVF );

        // Rewrite the FVF code
        TCHAR strFVF[20];
        _stprintf( strFVF, _T("FVF = 0x%08lx"), m_pSelectedFrame->m_dwMeshFVF );
        m_pTreeCtrl->SetItemText( m_pSelectedFrame->m_hFVFTreeItem, strFVF );

        // Tag the document as modified
        SetModifiedFlag( TRUE );
    }
}




//-----------------------------------------------------------------------------
// Name: OnStripify()
// Desc: 
//-----------------------------------------------------------------------------
void CXBGDocument::OnStripify()
{
    if( m_pSelectedFrame && m_pSelectedFrame->m_dwMeshFVF )
    {
        StripifyDlg dlg;
        dlg.m_pFrame = m_pSelectedFrame;

        if (dlg.DoModal() != IDOK)
            return;
    }
}




//-----------------------------------------------------------------------------
// Name: OnFileSave()
// Desc: Writes the .xbg geoemtry file
//-----------------------------------------------------------------------------
void CXBGDocument::OnFileSave() 
{
    // Get the filename, with the .xbg prepended
    TCHAR strFilename[512]="*.xbg";
    _tcscpy( strFilename, m_strFilename );
    if( _tcsrchr( strFilename, _T('.') ) )
        _tcscpy( _tcsrchr( strFilename, _T('.') ), _T(".xbg") );
    
    // Build the filter string
    CString strFilter;
    strFilter += _T("Geometry Files (*.xbg)"); strFilter += _T('\0');
    strFilter += _T("*.xbg");                  strFilter += _T('\0');
    strFilter += _T("All Files (*.*)");        strFilter += _T('\0');
    strFilter += _T("*.*");                    strFilter += _T('\0');

    // Display the "file save as" dialog
    CFileDialog dlg( FALSE );
    dlg.m_ofn.nMaxCustFilter++;
    dlg.m_ofn.nMaxCustFilter++;
    dlg.m_ofn.lpstrFilter = strFilter;
    dlg.m_ofn.lpstrTitle  = _T("Save XBG File As...");
    dlg.m_ofn.lpstrFile   = strFilename;
    if( dlg.DoModal() )
    {
        // Save the file
        m_pD3DFile->WriteToXBG( strFilename );
    }
}


