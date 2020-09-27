//////////////////////////////////////////////////////////////////////
// PATH.H
//
// Definition of CPath and CDir objects.
//
// History
// =======
// Date         Who         What
// ----         ---         ----
// 07-May-93    mattg       Created
// 12-May-93    danw        Add operator = and GetDisplayName
// 20-May-93    mattg       Added CDir object
// 22-May-93    danw        Added ConstructObject and DestructObject
//                              for collections.
// 11-Jul-93    mattg       Added many new methods to CPath and CDir
//                          Also "TCHAR'ified"
// 20-Jul-93    danw        Added relativization functions.
//////////////////////////////////////////////////////////////////////

#ifndef __PATH_H__
#define __PATH_H__

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

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

size_t RemoveNewlines(_TCHAR *);


//////////////////////////////////////////////////////////////////////
// Classes defined in this file

// CObject
    class CPath;
    class CDir;
//////////////////////////////////////////////////////////////////////
// Scan a path in see if it contains special charaters that would
// required it to be quoted:
BOOL ScanPathForSpecialCharacters (const TCHAR *pPath);
//////////////////////////////////////////////////////////////////////
// CPath
class CPath : public CObject
{
    DECLARE_DYNAMIC(CPath)

    friend  class       CDir;

    friend  static VOID     ConstructElement(CPath *);
    friend  static VOID     DestructElement(CPath *);

protected:
    // Data
            CString     m_strCanon;
            int     m_ichLastSlash; // used to quickly extract only dir or filename
            BOOL        m_Flags;
            enum    PathFlags
                {
                    eIsActualCase = 1,
                    eWantsRelative = 2,
                };
                // Canonicalized representation of pathname.
            static CMapStringToString c_DirCaseMap;
public:
    // Constructors, destructors, initialization methods
    inline              CPath() { m_ichLastSlash = -1; m_Flags = 0;}
    inline              CPath(const CPath & path)
                             {
                                 m_strCanon = path.m_strCanon;
                                 m_ichLastSlash = path.m_ichLastSlash;
                                 m_Flags = path.m_Flags;
                             }
    virtual             ~CPath();

    inline  BOOL        GetAlwaysRelative() const { return ((m_Flags & eWantsRelative) != 0); }
    inline  void        SetAlwaysRelative(BOOL bWantsRel = TRUE) { m_Flags =
            (bWantsRel) ? m_Flags | eWantsRelative : m_Flags & ~eWantsRelative;}

    inline  BOOL        IsInit() const { VSASSERT(this!=NULL, "CPath object not initialized"); return (m_ichLastSlash > 0); }

            BOOL        Create(const TCHAR *);
                // Initialize the object, given a filename.  The resulting
                // canonicalized filename will be relative to the current
                // directory.  For example, if the current directory is
                // C:\TEST and the argument is "FOO.C", the resulting
                // canonicalized filename will be "C:\TEST\FOO.C".  If the
                // argument is "..\FOO.C", the resulting canonicalized
                // filename will be "C:\FOO.C".

            BOOL        CreateFromDirAndFilename(const CDir &, const TCHAR *);
                // Initialize the object given a directory (CDir object) and
                // a filename.  This behaves exactly the same as the Create()
                // method, except that the Create() method canonicalizes the
                // filename relative to the CURRENT directory, whereas this
                // method canonicalizes the filename relative to the SPECIFIED
                // directory.

            BOOL        ContainsSpecialCharacters () const
                        {
                            return ::ScanPathForSpecialCharacters(m_strCanon);
                        }
                // Scan the pathname for special character.  We cache this
                // information.

    inline  CPath &     operator =(const CPath & path)
                        {
                            VSASSERT(path.IsInit(), "Path not initialized before use");
                            m_strCanon = path.m_strCanon;
                            m_ichLastSlash = path.m_ichLastSlash;
                            m_Flags = path.m_Flags;
                            return(*this);
                        }
                // Assignment operator.

