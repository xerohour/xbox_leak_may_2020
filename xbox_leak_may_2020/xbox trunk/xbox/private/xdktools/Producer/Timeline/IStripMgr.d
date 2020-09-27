// @doc Timeline


/*
	@enum SegmentGUIDs | GUIDs of Parameters or Notifications used in the Segment Designer
		@emem CLSID_* | When the user adds or deletes a track, the Segment Designer broadcasts a notifiction with
			the CLSID of the StripMgr that was added or removed.
		@emem GUID_ChordIndividualChordSharpsFlats | <p pData> is a pointer to a BOOL that gets or sets the
			sharps/flats flag for the chord at mtTime.
		@emem GUID_ChordKey | <p pData> is a pointer to a DWORD that stores the number of accidentals in the
			high word, and the key root in the low word.
		@emem GUID_ChordSharpsFlats | <p pData> is a pointer to a BOOL that stores whether the key signature
			uses flats or sharps.  If TRUE, it uses flats.  If FALSE, it uses sharps.
		@emem GUID_Conductor_OutputPortsChanged | This notification is broadcast when the output port is changed
			or enabled.  See <om IDMUSProdPortNotify.OnOutputPortsChanged>.
		@emem GUID_Conductor_OutputPortsRemoved | This notification is broadcast when the output port is removed
			or disabled.  See <om IDMUSProdPortNotify.OnOutputPortsRemoved>.
		@emem GUID_DocRootNode | <p pData> is a pointer to an <i IDMUSProdNode> interface that is on the
			segment node.  It is set immediately after a strip manager is created, for strip managers that
			support it.
		@emem GUID_Segment_AllTracksAdded | This notification is broadcast after all strip managers have
			been added to the Timeline.
		@emem GUID_Segment_BandTrackChange | This notification is broadcast when a change is made to a band strip in
			the segment.  <p dwGroupBits> are the groups the band track belongs to.
		@emem GUID_Segment_CreateTrack | This notification is send to a strip manager after it is added to the
			segment while the segment editor is open.
		@emem GUID_Segment_DeletedTrack | This notification is sent to a strip manager after is is deleted from
			the segment while the segment editor is open.  <p pData> is a pointer to the timeline.
		@emem GUID_Segment_DisplayContextMenu | If an editor supports this parameter, the timeline will display a
			generic context menu when areas outside the strip display are right-clicked on.
		@emem GUID_Segment_ExtensionLength | This notification is broadcast when the user changes the extension bar
			length from the segment property page.  <p pData> is a pointer to a DWORD that contains the number of
			extension bars to display.  This is also used for retrieving the number of extension bars from the StripMgr.
			If the StripMgr manages more than one strip, and the strips display a differing number of extension bars,
			0xFFFFFFFF should be stored in the DWORD.
		@emem GUID_Segment_FrameworkMsg | This notification is broadcast when the segment node receives a notification
			from the Framework.  <p pData> is a pointer to a <t DMUSProdFrameworkMsg> structure containing information
			about the message.
		@emem GUID_Segment_Length_Change | This notification is broadcast after the length of the segment changes.
			<p pData> is a pointer to a MUSIC_TIME that stores the old length of the segment.
		@emem GUID_Segment_NewStyleSelected | This notification is broadcast when a style is added to the style
			reference strip.  <p pData> is a pointer to the MUSIC_TIME the style was added at. <p dwGroupBits> contain
			the group the style was added to.
		@emem GUID_Segment_PickupLength | This notification is broadcast when the user changes the pickup bar
			length from the segment property page.  <p pData> is a pointer to a DWORD that contains either 0 or 1,
			denoting the number of pickup bars to display.  This is also used for retrieving the number of pickup bars
			from the StripMgr.	If the StripMgr manages more than one strip, and the strips display a differing number
			of extension bars, 0xFFFFFFFF should be stored in the DWORD.
		@emem GUID_Segment_RecordButton | This notification is broadcast when the state of the record button changes.
			It is also sent to each strip manager when they are first added to the segment so they will know the
			initial state of the record button. <p pData> is a pointer to a BOOL that is TRUE when the record button
			is pressed.
		@emem GUID_Segment_Set_Tempo | When the user changes the tempo in the transport toolbar, this notification is
			sent to the tempo strip manager that contains the currently active tempo.  <p pData> is a pointer to a
			DMUS_TEMPO_PARAM structure containing the new tempo.
		@emem GUID_Segment_Start | This notification is broadcast when a segment is queued to start.  <p pData>
			is a pointer to an <i IDirectMusicSegmentState>.  Is is also sent to each strip manager when they are
			added after the segment has been queued to start.
		@emem GUID_Segment_Stop | This notification is broadcast when the segment stops playing.
		@emem GUID_Segment_Undo_BSTR | <p pData> is a pointer to a BSTR that stores the strip to display
			for the undo and redo menu items.  After an <i IDMUSProdStripMgr> calls <om IDMUSProdTimeline.OnDataChanged>,
			the segment designer asks the <i IDMUSProdStripMgr> for this parameter.
		@emem GUID_Segment_WindowActive | This notification is broadcast when the activation state of the segment
			designer window changes.  It is also sent to each strip manager when they are first added to the segment
			so they will know the initial activation state of the segment designer window.  <p pData> is a pointer
			to a BOOL that is TRUE then the window is active.
		@emem GUID_Segment_WindowClosing | This notification is broadcast just before the segment window closes.
		@emem GUID_TimelineSetCursor | This notification is broadcast when the position of the time cursor changes
			as a result of a call to <om IDMUSProdTimeline::SetMarkerTime>, typically during playback.
			<p pData> is a pointer to the MUSIC_TIME of the new cursor position.
		@emem GUID_TimelineSetSegStartTime | <p pData> is a pointer to a REFERENCE_TIME.  If the time is 0, the
			real-time display will be frozen at its last value.  If the time is not zero, the real-time display will
			show the amount of time elapsed since the given REFERENCE_TIME.
		@emem GUID_TimelineSetSnapTo | This notification is broadcast when the snap-to setting changes.
			<p pData> is a pointer to the new <t DMUSPROD_TIMELINE_SNAP_TO> setting.
		@emem GUID_TimelineShowRealTime | <p pData> is a pointer to a HANDLE.  If the handle is NULL, or not set, no
			real-time display will be shown in the status bar.  If the handle is a valid handle from a call to
			<om IDMUSProdFramework.SetNbrStatusBarPanes>, a real-time display will be shown in the status bar.
		@emem GUID_TimelineShowTimeSig | If this parameter is set to FALSE, a time signature will note be displayed
			in the time strip.  If this parameter is not set, or set to TRUE, a time signature will be displayed.
		@emem GUID_TimelineUserSetCursor | This notification is broadcast when the position of the time cursor changes
			as the result of the user either clicking in the track or using shortcut keys to change its position.
			<p pData> is a pointer to the MUSIC_TIME of the new cursor position.
*/

