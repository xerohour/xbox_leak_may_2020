/* MsgBoxes.h
 *
 * Purpose:	Specification file for the Dolphin WADG compliant message box 
 *			interface.
 *
 * Created:	22 July 1993 -by- Andrew Milton [w-amilt]
 *
 * Notes:	To ensure WADG compliance,  all message boxes in Dolphin must be 
 *			posted with some flavour of the MsgBox() function or one of its 
 *			derivatives [InformationBox(), QuestionBox(), ErrorBox()].  
 *			The message	text it contains should be formatted using one of the  
 *			MsgText() functions.  
 *
 *			The essential points we're trying to ensure here are avoiding 
 *			focus-theft from other applications and the three-line COMMDLG
 *			format for file errors.
 *
 *			Through the magic of enum's and overloading, a wide variety of 
 *			options are available for message posting and formatting.
 *
 *			Five message box types are currently available, defined by the
 *			enum MsgBoxTypes:  Information, Question, Error, InternalError, 
 *			and FatalError.  All forms of the MsgBox() function take a 
 *			MsgBoxTypes as the first parameter.  The default style for each
 *			type of message box is:
 *				Information		=>	MB_ICONINFORMATION	| MB_OK
 *				Question		=>	MB_ICONQUESTION		| MB_YESNOCANCEL
 *				Error			=>	MB_ICONEXCLAMATION	| MB_OK
 *				InternalError	=>	MB_ICONSTOP			| MB_OK
 *				FatalError		=>	MB_ICONSTOP			| MB_OK
 *			All message boxes use the MB_APPLMODAL mode.  Except for 
 *			InternalError and FatalError, message boxes are posted silently 
 *
 *			The button styles on the Question, Error and InternalError
 *			message box types may be overridden.  However, neither the
 *			icon nor the mode may be changed.  See the MsgBox() prototypes
 *			for more details.
 *
 *			InternalError and FatalError do special processing when raised.
 *				InternalError	=>	The error text is substituted into the 
 *									string "Internal Errror.  Please contact
 *									Microsoft Product Support Services (%s)"
 *									before it is displayed.
 *				FatalError		=>	Causes a panic shutdown of Dolphin.
 *
 *			The MsgBox() functions take either a string ID or a string 
 *			pointer for the the message text.  They do not allow variable 
 *			substitution into the message text.  The MsgText() family is 
 *			provided for substitions.  All return a LPCTSTR so that the call
 *			may be imbedded into a MsgBox() call, as in:
 *				MsgBox(Error,	
 *						MsgText(strBuffer, IDS_STRING_NOT_FOUND, pszToFind));
 *
 *			The MsgBox() functions also allow an F1 help context to be 
 *			passed as the last parameter.  The default is 'No help'.  If
 *			several message boxes are going to be displayed in a function 
 *			that use a common help context, then the default help can changed
 *			by delcaring a CLocalHelpContext object on the stack & passing
 *			the new default help context to the constructor.  The destructor
 *			will reset the help context when the object goes out of scope.
 *			Lastly, to get default buttond & set a help context, you can use 
 *			the DEFAULT_BUTTONS symbol, as in:
 *				MsgBox(Error,	
 *						MsgText(strBuffer, IDS_STRING_NOT_FOUND, pszToFind),
 *						DEFAULT_BUTTONS,
 *						helpFind);
 *
 *			Observe that all calls to LoadString() occur inline.  This 
 *			guarantees that if you have a local resource context set up, then
 *			MFC will look in the right place first for your string.
 *
 *			Lastly, the InformationBox(), QuestionBox() and ErrorBox() 
 *			functions are provided as convenience to the IDE.  They are 
 *			remnants of 'the old way' of doing message boxes.  If at all 
 *			possible, please avoid using these functions.  They're only
 *			inline wrappers to a MsgBox() and MsgText() call.
 *
 * Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
 ****************************************************************************/

#ifndef MSGBOXES_H
#define MSGBOXES_H

#include <tchar.h>
#include <stdarg.h>	// for var args stuff.
#include "path.h"

#define IDT_WINDOW_FLASH		101
#define DEFAULT_BUTTONS			((UINT) -1)
#define DEFAULT_HELP			((UINT) -1)
#define MAX_SUBSTITUTION_BYTES	1024

#ifndef _WIN32
#define LPCTSTR LPCSTR
#define LPTSTR LPSTR
#endif

