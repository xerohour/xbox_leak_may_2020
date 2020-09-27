#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "node.h"
#include "runner.h"
#include "TitleCollection.h"

extern const TCHAR* g_szCurTitleImage;

class CCopyDestination : public CNode
{
	DECLARE_NODE(CCopyDestination, CNode)
public:
	CCopyDestination();
	~CCopyDestination();

	void Render();
	void Advance(float nSeconds);
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	CNode* m_pod;
	CNode* m_podIcon;
	CNode* m_panelMU;
	CNode* m_panelMUHilite;
	CNode* m_panelText;
	CNode* m_panelTextHilite;
	CNode* m_console;
	CNode* m_memoryUnit;

	int m_curDevUnit;
	int m_selDevUnit;
	int m_sourceDevUnit;
	float m_spacing;
	bool m_isActive;
	int m_select;

	float m_nScroll;
	int m_nScrollTo;
	XTIME m_nScrollTime;

	void selectUp();
	void selectDown();

	int m_rgDevUnit [9];
	int m_nDevUnitCount;

	DECLARE_NODE_PROPS();
	DECLARE_NODE_FUNCTIONS();

protected:
	
	const TCHAR* GetTitleID2(int nTitle);
	CStrObject* GetTitleID(int nTitle);
};

IMPLEMENT_NODE("CopyDestination", CCopyDestination, CNode)

START_NODE_PROPS(CCopyDestination, CNode)
	NODE_PROP(pt_node, CCopyDestination, pod)
	NODE_PROP(pt_node, CCopyDestination, panelMU)
	NODE_PROP(pt_node, CCopyDestination, panelMUHilite)
	NODE_PROP(pt_node, CCopyDestination, panelText)
	NODE_PROP(pt_node, CCopyDestination, panelTextHilite)
	NODE_PROP(pt_node, CCopyDestination, console)
	NODE_PROP(pt_node, CCopyDestination, memoryUnit)
	NODE_PROP(pt_integer, CCopyDestination, curDevUnit)
	NODE_PROP(pt_integer, CCopyDestination, selDevUnit)
	NODE_PROP(pt_integer, CCopyDestination, sourceDevUnit)
	NODE_PROP(pt_number, CCopyDestination, spacing)
	NODE_PROP(pt_boolean, CCopyDestination, isActive)
	NODE_PROP(pt_integer, CCopyDestination, select)
END_NODE_PROPS()

START_NODE_FUN(CCopyDestination, CNode)
	NODE_FUN_VV(selectUp)
	NODE_FUN_VV(selectDown)
END_NODE_FUN()


CCopyDestination::CCopyDestination() :
	m_pod(NULL),
	m_panelMU(NULL),
	m_panelMUHilite(NULL),
	m_panelText(NULL),
	m_panelTextHilite(NULL),
	m_console(NULL),
	m_memoryUnit(NULL),
	m_curDevUnit(-1),
	m_selDevUnit(-1),
	m_sourceDevUnit(-1),
	m_spacing(0.4f),
	m_isActive(false),
	m_select(-1)
{
	m_nScroll = 0.0f;
	m_nScrollTo = 0;
	m_nScrollTime = 0.0f;
	m_nDevUnitCount = 0;
}

CCopyDestination::~CCopyDestination()
{
	if (m_pod != NULL)
		m_pod->Release();

	if (m_panelMU != NULL)
		m_panelMU->Release();

	if (m_panelMUHilite != NULL)
		m_panelMUHilite->Release();

	if (m_panelText != NULL)
		m_panelText->Release();

	if (m_panelTextHilite != NULL)
		m_panelTextHilite->Release();

	if (m_console != NULL)
		m_console->Release();

	if (m_memoryUnit != NULL)
		m_memoryUnit->Release();
}

