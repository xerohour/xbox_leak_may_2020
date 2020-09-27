/*
	XIP -- Xbox "zip" file tool

	This tool will create XIP files and list the files within one.

	To create a XIP file:

		XIP <output-file> { <input-file> ... }

	To display the contents of a XIP file:

		XIP <xip-file>
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "lzss.h"
#include "xip.h"

/*
	File format:

	XIPHEADER header
	FILENAME filenames [...]
	char rgszNames [...]
	FILEDATA filedata [...]
	BYTE data [...]
*/

extern bool AddMesh(const char* szFileName, DWORD& dwMeshID, DWORD& dwPrimCount);
extern void WriteMergedMeshFiles(CXipCreator* pCreator);

bool bQuiet = false;
bool bMergeMeshFiles = false;

#define MAX_IGNORE 20
char* rgszIgnore [MAX_IGNORE];
int nIgnoreCount = 0;

void AddIgnore(char* szIgnoreFile)
{
	if (nIgnoreCount == MAX_IGNORE)
	{
		fprintf(stderr, "ERROR: too many ignore files (max=%d)\n", MAX_IGNORE);
		exit(1);
	}

	rgszIgnore[nIgnoreCount] = szIgnoreFile;
	nIgnoreCount += 1;
}

bool IgnoreFile(const char* szFile)
{
	for (int i = 0; i < nIgnoreCount; i += 1)
	{
		if (_stricmp(szFile, rgszIgnore[i]) == 0)
			return true;
	}

	return false;
}

bool CompareFiles(HANDLE hFile1, HANDLE hFile2);

bool Read(HANDLE hFile, void* pv, int cb)
{
	DWORD dwRead;
	return ReadFile(hFile, pv, cb, &dwRead, NULL) && dwRead == (DWORD)cb;
}

bool Write(HANDLE hFile, void* pv, int cb)
{
	DWORD dwWrite;
	return WriteFile(hFile, pv, cb, &dwWrite, NULL) && dwWrite == (DWORD)cb;
}

void DumpXIP(const char* szXipFile)
{
	printf("Dumping %s...\n", szXipFile);

	HANDLE hFile = CreateFile(szXipFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open %s\n", szXipFile);
		return;
	}

	XIPHEADER xhdr;
	if (!Read(hFile, &xhdr, sizeof (XIPHEADER)) || xhdr.m_dwMagic != MAGIC)
	{
		printf("%s: not a valid xip file!\n", szXipFile);
		CloseHandle(hFile);
		return;
	}

	FILEDATA* filedata = new FILEDATA [xhdr.m_wFileCount];
	if (!Read(hFile, filedata, sizeof (FILEDATA) * xhdr.m_wFileCount))
	{
		printf("%s: not a valid xip file!\n", szXipFile);
		CloseHandle(hFile);
		return;
	}

	FILENAME* directory = new FILENAME [xhdr.m_wNameCount];
	if (!Read(hFile, directory, sizeof (FILENAME) * xhdr.m_wNameCount))
	{
		printf("%s: not a valid xip file!\n", szXipFile);
		CloseHandle(hFile);
		return;
	}

	DWORD dwNameSize = xhdr.m_dwDataStart - SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	char* names = new char [dwNameSize];
	if (!Read(hFile, names, dwNameSize))
	{
		printf("%s: not a valid xip file!\n", szXipFile);
		CloseHandle(hFile);
		return;
	}

	for (DWORD i = 0; i < xhdr.m_wNameCount; i += 1)
	{
		const char* szName = names + directory[i].m_wNameOffset;
//		printf("%8d %s\n", filedata[directory[i].m_wFileDataIndex].m_dwSize, szName);
		FILEDATA* pFileData = &filedata[directory[i].m_wFileDataIndex];
		if (pFileData->m_dwType == XIP_TYPE_MESH_REFERENCE)
			printf("%3d %3d:%4d %8d %s\n", directory[i].m_wFileDataIndex, pFileData->m_dwDataOffset >> 24, pFileData->m_dwDataOffset & 0x00ffffff, pFileData->m_dwSize, szName);
		else if (pFileData->m_dwType == XIP_TYPE_TEXTURE)
			printf("%3d %8d %8d %s (texture)\n", directory[i].m_wFileDataIndex, xhdr.m_dwDataStart + pFileData->m_dwDataOffset, pFileData->m_dwSize, szName);
		else
			printf("%3d %8d %8d %s\n", directory[i].m_wFileDataIndex, xhdr.m_dwDataStart + pFileData->m_dwDataOffset, pFileData->m_dwSize, szName);

	}

	CloseHandle(hFile);
}



