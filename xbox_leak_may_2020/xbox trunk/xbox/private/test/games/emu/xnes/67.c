#include "mapinc.h"


#define suntoggle mapbyte1[0]

void Mapper67_write(uint16 A,uint8 V)
{
switch(A&0xF800)
 {
//case 0x8000:V&=vmask1;VROM_BANK1(0x000,(V));break;
case 0x8800:VROM_BANK1(0x000,(V<<1));VROM_BANK1(0x400,(V<<1)+1);break;
case 0x9800:VROM_BANK1(0x800,(V<<1));VROM_BANK1(0xc00,(V<<1)+1);break;
case 0xa800:VROM_BANK1(0x1000,(V<<1));VROM_BANK1(0x1400,(V<<1)+1);break;
case 0xb800:VROM_BANK1(0x1800,(V<<1));VROM_BANK1(0x1c00,(V<<1)+1);break;
case 0xc800:
case 0xc000:if(!suntoggle)
            {IRQCount&=0xFF;IRQCount|=V<<8;}
            else{IRQCount&=0xFF00;IRQCount|=V;}
            suntoggle^=1;
            break;
case 0xd800:suntoggle=0;IRQa=V&0x10;break;

case 0xe800:    switch(V&3)
                {
                case 0:MIRROR_SET2(1);break;
                case 1:MIRROR_SET2(0);break;
                case 2:onemir(0);break;
                case 3:onemir(2);break;
                }
                break;
case 0xf800:V&=pmask16;ROM_BANK16(0x8000,V);break;
 }
}
static void SunIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount-=a;
   if(IRQCount<=0)
   {TriggerIRQ();IRQa=0;IRQCount=0xFFFF;}
  }
}
void Mapper67_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper67_write);
MapIRQHook=(void *)SunIRQHook;
}
