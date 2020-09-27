#include "stdafx.h"
#include <afxwin.h>
#include "cleanup.h"


void CleanUp(LPCSTR szDirName)
	
{

	char psz[_MAX_DIR];
	GetCurrentDirectory(_MAX_DIR - 1, psz);
	CString strPath;
	strPath = (CString)"src\\" + szDirName + "\\";

	OurKillFile(strPath, "*.dsp");
	OurKillFile(strPath, "*.dsw");
	OurKillFile(strPath, "*.opt");
	OurKillFile(strPath, "*.plg");
	OurKillFile(strPath, "*.ncb");
	OurKillFile(strPath, "*.mak");
	OurKillFile(strPath, "*.lnk");
	OurKillFile(strPath, "*.mdp");
	OurKillFile(strPath, "*.pdb");
	OurKillFile(strPath, "*.obj");
	OurKillFile(strPath, "*.ilk");
	OurKillFile(strPath, "*.dll");
	OurKillFile(strPath, "*.exe");
	OurKillFile(strPath, "*.pch");
	OurKillFile(strPath, "*.idb");
	OurKillFile(strPath, "*.tmp");

	OurKillFile(strPath + "\\Debug\\", "*.*");
	RemoveDirectory(strPath + "\\Debug");
}


void OurKillFile(LPCSTR szPathName, LPCSTR szFileName /* NULL */)

	{
	WIN32_FIND_DATA ffdImgFile;
	HANDLE hFile;
	CString str, strPattern, strPath = szPathName;

	if(szFileName != NULL)
		
		{
		if((strPath.Right(1) != "\\") && (szFileName[0] != '\\'))
			strPath += "\\";

		strPattern = strPath + szFileName; 
		}

	else
		strPattern = strPath;

	hFile = FindFirstFile(strPattern, &ffdImgFile);
	
	if(hFile != INVALID_HANDLE_VALUE)
		{
		do
			{
			str = strPath + ffdImgFile.cFileName;
			DeleteFile(str);
			} while(FindNextFile(hFile, &ffdImgFile));
		}

	FindClose(hFile);
	}
