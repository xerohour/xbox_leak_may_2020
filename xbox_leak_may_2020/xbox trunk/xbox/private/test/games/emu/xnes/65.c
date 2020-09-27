#include "mapinc.h"


void IREMIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount-=a;
   if(IRQCount<=0-4)
   {
    TriggerIRQ();
    IRQa=0;
    IRQCount=0xFFFF;
   }
  }
}

void Mapper65_write(uint16 A,uint8 V)
{
switch(A)
{
case 0x8000:ROM_BANK8(0x8000,V);break;
case 0x9000:MIRROR_SET2((V>>6)&1);break;
case 0x9003:IRQa=V&0x80;break;
case 0x9004:IRQCount=IRQLatch;break;
case 0x9005:          IRQLatch&=0x00FF;
                      IRQLatch|=V<<8;
                      break;
case 0x9006:          IRQLatch&=0xFF00;IRQLatch|=V;
                      break;
case 0xB000:VROM_BANK1(0x0000,V);break;
case 0xB001:VROM_BANK1(0x0400,V);break;
case 0xB002:VROM_BANK1(0x0800,V);break;
case 0xB003:VROM_BANK1(0x0C00,V);break;
case 0xB004:VROM_BANK1(0x1000,V);break;
case 0xB005:VROM_BANK1(0x1400,V);break;
case 0xB006:VROM_BANK1(0x1800,V);break;
case 0xB007:VROM_BANK1(0x1C00,V);break;
case 0xa000:ROM_BANK8(0xA000,V);break;
case 0xC000:ROM_BANK8(0xC000,V);break;
 }
}

void Mapper65_init(void)
{
 MapIRQHook=(void *)IREMIRQHook;
 SetWriteHandler(0x8000,0xffff,(void *)Mapper65_write);
}
