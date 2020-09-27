#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"
#include "Lerper.h"

PRD CObject::m_rgprd [] =
{
	{ NULL, pt_null, _T("Object") },
	{ NULL, pt_null, NULL }
};

CObject::CObject()
{
	m_obj = objUndefined;
	m_nRefCount = 1;
	m_members = NULL;
	m_pParent = NULL;
}

CObject::~CObject()
{
	CLerper::RemoveObject(this);
	delete m_members;
}

void CObject::AddRef()
{
	m_nRefCount += 1;
}

void CObject::Release()
{
	m_nRefCount -= 1;
	if (m_nRefCount == 0)
		delete this;
}

CNodeClass CObject::classCObject(_T("Object"), sizeof(class CObject), CObject::CreateNode, NULL, NULL);

CNodeClass* CObject::GetNodeClass() const
{
	return &classCObject;
}

bool CObject::IsKindOf(CNodeClass* pClass) const
{
	for (CNodeClass* pCheckClass = GetNodeClass(); pCheckClass != NULL; pCheckClass = pCheckClass->m_pBaseClass)
	{
		if (pClass == pCheckClass)
			return true;
	}

	return false;
}

CObject* CObject::CreateNode()
{
	return new CObject;
}

CStrObject* CObject::ToStr()
{
	return new CStrObject(_T("[object]"));
}

CNumObject* CObject::ToNum()
{
	return new CNumObject(0.0f);
}

CObject* CObject::Deref()
{
	return this;
}

FND* CObject::GetFunctionMap() const
{
	return NULL;
}

int CObject::FindFunction(const TCHAR* pchFunction, int cchFunction)
{
	FND* rgfnd = GetFunctionMap();
	if (rgfnd == NULL)
		return -1;

	for (FND* pfnd = rgfnd; pfnd->pfn.pfn != NULL; pfnd += 1)
	{
		if ((int)_tcslen(pfnd->szName) == cchFunction && _tcsncmp(pfnd->szName, pchFunction, cchFunction) == 0)
			return (int)(pfnd - rgfnd);
	}

	return -1;
}

typedef CObject* (__cdecl CObject::*NODE_PFN_DEFAULT)(CObject**, int);

extern CObject* Dereference(CObject* pObject);

