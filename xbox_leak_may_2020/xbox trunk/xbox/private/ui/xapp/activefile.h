#pragma once

class CActiveFile
{
public:
	CActiveFile();
	~CActiveFile();

	void Reset();

	bool Fetch(const TCHAR* szURL, bool bSearchAppDir = false, bool bTry = false);
	bool Update();

	const TCHAR* GetURL() const { return m_szURL; }
	DWORD GetContentLength() const { return m_cbContent; }
	const BYTE* GetContent() const { return m_pbContent; }

	BYTE* DetachContent();


	// File-Like Read Functions...

	inline void Rewind()
	{
		Seek(0);
	}

	inline void Seek(int nNewBytePos)
	{
		ASSERT(nNewBytePos >= 0 && (DWORD)nNewBytePos <= m_cbContent);
		m_dwReadPos = nNewBytePos;
	}

	inline void Skip(int nSkipBytes)
	{
		ASSERT((int)m_dwReadPos + nSkipBytes >= 0 && (DWORD)(m_dwReadPos + nSkipBytes) <= m_cbContent);
		m_dwReadPos += nSkipBytes;
	}

	inline int Tell()
	{
		return (int)m_dwReadPos;
	}

	inline bool Read(void* pv, int cb)
	{
		ASSERT(cb >= 0);

		if (m_dwReadPos + (DWORD)cb > m_cbContent)
			return false;

		CopyMemory(pv, m_pbContent + m_dwReadPos, cb);
		m_dwReadPos += cb;

		return true;
	}

	DWORD m_dwReadPos;


#ifdef _UNICODE
	bool IsUnicode();
	void MakeUnicode();
#endif

protected:
	bool FetchFile(bool bTry = false);

	TCHAR* m_szURL;
	BYTE* m_pbContent;
	DWORD m_cbContent;
	FILETIME m_modifiedTime;

	bool m_bInXIP;

	float m_updatePeriod;
	XTIME m_nextUpdateTime;
};
