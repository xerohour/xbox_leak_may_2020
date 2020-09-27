#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"
#include "Camera.h"
#include "Parser.h"

#include "..\..\ntos\inc\xlaunch.h"

//#define LOCALTRACE TRACE
#define LOCALTRACE 1 ? (void)0 : ::Trace

CObject* LookupMember(CObject* pThis, const TCHAR* pchName, int cchName);

CObject* g_pThis;
CRunner* g_pRunner;

extern bool g_bInputEnable;

class CVec3Object : public CObject
{
    DECLARE_NODE(CVec3Object, CObject)
public:
    CVec3Object();
    CVec3Object(const D3DXVECTOR3& v);
    CStrObject* ToStr();

    float m_x, m_y, m_z;

    DECLARE_NODE_PROPS()
};

bool ExecuteScript(CObject* pObject, const TCHAR* szScript)
{
    CFunctionCompiler compiler;
    compiler.ParseBlock(szScript);
    if (compiler.HadError())
        return false;

    compiler.Write(opRet);
    CFunction* pFunction = compiler.CreateFunction();

    CRunner runner(pObject);
    runner.SetFunc(pFunction);
    runner.Run();

    delete pFunction;

    return true;
}

bool CallFunction(CObject* pObject, const TCHAR* szFunc, int nParam, CObject** rgParam)
{
    g_pThis = pObject;
    CRunner runner(pObject);
    if (!runner.SetFunc(szFunc))
        return false;

    for (int i = 0; i < nParam; i += 1)
        runner.Push(rgParam[i]);

    runner.Run();
    return true;
}

CObject* Dereference(CObject* pObject)
{
    CObject* pLastObject = pObject;

    while (pObject != NULL)
    {
        pObject = pLastObject->Deref();
        if (pObject == pLastObject)
            break;

        pLastObject = pObject;
    }

    return pObject;
}

////////////////////////////////////////////////////////////////////////////

class CLocalVariable : public CObject
{
public:
    CLocalVariable();
    ~CLocalVariable();

    CObject* Deref();
    void Assign(CObject* pObject);

    CObject* m_pValue;
};

CLocalVariable::CLocalVariable()
{
    m_pValue = NULL;
}

CLocalVariable::~CLocalVariable()
{
    if (m_pValue != NULL)
        m_pValue->Release();
}

void CLocalVariable::Assign(CObject* pObject)
{
    if (m_pValue != NULL)
        m_pValue->Release();

    m_pValue = pObject;

    if (m_pValue != NULL)
        m_pValue->AddRef();
}

CObject* CLocalVariable::Deref()
{
    CObject* pObject = m_pValue;

    if (pObject != NULL)
        pObject->AddRef();

    Release();

    return pObject;
}

////////////////////////////////////////////////////////////////////////////

CRunner::CRunner(CObject* pObject)
{
    ASSERT(pObject != NULL);

    m_nop = 0;
    m_ops = NULL;
    m_sp = 0;
    m_spBase = 0;
    m_spFrame = 0;
    m_pSelf = pObject;
    m_pThis = pObject;
    m_bError = false;
    m_pNextContext = NULL;
    m_wakeup = 0.0f;

    ZeroMemory(m_stack, sizeof (m_stack));

#if defined(_DEBUG)
//  _CrtMemCheckpoint(&m_memState);
#endif
}

CRunner::~CRunner()
{
    ResetFunc();
}

bool CRunner::SetFunc(const TCHAR* szFunc)
{
    ResetFunc();

    m_ops = LookupFunction(szFunc, _tcslen(szFunc), m_pThis);
    return m_ops != NULL;
}

void CRunner::ResetFunc()
{
    while (m_sp > 0)
    {
        m_sp -= 1;
        CObject* pObj = m_stack[m_sp];
        m_stack[m_sp] = NULL;
        if (pObj != NULL)
            pObj->Release();
    }

    m_bError = false;
    m_nop = 0;
    m_sp = 0;
    m_spFrame = 0;
    m_wakeup = 0.0f;

#if defined(_DEBUG)
//  _CrtMemDumpAllObjectsSince(&m_memState);
//  _CrtMemCheckpoint(&m_memState);
#endif
}

// REVIEW: Inline Push and Pop...

void CRunner::Push(float nValue)
{
    CNumObject* pObj = new CNumObject(nValue);
    Push(pObj);
}

void CRunner::Push(int nValue)
{
    CNumObject* pObj = new CNumObject((float)nValue);
    Push(pObj);
}

void CRunner::Push(const TCHAR* szValue, int nLen /*=-1*/)
{
    if (nLen == -1)
        nLen = _tcslen(szValue);

    CStrObject* pObj = new CStrObject(szValue, nLen);
    Push(pObj);
}

void CRunner::Push(CObject* pObject)
{
    m_stack[m_sp] = pObject;
    m_sp += 1;
    ASSERT(m_sp < sizeof (m_stack) / sizeof (CObject*));
}

CObject* CRunner::Pop()
{
    ASSERT(m_sp > 0);
    m_sp -= 1;

    CObject* pObject = m_stack[m_sp];
    m_stack[m_sp] = NULL;

    return pObject;
}



CClass* LookupClass(const TCHAR* pchName, int cchName)
{
    extern CNameSpace* g_classes;

    CObject* pObject = g_classes->Lookup(pchName, cchName);
    if (pObject == NULL || pObject->m_obj != objClass)
        return NULL;

    return (CClass*)pObject;
}


CObject** g_rgParam;
int g_nParam;

CObject* CreateNewObject(const TCHAR* pchClassName, int cchClassName, CObject** rgparam = NULL, int nParam = 0)
{
    g_rgParam = rgparam;
    g_nParam = nParam;

    // BLOCK: Look for class...
    {
        CClass* pClass = LookupClass(pchClassName, cchClassName);
        if (pClass != NULL)
            return pClass->CreateNode();
    }

    // BLOCK: Try a built-in node...
    {
        CObject* pNode = NewNode(pchClassName, cchClassName);
        if (pNode != NULL)
            return pNode;
    }

    return NULL;
}

