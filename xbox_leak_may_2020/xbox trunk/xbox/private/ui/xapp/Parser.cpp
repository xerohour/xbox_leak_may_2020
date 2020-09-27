#include "std.h"
#include "xapp.h"
#include "Parser.h"
#include "Node.h"
#include "Runner.h"
#include "DefUse.h"

//#define LOCALTRACE TRACE
#define LOCALTRACE 1 ? (void)0 : ::Trace

CNameSpace* g_classes; // REVIEW:: Kill this hack!

static const TCHAR szParseError [] = _T("");

////////////////////////////////////////////////////////////////////////////

CClassCompiler::CClassCompiler(CClass* pClass)
{
    ASSERT(pClass != NULL);
    m_pClass = pClass;
}

const TCHAR* CClassCompiler::Compile(const TCHAR* pch)
{
    for (;;)
    {
        pch = SkipWhite(pch);

        if (*pch == '\0' || *pch == '}')
            break;

        pch = ParseNode(pch);
    }

    Write(0);

    return pch;
}

const TCHAR* CClassCompiler::ParseNode(const TCHAR* pch)
{
    const TCHAR* pchToken;
    int cchToken;
    pch = Token(pch, pchToken, cchToken);

    if (cchToken == 3 && _tcsncmp(pchToken, _T("DEF"), cchToken) == 0)
    {
        pch = Token(pch, pchToken, cchToken, true); // NOTE: bAllowPaths so '-' works in node names...

        CMember* pMember = new CMember;
        pMember->m_nMember = m_pClass->m_nVarCount++;
        m_pClass->AddMember(pchToken, cchToken, pMember);

        Write(opDefNode);
        WriteInteger(pMember->m_nMember);

        pch = ParseNode(pch);
    }
    else if (cchToken == 3 && _tcsncmp(pchToken, _T("USE"), cchToken) == 0)
    {
        pch = Token(pch, pchToken, cchToken, true); // NOTE: bAllowPaths so '-' works in node names...

        CMember* pMember = (CMember*)m_pClass->GetMember(pchToken, cchToken);
        if (pMember == NULL || pMember->m_obj != objMember)
        {
            TCHAR szBuf [256];
            if (cchToken > 255)
                cchToken = 255;
            CopyChars(szBuf, pchToken, cchToken);
            szBuf[cchToken] = 0;
            SyntaxError(_T("Undefined USE '%s'"), szBuf);
            return szParseError;
        }

        Write(opUseNode);
        WriteInteger(pMember->m_nMember);
    }
    else if (cchToken == 8 && _tcsncmp(pchToken, _T("behavior"), cchToken) == 0)
    {
        CFunction* pFunction;
        pch = ParseFunction(pch, pFunction, true);
        if (pFunction != NULL)
            m_pClass->SetMember(pchToken, cchToken, pFunction);
    }
    else if (cchToken == 8 && _tcsncmp(pchToken, _T("function"), cchToken) == 0)
    {
#ifdef _DEBUG0
        int nStartTime = GetTickCount();
#endif
        pch = Token(pch, pchToken, cchToken);

        if (cchToken == 8 && _tcsncmp(_T("behavior"), pchToken, cchToken) == 0)
        {
            SyntaxError(_T("invalid function name"));
            return szParseError;
        }

        CFunction* pFunction;
        pch = ParseFunction(pch, pFunction);
        if (pFunction != NULL)
        {
#ifdef _DEBUG
            pFunction->m_szName = new TCHAR [cchToken + 1];
            CopyChars(pFunction->m_szName, pchToken, cchToken);
            pFunction->m_szName[cchToken] = 0;
            LOCALTRACE(_T("member function %s\n"), pFunction->m_szName);
#endif
            m_pClass->SetMember(pchToken, cchToken, pFunction);
        }
//      TRACE(_T("\t0x%08x\n"), pFunction);
#ifdef _DEBUG0
        TRACE(_T("%s took %d mS to compile\n"), pFunction->m_szName, GetTickCount() - nStartTime);
#endif
    }
    else if (cchToken == 3 && _tcsncmp(pchToken, _T("var"), cchToken) == 0)
    {
        pch = ParseMemberVar(pch);
    }
    else if (cchToken == 5 && _tcsncmp(pchToken, _T("class"), cchToken) == 0)
    {
        pch = ParseClass(pch);
    }
    else if (cchToken == 6 && _tcsncmp(pchToken, _T("import"), cchToken) == 0)
    {
        pch = Token(pch, pchToken, cchToken);
        pch = SkipWhite(pch);
        if (*pch != ',')
        {
            SyntaxError(_T("Expected ','"));
            return szParseError;
        }

        pch += 1;

        const TCHAR* pchURL;
        int cchURL;
        pch = Token(pch, pchURL, cchURL);

        TCHAR szURL [MAX_PATH];
        CopyChars(szURL, pchURL, cchURL);
        szURL[cchURL] = '\0';

        CClass* pClass = new CClass;

        TRACE(_T("Importing "), szURL);

        if (!pClass->Load(szURL))
        {
            delete pClass;
            return szParseError;
        }

        g_classes->Define(pchToken, cchToken, (CNode*)pClass);
    }
    else
    {
        CNodeClass* pNodeClass = LookupClass(pchToken, cchToken);
        if (pNodeClass == NULL)
            pNodeClass = CNodeClass::FindByName(pchToken, cchToken);

        if (pNodeClass == NULL)
        {
            TCHAR chSav = pchToken[cchToken];
            ((TCHAR*)pchToken)[cchToken] = '\0';
            SyntaxError(_T("Unknown class: %s"), pchToken);
            ((TCHAR*)pchToken)[cchToken] = chSav;
            return szParseError; // cause parsing to end
        }

        Write(opNewNode);
        WriteString(pchToken, cchToken);


        pch = SkipWhite(pch);

        if (*pch == '{')
        {
            pch += 1;

            pch = ParseProps(pch, pNodeClass/*pNode*/);

            if (*pch != '}')
            {
                SyntaxError(_T("Exected a '}'"));
                return szParseError; // cause parsing to end
            }

            pch += 1;
        }

        Write(opEndNode);
    }

    pch = SkipWhite(pch);
    if (*pch == ',')
        pch += 1;

    return pch;
}

