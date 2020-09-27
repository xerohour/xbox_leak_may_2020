/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: FlyingICount.cpp
*
* File Comments:
*
*
***********************************************************************/

#include <stdio.h>
#include "vulcanapi.h"
#include "flyingicount.h"

VSystem *g_pSystem = NULL;
VProg *g_pProg = NULL;
VComp *g_pComp = NULL;

ADDR g_pvCount = NULL;
VProc *g_pProcInstrumented = NULL;

// Local helper routines
void RemoveInstrumentation();

void RefreshProcesses()
{
    RemoveInstrumentation();

    g_pProg = NULL;
    g_pComp = NULL;

    ClearProgramList();
    ClearComponentList();
    ClearFunctionList();

    if (NULL == g_pSystem)
    {
        g_pSystem = VSystem::Open();
    }
    else
    {
        g_pSystem->RefreshProgs();
    }

    for (VProg *pProg = g_pSystem->FirstProg(); pProg; pProg = pProg->Next())
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

int GetCounter()
{
    DWORD dw = 0;

    if (g_pvCount)
    {
        // Read counter

        g_pProg->ReadMemory( g_pvCount, sizeof(dw), &dw );
    }

    return dw;
}

void RefreshModules( VProg *pProg )
{
    if (pProg == g_pProg)
    {
        return;
    }

    RemoveInstrumentation();

    g_pComp = NULL;

    ClearComponentList();
    ClearFunctionList();

    g_pProg = pProg;

    for (VComp *pComp = g_pProg->FirstComp(); pComp; pComp = pComp->Next())
    {
        const char *szName = pComp->InputName();
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

void RefreshFunctions(VComp *pComp)
{
    if (pComp == g_pComp)
    {
        return;
    }

    RemoveInstrumentation();

    ClearFunctionList();

    g_pComp = NULL;

    try
    {
        // If needed, set the local path to the binary here
        //
        // pComp->DynamicSetLocalModuleName("...");

        pComp->Build();
    }
    catch(VErr &err)
    {
        Error( err.GetWhat() );
        return;
    }
    catch(...)
    {
        Error( "Unknown Vulcan Error Building IR" );
        return;
    }

    for (VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
    {
        AddFunctionToList( pProc->Name(), pProc );
    }

    g_pComp = pComp;
    pComp->SetCurrentWorking();
}

void RemoveInstrumentation()
{
    if (g_pProcInstrumented)
    {
        g_pComp->RedirectProc( g_pProcInstrumented, NULL );
        g_pProcInstrumented = NULL;
    }
}

void SetInstrumentation( VProc *pProc )
{
    if (pProc == g_pProcInstrumented)
    {
        return;
    }

    RemoveInstrumentation();

    // Copy the procedure
    VProc *pProcCopy = pProc->Copy( g_pComp );

    if (NULL == g_pvCount)
    {
        // We only need 4 bytes for this counter

        g_pvCount = g_pProg->AllocateMemory( 4 );
    }

    // Zero counter
    DWORD dw = 0;
    g_pProg->WriteMemory( g_pvCount, sizeof(dw), &dw );

    VAddress *pAddr = VAddress::Create( NULL, (long)g_pvCount ); //displacement of address

    for (VBlock *pBlk = pProcCopy->FirstBlock(); pBlk; pBlk = pBlk->Next())
    {
        VInst *pInst = pBlk->FirstInst();

        pInst->InsertPrev( VInst::Create( COp::PUSHD, X86Register::EAX ));
        pInst->InsertPrev( VInst::Create( COp::MOVD, X86Register::EAX, pAddr ));

        VAddress *pAddrInc = VAddress::Create( X86Register::EAX, pBlk->Size() );
        pInst->InsertPrev( VInst::Create( COp::LEAD, X86Register::EAX, pAddrInc ));

        pInst->InsertPrev( VInst::Create( COp::MOVD, pAddr, X86Register::EAX ));
        pInst->InsertPrev( VInst::Create( COp::POPD, X86Register::EAX ));
    }

    pProcCopy->Commit( g_pComp );
    g_pComp->RedirectProc( pProc, pProcCopy);
    g_pProcInstrumented = pProc;
}
