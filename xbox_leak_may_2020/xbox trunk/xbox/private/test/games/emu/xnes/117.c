#include "mapinc.h"



void Mapper117_write(uint16 A,uint8 V)
{
 switch(A)
 {
  case 0xc003:IRQCount=V;break;
  case 0xc001:IRQa=V;break;
  case 0xa000:VROM_BANK1(0x0000,V);break;
  case 0xa001:VROM_BANK1(0x0400,V);break;
  case 0xa002:VROM_BANK1(0x0800,V);break;
  case 0xa003:VROM_BANK1(0x0c00,V);break;
  case 0xa004:VROM_BANK1(0x1000,V);break;
  case 0xa005:VROM_BANK1(0x1400,V);break;
  case 0xa006:VROM_BANK1(0x1800,V);break;
  case 0xa007:VROM_BANK1(0x1c00,V);break;
  case 0x8000:ROM_BANK8(0x8000,V);break;
  case 0x8001:ROM_BANK8(0xa000,V);break;
  case 0x8002:ROM_BANK8(0xc000,V);break;
  case 0x8003:ROM_BANK8(0xe000,V);break;
 }
}

static void Mapper117_hb(void)
{
 if(IRQa)
 {
        if(IRQCount<=0)
        {
         IRQa=0;
         TriggerIRQ();
        }
        else
        {
         if(scanline<240 && (ScreenON || SpriteON)) IRQCount--;
        }
 }
}

void Mapper117_init(void)
{
  MapHBIRQHook=(void *)Mapper117_hb;
  SetWriteHandler(0x8000,0xffff,(void *)Mapper117_write);
}

