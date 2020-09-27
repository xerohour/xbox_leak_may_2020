
----------------------------------------------------------
IDMUSProdPropPageManager AutoDocs for DirectMusic Producer
----------------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODPROPPAGEMANAGER
========================================================================================
@interface IDMUSProdPropPageManager |
	The <i IDMUSProdPropPageManager> interface defines interaction with the Framework's
	property sheet control.  It also contains methods for setting and refreshing the data
	displaying in the property sheet control.  

@comm
	The <o Framework> exports an <i IDMUSProdPropSheet> interface so that DirectMusic Producer
	<o Component>s can hook into its global property sheet control.  A pointer to the Framework's
	IDMUSProdPropSheet interface can be obtained via <om IDMUSProdFramework::QueryInterface>.
	
	In order to use the global property sheet, a Component must implement <i IDMUSProdPropPageManager>.
	The object behind this interface, the <o PropPageManager> object, manages the set of pages
	placed in the property sheet as well as the data displayed in those pages.  
	
	The PropPageManager uses the <i IDMUSProdPropPageObject> interface to interact with the object
	whose data is being shown in its pages.  Calls to <om IDMUSProdPropPageManager::SetObject> set
	the PropPageManager's current object so that the same set of pages can be used to display
	data for different objects.
	
@base public | IUnknown

@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageObject>

@meth HRESULT | GetPropertySheetTitle | Returns text to be used for the title of the
		Framework's property sheet control.
@meth HRESULT | GetPropertySheetPages | Returns an array of pages to be placed in the
		Framework's property sheet control.
@meth HRESULT | OnRemoveFromPropertySheet | Called when the PropPageManager object is
		being removed from the Framework's property sheet control.
@meth HRESULT | SetObject | Sets the object being displayed by the PropPageManager.
@meth HRESULT | RemoveObject | Removes the specified object from the PropPageManager.
@meth HRESULT | IsEqualObject | Determines whether the specified object is being displayed
		by the PropPageManager. 
@meth HRESULT | RefreshData | Notifies the PropPageManager to refresh its data and redraw
		the active page.
@meth HRESULT | IsEqualPageManagerGUID | Determines whether the specified GUID equals
		the PropPageManager's GUID. 

--------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER PROPPAGEMANAGER OBJECT
========================================================================================
@object PropPageManager | Represents a set of pages placed in the <o Framework>'s property
		sheet control.

@supint IDMUSProdPropPageManager | Defines the behavior of a PropPageManager object.
		Implementation is required.

@comm
	The Framework's property sheet mechanism consists of three (3) objects: <o PropSheet>, 
	<o PropPageManager>, and <o PropPageObject>.

	The Framework creates and manages the one and only DirectMusic Producer PropSheet object.
	This object is created during application launch and exists throughout the entire
	DirectMusic Producer work session. The PropSheet object is nothing more than a property
	sheet control.  A DirectMusic Producer <o Component> uses the <i IDMUSProdPropSheet>
	interface to interact with the Framework's PropSheet object.

	The PropPageManager object defines the set of pages placed in the Framework's property
	sheet control.  <om IDMUSProdPropSheet::SetPageManager> sets the current PropPageManager
	and places its pages into the Framework's property sheet.  PropPageManager objects must
	implement the <i IDMUSProdPropPageManager> interface.
	
	A PropPageObject represents the data being displayed in the PropPageManager's pages.
	PropPageObjects must implement the <i IDMUSProdPropPageObject> interface.  Calls to
	<om IDMUSProdPropPageManager::SetObject> set the PropPageManager's current object so
	that the same set of pages can be used to display data for different objects.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::GETPROPERTYSHEETTITLE
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | GetPropertySheetTitle | 
	Returns the text to be used for the title of the <o Framework>'s property sheet control.

@comm
	This method is called from within <om IDMUSProdPropSheet::SetPageManager>.

@parm BSTR* | pbstrTitle | [out] Pointer to the caller-allocated variable that receives
		a copy of the property sheet title.  The caller must free <p pbstrTitle> with
		SysFreeString when it is no longer needed. 
