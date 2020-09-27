//////////////////////////////////////////////////////////////////////
// PATH2.H
//
// Definition of CVCPath and CVCDir objects.
//
// History
// =======
// Date			Who			What
// ----			---			----
// 07-May-93	mattg		Created
// 12-May-93	danw		Add operator = and GetDisplayName
// 20-May-93	mattg		Added CVCDir object
// 22-May-93	danw		Added ConstructObject and DestructObject
//								for collections.
// 11-Jul-93	mattg		Added many new methods to CVCPath and CVCDir
//							Also "TCHAR'ified"
// 20-Jul-93    danw        Added relativization functions.
//////////////////////////////////////////////////////////////////////

#ifndef __PATH2_H__
#define __PATH2_H__

#pragma once

#ifndef _INC_DIRECT
#include <direct.h>
#endif

#ifndef _INC_IO
#include <io.h>
#endif

#ifndef _INC_TCHAR
#include <tchar.h>
#endif

#ifndef _WIN32
#include <ctype.h>
#endif

#ifndef _INC_STAT
#include <sys\stat.h>
#endif

#include "plex.h"
#include <VCFileClasses.h>
#include <VCColl.h>
#include <VCMap.h>
#include <OsUtil.h>

size_t VCRemoveNewlines(_TCHAR *);
size_t VCRemoveNewlines(wchar_t *);
BOOL WINAPI IsUnicodeSystem();
BOOL IsURLW( LPCOLESTR );

// scan a string for specific characters, but ignore anything that is quoted
size_t __cdecl FindWithNoQuotes(const wchar_t * string, const wchar_t * control);

//////////////////////////////////////////////////////////////////////
// Classes defined in this file

class CVCPath;
class CVCDir;

//////////////////////////////////////////////////////////////////////
// Scan a path in see if it contains special charaters that would
// required it to be quoted:
BOOL VCScanPathForSpecialCharacters (const TCHAR *pPath);
//////////////////////////////////////////////////////////////////////
// CVCPath
class CVCPath
{
	//DECLARE_DYNAMIC(CVCPath)

	friend	class		CVCDir;

	friend	static VOID		ConstructElement(CVCPath *);
	friend	static VOID		DestructElement(CVCPath *);

protected:
	// Data
	CVCString			m_strCanon;
	int					m_ichLastSlash;	// used to quickly extract only dir or filename
	BOOL				m_Flags;
	enum				PathFlags
						{
							eIsActualCase = 1,
							eWantsRelative = 2,
						};
		// Canonicalized representation of pathname.
	static CVCMapStringToString c_DirCaseMap;
public:
	// Constructors, destructors, initialization methods
	inline				CVCPath() { m_ichLastSlash = -1; m_Flags = 0;}
	inline				CVCPath(const CVCPath & path)
							 {
								 m_strCanon = path.m_strCanon;
								 m_ichLastSlash = path.m_ichLastSlash;
								 m_Flags = path.m_Flags;
							 }
	virtual				~CVCPath();

	inline	BOOL		GetAlwaysRelative() const { return ((m_Flags & eWantsRelative) != 0); }
	inline	void		SetAlwaysRelative(BOOL bWantsRel = TRUE) { m_Flags =
			(bWantsRel) ? m_Flags | eWantsRelative : m_Flags & ~eWantsRelative;}

	inline	BOOL		IsInit() const { return (m_ichLastSlash > 0); }

	BOOL				Create(const TCHAR *);
		// Initialize the object, given a filename.  The resulting
		// canonicalized filename will be relative to the current
		// directory.  For example, if the current directory is
		// C:\TEST and the argument is "FOO.C", the resulting
		// canonicalized filename will be "C:\TEST\FOO.C".  If the
		// argument is "..\FOO.C", the resulting canonicalized
		// filename will be "C:\FOO.C".

	BOOL				CreateFromDirAndFilename(const CVCDir &, const TCHAR *);
		// Initialize the object given a directory (CVCDir object) and
		// a filename.  This behaves exactly the same as the Create()
		// method, except that the Create() method canonicalizes the
		// filename relative to the CURRENT directory, whereas this
		// method canonicalizes the filename relative to the SPECIFIED
		// directory.

	BOOL				CreateTemporaryName(const CVCDir &, BOOL fKeep = TRUE);
		// Initialize the object given a directory.  The resulting
		// object will represent a UNIQUE filename in that directory.
		// This is useful for creating temporary filenames.
		//
		// WARNING
		// -------
		// After this method returns, the filename represented by this
		// object will EXIST ON DISK as a zero length file.  This is
		// to prevent subsequent calls to this method from returning
		// the same filename (this method checks to make sure it
		// doesn't return the name of an existing file).  IT IS YOUR
		// RESPONSIBILITY to delete the file one way or another.
		//
		// If you don't want this behavior, pass FALSE for 'fKeep',
		// and the file will not exist on disk.  Be aware, though,
		// that if you do this, subsequent calls to this method may
		// return the same filename.

	BOOL				ContainsSpecialCharacters () const
						{
							return ::VCScanPathForSpecialCharacters(m_strCanon);
						}
		// Scan the pathname for special character.  We cache this
		// information.

	inline  CVCPath&	operator =(const CVCPath & path)
						{
							VSASSERT(path.IsInit(), "CVCPath must be initialized before use");
							m_strCanon = path.m_strCanon;
							m_ichLastSlash = path.m_ichLastSlash;
							m_Flags = path.m_Flags;
							return(*this);
						}
		// Assignment operator.

	// Query methods
	inline	const TCHAR * GetFileName() const
						{
							VSASSERT(IsInit(), "CVCPath must be initialized before use");
							VSASSERT(m_ichLastSlash==m_strCanon.ReverseFind('\\'), "Last slash not where we expect it; buffer must have been manipulated directly");
							return ((const TCHAR *)m_strCanon + m_ichLastSlash + 1);
						}

		// Return a pointer to the filename part of the canonicalized
		// pathname, i.e., the filename with no leading drive or path
		// information. Return whole string if no backslash (not init).
		//
		// Please do not write through this pointer, as it is pointing
		// to internal data!

		VOID			PostFixNumber();
			// Modifies the path by postfixing a number on the end of the path's
			// basename. If there is no number on the end of the path's basename
			// then the number 1 is postfixed. Otherwise if there already is a
			// number on the end of the path's basename then that number is
			// incremented by 1 and postfixed on the end of the basename (less the
			// original number).
			//
			// e.g. foo.cpp -> foo1.cpp -> foo2.cpp -> foo3.cpp
		
		VOID			GetBaseNameString(CVCString &) const;
			// Creates a CVCString representing the base name of the fully
			// canonicalized pathname.  For example, the base name of
			// the pathname "C:\FOO\BAR.C" is "BAR".
			//
			// This method can't return a pointer to internal data like
			// some of the other methods since it would have to remove
			// the extension in order to do so.

		VOID  			GetDisplayNameString(
									CVCString &,
									int cchMax = 16,
									BOOL bTakeAllAsDefault = FALSE
									) const;
			// Creates a CVCString representing the name of the file
			// shortened to cchMax CHARACTERS (TCHARs, not bytes) or
			// less.  Only the actual characters are counted; the
			// terminating '\0' is not considered, so
			// CVCString::GetLength() on the result MAY return as much as
			// cchMax.  If cchMax is less than the length of the base
			// filename, the resulting CVCString will be empty, unless
			// bTakeAllAsDefault is TRUE, in which the base name is
			// copied in, regardless of length.
			//
			// As an example, "C:\SOMEDIR\OTHERDIR\SUBDIR\SPECIAL\FOO.C"
			// will be shortened to "C:\...\SPECIAL\FOO.C" if cchMax is 25.