/*
	@struct DMUSProdFrameworkMsg | Contains information from a <o Framework> notification.

	@comm When a Segment receives a Framework notification via <om IDMUSProdNotifySink.OnUpdate>, it repackages
	the method's arguments into a <t DMUSProdFrameworkMsg> structure and then sends interested StripMgrs a
	GUID_Segment_FrameworkMsg notification where <p pData> points to the <t DMUSProdFrameworkMsg> structure. 

	@field GUID | guidUpdateType | The type of Framework notification.
	@field IUnknown* | punkIDMUSProdNode | The node that sent the notification.
	@field void* | pData | A pointer to any data relevant to the notification.

	@xref <t SegmentGUIDs> <om IDMUSProdNotifySink.OnUpdate>
*/

/*
	@enum STRIPMGRPROPERTY | Strip Manager properties
		@emem SMP_ITIMELINECTL | VT_UNKNOWN.  Sets or retrieves an IUnknown pointer to the associated
			<i IDMUSProdTimeline> for this strip manager.  When this <i IDMUSProdStripMgr> is added to the Timeline by
			a call to <om IDMUSProdTimeline:: AddStripMgr>, this property will be set on the <i IDMUSProdStripMgr>
			with a pointer to the <i IDMUSProdTimeline> that the <i IDMUSProdStripMgr> was added to.  When this
			<i IDMUSProdStripMgr> is removed from the Timeline by a call to <om IDMUSProdTimeline::RemoveStripMgr>,
			this property will be set with the value NULL.
		@emem SMP_IDIRECTMUSICTRACK | VT_UNKNOWN.  Sets or retrieves an IUnknown pointer to the associated
			<i IDirectMusicTrack> for this strip manager.  When this <i IDMUSProdStripMgr> is first created, before
			it is added to the Timeline, this property will be set on the <i IDMUSProdStripMgr> with a pointer to
			the <i IDirectMusicTrack> that the <i IDMUSProdStripMgr> is associated with.  This property will be
			set only once over the lifetime of the <i IDMUSProdStripMgr> object.
		@emem SMP_IDMUSPRODFRAMEWORK | VT_UNKNOWN.  Sets or retrieves an IUnknown pointer to the associated
			<i IDMUSProdFramework> for this strip manager.  When this <i IDMUSProdStripMgr> is first created, before
			it is added to the Timeline, this property will be set on the <i IDMUSProdStripMgr> with a pointer to
			the <i IDMUSProdFramework> that the <i IDMUSProdStripMgr> is associated with.  This property will be
			set only once over the lifetime of the <i IDMUSProdStripMgr> object.
		@emem SMP_DMUSIOTRACKHEADER | VT_BYREF.  Sets or retrieves the track header information for this
			<i IDMUSProdStripMgr>.    When this <i IDMUSProdStripMgr> is first created, before it is added to the Timeline,
			this property will be set on the <i IDMUSProdStripMgr> with a pointer to the <t DMUS_IO_TRACK_HEADER>
			assocated with this <i IDMUSProdStripMgr>.  This property will be set only once over the lifetime of
			the <i IDMUSProdStripMgr> object.<nl>
			If this <i IDMUSProdStripMgr> does not support changing its group bits, it should not copy the dwGroupBits
			member of the <t DMUS_IO_TRACK_HEADER> when this property is set.  However, it must return the correct
			dwGroupBits when this property is read.
		@emem SMP_DMUSIOTRACKEXTRASHEADER | VT_BYREF.  Sets or retrieves the track extras header information for this
			<i IDMUSProdStripMgr>.    When this <i IDMUSProdStripMgr> is first created, before it is added to the Timeline,
			this property will be set on the <i IDMUSProdStripMgr> with a pointer to the <t DMUS_IO_TRACK_EXTRAS_HEADER>
			assocated with this <i IDMUSProdStripMgr>.  This property will be set only once over the lifetime of
			the <i IDMUSProdStripMgr> object.
		@emem SMP_PRODUCERONLY_FLAGS | VT_BYREF.  Sets or retrieves the producer-only information for this
			<i IDMUSProdStripMgr>.    When this <i IDMUSProdStripMgr> is first created, before it is added to the Timeline,
			this property will be set on the <i IDMUSProdStripMgr> with a pointer to the <t IOProducerOnlyChunk>
			assocated with this <i IDMUSProdStripMgr>.  This property will be set only once over the lifetime of
			the <i IDMUSProdStripMgr> object.
		@emem SMP_USER | First available user-defined strip manager property.
*/


