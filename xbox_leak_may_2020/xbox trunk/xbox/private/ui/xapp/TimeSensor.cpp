#include "std.h"
#include "xapp.h"
#include "Node.h"

class CTimeSensor : public CTimeDepNode
{
	DECLARE_NODE(CTimeSensor, CTimeDepNode)
public:
	CTimeSensor();

	float m_cycleInterval;
	bool m_enabled;
	XTIME m_cycleTime;
	float m_fraction_changed;
	XTIME m_time;

	void Advance(float nSeconds);
	void OnIsActiveChanged();
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("TimeSensor", CTimeSensor, CTimeDepNode)

START_NODE_PROPS(CTimeSensor, CTimeDepNode)
	NODE_PROP(pt_number, CTimeSensor, cycleInterval)
	NODE_PROP(pt_boolean, CTimeSensor, enabled)
	NODE_PROP(pt_number, CTimeSensor, cycleTime)
	NODE_PROP(pt_number, CTimeSensor, fraction_changed)
	NODE_PROP(pt_number, CTimeSensor, time)
END_NODE_PROPS()

CTimeSensor::CTimeSensor() :
	m_cycleInterval(1.0f),
	m_enabled(true)
{
}

bool CTimeSensor::OnSetProperty(const PRD* pprd, const void* pvValue)
{
    if ((int)pprd->pbOffset == offsetof(m_time))
    {
        m_time = (XTIME)(*(float*)pvValue);
        return false;
    }

    return true;
}

void CTimeSensor::Advance(float nSeconds)
{
	XTIME now = XAppGetNow();

	CTimeDepNode::Advance(nSeconds);

	if (m_isActive)
	{
		m_time = now;

		if (now >= m_cycleTime + m_cycleInterval)
		{
			if (m_loop)
			{
				m_cycleTime += m_cycleInterval;
			}
			else
			{
				m_isActive = false;
				OnIsActiveChanged();
			}
		}

		float temp = (float) (now - m_startTime) / m_cycleInterval;

		m_fraction_changed = temp - (int)temp;
		if (m_fraction_changed == 0.0f && now > m_startTime)
			m_fraction_changed = 1.0f;
		CallFunction(this, _T("fraction_changed"));
	}
}

void CTimeSensor::OnIsActiveChanged()
{
	if (m_isActive)
	{
		m_cycleTime = XAppGetNow();
	}
	else
	{
		CallFunction(this, _T("OnActiveChanged"));
	}
}
