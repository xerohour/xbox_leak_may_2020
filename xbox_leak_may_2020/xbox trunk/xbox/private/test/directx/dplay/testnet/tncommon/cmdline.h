#ifndef __COMMAND_LINE__
#define __COMMAND_LINE__


#if ((defined (CMDLINE_CONSOLE_ONLY)) && (defined (CMDLINE_WINDOWS_ONLY)))
#error You cannot define both CMDLINE_CONSOLE_ONLY and CMDLINE_CONSOLE_ONLY
#endif // CMDLINE_CONSOLE_ONLY and CMDLINE_WINDOWS_ONLY




//==================================================================================
// Defines
//==================================================================================
#ifdef LOCAL_TNCOMMON

	// define LOCAL_TNCOMMON when including this code directly into your project
	#ifdef DLLEXPORT
		#undef DLLEXPORT
	#endif // DLLEXPORT defined
	#define DLLEXPORT

#else // ! LOCAL_TNCOMMON

	#ifdef TNCOMMON_EXPORTS

		// define TNCOMMON_EXPORTS only when building the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllexport)

	#else // ! TNCOMMON_EXPORTS

		// default behavior is to import the functions from the TNCOMMON DLL
		#ifdef DLLEXPORT
			#undef DLLEXPORT
		#endif // DLLEXPORT defined
		#define DLLEXPORT __declspec(dllimport)

	#endif // ! TNCOMMON_EXPORTS
#endif // ! LOCAL_TNCOMMON

#ifndef DEBUG
	#ifdef _DEBUG
		#define DEBUG
	#endif // _DEBUG
#endif // DEBUG not defined



//==================================================================================
// Defines
//==================================================================================
#ifndef CMDLINE_SWITCH_TOKENS
// Switches begin with one of these characters
#define CMDLINE_SWITCH_TOKENS	"/+-"
#endif // ! CMDLINE_SWITCH_TOKENS

#ifndef CMDLINE_VALUE_TOKENS
// Switch names end and values begin with one of these characters
#define CMDLINE_VALUE_TOKENS	":="
#endif // ! CMDLINE_VALUE_TOKENS



//==================================================================================
// Errors
//==================================================================================
// No error
#define CMDLINE_OK							S_OK

// You specified a handled item more than once to Initialize.
#define CMDLINEERR_DUPLICATEHANDLEDITEM		ERROR_ALREADY_EXISTS

// You specified a handled item of an unknown type to Initialize.
#define CMDLINEERR_UNKNOWNITEMTYPE			ERROR_INVALID_DATA

// You specified that parameters must be passed to the app, but none were given.
#define CMDLINEERR_NOPARAMSPASSED			ERROR_BAD_LENGTH

// The command line did not contain a parameter that was marked as required.
#define CMDLINEERR_REQUIREDPARAMNOTPASSED	ERROR_NOT_FOUND

// You passed something goofy into the init routine.
#define CMDLINEERR_INVALIDINITDATA			ERROR_INVALID_PARAMETER

// The command line contained a parameter with a value that wasn't recognized or
// that isn't allowed.
#define CMDLINERR_INVALIDPARAMETERVALUE		ERROR_BAD_COMMAND




//==================================================================================
// Initialization flags
//==================================================================================
// Use this flag if you want the parser to match parameter names exactly.
#define CMDLINE_FLAG_CASESENSITIVE				0x01

// Use this so that Parse will fail with CMDLINEERR_NOPARAMSPASSED when no items are
// specified on the command line.
#define CMDLINE_FLAG_FAILIFNOPARAMS				0x02

// If a parameter that is required (see CMDLINE_PARAMOPTION_FAILIFNOTEXIST) does not
// exist, then the object is automatically placed in help mode (the caller can access
// this information via IsInHelpMode and can act however he sees fit, but usually he
// calls DisplayHelp).
#define CMDLINE_FLAG_MISSINGPARAMMEANSHELP		0x04

// If INI files are allowed, then if the user passes the switch "ini" followed by
// a valid file name on the command line, the file is read in and items under the
// "[CommandLine]" section in the file are parsed as if they were specified on the
// command line.  This flag prevents the ini switch from automatically being
// handled.
#define CMDLINE_FLAG_DONTHANDLEINIFILES			0x08

