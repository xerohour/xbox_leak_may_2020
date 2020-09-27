///////////////////////////////////////////////////////////////////////////////
//	FILETABL.CPP
//
//	Created by :			Date :
//		EnriqueP					3/4/94
//
//	Description :
//		Implementation of the File Table classes
//

#include "stdafx.h"
#include "filetabl.h"
#include "mstwrap.h"
#include "guiv1.h"
#include "resource.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CFileTableLine  implementation

CFileTableLine::CFileTableLine() 
{
}


CFileTableLine::CFileTableLine(LPCSTR szLine)
{
	while( szLine[0] != '\0' )
	{
		int nSeparator = strcspn(szLine, ",;\t");
		// For each column in table add a string value and grow array as needed.
		m_strArray.Add( CString(szLine, nSeparator) );	
		szLine = szLine + nSeparator + strspn(szLine + nSeparator, ",;\t");
	}
}

CFileTableLine::~CFileTableLine() 
{
	m_strArray.RemoveAll();	
}


int CFileTableLine::LangFromString(CString& str)

{
	str.MakeUpper();

	if(str.Find("Japanese") >= 0)
		return CAFE_LANG_JAPANESE;
	else if(str.Find("German") >= 0)
		return CAFE_LANG_GERMAN;
	else
		return CAFE_LANG_ENGLISH;
}


BOOL CFileTableLine::BoolFromString(CString& str, BOOL bDefault /*=TRUE*/)
{
	str.MakeUpper();
	if( (str == "0") || (str == "FALSE") || (str == "NO") || (str == "F") || (str == "N") )
		return FALSE;
	if( (str == "1") || (str == "TRUE") || (str == "YES") || (str == "T") || (str == "Y") )
		return TRUE;

	LOG->RecordInfo("Could not deduce a boolean value from the string '%s'", (LPCSTR)str);
	return bDefault;
}

/////////////////////////////////////////////////////////////////////////////
// CFileTableLineList implementation
 
CFileTableLineList::CFileTableLineList(void) : CObList()
{
	NULL;
}

CFileTableLineList::~CFileTableLineList()
{
	RemoveAll();
}

CFileTableLine *CFileTableLineList::GetHead(void)
{
	return ((CFileTableLine *)CObList::GetHead());
}

CFileTableLine *CFileTableLineList::GetTail(void)
{
	return ((CFileTableLine *)CObList::GetTail());
}

POSITION CFileTableLineList::AddHead(CFileTableLine *pFileTableLine)
{
	return (CObList::AddHead((CObject *)pFileTableLine));
}

POSITION CFileTableLineList::AddTail(CFileTableLine *pFileTableLine)
{
	return (CObList::AddTail((CObject *)pFileTableLine));
}

CFileTableLine *CFileTableLineList::GetNext(POSITION &pos)
{
	return ((CFileTableLine *)CObList::GetNext(pos));
}

CFileTableLine *CFileTableLineList::GetPrev(POSITION &pos)
{
	return ((CFileTableLine *)CObList::GetPrev(pos));
}

CFileTableLine *CFileTableLineList::GetAt(POSITION pos)
{
	return ((CFileTableLine *)CObList::GetAt(pos));
}

CFileTableLine* CFileTableLineList::RemoveHead(void)
{
	return ((CFileTableLine *)CObList::RemoveHead());
}