CXipCreator::CXipCreator()
{
	m_nFileCount = 0;
	m_nNameCount = 0;
	m_nFileAlloc = 0;
	m_nNameAlloc = 0;
	m_nTotalSize = 0;
	m_directory = NULL;
	m_filedata = NULL;
	m_rgszFileName = NULL;
	m_cchNames = 0;
	m_cchNamesAlloc = 0;
	m_names = NULL;
}

CXipCreator::~CXipCreator()
{
	delete [] m_directory;
	delete [] m_filedata;
	delete [] m_names;

	for (int i = 0; i < m_nFileCount; i += 1)
		delete [] m_rgszFileName [i];
	delete [] m_rgszFileName;
}

bool CXipCreator::AddFile(const char* szFileName)
{
	DWORD dwFileType = XIP_TYPE_GENERIC;

	// BLOCK: Determine type of file
	{
		const char* szFileType = strrchr(szFileName, '.');
		if (szFileType != NULL)
		{
			szFileType += 1;

			if (_stricmp(szFileType, "xm") == 0)
				dwFileType = XIP_TYPE_MESH;
			else if (_stricmp(szFileType, "ib") == 0)
				dwFileType = XIP_TYPE_INDEXBUFFER;
			else if (_stricmp(szFileType, "vb") == 0)
				dwFileType = XIP_TYPE_VERTEXBUFFER;
			else if (_stricmp(szFileType, "wav") == 0)
				dwFileType = XIP_TYPE_WAVE;
			else if (_stricmp(szFileType, "bmp") == 0)
				dwFileType = XIP_TYPE_TEXTURE;
			else if (_stricmp(szFileType, "tga") == 0)
				dwFileType = XIP_TYPE_TEXTURE;
			else if (_stricmp(szFileType, "xbx") == 0)
				dwFileType = XIP_TYPE_TEXTURE;
		}
	}

	char szTextureFileName [MAX_PATH];
    if (dwFileType == XIP_TYPE_TEXTURE)
	{
		// Change texture file references to *.xt when the *.xt exists...
		strcpy(szTextureFileName, szFileName);

        if (GetEnvironmentVariable("COOL_XDASH", NULL, 0)) {
            strcpy(strrchr(szTextureFileName, '.') + 1, "xbx.cool");
        } else {
            strcpy(strrchr(szTextureFileName, '.') + 1, "xbx");
        }

		HANDLE hFile = CreateFile(szTextureFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			szFileName = szTextureFileName;
		}
		else
		{
			printf("WARNING: missing %s\n", szTextureFileName);
		}
	}

	// BLOCK: Just ignore files that are specified more than once!
	{
		for (int i = 0; i < m_nFileCount; i += 1)
		{
			const char* szName = m_names + m_directory[i].m_wNameOffset;

			if (_stricmp(szFileName, szName) == 0)
				return true;
		}
	}

	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	if (m_nNameCount >= m_nNameAlloc)
	{
		m_nNameAlloc += 100;

		FILENAME* directory = new FILENAME [m_nNameAlloc];
		CopyMemory(directory, m_directory, sizeof (FILENAME) * m_nNameCount);
		delete [] m_directory;
		m_directory = directory;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);

	// See if this file is exactly the same as some other one...
	int nSameAs = -1;
	for (int i = 0; i < m_nFileCount; i += 1)
	{
		if (m_filedata[i].m_dwSize == dwFileSize && m_filedata[i].m_dwType == dwFileType)
		{
			const char* szName = m_names + m_directory[i].m_wNameOffset;
			HANDLE hFile2 = CreateFile(szName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile2 == INVALID_HANDLE_VALUE)
			{
				printf("Cannot open %s\n", szName);
				break;
			}

			bool bSame = CompareFiles(hFile, hFile2);

			CloseHandle(hFile2);

			if (bSame)
			{
				nSameAs = i;

				if (!bQuiet)
					printf("\t%s is the same as %s\n", szFileName, szName);
				break;
			}
		}
	}

	CloseHandle(hFile);

	if (nSameAs == -1)
	{
		// The file is unique so far, so add its content to the xip...
		m_directory[m_nNameCount].m_wFileDataIndex = (WORD)m_nFileCount;

		if (m_nFileCount >= m_nFileAlloc)
		{
			m_nFileAlloc += 100;

			FILEDATA* filedata = new FILEDATA [m_nFileAlloc];
			CopyMemory(filedata, m_filedata, sizeof (FILEDATA) * m_nFileCount);
			delete [] m_filedata;
			m_filedata = filedata;

			char** rgszFileName = new char* [m_nFileAlloc];
			CopyMemory(rgszFileName, m_rgszFileName, sizeof (char*) * m_nFileCount);
			delete [] m_rgszFileName;
			m_rgszFileName = rgszFileName;
		}

		if (bMergeMeshFiles && dwFileType == XIP_TYPE_MESH)
		{
			// Mesh's are funky...
			DWORD dwMeshID, dwPrimCount;
			if (!AddMesh(szFileName, dwMeshID, dwPrimCount))
			{
				fprintf(stderr, "ERROR: Cannot add mesh %s\n", szFileName);
				exit(1);
			}
			m_filedata[m_nFileCount].m_dwDataOffset = dwMeshID;
			m_filedata[m_nFileCount].m_dwSize = dwPrimCount;
			m_filedata[m_nFileCount].m_dwType = XIP_TYPE_MESH_REFERENCE;
			m_filedata[m_nFileCount].m_dwTimestamp = 0;

			m_rgszFileName[m_nFileCount] = NULL;
		}
		else // BLOCK: Add a filedata and its name...
		{
			m_filedata[m_nFileCount].m_dwDataOffset = m_nTotalSize;
			m_filedata[m_nFileCount].m_dwSize = dwFileSize;
			m_filedata[m_nFileCount].m_dwType = dwFileType;
			m_filedata[m_nFileCount].m_dwTimestamp = 0;

			m_rgszFileName[m_nFileCount] = new char [strlen(szFileName) + 1];
			strcpy(m_rgszFileName[m_nFileCount], szFileName);

//				printf("file %d at %d %s\n", m_nFileCount, m_nTotalSize, szFileName);

			m_nTotalSize += dwFileSize;
		}

		m_nFileCount += 1;
	}
	else
	{
		// This file is the same as another, so just reference the one we already have
		m_directory[m_nNameCount].m_wFileDataIndex = (WORD)nSameAs;
	}

	m_directory[m_nNameCount].m_wNameOffset = (WORD)m_cchNames;

	// BLOCK: Add the name to the char buffer...
	{
		int cchName = strlen(szFileName) + 1;
		if (m_cchNames + cchName > m_cchNamesAlloc)
		{
			m_cchNamesAlloc += 1024;
			char* names = new char [m_cchNamesAlloc];
			CopyMemory(names, m_names, m_cchNames);
			delete [] m_names;
			m_names = names;
		}

		CopyMemory(m_names + m_cchNames, szFileName, cchName);
		m_cchNames += cchName;
	}

	m_nNameCount += 1;

	return true;
}