CObject* CObject::Call(int nFunction, CObject** rgparam, int nParam)
{
	ASSERT(nFunction >= 0);

	FND* pfnd = &GetFunctionMap()[nFunction];

	union FSIG fsig = pfnd->pfn;

	CObject* pRetObject = NULL;

	for (int i = 0; i < nParam; i += 1)
		rgparam[i] = Dereference(rgparam[i]);

	switch (pfnd->sig)
	{
	default:
		ASSERT(FALSE);
		return NULL;

	case sig_default:
		pRetObject = (this->*fsig.pfn_default)(rgparam, nParam);
		break;

	case sig_vv:
		{
			if (nParam != 0)
				goto LBadParamCount;

			(this->*fsig.pfn_vv)();
		}
		break;
		
	case sig_vi:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			(this->*fsig.pfn_vi)(i1);
			pNum1->Release();
		}
		break;
		
	case sig_vii:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			CNumObject* pNum2 = rgparam[1]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			int i2 = (int)pNum2->m_nValue;
			(this->*fsig.pfn_vii)(i1, i2);
			pNum1->Release();
			pNum2->Release();
		}
		break;
		
	case sig_vis:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			CStrObject* pStr1 = rgparam[1]->ToStr();
			int i1 = (int)pNum1->m_nValue;
			const TCHAR* sz1 = pStr1->GetSz();
			(this->*fsig.pfn_vis)(i1, sz1);
			pNum1->Release();
			pStr1->Release();
		}
		break;
		
	case sig_viis:
		{
			if (nParam != 3)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			CNumObject* pNum2 = rgparam[1]->ToNum();
			CStrObject* pStr1 = rgparam[2]->ToStr();
			int i1 = (int)pNum1->m_nValue;
			int i2 = (int)pNum2->m_nValue;
			const TCHAR* sz1 = pStr1->GetSz();
			(this->*fsig.pfn_viis)(i1, i2, sz1);
			pNum1->Release();
			pNum2->Release();
			pStr1->Release();
		}
		break;
		
	case sig_iv:
		{
			if (nParam != 0)
				goto LBadParamCount;

			pRetObject = new CNumObject((float)(this->*fsig.pfn_iv)());
		}
		break;

	case sig_sv:
		{
			if (nParam != 0)
				goto LBadParamCount;

			pRetObject = (this->*fsig.pfn_sv)();
		}
		break;

	case sig_vs:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CStrObject* pStr1 = rgparam[0]->ToStr();
			const TCHAR* sz1 = pStr1->GetSz();
			(this->*fsig.pfn_vs)(sz1);
			pStr1->Release();
		}
		break;

	case sig_vss:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CStrObject* pStr1 = rgparam[0]->ToStr();
			CStrObject* pStr2 = rgparam[1]->ToStr();
			const TCHAR* sz1 = pStr1->GetSz();
			const TCHAR* sz2 = pStr2->GetSz();
			(this->*fsig.pfn_vss)(sz1, sz2);
			pStr1->Release();
			pStr2->Release();
		}
		break;

	case sig_ii:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			pRetObject = new CNumObject((float)(this->*fsig.pfn_ii)(i1));
			pNum1->Release();
		}
		break;

	case sig_ni:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			pRetObject = new CNumObject((this->*fsig.pfn_ni)(i1));
			pNum1->Release();
		}
		break;

	case sig_iii:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			CNumObject* pNum2 = rgparam[1]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			int i2 = (int)pNum2->m_nValue;
			pRetObject = new CNumObject((float)(this->*fsig.pfn_iii)(i1, i2));
			pNum1->Release();
			pNum2->Release();
		}
		break;

	case sig_si:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			pRetObject = (this->*fsig.pfn_si)(i1);
			pNum1->Release();
		}
		break;

	case sig_sii:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			CNumObject* pNum2 = rgparam[1]->ToNum();
			int i1 = (int)pNum1->m_nValue;
			int i2 = (int)pNum2->m_nValue;
			pRetObject = (this->*fsig.pfn_sii)(i1, i2);
			pNum1->Release();
			pNum2->Release();
		}
		break;

	case sig_ss:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CStrObject* pStr1 = rgparam[0]->ToStr();
			const TCHAR* sz1 = pStr1->GetSz();
			pRetObject = (this->*fsig.pfn_ss)(sz1);
			pStr1->Release();
		}
		break;

	case sig_nv:
		{
			if (nParam != 0)
				goto LBadParamCount;

			pRetObject = new CNumObject((this->*fsig.pfn_nv)());
		}
		break;

	case sig_nn:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			float n1 = pNum1->m_nValue;
			pRetObject = new CNumObject((this->*fsig.pfn_nn)(n1));
			pNum1->Release();
		}
		break;

	case sig_nnn:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			float n1 = pNum1->m_nValue;
			CNumObject* pNum2 = rgparam[1]->ToNum();
			float n2 = pNum2->m_nValue;
			pRetObject = new CNumObject((this->*fsig.pfn_nnn)(n1, n2));
			pNum1->Release();
			pNum2->Release();
		}
		break;

	case sig_vn:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			float n1 = pNum1->m_nValue;
			(this->*fsig.pfn_vn)(n1);
			pNum1->Release();
		}
		break;
		
	case sig_vnn:
		{
			if (nParam != 2)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			float n1 = pNum1->m_nValue;
			CNumObject* pNum2 = rgparam[1]->ToNum();
			float n2 = pNum2->m_nValue;
			(this->*fsig.pfn_vnn)(n1, n2);
			pNum1->Release();
			pNum2->Release();
		}
		break;
		
	case sig_vnnn:
		{
			if (nParam != 3)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			float n1 = pNum1->m_nValue;
			CNumObject* pNum2 = rgparam[1]->ToNum();
			float n2 = pNum2->m_nValue;
			CNumObject* pNum3 = rgparam[2]->ToNum();
			float n3 = pNum3->m_nValue;
			(this->*fsig.pfn_vnnn)(n1, n2, n3);
			pNum1->Release();
			pNum2->Release();
			pNum3->Release();
		}
		break;
		
	case sig_vnnnn:
		{
			if (nParam != 4)
				goto LBadParamCount;

			CNumObject* pNum1 = rgparam[0]->ToNum();
			float n1 = pNum1->m_nValue;
			CNumObject* pNum2 = rgparam[1]->ToNum();
			float n2 = pNum2->m_nValue;
			CNumObject* pNum3 = rgparam[2]->ToNum();
			float n3 = pNum3->m_nValue;
			CNumObject* pNum4 = rgparam[3]->ToNum();
			float n4 = pNum4->m_nValue;
			(this->*fsig.pfn_vnnnn)(n1, n2, n3, n4);
			pNum1->Release();
			pNum2->Release();
			pNum3->Release();
			pNum4->Release();
		}
		break;
		
	case sig_ov:
		{
			if (nParam != 0)
				goto LBadParamCount;

			pRetObject = (this->*fsig.pfn_ov)();
		}
		break;

	case sig_os:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CStrObject* pStr1 = rgparam[0]->ToStr();
			const TCHAR* sz1 = pStr1->GetSz();
			pRetObject = (this->*fsig.pfn_os)(sz1);
			pStr1->Release();
		}
		break;

	case sig_oo:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CObject* pObj1 = rgparam[0];
			pRetObject = (this->*fsig.pfn_oo)(pObj1);
		}
		break;

	case sig_vo:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CObject* pObj1 = rgparam[0];
			(this->*fsig.pfn_vo)(pObj1);
		}
		break;

	case sig_is:
		{
			if (nParam != 1)
				goto LBadParamCount;

			CStrObject* pStr1 = rgparam[0]->ToStr();
			const TCHAR* sz1 = pStr1->GetSz();
			pRetObject = new CNumObject((float)(this->*fsig.pfn_is)(sz1));
			pStr1->Release();
		}
		break;
	}

	return pRetObject;