void CRunner::SetFunc(CFunction* pFunction)
{
    ResetFunc();

    m_nop = 0;
    m_sp = 0;
    m_spFrame = 0;
    m_ops = pFunction->m_rgop;
    m_nLine = 0;
    m_wakeup = 0.0f;
}

void CRunner::Error(const TCHAR* szFmt, ...)
{
    if (m_bError)
        return;

    va_list args;
    va_start(args, szFmt);

    TCHAR szBuffer [512];
    _vsntprintf(szBuffer, countof (szBuffer), szFmt, args);

    TCHAR szMessage [1024];
    if (m_nLine == 0)
        _stprintf(szMessage, _T("Runtime Error\n\n%s"), szBuffer);
    else
        _stprintf(szMessage, _T("Runtime Error\n\nLine: %d\n\n%s"), m_nLine, szBuffer);

    Trace(_T("\001\007%s\n"), szMessage);

#ifdef _DEBUG
    DumpStack();
#endif

#ifdef _WINDOWS
    XAppMessageBox(szMessage);
#endif

    va_end(args);

    m_bError = true;
}

void CRunner::PushContext(UINT nParam)
{
    RUNCONTEXT* pContext = new RUNCONTEXT;

    pContext->m_ops = m_ops;
    pContext->m_nop = m_nop;
    pContext->m_spFrame = m_spFrame;
    pContext->m_pSelf = m_pSelf;
    pContext->m_pThis = m_pThis;
    pContext->m_sp = m_sp - (nParam + 1);

    pContext->m_pNextContext = m_pNextContext;
    m_pNextContext = pContext;
}

void CRunner::PopContext()
{
    RUNCONTEXT* pContext = m_pNextContext;
    ASSERT(pContext != NULL);

    CObject* pRetObject = Dereference(Pop());

    m_ops = pContext->m_ops;
    m_nop = pContext->m_nop;
    m_spFrame = pContext->m_spFrame;
    m_pSelf = pContext->m_pSelf;
    m_pThis = pContext->m_pThis;
    g_pThis = m_pThis;

    for (UINT i = pContext->m_sp; i < m_sp; i += 1)
    {
        if (m_stack[i] != NULL)
        {
            m_stack[i]->Release();
            m_stack[i] = NULL;
        }
    }

    m_sp = pContext->m_sp;

    m_pNextContext = pContext->m_pNextContext;
    delete pContext;

    Push(pRetObject);
}

CObject* CRunner::Run()
{
    m_spBase = m_sp;

    for (;;)
    {
        if (m_bError)
            return NULL;

#ifdef _DEBUG
        // Make sure everything on the stack has been cleaned up!
        {
            for (UINT i = 0; i < 20; i += 1)
                ASSERT(m_stack[m_sp + i] == NULL);
        }
#endif
        CObject* pRetObj = NULL;
        if (!Step(&pRetObj))
            return pRetObj;
    }
}

