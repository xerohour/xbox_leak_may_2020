
-----------------------------------------------------
IDMUSProdNotifySink AutoDocs for DirectMusic Producer
-----------------------------------------------------

/*-----------
@doc DMUSPROD
-----------*/


/*======================================================================================
INTERFACE:  IDMUSPRODNOTIFYSINK
========================================================================================
@interface IDMUSProdNotifySink | 
	If a <o Node> object implementing <i IDMUSProdNode> needs to know when a file it references
	changes it must also implement <i IDMUSProdNotifySink> and make itself known to the Framework
	via a call to <om IDMUSProdFramework.AddToNotifyList>.  Once this is done, the Framework
	notifies the object of changes via the method in the <i IDMUSProdNotifySink> interface.

@base public | IUnknown

@xref <om IDMUSProdFramework.AddToNotifyList>, <om IDMUSProdFramework.RemoveFromNotifyList>, <om IDMUSProdFramework.NotifyNodes>

@meth HRESULT | OnUpdate | The Framework calls this method to notify interested nodes when
		the state of <p pIDocRootNode> changes.
--------------------------------------------------------------------------------------*/



/*======================================================================================
METHOD:  IDMUSPRODNOTIFYSINK::ONUPDATE
========================================================================================
@method HRESULT | IDMUSProdNotifySink | OnUpdate | The Framework calls this method to notify
		interested <o Node>s when the state of <p pIDocRootNode> changes.
		
@comm
	If a <o Node> object implementing <i IDMUSProdNode> needs to know when a file it references
	changes it must also implement <i IDMUSProdNotifySink> and make itself known to the Framework
	via a call to <om IDMUSProdFramework.AddToNotifyList>.  Once this is done, the Framework
	notifies the object of changes via the <om IDMUSProdNotifySink.OnUpdate> method in the
	<i IDMUSProdNotifySink> interface.

	<p pIDocRootNode> is a pointer to the DocRoot node of the file that just changed.  A
	DocRoot node is the one and only child node of a File node.  DocRoot nodes mark the place
	in the Project Tree hierarchy where responsibility shifts from the <o Framework> to the
	<o Component>.

	<p pData> may be NULL.
	
	Valid <p guidUpdateType> and <p pData> values must be defined and documented in the .h
	and .lib files distributed by the Component supporting nodes of type <p pIDocRootNode>.
	For example, the Style Component's StyleDesigner.h and StyleDesignerGuid.lib files
	must contain and document the various <p guidUpdateType> and <p pData> values it
	uses to communicate Style file changes. 

	The following list contains notifications provided by the Framework:

	<tab><p guidUpdateType> <tab><tab><tab><p pData> <tab><tab><tab><p Description>

	<tab>FRAMEWORK_FileDeleted <tab><tab>NULL <tab><tab><tab>File deleted by user and is no longer in the Project Tree.

	<tab>FRAMEWORK_FileReplaced <tab>IDMUSProdNode* pINewDocRoot <tab>'Revert to Saved' caused DocRoot pointer to change.

	<tab>FRAMEWORK_FileClosed <tab><tab>NULL <tab><tab><tab>File closed normally and is no longer in the Project Tree.

	<tab>FRAMEWORK_FileNameChange <tab>NULL <tab><tab><tab>File was renamed via the Project Tree.

@parm IDMUSProdNode* | pIDocRootNode | [in] Pointer to the changed DocRoot's <i IDMUSProdNode>
	interface.
@parm GUID | guidUpdateType | [in] Identifies type of change.
@parm VOID* | pData | [in] Pointer to additional data associated with the change.

@rvalue S_OK | Always succeeds.  The Framework does not check the return code of
		<om IDMUSProdNotifySink.OnUpdate>.
 
@xref <i IDMUSProdNotifySink>, <om IDMUSProdFramework.AddToNotifyList>, <om IDMUSProdFramework.RemoveFromNotifyList>, <om IDMUSProdFramework.NotifyNodes>
--------------------------------------------------------------------------------------*/