/* WARNING!!!	WARNING!!!	WARNING!!!	 
 *
 * The following enum defines the available styles of message boxes.
 * If you add a message box style, then you MUST alter the three style arrays
 * g_rnIconStyles, g_rnButtonStyles, and  g_rfBeepOnMessage to reflect the 
 * change.
 */ 

#define NUMBER_OF_MESSAGE_BOXES 5

typedef enum tagMsgBoxTypes {
	Information		= 0, 
	Question		= 1, 
	Error			= 2, 
	InternalError	= 3, 
	FatalError		= 4
} MsgBoxTypes;

/* Function Prototypes ******************************************************/


extern void
InitMsgBoxes();	// Private to the message box implementation.  


/*************************** Interface Definition ***************************\
 MsgBox()

 Purpose:	To display a message box that conforms to the WADG.

 Returns:	The ID of the button pressed to dismiss the message.

 Notes:		- No mode or icon flags are allowed to be set for the message 
 			box by the calling function.  If any are slipped in with the 
			button options, then we ASSERT.
			- No button options are allowed with Information or FatalError
			message box types
			- If Dolphin is not active, then the IDE main frame is set into 
 			a flash state & the message box is suppressed until Dolphin 
 			becomes active. 
			- The displayed icon & default buttons are determined by the
			g_rnDefaultButtons[] and g_rnIconStyles[] arrays.  See the 
			WARNING!!! above.

			The first version of this function displays the string pointed 
			to by <pszMessage>.  The second version loads its message from 
			the resource table (inline), and then calls the first version to
			display the message.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/
 
int 
MsgBox(
	const MsgBoxTypes MsgBoxType, 
	LPCTSTR pszMessage,
	UINT nButtonIDs		= DEFAULT_BUTTONS,
	UINT nHelpContext	= DEFAULT_HELP);

inline int 
MsgBox(
	const MsgBoxTypes BoxType,
	const int nMessageID,
	UINT nButtonIDs		= DEFAULT_BUTTONS,
	UINT nHelpContext	= DEFAULT_HELP);

/*************************** Interface Definition ***************************\
 MsgText()

 Purpose:	To format text for display in a message box.  

 Returns:	Constant pointer to the formatted text.

 Notes:		Each version of this function requires a CString reference as its
 			first parameter.  This allows MsgText() to allocate buffer space
			on the heap for the formatted text with the assurance that the 
			buffer space will go away when the CString goes out of scope in 
			the calling function.  Why do we want to do this?  Well, it 
			guarantees that there will be enough space to store a message
			in the event that it changes length under translation to 
			different languages.  This way, we don't have to worry about
			buffer overruns on the stack (which can be unpleasant to isolate!)

			The first two versions of MsgText() corrospond with wsprintf()
			and wvsprintf() -- only the format string is loaded from the 
			resource table.

			The other two versions take a CFileException pointer & a CPath
			reference to construct a file error message.  The file error
			is formatted into three lines:

				<Name of the file>
				<Operation that failed>
				<Why it failed>

			This code is shamelessly based on code in <vres\mytlab.cpp>.

			The SubstituteBytes() function is reserved for use by the 
			message text routines.  It contains the common functionality for 
			the inline versions of MsgText().

			*IMPORTANT*

			The formatted text is stored in <strBuffer> AND returned by 
			MsgText().  If the same text is going to be used in several 
			message boxes, then  using the 	appropriate cast, <strBuffer> 
			can be reused in a later call to MsgBox().  ie:

			{
				CString strFooError;
				MsgBox(Error, MsgText(strFooError, IDS_FOO, cFooActions));

				// Stuff Happens to cause the same error 

				MsgBox(Error, (LPCTSR)strFooError);
			
			}

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

inline LPCTSTR
MsgText(
	CString &strBuffer,
	const int idsMsgFormat,
	...);

inline LPCTSTR
VMsgText(
	CString &strBuffer,
	const int idsMsgFormat,
	va_list VarArguments);

inline LPCTSTR
MsgText(
	CString &strBuffer,
	const CPath &pathOffendingFile,
	const int idsFileOperation,
	const CFileException *peOffence);

LPCTSTR
MsgText(
	CString &strBuffer,
	const CPath &pathOffendingFile,
	LPCTSTR pszFileOperation,
	const CFileException *peOffence);

LPCTSTR
SubstituteBytes(
	CString &strBuffer,
	LPCTSTR pszFormat,
	va_list VarArguments);	   // RESERVED.  Do not use.

/*************************** Interface Definition ***************************\
 InformationBox()
 QuestionBox()
 ErrorBox()

 Purpose:	Shortcut functions for the IDE's convenience.  Let's not be
 			using these, eh?

 Returns:	Information - Nada, Error - FALSE, Question - Button pressed.

 Notes:		Roughly speaking, we have <Foo>Box(ids, <stuff>) is the same as
 			MsgBox(<Foo>, MsgText(str, <stuff>))
			The exception is QuestionBox, which sets the available buttons.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

inline void
InformationBox(
	UINT idsMsgFormat,
	...);

inline int
QuestionBox(
	UINT idsMsgFormat,
	UINT nButtons,
	...);

inline BOOL
ErrorBox(
	UINT idsMsgFormat,
	...);

/*************************** Interface Definition ***************************\
 class CLocalHelpContext
 PushHelpContext()
 PopHelpContext()

 Purpose:	To set/reset the default message box F1-help context.
  
 Returns:	Push - Nada.  Pop - last help context

 Notes:		The push/pop functions should *not* be called directly.  The 
 			CLocalHelpContext constuctor/destructor call them to set up local
			help contexts, and using this class guarantees that help contexts
			will be reset at the end of a scope block.  See the comments at
			the top of the file for more details.

 Copyright (c) 1993 Microsoft Corporation.  All rights reserved.
\****************************************************************************/