bool CRunner::Step(CObject** ppRetObj/*=NULL*/)
{
    g_pRunner = this;

    BYTE op = m_ops[m_nop++];
    switch (op)
    {
    default:
        ASSERT(FALSE); // Lost in space!
        return NULL;

    case opSleep:
        {
            CObject* pObj = Dereference(Pop());
            if (pObj == NULL)
            {
                Error(_T("illegal null reference"));
                return false;
            }

            if (pObj->m_obj != objNumber)
            {
                Error(_T("expected a number"));
                return false;
            }

            m_wakeup = XAppGetNow() + ((CNumObject*)pObj)->m_nValue;
            pObj->Release();
            return true;
        }
        break;

    case opStatement:
        {
            m_nLine = FetchInt();
            LOCALTRACE(_T("opStatement: line %d, sp %d\n"), m_nLine, m_sp);
        }
        break;

    case opDrop:
        {
            LOCALTRACE(_T("opDrop\n"));
            CObject* pObject = Pop();
            if (pObject != NULL)
                pObject->Release();
        }
        break;

    case opFrame:
        {
            int nFrameSize = FetchInt();
            LOCALTRACE(_T("opFrame: %d\n"), nFrameSize);

#ifdef _DEBUG
            // Make sure everything on the stack has been cleaned up!
            {
                for (UINT i = m_sp; i < m_sp + nFrameSize; i += 1)
                    ASSERT(m_stack[i] == NULL);
            }
#endif
//          ZeroMemory(&m_stack[m_sp], sizeof (CObject*) * nFrameSize); // REVIEW: Not necessary!
            m_sp += nFrameSize;
            m_spBase = m_sp;
            ASSERT(m_sp < sizeof (m_stack) / sizeof (CObject*));
        }
        break;

    case opEndFrame:
        {
            int nFrameSize = FetchInt();
            LOCALTRACE(_T("opEndFrame: %d\n"), nFrameSize);

            // Remove parameters from the stack...
            while (nFrameSize-- > 0)
            {
                m_sp -= 1;
                if (m_stack[m_sp] != NULL)
                {
                    m_stack[m_sp]->Release();
                    m_stack[m_sp] = NULL;
                }
            }

            m_spBase = m_sp;
        }
        break;

    case opNull:
        {
            Push((CObject*)NULL);
        }
        break;

    case opThis:
        {
            ASSERT(m_pSelf != NULL);
            m_pSelf->AddRef();
            Push(m_pSelf);
        }
        break;

    case opNew: // New: nParam cchClass rgchClass
        {
            LOCALTRACE(_T("opNew\n"));

            int nParam = FetchInt();
            int cch;
            const TCHAR* pch = FetchString(cch);
            CObject** rgparam = &m_stack[m_sp - nParam];

            CObject* pObj = CreateNewObject(pch, cch, rgparam, nParam);

            if (pObj == NULL)
            {
                TCHAR szBuf [256];
                if (cch > 255)
                    cch = 255;
                CopyChars(szBuf, pch, cch);
                szBuf[cch] = '\0';
                Error(_T("Failed to create new object: \"%s\""), szBuf);
                return NULL;
            }

            // Remove parameters from the stack...
            for (int i = 0; i < nParam; i += 1)
            {
                CObject* pObj = rgparam[i];
                if (pObj != NULL)
                {
                    pObj->Release();
                    rgparam[i] = NULL;
                }
            }

            m_sp -= nParam;

            Push(pObj);
        }
        break;

    case opAssign:
        {
            LOCALTRACE(_T("opAssign\n"));

            CObject* pObjRight = Dereference(Pop());
            CObject* pObjLeft = Pop();

            if (pObjLeft == NULL)
            {
                Error(_T("cannot assign to null"));
                return NULL;
            }

            pObjLeft->Assign(pObjRight);
            pObjLeft->Release();

            // Keep the value on the stack...
            Push(pObjRight);
        }
        break;

    case opDot:
    case opArray:
        {
            LOCALTRACE(_T("%d %s: \"%s\"\n"), m_nop - 1, op == opDot ? "opDot" : "opArray");

            CObject* pObjRight = Pop();
            CObject* pObjLeft = Pop();

            if (pObjRight == NULL || pObjLeft == NULL)
            {
                Error(_T("illegal null reference"));
                return NULL;
            }

            if (op == opArray)
                pObjRight = Dereference(pObjRight);

            ASSERT(pObjRight->m_obj == objVariable || pObjRight->m_obj == objNumber || pObjRight->m_obj == objString);

            CObject* pRealLeft = pObjLeft->Deref();
            if (pRealLeft == NULL)
            {
                Error(_T("illegal null reference"));
                return NULL;
            }

            CObject* pDot = pRealLeft->Dot(pObjRight);

            if (pDot == NULL)
            {
                if (pObjRight->m_obj == objVariable)
                {
                    CVarObject* pVar = (CVarObject*)pObjRight;
                    Error(_T("Unknown member: %s"), pVar->m_sz);
                }
                else
                {
                    CStrObject* pStr = pObjRight->ToStr();
                    Error(_T("Unknown member: %s"), pStr->GetSz());
                    pStr->Release();
                }
            }

            Push(pDot);

            pRealLeft->Release();
            pObjRight->Release();
        }
        break;

    case opNeg:
        {
            CObject* pObj = Dereference(Pop());
            if (pObj == NULL || pObj->m_obj != objNumber)
            {
                Error(_T("expected a number"));
                return NULL;
            }

            Push(-((CNumObject*)pObj)->m_nValue);
            pObj->Release();
        }
        break;

    case opAdd:
    case opSub:
    case opMul:
    case opDiv:
    case opMod:
    case opEQ:
    case opNE:
    case opLT:
    case opLE:
    case opGT:
    case opGE:
    case opAnd:
    case opXor:
    case opOr:
    case opSHL:
    case opSHR:
        if (!BinaryOperator(op))
            return NULL;
        break;

    case opCall:
        {
            LOCALTRACE(_T("opCall sp=%d\n"), m_sp);

            CObject* pRetObj = NULL;;
            int nParam = m_ops[m_nop++];
            ASSERT(m_sp >= (UINT)(nParam + 1));
            CObject* pFun = (CVarObject*)m_stack[m_sp - (nParam + 1)];
            CObject** rgparam = &m_stack[m_sp - nParam];

            if (pFun->m_obj == objFunctionRef)
            {
                // This is a member of a built-in class...

                m_stack[m_sp - (nParam + 1)] = NULL;

                CFunctionObject* pFunction = (CFunctionObject*)pFun;
                pRetObj = pFunction->Call(rgparam, nParam);
                pFun->Release();

                for (int i = 0; i < nParam; i += 1)
                {
                    CObject* pObj = rgparam[i];
                    if (pObj != NULL)
                    {
                        pObj->Release();
                        rgparam[i] = NULL;
                    }
                }

                m_sp -= nParam + 1;
                LOCALTRACE(_T("opCall(done) sp=%d\n"), m_sp);

                Push(pRetObj);
            }
            else if (pFun->m_obj == objMemberFunction)
            {
                // This is a member of a scripted class and the script specified the object in the call...

                CMemberFunctionObject* pFunction = (CMemberFunctionObject*)pFun;
                pFunction->AddRef();

#ifdef _DEBUG
                LOCALTRACE(_T("Call2 %s\n"), pFunction->m_pFunction->m_szName);
#endif
                PushContext(nParam);

                m_spFrame = m_sp - nParam;

                ASSERT(pFunction->m_pObject != NULL);
                m_pSelf = pFunction->m_pObject;

                m_ops = pFunction->m_pFunction->m_rgop;
                m_nop = 0;
            }
            else
            {
                CVarObject* pFunction = (CVarObject*)pFun;
                int cch = pFunction->m_nLength;
                const TCHAR* pch = pFunction->m_sz;

                if (ExecuteBuiltIn(pch, cch, nParam, rgparam, pRetObj))
                {
                    // This was an intrinsic...

                    for (int i = 0; i < nParam; i += 1)
                        rgparam[i] = NULL;
                    m_sp -= nParam + 1;
                    Push(pRetObj);
                    pFun->Release();
                }
                else
                {
                    // This is an unqualified member of a scripted class...

                    CObject* pThis;
                    BYTE* pop = LookupFunction(pch, cch, pThis);
                    if (pop == NULL)
                    {
                        TCHAR szFunc [32];
                        if (cch > countof(szFunc) - 1)
                            cch = countof(szFunc) - 1;
                        _tcsncpy(szFunc, pch, cch);
                        szFunc[cch] = 0;
                        Error(_T("unknown function: %s"), szFunc);
                        return NULL;
                    }

                    PushContext(nParam);

                    m_pSelf = pThis;
                    g_pThis = pThis;

                    m_spFrame = m_sp - nParam;
                    m_ops = pop;
                    m_nop = 0;
                }
            }
        }
        break;

    case opRet:
        if (m_pNextContext != NULL)
        {
            PopContext();
        }
        else
        {
            if (ppRetObj != NULL)
                *ppRetObj = m_sp > 0 ? Pop() : NULL;
            return false;
        }
        break;

    case opCond:
        {
            LOCALTRACE(_T("opCond\n"));
            bool bCond = false;
            CObject* pObj = Dereference(Pop());

            if (pObj != NULL)
            {
                CNumObject* pNum = pObj->ToNum();
                if (pNum->m_nValue != 0.0f)
                    bCond = true;

                pNum->Release();
                pObj->Release();
            }

            if (bCond)
                m_nop += sizeof (UINT);
            else
                m_nop = FetchUInt();
        }
        break;

    case opJump:
        LOCALTRACE(_T("opJump\n"));
        m_nop = FetchUInt();
        break;

    case opVar:
        {
            LOCALTRACE(_T("opVar\n"));
            int cch;
            const TCHAR* pch = FetchString(cch);

#ifdef _DEBUG
            {
                TCHAR szBuf [256];
                CopyChars(szBuf, pch, cch);
                szBuf[cch] = '\0';
                LOCALTRACE(_T("opVar: \"%s\"\n"), szBuf);
            }
#endif

            CVarObject* pObj = new CVarObject(pch, cch);
            Push(pObj);
        }
        break;

    case opLocal:
        {
            LOCALTRACE(_T("opLocal\n"));

            int nLocal = FetchInt();

            ASSERT(m_spFrame + nLocal < m_sp);
            CObject* pObject = m_stack[m_spFrame + nLocal];
            if (pObject == NULL)
            {
                pObject = new CLocalVariable;
                m_stack[m_spFrame + nLocal] = pObject;
            }

            pObject->AddRef();
            Push(pObject);
        }
        break;

    case opStr:
        {
            int cch;
            const TCHAR* pch = FetchString(cch);
            Push(pch, cch);
        }
        break;

    case opNum:
        {
            float n = FetchFloat();
            Push(n);
        }
        break;
    }

    return true;
}


