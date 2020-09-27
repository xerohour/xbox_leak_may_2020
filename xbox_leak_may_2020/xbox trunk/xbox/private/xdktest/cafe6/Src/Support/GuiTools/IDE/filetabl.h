///////////////////////////////////////////////////////////////////////
////  FILETABL.H
//
//	Created by :			Date :
//		EnriqueP					3/4/94
//
////  File Table Classes used for table driven test sets.

#ifndef __FILETABL_H__
#define __FILETABL_H__

#include "idexprt.h"

#define   LANG_US  		0x0001
#define   LANG_J		0x0002


///////////////////////////////////////////////////////////////////////
////  CFileTableLine class:  defines a single line of a file table
///////////////////////////////////////////////////////////////////////

class IDE_CLASS CFileTableLine : public CObject
{
// Data members
public:
	CStringArray m_strArray; 
    
// Ctors
public:
	CFileTableLine(void);
	CFileTableLine(LPCSTR szLine);
	~CFileTableLine();

// 	Utilities  
	int LangFromString(CString& str);
	BOOL BoolFromString(CString& str, BOOL bDefault = TRUE);
};


///////////////////////////////////////////////////////////////////////
////  CFileTableLineList class:  defines a linked list of FileTableLine objects
///////////////////////////////////////////////////////////////////////


class IDE_CLASS CFileTableLineList: public CObList
{
public:
	CFileTableLineList();
	~CFileTableLineList();

	CFileTableLine* GetHead(void);
	CFileTableLine* GetTail(void);
	POSITION AddHead(CFileTableLine* pFileTableLine);
	POSITION AddTail(CFileTableLine* pFileTableLine);
	CFileTableLine* GetNext(POSITION& pos);
	CFileTableLine* GetPrev(POSITION& pos);
	CFileTableLine* GetAt(POSITION pos);
	CFileTableLine* RemoveHead(void);
	void RemoveAll(void);
};




///////////////////////////////////////////////////////////////////////
////  CFileTable class:  Stores all relevant info for each file to be used as a test target
///////////////////////////////////////////////////////////////////////

// BEGIN_CLASS_HELP
// ClassName: CFileTable
// BaseClass: None
// Category: Tools
// END_CLASS_HELP
class IDE_CLASS CFileTable 
{
public: 
	CFileTable();
	CFileTable(LPCSTR szName);
	virtual ~CFileTable();
	
// Data
protected:
	CFileTableLineList m_FileTableLineList;	// List that contains lines
	LPCSTR m_szTableName;
public:
	POSITION m_currPos;
// Attributes
public:
	inline int GetNumLines(void) const
	{
		return m_FileTableLineList.GetCount();
	}	
	inline void SetTableName(LPCSTR szTable)
	{
		m_szTableName = szTable;
	}

// Internal Operations
public:
	BOOL AppendLine(LPCSTR szNewLine);
	char* RemoveExtraChars(char* pchLine); 


// Interface
public:
	virtual	void DeleteContents();
	virtual	int LoadTable(LPCSTR szName = NULL);
	virtual CFileTableLine* GetFirstLine(void);
	virtual CFileTableLine* GetNextLine(POSITION& pos);
	virtual POSITION GetFirstLinePos(void); 
	virtual CString CFileTable::GetStrValue(POSITION& pos, int nColumn);
	virtual int GetLastColumnIndex(POSITION& pos);

};

/////////////////////////////////////////////////////////////////////////////


#endif //__FILETABL_H__