// Use this if you do not want the parser to automatically detect the help switch.
#define CMDLINE_FLAG_DONTHANDLEHELPSWITCHES		0x10

// Use this if you do not want the parser to automatically detect the debug switch.
#define CMDLINE_FLAG_DONTHANDLEDEBUGSWITCH		0x20

// Use this if you do not want the parser to automatically detect the HTML switch.
#define CMDLINE_FLAG_DONTHANDLEHTMLSWITCH		0x40


/*
// Use this so that if a non-switch is found while parsing the command line,
// CMDLINE will stop processing and return CMDLINERR_ILLEGAL_PARAMETER.
#define CMDLINE_NONSWITCHES_CAUSE_ERROR			?

// Use this if you want to simply ignore any non-switches.  Will cause
// CMDLINEERR_INVALID_INIT_FLAGS if specified with CMDLINE_NONSWITCHES_CAUSE_ERROR.
#define CMDLINE_IGNORE_NONSWITCHES				?
*/





//==================================================================================
// Structures
//==================================================================================
typedef struct tagCMDLINEHANDLEDITEM
{
	DWORD		dwType; // kind of item this is, see CMDLINE_PARAMTYPE_xxx
	char*		pszName; // pointer to string to look for in command line
	char*		pszDescription; // pointer to string to description of command
	DWORD		dwOptions; // additional options for this item, see CMDLINE_PARAMOPTION_xxx
	PVOID*		ppvValuePtr; // pointer to default value and place to update to value
	PVOID		pvAdditionalData; // pointer to additional data, if any, for param type
	DWORD		dwAdditionalDataSize; // size of above additional data
} CMDLINEHANDLEDITEM, * PCMDLINEHANDLEDITEM;

typedef struct tagCMDLINEIDITEM
{
	char*		pszName; // pointer to string that is a valid value
	DWORD		dwValue; // value to assign if the string is found
} CMDLINEIDITEM, * PCMDLINEIDITEM;






//==================================================================================
// Parameter types
//==================================================================================
// Note that the value and additional data pointers must remain valid throughout the
// life of the CCommandLine object using them.

// BOOL
// If the name exists on the command line, the value becomes the toggled default
// value, i.e. if you set the default value to TRUE, specifying it will set the
// value to FALSE.  If the name is followed by a string, then the value becomes the
// interpretation of that string.  "0"'s, "false" or "off" (case-insensitive) mean
// FALSE, anything else is interpreted as TRUE.
// This switch can NOT be separated from its value by a space.
//
// The default value is a cast as a BOOL.
#define CMDLINE_PARAMTYPE_BOOL			1
#define CLPT_BOOL						CMDLINE_PARAMTYPE_BOOL


// INT
// The string following the name is converted into an int, which becomes the value.
// This switch can be separated from its value by a space.
//
// The value pointer can point to an int.
// There is no additional data.
#define CMDLINE_PARAMTYPE_INT			2
#define CLPT_INT						CMDLINE_PARAMTYPE_INT


// DWORD
// The string following the name is converted into a DWORD, which becomes the value.
// This switch can be separated from its value by a space.
//
// The value pointer can point to a DWORD.
// There is no additional data.
#define CMDLINE_PARAMTYPE_DWORD			3
#define CLPT_DWORD						CMDLINE_PARAMTYPE_DWORD


// DWORD_HEX
// The string following the name is converted into a hexadecimal DWORD, which
// becomes the value.
// This switch can be separated from its value by a space.
//
// The value pointer can point to a DWORD.
// There is no additional data.
#define CMDLINE_PARAMTYPE_DWORD_HEX		4
#define CLPT_DWORD_HEX					CMDLINE_PARAMTYPE_DWORD_HEX


// STRING
// The string following the name becomes the value.  The default value passed in
// is duplicated, so it can safely be released after the Initialize call, if you
// want.
// This switch can be separated from its value by a space.
//
// The value pointer can point to a string (char*).
// There is no additional data.
#define CMDLINE_PARAMTYPE_STRING		5
#define CLPT_STRING						CMDLINE_PARAMTYPE_STRING


