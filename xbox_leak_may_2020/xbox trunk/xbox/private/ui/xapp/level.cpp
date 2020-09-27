#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Level.h"
#include "Camera.h"
#include "Viewpoint.h"
#include "xip.h"

extern float g_nEffectAlpha;
extern float g_transitionMotionBlur;

////////////////////////////////////////////////////////////////////////////

extern void BindJoystick(CNode* pJoystickNode);

CLevel* g_pCurLevel;
CLevel* g_pFromLevel;
CNode* g_pCurTunnel;
XTIME g_timeToNextLevel;
bool g_bLevelTransition;

IMPLEMENT_NODE("Level", CLevel, CGroup)

START_NODE_PROPS(CLevel, CGroup)
	NODE_PROP(pt_node, CLevel, control)
	NODE_PROP(pt_node, CLevel, tunnel)
	NODE_PROP(pt_node, CLevel, path)
	NODE_PROP(pt_node, CLevel, shell)
	NODE_PROP(pt_string, CLevel, archive)
	NODE_PROP(pt_boolean, CLevel, unloadable)
	NODE_PROP(pt_boolean, CLevel, fade)
END_NODE_PROPS()

START_NODE_FUN(CLevel, CGroup)
	NODE_FUN_VV(GoTo)
	NODE_FUN_VV(GoBackTo)
END_NODE_FUN()

CLevel::CLevel() :
	m_tunnel(NULL),
	m_control(NULL),
	m_path(NULL),
	m_shell(NULL),
	m_archive(NULL),
    m_fade(true),
	m_unloadable(true)
{
	m_visible = false; // override default
	m_pXipFile = NULL;
	g_timeToNextLevel = 0.0f;
	m_bArrive = false;
}

CLevel::~CLevel()
{
	if (this == g_pCurLevel)
		g_pCurLevel = NULL;
	
	if (this == g_pFromLevel)
		g_pFromLevel = NULL;

	if (m_tunnel == g_pCurTunnel)
		g_pCurTunnel = NULL;

	if (m_tunnel != NULL)
		m_tunnel->Release();

	if (m_control != NULL)
		m_control->Release();

	if (m_path != NULL)
		m_path->Release();

	if (m_shell != NULL)
		m_shell->Release();

	delete [] m_archive;
}

void CLevel::Render()
{
	float alpha = 1.0f;

    if (m_fade)
    {
    	float t = (float) (g_timeToNextLevel - XAppGetNow()) / 0.75f;
    	if (t > 1.0f)
    		t = 1.0f;
    	else if (t < 0.0f)
    		t = 0.0f;

    	if (g_pCurLevel == this)
    		alpha = 1.0f - t;
    	else if (g_pFromLevel == this)
    		alpha = t;

    	if (alpha == 0.0f)
    		return;
    }

	float nEffectAlphaSave = g_nEffectAlpha;
	g_nEffectAlpha *= alpha;

	if (m_shell != NULL)
		m_shell->Render();

	if (g_pCurLevel == this)
	{
		// turn on motion blur
		float blurAlpha = alpha * 1.3f;
		if(blurAlpha > 1.0f)
			blurAlpha = 1.0f;
		g_transitionMotionBlur = blurAlpha;
	}

	CGroup::Render();

//	if (m_tunnel != NULL && m_tunnel->m_visible)
//		m_tunnel->Render();

	g_nEffectAlpha = nEffectAlphaSave;
}

