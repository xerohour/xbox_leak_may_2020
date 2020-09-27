
--------------------------------------------------
IDMUSProdToolBar AutoDocs for DirectMusic Producer
--------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODTOOLBAR
========================================================================================
@interface IDMUSProdToolBar | 
	The <i IDMUSProdToolBar> interface provides the means for a DirectMusic Producer
	<o Component> to add a <o ToolBar> to the <o Framework>.  

@comm
	The ToolBar object's <i IDMUSProdToolBar> interface gets handed to the Framework via the
	Framework's <om IDMUSProdFramework::AddToolBar> method.  This method adds the ToolBar to
	the Framework's toolbar space.   

@base public | IUnknown

@xref <o ToolBar> Object, <om IDMUSProdFramework.AddToolBar>, <om IDMUSProdFramework.RemoveToolBar>

@meth HRESULT | GetInfo | Returns the owner window, HINSTANCE, resource ID, and title used
		when the Framework creates the toolbar control.
@meth HRESULT | GetMenuText | Returns text for the menu item placed in the Framework's View
		menu that is used to show/hide the toolbar control.
@meth HRESULT | GetMenuHelpText | Returns context help text for the menu item placed in the
		Framework's View menu that is used to show/hide the toolbar control.
@meth HRESULT | Initialize | Initializes the toolbar control.
--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER TOOLBAR OBJECT
========================================================================================
@object ToolBar | Represents a toolbar.

@supint IDMUSProdToolBar | Provides the means for a DirectMusic Producer <o Component>
	to add a ToolBar to the <o Framework>. 

@comm
	DirectMusic Producer ToolBar objects are not tied to a specific editor.  They reside
	in the Framework's toolbar space and are accessible to all Components via custom interfaces. 

	A DirectMusic Producer Component determines the life span of a ToolBar object.  However,
	a typical Component would create its ToolBar(s) and call <om IDMUSProdFramework::AddToolBar>
	from within its <om IDMUSProdComponent::Initialize> method.  <om IDMUSProdComponent::CleanUp>
	would then be used to call <om IDMUSProdFramework::RemoveToolBar> and delete existing
	ToolBar object(s).
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODTOOLBAR::GETINFO
========================================================================================
@method HRESULT | IDMUSProdToolBar | GetInfo | A <o Component> can call
		<om IDMUSProdFramework.AddToolBar> to add a <o ToolBar> to the Framework's toolbar
		space.  <om IDMUSProdToolBar::GetInfo> returns the owner window, HINSTANCE, resource
		ID, and title used when the Framework creates the toolbar control.

@parm HWND* | phWndOwner | [out] Pointer to the handle of the window that receives notifications
		from the toolbar control.
@parm HINSTANCE* | phInstance | [out] Pointer to the handle of the module containing resources
		for the toolbar control.
@parm UINT* | pnResourceId | [out] Pointer to the ID of the TOOLBAR resource used when creating
		the toolbar.  A value of -1 indicates that the <om IDMUSProdToolBar::Initialize> method
		takes care of adding buttons to the toolbar control.
@parm BSTR* | pbstrTitle | [out] Pointer to the caller-allocated variable that
		receives a copy of the ToolBar title.  The caller must free <p pbstrTitle> with
		SysFreeString when it is no longer needed. 

@rvalue S_OK | The owner window, HINSTANCE, resource ID, and title were returned in <p phWndOwner>,
		<p phInstance>, <p pnResourceId>, and <p pbstrTitle>. 
@rvalue E_POINTER | The address in either <p phWndOwner>, <p phInstance>, <p pnResourceId> or
		<p pbstrTitle> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and values were not returned in <p phWndOwner>, <p phInstance>,
		<p pnResourceId> and <p pbstrTitle>.

@ex The following example returns the owner window, HINSTANCE, resource ID, and title used by the
		Framework when creating the toolbar control: |

HRESULT CTempTransportMsgHandler::GetInfo( HWND* phWndOwner, HINSTANCE* phInstance, UINT* pnResourceId, BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (phWndOwner == NULL)
	||  (phInstance == NULL)
	||  (pnResourceId == NULL)
	||  (pbstrTitle == NULL) )
	{
		return E_POINTER;
	}

	*phWndOwner = GetSafeHwnd();
	*phInstance = theApp.m_hInstance;
	*pnResourceId = IDR_TEMP_TOOLBAR;

	CString strTitle = _T("Transport");
	*pbstrTitle = strTitle.AllocSysString();
	
	return S_OK;
}

@xref <i IDMUSProdToolBar>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODTOOLBAR::GETMENUTEXT
========================================================================================
@method HRESULT | IDMUSProdToolBar | GetMenuText |	A <o Component> can call
		<om IDMUSProdFramework.AddToolBar> to add a <o ToolBar> to the Framework's toolbar
		space.  <om IDMUSProdToolBar::GetMenuText> returns the text for the menu item placed
		in the Framework's View menu that is used to show/hide the toolbar control.
 
@comm
	This text appears in the Framework's View menu.

