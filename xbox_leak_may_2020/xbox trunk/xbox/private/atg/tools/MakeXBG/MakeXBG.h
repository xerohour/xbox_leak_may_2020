//-----------------------------------------------------------------------------
// File: MakeXBG.h
//
// Desc: App to convert DirectX geoemtry files (.x) into .xbg geometry files that
//       are more suited for Xbox apps. See the XBG.h header file for a better
//       description of .xbg geometry files.
//
// Hist: 03.01.00 - New for April XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "AFXCView.h"
#include "Resource.h"
#include "XBG.h"




//-----------------------------------------------------------------------------
// Name: class CMakeXBGApp
// Desc: The main application class
//-----------------------------------------------------------------------------
class CMakeXBGApp : public CWinApp
{
public:
    CMakeXBGApp();
    ~CMakeXBGApp();

// Overrides
    virtual BOOL InitInstance();

// Implementation
    //{{AFX_MSG(CMakeXBGApp)
    afx_msg void OnAppAbout();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CXBGDocument
// Desc: The main document class
//-----------------------------------------------------------------------------
class CXBGDocument : public CDocument
{
protected: // create from serialization only
    CXBGDocument();
    DECLARE_SERIAL(CXBGDocument)

public:
    // Load options
    CString m_strFilename;    // Filename
    BOOL    m_bCollapseMesh;  // Whether to collapse geometry to one mesh

    // Data from loaded file
    CD3DFile*  m_pD3DFile;

    // Data for displaying object's attributes
    CTreeCtrl* m_pTreeCtrl;
    CD3DFrame* m_pSelectedFrame;
    LONG       m_lSelectedSubset;

// Implementation
public:
    virtual ~CXBGDocument();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
protected:
    virtual BOOL    OnOpenDocument( LPCTSTR strFilename );

// Generated message map functions
protected:
    //{{AFX_MSG(CXBGDocument)
    afx_msg void OnSetMeshFVF();
    afx_msg void OnStripify();
    afx_msg void OnFileSave();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CModelTreeView
// Desc: The tree view class
//-----------------------------------------------------------------------------
class CModelTreeView : public CTreeView
{
protected: // create from serialization only
    CModelTreeView();
    DECLARE_DYNCREATE(CModelTreeView)

// Attributes
public:
    CXBGDocument* GetDocument() { return (CXBGDocument*)m_pDocument; }

// Operations
public:

// Implementation
public:
    virtual ~CModelTreeView();
    virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );

// Generated message map functions
protected:
    //{{AFX_MSG(CModelTreeView)
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CModelRenderView
// Desc: The render view (renders the geoemtry model using D3D)
//-----------------------------------------------------------------------------
class CModelRenderView : public CView
{
    // ArcBall parameters
    D3DXQUATERNION m_qDown;          // Quaternion before button down
    D3DXQUATERNION m_qNow;           // Composite quaternion for current drag
    D3DXMATRIX     m_matRotation;    // Matrix for arcball's orientation
    D3DXMATRIX     m_matTranslation; // Matrix for arcball's position
    BOOL           m_bDrag;          // Whether user is dragging arcball
    D3DXVECTOR3    m_vDown;          // Button down vector
    D3DXVECTOR3    m_vCur;           // Current vector
    D3DXVECTOR3 ScreenToVector( int sx, int sy );

    // Rendering objects
    LPDIRECT3DDEVICE8 m_pd3dDevice;
    D3DXMATRIX        m_matWorld; 

protected: // create from serialization only
    CModelRenderView();
    DECLARE_DYNCREATE(CModelRenderView)

// Attributes
public:
    CXBGDocument* GetDocument() { return (CXBGDocument*) m_pDocument; }

// Operations
public:

// Implementation
public:
    virtual ~CModelRenderView();
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual void OnInitialUpdate();

// Generated message map functions
protected:
    //{{AFX_MSG(CModelRenderView)
    afx_msg void OnDestroy();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CSplitterFrame
// Desc: Class to implement a splitter.
//-----------------------------------------------------------------------------
class CSplitterFrame : public CMDIChildWnd
{
    DECLARE_DYNCREATE(CSplitterFrame)
protected:
    CSplitterFrame();   // protected constructor used by dynamic creation

// Attributes
protected:
    CSplitterWnd      m_wndSplitter;
public:
    CModelTreeView*   m_pTreeView;
    CModelRenderView* m_pD3DRenderView;

// Implementation
public:
    virtual ~CSplitterFrame();
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

    // Generated message map functions
    //{{AFX_MSG(CSplitterFrame)
    afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};




//-----------------------------------------------------------------------------
// Name: class CMainSplitterWnd
// Desc: Class to implement a splitter.
//-----------------------------------------------------------------------------
class CMainSplitterWnd : public CSplitterWnd
{
    DECLARE_DYNAMIC(CMainSplitterWnd)

// Implementation
public:
    CMainSplitterWnd();
    ~CMainSplitterWnd();
    CWnd* GetActivePane(int* pRow = NULL, int* pCol = NULL);
};




