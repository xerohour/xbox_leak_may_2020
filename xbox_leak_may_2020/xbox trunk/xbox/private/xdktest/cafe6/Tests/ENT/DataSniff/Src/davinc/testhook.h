//-----------------------------------------------------------------------------
// Microsoft daVinci
//
// Microsoft Confidential
// Copyright 1994 - 1995 Microsoft Corporation. All Rights Reserved.
//
// File: testhook.h
// Area: test hooks
//
// Contents: This file contains the test hook messages and structures
//
// Owner: stevekr
//-----------------------------------------------------------------------------

#ifndef _TESTHOOK_H
#define _TESTHOOK_H

// Typedefs and constants -----------------------------------------------------

#define	MAX_NAME_LEN			256

typedef struct {
	OLE_XPOS_HIMETRIC		himetLeft;
	OLE_YPOS_HIMETRIC		himetTop;
	OLE_XSIZE_HIMETRIC		himetWidth;
	OLE_YSIZE_HIMETRIC		himetHeight;
	} ObjectLocation, *PObjectLocation;

enum LayoutStyle {
	eRectilinear	= 0,	// Schema
	eCircular		= 1,	// Link View radial layout
	eOrgTopToBottom	= 2,	// SDN current
	eOrgLeftToRight = 3,	// Link View horizontal layout
	eOrgRightToLeft = 4,	// the rest are NYI...
	eCyberbolic		= 5
	};

typedef struct {
	LONG			lDiagramZoom;	// zoom level of diagram (1-400)
	LayoutStyle		eLayout;		// current layout style
	BOOL			fDirty;			// diagram dirty flag
	ObjectLocation	olocViewport;	// current visible canvas
	} DiagramInfo, *PDiagramInfo;
//$ CONSIDER:  In the DiagramInfo structure, also list the window title?

typedef struct {
	INT					arrowOneEnd;
	INT					arrowOtherEnd;
	INT					DdsLinestyle;
	INT					PolylineRendition;
	LONG				cLabels;
	LONG				cPoints;
	LONG				cOneCombPoints;
	LONG				cOtherCombPoints;
	CHAR				szLabelName[MAX_NAME_LEN];
	POINT*				rgptConnector;
	} ConnectorInformation, *PConnectorInformation;

// Query Diagram Pane
typedef struct
	{
	WCHAR				strName[MAX_NAME_LEN];
	HWND				hwnd;
	RECT				rcPosition;
	BOOL				fMinimized;
	LONG				cColumns;
	} DiagramTableInfo, *PDiagramTableInfo;

typedef struct
	{
	CHAR				strName[MAX_NAME_LEN];
	HWND				hwnd;
	RECT				rcPosition;
	BOOL				fMinimized;
	LONG				cColumns;
	} ANSIDiagramTableInfo, *PANSIDiagramTableInfo;

// Bit flags for visibility of tables
#define fTableVisibleNone		0x00
#define fTableVisibleYes		0x01
#define fTableVisiblePartial	0x02
#define fTableVisibleObscured	0x04

typedef struct
	{
	WCHAR				strName[MAX_NAME_LEN];
	INT					iecolPosition;
	RECT				rcPosition;
	RECT				rcClick;
	RECT				rcName;
	RECT				rcSortIcon;
	RECT				rcFilterIcon;
	RECT				rcGroupIcon;
	BOOL				fPrimaryKeyCol;
	LONG				lStateClick;
	LONG				lStateSort;
	BOOL				fGrouped;
	LONG				lStateAgg;
	BOOL				fFiltered;
	} DiagramColumnInfo, *PDiagramColumnInfo;

typedef struct
	{
	CHAR				strName[MAX_NAME_LEN];
	INT					iecolPosition;
	RECT				rcPosition;
	RECT				rcClick;
	RECT				rcName;
	RECT				rcSortIcon;
	RECT				rcFilterIcon;
	RECT				rcGroupIcon;
	BOOL				fPrimaryKeyCol;
	LONG				lStateClick;
	LONG				lStateSort;
	BOOL				fGrouped;
	LONG				lStateAgg;
	BOOL				fFiltered;
	} ANSIDiagramColumnInfo, *PANSIDiagramColumnInfo;

