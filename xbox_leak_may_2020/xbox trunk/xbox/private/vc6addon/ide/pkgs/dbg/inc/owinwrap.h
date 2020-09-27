#ifndef OWINWRAP_H

#define OWINWRAP_H	1

/* This file contains inline wrappers to make it easier to call output window
   functionality from the debugger */

inline void OutputWindowQueueHit(UINT nCommandId, LPCSTR lpszError,
                                 BOOL fToolError, BOOL fAppendCrLf, BOOL fForceFlush = FALSE)
{
	BOOL	fFull;
	
	ASSERT(gpIOutputWindow != NULL);

	VERIFY (SUCCEEDED (gpIOutputWindow->OwinIsBufferFull (&fFull)));

	if (!fFull) {

		HRESULT	hr;

		hr = gpIOutputWindow->OutputWindowQueueHit (nCommandId,
													lpszError,
													fToolError,
													fAppendCrLf,
													fForceFlush
													);

		ASSERT (SUCCEEDED (hr));
	}
}

inline void ShowOutputWindow( )
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->ShowOutputWindow()))
		ASSERT(FALSE);
}

inline void ClearOutputWindow( )
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->ClearOutputWindow()))
		ASSERT(FALSE);
}

inline void WriteStringToOutputWindow(LPCSTR lpszError, BOOL fToolError, BOOL fAppendCrLf)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->WriteStringToOutputWindow(lpszError, fToolError, fAppendCrLf)))
		ASSERT(FALSE);
}

inline void OutputWindowDeleteVwin(UINT nCommandId)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowDeleteVwin(nCommandId)))
		ASSERT(FALSE);
}

inline void OutputWindowShowVwin(UINT nCommandId)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowShowVwin(nCommandId)))
		ASSERT(FALSE);
}

inline void OutputWindowSelectVwin(UINT nCommandId, BOOL fOnTop)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowSelectVwin(nCommandId, fOnTop)))
		ASSERT(FALSE);
}

inline UINT OutputWindowAddVwin(LPSTR szToolName, BOOL fCurrentVwin)
{
	UINT uRet = 0;

	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowAddVwin(szToolName, fCurrentVwin, &uRet)))
		ASSERT(FALSE);

	return uRet;
}

inline UINT OutputWindowModifyVwin(UINT nCommandId, LPSTR lpszToolName)
{
	UINT uRet = 0;

	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowModifyVwin(nCommandId, lpszToolName, &uRet)))
		ASSERT(FALSE);

	return uRet;
}

inline void OutputWindowVwinClear(UINT nCommandId)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowVwinClear(nCommandId)))
		ASSERT(FALSE);
}

inline void OutputWindowClearWindow(UINT nCommandId)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowClearWindow(nCommandId)))
		ASSERT(FALSE);
}

inline void OutputWindowClearContents(UINT nCommandId)
{
	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OutputWindowClearContents(nCommandId)))
		ASSERT(FALSE);
}

inline UINT OwinGetBuildCommandId( )
{
	UINT uRet;

	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OwinGetBuildCommandID(&uRet)))
		ASSERT(FALSE);

	return uRet;
}

inline UINT OwinGetDebugCommandID( )
{
	UINT uRet;

	ASSERT(gpIOutputWindow != NULL);
	if (!SUCCEEDED(gpIOutputWindow->OwinGetDebugCommandID(&uRet)))
		ASSERT(FALSE);

	return uRet;
}

#endif // OWINWRAP_H
