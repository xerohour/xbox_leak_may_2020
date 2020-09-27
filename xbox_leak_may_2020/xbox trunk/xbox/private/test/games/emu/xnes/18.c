#include "mapinc.h"

#define K4buf mapbyte2
#define K4buf2 mapbyte3


void JalecoIRQHook(int a)
{
  if(IRQa)
  {
   IRQCount-=a;
   if(IRQCount<=-8)
   {
    TriggerIRQ();
    IRQCount=IRQLatch;
   }
  }
}

void Mapper18_write(uint16 A,uint8 V)
{
        switch(A)
        {
case 0x8000:K4buf2[0]&=0xF0;K4buf2[0]|=V&0x0f;ROM_BANK8(0x8000,K4buf2[0]);break;
case 0x8001:K4buf2[0]&=0x0F;K4buf2[0]|=(V<<4);ROM_BANK8(0x8000,K4buf2[0]);break;
case 0x8002:K4buf2[1]&=0xF0;K4buf2[1]|=V&0x0f;ROM_BANK8(0xa000,K4buf2[1]);break;
case 0x8003:K4buf2[1]&=0x0F;K4buf2[1]|=(V<<4);ROM_BANK8(0xa000,K4buf2[1]);break;
case 0x9000:K4buf2[2]&=0xF0;K4buf2[2]|=V&0x0f;ROM_BANK8(0xc000,K4buf2[2]);break;
case 0x9001:K4buf2[2]&=0x0F;K4buf2[2]|=(V<<4);ROM_BANK8(0xc000,K4buf2[2]);break;
case 0xa000:K4buf[0]&=0xF0;K4buf[0]|=V&0x0f;VROM_BANK1(0x0,K4buf[0]);break;
case 0xa001:K4buf[0]&=0x0F;K4buf[0]=K4buf[0]|(V<<4);VROM_BANK1(0x0,K4buf[0]);break;
case 0xa002:K4buf[1]&=0xF0;K4buf[1]|=V&0x0f;VROM_BANK1(0x400,K4buf[1]);break;
case 0xa003:K4buf[1]&=0x0F;K4buf[1]=K4buf[1]|(V<<4);VROM_BANK1(0x400,K4buf[1]);break;
case 0xb000:K4buf[2]&=0xF0;K4buf[2]|=V&0x0f;VROM_BANK1(0x800,K4buf[2]);break;
case 0xb001:K4buf[2]&=0x0F;K4buf[2]=K4buf[2]|(V<<4);VROM_BANK1(0x800,K4buf[2]);break;
case 0xb002:K4buf[3]&=0xF0;K4buf[3]|=V&0x0f;VROM_BANK1(0xc00,K4buf[3]);break;
case 0xb003:K4buf[3]&=0x0F;K4buf[3]=K4buf[3]|(V<<4);VROM_BANK1(0xc00,K4buf[3]);break;
case 0xc000:K4buf[4]&=0xF0;K4buf[4]|=V&0x0f;VROM_BANK1(0x1000,K4buf[4]);break;
case 0xc001:K4buf[4]&=0x0F;K4buf[4]=K4buf[4]|(V<<4);VROM_BANK1(0x1000,K4buf[4]);break;
case 0xc002:K4buf[5]&=0xF0;K4buf[5]|=V&0x0f;VROM_BANK1(0x1400,K4buf[5]);break;
case 0xc003:K4buf[5]&=0x0F;K4buf[5]=K4buf[5]|(V<<4);VROM_BANK1(0x1400,K4buf[5]);break;
case 0xd000:K4buf[6]&=0xF0;K4buf[6]|=V&0x0f;VROM_BANK1(0x1800,K4buf[6]);break;
case 0xd001:K4buf[6]&=0x0F;K4buf[6]=K4buf[6]|(V<<4);VROM_BANK1(0x1800,K4buf[6]);break;
case 0xd002:K4buf[7]&=0xF0;K4buf[7]|=V&0x0f;VROM_BANK1(0x1c00,K4buf[7]);break;
case 0xd003:K4buf[7]&=0x0F;K4buf[7]=K4buf[7]|(V<<4);VROM_BANK1(0x1c00,K4buf[7]);break;

case 0xe000:IRQLatch&=0xF0;IRQLatch|=V&0x0f;break;
case 0xe001:IRQLatch&=0x0F;IRQLatch|=V<<4;break;
case 0xe002:IRQLatch&=0xF0FF;IRQLatch|=(V&0x0f)<<8;break;
case 0xe003:IRQLatch&=0x0FFF;IRQLatch|=V<<12;break;

case 0xf000:if(V&1)IRQCount=IRQLatch;break;
case 0xf001://IRQCount=IRQLatch;
            IRQa=V&1;break;

case 0xf002:MIRROR_SET2(V&1);
            if(V&2) onemir(0);
            break;
        }
}

void Mapper18_init(void)
{
SetWriteHandler(0x8000,0xffff,(void *)Mapper18_write);
MapIRQHook=(void *)JalecoIRQHook;
}