    // Query methods
    inline  const TCHAR * GetFileName() const
                    {
                        VSASSERT(IsInit(), "Path not initialized before use");
                        VSASSERT(m_ichLastSlash==m_strCanon.ReverseFind('\\'), "m_ichLastSlash not properly initialized");
                        return ((const TCHAR *)m_strCanon + m_ichLastSlash + 1);
                    }

                // Return a pointer to the filename part of the canonicalized
                // pathname, i.e., the filename with no leading drive or path
                // information. Return whole string if no backslash (not init).
                //
                // Please do not write through this pointer, as it is pointing
                // to internal data!

            VOID        PostFixNumber();
                // Modifies the path by postfixing a number on the end of the path's
                // basename. If there is no number on the end of the path's basename
                // then the number 1 is postfixed. Otherwise if there already is a
                // number on the end of the path's basename then that number is
                // incremented by 1 and postfixed on the end of the basename (less the
                // original number).
                //
                // e.g. foo.cpp -> foo1.cpp -> foo2.cpp -> foo3.cpp
            
            VOID        GetBaseNameString(CString &) const;
                // Creates a CString representing the base name of the fully
                // canonicalized pathname.  For example, the base name of
                // the pathname "C:\FOO\BAR.C" is "BAR".
                //
                // This method can't return a pointer to internal data like
                // some of the other methods since it would have to remove
                // the extension in order to do so.

            VOID        GetDisplayNameString(
                                        CString &,
                                        int cchMax = 16,
                                        BOOL bTakeAllAsDefault = FALSE
                                        ) const;
                // Creates a CString representing the name of the file
                // shortened to cchMax CHARACTERS (TCHARs, not bytes) or
                // less.  Only the actual characters are counted; the
                // terminating '\0' is not considered, so
                // CString::GetLength() on the result MAY return as much as
                // cchMax.  If cchMax is less than the length of the base
                // filename, the resulting CString will be empty, unless
                // bTakeAllAsDefault is TRUE, in which the base name is
                // copied in, regardless of length.
                //
                // As an example, "C:\SOMEDIR\OTHERDIR\SUBDIR\SPECIAL\FOO.C"
                // will be shortened to "C:\...\SPECIAL\FOO.C" if cchMax is 25.

