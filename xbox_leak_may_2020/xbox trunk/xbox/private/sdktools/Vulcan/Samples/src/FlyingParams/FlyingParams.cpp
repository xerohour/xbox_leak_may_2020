/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: FlyParamsUI.cpp
*
* File Comments:
*
*
***********************************************************************/

#include "flyingparams.h"
#include "vulcanapi.h"
#include "malloc.h"
#include "stdio.h"
#include "stdlib.h"

// State of currently selected things
char    g_szMachineSel[256] = {0};
VSystem *g_pSysSel  = NULL;
VProg   *g_pProgSel = NULL;
VComp   *g_pCompSel = NULL;
VProc   *g_pProcSel = NULL;
VSymbol *g_pSymSel  = NULL;

// Which procedures have recently been hit?
size_t   g_cProc = 0;
ADDR     g_addrProcHits = NULL;
BYTE *   g_rgProcHits = NULL;

// Value data
VALUE_DATA *g_rgValueData = NULL;
ADDR g_addrValueData = NULL;

// Dereference stuff
ADDR g_addrDerefStream = NULL;
VAddress *g_pAddrDerefHandler;
bool g_fProcSelInst = false;

// Local helper functions
void RemoveAllInstrumentation();
void AddProcHitInstrumentation( VProc *pProc, int iProc );
void GetNameFromSymbol( VSymbol *pSym, char *szSym, size_t cchSym );
void RemoveParamInstrumentation();
void RemoveProcHitInstrumentation();
bool GetProcessReadyForParams();
void WriteDerefStreamToProcess( int *rgDeref );
void ZeroParamCounters();
void CreateDerefStream( VSymbol **rgpSyms, size_t cPSym, int *rgDeref );
void AddParamInstrumentation( VSymbol *pSymRoot );
int _cdecl CmpValueData( const void *pv1, const void *pv2);
bool HasChildrenSym( VSymbol *pSym );

void RefreshProcesses( const char *szMachine )
{
    RemoveAllInstrumentation();

    ClearProgramList();
    g_pProcSel = NULL;

    ClearComponentList();
    g_pCompSel = NULL;

    ClearProcedureList();
    g_pProcSel = NULL;

    ClearParamsList();
    g_pSymSel = NULL;

    if (NULL != g_pSysSel &&
        0 != stricmp( szMachine, g_szMachineSel ))
    {
        // Machine name changed... go find that one?
        g_pSysSel->Destroy();
        g_pSysSel = NULL;
    }

    strcpy( g_szMachineSel, szMachine );

    if (NULL == g_pSysSel)
    {
        try
        {
            if (g_szMachineSel[0])
            {
                // Open named machine
                g_pSysSel = VSystem::Open(g_szMachineSel);
            }
            else
            {
                // Open local machine
                g_pSysSel = VSystem::Open((const char *)NULL);
            }
        }
        catch(...)
        {
            Error( "Vulcan failed to locate the remote client" );
            g_pSysSel = NULL;
            return;
        }
    }
    else
    {
        g_pSysSel->RefreshProgs();
    }

    for (VProg *pProg = g_pSysSel->FirstProg(); pProg; pProg = pProg->Next())
    {
        if (pProg->IsProcessLive())
        {
            const char *sz = pProg->Name();
            const char *szBase = strrchr( sz, '\\' );
            if (szBase)
            {
                sz = szBase + 1;
            }

            AddProgramToList( sz, pProg );
        }
    }
}

void OnSelChangeProcesses( VProg * pProg )
{
    if (pProg == g_pProgSel)
    {
        return;
    }

    RemoveAllInstrumentation();

    g_pCompSel = NULL;
    g_pProcSel = NULL;
    g_pSymSel = NULL;

    ClearComponentList();
    ClearProcedureList();
    ClearParamsList();

    g_pProgSel = pProg;

    if (NULL == pProg)
    {
        return;
    }

    for (VComp *pComp = pProg->FirstComp(); pComp; pComp = pComp->Next() )
    {
        const char *szName = pComp->InputName();
        if (szName)
        {
            const char *szLastSlash = strrchr( szName, '\\' );
            if (szLastSlash)
            {
                szLastSlash++;
            }
            else
            {
                szLastSlash = szName;
            }

            AddComponentToList( szLastSlash, pComp );
        }
    }
}

