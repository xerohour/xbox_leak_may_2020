/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-2000 Microsoft Corporation.
*
* File: vulcan.inl
*
* File Comments:
*
*
***********************************************************************/

#pragma warning(push)
#pragma warning(disable:4097)

//  VSrcIter inlines
VSrcIter::VSrcIter()
{
    m_pSrcCur = NULL;
}

VSrcIter::VSrcIter( VSrcIter &v) 
{
    m_pSrcCur = v.m_pSrcCur; 
    m_cSrcAfter = v.m_cSrcAfter; 
    m_cSrcBefore = v.m_cSrcBefore; 
}

VSrcInfo *VSrcIter::Curr() const     
{
    return m_pSrcCur;
}

bool VSrcIter::Done() const     
{
    return IsEmpty() || 
           m_cSrcAfter < 0   || 
           m_cSrcBefore < 0;
}

bool VSrcIter::IsEmpty() const  
{
   return  (m_cSrcAfter < 0 && m_cSrcBefore <= 0) ||
           (m_cSrcAfter <= 0 && m_cSrcBefore < 0) ||
           (m_cSrcAfter == 0 && m_cSrcBefore == 0 && (Curr() == NULL || (Curr()->FileName() == NULL)));

}

size_t VSrcIter::Length() const   
{
    return IsEmpty() ? 0 : 1 + m_cSrcAfter + m_cSrcBefore;
}
    
// VProg inlines
void VProg::SetUserData(void *pvData, VUserData *pDataStore)
{
    pDataStore->SetUserData(pvData, this);
}

void *VProg::GetUserData(VUserData *pDataStore)
{
    return pDataStore->GetUserData(this);
}

// VComp inlines
void VComp::SetUserData(void *pvData, VUserData *pDataStore)
{
    pDataStore->SetUserData(pvData, this);
}

void *VComp::GetUserData(VUserData *pDataStore)
{
    return pDataStore->GetUserData(this);
}

// VSect inlines

// VProc inlines


void VProc::SetUserData(void *pvData, VUserData *pDataStore)
{
    pDataStore->SetUserData(pvData, this);
}

void *VProc::GetUserData(VUserData *pDataStore)
{
    return pDataStore->GetUserData(this);
}

inline bool SkipAllButCodeBlks(void *p)
{
   VBlock *pBlk = (VBlock *) p;
   return ((NULL == pBlk) ||
           pBlk->IsDataBlock() ||
           pBlk->IsUnreachable());
}

inline bool SkipAllButDataBlks(void *p)
{
   VBlock *pBlk = (VBlock *) p;
   return (!pBlk->IsDataBlock());
}

inline bool SkipAllButDeadBlks(void *p)
{
   VBlock *pBlk = (VBlock *) p;
   return (!pBlk->IsUnreachable());
}

VBlock *VBlock::Next()
{
    VBlock *pBlk = this;
    
    do
    {
        pBlk = ((CDListElem_VBlock *)pBlk)->Next();
    }
    while (pBlk && SkipAllButCodeBlks(pBlk));

    return pBlk;
}

VBlock *VBlock::Prev()
{
    VBlock *pBlk = this;
    
    do
    {
        pBlk = ((CDListElem_VBlock *)pBlk)->Prev();
    }
    while (pBlk && SkipAllButCodeBlks(pBlk));

    return pBlk;
}

VBlock *VBlock::First()
{
    VBlock *pBlk = CDListElem_VBlock::First();

    if (pBlk && SkipAllButCodeBlks(pBlk))
    {
        pBlk = pBlk->Next();
    }

    return pBlk;
}

VBlock *VBlock::Last()
{
    VBlock *pBlk = CDListElem_VBlock::Last();

    if (pBlk && SkipAllButCodeBlks(pBlk))
    {
        pBlk = pBlk->Prev();
    }

    return pBlk;
}

VBlock *VBlock::NextAll()
{
    return CDListElem_VBlock::Next();
}

VBlock *VBlock::PrevAll()
{
    return CDListElem_VBlock::Prev();
}

VBlock *VBlock::FirstAll()
{
    return CDListElem_VBlock::First();
}

VBlock *VBlock::LastAll()
{
    return CDListElem_VBlock::Last();
}

void VBlock::InsertNext(VBlock *p)
{
    CDListElem_VBlock::InsertNext(p);
}

void VBlock::InsertPrev(VBlock *p)
{
    CDListElem_VBlock::InsertPrev(p);
}

void VBlock::Remove()
{
    CDListElem_VBlock::Remove();
}

void VBlock::SetUserData(void *pvData, VUserData *pDataStore)
{
    pDataStore->SetUserData(pvData, this);
}

void *VBlock::GetUserData(VUserData *pDataStore)
{
    return pDataStore->GetUserData(this);
}