@parm BOOL* | pfAddPropertiesText | [out] Points to value indicating whether the text
		"Properties" should be appended to the title returned in <p pbstrTitle>.

@rvalue S_OK | The title was returned in <p pbstrTitle>.
@rvalue E_POINTER | <p pbstrTitle> or <p pfAddPropertiesText> is not valid.  For example, it
		may be NULL.
@rvalue E_FAIL | An error occurred and the title was not returned.

@ex The following example returns the title to be used for the Framework's property sheet
		control: |

HRESULT CProjectPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CProject *pProject;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pProject))) )
	{
		*pbstrTitle = pProject->m_strName.AllocSysString();
	}
	else
	{
		CString strTitle;

		strTitle.LoadString( IDS_PROJECT_TEXT );
		*pbstrTitle = strTitle.AllocSysString();
	}

	return S_OK;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::GETPROPERTYSHEETPAGES
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | GetPropertySheetPages | 
	Returns an array of pages to be placed in the <o Framework>'s property sheet control.

@comm
	This method is called from within <om IDMUSProdPropSheet::SetPageManager>.

@parm IDMUSProdPropSheet* | pIPropSheet | [in] Pointer to the Framework's <i IDMUSProdPropSheet>
		interface.
@parm LONG* | hPropSheetPage[] | [out] Pointer to an array of HPROPSHEETPAGEs.
@parm short* | pnNbrPages | [out] Pointer to number of elements in the <p hPropSheetPage>
		array.

@rvalue S_OK | The array of pages was returned in <p hPropSheetPage>.
@rvalue E_POINTER | <p hPropSheetPage> or <p pnNbrPages> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | <p pIPropSheet> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred, and the array of pages was not returned.

@ex The following example creates and returns an array of HPROPSHEETPAGEs: |

HRESULT CProjectPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add General tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	m_pTabGeneral = new CProjectPropTabGeneral();
	if( m_pTabGeneral )
	{
		hPage = ::CreatePropertySheetPage( &m_pTabGeneral->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Folders tab
	m_pTabFolders = new CProjectPropTabFolders();
	if( m_pTabFolders )
	{
		hPage = ::CreatePropertySheetPage( &m_pTabFolders->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::ONREMOVEFROMPROPERTYSHEET
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | OnRemoveFromPropertySheet | 
	Called when the <o PropPageManager> object is being removed from the <o Framework>'s
	property sheet control.

@comm
	The Framework calls this method to notify the PropPageManager that it is being removed
	from the Framework's property sheet control.  This gives the PropPageManager an
	opportunity to perform cleanup processing.

@rvalue S_OK | Resources were freed successfully.
@rvalue E_FAIL | An error occurred, and resources could not be freed.

@ex The following example contains cleanup processing for a PropPageManager: |

HRESULT CProjectPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	RemoveCurrentObject();
	Release();	// delete myself

	return S_OK;
}

void CProjectPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::SETOBJECT
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | SetObject | 
	Sets the <o PropPageObject> being displayed by the <o PropPageManager>.

@comm
	The PropPageManager object defines the set of pages placed in the Framework's property
	sheet.  For example, the PropPageManager used to display Style properties might place
	two (2) pages, or tabs, in the Framework's property sheet control.  Different Styles
	would use the same PropPageManager to show their properties.  Calls to
	<om IDMUSProdPropPageManager::SetObject> would result in properties being displayed for
	different Styles.

@parm IDMUSProdPropPageObject* | pIPropPageObject | [in] Pointer to the <i IDMUSProdPropPageObject>
		interface of the object to be displayed.

@rvalue S_OK | The PropPageManager's current object was set to <p pIPropPageObject>.
@rvalue E_INVALIDARG | <p pIPropPageObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred.

@ex The following example sets the PropPageObject being displayed by the PropPageManager: |

HRESULT CProjectPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();

	m_pIPropPageObject = pINewPropPageObject;
//	m_pIPropPageObject->AddRef();		intentionally missing

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}

void CProjectPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <om IDMUSProdPropPageManager::RemoveObject>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::REMOVEOBJECT
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | RemoveObject | 
	Removes the object specified in <p pIPropPageObject> from the <o PropPageManager>.

@comm
	When <p pIPropPageObject> equals the object currently being displayed by the
	PropPageManager, it is removed from the PropPageManager and is no longer displayed
	in the <o Framework>'s property sheet.

@parm IDMUSProdPropPageObject* | pIPropPageObject | [in] Pointer to the <i IDMUSProdPropPageObject>
		interface of the object to be removed.

@rvalue S_OK | <p pIPropPageObject> was removed successfully.
@rvalue E_INVALIDARG | <p pIPropPageObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and <p pIPropPageObject> was not removed.

@ex The following example removes the object specified in <p pIPropPageManager> from
	the PropPageManager: |

HRESULT CProjectPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pIPropPageObject == NULL)
	||  (pIPropPageObject != m_pIPropPageObject) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();
	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}

void CProjectPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <om IDMUSProdPropPageManager::SetObject>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::ISEQUALOBJECT
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | IsEqualObject | 
	Determines whether the object specified in <p pIPropPageObject> is being displayed by
	the <o PropPageManager>.

@parm IDMUSProdPropPageObject* | pIPropPageObject | [in] Pointer to the <i IDMUSProdPropPageObject>
		interface to compare with the PropPageManager's current object.

@rvalue S_OK | The object specified in <p pIPropPageObject> is being displayed by the
		PropPageManager.
@rvalue S_FALSE | The object specified in <p pIPropPageObject> is not being displayed by
		the PropPageManager.

@ex The following example determines whether the object specified in <p pIPropPageObject>
	is currently being displayed by the PropPageManager: |

HRESULT FAR EXPORT CProjectPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIPropPageObject == m_pIPropPageObject )
	{
		return S_OK;
	}

	return S_FALSE;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::REFRESHDATA
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | RefreshData | 
	Notifies the <o PropPageManager> to refresh its data and redraw the active page.

@comm
	This method must be called when the <o PropPageObject> being displayed changes by some
	means other than the <o Framework>'s property sheet control.  For example, when the
	user changes a value in an editor for an object whose properties are being displayed,
	<om IDMUSProdPropPageManager::RefreshData> must be called on order for the property sheet 
	to display the new value.
	
	<om IDMUSProdPropPageObject::GetData> must be called to retrieve the changed data before
	refreshing the property sheet's active page.

@rvalue S_OK | The PropPageObject's data was refreshed successfully.
@rvalue E_FAIL | An error occurred, and the PropPageObject's data could not be refreshed.

@ex The following example refreshes the Framework's property sheet control with the
	new data associated with the PropPageObject currently being displayed: |

HRESULT CProjectPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CProject* pProject;
	
	if( m_pIPropPageObject == NULL )
	{
		pProject = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pProject ) ) )
	{
		return E_FAIL;
	}

	m_pTabGeneral->SetProject( pProject );
	m_pTabFolders->SetProject( pProject );

	return S_OK;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEMANAGER::ISEQUALPAGEMANAGERGUID
========================================================================================
@method HRESULT | IDMUSProdPropPageManager | IsEqualPageManagerGUID | 
	Determines whether the GUID specified in <p rguidPageManager> equals the GUID of the
	<o PropPageManager>.

@parm REFGUID | rguidPageManager | [in] GUID to compare with the PageManager's GUID.

@rvalue S_OK | The GUID specified in <p rguidPageManager> equals the PropPageManager's
		GUID.
@rvalue S_FALSE | The GUID specified in <p rguidPageManager> does not equal the
		PropPageManager's GUID.

@ex The following example determines whether the GUID specified in <p rguidPageManager>
	equals the GUID of the PropPageManager: |

HRESULT FAR EXPORT CProjectPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_ProjectPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/