void CLevel::Activate()
{
    g_bLevelTransition = true;

	TRACE(_T("Leaving level  0x%08x\n"), g_pCurLevel);
	TRACE(_T("Going to level 0x%08x\n"), this);

	if (m_archive != NULL && m_pXipFile == NULL)
	{
		m_pXipFile = LoadXIP(m_archive);
	}

	if (g_pFromLevel != NULL || g_pCurLevel != NULL && g_timeToNextLevel != 0.0f)
	{
		TRACE(_T("\taborting goto level 0x%08x\n"), g_pFromLevel);

		if (g_pFromLevel != NULL)
		{
			g_pFromLevel->Deactivate();
			g_pFromLevel = NULL;
			g_timeToNextLevel = 0.0f;
		}

		if (g_pCurLevel != NULL)
		{
			g_pCurLevel->m_visible = false;
			if (g_pCurLevel->m_pXipFile && g_pCurLevel->m_unloadable && !g_pCurLevel->m_pXipFile->m_bLocked)
				g_pCurLevel->m_pXipFile->DeleteMeshBuffers();
			g_pCurLevel = NULL;
			g_timeToNextLevel = 0.0f;
		}

		if (g_pCurTunnel != NULL)
		{
			g_pCurTunnel->m_visible = false;
			g_pCurTunnel = NULL;
		}
	}

	ASSERT(g_pFromLevel == NULL);

	g_timeToNextLevel = XAppGetNow() + 1.0f;
	g_pFromLevel = g_pCurLevel;

	m_visible = true;

	BindJoystick(NULL); // disable joystick while transitioning

	g_pCurLevel = this;
}

void CLevel::GoTo()
{
	if (this == g_pCurLevel)
		return;

	Activate();

	g_pCurTunnel = m_tunnel;

	if (g_pCurTunnel != NULL)
		g_pCurTunnel->m_visible = true;

	if (m_path != NULL)
	{
		if (m_path->IsKindOf(NODE_CLASS(CViewpoint)))
			BindViewpoint(m_path);
		else if (m_path->IsKindOf(NODE_CLASS(CCameraPath)))
			((CCameraPath*)m_path)->Activate(false);
	}

	CallFunction(this, _T("OnActivate"));
}

void CLevel::GoBackTo()
{
	if (this == g_pCurLevel)
		return;

	Activate();

	g_pCurTunnel = (g_pFromLevel == NULL) ? NULL : g_pFromLevel->m_tunnel;

	if (g_pCurTunnel != NULL)
		g_pCurTunnel->m_visible = true;

	if (g_pFromLevel != NULL && g_pFromLevel->m_path != NULL && g_pFromLevel->m_path->IsKindOf(NODE_CLASS(CCameraPath)))
		((CCameraPath*)g_pFromLevel->m_path)->Activate(true);
	else if (m_path != NULL && m_path->IsKindOf(NODE_CLASS(CViewpoint)))
		BindViewpoint(m_path);

	CallFunction(this, _T("OnActivate"));
}

void CLevel::Advance(float nSeconds)
{
	CGroup::Advance(nSeconds);

	if (m_tunnel != NULL)
		m_tunnel->Advance(nSeconds);

	if (m_path != NULL)
		m_path->Advance(nSeconds);

	if (m_control != NULL)
		m_control->Advance(nSeconds);

	if (m_shell != NULL)
		m_shell->Advance(nSeconds);

	if (g_pCurLevel == this && g_timeToNextLevel != 0.0f && XAppGetNow() >= g_timeToNextLevel)
	{
		TRACE(_T("Arrived at level 0x%08x\n"), this);

		ASSERT(g_pCurLevel == this);

		// We're there...
		g_timeToNextLevel = 0.0f;

		if (g_pCurTunnel != NULL)
			g_pCurTunnel->m_visible = false;

		if (g_pFromLevel != NULL)
		{
			g_pFromLevel->Deactivate();
			g_pFromLevel = NULL;
		}

		m_bArrive = true;
	}

	if (m_bArrive && (m_pXipFile == NULL || m_pXipFile->m_bLoaded))
	{
		m_bArrive = false;
		BindJoystick(m_control);
		CallFunction(this, _T("OnArrival"));
        g_bLevelTransition = false;
	}
}

void CLevel::Deactivate()
{
	ASSERT(this == g_pFromLevel);

	m_visible = false;
	CallFunction(this, _T("OnDeactivate"));

	if (m_unloadable && !m_pXipFile->m_bLocked)
		m_pXipFile->DeleteMeshBuffers();
}