// ID
// The string following the name is checked against the table passed in the
// additional data parameters, and if found, the value becomes the associated
// DWORD.
// This switch can be separated from its value by a space.
//
// The value pointer can point to a DWORD.
// The additional data must point to an array of CMDLINEIDITEMs holding strings and
// their associated values.
#define CMDLINE_PARAMTYPE_ID			6
#define CLPT_ID							CMDLINE_PARAMTYPE_ID


// MULTISTRING
// Multiple instances of parameters with this name can be specified, and a list
// of their values is built up.  If the default value exists, that existing
// CStringList is used, otherwise a new one is created.  Note there is no reference
// counting on the list object, so you must delete the command line object before
// deleting the CStringList.
// This switch can be separated from its value by a space.
//
// The value pointer can point to an PLSTRINGLIST.
// There is no additional data.
#define CMDLINE_PARAMTYPE_MULTISTRING	7
#define CLPT_MULTISTRING				CMDLINE_PARAMTYPE_MULTISTRING




//==================================================================================
// Parameter options
//==================================================================================
// If this item was not found on the command line, then return
// CMDLINEERR_REQUIREDPARAMNOTPASSED or go into help mode, if
// CMDLINE_FLAG_MISSINGPARAMMEANSHELP was specified.
// All types can specify this.
#define CMDLINE_PARAMOPTION_FAILIFNOTEXIST			0x1

// Don't display this item when displaying help.
// All types can specify this
#define CMDLINE_PARAMOPTION_HIDDEN					0x2

// Normally passing unknown IDs (strings that aren't in the lookup table) will cause
// parsing to fail with CMDLINERR_INVALIDPARAMETERVALUE.  Specifying this flag will
// causing parsing to continue (the item will be marked as found, but the value will
// remain the default).
// Only PARAMTYPE_IDs can specify this, it is ignored otherwise.
#define CMDLINE_PARAMOPTION_DONTFAILIFUNKNOWNID		0x4






//==================================================================================
// Parameter types
//==================================================================================
// Should the function fail if the specified file already exists?
#define CMDLINE_WRITEINIOPTION_FAILIFEXISTS			0x1

// If this is specified and the file exists, the text is added to the end of the
// file.
#define CMDLINE_WRITEINIOPTION_APPEND				0x2

// If this is used, the description specified for each command line item is printed
// in comments immediately preceding the command line item.
#define CMDLINE_WRITEINIOPTION_PRINTDESCRIPTIONS	0x4





//==================================================================================
// Class typedefs
//==================================================================================
typedef class CHandledParam				CHandledParam,				* PHANDLEDPARAM;
typedef class CHandledParamBool			CHandledParamBool,			* PHANDLEDPARAMBOOL;
typedef class CHandledParamInt			CHandledParamInt,			* PHANDLEDPARAMINT;
typedef class CHandledParamDWord		CHandledParamDWord,			* PHANDLEDPARAMDWORD;
typedef class CHandledParamDWordHex		CHandledParamDWordHex,		* PHANDLEDPARAMDWORDHEX;
typedef class CHandledParamString		CHandledParamString,		* PHANDLEDPARAMSTRING;
typedef class CHandledParamID			CHandledParamID,			* PHANDLEDPARAMID;
typedef class CHandledParamMultiString	CHandledParamMultiString,	* PHANDLEDPARAMMULTISTRING;

typedef class CHandledParamsList		CHandledParamsList,			* PHANDLEDPARAMSLIST;


typedef class CSpecifiedParam			CSpecifiedParam,			* PSPECIFIEDPARAM;
typedef class CSpecifiedParamsList		CSpecifiedParamsList,		* PSPECIFIEDPARAMSLIST;


typedef class CCommandLine				CCommandLine,				* PCOMMANDLINE;





#ifndef CMDLINE_CONSOLE_ONLY
//==================================================================================
// Local prototypes
//==================================================================================
INT_PTR CALLBACK HelpBoxWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // ! CMDLINE_CONSOLE_ONLY




