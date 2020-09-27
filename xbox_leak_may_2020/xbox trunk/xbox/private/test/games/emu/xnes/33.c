#include "mapinc.h"


void Mapper33_write(uint16 A,uint8 V)
{
        switch(A){
        case 0x8000:if(!mapbyte1[0])
                     MIRROR_SET((V>>6)&1);
                     ROM_BANK8(0x8000,V);
                    break;
        case 0x8001:ROM_BANK8(0xA000,V); break;
        case 0x8002:VROM_BANK2(0x0000,V);break;
        case 0x8003:VROM_BANK2(0x0800,V);break;
        case 0xA000:VROM_BANK1(0x1000,V); break;
        case 0xA001:VROM_BANK1(0x1400,V); break;
        case 0xA002:VROM_BANK1(0x1800,V); break;
        case 0xA003:VROM_BANK1(0x1C00,V); break;
	case 0xc000:IRQCount=V;break;
	case 0xc001:IRQa=V&1;break;
        case 0xe000:mapbyte1[0]=1;MIRROR_SET((V>>6)&1);break;
        }
}

static void heho(void)
{
if(IRQa)
 {
  if(scanline<=240 && (ScreenON || SpriteON))
  {
   IRQCount++;
   if(IRQCount==0x100) {TriggerIRQ();IRQa=0;}
  }
 }
}
void Mapper33_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper33_write);
MapHBIRQHook=(void *)heho;
}
