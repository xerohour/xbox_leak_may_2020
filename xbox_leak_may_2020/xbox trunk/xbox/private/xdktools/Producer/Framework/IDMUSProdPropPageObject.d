
---------------------------------------------------------
IDMUSProdPropPageObject AutoDocs for DirectMusic Producer
---------------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODPROPPAGEOBJECT
========================================================================================
@interface IDMUSProdPropPageObject | 
	This interface provides access to an object's data so that a <o PropPageManager> object
	can display it in the <o Framework>'s global property sheet. 

@comm
	The Framework exports an <i IDMUSProdPropSheet> interface so that DirectMusic Producer
	<o Component>s can hook into its global property sheet control.  A pointer to the
	Framework's IDMUSProdPropSheet interface can be obtained via <om IDMUSProdFramework::QueryInterface>.
	
	In order to use the global property sheet, a Component must implement <i IDMUSProdPropPageManager>.
	The object behind this interface, the <o PropPageManager> object, manages the set of pages
	placed in the property sheet as well as the data currently displayed in those pages.  
	
	The PropPageManager uses the <i IDMUSProdPropPageObject> interface to interact with the object
	whose data is being shown in its pages.  Calls to <om IDMUSProdPropPageManager::SetObject> set
	the PropPageManager's current object so that the same set of pages can be used to display
	data for different objects.

@base public | IUnknown

@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>

@meth HRESULT | GetData | Hands data to the PropPageManager so that it can be displayed 
		in the PropPageManager's property sheet pages.
@meth HRESULT | SetData | Receives data from the PropPageManager so that it can refresh
		itself with changes made through the PropPageManager's property sheet pages.
@meth HRESULT | OnShowProperties | Displays the object's properties.
@meth HRESULT | OnRemoveFromPageManager | Called when the PropPageObject is being
		removed from the PropPageManager.
 --------------------------------------------------------------------------------------*/


/*======================================================================================
OBJECT:  DIRECTMUSIC PRODUCER PROPPAGEOBJECT OBJECT
========================================================================================
@object PropPageObject | Represents the data shown in the <o Framework>'s property sheet
		control.

@supint IDMUSProdPropPageObject | Defines the behavior of a PropPageObject object.
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
METHOD:  IDMUSPRODPROPPAGEOBJECT::GETDATA
========================================================================================
@method HRESULT | IDMUSProdPropPageObject | GetData | 
	Hands data to the <o PropPageManager> so that it can be displayed in the PropPageManager's
	property sheet pages.

@parm void** | ppData | [out,retval] Address of a variable to receive the requested
		PropPageObject's data.

@rvalue S_OK | The <o PropPageObject>'s data was returned successfully.
@rvalue E_POINTER | <p ppData> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | The data could not be returned due to lack of memory.
@rvalue E_FAIL | An error occurred, and the data could not be returned for some reason
		other than lack of memory.
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEOBJECT::SETDATA
========================================================================================
@method HRESULT | IDMUSProdPropPageObject | SetData | 
	Receives data from the <o PropPageManager> so that it can refresh itself with changes
	made through the PropPageManager's property sheet pages.

@parm void* | pNewData | [in] Pointer to the PropPageObject's data.

@rvalue S_OK | The <o PropPageObject>'s data was retrieved successfully.
@rvalue E_INVALIDARG | <p pNewData> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred, and the PropPageManager could retrieve the object's
		data for some reason other than lack of memory or invalid arguments.
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEOBJECT::ONSHOWPROPERTIES
========================================================================================
@method HRESULT | IDMUSProdPropPageObject | OnShowProperties | 
	Displays the object's properties.

@rvalue S_OK | The properties were displayed successfully.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred, and the properties could not be shown for some reason
		other than lack of memory.

@ex The following example displays properties for the <o PropPageObject>: |

HRESULT CProject::OnShowProperties( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get the Project page manager
	CProjectPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_ProjectPropPageManager ) == S_OK )
	{
		pPageManager = (CProjectPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CProjectPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Project properties
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropSheet();
	if( pIPropSheet )
	{
		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPROPPAGEOBJECT::ONREMOVEFROMPAGEMANAGER
========================================================================================
@method HRESULT | IDMUSProdPropPageObject | OnRemoveFromPageManager | 
	Called when the <o PropPageObject> is being removed from the <o PropPageManager>.

@comm
	The PropPageManager calls this method to notify the PropPageObject that it is being
	removed from the <o Framework>'s property sheet control.  This gives the PropPageObject
	an opportunity to perform cleanup processing.

@rvalue S_OK | Resources were freed successfully.
@rvalue E_FAIL | An error occurred, and resources could not be freed.
	
@xref <o PropSheet> Object, <o PropPageManager> Object, <o PropPageObject> Object, <i IDMUSProdPropSheet>, <i IDMUSProdPropPageManager>, <i IDMUSProdPropPageObject>
--------------------------------------------------------------------------------------*/
