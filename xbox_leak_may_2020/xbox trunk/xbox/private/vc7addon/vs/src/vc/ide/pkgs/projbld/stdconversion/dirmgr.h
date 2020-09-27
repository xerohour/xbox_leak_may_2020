// DIRMGR.H
// --------
// Defines the directory manager, which manages the lists
// of directories for:
//
// - Executable files (traditionally the 'PATH' environment variable)
// - Include files ('INCLUDE')
// - Library files ('LIB')
// - Help files
//
// History
// =======
// 27-Aug-93	mattg		Created
// 10-Jan-93	colint		Added CToolset

#ifndef __DIRMGR_H__
#define __DIRMGR_H__

class CDirMgr; // Forward defn.

enum DIRLIST_TYPE	// these MUST start at -1 (with the uninitialized value) and be monotonically increasing
{
	DIRLIST_UNINITIALIZED = -1,
	DIRLIST_PATH = 0,
	DIRLIST_INC,
	DIRLIST_LIB,
    DIRLIST_SOURCE,
	C_DIRLIST_TYPES		// number of elements in the list: keep this last
};

////////////////////////////////////////////////////////////
// CToolset

class CToolset : public CObject
{
	friend class CDirMgr;

protected:
			CObList * m_Dirs[C_DIRLIST_TYPES];

			// Cache of the semi-colon seperated string representation
			// of this path. 
			CString   m_DirString[C_DIRLIST_TYPES];

			VOID RefreshAllCachedStrings( );

public:
			CToolset();
			~CToolset();

			CObList * GetDirList(DIRLIST_TYPE type);
				// Return a list of CDir objects.  The DIRLIST_TYPE argument
				// specifies which kind of list you want (DIRLIST_PATH,
				// DIRLIST_INC, etc.).   
				//
				// The returned list may be empty!
				//
				// PLEASE don't modify the list returned, as it's a pointer to
				// the actual list!

			VOID GetDirListString(CString &str, DIRLIST_TYPE type);

			VOID SetDirList(DIRLIST_TYPE type, CObList * pList);
				// Sets the specified list to the one passed in.  The 'original'
				// (existing) list is discarded, along with its contained objects.
};
 
////////////////////////////////////////////////////////////
// CDirMgr

class CDirMgr : public CObject
{
protected:
			CObList m_Toolsets;
				// List of CToolset objects, which contain arrays of lists of 
				// CDir objects for executable files,  include files, etc.
			INT	m_nCurrentToolset;
				// Index of the current toolset
			CStringList m_ToolsetNames;
				// List of the platform names for each toolset.

			CToolset * GetToolset(INT nToolset);
				// Useful helper function for retrieving a toolset.

public:
					CDirMgr();
	virtual			~CDirMgr();

			INT	GetCurrentToolset(); 
			VOID	SetCurrentToolset(INT nToolset);
				// Get or set the current toolset.

			INT		GetNumberOfToolsets();
				// Returns the number of toolsets currently stored in the
				// directories database

			INT		AddToolset(const CString & strPlatform);
				// Adds a new toolset to the directories database. The toolset
				// corresponds to the environment for a particular
				// platform, which is specified by the strTargetPlatform parameter.
			VOID		DeleteToolset(INT nToolset);
				// Deletes a toolset - this would be useful if the platforms supported
				// can change at run-time.
			
			const CObList * GetDirList(DIRLIST_TYPE type, INT nToolset = -1);
				// Return a list of CDir objects.  The DIRLIST_TYPE argument
				// specifies which kind of list you want (DIRLIST_PATH,
				// DIRLIST_INC, etc.).  The nToolSet argument specifies which list;
				// -1 means the 'current' list and is normally what you want.
				// If nToolSet is not -1, it must be in the range 0..CTOOLSETS-1
				// to specify a particular toolset.
				//
				// The returned list may be empty!
				//
				// PLEASE don't modify the list returned, as it's a pointer to
				// the actual list!

			VOID	GetDirListString(CString & str, DIRLIST_TYPE type, INT nToolset = -1);
				// Similar to GetDirList(), except returns (through the str
				// arg) a concatenated list of paths delimited by semicolon
				// characters (e.g., "C:\FOO;C:\BAR;C:\BIN").

			CObList * CloneDirList(DIRLIST_TYPE type, INT nToolset);
				// Clones the specified list and returns a pointer to the newly
				// created clone.  All CDir objects in the cloned list are also
				// created; that is, they are not simply copied from the 'original'
				// list.  It is the caller's responsibility to delete this list
				// and its contents (i.e., delete each element).  Typically,
				// however, this cloned list will later be used as an argument
				// to SetDirList(), in which case CDirMgr will take care of
				// eventually freeing it.

			VOID	SetDirList(DIRLIST_TYPE type, INT nToolset, CObList * pList);
				// Sets the specified list to the one passed in.  The 'original'
				// (existing) list is discarded, along with its contained objects.

			VOID	SetDirListFromString(DIRLIST_TYPE type, INT nToolset, const TCHAR * sz, BOOL fMustExist = FALSE);
				// Sets the specified list by parsing the string, which must
				// be of the form dir1;dir2;...
				// If 'fMustExist' then don't add a directory in this list if
				// it doesn't exist in the directory's file system.

			INT		GetPlatformToolset(const CString & strPlatform);
				// Returns the toolset number for a particular platform

			CString &	GetToolsetName(INT nToolset);
				// Returns the name of a toolset, this will be the name of a
				// platform that the toolset corresponds to.
};

// retrieve our single-instance of the directory manager for the shell
CDirMgr * GetDirMgr();

#endif // __DIRMGR_H__