class CAppendLZSS : public CLZSS
{
public:
	CAppendLZSS()
	{
		m_hDestFile = INVALID_HANDLE_VALUE;
		m_hSrcFile = INVALID_HANDLE_VALUE;
	}

	~CAppendLZSS()
	{
		if (m_hSrcFile != INVALID_HANDLE_VALUE)
			CloseHandle(m_hSrcFile);
	}

	bool CompressAll(HANDLE hDestFile, CXipCreator* pCreator)
	{
		m_pCreator = pCreator;
		m_nFile = 0;
		m_hDestFile = hDestFile;

		Encode();

		return true;
	}

	bool StartNextFile()
	{
		if (m_hSrcFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hSrcFile);
			m_hSrcFile = INVALID_HANDLE_VALUE;
		}

		if (m_nFile >= m_pCreator->m_nFileCount)
			return false;

		while (m_pCreator->m_rgszFileName[m_nFile] == NULL)
		{
			m_nFile += 1;
			if (m_nFile >= m_pCreator->m_nFileCount)
				return false;
		}

		m_hSrcFile = CreateFile(m_pCreator->m_rgszFileName[m_nFile], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (m_hSrcFile == INVALID_HANDLE_VALUE)
		{
			printf("Cannot open %s\n", m_pCreator->m_rgszFileName[m_nFile]);
			return false;
		}
		
		if (!bQuiet)
			printf("Compressing %s...\n", m_pCreator->m_rgszFileName[m_nFile]);

		m_nFile += 1;

		return true;
	}