//==================================================================================
// Classes
//==================================================================================
class DLLEXPORT CHandledParam:public CLString
{
	public:
		char*				m_pszDescription; // optional description of this item
		DWORD				m_dwType; // type of this item (i.e. child class ID)
		DWORD				m_dwOptions; // options for item
		int					m_fFound; // whether the item was found on the command line, or not
		PVOID*				m_ppvValuePtr; // pointer to place to update value


		CHandledParam(char* szName, char* pszDescription, DWORD dwType,
					DWORD dwOptions, PVOID* ppvValuePtr);
		virtual ~CHandledParam(void);

		// Pure virtual functions.  The child classes had better implement these
		// because we sure don't.
		virtual HRESULT GetSelfAndValues(BOOL fCaseSensitive, PSPECIFIEDPARAMSLIST pItemList) = 0;
#ifndef _XBOX // no file printing
		virtual HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose) = 0;
#endif // ! XBOX
};


class DLLEXPORT CHandledParamBool:public CHandledParam
{
	public:
		DWORD_PTR			m_fValue; // value of this item


		CHandledParamBool(char* szName, char* pszDescription, DWORD dwOptions,
						PVOID* ppvValuePtr);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};


class DLLEXPORT CHandledParamInt:public CHandledParam
{
	public:
		INT_PTR				m_iValue; // value of this item


		CHandledParamInt(char* szName, char* pszDescription, DWORD dwOptions,
						PVOID* ppvValuePtr);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};


class DLLEXPORT CHandledParamDWord:public CHandledParam
{
	public:
		DWORD_PTR				m_dwValue; // value of this item


		CHandledParamDWord(char* szName, char* pszDescription, DWORD dwOptions,
							PVOID* ppvValuePtr);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};


class DLLEXPORT CHandledParamDWordHex:public CHandledParam
{
	public:
		DWORD_PTR				m_dwValue; // value of this item


		CHandledParamDWordHex(char* szName, char* pszDescription, DWORD dwOptions,
							PVOID* ppvValuePtr);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};


class DLLEXPORT CHandledParamString:public CHandledParam
{
	public:
		char*				m_pszValue; // value of this item


		CHandledParamString(char* szName, char* pszDescription, DWORD dwOptions,
							PVOID* ppvValuePtr);
		virtual ~CHandledParamString(void);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};


class DLLEXPORT CHandledParamID:public CHandledParam
{
	public:
		PCMDLINEIDITEM		m_paLookupTable; // table to look up string values in
		DWORD				m_dwNumTableEntries; // number of items in the above table
		DWORD_PTR			m_dwValue; // value of this item


		CHandledParamID(char* szName, char* pszDescription, DWORD dwOptions,
						PVOID* ppvValuePtr, PCMDLINEIDITEM paLookupTable,
						DWORD dwNumTableEntries);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};


class DLLEXPORT CHandledParamMultiString:public CHandledParam
{
	public:
		PLSTRINGLIST		m_pValues; // list of values for this item
		BOOL				m_fCreatedValuesList; // TRUE if we created the above list, FALSE if it was given to us


		CHandledParamMultiString(char* szName, char* pszDescription, DWORD dwOptions,
								PVOID* ppvValuePtr);
		virtual ~CHandledParamMultiString(void);

