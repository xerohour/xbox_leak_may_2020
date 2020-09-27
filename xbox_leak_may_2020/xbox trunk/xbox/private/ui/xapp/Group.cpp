#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "xip.h"
#include "Camera.h"
#include "Shape.h"

IMPLEMENT_NODE("Group", CGroup, CNode)

START_NODE_PROPS(CGroup, CNode)
    NODE_PROP(pt_children, CGroup, children)
END_NODE_PROPS()

CGroup::CGroup()
{
    m_bboxCenter.x = 0.0f;
    m_bboxCenter.y = 0.0f;
    m_bboxCenter.z = 0.0f;
    m_bboxSize.x = -1.0f;
    m_bboxSize.y = -1.0f;
    m_bboxSize.z = -1.0f;
    m_bboxSpecified = true;
    m_bboxDirty = true;
}

CGroup::~CGroup()
{
}

void CGroup::GetBBox(BBox* pBBox)
{
    if (m_bboxSize.x == -1.0f && m_bboxSize.y == -1.0f && m_bboxSize.z == -1.0f)
        m_bboxSpecified = false;

    if (m_bboxDirty && !m_bboxSpecified)
    {
        int nChildCount = m_children.GetLength();
        if (nChildCount == 0)
        {
            m_bboxCenter.x = 0.0f;
            m_bboxCenter.y = 0.0f;
            m_bboxCenter.z = 0.0f;
            m_bboxSize.x = 0.0f;
            m_bboxSize.y = 0.0f;
            m_bboxSize.z = 0.0f;
        }
        else
        {
            BBox bbox;
            m_children.GetNode(0)->GetBBox(&bbox);

            float xMin = bbox.center.x - bbox.size.x;
            float yMin = bbox.center.y - bbox.size.y;
            float zMin = bbox.center.z - bbox.size.z;
            float xMax = bbox.center.x + bbox.size.x;
            float yMax = bbox.center.y + bbox.size.y;
            float zMax = bbox.center.z + bbox.size.z;

            for (int i = 1; i < nChildCount; i += 1)
            {
                m_children.GetNode(i)->GetBBox(&bbox);

                if (xMin > bbox.center.x - bbox.size.x)
                    xMin = bbox.center.x - bbox.size.x;

                if (yMin > bbox.center.y - bbox.size.y)
                    yMin = bbox.center.y - bbox.size.y;

                if (zMin > bbox.center.z - bbox.size.z)
                    zMin = bbox.center.z - bbox.size.z;

                if (xMax > bbox.center.x + bbox.size.x)
                    xMax = bbox.center.x + bbox.size.x;

                if (yMax > bbox.center.y + bbox.size.y)
                    yMax = bbox.center.y + bbox.size.y;

                if (zMax > bbox.center.z + bbox.size.z)
                    zMax = bbox.center.z + bbox.size.z;
            }

            m_bboxCenter.x = (xMax + xMin) / 2.0f;
            m_bboxCenter.y = (yMax + yMin) / 2.0f;
            m_bboxCenter.z = (zMax + zMin) / 2.0f;

            m_bboxSize.x = xMax - xMin;
            m_bboxSize.y = yMax - yMin;
            m_bboxSize.z = zMax - zMin;
        }
    }

    pBBox->center = m_bboxCenter;
    pBBox->size = m_bboxSize;
}

float CGroup::GetRadius()
{
    float radius = 0.0f;

    for (int i = 0; i < m_children.GetLength(); i += 1)
    {
        float r = m_children.GetNode(i)->GetRadius();
        if (radius < r)
            radius = r;
    }

    return radius;
}

void CGroup::Render()
{
    for (int i = 0; i < m_children.GetLength(); i += 1)
    {
        CNode* pNode = m_children.GetNode(i);
        if (pNode->m_visible)
            pNode->Render();
    }
}

void CGroup::Advance(float nSeconds)
{
    CNode::Advance(nSeconds);

    for (int i = 0; i < m_children.GetLength(); i += 1)
        m_children.GetNode(i)->Advance(nSeconds);
}

#ifdef _LIGHTS
void CGroup::SetLight(int& nLight, D3DCOLORVALUE& ambient)
{
    for (int i = 0; i < m_children.GetLength(); i += 1)
    {
        CNode* pNode = m_children.GetNode(i);
        if (pNode != NULL)
            pNode->SetLight(nLight, ambient);
    }
}
#endif