	int ReadByte()
	{
		if (m_hSrcFile == INVALID_HANDLE_VALUE)
		{
			if (!StartNextFile())
				return -1;
		}

		BYTE b;
		DWORD dwRead;
		if (!ReadFile(m_hSrcFile, &b, 1, &dwRead, NULL) || dwRead != 1)
		{
			if (!StartNextFile())
				return -1;

			if (!ReadFile(m_hSrcFile, &b, 1, &dwRead, NULL) || dwRead != 1)
				return -1;
		}
		
		return b;
	}

	void WriteByte(BYTE b)
	{
		DWORD dwWrite;
		if (!WriteFile(m_hDestFile, &b, 1, &dwWrite, NULL))
		{
			fprintf(stderr, "ERROR: cannot write file\n");
			exit(2);
		}
	}

	HANDLE m_hSrcFile;
	HANDLE m_hDestFile;
	CXipCreator* m_pCreator;
	int m_nFile;
};

bool AppendFile(HANDLE hDestFile, const char* szSrcFile)
{
	BYTE buffer [16384];

	HANDLE hSrcFile = CreateFile(szSrcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open %s\n", szSrcFile);
		return false;
	}

	for (;;)
	{
		DWORD dwRead, dwWrite;

		if (!ReadFile(hSrcFile, buffer, 16384, &dwRead, NULL))
		{
			printf("ReadFile(%s) failed (%d)\n", szSrcFile, GetLastError());
			CloseHandle(hSrcFile);
			return false;
		}

		if (dwRead == 0)
			break;

		if (!WriteFile(hDestFile, buffer, dwRead, &dwWrite, NULL))
		{
			printf("WriteFile failed (%d)\n", GetLastError());
			CloseHandle(hSrcFile);
			return false;
		}
	}

	CloseHandle(hSrcFile);

	return true;
}

bool CompareFiles(HANDLE hFile1, HANDLE hFile2)
{
	BYTE buffer1 [16384];
	BYTE buffer2 [16384];

	for (;;)
	{
		DWORD dwRead1, dwRead2;

		if (!ReadFile(hFile1, buffer1, 16384, &dwRead1, NULL))
			return false;

		if (!ReadFile(hFile2, buffer2, 16384, &dwRead2, NULL))
			return false;

		if (dwRead1 != dwRead2)
			return false;

		if (dwRead1 == 0)
			return true;

		if (memcmp(buffer1, buffer2, dwRead1) != 0)
			return false;
	}
}

CXipCreator* g_pCreator;

static int __cdecl SortDirectoryCompare(const void *elem1, const void *elem2)
{
	const FILENAME* pName1 = (const FILENAME*)elem1;
	const FILENAME* pName2 = (const FILENAME*)elem2;
	return _stricmp(g_pCreator->m_names + pName1->m_wNameOffset, g_pCreator->m_names + pName2->m_wNameOffset);
}