const TCHAR* CClassCompiler::ParseChildren(const TCHAR* pch)
{
    pch = SkipWhite(pch);

    if (*pch == '[')
    {
        pch += 1;
        pch = SkipWhite(pch);

        for (;;)
        {
            pch = SkipWhite(pch);

            if (*pch == '\0' || *pch == '}' || *pch == ']')
                break;

            pch = ParseNode(pch);
        }

        if (*pch == ']')
            pch += 1;
        else
            SyntaxError(_T("Exected a ']'"));
    }
    else
    {
        pch = ParseNode(pch);
    }

    return pch;
}

const TCHAR* CClassCompiler::ParseProps(const TCHAR* pch, CNodeClass* pNodeClass)
{
    for (;;)
    {
        pch = SkipWhite(pch);
        if (*pch == '\0' || *pch == '}')
            break;

        const TCHAR* pchToken;
        int cchToken;
        pch = Token(pch, pchToken, cchToken);

        if (cchToken == 8 && _tcsncmp(_T("behavior"), pchToken, cchToken) == 0)
        {
            CFunction* pFunction;
            pch = ParseFunction(pch, pFunction, true);
            if (pFunction != NULL)
            {
                int nFunction = m_pClass->m_instanceFunctions.GetLength();
                m_pClass->m_instanceFunctions.AddNode((CNode*)pFunction);

                Write(opFunction);
                WriteString(pchToken, cchToken);
                WriteInteger(nFunction);
            }
            continue;
        }

        if (cchToken == 8 && _tcsncmp(_T("function"), pchToken, cchToken) == 0)
        {
            // We have an instance function here...
            pch = Token(pch, pchToken, cchToken);

            if (cchToken == 8 && _tcsncmp(_T("behavior"), pchToken, cchToken) == 0)
            {
                SyntaxError(_T("invalid function name"));
                return szParseError;
            }

            CFunction* pFunction;
            pch = ParseFunction(pch, pFunction);
            if (pFunction != NULL)
            {
#ifdef _DEBUG
                pFunction->m_szName = new TCHAR [cchToken + 1];
                CopyChars(pFunction->m_szName, pchToken, cchToken);
                pFunction->m_szName[cchToken] = 0;
                LOCALTRACE(_T("instance function %s\n"), pFunction->m_szName);
#endif
                int nFunction = m_pClass->m_instanceFunctions.GetLength();
                m_pClass->m_instanceFunctions.AddNode((CNode*)pFunction);

                Write(opFunction);
                WriteString(pchToken, cchToken);
                WriteInteger(nFunction);

//              TRACE(_T("\t0x%08x\n"), pFunction);
            }
            continue;
        }

#ifdef _DEBUG_0
        {
            TCHAR chSav = pchToken[cchToken];
            ((TCHAR*)pchToken)[cchToken] = '\0';
            TRACE(_T("property %s\n"), pchToken);
            ((TCHAR*)pchToken)[cchToken] = chSav;
        }
#endif

        const PRD* pprd = pNodeClass->FindProp(pchToken, cchToken);

        if (pprd == NULL)
        {
            TCHAR chSav = pchToken[cchToken];
            ((TCHAR*)pchToken)[cchToken] = '\0';
            SyntaxError(_T("Unknown property: %s"), pchToken);
            ((TCHAR*)pchToken)[cchToken] = chSav;
            return szParseError;
        }
        else
        {
            if (pprd->nType == pt_children || pprd->nType == pt_nodearray)
            {
                Write(opInitArray);
                WriteInteger((int)pprd->pbOffset);

                pch = ParseChildren(pch);

                Write(opEndArray);
            }
            else if (pprd->nType == pt_node)
            {
                Write(opNewNodeProp);
                Write(pprd, sizeof (PRD));

                pch = ParseNode(pch);
            }
            else
            {
                union { bool b; int i; float n; float v [4]; TCHAR* s; /*CNode* p;*/} value;
                BYTE* pbArray = NULL;
                int cbValue = 0;

                switch (pprd->nType)
                {
                case pt_boolean:
                    pch = ParseBoolean(pch, value.b);
                    cbValue = sizeof (bool);
                    break;

                case pt_integer:
                    pch = ParseInteger(pch, value.i);
                    cbValue = sizeof (int);
                    break;

                case pt_number:
                    pch = ParseNumber(pch, value.n);
                    cbValue = sizeof (float);
                    break;

                case pt_string:
                    pch = ParseString(pch, value.s);
                    cbValue = _tcslen(value.s);
                    break;

                case pt_vec3:
                case pt_color:
                    pch = ParseVec3(pch, value.v);
                    cbValue = sizeof (D3DXVECTOR3);
                    break;

                case pt_vec4:
                case pt_quaternion:
                    pch = ParseVec4(pch, value.v);
                    cbValue = sizeof (D3DXVECTOR4);
                    break;

                case pt_intarray:
                    pch = ParseIntArray(pch, pbArray, cbValue);
                    break;

                case pt_numarray:
                    pch = ParseVecArray(pch, pbArray, cbValue, 1);
                    break;

                case pt_vec2array:
                    pch = ParseVecArray(pch, pbArray, cbValue, 2);
                    break;

                case pt_vec3array:
                    pch = ParseVecArray(pch, pbArray, cbValue, 3);
                    break;

                case pt_vec4array:
                    pch = ParseVecArray(pch, pbArray, cbValue, 4);
                    break;
                }

                Write(opInitProp);
                Write(pprd, sizeof (PRD));

                if (pprd->nType == pt_string)
                {
                    WriteString(value.s, cbValue);
                    delete [] value.s;
                }
                else
                {
                    WriteInteger(cbValue);
                    if (pbArray != NULL)
                    {
                        Write(pbArray, cbValue);
                        delete [] pbArray;
                    }
                    else
                    {
                        Write(&value, cbValue);
                    }
                }
            }
        }
    }

    return pch;
}

