//////////////////////////////////////////////////////////////////////
// MACMGR.H
//
// Definition of CMacList.  The CMacList class defines a list of macros.
// CMacLists are used to maintain the macros read from a .MAK file, although
// the class is flexible enough to be used for other types of macros.
//
// History
// =======
// Date			Who			What
// ----			---			----
// 22-May-93	mattg		Created
// 07-Jun-93	mattg		Added to VSHELL
//////////////////////////////////////////////////////////////////////

#ifndef __MACMGR_H__
#define __MACMGR_H__

#ifndef _SUSHI_PROJECT
// If you are encluding this in a file that is being compiled under
// WIN16, either make sure everything compiles under WIN16 and remove
// this error, or include conditional on _SUSHI_PROJECT.
//
#error This code may not be Win 3.1 compatible.
#endif

//////////////////////////////////////////////////////////////////////
// Classes defined in this file

// CObject
	class CMacList;

//////////////////////////////////////////////////////////////////////
// CMacList

class CMacList : public CObject
{
	DECLARE_DYNAMIC(CMacList)

protected:
	// Data
			CMapStringToString m_dict;
				// This is really the basis of the macro list -- a
				// collection which maps strings (macro names) to strings
				// (macro values).

public:
	// Constructors, destructors, initialization methods
	// FUTURE: tune for proper hash table size (must be prime)
	inline				CMacList() { m_dict.InitHashTable(97); }
	virtual				~CMacList();

	// Query methods
	inline	INT			GetMacCount() const
						{
							return(m_dict.GetCount());
						}
				// Return the number of macros currently in the list.

	inline	POSITION	GetFirstMacPosition() const
						{
							return(m_dict.GetStartPosition());
						}
				// Return the POSITION of the first macro in the list.

	inline	VOID		GetNextMac(POSITION & pos, CString & strMacName, CString & strMacVal) const
						{
							m_dict.GetNextAssoc(pos, strMacName, strMacVal);
						}
				// Get the next macro in the list.

			BOOL		IsMacDefined(const TCHAR *) const;
				// Return TRUE if the specified macro is defined, FALSE if not.

			const TCHAR * GetMacVal(const TCHAR *) const;
				// Return a pointer to the string representing the value of
				// the specified macro.  This method will ASSERT and return NULL
				// if the macro does not exist in this list.  If you are uncertain
				// whether the macro exists or not, call IsMacDefined() first.
				//
				// This method returns a pointer to internal data -- do not modify
				// this data!  If you require persistence of the resulting string,
				// make a copy of it.

	// Modification methods

	inline	VOID		SetMacVal(const TCHAR * szMacName, const TCHAR * szMacValue)
						{
							m_dict.SetAt(szMacName, szMacValue);
						}
				// Set the value of the macro specified by the szMacName parameter
				// to the value specified by the szMacValue parameter.  If the
				// specified macro is not already in the list, it is added; if it
				// is already in the list, its old value is replaced.

	inline	VOID		RemoveMac(const TCHAR * szMacName)
						{
							m_dict.RemoveKey(szMacName);
						}
				// Remove the macro specified by szMacName.

	// Miscellaneous methods

			VOID		ResolveString(CString &);
				// Resolves all macro references in the specified string.  This
				// occurs recursively, that is, if a macro resolves to more macros,
				// each of those macros is also resolved.  After this method is
				// invoked, the string will contain no macro references.
};

#endif // __MACMGR_H__
