/* MsgBoxes.inl
 *                                                    
 * Purpose:	Inline implementations for message boxes.
 *
 * Created:	28 July 1993 -by- Andrew Milton [w-amilt]
 *
 * Notes:	See <MsgBoxes.h> for details.
 *	
 * Copyright (c) 1993 Microsoft Corporation.  All rights reserved. 
 ****************************************************************************/

#ifndef MSGBOXES_H

#error Must #include 'msgboxes.h' before 'msgboxes.inl'

#else

#ifndef MSGBOXES_INL
#define MSGBOXES_INL

/***************************** Inline Function ******************************\
 MsgBox()

 Purpose:	Load a string from the resource table & display it in a message
 			box.

 Returns:	ID of the button pressed to dismiss the box.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/
 
inline int 
MsgBox(
	const MsgBoxTypes BoxType,
	const int nMessageID,
	UINT nButtonIDs, 	/* = DEFAULT_BUTTONS 	*/
	UINT nHelpContext)	/* = DEFAULT_HELP		*/
{
	CString strMsg;
	strMsg.LoadString(nMessageID);
	return(MsgBox(BoxType, strMsg, nButtonIDs, nHelpContext));
}

/***************************** Inline Function ******************************\
 [V]MsgText()

 Purpose:	To load strings from the resource table for formatting purposes.

 Returns:	Pointer to the formatted text.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

inline LPCTSTR
VMsgText(
	CString &strBuffer,
	const int idsMsgFormat,
	va_list VarArguments)
{
	CString strMsgFormat;
	VERIFY(strMsgFormat.LoadString(idsMsgFormat));
	return SubstituteBytes(strBuffer, strMsgFormat, VarArguments);
}

inline LPCTSTR
MsgText(
	CString &strBuffer,
	const int idsMsgFormat,
	...)
{ 
	CString strMsgFormat;
	VERIFY(strMsgFormat.LoadString(idsMsgFormat));
	va_list VarArguments;
	va_start(VarArguments, idsMsgFormat);
	return SubstituteBytes(strBuffer, strMsgFormat, VarArguments);
}

inline LPCTSTR
MsgText(
	CString &strBuffer,
	const CPath &pathOffendingFile,
	const int idsFileOperation,
	const CFileException *peOffence)
{
	CString strOperation;
	VERIFY(strOperation.LoadString(idsFileOperation));
	return MsgText(strBuffer, pathOffendingFile, strOperation, peOffence);
}

/***************************** Inline Function ******************************\
 InformationBox()
 QuestionBox()
 ErrorBox()

 Purpose:	Shortcut functions for the IDE's convenience.  Let's not be
 			using these, eh?

 Returns:	Information - Nada, Error - FALSE, Question - Button pressed.

 Notes:		Roughly speaking, we have <Foo>Box(ids, <stuff>) is the same as
 			MsgBox(<Foo>, MsgText(str, ids, <stuff>))
			The exception is QuestionBox, which sets the available buttons.

			These functions were ripped out of VCPP32\C\UTIL.C 

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

inline BOOL __cdecl 
ErrorBox(
	UINT idsMsgFormat,
	...)
{
	CString strErrText;
	va_list VarArguments;
	va_start(VarArguments, idsMsgFormat);

	MsgBox(Error, VMsgText(strErrText, idsMsgFormat, VarArguments));

	return FALSE; //Keep it always FALSE please
}

inline void __cdecl
InformationBox(
	UINT idsMsgFormat,
	...)
{
	CString strInfoText;
	va_list VarArguments;
	va_start(VarArguments, idsMsgFormat);

	MsgBox(Information, VMsgText(strInfoText, idsMsgFormat, VarArguments));

	return; 
}

inline int __cdecl 
QuestionBox(
	UINT idsMsgFormat,
	UINT nButtons,
	...)
{
	CString strQuestionText;
	va_list VarArguments;
	va_start(VarArguments, nButtons);

	return MsgBox(Question, 
					VMsgText(strQuestionText, idsMsgFormat, VarArguments),
					nButtons);
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

inline LPCTSTR
SubstituteBytes(
	CString &strBuffer,
	LPCTSTR pszFormat,
	va_list VarArguments)
{
// Get some space in the buffer...

    int cchBuffer = lstrlen(pszFormat) + MAX_SUBSTITUTION_BYTES;
    LPTSTR pszBuffer = strBuffer.GetBuffer(cchBuffer);

// ...and fill it.

	VERIFY(wvsprintf(pszBuffer, pszFormat, VarArguments) < cchBuffer);
	strBuffer.ReleaseBuffer();

	return strBuffer;
}


#endif
#endif 