// Add member variables to the current class...
const TCHAR* CClassCompiler::ParseMemberVar(const TCHAR* pch)
{
    for (;;)
    {
        const TCHAR* pchToken;
        int cchToken;
        pch = Token(pch, pchToken, cchToken);

#ifdef _DEBUG0
        {
            TCHAR chSav = pchToken[cchToken];
            ((TCHAR*)pchToken)[cchToken] = '\0';
            LOCALTRACE(_T("new member variable \"%s\"\n"), pchToken);
            ((TCHAR*)pchToken)[cchToken] = chSav;
        }
#endif

        CMember* pMember = new CMember;
        pMember->m_nMember = m_pClass->m_nVarCount++;
        m_pClass->AddMember(pchToken, cchToken, pMember);

        pch = SkipWhite(pch);

        if (*pch == '=')
        {
            pch += 1;

            // TODO: Initializers -- add init code to class constructor!
            {
                SyntaxError(_T("Sorry, class member initializers have not been implemented yet!"));
                return szParseError;
            }

            pch = SkipWhite(pch);
        }

        if (*pch != ',')
            break;

        pch += 1;
    }

    if (*pch != ';')
        SyntaxError(_T("Expected a ';'"));
    else
        pch += 1;

    return pch;
}


const TCHAR* CClassCompiler::ParseFunction(const TCHAR* pch, CFunction*& pFunction, bool bBehavior)
{
    CFunctionCompiler compiler;
    compiler.m_bBehavior = bBehavior;

    pFunction = NULL;

    pch = SkipWhite(pch);

    if (!bBehavior)
    {
        // ParseParameters

        if (*pch != '(')
        {
            LOCALTRACE(_T("bad function definition: missing (\n"));
            return pch;
        }

        pch += 1;

        for (;;)
        {
            pch = SkipWhite(pch);
            if (*pch == '\0' || *pch == ')')
                break;

            const TCHAR* pchToken;
            int cchToken;
            pch = Token(pch, pchToken, cchToken);

#ifdef _DEBUG_0
            {
                TCHAR chSav = pchToken[cchToken];
                ((TCHAR*)pchToken)[cchToken] = '\0';
                LOCALTRACE(_T("parameter %d \"%s\"\n"), compiler.m_nFrameSize, pchToken);
                ((TCHAR*)pchToken)[cchToken] = chSav;
            }
#endif

            compiler.m_rgstLocal[compiler.m_nFrameSize].pchName = pchToken;
            compiler.m_rgstLocal[compiler.m_nFrameSize].cchName = (short)cchToken;
            compiler.m_nFrameSize += 1;

            pch = SkipWhite(pch);
            if (*pch != ',')
                break;

            pch += 1;
        }

        if (*pch != ')')
        {
            SyntaxError(_T("expected ')'\n"));
            return szParseError;
        }

        pch += 1;

    }

    // ParseStatementBlock
    {
        pch = SkipWhite(pch);

        if (*pch != '{')
        {
            LOCALTRACE(_T("bad function definition: missing {\n"));
            return pch;
        }

        pch = compiler.ParseBlock(pch);
    }

    compiler.Write(opNull);
    LOCALTRACE(_T("%d: opRet\n"), compiler.GetAddress());
    compiler.Write(opRet);

    pFunction = compiler.CreateFunction();

    return pch;
}

const TCHAR* CClassCompiler::ParseClass(const TCHAR* pch)
{
    const TCHAR* pchToken;
    int cchToken;
    pch = Token(pch, pchToken, cchToken);

#ifdef _DEBUG
    {
        TCHAR chSav = pchToken[cchToken];
        ((TCHAR*)pchToken)[cchToken] = '\0';
        TRACE(_T("class %s\n"), pchToken);
        ((TCHAR*)pchToken)[cchToken] = chSav;
    }
#endif

    // TODO: base class? interfaces?

    // Parse Members
    pch = SkipWhite(pch);

    if (*pch != '{')
    {
        SyntaxError(_T("Expected '{'\n"));
        return szParseError;
    }

    pch += 1;

    CClass* pClass = new CClass;
    pch = pClass->ParseClassBody(pch);

    if (*pch != '}')
    {
        SyntaxError(_T("Expected '}'"));
        return szParseError;
    }

    pch += 1;

    // REVIEW: Is the class namespace flat or hierarchical? This assumes flat...
    g_classes->Define(pchToken, cchToken, (CNode*)pClass);

    return pch;
}

void Class_Init()
{
    g_classes = new CNameSpace;
}

void Class_Exit()
{
    delete g_classes;
}
