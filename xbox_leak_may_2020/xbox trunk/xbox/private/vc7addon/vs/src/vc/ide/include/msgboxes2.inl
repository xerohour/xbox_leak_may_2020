inline LPCTSTR VVCMsgText(CVCString &strBuffer, const int idsMsgFormat, va_list VarArguments)
{
	CVCString strMsgFormat;
	BOOL bOK = strMsgFormat.LoadString(idsMsgFormat);
	VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?");
	return VCSubstituteBytes(strBuffer, strMsgFormat, VarArguments);
}

inline LPCTSTR VCMsgText(CVCString &strBuffer, const int idsMsgFormat, ...)
{ 
	CVCString strMsgFormat;
	BOOL bOK = strMsgFormat.LoadString(idsMsgFormat);
	VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?");
	va_list VarArguments;
	va_start(VarArguments, idsMsgFormat);
	return VCSubstituteBytes(strBuffer, strMsgFormat, VarArguments);
}

inline LPCTSTR VCMsgText(CVCString &strBuffer, const CVCPath &pathOffendingFile, const int idsFileOperation)
{
	CVCString strOperation;
	BOOL bOK = strOperation.LoadString(idsFileOperation);
	VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?");
	return VCMsgText(strBuffer, pathOffendingFile, strOperation);
}

inline LPCOLESTR VVCMsgTextW(CStringW &strBuffer, const int idsMsgFormat, va_list VarArguments)
{
	CStringW strMsgFormat;
	BOOL bOK = strMsgFormat.LoadString(idsMsgFormat);
	VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?");
	return VCSubstituteBytesW(strBuffer, strMsgFormat, VarArguments);
}

inline LPCOLESTR VCMsgTextW(CStringW &strBuffer, const int idsMsgFormat, ...)
{ 
	CStringW strMsgFormat;
	BOOL bOK = strMsgFormat.LoadString(idsMsgFormat);
	VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?");
	va_list VarArguments;
	va_start(VarArguments, idsMsgFormat);
	return VCSubstituteBytesW(strBuffer, strMsgFormat, VarArguments);
}

inline LPCOLESTR VCMsgTextW(CStringW &strBuffer, const CPathW &pathOffendingFile, const int idsFileOperation)
{
	CStringW strOperation;
	BOOL bOK = strOperation.LoadString(idsFileOperation);
	VSASSERT(bOK, "Failed to load string!  Are the resources initialized properly?");
	return VCMsgTextW(strBuffer, pathOffendingFile, strOperation);
}

/************************** DLL Exported Function ***************************\
 SubstituteBytes()

 Purpose:	To construct a display string given a CString buffer, a format
 			string, and a va_list of substitution variables.

 Returns:	Constant pointer to the constructed string.

 Notes:		This function should *not* be called directly.  It is used by the
 			inline versions of the MsgText() functions to construct message
 			box string.  See the header file for details.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

inline LPCTSTR VCSubstituteBytes(CVCString &strBuffer, LPCTSTR pszFormat, va_list VarArguments)
{
// Get some space in the buffer...

    int cchBuffer = lstrlen(pszFormat) + MAX_SUBSTITUTION_BYTES;
    LPTSTR pszBuffer = strBuffer.GetBuffer(cchBuffer);

// ...and fill it.

	wvsprintf(pszBuffer, pszFormat, VarArguments);
	strBuffer.ReleaseBuffer();

	return strBuffer;
}

inline LPCOLESTR VCSubstituteBytesW(CStringW &strBuffer, LPCOLESTR pszFormat, va_list VarArguments)
{
// Get some space in the buffer...

    int cchBuffer = (int)wcslen(pszFormat) + MAX_SUBSTITUTION_BYTES*sizeof(wchar_t);
    LPOLESTR pszBuffer = strBuffer.GetBuffer(cchBuffer);

// ...and fill it.

	wvnsprintfW(pszBuffer, cchBuffer-1, pszFormat, VarArguments); // this API is not wrapped, but does existon Win9x with ie5
	strBuffer.ReleaseBuffer();

	return strBuffer;
}