VInst *VInst::Next()
{
    return (VInst *) CDListElem_VInst::Next();
}

VInst *VInst::Prev()
{
    return (VInst *) CDListElem_VInst::Prev();
}

VInst *VInst::First()
{
    return (VInst *) CDListElem_VInst::First();
}

VInst *VInst::Last()
{
    return (VInst *) CDListElem_VInst::Last();
}

void VInst::InsertNext(VInst *p)
{
    CDListElem_VInst::InsertNext(p);
}

void VInst::InsertPrev(VInst *p)
{
    CDListElem_VInst::InsertPrev(p);
}

void VInst::Remove()
{
    CDListElem_VInst::Remove();
}

void VInst::SetUserData(void *pvData, VUserData *pDataStore)
{
    pDataStore->SetUserData(pvData, this);
}

void *VInst::GetUserData(VUserData *pDataStore)
{
    return pDataStore->GetUserData(this);
}

VExport *VExport::Prev()
{
    return CDListElem_VExport::Prev();
}

VExport *VExport::Next()
{
    return CDListElem_VExport::Next();
}

VExport *VExport::First()
{
    return CDListElem_VExport::First();
}

VExport *VExport::Last()
{
    return CDListElem_VExport::Last();
}

VExport *VExport::NextProc()
{
    VBlock *pBlk = Block();
    
    VExport *pNext = Next();
    while (pNext && (pNext->Block() != pBlk))
    {
        pNext = pNext->Next();
    }

    return pNext;
}

VExport *VExport::PrevProc()
{
    VBlock *pBlk = Block();
    
    VExport *pPrev = Prev();
    while (pPrev && (pPrev->Block() != pBlk))
    {
        pPrev = pPrev->Prev();
    }

    return pPrev;
}

VExport *VExport::FirstProc()
{
    VExport *pPrev = PrevProc();

    if (pPrev)
    {
        while (pPrev->PrevProc())
        {
            pPrev = pPrev->PrevProc();
        }

        return pPrev;
    }

    return this;
}

VExport *VExport::LastProc()
{
    VExport *pNext = NextProc();

    if (pNext)
    {
        while (pNext->NextProc())
        {
            pNext = pNext->NextProc();
        }

        return pNext;
    }

    return this;
}

VImport *VImport::Next()
{
    return CDListElem_VImport::Next();
}

VImport *VImport::Prev()
{
    return CDListElem_VImport::Prev();
}

VImport *VImport::First()
{
    return CDListElem_VImport::First();
}

VImport *VImport::Last()
{
    return CDListElem_VImport::Last();
}

VOperand::VOperand()                      
{
   m_operand = NULL;
   m_type = OpndNotUsed;
}

VOperand::VOperand(VOperand &v)
{
   m_operand = v.m_operand; 
   m_type = v.m_type;
}

VOperand::VOperand(BYTE *pb)
{
   m_operand = (void *) pb; 
   m_type = OpndData;
}


VOperand::VOperand(ERegister reg)
{
#if     (_MSC_VER > 1200)
   m_operand = (void *) reg;
#else   // !(_MSC_VER > 1200)
   m_operand = (void *) reg;
#endif  // !(_MSC_VER > 1200)
   m_type = OpndReg;
}

VOperand::VOperand(VAddress *pAddr)
{
   m_operand = (void *) pAddr;
   m_type = OpndAddress;
}

VOperand::VOperand(long immed)
{
#if     (_MSC_VER > 1200)
   m_operand = (void *) (size_t) immed;
#else   // !(_MSC_VER > 1200)
   m_operand = (void *) immed;
#endif  // !(_MSC_VER > 1200)
   m_type = OpndImmediate;
}

VOperand::VOperand(VBlock *pBlk)
{
   m_operand = (void *) pBlk;
   m_type = OpndBlock;
}

VOperand::VOperand(VBlock *pBlk, long offset)
{
   m_operand = CreateGA(pBlk, offset);  
   m_type = OpndGenAddressBlock;
}

VOperand::VOperand(VInst *pInst)
{
   m_operand = CreateGA(pInst);  
   m_type = OpndGenAddressBlock;
}

EOpndTypes VOperand::Type() 
{
   return m_type;
}

ERegister VOperand::Register()  
{
    if (m_type == OpndReg || m_type == OpndRegPair)
    {
#if     (_MSC_VER > 1200)
        return (ERegister) (size_t) m_operand;
#else   // !(_MSC_VER > 1200)
        return (ERegister) (int) m_operand;
#endif  // !(_MSC_VER > 1200)
    }

    return X86Register::ZERO;
}  
    
const VAddress *VOperand::Address()   
{
    if (m_type == OpndAddress)
    {
        return (const VAddress *) m_operand;
    }

    return NULL;
}
 
