// @doc Timeline

/*  --------------------------------------------------------------------------
	@interface IDMUSProdTimelineEdit | This interface is used to perform edit operations
		on strips in the Timeline.

	@meth HRESULT | Cut | Copy the selected data, then delete it.

	@meth HRESULT | Copy | Copy the selected data.

	@meth HRESULT | Paste | Paste data from the clipboard into the strip.

	@meth HRESULT | Insert | Insert a new item into the strip.

	@meth HRESULT | Delete | Delete the selected data.

	@meth HRESULT | SelectAll | Select all data in the strip.

	@meth HRESULT | CanCut | Checks to see if 'Cut' is supported.

	@meth HRESULT | CanCopy | Checks to see if 'Copy' is supported.

	@meth HRESULT | CanPaste | Checks to see if 'Paste' is supported.

	@meth HRESULT | CanInsert | Checks to see if 'Insert' is supported.

	@meth HRESULT | CanDelete | Checks to see if 'Delete' is supported.

	@meth HRESULT | CanSelectAll | Checks to see if 'Select All' is supported.

	@base public | IUnknown

	@comm	To allow editing of a strip, this interface should be implemented on the same object that
			implements the <i IDMUSProdStrip> interface.

	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | Cut | Copy the selected data, then delete it.

	@parm   <i IDMUSProdTimelineDataObject>* | pIDataObject | If this is non-NULL, the method should add its
		data to the passed <p pIDataObject>.  If this is NULL, the method should add its data
		directly to the clipboard, preferably by using an <i IDMUSProdTimelineDataObject> object.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::Copy>, <om IDMUSProdTimelineEdit::Delete>,
		<i IDMUSProdTimelineDataObject>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | Copy | Copy the selected data.

	@parm   <i IDMUSProdTimelineDataObject>* | pIDataObject | If this is non-NULL, the method should add its
		data to the passed <p pIDataObject>.  If this is NULL, the method should add its data
		directly to the clipboard, preferably by using an <i IDMUSProdTimelineDataObject> object.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::Cut>, <i IDMUSProdTimelineDataObject>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | Paste | Paste data from the clipboard into the strip.

	@comm	Be sure to call <om IDMUSProdTimeline::GetPasteType> to ensure the correct paste operation
		is completed.

	@parm   <i IDMUSProdTimelineDataObject>* | pIDataObject | If this is non-NULL, the method should get its
		data from the passed <p pIDataObject>.  If this is NULL, the method should read its data
		directly to the clipboard, preferably by using an <i IDMUSProdTimelineDataObject> object.

	@xref <i IDMUSProdTimelineEdit>, <i IDMUSProdTimelineDataObject>, <om IDMUSProdTimeline::GetPasteType>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | Insert | Insert a new item into the strip.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::Delete>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | Delete | Delete the selected data.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::Insert>, <om IDMUSProdTimelineEdit::Cut>, <om IDMUSProdTimelineEdit::Copy>,
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | SelectAll | Select all data in the strip.

	@xref <i IDMUSProdTimelineEdit>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | CanCut | Checks to see if 'Cut' is supported.

	@rvalue S_OK | Cutting the selected data is currently supported.
	@rvalue S_FALSE | Cut is currently unsupported.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::CanCopy>, <om IDMUSProdTimelineEdit::CanDelete>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | CanCopy | Checks to see if 'Copy' is supported.

	@rvalue S_OK | Copying the selected data is currently supported.
	@rvalue S_FALSE | Copy is currently unsupported.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::CanCut>, <om IDMUSProdTimelineEdit::CanDelete>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | CanPaste | Checks to see if 'Paste' is supported.

	@parm   <i IDMUSProdTimelineDataObject>* | pIDataObject | If this is non-NULL, the method should check for
		supported formats in the passed <p pIDataObject>.  If this is NULL, the method should check
		the clipboard directly, preferably by using an <i IDMUSProdTimelineDataObject> object.

	@rvalue S_OK | A data format in either <p pIDataObject> or the clipboard is supported.
	@rvalue S_FALSE | No supported data formats found.

	@xref <i IDMUSProdTimelineEdit>, <i IDMUSProdTimelineDataObject>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | CanInsert | Checks to see if 'Insert' is supported.

	@rvalue S_OK | Inserting new data is currently supported.
	@rvalue S_FALSE | Insert is currently unsupported.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::CanDelete>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | CanDelete | Checks to see if 'Delete' is supported.

	@rvalue S_OK | Deleting the selected data is currently supported.
	@rvalue S_FALSE | Delete is currently unsupported.

	@xref <i IDMUSProdTimelineEdit>, <om IDMUSProdTimelineEdit::CanInsert>, <om IDMUSProdTimelineEdit::CanCut>, <om IDMUSProdTimelineEdit::CanCopy>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTimelineEdit | CanSelectAll | Checks to see if 'Select All' is supported.

	@rvalue S_OK | There is data available to be selected.
	@rvalue S_FALSE | No data is available to be selected.

	@xref <i IDMUSProdTimelineEdit>
	--------------------------------------------------------------------------*/