void CGroup::RenderDynamicTexture(CSurfx* pSurfx)
{
    for (int i = 0; i < m_children.GetLength(); i += 1)
    {
        CNode* pNode = m_children.GetNode(i);
        if (pNode != NULL)
            pNode->RenderDynamicTexture(pSurfx);
    }
}

LPDIRECT3DTEXTURE8 CGroup::GetTextureSurface()
{
    for (int i = 0; i < m_children.GetLength(); i += 1)
    {
        CNode* pNode = m_children.GetNode(i);
        if (pNode != NULL)
        {
            LPDIRECT3DTEXTURE8 pSurface = pNode->GetTextureSurface();
            if (pSurface != NULL)
                return pSurface;
        }
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////

// TODO: Lights are not transformed!

IMPLEMENT_NODE("Transform", CTransform, CGroup)

START_NODE_PROPS(CTransform, CGroup)
    NODE_PROP(pt_vec3, CTransform, center) // NOTE: Ignored!
    NODE_PROP(pt_vec4, CTransform, scaleOrientation) // NOTE: Ignored!
    NODE_PROP(pt_vec3, CTransform, scale)
    NODE_PROP(pt_vec4, CTransform, rotation)
    NODE_PROP(pt_vec3, CTransform, translation)
    NODE_PROP(pt_number, CTransform, fade)
    NODE_PROP(pt_boolean, CTransform, moving)
    NODE_PROP(pt_number, CTransform, alpha)
END_NODE_PROPS()

START_NODE_FUN(CTransform, CGroup)
    NODE_FUN_VNNN(SetScale)
    NODE_FUN_VNNNN(SetScaleOrientation)
    NODE_FUN_VNNN(SetTranslation)
    NODE_FUN_VNNN(SetCenter)
    NODE_FUN_VNNNN(SetRotation)
    NODE_FUN_VN(SetAlpha)
    NODE_FUN_VN(DisappearAfter)
END_NODE_FUN()

CTransform::CTransform() :
    m_center(0.0f, 0.0f, 0.0f),
    m_scaleOrientation(0.0f, 0.0f, 1.0f, 0.0f),
    m_scale(1.0f, 1.0f, 1.0f),
    m_rotation(0.0f, 0.0f, 1.0f, 0.0f),
    m_translation(0.0f, 0.0f, 0.0f),
    m_fade(0.0f),
    m_moving(false)
{
    m_bDirty = true;
    m_alpha = 1.0f;
    m_timeCenterStart = 0.0f;
    m_timeScaleStart = 0.0f;
    m_timeScaleOrientationStart = 0.0f;
    m_timeRotationStart = 0.0f;
    m_timeTranslationStart = 0.0f;
    m_timeAlphaStart = 0.0f;
    m_timeToDisappear = 0.0;
}

void CTransform::OnLoad()
{
    CGroup::OnLoad();

    D3DXQuaternionRotationAxis(&m_rotationQuat, (D3DXVECTOR3*)&m_rotation, m_rotation.w);
}

bool CTransform::OnSetProperty(const PRD* pprd, const void* pvValue)
{
    if ((int)pprd->pbOffset == offsetof(m_rotation))
    {
        const float* p = (const float*)pvValue;
        D3DXQuaternionRotationAxis(&m_rotationQuat, (D3DXVECTOR3*)p, p[3]);
    }
    else if ((int)pprd->pbOffset == offsetof(m_alpha))
    {
        SetAlpha(*(float*)pvValue);
        return false;
    }

/*
    if ((int)pprd->pbOffset == offsetof(m_scale))
    {
        m_scaleCur = m_scale;
        m_scaleStart = m_scale;
        m_scaleEnd = *((D3DXVECTOR3*)pvValue);
        m_timeStart = XAppGetNow();
    }

    m_bDirty = true;
*/
    return CGroup::OnSetProperty(pprd, pvValue);
}

void CTransform::Advance(float nSeconds)
{
    CGroup::Advance(nSeconds);
/*
    if (m_timeStart != 0.0f && m_fade != 0.0f)
    {
        float t = (XAppGetNow() - m_timeStart) / m_fade;
        if (t > 1.0f)
        {
            t = 1.0f;
            m_timeStart = 0.0f;
        }

        D3DXVec3Lerp(&m_scaleCur, &m_scaleStart, &m_scaleEnd, t);
        m_bDirty = true;
    }
*/

    if (m_timeAlphaStart > 0.0f)
    {
        float t = (float) (XAppGetNow() - m_timeAlphaStart) / m_fade;
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_timeAlphaStart = 0.0f;
        }

        m_alpha = m_alphaStart + (m_alphaEnd - m_alphaStart) * t;
    }

    if (m_timeToDisappear > 0.0f && XAppGetNow() >= m_timeToDisappear)
    {
        m_alpha = 0.0f;
        m_timeToDisappear = 0.0f;
    }
}

void CTransform::CalcMatrix()
{
    D3DXQUATERNION scaleOrientation, rotation;

    D3DXMatrixIdentity(&m_matrix);

    if (m_timeScaleStart > 0.0f)
    {
        m_moving = true;

        float t = (float) (XAppGetNow() - m_timeScaleStart) / m_fade;
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_timeScaleStart = 0.0f;
        }

        D3DXVec3Lerp(&m_scale, &m_scaleStart, &m_scaleEnd, t);
    }

    if (m_timeTranslationStart > 0.0f)
    {
        m_moving = true;

        float t = (float) (XAppGetNow() - m_timeTranslationStart) / m_fade;
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_timeTranslationStart = 0.0f;
        }

        D3DXVec3Lerp(&m_translation, &m_translationStart, &m_translationEnd, t);
    }

    if (m_timeRotationStart > 0.0f)
    {
        m_moving = true;

        float t = (float) (XAppGetNow() - m_timeRotationStart) / m_fade;
        if (t >= 1.0f)
        {
            t = 1.0f;
            m_timeRotationStart = 0.0f;
        }

        D3DXQuaternionSlerp(&m_rotationQuat, &m_rotationStart, &m_rotationEnd, t);
    }

    if (m_scale.x != 1.0f || m_scale.y != 1.0f || m_scale.z != 1.0f)
    {
        D3DXMATRIX mat;
        D3DXMatrixScaling(&mat, m_scale.x, m_scale.y, m_scale.z);
        D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
    }

    if (m_rotationQuat.w != 0.0f)
    {
        D3DXMATRIX mat;
//      D3DXMatrixRotationAxis(&mat, (D3DXVECTOR3*)&m_rotation, m_rotation.w);
        D3DXMatrixRotationQuaternion(&mat, &m_rotationQuat);
        D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
    }

    if (m_translation.x != 0.0f || m_translation.y != 0.0f || m_translation.z != 0.0f)
    {
        D3DXMATRIX mat;
        D3DXMatrixTranslation(&mat, m_translation.x, m_translation.y, m_translation.z);
        D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
    }

    m_bDirty = (m_timeScaleStart + m_timeTranslationStart + m_timeRotationStart) > 0.0f;
}

