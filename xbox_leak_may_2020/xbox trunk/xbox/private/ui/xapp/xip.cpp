#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "xip.h"
#include "Mesh.h"
#include <xcrypt.h>

#ifdef COOL_XDASH
#include "Node.h"
#include "Camera.h"
extern CCamera theCamera;
extern UINT g_uMeshRef;
extern D3DXMATRIX g_matProjection;
#endif

extern BOOL g_bEdgeAntialiasOverride;

#define FILE_BUFFER_SIZE 65536

void FileProtectionError()
{
#ifdef _DEBUG
	ALERT(_T("XIP File Protection Error"));
    __asm int 3;
#else
    HalReturnToFirmware(HalFatalErrorRebootRoutine);
#endif
}

CFileBuffer::CFileBuffer()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_pbBuffer = NULL;
	m_cbBuffer = 0;
	m_ibRead = 0;
    m_nBlkCur = 0;
	m_XipSigs = NULL;
}

void CFileBuffer::SetFile(PCTSTR Name, HANDLE hFile)
{
	ASSERT(m_hFile == INVALID_HANDLE_VALUE);
	m_hFile = hFile;
	
    if (m_XipSigs)
    {
        VERIFY(XFreeSectionByHandle(XGetSectionHandle("XIPS")));
    }

	m_XipSigs = (PXIP_PROTECTION)XLoadSection("XIPS");

	if (!m_XipSigs)
    {
		//
		// No Xip Signatures in the XBE.
		//
		ALERT(_T("No XIP section in XBE."));
		FileProtectionError();
	}
	else
    {	
		while (m_XipSigs->SignatureCount != 0 && lstrcmpi(Name + 3, m_XipSigs->Name))
        {
			m_XipSigs = (PXIP_PROTECTION)((PBYTE)(m_XipSigs + 1) +
                (m_XipSigs->SignatureCount * sizeof (XIP_SIG)));
		}

		if (m_XipSigs->SignatureCount == 0)
        {
			//
			// Did not find this XIP file in the XIP section.
			//
			ALERT(_T("File %s not found in XBE Xip section."), Name);
			FileProtectionError();
		}
	}
}

CFileBuffer::~CFileBuffer()
{
	FreeBuffer();

    // Keep the section load counts valid
    if (m_XipSigs)
    {
        VERIFY(XFreeSectionByHandle(XGetSectionHandle("XIPS")));
    }
}

void CFileBuffer::FreeBuffer()
{
	if (m_pbBuffer != NULL)
	{
		VERIFY(VirtualFree(m_pbBuffer, 0, MEM_RELEASE));
		m_pbBuffer = NULL;
	}
}

void CFileBuffer::VerifyXipSignature(PBYTE Buffer, DWORD DataSize)
{
	UCHAR digest[XC_DIGEST_LEN];

	//
	// Check the signature of this 64k block.
	//
	XCCalcDigest(Buffer, DataSize, digest);

	if (!m_XipSigs || (ULONG)m_nBlkCur >= m_XipSigs->SignatureCount ||
        memcmp (digest, ((PXIP_SIG)(m_XipSigs + 1))[m_nBlkCur].Signature,
        XIP_DIGEST_LENGTH))
    {
		TRACE(_T("File modified or corrupt!\n"));
		FileProtectionError();
	}
}