typedef struct
	{
	UINT				uIdTableFrom;
	UINT				uIdTableTo;
	UINT				uIdColumnFrom;	// a value of 0 signifies no column
	UINT				uIdColumnTo;	// a value of 0 signifies no column
	RECT				rcExtent;		// value not always available
	LONG				lJoinType;
	LONG				lJoinOp;
	} DiagramJoinLineInfo, *PDiagramJoinLineInfo;

enum SelectionStates {
	eNotSelected = 0,
	ePrimarySelection = 1,
	eSelected = 2,
	};

// WebScope structures
enum LV_IsolationState {
	eNotAvailable = 0,
	eOrphaned = 1,
	eGhosted = 2,
	eGhosted_SC = 3,
	eReadOnly = 4,
	eReadOnly_SC = 5,
	eWorking = 6,
	eCheckedOut_SC = 7,
	eBroken = 8
	};

typedef struct
	{
	WCHAR wszURL[MAX_NAME_LEN];
	ULONG ulLoadState;
	BOOL fExternal;
	WCHAR wszType[MAX_NAME_LEN];
	WCHAR wszTooltip[MAX_NAME_LEN];
	WCHAR wszLabel[MAX_NAME_LEN];
	ObjectLocation locLabel;
	ULONG ulIsolationState;
	} WebObjectInfo, *PWebObjectInfo;

typedef struct
	{
	CHAR strURL[MAX_NAME_LEN];
	LONG ulLoadState;
	LONG fExternal;
	CHAR strType[MAX_NAME_LEN];
	CHAR strTooltip[MAX_NAME_LEN];
	CHAR strLabel[MAX_NAME_LEN];
	ObjectLocation locLabel;
	ULONG ulIsolationState;
	} VBWebObjectInfo, *PVBWebObjectInfo;

// Note: These values are chosen so that a bitwise or of eOutBound with
//			eInBound yields eOutAndInBound.  If you change the values,
//			you may have to modify code that uses them in this way.
enum LV_LinkDirection {
	eOutBound = 1,
	eInBound = 2,
	eOutAndInBound = 3
	};

//	grid message structure
typedef struct {
	HWND	hwndGrid;
	UINT	uMsgGrid;
	BOOL	fWParamByVal;
	BOOL	fLParamByVal;
	union {
		WPARAM	wparamGrid;
		ULONG	cbWParamBuffer;
		};
	union {
		LPARAM	lparamGrid;
		struct {
			ULONG	cbLParamBuffer;
			ULONG	iLParamOffset;
			};
		};
	BYTE	*rgbBuffer;				//	memory used for the buffer
	} GridMessagePacket, *PGridMessagePacket;

enum IsFullyLoadedReturnValues 
	{
	eNotInDiagram = -1,
	eNotFullyLoaded = 0,
	eFullyLoaded = 1,
	};

enum SchemaCommitHookFlags
	{
	eBeginTransaction = 1,
	eCommit = 2,
	eRollback = 4,
	};

typedef struct {
	LONG eViewType;
	} FieldListInfo, *PFieldListInfo;


// Site Designer structures ---------------------------------------------------

// The maximum number of nodes allowed on a given diagram
//	NOTE:  This is not a limit in Site Designer.
//	The problem is that it's difficult to dynamically allocate memory within
//	structures in VB, so we needed to pick a big (constant) number.
#define SDN_MAXNODES	1024