void CTransform::SetScale(float sx, float sy, float sz)
{
    if (m_fade > 0.0f)
    {
        m_timeScaleStart = XAppGetNow();

        m_scaleStart = m_scale;

        m_scaleEnd.x = sx;
        m_scaleEnd.y = sy;
        m_scaleEnd.z = sz;

        m_moving = true;
    }
    else
    {
        m_scale.x = sx;
        m_scale.y = sy;
        m_scale.z = sz;
    }

    m_bDirty = true;
}

void CTransform::SetScaleOrientation(float x, float y, float z, float a)
{
    // TODO: Animation

    m_scaleOrientation.x = x;
    m_scaleOrientation.y = y;
    m_scaleOrientation.z = z;
    m_scaleOrientation.w = a;

    m_bDirty = true;
}

void CTransform::SetTranslation(float x, float y, float z)
{
    if (m_fade > 0.0f)
    {
        m_timeTranslationStart = XAppGetNow();

        m_translationStart = m_translation;

        m_translationEnd.x = x;
        m_translationEnd.y = y;
        m_translationEnd.z = z;

        m_moving = true;
    }
    else
    {
        m_translation.x = x;
        m_translation.y = y;
        m_translation.z = z;
    }

    m_bDirty = true;
}

void CTransform::SetCenter(float x, float y, float z)
{
    // TODO: Animation

    m_center.x = x;
    m_center.y = y;
    m_center.z = z;

    m_bDirty = true;
}