bool CRunner::BinaryOperator(BYTE op)
{
    LOCALTRACE(_T("opOperator: %d\n"), op);

    CObject* pObjRight = Dereference(Pop());
    CObject* pObjLeft = Dereference(Pop());

    if (pObjLeft == NULL || pObjRight == NULL || pObjLeft->m_obj == objNode && pObjRight->m_obj == objNode)
    {
        float fValue;

        switch (op)
        {
        default:
            Error(_T("illegal null reference"));
            return false;

        case opEQ:
            fValue = (pObjLeft == pObjRight) ? 1.0f : 0.0f;
            Push(fValue);
            break;

        case opNE:
            fValue = (pObjLeft != pObjRight) ? 1.0f : 0.0f;
            Push(fValue);
            break;
        }

        if (pObjRight != NULL)
            pObjRight->Release();

        if (pObjLeft != NULL)
            pObjLeft->Release();

        return true;
    }

    if (op != opAdd && pObjLeft->m_obj == objString && pObjRight->m_obj == objString)
    {
        const TCHAR* szLeft = ((CStrObject*)pObjLeft)->GetSz();
        const TCHAR* szRight = ((CStrObject*)pObjRight)->GetSz();
        int nCompare = _tcscmp(szLeft, szRight);
        bool bValue;

        switch (op)
        {
        default:
            Error(_T("type mismatch"));
            return false;

        case opEQ:
            bValue = nCompare == 0;
            break;

        case opNE:
            bValue = nCompare != 0;
            break;

        case opLT:
            bValue = nCompare < 0;
            break;

        case opLE:
            bValue = nCompare <= 0;
            break;

        case opGT:
            bValue = nCompare > 0;
            break;

        case opGE:
            bValue = nCompare >= 0;
            break;
        }

        Push(bValue);
    }
    else if (pObjLeft->m_obj == objNumber && pObjRight->m_obj == objNumber)
    {
        float fLeft = ((CNumObject*)pObjLeft)->m_nValue;
        float fRight = ((CNumObject*)pObjRight)->m_nValue;
        float fValue;

#ifdef _DEBUG
        double intptr;
#endif

        switch (op)
        {
        case opEQ:
            fValue = (fLeft == fRight) ? 1.0f : 0.0f;
            break;

        case opNE:
            fValue = (fLeft != fRight) ? 1.0f : 0.0f;
            break;

        case opLT:
            fValue = (fLeft < fRight) ? 1.0f : 0.0f;
            break;

        case opLE:
            fValue = (fLeft <= fRight) ? 1.0f : 0.0f;
            break;

        case opGT:
            fValue = (fLeft > fRight) ? 1.0f : 0.0f;
            break;

        case opGE:
            fValue = (fLeft >= fRight) ? 1.0f : 0.0f;
            break;

        case opAdd:
            fValue = fLeft + fRight;
            break;

        case opSub:
            fValue = fLeft - fRight;
            break;

        case opMul:
            fValue = fLeft * fRight;
            break;

        case opDiv:
            fValue = fLeft / fRight;
            break;

        // REVIEW: need to be more clever about this later

        case opAnd:
#ifdef _DEBUG
            if (modf(fLeft, &intptr) != 0 || modf(fRight, &intptr) != 0)
            {
                Error(_T("type mismatch"));
                return false;
            }
#endif
            fValue = (float)((ULONG)fLeft & (ULONG)fRight);
            break;

        case opXor:
#ifdef _DEBUG
            if (modf(fLeft, &intptr) != 0 || modf(fRight, &intptr) != 0)
            {
                Error(_T("type mismatch"));
                return false;
            }
#endif
            fValue = (float)((ULONG)fLeft ^ (ULONG)fRight);
            break;

        case opOr:
#ifdef _DEBUG
            if (modf(fLeft, &intptr) != 0 || modf(fRight, &intptr) != 0)
            {
                Error(_T("type mismatch"));
                return false;
            }
#endif
            fValue = (float)((ULONG)fLeft | (ULONG)fRight);
            break;

        case opSHL:
            fValue = (float)((ULONG)fLeft << (ULONG)fRight);
            break;

        case opSHR:
            fValue = (float)((ULONG)fLeft >> (ULONG)fRight);
            break;
        }

        Push(fValue);
    }
    else if (op == opAdd)
    {
        // Add strings...
        CStrObject* pStrLeft = pObjLeft->ToStr();
        CStrObject* pStrRight = pObjRight->ToStr();

        CStrObject* pNewStr = pStrLeft->concat(pStrRight->GetSz());

        Push(pNewStr);

        pStrLeft->Release();
        pStrRight->Release();
    }
    else
    {
        // TODO: More should be legal!
        Error(_T("type mismatch error"));
        return false;
    }

    pObjLeft->Release();
    pObjRight->Release();

    return true;
}


