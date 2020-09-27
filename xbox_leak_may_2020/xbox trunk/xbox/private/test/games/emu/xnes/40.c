#include "mapinc.h"



byte Mapper40_WRAM(word A)
{
 return Page[A>>13][A];
}

void Mapper40_write(word A, byte V)
{
switch(A&0xe000)
 {
  case 0x8000:IRQa=0;IRQCount=0;break;
  case 0xa000:IRQa=1;break;
  case 0xe000:ROM_BANK8(0xc000,V&7);break;
 }
}
static void Mapper40_hb(void)
{
 if(IRQa)
 {
        if(IRQCount<100) IRQCount++;
        if(IRQCount==36)
        {
        IRQa=0;
        TriggerIRQ();
        }
 }
}

void Mapper40_init(void)
{
  ROM_BANK8(0x6000,(pmask8-1));
  ROM_BANK8(0x8000,(pmask8-3));
  ROM_BANK8(0xa000,(pmask8-2));
  SetWriteHandler(0x8000,0xffff,(void *)Mapper40_write);
  SetReadHandler(0x6000,0x7fff,(void *)Mapper40_WRAM);
  MapHBIRQHook=(void *)Mapper40_hb;
}

