//////////////////////////////////////////////////////////////////////////////
//
//  maketrac.cpp
//
//  Creator: Galen C. Hunt (galenh), Rob Stets (stets@cs.rochester.edu)
//
//  Copyright 1996 - 1999 Microsoft Corporation.  All rights reserved.
//
//  Revision History
//   11-1999: Lee Hart, change to log only counts of API calls, not each & every API call.

#include <windows.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream.h>

#include "ast.h"

#ifndef INC_AstCL_H
#include "AstCL.h"
#endif

#ifndef INC_TOOLKIT_H
#include "toolkit.h"
#endif

//////////////////////////////////////////////////////////////////////

#ifndef _DEFINED_ASMBREAK_
#define _DEFINED_ASMBREAK_
#define ASMBREAK()  __asm { int 3 }
//#define ASMBREAK()    DebugBreak()
#endif  // _DEFINED_ASMBREAK_


#define ASSERT_ALWAYS(x)   \
    do {                                                        \
    if (!(x)) {                                                 \
            AssertMessage(#x, __FILE__, __LINE__);              \
            ASMBREAK();                                         \
    }                                                           \
    } while (0)

#ifndef NDEBUG
#define ASSERT(x)           ASSERT_ALWAYS(x)
#else
#define ASSERT(x)
#endif

#define UNUSED(c)    (c) = (c)

//////////////////////////////////////////////////////////////////////////////
//
void AssertMessage(CONST PCHAR pszMsg, CONST PCHAR pszFile, ULONG nLine)
{
    printf("ASSERT(%s) failed in %s, line %d.\n", pszMsg, pszFile, nLine);
}

////////////////////////////////////////////////////////// Assertion Handling.
//

//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//
static char * CallingConventionToString(CallingConvention cc)
{
    switch (cc) {
    case ccC:           return "__cdecl";
    case ccPascal:      return "__pascal";
    case ccFastCall:    return "__fastcall";
    case ccStdCall:     return "__stdcall";
    case ccSysCall:     return "__syscall";
    default:    ASSERT(!"Unsupported CallingConvention."); return NULL;
    }
}

static char * PrimitiveKindToString(PrimitiveKind pk)
{
    switch (pk) {
    case ptChar:        return "char";
    case ptSChar:       return "signed char";
    case ptUChar:       return "unsigned char";
    case ptSShort:      return "short";
    case ptUShort:      return "unsigned short";
    case ptSInt:        return "int";
    case ptUInt:        return "unsigned int";
    case ptSLong:       return "long";
    case ptULong:       return "unsigned long";
    case ptSInt64:      return "__int64";
    case ptUInt64:      return "unsigned __int64";
    case ptFloat:       return "float";
    case ptDouble:      return "double";
    case ptLDouble:     return "long double";
    case ptVoid:        return "void";
    case ptWChar:       return "WCHAR";
    default:    ASSERT(!"Unsupported PrimitiveKind."); return NULL;
    }
}

static PCHAR StrAdv(PCHAR pszOut, CONST CHAR *pszIn)
{
    while (*pszOut) {
        pszOut++;
    }
    if (__iscsym(pszOut[-1]) && __iscsym(pszIn[0])) {
        *pszOut++ = ' ';
    }
    while (*pszIn) {
        *pszOut++ = *pszIn++;
    }
    *pszOut = '\0';
    return pszOut;
}

void DumpTypeToString(char *pszOut, Type *ptArg, TypeIndex tiStop, char *pszStop, BOOL fConst)
{
    if (ptArg == NULL)
        return;

    if (ptArg->GetIndex() == tiStop) {
        pszOut = StrAdv(pszOut, pszStop);
        return;
    }

    switch (ptArg->GetTypeKind()) {

    case tkPrimitive:
        {
            TypePrimitive *pt = ptArg->asPrimitive();

            pszOut = StrAdv(pszOut, PrimitiveKindToString(pt->GetPrimitiveKind()));
        } break;

    case tkPointer:
        {
            TypePointer *pt =  ptArg->asPointer();
            pt = static_cast<TypePointer *>(ptArg);

            if (pt->FVolatile()) {
                pszOut = StrAdv(pszOut, "volatile");
            }
            if (pt->FConst() && fConst) {
                pszOut = StrAdv(pszOut, "const");
            }

            DumpTypeToString(pszOut, pt->GetTargetType(), tiStop, pszStop, fConst);
            if (pt->GetPointerKind() == pkReference) {
                pszOut = StrAdv(pszOut, "&");
            }
            else {
                pszOut = StrAdv(pszOut, "*");
            }
        } break;

    case tkArray:
        {
            TypeArray *pt = ptArg->asArray();

            DumpTypeToString(pszOut, pt->GetElemType(), tiStop, pszStop, fConst);
            pszOut = StrAdv(pszOut, "[");
            if (pt->GetLength()) {
                sprintf(pszOut, "%d", pt->GetLength());
            }
            pszOut = StrAdv(pszOut, "]");
        } break;

    case tkStruct:
        {
            TypeStruct *pt = ptArg->asStruct();
            pszOut = StrAdv(pszOut, "struct");
            pszOut = StrAdv(pszOut, pt->GetParseName());
        } break;

    case tkClass:
        {
            TypeClass *pt = ptArg->asClass();

            pszOut = StrAdv(pszOut, "class");
            pszOut = StrAdv(pszOut, pt->GetParseName());
        } break;

    case tkUnion:
        {
            TypeUnion *pt = ptArg->asUnion();

            pszOut = StrAdv(pszOut, "union");
            pszOut = StrAdv(pszOut, pt->GetParseName());
        } break;

    case tkEnum:
        {
            TypeEnum *pt = ptArg->asEnum();

            pszOut = StrAdv(pszOut, "enum");
            pszOut = StrAdv(pszOut, pt->GetParseName());
        } break;

    case tkModifier:
        {
            TypeModifier *pt = static_cast <TypeModifier *>(ptArg);

            if (pt->FVolatile()) {
                pszOut = StrAdv(pszOut, "volatile");
            }
            if (pt->FConst() && fConst) {
                pszOut = StrAdv(pszOut, "const");
            }
            DumpTypeToString(pszOut, pt->GetBaseType(), tiStop, pszStop, fConst);
        } break;

    case tkTypedef:
        {
            TypeTypedef *pt = ptArg->asTypedef();

            pszOut = StrAdv(pszOut, pt->GetSymbol()->GetName());
        } break;

    case tkProcedure:
        {
            TypeProcedure *pt = static_cast<TypeProcedure *>(ptArg);

            DumpTypeToString(pszOut, pt->GetReturnType(), -1, NULL, fConst);
            pszOut = StrAdv(pszOut, "(");
            pszOut = StrAdv(pszOut,
                            CallingConventionToString(pt->GetCallingConvention()));
            pszOut = StrAdv(pszOut, "^)(");

            TypeArgumentListIterator Arguments = pt->GetArgumentIterator();
            int nParm;
            Type *ptArgument = NULL;
            for (nParm = 0, ptArgument = Arguments.NextType();
                 ptArgument != NULL;
                 ptArgument = Arguments.NextType(), nParm++) {

                if (nParm) {
                    pszOut = StrAdv(pszOut, ", ");
                }

                DumpTypeToString(pszOut, ptArgument, -1, NULL, fConst);
            }

            if (pt->FVarArgs()) {
                if (nParm) {
                    pszOut = StrAdv(pszOut, ", ");
                }
                pszOut = StrAdv(pszOut, "...");
                nParm++;
            }
            if (nParm == 0) {
                pszOut = StrAdv(pszOut, "void");
            }
            pszOut = StrAdv(pszOut, ")");
        } break;


    default:
        ASSERT(!"Unsupported TypeKind.");
        break;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
class CFunction
{
protected:
    struct CArgument
    {
    public:
        CArgument();
        ~CArgument();

        HRESULT         Initialize(Symbol *psSymbol, Type *ptType, PCHAR pszSpec);

    public:
        PCHAR           m_pszName;
        PCHAR           m_pszSpec;
        PCHAR           m_pszType;

        Type *          m_ptType;
        Symbol *        m_psSymbol;

        BOOL            m_fIsString;
        BOOL            m_fIsUnicode;
        BOOL            m_fIsInput;
        BOOL            m_fIsOutput;

    };

public:
    CFunction();
    ~CFunction();

    HRESULT         Initialize(SymbolCommonFunctionStuff *psSymbol);

    PCHAR           GetFunctionName()           { return m_pszName; }
    PCHAR           GetFunctionSpec()           { return m_pszSpec; }
    PCHAR           GetFunctionCallConv();

    PCHAR           GetReturnType()             { return m_pRetVal->m_pszType; }

    CArgument       *GetArgument(INT nArg)      { return m_pArgs[nArg]; }

    PCHAR           GetArgumentName(INT nArg)   { return m_pArgs[nArg]
                                                      ? m_pArgs[nArg]->m_pszName
                                                      : "..."; }
    PCHAR           GetArgumentSpec(INT nArg)   { return m_pArgs[nArg]
                                                      ? m_pArgs[nArg]->m_pszSpec
                                                      : NULL; }
    PCHAR           GetArgumentType(INT nArg)   { return m_pArgs[nArg]
                                                      ? m_pArgs[nArg]->m_pszType
                                                      : NULL; }
    BOOL            IsArgumentString(INT nArg)
    {
        return m_pArgs[nArg] ? m_pArgs[nArg]->m_fIsString : FALSE;
    }
    BOOL            IsArgumentUnicode(INT nArg)
    {
        return m_pArgs[nArg] ? m_pArgs[nArg]->m_fIsUnicode : FALSE;
    }
    BOOL            IsArgumentInput(INT nArg)
    {
        return m_pArgs[nArg] ? m_pArgs[nArg]->m_fIsInput : FALSE;
    }
    BOOL            IsArgumentOutput(INT nArg)
    {
        return m_pArgs[nArg] ? m_pArgs[nArg]->m_fIsOutput : FALSE;
    }

    BOOL            IsReturnString()
    {
        return m_pRetVal->m_fIsString;
    }
    BOOL            IsReturnUnicode()
    {
        return m_pRetVal->m_fIsUnicode;
    }

    INT             GetArgumentCount()          { return m_nArgs; }
    BOOL            IsVoid()                    { return m_fVoid; }
    BOOL            HasVarArgs()                { return m_fVarArgs; }

    static VOID     SetTypeManager(TypeManager *ptmTypeManager);

protected:
    PCHAR           m_pszName;
    PCHAR           m_pszSpec;

    BOOL            m_fVoid;
    BOOL            m_fVarArgs;
    INT             m_nArgs;

    CArgument *     m_pRetVal;
    CArgument **    m_pArgs;

    AstFuncDecl *   m_pafdDeclaration;
    TypeProcedure * m_ptpType;
    SymbolCommonFunctionStuff * m_pscfsSymbol;

protected:
    static TypeManager *    s_ptmTypeManager;
};

//////////////////////////////////////////////////////////////////////////////
//
VOID CFunction::SetTypeManager(TypeManager *ptmTypeManager)
{
    s_ptmTypeManager = ptmTypeManager;
}

CFunction::CArgument::CArgument()
{
    m_pszName = NULL;
    m_pszSpec = NULL;
    m_pszType = NULL;

    m_ptType = NULL;
    m_psSymbol = NULL;

    m_fIsString = FALSE;
    m_fIsUnicode = FALSE;
    m_fIsInput = TRUE;
    m_fIsOutput = FALSE;
}

HRESULT CFunction::CArgument::Initialize(Symbol *psSymbol, Type *ptType, PCHAR pszSpec)
{
    ASSERT(m_ptType == NULL);                           // Only call once.

    m_ptType = ptType;
    m_psSymbol = psSymbol;

    if (pszSpec) {
        m_pszSpec = pszSpec;
    }
    if (psSymbol) {
        pszSpec = psSymbol->GetLintSpec();
        if (pszSpec && pszSpec[0] == '\0') {
            pszSpec = NULL;
        }
        if (pszSpec) {
            m_pszSpec = pszSpec;
        }
        // Type *ptRetyped = s_ptmTypeManager->RetypeSymbol(psSymbol);
    }

    if (m_pszSpec) {
        m_fIsInput = (strchr(m_pszSpec, 'i') != NULL);
        m_fIsOutput = (strchr(m_pszSpec, 'o') != NULL);
    }

    Type *ptStop = NULL;
    CHAR *pszStop = NULL;
    TypeIndex tiStop = -1;

    if (psSymbol) {
        ptStop = psSymbol->GetType();
        tiStop = ptStop->GetIndex();
        pszStop = psSymbol->GetName();
    }

    CHAR szOut[1024];
    szOut[0] = '\0';
    szOut[1] = '\0';
#if 0
    DumpTypeToString(&szOut[1], ptType, NULL, pszStop, FALSE);
#else
    DumpTypeToString(&szOut[1], ptType, tiStop, pszStop, FALSE);
#endif

    INT cbLen = strlen(&szOut[1]) + 1;
    m_pszType = new CHAR [cbLen];
    ASSERT(m_pszType);

    strcpy(m_pszType, &szOut[1]);

    if (strcmp(m_pszType, "PSTR") == 0) {
        m_fIsString = TRUE;
        m_fIsUnicode = FALSE;
    }
    else if (strcmp(m_pszType, "PWSTR") == 0) {
        m_fIsString = TRUE;
        m_fIsUnicode = TRUE;
    }
    else if (strcmp(m_pszType, "LPSTR") == 0) {
        m_fIsString = TRUE;
        m_fIsUnicode = FALSE;
    }
    else if (strcmp(m_pszType, "LPCSTR") == 0) {
        m_fIsString = TRUE;
        m_fIsUnicode = FALSE;
    }
    else if (strcmp(m_pszType, "LPCWSTR") == 0) {
        m_fIsString = TRUE;
        m_fIsUnicode = TRUE;
    }
    else if (strcmp(m_pszType, "LPWSTR") == 0) {
        m_fIsString = TRUE;
        m_fIsUnicode = TRUE;
    }
    return S_OK;
}

CFunction::CArgument::~CArgument()
{
    if (m_pszType) {
        delete[] m_pszType;
        m_pszType = NULL;
    }
    m_pszName = NULL;
    m_pszSpec = NULL;
    m_psSymbol = NULL;
    m_ptType = NULL;
}

//////////////////////////////////////////////////////////////////////////////
//
TypeManager *   CFunction::s_ptmTypeManager = NULL;

CFunction::CFunction()
{
    m_pszName = NULL;
    m_pszSpec = NULL;

    m_fVoid = FALSE;
    m_fVarArgs = FALSE;

    m_nArgs = 0;
    m_pArgs = NULL;
    m_pRetVal = NULL;

    m_pafdDeclaration = NULL;
    m_ptpType = NULL;
    m_pscfsSymbol = NULL;

}

HRESULT CFunction::Initialize(SymbolCommonFunctionStuff *pscfsSymbol)
{
    ASSERT(m_pscfsSymbol == NULL);                      // Only call once.

    ASSERT(pscfsSymbol->GetKind() == skImport ||
           pscfsSymbol->GetKind() == skFunction);

    m_pscfsSymbol = pscfsSymbol;
    m_pszName = pscfsSymbol->GetName();
    m_pszSpec = pscfsSymbol->GetLintSpec();
    if (m_pszSpec && m_pszSpec[0] == '\0')
        m_pszSpec = NULL;

    Symbol *psDecl = NULL;
    if (pscfsSymbol->GetKind() == skFunction) {
        psDecl = (static_cast <SymbolFunction *>(pscfsSymbol))->GetDeclaredType();
    }
    else if (pscfsSymbol->GetKind() == skImport) {
        psDecl = (static_cast <SymbolImport *>(pscfsSymbol))->GetDeclaredType();
    }
    else {
        ASSERT(!"Unsupported symbol kind.");
    }

    m_pafdDeclaration = static_cast <AstFuncDecl *>(pscfsSymbol->GetDeclaration());;
    ASSERT(m_pafdDeclaration->FDeclaration());
    ASSERT(m_pafdDeclaration->GetOpCode() == ocFuncDecl);

    m_ptpType = static_cast <TypeProcedure *>(pscfsSymbol->GetType());;
    ASSERT(m_ptpType->GetTypeKind() == tkProcedure);

    m_fVarArgs = m_ptpType->FVarArgs();
    m_fVoid = FALSE;

    if (m_ptpType->GetReturnType()) {
        TypePrimitive *pt = static_cast <TypePrimitive *>(m_ptpType->GetReturnType());
        if (pt->GetTypeKind() == tkPrimitive && pt->GetPrimitiveKind() == ptVoid) {
            m_fVoid = TRUE;
        }
    }

    m_nArgs = m_pafdDeclaration->GetParameterCount();
    Symbol ** psParms = m_pafdDeclaration->GetParameterDeclaredTypeVector();
    char **ppszSpecs = m_pafdDeclaration->GetParameterLintSpecVector();

    m_pRetVal = new CArgument;
    ASSERT(m_pRetVal);
    m_pArgs = new CArgument * [m_nArgs];
    ASSERT(m_pArgs);
    for (INT nArg = 0; nArg < m_nArgs; nArg++) {
        m_pArgs[nArg] = NULL;
    }

    m_pRetVal->Initialize(psDecl, m_ptpType->GetReturnType(), NULL);

    TypeArgumentListIterator Arguments = m_ptpType->GetArgumentIterator();
    Type *ptArgument = NULL;
    for (nArg = 0, ptArgument = Arguments.NextType();
         ptArgument != NULL;
         ptArgument = Arguments.NextType(), nArg++) {

        Symbol *psSymbol = NULL;
        if (nArg < m_nArgs && psParms && psParms[nArg]) {
            psSymbol = psParms[nArg];
        }

        m_pArgs[nArg] = new CArgument;
        ASSERT(m_pArgs[nArg]);

        m_pArgs[nArg]->Initialize(psSymbol,
                                  ptArgument,
                                  ppszSpecs ? ppszSpecs[nArg] : NULL);
    }

    if (m_fVarArgs) {
        ASSERT(nArg + 1 == m_nArgs);
    }
    else {
        if (nArg != m_nArgs) {
            printf("AST made error in counting args for %s (%d != %d).\n",
                   m_pszName, nArg, m_nArgs);
        }
        // @todo: Sometimes AST lies about the argument count: ASSERT(nArg == m_nArgs);
    }
    if (m_fVoid) {
        ASSERT(GetReturnType());
    }

    return S_OK;
}

CFunction::~CFunction()
{
    if (m_pRetVal) {
        delete m_pRetVal;
        m_pRetVal = NULL;
    }
    if (m_pArgs) {
        for (INT n = 0; n < m_nArgs; n++) {
            if (m_pArgs[n]) {
                delete m_pArgs[n];
                m_pArgs[n] = NULL;
            }
        }
        delete m_pArgs;
        m_pArgs = NULL;
    }
}

PCHAR CFunction::GetFunctionCallConv()
{
    ASSERT(m_ptpType);
    return CallingConventionToString(m_ptpType->GetCallingConvention());
}

//////////////////////////////////////////////////////////////////////////////
//

class COutputFile
{
public:
    COutputFile();
    ~COutputFile();

    HRESULT     Initialize(PCHAR pszFile);
    HRESULT     Print(PCHAR pszMsg, ...);

protected:
    FILE *      m_pFile;
    BOOL        m_fOnNewLine;
    INT         m_nColumn;
    INT         m_nIndent;
    INT         m_nParens;
    INT         m_nParenIndents[64];
};

COutputFile::COutputFile()
{
    m_pFile = NULL;
    m_nColumn = 0;
    m_nIndent = 0;
    m_nParens = 0;
    m_fOnNewLine = FALSE;
}

COutputFile::~COutputFile()
{
    if (m_pFile) {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

HRESULT COutputFile::Initialize(PCHAR pszFile)
{
    ASSERT(m_pFile == NULL);                            // Only call once.
    ASSERT(pszFile);

    m_pFile = fopen(pszFile, "w");
    if (FAILED(m_pFile)) {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT COutputFile::Print(PCHAR pszMsg, ...)
{
    CHAR szBuffer[2048];
    double d = 0.0;                                     // Required for FP support

    va_list args;
    va_start(args, pszMsg);

    vsprintf(szBuffer, pszMsg, args);

    for (PCHAR psz = szBuffer; *psz; psz++) {
        if (*psz == '\n') {
            m_fOnNewLine = TRUE;
            fputc(*psz, m_pFile);
            m_nColumn = 0;
        }
        else {
            if (*psz == '}') {
                m_nIndent -= 4;
            }

            if (m_fOnNewLine) {
                for (INT n = 0; n < m_nIndent; n++) {
                    fputc(' ', m_pFile);
                }
                m_nColumn = m_nIndent;
                m_fOnNewLine = FALSE;
            }

            if (*psz == '\t') {
                INT nSpace = (m_nColumn + 4) % 4;
                m_nColumn += nSpace - 1;                // 1 for char default below.
            }
            else if (*psz == '(') {
                m_nParenIndents[m_nParens++] = m_nIndent;
                m_nIndent = m_nColumn + 1;
            }
            else if (*psz == ')') {
                ASSERT(m_nParens > 0);
                m_nIndent = m_nParenIndents[--m_nParens];
            }
            else if (*psz == '{') {
                m_nIndent += 4;
            }

            fputc(*psz, m_pFile);
            m_nColumn++;
        }
    }
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
//
VOID DumpTrampoline(COutputFile *pcof, CFunction *pcd)
{
    pcof->Print("\n");
    pcof->Print("DETOUR_TRAMPOLINE(%s %s ",
                pcd->GetReturnType(),
                pcd->GetFunctionCallConv());
    pcof->Print("Real_%s(", pcd->GetFunctionName());

    INT nArgs = pcd->GetArgumentCount();
    for (INT nArg = 0; nArg < nArgs; nArg++) {
        if (pcd->GetArgumentType(nArg)) {
            pcof->Print("%s ", pcd->GetArgumentType(nArg));
        }
        if (pcd->GetArgumentName(nArg)) {
            pcof->Print("%s", pcd->GetArgumentName(nArg));
        }
        else {
            pcof->Print("a%d", nArg);
        }
        if (nArg < nArgs - 1) {
            pcof->Print(",\n");
        }
    }
    if (nArgs == 0) {
        pcof->Print("void");
    }
    pcof->Print("),\n");
    pcof->Print("%s);\n", pcd->GetFunctionName());
}

//////////////////////////////////////////////////////////////////////////////
//
VOID DumpTrampolineWith(COutputFile *pcof, CFunction *pcd)
{
    pcof->Print("DetourFunctionWithTrampoline((PBYTE)Real_%s,\n",
                pcd->GetFunctionName());
    pcof->Print("(PBYTE)Mine_%s);\n",
                pcd->GetFunctionName());
}

//////////////////////////////////////////////////////////////////////////////
//
VOID PrintEnter(COutputFile *pcof, CFunction *pcd)
{
    INT nArgs = pcd->GetArgumentCount();
    INT nArg;

    BOOL bHasString = FALSE;
    for (nArg = 0; nArg < nArgs; nArg++) {
        if (pcd->IsArgumentString(nArg) && pcd->IsArgumentInput(nArg)) {
            bHasString = TRUE;
            break;
        }
    }
#ifdef ONLY_PRINT_STRINGS
    if (!bHasString) {
        return;
    }
#endif

    pcof->Print("//_PrintEnter(\"%s(", pcd->GetFunctionName());
#ifdef LIMIT_TO_FIRST_FOUR_ARGUMENTS
    if (nArgs > 4) {
        nArgs = 4;
    }
#endif
    for (nArg = 0; nArg < nArgs; nArg++) {
        PCHAR pszSpec = pcd->GetArgumentSpec(nArg);

        if (pcd->IsArgumentString(nArg) && pcd->IsArgumentInput(nArg)) {
            if (pcd->IsArgumentUnicode(nArg)) {
                pcof->Print("%%ls");
            }
            else {
                pcof->Print("%%hs");
            }
        }
        else {
            pcof->Print("%%lx");
        }
        if (nArg < (nArgs - 1))
            pcof->Print(",");
    }
    pcof->Print(")\\n\"");
    for (nArg = 0; nArg < nArgs; nArg++) {
        pcof->Print(", ");
        if (pcd->GetArgumentName(nArg)) {
            pcof->Print("%s", pcd->GetArgumentName(nArg));
        }
        else {
            pcof->Print("a%d", nArg);
        }
    }
    pcof->Print(");\n\n");
}

//////////////////////////////////////////////////////////////////////////////
//
VOID PrintExit(COutputFile *pcof, CFunction *pcd, PCHAR pszRtn)
{
    INT nArgs = pcd->GetArgumentCount();
    INT nArg;

    BOOL bHasString = FALSE;
    for (nArg = 0; nArg < nArgs; nArg++) {
        if (pcd->IsArgumentString(nArg) && pcd->IsArgumentOutput(nArg)) {
            bHasString = TRUE;
            break;
        }
    }
#ifdef ONLY_PRINT_STRINGS
    if (!bHasString) {
        return;
    }
#endif

    pcof->Print("//_PrintExit(\"%s(", pcd->GetFunctionName());
#ifdef LIMIT_TO_FIRST_FOUR_ARGUMENTS
    if (nArgs > 4) {
        nArgs = 4;
    }
#endif
    for (nArg = 0; nArg < nArgs; nArg++) {
        if (pcd->IsArgumentString(nArg) && pcd->IsArgumentOutput(nArg)) {
            if (pcd->IsArgumentUnicode(nArg)) {
                pcof->Print("%%ls");
            }
            else {
                pcof->Print("%%hs");
            }
        }
        if (nArg < (nArgs - 1))
            pcof->Print(",");
    }
    pcof->Print(")");
    if (pszRtn) {
        if (pcd->IsReturnString()) {
            if (pcd->IsReturnUnicode()) {
                pcof->Print(" -> %%ls\\n\"");
            }
            else {
                pcof->Print(" -> %%hs\\n\"");
            }
        }
        else {
            pcof->Print(" -> %%lx\\n\"");
        }
    }
    else {
        pcof->Print(" ->\\n\"");
    }
    for (nArg = 0; nArg < nArgs; nArg++) {
        if (pcd->IsArgumentString(nArg) && pcd->IsArgumentOutput(nArg)) {
            pcof->Print(", ");
            if (pcd->GetArgumentName(nArg)) {
                pcof->Print("%s", pcd->GetArgumentName(nArg));
            }
            else {
                pcof->Print("a%d", nArg);
            }
        }
    }
    if (pszRtn)
        pcof->Print(", %s", pszRtn);
    pcof->Print(");\n");
}

//////////////////////////////////////////////////////////////////////////////
//
VOID DumpDetour(COutputFile *pcof, CFunction *pcd, PCHAR pszInterfaceVar, DWORD dwImport)
{

    pcof->Print("%s %s ",
                pcd->GetReturnType(),
                pcd->GetFunctionCallConv());
    pcof->Print("Mine_%s(", pcd->GetFunctionName());

    INT nArgs = pcd->GetArgumentCount();
    for (INT nArg = 0; nArg < nArgs; nArg++) {

        if (pcd->GetArgumentType(nArg)) {
            pcof->Print("%s ", pcd->GetArgumentType(nArg));
        }
        if (pcd->GetArgumentName(nArg)) {
            pcof->Print("%s", pcd->GetArgumentName(nArg));
        }
        else {
            pcof->Print("a%d", nArg);
        }
        if (nArg < nArgs - 1) {
            pcof->Print(",\n");
        }
    }
    if (nArgs == 0) {
        pcof->Print("void");
    }
    pcof->Print(")\n");
    pcof->Print("{\n");

    PrintEnter(pcof, pcd);

    if (!pcd->IsVoid()) {
        pcof->Print("%s rv = 0;\n", pcd->GetReturnType());
        pcof->Print("__try {\n");
        pcof->Print("rv = ");
    }
    else {
        pcof->Print("__try {\n");
    }

    pcof->Print("Real_%s(", pcd->GetFunctionName());

    nArgs = pcd->GetArgumentCount();
    for (nArg = 0; nArg < nArgs; nArg++) {
        if (pcd->GetArgumentName(nArg)) {
            pcof->Print("%s", pcd->GetArgumentName(nArg));
        }
        else {
            pcof->Print("a%d", nArg);
        }
        if (nArg < nArgs - 1) {
            pcof->Print(", ");
        }
    }
    pcof->Print(");\n");
    pcof->Print("} __finally {\n");
    PrintExit(pcof, pcd, (pcd->IsVoid() ? NULL : "rv"));
    pcof->Print("};\n");

    pcof->Print("m_dwAPICount[%d]++;\n",dwImport);
    if (!pcd->IsVoid()) {

        pcof->Print("return rv;\n");
    }

    pcof->Print("}\n");
}

//////////////////////////////////////////////////////////////////////////////
//
VOID DumpCleanup(COutputFile *pcof, CFunction *pcd, DWORD dwImport)
{

  pcof->Print("if (m_dwAPICount[%d]) {\n",dwImport);
  pcof->Print("_Print(\"\\t%s\\t",pcd->GetFunctionName());
  pcof->Print("%%");
  pcof->Print("d\\n\",m_dwAPICount[%d]);\n}\n",dwImport );

 }

//////////////////////////////////////////////////////////////////////////////
//
int __cdecl CompareFunctionNames(const void *e1, const void *e2)
{
    CFunction *pcf1 = *(CFunction **)e1;
    CFunction *pcf2 = *(CFunction **)e2;

    return strcmp(pcf1->GetFunctionName(), pcf2->GetFunctionName());
}

//////////////////////////////////////////////////////////////////////////////
//
CHAR *aszFilters[] = {
    "memcmp",
    "memset",
    "DeleteCriticalSection",
    "EnterCriticalSection",
    "InitializeCriticalSection",
    "LeaveCriticalSection",
    "GetLastError",
    "SetLastError",
    "TlsAlloc",
    "TlsFree",
    "TlsGetValue",
    "TlsSetValue",
    "UnhandledExceptionFilter",
    "VirtualAlloc",
    "VirtualFree",
    "VirtualLock",
    "VirtualProtect",
    "VirtualQuery",
    "VirtualUnlock",
    "GetProcessHeap",
    "HeapAlloc",
    "HeapCompact",
    "HeapCreate",
    "HeapDestroy",
    "HeapFree",
    "HeapReAlloc",
    "HeapSize",
    "HeapValidate",
    "HeapWalk",
    "InterlockedDecrement",
    "InterlockedExchange",
    "InterlockedIncrement"
};


INT nFilters = sizeof(aszFilters) / sizeof(CHAR *);


BOOL FFilterImport(const CHAR *pszName)
{
    BOOL  fRes = FALSE;

    for(INT i=0; (i < nFilters) && (FALSE == fRes); i++) {

        fRes = (BOOL)(0 == strcmp(pszName, aszFilters[i]));
    }

    return fRes;
}

class CompilerInterface : public AstCL
{
public:
    CompilerInterface() : AstCL()
    {
    }

    // Uses c1 or c1xx from AstCL

    int c2(char **args)
    {
        // Read c1/c1xx temp files into AST format
        ReadModule(args);

        CFunction::SetTypeManager(module->GetTypeManager());

        SymbolTable *Globals = module->GetGlobalSymbolTable();
        SymbolTableMaskedEntryIterator Symbols
            = Globals->GetMaskedEntryIterator(skImport);

        INT nImports = 0;
        for (Symbol *psSymbol = Symbols.NextSymbol();
             psSymbol != NULL;
             psSymbol = Symbols.NextSymbol()) {

            if (FFilterImport(psSymbol->GetName()))
                continue;

            AstNode *panDeclaration = psSymbol->GetDeclaration();
            if (panDeclaration == NULL ||
                panDeclaration->FCompilerGenerated()) {

                continue;
            }

            if (psSymbol->GetKind() == skImport) {
                nImports++;
            }
        }

        CFunction **pImports = new CFunction * [nImports];
        ASSERT(pImports);
        for (INT nImport = 0; nImport < nImports; nImport++) {
            pImports[nImport] = NULL;
        }

        nImport = 0;
        Symbols = Globals->GetMaskedEntryIterator(skImport);
        for (psSymbol = Symbols.NextSymbol();
             psSymbol != NULL;
             psSymbol = Symbols.NextSymbol()) {

            if (FFilterImport(psSymbol->GetName()))
                continue;

            AstNode *panDeclaration = psSymbol->GetDeclaration();
            if (panDeclaration == NULL ||
                panDeclaration->FCompilerGenerated()) {

                continue;
            }

            if (psSymbol->GetKind() == skImport) {
                pImports[nImport] = new CFunction;
                ASSERT(pImports[nImport]);

                pImports[nImport++]->Initialize(static_cast <SymbolImport *>(psSymbol));
            }
        }
        ASSERT(nImports == nImport);

        qsort(pImports, nImports, sizeof(pImports[0]), CompareFunctionNames);

        //////////////////////////////////////////////////////////////////////
        //
        COutputFile coutput;

        coutput.Initialize(objName);
        coutput.Print("/////////////////////////////////////////////////////////////\n");
        coutput.Print("//\n");
        coutput.Print("//  %s - Generated by MakeTrac from %s\n", objName, fileName);
        coutput.Print("//\n");
        coutput.Print("\n");
        coutput.Print("/////////////////////////////////////////////////////////////\n");
        coutput.Print("// Globals\n");
        coutput.Print("//\n");
        coutput.Print("DWORD m_dwAPICount[%d];\n",nImports);

        coutput.Print("/////////////////////////////////////////////////////////////\n");
        coutput.Print("// Trampolines\n");
        coutput.Print("//\n");
        for (nImport = 0; nImport < nImports; nImport++) {
            DumpTrampoline(&coutput, pImports[nImport]);
        }

        coutput.Print("\n");
        coutput.Print("/////////////////////////////////////////////////////////////\n");
        coutput.Print("// Detours\n");
        coutput.Print("//\n");
        for (nImport = 0; nImport < nImports; nImport++) {
            coutput.Print("\n");
            DumpDetour(&coutput, pImports[nImport], "pIWin32", nImport);
        }

        coutput.Print("\n");
        coutput.Print("/////////////////////////////////////////////////////////////\n");
        coutput.Print("// TrampolineWith\n");
        coutput.Print("//\n");
        coutput.Print("VOID TrampolineWith(VOID)\n");
        coutput.Print("{\n");
        for (nImport = 0; nImport < nImports; nImport++) {
            DumpTrampolineWith(&coutput, pImports[nImport]);
        }
        coutput.Print("}\n");

        coutput.Print("\n");
        coutput.Print("/////////////////////////////////////////////////////////////\n");
        coutput.Print("// Cleanup\n");
        coutput.Print("//\n");
        coutput.Print("VOID Cleanup(VOID)\n");
        coutput.Print("{\n");
        for (nImport = 0; nImport < nImports; nImport++) {
            DumpCleanup(&coutput, pImports[nImport], nImport);
        }
        coutput.Print("}\n");

        if (pImports) {
            for (nImport = 0; nImport < nImports; nImport++) {
                if (pImports[nImport]) {
                    delete pImports[nImport];
                    pImports[nImport] = NULL;
                }
            }
            delete[] pImports;
            pImports = NULL;
        }

        delete module;
        delete ilPath;

        return 0;
    }

    int link(char **args)
    {
        return 0;
    }

};

//////////////////////////////////////////////////////////////////////////////
//

int main(int argc, char **argv)
{
    Astlib::Initialize(stderr);

    CompilerInterface hooks;
    int rc = RunCL(&hooks, argc, argv);

    Astlib::Finalize();

    return rc;
}
