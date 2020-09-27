#include "mapinc.h"


byte Mapper42_WRAM(word A)
{
 return Page[A>>13][A];
}

void Mapper42_write(word A, byte V)
{
switch(A&0xe003)
 {
  case 0xe000:mapbyte1[0]=V;ROM_BANK8(0x6000,V&0xF);break;
  case 0xe001:MIRROR_SET((V>>3)&1);break;
  case 0xe002:IRQa=V&2;if(!IRQa) IRQCount=0;break;
 }
}
static void Mapper42_hb(void)
{
 if(IRQa)
 {
        if(IRQCount<215) IRQCount++;
        if(IRQCount==215)
        {
        IRQa=0;
        TriggerIRQ();
        }
 }
}
void Mapper42_StateRestore(int version)
{
    ROM_BANK8(0x6000,mapbyte1[0]&0xF);
}


void Mapper42_init(void)
{
  ROM_BANK8(0x6000,0);
  ROM_BANK32(0x8000,pmask32);
  SetWriteHandler(0xe000,0xffff,(void *)Mapper42_write);
  SetReadHandler(0x6000,0x7fff,(void *)Mapper42_WRAM);
  MapHBIRQHook=(void *)Mapper42_hb;
  MapStateRestore=(void *)Mapper42_StateRestore;
}

