/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: RegsUseDef.cpp
*
* File Comments:
*
*
***********************************************************************/
#pragma warning(disable: 4514)

#include "vulcanapi.h"
#include "vulcanbmp.h"

#include <stdlib.h>
#include <stdio.h>

int __cdecl main(int argc, char** argv);
void __stdcall Myhandler(VErr verr);
void UIRegs(char* szImage);
void PrintSetRegs( CBmp *pBmp );


void UIRegs(char* szImage)
{
   VProg *pprog = VProg::Open(szImage);

   CBmp bmp( X86Register::X86REGMAX );

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         printf("%s:\n",pProc->Name());
         for(VBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            for(VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next())
	         {
               char szBuf[256]; 
               pInst->PrintAsm( szBuf, sizeof(szBuf), pComp );
               printf(szBuf);

               bmp.ReInitialize();
               pInst->RegsUse( &bmp, true );

               printf("\tUses (no cover): ");
               PrintSetRegs( &bmp );
               printf("\n");

               bmp.ReInitialize();
               pInst->RegsUse( &bmp, false );
               
               printf("\tUses (cover): ");
               PrintSetRegs( &bmp );
               printf("\n");

               bmp.ReInitialize();
               pInst->RegsDef( &bmp, true );

               printf("\tDefs (no cover): ");
               PrintSetRegs( &bmp );
               printf("\n");

               bmp.ReInitialize();
               pInst->RegsDef( &bmp, false );

               printf("\tDefs (cover): ");
               PrintSetRegs( &bmp );
               printf("\n\n");
            }
         }
      }                             
   }

   // To save time, we don't destroy the program object
   // pprog->Destroy();

   // This should only be called after ALL Vulcan objects have been Destroyed
   // It will clean up any outstanding allocations
   // VulcanCleanUpAllAllocations();
}

char* ConvertX86RegToStr(ERegister reg)
{
   X86Reg ereg = (X86Reg) reg;


   // You'll notice in this function references to registers like EAH
   // We created EAH to independantly represent the high 16 bits of EAX.  
   // Thus, the x86 register picture looks like:
   //
   // +===========================+
   // |            EAX            |
   // +=============+=============+
   // |             |     AX      |
   // |     EAH     +======+======+
   // |             |  AH  |  AL  |
   // +=============+======+======+
   //


   switch (ereg)
   {
   case X86Register::AL:           return "AL";
   case X86Register::AH:           return "AH";
   case X86Register::AX:           return "AX";
   case X86Register::EAX:          return "EAX";
   case X86Register::EAH:          return "EAH"; 

   case X86Register::BL:           return "BL";
   case X86Register::BH:           return "BH";
   case X86Register::BX:           return "BX";
   case X86Register::EBX:          return "EBX";
   case X86Register::EBH:          return "EBH";

   case X86Register::CL:           return "CL";
   case X86Register::CH:           return "CH";
   case X86Register::CX:           return "CX";
   case X86Register::ECX:          return "ECX";
   case X86Register::ECH:          return "ECH";

   case X86Register::DL:           return "DL";
   case X86Register::DH:           return "DH";
   case X86Register::DX:           return "DX";
   case X86Register::EDX:          return "EDX";
   case X86Register::EDH:          return "EDH";

   case X86Register::SI:           return "SI";
   case X86Register::ESI:          return "ESI";
   case X86Register::SIH:          return "SIH";

   case X86Register::DI:           return "DI";
   case X86Register::EDI:          return "EDI";
   case X86Register::DIH:          return "DIH";

   case X86Register::SP:           return "SP";
   case X86Register::ESP:          return "ESP";
   case X86Register::SPH:          return "SPH";

   case X86Register::BP:           return "BP";
   case X86Register::EBP:          return "EBP";
   case X86Register::BPH:          return "BPH";

   //case X86Register::EFLAGS:     return "EFLAGS";
   case X86Register::CF:           return "CF";
   case X86Register::PF:           return "PF";
   case X86Register::AF:           return "AF";
   case X86Register::ZF:           return "ZF";
   case X86Register::SF:           return "SF";
   case X86Register::TF:           return "TF";
   case X86Register::IF:           return "IF";
   case X86Register::DF:           return "DF";
   case X86Register::OF:           return "OF";

   case X86Register::IP:           return "IP";
   case X86Register::EIP:          return "EIP";

   case X86Register::CS:           return "CS";
   case X86Register::DS:           return "DS";
   case X86Register::SS:           return "SS";
   case X86Register::ES:           return "ES";
   case X86Register::FS:           return "FS";
   case X86Register::GS:           return "GS";

   case X86Register::TR0:          return "TR0";
   case X86Register::TR1:          return "TR1";
   case X86Register::TR2:          return "TR2";
   case X86Register::TR3:          return "TR3";
   case X86Register::TR4:          return "TR4";
   case X86Register::TR5:          return "TR5";
   case X86Register::TR6:          return "TR6";
   case X86Register::TR7:          return "TR7";

   case X86Register::DR0:          return "DR0";
   case X86Register::DR1:          return "DR1";
   case X86Register::DR2:          return "DR2";
   case X86Register::DR3:          return "DR3";
   case X86Register::DR4:          return "DR4";
   case X86Register::DR5:          return "DR5";
   case X86Register::DR6:          return "DR6";
   case X86Register::DR7:          return "DR7";

   case X86Register::CR0:          return "CR0";
   case X86Register::CR1:          return "CR1";
   case X86Register::CR2:          return "CR2";
   case X86Register::CR3:          return "CR3";
   case X86Register::CR4:          return "CR4";
   case X86Register::CR5:          return "CR5";
   case X86Register::CR6:          return "CR6";
   case X86Register::CR7:          return "CR7";

   case X86Register::MM0:          return "MM0";
   case X86Register::MM1:          return "MM1";
   case X86Register::MM2:          return "MM2";
   case X86Register::MM3:          return "MM3";
   case X86Register::MM4:          return "MM4";
   case X86Register::MM5:          return "MM5";
   case X86Register::MM6:          return "MM6";
   case X86Register::MM7:          return "MM7";

   case X86Register::XMM0:         return "XMM0";
   case X86Register::XMM1:         return "XMM1";
   case X86Register::XMM2:         return "XMM2";
   case X86Register::XMM3:         return "XMM3";
   case X86Register::XMM4:         return "XMM4";
   case X86Register::XMM5:         return "XMM5";
   case X86Register::XMM6:         return "XMM6";
   case X86Register::XMM7:         return "XMM7";
   
   default:
         return "?";
   }
};


void PrintSetRegs( CBmp *pBmp )
{
   size_t iBit = ibitNil;
   while (ibitNil != (iBit = pBmp->IbitNextSet( iBit )))
   {
      printf(" %s", ConvertX86RegToStr( (ERegister)iBit ));
   }
}

int __cdecl main(int argc, char** argv)
{

   if (argc != 2)
   {
      printf("usage: RegsUseDef foo.exe\n");
      return 1;
   }

   try
   {
      UIRegs(argv[1]);
   }
   catch (VErr& verr) 
   {
      printf("Error: %s\n",verr.GetWhat());
      return 1;
   }
   catch (...)
   {
      printf("Error: unknown\n");
      return 1; 
   }

   return 0;
}