void CCopyDestination::Render()
{
//	ASSERT(m_isActive); // you really should show an inactive copy dest object!

	if (m_pod == NULL)
		return;

	float y = m_nScroll;
	for (int i = 0; i < m_nDevUnitCount; i += 1)
	{
		D3DXMATRIX mat;
		D3DXMatrixTranslation(&mat, 0.0f, y, 0.0f);
		XAppPushWorld();
		XAppMultWorld(&mat);
		XAppUpdateWorld();
		m_pod->Render();
		
		// update the text rendered under each MU
		m_selDevUnit = m_rgDevUnit[i];
		CallFunction(this, _T("UpdateMemUnitText"));

		if (m_rgDevUnit[i] == m_curDevUnit)
		{
			if(m_selDevUnit == 8)
			{
				m_panelMUHilite->Render();
				m_panelTextHilite->Render();
				m_console->Render();
			}
			else
			{
				m_panelMUHilite->Render();
				m_panelTextHilite->Render();
				m_memoryUnit->Render();
			}
		}
		else
		{
			if(m_selDevUnit == 8)
			{
				m_panelMU->Render();
				m_panelText->Render();
				m_console->Render();
			}
			else
			{
				m_panelMU->Render();
				m_panelText->Render();
				m_memoryUnit->Render();
			}
		}

		// TODO: Work the text into here...

		XAppPopWorld();

		y -= m_spacing;
	}
}

void CCopyDestination::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);
    bool fCurDevUnitRemoved = false;

	if (!m_isActive)
		return;

	int nDevUnitIndex = -1;

	// Look for memory units...
	m_nDevUnitCount = 0;
	for (int i = 0; i < 9; i += 1)
	{
		int devUnit = (i == 0) ? 8/*Dev0*/ : (i - 1);

		// Ignore the source unit
		if (devUnit == m_sourceDevUnit)
			continue;

		// Ignore missing units
		if (devUnit != 8/*Dev0*/ && !g_titles[devUnit].IsValid())
        {
            if(devUnit == m_curDevUnit)
            {
                fCurDevUnitRemoved = true;
            }
			continue;
        }

		m_rgDevUnit[m_nDevUnitCount] = devUnit;

		if (devUnit == m_curDevUnit)
			nDevUnitIndex = m_nDevUnitCount;

		m_nDevUnitCount += 1;
	}

	// TODO: look for inserted units and auto-select them...

	if (nDevUnitIndex != -1 && m_select != nDevUnitIndex)
	{
		// The current unit switched positions; scroll immediately
		m_select = nDevUnitIndex;
		m_nScrollTo = m_select;
		m_nScroll = m_spacing * m_select;
	}

	if (m_select < 0)
		m_select = 0;
	if (m_select > m_nDevUnitCount - 1)
		m_select = m_nDevUnitCount - 1;

	if (m_nScrollTo != m_select)
	{
		m_nScrollTo = m_select;
		m_nScrollTime = XAppGetNow();
	}
	else if (m_nScrollTime != 0.0f)
	{
		float t = (float) (XAppGetNow() - m_nScrollTime) / 0.25f;
		if (t >= 1.0f)
		{
			m_nScrollTime = 0.0f;
			t = 1.0f;
		}

		float t1 = 1.0f - t;
		m_nScroll = t1 * m_nScroll + t * (m_spacing * m_nScrollTo);
	}

    int curDevUnit = -1;
	if (m_select != -1)
	    curDevUnit = m_rgDevUnit[m_select];
    
	if (curDevUnit != m_curDevUnit)
	{
        m_curDevUnit = curDevUnit;
        
        
        if(fCurDevUnitRemoved)
        {
            CallFunction(this, _T("OnDestinationUnitRemoved"));
        }
		CallFunction(this, _T("OnCurDevUnitChange"));
    }
}

bool CCopyDestination::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	return CNode::OnSetProperty(pprd, pvValue);
}

void CCopyDestination::selectUp()
{
	if (m_nDevUnitCount < 2)
		return;

	int nSel = m_select - 1;
	if (nSel >= 0 && nSel < m_nDevUnitCount)
	{
		m_select = nSel;
		m_curDevUnit = m_rgDevUnit[nSel];
		CallFunction(this, _T("OnCurDevUnitChange"));
	}
}

void CCopyDestination::selectDown()
{
	if (m_nDevUnitCount < 2)
		return;

	int nSel = m_select + 1;
	if (nSel >= 0 && nSel < m_nDevUnitCount)
	{
		m_select = nSel;
		m_curDevUnit = m_rgDevUnit[nSel];
		CallFunction(this, _T("OnCurDevUnitChange"));
	}
}