// NodeIDList - used whenever the caller wants a list of nodes to be returned
//	(if fDiagram is TRUE, the caller is requesting visible node controls on
//	the diagram, if fDiagram is FALSE, the caller is requesting nodes from the
//	site structure model in memory).
typedef struct {
	BOOL	fDiagram;	// TRUE if diagram-centered, FALSE if model-centered
	INT		cNodes;		// size of array, in UINTs
	UINT	rgnID[SDN_MAXNODES];	// array of node IDs
	} NodeIDList, *PNodeIDList;

typedef struct {
	ObjectLocation	olocPosition;	// size and position of a node control
									//	(coordinates are canvas himetric)
	ObjectLocation	olocExpand;		// size and position of expand/collapse btn
									//	(himetric, relative to node control)
	ObjectLocation	olocCaption;	// size and position of draggable caption
									//	(himetric relative to node control)
	ObjectLocation	olocEdit;		// size and position of the text box
									//	(himetric relative to node control)
	BOOL			fExpanded;		// TRUE if node is expanded
	BOOL			fExpandVisible;	// TRUE if +/- symbol is visible
	BOOL			fDirty;			// node's dirty flag
	BOOL			fHasEditCtrl;	// TRUE if an edit control is up
	CHAR			strEditText[MAX_NAME_LEN];	// contents of edit control
	UINT			nParentID;		// ID of parent (0 if none on diagram)
	SelectionStates	eSelect;		// selection state
	LayoutStyle		eLayout;		// layout direction of this node control	
	} NodeDiagramInfo, *PNodeDiagramInfo;

typedef struct {
	CHAR	strName[MAX_NAME_LEN];	// name of the node
	CHAR	strFile[MAX_NAME_LEN];	// associated file on the server
	BOOL	fDirty;					// node's dirty flag
	BOOL	fExternal;				// TRUE if not in current project
	UINT	nParentID;				// ID of parent (0 if a root/orphan node)
	} NodeModelInfo, *PNodeModelInfo;

enum SDN_ErrorCodes {
	eInvalidID		= -1,	// given ID doesn't correspond to a valid node
	eDiagramOnly	= -2,	// this message requires fDiagram = TRUE
	eCaughtException = -3,	// something threw!
	eError			= -100	// some other, unspecified error
	};


// Messages -------------------------------------------------------------------
#define WM_DaVinciBase			(WM_USER+1000)
#define WM_DaVinciQueryBase		(WM_DaVinciBase + 200)
#define WM_DaVinciWebScopeBase	(WM_DaVinciBase + 400)
#define WM_DaVinciSDNBase		(WM_DaVinciBase + 600)
//$ CONSIDER: split apart Schema messages from DaVinciBase

//	NOTE:
//		for the below comments, the wParam and lParam refer to those passed in
//		the VB code, not to the wParam and lParam sent to the component


//	----------------------------------------------------------------------------
//	message:		WM_UseTestHooks
//
//	description:	turns on the test hook mechanism for the given hwnd
//
//	wParam:			ignored
//
//	lParam:			size in bytes of shared memory to create
//
//	return value:	return values from CreateFileMapping and MapViewOfFile
//
#define WM_UseTestHooks			(WM_DaVinciBase + 0)

//	----------------------------------------------------------------------------
//	message:		WM_DontUseTestHooks
//
//	description:	turns off the test hook mechanism for the given hwnd
//
//	wParam:			ignored
//
//	lParam:			ignored
//
//	return value:	return values from CloseMapping and UnMapViewOfFile
//
#define WM_DontUseTestHooks			(WM_DaVinciBase + 1)

//	----------------------------------------------------------------------------
//	message:		WM_IsTableLoaded
//
//	description:	determines if a given table is loaded into the component
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the table name
//
//	return value:	< 0	error, 0 table is not loaded, 1 table is loaded
//
#define	WM_IsTableLoaded		(WM_DaVinciBase + 2)

//	----------------------------------------------------------------------------
//	message:		WM_TableLocation
//
//	description:	returns the location and size of the table
//
//	wParam:			a PLocation which gets filled out
//
//	lParam:			ANSI, null terminated string containing the table name
//
//	return value:	< 0 error, S_OK ok
//
#define WM_TableLocation		(WM_DaVinciBase + 3)

