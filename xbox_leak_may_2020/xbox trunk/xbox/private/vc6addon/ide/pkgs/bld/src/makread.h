//////////////////////////////////////////////////////////////////////
// MAKREAD.H
//
// Definition of CMakFileReader and CMakFileWriter objects.
//
// History
// =======
// Date			Who			What
// ----			---			----
// 30-May-93	mattg		Created
// 07-Jun-93	mattg		Added to VSHELL
// 21-Jul-93    v-danwh     Added CNameMunger and CMakFileWriter
//
// Description
// ===========
// The CMakFileReader object will read in a .MAK file which has been
// written out by the IDE.  It accepts a subset of the NMAKE syntax.
//
// The CMakFileReader will read:
//
// - Blank lines
//		- Ignored
// - Lines containing only whitespace
//		- Ignored
// - Lines containing only a comment, possibly with leading whitespace
//		- Returns text of comment (remainder of line)
// - Macro definitions
//		- Returns name and value
//		- Handles value continuation with trailing '\'
//		- Does NOT handle any other special characters
//		- Does NOT check for invalid characters in macro name
// - Directives
//		- Recognizes all types of directives
//		- Returns type of directive
//		- Returns remainder of directive line
//			- CMakFileReader user is responsible for interpretation
// - Description blocks
//		- Accepts :: dependency lines (although no difference in handling)
//		- Returns left side (targets), right side (deps) and commands
//		- Handles deps continuation with trailing '\'
//		- Handles inline file specifications
//		- Does NOT handle any other special characters
//////////////////////////////////////////////////////////////////////

#ifndef __MAKREAD_H__
#define __MAKREAD_H__

#ifndef _SUSHI_PROJECT
// If you are encluding this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

#ifndef __UTIL_H__
#include "util.h"	// CMakFileWriter uses CFileSaver class
#endif

// standard include

//////////////////////////////////////////////////////////////////////
// Classes defined in this file

// CObject
	class CMakComment;
	class CMakMacro;
	class CMakDescBlk;
	class CMakDirective;
	class CMakError;
	class CMakEndOfFile;
	class CMakFileReader;
	class CMakFileWriter;
	class CNameMunger;
//////////////////////////////////////////////////////////////////////
// Helper routines

extern VOID		StripLeadingWhite(CString &);
	// Strip leading whitespace from the referenced string IN PLACE
	// (i.e., the string passed in is altered).  Whitespace is considered
	// to consist of spaces and tabs only.

extern VOID		StripTrailingWhite(CString &);
	// Strip trailing whitespace from the referenced string IN PLACE
	// (i.e., the string passed in is altered).  Whitespace is considered
	// to consist of spaces and tabs only.

extern VOID		StripLeadingAndTrailingWhite(CString &);
	// Strip leading AND trailing whitespace from the referenced string
	// IN PLACE (i.e., the string passed in is altered).  Whitespace is
	// considered to consist of spaces and tabs only.

extern VOID		SkipWhite(const TCHAR * &);
	// Skips to the next non-white character, where whitespace considered
	// to consist of tabs and spaces only.  On entry, the pointer points
	// to the first character to test; on exit, it will point to the first
	// non-white character (including possibly the '\0' string terminator).
extern VOID		SkipNonWhite(const TCHAR * &);
	// Opposite of above

extern int  GetMakToken(const TCHAR * pBase, const TCHAR *& pTokenEnd);
	// Fast forward to the end of the next token in the string at pBase.
	// Returns the number of _characters_ in the token.  Tokens are 
	// are delimetered by whitespace unless inside double quotes.  Quotes
	// Routine does not account for quotes inside quotes.

extern BOOL GetQuotedString(const TCHAR *& pBase, const TCHAR *& pEnd); 
	// Look through the string to find a quoted substring. On return
	// pBase points to first char after first pEnd points to last quote.
	// Return FALSE if < 2 quotes or 0 length substring.