void CTransform::SetRotation(float x, float y, float z, float a)
{
    D3DXVECTOR3 v(x, y, z);
    D3DXQUATERNION q;
    D3DXQuaternionRotationAxis(&q, &v, a);

    if (m_fade > 0.0f)
    {
        m_timeRotationStart = XAppGetNow();

        m_rotationStart = m_rotationQuat;

        m_rotationEnd = q;

        m_moving = true;
    }
    else
    {
        m_rotation.x = x;
        m_rotation.y = y;
        m_rotation.z = z;
        m_rotation.w = a;

        m_rotationQuat = q;
    }

    m_bDirty = true;
}

void CTransform::SetAlpha(float a)
{
    if (m_fade > 0.0f)
    {
        m_timeAlphaStart = XAppGetNow();
        m_alphaStart = m_alpha;
        m_alphaEnd = a;
    }
    else
    {
        m_alpha = a;
    }
}

void CTransform::DisappearAfter(float t)
{
    m_timeToDisappear = XAppGetNow() + t;
}

extern float g_nEffectAlpha;

void CTransform::Render()
{
    m_moving = false;

    if (m_bDirty)
        CalcMatrix();

    XAppPushWorld();
    XAppMultWorld(&m_matrix);

    XAppUpdateWorld();

    float nEffectAlphaSave = g_nEffectAlpha;
    g_nEffectAlpha *= m_alpha;

	if(m_alpha > 0.0f)  // If object has an alpha of 0, it is not rendered
	{
		CGroup::Render();
	}

    g_nEffectAlpha = nEffectAlphaSave;

    XAppPopWorld();
}

////////////////////////////////////////////////////////////////////////////

class CInline : public CGroup
{
    DECLARE_NODE(CInline, CGroup)
public:
    CInline();
    ~CInline();

    TCHAR* m_url;
    bool m_preload;
    bool m_bClassLoaded;
    bool m_fadeInDelayLoad;

    void Render();
    void Advance(float nSeconds);

protected:
    void Init();
    void FinishLoad();
    bool m_bDirty;

    bool m_bLoading; // true when a background XIP load is in progress
    bool m_bFirstLoad;
    CXipFile* m_pXipFile;
    XTIME m_timeOfLoad;

    CClass m_class;

    DECLARE_NODE_PROPS()

private:
    HANDLE m_hClassLoaderThread;
    TCHAR m_szAbsURL[MAX_PATH];
    static void WINAPI ClassLoaderThread(CInline* p);
};

IMPLEMENT_NODE("Inline", CInline, CGroup)

START_NODE_PROPS(CInline, CGroup)
    NODE_PROP(pt_string, CInline, url)
    NODE_PROP(pt_boolean, CInline, preload)
    NODE_PROP(pt_boolean, CInline, fadeInDelayLoad)
END_NODE_PROPS()

CInline::CInline() :
    m_url(NULL),
    m_hClassLoaderThread(NULL),
    m_bClassLoaded(false),
    m_fadeInDelayLoad(true),
    m_preload(false)
{
    m_bDirty = true;
    m_bLoading = false;
    m_bFirstLoad = false;
    m_pXipFile = NULL;
    m_timeOfLoad = 0.0f;
}

CInline::~CInline()
{
    delete [] m_url;
}

void CInline::Init()
{
    ASSERT(m_bDirty);
    m_bDirty = false;
    m_children.ReleaseAll();

    if (m_url == NULL)
        return;

    ASSERT(!m_bLoading);

    // BLOCK: Start the background load if there is a XIP file...
    {
        TCHAR szURL [MAX_PATH];

        MakeAbsoluteURL(szURL, m_url);
        TCHAR* pch = _tcsrchr(szURL, '/');
        if (pch != NULL)
        {
            _tcscpy(pch, _T(".xip"));

            m_bLoading = true;
            m_bFirstLoad = true;
            m_pXipFile = LoadXIP(szURL);
            if (m_pXipFile != NULL)
                return;

            TRACE(_T("\001LoadXIP: %s failed!\n"));

            m_bLoading = false;
        }
    }

    FinishLoad();
}