BOOL CRunner::ExecuteBuiltIn(const TCHAR* pchName, int cchName, int nParam, CObject** rgParam, CObject*& pRetObj)
{
    pRetObj = NULL;

    if (cchName == 11 && _tcsncmp(pchName, _T("EnableInput"), cchName) == 0)
    {
        if (nParam == 1)
        {
            CObject* pObj = Dereference(rgParam[0]);
            CNumObject* pNum = pObj->ToNum();
            g_bInputEnable = (bool)(pNum->m_nValue != 0.0f);
        }
        else
        {
            Error(_T("Invalid parameter"));
        }

        return TRUE;
    }

    if (cchName == 4 && _tcsncmp(pchName, _T("eval"), cchName) == 0)
    {
        if (nParam == 1)
        {
            CObject* pObject = Dereference(rgParam[0]);
            CStrObject* pStr = pObject->ToStr();
            ExecuteScript(m_pSelf, pStr->GetSz());
            pStr->Release();
            pObject->Release();
        }
        else
        {
            Error(_T("Bad launch"));
        }

        return TRUE;
    }

    if (cchName == 6 && _tcsncmp(pchName, _T("launch"), cchName) == 0)
    {
        if (nParam == 1 || nParam == 2)
        {
            CObject* pObject1 = Dereference(rgParam[0]);
            CStrObject* pStr1 = pObject1->ToStr();
            const TCHAR* sz1 = pStr1->GetSz();

            TCHAR sz2 [MAX_PATH];
            if (nParam == 2)
            {
                CObject* pObject2 = Dereference(rgParam[1]);
                CStrObject* pStr2 = pObject2->ToStr();
                _tcscpy(sz2, pStr2->GetSz());
                pStr2->Release();
                pObject2->Release();
            }
            else
            {
                _tcscpy(sz2, sz1);
                TCHAR* pch = _tcsrchr(sz2, '/');
                if (pch != NULL)
                    *pch = '\\';
                pch = _tcsrchr(sz2, '\\');
                ASSERT(pch != NULL);

                *pch = 0;
            }

            TRACE(_T("Launch title: %s, %s\n"), sz1, sz2);

#if defined(_XBOX)
            FSCHAR ssz1 [MAX_PATH];
            FSCHAR ssz2 [MAX_PATH];
            CleanFilePath(ssz1, sz1);
            CleanFilePath(ssz2, sz2);

            XWriteTitleInfoAndReboot(ssz1, ssz2, LDT_NONE, 0, NULL);
#else
#endif
            pStr1->Release();
            pObject1->Release();
        }
        else
        {
            Error(_T("Bad launch"));
        }

        return TRUE;
    }

    if (cchName == 10 && _tcsncmp(pchName, _T("DebugBreak"), cchName) == 0)
    {
#ifdef _DEBUG
        _CrtDbgBreak();
#endif
        return TRUE;
    }

    if (cchName == 5 && _tcsncmp(pchName, _T("alert"), cchName) == 0)
    {
        if (nParam == 1)
        {
            CObject* pObject = Dereference(rgParam[0]);
            CStrObject* pStr = pObject->ToStr();
            Alert(_T("%s"), pStr->GetSz());
            pStr->Release();
            pObject->Release();
        }
        else
        {
            Error(_T("Bad alert"));
        }

        return TRUE;
    }

    if (cchName == 3 && _tcsncmp(pchName, _T("log"), cchName) == 0)
    {
        if (nParam == 1)
        {
            CObject* pObject = Dereference(rgParam[0]);
            CStrObject* pStr = pObject->ToStr();
            TRACE(_T("%s\n"), pStr->GetSz());
            pStr->Release();
            pObject->Release();
        }
        else
        {
            Error(_T("Bad log!"));
        }

        return TRUE;
    }

    return FALSE;
}

BYTE* CRunner::LookupFunction(const TCHAR* pchName, int cchName, CObject*& pOwner)
{
    pOwner = m_pSelf;

    {
        CFunction* pFunction = m_pSelf->FindMemberFunction(pchName, cchName);
        if (pFunction != NULL)
            return pFunction->m_rgop;
    }
/*
    // BLOCK: Try for an instance function...
    {
        CFunction* pFunction = (CFunction*)m_pSelf->GetMember(pchName, cchName);
        if (pFunction != NULL && pFunction->m_obj == objFunction)
            return pFunction->m_rgop;
    }

    // BLOCK: Try for a class member (all the way up through the base classes...
    {
        CFunction* pFunction = (CFunction*)FindMember(m_pSelf->GetNodeClass(), pchName, cchName);
        if (pFunction != NULL && pFunction->m_obj == objFunction)
            return pFunction->m_rgop;
    }
*/

    // BLOCK: Try the parent's of the object...
    {
        for (CObject* pObject = m_pSelf->m_pParent; pObject != NULL; pObject = pObject->m_pParent)
        {
            CObject* pObj = LookupMember(pObject, pchName, cchName);
            if (pObj != NULL)
            {
                pOwner = pObject;
                if (pObj->m_obj == objFunction)
                    return ((CFunction*)pObj)->m_rgop;

                ASSERT(FALSE);
                return (BYTE*)pObj;
            }
        }
    }

    pOwner = NULL;
    return NULL;
}