//	-------------------------------------------------------------------------- 
//	@interface IDMUSProdStripMgr | The Timeline uses this interface to interact with the editors that
//		edit DirectMusic tracks inside a Segment.
//
//	@meth HRESULT | IsParamSupported | Determines whether the strip manager supports a given data type in the
//			<om IDMUSProdStripMgr::GetParam> and <om IDMUSProdStripMgr::SetParam> methods.
//
//	@meth HRESULT | GetParam | Retrieve data from a strip manager.
//
//	@meth HRESULT | SetParam | Set data in a strip manager.
//
//	@meth HRESULT | OnUpdate | Callback which notifies interested strip managers when data of type <p rguidType> changes.
//
//	@meth HRESULT | GetStripMgrProperty | Get a property of the strip manager.
//
//	@meth HRESULT | SetStripMgrProperty | Set a property of the strip manager. 
//
//	@base public | IUnknown
//
//	@xref  <i IDMUSProdStrip>, <i IDMUSProdStripFunctionBar>
//	-------------------------------------------------------------------------- 

/*
	@method HRESULT | IDMUSProdStripMgr | IsParamSupported | This method is analagous to
		<om IDirectMusicTrack::IsParamSupported>.  This method determines whether the track
		supports a given data type in the <om IDMUSProdStripMgr::GetParam> and <om IDMUSProdStripMgr::SetParam>
		methods.

	@parm   REFGUID | rguidType | Reference to (C++) or address of (C) the identifier of the type
		of data. See <t SegmentGUIDs> and the DirectMusic documentation of Track Parameter Types.

	@rdesc	If the method succeeds and the type is supported, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue	E_POINTER | An invalid pointer (usually NULL) was passed as a parameter.
	@rvalue	E_NOTIMPL | The parameter type is not supported.

	@xref	<i IDMUSProdStripMgr>, <om IDMUSProdStripMgr::GetParam> <om IDMUSProdStripMgr::SetParam> <t SegmentGUIDs>
*/

/*
	@method HRESULT | IDMUSProdStripMgr | GetParam | This method is analagous to the
		<om IDirectMusicTrack::GetParam>.  This method retrieves data from a StripMgr.

	@parm   REFGUID | rguidType | Reference to (C++) or address of (C) the identifier of the type
		of data to obtain. See <t SegmentGUIDs> and the DirectMusic documentation of Track Parameter Types.
	@parm   MUSIC_TIME | mtTime | Time from which to obtain the data.
	@parm   MUSIC_TIME* | pmtNext | Address of a variable to receive the time (relative to
		the current time) until which the data is valid. If this returns a value of 0, it means
		either that the data will always be valid, or that it is unknown when it will become
		invalid. If this information is not needed, <p pmtNext> can be set to NULL. 
	@parm   void* | pData | Address of an allocated structure in which the data is to be
		returned.  The structure must be of the appropriate kind and size for the data type
		identified by <p rguidType>. 

	@rdesc	If the method succeeds and the type is supported, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue	E_POINTER | An invalid pointer (usually NULL) was passed as a parameter.
	@rvalue	E_NOTIMPL | The parameter type is not supported.

	@xref	<i IDMUSProdStripMgr>, <om IDMUSProdStripMgr::IsParamSupported> <om IDMUSProdStripMgr::SetParam> <t SegmentGUIDs>
*/