        inline  const TCHAR * GetExtension() const
                    {
                        VSASSERT(IsInit(), "Path not initialized before use");
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

    inline  const TCHAR * GetFullPath() const { return(m_strCanon); }
                // Return a pointer to the full (canonicalized) pathname.
                //
                // Please do not write through this pointer, as it is pointing
                // to internal data!
    inline  const TCHAR * GetFullPath(CString & strPath) const { return(strPath = m_strCanon); }

    inline  BOOL        IsActualCase() const { VSASSERT(this!=NULL, "Path not initialized before use"); return ((m_Flags & eIsActualCase)!=0); }
    void GetActualCase(BOOL bEntirePath = FALSE);
                // Adjusts the paths case to match the actual path and filename
                // on disk.
    void SetActualCase(LPCTSTR pszFileCase); 
                // Adjusts the paths case to match the actual path and filename
                // on disk, where pszFileCase already contains the correct case
                // for just the filename portion.
    static void ResetDirMap();

    inline              operator const TCHAR *() const { return(m_strCanon); }
                // Return the fully canonicalized filename as a (const TCHAR *).
                // Same thing as GetFullPath(), but more convenient in some
                // cases.
                //
                // Please do not write through this pointer, as it is pointing
                // to internal data!

    inline  BOOL        IsUNC() const { return(m_strCanon[0] == _T('\\')); }
                // Returns TRUE if the pathname is UNC (e.g.,
                // "\\server\share\file"), FALSE if not.

    inline BOOL IsEmpty() const { return (m_strCanon.IsEmpty()); }

    // Comparison methods

            int         operator ==(const CPath &) const;
                // Returns 1 if the two CPaths are identical, 0 if they are
                // different.

    inline  int         operator !=(const CPath & path) const { return(!(operator ==(path))); }
                // Returns 1 if the two CPaths are different, 0 if they are
                // identical.

    // Modification methods

            VOID        ChangeFileName(const TCHAR *);
                // Changes the file name to that specified by the
                // (const TCHAR *) argument.  The directory portion of the
                // pathname remains unchanged.  DO NOT pass in anything
                // other than a simple filename, i.e., do not pass in
                // anything with path modifiers.

            VOID        ChangeExtension(const TCHAR *);
                // Changes the extension of the pathname to be that specified
                // by the (const TCHAR *) argument.  The argument can either be
                // of the form ".EXT" or "EXT".  If the current pathname has
                // no extension, this is equivalent to adding the new extension.

            BOOL        GetRelativeName (const CDir&, CString&, BOOL bQuote = FALSE, BOOL bIgnoreAlwaysRelative = FALSE) const;
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
                //  \\danwhite\tmp\test.cpp   => \\danwhite\tmp\test.cpp

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

            BOOL        CreateFromDirAndRelative (const CDir&, const TCHAR *);
                // THIS FUNCTION IS OBSOLETE.  New code should use
                // CreateFromDirAndFilename().  The only difference between
                // that function and this one is that this one will
                // automatically remove quotes from around the relative
                // path name (if present).


    // Miscellaneous methods
    inline  BOOL        IsReadOnlyOnDisk() const
                        {
                            HANDLE  h;

                            VSASSERT(IsInit(), "Path not initialized before use");
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
                //      - Files marked as read-only
                //      - Files on read-only network drives

    inline  BOOL        ExistsOnDisk() const
                        {
                            VSASSERT(IsInit(), "Path not initialized before use");
                            return(_taccess(m_strCanon, 00) != -1);
                        }
                // Returns TRUE if the filename represented by this object
                // exists on disk, FALSE if not.

    inline  BOOL        CanCreateOnDisk(BOOL fOverwriteOK = FALSE) const
                        {
                            VSASSERT(IsInit(), "Path not initialized before use");
                            if (!fOverwriteOK && ExistsOnDisk())
                                return(FALSE);
                            int hFile = _tcreat(m_strCanon, _S_IREAD | _S_IWRITE);
                            BOOL fCreate = (hFile != -1);
                            if (fCreate)
                            {
                                _close(hFile);
                                _unlink(m_strCanon);
                            }
                            return(fCreate);
                        }
                // Returns TRUE if the filename represented by this object
                // can be created on disk, FALSE if not.

    inline  BOOL        DeleteFromDisk() const
                        {
                            VSASSERT(IsInit(), "Path not initialized before use");
#ifdef _WIN32
                            return(DeleteFile((TCHAR *)(const TCHAR *)m_strCanon));
#else
                            return(remove(m_strCanon) != -1);
#endif
                        }
                // Removes the file represented by this object from the disk.

    BOOL GetFileTime(LPFILETIME lpftLastWrite);
    BOOL GetFileTime(CString& rstrLastWrite, DWORD dwFlags = DATE_SHORTDATE);
    // Returns the last modified time, as either an FILETIME struct or a string
};

extern const CString AFX_DATA pthEmptyString;


//  File Name Utility Functions
//      These are redundant and could be replaced with use of CPath, but are
//      kept since they are easier to use and already exist in VRES.

// Remove the drive and directory from a file name.
CString StripPath(LPCTSTR szFilePath);

// Remove the name part of a file path.  Return just the drive and directory.
CString StripName(LPCTSTR szFilePath);

// Get only the extension of a file path.
CString GetExtension(LPCTSTR szFilePath);

// Return the path to szFilePath relative to szDirectory.  (e.g. if szFilePath
// is "C:\FOO\BAR\CDR.CAR" and szDirectory is "C:\FOO", then "BAR\CDR.CAR"
// is returned.  This will never use '..'; if szFilePath is not in szDirectory
// or a sub-directory, then szFilePath is returned unchanged.
//
CString GetRelativeName(LPCTSTR szFilePath, LPCTSTR szDirectory = NULL);

// Makes a file path look like in MRU.
CString GetDisplayName(LPCTSTR szFilePath, int nMaxDisplayLength,
    LPCTSTR szDirectory = NULL);

BOOL FileExists(LPCTSTR szFilePath); //REVIEW(chriskoz): only res\src\rcio.cpp uses it

//////////////////////////////////////////////////////////////////////
// CDir
//
// The CDir object represents a file system directory on some disk.
//
// A CDir object can be created to represent the current directory,
// to represent the directory of a CPath object (i.e., the directory
// in which a file resides), and to represent a temporary directory.
// Note that a CDir object CANNOT be created given an arbitrary string --
// this is intentional, since this should not be necessary.
//
// The string representation of a CDir object (e.g., operator const TCHAR *())
// MAY or MAY NOT end in '\'.  The root directory of a local drive (e.g., C:)
// will end in '\' ("C:\"), while other directories on a local drive will
// not ("C:\OTHERDIR").  The root directory on a REMOTE drive will NOT end
// in '\' ("\\server\share").  Don't make any assumptions about whether or
// not the string representation ends in '\'.
//
// See also several CPath methods which use CDir objects.

class CDir : public CObject
{
    DECLARE_DYNAMIC(CDir)

    friend  class       CPath;

    friend  static VOID     ConstructElement(CDir *);
    friend  static VOID     DestructElement(CDir *);

protected:
            CString     m_strDir;
                // Directory name, including drive letter or
                // server/share.  Do NOT make any assumptions
                // about whether or not this ends in '\'!

            // Creates multi level directories just fine
            BOOL MakeDirectory(LPCTSTR lpszPathName) const;
public:
    // Constructors, destructors, initialization methods
    inline              CDir() {}
    inline              CDir(const CDir & dir) { m_strDir = dir.m_strDir; }
    virtual             ~CDir();

            BOOL        CreateFromCurrent();
                // Initialize from the current working directory.  This
                // may fail if the current working directory is unknown
                // or invalid.

            BOOL        CreateFromPath(const CPath &);
                // Initialize based on the directory of the specified
                // CPath object.  That is, if the CPath object represents
                // the file "C:\FOO\BAR\BLIX.C", the resulting directory
                // for this object will be "C:\FOO\BAR".  Returns FALSE
                // on failure.

            BOOL        CreateFromPath(const TCHAR *pszPath);
                // Initialize based on the directory of the specified
                // string.  That is, if the string contains the file name
                // "C:\FOO\BAR\BLIX.C", the generated directory for this
                // string will be "C:\FOO\BAR".  Returns FALSE on failure.

            inline BOOL     CreateFromString(const TCHAR * sz)
                    {
                        return  CreateFromStringEx(sz, FALSE);
                    }   
                // Create from a string (e.g., "C:\", "C:\TMP", etc.).  Please
                // do not use this method when another would suffice!

            BOOL        CreateFromStringEx(const TCHAR * sz, BOOL fRootRelative);
                // Create from a string (e.g., "C:\", "C:\TMP", etc.).  Please
                // do not use this method when another would suffice!
                // same as CreateFromString with minor change. Not treating as bug fix to CFS
                // due to lateness in VC 4.0 project time

                // if fRootRelative true, treat dir ending with colon as relative not root dir 
                // (actual correct handling)


            BOOL        ContainsSpecialCharacters () const
                        {
                            return ::ScanPathForSpecialCharacters(m_strDir);
                        }
                // Scan the pathname for special character.  We cache this information.

    inline  CDir &      operator =(const CDir & dir)
                        {
                            m_strDir = dir.m_strDir;
                            return(*this);
                        }
                // Assignment operator.

    // Query methods

    inline              operator const TCHAR *() const { return(m_strDir); }
                // Return the directory name as a (const TCHAR *) string.

    inline int GetLength() const { return (int)m_strDir.GetLength(); }
          // Returns the length of the directory name

    // Miscellaneous methods

    BOOL    MakeCurrent() const;
                // Make this object the current working directory.  May fail
                // if the directory no longer exists (e.g., a floppy drive).

    inline  BOOL        ExistsOnDisk() const
                        {
                            // Tests if the directory exists.  We return FALSE
                            // if <m_strDir> exists but is not a directory
                            struct _stat statDir;
                            if (_stat(m_strDir, &statDir) == -1)
                                return FALSE;        // Not found.
                            else if (!(statDir.st_mode & _S_IFDIR))
                                return FALSE;        // Not a directory.
                            else
                                return TRUE;
                        }
                // Returns TRUE if the directory represented by this object
                // exists on disk, FALSE if not.

    inline  BOOL        CreateOnDisk() const { return MakeDirectory(m_strDir); }
                // Creates the directory on disk.  If this fails, returns
                // FALSE.  If the directory already existed on disk, returns
                // TRUE (i.e., that is not an error condition).

    inline  BOOL        RemoveFromDisk() const { return RemoveDirectory(m_strDir); }
                // Removes the directory from the disk.  If this fails for
                // any reason (directory does not exist, directory is not
                // empty, etc.), returns FALSE.

            BOOL        IsRootDir() const;
                // Returns TRUE if the directory represented by this object
                // is a root directory (e.g., "C:\"), FALSE if not.  Note that
                // calling this method will NOT tell you whether or not the
                // string representation ends in '\', since "\\server\share"
                // is a root directory, and does not end in '\'.

    inline  BOOL        IsUNC() const { return(m_strDir[0] == _T('\\')); }
                // Returns TRUE if this is a UNC directory, FALSE if not.

            VOID        AppendSubdirName(const TCHAR *);
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

            VOID        RemoveLastSubdirName();
                // Removes the last component of the directory name.  For
                // example, if this object currently represents
                // "C:\FOO\BAR\$AUTSAV$", after this method it will
                // represent "C:\FOO\BAR".  If you try to call this method
                // when the object represents a root directory (e.g., "C:\"),
                // it will assert.

    // Comparison methods

            int         operator ==(const CDir &) const;
                // Returns 1 if the two CDirs are identical, 0 if they are
                // different.

    inline  int         operator !=(const CDir & dir) const { return(!(operator ==(dir))); }
                // Returns 1 if the two CDirs are different, 0 if they are
                // identical.
};

///////////////////////////////////////////////////////////////////////////////
//  CCurDir
//      This class is used to switch the current drive/directory during the
//      life of the object and to restore the previous dirve/directory upon
//      destruction.

class CCurDir : CDir
{
public:
    CCurDir(const char* szPath, BOOL bFile = FALSE);
    CCurDir(const CDir& dir);
    CCurDir();  // just saves the current directory and resets it
    ~CCurDir();

    CDir m_dir;
};


///////////////////////////////////////////////////////////////////////////////
//  CFileOpenReturn
//      This class represents the return value from the Common Dialogs
//      File.Open.  It handles both single and multiple select types.
//

class CFileOpenReturn : CObject
{
    BOOL        m_bSingle;
    BOOL        m_bBufferInUse;
    BOOL        m_bArrayHasChanged;

    INT_PTR     m_cbData;
    _TCHAR *    m_pchData;

    // Multiple Files
    CPtrArray   m_rgszNames;

public:
    CFileOpenReturn (const _TCHAR * szRawString = NULL);
    ~CFileOpenReturn ();

    inline BOOL IsSingle () const;
    inline BOOL IsDirty() const;
    inline BOOL BufferOverflow () const;
    //inline int  GetLength () const;

    // GetBuffer gives permission for something else to directly change the buffer
    // ReleaseBuffer signifies that the something else is done with it.
    _TCHAR * GetBuffer (int cbBufferNew);
    inline void ReleaseBuffer ();

    // allows the object to be re-initialized
    void ReInit (const _TCHAR * szRawString);

    // This supports the dynamic file extension update in OnFileNameOK().
    void ChangeExtension (int i, const CString& szExt);

    void CopyBuffer (_TCHAR * szTarget);

    // This is the function to use to get at the user's selections,
    // whether single or multiple.
    BOOL GetPathname (int i, CString& strPath) const;

private:
    void GenArrayFromBuffer ();
    void GenBufferFromArray ();
    void ClearNamesArray ();
    void SetBuffer (const _TCHAR * szRawString);    
};


inline BOOL CFileOpenReturn::IsSingle () const
{
    return m_bSingle;
}

inline BOOL CFileOpenReturn::IsDirty() const
{
    return m_bArrayHasChanged;
}

inline BOOL CFileOpenReturn::BufferOverflow () const
{
    return m_cbData == 2 && m_pchData[0] == '?';
}

///// ReleaseBuffer - Tell object we're done changing the buffer
//
//  Processes the raw string
//
///
inline void CFileOpenReturn::ReleaseBuffer ()
{
    m_bBufferInUse = FALSE;
    GenArrayFromBuffer ();
}

///////////////////////////////////////////////////////////////////////////////
//  Smart case helpers.
//      These functions are used to do smart casing of paths and file extensions.

extern BOOL GetActualFileCase( CString& rFilename, LPCTSTR lpszDir = NULL );
extern LPCTSTR GetExtensionCase( LPCTSTR lpszFilename, LPCTSTR lpszExtension );

extern BOOL GetDisplayFile(CString &rFilename, CDC *pDC, int &cxPels); // truncates from left

/////////////////////////////////////////////////////////////////////////////
//  Path collections
//

class CPathList : public CObject
{

    DECLARE_DYNAMIC(CPathList)

protected:
    struct CNode
    {
        CNode* pNext;
        CNode* pPrev;
        CPath* data;
    };
public:

// Construction
    CPathList(int nBlockSize=10);

// Attributes (head and tail)
    // count of elements
    int GetCount() const;
    BOOL IsEmpty() const;

    // peek at head or tail
    CPath*& GetHead();
    CPath* GetHead() const;
    CPath*& GetTail();
    CPath* GetTail() const;

// Operations
    // get head or tail (and remove it) - don't call on empty list !
    CPath* RemoveHead();
    CPath* RemoveTail();

    // add before head or after tail
    POSITION AddHead(CPath* newElement);
    POSITION AddTail(CPath* newElement);

    // add another list of elements before head or after tail
    void AddHead(CPathList* pNewList);
    void AddTail(CPathList* pNewList);

    // remove all elements
    void RemoveAll();

    // iteration
    POSITION GetHeadPosition() const;
    POSITION GetTailPosition() const;
    CPath*& GetNext(POSITION& rPosition); // return *Position++
    CPath* GetNext(POSITION& rPosition) const; // return *Position++
    CPath*& GetPrev(POSITION& rPosition); // return *Position--
    CPath* GetPrev(POSITION& rPosition) const; // return *Position--

    // getting/modifying an element at a given position
    CPath*& GetAt(POSITION position);
    CPath* GetAt(POSITION position) const;
    void SetAt(POSITION pos, CPath* newElement);
    void RemoveAt(POSITION position);

    // inserting before or after a given position
    POSITION InsertBefore(POSITION position, CPath* newElement);
    POSITION InsertAfter(POSITION position, CPath* newElement);

    // helper functions (note: O(n) speed)
    POSITION Find(CPath* searchValue, POSITION startAfter = NULL) const;
                        // defaults to starting at the HEAD
                        // return NULL if not found
    POSITION FindIndex(int nIndex) const;
                        // get the 'nIndex'th element (may return NULL)

// Implementation
protected:
    CNode* m_pNodeHead;
    CNode* m_pNodeTail;
    int m_nCount;
    CNode* m_pNodeFree;
    struct CPlex* m_pBlocks;
    int m_nBlockSize;

    CNode* NewNode(CNode*, CNode*);
    void FreeNode(CNode*);

public:
    ~CPathList();
#ifdef _DEBUG
    void Dump(CDumpContext&) const;
    void AssertValid() const;
#endif
};


////////////////////////////////////////////////////////////////////////////
#ifndef _AFXCOLL_INLINE
#define _AFXCOLL_INLINE inline
#endif

_AFXCOLL_INLINE int CPathList::GetCount() const
    { return m_nCount; }
_AFXCOLL_INLINE BOOL CPathList::IsEmpty() const
    { return m_nCount == 0; }
_AFXCOLL_INLINE CPath*& CPathList::GetHead()
    { VSASSERT(m_pNodeHead != NULL, "Cannot do GetHead on empty list");
        return m_pNodeHead->data; }
_AFXCOLL_INLINE CPath* CPathList::GetHead() const
    { VSASSERT(m_pNodeHead != NULL, "Cannot do GetHead on empty list");
        return m_pNodeHead->data; }
_AFXCOLL_INLINE CPath*& CPathList::GetTail()
    { VSASSERT(m_pNodeTail != NULL, "Cannot do GetTail on empty list");
        return m_pNodeTail->data; }
_AFXCOLL_INLINE CPath* CPathList::GetTail() const
    { VSASSERT(m_pNodeTail != NULL, "Cannot do GetTail on empty list");
        return m_pNodeTail->data; }
_AFXCOLL_INLINE POSITION CPathList::GetHeadPosition() const
    { return (POSITION) m_pNodeHead; }
_AFXCOLL_INLINE POSITION CPathList::GetTailPosition() const
    { return (POSITION) m_pNodeTail; }
_AFXCOLL_INLINE CPath*& CPathList::GetNext(POSITION& rPosition) // return *Position++
    { CNode* pNode = (CNode*) rPosition;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to GetNext");
        rPosition = (POSITION) pNode->pNext;
        return pNode->data; }
_AFXCOLL_INLINE CPath* CPathList::GetNext(POSITION& rPosition) const // return *Position++
    { CNode* pNode = (CNode*) rPosition;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to GetNext");
        rPosition = (POSITION) pNode->pNext;
        return pNode->data; }
_AFXCOLL_INLINE CPath*& CPathList::GetPrev(POSITION& rPosition) // return *Position--
    { CNode* pNode = (CNode*) rPosition;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to GetPrev");
        rPosition = (POSITION) pNode->pPrev;
        return pNode->data; }
_AFXCOLL_INLINE CPath* CPathList::GetPrev(POSITION& rPosition) const // return *Position--
    { CNode* pNode = (CNode*) rPosition;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to GetPrev");
        rPosition = (POSITION) pNode->pPrev;
        return pNode->data; }
_AFXCOLL_INLINE CPath*& CPathList::GetAt(POSITION position)
    { CNode* pNode = (CNode*) position;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to GetAt");
        return pNode->data; }
_AFXCOLL_INLINE CPath* CPathList::GetAt(POSITION position) const
    { CNode* pNode = (CNode*) position;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to GetAt");
        return pNode->data; }
_AFXCOLL_INLINE void CPathList::SetAt(POSITION pos, CPath* newElement)
    { CNode* pNode = (CNode*) pos;
        VSASSERT(AfxIsValidAddress(pNode, sizeof(CNode)), "Bad parameter to SetAt");
        pNode->data = newElement; }

/////////////////////////////////////////////////////////////////////////////

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif // __PATH_H__