CObject* CRunner::LookupVariable(const TCHAR* pchName, int cchName)
{
    CObject* pObject = LookupMember(m_pSelf, pchName, cchName);
    if (pObject != NULL || m_pSelf == m_pThis)
        return pObject;

    return LookupMember(m_pThis, pchName, cchName);
}

#ifdef _DEBUG
void CRunner::DumpStack()
{
    TRACE(_T("\nStack Trace\n{\n"));

    TRACE(_T("ops: 0x%08x\n"), m_ops);
    TRACE(_T("nop: %d\n"), m_nop);
    TRACE(_T("spFrame: %d\n"), m_spFrame);
    TRACE(_T("sp: %d\n"), m_sp);
    TRACE(_T("self:\n")); m_pSelf->Dump();
    TRACE(_T("this:\n")); m_pThis->Dump();

    for (RUNCONTEXT* pContext = m_pNextContext; pContext != NULL; pContext = pContext->m_pNextContext)
    {
        TRACE(_T("{\nContext: 0x%08x\n"), pContext);

        TRACE(_T("ops: 0x%08x\n"), pContext->m_ops);
        TRACE(_T("nop: %d\n"), pContext->m_nop);
        TRACE(_T("spFrame: %d\n"), pContext->m_spFrame);
        TRACE(_T("sp: %d\n"), pContext->m_sp);
        TRACE(_T("self:\n")); pContext->m_pSelf->Dump();
        TRACE(_T("this:\n")); pContext->m_pThis->Dump();

        TRACE(_T("}\n"));
    }

    TRACE(_T("}\n"));
}
#endif


////////////////////////////////////////////////////////////////////////////




CProperty::CProperty(CObject* pNode, const PRD* pprd)
{
    m_pNode = pNode;
    m_pprd = pprd;

    pNode->AddRef();
}

CProperty::~CProperty()
{
    m_pNode->Release();
}

CObject* CProperty::Deref()
{
    void* pvValue = (BYTE*)m_pNode + (int)m_pprd->pbOffset;

    switch (m_pprd->nType)
    {
    case pt_string:
        {
            CStrObject* pStr =  new CStrObject(*(TCHAR**)pvValue);
            Release();
            return pStr;
        }

    case pt_number:
        {
            float n = *(float*)pvValue;
            Release();
            return new CNumObject(n);
        }

    case pt_integer:
        {
            int n = *(int*)pvValue;
            Release();
            return new CNumObject((float)n);
        }

    case pt_boolean:
        {
            bool n = *(bool*)pvValue;
            Release();
            return new CNumObject((float)n);
        }

    case pt_node:
        {
            CNode* pNode = *(CNode**)pvValue;
            if (pNode != NULL)
                pNode->AddRef();
            Release();
            return pNode;
        }

    case pt_vec3:
        {
            D3DXVECTOR3 v = *(D3DXVECTOR3*)pvValue;
            Release();
            return new CVec3Object(v);
        }

    case pt_nodearray:
    case pt_children:
        {
            CNodeArrayObject* pRet = new CNodeArrayObject((CNodeArray*)pvValue);
            Release();
            return pRet;
        }
    }

    g_pRunner->Error(_T("Unknown property type"));

    return NULL;
}

void CProperty::Assign(CObject* pObject)
{
    if (pObject == NULL && m_pprd->nType != pt_node)
    {
        g_pRunner->Error(_T("Illegal null assignement"));
        return;
    }

    switch (m_pprd->nType)
    {
    case pt_string:
        {
            CStrObject* pStrObject = pObject->Deref()->ToStr();

            const TCHAR* sz = pStrObject->GetSz();
            m_pNode->SetProperty(m_pprd, &sz, pStrObject->GetLength());
            pStrObject->Release();
        }
        break;

    case pt_number:
        {
            CNumObject* pNumObject = pObject->Deref()->ToNum();

            m_pNode->SetProperty(m_pprd, &pNumObject->m_nValue, sizeof (float));
            pNumObject->Release();
        }
        break;

    case pt_integer:
        {
            CNumObject* pNumObject = pObject->Deref()->ToNum();

            int n = (int)pNumObject->m_nValue;
            m_pNode->SetProperty(m_pprd, &n, sizeof (int));
            pNumObject->Release();
        }
        break;

    case pt_boolean:
        {
            CNumObject* pNumObject = pObject->Deref()->ToNum();

            bool b = pNumObject->m_nValue != 0.0f;
            m_pNode->SetProperty(m_pprd, &b, sizeof (bool));
            pNumObject->Release();
        }
        break;

    case pt_vec3:
        {
            CVec3Object* pVec3Object = (CVec3Object*)pObject->Deref();
            if (pVec3Object->m_obj != objVec3)
                TRACE(_T("\001Expected a Vector3 object!\n"));
            else
                m_pNode->SetProperty(m_pprd, &pVec3Object->m_x, sizeof (float) * 3);
//          pVec3Object->Release();
        }
        break;

    case pt_node:
        {
            CNode* pNode = NULL;
            if (pObject != NULL)
                pNode = (CNode*)pObject->Deref();
            m_pNode->SetProperty(m_pprd, &pNode, sizeof (CNode*));
        }
        break;
    }
}

CObject* CNode::Dot(CObject* pObj)
{
    return CObject::Dot(pObj);
}

CStrObject* CNode::ToStr()
{
    return new CStrObject(GetNodeClass()->m_szClassName);
}


////////////////////////////////////////////////////////////////////////////


