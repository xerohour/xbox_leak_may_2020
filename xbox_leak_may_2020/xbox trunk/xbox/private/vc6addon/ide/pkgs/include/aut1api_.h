/////////////////////////////////////////////////////////////////////////////
//	AUT1API_.H
//		Automation1 private package interfaces

#ifndef __AUT1API__H__
#define __AUT1API__H__

/////////////////////////////////////////////////////////////////////////////
//	IAut1Misc interface

// Miscellaneous methods we expose to the shell, which don't have a better home can go here

#undef  INTERFACE
#define INTERFACE IAut1Misc

DECLARE_INTERFACE_(IAut1Misc, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IAut1Misc methods

	// Called by tools customize tabs to reload the macros.  Returns
	//  S_OK on success, S_FALSE if an error occurred in loading
	//  one of the macro files (e.g., syntax error).
	STDMETHOD(ReloadMacros)() PURE;
};

	
/////////////////////////////////////////////////////////////////////////////
//	IAddIn interface

#undef  INTERFACE
#define INTERFACE IAddIn

DECLARE_INTERFACE_(IAddIn, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IAddIn methods

	// These are delegated to directly by the OLE Automation Application
	//  object.  These are typically called by an AddIn through the
	//  DevStudio object model's Application object.

	// Returns S_OK for TRUE, S_FALSE for FALSE.  It will throw
	//  an exception via DsThrowOleDispatchException if something
	//  bad happens.
	STDMETHOD(AddCommand)(LPCTSTR szCmdName, LPCTSTR szMethodName,
							long nBitmapOffset, long dwCookie) PURE;

	// Just returns S_OK.  If this returns an error, that error should
	//  be returned from the caller
	STDMETHOD(SetAddInInfo)(long nInstanceHandle, LPDISPATCH pCmdDispatch,
		long nIDBitmapResourceMedium, long nIDBitmapResourceLarge,
		long dwCookie) PURE;

	// Just returns S_OK.  It will throw an exception via 
	//  DsThrowOleDispatchException if something bad happens.
	STDMETHOD(AddCommandBarButton)(long nButtonType, LPCTSTR szCmdName,
		long dwCookie) PURE;
};


// NOTE: This is a private interface used ONLY by the shell to hook in
//  to recording and provide default recorded text if none is provided by
//  the package.
//
// The shell will use this interface in the following way:
//
//	1) A command comes in from the user
//	2) The shell checks to see if recording is on.  If it is,
//	3) Shell calls IRecorderForShell::BeginListenForRecord()
//	4) Shell dispatches command
//	5) Shell calls IRecorderForShell::EndListenForRecord()
//	6) If *pbWasTextRecorded is FALSE, the shell will record the action
//		as a call to Application::ExecuteCommand("<command-name>").
//	7) If *pbWasTextRecorded is TRUE, the shell returns to the message pump


// Note that this interface is not to be used by any client other
//  than the shell.  If two clients try to use this interface,
//  the Begin/End ListenForRecord bracketing will get all fouled
//  up.

#undef  INTERFACE
#define INTERFACE IRecorderForShell

DECLARE_INTERFACE_(IRecorderForShell, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IRecorderForShell methods

	// The shell calls this method to let the macro recorder know that in
	//  the future, the shell will call EndListenForRecord.  When it makes
	//  this call, the recorder will return whether
	//  IMacroRecorder::RecordText was called by any package since
	//  this call to BeginListenForRecord().
	STDMETHOD(BeginListenForRecord)() PURE;

	// Returns (in *pbWasTextRecorded) whether a package called
	//  IMacroRecorder::RecordText (or IMacroRecorder::ReplaceLastText)
	//  since the last call to BeginListenForRecord.  This method has the
	//  side effect of "ending" listening for record, which means that if this
	//  is called two times in a row, the second time will *always*
	//  return FALSE in pbTextRecorded.  In other words, EACH
	//  call to this method must be preceded by its own matching
	//  BeginListenForRecord
	STDMETHOD(EndListenForRecord)(BOOL* pbWasTextRecorded) PURE;
};



#endif //__AUT1API__H__

