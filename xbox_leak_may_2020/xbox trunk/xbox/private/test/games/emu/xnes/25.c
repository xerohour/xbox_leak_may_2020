#include "mapinc.h"

#define K4buf mapbyte2
#define K4IRQ mapbyte1[1]
#define K4sel mapbyte1[0]

void Mapper25_write(uint16 A,uint8 V)
{

if((A&0xF000)==0xA000)
  ROM_BANK8(0xA000,V);
else if((A&0xF000)==0x8000)
 {
        if(K4sel&2)
         ROM_BANK8(0xC000,V);
        else
         ROM_BANK8(0x8000,V);
 }
else switch(A&0xF00F)
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
 case 0x9004:
 case 0x9001:if((K4sel&2)!=(V&2))
             {
             byte swa;
             swa=PRGBankList[0];
             ROM_BANK8(0x8000,PRGBankList[2]);
             ROM_BANK8(0xc000,swa);
             }
             K4sel=V;
             break;
case 0xb000:K4buf[0]&=0xF0;K4buf[0]|=V&0x0f;VROM_BANK1(0x0,K4buf[0]);break;
case 0xb008:
case 0xb002:K4buf[0]&=0x0F;K4buf[0]=K4buf[0]|(V<<4);VROM_BANK1(0x0,K4buf[0]);
break;
case 0xb004:
case 0xb001:K4buf[1]&=0xF0;K4buf[1]|=V&0x0f;VROM_BANK1(0x400,K4buf[1]);break;
case 0xb00c:
case 0xb003:K4buf[1]&=0x0F;K4buf[1]=K4buf[1]|(V<<4);VROM_BANK1(0x400,K4buf[1]);
break;
case 0xc000:K4buf[2]&=0xF0;K4buf[2]|=V&0x0f;VROM_BANK1(0x800,K4buf[2]);break;
case 0xc008:
case 0xc002:K4buf[2]&=0x0F;K4buf[2]=K4buf[2]|(V<<4);VROM_BANK1(0x800,K4buf[2]);
break;
case 0xc004:
case 0xc001:K4buf[3]&=0xF0;K4buf[3]|=V&0x0f;VROM_BANK1(0xc00,K4buf[3]);break;
case 0xc00c:
case 0xc003:K4buf[3]&=0x0F;K4buf[3]=K4buf[3]|(V<<4);VROM_BANK1(0xc00,K4buf[3]);
break;
case 0xd000:K4buf[4]&=0xF0;K4buf[4]|=V&0x0f;VROM_BANK1(0x1000,K4buf[4]);break;
case 0xd008:
case 0xd002:K4buf[4]&=0x0F;K4buf[4]=K4buf[4]|(V<<4);VROM_BANK1(0x1000,K4buf[4]);break;
case 0xd004:
case 0xd001:K4buf[5]&=0xF0;K4buf[5]|=V&0x0f;VROM_BANK1(0x1400,K4buf[5]);break;
case 0xd00c:
case 0xd003:K4buf[5]&=0x0F;K4buf[5]=K4buf[5]|(V<<4);VROM_BANK1(0x1400,K4buf[5]);break;

case 0xe000:K4buf[6]&=0xF0;K4buf[6]|=V&0x0f;VROM_BANK1(0x1800,K4buf[6]);break;
case 0xe008:
case 0xe002:K4buf[6]&=0x0F;K4buf[6]=K4buf[6]|(V<<4);VROM_BANK1(0x1800,K4buf[6]);break;
case 0xe004:
case 0xe001:K4buf[7]&=0xF0;K4buf[7]|=V&0x0f;VROM_BANK1(0x1c00,K4buf[7]);break;
case 0xe00c:
case 0xe003:K4buf[7]&=0x0F;K4buf[7]=K4buf[7]|(V<<4);VROM_BANK1(0x1c00,K4buf[7]);break;
case 0xf000:IRQLatch&=0xF0;IRQLatch|=V&0xF;break;
case 0xf008:
case 0xf002:IRQLatch&=0x0F;IRQLatch|=V<<4;break;
case 0xf004:
case 0xf001:IRQCount=IRQLatch;IRQa=V&2;K4IRQ=V&1;break;
case 0xf00c:
case 0xf003:IRQa=K4IRQ;break;
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

void Mapper25_init(void)
{
        SetWriteHandler(0x8000,0xffff,(void *)Mapper25_write);
        MapIRQHook=(void *)KonamiIRQHook;
}

