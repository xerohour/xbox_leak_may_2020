#include "std.h"
#include "xapp.h"
#include "node.h"

class CScreenSaver : public CNode
{
	DECLARE_NODE(CScreenSaver, CNode)
public:
	CScreenSaver();
	~CScreenSaver();

	bool m_enabled;
	bool m_isActive;
	bool m_isActive2;
	bool m_isActive3;
	float m_delay;
	float m_delay2;
	float m_delay3;
	float m_shortDelay;

	void reset();

	void Advance(float nSeconds);

	XTIME m_timeOfLastEvent;

	static CScreenSaver* c_pTheScreenSaver;
    bool OnSetProperty(const PRD* pprd, const void* pvValue);

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS()
};

IMPLEMENT_NODE("ScreenSaver", CScreenSaver, CNode)

START_NODE_PROPS(CScreenSaver, CNode)
	NODE_PROP(pt_boolean, CScreenSaver, enabled)
	NODE_PROP(pt_boolean, CScreenSaver, isActive)
	NODE_PROP(pt_boolean, CScreenSaver, isActive2)
	NODE_PROP(pt_boolean, CScreenSaver, isActive3)
	NODE_PROP(pt_number, CScreenSaver, delay)
	NODE_PROP(pt_number, CScreenSaver, delay2)
	NODE_PROP(pt_number, CScreenSaver, delay3)
END_NODE_PROPS()

START_NODE_FUN(CScreenSaver, CNode)
	NODE_FUN_VV(reset)
END_NODE_FUN()

CScreenSaver* CScreenSaver::c_pTheScreenSaver = NULL;
XTIME g_timeOfLastEvent;

CScreenSaver::CScreenSaver() :
	m_enabled(true),
	m_isActive(false),
	m_isActive2(false),
	m_isActive3(false),
	m_delay(300.0f),
	m_delay2(0.0f),
	m_delay3(0.0f)
{
	c_pTheScreenSaver = this;
	m_timeOfLastEvent = XAppGetNow();
}

CScreenSaver::~CScreenSaver()
{
	if (this == c_pTheScreenSaver)
		c_pTheScreenSaver = NULL;
}

void CScreenSaver::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	XTIME now = XAppGetNow();

	if (!m_enabled && m_isActive)
	{
		m_isActive = false;
		CallFunction(this, _T("OnEnd"));
	}

	if (m_enabled && !m_isActive && now >= m_timeOfLastEvent + m_delay)
	{
		m_isActive = true;
		CallFunction(this, _T("OnStart"));
	}

	if (m_delay2 > 0.0f && !m_isActive2 && now >= m_timeOfLastEvent + m_delay2)
	{
		m_isActive2 = true;
		CallFunction(this, _T("OnDelay2"));
	}

	if (m_delay3 > 0.0f && !m_isActive3 && now >= m_timeOfLastEvent + m_delay3)
	{
		m_isActive3 = true;
		CallFunction(this, _T("OnDelay3"));
	}
}

void CScreenSaver::reset()
{
	if (m_isActive)
	{
		m_isActive = false;
		CallFunction(this, _T("OnEnd"));
	}

	m_isActive2 = false;
	m_isActive3 = false;

	m_timeOfLastEvent = XAppGetNow();
}

bool CScreenSaver::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_enabled))
    {
        if (*(bool*)pvValue) reset();
		m_enabled = *(bool*)pvValue;
    }
    return true;
}

bool ResetScreenSaver()
{
	g_timeOfLastEvent = XAppGetNow();

	if (CScreenSaver::c_pTheScreenSaver != NULL)
	{
		bool bRet = CScreenSaver::c_pTheScreenSaver->m_isActive;
		CScreenSaver::c_pTheScreenSaver->reset();

#ifdef _XBOX
		XAutoPowerDownResetTimer();
#endif
		return bRet;
	}

	return false;
}