		inline	const TCHAR * GetExtension() const
								{
									VSASSERT(IsInit(), "CVCPath must be initialized before use");
									INT_PTR iDot = m_strCanon.ReverseFind(_T('.'));
 									if (iDot < m_ichLastSlash)
										iDot = m_strCanon.GetLength();
									const TCHAR * retval = ((const TCHAR *)m_strCanon) + iDot;
 									return retval;
								}

			// Return a pointer to the extension part of the canonicalized
			// pathname.  Returns a pointer to the '.' character of the
			// extension.  If the filename doesn't have an extension,
			// the pointer returned will point to the terminating '\0'.
			//
			// Please do not write through this pointer, as it is pointing
			// to internal data!

	inline	const TCHAR * GetFullPath() const { return(m_strCanon); }
			// Return a pointer to the full (canonicalized) pathname.
			//
			// Please do not write through this pointer, as it is pointing
			// to internal data!
	inline	const TCHAR * GetFullPath(CVCString & strPath) const { return(strPath = m_strCanon); }

	inline	BOOL		IsActualCase() const { return ((m_Flags & eIsActualCase)!=0); }
	void				GetActualCase(BOOL bEntirePath = FALSE);
			// Adjusts the paths case to match the actual path and filename
			// on disk.
	void				SetActualCase(LPCTSTR pszFileCase); 
			// Adjusts the paths case to match the actual path and filename
			// on disk, where pszFileCase already contains the correct case
			// for just the filename portion.
	static void			ResetDirMap();

	inline				operator const TCHAR *() const { return(m_strCanon); }
			// Return the fully canonicalized filename as a (const TCHAR *).
			// Same thing as GetFullPath(), but more convenient in some
			// cases.
			//
			// Please do not write through this pointer, as it is pointing
			// to internal data!

	inline	BOOL		IsUNC() const { return(m_strCanon[0] == _T('\\')); }
				// Returns TRUE if the pathname is UNC (e.g.,
				// "\\server\share\file"), FALSE if not.

	inline BOOL			IsEmpty() const { return (m_strCanon.IsEmpty()); }

	// Comparison methods

	int					operator ==(const CVCPath &) const;
		// Returns 1 if the two CPaths are identical, 0 if they are
		// different.

	inline	int			operator !=(const CVCPath & path) const { return(!(operator ==(path))); }
		// Returns 1 if the two CPaths are different, 0 if they are
		// identical.

	// Modification methods

	VOID				ChangeFileName(const TCHAR *);
		// Changes the file name to that specified by the
		// (const TCHAR *) argument.  The directory portion of the
		// pathname remains unchanged.  DO NOT pass in anything
		// other than a simple filename, i.e., do not pass in
		// anything with path modifiers.

	VOID				ChangeExtension(const TCHAR *);
		// Changes the extension of the pathname to be that specified
		// by the (const TCHAR *) argument.  The argument can either be
		// of the form ".EXT" or "EXT".  If the current pathname has
		// no extension, this is equivalent to adding the new extension.

	BOOL 				GetRelativeName (const CVCDir&, CVCString&, BOOL bQuote = FALSE, BOOL bIgnoreAlwaysRelative = FALSE) const;
		// Makes the path name relative to the supplied directory and
		// placed the result in strResult.  Function will only go
		// down from the supplied directy (no ..'s).  Returns TRUE if
		// relativization was successful, or FALSE if not (e.g. if
		// string doesn't start with ".\" or ..\ or at least \).
		//
		// Thus, if the base directory is c:\sushi\vcpp32:
		//
		//  s:\sushi\vcpp32\c\fmake.c => s:\sushi\vcpp32\c\fmake.c
		//  c:\sushi\vcpp32\c\fmake.c => .\fmake.c
		//  c:\dolftool\bin\cl.exe    => \dolftool\bin\cl.exe
		//	\\danwhite\tmp\test.cpp   => \\danwhite\tmp\test.cpp

		// Thus, if the base directory is \\danwhite\c$\sushi\vcpp32:
		//
		// \\danwhite\c$\dolftool\bin\cl.exe => \dolftool\bin\cl.exe
		// \\danwhite\tmp\test.cpp           => \\danwhite\tmp\test.cpp

		// If bQuote is true, then quotes are put around the relative
		// file name. (Useful for writing the filename out to a file)

		// If (!bIgnoreAlwaysRelative && GetAlwaysRelative()) is TRUE
		// and if the file is on the same drive we will ALWAYS
		// relativize it. Thus for the base dir c:\sushi\vcpp32
		//  c:\dolftool\bin\cl.exe    => ..\..\dolftool\bin\cl.exe

	BOOL			   CreateFromDirAndRelative (const CVCDir&, const TCHAR *);
		// THIS FUNCTION IS OBSOLETE.  New code should use
		// CreateFromDirAndFilename().  The only difference between
		// that function and this one is that this one will
		// automatically remove quotes from around the relative
		// path name (if present).


	// Miscellaneous methods
	inline	BOOL		IsReadOnlyOnDisk() const
						{
							HANDLE	h;

							VSASSERT(IsInit(), "CVCPath must be initialized before use");
							h = CreateFile(m_strCanon, GENERIC_WRITE,
								FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);

							if (h == INVALID_HANDLE_VALUE && GetLastError() != ERROR_FILE_NOT_FOUND)
								return TRUE;

							if (h != INVALID_HANDLE_VALUE)
								CloseHandle(h);

							return FALSE;
						}
		// Returns TRUE if the filename represented by this object
		// is read-only on disk, FALSE if not.  NOT guaranteed to
		// work in all circumstances -- for example, will not return
		// TRUE for a file on a floppy drive that has been write-
		// protected.  I don't know of any way to get this information
		// from NT (GetFileAttributes doesn't work; GetVolumeInformation
		// doesn't work; _access just calls GetFileAttributes; etc.).
		// This method WILL correctly detect:
		//		- Files marked as read-only
		//		- Files on read-only network drives

	inline	BOOL		ExistsOnDisk() const
						{
							VSASSERT(IsInit(), "CVCPath must be initialized before use");
							return(_taccess(m_strCanon, 00) != -1);
						}
		// Returns TRUE if the filename represented by this object
		// exists on disk, FALSE if not.

	BOOL		CanCreateOnDisk(BOOL fOverwriteOK = FALSE) const;
		// Returns TRUE if the filename represented by this object
		// can be created on disk, FALSE if not.

	inline	BOOL		DeleteFromDisk() const
						{
							VSASSERT(IsInit(), "CVCPath must be initialized before use");
#ifdef _WIN32
							return(DeleteFile((TCHAR *)(const TCHAR *)m_strCanon));
#else
							return(remove(m_strCanon) != -1);
#endif
						}
		// Removes the file represented by this object from the disk.

	BOOL				GetFileTime(LPFILETIME lpftLastWrite);
	BOOL				GetFileTime(CVCString& rstrLastWrite, DWORD dwFlags = DATE_SHORTDATE);
	// Returns the last modified time, as either an FILETIME struct or a string
};
//	Creation and destruction functions used by CVCMapPathToPtr:

extern const CVCString /*AFX_DATA*/ pthEmptyString;

static inline VOID ConstructElement(CVCPath * pNewData)
{
	memcpy(&pNewData->m_strCanon, &pthEmptyString, sizeof(CVCString));
}

static inline VOID DestructElement(CVCPath * pOldData)
{
	pOldData->m_strCanon.Empty();
}


//	File Name Utility Functions
//		These are redundant and could be replaced with use of CVCPath, but are
//		kept since they are easier to use and already exist in VRES.

