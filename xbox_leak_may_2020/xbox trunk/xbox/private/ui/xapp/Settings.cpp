#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"
#include "SettingsFile.h"

class CSettings;
class CSetting;


class CSetting : public CObject
{
public:
	CSetting();
	~CSetting();

	void Assign(CObject* pObject);
	CStrObject* ToStr();
	CObject* Deref();

	CSettings* m_pSettings;
	TCHAR* m_szName;
};


class CSettings : public CNode
{
	DECLARE_NODE(CSettings, CNode)
public:
	CSettings();
	~CSettings();

	CObject* Dot(CObject* pObject);

	TCHAR* m_file;
	TCHAR* m_section;

	void SetValue(const TCHAR* szName, const TCHAR* szValue);
	CStrObject* GetValue(const TCHAR* szName);

protected:
	CSettingsFile m_sfile;

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS()
};

////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("Settings", CSettings, CNode)

START_NODE_PROPS(CSettings, CNode)
	NODE_PROP(pt_string, CSettings, file)
	NODE_PROP(pt_string, CSettings, section)
END_NODE_PROPS()

START_NODE_FUN(CSettings, CNode)
	NODE_FUN_SS(GetValue)
	NODE_FUN_VSS(SetValue)
END_NODE_FUN()

CSettings::CSettings()
{
	m_file = NULL;
	m_section = NULL;
}

CSettings::~CSettings()
{
	delete [] m_file;
	delete [] m_section;
}

CObject* CSettings::Dot(CObject* pObj)
{
	CObject* pObject = CNode::Dot(pObj);
	if (pObject != NULL)
		return pObject;

	if (pObj->m_obj == objVariable)
	{
		CVarObject* pVar = (CVarObject*)pObj;

		CSetting* pSetting = new CSetting;
		pSetting->m_pSettings = this;
		int cch = _tcslen(pVar->m_sz) + 1;
		pSetting->m_szName = new TCHAR [cch];
		CopyChars(pSetting->m_szName, pVar->m_sz, cch);
		return pSetting;
	}

	return NULL;
}

void CSettings::SetValue(const TCHAR* szName, const TCHAR* szValue)
{
	if (m_sfile.GetFileName() == NULL || _tcsicmp(m_sfile.GetFileName(), m_file) != 0)
	{
		m_sfile.Close();
		m_sfile.Open(m_file);
	}

	m_sfile.SetValue(m_section, szName, szValue);
}

CStrObject* CSettings::GetValue(const TCHAR* szName)
{
	if (m_sfile.GetFileName() == NULL || _tcsicmp(m_sfile.GetFileName(), m_file) != 0)
	{
		m_sfile.Close();
		if (!m_sfile.Open(m_file))
			return new CStrObject(_T(""));
	}

	TCHAR szBuf [1024];
	m_sfile.GetValue(m_section, szName, szBuf, countof(szBuf));
	return new CStrObject(szBuf);
}

////////////////////////////////////////////////////////////////////////////

CSetting::CSetting()
{
	m_pSettings = NULL;
	m_szName = NULL;
}

CSetting::~CSetting()
{
	delete [] m_szName;
}

void CSetting::Assign(CObject* pObject)
{
	CStrObject* pStrObject = pObject->Deref()->ToStr();
	const TCHAR* sz = pStrObject->GetSz();

	m_pSettings->SetValue(m_szName, sz);
}

CStrObject* CSetting::ToStr()
{
	return m_pSettings->GetValue(m_szName);
}

CObject* CSetting::Deref()
{
	CObject* pObject = m_pSettings->GetValue(m_szName);
	Release();
	return pObject;
}