void CInline::FinishLoad()
{
    TRACE(_T("FinishLoad: (0x%08x) %s\n"), this, m_url);
    ASSERT(!m_bClassLoaded);

    if (m_class.LoadAbsURL(m_szAbsURL))
    {
        CNode* pNode = m_class.CreateNode();
        CallFunction(pNode, _T("initialize"));

        m_children.AddNode(pNode);
        pNode->m_pParent = this;
        CallFunction(this, _T("onLoad"));
    }
    m_bFirstLoad = false;
    m_bClassLoaded = true;
}

void CInline::Render()
{
    if (m_url == NULL || m_bLoading)
        return;

    if (m_pXipFile != NULL && m_pXipFile->IsUnloaded())
    {
        m_bLoading = true;
        m_pXipFile->Reload();
        return;
    }

    if (m_bDirty)
        Init();

    if (m_bLoading || !m_bClassLoaded)
        return;

    float a = (float) (XAppGetNow() - m_timeOfLoad) / 2.0f;

    if (!m_fadeInDelayLoad)
    {
        a = 1.0f;
    }
    else if (a > 1.0f)
    {
        a = 1.0f;
    }

    float nEffectAlphaSave = g_nEffectAlpha;
    g_nEffectAlpha *= a;

    CDirPush dirPush(m_url);

    CGroup::Render();

    g_nEffectAlpha = nEffectAlphaSave;
}

void WINAPI CInline::ClassLoaderThread(CInline* p)
{
    ASSERT(p->m_pXipFile);
    ASSERT(p->m_bFirstLoad);
    ASSERT(!p->m_bLoading);
    ASSERT(!p->m_bClassLoaded);
    ASSERT(p->m_pXipFile->IsReady());
    p->FinishLoad();
}

void CInline::Advance(float nSeconds)
{
    if (m_bDirty && m_preload)
        Init();

    if (m_pXipFile != NULL && !m_pXipFile->IsReady())
    {
        m_bLoading = true;
        return;
    }

    if (m_bLoading)
    {
        ASSERT(m_pXipFile != NULL);
        if (!m_pXipFile->IsReady())
            return;

        m_bLoading = false;

        if (m_bFirstLoad)
        {
            ASSERT(m_hClassLoaderThread == NULL);

            MakeAbsoluteURL(m_szAbsURL, m_url);

            // BUGBUG: class loader can't be in separate thread since many of scripting engine is not multi-threaded safe
            // m_hClassLoaderThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ClassLoaderThread, (LPVOID)this, 0, 0);

            if (!m_hClassLoaderThread)
            {
                FinishLoad();
            }
        }

        m_timeOfLoad = XAppGetNow();
    }

    if (m_hClassLoaderThread)
    {
        if (WaitForSingleObject(m_hClassLoaderThread, 0) == WAIT_OBJECT_0)
        {
            ASSERT(m_bClassLoaded);
            CloseHandle(m_hClassLoaderThread);
            m_hClassLoaderThread = NULL;
        }
    }

    //if (m_bLoading || !m_bClassLoaded)
        //return;

    CGroup::Advance(nSeconds);
}

////////////////////////////////////////////////////////////////////////////

class CSpinner : public CGroup
{
    DECLARE_NODE(CSpinner, CGroup)
public:
    CSpinner();

    float m_rpm;
    D3DXVECTOR3 m_axis;

    float m_angle;

    void Render();
    void Advance(float nSeconds);

    DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Spinner", CSpinner, CGroup)

START_NODE_PROPS(CSpinner, CGroup)
    NODE_PROP(pt_number, CSpinner, rpm)
    NODE_PROP(pt_vec3, CSpinner, axis)
    NODE_PROP(pt_number, CSpinner, angle)
END_NODE_PROPS()

CSpinner::CSpinner()
{
    m_rpm = 1.0f;
    m_axis.x = 0.0f;
    m_axis.y = 1.0f;
    m_axis.z = 0.0f;
    m_angle = 0.0f;
}

void CSpinner::Render()
{
    XAppPushWorld();

    D3DXMATRIX matrix;
    D3DXMatrixRotationAxis(&matrix, &m_axis, m_angle);

    XAppMultWorld(&matrix);

    XAppUpdateWorld();

    CGroup::Render();

    XAppPopWorld();
}

void CSpinner::Advance(float nSeconds)
{
    CGroup::Advance(nSeconds);

    m_angle += ((m_rpm / 60.0f) * nSeconds) * (2.0f * D3DX_PI);

    while (m_angle > 2.0f * D3DX_PI)
        m_angle -= 2.0f * D3DX_PI;
    while (m_angle < -2.0f * D3DX_PI)
        m_angle += 2.0f * D3DX_PI;
}

////////////////////////////////////////////////////////////////////////////


class CWaver : public CSpinner
{
    DECLARE_NODE(CWaver, CSpinner)
public:
    CWaver();