//////////////////////////////////////////////////////////////////////
// CNameMunger class
//
// Takes names containing possibly nasty characters and produces
// a nice name (i.e. suitable as an NMAKE macro name) guranteed not
// to collide with any other names it produced/
class CNameMunger : public CObject
{

public:
	CNameMunger() { m_InMap.InitHashTable(179); m_ResMap.InitHashTable(197); }
	void MungeName (const TCHAR *, CString &);
private:

	// Special subclass of CMapStringToPtr  that returns a poitner
	// to the stored value when it sets a new element:
	class CMungeMapStringToPtr : public CMapStringToPtr
	{
	public:
		// Okay, the way this works is that the usua CMapXXXtoXXX
		// [] oparator returns a pointer to the key in the new
		// CAssoc it has created.  We calcualte the offset between
		// the key and the value and use that to get the address
		// of the value itself.  Yes, this is a GIGA-hack, but the
		// CSlob property bag stuff uses the same principle.  So sue me.
		 
		inline CString *GetStringAddressFromPtr ( void *& rvoid )
		{
		return (CString *) ( ( (BYTE *) &rvoid) - 
							 ( (BYTE *) &((CAssoc *) 0)->value )
	 						+( (BYTE *) &((CAssoc *) 0)->key   ) );
		}
		inline CString *SetAtAndReturnStringAddress (	
										const char* key 
										)
		{
		return GetStringAddressFromPtr ((*this)[key]);
		}

	};

	// We store the previosly seen names in a map which maps 
	// strings to pointers to void.  The void pointers are actually
	// pointers to CStrings which are values in anohter map which
	// contains Munged names we've previously used.  This allows
	// to quickly see if there is already a munged name for an
	// input string, and to see if a possible munged name has already
	// been used:
	CMapStringToPtr m_InMap;
	CMungeMapStringToPtr m_ResMap;

};

// CMak* builder component classes are declared here
#include "bldrcvtr.h"

//////////////////////////////////////////////////////////////////////
//
// Attributes of reading and writing of makefiles
#define MakRW_ShowStatus	0x1
#define MakR_IgnLineCont	0x2
#define MakW_Makefile		0x4
#define MakW_Depfile		0x8
#define MakRW_Default		MakRW_ShowStatus

//////////////////////////////////////////////////////////////////////
// CMakFileReader

#ifdef _UNICODE
#define MAK_EOF		((TCHAR)0xFFFF)
#else
#define MAK_EOF		((TCHAR)0xFF)
#endif

// Line prefix for indented lines ( e.g. "    " )
extern const TCHAR *szMkLineIndent;
extern const TCHAR cMkContinuation;

class BLD_IFACE CMakFileReader : public CObject
{
private:
	// Local types

	enum { CCH_BUF = 512 };
	// Data

			CFile		m_file;
				// Current open file.
			DWORD		m_nFileSize;

			UINT		m_nLineNum;
				// Current line number.

			TCHAR		m_rgchBuf[CCH_BUF];
				// Buffer for buffered I/O.

			UINT		m_ichBuf;
				// Index of next TCHAR in m_rgchBuf[].

			BOOL		m_fEOF;
				// TRUE if end-of-file has been reached, FALSE if not.

			UINT		m_cchFile;
				// Our count of chars through the file so far

			CObject	*	m_pUngotElement;

			int			m_nObjectsRead;

  			UINT		m_iAttrib;	// attributes of makefile reading

			UINT		m_nProgressGoal;
			static const int nStatusBarUpdate;

	// Read/write methods

			VOID		FillBuf();
				// Fills the buffer with up to CCH_BUF - 1 TCHARs from
				// the file.
				//
				// throw(CFileException)

