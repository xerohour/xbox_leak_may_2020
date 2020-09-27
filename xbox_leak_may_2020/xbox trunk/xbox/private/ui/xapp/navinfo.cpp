#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "NavInfo.h"


IMPLEMENT_NODE("NavigationInfo", CNavigationInfo, CNode)

START_NODE_PROPS(CNavigationInfo, CNode)
	NODE_PROP(pt_boolean, CNavigationInfo, isBound)
	NODE_PROP(pt_vec3, CNavigationInfo, avatarSize)
	NODE_PROP(pt_boolean, CNavigationInfo, headlight)
	NODE_PROP(pt_number, CNavigationInfo, speed)
	NODE_PROP(pt_string, CNavigationInfo, type)
	NODE_PROP(pt_number, CNavigationInfo, visibilityLimit)
	NODE_PROP(pt_node, CNavigationInfo, shape)
END_NODE_PROPS()

CNavigationInfo::CNavigationInfo() :
	m_isBound(false),
	m_avatarSize(0.25f, 1.6f, 0.75f),
	m_headlight(true),
	m_speed(1.0f),
	m_type(NULL),
	m_visibilityLimit(0.0f),
	m_shape(NULL)
{
}

CNavigationInfo::~CNavigationInfo()
{
	delete [] m_type;

	if (theApp.m_pNavigationInfo == this)
		theApp.m_pNavigationInfo = NULL;
}

void CNavigationInfo::OnLoad()
{
	CNode::OnLoad();

	if (theApp.m_pNavigationInfo == NULL)
	{
		theApp.m_pNavigationInfo = this;
		Bind();
	}
}

void CNavigationInfo::Bind()
{
	m_isBound = true;

#ifndef _XBOX
	if (m_type != NULL)
	{
		CNavigator* pNavigator = CNavigator::Lookup(m_type);
		if (pNavigator == NULL)
		{
			TRACE(_T("\001Unknown navigation type: %s\n"), m_type);
		}
		else
		{
			theApp.m_pNavigator = pNavigator;
		}
	}
#endif

	theApp.m_bProjectionDirty = true;
}

bool CNavigationInfo::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_isBound))
	{
		if (*(bool*)pvValue)
			Bind();
	}
	else if ((int)pprd->pbOffset == offsetof(m_avatarSize))
	{
		theApp.m_bProjectionDirty = true;
	}
	else if ((int)pprd->pbOffset == offsetof(m_visibilityLimit))
	{
		theApp.m_bProjectionDirty = true;
	}

	return true;
}
