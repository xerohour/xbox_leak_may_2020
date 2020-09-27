#include "mapinc.h"


#define FVRAM_BANK8(A,V) {VPage[0]=VPage[1]=VPage[2]=VPage[3]=VPage[4]=VPage[5]=VPage[6]=VPage[7]=V?&MapperExRAM[(V)<<13]-(A):&VRAM[(V)<<13]-(A);CHRBankList[0]=((V)<<3);CHRBankList[1]=((V)<<3)+1;CHRBankList[2]=((V)<<3)+2;CHRBankList[3]=((V)<<3)+3;CHRBankList[4]=((V)<<3)+4;CHRBankList[5]=((V)<<3)+5;CHRBankList[6]=((V)<<3)+6;CHRBankList[7]=((V)<<3)+7;VPAL[0]=VPAL[1]=VPAL[2]=VPAL[3]=VPAL[4]=VPAL[5]=VPAL[6]=VPAL[7]=1;}

static void FFEIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount+=a;
   if(IRQCount>=0x10000)
   {
    TriggerIRQ();
    IRQa=0;
    IRQCount=0;
   }
  }
}

void Mapper6_write(uint16 A,uint8 V)
{
        if(A<0x8000)
        {
                switch(A){
                case 0x42FF:MIRROR_SET((V>>4)&1);break;
                case 0x42FE:onemir((V>>3)&2);break;
                case 0x4501:IRQa=0;break;
                case 0x4502:IRQCount&=0xFF00;IRQCount|=V;break;
                case 0x4503:IRQCount&=0xFF;IRQCount|=V<<8;IRQa=1;break;
                }
        } else {
        ROM_BANK16(0x8000,(V>>2)&pmask16);
        FVRAM_BANK8(0x0000,V&3);
        }
}
void Mapper6_StateRestore(int version)
{
 int x;
 for(x=0;x<8;x++)
  if(VPAL[x])
  {
   if(CHRBankList[x]>7)
    VPage[x]=&MapperExRAM[(CHRBankList[x]&31)*0x400]-(x*0x400);
   else VPage[x]=&VRAM[(CHRBankList[x]&7)*0x400]-(x*0x400);
  }
}
void Mapper6_init(void)
{
MapIRQHook=FFEIRQHook;
ROM_BANK16(0xc000,7);

SetWriteHandler(0x4020,0x5fff,(void *)Mapper6_write);
SetWriteHandler(0x8000,0xffff,(void *)Mapper6_write);
MapStateRestore=(void *)Mapper6_StateRestore;
}