	inline	TCHAR		GetChar()
						{
							if (m_rgchBuf[m_ichBuf] == _T('\0'))
								FillBuf();

							return(m_rgchBuf[m_ichBuf++]);
						}
				// Returns the next TCHAR.  If the TCHAR MAK_EOF is returned,
				// check m_fEOF to ensure an end-of-file condition.
				//
				// throw(CFileException)

	inline	TCHAR		PeekChar()
						{
							if (m_rgchBuf[m_ichBuf] == _T('\0'))
								FillBuf();

							return(m_rgchBuf[m_ichBuf]);
						}
				// Returns the next TCHAR without incrementing the index (i.e.,
				// a subsequent call to GetChar() will return the same TCHAR).
				// If the TCHAR MAK_EOF is returned, check m_fEOF to ensure an
				// end-of-file condition.
				//
				// throw(CFileException)

			BOOL		GetLine(CString &);
				// Fetches the next line into the referenced string.
				//
				// The resulting string does NOT have a trailing
				// carriage-return + linefeed pair.
				//
				// Returns TRUE if successful, FALSE at end-of-file.
				//
				// throw(CFileException)

			CObject *	ParseDirective(const CString &);
				// Parse the specified line as a directive and return
				// one of the following objects:
				//
				// CMakDirective
				// CMakError
				//
				// It is the caller's responsibility to free the object
				// returned.

			CObject *	ParseMacro(const CString &, int);
				// Parse the specified line as a macro definition and
				// return one of the following objects:
				//
				// CMakMacro
				// CMakError
				//
				// The INT parameter specifies the TCHAR offset within
				// the string of the '=' character which was found.
				//
				// It is the caller's responsibility to free the object
				// returned.
				//
				// throw(CFileException)

			CObject *	ParseDescBlk(const CString &, int);
				// Parse the specified line as the dependency line of a
				// description block and return one of the following
				// objects:
				//
				// CMakDescBlk
				// CMakError
				// CMakEndOfFile
				//
				// The INT parameter specifies the TCHAR offset within
				// the string of the ':' character which was found.
				//
				// It is the caller's responsibility to free the object
				// returned.
				//
				// throw(CFileException)

public:
	// Constructors, destructors

						CMakFileReader();
	virtual				~CMakFileReader();

	// Open and close methods

			BOOL		Open(
							 const TCHAR *, 
							 UINT, 
							 CFileException *,
							 UINT iAttrib = MakRW_Default
							);
				// Open the .MAK file for reading.  The first parameter
				// specifies the filename, and the second parameter specifies
				// the access mode as per the CFile constructor (e.g.,
				// CFile::modeRead).  The third parameter is a pointer to an
				// existing CFileException object, whose contents are meaningful
				// after the call only if this method returns FALSE.

				// Re-open the .MAK file for reading, ie. start reading from
				// beginning of file.
			void		ReOpen();

				// Attributes of makefile reading.
	__inline void		SetAttributes(UINT iAttrib) {m_iAttrib = iAttrib;}

				// Return the current line being read.
	__inline UINT		GetCurrentLine() {return m_nLineNum;}

				//  Abort after an exception:
			void		Abort(); 

			VOID		Close();

			CObject *	GetNextElement();
				// Get the next element of the .MAK file.  Returns one of:
				//
				// CMakComment
				// CMakMacro
				// CMakDescBlk
				// CMakDirective
				// CMakError
				// CMakEndOfFile
				//
				// cast to a CObject.  It is the caller's responbility to
				// free the resulting object.
				//
				// throw(CFileException)
			void UngetElement (CObject *);
				// Return an element to the read so that it will be 
				// used for the GetNextElementCall.  Depth is limited
				// to one.
};

//////////////////////////////////////////////////////////////////////
// CMakFileWriter

class BLD_IFACE CMakFileWriter : public CObject
{
private:
			int				m_nCharCount;

			CFile			m_File;
			CFileSaver *	m_pFileSaver;

			// Set this small for testing so we see all the buffer behavior,
			// large for release:
			#ifdef _DEBUG
			enum { CCH_BUF = 16 };  
			#else
			enum { CCH_BUF = 1024 };  
			#endif

