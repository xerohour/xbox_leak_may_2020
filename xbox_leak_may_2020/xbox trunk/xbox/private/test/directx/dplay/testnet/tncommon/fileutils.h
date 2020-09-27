#ifndef __TNCOMMON_FILEUTILS__
#define __TNCOMMON_FILEUTILS__





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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define READTEXTFILE_BUFFER_SIZE_INCREMENT		50




//----------------------------------------------------------------------------------
// Read text file options
//----------------------------------------------------------------------------------
// Setting this causes empty lines not to be passed to the callback.
#define FILERTFO_SKIP_EMPTY_LINES					0x001

// Specify this option to identify of the section/subsection etc.  These are .ini
// style tokens.  A section is defined by "[sectionname]".  Sub-sections under that
// are "{subsectionname}".  A further level can be denoted by parentheses
// "(subsubsectionname)".  See the FILERTFT_xxx line types.
#define FILERTFO_PARSE_INI_STYLE_SECTIONS			0x002

// Use this to ignore beginning and ending white space on each line
#define FILERTFO_IGNORE_SURROUNDING_WHITESPACE		0x004

// Specify this option to ignore anything after double slashes ("//") and between
// "/*" "*/" tokens.  The latter can be spread across multiple lines.
#define FILERTFO_IGNORE_C_COMMENTS					0x008

// Specify this option to ignore anything after a semicolon (";")
#define FILERTFO_IGNORE_SEMICOLON_COMMENTS			0x010

// If this option is specified, environment variables found in the file are
// expanded to their value.  The parser looks for the string "%VARNAME%", and
// replaces that with the value assigned to VARNAME, if any.
#define FILERTFO_EXPAND_ENV_VARS					0x020

// If this option is specified, additional files can be included by using #include
// followed by the filename in quotes.  The given file is then read as if the text
// it contains were actually inside the including file.
#define FILERTFO_ALLOW_POUND_INCLUDES				0x040

// If this option is specified, environment variables can be declared using
// #define by the variable name.  An optional value can also be specified after
// the name.  This can be useful when combined with FILERTFO_EXPAND_ENV_VARS.
#define FILERTFO_ALLOW_POUND_DEFINES				0x080

// If this option is specified, then any line that ends with a backslash character
// is considered to be continued on the next line, and the carraige-return/newline
// sequence is ignored.
#define FILERTFO_ALLOW_ESCAPED_NEWLINES				0x100


#define FILERTFO_INIOPTIONS							(FILERTFO_SKIP_EMPTY_LINES |\
													FILERTFO_PARSE_INI_STYLE_SECTIONS |\
													FILERTFO_IGNORE_SURROUNDING_WHITESPACE)

#define FILERTFO_EXTENDEDINIOPTIONS					(FILERTFO_INIOPTIONS |\
													FILERTFO_IGNORE_C_COMMENTS |\
													FILERTFO_EXPAND_ENV_VARS |\
													FILERTFO_ALLOW_POUND_INCLUDES |\
													FILERTFO_ALLOW_POUND_DEFINES |\
													FILERTFO_ALLOW_ESCAPED_NEWLINES)

#define FILERTFO_ALLOPTIONS							(FILERTFO_EXTENDEDINIOPTIONS |\
													FILERTFO_IGNORE_SEMICOLON_COMMENTS)

//----------------------------------------------------------------------------------
// Read text file line types
//----------------------------------------------------------------------------------
// This line is just a regular line.  Will always be this if
// FILERTFO_PARSE_INI_STYLE_SECTIONS is not specifed.
#define FILERTFT_NORMAL			1

// This line is a section header.  FILERTFO_PARSE_INI_STYLE_SECTIONS must have been
// specified.  The "[]" characters are stripped.
#define FILERTFT_SECTION		2

// This line is a subsection header.  FILERTFO_PARSE_INI_STYLE_SECTIONS must have
// been specified.  The "{}" characters are stripped.
#define FILERTFT_SUBSECTION		3

// This line is a subsubsection header.  FILERTFO_PARSE_INI_STYLE_SECTIONS must have
// been specified.  The "()" characters are stripped.
#define FILERTFT_SUBSUBSECTION	4




//----------------------------------------------------------------------------------
// Copy tree options
//----------------------------------------------------------------------------------
// If a file already exists at the destination, then the function will fail.  Cannot
// be used with any of the other first three options.
#define FILECTO_FAIL_IF_EXISTS			0x01

// Normally files that already exist at the destination and have the same time, size,
// etc. are not copied.  This option overrides that.  Cannot be used with any of the
// other first three options.
#define FILECTO_FORCE_COPY_IF_SAME		0x02

// Erases all existing files in the directories at the destination where files will
// be copied prior to the actual copying.  Note that subdirectories are untouched;
// for that, use FileCleanDirectory.  Cannot be used with any of the other first
// three options.
#define FILECTO_EMPTY_COPIED_DIRS		0x04

