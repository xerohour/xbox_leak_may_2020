namespace ATL
{

inline void CIncludeServerContext::CrackTag()
{
	if (m_bTagCracked)
		return;

	LPCSTR szStencilBegin = m_szTagBegin;
	LPCSTR szStencilEnd = m_szTagEnd;
	LPCSTR szParamBegin = m_szTagBegin;
	LPCSTR szParamEnd = m_szTagEnd;

	while (*szParamBegin && *szParamBegin != '?' && *szParamBegin != '}')
		szParamBegin = CharNextExA(GetCodePage(), szParamBegin, 0);

	if (*szParamBegin=='?')
	{
		szStencilEnd = szParamBegin-1;
		szParamBegin = CharNextExA(GetCodePage(), szParamBegin, 0);
	}
	
	if (szParamBegin > szParamEnd)
		szParamBegin = szParamEnd + 1;

	CHAR szFileNameRelative[MAX_PATH];

	memcpy(szFileNameRelative, szStencilBegin, szStencilEnd-szStencilBegin+1);
	szFileNameRelative[szStencilEnd-szStencilBegin+1] = '\0';

	memcpy(m_szQueryString, szParamBegin, szParamEnd-szParamBegin+1);
	m_szQueryString[szParamEnd-szParamBegin+1] = '\0';

	if (!IsFullPathA(szFileNameRelative))
	{
		CHAR szTemp[MAX_PATH*2];
		strcpy(szTemp, GetBaseDir());
		strcat(szTemp, szFileNameRelative);
		PathCanonicalizeA(m_szFileName, szTemp);
	}
	else
	{
		strcpy(m_szFileName, szFileNameRelative);
	}

	m_bTagCracked = true;
}

inline WORD CIncludeServerContext::GetCodePage()
{
	if (m_pStencil)
		return m_pStencil->GetCodePage();
	return m_nCodePage;
}

inline LPCSTR CIncludeServerContext::GetBaseDir()
{
	if (m_pStencil)
		return m_pStencil->GetBaseDir();
	return m_szBaseDir;
}

} // namespace ATL