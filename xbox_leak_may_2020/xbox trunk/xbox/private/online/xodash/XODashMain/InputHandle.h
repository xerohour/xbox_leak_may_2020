class CInputHandle
{
public: 
	CInputHandle(): m_bDirty(false), m_nCurLen(MAX_INPUT_TEXT_SIZE) {}
	inline void SetInputText(const TCHAR* szSource);
	inline void SetInputMaxLength(size_t nLen);
	inline size_t GetInputMaxLength() { return m_nCurLen;}
	inline void ResetInputText() {ZeroMemory(m_szBuffer,(MAX_INPUT_TEXT_SIZE+1)*sizeof(TCHAR)); m_bDirty = false; m_nCurLen = MAX_INPUT_TEXT_SIZE;}
	inline bool Empty() {return (_tcslen(m_szBuffer) == 0);}
	inline bool IsNewInput(){return m_bDirty;}
	inline const TCHAR* GetInputText() { return m_szBuffer;}
	void Dirty(bool bDirty) { m_bDirty = bDirty;}
private:
	TCHAR m_szBuffer[MAX_INPUT_TEXT_SIZE+1];
	size_t m_nCurLen;
	bool m_bDirty;
};

void CInputHandle::SetInputText(const TCHAR* szSource)
{
	ASSERT(szSource && _tcslen(szSource) <= m_nCurLen );
	if(szSource)
	{
		ZeroMemory(m_szBuffer,(MAX_INPUT_TEXT_SIZE+1)*sizeof(TCHAR) );
		_tcsncpy(m_szBuffer, szSource, min(_tcslen(szSource), m_nCurLen));
	}

}

void CInputHandle::SetInputMaxLength(size_t nLen) 
{ 
	nLen <= MAX_INPUT_TEXT_SIZE? m_nCurLen = nLen: m_nCurLen = MAX_INPUT_TEXT_SIZE; 
}