// @doc Conductor

/*  --------------------------------------------------------------------------
	@struct ConductorNotifyEvent | Contains the event structure passed to the
		<om IDMUSProdNotifyCPt.OnNotify> method.

	@field long | m_cbSize | Size of the data passed in m_pbData.
	@field DWORD | m_dwType | Type of event passed in m_pbData.
	@field BYTE * | m_pbData | Pointer to a DMUS_NOTIFICATION_PMSG.

	@xref  <om IDMUSProdNotifyCPt.OnNotify>, <om IDMUSProdConductor.RegisterNotify>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdNotifyCPt | The Conductor uses this interface to communicate performance
		notification events to interested editors.

	@meth HRESULT | OnNotify | Called when a notification is available.

	@base public | IUnknown

	@xref	<i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterNotify>, <om IDMUSProdConductor.UnregisterNotify>
	--------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------
	@method HRESULT | IDMUSProdNotifyCPt | OnNotify | This method is called when a notification
		is available for an editor to process.

	@parm	ConductorNotifyEvent* | pConductorNotifyEvent | Contains a pointer to the <t ConductorNotifyEvent>
		structure that contains the notification event.

	@comm	The implementation of this method should take as little time as possible, to ensure timely
		delivery of notifications to all editors.  If too much time is spent here, notification messages
		will be lost.<nl>
		This method is called in a thread separate from the application's main message
		processing thread.

	@rvalue S_OK | The operation was successful

	@xref  <i IDMUSProdNotifyCPt>, <t ConductorNotifyEvent>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdMidiInCPt | The Conductor uses this interface to communicate MIDI input
		messages to interested editors.

	@meth HRESULT | OnMidiMsg | Called when a MIDI input message is available

	@base public | IUnknown

	@xref	<i IDMUSProdConductor>
	--------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------
	@method HRESULT | IDMUSProdMidiInCPt | OnMidiMsg | This method is called when a MIDI input
		message is available for handling.

	@parm	REFERENCE_TIME | rtTime | The time stamp of the event.
	@parm	BYTE | bStatus | The MIDI status byte.
	@parm	BYTE | bData1 | The first MIDI data byte.
	@parm	BYTE | bData2 | The second MIDI data byte.

	@comm	The implementation of this method should take as little time as possible, to ensure
		timely delivery of MIDI input to all editors.<nl>
		This method is called in a thread separate from the application's main message
		processing thread.

	@rvalue S_OK | The operation was successful

	@xref  <i IDMUSProdMidiInCPt>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdConductorTempo | The Conductor uses this interface to interact with the editors
		that need to use the functionality provided by the Tempo edit control in the transport toolbar.

	@meth HRESULT | SetTempo | Called when the user changes the tempo in the tempo edit control.

	@meth HRESULT | GetTempo | Called to get the tempo displayed in the tempo edit control.

	@base public | IUnknown

	@comm To support tempo editing from the Transport Control toolbar, this interface must be supported
		by the same object that implements <i IDMUSProdTransport>

	@xref	<i IDMUSProdConductor>, <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------
	@method HRESULT | IDMUSProdConductorTempo | SetTempo | This method is called when the user changes
		the tempo displayed in the Tempo edit control.

	@parm	double | dblTempo | The new tempo set by the user.  It is limited to the
		range DMUS_TEMPO_MIN to DMUS_TEMPO_MAX.

	@rvalue S_OK | The operation was successful

	@xref  <i IDMUSProdConductorTempo>
	--------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------
	@method HRESULT | IDMUSProdConductorTempo | GetTempo | This method is called to update the tempo
		displayed in the Transport toolbar's Tempo edit control.

	@parm	double* | pdblTempo | A pointer to the location to store the tempo in.
		The returned tempo should be within the range DMUS_TEMPO_MIN to DMUS_TEMPO_MAX.

	@rdesc	Depending on the return code from this method, the tempo edit control
		will display and allow editing of the tempo, display and disallow editing of
		the tempo, or clear the display and disallow editing of the tempo.

	@rvalue S_OK | The operation was successful, and the tempo should be editable.
	@rvalue S_FALSE | The operation was successful, but disallow editing of the tempo information.
	@rvalue E_NOTIMPL | Clear the tempo edit control, and disallow editing of the tempo information.

	@xref  <i IDMUSProdConductorTempo>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdPortNotify | The Conductor uses this interface to notify interested components when
		the user changes the output port configuration.

	@meth HRESULT | OnOutputPortsChanged | Called after the user changes which ports are active.

	@meth HRESULT | OnOutputPortsRemoved | Called when all output ports are removed.

	@comm	When the output port configuration is changed, disabled, or re-enabled, the appropriate method
		will be called on each <i IDMUSProdComponent> that supports this interface.

	@base public | IUnknown

	@xref	<i IDMUSProdConductor> <i IDMUSProdComponent>
	--------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------
	@method HRESULT | IDMUSProdPortNotify | OnOutputPortsChanged | This method is called after the output
		port configuration has been changed.

	@comm	This method is called when:<nl>
			1. The user switches to the other port configuration by pressing the [1] or [2] button.<nl>
			2. The user changed the port configuration with the Port Configuration editor.<nl>
			3. Output is enabled by depressing the MIDI Enable button.

	@comm	The UI will be frozen until this method returns.

	@comm	<om IDMUSProdPortNotify.OnOutputPortsRemoved> will always be called prior to this method.

	@ex		The following code gets the current port configuration: |

// Look for DirectMusic ports for their capabilities
IDirectMusicPort *pPort;
DMUS_PORTCAPS dmPortCaps;

// Iterate through all groups that include PChannels 0-999, inclusive
for(int iGroup = 0; iGroup <= 999/16; iGroup++)
{
	pPort = NULL;
	if( SUCCEEDED( m_pIPerformance->PChannelInfo( iGroup * 16, &pPort, NULL, NULL ) )
	 && (pPort != NULL) )
	{
		// Check if the port supports DLS
		dmPortCaps.dwSize = sizeof(dmPortCaps);
		if( SUCCEEDED( pPort->GetCaps( &dmPortCaps ) ) )
		{
			if( dmPortCaps.dwFlags & DMUS_PC_DLS )
			{
				// Port supports DLS - add it to our list
			}
		}

		// Release our reference on the port
		pPort->Release();
	}
}

	@rvalue S_OK | The operation was successful

	@xref  <i IDMUSProdPortNotify> <om IDMUSProdPortNotify.OnOutputPortsRemoved>
	--------------------------------------------------------------------------*/