bool CFileBuffer::Seek(int nPos)
{
#if DBG
    static int nLastPos;
    nLastPos = nPos;
#endif

	int nBlock = nPos / FILE_BUFFER_SIZE;

	if (m_pbBuffer == NULL)
	{
        for (;;)
        {
            m_pbBuffer = (BYTE*)VirtualAlloc(NULL, FILE_BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (m_pbBuffer)
            {
                break;
            }
            NewFailed(FILE_BUFFER_SIZE);
        }
		m_nBlkCur = -1;
	}

	if (nBlock != m_nBlkCur)
	{
        ASSERT(m_XipSigs == NULL || (ULONG)nBlock < m_XipSigs->SignatureCount);

		VERIFY(SetFilePointer(m_hFile, nBlock * FILE_BUFFER_SIZE, NULL, FILE_BEGIN) != ~0);
        m_nBlkCur = nBlock;

		DWORD dwRead;
		if (!ReadFile(m_hFile, m_pbBuffer, FILE_BUFFER_SIZE, &dwRead, NULL) || dwRead == 0)
        {
			return false;
        }

        VerifyXipSignature(m_pbBuffer, dwRead);
        ++m_nBlkCur;

		m_cbBuffer = (int)dwRead;
	}

	m_ibRead = nPos % FILE_BUFFER_SIZE;

	return true;
}

int CFileBuffer::Read(void* pv, int cb)
{
	if (m_pbBuffer == NULL)
	{
        for (;;)
        {
    		m_pbBuffer = (BYTE*)VirtualAlloc(NULL, FILE_BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (m_pbBuffer)
            {
                break;
            }
            NewFailed(FILE_BUFFER_SIZE);
        }
	}

	int cbTotalRead = 0;
	int cbRead = 0;
	BYTE* pb = (BYTE*)pv;

	while (cb > 0)
	{
		if (m_ibRead < m_cbBuffer)
		{
			cbRead = m_cbBuffer - m_ibRead;
			if (cbRead > cb)
				cbRead = cb;

			CopyMemory(pb, m_pbBuffer + m_ibRead, cbRead);

			pb += cbRead;
			cb -= cbRead;
			ASSERT(cb >= 0);

			m_ibRead += cbRead;
			cbTotalRead += cbRead;
		}

		if (m_ibRead == m_cbBuffer)
		{
			DWORD dwRead = 0;
			if (!ReadFile(m_hFile, m_pbBuffer, FILE_BUFFER_SIZE, &dwRead, NULL))
            {
                ASSERT(FALSE && "Unable to read from XIP!");
                FileProtectionError();
                return -1;
            }

            if (dwRead == 0)
            {
                return -1;
            }
				
            VerifyXipSignature(m_pbBuffer, dwRead);
            ++m_nBlkCur;

			m_cbBuffer = (int)dwRead;
			m_ibRead = 0;
		}
	}

	return cbTotalRead;
}

CXipFile c_rgXipFile[20];
int c_nXipFileCount = 0;

CXipFile* LoadXIP(const TCHAR* szURL, bool bSync/*=false*/)
{
	TCHAR szBuf [MAX_PATH];

    if (szURL[0] && szURL[1] != ':')
    {
        // REVIEW: all xip files must be on y:\ if szURL is relative
        _tcscpy(szBuf, _T("y:/"));
    }
    else
    {
        szBuf[0] = 0;
    }

    _tcscat(szBuf, szURL);

	{
		char szDirPath [MAX_PATH];
		CleanFilePath(szDirPath, szBuf);
		char* pch = strrchr(szDirPath, '.');
		ASSERT(pch != NULL);
		*pch = 0;

		for (int i = 0; i < c_nXipFileCount; i += 1)
		{
			if (_stricmp(szDirPath, c_rgXipFile[i].m_szDirPath) == 0)
			{
//				TRACE(_T("\002XIP %s is already loaded!\n"), szURL);
				
				c_rgXipFile[i].m_cacheTime = XAppGetNow();
				if (c_rgXipFile[i].m_bLoaded && c_rgXipFile[i].m_nVertexBufferCount > 0 && c_rgXipFile[i].m_rgMeshBuffer[0].m_pVertexBuffer == NULL)
					c_rgXipFile[i].ReloadMeshBuffers();

				return &c_rgXipFile[i];
			}
		}
	}

//	TRACE(_T("\003LoadXIP: %s\n"), szURL);

	ASSERT(c_nXipFileCount < countof(c_rgXipFile));
	CXipFile* pXipFile = &c_rgXipFile[c_nXipFileCount];

	if (!pXipFile->Open(szBuf))
    {
		return NULL;
    }

	c_nXipFileCount += 1;

	if (bSync)
	{
		pXipFile->Load();
	}
	else
	{
		DWORD dwThreadID;
		HANDLE hThread = CreateThread(NULL, 0, CXipFile::StartLoadThread, pXipFile, 0, &dwThreadID);
		if (hThread)
        {
            CloseHandle(hThread);
        }
        else
        {
            CXipFile::StartLoadThread(pXipFile);
        }
	}

	return pXipFile;
}

bool FindInXIPAndDetach(const TCHAR* szURL, BYTE*& pbContent, DWORD& cbContent)
{
	char szFilePath [MAX_PATH];
	CleanFilePath(szFilePath, szURL);

	for (int i = c_nXipFileCount - 1; i >= 0; i -= 1)
	{
		if (c_rgXipFile[i].m_bLocked || !c_rgXipFile[i].m_bLoaded)
			continue;

		int nObject = c_rgXipFile[i].Find(szFilePath);
		if (nObject >= 0 && c_rgXipFile[i].m_objects[nObject] != NULL)
		{
			pbContent = (BYTE*)c_rgXipFile[i].m_objects[nObject];
			cbContent = c_rgXipFile[i].m_filedata[nObject].m_dwSize;
			c_rgXipFile[i].m_objects[nObject] = NULL;
			return true;
		}
	}

	return false;
}

void* FindObjectInXIP(const TCHAR* szURL, const TCHAR* szFilename, int nType/*=-1*/)
{
	char szFilePath [MAX_PATH];
	CleanFilePath(szFilePath, szURL);

	for (int i = c_nXipFileCount - 1; i >= 0; i -= 1)
	{
		if (c_rgXipFile[i].m_bLocked || !c_rgXipFile[i].m_bLoaded)
        {
			continue;
        }

		if(i == 0 && nType == XIP_TYPE_TEXTURE)
		{
			TCHAR szBuf [MAX_PATH];
			TCHAR szBufName [MAX_PATH];
			_tcscpy(szBufName, szFilename);
#ifdef COOL_XDASH
			_tcscpy(_tcsrchr(szBufName, '.') + 1, _T("xbx.cool"));
#else
			_tcscpy(_tcsrchr(szBufName, '.') + 1, _T("xbx"));
#endif
			_tcscpy(szBuf, theApp.m_szAppDir);
			_tcscat(szBuf, szBufName);
			CleanFilePath(szFilePath, szBuf);
			void* pObject = c_rgXipFile[i].FindObject(szFilePath, nType);

			if (pObject != NULL)
				return pObject;
		}
		else
		{
			void* pObject = c_rgXipFile[i].FindObject(szFilePath, nType);

			if (pObject != NULL)
				return pObject;
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////

CXipFile::CXipFile()
{
	m_szXipFileName = NULL;
	m_szDirPath = NULL;
	m_bLoaded = false;
	m_bLocked = false;
	m_bReloading = false;

	ZeroMemory(m_rgMeshBuffer, sizeof (m_rgMeshBuffer));
	m_nVertexBufferCount = 0;
	m_nIndexBufferCount = 0;
}

CXipFile::~CXipFile()
{
	delete [] m_szXipFileName;
	m_szXipFileName = NULL;

	delete [] m_szDirPath;
	m_szDirPath = NULL;

	delete [] m_filedata;
	m_filedata = NULL;

	delete [] m_directory;
	m_directory = NULL;

	delete [] m_names;
	m_names = NULL;

	DeleteMeshBuffers();

	ZeroMemory(m_rgMeshBuffer, sizeof (m_rgMeshBuffer));
	m_nVertexBufferCount = 0;
	m_nIndexBufferCount = 0;
}

bool CXipFile::Open(const TCHAR* szXipFileName)
{
/*
	HANDLE hFile;
	if ((hFile = XAppCreateFile(szXipFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING, NULL)) == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001CXipFile::Open (%s) failed %d\n"), szXipFileName, GetLastError());
		return false;
	}

	m_file.SetFile(hFile);
*/
	m_szXipFileName = new TCHAR [_tcslen(szXipFileName) + 1];
	_tcscpy(m_szXipFileName, szXipFileName);

	char szDirPath [MAX_PATH];
	CleanFilePath(szDirPath, szXipFileName);

	char* pch = strrchr(szDirPath, '.');
	ASSERT(pch != NULL);
	*pch = 0;

	pch = strrchr(szDirPath, '\\');
	if (pch == NULL)
		pch = szDirPath;
	else
		pch += 1;

	if (_stricmp(pch, "default") == 0)
	{
		if (pch > szDirPath)
			pch -= 1;
		*pch = 0;
	}

	m_szDirPath = new char [strlen(szDirPath) + 1];
	strcpy(m_szDirPath, szDirPath);

	return true;
}

DWORD CALLBACK CXipFile::StartLoadThread(LPVOID pvContext)
{
	START_PROFILE();
	CXipFile *pThis = (CXipFile*)pvContext;

	if (pThis->m_bReloading)
		pThis->ReloadMeshBuffers();
	else
		VERIFY(pThis->Load());

	END_PROFILE();
	
	return 0;
}

bool CXipFile::Load()
{
#ifdef _DEBUG
	TCHAR szDirPath [MAX_PATH];
	Unicode(szDirPath, m_szDirPath, MAX_PATH);
	DWORD ticks = GetTickCount ();
#endif

	HANDLE hFile;
	XCALCSIG_SIGNATURE sig;
	

	if ((hFile = XAppCreateFile(m_szXipFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING, NULL)) == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("\001CXipFile::Open (%s) failed %d\n"), m_szXipFileName, GetLastError());
		return false;
	}



	m_file.SetFile(m_szXipFileName, hFile);
	if (!m_file.Read(&m_header, sizeof (XIPHEADER)))
		return false;

	if (m_header.m_dwMagic != XIP_MAGIC)
	{
#ifdef _DEBUG
		TRACE(_T("\001CXipFile::Open (%s.xip) not a valid XIP file!\n"), szDirPath);
#endif
		return false;
	}

	m_filedata = new FILEDATA [m_header.m_wFileCount];
	if (!m_file.Read(m_filedata, m_header.m_wFileCount * sizeof (FILEDATA)))
		return false;
	m_directory = new FILENAME [m_header.m_wNameCount];
	if (!m_file.Read(m_directory, m_header.m_wNameCount * sizeof (FILENAME)))
		return false;


	{
		int cbNames = m_header.m_dwDataStart - (sizeof (XIPHEADER) + m_header.m_wFileCount * sizeof (FILEDATA) + m_header.m_wNameCount * sizeof (FILENAME));
		m_names = new char [cbNames];
		if (!m_file.Read(m_names, cbNames))
			return false;


	}

	ASSERT(GetFileSize(m_file.m_hFile, NULL) == m_header.m_dwDataStart + m_header.m_dwDataSize);

	CreateObjects();

	m_bLoaded = true;

	return true;
}

LPDIRECT3DTEXTURE8 ReadTexture(CFileBuffer& file, int nBytes)
{
	BYTE* pbContent = new BYTE[nBytes];
	file.Read(pbContent, nBytes);

	const XPR_HEADER* pxprh = (const XPR_HEADER*)pbContent;
	if (pxprh->dwMagic == XPR_MAGIC_VALUE)
	{
		int cbHeaders = pxprh->dwHeaderSize - sizeof (XPR_HEADER);
		int cbData = pxprh->dwTotalSize - pxprh->dwHeaderSize;

		IDirect3DTexture8* pTexture = (IDirect3DTexture8*)XAppD3D_AllocNoncontiguousMemory(sizeof (D3DBaseTexture));
		if (pTexture == NULL)
		{
			TRACE(_T("Not enough memory to load XBX image file!\n"));
			return NULL;
		}

		CopyMemory(pTexture, pbContent + sizeof (XPR_HEADER), sizeof (IDirect3DTexture8));

		BYTE* pbData = (BYTE*)XAppD3D_AllocContiguousMemory(cbData, D3DTEXTURE_ALIGNMENT);
		if (pbData == NULL)
		{
			// TODO: Leaking pTexture!
			TRACE(_T("Not enough memory to load XBX image file!\n"));
			return NULL;
		}

		CopyMemory(pbData, pbContent + pxprh->dwHeaderSize, cbData);
		D3D_CopyContiguousMemoryToVideo(pbData);

		pTexture->Data = NULL;
		pTexture->Register(pbData);
		pTexture->Common |= D3DCOMMON_D3DCREATED;

		return pTexture;
	}
	else
	{
		ALERT(_T("Unable to load XBX file for scene!"));
		return NULL;
	}
}

void CXipFile::CreateObjects()
{
	m_bLocked = true;

	m_objects = new void* [m_header.m_wFileCount];
	ZeroMemory(m_objects, sizeof (void*) * m_header.m_wFileCount);

	for (UINT i = 0; i < m_header.m_wFileCount; i += 1)
	{
		switch (m_filedata[i].m_dwType)
		{
		default:
			m_objects[i] = XAppAllocMemory(m_filedata[i].m_dwSize);
			m_file.Read(m_objects[i], m_filedata[i].m_dwSize);			
			
			break;

		case XIP_TYPE_MESH:
			ASSERT(FALSE); // Obsolete...
//			m_objects[i] = CreateMesh(m_hFile);
			break;

		case XIP_TYPE_MESH_REFERENCE:
			{
				CMeshRef* pMeshRef = new CMeshRef;
				pMeshRef->m_pXipFile = this;
				pMeshRef->m_nMeshBuffer = m_filedata[i].m_dwDataOffset >> 24;
				ASSERT(pMeshRef->m_nMeshBuffer < MAX_MESHBUFFER);
				pMeshRef->m_nFirstIndex = m_filedata[i].m_dwDataOffset & 0x00ffffff;
				pMeshRef->m_nPrimitiveCount = m_filedata[i].m_dwSize;
				m_objects[i] = pMeshRef;
			}
			break;

		case XIP_TYPE_TEXTURE:
			m_objects[i] = ReadTexture(m_file, m_filedata[i].m_dwSize);
			
			break;

		case XIP_TYPE_INDEXBUFFER:
			ASSERT(m_nIndexBufferCount < MAX_MESHBUFFER);
			ReadIndexBuffer(i, m_nVertexBufferCount);
			m_nIndexBufferCount += 1;
			break;

		case XIP_TYPE_VERTEXBUFFER:
			ASSERT(m_nVertexBufferCount < MAX_MESHBUFFER);
			ReadVertexBuffer(i, m_nVertexBufferCount);
			m_nVertexBufferCount += 1;
			break;
		}
	}

	ASSERT(m_nVertexBufferCount == m_nIndexBufferCount);

	m_bLocked = false;
}

struct SEARCHXIP
{
	SEARCHXIP(CXipFile* pXipFile, const char* szFind)
	{
		m_names = pXipFile->m_names;
		m_szFind = szFind;
	}

	const char* m_names;
	const char* m_szFind;
};

static int __cdecl SearchXipCompare(const void *elem1, const void *elem2)
{
	const SEARCHXIP* pSearch = (const SEARCHXIP*)elem1;
	const FILENAME* pName = (const FILENAME*)elem2;
	return _stricmp(pSearch->m_szFind, pSearch->m_names + pName->m_wNameOffset);
}

int CXipFile::Find(const char* szURL)
{
	int cchDirPath = strlen(m_szDirPath);

    if (_strnicmp(szURL, m_szDirPath, cchDirPath) != 0)
        return -1;

    const char* szFile = szURL + cchDirPath;

    if (*szFile != '\\')
        return -1;

    szFile += 1;

	SEARCHXIP searchxip(this, szFile);
	FILENAME* pFileName = (FILENAME*)bsearch(&searchxip, m_directory, m_header.m_wNameCount, sizeof (FILENAME), SearchXipCompare);

	if (pFileName == NULL)
		return -1;

	return m_directory[(int)((BYTE*)pFileName - (BYTE*)m_directory) / sizeof (FILENAME)].m_wFileDataIndex;
}

void* CXipFile::FindObject(const char* szURL, int nType/*=-1*/)
{
	int nObject = Find(szURL);
    if (nObject == -1)
	{
        return NULL;
	}

    if (nType != -1 && m_filedata[nObject].m_dwType != (DWORD)nType)
	{
        return NULL;
	}

    //
    //  Bug 7092 - AddRef Textures when we give them out
    //             such that they don't get deleted
    //             when the caller is done with them.
    if(XIP_TYPE_TEXTURE == nType)
    {
        ((LPDIRECT3DTEXTURE8)m_objects[nObject])->AddRef();
    }

    return m_objects[nObject];
}

void CXipFile::DeleteMeshBuffers()
{
	ASSERT(!m_bLocked); // thread synchronization issue!

//#ifdef _DEBUG
//	TCHAR szDirPath [MAX_PATH];
//	Unicode(szDirPath, m_szDirPath, MAX_PATH);
//	TRACE(_T("\002DeleteMeshBuffers: %s.xip\n"), szDirPath);
//#endif

	for (int i = 0; i < MAX_MESHBUFFER; i += 1)
	{
		if (m_rgMeshBuffer[i].m_pVertexBuffer != NULL)
		{
            m_rgMeshBuffer[i].m_pVertexBuffer->Release();
            m_rgMeshBuffer[i].m_pVertexBuffer = NULL;
		}

		if (m_rgMeshBuffer[i].m_pIndexBuffer != NULL)
		{
            m_rgMeshBuffer[i].m_pIndexBuffer->Release();
            m_rgMeshBuffer[i].m_pIndexBuffer = NULL;
		}
	}
}

bool CleanupMeshCache()
{
//	TRACE(_T("\002CleanupMeshCache\n"));

	CXipFile* pOldOne = NULL;

	for (int i = 0; i < c_nXipFileCount; i += 1)
	{
//#ifdef _DEBUG
//		TCHAR szDirPath [MAX_PATH];
//		Unicode(szDirPath, c_rgXipFile[i].m_szDirPath, MAX_PATH);
//		TRACE(_T("\002\tchecking: %s.xip (%f)\n"), szDirPath, c_rgXipFile[i].m_cacheTime);
//#endif
		if (c_rgXipFile[i].m_bLocked)
		{
//			TRACE(_T("\002\t\tlocked\n"));
			continue;
		}

		if (!c_rgXipFile[i].m_bLoaded)
		{
//			TRACE(_T("\002\t\tnot loaded\n"));
			continue;
		}

		if (c_rgXipFile[i].m_nVertexBufferCount == 0)
		{
//			TRACE(_T("\002\t\tno vertex buffers\n"));
			continue;
		}

		if (c_rgXipFile[i].m_rgMeshBuffer[0].m_pVertexBuffer == NULL)
		{
//			TRACE(_T("\002\t\tunloaded\n"));
			continue;
		}

		if (pOldOne == NULL || c_rgXipFile[i].m_cacheTime < pOldOne->m_cacheTime)
			pOldOne = &c_rgXipFile[i];
	}

	if (pOldOne == NULL)
		return false;

	pOldOne->DeleteMeshBuffers();

	return true;
}

void CXipFile::ReadIndexBuffer(int nFileIndex, int nIndexBuffer)
{
	CMeshBuffer* pMeshBuffer = &m_rgMeshBuffer[nIndexBuffer];

	XAppCreateIndexBuffer(m_filedata[nFileIndex].m_dwSize, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pMeshBuffer->m_pIndexBuffer);

	BYTE* indices;
#ifdef _XBOX
	const DWORD dwLockFlags = D3DLOCK_DISCARD | D3DLOCK_NOFLUSH;
#else
	const DWORD dwLockFlags = D3DLOCK_DISCARD;
#endif
	VERIFYHR(pMeshBuffer->m_pIndexBuffer->Lock(0, m_filedata[nFileIndex].m_dwSize, (BYTE**)&indices, dwLockFlags));
	m_file.Read(indices, m_filedata[nFileIndex].m_dwSize);
	VERIFYHR(pMeshBuffer->m_pIndexBuffer->Unlock());

	pMeshBuffer->m_nIndexCount = m_filedata[nFileIndex].m_dwSize /  sizeof (WORD);
}

void CXipFile::ReadVertexBuffer(int nFileIndex, int nVertexBuffer)
{
	CMeshBuffer* pMeshBuffer = &m_rgMeshBuffer[nVertexBuffer];

	int nVertexCount;
	DWORD fvf;

	m_file.Read(&nVertexCount, sizeof (int));
	m_file.Read(&fvf, sizeof (DWORD));
	
	pMeshBuffer->m_nVertexStride = (m_filedata[nFileIndex].m_dwSize - 8) / nVertexCount;

	XAppCreateVertexBuffer(m_filedata[nFileIndex].m_dwSize - 8, D3DUSAGE_DYNAMIC, fvf, D3DPOOL_DEFAULT, &pMeshBuffer->m_pVertexBuffer);

	
	BYTE* verts;
#ifdef _XBOX
	const DWORD dwLockFlags = D3DLOCK_DISCARD | D3DLOCK_NOFLUSH;
#else
	const DWORD dwLockFlags = D3DLOCK_DISCARD;
#endif
	VERIFYHR(pMeshBuffer->m_pVertexBuffer->Lock(0, 0, &verts, dwLockFlags));
	m_file.Read(verts, m_filedata[nFileIndex].m_dwSize - 8);
	VERIFYHR(pMeshBuffer->m_pVertexBuffer->Unlock());

	pMeshBuffer->m_fvf = fvf;
	pMeshBuffer->m_nVertexCount = nVertexCount;
}

void CXipFile::ReloadMeshBuffers()
{
//#ifdef _DEBUG
//	TCHAR szDirPath [MAX_PATH];
//	Unicode(szDirPath, m_szDirPath, MAX_PATH);
//	TRACE(_T("\002ReloadMeshBuffers: %s.xip\n"), szDirPath);
//#endif

	ASSERT(!m_bLocked);
	m_bLocked = true;

	int nIndexBuffer = 0;
	int nVertexBuffer = 0;

	for (UINT i = 0; i < m_header.m_wFileCount; i += 1)
	{
		switch (m_filedata[i].m_dwType)
		{
		case XIP_TYPE_INDEXBUFFER:
			ASSERT(nIndexBuffer < m_nIndexBufferCount);
			m_file.Seek(m_header.m_dwDataStart + m_filedata[i].m_dwDataOffset);
			ReadIndexBuffer(i, nIndexBuffer);
			nIndexBuffer += 1;
			break;

		case XIP_TYPE_VERTEXBUFFER:
			ASSERT(nVertexBuffer < m_nVertexBufferCount);
			m_file.Seek(m_header.m_dwDataStart + m_filedata[i].m_dwDataOffset);
			ReadVertexBuffer(i, nVertexBuffer);
			nVertexBuffer += 1;
			break;
		}
	}

	ASSERT(nIndexBuffer == m_nIndexBufferCount);
	ASSERT(nVertexBuffer == m_nVertexBufferCount);

	m_bLocked = false;
	m_bReloading = false;
}

void CXipFile::Reload()
{
	ASSERT(!m_bReloading);

	m_bReloading = true;
	DWORD dwThreadID;
	HANDLE hThread = CreateThread(NULL, 0, CXipFile::StartLoadThread, this, 0, &dwThreadID);
    if (hThread)
    {
        CloseHandle(hThread);
    }
    else
    {
        StartLoadThread(this);
    }
}

bool CXipFile::IsUnloaded() const
{
	if (!m_bLoaded)
		return false;

	if (m_bReloading)
		return false;

	if (m_nVertexBufferCount == 0)
		return false;

	if (m_rgMeshBuffer[0].m_pVertexBuffer != NULL)
		return false;

	return true;
}

bool CXipFile::IsReloading() const
{
	return m_bReloading;
}

void CMeshRef::Render(bool bSetFVF/*=true*/)
{
	ASSERT(m_nMeshBuffer < m_pXipFile->m_nVertexBufferCount);
	CMeshBuffer* pMeshBuffer = &m_pXipFile->m_rgMeshBuffer[m_nMeshBuffer];

	m_pXipFile->m_cacheTime = XAppGetNow();

	if (m_pXipFile->IsReloading())
		return;

	if (pMeshBuffer->m_pVertexBuffer == NULL)
	{
		m_pXipFile->Reload();
		return;
	}

	if (bSetFVF)
		XAppSetVertexShader(GetFixedFunctionShader(pMeshBuffer->m_fvf));

	if (m_nPrimitiveCount > 800 && !g_bEdgeAntialiasOverride) {
		XAppSetRenderState(D3DRS_EDGEANTIALIAS, FALSE);
		XAppSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		//XAppSetRenderState(D3DRS_MULTISAMPLETYPE, D3DMULTISAMPLE_4_SAMPLES_MULTISAMPLE_GAUSSIAN);
	}

#ifdef COOL_XDASH

    if (g_uMeshRef++ == 0) {

        // Turn off breathing for the background sphere in the ugliest way imaginable...

        D3DXMATRIX matPosition, matView, mat, worldView;
	    D3DXMatrixAffineTransformation(&matPosition, 1.0f, NULL, &theCamera.m_orientation, &theCamera.m_position);
	    D3DXMatrixInverse(&matView, NULL, &matPosition);
	    matView._31 = -matView._31;
	    matView._32 = -matView._32;
	    matView._33 = -matView._33;
	    matView._34 = -matView._34;

	    D3DXMatrixMultiply(&worldView, XAppGetWorld(), &matView);

	    // World/View/Projection
	    D3DXMatrixMultiply(&mat, &worldView, &g_matProjection);
	    D3DXMatrixTranspose(&mat, &mat);
	    XAppSetVertexShaderConstant(0, &mat(0,0), 4);

	    // Position Transform
	    D3DXMatrixTranspose(&mat, &worldView);
	    XAppSetVertexShaderConstant(10, &mat(0,0), 4);

	    // Normal Transform
	    D3DXMatrixInverse(&mat, NULL, &worldView);
	    XAppSetVertexShaderConstant(5, &mat(0,0), 4);

	    D3DXVECTOR4 lightDir(1.0f, 1.0f, -1.0f, 0.0f);
	    D3DXVec4Normalize(&lightDir, &lightDir);
	    D3DXMatrixTranspose(&mat, &worldView);
	    D3DXVec3TransformNormal((D3DXVECTOR3*)&lightDir, (D3DXVECTOR3*)&lightDir, &mat);
	    D3DXVec4Normalize(&lightDir, &lightDir);
	    XAppSetVertexShaderConstant(4, &lightDir, 1);
    }

#endif

	XAppSetStreamSource(0, pMeshBuffer->m_pVertexBuffer, pMeshBuffer->m_nVertexStride);
	XAppSetIndices(pMeshBuffer->m_pIndexBuffer, 0);

	XAppDrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, pMeshBuffer->m_nVertexCount, m_nFirstIndex, m_nPrimitiveCount);
}

DWORD CMeshRef::GetFVF() const
{
	ASSERT(m_nMeshBuffer < m_pXipFile->m_nVertexBufferCount);
	return m_pXipFile->m_rgMeshBuffer[m_nMeshBuffer].m_fvf;
}
