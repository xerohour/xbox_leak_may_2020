#include "std.h"
#include "xapp.h"
#include "Parser.h"
#include "Node.h"
#include "Runner.h"
#include "ActiveFile.h"
#include "xip.h"
#include "DefUse.h"

extern bool g_bParseError;


CClass::CClass() : CNodeClass(_T("[class]"), 0, NULL, NULL, NULL)
{
    m_obj = objClass;
    m_szURL = NULL;
    m_nVarCount = 0; // TODO: This should start with the count from the base class!
    m_pConstructor = NULL;
}

CClass::~CClass()
{
    delete [] m_szURL;

    if (m_pConstructor != NULL)
        m_pConstructor->Release();
}

CNode* CClass::CreateNode()
{
    return new CInstance(this);
}

// REVIEW: These four functions should be moved to CClassCompiler or a new class or globals...

bool CClass::Load(const TCHAR* szURL)
{
    CDirPush dirPush(szURL);

    int cch = _tcslen(szURL) + 1;
    delete m_szURL;
    m_szURL = new TCHAR [cch];
    CopyChars(m_szURL, szURL, cch);

    if (!m_file.Fetch(szURL))
        return false;

#ifdef _UNICODE
    m_file.MakeUnicode();
#endif

    BYTE* pbContent = m_file.DetachContent();
    bool fParse = ParseFile(szURL, (const TCHAR*)pbContent);
    XAppFreeMemory(pbContent);
    if (!fParse)
        return false;

    return true;
}

bool CClass::LoadAbsURL(const TCHAR* szURL)
{
    ASSERT(szURL[0] && szURL[1] == ':');

    int cch = _tcslen(szURL) + 1;
    delete m_szURL;
    m_szURL = new TCHAR [cch];
    CopyChars(m_szURL, szURL, cch);

    if (!m_file.Fetch(szURL))
        return false;

#ifdef _UNICODE
    m_file.MakeUnicode();
#endif

    BYTE* pbContent = m_file.DetachContent();
    bool fParse = ParseFile(szURL, (const TCHAR*)pbContent);
    XAppFreeMemory(pbContent);
    if (!fParse)
        return false;

    return true;
}

bool CClass::ParseFile(const TCHAR* szFileName, const TCHAR* szFile)
{
    const TCHAR* pch = szFile;

    StartParse(pch, szFileName);

    pch = ParseClassBody(pch);

    if (*pch != '\0')
        SyntaxError(_T("Stuff past expected end of file!"));

    EndParse();

    return !g_bParseError;
}

const TCHAR* CClass::ParseClassBody(const TCHAR* pch)
{
    CClassCompiler constructor(this);
    pch = constructor.Compile(pch);
    m_pConstructor = constructor.CreateFunction();

    return pch;
}


////////////////////////////////////////////////////////////////////////////

CInstance::CInstance(CClass* pClass)
{
    m_obj = objInstance;
    m_pClass = pClass;
    m_pClass->AddRef();

    m_vars.SetLength(pClass->GetVariableCount());

#ifdef _DEBUG
//  int nStartTime = GetTickCount();
#endif

    Construct();

#ifdef _DEBUG
//  TRACE(_T("%s took %d mS to construct\n"), pClass->m_szURL, GetTickCount() - nStartTime);
#endif

    OnLoad();
}



extern CObject* CreateNewObject(const TCHAR* pchClassName, int cchClassName, CObject** rgparam = NULL, int nParam = 0);

inline int FetchInt(BYTE*& pop)
{
    int n;
    CopyMemory(&n, pop, sizeof (int));
    pop += sizeof (int);
    return n;
}