// This specifies that even directories where no files are copied are created at the
// destination.  If FILECTO_EMPTY_COPIED_DIRS is specified, these empty directories
// are cleaned of existing files at the destination.
#define FILECTO_COPY_EMPTY_DIRS			0x08


#ifndef NO_FILEUTILS_COPYTREEPROGRESS
// This causes a window with the current file and a total progress bar to be
// displayed while copying.
#define FILECTO_DISPLAY_PROGRESS_BAR	0x10
#endif // ! NO_FILEUTILS_COPYTREEPROGRESS



//==================================================================================
// Structures
//==================================================================================
typedef struct tagFILELINEINFO
{
	// Type of current line, see FILERTFT_xxx above.
	DWORD	dwType;

	// How many lines into the file this one is (note this count includes all lines,
	// even blank ones that may have been skipped, and newline\carraige-returns that
	// were escaped).
	DWORD	dwLineNumber;

	// Current section for line, or NULL if none or not parsed.
	char*	pszCurrentSection;

	// Current subsection for line, or NULL if none or not parsed.
	char*	pszCurrentSubsection;

	// Current subsubsection for line, or NULL if none or not parsed.
	char*	pszCurrentSubsubsection;
} FILELINEINFO, * PFILELINEINFO;



//==================================================================================
// Callback function definitions
//==================================================================================
typedef HRESULT	(__stdcall *PFILEREADLINEPROC)	(char*, PFILELINEINFO, PVOID, BOOL*);





//==================================================================================
// Prototypes
//==================================================================================
DLLEXPORT HRESULT FileReadTextFile(char* szFilePath, DWORD dwOptions,
									PFILEREADLINEPROC pfnReadLine,
									PVOID pvContext);

DLLEXPORT void FileGetNameWithoutExtension(char* szPath, char* szResult, BOOL fIncludeDelimiter);

DLLEXPORT void FileGetExtension(char* szPath, char* szResult, BOOL fIncludeDelimiter);

DLLEXPORT void FileGetLastItemInPath(char* szPath, char* szResult);

DLLEXPORT void FileGetParentDirFromPath(char* szPath, char* szResult);

DLLEXPORT void FileGetDriveFromPath(char* szPath, char* szResult);

DLLEXPORT HRESULT FileCombinePaths(char* szBaseDir, char* szAdditivePath,
								char* pszNewPath, DWORD* pdwNewPathSize);

DLLEXPORT HRESULT FileGetAttributes(char* szItemPath, BOOL* pfIsDirectory,
									BOOL* pfIsReadOnly);
DLLEXPORT HRESULT FileSetAttributes(char* szItemPath, BOOL* pfReadOnly);

DLLEXPORT HRESULT FileGetRealFilePath(char* szItemPath, BOOL fFileAlreadyExists,
									char* pszResultPath, DWORD* pdwResultPathSize);

DLLEXPORT HRESULT FileEnsureDirectoryExists(char* szDirPath);

DLLEXPORT BOOL FileCompareFiles(char* szFirstPath, char* szSecondPath,
								BOOL fCheckCreationTime);


DLLEXPORT HRESULT FileCountFilesInDir(char* szDirPath, BOOL fCountSubdirectories,
									DWORD* lpdwCount);

DLLEXPORT HRESULT FileCopyTree(char* szSourceDirPath, char* szDestDirPath,
							   DWORD dwOptions, PLSTRINGLIST pSkipFiles);

DLLEXPORT HRESULT FileCleanDirectory(char* szDirPath, BOOL fDeleteDirectory,
									BOOL fApplyToSubDirs);


DLLEXPORT HRESULT FileCreateAndOpenFile(char* szFilepath, BOOL fFailIfExists,
										BOOL fAppend, BOOL fInheritableHandle,
										HANDLE* phFile);


DLLEXPORT HRESULT FileWriteString(HANDLE hFile, char* szString);

DLLEXPORT HRESULT FileSprintfWriteString(HANDLE hFile, char* szFormatString,
										DWORD dwNumParms, ...);

DLLEXPORT HRESULT FileWriteLine(HANDLE hFile, char* lpszString);

DLLEXPORT HRESULT FileSprintfWriteLine(HANDLE hFile, char* szFormatString,
										DWORD dwNumParms, ...);

DLLEXPORT HRESULT FileStartTrackingOutputFiles(void);

DLLEXPORT HRESULT FileGetOutputFilesArray(BOOL fIgnoreDuplicates, char** papszArray,
										DWORD* pdwMaxNumEntries);

DLLEXPORT HRESULT FileStopTrackingOutputFiles(void);





#endif // __TNCOMMON_FILEUTILS__
