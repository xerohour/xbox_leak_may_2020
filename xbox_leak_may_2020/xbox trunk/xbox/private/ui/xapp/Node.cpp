#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"

#ifdef _DEBUG
int SizeOfType(PROP_TYPE type)
{
	switch (type)
	{
	default:
		return 0;

	case pt_boolean:
		return sizeof (bool);

	case pt_integer:
		return sizeof (int);

	case pt_number:
		return sizeof (float);

	case pt_string:
		return sizeof (TCHAR*);

	case pt_children:
		return sizeof (CNodeArray);

	case pt_vec3:
	case pt_color:
		return sizeof (D3DXVECTOR3);

	case pt_vec4:
	case pt_quaternion:
		return sizeof (D3DXVECTOR4);

	case pt_node:
		return sizeof (CNode*);
	}
}
#endif

CNodeClass* CNodeClass::c_pFirstClass = NULL;

CNodeClass::CNodeClass(const TCHAR* szClassName, int nObjectSize, CObject* (*pfnCreateNode)(), CNodeClass* pBaseClass, const PRD* rgprd)
{
	m_szClassName = szClassName;
	m_nObjectSize = nObjectSize;
	m_pfnCreateNode = pfnCreateNode;
	m_pBaseClass = pBaseClass;
	m_pNextClass = c_pFirstClass;
	m_rgprd = rgprd;
	c_pFirstClass = this;
}

CNodeClass::~CNodeClass()
{
	for (CNodeClass** ppClass = &c_pFirstClass; *ppClass != NULL && *ppClass != this; ppClass = &(*ppClass)->m_pNextClass)
		;

	if (*ppClass == this)
		*ppClass = m_pNextClass;
#ifdef _DEBUG
	else
		ASSERT(FALSE);
#endif
}

CObject* CNodeClass::CreateNode()
{
	return (*m_pfnCreateNode)();
}

CNodeClass* CNodeClass::FindByName(const TCHAR* pchNodeClass, int cchNodeClass)
{
	for (CNodeClass* pClass = CNodeClass::c_pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
	{
		if (_tcsncmp(pClass->m_szClassName, pchNodeClass, cchNodeClass) == 0 && pClass->m_szClassName[cchNodeClass] == '\0')
			return pClass;
	}

	return NULL;
}

CObject* NewNode(const TCHAR* pchNodeClass, int cchNodeClass)
{
	CNodeClass* pNodeClass = CNodeClass::FindByName(pchNodeClass, cchNodeClass);
	if (pNodeClass == NULL)
	{
		return NULL;
	}

	return pNodeClass->CreateNode();
}



PRD CNode::m_rgprd [] =
{
	{ NULL, pt_null, _T("Node") },
	NODE_PROP(pt_boolean, CNode, visible)
	{ NULL, pt_null, NULL }
};

CNode::CNode()
{
	m_obj = objNode;
	m_pBehavior = NULL;
	m_visible = true;
}

CNode::~CNode()
{
	delete m_pBehavior;
}

void CNode::OnLoad()
{
	CFunction* pFunction = FindMemberFunction(_T("behavior"), 8);
	if (pFunction != NULL)
	{
		m_pBehavior = new CRunner(this);
		m_pBehavior->SetFunc(pFunction);
	}
}

CObject* CNode::CreateNode()
{
	return new CNode;
}

CNodeClass CNode::classCNode(_T("Node"), sizeof(class CNode), CNode::CreateNode, NULL, CNode::m_rgprd);

CNodeClass* CNode::GetNodeClass() const
{
	return &classCNode;
}

const PRD* CNodeClass::FindProp(const TCHAR* szName, int cchName)
{
	const PRD* rgprd = GetPropMap();

	while (rgprd != NULL)
	{
		const PRD* pNext = (const PRD*)rgprd[0].pbOffset;

		for (int i = 1; rgprd[i].szName != NULL/*rgprd[i].nType != pt_null*/; i += 1)
		{
			if (_tcsnicmp(szName, rgprd[i].szName, cchName) == 0 && rgprd[i].szName[cchName] == '\0')
				return &rgprd[i];
		}

		rgprd = pNext;
	}

	return NULL;
}

const PRD* CObject::FindProp(const TCHAR* szName, int cchName)
{
	const PRD* rgprd = GetPropMap();

	while (rgprd != NULL)
	{
		const PRD* pNext = (const PRD*)rgprd[0].pbOffset;

		for (int i = 1; rgprd[i].szName != NULL/*rgprd[i].nType != pt_null*/; i += 1)
		{
			if (_tcsnicmp(szName, rgprd[i].szName, cchName) == 0 && rgprd[i].szName[cchName] == '\0')
				return &rgprd[i];
		}

		rgprd = pNext;
	}

	return NULL;
}

bool CObject::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	return true;
}