void OnSelChangeComponent( VComp *pComp )
{
    if (pComp == g_pCompSel)
    {
        return;
    }
    g_pCompSel = NULL;

    RemoveAllInstrumentation();

    g_pProcSel = NULL;
    g_pSymSel = NULL;

    ClearProcedureList();
    ClearParamsList();

    if (NULL == pComp)
    {
        return;
    }

    try
    {
        pComp->Build();
    }
    catch (VErr err)
    {
        Error( err.GetWhat() );
        return;
    }
    catch (...)
    {
        Error( "Vulcan failed to open the component" );
        return;
    }

    g_pCompSel = pComp;
    g_cProc = pComp->CountProcs();

    if (NULL == g_addrProcHits)
    {
        // To keep things simple, we assume all modules have less than 64k procedures
        g_addrProcHits = g_pProgSel->AllocateMemory( 64*1024 );
        g_rgProcHits = (BYTE *)malloc( 64*1024 );
    }

    // We are going to add instrumentation, so freeze the process
    g_pProgSel->Suspend();

    int iProc = 0;
    for (VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
    {
        if (!pProc->IsThunk())
        {
            // The procedure listbox will call us back for the name...
            AddProcedureToList( pProc, iProc );

            AddProcHitInstrumentation( pProc, iProc );

            iProc++;
        }
    }

    g_pProgSel->Resume();
    g_pCompSel->SetCurrentWorking();
}

void OnSelChangeProcedures( VProc *pProc )
{
    char szSym[1024];
    VSymbol *pSym;

    if (pProc == g_pProcSel)
    {
        return;
    }

    RemoveParamInstrumentation();
    ZeroParamCounters();

    g_pProcSel = NULL;
    g_pSymSel = NULL;
    ClearParamsList();

    if (NULL == pProc)
    {
        return;
    }

    for (pSym = pProc->FirstParam(g_pCompSel); pSym; pSym = pSym->Next())
    {
        GetNameFromSymbol( pSym, szSym, sizeof(szSym) );

        AddParamToTree( szSym, pSym, HasChildrenSym(pSym) );
    }

    // Also add the return symbol
    pSym = pProc->ReturnSymbol(g_pCompSel);
    if (pSym)
    {
        GetNameFromSymbol( pSym, szSym, sizeof(szSym) );

        AddParamToTree( szSym, pSym, HasChildrenSym(pSym) );
    }

    g_pProcSel = pProc;
}

void OnExpandParam( VSymbol *pSymFrom )
{
    char szSym[1024];

    VSymbol *pSym;

    if (pSymFrom->IsPointer())
    {
        pSym = pSymFrom->PTarget();
    }
    else
    {
        pSym = pSymFrom->FirstField();
    }

    for ( ; pSym; pSym = pSym->Next())
    {
        // Insert children
        GetNameFromSymbol( pSym, szSym, sizeof(szSym) );

        AddParamToTree( szSym, pSym, HasChildrenSym(pSym) );
    }

    pSym = pSymFrom->FirstBase();
    for ( ; pSym; pSym = pSym->Next())
    {
        // Insert children
        GetNameFromSymbol( pSym, szSym, sizeof(szSym) );

        AddParamToTree( szSym, pSym, HasChildrenSym(pSym) );
    }

}

void OnSelChangeParams( VSymbol **rgpSyms, size_t cPSym )
{
    // rgpSyms is a pointer to the array of selected symbols
    // The first one is actually selected, the next ones are its parents

    if (cPSym != 0 && rgpSyms[0] == g_pSymSel)
    {
        return;
    }

    RemoveParamInstrumentation();
    g_pSymSel = rgpSyms[0];

    if (cPSym == 0 || NULL == rgpSyms[0] || g_pProcSel == NULL)
    {
        return;
    }

    GetProcessReadyForParams();

    int *rgDeref = new int[ cPSym + 2 ];
    rgDeref[0] = -4; //default to reading 4 bytes
    CreateDerefStream( rgpSyms, cPSym, rgDeref );
    WriteDerefStreamToProcess( rgDeref );
    delete [] rgDeref;

    ZeroParamCounters();

    AddParamInstrumentation( rgpSyms[cPSym-1] );
}

void GetNameFromSymbol( VSymbol *pSym, char *szSym, size_t cchSym )
{
    char *pchName = NULL;
    const char *szName = pSym->Name();
    pSym->Type( szSym, cchSym, &pchName );
    if (szName && pchName)
    {
        // pchName points to the location to put the name after the type
        // this will be the end of the type name, unless this is a pointer to a function
        memmove( pchName + strlen(szName) + 1, pchName, strlen(pchName) );
        *pchName = ' ';
        strncpy( pchName+1, szName, strlen(szName) );
    }
}

void UpdateProcedureHitData()
{
    if (NULL == g_addrProcHits)
    {
        return;
    }

    g_pProgSel->ReadMemory( g_addrProcHits, 64*1024, g_rgProcHits );

    // Decay the hit functions so we can see them for a little while
    for (size_t i =0; i<g_cProc; i++)
    {
        if (g_rgProcHits[i] == 0)
        {
            continue;
        }
        else if (g_rgProcHits[i] < 5)
        {
            // trying to decay...
            g_rgProcHits[i]++;
        }
        else
        {
            g_rgProcHits[i] = 0;
        }
    }
    g_pProgSel->WriteMemory( g_addrProcHits, 64*1024, g_rgProcHits );
}

const char *SzNameAndHitFromProcedure( VProc * pProc, int iProc, bool &fHit )
{
    const char *szName = pProc->Name();

    fHit = false;

    if (g_rgProcHits)
    {
        fHit = g_rgProcHits[iProc] > 0;
    }

    return szName;
}

void RemoveAllInstrumentation()
{
    RemoveParamInstrumentation();
    ZeroParamCounters();

    RemoveProcHitInstrumentation();
}

void AddProcHitInstrumentation( VProc *pProc, int iProc )
{
    // Create a copy of the first block in the procedure
    VBlock *pEntry = pProc->FirstBlock();
    VBlock *pBlk = pEntry->Copy(g_pCompSel);
    pProc->InsertLastBlock( pBlk );

    // Add mov byte ptr [g_addrProcHits + iProc], 1 to the copied entry block
    VAddress *pAddrProcHit = VAddress::Create( NULL, long(g_addrProcHits) + iProc );
    pBlk->InsertFirstInst( VInst::Create( COp::MOVB, pAddrProcHit, 1 ));
    pAddrProcHit->Destroy();
    pBlk->Commit( g_pCompSel );

    // Make a new block which jumps to the copy of the first block in the procedure
    VBlock *pJump = VBlock::CreateCodeBlock(g_pCompSel);
    pJump->InsertFirstInst( VInst::Create( COp::JMP, pBlk ));

    // Write the jump block into the first block of the original procedure
    pEntry->ReplaceInstructions( pJump, g_pCompSel );
    pJump->Destroy(); // Free temp block
}

void RemoveProcHitInstrumentation()
{
    // This is unimplemented...
}

void WriteDerefStreamToProcess( int *rgDeref )
{
    if (NULL == g_addrDerefStream)
    {
        g_addrDerefStream = g_pProgSel->AllocateMemory( 2048 );
    }

    for (int i=0; rgDeref[i] >= 0; i++)
        ;

    size_t cb = (i + 1)* sizeof(int);
    g_pProgSel->WriteMemory( g_addrDerefStream, cb, rgDeref );
}

void CreateDerefStream( VSymbol **rgpSyms, size_t cPSym, int *rgDeref )
{
    if (cPSym > 1)
    {
        CreateDerefStream( rgpSyms + 1, cPSym - 1, rgDeref );
    }

    VSymbol *pSym = rgpSyms[0];

    size_t size = pSym->Size();
    if (size == 0)
    {
        // default to reading first 4 bytes of object
        size = 4;
    }

    // Go find the negative number in the stream
    for (int i=0; rgDeref[i] >= 0; i++)
        ;

    if (pSym->Offset())
    {
        rgDeref[i++] = pSym->Offset();
    }

    if (!pSym->FirstField() &&
        !pSym->IsInReg())
    {
        // This is not a structure
        rgDeref[i++] = 0;
    }

    if (pSym->IsBitField())
    {
        // Offset and size are packed into 6 bits each, with next highest bit set.
        // Then the whole thing is negated so we know we are at then end of the string
        rgDeref[i] = -(int)( 0x1000 | pSym->BitOffset() << 6 | pSym->BitSize() );
    }
    else
    {
        // We pass in the contents of the register
        rgDeref[i] = -(int)size;
    }
}

void AddParamInstrumentation( VSymbol *pSymRoot )
{
    VProc *pProcCopy = g_pProcSel->Copy( g_pCompSel );

    VBlock *pBlk = pProcCopy->FirstBlock();
    VInst *pInst = pBlk->FirstInst();

    if (pSymRoot == g_pProcSel->ReturnSymbol(g_pCompSel))
    {
        // Go get EAX at each return instruction
        for ( ; pBlk; pBlk = pBlk->Next())
        {
            VInst *pInst = pBlk->LastInst();
            if (pInst->Opcode() == COp::RET)
            {
                pInst->InsertPrev( VInst::Create( COp::PUSHD, long(g_addrDerefStream) ));
                pInst->InsertPrev( VInst::Create( COp::PUSHD, X86Register::EAX ));
                pInst->InsertPrev( VInst::Create( COp::CALL, g_pAddrDerefHandler ));
            }
        }
    }
    else if (pSymRoot->IsInReg())
    {
        // ECX, EDX, or EAX
        pInst->InsertPrev( VInst::Create( COp::PUSHD, long(g_addrDerefStream) ));
        pInst->InsertPrev( VInst::Create( COp::PUSHD, pSymRoot->Register() ));
        pInst->InsertPrev( VInst::Create( COp::CALL, g_pAddrDerefHandler ));
    }
    else
    {
        // Watch out for JMP to entry block...
        if (pInst->Opcode() == COp::JMP &&
            pInst->BlockTarget())
        {
            pBlk = pInst->BlockTarget();
            pInst = pBlk->FirstInst();
        }

        // Watch for MOVB [addr], 1 (our instrumentation)
        if (pInst->Opcode() == COp::MOVB &&
            pInst->Operand( Inst::Dest ).Type() == OpndAddress &&
            pInst->Operand( Inst::Src1 ).Immediate() == 1)
        {
            pInst = pInst->Next();
        }

        // Watch out for PUSH EBP, MOV ESP, EBP.. the param offset are after EBP is pushed
        if (pInst->Opcode() == COp::PUSHD &&
            pInst->Operand( Inst::Src1 ).Register() == X86Register::EBP)
        {
            VInst *pInstMov = pInst->Next();

            if (pInstMov->Opcode() == COp::MOVD &&
                pInstMov->Operand( Inst::Dest ).Register() == X86Register::EBP &&
                pInstMov->Operand( Inst::Src1 ).Register() == X86Register::ESP)
            {
                // Bump passed push ebp since that's where param offsets are calculated from
                pInst = pInstMov;
            }
        }

        // Push address of dererence data
        pInst->InsertPrev( VInst::Create( COp::PUSHD, long(g_addrDerefStream) ));

        // Save EAX
        pInst->InsertPrev( VInst::Create( COp::PUSHD, X86Register::EAX ));

        // Compute address of parameters on stack (dereference stream determines which one to read)
        VAddress *pAddrParams = VAddress::Create( X86Register::ESP, 8 ); // + 8 to reach across the pushed params
        pInst->InsertPrev( VInst::Create( COp::LEAD, X86Register::EAX, pAddrParams ));
        pAddrParams->Destroy();

        // Effectively push address and restore EAX by exchanging them
        VAddress *pAddrXCHG = VAddress::Create( X86Register::ESP, 0 );
        pInst->InsertPrev( VInst::Create( COp::IXCHD, VOperand(pAddrXCHG), VOperand(X86Register::EAX), VOperand(X86Register::EAX) ));
        pAddrXCHG->Destroy();

        // Call handler
        pInst->InsertPrev( VInst::Create( COp::CALL, g_pAddrDerefHandler ));
    }

    pProcCopy->Commit( g_pCompSel );
    g_pCompSel->RedirectProc( g_pProcSel, pProcCopy);
    g_fProcSelInst = true;
}

void RemoveParamInstrumentation()
{
    if (g_pProcSel && g_fProcSelInst)
    {
        g_pCompSel->RedirectProc( g_pProcSel, NULL );
        g_fProcSelInst = false;
    }
}

bool GetProcessReadyForParams()
{
    if (g_rgValueData != NULL)
    {
        return true; //alrady done
    }

    g_addrValueData = g_pProgSel->AllocateMemory( cbValueMax );
    g_rgValueData = (VALUE_DATA *)malloc( cbValueMax );

    // Inject the dll
    VBlock *pBlk = g_pCompSel->CreateImport( "FlyingParamsDll.dll", "rgValueData" );
    if (NULL == pBlk)
    {
        Error( "Can't load FlyingParamsDll into remote process" );
        return false;
    }

    // Write the counter into the dll
    ADDR addr = pBlk->Addr();
    g_pProgSel->ReadMemory( addr, sizeof(void *), &addr );
    g_pProgSel->WriteMemory( addr, sizeof(void *), &g_addrValueData );

    // Get the handler proc too
    pBlk = g_pCompSel->CreateImport( "FlyingParamsDll.dll", "_ValueHandler@4" );
    if (NULL == pBlk)
    {
        return false;
    }
    g_pAddrDerefHandler = VAddress::Create( pBlk );

    return true;
}

void ZeroParamCounters()
{
    if (g_rgValueData == NULL)
    {
        return;
    }

    memset( g_rgValueData, 0, cbValueMax );
    g_pProgSel->WriteMemory( g_addrValueData, cbValueMax, g_rgValueData );
}

void GetValueStringFromValueIndex( int iVI, char *szName )
{
    if (NULL == g_rgValueData || NULL == g_pSymSel)
    {
        strcpy(szName, "0");
        return;
    }

    if (g_pSymSel->IsPointer())
    {
        //UNDONE: use type information to print this pretty
        sprintf(szName, "0x%08X", g_rgValueData[iVI].value );
    }
    else
    {
        // Get type
        char szType[1024];
        g_pSymSel->Type( szType, sizeof(szType), NULL );

        if (strstr( szType, "float" ))
        {
            sprintf(szName, "%f", *(float *)&g_rgValueData[iVI].value );
        }
        else if (strstr( szType, "double" ))
        {
            sprintf(szName, "%f", *(double *)&g_rgValueData[iVI].value );
        }
        else if (strstr( szType, "int64" ))
        {
            sprintf(szName, "%I64d", *(double *)&g_rgValueData[iVI].value );
        }
        else if (strstr( szType, "int" ) ||
                 strstr( szType, "char" ) ||
                 strstr( szType, "short" ) ||
                 strstr( szType, "long" ))
        {
            if (strstr( szType, "unsigned" ))
            {
                sprintf(szName, "%u", *(int *)&g_rgValueData[iVI].value );
            }
            else
            {
                sprintf(szName, "%d", *(int *)&g_rgValueData[iVI].value );
            }
        }
        else
        {
            sprintf(szName, "%p", *(int **)&g_rgValueData[iVI].value );
        }
    }
}

int GetValueCountFromValueIndex( int iVI )
{
    if (NULL == g_rgValueData)
    {
        return 0;
    }

    return g_rgValueData[iVI].count;
}

int _cdecl CmpValueData( const void *pv1, const void *pv2)
{
    VALUE_DATA *pp1 = (VALUE_DATA *)pv1;
    VALUE_DATA *pp2 = (VALUE_DATA *)pv2;

    return pp2->count - pp1->count;
}

int CValuesUpdateValueData()
{
    int i;

    if (g_pCompSel == NULL ||
        g_rgValueData == NULL)
    {
        return 0;
    }

    g_pProgSel->ReadMemory( g_addrValueData, cbValueMax, g_rgValueData );
    for (i=0; i<cValueMax; i++)
    {
        // 0 terminated array
        if (0 == g_rgValueData[i].count)
        {
            break;
        }
    }

    qsort( g_rgValueData, i, sizeof(VALUE_DATA), CmpValueData );
    return i;
}

bool HasChildrenSym( VSymbol *pSym )
{
    if (pSym->IsPointer())
    {
        if (pSym->PTarget() != NULL)
        {
            return true;
        }
    }
    else
    {
        if (pSym->FirstField() != NULL)
        {
            return true;
        }

        if (pSym->FirstBase() != NULL)
        {
            return true;
        }
    }

    return false;
}

void Cleanup()
{
    if (g_pSysSel)
    {
        g_pSysSel->Destroy();
        g_pSysSel = NULL;
    }
}