// Remove the drive and directory from a file name.
CVCString StripPath(LPCTSTR szFilePath);

// Remove the name part of a file path.  Return just the drive and directory.
CVCString StripName(LPCTSTR szFilePath);

// Get only the extension of a file path.
CVCString GetExtension(LPCTSTR szFilePath);

// Return the path to szFilePath relative to szDirectory.  (e.g. if szFilePath
// is "C:\FOO\BAR\CDR.CAR" and szDirectory is "C:\FOO", then "BAR\CDR.CAR"
// is returned.  This will never use '..'; if szFilePath is not in szDirectory
// or a sub-directory, then szFilePath is returned unchanged.
//
// CVCString GetRelativeName(LPCTSTR szFilePath, LPCTSTR szDirectory = NULL);
CStringW GetRelativeName(LPCOLESTR szFilePath, LPCOLESTR szDirectory = NULL);

// Makes a file path look like in MRU.
CVCString GetDisplayName(LPCTSTR szFilePath, int nMaxDisplayLength,
	LPCTSTR szDirectory = NULL);


//////////////////////////////////////////////////////////////////////
// CVCDir
//
// The CVCDir object represents a file system directory on some disk.
//
// A CVCDir object can be created to represent the current directory,
// to represent the directory of a CVCPath object (i.e., the directory
// in which a file resides), and to represent a temporary directory.
// Note that a CVCDir object CANNOT be created given an arbitrary string --
// this is intentional, since this should not be necessary.
//
// The string representation of a CVCDir object (e.g., operator const TCHAR *())
// MAY or MAY NOT end in '\'.  The root directory of a local drive (e.g., C:)
// will end in '\' ("C:\"), while other directories on a local drive will
// not ("C:\OTHERDIR").  The root directory on a REMOTE drive will NOT end
// in '\' ("\\server\share").  Don't make any assumptions about whether or
// not the string representation ends in '\'.
//
// See also several CVCPath methods which use CVCDir objects.

class CVCDir
{
	//DECLARE_DYNAMIC(CVCDir)

	friend	class		CVCPath;

	friend	static VOID		ConstructElement(CVCDir *);
	friend	static VOID		DestructElement(CVCDir *);

protected:
	CVCString			m_strDir;
		// Directory name, including drive letter or
		// server/share.  Do NOT make any assumptions
		// about whether or not this ends in '\'!

	// Creates multi level directories just fine
	BOOL				MakeDirectory(LPCTSTR lpszPathName) const;
public:
	// Constructors, destructors, initialization methods
	inline				CVCDir() {}
	inline				CVCDir(const CVCDir & dir) { m_strDir = dir.m_strDir; }
	virtual				~CVCDir();
	inline	BOOL		IsInit() { return !m_strDir.IsEmpty(); }

	BOOL				CreateFromCurrent();
		// Initialize from the current working directory.  This
		// may fail if the current working directory is unknown
		// or invalid.

	BOOL				CreateFromPath(const CVCPath &, BOOL bIsAlreadyDirectory = FALSE);
		// Initialize based on the directory of the specified
		// CVCPath object.  That is, if the CVCPath object represents
		// the file "C:\FOO\BAR\BLIX.C", the resulting directory
		// for this object will be "C:\FOO\BAR".  However, you can set bIsAlreadyDirectory
		// to TRUE and get the whole path.  For example, if the CVCPath object
		// represents "C:\FOO\BAR", the resulting directory will stay "C:\FOO\BAR"
		// Returns FALSE on failure.

	BOOL				CreateFromPath(const TCHAR *pszPath, BOOL bIsAlreadyDirectory = FALSE);
		// Initialize based on the directory of the specified
		// CVCPath object.  That is, if the CVCPath object represents
		// the file "C:\FOO\BAR\BLIX.C", the resulting directory
		// for this object will be "C:\FOO\BAR".  However, you can set bIsAlreadyDirectory
		// to TRUE and get the whole path.  For example, if the CVCPath object
		// represents "C:\FOO\BAR", the resulting directory will stay "C:\FOO\BAR"
		// Returns FALSE on failure.

	BOOL				CreateTemporaryName();
		// Initialize this object to represent a temporary directory
		// on disk (e.g., "C:\TMP").

	inline BOOL			CreateFromString(const TCHAR * sz, BOOL bIsAlreadyDirectory = FALSE)
						{
							return  CreateFromStringEx(sz, FALSE, bIsAlreadyDirectory);
						}	
		// Create from a string (e.g., "C:\", "C:\TMP", etc.).  Please
		// do not use this method when another would suffice!

	BOOL				CreateFromStringEx(const TCHAR * sz, BOOL fRootRelative, BOOL bIsAlreadyDirectory = FALSE);
		// Create from a string (e.g., "C:\", "C:\TMP", etc.).  Please
		// do not use this method when another would suffice!
		// same as CreateFromString with minor change. Not treating as bug fix to CFS
		// due to lateness in VC 4.0 project time

		// if fRootRelative true, treat dir ending with colon as relative not root dir 
		// (actual correct handling)


	BOOL				ContainsSpecialCharacters () const
						{
							return ::VCScanPathForSpecialCharacters(m_strDir);
						}
	inline BOOL			IsEmpty() const { return (m_strDir.IsEmpty()); }

		// Scan the pathname for special character.  We cache this information.

	inline	CVCDir&		operator =(const CVCDir & dir)
						{
							m_strDir = dir.m_strDir;
							return(*this);
						}
		// Assignment operator.

	// Query methods

	inline				operator const TCHAR *() const { return(m_strDir); }
				// Return the directory name as a (const TCHAR *) string.

	inline int			GetLength() const { return (int) m_strDir.GetLength(); }
		// Returns the length of the directory name

	// Miscellaneous methods

	BOOL			MakeCurrent() const;
		// Make this object the current working directory.  May fail
		// if the directory no longer exists (e.g., a floppy drive).

	BOOL			ExistsOnDisk() const;
		// Returns TRUE if the directory represented by this object
		// exists on disk, FALSE if not.

	inline	BOOL		CreateOnDisk() const { return MakeDirectory(m_strDir); }
		// Creates the directory on disk.  If this fails, returns
		// FALSE.  If the directory already existed on disk, returns
		// TRUE (i.e., that is not an error condition).

	inline	BOOL		RemoveFromDisk() const { return RemoveDirectory(m_strDir); }
		// Removes the directory from the disk.  If this fails for
		// any reason (directory does not exist, directory is not
		// empty, etc.), returns FALSE.

	BOOL				IsRootDir() const;
		// Returns TRUE if the directory represented by this object
		// is a root directory (e.g., "C:\"), FALSE if not.  Note that
		// calling this method will NOT tell you whether or not the
		// string representation ends in '\', since "\\server\share"
		// is a root directory, and does not end in '\'.

	inline	BOOL		IsUNC() const { return(m_strDir[0] == _T('\\')); }
				// Returns TRUE if this is a UNC directory, FALSE if not.

	VOID				AppendSubdirName(const TCHAR *);
		// Adds a subdirectory name.  For example, if this object
		// currently represents "C:\FOO\BAR", and the argument is
		// "$AUTSAV$", the resulting object represents
		// "C:\FOO\BAR\$AUTSAV$".
		//
		// WARNING: This method does NO validation of the result --
		// it does not check for illegal characters, or for a
		// directory name that is too long.  In particular, don't
		// pass "DIR1/DIR2" as an argument, since no conversion
		// (of '/' to '\') will occur.

	VOID				RemoveLastSubdirName();
		// Removes the last component of the directory name.  For
		// example, if this object currently represents
		// "C:\FOO\BAR\$AUTSAV$", after this method it will
		// represent "C:\FOO\BAR".  If you try to call this method
		// when the object represents a root directory (e.g., "C:\"),
		// it will assert.

