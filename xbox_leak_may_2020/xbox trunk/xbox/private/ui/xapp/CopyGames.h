#pragma once

class CGameCopier
{
public:
	CGameCopier();
	~CGameCopier();

	void SetSource(int nDevUnit);
	void SetDestination(int nDevUnit);
	void AddGame(const TCHAR* szTitleID, const TCHAR* szGameDir, FILETIME saveTime, int nBlocks);

	void Start();
	void Finish();

	float m_progress;
	bool m_error;
	bool m_done;

	bool DeleteDirectory(const TCHAR* szDir, bool RemoveSelf = true);

private:
	HANDLE m_hThread;
	int m_srcDevUnit;
	int m_destDevUnit;
	TCHAR* m_szSrcRoot;
	TCHAR* m_szDestRoot;
	struct CCopyGame* m_rgCopyGame;
	int m_nCopyGameCount;
	int m_nCopyGameAlloc;
	BYTE* m_buffer;
	bool m_bInternalError;
    bool m_bAlreadyExists;

	int m_nCopyGameCur;
	int m_nTotalBlocks;
	int m_nCopiedBlocks;

	DWORD ThreadProc();
	void CopyGame(int nCopyGame/*const TCHAR* szTitleID, const TCHAR* szGameDir*/);
	bool CreateDirectory(const TCHAR* szDir);
	bool CopyDirectory(const TCHAR* szSrcDir, const TCHAR* szDestDir);
	bool CopyFile(const TCHAR* szSrcFile, const TCHAR* szDestFile);
	bool DeleteFile(const TCHAR* szFile);
	void RemoveAll();

	static DWORD CALLBACK StartThread(LPVOID pvContext);
};