void CreateXIP(const char* szXipFile, int nFiles, char* rgszFiles [], bool bCompress)
{
	CXipCreator creator;

	if (!bQuiet)
		printf("Creating %s...\n", szXipFile);

	HANDLE hFile = CreateFile(szXipFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot create %s\n", szXipFile);
		return;
	}

	for (int i = 0; i < nFiles; i += 1)
	{
		if (strchr(rgszFiles[i], '?') != NULL || strchr(rgszFiles[i], '*') != NULL)
		{
			WIN32_FIND_DATA fd;
			HANDLE h = FindFirstFile(rgszFiles[i], &fd);
			if (h != INVALID_HANDLE_VALUE)
			{
				do
				{
					if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
						continue;

					if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
						continue;

					// TODO: If wildcard not in curdir, need to make a path for cFileName...
					if (!creator.AddFile(fd.cFileName))
						printf("Cannot open %s\n", fd.cFileName);
				}
				while (FindNextFile(h, &fd));
				FindClose(h);
			}
		}
		else
		{
			const char* szType = strrchr(rgszFiles[i], '.');
			if (szType != NULL)
			{
				szType += 1;
				if (_stricmp(szType, "xap") == 0)
				{
					HANDLE hXapFile = CreateFile(rgszFiles[i], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (hXapFile != INVALID_HANDLE_VALUE)
					{
						// Check for a corresponding xab file and add it instead
						{
							char szXabFile [MAX_PATH];
							strcpy(szXabFile, rgszFiles[i]);
							strcpy(strrchr(szXabFile, '.') + 1, "xab");

							HANDLE hXabFile = CreateFile(szXabFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
							if (hXabFile != INVALID_HANDLE_VALUE)
							{
								// Check file time and warn if xap is newer than xab!
								FILETIME xapTime, xabTime;

								GetFileTime(hXapFile, NULL, NULL, &xapTime);
								GetFileTime(hXabFile, NULL, NULL, &xabTime);

								CloseHandle(hXabFile);

								if (CompareFileTime(&xapTime, &xabTime) > 0)
								{
									printf("%s: WARNING Compile is out of date; using text version\n", rgszFiles[i]);
									if (!creator.AddFile(rgszFiles[i]))
										printf("Cannot open %s\n", rgszFiles[i]);
								}
								else
								{
									if (!creator.AddFile(szXabFile))
										printf("Cannot open %s\n", szXabFile);
								}
							}
							else
							{
								printf("%s: WARNING Source has not been compiled; using text version\n", rgszFiles[i]);
								if (!creator.AddFile(rgszFiles[i]))
									printf("Cannot open %s\n", rgszFiles[i]);
							}
						}

						// Scan the xap file for .xm files and automatically add those too!
						{
							int nLine = 0;
							for (;;)
							{
								char szLine [1024];
								int cchLine = 0;
								DWORD dwRead;
								while (cchLine < sizeof (szLine) - 1)
								{
									if (!ReadFile(hXapFile, &szLine[cchLine], 1, &dwRead, NULL) || dwRead == 0 || szLine[cchLine] == '\n')
										break;
									cchLine += 1;
								}

								if (cchLine == 0)
									break;

								szLine[cchLine] = 0;
								nLine += 1;

								const char* szURL = strstr(szLine, "url \"");
								if (szURL == NULL)
									continue;

								szURL += 5;
								char* pch = strchr(szURL, '"');
								if (pch == NULL)
									continue;

								*pch = 0;

								pch = strchr(szURL, '.');
								if (pch == NULL)
									continue;
								pch += 1;

								if (!IgnoreFile(szURL))
								{
									if (_stricmp(pch, "wav") != 0)
									{
										if (!creator.AddFile(szURL))
											printf("%s(%d): WARNING reference to missing file: %s\n", rgszFiles[i], nLine, szURL);
									}
									else
									{
										if (!bQuiet)
											printf("\t%s(%d): Ignoring %s\n", rgszFiles[i], nLine, szURL);
									}
								}
							}
						}

						CloseHandle(hXapFile);
						continue;
					}
				}
			}

			creator.AddFile(rgszFiles[i]);
		}
	}

	if (bMergeMeshFiles)
	{
		// now add the merged mesh files...
		WriteMergedMeshFiles(&creator);
	}

	g_pCreator = &creator;
	qsort(creator.m_directory, creator.m_nNameCount, sizeof (FILENAME), SortDirectoryCompare);
	g_pCreator = NULL;

	XIPHEADER xhdr;
	xhdr.m_dwMagic = MAGIC;
	xhdr.m_wNameCount = (WORD)creator.m_nNameCount;
	xhdr.m_wFileCount = (WORD)creator.m_nFileCount;
	xhdr.m_dwDataStart = sizeof (XIPHEADER) + creator.m_nNameCount * sizeof (FILENAME) + creator.m_cchNames + creator.m_nFileCount * sizeof (FILEDATA);
	xhdr.m_dwDataSize = creator.m_nTotalSize;
	Write(hFile, &xhdr, sizeof (xhdr));

	Write(hFile, creator.m_filedata, creator.m_nFileCount * sizeof (FILEDATA));
	Write(hFile, creator.m_directory, creator.m_nNameCount * sizeof (FILENAME));
	Write(hFile, creator.m_names, creator.m_cchNames);

	DWORD dwDataStart = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	if (bCompress)
	{
		CAppendLZSS lzss;
		lzss.CompressAll(hFile, &creator);
	}
	else
	{
		for (i = 0; i < creator.m_nFileCount; i += 1)
		{
			const char* szName = creator.m_rgszFileName[i];//creator.m_names + creator.m_directory[i].m_wNameOffset;

			if (creator.m_rgszFileName[i] == NULL)
				continue;

			if (!bQuiet)
				printf("\tadding %s\n", szName);

			if (SetFilePointer(hFile, 0, NULL, FILE_CURRENT) - dwDataStart != creator.m_filedata[i].m_dwDataOffset)
			{
				printf("\t\tthat file changed size!\n");
				break;
			}

			if (!AppendFile(hFile, szName))
				break;
		}
	}

	CloseHandle(hFile);
}

void UsageExit()
{
	printf("This tool will create XIP files and list the files within one.\n");
	printf("Usage:\n");
	printf("\txip [-options] <xip-file> <files> ...\n");
	printf("Options:\n");
	printf("\t-c\tCompress\n");
	printf("\t-i <file>\tIgnore references to <file>\n");
	printf("\t-q\tQuiet\n");

	exit(1);
}

int __cdecl main(int argc, char* argv [])
{
	const char* szXipFile = NULL;
	char** rgszAddFile = NULL;
	int nAddFileCount = 0;
	bool bCompress = false;

	for (int i = 1; i < argc; i += 1)
	{
		if (argv[i][0] == '/' || argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			default:
				UsageExit();
				break;

			case 'c':
			case 'C':
				bCompress = true;
				break;

			case 'q':
			case 'Q':
				bQuiet = true;
				break;

			case 'i':
			case 'I':
				{
					char* sz = argv[i] + 2;
					if (*sz == 0)
					{
						i += 1;
						sz = argv[i];
					}
					AddIgnore(sz);
				}
				break;

			case 'm':
			case 'M':
				bMergeMeshFiles = true;
				break;
			}
		}
		else if (szXipFile == NULL)
		{
			szXipFile = argv[i];
			nAddFileCount = argc - (i + 1);
			rgszAddFile = &argv[i + 1];
			break;
		}
	}

	if (szXipFile == NULL)
		UsageExit();

	if (nAddFileCount == 0)
	{
		DumpXIP(szXipFile);
		return 0;
	}

	CreateXIP(szXipFile, nAddFileCount, rgszAddFile, bCompress);

	return 0;
}