	// Comparison methods

	int					operator ==(const CVCDir &) const;
		// Returns 1 if the two CVCDirs are identical, 0 if they are
		// different.

	inline	int			operator !=(const CVCDir & dir) const { return(!(operator ==(dir))); }
		// Returns 1 if the two CVCDirs are different, 0 if they are identical.
};

//	Creation and destruction functions used by CVCMapDirToPtr:

static inline VOID ConstructElement(CVCDir * pNewData)
{
	memcpy(&pNewData->m_strDir, &pthEmptyString, sizeof(CVCString));
}

static inline VOID DestructElement(CVCDir * pOldData)
{
	pOldData->m_strDir.Empty();
}

///////////////////////////////////////////////////////////////////////////////
//	CVCCurDir
//		This class is used to switch the current drive/directory during the
//		life of the object and to restore the previous dirve/directory upon
//		destruction.

class CVCCurDir : CVCDir
{
public:
	CVCCurDir(const char* szPath, BOOL bFile = FALSE);
	CVCCurDir(const CVCDir& dir);
	CVCCurDir();	// just saves the current directory and resets it
	~CVCCurDir();

	CVCDir m_dir;
};

///////////////////////////////////////////////////////////////////////////////
//	CVCFileOpenReturn
//		This class represents the return value from the Common Dialogs
//		File.Open.  It handles both single and multiple select types.
//

class CVCFileOpenReturn
{
	BOOL		m_bSingle;
	BOOL		m_bBufferInUse;
	BOOL		m_bArrayHasChanged;

	INT_PTR		m_cbData;
	_TCHAR * 	m_pchData;

	// Multiple Files
	CVCPtrArray	m_rgszNames;

public:
	CVCFileOpenReturn(const _TCHAR * szRawString = NULL);
	~CVCFileOpenReturn();

	inline BOOL IsSingle() const;
	inline BOOL IsDirty() const;
	inline BOOL BufferOverflow() const;
	//inline int  GetLength() const;

	// GetBuffer gives permission for something else to directly change the buffer
	// ReleaseBuffer signifies that the something else is done with it.
	_TCHAR * GetBuffer(int cbBufferNew);
	inline void ReleaseBuffer ();

	// allows the object to be re-initialized
	void ReInit(const _TCHAR * szRawString);

	// This supports the dynamic file extension update in OnFileNameOK().
	void ChangeExtension(int i, const CVCString& szExt);

	void CopyBuffer(_TCHAR * szTarget);

	// This is the function to use to get at the user's selections,
	// whether single or multiple.
	BOOL GetPathname(int i, CVCString& strPath) const;

private:
	void GenArrayFromBuffer();
	void GenBufferFromArray();
	void ClearNamesArray();
	void SetBuffer(const _TCHAR * szRawString);	
};


inline BOOL CVCFileOpenReturn::IsSingle() const
{
	return m_bSingle;
}

inline BOOL CVCFileOpenReturn::IsDirty() const
{
	return m_bArrayHasChanged;
}

inline BOOL CVCFileOpenReturn::BufferOverflow() const
{
	return m_cbData == 2 && m_pchData[0] == '?';
}

///// ReleaseBuffer - Tell object we're done changing the buffer
//
//	Processes the raw string
//
///
inline void CVCFileOpenReturn::ReleaseBuffer()
{
	m_bBufferInUse = FALSE;
	GenArrayFromBuffer ();
}

///////////////////////////////////////////////////////////////////////////////
//	Smart case helpers.
//		These functions are used to do smart casing of paths and file extensions.

extern BOOL VCGetActualFileCase(CVCString& rFilename, LPCTSTR lpszDir = NULL);
extern LPCTSTR VCGetExtensionCase(LPCTSTR lpszFilename, LPCTSTR lpszExtension);

//extern BOOL VCGetDisplayFile(CVCString &rFilename, DC *pDC, int &cxPels); // truncates from left

///////////////////////////////////////////////////////////////////////////////
// Smart canonicalization helper
extern BOOL VCCanonicalizePathNameW(const wchar_t* szOriginalFile, CStringW& strFixedFile, int& nLastSlashLoc, BOOL bMustBeFile = TRUE);

/////////////////////////////////////////////////////////////////////////////
//	Path collections
//

class CVCPathList
{

	//DECLARE_DYNAMIC(CVCPathList)

protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		CVCPath* data;
	};
public:

// Construction
	CVCPathList(int nBlockSize=10);

// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	CVCPath*& GetHead();
	CVCPath* GetHead() const;
	CVCPath*& GetTail();
	CVCPath* GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list !
	CVCPath* RemoveHead();
	CVCPath* RemoveTail();

	// add before head or after tail
	VCPOSITION AddHead(CVCPath* newElement);
	VCPOSITION AddTail(CVCPath* newElement);

	// add another list of elements before head or after tail
	void AddHead(CVCPathList* pNewList);
	void AddTail(CVCPathList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	VCPOSITION GetHeadPosition() const;
	VCPOSITION GetTailPosition() const;
	CVCPath*& GetNext(VCPOSITION& rPosition); // return *Position++
	CVCPath* GetNext(VCPOSITION& rPosition) const; // return *Position++
	CVCPath*& GetPrev(VCPOSITION& rPosition); // return *Position--
	CVCPath* GetPrev(VCPOSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	CVCPath*& GetAt(VCPOSITION position);
	CVCPath* GetAt(VCPOSITION position) const;
	void SetAt(VCPOSITION pos, CVCPath* newElement);
	void RemoveAt(VCPOSITION position);

	// inserting before or after a given position
	VCPOSITION InsertBefore(VCPOSITION position, CVCPath* newElement);
	VCPOSITION InsertAfter(VCPOSITION position, CVCPath* newElement);

	// helper functions (note: O(n) speed)
	VCPOSITION Find(CVCPath* searchValue, VCPOSITION startAfter = NULL) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	VCPOSITION FindIndex(int nIndex) const;
						// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	struct CVCPlex* m_pBlocks;
	int m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~CVCPathList();
};


/////////////////////////////////////////////////////////////////////////////

class CVCDirList
{

	//DECLARE_DYNAMIC(CVCDirList)

protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		CVCDir* data;
	};
public:

// Construction
	CVCDirList(int nBlockSize=10);

// Attributes (head and tail)
	// count of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// peek at head or tail
	CVCDir*& GetHead();
	CVCDir* GetHead() const;
	CVCDir*& GetTail();
	CVCDir* GetTail() const;

// Operations
	// get head or tail (and remove it) - don't call on empty list !
	CVCDir* RemoveHead();
	CVCDir* RemoveTail();

	// add before head or after tail
	VCPOSITION AddHead(CVCDir* newElement);
	VCPOSITION AddTail(CVCDir* newElement);

	// add another list of elements before head or after tail
	void AddHead(CVCDirList* pNewList);
	void AddTail(CVCDirList* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	VCPOSITION GetHeadPosition() const;
	VCPOSITION GetTailPosition() const;
	CVCDir*& GetNext(VCPOSITION& rPosition); // return *Position++
	CVCDir* GetNext(VCPOSITION& rPosition) const; // return *Position++
	CVCDir*& GetPrev(VCPOSITION& rPosition); // return *Position--
	CVCDir* GetPrev(VCPOSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	CVCDir*& GetAt(VCPOSITION position);
	CVCDir* GetAt(VCPOSITION position) const;
	void SetAt(VCPOSITION pos, CVCDir* newElement);
	void RemoveAt(VCPOSITION position);

	// inserting before or after a given position
	VCPOSITION InsertBefore(VCPOSITION position, CVCDir* newElement);
	VCPOSITION InsertAfter(VCPOSITION position, CVCDir* newElement);

	// helper functions (note: O(n) speed)
	VCPOSITION Find(CVCDir* searchValue, VCPOSITION startAfter = NULL) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	VCPOSITION FindIndex(int nIndex) const;
						// get the 'nIndex'th element (may return NULL)

	// Cool functions to convert to and from a semi-colon separated string
	BOOL FromString(LPCTSTR lpszString, BOOL bCheckExist = FALSE); // Note: first does a remove all
	BOOL ToString(CVCString& str);

// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	struct CVCPlex* m_pBlocks;
	int m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~CVCDirList();
};


/////////////////////////////////////////////////////////////////////////////
class CVCMapPathToPtr	// replaces CVCMapPathToPtr
{
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CVCPath key;
		void* value;
	};
public:

// Construction
	CVCMapPathToPtr(int nBlockSize=10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(const CVCPath& key, void*& rValue) const;

	//	Function returns position of an item.  This is useful if you're trying
	//	to save the address of an item, say for a listbox.  If you save the 
	//	return VCPOSITION, you can get the key back with GetNextAssoc.

	VCPOSITION LookupPos(const CVCPath& key) const
	{
		UINT nHash;
		return (VCPOSITION) GetAssocAt((CVCPath&) key, nHash);
	};

// Operations
	// Lookup and add if not there
	void*& operator[](const CVCPath& key);

	// add a new (key, value) pair
	void SetAt(CVCPath& key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(CVCPath& key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CVCPath& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(CVCPath& key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(CVCPath&, UINT&) const;


public:
	~CVCMapPathToPtr();
};

/////////////////////////////////////////////////////////////////////////////

class CVCMapDirToPtr	// replaces CVCMapDirToPtr
{
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CVCDir key;
		void* value;
	};
public:

// Construction
	CVCMapDirToPtr(int nBlockSize=10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(const CVCDir& key, void*& rValue) const;

	//	Function returns position of an item.  This is useful if you're trying
	//	to save the address of an item, say for a listbox.  If you save the 
	//	return VCPOSITION, you can get the key back with GetNextAssoc.
	VCPOSITION LookupPos(const CVCDir& key) const
	{
		UINT nHash;
		return (VCPOSITION) GetAssocAt((CVCDir&)key, nHash);
	};

// Operations
	// Lookup and add if not there
	void*& operator[](const CVCDir& key);

	// add a new (key, value) pair
	void SetAt(CVCDir& key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(CVCDir& key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CVCDir& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(CVCDir& key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(CVCDir&, UINT&) const;

public:
	~CVCMapDirToPtr();
};

////////////////////////////////////////////////////////////////////////////

inline int CVCPathList::GetCount() const
	{ return m_nCount; }
inline BOOL CVCPathList::IsEmpty() const
	{ return m_nCount == 0; }
inline CVCPath*& CVCPathList::GetHead()
	{ VSASSERT(m_pNodeHead != NULL, "Calling GetHead on empty CVCPathList!");
		return m_pNodeHead->data; }
inline CVCPath* CVCPathList::GetHead() const
	{ VSASSERT(m_pNodeHead != NULL, "Calling GetHead on empty CVCPathList!");
		return m_pNodeHead->data; }
inline CVCPath*& CVCPathList::GetTail()
	{ VSASSERT(m_pNodeTail != NULL, "Calling GetTail on empty CVCPathList!");
		return m_pNodeTail->data; }
inline CVCPath* CVCPathList::GetTail() const
	{ VSASSERT(m_pNodeTail != NULL, "Calling GetTail on empty CVCPathList!");
		return m_pNodeTail->data; }
inline VCPOSITION CVCPathList::GetHeadPosition() const
	{ return (VCPOSITION) m_pNodeHead; }
inline VCPOSITION CVCPathList::GetTailPosition() const
	{ return (VCPOSITION) m_pNodeTail; }
inline CVCPath*& CVCPathList::GetNext(VCPOSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CVCPath* CVCPathList::GetNext(VCPOSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CVCPath*& CVCPathList::GetPrev(VCPOSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CVCPath* CVCPathList::GetPrev(VCPOSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CVCPath*& CVCPathList::GetAt(VCPOSITION position)
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		return pNode->data; }
inline CVCPath* CVCPathList::GetAt(VCPOSITION position) const
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		return pNode->data; }
inline void CVCPathList::SetAt(VCPOSITION pos, CVCPath* newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCPathList");
		pNode->data = newElement; }


////////////////////////////////////////////////////////////////////////////

inline int CVCDirList::GetCount() const
	{ return m_nCount; }
inline BOOL CVCDirList::IsEmpty() const
	{ return m_nCount == 0; }
inline CVCDir*& CVCDirList::GetHead()
	{ VSASSERT(m_pNodeHead != NULL, "Calling GetHead on empty CVCDirList!");
		return m_pNodeHead->data; }
inline CVCDir* CVCDirList::GetHead() const
	{ VSASSERT(m_pNodeHead != NULL, "Calling GetHead on empty CVCDirList!");
		return m_pNodeHead->data; }
inline CVCDir*& CVCDirList::GetTail()
	{ VSASSERT(m_pNodeTail != NULL, "Calling GetTail on empty CVCDirList!");
		return m_pNodeTail->data; }
inline CVCDir* CVCDirList::GetTail() const
	{ VSASSERT(m_pNodeTail != NULL, "Calling GetTail on empty CVCDirList!");
		return m_pNodeTail->data; }
inline VCPOSITION CVCDirList::GetHeadPosition() const
	{ return (VCPOSITION) m_pNodeHead; }
inline VCPOSITION CVCDirList::GetTailPosition() const
	{ return (VCPOSITION) m_pNodeTail; }
inline CVCDir*& CVCDirList::GetNext(VCPOSITION& rPosition) // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CVCDir* CVCDirList::GetNext(VCPOSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		rPosition = (VCPOSITION) pNode->pNext;
		return pNode->data; }
inline CVCDir*& CVCDirList::GetPrev(VCPOSITION& rPosition) // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CVCDir* CVCDirList::GetPrev(VCPOSITION& rPosition) const // return *Position--
	{ CNode* pNode = (CNode*) rPosition;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		rPosition = (VCPOSITION) pNode->pPrev;
		return pNode->data; }
inline CVCDir*& CVCDirList::GetAt(VCPOSITION position)
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		return pNode->data; }
inline CVCDir* CVCDirList::GetAt(VCPOSITION position) const
	{ CNode* pNode = (CNode*) position;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		return pNode->data; }
inline void CVCDirList::SetAt(VCPOSITION pos, CVCDir* newElement)
	{ CNode* pNode = (CNode*) pos;
		VSASSERT(VCIsValidAddress(pNode, sizeof(CNode)), "Bad node in CVCDirList");
		pNode->data = newElement; }


////////////////////////////////////////////////////////////////////////////

inline int CVCMapPathToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapPathToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapPathToPtr::SetAt(CVCPath& key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapPathToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CVCMapPathToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }

////////////////////////////////////////////////////////////////////////////

inline int CVCMapDirToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CVCMapDirToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CVCMapDirToPtr::SetAt(CVCDir& key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CVCMapDirToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CVCMapDirToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////

BOOL ScanPathForSpecialCharactersW (const wchar_t *pPath);
//////////////////////////////////////////////////////////////////////
// CPathW
class CPathW
{
	friend	class		CDirW;
	friend	static VOID		ConstructElement(CPathW *);
	friend	static VOID		DestructElement(CPathW *);

protected:
	// Data
	CStringW			m_strCanon;
	int					m_ichLastSlash;	// used to quickly extract only dir or filename
	BOOL				m_Flags;
	enum				PathFlags
						{
							eIsActualCase = 1,
							eWantsRelative = 2,
						};
	// Canonicalized representation of pathname.
	static CVCMapStringWToStringW c_DirCaseMap;

public:
	// Constructors, destructors, initialization methods
	CPathW()
	{
		m_ichLastSlash = -1;
		m_Flags = 0;
	}
	CPathW(const CPathW & path)
	{
		m_strCanon = path.m_strCanon;
		m_ichLastSlash = path.m_ichLastSlash;
		m_Flags = path.m_Flags;
	}
	virtual	~CPathW();

	BOOL GetAlwaysRelative() const
	{
		return ((m_Flags & eWantsRelative) != 0);
	}
	void SetAlwaysRelative(BOOL bWantsRel = TRUE)
	{
		m_Flags = (bWantsRel) ? m_Flags | eWantsRelative : m_Flags & ~eWantsRelative;
	}

	BOOL IsInit() const { return (m_ichLastSlash > 0); }

	BOOL Create(const wchar_t *);
		// Initialize the object, given a filename.  The resulting
		// canonicalized filename will be relative to the current
		// directory.  For example, if the current directory is
		// C:\TEST and the argument is "FOO.C", the resulting
		// canonicalized filename will be "C:\TEST\FOO.C".  If the
		// argument is "..\FOO.C", the resulting canonicalized
		// filename will be "C:\FOO.C".

	BOOL				CreateFromDirAndFilename(const CDirW &, const wchar_t *);
		// Initialize the object given a directory (CDirW object) and
		// a filename.  This behaves exactly the same as the Create()
		// method, except that the Create() method canonicalizes the
		// filename relative to the CURRENT directory, whereas this
		// method canonicalizes the filename relative to the SPECIFIED
		// directory.

	BOOL				CreateFromKnown(const wchar_t* szPath);
		// Initialize the object given a known, actual case name.
		// Warning: use this method only if you *KNOW* the file name is fully valid
		// AND the correct case

	BOOL				CreateTemporaryName(const CDirW &, BOOL fKeep = TRUE);
		// Initialize the object given a directory.  The resulting
		// object will represent a UNIQUE filename in that directory.
		// This is useful for creating temporary filenames.
		//
		// WARNING
		// -------
		// After this method returns, the filename represented by this
		// object will EXIST ON DISK as a zero length file.  This is
		// to prevent subsequent calls to this method from returning
		// the same filename (this method checks to make sure it
		// doesn't return the name of an existing file).  IT IS YOUR
		// RESPONSIBILITY to delete the file one way or another.
		//
		// If you don't want this behavior, pass FALSE for 'fKeep',
		// and the file will not exist on disk.  Be aware, though,
		// that if you do this, subsequent calls to this method may
		// return the same filename.

	BOOL				ContainsSpecialCharacters () const
						{
							return ::ScanPathForSpecialCharactersW(m_strCanon);
						}
		// Scan the pathname for special character.  We cache this
		// information.

	 CPathW&	operator =(const CPathW & path)
						{
							VSASSERT(path.IsInit(), "CPathW must be initialized before use");
							m_strCanon = path.m_strCanon;
							m_ichLastSlash = path.m_ichLastSlash;
							m_Flags = path.m_Flags;
							return(*this);
						}
		// Assignment operator.

	// Query methods
	const wchar_t * GetFileName() const
						{
							VSASSERT(IsInit(), "CPathW must be initialized before use");
							VSASSERT(m_ichLastSlash==m_strCanon.ReverseFind('\\'), "Last slash not where we expect it; buffer must have been manipulated directly");
							return ((const wchar_t *)m_strCanon + m_ichLastSlash + 1);
						}

		// Return a pointer to the filename part of the canonicalized
		// pathname, i.e., the filename with no leading drive or path
		// information. Return whole string if no backslash (not init).
		//
		// Please do not write through this pointer, as it is pointing
		// to internal data!

		VOID			PostFixNumber();
			// Modifies the path by postfixing a number on the end of the path's
			// basename. If there is no number on the end of the path's basename
			// then the number 1 is postfixed. Otherwise if there already is a
			// number on the end of the path's basename then that number is
			// incremented by 1 and postfixed on the end of the basename (less the
			// original number).
			//
			// e.g. foo.cpp -> foo1.cpp -> foo2.cpp -> foo3.cpp
		
		VOID			GetBaseNameString(CStringW &) const;
			// Creates a CStringW representing the base name of the fully
			// canonicalized pathname.  For example, the base name of
			// the pathname "C:\FOO\BAR.C" is "BAR".
			//
			// This method can't return a pointer to internal data like
			// some of the other methods since it would have to remove
			// the extension in order to do so.

		VOID  			GetDisplayNameString(
									CStringW &,
									int cchMax = 16,
									BOOL bTakeAllAsDefault = FALSE
									) const;
			// Creates a CStringW representing the name of the file
			// shortened to cchMax CHARACTERS (wchar_ts, not bytes) or
			// less.  Only the actual characters are counted; the
			// terminating '\0' is not considered, so
			// CStringW::GetLength() on the result MAY return as much as
			// cchMax.  If cchMax is less than the length of the base
			// filename, the resulting CStringW will be empty, unless
			// bTakeAllAsDefault is TRUE, in which the base name is
			// copied in, regardless of length.
			//
			// As an example, "C:\SOMEDIR\OTHERDIR\SUBDIR\SPECIAL\FOO.C"
			// will be shortened to "C:\...\SPECIAL\FOO.C" if cchMax is 25.

		const wchar_t * GetExtension() const
								{
									VSASSERT(IsInit(), "CPathW must be initialized before use");
									INT_PTR iDot = m_strCanon.ReverseFind(_T('.'));
 									if (iDot < m_ichLastSlash)
										iDot = m_strCanon.GetLength();
									const wchar_t * retval = ((const wchar_t *)m_strCanon) + iDot;
 									return retval;
								}

			// Return a pointer to the extension part of the canonicalized
			// pathname.  Returns a pointer to the '.' character of the
			// extension.  If the filename doesn't have an extension,
			// the pointer returned will point to the terminating '\0'.
			//
			// Please do not write through this pointer, as it is pointing
			// to internal data!

	const wchar_t * GetFullPath() const { return(m_strCanon); }
			// Return a pointer to the full (canonicalized) pathname.
			//
			// Please do not write through this pointer, as it is pointing
			// to internal data!
	const wchar_t * GetFullPath(CStringW & strPath) const { return(strPath = m_strCanon); }

	BOOL		IsActualCase() const { return ((m_Flags & eIsActualCase)!=0); }
	void				GetActualCase(BOOL bEntirePath = FALSE);
			// Adjusts the paths case to match the actual path and filename
			// on disk.
	void				SetActualCase(LPCOLESTR pszFileCase);
			// Adjusts the paths case to match the actual path and filename
			// on disk, where pszFileCase already contains the correct case
			// for just the filename portion.
	static void			ResetDirMap();

				operator const wchar_t *() const { return(m_strCanon); }
			// Return the fully canonicalized filename as a (const wchar_t *).
			// Same thing as GetFullPath(), but more convenient in some
			// cases.
			//
			// Please do not write through this pointer, as it is pointing
			// to internal data!

	BOOL		IsUNC() const { return(m_strCanon[0] == _T('\\')); }
				// Returns TRUE if the pathname is UNC (e.g.,
				// "\\server\share\file"), FALSE if not.

	BOOL			IsEmpty() const { return (m_strCanon.IsEmpty()); }

	// Comparison methods

	int					operator ==(const CPathW &) const;
		// Returns 1 if the two CPaths are identical, 0 if they are
		// different.

	int			operator !=(const CPathW & path) const { return(!(operator ==(path))); }
		// Returns 1 if the two CPaths are different, 0 if they are
		// identical.

	// Modification methods

	VOID				ChangeFileName(const wchar_t *);
		// Changes the file name to that specified by the
		// (const wchar_t *) argument.  The directory portion of the
		// pathname remains unchanged.  DO NOT pass in anything
		// other than a simple filename, i.e., do not pass in
		// anything with path modifiers.

	VOID				ChangeExtension(const wchar_t *);
		// Changes the extension of the pathname to be that specified
		// by the (const wchar_t *) argument.  The argument can either be
		// of the form ".EXT" or "EXT".  If the current pathname has
		// no extension, this is equivalent to adding the new extension.

	BOOL 				GetRelativeName (const CDirW&, CStringW&, BOOL bQuote = FALSE, BOOL bIgnoreAlwaysRelative = FALSE) const;
		// Makes the path name relative to the supplied directory and
		// placed the result in strResult.  Function will only go
		// down from the supplied directy (no ..'s).  Returns TRUE if
		// relativization was successful, or FALSE if not (e.g. if
		// string doesn't start with ".\" or ..\ or at least \).
		//
		// Thus, if the base directory is c:\sushi\vcpp32:
		//
		//  s:\sushi\vcpp32\c\fmake.c => s:\sushi\vcpp32\c\fmake.c
		//  c:\sushi\vcpp32\c\fmake.c => .\fmake.c
		//  c:\dolftool\bin\cl.exe    => \dolftool\bin\cl.exe
		//	\\danwhite\tmp\test.cpp   => \\danwhite\tmp\test.cpp

		// Thus, if the base directory is \\danwhite\c$\sushi\vcpp32:
		//
		// \\danwhite\c$\dolftool\bin\cl.exe => \dolftool\bin\cl.exe
		// \\danwhite\tmp\test.cpp           => \\danwhite\tmp\test.cpp

		// If bQuote is true, then quotes are put around the relative
		// file name. (Useful for writing the filename out to a file)

		// If (!bIgnoreAlwaysRelative && GetAlwaysRelative()) is TRUE
		// and if the file is on the same drive we will ALWAYS
		// relativize it. Thus for the base dir c:\sushi\vcpp32
		//  c:\dolftool\bin\cl.exe    => ..\..\dolftool\bin\cl.exe

	BOOL			   CreateFromDirAndRelative (const CDirW&, const wchar_t *);
		// THIS FUNCTION IS OBSOLETE.  New code should use
		// CreateFromDirAndFilename().  The only difference between
		// that function and this one is that this one will
		// automatically remove quotes from around the relative
		// path name (if present).


	// Miscellaneous methods
	BOOL		IsReadOnlyOnDisk() const;
		// Returns TRUE if the filename represented by this object
		// is read-only on disk, FALSE if not.  NOT guaranteed to
		// work in all circumstances -- for example, will not return
		// TRUE for a file on a floppy drive that has been write-
		// protected.  I don't know of any way to get this information
		// from NT (GetFileAttributes doesn't work; GetVolumeInformation
		// doesn't work; _access just calls GetFileAttributes; etc.).
		// This method WILL correctly detect:
		//		- Files marked as read-only
		//		- Files on read-only network drives

	BOOL		ExistsOnDisk() const;
		// Returns TRUE if the filename represented by this object
		// exists on disk, FALSE if not.

	BOOL		CanCreateOnDisk(BOOL fOverwriteOK = FALSE) const;
		// Returns TRUE if the filename represented by this object
		// can be created on disk, FALSE if not.

	BOOL		CreateOnDisk(BOOL bMakeNew = FALSE) const;
		// Returns TRUE if the filename represented by this object has been created on disk as a 
		// result of this call, FALSE if not.  If bMakeNew is true, then requires that the file 
		// not previously exist.

	BOOL		DeleteFromDisk() const;
		// Removes the file represented by this object from the disk.

	BOOL				GetFileTime(LPFILETIME lpftLastWrite);
	BOOL				GetFileTime(CStringW& rstrLastWrite, DWORD dwFlags = DATE_SHORTDATE);
	// Returns the last modified time, as either an FILETIME struct or a string

	static BOOL	IsReservedOSName(LPCOLESTR szName);
	
	void				GetClassicOSPath(CStringW& strPath);
		// gets the classic 8.3 format of the file path associated with this CPathW object
};


//////////////////////////////////////////////////////////////////////
// CDirW
//
// The CDirW object represents a file system directory on some disk.
//
// A CDirW object can be created to represent the current directory,
// to represent the directory of a CVCPath object (i.e., the directory
// in which a file resides), and to represent a temporary directory.
// Note that a CDirW object CANNOT be created given an arbitrary string --
// this is intentional, since this should not be necessary.
//
// The string representation of a CDirW object (e.g., operator const TCHAR *())
// MAY or MAY NOT end in '\'.  The root directory of a local drive (e.g., C:)
// will end in '\' ("C:\"), while other directories on a local drive will
// not ("C:\OTHERDIR").  The root directory on a REMOTE drive will NOT end
// in '\' ("\\server\share").  Don't make any assumptions about whether or
// not the string representation ends in '\'.
//
// See also several CVCPath methods which use CDirW objects.

class CDirW
{
	friend	class		CPathW;
	friend	static VOID		ConstructElement(CDirW *);
	friend	static VOID		DestructElement(CDirW *);

protected:
	CStringW			m_strDir;
		// Directory name, including drive letter or
		// server/share.  Do NOT make any assumptions
		// about whether or not this ends in '\'!

	// Creates multi level directories just fine
	BOOL				MakeDirectory(LPCOLESTR lpszPathName) const;
public:
	// Constructors, destructors, initialization methods
	inline				CDirW() {}
	inline				CDirW(const CDirW & dir) { m_strDir = dir.m_strDir; }
	virtual				~CDirW();
	inline	BOOL		IsInit() { return !m_strDir.IsEmpty(); }

	BOOL				CreateFromKnown(const wchar_t* szPath);
		// Initialize based on a known string.  WARNING: only use this method if you KNOW that what you're
		// passing in is valid!

	BOOL				CreateFromCurrent();
		// Initialize from the current working directory.  This
		// may fail if the current working directory is unknown
		// or invalid.

	BOOL				CreateFromPath(const CPathW &, BOOL bIsAlreadyDirectory = FALSE);
		// Initialize based on the directory of the specified
		// CPathW object.  That is, if the CPathW object represents
		// the file "C:\FOO\BAR\BLIX.C", the resulting directory
		// for this object will be "C:\FOO\BAR".  However, you can set bIsAlreadyDirectory
		// to TRUE and get the whole path.  For example, if the CPathW object
		// represents "C:\FOO\BAR", the resulting directory will stay "C:\FOO\BAR"
		// Returns FALSE on failure.

	BOOL				CreateFromPath(const wchar_t *pszPath, BOOL bIsAlreadyDirectory = FALSE, BOOL bMakeActualCase = FALSE);
		// Initialize based on the directory of the specified
		// CPathW object.  That is, if the CPathW object represents
		// the file "C:\FOO\BAR\BLIX.C", the resulting directory
		// for this object will be "C:\FOO\BAR".  However, you can set bIsAlreadyDirectory
		// to TRUE and get the whole path.  For example, if the CPathW object
		// represents "C:\FOO\BAR", the resulting directory will stay "C:\FOO\BAR"
		// Returns FALSE on failure.

	BOOL				CreateTemporaryName();
		// Initialize this object to represent a temporary directory
		// on disk (e.g., "C:\TMP").

	inline BOOL			CreateFromString(const wchar_t * sz, BOOL bIsAlreadyDirectory = FALSE)
						{
							return  CreateFromStringEx(sz, FALSE, bIsAlreadyDirectory);
						}	
		// Create from a string (e.g., "C:\", "C:\TMP", etc.).  Please
		// do not use this method when another would suffice!

	BOOL				CreateFromStringEx(const wchar_t * sz, BOOL fRootRelative, BOOL bIsAlreadyDirectory = FALSE);
		// Create from a string (e.g., "C:\", "C:\TMP", etc.).  Please
		// do not use this method when another would suffice!
		// same as CreateFromString with minor change. Not treating as bug fix to CFS
		// due to lateness in VC 4.0 project time

		// if fRootRelative true, treat dir ending with colon as relative not root dir 
		// (actual correct handling)


	BOOL				ContainsSpecialCharacters () const
						{
							return ::ScanPathForSpecialCharactersW(m_strDir);
						}
	inline BOOL			IsEmpty() const { return (m_strDir.IsEmpty()); }

		// Scan the pathname for special character.  We cache this information.

	inline	CDirW&		operator =(const CDirW & dir)
						{
							m_strDir = dir.m_strDir;
							return(*this);
						}
		// Assignment operator.

	// Query methods

	inline				operator const wchar_t *() const { return(m_strDir); }
				// Return the directory name as a (const wchar_t *) string.

	inline int			GetLength() const { return (int)m_strDir.GetLength(); }
		// Returns the length of the directory name

	// Miscellaneous methods

	BOOL				MakeCurrent() const;
		// Make this object the current working directory.  May fail
		// if the directory no longer exists (e.g., a floppy drive).

	BOOL		ExistsOnDisk() const;
		// Returns TRUE if the directory represented by this object
		// exists on disk, FALSE if not.

	inline	BOOL		CreateOnDisk() const { return MakeDirectory(m_strDir); }
		// Creates the directory on disk.  If this fails, returns
		// FALSE.  If the directory already existed on disk, returns
		// TRUE (i.e., that is not an error condition).

	BOOL		RemoveFromDisk() const;
		// Removes the directory from the disk.  If this fails for
		// any reason (directory does not exist, directory is not
		// empty, etc.), returns FALSE.

	BOOL				IsRootDir() const;
		// Returns TRUE if the directory represented by this object
		// is a root directory (e.g., "C:\"), FALSE if not.  Note that
		// calling this method will NOT tell you whether or not the
		// string representation ends in '\', since "\\server\share"
		// is a root directory, and does not end in '\'.

	inline	BOOL		IsUNC() const { return (m_strDir[0] == L'\\'); }
				// Returns TRUE if this is a UNC directory, FALSE if not.

	VOID				AppendSubdirName(const wchar_t *);
		// Adds a subdirectory name.  For example, if this object
		// currently represents "C:\FOO\BAR", and the argument is
		// "$AUTSAV$", the resulting object represents
		// "C:\FOO\BAR\$AUTSAV$".
		//
		// WARNING: This method does NO validation of the result --
		// it does not check for illegal characters, or for a
		// directory name that is too long.  In particular, don't
		// pass "DIR1/DIR2" as an argument, since no conversion
		// (of '/' to '\') will occur.

	VOID				RemoveLastSubdirName();
		// Removes the last component of the directory name.  For
		// example, if this object currently represents
		// "C:\FOO\BAR\$AUTSAV$", after this method it will
		// represent "C:\FOO\BAR".  If you try to call this method
		// when the object represents a root directory (e.g., "C:\"),
		// it will assert.

	// Comparison methods

	int					operator ==(const CDirW &) const;
		// Returns 1 if the two CDirWs are identical, 0 if they are
		// different.

	inline	int			operator !=(const CDirW & dir) const { return(!(operator ==(dir))); }
		// Returns 1 if the two CDirWs are different, 0 if they are identical.
};

///////////////////////////////////////////////////////////////////////////////
//	CCurDirW
//		This class is used to switch the current drive/directory during the
//		life of the object and to restore the previous dirve/directory upon
//		destruction.

class CCurDirW : CDirW
{
public:
	CCurDirW(const wchar_t* szPath, BOOL bFile = FALSE);
	CCurDirW(const CDirW& dir);
	CCurDirW();	// just saves the current directory and resets it
	~CCurDirW();

	CDirW m_dir;
};

/////////////////////////////////////////////////////////////////////////////
class CMapPathWToPtr	// replaces CMapPathToPtr
{
protected:
	// Association
	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		CPathW key;
		void* value;
	};
public:

// Construction
	CMapPathWToPtr(int nBlockSize=10);

// Attributes
	// number of elements
	int GetCount() const;
	BOOL IsEmpty() const;

	// Lookup
	BOOL Lookup(const CPathW& key, void*& rValue) const;

	//	Function returns position of an item.  This is useful if you're trying
	//	to save the address of an item, say for a listbox.  If you save the 
	//	return VCPOSITION, you can get the key back with GetNextAssoc.

	VCPOSITION LookupPos(const CPathW& key) const
	{
		UINT nHash;
		return (VCPOSITION) GetAssocAt((CPathW&) key, nHash);
	};

// Operations
	// Lookup and add if not there
	void*& operator[](const CPathW& key);

	// add a new (key, value) pair
	void SetAt(CPathW& key, void* newValue);

	// removing existing (key, ?) pair
	BOOL RemoveKey(CPathW& key);
	void RemoveAll();

	// iterating all (key, value) pairs
	VCPOSITION GetStartPosition() const;
	void GetNextAssoc(VCPOSITION& rNextPosition, CPathW& rKey, void*& rValue) const;

	// advanced features for derived classes
	UINT GetHashTableSize() const;
	void InitHashTable(UINT hashSize);

// Overridables: special non-virtual (see map implementation for details)
	// Routine used to user-provided hash keys
	UINT HashKey(CPathW& key) const;

// Implementation
protected:
	CAssoc** m_pHashTable;
	UINT m_nHashTableSize;
	int m_nCount;
	CAssoc* m_pFreeList;
	struct CVCPlex* m_pBlocks;
	int m_nBlockSize;

	CAssoc* NewAssoc();
	void FreeAssoc(CAssoc*);
	CAssoc* GetAssocAt(CPathW&, UINT&) const;


public:
	~CMapPathWToPtr();
};

//	Creation and destruction functions used by CMapPathWToPtr:
extern const CStringW pthEmptyStringW;

static inline VOID ConstructElement(CPathW * pNewData)
{
	memcpy(&pNewData->m_strCanon, &pthEmptyStringW, sizeof(CStringW));
}

static inline VOID DestructElement(CPathW * pOldData)
{
	pOldData->m_strCanon.Empty();
}

////////////////////////////////////////////////////////////////////////////

inline int CMapPathWToPtr::GetCount() const
	{ return m_nCount; }
inline BOOL CMapPathWToPtr::IsEmpty() const
	{ return m_nCount == 0; }
inline void CMapPathWToPtr::SetAt(CPathW& key, void* newValue)
	{ (*this)[key] = newValue; }
inline VCPOSITION CMapPathWToPtr::GetStartPosition() const
	{ return (m_nCount == 0) ? NULL : VCBEFORE_START_POSITION; }
inline UINT CMapPathWToPtr::GetHashTableSize() const
	{ return m_nHashTableSize; }

#endif // __PATH2_H__