    float m_field;

    void Advance(float nSeconds);

    DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Waver", CWaver, CGroup)

START_NODE_PROPS(CWaver, CSpinner)
    NODE_PROP(pt_number, CWaver, field)
END_NODE_PROPS()

CWaver::CWaver()
{
    m_field = D3DX_PI / 4.0f;
}

void CWaver::Advance(float nSeconds)
{
    CGroup::Advance(nSeconds); // Skip CSpinner!

    m_angle = sinf((float) (XAppGetNow() * D3DX_PI * m_rpm / 60.0f)) * m_field / 2.0f;
}

////////////////////////////////////////////////////////////////////////////

class CLayout : public CGroup
{
    DECLARE_NODE(CLayout, CGroup)
public:
    CLayout();

    D3DXVECTOR3 m_direction;
    float m_spacing;

    void Render();
    void GetBBox(BBox* pBBox);
    float GetRadius();

    DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Layout", CLayout, CGroup)

START_NODE_PROPS(CLayout, CGroup)
    NODE_PROP(pt_vec3, CLayout, direction)
    NODE_PROP(pt_number, CLayout, spacing)
END_NODE_PROPS()

CLayout::CLayout() :
    m_direction(1.0f, 0.0f, 0.0f),
    m_spacing(1.0f)
{
}

void CLayout::Render()
{
    int nChildren = m_children.GetLength();
    if (nChildren == 0)
        return;

    if (m_direction.x == 0 && m_direction.y == 0 && m_direction.z == 0)
    {
        // Make a circle...
        D3DXMATRIX mat = *XAppGetWorld();

        float r = (nChildren / 4) * m_spacing;

        for (int i = 0; i < nChildren; i += 1)
        {
            XAppPushWorld();

            CNode* pNode = m_children.GetNode(i);
            if (!pNode->m_visible)
                continue;

            float a = D3DX_PI * 2.0f * i / nChildren;
            float x = r * cosf(-a);
            float z = r * sinf(-a);

            D3DXVECTOR3 axis(0.0f, 0.1f, 0.0f);
            XAppTranslateWorld(x, 0.0f, z);
            XAppRotateWorld(&axis, a + D3DX_PI / 2.0f);
            XAppUpdateWorld();

            pNode->Render();

            XAppPopWorld();
        }

        return;
    }

    float spacing = m_spacing;

    XAppPushWorld();

    D3DXVECTOR3 v;
    D3DXVec3Scale(&v, &m_direction, /*-GetRadius()*/-((float)nChildren + spacing) / 2);
    XAppTranslateWorld(v.x, v.y, v.z);

    for (int i = 0; i < nChildren; i += 1)
    {
        CNode* pNode = m_children.GetNode(i);
        if (!pNode->m_visible)
            continue;

        D3DXVec3Scale(&v, &m_direction, 0.5f/*pNode->GetRadius()*/);

        XAppTranslateWorld(v.x, v.y, v.z);

        XAppUpdateWorld();
        pNode->Render();

        if (i < nChildren - 1)
        {
            D3DXVec3Scale(&v, &m_direction, 0.5f/*pNode->GetRadius()*/ + spacing);
            XAppTranslateWorld(v.x, v.y, v.z);
        }
    }

    XAppPopWorld();
}

void CLayout::GetBBox(BBox* pBBox)
{
    if (m_children.GetLength() == 0)
    {
        CNode::GetBBox(pBBox);
        return;
    }

    float spacing = m_spacing + 1.0f;
    m_children.GetNode(0)->GetBBox(pBBox);
    for (int i = 1; i < m_children.GetLength(); i += 1)
    {
        CNode* pNode = m_children.GetNode(i);
        BBox bbox;
        pNode->GetBBox(&bbox);

        D3DXVECTOR3 v;
        D3DXVec3Scale(&v, &m_direction, spacing);
        v.x *= bbox.size.x;
        v.y *= bbox.size.y;
        v.z *= bbox.size.z;
        pBBox->size += bbox.size;

        if (pBBox->size.x > bbox.size.x)
            pBBox->size.x = bbox.size.x;
        if (pBBox->size.y > bbox.size.y)
            pBBox->size.y = bbox.size.y;
        if (pBBox->size.z > bbox.size.z)
            pBBox->size.z = bbox.size.z;
    }
}

float CLayout::GetRadius()
{
    float radius = 0.0f;

    for (int i = 0; i < m_children.GetLength(); i += 1)
        radius += m_children.GetNode(i)->GetRadius();

    if (m_children.GetLength() > 1)
        radius += (m_children.GetLength() - 1) * m_spacing / 2.0f;

    return radius;
}

////////////////////////////////////////////////////////////////////////////

class CSwitch : public CNode
{
    DECLARE_NODE(CSwitch, CNode)
public:
    CSwitch();
    ~CSwitch();

