#include "mapinc.h"



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


void Mapper17_write(uint16 A,uint8 V)
{
        switch(A){
        default:
              break;
        case 0x42FE:
                   onemir((V>>3)&2);
                   break;
        case 0x42FF:
                   MIRROR_SET((V>>4)&1);
        break;
        case 0x4501:IRQa=V;break;
        case 0x4502:IRQCount&=0xFF00;IRQCount|=V;break;
        case 0x4503:IRQCount&=0x00FF;IRQCount|=V<<8;IRQa=1;break;
        case 0x4504: ROM_BANK8(0x8000,V);break;
        case 0x4505: ROM_BANK8(0xA000,V);break;
        case 0x4506: ROM_BANK8(0xC000,V);break;
        case 0x4507: ROM_BANK8(0xE000,V);break;
        case 0x4510: VROM_BANK1(0x0000,V);break;
        case 0x4511: VROM_BANK1(0x0400,V);break;
        case 0x4512: VROM_BANK1(0x0800,V);break;
        case 0x4513: VROM_BANK1(0x0C00,V);break;
        case 0x4514: VROM_BANK1(0x1000,V);break;
        case 0x4515: VROM_BANK1(0x1400,V);break;
        case 0x4516: VROM_BANK1(0x1800,V);break;
        case 0x4517: VROM_BANK1(0x1C00,V);break;
        }
}

void Mapper17_init(void)
{
MapIRQHook=(void *)FFEIRQHook;
SetWriteHandler(0x4020,0x5fff,(void *)Mapper17_write);
}
