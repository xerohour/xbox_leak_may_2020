#include "mapinc.h"

#define K4buf mapbyte2
#define K4IRQ mapbyte1[1]
#define K4sel mapbyte1[0]


void Mapper21_write(uint16 A,uint8 V)
{
A|=((A>>5)&0xF);

if((A&0xF000)==0xA000)
  ROM_BANK8(0xA000,V);
else if((A&0xF000)==0x8000)
 {
        if(K4sel&2)
         ROM_BANK8(0xC000,V);
        else
         ROM_BANK8(0x8000,V);
 }
else switch(A&0xF006)
 {
 case 0x9000:
             switch(V&0x3)
             {
             case 0:MIRROR_SET(0);break;
             case 1:MIRROR_SET(1);break;
             case 2:onemir(0);break;
             case 3:onemir(2);break;
             }
             break;
 case 0x9006:
 case 0x9004:
 case 0x9002:if((K4sel&2)!=(V&2))
             {
             byte swa;
             swa=PRGBankList[0];
             ROM_BANK8(0x8000,PRGBankList[2]);
             ROM_BANK8(0xc000,swa);
             }
             K4sel=V;
             break;
   case 0xb000:K4buf[0]&=0xF0;K4buf[0]|=V&0x0F;VROM_BANK1(0x000,(K4buf[0]));break;
   case 0xb002:K4buf[0]&=0x0F;K4buf[0]|=V<<4;VROM_BANK1(0x000,(K4buf[0]));break;

   case 0xb004:K4buf[1]&=0xF0;K4buf[1]|=V&0x0F;VROM_BANK1(0x400,(K4buf[1]));break;
   case 0xb006:K4buf[1]&=0x0F;K4buf[1]|=V<<4;VROM_BANK1(0x400,(K4buf[1]));break;

   case 0xc000:K4buf[2]&=0xF0;K4buf[2]|=V&0x0F;VROM_BANK1(0x800,(K4buf[2]));break;
   case 0xc002:K4buf[2]&=0x0F;K4buf[2]|=V<<4;VROM_BANK1(0x800,(K4buf[2]));break;

   case 0xc004:K4buf[3]&=0xF0;K4buf[3]|=V&0x0F;VROM_BANK1(0xc00,(K4buf[3]));break;
   case 0xc006:K4buf[3]&=0x0F;K4buf[3]|=V<<4;VROM_BANK1(0xc00,(K4buf[3]));break;

   case 0xd000:K4buf[4]&=0xF0;K4buf[4]|=V&0x0F;VROM_BANK1(0x1000,(K4buf[4]));break;
   case 0xd002:K4buf[4]&=0x0F;K4buf[4]|=V<<4;VROM_BANK1(0x1000,(K4buf[4]));break;

   case 0xd004:K4buf[5]&=0xF0;K4buf[5]|=V&0x0F;VROM_BANK1(0x1400,(K4buf[5]));break;
   case 0xd006:K4buf[5]&=0x0F;K4buf[5]|=V<<4;VROM_BANK1(0x1400,(K4buf[5]));break;

   case 0xe000:K4buf[6]&=0xF0;K4buf[6]|=V&0x0F;VROM_BANK1(0x1800,(K4buf[6]));break;
   case 0xe002:K4buf[6]&=0x0F;K4buf[6]|=V<<4;VROM_BANK1(0x1800,(K4buf[6]));break;

   case 0xe004:K4buf[7]&=0xF0;K4buf[7]|=V&0x0F;VROM_BANK1(0x1c00,(K4buf[7]));break;
   case 0xe006:K4buf[7]&=0x0F;K4buf[7]|=V<<4;VROM_BANK1(0x1c00,(K4buf[7]));break;
   case 0xf000:IRQLatch&=0xF0;IRQLatch|=V&0xF;break;
   case 0xf002:IRQLatch&=0x0F;IRQLatch|=V<<4;break;
   case 0xf004:IRQCount=IRQLatch;
               IRQa=V&2;K4IRQ=V&1;break;
   case 0xf006:IRQa=K4IRQ;break;
 }
}
static void KonamiIRQHook(int a)
{
  static int count=0;
  if(IRQa)
   {
    count+=(a<<1)+a;
    if(count>=342)
    {
     doagainbub:count-=342;IRQCount++;
     if(IRQCount&0x100) {count=0;TriggerIRQ();IRQCount=IRQLatch;}
     if(count>=342) goto doagainbub;
    }
 }
}

void Mapper21_init(void)
{
        SetWriteHandler(0x8000,0xffff,(void *)Mapper21_write);
        MapIRQHook=(void *)KonamiIRQHook;
}