//	----------------------------------------------------------------------------
//	message:		WM_ListTables
//
//	description:	returns a list of tables
//
//	wParam:			size in characters of buffer
//
//	lParam:			ANSI, null terminated string which will be filled in with
//					a comma delimited list of table names in quotes
//					"Table1", "this is table 2"
//
//	return value:	
//
#define WM_ListTables			(WM_DaVinciBase + 4)

//	----------------------------------------------------------------------------
//	message:		WM_ListRelationships
//
//	description:	returns a list of relationships
//
//	wParam:			size in characters of buffer
//
//	lParam:			ANSI, null terminated string which will be filled in with
//					a comma delimited list of relationship names in quotes
//
//	return value:	
//
#define WM_ListRelationships	(WM_DaVinciBase + 5)

//	----------------------------------------------------------------------------
//	message:		WM_GetViewport
//
//	description:	returns the location of the top left corner relative to
//					the DDS virtual canvas, and the width and height
//
//	wParam:			a PLocation which gets filled out
//
//	lParam:			ignored
//
//	return value:	
//
#define WM_GetViewport			(WM_DaVinciBase + 6)

//	----------------------------------------------------------------------------
//	message:		WM_ConnectorInfo
//
//	description:	returns the information for the specified connector
//
//	wParam:			a PConnectorInformation which gets filled out
//
//	lParam:			ANSI, null terminated string containing the connector name
//
//	return value:	
//
#define WM_ConnectorInfo		(WM_DaVinciBase + 7)  

//	----------------------------------------------------------------------------
//	message:		WM_IsTableSelected
//
//	description:	is the specified table selected?
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the table name
//
//	return value:	
//
#define WM_IsTableSelected		(WM_DaVinciBase + 8)  

//	----------------------------------------------------------------------------
//	message:		WM_IsRelationshipSelected
//
//	description:	is the specified relationship selected?
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the relationship name
//
//	return value:	
//
#define WM_IsRelationshipSelected		(WM_DaVinciBase + 9)  

//	----------------------------------------------------------------------------
//	message:		WM_IsLoadComplete
//
//	description:	is the background loading complete?
//
//	wParam:			ignored
//
//	lParam:			ignored
//
//	return value:	TRUE if complete, FALSE if not
//
#define WM_IsLoadComplete				(WM_DaVinciBase + 10)  

//	----------------------------------------------------------------------------
//	message:		WM_GridMessage
//
//	description:	wrapper around sendmessage to the grid.  The shared memory
//					must contain a GridMessagePacket.
//
//	wParam:			ignored
//
//	lParam:			ignored
//
//	return value:	return value from the sendmessage to the grid
//
#define WM_GridMessage				(WM_DaVinciBase + 11)  

//	----------------------------------------------------------------------------
//	message:		WM_IsFieldListFullyLoaded
//
//	description:	has the specified field list ocx fully loaded its grid & proxy
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the table name
//
//	return value:	from IsFullyLoadedReturnValues enum:
//		< 0	error, 0 field list is not on diagram, 1 field list is fully loaded.
//
#define WM_IsFieldListFullyLoaded	(WM_DaVinciBase + 12)  

//	----------------------------------------------------------------------------
//	message:		WM_IsRelationLineFullyLoaded
//
//	description:	has the specified relation line ocx fully loaded its proxy
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the relation name
//
//	return value:	from IsFullyLoadedReturnValues enum:
//		< 0	error, 0 relation line is not on diagram, 1 relation line is fully loaded.
//
#define WM_IsRelationLineFullyLoaded	(WM_DaVinciBase + 13)  