long VOperand::Immediate() 
{
   if (Type() == OpndImmediate) 
   {
#if     (_MSC_VER > 1200)
      return (long) (size_t) m_operand;
#else   // !(_MSC_VER > 1200)
      return (long) m_operand;
#endif  // !(_MSC_VER > 1200)
   }

   if (Type() == OpndGenAddressBlock)
   {
      return ImmediateGA(m_operand);
   }

   if (Type() == OpndAddress)
   {
      return Address()->Offset();
   }

   return 0;
}

VBlock *VOperand::Block()     
{
   if (Type() == OpndBlock) 
   {
      return (VBlock *) m_operand;
   }

   if (Type() == OpndGenAddressBlock)
   {
      return BlockGA(m_operand);
   }

   if (Type() == OpndAddress)
   {
      return Address()->Block();
   }

   return NULL;
}

inline VInst * VOperand::Inst()
{
   if (Type() == OpndGenAddressBlock)
   {
      return InstGA(m_operand);
   }

   return NULL;
}


void *VOperand::Anything()
{
   return m_operand;
}

bool VOperand::SetBlock( VBlock *pBlk )
{
    switch (Type())
    {
        case OpndImmediate:
            if (pBlk && Immediate())
            {
                // must convert to OpndGenAddressBlock
                m_operand = CreateGA(pBlk, Immediate());  
                m_type = OpndGenAddressBlock;
                return true;
            }
            else if (pBlk)
            {
                m_operand = (void *)pBlk;
                m_type = OpndBlock;
                return true;
            }
            else
            {
                return false;
            }

        case OpndAddress:
            {
                // Oops, this results in a small memory leak... don't do it alot
                VAddress *pAddr = VAddress::Create( Address() );
                pAddr->SetBlock( pBlk );
                m_operand = (void *) pAddr;
                return true;
            }

        case OpndBlock:
            if (pBlk)
            {
                // switch it
                m_operand = (void *)pBlk;
            }
            else
            {
                // immediate of 0
                m_operand = 0;
                m_type = OpndImmediate;
            }
            return true;

        case OpndGenAddressBlock:
            if (pBlk)
            {
                // switch it
                m_operand = CreateGA( pBlk, Immediate());
            }
            else
            {
                // immediate
                m_operand = (void *)(size_t)Immediate();
                m_type = OpndImmediate;
            }
            return true;
    }

    return false;
}

bool VOperand::SetImmediate( long imm )
{
    switch (Type())
    {
        case OpndImmediate:
            // must convert to OpndGenAddressBlock
            m_operand = (void *)(size_t)imm; 
            return true;

        case OpndAddress:
            {
                // Oops, this results in a small memory leak... don't do it alot
                VAddress *pAddr = VAddress::Create( Address() );
                pAddr->SetOffset( imm );
                m_operand = (void *) pAddr;
            }
            return true;

        case OpndBlock:
            if (imm)
            {
                // switch it
                m_operand = CreateGA( Block(), imm );
                m_type = OpndGenAddressBlock;
            }
            return true;

        case OpndGenAddressBlock:
            if (imm)
            {
                // switch it
                m_operand = CreateGA( Block(), imm);
                m_type = OpndGenAddressBlock;
            }
            else
            {
                // immediate
                m_operand = (void *)Block();
                m_type = OpndBlock;
            }
            return true;
    }

    return false;
}

bool VOperand::SetInst( VInst * pInst )
{
    switch (Type())
    {
        case OpndAddress:
            {
                // Oops, this results in a small memory leak... don't do it alot
                VAddress *pAddr = VAddress::Create( Address() );
                pAddr->SetInst( pInst );
                m_operand = (void *) pAddr;
            }
            return true;

        case OpndImmediate:
        case OpndBlock:
        case OpndGenAddressBlock:
            // Inst is always a GA from these guys
            m_operand = CreateGA( pInst );
            m_type = OpndGenAddressBlock;
            return true;
    }

    return false;
}

#pragma warning(pop)


#if     defined(VULCANDLL__)    //  Building the Vulcan DLL
#pragma warning(disable: 4786)        // 255 size identifier.  A template nightmare
#pragma warning(disable: 4514)        // unreferenced inline function has been removed
#pragma warning(disable: 4710)        // Function not expanded
#elif   defined(VULCANI__)      //  Building the static Vulcan lib
#pragma warning(disable: 4786)        // 255 size identifier.  A template nightmare
#pragma warning(disable: 4514)        // unreferenced inline function has been removed
#pragma warning(disable: 4710)        // Function not expanded
#endif

//These are required for those internal-external components
#pragma warning(disable: 4275)        // non dll-interface class
#pragma warning(disable: 4251)        // Needs DLL interface
#pragma warning(disable: 4239)        // Non standard extension used
