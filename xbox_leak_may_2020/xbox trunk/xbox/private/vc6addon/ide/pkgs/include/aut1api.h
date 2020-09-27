/////////////////////////////////////////////////////////////////////////////
//	AUT1API.H
//		Automation1 package interfaces

#ifndef __AUT1API_H__
#define __AUT1API_H__


/////////////////////////////////////////////////////////////////////////////
//		Macro recording API declarations.  Implemented in devaut1.pkg
//		Further documentation available in recorder.doc (v5spec\vba tree)


/////////////////////////////////////////////////////////////////////////////
//	IMacroRecorder interface

// To check whether to record VBS code, test theApp.ShouldRecordText() before
//  calling this interface's methods.  It will return true if we're recording,
//  pause is off, and it's actually safe to record text (e.g., your code isn't
//  being called from another macro whose invocation was already recorded).
// This function is in the shell so that devaut1.pkg need not
//  be loaded in memory (caused by a call, say, to IMacroRecorder::IsRecording) every
//  time a command is executed.  Also, the shell needs to know whether recording
//  is occurring anyway, since it provides the default "ExecuteCommand" recorded
//  text described below.

#define REC_MAX_TEXT_OWNER_NAME 16

#undef  INTERFACE
#define INTERFACE IMacroRecorder

DECLARE_INTERFACE_(IMacroRecorder, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IMacroRecorder methods

	// This is the method you use to send the VBS text to duplicate the action
	//  just committed by the user in the UI.  For szPkg, provide a string that
	//  identifies yourself so other packages can query the recorder about who
	//  recorded the last text.  For szPkg, provide your package name as in your .pkg
	//  filename, e.g., "devMyPg(d).pkg" would pass "MyPg" as szPkg.
	//
	// If you want to record multiple lines, either call this multiple times, or
	//  call it once, separating the multiple lines with \n.  (Do not begin or end
	//  szText with \n unless you really want the extra newline in there; successive
	//  calls to RecordText are automatically separated by \n.)
	// 
	// If you do not call this method in response to a command, then the shell
	//  will provide a default recorded text:
	//		"ExecuteCommand("<command-name>")"
	//  This will cause "shl" to be the last record text owner (see
	//  GetLastTextOwner below)
	// If you intentionally do not want this command to be recorded at all,
	//  then call RecordText with NULL as szText.  Providing NULL as
	//  szText tells the recorder not to generate any text in the VBS macro
	//  for this action *AND* to tell the shell not to generate any default
	//  text either.
	// The length of szPkg must not exceed REC_MAX_TEXT_OWNER_NAME bytes
	//  (the ending NULL character is included in this count).
	STDMETHOD(RecordText)(LPCTSTR szText, LPCTSTR szPkg) PURE;

	// Before calling ReplaceLastText (see below), call this to find out who
	//  recorded the last piece of text.  This returns the szPkg passed to the
	//  recorder by the last package who called RecordText or ReplaceLastText.
	// The caller must allocate space for rszLastTextOwner.  This method
	//  expects that rszLastTextOwner points to REC_MAX_TEXT_OWNER_NAME
	//  many allocated bytes (including the byte for the NULL character
	//  at the end).  Typically a package will allocate a character array
	//  of this length on the stack, and pass the pointer to the array
	//  as rszLastTextOwner.
	STDMETHOD(GetLastTextOwner)(LPTSTR rszLastTextOwner) PURE;

	// Call this to replace the last text recorded (via RecordText or
	//  ReplaceLastText).  This is used to coalesce commands into a
	//  single VBS call.  For example, if a window is moved twice, the
	//  second move's VBS text can replace the first (since the first
	//  is rendered useless).  Similarly, if the user types "ab", both
	//  kepresses may be coalesced into a single VBS call.  Use
	//  GetLastTextOwner first to make sure you're really the package who
	//  wrote the last text.  Note that if the previous call to RecordText
	//  or RecordLastText contained several lines of code (separated with
	//  \n as described above) then ALL THOSE LINES will be replaced.
	// The length of szPkg must not exceed REC_MAX_TEXT_OWNER_NAME bytes
	//  (the ending NULL character is included in this count).
	STDMETHOD(ReplaceLastText)(LPCTSTR szText, LPCTSTR szPkg) PURE;
};

#endif //__AUT1API_H__