    int m_whichChoice;
    CNodeArray m_choice;

    void Render();
    void Advance(float nSeconds);

    void GetBBox(BBox* pBBox);
    float GetRadius();
#ifdef _LIGHTS
    void SetLight(int& nLight, D3DCOLORVALUE& ambient);
#endif

    DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("Switch", CSwitch, CNode)

START_NODE_PROPS(CSwitch, CNode)
    NODE_PROP(pt_integer, CSwitch, whichChoice)
    NODE_PROP(pt_nodearray, CSwitch, choice)
END_NODE_PROPS()

CSwitch::CSwitch() :
    m_whichChoice(-1)
{
}

CSwitch::~CSwitch()
{
}

void CSwitch::Render()
{
    if (m_whichChoice >= 0 && m_whichChoice < m_choice.GetLength())
    {
        CNode* pNode = m_choice.GetNode(m_whichChoice);
        if (pNode->m_visible)
            pNode->Render();
    }
}

void CSwitch::GetBBox(BBox* pBBox)
{
    if (m_whichChoice >= 0 && m_whichChoice < m_choice.GetLength())
        m_choice.GetNode(m_whichChoice)->GetBBox(pBBox);
    else
        CNode::GetBBox(pBBox);
}

float CSwitch::GetRadius()
{
    if (m_whichChoice >= 0 && m_whichChoice < m_choice.GetLength())
        return m_choice.GetNode(m_whichChoice)->GetRadius();

    return 0.0f;
}

void CSwitch::Advance(float nSeconds)
{
    CNode::Advance(nSeconds);

    for (int i = 0; i < m_choice.GetLength(); i += 1)
        m_choice.GetNode(i)->Advance(nSeconds);
}

#ifdef _LIGHTS
void CSwitch::SetLight(int& nLight, D3DCOLORVALUE& ambient)
{
    if (m_whichChoice >= 0 && m_whichChoice < m_choice.GetLength())
    {
        CNode* pNode = m_choice.GetNode(m_whichChoice);

        if (pNode != NULL)
            pNode->SetLight(nLight, ambient);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////

class CBillboard : public CGroup
{
    DECLARE_NODE(CBillboard, CGroup)
public:
    CBillboard();
    ~CBillboard();

    D3DXVECTOR3 m_axisOfRotation;

    void Render();

    D3DXMATRIX m_matrix;

    void CalcMatrix();

    DECLARE_NODE_PROPS()
};


IMPLEMENT_NODE("Billboard", CBillboard, CGroup)

START_NODE_PROPS(CBillboard, CGroup)
    NODE_PROP(pt_vec3, CBillboard, axisOfRotation)
END_NODE_PROPS()


CBillboard::CBillboard() :
    m_axisOfRotation(0.0f, 1.0f, 0.0f)
{
}

CBillboard::~CBillboard()
{
}

void CBillboard::Render()
{
    CalcMatrix();

    XAppPushWorld();
    XAppMultWorld(&m_matrix);

    XAppUpdateWorld();

    CGroup::Render();

    XAppPopWorld();
}

void CBillboard::CalcMatrix()
{
    // Determine the line between the Billboard's origin and the viewer's
    // position. This line and the axis of rotation define a plane. The
    // local z-axis of the Billboard needs to be rotated onto that plane,
    // pivoting around the axis of rotation.

    // Using the plane equation, find the distance between a unit vector
    // along the z-axis of the billboard and the plane. Use this distance
    // to create the base of right-triangle with the oposite corner at 0,0,0.
    // Now I've got the hypotenuse and one side of a right triangle and can
    // find the angle

    D3DXMATRIX world;
    D3DXMATRIX world2local;

    XAppGetTransform(D3DTS_WORLD, &world);

    D3DXMatrixInverse(&world2local, NULL, &world);

    // get eye vector in local coordinate space
    D3DXVECTOR3 eye = theCamera.m_position;
    D3DXVec3TransformNormal(&eye, &eye, &world2local);
    D3DXVec3Normalize(&eye, &eye);


    D3DXVECTOR3 axis = m_axisOfRotation;
    float alen = D3DXVec3Length(&axis);

    if (alen <= 0.001f)
    {
        // viewer-alignment

        // viewer's up vector
        D3DXVECTOR3 up(world2local.m[0][3], world2local.m[1][1], world2local.m[1][2]);
        D3DXVec3Normalize(&up, &up);

        // get x-axis vector from cross product of z and y
        D3DXVECTOR3 x;
        D3DXVec3Cross(&x, &up, &eye);
        D3DXVec3Normalize(&x, &x);

        m_matrix.m[0][0] = x.x;
        m_matrix.m[0][1] = x.y;
        m_matrix.m[0][2] = x.z;
        m_matrix.m[0][3] = 0.0f;

        m_matrix.m[1][0] = up.x;
        m_matrix.m[1][1] = up.y;
        m_matrix.m[1][2] = up.z;
        m_matrix.m[1][3] = 0.0f;

        m_matrix.m[2][0] = eye.x;
        m_matrix.m[2][1] = eye.y;
        m_matrix.m[2][2] = eye.z;
        m_matrix.m[2][3] = 0.0f;

        m_matrix.m[3][0] = 0.0f;
        m_matrix.m[3][1] = 0.0f;
        m_matrix.m[3][2] = 0.0f;
        m_matrix.m[3][3] = 1.0f;
    }
    else
    {
        // normalize axis of rotation
        axis *= 1.0f / alen;

        D3DXVECTOR3 x;
        D3DXVec3Cross(&x, &axis, &eye);
        D3DXVec3Normalize(&x, &x);

        // get z-axis vector from cross product of x and axis
        D3DXVECTOR3 z;
        D3DXVec3Cross(&z, &x, &axis);

        // calculate angle which the z-axis vector of the current coordinate system has to be rotated around
        // the y-axis of the new coordinate system to lie in its x=0 plane
        float angle = acosf(z.z);
        if (x.z > 0)
          angle = -angle;

        D3DXMatrixRotationAxis(&m_matrix, &axis, angle);


/*
        // normalize axis of rotation
        axis *= 1.0f / alen;

        // map eye & z into plane with normal axis through 0.0

        float d = -D3DXVec3Dot(&axis, &eye);
        eye += axis * d;
        D3DXVec3Normalize(&eye, &eye);


        // rotate z into plane
        D3DXVECTOR3 z(0.0f, 0.0f, 1.0f);
        d = -D3DXVec3Dot(&axis, &z);
        z += axis * d;
        D3DXVec3Normalize(&z, &z);


        // now take the angle between eye and z
        float cosw = D3DXVec3Dot(&eye, &z);

        D3DXVECTOR3 v;
        D3DXVec3Cross(&v, &eye, &z);
        float sinw = D3DXVec3Length(&v);

        float angle = acosf(cosw);


        D3DXVec3Normalize(&v, &v);

        if ((sinw > 0.0) && (D3DXVec3Dot(&axis, &v) > 0.0f))  // axis,v are opposite
        {
            axis.x = -axis.x;
            axis.y = -axis.y;
            axis.z = -axis.z;
        }

        D3DXMatrixRotationAxis(&m_matrix, &axis, angle);
*/
    }
}
