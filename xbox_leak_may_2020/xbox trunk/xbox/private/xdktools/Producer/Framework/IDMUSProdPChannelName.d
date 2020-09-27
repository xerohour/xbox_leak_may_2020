
---------------------------------------------------
IDMUSProdPChannelName AutoDocs for DirectMusic Producer
---------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODPCHANNELNAME
========================================================================================
@interface IDMUSProdPChannelName | 
	Allows PChannel names to be assigned on a <o Project> by Project basis.
	
	A pointer to a Project's IDMUSProdPChannelName interface can be obtained via its
	<om IDMUSProdProject::QueryInterface> method.

@base public | IUnknown

@xref <o Project> Object, <i IDMUSProdProject> 

@meth HRESULT | GetPChannelName | Gets the PChannel name associated with <p dwPChannel>.
@meth HRESULT | SetPChannelName | Sets the PChannel name associated with <p dwPChannel>.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPCHANNELNAME::GETPCHANNELNAME
========================================================================================
@method HRESULT | IDMUSProdPChannelName | GetPChannelName | Gets the PChannel name associated
		with <p dwPChannel>.
 
@comm
	Producer allows PChannel names to be assigned on a <o Project> by Project basis.  A
	pointer to a Project's <i IDMUSProdPChannelName> interface can be obtained via
	<om IDMUSProdProject::QueryInterface>.
@comm
	The names of broadcast PChannels cannot be changed, and a fixed name will be returned
	for these PChannels.

@parm DWORD | dwPChannel | [in] PChannel number.
@parm WCHAR* | pwszName | [out,retval] Address of a variable to receive the requested
	PChannel name.

@rvalue S_OK | The PChannel name was returned in <p pwszName>.
@rvalue E_FAIL | An error occurred and the PChannel name could not be returned.
@rvalue E_POINTER | NULL was passed for <p pwszName>.

@ex The following example gets the name associated with PChannel 2: |

	IDMUSProdProject* pIProject;
	IDMUSProdPChannelName* pIPChannelName;

	// Get the Project that this node belongs to...
	if( SUCCEEDED ( m_pIFramework->FindProject( pINode, &pIProject ) ) )
	{
		if( SUCCEEDED( pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
		{
			WCHAR wstrPChannelName[MAX_PATH];

			if( SUCCEEDED ( pIPChannelName->GetPChannelName( 2, wstrPChannelName ) ) )
			{
				...
				...
				...
			}

			pIPChannelName->Release();
		}

		pIProject->Release();
	}
	
@xref <i IDMUSProdPChannelName>, <om IDMUSProdPChannelName.SetPChannelName>, <i IDMUSProdProject>
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODPCHANNELNAME::SETPCHANNELNAME
========================================================================================
@method HRESULT | IDMUSProdPChannelName | SetPChannelName | Sets the PChannel name associated
		with <p dwPChannel>.
 
@comm
	Producer allows PChannel names to be assigned on a <o Project> by Project basis.  A
	pointer to a Project's <i IDMUSProdPChannelName> interface can be obtained via
	<om IDMUSProdProject::QueryInterface>.
@comm
	The names of broadcast PChannels cannot be changed.

@parm DWORD | dwPChannel | [in] PChannel number.
@parm WCHAR* | pwszName | [in] PChannel's new name.

@rvalue S_OK | The PChannel name was updated successfully.
@rvalue E_FAIL | An error occurred and the PChannel name could not be changed.
@rvalue E_POINTER | NULL was passed for <p pwszName>.
@rvalue E_INVALIDARG | A broadcast PChannel was passed for <p dwPChannel>.

@ex The following example sets the name associated with PChannel 2: |

	IDMUSProdProject* pIProject;
	IDMUSProdPChannelName* pIPChannelName;

	// Get the Project that this node belongs to...
	if( SUCCEEDED ( m_pIFramework->FindProject( pINode, &pIProject ) ) )
	{
		if( SUCCEEDED( pIProject->QueryInterface( IID_IDMUSProdPChannelName, (void**)&pIPChannelName ) ) )
		{
			WCHAR wstrOldPChannelName[MAX_PATH];
			WCHAR wstrNewPChannelName[MAX_PATH];

			if( SUCCEEDED( pIPChannelName->GetPChannelName( 2, wstrOldPChannelName) ) )
			{
				CString strOldPChannelName = wstrOldPChannelName;

				if( strOldPChannelName.Compare( strNewPChannelName ) != 0 )
				{
					MultiByteToWideChar( CP_ACP, 0, strNewPChannelName, -1, wstrNewPChannelName, MAX_PATH );
					pIPChannelName->SetPChannelName( 2, wstrNewPChannelName );
				}
			}
			
			pIPChannelName->Release();
		}

		pIProject->Release();
	}
	
@xref <i IDMUSProdPChannelName>, <om IDMUSProdPChannelName.GetPChannelName>, <i IDMUSProdProject>
--------------------------------------------------------------------------------------*/
