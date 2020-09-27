class CPlatNameConvert
{
public:
	void Init();
	void UItoLong(const CString& strUI, CString& strLong);
	void LongToUI(const CString& strLong, CString& strUI);

protected:
	CMapStringToString m_NameMap;
};