@parm BSTR* | pbstrText | [out,retval] Pointer to the caller-allocated variable that
		receives a copy of the menu text.  The caller must free <p pbstrText> with
		SysFreeString when it is no longer needed. 

@rvalue S_OK | The menu text was returned successfully.
@rvalue E_POINTER | The address in <p pbstrText> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the menu text was not returned.

@ex The following example places "Transport Controls" into the Framework's View menu: |

HRESULT CTempTransportMsgHandler::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrText == NULL )
	{
		return E_POINTER;
	}

	CString strText = _T("Transport &Controls");
	*pbstrText = strText.AllocSysString();
	
	return S_OK;
}
	
@xref <i IDMUSProdToolBar>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODTOOLBAR::GETMENUHELPTEXT
========================================================================================
@method HRESULT | IDMUSProdToolBar | GetMenuHelpText | A <o Component> can call
		<om IDMUSProdFramework.AddToolBar> to add a <o ToolBar> to the Framework's toolbar
		space.  <om IDMUSProdToolBar::GetMenuHelpText> returns context help text for the menu
		item placed in the Framework's View menu that is used to show/hide the toolbar
		control.
 
@comm
	This text appears in the Framework's status bar when the mouse pointer moves over the
	menu item placed in the Framework's View menu that is used to show/hide the toolbar
	control.

@parm BSTR* | pbstrHelpText | [out,retval] Pointer to the caller-allocated variable that
		receives a copy of the context help text.  The caller must free <p pbstrHelpText>
		with SysFreeString when it is no longer needed.

@rvalue S_OK | The context help text was returned successfully.
@rvalue E_POINTER | The address in <p pbstrHelpText> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the context help text was not returned.

@ex The following example provides context help for the menu item added to the Framework's
		View menu: |

HRESULT CTempTransportMsgHandler::GetMenuHelpText( BSTR* pbstrMenuHelpText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pbstrMenuHelpText == NULL )
	{
		return E_POINTER;
	}

	CString strMenuHelpText;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_TEMP_TOOLBAR_HELP, achBuffer, MID_BUFFER) )
	{
		strMenuHelpText = achBuffer;
	}

    *pbstrMenuHelpText = strMenuHelpText.AllocSysString();

	return S_OK;
}
	
@xref <i IDMUSProdToolBar>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODTOOLBAR::INITIALIZE
========================================================================================
@method HRESULT | IDMUSProdToolBar | Initialize | 
	A <o Component> can call <om IDMUSProdFramework.AddToolBar> to add a <o ToolBar> object to
	the Framework's toolbar space.  This method initializes the toolbar control.

@comm
	Called immediately after the Framework creates the toolbar control.

	In addition to allocating necessary resources, this method may add additional controls
	to the toolbar.

@parm HWND | hWndToolBar | [in] Handle to the toolbar control's window.

@rvalue S_OK | The toolbar control was initialized successfully.
@rvalue E_INVALIDARG | <p hWndToolBar> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | The toolbar control could not be initialized due to lack of memory.
@rvalue E_FAIL | The toolbar control could not be initialized for some reason other than
		lack of memory or invalid arguments.

@ex The following example adds a combo box to the toolbar control: |

HRESULT CTempTransportMsgHandler::Initialize( HWND hWndToolBar )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( hWndToolBar == NULL )
	{
		return E_INVALIDARG;
	}

	CToolBarCtrl* pToolBarCtrl = new CToolBarCtrl;
	if( pToolBarCtrl == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pToolBarCtrl->Attach( hWndToolBar );

// Create font for toolbar combo box
	CClientDC* pDC = new CClientDC( pToolBarCtrl );

	int nHeight = -( (pDC->GetDeviceCaps(LOGPIXELSY) * 8) / 72 );

	m_font.CreateFont( nHeight, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif" );
	
	CFont* pOldFont = pDC->SelectObject( &m_font );
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics( &tm );
	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading;

	pDC->SelectObject( pOldFont );
	delete pDC;

// Create combo box
	TBBUTTON button;

	button.iBitmap = (cxChar * 15);	// width
	button.idCommand = 0;
	button.fsStyle = TBSTYLE_SEP;
	button.fsState = 0;

	pToolBarCtrl->InsertButton( 0, &button );

	CRect rect;
	pToolBarCtrl->GetItemRect( 0, &rect );
	rect.bottom = rect.top + (cyChar * 10);

	if( !m_ctlComboBox.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL |
			CBS_DROPDOWNLIST | CBS_SORT, rect, pToolBarCtrl, IDC_TEMP_TOOLBAR_COMBO) )
	{
		return E_FAIL;
	}

// Set combo box font which controls height of combo box
	m_ctlComboBox.SetFont( &m_font );

	if( pToolBarCtrl )
	{
		pToolBarCtrl->Detach();
		delete pToolBarCtrl;
	}

	return S_OK;
}
	
@xref <o ToolBar> Object, <o Framework> Object, <i IDMUSProdToolBar>, <i IDMUSProdFramework>
--------------------------------------------------------------------------------------*/