CNumObject::CNumObject()
{
    m_obj = objNumber;
    m_nValue = 0.0f;
}

CNumObject::CNumObject(float nValue)
{
    m_obj = objNumber;
    m_nValue = nValue;
}

CNumObject::CNumObject(const TCHAR* szValue)
{
    m_obj = objNumber;
    m_nValue = (float)_tcstod(szValue, NULL);
}

CStrObject* CNumObject::ToStr()
{
    TCHAR szBuf [20];
    TCHAR* pch = szBuf + _stprintf(szBuf, _T("%f"), m_nValue) - 1;
    while (*pch == '0')
        pch -= 1;
    if (*pch == '.')
        pch -= 1;
    pch += 1;
    *pch = '\0';
    return new CStrObject(szBuf);
}


////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NODE("Vector3", CVec3Object, CObject)

START_NODE_PROPS(CVec3Object, CObject)
    NODE_PROP(pt_number, CVec3Object, x)
    NODE_PROP(pt_number, CVec3Object, y)
    NODE_PROP(pt_number, CVec3Object, z)
END_NODE_PROPS()

CVec3Object::CVec3Object()
{
    m_obj = objVec3;
    m_x = 0.0f;
    m_y = 0.0f;
    m_z = 0.0f;
}

CVec3Object::CVec3Object(const D3DXVECTOR3& v)
{
    m_obj = objVec3;
    m_x = v.x;
    m_y = v.y;
    m_z = v.z;
}

CStrObject* CVec3Object::ToStr()
{
    TCHAR szBuf [60];
    _stprintf(szBuf, _T("%f %f %f"), m_x, m_y, m_z);
    return new CStrObject(szBuf);
}


////////////////////////////////////////////////////////////////////////////


CNodeArrayObject::CNodeArrayObject(CNodeArray* pNodeArray)
{
    m_obj = objNodeArray;
    m_pNodeArray = pNodeArray;
}


CObject* CNodeArrayObject::Call(int nFunction, CObject** rgparam, int nParam)
{
    switch (nFunction)
    {
    case 0:
        {
            int n = m_pNodeArray->GetLength();
            return new CNumObject((float)n);
        }
        break;

    case 1:
        {
            CObject* pObj = rgparam[0]->Deref();
            pObj->AddRef();
            m_pNodeArray->AddNode((CNode*)pObj);
        }
        break;

    case 2:
        {
            CObject* pObj = rgparam[0]->Deref();
            m_pNodeArray->RemoveNode((CNode*)pObj);
            pObj->Release(); // REVIEW: Only if it was in there!
        }
        break;
    }

    return NULL;
}

CObject* CNodeArrayObject::Dot(CObject* pObj)
{
    if (pObj->m_obj == objVariable)
    {
        CVarObject* pVar = (CVarObject*)pObj;

        if (pVar->m_nLength == 6 && _tcsncmp(pVar->m_sz, _T("length"), pVar->m_nLength) == 0)
        {
            CFunctionObject* pFun = new CFunctionObject;
            pFun->m_nFunction = 0;
            pFun->m_pObject = this;
            AddRef();

            return pFun;
        }
        else if (pVar->m_nLength == 3 && _tcsncmp(pVar->m_sz, _T("add"), pVar->m_nLength) == 0)
        {
            CFunctionObject* pFun = new CFunctionObject;
            pFun->m_nFunction = 1;
            pFun->m_pObject = this;
            AddRef();

            return pFun;
        }
        else if (pVar->m_nLength == 6 && _tcsncmp(pVar->m_sz, _T("remove"), pVar->m_nLength) == 0)
        {
            CFunctionObject* pFun = new CFunctionObject;
            pFun->m_nFunction = 2;
            pFun->m_pObject = this;
            AddRef();

            return pFun;
        }
    }
    else
    {
        int n = (int)((CNumObject*)pObj)->m_nValue;
        if (n < 0 || n >= m_pNodeArray->GetLength())
        {
            g_pRunner->Error(_T("Array reference out of bounds"));
            return NULL;
        }

        CNode* pNode = m_pNodeArray->GetNode(n);
        pNode->AddRef();
        return pNode;
    }

    return CObject::Dot(pObj);
}








// A CVarObject is actually just a name without a current scope...
// When one is Deref()'d or Assign()'d, a scope is known and it is resolved within that scope.
//
CVarObject::CVarObject(const TCHAR* pch, int cch)
{
    m_obj = objVariable;
    m_nLength = cch;
    m_nAlloc = cch + 1;
    m_sz = new TCHAR [m_nAlloc];
    CopyChars(m_sz, pch, cch);
    m_sz[cch] = '\0';
}

CVarObject::~CVarObject()
{
    delete [] m_sz;
}



CObject* CVarObject::Deref()
{
    CObject* pObject;

    // REVIEW: This is pretty bogus! We do have a table of classes to look the name up in...
    if (_tcscmp(m_sz, _T("Math")) == 0)
    {
        extern CMathClass g_Math;
        pObject = &g_Math;
    }
    else if (_tcscmp(m_sz, _T("camera")) == 0)
    {
        pObject = &theCamera;
    }
#ifndef _XBOX
    else if (_tcscmp(m_sz, _T("navigator")) == 0)
    {
        pObject = theApp.m_pNavigator;
    }
#endif
    else
    {
        pObject = g_pRunner->LookupVariable(m_sz, _tcslen(m_sz));
        if (pObject == NULL)
        {
            g_pRunner->Error(_T("Unknown object: %s"), m_sz);
            return NULL;
        }

        if (pObject->m_obj == objMember)
        {
            LOCALTRACE(_T("member\n"));
        }
    }

    pObject->AddRef();
    pObject = pObject->Deref();
    Release();

    return pObject;
}

void CVarObject::Assign(CObject* pNewObject)
{
    CObject* pObject;

    pObject = g_pRunner->LookupVariable(m_sz, _tcslen(m_sz));
    if (pObject == NULL)
    {
        g_pRunner->Error(_T("Unknown variable: %s"), m_sz);
        return;
    }

    pObject->Assign(pNewObject);
}