/*	--------------------------------------------------------------------------
	@method HRESULT | IDMUSProdPortNotify | OnOutputPortsRemoved | This method is called after all output
		ports have been removed.

	@comm	This method is called when:<nl>
			1. The user changed the port configuration with the Port Configuration editor.<nl>
			2. Output is disabled by depressing the MIDI Enable button.

	@comm	The UI will be frozen until this method returns.

	@comm	<om IDMUSProdPortNotify.OnOutputPortsChanged> will be called after the output ports have
		been added to the performance.

	@rvalue S_OK | The operation was successful

	@xref  <i IDMUSProdPortNotify> <om IDMUSProdPortNotify.OnOutputPortsChanged>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdSecondaryTransport | The Conductor uses this interface to interact with editors
		that want to use the functionality provided by the secondary segment toolbar.

	@meth HRESULT | GetSecondaryTransportSegment | When the user hits a play button, this method is called to retrieve a segment to play.

	@meth HRESULT | GetSecondaryTransportName | This method is called to retrieve the name that should be displayed in the secondary segment toolbar.

	@base public | IUnknown

	@xref	<i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterSecondaryTransport>, <om IDMUSProdConductor.UnRegisterSecondaryTransport>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdSecondaryTransport | GetSecondaryTransportSegment | Returns a pointer to the
		segment to play.

	@parm IUnknown ** | ppunkSegment | Pointer to the location to store a the segment interface in.

	@comm	This method is called when the user presses a play button in the secondary segment toolbar
		while this <i IDMUSProdSecondaryTransport> is chosen.

	@rvalue S_OK | The segment was returned successfully.
	@rvalue E_POINTER | The address in <p pIUnknown> is not valid.  For example, it may be NULL.
	@rvalue E_FAIL | An error occurred.

	@xref  <i IDMUSProdSecondaryTransport>
	--------------------------------------------------------------------------*/