		// Virtual function implementation.
		HRESULT GetSelfAndValues(BOOL fCaseSensitive,
								PSPECIFIEDPARAMSLIST pItemList);
#ifndef _XBOX // no file printing
		HRESULT PrintToFile(HANDLE hFile, BOOL fVerbose);
#endif // ! XBOX
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class DLLEXPORT CHandledParamsList:public CLStringList
{
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class DLLEXPORT CSpecifiedParam:public CLString
{
	public:
		BOOL	m_fSwitch; // is this item a switch
		BOOL	m_fSeparated; // whether this item was split from from the previous item


		CSpecifiedParam(char* szName, BOOL fSwitch, BOOL fSeparated);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class DLLEXPORT CSpecifiedParamsList:public CLStringList
{
	public:
		HRESULT AddItem(char* szString, BOOL fSwitch, BOOL fTryToSplit,
						BOOL fWasSplit);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class DLLEXPORT CCommandLine
{
#ifndef CMDLINE_CONSOLE_ONLY
	// This is a friend so it can access the protected members
	friend INT_PTR CALLBACK HelpBoxWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif // ! CMDLINE_CONSOLE_ONLY


	private:
		char*					m_pszPathToApp; // path to the application that created this object
		char*					m_pszOriginalCommandLine; // original command line item passed
#if (! ((defined (CMDLINE_CONSOLE_ONLY)) || (defined (CMDLINE_WINDOWS_ONLY))))
		BOOL					m_fConsole; // was the console version of Parse called?
#endif // not (CMDLINE_CONSOLE_ONLY or CMDLINE_WINDOWS_ONLY)
		DWORD					m_dwFlags; // flags we were initialized with

		BOOL					m_fHTML; // was the HTML switch found?
		BOOL					m_fHelp; // was a help switch found or a required parameter missing?
		BOOL					m_fDebug; // was the debug switch found?
		char*					m_pszIniFile; // pointer to INI file specified, if any

		CHandledParamsList		m_handleditems; // list of items that can be handled
		CSpecifiedParamsList	m_specifieditems; // list of items that were specified
		PHANDLEDPARAM*			m_papHelpItems; // temporary array of help items to display
		DWORD					m_dwNumHelpItems; // how many items in the preceding array
		LONG					m_lMaxHelpLineWidth; // max number of units a line can hold
		LONG					m_lMaxHelpHeight; // max number of units a help page can hold
		LONG					m_lLargestLeftHelpTextWidth; // largest name width
		LONG					m_lLargestRightHelpTextWidth; // largest description width
		LONG					m_lLargestHelpLineHeight; // largest help line height
#ifndef CMDLINE_CONSOLE_ONLY
		char*					m_pszTempParamNameBuffer; // buffer to hold largest possible parameter name
#endif // not CMDLINE_CONSOLE_ONLY



		HRESULT AddItemInternal(PCMDLINEHANDLEDITEM pItem,
								PHANDLEDPARAMSLIST pHandledItems);

		HRESULT ParseInternal(char* szCommandLine,
								PHANDLEDPARAMSLIST pHandledItems,
								PSPECIFIEDPARAMSLIST pSpecifiedItems);

		HRESULT ReadIniFileInternal(char* szFilepath,
									PSPECIFIEDPARAMSLIST pItemList);



	protected:
		DWORD		m_dwHelpStartItem; // first item index for current page


		HRESULT DisplayHelpPageInternal(DWORD* pdwStartItem, HWND hWnd);

	
	public:
		DLLSAFE_NEWS_AND_DELETES;

		CCommandLine(void);
		virtual ~CCommandLine(void);


		HRESULT Initialize(DWORD dwFlags, PCMDLINEHANDLEDITEM aHandledItems,
							DWORD dwNumHandledItems);

		// Overloaded
#ifndef CMDLINE_CONSOLE_ONLY
		HRESULT Parse(LPCSTR lpcszString);
#endif // ! CMDLINE_CONSOLE_ONLY
#ifndef CMDLINE_WINDOWS_ONLY
		HRESULT Parse(int iNumArgs, char* aszArgsArray[]);
#endif // ! CMDLINE_CONSOLE_ONLY

		BOOL InHelpMode(void);
		BOOL InDebugMode(void);
		BOOL InHTMLMode(void);

		HRESULT DisplayHelp(BOOL fAlphabetize);

		HRESULT CheckForItem(PCMDLINEHANDLEDITEM pHandledItem, BOOL* pfWasSpecified);
		HRESULT CheckForItems(PCMDLINEHANDLEDITEM aHandledItems, DWORD dwNumHandledItems);

		HRESULT GetPathToApp(char* pszBuffer, DWORD* pdwBufferSize);
		HRESULT GetPathToAppDir(char* pszBuffer, DWORD* pdwBufferSize);
		HRESULT GetNameOfApp(char* pszBuffer, DWORD* pdwBufferSize);

		BOOL WasSpecified(PVOID* ppvItem);

		char* GetSwitchValueString(char* szName);

		HRESULT WriteToINI(char* szFilepath, DWORD dwOptions);
};





#endif //__COMMAND_LINE__
