/*
	@doc Timeline
*/

/*
	@enum STRIPVIEW | Strip views
		@emem SV_NORMAL | Normal strip view.  If a strip supports minimizing, this is the maximized
					view.
		@emem SV_MINIMIZED | Minimized strip view.
		@emem SV_FUNCTIONBAR_NORMAL | Normal functionbar view.  If a strip supports minimizing,
					this is the maximized view.
		@emem SV_FUNCTIONBAR_MINIMIZED | Minimized function bar view.
*/

/*
	@enum STRIPPROPERTY | Strip properties
		@emem SP_NAME | VT_BSTR.  Gets the name to display in the function bar title area.
		@emem SP_GUTTERSELECTABLE | VT_BOOL.  If supported and TRUE, the strip supports having its gutter
					selected.
		@emem SP_GUTTERSELECT | VT_BOOL.  Set when the gutter selection state of the strip changes.  If TRUE,
					the gutter is selected, otherwise the gutter is unselected.
		@emem SP_BEGINSELECT | VT_I4.  Sets the start of the timeline selection range.
		@emem SP_ENDSELECT | VT_I4.  Sets the end of the timeline selection range.
		@emem SP_CURSOR_HANDLE | HCURSOR (as VT_I4).  Gets a handle to the cursor to display.  If unsupported,
					the default arrow cursor is used.
		@emem SP_DEFAULTHEIGHT | VT_I4.  Gets the default height of the strip, in pixels.  If not supported,
					the Timeline tries to get SP_MAXHEIGHT, then SP_MINHEIGHT.  If none of these properties
					are supported, the strip is created with a height of 100 pixels.
		@emem SP_RESIZEABLE | VT_BOOL.  If supported and TRUE, the strip supports being vertically resized.
		@emem SP_MAXHEIGHT | VT_I4.  Gets the maximum height of the strip, in pixels.  Used when vertically
					resizing a strip that supports the SP_RESIZABLE property.  If not supported, an unlimited
					height is supported.
		@emem SP_MINHEIGHT | VT_I4.  Gets the minimum height of the strip, in pixels.  Used when vertically
					resizing a strip that supports the SP_RESIZABLE property.  If not supported, a default
					of 20 pixels is used.
		@emem SP_MINMAXABLE | VT_BOOL.  If supported and TRUE, the strip supports being minimized.  A resize
					button will be displayed in the strip's upper right-hand corner.
		@emem SP_MINIMIZE_HEIGHT | VT_I4.  Gets the height of the strip when it is in the SV_MINIMIZED view.
					If unsupported, a default of 20 pixels is used.  Only used when the strip supports the
					SP_MINMAXABLE property.
		@emem SP_STRIPMGR | VT_UNKNOWN.  Gets an IUnknown pointer to the associated <i IDMUSProdStripMgr>
					for this strip.
		@emem SP_FUNCTIONBAR_EXCLUDE_WIDTH | VT_I4.  Get the width of the title bar (measured from the right
					edge of the function bar) to exclude from automatic	double-click to minimize mapping.
					Only useful for strips that support SP_MINMAXABLE.
		@emem SP_EARLY_TIME | VT_I4.  Get the amount of time (in clocks) that should be shown before the start
					of the segment.  Useful for strips that allow the user to create 'pick-up' events.
		@emem SP_LATE_TIME | VT_I4.  Get the amound of time (in clocks) that should be shown after the end
					of the segment.  Useful for strips that allow the user to create events that play after
					the segment.
		@emem SP_USER | First available user-defined strip property.
*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdStrip | The Timeline uses this interface to interact with the editors that
		display DirectMusic tracks inside a Segment.

	@meth HRESULT | Draw | Draw the client area of the strip.

	@meth HRESULT | GetStripProperty | Get a property of the strip.

	@meth HRESULT | SetStripProperty | Set a property of the strip.

	@meth HRESULT | OnWMMessage | Pass window messages to a strip.

	@base public | IUnknown

	@xref	<i IDMUSProdTimeline>, <om IDMUSProdTimeline.AddStrip>, <om IDMUSProdTimeline.RemoveStrip>,
		<om IDMUSProdTimeline.StripSetTimelineProperty>, <om IDMUSProdTimeline.StripGetTimelineProperty>
	--------------------------------------------------------------------------*/

/*
	@method HRESULT | IDMUSProdStrip | Draw | This method draws the client area of the strip in the Timeline control.

	@comm	From the given <p hDC>, the X position 0 will always be the left edge of the strip, as displayed
				on the screen.  Horizontal scrolling is accomplished by the Timeline changing <p lXOffset>.
				The Y position 0 will by at the top edge of the strip, unless the strip's
				STP_VERTICAL_SCROLL is non-zero, in which case the Y position set by
				STP_VERTICAL_SCROLL will be at the top edge of the strip.  The clipping rectangle will
				always be set such that the strip is unable to draw outside of its screen boundaries.

	@parm   HDC | hDC | A handle to the device context in which to draw.
	@parm	<t STRIPVIEW> | stripView | The current strip view state, either SV_NORMAL or SV_MINIMIZED.
	@parm	LONG | lXOffset | The pixel amount that this strip has scrolled to the right.

	@xref	<i IDMUSProdStrip>, <t STRIPVIEW>, <om IDMUSProdStripFunctionBar::FBDraw>
*/

/*
	@method HRESULT | IDMUSProdStrip | GetStripProperty | This method gets a property of the strip.

	@parm   <t STRIPPROPERTY> | stripProperty | Which property to get.
	@parm   VARIANT* | pVar | Address of the variant to return the property in.

	@comm	<om IUnknown::AddRef> should be called on all valid <i IUnknown> interfaces
		returned by this method.

	@rdesc	If the method succeeds, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue E_POINTER | <p pVar> is NULL.
	@rvalue E_INVALIDARG | <p stripProperty> contains an unsupported property type.

	@xref	<i IDMUSProdStrip>, <t STRIPPROPERTY>, <om IDMUSProdStrip::SetStripProperty>
*/