void CFileTableLineList::RemoveAll(void)
{
	while( !IsEmpty() )
	{
		CFileTableLine* pftll = RemoveHead();
		delete pftll;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFileTable implementation


CFileTable::CFileTable()
{
	m_szTableName = "files.txt";		 // default name for file table 
}

CFileTable::CFileTable(LPCSTR szName)
{
	m_szTableName = szName;
}

CFileTable::~CFileTable()
{
	DeleteContents();
}


// BEGIN_HELP_COMMENT
// Function: void CFileTable::DeleteContents()
// Description: Removes all entries in the File Table.
// Return: void. 
// Param: none
// END_HELP_COMMENT
void CFileTable::DeleteContents()
{
	m_FileTableLineList.RemoveAll();
}

// BEGIN_HELP_COMMENT
// Function: BOOL CFileTable::LoadTable(LPCSTR szName /*=NULL*/)
// Description: Opens the text file specified by szName and loads its info into the table.
// Return: Returns TRUE if it was successfull loading the table.
// Param: The table's filename to be loaded. It may contain a full path.
// END_HELP_COMMENT
BOOL CFileTable::LoadTable(LPCSTR szName /*=NULL*/)
{
	if( szName != NULL )
		m_szTableName = szName;

	DeleteContents(); // Make sure list is empty
	TRY
	{
		CStdioFile file(m_szTableName, CFile::modeRead | CFile::typeText);
		char acLine[512];
		while( file.ReadString(acLine, 511) != NULL )
			AppendLine(acLine);
	}
	CATCH(CFileException, e)
	{
		AfxMessageBox(IDS_ERROR_CANT_OPEN);
		return FALSE;
	}
	END_CATCH;
	return TRUE;
}

 // CFileTable internal commands

BOOL CFileTable::AppendLine(LPCSTR szNewLine)
{
	char* sz = new char[strlen(szNewLine) + 1];		 
	strcpy(sz, szNewLine);
	char* pchStr = sz;
	pchStr = RemoveExtraChars( pchStr );

	if (pchStr[0] == '\0')
	{
		delete[] sz;
		return FALSE;
	}

	CFileTableLine*  pFileTableLine = new CFileTableLine(pchStr);
	POSITION line = m_FileTableLineList.AddTail( pFileTableLine );	   // Add string to array if is not empty or blank
	delete[] sz;
	return TRUE;
}

char* CFileTable::RemoveExtraChars(char* pchLine) 	  // REVIEW (EnriqueP): Move this code to CFileTableLine::CFileTableLine(pchStr)
{
	int iCRLF = strcspn(pchLine, "\'\r\n");	 // remove trailing comments , CR or LF
	pchLine[iCRLF] = '\0';
	pchLine += strspn(pchLine, "\t ");	// Remove leading spaces or tabs
	return pchLine;
}

// BEGIN_HELP_COMMENT
// Function: CFileTableLine* CFileTable::GetFirstLine()
// Description: Returns a pointer to the first line in the table.
// Return: A CFileTableLine* that can be used to access its member variables or NULL if table is empty.
// Param: None.
// END_HELP_COMMENT
CFileTableLine* CFileTable::GetFirstLine() 
{
	if (m_FileTableLineList.IsEmpty())
	{
		LOG->RecordInfo("The file table is empty");
		return NULL;
	}
	
	return m_FileTableLineList.GetHead();
}


// BEGIN_HELP_COMMENT
// Function: CFileTableLine* CFileTable::GetNextLine(POSITION& pos)
// Description: Retrieves a pointer to the line pointed by pos and advances pos automatically to point to the next line.
// Return: Returns a pointer to a line CFileTableLine*
// Param: pos points to the line that is to be retrieved.
// END_HELP_COMMENT
CFileTableLine* CFileTable::GetNextLine(POSITION& pos) 
{
	return m_FileTableLineList.GetNext(pos);
}

               
// BEGIN_HELP_COMMENT
// Function: POSITION CFileTable::GetFirstLinePos()
// Description: Returns the first POSITION of the first entry on the table. Use this to start a navigating the table.
// Return: Returns a POSITION value to the first entry or NULL if table is empty.
// Param: None.
// END_HELP_COMMENT
POSITION CFileTable::GetFirstLinePos() 
{
	return m_FileTableLineList.GetHeadPosition();
}

// BEGIN_HELP_COMMENT
// Function: CString CFileTable::GetStrValue(POSITION& pos, int column) 
// Description: Returns the first POSITION of the first entry on the table. Use this to start a navigating the table.
// Return: Returns a POSITION value to the first entry or NULL if table is empty.
// Param: None.
// END_HELP_COMMENT
CString CFileTable::GetStrValue(POSITION& pos, int nColumn) 
{
	return m_FileTableLineList.GetAt(pos)->m_strArray[nColumn];
}

// BEGIN_HELP_COMMENT
// Function: int CFileTable::GetLastColumnIndex(POSITION& pos) 
// Description: Returns the first POSITION of the first entry on the table. Use this to start a navigating the table.
// Return: Returns a POSITION value to the first entry or NULL if table is empty.
// Param: None.
// END_HELP_COMMENT
int CFileTable::GetLastColumnIndex(POSITION& pos) 
{
	return m_FileTableLineList.GetAt(pos)->m_strArray.GetUpperBound();
}