void CObject::SetProperty(const PRD* pprd, const void* pvValue, int cbValueIn)
{
	BYTE* pbThisValue = (BYTE*)this + (int)pprd->pbOffset;

	if (pprd->nType == pt_string)
	{
		// Shortcut equal strings...

		TCHAR* szOld = *(TCHAR**)pbThisValue;
		TCHAR* szNew = *(TCHAR**)pvValue;
		if (szOld != NULL && szNew != NULL && _tcscmp(szOld, szNew) == 0)
			return;
	}

	if (!OnSetProperty(pprd, pvValue))
		return;

	switch (pprd->nType)
	{
	case pt_intarray:
		{
			CIntArray* pArray = (CIntArray*)pbThisValue;
			pArray->SetSize(cbValueIn / sizeof (int));
			CopyMemory(pArray->m_value, pvValue, cbValueIn);
		}
		break;

	case pt_numarray:
		{
			CNumArray* pArray = (CNumArray*)pbThisValue;
			pArray->SetSize(cbValueIn / sizeof (float));
			CopyMemory(pArray->m_value, pvValue, cbValueIn);
		}
		break;

	case pt_vec2array:
		{
			CVec2Array* pArray = (CVec2Array*)pbThisValue;
			pArray->SetSize(cbValueIn / (sizeof (float) * 2));
			CopyMemory(pArray->m_value, pvValue, cbValueIn);
		}
		break;

	case pt_vec3array:
		{
			CVec3Array* pArray = (CVec3Array*)pbThisValue;
			pArray->SetSize(cbValueIn / (sizeof (float) * 3));
			CopyMemory(pArray->m_value, pvValue, cbValueIn);
		}
		break;

	case pt_vec4array:
		{
			CVec4Array* pArray = (CVec4Array*)pbThisValue;
			pArray->SetSize(cbValueIn / (sizeof (float) * 4));
			CopyMemory(pArray->m_value, pvValue, cbValueIn);
		}
		break;

	case pt_string:
		{
			// NOTE: cbValueIn is really a character count!

			TCHAR* szOld = *(TCHAR**)pbThisValue;
			delete [] szOld;

			TCHAR* sz = *(TCHAR**)pvValue;
			ASSERT((int)_tcslen(sz) == cbValueIn);
			TCHAR* szNew = new TCHAR [cbValueIn + 1];
			CopyChars(szNew, sz, cbValueIn);
			szNew[cbValueIn] = 0;
			*(TCHAR**)pbThisValue = szNew;
		}
		break;

	default:
		if (pprd->nType == pt_node)
		{
			CNode* pNode = *(CNode**)pbThisValue;
			if (pNode != NULL)
				pNode->Release();
		}

		ASSERT(SizeOfType(pprd->nType) == cbValueIn);
		CopyMemory(pbThisValue, pvValue, cbValueIn);

		if (pprd->nType == pt_node)
		{
			CNode* pNode = *(CNode**)pvValue;
			if (pNode != NULL)
				pNode->AddRef();
		}
		break;
	}
}

void CNode::Render()
{
	// If we get here, the node subclass does not have a render function, 
	// so don't bother calling it anymore!
	m_visible = false;
}

void CNode::Advance(float nSeconds)
{
	if (m_pBehavior != NULL && !m_pBehavior->IsSleeping())
	{
		g_pRunner = m_pBehavior;
		g_pThis = this;

		do
		{
			if (!m_pBehavior->Step())
			{
				// The function completed, so start it over!
				m_pBehavior->ResetFunc();
			}
		}
		while (!m_pBehavior->IsSleeping());
	}
}

float CNode::GetGroundHeight(float x, float z)
{
	return 0.0f;
}

void CNode::GetBBox(BBox* pBBox)
{
	pBBox->center.x = 0.0f;
	pBBox->center.y = 0.0f;
	pBBox->center.z = 0.0f;
	pBBox->size.x = 0.0f;
	pBBox->size.y = 0.0f;
	pBBox->size.z = 0.0f;
}

float CNode::GetRadius()
{
	return 0.0f;
}

#ifdef _LIGHTS
void CNode::SetLight(int& nLight, D3DCOLORVALUE& ambient)
{
}
#endif

LPDIRECT3DTEXTURE8 CNode::GetTextureSurface()
{
	return NULL;
}