//	----------------------------------------------------------------------------
//	message:		WM_HwndGridOfFieldList
//
//	description:	return the grid control hwnd of the specified field list ocx,
//					FORCING it to fully load if the grid and proxy are not yet loaded.
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the table name
//
//	return value:	NULL if error, valid hwnd if Success
//
#define WM_HwndGridOfFieldList		(WM_DaVinciBase + 14)  

//	----------------------------------------------------------------------------
//	message:		WM_SetDelayLoadMode
//
//	description:	turn of delay loading of proxies either on or off.
//
//	wParam:			boolean specifying On (1) or Off (0).
//
//	lParam:			ignored
//
//	return value:	S_OK
//
#define WM_SetDelayLoadMode			(WM_DaVinciBase + 15)  

//	----------------------------------------------------------------------------
//	message:		WM_SetCommitHooks
//
//	description:	turn commit hooks on/off
//
//	wParam:			any combination of SchemaCommitHookFlags
//
//	lParam:			ignored
//
//	return value:	S_OK if ok
//
#define WM_SetCommitHooks			(WM_DaVinciBase + 16)  

//	----------------------------------------------------------------------------
//	message:		WM_GetFieldListInfo
//
//	description:	get info about a particular field list
//
//	wParam:			ignored
//
//	lParam:			ANSI, null terminated string containing the table name
//
//	return value:	S_OK if ok
//
#define WM_GetFieldListInfo			(WM_DaVinciBase + 17)

//	----------------------------------------------------------------------------
//	message:		WM_FirePerformanceEvents
//
//	description:	turn on firing performance events
//	IMPORTANT NOTE:	the performance events are sent using PostMessage rather
//					than SendMessage so that the act of sending them doesn't
//					adversely screw up the timings.  This means that consumers
//					of these messages MUST use GetMessageTime to get the time
//					the message was posted.
//
//	wParam:			ignored
//
//	lParam:			HWND to send the events to
//
//	return value:	S_OK if ok
//
#define WM_FirePerformanceEvents	(WM_DaVinciBase + 18)

//	----------------------------------------------------------------------------
//	message:		WM_AllTablesLoaded
//
//	description:	message to notify test that all tables have been loaded
//
//	wParam:			0
//
//	lParam:			0
//
//	return value:	should always be S_OK
//
#define WM_AllTablesLoaded			(WM_DaVinciBase + 19)

//	----------------------------------------------------------------------------
//	message:		WM_TableLoaded
//
//	description:	message to notify test that one table has been loaded
//
//	wParam:			0
//
//	lParam:			0
//
//	return value:	should always be S_OK
//
#define WM_TableLoaded				(WM_DaVinciBase + 20)

//	---------------------------------------------------------------------------
//	message:		WM_DiagramInfo
//
//	description:	message to determine information about diagram (zoom,
//					viewport size, etc.)
//
//		NOTE:  Only supported by Site Designer at the moment
//
//	wParam:			0
//
//	lParam:			pointer to a DiagramInfo structure
//
//	return value:	negative for error, 0 for success (S_OK)
//
#define WM_DiagramInfo				(WM_DaVinciBase + 21)



// Query test hooks -----------------------------------------------------------

// Diagram pane hooks

