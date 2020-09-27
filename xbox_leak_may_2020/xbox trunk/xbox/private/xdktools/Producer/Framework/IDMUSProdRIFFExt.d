
--------------------------------------------------
IDMUSProdRIFFExt AutoDocs for DirectMusic Producer
--------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODRIFFEXT
========================================================================================
@interface IDMUSProdRIFFExt | 
	The IDMUSProdRIFFExt interface is an agreement between two Components.  Components implementing
	IDMUSProdRIFFExt agree to load RIFF chunks and manage Project Tree nodes for other Components.

@comm
	To obtain a pointer to another Component's IDMUSProdRIFFExt interface, call QueryInterface on
	the <i IDMUSProdComponent> interface pointer returned by <om IDMUSProdFramework.FindComponent>.

@base public | IUnknown

@xref <om IDMUSProdFramework.FindComponent>, <i IDMUSProdNode>

@meth HRESULT | LoadRIFFChunk | Loads a RIFF chunk and returns an IDMUSProdNode interface
		pointer to the top node of data contained in the chunk.
--------------------------------------------------------------------------------------*/


/*======================================================================================
METHOD:  IDMUSPRODRIFFEXT::LOADRIFFCHUNK
========================================================================================
@method HRESULT | IDMUSProdRIFFExt | LoadRIFFChunk | 
	Loads a RIFF chunk and returns an <i IDMUSProdNode> interface pointer to the top node of
	data contained in the chunk.

@comm 
	Components implementing IDMUSProdRIFFExt::LoadRIFFChunk agree to load RIFF chunks and manage
	Project Tree nodes for other Components.  A DirectMusic Producer Component obtains a pointer
	to another Component's IDMUSProdRIFFExt interface by calling QueryInterface on the
	<i IDMUSProdComponent> interface pointer returned from <om IDMUSProdFramework.FindComponent>.

@parm IStream*| pIStream | [in] Pointer to the stream from which the RIFF chunk should be
		loaded.
@parm IDMUSProdNode**| ppITopNode | [out, retval] Address of a variable to receive the requested
	<i IDMUSProdNode> interface.  If LoadRIFFChunk fails, *ppITopNode is set to NULL.  On success,
	the caller is responsible for calling <om IDMUSProdNode.Release> when this pointer is no longer
	needed.

@rvalue S_OK | The RIFF chunk was successfully loaded and newly created <i IDMUSProdNode> was
		returned in <p ppITopNode>.
@rvalue E_OUTOFMEMORY | The RIFF chunk could not be loaded due to lack of memory.
@rvalue E_FAIL | The RIFF chunk could not be loaded for some reason other than lack
		of memory.

@ex The following example parses a RIFF file and hands a RIFF chunk to a different
		component for loading: |

HRESULT CStyle::LoadStyle( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	IStream*        pIStream;
	IDMUSProdComponent* pIComponent;
	IDMUSProdRIFFExt*   pIRIFFExt;
	IDMUSProdNode*      pITopNode;
	MMCKINFO        ck;
	DWORD           dwPos;
	HRESULT         hr = S_OK;

	pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	dwPos = StreamTell( pIStream );

	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			...
			... 
			...

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					...
					...
					...

					case FOURCC_BAND_FORM:
						hr = m_pIFramework->FindComponent( CLSID_BandComponent, &pIComponent );
						if( SUCCEEDED ( hr ) )
						{
							hr = pIComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
							if( SUCCEEDED ( hr ) )
							{
								StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
								hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pITopNode );
								if( SUCCEEDED ( hr ) )
								{
									m_lstBands.AddTail( pITopNode );
								}
								pIRIFFExt->Release();
							}
							pIComponent->Release();
						}

						if( FAILED ( hr ) )
						{
							go to ON_ERR;
						} 
						break;
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
		dwPos = StreamTell( pIStream );
	}

ON_ERR:
	pIStream->Release();
	return hr;
}
	
@xref <om IDMUSProdFramework.FindComponent>, <i IDMUSProdRIFFExt>
--------------------------------------------------------------------------------------*/