LBadParamCount:
	g_pRunner->Error(_T("invalid number of parameters"));
	return NULL;
}

CObject* CObject::Dot(CObject* pObj)
{
	if (pObj->m_obj == objVariable)
	{
		CVarObject* pVar = (CVarObject*)pObj;
		int nFunction = FindFunction(pVar->m_sz, pVar->m_nLength);
		if (nFunction >= 0)
		{
			CFunctionObject* pFun = new CFunctionObject;
			pFun->m_nFunction = nFunction;
			pFun->m_pObject = this;
			AddRef();

			return pFun;
		}

		CObject* pObject = GetMember(pVar->m_sz, pVar->m_nLength);
		if (pObject != NULL)
		{
			pObject->AddRef();
			return pObject;
		}
	}

	if (pObj->m_obj == objVariable)
	{
		CVarObject* pVar = (CVarObject*)pObj;
		const PRD* pprd = FindProp(pVar->m_sz, pVar->m_nLength);
		if (pprd != NULL)
		{
			return new CProperty(this, pprd);
		}
	}

	return NULL;
}

void CObject::AddMember(const TCHAR* pchName, int cchName, CObject* pObject)
{
	if (m_members == NULL)
		m_members = new CNameSpace;

	CDefine* pDefine = m_members->Add(pchName, cchName);
	pDefine->m_pNode = (CNode*)pObject;
}

void CObject::SetMember(const TCHAR* pchName, int cchName, CObject* pObject)
{
	if (m_members == NULL)
		m_members = new CNameSpace;

	CDefine* pDefine = m_members->Get(pchName, cchName);
	pDefine->m_pNode = (CNode*)pObject;
}

CObject* CObject::GetMember(const TCHAR* pchName, int cchName)
{
	if (m_members == NULL)
		return NULL;

	return m_members->Lookup(pchName, cchName);
}


extern CObject* FindMember(CNodeClass* pClass, const TCHAR* pchName, int cchName);

CFunction* CObject::FindMemberFunction(const TCHAR* pchName, int cchName)
{
	// BLOCK: Try for an instance function...
	{
		CFunction* pFunction = (CFunction*)GetMember(pchName, cchName);
		if (pFunction != NULL && pFunction->m_obj == objFunction)
			return pFunction;
	}

	// BLOCK: Try for a class member (all the way up through the base classes...
	{
		CFunction* pFunction = (CFunction*)FindMember(GetNodeClass(), pchName, cchName);
		if (pFunction != NULL && pFunction->m_obj == objFunction)
			return pFunction;
	}

	return NULL;
}


#ifdef _DEBUG

void CObject::Dump() const
{
	static const TCHAR* rgszType [] =
	{
		_T("Unknown"),
		_T("Null"),
		_T("Number"),
		_T("String"),
		_T("Variable"),
		_T("Node"),
		_T("Class"),
		_T("NodeArray"),
		_T("FunctionRef"),
		_T("Member"),
		_T("MemberVar"),
		_T("Instance"),
		_T("Use"),
		_T("Function"),
		_T("MemberFunction"),
		_T("Array"),
	};

	TRACE(_T("{\nObject: 0x%08x\n"), this);
	if (this != NULL)
	{
		TRACE(_T("Type: %s\n"), rgszType[m_obj]);
		TRACE(_T("Parent: 0x%08x\n"), m_pParent);
		TRACE(_T("RefCount: %d\n"), m_nRefCount);

		CNodeClass* pNodeClass = GetNodeClass();
		if (pNodeClass != NULL)
			TRACE(_T("Class: %s\n"), pNodeClass->m_szClassName);
		else
			TRACE(_T("Class: null\n"));

		if (m_members != NULL)
		{
			TRACE(_T("members:\n"));
			m_members->Dump();
		}
	}

	TRACE(_T("}\n"));
}
#endif
