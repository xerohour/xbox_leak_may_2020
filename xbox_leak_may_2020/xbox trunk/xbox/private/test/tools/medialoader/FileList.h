/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	FileList.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 01-Aug-2001

Revision History:

	01-Aug-2001 jeffsul
		Initial Version

--*/

#ifndef __FILELIST_H__
#define __FILELIST_H__

#include "HTTPHeader.h"

#include <xtl.h>

typedef struct _FILELISTNODE			//	a stucture to hold info for each file in a list
{
	WIN32_FIND_DATA	FindData;			//	contains file info (see msdn)
	BOOL	bUsed;						//	whether or not the file has been "accessed" already
	_FILELISTNODE* pNext;				//	pointer to the next file node structure in the list
} FILELISTNODE, *PFILELISTNODE;

class CFileList
{
public:
	CFileList();
	~CFileList();

	HRESULT												//	adds a file to the list
	AddFile( 
		IN CONST LPWIN32_FIND_DATA lpFindFileData		//	file info for node to be added
	);

	HRESULT												//	remove a file from the list
	RemoveFile(					
		IN LPCSTR lpszFileName							//	the name of the file to remove
	);

	HRESULT												//	gets the file info from a random unused file in the list,
	GetRandomUnusedFile(								//	then marks it as used
		OUT LPWIN32_FIND_DATA lpFindFileData			//	a pointer to the structure to fill with the found data
	);

	HRESULT												//	gets the first unused file from the list,
	GetFirstUnusedFile(									//	then marks it as used
		OUT LPWIN32_FIND_DATA lpFindFileData			//	a pointer to the structure to fill with the found data
	);

	INT													//	returns the number of files in the list
	GetNumFiles();										

	HRESULT												//	creates a file list from an html directory listing,
	ParseHTML(											//	files that match the search string are added
		IN LPCSTR lpszBuf,								//	a character buffer containing a full html file 
		IN DWORD dwSize,								//	the size of the html buffer
		IN LPCSTR lpszSearchString						//	the regular expression string that file names must match
	);

protected:

private:
	PFILELISTNODE	m_pFirstFile;						//	a pointer to the first file node in the list
	UINT			m_nNumFiles;						//	the number of nodes in the list

	BOOL												//	a utility function to see if a filename matches
	FileNameMatches(									//	a particular regular expression
		IN LPCSTR lpszSearchString,					//	the regular expression string that file names must match
		IN LPCSTR lpszFileName							//	the (full path and) file name of the file in question
	);

	//VOID
	//CopyFileName(
	//	IN LPSTR  lpszDest,
	//	IN LPCSTR lpszSrc
	//);
};
	
#endif // #ifndef __FILELIST_H__