void
PushHelpContext(             
	UINT nNewHelp);

UINT
PopHelpContext();

class CLocalHelpContext {
public:
	CLocalHelpContext(UINT nHelp) { PushHelpContext(nHelp);}
	~CLocalHelpContext() 		 { PopHelpContext();}
};

// Stolen out of the debugger headers for consistancy
void FAR PASCAL 
DebuggerMessage(
	MsgBoxTypes MsgType, 
	UINT idsErrorFormat,	
	LPSTR Msg,	
	int ShowType);

#include "msgboxes.inl"

/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog
//	Simulates a message box (by borrowing from Windows code)
//	but allows you to set your own buttons
//	Can either create an instance, and call DoModal(), 
//	or call the static DisplayMessageBox().

class CMessageBox : public CDialog
{
// Construction
public:
	CMessageBox(LPCSTR lpszText, LPCSTR lpszCaption, LPCSTR lpszButtons, 
		WORD wStyle, DWORD* pHelpIDs = NULL, CWnd* pParentWnd = NULL);
	~CMessageBox();

// Attributes
	CFont* m_pFont;
// Operations
	static int DoMessageBox(LPCSTR lpszText, LPCSTR lpszCaption, 
		LPCSTR lpszButtons, WORD wStyle, int nDef = 0, int nCancel = -1, 
		DWORD* pHelpIDs = NULL, CWnd* pParentWnd = NULL);

	void AddButton(CString& strButton) { m_strArray.Add(strButton);}
	void AddButtons(LPCSTR lpszButton);
	void SetCancel(int nCancel)
		{ ASSERT(nCancel < m_strArray.GetSize()); m_nCancel = nCancel;}
	void SetDefault(int nDef)
		{ ASSERT(nDef < m_strArray.GetSize()); m_nDefButton = nDef;}
	void FillInHeader(LPDLGTEMPLATE lpDlgTmp);

// Overridables
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual int DoModal();

// Implementation
protected:
	virtual const DWORD* GetHelpIDs() {return m_pHelpIDs;}
	DWORD* m_pHelpIDs;
	int m_nDefButton;
	int m_nCancel;
	HGLOBAL m_hDlgTmp;
	UINT m_nBaseID;
	WORD m_wStyle;
	CButton* m_pButtons;
	CStatic m_staticIcon;
	CStatic m_staticText;
	CString m_strCaption;
	CString m_strText;
	CStringArray m_strArray;
	CSize GetBaseUnits();
	LPCSTR GetIconID(WORD wFlags);
	void PositionControls();

	// Generated message map functions
	//{{AFX_MSG(CMessageBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////
//	File Error support
//		note that file error codes where the high word is 0 are 
//		CFileException::m_cause codes.

#ifndef __IDS_H__
#include "ids.h"
#endif

enum
{
	ferrShellPackageFirst = MAKELONG(1000, PACKAGE_SUSHI),
	ferrCantSaveReadOnly,
	ferrCantSaveTempFiles,
	ferrIllformedFile,
};

int GetFileError();
void SetFileError(int nCause);
void FileErrorMessageBox(UINT idsOperation, LPCTSTR szFileName);

#endif	// MSGBOXES_H