/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdSecondaryTransport | GetSecondaryTransportName | Returns text used for the label
		displayed in the secondary segment dropdown.

	@parm BSTR* | pbstrName | Pointer to the caller-allocated variable that receives a copy of the name.
		  The caller frees <p pbstrName> with SysFreeString when it is no longer needed.

	@rvalue S_OK | The name was returned successfully.
	@rvalue E_POINTER | The address in <p pbstrName> is not valid.  For example, it may be NULL.
	@rvalue E_FAIL | An error occurred and the name was not returned.

	@comm	If the name of the secondary transport changes, the transport should call
		<om IDMUSProdConductor.SetSecondaryTransportName>.

	@xref  <i IDMUSProdSecondaryTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdTransport | The Conductor uses this interface to interact with the editors that
		need to use the functionality provided by the transport toolbar.

	@meth HRESULT | Play | Starts playback of the associated transport.

	@meth HRESULT | Stop | Stops playback of the associated transport.

	@meth HRESULT | Record | Starts or stops recording of the associated transport.

	@meth HRESULT | Transition | Starts a transition to the associated transport.

	@meth HRESULT | GetName | Returns text used for this Transport's label.

	@meth HRESULT | TrackCursor | Callback which receives the state of the 'Play Cursor'
		button on the 'Transport Options' toolbar. 

	@base public | IUnknown

	@xref  <i IDMUSProdConductor>, <om IDMUSProdConductor.RegisterTransport>, <om IDMUSProdConductor.UnRegisterTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTransport | Play | This method starts playback of the associated transport.
		It is called in response to the user clicking on the play button in the transport toolbar.

	@parm   BOOL | fPlayFromStart | TRUE if the transport should play from its beginning.  If FALSE, the
		transport should start playback from the current cursor position.

	@rvalue S_OK | The operation was successful

	@rdesc	If this method succeeds, the conductor assumes the transport was able to start playback.
		  If this method fails, the conductor assumes the transport was not able to start playback.

	@xref  <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTransport | Stop | This method stops playback of the associated transport.
		It is called in response to the user clicking on the stop button in the transport toolbar.

	@parm   BOOL | fStopImmediate | TRUE if the transport should stop immediately. FALSE if the transport
		should stop at its default boundary.

	@rvalue S_OK | The operation was successful

	@rdesc	If this method succeeds, the conductor assumes the transport was able to stop playback.
		  If this method fails, the conductor assumes the transport was not able to stop playback.

	@xref  <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTransport | Record | This method starts or stops recording of the associated transport.
		It is called in response to the user clicking on the record button in the transport toolbar.

	@parm   BOOL | fPressed | TRUE if the button was pressed. FALSE if the button was unpressed.

	@rvalue S_OK | The operation was successful

	@rdesc	If this method succeeds, the conductor assumes the transport was able to start or stop recording.
		If this method fails, the conductor assumes the transport was not able to start or
		stop recording and will set the record button to its previous state.

	@comm	This method will only be called after the record button has been enabled, by passing BS_REC_ENABLED
		as a flag to either <om IDMUSProdConductor.SetBtnStates> or <om IDMUSProdConductor.RegisterTransport>.

	@xref  <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTransport | Transition | This method starts a transition to the associated transport.
		It is called in response to the user clicking on the transition button in the transport toolbar.

	@rvalue S_OK | The operation was successful

	@rdesc If this method succeeds, the conductor assumes the transport was able to successfully
		  transition.<nl>
		  If this method fails, the conductor assumes the transport was unable to transition
		  successfully.  It will then attempt a default transition by calling <om IDMUSProdTransport::Stop> on the
		  currently playing transport, followed by <om IDMUSProdTransport::Play> on the currently
		  active transport.

	@xref  <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTransport | GetName | Returns text used for this <i IDMUSProdTransport>'s label. 

	@parm BSTR* | pbstrName | Pointer to the caller-allocated variable that receives a copy of the name.
		  The caller must free <p pbstrName> with SysFreeString when it is no longer needed.

	@rvalue S_OK | The name was returned successfully.
	@rvalue E_POINTER | The address in <p pbstrName> is not valid.  For example, it may be NULL.
	@rvalue E_FAIL | An error occurred and the name was not returned.

	@comm	If the name of the transport changes, the transport should call
		<om IDMUSProdConductor.SetTransportName>.

	@xref  <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/


/*  --------------------------------------------------------------------------
	@method HRESULT | IDMUSProdTransport | TrackCursor | Callback which receives the state of the
		Play Cursor button on the Transport Options toolbar.

	@parm BOOL | fTrackCursor | TRUE when the user wants the cursor to track playback.
			FALSE when the user does not want the cursor to move during playback. 

	@comm	This method is called when the user toggles the 'Play Cursor' button while
		<i IDMUSProdTransport> "owns" the Transport Control toolbar.

	@comm	If a transport is currently playing, that transport "owns" the toolbar.  If no transport is currently
		playing, the active transport "owns" the toolbar.

	@rvalue S_OK | Always succeeds.

	@xref  <i IDMUSProdTransport>
	--------------------------------------------------------------------------*/