//-----------------------------------------------------------------------------
// Message:		WM_DiagramEnumTables
//
// Description:	Used to obtain a list of table ids currently on the diagram
//				surface
//
// wParam:		[IN] Maximum number of elements the passed in array can hold. 
//				If there are more tables on the diagram than wParam, only
//				the first wParam of them are returned, and the return value
//				is wParam. If lParam is NULL, this value is ignored.
//
// lParam:		[OUT] A pointer to an array of UINTs that is used to receive
//				table ids. If this pointer is NULL, a count of the number of
//				tables is returned
//
// Return Value:The number of tables whose ids have been passed back (0..n)
//				if lParam was non-null. If lParam was NULL, it will return
//				the total number of tables currently in the diagram.
//				-ve values to indicate error.
//-----------------------------------------------------------------------------
#define WM_DiagramEnumTables		(WM_DaVinciQueryBase + 0)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramTableInfo
//
// Description:	Used to obtain info about a particular table (ECL) on the 
//				diagram	surface.
//
// wParam:		[IN] Table ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumTables message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		[OUT] A pointer to a DiagramTableInfo struct (PDiagramTableInfo)
//				If this is NULL, E_INVALIDARG is returned
//
// Return Value:0 on success, -ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramTableInfo			(WM_DaVinciQueryBase + 1)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramIsTableVisible
//
// Description:	Determines whether the particular table (ECL) on the 
//				diagram	surface is currently visible or hidden
//
// wParam:		[IN] Table ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumTables message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		unused
//
// Return Value:A group of bit flags. The following describes the meaning of
//				the various bits when set:
//
//				fTableVisibleYes : table is currently in the visible region of
//				the diagram pane
//				fTableVisiblePartial : table is only partially visible if this
//				bit is set. This bit can only be set if fTableVisibleYes is set
//				(If this bit is not set & fTableVisibleYes is set, the table
//				is fully visible)
//				fTableVisibleObscured : table is partially or fully obsured
//				by another table which is above it in the z-order.
//				-ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramIsTableVisible	(WM_DaVinciQueryBase + 2)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramIsTableSelected
//
// Description:	Tells whether the particular table (ECL) is currently selected
//				or not. Note that the table's column may be selected while the
//				table itself may not be selected.
//
// wParam:		[IN] Table ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumTables message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		Not used
//
// Return Value:1 if selected, 0 if not selected, -ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramIsTableSelected	(WM_DaVinciQueryBase + 3)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramEnumTableColumns
//
// Description:	Used to obtain a list of column ids for a given table which is
//				currently on the diagram surface
//
// wParam:		[IN] Table ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumTables message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		[OUT] A pointer to an array of UINTs that is used to receive
//				column ids. If this pointer is NULL, a count of the number of
//				columns is returned
//
// Return Value:The number of columns whose ids have been passed back (0..n)
//				if lParam was non-null. If lParam was NULL, it will return
//				the total number of columns in that table
//				-ve values to indicate error.
//
// Note:		Column ids are listed in the order they appear in the table.
//				Also column ids are unique over the entire set of tables in the
//				diagram surface
//-----------------------------------------------------------------------------
#define WM_DiagramEnumTableColumns	(WM_DaVinciQueryBase + 4)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramTableColumnInfo
//
// Description:	Used to obtain info about a particular column in a table on the 
//				diagram	surface.
//
// wParam:		[IN] Column ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumTableColumns message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		[OUT] A pointer to a DiagramColumnInfo struct 
//				(PDiagramColumnInfo) If this is NULL, E_INVALIDARG is returned
//
// Return Value:0 on success, -ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramTableColumnInfo	(WM_DaVinciQueryBase + 5)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramIsTableColumnSelected
//
// Description:	Tells whether the particular column is currently selected
//				or not. Note that the table's column may be selected while the
//				table itself may not be selected.
//
// wParam:		[IN] Column ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumTableColumns message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		Not used
//
// Return Value:1 if selected, 0 if not selected, -ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramIsTableColumnSelected	(WM_DaVinciQueryBase + 6)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramEnumJoinLines
//
// Description:	Used to obtain a list of join line ids currently on the diagram
//				surface
//
// wParam:		[IN] Maximum number of elements the passed in array can hold. 
//				If there are more join lines on the diagram than wParam, only
//				the first wParam of them are returned, and the return value
//				is wParam. If lParam is NULL, this value is ignored.
//
// lParam:		[OUT] A pointer to an array of UINTs that is used to receive
//				join line ids. If this pointer is NULL, a count of the number 
//				of join lines is returned
//
// Return Value:The number of join line ids that have been passed back (0..n)
//				if lParam was non-null. If lParam was NULL, it will return
//				the total number of join lines currently in the diagram.
//				-ve values to indicate error.
//-----------------------------------------------------------------------------
#define WM_DiagramEnumJoinLines		(WM_DaVinciQueryBase + 7)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramJoinLineInfo
//
// Description:	Used to obtain info about a particular join line on the 
//				diagram	surface.
//
// wParam:		[IN] Join Line ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumJoinLines message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		[OUT] A pointer to a DiagramJoinLineInfo struct 
//				(PDiagramJoinLineInfo) If NULL, E_INVALIDARG is returned
//
// Return Value:0 on success, -ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramJoinLineInfo		(WM_DaVinciQueryBase + 8)  