// This will construct the scene graph of a CInstance
BYTE* CInstance::Construct()
{
//  TRACE("Construct node 0x%08x\n", this);

    BYTE* pop = m_pClass->m_pConstructor->m_rgop;

    if (pop == NULL)
        return pop;

    int nNodeStack = 0;
    CNode* nodeStack [100];

    int nNodeArrayStack = 0;
    CNodeArray* nodeArrayStack [100];

    CNode* pNode = NULL;
    CNodeArray* pNodeArray = &m_children;

    int nDefNextNodeVar = -1;

    for (;;)
    {
        BYTE op = *pop++;
//      TRACE(_T("op: %d\n"), op);

        switch (op)
        {
        default:
            // Done constructing
            return pop - 1;

        case opNewNode:
            {
                int cch = FetchInt(pop);
                const TCHAR* pch = (const TCHAR*)pop;
                pop += cch * sizeof (TCHAR);

                nodeStack[nNodeStack] = pNode;
                nNodeStack += 1;

                pNode = (CNode*)CreateNewObject(pch, cch);
                pNode->m_pParent = this;
                pNodeArray->AddNode(pNode);

                if (nDefNextNodeVar != -1)
                {
//                  TRACE(_T("Set var %d to node 0x%08x\n"), nDefNextNodeVar, pNode);

                    pNode->AddRef();
                    m_vars.SetNode(nDefNextNodeVar, pNode);
                    nDefNextNodeVar = -1;
                }
            }
            break;

        case opNewNodeProp:
            {
                PRD prd;
                CopyMemory(&prd, pop, sizeof (PRD));
                pop += sizeof (PRD);

                if (*pop == opUseNode)
                {
                    pop += 1;

                    int nVar = FetchInt(pop);

                    CNode* pNodeT = m_vars.GetNode(nVar);
                    pNode->SetProperty(&prd, &pNodeT, sizeof (CNode*));
                    break;
                }

                if (*pop == opDefNode)
                {
                    pop += 1;

                    int nVar = FetchInt(pop);

                    nDefNextNodeVar = nVar;
                }

                ASSERT(*pop == opNewNode);
                pop += 1;

                int cch = FetchInt(pop);
                const TCHAR* pch = (const TCHAR*)pop;
                pop += cch * sizeof (TCHAR);

                nodeStack[nNodeStack] = pNode;
                nNodeStack += 1;

                pNode = (CNode*)CreateNewObject(pch, cch);
                pNode->m_pParent = this;
                nodeStack[nNodeStack - 1]->SetProperty(&prd, &pNode, sizeof (CNode*));

                // Adjust for reference added by SetProperty...
                ASSERT(pNode->m_nRefCount == 2);
                pNode->Release();

                if (nDefNextNodeVar != -1)
                {
//                  TRACE(_T("Set var %d to node 0x%08x\n"), nDefNextNodeVar, pNode);

                    pNode->AddRef();
                    m_vars.SetNode(nDefNextNodeVar, pNode);
                    nDefNextNodeVar = -1;
                }
            }
            break;

        case opDefNode:
            {
                int nVar = FetchInt(pop);

                nDefNextNodeVar = nVar;
            }
            break;

        case opUseNode:
            {
                int nVar = FetchInt(pop);

                CNode* pNode = m_vars.GetNode(nVar);
                pNode->AddRef();
                pNodeArray->AddNode(pNode);
            }
            break;

        case opEndNode:
            ASSERT(nNodeStack > 0);

            pNode->OnLoad();

            nNodeStack -= 1;
            pNode = nodeStack[nNodeStack];
            break;

        case opInitProp:
            {
                PRD prd;
                CopyMemory(&prd, pop, sizeof (PRD));
                pop += sizeof (PRD);

                int cbProp = FetchInt(pop);
                const void* pValue = pop;
                pop += cbProp;

                ASSERT(pNode != NULL);

                TCHAR szBuf [1024];
                const TCHAR* sz;
                if (prd.nType == pt_string)
                {
                    CopyChars(szBuf, pValue, cbProp);
                    szBuf[cbProp] = 0;
                    sz = szBuf;
                    pValue = &sz;
#ifdef _UNICODE
                    pop += cbProp;
#endif
                }

                pNode->SetProperty(&prd, pValue, cbProp);
            }
            break;

        case opInitArray:
            {
                int nProp = FetchInt(pop);

                nodeArrayStack[nNodeArrayStack] = pNodeArray;
                nNodeArrayStack += 1;

                pNodeArray = (CNodeArray*)((BYTE*)pNode + nProp);
            }
            break;

        case opEndArray:
            ASSERT(nNodeArrayStack > 0);

            nNodeArrayStack -= 1;
            pNodeArray = nodeArrayStack[nNodeArrayStack];
            break;

        case opFunction:
            {
                int cch = FetchInt(pop);
                const TCHAR* pch = (const TCHAR*)pop;
                pop += cch * sizeof (TCHAR);
                int nFunction = FetchInt(pop);

                CObject* pFun = m_pClass->m_instanceFunctions.GetNode(nFunction);
                pFun->AddRef();
                pNode->SetMember(pch, cch, pFun);
            }
            break;
        }
    }
}


CInstance::~CInstance()
{
    m_pClass->Release();
}

CNodeClass* CInstance::GetNodeClass() const
{
    return m_pClass;
}

CObject* CInstance::Dot(CObject* pObj)
{
    if (pObj->m_obj == objVariable)
    {
        CVarObject* pVar = (CVarObject*)pObj;

        if (m_pClass->m_members != NULL)
        {
            CObject* pLookup = m_pClass->m_members->Lookup(pVar->m_sz, pVar->m_nLength);
            if (pLookup != NULL)
            {
                if (pLookup->m_obj == objMember)
                {
                    // Found a member variable...
                    CMemberVarObject* pMemberVar = new CMemberVarObject(this, ((CMember*)pLookup)->m_nMember);
                    return pMemberVar;
                }

                if (pLookup->m_obj == objFunction)
                {
                    CMemberFunctionObject* pMember = new CMemberFunctionObject(this, (CFunction*)pLookup);
                    return pMember;
                }
            }
        }
    }

    return CNode::Dot(pObj);
}

void CInstance::Render()
{
    CGroup::Render();
}
