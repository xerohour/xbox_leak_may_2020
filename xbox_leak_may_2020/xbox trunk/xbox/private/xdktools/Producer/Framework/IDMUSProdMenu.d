
-----------------------------------------------
IDMUSProdMenu AutoDocs for DirectMusic Producer
-----------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODMENU
========================================================================================
@interface IDMUSProdMenu | 
	The <i IDMUSProdMenu> interface defines behavior for menu items that DirectMusic
	Producer Components add to the Framework's 'Add-Ins' menu.

@comm
	A DirectMusic Producer <o Menu> object defines the behavior of a single menu item.
	A Menu object must implement <i IDMUSProdMenu>.

	After creation, the object's <i IDMUSProdMenu> interface gets handed to the Framework via the
	Framework's <om IDMUSProdFramework::AddMenuItem> method.  This method adds the menu item to
	the Framework's 'Add-Ins' menu. <om IDMUSProdFramework::AddMenuItem> provides a way for a
	DirectMusic Producer Component, that otherwise has no UI, to hook into the Framework.  For
	example, a Component may want to place a menu item in the Framework's 'Add-Ins' menu to
	provide a dialog that can be used to setup various options.  

@base public | IUnknown

@xref <o Menu> Object, <om IDMUSProdFramework.AddMenuItem>, <om IDMUSProdFramework.RemoveMenuItem>

@meth HRESULT | GetMenuText | Returns the text asociated with the menu item.
@meth HRESULT | GetMenuHelpText | Returns the context help text associated with the menu item.
@meth HRESULT | OnMenuInit | Initializes the state of the menu item.
@meth HRESULT | OnMenuSelect | Performs the action associated with the menu item.
--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER MENU OBJECT
========================================================================================
@object Menu | Represents a single menu item.

@supint IDMUSProdMenu | Defines the behavior of the menu item.  Implementation is required.

@comm
	A DirectMusic Producer Menu object consists of a label, a context help string, a method
	to initialize its state, and a method to perform when selected.  A Menu object must implement
	<i IDMUSProdMenu>.

	After creation, the object's <i IDMUSProdMenu> interface gets handed to the Framework via the
	Framework's <om IDMUSProdFramework::AddMenuItem> method.  This method adds the menu item
	to the Framework's 'Add-Ins' menu. <om IDMUSProdFramework::AddMenuItem> provides a way for a
	DirectMusic Producer <o Component>, that otherwise has no UI, to hook into the Framework.
	For example, a Component may want to place a menu item in the Framework's 'Add-Ins' menu to
	provide a dialog that can be used to setup various options.  

	A DirectMusic Producer Component determines the life span of a Menu object.  However, a typical
	Component using Menu objects would create its Menu(s) and call <om IDMUSProdFramework::AddMenuItem>
	from within in its <om IDMUSProdComponent::Initialize> method.  <om IDMUSProdComponent::CleanUp>
	would then be used to call <om IDMUSProdFramework::RemoveMenuItem> and delete existing
	Menu object(s).
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODMENU::GETMENUTEXT
========================================================================================
@method HRESULT | IDMUSProdMenu | GetMenuText |	A Component can call 
		<om IDMUSProdFramework.AddMenuItem> to add a <o Menu> object to the Framework's
		'Add-Ins' menu.  GetMenuText returns the text associated with that menu item.
 
@comm
	This text appears in the Framework's 'Add-Ins' menu.

@parm BSTR* | pbstrText | [out,retval] Pointer to the caller-allocated variable that
		receives a copy of the menu text.  The caller must free <p pbstrText> with
		SysFreeString when it is no longer needed. 

@rvalue S_OK | The menu text was returned successfully.
@rvalue E_FAIL | An error occurred and the menu text was not returned.

@ex The following example places "WordPad" into the Framework's 'Add-Ins' menu: |

HRESULT CMyMenu::GetMenuText( BSTR* pbstrText )
{
	CString strText= "WordPad";

    *pbstrText = strText.AllocSysString();

	return S_OK;
}
	
@xref <i IDMUSProdMenu>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODMENU::GETMENUHELPTEXT
========================================================================================
@method HRESULT | IDMUSProdMenu | GetMenuHelpText | A Component can call
		<om IDMUSProdFramework.AddMenuItem> to add a <o Menu> object to the Framework's
		'Add-Ins' menu.  GetMenuHelpText returns the context help text associated with
		that menu item.
 
@comm
	This text appears in the Framework's status bar when the mouse pointer moves over the
	menu item.

@parm BSTR* | pbstrHelpText | [out,retval] Pointer to the caller-allocated variable that
		receives a copy of the context help text.  The caller must free <p pbstrHelpText>
		with SysFreeString when it is no longer needed.

@rvalue S_OK | The context help text was returned successfully.
@rvalue E_FAIL | An error occurred and the context help text was not returned.

@ex The following example provides context help for the menu item added to the Framework's
		'Add-Ins' menu: |

HRESULT CMyMenu::GetMenuHelpText( BSTR* pbstrHelpText )
{
	CString strHelpText= "Run WordPad";

    *pbstrHelpText = strHelpText.AllocSysString();

	return S_OK;
}
	
@xref <i IDMUSProdMenu>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODMENU::ONMENUINIT
========================================================================================
@method HRESULT | IDMUSProdMenu | OnMenuInit | A Component can call
		<om IDMUSProdFramework.AddMenuItem> to add a <o Menu> object to the Framework's
		'Add-Ins' menu.  OnMenuInit initializes the state of that menu item. 

@parm HMENU | hMenu | [in] Handle to Framework's 'Add-Ins' menu.
@parm UINT | nMenuId | [in] Command ID of the menu item.

@rvalue S_OK | Always succeeds. 

@ex The following example enables the menu item: |

HRESULT CMyMenu::OnMenuInit( HMENU hMenu, UINT nMenuId )
{
	::EnableMenuItem( hMenu, nMenuId, (MF_ENABLED | MF_BYCOMMAND) );

	return S_OK;
}
	
@xref <i IDMUSProdMenu>, <om IDMUSProdMenu.OnMenuSelect>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODMENU::ONMENUSELECT
========================================================================================
@method HRESULT | IDMUSProdMenu | OnMenuSelect | A Component can call
		<om IDMUSProdFramework.AddMenuItem> to add a <o Menu> object to the Framework's
		'Add-Ins' menu.  OnMenuSelect performs the action associated with that menu item. 

@rvalue S_OK | Action associated with menu item was successful. 
@rvalue E_FAIL | Action failed. 

@ex The following example runs "WordPad" as a result of selecting the menu item: |

HRESULT CMyMenu::OnMenuSelect( void )
{
    if( ::WinExec("WordPad.exe", SW_SHOWNORMAL) > 31 )
	{
		return S_OK;
	}

	return E_FAIL;
}
	
@xref <i IDMUSProdMenu>, <om IDMUSProdMenu.OnMenuInit>
--------------------------------------------------------------------------------------*/