/*
	@method HRESULT | IDMUSProdStripMgr | SetParam | This method is analagous to the
		<om IDirectMusicTrack::SetParam>.  This method sets data on a track.

	@parm   REFGUID | rguidType | Reference to (C++) or address of (C) the identifier of the type
		of data to obtain. See <t SegmentGUIDs> and the DirectMusic documentation of Track Parameter Types.
	@parm   MUSIC_TIME | mtTime | Time at which to set the data.
	@parm   void* | pData | Address of structure containing the data, or NULL if no data is
		required. The structure must be of the appropriate kind and size for the data type
		identified by <p rguidType>.

	@rdesc	If the method succeeds, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue	E_POINTER | An invalid pointer (usually NULL) was passed as a parameter.
	@rvalue	E_NOTIMPL | The parameter type is not supported.
	@rvalue E_OUTOFMEMORY | Insufficient memory to complete task.

	@xref	<i IDMUSProdStripMgr>, <om IDMUSProdStripMgr::IsParamSupported> <om IDMUSProdStripMgr::GetParam> <t SegmentGUIDs>
*/

/*
	@method HRESULT | IDMUSProdStripMgr | OnUpdate | The Timeline calls this method to notify interested StripMgrs
		when data of type <p rguidType> changes.

	@comm	<om IDMUSProdTimeline::AddToNotifyList> initiates the callback process. <om IDMUSProdTimeline::RemoveFromNotifyList>
			must be called when the StripMgr no longer wants to receive notifications.

	@parm   REFGUID | rguidType | Reference to (C++) or address of (C) the identifier of the type
		of data that changed. See <t SegmentGUIDs> and the DirectMusic documentation of Track Parameter Types.
	@parm   DWORD | dwGroupBits | Track groups in which the data changed. A value of 0 is invalid. Each
		bit in <p dwGroupBits> corresponds to a track group. If the data affects all groups, this parameter
		will be 0xFFFFFFFF.
	@parm   void* | pData | Address of structure containing the data, or NULL if no data is
		required. The structure must be of the appropriate kind and size for the data type
		identified by <p rguidType>.

	@rdesc	The most successful error code returned by this method will be returned by
		<om IDMUSProdTimeline::NotifyStripMgrs>.

	@xref	<i IDMUSProdStripMgr>, <t SegmentGUIDs>, <om IDMUSProdTimeline::NotifyStripMgrs>, <om IDMUSProdTimeline::AddToNotifyList>, <om IDMUSProdTimeline::RemoveFromNotifyList>
*/

/*
	@method HRESULT | IDMUSProdStripMgr | GetStripMgrProperty | This method gets a property of the strip.

	@parm   <t STRIPMGRPROPERTY> | stripMgrProperty | Which property to get.
	@parm   VARIANT* | pVar | Address of the variant to return the property in.

	@comm	<om IUnknown::AddRef> should be called on all valid <i IUnknown> interfaces
		returned by this method when S_OK is returned.  The caller is responsible for calling <om IUnknown::Release>.

	@rdesc	If the method succeeds, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue E_POINTER | <p pVar> is NULL.
	@rvalue E_INVALIDARG | <p stripMgrProperty> contains an unsupported property type.

	@xref	<i IDMUSProdStripMgr>, <t STRIPMGRPROPERTY> <om IDMUSProdStripMgr::SetStripMgrProperty>
*/

/*
	@method HRESULT | IDMUSProdStripMgr | SetStripMgrProperty | This method sets a property of the strip.

	@parm   <t STRIPMGRPROPERTY> | stripMgrProperty | Which property to set.
	@parm   VARIANT | var | The data to set the property with.

	@comm	If <p var> is of type VT_UNKNOWN and the <i IDMUSProdStripMgr> wishes to keep a local copy of
		the data, <om IUnknown::AddRef>	should be called on the passed interface.

	@rdesc	If the method succeeds, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue E_POINTER | <p var> contains a NULL pointer (except for <e STRIPMGRPROPERTY::SMP_ITIMELINECTL>,
		where NULL is a valid value).
	@rvalue E_INVALIDARG | <p stripMgrProperty> contains an unsupported property type.

	@xref	<i IDMUSProdStripMgr>, <t STRIPMGRPROPERTY> <om IDMUSProdStripMgr::GetStripMgrProperty>
*/