/*
	@method HRESULT | IDMUSProdStrip | SetStripProperty | This method sets a property of the strip.

	@parm   <t STRIPPROPERTY> | stripProperty | Which property to set.
	@parm   VARIANT | var |  The data to set the property with.

	@comm	If <p var> is of type VT_UNKNOWN and the <i IDMUSProdStrip> wishes to keep a local copy of
		the data, <om IUnknown::AddRef>	should be called on the passed interface.

	@rdesc	If the method succeeds, the return value is S_OK. <nl>
		If it fails, the method may return one of the following error values:

	@rvalue E_POINTER | <p var> contains a NULL pointer.
	@rvalue E_INVALIDARG | <p stripProperty> contains an unsupported property type.

	@xref	<i IDMUSProdStrip>, <t STRIPPROPERTY>, <om IDMUSProdStrip::GetStripProperty>
*/

/*
	@method HRESULT | IDMUSProdStrip | OnWMMessage | This method passes window messages to a strip.

	@parm   UINT | nMsg | The window message identifier.
	@parm   WPARAM | wParam | The wParam value.
	@parm   LPARAM | lParam | The wParam value.
	@parm   LONG | lXPos | For window messages that contain position information, this contains the X
						  position in strip coordinates.
	@parm   LONG | lYPos | For window messages that contain position information, this contains the Y
						  position in strip coordinates.

	@comm	The following window messages store valid data in <p lXPos> and <p lYPos>:<nl>
		<m WM_MOUSEMOVE> <m WM_LBUTTONDBLCLK> <m WM_LBUTTONDOWN> <m WM_LBUTTONUP>
		<m WM_RBUTTONDOWN> <m WM_RBUTTONUP> <m WM_SETCURSOR> <m WM_CONTEXTMENU>

	@comm	A strip will receive the <m WM_CREATE> message after it is added to the timeline, and a
		<m WM_DESTROY> message after it is removed from the Timeline. 

	@comm	A strip will receive the <m WM_MOVE> message when its position is changed, via adding,
		removing, or resizing of a strip above it.

	@comm	A strip will receive the <m WM_SIZE> message when its size is changed, via resizing the
		function bar, the user clicking the minimize/maximize button, the user double-clicking
		either a minimized strip or the title bar of the maximized strip, or the user vertically
		resizing the strip by dragging the separator line below it.

	@comm	A strip will receive the <m WM_KILLFOCUS> message when it loses the active focus.  Graphically,
		this is represented by the strip's gutter color changing from yellow or orange to grey or red,
		respectively.

	@comm	A strip will receive the <m WM_SETFOCUS> message when it gains the active focus.  Graphically,
		this is represented by the strip's gutter color changing from grey or red to yellow or orange,
		respectively.

	@rvalue S_OK | The operation was successful.

	@xref	<i IDMUSProdStrip>, <om IDMUSProdStripFunctionBar::FBOnWMMessage>
*/


/*  --------------------------------------------------------------------------
	@interface IDMUSProdStripFunctionBar | The Timeline uses this interface to interact with the editors that
		display information in the functionbar area of a strip.

	@meth HRESULT | FBDraw | Draw the client area of the strip function bar.

	@meth HRESULT | FBOnWMMessage | Pass window messages to a strip function bar.

	@base public | IUnknown

	@xref	<i IDMUSProdTimeline>, <i IDMUSProdStrip>
	--------------------------------------------------------------------------*/

/*
	@method HRESULT | IDMUSProdStripFunctionBar | FBDraw | This method draws the client area of the strip's function
		bar in the Timeline control.

	@comm	From the given <p hDC>, the X position 0 will always be the left edge of the function bar, as
				displayed on the screen.  The Y position 0 will by at the top edge of the strip, unless the
				strip's	STP_VERTICAL_SCROLL is non-zero, in which case the Y position set by
				STP_VERTICAL_SCROLL will be at the top edge of the strip.  The clipping rectangle will
				always be set such that the strip's function bar is unable to draw outside of its screen
				boundaries, and is unable to draw on the bar displaying the strip's name.

	@parm   HDC | hDC | A handle to the device context in which to draw.
	@parm	<t STRIPVIEW> | stripView | The current strip view state, either SV_FUNCTIONBAR_NORMAL or
									SV_FUNCTIONBAR_MINIMIZED.

	@xref	<i IDMUSProdStripFunctionBar>, <t STRIPVIEW>, <om IDMUSProdStrip::Draw>
*/

/*
	@method HRESULT | IDMUSProdStripFunctionBar | FBOnWMMessage | This method passes on window messages to a strip's
		function bar area.

	@parm   UINT | nMsg | The window message identifier.
	@parm   WPARAM | wParam | The wParam value.
	@parm   LPARAM | lParam | The wParam value.
	@parm   LONG | lXPos | For window messages that contain position information, this contains the X
						  position in strip coordinates.
	@parm   LONG | lYPos | For window messages that contain position information, this contains the Y
						  position in strip coordinates.

	@comm	The following window messages store valid data in <p lXPos> and <p lYPos>:<nl>
		<m WM_MOUSEMOVE> <m WM_LBUTTONDBLCLK> <m WM_LBUTTONDOWN> <m WM_LBUTTONUP>
		<m WM_RBUTTONDOWN> <m WM_RBUTTONUP> <m WM_SETCURSOR> <m WM_CONTEXTMENU>

	@rvalue S_OK | The operation was successful.

	@xref	<i IDMUSProdStripFunctionBar>, <om IDMUSProdStrip::OnWMMessage>
*/