#include "mapinc.h"


static __inline void VRAM_BANK4(uint32 A,uint32 V)
{
VPage[(A)>>10]=VPage[((A)>>10)+1]=VPage[((A)>>10)+2]=VPage[((A)>>10)+3]=&MapperExRAM[(V)<<12]-(A);
CHRBankList[(A)>>10]=((V)<<2);
CHRBankList[((A)>>10)+1]=((V)<<2)+1;
CHRBankList[((A)>>10)+2]=((V)<<2)+2;CHRBankList[((A)>>10)+3]=((V)<<2)+3;
}



void Mapper13_write(uint16 A,uint8 V)
{
VRAM_BANK4(0x1000,V&3);
ROM_BANK32(0x8000,(((V>>4)&3)));
}
void Mapper13_StateRestore(int version)
{
int x;
 for(x=0;x<8;x++)
  if(VPAL[x])
   VPage[x]=&MapperExRAM[(CHRBankList[x]&15)*0x400]-(x*0x400);
}
void Mapper13_init(void)
{
VRAM_BANK4(0,0);VRAM_BANK4(0x1000,1);
SetWriteHandler(0x8000,0xFFFF,(void *)Mapper13_write);
MapStateRestore=(void *)Mapper13_StateRestore;
}