//-----------------------------------------------------------------------------
// Message:		WM_DiagramIsJoinLineSelected
//
// Description:	Tells whether the particular join line is currently selected
//				or not. 
//
// wParam:		[IN] Join Line ID as a UINT. This must be one of the values
//				returned by the WM_DiagramEnumJoinLines message, otherwise
//				an E_INVALIDARG is returned.
//
// lParam:		Not used
//
// Return Value:1 if selected, 0 if not selected, -ve values on failure
//-----------------------------------------------------------------------------
#define WM_DiagramIsJoinLineSelected	(WM_DaVinciQueryBase + 9)  

//-----------------------------------------------------------------------------
// Message:		WM_GetActivePane
//
// Description:	Returns the hwnd of the currently active pane in the query 
//				designer
//
// wParam:		Not used
//
// lParam:		Not used
//
// Return Value: a valid hwnd, NULL on failure
//-----------------------------------------------------------------------------
#define WM_GetActivePane	(WM_DaVinciQueryBase + 10)  

//-----------------------------------------------------------------------------
// Message:			WM_DataPaneQueryComplete
//
// Description:		Used to notify test when the query result pane 
//					is about to be populated as a result of an executed query
//					(strictly speaking, at the moment when the query is complete).
//
// wParam:			Not used
//
// lParam:			Not used
//
// Return Value:	should always be S_OK
//-----------------------------------------------------------------------------
#define WM_DataPaneQueryComplete	(WM_DaVinciQueryBase + 11)  

//-----------------------------------------------------------------------------
// Message:			WM_LV_GetWebObjectInfo
//
// Description:		Returns information about the webobject specified
//
// wParam:			A pointer to a WebObjectInfo structure to be filled in
//
// lParam:			ANSI, null terminated string containing the Web Object ID.
//
// Return Value:	< 0 error, S_OK ok
//-----------------------------------------------------------------------------
#define WM_LV_GetWebObjectInfo		(WM_DaVinciWebScopeBase + 0)


//----------------------------------------------------------------------------
// message:			WM_LV_WebScopeClusterExpanded
//
// description:		message to notify test that the cluster has been expanded
//
// wParam:			0
//
// lParam:			0
//
// return value:	should always be S_OK
//
#define WM_LV_WebScopeClusterExpanded 	(WM_DaVinciWebScopeBase + 1)

//----------------------------------------------------------------------------
// message:			WM_LV_GetZoomFactor
//
// description:		Return the zoom factor
//
// wParam:			0
//
// lParam:			0
//
// return value:	the zoom factor
//
#define WM_LV_GetZoomFactor	(WM_DaVinciWebScopeBase + 2)

//----------------------------------------------------------------------------
// message:			WM_LV_GetLayoutStyle
//
// description:		Return the layout style
//
// wParam:			0
//
// lParam:			0
//
// return value:	current layout style (enum LayoutStyle)
//
#define WM_LV_GetLayoutStyle	(WM_DaVinciWebScopeBase + 3)

//-----------------------------------------------------------------------------
// Message:			WM_LV_IsWebObjectValid
//
// Description:		Determines if the passed in Web Object ID refers to a
//					currently existing diagram web object.
//
// wParam:			0
//
// lParam:			ANSI, null terminated string containing the Web Object ID.
//
// Return Value:	1 object exists.  0 object does not exist.  <0 error.
//-----------------------------------------------------------------------------
#define WM_LV_IsWebObjectValid		(WM_DaVinciWebScopeBase + 4)

