#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "surfx.h"


class CDotField : public CNode
{
	DECLARE_NODE(CDotField, CNode)
public:
	CDotField();
	~CDotField();

	float m_spacing;
	float m_hSpeed;
	float m_vSpeed;

	void Advance(float nSeconds);
	void RenderDynamicTexture(CSurfx* pSurfx);

protected:
	float m_xStart;
	float m_yStart;
	float m_xSpeed;
	float m_ySpeed;
	float m_newSpacing;
	float m_oldSpacing;

	XTIME m_nextChangeTime;
	XTIME m_lastChangeTime;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("DotField", CDotField, CNode)

START_NODE_PROPS(CDotField, CNode)
	NODE_PROP(pt_number, CDotField, spacing)
	NODE_PROP(pt_number, CDotField, hSpeed)
	NODE_PROP(pt_number, CDotField, vSpeed)
END_NODE_PROPS()

CDotField::CDotField() :
	m_spacing(8.0f),
	m_hSpeed(0.0f),
	m_vSpeed(0.0f)
{
	m_xStart = 0.0f;
	m_yStart = 0.0f;
	m_xSpeed = 0.0f;
	m_ySpeed = 0.0f;
	m_nextChangeTime = 0.0f;
	m_lastChangeTime = 0.0f;
}

CDotField::~CDotField()
{
}

void CDotField::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_hSpeed == 0.0f && m_vSpeed == 0.0f)
	{
		if (XAppGetNow() >= m_nextChangeTime)
		{
			m_xSpeed = (rnd(2.0f) - 1.0f) * m_spacing * 4.0f;
			m_ySpeed = (rnd(2.0f) - 1.0f) * m_spacing * 4.0f;
			m_newSpacing = 8.0f + rnd(24.0f);
			m_oldSpacing = m_spacing;

			m_lastChangeTime = XAppGetNow();
			m_nextChangeTime = XAppGetNow() + 10.0f + rnd(5.0f);

//			TRACE("DotField: change %f,%f\n", m_xSpeed, m_ySpeed);
		}
	}
	else
	{
		m_xSpeed = m_hSpeed;
		m_ySpeed = m_vSpeed;
	}

	if (m_lastChangeTime > 0.0f)
	{
		float r = (float) (XAppGetNow() - m_lastChangeTime);
		if (r < 1.0f)
			m_spacing = m_oldSpacing + (m_newSpacing - m_oldSpacing) * r;
		else
			m_lastChangeTime = 0.0f;
	}

	m_xStart = wrap(m_xStart + m_xSpeed * nSeconds / m_spacing);
	m_yStart = wrap(m_yStart + m_ySpeed * nSeconds / m_spacing);

//	TRACE("DotField: %f,%f  %f, %f\n", m_xSpeed, m_ySpeed, m_xStart, m_yStart);
}

void CDotField::RenderDynamicTexture(CSurfx* pSurfx)
{
	for (int y = (int)(m_yStart * m_spacing); y < pSurfx->m_nHeight; y += (int)m_spacing)
	{
		for (int x = (int)(m_xStart * m_spacing); x < pSurfx->m_nWidth; x += (int)m_spacing)
		{
			pSurfx->m_pels[x + y * pSurfx->m_nWidth] = 255;
		}
	}
}