			BYTE			m_buf[CCH_BUF];

			int 			m_CurrentPos;
				// Position within the file buffer.
			static const int nLineLen;
				// Desired maxmimum line length when writing out make file.
				// (Since we won't break up long items, we may go over this)

			BOOL	m_bLastLineBlank;

			UINT	m_iAttrib;	// attributes of makefile writing

			UINT		m_nProgressGoal;
			static const int nStatusBarUpdate;

			int		m_nObjects;
			int		m_nObjectsWritten;
			BOOL		m_bInit;

public:
	// Constructors, destructors

						CMakFileWriter();
	virtual				~CMakFileWriter();

	
	// Open and close methods
			BOOL		Create(
							   const TCHAR *,
							   CFileException *,
							   int = 0,
							   UINT iAttrib = MakRW_Default
							  );
			// The third paramter is the number of object we intend to
			// write out, and is used to upadate the status bar.

			// attributes of makefile writing
	__inline void		SetAttributes(UINT iAttrib) {m_iAttrib = iAttrib;}
	__inline BOOL	IsMakefile(void) { return ((m_iAttrib & MakW_Makefile)!=0); }
	__inline BOOL	IsDepFile(void) { return ((m_iAttrib & MakW_Depfile)!=0); }
	__inline BOOL	IsProjectFile(void) { return ((m_iAttrib & (MakW_Depfile|MakW_Makefile))==0); }
	__inline BOOL	IsInit(void) { return m_bInit; }

			const TCHAR* GetFileName();
			// Returns the name of the file that this MakeFile Writer
			// is writing to

			void		WroteObject ();
			// Call when your through writing an object.  Used to update
			// the status bar.

			//  Abort after an exception:
			void		Abort(); 
			//	Open the supplied for for writing, and write our header
			//  into it.
	 		BOOL		Close();
				// Close the .MAK file.

	// Basic output methods:
			void		WriteString ( const TCHAR * );
				// throw(CFileException)
				// Writes a string into the make file.  If the string
				// would make the line longer that nLineLen, we'll try
				// to break it in some reasonable way: Between words that
				// are not in quotes.

			void		WriteStringEx ( const TCHAR * );
				// throw(CFileException)
				// Writes a string into the make file.  No special formatting
				// is performed, making this very similar to WriteBytes
				// except it updates the various member variables properly.

			void 		WriteBytes ( const BYTE *pByte, int nBytes );
				// This should be treated as Private and not used unless
				// absolutely necessary.  Use WriteStringEx() instead.

			void 		EndLine ();
				// throw(CFileException)
				// Start a new line.				
		
			void 		InsureSpace ();
				// Starts a new line _unless_ the last line was blank.
				// Use to insure nive spacing.
			void 		SplitLine ();
				// throw(CFileException)
				// Write configuration character and start new line.				
			void 		Indent ();
				// Indent from begging of line (should only be called 
				// at start of line).

			void 		WriteSeperatorString ();
				// Writes a line of #'s
				 
	// High level output:

			void 		WriteComment (const TCHAR *pszFormat);
			void 		WriteCommentEx (const TCHAR *pszFormat, ...);
				//	Note: comment lines are not broken up to fit in nLineLen.

			void 		WriteDirective (CMakDirective::DTYP, CString &str );
			void 		WriteDirective (CMakDirective::DTYP, const TCHAR *pszText);
			void		WriteMacro (const TCHAR *pszLeftSide, const TCHAR *pszRightSide);
			void		WriteDesc (const TCHAR *pszTargets, const TCHAR *pszDependency);
			void		WriteDescAndAction
						(
							const TCHAR *pszTargets,
							const TCHAR *pszDependency,
							const TCHAR *pszActions
						);

};				// throw(CFileException)

#endif // __MAKREAD_H__