CMemberVarObject::CMemberVarObject(CInstance* pObject, int nMember)
{
    ASSERT(pObject != NULL);
    ASSERT(nMember >= 0 && nMember < pObject->m_vars.GetLength());

    m_obj = objMemberVar;

    m_pObject = pObject;
    m_nMember = nMember;

    m_pObject->AddRef();
}

CMemberVarObject::~CMemberVarObject()
{
    ASSERT(m_pObject != NULL);

    m_pObject->Release();
}


CObject* CMemberVarObject::Deref()
{
    ASSERT(m_nMember >= 0 && m_nMember < m_pObject->m_vars.GetLength());

    CObject* pObj = m_pObject->m_vars.GetNode(m_nMember);
    if (pObj != NULL)
        pObj->AddRef();

    Release();

    return pObj;
}

void CMemberVarObject::Assign(CObject* pNewObject)
{
    ASSERT(m_nMember >= 0 && m_nMember < m_pObject->m_vars.GetLength());

    CObject* pOldObj = m_pObject->m_vars.GetNode(m_nMember);
    if (pOldObj != NULL)
        pOldObj->Release();

    m_pObject->m_vars.SetNode(m_nMember, (CNode*)pNewObject);
    if (pNewObject != NULL)
        pNewObject->AddRef();
}


CMemberFunctionObject::CMemberFunctionObject(CInstance* pInstance, CFunction* pFunction)
{
    m_obj = objMemberFunction;
    m_pObject = pInstance;
    m_pFunction = pFunction;
}

CMemberFunctionObject::~CMemberFunctionObject()
{
}

CObject* CMemberFunctionObject::Deref()
{
    CRunner runner(m_pObject);
    runner.SetFunc(m_pFunction);
    CObject* pObject = runner.Run();
    Release();
    return pObject;
}




void CObject::Assign(CObject* pObject)
{
    g_pRunner->Error(_T("Cannot assign to objects"));
}


CFunctionObject::CFunctionObject()
{
    m_obj = objFunctionRef;
    m_pObject = NULL;
}

CFunctionObject::~CFunctionObject()
{
    if (m_pObject != NULL)
        m_pObject->Release();
}

CObject* CFunctionObject::Call(CObject** rgparam, int nParam)
{
    return m_pObject->Call(m_nFunction, rgparam, nParam);
}



////////////////////////////////////////////////////////////////////////////


CObject* FindMember(CNodeClass* pClass, const TCHAR* pchName, int cchName)
{
    for ( ; pClass != NULL; pClass = pClass->m_pBaseClass)
    {
        CObject* pObj = pClass->GetMember(pchName, cchName);
        if (pObj != NULL)
            return pObj;
    }

    return NULL;
}

CObject* LookupMember(CObject* pThis, const TCHAR* pchName, int cchName)
{
#ifdef _DEBUG0
    TCHAR szBuf [256];
    int cch = cchName;
    if (cch > 255)
        cch = 255;
    CopyChars(szBuf, pchName, cch);
    szBuf[cch] = 0;
    TRACE(_T("LookupMember: 0x%08x '%s'\n"), pThis, szBuf);
#endif

    ASSERT(pThis != NULL);

    g_pThis = pThis;

    if (pThis->m_obj == objNode)
    {
        CNode* pNode = (CNode*)pThis;
        const PRD* pprd = pNode->FindProp(pchName, cchName);
        if (pprd != NULL)
        {
            CObject* pObj = new CProperty(pNode, pprd);
            pObj->m_nRefCount = 0;
            return pObj;
        }
    }

    // BLOCK: Try for an instance member...
    {
        CObject* pObject = pThis->GetMember(pchName, cchName);
        if (pObject != NULL)
            return pObject;
    }

    // BLOCK: Try for a class member (all the way up through the base classes...
    {
        CObject* pObj = FindMember(pThis->GetNodeClass(), pchName, cchName);
        if (pObj != NULL)
            return pObj;
    }

    // BLOCK: Try the parent's of the object...
    {
        for (CObject* pObject = pThis->m_pParent; pObject != NULL; pObject = pObject->m_pParent)
        {
            CObject* pObj = LookupMember(pObject, pchName, cchName);
            if (pObj != NULL)
                return pObj;
        }
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////


CFunction::CFunction()
{
    m_obj = objFunction;
    m_cbop = 0;

#ifdef _DEBUG
    m_szName = NULL;
#endif
}

CFunction::~CFunction()
{
#ifdef _DEBUG
    delete [] m_szName;
#endif
}

////////////////////////////////////////////////////////////////////////////


CMember::CMember()
{
    m_obj = objMember;
    m_nMember = -1;
}

CMember::~CMember()
{
}

CObject* CMember::Deref()
{
    ASSERT(g_pThis != NULL);
    ASSERT(g_pThis->m_obj == objInstance);

    CInstance* pThis = (CInstance*)g_pThis;

    ASSERT(m_nMember >= 0 && m_nMember < pThis->m_vars.GetLength());

    CObject* pObj = pThis->m_vars.GetNode(m_nMember);
    if (pObj != NULL)
        pObj->AddRef();
    Release();

    return pObj;
}

void CMember::Assign(CObject* pNewValue)
{
    ASSERT(g_pThis != NULL);
    ASSERT(g_pThis->m_obj == objInstance);

    CInstance* pThis = (CInstance*)g_pThis;

    ASSERT(m_nMember >= 0 && m_nMember < pThis->m_vars.GetLength());

    CObject* pObj = pThis->m_vars.GetNode(m_nMember);
    if (pObj != NULL)
        pObj->Release();

    pThis->m_vars.SetNode(m_nMember, (CNode*)pNewValue);
    if (pNewValue != NULL)
        pNewValue->AddRef();
}