void CNode::RenderDynamicTexture(CSurfx* pSurfx)
{
}

const DWORD* CNode::GetPalette()
{
	return NULL;
}


#ifdef _DEBUG
void CNode::Dump() const
{
	CObject::Dump();

	for (const PRD* rgprd = GetPropMap(); rgprd != NULL; rgprd = (const PRD*)rgprd[0].pbOffset)
	{
		for (int i = 1; rgprd[i].szName != NULL; i += 1)
		{
			TRACE(_T("prop: '%s::%s' = "), rgprd[0].szName, rgprd[i].szName);
			const BYTE* pbThisValue = (const BYTE*)this + (int)rgprd[i].pbOffset;
			switch (rgprd[i].nType)
			{
			case pt_integer:
				TRACE(_T("%d"), *((const int*)pbThisValue));
				break;

			case pt_boolean:
				TRACE(_T("%s"), *((const bool*)pbThisValue) ? _T("true") : _T("false"));
				break;

			case pt_number:
				TRACE(_T("%f"), *((const float*)pbThisValue));
				break;

			case pt_string:
				TRACE(_T("%s"), *((const TCHAR**)pbThisValue));
				break;

			// FUTURE: Dump other types too!
			}

			TRACE(_T("\n"));
		}
	}
}
#endif

////////////////////////////////////////////////////////////////////////////


CNodeArray::CNodeArray()
{
	m_nLength = 0;
	m_nAlloc = 0;
	m_rgpNode = NULL;
}

CNodeArray::~CNodeArray()
{
	ReleaseAll();
}

void CNodeArray::AddNode(CNode* pNode)
{
	if (m_nAlloc < m_nLength + 1)
		Allocate(m_nAlloc + 16);

	m_rgpNode[m_nLength++] = pNode;
}

void CNodeArray::RemoveNode(CNode* pNode)
{
	for (int i = 0; i < m_nLength; i += 1)
	{
		if (m_rgpNode[i] == pNode)
		{
			CopyMemory(&m_rgpNode[i], &m_rgpNode[i + 1], (m_nLength - i) - 1);
			m_nLength -= 1;
			i -= 1;
		}
	}
}

void CNodeArray::ReleaseAll()
{
	for (int i = 0; i < m_nLength; i += 1)
	{
		if (m_rgpNode[i] != NULL)
			m_rgpNode[i]->Release();
	}

	RemoveAll();
}

void CNodeArray::RemoveAll()
{
	m_nLength = 0;
	m_nAlloc = 0;
	delete [] m_rgpNode;
	m_rgpNode = NULL;
}

void CNodeArray::Allocate(int nLength)
{
	if (m_nAlloc < nLength)
	{
		CNode** rgpNode = new CNode* [nLength];

		if (m_rgpNode != NULL)
		{
			CopyMemory(rgpNode, m_rgpNode, m_nAlloc * sizeof (CNode*));
			delete [] m_rgpNode;
		}

		m_rgpNode = rgpNode;
		m_nAlloc = nLength;
	}
}

void CNodeArray::SetLength(int nLength)
{
	Allocate(nLength);

	if (nLength > m_nLength)
		ZeroMemory(m_rgpNode + m_nLength, (nLength - m_nLength) * sizeof (CNode*));

	m_nLength = nLength;
}


////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("TimeDepNode", CTimeDepNode, CNode)

START_NODE_PROPS(CTimeDepNode, CNode)
	NODE_PROP(pt_boolean, CTimeDepNode, loop)
	NODE_PROP(pt_number, CTimeDepNode, startTime)
	NODE_PROP(pt_number, CTimeDepNode, stopTime)
	NODE_PROP(pt_boolean, CTimeDepNode, isActive)
END_NODE_PROPS()

CTimeDepNode::CTimeDepNode() :
	m_loop(false),
	m_startTime(0),
	m_stopTime(0),
	m_isActive(false)
{
	m_lastStartTime = 0.0f;
}

void CTimeDepNode::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_isActive)
	{
		if (m_stopTime > m_startTime && theApp.m_now >= m_stopTime)
		{
			m_isActive = false;
			OnIsActiveChanged();
		}
	}
	else
	{
		if (theApp.m_now >= m_startTime && m_startTime != m_lastStartTime && (m_stopTime == 0.0f || theApp.m_now < m_stopTime))
		{
			m_lastStartTime = m_startTime;
			m_isActive = true;
			OnIsActiveChanged();
		}
	}
}

void CTimeDepNode::OnIsActiveChanged()
{
}

void CTimeDepNode::OnCycleEnded()
{
}