//----------------------------------------------------------------------------
// message:			WM_LV_GetLinkDirection
//
// description:		Return the link direction
//
// wParam:			0
//
// lParam:			0
//
// return value:	current link direction (enum LV_LinkDirection)
//
#define WM_LV_GetLinkDirection	(WM_DaVinciWebScopeBase + 5)

//-----------------------------------------------------------------------------
// Message:			WM_IsConnectorValid
//
// Description:		Determines if the passed in Web Link ID refers to a
//					currently existing diagram connector.
//
// wParam:			0
//
// lParam:			ANSI, null terminated string containing the Web Link ID.
//
// Return Value:	1 object exists.  0 object does not exist.  <0 error.
//-----------------------------------------------------------------------------
#define WM_LV_IsConnectorValid		(WM_DaVinciWebScopeBase + 6)


//-----------------------------------------------------------------------------
//
//	Site Designer test hooks
//
//-----------------------------------------------------------------------------
//	---------------------------------------------------------------------------
//	message:		WM_SDN_EnumerateNodes
//
//	description:	message to enumerate all of the visible nodes on a diagram.
//
//	wParam:			not used
//
//	lParam:			pointer to a NodeIDList structure
//
//	return value:	negative for error, otherwise the number of nodes on the
//					diagram (see SDN_ErrorCodes above)
//
#define WM_SDN_EnumerateNodes			(WM_DaVinciSDNBase + 0)

//	---------------------------------------------------------------------------
//	message:		WM_SDN_NodeDiagramInfo
//
//	description:	message returns diagram-specific information about a given
//					node control
//
//	wParam:			ID of the node we're interested in
//
//	lParam:			pointer to a NodeDiagramInfo structure
//
//	return value:	negative for error, 0 for success (S_OK)
//					 (see SDN_ErrorCodes above)
//
#define WM_SDN_NodeDiagramInfo			(WM_DaVinciSDNBase + 1)

//	---------------------------------------------------------------------------
//	message:		WM_SDN_NodeModelInfo
//
//	description:	message returns model-specific information about a given
//					node.
//
//	wParam:			ID of the node
//
//	lParam:			pointer to a NodeModelInfo structure
//
//	return value:	negative for error, 0 for success (S_OK)
//
#define WM_SDN_NodeModelInfo			(WM_DaVinciSDNBase + 2)

//	---------------------------------------------------------------------------
//	message:		WM_SDN_GetNodeSiblings
//
//	description:	message to return a list of a given node's siblings.
//
//	wParam:			ID of the node we want siblings for
//
//	lParam:			pointer to a NodeIDList structure
//
//	return value:	negative for error, otherwise the number of siblings
//					 (see SDN_ErrorCodes above)
//
#define WM_SDN_GetNodeSiblings			(WM_DaVinciSDNBase + 3)

//	---------------------------------------------------------------------------
//	message:		WM_SDN_GetNodeChildren
//
//	description:	message to determine information about diagram (zoom,
//					viewport size, etc.)
//
//	wParam:			ID of the node we want children of
//
//	lParam:			pointer to a NodeIDList structure
//
//	return value:	negative for error, otherwise the number of children
//					 (see SDN_ErrorCodes above)
//
#define WM_SDN_GetNodeChildren			(WM_DaVinciSDNBase + 4)


//	---------------------------------------------------------------------------
//	message:		WM_SDN_SaveModel
//
//	description:	message to request that the site structure model be saved
//					to a specified output file.
//
//	wParam:			not used
//
//	lParam:			pointer to the null-terminated ANSI filename (full path)
//
//	return value:	negative for error, 0 for success
//
#define WM_SDN_SaveModel				(WM_DaVinciSDNBase + 5)


#endif	// _TESTHOOK_H
